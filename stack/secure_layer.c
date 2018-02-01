/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      secure_layer.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/        

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined USE_SECURE_SOCKET_LAYER

// To include in standard header
//
extern int fnInitialiseSecureLayer(const unsigned char *ptrOurCertificate, unsigned long ulCertificateLength, const unsigned char *ptrOutPrivateKey, unsigned long ulOurPrivateKeyLength);
extern void fnHandshakeStats(unsigned long ulHandshakeSize, unsigned char *ucPrtData);
extern void fnEnterRandom(unsigned char *ucPrtData, int iServer);
extern void fnSetSessionCipher(unsigned short session_cipher, unsigned char ucVersion[2], unsigned char ucIdLength, unsigned char *ptrID);
extern int fnHandleSecurityExtension(unsigned short ext_id, unsigned short ext_size, unsigned char *ptrExtensionData);
extern unsigned char *fnInsertTLS_random(unsigned char *ptrData, size_t Length);
extern void fnSwitchTransformSpec(void);
extern int fnDecrypt(unsigned char **ptrptrInput, unsigned long *ptr_ulLength);
extern unsigned char *fnEncrypt(unsigned char *ptrInput, unsigned char *ptrInputData, unsigned long ulLength);
extern void fnTearDown(void);


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define HANDSHAKE_EXTENSION_SERVER_NAME            0
#define HANDSHAKE_EXTENSION_SUPPORTED_GROUPS       10
#define HANDSHAKE_EXTENSION_EC_POINT_FORMATS       11
#define HANDSHAKE_EXTENSION_SIGNATURE_ALGORITHMS   13
#define HANDSHAKE_EXTENSION_ENCRYPT_THEN_MAC       22
#define HANDSHAKE_EXTENSION_EXTENDED_MASTER_SECRET 23

#define SIGNATURE_HASH_ALGORITHM_HASH_SHA224        3
#define SIGNATURE_HASH_ALGORITHM_HASH_SHA256        4

#define SIGNATURE_HASH_ALGORITHM_SIGNATURE_RSA      1
#define SIGNATURE_HASH_ALGORITHM_SIGNATURE_ECDSA    3

#define GROUP_SECP245R1                             0x0017

#define EC_POINT_FORMAT_UNCOMPRESSED                0

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stSIGNATURE_HASH_ALGORITHM_ENTRY {
    unsigned char ucHash;
    unsigned char ucSignature;
} SIGNATURE_HASH_ALGORITHM_ENTRY;


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int  fnHandleHandshake(USOCKET Socket, unsigned char *ucPrtData, unsigned long ulHandshakeSize, unsigned char ucPresentHandshakeType);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

// The cipher suites that we support
//
static const unsigned short cusCipherSuites[] = {
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
    TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
    TLS_EMPTY_RENEGOTIATION_INFO_SCSV,
};

// The handshake extensions that we send
//
static const unsigned short cusHandshakeExtensions[] = {
    HANDSHAKE_EXTENSION_SERVER_NAME,
    HANDSHAKE_EXTENSION_SIGNATURE_ALGORITHMS,
    HANDSHAKE_EXTENSION_SUPPORTED_GROUPS,
    HANDSHAKE_EXTENSION_EC_POINT_FORMATS,
    HANDSHAKE_EXTENSION_ENCRYPT_THEN_MAC,
    HANDSHAKE_EXTENSION_EXTENDED_MASTER_SECRET
};

#define MAX_HANDSHAKE_EXTENSIONS     (sizeof(cusHandshakeExtensions)/sizeof(unsigned short))

// The hash algorithms that we support
//
static const SIGNATURE_HASH_ALGORITHM_ENTRY cucSignatureAlgorithms[] = {
    { SIGNATURE_HASH_ALGORITHM_HASH_SHA256,          SIGNATURE_HASH_ALGORITHM_SIGNATURE_ECDSA },
    { SIGNATURE_HASH_ALGORITHM_HASH_SHA256,          SIGNATURE_HASH_ALGORITHM_SIGNATURE_RSA },
    { SIGNATURE_HASH_ALGORITHM_HASH_SHA224,          SIGNATURE_HASH_ALGORITHM_SIGNATURE_ECDSA },
    { SIGNATURE_HASH_ALGORITHM_HASH_SHA224,          SIGNATURE_HASH_ALGORITHM_SIGNATURE_RSA },
};

#define MAX_SIGNATURE_HASH_ALGORITHMS (sizeof(cucSignatureAlgorithms)/sizeof(SIGNATURE_HASH_ALGORITHM_ENTRY))

// The groups that we support
//
static const unsigned short cusGroups[] = {
    GROUP_SECP245R1
};

#define MAX_GROUPS (sizeof(cusGroups)/sizeof(unsigned short))

static const unsigned char cucEC_pointFormats[] = {
    EC_POINT_FORMAT_UNCOMPRESSED
};

#define MAX_GROUPS (sizeof(cusGroups)/sizeof(unsigned short))

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */
#define TLS_TX_STATE_IDLE                     0

#define TLS_RX_STATE_IDLE                     0
#define TLS_RX_STATE_VERSION_MSB              1
#define TLS_RX_STATE_VERSION_LSB              2
#define TLS_RX_STATE_LENGTH_MSB               3
#define TLS_RX_STATE_LENGTH_LSB               4
#define TLS_RX_STATE_PROTOCOL_TYPE            5
#define TLS_RX_STATE_TYPE_LENGTH_HSB          6
#define TLS_RX_STATE_TYPE_LENGTH_MSB          7
#define TLS_RX_STATE_TYPE_LENGTH_LSB          8
#define TLS_RX_STATE_CONTENT                  9
#define TLS_RX_STATE_CIPHER_SPEC_MESSAGE      10
#define TLS_RX_STATE_ENCRYPTED_RECORD_CONTENT 11
#define TLS_RX_STATE_ENCRYPTED_APP_CONTENT    12
#define TLS_RX_STATE_ENCRYPTED_ALERT_CONTENT  13

static int iTLS_tx_state = TLS_TX_STATE_IDLE;
static int iTLS_rx_state = TLS_RX_STATE_IDLE;
static int iTLS_rx_type = 0;
static int iTLS_rx_encryted = 0;
static unsigned short usTLS_rx_version = 0;
static unsigned short session_cipher = 0;
static unsigned char *ptrReceptionBuffer = 0;
static unsigned long ulBufferContent = 0;




// Temporary
//
#define TCP_TLS_CERTIFICATES            20                               // to extend the TCP event defines - after TCP_WINDOW_PROBE
#define TCP_TLS_CLIENT_KEY_EXCHANGE     21
#define TCP_TLS_CERTIFICATE_VERIFY      22
#define TCP_TLS_CHANGE_CIPHER_SPEC      23
#define TCP_TLS_ENCRYPTED_HANDSHAKE     24

extern unsigned char *fnGeneratePublicKey(unsigned char *ptrData);       // in uTaskerInterface.c
extern unsigned char *fnInsertSignatureAlgorithm(unsigned char *ptrData);
extern int fnSaveServerCertificate(unsigned char *ptrCertificate, unsigned long ulCertificateLength);
extern int fnExtractPublicKey(unsigned char *ptrData, unsigned long ulKeyExchangeLength);
extern int fnPrepareCertificate(unsigned char *ptrData, unsigned short usSignatureHashAlgorithmsLength);
extern unsigned char *fnFinished(unsigned char *ptrData);

void * (*mbedtls_calloc)(size_t, size_t) = 0;
void(*mbedtls_free)(void *) = 0;

extern int mbedtls_snprintf(char * s, size_t n, const char * format, ...)
{
    _EXCEPTION("Do we need this?");
    return 0;
}



static unsigned char *fnInsertCipherSuites(unsigned char *ptrData)
{
    int i;
    unsigned short usCipherSuite;
    unsigned short usLength = sizeof(cusCipherSuites);                   // cipher suites length
    *ptrData++ = (unsigned char)(usLength >> 8);
    *ptrData++ = (unsigned char)(usLength);
    for (i = 0; i < (sizeof(cusCipherSuites)/sizeof(unsigned short)); i++) {
        usCipherSuite = cusCipherSuites[i];
        *ptrData++ = (unsigned char)(usCipherSuite >> 8);
        *ptrData++ = (unsigned char)(usCipherSuite);
    }
    return ptrData;
}



static unsigned char *fnInsertSessionID(unsigned char *ptrData, unsigned char ucLength)
{
    int i;
    unsigned short usRandom;
    if ((ucLength != 16) && (ucLength != 32)) {
        ucLength = 0;
    }
    *ptrData++ = ucLength;
    for (i = 0; i < ucLength; i += sizeof(unsigned short)) {
        usRandom = fnGetRndHW();
        *ptrData++ = (unsigned char)(usRandom >> 8);
        *ptrData++ = (unsigned char)(usRandom);
    }
    return ptrData;
}

static unsigned char *fnInsertServerNameIndication(unsigned char *ptrData, const CHAR *cServer, unsigned char ucServerNameType)
{
    unsigned char *ptrLength = ptrData;
    unsigned short usLength;
    ptrData += 5;
    ptrData = (unsigned char *)uStrcpy((CHAR *)ptrData, cServer);        // insert the server name string
    usLength = (ptrData - ptrLength - 2);                                // server name list length
    *ptrLength++ = (unsigned char)(usLength >> 8);
    *ptrLength++ = (unsigned char)(usLength);
    *ptrLength++ = ucServerNameType;                                     // server name type
    usLength -= 3;                                                       // server name length
    *ptrLength++ = (unsigned char)(usLength >> 8);
    *ptrLength++ = (unsigned char)(usLength);
    return ptrData;
}

static unsigned char *fnInsertSigatureHashAlgorithms(unsigned char *ptrData)
{
    int iAlgorithm;
    unsigned short usLength = sizeof(cucSignatureAlgorithms);
    *ptrData++ = (unsigned char)(usLength >> 8);
    *ptrData++ = (unsigned char)(usLength);
    for (iAlgorithm = 0; iAlgorithm < MAX_SIGNATURE_HASH_ALGORITHMS; iAlgorithm++) { // insert each siganture alogorithm
        *ptrData++ = cucSignatureAlgorithms[iAlgorithm].ucHash;
        *ptrData++ = cucSignatureAlgorithms[iAlgorithm].ucSignature;
    }
    return ptrData;
}

static unsigned char *fnInsertGroups(unsigned char *ptrData)
{
    int iGroup;
    unsigned short usLength = sizeof(cusGroups);
    *ptrData++ = (unsigned char)(usLength >> 8);
    *ptrData++ = (unsigned char)(usLength);
    for (iGroup = 0; iGroup < MAX_GROUPS; iGroup++) {                    // insert each group
        *ptrData++ = (unsigned char)(cusGroups[iGroup] >> 8);
        *ptrData++ = (unsigned char)(cusGroups[iGroup]);
    }
    return ptrData;
}

static unsigned char *fnInsertEC_pointFormats(unsigned char *ptrData)
{
    int iFormat;
    *ptrData++ = sizeof(cucEC_pointFormats);
    for (iFormat = 0; iFormat < sizeof(cucEC_pointFormats); iFormat++) { // insert each format
        *ptrData++ = cucEC_pointFormats[iFormat];
    }
    return ptrData;
}

static unsigned char *fnInsertExtension(unsigned char *ptrData, const unsigned short usHandshakeExtension)
{
    unsigned char *ptrLength;
    unsigned short usLength;
    *ptrData++ = (unsigned char)(usHandshakeExtension >> 8);
    *ptrData++ = (unsigned char)(usHandshakeExtension);
    ptrLength = ptrData;
    ptrData += 2;                                                        // leave space for the extension length
    switch (usHandshakeExtension) {
    case HANDSHAKE_EXTENSION_SERVER_NAME:
        ptrData = fnInsertServerNameIndication(ptrData, "a5zj8ezn577ey.iot.us-east-2.amazonaws.com", 0); // server name type - host name
        break;
    case HANDSHAKE_EXTENSION_SIGNATURE_ALGORITHMS:
        ptrData = fnInsertSigatureHashAlgorithms(ptrData);
        break;
    case HANDSHAKE_EXTENSION_SUPPORTED_GROUPS:
        ptrData = fnInsertGroups(ptrData);
        break;
    case HANDSHAKE_EXTENSION_EC_POINT_FORMATS:
        ptrData = fnInsertEC_pointFormats(ptrData);
        break;
    case HANDSHAKE_EXTENSION_ENCRYPT_THEN_MAC:
    case HANDSHAKE_EXTENSION_EXTENDED_MASTER_SECRET:
        // Insert nothing
        //
        break;
    default:
        _EXCEPTION("??");
        break;
    }
    usLength = (ptrData - ptrLength - 2);                                // server name list length
    *ptrLength++ = (unsigned char)(usLength >> 8);
    *ptrLength++ = (unsigned char)(usLength);
    return ptrData;
}

static unsigned char *fnInsertHandshakeExtensions(unsigned char *ptrData)
{
    int iExtension;
    for (iExtension = 0; iExtension < MAX_HANDSHAKE_EXTENSIONS; iExtension++) { // insert each extension
        ptrData = fnInsertExtension(ptrData, cusHandshakeExtensions[iExtension]);
    }
    return ptrData;
}

// This assumes an X502 formatted certificate in flash memory
//
static int fnInsertCertificate(unsigned char **ptrptrData, int iCertificate)
{
    if (iCertificate == 0) {                                             // just one supported
        MEMORY_RANGE_POINTER file;
        unsigned long ulLength;
        CHAR *ptrSource;
        file = uOpenFile("7.txt");                                       // first certificate in flash
        ulLength = uGetFileLength(file);
        if (ulLength != 0) {                                             // if loaded
            unsigned char *ptrData = *ptrptrData;
            ptrSource = (CHAR *)fnGetFlashAdd(file + FILE_HEADER);       // the plain text version in Flash
            // Base64 decode the content between "-----BEGIN CERTIFICATE-----" and "-----END CERTIFICATE-----"
            //
            ptrSource += 27;                                             // assume we start with "-----BEGIN CERTIFICATE-----"
            ulLength = fnDecode64(ptrSource, (CHAR *)(ptrData + 3));     // this ignores carriage returns in the input string and the "-----END CERTIFICATE-----" at the end of it
            *ptrData++ = (unsigned char)(ulLength >> 16);
            *ptrData++ = (unsigned char)(ulLength >> 8);
            *ptrData++ = (unsigned char)(ulLength);
            ptrData += ulLength;
            *ptrptrData = ptrData;
            return (int)ulLength;                                        // the certificate's length
        }
    }
    return -1;                                                           // certificate doesn't exist
}


static unsigned char *fnInsertPublicKey(unsigned char *ptrData)
{
    unsigned char *ptrLength = ptrData++;
    unsigned char ucLength;

    switch (session_cipher) {
    case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:                           // tested at AWS
    case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA:                             // tested as Mosquitto.org
        ptrData = fnGeneratePublicKey(ptrData);                          // use the security library to generate the key and insert it into the data stream
        break;
    default:
        _EXCEPTION("cipher suite not tested!");
        return ptrData;
    }
    ucLength = (ptrData - ptrLength - 1);
    *ptrLength = ucLength;
    return ptrData;
}
/*
static const unsigned char referenceTx[] = {
    0x10, 0x48, 0x00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0x82, 0x04, 0xb0, 0x00, 0x08, 0x4d, 0x51, 0x54, 0x54, 0x45, 0x63, 0x68, 0x6f, 0x00, 0x32, 0x3f, 0x53, 0x44, 0x4b, 0x3d, 0x41, 0x6d, 0x61, 0x7a, 0x6f, 0x6e, 0x46,
    0x72, 0x65, 0x65, 0x52, 0x54, 0x4f, 0x53, 0x26, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d, 0x56, 0x39, 0x2e, 0x30, 0x2e, 0x30, 0x26, 0x50, 0x6c, 0x61, 0x74, 0x66, 0x6f, 0x72, 0x6d, 0x3d, 0x57, 0x69, 0x6e, 0x53,
    0x69, 0x6d
};
*/
extern int fnSecureLayerTransmission(USOCKET Socket, unsigned char *ucPrtData, unsigned short usLength, unsigned char ucFlag)
{
    unsigned char *ptrRecordLength;
    unsigned char *ptrBuffer = ucPrtData;
    unsigned char *ptrContentLength;
    ucPrtData += MIN_TCP_HLEN;                                           // leave space for TCP header
    ptrContentLength = ucPrtData;
    *ucPrtData++ = SSL_TLS_CONTENT_APPLICATION_DATA;
    *ucPrtData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
    *ucPrtData++ = (unsigned char)(TLS_VERSION_1_2);
    ptrRecordLength = ucPrtData;
    *ucPrtData++ = (unsigned char)(usLength >> 8);                       // unencrypted length (will be overwritten with the encrypted length)
    *ucPrtData++ = (unsigned char)(usLength);
    ucPrtData = fnEncrypt(ucPrtData, ucPrtData, usLength);
    usLength = (ucPrtData - ptrRecordLength - 2);                        // the encrypted content length
    *ptrRecordLength++ = (unsigned char)(usLength >> 8);                 // overwrite with encrypted content length
    *ptrRecordLength = (unsigned char)(usLength);
    usLength = (ucPrtData - ptrContentLength);                           // complete content length
    return (fnSendTCP((Socket & ~(SECURE_SOCKET_MODE)), ptrBuffer, usLength, ucFlag) > 0); // send data
}

extern int fnTLS(USOCKET Socket, unsigned char ucEvent)
{
    unsigned char ucTLS_frame[MIN_TCP_HLEN + 1400];                      // temporary buffer for constructing the secure socket layer message in
    unsigned char *ptrData = &ucTLS_frame[MIN_TCP_HLEN];                 // start of the message content
    unsigned char *ptrRecordLength;
    unsigned char *ptrHandshakeLength;
    unsigned char *ptrExtensionLength;
    unsigned short ucChecksumLength;
    unsigned short usLength;
    switch (ucEvent) {
    case TCP_EVENT_CONNECTED:                                            // send Client Hello since TCP has connected
        {
            // TLSv1.2 record layer
            //
            MEMORY_RANGE_POINTER our_private_key = uOpenFile("6.bin");
            MEMORY_RANGE_POINTER our_certificate = uOpenFile("7.bin");
    #if defined SUPPORT_RTC_ || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
            RTC_SETUP rtc;
            fnGetRTC(&rtc);                                              // present time
    #endif
            if (fnInitialiseSecureLayer((const unsigned char *)fnGetFlashAdd(our_certificate + FILE_HEADER), uGetFileLength(our_certificate), (const unsigned char *)fnGetFlashAdd(our_private_key + FILE_HEADER), uGetFileLength(our_private_key)) != 0) {
                return -1;                                               // can't create the session
            }
            *ptrData++ = SSL_TLS_CONTENT_HANDSHAKE;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_0 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_0);
            ptrRecordLength = ptrData;                                   // the location where the overall length is to be inserted
            ptrData += 2;                                                // leave space for the TLSv1.2 record content length
            // Handshake protocol
            //
            *ptrData++ = SSL_TLS_HANDSHAKE_TYPE_CLIENT_HELLO;
            ptrHandshakeLength = ptrData;
            ptrData += 3;                                                // leave space for the handshake protocol content length

            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);

    #if defined SUPPORT_RTC_ || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
            ptrData = fnInsertTLS_random(ptrData, rtc.ulLocalUTC);       // insert random (with UCT as first four bytes of the random number)
    #else
            ptrData = fnInsertTLS_random(ptrData, 0);                    // insert random
    #endif
            ptrData = fnInsertSessionID(ptrData, 0);                     // session ID length (0 to 32)
            ptrData = fnInsertCipherSuites(ptrData);                     // insert the accepted cipher suites
            *ptrData++ = 1;                                              // compression method length
            *ptrData++ = 0;                                              // no compression method

            ptrExtensionLength = ptrData;
            ptrData += 2;                                                // leave space for the extension content length

            ptrData = fnInsertHandshakeExtensions(ptrData);

            ucChecksumLength = (ptrData - ptrRecordLength - 2);          // the TLSv1.2 record content length
            *ptrRecordLength++ = (unsigned char)(ucChecksumLength >> 8);
            *ptrRecordLength = (unsigned char)(ucChecksumLength);

            usLength = (ptrData - ptrHandshakeLength - 3);               // the handshake protocol content length
            *ptrHandshakeLength++ = 0;                                   // most significant byte is always 0
            *ptrHandshakeLength++ = (unsigned char)(usLength >> 8);
            *ptrHandshakeLength++ = (unsigned char)(usLength);

            usLength = (ptrData - ptrExtensionLength - 2);               // the extension content length
            *ptrExtensionLength++ = (unsigned char)(usLength >> 8);
            *ptrExtensionLength = (unsigned char)(usLength);

            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats((ucChecksumLength - 4), ptrHandshakeLength);// update handshake statistics (calculates handshake check sum)
            iTLS_tx_state = 1;                                           // sending client hello
        }
        break;
    case TCP_TLS_CERTIFICATES:                                           // send our certificates(s) to the server
        {
            unsigned long ulLength;
            int iCertificates = 0;
            *ptrData++ = SSL_TLS_CONTENT_HANDSHAKE;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);
            ptrRecordLength = ptrData;                                   // the location where the overall length is to be inserted
            ptrData += 2;                                                // leave space for the TLSv1.2 record content length
            *ptrData++ = SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE;
            ptrHandshakeLength = ptrData;                                // the location where the certificate lengths are to be inserted
            ptrData += 6;                                                // leave space for length and certificates length fields
            FOREVER_LOOP() {
                if (fnInsertCertificate(&ptrData, iCertificates++) < 0) {
                    break;
                }
            }
            ucChecksumLength = (ptrData - ptrRecordLength - 2);
            *ptrRecordLength++ = (unsigned char)(ucChecksumLength >> 8);
            *ptrRecordLength++ = (unsigned char)(ucChecksumLength);
            ulLength = (ptrData - ptrHandshakeLength - 3);
            *ptrHandshakeLength++ = (unsigned char)(ulLength >> 16);
            *ptrHandshakeLength++ = (unsigned char)(ulLength >> 8);
            *ptrHandshakeLength++ = (unsigned char)(ulLength);
            ulLength -= 3;
            *ptrHandshakeLength++ = (unsigned char)(ulLength >> 16);
            *ptrHandshakeLength++ = (unsigned char)(ulLength >> 8);
            *ptrHandshakeLength = (unsigned char)(ulLength);

            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats((ucChecksumLength - 4), (ptrHandshakeLength - 2)); // update handshake statistics (calculates handshake check sum)
            iTLS_tx_state = 101;                                         // the next step if to send the client key exchange
            // Fall through intentionally (assuming enough output message space)
            //
    case TCP_TLS_CLIENT_KEY_EXCHANGE:
            *ptrData++ = SSL_TLS_CONTENT_HANDSHAKE;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);
            ptrHandshakeLength = ptrData;                                // the location where the client key exchange length is to be inserted
            ptrData += 2;                                                // leave space for length
            *ptrData++ = SSL_TLS_HANDSHAKE_TYPE_CLIENT_KEY_EXCHANGE;
            ptrExtensionLength = ptrData;                                // the location where the public key length is to be inserted
            ptrData += 3;                                                // leave space for length
            ptrData = fnInsertPublicKey(ptrData);                        // 
            ucChecksumLength = (ptrData - ptrHandshakeLength - 2);
            *ptrHandshakeLength++ = (unsigned char)(ucChecksumLength >> 8);
            *ptrHandshakeLength = (unsigned char)(ucChecksumLength);
            ulLength = (ptrData - ptrExtensionLength - 3);               // the public key content length
            *ptrExtensionLength++ = 0;                                   // MSB is always 0
            *ptrExtensionLength++ = (unsigned char)(ulLength >> 8);
            *ptrExtensionLength = (unsigned char)(ulLength);
            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats((ucChecksumLength - 4), (ptrHandshakeLength + 5)); // update handshake statistics (calculates handshake check sum)
            iTLS_tx_state = 102;                                         // the next step if to send a certificate verify
            // Fall through intentionally (assuming enough output message space)
            //
    case TCP_TLS_CERTIFICATE_VERIFY:
            *ptrData++ = SSL_TLS_CONTENT_HANDSHAKE;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);
            ptrHandshakeLength = ptrData;                                // the location where the client key exchange length is to be inserted
            ptrData += 2;                                                // leave space for length
            *ptrData++ = SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_VERIFY;
            ptrExtensionLength = ptrData;                                // the location where the signature algorithm length is to be inserted
            ptrData += 3;                                                // leave space for length
            ptrData = fnInsertSignatureAlgorithm(ptrData);
            ucChecksumLength = (unsigned short)(ptrData - ptrHandshakeLength - 2);
            *ptrHandshakeLength++ = (unsigned char)(ucChecksumLength >> 8);
            *ptrHandshakeLength = (unsigned char)(ucChecksumLength);
            ulLength = (ptrData - ptrExtensionLength - 3);               // the public key content length
            if (ulLength == 0) {                                         // if no certificate verification is needed we skip the message
                ptrData -= 9;
            }
            else {
                *ptrExtensionLength++ = (unsigned char)(ulLength >> 16);
                *ptrExtensionLength++ = (unsigned char)(ulLength >> 8);
                *ptrExtensionLength = (unsigned char)(ulLength);
                usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);       // complete content length
                fnHandshakeStats((ucChecksumLength - 4), (ptrHandshakeLength + 5)); // update handshake statistics (calculates handshake check sum)
            }
            iTLS_tx_state = 103;                                         // the next step if to send a change cipher spec request
            // Fall through intentionally (assuming enough output message space)
            //
    case TCP_TLS_CHANGE_CIPHER_SPEC:
            *ptrData++ = SSL_TLS_CONTENT_CHANGE_CIPHER_SPEC;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);
            *ptrData++ = 0;                                              // fixed length of 1
            *ptrData++ = 1;
            ptrHandshakeLength = ptrData;
            *ptrData++ = SSL_TLS_CHANGE_CIPHER_SPEC_MESSAGE;
            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            iTLS_tx_state = 104;                                         // the next step if to send an encrypted handshake message
            // Fall through intentionally (assuming enough output message space)
            //
    case TCP_TLS_ENCRYPTED_HANDSHAKE:
            *ptrData++ = SSL_TLS_CONTENT_HANDSHAKE;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);
            ptrHandshakeLength = ptrData;                                // the location where the encrypted handshake length is to be inserted
            ptrData += 2;                                                // leave space for length
            ptrData = fnFinished(ptrData);                               // insert encrypted handshake message
            ulLength = (ptrData - ptrHandshakeLength - 2);
            *ptrHandshakeLength++ = (unsigned char)(ulLength >> 8);
            *ptrHandshakeLength++ = (unsigned char)(ulLength);
            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            iTLS_tx_state = 105;                                         // the next step if to send an encrypted handshake message
        }
        break;
    case TCP_EVENT_CLOSE:
    case TCP_EVENT_ABORT:
        fnTearDown();
        // Reset
        //
        iTLS_tx_state = TLS_TX_STATE_IDLE;
        iTLS_rx_state = TLS_RX_STATE_IDLE;
        iTLS_rx_type = 0;
        iTLS_rx_encryted = 0;
        usTLS_rx_version = 0;
        session_cipher = 0;
        ptrReceptionBuffer = 0;
        ulBufferContent = 0;
        return 0;
    case TCP_EVENT_ACK:
    case TCP_EVENT_REGENERATE:
        return APP_SECURITY_HANDLED;
    default:
        return 0;
    }
    return (fnSendTCP((Socket & ~(SECURE_SOCKET_MODE)), ucTLS_frame, usLength, TCP_FLAG_PUSH) > 0); // send data
}

static unsigned char *fnExtractCertificate(unsigned char *ucPrtData, int iCertificateReference, unsigned long *ptrulCertificatesLength)
{
    unsigned long ulCertificateLength = *ucPrtData++;
    ulCertificateLength <<= 8;
    ulCertificateLength |= *ucPrtData++;
    ulCertificateLength <<= 8;
    ulCertificateLength |= *ucPrtData++;
    fnSaveServerCertificate(ucPrtData, ulCertificateLength);             // parse the certificate and save details
    ucPrtData += ulCertificateLength;
    ulCertificateLength += 3;                                            // account for the certificate length fields
    if (*ptrulCertificatesLength >= ulCertificateLength) {
        *ptrulCertificatesLength -= ulCertificateLength;
    }
    else {
        *ptrulCertificatesLength = 0;
    }
    return ucPrtData;
}

static int fnHandleAlert(unsigned char *ucPrtData, unsigned long ulHandshakeSize)
{
    fnDebugMsg("Alert ");
    switch (*ucPrtData++) {
    case SSL_TLS_ALERT_LEVEL_WARNING:
        fnDebugMsg("Warning: ");
        break;
    case SSL_TLS_ALERT_LEVEL_FATAL:
        fnDebugMsg("Fatal: ");
        break;
    default:
        fnDebugMsg("?\r\n");
        return 0;
    }
    fnDebugDec(*ucPrtData, WITH_CR_LF);                                  // alert code
    return 0;
}


// Handle individual handshake fields in the input buffer
//
static int fnHandleHandshake(USOCKET Socket, unsigned char *ucPrtData, unsigned long ulHandshakeSize, unsigned char ucPresentHandshakeType)
{
    static int iNextState = 0;
#if defined _WINDOWS
    unsigned char ucBuffer[4 * 1024];
    MEMORY_RANGE_POINTER file = 0;
    switch (ucPresentHandshakeType) {                                    // the handshake protocol being treated
    case SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO:
        file = uOpenFile("0.bin");
        break;
    case SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE:
        file = uOpenFile("1.bin");
        break;
    case SSL_TLS_HANDSHAKE_TYPE_SERVER_KEY_EXCHANGE:
        file = uOpenFile("4.bin");
        break;
    case SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST:
        file = uOpenFile("5.bin");
        break;
    case SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO_DONE:
        {
            static const unsigned char server_hello[] = { 0x0e, 0x00, 0x00, 0x00 };
            ucPrtData = (unsigned char *)server_hello;
            ucPrtData += 4;
        }
        break;
    default:
        break;
    }
    if (file != 0) {
        if (ucPrtData == 0) {                                            // playback mode
            MAX_FILE_LENGTH length = uGetFileLength(file);
            uMemcpy(ucBuffer, fnGetFlashAdd(file + FILE_HEADER), length);
            ucPrtData = ucBuffer;
            ulHandshakeSize = length;
        }
        else {                                                           // session recoring
            unsigned char ucMimeType = MIME_BINARY;
            uFileWrite(file, ucPrtData, (MAX_FILE_LENGTH)ulHandshakeSize);
            uFileCloseMime(file, &ucMimeType);
        }
    }
#endif
    switch (ucPresentHandshakeType) {                                    // the handshake protocol being treated
    case SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO:                            // receiving server hello
        {
            SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_32_ID *ptrHello = (SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_32_ID *)ucPrtData;
            SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS *ptrHelloSession;
            unsigned short usExtensionLength;
            unsigned char *ptrExtensionData;
            iNextState = 0;
            fnDebugMsg("Server hello recognised ");
            if (ptrHello->version[0] == (unsigned char)(TLS_VERSION_1_2 >> 8)) { // we only accept TLSv1.2
                if (ptrHello->version[1] == (unsigned char)(TLS_VERSION_1_2)) {
                    if (ptrHello->session_id_length <= 32) {
                        fnHandshakeStats(ulHandshakeSize, ucPrtData);    // update handshake statistics (calculates handshake check sum)
                        fnEnterRandom(ptrHello->random, 1);              // save the server's 32 bytes of random data (the first 4 may contain the UTC time)
                        ptrHello = (SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_32_ID *)(((unsigned char *)ptrHello) + (32 - ptrHello->session_id_length)); // set the session content pointer accordingly
                        ptrHelloSession = (SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS *)&(ptrHello->session_details);
                        session_cipher = ((ptrHelloSession->cipher[0] << 8) | (ptrHelloSession->cipher[1])); // the cipher suite to be used during the session
                        fnSetSessionCipher(session_cipher, ptrHello->version, ptrHello->session_id_length, (ptrHelloSession->cipher - ptrHello->session_id_length));
                        // ptrHelloSession->compression_method will be 0 since we always set zero
                        usExtensionLength = ptrHelloSession->extensionsLength[0];
                        usExtensionLength <<= 8;
                        usExtensionLength |= ptrHelloSession->extensionsLength[1];
                        ptrExtensionData = (unsigned char *)&(ptrHelloSession->extension);
                        while (usExtensionLength >= 4) {                 // handle each extension
                            unsigned short usThisExtensionLength;
                            unsigned short usExtensionType = *ptrExtensionData++; // the extension type
                            usExtensionType <<= 8;
                            usExtensionType |= *ptrExtensionData++;
                            usThisExtensionLength = *ptrExtensionData++;
                            usThisExtensionLength <<= 8;
                            usThisExtensionLength |= *ptrExtensionData++;
                            if (fnHandleSecurityExtension(usExtensionType, usThisExtensionLength, ptrExtensionData) != 0) {
                                return -1;
                            }
                            ptrExtensionData += usThisExtensionLength;
                            usThisExtensionLength += 4;                  // account for the extension type and length fields
                            if (usExtensionLength <= usThisExtensionLength) {
                                break;
                            }
                            usExtensionLength -= usThisExtensionLength;  // remaining length
                        }
                    }
                }
            }
            else {
                return -1;
            }
        }
        break;
    case SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE:                             // we are receiving the server's certificate(s)
        {
            int iCertificateReference = 0;
            unsigned long ulCertificatesLength = *ucPrtData;
            fnHandshakeStats(ulHandshakeSize, ucPrtData++);              // update handshake statistics (calculates handshake check sum)
            ulCertificatesLength <<= 8;
            ulCertificatesLength |= *ucPrtData++;
            ulCertificatesLength <<= 8;
            ulCertificatesLength |= *ucPrtData++;
            fnDebugMsg("Certificate recognised ");
            while (ulCertificatesLength != 0) {
                ucPrtData = fnExtractCertificate(ucPrtData, iCertificateReference++, &ulCertificatesLength);
            }
        }
        break;
    case SSL_TLS_HANDSHAKE_TYPE_SERVER_KEY_EXCHANGE:                     // we are receiving the server's public key
        {
            fnDebugMsg("Key exchange recognised ");
            fnHandshakeStats(ulHandshakeSize, ucPrtData);                // update handshake statistics (calculates handshake check sum)
            fnExtractPublicKey(ucPrtData, ulHandshakeSize);
        }
        break;
    case SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST:                     // the server is requesting a certificate from us - this is the next step that we must do
        {
            unsigned short usSignatureHashAlgorithmsLength;
            fnHandshakeStats(ulHandshakeSize, ucPrtData++);              // update handshake statistics (calculates handshake check sum)
            ucPrtData += 3;                                              // the certificate types
            usSignatureHashAlgorithmsLength = *ucPrtData++;
            usSignatureHashAlgorithmsLength <<= 8;
            usSignatureHashAlgorithmsLength |= *ucPrtData++;
            fnDebugMsg("Certificate request recognised ");
            fnPrepareCertificate(ucPrtData, usSignatureHashAlgorithmsLength);
            ucPrtData += usSignatureHashAlgorithmsLength;                // the certificate algorithms that are accepted
            iNextState = 100;
        }
        break;
    case SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO_DONE:                       // the handshake record has copleted
        fnDebugMsg("Hello done recognised ");
        fnDebugDec(ulHandshakeSize, WITH_CR_LF);
        fnHandshakeStats(ulHandshakeSize, ucPrtData);                    // update handshake statistics (calculates handshake check sum)
        if (iNextState == 100) {                                         // immediately respond with our certificate(s)
            return (fnTLS(Socket, TCP_TLS_CERTIFICATES));
        }
        else {
            return (fnTLS(Socket, TCP_TLS_CLIENT_KEY_EXCHANGE));         // skip certificate and start with client key exchange
        }
        break;
    case SSL_TLS_HANDSHAKE_TYPE_FINISHED:
        fnDebugMsg("Finished recognised\r\n");
        return APP_SECURITY_CONNECTED;                                   // the secure connetion is complete so we allow the application layer to use it
    default:
        fnDebugMsg("????");
        break;
    }
    fnDebugDec(ulHandshakeSize, WITH_CR_LF);
    return 0;
}

// TLS content being received
//
extern int fnSecureLayerReception(USOCKET Socket, unsigned char **ptr_ucPrtData, unsigned short *ptr_usLength)
{
    static unsigned long ulHandshakeSize = 0;
    static unsigned short usRecordLength = 0;
    static unsigned char ucPresentHandshakeType = 0;
    unsigned long ulThisLength;
    int iReturn = APP_ACCEPT;
    unsigned char *ucPrtData = *ptr_ucPrtData;
    unsigned short usLength = *ptr_usLength;
    while (usLength != 0) {                                              // while we still have data to handle
        switch (iTLS_rx_state) {
        case TLS_RX_STATE_IDLE:                                          // we are expecting a TLS record layer message
            iTLS_rx_type = *ucPrtData;
            switch (iTLS_rx_type) {                                      // record layer content type
            case SSL_TLS_CONTENT_HANDSHAKE:
            case SSL_TLS_CONTENT_CHANGE_CIPHER_SPEC:
            case SSL_TLS_CONTENT_APPLICATION_DATA:
            case SSL_TLS_CONTENT_ALERT:
                ulHandshakeSize = 0;
                usRecordLength = 0;
                ucPresentHandshakeType = 0;
                iTLS_rx_state = TLS_RX_STATE_VERSION_MSB;
                break;
            default:                                                     // ignore unknown type
                break;
            }
            break;
        case TLS_RX_STATE_VERSION_MSB:                                   // collecting MSB of version
            usTLS_rx_version = (*ucPrtData << 8);
            iTLS_rx_state = TLS_RX_STATE_VERSION_LSB;
            break;
        case TLS_RX_STATE_VERSION_LSB:                                   // collecting LSB of version
            usTLS_rx_version |= *ucPrtData;                              // version being received is now known
            iTLS_rx_state = TLS_RX_STATE_LENGTH_MSB;                     // we have checked the TLS version number
            break;
        case TLS_RX_STATE_LENGTH_MSB:                                    // collecting MSB of length
            usRecordLength = *ucPrtData;
            usRecordLength <<= 8;
            iTLS_rx_state = TLS_RX_STATE_LENGTH_LSB;
            break;
        case TLS_RX_STATE_LENGTH_LSB:                                    // collecting LSB of length
            usRecordLength |= *ucPrtData;                                // total content known
            if (iTLS_rx_type == SSL_TLS_CONTENT_CHANGE_CIPHER_SPEC) {
                if (usRecordLength != 1) {                               // only content length of one is ever expected
                    iTLS_rx_state = TLS_RX_STATE_IDLE;
                }
                iTLS_rx_state = TLS_RX_STATE_CIPHER_SPEC_MESSAGE;
            }
            else {
                if (iTLS_rx_encryted != 0) {
                    ulHandshakeSize = usRecordLength;                    // the content size
                    ucPresentHandshakeType = 0;
                    if (SSL_TLS_CONTENT_APPLICATION_DATA == iTLS_rx_type) {
                        iTLS_rx_state = TLS_RX_STATE_ENCRYPTED_APP_CONTENT; // collect encrypted application content
                    }
                    else if (SSL_TLS_CONTENT_ALERT == iTLS_rx_type) {
                        iTLS_rx_state = TLS_RX_STATE_ENCRYPTED_ALERT_CONTENT; // collect encrypted alert content
                    }
                    else {
                        iTLS_rx_state = TLS_RX_STATE_ENCRYPTED_RECORD_CONTENT; // collect encrypted record content
                    }
                }
                else {
                    iTLS_rx_state = TLS_RX_STATE_PROTOCOL_TYPE;
                }
            }
            break;
        case TLS_RX_STATE_PROTOCOL_TYPE:                                 // collecting protocol type field
            ucPresentHandshakeType = *ucPrtData;                         // the handshake protocol type
            iTLS_rx_state = TLS_RX_STATE_TYPE_LENGTH_HSB;
            break;
        case TLS_RX_STATE_TYPE_LENGTH_HSB:                               // collecting highest byte of type length
            ulHandshakeSize = *ucPrtData;
            ulHandshakeSize <<= 8;
            iTLS_rx_state = TLS_RX_STATE_TYPE_LENGTH_MSB;
            break;
        case TLS_RX_STATE_TYPE_LENGTH_MSB:                               // collecting mid byte of type length
            ulHandshakeSize |= *ucPrtData;
            ulHandshakeSize <<= 8;
            iTLS_rx_state = TLS_RX_STATE_TYPE_LENGTH_LSB;
            break;
        case TLS_RX_STATE_TYPE_LENGTH_LSB:                               // collecting LSB of type length
            ulHandshakeSize |= *ucPrtData;
            iTLS_rx_state = TLS_RX_STATE_CONTENT;
            if (ulHandshakeSize != 0) {
                break;
            }
            usLength--;                                                  // this is expected only on reception of zero content server hello done but this would ensure that following records would also be handled correctly if they ever followed
            ucPrtData++;
            // Fall through intentionally if the handshake size is zero because there is no content to be collected
            //
        case TLS_RX_STATE_CONTENT:                                       // collecting content
            if (usLength < ulHandshakeSize) {                            // if the complete handshake protocol is not contained in the input buffer
                register unsigned long ulSave = usLength;
                if ((ulBufferContent + ulSave) > ulHandshakeSize) {      // handle only the required length
                    ulSave = (unsigned short)(ulHandshakeSize - ulBufferContent);
                }
                if (ptrReceptionBuffer == 0) {                           // buffer doesn't yet exist
                    ptrReceptionBuffer = (unsigned char *)uCalloc(1, (ulHandshakeSize + 4)); // create reception buffer as required to handle this content
                    ptrReceptionBuffer[0] = ucPresentHandshakeType;      // insert the type and length fields since these are required by the handshake statistics function
                    ptrReceptionBuffer[1] = (unsigned char)(ulHandshakeSize >> 16);
                    ptrReceptionBuffer[2] = (unsigned char)(ulHandshakeSize >> 8);
                    ptrReceptionBuffer[3] = (unsigned char)ulHandshakeSize;
                    ptrReceptionBuffer += 4;                             // generally we don't need to know that these fields are in the buffer
                }
                uMemcpy(&ptrReceptionBuffer[ulBufferContent], ucPrtData, ulSave);  // save to the intermediate buffer
                if ((ulBufferContent + ulSave) < ulHandshakeSize) {      // if the handshake protocol content hasn't yet been completely collected
                    ulBufferContent += ulSave;
                    return iReturn;
                }
                iReturn |= fnHandleHandshake(Socket, ptrReceptionBuffer, ulHandshakeSize, ucPresentHandshakeType); // handle from intermediate buffer
                uCFree(ptrReceptionBuffer - 4);                          // deallocate intermediate reception buffer memory (note that we set the pointer back to be beginning of the physical buffer)
                ptrReceptionBuffer = 0;                                  // no more memory allocated
                ulThisLength = ulSave;
            }
            else {                                                       // this tcp frame contains the complete handshake protocol content
                iReturn |= fnHandleHandshake(Socket, ucPrtData, ulHandshakeSize, ucPresentHandshakeType); // handle directly in tcp reception buffer
                ulThisLength = ulHandshakeSize;
            }
            usRecordLength -= 4;                                         // compensate for the handshake protocol type and length in each handled protocol
            if (ulHandshakeSize >= usRecordLength) {                     // if the complete record has been handled
                usRecordLength = 0;
                ulBufferContent = 0;
                iTLS_rx_state = TLS_RX_STATE_IDLE;                       // the record has been completely handled so start searching the next
                usLength -= (unsigned short)ulThisLength;
                ucPrtData += ulThisLength;
                continue;                                                // do not perform usLength and ucPtrData manipulation
            }
            usLength -= (unsigned short)(ulHandshakeSize - ulBufferContent); // remaining in present input buffer
            ucPrtData += (ulHandshakeSize - ulBufferContent);
            ulBufferContent = 0;
            usRecordLength -= (unsigned short)ulHandshakeSize;           // total record content remaining
            iTLS_rx_state = TLS_RX_STATE_PROTOCOL_TYPE;                  // continue with next handshake protocol
            continue;                                                    // do not perform usLength and ucPtrData manipulation
        case TLS_RX_STATE_ENCRYPTED_APP_CONTENT:
        case TLS_RX_STATE_ENCRYPTED_ALERT_CONTENT:
        case TLS_RX_STATE_ENCRYPTED_RECORD_CONTENT:
            if (usLength < ulHandshakeSize) {                            // if the complete handshake protocol is not contained in the input buffer
                _EXCEPTION("No buffering available (at the moment)...");
            }
            else {                                                       // this tcp frame contains the complete handshake protocol content
                fnDecrypt(&ucPrtData, &ulHandshakeSize);                 // decrypt the collected content (performed in place, with reduction in output length)
                if (TLS_RX_STATE_ENCRYPTED_APP_CONTENT == iTLS_rx_state) {
                    *ptr_ucPrtData = ucPrtData;                          // the data content
                    *ptr_usLength = (unsigned short)ulHandshakeSize;     // the data length
                    iReturn = APP_SECURITY_HANDLED;                      // allow the user to handle the data as if it were received unencoded
                }
                else if (TLS_RX_STATE_ENCRYPTED_ALERT_CONTENT == iTLS_rx_state) {
                    iReturn |= fnHandleAlert(ucPrtData, ulHandshakeSize);
                }
                else {                                                   // decrypted handshake
                    ucPresentHandshakeType = *ucPrtData++;
                    ulHandshakeSize = (*ucPrtData++ << 16);
                    ulHandshakeSize |= (*ucPrtData++ << 8);
                    ulHandshakeSize |= *ucPrtData++;
                    iReturn |= fnHandleHandshake(Socket, ucPrtData, ulHandshakeSize, ucPresentHandshakeType); // handle directly in tcp reception buffer
                }
            }
            usRecordLength = 0;
            ulBufferContent = 0;
            iTLS_rx_state = TLS_RX_STATE_IDLE;                           // the record has been completely handled so start searching the next
            return iReturn;
        case TLS_RX_STATE_CIPHER_SPEC_MESSAGE:
            if (SSL_TLS_CHANGE_CIPHER_SPEC_MESSAGE == *ucPrtData) {      // change cipher spec message
                fnSwitchTransformSpec();
                iTLS_rx_encryted = 1;                                    // from this point on the reception needs to be decrypted
            }
            iTLS_rx_state = TLS_RX_STATE_IDLE;                           // continue with next record
            break;
        default:
            return iReturn;
        }
        usLength--;
        ucPrtData++;
    }
    return iReturn;
}

static int (*app_listener[NO_OF_TCPSOCKETS])(USOCKET, unsigned char, unsigned char *, unsigned short) = {0};


// Local listener on secure port (inserted between the secure socket layer and the application's listener)
//
static int fnSecureListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    static unsigned char ucUnacked = 0;
    int iResult;
    switch (ucEvent) {
    case TCP_EVENT_CONNECTED:                                            // the server has accepted the TCP connection request
        iResult = fnTLS(Socket, TCP_EVENT_CONNECTED);                    // start establishing a secure socket connection
        if (iResult < 0) {                                               // if it is not possible to start we need to close
            fnTCP_close(Socket);
            return APP_REQUEST_CLOSE;
        }
        return (ucUnacked = (iResult > 0));

    case TCP_EVENT_CLOSE:                                                // server is requesting a TCP close
        fnTLS(Socket, TCP_EVENT_CLOSE);
        break;

    case TCP_EVENT_ABORT:                                                // server has reset the TCP connection
        fnTLS(Socket, TCP_EVENT_ABORT);
        break;

    case TCP_EVENT_ACK:                                                  // last TCP transmission has been acknowledged
        {
            int iReturn;
            ucUnacked = 0;                                               // no more outstanding data to be acked
            iReturn = fnTLS(Socket, TCP_EVENT_ACK);
            if (APP_SECURITY_HANDLED != iReturn) {                       // if the handling responded to an ack
                ucUnacked = 1;
                return iReturn;                                          // return according to the secure socket layer's response
            }
        }
        break;
    case TCP_EVENT_REGENERATE:                                           // we must repeat the previous transmission
        {
            int iReturn;
            ucUnacked = 0;
            iReturn = fnTLS(Socket, TCP_EVENT_REGENERATE);
            if (APP_SECURITY_HANDLED != iReturn) {                       // if the handling needed to regenerate
                ucUnacked = 1;
                return iReturn;                                          // return according to the secure socket layer's response
            }
        }
        break;

    case TCP_EVENT_DATA:                                                 // we have new receive data
        {
            int iReturn = fnSecureLayerReception(Socket, &ucIp_Data, &usPortLen);
            if ((APP_SECURITY_HANDLED & iReturn) == 0) {                 // if the handling did not decrypt the content to be handled subsequently by the application
                if ((APP_SECURITY_CONNECTED & iReturn) != 0) {           // if the secure layer has just been established
                    ucEvent = TCP_EVENT_CONNECTED;
                    break;
                }
                return iReturn;                                          // return according to the secure socket layer's response
            }
        }
        break;

    case TCP_EVENT_ARP_RESOLUTION_FAILED:
    case TCP_EVENT_CLOSED:                                               // the TCP connection has closed
    case TCP_EVENT_CONREQ:
    default:
        break;
    }
    // Pass on to the application layer callback
    //
    return (app_listener[_TCP_SOCKET_MASK(Socket)]((Socket | SECURE_SOCKET_MODE), ucEvent, ucIp_Data, usPortLen));
}

extern void *fnInsertSecureLayer(USOCKET TCP_socket, int (*listener)(USOCKET, unsigned char, unsigned char *, unsigned short), int iInsert)
{
    if (iInsert != 0) {
        app_listener[_TCP_SOCKET_MASK(TCP_socket)] = listener;           // enter the application listener
        return fnSecureListener;                                         // return the inserted secure listener
    }
    else {
        if (app_listener[_TCP_SOCKET_MASK(TCP_socket)] != 0) {
            return (app_listener[_TCP_SOCKET_MASK(TCP_socket)]);
        }
        else {
            return listener;
        }
    }
}


extern void test_secure(USOCKET socket)
{
    #if defined _WINDOWS                                                 // test server handshake sequence
    fnTLS(socket, TCP_EVENT_CONNECTED);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_SERVER_KEY_EXCHANGE);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO_DONE);
    #endif
}
#endif

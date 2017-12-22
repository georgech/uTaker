/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      secure_layer.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/        

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined SECURE_MQTT

// To include in standard header
//
extern int fnInitialiseSecureLayer(const unsigned char *ptrOurCertificate, unsigned long ulCertificateLength, const unsigned char *ptrOutPrivateKey, unsigned long ulOurPrivateKeyLength);
extern void fnHandshakeStats(unsigned long ulHandshakeSize, unsigned char *ucPrtData);
extern void fnEnterRandom(unsigned char *ucPrtData);
extern void fnSetSessionCipher(unsigned short session_cipher, unsigned char ucVersion[2], unsigned char ucIdLength, unsigned char *ptrID);
extern int fnHandleSecurityExtension(unsigned short ext_id, unsigned short ext_size, unsigned char *ptrExtensionData);


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

static int iTLS_state = 0;
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

static unsigned char *fnInsertTLS_random(unsigned char *ptrData)
{
    RTC_SETUP rtc;
    int i;
    unsigned short usRandom;
#if defined SUPPORT_RTC || defined USE_SNTP || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC
    #define TLS_RANDOM_LENGTH     28
    fnGetRTC(&rtc);                                                      // present time
    *ptrData++ = (unsigned char)(rtc.ulLocalUTC >> 24);
    *ptrData++ = (unsigned char)(rtc.ulLocalUTC >> 16);
    *ptrData++ = (unsigned char)(rtc.ulLocalUTC >> 8);
    *ptrData++ = (unsigned char)(rtc.ulLocalUTC);
#else
    #define TLS_RANDOM_LENGTH     32
#endif
    for (i = 0; i < TLS_RANDOM_LENGTH; i += sizeof(unsigned short)) {
        usRandom = fnGetRndHW();
        *ptrData++ = (unsigned char)(usRandom >> 8);
        *ptrData++ = (unsigned char)(usRandom);
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
    case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:
        ptrData = fnGeneratePublicKey(ptrData);                          // use the security library to generate the key and insert it into the data stream
        break;
    default:
        _EXCEPTION("cipher suite not supported!");
        return ptrData;
    }
    ucLength = (ptrData - ptrLength - 1);
    *ptrLength = ucLength;
    return ptrData;
}


extern int fnTLS(USOCKET Socket, unsigned char ucEvent)
{
    unsigned char ucTLS_frame[MIN_TCP_HLEN + 1024];                      // temporary buffer for constructing the secure socket layer message in
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
            if (fnInitialiseSecureLayer((const unsigned char *)fnGetFlashAdd(our_certificate + FILE_HEADER), uGetFileLength(our_certificate), (const unsigned char *)fnGetFlashAdd(our_private_key + FILE_HEADER), uGetFileLength(our_private_key)) != 0) {
                return -1;                                               // can't create the session or local keys missing
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

            ptrData = fnInsertTLS_random(ptrData);                       // insert random
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
            fnHandshakeStats(ucChecksumLength, (ptrHandshakeLength - 4));// update handshake statistics (calculates handshake check sum)
            iTLS_state = 1;                                              // sending client hello
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
            *ptrHandshakeLength++ = (unsigned char)(ulLength);

            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats(ucChecksumLength, (ptrHandshakeLength - 4));// update handshake statistics (calculates handshake check sum)
            iTLS_state = 101;                                            // the next step if to send the client key exchange
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
            *ptrHandshakeLength++ = (unsigned char)(ucChecksumLength);
            ulLength = (ptrData - ptrExtensionLength - 3);               // the public key content length
            *ptrExtensionLength++ = 0;                                   // MSB is always 0
            *ptrExtensionLength++ = (unsigned char)(ulLength >> 8);
            *ptrExtensionLength = (unsigned char)(ulLength);
            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats(ucChecksumLength, (ptrHandshakeLength - 4));// update handshake statistics (calculates handshake check sum)
            iTLS_state = 102;                                            // the next step if to send a certificate verify
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
            *ptrExtensionLength++ = (unsigned char)(ulLength >> 16);
            *ptrExtensionLength++ = (unsigned char)(ulLength >> 8);
            *ptrExtensionLength = (unsigned char)(ulLength);
            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats(ucChecksumLength, (ptrHandshakeLength - 4));// update handshake statistics (calculates handshake check sum)
            iTLS_state = 103;                                            // the next step if to send a change cipher spec request
            // Fall through intentionally (assuming enough output message space)
            //
    case TCP_TLS_CHANGE_CIPHER_SPEC:
            *ptrData++ = SSL_TLS_CONTENT_HANDSHAKE;
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2 >> 8);
            *ptrData++ = (unsigned char)(TLS_VERSION_1_2);
            *ptrData++ = 0;                                              // fixed length of 1
            *ptrData++ = 1;
            ptrHandshakeLength = ptrData;
            *ptrData++ = SSL_TLS_CHANGE_CIPHER_SPEC_MESSAGE;
            usLength = (ptrData - &ucTLS_frame[MIN_TCP_HLEN]);           // complete content length
            fnHandshakeStats(6, (ptrHandshakeLength - 4));               // update handshake statistics (calculates handshake check sum)
            iTLS_state = 104;                                            // the next step if to send an encrypted handshake message
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
            iTLS_state = 105;                                            // the next step if to send an encrypted handshake message
        }
        break;
    case TCP_EVENT_ACK:
    case TCP_EVENT_REGENERATE:
        return APP_SECURITY_HANDLED;
    default:
        return 0;
    }
    return (fnSendTCP(Socket, ucTLS_frame, usLength, TCP_FLAG_PUSH) > 0); // send data
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
            fnDebugMsg("Hello server recognised ");
            if (ptrHello->version[0] == (unsigned char)(TLS_VERSION_1_2 >> 8)) { // we only accept TLSv1.2
                if (ptrHello->version[1] == (unsigned char)(TLS_VERSION_1_2)) {
                    if (ptrHello->session_id_length <= 32) {
                        fnHandshakeStats(ulHandshakeSize, ucPrtData);    // update handshake statistics (calculates handshake check sum)
                        fnEnterRandom(ptrHello->random);                 // save the server's 32 bytes of random data (the first 4 may contain the UTC time)
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
                            usThisExtensionLength += 4;                  // account for the extension type and length fields
                            if (usExtensionLength <= usThisExtensionLength) {
                                break;
                            }
                            usExtensionLength -= usThisExtensionLength;  // remaining length
                        }
                    }
                    return 0;
                }
            }
            return -1;
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
        fnHandshakeStats(ulHandshakeSize, ucPrtData);                    // update handshake statistics (calculates handshake check sum)
        if (iNextState == 100) {                                         // immediately respond with our certificate(s)
            return (fnTLS(Socket, TCP_TLS_CERTIFICATES));
        }
        else {
            return (fnTLS(Socket, TCP_TLS_CLIENT_KEY_EXCHANGE));         // skip certificate and start with client key exchange
        }
        break;
    default:
        fnDebugMsg("????");
        break;
    }
    fnDebugDec(ulHandshakeSize, WITH_CR_LF);
    return 0;
}

// TLS content being received
//
extern int fnSecureLayerReception(USOCKET Socket, unsigned char *ucPrtData, unsigned short *ptr_usLength)
{
    static unsigned long ulHandshakeSize = 0;
    static unsigned short usRecordLength = 0;
    static unsigned char ucPresentHandshakeType = 0;
    int iReturn = APP_ACCEPT;
    unsigned short usLength = *ptr_usLength;
    while (usLength != 0) {                                              // while we still have data to handle
        switch (iTLS_state) {
        case 1:                                                          // we are expecting a TLSv1.2 record layer message
            if (*ucPrtData == SSL_TLS_CONTENT_HANDSHAKE) {
                ulHandshakeSize = 0;
                usRecordLength = 0;
                ucPresentHandshakeType = 0;
                iTLS_state = 2;
            }
            break;
        case 2:
            if (*ucPrtData == (unsigned char)(TLS_VERSION_1_2 >> 8)) {
                iTLS_state = 3;
            }
            break;
        case 3:
            if (*ucPrtData == (unsigned char)(TLS_VERSION_1_2)) {
                iTLS_state = 4;                                          // we have checked the TLS version number
            }
            break;
        case 4:
            usRecordLength = *ucPrtData;
            usRecordLength <<= 8;
            iTLS_state = 5;
            break;
        case 5:
            usRecordLength |= *ucPrtData;                                // total content
            iTLS_state = 6;
            break;
        case 6:
            ucPresentHandshakeType = *ucPrtData;                         // the handshake protocol type
            iTLS_state = 7;
            break;
        case 7:
            ulHandshakeSize = *ucPrtData;
            ulHandshakeSize <<= 8;
            iTLS_state = 8;
            break;
        case 8:
            ulHandshakeSize |= *ucPrtData;
            ulHandshakeSize <<= 8;
            iTLS_state = 9;
            break;
        case 9:
            ulHandshakeSize |= *ucPrtData;
            iTLS_state = 10;
            if (ulHandshakeSize != 0) {
                break;
            }
            usLength--;                                                  // this is expected only on reception of zero content server hello done but this would ensure that following records would also be handled correctly if they ever followed
            ucPrtData++;
            // Fall through intentionally if the handshake size is zero
            //
        case 10:
            if (usLength < ulHandshakeSize) {                            // if the complete handshake protocol is not contained in the input buffer
                register unsigned long ulSave = usLength;
                if ((ulBufferContent + ulSave) > ulHandshakeSize) {      // handle only the required length
                    ulSave = (unsigned short)(ulHandshakeSize - ulBufferContent);
                }
                if (ptrReceptionBuffer == 0) {
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
                    return APP_ACCEPT;
                }
                iReturn |= fnHandleHandshake(Socket, ptrReceptionBuffer, ulHandshakeSize, ucPresentHandshakeType); // handle from intermediate buffer
                uCFree(ptrReceptionBuffer - 4);                          // deallocate intermediate reception buffer memory (note that we set the pointer back to be beginning of the physical buffer)
            }
            else {                                                       // this tcp frame contains the complete handshake protocol content
                iReturn |= fnHandleHandshake(Socket, ucPrtData, ulHandshakeSize, ucPresentHandshakeType); // handle directly in  tcp reception buffer
            }
            usRecordLength -= 4;                                         // compensate for the handshake protocol type and length in each handled protocol
            if (ulHandshakeSize >= usRecordLength) {                     // if the complete record has been handled
                usRecordLength = 0;
                ulBufferContent = 0;
                iTLS_state = 1;                                          // the record has been completely handled
                return iReturn;
            }
            usLength -= (unsigned short)(ulHandshakeSize - ulBufferContent); // remaining in present input buffer
            ucPrtData += (ulHandshakeSize - ulBufferContent);
            ulBufferContent = 0;
            usRecordLength -= (unsigned short)ulHandshakeSize;           // total remaining
            iTLS_state = 6;                                              // continue with next handshake protocol
            continue;                                                    // do not perform usLength and ucPtrData manipulation
        default:
            // We are connected so can decrypt content for the application
            //
            switch (*ucPrtData) {
            case SSL_TLS_CONTENT_HANDSHAKE:
                break;
            case SSL_TLS_CONTENT_CHANGE_CIPHER_SPEC:
                break;
            case SSL_TLS_CONTENT_APPLICATION_DATA:
                //return APP_SECURITY_HANDLED; // if decrypted, whereby the new data length can be changed by *ptr_usLength
                break;
            case SSL_TLS_CONTENT_ALERT:
                break;
            }
            return APP_ACCEPT;
        }
        usLength--;
        ucPrtData++;
    }
    return APP_ACCEPT;
}

extern void test_secure(USOCKET socket)
{
#if defined _WINDOWS                                                     // test server handshake sequence
    fnTLS(socket, TCP_EVENT_CONNECTED);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_SERVER_KEY_EXCHANGE);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST);
    fnHandleHandshake(socket, 0, 0, SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO_DONE);
#endif
}
#endif

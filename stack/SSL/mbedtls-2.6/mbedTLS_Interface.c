/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      mbedTLS_Interface.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/

#include "config.h"

#if defined SECURE_MQTT

#include "mbedtls/config.h"
#include "mbedtls/ecp.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/platform.h"

extern int fnSHA256(const unsigned char *ptrInput, unsigned char *ptrOutput, unsigned long ulLength, int iMode)
{
    mbedtls_sha256_context sha256;
    mbedtls_sha256_starts(&sha256, 0);
    mbedtls_sha256_update(&sha256, ptrInput, ulLength);
    mbedtls_sha256_finish(&sha256, ptrOutput);
    return 0;
}

typedef struct stUTASKER_MBEDSSL_SESSION
{
    mbedtls_ssl_context  ssl;
    mbedtls_ssl_config   config;
    mbedtls_x509_crt     ourCertificate;
    mbedtls_pk_context   ourPrivateKey;
    mbedtls_ecdh_context edch;                                           // elliptical curve Diffie-Hellman context
    mbedtls_x509_crt     certificates;                                   // initial certificate object
} UTASKER_MBEDSSL_SESSION;


static void ssl_handshake_params_init(mbedtls_ssl_handshake_params *handshake);
static int ssl_check_server_ecdh_params(const mbedtls_ssl_context *ssl);
static int ssl_parse_renegotiation_info(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len); // from ssl_cli.c
static int ssl_parse_max_fragment_length_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len); // from ssl_cli.c
static int ssl_parse_encrypt_then_mac_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len); // from ssl_cli.c
static int ssl_parse_extended_ms_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len); // from ssl_cli.c
static int ssl_parse_supported_point_formats_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len); // from ssl_cli.c
static int ssl_parse_alpn_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len); // from ssl_cli.c


static UTASKER_MBEDSSL_SESSION *secure_session = 0;

static int fnSecureRandomNumber(void *ptr, unsigned char *ptrBuf, size_t length)
{
    static unsigned char ucPoorRandomForTest = 0;
    while (length-- != 0) {
        *ptrBuf++ = 0x55;// ucPoorRandomForTest++;
    }
    return 0;
}

extern void fnEnterRandom(unsigned char *ucPrtData, int iServer)
{
    int iOffset;
    if (iServer != 0) {
        iOffset = 32;                                                    // server's random number occupies the second half of the session's random number buffer
    }
    else {
        iOffset = 0;                                                     // client's random number occupies the first half of the session's random number buffer
    }
    uMemcpy((secure_session->ssl.handshake->randbytes + iOffset), ucPrtData, 32);
}

extern unsigned char *fnInsertTLS_random(unsigned char *ptrData, unsigned long ulUTC)
{
    size_t Length;
    if (ulUTC != 0) {
        *ptrData++ = (unsigned char)(ulUTC >> 24);                       // insert UCT as first four bytes of the random number
        *ptrData++ = (unsigned char)(ulUTC >> 16);
        *ptrData++ = (unsigned char)(ulUTC >> 8);
        *ptrData++ = (unsigned char)(ulUTC);
        Length = 28;
    }
    else {
        Length = 32;
    }
    fnSecureRandomNumber(0, ptrData, Length);
    ptrData += Length;
    fnEnterRandom((ptrData - 32), 0);                                    // session's client random number
    return (ptrData);
}

// Deallocate used session memory
//
extern void fnTearDown(void)
{                                                                        // before starting 13024 still allocated - 17 holes - 100 blocks not deallocated still - 7116 in holes
    if (secure_session != 0) {
        mbedtls_x509_crt_free(&(secure_session->certificates));          // after call 9444 still allocated - 20 holes - 73 blocks not deallocated still - 10696 in holes
        mbedtls_x509_crt_free(&(secure_session->ourCertificate));        // unallocate all certificate data
        mbedtls_pk_free(&(secure_session->ourPrivateKey));               // unallocate private key context components
        mbedtls_ecdh_free(&(secure_session->edch));
        mbedtls_free(secure_session->ssl.out_ctr);
        mbedtls_free(secure_session->ssl.in_ctr);
        mbedtls_ssl_free(&(secure_session->ssl));
        mbedtls_ssl_config_free(&(secure_session->config));
        mbedtls_free(secure_session);                                    // free the session memory (1252)
        secure_session = 0;
    }
    // remaining state 28 bytes still allocated - 3 holes with 17028 - 3 blocks still allocated
}

// Enter calloc() and free() implementation to be used by mbedTLS
//
static void set_calloc_free(void * (*calloc_func)(size_t, size_t), void(*free_func)(void *))
{
    mbedtls_calloc = calloc_func;
    mbedtls_free = free_func;
}

// Notes - the handling expects that the x509 content is a null-terminated string!
//
extern int fnInitialiseSecureLayer(const unsigned char *ptrOurCertificate, unsigned long ulCertificateLength, const unsigned char *ptrOutPrivateKey, unsigned long ulOurPrivateKeyLength)
{
    int iReturn = 0;
    unsigned char *ptrTempString;
    if (secure_session != 0) {
        return -1;                                                       // session already created
    }
    set_calloc_free(uCalloc, uCFree);                                    // enter the calloc() / free() methods to be used
    secure_session = mbedtls_calloc(1, sizeof(UTASKER_MBEDSSL_SESSION)); // get the memory needed for the session (1252 bytes)
    if (ulOurPrivateKeyLength != 0) {                                    // if we have a private key
        ptrTempString = (unsigned char *)mbedtls_calloc(1, (ulOurPrivateKeyLength + 1));
        uMemcpy(ptrTempString, ptrOutPrivateKey, ulOurPrivateKeyLength); // copy the private key string
        ptrTempString[ulOurPrivateKeyLength++] = 0;                      // terminate the string, which is required for its further processing
        iReturn = mbedtls_pk_parse_key(&(secure_session->ourPrivateKey), ptrTempString, ulOurPrivateKeyLength, 0, 0); // no password - allocates 10 blocks of heap - total 2580 - 2 holes of 5624 total
        mbedtls_free(ptrTempString);
    }
    if (iReturn == 0) {
        if (ulCertificateLength != 0) {                                  // if we have a certificate
            ptrTempString = (unsigned char *)mbedtls_calloc(1, (ulCertificateLength + 1));
            uMemcpy(ptrTempString, ptrOurCertificate, ulCertificateLength); // copy the private key string
            ptrTempString[ulCertificateLength++] = 0;                    // terminate the string, which is required for its further processing
            iReturn = mbedtls_x509_crt_parse(&(secure_session->ourCertificate), ptrTempString, ulCertificateLength); // allocates 4 blocks of heap - total 3876 - 3 holes of 4328 total
            mbedtls_free(ptrTempString);
        }
        if (iReturn == 0) {
            secure_session->ssl.handshake = (mbedtls_ssl_handshake_params *)mbedtls_calloc(1, (sizeof(mbedtls_ssl_handshake_params))); // handshake parameters (744 bytes)
            ssl_handshake_params_init(secure_session->ssl.handshake);   // initialise the handshake parameters - allocates 1 block of heap - total 4620 - 3 holes of 3584 total
            secure_session->ssl.transform_negotiate = mbedtls_calloc(1, sizeof(mbedtls_ssl_transform)); // (208 bytes)
            secure_session->ssl.session_negotiate = mbedtls_calloc(1, sizeof(mbedtls_ssl_session)); // (108 bytes)

            mbedtls_ssl_conf_rng(&(secure_session->config), fnSecureRandomNumber, secure_session); // set the random number method
            secure_session->ssl.conf = &(secure_session->config);
            secure_session->config.endpoint = MBEDTLS_SSL_IS_CLIENT;     // we are client

#if defined(MBEDTLS_ECP_C)
            secure_session->config.curve_list = mbedtls_ecp_grp_id_list();
#endif
            secure_session->ssl.out_ctr = (unsigned char *)mbedtls_calloc(1, 8); // 64 bit counter used by transmitter
            secure_session->ssl.in_ctr  = (unsigned char *)mbedtls_calloc(1, 8); // 64 bit counter used by receiver
            if ((ulCertificateLength != 0) && (ulOurPrivateKeyLength != 0)) {
                iReturn = mbedtls_ssl_conf_own_cert(&(secure_session->config), &(secure_session->ourCertificate), &(secure_session->ourPrivateKey)); // attach the client certificate and private key to the configuration - allocates 1 block of heap - total 4964 - 3 holes of 3240 total
            }
        }
    }
    return iReturn;
}

extern void fnHandshakeStats(unsigned long ulHandshakeSize, unsigned char *ucPrtData)
{
    secure_session->ssl.in_msg = (ucPrtData - 4);
    secure_session->ssl.in_hslen = (ulHandshakeSize + 4);
    mbedtls_ssl_update_handshake_status(&(secure_session->ssl));
}

#if defined(MBEDTLS_SSL_PROTO_SSL3)
    #define SSL_MAX_HASH_LEN 36
#else
    #define SSL_MAX_HASH_LEN 12
#endif

extern void fnSwitchTransformSpec(void)
{
    unsigned char buf[SSL_MAX_HASH_LEN];
    secure_session->ssl.transform_in = secure_session->ssl.transform_negotiate;
    secure_session->ssl.session_in = secure_session->ssl.session_negotiate;
    memset(secure_session->ssl.in_ctr, 0, 8);                            // reset the counter
    secure_session->ssl.handshake->calc_finished(&(secure_session->ssl), buf, (secure_session->ssl.conf->endpoint ^ 1)); //  - allocates (?) to 100 block of heap - total 13024 - 17 holes of 7116 total

}

// During the server hello we receive the cipher to be used during the session
//
extern void fnSetSessionCipher(unsigned short session_cipher, unsigned char ucVersion[2], unsigned char ucIdLength, unsigned char *ptrID)
{
    secure_session->ssl.major_ver = ucVersion[0];
    secure_session->ssl.minor_ver = ucVersion[1];
    secure_session->ssl.session_negotiate->ciphersuite = session_cipher;
    secure_session->ssl.session_negotiate->id_len = ucIdLength;
    uMemcpy(secure_session->ssl.session_negotiate->id, ptrID, ucIdLength);
    secure_session->ssl.transform_negotiate->ciphersuite_info = mbedtls_ssl_ciphersuite_from_id(session_cipher);
    mbedtls_ssl_optimize_checksum(&(secure_session->ssl), secure_session->ssl.transform_negotiate->ciphersuite_info);
    secure_session->ssl.handshake->ecdh_ctx.point_format = MBEDTLS_ECP_PF_UNCOMPRESSED; // only uncompressed supported
}

// This is used to check and save received certificates
//
extern int fnSaveServerCertificate(unsigned char *ptrCertificate, unsigned long ulCertificateLength)
{
    return (mbedtls_x509_crt_parse_der(&(secure_session->certificates), ptrCertificate, ulCertificateLength)); //  - allocates (?) to 35 block of heap - total 8952 - 2 holes of 2340 total
}

//When the server requests our certificate it defines also the method of passing it (although the supported algorithms are ignored....)
//
extern int fnPrepareCertificate(unsigned char *ptrData, unsigned short usSignatureHashAlgorithmsLength)
{
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info = secure_session->ssl.transform_negotiate->ciphersuite_info;
    secure_session->ssl.client_auth = 1;                                 // mark that we need to return the certificate as part of the handshake procedure
    if (0 == mbedtls_ssl_ciphersuite_cert_req_allowed(ciphersuite_info)) {
        return(-1);
    }
    return 0;
}

extern int fnExtractPublicKey(unsigned char *ptrData, unsigned long ulKeyExchangeLength)
{
    int iReturn = mbedtls_ecdh_read_params(&(secure_session->ssl.handshake->ecdh_ctx), (const unsigned char **)&ptrData, (const unsigned char *)(ptrData + ulKeyExchangeLength)); // - allocates (?) to 50 block of heap - total 8612 - 3 holes of 1572 total
    if (iReturn == 0) {
        iReturn = ssl_check_server_ecdh_params(&secure_session->ssl);    // check server key exchange message (ECDHE curve)
    }
    return iReturn;
}

extern unsigned char *fnGeneratePublicKey(unsigned char *ptrData)
{
    int iReturn;
    size_t output_length;
    iReturn = mbedtls_ecp_gen_keypair_base(&(secure_session->ssl.handshake->ecdh_ctx.grp), &(secure_session->ssl.handshake->ecdh_ctx.grp.G), &(secure_session->ssl.handshake->ecdh_ctx.d), &(secure_session->ssl.handshake->ecdh_ctx.Q), secure_session->config.f_rng, secure_session->config.p_rng); // generate public key [ecp.c] - allocates (?) to 88 block of heap - total 10440 - 14 holes of 1748 total
    if (iReturn == 0) {
        iReturn = mbedtls_ecp_point_write_binary(&(secure_session->ssl.handshake->ecdh_ctx.grp), &(secure_session->ssl.handshake->ecdh_ctx.Q), secure_session->ssl.handshake->ecdh_ctx.point_format, &output_length, ptrData, 1000); // export a point into unsigned binary data [ecp.c]
        if (iReturn == 0) {
            ptrData += output_length;
            iReturn = mbedtls_ecdh_calc_secret(&secure_session->ssl.handshake->ecdh_ctx, &(secure_session->ssl.handshake->pmslen), secure_session->ssl.handshake->premaster, MBEDTLS_MPI_MAX_SIZE, secure_session->config.f_rng, secure_session->config.p_rng); //  - allocates (?) to 89 block of heap - total 10472 - 13 holes of 1716 total
        }
    }
    if (iReturn != 0) {
        return 0;
    }
    return ptrData;
}


extern int fnHandleSecurityExtension(unsigned short ext_id, unsigned short ext_size, unsigned char *ptrExtensionData)
{
    int ret;
    mbedtls_ssl_context *ssl = &(secure_session->ssl);
    switch (ext_id)
    {
    case MBEDTLS_TLS_EXT_RENEGOTIATION_INFO:
    //    MBEDTLS_SSL_DEBUG_MSG(3, ("found renegotiation extension"));
#if defined(MBEDTLS_SSL_RENEGOTIATION)
        renegotiation_info_seen = 1;
#endif
        if ((ret = ssl_parse_renegotiation_info(ssl, ptrExtensionData, ext_size)) != 0) {
            return(ret);
        }
        break;

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    case MBEDTLS_TLS_EXT_MAX_FRAGMENT_LENGTH:
   //     MBEDTLS_SSL_DEBUG_MSG(3, ("found max_fragment_length extension"));
        if ((ret = ssl_parse_max_fragment_length_ext(ssl, ptrExtensionData, ext_size)) != 0) {
            return(ret);
        }
        break;
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_SSL_TRUNCATED_HMAC)
    case MBEDTLS_TLS_EXT_TRUNCATED_HMAC:
    //    MBEDTLS_SSL_DEBUG_MSG(3, ("found truncated_hmac extension"));

        if ((ret = ssl_parse_truncated_hmac_ext(ssl,
            ptrExtensionData, ext_size)) != 0)
        {
            return(ret);
        }

        break;
#endif /* MBEDTLS_SSL_TRUNCATED_HMAC */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    case MBEDTLS_TLS_EXT_ENCRYPT_THEN_MAC:
    //    MBEDTLS_SSL_DEBUG_MSG(3, ("found encrypt_then_mac extension"));

        if ((ret = ssl_parse_encrypt_then_mac_ext(ssl, ptrExtensionData, ext_size)) != 0) {
            return(ret);
        }
        break;
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    case MBEDTLS_TLS_EXT_EXTENDED_MASTER_SECRET:
    //    MBEDTLS_SSL_DEBUG_MSG(3, ("found extended_master_secret extension"));

        if ((ret = ssl_parse_extended_ms_ext(ssl, ptrExtensionData, ext_size)) != 0) {
            return(ret);
        }
        break;
#endif /* MBEDTLS_SSL_EXTENDED_MASTER_SECRET */

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    case MBEDTLS_TLS_EXT_SESSION_TICKET:
    //    MBEDTLS_SSL_DEBUG_MSG(3, ("found session_ticket extension"));

        if ((ret = ssl_parse_session_ticket_ext(ssl,
            ptrExtensionData, ext_size)) != 0)
        {
            return(ret);
        }

        break;
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#if defined(MBEDTLS_ECDH_C) || defined(MBEDTLS_ECDSA_C) || \
    defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    case MBEDTLS_TLS_EXT_SUPPORTED_POINT_FORMATS:
   //     MBEDTLS_SSL_DEBUG_MSG(3, ("found supported_point_formats extension"));

        if ((ret = ssl_parse_supported_point_formats_ext(ssl, ptrExtensionData, ext_size)) != 0) {
            return(ret);
        }
        break;
#endif /* MBEDTLS_ECDH_C || MBEDTLS_ECDSA_C ||
            MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    case MBEDTLS_TLS_EXT_ECJPAKE_KKPP:
    //    MBEDTLS_SSL_DEBUG_MSG(3, ("found ecjpake_kkpp extension"));

        if ((ret = ssl_parse_ecjpake_kkpp(ssl,
            ptrExtensionData, ext_size)) != 0)
        {
            return(ret);
        }

        break;
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_SSL_ALPN)
    case MBEDTLS_TLS_EXT_ALPN:
   //     MBEDTLS_SSL_DEBUG_MSG(3, ("found alpn extension"));

        if ((ret = ssl_parse_alpn_ext(ssl, ptrExtensionData, ext_size)) != 0) {
            return(ret);
        }
        break;
#endif /* MBEDTLS_SSL_ALPN */

    default:
       // MBEDTLS_SSL_DEBUG_MSG(3, ("unknown extension found: %d (ignoring)",
         //   ext_id));
        return -1;
    }
    return 0;
}


extern unsigned char *fnInsertSignatureAlgorithm(unsigned char *ptrData)
{
#if !defined(MBEDTLS_KEY_EXCHANGE_RSA_ENABLED)       && \
    !defined(MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED)   && \
    !defined(MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED)  && \
    !defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED) && \
    !defined(MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)&& \
    !defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
        const mbedtls_ssl_ciphersuite_t *ciphersuite_info =
            ssl->transform_negotiate->ciphersuite_info;
        int ret;

        MBEDTLS_SSL_DEBUG_MSG(2, ("=> write certificate verify"));

        if ((ret = mbedtls_ssl_derive_keys(ssl)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_derive_keys", ret);
            return(ret);
        }

        if (ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDHE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_DHE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECJPAKE)
        {
            MBEDTLS_SSL_DEBUG_MSG(2, ("<= skip write certificate verify"));
            ssl->state++;
            return(0);
        }

        MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
        return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
    }
#else
        int ret = MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
        const mbedtls_ssl_ciphersuite_t *ciphersuite_info = secure_session->ssl.transform_negotiate->ciphersuite_info;
        size_t n = 0;// , offset = 0;
        unsigned char hash[48];
        unsigned char *hash_start = hash;
        mbedtls_md_type_t md_alg = MBEDTLS_MD_NONE;
        unsigned int hashlen;

//      MBEDTLS_SSL_DEBUG_MSG(2, ("=> write certificate verify"));

        if ((ret = mbedtls_ssl_derive_keys(&secure_session->ssl)) != 0) { //  - allocates (?) to 95 block of heap - total 11472 - 12 holes of 716 total
//            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_derive_keys", ret);
            //return(ret);
            return 0;
        }

        if (ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECDHE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_DHE_PSK ||
            ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_ECJPAKE)
        {
    //        MBEDTLS_SSL_DEBUG_MSG(2, ("<= skip write certificate verify"));
            //ssl->state++;
            return(ptrData);
        }

        if (secure_session->ssl.client_auth == 0 || mbedtls_ssl_own_cert(&secure_session->ssl) == NULL) {
        //    MBEDTLS_SSL_DEBUG_MSG(2, ("<= skip write certificate verify"));
            //ssl->state++;
            return(ptrData);
        }

        if (mbedtls_ssl_own_key(&secure_session->ssl) == NULL) {
   //         MBEDTLS_SSL_DEBUG_MSG(1, ("got no private key for certificate"));
           // return(MBEDTLS_ERR_SSL_PRIVATE_KEY_REQUIRED);
            return 0;
        }

        /*
        * Make an RSA signature of the handshake digests
        */
        secure_session->ssl.handshake->calc_verify(&secure_session->ssl, hash);

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
        if (ssl->minor_ver != MBEDTLS_SSL_MINOR_VERSION_3)
        {
            /*
            * digitally-signed struct {
            *     opaque md5_hash[16];
            *     opaque sha_hash[20];
            * };
            *
            * md5_hash
            *     MD5(handshake_messages);
            *
            * sha_hash
            *     SHA(handshake_messages);
            */
            hashlen = 36;
            md_alg = MBEDTLS_MD_NONE;

            /*
            * For ECDSA, default hash is SHA-1 only
            */
            if (mbedtls_pk_can_do(mbedtls_ssl_own_key(ssl), MBEDTLS_PK_ECDSA))
            {
                hash_start += 16;
                hashlen -= 16;
                md_alg = MBEDTLS_MD_SHA1;
            }
        }
        else
#endif /* MBEDTLS_SSL_PROTO_SSL3 || MBEDTLS_SSL_PROTO_TLS1 || \
              MBEDTLS_SSL_PROTO_TLS1_1 */
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
            if (secure_session->ssl.minor_ver == MBEDTLS_SSL_MINOR_VERSION_3) {
                /*
                * digitally-signed struct {
                *     opaque handshake_messages[handshake_messages_length];
                * };
                *
                * Taking shortcut here. We assume that the server always allows the
                * PRF Hash function and has sent it in the allowed signature
                * algorithms list received in the Certificate Request message.
                *
                * Until we encounter a server that does not, we will take this
                * shortcut.
                *
                * Reason: Otherwise we should have running hashes for SHA512 and SHA224
                *         in order to satisfy 'weird' needs from the server side.
                */
                if (secure_session->ssl.transform_negotiate->ciphersuite_info->mac == MBEDTLS_MD_SHA384) {
                    md_alg = MBEDTLS_MD_SHA384;
                    *ptrData++ = MBEDTLS_SSL_HASH_SHA384;
                }
                else {
                    md_alg = MBEDTLS_MD_SHA256;
                    *ptrData++ = MBEDTLS_SSL_HASH_SHA256;
                }
                *ptrData++ = mbedtls_ssl_sig_from_pk(mbedtls_ssl_own_key(&secure_session->ssl));

                /* Info from md_alg will be used instead */
                hashlen = 0;
                //offset = 2;
            }
            else
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
            {
//                MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
               // return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
                return 0;
            }

        if ((ret = mbedtls_pk_sign(mbedtls_ssl_own_key(&secure_session->ssl), md_alg, hash_start, hashlen, (ptrData + 2), &n, secure_session->ssl.conf->f_rng, secure_session->ssl.conf->p_rng)) != 0) { //  - allocates (?) to 100 block of heap - total 13024 - 17 holes of 7116 total
      //      MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_pk_sign", ret);
           // return(ret);
            return 0;
        }
        *ptrData++ = (unsigned char)(n >> 8);
        *ptrData++ = (unsigned char)(n);
        ptrData += n;
        return ptrData;
    }
#endif /* !MBEDTLS_KEY_EXCHANGE_RSA_ENABLED &&
        !MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED &&
        !MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED &&
        !MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED &&
        !MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED &&
        !MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED */



#if defined(MBEDTLS_ARC4_C) || defined(MBEDTLS_CIPHER_NULL_CIPHER) ||     \
    ( defined(MBEDTLS_CIPHER_MODE_CBC) &&                                  \
      ( defined(MBEDTLS_AES_C) || defined(MBEDTLS_CAMELLIA_C) ) )
#define SSL_SOME_MODES_USE_MAC
#endif

    /* Length of the "epoch" field in the record header */
static inline size_t ssl_ep_len(const mbedtls_ssl_context *ssl)
{
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM)
        return(2);
#else
    ((void)ssl);
#endif
    return(0);
}

static void fnIncCounter(mbedtls_ssl_context *ssl, unsigned char *ctr)   // increment a 64 bit counter (8 bytes)
{
    size_t i;
    for (i = 8; i > ssl_ep_len(ssl); i--) {
        if (++ctr[i - 1] != 0) {
            break;
        }
    }
}

extern unsigned char *fnEncrypt(unsigned char *ptrData, unsigned char *ptrInputData, unsigned long ulLength)
{
    mbedtls_cipher_mode_t mode;
    int auth_done = 0;
    mbedtls_ssl_context *ssl = &(secure_session->ssl);

    if (ptrInputData != 0) {                                             // if the content hasn't been prepared yet
        secure_session->ssl.out_hdr = (ptrData - 5);
        secure_session->ssl.out_len = (ptrData - 2);
        secure_session->ssl.out_iv = (ptrData);

        ssl->out_msglen = ulLength;

        /*
        * Set the out_msg pointer to the correct location based on IV length
        */
        if (secure_session->ssl.minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2) {
            secure_session->ssl.out_msg = (secure_session->ssl.out_iv + secure_session->ssl.transform_negotiate->ivlen - secure_session->ssl.transform_negotiate->fixed_ivlen);
        }
        else {
            secure_session->ssl.out_msg = secure_session->ssl.out_iv;
        }
    }

    mode = mbedtls_cipher_get_cipher_mode( &ssl->transform_out->cipher_ctx_enc );

  //  MBEDTLS_SSL_DEBUG_BUF( 4, "before encrypt: output payload",
    //                  ssl->out_msg, ssl->out_msglen );

    /*
     * Add MAC before if needed
     */
#if defined(SSL_SOME_MODES_USE_MAC)
    if (mode == MBEDTLS_MODE_STREAM || ( mode == MBEDTLS_MODE_CBC
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
          && ssl->session_out->encrypt_then_mac == MBEDTLS_SSL_ETM_DISABLED
#endif
        ) ) {
#if defined(MBEDTLS_SSL_PROTO_SSL3)
        if( ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            ssl_mac( &ssl->transform_out->md_ctx_enc,
                      ssl->transform_out->mac_enc,
                      ssl->out_msg, ssl->out_msglen,
                      ssl->out_ctr, ssl->out_msgtype );
        }
        else
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
        defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if (ssl->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_1) {
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_ctr, 8 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_hdr, 3 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_len, 2 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_msg, ssl->out_msglen );
            mbedtls_md_hmac_finish( &ssl->transform_out->md_ctx_enc, ssl->out_msg + ssl->out_msglen );
            mbedtls_md_hmac_reset( &ssl->transform_out->md_ctx_enc );
            fnIncCounter(ssl, ssl->out_ctr);                             // increment the output counter
        }
        else
#endif
        {
//            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( 0 /*MBEDTLS_ERR_SSL_INTERNAL_ERROR*/ );
        }
        ssl->out_msglen += ssl->transform_out->maclen;
        auth_done++;
    }
#endif /* AEAD not the only option */

    /*
     * Encrypt
     */
#if defined(MBEDTLS_ARC4_C) || defined(MBEDTLS_CIPHER_NULL_CIPHER)
    if( mode == MBEDTLS_MODE_STREAM )
    {
        int ret;
        size_t olen = 0;

        MBEDTLS_SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                            "including %d bytes of padding",
                       ssl->out_msglen, 0 ) );

        if( ( ret = mbedtls_cipher_crypt( &ssl->transform_out->cipher_ctx_enc,
                                   ssl->transform_out->iv_enc,
                                   ssl->transform_out->ivlen,
                                   ssl->out_msg, ssl->out_msglen,
                                   ssl->out_msg, &olen ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_crypt", ret );
            return( ret );
        }

        if( ssl->out_msglen != olen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }
    }
    else
#endif /* MBEDTLS_ARC4_C || MBEDTLS_CIPHER_NULL_CIPHER */
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CCM_C)
    if( mode == MBEDTLS_MODE_GCM ||
        mode == MBEDTLS_MODE_CCM ) {
        int ret;
        size_t enc_msglen, olen;
        unsigned char *enc_msg;
        unsigned char add_data[13];
        unsigned char taglen = ssl->transform_out->ciphersuite_info->flags &
                               MBEDTLS_CIPHERSUITE_SHORT_TAG ? 8 : 16;

        memcpy( add_data, ssl->out_ctr, 8 );
        add_data[8]  = ssl->out_msgtype;
        mbedtls_ssl_write_version( ssl->major_ver, ssl->minor_ver,
                           ssl->conf->transport, add_data + 9 );
        add_data[11] = ( ssl->out_msglen >> 8 ) & 0xFF;
        add_data[12] = ssl->out_msglen & 0xFF;

 //       MBEDTLS_SSL_DEBUG_BUF( 4, "additional data used for AEAD",
   //                    add_data, 13 );

        /*
         * Generate IV
         */
        if( ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen != 8 ) {
            /* Reminder if we ever add an AEAD mode with a different size */
         //   MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( 0/*MBEDTLS_ERR_SSL_INTERNAL_ERROR*/ );
        }

        memcpy( ssl->transform_out->iv_enc + ssl->transform_out->fixed_ivlen,
                             ssl->out_ctr, 8 );
        memcpy( ssl->out_iv, ssl->out_ctr, 8 );

   //     MBEDTLS_SSL_DEBUG_BUF( 4, "IV used", ssl->out_iv,
     //           ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen );

        /*
         * Fix pointer positions and message length with added IV
         */
        enc_msg = ssl->out_msg;
        enc_msglen = ssl->out_msglen;
        ssl->out_msglen += ssl->transform_out->ivlen -
                           ssl->transform_out->fixed_ivlen;

      //  MBEDTLS_SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
        //                    "including %d bytes of padding",
          //             ssl->out_msglen, 0 ) );

        /*
         * Encrypt and authenticate
         */
        if( ( ret = mbedtls_cipher_auth_encrypt( &ssl->transform_out->cipher_ctx_enc,
                                         ssl->transform_out->iv_enc,
                                         ssl->transform_out->ivlen,
                                         add_data, 13,
                                         enc_msg, enc_msglen,
                                         enc_msg, &olen,
                                         enc_msg + enc_msglen, taglen ) ) != 0 ) {
         //   MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_auth_encrypt", ret );
            return( 0 );
        }

        if (olen != enc_msglen) {
     //       MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( 0/*MBEDTLS_ERR_SSL_INTERNAL_ERROR*/ );
        }

        ssl->out_msglen += taglen;
        auth_done++;

     //   MBEDTLS_SSL_DEBUG_BUF( 4, "after encrypt: tag", enc_msg + enc_msglen, taglen );
    }
    else
#endif /* MBEDTLS_GCM_C || MBEDTLS_CCM_C */
#if defined(MBEDTLS_CIPHER_MODE_CBC) &&                                    \
    ( defined(MBEDTLS_AES_C) || defined(MBEDTLS_CAMELLIA_C) )
    if( mode == MBEDTLS_MODE_CBC )
    {
        int ret;
        unsigned char *enc_msg;
        size_t enc_msglen, padlen, olen = 0, i;

        padlen = ssl->transform_out->ivlen - ( ssl->out_msglen + 1 ) %
                 ssl->transform_out->ivlen;
        if( padlen == ssl->transform_out->ivlen )
            padlen = 0;

        for( i = 0; i <= padlen; i++ )
            ssl->out_msg[ssl->out_msglen + i] = (unsigned char) padlen;

        ssl->out_msglen += padlen + 1;

        enc_msglen = ssl->out_msglen;
        enc_msg = ssl->out_msg;

#if defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2)
        /*
         * Prepend per-record IV for block cipher in TLS v1.1 and up as per
         * Method 1 (6.2.3.2. in RFC4346 and RFC5246)
         */
        if( ssl->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            /*
             * Generate IV
             */
            ret = ssl->conf->f_rng( ssl->conf->p_rng, ssl->transform_out->iv_enc, ssl->transform_out->ivlen );
            if (ret != 0) {
                return(0);
            }
            uMemcpy(ssl->out_iv, ssl->transform_out->iv_enc, ssl->transform_out->ivlen );

            /*
             * Fix pointer positions and message length with added IV
             */
            enc_msg = ssl->out_msg;
            enc_msglen = ssl->out_msglen;
            ssl->out_msglen += ssl->transform_out->ivlen;
        }
#endif /* MBEDTLS_SSL_PROTO_TLS1_1 || MBEDTLS_SSL_PROTO_TLS1_2 */

 //       MBEDTLS_SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
   //                         "including %d bytes of IV and %d bytes of padding",
     //                       ssl->out_msglen, ssl->transform_out->ivlen,
       //                     padlen + 1 ) );

        if( ( ret = mbedtls_cipher_crypt( &ssl->transform_out->cipher_ctx_enc,
                                   ssl->transform_out->iv_enc,
                                   ssl->transform_out->ivlen,
                                   enc_msg, enc_msglen,
                                   enc_msg, &olen ) ) != 0 )
        {
 //           MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_crypt", ret );
            return( 0 );
        }

        if( enc_msglen != olen ) {
      //      MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( 0 /*MBEDTLS_ERR_SSL_INTERNAL_ERROR */);
        }

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1)
        if( ssl->minor_ver < MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            /*
             * Save IV in SSL3 and TLS1
             */
            memcpy( ssl->transform_out->iv_enc,
                    ssl->transform_out->cipher_ctx_enc.iv,
                    ssl->transform_out->ivlen );
        }
#endif

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
        if( auth_done == 0 )
        {
            /*
             * MAC(MAC_write_key, seq_num +
             *     TLSCipherText.type +
             *     TLSCipherText.version +
             *     length_of( (IV +) ENC(...) ) +
             *     IV + // except for TLS 1.0
             *     ENC(content + padding + padding_length));
             */
            unsigned char pseudo_hdr[13];

 //           MBEDTLS_SSL_DEBUG_MSG( 3, ( "using encrypt then mac" ) );

            memcpy( pseudo_hdr +  0, ssl->out_ctr, 8 );
            memcpy( pseudo_hdr +  8, ssl->out_hdr, 3 );             // ???????????????????????????????
            pseudo_hdr[11] = (unsigned char)( ( ssl->out_msglen >> 8 ) & 0xFF );
            pseudo_hdr[12] = (unsigned char)( ( ssl->out_msglen      ) & 0xFF );

         //   MBEDTLS_SSL_DEBUG_BUF( 4, "MAC'd meta-data", pseudo_hdr, 13 );

            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, pseudo_hdr, 13 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc,
                             ssl->out_iv, ssl->out_msglen );
            mbedtls_md_hmac_finish( &ssl->transform_out->md_ctx_enc,
                             ssl->out_iv + ssl->out_msglen );
            mbedtls_md_hmac_reset( &ssl->transform_out->md_ctx_enc );

            ssl->out_msglen += ssl->transform_out->maclen;
            auth_done++;
        }
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */
    }
    else
#endif /* MBEDTLS_CIPHER_MODE_CBC &&
          ( MBEDTLS_AES_C || MBEDTLS_CAMELLIA_C ) */
    {
  //      MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( 0/*MBEDTLS_ERR_SSL_INTERNAL_ERROR*/ );
    }

    /* Make extra sure authentication was performed, exactly once */
    if( auth_done != 1 )
    {
   //     MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return(0/* MBEDTLS_ERR_SSL_INTERNAL_ERROR */);
    }

  //  MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= encrypt buf" ) );

    return(ptrData + ssl->out_msglen);
}



extern int fnDecrypt(unsigned char **ptrptrInput, unsigned long *ptr_ulLength)
{
    mbedtls_ssl_context *ssl = &(secure_session->ssl);
    size_t i;
    mbedtls_cipher_mode_t mode;
    int auth_done = 0;
    unsigned char *ptrInput = *ptrptrInput;
#if defined(SSL_SOME_MODES_USE_MAC)
    size_t padlen = 0, correct = 1;
#endif

    ssl->in_msglen = *ptr_ulLength;
    ssl->in_hdr = (ptrInput - 5); // warning - this assumes that the record header is in the buffer!!!!!
    ssl->in_len = (ssl->in_hdr + 3);
    ssl->in_iv = ptrInput;
    if (secure_session->ssl.minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2) {
        secure_session->ssl.in_msg = (secure_session->ssl.in_iv + secure_session->ssl.transform_negotiate->ivlen - secure_session->ssl.transform_negotiate->fixed_ivlen);
    }
    else {
        secure_session->ssl.in_msg = secure_session->ssl.in_iv;
    }

    if (ssl->session_in == NULL || ssl->transform_in == NULL) {
   //     MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
        return (MBEDTLS_ERR_SSL_INTERNAL_ERROR);
    }

    mode = mbedtls_cipher_get_cipher_mode(&ssl->transform_in->cipher_ctx_dec);

    if (ssl->in_msglen < ssl->transform_in->minlen) {
//        MBEDTLS_SSL_DEBUG_MSG(1, ("in_msglen (%d) < minlen (%d)",
        //    ssl->in_msglen, ssl->transform_in->minlen));
        return(MBEDTLS_ERR_SSL_INVALID_MAC);
    }

#if defined(MBEDTLS_ARC4_C) || defined(MBEDTLS_CIPHER_NULL_CIPHER)
    if (mode == MBEDTLS_MODE_STREAM)
    {
        int ret;
        size_t olen = 0;

        padlen = 0;

        if ((ret = mbedtls_cipher_crypt(&ssl->transform_in->cipher_ctx_dec,
            ssl->transform_in->iv_dec,
            ssl->transform_in->ivlen,
            ssl->in_msg, ssl->in_msglen,
            ssl->in_msg, &olen)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_crypt", ret);
            return(ret);
        }

        if (ssl->in_msglen != olen)
        {
            MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
            return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
        }
    }
    else
#endif /* MBEDTLS_ARC4_C || MBEDTLS_CIPHER_NULL_CIPHER */
#if defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CCM_C)
        if (mode == MBEDTLS_MODE_GCM ||
            mode == MBEDTLS_MODE_CCM) {
            int ret;
            size_t dec_msglen, olen;
            unsigned char *dec_msg;
            unsigned char *dec_msg_result;
            unsigned char add_data[13];
            unsigned char taglen = ssl->transform_in->ciphersuite_info->flags &
                MBEDTLS_CIPHERSUITE_SHORT_TAG ? 8 : 16;
            size_t explicit_iv_len = ssl->transform_in->ivlen -
                ssl->transform_in->fixed_ivlen;

            if (ssl->in_msglen < explicit_iv_len + taglen)
            {
//                MBEDTLS_SSL_DEBUG_MSG(1, ("msglen (%d) < explicit_iv_len (%d) "
            //        "+ taglen (%d)", ssl->in_msglen,
            //        explicit_iv_len, taglen));
                return(MBEDTLS_ERR_SSL_INVALID_MAC);
            }
            dec_msglen = ssl->in_msglen - explicit_iv_len - taglen;

            dec_msg = ssl->in_msg;
            dec_msg_result = ssl->in_msg;
            ssl->in_msglen = dec_msglen;

            memcpy(add_data, ssl->in_ctr, 8);
            add_data[8] = ssl->in_msgtype;
            mbedtls_ssl_write_version(ssl->major_ver, ssl->minor_ver,
                ssl->conf->transport, add_data + 9);
            add_data[11] = (ssl->in_msglen >> 8) & 0xFF;
            add_data[12] = ssl->in_msglen & 0xFF;

//            MBEDTLS_SSL_DEBUG_BUF(4, "additional data used for AEAD",
  //              add_data, 13);

            memcpy(ssl->transform_in->iv_dec + ssl->transform_in->fixed_ivlen,
                ssl->in_iv,
                ssl->transform_in->ivlen - ssl->transform_in->fixed_ivlen);

//            MBEDTLS_SSL_DEBUG_BUF(4, "IV used", ssl->transform_in->iv_dec,
//                ssl->transform_in->ivlen);
//            MBEDTLS_SSL_DEBUG_BUF(4, "TAG used", dec_msg + dec_msglen, taglen);

            /*
            * Decrypt and authenticate
            */
            if ((ret = mbedtls_cipher_auth_decrypt(&ssl->transform_in->cipher_ctx_dec,
                ssl->transform_in->iv_dec,
                ssl->transform_in->ivlen,
                add_data, 13,
                dec_msg, dec_msglen,
                dec_msg_result, &olen,
                dec_msg + dec_msglen, taglen)) != 0)
            {
//                MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_auth_decrypt", ret);

                if (ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED)
                    return(MBEDTLS_ERR_SSL_INVALID_MAC);

                return(ret);
            }
            auth_done++;

            if (olen != dec_msglen)
            {
//                MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
                return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
            }
        }
        else
#endif /* MBEDTLS_GCM_C || MBEDTLS_CCM_C */
#if defined(MBEDTLS_CIPHER_MODE_CBC) &&                                    \
    ( defined(MBEDTLS_AES_C) || defined(MBEDTLS_CAMELLIA_C) )
            if (mode == MBEDTLS_MODE_CBC)
            {
                /*
                * Decrypt and check the padding
                */
                int ret;
                unsigned char *dec_msg;
                unsigned char *dec_msg_result;
                size_t dec_msglen;
                size_t minlen = 0;
                size_t olen = 0;

                /*
                * Check immediate ciphertext sanity
                */
#if defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2)
                if (ssl->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2)
                    minlen += ssl->transform_in->ivlen;
#endif

                if (ssl->in_msglen < minlen + ssl->transform_in->ivlen ||
                    ssl->in_msglen < minlen + ssl->transform_in->maclen + 1)
                {
//                    MBEDTLS_SSL_DEBUG_MSG(1, ("msglen (%d) < max( ivlen(%d), maclen (%d) "
      //                  "+ 1 ) ( + expl IV )", ssl->in_msglen,
         //               ssl->transform_in->ivlen,
            //            ssl->transform_in->maclen));
                    return(MBEDTLS_ERR_SSL_INVALID_MAC);
                }

                dec_msglen = ssl->in_msglen;
                dec_msg = ssl->in_msg;
                dec_msg_result = ssl->in_msg;

                /*
                * Authenticate before decrypt if enabled
                */
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
                if (ssl->session_in->encrypt_then_mac == MBEDTLS_SSL_ETM_ENABLED)
                {
#define SSL_MAX_MAC_SIZE   48
                    unsigned char computed_mac[SSL_MAX_MAC_SIZE];
                    unsigned char pseudo_hdr[13];

//                    MBEDTLS_SSL_DEBUG_MSG(3, ("using encrypt then mac"));

                    dec_msglen -= ssl->transform_in->maclen;
                    ssl->in_msglen -= ssl->transform_in->maclen;

                    memcpy(pseudo_hdr + 0, ssl->in_ctr, 8);
                    memcpy(pseudo_hdr + 8, ssl->in_hdr, 3);
                    pseudo_hdr[11] = (unsigned char)((ssl->in_msglen >> 8) & 0xFF);
                    pseudo_hdr[12] = (unsigned char)((ssl->in_msglen) & 0xFF);

//                    MBEDTLS_SSL_DEBUG_BUF(4, "MAC'd meta-data", pseudo_hdr, 13);

                    mbedtls_md_hmac_update(&ssl->transform_in->md_ctx_dec, pseudo_hdr, 13);
                    mbedtls_md_hmac_update(&ssl->transform_in->md_ctx_dec,
                        ssl->in_iv, ssl->in_msglen);
                    mbedtls_md_hmac_finish(&ssl->transform_in->md_ctx_dec, computed_mac);
                    mbedtls_md_hmac_reset(&ssl->transform_in->md_ctx_dec);

//                    MBEDTLS_SSL_DEBUG_BUF(4, "message  mac", ssl->in_iv + ssl->in_msglen,
               //         ssl->transform_in->maclen);
//                    MBEDTLS_SSL_DEBUG_BUF(4, "computed mac", computed_mac,
           //             ssl->transform_in->maclen);

                    if (mbedtls_ssl_safer_memcmp(ssl->in_iv + ssl->in_msglen, computed_mac,
                        ssl->transform_in->maclen) != 0)
                    {
//                        MBEDTLS_SSL_DEBUG_MSG(1, ("message mac does not match"));

                        return(MBEDTLS_ERR_SSL_INVALID_MAC);
                    }
                    auth_done++;
                }
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */

                /*
                * Check length sanity
                */
                if (ssl->in_msglen % ssl->transform_in->ivlen != 0)
                {
//                    MBEDTLS_SSL_DEBUG_MSG(1, ("msglen (%d) %% ivlen (%d) != 0",
      //                  ssl->in_msglen, ssl->transform_in->ivlen));
                    return(MBEDTLS_ERR_SSL_INVALID_MAC);
                }

#if defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2)
                /*
                * Initialize for prepended IV for block cipher in TLS v1.1 and up
                */
                if (ssl->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2)
                {
                    dec_msglen -= ssl->transform_in->ivlen;
                    ssl->in_msglen -= ssl->transform_in->ivlen;

                    for (i = 0; i < ssl->transform_in->ivlen; i++)
                        ssl->transform_in->iv_dec[i] = ssl->in_iv[i];
                }
#endif /* MBEDTLS_SSL_PROTO_TLS1_1 || MBEDTLS_SSL_PROTO_TLS1_2 */

                if ((ret = mbedtls_cipher_crypt(&ssl->transform_in->cipher_ctx_dec,
                    ssl->transform_in->iv_dec,
                    ssl->transform_in->ivlen,
                    dec_msg, dec_msglen,
                    dec_msg_result, &olen)) != 0)
                {
//                    MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_crypt", ret);
                    return(ret);
                }

                if (dec_msglen != olen)
                {
//                    MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
                    return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
                }

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1)
                if (ssl->minor_ver < MBEDTLS_SSL_MINOR_VERSION_2)
                {
                    /*
                    * Save IV in SSL3 and TLS1
                    */
                    memcpy(ssl->transform_in->iv_dec,
                        ssl->transform_in->cipher_ctx_dec.iv,
                        ssl->transform_in->ivlen);
                }
#endif

                padlen = 1 + ssl->in_msg[ssl->in_msglen - 1];

                if (ssl->in_msglen < ssl->transform_in->maclen + padlen &&
                    auth_done == 0)
                {
#if defined(MBEDTLS_SSL_DEBUG_ALL)
                    MBEDTLS_SSL_DEBUG_MSG(1, ("msglen (%d) < maclen (%d) + padlen (%d)",
                        ssl->in_msglen, ssl->transform_in->maclen, padlen));
#endif
                    padlen = 0;
                    correct = 0;
                }

#if defined(MBEDTLS_SSL_PROTO_SSL3)
                if (ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0)
                {
                    if (padlen > ssl->transform_in->ivlen)
                    {
#if defined(MBEDTLS_SSL_DEBUG_ALL)
                        MBEDTLS_SSL_DEBUG_MSG(1, ("bad padding length: is %d, "
                            "should be no more than %d",
                            padlen, ssl->transform_in->ivlen));
#endif
                        correct = 0;
                    }
                }
                else
#endif /* MBEDTLS_SSL_PROTO_SSL3 */
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_2)
                    if (ssl->minor_ver > MBEDTLS_SSL_MINOR_VERSION_0)
                    {
                        /*
                        * TLSv1+: always check the padding up to the first failure
                        * and fake check up to 256 bytes of padding
                        */
                        size_t pad_count = 0, real_count = 1;
                        size_t padding_idx = ssl->in_msglen - padlen - 1;

                        /*
                        * Padding is guaranteed to be incorrect if:
                        *   1. padlen >= ssl->in_msglen
                        *
                        *   2. padding_idx >= MBEDTLS_SSL_MAX_CONTENT_LEN +
                        *                     ssl->transform_in->maclen
                        *
                        * In both cases we reset padding_idx to a safe value (0) to
                        * prevent out-of-buffer reads.
                        */
                        correct &= (ssl->in_msglen >= padlen + 1);
                        correct &= (padding_idx < MBEDTLS_SSL_MAX_CONTENT_LEN +
                            ssl->transform_in->maclen);

                        padding_idx *= correct;

                        for (i = 1; i <= 256; i++)
                        {
                            real_count &= (i <= padlen);
                            pad_count += real_count *
                                (ssl->in_msg[padding_idx + i] == padlen - 1);
                        }

                        correct &= (pad_count == padlen); /* Only 1 on correct padding */

#if defined(MBEDTLS_SSL_DEBUG_ALL)
                        if (padlen > 0 && correct == 0)
                            MBEDTLS_SSL_DEBUG_MSG(1, ("bad padding byte detected"));
#endif
                        padlen &= correct * 0x1FF;
                    }
                    else
#endif /* MBEDTLS_SSL_PROTO_TLS1 || MBEDTLS_SSL_PROTO_TLS1_1 || \
              MBEDTLS_SSL_PROTO_TLS1_2 */
                    {
//                        MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
                        return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
                    }

                ssl->in_msglen -= padlen;
            }
            else
#endif /* MBEDTLS_CIPHER_MODE_CBC &&
                ( MBEDTLS_AES_C || MBEDTLS_CAMELLIA_C ) */
            {
//                MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
                return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
            }

//    MBEDTLS_SSL_DEBUG_BUF(4, "raw buffer after decryption",
  //      ssl->in_msg, ssl->in_msglen);

    /*
    * Authenticate if not done yet.
    * Compute the MAC regardless of the padding result (RFC4346, CBCTIME).
    */
#if defined(SSL_SOME_MODES_USE_MAC)
    if (auth_done == 0)
    {
        unsigned char tmp[SSL_MAX_MAC_SIZE];

        ssl->in_msglen -= ssl->transform_in->maclen;

        ssl->in_len[0] = (unsigned char)(ssl->in_msglen >> 8);
        ssl->in_len[1] = (unsigned char)(ssl->in_msglen);

        memcpy(tmp, ssl->in_msg + ssl->in_msglen, ssl->transform_in->maclen);

#if defined(MBEDTLS_SSL_PROTO_SSL3)
        if (ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0)
        {
            ssl_mac(&ssl->transform_in->md_ctx_dec,
                ssl->transform_in->mac_dec,
                ssl->in_msg, ssl->in_msglen,
                ssl->in_ctr, ssl->in_msgtype);
        }
        else
#endif /* MBEDTLS_SSL_PROTO_SSL3 */
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
        defined(MBEDTLS_SSL_PROTO_TLS1_2)
            if (ssl->minor_ver > MBEDTLS_SSL_MINOR_VERSION_0)
            {
                /*
                * Process MAC and always update for padlen afterwards to make
                * total time independent of padlen
                *
                * extra_run compensates MAC check for padlen
                *
                * Known timing attacks:
                *  - Lucky Thirteen (http://www.isg.rhul.ac.uk/tls/TLStiming.pdf)
                *
                * We use ( ( Lx + 8 ) / 64 ) to handle 'negative Lx' values
                * correctly. (We round down instead of up, so -56 is the correct
                * value for our calculations instead of -55)
                */
                size_t j, extra_run = 0;
                extra_run = (13 + ssl->in_msglen + padlen + 8) / 64 -
                    (13 + ssl->in_msglen + 8) / 64;

                extra_run &= correct * 0xFF;

                mbedtls_md_hmac_update(&ssl->transform_in->md_ctx_dec, ssl->in_ctr, 8);
                mbedtls_md_hmac_update(&ssl->transform_in->md_ctx_dec, ssl->in_hdr, 3);
                mbedtls_md_hmac_update(&ssl->transform_in->md_ctx_dec, ssl->in_len, 2);
                mbedtls_md_hmac_update(&ssl->transform_in->md_ctx_dec, ssl->in_msg,
                    ssl->in_msglen);
                mbedtls_md_hmac_finish(&ssl->transform_in->md_ctx_dec,
                    ssl->in_msg + ssl->in_msglen);
                /* Call mbedtls_md_process at least once due to cache attacks */
                for (j = 0; j < extra_run + 1; j++)
                    mbedtls_md_process(&ssl->transform_in->md_ctx_dec, ssl->in_msg);

                mbedtls_md_hmac_reset(&ssl->transform_in->md_ctx_dec);
            }
            else
#endif /* MBEDTLS_SSL_PROTO_TLS1 || MBEDTLS_SSL_PROTO_TLS1_1 || \
                  MBEDTLS_SSL_PROTO_TLS1_2 */
            {
//                MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
                return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
            }

//        MBEDTLS_SSL_DEBUG_BUF(4, "message  mac", tmp, ssl->transform_in->maclen);
//        MBEDTLS_SSL_DEBUG_BUF(4, "computed mac", ssl->in_msg + ssl->in_msglen,
   //         ssl->transform_in->maclen);

        if (mbedtls_ssl_safer_memcmp(tmp, ssl->in_msg + ssl->in_msglen,
            ssl->transform_in->maclen) != 0)
        {
#if defined(MBEDTLS_SSL_DEBUG_ALL)
            MBEDTLS_SSL_DEBUG_MSG(1, ("message mac does not match"));
#endif
            correct = 0;
        }
        auth_done++;

        /*
        * Finally check the correct flag
        */
        if (correct == 0)
            return(MBEDTLS_ERR_SSL_INVALID_MAC);
    }
#endif /* SSL_SOME_MODES_USE_MAC */

    /* Make extra sure authentication was performed, exactly once */
    if (auth_done != 1)
    {
//        MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
        return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
    }

    if (ssl->in_msglen == 0)
    {
        ssl->nb_zero++;

        /*
        * Three or more empty messages may be a DoS attack
        * (excessive CPU consumption).
        */
        if (ssl->nb_zero > 3)
        {
//            MBEDTLS_SSL_DEBUG_MSG(1, ("received four consecutive empty "
     //           "messages, possible DoS attack"));
            return(MBEDTLS_ERR_SSL_INVALID_MAC);
        }
    }
    else
        ssl->nb_zero = 0;

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM)
    {
        ; /* in_ctr read from peer, not maintained internally */
    }
    else
#endif
    {
        fnIncCounter(ssl, ssl->in_ctr);                                  // increment the input counter
      //  for (i = 8; i > ssl_ep_len(ssl); i--)
        //    if (++ssl->in_ctr[i - 1] != 0)
          //      break;

        /* The loop goes to its end iff the counter is wrapping */
        if (i == ssl_ep_len(ssl))
        {
//            MBEDTLS_SSL_DEBUG_MSG(1, ("incoming message counter would wrap"));
            return(MBEDTLS_ERR_SSL_COUNTER_WRAPPING);
        }
    }

//    MBEDTLS_SSL_DEBUG_MSG(2, ("<= decrypt buf"));

    // Decrypted length and location
    //
    *ptr_ulLength = ssl->in_msglen;
    *ptrptrInput = secure_session->ssl.in_msg;

    return(0);
}



extern unsigned char *fnFinished(unsigned char *ptrData)
{
    int /*ret, */hash_len;

    // MBEDTLS_SSL_DEBUG_MSG(2, ("=> write finished"));

    secure_session->ssl.out_hdr = (ptrData - 5);
    secure_session->ssl.out_len = (ptrData - 2);
    secure_session->ssl.out_iv = ptrData;

    /*
    * Set the out_msg pointer to the correct location based on IV length
    */
    if (secure_session->ssl.minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2) {
        secure_session->ssl.out_msg = (secure_session->ssl.out_iv + secure_session->ssl.transform_negotiate->ivlen - secure_session->ssl.transform_negotiate->fixed_ivlen);
    }
    else {
        secure_session->ssl.out_msg = secure_session->ssl.out_iv;
    }

    secure_session->ssl.handshake->calc_finished(&(secure_session->ssl), (secure_session->ssl.out_msg + 4), secure_session->ssl.conf->endpoint); // complete the calculation of the check sum and insert the result into the message - allocates (?) to 100 block of heap - total 13024 - 17 holes of 7116 total

    /*
    * RFC 5246 7.4.9 (Page 63) says 12 is the default length and ciphersuites
    * may define some other value. Currently (early 2016), no defined
    * ciphersuite does this (and this is unlikely to change as activity has
    * moved to TLS 1.3 now) so we can keep the hardcoded 12 here.
    */
    hash_len = (secure_session->ssl.minor_ver == MBEDTLS_SSL_MINOR_VERSION_0) ? 36 : 12;

#if defined(MBEDTLS_SSL_RENEGOTIATION)
    ssl->verify_data_len = hash_len;
    memcpy(ssl->own_verify_data, ssl->out_msg + 4, hash_len);
#endif

    secure_session->ssl.out_msglen = (4 + hash_len);
    //ssl->out_msgtype = MBEDTLS_SSL_MSG_HANDSHAKE;
    secure_session->ssl.out_msg[0] = MBEDTLS_SSL_HS_FINISHED;
    secure_session->ssl.out_msg[1] = (unsigned char)(hash_len >> 16);
    secure_session->ssl.out_msg[2] = (unsigned char)(hash_len >> 8);
    secure_session->ssl.out_msg[3] = (unsigned char)hash_len;
    secure_session->ssl.out_len[0] = (unsigned char)(secure_session->ssl.out_msglen >> 8);
    secure_session->ssl.out_len[1] = (unsigned char)secure_session->ssl.out_msglen;

    // calculate the check sum - warning - does this always get done in every mode?????
    mbedtls_sha256_update(&(secure_session->ssl.handshake->fin_sha256), secure_session->ssl.out_msg, secure_session->ssl.out_msglen);

    /*
    * In case of session resuming, invert the client and server
    * ChangeCipherSpec messages order.
    */
    if (secure_session->ssl.handshake->resume != 0) {
#if defined(MBEDTLS_SSL_CLI_C)
        //if (secure_session->ssl.conf->endpoint == MBEDTLS_SSL_IS_CLIENT)
         //   ssl->state = MBEDTLS_SSL_HANDSHAKE_WRAPUP;
#endif
#if defined(MBEDTLS_SSL_SRV_C)
        if (ssl->conf->endpoint == MBEDTLS_SSL_IS_SERVER) {
            ssl->state = MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC;
        }
#endif
    }
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM)
    {
        unsigned char i;

        /* Remember current epoch settings for resending */
        ssl->handshake->alt_transform_out = ssl->transform_out;
        memcpy(ssl->handshake->alt_out_ctr, ssl->out_ctr, 8);

        /* Set sequence_number to zero */
        memset(ssl->out_ctr + 2, 0, 6);

        /* Increment epoch */
        for (i = 2; i > 0; i--)
            if (++ssl->out_ctr[i - 1] != 0)
                break;

        /* The loop goes to its end iff the counter is wrapping */
        if (i == 0) {
            MBEDTLS_SSL_DEBUG_MSG(1, ("DTLS epoch would wrap"));
            return(MBEDTLS_ERR_SSL_COUNTER_WRAPPING);
        }
    }
    else
#endif /* MBEDTLS_SSL_PROTO_DTLS */
    uMemset(secure_session->ssl.out_ctr, 0, 8);
    secure_session->ssl.transform_out = secure_session->ssl.transform_negotiate;
    secure_session->ssl.session_out = secure_session->ssl.session_negotiate;
#if defined(MBEDTLS_SSL_HW_RECORD_ACCEL)
    if (mbedtls_ssl_hw_record_activate != NULL) {
        if ((ret = mbedtls_ssl_hw_record_activate(ssl, MBEDTLS_SSL_CHANNEL_OUTBOUND)) != 0) {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_hw_record_activate", ret);
            return(MBEDTLS_ERR_SSL_HW_ACCEL_FAILED);
        }
    }
#endif

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        mbedtls_ssl_send_flight_completed(ssl);
    }
#endif
    return (fnEncrypt(ptrData, 0, hash_len));
}

// Taken from ssl_tls.c to be able to be entered locally
//
static void ssl_update_checksum_start( mbedtls_ssl_context *ssl,
                                       const unsigned char *buf, size_t len )
{
#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
     mbedtls_md5_update( &ssl->handshake->fin_md5 , buf, len );
    mbedtls_sha1_update( &ssl->handshake->fin_sha1, buf, len );
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_SHA256_C)
    mbedtls_sha256_update( &ssl->handshake->fin_sha256, buf, len );
#endif
#if defined(MBEDTLS_SHA512_C)
    mbedtls_sha512_update( &ssl->handshake->fin_sha512, buf, len );
#endif
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
}

// Taken from ssl_tls.c to avoid rx/tx initialisations
//
static void ssl_handshake_params_init( mbedtls_ssl_handshake_params *handshake )
{
  //memset( handshake, 0, sizeof( mbedtls_ssl_handshake_params ) ); // uTasker - not required because content is already zeroed

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
     mbedtls_md5_init(   &handshake->fin_md5  );
    mbedtls_sha1_init(   &handshake->fin_sha1 );
     mbedtls_md5_starts( &handshake->fin_md5  );
    mbedtls_sha1_starts( &handshake->fin_sha1 );
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_SHA256_C)
  //mbedtls_sha256_init(   &handshake->fin_sha256    ); not needed because content is already zeroed
    mbedtls_sha256_starts( &handshake->fin_sha256, 0 );
#endif
#if defined(MBEDTLS_SHA512_C)
    mbedtls_sha512_init(   &handshake->fin_sha512    );
    mbedtls_sha512_starts( &handshake->fin_sha512, 1 );
#endif
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

    handshake->update_checksum = ssl_update_checksum_start;

#if defined(MBEDTLS_SSL_PROTO_TLS1_2) && \
    defined(MBEDTLS_KEY_EXCHANGE__WITH_CERT__ENABLED)
  //mbedtls_ssl_sig_hash_set_init( &handshake->hash_algs ); // uTasker - not required
#endif

#if defined(MBEDTLS_DHM_C)
  //mbedtls_dhm_init( &handshake->dhm_ctx ); // uTasker - not required
#endif
#if defined(MBEDTLS_ECDH_C)
  //mbedtls_ecdh_init( &handshake->ecdh_ctx ); // uTasker - not required
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    mbedtls_ecjpake_init( &handshake->ecjpake_ctx );
#if defined(MBEDTLS_SSL_CLI_C)
    handshake->ecjpake_cache = NULL;
    handshake->ecjpake_cache_len = 0;
#endif
#endif

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    handshake->sni_authmode = MBEDTLS_SSL_VERIFY_UNSET;
#endif
}

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED) ||                     \
    defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED) ||                   \
    defined(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED) ||                     \
    defined(MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED) ||                      \
    defined(MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED)
static int ssl_check_server_ecdh_params(const mbedtls_ssl_context *ssl)
{
    const mbedtls_ecp_curve_info *curve_info;

    curve_info = mbedtls_ecp_curve_info_from_grp_id(ssl->handshake->ecdh_ctx.grp.id);
    if (curve_info == NULL)
    {
      //MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
        return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
    }

  //MBEDTLS_SSL_DEBUG_MSG(2, ("ECDH curve: %s", curve_info->name));

#if defined(MBEDTLS_ECP_C)
    if (mbedtls_ssl_check_curve(ssl, ssl->handshake->ecdh_ctx.grp.id) != 0)
#else
    if (ssl->handshake->ecdh_ctx.grp.nbits < 163 ||
        ssl->handshake->ecdh_ctx.grp.nbits > 521)
#endif
        return(-1);

  //MBEDTLS_SSL_DEBUG_ECP(3, "ECDH: Qp", &ssl->handshake->ecdh_ctx.Qp);

    return(0);
}
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED ||
MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED ||
MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED ||
MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED ||
MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED */


static int ssl_parse_renegotiation_info(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    if (ssl->renego_status != MBEDTLS_SSL_INITIAL_HANDSHAKE)
    {
        /* Check verify-data in constant-time. The length OTOH is no secret */
        if (len != 1 + ssl->verify_data_len * 2 ||
            buf[0] != ssl->verify_data_len * 2 ||
            mbedtls_ssl_safer_memcmp(buf + 1,
                ssl->own_verify_data, ssl->verify_data_len) != 0 ||
            mbedtls_ssl_safer_memcmp(buf + 1 + ssl->verify_data_len,
                ssl->peer_verify_data, ssl->verify_data_len) != 0)
        {
            MBEDTLS_SSL_DEBUG_MSG(1, ("non-matching renegotiation info"));
            mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
            return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
        }
    }
    else
#endif /* MBEDTLS_SSL_RENEGOTIATION */
    {
        if (len != 1 || buf[0] != 0x00)
        {
//            MBEDTLS_SSL_DEBUG_MSG(1, ("non-zero length renegotiation info"));
            mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
            return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
        }

        ssl->secure_renegotiation = MBEDTLS_SSL_SECURE_RENEGOTIATION;
    }

    return(0);
}

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
static int ssl_parse_max_fragment_length_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
    /*
    * server should use the extension only if we did,
    * and if so the server's value should match ours (and len is always 1)
    */
    if (ssl->conf->mfl_code == MBEDTLS_SSL_MAX_FRAG_LEN_NONE ||
        len != 1 ||
        buf[0] != ssl->conf->mfl_code)
    {
//        MBEDTLS_SSL_DEBUG_MSG(1, ("non-matching max fragment length extension"));
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    return(0);
}
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
static int ssl_parse_encrypt_then_mac_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
    if (ssl->conf->encrypt_then_mac == MBEDTLS_SSL_ETM_DISABLED ||
        ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0 ||
        len != 0)
    {
       // MBEDTLS_SSL_DEBUG_MSG(1, ("non-matching encrypt-then-MAC extension"));
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    ((void)buf);

    ssl->session_negotiate->encrypt_then_mac = MBEDTLS_SSL_ETM_ENABLED;

    return(0);
}
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
static int ssl_parse_extended_ms_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
    if (ssl->conf->extended_ms == MBEDTLS_SSL_EXTENDED_MS_DISABLED ||
        ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0 ||
        len != 0)
    {
    //    MBEDTLS_SSL_DEBUG_MSG(1, ("non-matching extended master secret extension"));
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    ((void)buf);

    ssl->handshake->extended_ms = MBEDTLS_SSL_EXTENDED_MS_ENABLED;

    return(0);
}
#endif /* MBEDTLS_SSL_EXTENDED_MASTER_SECRET */


#if defined(MBEDTLS_ECDH_C) || defined(MBEDTLS_ECDSA_C) || \
    defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
static int ssl_parse_supported_point_formats_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
    size_t list_size;
    const unsigned char *p;

    list_size = buf[0];
    if (list_size + 1 != len)
    {
//        MBEDTLS_SSL_DEBUG_MSG(1, ("bad server hello message"));
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    p = buf + 1;
    while (list_size > 0)
    {
        if (p[0] == MBEDTLS_ECP_PF_UNCOMPRESSED ||
            p[0] == MBEDTLS_ECP_PF_COMPRESSED)
        {
#if defined(MBEDTLS_ECDH_C) || defined(MBEDTLS_ECDSA_C)
            ssl->handshake->ecdh_ctx.point_format = p[0];
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
            ssl->handshake->ecjpake_ctx.point_format = p[0];
#endif
//            MBEDTLS_SSL_DEBUG_MSG(4, ("point format selected: %d", p[0]));
            return(0);
        }

        list_size--;
        p++;
    }

//    MBEDTLS_SSL_DEBUG_MSG(1, ("no point format in common"));
    mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
        MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
    return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
}
#endif /* MBEDTLS_ECDH_C || MBEDTLS_ECDSA_C || 
MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */


#if defined(MBEDTLS_SSL_ALPN)
static int ssl_parse_alpn_ext(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
    size_t list_len, name_len;
    const char **p;

    /* If we didn't send it, the server shouldn't send it */
    if (ssl->conf->alpn_list == NULL)
    {
 //       MBEDTLS_SSL_DEBUG_MSG(1, ("non-matching ALPN extension"));
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    /*
    * opaque ProtocolName<1..2^8-1>;
    *
    * struct {
    *     ProtocolName protocol_name_list<2..2^16-1>
    * } ProtocolNameList;
    *
    * the "ProtocolNameList" MUST contain exactly one "ProtocolName"
    */

    /* Min length is 2 (list_len) + 1 (name_len) + 1 (name) */
    if (len < 4)
    {
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    list_len = (buf[0] << 8) | buf[1];
    if (list_len != len - 2)
    {
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    name_len = buf[2];
    if (name_len != list_len - 1)
    {
        mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
            MBEDTLS_SSL_ALERT_MSG_DECODE_ERROR);
        return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
    }

    /* Check that the server chosen protocol was in our list and save it */
    for (p = ssl->conf->alpn_list; *p != NULL; p++)
    {
        if (name_len == uStrlen(*p) &&
            memcmp(buf + 3, *p, name_len) == 0)
        {
            ssl->alpn_chosen = *p;
            return(0);
        }
    }

   // MBEDTLS_SSL_DEBUG_MSG(1, ("ALPN extension: no matching protocol"));
    mbedtls_ssl_send_alert_message(ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
        MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
    return(MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO);
}
#endif /* MBEDTLS_SSL_ALPN */

#endif


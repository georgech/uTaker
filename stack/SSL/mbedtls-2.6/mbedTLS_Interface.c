/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      mbedTLS_Interface.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
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


static UTASKER_MBEDSSL_SESSION *secure_session = 0;

static int fnSimpleRandom(void *ptr, unsigned char *ptrBuf, size_t length)
{
    static unsigned char ucPoorRandomForTest = 0;
    while (length-- != 0) {
        *ptrBuf++ = ucPoorRandomForTest++;
    }
    return 0;
}

// Notes - the handling expects that the x509 content is a null-terminated string!
//
extern int fnInitialiseSecureLayer(const unsigned char *ptrOurCertificate, unsigned long ulCertificateLength, const unsigned char *ptrOutPrivateKey, unsigned long ulOurPrivateKeyLength)
{
    int iReturn;
    unsigned char *ptrTempString;
    secure_session = mbedtls_calloc(1, sizeof(UTASKER_MBEDSSL_SESSION)); // get the memory needed for the session (1252 bytes)
    ptrTempString = (unsigned char *)mbedtls_calloc(1, (ulOurPrivateKeyLength + 1));
    uMemcpy(ptrTempString, ptrOutPrivateKey, ulOurPrivateKeyLength);     // copy the private key string
    ptrTempString[ulOurPrivateKeyLength++] = 0;                          // terminate the string, which is required for its further processing
    iReturn = mbedtls_pk_parse_key(&(secure_session->ourPrivateKey), ptrTempString, ulOurPrivateKeyLength, 0, 0); // no password
    mbedtls_free(ptrTempString);
    if (iReturn == 0) {
        ptrTempString = (unsigned char *)mbedtls_calloc(1, (ulCertificateLength + 1));
        uMemcpy(ptrTempString, ptrOurCertificate, ulCertificateLength);  // copy the private key string
        ptrTempString[ulCertificateLength++] = 0;                        // terminate the string, which is required for its further processing
        iReturn = mbedtls_x509_crt_parse(&(secure_session->ourCertificate), ptrTempString, ulCertificateLength);
        mbedtls_free(ptrTempString);
        if (iReturn == 0) {
            secure_session->ssl.handshake = (mbedtls_ssl_handshake_params *)mbedtls_calloc(1, (sizeof(mbedtls_ssl_handshake_params))); // handshake parameters
            ssl_handshake_params_init(secure_session->ssl.handshake);   // initialise the handshake parameters
            secure_session->ssl.transform_negotiate = mbedtls_calloc(1, sizeof(mbedtls_ssl_transform));
            secure_session->ssl.session_negotiate = mbedtls_calloc(1, sizeof(mbedtls_ssl_session));

            mbedtls_ssl_conf_rng(&(secure_session->config), fnSimpleRandom, secure_session); // set the random number method
            secure_session->ssl.conf = &(secure_session->config);
            secure_session->config.endpoint = MBEDTLS_SSL_IS_CLIENT;     // we are client

            secure_session->ssl.out_ctr = (unsigned char *)mbedtls_calloc(1, 8); // 64 bit outgoing counter memory
            secure_session->ssl.out_iv = (unsigned char *)mbedtls_calloc(1, 16); // ??
            iReturn = mbedtls_ssl_conf_own_cert(&(secure_session->config), &(secure_session->ourCertificate), &(secure_session->ourPrivateKey)); // attach the client certificate and private key to the  configuration
        }
    }
    return iReturn;
}

// During the server hello we receive the cipher to be used during the session
//
extern void fnSetSessionCipher(unsigned short session_cipher, unsigned char ucVersion[2])
{
    secure_session->ssl.major_ver = ucVersion[0];
    secure_session->ssl.minor_ver = ucVersion[1];
    secure_session->ssl.session_negotiate->ciphersuite = session_cipher;
    secure_session->ssl.transform_negotiate->ciphersuite_info = mbedtls_ssl_ciphersuite_from_id(session_cipher);
    secure_session->ssl.handshake->ecdh_ctx.point_format = MBEDTLS_ECP_PF_UNCOMPRESSED; // only uncompressed supported
}

// This is used to check and save received certificates
//
extern int fnSaveServerCertificate(unsigned char *ptrCertificate, unsigned long ulCertificateLength)
{
    return (mbedtls_x509_crt_parse_der(&(secure_session->certificates), ptrCertificate, ulCertificateLength));
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
    int iReturn = mbedtls_ecdh_read_params(&(secure_session->ssl.handshake->ecdh_ctx), (const unsigned char **)&ptrData, (const unsigned char *)(ptrData + ulKeyExchangeLength));
    if (iReturn == 0) {
      // The following would need            conf->curve_list
      //iReturn = ssl_check_server_ecdh_params(&secure_session->ssl); // check server key exchange message (ECDHE curve)
    }
    return iReturn;
}

extern unsigned char *fnGeneratePublicKey(unsigned char *ptrData)
{
    int iReturn;
    size_t output_length;
    unsigned char test = 0x5a;
    iReturn = mbedtls_ecp_gen_keypair_base(&(secure_session->ssl.handshake->ecdh_ctx.grp), &(secure_session->ssl.handshake->ecdh_ctx.grp.G), &(secure_session->ssl.handshake->ecdh_ctx.d), &(secure_session->ssl.handshake->ecdh_ctx.Q), secure_session->config.f_rng, secure_session->config.p_rng); // generate public key [ecp.c]
    if (iReturn == 0) {
        iReturn = mbedtls_ecp_point_write_binary(&(secure_session->ssl.handshake->ecdh_ctx.grp), &(secure_session->ssl.handshake->ecdh_ctx.Q), secure_session->ssl.handshake->ecdh_ctx.point_format, &output_length, ptrData, 1000); // export a point into unsigned binary data [ecp.c]
        if (iReturn == 0) {
            ptrData += output_length;
            iReturn = mbedtls_ecdh_calc_secret(&secure_session->ssl.handshake->ecdh_ctx, &(secure_session->ssl.handshake->pmslen), secure_session->ssl.handshake->premaster, MBEDTLS_MPI_MAX_SIZE, secure_session->config.f_rng, secure_session->config.p_rng);
        }
    }
    if (iReturn != 0) {
        return 0;
    }
    return ptrData;
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
        size_t n = 0, offset = 0;
        unsigned char hash[48];
        unsigned char *hash_start = hash;
        mbedtls_md_type_t md_alg = MBEDTLS_MD_NONE;
        unsigned int hashlen;

//      MBEDTLS_SSL_DEBUG_MSG(2, ("=> write certificate verify"));

        if ((ret = mbedtls_ssl_derive_keys(&secure_session->ssl)) != 0)
        {
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
            return(0);
        }

        if (secure_session->ssl.client_auth == 0 || mbedtls_ssl_own_cert(&secure_session->ssl) == NULL)
        {
        //    MBEDTLS_SSL_DEBUG_MSG(2, ("<= skip write certificate verify"));
            //ssl->state++;
            return(0);
        }

        if (mbedtls_ssl_own_key(&secure_session->ssl) == NULL)
        {
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
            if (secure_session->ssl.minor_ver == MBEDTLS_SSL_MINOR_VERSION_3)
            {
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
                if (secure_session->ssl.transform_negotiate->ciphersuite_info->mac ==
                    MBEDTLS_MD_SHA384)
                {
                    md_alg = MBEDTLS_MD_SHA384;
                    *ptrData++ = MBEDTLS_SSL_HASH_SHA384;
                }
                else
                {
                    md_alg = MBEDTLS_MD_SHA256;
                    *ptrData++ = MBEDTLS_SSL_HASH_SHA256;
                }
                *ptrData++ = mbedtls_ssl_sig_from_pk(mbedtls_ssl_own_key(&secure_session->ssl));

                /* Info from md_alg will be used instead */
                hashlen = 0;
                offset = 2;
            }
            else
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
            {
//                MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
               // return(MBEDTLS_ERR_SSL_INTERNAL_ERROR);
                return 0;
            }

        if ((ret = mbedtls_pk_sign(mbedtls_ssl_own_key(&secure_session->ssl), md_alg, hash_start, hashlen,
            (ptrData + 2), &n,
            secure_session->ssl.conf->f_rng, secure_session->ssl.conf->p_rng)) != 0)
        {
      //      MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_pk_sign", ret);
           // return(ret);
            return 0;
        }
        *ptrData++ = (unsigned char)(n >> 8);
        *ptrData++ = (unsigned char)(n);
        ptrData += n;

        /*
        ssl->out_msg[4 + offset] = (unsigned char)(n >> 8);
        ssl->out_msg[5 + offset] = (unsigned char)(n);

        ssl->out_msglen = 6 + n + offset;
        ssl->out_msgtype = MBEDTLS_SSL_MSG_HANDSHAKE;
        ssl->out_msg[0] = MBEDTLS_SSL_HS_CERTIFICATE_VERIFY;

        ssl->state++;
        */
      //  if ((ret = mbedtls_ssl_write_record(&secure_session->ssl)) != 0)
      //{
     //       MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_write_record", ret);
         //   return(ret);
      //    return 0;
      //}

    //    MBEDTLS_SSL_DEBUG_MSG(2, ("<= write certificate verify"));

        //return(ret);
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


/*
 * Encryption/decryption functions
 */
static unsigned char *ssl_encrypt_buf( mbedtls_ssl_context *ssl, unsigned char *ptrData, unsigned long ulLength)
{
    mbedtls_cipher_mode_t mode;
    int auth_done = 0;

   // MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> encrypt buf" ) );

 /*   if( ssl->session_out == NULL || ssl->transform_out == NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }*/

    mode = mbedtls_cipher_get_cipher_mode( &ssl->transform_out->cipher_ctx_enc );

  //  MBEDTLS_SSL_DEBUG_BUF( 4, "before encrypt: output payload",
    //                  ssl->out_msg, ssl->out_msglen );

    /*
     * Add MAC before if needed
     */
#if defined(SSL_SOME_MODES_USE_MAC)
    if( mode == MBEDTLS_MODE_STREAM ||
        ( mode == MBEDTLS_MODE_CBC
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
          && ssl->session_out->encrypt_then_mac == MBEDTLS_SSL_ETM_DISABLED
#endif
        ) )
    {
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
        if( ssl->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_1 )
        {
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_ctr, 8 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_hdr, 3 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_len, 2 );
            mbedtls_md_hmac_update( &ssl->transform_out->md_ctx_enc,
                             ssl->out_msg, ssl->out_msglen );
            mbedtls_md_hmac_finish( &ssl->transform_out->md_ctx_enc,
                             ssl->out_msg + ssl->out_msglen );
            mbedtls_md_hmac_reset( &ssl->transform_out->md_ctx_enc );
        }
        else
#endif
        {
//            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( 0 /*MBEDTLS_ERR_SSL_INTERNAL_ERROR*/ );
        }

//        MBEDTLS_SSL_DEBUG_BUF( 4, "computed mac",
  //                     ssl->out_msg + ssl->out_msglen,
    //                   ssl->transform_out->maclen );

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
        mode == MBEDTLS_MODE_CCM )
    {
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
        if( ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen != 8 )
        {
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
                                         enc_msg + enc_msglen, taglen ) ) != 0 )
        {
         //   MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_auth_encrypt", ret );
            return( 0 );
        }

        if( olen != enc_msglen )
        {
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
            *ptrData++ = (unsigned char)padlen;
            //ssl->out_msg[ssl->out_msglen + i] = (unsigned char) padlen;

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
            ret = ssl->conf->f_rng( ssl->conf->p_rng, ssl->transform_out->iv_enc,
                                  ssl->transform_out->ivlen );
            if( ret != 0 )
                return( 0 );

            memcpy( ssl->out_iv, ssl->transform_out->iv_enc,
                    ssl->transform_out->ivlen );

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

        if( enc_msglen != olen )
        {
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

    return( 0 );
}



extern unsigned char *fnFinished(unsigned char *ptrData)
{
    int /*ret, */hash_len;

   // MBEDTLS_SSL_DEBUG_MSG(2, ("=> write finished"));

    /*
    * Set the out_msg pointer to the correct location based on IV length
    */
    if (secure_session->ssl.minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2)
    {
        secure_session->ssl.out_msg = ptrData /*secure_session->ssl.out_iv*/ + secure_session->ssl.transform_negotiate->ivlen -
            secure_session->ssl.transform_negotiate->fixed_ivlen;
    }
    else
        secure_session->ssl.out_msg = ptrData;// secure_session->ssl.out_iv;

        secure_session->ssl.handshake->calc_finished(&(secure_session->ssl), secure_session->ssl.out_msg, secure_session->ssl.conf->endpoint);

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

    //ssl->out_msglen = 4 + hash_len;
    //ssl->out_msgtype = MBEDTLS_SSL_MSG_HANDSHAKE;
    //ssl->out_msg[0] = MBEDTLS_SSL_HS_FINISHED;

    /*
    * In case of session resuming, invert the client and server
    * ChangeCipherSpec messages order.
    */
    if (secure_session->ssl.handshake->resume != 0)
    {
#if defined(MBEDTLS_SSL_CLI_C)
        //if (secure_session->ssl.conf->endpoint == MBEDTLS_SSL_IS_CLIENT)
         //   ssl->state = MBEDTLS_SSL_HANDSHAKE_WRAPUP;
#endif
#if defined(MBEDTLS_SSL_SRV_C)
        if (ssl->conf->endpoint == MBEDTLS_SSL_IS_SERVER)
            ssl->state = MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC;
#endif
    }
    //else
    //    ssl->state++;

    /*
    * Switch to our negotiated transform and session parameters for outbound
    * data.
    */
   // MBEDTLS_SSL_DEBUG_MSG(3, ("switching to new transform spec for outbound data"));

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
        if (i == 0)
        {
            MBEDTLS_SSL_DEBUG_MSG(1, ("DTLS epoch would wrap"));
            return(MBEDTLS_ERR_SSL_COUNTER_WRAPPING);
        }
    }
    else
#endif /* MBEDTLS_SSL_PROTO_DTLS */
        memset(secure_session->ssl.out_ctr, 0, 8);

    secure_session->ssl.transform_out = secure_session->ssl.transform_negotiate;
    secure_session->ssl.session_out = secure_session->ssl.session_negotiate;

#if defined(MBEDTLS_SSL_HW_RECORD_ACCEL)
    if (mbedtls_ssl_hw_record_activate != NULL)
    {
        if ((ret = mbedtls_ssl_hw_record_activate(ssl, MBEDTLS_SSL_CHANNEL_OUTBOUND)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_hw_record_activate", ret);
            return(MBEDTLS_ERR_SSL_HW_ACCEL_FAILED);
        }
    }
#endif

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM)
        mbedtls_ssl_send_flight_completed(ssl);
#endif

    //if ((ret = mbedtls_ssl_write_record(ssl)) != 0)
    //{
      //  MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_write_record", ret);
    //    return(ret);
    //}

  //  MBEDTLS_SSL_DEBUG_MSG(2, ("<= write finished"));
    return (ssl_encrypt_buf(&(secure_session->ssl), ptrData, hash_len));
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
  //memset( handshake, 0, sizeof( mbedtls_ssl_handshake_params ) ); // uTasker - not required

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
     mbedtls_md5_init(   &handshake->fin_md5  );
    mbedtls_sha1_init(   &handshake->fin_sha1 );
     mbedtls_md5_starts( &handshake->fin_md5  );
    mbedtls_sha1_starts( &handshake->fin_sha1 );
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_SHA256_C)
  //mbedtls_sha256_init(   &handshake->fin_sha256    ); // uTasker - not required
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

#endif


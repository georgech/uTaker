/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      crypto.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    
*/        

#include "config.h"
#if defined CRYPTOGRAPHY
#include "../../stack/ssl/mbedtls-1.3.10/aes.h"                          // AES uses mbedTLS code


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined AES_INSTANCE_COUNT
    #define AES_INSTANCE_COUNT 1
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stAES_INSTANCE
{
    aes_context aes_encrypt_context;
    unsigned char iv[AES_BLOCK_LENGTH];
} AES_INSTANCE;


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


static AES_INSTANCE aes_instance[AES_INSTANCE_COUNT] = { {{0}} };


// AES key or iv initialisation
//
extern int fnAES_Init(int iInstanceCommand, const unsigned char *ptrKey, int iKeyLength)
{
    int iInstance = (iInstanceCommand & AES_INSTANCE_MASK);
    if (iInstance >= AES_INSTANCE_COUNT) {
        return AES_INVALID_INSTANCE_REFERENCE;
    }
    if ((iInstanceCommand & AES_COMMAND_AES_SET_KEY_ENCRYPT) != 0) {
        aes_setkey_enc(&aes_instance[iInstance].aes_encrypt_context, ptrKey, iKeyLength); // set encryption key
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    }
    else if ((iInstanceCommand & AES_COMMAND_AES_SET_KEY_DECRYPT) != 0) {
        aes_setkey_dec(&aes_instance[iInstance].aes_encrypt_context, ptrKey, iKeyLength); // set decryption key
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    }
    else if ((iInstanceCommand & AES_COMMAND_AES_PRIME_IV) != 0) {
        uMemcpy(aes_instance[iInstance].iv, ptrKey, sizeof(aes_instance[iInstance].iv)); // prime the iv
    }
    return 0;
}

// AES encryption or decryption
//
extern int fnAES_Cipher(int iInstanceCommand, const unsigned char *ptrPlaintext, unsigned char *ptrCiphertext, unsigned long ulDataLength)
{
    int iInstance = (iInstanceCommand & AES_INSTANCE_MASK);
    if (iInstance >= AES_INSTANCE_COUNT) {
        return AES_INVALID_INSTANCE_REFERENCE;
    }
    if ((iInstanceCommand & (AES_COMMAND_AES_ENCRYPT | AES_COMMAND_AES_DECRYPT)) != 0) {
        int iMode;
        if (aes_instance[iInstance].aes_encrypt_context.nr == 0) {
            return AES_INSTANCE_NOT_INITIALISED;
        }
        if ((ulDataLength % AES_BLOCK_LENGTH) != 0) {
            uMemset(ptrCiphertext, 0, ulDataLength);                     // if a bad length is called we set the ciphertext to zeros so that if it is still sent it will not be plain text in any way (eg. when the plain text input buffer is being used also as output buffer)
            return AES_ENCRYPT_BAD_LENGTH;
        }
        if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) != 0) {
            uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
        }
        if (iInstanceCommand & AES_COMMAND_AES_ENCRYPT) {
            iMode = AES_ENCRYPT;
        }
        else {
            iMode = AES_DECRYPT;
        }
        aes_crypt_cbc(&aes_instance[iInstance].aes_encrypt_context, iMode, ulDataLength, aes_instance[iInstance].iv, ptrPlaintext, ptrCiphertext); // encrypt always using cipher block chaining
    }
    return 0;
}
#endif



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
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    
*/        

#include "config.h"
#if defined CRYPTOGRAPHY
    #if defined _WINDOWS || (!defined CAU_V1_AVAILABLE && !defined CAU_V2_AVAILABLE && !defined LTC_AVAILABLE)
        #undef NATIVE_AES_CAU
    #endif
    #if defined NATIVE_AES_CAU
        #if !defined LTC_AVAILABLE || defined AES_DISABLE_LTC
    // Freescale mmCAU crypto acceleration routines
    //
    extern void mmcau_aes_set_key(const unsigned char *key, const int key_size, unsigned char *key_sch);
    extern void mmcau_aes_decrypt(const unsigned char *in, const unsigned char *key_sch, const int nr, unsigned char *out);
    extern void mmcau_aes_encrypt(const unsigned char *in, const unsigned char *key_sch, const int nr, unsigned char *out);
        #endif
    #elif defined CRYPTO_OPEN_SSL
        #include "../../stack/ssl/openssl-1.0.2/aes.h"                   // AES uses wolfSSL code
    #elif defined CRYPTO_WOLF_SSL
        #include "../../stack/ssl/wolfssl-3.9.6/aes.h"                   // AES uses wolfSSL code
    #else
        #include "../../stack/ssl/mbedtls-1.3.10/aes.h"                  // AES uses mbedTLS code
    #endif

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined AES_INSTANCE_COUNT
    #define AES_INSTANCE_COUNT   2                                       // if not otherwise specified we have 2 instances (one encryption and one for decryption)
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct _PACK stAES_INSTANCE
{
#if defined NATIVE_AES_CAU
    int iRounds; 
    #if !defined LTC_AVAILABLE || defined AES_DISABLE_LTC
    unsigned char iv[AES_BLOCK_LENGTH];                                  // long word aligned
    unsigned char key[60 * 4];
    #endif
#elif defined CRYPTO_OPEN_SSL
    AES_KEY       aes_encrypt_context;
    unsigned char iv[AES_BLOCK_LENGTH];
#elif defined CRYPTO_WOLF_SSL
    Aes           aes_encrypt_context;
#else
    #if (defined LTC_AVAILABLE && !defined AES_DISABLE_LTC && !defined _WINDOWS)
    int iRounds;
    #endif
    aes_context   aes_encrypt_context;
    unsigned char iv[AES_BLOCK_LENGTH];
#endif
} AES_INSTANCE;


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined NATIVE_AES_CAU || (defined LTC_AVAILABLE && !defined AES_DISABLE_LTC && !defined _WINDOWS)
    static AES_INSTANCE aes_instance[AES_INSTANCE_COUNT] = {{0}};
#elif defined CRYPTO_OPEN_SSL
    static AES_INSTANCE aes_instance[AES_INSTANCE_COUNT] = {{{{0}}}};
#elif defined CRYPTO_WOLF_SSL
    static AES_INSTANCE aes_instance[AES_INSTANCE_COUNT] = {{{{0}}}};
#else
    static AES_INSTANCE aes_instance[AES_INSTANCE_COUNT] = {{{0}}};
#endif

// AES key or iv initialisation
//
extern int fnAES_Init(int iInstanceCommand, const unsigned char *ptrKey, int iKeyLength)
{
    int iInstance = (iInstanceCommand & AES_INSTANCE_MASK);
    if (iInstance >= AES_INSTANCE_COUNT) {
        return AES_INVALID_INSTANCE_REFERENCE;
    }
    #if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE || defined LTC_AVAILABLE
    if ((iInstanceCommand & (AES_COMMAND_AES_SET_KEY_ENCRYPT | AES_COMMAND_AES_SET_KEY_DECRYPT)) != 0) {
        if (((CAST_POINTER_ARITHMETIC)ptrKey & 0x3) != 0) {        // mmCAU/LTC requires long word aligned input key
            _EXCEPTION("Check secret key buffer alignment!");
            return AES_ENCRYPT_BAD_ALIGNMENT;
        }
    }
    #endif
    #if defined NATIVE_AES_CAU || (defined LTC_AVAILABLE && !defined AES_DISABLE_LTC && !defined _WINDOWS)
    if ((iInstanceCommand & (AES_COMMAND_AES_SET_KEY_ENCRYPT | AES_COMMAND_AES_SET_KEY_DECRYPT)) != 0) {
        switch (iKeyLength) {
        case 128:
            aes_instance[iInstance].iRounds = 10;
            break;
        case 196:
            aes_instance[iInstance].iRounds = 12;
            break;
        case 256:
            aes_instance[iInstance].iRounds = 14;
            break;
        default:
            _EXCEPTION("Bad key length!");
            return AES_INVALID_KEY_LENGTH;
        }
    }
    #endif
    if ((iInstanceCommand & AES_COMMAND_AES_SET_KEY_ENCRYPT) != 0) {
    #if defined NATIVE_AES_CAU
        #if defined LTC_AVAILABLE && !defined AES_DISABLE_LTC            // use LTC
        POWER_UP_LTC_MODULE();                                           // ensure that the module is powered up before use
        WRITE_ONE_TO_CLEAR(LTC0_CW, (LTC_CW_CM | LTC_CW_CDS | LTC_CW_CICV | LTC_CW_CCR | LTC_CW_CKR | LTC_CW_CPKA | LTC_CW_CPKB | LTC_CW_CPKN | LTC_CW_CPKE | LTC_CW_COF | LTC_CW_CIF)); // clear internal registers
        LTC0_CTL = (LTC_CTL_IFS | LTC_CTL_OFS | LTC_CTL_KIS | LTC_CTL_KOS | LTC_CTL_CIS | LTC_CTL_COS); // enable byte swap for registers to be used
        LTC0_KEY_0 = *(unsigned long *)ptrKey;                           // key needs to be long word aligned
        ptrKey += 4;
        LTC0_KEY_1 = *(unsigned long *)ptrKey;
        ptrKey += 4;
        LTC0_KEY_2 = *(unsigned long *)ptrKey;
        ptrKey += 4;
        LTC0_KEY_3 = *(unsigned long *)ptrKey;                           // AES128
        if (iKeyLength >= 128) {
            ptrKey += 4;
            LTC0_KEY_4 = *(unsigned long *)ptrKey;
            ptrKey += 4;
            LTC0_KEY_5 = *(unsigned long *)ptrKey;                       // AES192
            if (iKeyLength == 256) {
                ptrKey += 4;
                LTC0_KEY_6 = *(unsigned long *)ptrKey;
                ptrKey += 4;
                LTC0_KEY_7 = *(unsigned long *)ptrKey;                   // AES256
                LTC0_KS = 32;
            }
            else {
                LTC0_KS = 24;
            }
        }
        else {
            LTC0_KS = 16;                                                // the key size must be written after setting the key and it locks the register against further writes
        }
        LTC0_CTX_0 = 0;                                                  // zero IV
        LTC0_CTX_1 = 0;
        LTC0_CTX_2 = 0;
        LTC0_CTX_3 = 0;
        WRITE_ONE_TO_CLEAR(LTC0_STA, LTC_STA_DI);                        // reset the done interrupt
        #else
        mmcau_aes_set_key(ptrKey, iKeyLength, aes_instance[iInstance].key);
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
        #endif
    #elif defined CRYPTO_OPEN_SSL
        private_AES_set_encrypt_key(ptrKey, iKeyLength, &aes_instance[iInstance].aes_encrypt_context);
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    #elif defined CRYPTO_WOLF_SSL
        wc_AesSetKey(&aes_instance[iInstance].aes_encrypt_context, ptrKey, (iKeyLength/8), NULL, AES_ENCRYPTION); // initial vectors are zeroed in the function
    #else                                                                // mbedTLS
        aes_setkey_enc(&aes_instance[iInstance].aes_encrypt_context, ptrKey, iKeyLength); // set encryption key
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    #endif
    }
    else if ((iInstanceCommand & AES_COMMAND_AES_SET_KEY_DECRYPT) != 0) {
    #if defined NATIVE_AES_CAU
        #if defined LTC_AVAILABLE && !defined AES_DISABLE_LTC
        // To do..
        //
        #else
        mmcau_aes_set_key(ptrKey, iKeyLength, aes_instance[iInstance].key);
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
        #endif
    #elif defined CRYPTO_OPEN_SSL
        private_AES_set_decrypt_key(ptrKey, iKeyLength, &aes_instance[iInstance].aes_encrypt_context);
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    #elif defined CRYPTO_WOLF_SSL
        wc_AesSetKey(&aes_instance[iInstance].aes_encrypt_context, ptrKey, (iKeyLength/8), NULL, AES_DECRYPTION); // initial vectors are zeroed in the function
    #else
        aes_setkey_dec(&aes_instance[iInstance].aes_encrypt_context, ptrKey, iKeyLength); // set decryption key
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    #endif
    }
    else if ((iInstanceCommand & AES_COMMAND_AES_PRIME_IV) != 0) {
    #if defined NATIVE_AES_CAU && defined LTC_AVAILABLE && !defined AES_DISABLE_LTC
        LTC0_CTX_0 = ((ptrKey[3] << 24) | (ptrKey[2] << 16) | (ptrKey[1] << 8) | ptrKey[0]); // zero IV
        LTC0_CTX_1 = ((ptrKey[7] << 24) | (ptrKey[6] << 16) | (ptrKey[5] << 8) | ptrKey[4]);
        LTC0_CTX_2 = ((ptrKey[11] << 24) | (ptrKey[10] << 16) | (ptrKey[9] << 8) | ptrKey[8]);
        LTC0_CTX_3 = ((ptrKey[15] << 24) | (ptrKey[14] << 16) | (ptrKey[13] << 8) | ptrKey[12]);
    #elif defined CRYPTO_WOLF_SSL
        uMemcpy(aes_instance[iInstance].aes_encrypt_context.reg, ptrKey, sizeof(aes_instance[iInstance].aes_encrypt_context.reg)); // prime the iv
    #else
        uMemcpy(aes_instance[iInstance].iv, ptrKey, sizeof(aes_instance[iInstance].iv)); // prime the iv
    #endif
    }
    else if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) != 0) {
    #if defined NATIVE_AES_CAU && defined LTC_AVAILABLE && !defined AES_DISABLE_LTC
        LTC0_CTX_0 = 0;                                                  // zero IV
        LTC0_CTX_1 = 0;
        LTC0_CTX_2 = 0;
        LTC0_CTX_3 = 0;
    #elif defined CRYPTO_WOLF_SSL && !defined NATIVE_AES_CAU
        uMemset(aes_instance[iInstance].aes_encrypt_context.reg, 0, sizeof(aes_instance[iInstance].aes_encrypt_context.reg)); // zero the initial vector
    #else
        uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
    #endif
    }
    return 0;
}

// AES encryption or decryption
//
extern int fnAES_Cipher(int iInstanceCommand, const unsigned char *ptrTextIn, unsigned char *ptrTextOut, unsigned long ulDataLength)
{
    int iInstance = (iInstanceCommand & AES_INSTANCE_MASK);
    if (iInstance >= AES_INSTANCE_COUNT) {
        return AES_INVALID_INSTANCE_REFERENCE;
    }
    if ((iInstanceCommand & (AES_COMMAND_AES_ENCRYPT | AES_COMMAND_AES_DECRYPT)) != 0) {
    #if (defined CRYPTO_OPEN_SSL || defined CRYPTO_MBEDTLS) && !defined NATIVE_AES_CAU
        int iMode;
    #endif
    #if defined NATIVE_AES_CAU || (defined LTC_AVAILABLE && !defined AES_DISABLE_LTC && !defined _WINDOWS)
        if (aes_instance[iInstance].iRounds == 0) {
            _EXCEPTION("Initialise AES instance before use!!");
            return AES_INSTANCE_NOT_INITIALISED;
        }
    #elif defined CRYPTO_WOLF_SSL || defined CRYPTO_OPEN_SSL
        if (aes_instance[iInstance].aes_encrypt_context.rounds == 0) {
            _EXCEPTION("Initialise AES instance before use!!");
            return AES_INSTANCE_NOT_INITIALISED;
        }
    #else
        if (aes_instance[iInstance].aes_encrypt_context.nr == 0) {
            _EXCEPTION("Initialise AES instance before use!!");
            return AES_INSTANCE_NOT_INITIALISED;
        }
    #endif
        if ((ulDataLength % AES_BLOCK_LENGTH) != 0) {
            _EXCEPTION("Invalid AES data length being used!!");
            uMemset(ptrTextOut, 0, ulDataLength);                        // if a bad length is called we set the ciphertext to zeros so that if it is still sent it will not be plain text in any way (eg. when the plain text input buffer is being used also as output buffer)
            return AES_ENCRYPT_BAD_LENGTH;
        }
    #if defined CRYPTO_WOLF_SSL && !defined NATIVE_AES_CAU
        if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) != 0) {
            uMemset(aes_instance[iInstance].aes_encrypt_context.reg, 0, sizeof(aes_instance[iInstance].aes_encrypt_context.reg)); // zero the initial vector
        }
        if ((iInstanceCommand & AES_COMMAND_AES_ENCRYPT) != 0) {
        #if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE
            if (((CAST_POINTER_ARITHMETIC)ptrTextOut & 0x3) != 0) {      // mmCAU requires long word aligned output buffer
                _EXCEPTION("Check ciphertext buffer alignment!");
                uMemset(ptrTextOut, 0, ulDataLength);                    // if a bad length is called we set the ciphertext to zeros so that if it is still sent it will not be plain text in any way (eg. when the plain text input buffer is being used also as output buffer)
                return AES_ENCRYPT_BAD_ALIGNMENT;
            }
        #endif
            wc_AesCbcEncrypt(&aes_instance[iInstance].aes_encrypt_context, ptrTextOut, ptrTextIn, ulDataLength);
        }
        else {
        #if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE
            if (((CAST_POINTER_ARITHMETIC)ptrTextIn & 0x3) != 0) {       // mmCAU requires long word aligned output buffer
                _EXCEPTION("Check plaintext buffer alignment!");
                return AES_ENCRYPT_BAD_ALIGNMENT;
            }
        #endif
            wc_AesCbcDecrypt(&aes_instance[iInstance].aes_encrypt_context, ptrTextOut, ptrTextIn, ulDataLength);
        }
    #else
        #if !defined NATIVE_AES_CAU
        if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) != 0) {        // reset the initial vector before starting encrypting/decrypting a new block
            uMemset(aes_instance[iInstance].iv, 0, sizeof(aes_instance[iInstance].iv));
        }
        #endif
        if ((iInstanceCommand & AES_COMMAND_AES_ENCRYPT) != 0) {         // encrypt data
        #if defined NATIVE_AES_CAU
            register unsigned long *ptrPlainTextInput = (unsigned long *)ptrTextIn;
            register unsigned long *ptrCipherTextOutput = (unsigned long *)ptrTextOut;
            #if !defined LTC_AVAILABLE || defined AES_DISABLE_LTC
            register unsigned long *ptrInputVector = (unsigned long *)aes_instance[iInstance].iv;
            #endif
        #endif
        #if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE
            #if defined NATIVE_AES_CAU
            if ((((CAST_POINTER_ARITHMETIC)ptrPlainTextInput | (CAST_POINTER_ARITHMETIC)ptrCipherTextOutput) & 0x3) != 0) // mmCAU requires long word aligned output buffer and input buffer should also be aligned for efficiency
            #else
            if (((CAST_POINTER_ARITHMETIC)ptrTextOut & 0x3) != 0)        // mmCAU requires long word aligned output buffer
            #endif
            {
                _EXCEPTION("Check buffer alignment!");
                uMemset(ptrTextOut, 0, ulDataLength);                    // if a bad length is called we set the ciphertext to zeros so that if it is still sent it will not be plain text in any way (eg. when the plain text input buffer is being used also as output buffer)
                return AES_ENCRYPT_BAD_ALIGNMENT;
            }
        #endif
        #if defined NATIVE_AES_CAU
            #if defined LTC_AVAILABLE && !defined AES_DISABLE_LTC
            LTC0_MD = (LTC_MD_ENC_ENCRYPT | LTC_MD_AS_UPDATE | LTC_MD_ALG_AES | LTC_MD_AAI_CBC); // set the mode
            if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) != 0) {    // if the initial vector is to be reset before start
                LTC0_CTX_0 = 0;                                          // zero IV
                LTC0_CTX_1 = 0;
                LTC0_CTX_2 = 0;
                LTC0_CTX_3 = 0;
            }
            while (ulDataLength != 0) {
                unsigned long ulThisLengthIn;
                unsigned long ulThisLengthOut;
                if (ulDataLength > 0xff0) {
                    ulThisLengthIn = 0xff0;
                }
                else {
                    ulThisLengthIn = ulDataLength;
                }
                ulDataLength -= ulThisLengthIn;
                LTC0_DS = ulThisLengthIn;                                // write the data size
                ulThisLengthIn /= sizeof(unsigned long);                 // the number of long words
                ulThisLengthOut = ulThisLengthIn;
                while (ulThisLengthIn-- != 0) {                          // copy to the input FIFO and read from the output FIFO
                    while ((LTC0_FIFOSTA & LTC_FIFOSTA_IFF) != 0) {      // if the input FIFO is full we must wait before adding further data
                        if ((LTC0_FIFOSTA & LTC_FIFOSTA_OFL_MASK) != 0) {// if there is at least one output result ready
                            *ptrCipherTextOutput++ = LTC0_OFIFO;
                            ulThisLengthOut--;
                        }
                    }
                    LTC0_IFIFO = *ptrPlainTextInput++;                   // long word aligned
                    if ((LTC0_FIFOSTA & LTC_FIFOSTA_OFL_MASK) != 0) {    // if there is at least one output result ready
                        *ptrCipherTextOutput++ = LTC0_OFIFO;
                        ulThisLengthOut--;
                    }
                }
                while (ulThisLengthOut != 0) {
                    if ((LTC0_FIFOSTA & LTC_FIFOSTA_OFL_MASK) != 0) {    // if there is at least one output result ready
                        *ptrCipherTextOutput++ = LTC0_OFIFO;
                        ulThisLengthOut--;
                    }
                }
                while ((LTC0_STA & LTC_STA_DI) == 0) {}                  // wait for completion
                WRITE_ONE_TO_CLEAR(LTC0_CW, LTC_CW_CDS);                 // clear the data size
                WRITE_ONE_TO_CLEAR(LTC0_STA, LTC_STA_DI);                // reset the done interrupt
                LTC0_MD = (LTC_MD_ENC_ENCRYPT | LTC_MD_AS_UPDATE | LTC_MD_ALG_AES | LTC_MD_AAI_CBC); // re-write the mode
            }
            WRITE_ONE_TO_CLEAR(LTC0_CW, (LTC_CW_CM | LTC_CW_CDS | LTC_CW_CICV | LTC_CW_CCR | LTC_CW_CKR | LTC_CW_CPKA | LTC_CW_CPKB | LTC_CW_CPKN | LTC_CW_CPKE | LTC_CW_COF | LTC_CW_CIF)); // clear internal registers
            #else
            if (ulDataLength >= 16) {                                    // first block
                if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) != 0) {// if the initial vector is to be reset before start
                    ptrCipherTextOutput[0] = ptrPlainTextInput[0];       // iv[] are zero so don't XOR the plaintext input with it (this can be performed as long word copies since both are long word aligned)
                    ptrCipherTextOutput[1] = ptrPlainTextInput[1];
                    ptrCipherTextOutput[2] = ptrPlainTextInput[2];
                    ptrCipherTextOutput[3] = ptrPlainTextInput[3];
                }
                else {
                    ptrCipherTextOutput[0] = (ptrPlainTextInput[0] ^ ptrInputVector[0]); // this can be performed as long word copies since both are long word aligned
                    ptrCipherTextOutput[1] = (ptrPlainTextInput[1] ^ ptrInputVector[1]);
                    ptrCipherTextOutput[2] = (ptrPlainTextInput[2] ^ ptrInputVector[2]);
                    ptrCipherTextOutput[3] = (ptrPlainTextInput[3] ^ ptrInputVector[3]);
                }
                mmcau_aes_encrypt((const unsigned char *)ptrCipherTextOutput, aes_instance[iInstance].key, aes_instance[iInstance].iRounds, (unsigned char *)ptrCipherTextOutput);
                ptrInputVector[0] = ptrCipherTextOutput[0];              // this can be performed as long word copies since both are long word aligned
                ptrInputVector[1] = ptrCipherTextOutput[1];
                ptrInputVector[2] = ptrCipherTextOutput[2];
                ptrInputVector[3] = ptrCipherTextOutput[3];
                ptrPlainTextInput += 4;
                ptrCipherTextOutput += 4;
                ulDataLength -= 16;
            }
            while (ulDataLength >= 16) {                                 // for each subsequent block
                ptrCipherTextOutput[0] = (ptrPlainTextInput[0] ^ ptrInputVector[0]); // this can be performed as long word copies since both are long word aligned
                ptrCipherTextOutput[1] = (ptrPlainTextInput[1] ^ ptrInputVector[1]);
                ptrCipherTextOutput[2] = (ptrPlainTextInput[2] ^ ptrInputVector[2]);
                ptrCipherTextOutput[3] = (ptrPlainTextInput[3] ^ ptrInputVector[3]);
                mmcau_aes_encrypt((const unsigned char *)ptrCipherTextOutput, aes_instance[iInstance].key, aes_instance[iInstance].iRounds, (unsigned char *)ptrCipherTextOutput);
                ptrInputVector[0] = ptrCipherTextOutput[0];              // this can be performed as long word copies since both are long word aligned
                ptrInputVector[1] = ptrCipherTextOutput[1];
                ptrInputVector[2] = ptrCipherTextOutput[2];
                ptrInputVector[3] = ptrCipherTextOutput[3];
                ptrPlainTextInput += 4;
                ptrCipherTextOutput += 4;
                ulDataLength -= 16;
            }
            #endif
            return 0;
        #else
            iMode = AES_ENCRYPT;
        #endif
        }
        else {                                                           // decryption
        #if defined NATIVE_AES_CAU
            register unsigned long *ptrCipherTextInput = (unsigned long *)ptrTextIn;
            register unsigned long *ptrPlainTextOutput = (unsigned long *)ptrTextOut;
            #if !defined LTC_AVAILABLE || defined AES_DISABLE_LTC
            register unsigned long *ptrInputVector = (unsigned long *)aes_instance[iInstance].iv;
            #endif
        #endif
        #if defined CAU_V1_AVAILABLE || defined CAU_V2_AVAILABLE
            #if defined NATIVE_AES_CAU
            if ((((CAST_POINTER_ARITHMETIC)ptrCipherTextInput | (CAST_POINTER_ARITHMETIC)ptrPlainTextOutput) & 0x3) != 0) // mmCAU requires long word aligned output buffer and input buffer should also be aligned for efficiency
            #else
            if (((CAST_POINTER_ARITHMETIC)ptrTextOut & 0x3) != 0)        // mmCAU requires long word aligned output buffer
            #endif
            {
                _EXCEPTION("Check buffer alignment!");
                uMemset(ptrTextOut, 0, ulDataLength);                    // if a bad length is called we set the ciphertext to zeros so that if it is still sent it will not be plain text in any way (eg. when the plain text input buffer is being used also as output buffer)
                return AES_ENCRYPT_BAD_ALIGNMENT;
            }
        #endif
        #if defined NATIVE_AES_CAU
            #if defined LTC_AVAILABLE && !defined AES_DISABLE_LTC
            #else
            if (ptrCipherTextInput == ptrPlainTextOutput) {              // if the same input buffer being used as output buffer
                unsigned long ulBackup[4];                               // if the decryption is being performed in place we need to backup the original decrypted block to be able to create the following initial vector
                if (ulDataLength >= 16) {                                // first block
                    ulBackup[0] = ptrCipherTextInput[0];                 // backup the input cipher block
                    ulBackup[1] = ptrCipherTextInput[1];
                    ulBackup[2] = ptrCipherTextInput[2];
                    ulBackup[3] = ptrCipherTextInput[3];
                    mmcau_aes_decrypt((const unsigned char *)ptrCipherTextInput, aes_instance[iInstance].key, aes_instance[iInstance].iRounds, (unsigned char *)ptrPlainTextOutput); // decrypt a block
                    if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) == 0) { // if iv[] are zero we don't XOR the plaintext input with it
                        ptrPlainTextOutput[0] ^= ptrInputVector[0];      // this can be performed as long word copies since both are long word aligned
                        ptrPlainTextOutput[1] ^= ptrInputVector[1];
                        ptrPlainTextOutput[2] ^= ptrInputVector[2];
                        ptrPlainTextOutput[3] ^= ptrInputVector[3];
                    }
                    ptrInputVector[0] = ulBackup[0];                     // next iv[] is the input before descrypting (this can be performed as long word copies since both are long word aligned)
                    ptrInputVector[1] = ulBackup[1];
                    ptrInputVector[2] = ulBackup[2];
                    ptrInputVector[3] = ulBackup[3];
                    ptrCipherTextInput += 4;
                    ptrPlainTextOutput += 4;
                    ulDataLength -= 16;
                }
                while (ulDataLength >= 16) {                             // subsequent blocks
                    ulBackup[0] = ptrCipherTextInput[0];                 // backup the input cipher block
                    ulBackup[1] = ptrCipherTextInput[1];
                    ulBackup[2] = ptrCipherTextInput[2];
                    ulBackup[3] = ptrCipherTextInput[3];
                    mmcau_aes_decrypt((const unsigned char *)ptrCipherTextInput, aes_instance[iInstance].key, aes_instance[iInstance].iRounds, (unsigned char *)ptrPlainTextOutput); // decrypt a block
                    ptrPlainTextOutput[0] ^= ptrInputVector[0];          // this can be performed as long word copies since both are long word aligned
                    ptrPlainTextOutput[1] ^= ptrInputVector[1];
                    ptrPlainTextOutput[2] ^= ptrInputVector[2];
                    ptrPlainTextOutput[3] ^= ptrInputVector[3];
                    ptrInputVector[0] = ulBackup[0];                     // next iv[] is the input before descrypting (this can be performed as long word copies since both are long word aligned)
                    ptrInputVector[1] = ulBackup[1];
                    ptrInputVector[2] = ulBackup[2];
                    ptrInputVector[3] = ulBackup[3];
                    ptrCipherTextInput += 4;
                    ptrPlainTextOutput += 4;
                    ulDataLength -= 16;
                }
            }
            else {                                                       // not in place (no temporary buffer needed
                if (ulDataLength >= 16) {                                // first block
                    mmcau_aes_decrypt((const unsigned char *)ptrCipherTextInput, aes_instance[iInstance].key, aes_instance[iInstance].iRounds, (unsigned char *)ptrPlainTextOutput); // decrypt a block
                    if ((iInstanceCommand & AES_COMMAND_AES_RESET_IV) == 0) { // if iv[] are zero we don't XOR the plaintext input with it
                        ptrPlainTextOutput[0] ^= ptrInputVector[0];      // this can be performed as long word copies since both are long word aligned
                        ptrPlainTextOutput[1] ^= ptrInputVector[1];
                        ptrPlainTextOutput[2] ^= ptrInputVector[2];
                        ptrPlainTextOutput[3] ^= ptrInputVector[3];
                    }
                    ptrInputVector[0] = ptrCipherTextInput[0];           // next iv[] is the input before descrypting (this can be performed as long word copies since both are long word aligned)
                    ptrInputVector[1] = ptrCipherTextInput[1];
                    ptrInputVector[2] = ptrCipherTextInput[2];
                    ptrInputVector[3] = ptrCipherTextInput[3];
                    ptrCipherTextInput += 4;
                    ptrPlainTextOutput += 4;
                    ulDataLength -= 16;
                }
                while (ulDataLength >= 16) {                             // subsequent blocks
                    mmcau_aes_decrypt((const unsigned char *)ptrCipherTextInput, aes_instance[iInstance].key, aes_instance[iInstance].iRounds, (unsigned char *)ptrPlainTextOutput); // decrypt a block
                    ptrPlainTextOutput[0] ^= ptrInputVector[0];          // this can be performed as long word copies since both are long word aligned
                    ptrPlainTextOutput[1] ^= ptrInputVector[1];
                    ptrPlainTextOutput[2] ^= ptrInputVector[2];
                    ptrPlainTextOutput[3] ^= ptrInputVector[3];
                    ptrInputVector[0] = ptrCipherTextInput[0];           // next iv[] is the input before descrypting (this can be performed as long word copies since both are long word aligned)
                    ptrInputVector[1] = ptrCipherTextInput[1];
                    ptrInputVector[2] = ptrCipherTextInput[2];
                    ptrInputVector[3] = ptrCipherTextInput[3];
                    ptrCipherTextInput += 4;
                    ptrPlainTextOutput += 4;
                    ulDataLength -= 16;
                }
            }
            #endif
        #else
            iMode = AES_DECRYPT;
        #endif
        }
        #if !defined NATIVE_AES_CAU
            #if defined CRYPTO_OPEN_SSL
        AES_cbc_encrypt(ptrTextIn, ptrTextOut, ulDataLength, (const AES_KEY *)&aes_instance[iInstance], aes_instance[iInstance].iv, iMode); // encrypt always using cipher block chaining
            #else
        aes_crypt_cbc(&aes_instance[iInstance].aes_encrypt_context, iMode, ulDataLength, aes_instance[iInstance].iv, ptrTextIn, ptrTextOut); // encrypt always using cipher block chaining
            #endif
        #endif
    #endif
    }
    return 0;
}
#endif


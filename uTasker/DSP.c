/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      DSP.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    
*/        

#include "config.h"
#if defined CMSIS_DSP_CFFT
    #include "../../Hardware/CMSIS_DSP/arm_const_structs.h"              // include defines required for the use of ARM CMSIS FFT

#if defined _WINDOWS
static size_t reverse_bits(size_t x, int n) {
    size_t result = 0;
    for (int i = 0; i < n; i++, x >>= 1) {
        result = (result << 1) | (x & 1);
    }
    return result;
}
#endif

extern float fnGenerateWindowFloat(float *ptrWindowBuffer, int iInputSamples, int iWindowType)
{
    #define coeffa0 (double)0.35875                                      // Blackmann-Harris cooefficients for the calculation
    #define coeffa1 (double)0.48829
    #define coeffa2 (double)0.14128
    #define coeffa3 (double)0.01168
    int i;
    float window_conversionFactor = 0;                                   // scaling factor due to windowing
    #if defined _WINDOWS


#define FFT_LENGTH 16                                                                      // Compute levels = floor(log2(n))
    int levels = 0;
    {
        int temp = FFT_LENGTH;

        while (temp > 1) {
            levels++;
            temp >>= 1;
        }
        if (1u << levels != FFT_LENGTH)
            return 0;  // n is not a power of 2
    }

    {
        int iLengthOfIt = 0;
        int iError = 0;
        unsigned short x[10000];
        // Bit-reversed addressing permutation
        for (i = 0; i < FFT_LENGTH; i++) {
            int j = reverse_bits(i, levels);
            if (j > i) {
                x[iLengthOfIt] = (i * sizeof(float) * 2);
                if (armBitRevIndexTable_fixed_4096[iLengthOfIt] != x[iLengthOfIt]) {
                    iError++;
                }
                iLengthOfIt++;
                x[iLengthOfIt] = (j * sizeof(float) * 2);
                if (armBitRevIndexTable_fixed_4096[iLengthOfIt] != x[iLengthOfIt]) {
                    iError++;
                }
                iLengthOfIt++;
            }
        }
        iLengthOfIt = iLengthOfIt;
    }




    if (iWindowType != BLACKMANN_HARRIS_WINDOW) {
        _EXCEPTION("Only Blackmann-Harris windows supported!!");
    }
    #endif
    for (i = 0; i < iInputSamples; i++) {
        ptrWindowBuffer[i] = (float)(coeffa0 - (coeffa1 * (cos((2 * PI * i) / (iInputSamples - 1)))) + (coeffa2 * (cos((4 * PI * i) / (iInputSamples - 1)))) - (coeffa3 * (cos((6 * PI * i) / (iInputSamples - 1)))));
        window_conversionFactor += (ptrWindowBuffer[i] * ptrWindowBuffer[i]); // sum the squares
    }
    window_conversionFactor /= (float)iInputSamples;
    return (float)(1.0 / window_conversionFactor);                       // this value is used for compensating the amplitude loss due to the window
}


#if defined CMSIS_DSP_FFT_8192                                           // define the required dimension of the FFT input buffer based on the FFT size to be tested
    #define MAX_FFT_BUFFER_LENGTH    (8192 * 2)
#elif defined CMSIS_DSP_FFT_4096
    #define MAX_FFT_BUFFER_LENGTH    (4096 * 2)
#elif defined CMSIS_DSP_FFT_2048
    #define MAX_FFT_BUFFER_LENGTH    (2048 * 2)
#elif defined CMSIS_DSP_FFT_1024
    #define MAX_FFT_BUFFER_LENGTH    (1024 * 2)
#elif defined CMSIS_DSP_FFT_512
    #define MAX_FFT_BUFFER_LENGTH    (512 * 2)
#elif defined CMSIS_DSP_FFT_256
    #define MAX_FFT_BUFFER_LENGTH    (256 * 2)
#elif defined CMSIS_DSP_FFT_128
    #define MAX_FFT_BUFFER_LENGTH    (128 * 2)
#elif defined CMSIS_DSP_FFT_64
    #define MAX_FFT_BUFFER_LENGTH    (64 * 2)
#elif defined CMSIS_DSP_FFT_32
    #define MAX_FFT_BUFFER_LENGTH    (32 * 2)
#else
    #define MAX_FFT_BUFFER_LENGTH    (16 * 2)
#endif
// Perform complex fast-fourier transform on an input circular sample buffer with optional offset and optional windowing
//
extern int fnFFT(void *ptrInputBuffer, void *ptrOutputBuffer, int iInputSamples, int iSampleOffset, int iInputBufferSize, float *ptrWindowingBuffer, float window_conversionFactor, int iInputOutputType)
{
    float fft_buffer[MAX_FFT_BUFFER_LENGTH];                             // temporary working buffer (for complex inputs and so twice the size)
    #if defined CMSIS_DSP_CFFT_Q15
    signed short fft_buffer_q15[MAX_FFT_BUFFER_LENGTH];                  // temporary working buffer (for complex inputs and so twice the size)
    #endif
    #if defined CMSIS_DSP_CFFT_FLOAT
    const arm_cfft_instance_f32 *ptrFFT_consts_f32 = 0;
    #endif
    #if defined CMSIS_DSP_CFFT_Q15
    const arm_cfft_instance_q15 *ptrFFT_consts_q15 = 0;;
    #endif
    int iInput = iSampleOffset;                                          // original input offset
    int iCopyLimit = iInputBufferSize;
    int ifft_sample = 0;
    switch (iInputSamples) {                                             // select the appropriate FFT coefficient values
    #if defined CMSIS_DSP_FFT_16
    case 16:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len16;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len16;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_32
    case 32:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len32;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len32;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_64
    case 64:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len64;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len64;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_128
    case 128:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len128;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len128;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_256
    case 256:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len256;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len256;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_512
    case 512:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len512;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len512;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_1024
    case 1024:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len1024;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len1024;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_2048
    case 2048:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len2048;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len2048;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_4096
    case 4096:
        #if defined CMSIS_DSP_CFFT_FLOAT
        ptrFFT_consts_f32 = &arm_cfft_sR_f32_len4096;
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len4096;
        #endif
        break;
    #endif
    #if defined CMSIS_DSP_FFT_8192
    case 8192:                                                           // added to CMSIS FFT code
        #if defined CMSIS_DSP_CFFT_FLOAT
        if ((iInputOutputType & FFT_RAM_COEFFICIENTS) != 0) {
            static arm_cfft_instance_f32 *ram_arm_cfft_sR_f32_len8192 = 0;
            if (ram_arm_cfft_sR_f32_len8192 == 0) {
                ram_arm_cfft_sR_f32_len8192 = uMalloc(sizeof(arm_cfft_instance_f32));
                ram_arm_cfft_sR_f32_len8192->bitRevLength = arm_cfft_sR_f32_len8192.bitRevLength;
                ram_arm_cfft_sR_f32_len8192->fftLen = arm_cfft_sR_f32_len8192.fftLen;
                ram_arm_cfft_sR_f32_len8192->pTwiddle = uMalloc(sizeof(float) * arm_cfft_sR_f32_len8192.fftLen);
                memcpy((float *)(ram_arm_cfft_sR_f32_len8192->pTwiddle), arm_cfft_sR_f32_len8192.pTwiddle, (sizeof(float) * arm_cfft_sR_f32_len8192.fftLen * 2));
              //ram_arm_cfft_sR_f32_len8192->pBitRevTable = uMalloc(sizeof(unsigned short) * arm_cfft_sR_f32_len8192.bitRevLength);
              //memcpy((unsigned short *)(ram_arm_cfft_sR_f32_len8192->pBitRevTable), arm_cfft_sR_f32_len8192.pBitRevTable, (sizeof(unsigned short) * arm_cfft_sR_f32_len8192.bitRevLength));
                ram_arm_cfft_sR_f32_len8192->pBitRevTable = arm_cfft_sR_f32_len8192.pBitRevTable;
            }
            ptrFFT_consts_f32 = ram_arm_cfft_sR_f32_len8192;
        }
        else {
            ptrFFT_consts_f32 = &arm_cfft_sR_f32_len8192;
        }
        #endif
        #if defined CMSIS_DSP_CFFT_Q15
        ptrFFT_consts_q15 = &arm_cfft_sR_q15_len8192;
        #endif
        break;
    #endif
    default:
        _EXCEPTION("Invalid FFT length!!!");
        return -1;
    }
  //TOGGLE_TEST_OUTPUT();                                                // start measurement of processing time
    switch (iInputOutputType & FFT_INPUT_MASK) {
    case FFT_INPUT_FLOATS:                                               // input samples are floats
        {
            float *_ptrInputBuffer = (float *)ptrInputBuffer;
            do {                                                         // transfer input from a circular input buffer to a linear fft buffer with complex sample inputs
                if ((iCopyLimit - iInput) > iInputSamples){
                    iCopyLimit = (iInput + iInputSamples);
                }
                while (iInput < iCopyLimit) {
    #if defined _WINDOWS
                    fnInjectSine(0, INJECT_SINE_FLOATS, &_ptrInputBuffer[iInput], sizeof(float)); // when simulating we inject a sine wave for test purposes
    #endif
                    fft_buffer[ifft_sample] = _ptrInputBuffer[iInput++]; // transfer the input sample to the fft buffer (real component)
                    if (ptrWindowingBuffer != 0) {                       // if windowing is to be applied
                        fft_buffer[ifft_sample] *= *ptrWindowingBuffer++;// perform windowing
                    }
                    fft_buffer[++ifft_sample] = 0.0;                     // insert zeroed imaginary component
                    ++ifft_sample;
                }
                iInput = 0;                                              // circle back to the input of the input buffer
                iCopyLimit -= iSampleOffset;                             // set the new copy limit
            } while (ifft_sample < (iInputSamples * 2));                 // until a complete complex input buffer has been prepared
        }
        break;
    case FFT_INPUT_HALF_WORDS_SIGNED:                                    // input samples are signed shorts
        {
            signed short *_ptrInputBuffer = (signed short *)ptrInputBuffer;
            do {                                                         // transfer input from a circular input buffer to a linear fft buffer with complex sample inputs
                if ((iCopyLimit - iInput) > iInputSamples){
                    iCopyLimit = (iInput + iInputSamples);
                }
                while (iInput < iCopyLimit) {
    #if defined _WINDOWS
                    fnInjectSine(0, INJECT_SINE_HALF_WORDS_SIGNED, &_ptrInputBuffer[iInput], sizeof(signed short)); // when simulating we inject a sine wave for test purposes
    #endif
                    fft_buffer[ifft_sample] = _ptrInputBuffer[iInput++]; // transfer the input sample to the fft buffer (real component)
                    if (ptrWindowingBuffer != 0) {                       // if windowing is to be applied
                        fft_buffer[ifft_sample] *= *ptrWindowingBuffer++;// perform windowing
                    }
                    fft_buffer[++ifft_sample] = 0.0;                     // insert zeroed imaginary component
                    ++ifft_sample;
                }
                iInput = 0;                                              // circle back to the input of the input buffer
                iCopyLimit -= iSampleOffset;                             // set the new copy limit
            } while (ifft_sample < (iInputSamples * 2));                 // until a complete complex input buffer has been prepared
        }
        break;
    case FFT_INPUT_HALF_WORDS_UNSIGNED:                                  // input samples are unsigned shorts
        {
            unsigned short *_ptrInputBuffer = (unsigned short *)ptrInputBuffer;
            do {                                                         // transfer input from a circular input buffer to a linear fft buffer with complex sample inputs
                if ((iCopyLimit - iInput) > iInputSamples){
                    iCopyLimit = (iInput + iInputSamples);
                }
                while (iInput < iCopyLimit) {
    #if defined _WINDOWS
                    fnInjectSine(0, INJECT_SINE_HALF_WORDS_UNSIGNED, &_ptrInputBuffer[iInput], sizeof(unsigned short)); // when simulating we inject a sine wave for test purposes
    #endif
                    fft_buffer[ifft_sample] = _ptrInputBuffer[iInput++]; // transfer the input sample to the fft buffer (real component)
                    if (ptrWindowingBuffer != 0) {                       // if windowing is to be applied
                        fft_buffer[ifft_sample] *= *ptrWindowingBuffer++;// perform windowing
                    }
                    fft_buffer[++ifft_sample] = 0.0;                     // insert zeroed imaginary component
                    ++ifft_sample;
                }
                iInput = 0;                                              // circle back to the input of the input buffer
                iCopyLimit -= iSampleOffset;                             // set the new copy limit
            } while (ifft_sample < (iInputSamples * 2));                 // until a complete complex input buffer has been prepared
        }
        break;
    default:
        _EXCEPTION("Invalid input type!!!");
      //TOGGLE_TEST_OUTPUT();                                            // start measurement of processing time
        return -1;
    }
  //TOGGLE_TEST_OUTPUT();                                                // stop/start measurement of processing time
    if ((iInputOutputType & FFT_CALCULATION_Q15) != 0) {
        #if defined CMSIS_DSP_CFFT_Q15
      //fft_buffer_q15[];                                                // copy the windowed input to the q15 buffer
        arm_cfft_q15(ptrFFT_consts_q15, fft_buffer_q15, 0, 1);           // perform q15 FFT
      //fft_buffer_q15[];                                                // copy result to the float buffer
        #endif
    }
    else {
    #if defined CMSIS_DSP_CFFT_FLOAT
        arm_cfft_f32(ptrFFT_consts_f32, fft_buffer, 0, 1);               // perform an in-place complex FFT
    #endif
    }
  //TOGGLE_TEST_OUTPUT();                                                // stop/start measurement of processing time
    switch (iInputOutputType & FFT_OUTPUT_MASK) {
    case FFT_OUTPUT_FLOATS:
        if ((iInputOutputType & FFT_MAGNITUDE_RESULT) != 0) {            // if the magnitudes are required
            float *ptrFloatOutputBuffer = (float *)ptrOutputBuffer;
            iInputSamples /= 2;                                          // half the values are of interest (the second half is a mirrored version of the first half)
            arm_cmplx_mag_f32(fft_buffer, ptrFloatOutputBuffer, (unsigned long)iInputSamples); // calculate the magnitude of each frequency component            
            if (ptrWindowingBuffer != 0) {
              //TOGGLE_TEST_OUTPUT();                                    // start/stop measurement of processing time
                while (iInputSamples-- > 0) {
                    *ptrFloatOutputBuffer++ *= window_conversionFactor;  // compensate each frequency amplitude with the windowing coefficient
                }
            }
        }
        else {                                                           // complex result is to be returned
            uMemcpy(ptrOutputBuffer, fft_buffer, (iInputSamples * sizeof(float))); // it is expected that the output buffer is float of adequate size!
          //TOGGLE_TEST_OUTPUT();                                        // start/stop measurement of processing time
            return 0;
        }
        break;
    default:
        _EXCEPTION("Invalid output type!!!");
        return -1;
    }
  //TOGGLE_TEST_OUTPUT();                                                // stop measurement of processing time
    return 0;
}
#endif

#if defined _WINDOWS
    #if !defined CMSIS_DSP_CFFT
        #include "../../Hardware/CMSIS_DSP/arm_const_structs.h"          // include defines required for the use of ARM CMSIS FFT
    #endif
    #if !defined SINE_INSTANCES
        #define SINE_INSTANCES 1
    #endif
extern void fnInjectSine(int instance, int iType, void *ptrData, unsigned short usLength)
{
    #if !defined PI
        #define PI 3.14159265358979323846
    #endif
    #define TEST_SIG_AMPLITUDE      0x0fff
    #define SAMPLING_FREQUENCY      48000.0
    #define TEST_FREQUENCY          4800
    #define INCREMENT_PHASE  ((2 * PI * TEST_FREQUENCY) / SAMPLING_FREQUENCY)
    static int iInitalised[SINE_INSTANCES] = {0};
    static double phase_angle[SINE_INSTANCES] = {0.0};
    static double phase_increment[SINE_INSTANCES] = {0};
    double result;
    if (iInitalised[instance] == 0) {
        iInitalised[instance] = 1;
        phase_increment[instance] = INCREMENT_PHASE;
    }
    switch (iType) {
    case INJECT_SINE_HALF_WORDS_SIGNED:
    case INJECT_SINE_HALF_WORDS_UNSIGNED:
        while (usLength >= sizeof(unsigned short)) {
            result = sin(phase_angle[instance]);
            result *= TEST_SIG_AMPLITUDE;
            if (iType == INJECT_SINE_HALF_WORDS_UNSIGNED) {
                *(signed short *)ptrData = (signed short)(result + TEST_SIG_AMPLITUDE);
            }
            else {
                *(unsigned short *)ptrData = (signed short)result;
            }
            (unsigned long)ptrData = (unsigned long)ptrData + sizeof(unsigned short);
            phase_angle[instance] += phase_increment[instance];
            if (phase_angle[instance] >= (2 * PI)) {
                phase_angle[instance] -= (2 * PI);
            }
            usLength -= sizeof(unsigned short);
        }
        break;
    default:
        _EXCEPTION("Only half-words supported!!");
        break;
    }
}
#endif


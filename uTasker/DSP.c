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
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    
*/        

#include "config.h"
#if defined CMSIS_DSP_CFFT
    #include "../../Hardware/Kinetis/CMSIS_DSP/arm_const_structs.h"      // include defines required for the use of ARM CMSIS FFT

extern float fnGenerateWindowFloat(float *ptrWindowBuffer, int iInputSamples, int iWindowType)
{
    #define coeffa0 (double)0.35875                                      // Blackmann-Harris cooefficients for the calculation
    #define coeffa1 (double)0.48829
    #define coeffa2 (double)0.14128
    #define coeffa3 (double)0.01168
    int i;
    float window_conversionFactor = 0;                                   // scaling factor due to windowing
    #if defined _WINDOWS
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


#if defined CMSIS_DSP_FFT_4096                                           // define the required dimension of the FFT input buffer based on the FFT size to be tested
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
    const arm_cfft_instance_f32 *ptrFFT_consts = 0;
    int iInput = iSampleOffset;                                          // original input offset
    int iCopyLimit = iInputBufferSize;
    int ifft_sample = 0;
    switch (iInputSamples) {                                             // select the appropriate FFT values
    #if defined CMSIS_DSP_FFT_16
    case 16:
        ptrFFT_consts = &arm_cfft_sR_f32_len16;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_32
    case 32:
        ptrFFT_consts = &arm_cfft_sR_f32_len32;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_64
    case 64:
        ptrFFT_consts = &arm_cfft_sR_f32_len64;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_128
    case 128:
        ptrFFT_consts = &arm_cfft_sR_f32_len128;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_256
    case 256:
        ptrFFT_consts = &arm_cfft_sR_f32_len256;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_512
    case 512:
        ptrFFT_consts = &arm_cfft_sR_f32_len512;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_1024
    case 1024:
        ptrFFT_consts = &arm_cfft_sR_f32_len1024;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_2048
    case 2048:
        ptrFFT_consts = &arm_cfft_sR_f32_len2048;
        break;
    #endif
    #if defined CMSIS_DSP_FFT_4096
    case 4096:
        ptrFFT_consts = &arm_cfft_sR_f32_len4096;
        break;
    #endif
    default:
        _EXCEPTION("Invalid FFT length!!!");
        return -1;
    }
    TOGGLE_TEST_OUTPUT();                                                // start measurement of processing time
    switch (iInputOutputType & FFT_INPUT_MASK) {
    case FFT_INPUT_FLOATS:                                               // input samples are signed shorts
        {
            float * _ptrInputBuffer = (float *)ptrInputBuffer;
            do {                                                         // transfer input from a circular input buffer to a linear fft buffer with complex sample inputs
                if ((iCopyLimit - iInput) > iInputSamples){
                    iCopyLimit = (iInput + iInputSamples);
                }
                while (iInput < iCopyLimit) {
                    fft_buffer[ifft_sample] = _ptrInputBuffer[iInput++]; // transfer the input sample to the fft buffer (real component)
                    if (ptrWindowingBuffer != 0) {                       // if windowing is to be applied
                        fft_buffer[ifft_sample] += *ptrWindowingBuffer++;// perform windowing
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
            signed short * _ptrInputBuffer = (signed short *)ptrInputBuffer;
            do {                                                         // transfer input from a circular input buffer to a linear fft buffer with complex sample inputs
                if ((iCopyLimit - iInput) > iInputSamples){
                    iCopyLimit = (iInput + iInputSamples);
                }
                while (iInput < iCopyLimit) {
                    fft_buffer[ifft_sample] = _ptrInputBuffer[iInput++]; // transfer the input sample to the fft buffer (real component)
                    if (ptrWindowingBuffer != 0) {                       // if windowing is to be applied
                        fft_buffer[ifft_sample] += *ptrWindowingBuffer++;// perform windowing
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
        TOGGLE_TEST_OUTPUT();                                            // start measurement of processing time
        return -1;
    }
    TOGGLE_TEST_OUTPUT();                                                // stop/start measurement of processing time
    switch (iInputOutputType & FFT_OUTPUT_MASK) {
    case FFT_OUTPUT_FLOATS:
        arm_cfft_f32(ptrFFT_consts, fft_buffer, 0, 1);                   // perform an in-place complex FFT
        TOGGLE_TEST_OUTPUT();                                            // stop/start measurement of processing time
        if ((iInputOutputType & FFT_MAGNITUDE_RESULT) != 0) {            // if the magnitudes are required
            float *ptrFloatOutputBuffer = (float *)ptrOutputBuffer;
            iInputSamples /= 2;                                          // half the values are of interest (the second half is a mirrored version of the first half)
            arm_cmplx_mag_f32(fft_buffer, ptrFloatOutputBuffer, (unsigned long)iInputSamples); // calculate the magnitude of each frequency component            
            if (ptrWindowingBuffer != 0) {
                TOGGLE_TEST_OUTPUT();                                    // start/stop measurement of processing time
                while (iInputSamples-- > 0) {
                    *ptrFloatOutputBuffer++ *= window_conversionFactor;  // compensate each frequency amplitude with the windowing coefficient
                }
            }
        }
        else {                                                           // complex result is to be returned
            uMemcpy(ptrOutputBuffer, fft_buffer, (iInputSamples * sizeof(float))); // it is expected that the output buffer is float of adequate size!
            TOGGLE_TEST_OUTPUT();                                        // start/stop measurement of processing time
            return 0;
        }
        break;
    default:
        _EXCEPTION("Invalid output type!!!");
        return -1;
    }
    TOGGLE_TEST_OUTPUT();                                                // stop measurement of processing time
    return 0;
}
#endif

#if defined _WINDOWS
#if !defined CMSIS_DSP_CFFT
    #include "../../Hardware/Kinetis/CMSIS_DSP/arm_const_structs.h"      // include defines required for the use of ARM CMSIS FFT
#endif
#if !defined SINE_INSTANCES
    #define SINE_INSTANCES 1
#endif
extern void fnInjectSine(int instance, int iType, void *ptrData, unsigned short usLength)
{
    #if !defined PI
        #define PI 3.14159265358979323846
    #endif
    #define TEST_SIG_AMPLITUDE      0x0fff;
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
    if (iType != INJECT_SINE_HALF_WORDS_SIGNED) {
        _EXCEPTION("Only signed shorts supported!!");
    }
    while (usLength >= sizeof(unsigned short)) {
        result = sin(phase_angle[instance]);
        result *= TEST_SIG_AMPLITUDE;
        *(unsigned short *)ptrData = (signed short)result;
        (unsigned long)ptrData = (unsigned long)ptrData + sizeof(unsigned short);
        phase_angle[instance] += phase_increment[instance];
        if (phase_angle[instance] >= (2 * PI)) {
            phase_angle[instance] -= (2 * PI);
        }
        usLength -= sizeof(unsigned short);
    }
}
#endif


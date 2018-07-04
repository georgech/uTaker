/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_timer_pins.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/

// Timer pin configuration that can be used by PWM outputs or capture inputs
//
static void fnConfigTimerPin(int iTimer, int iChannel, unsigned long ulCharacteristics)
{
    switch (iTimer) {
    case 0:                                                              // timer 0
                    switch (iChannel) {                                  // configure appropriate pin for the timer signal
                    case 0:                                              // timer 0, channel 0
    #if defined KINETIS_KE15
        #if defined TPM0_0_ON_B
                        _CONFIG_PERIPHERAL(B, 12, (PB_12_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PB.12 (alt. function 2)
        #elif defined TPM0_0_ON_C
                        _CONFIG_PERIPHERAL(C, 0, (PC_0_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PC.0 (alt. function 2)
        #else
                        _CONFIG_PERIPHERAL(D, 15, (PD_15_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PD.15 (alt. function 2)
        #endif
    #elif defined KINETIS_KE && !defined KINETIS_KE18
        #if defined FTM0_0_ON_B
                        SIM_PINSEL0 |= SIM_PINSEL_FTM0PS0;
                        _CONFIG_PERIPHERAL(B, 2, (PB_2_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PB.2 (alt. function 3)
        #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM0PS0;
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PA.0 (alt. function 2)
        #endif
    #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM0_0_ON_A
                        _CONFIG_PERIPHERAL(A, 6,(PA_6_TPM0_CH0 | ulCharacteristics)); // TPM0_CH0 on PA.6 (alt. function 2)
    #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 11,(PB_11_TPM0_CH0 | ulCharacteristics)); // TPM0_CH0 on PB.11 (alt. function 2)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_0_ON_D
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PD.0 (alt. function 4)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_0_ON_E
                        _CONFIG_PERIPHERAL(E, 24, (PE_24_TPM0_CH0 | ulCharacteristics)); // TPM0_CH0 on PE.24 (alt. function 3)
    #elif defined FTM0_0_ON_C
                        _CONFIG_PERIPHERAL(C, 1, (PC_1_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PC.1 (alt. function 4)
    #else
                        _CONFIG_PERIPHERAL(A, 3, (PA_3_FTM0_CH0 | ulCharacteristics)); // FTM0_CH0 on PA.3 (alt. function 3)
    #endif
                        break;
                    case 1:                                              // timer 0, channel 1
    #if defined KINETIS_KE15
        #if defined TPM0_1_ON_B
                        _CONFIG_PERIPHERAL(B, 13, (PB_13_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PB.13 (alt. function 2)
        #elif defined TPM0_1_ON_C
                        _CONFIG_PERIPHERAL(C, 1, (PC_1_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PC.1 (alt. function 2)
        #else
                        _CONFIG_PERIPHERAL(D, 16, (PD_16_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PD.16 (alt. function 2)
        #endif
    #elif defined KINETIS_KE && !defined KINETIS_KE18
        #if defined FTM0_1_ON_B
                        SIM_PINSEL0 |= SIM_PINSEL_FTM0PS1;
                        _CONFIG_PERIPHERAL(B, 3, (PB_3_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PB.3 (alt. function 3)
        #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM0PS1;
                        _CONFIG_PERIPHERAL(A, 1, (PA_1_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PA.1 (alt. function 2)
        #endif
    #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM0_1_ON_A
                        _CONFIG_PERIPHERAL(A, 5, (PA_5_TPM0_CH1 | ulCharacteristics)); // TPM0_CH1 on PA.5 (alt. function 2)
    #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 10,(PB_10_TPM0_CH1 | ulCharacteristics)); // TPM0_CH1 on PB.10 (alt. function 2)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_1_ON_D
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PD.1 (alt. function 4)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_1_ON_E
                        _CONFIG_PERIPHERAL(E, 25, (PE_25_TPM0_CH1 | ulCharacteristics)); // TPM0_CH1 on PE.25 (alt. function 3)
    #elif defined FTM0_1_ON_C
                        _CONFIG_PERIPHERAL(C, 2, (PC_2_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PC.2 (alt. function 4)
    #else
                        _CONFIG_PERIPHERAL(A, 4, (PA_4_FTM0_CH1 | ulCharacteristics)); // FTM0_CH1 on PA.4 (alt. function 3)
    #endif
                        break;
    #if FLEX_TIMERS_0_CHANNELS > 2
                    case 2:                                              // timer 0, channel 2
        #if defined KINETIS_KE15
            #if defined TPM0_2_ON_B
                        _CONFIG_PERIPHERAL(B, 14, (PB_14_FTM0_CH2 | ulCharacteristics)); // FTM0_CH2 on PB.14 (alt. function 2)
            #elif defined TPM0_2_ON_C
                        _CONFIG_PERIPHERAL(C, 2, (PC_2_FTM0_CH2 | ulCharacteristics)); // FTM0_CH2 on PC.2 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM0_CH2 | ulCharacteristics)); // FTM0_CH2 on PD.0 (alt. function 2)
            #endif
        #elif defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 9, (PB_9_TPM0_CH2 | ulCharacteristics)); // TPM0_CH2 on PB.9 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_2_ON_D
                        _CONFIG_PERIPHERAL(D, 2, (PD_2_FTM0_CH2 | ulCharacteristics)); // FTM0_CH2 on PD.2 (alt. function 4)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_2_ON_E
                        _CONFIG_PERIPHERAL(E, 29, (PE_29_TPM0_CH2 | ulCharacteristics)); // TPM0_CH2 on PE.29 (alt. function 3)
        #elif defined FTM0_2_ON_C
                        _CONFIG_PERIPHERAL(C, 3, (PC_3_FTM0_CH2 | ulCharacteristics)); // FTM0_CH2 on PC.3 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 5, (PA_5_FTM0_CH2 | ulCharacteristics)); // FTM0_CH2 on PA.5 (alt. function 3)
        #endif
                        break;
                    case 3:                                              // timer 0, channel 3
        #if defined KINETIS_KE15
            #if defined TPM0_3_ON_B
                        _CONFIG_PERIPHERAL(B, 15, (PB_15_FTM0_CH3 | ulCharacteristics)); // FTM0_CH3 on PB.15 (alt. function 2)
            #elif defined TPM0_3_ON_C
                        _CONFIG_PERIPHERAL(C, 3, (PC_3_FTM0_CH3 | ulCharacteristics)); // FTM0_CH3 on PC.3 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM0_CH3 | ulCharacteristics)); // FTM0_CH3 on PD.1 (alt. function 2)
            #endif
        #elif defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 8, (PB_8_TPM0_CH3 | ulCharacteristics)); // TPM0_CH3 on PB.8 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_3_ON_D
                        _CONFIG_PERIPHERAL(D, 3, (PD_3_FTM0_CH3 | ulCharacteristics)); // FTM0_CH3 on PD.3 (alt. function 4)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_3_ON_E
                        _CONFIG_PERIPHERAL(E, 30, (PE_30_TPM0_CH3 | ulCharacteristics)); // TPM0_CH3 on PE.30 (alt. function 3)
        #elif (defined FTM0_3_ON_C && (defined KINETIS_K64  || defined KINETIS_KL43 || defined KINETIS_K80 || defined KINETIS_KL82 || defined KINETIS_KL28))
                        _CONFIG_PERIPHERAL(C, 4, (PC_4_FTM0_CH3 | ulCharacteristics)); // FTM0_CH3 on PC.4 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 6, (PA_6_FTM0_CH3 | ulCharacteristics)); // FTM0_CH3 on PA.6 (alt. function 3)
        #endif
                        break;
                    case 4:                                              // timer 0, channel 4
        #if defined KINETIS_KE15
            #if defined TPM0_4_ON_B_HIGH
                        _CONFIG_PERIPHERAL(B, 16, (PB_16_FTM0_CH4 | ulCharacteristics)); // FTM0_CH4 on PB.16 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(B, 4, (PB_4_FTM0_CH4 | ulCharacteristics)); // FTM0_CH4 on PB.4 (alt. function 2)
            #endif
        #elif defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(A, 6, (PA_6_TPM0_CH4 | ulCharacteristics)); // TPM0_CH4 on PA.6 (alt. function 2)
        #elif defined FTM0_4_ON_C && defined KINETIS_KL28
                        _CONFIG_PERIPHERAL(C, 8, (PC_0_TPM0_CH4 | ulCharacteristics)); // TPM0_CH4 on PC.8 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM0_4_ON_E
                        _CONFIG_PERIPHERAL(E, 31, (PE_31_TPM0_CH4 | ulCharacteristics)); // TPM0_CH4 on PE.31 (alt. function 3)
        #elif defined FTM0_4_ON_D
                        _CONFIG_PERIPHERAL(D, 4, (PD_4_FTM0_CH4 | ulCharacteristics)); // FTM0_CH4 on PD.4 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 7, (PA_7_FTM0_CH4 | ulCharacteristics)); // FTM0_CH4 on PA.7 (alt. function 3)
        #endif
                        break;
                    case 5:                                              // timer 0, channel 5
        #if defined KINETIS_KE15
            #if defined TPM0_5_ON_B_HIGH
                        _CONFIG_PERIPHERAL(B, 17, (PB_17_FTM0_CH5 | ulCharacteristics)); // FTM0_CH5 on PB.17 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(B, 5, (PB_5_FTM0_CH5 | ulCharacteristics)); // FTM0_CH5 on PB.5 (alt. function 2)
            #endif
        #elif defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(A, 5, (PA_5_TPM0_CH5 | ulCharacteristics)); // TPM0_CH5 on PA.5 (alt. function 2)
        #elif defined FTM0_5_ON_C && defined KINETIS_KL28
                        _CONFIG_PERIPHERAL(C, 9, (PC_9_TPM0_CH4 | ulCharacteristics)); // TPM0_CH4 on PC.9 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28) && defined TPM0_5_ON_E
                        _CONFIG_PERIPHERAL(E, 26, (PE_26_TPM0_CH5 | ulCharacteristics)); // TPM0_CH5 on PE.26 (alt. function 3)
        #elif defined FTM0_5_ON_D
                        _CONFIG_PERIPHERAL(D, 5, (PD_5_FTM0_CH5 | ulCharacteristics)); // FTM0_CH5 on PD.5 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_FTM0_CH5 | ulCharacteristics)); // FTM0_CH5 on PA.0 (alt. function 3)
        #endif
                        break;
        #if FLEX_TIMERS_0_CHANNELS > 6
                    case 6:                                              // timer 0, channel 6
            #if defined KINETIS_KE15
                #if defined TPM0_6_ON_A
                        _CONFIG_PERIPHERAL(A, 17, (PA_17_FTM0_CH6 | ulCharacteristics)); // FTM0_CH6 on PA.17 (alt. function 2)
                #else
                        _CONFIG_PERIPHERAL(E, 8, (PE_8_FTM0_CH6 | ulCharacteristics)); // FTM0_CH6 on PE.8 (alt. function 2)
                #endif
            #elif defined FTM0_6_ON_D
                        _CONFIG_PERIPHERAL(D, 6, (PD_6_FTM0_CH6 | ulCharacteristics)); // FTM0_CH6 on PD.6 (alt. function 4)
            #else
                        _CONFIG_PERIPHERAL(A, 1, (PA_1_FTM0_CH6 | ulCharacteristics)); // FTM0_CH6 on PA.1 (alt. function 3)
            #endif
                        break;
                    case 7:                                              // timer 0, channel 7
            #if defined KINETIS_KE15
                #if defined TPM0_7_ON_E9
                        _CONFIG_PERIPHERAL(E, 9, (PE_9_FTM0_CH7 | ulCharacteristics)); // FTM0_CH7 on PE.9 (alt. function 2)
                #else
                        _CONFIG_PERIPHERAL(E, 7, (PE_7_FTM0_CH7 | ulCharacteristics)); // FTM0_CH7 on PE.7 (alt. function 2)
                #endif
            #elif defined FTM0_7_ON_D
                        _CONFIG_PERIPHERAL(D, 7, (PD_7_FTM0_CH7 | ulCharacteristics)); // FTM0_CH7 on PD.7 (alt. function 4)
            #elif defined KINETIS_K80
                        _CONFIG_PERIPHERAL(A, 2, (PA_2_FTM0_CH7 | ulCharacteristics)); // FTM0_CH7 on PA.2 (alt. function 3)
            #else
                        _CONFIG_PERIPHERAL(A, 6, (PA_6_FTM0_CH7 | ulCharacteristics)); // FTM0_CH7 on PA.6 (alt. function 3)
            #endif
                        break;
        #endif
    #endif
                    default:
    #if defined _WINDOWS
                        _EXCEPTION("Invalid timer channel!!");
    #endif
                        return;                                          // invalid channel
                    }
        break;

    #if FLEX_TIMERS_AVAILABLE > 1
    case 1:                                                              // timer 1
                    switch (iChannel) {                                  // configure appropriate pin for the timer signal
                    case 0:                                              // timer 1, channel 0
        #if defined KINETIS_KE15
            #if defined TPM1_0_ON_B
                        _CONFIG_PERIPHERAL(B, 2, (PB_2_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PB.2 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(C, 4, (PC_4_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PC.4 (alt. function 2)
            #endif
        #elif defined KINETIS_KE && !defined KINETIS_KE18
            #if defined FTM1_0_ON_H
                        SIM_PINSEL0 |= SIM_PINSEL_FTM1PS0;
                        _CONFIG_PERIPHERAL(H, 2, (PH_2_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PH.2 (alt. function 4)
            #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM1PS0;
                        _CONFIG_PERIPHERAL(C, 4, (PC_4_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PC.4
            #endif
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM1_0_ALT_2
                        _CONFIG_PERIPHERAL(B, 7, (PB_7_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PB.7 (alt. function 2)
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05) && defined TPM1_0_ALT
                        _CONFIG_PERIPHERAL(A, 12, (PA_12_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PA.12 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PA.0 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM1_0_ON_E
                        _CONFIG_PERIPHERAL(E, 20, (PE_20_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PE.20 (alt. function 3)
        #elif defined FTM1_0_ON_B
                        _CONFIG_PERIPHERAL(B, 0, (PB_0_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PB.0 (alt. function 3)
        #elif (defined FTM1_0_ALT_A && (defined KINETIS_K65 || defined KINETIS_K66)) || defined KINETIS_KL28
                        _CONFIG_PERIPHERAL(A, 12, (PA_12_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PA.12 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 8, (PA_8_FTM1_CH0 | ulCharacteristics)); // FTM1_CH0 on PA.8 (alt. function 3)
        #endif
                        break;
                    case 1:                                              // timer 1, channel 1
        #if defined KINETIS_KE15
            #if defined TPM1_1_ON_A
                        _CONFIG_PERIPHERAL(A, 1, (PA_1_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PA.1 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(B, 3, (PB_3_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PB.3 (alt. function 2)
            #endif
        #elif defined KINETIS_KE && !defined KINETIS_KE18
            #if defined FTM1_1_ON_E
                        SIM_PINSEL0 |= SIM_PINSEL_FTM1PS1;
                        _CONFIG_PERIPHERAL(E, 7, (PE_7_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PE.7 (alt. function 4)
            #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM1PS1;
                        _CONFIG_PERIPHERAL(C, 5, (PC_5_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PC.5
            #endif
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM1_1_ALT_2
                        _CONFIG_PERIPHERAL(B, 6, (PB_6_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PB.6 (alt. function 2)
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05) && defined FTM1_1_ALT
                        _CONFIG_PERIPHERAL(B, 13, (PB_13_TPM1_CH1 | ulCharacteristics)); // TPM1_CH1 on PB.13 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 5, (PB_5_TPM1_CH1 | ulCharacteristics)); // TPM1_CH1 on PB.5 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43) && defined TPM1_1_ON_E
                        _CONFIG_PERIPHERAL(E, 21, (PE_21_TPM1_CH1 | ulCharacteristics)); // TPM1_CH1 on PE.21 (alt. function 3)
        #elif defined FTM1_1_ON_B
                        _CONFIG_PERIPHERAL(B, 1, (PB_1_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PB.1 (alt. function 3)
        #elif (defined FTM1_1_ALT_A && (defined KINETIS_K65 || defined KINETIS_K66)) || defined KINETIS_KL28
                        _CONFIG_PERIPHERAL(A, 13, (PA_13_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PA.13 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 9, (PA_9_FTM1_CH1 | ulCharacteristics)); // FTM1_CH1 on PA.9 (alt. function 3)
        #endif
                        break;
        #if FLEX_TIMERS_1_CHANNELS > 2
                    case 2:                                              // timer 1, channel 2
            #if defined KINETIS_KE15
                #if defined TPM1_2_ON_A
                            _CONFIG_PERIPHERAL(A, 15, (PA_15_FTM1_CH2 | ulCharacteristics)); // FTM1_CH2 on PA.15 (alt. function 2)
                #else
                            _CONFIG_PERIPHERAL(C, 14, (PC_14_FTM1_CH2 | ulCharacteristics)); // FTM1_CH2 on PC.14 (alt. function 2)
                #endif
            #endif
                        break;
        #endif
        #if FLEX_TIMERS_1_CHANNELS > 3
                    case 3:                                              // timer 1, channel 3
            #if defined KINETIS_KE15
                #if defined TPM1_3_ON_A
                            _CONFIG_PERIPHERAL(A, 16, (PA_16_FTM1_CH3 | ulCharacteristics)); // FTM1_CH3 on PA.16 (alt. function 2)
                #else
                            _CONFIG_PERIPHERAL(C, 15, (PC_15_FTM1_CH3 | ulCharacteristics)); // FTM1_CH3 on PC.15 (alt. function 2)
                #endif
            #endif
                        break;
        #endif
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
        break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    case 2:                                                              // timer 2
                    switch (iChannel) {                                  // configure appropriate pin for the timer signal
                    case 0:                                              // timer 2, channel 0
        #if defined KINETIS_KE15
            #if defined TPM2_0_ON_C
                        _CONFIG_PERIPHERAL(C, 5, (PC_5_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PC.5 (alt. function 2)
            #elif defined TPM2_0_ON_D_HIGH
                        _CONFIG_PERIPHERAL(D, 10, (PD_10_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PD.10 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PD.0 (alt. function 4)
            #endif
        #elif defined KINETIS_KE && !defined KINETIS_KE18
            #if defined FTM2_0_ON_H
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS0;
                        _CONFIG_PERIPHERAL(H, 0, (PH_0_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PH.0 (alt. function 2)
            #elif defined FTM2_0_ON_F
                        _CONFIG_PERIPHERAL(F, 0, (PF_0_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PF.0 (alt. function 2)
            #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS0;
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS0;
                        _CONFIG_PERIPHERAL(C, 0, (PC_0_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PC.0 (alt. function 2)
            #endif
        #elif defined FTM2_0_ON_A && defined KINETIS_KL28
                        _CONFIG_PERIPHERAL(A, 1, (PA_1_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PA.1 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28) && defined TPM2_0_ON_E
                        _CONFIG_PERIPHERAL(E, 22, (PE_22_TPM2_CH0 | ulCharacteristics)); // TPM2_CH0 on PE.22 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28) && defined FTM2_0_ON_B_LOW
                        _CONFIG_PERIPHERAL(B, 2,  (PB_2_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PB.2 (alt. function 3)
        #elif defined FTM2_0_ON_B
                        _CONFIG_PERIPHERAL(B, 18, (PB_18_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PB.18 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 10, (PA_10_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PA.10 (alt. function 3)
        #endif
                        break;
                    case 1:                                              // timer 2, channel 1
        #if defined KINETIS_KE15
            #if defined TPM2_1_ON_A
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PA.0 (alt. function 2)
            #elif defined TPM2_1_ON_D_HIGH
                        _CONFIG_PERIPHERAL(D, 11, (PD_11_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PD.11 (alt. function 2)
            #else
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PD.1 (alt. function 4)
            #endif
        #elif defined KINETIS_KE && !defined KINETIS_KE18
            #if defined FTM2_1_ON_H
                #if defined KINETIS_KE06
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS0_PTH0;
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS0_PTF0;
                #else
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS1;
                #endif
                        _CONFIG_PERIPHERAL(H, 1, (PH_1_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PH.1 (alt. function 2)
            #elif defined FTM2_1_ON_F
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS0_PTH0;
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS0_PTF0;
                        _CONFIG_PERIPHERAL(F, 1, (PF_1_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PF.1 (alt. function 2)
            #else
                #if (defined SIM_PINSEL1 && (((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06) || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS0_PTH0 | SIM_PINSEL1_FTM2PS0_PTF0);
                #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS1;
                #endif
                        _CONFIG_PERIPHERAL(C, 1, (PC_1_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PC.1 (alt. function 2)
            #endif
        #elif defined FTM2_1_ON_A && defined KINETIS_KL28
                        _CONFIG_PERIPHERAL(A, 2, (PA_2_FTM2_CH0 | ulCharacteristics)); // FTM2_CH0 on PA.2 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28) && defined TPM2_1_ON_E
                        _CONFIG_PERIPHERAL(E, 23, (PE_23_TPM2_CH1 | ulCharacteristics)); // TPM2_CH1 on PE.23 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28) && defined FTM2_1_ON_B_LOW
                        _CONFIG_PERIPHERAL(B, 3,  (PB_3_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PB.3 (alt. function 3)
        #elif defined FTM2_1_ON_B
                        _CONFIG_PERIPHERAL(B, 19, (PB_19_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PB.19 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 11, (PA_11_FTM2_CH1 | ulCharacteristics)); // FTM2_CH1 on PA.11 (alt. function 3)
        #endif
                        break;
        #if FLEX_TIMERS_2_CHANNELS > 2
                    case 2:                                              // timer 2, channel 2
            #if defined KINETIS_KE15
                #if defined TPM2_2_ON_D
                        _CONFIG_PERIPHERAL(D, 12, (PD_12_FTM2_CH2 | ulCharacteristics)); // FTM2_CH2 on PD.12 (alt. function 2)
                #else
                        _CONFIG_PERIPHERAL(E, 4, (PE_4_FTM2_CH2 | ulCharacteristics)); // FTM2_CH2 on PE.4 (alt. function 4)
                #endif
            #elif defined FTM2_2_ON_D
                #if defined KINETIS_KE06
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS2_PTD0;
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS2_PTG4;
                #else
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS2;
                #endif
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM2_CH2 | ulCharacteristics)); // FTM2_CH2 on PD.0 (alt. function 2)
            #elif defined FTM2_2_ON_G
                        SIM_PINSEL1 |= (SIM_PINSEL1_FTM2PS2_PTD0 | SIM_PINSEL1_FTM2PS2_PTG4);
                        _CONFIG_PERIPHERAL(G, 4, (PG_4_FTM2_CH2 | ulCharacteristics)); // FTM2_CH2 on PG.4 (alt. function 2)
            #else
                #if (defined SIM_PINSEL1 && (((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06) || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS2_PTD0 | SIM_PINSEL1_FTM2PS2_PTG4);
                #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS2;
                #endif
                        _CONFIG_PERIPHERAL(C, 2, (PC_2_FTM2_CH2 | ulCharacteristics)); // FTM2_CH2 on PC.2 (alt. function 1)
            #endif
                        break;
        #endif
        #if FLEX_TIMERS_2_CHANNELS > 3
                    case 3:                                              // timer 2, channel 3
            #if defined KINETIS_KE15
                #if defined TPM2_3_ON_D
                        _CONFIG_PERIPHERAL(D, 5, (PD_5_FTM2_CH3 | ulCharacteristics)); // FTM2_CH3 on PD.5 (alt. function 2)
                #else
                        _CONFIG_PERIPHERAL(E, 5, (PE_5_FTM2_CH3 | ulCharacteristics)); // FTM2_CH3 on PE.5 (alt. function 4)
                #endif
            #elif defined FTM2_3_ON_D
                #if defined KINETIS_KE06
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS3_PTD1;
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS3_PTG5;
                #else
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS3;
                #endif
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM2_CH3 | ulCharacteristics)); // FTM2_CH3 on PD.1 (alt. function 2)
            #elif defined FTM2_3_ON_G
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS3_PTD1);
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS3_PTG5;
                        _CONFIG_PERIPHERAL(G, 5, (PG_5_FTM2_CH3 | ulCharacteristics)); // FTM2_CH3 on PG.5 (alt. function 2)
            #else
                    #if (defined SIM_PINSEL1 && (((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06) || defined KINETIS_KEA64 ||  defined KINETIS_KEA128))
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS3_PTD1 | SIM_PINSEL1_FTM2PS3_PTG5);
                    #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS3;
                    #endif
                        _CONFIG_PERIPHERAL(C, 3, (PC_3_FTM2_CH3 | ulCharacteristics)); // FTM2_CH3 on PC.3 (alt. function 1)
            #endif
                        break;
        #endif
        #if FLEX_TIMERS_2_CHANNELS > 4
                    case 4:                                              // timer 2, channel 4
            #if defined FTM2_4_ON_G && defined SIM_PINSEL1
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS4;
                        _CONFIG_PERIPHERAL(G, 6, (PG_6_FTM2_CH4 | ulCharacteristics)); // FTM2_CH4 on PG.6 (alt. function 2)
            #else
                #if defined KINETIS_KE06
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS4;
                #endif
                        _CONFIG_PERIPHERAL(B, 4, (PB_4_FTM2_CH4 | ulCharacteristics)); // FTM2_CH4 on PB.4 (alt. function 1)
            #endif
                        break;
        #endif
        #if FLEX_TIMERS_2_CHANNELS > 5
                    case 5:                                              // timer 2, channel 5
            #if defined FTM2_5_ON_G && defined SIM_PINSEL1
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS5;
                        _CONFIG_PERIPHERAL(G, 7, (PG_7_FTM2_CH5 | ulCharacteristics)); // FTM2_CH5 on PG.7 (alt. function 2)
            #else
                #if defined KINETIS_KE06
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS5;
                #endif
                        _CONFIG_PERIPHERAL(B, 5, (PB_5_FTM2_CH5 | ulCharacteristics)); // FTM2_CH5 on PB.5 (alt. function 1)
            #endif
                        break;
        #endif
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
        break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    case 3:                                                              // timer 3
                    switch (iChannel) {                                  // configure appropriate pin for the timer signal
                    case 0:                                              // timer 3, channel 0
        #if defined FTM3_0_ON_D
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM3_CH0 | ulCharacteristics)); // FTM3_CH0 on PD.0 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 5, (PE_5_FTM3_CH0 | ulCharacteristics)); // FTM3_CH0 on PE.5 (alt. function 6)
        #endif
                        break;
                    case 1:                                              // timer 3, channel 1
        #if defined FTM3_1_ON_D
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM3_CH1 | ulCharacteristics)); // FTM3_CH1 on PD.1 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 6, (PE_6_FTM3_CH1 | ulCharacteristics)); // FTM3_CH1 on PE.6 (alt. function 6)
        #endif
                        break;
                    case 2:                                              // timer 3, channel 2
        #if defined FTM3_2_ON_D
                        _CONFIG_PERIPHERAL(D, 2, (PD_2_FTM3_CH2 | ulCharacteristics)); // FTM3_CH2 on PD.2 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 7, (PE_7_FTM3_CH2 | ulCharacteristics)); // FTM3_CH2 on PE.7 (alt. function 6)
        #endif
                        break;
                    case 3:                                              // timer 3, channel 3
        #if defined FTM3_3_ON_D
                        _CONFIG_PERIPHERAL(D, 3, (PD_3_FTM3_CH3 | ulCharacteristics)); // FTM3_CH3 on PD.3 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 8, (PE_8_FTM3_CH3 | ulCharacteristics)); // FTM3_CH3 on PE.8 (alt. function 6)
        #endif
                        break;
                    case 4:                                              // timer 3, channel 4
        #if defined FTM3_4_ON_C
                        _CONFIG_PERIPHERAL(C, 8, (PC_8_FTM3_CH4 | ulCharacteristics)); // FTM3_CH4 on PC.8 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 9, (PE_9_FTM3_CH4 | ulCharacteristics)); // FTM3_CH4 on PE.9 (alt. function 6)
        #endif
                        break;
                    case 5:                                              // timer 3, channel 5
        #if defined FTM3_5_ON_C
                        _CONFIG_PERIPHERAL(C, 9, (PC_9_FTM3_CH5 | ulCharacteristics)); // FTM3_CH5 on PC.9 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 10, (PE_10_FTM3_CH5 | ulCharacteristics)); // FTM3_CH5 on PE.10 (alt. function 6)
        #endif
                        break;
                    case 6:                                              // timer 3, channel 6
        #if defined FTM3_6_ON_C
                        _CONFIG_PERIPHERAL(C, 10, (PC_10_FTM3_CH6 | ulCharacteristics)); // FTM3_CH6 on PC.10 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 11, (PE_11_FTM3_CH6 | ulCharacteristics)); // FTM3_CH6 on PE.11 (alt. function 6)
        #endif
                        break;
                    case 7:                                              // timer 3, channel 7
        #if defined FTM3_7_ON_C
                        _CONFIG_PERIPHERAL(C, 11, (PC_11_FTM3_CH7 | ulCharacteristics)); // FTM3_CH7 on PC.11 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 12, (PE_5_FTM3_CH7 | ulCharacteristics)); // FTM3_CH7 on PE.12 (alt. function 6)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
        break;
    #endif
    #if (FLEX_TIMERS_AVAILABLE > 4) && defined TPMS_AVAILABLE_TOO
    case 4:                                                              // timer 4
                    switch (iChannel) {                                  // configure appropriate pin for the timer signal
                    case 0:                                              // TPM 1, channel 0
        #if defined TPM1_0_ON_B
                        _CONFIG_PERIPHERAL(B, 0, (PB_0_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PB.0 (alt. function 6)
        #elif defined TPM1_0_ON_A_HIGH
                        _CONFIG_PERIPHERAL(A, 12, (PA_12_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PA.12 (alt. function 7)
        #else
                        _CONFIG_PERIPHERAL(A, 8, (PA_8_TPM1_CH0 | ulCharacteristics)); // TPM1_CH0 on PA.8 (alt. function 6)
        #endif
                        break;
                    case 1:                                              // TPM 1, channel 1
        #if defined TPM1_1_ON_B
                        _CONFIG_PERIPHERAL(B, 1, (PB_1_TPM1_CH1 | ulCharacteristics)); // TPM1_CH1 on PB.1 (alt. function 6)
        #elif defined TPM1_1_ON_A_HIGH
                        _CONFIG_PERIPHERAL(A, 13, (PA_13_TPM1_CH1 | ulCharacteristics)); // TPM1_CH1 on PA.13 (alt. function 7)
        #else
                        _CONFIG_PERIPHERAL(A, 9, (PA_9_TPM1_CH1 | ulCharacteristics)); // TPM1_CH1 on PA.9 (alt. function 6)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
        break;
    case 5:
                    switch (iChannel) {                                  // configure appropriate pin for the timer signal
                    case 0:                                              // TPM 2, channel 0
        #if defined TPM2_0_ON_B
                        _CONFIG_PERIPHERAL(B, 18, (PB_18_TPM2_CH0 | ulCharacteristics)); // TPM2_CH0 on PB.18 (alt. function 6)
        #else
                        _CONFIG_PERIPHERAL(A, 10, (PA_10_TPM2_CH0 | ulCharacteristics)); // TPM2_CH0 on PA.10 (alt. function 6)
        #endif
                        break;
                    case 1:                                              // TPM 2, channel 1
        #if defined TPM2_1_ON_B
                        _CONFIG_PERIPHERAL(B, 19, (PB_19_TPM2_CH1 | ulCharacteristics)); // TPM2_CH1 on PB.19 (alt. function 6)
        #else
                        _CONFIG_PERIPHERAL(A, 11, (PA_11_TPM2_CH1 | ulCharacteristics)); // TPM2_CH1 on PA.11 (alt. function 6)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
        break;
    #endif
    }
}

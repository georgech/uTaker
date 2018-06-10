/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_KM_CLOCK.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


#if defined OSC_LOW_GAIN_MODE                                            // if using low frequency low power mode no external resistor or load capacitors are used
    #define MCG_C2_GAIN_MODE    0                                        // don't select high gain mode since the oscillator will not start
#else
    #define MCG_C2_GAIN_MODE    MCG_C2_HGO                               // select high gain mode
#endif

#if CRYSTAL_FREQUENCY > 8000000                                          // crystal > 8MHz
    #define MCG_C2_FREQ_RANGE     MCG_C2_RANGE_8M_32M
#elif CRYSTAL_FREQUENCY >= 1000000                                       // crystal bwteeen 1MHz and 8MHz
    #define MCG_C2_FREQ_RANGE     MCG_C2_RANGE_1M_8M
#else                                                                    // assumed to be 32kHz crystal
    #define MCG_C2_FREQ_RANGE     MCG_C2_RANGE_32K_40K
#endif


#if !defined RUN_FROM_DEFAULT_CLOCK && !defined EXTERNAL_CLOCK           // no configuration performed - remain in default clocked mode
    #if CRYSTAL_FREQUENCY == 8000000
        #define MCG_C1_FRDIV_VALUE    MCG_C1_FRDIV_256
    #elif CRYSTAL_FREQUENCY == 16000000
        #define MCG_C1_FRDIV_VALUE    MCG_C1_FRDIV_512
    #elif CRYSTAL_FREQUENCY == 24000000
        #define MCG_C1_FRDIV_VALUE    MCG_C1_FRDIV_1024
    #elif CRYSTAL_FREQUENCY >= 10000000 && CRYSTAL_FREQUENCY <= 12000000
        #define MCG_C1_FRDIV_VALUE    MCG_C1_FRDIV_256
    #elif CRYSTAL_FREQUENCY == 4000000
        #define MCG_C1_FRDIV_VALUE    MCG_C1_FRDIV_128
    #elif CRYSTAL_FREQUENCY == 32768
        #if !defined FLL_FACTOR
           #define MCG_C1_FRDIV_VALUE MCG_C1_FRDIV_RANGE0_1
        #endif
    #else
        #error crystal speed support needs to be added!
    #endif
#endif




#if BUS_CLOCK_DIVIDE == 2
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | (SIM_CLKDIV1_SYSCLKMODE)); // prepare bus clock divides
#elif BUS_CLOCK_DIVIDE == 1
    SIM_CLKDIV1 = ((SYSTEM_CLOCK_DIVIDE - 1) << 28);                     // prepare bus clock divides
#else
    _EXCEPTION("Bus/Flash clock can be either equal to sysclock or divided by 2")
#endif

#if defined RUN_FROM_DEFAULT_CLOCK                                       // no configuration performed - remain in default clocked mode (4MHz IRC/2)
#elif defined RUN_FROM_EXTERNAL_CLOCK || defined RUN_FROM_EXTERNAL_CLOCK_FLL
    OSC0_CR = (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                        // enable the oscillator and allow it to continue oscillating in stop mode
    MCG_C7 = (MCG_C7_PLL32KREFSEL_IRC);                                  // select the OSC clock as external clock input to the FLL and 32kHz IRC as input to the 32k PLL
    MCG_C2 = (MCG_C2_FREQ_RANGE | MCG_C2_GAIN_MODE | MCG_C2_EREFS | MCG_C2_LOCRE0 | MCG_C2_IRCS); // select crystal oscillator and select a suitable range
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_VALUE);              // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
    #if defined RUN_FROM_EXTERNAL_CLOCK_FLL && defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
    #endif
    while ((MCG_S & MCG_S_OSCINIT) == 0) {                               // loop until the crystal source has been selected
    #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT;                                          // set the flag indicating that the ocsillator initialisation has completed
    #endif
    }
    while ((MCG_S & MCG_S_IREFST) != 0) {                                // loop until the FLL source is no longer the internal reference clock
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_IREFST;
    #endif
    }
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // loop until the external reference clock source is valid
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
    #endif
    }
    MCG_C6 = (MCG_C6_CME | MCG_C6_PLLS_FLL);                             // select FLL (rather than PLL) and enable oscillator monitor
    // At this point we are runing directly from the 8MHz external clock source
    //
    #if defined RUN_FROM_EXTERNAL_CLOCK_FLL
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_VALUE);                 // switch to FLL
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_FLL) {              // loop until the FLL clock source is valid
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        #endif
    }
    #endif
#else
    #if defined EXTERNAL_CLOCK || defined CLOCK_FROM_RTC_OSCILLATOR      // first move from state FEI to state FBE
        #if defined CLOCK_FROM_RTC_OSCILLATOR
    POWER_UP_ATOMIC(6, RTC);                                             // enable access to the RTC
    MCG_C7 = MCG_C7_OSCSEL_32K;                                          // select the RTC clock as external clock input to the FLL
    RTC_CR = (RTC_CR_OSCE);                                              // enable RTC oscillator and output the 32.768kHz output clock so that it can be used by the MCG (the first time that it starts it can have a startup/stabilisation time but this is not critical for the FLL usage)
    MCG_C1 = ((MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_RANGE0_1) & ~MCG_C1_IREFS); // switch the FLL input to the undivided external clock source (RTC)
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    while ((MCG_S & MCG_S_IREFST) != 0) {                                // wait until the switch to the external clock source has completed
            #if defined _WINDOWS
        MCG_S &= ~(MCG_S_IREFST);
            #endif
    }
            #if defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
            #endif
        #elif defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #else                                                            // external oscillator
            #if EXTERNAL_CLOCK >= 8000000
    MCG_C2 = (MCG_C2_RANGE_8M_32M | MCG_C2_LOCRE0);                      // select external clock source (with reset on clock loss)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1024);               // switch to external input clock (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary since the FLL will not be used)
            #else
    MCG_C2 = (MCG_C2_RANGE_1M_8M | MCG_C2_LOCRE0);                       // select external clock source (with reset on clock loss)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_128);                // switch to external input clock (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary since the FLL will not be used)
            #endif
        #endif
    #else                                                                // crystal clock
        #if defined FLL_FACTOR || defined RUN_FROM_EXTERNAL_CLOCK
    OSC0_CR = (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                        // enable the oscillator and allow it to continue oscillating in stop mode
        #endif
    MCG_C7 = (MCG_C7_PLL32KREFSEL_IRC);                                  // select the OSC clock as external clock input to the FLL and 32kHz IRC as input to the 32k PLL
    MCG_C2 = (MCG_C2_FREQ_RANGE | MCG_C2_GAIN_MODE | MCG_C2_EREFS | MCG_C2_LOCRE0 | MCG_C2_IRCS); // select crystal oscillator and select a suitable range
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_VALUE);              // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
    while ((MCG_S & MCG_S_OSCINIT) == 0) {                               // loop until the crystal source has been selected
        #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT;                                          // set the flag indicating that the ocsillator initialisation has completed
        #endif
    }
        #if defined FLL_FACTOR
                #if defined ADC_CLOCK_ENABLED 
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)  | ((ADC_CLOCK_DIVIDE - 1) << 12) | SIM_OUTDIV5EN); // prepare bus clock divides
                #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)  | ((SIM_CLKDIV5_ADC_8 - 1) << 12)); // prepare bus clock divides
                #endif
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
    MCG_C1 &= ~(MCG_C1_CLKS_INTERN_CLK | MCG_C1_CLKS_EXTERN_CLK | MCG_C1_IREFS); // move to FEE (MCGOUTCLK is derived from the FLL clock), selecting the external clock as source (rather than slow internal clock)
    while ((MCG_S & (MCG_S_CLKST_EXTERN_CLK | MCG_S_CLKST_INTERN_CLK)) != MCG_S_CLKST_FLL) { // loop until the FLL clock source becomes valid
            #if defined _WINDOWS
        MCG_S &= ~(MCG_S_CLKST_EXTERN_CLK | MCG_S_CLKST_INTERN_CLK);
            #endif
    }
        #endif
    #endif                                                               // endif not EXTERNAL_CLOCK
    #if !defined CLOCK_FROM_RTC_OSCILLATOR
    while ((MCG_S & MCG_S_IREFST) != 0) {                                // loop until the FLL source is no longer the internal reference clock
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_IREFST;
        #endif
    }
        #if !defined FLL_FACTOR
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // loop until the external reference clock source is valid
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
            #endif
    }
            #if defined RUN_FROM_EXTERNAL_CLOCK
                #if BUS_CLOCK_DIVIDE == 2
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | (SIM_CLKDIV1_SYSCLKMODE)); // prepare bus clock divides
                #elif BUS_CLOCK_DIVIDE == 1
    SIM_CLKDIV1 = ((SYSTEM_CLOCK_DIVIDE - 1) << 28);                     // prepare bus clock divides
                #else
    _EXCEPTION("Bus/Flash clock can be either equal to sysclock or divided by 2")
                #endif
            #endif
        #endif
    #endif
#endif
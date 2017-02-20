/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_KL_CLOCK.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

#if defined OSC_LOW_GAIN_MODE                                            // {66} if using low frequency low power mode no external resistor or load capacitors are used
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


#if defined RUN_FROM_DEFAULT_CLOCK                                       // no configuration performed - remain in default clocked mode
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare system and flash clock divides (valid also as bus clock divider)
    #if defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
    #endif
#elif defined RUN_FROM_LIRC
    MCG_SC = MCG_SC_FCRDIV_1;                                            // no divide after fast clock (4MHz)
    MCG_C2 |= MCG_C2_IRCS;                                               // select fast internal reference clock (rather than slow one) for MCGIRCLK
    MCG_C1 = (MCG_C1_IREFSTEN | MCG_C1_IRCLKEN | MCG_C1_CLKS_INTERN_CLK);// enable and select 4MHz IRC clock source and allow it to continue operating in STOP mode
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_INTERN_CLK) {       // wait until the 4MHz IRC source is selected
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_INTERN_CLK;
    #endif
    }
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // set system and bus clock dividers
    MCG_C2 |= MCG_C2_LP;                                                 // disable FLL in bypass mode
#else
    #if defined EXTERNAL_CLOCK                                           // first move from state FEI to state FBE
        #if defined RUN_FROM_HIRC_PLL
    MCG_C7 = MCG_C7_OSCSEL_IRC48MCLK;                                    // route the IRC48M clock to the external reference clock input (this enables IRC48M)
        #endif
        #if defined RUN_FROM_RTC_FLL
    POWER_UP(6, SIM_SCGC6_RTC);                                          // enable access to the RTC
    MCG_C7 = MCG_C7_OSCSEL_32K;                                          // select the RTC clock as external clock input to the FLL
    RTC_CR = (RTC_CR_OSCE);                                              // enable RTC oscillator and output the 32.768kHz output clock so that it can be used by the MCG (the first time that it starts it can have a startup/stabilisation time but this is not critical for the FLL usage)
    MCG_C1 = ((MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_RANGE0_1) & ~MCG_C1_IREFS); // switch the FLL input to the undivided external clock source (RTC)
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    while ((MCG_S & MCG_S_IREFST) != 0) {                                // wait until the switch to the external clock source has completed
            #if defined _WINDOWS
        MCG_S &= ~(MCG_S_IREFST);
            #endif
    }
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #elif defined RUN_FROM_RTC_FLL
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
    #else                                                                // crystal source
        #if defined FLL_FACTOR || defined RUN_FROM_EXTERNAL_CLOCK
    OSC0_CR = (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                        // enable the oscillator and allow it to continue oscillating in stop mode
        #endif
    MCG_C2 = (MCG_C2_FREQ_RANGE | MCG_C2_GAIN_MODE | MCG_C2_EREFS | MCG_C2_LOCRE0); // select crystal oscillator and select a suitable range
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_VALUE);              // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary if the FLL will not be used)
    while ((MCG_S & MCG_S_OSCINIT) == 0) {                               // loop until the crystal source has been selected
        #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT;                                          // set the flag indicating that the ocsillator initialisation has completed
        #endif
    }
        #if defined FLL_FACTOR
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
    MCG_C1 &= ~(MCG_C1_CLKS_INTERN_CLK | MCG_C1_CLKS_EXTERN_CLK | MCG_C1_IREFS); // move to FEE (MCGOUTCLK is derived from the FLL clock), selecting the external clock as source (rather than slow internal clock)
    while ((MCG_S & (MCG_S_CLKST_EXTERN_CLK | MCG_S_CLKST_INTERN_CLK)) != MCG_S_CLKST_FLL) { // loop until the FLL clock source becomes valid
        #if defined _WINDOWS
        MCG_S &= ~(MCG_S_CLKST_EXTERN_CLK | MCG_S_CLKST_INTERN_CLK);
        #endif
    }
        #endif
    #endif
    #if !defined RUN_FROM_RTC_FLL
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
            #if defined RUN_FROM_EXTERNAL_CLOCK                          // {101}
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
            #endif
        #endif
        #if !defined FLL_FACTOR && !defined RUN_FROM_EXTERNAL_CLOCK      // {95}
    MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                        // now move from state FEE to state PBE (or FBE) PLL remains enabled in normal stop modes
    MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);          // set the PLL multiplication factor
    while ((MCG_S & MCG_S_PLLST) == 0) {                                 // loop until the PLLS clock source becomes valid
            #if defined _WINDOWS
        MCG_S |= MCG_S_PLLST;
            #endif
    }
    while ((MCG_S & MCG_S_LOCK) == 0) {                                  // loop until PLL locks
            #if defined _WINDOWS
        MCG_S |= MCG_S_LOCK;
            #endif
    }
            #if !defined BUS_FLASH_CLOCK_SHARED                           // the KL82 has seperate flash and bus clocks, as well as a QSPI clock
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLASH_CLOCK_DIVIDE - 1) << 16) | ((QSPI_CLOCK_DIVIDE - 1) << 12)); // prepare bus clock divides
            #else
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides (flash and bus clocks are the same)
            #endif
            #if defined HIGH_SPEED_RUN_MODE_AVAILABLE && defined HIGH_SPEED_RUN_MODE_REQUIRED
    SMC_PMCTRL = SMC_PMCTRL_RUNM_HSRUN;                                  // {118} set high speed run mode (restrictions apply) so that the clock speeds can be obtained  
            #endif
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
            #endif
    }
        #endif
    #endif
#endif

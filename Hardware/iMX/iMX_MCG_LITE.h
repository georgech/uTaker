/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_MCG_LITE.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/




#if defined RUN_FROM_DEFAULT_CLOCK                                       // no configuration performed - remain in default clocked mode
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare system and flash clock divides (valid also as bus clock divider)
    #if defined FLL_FACTOR
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
    #endif
#elif defined RUN_FROM_LIRC                                              // 8MHz default
    // The IRC8 defaults to 8MHz with no FCRDIV divide
    //
    #if defined RUN_FROM_LIRC_2M                                         // select 2MHz ICR rather than 8MHz
    MCG_C2 = 0;
    #else
    MCG_C2 = MCG_C2_IRCS;                                                // ensure 8MHz ICR is selected
    #endif
    #if defined SLOW_CLOCK_DIVIDE                                        // if a slow clock output divider is specified
    MCG_SC = SLOW_CLOCK_DIVIDE_VALUE;                                    // select the output divider ratio
    #endif
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // set bus clock divides
#else                                                                    // EXT mode (run directly from the oscillator input)
    MCG_C2 = (MCG_C2_EREFS | MCG_C2_IRCS);                               // select oscillator as external clock source
    OSC0_CR = (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                        // enable the oscillator and allow it to continue oscillating in stop mode
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    while ((MCG_S & MCG_S_OSCINIT0) == 0) {                              // wait for the oscillator to start
    #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT0;
    #endif
    }
    MCG_C1 = MCG_C1_CLKS_EXTERN_CLK;                                     // select external clock source
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXT) {              // wait until the source is selected
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXT;
    #endif
    }
#endif

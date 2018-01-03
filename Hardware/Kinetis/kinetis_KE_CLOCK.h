/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_KE_CLOCK.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


    SIM_SOPT0 = SIM_SOPT_KE_DEFAULT;                                     // set required default - some fields are "write-only" and so can only be set once
    #if !defined RUN_FROM_DEFAULT_CLOCK
    OSC0_CR = (OSC_CR_OSCEN | OSC_CR_OSCSTEN | OSC_CR_OSCOS_SOURCE | _OSC_RANGE); // low gain mode, select crystal range and enable oscillator
    while ((OSC0_CR & OSC_CR_OSCINIT) == 0) {                            // wait until the oscillator has been initialised
        #if defined _WINDOWS
        OSC0_CR |= OSC_CR_OSCINIT;
        #endif
    }
        #if defined RUN_FROM_EXTERNAL_CLOCK
    ICS_C1 = (ICS_C1_CLKS_EXTERNAL_REF | _FLL_VALUE);                    // divide value to obtain 31.25kHz..39.06525kHz range from input frequency and select external clock as clock source
    while ((ICS_S & ICS_S_IREFST) != 0) {                                // wait for the clock source to become external clock
            #if defined _WINDOWS
        ICS_S &= ~(ICS_S_IREFST);
            #endif
    }
        #else
    ICS_C1 = (ICS_C1_CLKS_FLL | _FLL_VALUE);                             // divide value to obtain 31.25kHz..39.06525kHz range from input frequency and select FLL as clock source
    while ((ICS_S & ICS_S_IREFST) != 0) {                                // wait for the clock source to become external clock
            #if defined _WINDOWS
        ICS_S &= ~(ICS_S_IREFST);
            #endif
    }
    while ((ICS_S & ICS_S_LOCK) == 0) {                                  // wait for the FLL to lock
            #if defined _WINDOWS
        ICS_S |= ICS_S_LOCK;
            #endif
    }
        #endif
    #endif
    #if (BUS_CLOCK_DIVIDE == 2)                                          // divide the core/sytem clock by 2 to derive the bus/flash clock
        #if defined SIM_CLKDIV
    SIM_CLKDIV = (SIM_CLKDIV_OUTDIV2_2);                                 // bus clock half the system clock (ICSOUTCLK)
        #else
    SIM_BUSDIV = SIM_BUSDIVBUSDIV;                                       // bus clock half the system clock (ICSOUTCLK)
        #endif
    #else
        #if defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA8 || defined KINETIS_KEA64 || defined KINETIS_KEA128
    SIM_CLKDIV = 0;                                                      // bus clock is equal to the system clock (ICSOUTCLK)
        #else
    SIM_BUSDIV = 0;                                                      // bus clock is equal to the system clock (ICSOUTCLK)
        #endif
    #endif
    #if (defined KINETIS_KE04 || defined KINETIS_KE06 || defined KINETIS_KEA8 || defined KINETIS_KEA64 || defined KINETIS_KEA128) && defined TIMER_CLOCK_DIVIDE_2
	SIM_CLKDIV |= SIM_CLKDIV_OUTDIV3_2;                                  // divide the timer clock by 2
	#endif
    ICS_C2 = _SYSCLK__DIV;                                               // set system clock frequency (ICSOUTCLK) once the bus/flash divider has been configured
    #if !defined _WINDOWS
    ICS_S |= ICS_S_LOLS;                                                 // clear loss of lock status
    #endif
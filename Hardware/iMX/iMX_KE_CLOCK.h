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
    07.09.2016 correct KE operation directly from crystal source when bypassing FEE [RUN_FROM_EXTERNAL_CLOCK] {1}
    12.07.2018 Move the oscillator range and FLL value caluclations form kinetis.h to here
    12.07.2018 Add system clock divide setting for KE06 {2}

*/

    #if !defined RUN_FROM_DEFAULT_CLOCK
        #if ((CRYSTAL_FREQUENCY >= MIN_HIGH_RANGE_XTAL) && (CRYSTAL_FREQUENCY <= MAX_HIGH_RANGE_XTAL)) // select crystal range setting depending on the crystal used
            #define _OSC_RANGE   (OSC_CR_RANGE_HIGH)
        #elif ((CRYSTAL_FREQUENCY >= 31250) && (CRYSTAL_FREQUENCY <= 39063)) // {1}
            #define _OSC_RANGE   (OSC_CR_RANGE_LOW)
        #else
            #error Invalid crystal frequency!! (either 32kHz range or MIN_HIGH_RANGE_XTAL..MAX_HIGH_RANGE_XTAL)
        #endif
        #if defined RUN_FROM_EXTERNAL_CLOCK                              // {1}
             #define _FLL_VALUE (ICS_C1_RDIV_RANGE1_1024)                // value is not important when driven directly by oscillator input
        #else
            #if (CLOCK_DIV == 1)                                         // only possible with low frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #error Invalid clock divide from high frequency crystal
                #else
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE0_1)
                #endif
            #elif (CLOCK_DIV == 2)                                       // only possible with low frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #error Invalid clock divide from high frequency crystal
                #else
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE0_2)
                #endif
            #elif (CLOCK_DIV == 4)                                       // only possible with low frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #error Invalid clock divide from high frequency crystal
                #else
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE0_4)
                #endif
            #elif (CLOCK_DIV == 8)                                       // only possible with low frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #error Invalid clock divide from high frequency crystal
                #else
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE0_8)
                #endif
            #elif (CLOCK_DIV == 16)                                      // only possible with low frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #error Invalid clock divide from high frequency crystal
                #else
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE0_16)
                #endif
            #elif (CLOCK_DIV == 32)
                #define _FLL_VALUE (MCG_C4_MID_HIGH_RANGE | MCG_C4_DMX32)
            #elif (CLOCK_DIV == 64)
                #define _FLL_VALUE (MCG_C4_HIGH_RANGE)
            #elif (CLOCK_DIV == 128)
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE1_128)
                #else
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE0_128)
                #endif
            #elif (CLOCK_DIV == 256)                                     // only possible with high frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE1_256)
                #else
                    #error Invalid clock divide from low frequency crystal            
                #endif
            #elif (CLOCK_DIV == 512)                                     // only possible with high frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE1_512)
                #else
                    #error Invalid clock divide from low frequency crystal            
                #endif
            #elif (CLOCK_DIV == 1024)                                    // only possible with high frequency crystal
                #if  (_OSC_RANGE == OSC_CR_RANGE_HIGH)
                    #define _FLL_VALUE (ICS_C1_RDIV_RANGE1_1024)
                #else
                    #error Invalid clock divide from low frequency crystal            
                #endif
            #else
                #error Invalid input clock divide has been specified - valid are 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024
            #endif
            #if (((_EXTERNAL_CLOCK / CLOCK_DIV) > 39066) || ((_EXTERNAL_CLOCK / CLOCK_DIV) < 31250))
                #error Invalid FLL input frequency - 31.25kHz..39.06525kHz required
            #endif
        #endif
    #endif

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
    #if (BUS_CLOCK_DIVIDE == 2)                                          // divide the core/system clock by 2 to derive the bus/flash clock
        #if defined SIM_CLKDIV
            #if defined KINETIS_KE06                                     // {2}
    SIM_CLKDIV = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | SIM_CLKDIV_OUTDIV2_2);
            #else
    SIM_CLKDIV = (SIM_CLKDIV_OUTDIV2_2);                                 // bus clock half the system clock (ICSOUTCLK)
            #endif
        #else
    SIM_BUSDIV = SIM_BUSDIVBUSDIV;                                       // bus clock half the system clock (ICSOUTCLK)
        #endif
    #else
        #if defined KINETIS_KE06                                         // {2}
    SIM_CLKDIV = ((SYSTEM_CLOCK_DIVIDE - 1) << 28);
        #elif defined KINETIS_KE04 || defined KINETIS_KEA8 || defined KINETIS_KEA64 || defined KINETIS_KEA128
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
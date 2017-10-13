/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_HIRC.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    26.07.2017 Add SCG support for fast IRC clocking                     {1}

*/

// 48MHz HIRC
//
#if defined KINETIS_WITH_SCG                                             // {1}
    // After a reset in RUN mode the processor will be in SIRC mode (8MHz internal RC)
    // - we now move to FIRC
    //
    SCG_RCCR = (SCG_RCCR_SCS_FIRC_CLK | ((SYSTEM_CLOCK_DIVIDE - 1) << SCG_RCCR_DIVCORE_SHIFT) | ((BUS_CLOCK_DIVIDE - 1) << SCG_RCCR_DIVSLOW_SHIFT)); // prepare the run mode clock source
            #if defined RUN_FROM_HIRC_60MHz
    SCG_FIRCCFG = SCG_FIRCCFG_RANGE_60MHz;                               // 60MHz
            #elif defined RUN_FROM_HIRC_56MHz
    SCG_FIRCCFG = SCG_FIRCCFG_RANGE_56MHz;                               // 56MHz
            #elif defined RUN_FROM_HIRC_52MHz
    SCG_FIRCCFG = SCG_FIRCCFG_RANGE_52MHz;                               // 52MHz
            #else
    SCG_FIRCCFG = SCG_FIRCCFG_RANGE_48MHz;                               // 48MHz
            #endif
            #if defined USB_INTERFACE
    SCG_FIRCDIV = (SCG_FIRCDIV_FIRCDIV3_1 | SCG_FIRCDIV_FIRCDIV1_1);     // enable FIRC output for use by various peripherals, including USB (configured before enabling the FIRC to avoid clock glitches))
            #else
    SCG_FIRCDIV = SCG_FIRCDIV_FIRCDIV3_1;                                // enable FIRC output for use by various peripherals (configured before enabling the FIRC to avoid clock glitches)
            #endif
    SCG_FIRCCSR = SCG_FIRCCSR_FIRCEN;                                    // enable the fast IRC
    while ((SCG_FIRCCSR & (SCG_FIRCCSR_FIRCSEL | SCG_FIRCCSR_FIRCVLD)) != (SCG_FIRCCSR_FIRCSEL | SCG_FIRCCSR_FIRCVLD)) { // wait until the source has been selected and is valid
            #if defined _WINDOWS
        SCG_FIRCCSR |= (SCG_FIRCCSR_FIRCSEL | SCG_FIRCCSR_FIRCVLD);
            #endif
    }
#else
    #if !defined KINETIS_K64 && defined SUPPORT_RTC && !defined RTC_USES_RTC_CLKIN && !defined RTC_USES_LPO_1kHz
    MCG_C2 = MCG_C2_EREFS;                                               // request oscillator
    OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                       // enable the external reference clock and keep it enabled in stop mode
  //MCG_MC = MCG_MC_HIRCEN;                                              // this is optional and would allow the HIRC to run even when the processor is not working in HIRC mode
    #endif
    #if defined MCG_C1_CLKS_HIRC                                         // processor with MCG-Lite module
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C1 = MCG_C1_CLKS_HIRC;                                           // select HIRC clock source
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_HICR) {             // wait until the source is selected
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_HICR;
        #endif
    }
    #else
    MCG_C7 = MCG_C7_OSCSEL_IRC48MCLK;                                    // route the IRC48M clock to the external reference clock input (this enables IRC48M)
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C1 = (MCG_C1_IREFS | MCG_C1_CLKS_EXTERN_CLK);                    // switch IRC48M reference to MCGOUTCLK
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // wait until the new source is valid (move to FBI using IRC48M external source is complete)
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
        #endif
        #if (defined KINETIS_K64 || (defined KINETIS_K24 && (SIZE_OF_FLASH == (1024 * 1024)))) // older K64 devices require the IRC48M to be switched on by the USB module
        if (++iIRC48M_USB_control >= IRC48M_TIMEOUT) {                   // if the switch-over is taking too long it means that the clock needs to be enabled in the USB controller
            POWER_UP_ATOMIC(4, USBOTG);                                  // power up the USB controller module
            USB_CLK_RECOVER_IRC_EN = (USB_CLK_RECOVER_IRC_EN_REG_EN | USB_CLK_RECOVER_IRC_EN_IRC_EN); // the IRC48M is only usable when enabled via the USB module
        }
        #endif
    }
        #if defined RUN_FROM_HIRC_FLL
    MCG_C2 = (MCG_C2_IRCS | MCG_C2_RANGE_8M_32M);                        // select a high frquency range values so that the FLL input divide range is increased
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1280);                  // switch FLL input to the external clock source with correct divide value, and select the FLL output for MCGOUTCLK
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_FLL) {              // wait for the output to be set
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_FLL;
        #endif
    }
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #elif defined RUN_FROM_HIRC_PLL                                  // we are presently running directly from the IRC48MCLK and have also determined whether a K64 is an older or newer device (with IRC48M independent from the USB module)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1280);               // switch the external clock source also to the FLL to satisfy the PBE state requirement
    MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                        // PLL remains enabled in normal stop modes
    MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);          // complete PLL configuration and move to PBE
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
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
            #endif
    }
        #else
    MCG_C2 |= MCG_C2_LP;                                                 // set bypass to disable FLL and complete move to BLPE (in which PLL is also always disabled)
        #endif
    #endif
#endif

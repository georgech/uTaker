/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_FLEXTIMER.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    21.06.2014 Adjust FlexTimer/TPM use of FTM_SC_TOF to clear interrupt correctly {1}
    22.07.2014 Add clock source selection to TPM                         {2}
    04.01.2017 Don't adjust the RC clock setting when the processor is running from it {3}
    26.01.2017 Add external clock selection for KL parts                 {4}
    26.04.2017 Add KL82 TPM clock input selection                        {5}
    20.05.2017 Add capture mode to Kinetis                               {6}
    07.02.2018 Ensure that the count value is reset to zero when starting timer {7}
    09.08.2018 Add control to freeze and release an operating timer      {8}
    09.08.2018 Add TPM1 and TPM2 extension (for K65/K66/K80)             {9}

*/

#if defined _FLEXTIMER_CODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _flexTimerInterrupt_0(void);
    #if FLEX_TIMERS_AVAILABLE > 1 && !defined NO_FLEX_TIMER_2
static __interrupt void _flexTimerInterrupt_1(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _flexTimerInterrupt_2(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _flexTimerInterrupt_3(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
static __interrupt void _flexTimerInterrupt_4(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
static __interrupt void _flexTimerInterrupt_5(void);
    #endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned short usFlexTimerMode[FLEX_TIMERS_AVAILABLE] = {0};      // operating mode details of each FlexTimer
static void (*_flexTimerHandler[FLEX_TIMERS_AVAILABLE])(void) = {0};     // user interrupt handlers
static unsigned char ucClockSource[FLEX_TIMERS_AVAILABLE] = {0};         // {8} clock source backup for each FlexTimer

static void (*_flexTimerInterrupt[FLEX_TIMERS_AVAILABLE])(void) = {
    _flexTimerInterrupt_0,
    #if FLEX_TIMERS_AVAILABLE > 1
        #if defined NO_FLEX_TIMER_2
    0,                                                                   // dummy when the timer is not available
        #else
    _flexTimerInterrupt_1,
        #endif
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    _flexTimerInterrupt_2,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    _flexTimerInterrupt_3,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
    _flexTimerInterrupt_4,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
    _flexTimerInterrupt_5,
    #endif
};


/* =================================================================== */
/*                  FlexTimer Interrupt Handlers                       */
/* =================================================================== */

// Generic interrupt handling
//
static void fnHandleFlexTimer(FLEX_TIMER_MODULE *ptrFlexTimer, int iFlexTimerReference)
{
#if defined SUPPORT_CAPTURE
    if ((usFlexTimerMode[iFlexTimerReference] & FTM_SC_TOIE) == 0) {     // capture mode rather than timer overflow mode
        if ((ptrFlexTimer->FTM_channel[0].FTM_CSC & (FTM_CSC_CHIE | FTM_CSC_CHF)) == (FTM_CSC_CHIE | FTM_CSC_CHF)) { // if the channel interrupt is enabled and its event flag set
            ptrFlexTimer->FTM_channel[0].FTM_CSC = ptrFlexTimer->FTM_channel[0].FTM_CSC; // clear the interrupt - this allows the next capture event to be released
        }
        else if ((ptrFlexTimer->FTM_channel[1].FTM_CSC & (FTM_CSC_CHIE | FTM_CSC_CHF)) == (FTM_CSC_CHIE | FTM_CSC_CHF)) { // if the channel interrupt is enabled and its event flag set
            ptrFlexTimer->FTM_channel[1].FTM_CSC = ptrFlexTimer->FTM_channel[1].FTM_CSC; // clear the interrupt - this allows the next capture event to be released
        }
        if (iFlexTimerReference == 0) {
            if ((ptrFlexTimer->FTM_channel[2].FTM_CSC & (FTM_CSC_CHIE | FTM_CSC_CHF)) == (FTM_CSC_CHIE | FTM_CSC_CHF)) { // if the channel interrupt is enabled and its event flag set
                ptrFlexTimer->FTM_channel[2].FTM_CSC = ptrFlexTimer->FTM_channel[2].FTM_CSC; // clear the interrupt - this allows the next capture event to be released
            }
            else if ((ptrFlexTimer->FTM_channel[3].FTM_CSC & (FTM_CSC_CHIE | FTM_CSC_CHF)) == (FTM_CSC_CHIE | FTM_CSC_CHF)) { // if the channel interrupt is enabled and its event flag set
                ptrFlexTimer->FTM_channel[3].FTM_CSC = ptrFlexTimer->FTM_channel[3].FTM_CSC; // clear the interrupt - this allows the next capture event to be released
            }
            else if ((ptrFlexTimer->FTM_channel[3].FTM_CSC & (FTM_CSC_CHIE | FTM_CSC_CHF)) == (FTM_CSC_CHIE | FTM_CSC_CHF)) { // if the channel interrupt is enabled and its event flag set
                ptrFlexTimer->FTM_channel[4].FTM_CSC = ptrFlexTimer->FTM_channel[4].FTM_CSC; // clear the interrupt - this allows the next capture event to be released
            }
            else if ((ptrFlexTimer->FTM_channel[3].FTM_CSC & (FTM_CSC_CHIE | FTM_CSC_CHF)) == (FTM_CSC_CHIE | FTM_CSC_CHF)) { // if the channel interrupt is enabled and its event flag set
                ptrFlexTimer->FTM_channel[5].FTM_CSC = ptrFlexTimer->FTM_channel[5].FTM_CSC; // clear the interrupt - this allows the next capture event to be released
            }
        }
    }
    else
#endif
    if ((ptrFlexTimer->FTM_SC & FTM_SC_TOF) != 0) {                     // flag will always be set but it has to be read at '1' before it can be reset
        if ((usFlexTimerMode[iFlexTimerReference] & FLEX_TIMER_PERIODIC) != 0) { // if the timer is being used in periodic mode
            ptrFlexTimer->FTM_SC = (usFlexTimerMode[iFlexTimerReference] & FTM_SC_USED_MASK); // reset interrupt and allow the FlexTimer to continue running for periodic interrupts
        }
        else {
            ptrFlexTimer->FTM_SC = FTM_SC_TOF;                           // stop further activity (single-shot mode)
            switch (iFlexTimerReference) {                               // power down the FlexTimer after single-shot use
            case 0:
                POWER_DOWN_ATOMIC(6, FTM0);
                break;
    #if FLEX_TIMERS_AVAILABLE > 1 && !defined NO_FLEX_TIMER_2
            case 1:
                POWER_DOWN_ATOMIC(6, FTM1);
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
        #if defined KINETIS_KL ||defined KINETIS_K22_SF7
                POWER_DOWN_ATOMIC(6, FTM2);
        #else
                POWER_DOWN_ATOMIC(3, FTM2);
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
            case 3:
        #if defined KINETIS_K22_SF7
                POWER_DOWN_ATOMIC(6, FTM3);
        #else
                POWER_DOWN_ATOMIC(3, FTM3);
        #endif
                break;
    #endif
            }
        }
    }
    #if defined _WINDOWS
    ptrFlexTimer->FTM_SC &= ~FTM_SC_TOF;
    #endif
    if (_flexTimerHandler[iFlexTimerReference] != 0) {                   // if there is a user handler
        uDisable_Interrupt();
            _flexTimerHandler[iFlexTimerReference]();                    // call user handler
        uEnable_Interrupt();
    }
}

static __interrupt void _flexTimerInterrupt_0(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_0, 0);
}

    #if FLEX_TIMERS_AVAILABLE > 1 && !defined NO_FLEX_TIMER_2
static __interrupt void _flexTimerInterrupt_1(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_1, 1);
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _flexTimerInterrupt_2(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_2, 2);
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _flexTimerInterrupt_3(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_3, 3);
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
static __interrupt void _flexTimerInterrupt_4(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_4, 4);
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
static __interrupt void _flexTimerInterrupt_5(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_5, 5);
}
    #endif
#endif


/* =================================================================== */
/*                     FlexTimer Configuration                         */
/* =================================================================== */


#if defined _FLEXTIMER_CONFIG_CODE
        {
            TIMER_INTERRUPT_SETUP *ptrTimerSetup = (TIMER_INTERRUPT_SETUP *)ptrSettings;
            int iInterruptID;
            register int iTimerReference = ptrTimerSetup->timer_reference;
            register unsigned long ulDelay = ptrTimerSetup->timer_value;
            register int iPrescaler = 0;
            FLEX_TIMER_MODULE *ptrFlexTimer;
            int iReducedFunctionality = 0;
    #if defined KINETIS_KL
        #if !defined KINETIS_WITH_PCC
            unsigned long ulExtSelect;
            #if defined TPM_CLOCKED_FROM_MCGIRCLK                        // {2}
                #if !defined RUN_FROM_LIRC                               // {3} if the processor is running from the the internal clock we don't adjust settings here
            MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);                // enable internal reference clock and allow it to continue running in stop modes
                    #if defined USE_FAST_INTERNAL_CLOCK
            MCG_SC = 0;                                                  // remove fast IRC divider
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock (4MHz)
                    #else
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock (32kHz)
                    #endif
                #endif
            SIM_SOPT2 |= SIM_SOPT2_TPMSRC_MCGIRCLK;                      // use MCGIRCLK as timer clock source
            #elif defined TPM_CLOCKED_FROM_OSCERCLK
            OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);               // enable the external reference clock and keep it enabled in stop mode
            SIM_SOPT2 |= (SIM_SOPT2_TPMSRC_OSCERCLK);                    // use OSCERCLK as timer clock source
            #else
            SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_TPMSRC_MCG);   // use MCGPLLCLK/2 (or MCGFLL if FLL is used)
            #endif
        #endif
    #endif
            switch (iTimerReference) {                                   // FlexTimer/TPM to be used
            case 0:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
    #if defined KINETIS_WITH_PCC
                    PCC_FTM0 = 0;                                        // disable clocks to module
    #else
                    POWER_DOWN_ATOMIC(6, FTM0);
    #endif
                    return;
                }
    #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM0, FTM0_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 0
    #endif
                POWER_UP_ATOMIC(6, FTM0);                                // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_0;         // KL and KE parts actually use the TPM which is however very similar to the FlexTimer
    #if defined KINETIS_KL
                iInterruptID = irq_TPM0_ID;
        #if !defined KINETIS_WITH_PCC
            #if defined KINETIS_KL82                                     // {5}
                ulExtSelect = SIM_SOPT9_TPM0CLKSEL;
            #else
                ulExtSelect = SIM_SOPT4_FTM0CLKSEL;
            #endif
        #endif
    #else
                iInterruptID = irq_FTM0_ID;
    #endif
    #if defined FLEX_TIMER_0_REDUCED
                iReducedFunctionality = 1;
    #endif
                break;
    #if FLEX_TIMERS_AVAILABLE > 1 && !defined NO_FLEX_TIMER_2
            case 1:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
        #if defined KINETIS_WITH_PCC
                    PCC_FTM1 = 0;                                        // disable clocks to module
        #else
                    POWER_DOWN_ATOMIC(6, FTM1);
        #endif
                    return;
                }
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM1, FTM1_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 1
        #endif
                POWER_UP_ATOMIC(6, FTM1);                                // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_1;         // KL and KE parts actually use the TPM which is however very similar to the FlexTimer
        #if defined KINETIS_KL
                iInterruptID = irq_TPM1_ID;
            #if !defined KINETIS_WITH_PCC
                #if defined KINETIS_KL82                                 // {5}
                ulExtSelect = SIM_SOPT9_TPM1CLKSEL;
                #else
                ulExtSelect = SIM_SOPT4_FTM1CLKSEL;
                #endif
            #endif
        #else
                iInterruptID = irq_FTM1_ID;
        #endif
        #if defined FLEX_TIMER_1_REDUCED
                iReducedFunctionality = 1;
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
        #if defined KINETIS_WITH_PCC
                    PCC_FTM2 = 0;                                        // disable clocks to module
        #else
            #if defined KINETIS_KL || defined KINETIS_K22_SF7
                    POWER_DOWN_ATOMIC(6, FTM2);
            #else
                    POWER_DOWN_ATOMIC(3, FTM2);
            #endif
        #endif
                    return;
                }
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM2, FTM2_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 2
        #endif
        #if defined KINETIS_KL || defined KINETIS_K22_SF7
                POWER_UP_ATOMIC(6, FTM2);                                // ensure that the FlexTimer module is powered up
        #else
                POWER_UP_ATOMIC(3, FTM2);                                // ensure that the FlexTimer module is powered up
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_2;         // KL and KE parts actually use the TPM which is however very similar to the FlexTimer
        #if defined KINETIS_KL
                iInterruptID = irq_TPM2_ID;
            #if !defined KINETIS_WITH_PCC
                #if defined KINETIS_KL82                                 // {5}
                ulExtSelect = SIM_SOPT9_TPM2CLKSEL;
                #else
                ulExtSelect = SIM_SOPT4_FTM2CLKSEL;
                #endif
            #endif
        #else
                iInterruptID = irq_FTM2_ID;
        #endif
        #if defined FLEX_TIMER_2_REDUCED
                iReducedFunctionality = 1;
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3 && !defined KINETIS_KE18
            case 3:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
        #if defined KINETIS_WITH_PCC
                    PCC_FTM3 = 0;                                        // disable clocks to module
        #elif defined KINETIS_K22_SF7
                    POWER_DOWN_ATOMIC(6, FTM3);
        #else
                    POWER_DOWN_ATOMIC(3, FTM3);
        #endif
                    return;
                }
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM3, FTM3_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 3
        #endif
        #if defined KINETIS_K22_SF7
                POWER_UP_ATOMIC(6, FTM3);                                // ensure that the FlexTimer module is powered up
        #else
                POWER_UP_ATOMIC(3, FTM3);                                // ensure that the FlexTimer module is powered up
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_3;
                iInterruptID = irq_FTM3_ID;
        #if defined FLEX_TIMER_3_REDUCED
                iReducedFunctionality = 1;
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4                                        // {9} K65/K66/K80 - TPM1
            case 4:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
                    POWER_DOWN_ATOMIC(2, TPM1);
                    return;
                }
                POWER_UP_ATOMIC(2, TPM1);                                // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_4;
                iInterruptID = irq_TPM1_ID;
        #if defined FLEX_TIMER_4_REDUCED
                iReducedFunctionality = 1;
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5                                        // {9} K65/K66/K80 - TPM2
            case 5:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
                    POWER_DOWN_ATOMIC(2, TPM2);
                    return;
                }
                POWER_UP_ATOMIC(2, TPM2);                                // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_5;
                iInterruptID = irq_TPM2_ID;
        #if defined FLEX_TIMER_4_REDUCED
                iReducedFunctionality = 1;
        #endif
                break;
    #endif
            default:
                _EXCEPTION("FlexTimer is unavailable!!");
                return;
            }
            if ((ptrTimerSetup->timer_mode & (TIMER_FREEZE | TIMER_CONTINUE)) != 0) { // {8}
                if ((ptrTimerSetup->timer_mode & TIMER_FREEZE) != 0) {
                    ucClockSource[iTimerReference] = (unsigned char)(ptrFlexTimer->FTM_SC & FTM_SC_CLKS_MASK); // back-up the original clock source
                    ptrFlexTimer->FTM_SC &= ~(FTM_SC_CLKS_MASK);         // disable the clock so that the timer freezes
                }
                else {
                    ptrFlexTimer->FTM_SC |= ucClockSource[iTimerReference]; // restore the clock so that the timer continues to run again
                }
                return;                                                  // work complete
            }
            ptrFlexTimer->FTM_SC = 0;                                    // ensure not operating
            if ((ptrFlexTimer->FTM_SC & FTM_SC_TOF) != 0) {              // ensure no pending interrupt
                ptrFlexTimer->FTM_SC = 0;                                // clear pending interrupt (requires a read of the interrupt bit at '1' beforehand)
            }
            if (iReducedFunctionality == 0) {                            // don't configure debugger behaviour on timers with reduced functionality
                ptrFlexTimer->FTM_CONF = FTM_DEBUG_BEHAVIOUR;            // set the debugging behaviour (whether the counter runs in debug mode and how the outputs react)
            }
    #if !defined KINETIS_KL && !defined KINETIS_KE
            ptrFlexTimer->FTM_CNTIN = 0;                                 // counter start value
            ptrFlexTimer->FTM_CNT = 0;                                   // {7} cause the counter to be set to zero
    #endif
    #if defined SUPPORT_CAPTURE
            if ((ptrTimerSetup->timer_mode & TIMER_CAPTURE_RISING_FALLING) != 0) { // {6} if capture mode is required
                unsigned long ulCharacteristics = PORT_PS_UP_ENABLE;
                unsigned long ulEdge;
                iPrescaler = ptrTimerSetup->capture_prescaler;           // the capture clock prescaler
                ulDelay = 0xffff;                                        // set maximum count value so that the timer free-runs
                switch (ptrTimerSetup->timer_mode & TIMER_CAPTURE_RISING_FALLING) {
                case TIMER_CAPTURE_RISING_FALLING:
                    ulEdge = (FTM_CSC_ELSA | FTM_CSC_ELSB);              // capture on rising and falling edges
                    break;
                case TIMER_CAPTURE_RISING:
                    ulCharacteristics = PORT_PS_DOWN_ENABLE;
                    ulEdge = FTM_CSC_ELSA;                               // capture on rising edge
                    break;
              //case TIMER_CAPTURE_FALLING:
                default:
                    ulEdge = FTM_CSC_ELSB;                               // capture on falling edge
                    break;
                }
        #if defined KINETIS_KL && defined _WINDOWS
                if (ptrTimerSetup->capture_channel > 3) {
                    _EXCEPTION("Invalid capture channel");
                }
        #endif
                fnConfigTimerPin(iTimerReference, ptrTimerSetup->capture_channel, ulCharacteristics);
                ptrFlexTimer->FTM_channel[ptrTimerSetup->capture_channel].FTM_CSC = ulEdge; // program the edge sensitivity of the capture input
        #if defined KINETIS_KL
                ptrFlexTimer->FTM_CONF |= (FTM_CONF_TRGSEL0 << ptrTimerSetup->capture_channel); // enable trigger source
        #else
                _EXCEPTION("Mode not available");
        #endif
            }
            else {
    #endif
                while (ulDelay > 0xffff) {                               // calculate the optimal prescaler setting
                    if (iPrescaler >= 7) {
                        ulDelay = 0xffff;                                // set maximum delay
                        break;
                    }
                    iPrescaler++;
                    ulDelay /= 2;
                }
    #if defined SUPPORT_CAPTURE
            }
    #endif
            iPrescaler &= 0x7f;
            usFlexTimerMode[iTimerReference] = (unsigned short)iPrescaler;
            if ((ptrTimerSetup->timer_mode & TIMER_PERIODIC) != 0) {     // if periodic operation required
                usFlexTimerMode[iTimerReference] |= FLEX_TIMER_PERIODIC; // mark that periodic mode is being used
            }
            ptrFlexTimer->FTM_MOD = ulDelay;                             // set upper count value
    #if defined KINETIS_KL
            if ((ptrTimerSetup->timer_mode & (TIMER_EXT_CLK_0 | TIMER_EXT_CLK_1)) != 0) { // {4} the external clock source is to be used
                usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_EXT | FTM_SC_TOIE | FTM_SC_TOF); // select external clock (which should be half the speed of the module's clock due to synchronisation requirements)
                if ((ptrTimerSetup->timer_mode & (TIMER_EXT_CLK_1)) != 0) {
        #if !defined KINETIS_WITH_PCC
            #if defined KINETIS_KL82                                     // {5}
                    SIM_SOPT9 |= ulExtSelect;                            // select CLKIN1 source to this timer
            #else
                    SIM_SOPT4 |= ulExtSelect;                            // select CLKIN1 source to this timer
            #endif
        #endif
        #if defined KINETIS_KL02 || defined KINETIS_KL03
                    _CONFIG_PERIPHERAL(B, 6, (PB_6_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PB.6 (alt. function 3)
        #else
                    _CONFIG_PERIPHERAL(E, 30, (PE_30_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PE.30 (alt. function 4)
        #endif
                }
                else {
        #if !defined KINETIS_WITH_PCC
            #if defined KINETIS_KL82                                     // {5}
                    SIM_SOPT9 &= ~(ulExtSelect);                         // select CLKIN0 source to this timer
            #else
                    SIM_SOPT4 &= ~(ulExtSelect);                         // select CLKIN0 source to this timer
            #endif
        #endif
        #if defined KINETIS_KL02 || defined KINETIS_KL03
                    _CONFIG_PERIPHERAL(A, 12, (PA_12_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.12 (alt. function 3)
                  //_CONFIG_PERIPHERAL(A, 1, (PA_1_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.1 (alt. function 2)
        #else
                    _CONFIG_PERIPHERAL(E, 29, (PE_29_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PE.29 (alt. function 4)
        #endif
                }
            }
            else {
                usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_SYS | FTM_SC_TOF); // set mode to start (shared by all channels) - system clock with overflow interrupt enabled [FTM_SC_TOF must be written with 1 to clear]
            }
    #elif defined FTM_CLOCKED_FROM_MCGFFLCLK
            // Ensure that slow internal clock is enabled and selected for MCGFFCLK
            //
            MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN | MCG_C1_IREFS); // enable internal reference clock and allow it to continue running in stop modes, plus select it as FLL reference (MCGFFLCLK)
            usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_FIX);       // set fixed clock
    #else
            usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_SYS);       // {1} set mode to start (shared by all channels) - system clock with overflow interrupt enabled [FTM_SC_TOF must be written with 0 to clear]
    #endif
            if ((_flexTimerHandler[iTimerReference] = ptrTimerSetup->int_handler) != 0) { // enter the user interrupt handler
                fnEnterInterrupt(iInterruptID, ptrTimerSetup->int_priority, _flexTimerInterrupt[iTimerReference]); // enter flex timer interrupt handler
    #if defined SUPPORT_CAPTURE
                if ((ptrTimerSetup->timer_mode & TIMER_CAPTURE_RISING_FALLING) != 0) {
                    ptrFlexTimer->FTM_channel[ptrTimerSetup->capture_channel].FTM_CSC |= FTM_CSC_CHIE; // enable channel interrupt
                }
                else {
    #endif
                    usFlexTimerMode[iTimerReference] |= FTM_SC_TOIE;     // enable timer overflow interrupt
    #if defined SUPPORT_CAPTURE
                }
    #endif
            }
    #if !defined DEVICE_WITHOUT_DMA
            if ((ptrTimerSetup->timer_mode & TIMER_DMA_TRIGGER) != 0) {  // when DMA required
                ptrFlexTimer->FTM_channel[0].FTM_CV = 0;                 // set the match value for channel 0
                ptrFlexTimer->FTM_channel[0].FTM_CSC = FTM_CSC_DMA;      // enable DMA trigger on match
            }
    #endif
            ptrFlexTimer->FTM_SC = (usFlexTimerMode[iTimerReference] & FTM_SC_USED_MASK); // start timer
        }
#endif


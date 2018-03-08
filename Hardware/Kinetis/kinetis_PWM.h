/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_PWM.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    22.07.2014 Add clock source selection to TPM                         {1}
    04.01.2016 Added DMA buffer to PWM support                           {2}
    05.01.2016 Added optional PWM cycle interrupt                        {3}
    16.12.2016 Correct PWM interrupt clear                               {4}
    04.01.2017 Don't adjust the RC clock setting when the processor is running from it {5}
    05.03.2017 Add PWM_NO_OUTPUT option to allow PWM channel operation without connecting to an output {6}
    24.04.2017 Add DMA based freqence control opton (eg. for stepper motors) {7}
    20.05.2017 PWM output configuration moded to kinetis.c [kinetis_timer_pins.h] so that it can be shared by capture input use
    20.11.2017 Add KE15 PWM channel output enable                        {8}

*/


#if defined _PWM_CODE                                                    // {3}
/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _PWM_Interrupt_0(void);
    #if FLEX_TIMERS_AVAILABLE > 1
static __interrupt void _PWM_Interrupt_1(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _PWM_Interrupt_2(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _PWM_Interrupt_3(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
static __interrupt void _PWM_Interrupt_4(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
static __interrupt void _PWM_Interrupt_5(void);
    #endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_PWM_TimerHandler[FLEX_TIMERS_AVAILABLE])(void) = {0};     // user interrupt handlers

static void (*_PWM_TimerInterrupt[FLEX_TIMERS_AVAILABLE])(void) = {
    _PWM_Interrupt_0,
    #if FLEX_TIMERS_AVAILABLE > 1
    _PWM_Interrupt_1,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    _PWM_Interrupt_2,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    _PWM_Interrupt_3,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
    _PWM_Interrupt_4,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
    _PWM_Interrupt_5
    #endif
};

/* =================================================================== */
/*                   PWM cycle Interrupt Handlers                      */
/* =================================================================== */

static __interrupt void _PWM_Interrupt_0(void)
{
    FTM0_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[0] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[0]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}

    #if FLEX_TIMERS_AVAILABLE > 1
static __interrupt void _PWM_Interrupt_1(void)
{
    FTM1_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[1] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[1]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _PWM_Interrupt_2(void)
{
    FTM2_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[2] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[2]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _PWM_Interrupt_3(void)
{
    FTM3_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[3] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[3]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
static __interrupt void _PWM_Interrupt_4(void)
{
    FTM4_SC &= ~(FTM_SC_TOF);                                            // clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[4] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[4]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
static __interrupt void _PWM_Interrupt_5(void)
{
    FTM5_SC &= ~(FTM_SC_TOF);                                            // clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[5] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[5]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
#endif

#if defined _PWM_CONFIG_CODE
        {
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
            int iTPM_type = 0;
    #endif
            PWM_INTERRUPT_SETUP *ptrPWM_settings = (PWM_INTERRUPT_SETUP *)ptrSettings;
            int iInterruptID;
            unsigned long ulMode = ptrPWM_settings->pwm_mode;
            unsigned char ucChannel = (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK);
            unsigned char ucFlexTimer = (ptrPWM_settings->pwm_reference >> _TIMER_MODULE_SHIFT);
            FLEX_TIMER_MODULE *ptrFlexTimer;
    #if defined KINETIS_KL
        #if !defined KINETIS_WITH_PCC
            #if defined TPM_CLOCKED_FROM_MCGIRCLK                        // {1}
                #if !defined RUN_FROM_LIRC                               // {5} if the processor is running from the the internal clock we don't change settings here
            MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);                // enable internal reference clock and allow it to continue running in stop modes
                    #if defined USE_FAST_INTERNAL_CLOCK
            MCG_SC = MCG_SC_FCRDIV_1;                                    // remove fast IRC divider
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock (4MHz [8MHz for devices with MCG Lite]) as MCGIRCLK
                    #else
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock (32kHz [2MHz for devices with MCG Lite]) as MCGIRCLK
                    #endif
                #endif
            SIM_SOPT2 |= SIM_SOPT2_TPMSRC_MCGIRCLK;                      // use MCGIRCLK as timer clock source
            #elif defined TPM_CLOCKED_FROM_OSCERCLK
            OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);               // enable the external reference clock and keep it enabled in stop mode
            SIM_SOPT2 |= (SIM_SOPT2_TPMSRC_OSCERCLK);                    // use OSCERCLK as timer clock source
            #elif defined TPM_CLOCKED_FROM_IRC48M && defined SIM_SOPT2_PLLFLLSEL_IRC48M && (SIM_SOPT2_PLLFLLSEL_IRC48M != 0)
            SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_IRC48M | SIM_SOPT2_TPMSRC_MCG); // use IRC48M
            #else
            SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_TPMSRC_MCG);   // use MCGPLLCLK/2 (or MCGFLL if FLL is used)
            #endif
        #endif
    #endif
            switch (ucFlexTimer) {
            case 0:
    #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM0, FTM0_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 0
    #endif
                POWER_UP_ATOMIC(6, FTM0);                                // ensure that the FlexTimer/TPM module is powered up
    #if defined KINETIS_KL
                iInterruptID = irq_TPM0_ID;
    #else
                iInterruptID = irq_FTM0_ID;
    #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(0, (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK), (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
                }
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_0;
                break;
    #if FLEX_TIMERS_AVAILABLE > 1
            case 1:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM1, FTM1_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 1
        #endif
                POWER_UP_ATOMIC(6, FTM1);                                // ensure that the FlexTimer module is powered up
        #if defined KINETIS_KL
                iInterruptID = irq_TPM1_ID;
        #else
                iInterruptID = irq_FTM1_ID;
        #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(1, (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK), (PORT_SRE_FAST | PORT_DSE_HIGH));
                }
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_1;
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM2, FTM2_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 2
        #endif
        #if defined KINETIS_KL || defined KINETIS_K22_SF7
                POWER_UP_ATOMIC(6, FTM2);                                // ensure that the FlexTimer module is powered up
        #else
                POWER_UP_ATOMIC(3, FTM2);                                // ensure that the FlexTimer module is powered up
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_2;
        #if defined KINETIS_KE
                ptrFlexTimer->FTM_CONF = FTM_DEBUG_BEHAVIOUR;            // set the debugging behaviour (whether the counter runs in debug mode and how the outputs react - only available on FlexTimer 2)
        #endif
        #if defined KINETIS_KL
                iInterruptID = irq_TPM2_ID;
        #else
                iInterruptID = irq_FTM2_ID;
        #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(2, (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK), (PORT_SRE_FAST | PORT_DSE_HIGH));
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
            case 3:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM3, FTM3_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 3
        #endif
        #if defined KINETIS_K22_SF7
                POWER_UP_ATOMIC(6, FTM3);                                // ensure that the FlexTimer module is powered up
        #else
                POWER_UP_ATOMIC(3, FTM3);                                // ensure that the FlexTimer module is powered up
        #endif
        #if defined KINETIS_KL
                iInterruptID = irq_TPM3_ID;
        #else
                iInterruptID = irq_FTM3_ID;
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_3;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(3, (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK), (PORT_SRE_FAST | PORT_DSE_HIGH));
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
            case 4:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM1, FTM1_PCC_SOURCE);     // select the PCC clock used by TPM 1
        #endif
                POWER_UP_ATOMIC(2, TPM1);                                // ensure that the TPM module is powered up
                iInterruptID = irq_FTM1_ID;
                iTPM_type = 1;
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_4;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(4, (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK), (PORT_SRE_FAST | PORT_DSE_HIGH));
                }
                break;

            case 5:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM2, FTM2_PCC_SOURCE);     // select the PCC clock used by TPM 2
        #endif
                POWER_UP_ATOMIC(2, TPM2);                                // ensure that the TPM module is powered up
                iInterruptID = irq_FTM2_ID;
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_5;
                iTPM_type = 1;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(5, (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK), (PORT_SRE_FAST | PORT_DSE_HIGH));
                }
                break;
    #endif
            default:
    #if defined _WINDOWS
                _EXCEPTION("Invalid timer !!");
    #endif
                return;                                                  // invalid FlexTimer
            }
    #if !defined KINETIS_KE
            ptrFlexTimer->FTM_CONF = FTM_DEBUG_BEHAVIOUR;                // set the debugging behaviour (whether the counter runs in debug mode and how the outputs react)
    #endif
            if (PWM_EXTERNAL_CLK == (ulMode & FTM_SC_CLKS_EXT)) {        // if external clock source is to be used program the clock input
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
                if (iTPM_type != 0) {
                    ulMode &= ~(FTM_SC_CLKS_EXT);                        // convert FTM external clock to TPM external clock setting
                    ulMode |= FTM_SC_CLKS_FIX;
        #if defined TPM_CLKIN_1
                    SIM_SOPT9 |= (SIM_SOPT9_TPM1CLKSEL << (ucFlexTimer - 4)); // select TPM_CLKIN1
            #if defined TPMCLKIN1_ON_A
                    _CONFIG_PERIPHERAL(A, 19, (PA_19_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PA.19 (alt. function 7)
            #elif defined TPMCLKIN1_ON_B
                    _CONFIG_PERIPHERAL(B, 17, (PB_17_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PB.17 (alt. function 7)
            #else
                    _CONFIG_PERIPHERAL(C, 13, (PC_13_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PC.13 (alt. function 7)
            #endif
        #else
                    SIM_SOPT9 &= ~(SIM_SOPT9_TPM1CLKSEL << (ucFlexTimer - 4)); // select TPM_CLKIN0
            #if defined TPMCLKIN0_ON_A
                    _CONFIG_PERIPHERAL(A, 18, (PA_18_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.18 (alt. function 7)
            #elif defined TPMCLKIN0_ON_B
                    _CONFIG_PERIPHERAL(B, 16, (PB_16_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PB.16 (alt. function 7)
            #else
                    _CONFIG_PERIPHERAL(C, 12, (PC_12_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PC.12 (alt. function 7)
            #endif
        #endif
                }
                else {
    #endif
    #if defined KINETIS_KL28                                             // KL28 has a dedicated external clock pin for each timer
                    switch (ucFlexTimer) {
                    case 0:                                              // TPM0
        #if defined TPMCLKIN0_ON_E_HIGH
                        _CONFIG_PERIPHERAL(E, 29, (PE_29_TPM0_CLKIN | PORT_PS_UP_ENABLE)); // TPM0_CLKIN on PE.29 (alt. function 4)
        #elif defined TPMCLKIN0_ON_E_LOW
                        _CONFIG_PERIPHERAL(E, 16, (PE_16_TPM0_CLKIN | PORT_PS_UP_ENABLE)); // TPM0_CLKIN on PE.16 (alt. function 4)
        #elif defined TPMCLKIN0_ON_C
                        _CONFIG_PERIPHERAL(C, 12, (PC_12_TPM0_CLKIN | PORT_PS_UP_ENABLE)); // TPM0_CLKIN on PC.12 (alt. function 4)
        #elif defined TPMCLKIN0_ON_B
                        _CONFIG_PERIPHERAL(B, 16, (PB_16_TPM0_CLKIN | PORT_PS_UP_ENABLE)); // TPM0_CLKIN on PB.16 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 18, (PA_18_TPM0_CLKIN | PORT_PS_UP_ENABLE)); // TPM0_CLKIN on PA.18 (alt. function 4)
        #endif
                        break;
                    case 1:                                              // TPM1
        #if defined TPMCLKIN1_ON_E_HIGH
                        _CONFIG_PERIPHERAL(E, 30, (PE_30_TPM1_CLKIN | PORT_PS_UP_ENABLE)); // TPM1_CLKIN on PE.30 (alt. function 4)
        #elif defined TPMCLKIN1_ON_E_LOW
                        _CONFIG_PERIPHERAL(E, 17, (PE_17_TPM1_CLKIN | PORT_PS_UP_ENABLE)); // TPM1_CLKIN on PE.17 (alt. function 4)
        #elif defined TPMCLKIN1_ON_C
                        _CONFIG_PERIPHERAL(C, 13, (PC_13_TPM1_CLKIN | PORT_PS_UP_ENABLE)); // TPM1_CLKIN on PC.13 (alt. function 4)
        #elif defined TPMCLKIN1_ON_B
                        _CONFIG_PERIPHERAL(B, 17, (PB_17_TPM1_CLKIN | PORT_PS_UP_ENABLE)); // TPM1_CLKIN on PB.17 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 19, (PA_19_TPM1_CLKIN | PORT_PS_UP_ENABLE)); // TPM1_CLKIN on PA.19 (alt. function 4)
        #endif
                        break;
                    case 2:                                              // TPM2
        //#if defined TPMCLKIN1_ON_E_HIGH
                        _CONFIG_PERIPHERAL(E, 31, (PE_31_TPM2_CLKIN | PORT_PS_UP_ENABLE)); // TPM2_CLKIN on PE.31 (alt. function 4)
        //#elif defined TPMCLKIN1_ON_B
                        _CONFIG_PERIPHERAL(B, 11, (PB_11_TPM2_CLKIN | PORT_PS_UP_ENABLE)); // TPM2_CLKIN on PB.11 (alt. function 4)
        //#else
                        _CONFIG_PERIPHERAL(A, 20, (PA_20_TPM2_CLKIN | PORT_PS_UP_ENABLE)); // TPM2_CLKIN on PA.20 (alt. function 4)
        //#endif
                        break;
                    }
    #elif defined FTM_CLKIN_1                                            // use CLKIN1 source
        #if !defined KINETIS_KE
                    SIM_SOPT4 |= (SIM_SOPT4_FTM0CLKSEL << ucChannel);    // select CLKIN1 to FTN
        #endif
        #if defined KINETIS_KL02
                    _CONFIG_PERIPHERAL(B, 6, (PB_6_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PB.6 (alt. function 3)
        #elif defined KINETIS_KL04 || defined KINETIS_KL05
                    _CONFIG_PERIPHERAL(B, 17, (PB_17_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PB.17 (alt. function 2)
        #else
                    _CONFIG_PERIPHERAL(A, 19, (PA_19_FTM_CLKIN1 | PORT_PS_UP_ENABLE)); // FTM_CKLIN1 on PA.19 (alt. function 4)
        #endif
    #else                                                                // use CLKIN0 source
        #if !defined KINETIS_KE && !defined KINETIS_KL82 && !defined KINETIS_KL28
                    SIM_SOPT4 &= ~(SIM_SOPT4_FTM0CLKSEL << ucChannel);   // select CLKIN0 to FTM
        #endif
        #if defined KINETIS_KL02 && defined TPMCLKIN0_ALT
                    _CONFIG_PERIPHERAL(A, 12,  (PA_12_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.12 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL04 || defined KINETIS_KL05
                    _CONFIG_PERIPHERAL(A, 1,  (PA_1_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.1 (alt. function 2)
        #elif defined KINETIS_K66
                    _CONFIG_PERIPHERAL(C, 12, (PC_12_FTM_CLKIN0 | PORT_PS_UP_ENABLE)); // FTM_CLKIN0 on PC.12 (alt. function 4)
        #elif defined KINETIS_KL26
                    _CONFIG_PERIPHERAL(B, 16, (PB_16_FTM_CLKIN0 | PORT_PS_UP_ENABLE)); // FTM_CLKIN0 on PA.18 (alt. function 4)
        #else
                    _CONFIG_PERIPHERAL(A, 18, (PA_18_FTM_CLKIN0 | PORT_PS_UP_ENABLE)); // FTM_CLKIN0 on PA.18 (alt. function 4)
        #endif
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
                }
    #endif
            }
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
            if (iTPM_type != 0) {
        #if defined TPM_CLOCKED_FROM_MCGIRCLK
            #if !defined RUN_FROM_LIRC                                   // if the processor is running from the the internal clock we don't change settings here
                MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);            // enable internal reference clock and allow it to continue running in stop modes
                #if defined USE_FAST_INTERNAL_CLOCK
                MCG_SC = MCG_SC_FCRDIV_1;                                // remove fast IRC divider
                MCG_C2 |= MCG_C2_IRCS;                                   // select fast internal reference clock (4MHz [8MHz for devices with MCG Lite]) as MCGIRCLK
                #else
                MCG_C2 &= ~MCG_C2_IRCS;                                  // select slow internal reference clock (32kHz [2MHz for devices with MCG Lite]) as MCGIRCLK
                #endif
            #endif
                SIM_SOPT2 |= SIM_SOPT2_TPMSRC_MCGIRCLK;                  // use MCGIRCLK as timer clock source
        #elif defined TPM_CLOCKED_FROM_OSCERCLK
                OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);           // enable the external reference clock and keep it enabled in stop mode
                SIM_SOPT2 |= (SIM_SOPT2_TPMSRC_OSCERCLK);                // use OSCERCLK as timer clock source
        #else                                                            // use MCGPLLCLK, MCGPPL_CLK, IRC48M or USB1PFDCLK with optional divider
            #if defined TPM_CLOCKED_FROM_MCGFFLCLK
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_FLL | SIM_SOPT2_TPMSRC_ALT);
            #elif defined TPM_CLOCKED_FROM_IRC48M
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_IRC48M | SIM_SOPT2_TPMSRC_ALT);
            #elif defined TPM_CLOCKED_FROM_USB1_PDF
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_USB1_PFD_CLK | SIM_SOPT2_TPMSRC_ALT);
            #else                                                        // MCGPLLCLK by default
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_PLL | SIM_SOPT2_TPMSRC_ALT);
            #endif
        #endif
            }
    #endif
            if ((ulMode & PWM_POLARITY) != 0) {                          // polarity
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = FTM_CSC_MS_ELS_PWM_LOW_TRUE_PULSES;
            }
            else {
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = FTM_CSC_MS_ELS_PWM_HIGH_TRUE_PULSES;
            }
            //
            //ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = (FTM_CSC_ELSA | FTM_CSC_MSA);
            //
    #if !defined DEVICE_WITHOUT_DMA
            if ((ulMode & PWM_DMA_CHANNEL_ENABLE) != 0) {
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC |= (FTM_CSC_DMA | FTM_CSC_CHIE); // enable DMA trigger from this channel (also the interrupt needs to be enabled for the DMA to operate - interrupt is not generated in this configuration)
            }
    #endif
    #if !defined KINETIS_KL && !defined KINETIS_KE
            ptrFlexTimer->FTM_CNTIN = 0;
    #endif
            if ((ulMode & FTM_SC_CPWMS) != 0) {                          // if center-aligned
                ptrFlexTimer->FTM_MOD = (ptrPWM_settings->pwm_frequency / 2); // set the PWM period - valid for all channels of a single timer
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = (ptrPWM_settings->pwm_value / 2); // set the duty cycle for the particular channel
            }
            else {
                ptrFlexTimer->FTM_MOD = (ptrPWM_settings->pwm_frequency - 1); // set the PWM period - valid for all channels of a single timer
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = ptrPWM_settings->pwm_value; // set the duty cycle for the particular channel            
            }
    #if !defined DEVICE_WITHOUT_DMA                                      // {2}
            if ((ulMode & (PWM_FULL_BUFFER_DMA | PWM_HALF_BUFFER_DMA)) != 0) { // if DMA is being specified
                unsigned long ulDMA_rules = (DMA_DIRECTION_OUTPUT | DMA_HALF_WORDS);
                void *ptrRegister;
                if ((ulMode & PWM_FULL_BUFFER_DMA_AUTO_REPEAT) != 0) {
                    ulDMA_rules |= DMA_AUTOREPEAT;
                }
                if ((ulMode & PWM_HALF_BUFFER_DMA) != 0) {
                    ulDMA_rules |= DMA_HALF_BUFFER_INTERRUPT;
                }
                if ((ulMode & PWM_DMA_CONTROL_FREQUENCY) != 0) {         // {7}
                    ptrRegister = (void *)&ptrFlexTimer->FTM_MOD;        // each DMA trigger causes a new frequency to be set
                }
                else {
                    ptrRegister = (void *)&ptrFlexTimer->FTM_channel[ucChannel].FTM_CV; // each DMA trigger causes a new PWM value to be set
                }
                fnConfigDMA_buffer(ptrPWM_settings->ucDmaChannel, ptrPWM_settings->usDmaTriggerSource, ptrPWM_settings->ulPWM_buffer_length, ptrPWM_settings->ptrPWM_Buffer, ptrRegister, ulDMA_rules, ptrPWM_settings->dma_int_handler, ptrPWM_settings->dma_int_priority); // source is the PWM buffer and destination is the PWM mark-space ratio register
                fnDMA_BufferReset(ptrPWM_settings->ucDmaChannel, DMA_BUFFER_START);
            }
    #endif
            ulMode &= PWM_MODE_SETTINGS_MASK;                            // keep just the user's mode settings for the hardware
            if (ptrPWM_settings->int_handler != 0) {                     // {3} if an interrupt handler is specified it is called at each period
                _PWM_TimerHandler[ucFlexTimer] = ptrPWM_settings->int_handler;
                fnEnterInterrupt(iInterruptID, ptrPWM_settings->int_priority, _PWM_TimerInterrupt[ucFlexTimer]);
    #if defined KINETIS_KL
                ulMode |= (FTM_SC_TOIE | FTM_SC_TOF);                    // enable interrupt [FTM_SC_TOF must be written with 1 to clear]
    #else
                ulMode |= (FTM_SC_TOIE);                                 // enable interrupt 
    #endif
            }
    #if defined KINETIS_KE15                                             // {8}
            ulMode |= (ptrFlexTimer->FTM_SC & (FTM_SC_PWMEN0 | FTM_SC_PWMEN1 | FTM_SC_PWMEN2 | FTM_SC_PWMEN3 | FTM_SC_PWMEN4 | FTM_SC_PWMEN5 | FTM_SC_PWMEN6 | FTM_SC_PWMEN7)); // preserve already set PWM outputs
            ulMode |= (FTM_SC_PWMEN0 << ptrPWM_settings->pwm_reference); // enable the PWM channel output
    #endif
            ptrFlexTimer->FTM_SC = ulMode;                               // note that the mode is shared by all channels in the flex timer
    #if defined KINETIS_KE
            _SIM_PER_CHANGE;                                             // update simulator ports
    #endif
        }
#endif

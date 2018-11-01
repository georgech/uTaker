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
    24.04.2017 Add DMA based frequence control opton (eg. for stepper motors) {7}
    20.05.2017 PWM output configuration moded to kinetis.c [kinetis_timer_pins.h] so that it can be shared by capture input use
    20.11.2017 Add KE15 PWM channel output enable                        {8}
    19.03.2018 Add PWM clock source from TRGMUX settings                 {9}
    10.04.2018 Add KL27 clock inputs                                     {10}
    05.05.2018 Add user defined target register option for DMA operation {11}
    08.05.2018 Add channel interrupt support (in addition to period interrupts) {12}
    29.08.2018 Correct FTM2 clock gating for K64, K65 and K66            {13}
    05.10.2018 Add combined channel pairs to support phases shifted output pairs {14}
    08.10.2018 Correct support for channel interrupts on multiple timers {15}
    19.10.2018 Add option to modulate UART Tx outputs                    {16}

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
/*                              local consts                           */
/* =================================================================== */

static const unsigned char ucSumChannels[FLEX_TIMERS_AVAILABLE + 1] = {      // {12}
    0,
    FLEX_TIMERS_0_CHANNELS,
    (FLEX_TIMERS_0_CHANNELS + FLEX_TIMERS_1_CHANNELS),
#if FLEX_TIMERS_AVAILABLE > 2
    (FLEX_TIMERS_0_CHANNELS + FLEX_TIMERS_1_CHANNELS + FLEX_TIMERS_2_CHANNELS),
#endif
#if FLEX_TIMERS_AVAILABLE > 3
    (FLEX_TIMERS_0_CHANNELS + FLEX_TIMERS_1_CHANNELS + FLEX_TIMERS_2_CHANNELS + FLEX_TIMERS_3_CHANNELS),
#endif
#if FLEX_TIMERS_AVAILABLE > 4
    (FLEX_TIMERS_0_CHANNELS + FLEX_TIMERS_1_CHANNELS + FLEX_TIMERS_2_CHANNELS + FLEX_TIMERS_3_CHANNELS + FLEX_TIMERS_4_CHANNELS),
#endif
#if FLEX_TIMERS_AVAILABLE > 5
    (FLEX_TIMERS_0_CHANNELS + FLEX_TIMERS_1_CHANNELS + FLEX_TIMERS_2_CHANNELS + FLEX_TIMERS_3_CHANNELS + FLEX_TIMERS_4_CHANNELS + FLEX_TIMERS_5_CHANNELS),
#endif
};

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_PWM_TimerHandler[FLEX_TIMERS_AVAILABLE])(void) = {0};     // user period interrupt handlers
static void(*_PWM_ChannelHandler[FLEX_TIMERS_CHANNEL_COUNT])(void) = {0};// {12} user channel interrupt handlers
static unsigned char ucEnabledChannelInterrupts[FLEX_TIMERS_AVAILABLE] = {0}; // {15} enabled channel flags for each timer

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

#if defined _WINDOWS                                                     // reset channel event flags when simulating
static void fnResetChannelEventFlags(int iTimerRef, FLEX_TIMER_MODULE *ptrTimer)
{
    int iChannel = 0;
    while (iChannel < (ucSumChannels[iTimerRef + 1] - ucSumChannels[iTimerRef])) { // for each channel
        ptrTimer->FTM_channel[iChannel++].FTM_CSC &= ~FTM_CSC_CHF;       // reset the channel event flag
    }
}
#endif


static void fnHandleChannels(int iTimer, unsigned char ucPendingChannels)
{
    int iHandlerRef = ucSumChannels[iTimer];
    unsigned char ucChannel = 0x1;
    while (ucPendingChannels != 0) {                                     // while channels are pending
        if ((ucPendingChannels & ucChannel) != 0) {                      // if interrupt pending on this channel
            if (_PWM_ChannelHandler[iHandlerRef] != 0) {                 // if there is a user handler installed
                uDisable_Interrupt();
                    _PWM_ChannelHandler[iHandlerRef]();                  // call user interrupt handler
                uEnable_Interrupt();
            }
            ucPendingChannels &= ~(ucChannel);                           // reset handled channel flag
        }
        iHandlerRef++;
        ucChannel <<= 1;
    }
}

static __interrupt void _PWM_Interrupt_0(void)
{
    unsigned char ucPendingChannels = (unsigned char)(FTM0_STATUS & ucEnabledChannelInterrupts[0]); // {15}
    if (ucPendingChannels != 0) {
    #if defined KINETIS_KL
        WRITE_ONE_TO_CLEAR(FTM0_STATUS, (FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F)); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #else
        FTM0_STATUS &= ~(FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #endif
    #if defined _WINDOWS                                                 // reset channel event flags when simulating
        fnResetChannelEventFlags(0, (FLEX_TIMER_MODULE *)FTM_BLOCK_0);
    #endif
        fnHandleChannels(0, ucPendingChannels);
    }
    if ((FTM0_SC & FTM_SC_TOF) == 0) {                                   // if no period interrupt we return (probably it was a channel interrupt)
        return;
    }
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
    unsigned char ucPendingChannels = (unsigned char)(FTM1_STATUS & ucEnabledChannelInterrupts[1]); // {15}
    if (ucPendingChannels != 0) {
    #if defined KINETIS_KL
        WRITE_ONE_TO_CLEAR(FTM1_STATUS, (FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F)); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #else
        FTM1_STATUS &= ~(FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #endif
    #if defined _WINDOWS                                                 // reset channel event flags when simulating
        fnResetChannelEventFlags(1, (FLEX_TIMER_MODULE *)FTM_BLOCK_1);
    #endif
        fnHandleChannels(1, ucPendingChannels);
    }
    if ((FTM1_SC & FTM_SC_TOF) == 0) {                                   // if no period interrupt we return (probably it was a channel interrupt)
        return;
    }
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
    unsigned char ucPendingChannels = (unsigned char)(FTM2_STATUS & ucEnabledChannelInterrupts[2]); // {15}
    if (ucPendingChannels != 0) {
    #if defined KINETIS_KL
        WRITE_ONE_TO_CLEAR(FTM2_STATUS, (FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F)); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #else
        FTM2_STATUS &= ~(FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #endif
    #if defined _WINDOWS                                                 // reset channel event flags when simulating
        fnResetChannelEventFlags(2, (FLEX_TIMER_MODULE *)FTM_BLOCK_2);
    #endif
        fnHandleChannels(2, ucPendingChannels);
    }
    if ((FTM2_SC & FTM_SC_TOF) == 0) {                                   // if no period interrupt we return (probably it was a channel interrupt)
        return;
    }
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
    unsigned char ucPendingChannels = (unsigned char)(FTM3_STATUS & ucEnabledChannelInterrupts[3]); // {15}
    if (ucPendingChannels != 0) {
    #if defined KINETIS_KL
        WRITE_ONE_TO_CLEAR(FTM3_STATUS, (FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F)); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #else
        FTM3_STATUS &= ~(FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #endif
    #if defined _WINDOWS                                                 // reset channel event flags when simulating
        fnResetChannelEventFlags(3, (FLEX_TIMER_MODULE *)FTM_BLOCK_3);
    #endif
        fnHandleChannels(3, ucPendingChannels);
    }
    if ((FTM3_SC & FTM_SC_TOF) == 0) {                                   // if no period interrupt we return (probably it was a channel interrupt)
        return;
    }
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
    unsigned char ucPendingChannels = (unsigned char)(FTM4_STATUS & ucEnabledChannelInterrupts[4]); // {15}
    if (ucPendingChannels != 0) {
    #if defined KINETIS_KL || defined TPMS_AVAILABLE_TOO
        WRITE_ONE_TO_CLEAR(FTM4_STATUS, (FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F)); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #else
        FTM4_STATUS &= ~(FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #endif
    #if defined _WINDOWS                                                 // reset channel event flags when simulating
        fnResetChannelEventFlags(4, (FLEX_TIMER_MODULE *)FTM_BLOCK_4);
    #endif
        fnHandleChannels(4, ucPendingChannels);
    }
    if ((FTM4_SC & FTM_SC_TOF) == 0) {                                   // if no period interrupt we return (probably it was a channel interrupt)
        return;
    }
        #if defined TPMS_AVAILABLE_TOO                                   // TPM1
    OR_ONE_TO_CLEAR(FTM4_SC, FTM_SC_TOF);                                // clear interrupt (write 1 to reset)
        #else
    FTM4_SC &= ~(FTM_SC_TOF);                                            // clear interrupt (read when set and write 0 to reset)
        #endif
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
    unsigned char ucPendingChannels = (unsigned char)(FTM5_STATUS & ucEnabledChannelInterrupts[5]); // {15}
    if (ucPendingChannels != 0) {
    #if defined KINETIS_KL || defined TPMS_AVAILABLE_TOO
        WRITE_ONE_TO_CLEAR(FTM5_STATUS, (FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F)); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #else
        FTM5_STATUS &= ~(FTM_STATUS_CH7F | FTM_STATUS_CH6F | FTM_STATUS_CH5F | FTM_STATUS_CH4F | FTM_STATUS_CH3F | FTM_STATUS_CH2F | FTM_STATUS_CH1F | FTM_STATUS_CH0F); // reset the flags (hardware ensures that any flags being set between the previous read and the clear are not cleared)
    #endif
    #if defined _WINDOWS                                                 // reset channel event flags when simulating
        fnResetChannelEventFlags(5, (FLEX_TIMER_MODULE *)FTM_BLOCK_5);
    #endif
        fnHandleChannels(5, ucPendingChannels);
    }
    if ((FTM5_SC & FTM_SC_TOF) == 0) {                                   // if no period interrupt we return (probably it was a channel interrupt)
        return;
    }
        #if defined TPMS_AVAILABLE_TOO                                   // TPM2
    OR_ONE_TO_CLEAR(FTM5_SC, FTM_SC_TOF);                                // clear interrupt (write 1 to reset)
        #else
    FTM5_SC &= ~(FTM_SC_TOF);                                            // clear interrupt (read when set and write 0 to reset)
        #endif
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
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO
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
                    fnConfigTimerPin(0, ucChannel, (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
                }
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_0;
                break;
    #if FLEX_TIMERS_AVAILABLE > 1
            case 1:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM1, FTM1_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 1
        #endif
                POWER_UP_ATOMIC(6, FTM1);                                // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_1;
        #if defined KINETIS_KL
                iInterruptID = irq_TPM1_ID;
        #else
                iInterruptID = irq_FTM1_ID;
        #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(1, ucChannel, (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
                }
                // Handle timer 1 - channel options
                //
                switch (ucChannel) {
                case 0:
        #if defined SIM_SOPT5_LPUART0TXSRC_TPM1_0                        // {16} LPUART0 modulation
                    if ((ulMode & PWM_OPTION_MODULATE_LPUART0) != 0) {
                        SIM_SOPT5 |= SIM_SOPT5_LPUART0TXSRC_TPM1_0;      // the PWM output will be driven onto the LPUART0 Tx line
                    }
                    else {
                        SIM_SOPT5 &= ~(SIM_SOPT5_LPUART0TXSRC_TPM1_0);
                    }
        #endif
        #if defined SIM_SOPT5_LPUART1TXSRC_TPM1_0                        // {16} LPUART1 modulation
                    if ((ulMode & PWM_OPTION_MODULATE_LPUART1) != 0) {
                        SIM_SOPT5 |= SIM_SOPT5_LPUART1TXSRC_TPM1_0;      // the PWM output will be driven onto the LPUART1 Tx line
                    }
                    else {
                        SIM_SOPT5 &= ~(SIM_SOPT5_LPUART1TXSRC_TPM1_0);
                    }
        #endif
                    break;
                default:
                    break;
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM2, FTM2_PCC_SOURCE);     // select the PCC clock used by FlexTimer/TPM 2
        #endif
        #if defined KINETIS_KL || defined KINETIS_K22_SF7 || defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66 || defined KINETIS_KV50 // {13}
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
                    fnConfigTimerPin(2, ucChannel, (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
                }
                // Handle timer 2 - channel options
                //
                switch (ucChannel) {
                case 0:
        #if defined SIM_SOPT5_LPUART0TXSRC_TPM2_0                        // {16} LPUART0 modulation
                    if ((ulMode & PWM_OPTION_MODULATE_LPUART0) != 0) {
                        SIM_SOPT5 |= SIM_SOPT5_LPUART0TXSRC_TPM2_0;      // the PWM output will be driven onto the LPUART0 Tx line
                    }
                    else {
                        SIM_SOPT5 &= ~(SIM_SOPT5_LPUART0TXSRC_TPM2_0);
                    }
        #endif
        #if defined SIM_SOPT5_LPUART1TXSRC_TPM2_0                        // {16} LPUART1 modulation
                    if ((ulMode & PWM_OPTION_MODULATE_LPUART1) != 0) {
                        SIM_SOPT5 |= SIM_SOPT5_LPUART1TXSRC_TPM2_0;      // the PWM output will be driven onto the LPUART1 Tx line
                    }
                    else {
                        SIM_SOPT5 &= ~(SIM_SOPT5_LPUART1TXSRC_TPM2_0);
                    }
        #endif
                    break;
                default:
                    break;
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3 && !defined KINETIS_KE18
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
                    fnConfigTimerPin(3, ucChannel, (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO          // TPM1
            case 4:
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM1, FTM1_PCC_SOURCE);     // select the PCC clock used by TPM 1
        #endif
                POWER_UP_ATOMIC(2, TPM1);                                // ensure that the TPM module is powered up
                iInterruptID = irq_TPM1_ID;
                iTPM_type = 1;
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_4;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(4, ucChannel, (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
                }
                break;

            case 5:                                                      // TPM2
        #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15
                SELECT_PCC_PERIPHERAL_SOURCE(FTM2, FTM2_PCC_SOURCE);     // select the PCC clock used by TPM 2
        #endif
                POWER_UP_ATOMIC(2, TPM2);                                // ensure that the TPM module is powered up
                iInterruptID = irq_TPM2_ID;
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_5;
                iTPM_type = 1;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    fnConfigTimerPin(5, ucChannel, (PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the PWM output pin
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
            if (PWM_EXTERNAL_CLK == (ulMode & FTM_SC_CLKS_MASK)) {       // if external clock source is to be used program the clock input
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO
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
        #if defined TPMCLKIN1_ON_E_HIGH
                        _CONFIG_PERIPHERAL(E, 31, (PE_31_TPM2_CLKIN | PORT_PS_UP_ENABLE)); // TPM2_CLKIN on PE.31 (alt. function 4)
        #elif defined TPMCLKIN1_ON_B
                        _CONFIG_PERIPHERAL(B, 11, (PB_11_TPM2_CLKIN | PORT_PS_UP_ENABLE)); // TPM2_CLKIN on PB.11 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 20, (PA_20_TPM2_CLKIN | PORT_PS_UP_ENABLE)); // TPM2_CLKIN on PA.20 (alt. function 4)
        #endif
                        break;
                    }
    #elif defined KINETIS_KL27                                           // {10} KL27 has two external clock sources (TPM_CLKIN0 and TPM_CLKIN1) which can be individually defined for use by each of the TPMs
                    if ((ulMode & PWM_SOURCE_CLKIN1) != 0) {             // use TPM_CLKIN1
        #if defined TPMCLKIN1_ON_E_HIGH
                        _CONFIG_PERIPHERAL(E, 30, (PE_30_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PE.30 (alt. function 4)
        #elif defined TPMCLKIN1_ON_E_LOW
                        _CONFIG_PERIPHERAL(E, 17, (PE_17_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PE.17 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 19, (PA_19_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PA.19 (alt. function 4)
        #endif
                        SIM_SOPT4 |= (SIM_SOPT4_FTM0CLKSEL << ucFlexTimer); // select CLKIN1 to TPM
                    }
                    else {                                               // use TPM_CLKIN0
        #if defined TPMCLKIN0_ON_E_HIGH
                        _CONFIG_PERIPHERAL(E, 29, (PE_29_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PE.29 (alt. function 4)
        #elif defined TPMCLKIN0_ON_E_LOW
                        _CONFIG_PERIPHERAL(E, 16, (PE_16_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PE.16 (alt. function 4)
        #elif defined TPMCLKIN0_ON_B
                        _CONFIG_PERIPHERAL(B, 16, (PB_16_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PB.16 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 18, (PA_18_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.18 (alt. function 4)
        #endif
                        SIM_SOPT4 &= ~(SIM_SOPT4_FTM0CLKSEL << ucFlexTimer); // select CLKIN0 to TPM
                    }
    #elif defined FTM_CLKIN_1                                            // use CLKIN1 source
        #if !defined KINETIS_KE
                    SIM_SOPT4 |= (SIM_SOPT4_FTM0CLKSEL << ucFlexTimer);  // select CLKIN1 to FTM
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
                    SIM_SOPT4 &= ~(SIM_SOPT4_FTM0CLKSEL << ucFlexTimer); // select CLKIN0 to FTM
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
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO
                }
    #endif
    #if defined FTM_CONF_TRGPOL_LOW
                if ((PWM_TRIGGER_CLOCK_INVERT & ulMode) != 0) {
                    ptrFlexTimer->FTM_CONF |= FTM_CONF_TRGPOL_LOW;       // falling edge triggered (note that this has no effect on the KL28)
                }
    #endif
            }
    #if defined TRGMUX_AVAILABLE && defined FTM_SC_CLK_TRIGGER           // {9}
            else if (PWM_TRIGGER_CLK == (ulMode & FTM_SC_CLKS_MASK)) {   // if trigger clock source is to be used program the trigger input connection using the TRGMUX
                switch (ucFlexTimer) {
                case 0:                                                  // FTM0/TPM0
                    TRGMUX_FTM0 = ptrPWM_settings->ucTriggerSource;      // connect the trigger source to the clock input
                    break;
                case 1:                                                  // FTM1/TPM1
                    TRGMUX_FTM1 = ptrPWM_settings->ucTriggerSource;      // connect the trigger source to the clock input
                    break;
                case 2:                                                  // FTM2/TPM2
                    TRGMUX_FTM2 = ptrPWM_settings->ucTriggerSource;      // connect the trigger source to the clock input
                    break;
                }
                ptrFlexTimer->FTM_CONF &= ~(FTM_CONF_TRGSRC_INTERNAL | FTM_CONF_TRGSEL_EXT_MASK); // ensure external trigger on trigger 0
            }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO
            if (iTPM_type != 0) {                                        // configure for TPM operation
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
            //ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = (FTM_CSC_ELSA | FTM_CSC_MSA); // test line
            //
    #if !defined DEVICE_WITHOUT_DMA
            if ((ulMode & PWM_DMA_CHANNEL_ENABLE) != 0) {
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC |= (FTM_CSC_DMA | FTM_CSC_CHIE); // enable DMA trigger from this channel (also the interrupt needs to be enabled for the DMA to operate - interrupt is however not generated in this configuration)
            }
    #endif
    #if !defined KINETIS_KL && !defined KINETIS_KE
            ptrFlexTimer->FTM_CNTIN = 0;
    #endif
            if ((ulMode & FTM_SC_CPWMS) != 0) {                          // if center-aligned
                ptrFlexTimer->FTM_MOD = (ptrPWM_settings->pwm_frequency/2); // set the PWM period - valid for all channels of a single timer
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = (ptrPWM_settings->pwm_value / 2); // set the duty cycle for the particular channel
    #if defined _WINDOWS && defined FTM_MODE_FTMEN
                if ((ulMode & PWM_COMBINED_PHASE_SHIFT) != 0) {          // {14}
                    _EXCEPTION("Combined channels are not possible in center-aligned mode!");
                }
    #endif
            }
            else {                                                       // edge aligned
                ptrFlexTimer->FTM_MOD = (ptrPWM_settings->pwm_frequency - 1); // set the PWM period - valid for all channels of a single timer
    #if defined FTM_MODE_FTMEN
                if ((ulMode & PWM_COMBINED_PHASE_SHIFT) != 0) {          // {14} only possible in edge aligned mode
                    if ((ucChannel & 1) == 0) {                          // the first combined channel pair defines the shift from the start position
                        ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = ptrPWM_settings->pwm_phase_shift; // set the delay before the pulse starts
                    }
                    else {                                               // the second in the channel pair defines the end of the pulse
                        unsigned short usEnd = (ptrPWM_settings->pwm_phase_shift + ptrPWM_settings->pwm_value);
                        if (usEnd > (ptrPWM_settings->pwm_frequency - 1)) { // limit the end - the phase shift is effectively limited by the pulse width duration
                            usEnd = (ptrPWM_settings->pwm_frequency - 1);
                        }
                        ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = usEnd; // set the point where the pulse ends
                        if ((ulMode & PWM_POLARITY) != 0) {              // polarity
                            ptrFlexTimer->FTM_COMBINE |= (FTM_COMBINE_COMP0 << (8 * (ucChannel/2))); // second channel is complimentary of first channel pair
                        }
                    }
                  //ptrFlexTimer->FTM_MODE |= (FTM_MODE_FTMEN);          // allow extended features
                    ptrFlexTimer->FTM_COMBINE |= (FTM_COMBINE_COMBINE0 << (8 * (ucChannel/2))); // set combined mode
                }
                else {
                    ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = ptrPWM_settings->pwm_value; // set the duty cycle for the particular channel
                }
    #else
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = ptrPWM_settings->pwm_value; // set the duty cycle for the particular channel
    #endif
            }
    #if !defined DEVICE_WITHOUT_DMA                                      // {2}
            if ((ulMode & (PWM_FULL_BUFFER_DMA | PWM_HALF_BUFFER_DMA)) != 0) { // if DMA is being specified
                unsigned long ulDMA_rules = (DMA_DIRECTION_OUTPUT | DMA_HALF_WORDS); // default DMA rules
                void *ptrRegister;
                if ((ulMode & PWM_FULL_BUFFER_DMA_AUTO_REPEAT) != 0) {
                    ulDMA_rules |= DMA_AUTOREPEAT;
                }
                if ((ulMode & PWM_HALF_BUFFER_DMA) != 0) {
                    ulDMA_rules |= DMA_HALF_BUFFER_INTERRUPT;
                }
                if ((ulMode & PWM_DMA_SPECIFY_DESTINATION) == 0) {       // {11}
                    if ((ulMode & PWM_DMA_CONTROL_FREQUENCY) != 0) {     // {7}
                        ptrRegister = (void *)&ptrFlexTimer->FTM_MOD;    // each DMA trigger causes a new frequency to be set
                    }
                    else {
                        ptrRegister = (void *)&ptrFlexTimer->FTM_channel[ucChannel].FTM_CV; // each DMA trigger causes a new PWM value to be set
                    }
                }
                else {
                    ptrRegister = ptrPWM_settings->ptrRegister;          // {11} else use the destination register as specified by the user
                    if (((PWM_DMA_SPECIFY_LONG_WORD | PWM_DMA_SPECIFY_BYTE) & ulMode) != 0) { // and use the specified destination width
                        ulDMA_rules &= ~(DMA_HALF_WORDS);
                        if ((PWM_DMA_SPECIFY_LONG_WORD & ulMode) != 0) {
                            ulDMA_rules |= (DMA_LONG_WORDS);
                        }
                        else {
                            ulDMA_rules |= (DMA_BYTES);
                        }
                    }
                }
                fnConfigDMA_buffer(ptrPWM_settings->ucDmaChannel, ptrPWM_settings->usDmaTriggerSource, ptrPWM_settings->ulPWM_buffer_length, ptrPWM_settings->ptrPWM_Buffer, ptrRegister, ulDMA_rules, ptrPWM_settings->dma_int_handler, ptrPWM_settings->dma_int_priority); // source is the PWM buffer and destination is the PWM mark-space ratio register
                fnDMA_BufferReset(ptrPWM_settings->ucDmaChannel, DMA_BUFFER_START);
            }
    #endif
            ulMode &= PWM_MODE_SETTINGS_MASK;                            // keep just the user's mode settings for the hardware
            if  (ptrPWM_settings->int_handler != 0) {                    // {3} if an interrupt handler is specified it is called at each period
                _PWM_TimerHandler[ucFlexTimer] = ptrPWM_settings->int_handler; // period interrupt handler
                fnEnterInterrupt(iInterruptID, ptrPWM_settings->int_priority, _PWM_TimerInterrupt[ucFlexTimer]);
    #if defined KINETIS_KL
                ulMode |= (FTM_SC_TOIE | FTM_SC_TOF);                    // enable interrupt [FTM_SC_TOF must be written with 1 to clear]
    #elif FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO
                if (iTPM_type != 0) {
                    ulMode |= (FTM_SC_TOIE | FTM_SC_TOF);                // enable interrupt [FTM_SC_TOF must be written with 1 to clear]
                }
                else {
                    ulMode |= (FTM_SC_TOIE);                             // enable interrupt 
                }
    #else
                ulMode |= (FTM_SC_TOIE);                                 // enable interrupt
    #endif
            }
            if (((ptrPWM_settings->pwm_mode & PWM_CHANNEL_INTERRUPT) != 0) && (ptrPWM_settings->channel_int_handler != 0)) { // {12} if a channel match interrupt is to be generated
                unsigned char ucChannelReference = (ucSumChannels[ucFlexTimer] + ucChannel);
                _PWM_ChannelHandler[ucChannelReference] = ptrPWM_settings->channel_int_handler; // channel interrupt handler
                fnEnterInterrupt(iInterruptID, ptrPWM_settings->int_priority, _PWM_TimerInterrupt[ucFlexTimer]);
                ucEnabledChannelInterrupts[ucFlexTimer] |= (1 << ucChannel); // {15} mark that this flextimer channel is enabled for interrupts
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC |= FTM_CSC_CHIE; // enable channel interrupt
            }
            else {
                ucEnabledChannelInterrupts[ucFlexTimer] &= ~(1 << ucChannel); // {15}
            }
    #if defined KINETIS_KE15                                             // {8}
            ulMode |= (ptrFlexTimer->FTM_SC & (FTM_SC_PWMEN0 | FTM_SC_PWMEN1 | FTM_SC_PWMEN2 | FTM_SC_PWMEN3 | FTM_SC_PWMEN4 | FTM_SC_PWMEN5 | FTM_SC_PWMEN6 | FTM_SC_PWMEN7)); // preserve already set PWM outputs
            ulMode |= (FTM_SC_PWMEN0 << ptrPWM_settings->pwm_reference); // enable the PWM channel output
    #endif
            ptrFlexTimer->FTM_SC = ulMode;                               // note that the mode is shared by all channels in the flex timer [when the clock is stopped to the flextimer its outputs are also tri-stated, meaning that pull-up/downs on the outputs can be used to determine their idle state]
    #if defined _WINDOWS
            ptrFlexTimer->FTM_SC &= ~(FTM_SC_TOF);                       // this is a write '1' to clear flag so we reset it when simulating
    #endif
    #if defined KINETIS_KE
            _SIM_PER_CHANGE;                                             // update simulator ports
    #endif
        }
#endif

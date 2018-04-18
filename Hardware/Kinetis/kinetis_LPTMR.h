/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_LPTRM.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    14.09.2017 Extended to support multiple LPTMRs
    18.04.2018 Add interrupt to handle both single-shot and periodic interrupts when fixed vector is to be used {1}

*/

#if defined _LPTMR_CODE

static void (*LPTMR_interrupt_handler[LPTMR_AVAILABLE])(void) = {0};

#if defined INTERRUPT_VECTORS_IN_FLASH                                   // {1} when interrupt vectors are fixed in flash the decision between periodic and single-shot needs to be performed in code
static unsigned char ucPeriodicMode = 0;

static __interrupt void _LPTMR0_handler(void)
{
    if (ucPeriodicMode != 0) {
        LPTMR0_CSR = LPTMR0_CSR;                                         // clear pending interrupt
    }
    else {
        LPTMR0_CSR = 0;                                                  // clear pending interrupt and stop the timer
        POWER_DOWN_ATOMIC(5, LPTMR0);                                    // power down the timer
    }
    uDisable_Interrupt();
        LPTMR_interrupt_handler[0]();                                    // call handling function
    uEnable_Interrupt();
}
#else
static __interrupt void _LPTMR0_periodic(void)
{
    LPTMR0_CSR = LPTMR0_CSR;                                             // clear pending interrupt
    uDisable_Interrupt();
        LPTMR_interrupt_handler[0]();                                    // call handling function
    uEnable_Interrupt();
}

static __interrupt void _LPTMR0_single(void)
{
    LPTMR0_CSR = 0;                                                      // clear pending interrupt and stop the timer
    POWER_DOWN_ATOMIC(5, LPTMR0);                                        // power down the timer
    uDisable_Interrupt();
        LPTMR_interrupt_handler[0]();                                    // call handling function
    uEnable_Interrupt();
}
#endif

    #if LPTMR_AVAILABLE > 1
static __interrupt void _LPTMR1_periodic(void)
{
    LPTMR1_CSR = LPTMR1_CSR;                                             // clear pending interrupt
    uDisable_Interrupt();
        LPTMR_interrupt_handler[1]();                                    // call handling function
    uEnable_Interrupt();
}

static __interrupt void _LPTMR1_single(void)
{
    LPTMR1_CSR = 0;                                                      // clear pending interrupt and stop the timer
    POWER_DOWN_ATOMIC(5, LPTMR1);                                        // power down the timer
    uDisable_Interrupt();
        LPTMR_interrupt_handler[1]();                                    // call handling function
    uEnable_Interrupt();
}
    #endif
#endif



#if defined _LPTMR_CONFIG_CODE
        {
            LPTMR_SETUP *lptmr_setup = (LPTMR_SETUP *)ptrSettings;
            KINETIS_LPTMR_CTR *ptrLPTMR;
    #if LPTMR_AVAILABLE > 1
            if (lptmr_setup->ucTimer != 0) {
                POWER_UP_ATOMIC(5, LPTMR1);                              // ensure that the timer can be accessed
                ptrLPTMR = (KINETIS_LPTMR_CTR *)LPTMR_BLOCK_1;
            }
            else {
    #endif
                POWER_UP_ATOMIC(5, LPTMR0);                              // ensure that the timer can be accessed
                ptrLPTMR = (KINETIS_LPTMR_CTR *)LPTMR_BLOCK_0;
    #if LPTMR_AVAILABLE > 1
            }
    #endif
            ptrLPTMR->LPTMR_CSR = 0;                                     // reset the timer and ensure no pending interrupts
            if ((lptmr_setup->mode & LPTMR_STOP) != 0) {                 // stop timer
    #if LPTMR_AVAILABLE > 1
                if (lptmr_setup->ucTimer != 0) {
                    POWER_DOWN_ATOMIC(5, LPTMR1);
                }
                else {
    #endif
                    POWER_DOWN_ATOMIC(5, LPTMR0);
    #if LPTMR_AVAILABLE > 1
                }
    #endif
                return;
            }
        #if defined LPTMR_CLOCK_LPO                                      // define the low power clock speed for calculations
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_LPO | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_INTERNAL_30_40kHz
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_MCGIRCLK; | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_INTERNAL_2MHz
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock
            MCG_C1 |= MCG_C1_IRCLKEN;                                    // enable IRC
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_MCGIRCLK | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_INTERNAL_4MHz
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_MCGIRCLK | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_INTERNAL_8MHz
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock
            MCG_C1 |= MCG_C1_IRCLKEN;                                    // enable IRC
        #elif defined LPTMR_CLOCK_EXTERNAL_32kHz
            #if defined KINETIS_WITH_RTC_CRYSTAL                         // devices with RTC crystal oscillator circuity
            RTC_CR &= ~(RTC_CR_CLKO);                                    // allow RTC to supply its clock to other peripherals
            #endif
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_ERCLK32K | LPTMR_PSR_PBYP);
        #else                                                            // LPTMR_CLOCK_OSCERCLK
            OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);               // enable the external reference clock and keep it enabled in stop mode
            #if defined LPTMR_PRESCALE
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | (LPTMR_PRESCALE_VALUE << LPTMR_PSR_PRESCALE_SHIFT)); // program prescaler
            #else
            ptrLPTMR->LPTMR_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | LPTMR_PSR_PBYP);
            #endif
        #endif
        #if LPTMR_AVAILABLE > 1
            if (lptmr_setup->ucTimer != 0) {
                if ((LPTMR_interrupt_handler[1] = lptmr_setup->int_handler) != 0) { // enter the user's interrupt handler
                    if ((lptmr_setup->mode & LPTMR_PERIODIC) != 0) {     // periodic mode
            #if !defined irq_LPTMR1_ID && defined INTMUX0_AVAILABLE
                        fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_LPTMR1), INTMUX0_PERIPHERAL_LPTMR1, (void(*)(void))_LPTMR1_periodic); // enter interrupt handler
            #else
                        fnEnterInterrupt(irq_LPTMR1_ID, lptmr_setup->int_priority, (void(*)(void))_LPTMR1_periodic); // enter interrupt handler
            #endif
                    }
                    else {                                               // single-shot mode
            #if !defined irq_LPTMR1_ID && defined INTMUX0_AVAILABLE
                        fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_LPTMR1), INTMUX0_PERIPHERAL_LPTMR1, (void(*)(void))_LPTMR1_single); // enter interrupt handler
            #else
                        fnEnterInterrupt(irq_LPTMR1_ID, lptmr_setup->int_priority, (void(*)(void))_LPTMR1_single); // enter interrupt handler
            #endif
                    }
                    LPTMR1_CSR |= LPTMR_CSR_TIE;                         // enable timer interrupt
                }
            }
            else {
        #endif
                if ((LPTMR_interrupt_handler[0] = lptmr_setup->int_handler) != 0) { // enter the user's interrupt handler
        #if defined INTERRUPT_VECTORS_IN_FLASH
                    if ((lptmr_setup->mode & LPTMR_PERIODIC) != 0) {     // periodic mode
                        ucPeriodicMode = 1;
                    }
                    else {                                               // single-shot mode
                        ucPeriodicMode = 0;
                    }
                    fnEnterInterrupt(irq_LPTMR0_ID, lptmr_setup->int_priority, (void(*)(void))_LPTMR0_handler); // enter interrupt handler
        #else
                    if ((lptmr_setup->mode & LPTMR_PERIODIC) != 0) {     // periodic mode
                        fnEnterInterrupt(irq_LPTMR0_ID, lptmr_setup->int_priority, (void(*)(void))_LPTMR0_periodic); // enter interrupt handler
                    }
                    else {                                               // single-shot mode
                        fnEnterInterrupt(irq_LPTMR0_ID, lptmr_setup->int_priority, (void(*)(void))_LPTMR0_single); // enter interrupt handler
                    }
        #endif
                    LPTMR0_CSR |= LPTMR_CSR_TIE;                         // enable timer interrupt
                }
        #if LPTMR_AVAILABLE > 1
            }
        #endif
            ptrLPTMR->LPTMR_CMR = lptmr_setup->count_delay;              // set the match value
        #if !defined TRGMUX_AVAILABLE
            if ((lptmr_setup->mode & LPTMR_TRIGGER_ADC0_A) != 0) {       // if the LPTMR is to trigger ADC 0 A conversion
                SIM_SOPT7 = (SIM_SOPT7_ADC0TRGSEL_LPTMR0 | SIM_SOPT7_ADC0PRETRGSEL_A | SIM_SOPT7_ADC0ALTTRGEN);
            }
            else if ((lptmr_setup->mode & LPTMR_TRIGGER_ADC0_B) != 0) {  // if the LPTMR is to trigger ADC 0 B conversion
                SIM_SOPT7 = (SIM_SOPT7_ADC0TRGSEL_LPTMR0 | SIM_SOPT7_ADC0PRETRGSEL_B | SIM_SOPT7_ADC0ALTTRGEN);
            }
        #endif
            ptrLPTMR->LPTMR_CSR |= LPTMR_CSR_TEN;                        // enable the timer
        }
#endif
/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_PIT.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    25.02.2012 Mask PIT old mode before setting new one
    09.03.2012 Disable PIT before configuring load value so that it is retriggerable {1}
    04.02.2014 Power up PIT module after setting mode to protect from power down from another PIT channel interrupt - also protect mode variable changes from interrupts {2}
    21.04.2014 Add KL PIT support                                        {3}
    21.07.2014 Add PIT errate e2682 workaround for some older devices    {4}
    04.09.2015 Exit KL PIT interrupt if the PIT has been powered down
    22.11.2015 Move PIT power up to avoid the need to apply errate e7914 workaround {5}
    13.07.2016 Test workaround for potential race state that can cause a hard fault to occur when PIT interrupt disables module while it is being enabled by user code {6}
    22.01.2017 Enable PIT DMA support with SUPPORT_PIT_DMA_PORT_TOGGLE   {7}
    03.02.2017 Check channel's interrupt flag before clearing it (KL)    {8}
    26.09.2017 Add LPIT support                                          {9}
    28.11.2017 Add configuration of LPIT behaviour in doze and debug modes {10}
    05.01.2018 Check whether PIT module is powered up before disabling individual PIT channel {11}

*/

#if defined _PIT_CODE

#if defined LPITS_AVAILABLE                                              // {9}
    #define _PITS_AVAILABLE  LPIT_CHANNELS
    #define irq_PIT_ID       irq_LPIT0_ID
#else
    #define _PITS_AVAILABLE  PITS_AVAILABLE
#endif
/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnDisablePIT(int iPIT);

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*pit_interrupt_handler[_PITS_AVAILABLE])(void) = {0};       // user callback for each PIT
static unsigned char ucPITmodes = 0;                                     // PIT mode containing flags for all PITs

/* =================================================================== */
/*                        PIT Interrupt Handlers                       */
/* =================================================================== */


#if defined LPITS_AVAILABLE
static int fnHandleLPIT(int iChannel)
{
    WRITE_ONE_TO_CLEAR(LPIT0_MSR, (LPIT_MSR_TIF0 << iChannel));          // clear pending interrupt
    if ((ucPITmodes & (PIT_PERIODIC << (iChannel * 2))) == 0) {          // if not periodic mode (single-shot usage)
        fnDisablePIT(iChannel);                                          // stop PIT operation and power down when no other activity
    }
    uDisable_Interrupt();
        pit_interrupt_handler[iChannel]();                               // call handling function
    uEnable_Interrupt();
    if ((PCC_LPIT0 & PCC_CGC) == 0) {                                    // if the PIT module has been powered down we return rather than checking further channels
        return 1;
    }
    return 0;
}
#endif

    #if defined KINETIS_KL || defined KINETIS_KM || (defined LPITS_AVAILABLE && !defined KINETIS_KE18)
// KL device PIT, and LPIT, have a single interrupt which is shared by all PIT channels
//
static __interrupt void _PIT_Interrupt(void)
{
    int iChannel = 0;
    #if !defined LPITS_AVAILABLE
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;            // set PIT control structure pointer to first PIT channel
    #endif
    do {
    #if defined LPITS_AVAILABLE                                          // {9}
        if ((LPIT0_MIER & (LPIT_MIER_TIE0 << iChannel)) != 0) {          // interrupt is enabled on this channel
            if ((LPIT0_MSR & (LPIT_MSR_TIF0 << iChannel)) != 0) {        // interrupt is pending on this channel
                if (fnHandleLPIT(iChannel) != 0) {
                    return;                                              // no futher channels need checking so quit
                }
            }
        }
    #else
        if ((ptrCtl->PIT_TCTRL & (PIT_TCTRL_TIE | PIT_TCTRL_TEN)) == (PIT_TCTRL_TIE | PIT_TCTRL_TEN)) { // if the channel and its interrupt are enabled
            if ((ptrCtl->PIT_TFLG & PIT_TFLG_TIF) != 0) {                // {8} if this channel's interrupt has fired
                WRITE_ONE_TO_CLEAR(ptrCtl->PIT_TFLG, PIT_TFLG_TIF);      // clear pending interrupts
                if ((ucPITmodes & (PIT_PERIODIC << (iChannel * 2))) == 0) { // if not periodic mode (single-shot usage)
                    fnDisablePIT(iChannel);                              // stop PIT operation and power down when no other activity
                }
                uDisable_Interrupt();
                    pit_interrupt_handler[iChannel]();                   // call handling function
                uEnable_Interrupt();
                if (IS_POWERED_UP(6, PIT0) == 0) {                       // if the PIT module has been powered down we return rather than checking further channels
                    return;
                }
            }
        }
        ptrCtl++;                                                        // move to next PIT control structure
    #endif
    } while (++iChannel < _PITS_AVAILABLE);                              // for each PIT channel that may be interrupting
}
    #else
// Common interrupt handler for all PITs
//
    #if !defined KINETIS_KE18
static void _PIT_Handler(int iPIT)
{
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
    ptrCtl += iPIT;
    WRITE_ONE_TO_CLEAR(ptrCtl->PIT_TFLG, PIT_TFLG_TIF);                  // clear pending interrupts
        #if defined ERRATA_ID_2682                                       // {4}
    (void)(ptrCtl->PIT_TCTRL);                                           // access and PIT register after clearing the interrupt flag to allow subsequent interrupts to operate
        #endif
    if ((ucPITmodes & (PIT_PERIODIC << (iPIT * 2))) == 0) {              // if not periodic mode (single-shot usage)
        fnDisablePIT(iPIT);                                              // stop PIT operation and power down when no other activity
    }
    uDisable_Interrupt();
        pit_interrupt_handler[iPIT]();                                   // call handling function
    uEnable_Interrupt();
}
    #endif

static __interrupt void _PIT0_Interrupt(void)
{
    #if defined LPITS_AVAILABLE && defined KINETIS_KE18
    fnHandleLPIT(0);                                                     // dedicated LPIT channel 0 interrupt
    #else
    _PIT_Handler(0);
    #endif
}

static __interrupt void _PIT1_Interrupt(void)
{
    #if defined LPITS_AVAILABLE && defined KINETIS_KE18
    fnHandleLPIT(1);                                                     // dedicated LPIT channel 1 interrupt
    #else
    _PIT_Handler(1);
    #endif
}

    #if (defined PITS_AVAILABLE && (PITS_AVAILABLE > 2)) || (defined LPITS_AVAILABLE && (LPIT_CHANNELS > 2))
static __interrupt void _PIT2_Interrupt(void)
{
        #if defined LPITS_AVAILABLE && defined KINETIS_KE18
    fnHandleLPIT(2);                                                     // dedicated LPIT channel 2 interrupt
        #else
    _PIT_Handler(2);
        #endif
}

static __interrupt void _PIT3_Interrupt(void)
{
        #if defined LPITS_AVAILABLE && defined KINETIS_KE18
    fnHandleLPIT(3);                                                     // dedicated LPIT channel 3 interrupt
        #else
    _PIT_Handler(3);
        #endif
}
    #endif
#endif

// Stop PIT operation and power down when no other activity
//
static void fnDisablePIT(int iPIT)                                                  
{
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
    ptrCtl += iPIT;
  //ptrCtl->PIT_TCTRL = 0;                                               // {11} disable the individual PIT
    uDisable_Interrupt();                                                // {2} protect the mode variable during modification
        if (IS_POWERED_UP(6, PIT0) != 0) {                               // {11} ignore if the PIT module is not powered up
            ptrCtl->PIT_TCTRL = 0;                                       // disable the individual PIT
            ucPITmodes &= ~((PIT_SINGLE_SHOT | PIT_PERIODIC) << (iPIT * 2)); // clear the PIT's mode flags
    #if !defined PIT_TIMER_USED_BY_PERFORMANCE_MONITOR                   // don't power the PITs down if one is being used for performance monitoring
            if (ucPITmodes == 0) {                                       // if no PITs are in use power down the PIT module
        #if defined LPITS_AVAILABLE                                      // {9}
                LPIT0_MCR = 0;                                           // disable clocks to module since no more timers are active
                PCC_LPIT0 = 0;
        #else
                //POWER_UP_ATOMIC(6, PIT0);                              // {2}{11} ensure that the module is powered up for the next operation
                PIT_MCR = PIT_MCR_MDIS;                                  // disable clocks to module since no more timers are active
                POWER_DOWN_ATOMIC(6, PIT0);                              // power down the PIT module
        #endif
            }
    #endif
        }
    uEnable_Interrupt();
}
#endif



#if defined _PIT_CONFIG_CODE
// PIT_INTERRUPT case in fnConfigureInterrupt()
//
    {
        PIT_SETUP *PIT_settings = (PIT_SETUP *)ptrSettings;
        KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
        unsigned long ulCommand;
        if (PIT_settings->ucPIT >= _PITS_AVAILABLE) {
    #if defined _WINDOWS
            _EXCEPTION("Invalid PIT channel!");
    #endif
            return;
        }
        if ((PIT_settings->mode & PIT_STOP) != 0) {
            fnDisablePIT(PIT_settings->ucPIT);                           // stop PIT operation and power down when no other activity
            return;
        }
      //POWER_UP_ATOMIC(6, PIT0);                                        // {6}{5} ensure the PIT module is powered up
        pit_interrupt_handler[PIT_settings->ucPIT] = PIT_settings->int_handler; // enter the user handler
        uDisable_Interrupt();                                            // {2} protect the mode variable during modification
    #if defined KINETIS_WITH_PCC
            if ((PCC_LPIT0 & PCC_CGC) == 0) {                            // if presently powered down
                SELECT_PCC_PERIPHERAL_SOURCE(LPIT0, LPIT0_PCC_SOURCE);   // select the clock source
                POWER_UP_ATOMIC(0, LPIT0);                               // power up the LPIT module         
            }
    #else
            POWER_UP_ATOMIC(6, PIT0);                                    // {5}{6} ensure the PIT module is powered up
    #endif
            ucPITmodes = ((ucPITmodes & ~((PIT_SINGLE_SHOT | PIT_PERIODIC) << (PIT_settings->ucPIT * 2))) | ((PIT_settings->mode & (PIT_SINGLE_SHOT | PIT_PERIODIC)) << (PIT_settings->ucPIT * 2))); // {5} [the variable protects from power downs from this point]
        uEnable_Interrupt();
        ptrCtl += PIT_settings->ucPIT;                                   // set the PIT (channel) to be configured
      //POWER_UP_ATOMIC(6, PIT0);                                        // {2} ensure the PIT module is powered up
  //#if defined ERRATA_ID_7914
      //(void)PIT_MCR;                                                   // dummy read of PIT_MCR to guaranty a minimum delay of two bus cycles after enabling the clock gate and not losing next write
  //#endif
    #if defined LPITS_AVAILABLE                                          // {9}
        LPIT0_MCR = (LPIT_MCR_M_CEN | LPIT_CHARACTERISTICS);             // {10} ensure the PIT module is clocked (when enabled for the first time 4 clock cycles are required to allow for clock synchronisation and reset assertion)
    #else
        PIT_MCR = 0;                                                     // ensure the PIT module is clocked
    #endif
        if (PIT_settings->int_handler != 0) {                            // if an interrupt is required
    #if defined KINETIS_KL || defined KINETIS_KM || (defined LPITS_AVAILABLE && !defined KINETIS_KE18) // {3} KL, and LPIT, devices have a single interrupt from the PIT channels
            fnEnterInterrupt(irq_PIT_ID, PIT_settings->int_priority, _PIT_Interrupt); // ensure that the handler for the PIT module is entered
    #else
            switch (PIT_settings->ucPIT) {
            case 0:
                fnEnterInterrupt(irq_PIT0_ID, PIT_settings->int_priority, _PIT0_Interrupt); // ensure that the handler for this PIT is entered
                break;
            case 1:
                fnEnterInterrupt(irq_PIT1_ID, PIT_settings->int_priority, _PIT1_Interrupt); // ensure that the handler for this PIT is entered
                break;
         #if (defined PITS_AVAILABLE && (PITS_AVAILABLE > 2)) || (defined LPITS_AVAILABLE && (LPIT_CHANNELS > 2))
            case 2:
                fnEnterInterrupt(irq_PIT2_ID, PIT_settings->int_priority, _PIT2_Interrupt); // ensure that the handler for this PIT is entered
                break;
            case 3:
                fnEnterInterrupt(irq_PIT3_ID, PIT_settings->int_priority, _PIT3_Interrupt); // ensure that the handler for this PIT is entered
                break;
        #endif
            }
    #endif
    #if defined LPITS_AVAILABLE                                          // {9}
        #if defined _WINDOWS
            LPIT0_MSR &= ~(LPIT_MSR_TIF0 << PIT_settings->ucPIT);        // clear interrupt flag
        #else
            LPIT0_MSR = (LPIT_MSR_TIF0 << PIT_settings->ucPIT);          // clear interrupt flag
        #endif
            LPIT0_MIER |= (LPIT_MIER_TIE0 << PIT_settings->ucPIT);       // enable interrupt
    #else
            ulCommand = (PIT_TCTRL_TEN | PIT_TCTRL_TIE);                 // PIT to be enabled with interrupt
    #endif
        }
        else {
    #if defined LPITS_AVAILABLE
            LPIT0_MIER &= ~(LPIT_MIER_TIE0 << PIT_settings->ucPIT);      // disable interrupt
    #else
            ulCommand = (PIT_TCTRL_TEN);                                 // no interrupt used (enable without interrupt)
    #endif
        }
        if ((PIT_settings->mode & (PIT_SINGLE_SHOT | PIT_RETRIGGER)) != 0) { // single shot always behaves as retriggerable - periodic change at next timeout by default but can be forced with PIT_RETRIGGER
            ptrCtl->PIT_TCTRL = 0;                                       // {1} disable PIT so that it can be retriggered if needed
        }
        ptrCtl->PIT_LDVAL = PIT_settings->count_delay;                   // load interval value
    #if defined LPITS_AVAILABLE                                          // {9}
        if ((PIT_settings->mode & PIT_SINGLE_SHOT) != 0) {
            ulCommand = (LPIT_TCTRL_MODE_32 | LPIT_TCTRL_TSOI | LPIT_TCTRL_T_EN); // enable timer and stop timer on interrupt
        }
        else {
            ulCommand = (LPIT_TCTRL_MODE_32 | LPIT_TCTRL_T_EN);          // enable timer
        }
    #else
        WRITE_ONE_TO_CLEAR(ptrCtl->PIT_TFLG, PIT_TFLG_TIF);              // clear possible pending interrupt
        if ((PIT_settings->mode & PIT_TRIGGER_ADC0_A) != 0) {            // if the PIT output TIF is to trigger ADC0 conversion on channel A
        #if defined KINETIS_KE
            SIM_SOPT0 |= SIM_SOPT_ADHWT_PIT0;                            // trigger ADC on PIT0 overflow
        #else
            SIM_SOPT7 = ((SIM_SOPT7_ADC0TRGSEL_PIT0 + PIT_settings->ucPIT) | SIM_SOPT7_ADC0PRETRGSEL_A | SIM_SOPT7_ADC0ALTTRGEN); // trigger ADC0 channel A
        #endif
        }
        else if ((PIT_settings->mode & PIT_TRIGGER_ADC0_B) != 0) {       // if the PIT output TIF is to trigger ADC0 conversion on channel B
        #if defined KINETIS_KE
            SIM_SOPT0 |= SIM_SOPT_ADHWT_PIT0;                            // trigger ADC on PIT0 overflow
        #else
            SIM_SOPT7 = ((SIM_SOPT7_ADC0TRGSEL_PIT0 + PIT_settings->ucPIT) | SIM_SOPT7_ADC0PRETRGSEL_B | SIM_SOPT7_ADC0ALTTRGEN); // trigger ADC0 channel B
        #endif
        }
        #if !defined DEVICE_WITHOUT_DMA && defined SUPPORT_PIT_DMA_PORT_TOGGLE // {7}
        if ((PIT_settings->mode & PIT_OUTPUT_DMA_TRIG) != 0) {           // if the PIT is to trigger a port toggle by DMA
            // Note that PIT channel number must match with the DMA channel (hardware requirement) and the DMA channel is chosen here to match the PIT
            //
            static unsigned long ulPortBits[_PITS_AVAILABLE] = {0};
            ulPortBits[PIT_settings->ucPIT] = PIT_settings->ulPortBits;  // the port bit(s) to be toggled
            fnConfigDMA_buffer(PIT_settings->ucPIT, (DMAMUX0_DMA0_CHCFG_SOURCE_PIT0 + PIT_settings->ucPIT), sizeof(ulPortBits[PIT_settings->ucPIT]), &ulPortBits[PIT_settings->ucPIT], (unsigned long *)(GPIO_BLOCK + (0x040 * PIT_settings->ucPortRef) + 0x00c), (DMA_DIRECTION_OUTPUT | DMA_LONG_WORDS | DMA_FIXED_ADDRESSES | DMA_NO_MODULO), 0, 0); // source is the port bit and destination is the GPIO toggle register (DMA_FIXED_ADDRESSES and DMA_NO_MODULO are used only by KL parts)
            fnDMA_BufferReset(PIT_settings->ucPIT, DMA_BUFFER_START);
        }
        #endif
    #endif
        ptrCtl->PIT_TCTRL = ulCommand;                                   // start PIT with interrupt enabled, when handler defined
    #if defined _WINDOWS
        ptrCtl->PIT_CVAL = ptrCtl->PIT_LDVAL;                            // load current count value with the load value
    #endif
    }
#endif

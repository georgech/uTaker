/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      iMX_PORTS.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

    See the following video showing port interrupt operation on a KL27: https://youtu.be/CubinvMuTwU

*/

#if defined _PORT_MUX_CODE
// This routine is used to connect one or more pins to their GPIO function with defined characteristics (it also powers the port in question)
//
extern void fnConnectGPIO(int iPortRef, unsigned long ulPortBits, unsigned long ulCharacteristics)
{
    register unsigned long ulMask;
    unsigned long ulBit = 0x00000001;
    unsigned long *ptrGPIO;

    switch (iPortRef) {
    case PORT1:
        ptrGPIO = IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_00_ADD;
        CCM_CCGR1 &= ~(CCM_CCGR1_GPIO1_CLOCKS_MASK);
        CCM_CCGR1 |= (CCM_CCGR1_GPIO1_CLOCKS_STOP);
        //CCM_CCGR1  |= (CCM_CCGR1_GPIO1_CLOCKS_RUN);
        break;
    case PORT2:
        ptrGPIO = IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_00_ADD;
        CCM_CCGR0 &= ~(CCM_CCGR0_GPIO2_CLOCKS_MASK);
        CCM_CCGR0 |= (CCM_CCGR0_GPIO2_CLOCKS_STOP);
      //CCM_CCGR0  |= (CCM_CCGR0_GPIO2_CLOCKS_RUN);
        break;
    case PORT3:
        ptrGPIO = IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_32_ADD;
        CCM_CCGR2 &= ~(CCM_CCGR2_GPIO3_CLOCKS_MASK);
        CCM_CCGR2 |= (CCM_CCGR2_GPIO3_CLOCKS_STOP);
        //CCM_CCGR2  |= (CCM_CCGR2_GPIO3_CLOCKS_RUN);
        break;
    case PORT5:
        ptrGPIO = IOMUXC_SNVS_SW_MUX_CTL_PAD_WAKEUP_ADD;
        break;
    default:
        _EXCEPTION("The port that is being accessed is not available on this processor!!");
        return;
    }

    // Complete set of GPIO pins as reference
    //
    /*
    _CONFIG_PERIPHERAL(GPIO_AD_B0_00, GPIO1_IO00, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_01, GPIO1_IO01, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_02, GPIO1_IO02, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_03, GPIO1_IO03, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_04, GPIO1_IO04, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_05, GPIO1_IO05, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_06, GPIO1_IO06, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_07, GPIO1_IO07, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_08, GPIO1_IO08, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_09, GPIO1_IO09, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_10, GPIO1_IO10, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_11, GPIO1_IO11, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_12, GPIO1_IO12, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_13, GPIO1_IO13, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_14, GPIO1_IO14, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B0_15, GPIO1_IO15, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_00, GPIO1_IO16, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_01, GPIO1_IO17, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_02, GPIO1_IO18, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_03, GPIO1_IO19, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_04, GPIO1_IO20, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_05, GPIO1_IO21, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_06, GPIO1_IO22, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_07, GPIO1_IO23, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_08, GPIO1_IO24, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_09, GPIO1_IO25, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_10, GPIO1_IO26, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_11, GPIO1_IO27, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_12, GPIO1_IO28, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_13, GPIO1_IO29, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_14, GPIO1_IO30, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_AD_B1_15, GPIO1_IO31, UART_PULL_UPS);

    _CONFIG_PERIPHERAL(GPIO_EMC_00, GPIO2_IO00, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_01, GPIO2_IO01, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_02, GPIO2_IO02, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_03, GPIO2_IO03, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_04, GPIO2_IO04, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_05, GPIO2_IO05, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_06, GPIO2_IO06, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_07, GPIO2_IO07, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_08, GPIO2_IO08, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_09, GPIO2_IO09, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_10, GPIO2_IO10, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_11, GPIO2_IO11, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_12, GPIO2_IO12, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_13, GPIO2_IO13, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_14, GPIO2_IO14, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_15, GPIO2_IO15, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_16, GPIO2_IO16, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_17, GPIO2_IO17, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_18, GPIO2_IO18, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_19, GPIO2_IO19, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_20, GPIO2_IO20, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_21, GPIO2_IO21, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_22, GPIO2_IO22, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_23, GPIO2_IO23, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_24, GPIO2_IO24, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_25, GPIO2_IO25, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_26, GPIO2_IO26, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_27, GPIO2_IO27, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_28, GPIO2_IO28, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_29, GPIO2_IO29, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_30, GPIO2_IO30, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_31, GPIO2_IO31, UART_PULL_UPS);

    _CONFIG_PERIPHERAL(GPIO_EMC_32, GPIO3_IO00, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_33, GPIO3_IO01, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_34, GPIO3_IO02, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_35, GPIO3_IO03, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_36, GPIO3_IO04, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_37, GPIO3_IO05, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_38, GPIO3_IO06, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_39, GPIO3_IO07, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_40, GPIO3_IO08, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_EMC_41, GPIO3_IO09, UART_PULL_UPS);

    _CONFIG_PERIPHERAL(GPIO_SD_B0_00, GPIO3_IO13, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B0_01, GPIO3_IO14, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B0_02, GPIO3_IO15, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B0_03, GPIO3_IO16, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B0_04, GPIO3_IO17, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B0_05, GPIO3_IO18, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B0_06, GPIO3_IO19, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_00, GPIO3_IO20, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_01, GPIO3_IO21, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_02, GPIO3_IO22, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_03, GPIO3_IO23, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_04, GPIO3_IO24, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_05, GPIO3_IO25, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_06, GPIO3_IO26, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_07, GPIO3_IO27, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_08, GPIO3_IO28, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_09, GPIO3_IO29, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_10, GPIO3_IO30, UART_PULL_UPS);
    _CONFIG_PERIPHERAL(GPIO_SD_B1_11, GPIO3_IO31, UART_PULL_UPS);

    // Exceptions
    //
    IOMUXC_SNVS_SW_MUX_CTL_PAD_WAKEUP = PAD_WAKEUP_GPIO5_IO00;
    IOMUXC_SNVS_SW_MUX_CTL_PAD_PMIC_ON_REQ = PAD_PMIC_ON_REQ_GPIO5_IO01:
    IOMUXC_SNVS_SW_MUX_CTL_PAD_PMIC_STBY_REQ = PAD_PMIC_STBY_REQ_GPIO5_IO02
    */



    if ((PORT_PSEUDO_FLAG_SET_ONLY_PULLS & ulCharacteristics) != 0) {    // don't allow the multiplexer setting to be changed so that peripheral function setting is not modified
        ulCharacteristics &= (PORT_IRQC_INT_MASK | PORT_LOCK | PORT_PS_UP_ENABLE); // allow only these field to be set
        ulMask = (PORT_IRQC_INT_MASK | PORT_PS_UP_ENABLE);               // allow only these fields to be reset
    }
    else {
        ulMask = (PORT_IRQC_INT_MASK | PORT_MUX_MASK | PORT_DSE_HIGH | PORT_ODE | PORT_PFE | PORT_SRE_SLOW | PORT_PS_UP_ENABLE); // {7} allow all fields to be modified (set or reset)
    }

    while (ulPortBits != 0) {                                            // for each specified pin
        if ((ulPortBits & ulBit) != 0) {                                 // if this port bit is to be set as GPIO
            *ptrGPIO = PAD_MUX_MODE_GPIO;                                // set the GPIO mode for this pin
          //*ptrGPIO = PAD_MUX_MODE_GPIO;                                // set the GPIO characteristics for each port pin
            ulPortBits &= ~(ulBit);                                      // pin has been set
        }
        ptrGPIO++;
        ulBit <<= 1;
    }
    _SIM_PER_CHANGE;
}
#endif

#if defined _PORT_INTERRUPT_CODE
    #if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18 // KE uses external interrupt
    #else
    #if !defined NO_PORT_INTERRUPTS_PORT1                                // if port 1 support has not been removed
        #if defined PORT_INTERRUPT_USER_DISPATCHER
static void (*gpio_handlers_1)(int) = 0;                                 // a single handler for port 1
        #else
static void (*gpio_handlers_1[PORT_WIDTH])(void) = {0};                  // a handler for each possible port 1 pin
        #endif
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORT2                                // if port 2 support has not been removed
        #if defined PORT_INTERRUPT_USER_DISPATCHER
static void (*gpio_handlers_2)(int) = 0;                                 // a single handler for port 2
        #else
static void (*gpio_handlers_2[PORT_WIDTH])(void) = {0};                  // a handler for each possible port 2 pin
        #endif
    #endif
    #if (PORTS_AVAILABLE > 2) &&  !defined NO_PORT_INTERRUPTS_PORT3     // if port 3 support has not been removed
        #if defined PORT_INTERRUPT_USER_DISPATCHER
static void (*gpio_handlers_3)(int) = 0;                                 // a single handler for port 3
        #else
static void (*gpio_handlers_3[PORT_WIDTH])(void) = {0};                  // a handler for each possible port 3 pin
        #endif
    #endif
    #if (PORTS_AVAILABLE > 4) && !defined NO_PORT_INTERRUPTS_PORT5       // if port 5 support has not been removed
        #if defined PORT_INTERRUPT_USER_DISPATCHER
static void (*gpio_handlers_5)(int) = 0;                                 // a single handler for port 5
        #else
static void (*gpio_handlers_5[PORT_WIDTH])(void) = {0};                  // a handler for each possible port 5 pin
        #endif
    #endif

    #if !defined NO_PORT_INTERRUPTS_PORT1                                // if port 1 support has not been removed
// Interrupt routine called to handle port 1 input interrupt on bit 0
//
static __interrupt void _port_1_0_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_0);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(0);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[0]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_0_INT_MASK) < PORT_ICR1_0_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_0);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 1
//
static __interrupt void _port_1_1_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_1);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(1);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[1]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_1_INT_MASK) < PORT_ICR1_1_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_1);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 2
//
static __interrupt void _port_1_2_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_2);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(2);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[2]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_2_INT_MASK) < PORT_ICR1_2_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_2);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 3
//
static __interrupt void _port_1_3_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_3);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(3);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[3]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_3_INT_MASK) < PORT_ICR1_3_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_3);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 4
//
static __interrupt void _port_1_4_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_4);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(4);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[4]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_4_INT_MASK) < PORT_ICR1_4_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_4);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 5
//
static __interrupt void _port_1_5_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_5);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(5);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[5]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_5_INT_MASK) < PORT_ICR1_5_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_5);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 6
//
static __interrupt void _port_1_6_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_6);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(6);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[6]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_6_INT_MASK) < PORT_ICR1_6_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_6);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupt on bit 7
//
static __interrupt void _port_1_7_isr(void)
{
    WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_7);                       // reset the edge sensitive interrupt that will be handled
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when user callback operates
#if defined PORT_INTERRUPT_USER_DISPATCHER
        gpio_handlers_1(7);                                              // call the application handler (this is expected to clear level sensitive input sources)
#else
        gpio_handlers_1[7]();                                            // call the application handler (this is expected to clear level sensitive input sources)
#endif
        if ((GPIO1_ICR1 & PORT_ICR1_7_INT_MASK) < PORT_ICR1_7_RISING) {  // if level sensitive type
            WRITE_ONE_TO_CLEAR(GPIO1_ISR, GPIO_ISR_ISR_7);               // attempt to clear the level sensitive interrupt "after" the user has serviced it
        }
    uEnable_Interrupt();
}

// Interrupt routine called to handle port 1 input interrupts (dedicated to port 1 bits 8..15)
//
static __interrupt void _port_1_low_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO1_ISR & 0x0000ff00)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO1_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000100;
        iInterrupt = 8;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                         // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_1(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_1[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO1_ICR1 >> (iInterrupt * 2)) & PORT_ICR1_0_INT_MASK) < PORT_ICR1_0_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO1_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
        #if defined _WINDOWS
                    GPIO1_ISR &= ~ulPortBit;
        #endif
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}

// Interrupt routine called to handle port 1 input interrupts (dedicated to port 1 bits 16..31)
//
static __interrupt void _port_1_high_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO1_ISR & 0xffff0000)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO1_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00010000;
        iInterrupt = 16;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                         // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_1(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_1[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO1_ICR2 >> (iInterrupt * 2)) & PORT_ICR2_16_INT_MASK) < PORT_ICR2_16_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO1_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORT2                                // if port 2 support has not been removed
// Interrupt routine called to handle port 2 input interrupts (dedicated to port 2 bits 0..15)
//
static __interrupt void _port_2_low_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO2_ISR & 0x0000ffff)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO2_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                          // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_2(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_2[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO2_ICR1 >> (iInterrupt * 2)) & PORT_ICR1_0_INT_MASK) < PORT_ICR1_0_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO2_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}

// Interrupt routine called to handle port 2 input interrupts (dedicated to port 2 bits 16..31)
//
static __interrupt void _port_2_high_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO2_ISR & 0xffff0000)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO2_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00010000;
        iInterrupt = 16;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                         // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_2(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_2[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO2_ICR2 >> (iInterrupt * 2)) & PORT_ICR2_16_INT_MASK) < PORT_ICR2_16_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO2_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif
    #if (PORTS_AVAILABLE > 2) && !defined NO_PORT_INTERRUPTS_PORT3       // if port 3 support has not been removed
// Interrupt routine called to handle port 3 input interrupts (dedicated to port 3 bits 0..15)
//
static __interrupt void _port_3_low_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO3_ISR & 0x0000ffff)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO3_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                          // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_3(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_3[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO3_ICR1 >> (iInterrupt * 2)) & PORT_ICR1_0_INT_MASK) < PORT_ICR1_0_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO3_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}

// Interrupt routine called to handle port 3 input interrupts (dedicated to port 3 bits 16..31)
//
static __interrupt void _port_3_high_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO3_ISR & 0xffff0000)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO3_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00010000;
        iInterrupt = 16;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                         // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_3(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_3[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO3_ICR2 >> (iInterrupt * 2)) & PORT_ICR2_16_INT_MASK) < PORT_ICR2_16_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO3_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif
    #if (PORTS_AVAILABLE > 4) && !defined NO_PORT_INTERRUPTS_PORT5       // if port 5 support has not been removed
// Interrupt routine called to handle port 5 input interrupts (dedicated to port 5 bits 0..15)
//
static __interrupt void _port_5_low_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO5_ISR & 0x0000ffff)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO5_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00000001;
        iInterrupt = 0;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                          // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_5(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_5[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO5_ICR1 >> (iInterrupt * 2)) & PORT_ICR1_0_INT_MASK) < PORT_ICR1_0_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO5_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}

// Interrupt routine called to handle port 5 input interrupts (dedicated to port 5 bits 16..31)
//
static __interrupt void _port_5_high_isr(void)
{
    unsigned long ulSources;
    unsigned long ulPortBit;
    int iInterrupt;
    while ((ulSources = (GPIO5_ISR & 0xffff0000)) != 0) {                // read which pins are generating interrupts
        WRITE_ONE_TO_CLEAR(GPIO5_ISR, ulSources);                        // reset the edge sensitive ones that will be handled
        ulPortBit = 0x00010000;
        iInterrupt = 16;
        while (ulSources != 0) {
            if ((ulSources & ulPortBit) != 0) {                         // pending interrupt detected on this input
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                    gpio_handlers_5(iInterrupt);                         // call the application handler (this is expected to clear level sensitive input sources)
        #else
                    gpio_handlers_5[iInterrupt]();                       // call the application handler (this is expected to clear level sensitive input sources)
        #endif
                    if (((GPIO5_ICR2 >> (iInterrupt * 2)) & PORT_ICR2_16_INT_MASK) < PORT_ICR2_16_RISING) { // if level sensitive type
                        WRITE_ONE_TO_CLEAR(GPIO3_ISR, ulPortBit);        // attempt to clear the level sensitive interrupt "after" the user has serviced it
                    }
                    ulSources &= ~ulPortBit;                             // clear the processed port pin interrupt
                uEnable_Interrupt();
            }
            ulPortBit <<= 1;
            iInterrupt++;
        }
    }
}
    #endif

static void fnProgramInterruptSensitivity(iMX_GPIO *ptrGPIO, unsigned long ulChars, unsigned long ulBit, int port_bit)
{
    if ((ulChars & PORT_ICR_DISABLE) != 0) {
        ptrGPIO->GPIO_IMR &= ~(ulBit);                                   // disable the pin interrupt
    }
    else {
        // Program the interrupt sensitivity
        //
        if ((PORT_ICR_BOTH & ulChars) != 0) {
            ptrGPIO->GPIO_EDGE_SEL |= (GPIO_EDGE_SEL_0 << port_bit);
        }
        else {
            ptrGPIO->GPIO_EDGE_SEL &= ~(GPIO_EDGE_SEL_0 << port_bit);
            if (port_bit >= 16) {
                ptrGPIO->GPIO_ICR2 &= ~(PORT_ICR2_16_INT_MASK << (port_bit - 16));
                ptrGPIO->GPIO_ICR2 |= ((ulChars & PORT_ICR2_16_INT_MASK) << (port_bit - 16));
            }
            else {
                ptrGPIO->GPIO_ICR1 &= ~(PORT_ICR1_0_INT_MASK << port_bit);
                ptrGPIO->GPIO_ICR1 |= ((ulChars & PORT_ICR1_0_INT_MASK) << port_bit);
            }
        }
        WRITE_ONE_TO_CLEAR(ptrGPIO->GPIO_ISR, ulBit);                    // clear possible pending interrupt
        ptrGPIO->GPIO_IMR |= (ulBit);                                    // enable the pin interrupt
    }
}

// This routine enters the user handler for a port interrupt. The handler can be assigned to multiple inputs
//
static void fnEnterPortInterruptHandler(INTERRUPT_SETUP *port_interrupt, unsigned long ulChars)
{
    register unsigned long ulPortBits = port_interrupt->int_port_bits;
    register int ucPortRef = (int)(port_interrupt->int_port);
    unsigned long ulBit = 0x00000001;
    int port_bit = 0;
    while (ulPortBits != 0) {                                            // for each enabled port bit
        if ((ulPortBits & ulBit) != 0) {                                 // if this port bit is enabled
            switch (ucPortRef) {                                         // switch on the port
    #if !defined NO_PORT_INTERRUPTS_PORT1                                // if port 1 support has not been removed
            case PORT1:                                                  // port 1
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                if ((gpio_handlers_1 = port_interrupt->int_handler) != 0)// enter the application handler
        #else
                if ((gpio_handlers_1[port_bit] = port_interrupt->int_handler) != 0) // {enter the application handler
        #endif
                {
                    switch (ulBit) {
                    case PORT1_BIT0:
                        fnEnterInterrupt(irq_GPIO1_Hi_0_ID, port_interrupt->int_priority, _port_1_0_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT1:
                        fnEnterInterrupt(irq_GPIO1_Hi_1_ID, port_interrupt->int_priority, _port_1_1_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT2:
                        fnEnterInterrupt(irq_GPIO1_Hi_2_ID, port_interrupt->int_priority, _port_1_2_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT3:
                        fnEnterInterrupt(irq_GPIO1_Hi_3_ID, port_interrupt->int_priority, _port_1_3_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT4:
                        fnEnterInterrupt(irq_GPIO1_Hi_4_ID, port_interrupt->int_priority, _port_1_4_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT5:
                        fnEnterInterrupt(irq_GPIO1_Hi_5_ID, port_interrupt->int_priority, _port_1_5_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT6:
                        fnEnterInterrupt(irq_GPIO1_Hi_6_ID, port_interrupt->int_priority, _port_1_6_isr); // ensure that the handler for this pin is entered
                        break;
                    case PORT1_BIT7:
                        fnEnterInterrupt(irq_GPIO1_Hi_7_ID, port_interrupt->int_priority, _port_1_7_isr); // ensure that the handler for this pin is entered
                        break;
                    default:
                        if (ulBit >= 0x00010000) {
                            fnEnterInterrupt(irq_GPIO1_16_31_ID, port_interrupt->int_priority, _port_1_high_isr); // ensure that the handler for this port is entered
                        }
                        else {
                            fnEnterInterrupt(irq_GPIO1_0_15_ID, port_interrupt->int_priority, _port_1_low_isr); // ensure that the handler for this port is entered
                        }
                    }
                }
                else {
                    ulChars |= PORT_ICR_DISABLE;
                }
                fnProgramInterruptSensitivity((iMX_GPIO *)GPIO1_BLOCK, ulChars, ulBit, port_bit);
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) == 0) { // if the interrupt is being added to a peripheral function
                    _CONFIG_PORT_INPUT(1, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORT2                                // if port 2 support has not been removed
            case PORT2:                                                  // port 2
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                if ((gpio_handlers_2 = port_interrupt->int_handler) != 0)// enter the application handler
        #else
                if ((gpio_handlers_2[port_bit] = port_interrupt->int_handler) != 0) // {enter the application handler
        #endif
                {
                    if (ulBit >= 0x00010000) {
                        fnEnterInterrupt(irq_GPIO2_16_31_ID, port_interrupt->int_priority, _port_2_high_isr); // ensure that the handler for this port is entered
                    }
                    else {
                        fnEnterInterrupt(irq_GPIO2_0_15_ID, port_interrupt->int_priority, _port_2_low_isr); // ensure that the handler for this port is entered
                    }
                }
                else {
                    ulChars |= PORT_ICR_DISABLE;
                }
                fnProgramInterruptSensitivity((iMX_GPIO *)GPIO2_BLOCK, ulChars, ulBit, port_bit);
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) == 0) { // if the interrupt is being added to a peripheral function
                    _CONFIG_PORT_INPUT(2, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORT3                                // if port 3 support has not been removed
            case PORT3:                                                  // port 3
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                if ((gpio_handlers_3 = port_interrupt->int_handler) != 0)// enter the application handler
        #else
                if ((gpio_handlers_3[port_bit] = port_interrupt->int_handler) != 0) // {enter the application handler
        #endif
                {
                    if (ulBit >= 0x00010000) {
                        fnEnterInterrupt(irq_GPIO3_16_31_ID, port_interrupt->int_priority, _port_3_high_isr); // ensure that the handler for this port is entered
                    }
                    else {
                        fnEnterInterrupt(irq_GPIO3_0_15_ID, port_interrupt->int_priority, _port_3_low_isr); // ensure that the handler for this port is entered
                    }
                }
                else {
                    ulChars |= PORT_ICR_DISABLE;
                }
                fnProgramInterruptSensitivity((iMX_GPIO *)GPIO3_BLOCK, ulChars, ulBit, port_bit);
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) == 0) { // if the interrupt is being added to a peripheral function
                    _CONFIG_PORT_INPUT(3, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
    #if !defined NO_PORT_INTERRUPTS_PORT5                                // if port 5 support has not been removed
            case PORT5:                                                  // port 5
        #if defined PORT_INTERRUPT_USER_DISPATCHER
                if ((gpio_handlers_5 = port_interrupt->int_handler) != 0)// enter the application handler
        #else
                if ((gpio_handlers_5[port_bit] = port_interrupt->int_handler) != 0) // {enter the application handler
        #endif
                {
                    if (ulBit >= 0x00010000) {
                        fnEnterInterrupt(irq_GPIO5_16_31_ID, port_interrupt->int_priority, _port_5_high_isr); // ensure that the handler for this port is entered
                    }
                    else {
                        fnEnterInterrupt(irq_GPIO5_0_15_ID, port_interrupt->int_priority, _port_5_low_isr); // ensure that the handler for this port is entered
                    }
                }
                else {
                    ulChars |= PORT_ICR_DISABLE;
                }
                fnProgramInterruptSensitivity((iMX_GPIO *)GPIO5_BLOCK, ulChars, ulBit, port_bit);
                if ((port_interrupt->int_port_sense & PORT_KEEP_PERIPHERAL) == 0) { // if the interrupt is being added to a peripheral function
                    _CONFIG_PORT_INPUT(5, ulBit, ulChars);               // configure the port bit as input with the required interrupt sensitivity and characteristics
                }
                break;
    #endif
            default:
                _EXCEPTION("Warning - port for interrupt not enabled (check also that this port interrupt is possible on the particular device being used)!!");
                return;
            }
            ulPortBits &= ~ulBit;
        }
        ulBit <<= 1;
        port_bit++;
    }
    _SIM_PORT_CHANGE;
}
    #endif
#endif




#if defined _PORT_INT_CONFIG_CODE
        {
            INTERRUPT_SETUP *port_interrupt = (INTERRUPT_SETUP *)ptrSettings;
            unsigned long ulCharacteristics = PORT_ICR_DISABLE;          // default is to disable interrupts
            if ((port_interrupt->int_port_sense & IRQ_DISABLE_INT) == 0) { // if the interrupt is not being disabled
                if ((port_interrupt->int_port_sense & IRQ_LOW_LEVEL) != 0) {
                    ulCharacteristics = PORT_ICR_LOW_LEVEL;
                }
                else if ((port_interrupt->int_port_sense & IRQ_HIGH_LEVEL) != 0) {
                    ulCharacteristics = PORT_ICR_HIGH_LEVEL;
                }
                else if ((port_interrupt->int_port_sense & IRQ_RISING_EDGE) != 0) {
                    if ((port_interrupt->int_port_sense & IRQ_FALLING_EDGE) != 0) {
                        ulCharacteristics = PORT_ICR_BOTH;
                    }
                    else {
                        ulCharacteristics = PORT_ICR_RISING;
                    }
                }
                else if ((port_interrupt->int_port_sense & IRQ_FALLING_EDGE) != 0) {
                    ulCharacteristics = PORT_ICR_FALLING;
                }
            }
            if ((port_interrupt->int_port_sense & PULLUP_ON) != 0) {
                ulCharacteristics |= (PORT_ICR_PULL_UP_ENABLE);          // enable pull-up resistor on the input
            }
            else if ((port_interrupt->int_port_sense & PULLDOWN_ON) != 0) {
                ulCharacteristics |= (PORT_ICR_PULL_DOWN_ENABLE);          // enable pull-down resistor on the input
            }
            fnEnterPortInterruptHandler(port_interrupt, ulCharacteristics); // configure the interrupt and port bits according to the interrupt requirements
        }
#endif


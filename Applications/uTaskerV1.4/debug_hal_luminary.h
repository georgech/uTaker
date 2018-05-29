/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      debug_hal_luminary.h
    Project:   uTasker project
               - hardware application layer for Kinetis
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    This file includes a number of hardware specific routines that are
    included in debug.c. This include file allows the main content of debug.c
    to be hardware independent and thus shared by all processor projects. 
    In addition it makes adding specific setups for different processor boards
    possible by exchanging this file or by including multiple sets of the
    routines in it, controlled by defines of each target type

*/


#if defined USE_MAINTENANCE
#if !defined REMOVE_PORT_INITIALISATIONS
// Check the present state of a particular port
//
extern int fnPortState(CHAR cPortBit)
{
    unsigned char ucBit = 0x01;
    cPortBit -= '1';
    ucBit <<= cPortBit;
    if (DEMO_LED_PORT & ucBit) {
        return 1;
    }
    return 0;
}

// Check whether the port is configured as an input or as output
//
extern int fnPortInputConfig(CHAR cPortBit)
{
    unsigned char ucBit = 0x01;
    cPortBit -= '1';
    ucBit <<= cPortBit;
    if (DEMO_LED_DDR & ucBit) {
        return 0;
    }
    return 1;
}

// Configure a processor port pin as input/output or analogue
//
extern int fnConfigPort(CHAR cPortBit, CHAR cType)
{
    cPortBit -= '0';                                                     // select bit offset

    switch (cType) {
    case 'd':                                                            // default use for 0..4
    case 'i':                                                            // port to be input
    #if defined EK_LM3S3748 && (defined GLCD_COLOR || defined CGLCD_GLCD_MODE) 
        if (cPortBit == 1) {                                             // don't disturn CGLCD backlight
            break;
        }
    #endif
        if (cPortBit > 3) {                                              // 4 user ports available
            break;
        }
        DEMO_LED_DDR &= ~(1 << cPortBit);                                // set port bit to input
        DEMO_DIGITAL_ENABLE |= (1 << cPortBit);                          // connect for digital use
        temp_pars->temp_parameters.ucUserOutputs = (unsigned char)DEMO_LED_DDR;
        break;

    case 'o':                                                            // port to be output
        if (cPortBit > 3) {                                              // 4 user ports available
            break;
        }
        DEMO_LED_DDR |= (1 << cPortBit);                                 // set port bit to output
        DEMO_DIGITAL_ENABLE |= (1 << cPortBit);                          // connect for digital use
        temp_pars->temp_parameters.ucUserOutputs = (unsigned char)DEMO_LED_DDR;
        break;

    case 'a':                                                            // port to be analog
        break;

    default:
        return -1;
    }
    return 0;
}


// Request present logic level of an output
//
extern int fnUserPortState(CHAR cPortBit)
{
    switch (cPortBit) {
    case 'a':
        return ((USER_PORT_1 & USER_PORT_1_BIT) != 0);
    case 'b':
        return ((USER_PORT_2 & USER_PORT_2_BIT) != 0);
    case 'c':
        return ((USER_PORT_3 & USER_PORT_3_BIT) != 0);
    case 'd':
        return ((USER_PORT_4 & USER_PORT_4_BIT) != 0);
    case 'e':
        return ((USER_PORT_5 & USER_PORT_5_BIT) != 0);
    case 'f':
        return ((USER_PORT_6 & USER_PORT_6_BIT) != 0);
    case 'g':
        return ((USER_PORT_7 & USER_PORT_7_BIT) != 0);
    case 'h':
        return ((USER_PORT_8 & USER_PORT_8_BIT) != 0);
    case 'i':
        return ((USER_PORT_9 & USER_PORT_9_BIT) != 0);
    case 'j':
        return ((USER_PORT_10 & USER_PORT_10_BIT) != 0);
    case 'k':
        return ((USER_PORT_11 & USER_PORT_11_BIT) != 0);
    case 'l':
        return ((USER_PORT_12 & USER_PORT_12_BIT) != 0);
    case 'm':
        return ((USER_PORT_13 & USER_PORT_13_BIT) != 0);
    case 'n':
        return ((USER_PORT_14 & USER_PORT_14_BIT) != 0);
    case 'o':
        return ((USER_PORT_15 & USER_PORT_15_BIT) != 0);
    case 'p':
        return ((USER_PORT_16 & USER_PORT_16_BIT) != 0);
    default:
        break;
    }
    return 0;
}

// Set a single output to logical '1' or '0' state
//
static void fnSetPortBit(unsigned short usBit, int iSetClr)
{
#if 0
    if (iSetClr != 0) {
        switch (usBit) {
        case 0x0001:
            USER_SET_PORT_1 = USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_SET_PORT_2 = USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_SET_PORT_3 = USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_SET_PORT_4 = USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_SET_PORT_5 = USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_SET_PORT_6 = USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_SET_PORT_7 = USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_SET_PORT_8 = USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_SET_PORT_9 = USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_SET_PORT_10 = USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_SET_PORT_11 = USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_SET_PORT_12 = USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_SET_PORT_13 = USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_SET_PORT_14 = USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_SET_PORT_15 = USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_SET_PORT_16 = USER_PORT_16_BIT;
            break;
        }
    }
    else {
        switch (usBit) {
        case 0x0001:
            USER_CLEAR_PORT_1 = USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_CLEAR_PORT_2 = USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_CLEAR_PORT_3 = USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_CLEAR_PORT_4 = USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_CLEAR_PORT_5 = USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_CLEAR_PORT_6 = USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_CLEAR_PORT_7 = USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_CLEAR_PORT_8 = USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_CLEAR_PORT_9 = USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_CLEAR_PORT_10 = USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_CLEAR_PORT_11 = USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_CLEAR_PORT_12 = USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_CLEAR_PORT_13 = USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_CLEAR_PORT_14 = USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_CLEAR_PORT_15 = USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_CLEAR_PORT_16 = USER_PORT_16_BIT;
            break;
        }
    }                                                                    _SIM_PORTS; // ensure the simulator updates its port states
#endif
}


// Configure port bit as output (a..p are port bit references)
//
static int fnConfigOutputPort(CHAR cPortBit)
{
    switch (cPortBit) {
    case 'a':
        ENABLE_USER_PORT_1();
        USER_DDR_1 |= USER_PORT_1_BIT;
        break;
    case 'b':
        ENABLE_USER_PORT_2();
        USER_DDR_2 |= USER_PORT_2_BIT;
        break;
    case 'c':
        ENABLE_USER_PORT_3();
        USER_DDR_3 |= USER_PORT_3_BIT;
        break;
    case 'd':
        ENABLE_USER_PORT_4();
        USER_DDR_4 |= USER_PORT_4_BIT;
        break;
    case 'e':
        ENABLE_USER_PORT_5();
        USER_DDR_5 |= USER_PORT_5_BIT;
        break;
    case 'f':
        ENABLE_USER_PORT_6();
        USER_DDR_6 |= USER_PORT_6_BIT;
        break;
    case 'g':
        ENABLE_USER_PORT_7();
        USER_DDR_7 |= USER_PORT_7_BIT;
        break;
    case 'h':
        ENABLE_USER_PORT_8();
        USER_DDR_8 |= USER_PORT_8_BIT;
        break;
    case 'i':
        ENABLE_USER_PORT_9();
        USER_DDR_9 |= USER_PORT_9_BIT;
        break;
    case 'j':
        ENABLE_USER_PORT_10();
        USER_DDR_10 |= USER_PORT_10_BIT;
        break;
    case 'k':
        ENABLE_USER_PORT_11();
        USER_DDR_11 |= USER_PORT_11_BIT;
        break;
    case 'l':
        ENABLE_USER_PORT_12();
        USER_DDR_12 |= USER_PORT_12_BIT;
        break;
    case 'm':
        ENABLE_USER_PORT_13();
        USER_DDR_13 |= USER_PORT_13_BIT;
        break;
    case 'n':
        ENABLE_USER_PORT_14();
        USER_DDR_14 |= USER_PORT_14_BIT;
        break;
    case 'o':
        ENABLE_USER_PORT_15();
        USER_DDR_15 |= USER_PORT_15_BIT;
        break;
    case 'p':
        ENABLE_USER_PORT_16();
        USER_DDR_16 |= USER_PORT_16_BIT;
        break;
    default:
        return 1;
    }
    return 0;
}

// Toggle the state of an output port
//
extern int fnTogglePortOut(CHAR cPortBit)
{
    switch (cPortBit) {
    case 'a':
        USER_PORT_1 ^= USER_PORT_1_BIT;
        break;
    case 'b':
        USER_PORT_2 ^= USER_PORT_2_BIT;
        break;
    case 'c':
        USER_PORT_3 ^= USER_PORT_3_BIT;
        break;
    case 'd':
        USER_PORT_4 ^= USER_PORT_4_BIT;
        break;
    case 'e':
        USER_PORT_5 ^= USER_PORT_5_BIT;
        break;
    case 'f':
        USER_PORT_6 ^= USER_PORT_6_BIT;
        break;
    case 'g':
        USER_PORT_7 ^= USER_PORT_7_BIT;
        break;
    case 'h':
        USER_PORT_8 ^= USER_PORT_8_BIT;
        break;
    case 'i':
        USER_PORT_9 ^= USER_PORT_9_BIT;
        break;
    case 'j':
        USER_PORT_10 ^= USER_PORT_10_BIT;
        break;
    case 'k':
        USER_PORT_11 ^= USER_PORT_11_BIT;
        break;
    case 'l':
        USER_PORT_12 ^= USER_PORT_12_BIT;
        break;
    case 'm':
        USER_PORT_13 ^= USER_PORT_13_BIT;
        break;
    case 'n':
        USER_PORT_14 ^= USER_PORT_14_BIT;
        break;
    case 'o':
        USER_PORT_15 ^= USER_PORT_15_BIT;
        break;
    case 'p':
        USER_PORT_16 ^= USER_PORT_16_BIT;
        break;
    default:
        break;
    }
    temp_pars->temp_parameters.usUserDefinedOutputs ^= (0x0001 << (cPortBit - 'a'));
    _SIM_PORTS;                                                          // ensure the simulator updates its port states
    return 0;
}

// Write a value to a group of output bits grouped as a byte
//
extern void fnSetPortOut(unsigned char ucPortOutputs, int iInitialisation)
{
    if (iInitialisation) {
        ENABLE_LED_PORT();                                               // ensure port is clocked and not in reset
    }
    else {
        ucPortOutputs &= DEMO_LED_DDR;                                   // remove non-output bits
    }
    DEMO_LED_PORT = (DEMO_LED_PORT & ~DEMO_USER_PORTS) | (ucPortOutputs & DEMO_USER_PORTS); // {12} set port values
    temp_pars->temp_parameters.ucUserOutputValues = (unsigned char)DEMO_LED_PORT;
    #if defined _WINDOWS
    fnPortChanges(0);                                                    // {14} ensure the simulator updates its port states
    #endif
}
#endif                                                                   // end !defined REMOVE_PORT_INITIALISATIONS
#endif                                                                   // end USE_MAINTENANCE


// Determine the cause of the last reset
//
extern unsigned char fnAddResetCause(CHAR *ptrBuffer)
{
    return 0;
}


#if defined SUPPORT_LOW_POWER                                            // {12}
extern void fnShowThisLowPowerMode(int iThisMode)
{
}

// Display the low power modes that the processor offers and show presently active one
//
extern void fnShowLowPowerMode(void)
{
}


    #if defined LOW_POWER_CYCLING_MODE
extern int fnVirtualWakeupInterruptHandler(int iDeepSleep)               // {13}
{
    return 0;
}
    #endif
#endif

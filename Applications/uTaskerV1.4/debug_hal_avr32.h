/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        debug_hal.h
    Project:     uTasker Demonstration project
                 - hardware application layer for AVR32
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    This file includes a number of hardware specific routines that are
    included in debug.c. This include file allows the main content of debug.c
    to be hardware independent and thus shared by all processor projects. 
    In addition it makes adding specific setups for different processor boards
    possible by exchanging this file or by including multiple sets of the
    routines in it, controlled by defines of each target type
    06.10.2018 Adjust fnAddResetCause() to allow output to debug interface {1}

*/


#if defined USE_MAINTENANCE

// Check the present state of a particular port
//
extern int fnPortState(CHAR cPortBit)
{
#if defined AVR32_AT32UC3C_EK                                            // this board has the LEDs connected to different ports
    switch (cPortBit) {
    case '1':
        return ((PVR_0 & DEMO_LED_1) != 0);
    case '2':
        return ((PVR_2 & DEMO_LED_2) != 0);
    case '3':
        return ((PVR_3 & DEMO_LED_3) != 0);
    case '4':
        return ((PVR_3 & DEMO_LED_4) != 0);
    default:
        return 0;
    }
#elif defined AVR32_UC3_C2_XPLAINED
    switch (cPortBit) {
    case '1':
        return ((PVR_3 & DEMO_LED_1) != 0);
    case '2':
        return ((PVR_2 & DEMO_LED_2) != 0);
    default:
        return 0;
    }
#elif defined AVR32_EVK1105 || defined AVR32_EVK1101 || defined AVR32_EVK1100  // {16}
    return ((PVR_1 & (FIRST_USER_PORT << (cPortBit - '1'))) != 0);       // return non zero if the port is reading a '1'
#else
    return 0;
#endif
}



// Check whether the port is configured as an input or as output
//
extern int fnPortInputConfig(CHAR cPortBit)
{
#if defined AVR32_AT32UC3C_EK                                            // this board has the LEDs connected to different ports
    switch (cPortBit) {
    case '1':
        return ((ODER_0 & DEMO_LED_1) == 0);
    case '2':
        return ((ODER_2 & DEMO_LED_2) == 0);
    case '3':
        return ((ODER_3 & DEMO_LED_3) == 0);
    case '4':
        return ((ODER_3 & DEMO_LED_4) == 0);
    default:
        return 0;
    }
#elif defined AVR32_UC3_C2_XPLAINED
    switch (cPortBit) {
    case '1':
        return ((ODER_3 & DEMO_LED_1) == 0);
    case '2':
        return ((ODER_2 & DEMO_LED_2) == 0);
    default:
        return 0;
    }
#elif defined AVR32_EVK1105 || defined AVR32_EVK1101 || defined AVR32_EVK1100  // {16}
    return ((ODER_1 & (FIRST_USER_PORT << (cPortBit - '1'))) == 0);      // return zero if the port is presently programmed as output
#else
    return 0;
#endif
}



// Configure a processor port pin as input/output or analogue
//
extern int fnConfigPort(CHAR cPortBit, CHAR cType)
{
    cPortBit -= '0';                                                     // select bit offset
    if ((cPortBit < 0) || (cPortBit > 4)) {
        return -1;
    }
    switch (cType) {
    case 'd':                                                            // default use for 0..4
    case 'i':                                                            // port to be input
#if defined AVR32_EVK1105
        ODERC_1 = (FIRST_USER_PORT << cPortBit);                         // port bits are assumed to be next to another on port B
    #ifdef _WINDOWS
        fnSimPorts(-1);                                                  // ensure the simulator updates its port states
    #endif
        temp_pars->temp_parameters.ucUserOutputs = (unsigned char)((ODER_1 & DEMO_USER_PORTS) >> SHIFT_PORT_TO_BYTE);
#elif defined AVR32_AT32UC3C_EK
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(0, DEMO_LED_1);                           // set port bit to input
            break;
        case 1:
            _CONFIG_PORT_INPUT(2, DEMO_LED_2);                           // set port bit to input
            break;
        case 2:
            _CONFIG_PORT_INPUT(3, DEMO_LED_3);                           // set port bit to input
            break;
        case 3:
            _CONFIG_PORT_INPUT(3, DEMO_LED_4);                           // set port bit to input
            break;
        }
        temp_pars->temp_parameters.ucUserOutputs &= ~(1 << cPortBit);    // save present DDR setting
#elif defined AVR32_UC3_C2_XPLAINED
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_INPUT(3, DEMO_LED_1);                           // set port bit to input
            break;
        case 1:
            _CONFIG_PORT_INPUT(2, DEMO_LED_2);                           // set port bit to input
            break;
        }
        temp_pars->temp_parameters.ucUserOutputs &= ~(1 << cPortBit);    // save present DDR setting
#endif
        break;

    case 'o':                                                            // port to be output
#if defined AVR32_EVK1105
        ODERS_1 = (FIRST_USER_PORT << cPortBit);                         // port bits are assumed to be next to another on port B
    #ifdef _WINDOWS
        fnSimPorts(-1);                                                  // ensure the simulator updates its port states
    #endif
        temp_pars->temp_parameters.ucUserOutputs = (unsigned char)((ODER_1 & DEMO_USER_PORTS) >> SHIFT_PORT_TO_BYTE);
#elif defined AVR32_AT32UC3C_EK
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(0, DEMO_LED_1);                          // set port bit to output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(2, DEMO_LED_2);                          // set port bit to output
            break;
        case 2:
            _CONFIG_PORT_OUTPUT(3, DEMO_LED_3);                          // set port bit to output
            break;
        case 3:
            _CONFIG_PORT_OUTPUT(3, DEMO_LED_4);                          // set port bit to output
            break;
        }
        temp_pars->temp_parameters.ucUserOutputs |= (1 << cPortBit);     // save present DDR setting
#elif defined AVR32_UC3_C2_XPLAINED
        switch (cPortBit) {
        case 0:
            _CONFIG_PORT_OUTPUT(3, DEMO_LED_1);                          // set port bit to output
            break;
        case 1:
            _CONFIG_PORT_OUTPUT(2, DEMO_LED_2);                          // set port bit to output
            break;
        }
        temp_pars->temp_parameters.ucUserOutputs |= (1 << cPortBit);     // save present DDR setting
#endif
        break;

    case 'a':                                                            // port to be analog
        break;

    default:
        return -1;
    }
    return 0;
}




// Set a single output to logical '1' or '0' state
//
static void fnSetPortBit(unsigned short usBit, int iSetClr)
{
    if (iSetClr) {
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
    }
    #if defined _WINDOWS
    fnSimPorts(-1);                                                      // ensure the simulator updates its port states
    #endif
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


// Configure port bit as output (a..p are port bit references)
//
static int fnConfigOutputPort(CHAR cPortBit)
{
    switch (cPortBit) {
    case 'a':
        USER_DDR_1 = USER_PORT_1_BIT;
        break;
    case 'b':
        USER_DDR_2 = USER_PORT_2_BIT;
        break;
    case 'c':
        USER_DDR_3 = USER_PORT_3_BIT;
        break;
    case 'd':
        USER_DDR_4 = USER_PORT_4_BIT;
        break;
    case 'e':
        USER_DDR_5 = USER_PORT_5_BIT;
        break;
    case 'f':
        USER_DDR_6 = USER_PORT_6_BIT;
        break;
    case 'g':
        USER_DDR_7 = USER_PORT_7_BIT;
        break;
    case 'h':
        USER_DDR_8 = USER_PORT_8_BIT;
        break;
    case 'i':
        USER_DDR_9 = USER_PORT_9_BIT;
        break;
    case 'j':
        USER_DDR_10 = USER_PORT_10_BIT;
        break;
    case 'k':
        USER_DDR_11 = USER_PORT_11_BIT;
        break;
    case 'l':
        USER_DDR_12 = USER_PORT_12_BIT;
        break;
    case 'm':
        USER_DDR_13 = USER_PORT_13_BIT;
        break;
    case 'n':
        USER_DDR_14 = USER_PORT_14_BIT;
        break;
    case 'o':
        USER_DDR_15 = USER_PORT_15_BIT;
        break;
    case 'p':
        USER_DDR_16 = USER_PORT_16_BIT;
        break;
    default:
        return 1;
    }
                                                             _SIM_PORTS; // ensure the simulator updates its port states
    return 0;
}


// Toggle the state of an output port
//
extern int fnTogglePortOut(CHAR cPortBit)
{
    unsigned short usNewBit = (0x0001 << (cPortBit - 'a'));

    if (fnUserPortState(cPortBit)) {
        fnSetPortBit(usNewBit, 0);
    }
    else {
        fnSetPortBit(usNewBit, 1);
    }

    temp_pars->temp_parameters.usUserDefinedOutputs ^= usNewBit;
    return 0;
}



// Write a value to a group of output bits grouped as a byte
//
extern void fnSetPortOut(unsigned char ucPortOutputs, int iInitialisation)
{
    #if defined AVR32_AT32UC3C_EK                                        // this board has the LEDs connected to different ports
    unsigned long ulPortOutputSetting[3] = {0};
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        ulPortOutputSetting[0] |= DEMO_LED_1;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        ulPortOutputSetting[1] |= DEMO_LED_2;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_3) {
        ulPortOutputSetting[2] |= DEMO_LED_3;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        ulPortOutputSetting[2] |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(0, ulPortOutputSetting[0], (DEMO_LED_1));
    _WRITE_PORT_MASK(2, ulPortOutputSetting[1], (DEMO_LED_2));
    _WRITE_PORT_MASK(3, ulPortOutputSetting[2], (DEMO_LED_3 | DEMO_LED_4));
    #elif defined AVR32_UC3_C2_XPLAINED
    unsigned long ulPortOutputSetting[2] = {0};
    if (ucPortOutputs & MAPPED_DEMO_LED_1) {
        ulPortOutputSetting[0] |= DEMO_LED_1;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_2) {
        ulPortOutputSetting[1] |= DEMO_LED_2;
    }
    _WRITE_PORT_MASK(3, ulPortOutputSetting[0], (DEMO_LED_1));
    _WRITE_PORT_MASK(2, ulPortOutputSetting[1], (DEMO_LED_2));
    #elif defined AVR32_EVK1105
    unsigned long ulChange = ((ucPortOutputs << SHIFT_PORT_TO_BYTE) & DEMO_USER_PORTS);
    OVRS_1 = (ulChange & DEMO_USER_PORTS);
    OVRC_1 = (~ulChange & DEMO_USER_PORTS);
        #if defined _WINDOWS
    fnSimPorts(-1);                                                      // ensure the simulator updates its port states
        #endif
    #endif
}
#endif                                                                   // end USE_MAINTENANCE

#if defined SUPPORT_LOW_POWER
extern void fnShowThisLowPowerMode(int iThisMode)
{
}

// Display the low power modes that the processor offers and show presently active one
//
extern void fnShowLowPowerMode(void)
{
}


    #if defined LOW_POWER_CYCLING_MODE
extern int fnVirtualWakeupInterruptHandler(int iDeepSleep)
{
    return 0;
}
    #endif
#endif

// Determine the cause of the last reset
//
extern unsigned char fnAddResetCause(CHAR *ptrBuffer)
{
    static const CHAR cWatchdog[]      = "WDOG";
    static const CHAR cPowerOn[]       = "Power-on";
    static const CHAR cUndervoltage[]  = "Brownout";
    static const CHAR cExternalReset[] = "External";
    static const CHAR cJTAG[]          = "JTAG";
    static const CHAR cCPU[]           = "CPU error";
    static const CHAR cODC[]           = "ODC";
    static const CHAR cHalt[]          = "JTAG/halt";

    static const CHAR cUnknown[]       = "???";
    const CHAR *ptrStr;
    if (PM_RCAUSE & PM_RCAUSE_POR) {                                     // power-on reset
        ptrStr = cPowerOn;
    }
    else if (PM_RCAUSE & PM_RCAUSE_BOD) {                                // under voltage
        ptrStr = cUndervoltage;
    }
    else if (PM_RCAUSE & PM_RCAUSE_EXT) {                                // external reset
        ptrStr = cExternalReset;
    }
    else if (PM_RCAUSE & PM_RCAUSE_WDT) {                                // watchdog timer
        ptrStr = cWatchdog;
    }
    else if (PM_RCAUSE & PM_RCAUSE_JTAG) {                               // j-tag reset
        ptrStr = cJTAG;
    }
    else if (PM_RCAUSE & PM_RCAUSE_CPU_ERR) {                            // cpu error reset
        ptrStr = cCPU;
    }
    else if (PM_RCAUSE & PM_RCAUSE_OCD) {                                // OCD reset
        ptrStr = cODC;
    }
    else if (PM_RCAUSE & PM_RCAUSE_JTAG_HARD) {                          // JTAG reset or halt instruction reset
        ptrStr = cHalt;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
    if (ptrBuffer == 0) {                                                // {1}
        fnDebugMsg((CHAR *)ptrStr);
        return 0;
    }
    return (uStrcpy(ptrBuffer, ptrStr) - ptrBuffer);                     // return the length of the string
}

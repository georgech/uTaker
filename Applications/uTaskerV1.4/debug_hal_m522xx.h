/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        debug_hal.h
    Project:     uTasker Demonstration project
                 - hardware application layer for M522xx
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    This file includes a number of hardware specific routines that are
    included in debug.c. This include file allows the main content of debug.c
    to be hardware independent and thus shared by all processor projects. 
    In addition it makes adding specific setups for different processor boards
    possible by exchanging this file or by including multiple sets of the
    routines in it, controlled by defines of each target type

    16.05.2013 Add ezport hardware interface                             {1}
    06.06.2018 Adjust fnAddResetCause() to allow output to debug interface {2}


*/


#if defined USE_MAINTENANCE

// Check the present state of a particular port
//
extern int fnPortState(CHAR cPortBit)
{
    unsigned char ucBit = 0x01;
    cPortBit -= '1';
    ucBit <<= cPortBit;

    if (ucBit & DDRTC) {
        if (PORTTC & ucBit) {
            return 1;                                                    // display as selected if output driven to '1'
        }
    }
    else {
        if (PORTIN_SETTC & ucBit) {
            return 1;                                                    // display as selected if input pulled to '1'
        }
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
    if (DDRTC & ucBit) {
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
    #if defined M52259_TOWER && defined GLCD_BACKLIGHT_CONTROL
        if (cPortBit > 2) {                                              // don't disturn backlight control
            break;
        }
    #else
        if (cPortBit > 3) {
            break;
        }
    #endif
        DDRTC &= ~(1 << cPortBit);                                       // set port bit to input
        temp_pars->temp_parameters.ucUserOutputs = DDRTC;
        break;

    case 'o':                                                            // port to be output
        if (cPortBit > 3) {
            break;
        }
        DDRTC |= (1 << cPortBit);                                        // set port bit to output
        temp_pars->temp_parameters.ucUserOutputs = DDRTC;
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
            USER_PORT_1 |= USER_PORT_1_BIT;
            break;
        case 0x0002:
            USER_PORT_2 |= USER_PORT_2_BIT;
            break;
        case 0x0004:
            USER_PORT_3 |= USER_PORT_3_BIT;
            break;
        case 0x0008:
            USER_PORT_4 |= USER_PORT_4_BIT;
            break;
        case 0x0010:
            USER_PORT_5 |= USER_PORT_5_BIT;
            break;
        case 0x0020:
            USER_PORT_6 |= USER_PORT_6_BIT;
            break;
        case 0x0040:
            USER_PORT_7 |= USER_PORT_7_BIT;
            break;
        case 0x0080:
            USER_PORT_8 |= USER_PORT_8_BIT;
            break;
        case 0x0100:
            USER_PORT_9 |= USER_PORT_9_BIT;
            break;
        case 0x0200:
            USER_PORT_10 |= USER_PORT_10_BIT;
            break;
        case 0x0400:
            USER_PORT_11 |= USER_PORT_11_BIT;
            break;
        case 0x0800:
            USER_PORT_12 |= USER_PORT_12_BIT;
            break;
        case 0x1000:
            USER_PORT_13 |= USER_PORT_13_BIT;
            break;
        case 0x2000:
            USER_PORT_14 |= USER_PORT_14_BIT;
            break;
        case 0x4000:
            USER_PORT_15 |= USER_PORT_15_BIT;
            break;
        case 0x8000:
            USER_PORT_16 |= USER_PORT_16_BIT;
            break;
        }
    }
    else {
        switch (usBit) {
        case 0x0001:
            USER_PORT_1 &= ~(USER_PORT_1_BIT);
            break;
        case 0x0002:
            USER_PORT_2 &= ~(USER_PORT_2_BIT);
            break;
        case 0x0004:
            USER_PORT_3 &= ~(USER_PORT_3_BIT);
            break;
        case 0x0008:
            USER_PORT_4 &= ~(USER_PORT_4_BIT);
            break;
        case 0x0010:
            USER_PORT_5 &= ~(USER_PORT_5_BIT);
            break;
        case 0x0020:
            USER_PORT_6 &= ~(USER_PORT_6_BIT);
            break;
        case 0x0040:
            USER_PORT_7 &= ~(USER_PORT_7_BIT);
            break;
        case 0x0080:
            USER_PORT_8 &= ~(USER_PORT_8_BIT);
            break;
        case 0x0100:
            USER_PORT_9 &= ~(USER_PORT_9_BIT);
            break;
        case 0x0200:
            USER_PORT_10 &= ~(USER_PORT_10_BIT);
            break;
        case 0x0400:
            USER_PORT_11 &= ~(USER_PORT_11_BIT);
            break;
        case 0x0800:
            USER_PORT_12 &= ~(USER_PORT_12_BIT);
            break;
        case 0x1000:
            USER_PORT_13 &= ~(USER_PORT_13_BIT);
            break;
        case 0x2000:
            USER_PORT_14 &= ~(USER_PORT_14_BIT);
            break;
        case 0x4000:
            USER_PORT_15 &= ~(USER_PORT_15_BIT);
            break;
        case 0x8000:
            USER_PORT_16 &= ~(USER_PORT_16_BIT);
            break;
        }
    }                                                                    _SIM_PORTS; // ensure the simulator updates its port states
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
        USER_DDR_1 |= USER_PORT_1_BIT;
        break;
    case 'b':
        USER_DDR_2 |= USER_PORT_2_BIT;
        break;
    case 'c':
        USER_DDR_3 |= USER_PORT_3_BIT;
        break;
    case 'd':
        USER_DDR_4 |= USER_PORT_4_BIT;
        break;
    case 'e':
        USER_DDR_5 |= USER_PORT_5_BIT;
        break;
    case 'f':
        USER_DDR_6 |= USER_PORT_6_BIT;
        break;
    case 'g':
        USER_DDR_7 |= USER_PORT_7_BIT;
        break;
    case 'h':
        USER_DDR_8 |= USER_PORT_8_BIT;
        break;
    case 'i':
        USER_DDR_9 |= USER_PORT_9_BIT;
        break;
    case 'j':
        USER_DDR_10 |= USER_PORT_10_BIT;
        break;
    case 'k':
        USER_DDR_11 |= USER_PORT_11_BIT;
        break;
    case 'l':
        USER_DDR_12 |= USER_PORT_12_BIT;
        break;
    case 'm':
        USER_DDR_13 |= USER_PORT_13_BIT;
        break;
    case 'n':
        USER_DDR_14 |= USER_PORT_14_BIT;
        break;
    case 'o':
        USER_DDR_15 |= USER_PORT_15_BIT;
        break;
    case 'p':
        USER_DDR_16 |= USER_PORT_16_BIT;
        break;
    default:
        return 1;
    }                                                                    _SIM_PORTS; // ensure the simulator updates its port states
    return 0;
}



// Toggle the state of an output port
//
extern int fnTogglePortOut(CHAR cPortBit)
{
    #if !defined _M520X && !defined _M523X 
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
    temp_pars->temp_parameters.usUserDefinedOutputs ^= (0x0001 << (cPortBit - 'a'));    _SIM_PORTS; // ensure the simulator updates its port states
    #endif
    return 0;
}

// Write a value to a group of output bits grouped as a byte
//
extern void fnSetPortOut(unsigned char ucPortOutputs, int iInitialisation)
{
    if (!iInitialisation) {
        ucPortOutputs &= DDRTC;                                          // remove non-output bits
    }
    PORTTC = ucPortOutputs;                                              // set port values
    temp_pars->temp_parameters.ucUserOutputValues = PORTTC;              _SIM_PORTS; // ensure the simulator updates its port states
}

#endif                                                                   // end USE_MAINTENANCE

#if defined EZPORT_CLONER                                                // {1}

#define EZRESETOUT          PORT_QS_BIT5                                  // port output for reset signal
#define EZCSOUT             PORT_QS_BIT3                                  // QSPI_CS0
#define EZDIN               PORT_QS_BIT0                                  // QSPI_DOUT
#define EZDOUT              PORT_QS_BIT1                                  // QSPI_DIN
#define EZCLKOUT            PORT_QS_BIT2                                  // QSPI_CLK

// Send and receive a single byte
//
static unsigned char fnSendEZPORT(unsigned char ucDataOut)
{
    unsigned char ucBit = 0x80;                                          // data sent and receive most significant bit first
    unsigned char ucReadByte = 0;
    do {
        _SETBITS(QS, EZCLKOUT);                                          // generate rising clock edge
        fnDelayLoop(1);
        if (ucDataOut & ucBit) {                                         // set new data output state
             _SETBITS(QS, EZDOUT);
        }
        else {
             _CLEARBITS(QS, EZDOUT);
        }
        fnDelayLoop(1);
        _CLEARBITS(QS, EZCLKOUT);                                         // falling clock edge
        fnDelayLoop(1);
        _SETBITS(QS, EZCLKOUT);                                           // rising clock edge
        if (_READ_PORT_MASK(QS, EZDIN)) {                                 // read data in
            ucReadByte |= ucBit;
        }
        ucBit >>= 1;
    } while (ucBit != 0);
    return ucReadByte;
}

static void fnConfigEz(void)
{
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, (EZRESETOUT | EZCSOUT | EZDOUT | EZCLKOUT), (EZDOUT | EZCLKOUT)); // configure outputs and drive EZDOUT and EZCLKOUT to '1' and EZCSOUT and EZRESETOUT to '0'
    _CONFIG_PORT_INPUT(QS, (EZDIN));                                     // configure EZDIN as input
}

static void fnNegateResetEz(void)                                        // remove reset with ezport chip select asserted
{
    _SETBITS(QS, EZRESETOUT);                                            // negate the reset line
    fnDelayLoop(1);                                                      // delay for the chip select to be latched
    _SETBITS(QS, EZCSOUT);                                               // negate ezport chip select
}

static void fnGetStatusEz(void)
{
    _CLEARBITS(QS, EZCSOUT);                                             // assert ezport chip select - clock always starts at '1'
    fnDebugMsg("EZPORT status = ");
    fnSendEZPORT(EZCOMMAND_RDSR);                                        // command status read
    fnDebugHex(fnSendEZPORT(0xff), (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF)); // clock in status read and display value
    _SETBITS(QS, EZCSOUT);                                               // terminate sequence
}

static void fnPrepareEz(CHAR *ptrInput)
{
    unsigned long ulClockSpeed = 25000000;                               // default clock speed 25MHz if nothing else entered
    unsigned char ucConfigSetting;
    if (*ptrInput != 0) {                                                // if a clock speed it passed, we calculate the correct setting for it
        ulClockSpeed = fnDecStrHex(ptrInput);                            // extract the speed in MHz
        ulClockSpeed *= 1000000;
    }
    ucConfigSetting = (unsigned char)((ulClockSpeed/2)/185000);          // the divisor needed to get around 200k (not less that 150k and not more than 200k)
    _CLEARBITS(QS, EZCSOUT);                                              // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_WRCR);                                        // start commanding configuration value
    fnDebugMsg("EZPORT configured with ");
    fnSendEZPORT(ucConfigSetting);                                       // send configuration value
    fnDebugHex(ucConfigSetting, (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF));
    _SETBITS(QS, EZCSOUT);                                                // terminate sequence
    fnDoHardware(DO_EZSTATUS , 0);                                       // show new status
}

static void fnBulkEraseEz(void)
{
    _CLEARBITS(QS, EZCSOUT);                                              // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_BE);
    _SETBITS(QS, EZCSOUT);                                                // terminate sequence
    fnDebugMsg("EZPORT bulk erase issued\r\n");
}

static void fnWriteEz(CHAR *ptrInput)
{
    if (*ptrInput != 0) {                                                // if a clock speed it passed, we calculate the correct setting for it
        unsigned char ucValue;
        _CLEARBITS(QS, EZCSOUT);                                          // assert ezport chip select - clock always starts at '1'
        ucValue = (unsigned char)fnHexStrHex(ptrInput);
        fnSendEZPORT(ucValue);
        fnDebugHex(ucValue, (sizeof(unsigned char) | WITH_LEADIN));
        fnDebugMsg(" written\r\n");
        _SETBITS(QS, EZCSOUT);                                            // terminate sequence
        fnDoHardware(DO_EZSTATUS , 0);                                   // show new status
    }
    else {
        fnDebugMsg("Value missing!\r\n");
    }
}
#endif

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
    #ifdef BACKUP_WATCHDOG_TIMER
    static const CHAR cWatchdog[]      = "WDOG";
    #endif
    static const CHAR cPowerOn[]       = "Power-on";
    static const CHAR cUndervoltage[]  = "Undervoltage";
    static const CHAR cClockLoss[]     = "Clock loss";
    static const CHAR cSoftwareReset[] = "Software";
    static const CHAR cExternalReset[] = "External";
    static const CHAR cUnknown[]       = "???";
    const CHAR *ptrStr;
    if (RESET_RSR == (RESET_POR | RESET_LVD)) {                          // power-on reset
        ptrStr = cPowerOn;
    }
    else if (RESET_RSR & RESET_LVD) {                                    // under voltage
        ptrStr = cUndervoltage;
    }
    #ifdef BACKUP_WATCHDOG_TIMER
    else if (RESET_RSR & (RESET_WDR | RESET_BWT)) {                      // background watchdog timer
        ptrStr = cWatchdog;
    }
    #endif
    else if (RESET_RSR & (RESET_LOL & RESET_LOC)) {                      // clock loss
        ptrStr = cClockLoss;
    }
    else if (RESET_RSR & RESET_SOFT) {                                   // commanded reset
        ptrStr = cSoftwareReset;
    }
    else if (RESET_RSR & RESET_EXT) {                                    // external reset
        ptrStr = cExternalReset;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
    if (ptrBuffer == 0) {                                                // {2}
        fnDebugMsg((CHAR *)ptrStr);
        return 0;
    }
    return (uStrcpy(ptrBuffer, ptrStr) - ptrBuffer);                     // return the length of the string
}

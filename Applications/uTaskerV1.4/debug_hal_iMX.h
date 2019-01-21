/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      debug_hal_iMX.h
    Project:   uTasker project
               - hardware application layer for iMX
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/


#if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
// Check the present state of a particular port
//
extern int fnPortState(CHAR cPortBit)
{
    switch (cPortBit) {
    case '1':
        return (((GPIO1_GDIR & GPIO1_DR & DEMO_LED_1) | (~GPIO1_GDIR & GPIO1_PSR & DEMO_LED_1)) != 0);
    case '2':
        return (((GPIO1_GDIR & GPIO1_DR & DEMO_LED_2) | (~GPIO1_GDIR & GPIO1_PSR & DEMO_LED_2)) != 0);
    case '3':
        return (((GPIO1_GDIR & GPIO1_DR & DEMO_LED_3) | (~GPIO1_GDIR & GPIO1_PSR & DEMO_LED_3)) != 0);
    case '4':
        return (((GPIO1_GDIR & GPIO1_DR & DEMO_LED_4) | (~GPIO1_GDIR & GPIO1_PSR & DEMO_LED_4)) != 0);
    default:
        return 0;
    }
}

// Check whether the port is configured as an input or as output
//
extern int fnPortInputConfig(CHAR cPortBit)
{
    switch (cPortBit) {
    case '1':
        return ((GPIO1_GDIR & DEMO_LED_1) == 0);
    case '2':
        return ((GPIO1_GDIR & DEMO_LED_2) == 0);
    case '3':
        return ((GPIO1_GDIR & DEMO_LED_3) == 0);
    case '4':
        return ((GPIO1_GDIR & DEMO_LED_4) == 0);
    default:
        return 0;
    }
}

// Configure a processor port pin as input/output or analogue
//
extern int fnConfigPort(CHAR cPortBit, CHAR cType)
{
    unsigned long ulPortBit;
    switch (cPortBit) {
    case '0':
        ulPortBit = DEMO_LED_1;
        break;
    case '1':
        ulPortBit = DEMO_LED_2;
        break;
    case '2':
        ulPortBit = DEMO_LED_3;
        break;
    case '3':
        ulPortBit = DEMO_LED_4;
        break;
    default:
        return 0;
    }
    cPortBit -= '0';                                                     // select bit offset
    switch (cType) {
    case 'd':                                                            // default use for 0..4
    case 'i':                                                            // port to be input
        _CONFIG_PORT_INPUT(1, ulPortBit, PORT_PS_UP_ENABLE);             // set port bit to input (on port 1)
        temp_pars->temp_parameters.ucUserOutputs &= ~(1 << cPortBit);    // save present DDR setting
        break;

    case 'o':                                                            // port to be output
        _CONFIG_PORT_OUTPUT(1, ulPortBit, PORT_SRE_SLOW);                // set port bit to output (on port 1)
        temp_pars->temp_parameters.ucUserOutputs |= (1 << cPortBit);     // save present DDR setting
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
        _CONFIG_OUTPUT_PORT_1();        
        break;
    case 'b':
        _CONFIG_OUTPUT_PORT_2(); 
        break;
    case 'c':
        _CONFIG_OUTPUT_PORT_3(); 
        break;
    case 'd':
        _CONFIG_OUTPUT_PORT_4(); 
        break;
    case 'e':
        _CONFIG_OUTPUT_PORT_5(); 
        break;
    case 'f':
        _CONFIG_OUTPUT_PORT_6(); 
        break;
    case 'g':
        _CONFIG_OUTPUT_PORT_7(); 
        break;
    case 'h':
        _CONFIG_OUTPUT_PORT_8(); 
        break;
    case 'i':
        _CONFIG_OUTPUT_PORT_9(); 
        break;
    case 'j':
        _CONFIG_OUTPUT_PORT_10(); 
        break;
    case 'k':
        _CONFIG_OUTPUT_PORT_11(); 
        break;
    case 'l':
        _CONFIG_OUTPUT_PORT_12(); 
        break;
    case 'm':
        _CONFIG_OUTPUT_PORT_13(); 
        break;
    case 'n':
        _CONFIG_OUTPUT_PORT_14(); 
        break;
    case 'o':
        _CONFIG_OUTPUT_PORT_15(); 
        break;
    case 'p':
        _CONFIG_OUTPUT_PORT_16(); 
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
        USER_TOGGLE_PORT_1 = USER_PORT_1_BIT;
        break;
    case 'b':
        USER_TOGGLE_PORT_2 = USER_PORT_2_BIT;
        break;
    case 'c':
        USER_TOGGLE_PORT_3 = USER_PORT_3_BIT;
        break;
    case 'd':
        USER_TOGGLE_PORT_4 = USER_PORT_4_BIT;
        break;
    case 'e':
        USER_TOGGLE_PORT_5 = USER_PORT_5_BIT;
        break;
    case 'f':
        USER_TOGGLE_PORT_6 = USER_PORT_6_BIT;
        break;
    case 'g':
        USER_TOGGLE_PORT_7 = USER_PORT_7_BIT;
        break;
    case 'h':
        USER_TOGGLE_PORT_8 = USER_PORT_8_BIT;
        break;
    case 'i':
        USER_TOGGLE_PORT_9 = USER_PORT_9_BIT;
        break;
    case 'j':
        USER_TOGGLE_PORT_10 = USER_PORT_10_BIT;
        break;
    case 'k':
        USER_TOGGLE_PORT_11 = USER_PORT_11_BIT;
        break;
    case 'l':
        USER_TOGGLE_PORT_12 = USER_PORT_12_BIT;
        break;
    case 'm':
        USER_TOGGLE_PORT_13 = USER_PORT_13_BIT;
        break;
    case 'n':
        USER_TOGGLE_PORT_14 = USER_PORT_14_BIT;
        break;
    case 'o':
        USER_TOGGLE_PORT_15 = USER_PORT_15_BIT;
        break;
    case 'p':
        USER_TOGGLE_PORT_16 = USER_PORT_16_BIT;
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
    unsigned long ulPortOutputSetting = 0;
    if (iInitialisation != 0) {
        POWER_UP_ATOMIC(5, PORTC);                                       // ensure port is powered before writing initial values
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_1) != 0) {
        ulPortOutputSetting |= DEMO_LED_1;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_2) != 0) {
        ulPortOutputSetting |= DEMO_LED_2;
    }
    if ((ucPortOutputs & MAPPED_DEMO_LED_3) != 0) {
        ulPortOutputSetting |= DEMO_LED_3;
    }
    if (ucPortOutputs & MAPPED_DEMO_LED_4) {
        ulPortOutputSetting |= DEMO_LED_4;
    }
    _WRITE_PORT_MASK(1, ulPortOutputSetting, (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4));
}
#endif                                                                   // end USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS

#if defined EZPORT_CLONER && defined USE_MAINTENANCE && defined KEEP_DEBUG // {2}
#if defined FRDM_K64F
    #define EZRESETOUT          PORTE_BIT24                              // port output for reset signal
    #define EZCSOUT             PORTC_BIT4                               // port output for chip select control
    #define EZDIN               PORTD_BIT3                               // port input for data
    #define EZDOUT              PORTD_BIT2                               // port output for data
    #define EZCLKOUT            PORTD_BIT1                               // port output for clock control

    #define _ASSERT_EZRESETOUT() _CLEARBITS(E, EZRESETOUT)               // assert the reset line
    #define _NEGATE_EZRESETOUT() _SETBITS(E, EZRESETOUT)                 // negate the reset line
    #define _ASSERT_EZCSOUT()   _CLEARBITS(C, EZCSOUT)                   // assert the chip select line
    #define _NEGATE_EZCSOUT()   _SETBITS(C, EZCSOUT)                     // negate the chip select line
    #define _SET_EZCLKOUT_HIGH() _SETBITS(D, EZCLKOUT)                   // generate a rising clock edge
    #define _SET_EZCLKOUT_LOW()  _CLEARBITS(D, EZCLKOUT)                 // generate a falling clock edge
    #define _SET_EZDOUT_HIGH()  _SETBITS(D, EZDOUT)                      // set EZDOUT to '1'
    #define _SET_EZDOUT_LOW()   _CLEARBITS(D, EZDOUT)                    // set EZDOUT to '0'
    #define _GET_EZDIN_STATE()  _READ_PORT_MASK(D, EZDIN)                // read the state of the EZDIN pin
#else
    #define EZRESETOUT          PORTD_BIT15                              // port output for reset signal
    #define EZCSOUT             PORTD_BIT11                              // SPI2_SC0
    #define EZDIN               PORTD_BIT13                              // SPI2_MOSI
    #define EZDOUT              PORTD_BIT13                              // SPI2_MISO
    #define EZCLKOUT            PORTD_BIT12                              // SPI2_CLK

    #define _ASSERT_EZRESETOUT() _SETBITS(D, EZRESETOUT);                // assert the reset line
    #define _NEGATE_EZRESETOUT() _CLEARBITS(D, EZRESETOUT);              // negate the reset line
    #define _ASSERT_EZCSOUT()   _CLEARBITS(D, EZCSOUT)                   // assert the chip select line
    #define _NEGATE_EZCSOUT()   _SETBITS(D, EZCSOUT)                     // negate the chip select line
    #define _SET_EZCLKOUT_HIGH() _SETBITS(D, EZCLKOUT)                   // generate a rising clock edge
    #define _SET_EZCLKOUT_LOW()  _CLEARBITS(D, EZCLKOUT)                 // generate a falling clock edge
    #define _SET_EZDOUT_HIGH()  _SETBITS(D, EZDOUT)                      // set EZDOUT to '1'
    #define _SET_EZDOUT_LOW()   _CLEARBITS(D, EZDOUT)                    // set EZDOUT to '0'
    #define _GET_EZDIN_STATE()  _READ_PORT_MASK(D, EZDIN)                // read the state of the EZDIN pin
#endif

// Configure the EzPort signals to their default state
//
static void fnConfigEz(void)
{
    #if defined FRDM_K64F
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, (EZRESETOUT), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); // configure reset output driven low
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (EZCSOUT), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); // configure the chip select output driven low
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (EZDOUT | EZCLKOUT), (EZDOUT | EZCLKOUT), (PORT_SRE_SLOW | PORT_DSE_HIGH)); // configure the data and clock outputs driven high
    _CONFIG_PORT_INPUT_FAST_LOW(D, (EZDIN), (PORT_PS_UP_ENABLE));        // configure the data input
    #else
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (EZRESETOUT | EZCSOUT | EZDOUT | EZCLKOUT), (EZDOUT | EZCLKOUT), (PORT_SRE_SLOW | PORT_DSE_HIGH));
    _CONFIG_PORT_INPUT(D, (EZDIN), (PORT_PS_UP_ENABLE));
    #endif
}

// Send and receive a single byte
// - clock is '1' on entry (this can be used to send a command or data)
// - the chip select is asserted
//
static unsigned char fnSendEZPORT(unsigned char ucDataOut)
{
    unsigned char ucBit = 0x80;                                          // data sent and receive most significant bit first
    unsigned char ucReadByte = 0;
    do {
        if ((ucDataOut & ucBit) != 0) {                                  // set data output state
            _SET_EZDOUT_HIGH();                                          // set EZDOUT to '1'
        }
        else {
            _SET_EZDOUT_LOW();                                           // set EZDOUT to '0'
        }
        fnDelayLoop(1);
        _SET_EZCLKOUT_LOW();                                             // generate falling clock edge
        fnDelayLoop(1);                   
        _SET_EZCLKOUT_HIGH();                                            // rising clock edge
        if (_GET_EZDIN_STATE() != 0) {                                   // read data in
            ucReadByte |= ucBit;
        }
        ucBit >>= 1;
    } while (ucBit != 0);
    return ucReadByte;                                                   // returns with clock set to '1'
}

// Take the slave out of reset, latching its EzPort mode
//
static void fnNegateResetEz(int iEzMode)                                 // remove reset with ezport chip select asserted
{
    if (iEzMode != 0) {
        fnDelayLoop(1);
        _ASSERT_EZCSOUT();                                               // assert ezport chip select
        _NEGATE_EZRESETOUT();                                            // remove the reset state
        fnDelayLoop(EZ_RESET_CS_DELAY_US);                               // delay for the chip select to be latched by the slave
        _NEGATE_EZCSOUT();                                               // negate ezport chip select
    }
    else {
        _ASSERT_EZRESETOUT();                                            // assert the reset state
        _NEGATE_EZCSOUT();                                               // negate ezport chip select
        fnDelayLoop(10);
        _NEGATE_EZRESETOUT();                                            // remove the reset state so that the device can start up normally
    }
    fnDelayLoop(10);
}

// Read the slave's EzPort status register
//
static unsigned char fnGetStatusEz(int iDisplay)
{
    unsigned char ucStatus;
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_RDSR);                                        // command status read
    ucStatus = fnSendEZPORT(0xff);                                       // read the returned status value
    #if defined _WINDOWS
    ucStatus = 0;
    #endif
    if (iDisplay != 0) {                                                 // if the value is to be displayed
        fnDebugMsg("EZPORT status = ");
        fnDebugHex(ucStatus, (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF));
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
    return ucStatus;                                                     // return the read status value
}

#if defined _M5223X
#define RESET_CLOCK_SPEED       20000000                                 // default clock speed out of reset
static void fnPrepareEz(CHAR *ptrInput)
{
    unsigned long ulClockSpeed = RESET_CLOCK_SPEED;                      // default slave processor clock speed if nothing else entered
    unsigned char ucConfigSetting;
    if (*ptrInput != 0) {                                                // if a clock speed it passed, we calculate the correct setting for it
        ulClockSpeed = fnDecStrHex(ptrInput);                            // extract the speed in Hz
    }
    ucConfigSetting = (unsigned char)((ulClockSpeed/2)/185000);          // the divisor needed to get around 200k (not less that 150k and not more than 200k)
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    fnSendEZPORT(EZCOMMAND_WRCR);                                        // start commanding configuration value
    fnDebugMsg("EZPORT configured with ");
    fnSendEZPORT(ucConfigSetting);                                       // send configuration value
    fnDebugHex(ucConfigSetting, (sizeof(unsigned char) | WITH_LEADIN | WITH_CR_LF));
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
    fnDoHardware(DO_EZSTATUS , 0);                                       // show new status
}
#endif

// The write enable must have been set before these commands are acceted
//
static void fnEraseEz(CHAR *ptrInput)
{
    _ASSERT_EZCSOUT();
    fnSendEZPORT(EZCOMMAND_WREN);
    _NEGATE_EZCSOUT();
    fnDelayLoop(1);
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    if (ptrInput != 0) {
        unsigned long ulSectorAddress = fnHexStrHex(ptrInput);
        ulSectorAddress &= ~(0xff000000f);                               // address must be 128 bit aligned
        fnSendEZPORT(EZCOMMAND_SE);                                      // sector erase command
        fnSendEZPORT((unsigned char)(ulSectorAddress >> 16));            // send 24 bit address
        fnSendEZPORT((unsigned char)(ulSectorAddress >> 8));
        fnSendEZPORT((unsigned char)(ulSectorAddress));
        fnDebugMsg("Sector [");
        fnDebugHex(ulSectorAddress, (sizeof(ulSectorAddress) | WITH_LEADIN));
        fnDebugMsg("]");
    }
    else {
        fnSendEZPORT(EZCOMMAND_BE);
        fnDebugMsg("Bulk");
    }
    fnDebugMsg(" erase issued\r\n");
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

// Send a command and display the resulting status register value
//
static void fnCommandEz(CHAR *ptrInput)
{
    if (*ptrInput != 0) {
        unsigned char ucValue;
        _ASSERT_EZCSOUT();                                               // assert ezport chip select - clock always starts at '1'
        ucValue = (unsigned char)fnHexStrHex(ptrInput);
        fnSendEZPORT(ucValue);
        fnDebugHex(ucValue, (sizeof(unsigned char) | WITH_LEADIN));
        fnDebugMsg(" written\r\n");
        _NEGATE_EZCSOUT();                                               // terminate sequence by negating the chip select line
        fnDoHardware(DO_EZSTATUS , 0);                                   // show new status
    }
    else {
        fnDebugMsg("Command missing!\r\n");
    }
}

// The start address must be long word aligned, the clock must be <= 1/8 the system clock
//  the command is not accepted if WEF, WIP or FS status is true
//
static void fnReadEz(CHAR *ptrInput)
{
    static unsigned long ulLastReadAddress = 0;
    static unsigned short usLastReadLength = 16;
    int iRead = 0;
    unsigned short usLength = usLastReadLength;                          // default length if nothing else entered
    if (*ptrInput != 0) {
        ulLastReadAddress = fnHexStrHex(ptrInput);                       // the address to be read from
        if (fnJumpWhiteSpace(&ptrInput) == 0) {                          // optional length
            usLength = (unsigned short)fnDecStrHex(ptrInput);
            if (usLength == 0) {
                usLength = 1;
            }
        }
    }
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulLastReadAddress &= ~(0xff0000003);                                 // address must be long word aligned
    fnSendEZPORT(EZCOMMAND_READ);                                        // send read command
    fnSendEZPORT((unsigned char)(ulLastReadAddress >> 16));              // send 24 bit address
    fnSendEZPORT((unsigned char)(ulLastReadAddress >> 8));
    fnSendEZPORT((unsigned char)(ulLastReadAddress));
    fnDebugMsg("\r\nRead from address ");
    fnDebugHex(ulLastReadAddress, (sizeof(ulLastReadAddress) | WITH_LEADIN | WITH_CR_LF));
    usLastReadLength = usLength;
    while (usLength-- != 0) {
        fnDebugHex(fnSendEZPORT(0xff), (sizeof(unsigned char) | WITH_LEADIN | WITH_SPACE)); // read bytes
        ulLastReadAddress++;
        if (++iRead >= 16) {
            fnDebugMsg("\r\n");
            iRead = 0;
        }
    }
    if (iRead != 0) {
        fnDebugMsg("\r\n");
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

static void fnGetEz(unsigned char *ptrBuffer, unsigned long ulReadAddress, int iLength)
{
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulReadAddress &= ~(0xff0000003);                                     // address must be long word aligned
    fnSendEZPORT(EZCOMMAND_READ);                                        // send read command
    fnSendEZPORT((unsigned char)(ulReadAddress >> 16));                  // send 24 bit address
    fnSendEZPORT((unsigned char)(ulReadAddress >> 8));
    fnSendEZPORT((unsigned char)(ulReadAddress));
    while (iLength-- != 0) {
        *ptrBuffer++ = fnSendEZPORT(0xff);
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

static void fnProgEz(CHAR *ptrInput)
{
    static unsigned long ulLastWriteAddress = 0;
    int iWriteLength = 16;                                               // writes must be multiples of 16 bytes
    unsigned char ucData[16];
    unsigned char *ptrData = ucData;
    _ASSERT_EZCSOUT();
    fnSendEZPORT(EZCOMMAND_WREN);
    _NEGATE_EZCSOUT();
    uMemset(ucData, 0x55, sizeof(ucData));                               // default pattern if nothing else set
    if (*ptrInput != 0) {
        if (*ptrInput == 'U') {                                          // unsecure command
            ulLastWriteAddress = 0x400;                                  // flash configuration
            uMemset(ucData, 0xff, sizeof(ucData));
            ucData[12] = 0xfe;                                           // clear unsecure bit
    #if !defined _WINDOWS
            while ((fnGetStatusEz(0) & EZCOMMAND_SR_WIP) != 0) {}        // wait for a possible bulk-erase to complete
    #endif
        }
        else {
            ulLastWriteAddress = fnHexStrHex(ptrInput);                  // the address to be written to
            if (fnJumpWhiteSpace(&ptrInput) == 0) {                      // optional value
                unsigned char ucPattern = (unsigned char)fnDecStrHex(ptrInput);
                int i = 0;
                while (i < sizeof(ucData)) {
                    ucData[i++] = ucPattern++;
                }
            }
        }
    }
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulLastWriteAddress &= ~(0xff000000f);                                // address must be 128 bit aligned
    fnSendEZPORT(EZCOMMAND_PP);                                          // send program command
    fnSendEZPORT((unsigned char)(ulLastWriteAddress >> 16));             // send 24 bit address
    fnSendEZPORT((unsigned char)(ulLastWriteAddress >> 8));
    fnSendEZPORT((unsigned char)(ulLastWriteAddress));
    fnDebugMsg("\r\nWriting to address ");
    fnDebugHex(ulLastWriteAddress, (sizeof(ulLastWriteAddress) | WITH_LEADIN | WITH_CR_LF));
    while (iWriteLength-- != 0) {
        fnSendEZPORT(*ptrData++);                                        // send bytes
        ulLastWriteAddress++;
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}

// This routine enables a write and copies/programs up to a sector of data content
//
static void fnCopyEz(unsigned long ulWriteAddress, unsigned char *ptrSource, int iLength)
{
    _ASSERT_EZCSOUT();
    fnSendEZPORT(EZCOMMAND_WREN);                                        // enable writes
    _NEGATE_EZCSOUT();
    fnDelayLoop(1);
    _ASSERT_EZCSOUT();                                                   // assert ezport chip select - clock always starts at '1'
    ulWriteAddress &= ~(0xff000000f);                                    // address must be 128 bit aligned
    fnSendEZPORT(EZCOMMAND_PP);                                          // send program command
    fnSendEZPORT((unsigned char)(ulWriteAddress >> 16));                 // send 24 bit address
    fnSendEZPORT((unsigned char)(ulWriteAddress >> 8));
    fnSendEZPORT((unsigned char)(ulWriteAddress));
    while (iLength-- != 0) {
        fnSendEZPORT(*ptrSource++);                                      // send bytes
    }
    _NEGATE_EZCSOUT();                                                   // terminate sequence by negating the chip select line
}
#endif

#if defined SUPPORT_LOW_VOLTAGE_DETECTION                                // {14}
// Power failure user interrupt callback that can be used to save critical work
//
extern __callback_interrupt int fnPowerFailureWarning(void)
{
    fnDebugMsg("Power failing!!\r\n");
    return 0;                                                            // don't re-enable detection interrupt since in the case of power loss it will continue to fire
}
#endif

// Determine the cause of the last reset
//
extern unsigned char fnAddResetCause(CHAR *ptrBuffer)
{
    const CHAR *ptrStr;
#if !defined KINETIS_KE                                                  // {11}
    #if !defined KINETIS_KL
    static const CHAR cJtag[]          = "JTAG";
    #endif
#endif
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
    static const CHAR cWakeupOther[]   = "Wakeup";
    static const CHAR cWakeup[]        = "Wakeup reset";
#endif
    static const CHAR cLockup[]        = "Lockup";
    static const CHAR cSoftwareReset[] = "Software";
    static const CHAR cPowerOn[]       = "Power-on";
    static const CHAR cUndervoltage[]  = "Undervoltage";
    static const CHAR cResetInput[]    = "External";
    static const CHAR cWatchdog[]      = "WDOG";
    static const CHAR cClockLoss[]     = "Clock loss";
    static const CHAR cUnknown[]       = "???";
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18 // {11}
    static const CHAR cHostDebug[]     = "Host debugger";
    static const CHAR cPerFailure[]    = "peripheral failure";
    if ((SIM_SRSID & SIM_SRSID_POR) != 0) {                              // power on reset
        ptrStr = cPowerOn;
    }
    else if ((SIM_SRSID & SIM_SRSID_LVD) != 0) {                         // low voltage detector
        ptrStr = cUndervoltage;
    }
    else if ((SIM_SRSID & SIM_SRSID_LOC) != 0) {                         // loss of clock
        ptrStr = cClockLoss;
    }
    else if ((SIM_SRSID & SIM_SRSID_WDOG) != 0) {                        // watchdog
        ptrStr = cWatchdog;
    }
    else if ((SIM_SRSID & SIM_SRSID_PIN) != 0) {                         // reset pin
        ptrStr = cResetInput;
    }
    else if ((SIM_SRSID & SIM_SRSID_LOCKUP) != 0) {                      // core lockup
        ptrStr = cLockup;
    }
    else if ((SIM_SRSID & SIM_SRSID_SW) != 0) {                          // software reset
        ptrStr = cSoftwareReset;
    }
    else if ((SIM_SRSID & SIM_SRSID_MDMAP) != 0) {                       // host debugger
        ptrStr = cHostDebug;
    }
    else if ((SIM_SRSID & SIM_SRSID_SACKERR) != 0) {                     // peripheral failed to acknowledge attempt to enter stop mode
        ptrStr = cPerFailure;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
#elif defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_KM || defined KINETIS_KE15 || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000) // {7}
    static const CHAR cHostDebug[]     = "Host debugger";
    #if !defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_KM
    static const CHAR cEZPORT[]        = "EZPORT";
    static const CHAR cTamper[]        = "tamper";
    #endif
    #if defined RCM_SRS0_LOL
    static const CHAR cPLL_LockLoss[]  = "lost PLL lock";
    #endif
    static const CHAR cPerFailure[]    = "peripheral failure";

    if ((RCM_SRS0 & RCM_SRS0_POR) != 0) {                                // power on reset
        ptrStr = cPowerOn;
    }
    else if ((RCM_SRS0 & RCM_SRS0_LVD) != 0) {                           // low voltage detector
        ptrStr = cUndervoltage;
    }
    else if ((RCM_SRS0 & RCM_SRS0_LOC) != 0) {                           // loss of external clock
        ptrStr = cClockLoss;
    }
    #if defined RCM_SRS0_LOL
    else if ((RCM_SRS0 & RCM_SRS0_LOL) != 0) {                           // loss of PLL lock
        ptrStr = cPLL_LockLoss;
    }
    #endif
    else if ((RCM_SRS0 & RCM_SRS0_WDOG) != 0) {                          // watchdog
        ptrStr = cWatchdog;
    }
    else if ((RCM_SRS0 & RCM_SRS0_WAKEUP) != 0) {
        if ((RCM_SRS0 & RCM_SRS0_PIN) != 0) {
            ptrStr = cWakeup;                                            // wakeup via reset pin
        }
        else {
            ptrStr = cWakeupOther;                                       // wakeup from other source
        }
    }
    else if ((RCM_SRS0 & RCM_SRS0_PIN) != 0) {                           // reset pin
        ptrStr = cResetInput;
    }
    #if !defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_KM
    else if ((RCM_SRS1 & RCM_SRS1_JTAG) != 0) {                          // jtag
        ptrStr = cJtag;
    }
    #endif
    else if ((RCM_SRS1 & RCM_SRS1_LOCKUP) != 0) {                        // core lockup
        ptrStr = cLockup;
    }
    else if ((RCM_SRS1 & RCM_SRS1_SW) != 0) {                            // software reset
        ptrStr = cSoftwareReset;
    }
    else if ((RCM_SRS1 & RCM_SRS1_MDM_AP) != 0) {                        // host debugger
        ptrStr = cHostDebug;
    }
    #if !defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_KM
    else if ((RCM_SRS1 & RCM_SRS1_EZPT) != 0) {                          // EZPORT reset
        ptrStr = cEZPORT;
    }
    #endif
    else if ((RCM_SRS1 & RCM_SRS1_SACKERR) != 0) {                       // peripheral failed to acknowledge attempt to enter stop mode
        ptrStr = cPerFailure;
    }
    #if !defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_KM
    else if ((RCM_SRS1 & RCM_SRS1_TAMPER) != 0) {                        // tamper detect
        ptrStr = cTamper;
    }
    #endif
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
#else
    if ((MC_SRSH & MC_SRSH_JTAG) != 0) {                                 // jtag reset
        ptrStr = cJtag;
    }
    else if ((MC_SRSH & MC_SRSH_LOCKUP) != 0) {                          // lockup reset
        ptrStr = cLockup;
    }
    else if ((MC_SRSH & MC_SRSH_SW) != 0) {                              // software reset
        ptrStr = cSoftwareReset;
    }
    else if (MC_SRSL == (MC_SRSL_POR | MC_SRSL_LVD)) {                   // power on reset
        ptrStr = cPowerOn;
    }
    else if (MC_SRSL == MC_SRSL_LVD) {                                   // low voltage detect reset
        ptrStr = cUndervoltage;
    }
    else if (MC_SRSL == (MC_SRSL_PIN | MC_SRSL_WAKEUP)) {                // low leakage wakeup reset due to reset pin
        ptrStr = cWakeup;
    }
    else if (MC_SRSL == MC_SRSL_WAKEUP) {                                // low voltage wakeup due to other source
        ptrStr = cWakeupOther;
    }
    else if ((MC_SRSL & MC_SRSL_PIN) != 0) {                             // reset input
        ptrStr = cResetInput;
    }
    else if ((MC_SRSL & MC_SRSL_COP) != 0) {                             // watchdog reset
        ptrStr = cWatchdog;
    }
    else if ((MC_SRSL & MC_SRSL_LOC) != 0) {                             // clock-loss
        ptrStr = cClockLoss;
    }
    else {                                                               // unexpected
        ptrStr = cUnknown;
    }
#endif
    if (ptrBuffer == 0) {                                                // {8}
        fnDebugMsg((CHAR *)ptrStr);
        return 0;
    }
    return (uStrcpy(ptrBuffer, ptrStr) - ptrBuffer);                     // return the length of the string
}

#if defined SUPPORT_LOW_POWER                                            // {12}
extern void fnShowThisLowPowerMode(int iThisMode)
{
    switch (iThisMode) {
    case RUN_MODE:
        fnDebugMsg("RUN");                                               // no low power mode used
        break;
    case WAIT_MODE:
        fnDebugMsg("WAIT");
        break;
    case STOP_MODE:
        fnDebugMsg("STOP");
        break;
    #if defined KINETIS_K22
    case VLPR_MODE:
        fnDebugMsg("VLPR");
        break;
    case VLPW_MODE:
        fnDebugMsg("VLPW");
        break;
    case VLPS_MODE:
        fnDebugMsg("VLPS");
        break;
    case LLS2_MODE:
        fnDebugMsg("LLS2");
        break;
    case LLS3_MODE:
        fnDebugMsg("LLS3");
        break;
    case VLLS0_MODE:
        fnDebugMsg("VLLS0");
        break;
    case VLLS1_MODE:
        fnDebugMsg("VLLS1");
        break;
    case VLLS2_MODE:
        fnDebugMsg("VLLS2");
        break;
    case VLLS3_MODE:
        fnDebugMsg("VLLS3");
        break;
    #elif defined KINETIS_KL17 || defined KINETIS_KL27
    case VLPR_MODE:
        fnDebugMsg("VLPR");
        break;
    case VLPW_MODE:
        fnDebugMsg("VLPW");
        break;
    case VLPS_MODE:
        fnDebugMsg("VLPS");
        break;
    case LLS_MODE:
        fnDebugMsg("LLS");
        break;
    case VLLS0_MODE:
        fnDebugMsg("VLLS0");
        break;
    case VLLS1_MODE:
        fnDebugMsg("VLLS1");
        break;
    case VLLS3_MODE:
        fnDebugMsg("VLLS3");
        break;
    #elif defined KINETIS_KL03
    case PSTOP1_MODE:
        fnDebugMsg("PSTOP1");
        break;
    case PSTOP2_MODE:
        fnDebugMsg("PSTOP2");
        break;
    case VLPR_MODE:
        fnDebugMsg("VLPR");
        break;
    case VLPW_MODE:
        fnDebugMsg("VLPW");
        break;
    case VLPS_MODE:
        fnDebugMsg("VLPS");
        break;
    case VLLS0_MODE:
        fnDebugMsg("VLLS0");
        break;
    case VLLS1_MODE:
        fnDebugMsg("VLLS1");
        break;
    case VLLS3_MODE:
        fnDebugMsg("VLLS3");
        break;
    #elif !defined KINETIS_KE
    case VLPR_MODE:
        fnDebugMsg("VLPR");
        break;
    case VLPS_MODE:
        fnDebugMsg("VLPS");
        break;
    case LLS_MODE:
        fnDebugMsg("LLS");
        break;
    case VLLS0_MODE:
        fnDebugMsg("VLLS0");
        break;
    case VLLS1_MODE:
        fnDebugMsg("VLLS1");
        break;
        #if defined KINETIS_KL
    case VLLS3_MODE:
        fnDebugMsg("VLLS3");
        break;
        #else
    case VLLS2_MODE:
        fnDebugMsg("VLLS2");
        break;
    case VLLS3_MODE:
        fnDebugMsg("VLLS3");
        break;
        #endif
    #endif
    }
}

// Display the low power modes that the processor offers and show presently active one
//
extern void fnShowLowPowerMode(void)
{
    int iMode;
    int iPresentMode;
    // Determine the present low power mode of operation from the SMC settings
    //
    iPresentMode = fnGetLowPowerMode();                                  // get the present mode from the device

    for (iMode = 0; iMode <= MAX_LP_MODES; iMode++) {                    // display the possible low power modes
        fnShowThisLowPowerMode(iMode);
        fnDebugMsg(" = ");
        fnDebugDec(iMode, 0);
        if (iPresentMode == iMode) {
            fnDebugMsg(" [active]");                                     // this mode is presently active
        }
        fnDebugMsg("\r\n");
    }
}


    #if defined LOW_POWER_CYCLING_MODE
extern int fnVirtualWakeupInterruptHandler(int iDeepSleep)               // {13}
{
    if (iDeepSleep == 0) {
        return 0;                                                        // only loop in deep sleep modes (not wait based)
    }
    #if defined FRDM_K22F
    if (fnIsPending(irq_UART1_ID) != 0) {                                // if there is a pending interrupt from the UART
        iLowPowerLoopMode = LOW_POWER_CYCLING_PAUSED;
        return 0;
    }
    if (fnIsPending(irq_RTC_SECONDS_ID) != 0) {                          // if there is a pending interrupt from the RTC
        iLowPowerLoopMode = LOW_POWER_CYCLING_PAUSED;
        return 0;
    }
    if (fnIsPending(irq_DMA4_ID) != 0) {                                 // if there is a pending interrupt from the UART tx DMA
        iLowPowerLoopMode = LOW_POWER_CYCLING_PAUSED;
        return 0;
    }
    #endif
    #if defined TICK_USES_LPTMR
    TOGGLE_TEST_OUTPUT();
    LPTMR0_CSR = LPTMR0_CSR;                                             // clear pending interrupt at LPTMR (wakeup source)
    WRITE_ONE_TO_CLEAR(*(volatile unsigned char *)(LLWU_FLAG_ADDRESS + 2), MODULE_LPTMR0); // reset the wakeup flag (write '1' to clear)
    fnClearPending(irq_LL_wakeup_ID);
    fnClearPending(irq_LPTMR0_ID);
    if (++iLowPowerLoopMode > (DELAY_LIMIT)(0.2 * SEC)) {                // take over basic tick operation to retrigger the watchog every 200ms
        fnRetriggerWatchdog();
        iLowPowerLoopMode = LOW_POWER_CYCLING_ENABLED;
    }
    TOGGLE_TEST_OUTPUT();
    #endif
    return 1;                                                            // stay in low power cycling mode
}
    #endif
#endif

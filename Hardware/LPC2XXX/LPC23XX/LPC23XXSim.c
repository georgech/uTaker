/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      LPC23XXSim.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    16.08.2009 Add key scanning support                                  {1}
    16.08.2009 Add port refresh flag                                     {2}
    08.10.2009 GPIO quantity made chip dependent                         {3}
    19.10.2009 Add fnInitInternalRTC() and simulate RTC                  {4}
    19.10.2009 Add LPC2101/2/3-rev.A initialisation                      {5}
    02.11.2009 Use fast IO with LPC2104/5/6 since new revisions have fast ports {6}
    03.11.2009 Add save and retrieval of battery backed up data          {7}
    07.11.2009 Remove P0.31 restriction for LPC2101/2/3 after tests show that it can be used as input {8}
    07.11.2009 Correct Timer simulation to count over longer delays      {9}
    08.11.2009 Adapt the LPC21XX vectored interrupt source operation to use slots {10}
    29.01.2010 Accept input changes on EXINT0..3 when their DDR is as output {11}
    27.02.2010 Add LPC2148                                               {12}
    11.03.2010 Correct interrupt vector slot number for LPC21xx UART1    {13}
    13.03.2010 Add USB device                                            {14}
    11.08.2010 Add ADC                                                   {15}
    14.02.2011 Increment timers when no clear on match and respect prescaler {16}
    10.10.2013 Add EMAC CRC to rx frame length                           {17}

*/  
                          

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined _LPC23XX && !defined _LPC17XX

/**************************************************************************************************/
/********************************* NXP LPC23XX ****************************************************/
/**************************************************************************************************/

// A copy of the LPC23XX peripheral registers for simulation purposes
//
extern LPC23XX_PERIPH  ucLPC23xx = {0};

static unsigned long ulPort_in_0, ulPort_in_1, ulPort_in_2, ulPort_in_3, ulPort_in_4;

static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = { 0 };

static int iFlagRefresh = 0;                                             // {2}


#ifdef USB_INTERFACE                                                     // {14}
    static unsigned long ulEndpointInt = 0;    
    static int iRxDataBank[NUMBER_OF_USB_ENDPOINTS] = {0};
    static unsigned long ulTxDataBank[NUMBER_OF_USB_ENDPOINTS] = {0};
    static int iTxDataToggle[NUMBER_OF_USB_ENDPOINTS] = {0};
    static unsigned char ucUSB_address = DEV_EN;                         // USB device address defaults to 0x00 and enabled after reset
#endif

// Initialisation of all non-zero registers in the device
//
static void fnSetDevice(unsigned long *port_inits)
{
    extern void fnEnterHW_table(void *hw_table);

    VICSWPriorityMask = 0xffff;

    VICVectPriority0  = 0xf;
    VICVectPriority1  = 0xf;
    VICVectPriority2  = 0xf;
    VICVectPriority3  = 0xf;
    VICVectPriority4  = 0xf;
    VICVectPriority5  = 0xf;
    VICVectPriority6  = 0xf;
    VICVectPriority7  = 0xf;
    VICVectPriority8  = 0xf;
    VICVectPriority9  = 0xf;
    VICVectPriority10 = 0xf;
    VICVectPriority11 = 0xf;
    VICVectPriority12 = 0xf;
    VICVectPriority13 = 0xf;
    VICVectPriority14 = 0xf;
    VICVectPriority15 = 0xf;
    VICVectPriority16 = 0xf;
    VICVectPriority17 = 0xf;
    VICVectPriority18 = 0xf;
    VICVectPriority19 = 0xf;
    VICVectPriority20 = 0xf;
    VICVectPriority21 = 0xf;
    VICVectPriority22 = 0xf;
    VICVectPriority23 = 0xf;
    VICVectPriority24 = 0xf;
    VICVectPriority25 = 0xf;
    VICVectPriority26 = 0xf;
    VICVectPriority27 = 0xf;
    VICVectPriority28 = 0xf;
    VICVectPriority29 = 0xf;
    VICVectPriority30 = 0xf;
    VICVectPriority31 = 0xf;

#ifdef _LPC214X
    PCONP = (PCTIM0 | PCTIM1 | PCUART0 | PCUART1 | PCPWM0 | PCPWM1 | PCI2C0 | PCSPI | PCRTC | PCI2C1 | PCSSP0 | PCI2C2);
#else
    PCONP = (PCTIM0 | PCTIM1 | PCUART0 | PCUART1 | PCPWM0 | PCPWM1 | PCI2C0 | PCSPI | PCRTC | PCSSP1 | PCEMC | PCI2C1 | PCSSP0 | PCI2C2);
#endif
    IRCTRIM = 0xa0;

	I2C0STAT = I2C_IDLE_STATE;                                           // I2C Controller
	I2C1STAT = I2C_IDLE_STATE;
	I2C2STAT = I2C_IDLE_STATE;
	I2C0SCLH = 0x04;
	I2C1SCLH = 0x04;
	I2C2SCLH = 0x04;
	I2C0SCLL = 0x04;
	I2C1SCLL = 0x04;
	I2C2SCLL = 0x04;

	MAMTIM   = 0x07;                                                     // memory Accelerator Module

#if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {6}
    FIO0PIN = ulPort_in_0 = *port_inits++;                               // set port inputs to default states
    FIO1PIN = ulPort_in_1 = *port_inits++;
    FIO2PIN = ulPort_in_2 = *port_inits++;
    FIO3PIN = ulPort_in_3 = *port_inits++;
    FIO4PIN = ulPort_in_4 = *port_inits;

    EMCControl = EMC_ENABLE;
    EMCStatus = 5;
    EMCDynamicControl = 0x006;
    EMCDynamicRP   = 0xf;
    EMCDynamicRAS  = 0xf;
    EMCDynamicSREX = 0xf;
    EMCDynamicAPR  = 0xf;
    EMCDynamicDAL  = 0xf;
    EMCDynamicWR   = 0xf;
    EMCDynamicRC   = 0x1f;
    EMCDynamicRFC  = 0x1f;
    EMCDynamicXSR  = 0x1f;
    EMCDynamicRRD  = 0xf;
    EMCDynamicMRD  = 0xf;

    EMCDynamicRasCas0 = (RAS_LATENCY_THREE_CCLK | CAS_LATENCY_THREE_CCLK);
    EMCDynamicRasCas1 = (RAS_LATENCY_THREE_CCLK | CAS_LATENCY_THREE_CCLK);
    EMCDynamicRasCas2 = (RAS_LATENCY_THREE_CCLK | CAS_LATENCY_THREE_CCLK);
    EMCDynamicRasCas3 = (RAS_LATENCY_THREE_CCLK | CAS_LATENCY_THREE_CCLK);

    EMCStaticWaitRd0   = 0x1f;
    EMCStaticWaitPage0 = 0x1f;
    EMCStaticWaitWr0   = 0x1f;
    EMCStaticWaitTurn0 = 0xf;

    EMCStaticWaitRd1   = 0x1f;
    EMCStaticWaitPage1 = 0x1f;
    EMCStaticWaitWr1   = 0x1f;
    EMCStaticWaitTurn1 = 0xf;

    EMCStaticWaitRd2   = 0x1f;
    EMCStaticWaitPage2 = 0x1f;
    EMCStaticWaitWr2   = 0x1f;
    EMCStaticWaitTurn2 = 0xf;

    EMCStaticWaitRd3   = 0x1f;
    EMCStaticWaitPage3 = 0x1f;
    EMCStaticWaitWr3   = 0x1f;
    EMCStaticWaitTurn3 = 0xf;
#else
    IO0PIN = ulPort_in_0 = *port_inits;
    #ifdef _LPC213X
    port_inits++;
    IO1PIN = ulPort_in_1 = *port_inits;
    #endif
#endif

#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {5}
    RTC_PWRCTRL = (RTC_PMAIN | RTC_PSRAM | RTC_POSC);
#endif

#if defined USB_INTERFACE                                                // {14}
    USBIntSt = EN_USB_INTS;
#endif

    AD0CR = ADCR_SEL_CHAN0;                                              // {15}
    AD0INTEN = ADCR_ADGINTEN;

    WDTC = 0xff;                                                         // watchdog
    WDTV = 0xff;
    WDFEED = 0x12345678;                                                 // read only register - value used by simulator

    CLRT = 0x370f;                                                       // Ethernet Controller
    MAXF = 0x600;
    MAC1 = (SOFT_RESET);
    EMAC_MODULE_ID = OLD_MAC_MODULE_ID;

    U0IIR = U1IIR = U2IIR = U3IIR = 0x01;                                // UART
    U0LSR = U1LSR = U2LSR = U3LSR = 0x60;
    U0TER = U1TER = U2TER = U3TER = 0x80;
    U0FDR = U1FDR = U2FDR = U3FDR = 0x10;

    RSIR = POR;                                                          // set power up as reset cause

    fnEnterHW_table(ucPortFunctions);
}


unsigned char ucFLASH[SIZE_OF_FLASH];

extern void fnInitialiseDevice(void *port_inits)
{
    uMemset(ucFLASH, 0xff, sizeof(ucFLASH));                             // we start with deleted FLASH memory contents
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned long *)port_inits);                            // set device registers to startup condition (if not zerod)
}

extern void fnDeleteFlashSector(unsigned char *ptrSector)
{
	int i = FLASH_GRANULARITY;

	while (i--) {
		*ptrSector++ = 0xff;
	}
}


extern unsigned char *fnGetEEPROM(unsigned short usOffset);

extern unsigned char *fnGetFileSystemStart(int iMemory)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROM(uFILE_START));
#elif defined (SAVE_COMPLETE_FLASH)
    return (&ucFLASH[0]);
#else
    return (&ucFLASH[uFILE_START-FLASH_START_ADDRESS]);
#endif
}


extern unsigned long fnGetFlashSize(int iMemory)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROMSize());
#elif defined (SAVE_COMPLETE_FLASH)
    return (sizeof(ucFLASH));
#else
    return (FILE_SYSTEM_SIZE);
#endif
}

static int iSectorsSelected[28] = {0};

static unsigned long fnGetSector(unsigned long FlashAddress)
{
#ifdef _LPC21XX
    return (FlashAddress/FLASH_GRANULARITY_LARGE);
#else
    FlashAddress -= FLASH_START_ADDRESS;
    if (FlashAddress < FLASH_SECTOR_8_OFFSET) {                          // one of the first small sectors
        return (FlashAddress/FLASH_GRANULARITY_SMALL);
    }
    else if (FlashAddress < FLASH_SECTOR_22_OFFSET) {                    // one of the middle large sectors
        FlashAddress -= FLASH_SECTOR_8_OFFSET;
        return ((FlashAddress/FLASH_GRANULARITY_LARGE) + 8);
    }
    else {                                                               // one of the last small sectors
        FlashAddress -= FLASH_SECTOR_22_OFFSET;
        return ((FlashAddress/FLASH_GRANULARITY_SMALL) + 22);
    }
#endif
}

static unsigned char *fnGetSectorAdd(unsigned long ulStartSector)
{
#ifdef _LPC21XX
    return (unsigned char *)(FLASH_START_ADDRESS + FLASH_SECTOR_0_OFFSET + (ulStartSector * FLASH_GRANULARITY_LARGE));
#else
    if (ulStartSector < 8) {                                             // one of the first small sectors
        return (unsigned char *)(FLASH_START_ADDRESS + FLASH_SECTOR_0_OFFSET + (ulStartSector * FLASH_GRANULARITY_SMALL));
    }
    else if (ulStartSector < 22) {                                       // one of the middle large sectors
        return (unsigned char *)(FLASH_START_ADDRESS + FLASH_SECTOR_8_OFFSET + ((ulStartSector - 8) * FLASH_GRANULARITY_LARGE));
    }
    else {                                                               // one of the last small sectors
        return (unsigned char *)(FLASH_START_ADDRESS + FLASH_SECTOR_22_OFFSET + ((ulStartSector - 22) * FLASH_GRANULARITY_SMALL));
    }
#endif
}

unsigned long fnGetSectorLength(unsigned long ulStartSector)
{
#ifdef _LPC21XX
    return (FLASH_GRANULARITY_LARGE);
#else
    if (ulStartSector < 8) {                                             // one of the first small sectors
        return (FLASH_GRANULARITY_SMALL);
    }
    else if (ulStartSector < 22) {                                       // one of the middle large sectors
        return (FLASH_GRANULARITY_LARGE);
    }
    else {                                                               // one of the last small sectors
        return (FLASH_GRANULARITY_SMALL);
    }
#endif
}

extern void fnSimNXPFlash(unsigned long commands[], unsigned long results[])
{
    int iStartSector;
    int iEndSector;
    switch (commands[0]) {
    case FLASH_PREPARE_SECTOR_TO_WRITE:
        iStartSector = commands[1];
        iEndSector   = commands[2];
        results[0] = INVALID_SECTOR;
        while (iStartSector <= iEndSector) {
            iSectorsSelected[iStartSector] = 1;                          // mark that the sector has been prepared for the next operation            
            iStartSector++;
            results[0] = CMD_SUCCESS;
        }
        break;
    case FLASH_COPY_RAM_TO_FLASH:
        {
            unsigned char *ptrDestination = (unsigned char *)commands[1];
            unsigned char *ptrSource      = (unsigned char *)commands[2];
            unsigned long ulBytesToWrite  = commands[3];
            unsigned long ulSector;
            if (commands[4] != (MASTER_CLOCK/1000)) {
                _EXCEPTION("Bad clock speed !!");
            }
            if ((CAST_POINTER_ARITHMETIC)ptrSource & 0x03) {
                results[0] = SRC_ADDR_ERROR;
                break;
            }
            if ((CAST_POINTER_ARITHMETIC)ptrDestination & 0x03) {
                results[0] = DST_ADDR_ERROR;
                break;
            }
            ulSector = fnGetSector((unsigned long)ptrDestination);
            if (ulSector >= 28) {
                results[0] = DST_ADDR_NOT_MAPPED;
                break;
            }
            if (iSectorsSelected[ulSector] == 0) {
                results[0] = SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION;
                break;                
            }
            ptrDestination = fnGetFlashAdd(ptrDestination);
            if ((ptrDestination < ucFLASH) || (ptrDestination >= &ucFLASH[SIZE_OF_FLASH])) {
                results[0] = DST_ADDR_NOT_MAPPED;
                break;
            }
            if ((ulBytesToWrite != 256) && (ulBytesToWrite != 512) && (ulBytesToWrite != 1024) && (ulBytesToWrite != 4096)) {
                results[0] = COUNT_ERROR;
                break;
            }
            while (ulBytesToWrite >= FLASH_LINE_SIZE) {                  // program to FLASH (change only bits from 1 to 0) a 16 byte line can not change individual bits
                int iLineLength = 0;
                int iOldLineBlank = 1;
                unsigned char ucOldLine[FLASH_LINE_SIZE];
                unsigned char ucNewLine[FLASH_LINE_SIZE];
                uMemcpy(ucNewLine, ptrSource, FLASH_LINE_SIZE);          // make a copy of old and new FLASH data
                uMemcpy(ucOldLine, ptrDestination, FLASH_LINE_SIZE);
                while (iLineLength < FLASH_LINE_SIZE) {
                    if (ucOldLine[iLineLength++] != 0xff) {
                        iOldLineBlank = 0;
                    }
                }
                if (iOldLineBlank == 0) {                                // check that we only write 1 to non-blank lines
                    iLineLength = 0;
                    while (iLineLength < FLASH_LINE_SIZE) {
                        if (ucNewLine[iLineLength++] != 0xff) {
                            _EXCEPTION("Writing non '1' to a non-blank line !!");
                        }
                    }
                }
                else {
                    uMemcpy(ptrDestination, ptrSource, FLASH_LINE_SIZE); // write the FLASH line - we have checked that only one write to a line tries to change bits to '0'
                }
                ulBytesToWrite -= FLASH_LINE_SIZE;
                ptrDestination += FLASH_LINE_SIZE;
                ptrSource += FLASH_LINE_SIZE;
            }         
            uMemset(iSectorsSelected, 0, sizeof(iSectorsSelected));
            results[0] = CMD_SUCCESS;
        }
        break;
    case FLASH_ERASE_SECTORS:
        {
            unsigned long ulStartSector = commands[1];
            unsigned long ulEndSector   = commands[2];
            if (commands[3] != (MASTER_CLOCK/1000)) {
                 _EXCEPTION("Bad clock speed !!");
            }
            if (ulStartSector >= 28) {
                results[0] = INVALID_SECTOR;
                break;
            }
            while (ulStartSector <= ulEndSector) {
                if (iSectorsSelected[ulStartSector] == 0) {
                    results[0] = SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION;
                    break; 
                }
                uMemset(fnGetFlashAdd(fnGetSectorAdd(ulStartSector)), 0xff, fnGetSectorLength(ulStartSector));
                ulStartSector++;
            }
            uMemset(iSectorsSelected, 0, sizeof(iSectorsSelected));
            results[0] = CMD_SUCCESS;
        }
        break;
    case FLASH_BLANK_CHECK_SECTORS:
        {
            unsigned long ulStartSector = commands[1];
            unsigned long ulEndSector   = commands[2];

            if (ulStartSector >= 28) {
                results[0] = INVALID_SECTOR;
                break;
            }
            while (ulStartSector <= ulEndSector) {
                unsigned char *PtrSectorAddress = fnGetSectorAdd(ulStartSector);
                unsigned long ulSectorSize = fnGetSectorLength(ulStartSector);
                while (ulSectorSize--) {
                    if (*PtrSectorAddress != 0xff) {
                        results[0] = SECTOR_NOT_BLANK;
                        results[1] = (unsigned long)(PtrSectorAddress - fnGetSectorAdd(ulStartSector)); // offset
                        results[2] = *(unsigned long *)((CAST_POINTER_ARITHMETIC)PtrSectorAddress & ~0x3);
                        break; 
                    }
                    PtrSectorAddress++;
                }               
                ulStartSector++;
            }
            results[0] = CMD_SUCCESS;
        }
        break;
#ifndef _LPC21XX
    case FLASH_READ_PART_ID:
        results[1] = PART_IDENTIFICATION_NUMBER;                         // part identification number
        results[0] = CMD_SUCCESS;
        break;
#endif
    case FLASH_READ_BOOT_CODE_VERSION:
        results[1] = 0x00000101;
        results[0] = CMD_SUCCESS;
        break;
    case FLASH_COMPARE:                                                  // not supported becasue we do not simulate RAM
        results[0] = CMD_SUCCESS;
        break;
    case FLASH_REVOKE_ISP:
        break;
    default:
        _EXCEPTION("Invalid IAP command !!");
        break;
    }
}

// Transform physical access address to address in simulated FLASH
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)                // {2}
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    return ucAdd + sim_add;
}

extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)                //  {3}
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)START_OF_FLASH;
    return ucAdd - sim_add;
}

// Periodic tick (on timer 1) - dummy since timer simulation used now
//
extern void RealTimeInterrupt(void)
{
}

// Get the present state of a port - note that devices with Fast GPIO only simulate these
//
extern unsigned long fnGetPresentPortState(int portNr)
{
    portNr--;
    switch (portNr) {
#if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {6}
    case _PORT0:
    #if defined _P0_31_RESTRICTION && (defined LPC2101 || defined LPC2102 || defined LPC2103) // {8} P0.31 is output only
        return (((FIO0DIR | 0x80000000) & FIO0SET) | (~(FIO0DIR | 0x80000000) & ulPort_in_0));// presently only GPIO mode 
    #else
        return ((FIO0DIR & FIO0SET) | (~FIO0DIR & ulPort_in_0));         // presently only GPIO mode                   
    #endif
    case _PORT1:
        return ((FIO1DIR & FIO1SET) | (~FIO1DIR & ulPort_in_1));  
    case _PORT2:
        return ((FIO2DIR & FIO2SET) | (~FIO2DIR & ulPort_in_2));  
    case _PORT3:
        return ((FIO3DIR & FIO3SET) | (~FIO3DIR & ulPort_in_3));  
    case _PORT4:
        return ((FIO4DIR & FIO4SET) | (~FIO4DIR & ulPort_in_4));
#else
    case _PORT0:
        return ((IO0DIR & IO0SET) | (~IO0DIR & ulPort_in_0));            // presently only GPIO mode 
    #ifdef _LPC213X
    case _PORT1:
    return ((IO1DIR & IO1SET) | (~IO1DIR & ulPort_in_1));                // presently only GPIO mode 
    #endif
#endif
    default:
        return 0;
    }
}

// Get the present state of a port direction - note that devices with Fast GPIO only simulate these
//
extern unsigned long fnGetPresentPortDir(int portNr)
{
    portNr--;
    switch (portNr) {
#if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {6}
    case _PORT0:
    #if defined _P0_31_RESTRICTION && (defined LPC2101 || defined LPC2102 || defined LPC2103) // {8}
    return (FIO0DIR | 0x80000000);                                       // P0.31 is output only
    #else
    return (FIO0DIR);
    #endif
    case _PORT1:
    return (FIO1DIR);
    case _PORT2:
    return (FIO2DIR);
    case _PORT3:
    return (FIO3DIR);
    case _PORT4:
    return (FIO4DIR);
#else
    case _PORT0:
    return (IO0DIR);
#endif
    default:
        return 0;
    }
}

static unsigned short fnIsPer(unsigned long ucPinSelValue)
{
    unsigned long ulCheck = 0x00000003;
    unsigned short usPeripheral = 0;
    unsigned short usPeripheralBit = 0x0001;
    while (usPeripheralBit != 0) {
        if ((ucPinSelValue & ulCheck) != 0) {
            usPeripheral |= usPeripheralBit;                             // port has a peripheral function
        }
        usPeripheralBit <<= 1;
        ulCheck <<= 2;
    }
    return usPeripheral;
}

extern unsigned long fnGetPresentPortPeriph(int portNr)
{
    unsigned long ulPeripherals = 0;

    portNr--;
    switch (portNr) {
    case _PORT0:
        ulPeripherals = fnIsPer(PINSEL1);
        ulPeripherals <<= 16;
        ulPeripherals |= fnIsPer(PINSEL0);
        return (ulPeripherals);
    case _PORT1:
        ulPeripherals = fnIsPer(PINSEL3);
        ulPeripherals <<= 16;
        ulPeripherals |= fnIsPer(PINSEL2);
        return (ulPeripherals);
    case _PORT2:
        ulPeripherals = fnIsPer(PINSEL5);
        ulPeripherals <<= 16;
        ulPeripherals |= fnIsPer(PINSEL4);
        return (ulPeripherals);
    case _PORT3:
        ulPeripherals = fnIsPer(PINSEL7);
        ulPeripherals <<= 16;
        ulPeripherals |= fnIsPer(PINSEL6);
        return (ulPeripherals);
    case _PORT4:
        ulPeripherals = fnIsPer(PINSEL9);
        ulPeripherals <<= 16;
        ulPeripherals |= fnIsPer(PINSEL8);
        return (ulPeripherals);
    default:
        return 0;
    }
}

#if defined LPC2478FBD208 || defined LPC2478FET208
static void fnMapLCD_pins(int iPort, int iBit, unsigned char ucFunction)
{
    switch (iPort) {
    case 0:
        if (((iBit >= 4) && (iBit <= 9)) && (ucFunction == 0x01)) {
            ucPortFunctions[iPort][iBit] = 4;                            // LCD function
        }
        break;
    case 1:
        if (((iBit >= 20) && (iBit <= 29)) && (ucFunction == 0x01)) {
            ucPortFunctions[iPort][iBit] = 4;                            // LCD function
        }
        break;
    case 2:
        if (((iBit >= 11) && (iBit <= 13)) && (ucFunction == 0x01)) {
            ucPortFunctions[iPort][iBit] = 4;                            // LCD function
        }
        else if ((iBit <= 9) && (ucFunction == 0x03)) {
            ucPortFunctions[iPort][iBit] = 4;                            // LCD function
        }
        break;
    case 3:
        break;
    case 4:
        if (((iBit >= 28) && (iBit <= 29)) && (ucFunction == 0x02)) {
            ucPortFunctions[iPort][iBit] = 4;                            // LCD function
        }
        break;
    }
}
#endif

static void fnUpdatePeripheralFunction(int iPort, unsigned long ulPortFunction)
{
    int i = 0;
    unsigned long ulPortBit = 0x00000001;
    unsigned long *ptrPinSel = (unsigned long *)PIN_CONNECT_BLOCK;
    unsigned short usShift = 0;

    ptrPinSel += (2*iPort);

    while (ulPortFunction != 0) {
        if (ulPortFunction & ulPortBit) {
            unsigned char ucFunction = (unsigned char)(0x03 & (*ptrPinSel >> usShift));
            ucPortFunctions[iPort][i] = ucFunction;
#if defined LPC2478FBD208 || defined LPC2478FET208
            if (PINSEL11 & LCDPE) {                                      // if the LCD function is enabled map any LCD pins to their LCD use
                fnMapLCD_pins(iPort, i, ucFunction);
            }
#endif
            ulPortFunction &= ~ulPortBit;
        }
        i++;
        ulPortBit <<= 1;
        usShift += 2;
        if (usShift >= 32) {
            usShift = 0;
            ptrPinSel++;
        }
    }
}

#ifdef _LPC21XX                                                          // {10}
// Search through the vectored interrupt slots to find the entry corresponding to a particular interrupt source
//
static unsigned long fnGetVectoredIntSlot(unsigned long ulInterruptSource)
{
    unsigned long *ptrSlots = VICPRIORITY_ADD;
    int i;
    for (i = 0; i < 16; i++) {
        if ((0x1 << (*ptrSlots & 0x1f)) & ulInterruptSource) {
            return *(VICVECT_ADD + i);
        }
        ptrSlots++;
    }
    return 0;                                                            // not found
}
#endif



// See whether there has been a port change which should be displayed
//
extern int fnPortChanges(int iForce)
{
    static unsigned long ulPortDir0 = 0, ulPortDir1 = 0, ulPortDir2 = 0, ulPortDir3 = 0, ulPortDir4 = 0;
    static unsigned long ulPortVal0 = 0, ulPortVal1 = 0, ulPortVal2 = 0, ulPortVal3 = 0, ulPortVal4 = 0;
    static unsigned long ulPortFunction0 = 0, ulPortFunction1 = 0, ulPortFunction2 = 0, ulPortFunction3 = 0, ulPortFunction4 = 0;
    unsigned long ulNewValue;
    unsigned long ulNewPortPer;
    int iRtn = iFlagRefresh;
    iFlagRefresh = 0;

#if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {6}
    ulNewValue = fnGetPresentPortState(_PORT0+1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORT0+1);
    if ((ulPortDir0 != FIO0DIR) || (ulNewValue != ulPortVal0) || (ulNewPortPer != ulPortFunction0)) {
        ulPortDir0 = FIO0DIR;
        ulPortVal0 = ulNewValue;
        ulPortFunction0 = ulNewPortPer;
        fnUpdatePeripheralFunction(_PORT0, ulPortFunction0);
        iRtn |= PORT_CHANGE;
    }
    ulNewValue = fnGetPresentPortState(_PORT1+1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORT1+1);
    if ((ulPortDir1 != FIO1DIR) || (ulNewValue != ulPortVal1) || (ulNewPortPer != ulPortFunction1)) {
        ulPortDir1 = FIO1DIR;
        ulPortVal1 = ulNewValue;
        ulPortFunction1 = ulNewPortPer;
        fnUpdatePeripheralFunction(_PORT1, ulPortFunction1);
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORT2+1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORT2+1);
    if ((ulPortDir2 != FIO2DIR) || (ulNewValue != ulPortVal2) || (ulNewPortPer != ulPortFunction2)) {
        ulPortDir2 = FIO2DIR;
        ulPortVal2 = ulNewValue;
        ulPortFunction2 = ulNewPortPer;
        fnUpdatePeripheralFunction(_PORT2, ulPortFunction2);
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORT3+1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORT3+1);
    if ((ulPortDir3 != FIO3DIR) || (ulNewValue != ulPortVal3) || (ulNewPortPer != ulPortFunction3)) {
        ulPortDir3 = FIO3DIR;
        ulPortVal3 = ulNewValue;
        ulPortFunction3 = ulNewPortPer;
        fnUpdatePeripheralFunction(_PORT3, ulPortFunction3);
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORT4+1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORT4+1);
    if ((ulPortDir4 != FIO4DIR) || (ulNewValue != ulPortVal4) || (ulNewPortPer != ulPortFunction4)) {
        ulPortDir4 = FIO4DIR;
        ulPortVal4 = ulNewValue;
        ulPortFunction4 = ulNewPortPer;
        fnUpdatePeripheralFunction(_PORT4, ulPortFunction4);
        iRtn |= PORT_CHANGE;
    }   
#else
    ulNewValue = fnGetPresentPortState(_PORT0+1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORT0+1);
    if ((ulPortDir0 != IO0DIR) || (ulNewValue != ulPortVal0) || (ulNewPortPer != ulPortFunction0)) {
        ulPortDir0 = IO0DIR;
        ulPortVal0 = ulNewValue;
        ulPortFunction0 = ulNewPortPer;
        fnUpdatePeripheralFunction(_PORT0, ulPortFunction0);
        iRtn |= PORT_CHANGE;
    }
#endif
    return iRtn;
}

// Handle port interrupts on input changes
//
static void fnPortInterrupt(int iPort, unsigned long ulChangedBit)
{
    switch (iPort) {
    case _PORT0:
        if (ulChangedBit & IO0IntEnR & ulPort_in_0) {                    // rising edge on this rising-edge enabled bit
            IO0IntStatR |= (ulChangedBit & IO0IntEnR & ulPort_in_0);     // signal that an interrupt is pending
            IOIntStatus |= P0int;
        }
        if (ulChangedBit & IO0IntEnF & ~ulPort_in_0) {                   // falling edge on this falling-edge enabled bit
            IO0IntStatF |= (ulChangedBit & IO0IntEnF & ~ulPort_in_0);    // signal that an interrupt is pending
            IOIntStatus |= P0int;
        }        
        break;
    case _PORT2:
        if (ulChangedBit & IO2IntEnR & ulPort_in_2) {                    // rising edge on this rising-edge enabled bit
            IO2IntStatR |= (ulChangedBit & IO2IntEnR & ulPort_in_2);     // signal that an interrupt is pending
            IOIntStatus |= P2int;
        }
        if (ulChangedBit & IO2IntEnF & ~ulPort_in_2) {                   // falling edge on this falling-edge enabled bit
            IO2IntStatF |= (ulChangedBit & IO2IntEnF & ~ulPort_in_2);    // signal that an interrupt is pending
            IOIntStatus |= P2int;
        }
#ifndef _LPC21XX                                                         // edge interrupts handled on LPC23XX
        if (ulChangedBit & PORT2_BIT10) {                                // handle external interrupts on EINT0
            if ((PINSEL4 & PINSEL4_P2_10_RESET) == PINSEL4_P2_10_EINT0) {// configured as EINT0 pin
                if ((ulPort_in_2 & PORT2_BIT10) && (EXTPOLAR & EXTMODE & EXTPOLAR0)) { // positive edge sensitive and change detected
                    EXTINT |= EINT0;
                }
                else if ((!(ulPort_in_2 & PORT2_BIT10)) && (EXTMODE & EXTMODE0) && (!(EXTPOLAR & EXTPOLAR0))) {// negative edge sensitive and change detected
                    EXTINT |= EINT0;
                }
            }
            if (EXTINT & EINT0) {
                if (VICIntEnable & INTERRUPT_EINT0) {                    // if the interrupt is enabled
                    void (*interrupt_call)(void);
                    interrupt_call = (void (*)(void))VICVectAddr14;
                    interrupt_call();                                    // call interrupt handler
                }
            }
        }
        if (ulChangedBit & PORT2_BIT11) {                                // handle external interrupts on EINT1
            if ((PINSEL4 & PINSEL4_P2_11_RESET) == PINSEL4_P2_11_EINT1) {// configured as EINT1 pin
                if ((ulPort_in_2 & PORT2_BIT11) && (EXTPOLAR & EXTMODE & EXTPOLAR1)) { // positive edge sensitive and change detected
                    EXTINT |= EINT1;
                }
                else if ((!(ulPort_in_2 & PORT2_BIT11)) && (EXTMODE & EXTMODE1) && (!(EXTPOLAR & EXTPOLAR1))) {// negative edge sensitive and change detected
                    EXTINT |= EINT1;
                }
            }
            if (EXTINT & EINT1) {
                if (VICIntEnable & INTERRUPT_EINT1) {                    // if the interrupt is enabled
                    void (*interrupt_call)(void);
                    interrupt_call = (void (*)(void))VICVectAddr15;
                    interrupt_call();                                    // call interrupt handler
                }
            }
        }
        if (ulChangedBit & PORT2_BIT12) {                                // handle external interrupts on EINT2
            if ((PINSEL4 & PINSEL4_P2_12_RESET) == PINSEL4_P2_12_EINT2) {// configured as EINT2 pin
                if ((ulPort_in_2 & PORT2_BIT12) && (EXTPOLAR & EXTMODE & EXTPOLAR2)) { // positive edge sensitive and change detected
                    EXTINT |= EINT2;
                }
                else if ((!(ulPort_in_2 & PORT2_BIT12)) && (EXTMODE & EXTMODE2) && (!(EXTPOLAR & EXTPOLAR2))) {// negative edge sensitive and change detected
                    EXTINT |= EINT2;
                }
            }
            if (EXTINT & EINT2) {
                if (VICIntEnable & INTERRUPT_EINT2) {                    // if the interrupt is enabled
                    void (*interrupt_call)(void);
                    interrupt_call = (void (*)(void))VICVectAddr16;
                    interrupt_call();                                    // call interrupt handler
                }
            }
        }
        if (ulChangedBit & PORT2_BIT13) {                                // handle external interrupts on EINT3
            if ((PINSEL4 & PINSEL4_P2_13_RESET) == PINSEL4_P2_13_EINT3) {// configured as EINT3 pin
                if ((ulPort_in_2 & PORT2_BIT13) && (EXTPOLAR & EXTMODE & EXTPOLAR3)) { // positive edge sensitive and change detected
                    EXTINT |= EINT3;
                }
                else if ((!(ulPort_in_2 & PORT2_BIT13)) && (EXTMODE & EXTMODE3) && (!(EXTPOLAR & EXTPOLAR3))) {// negative edge sensitive and change detected
                    EXTINT |= EINT3;
                }
            }
        }
#endif
        break;
    }
    if ( 
#ifndef _LPC21XX
        (EXTINT & EINT3) ||                                              // external interrupt 3 shares an interrupt with the GPO ports
#endif
        (IOIntStatus & (P0int | P2int))) {
        if (VICIntEnable & INTERRUPT_EINT3) {                            // if the interrupt is enabled
            void (*interrupt_call)(void);
#ifdef _LPC21XX
            interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_EINT3);
#else
            interrupt_call = (void (*)(void))VICVectAddr17;
#endif
            interrupt_call();                                            // call interrupt handler
        }
    }
}

// Simulate setting, clearing or toggling of an input pin
//
extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    unsigned long ulBit = (0x80000000 >> ucPortBit);
    switch (ucPort) {
#if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {6}
    case _PORT0:
    #if defined _P0_31_RESTRICTION && (defined LPC2101 || defined LPC2102 || defined LPC2103) // {8}
        if (~FIO0DIR & ulBit & ~0x80000000)
    #else
        if (~FIO0DIR & ulBit)
    #endif
        {
            unsigned long ulOriginal_port_state = ulPort_in_0;
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_0 ^= ulBit;
                FIO0PIN &= ~ulBit;
                FIO0PIN |= (ulPort_in_0 & ulBit);
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_0 |= ulBit;
                FIO0PIN |= ulBit;
            }
            else {
                ulPort_in_0 &= ~ulBit;
                FIO0PIN &= ~ulBit;
            }
            if (ulPort_in_0 != ulOriginal_port_state) {
                fnPortInterrupt(_PORT0, ulBit);
            }
        }
        break;
    case _PORT1:
        if (~FIO1DIR & ulBit) {
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_1 ^= ulBit;
                FIO1PIN &= ~ulBit;
                FIO1PIN |= (ulPort_in_1 & ulBit);
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_1 |= ulBit;
                FIO1PIN |= ulBit;
            }
            else {
                ulPort_in_1 &= ~ulBit;
                FIO1PIN &= ~ulBit;
            }
        }
        break;
    case _PORT2:
        {
            unsigned long ulOriginal_port_state = ulPort_in_2;
            if (!(~FIO2DIR & ulBit)) {                                   // {11} accept input change if an input or an external interrupt
                if ((ulBit == PORT2_BIT10) && ((PINSEL4 & PINSEL4_P2_10_RESET) == PINSEL4_P2_10_EINT0)) {
                }
                else if ((ulBit == PORT2_BIT11) && ((PINSEL4 & PINSEL4_P2_11_RESET) == PINSEL4_P2_11_EINT1)) {
                }
                else if ((ulBit == PORT2_BIT12) && ((PINSEL4 & PINSEL4_P2_12_RESET) == PINSEL4_P2_12_EINT2)) {
                }
                else if ((ulBit == PORT2_BIT13) && ((PINSEL4 & PINSEL4_P2_13_RESET) == PINSEL4_P2_13_EINT3)) {
                }
                else {
                    break;
                }
            }
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_2 ^= ulBit;
                FIO2PIN &= ~ulBit;
                FIO2PIN |= (ulPort_in_2 & ulBit);
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_2 |= ulBit;
                FIO2PIN |= ulBit;
            }
            else {
                ulPort_in_2 &= ~ulBit;
                FIO2PIN &= ~ulBit;
            }
            if (ulPort_in_2 != ulOriginal_port_state) {
                fnPortInterrupt(_PORT2, ulBit);
            }
        }
        break;
    case _PORT3:
        if (~FIO3DIR & ulBit) {
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_3 ^= ulBit;
                FIO3PIN &= ~ulBit;
                FIO3PIN |= (ulPort_in_3 & ulBit);
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_3 |= ulBit;
                FIO3PIN |= ulBit;
            }
            else {
                ulPort_in_3 &= ~ulBit;
                FIO3PIN &= ~ulBit;
            }
        }
        break;
    case _PORT4:
        if (~FIO4DIR & ulBit) {
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_4 ^= ulBit;
                FIO4PIN &= ~ulBit;
                FIO4PIN |= (ulPort_in_4 & ulBit);
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_4 |= ulBit;
                FIO4PIN |= ulBit;
            }
            else {
                ulPort_in_4 &= ~ulBit;
                FIO4PIN &= ~ulBit;
            }
        }
        break;
#else
    case _PORT0:
        if (~IO0DIR & ulBit) {
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_0 ^= ulBit;
                IO0PIN &= ~ulBit;
                IO0PIN |= (ulPort_in_0 & ulBit);
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_0 |= ulBit;
                IO0PIN |= ulBit;
            }
            else {
                ulPort_in_0 &= ~ulBit;
                IO0PIN &= ~ulBit;
            }
        }
        break;
#endif
    }
}

// Update ports based on present register settings
//
extern void fnSimPorts(void)
{
    static unsigned long ulOutput0 = 0, ulOutput1 = 0, ulOutput2 = 0, ulOutput3 = 0, ulOutput4 = 0;
    unsigned long ulNewState;

#if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {6}
    ulNewState = ulOutput0 & (~(FIO0CLR & ~FIO0MASK));
    FIO0CLR = 0;
    ulNewState |= (FIO0SET & ~ulOutput0 & ~FIO0MASK);
    FIO0SET = ulOutput0 = ulNewState;
    FIO0PIN = FIO0DIR & FIO0SET | (~FIO0DIR & ulPort_in_0);
    #ifndef _LPC21XX                                                     // {3}
    ulNewState = ulOutput1 & (~(FIO1CLR & ~FIO1MASK));
    FIO1CLR = 0;
    ulNewState |= (FIO1SET & ~ulOutput1 & ~FIO1MASK);
    FIO1SET = ulOutput1 = ulNewState;
    FIO1PIN = FIO1DIR & FIO1SET | (~FIO1DIR & ulPort_in_1);

    ulNewState = ulOutput2 & (~(FIO2CLR & ~FIO2MASK));
    FIO2CLR = 0;
    ulNewState |= (FIO2SET & ~ulOutput2 & ~FIO2MASK);
    FIO2SET = ulOutput2 = ulNewState;
    FIO2PIN = FIO2DIR & FIO2SET | (~FIO2DIR & ulPort_in_2);
    FIO2CLR = 0;

    ulNewState = ulOutput3 & (~(FIO3CLR & ~FIO3MASK));
    FIO3CLR = 0;
    ulNewState |= (FIO3SET & ~ulOutput3 & ~FIO3MASK);
    FIO3SET = ulOutput3 = ulNewState;
    FIO3PIN = FIO3DIR & FIO3SET | (~FIO3DIR & ulPort_in_3);

    ulNewState = ulOutput4 & (~(FIO4CLR & ~FIO4MASK));
    FIO4CLR = 0;
    ulNewState |= (FIO4SET & ~ulOutput4 & ~FIO4MASK);
    FIO4SET = ulOutput4 = ulNewState;
    FIO4PIN = FIO4DIR & FIO4SET | (~FIO4DIR & ulPort_in_4);
    #endif
#elif defined _LPC213X
    ulNewState = ulOutput0 & (~(IO0CLR));
    IO0CLR = 0;
    ulNewState |= (IO0SET & ~ulOutput0);
    IO0SET = ulOutput0 = ulNewState;
    IO0PIN = IO0DIR & IO0SET | (~IO0DIR & ulPort_in_0);

    ulNewState = ulOutput1 & (~(IO1CLR));
    IO1CLR = 0;
    ulNewState |= (IO1SET & ~ulOutput1);
    IO1SET = ulOutput1 = ulNewState;
    IO1PIN = IO1DIR & IO1SET | (~IO1DIR & ulPort_in_1);
#else
    ulNewState = ulOutput0 & (~(IO0CLR & ~IO0MASK));
    IO0CLR = 0;
    ulNewState |= (IO0SET & ~ulOutput0 & ~IO0MASK);
    IO0SET = ulOutput0 = ulNewState;
    IO0PIN = IO0DIR & IO0SET | (~IO0DIR & ulPort_in_0);
#endif

#if CHIP_HAS_UARTS > 0 && defined RTS_0_PIN                              // RTS outputs
    if (RTS_0_PORT_DDR & RTS_0_PIN) {                                    // user defined RTS pin configured as output
        static unsigned char ucRTS0 = 1;
        if (RTS_0_PORT & RTS_0_PIN) {
            if (ucRTS0 == 0) {
                ucRTS0 = 1;
                fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                  // negate control line
            }
        }
        else {
            if (ucRTS0 != 0) {
                ucRTS0 = 0;
                fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                  // assert control line
            }
        }
    }
#endif
#if CHIP_HAS_UARTS > 1 && defined RTS_1_PIN                              // RTS outputs
    if (RTS_1_PORT_DDR & RTS_1_PIN) {                                    // user defined RTS pin configured as output
        static unsigned char ucRTS1 = 1;
        if (RTS_1_PORT & RTS_1_PIN) {
            if (ucRTS1 == 0) {
                ucRTS1 = 1;
                fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                  // negate control line
            }
        }
        else {
            if (ucRTS1 != 0) {
                ucRTS1 = 0;
                fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                  // assert control line
            }
        }
    }
#endif
#if CHIP_HAS_UARTS > 2 && defined RTS_2_PIN                              // RTS outputs
    if (RTS_2_PORT_DDR & RTS_2_PIN) {                                    // user defined RTS pin configured as output
        static unsigned char ucRTS2 = 1;
        if (RTS_2_PORT & RTS_2_PIN) {
            if (ucRTS2 == 0) {
                ucRTS2 = 1;
                fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);                  // negate control line
            }
        }
        else {
            if (ucRTS2 != 0) {
                ucRTS2 = 0;
                fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);                  // assert control line
            }
        }
    }
#endif
#if CHIP_HAS_UARTS > 3 && defined RTS_3_PIN                              // RTS outputs
    if (RTS_3_PORT_DDR & RTS_3_PIN) {                                    // user defined RTS pin configured as output
        static unsigned char ucRTS3 = 1;
        if (RTS_3_PORT & RTS_3_PIN) {
            if (ucRTS3 == 0) {
                ucRTS3 = 1;
                fnConfigSimSCI(3, NEGATE_RTS_COM_3, 0);                  // negate control line
            }
        }
        else {
            if (ucRTS3 != 0) {
                ucRTS3 = 0;
                fnConfigSimSCI(3, ASSERT_RTS_COM_3, 0);                  // assert control line
            }
        }
    }
#endif
}


// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
#ifdef SERIAL_INTERFACE
    void (*interrupt_call)(void);
    switch (iPort) {
    case 0:
        if (U0FCR & FIFO_ENABLE) {
            U0IIR |= UART_FIFO_ENABLED;
        }
        else {
            return;
        }
        while (usLen--) {
            U0LSR |= UART_RDR;                                           // line status
            U0IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
            U0IIR |= UART_RDA_INT_PENDING;                               // set interrupt cause
	        U0RBR_THR = *ptrDebugIn++;                                   // put each character into the receive holding register
            if (VICIntEnable & INTERRUPT_UART0) {                        // if the interrupt is defined
    #ifdef _LPC21XX
                interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_UART0);
    #else
                interrupt_call = (void (*)(void))VICVectAddr6;
    #endif
                interrupt_call();                                        // call interrupt handler
            }
        }
        break;
    case 1:
        if (U1FCR & FIFO_ENABLE) {
            U1IIR |= UART_FIFO_ENABLED;
        }
        else {
            return;
        }
        while (usLen--) {
            U1LSR |= UART_RDR;                                           // line status
            U1IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
            U1IIR |= UART_RDA_INT_PENDING;                               // set interrupt cause
	        U1RBR_THR = *ptrDebugIn++;                                   // put each character into the receive holding register
            if (VICIntEnable & INTERRUPT_UART1) {                        // if the interrupt is defined
    #ifdef _LPC21XX
                interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_UART1);
    #else
                interrupt_call = (void (*)(void))VICVectAddr7;
    #endif
                interrupt_call();                                        // call interrupt handler
            }
        }
        break;
    case 2:
        if (U2FCR & FIFO_ENABLE) {
            U2IIR |= UART_FIFO_ENABLED;
        }
        else {
            return;
        }
        while (usLen--) {
            U2LSR |= UART_RDR;                                           // line status
            U2IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
            U2IIR |= UART_RDA_INT_PENDING;                               // set interrupt cause
	        U2RBR_THR = *ptrDebugIn++;                                   // put each character into the receive holding register
            if (VICIntEnable & INTERRUPT_UART2) {                        // if the interrupt is defined
                interrupt_call = (void (*)(void))VICVectAddr28;
                interrupt_call();                                        // call interrupt handler
            }
        }
        break;
    case 3:
        if (U3FCR & FIFO_ENABLE) {
            U3IIR |= UART_FIFO_ENABLED;
        }
        else {
            return;
        }
        while (usLen--) {
            U3LSR |= UART_RDR;                                           // line status
            U3IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
            U3IIR |= UART_RDA_INT_PENDING;                               // set interrupt cause
	        U3RBR_THR = *ptrDebugIn++;                                   // put each character into the receive holding register
            if (VICIntEnable & INTERRUPT_UART3) {                        // if the interrupt is defined
                interrupt_call = (void (*)(void))VICVectAddr29;
                interrupt_call();                                        // call interrupt handler
            }
        }
        break;
    }
#endif
}

// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{
#ifdef SERIAL_INTERFACE
    extern unsigned char ucTxLast[NUMBER_SERIAL];
#endif
    unsigned long ulNewActions = 0;
    int *ptrCnt;

    if ((iInts & CHANNEL_0_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#ifdef SERIAL_INTERFACE
                void (*interrupt_call)(void);
#endif
		        iInts &= ~CHANNEL_0_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx0(ucTxLast[0]);
                ulNewActions |= SEND_COM_0;
                U0LSR |= UART_THRE;                                      // line status
                U0IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
                U0IIR |= UART_THRE_INT_PENDING;                          // set interrupt cause
                if (VICIntEnable & INTERRUPT_UART0) {                    // if the interrupt is defined
    #ifdef _LPC21XX
                    interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_UART0);
    #else
                    interrupt_call = (void (*)(void))VICVectAddr6;
    #endif
                    interrupt_call();                                    // call interrupt handler
                }
#endif
            }
        }
	}

    if ((iInts & CHANNEL_1_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == THROUGHPUT_UART1) {
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#ifdef SERIAL_INTERFACE
                void (*interrupt_call)(void);
#endif
		        iInts &= ~CHANNEL_1_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx1(ucTxLast[1]);
                ulNewActions |= SEND_COM_1;
                U1LSR |= UART_THRE;                                      // line status
                U1IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
                U1IIR |= UART_THRE_INT_PENDING;                          // set interrupt cause
                if (VICIntEnable & INTERRUPT_UART1) {                    // if the interrupt is defined
    #ifdef _LPC21XX
                    interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_UART1); // {13}
    #else
                    interrupt_call = (void (*)(void))VICVectAddr7;
    #endif
                    interrupt_call();                                    // call interrupt handler
                }
#endif
            }
        }
	}

    if ((iInts & CHANNEL_2_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART2];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#ifdef SERIAL_INTERFACE
                void (*interrupt_call)(void);
#endif
		        iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx2(ucTxLast[2]);
                ulNewActions |= SEND_COM_2;
                U2LSR |= UART_THRE;                                      // line status
                U2IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
                U2IIR |= UART_THRE_INT_PENDING;                          // set interrupt cause
                if (VICIntEnable & INTERRUPT_UART2) {                    // if the interrupt is defined
                    interrupt_call = (void (*)(void))VICVectAddr28;
                    interrupt_call();                                    // call interrupt handler
                }
#endif
            }
        }
	}

    if ((iInts & CHANNEL_3_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART3];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#ifdef SERIAL_INTERFACE
                void (*interrupt_call)(void);
#endif
		        iInts &= ~CHANNEL_3_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx3(ucTxLast[3]);
                ulNewActions |= SEND_COM_3;
                U3LSR |= UART_THRE;                                      // line status
                U3IIR &= ~(UART_NO_INT_PENDING | UART_INT_MASK);
                U3IIR |= UART_THRE_INT_PENDING;                          // set interrupt cause
                if (VICIntEnable & INTERRUPT_UART3) {                    // if the interrupt is defined
                    interrupt_call = (void (*)(void))VICVectAddr29;
                    interrupt_call();                                    // call interrupt handler
                }
#endif
            }
        }
	}

    if ((iInts & IIC_INT0) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= IIC_INT0;                                      // enough IIC interupts handled in this tick period
            }
            else {
		        iInts &= ~IIC_INT0;                                      // interrupt has been handled
#ifdef IIC_INTERFACE
                if (I2C0STAT != I2C_IDLE_STATE) {                        // if a state generating an interrupt
                    if (VICIntEnable & INTERRUPT_I2C0) {                 // if the interrupt is defined
                        void (*interrupt_call)(void);
    #ifdef _LPC21XX
                        interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_I2C0);
    #else
                        interrupt_call = (void (*)(void))VICVectAddr9;
    #endif
                        interrupt_call();                                // call interrupt handler
                    }
                }
#endif
            }
        }
	}

    if ((iInts & IIC_INT1) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= IIC_INT1;                                      // enough IIC interupts handled in this tick period
            }
            else {
		        iInts &= ~IIC_INT1;                                      // interrupt has been handled
#ifdef IIC_INTERFACE
                if (I2C1STAT != I2C_IDLE_STATE) {                        // if a state generating an interrupt
                    if (VICIntEnable & INTERRUPT_I2C1) {                 // if the interrupt is defined
                        void (*interrupt_call)(void);
    #ifdef _LPC21XX
                        interrupt_call = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_I2C1);
    #else
                        interrupt_call = (void (*)(void))VICVectAddr19;
    #endif
                        interrupt_call();                                // call interrupt handler
                    }
                }
#endif
            }
        }
	}

    if ((iInts & IIC_INT2) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C2];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= IIC_INT2;                                      // enough IIC interupts handled in this tick period
            }
            else {
		        iInts &= ~IIC_INT2;                                      // interrupt has been handled
#ifdef IIC_INTERFACE
                if (I2C2STAT != I2C_IDLE_STATE) {                        // if a state generating an interrupt
                    if (VICIntEnable & INTERRUPT_I2C2) {                 // if the interrupt is defined
                        void (*interrupt_call)(void);
                        interrupt_call = (void (*)(void))VICVectAddr30;
                        interrupt_call();                                // call interrupt handler
                    }
                }
#endif
            }
        }
	}

#ifdef USB_INTERFACE                                                     // {14}
    if (iInts & USB_INT) {
        int iEndpoint = 0;
        iInts &= ~USB_INT;
        while ((iEndpoint < NUMBER_OF_USB_ENDPOINTS) && (ulEndpointInt != 0)) {
            if (ulEndpointInt & (1 << iEndpoint)) {
                ulEndpointInt &= ~(1 << iEndpoint);
                fnCheckUSBOut(0, iEndpoint);
            }
            iEndpoint++;
        }
    }
#endif
    return ulNewActions;
}

extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState)
{
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect
#ifdef SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);
    switch (iPort) {
    case 0:
    #ifdef CTS_0_PIN
        if (ulChange & MS_CTS_ON) {                                      // CTS change - set the state in the corresponding register and generate interrupt, if enabled
            unsigned long ulPortBit = CTS_0_PIN;
            unsigned char ucInput = 31;
            while (!(ulPortBit & 0x01)) {
                ucInput--;
                ulPortBit >>= 1;
            }
            if (ulNewState & MS_CTS_ON) {                                // CTS has just been activated
                fnSimulateInputChange(CTS_0_PORT, ucInput, CLEAR_INPUT);
            }
            else {                                                       // CTS has just been deactivated
                fnSimulateInputChange(CTS_0_PORT, ucInput, SET_INPUT);
            }
        }
    #endif
        break;
    case 1:
    #ifdef CTS_1_PIN
        if (ulChange & MS_CTS_ON) {                                      // CTS change - set the state in the corresponding register and generate interrupt, if enabled
            unsigned char ucPortBit = CTS_1_PIN;
            unsigned char ucInput = 7;
            while (!(ucPortBit & 0x01)) {
                ucInput--;
                ucPortBit >>= 1;
            }
            if (ulNewState & MS_CTS_ON) {                                // CTS has just been activated
                fnSimulateInputChange(CTS_1_PORT, ucInput, CLEAR_INPUT);
            }
            else {                                                       // CTS has just been deactivated
                fnSimulateInputChange(CTS_1_PORT, ucInput, SET_INPUT);
            }
        }
        break;
    #endif
    #ifdef CTS_2_PIN
    case 2:
        if (ulChange & MS_CTS_ON) {                                      // CTS change - set the state in the corresponding register and generate interrupt, if enabled
            unsigned char ucPortBit = CTS_2_PIN;
            unsigned char ucInput = 7;
            while (!(ucPortBit & 0x01)) {
                ucInput--;
                ucPortBit >>= 1;
            }
            if (ulNewState & MS_CTS_ON) {                                // CTS has just been activated
                fnSimulateInputChange(CTS_2_PORT, ucInput, CLEAR_INPUT);
            }
            else {                                                       // CTS has just been deactivated
                fnSimulateInputChange(CTS_2_PORT, ucInput, SET_INPUT);
            }
        }
        break;
    #endif
    #ifdef CTS_3_PIN
    case 3:
        if (ulChange & MS_CTS_ON) {                                      // CTS change - set the state in the corresponding register and generate interrupt, if enabled
            unsigned char ucPortBit = CTS_3_PIN;
            unsigned char ucInput = 7;
            while (!(ucPortBit & 0x01)) {
                ucInput--;
                ucPortBit >>= 1;
            }
            if (ulNewState & MS_CTS_ON) {                                // CTS has just been activated
                fnSimulateInputChange(CTS_3_PORT, ucInput, CLEAR_INPUT);
            }
            else {                                                       // CTS has just been deactivated
                fnSimulateInputChange(CTS_3_PORT, ucInput, SET_INPUT);
            }
        }
        break;
    #endif
    }
#endif
}

// UART Break detection simulation
//
extern void fnSimulateBreak(int iPort)
{
}

// Process simulated DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    return 0;
}


extern void fnSimulateLinkUp(void)
{
#ifdef ETH_INTERFACE
    #ifdef PHY_INTERRUPT
    unsigned char ucBit = 31;
    unsigned long ulBit = PHY_INT_PIN;
    while (ulBit > 1) {
        ulBit >>= 1;
        ucBit--;
    }
    fnSimulateInputChange(PHY_INT_PORT, ucBit, CLEAR_INPUT);
    #endif
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}

#ifdef SUPPORT_RTC                                                       // {4}
// Synchronise the internal RTC to the PC time when simulating
//
extern void fnInitInternalRTC(char *argv[])
{
    unsigned short *ptrShort = (unsigned short *)*argv;

    RTC_YEAR = *ptrShort++;
    RTC_MONTH = *ptrShort++;
    RTC_DOW = *ptrShort++;
    RTC_DOM = *ptrShort++;
    RTC_HOUR = *ptrShort++;
    RTC_MIN = *ptrShort++;
    RTC_SEC = *ptrShort++;
    RTC_DOY = ((RTC_MONTH * 30) + RTC_DOM);                              // approximate day of year

    RTC_CTIME0 = (RTC_SEC | (RTC_MIN << 8) | (RTC_HOUR << 16) | (RTC_DOW << 24));
    RTC_CTIME1 = (RTC_DOM | (RTC_MONTH << 8) | (RTC_YEAR << 16));
    RTC_CTIME2 = RTC_DOY;

    RTC_CCR = (CCR_CLKSRC_32K | CCR_CLKEN);                              // enable the RTC (this assumes that it was previously enabled before last power down and running from 32kHz clock, which is the general case)
}
#endif


// We can simulate timers during a tick here if required
//
extern int fnSimTimers(void)
{
    // Timer 0..3 simulation. These assume no prescaler and only match on internal match channel 0
    //
    if (T0TCR & COUNTER_ENABLE) {                                        // ensure enabled
#if defined LPC2101 || defined LPC2102 || defined LPC2103
        if (!(PWM0CON & (PWM_MAT0_ENABLE | PWM_MAT1_ENABLE | PWM_MAT2_ENABLE | PWM_MAT3_ENABLE))) { // ignore if in PWM mode
#endif
        unsigned long ulTickCount = 0;
        switch (PCLKSEL0 & PCLK_TIMER0_CCLK_8) {
        case PCLK_TIMER0_CCLK_1:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(1*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER0_CCLK_2:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(2*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER0_CCLK_4:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(4*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER0_CCLK_8:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(8*1000)); // clock pulses in tick period
            break;
        }
        if (T0PR != 0) {                                                 // {16}
            ulTickCount /= (T0PR + 1);
        }
        if ((T0TC + ulTickCount) >= T0MR0) {                             // match with register 0
            if (T0MCR & MR0R) {                                          // reset on match
                T0TC = 0;
            }
            else {                                                       // {16}
                T0TC += ulTickCount;
            }
            if (T0MCR & MR0S) {                                          // stop on match
                T0TCR = 0;
            }
            if (VICIntEnable & INTERRUPT_TIMER0) {                       // ensure interrupt enabled
#ifdef _LPC21XX
                void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_TIMER0);
#else
                void (*interrupt_call)(void) = (void (*)(void))VICVectAddr4;
#endif
                interrupt_call();                                        // call interrupt handler
            }
        }
        else {
            T0TC += ulTickCount;
        }
#if defined LPC2101 || defined LPC2102 || defined LPC2103
        }
#endif
    }

    if (T1TCR & COUNTER_ENABLE) {                                        // ensure enabled
#if defined LPC2101 || defined LPC2102 || defined LPC2103
        if (!(PWM1CON & (PWM_MAT0_ENABLE | PWM_MAT1_ENABLE | PWM_MAT2_ENABLE | PWM_MAT3_ENABLE))) { // ignore if in PWM mode
#endif
        unsigned long ulTickCount = 0;
        switch (PCLKSEL0 & PCLK_TIMER1_CCLK_8) {
        case PCLK_TIMER1_CCLK_1:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(1*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER1_CCLK_2:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(2*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER1_CCLK_4:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(4*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER1_CCLK_8:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(8*1000)); // clock pulses in tick period
            break;
        }
        if (T1PR != 0) {                                                 // {16}
            ulTickCount /= (T1PR + 1);
        }
        if ((T1TC + ulTickCount) >= T1MR0) {                             // match with register 0
            if (T1MCR & MR0R) {                                          // reset on match
                T1TC = 0;
            }
            else {                                                       // {16}
                T1TC += ulTickCount;
            }
            if (T1MCR & MR0S) {                                          // stop on match
                T1TCR = 0;
            }
            if (VICIntEnable & INTERRUPT_TIMER1) {                       // ensure interrupt enabled
#ifdef _LPC21XX
                void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_TIMER1);
#else
                void (*interrupt_call)(void) = (void (*)(void))VICVectAddr5;
#endif
                interrupt_call();                                        // call interrupt handler
            }
        }
        else {                                                           // {9}
            T1TC += ulTickCount;
        }
#if defined LPC2101 || defined LPC2102 || defined LPC2103
        }
#endif
    }

#if TIMERS_AVAILABLE > 2                                                 // {12} LPC2148 has only timers 0 and 1
    if (T2TCR & COUNTER_ENABLE) {                                        // ensure enabled
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        if (!(PWM2CON & (PWM_MAT0_ENABLE | PWM_MAT1_ENABLE | PWM_MAT2_ENABLE | PWM_MAT3_ENABLE))) { // ignore if in PWM mode
    #endif
        unsigned long ulTickCount = 0;
        switch (PCLKSEL1 & PCLK_TIMER2_CCLK_8) {
        case PCLK_TIMER2_CCLK_1:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(1*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER2_CCLK_2:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(2*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER2_CCLK_4:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(4*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER2_CCLK_8:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(8*1000)); // clock pulses in tick period
            break;
        }
        if (T2PR != 0) {                                                 // {16}
            ulTickCount /= (T2PR + 1);
        }
        if ((T2TC + ulTickCount) >= T2MR0) {                             // match with register 0
            if (T2MCR & MR0R) {                                          // reset on match
                T2TC = 0;
            }
            else {
                T2TC += ulTickCount;                                     // {16}
            }
            if (T2MCR & MR0S) {                                          // stop on match
                T2TCR = 0;
            }
            if (VICIntEnable & INTERRUPT_TIMER2) {                       // ensure interrupt enabled
    #ifdef _LPC21XX
                void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_TIMER2);
    #else
                void (*interrupt_call)(void) = (void (*)(void))VICVectAddr26;
    #endif
                interrupt_call();                                        // call interrupt handler
            }
        }
        else {                                                           // {9}
            T2TC += ulTickCount;
        }
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        }
    #endif
    }

    if (T3TCR & COUNTER_ENABLE) {                                        // ensure enabled
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        if (!(PWM3CON & (PWM_MAT0_ENABLE | PWM_MAT1_ENABLE | PWM_MAT2_ENABLE | PWM_MAT3_ENABLE))) { // ignore if in PWM mode
    #endif
        unsigned long ulTickCount = 0;
        switch (PCLKSEL1 & PCLK_TIMER2_CCLK_8) {
        case PCLK_TIMER3_CCLK_1:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(1*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER3_CCLK_2:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(2*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER3_CCLK_4:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(4*1000)); // clock pulses in tick period
            break;
        case PCLK_TIMER3_CCLK_8:
            ulTickCount = (unsigned long)((float)((float)TICK_RESOLUTION*(float)PCLK)/(float)(8*1000)); // clock pulses in tick period
            break;
        }
        if (T3PR != 0) {                                                 // {16}
            ulTickCount /= (T3PR + 1);
        }
        if ((T3TC + ulTickCount) >= T3MR0) {                             // match with register 0
            if (T3MCR & MR0R) {                                          // reset on match
                T3TC = 0;
            }
            else {                                                       // {16}
                T3TC += ulTickCount;
            }
            if (T3MCR & MR0S) {                                          // stop on match
                T3TCR = 0;
            }
            if (VICIntEnable & INTERRUPT_TIMER3) {                       // ensure interrupt enabled
    #ifdef _LPC21XX
                void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_TIMER3);
    #else
                void (*interrupt_call)(void) = (void (*)(void))VICVectAddr27;
    #endif
                interrupt_call();                                        // call interrupt handler
            }
        }
        else {                                                           // {9}
            T3TC += ulTickCount;
        }
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        }
    #endif
    }
#endif
    if (WDMOD & WDEN) {
        if (WDFEED == 0x55) {                                            // recognised as retrigger
            WDTV = WDTC;                                                 // reload watchdog counter since retriggered
            WDFEED = 0x12345678;
        }
        else {
            unsigned long ulTickCount;
            switch (WDCLKSEL) {
            case WD_CLOCK_INT_RC:                                        // not supported
            case WD_CLOCK_RTC:
                WDTV = 0;
                break;
            case WD_CLOCK_PCLK:
                switch (PCLKSEL0 & PCLK_WDT_CCLK_8) {
                    case PCLK_WDT_CCLK_1:
                        ulTickCount = ((PCLK/1000)*TICK_RESOLUTION);
                        break;
                    case PCLK_WDT_CCLK_2:
                        ulTickCount = ((PCLK/2000)*TICK_RESOLUTION);
                        break;
                    case PCLK_WDT_CCLK_4:
                        ulTickCount = ((PCLK/4000)*TICK_RESOLUTION);
                        break;
                    case PCLK_WDT_CCLK_8:
                        ulTickCount = ((PCLK/8000)*TICK_RESOLUTION);
                        break;
                }
                if (WDTV >= ulTickCount) {
                    WDTV -= ulTickCount;
                }
                else {
                    WDTV = 0;
                }
                break;
            }
            if (WDTV == 0) {
                if (WDRESET & WDMOD) {                                   // programmed to generate reset on timeout
                    return RESET_CARD_WATCHDOG;                          // signal watchdog reset
                }
            }
        }
    }
#ifdef SUPPORT_RTC                                                       // {4}
    if (RTC_CCR & CCR_CLKEN) {                                           // if RTC is enabled
        if (RTC_CCR & CCR_CLKSRC_32K) {                                  // if clocked from 32kHz
            RTC_CTC += (32768/(1000/TICK_RESOLUTION));                   // the count per TICK based on 32kHz source
        }
        else {
            RTC_CTC += ((PCLK/(RTC_PREINT + 1))/(1000/TICK_RESOLUTION)); // the count per TICK
        }
        if (RTC_CTC >= 32768) {                                          // one second count
            RTC_CTC = 0;
            (RTC_SEC)++;
            if (RTC_CIIR & CIIR_IMSEC) {                                 // interrupt on each second increment
                RTC_ILR |= RTCCIF;                                       // set the increment interrupt flag
            }
            if (RTC_SEC >= 60) {
                RTC_SEC = 0;
                (RTC_MIN)++;
                if (RTC_CIIR & CIIR_IMMIN) {                             // interrupt on each minute increment
                    RTC_ILR |= RTCCIF;                                   // set the increment interrupt flag
                }
                if (RTC_MIN >= 60) {
                    RTC_MIN = 0;
                    (RTC_HOUR)++;
                    if (RTC_CIIR & CIIR_IMHOUR) {                        // interrupt on each hour increment
                        RTC_ILR |= RTCCIF;                               // set the increment interrupt flag
                    }
                    if (RTC_HOUR >= 24) {
                        RTC_HOUR = 0;
                        (RTC_DOW)++;
                        if (RTC_CIIR & (CIIR_IMDOW | CIIR_IMDOW | CIIR_IMDOY)) { // interrupt on each day of week, month or year increment
                            RTC_ILR |= RTCCIF;                           // set the increment interrupt flag
                        }
                        if (RTC_DOW >= 7) {
                            RTC_DOW = 0;
                        }
                        (RTC_DOY)++;
                        (RTC_DOM)++;
                        if (RTC_DOM >= 30) {                             // approximate
                            RTC_DOM = 0;
                            (RTC_MONTH)++;
                            if (RTC_CIIR & CIIR_IMMON) {                 // interrupt on each month increment
                                RTC_ILR |= RTCCIF;                       // set the increment interrupt flag
                            }
                            if (RTC_MONTH >= 12) {
                                RTC_MONTH = 0;
                                (RTC_YEAR)++;
                                if (RTC_CIIR & CIIR_IMYEAR) {             // interrupt on each year increment
                                    RTC_ILR |= RTCCIF;                   // set the increment interrupt flag
                                }
                            }
                        }
                    }
                }
            }
            RTC_CTIME0 = (RTC_SEC | (RTC_MIN << 13) | (RTC_HOUR << 20) | (RTC_DOW << 26));
            RTC_CTIME1 = (RTC_DOM | (RTC_MONTH << 11) | (RTC_YEAR << 27));
            RTC_CTIME2 = RTC_DOY;

            if ((VICIntEnable & INTERRUPT_RTC) && (RTC_ILR & RTCCIF)) {  // interrupt on increment
    #ifdef _LPC21XX
                void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_RTC);
    #else
                void (*interrupt_call)(void) = (void (*)(void))VICVectAddr13;
    #endif
                interrupt_call();                                        // call interrupt handler
            }
        }
    }
#endif
    return 0;
}


unsigned char *fnGetSimTxBufferAdd(void)
{
#ifdef ETH_INTERFACE
    return (fnGetTxBufferAdd(0));
#else
    return 0;
#endif
}


extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce)
{
#ifdef ETH_INTERFACE                                                     // we feed frames in promiscuous mode and filter them according to their details
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
    LPC23XX_RX_BD *ptrRxBd;
    LPC23XX_RX_STATUS *ptrRxStatus;

    if ((iForce == 0) && (fnCheckEthernetMode(ucData, usLen) == 0)) {
        return 0;                                                        // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
    }

    ptrRxBd = (LPC23XX_RX_BD *)EMAC_RxDescriptor;
    ptrRxBd += EMAC_RxProduceIndex;

    ptrRxStatus = (LPC23XX_RX_STATUS *)EMAC_RxStatus;
    ptrRxStatus += EMAC_RxConsumeIndex;                          

    uMemcpy((unsigned char *)ptrRxBd->ptrBlock, ucData, usLen);          // copy received data to the input buffer
    ptrRxStatus->ulStatusInfo = (usLen + 3);                             // {17} the frame length (with CRC-32)

    if (ptrRxBd->bd_details & RX_LAN_INTERRUPT_ON_DONE) {
        EMAC_IntStatus |= EMAC_RxDoneInt;                                // set interrupt flag
        if (EMAC_IntEnable & EMAC_RxDoneInt) {
            void (*interrupt_call)(void);
            interrupt_call = (void (*)(void))VICVectAddr21;
            interrupt_call();
        }
    }

    if (EMAC_RxProduceIndex >= EMAC_RxDescriptorNumber) {
        EMAC_RxProduceIndex = 0;
    }
    else {
        (EMAC_RxProduceIndex)++;
    }
    return 1;
#else
    return 0;
#endif
}


#ifdef SUPPORT_KEY_SCAN                                                  // {1}

#if KEY_COLUMNS == 0
    #define _NON_MATRIX
    #undef KEY_COLUMNS
    #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
    #undef KEY_ROWS
    #define KEY_ROWS VIRTUAL_KEY_ROWS
#endif

static unsigned char fnMapPortBit(unsigned long ulRealBit)
{
    unsigned long ulBit = 0x80000000;
    unsigned char ucRef = 0;

    while (ulBit) {
        if (ulRealBit & ulBit) {
            break;
        }
        ulBit >>= 1;
        ucRef++;
    }
    return ucRef;
}

int iKeyPadInputs[KEY_COLUMNS][KEY_ROWS] = {0};

extern void fnSimulateKeyChange(int *intTable)
{
#if defined _NON_MATRIX
    int iCol, iRow;
    int iChange;
    for (iCol = 0; iCol < KEY_COLUMNS; iCol++) {
        for (iRow = 0; iRow < KEY_ROWS; iRow++) {
            iChange = iKeyPadInputs[iCol][iRow];                         // original value
            iKeyPadInputs[iCol][iRow] = *intTable++;                     // new value
            if (iChange != iKeyPadInputs[iCol][iRow]) {
    #if defined KEY_POLARITY_POSITIVE
                if (!iChange)
    #else
                if (iChange)                                             // generally a key press is a '0' 
    #endif
                {
                    iChange = SET_INPUT;
                }
                else {
                    iChange = CLEAR_INPUT;
                }
                switch ((iCol * KEY_ROWS) + iRow) {                      // the specific input
    #if defined KEY_1_PORT_REF
                case 0:                                                  // first key
                    fnSimulateInputChange(KEY_1_PORT_REF, fnMapPortBit(KEY_1_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_2_PORT_REF
                case 1:                                                  // second key
                    fnSimulateInputChange(KEY_2_PORT_REF, fnMapPortBit(KEY_2_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_3_PORT_REF
                case 2:                                                  // third key
                    fnSimulateInputChange(KEY_3_PORT_REF, fnMapPortBit(KEY_3_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_4_PORT_REF
                case 3:                                                  // fourth key
                    fnSimulateInputChange(KEY_4_PORT_REF, fnMapPortBit(KEY_4_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_5_PORT_REF
                case 4:                                                  // fifth key
                    fnSimulateInputChange(KEY_5_PORT_REF, fnMapPortBit(KEY_5_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_6_PORT_REF
                case 5:                                                  // sixth key
                    fnSimulateInputChange(KEY_6_PORT_REF, fnMapPortBit(KEY_6_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_7_PORT_REF
                case 6:                                                  // seventh key
                    fnSimulateInputChange(KEY_7_PORT_REF, fnMapPortBit(KEY_7_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_8_PORT_REF
                case 7:                                                  // eighth key
                    fnSimulateInputChange(KEY_8_PORT_REF, fnMapPortBit(KEY_8_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_9_PORT_REF
                case 8:                                                  // ninth key
                    fnSimulateInputChange(KEY_9_PORT_REF, fnMapPortBit(KEY_9_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_10_PORT_REF
                case 9:                                                  // tenth key
                    fnSimulateInputChange(KEY_10_PORT_REF, fnMapPortBit(KEY_10_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_11_PORT_REF
                case 10:                                                 // eleventh key
                    fnSimulateInputChange(KEY_11_PORT_REF, fnMapPortBit(KEY_11_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_12_PORT_REF
                case 11:                                                 // twelf key
                    fnSimulateInputChange(KEY_12_PORT_REF, fnMapPortBit(KEY_12_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_13_PORT_REF
                case 12:                                                 // thirteenth key
                    fnSimulateInputChange(KEY_13_PORT_REF, fnMapPortBit(KEY_13_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_14_PORT_REF
                case 13:                                                 // fourteenth key
                    fnSimulateInputChange(KEY_14_PORT_REF, fnMapPortBit(KEY_14_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_15_PORT_REF
                case 14:                                                 // fifteenth key
                    fnSimulateInputChange(KEY_15_PORT_REF, fnMapPortBit(KEY_15_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_16_PORT_REF
                case 15:                                                 // sixteenth key
                    fnSimulateInputChange(KEY_16_PORT_REF, fnMapPortBit(KEY_16_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_17_PORT_REF
                case 16:                                                 // seventeenth key
                    fnSimulateInputChange(KEY_17_PORT_REF, fnMapPortBit(KEY_17_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_18_PORT_REF
                case 17:                                                 // eighteenth key
                    fnSimulateInputChange(KEY_18_PORT_REF, fnMapPortBit(KEY_18_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_19_PORT_REF
                case 18:                                                 // nineteenth key
                    fnSimulateInputChange(KEY_19_PORT_REF, fnMapPortBit(KEY_19_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_20_PORT_REF
                case 19:                                                 // twentieth key
                    fnSimulateInputChange(KEY_20_PORT_REF, fnMapPortBit(KEY_20_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_21_PORT_REF
                case 20:                                                 // twenty first key
                    fnSimulateInputChange(KEY_21_PORT_REF, fnMapPortBit(KEY_21_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_22_PORT_REF
                case 21:                                                 // twenty second key
                    fnSimulateInputChange(KEY_22_PORT_REF, fnMapPortBit(KEY_22_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_23_PORT_REF
                case 22:                                                 // twenty third key
                    fnSimulateInputChange(KEY_23_PORT_REF, fnMapPortBit(KEY_23_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_24_PORT_REF
                case 23:                                                 // twenty fourth key
                    fnSimulateInputChange(KEY_24_PORT_REF, fnMapPortBit(KEY_24_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_25_PORT_REF
                case 24:                                                 // twenty fifth key
                    fnSimulateInputChange(KEY_25_PORT_REF, fnMapPortBit(KEY_25_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_26_PORT_REF
                case 25:                                                 // twenty sixth key
                    fnSimulateInputChange(KEY_26_PORT_REF, fnMapPortBit(KEY_26_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_27_PORT_REF
                case 26:                                                  // twenty seventh key
                    fnSimulateInputChange(KEY_27_PORT_REF, fnMapPortBit(KEY_27_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_28_PORT_REF
                case 27:                                                  // twenty eighth key
                    fnSimulateInputChange(KEY_28_PORT_REF, fnMapPortBit(KEY_28_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_29_PORT_REF
                case 28:                                                  // twenty ninth key
                    fnSimulateInputChange(KEY_29_PORT_REF, fnMapPortBit(KEY_29_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_30_PORT_REF
                case 29:                                                  // thirtieth key
                    fnSimulateInputChange(KEY_30_PORT_REF, fnMapPortBit(KEY_30_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_31_PORT_REF
                case 30:                                                  // thirty first key
                    fnSimulateInputChange(KEY_31_PORT_REF, fnMapPortBit(KEY_31_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_32_PORT_REF
                case 31:                                                  // thirty second key
                    fnSimulateInputChange(KEY_32_PORT_REF, fnMapPortBit(KEY_32_PORT_PIN), iChange);
                    break;
    #endif
                }
            }
        }
    }
#else
    memcpy(iKeyPadInputs, intTable, sizeof(iKeyPadInputs));              // copy key pad state to local set
#endif
}

static int fnColumnLow(int iColumnOutput)
{
#if defined KEY_COLUMNS && !defined _NON_MATRIX
    switch (iColumnOutput) {
    case 0:
        return (KEY_COL_OUT_1 & ~KEY_COL_OUT_PORT_1 & KEY_COL_OUT_DDR_1);// if column 1 is being driven low
    #if KEY_COLUMNS > 1
    case 1:
        return (KEY_COL_OUT_2 & ~KEY_COL_OUT_PORT_2 & KEY_COL_OUT_DDR_2);// if column 2 is being driven low
    #endif
    #if KEY_COLUMNS > 2
    case 2:
        return (KEY_COL_OUT_3 & ~KEY_COL_OUT_PORT_3 & KEY_COL_OUT_DDR_3);// if column 3 is being driven low
    #endif
    #if KEY_COLUMNS > 3
    case 3:
        return (KEY_COL_OUT_4 & ~KEY_COL_OUT_PORT_4 & KEY_COL_OUT_DDR_4);// if column 4 is being driven low
    #endif
    #if KEY_COLUMNS > 4
    case 4:
        return (KEY_COL_OUT_5 & ~KEY_COL_OUT_PORT_5 & KEY_COL_OUT_DDR_5);// if column 5 is being driven low
    #endif
    #if KEY_COLUMNS > 5
    case 5:
        return (KEY_COL_OUT_6 & ~KEY_COL_OUT_PORT_6 & KEY_COL_OUT_DDR_6);// if column 6 is being driven low
    #endif
    #if KEY_COLUMNS > 6
    case 6:
        return (KEY_COL_OUT_7 & ~KEY_COL_OUT_PORT_7 & KEY_COL_OUT_DDR_7);// if column 7 is being driven low
    #endif
    #if KEY_COLUMNS > 7
    case 7:
        return (KEY_COL_OUT_8 & ~KEY_COL_OUT_PORT_8 & KEY_COL_OUT_DDR_8);// if column 8 is being driven low
    #endif
    }
#endif
    return 0;
}

static void fnSetRowInput(int iRowInput, int iState)
{
    int iChange;

    if (iState) {
        iChange = CLEAR_INPUT;
    }
    else {
        iChange = SET_INPUT;
    }

#if !defined _NON_MATRIX
    switch (iRowInput) {
    case 0:
    #ifdef KEY_ROWS
        fnSimulateInputChange(KEY_ROW_IN_PORT_1_REF, fnMapPortBit(KEY_ROW_IN_1), iChange);
    #endif
        break;
    case 1:
    #if KEY_ROWS > 1
        fnSimulateInputChange(KEY_ROW_IN_PORT_2_REF, fnMapPortBit(KEY_ROW_IN_2), iChange);
    #endif
        break;
    case 2:
    #if KEY_ROWS > 2
        fnSimulateInputChange(KEY_ROW_IN_PORT_3_REF, fnMapPortBit(KEY_ROW_IN_3), iChange);
    #endif
        break;
    case 3:
    #if KEY_ROWS > 3
        fnSimulateInputChange(KEY_ROW_IN_PORT_4_REF, fnMapPortBit(KEY_ROW_IN_4), iChange);
    #endif
        break;
    case 4:
    #if KEY_ROWS > 4
        fnSimulateInputChange(KEY_ROW_IN_PORT_5_REF, fnMapPortBit(KEY_ROW_IN_5), iChange);
    #endif
        break;
    case 5:
    #if KEY_ROWS > 5
        fnSimulateInputChange(KEY_ROW_IN_PORT_6_REF, fnMapPortBit(KEY_ROW_IN_6), iChange);
    #endif
        break;
    case 6:
    #if KEY_ROWS > 6
        fnSimulateInputChange(KEY_ROW_IN_PORT_7_REF, fnMapPortBit(KEY_ROW_IN_7), iChange);
    #endif
        break;
    case 7:
    #if KEY_ROWS > 7
        fnSimulateInputChange(KEY_ROW_IN_PORT_8_REF, fnMapPortBit(KEY_ROW_IN_8), iChange);
    #endif
        break;
    }
#endif
}

// This routine updates the ports to respect the present setting of a connected matrix key pad
//
extern void fnSimMatrixKB(void)
{
    int i, j;
    iFlagRefresh = fnPortChanges(1);                                     // synchronise with present settings                                                                         
    for (i = 0; i < KEY_COLUMNS; i++) {                                  // check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);
            }
        }
    }
}
#endif


#ifdef USB_INTERFACE                                                     // {14}

// Handle data sent by USB                                         
//
extern void fnSimUSB(int iType, int iEndpoint, USB_HW *ptrUSB_HW)
{
    extern void fnChangeUSBState(int iNewState);
    switch (iType) {
    case USB_SIM_RESET:
        {
            int x;
            ucUSB_address = DEV_EN;
            fnChangeUSBState(0);
            for (x = 0; x < NUMBER_OF_USB_ENDPOINTS; x++) {
                fnLogUSB(x, 0, (unsigned char *)0xffffffff, 0);          // log reset on each endpoint
            }
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
//        if (((TXPKTRDY | EPEDS) & *ptrUSB_HW->ptr_ulUSB_BDControl) != (TXPKTRDY | EPEDS)) { // if the ownership has not been passed to the USB controller ignore it
//          return;
//      }
        iInts |= USB_INT;                                                // flag that the interrupt should be handled
        ulEndpointInt |= (1 << iEndpoint);                               // flag endpoint
        break;
    case USB_SIM_ENUMERATED:                                             // flag that we have completed emumeration
        fnChangeUSBState(1);
        break;
    case USB_SIM_STALL:
        fnLogUSB(iEndpoint, 1, (unsigned char *)0xffffffff, 0);          // log stall
        break;
    case USB_SIM_SUSPEND:
        fnChangeUSBState(0);
        break;
    }
}

static unsigned char ucEndpointFifoIn0[64];                               // LPC endpoint rx FIFOs - control
static unsigned char ucEndpointFifoIn1[64];                               // isochronous
static unsigned char ucEndpointFifoIn2[2][64];                            // bulk - double buffered
static unsigned char ucEndpointFifoIn3[2][1023];                          // isochronous - double buffered
static unsigned char ucEndpointFifoIn4[64];                               // interrupt
static unsigned char ucEndpointFifoIn5[2][64];                            // bulk - double buffered
static unsigned char ucEndpointFifoIn6[2][1023];                          // isochronous - double buffered
static unsigned char ucEndpointFifoIn7[64];                               // interrupt
static unsigned char ucEndpointFifoIn8[2][64];                            // bulk - double buffered
static unsigned char ucEndpointFifoIn9[9][1023];                          // isochronous - double buffered
static unsigned char ucEndpointFifoIn10[64];                              // interrupt
static unsigned char ucEndpointFifoIn11[2][64];                           // bulk - double buffered
static unsigned char ucEndpointFifoIn12[9][1023];                         // isochronous - double buffered
static unsigned char ucEndpointFifoIn13[64];                              // interrupt
static unsigned char ucEndpointFifoIn14[2][64];                           // bulk - double buffered
static unsigned char ucEndpointFifoIn15[2][64];                           // bulk - double buffered

static unsigned short usEndpoint_in_index[16] = {0};
static unsigned char ucEndpoint_in_buffer_put[16] = {0};
static unsigned char ucEndpoint_in_buffer_get[16] = {0};
static unsigned long ulEndpoint_in_valid[16] = {0};

static unsigned char ucEndpointFifoOut0[64];                             // LPC endpoint rx FIFOs - control
static unsigned char ucEndpointFifoOut1[64];                             // isochronous
static unsigned char ucEndpointFifoOut2[2][64];                          // bulk - double buffered
static unsigned char ucEndpointFifoOut3[2][1023];                        // isochronous - double buffered
static unsigned char ucEndpointFifoOut4[64];                             // interrupt
static unsigned char ucEndpointFifoOut5[2][64];                          // bulk - double buffered
static unsigned char ucEndpointFifoOut6[2][1023];                        // isochronous - double buffered
static unsigned char ucEndpointFifoOut7[64];                             // interrupt
static unsigned char ucEndpointFifoOut8[2][64];                          // bulk - double buffered
static unsigned char ucEndpointFifoOut9[2][1023];                        // isochronous - double buffered
static unsigned char ucEndpointFifoOut10[64];                            // interrupt
static unsigned char ucEndpointFifoOut11[2][64];                         // bulk - double buffered
static unsigned char ucEndpointFifoOut12[2][1023];                       // isochronous - double buffered
static unsigned char ucEndpointFifoOut13[64];                            // interrupt
static unsigned char ucEndpointFifoOut14[2][64];                         // bulk - double buffered
static unsigned char ucEndpointFifoOut15[2][64];                         // bulk - double buffered

static int iEndpointProperties[16] = {                                   // double-buffered property
    0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1
};

static unsigned long endpoint_enabled = 0;

static unsigned char ucDeviceStatus = 0;
static unsigned char ucEndpointStatus[2*16] = {0};


extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    extern int iGetGetBank(int iEndpoint);

    int iBank = 0;
    switch (iEndpoint) {
    case 0:                                                              // endpoint 0
        while (ulEndpoint_in_valid[0]) {                                 // IN buffer valid
            unsigned short usUSBLength;
            usUSBLength = (unsigned short)ulEndpoint_in_valid[0];        // extract the length of data content
            ulEndpoint_in_valid[0] = 0;
            iTxDataToggle[0] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(0, usUSBLength, ucEndpointFifoOut0, iTxDataToggle[0]);
            USBEpIntSt  |= 0x00000002;                                   // mark that this endpoint has a pending interrupt
            if (USBEpIntSt & 0x00000002) {                               // if the interrupt is enabled set the device interrupt state
                if (USBEpIntPri & 0x00000002) {
                    USBDevIntSt |= DEVICE_STATUS_EP_FAST;
                }
                else {
                    USBDevIntSt |= DEVICE_STATUS_EP_SLOW;
                }
            }
            ucEndpointStatus[1] &= ~(USB_EP_BUFFER_1_FULL | USB_EP_FULL_EMPTY); // tx buffer free
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt            
        }
        /*if (UDP_CSR0 & FORCESTALL) {
            UDP_CSR0 |= STALLSENT;                                       // stall acknowledged
            UDP_ISR = EP0INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }*/
        break;
/*
    case 1:                                                              // endpoint 1
        if (!(UDP_CSR1 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR1 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(1);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut1[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[1] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(1, usUSBLength, &ucEndpointFifoOut1[iBank][2], iTxDataToggle[1]);
            if (UDP_IMR & EP1INT) {
                UDP_ISR |= EP1INT;
            }
            UDP_CSR1 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR1 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR1 & FORCESTALL) {
            UDP_CSR1 |= STALLSENT;                                       // stall acknowledged
            UDP_ISR = EP1INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    case 2:                                                              // endpoint 2
        if (!(UDP_CSR2 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR2 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(2);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut2[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[2] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(2, usUSBLength, &ucEndpointFifoOut2[iBank][2], iTxDataToggle[2]);
            if (UDP_IMR & EP2INT) {
                UDP_ISR |= EP2INT;
            }
            UDP_CSR2 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR2 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR2 & FORCESTALL) {
            UDP_CSR2 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP2INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    case 3:                                                              // endpoint 3
        if (!(UDP_CSR3 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR3 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut3, sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[3] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(3, usUSBLength, &ucEndpointFifoOut3[2], iTxDataToggle[3]);
            if (UDP_IMR & EP3INT) {
                UDP_ISR |= EP3INT;
            }
            UDP_CSR3 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR3 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR3 & FORCESTALL) {
            UDP_CSR3 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP0INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    case 4:                                                              // endpoint 4
        if (!(UDP_CSR4 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR4 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(4);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut4[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[4] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(4, usUSBLength, &ucEndpointFifoOut4[iBank][2], iTxDataToggle[4]);
            if (UDP_IMR & EP4INT) {
                UDP_ISR |= EP4INT;
            }
            UDP_CSR4 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR4 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR4 & FORCESTALL) {
            UDP_CSR4 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP0INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;*/
    case 5:                                                              // endpoint 5
        while (ulEndpoint_in_valid[5]) {                                 // IN buffer valid
            unsigned short usUSBLength;
            usUSBLength = (unsigned short)ulEndpoint_in_valid[5];        // extract the length of data content
            ulEndpoint_in_valid[5] = 0;
            iTxDataToggle[5] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(5, usUSBLength, ucEndpointFifoOut5[0], iTxDataToggle[5]);
            USBEpIntSt  |= 0x00000800;                                   // mark that this endpoint has a pending interrupt
            if (USBEpIntSt & 0x00000800) {                               // if the interrupt is enabled set the device interrupt state
                if (USBEpIntPri & 0x00000800) {
                    USBDevIntSt |= DEVICE_STATUS_EP_FAST;
                }
                else {
                    USBDevIntSt |= DEVICE_STATUS_EP_SLOW;
                }
            }
            ucEndpointStatus[11] &= ~(USB_EP_BUFFER_1_FULL | USB_EP_FULL_EMPTY); // tx buffer free
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt            
        }
        /*if (UDP_CSR0 & FORCESTALL) {
            UDP_CSR0 |= STALLSENT;                                       // stall acknowledged
            UDP_ISR = EP0INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }*/
        break;
    }
}



extern unsigned long fnUSB_command_interpreter(unsigned long ulCommand, unsigned long ulData)
{
    switch (ulCommand) {
    case USB_CMD_GET_DEVICE_STATUS:
        if (USB_CMD_GET_DEVICE_STATUS_DATA_PHASE == ulData) {
            return ucDeviceStatus;
        }
        break;
    default:
        if ((ulCommand & ~0x00ff0000) == USB_CMD_SELECT_EP) {
            unsigned char ucEndpoint = (unsigned char)((ulCommand & 0x00ff0000) >> 16);
            if ((ulData & ~0x00ff0000) == USB_CMD_SELECT_EP_DATA_PHASE) {
                if ((unsigned char)((ulData & 0x00ff0000) >> 16) == ucEndpoint) {
                    return ucEndpointStatus[ucEndpoint];
                }
            }
        }
        break;
    }
    return 0;
}


extern unsigned long fnUSB_command_command(unsigned long ulCommand, unsigned long ulData)
{
    static unsigned char ucEndpoint = 0;
    switch (ulCommand) {
    case USB_CMD_SET_DEVICE_STATUS:
        break;
    case USB_CMD_SET_ADDRESS:
        if (USB_CMD_SET_ADDRESS_DATA_PHASE == (ulData & ~0x00ff0000)) {
            ucUSB_address = (unsigned char)(ulData >> 16);
        }
        break;
    case USB_CMD_VALID_EP_BUFFER:                                        // validates that the IN buffer corresponding to the presently selected endpoint is reads to be dispatched
        ulEndpoint_in_valid[ucEndpoint/2] = (USBTxPLen | 0x80000000);    // mark that data is ready and set its length
        if (ucEndpoint <= 1) {
            ucEndpointStatus[ucEndpoint] |= (USB_EP_BUFFER_1_FULL | USB_EP_FULL_EMPTY); // mark that buffer 1 is full and also set the full flag
        }
        else {
            ucEndpointStatus[ucEndpoint] |= (USB_EP_BUFFER_1_FULL | USB_EP_FULL_EMPTY); // mark that buffer 1 is full and also set the full flag
        }
        break;
    default:
        if ((ulCommand & ~0x00ff0000) == USB_CMD_SELECT_EP) {
            ucEndpoint = (unsigned char)((ulCommand & 0x00ff0000) >> 16);// select physical enpoint index
        }
        break;
    }
    return 0;
}





static volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned char *ptrAdd, int iDirection)
{
    /*
    if (iDirection != 0) {                                               // transmission buffer
        if (UDP_FDR0_ADD == ptrAdd) {
            return ucEndpointFifoOut0;
        }
        if (UDP_FDR1_ADD == ptrAdd) {
            return ucEndpointFifoOut1[0];
        }
        if (UDP_FDR2_ADD == ptrAdd) {
            return ucEndpointFifoOut2[0];
        }
        if (UDP_FDR3_ADD == ptrAdd) {
            return ucEndpointFifoOut3;
        }
        if (UDP_FDR4_ADD == ptrAdd) {
            return ucEndpointFifoOut4[0];
        }
        if (UDP_FDR5_ADD == ptrAdd) {
            return ucEndpointFifoOut5[0];
        }
    }
    else {
        if (UDP_FDR0_ADD == ptrAdd) {
            return ucEndpointFifoIn0;
        }
        if (UDP_FDR1_ADD == ptrAdd) {
            return ucEndpointFifoIn1[0];
        }
        if (UDP_FDR2_ADD == ptrAdd) {
            return ucEndpointFifoIn2[0];
        }
        if (UDP_FDR3_ADD == ptrAdd) {
            return ucEndpointFifoIn3;
        }
        if (UDP_FDR4_ADD == ptrAdd) {
            return ucEndpointFifoIn4[0];
        }
        if (UDP_FDR5_ADD == ptrAdd) {
            return ucEndpointFifoIn5[0];
        }
    }
    return ptrAdd;
    */
    return 0;
}

extern unsigned long fnGetUSB_from_FIFO(void)
{
    if (USBCtrl & CTRL_RD_EN) {
        unsigned char ucEndpoint = (unsigned char)((USBCtrl >> 2) & 0x0f);// logical endpoint
        unsigned long ulData;
        switch (ucEndpoint) {
        case 0:
            ulData  =  ucEndpointFifoIn0[usEndpoint_in_index[0]++];
            ulData |= (ucEndpointFifoIn0[usEndpoint_in_index[0]++] << 8);
            ulData |= (ucEndpointFifoIn0[usEndpoint_in_index[0]++] << 16);
            ulData |= (ucEndpointFifoIn0[usEndpoint_in_index[0]++] << 24);
            break;

        case 2:
            ulData  =  ucEndpointFifoIn2[ucEndpoint_in_buffer_get[2]][usEndpoint_in_index[2]++];
            ulData |= (ucEndpointFifoIn2[ucEndpoint_in_buffer_get[2]][usEndpoint_in_index[2]++] << 8);
            ulData |= (ucEndpointFifoIn2[ucEndpoint_in_buffer_get[2]][usEndpoint_in_index[2]++] << 16);
            ulData |= (ucEndpointFifoIn2[ucEndpoint_in_buffer_get[2]][usEndpoint_in_index[2]++] << 24);
            ucEndpoint_in_buffer_get[2] ^= 0x01;
            break;
        }
        return ulData;
    }
    return 0;
}

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
    if (ptrDebugIn == 0) {
        switch (usLenEvent) {
        case USB_RESET_CMD:
//          memset(iTxDataToggle, 0, sizeof(iTxDataToggle));
//          memset(iRxDataBank, 0, sizeof(iRxDataBank));
//          memset(ulTxDataBank, 0, sizeof(ulTxDataBank));
            USBDevIntSt |= DEVICE_STATUS_DEV_STAT;                       // flag that a USB bus reset has been detected
            ucDeviceStatus = USB_STATUS_RST;
            break;
        case USB_SLEEP_CMD:
            USBDevIntSt |= DEVICE_STATUS_DEV_STAT;                       // flag that a suspend reset has been detected
            ucDeviceStatus = (0x00000004 | 0x00000008);
            break;
        case USB_RESUME_CMD:
//          UDP_ISR = RXRSM;
            break;
        case USB_STALL_SUCCESS:
        case USB_IN_SUCCESS:
            break;
        default:
            return 0;
        }
    }
    else {                                                               // data being injected
        if (((ucUSB_address & DEV_EN) == 0) || ((ucUSB_address & ~DEV_EN) != iDevice)) { // if the USB address is either not enabled or is different to the addressed one
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        switch (iEndPoint) {
        case 0:                                                          // endpoint 0 - control/bulk/interrupt - max endpoint size 8 bytes - not dual-banked
            USBEpIntSt |= 0x00000001;                                    // mark that this endpoint has a pending interrupt
            if (USBEpIntEn & 0x00000001) {                               // if the interrupt is enabled set the device interrupt state
                if (USBEpIntPri & 0x00000001) {
                    USBDevIntSt |= DEVICE_STATUS_EP_FAST;
                }
                else {
                    USBDevIntSt |= DEVICE_STATUS_EP_SLOW;
                }
            }
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                ucEndpointStatus[0] = USB_EP_SETUP_PACKET_RECEIVE;
            }
            else if (OUT_PID == ucPID) {
                ucEndpointStatus[0] = 0;
            }
            USBRxPLen = (usLenEvent | PKT_DV | PKT_RDY);                 // the reception length
            if (usLenEvent > sizeof(ucEndpointFifoIn0)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn0);
            }
            memcpy(ucEndpointFifoIn0, ptrDebugIn, usLenEvent); 
            usEndpoint_in_index[0] = 0;
            break;
        case 2:                                                          // double-buffered bulk
            USBEpIntSt |= 0x00000010;                                    // mark that this endpoint has a pending interrupt (or DMA request)
            if (USBEpIntEn & 0x00000010) {                               // if the interrupt is enabled set the device interrupt state
                if (USBEpIntPri & 0x00000010) {
                    USBDevIntSt |= DEVICE_STATUS_EP_FAST;
                }
                else {
                    USBDevIntSt |= DEVICE_STATUS_EP_SLOW;
                }
            }
            ucEndpointStatus[2] = 0;
            USBRxPLen = (usLenEvent | PKT_DV | PKT_RDY);                 // the reception length
            if (usLenEvent > sizeof(ucEndpointFifoIn2)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn2);
            }
            memcpy(ucEndpointFifoIn2[ucEndpoint_in_buffer_put[2]], ptrDebugIn, usLenEvent); 
            usEndpoint_in_index[2] = 0;
            ucEndpoint_in_buffer_put[2] ^= 0x01;
            break;
        }
    }
    if (USBDevIntSt & USBDevIntEn) {                    
    #ifdef _LPC21XX
        void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_USB);
    #else
        void (*interrupt_call)(void) = (void (*)(void))VICVectAddr22;
    #endif
        interrupt_call();                                                // call interrupt handler
    }
#if (defined USB_DMA_RX || defined USB_DMA_TX) && defined USB_RAM_SIZE
    else if (USBEpIntSt & USBEpDMASt) {                                  // DMA enabled on endpoint
        extern unsigned char *fnGetUSB_mem(void);
        unsigned short usFrameSize;
        unsigned short usBufferSize;
        unsigned long *ptrUSB_mem = (unsigned long *)fnGetUSB_mem();
        DMA_DESCRIPTOR_ISO *buffer_descriptor;
        ptrUSB_mem += (2 * iEndPoint);                                   // first buffer descriptor belonging to this endpoint
        buffer_descriptor = (DMA_DESCRIPTOR_ISO *)*ptrUSB_mem;
        usFrameSize = (unsigned short)((MAX_PACKET_SIZE_MASK & buffer_descriptor->dma_mode_status) >> 5);
        usBufferSize = (unsigned short)(buffer_descriptor->dma_mode_status >> 16);
        if (usFrameSize < usLenEvent) {                                  // copy FIFO content to USB memory
            uMemcpy((unsigned char *)buffer_descriptor->buffer_start_address, ptrDebugIn, usFrameSize);
            buffer_descriptor->buffer_start_address += usFrameSize;
            buffer_descriptor->dd_mode_status = (DD_RETIRED | DD_STATUS_DATA_UNDERRUN_SHORT | DD_PACKET_VALID | ((usFrameSize << 8) & DD_MESSAGE_LENGTH_POS_MASK) | (usFrameSize << 16));
        }
        else {
            uMemcpy((unsigned char *)buffer_descriptor->buffer_start_address, ptrDebugIn, usLenEvent);
            buffer_descriptor->buffer_start_address += usLenEvent;
            buffer_descriptor->dd_mode_status = (DD_RETIRED | DD_STATUS_DATA_UNDERRUN_SHORT | DD_PACKET_VALID | (usLenEvent << 16));
        }
        USBDMAIntSt |= USB_DMA_EOT;
        if (USBDMAIntEn & USB_DMA_EOT) {                                 // if interrupt enabled on DMA descriptor completion
    #ifdef _LPC21XX
            void (*interrupt_call)(void) = (void (*)(void))fnGetVectoredIntSlot(INTERRUPT_USB);
    #else
            void (*interrupt_call)(void) = (void (*)(void))VICVectAddr22;
    #endif
            USBIntSt |= USB_INT_REQ_DMA;
            USBEoTIntSt |= (0x00000001 << (iEndPoint * 2));
            USBEpIntSt &= ~(0x00000001 << (iEndPoint * 2));
            interrupt_call();                                            // call interrupt handler
        }
        
    }
#endif
    return 0;
}
#endif

#ifdef BATTERY_BACKED_RAM                                                // {7}
// Return all RTC content which is battery backed up
//
extern int fnGetBatteryRAMContent(unsigned char *ucData, unsigned long ulReference)
{
#ifdef _LPC21XX
    unsigned char *ptrData = (RTC_BLOCK + ulReference);
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
    if (ulReference < 0x80) {                                            // save the RTC content
        ptrData = (RTC_BLOCK + ulReference);
        *ucData = *ptrData;
        return 1;
    }
    #else
    #endif
#endif
    return 0;                                                            // all data saved
}
extern int fnPutBatteryRAMContent(unsigned char ucData, unsigned long ulReference)
{
#ifdef _LPC21XX
    unsigned char *ptrData = (RTC_BLOCK + ulReference);
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
    if (ulReference < 0x80) {                                            // save the RTC content
        ptrData = (RTC_BLOCK + ulReference);
        if ((ulReference < 0x14) || (ulReference >= 0x40)) {             // don't overwrite time which has been taken from the local PC
            *ptrData = ucData;
        }
        return 1;
    }
    #else
    #endif
#endif
    return 0;                                                            // no more data accepted
}
#endif


#ifdef SUPPORT_TOUCH_SCREEN
static int iPenLocationX = 0;                                            // last sample ADC input value
static int iPenLocationY = 0;
static int iPenDown = 0;

#define MIN_X_TOUCH          0x2000                                      // reference values for 4-wire touch on LPC2478-STK
#define MAX_X_TOUCH          0xe800
#define MIN_Y_TOUCH          0x2900
#define MAX_Y_TOUCH          0xe000

extern void fnPenPressed(int iX, int iY)
{
    iPenLocationX = (iX - 6);
    iPenLocationY = (iY - 3);
    iPenDown = 1;
}

extern void fnPenMoved(int iX, int iY)
{
    iPenLocationX = (iX - 6);
    iPenLocationY = (iY - 3);
}

extern void fnPenLifted(void)
{
    iPenLocationX = 0;
    iPenLocationY = 0;
    iPenDown = 0;
}

extern void fnGetPenSamples(unsigned short *ptrX, unsigned short *ptrY)
{
    if (iPenLocationX != 0) {
        iPenLocationX = iPenLocationX;
    }
    if (iPenDown == 0) {
        *ptrX = 0;
        *ptrY = 0;
    }
    else {
        *ptrX = (MIN_X_TOUCH + ((iPenLocationX * ((MAX_X_TOUCH - MIN_X_TOUCH)))/GLCD_X));
        *ptrY = (MIN_Y_TOUCH + ((iPenLocationY * ((MAX_Y_TOUCH - MIN_Y_TOUCH)))/GLCD_Y));
    }
}
#endif
#endif

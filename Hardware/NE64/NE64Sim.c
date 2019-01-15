/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:   	    NE64Sim.c
   Project: 	Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   *********************************************************************

   12.4.2007 Added fnGetFlash() and fnPutFlashAdd() routines
   10.10.2007 Allow application calling of interrupt simulation without argument list {2}
   10.10.2007 Limit serial interrupt processing to expected tick amount of characters {3}
   29.10.2007 Add fnSimulateModemChange() for compatibility              {4}
   15.12.2007 Correct UART1 speed simulation limit                       {5}
   15.12.2007 Add IIC simulation speed limitation for increased accuracy {6}
   15.12.2007 Swapped keypad rows/columns (correction)                   {7}
   31.12.2007 Use FLASH defines from NE64.h                              {8}
   31.12.2007 Allow SPI and FLASH files system to work together          {9}
   19.10.2008 Add dummy fnSimulateBreak()                                {10}
   08.02.2009 Add iForce parameter to fnSimulateEthernetIn()             {11}
   03.03.2009 Add iForce parameter to fnPortChanges()                    {12}

*/                            

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#ifdef _HW_NE64

#include "config.h"


/**************************************************************************************************/
/********************************* FREESCALE MC9S12NE64 *******************************************/
/**************************************************************************************************/

static unsigned char puPTA, orig_puPTA;
static unsigned char puPTB, orig_puPTB;
static unsigned char puPTE, orig_puPTE;
static unsigned char puPTG, orig_puPTG;
static unsigned char puPTH, orig_puPTH;
static unsigned char puPTJ, orig_puPTJ;
static unsigned char puPTK, orig_puPTK;
static unsigned char puPTL, orig_puPTL;
static unsigned char puPTS, orig_puPTS;
static unsigned char puPTT, orig_puPTT;
static unsigned char puPAD, orig_puPAD;

static int iFlagRefresh = 0;

static char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = { 0 };

// Initialisation for all non-zero registers in the device
//
static void fnSetDevice(unsigned char *port_pullups)
{
    extern void fnEnterHW_table(void *hw_table);

    SCISR1_0 = (TDRE | TC);
    SCISR1_1 = (TDRE | TC);

    SPICR1   = CPHA;

    SPISR    = SPTEF;

    IBSR     = 0x80;

    INITRM   = 0x09;
    MISC     = 0x0d;
    MTST1    = 0x10;
    PARTID   = MASK_SET_1L19S;

    PUCR     = 0x90;
    IRQCR    = 0x40;


    puPTA = orig_puPTA = *port_pullups++;
    puPTB = orig_puPTB = *port_pullups++;
    puPTE = orig_puPTE = *port_pullups++;
    puPTG = orig_puPTG = *port_pullups++;
    puPTH = orig_puPTH = *port_pullups++;
    puPTJ = orig_puPTJ = *port_pullups++;
    puPTK = orig_puPTK = *port_pullups++;
    puPTL = orig_puPTL = *port_pullups++;
    puPTS = orig_puPTS = *port_pullups++;
    puPTT = orig_puPTT = *port_pullups++;
    puPAD = orig_puPAD = *port_pullups;

    fnEnterHW_table(ucPortFunctions);
}

unsigned char ucFLASH[SIZE_OF_FLASH];

void fnInitialiseDevice(void *port_inits)
{
    uMemset(ucFLASH, 0xff, sizeof(ucFLASH));                             // We start with deleted FLASH memory contents
    fnPrimeFileSystem();                                                 // The project can then set parameters or files as required
    fnSetDevice((unsigned char *)port_inits);                            // set device registers to startup condition (if not zerod)
}

void fnDeleteFlashSector(unsigned char *ptrSector)
{
	int i = FLASH_GRANULARITY;                                           // {8}

	while (i--) {
		*ptrSector++ = 0xff;
	}
}

extern unsigned char *fnGetEEPROM(unsigned short usOffset);

extern unsigned char *fnGetFileSystemStart(void)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // {9}
    return (fnGetEEPROM(uFILE_START));
#else
    return (&ucFLASH[uFILE_START]);
#endif
}


// Enable simulation of direct FLASH access
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)START_OF_FLASH;
    return ucAdd + sim_add;
}

extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)START_OF_FLASH;
    return ucAdd - sim_add;
}


extern NE64_PERIPH ucNE64 = {0};



// simulate buffers for Ethernet use
unsigned char ucSimRxABuf[1536];
unsigned char ucSimRxBBuf[1536];
unsigned char ucSimTxBuf [1536];

unsigned char *fnGetSimTxBufferAdd(void)
{
    return ucSimTxBuf;
}


// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{    
    unsigned long ulNewActions = 0;
    int *ptrCnt;

	if ((iInts & CHANNEL_0_SERIAL_INT) && (argv)) {                      // {2}
        ptrCnt = (int *)argv[0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {                                      // {3}
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_SERIAL_INT;
#ifdef SERIAL_INTERFACE
	            fnLogTx0(SCIDRL_0);
                ulNewActions |= SEND_COM_0;
		        SCISR1_0 |= (TDRE);                                      // simulate Tx int 
		        if (SCICR1_0 & LOOPS) {
			        SCISR1_0 |= RDRF;                                    // and Rx due to loop
                }
                SCI1_Interrupt();
#endif
            }
        }
	}

	if ((iInts & CHANNEL_1_SERIAL_INT) && (argv)) {                      // {2}
        ptrCnt = (int *)argv[1];                                         // {5}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {                                      // {3}
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_1_SERIAL_INT;
#ifdef SERIAL_INTERFACE
	            fnLogTx1(SCIDRL_1);
                ulNewActions |= SEND_COM_1;
		        SCISR1_1 |= (TDRE);                                      // simulate Tx int 
		        if (SCICR1_1 & LOOPS) {
			        SCISR1_1 |= RDRF;                                    // and Rx due to loop
                }
                SCI1_Interrupt();
#endif
            }
        }
	}

    if (iInts & IIC_INT0) {
        ptrCnt = (int *)argv[3];                                         // {6}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= IIC_INT0;                                      // enough IIC interupts handled in this tick period
            }
            else {
#ifdef IIC_INTERFACE
		        iInts &= ~IIC_INT0;
		        IBSR |= IBIF;                                            // simulate IIC int 
                IIC_Interrupt();
#endif
            }
        }
    }
    return ulNewActions;
}

// process simulated DMA
extern unsigned long fnSimDMA(char *argv[])
{
    return 0;                                                            // NE64 doesn't support DMA
}



// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
 #ifdef SERIAL_INTERFACE
    if (iPort == 1) {
	    while (usLen--) {
		    SCISR1_1 |= RDRF;
		    SCIDRL_1 = *ptrDebugIn++;
            SCI1_Interrupt();
	    }
    }
    else if (iPort == 0) {
	    while (usLen--) {
		    SCISR1_0 |= RDRF;
		    SCIDRL_0 = *ptrDebugIn++;
            SCI0_Interrupt();
	    }
    }
 #endif
}

extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState) // {4}
{
#ifdef SUPPORT_HW_FLOW
    switch (iPort) {
    case 0:
        break;
    case 1:
        break;
    }
#endif
}

// UART Break detection simulation                                       {10}
//
extern void fnSimulateBreak(int iPort)
{
}

#ifdef SUPPORT_KEY_SCAN

static unsigned char fnMapPortBit(unsigned char ucRealBit)
{
    unsigned char ucBit = 0x80;
    unsigned char ucRef = 0;

    while (ucBit) {
        if (ucRealBit & ucBit) {
            break;
        }
        ucBit >>= 1;
        ucRef++;
    }
    return ucRef;
}


int iKeyPadInputs[KEY_ROWS][KEY_COLUMNS] = {0};

extern void fnSimulateKeyChange(int *intTable)
{
    memcpy(iKeyPadInputs, intTable, sizeof(iKeyPadInputs));              // copy key pad state to local set
}

static int fnColumnLow(int iColumnOutput)
{
#ifdef KEY_COLUMNS
    switch (iColumnOutput) {
    case 0:
        return (KEY_COL_OUT_1 & ~KEY_COL_OUT_PORT_1 & KEY_COL_OUT_DDR_1);
    #if KEY_COLUMNS > 1
    case 1:
        return (KEY_COL_OUT_2 & ~KEY_COL_OUT_PORT_2 & KEY_COL_OUT_DDR_2);
    #endif
    #if KEY_COLUMNS > 2
    case 2:
        return (KEY_COL_OUT_3 & ~KEY_COL_OUT_PORT_3 & KEY_COL_OUT_DDR_3);
    #endif
    #if KEY_COLUMNS > 3
    case 3:
        return (KEY_COL_OUT_4 & ~KEY_COL_OUT_PORT_4 & KEY_COL_OUT_DDR_4);
    #endif
    #if KEY_COLUMNS > 4
    case 4:
        return (KEY_COL_OUT_5 & ~KEY_COL_OUT_PORT_5 & KEY_COL_OUT_DDR_5);
    #endif
    #if KEY_COLUMNS > 5
    case 5:
        return (KEY_COL_OUT_6 & ~KEY_COL_OUT_PORT_6 & KEY_COL_OUT_DDR_6);
    #endif
    #if KEY_COLUMNS > 6
    case 6:
        return (KEY_COL_OUT_7 & ~KEY_COL_OUT_PORT_7 & KEY_COL_OUT_DDR_7);
    #endif
    #if KEY_COLUMNS > 7
    case 7:
        return (KEY_COL_OUT_8 & ~KEY_COL_OUT_PORT_8 & KEY_COL_OUT_DDR_8);
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
}

// This routine updates the ports to respect the present setting of a connected matrix key pad
//
extern void fnSimMatrixKB(void)
{
    int i, j;

    iFlagRefresh = fnPortChanges(1);                                     // {12} synchronise with present settings
    // Check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
    for (i = 0; i < KEY_COLUMNS; i++) {
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);                   // {7}
            }
        }
    }
}
#endif


extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    unsigned char ucBit = (0x01 << (7 - ucPortBit));
    switch (ucPort) {
    case _PORT_A:
        if (!(DDRA & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTA ^= ucBit;                                          // change pullup/down value so that input will change
                PORTA ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTA |= ucBit;
                PORTA |= ucBit;                                          // set the input high
            }
            else  {
                puPTA &= ~ucBit;                                         // set the input low
                PORTA &= ~ucBit;
            }
        }
        break;

    case _PORT_B:
        if (!(DDRB & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTB ^= ucBit;                                          // change pullup/down value so that input will change
                PORTB ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTB |= ucBit;
                PORTB |= ucBit;                                          // set the input high
            }
            else  {
                puPTB &= ~ucBit;                                         // set the input low
                PORTB &= ~ucBit;
            }        
        }
        break;

    case _PORT_E:
        if (!(DDRE & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTE ^= ucBit;                                          // change pullup/down value so that input will change
                PORTE ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTE |= ucBit;
                PORTE |= ucBit;                                          // set the input high
            }
            else  {
                puPTE &= ~ucBit;                                         // set the input low
                PORTE &= ~ucBit;
            }
        }
        break;

    case _PORT_G:
        if (!(DDRG & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTG ^= ucBit;                                          // change pullup/down value so that input will change
                PTG ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTG |= ucBit;
                PTG |= ucBit;                                            // set the input high
            }
            else  {
                puPTG &= ~ucBit;                                         // set the input low
                PTG &= ~ucBit;
            }
        }
        break;

    case _PORT_H:
        if (!(DDRH & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTH ^= ucBit;                                          // change pullup/down value so that input will change
                PTH ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTH |= ucBit;
                PTH |= ucBit;                                            // set the input high
            }
            else  {
                puPTH &= ~ucBit;                                         // set the input low
                PTH &= ~ucBit;
            }
        }
        break;

    case _PORT_J:
        if (!(DDRJ & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTJ ^= ucBit;                                          // change pullup/down value so that input will change
                PTJ ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTJ |= ucBit;
                PTJ |= ucBit;                                            // set the input high
            }
            else  {
                puPTJ &= ~ucBit;                                         // set the input low
                PTJ &= ~ucBit;
            }
        }
        break;

    case _PORT_K:                                                        // Port K
        if (!(DDRK & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTK ^= ucBit;                                          // change pullup/down value so that input will change
                PORTK ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTK |= ucBit;
                PORTK |= ucBit;                                          // set the input high
            }
            else  {
                puPTK &= ~ucBit;                                         // set the input low
                PORTK &= ~ucBit;
            }
        }
        break;

    case _PORT_L:
        if (!(DDRL & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTL ^= ucBit;                                          // change pullup/down value so that input will change
                PTL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTL |= ucBit;
                PTL |= ucBit;                                            // set the input high
            }
            else  {
                puPTL &= ~ucBit;                                         // set the input low
                PTL &= ~ucBit;
            }
        }
        break;

    case _PORT_S:
        if (!(DDRS & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTS ^= ucBit;                                          // change pullup/down value so that input will change
                PTS ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTS |= ucBit;
                PTS |= ucBit;                                            // set the input high
            }
            else  {
                puPTS &= ~ucBit;                                         // set the input low
                PTS &= ~ucBit;
            }
        }
        break;

    case _PORT_T:
        if (!(DDRT & ucBit)) {                                           // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPTT ^= ucBit;                                          // change pullup/down value so that input will change
                PTT ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPTT |= ucBit;
                PTT |= ucBit;                                            // set the input high
            }
            else  {
                puPTT &= ~ucBit;                                         // set the input low
                PTT &= ~ucBit;
            }       
        }
        break;

    case _PORT_AD:
        if (!(ATDDIEN & ucBit)) {                                        // only change if really an input
            if (iChange == TOGGLE_INPUT) {
                puPAD ^= ucBit;                                          // change pullup/down value so that input will change
                PORTAD ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                puPAD |= ucBit;
                PORTAD |= ucBit;                                        // set the input high
            }
            else  {
                puPAD &= ~ucBit;                                         // set the input low
                PORTAD &= ~ucBit;
            }       
        }
        break;
   }
}

#ifdef ETH_INTERFACE
  extern void emac_rx_b_a_c_isr(void);                                   // NE64 ethernet receive interrupts
  extern void emac_rx_b_b_c_isr(void);
  extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce) // {11}
{
 #ifdef ETH_INTERFACE                                                    // we feed frame in promiscuous mode and filter them according to their details
    static int ibuffer = 0;
    static int iOverrun = 0;
	unsigned char *ptrInput;

    if ((iForce == 0) && (!fnCheckEthernetMode(ucData, usLen))) {        // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
        return 0;
    }

    if (!ibuffer) {	                                                     // swap buffers for better testing
		if (RXAEFP) {
            iOverrun++;
			//return;                                                    // loose frame rather than overwrite
		}
		ptrInput = fnGetRxBufferAdd(0);
	    RXAEFP = usLen;
	}
	else {
		if (RXBEFP) {
            iOverrun++;
			//return;                                                    // loose frame rather than overwrite
		}
		ptrInput = fnGetRxBufferAdd(1);
	    RXBEFP = usLen;
	}

	while (usLen--) {
	  *ptrInput++ = *ucData++;                                           // put byte in input buffer
	}

	if (!ibuffer) {
      IEVENT |= RXACIE;
	  if (IMASK & RXACIE) {
	    emac_rx_b_a_c_isr();
	  }
	}
	else {
      IEVENT |= RXBCIE;
	  if (IMASK & RXBCIE) {
	    emac_rx_b_b_c_isr();
	  }
	}

	ibuffer ^= 1;
    return 1;
 #else
    return 0;
 #endif
}

// We can update port state displays if we want
extern void fnSimPorts(void)
{
}

extern void Timer_7_int(void);

// we can simulate timers during a tick here if required
extern int fnSimTimers(void)
{
    static unsigned long COP_Timeout = 0;
    if (TSCR1 & TEN) {                                                   // if timer enabled
        unsigned long ulPulse = ((BUS_CLOCK / 1000) * 50);               // pulses expected in 50 ms
        switch (TSCR2 & 0x07) {
        case BUS_CLOCK_128:
            ulPulse /= 128;
            break;

        default:
            break;
        }
        ulPulse = TCNT_HI + ulPulse;
        if (ulPulse >= TC7_HI) {
          if (TSCR2 & TCRE) {
              TCNT_HI = 0;                                               // simulate the counter having been reset on match
          }
  #ifdef GLOBAL_HARDWARE_TIMER
          Timer_7_int();                                                 // simulate interrupt
  #endif
        }
        else {
            TCNT_HI += (unsigned short)ulPulse;
        }
    }

    if (ARMCOP && (ARMCOP != 0xaa)) {                                    // check for a false feed sequence, which leads to a reset
        return RESET_SIM_CARD;                                           // signal commanded reset
    }
    if (COPCTL & 0x07) {                                                 // if watchdog active
        if (ARMCOP == 0xaa) {                                            // assue watchdog has been triggered
            COP_Timeout = 0;
        }
        else {
            unsigned long ulTimeout;
            switch (COPCTL & 0x07) {
            case 0x01:
                ulTimeout = (0x01<<14);
                break;
            case 0x02:
                ulTimeout = (0x01<<16);
                break;
            case 0x03:
                ulTimeout = (0x01<<18);
                break;
            case 0x04:
                ulTimeout = (0x01<<29);
                break;
            case 0x05:
                ulTimeout = (0x01<<22);
                break;
            case 0x06:
                ulTimeout = (0x01<<23);
                break;
            case 0x07:
                ulTimeout = (0x01<<24);
                break;               
            }
            COP_Timeout += (OSCCLK/1000)*TICK_RESOLUTION;                // clocks in a TICK period
            if (COP_Timeout > ulTimeout) {
                return RESET_CARD_WATCHDOG;                              // signal watchdog fired
            }
        }
        ARMCOP = 0;                                                      // mark that we have read
    }

    return 0;
}


static unsigned char fnGetPorts(int iPort, unsigned char *ptrBitSet)
{
    unsigned char ucPeripherals = 0;
    unsigned char ucFunctionNumber = 2;

    switch (iPort) {
    case _PORT_A:                                                        // Port A
    case _PORT_B:                                                        // Port B
    case _PORT_E:                                                        // Port E
    case _PORT_K:                                                        // Port K
        break;

    case _PORT_T:
        if (TCTL1 & 0xc0) {
            ucPeripherals = 0x80;                                        // Timer output T7
        }
        if (TCTL1 & 0x30) {
            ucPeripherals |= 0x40;                                       // Timer output T6
        }
        if (TCTL1 & 0x0c) {
            ucPeripherals |= 0x20;                                       // Timer output T5
        }
        if (TCTL1 & 0x03) {
            ucPeripherals |= 0x10;                                       // Timer output T4
        }
        break;                                                    

    case _PORT_G:                                                        // Port G
        if (NETCTL & EXTPHY) {
            ucPeripherals = 0x7f;                                        // MII interface active 
        }
        break;

    case _PORT_H:                                                        // Port H
        if (NETCTL & EXTPHY) {
            ucPeripherals = 0x7f;                                        // MII interface active 
        }
        ucFunctionNumber = 3;
        break;

    case _PORT_J:                                                        // Port J
        if (NETCTL & EXTPHY) {
            ucPeripherals = 0x0f;                                        // MII interface active 
        }
        if (IBCR & IBEN) {
            ucPeripherals |= 0xc0;                                       // IIC interface active
        }
        break;

    case _PORT_L:                                                        // Port L
        if (EPHYCTL0 & LEDEN) {
            ucPeripherals = 0x1f;                                        // Ethernet activity LEDs active
        }
        break;

    case _PORT_S:
        if (SCICR2_0 & TE) {
            ucPeripherals = 0x02;                                        // SCI 0 Tx
        }
        if (SCICR2_0 & RE) {
            ucPeripherals |= 0x01;                                       // SCI 0 Rx
        }
        if (SCICR2_1 & TE) {
            ucPeripherals |= 0x08;                                       // SCI 1 Tx
        }
        if (SCICR2_1 & RE) {
            ucPeripherals |= 0x04;                                       // SCI 1 Rx
        }
        if (SPICR1 & SPE) {
            ucPeripherals |= 0x70;                                       // SPI
        }
        if (SPICR1 & SSOE) {
            ucPeripherals |= 0x80;                                       // SPI SS
        }
        break;

    case _PORT_AD:
        ucPeripherals = ATDDIEN;
        break;
    }

    if (ptrBitSet != 0) {
        unsigned char ucBit = 0x01;
        while (ucBit) {
            if (ucPeripherals & ucBit) {
                *ptrBitSet = ucFunctionNumber;
            }
            else {
                *ptrBitSet = 0;
            }
            ptrBitSet++;
            ucBit <<= 1;
        }
    }
    return ucPeripherals;
}

// Macro for updating ports (note that since data port is shared for holding output and input data it is necessary to set a port state before driving the line to ensure simulator accuracy)
//
#define UPDATE(port, ddr, pullup, backup_port, backup_ddr, backup_per, port_ref)  \
    ucMask = fnGetPorts(port_ref, 0);                       \
    port &= (ddr & ~ucMask);                                \
    port |= (~ddr & pullup);                                \
    if (ucMask != backup_per) {                             \
        backup_per = ucMask;                                \
        fnGetPorts(port_ref, ucPortFunctions[port_ref]);    \
        iRtn |= PORT_CHANGE;                                \
    }                                                       \
    if ((port != backup_port) || (ddr != backup_ddr)) {     \
        backup_port = port;                                 \
        backup_ddr = ddr;                                   \
        iRtn |= PORT_CHANGE;                                \
    } 


extern int fnPortChanges(int iForce)                                     // {12}
{
    static unsigned char ucPortA = 0, ucPortDDR_A = 0, ucPortPer_A = 0;
    static unsigned char ucPortB = 0, ucPortDDR_B = 0, ucPortPer_B = 0;
    static unsigned char ucPortE = 0, ucPortDDR_E = 0, ucPortPer_E = 0;
    static unsigned char ucPortG = 0, ucPortDDR_G = 0, ucPortPer_G = 0;
    static unsigned char ucPortH = 0, ucPortDDR_H = 0, ucPortPer_H = 0;
    static unsigned char ucPortJ = 0, ucPortDDR_J = 0, ucPortPer_J = 0;
    static unsigned char ucPortK = 0, ucPortDDR_K = 0, ucPortPer_K = 0;
    static unsigned char ucPortL = 0, ucPortDDR_L = 0, ucPortPer_L = 0;
    static unsigned char ucPortS = 0, ucPortDDR_S = 0, ucPortPer_S = 0;
    static unsigned char ucPortT = 0, ucPortDDR_T = 0, ucPortPer_T = 0;
    static unsigned char ucPortAD = 0, ucPortPer_AD = 0;

    unsigned char ucMask;

    int iRtn = iFlagRefresh;
    iFlagRefresh = 0;

    UPDATE(PORTA, DDRA, puPTA, ucPortA, ucPortDDR_A, ucPortPer_A, _PORT_A); 
    UPDATE(PORTB, DDRB, puPTB, ucPortB, ucPortDDR_B, ucPortPer_B, _PORT_B); 
    UPDATE(PORTE, DDRE, puPTE, ucPortE, ucPortDDR_E, ucPortPer_E, _PORT_E); 
    UPDATE(PORTK, DDRK, puPTK, ucPortK, ucPortDDR_K, ucPortPer_K, _PORT_K); 
    UPDATE(PTG,   DDRG, puPTG, ucPortG, ucPortDDR_G, ucPortPer_G, _PORT_G);       
    UPDATE(PTH,   DDRH, puPTH, ucPortH, ucPortDDR_H, ucPortPer_H, _PORT_H);   
    UPDATE(PTJ,   DDRJ, puPTJ, ucPortJ, ucPortDDR_J, ucPortPer_J, _PORT_J);     
    UPDATE(PTL,   DDRL, puPTL, ucPortL, ucPortDDR_L, ucPortPer_L, _PORT_L); 
    UPDATE(PTS,   DDRS, puPTS, ucPortS, ucPortDDR_S, ucPortPer_S, _PORT_S);
    UPDATE(PTT,   DDRT, puPTT, ucPortT, ucPortDDR_T, ucPortPer_T, _PORT_T);


    if (ucPortPer_AD != ATDDIEN) {
        ucPortPer_AD = ATDDIEN;
        fnGetPorts(_PORT_AD, ucPortFunctions[_PORT_AD]);
    }
    PORTAD &= ATDDIEN;
    PORTAD |= (~ATDDIEN & puPAD);
    if ((PORTAD & ATDDIEN) != ucPortAD) {
        ucPortAD = (PORTAD & ATDDIEN);
        iRtn |= PORT_CHANGE;
    } 
    return iRtn;
}

extern unsigned long fnGetPresentPortState(int portNr)
{
    switch (portNr-1) {
    case _PORT_A:
    return PORTA;
    case _PORT_B:
    return PORTB;
    case _PORT_E:
    return PORTE;
    case _PORT_G:
    return (PTG);
    case _PORT_H:
    return (PTH);
    case _PORT_J:
    return (PTJ);
    case _PORT_K:
    return PORTK;
    case _PORT_L:
    return (PTL);
    case _PORT_S:
    return (PTS);
    case _PORT_T:
    return ((PTT) & 0xf0);
    case _PORT_AD:
    return (PORTAD);
    default:
        return 0;
    }
}

extern unsigned long fnGetPresentPortDir(int portNr)
{
    switch (portNr-1) {
    case _PORT_A:
    return DDRA;
    case _PORT_B:
    return DDRB;
    case _PORT_E:
    return DDRE;
    case _PORT_G:
    return (DDRG);
    case _PORT_H:
    return (DDRH);
    case _PORT_J:
    return (DDRJ);
    case _PORT_K:
    return DDRK;
    case _PORT_L:
    return (DDRL);
    case _PORT_S:
    return (DDRS);
    case _PORT_T:
    return ((DDRT) & 0xf0);
    case _PORT_AD:
    return (0);
    default:
        return 0;
    }
}



extern unsigned long fnGetPresentPortPeriph(int iPort)
{
    return fnGetPorts(iPort-1, 0);
}

extern void ephy_isr(void);

extern void fnSimulateLinkUp(void)
{
#ifdef ETH_INTERFACE
    ephy_isr();                                                          // simulate link up 
#endif
    fnUpdateIPConfig();                                                  // Update display in simulator
}


#endif
/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	    SAM7XSim.c
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2015
    *********************************************************************
    01.03.2007 Added fnGetFlash() routine
    12.04.2007 Added fnPutFlashAdd() routine
    28.09.2007 Add external interrupt simulation support - IRQ0          {1}
    10.10.2007 Allow application calling of interrupt simulation without argument list {2}
    02.11.2007 Add fnSimulateModemChange() for compatibility             {3}
    03.11.2007 Extend support of timers to include 0 and 1               {4}
    14.11.2007 Force port redraw on toggle changes                       {5}
    14.11.2007 Update interrupt masks from set and clear registers       {6}
    14.11.2007 Add fnSimulateSPIIn()                                     {7}
    17.11.2007 Simulate PIT rather than use RealTimeInterrupt()          {8}
    19.11.2007 Add FIQ interrupt                                         {9}
    15.12.2007 Add IIC simulation speed limitation for increased accuracy{10}
    16.12.2007 Allow SPI and FLASH files system to work together         {11}
    23.12.2007 Add USART peripheral DMA operation                        {12}
    15.01.2008 Correct WDT_MR initialisation value                       {13}
    14.06.2008 Add USB initialisation values                             {14}
    14.06.2008 Add EMAC rx and tx flags when simulating                  {15}
    02.08.2008 Add dummy fnSimulateBreak()                               {16}
    05.10.2008 Add DBGU UART interface                                   {17}
    06.10.2008 Add USB simulation support                                {18}
    06.10.2008 Remove power clock simulation due to integration in POWER_UP/POWER_DOWN macros {19}
    08.10.2008 Set interrupt bit when simulating PIT interrupt           {20}
    18.10.2008 Correct prot B peripheral simulation                      {21}
    24.12.2008 Add ADC support                                           {22}
    24.12.2008 Check whether timers are powered up before simulating them{23}
    28.12.2008 Add timer TIOA output monitoring                          {24}
    14.01.2009 Modify port simulation input logic to respect peripheral overrides {25}
    14.01.2009 Add IRQ1 interrupt (PA14)                                 {26}
    28.01.2009 Add parameter to fnGetFileSystemStart() and fnGetFlashSize() {27}
    01.02.2009 Improve PIT simulation using float and PIVR counting      {28}
    08.02.2009 Add iForce parameter to fnSimulateEthernetIn()            {29}
    28.02.2009 Add RTS handling on UART0 and UART1                       {30}
    03.03.2009 Add idle line handling on UART0 and UART1                 {31}
    03.03.2009 Add iForce parameter to fnPortChanges()                   {32}
    17.04.2009 Improve flow control in DMA mode                          {33}
    17.04.2009 Add modem control signal simulation                       {34}
    18.04.2009 Add RTS handling on UART2 (BDGU)                          {35}
    10.08.2009 Initialise iType in fnSimulateEthernetIn()                {36}
    16.08.2009 Support directly connected keypad (non-matrix)            {37}
    17.08.2009 Add SSC support                                           {38}
    22.10.2009 Add external UART transmit interrupt support              {39}
    11.01.2010 Add unicast and multicast reception flags                 {40}
    13.02.2010 Add SAM7S                                                 {41}
    16.02.2010 Allow Ethernet simulation without PHY interrupt           {42}
    17.05.2010 Reset PIO_ASR_X and PIO_BSR_X after assigning             {43}
    12.10.2010 Add SAM7SExxx                                             {44}
    09.02.2011 Add RTT                                                   {45}
    27.10.2011 Add fnInitInternalRTC()                                   {46}
    27.10.2011 Add RTT prescaler and handle RTT increment interrupt      {47}
    06.12.2011 Handle changes to USART reception timeout values with ports {48}
    26.01.2013 Add SAM7L                                                 {49}
    16.06.2015 Add hardware operating details to simulator status bar    {50}

*/  


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
#include "config.h"

#ifdef _HW_SAM7X


/**************************************************************************************************/
/********************************* ATMEL AT91SAM7X128/256******************************************/
/**************************************************************************************************/

static unsigned long porta_in;
static unsigned long portb_in;
static unsigned long portc_in;                                           // {44}

static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = { 0 };

#ifdef SUPPORT_ADC                                                       // {22}
    static unsigned long ulADC_values[ADC_CHANNELS];
    static void *ptrPorts[3] = {(void *)ucPortFunctions, (void *)ulADC_values, (void *)(ADC_PERIPHERAL_BLOCK + 0x4)};
#endif

static unsigned long ulTIOA = 0;                                         // {24}

static int iFlagRefresh = 0;
static unsigned short usIdleCounter0;                                    // {31}
static unsigned short usIdleCounter1;

#ifdef USB_INTERFACE                                                     // {18}
    static unsigned long ulEndpointInt = 0;    
    static int iRxDataBank[NUMBER_OF_USB_ENDPOINTS] = {0};
    static unsigned long ulTxDataBank[NUMBER_OF_USB_ENDPOINTS] = {0};
    static int iTxDataToggle[NUMBER_OF_USB_ENDPOINTS] = {0};
#endif

#define ASSERT_RTS_0    0x01
#define NEGATE_RTS_0    0x02
#define ASSERT_RTS_1    0x04
#define NEGATE_RTS_1    0x08

#ifdef _HW_SAM7S                                                         // {41}
    #define ADC_PORT_NUMBER    1
    #define ADC_FIRST_PORT_BIT 17
#else
    #define ADC_PORT_NUMBER    2
    #define ADC_FIRST_PORT_BIT 27
#endif

// Initialisation fo all non-zero registers in the device
//
static void fnSetDevice(unsigned long *ulPortInits)
{
    extern void fnEnterHW_table(void *hw_table);

    ADC_SR    = (ADC_ENDRX | ADC_RXBUFF);
    PIT_MR    = 0x000fffff;
    PMC_SCSR  = 0x01;
    CKGR_PLLR = 0x3f00;
    PMC_SR    = 0x08;
    WDT_MR    = 0x3fff2fff;                                              // {3}
    PIO_PSR_A = 0xffffffff;
    #ifndef _HW_SAM7S                                                    // {41}
    PIO_PSR_B = 0xffffffff;
    #endif
    SPI_SR_0  = 0x000000f0;
    #if defined _HW_SAM7SE || defined _HW_SAM7L                          // {44}{49}
    PIO_PSR_C = 0xffffffff;
    #elif !defined _HW_SAM7S                                             // SAM7SE and SAM7S dont have SPI1
    SPI_SR_1  = 0x000000f0;
    #endif

    #ifdef _HW_SAM7SE                                                    // {44}
    SMC_CSR0 = 0x00002000;                                               // Static Memory Controller
    SMC_CSR1 = 0x00002000;
    SMC_CSR2 = 0x00002000;
    SMC_CSR3 = 0x00002000;
    SMC_CSR4 = 0x00002000;
    SMC_CSR5 = 0x00002000;
    SMC_CSR6 = 0x00002000;
    SMC_CSR7 = 0x00002000;
    #endif


    TWI_SR = 0x0008;

    RTT_MR = 0x00008000;                                                 // {45}
    RTT_AR = 0xffffffff;

    UDP_FADDR = FEN;                                                     // {14}
    UDP_IMR   = (ENDBUSRES | RXRSM);

    PIO_PDSR_A = porta_in = *ulPortInits++;
    portb_in = *ulPortInits++;
    #ifndef _HW_SAM7S                                                    // {41}
    PIO_PDSR_B = portb_in;
    #endif
    #if defined _HW_SAM7SE || defined _HW_SAM7L                          // {44}{49}
    portc_in = *ulPortInits++;
    PIO_PDSR_C = portc_in;
    #endif

    US_CSR_0 = CTS_HIGH;                                                 // {34} default input is high
    US_CSR_1 = CTS_HIGH;

    DBGU_CIDR = (NVPTYPE_EMBEDDED_FLASH_MEMORY | ARCH_AT91SAM7X_SERIES | EPROC_ARM7TDMI | VERSION_ID); // {17}
#if SIZE_OF_FLASH == (64*1024)
    DBGU_CIDR |= NVPSIZ_64K;
#elif SIZE_OF_FLASH == (128*1024)
    DBGU_CIDR |= NVPSIZ_128K;
#elif SIZE_OF_FLASH == (256*1024)
    DBGU_CIDR |= NVPSIZ_256K;
#elif SIZE_OF_FLASH == (512*1024)
    DBGU_CIDR |= NVPSIZ_512K;
#endif

#if SIZE_OF_RAM == (32*1024)
    DBGU_CIDR |= SRAM_SIZ_32K;
#elif SIZE_OF_RAM == (64*1024)
    DBGU_CIDR |= SRAM_SIZ_64K;
#elif SIZE_OF_RAM == (128*1024)
    DBGU_CIDR |= SRAM_SIZ_128K;
#elif SIZE_OF_RAM == (256*1024)
    DBGU_CIDR |= SRAM_SIZ_256K;
#endif

    UDP_FADDR = FEN;                                                     // {18}
    UDP_IMR   = (ENDBUSRES | RXRSM);

    SSC_SR    = (SSC_RX_BUFF_FULL | SSC_RX_END | SSC_TX_BUF_EMPTY | SSC_TX_END); // {38}

#ifdef SUPPORT_ADC
    memcpy(ulADC_values, ulPortInits, sizeof(ulADC_values));             // {22} prime initial ADC values
    fnEnterHW_table(ptrPorts);                                           
#else
    fnEnterHW_table(ucPortFunctions);
#endif
}


unsigned char ucFLASH[SIZE_OF_FLASH];

void fnInitialiseDevice(void *port_inits)
{
    memset(ucFLASH, 0xff, sizeof(ucFLASH));                              // we start with deleted FLASH memory contents
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned long *)port_inits);                            // set device registers to startup condition (if not zerod)
}

void fnDeleteFlashSector(unsigned char *ptrSector)
{
	int i = FLASH_GRANULARITY;

	while (i--) {
		*ptrSector++ = 0xff;
	}
}

// Enable simulation of direct FLASH access
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)START_OF_FLASH;
    return (ucAdd + sim_add);
}

extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)START_OF_FLASH;
    return (ucAdd - sim_add);
}

extern unsigned char *fnGetEEPROM(unsigned short usOffset);

extern unsigned char *fnGetFileSystemStart(int iMemory)              // {27}
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {11}
    return (fnGetEEPROM(uFILE_START));
#elif defined (SAVE_COMPLETE_FLASH)
    return (&ucFLASH[0]);
#else
    return (&ucFLASH[uFILE_START-FLASH_START_ADDRESS]);
#endif
}

extern unsigned long fnGetFlashSize(int iMemory)                     // {27}
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {11}
    return (fnGetEEPROMSize());
#elif defined (SAVE_COMPLETE_FLASH)
    return (sizeof(ucFLASH));
#else
    return (FILE_SYSTEM_SIZE);
#endif
}



/******************************************************************************************************/

// A copy of the SAM7X peripheral registers for simulation purposes
//
extern SAM7X_PERIPH ucSAM7x = {0};

extern unsigned char fnMapPortBit(unsigned long ulRealBit)
{
#if defined _HW_SAM7S || defined _HW_SAM7SE || defined _HW_SAM7L         // {44}{49}
    unsigned long ulBit = 0x80000000;
#else
    unsigned long ulBit = 0x40000000;
#endif
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

// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{   
    static int iRTS_action = 0;
    unsigned long ulNewActions = 0;
    int *ptrCnt;
    iInts &= ~PSEUDO_INT;

    if ((iInts & CHANNEL_0_SERIAL_INT) && (argv)) {                      // {2}
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
	            fnLogTx0((unsigned char)US_THR_0);
                ulNewActions |= SEND_COM_0;
                iRTS_action |= NEGATE_RTS_0;
		        US_CSR_0 |= (TXRDY);                                     // simulate Tx int 
                if ((US_MR_0 & 0x0000c000) == 0x00008000) {              // check whether we are in tx loop back mode
                    US_RHR_0 = US_THR_0;                                 // put tx byte in rx buffer
			        US_CSR_0 |= (RXRDY);                                 // and Rx due to loop
		        }
                AIC_IPR |= USART0;
                interrupt_call = (void (*)(void))AIC_SVR6;
                interrupt_call();                                        // call UART 0 int handler
#endif
            }
        }
	}

	if ((iInts & CHANNEL_1_SERIAL_INT) && (argv)) {                      // {2}
        ptrCnt = (int *)argv[THROUGHPUT_UART1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
#ifdef SERIAL_INTERFACE
                void (*interrupt_call)(void);
#endif
		        iInts &= ~CHANNEL_1_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx1((unsigned char)US_THR_1);
                ulNewActions |= SEND_COM_1;
                iRTS_action |= NEGATE_RTS_1;
		        US_CSR_1 |= (TXRDY);                                     // simulate Tx int 
                if ((US_MR_1 & 0x0000c000) == 0x00008000) {              // check whether we are in tx loop back mode
                    US_RHR_1 = US_THR_1;                                 // put tx byte in rx buffer
			        US_CSR_1 |= (RXRDY);                                 // and Rx due to loop
		        }
                AIC_IPR |= USART1;
                interrupt_call = (void (*)(void))AIC_SVR7;
                interrupt_call();                                        // simulate UART1 int  
#endif
            }
        }
	}
    if ((!(iInts & CHANNEL_0_SERIAL_INT)) || (iMasks & CHANNEL_0_SERIAL_INT)) { // not going to enter again this time so react to RTS changes
        if (NEGATE_RTS_0 & iRTS_action) {                                // {30}
            iRTS_action &= ~NEGATE_RTS_0;
            fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                      // negate control line (RS485 uses negative logic)
#if defined _HW_SAM7L                                                    // {49}
            portc_in |= RTS_0;
#else
            porta_in |= RTS_0;
#endif
            if (!(iMasks & CHANNEL_0_SERIAL_INT)) {                      // if transmission has terminated queue assertion
                iRTS_action |= ASSERT_RTS_0;
            }
        }
        else if (ASSERT_RTS_0 & iRTS_action) {
            iRTS_action &= ~ASSERT_RTS_0;
            fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                      // negate control line (RS485 uses negative logic)
#if defined _HW_SAM7L                                                    // {49}
            portc_in &= ~RTS_0;
#else
            porta_in &= ~RTS_0;
#endif
        }
    }
    if ((ASSERT_RTS_0 | NEGATE_RTS_0 | ASSERT_RTS_1 | NEGATE_RTS_1) & iRTS_action) {
        iInts |= PSEUDO_INT;                                             // add a pseudo interrupt so we are called on the next tick
        iMasks |= PSEUDO_INT;                                            // never enter immediately due to a speudo-interrupt
    }

	if ((iInts & CHANNEL_2_SERIAL_INT) && (argv)) {                      // {17}
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
	            fnLogTx2((unsigned char)DBGU_THR);
                ulNewActions |= SEND_COM_2;
		        DBGU_SR |= (TXRDY);                                      // simulate Tx int 
                if ((DBGU_MR & 0x0000c000) == 0x00008000) {              // check whether we are in tx loop back mode
                    DBGU_RHR = US_THR_1;                                 // put tx byte in rx buffer
			        DBGU_SR |= (RXRDY);                                  // and Rx due to loop
		        }
                AIC_IPR |= SYSIRQ;
                interrupt_call = (void (*)(void))AIC_SVR1;
                interrupt_call();                                        // simulate UART1 int  
#endif
            }
        }
	}

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {39}
	if ((iInts & CHANNEL_0_EXT_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx0() != 0) {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_0;
            }
        }
	}
	if ((iInts & CHANNEL_1_EXT_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_1_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx1() != 0) {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_1;
            }
        }
	}
    #if NUMBER_EXTERNAL_SERIAL > 2
	if ((iInts & CHANNEL_2_EXT_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART2];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_2_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx2() != 0) {
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_2;
            }
        }
	}
	if ((iInts & CHANNEL_3_EXT_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART3];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_3_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx3() != 0) {
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_3;
            }
        }
	}
    #endif
#endif

    if (iInts & IIC_INT0) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];                           // {10}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= IIC_INT0;                                      // enough IIC interupts handled in this tick period
            }
            else {
#ifdef IIC_INTERFACE
		        iInts &= ~IIC_INT0;
                if (TWI_IMR & (TWI_TXRDY | TWI_RXRDY | TWI_TXCOMP)) {
                    void (*interrupt_call)(void);
                    interrupt_call = (void (*)(void))AIC_SVR9;
                    interrupt_call();                                    // simulate IIC int                                                 
                }
#endif
            }
        }
    }
#ifdef USB_INTERFACE                                                     // {18}
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
#ifdef SSC_INTERFACE                                                     // {38}
    if (iInts & CHANNEL_0_SSC_INT) {
        iInts &= ~CHANNEL_0_SSC_INT;
        SSC_SR |= SSC_TX_READY;                                          // data transmitted and holding register free
        if (SSC_IMR & SSC_TX_READY) {                                    // if interrupt enabled
            void (*interrupt_call)(void);
            AIC_IPR |= SSC;
            interrupt_call = (void (*)(void))AIC_SVR8;
            interrupt_call();                                            // simulate SSC int
        }
    }
#endif
    return ulNewActions;
}

// Process simulated Peripheral DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    static int iRTS_action = 0;
    unsigned long ulNewActions = 0;
#ifdef SERIAL_INTERFACE
    int *ptrCnt;

    if (iDMA & DMA_CONTROLLER_0) {                                       // USART 0 Tx DMA {12}
        if (US_PTSR_0 & PDC_TXTEN) {                                     // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[0];
            while (US_TCR_0 != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_0;                          // no futher transmissions in this interval
                    break;
                }
                if ((US_MR_0 & 0x7) == USART_MODE_RS485) {               // {30} if in RS485 mode automatically control the RTS line
                    iRTS_action |= NEGATE_RTS_0;
                }
                US_THR_0 = *(unsigned char *)US_TPR_0;
                US_TPR_0 = US_TPR_0 + 1;
                fnLogTx0((unsigned char)US_THR_0);
                ulNewActions |= SEND_COM_0;
                (US_TCR_0)--;
                --(*ptrCnt);                                             // limit transmission in this TICK interval
            }
            if ((iMasks & DMA_CONTROLLER_0) == 0) {
                iDMA &= ~DMA_CONTROLLER_0;                               // transfer complete
                US_CSR_0 |= (ENDTX);                                     // simulate Tx int
                if ((US_MR_0 & 0x7) == USART_MODE_RS485) {               // {30} if in RS485 mode automatically control the RTS line
                    iRTS_action |= ASSERT_RTS_0;
                }
                if (US_IMR_0 & ENDTX) {                                  // if interrupt enabled
                    void (*interrupt_call)(void);
                    AIC_IPR |= USART0;
                    interrupt_call = (void (*)(void))AIC_SVR6;
                    interrupt_call();                                    // call UART 0 int handler
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_0;                                  // {33}
        }
    }
    if (iDMA & DMA_CONTROLLER_2) {                                       // USART 1 Tx DMA
        if (US_PTSR_1 & PDC_TXTEN) {                                     // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[1];
            while (US_TCR_1 != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_2;                          // no futher transmissions in this interval
                    break;
                }
                if ((US_MR_1 & 0x7) == USART_MODE_RS485) {               // {30} if in RS485 mode automatically control the RTS line
                    iRTS_action |= NEGATE_RTS_1;                
                }
                US_THR_1 = *(unsigned char *)US_TPR_1;
                US_TPR_1 = US_TPR_1 + 1;
                fnLogTx1((unsigned char)US_THR_1);
                ulNewActions |= SEND_COM_1;
                (US_TCR_1)--;
                --(*ptrCnt);                                             // limit transmission in this TICK interval
            }
            if ((iMasks & DMA_CONTROLLER_2) == 0) {
                iDMA &= ~DMA_CONTROLLER_2;                               // transfer complete
                US_CSR_1 |= (ENDTX);                                     // simulate Tx int
                if ((US_MR_1 & 0x7) == USART_MODE_RS485) {               // {30} if in RS485 mode automatically control the RTS line
                    iRTS_action |= ASSERT_RTS_1;
                    fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);              // negate control lines (RS485 uses negative logic)
#if defined _HW_SAM7L                                                    // {49}
                    portc_in &= ~RTS_1;
#else
                    porta_in &= ~RTS_1;
#endif
                }
                if (US_IMR_1 & ENDTX) {                                  // if interrupt enabled
                    void (*interrupt_call)(void);
                    AIC_IPR |= USART1;
                    interrupt_call = (void (*)(void))AIC_SVR7;
                    interrupt_call();                                    // call UART 1 int handler
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_2;                                  // {33}
        }
    }
    if (NEGATE_RTS_0 & iRTS_action) {                                    // {30}
        iRTS_action &= ~NEGATE_RTS_0;
        fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                          // negate control lines (RS485 uses negative logic)
        porta_in |= RTS_0;
    }
    else if (ASSERT_RTS_0 & iRTS_action) {
        iRTS_action &= ~ASSERT_RTS_0;
        fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                          // negate control lines (RS485 uses negative logic)
        porta_in &= ~RTS_0;
    }
    if (NEGATE_RTS_1 & iRTS_action) {                                    // {30}
        iRTS_action &= ~NEGATE_RTS_1;
        fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                          // negate control lines (RS485 uses negative logic)
        porta_in |= RTS_1;
    }
    else if (ASSERT_RTS_1 & iRTS_action) {
        iRTS_action &= ~ASSERT_RTS_1;
        fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                          // negate control lines (RS485 uses negative logic)
        porta_in &= ~RTS_1;
    }
    #ifdef DBGU_UART                                                     // {17}
    if (iDMA & DMA_CONTROLLER_4) {                                       // UART 2 Tx DMA (DBGU)
        if (DBGU_PTSR & PDC_TXTEN) {                                     // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[2];
            while (DBGU_TCR != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_4;                          // no futher transmissions in this interval
                    break;
                }
                DBGU_THR = *(unsigned char *)DBGU_TPR;
                DBGU_TPR = DBGU_TPR + 1;
                fnLogTx2((unsigned char)DBGU_THR);
                ulNewActions |= SEND_COM_2;
                (DBGU_TCR)--;
                --(*ptrCnt);                                             // limit transmission in this TICK interval
            }
            if ((iMasks & DMA_CONTROLLER_4) == 0) {
                iDMA &= ~DMA_CONTROLLER_4;                               // transfer complete
                DBGU_SR |= (ENDTX);                                      // simulate Tx int             
                if (DBGU_IMR & ENDTX) {                                  // if interrupt enabled
                    void (*interrupt_call)(void);
                    AIC_IPR |= SYSIRQ;
                    interrupt_call = (void (*)(void))AIC_SVR1;
                    interrupt_call();                                    // call UART 2 (DBGU) int handler
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_4;                                  // {33}
        }
    }    
    #endif
#endif
#ifdef SSC_INTERFACE                                                     // {38}
    if (iDMA & DMA_CONTROLLER_6) {                                       // SSC Tx DMA
        if (SSC_PTSR & PDC_TXTEN) {                                      // peripheral DMA transfer enabled
            unsigned long ulWordMask = 0xffffffff;
            unsigned char ucWordWidth = (unsigned char)((SSC_TFMR & 0x1f) + 1);
            ulWordMask >>= (32 - ucWordWidth);
            ucWordWidth = ((ucWordWidth + 7)/8);
            while (SSC_TCR != 0) {
                switch (ucWordWidth) {
                case 1:
                    SSC_THR = *(unsigned char *)SSC_TPR;
                    SSC_TPR = (SSC_TPR + 1);
                    break;
                case 2:
                    SSC_THR = *(unsigned short *)SSC_TPR;
                    SSC_TPR = (SSC_TPR + 2);
                    break;
                case 3:
                case 4:
                    SSC_THR = *(unsigned long *)SSC_TPR;
                    SSC_TPR = (SSC_TPR + 4);
                    break;
                }
                fnLogSSC0((SSC_THR & ulWordMask), ucWordWidth);
                if (SSC_RFMR & SSC_RX_LOOP_ENABLE) {                     // loop tx to rx
                    if (SSC_SR & SSC_RX_ENABLED) {                       // receive enabled
                        SSC_RHR = (SSC_THR);
                        if ((SSC_PTSR & PDC_RXTEN) && (SSC_RCR != 0)) {  // DMA enable for reception
                            unsigned long ulRxWordMask = 0xffffffff;
                            unsigned char ucRxWordWidth = (unsigned char)((SSC_RFMR & 0x1f) + 1);
                            ulRxWordMask >>= (32 - ucRxWordWidth);
                            ucRxWordWidth = ((ucRxWordWidth + 7)/8);                          
                            switch (ucRxWordWidth) {
                            case 1:
                                *(unsigned char *)SSC_RPR = (unsigned char)SSC_RHR;
                                SSC_RPR = (SSC_RPR + 1);
                                break;
                            case 2:
                                *(unsigned short *)SSC_RPR = (unsigned short)SSC_RHR;
                                SSC_RPR = (SSC_RPR + 2);
                                break;
                            case 3:
                            case 4:
                                *(unsigned long *)SSC_RPR = SSC_RHR;
                                SSC_RPR = (SSC_RPR + 4);
                                break;
                            }
                            (SSC_RCR)--;
                            if (SSC_RCR == 0) {                          // last transfer in a frame
                                SSC_SR |= (SSC_RX_END);                  // simulate Rx int
                                if (SSC_IMR & SSC_RX_END) {              // if interrupt enabled
                                    void (*interrupt_call)(void);
                                    AIC_IPR |= SSC;
                                    interrupt_call = (void (*)(void))AIC_SVR8;
                                    interrupt_call();                    // call SSC int handler
                                }
                            }
                        }
                        else {                                           // interrupt mode could be added here
                        }
                    }
                }
                (SSC_TCR)--;
            }
            fnLogSSC0(0, 0);
            iDMA &= ~DMA_CONTROLLER_6;                                   // transfer complete (always considered complete after a single TICK period)
            SSC_SR |= (SSC_TX_END);                                      // simulate Tx int
            if (SSC_IMR & SSC_TX_END) {                                  // if interrupt enabled
                void (*interrupt_call)(void);
                AIC_IPR |= SSC;
                interrupt_call = (void (*)(void))AIC_SVR8;
                interrupt_call();                                        // call SSC int handler
            }
        }
    }
#endif
    return ulNewActions;
}

// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
 #ifdef SERIAL_INTERFACE
    void (*interrupt_call)(void);

    if (iPort == 1) {
        if (US_RTOR_1 != 0) {                                            // {31}
            usIdleCounter1 = (unsigned short)US_RTOR_1;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_1 |= RXRDY;
		    US_RHR_1 = *ptrDebugIn++;
            if (!(US_IMR_1 & RXRDY)) {                                   // no RXRDY interrupt masked
                if (US_RCR_1 != 0) {                                     // {12} DMA reception
                    *(unsigned long *)US_RPR_1 = US_RHR_1;
                    (US_RPR_1)++;
                    (US_RCR_1)--;
                    if (US_RCR_1 == 0) {                                 // last transfer
                        US_CSR_1 |= ENDRX;
                        if ((US_IMR_1 & ENDRX) == 0) {
                            continue;                                    // no interrupt enabled
                        }
                    }
                    else {
                        continue;
                    }
                }
            }
            AIC_IPR |= USART1;
            if (AIC_IMR & USART1) {
                interrupt_call = (void (*)(void))AIC_SVR7;
                interrupt_call();
            }
	    }
    }
    #ifdef DBGU_UART                                                     // {17} 
    else if (iPort == 2) {
	    while (usLen--) {
            DBGU_SR |= RXRDY;
		    DBGU_RHR = *ptrDebugIn++;
            if (!(DBGU_IMR & RXRDY)) {                                   // no RXRDY interrupt masked
                if (DBGU_RCR != 0) {                                     // {12} DMA reception
                    *(unsigned long *)DBGU_RPR = DBGU_RHR;
                    (DBGU_RPR)++;
                    (DBGU_RCR)--;
                    if (DBGU_RCR == 0) {                                 // last transfer
                        DBGU_SR |= ENDRX;
                        if ((DBGU_IMR & ENDRX) == 0) {
                            continue;                                    // no interrupt enabled
                        }
                    }
                    else {
                        continue;
                    }
                }
            }
            AIC_IPR |= SYSIRQ;
            if (AIC_IMR & SYSIRQ) {
                interrupt_call = (void (*)(void))AIC_SVR1;
                interrupt_call();
            }
	    }
    }
    #endif
    else if (iPort == 0) {
        if (US_RTOR_0 != 0) {                                            // {31}
            usIdleCounter0 = (unsigned short)US_RTOR_0;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_0 |= RXRDY;
		    US_RHR_0 = *ptrDebugIn++;
            if (!(US_IMR_0 & RXRDY)) {                                   // no RXRDY interrupt masked
                if (US_RCR_0 != 0) {                                     // {12} DMA reception
                    *(unsigned long *)US_RPR_0 = US_RHR_0;
                    (US_RPR_0)++;
                    (US_RCR_0)--;
                    if (US_RCR_0 == 0) {                                 // last transfer
                        US_CSR_0 |= ENDRX;
                        if ((US_IMR_0 & ENDRX) == 0) {
                            continue;                                    // no interrupt enabled
                        }
                    }
                    else {
                        continue;
                    }
                }
            }
            AIC_IPR |= USART0;
            if (AIC_IMR  & USART0) {
                interrupt_call = (void (*)(void))AIC_SVR6;
                interrupt_call();
            }
	    }
    }
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {39}
    else {
        extern int fnRxExtSCI(int iChannel, unsigned char *ptrData, unsigned short usLength);
        int iHandled;
        while ((iHandled = fnRxExtSCI((iPort - NUMBER_SERIAL), ptrDebugIn, usLen)) > 0) { // handle reception of each byte
            switch (iPort - NUMBER_SERIAL) {                             // generate an interrupt on the corresponding port line (assumed negative)
            case 0:
            case 1:
                fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                break;
        #if NUMBER_EXTERNAL_SERIAL > 2
            case 2:
            case 3:
                fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters              
                break;
        #endif
            }
            ptrDebugIn += iHandled;
            usLen -= iHandled;
        }
    }
    #endif
 #endif
}

#ifdef SPI_SIM_INTERFACE
// Simulate the reception of SPI slave data by inserting bytes into the input buffer and calling interrupts {7}
//
extern void fnSimulateSPIIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
    void (*interrupt_call)(void);

    if (iPort == 1) {
        if (SPI_CR_1 & SPIEN) {                                          // only accept if the interface is enabled
            if (SPI_MR_1 & SPI_MSTR) {                                   // no unsolicited reception in master mode
                return;
            }
            if (SPI_CSR0_1 & 0xf0) {                                     // check whether the reception length is longer than 8 bits
                static int iCount = 0;
                unsigned char ucMask = (unsigned char)(SPI_CSR0_1 >> 4);
                ucMask = (0xff >> (8 - ucMask));
                while (usLen--) {
                    SPI_RDR_1 <<= 8;                                     // clear space for first byte
                    if (iCount == 0) {
                        iCount = 1;
                        SPI_RDR_1 |= (*ptrDebugIn++ & ucMask);           // add first bits
                    }
                    else {
                        iCount = 0;
                        SPI_RDR_1 |= *ptrDebugIn++;                      // add last 8 bits
                        SPI_SR_1 |= SPI_RDRF;                            // set rx ready flag
                        AIC_IPR |= SPI1;
                        if (AIC_IMR  & SPI1) {                           // if interrupt is enabled call the handling routine
                            interrupt_call = (void (*)(void))AIC_SVR5;
                            interrupt_call();
                        }
                    } 
                }
            }
            else {
                while (usLen--) {
                    SPI_RDR_1 |= *ptrDebugIn++;                          // add 8 bits
                    SPI_SR_1 |= SPI_RDRF;                                // set rx ready flag
                    AIC_IPR |= SPI1;
                    if (AIC_IMR  & SPI1) {                               // if interrupt is enabled call the handling routine
                        interrupt_call = (void (*)(void))AIC_SVR5;
                        interrupt_call();
                    }
                }
            }
        }
    }
    else if (iPort == 0) {
    }
}
#endif

static void fnModemChange(unsigned long ulInput, int iState, int iSetPort)
{
#if defined _HW_SAM7L                                                    // {49}
    if ((!(PIO_PSR_C & ulInput)) && (!(PIO_ABSR_C & ulInput)))
#else
    if ((!(PIO_PSR_A & ulInput)) && (!(PIO_ABSR_A & ulInput)))
#endif
    {                                                                    // if input programmed for CTS operation
        unsigned char ucBit = (PORT_WIDTH - 1);
        unsigned long ulPin = ulInput;
        while (!(ulPin & 0x00000001)) {
            ulPin >>= 1;
            ucBit--;
        }
        if (iState != 0) {                                               // CTS has just been activated
            if (iSetPort != 0) {
#if defined _HW_SAM7L                                                    // {49}
                fnSimulateInputChange(2, ucBit, CLEAR_INPUT);
#else
                fnSimulateInputChange(0, ucBit, CLEAR_INPUT);
#endif
                return;
            }
            if (ulInput & CTS_0) {
                US_CSR_0 &= ~CTS_HIGH;
            }
            else {
                US_CSR_1 &= ~CTS_HIGH;
            }
        }
        else {                                                           // CTS has just been deactivated
            if (iSetPort != 0) {
#if defined _HW_SAM7L                                                    // {49}
                fnSimulateInputChange(2, ucBit, SET_INPUT);
#else
                fnSimulateInputChange(0, ucBit, SET_INPUT);
#endif
                return;
            }
            if (ulInput & CTS_0) {
                US_CSR_0 |= CTS_HIGH;
            }
            else {
                US_CSR_1 |= CTS_HIGH;
            }
        }
        if (ulInput & CTS_0) {
            US_CSR_0 |= CTSIC;                                           // flag CTS change
            if ((AIC_IMR & USART0) && (US_IMR_0 & CTSIC)) {              // if interrupt is enabled
                void (*interrupt_call)(void);
                AIC_IPR |= USART0;
                interrupt_call = (void (*)(void))AIC_SVR6;               // call UART0 interrup routine
                interrupt_call();
            }
        }
        else {
            US_CSR_1 |= CTSIC;                                           // flag CTS change
            if ((AIC_IMR & USART1) && (US_IMR_1 & CTSIC)) {              // if interrupt is enabled
                void (*interrupt_call)(void);
                AIC_IPR |= USART1;
                interrupt_call = (void (*)(void))AIC_SVR7;               // call UART1 interrup routine
                interrupt_call();
            }
        }
    }
}


extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState) // {3}
{
#ifdef SUPPORT_HW_FLOW                                                   // {34}
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect
#ifdef SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);
    switch (iPort) {
    case 0:                                                              // USART 0
        if (ulChange & MS_CTS_ON) {
            fnModemChange(CTS_0, ((ulNewState & MS_CTS_ON) != 0), 1);
        }
        break;
    case 1:                                                              // USART 1
        if (ulChange & MS_CTS_ON) {
            fnModemChange(CTS_1, ((ulNewState & MS_CTS_ON) != 0), 1);
        }
        break;
    #if NUMBER_SERIAL > 2 && defined CTS_2_PIN                           // UART 2 (BDGU)
    case 2:
        if (ulChange & MS_CTS_ON) {                                      // CTS change - set the state in the corresponding register and generate interrupt, if enabled
            unsigned long ulPin = CTS_2_PIN;
            unsigned char ucBit = (PORT_WIDTH - 1);
            unsigned char ucPort;
            while (!(ulPin & 0x00000001)) {
                ulPin >>= 1;
                ucBit--;
            }
            if (CTS_2_PORT == PIOA) {
                ucPort = 0;
            }
            else {
                ucPort = 1;
            }
            if (ulNewState & MS_CTS_ON) {                                // CTS has just been activated
                fnSimulateInputChange(ucPort, ucBit, CLEAR_INPUT);
            }
            else {                                                       // CTS has just been deactivated
                fnSimulateInputChange(ucPort, ucBit, SET_INPUT);
            }
        }
        break;
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {39}
    default:
        if (ulChange & MS_CTS_ON) {
            extern int fnExt_CTS_change(int iUART, int iChange);
            if (fnExt_CTS_change((iPort - NUMBER_SERIAL), ((ulNewState & MS_CTS_ON) != 0)) != 0) {
        #if NUMBER_EXTERNAL_SERIAL > 2
                if ((iPort - NUMBER_SERIAL) >= 2) {
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, fnMapPortBit(EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG);
                }
                else {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG);
                }
        #else
                fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG);
        #endif
            }
        }
        break;
    #endif
    }
#endif
#endif
}

// UART Break detection simulation                                       {16}
//
extern void fnSimulateBreak(int iPort)
{
}

// Dummy {8}
//
extern void RealTimeInterrupt(void)
{
}


// Simulate buffers for Ethernet use 
unsigned char ucSimRxABuf[1536];
unsigned char ucSimRxBBuf[1536];

unsigned char *fnGetSimTxBufferAdd(void)
{
#ifdef ETH_INTERFACE
    return (fnGetTxBufferAdd(0));
#else
    return 0;
#endif
}



#ifdef ETH_INTERFACE
    extern void EMAC_Interrupt(void);
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce) // {29}
{
#ifdef ETH_INTERFACE                                                     // we feed frames in promiscuous mode and filter them according to their details
    int iType = 2;                                                       // {36} default to MAC address match (only used when forcing input)
    unsigned long ulTemp;
    static int iFirstRec = 0;
    static SAM7X_RX_BD *ptrStartRxBd;
    SAM7X_RX_BD *ptrRxBd;
    unsigned short usFrameLength = usLen;
    int iFirstBuffer = 1;

    if ((iForce == 0) && ((iType = fnCheckEthernetMode(ucData, usLen)) == 0)) { // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
        return 0;         
    }

    if (iFirstRec == 0) {                                                // we play at being the EMAC receiver
        ptrStartRxBd = (SAM7X_RX_BD *)EMAC_RBQP;                         // get the first buffer descriptor address and remember it
        iFirstRec = 1;                                                   // don't let it happen again
    }
    ptrRxBd = (SAM7X_RX_BD *)EMAC_RBQP;


    ptrRxBd->bd_details = BD_INFO_START_OF_FRAME;

    while (usLen != 0) {
        ulTemp = (unsigned long)ptrRxBd->ptrBlock;

        if (ulTemp & BD_OWNERSHIP) {                                     // check ownership and ignore if the buffer is blocked
            return 0;
        }

        ulTemp &= ~(BD_RX_WRAP | BD_OWNERSHIP);                          // mask out flags to leave the address
        if (usLen >= 128) {
            uMemcpy((unsigned char*)ulTemp, ucData, 128);                // copy the receive data to the input buffer
            ucData += 128;
            usLen -= 128;
            ptrRxBd->bd_details &= ~BD_INFO_END_OF_FRAME;
        }
        else {
            uMemcpy((unsigned char *)ulTemp, ucData, usLen);             // copy the receive data to the input buffer
            usLen = 0;
        }
        if (iFirstBuffer == 0) {
            ptrRxBd->bd_details &= ~BD_INFO_START_OF_FRAME;
        }
        iFirstBuffer = 0;

        if (usLen == 0) {
            ptrRxBd->bd_details |= BD_INFO_END_OF_FRAME;                 // mark end of frame in this buffer
            if (iType == 1) {
                ptrRxBd->bd_details |= BD_INFO_GLOBAL_FRAME;             // mark broadcast frame received
            }
            else if (iType == 2) {
                ptrRxBd->bd_details |= BD_INFO_SPECIFIC_ADD_1_MATCH;     // mark MAC address match
            }
            else if (iType == 4) {                                       // {40} mark multicast frame
                ptrRxBd->bd_details |= BD_INFO_MULTICAST_HASH_MATCH;
            }
            else if (iType == 5) {                                       // {40}  mark unicast frame
                ptrRxBd->bd_details |= BD_INFO_UNICAST_HASH_MATCH;
            }
            if (usFrameLength > 1536) {
                usFrameLength = 0;
            }
            ptrRxBd->bd_details &= ~BD_INFO_LENGTH_OF_FRAME_MASK;        // clear any old info (shoudn't really be there..)
            ptrRxBd->bd_details |= usFrameLength;                        // add frame length received
            if ((unsigned short)(ptrRxBd->bd_details & BD_INFO_LENGTH_OF_FRAME_MASK) > 1536) {
                usFrameLength = 0;
            }
        }

        ulTemp = (unsigned long)ptrRxBd->ptrBlock;
        ulTemp |= (BD_OWNERSHIP);                                        // mark written to
        ptrRxBd->ptrBlock = (unsigned char *)ulTemp;

        if ((unsigned long)(ptrRxBd->ptrBlock) & BD_RX_WRAP) {           // check whether this is the last in the list
            ptrRxBd = ptrStartRxBd;
        }
        else {
            ptrRxBd++;                                                   // increment to next buffer descriptor
        }
    }

    EMAC_RBQP = (unsigned long)ptrRxBd;                                  // update the present buffer pointer
    EMAC_ISR |= RCOMP;                                                   // set interrupt flag
    EMAC_RSR |= RSR_REC;                                                 // {15} set receive status flag
    EMAC_Interrupt(); 	                                                 // simulate frame received
    return 1;
#else
    return 0;
#endif
}


// We can update port state displays if we want
//
extern void fnSimPorts(void)
{
    static unsigned long ulPerA = 0xffffffff, ulPerB = 0xffffffff, ulPerC = 0xffffffff, ulADC = 0x00000000;
    static unsigned long ulPerSelA = 0, ulPerSelB = 0, ulPerSelC = 0;

//  PMC_PCSR |= PMC_PCER;                                                // {19} update clocks (automated in POWER_UP / POWER_DOWN macros)
//  PMC_PCSR &= ~PMC_PCDR;
//  PMC_PCER = PMC_PCDR = 0;
#ifndef _HW_SAM7S                                                        // {41}
    PIO_ABSR_B |= PIO_BSR_B;                                             // update the peripheral A/B select register
    PIO_ABSR_B &= ~PIO_ASR_B;

    PIO_PSR_B |= PIO_PER_B;                                              // update the PIO status register (enable PIO and disable peripheral use)
    PIO_PSR_B &= ~PIO_PDR_B;                                             // enable peripheral and disable PIO use

    if (PMC_PCSR & PIOB) {                                               // only handle port B if clocks are enabled to it
        PIO_OSR_B |= PIO_OER_B;                                          // simple synchronisation of port direction depending on bits commanded to be set adn cleared
        PIO_OSR_B &= ~PIO_ODR_B;
        PIO_ODSR_B |= PIO_SODR_B;                                        // simple synchronisation of port state dependingf on bits commanded to be set adn cleared
        PIO_ODSR_B &= ~PIO_CODR_B;
        PIO_PDSR_B = (PIO_ODSR_B & PIO_OSR_B & PIO_PSR_B);               // {25} bits defined by outputs
        PIO_PDSR_B |= (portb_in & (~(PIO_OSR_B) | ~(PIO_PSR_B)));        // {25} bits defined by inputs
    }
    PIO_PER_B = PIO_PDR_B = PIO_OER_B = PIO_ODR_B = PIO_SODR_B = PIO_CODR_B = PIO_ASR_B = PIO_BSR_B = 0; // {43}
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    PIO_ABSR_C |= PIO_BSR_C;                                             // update the peripheral A/B select register
    PIO_ABSR_C &= ~PIO_ASR_C;

    PIO_PSR_C |= PIO_PER_C;                                              // update the PIO status register (enable PIO and disable peripheral use)
    PIO_PSR_C &= ~PIO_PDR_C;                                             // enable peripheral and disable PIO use

    if (PMC_PCSR & PIOC) {                                               // only handle port C if clocks are enabled to it
        PIO_OSR_C |= PIO_OER_C;                                          // simple synchronisation of port direction depending on bits commanded to be set adn cleared
        PIO_OSR_C &= ~PIO_ODR_C;
        PIO_ODSR_C |= PIO_SODR_C;                                        // simple synchronisation of port state dependingf on bits commanded to be set adn cleared
        PIO_ODSR_C &= ~PIO_CODR_C;
        PIO_PDSR_C = (PIO_ODSR_C & PIO_OSR_C & PIO_PSR_C);               // bits defined by outputs
        PIO_PDSR_C |= (portc_in & (~(PIO_OSR_C) | ~(PIO_PSR_C)));        // bits defined by inputs
    }
    PIO_PER_C = PIO_PDR_C = PIO_OER_C = PIO_ODR_C = PIO_SODR_C = PIO_CODR_C = PIO_ASR_C = PIO_BSR_C = 0;
#endif

    PIO_ABSR_A |= PIO_BSR_A;
    PIO_ABSR_A &= ~PIO_ASR_A;

    PIO_PSR_A |= PIO_PER_A;
    PIO_PSR_A &= ~PIO_PDR_A;

    if (PMC_PCSR & PIOA) {                                               // only handle port A if clocks are enabled to it
        PIO_OSR_A |= PIO_OER_A;                                          // simple synchronisation of port direction depending on bits commanded to be set adn cleared
        PIO_OSR_A &= ~PIO_ODR_A;
        PIO_ODSR_A |= PIO_SODR_A;                                        // simple synchronisation of port state depending on bits commanded to be set adn cleared
        PIO_ODSR_A &= ~PIO_CODR_A;
        PIO_PDSR_A = (PIO_ODSR_A & PIO_OSR_A & PIO_PSR_A);               // {25} bits defined by outputs
        PIO_PDSR_A |= (porta_in & (~(PIO_OSR_A) | ~(PIO_PSR_A)));        // {25} bits defined by inputs
    }
    PIO_PER_A = PIO_PDR_A = PIO_OER_A = PIO_ODR_A = PIO_SODR_A = PIO_CODR_A = PIO_ASR_A = PIO_BSR_A = 0; // {43}

#ifdef _HW_SAM7S                                                         // {41}
    if ((ulPerA != PIO_PSR_A)  || (ulPerSelA != PIO_ABSR_A) || (ulADC != ADC_CHSR))
#else
    if ((ulPerA != PIO_PSR_A) || (ulPerSelA != PIO_ABSR_A))              // update peripherals
#endif
    {
        unsigned long ulBits = 0x00000001;
        int i = 0;
        ulPerA = PIO_PSR_A;
        ulPerSelA = PIO_ABSR_A;
        while (i < PORT_WIDTH) {
            ucPortFunctions[0][i] = ((ulPerSelA & ulBits) != 0) + 1;
            ulBits <<= 1;
#ifdef _HW_SAM7S                                                         // {41}
            if ((i >= ADC_FIRST_PORT_BIT) && (i < (ADC_FIRST_PORT_BIT + 4))) {
                if (ADC_CHSR & (0x01 << (i - ADC_FIRST_PORT_BIT))) {     // ADC function set
                    ucPortFunctions[0][i] = 3;
                }
            }
#endif
            i++;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure ports are updated
    }
#ifndef _HW_SAM7S                                                        // {41}
    if ((ulPerB != PIO_PSR_B)  || (ulPerSelB != PIO_ABSR_B) || (ulADC != ADC_CHSR)) { // {21} correct ulPerSelB
        unsigned long ulBits = 0x00000001;
        int i = 0;
        ulPerB = PIO_PSR_B;
        ulPerSelB = PIO_ABSR_B;
        while (i < PORT_WIDTH) {
            ucPortFunctions[1][i] = ((ulPerSelB & ulBits) != 0) + 1;     // {21} correct ulPerSelB
            if (i >= ADC_FIRST_PORT_BIT) {
                if (ADC_CHSR & (0x01 << (i - ADC_FIRST_PORT_BIT))) {     // ADC function set
                    ucPortFunctions[1][i] = 3;
                }
            }
            i++;
            ulBits <<= 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure ports are updated
    }
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    if ((ulPerC != PIO_PSR_C)  || (ulPerSelC != PIO_ABSR_C) || (ulADC != ADC_CHSR)) {
        unsigned long ulBits = 0x00000001;
        int i = 0;
        ulPerC = PIO_PSR_C;
        ulPerSelC = PIO_ABSR_C;
        while (i < PORT_WIDTH) {
            ucPortFunctions[2][i] = ((ulPerSelC & ulBits) != 0) + 1;
            if (i >= ADC_FIRST_PORT_BIT) {
                if (ADC_CHSR & (0x01 << (i - ADC_FIRST_PORT_BIT))) {     // ADC function set
                    ucPortFunctions[2][i] = 3;
                }
            }
            i++;
            ulBits <<= 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure ports are updated
    }
#endif
    if (ulADC != ADC_CHSR) {                                             // {22} change of ADC setting
        int i = (PORT_WIDTH - 4);
        unsigned long ulBits = 0x00000010;
        ulADC = ADC_CHSR;
        while (i < PORT_WIDTH) {
            if ((ADC_CHSR & ulBits) != 0) {
                ucPortFunctions[ADC_PORT_NUMBER][i++] = 3;               // special function
            }
            else {
                ucPortFunctions[ADC_PORT_NUMBER][i++] = 0;               // not ADC use
            }
            ulBits <<= 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure ports are updated
    }
    AIC_IMR |= AIC_IECR;                                                 // update interrupt masks {6}
    AIC_IMR &= ~AIC_IDCR;
    AIC_IECR = AIC_ICCR = AIC_IDCR = 0;
#ifdef SUPPORT_HW_FLOW
    if (US_CR_0 & (SAM7_RTSEN | SAM7_RTSDIS)) {                          // {30} handle RTS changes on UART0 and UART1
        if (US_CR_0 & SAM7_RTSEN) {
            fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                      // assert control line
            porta_in &= ~RTS_0;
        }
        if (US_CR_0 & SAM7_RTSDIS) {
            fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                      // negate control line
            porta_in |= RTS_0;
        }
        US_CR_0 = 0;
    }
    if (US_CR_1 & (SAM7_RTSEN | SAM7_RTSDIS)) {
        if (US_CR_1 & SAM7_RTSEN) {
            fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                      // assert control line
            porta_in &= ~RTS_1;
        }
        if (US_CR_1 & SAM7_RTSDIS) {
            fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                      // negate control line
            porta_in |= RTS_1;
        }
        US_CR_1 = 0;
    }
    #if defined DBGU_UART && defined RTS_2_PIN                           // {35}
    if (PMC_PCSR & RTS_2_PORT) {                                         // port with RTS output is enabled
        static int iRTS2 = 1;
        if (RTS_2_PORT == PIOA) {
            if (PIO_ODSR_A & RTS_2_PORT) {                               // configured as output
                if (PIO_ODSR_A & RTS_2_PORT) {
                    if (iRTS2 == 0) {
                        iRTS2 = 1;
                        fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);          // negate control line
                    }
                }
                else {
                    if (iRTS2 == 1) {
                        iRTS2 = 0;
                        fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);          // assert control line
                    }
                }
            }
        }
        else {
            if (PIO_ODSR_B & RTS_2_PORT) {                               // configured as output
                if (PIO_ODSR_B & RTS_2_PORT) {
                    if (iRTS2 == 0) {
                        iRTS2 = 1;
                        fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);          // negate control line
                    }
                }
                else {
                    if (iRTS2 == 1) {
                        iRTS2 = 0;
                        fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);          // assert control line
                    }
                }
            }
        }
    }
    #endif
#endif
    if (US_CR_0 & SAM7X_RETTO) {                                         // {48}
        usIdleCounter0 = (unsigned short)US_RTOR_0;                      // reset next timeout
    }
    if (US_CR_0 & SAM7X_STTTO) {                                         // reset the interrupt but don't start new timeout until next character received
        US_CSR_0 &= ~RX_TIMEOUT;
    }
    US_CR_1 = 0;
    if (US_CR_1 & SAM7X_RETTO) {
        usIdleCounter1 = (unsigned short)US_RTOR_1;                      // reset next timeout
    }
    if (US_CR_1 & SAM7X_STTTO) {                                         // reset the interrupt but don't start new timeout until next character received
        US_CSR_1 &= ~RX_TIMEOUT;
    }
    US_CR_1 = 0;
}

extern int fnPortChanges(int iForce)                                     // {30}
{
    static unsigned long ulPortA = 0, ulPortB = 0, ulPortC = 0;
    int iRtn = iFlagRefresh;
    iFlagRefresh = 0;

    if (PIO_ODSR_A != ulPortA) {
        ulPortA = PIO_ODSR_A;
        iRtn |= PORT_CHANGE;
    }
#ifndef _HW_SAM7S                                                        // {41}
    if (PIO_ODSR_B != ulPortB) {
        ulPortB = PIO_ODSR_B;
        iRtn |= PORT_CHANGE;
    }
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    if (PIO_ODSR_C != ulPortC) {
        ulPortC = PIO_ODSR_C;
        iRtn |= PORT_CHANGE;
    }
#endif
    if (iForce == 0) {                                                   // on tick
        if ((usIdleCounter0 != 0) && (US_BRGR_0 != 0)) {                 // {31} idle bit period counter active on UART0
            unsigned long ulTickBitCount = (((MASTER_CLOCK/1000)*TICK_RESOLUTION)/(US_BRGR_0*16)); // bit periods in a tick
            if (ulTickBitCount >= usIdleCounter0) {
                usIdleCounter0 = 0;
                US_CSR_0 |= RX_TIMEOUT;                                  // set the timeout bit in the status register
                if (US_IMR_0 & RX_TIMEOUT) {                             // if timeout interrupt enabled
                    AIC_IPR |= USART0;
                    while ((AIC_IMR & USART0) && (US_CSR_0 & RX_TIMEOUT)) {
                        void (*interrupt_call)(void);
                        interrupt_call = (void (*)(void))AIC_SVR6;
                        interrupt_call();
                        if (US_CR_0 & SAM7X_RETTO) {
                            usIdleCounter0 = (unsigned short)US_RTOR_0;  // reset next timeout
                        }
                        if (US_CR_0 & SAM7X_STTTO) {                     // reset the interrupt but don't start new timeout until next character received
                            US_CSR_0 &= ~RX_TIMEOUT;
                        }
                        US_CR_0 = 0;
                    }
                }
            }
            else {
                usIdleCounter0 -= (unsigned short)ulTickBitCount;
            }
        }
        if ((usIdleCounter1 != 0) && (US_BRGR_1 != 0)) {                 // {31} idle bit period counter active on UART1
            unsigned long ulTickBitCount = (((MASTER_CLOCK/1000)*TICK_RESOLUTION)/(US_BRGR_1*16)); // bit periods in a tick
            if (ulTickBitCount >= usIdleCounter1) {
                usIdleCounter1 = 0;
                US_CSR_1 |= RX_TIMEOUT;                                  // set the timeout bit in the status register
                if (US_IMR_1 & RX_TIMEOUT) {                             // if timeout interrupt enabled
                    AIC_IPR |= USART1;
                    while ((AIC_IMR & USART1) && (US_CSR_1 & RX_TIMEOUT)) {
                        void (*interrupt_call)(void);
                        interrupt_call = (void (*)(void))AIC_SVR7;
                        interrupt_call();
                        if (US_CR_1 & SAM7X_RETTO) {
                            usIdleCounter1 = (unsigned short)US_RTOR_1;  // reset next timeout
                        }
                        if (US_CR_1 & SAM7X_STTTO) {                     // reset the interrupt but don't start new timeout until next character received
                            US_CSR_1 &= ~RX_TIMEOUT;
                        }
                        US_CR_1 = 0;
                    }
                }
            }
            else {
                usIdleCounter1 -= (unsigned short)ulTickBitCount;
            }
        }
    }
    return iRtn;
}

extern unsigned long fnGetPresentPortState(int portNr)
{
    unsigned long ulState = 0;

    switch (portNr) {
    case 1:
        ulState = (PIO_PSR_A & PIO_ODSR_A & PIO_OSR_A);  
        ulState |= ~(PIO_PSR_A & PIO_OSR_A) & PIO_PDSR_A;
        break;
#ifndef _HW_SAM7S                                                        // {41}
    case 2:  
        ulState = (PIO_PSR_B & PIO_ODSR_B & PIO_OSR_B);  
        ulState |= ~(PIO_PSR_B & PIO_OSR_B) & PIO_PDSR_B;
        break;
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    case 3: 
        ulState = (PIO_PSR_C & PIO_ODSR_C & PIO_OSR_C);  
        ulState |= ~(PIO_PSR_C & PIO_OSR_C) & PIO_PDSR_C;
        break;
#endif
    }
    return ulState;
}

extern unsigned long fnGetPresentPortDir(int portNr)
{
    switch (portNr) {
    case 1:
        return (PIO_PSR_A & PIO_OSR_A);                                  // non outputs returned as zeros
#ifndef _HW_SAM7S                                                        // {41}
    case 2:
        return (PIO_PSR_B & PIO_OSR_B);                                  // non outputs returned as zeros
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    case 3:
        return (PIO_PSR_C & PIO_OSR_C);                                  // non outputs returned as zeros
#endif
    default:
        return 0;
    }
}

extern unsigned long fnGetPresentPortPeriph(int portNr)
{
    switch (portNr) {
    case 1:
#ifdef _HW_SAM7S                                                         // {41}
        return (~PIO_PSR_A | ((ADC_CHSR & 0x0f) << ADC_FIRST_PORT_BIT));
#else
        return ~PIO_PSR_A;
#endif
    case 2:
#ifndef _HW_SAM7S                                                        // {41}
        return (~PIO_PSR_B | ((ADC_CHSR & 0x0f) << ADC_FIRST_PORT_BIT)); // {22}
    #if !defined _HW_SAM7SE && !defined _HW_SAM7L                        // {44}{49}
    case 3:                                                              // {22}
    #endif
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    case 3:
        return (~PIO_PSR_C | ((ADC_CHSR & 0x0f) << ADC_FIRST_PORT_BIT)); // {22}
    case 4:
#endif
        return ((ADC_CHSR & 0xf0) << (PORT_WIDTH - 8));
    }
    return 0;
}

#ifdef SUPPORT_ADC
static void fnSimADC(int iChannel)                                       // {22}
{
    ADC_SR |= (0x01 << iChannel);                                        // set conversion complete bit for the channel
    *(ADC_CDR0_ADD + iChannel) = (unsigned short)ulADC_values[iChannel]; // set the sample to the channel input register
    ADC_LCDR = (unsigned short)ulADC_values[iChannel];                   // set the sample to the last data register
    ADC_SR |= DRDY;                                                      // set the data ready bit
}

// Capture the present input, if enabled
//
extern void fnUpdateADC(int iChannel)                                    // {32}
{
    if (iChannel < ADC_CHANNELS) {
        if (ADC_CHSR & (0x01 << iChannel)) {                             // if channel is not disabled
            fnSimADC(iChannel);                                          // update present sample
        }
    }
    else {
        iChannel = 0;
        while (iChannel < ADC_CHANNELS) {
            if (ADC_CHSR & (0x01 << iChannel)) {                         // if channel is not disabled
                fnSimADC(iChannel);                                      // update present sample
            }
            iChannel++;
        }
    }
}

static void fnADC_trigger(void)
{
    unsigned long ulBit = 0x01;
    int iChannel = 0;
    ADC_CR = 0;
    while (ulBit <= 0x80) {
        if (ADC_CHSR & ulBit) {                                          // channel active
            fnSimADC(iChannel);                                          // sample each active channel
            if (ADC_PTCR & PDC_RXTEN) {                                  // PDC connected
                if (ADC_RCR != 0) {                                      // transfers remaining
                    if (ADC_MR & LOWRES) {                               // 8 bit mode
                        *(unsigned char *)(ADC_RPR) = (unsigned char)ADC_LCDR;
                        (ADC_RPR)++;
                    }
                    else {                                                // 10 bit mode
                        *(unsigned short *)(ADC_RPR) = (unsigned short)ADC_LCDR;
                        (ADC_RPR)++;
                        (ADC_RPR)++;
                    }
                    (ADC_RCR)--;
                    if (ADC_RCR == 0) {                                  // last transfer
                        ADC_SR |= ADC_ENDRX;
                        if (ADC_IMR & ADC_ENDRX) {
                            void (*int_routine)( void) = (void *)AIC_SVR17;
                            int_routine();                                    
                        }
                    }
                }
            }
        }
        iChannel++;
        ulBit <<= 1;
    }
    while (ADC_IMR & (DRDY | EOC0 | EOC1 | EOC2 | EOC3 | EOC4 | EOC5 | EOC6 | EOC7) & ADC_SR) { // if end of conversion interrupt (or data read interrupt) enabled
        void (*int_routine)( void) = (void *)AIC_SVR17;                  // adc interrupt handler
        int_routine();                                                   // call the handler
    }
}

#endif

extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    int iPortChanged = 0;
    unsigned long ulBit = (0x00000001 << ((PORT_WIDTH - 1) - ucPortBit));

    switch (ucPort) {
    case 0:                                                              // port A
    #if defined _HW_SAM7S
        if ((!(ulBit & PIO_OSR_A & PIO_PSR_A)) || ((ADC_CHSR & (ulBit >> ADC_FIRST_PORT_BIT)) && (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)))) // programmed as an input or ADC
    #else
        if (!(ulBit & PIO_OSR_A & PIO_PSR_A))                            // input bit
    #endif
        {
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
    #if defined _HW_SAM7S && defined SUPPORT_ADC                         // {41}
                if (ADC_CHSR & (ulBit >> ADC_FIRST_PORT_BIT)) {          // input is ADC so change the voltage
                    int iAdc = 0;
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                    ulBit >>= ADC_FIRST_PORT_BIT;
                    do {
                        iAdc++,
                        ulBit >>= 1;
                    } while (ulBit);
                    if (TOGGLE_INPUT_NEG & iChange) {
                        if (ulADC_values[iAdc-1] >= usStepSize) {
                            ulADC_values[iAdc-1] -= usStepSize;
                            fnSimADC(iAdc-1);
                        }
                    }
                    else {
                        if (ADC_MR & LOWRES) {
                            if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_8BIT) {
                                ulADC_values[iAdc-1] += usStepSize;
                                fnSimADC(iAdc-1);
                            }
                        }
                        else {
                            if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_10BIT) {
                                ulADC_values[iAdc-1] += usStepSize;
                                fnSimADC(iAdc-1);
                            }
                        }
                    }
                    break;
                }
    #endif
                porta_in ^= ulBit;
                PIO_PDSR_A ^= ulBit;                                     // toggle the input state
                if (ulBit & PA30) {                                      // handle IRQ0 if enabled {1}
                    if (PIO_PDSR_A & PA30) {                             // rising edge
                        if ((AIC_SMR30 & 0x60) == POSITIVE_EDGE_TRIGGERED) {
                            if (AIC_IMR & AIC_IRQ0) {                              
                                void ( *int_routine )( void) = (void(*)(void))AIC_SVR30;
                                int_routine();
                            }
                        }
                    }
                    else {                                               // falling edge
                        if ((AIC_SMR30 & 0x60) == NEGATIVE_EDGE_TRIGGERED) {
                            if (AIC_IMR & AIC_IRQ0) {
                                void ( *int_routine )( void) = (void(*)(void))AIC_SVR30;
                                int_routine();
                            }
                        }
                    }
                }
                if (ulBit & PA14) {                                      // handle IRQ1 if enabled {26}
                    if (PIO_PDSR_A & PA14) {                             // rising edge
                        if ((AIC_SMR31 & 0x60) == POSITIVE_EDGE_TRIGGERED) {
                            if (AIC_IMR & AIC_IRQ1) {                              
                                void ( *int_routine )( void) = (void(*)(void))AIC_SVR31;
                                int_routine();
                            }
                        }
                    }
                    else {                                               // falling edge
                        if ((AIC_SMR31 & 0x60) == NEGATIVE_EDGE_TRIGGERED) {
                            if (AIC_IMR & AIC_IRQ1) {
                                void ( *int_routine )( void) = (void(*)(void))AIC_SVR31;
                                int_routine();
                            }
                        }
                    }
                }
                if (ulBit & PA29) {                                      // handle FIQ if enabled {9}
                    if (PIO_PDSR_A & PA29) {                             // rising edge
                        if ((AIC_SMR0 & 0x60) == POSITIVE_EDGE_TRIGGERED) {
                            if (AIC_IMR & AIC_FIQ) {                              
                                void ( *int_routine )( void) = (void(*)(void))AIC_SVR0;
                                int_routine();
                            }
                        }
                    }
                    else {                                               // falling edge
                        if ((AIC_SMR0 & 0x60) == NEGATIVE_EDGE_TRIGGERED) {
                            if (AIC_IMR & AIC_FIQ) {
                                void ( *int_routine )( void) = (void(*)(void))AIC_SVR0;
                                int_routine();
                            }
                        }
                    }
                }
    #if defined _HW_SAM7S && defined SUPPORT_ADC                         // {41}
                if ((ulBit & ADTRG_B) && (PIO_ABSR_A & ADTRG_B) && (ADC_MR & TRGEN) && (ADC_MR & TRGSEL_EXT)) { // if ADC trigger configured
                    if (porta_in & ADTRG_B) {                            // rising edge causes trigger
                        fnADC_trigger();
                    }
                }
    #endif
                iPortChanged = 1;
            }
            else if (iChange == SET_INPUT) {
                if (~porta_in & ulBit) {                                 // this represents a low to high change
                    iPortChanged = 1;
                }
                porta_in |= ulBit;
                PIO_PDSR_A |= ulBit;                                     // set the input high
            }
            else  {
                if (porta_in & ulBit) {                                  // this represents a high to low change
                    iPortChanged = 1;
                }
                porta_in &= ~ulBit;
                PIO_PDSR_A &= ~ulBit;                                    // set the input low
            }
            if (iPortChanged != 0) {
                PIO_ISR_A |= ulBit;                                      // change detected
                if (ulBit & (CTS_0 | CTS_1)) {
                    fnModemChange(ulBit, ((porta_in & ulBit) == 0), 0);  // {34} handle potential CTS changes
                }
                if (ulBit & PIO_IMR_A) {                                 // handle port change interrupts which are enabled
                    void ( *int_routine )( void) = (void(*)(void))AIC_SVR2;
                    int_routine();
                    break;
                }
            }
            iFlagRefresh = PORT_CHANGE;                                  // ensure port display is updated {5}
        }
        break;
#ifndef _HW_SAM7S                                                        // {41}
    case 1:                                                              // port B
        if ((!(ulBit & PIO_OSR_B & PIO_PSR_B)) || ((ADC_CHSR & (ulBit >> ADC_FIRST_PORT_BIT)) && (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)))) { // programmed as an input or ADC
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {           // {22}
    #ifdef SUPPORT_ADC                                                   // {22}
                if (ADC_CHSR & (ulBit >> ADC_FIRST_PORT_BIT)) {          // input is ADC so change the voltage
                    int iAdc = 0;
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                    ulBit >>= ADC_FIRST_PORT_BIT;
                    do {
                        iAdc++,
                        ulBit >>= 1;
                    } while (ulBit);
                    if (TOGGLE_INPUT_NEG & iChange) {
                        if (ulADC_values[iAdc-1] >= usStepSize) {
                            ulADC_values[iAdc-1] -= usStepSize;
                            fnSimADC(iAdc-1);
                        }
                    }
                    else {
                        if (ADC_MR & LOWRES) {
                            if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_8BIT) {
                                ulADC_values[iAdc-1] += usStepSize;
                                fnSimADC(iAdc-1);
                            }
                        }
                        else {
                            if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_10BIT) {
                                ulADC_values[iAdc-1] += usStepSize;
                                fnSimADC(iAdc-1);
                            }
                        }
                    }
                    break;
                }
    #endif
                portb_in ^= ulBit;
                PIO_PDSR_B ^= ulBit;                                     // toggle the input state
                iPortChanged = 1;
            }
            else if (iChange == SET_INPUT) {
                if (~portb_in & ulBit) {                                 // this represents a low to high change
                    iPortChanged = 1;
                }
                portb_in |= ulBit;
                PIO_PDSR_B |= ulBit;                                     // set the input high
            }
            else  {
                if (portb_in & ulBit) {                                  // this represents a high to low change
                    iPortChanged = 1;
                }
                portb_in &= ~ulBit;
                PIO_PDSR_B &= ~ulBit;                                    // set the input low
            }
            if (iPortChanged != 0) {
                PIO_ISR_B |= ulBit;                                      // change detected
                if (ulBit & PIO_IMR_B) {                                 // handle port change interrupts which are enabled
                    void ( *int_routine )( void) = (void(*)(void))AIC_SVR3;
                    int_routine();
                    break;
                }
    #ifdef SUPPORT_ADC                                                   // {22}
        #if defined _HW_SAM7SE                                           // {44}
                if ((ulBit & ADTRG) && (!(PIO_ABSR_B & ADTRG)) && (ADC_MR & TRGEN) && (ADC_MR & TRGSEL_EXT))
        #else
                if ((ulBit & ADTRG_B) && (PIO_ABSR_B & ADTRG_B) && (ADC_MR & TRGEN) && (ADC_MR & TRGSEL_EXT))
        #endif
                { // if ADC trigger configured
        #if defined _HW_SAM7SE                                           // {44}
                    if (portb_in & ADTRG) 
        #else
                    if (portb_in & ADTRG_B) 
        #endif
                    {                                                    // rising edge causes trigger
                        fnADC_trigger();
                    }
                }
    #endif
            }
            iFlagRefresh = PORT_CHANGE;                                  // ensure port display is updated {5}
        }
        break;
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {44}{49}
    case 2:                                                              // port C
        if (!(ulBit & PIO_OSR_C & PIO_PSR_C)) {                          // input bit
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
    #if defined  SUPPORT_ADC
                if (ADC_CHSR & (ulBit >> ADC_FIRST_PORT_BIT)) {          // input is ADC so change the voltage
                    int iAdc = 0;
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                    ulBit >>= ADC_FIRST_PORT_BIT;
                    do {
                        iAdc++,
                        ulBit >>= 1;
                    } while (ulBit);
                    if (TOGGLE_INPUT_NEG & iChange) {
                        if (ulADC_values[iAdc-1] >= usStepSize) {
                            ulADC_values[iAdc-1] -= usStepSize;
                            fnSimADC(iAdc-1);
                        }
                    }
                    else {
                        if (ADC_MR & LOWRES) {
                            if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_8BIT) {
                                ulADC_values[iAdc-1] += usStepSize;
                                fnSimADC(iAdc-1);
                            }
                        }
                        else {
                            if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_10BIT) {
                                ulADC_values[iAdc-1] += usStepSize;
                                fnSimADC(iAdc-1);
                            }
                        }
                    }
                    break;
                }
    #endif
                portc_in ^= ulBit;
                PIO_PDSR_C ^= ulBit;                                     // toggle the input state
                iPortChanged = 1;
            }
            else if (iChange == SET_INPUT) {
                if (~portc_in & ulBit) {                                 // this represents a low to high change
                    iPortChanged = 1;
                }
                portc_in |= ulBit;
                PIO_PDSR_C |= ulBit;                                     // set the input high
            }
            else  {
                if (portc_in & ulBit) {                                  // this represents a high to low change
                    iPortChanged = 1;
                }
                portc_in &= ~ulBit;
                PIO_PDSR_C &= ~ulBit;                                    // set the input low
            }
            if (iPortChanged != 0) {
                PIO_ISR_C |= ulBit;                                      // change detected
                if (ulBit & PIO_IMR_C) {                                 // handle port change interrupts which are enabled
                    void ( *int_routine )( void) = (void(*)(void))AIC_SVR2;
                    int_routine();
                    break;
                }
            }
            iFlagRefresh = PORT_CHANGE;                                  // ensure port display is updated {5}
        }
        break;
#endif
#ifdef SUPPORT_ADC                                                       // {22}
    case _GPIO_ADC:                                                      // ADC inputs
        if ((ulBit & (0xf0000000 >> (32 - PORT_WIDTH))) && (ADC_CHSR & (ulBit >> (PORT_WIDTH - 8)))) { // input is ADC so change the voltage
            int iAdc = 0;
            unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
            ulBit >>= (PORT_WIDTH - 8);
            do {
                iAdc++,
                ulBit >>= 1;
            } while (ulBit);
            if (TOGGLE_INPUT_NEG & iChange) {
                if (ulADC_values[iAdc-1] >= usStepSize) {
                    ulADC_values[iAdc-1] -= usStepSize;
                    fnSimADC(iAdc-1);
                }
            }
            else {
                if (ADC_MR & LOWRES) {
                    if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_8BIT) {
                        ulADC_values[iAdc-1] += usStepSize;
                        fnSimADC(iAdc-1);
                    }
                }
                else {
                    if ((ulADC_values[iAdc-1] + usStepSize) <= ADC_FULL_SCALE_10BIT) {
                        ulADC_values[iAdc-1] += usStepSize;
                        fnSimADC(iAdc-1);
                    }
                }
            }
            break;
        }
        break;
#endif
    }
}

#ifdef SUPPORT_KEY_SCAN

#if KEY_COLUMNS == 0                                                     // {37}
    #define _NON_MATRIX
    #undef KEY_COLUMNS
    #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
    #undef KEY_ROWS
    #define KEY_ROWS VIRTUAL_KEY_ROWS
#endif


int iKeyPadInputs[KEY_COLUMNS][KEY_ROWS] = {0};

extern void fnSimulateKeyChange(int *intTable)
{
#if defined _NON_MATRIX                                                  // {37}
    int iCol, iRow;
    int iChange;
    for (iCol = 0; iCol < KEY_COLUMNS; iCol++) {
        for (iRow = 0; iRow < KEY_ROWS; iRow++) {
            iChange = iKeyPadInputs[iCol][iRow];                         // original value
            iKeyPadInputs[iCol][iRow] = *intTable++;                     // new value
            if (iChange != iKeyPadInputs[iCol][iRow]) {
    #if defined KEY_POLARITY_POSITIVE
                if (iChange)
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
#if defined KEY_COLUMNS && !defined _NON_MATRIX                           // {37}
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

#if !defined _NON_MATRIX                                                 // {37}
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
    iFlagRefresh = fnPortChanges(1);                                     // {30} synchronise with present settings                                                                         
    for (i = 0; i < KEY_COLUMNS; i++) {                                  // check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);
            }
        }
    }
}
#endif

#ifdef USB_INTERFACE                                                     // {18}

static unsigned char ucEndpointFifoIn0[8];                               // SAM7X endpoint rx FIFOs
static unsigned char ucEndpointFifoIn1[2][64];
static unsigned char ucEndpointFifoIn2[2][64];
static unsigned char ucEndpointFifoIn3[64];
#ifdef _HW_SAM7SE
    static unsigned char ucEndpointFifoIn4[2][64];
    static unsigned char ucEndpointFifoIn5[2][64];
    static unsigned char ucEndpointFifoIn6[2][64];
    static unsigned char ucEndpointFifoIn7[2][64];
#else
    static unsigned char ucEndpointFifoIn4[2][256];
    static unsigned char ucEndpointFifoIn5[2][256];
#endif

static unsigned char ucEndpointFifoOut0[8 + sizeof(unsigned short)];      // SAM7X endpoint tx FIFOs (when simulating transmission, first 2 bytes are used to hold the valid length)
static unsigned char ucEndpointFifoOut1[2][64 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut2[2][64 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut3[64 + sizeof(unsigned short)];
#ifdef _HW_SAM7SE
    static unsigned char ucEndpointFifoOut4[2][64 + sizeof(unsigned short)];
    static unsigned char ucEndpointFifoOut5[2][64 + sizeof(unsigned short)];
    static unsigned char ucEndpointFifoOut6[2][64 + sizeof(unsigned short)];
    static unsigned char ucEndpointFifoOut7[2][64 + sizeof(unsigned short)];
#else
    static unsigned char ucEndpointFifoOut4[2][256 + sizeof(unsigned short)];
    static unsigned char ucEndpointFifoOut5[2][256 + sizeof(unsigned short)];
#endif

extern volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned char *ptrAdd, int iDirection)
{
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
    #ifdef _HW_SAM7SE
        if (UDP_FDR6_ADD == ptrAdd) {
            return ucEndpointFifoOut6[0];
        }
        if (UDP_FDR7_ADD == ptrAdd) {
            return ucEndpointFifoOut7[0];
        }
    #endif
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
    #ifdef _HW_SAM7SE
        if (UDP_FDR6_ADD == ptrAdd) {
            return ucEndpointFifoIn6[0];
        }
        if (UDP_FDR7_ADD == ptrAdd) {
            return ucEndpointFifoIn7[0];
        }
    #endif
    }
    return ptrAdd;
}

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
    if (ptrDebugIn == 0) {
        switch (usLenEvent) {
        case USB_RESET_CMD:
            memset(iTxDataToggle, 0, sizeof(iTxDataToggle));
            memset(iRxDataBank,   0, sizeof(iRxDataBank));
            memset(ulTxDataBank,  0, sizeof(ulTxDataBank));
            UDP_ISR = ENDBUSRES;                                         // flag that a USB bus reset has been detected
            break;
        case USB_SLEEP_CMD:
            UDP_ISR = RXSUSP;
            break;
        case USB_RESUME_CMD:
            UDP_ISR = RXRSM;
            break;
        case USB_STALL_SUCCESS:
        case USB_IN_SUCCESS:
            break;
        default:
            return 0;
        }
    }
    else {                                                               // data being injected
        unsigned long ulEndpointConfig;
        if ((UDP_FADDR & FADD_MASK) != (unsigned char)iDevice) {         // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        switch (iEndPoint) {
        case 0:                                                          // endpoint 0 - control/bulk/interrupt - max endpoint size 8 bytes - not dual-banked
            if (!(UDP_CSR0 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR0 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR0 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR0 |= RXSETUP;
                ulTxDataBank[0] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[0] ^= DTGLE; 
                UDP_CSR0 |= ulTxDataBank[0];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn0)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn0);
            }
            memcpy(ucEndpointFifoIn0, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP0INT;
            break;
        case 1:                                                          // endpoint 1 - bulk/iso/interrupt - max endpoint size 64 bytes - dual-banked
            if (!(UDP_CSR1 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR1 & (EPTYPE_INTERRUPT_IN | EPEDS));
            if (iRxDataBank[1] == 0) {                                   // simulate dual bank operation
                ulEndpointConfig |= RX_DATA_BK0;
            }
            else {
                ulEndpointConfig |= RX_DATA_BK1;
            }
            iRxDataBank[1] ^= 1;
            UDP_CSR1 = ((usLenEvent << RXBYTECNT_SHIFT) | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR1 |= RXSETUP;
                ulTxDataBank[1] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[1] ^= DTGLE; 
                UDP_CSR1 |= ulTxDataBank[1];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn1)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn1);
            }
            memcpy(ucEndpointFifoIn1, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP1INT;
            break;
        case 2:                                                          // endpoint 2 - bulk/iso/interrupt - max endpoint size 64 bytes - dual-banked
            if (!(UDP_CSR2 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR2 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR2 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR2 |= RXSETUP;
                ulTxDataBank[2] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[2] ^= DTGLE; 
                UDP_CSR2 |= ulTxDataBank[2];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn2)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn2);
            }
            memcpy(ucEndpointFifoIn2, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP2INT;
            break;
        case 3:                                                          // endpoint 3 - control/bulk/interrupt - max endpoint size 64 bytes - not dual-banked
            if (!(UDP_CSR3 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR3 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR3 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR3 |= RXSETUP;
                ulTxDataBank[3] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[3] ^= DTGLE; 
                UDP_CSR3 |= ulTxDataBank[3];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn3)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn3);
            }
            memcpy(ucEndpointFifoIn3, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP3INT;
            break;
        case 4:                                                          // endpoint 4 - bulk/iso/interrupt - max endpoint size 256 (64 SAM7SE) bytes - dual-banked
            if (!(UDP_CSR4 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR4 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR4 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR4 |= RXSETUP;
                ulTxDataBank[4] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[4] ^= DTGLE;
                UDP_CSR4 |= ulTxDataBank[4]; 
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn4)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn4);
            }
            memcpy(ucEndpointFifoIn4, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP4INT;
            break;
        case 5:                                                          // endpoint 5 - bulk/iso/interrupt - max endpoint size 256 (64 SAM7SE) bytes - dual-banked
            if (!(UDP_CSR5 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR5 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR5 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR5 |= RXSETUP;
                ulTxDataBank[5] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[5] ^= DTGLE; 
                UDP_CSR5 |= ulTxDataBank[5];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn5)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn5);
            }
            memcpy(ucEndpointFifoIn5, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP5INT;
            break;
    #ifdef _HW_SAM7SE
        case 6:                                                          // endpoint 6 - bulk/iso/interrupt - max endpoint size 64 bytes - dual-banked
            if (!(UDP_CSR6 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR6 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR6 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR6 |= RXSETUP;
                ulTxDataBank[6] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[6] ^= DTGLE; 
                UDP_CSR5 |= ulTxDataBank[6];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn6)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn6);
            }
            memcpy(ucEndpointFifoIn5, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP6INT;
            break;
        case 7:                                                          // endpoint 7 - bulk/iso/interrupt - max endpoint size 64 bytes - dual-banked
            if (!(UDP_CSR7 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR7 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR7 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR7 |= RXSETUP;
                ulTxDataBank[7] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[7] ^= DTGLE; 
                UDP_CSR7 |= ulTxDataBank[7];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn7)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn7);
            }
            memcpy(ucEndpointFifoIn7, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP7INT;
            break;
    #endif
        }
    }
    if ((UDP_IMR & UDP_ISR) && (PMC_PCSR & UDP) && (UDP_FADDR & FEN) && (AIC_IMR & UDP)) { // if USB enabled (USB reset interrupt is always enabled)
        void (*interrupt_call)(void);
        interrupt_call = (void (*)(void))AIC_SVR11;
        interrupt_call();                                                // call interrupt handler
    }
    return 0;
}


// Handle data sent by USB
//
extern void fnSimUSB(int iType, int iEndpoint, USB_HW *ptrUSB_HW)
{
    extern void fnChangeUSBState(int iNewState);
    switch (iType) {
    case USB_SIM_RESET:
        {
            int x;
            fnChangeUSBState(0);
            for (x = 0; x < NUMBER_OF_USB_ENDPOINTS; x++) {
                fnLogUSB(x, 0, 0, (unsigned char *)0xffffffff, 0);       // log reset on each endpoint
            }
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
        if (((TXPKTRDY | EPEDS) & *ptrUSB_HW->ptr_ulUSB_BDControl) != (TXPKTRDY | EPEDS)) { // if the ownership has not been passed to the USB controller ignore it
            return;
        }
        iInts |= USB_INT;                                                // flag that the interrupt should be handled
        ulEndpointInt |= (1 << iEndpoint);                               // flag endpoint
        break;
    case USB_SIM_ENUMERATED:                                             // flag that we have completed emumeration
        fnChangeUSBState(1);
        break;
    case USB_SIM_STALL:
        fnLogUSB(iEndpoint, 0, 1, (unsigned char *)0xffffffff, 0);       // log stall
        break;
    case USB_SIM_SUSPEND:
        fnChangeUSBState(0);
        break;
    }
}

extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    extern int iGetGetBank(int iEndpoint);

    int iBank = 0;
    switch (iEndpoint) {
    case 0:                                                              // endpoint 0
        if (!(UDP_CSR0 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR0 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut0, sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[0] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(0, 0, usUSBLength, &ucEndpointFifoOut0[2], iTxDataToggle[0]);
            if (UDP_IMR & EP0INT) {
                UDP_ISR |= EP0INT;
            }
            UDP_CSR0 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR0 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR0 & FORCESTALL) {
            UDP_CSR0 |= STALLSENT;                                       // stall acknowledged
            UDP_ISR = EP0INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    case 1:                                                              // endpoint 1
        if (!(UDP_CSR1 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR1 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(1);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut1[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[1] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(1, 0, usUSBLength, &ucEndpointFifoOut1[iBank][2], iTxDataToggle[1]);
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
            fnLogUSB(2, 0, usUSBLength, &ucEndpointFifoOut2[iBank][2], iTxDataToggle[2]);
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
            fnLogUSB(3, 0, usUSBLength, &ucEndpointFifoOut3[2], iTxDataToggle[3]);
            if (UDP_IMR & EP3INT) {
                UDP_ISR |= EP3INT;
            }
            UDP_CSR3 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR3 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR3 & FORCESTALL) {
            UDP_CSR3 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP3INT;
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
            fnLogUSB(4, 0, usUSBLength, &ucEndpointFifoOut4[iBank][2], iTxDataToggle[4]);
            if (UDP_IMR & EP4INT) {
                UDP_ISR |= EP4INT;
            }
            UDP_CSR4 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR4 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR4 & FORCESTALL) {
            UDP_CSR4 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP4INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    case 5:                                                              // endpoint 5
        if (!(UDP_CSR5 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR5 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(5);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut5[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[5] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(5, 0, usUSBLength, &ucEndpointFifoOut5[iBank][2], iTxDataToggle[5]);
            if (UDP_IMR & EP5INT) {
                UDP_ISR |= EP5INT;
            }
            UDP_CSR5 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR5 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR5 & FORCESTALL) {
            UDP_CSR5 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP5INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    #ifdef _HW_SAM7SE
    case 6:                                                              // endpoint 6
        if (!(UDP_CSR6 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR6 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(6);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut6[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[6] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(6, 0, usUSBLength, &ucEndpointFifoOut6[iBank][2], iTxDataToggle[6]);
            if (UDP_IMR & EP6INT) {
                UDP_ISR |= EP6INT;
            }
            UDP_CSR6 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR6 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR6 & FORCESTALL) {
            UDP_CSR6 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP6INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    case 7:                                                              // endpoint 7
        if (!(UDP_CSR7 & EPEDS)) {
            return;                                                      // endpoint not enabled so ignore
        }
        while (UDP_CSR7 & TXPKTRDY) {                                    // transmission bit set
            unsigned short usUSBLength;
            iBank = iGetGetBank(7);
            memcpy((unsigned char *)&usUSBLength, ucEndpointFifoOut7[iBank], sizeof(unsigned short)); // extract the length of data content
            iTxDataToggle[7] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(7, 0, usUSBLength, &ucEndpointFifoOut7[iBank][2], iTxDataToggle[7]);
            if (UDP_IMR & EP7INT) {
                UDP_ISR |= EP7INT;
            }
            UDP_CSR7 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR7 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR7 & FORCESTALL) {
            UDP_CSR7 |= STALLSENT;                                       // stall acnowledged
            UDP_ISR = EP7INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
    #endif
    }
}
#endif

#ifndef SUPPORT_PORT_INTERRUPTS
    extern void PortB_Interrupt(void);
#endif

// The PHY interrupt line is pulled low to simulate change of PHY state - the IRQ is assumed to be on port B
//
extern void fnSimulateLinkUp(void)
{
#if defined ETH_INTERFACE && defined PHY_INTERRUPT                       // {42}
    #ifdef SUPPORT_PORT_INTERRUPTS
    unsigned char ucBit = (PORT_WIDTH - 1);
    unsigned long ulBit = PHY_INTERRUPT;
    while (ulBit > 1) {
        ulBit >>= 1;
        ucBit--;
    }
    fnSimulateInputChange(1, ucBit, CLEAR_INPUT);
    #else
    PIO_PDSR_B &= ~PHY_INTERRUPT;
    PIO_ISR_B = PHY_INTERRUPT;
    PortB_Interrupt();                                                   // simulate link up 
    #endif
#endif
#if defined ETH_INTERFACE
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}

#ifdef SUPPORT_RTC                                                       // {46}
#define NTP_TO_1970_TIME 2208988800u
#define LEAP_YEAR(year) ((year % 4)==0)                                  // valid from 1970 to 2038
static const unsigned char monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
// Synchronise the internal RTC to the PC time when simulating - the SAM7X doesn't have a battery backup and the RTC doesn't run when the processor is powered down!!
//
extern void fnInitInternalRTC(char *argv[])
{
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned short RTC_YEAR, RTC_MONTH, RTC_DOW, RTC_DOM, RTC_HOUR, RTC_MIN, RTC_SEC;
    unsigned long ulRTCTime;
    unsigned long ulLeapYears = 1970;

    RTC_YEAR = *ptrShort++;
    RTC_MONTH = *ptrShort++;
    RTC_DOW = *ptrShort++;
    RTC_DOM = *ptrShort++;
    RTC_HOUR = *ptrShort++;
    RTC_MIN = *ptrShort++;
    RTC_SEC = *ptrShort++;

    ulRTCTime = ((RTC_YEAR - 1970) * 365);                               // years since reference time, represented in days without respecting leap years
    while (ulLeapYears < RTC_YEAR) {
        if (LEAP_YEAR(ulLeapYears)) {                                    // count leap years
            if (ulLeapYears == RTC_YEAR) {                               // presently in a leap year
                if ((RTC_MONTH > 2) || (RTC_DOM > 28)) {                 // past February 28 so count extra leap day in this year
                    ulRTCTime++;
                }
            }
            else {
                ulRTCTime++;                                             // count extra days in passed leap years
            }
        }
        ulLeapYears++;
    }
    while (--RTC_MONTH != 0) {
        ulRTCTime += monthDays[RTC_MONTH];                               // add past days of previous months of this year
    }
    ulRTCTime += (RTC_DOM - 1);                                          // add past number of days in present month
    ulRTCTime *= 24;                                                     // convert days to hours
    ulRTCTime += RTC_HOUR;                                               // add hours passed in present day
    ulRTCTime *= 60;                                                     // convert hours to minutes
    ulRTCTime += RTC_MIN;                                                // add minutes in present hour
    ulRTCTime *= 60;                                                     // convert minutes to seconds
    ulRTCTime += RTC_SEC;                                                // add seconds in present minute

    RTT_VR = ulRTCTime;                                                  // set the initial seconds count value (since 1970)
}
#endif

// we can simulate timers during a tick here if required
//
extern int fnSimTimers(void)
{
    unsigned long ulRTT_count = (RTT_MR & 0x0000ffff);                   // {45}
    static unsigned long ulRTT_prescaler = 0;                            // {47}
    if (ulRTT_count == 0) {                                              // fastest rate
        ulRTT_count = ((32 * TICK_RESOLUTION) / 0x10000);                // RTT count increment in a TICK period
    }
    else {
        ulRTT_prescaler += ((32768 * TICK_RESOLUTION)/1000);
        if (ulRTT_prescaler >= ulRTT_count) {                            // RTT increment
            ulRTT_prescaler -= ulRTT_count;
            ulRTT_count = 1;
        }
        else {
            ulRTT_count = 0;
        }
    }
    RTT_VR += ulRTT_count;
    if (ulRTT_count != 0) {                                              // {47}
        if ((RTT_MR & RTT_MR_RTTINCIEN) && (AIC_IMR  & SYSIRQ)) {        // if increment interrupt is enabled
            void (*interrupt_call)(void);
            AIC_IPR |= SYSIRQ;
            RTT_SR |= RTT_SR_RTTINC;
            interrupt_call = (void (*)(void))AIC_SVR1;
            interrupt_call();                                            // call interrupt handler
        }
    }

#ifdef SIMULATE_FPGA
    fnSimFPGAPeriod();
#endif

    if ((RSTC_CR & (0xff000000 | PROCRST)) == (PROCRST | RESET_KEY)) {   // valid processor reset command
        return RESET_SIM_CARD;                                           // inform that we have been reset
    }
    if (!(WDT_MR & WDDIS)) {                                             // if watchdog activated
        static unsigned short usWatchdogCount = 0;
        if (WDT_CR == (WATCHDOG_KEY | WDRSTT)) {                         // valid watchdog retrigger has been performed
            WDT_CR = 0;                                                  // mark read
            if (usWatchdogCount) {
                usWatchdogCount -= (TICK_RESOLUTION*SLOW_CLOCK/128/1000);// the countdown rate
                if (usWatchdogCount > ((WDT_MR >> 16) & 0xfff)) {        // illegal retrigger - too fast
                    if (WDRSTEN & WDT_MR) {
                        return RESET_CARD_WATCHDOG;                      // signal watchdog reset
                    }
                }
            }
            usWatchdogCount = (unsigned short)(WDT_MR & 0xfff);          // reload watchdog counter value
        }
        else {
            if (usWatchdogCount) {
                if (usWatchdogCount <= (TICK_RESOLUTION*SLOW_CLOCK/128/1000)) {
                    if (WDRSTEN & WDT_MR) {
                        return RESET_CARD_WATCHDOG;                      // signal watchdog reset
                    }
                    usWatchdogCount = (unsigned short)(WDT_MR & 0xfff);  // reload watchdog counter value
                }
                else {
                    usWatchdogCount -= TICK_RESOLUTION*SLOW_CLOCK/128/1000; // the countdown rate
                }
            }
        }
    }

    if (PIT_MR & PITEN) {                                                // if PIT enabled {8}
        unsigned long ulCounter = (PIT_PIVR & 0x000fffff);
        ulCounter += (unsigned long)((((float)MASTER_CLOCK/(float)16)*(float)TICK_RESOLUTION)/(float)1000); // {28} count during tick interval
        if (ulCounter >= ((PIT_MR & 0x000fffff) - 2)) {                  // {28} small tolerance accepted
            PIT_PIVR = (((PIT_PIVR >> 20) + 1) << 20);                   // increment overflow counter and reset PIT counter
            PIT_SR = PITS;
            if ((PIT_MR & PITIEN) && (AIC_IMR  & SYSIRQ)) {              // if PIT interrupt is enabled
                void (*interrupt_call)(void);
                AIC_IPR |= SYSIRQ;                                       // {20}
                interrupt_call = (void (*)(void))AIC_SVR1;
                interrupt_call();                                        // call interrupt handler
            }
        }
        else {
            PIT_PIVR += ulCounter;                                       // {28}
            PIT_PIVR &= 0x000fffff;
        }
    }

    if ((PMC_PCSR & TC0) && (TC_SR_0 & TIM_CLKSTA)) {                    // {4}{23} if timer 0 is powered and running
        unsigned long ulTickCount;
        switch (TC_CMR_0 & 0x07) {
        case 0:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(2*1000));       // clock pulses in tick period
            break;
        case 1:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(8*1000));       // clock pulses in tick period
            break;
        case 2:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(32*1000));      // clock pulses in tick period
            break;
        case 3:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(128*1000));     // clock pulses in tick period
            break;
        case 4:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(1024*1000));    // clock pulses in tick period
            break;
        default:                                                         // external clock
            ulTickCount = 0;
            break;
        }
                                                                         // we support only RC compare
                                                                         // we support only RC compare
        if ((TC_CV_0 + ulTickCount) >= TC_RC_0) {                        // TC match?
            if (TC_CMR_0 & CPCSTOP) {
                TC_SR_0 &= ~TIM_CLKSTA;                                  // stop timer
            }
            if ((TC_CMR_0 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) {         // {24} toggle TIOA0 on match
    #if defined _HW_SAM7S || defined _HW_SAM7L                           // {41}{49}
                ulTIOA ^= TIOA0_B;
    #else
                ulTIOA ^= TIOA0;
    #endif
            }
            if (TC_IER_0 & TIM_CPCS) {                                   // if interrupt enabled on RC match
                if (ADC_MR & TRGSEL_T0) {                                // {22} if the timer is set up to control ADC triggering
                    if ((TC_CMR_0 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) { // toggle TIOA0 on match
    #if defined _HW_SAM7S || defined _HW_SAM7L                           // {41}{49}
                        if (ulTIOA & TIOA0_B) 
    #else
                        if (ulTIOA & TIOA0) 
    #endif
                        {                                                // positive edge on TIOA0
    #ifdef SUPPORT_ADC
                            fnADC_trigger();
    #endif
                        }
                    }
                    TC_CV_0 = 0;                                         // reset count
                }
                else {
                    void (*int_routine)( void) = (void *)AIC_SVR12;
                    TC_CV_0 = 0;                                         // reset count
                    int_routine();
                }
            }
        }
        else {
            TC_CV_0 += ulTickCount;
        }
    }

    if ((PMC_PCSR & TC1) && (TC_SR_1 & TIM_CLKSTA)) {                    // {23} if timer 1 is powered and running {4}
        unsigned long ulTickCount;
        switch (TC_CMR_1 & 0x07) {
        case 0:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(2*1000));       // clock pulses in tick period
            break;
        case 1:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(8*1000));       // clock pulses in tick period
            break;
        case 2:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(32*1000));      // clock pulses in tick period
            break;
        case 3:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(128*1000));     // clock pulses in tick period
            break;
        case 4:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(1024*1000));    // clock pulses in tick period
            break;
        default:                                                         // external clock
            ulTickCount = 0;
            break;
        }
                                                                         // we support only RC compare
        if ((TC_CV_1 + ulTickCount) >= TC_RC_1) {                        // TC match?
            if (TC_CMR_1 & CPCSTOP) {
                TC_SR_1 &= ~TIM_CLKSTA;                                  // stop timer
            }
            if ((TC_CMR_1 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) {         // {24} toggle TIOA1 on match
    #if defined _HW_SAM7S || defined _HW_SAM7L                           // {41}{49}
                ulTIOA ^= TIOA1_B;
    #else
                ulTIOA ^= TIOA1;
    #endif
            }
            if (TC_IER_1 & TIM_CPCS) {                                   // if interrupt enabled on RC match
                if (ADC_MR & TRGSEL_T1) {                                // {22} if the timer is set up to control ADC triggering
                    if ((TC_CMR_1 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) { // toggle TIOA1 on match
    #if defined _HW_SAM7S || defined _HW_SAM7L                           // {41}{49}
                        if (ulTIOA & TIOA1_B) 
    #else
                        if (ulTIOA & TIOA1) 
    #endif
                        {                                                // positive edge on TIOA1
    #ifdef SUPPORT_ADC
                            fnADC_trigger();
    #endif
                        }
                    }
                    TC_CV_1 = 0;                                         // reset count
                }
                else {
                    void (*int_routine)( void) = (void *)AIC_SVR13;
                    TC_CV_1 = 0;                                         // reset count
                    int_routine();
                }
            }
        }
        else {
            TC_CV_1 += ulTickCount;
        }
    }

    if ((PMC_PCSR & TC2) && (TC_SR_2 & TIM_CLKSTA)) {                    // {23} if timer 2 is powered and running 
        unsigned long ulTickCount;
        switch (TC_CMR_2 & 0x07) {
        case 0:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(2*1000));       // clock pulses in tick period
            break;
        case 1:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(8*1000));       // clock pulses in tick period
            break;
        case 2:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(32*1000));      // clock pulses in tick period
            break;
        case 3:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(128*1000));     // clock pulses in tick period
            break;
        case 4:
            ulTickCount = TICK_RESOLUTION*(MASTER_CLOCK/(1024*1000));    // clock pulses in tick period
            break;
        default:                                                         // external clock
            ulTickCount = 0;
            break;
        }
                                                                         // we support only RC compare
        if ((TC_CV_2 + ulTickCount) >= TC_RC_2) {                        // TC match?
            if (TC_CMR_2 & CPCSTOP) {
                TC_SR_2 &= ~TIM_CLKSTA;                                  // stop timer
            }
            if ((TC_CMR_2 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) {         // {24} toggle TIOA2 on match
    #if defined _HW_SAM7S || defined _HW_SAM7L                           // {41}{49}
                ulTIOA ^= TIOA2_B;
    #else
                ulTIOA ^= TIOA2;
    #endif
            }
            if (TC_IER_2 & TIM_CPCS) {                                   // if interrupt enabled on RC match
                if (ADC_MR & TRGSEL_T2) {                                // {22} if the timer is set up to control ADC triggering
                    if ((TC_CMR_2 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) { // toggle TIOA2 on match
    #if defined _HW_SAM7S || defined _HW_SAM7L                           // {41}{49}
                        if (ulTIOA & TIOA2_B)
    #else
                        if (ulTIOA & TIOA2)
    #endif
                        {                                                // positive edge on TIOA2
    #ifdef SUPPORT_ADC
                            fnADC_trigger();
    #endif
                        }
                    }
                    TC_CV_2 = 0;                                         // reset count
                }
                else {
                    void (*int_routine)( void) = (void *)AIC_SVR14;
                    TC_CV_2 = 0;                                         // reset count
                    int_routine();
                }
            }
        }
        else {
            TC_CV_2 += ulTickCount;
        }
    }
    #ifdef SUPPORT_ADC
    if (ADC_CR & START) {                                                // software ADC conversion start
        ADC_CR = 0;
        fnADC_trigger();
    }
    #endif
    return 0;
}

// Prepare a string to be displayed in the simulator status bar          // {50}
//
extern void fnUpdateOperatingDetails(void)
{
#if !defined NO_STATUS_BAR
    extern void fnPostOperatingDetails(char *ptrDetails);
    CHAR cOperatingDetails[256];
    CHAR *ptrBuffer = cOperatingDetails;
    ptrBuffer = uStrcpy(ptrBuffer, "FLASH = ");
    ptrBuffer = fnBufferDec((SIZE_OF_FLASH/1024), 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    ptrBuffer = fnBufferDec((SIZE_OF_RAM/1024), 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, "k");
    fnPostOperatingDetails(cOperatingDetails);
#endif
}
#endif





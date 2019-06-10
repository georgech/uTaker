/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      AVR32Sim.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    18.11.2009 Add _AT32UC3B support                                     {1}
    21.11.2009 Modification in the way that oscillator pin mapping is controlled - and extend to support all clock sources {2}
    26.01.2010 Add USBB                                                  {3}
    19.10.2010 Correct Timer 2 clock source                              {4}
    04.05.2011 Correct ADC block                                         {5}
    19.06.2011 Add _AT32UC3C support                                     {6}
    17.09.2011 Add SET_INPUT and CLEAR_INPUT port simulation support     {7}
    17.09.2011 Add external interrupt controller simulation support for _AT32UC3C {8}
    08.01.2012 Set power on reset cause                                  {9}
    19.07.2012 Corrected key scan support and enable non-matrix keypad   {10}
    19.07.2012 Add RTC                                                   {11}
    30.11.2012 Remove ODMER_x use when UC3C enabled                      {12}
    07.12.2012 Disable PHY interrupt when PHY_INTERRUPT is not defined   {13}
    05.01.2013 Correctly recognise UC3C watchdog retrigger               {14}

*/  


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
#include "config.h"

#if defined _HW_AVR32


/**************************************************************************************************/
/********************************* ATMEL AT32UC3A *************************************************/
/**************************************************************************************************/

static unsigned long port0_in;
static unsigned long port1_in;
static unsigned long port2_in;
static unsigned long port3_in;
static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = { 0 };
static unsigned long ulOscillator = 0;
#ifdef SUPPORT_ADC
    static unsigned long ulADC_values[ADC_CHANNELS];
    static void *ptrPorts[3] = {(void *)ucPortFunctions, (void *)ulADC_values, (void *)(ADC_BLOCK + 0x4)}; // {5}
#endif

static unsigned long ulTIOA = 0;

static int iFlagRefresh = 0;
static unsigned short usIdleCounter0;
static unsigned short usIdleCounter1;
static unsigned short usIdleCounter2;
static unsigned short usIdleCounter3;
#if NUMBER_SERIAL > 4
    static unsigned short usIdleCounter4;
#endif

#ifdef USB_INTERFACE
    static unsigned long ulEndpointInt = 0;    
    static int iRxDataBank[NUMBER_OF_USB_ENDPOINTS] = {0};
    static unsigned long ulTxDataBank[NUMBER_OF_USB_ENDPOINTS] = {0};
    static int iTxDataToggle[NUMBER_OF_USB_ENDPOINTS] = {0};
#endif

#define ASSERT_RTS_0    0x01
#define NEGATE_RTS_0    0x02
#define ASSERT_RTS_1    0x04
#define NEGATE_RTS_1    0x08
#define ASSERT_RTS_2    0x10
#define NEGATE_RTS_2    0x20
#define ASSERT_RTS_3    0x40
#define NEGATE_RTS_3    0x80


unsigned char ucFLASH[(SIZE_OF_FLASH*2) + USER_PAGE_SIZE];               // the flash size contains user page and space between
                                                                         // the space between the flash data array and user page is however not to be used


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
    unsigned char *ucSimulatedFlash = ucAdd + (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    if ((ucSimulatedFlash < &ucFLASH[0]) || (ucSimulatedFlash >= &ucFLASH[SIZE_OF_FLASH])) { // check flash bounds
        _EXCEPTION("Attempted access outside of internal Flash bounds!!!");
    }
    return (ucSimulatedFlash);                                           // location in simulated internal FLASH memory
}

extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    return ucAdd - sim_add;
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





// Process simulated Peripheral DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    static int iRTS_action = 0;
    unsigned long ulNewActions = 0;
#if defined SERIAL_INTERFACE && !defined _AT32UC3C
    int *ptrCnt;

    if (iDMA & DMA_CONTROLLER_0) {                                       // USART 0 Tx DMA
        if (DMA_CR_0 & DMA_TEN) {                                        // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[THROUGHPUT_UART0];
            while (DMA_TCR_0 != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_0;                          // no futher transmissions in this interval
                    break;
                }
                if ((US_MR_0 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= NEGATE_RTS_0;
                }
                US_THR_0 = *(unsigned char *)DMA_MAR_0;
                DMA_MAR_0 = DMA_MAR_0 + 1;
                fnLogTx0((unsigned char)US_THR_0);
                ulNewActions |= SEND_COM_0;
                (DMA_TCR_0)--;
                --(*ptrCnt);                                             // limit transmission in this TICK interval
            }
            if ((iMasks & DMA_CONTROLLER_0) == 0) {
                iDMA &= ~DMA_CONTROLLER_0;                               // transfer complete
                US_CSR_0 |= (ENDTX);                                     // simulate Tx int
                if ((US_MR_0 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= ASSERT_RTS_0;
                }
                if (DMA_IMR_0 & DMA_TRC) {                               // if interrupt enabled
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART0 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART0 = IR_PDCA0;
                        interrupt_call();                                // call UART 0 int handler
                    }
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_0;
        }
    }
    /*
    if (iDMA & DMA_CONTROLLER_1) {                                       // USART 1 Tx DMA
        if (US_PTSR_1 & PDC_TXTEN) {                                     // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[THROUGHPUT_UART1];
            while (US_TCR_1 != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_2;                          // no futher transmissions in this interval
                    break;
                }
                if ((US_MR_1 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
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
                if ((US_MR_1 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= ASSERT_RTS_1;
                    fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);              // negate control lines (RS485 uses negative logic)
//                  porta_in &= ~RTS_1;
                }
                if (US_IMR_1 & ENDTX) {                                  // if interrupt enabled
                    AIC_IPR |= USART1;
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART1 = IR_USART1;
                        interrupt_call();                                // call UART 1 int handler
                    }
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_2;
        }
    }
    if (iDMA & DMA_CONTROLLER_2) {                                       // USART 2 Tx DMA
        if (US_PTSR_2 & PDC_TXTEN) {                                     // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[THROUGHPUT_UART2];
            while (US_TCR_2 != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_4;                          // no futher transmissions in this interval
                    break;
                }
                if ((US_MR_2 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= NEGATE_RTS_2;                
                }
                US_THR_2 = *(unsigned char *)US_TPR_2;
                US_TPR_2 = US_TPR_2 + 1;
                fnLogTx2((unsigned char)US_THR_2);
                ulNewActions |= SEND_COM_2;
                (US_TCR_2)--;
                --(*ptrCnt);                                             // limit transmission in this TICK interval
            }
            if ((iMasks & DMA_CONTROLLER_4) == 0) {
                iDMA &= ~DMA_CONTROLLER_4;                               // transfer complete
                US_CSR_2 |= (ENDTX);                                     // simulate Tx int
                if ((US_MR_2 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= ASSERT_RTS_2;
                    fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);              // negate control lines (RS485 uses negative logic)
// TO DO            porta_in &= ~RTS_2;
                }
                if (US_IMR_2 & ENDTX) {                                  // if interrupt enabled
                    AIC_IPR |= USART2;
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART2 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART2 = IR_USART2;
                        interrupt_call();                                // call UART 2 int handler
                    }
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_4;
        }
    }
    if (iDMA & DMA_CONTROLLER_3) {                                       // USART 3 Tx DMA
        if (US_PTSR_3 & PDC_TXTEN) {                                     // peripheral DMA transfer enabled
            ptrCnt = (int *)argv[THROUGHPUT_UART3];
            while (US_TCR_3 != 0) {
                if (*ptrCnt == 0) {
                    iMasks |= DMA_CONTROLLER_6;                          // no futher transmissions in this interval
                    break;
                }
                if ((US_MR_3 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= NEGATE_RTS_3;
                }
                US_THR_3 = *(unsigned char *)US_TPR_3;
                US_TPR_3 = US_TPR_3 + 1;
                fnLogTx3((unsigned char)US_THR_3);
                ulNewActions |= SEND_COM_3;
                (US_TCR_3)--;
                --(*ptrCnt);                                             // limit transmission in this TICK interval
            }
            if ((iMasks & DMA_CONTROLLER_6) == 0) {
                iDMA &= ~DMA_CONTROLLER_6;                               // transfer complete
                US_CSR_3 |= (ENDTX);                                     // simulate Tx int
                if ((US_MR_3 & 0x7) == USART_MODE_RS485) {               // if in RS485 mode automatically control the RTS line
                    iRTS_action |= ASSERT_RTS_3;
                    fnConfigSimSCI(3, ASSERT_RTS_COM_3, 0);              // negate control lines (RS485 uses negative logic)
// TO DO            porta_in &= ~RTS_3;
                }
                if (US_IMR_3 & ENDTX) {                                  // if interrupt enabled
                    AIC_IPR |= USART3;
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART3 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART3 = IR_USART3;
                        interrupt_call();                                // call UART 3 int handler
                    }
                }
            }
        }
        else {
            iMasks |= DMA_CONTROLLER_6;
        }
    }*/
    if (NEGATE_RTS_0 & iRTS_action) {
        iRTS_action &= ~NEGATE_RTS_0;
        fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                          // negate control lines (RS485 uses negative logic)
    #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
        port3_in |= PX03;
    #else
        port0_in |= PA03;
    #endif
    }
    else if (ASSERT_RTS_0 & iRTS_action) {
        iRTS_action &= ~ASSERT_RTS_0;
        fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                          // negate control lines (RS485 uses negative logic)
    #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
        port3_in &= ~PX03;
    #else
        port0_in &= ~PA03;
    #endif
    }
    if (NEGATE_RTS_1 & iRTS_action) {
        iRTS_action &= ~NEGATE_RTS_1;
        fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                          // negate control lines (RS485 uses negative logic)
    #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
        port2_in |= PX07;
    #else
        port0_in |= PA08;
    #endif
    }
    else if (ASSERT_RTS_1 & iRTS_action) {
        iRTS_action &= ~ASSERT_RTS_1;
        fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                          // negate control lines (RS485 uses negative logic)
    #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
        port2_in &= ~PX07;
    #else
        port0_in &= ~PA08;
    #endif
    }
    if (NEGATE_RTS_2 & iRTS_action) {
        iRTS_action &= ~NEGATE_RTS_2;
        fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);                          // negate control lines (RS485 uses negative logic)
    #ifndef _AT32UC3C
        #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
        port3_in |= PX13;
        #elif defined USART2_RTS_ON_1
        port1_in |= PB01;
        #else
        port0_in |= PA29;
        #endif
    #endif
    }
    else if (ASSERT_RTS_2 & iRTS_action) {
        iRTS_action &= ~ASSERT_RTS_2;
        fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);                          // negate control lines (RS485 uses negative logic)
    #ifndef _AT32UC3C
        #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
        port3_in &= ~PX13;
        #elif defined USART2_RTS_ON_1
        port1_in &= ~PB01;
        #else
        port0_in &= ~PA29;
        #endif
    #endif
    }
    if (NEGATE_RTS_3 & iRTS_action) {
        iRTS_action &= ~NEGATE_RTS_3;
        fnConfigSimSCI(3, NEGATE_RTS_COM_3, 0);                          // negate control lines (RS485 uses negative logic)
    #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
        port2_in |= PX15;
    #else
        port1_in |= PB00;
    #endif
    }
    else if (ASSERT_RTS_3 & iRTS_action) {
        iRTS_action &= ~ASSERT_RTS_3;
        fnConfigSimSCI(3, ASSERT_RTS_COM_3, 0);                          // negate control lines (RS485 uses negative logic)
    #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
        port2_in &= ~PX15;
    #else
        port1_in &= ~PB00;
    #endif
    }
#endif
    return ulNewActions;
}


#ifdef SPI_SIM_INTERFACE
// Simulate the reception of SPI slave data by inserting bytes into the input buffer and calling interrupts
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
    /*
    if ((!(PIO_PSR_A & ulInput)) && (!(PIO_ABSR_A & ulInput))) {         // if input programmed for CTS operation
        unsigned char ucBit = 30;
        unsigned long ulPin = ulInput;
        while (!(ulPin & 0x00000001)) {
            ulPin >>= 1;
            ucBit--;
        }
        if (iState != 0) {                                               // CTS has just been activated
            if (iSetPort != 0) {
                fnSimulateInputChange(0, ucBit, CLEAR_INPUT);
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
                fnSimulateInputChange(0, ucBit, SET_INPUT);
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
    }*/
}


extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState)
{
#if defined SUPPORT_HW_FLOW
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect
    #if defined SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);
    switch (iPort) {
    case 0:                                                              // USART 0
        if (ulChange & MS_CTS_ON) {
        #if defined USART0_CTS_ON_3 && defined CHIP_144_PIN
            fnModemChange(PX02, ((ulNewState & MS_CTS_ON) != 0), 3);
        #else
            fnModemChange(PA04, ((ulNewState & MS_CTS_ON) != 0), 0);
        #endif
        }
        break;
    case 1:                                                              // USART 1
        if (ulChange & MS_CTS_ON) {
        #if defined USART1_CTS_ON_2 && defined CHIP_144_PIN
            fnModemChange(PX07, ((ulNewState & MS_CTS_ON) != 0), 2);
        #else
            fnModemChange(PA08, ((ulNewState & MS_CTS_ON) != 0), 0);
        #endif
        }
        break;
    case 2:                                                              // USART 2
        if (ulChange & MS_CTS_ON) {
        #if defined USART2_CTS_ON_3 && defined CHIP_144_PIN
            fnModemChange(PX12, ((ulNewState & MS_CTS_ON) != 0), 3);
        #elif defined USART2_CTS_ON_1
            fnModemChange(PB01, ((ulNewState & MS_CTS_ON) != 0), 1);
        #else
            fnModemChange(PA30, ((ulNewState & MS_CTS_ON) != 0), 0);
        #endif
        }
        break;
    case 4:                                                              // USART 3
        if (ulChange & MS_CTS_ON) {
        #if defined USART3_CTS_ON_2 && defined CHIP_144_PIN
            fnModemChange(PX16, ((ulNewState & MS_CTS_ON) != 0), 2);
        #else
            fnModemChange(PB01, ((ulNewState & MS_CTS_ON) != 0), 1);
        #endif
        }
        break;
    }
    #endif
#endif
}

// UART Break detection simulation
//
extern void fnSimulateBreak(int iPort)
{
}

// Dummy
//
extern void RealTimeInterrupt(void)
{
}


// Simulate buffers for Ethernet use
//
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
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce)
{
#ifdef ETH_INTERFACE                                                     // we feed frames in promiscuous mode and filter them according to their details
    int iType = 0;
    unsigned long ulTemp;
    static int iFirstRec = 0;
    static AVR32_RX_BD *ptrStartRxBd;
    AVR32_RX_BD *ptrRxBd;
    unsigned short usFrameLength = usLen;
    int iFirstBuffer = 1;


    if ((!iForce) && (!(iType = fnCheckEthernetMode(ucData, usLen)))) {  // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
        return 0;         
    }

    if (!iFirstRec) {                                                    // we play at being the EMAC receiver
        ptrStartRxBd = (AVR32_RX_BD *)MACB_RBQP;                         // get the first buffer descriptor address and remember it
        iFirstRec = 1;                                                   // don't let it happen again
    }
    ptrRxBd = (AVR32_RX_BD *)MACB_RBQP;


    ptrRxBd->bd_details = BD_INFO_START_OF_FRAME;

    while (usLen) {
        ulTemp = (unsigned long)ptrRxBd->ptrBlock;

        if (ulTemp & BD_OWNERSHIP) return 0;                             // check ownership and ignore if the buffer is blocked

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
        if (!iFirstBuffer) {
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
            if (usFrameLength > 1536) {
                usFrameLength = 0;
            }
            ptrRxBd->bd_details &= ~BD_INFO_LENGTH_OF_FRAME_MASK;        // clear any old info (shoudln't really be there..)
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

    MACB_RBQP = (unsigned long)ptrRxBd;                                  // update the present buffer pointer
    MACB_ISR |= RCOMP;                                                   // set interrupt flag
    MACB_RSR |= RSR_REC;                                                 // set receive status flag
    if (!(ulAVR32_CPU.SR & GM)) {                                        // global interrupt not masked
        void (*interrupt_call)(void);
        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_MACB & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
        IRR_MACB = IR_MACB;
        interrupt_call();                                                // call MACB 0 int handler
    }
    return 1;
#else
    return 0;
#endif
}

// The PHY interrupt line is pulled low to simulate change of PHY state
//
extern void fnSimulateLinkUp(void)
{
#if defined ETH_INTERFACE && defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS // {13}
    unsigned char ucBit = 31;
    unsigned long ulBit = PHY_INTERRUPT;
    while (ulBit > 1) {
        ulBit >>= 1;
        ucBit--;
    }
    fnSimulateInputChange(PORT_PHY_INT, ucBit, CLEAR_INPUT);   
#endif
    fnUpdateIPConfig();                                                  // update display in simulator
}


// We can update port state displays if we want
//
extern void fnSimPorts(int iThisPort)
{
    fnSimPortsAVR(0);
#if !defined _AT32UC3B || !defined CHIP_48_PIN
    fnSimPortsAVR(1);
#endif
#ifndef _AT32UC3B
    fnSimPortsAVR(2);
    fnSimPortsAVR(3);
#endif

#if defined SUPPORT_HW_FLOW
    if (US_CR_0 & (AVR32_RTSEN | AVR32_RTSDIS)) {                        // handle RTS changes on USARTs
        if (US_CR_0 & AVR32_RTSEN) {
            fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                      // assert control line
    #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
            port3_in &= ~PX03;
    #else
            port0_in &= ~PA03;
    #endif
        }
        if (US_CR_0 & AVR32_RTSDIS) {
            fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                      // negate control line
    #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
            port3_in |= PX03;
    #else
            port0_in |= PA03;
    #endif
        }
        US_CR_0 = 0;
    }
    if (US_CR_1 & (AVR32_RTSEN | AVR32_RTSDIS)) {
        if (US_CR_1 & AVR32_RTSEN) {
            fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                      // assert control line
    #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
            port2_in &= ~PX07;
    #else
            port0_in &= ~PA08;
    #endif
        }
        if (US_CR_1 & AVR32_RTSDIS) {
            fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                      // negate control line
    #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
            port2_in |= PX07;
    #else
            port0_in |= PA08;
    #endif
        }
        US_CR_1 = 0;
    }
    if (US_CR_2 & (AVR32_RTSEN | AVR32_RTSDIS)) {
        if (US_CR_2 & AVR32_RTSEN) {
            fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);                      // assert control line
    #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
            port3_in &= ~PX13;
    #elif defined USART2_RTS_ON_1
            port1_in &= ~PB01;
    #else
            port0_in &= ~PA29;
    #endif
        }
        if (US_CR_2 & AVR32_RTSDIS) {
            fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);                      // negate control line
    #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
            port3_in |= PX13;
    #elif defined USART2_RTS_ON_1
            port1_in |= PB01;
    #else
            port0_in |= PA29;
    #endif
        }
        US_CR_2 = 0;
    }
    if (US_CR_3 & (AVR32_RTSEN | AVR32_RTSDIS)) {
        if (US_CR_3 & AVR32_RTSEN) {
            fnConfigSimSCI(3, ASSERT_RTS_COM_3, 0);                      // assert control line
    #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
            port2_in &= ~PX15;
    #else
            port1_in &= ~PB00;
    #endif
        }
        if (US_CR_3 & AVR32_RTSDIS) {
            fnConfigSimSCI(3, NEGATE_RTS_COM_3, 0);                      // negate control line
    #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
            port2_in |= PX15;
    #else
            port1_in |= PB00;
    #endif
        }
        US_CR_3 = 0;
    }
#endif
}

extern int fnPortChanges(int iForce)
{
    static unsigned long ulPortA = 0, ulPortB = 0;
    int iRtn = iFlagRefresh;
    iFlagRefresh = 0;
    if (iForce == 0) {                                                   // on tick
        if ((usIdleCounter0 != 0) && (US_BRGR_0 != 0)) {                 // idle bit period counter active on UART0
            unsigned long ulTickBitCount = (((MASTER_CLOCK/1000)*(TICK_RESOLUTION /1000))/(US_BRGR_0*16)); // bit periods in a tick
            if (ulTickBitCount >= usIdleCounter0) {
                usIdleCounter0 = 0;
                US_CSR_0 |= RX_TIMEOUT;                                  // set the timeout bit in the status register
                if (US_IMR_0 & RX_TIMEOUT) {                             // if timeout interrupt enabled
                    while (US_CSR_0 & RX_TIMEOUT) {
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART0 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART0 = IR_USART0;
                        interrupt_call();
                        if (US_CR_0 & AVR32_RETTO) {
                            usIdleCounter0 = (unsigned short)US_RTOR_0;  // reset next timeout
                        }
                        if (US_CR_0 & AVR32_STTTO) {                     // reset the interrupt but don't start new timeout until next character received
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
        if ((usIdleCounter1 != 0) && (US_BRGR_1 != 0)) {                 // idle bit period counter active on UART1
            unsigned long ulTickBitCount = (((MASTER_CLOCK/1000)*(TICK_RESOLUTION /1000))/(US_BRGR_1*16)); // bit periods in a tick
            if (ulTickBitCount >= usIdleCounter1) {
                usIdleCounter1 = 0;
                US_CSR_1 |= RX_TIMEOUT;                                  // set the timeout bit in the status register
                if (US_IMR_1 & RX_TIMEOUT) {                             // if timeout interrupt enabled
                    while (US_CSR_1 & RX_TIMEOUT) {
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART1 = IR_USART1;
                        interrupt_call();
                        if (US_CR_1 & AVR32_RETTO) {
                            usIdleCounter1 = (unsigned short)US_RTOR_1;  // reset next timeout
                        }
                        if (US_CR_1 & AVR32_STTTO) {                     // reset the interrupt but don't start new timeout until next character received
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
        if ((usIdleCounter2 != 0) && (US_BRGR_2 != 0)) {                 // idle bit period counter active on UART1
            unsigned long ulTickBitCount = (((MASTER_CLOCK/1000)*(TICK_RESOLUTION / 1000))/(US_BRGR_2*16)); // bit periods in a tick
            if (ulTickBitCount >= usIdleCounter2) {
                usIdleCounter2 = 0;
                US_CSR_2 |= RX_TIMEOUT;                                  // set the timeout bit in the status register
                if (US_IMR_2 & RX_TIMEOUT) {                             // if timeout interrupt enabled
                    while (US_CSR_2 & RX_TIMEOUT) {
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART2 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART2 = IR_USART2;
                        interrupt_call();
                        if (US_CR_2 & AVR32_RETTO) {
                            usIdleCounter2 = (unsigned short)US_RTOR_2;  // reset next timeout
                        }
                        if (US_CR_2 & AVR32_STTTO) {                     // reset the interrupt but don't start new timeout until next character received
                            US_CSR_2 &= ~RX_TIMEOUT;
                        }
                        US_CR_2 = 0;
                    }
                }
            }
            else {
                usIdleCounter2 -= (unsigned short)ulTickBitCount;
            }
        }
        if ((usIdleCounter3 != 0) && (US_BRGR_3 != 0)) {                 // idle bit period counter active on UART1
            unsigned long ulTickBitCount = (((MASTER_CLOCK/1000)*(TICK_RESOLUTION / 1000))/(US_BRGR_3*16)); // bit periods in a tick
            if (ulTickBitCount >= usIdleCounter3) {
                usIdleCounter3 = 0;
                US_CSR_3 |= RX_TIMEOUT;                                  // set the timeout bit in the status register
                if (US_IMR_3 & RX_TIMEOUT) {                             // if timeout interrupt enabled
                    while (US_CSR_3 & RX_TIMEOUT) {
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART3 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR_UART3 = IR_USART3;
                        interrupt_call();
                        if (US_CR_3 & AVR32_RETTO) {
                            usIdleCounter3 = (unsigned short)US_RTOR_3;  // reset next timeout
                        }
                        if (US_CR_3 & AVR32_STTTO) {                     // reset the interrupt but don't start new timeout until next character received
                            US_CSR_3 &= ~RX_TIMEOUT;
                        }
                        US_CR_3 = 0;
                    }
                }
            }
            else {
                usIdleCounter3 -= (unsigned short)ulTickBitCount;
            }
        }
    }
    return iRtn;
}



#ifdef SUPPORT_ADC
static void fnSimADC(int iChannel)
{
    ADC_SR |= (0x01 << iChannel);                                        // set conversion complete bit for the channel
    *(ADC_CDR0_ADD + iChannel) = (unsigned short)ulADC_values[iChannel]; // set the sample to the channel input register
    ADC_LCDR = (unsigned short)ulADC_values[iChannel];                   // set the sample to the last data register
    ADC_SR |= DRDY;                                                      // set the data ready bit
}

// Capture the present input, if enabled
//
extern void fnUpdateADC(int iChannel)
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
            /*
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
            }*/
        }
        iChannel++;
        ulBit <<= 1;
    }
    while (ADC_IMR & (DRDY | EOC0 | EOC1 | EOC2 | EOC3 | EOC4 | EOC5 | EOC6 | EOC7) & ADC_SR) { // if end of conversion interrupt (or data read interrupt) enabled
        if (!(ulAVR32_CPU.SR & GM)) {                                    // global interrupt not masked
            void (*interrupt_call)(void);
            interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR15 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
            IRR15 = IR_ADC;
            interrupt_call();                                            // call ADC int handler
        }
    }
}

#endif



#ifdef SUPPORT_KEY_SCAN

#if KEY_COLUMNS == 0                                                     // {10}
    #define _NON_MATRIX
    #undef KEY_COLUMNS
    #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
    #undef KEY_ROWS
    #define KEY_ROWS VIRTUAL_KEY_ROWS
#endif

static unsigned char fnMapPortBit(unsigned long ulRealBit)
{
    unsigned long ulBit = 0x80000000;                                    // {10}
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
#if defined _NON_MATRIX                                                  // {10}
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

#if !defined _NON_MATRIX                                                 // {10}
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

#ifdef USB_INTERFACE

static unsigned char ucDPRAM[SIZE_OF_DPRAM];                             // dual-port RAM shared by USB endpoints
static unsigned char *ptrDPRAM_area[NUMBER_OF_USB_ENDPOINTS] = {0};
static unsigned short usSizeEndpoint[NUMBER_OF_USB_ENDPOINTS] = {0};
static int iAllocated[NUMBER_OF_USB_ENDPOINTS] = {0};

/*
static unsigned char ucEndpointFifoIn0[8];                               // AVR32 endpoint rx FIFOs - no DMA on endpoint 0
static unsigned char ucEndpointFifoIn1[2][64];
static unsigned char ucEndpointFifoIn2[2][64];
static unsigned char ucEndpointFifoIn3[2][64];
static unsigned char ucEndpointFifoIn4[2][64];
static unsigned char ucEndpointFifoIn5[2][256];
static unsigned char ucEndpointFifoIn6[2][256];

static unsigned char ucEndpointFifoOut0[8 + sizeof(unsigned short)];      // AVR32 endpoint tx FIFOs (when simulating transmission, first 2 bytes are used to hold the valid length)
static unsigned char ucEndpointFifoOut1[2][64 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut2[2][64 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut3[2][64 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut4[2][64 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut5[2][256 + sizeof(unsigned short)];
static unsigned char ucEndpointFifoOut6[2][256 + sizeof(unsigned short)];
*/

extern volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned char *ptrAdd, int iDirection)
{  
    if (iDirection != 0) {                                               // transmission buffer
        if (USBB_HSB_0 == ptrAdd) {
            return ucDPRAM;                                              // control endpoint 0 always uses the first area of the DPRAM
        }
        /*
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
        if (UDP_FDR6_ADD == ptrAdd) {
            return ucEndpointFifoOut6[0];
        }*/
    }
    else {
        if (USBB_HSB_0 == ptrAdd) {
            return ucDPRAM;                                              // control endpoint 0 always uses the first area of the DPRA
        }/*
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
        if (UDP_FDR6_ADD == ptrAdd) {
            return ucEndpointFifoIn6[0];
        }*/
    }
    return ptrAdd;
}

static void fnCheckAllocation(int iEndpoint)
{
    if (iAllocated[iEndpoint] == 0) {                                    // if endpoint allocation has not been registered do it now
        unsigned short usSize = 0;
        unsigned char *ptrDRAM = ucDPRAM;
        switch (iEndpoint) {
        case 0:                                                          // control endpoint - not reset
            switch (UECFG0 & UECFG_EPSIZE_1024) {
            case UECFG_EPSIZE_8:
                usSize = 8;
                break;
            case UECFG_EPSIZE_16:
                usSize = 16;
                break;
            case UECFG_EPSIZE_32:
                usSize = 32;
                break;
            case UECFG_EPSIZE_64:
                usSize = 64;
                break;
            default:
                *(unsigned char*)(0) = 0;                                // exception on invalid setting for this endpoint
                break;
            }
            break;
        }
        usSizeEndpoint[iEndpoint] = usSize;
        ptrDPRAM_area[iEndpoint] = ptrDRAM;
        iAllocated[iEndpoint] = 1;
    }
}

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
    if (ptrDebugIn == 0) {
        switch (usLenEvent) {
        case USB_RESET_CMD:
            memset(iTxDataToggle, 0, sizeof(iTxDataToggle));
            memset(iRxDataBank, 0, sizeof(iRxDataBank));
            memset(ulTxDataBank, 0, sizeof(ulTxDataBank));
            UDINT |= UDINT_EORST;                                        // flag that a USB bus reset has been detected
            UDCON &= ~(UDCON_ADDMASK | UDCON_ADDEN);                     // clear address
            break;
        case USB_SLEEP_CMD:
            UDINT |= UDINT_SUSP;                                         // flag that a USB bus suspend has been detected
            break;
        case USB_RESUME_CMD:
            UDINT |= UDINT_EORSM;                                        // flag that a USB bus resume has been detected
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
        if ((UDCON & UDCON_ADDMASK) != (unsigned char)iDevice) {         // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        fnCheckAllocation(iEndPoint);
        switch (iEndPoint) {
        case 0:                                                          // endpoint 0 - control/bulk/interrupt - max endpoint size 8 bytes - not dual-banked
            if ((UERST & (EPEN0 | EPRST0)) != EPEN0) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UECFG0 & (UECFG_EPTYPE_INTERRUPT | UECFG_EPDIR_IN));
            UPSTA0 = ((usLenEvent << RXBYTECNT_SHIFT) | RXINE | TXOUTE);
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UESTA0 = (RXSTPI | TXINI);                               // mark that a setup token has been received and that IN frames can be sent
              //UDP_CSR0 |= RXSETUP; ??
              //ulTxDataBank[0] = DTGLE; ??                              // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                UESTA0 = (RXOUTI | TXINI | (((unsigned long)usLenEvent << BYCT_SHIFT) & BYCT_MASK)); // OUT frame with length
              //ulTxDataBank[0] ^= DTGLE; ??
              //UDP_CSR0 |= ulTxDataBank[0]; ??
            }
            if (usLenEvent > usSizeEndpoint[0]) {                        // limit data length to the FIFO length
                usLenEvent = usSizeEndpoint[0];
            }
            memcpy(ptrDPRAM_area[0], ptrDebugIn, usLenEvent);
            UDINT |= UDINT_EP0;                                          // set endpoint 0 interrupt
            break;
            /*
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
        case 4:                                                          // endpoint 4 - bulk/iso/interrupt - max endpoint size 256 bytes - dual-banked
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
        case 5:                                                          // endpoint 5 - bulk/iso/interrupt - max endpoint size 256 bytes - dual-banked
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
        }
        case 6:                                                          // endpoint 6 - bulk/iso/interrupt - max endpoint size 256 bytes - dual-banked
            if (!(UDP_CSR6 & EPEDS)) {
                return 1;                                                // endpoint not enabled so ignore
            }
            ulEndpointConfig = (UDP_CSR5 & (EPTYPE_INTERRUPT_IN | EPEDS));
            UDP_CSR5 = ((usLenEvent << RXBYTECNT_SHIFT) | RX_DATA_BK0 | ulEndpointConfig); // data length
            if (SETUP_PID == ucPID) {                                    // if a setup packet set the RXSETUP bit
                UDP_CSR6 |= RXSETUP;
                ulTxDataBank[6] = DTGLE;                                 // always a DATA1 after a SETUP pid
            }
            else if (OUT_PID == ucPID) {
                ulTxDataBank[6] ^= DTGLE; 
                UDP_CSR6 |= ulTxDataBank[5];
            }
            if (usLenEvent > sizeof(ucEndpointFifoIn6)) {                // limit data length to the FIFO length
                usLenEvent = sizeof(ucEndpointFifoIn6);
            }
            memcpy(ucEndpointFifoIn6, ptrDebugIn, usLenEvent); 
            UDP_ISR = EP6INT;
            break;
        */
        }
    }
    if (UDINTE & UDINT) {                                                // USBB interrupt enabled
        if (!(ulAVR32_CPU.SR & GM)) {                                    // global interrupt not masked
            void (*interrupt_call)(void);
            interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR17 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
            IRR17 = IR_USBB;
            interrupt_call();                                            // call USBB int handler
        }
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
                fnLogUSB(x, 0, (unsigned char *)0xffffffff, 0);          // log reset on each endpoint
                if ( x != 0) {
                    ptrDPRAM_area[x] = 0;                                // clear the DPRAM allocations to all endpoints apart from control endpoint 0
                    usSizeEndpoint[x] = 0;
                    iAllocated[x] = 0;
                }
            }
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
//      if (((TXPKTRDY | EPEDS) & *ptrUSB_HW->ptr_ulUSB_BDControl) != (TXPKTRDY | EPEDS)) { // if the ownership has not been passed to the USB controller ignore it
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

extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    /*
    extern int iGetGetBank(int iEndpoint);

    int iBank = 0;
    */
    switch (iEndpoint) {
    case 0:                                                              // endpoint 0
      //if (!(UDP_CSR0 & EPEDS)) {
      //    return;                                                      // endpoint not enabled so ignore
      //}
        while (!(UESTA0 & TXINE)) {                                      // transmission bit clear
            unsigned short usUSBLength = (unsigned short)((UESTA0 & BYCT_MASK) >> BYCT_SHIFT); // the transmission length
            iTxDataToggle[0] ^= 1;                                       // automatic data toggle on transmission since it is automatically controlled by the chip
            fnLogUSB(0, usUSBLength, ptrDPRAM_area[0], iTxDataToggle[0]);
            UESTA0 |= TXINE;                                             // transmission complete and acknowledged
            if ((UDINTE & UDINT_EP0) && (UECON0 & TXINE)) {              // if endpoint interrupt and interrupt on tx completion enabled
                UDINT |= UDINT_EP0;                                      // set endpoint interrupt (acknowledged)
                fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
            }
        }
        if (UECON0SET & STALLRQ) {                                       // if a stall has been requested
            UECON0 |= STALLRQ;
            UECON0SET &= ~STALLRQ;
          //UDP_CSR0 |= STALLSENT;                                       // stall acknowledged
          //UDP_ISR = EP0INT;
          //fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
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
            fnLogUSB(5, usUSBLength, &ucEndpointFifoOut5[iBank][2], iTxDataToggle[5]);
            if (UDP_IMR & EP5INT) {
                UDP_ISR |= EP5INT;
            }
            UDP_CSR5 &= ~TXPKTRDY;                                       // transmission complete
            UDP_CSR5 |= TXCOMP;                                          // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
        }
        if (UDP_CSR5 & FORCESTALL) {
            UDP_CSR5 |= STALLSENT;                                       // stall acknowledged
            UDP_ISR = EP0INT;
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_STALL_SUCCESS);  // generate stall interrupt
        }
        break;
        */
    }
}
#endif

#ifndef SUPPORT_PORT_INTERRUPTS
    extern void _PortB_Interrupt(void);
#endif









// A copy of the AVR32 peripheral registers for simulation purposes
//
extern AVR32_PERIPH  ucAVR32 = {0};
extern AVR32_SYSTEM_REGISTERS ulAVR32_CPU = {0};

#ifdef _AT32UC3C
    extern unsigned long AVR32_ExceptionRAM[65 + 54] = {0};
#else
    extern unsigned long AVR32_ExceptionRAM[65] = {0};
#endif



// Initialisation fo all non-zero registers in the device
//
static void fnSetDevice(unsigned long *ulPortInits)
{
    extern void fnEnterHW_table(void *hw_table);

    ulAVR32_CPU.SR = (GM | EM | MODE1);

    PM_RCAUSE = PM_RCAUSE_POR;                                           // {9}

    PVR_0 = port0_in = *ulPortInits++;
    PVR_1 = port1_in = *ulPortInits++;
#ifndef _AT32UC3B                                                        // {1} UC3B has only ports A and B
    PVR_2 = port2_in = *ulPortInits++;
    PVR_3 = port3_in = *ulPortInits++;
#endif
#ifdef _AT32UC3C                                                         // {6}
    PM_CPUMASK = 0x00000003;
    PM_HSBMASK = 0x00003fff;
    PM_PBAMASK = 0x07ffffff;
    PM_PBBMASK = 0x0000007f;
    PM_PBCMASK = 0x000003ff;
#else
    PM_CPUMASK = 0x00000003;
    PM_HSBMASK = (HSBMASK_FLASHC | HSBMASK_PBA_BRIDGE | HSBMASK_PBB_BRIDGE | HSBMASK_USBB | HSBMASK_MACB | HSBMASK_PDCA | HSBMASK_EBI); // powered up by default
    PM_PBAMASK = (PBAMASK_INTC | PBAMASK_GPIO | PBAMASK_PDCA | PBAMASK_RM_RTC_EIC | PBAMASK_ADC | PBAMASK_SPI0 | \
                  PBAMASK_SPI1 | PBAMASK_TWI | PBAMASK_USART0 | PBAMASK_USART1 | PBAMASK_USART2 | PBAMASK_USART3 | \
                  PBAMASK_PWM | PBAMASK_SSC | PBAMASK_TC | PBAMASK_ABDAC); // powered up by default
    PM_PBBMASK = (PM_PBBMASK | PBBMASK_HMATRIX | PBBMASK_USBB | PBBMASK_FLASHC | PBBMASK_MACB | PBBMASK_SMC | PBBMASK_SDRAMC); // powered up by default
#endif

    GPER_0     = 0xffffffff;                                             // all GPIO enabled by default
    GPER_1     = 0xffffffff;
    GPER_2     = 0xffffffff;
    GPER_3     = 0xffffffff;

    WDT_CLR    = 0x55aa;                                                 // mark that the watchdog hasn't been retriggered

    MACB_NCFGR = MCLK_32;                                                // MACB
    MACB_IMR   = 0x00003fff;
    MACB_TPQ   = 0x0000ffff;

    HSB_MCFG0  = 0x00000002;                                             // HSB matrix
    HSB_MCFG1  = 0x00000002;
    HSB_MCFG2  = 0x00000002;
    HSB_MCFG3  = 0x00000002;
    HSB_MCFG4  = 0x00000002;
    HSB_MCFG5  = 0x00000002;
    HSB_MCFG6  = 0x00000002;
    HSB_MCFG7  = 0x00000002;
    HSB_MCFG8  = 0x00000002;
    HSB_MCFG9  = 0x00000002;
    HSB_MCFG10 = 0x00000002;
    HSB_MCFG11 = 0x00000002;
    HSB_MCFG12 = 0x00000002;
    HSB_MCFG13 = 0x00000002;
    HSB_MCFG14 = 0x00000002;
    HSB_MCFG15 = 0x00000002;
    HSB_SCFG0  = 0x00000010;
    HSB_SCFG1  = 0x00000010;
    HSB_SCFG2  = 0x00000010;
    HSB_SCFG3  = 0x00000010;
    HSB_SCFG4  = 0x00000010;
    HSB_SCFG5  = 0x00000010;
    HSB_SCFG6  = 0x00000010;
    HSB_SCFG7  = 0x00000010;
    HSB_SCFG8  = 0x00000010;
    HSB_SCFG9  = 0x00000010;
    HSB_SCFG10 = 0x00000010;
    HSB_SCFG11 = 0x00000010;
    HSB_SCFG12 = 0x00000010;
    HSB_SCFG13 = 0x00000010;
    HSB_SCFG14 = 0x00000010;
    HSB_SCFG15 = 0x00000010;

    TWI_SR = 0x0008;

    US_CSR_0 = CTS_HIGH;                                                 // default input is high
    US_CSR_1 = CTS_HIGH;
    US_CSR_2 = CTS_HIGH;
    US_CSR_3 = CTS_HIGH;

    ADC_SR    = (ADC_ENDRX | ADC_RXBUFF);

    SPI_SR_0  = 0x000000f0;
    SPI_SR_1  = 0x000000f0;

    USBCON = (USBCON_FRZCLK | USBCON_UIDE | USBCON_UIMOD);               // {3} USBB
    UDCON  = UDCON_DETACH;
    USBSTA = 0x00000400;
    UVERS = 0x00000310;
    UFEATURES = 0x00012467;
    UADDRSIZE = 0x00001000;
    UNAME1 = 0x48555342;                                                 // "HUSB"
    UNAME2 = 0x004f5447;                                                 // "\0OTG"
    USBFSM = 0x00000009;

    
    SDRAMC_CR = 0x852372c0;                                              // SDRAM controller

#ifdef SUPPORT_ADC
    memcpy(ulADC_values, ulPortInits, sizeof(ulADC_values));             // prime initial ADC values
    fnEnterHW_table(ptrPorts);                                           
#else
    fnEnterHW_table(ucPortFunctions);
#endif
}


extern void fnInitialiseDevice(void *port_inits)
{
    memset(ucFLASH, 0xff, sizeof(ucFLASH));                              // we start with deleted FLASH memory contents
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned long *)port_inits);                            // set device registers to startup condition (if not zerod)
}

#ifdef _AT32UC3C                                                         // {6}
#define PERIPHERAL_A_FUNCTION 1
#define PERIPHERAL_B_FUNCTION 2
#define PERIPHERAL_C_FUNCTION 3
#define PERIPHERAL_D_FUNCTION 4
#define PERIPHERAL_E_FUNCTION 5
#define PERIPHERAL_F_FUNCTION 6
#define PERIPHERAL_G_FUNCTION 7
#define PERIPHERAL_H_FUNCTION 8

static void fnUpdataPeripheralFunctions(int iPort, unsigned long ulMux0, unsigned long ulMux1, unsigned long ulMux2, unsigned long ulGPIO)
{
    unsigned long ulBit = 0x00000001;
    int i = 0;
    while (i < PORT_WIDTH) {
        if (ulGPIO & ulBit) {
            ucPortFunctions[iPort][i] = 0;                               // GPIO
        }
        else {
            if (ulMux0 & ulBit) {
                ucPortFunctions[iPort][i] = PERIPHERAL_B_FUNCTION;       // B
            }
            else {
                ucPortFunctions[iPort][i] = PERIPHERAL_A_FUNCTION;       // A
            }
            if (ulMux1 & ulBit) {
                ucPortFunctions[iPort][i] += PERIPHERAL_B_FUNCTION;      // C or D
            }
            if (ulMux2 & ulBit) {
                ucPortFunctions[iPort][i] += PERIPHERAL_D_FUNCTION;      // E to H
                if (ucPortFunctions[iPort][i] == PERIPHERAL_H_FUNCTION) {// there is no peripheral H function on thsi device so it is signalling the special oscillator mode
                    ucPortFunctions[iPort][i]++;                         // move to the oscillator mode display
                }
            }
        }
        i++;
        ulBit <<= 1;
    }
}
#else
static void fnUpdataPeripheralFunctions(int iPort, unsigned long ulMux0, unsigned long ulMux1, unsigned long ulGPIO)
{
    unsigned long ulBit = 0x00000001;
    int i = 0;
    while (i < PORT_WIDTH) {
        if (ulGPIO & ulBit) {
            ucPortFunctions[iPort][i] = 0;                               // GPIO
        }
        else {
            if (ulMux0 & ulBit) {
                ucPortFunctions[iPort][i] = 2;                           // B
            }
            else {
                ucPortFunctions[iPort][i] = 1;                           // A
            }
            if (ulMux1 & ulBit) {
                ucPortFunctions[iPort][i] += 2;                          // C or D
            }
        }
        i++;
        ulBit <<= 1;
    }
}
#endif

extern void fnSimPortsAVR(int iPort)
{ 
    static unsigned long ulADC = 0x00000000;
    static unsigned long LastPeripheral0[4] = {0};
    static unsigned long LastPeripheral1[4] = {0};
#ifdef _AT32UC3C                                                         // {6}
    static unsigned long LastPeripheral2[4] = {0};
#endif
    static unsigned long LastGPER[4] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
    int iPeripheralChange = 0;
    switch (iPort) {
    case 0:
        GPER_0 ^= GPERT_0;                                               // toggle enable bits
        GPER_0 |= GPERS_0;                                               // set enable bits
        GPER_0 &= ~GPERC_0;                                              // clear enable bits
        GPERT_0 = GPERS_0 = GPERC_0 = 0;                                 // reset write only registers

        PMR0_0 ^= PMR0T_0;                                               // toggle mux0 bits
        PMR0_0 |= PMR0S_0;                                               // set mux0 bits
        PMR0_0 &= ~PMR0C_0;                                              // clear mux0 bits
        PMR0T_0 = PMR0S_0 = PMR0C_0 = 0;                                 // reset write only registers

        PMR1_0 ^= PMR1T_0;                                               // toggle mux1 bits
        PMR1_0 |= PMR1S_0;                                               // set mux1 bits
        PMR1_0 &= ~PMR1C_0;                                              // clear mux1 bits
        PMR1T_0 = PMR1S_0 = PMR1C_0 = 0;                                 // reset write only registers

    #if defined _AT32UC3C                                                // {6}
        PMR2_0 ^= PMR2T_0;                                               // toggle mux1 bits
        PMR2_0 |= PMR2S_0;                                               // set mux1 bits
        PMR2_0 &= ~PMR2C_0;                                              // clear mux1 bits
        PMR2T_0 = PMR2S_0 = PMR2C_0 = 0;                                 // reset write only registers
    #endif

        ODER_0 ^= ODERT_0;                                               // toggle driver bits
        ODER_0 |= ODERS_0;                                               // set driver bits
        ODER_0 &= ~ODERC_0;                                              // clear driver bits
        ODERT_0 = ODERS_0 = ODERC_0 = 0;                                 // reset write only registers

        PUER_0 ^= PUERT_0;                                               // toggle pull-up bits
        PUER_0 |= PUERS_0;                                               // set pull-up bits
        PUER_0 &= ~PUERC_0;                                              // clear pull-up bits
        PUERT_0 = PUERS_0 = PUERC_0 = 0;                                 // reset write only registers
    #if !defined _AT32UC3C                                               // {12}
        ODMER_0 ^= ODMERT_0;                                             // toggle open drain bits
        ODMER_0 |= ODMERS_0;                                             // set open drain bits
        ODMER_0 &= ~ODMERC_0;                                            // clear open drain bits
        ODMERT_0 = ODMERS_0 = ODMERC_0 = 0;                              // reset write only registers
    #endif
        GFER_0 ^= GFERT_0;                                               // toggle glitch filter bits
        GFER_0 |= GFERS_0;                                               // set glitch filter bits
        GFER_0 &= ~GFERC_0;                                              // clear glitch filter bits
        GFERT_0 = GFERS_0 = GFERC_0 = 0;                                 // reset write only registers

        OVR_0 ^= OVRT_0;                                                 // toggle port bits
        OVR_0 |= OVRS_0;                                                 // set port bits
        OVR_0 &= ~OVRC_0;                                                // clear port bits
        OVRT_0 = OVRS_0 = OVRC_0 = 0;                                    // reset write only registers

        PVR_0 = (ODER_0 & OVR_0);                                        // output driven values
        PVR_0 |= (~ODER_0 & port0_in);                                   // complete pin state after adding inputs
        if (LastPeripheral0[0] != PMR0_0) {
            LastPeripheral0[0] = PMR0_0;
            iPeripheralChange = 1;
        }
        if (LastPeripheral1[0] != PMR1_0) {
            LastPeripheral1[0] = PMR1_0;
            iPeripheralChange = 1;
        }
    #ifdef _AT32UC3C                                                     // {6}
        if (LastPeripheral2[0] != PMR2_0) {
            LastPeripheral2[0] = PMR2_0;
            iPeripheralChange = 1;
        }
    #endif
        if (LastGPER[0] != GPER_0) {
            LastGPER[0] = GPER_0;
            iPeripheralChange = 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure port display is updated
        if ((iPeripheralChange != 0) || (ulADC != ADC_CHSR)) {
            int i = 21;
            ulADC = ADC_CHSR;
#if defined _AT32UC3B
            fnUpdataPeripheralFunctions(0, (PMR0_0 | ulOscillator), (PMR1_0 | ulOscillator), (GPER_0 & ~ulOscillator));
#elif defined _AT32UC3C                                                  // {6}
            fnUpdataPeripheralFunctions(0, PMR0_0, PMR1_0, PMR2_0, GPER_0);
#else
            fnUpdataPeripheralFunctions(0, PMR0_0, PMR1_0, GPER_0);
#endif
            while (i < 29) {                                             // all possible ADC inputs
                if (ADC_CHSR & (0x01 << (i - 21))) {                     // ADC function set
                    ucPortFunctions[0][i] = 1;                           // force ADC peripheral mode
                }
                i++;
            }
        }
        break;
#if PORTS_AVAILABLE > 1
    case 1:
        GPER_1 ^= GPERT_1;                                               // toggle enable bits
        GPER_1 |= GPERS_1;                                               // set enable bits
        GPER_1 &= ~GPERC_1;                                              // clear enable bits
        GPERT_1 = GPERS_1 = GPERC_1 = 0;                                 // reset write only registers

        PMR0_1 ^= PMR0T_1;                                               // toggle mux0 bits
        PMR0_1 |= PMR0S_1;                                               // set mux0 bits
        PMR0_1 &= ~PMR0C_1;                                              // clear mux0 bits
        PMR0T_1 = PMR0S_1 = PMR0C_1 = 0;                                 // reset write only registers

        PMR1_1 ^= PMR1T_1;                                               // toggle mux1 bits
        PMR1_1 |= PMR1S_1;                                               // set mux1 bits
        PMR1_1 &= ~PMR1C_1;                                              // clear mux1 bits
        PMR1T_1 = PMR1S_1 = PMR1C_1 = 0;                                 // reset write only registers

    #ifdef _AT32UC3C                                                     // {6}
        PMR2_1 ^= PMR2T_1;                                               // toggle mux1 bits
        PMR2_1 |= PMR2S_1;                                               // set mux1 bits
        PMR2_1 &= ~PMR2C_1;                                              // clear mux1 bits
        PMR2T_1 = PMR2S_1 = PMR2C_1 = 0;                                 // reset write only registers
    #endif

        ODER_1 ^= ODERT_1;                                               // toggle driver bits
        ODER_1 |= ODERS_1;                                               // set driver bits
        ODER_1 &= ~ODERC_1;                                              // clear driver bits
        ODERT_1 = ODERS_1 = ODERC_1 = 0;                                 // reset write only registers

        PUER_1 ^= PUERT_1;                                               // toggle pull-up bits
        PUER_1 |= PUERS_1;                                               // set pull-up bits
        PUER_1 &= ~PUERC_1;                                              // clear pull-up bits
        PUERT_1 = PUERS_1 = PUERC_1 = 0;                                 // reset write only registers
    #if !defined _AT32UC3C                                               // {12}
        ODMER_1 ^= ODMERT_1;                                             // toggle open drain bits
        ODMER_1 |= ODMERS_1;                                             // set open drain bits
        ODMER_1 &= ~ODMERC_1;                                            // clear open drain bits
        ODMERT_1 = ODMERS_1 = ODMERC_1 = 0;                              // reset write only registers
    #endif
        GFER_1 ^= GFERT_1;                                               // toggle glitch filter bits
        GFER_1 |= GFERS_1;                                               // set glitch filter bits
        GFER_1 &= ~GFERC_1;                                              // clear glitch filter bits
        GFERT_1 = GFERS_1 = GFERC_1 = 0;                                 // reset write only registers

        OVR_1 ^= OVRT_1;                                                 // toggle port bits
        OVR_1 |= OVRS_1;                                                 // set port bits
        OVR_1 &= ~OVRC_1;                                                // clear port bits
        OVRT_1 = OVRS_1 = OVRC_1 = 0;                                    // reset write only registers

        PVR_1 = (ODER_1 & OVR_1);                                        // output driven values
        PVR_1 |= (~ODER_1 & port1_in);                                   // complete pin state after adding inputs

        if (LastPeripheral0[1] != PMR0_1) {
            LastPeripheral0[1] = PMR0_1;
            iPeripheralChange = 1;
        }
        if (LastPeripheral1[1] != PMR1_1) {
            LastPeripheral1[1] = PMR1_1;
            iPeripheralChange = 1;
        }
    #ifdef _AT32UC3C                                                     // {6}
        if (LastPeripheral2[1] != PMR2_1) {
            LastPeripheral2[1] = PMR2_1;
            iPeripheralChange = 1;
        }
    #endif
        if (LastGPER[1] != GPER_1) {
            LastGPER[1] = GPER_1;
            iPeripheralChange = 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure port display is updated
        if (iPeripheralChange != 0) {
    #if defined _AT32UC3C                                                // {6}
            fnUpdataPeripheralFunctions(1, (PMR0_1 | ulOscillator), (PMR1_1 | ulOscillator), (PMR2_1 | ulOscillator), (GPER_1 & ~ulOscillator));
    #else
            fnUpdataPeripheralFunctions(1, PMR0_1, PMR1_1, GPER_1);
    #endif
        }
        break;
#endif
#if PORTS_AVAILABLE > 2
    case 2:
        GPER_2 ^= GPERT_2;                                               // toggle enable bits
        GPER_2 |= GPERS_2;                                               // set enable bits
        GPER_2 &= ~GPERC_2;                                              // clear enable bits
        GPERT_2 = GPERS_2 = GPERC_2 = 0;                                 // reset write only registers

        PMR0_2 ^= PMR0T_2;                                               // toggle mux0 bits
        PMR0_2 |= PMR0S_2;                                               // set mux0 bits
        PMR0_2 &= ~PMR0C_2;                                              // clear mux0 bits
        PMR0T_2 = PMR0S_2 = PMR0C_2 = 0;                                 // reset write only registers

        PMR1_2 ^= PMR1T_2;                                               // toggle mux1 bits
        PMR1_2 |= PMR1S_2;                                               // set mux1 bits
        PMR1_2 &= ~PMR1C_2;                                              // clear mux1 bits
        PMR1T_2 = PMR1S_2 = PMR1C_2 = 0;                                 // reset write only registers

    #ifdef _AT32UC3C                                                     // {6}
        PMR2_2 ^= PMR2T_2;                                               // toggle mux1 bits
        PMR2_2 |= PMR2S_2;                                               // set mux1 bits
        PMR2_2 &= ~PMR2C_2;                                              // clear mux1 bits
        PMR2T_2 = PMR2S_2 = PMR2C_2 = 0;                                 // reset write only registers
    #endif

        ODER_2 ^= ODERT_2;                                               // toggle driver bits
        ODER_2 |= ODERS_2;                                               // set driver bits
        ODER_2 &= ~ODERC_2;                                              // clear driver bits
        ODERT_2 = ODERS_2 = ODERC_2 = 0;                                 // reset write only registers

        PUER_2 ^= PUERT_2;                                               // toggle pull-up bits
        PUER_2 |= PUERS_2;                                               // set pull-up bits
        PUER_2 &= ~PUERC_2;                                              // clear pull-up bits
        PUERT_2 = PUERS_2 = PUERC_2 = 0;                                 // reset write only registers
    #if !defined _AT32UC3C                                               // {12}
        ODMER_2 ^= ODMERT_2;                                             // toggle open drain bits
        ODMER_2 |= ODMERS_2;                                             // set open drain bits
        ODMER_2 &= ~ODMERC_2;                                            // clear open drain bits
        ODMERT_2 = ODMERS_2 = ODMERC_2 = 0;                              // reset write only registers
    #endif
        GFER_2 ^= GFERT_2;                                               // toggle glitch filter bits
        GFER_2 |= GFERS_2;                                               // set glitch filter bits
        GFER_2 &= ~GFERC_2;                                              // clear glitch filter bits
        GFERT_2 = GFERS_2 = GFERC_2 = 0;                                 // reset write only registers

        OVR_2 ^= OVRT_2;                                                 // toggle port bits
        OVR_2 |= OVRS_2;                                                 // set port bits
        OVR_2 &= ~OVRC_2;                                                // clear port bits
        OVRT_2 = OVRS_2 = OVRC_2 = 0;                                    // reset write only registers

        PVR_2 = (ODER_2 & OVR_2);                                        // output driven values
        PVR_2 |= (~ODER_2 & port2_in);                                   // complete pin state after adding inputs

        if (LastPeripheral0[2] != PMR0_2) {
            LastPeripheral0[2] = PMR0_2;
            iPeripheralChange = 1;
        }
        if (LastPeripheral1[2] != PMR1_2) {
            LastPeripheral1[2] = PMR1_2;
            iPeripheralChange = 1;
        }
    #ifdef _AT32UC3C                                                     // {6}
        if (LastPeripheral2[2] != PMR2_2) {
            LastPeripheral2[2] = PMR2_2;
            iPeripheralChange = 1;
        }
    #endif
        if (LastGPER[2] != GPER_2) {
            LastGPER[2] = GPER_2;
            iPeripheralChange = 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure port display is updated
        if (iPeripheralChange != 0) {
    #if defined _AT32UC3C                                                // {6}
            fnUpdataPeripheralFunctions(2, PMR0_2, PMR1_2, PMR2_2, GPER_2);
    #elif defined _AT32UC3B
            fnUpdataPeripheralFunctions(2, PMR0_2, PMR1_2, GPER_2);
    #else
            fnUpdataPeripheralFunctions(2, (PMR0_2 | ulOscillator), (PMR1_2 | ulOscillator), (GPER_2 & ~ulOscillator));
    #endif
        }
        break;
#endif
#if PORTS_AVAILABLE > 3
    case 3:
        GPER_3 ^= GPERT_3;                                               // toggle enable bits
        GPER_3 |= GPERS_3;                                               // set enable bits
        GPER_3 &= ~GPERC_3;                                              // clear enable bits
        GPERT_3 = GPERS_3 = GPERC_3 = 0;                                 // reset write only registers

        PMR0_3 ^= PMR0T_3;                                               // toggle mux0 bits
        PMR0_3 |= PMR0S_3;                                               // set mux0 bits
        PMR0_3 &= ~PMR0C_3;                                              // clear mux0 bits
        PMR0T_3 = PMR0S_3 = PMR0C_3 = 0;                                 // reset write only registers

        PMR1_3 ^= PMR1T_3;                                               // toggle mux1 bits
        PMR1_3 |= PMR1S_3;                                               // set mux1 bits
        PMR1_3 &= ~PMR1C_3;                                              // clear mux1 bits
        PMR1T_3 = PMR1S_3 = PMR1C_3 = 0;                                 // reset write only registers

    #ifdef _AT32UC3C                                                     // {6}
        PMR2_3 ^= PMR2T_3;                                               // toggle mux2 bits
        PMR2_3 |= PMR2S_3;                                               // set mux2 bits
        PMR2_3 &= ~PMR2C_3;                                              // clear mux1 bits
        PMR2T_3 = PMR2S_3 = PMR2C_3 = 0;                                 // reset write only registers
    #endif

        ODER_3 ^= ODERT_3;                                               // toggle driver bits
        ODER_3 |= ODERS_3;                                               // set driver bits
        ODER_3 &= ~ODERC_3;                                              // clear driver bits
        ODERT_3 = ODERS_3 = ODERC_3 = 0;                                 // reset write only registers

        PUER_3 ^= PUERT_3;                                               // toggle pull-up bits
        PUER_3 |= PUERS_3;                                               // set pull-up bits
        PUER_3 &= ~PUERC_3;                                              // clear pull-up bits
        PUERT_3 = PUERS_3 = PUERC_3 = 0;                                 // reset write only registers
    #if !defined _AT32UC3C                                               // {12}
        ODMER_3 ^= ODMERT_3;                                             // toggle open drain bits
        ODMER_3 |= ODMERS_3;                                             // set open drain bits
        ODMER_3 &= ~ODMERC_3;                                            // clear open drain bits
        ODMERT_3 = ODMERS_3 = ODMERC_3 = 0;                              // reset write only registers
    #endif
        GFER_3 ^= GFERT_3;                                               // toggle glitch filter bits
        GFER_3 |= GFERS_3;                                               // set glitch filter bits
        GFER_3 &= ~GFERC_3;                                              // clear glitch filter bits
        GFERT_3 = GFERS_3 = GFERC_3 = 0;                                 // reset write only registers

        OVR_3 ^= OVRT_3;                                                 // toggle port bits
        OVR_3 |= OVRS_3;                                                 // set port bits
        OVR_3 &= ~OVRC_3;                                                // clear port bits
        OVRT_3 = OVRS_3 = OVRC_3 = 0;                                    // reset write only registers

        PVR_3 = (ODER_3 & OVR_3);                                        // output driven values
        PVR_3 |= (~ODER_3 & port3_in);                                   // complete pin state after adding inputs

        if (LastPeripheral0[3] != PMR0_3) {
            LastPeripheral0[3] = PMR0_3;
            iPeripheralChange = 1;
        }
        if (LastPeripheral1[3] != PMR1_3) {
            LastPeripheral1[3] = PMR1_3;
            iPeripheralChange = 1;
        }
    #ifdef _AT32UC3C                                                     // {6}
        if (LastPeripheral2[3] != PMR2_3) {
            LastPeripheral2[3] = PMR2_3;
            iPeripheralChange = 1;
        }
    #endif
        if (LastGPER[3] != GPER_3) {
            LastGPER[3] = GPER_3;
            iPeripheralChange = 1;
        }
        iFlagRefresh = PORT_CHANGE;                                      // ensure port display is updated
        if (iPeripheralChange != 0) {
    #if defined _AT32UC3C                                                // {6}
            fnUpdataPeripheralFunctions(3, PMR0_3, PMR1_3, PMR2_3, GPER_3);
    #else
            fnUpdataPeripheralFunctions(3, PMR0_3, PMR1_3, GPER_3);
    #endif
        }
        break;
#endif
    default:                                                             // {2} special case after change of oscillator pins
#ifdef _AT32UC3C
        if (SCIF_OSCCTRL0 & SCIF_OSCCTRL_MODE_CRYSTAL) {
            ulOscillator |= (PIN_XIN0 | PIN_XOUT0);
        }
        else {
            ulOscillator &= ~(PIN_XIN0 | PIN_XOUT0);
        }
        if (SCIF_OSCCTRL1 & SCIF_OSCCTRL_MODE_CRYSTAL) {
            ulOscillator |= (PIN_XIN1 | PIN_XOUT1);
        }
        else {
            ulOscillator &= ~(PIN_XIN1 | PIN_XOUT1);
        }
#else
        if (PM_OSCCTRL0 & 0x07) {                                        // oscillator 0 is enabled
            ulOscillator |= (PIN_XIN0 | PIN_XOUT0);
        }
        else {
            ulOscillator &= ~(PIN_XIN0 | PIN_XOUT0);
        }
    #if !defined _AT32UC3B || !defined CHIP_48_PIN                       // oscillator 1 not available on smaller AT32UC3B parts
        if (PM_OSCCTRL1 & 0x07) {                                        // oscillator 1 is enabled
            ulOscillator |= (PIN_XIN1 | PIN_XOUT1);
        }
        else {
            ulOscillator &= ~(PIN_XIN1 | PIN_XOUT1);
        }
    #endif
        if (PM_OSCCTRL32 & 0x00000700) {                                 // oscillator 32 is enabled
            ulOscillator |= (PIN_XIN32 | PIN_XOUT32);
        }
        else {
            ulOscillator &= ~(PIN_XIN32 | PIN_XOUT32);
        }
#endif
#if defined _AT32UC3B
        iPort = 0;                                                       // clocks are all on port 0
        fnUpdataPeripheralFunctions(iPort, ulOscillator, ulOscillator, ~ulOscillator);
#elif defined _AT32UC3C
        iPort = 1;                                                       // clocks are all on port 1
        fnUpdataPeripheralFunctions(iPort, ulOscillator, ulOscillator, ulOscillator, ~ulOscillator);
#else
        iPort = 2;                                                       // clocks are all on port 2
        fnUpdataPeripheralFunctions(iPort, ulOscillator, ulOscillator, ~ulOscillator);
#endif
        iFlagRefresh = PORT_CHANGE;
        break;
    }
}

#if defined _AT32UC3C                                                    // {8}
static unsigned long fnGetExtIntPin(unsigned char ucPort, int iInterrupt)
{
    switch (ucPort) {
    case 0:                                                              // external interrupts on port A
        switch (iInterrupt) {                                            // the interrupt number
        case 0:
            if (ucPortFunctions[ucPort][25] == PERIPHERAL_C_FUNCTION) {  // PA25-C function
                return PA25;
            }
            if (ucPortFunctions[ucPort][29] == PERIPHERAL_B_FUNCTION) {  // PA29-B function
                return PA29;
            }
            break;
        case 1:
            if (ucPortFunctions[ucPort][3] == PERIPHERAL_B_FUNCTION) {   // PA03-B function
                return PA03;
            }
            if (ucPortFunctions[ucPort][19] == PERIPHERAL_B_FUNCTION) {  // PA19-B function
                return PA19;
            }
            if (ucPortFunctions[ucPort][26] == PERIPHERAL_B_FUNCTION) {  // PA26-B function
                return PA26;
            }
            break;
        case 2:
            if (ucPortFunctions[ucPort][8] == PERIPHERAL_C_FUNCTION) {   // PA08-C function
                return PA08;
            }
            if (ucPortFunctions[ucPort][27] == PERIPHERAL_B_FUNCTION) {  // PA27-B function
                return PA27;
            }
            break;
        case 3:
            if (ucPortFunctions[ucPort][28] == PERIPHERAL_B_FUNCTION) {  // PA28-B function
                return PA28;
            }
            break;
        case 4:
            if (ucPortFunctions[ucPort][10] == PERIPHERAL_B_FUNCTION) {  // PA10-B function
                return PA10;
            }
            break;
        }
        break;

    case 1:                                                              // external interrupts on port B
        switch (iInterrupt) {                                            // the interrupt number
        case 2:
            if (ucPortFunctions[ucPort][7] == PERIPHERAL_D_FUNCTION) {   // PB07-B function
                return PB07;
            }
            break;
        case 4:
            if (ucPortFunctions[ucPort][18] == PERIPHERAL_C_FUNCTION) {  // PB18-C function
                return PB18;
            }
            break;
        case 8:
            if (ucPortFunctions[ucPort][0] == PERIPHERAL_C_FUNCTION) {   // PB00-C function
                return PB00;
            }
            break;
        }
        break;

    case 2:                                                              // external interrupts on port C
        switch (iInterrupt) {                                            // the interrupt number
        case 0:
            if (ucPortFunctions[ucPort][27] == PERIPHERAL_D_FUNCTION) {  // PC27-D function
                return PC27;
            }
            break;
        case 1:
            if (ucPortFunctions[ucPort][3] == PERIPHERAL_B_FUNCTION) {   // PC03-B function
                return PC03;
            }
            break;
        case 3:
            if (ucPortFunctions[ucPort][4] == PERIPHERAL_B_FUNCTION) {   // PC04-B function
                return PC04;
            }
            break;
        case 4:
            if (ucPortFunctions[ucPort][5] == PERIPHERAL_B_FUNCTION) {   // PC05-B function
                return PC05;
            }
            break;
        case 5:
            if (ucPortFunctions[ucPort][18] == PERIPHERAL_B_FUNCTION) {  // PC18-B function
                return PC18;
            }
            break;
        case 7:
            if (ucPortFunctions[ucPort][13] == PERIPHERAL_B_FUNCTION) {  // PC13-B function
                return PC13;
            }
            break;
        }
        break;

    case 3:                                                              // external interrupts on port D
        switch (iInterrupt) {                                            // the interrupt number
        case 0:
            if (ucPortFunctions[ucPort][21] == PERIPHERAL_B_FUNCTION) {  // PD21-B function
                return PD21;
            }
            break;
        case 3:
            if (ucPortFunctions[ucPort][30] == PERIPHERAL_B_FUNCTION) {  // PD30-B function
                return PD30;
            }
            break;
        case 5:
            if (ucPortFunctions[ucPort][7] == PERIPHERAL_B_FUNCTION) {   // PD07-B function
                return PD07;
            }
            break;
        case 6:
            if (ucPortFunctions[ucPort][8] == PERIPHERAL_B_FUNCTION) {   // PD08-B function
                return PD08;
            }
            if (ucPortFunctions[ucPort][29] == PERIPHERAL_B_FUNCTION) {  // PD29-B function
                return PD29;
            }
            break;
        case 7:
            if (ucPortFunctions[ucPort][14] == PERIPHERAL_B_FUNCTION) {  // PD14-B function
                return PD14;
            }
            break;
        case 8:
            if (ucPortFunctions[ucPort][24] == PERIPHERAL_B_FUNCTION) {  // PB24-B function
                return PB24;
            }
            break;
        }
        break;
    }
    return 0;                                                            // no pins on this port are configured for external interrupt
}

static void fnHandleUC3C_EIC(unsigned char ucPort, unsigned long ulBit, unsigned long port_in)
{
    unsigned long ext_int_port_bit;
    int iNMI = 0;

    ext_int_port_bit = fnGetExtIntPin(ucPort, 0);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_0) && (ext_int_port_bit & ulBit)) {          // if external interrupt 0 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_0)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_0) && (!(ext_int_port_bit & port_in))) { // rising edge
                iNMI = 1;
            }
            else if ((!(EIC_EDGE & EXT_INT_0)) && (ext_int_port_bit & port_in)) { // falling edge
                iNMI = 1;
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 1);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_1) && (ext_int_port_bit & ulBit)) {          // if external interrupt 1 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_1)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_1) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_1 << 1);
            }
            else if ((!(EIC_EDGE & EXT_INT_1)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_1 << 1);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 2);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_2) && (ext_int_port_bit & ulBit)) {          // if external interrupt 2 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_2)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_2) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_2 << 1);
            }
            else if ((!(EIC_EDGE & EXT_INT_2)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_2 << 1);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 3);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_3) && (ext_int_port_bit & ulBit)) {          // if external interrupt 3 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_3)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_3) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_3 << 1);
            }
            else if ((!(EIC_EDGE & EXT_INT_3)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_3 << 1);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 4);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_4) && (ext_int_port_bit & ulBit)) {          // if external interrupt 4 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_4)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_4) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_4 << 1);
            }
            else if ((!(EIC_EDGE & EXT_INT_4)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR15 |= (IR_EXTERNAL_INT_CONTROLLER_4 << 1);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 5);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_5) && (ext_int_port_bit & ulBit)) {          // if external interrupt 5 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_5)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_5) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_5 << 5);
            }
            else if ((!(EIC_EDGE & EXT_INT_5)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_5 << 5);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 6);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_6) && (ext_int_port_bit & ulBit)) {          // if external interrupt 6 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_6)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_6) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_6 << 5);
            }
            else if ((!(EIC_EDGE & EXT_INT_6)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_6 << 5);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 7);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_7) && (ext_int_port_bit & ulBit)) {          // if external interrupt 7 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_7)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_7) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_7 << 5);
            }
            else if ((!(EIC_EDGE & EXT_INT_7)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_7 << 5);
            }
        }
        else {                                                           // level sensitive
        }
    }
    ext_int_port_bit = fnGetExtIntPin(ucPort, 8);                        // get the port bit that may be configured for this function
    if ((EIC_CTRL & EXT_INT_8) && (ext_int_port_bit & ulBit)) {          // if external interrupt 8 enabled and change on corresponding pin
        if (!(EIC_MODE & EXT_INT_8)) {                                   // edge triggered
            if ((EIC_EDGE & EXT_INT_8) && (!(ext_int_port_bit & port_in))) { // rising edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_8 << 5);
            }
            else if ((!(EIC_EDGE & EXT_INT_8)) && (ext_int_port_bit & port_in)) { // falling edge
                IRR16 |= (IR_EXTERNAL_INT_CONTROLLER_8 << 5);
            }
        }
        else {                                                           // level sensitive
        }
    }
    if (iNMI != 0) {                                                     // external interrupt 0 is NMI in UC3C
        void (*interrupt_call)(void);
        interrupt_call = (void *)AVR32_ExceptionRAM[0x10 + 1];
        interrupt_call();                                                // call external int handler
    }
    if (EIC_IMR & IRR15) {                                               // interrupt not masted
        if (!(ulAVR32_CPU.SR & GM)) {                                    // global interrupt not masked
            void (*interrupt_call)(void);
            interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR15 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
            interrupt_call();                                            // call external int handler
        }
    }
    if (EIC_IMR & IRR16) {                                               // interrupt not masted
        if (!(ulAVR32_CPU.SR & GM)) {                                    // global interrupt not masked
            void (*interrupt_call)(void);
            interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR16 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
            interrupt_call();                                            // call external int handler
        }
    }
}
        
#endif

extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    unsigned long ulBit = (0x00000001 << (31 - ucPortBit));
    if (!(PM_PBAMASK & PBAMASK_GPIO)) {                                  // check that power is powered
        return;
    }

    switch (ucPort) {
    case 0:                                                              // port 0
        if (ulBit & ODER_0) {
            break;                                                       // port is enabled as output
        }
        if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {               // if port is not enabled as output
#ifdef SUPPORT_ADC
            if (ADC_CHSR & (ulBit >> 21)) {                              // input is ADC so change the voltage
                int iAdc = 0;
                unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                ulBit >>= 21;
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
            port0_in ^= ulBit;
            PVR_0 ^= ulBit;                                              // toggle the input state
        }
        else if (iChange == SET_INPUT) {                                 // {7}
            if (port0_in & ulBit) {
                break;                                                   // no change caused
            }
            port0_in |= ulBit;                                           // set the input high
            PVR_0 |= ulBit;
        }
        else  {
            if (!(port0_in & ulBit)) {
                break;                                                   // no change caused
            }
            port0_in &= ~ulBit;                                          // set the input low
            PVR_0 &= ~ulBit;
        }
        if (IER_0 & ulBit) {                                             // if change interrupt enabled
          //if (ulBit & (CTS_0 | CTS_1)) {
          //    fnModemChange(ulBit, ((porta_in & ulBit) == 0), 0);      // handle potential CTS changes
          //}
            if (((ulBit & IMR1_0) && (!(ulBit & port0_in))) ||           // falling edge
                ((!(ulBit & IMR1_0)) && (!((ulBit & IMR0_0)))) ||        // any edge
                ((ulBit & IMR0_0) && (ulBit & port0_in))) {              // rising edge
                IFR_0 |= ulBit;                                          // change detected
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_GPIO & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    if (ulBit & 0x000000ff) {
                        IRR_GPIO |= IR_GPIO_0;
                    }
                    else if (ulBit & 0x0000ff00) {
                        IRR_GPIO |= IR_GPIO_1;
                    }
                    else if (ulBit & 0x00ff0000) {
                        IRR_GPIO |= IR_GPIO_2;
                    }
                    else {
                        IRR_GPIO |= IR_GPIO_3;
                    }
                    interrupt_call();                                    // call GPIO change int handler
                }
            }
        }
#if defined _AT32UC3C                                                    // {8}
        fnHandleUC3C_EIC(ucPort, ulBit, port0_in);
#else
            if ((EIC_CTRL & EXT_INT_0) && (EIC_0_X & ulBit)) {           // if external interrupt 0 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_0)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_0) && (!(EIC_0_X & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_0;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_0)) && (EIC_0_X & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_0;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_1) && (EIC_1_X & ulBit)) {           // if external interrupt 1 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_1)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_1) && (!(EIC_1_X & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_1;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_1)) && (EIC_1_X & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_1;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_2) && (EIC_2_X & ulBit)) {           // if external interrupt 2 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_2)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_2) && (!(EIC_2_X & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_2;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_2)) && (EIC_2_X & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_2;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_3) && (EIC_3_X & ulBit)) {           // if external interrupt 3 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_3)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_3) && (!(EIC_3_X & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_3;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_3)) && (EIC_3_X & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_3;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_4) && (EIC_4_X & ulBit)) {           // if external interrupt 4 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_4)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_4) && (!(EIC_4_X & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_4;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_4)) && (EIC_4_X & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_4;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_5) && (EIC_5_X & ulBit)) {           // if external interrupt 5 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_5)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_5) && (!(EIC_5_X & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_5;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_5)) && (EIC_5_X & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_5;
                    }
                }
                else {                                                   // level sensitive
                }
            }
    #if !defined _AT32UC3B                                               // {1} AT32UC3B has INT0..5 on port A and the 64 pin version has INT6..7 on port B
            if ((EIC_CTRL & EXT_INT_6) && (EIC_6_B & ulBit)) {           // if external interrupt 6 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_6)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_6) && (!(EIC_6_B & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_6;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_6)) && (EIC_6_B & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_6;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_7) && (EIC_7_C & ulBit)) {           // if external interrupt 7 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_7)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_7) && (!(EIC_7_C & port0_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_7;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_7)) && (EIC_7_C & port0_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_7;
                    }
                }
                else {                                                   // level sensitive
                }
            }
    #endif
            if (EIC_IMR & IRR1) {                                        // interrupt not masted
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    interrupt_call();                                    // call external int handler
                }
            }
#endif
        break;
    case 1:                                                              // port 1
        if (ulBit & ODER_1) {
            break;                                                       // port is enabled as output
        }
        if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {               // if port is not enabled as output
            port1_in ^= ulBit;
            PVR_1 ^= ulBit;                                              // toggle the input state
        }
        else if (iChange == SET_INPUT) {                                 // {7}
            if (port1_in & ulBit) {
                break;                                                   // no change caused
            }
            port1_in |= ulBit;                                           // set the input high
            PVR_1 |= ulBit;
        }
        else  {
            if (!(port1_in & ulBit)) {
                break;                                                   // no change caused
            }
            port1_in &= ~ulBit;                                          // set the input low
            PVR_1 &= ~ulBit;
        }
        if (IER_1 & ulBit) {                                             // if change interrupt enabled
          //if (ulBit & (CTS_0 | CTS_1)) {
          //    fnModemChange(ulBit, ((porta_in & ulBit) == 0), 0);      // handle potential CTS changes
          //}
            if (((ulBit & IMR1_1) && (!(ulBit & port1_in))) ||           // falling edge
                ((!(ulBit & IMR1_1)) && (!((ulBit & IMR0_1)))) ||        // any edge
                ((ulBit & IMR0_1) && (ulBit & port1_in))) {              // rising edge
                IFR_1 |= ulBit;                                          // change detected
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_GPIO & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    if (ulBit & 0x000000ff) {
                        IRR_GPIO |= IR_GPIO_4;
                    }
                    else if (ulBit & 0x0000ff00) {
                        IRR_GPIO |= IR_GPIO_5;
                    }
                    else if (ulBit & 0x00ff0000) {
                        IRR_GPIO |= IR_GPIO_6;
                    }
                    else {
                        IRR_GPIO |= IR_GPIO_7;
                    }
                    interrupt_call();                                    // call GPIO change int handler
                }
            }
        }
#ifdef SUPPORT_ADC
        if ((ulBit & ACD_TRIGGER_INPUT) && ((PMR0_1 & ACD_TRIGGER_INPUT) && (!(PMR1_1 & ACD_TRIGGER_INPUT))) && (ADC_MR & TRGEN) && (ADC_MR & TRGSEL_EXT)) { // if ADC trigger configured
            if (port1_in & ACD_TRIGGER_INPUT) {                          // rising edge causes trigger
                fnADC_trigger();
            }
        }
#endif
#if defined _AT32UC3C                                                    // {8}
        fnHandleUC3C_EIC(ucPort, ulBit, port1_in);
#elif defined _AT32UC3B && !defined CHIP_48_PIN                          // {1} AT32UC3B 64 pin version has INT6..7 on port B
            if ((EIC_CTRL & EXT_INT_6) && (EIC_6_A & ulBit)) {           // if external interrupt 6 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_6)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_6) && (!(EIC_6_A & port1_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_6;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_6)) && (EIC_6_A & port1_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_6;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if ((EIC_CTRL & EXT_INT_7) && (EIC_7_A & ulBit)) {           // if external interrupt 7 enabled and change on corresponding pin
                if (!(EIC_MODE & EXT_INT_7)) {                           // edge triggered
                    if ((EIC_EDGE & EXT_INT_7) && (!(EIC_7_A & port1_in))) { // rising edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_7;
                    }
                    else if ((!(EIC_EDGE & EXT_INT_7)) && (EIC_7_A & port1_in)) { // falling edge
                        IRR1 |= IR_EXTERNAL_INT_CONTROLLER_7;
                    }
                }
                else {                                                   // level sensitive
                }
            }
            if (EIC_IMR & IRR1) {                                        // interrupt not masted
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    interrupt_call();                                    // call external int handler
                }
            }
#endif
        break;
    case 2:                                                              // port 2
        if (ulBit & ODER_2) {
            break;                                                       // port is enabled as output
        }
        if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {               // if port is not enabled as output
            port2_in ^= ulBit;
            PVR_2 ^= ulBit;                                              // toggle the input state
        }
        else if (iChange == SET_INPUT) {                                 // {7}
            if (port2_in & ulBit) {
                break;                                                   // no change caused
            }
            port2_in |= ulBit;                                           // set the input high
            PVR_2 |= ulBit;
        }
        else  {
            if (!(port2_in & ulBit)) {
                break;                                                   // no change caused
            }
            port2_in &= ~ulBit;                                          // set the input low
            PVR_2 &= ~ulBit;
        }
        if (IER_2 & ulBit) {                                             // if change interrupt enabled
          //if (ulBit & (CTS_0 | CTS_1)) {
          //    fnModemChange(ulBit, ((porta_in & ulBit) == 0), 0);      // handle potential CTS changes
          //}
            if (((ulBit & IMR1_2) && (!(ulBit & port2_in))) ||           // falling edge
                ((!(ulBit & IMR1_2)) && (!((ulBit & IMR0_2)))) ||        // any edge
                ((ulBit & IMR0_2) && (ulBit & port2_in))) {              // rising edge
                IFR_2 |= ulBit;                                          // change detected
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_GPIO & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    if (ulBit & 0x000000ff) {
                        IRR_GPIO |= IR_GPIO_8;
                    }
                    else if (ulBit & 0x0000ff00) {
                        IRR_GPIO |= IR_GPIO_9;
                    }
                    else if (ulBit & 0x00ff0000) {
                        IRR_GPIO |= IR_GPIO_10;
                    }
                    else {
                        IRR_GPIO |= IR_GPIO_11;
                    }                     
                    interrupt_call();                                    // call GPIO change int handler
                }
            }
        }
#if defined _AT32UC3C                                                    // {8}
        fnHandleUC3C_EIC(ucPort, ulBit, port2_in);
#endif
        break;
    case 3:                                                              // port 3
        if (ulBit & ODER_3) {
            break;                                                       // port is enabled as output
        }
        if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {               // if port is not enabled as output
            port3_in ^= ulBit;
            PVR_3 ^= ulBit;                                              // toggle the input state
        }
        else if (iChange == SET_INPUT) {                                 // {7}
            if (port3_in & ulBit) {
                break;                                                   // no change caused
            }
            port3_in |= ulBit;                                           // set the input high
            PVR_3 |= ulBit;
        }
        else  {
            if (!(port3_in & ulBit)) {
                break;                                                   // no change caused
            }
            port3_in &= ~ulBit;                                          // set the input low
            PVR_3 &= ~ulBit;
        }
        if (IER_3 & ulBit) {                                             // if change interrupt enabled
          //if (ulBit & (CTS_0 | CTS_1)) {
          //    fnModemChange(ulBit, ((porta_in & ulBit) == 0), 0);      // handle potential CTS changes
          //}
            if (((ulBit & IMR1_3) && (!(ulBit & port3_in))) ||           // falling edge
                ((!(ulBit & IMR1_3)) && (!((ulBit & IMR0_3)))) ||        // any edge
                ((ulBit & IMR0_3) && (ulBit & port3_in))) {               // rising edge
                IFR_3 |= ulBit;                                          // change detected
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_GPIO & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    if (ulBit & 0x000000ff) {
                        IRR_GPIO |= IR_GPIO_12;
                    }
                    else if (ulBit & 0x0000ff00) {
                        IRR_GPIO |= IR_GPIO_13;
                    }
#ifdef _AT32UC3C
                    else if (ulBit & 0x00ff0000) {
                        IRR_GPIO |= IR_GPIO_14;
                    }
                    else if (ulBit & 0xff000000) {
                        IRR_GPIO |= IR_GPIO_15;
                    }
#endif
                    interrupt_call();                                    // call GPIO change int handler
                }
            }
        }
#if defined _AT32UC3C                                                    // {8}
        fnHandleUC3C_EIC(ucPort, ulBit, port3_in);
#endif
        break;
    }
    iFlagRefresh = PORT_CHANGE;                                          // ensure port display is updated
}

extern unsigned long fnGetPresentPortState(int portNr)
{
    unsigned long ulState = 0;

    switch (portNr-1) {
    case 0:
        ulState = (PVR_0);
        break;
    case 1:
        ulState = (PVR_1);
        break;
    case 2:  
        ulState = (PVR_2);
        break;
    case 3:
        ulState = (PVR_3);
        break;
    }
    return ulState;
}

extern unsigned long fnGetPresentPortDir(int portNr)
{
    switch (portNr-1) {
    case 0:
        return (ODER_0 & GPER_0);                                        // non outputs returned as zeros
    case 1:
        return (ODER_1 & GPER_1);                                        // non outputs returned as zeros
    case 2:
        return (ODER_2 & GPER_2);                                        // non outputs returned as zeros
    case 3:
        return (ODER_3 & GPER_3);                                        // non outputs returned as zeros
    default:
        return 0;
    }
}

extern unsigned long fnGetPresentPortPeriph(int portNr)
{
    switch (portNr-1) {
    case 0:
#ifdef _AT32UC3B
        return (~(GPER_0) | ulOscillator);                               // respecting oscillator pin overrides
#else
        return (~GPER_0 | (ADC_CHSR << 21));
#endif
    case 1:
#if defined _AT32UC3C
        return (~(GPER_1) | ulOscillator);                               // respecting oscillator pin overrides
#else
        return (~GPER_1);
#endif
    case 2:
#if defined _AT32UC3B || defined _AT32UC3C
        return (~GPER_2);
#else
        return (~(GPER_2) | ulOscillator);                               // respecting oscillator pin overrides
#endif
    case 3:
        return (~GPER_3);
    }
    return 0;
}

// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{
    static int iRTS_action = 0;
    unsigned long ulNewActions = 0;
    int *ptrCnt;
    iInts &= ~PSEUDO_INT;

    if ((iInts & CHANNEL_0_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
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
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART0 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    IRR_UART0 = IR_USART0;
                    interrupt_call();                                    // call UART 0 int handler
                }
#endif
            }
        }
	}

    if ((!(iInts & CHANNEL_0_SERIAL_INT)) || (iMasks & CHANNEL_0_SERIAL_INT)) { // not going to enter again this time so react to RTS changes
        if (NEGATE_RTS_0 & iRTS_action) {
            iRTS_action &= ~NEGATE_RTS_0;
			if (0) {                                                     // when RTS line configured accordingly
                fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                  // negate control line (RS485 uses negative logic)
    #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
                port3_in |= PX03;
    #else
                port0_in |= PA03;
    #endif
				if (!(iMasks & CHANNEL_0_SERIAL_INT)) {                  // if transmission has terminated queue assertion
					iRTS_action |= ASSERT_RTS_0;
				}
			}
        }
        else if (ASSERT_RTS_0 & iRTS_action) {
            iRTS_action &= ~ASSERT_RTS_0;
            fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                      // negate control line (RS485 uses negative logic)
    #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
            port3_in &= ~PX03;
    #else
            port0_in &= ~PA03;
    #endif
        }
    }
	if ((iInts & CHANNEL_1_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
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
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    IRR_UART1 = IR_USART1;
                    interrupt_call();                                    // call UART 1 int handler
                }
#endif
            }
        }
	}
    if ((!(iInts & CHANNEL_1_SERIAL_INT)) || (iMasks & CHANNEL_1_SERIAL_INT)) { // not going to enter again this time so react to RTS changes
        if (NEGATE_RTS_1 & iRTS_action) {
            iRTS_action &= ~NEGATE_RTS_1;
            fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                      // negate control line (RS485 uses negative logic)
    #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
            port2_in |= PX07;
    #else
            port0_in |= PA08;
    #endif
            if (!(iMasks & CHANNEL_0_SERIAL_INT)) {                      // if transmission has terminated queue assertion
                iRTS_action |= ASSERT_RTS_1;
            }
        }
        else if (ASSERT_RTS_1 & iRTS_action) {
            iRTS_action &= ~ASSERT_RTS_1;
            fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                      // negate control line (RS485 uses negative logic)
    #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
            port2_in &= ~PX07;
    #else
            port0_in &= ~PA08;
    #endif
        }
    }

	if ((iInts & CHANNEL_2_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART2];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx2((unsigned char)US_THR_2);
                ulNewActions |= SEND_COM_2;
                iRTS_action |= NEGATE_RTS_2;
		        US_CSR_2 |= (TXRDY);                                     // simulate Tx int 
                if ((US_MR_2 & 0x0000c000) == 0x00008000) {              // check whether we are in tx loop back mode
                    US_RHR_2 = US_THR_2;                                 // put tx byte in rx buffer
			        US_CSR_2 |= (RXRDY);                                 // and Rx due to loop
		        }
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART2 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    IRR_UART2 = IR_USART2;
                    interrupt_call();                                    // call UART 2 int handler
                }
#endif
            }
        }
	}
    if ((!(iInts & CHANNEL_2_SERIAL_INT)) || (iMasks & CHANNEL_2_SERIAL_INT)) { // not going to enter again this time so react to RTS changes
        if (NEGATE_RTS_2 & iRTS_action) {
            iRTS_action &= ~NEGATE_RTS_2;
            fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);                      // negate control line (RS485 uses negative logic)
    #ifndef _AT32UC3C
        #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
            port3_in |= PX13;
        #elif defined USART2_RTS_ON_1
            port1_in |= PB01;
        #else
            port0_in |= PA29;
        #endif
    #endif
            if (!(iMasks & CHANNEL_2_SERIAL_INT)) {                      // if transmission has terminated queue assertion
                iRTS_action |= ASSERT_RTS_2;
            }
        }
        else if (ASSERT_RTS_2 & iRTS_action) {
            iRTS_action &= ~ASSERT_RTS_2;
            fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);                      // negate control line (RS485 uses negative logic)
    #ifndef _AT32UC3C
        #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
            port3_in &= ~PX13;
        #elif defined USART2_RTS_ON_1
            port1_in &= ~PB01;
        #else
            port0_in &= ~PA29;
        #endif
    #endif
        }
    }

	if ((iInts & CHANNEL_3_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART3];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_3_SERIAL_INT;                          // interrupt has been handled
#ifdef SERIAL_INTERFACE
	            fnLogTx3((unsigned char)US_THR_3);
                ulNewActions |= SEND_COM_3;
                iRTS_action |= NEGATE_RTS_3;
		        US_CSR_3 |= (TXRDY);                                     // simulate Tx int 
                if ((US_MR_3 & 0x0000c000) == 0x00008000) {              // check whether we are in tx loop back mode
                    US_RHR_3 = US_THR_3;                                 // put tx byte in rx buffer
			        US_CSR_3 |= (RXRDY);                                 // and Rx due to loop
		        }
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART3 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    IRR_UART3 = IR_USART3;
                    interrupt_call();                                    // call UART 3 int handler
                }
#endif
            }
        }
	}
    if ((!(iInts & CHANNEL_3_SERIAL_INT)) || (iMasks & CHANNEL_3_SERIAL_INT)) { // not going to enter again this time so react to RTS changes
        if (NEGATE_RTS_3 & iRTS_action) {
            iRTS_action &= ~NEGATE_RTS_3;
            fnConfigSimSCI(3, NEGATE_RTS_COM_3, 0);                      // negate control line (RS485 uses negative logic)
    #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
            port2_in |= PX15;
    #else
            port1_in |= PB00;
    #endif
            if (!(iMasks & CHANNEL_3_SERIAL_INT)) {                      // if transmission has terminated queue assertion
                iRTS_action |= ASSERT_RTS_3;
            }
        }
        else if (ASSERT_RTS_3 & iRTS_action) {
            iRTS_action &= ~ASSERT_RTS_3;
            fnConfigSimSCI(3, ASSERT_RTS_COM_3, 0);                      // negate control line (RS485 uses negative logic)
    #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
            port2_in &= ~PX15;
    #else
            port1_in &= ~PB00;
    #endif
        }
    }
#if NUMBER_SERIAL > 4
	if ((iInts & CHANNEL_4_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART4];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_4_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_4_SERIAL_INT;                          // interrupt has been handled
    #ifdef SERIAL_INTERFACE
	            fnLogTx4((unsigned char)US_THR_4);
                ulNewActions |= SEND_COM_4;
//              iRTS_action |= NEGATE_RTS_4;
		        US_CSR_4 |= (TXRDY);                                     // simulate Tx int 
                if ((US_MR_4 & 0x0000c000) == 0x00008000) {              // check whether we are in tx loop back mode
                    US_RHR_4 = US_THR_4;                                 // put tx byte in rx buffer
			        US_CSR_4 |= (RXRDY);                                 // and Rx due to loop
		        }
                if (!(ulAVR32_CPU.SR & GM)) {                            // global interrupt not masked
                    void (*interrupt_call)(void);
                    interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART4 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                    IRR_UART4 = IR_USART4;
                    interrupt_call();                                    // call UART 4 int handler
                }
    #endif
            }
        }
	}
#endif
    if ((ASSERT_RTS_0 | NEGATE_RTS_0 | ASSERT_RTS_1 | NEGATE_RTS_1 | ASSERT_RTS_2 | NEGATE_RTS_2 | ASSERT_RTS_3 | NEGATE_RTS_3) & iRTS_action) {
        iInts |= PSEUDO_INT;                                             // add a pseudo interrupt so we are called on the next tick
        iMasks |= PSEUDO_INT;                                            // never enter immediately due to a speudo-interrupt
    }


    if ((iInts & I2C_INT0) != 0) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT0;                                      // enough I2C interupts handled in this tick period
            }
            else {
#if defined I2C_INTERFACE
		        iInts &= ~I2C_INT0;
                if (TWI_IMR & (TWI_TXRDY | TWI_RXRDY | TWI_TXCOMP)) {
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR11 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR11 = IR_TWI;
                        interrupt_call();                                // call TWI int handler
                    }                                              
                }
#endif
            }
        }
    }
#ifdef USB_INTERFACE
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

// Simulate timers at tick rate if required
//
extern int fnSimTimers(void)
{
#if defined SIMULATE_FPGA
    fnSimFPGAPeriod();
#endif

    if ((WDT_CTRL & WATCHDOG_ENABLE) != 0) {                             // if watchdog enabled
        static unsigned long ulWatchdogCount = 0;
#if defined _AT32UC3C                                                    // {14}
        if (WDT_CLR == (WATCHDOG_KEY_2 | WDT_CLR_WDTCLR)) 
#else
        if (WDT_CLR == 0) 
#endif
        {                                                                // watchdog retrigger has been performed
            WDT_CLR = 0x55aa;                                            // mark read
            ulWatchdogCount = 0;
        }
        else {
            unsigned long ulWatchdogTrigger = (1 << (((WDT_CTRL >> 8) & 0x1f) + 1));
            ulWatchdogCount += ((TICK_RESOLUTION/1000) * SLOW_CLOCK/1000); // add the slow clocks during this interval
            if (ulWatchdogCount >= ulWatchdogTrigger) {
                ulWatchdogCount = 0;
                return RESET_CARD_WATCHDOG;                              // signal watchdog reset
            }
        }
    }

    ulAVR32_CPU.COUNT += (unsigned long)(((float)MASTER_CLOCK*(float)(TICK_RESOLUTION/1000))/(float)1000); // count during tick interval
    if ((ulAVR32_CPU.COMPARE != 0) && (ulAVR32_CPU.COUNT >= ulAVR32_CPU.COMPARE)) {
        ulAVR32_CPU.COUNT = 0;
        if (!(ulAVR32_CPU.SR & GM)) {                                    // global interrupt not masked
            void (*interrupt_call)(void);
            interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR0 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
            IRR0 = IR_SYSBLOCK_COMPARE;
            interrupt_call();                                            // call the interrupt handler
        }
    }
#if !defined _AT32UC3C                                                   // {6}
    if ((PM_PBAMASK & PBAMASK_TC) && (TC_SR_0 & TIM_CLKSTA)) {           // if timer 0 is powered and running
        unsigned long ulTickCount;
        switch (TC_CMR_0 & 0x07) {
        case 0:
            ulTickCount = (((TICK_RESOLUTION / 1000) *32768)/1000);      // clock pulses in tick period (32kHz oscillator)
            break;
        case 1:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(2*1000)); // clock pulses in tick period
            break;
        case 2:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(8*1000)); // clock pulses in tick period
            break;
        case 3:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(32*1000)); // clock pulses in tick period
            break;
        case 4:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(128*1000)); // clock pulses in tick period
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
            if ((TC_CMR_0 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) {         // toggle TIOA0 on match
    #ifdef TIMER_A0_PX
    #else
                ulTIOA ^= TIMER_TIOA0;
    #endif
            }
            if (TC_IER_0 & TIM_CPCS) {                                   // if interrupt enabled on RC match
                if (ADC_MR & TRGSEL_T0) {                                // if the timer is set up to control ADC triggering
                    if ((TC_CMR_0 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) { // toggle TIOA0 on match
                        if (ulTIOA & TIMER_TIOA0) {                      // positive edge on TIOA0
    #ifdef SUPPORT_ADC
                            fnADC_trigger();
    #endif
                        }
                    }
                    TC_CV_0 = 0;                                         // reset count
                }
                else {
                    TC_CV_0 = 0;                                         // reset count
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR14 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR14 = IR_TC0;
                        interrupt_call();                                // call timer interrupt handler
                    }
                }
            }
        }
        else {
            TC_CV_0 = (unsigned short)(TC_CV_0 + ulTickCount);
        }
    }

    if ((PM_PBAMASK & PBAMASK_TC) && (TC_SR_1 & TIM_CLKSTA)) {           // if timer 1 is powered and running
        unsigned long ulTickCount;
        switch (TC_CMR_1 & 0x07) {
        case 0:
            ulTickCount = (((TICK_RESOLUTION / 1000) *32768)/1000);      // clock pulses in tick period (32kHz oscillator)
            break;
        case 1:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(2*1000)); // clock pulses in tick period
            break;
        case 2:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(8*1000)); // clock pulses in tick period
            break;
        case 3:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(32*1000));// clock pulses in tick period
            break;
        case 4:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(128*1000)); // clock pulses in tick period
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
            if ((TC_CMR_1 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) {         // toggle TIOA1 on match
    #ifdef TIMER_A1_PX
    #else
                ulTIOA ^= TIMER_TIOA1;
    #endif
            }
            if (TC_IER_1 & TIM_CPCS) {                                   // if interrupt enabled on RC match
                if (ADC_MR & TRGSEL_T1) {                                // if the timer is set up to control ADC triggering
                    if ((TC_CMR_1 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) { // toggle TIOA1 on match
                        if (ulTIOA & TIMER_TIOA1) {                      // positive edge on TIOA1
    #ifdef SUPPORT_ADC
                            fnADC_trigger();
    #endif
                        }
                    }
                    TC_CV_1 = 0;                                         // reset count
                }
                else {
                    TC_CV_1 = 0;                                         // reset count
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR14 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR14 = IR_TC1;
                        interrupt_call();                                // call timer interrupt handler
                    }
                }
            }
        }
        else {
            TC_CV_1 = (unsigned short)(TC_CV_1 + ulTickCount);
        }
    }

    if ((PM_PBAMASK & PBAMASK_TC) && (TC_SR_2 & TIM_CLKSTA)) {           // if timer 2 is powered and running 
        unsigned long ulTickCount;
        switch (TC_CMR_2 & 0x07) {
        case 0:
            ulTickCount = (((TICK_RESOLUTION / 1000) *32768)/1000);       // {4} clock pulses in tick period (32kHz oscillator)
            break;
        case 1:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(2*1000)); // clock pulses in tick period
            break;
        case 2:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(8*1000)); // clock pulses in tick period
            break;
        case 3:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(32*1000));// clock pulses in tick period
            break;
        case 4:
            ulTickCount = (TICK_RESOLUTION / 1000)*(PBA_CLOCK/(128*1000)); // clock pulses in tick period
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
            if ((TC_CMR_2 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) {         // toggle TIOA2 on match
    #ifdef TIMER_A1_PX
    #else
                ulTIOA ^= TIMER_TIOA2;
    #endif
            }
            if (TC_IER_2 & TIM_CPCS) {                                   // if interrupt enabled on RC match
                if (ADC_MR & TRGSEL_T2) {                                // if the timer is set up to control ADC triggering
                    if ((TC_CMR_2 & ACPC_RC_TOGGLE) == ACPC_RC_TOGGLE) { // toggle TIOA2 on match
                        if (ulTIOA & TIMER_TIOA2) {                      // positive edge on TIOA2
    #ifdef SUPPORT_ADC
                            fnADC_trigger();
    #endif
                        }
                    }
                    TC_CV_2 = 0;                                         // reset count
                }
                else {
                    TC_CV_2 = 0;                                         // reset count
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR14 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR14 = IR_TC2;
                        interrupt_call();                                // call timer interrupt handler
                    }
                }
            }
        }
        else {
            TC_CV_2 = (unsigned short)(TC_CV_2 + ulTickCount);
        }
    }

    if ((PM_PBAMASK & PBAMASK_RM_RTC_EIC) && (RTC_CTRL & RTC_CTRL_EN)) { // {11} RTC is powered up and enabled
        if (RTC_CTRL & RTC_CTRL_CLKEN) {                                 // clock enabled to RTC
            double llCount;
            double llOverflow = (RTC_TOP + 1);
            if (RTC_CTRL & RTC_CTRL_CLK32) {                             // 32kHz clock
                llCount = (unsigned long)(((float)32768 * (float)(TICK_RESOLUTION / 1000))/(float)1000);
            }
            else {                                                       // RC oscillator input (slow clock)
                llCount = (unsigned long)(((float)SLOW_CLOCK * (float)(TICK_RESOLUTION / 1000))/(float)1000);
            }
            llCount += RTC_VAL;
            if (llCount >= llOverflow) {
                llCount -= llOverflow;
                RTC_ISR = RTC_TOPI;
            }
            RTC_VAL = (unsigned long)llCount;
            if (RTC_ISR & RTC_TOPI) {                                    // overflow has occurred
                if (RTC_IMR & RTC_TOPI) {                                // interrupt not masked
                    if (!(ulAVR32_CPU.SR & GM)) {                        // global interrupt not masked
                        void (*interrupt_call)(void);
                        interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                        IRR1 |= IR_REAL_TIME_COUNTER;                    // flag that the interrupt source is active
                        interrupt_call();                                // call RTC interrupt handler
                    }
                }
            }
        }
    }
#endif
#ifdef SUPPORT_ADC
    if (ADC_CR & START) {                                                // software ADC conversion start
        ADC_CR = 0;
        fnADC_trigger();
    }
#endif
    return 0;
}

// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
    #ifdef SERIAL_INTERFACE
    if (iPort == 0) {
        if (!(US_CSR_0 & _RX_ENABLED)) {
            return;                                                      // receiver not enabled so ignore input data
        }
        if (US_RTOR_0 != 0) {
            usIdleCounter0 = (unsigned short)US_RTOR_0;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_0 |= RXRDY;
		    US_RHR_0 = *ptrDebugIn++;
            if (!(US_IMR_0 & RXRDY)) {                                   // no RXRDY interrupt masked
/*                if (US_RCR_0 != 0) {                                   // DMA reception
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
                }*/
            }
            if (!(ulAVR32_CPU.SR & GM)) {                                // global interrupt not masked
                void (*interrupt_call)(void);
                interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART0 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                IRR_UART0 = IR_USART0;
                interrupt_call();                                        // call UART 0 int handler
            }
	    }
    }
    else if (iPort == 1) {
        if (!(US_CSR_1 & _RX_ENABLED)) {
            return;                                                      // receiver not enabled so ignore input data
        }
        if (US_RTOR_1 != 0) {
            usIdleCounter1 = (unsigned short)US_RTOR_1;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_1 |= RXRDY;
		    US_RHR_1 = *ptrDebugIn++;
            if (!(US_IMR_1 & RXRDY)) {                                   // no RXRDY interrupt masked
/*                if (US_RCR_1 != 0) {                                   // DMA reception
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
                }*/
            }
            if (!(ulAVR32_CPU.SR & GM)) {                                // global interrupt not masked
                void (*interrupt_call)(void);
                interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART1 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                IRR_UART1 = IR_USART1;
                interrupt_call();                                        // call UART 1 int handler
            }
	    }
    }
    else if (iPort == 2) {
        if (!(US_CSR_2 & _RX_ENABLED)) {
            return;                                                      // receiver not enabled so ignore input data
        }
        if (US_RTOR_2 != 0) {
            usIdleCounter2 = (unsigned short)US_RTOR_2;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_2 |= RXRDY;
		    US_RHR_2 = *ptrDebugIn++;
            if (!(US_IMR_2 & RXRDY)) {                                   // no RXRDY interrupt masked
/*                if (US_RCR_2 != 0) {                                   // DMA reception
                    *(unsigned long *)US_RPR_2 = US_RHR_2;
                    (US_RPR_2)++;
                    (US_RCR_2)--;
                    if (US_RCR_2 == 0) {                                 // last transfer
                        US_CSR_2 |= ENDRX;
                        if ((US_IMR_2 & ENDRX) == 0) {
                            continue;                                    // no interrupt enabled
                        }
                    }
                    else {
                        continue;
                    }
                }*/
            }
            if (!(ulAVR32_CPU.SR & GM)) {                                // global interrupt not masked
                void (*interrupt_call)(void);
                interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART2 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                IRR_UART2 = IR_USART2;
                interrupt_call();                                        // call UART 2 int handler
            }
	    }
    }
    else if (iPort == 3) {
        if (!(US_CSR_3 & _RX_ENABLED)) {
            return;                                                      // receiver not enabled so ignore input data
        }
        if (US_RTOR_3 != 0) {
            usIdleCounter3 = (unsigned short)US_RTOR_3;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_3 |= RXRDY;
		    US_RHR_3 = *ptrDebugIn++;
            if (!(US_IMR_3 & RXRDY)) {                                   // no RXRDY interrupt masked
/*                if (US_RCR_3 != 0) {                                   // DMA reception
                    *(unsigned long *)US_RPR_3 = US_RHR_3;
                    (US_RPR_3)++;
                    (US_RCR_3)--;
                    if (US_RCR_3 == 0) {                                 // last transfer
                        US_CSR_3 |= ENDRX;
                        if ((US_IMR_3 & ENDRX) == 0) {
                            continue;                                    // no interrupt enabled
                        }
                    }
                    else {
                        continue;
                    }
                }*/
            }
            if (!(ulAVR32_CPU.SR & GM)) {                                // global interrupt not masked
                void (*interrupt_call)(void);
                interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART3 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                IRR_UART3 = IR_USART3;
                interrupt_call();                                        // call UART 3 int handler
            }
	    }
    }
        #if NUMBER_SERIAL > 4
    else if (iPort == 4) {
        if (!(US_CSR_4 & _RX_ENABLED)) {
            return;                                                      // receiver not enabled so ignore input data
        }
        if (US_RTOR_4 != 0) {
            usIdleCounter4 = (unsigned short)US_RTOR_4;                  // retrigger the idle counter on character detection
        }
	    while (usLen--) {
            US_CSR_4 |= RXRDY;
		    US_RHR_4 = *ptrDebugIn++;
            if (!(US_IMR_4 & RXRDY)) {                                   // no RXRDY interrupt masked
/*                if (US_RCR_4 != 0) {                                   // DMA reception
                    *(unsigned long *)US_RPR_4 = US_RHR_4;
                    (US_RPR_4)++;
                    (US_RCR_4)--;
                    if (US_RCR_4 == 0) {                                 // last transfer
                        US_CSR_4 |= ENDRX;
                        if ((US_IMR_4 & ENDRX) == 0) {
                            continue;                                    // no interrupt enabled
                        }
                    }
                    else {
                        continue;
                    }
                }*/
            }
            if (!(ulAVR32_CPU.SR & GM)) {                                // global interrupt not masked
                void (*interrupt_call)(void);
                interrupt_call = (void *)AVR32_ExceptionRAM[(((IPR_UART4 & 0x3fff) | ulAVR32_CPU.EVBA) + sizeof(unsigned long))/sizeof(unsigned long)];
                IRR_UART4 = IR_USART4;
                interrupt_call();                                        // call UART 4 int handler
            }
	    }
    }
        #endif
    #endif
}

#ifdef SUPPORT_RTC                                                       // {11}

// The AVR32 doesn't have battery backup for its RTC (real-time counter)
//
extern void fnInitInternalRTC(char *argv[])
{
}
#endif


#endif
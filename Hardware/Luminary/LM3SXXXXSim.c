/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3SXXXXSim.c
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   *********************************************************************
   06.12.2008 Added additional serial receivers for the simulator        {1}
   06.12.2008 Corrected UART 2 transmission counter                      {2}
   16.12.2008 Add _LM3S3748 / _LM3S3768 / _LM3S5732 USB support          {3}
   02.01.2009 Add uDMA support                                           {4}
   04.02.2009 Add fnGetFifo_long() to ensure order of FIFO reads when simulating {5}
   05.02.2009 Add General Purpose Timer Modules                          {6}
   05.02.2009 Add fnClearInt()                                           {7}
   03.03.2009 Add iForce parameter to fnPortChanges()                    {8}
   31.03.2009 Add ADC support                                            {9}
   13.08.2009 Improved GPIO input simulation                             {10}
   13.08.2009 Implemented key scan support with direct connections       {11}
   13.08.2009 Queue forced port refreshes                                {12}
   03.09.2009 Correct ADCSSCTL3_1 name                                   {13}
   03.09.2009 Replace DMA_UART2_TRANSMIT_CH by DMA_ALT_UART2_TRANSMIT_CH_1 and DMA_UART2_TRANSMIT_BIT by DMA_ALT_UART2_TRANSMIT_BIT_2
   08.09.2009 Add fixed internal temperature value for ADC tests         {14}
   11.11.2009 Correct fnUpdatePeripherals() port offset use from 0x400 to 0x3fc
   10.02.2010 Correct port interrupt clearing                            {15}
   15.05.2010 Move ADC SW triggered sample before timers to ensure it is handled immediately {16}
   17.05.2010 Block port function changes when lock is active            {17}
   13.07.2010 Add TEMPEST ADC simulation                                 {18}
   05.09.2010 Add touch screen simulation                                {19}
   11.09.2010 When no watchdog interrupt vector command reset            {20}
   28.09.2010 Add external UART transmit interrupt support               {21}
   10.10.2010 Add 16 bit timer mode support                              {22}
   07.10.2018 Add external ports                                         {23}
   07.10.2018 Add key input port default override and extended port default states {24}

*/  


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
#include "config.h"

#ifdef _LM3SXXXX


/**************************************************************************************************/
/********************************* LUMINARY _LM3SXXXX *********************************************/
/**************************************************************************************************/

static void fnCheckPortInterrupts(unsigned char ucPort, unsigned char ucOriginalInput);
static void fnUpdatePeripherals(int iPort, unsigned char ucFunctions, LM3XXXXX_PORT_CONTROL *ptrPort);

#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {23}
    extern unsigned long fnGetExtPortFunction(int iExtPortReference);
    extern unsigned long fnGetExtPortState(int iExtPortReference);
#endif


#if defined CHIP_HAS_uDMA && defined SERIAL_SUPPORT_DMA
    #ifdef PING_PONG_DMA                                                 // uDMA control block size at start of SRAM
        #define RESERVE_DMA_MEMORY   1024                                // assumes maximum 32 DMA channels
    #else
        #define RESERVE_DMA_MEMORY   512                                 // assumes maximum 32 DMA channels
    #endif
#else
    #define RESERVE_DMA_MEMORY       0
#endif

#define GET_PERIPHERALS 0
#define GET_OUTPUTS     1
#define GET_PER_TYPE    2
#define GET_OUTPUT_MASK 3

static unsigned char ucPort_in_A;
static unsigned char ucPort_in_B;
static unsigned char ucPort_in_C;
static unsigned char ucPort_in_D;
static unsigned char ucPort_in_E;
static unsigned char ucPort_in_F;
static unsigned char ucPort_in_G;
static unsigned char ucPort_in_H;                                        // {3}
static unsigned char ucPort_in_J;

static unsigned long ucGPIO_per_A = 0, ucGPIO_per_B = 0, ucGPIO_per_C = 0, ucGPIO_per_D = 0, ucGPIO_per_E = 0, ucGPIO_per_F = 0, ucGPIO_per_G = 0, ucGPIO_per_H = 0, ucGPIO_per_J = 0;

unsigned long vector_ram[(RESERVE_DMA_MEMORY + sizeof(VECTOR_TABLE))/sizeof(unsigned long)]; // long word aligned

static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = { 0 };

#if defined SUPPORT_ADC && defined ADC_CHANNELS                          // {9}
    static unsigned long ulADC_values[ADC_CHANNELS];
    static void *ptrPorts[2] = {(void *)ucPortFunctions, (void *)ulADC_values};
#endif

static int iFlagRefresh = 0;

#if defined USB_INTERFACE                                                // {3}
    typedef struct stUSB_FIFO
    {
        int iTxDoubleBuffered;
        int iRxDoubleBuffered;
        unsigned short tx_add;
        unsigned short rx_add;
        unsigned short tx_size;
        unsigned short rx_size;
        unsigned short last_tx[2];
        unsigned char  ucBuffered;
        unsigned char  txGet;
    } SIM_USB_FIFO;

    static SIM_USB_FIFO USB_FIFOs[NUMBER_OF_USB_ENDPOINTS] = {
        { 0, 0, 0, 0, 64, 64},                                           // endpoint 0 has 64 bytes of FIFo reserved for shared rx/tx
        {0},
        {0},
        {0}
    };
    static unsigned long ulEndpointInt = 0;  
    static int iTxDataToggle[NUMBER_OF_USB_ENDPOINTS] = {0};
    static unsigned long ulFIFO[1024];
#endif
// Initialisation all non-zero registers in the device
//
static void fnSetDevice(unsigned long *ulPortInits)
{
    extern void fnEnterHW_table(void *hw_table);
    unsigned long initial_input_state;
#if defined _TEMPEST_CLASS
    DID0 = 0x10040000;                                                   // Tempest Class
#elif defined _DUST_DEVIL_CLASS
    DID0 = 0x10030000;                                                   // Dust Devil Class
#elif defined _FURY_CLASS
    DID0 = 0x10010000;                                                   // Fury Class
#else
    DID0 = 0x00000000;                                                   // Sandstorm Class
#endif
    DID1 = (0x10000006 | PART_CODE | PIN_COUNT | PACKAGE_TYPE);
    DC0  = PART_DC0;
    DC1  = PART_DC1;
    DC2  = PART_DC2;
    DC3  = PART_DC3;
    DC4  = PART_DC4;
    DC5  = PART_DC5;
    DC6  = PART_DC6;
    DC7  = PART_DC7;
    DC8  = PART_DC8;

    GPIODR2R_A = 0xff;                                                   // GPIO initialisation
    GPIODR2R_B = 0xff;
    GPIODR2R_C = 0xff;
    GPIODR2R_D = 0xff;
    GPIODR2R_E = 0xff;
    GPIODR2R_F = 0xff;
    GPIODR2R_G = 0xff;
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    GPIODR2R_H = 0xff;

    GPIOPeriphID0_H = 0x61;
    GPIOPeriphID2_H = 0x18;
    GPIOPeriphID3_H = 0x01;

    GPIOPcellID0_H = 0x0d;
    GPIOPcellID1_H = 0xf0;
    GPIOPcellID2_H = 0x05;
    GPIOPcellID3_H = 0xd1;
#endif
#if PART_DC4 & GPIOJ_PRESENT4                                            // {3}
    GPIODR2R_J = 0xff;

    GPIOPeriphID0_J = 0x61;
    GPIOPeriphID2_J = 0x18;
    GPIOPeriphID3_J = 0x01;

    GPIOPcellID0_J = 0x0d;
    GPIOPcellID1_J = 0xf0;
    GPIOPcellID2_J = 0x05;
    GPIOPcellID3_J = 0xd1;
#endif
#if PART_DC4 & EMAC0_PRESENT4
    MACIM   = 0x0000007f;                                                // Ethernet controller
    MACRCTL = MAC_BADCRC;
    MACTHR  = 0x0000003f;
    MACMDV  = 0x00000080;
    #ifdef _TEMPEST_CLASS
    MACLED  = 0x00000002;
    #endif
#endif
#if PART_DC4 & UDMA_PRESENT4                                             // {4} uDMA
    DMASTAT      = 0x001f0000;
    DMAALTBASE   = 0x00000200;
    DMAPeriphID4 = 0x00000004;
    DMAPeriphID0 = 0x00000030;
    DMAPeriphID1 = 0x000000b2;
    DMAPeriphID2 = 0x0000000b;
    DMAPeriphID3 = 0x00000000;
    DMACellID0   = 0x0000000d;
    DMACellID1   = 0x000000f0;
    DMACellID2   = 0x00000005;
    DMACellID3   = 0x000000b1;
#endif
#if PART_DC6 & USB0_OTG_PRESENT6                                         // {3} USB
    RCC2      = (0x07806810 | USBPWRDWN);

    USBPOWER  = 0x20;
    USBTXIE   = 0x000f;
    USBRXIE   = 0x000e;
    USBIE     = 0x06;
    USBDEVCTL = 0x80;
    USBCONTIM = 0x5c;
    USBVPLEN  = 0x3c;
    USBFSEOF  = 0x77;
    USBLSEOF  = 0x72;
    USBTXDPKTBUFDIS = 0x000e;
#endif
#if defined DEVICE_HAS_JTAG_PB7
    ucGPIO_per_B = GPIOAFSEL_B = GPIODEN_B = GPIOPUR_B = 0x80;           // active JTAG default (PB7)
#endif
    ucGPIO_per_C = GPIOAFSEL_C = GPIODEN_C = GPIOPUR_C = 0x0f;           // active JTAG default (PC[0..3])
#ifdef _TEMPEST_CLASS
    ucGPIO_per_A = GPIOAFSEL_A = GPIODEN_A = 0x3f;
    ucGPIO_per_B = GPIOAFSEL_B = GPIODEN_B = 0x0c;
    GPIOPCTL_A  = (PA0_U0Rx | PA1_U0Tx | PA2_SSI0Clk | PA3_SSI0Fss | PA4_SSI0Rx | PA5_SSI0Tx);
    GPIOPCTL_B  = (PB2_I2C0SCL | PB3_I2C0SDA);
    GPIOPCTL_C  = (PC0_TCK_SWCLK | PC1_TMS_SWDIO | PC2_TDI | PC3_TDO_SWO);

    RMVER       = 0x02025400;                                            // ROM version register
    FMPRE0_0    = 0xffffffff;                                            // no FLASH read protection (alias)
    FMPPE0_0    = 0xffffffff;                                            // no FLASH program protection (alias)
    USER_REG2   = USER_REG2_VALUE;
    USER_REG3   = USER_REG3_VALUE;
#endif
    GPIOCR_B = 0x7f;                                                     // {17} NMI blocked
    GPIOCR_C = 0xf0;                                                     // {17} JTAG pins blocked

    GPIOPeriphID0_A = GPIOPeriphID0_B = GPIOPeriphID0_C = GPIOPeriphID0_D = GPIOPeriphID0_E = GPIOPeriphID0_F = GPIOPeriphID0_G = 0x61;
    GPIOPeriphID2_A = GPIOPeriphID2_B = GPIOPeriphID2_C = GPIOPeriphID2_D = GPIOPeriphID2_E = GPIOPeriphID2_F = GPIOPeriphID2_G = 0x18;
    GPIOPeriphID3_A = GPIOPeriphID3_B = GPIOPeriphID3_C = GPIOPeriphID3_D = GPIOPeriphID3_E = GPIOPeriphID3_F = GPIOPeriphID3_G = 0x01;

    GPIOPcellID0_A = GPIOPcellID0_B = GPIOPcellID0_C = GPIOPcellID0_D = GPIOPcellID0_E = GPIOPcellID0_F = GPIOPcellID0_G = 0x0d;
    GPIOPcellID1_A = GPIOPcellID1_B = GPIOPcellID1_C = GPIOPcellID1_D = GPIOPcellID1_E = GPIOPcellID1_F = GPIOPcellID1_G = 0xf0;
    GPIOPcellID2_A = GPIOPcellID2_B = GPIOPcellID2_C = GPIOPcellID2_D = GPIOPcellID2_E = GPIOPcellID2_F = GPIOPcellID2_G = 0x05;
    GPIOPcellID3_A = GPIOPcellID3_B = GPIOPcellID3_C = GPIOPcellID3_D = GPIOPcellID3_E = GPIOPcellID3_F = GPIOPcellID3_G = 0xd1;

    PBORCTL    = 0x00007ffd;                                             // system control
#ifdef _LM3S9B95
    RCC        = 0x078e3ad1;
#else
    RCC        = 0x07802800;
#endif
    RCGC0      = 0x00000040;
    SCGC0      = 0x00000040;
    DCGC0      = 0x00000040;
    DSLPCLKCFG = 0x07800000;

    CPUID_BASE_REGISTER = 0x411fc231;

    WDTLOAD      = 0xffffffff;                                           // watchdog
    WDTVALUE     = 0xffffffff;                                           // watchdog counter value
    WDTPeriphID0 = 0x05;
    WDTPeriphID1 = 0x18;
    WDTPeriphID2 = 0x18;
    WDTPeriphID3 = 0x01;
    WDTPCellID0  = 0x0d;
    WDTPCellID1  = 0xf0;
    WDTPCellID2  = 0x05;
    WDTPCellID3  = 0xb1;

    WDTICR       = 0x55555555;                                           // this is a write only register - it is used here to determine if the watchdog has been retriggered

    FMPRE0       = 0xffffffff;                                           // no FLASH read protection
    FMPPE0       = 0xffffffff;                                           // no FLASH program protection
    USECRL       = 0x31;
    USER_DBG     = 0xffffffff;
    USER_REG0    = 0xffffffff;
    USER_REG1    = 0xffffffff;
    FMPRE1       = 0xffffffff;
    FMPRE2       = 0xffffffff;
    FMPRE3       = 0xffffffff;
    FMPPE1       = 0xffffffff;
    FMPPE2       = 0xffffffff;
    FMPPE3       = 0xffffffff;

    SSISR_0         = (SSI_TFE | SSI_TNF);                               // SSI 0
    SSIRIS_0        = 0x00000008;
    SSIPeriphID0_0  = 0x00000022;
    SSIPeriphID2_0  = 0x00000018;
    SSIPeriphID3_0  = 0x00000001;
    SSIPCellID0_0   = 0x0000000d;
    SSIPCellID1_0   = 0x000000f0;
    SSIPCellID2_0   = 0x00000005;
    SSIPCellID3_0   = 0x000000b1;

    UARTFR_0        = 0x00000090;                                        // UART 0
    UARTCTL_0       = 0x00000300;
    UARTIFLS_0      = 0x00000012;
    UARTPeriphID0_0 = 0x00000011;
    UARTPeriphID2_0 = 0x00000018;
    UARTPeriphID3_0 = 0x00000001;
    UARTPCellID0_0  = 0x0000000d;
    UARTPCellID1_0  = 0x000000f0;
    UARTPCellID2_0  = 0x00000005;
    UARTPCellID3_0  = 0x000000b1;

    UARTFR_1        = 0x00000090;                                        // UART 1
    UARTCTL_1       = 0x00000300;
    UARTIFLS_1      = 0x00000012;
    UARTPeriphID0_1 = 0x00000011;
    UARTPeriphID2_1 = 0x00000018;
    UARTPeriphID3_1 = 0x00000001;
    UARTPCellID0_1  = 0x0000000d;
    UARTPCellID1_1  = 0x000000f0;
    UARTPCellID2_1  = 0x00000005;
    UARTPCellID3_1  = 0x000000b1;
#if CHIP_HAS_UARTS > 2
    UARTFR_2        = 0x00000090;                                        // UART 2
    UARTCTL_2       = 0x00000300;
    UARTIFLS_2      = 0x00000012;
    UARTPeriphID0_2 = 0x00000011;
    UARTPeriphID2_2 = 0x00000018;
    UARTPeriphID3_2 = 0x00000001;
    UARTPCellID0_2  = 0x0000000d;
    UARTPCellID1_2  = 0x000000f0;
    UARTPCellID2_2  = 0x00000005;
    UARTPCellID3_2  = 0x000000b1;
#endif
    GPTMTAILR_0     = 0xffffffff;                                        // {6} General Purpose Timer Modules
    GPTMTBILR_0     = 0x0000ffff;
    GPTMTAMATCHR_0  = 0xffffffff;
    GPTMTBMATCHR_0  = 0x0000ffff;
    GPTMTAR_0       = 0xffffffff;
    GPTMTBR_0       = 0xffffffff;
    GPTMTAILR_1     = 0xffffffff;
    GPTMTBILR_1     = 0x0000ffff;
    GPTMTAMATCHR_1  = 0xffffffff;
    GPTMTBMATCHR_1  = 0x0000ffff;
    GPTMTAR_1       = 0xffffffff;
    GPTMTBR_1       = 0xffffffff;
#if CHIP_HAS_GP_TIMER > 2
    GPTMTAILR_2     = 0xffffffff;
    GPTMTBILR_2     = 0x0000ffff;
    GPTMTAMATCHR_2  = 0xffffffff;
    GPTMTBMATCHR_2  = 0x0000ffff;
    GPTMTAR_2       = 0xffffffff;
    GPTMTBR_2       = 0xffffffff;
#endif
#if CHIP_HAS_GP_TIMER > 3
    GPTMTAILR_3     = 0xffffffff;
    GPTMTBILR_3     = 0x0000ffff;
    GPTMTAMATCHR_3  = 0xffffffff;
    GPTMTBMATCHR_3  = 0x0000ffff;
    GPTMTAR_3       = 0xffffffff;
    GPTMTBR_3       = 0xffffffff;
#endif
#if PART_DC1 & (CAN0_PRESENT1 | CAN1_PRESENT1)
    CANCTL = 0x00000001;                                                 // CAN controller
    CANBIT = 0x00002301;
    CANIF1CRQ = 0x00000001;
    CANIF1MSK1 = 0x0000ffff;
    CANIF1MSK2 = 0x0000ffff;
    CANIF2MSK1 = 0x0000ffff;
    CANIF2MSK2 = 0x0000ffff;
#endif
#if PART_DC4 & GPIOA_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTA);         // {24} allow key input defaults override port input defaults
    GPIODATA_A   = ucPort_in_A = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOB_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTB);         // allow key input defaults override port input defaults
    GPIODATA_B   = ucPort_in_B = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOC_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTC);         // allow key input defaults override port input defaults
    GPIODATA_C   = ucPort_in_C = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOD_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTD);         // allow key input defaults override port input defaults
    GPIODATA_D   = ucPort_in_D = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOE_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTE);         // allow key input defaults override port input defaults
    GPIODATA_E   = ucPort_in_E = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOF_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTF);         // allow key input defaults override port input defaults
    GPIODATA_F   = ucPort_in_F = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOG_PRESENT4
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTG);         // allow key input defaults override port input defaults
    GPIODATA_G   = ucPort_in_G = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTH);         // allow key input defaults override port input defaults
    GPIODATA_H   = ucPort_in_H = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if PART_DC4 & GPIOJ_PRESENT4                                            // {3}
    initial_input_state = fnKeyPadState(*ulPortInits++, _PORTJ);         // allow key input defaults override port input defaults
    GPIODATA_J   = ucPort_in_J = (unsigned char)initial_input_state;     // set port inputs to default states
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 0) && defined PORT_EXT0_DEFAULT_INPUT // {24}
    initial_input_state = fnKeyPadState(PORT_EXT0_DEFAULT_INPUT, _PORT_EXT_0);
    fnSetI2CPort(_PORT_EXT_0, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 1) && defined PORT_EXT1_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT1_DEFAULT_INPUT, _PORT_EXT_1);
    fnSetI2CPort(_PORT_EXT_1, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 2) && defined PORT_EXT2_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT2_DEFAULT_INPUT, _PORT_EXT_2);
    fnSetI2CPort(_PORT_EXT_2, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 3) && defined PORT_EXT3_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT3_DEFAULT_INPUT, _PORT_EXT_3);
    fnSetI2CPort(_PORT_EXT_3, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 4) && defined PORT_EXT0_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT4_DEFAULT_INPUT, _PORT_EXT_4);
    fnSetI2CPort(_PORT_EXT_4, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 5) && defined PORT_EXT1_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT5_DEFAULT_INPUT, _PORT_EXT_5);
    fnSetI2CPort(_PORT_EXT_5, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 6) && defined PORT_EXT2_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT6_DEFAULT_INPUT, _PORT_EXT_6);
    fnSetI2CPort(_PORT_EXT_6, DEFINE_INPUT, initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 7) && defined PORT_EXT3_DEFAULT_INPUT
    initial_input_state = fnKeyPadState(PORT_EXT7_DEFAULT_INPUT, _PORT_EXT_7);
    fnSetI2CPort(_PORT_EXT_7, DEFINE_INPUT, initial_input_state);
#endif
#if PART_DC2 & (I2C0_PRESENT2 | I2C1_PRESENT2)
    I2CMTPR_0 = 0x00000001;
    I2CMTPR_1 = 0x00000001;
#endif
#if PART_DC4 & GPIOA_PRESENT4
    fnUpdatePeripherals(_PORTA, (unsigned char)GPIOAFSEL_A, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_A_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOB_PRESENT4
    fnUpdatePeripherals(_PORTB, (unsigned char)GPIOAFSEL_B, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_B_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOC_PRESENT4
    fnUpdatePeripherals(_PORTC, (unsigned char)GPIOAFSEL_C, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_C_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOD_PRESENT4
    fnUpdatePeripherals(_PORTD, (unsigned char)GPIOAFSEL_D, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_D_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOE_PRESENT4
    fnUpdatePeripherals(_PORTE, (unsigned char)GPIOAFSEL_E, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_E_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOF_PRESENT4
    fnUpdatePeripherals(_PORTF, (unsigned char)GPIOAFSEL_F, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_F_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOG_PRESENT4
    fnUpdatePeripherals(_PORTG, (unsigned char)GPIOAFSEL_G, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_G_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOH_PRESENT4
    fnUpdatePeripherals(_PORTH, (unsigned char)GPIOAFSEL_H, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_H_BLOCK + 0x03fc));
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    fnUpdatePeripherals(_PORTJ, (unsigned char)GPIOAFSEL_J, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_J_BLOCK + 0x03fc));
#endif
#if PART_DC1 & ADC0_PRESENT1                                             // {9}
    ADCPSSI_0     = (SS3_PRIORITY_3 | SS2_PRIORITY_2 | SS1_PRIORITY_1 | SS0_PRIORITY_0);
    ADCSSFSTAT0_0 = FIFO_EMPTY;
    ADCSSFSTAT1_0 = FIFO_EMPTY;
    ADCSSFSTAT2_0 = FIFO_EMPTY;
    ADCSSFSTAT3_0 = FIFO_EMPTY;
    ADCSSCTL3_0   = END_SAMPLE_1;
#endif
#if PART_DC1 & ADC1_PRESENT1
    ADCPSSI_1     = (SS3_PRIORITY_3 | SS2_PRIORITY_2 | SS1_PRIORITY_1 | SS0_PRIORITY_0);
    ADCSSFSTAT0_1 = 0x00000100;
    ADCSSFSTAT1_1 = 0x00000100;
    ADCSSFSTAT2_1 = 0x00000100;
    ADCSSFSTAT3_1 = 0x00000100;
    ADCSSCTL3_1   = END_SAMPLE_1;                                        // {13}
#endif
#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
    memcpy(ulADC_values, ulPortInits, sizeof(ulADC_values));             // prime initial ADC values
    fnEnterHW_table(ptrPorts);                                           
#else
    fnEnterHW_table(ucPortFunctions);
#endif
    USER_REG0 = USER_REG0_VALUE;
    USER_REG1 = USER_REG1_VALUE;
}


unsigned char ucFLASH[SIZE_OF_FLASH];

extern unsigned long fnGetFlashSize(int iMemory)
{
    return SIZE_OF_FLASH;
}

void fnInitialiseDevice(void *port_inits)
{
    uMemset(ucFLASH, 0xff, sizeof(ucFLASH));                             // we start with deleted FLASH memory contents
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
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    return ucAdd + sim_add;
}

extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    return ucAdd - sim_add;
}

extern unsigned char *fnGetEEPROM(unsigned short usOffset);

#ifdef ACTIVE_FILE_SYSTEM
extern unsigned char *fnGetFileSystemStart(int iMemory)
{
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROM(uFILE_START));
    #else
        #ifdef SAVE_COMPLETE_FLASH
    return (ucFLASH);
        #else
    return (&ucFLASH[uFILE_START]);
        #endif
    #endif
}
#endif




/******************************************************************************************************/

// A copy of the LM3SXxxx peripheral registers for simulation purposes
extern LM3SXXXX_PERIPH LM3Sxxxx = {0};


// Convert a port bit to input count value compatible with the simulator
//
static unsigned char fnMapPortBit(unsigned long ulRealBit)
{
    unsigned long ulBit = 0x80;
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

// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
 #ifdef SERIAL_INTERFACE
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));

    switch (iPort) {
    case 0:
        while (usLen--) {
            UARTRIS_0 |= UART_RXIM;                                      // set interrupt bit
            IRQ0_31_SPR |= (1 << irq_UART0_ID);
            IRQ0_31_CPR = IRQ0_31_SPR;
            UARTDR_0 = *ptrDebugIn++;                                    // put received character into rx holding register
            if (IRQ0_31_SER & (1 << irq_UART0_ID)) {                     // if UART 0 interrupt is not disabled
                if (UARTIM_0 & UART_RXIM) {                              // if receive interrupt is enabled
                    UARTMIS_0 |= UART_RXIM;                              // set masked interrupt status
                    ptrVect->processor_interrupts.irq_UART0();           // call the interrupt handler
                }
            }
        }
        break;
    #if CHIP_HAS_UARTS > 1
    case 1:                                                              // {1}
        while (usLen--) {
            UARTRIS_1 |= UART_RXIM;                                      // set interrupt bit
            IRQ0_31_SPR |= (1 << irq_UART1_ID);
            IRQ0_31_CPR = IRQ0_31_SPR;
            UARTDR_1 = *ptrDebugIn++;                                    // put received character into rx holding register
            if (IRQ0_31_SER & (1 << irq_UART1_ID)) {                     // if UART 1 interrupt is not disabled
                if (UARTIM_1 & UART_RXIM) {                              // if receive interrupt is enabled
                    UARTMIS_1 |= UART_RXIM;                              // set masked interrupt status
                    ptrVect->processor_interrupts.irq_UART1();           // call the interrupt handler
                }
            }
        }
        break;
    #endif
    #if CHIP_HAS_UARTS > 2
    case 2:                                                              // {1}
        while (usLen--) {
            UARTRIS_2 |= UART_RXIM;                                      // set interrupt bit
            IRQ32_63_SPR |= (1 << (irq_UART2_ID - 32));
            IRQ32_63_CPR = IRQ32_63_SPR;
            UARTDR_2 = *ptrDebugIn++;                                    // put received character into rx holding register
            if (IRQ32_63_SER & (1 << (irq_UART2_ID - 32))) {             // if UART 2 interrupt is not disabled
                if (UARTIM_2 & UART_RXIM) {                              // if receive interrupt is enabled
                    UARTMIS_2 |= UART_RXIM;                              // set masked interrupt status
                    ptrVect->processor_interrupts.irq_UART2();           // call the interrupt handler
                }
            }
        }
        break;
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {21}
    default:
        {
            extern int fnRxExtSCI(int iChannel, unsigned char *ptrData, unsigned short usLength);
            int iHandled;
            while ((iHandled = fnRxExtSCI((iPort - NUMBER_SERIAL), ptrDebugIn, usLen)) > 0) { // handle reception of each byte
                switch (iPort - NUMBER_SERIAL) {                         // generate an interrupt on the corresponding port line (assumed negative)
                case 0:
                case 1:
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, (7 - EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                    break;
            #if NUMBER_EXTERNAL_SERIAL > 2
                case 2:
                case 3:
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, (7 - EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters              
                    break;
            #endif
                }
                ptrDebugIn += iHandled;
                usLen -= iHandled;
            }
        }
    #endif
    }
 #endif
}

// UART Break detection simulation
//
extern void fnSimulateBreak(int iPort)
{
}

static void fnUART_Tx_int(int iChannel)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
    switch (iChannel) {
    case 0:
        UARTRIS_0 |= (UART_TXIM);                                        // simulate Tx int 
        UARTFR_0 |= UARTTXFE;                                            // mark that the transmitter is no longer busy
        IRQ0_31_SPR |= (1 << irq_UART0_ID);
        IRQ0_31_CPR = IRQ0_31_SPR;
        if (IRQ0_31_SER & (1 << irq_UART0_ID)) {                         // if UART 0 interrupt is not disabled
            if (UARTIM_0 & UART_TXIM) {                                  // if transmit interrupt is enabled
                UARTMIS_0 |= UART_TXIM;                                  // set masked interrupt status
                ptrVect->processor_interrupts.irq_UART0();               // call the interrupt handler
            }
        } 
        break;
#if CHIP_HAS_UARTS > 1
    case 1:
        UARTRIS_1 |= (UART_TXIM);                                        // simulate Tx int 
        UARTFR_1 |= UARTTXFE;                                            // mark that the transmitter is no longer busy
        IRQ0_31_SPR |= (1 << irq_UART1_ID);
        IRQ0_31_CPR = IRQ0_31_SPR;
        if (IRQ0_31_SER & (1 << irq_UART1_ID)) {                         // if UART 1 interrupt is not disabled
            if (UARTIM_1 & UART_TXIM) {                                  // if transmit interrupt is enabled
                UARTMIS_1 |= UART_TXIM;                                  // set masked interrupt status
                ptrVect->processor_interrupts.irq_UART1();               // call the interrupt handler
            }
        }  
        break;
#endif
    case 2:
        UARTRIS_2 |= (UART_TXIM);                                        // simulate Tx int 
        UARTFR_2 |= UARTTXFE;                                            // mark that the transmitter is no longer busy
        IRQ32_63_SPR |= (1 << (irq_UART2_ID - 32));
        IRQ32_63_CPR = IRQ32_63_SPR;
        if (IRQ32_63_SER & (1 << (irq_UART2_ID - 32))) {                 // if UART 2 interrupt is not disabled
            if (UARTIM_2 & UART_TXIM) {                                  // if transmit interrupt is enabled
                UARTMIS_2 |= UART_TXIM;                                  // set masked interrupt status
                ptrVect->processor_interrupts.irq_UART2();               // call the interrupt handler
            }
        }  
        break;
    }
}

// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{
#ifdef SERIAL_INTERFACE
    extern unsigned char ucTxLast[CHIP_HAS_UARTS];
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
                iInts &= ~CHANNEL_0_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
                if ((unsigned char)UARTDR_0 == 0x13) {
                    (unsigned char)UARTDR_0 = 0x13;
                }
                fnLogTx0(ucTxLast[0]);
                ulNewActions |= SEND_COM_0;
                fnUART_Tx_int(0);
#endif
            }
        }
    }

#if CHIP_HAS_UARTS > 1
    if ((iInts & CHANNEL_1_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
                iInts &= ~CHANNEL_1_SERIAL_INT;                          // interrupt has been handled
    #if defined SERIAL_INTERFACE
                fnLogTx1(ucTxLast[1]);
                ulNewActions |= SEND_COM_1;
                fnUART_Tx_int(1);
    #endif
            }
        }
    }
#endif
#if CHIP_HAS_UARTS > 2
    if ((iInts & CHANNEL_2_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART2];                          // {2}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
                iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
    #ifdef SERIAL_INTERFACE
                fnLogTx2(ucTxLast[2]);
                ulNewActions |= SEND_COM_2;
                fnUART_Tx_int(2);
    #endif
            }
        }
    }
#endif

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {21}
	if ((iInts & CHANNEL_0_EXT_SERIAL_INT) && (argv)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_EXT_SERIAL_INT;                      // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_EXT_SERIAL_INT;                      // interrupt has been handled
                if (fnLogExtTx0() != 0) {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, (7 - EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
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
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, (7 - EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
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
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, (7 - EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
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
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, (7 - EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG); // generate interrupts for each character or a block of characters
                }
                ulNewActions |= SEND_EXT_COM_3;
            }
        }
	}
    #endif
#endif

    if ((iInts & I2C_INT0) != 0) {
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT0;                                      // enough I2C interupts handled in this tick period
            }
            else {
                iInts &= ~I2C_INT0;
#if defined I2C_INTERFACE
                I2CMRIS_0 = I2C_INTERRUPT_BIT;                           // set raw interrupt
                IRQ0_31_SPR |= (1 << irq_I2C0_ID);
                IRQ0_31_CPR = IRQ0_31_SPR;
                if (I2CMIMR_0 & I2C_INTERRUPT_BIT) {
                    I2CMMIS_0 = I2C_INTERRUPT_BIT;
                }
                if ((IRQ0_31_SER & (1 << irq_I2C0_ID)) && (I2CMMIS_0 & I2C_INTERRUPT_BIT)) { // if I2C0 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_I2C0();            // call the interrupt handler
                }  
#endif
            }
        }
    }

    if ((iInts & I2C_INT1) != 0) {
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
        ptrCnt = (int *)argv[THROUGHPUT_I2C1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT1;                                      // enough I2C interupts handled in this tick period
            }
            else {
                iInts &= ~I2C_INT1;
#if defined I2C_INTERFACE
                I2CMRIS_1 = I2C_INTERRUPT_BIT;                           // set raw interrupt
                IRQ32_63_SPR |= (1 << (irq_I2C1_ID - 32));
                IRQ32_63_CPR = IRQ32_63_SPR;
                if (I2CMIMR_1 & I2C_INTERRUPT_BIT) {
                    I2CMMIS_1 = I2C_INTERRUPT_BIT;
                }
                if ((IRQ32_63_SER & (1 << (irq_I2C1_ID - 32))) && (I2CMMIS_1 & I2C_INTERRUPT_BIT)) {          // if I2C1 interrupt is not disabled
                    ptrVect->processor_interrupts.irq_I2C1();            // call the interrupt handler
                }  
#endif
            }
        }
    }
#ifdef USB_INTERFACE                                                     // {3}
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

#if defined CHIP_HAS_uDMA
static int fnHandleTxUART_DMA(DMA_CONTROL_STRUCTURE *ptrDmaControl, int iChannel, int *iCnt)
{
    unsigned short usLength;
    unsigned short usPositionLength;
    int iMask = 0;
    if ((ptrDmaControl->DMACHCTL & XFERMODE_MASK) == 0) {
        return 0;                                                        // not active
    }
    usLength = (unsigned short)((ptrDmaControl->DMACHCTL & XFERSIZE_MASK) >> XFERSIZE_SHIFT);
    usPositionLength = usLength;
    usLength++;
    if (usLength <= *iCnt) {
        ptrDmaControl->DMACHCTL &= ~(XFERSIZE_MASK | XFERMODE_MASK);     // clear the length and set to stop mode
    }
    else {
        usLength -= *iCnt;
        ptrDmaControl->DMACHCTL &= ~(XFERSIZE_MASK);
        ptrDmaControl->DMACHCTL |= ((usLength - 1) << XFERSIZE_SHIFT);   // set remaining length
        usLength = *iCnt;                                                // send maximum for TICK period
        switch (iChannel) {
        case 0:
	        iMask = DMA_UART0_TRANSMIT_BIT;
            break;
    #if CHIP_HAS_UARTS > 1
        case 1:
	        iMask = DMA_UART1_TRANSMIT_BIT;
            break;
    #endif
    #if CHIP_HAS_UARTS > 2
        case 2:
	        iMask = DMA_ALT_UART2_TRANSMIT_BIT_2;                        // 
            break;
    #endif
        }
    }
    *iCnt -= usLength;
    while (usLength--) {
        unsigned long ulValue;
        unsigned short usOffset;
        switch (ptrDmaControl->DMACHCTL & SRCINC_NO_INC) {
        case SRCINC_BYTE:
            usOffset = usPositionLength;
            break;
        case SRCINC_HALF_WORD:
            usOffset = (usPositionLength * sizeof(unsigned short));
            break;
        case SRCINC_WORD:
            usOffset = (usPositionLength * sizeof(unsigned long));
            break;
        default:
            usOffset = 0;
            break;
        }
        switch (ptrDmaControl->DMACHCTL & (SRCSIZE_32_BIT | SRCSIZE_16_BIT)) {
        case SRCSIZE_8_BIT:
            ulValue = *(unsigned char*)(ptrDmaControl->DMASRCENDP - usOffset);
            break;
        case SRCSIZE_16_BIT:
            ulValue = *(unsigned short*)(ptrDmaControl->DMASRCENDP - usOffset);
            break;
        case SRCSIZE_32_BIT:
            ulValue = *(unsigned long*)(ptrDmaControl->DMASRCENDP - usOffset);
            break;
        }
        switch (ptrDmaControl->DMACHCTL & DSTINC_NO_INC) {
        case DSTINC_BYTE:
            usOffset = usPositionLength;
            break;
        case DSTINC_HALF_WORD:
            usOffset = (usPositionLength * sizeof(unsigned short));
            break;
        case DSTINC_WORD:
            usOffset = (usPositionLength * sizeof(unsigned long));
            break;
        default:
            usOffset = 0;
            break;
        }
        switch (ptrDmaControl->DMACHCTL & (DSTSIZE_32_BIT | DSTSIZE_16_BIT)) {
        case DSTSIZE_8_BIT:
            *(unsigned char *)(ptrDmaControl->DMADSTENDP - usOffset) = (unsigned char)ulValue;
            break;
        case DSTSIZE_16_BIT:
             *(unsigned short *)(ptrDmaControl->DMADSTENDP - usOffset) = (unsigned short)ulValue;
            break;
        case DSTSIZE_32_BIT:
            *(unsigned long *)(ptrDmaControl->DMADSTENDP - usOffset) = (unsigned long)ulValue;
            break;
        }
        switch (iChannel) {
        case 0:
	        fnLogTx0((unsigned char)UARTDR_0);                               // assuming the transfer has been into the tx buffer, cause the byte to be sent
            break;
    #if CHIP_HAS_UARTS > 1
        case 1:
	        fnLogTx1((unsigned char)UARTDR_1);                               // assuming the transfer has been into the tx buffer, cause the byte to be sent
            break;
    #endif
    #if CHIP_HAS_UARTS > 2
        case 2:
	        fnLogTx2((unsigned char)UARTDR_2);                               // assuming the transfer has been into the tx buffer, cause the byte to be sent
            break;
    #endif
        }
        usPositionLength--;
    }
    return iMask;
}
#endif

// Process simulated DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    unsigned long ulNewActions = 0;
#if defined CHIP_HAS_uDMA
    DMA_CONTROL_STRUCTURE *ptrDmaControl;
    if ((iDMA & DMA_UART0_TRANSMIT_BIT) != 0) {                          // UART 0 Tx DMA is active
        if ((UARTDMACTL_0 & TXDMAE) && (DMAENASET & DMA_UART0_TRANSMIT_BIT)) { // only when UART DMA operation is connected and channel enabled
            int *ptrCnt = (int *)argv[0];                                // the maximum number of characters that can be sent in a TICK period
            if (*ptrCnt != 0) {
                ptrDmaControl = (DMA_CONTROL_STRUCTURE *)((CAST_POINTER_ARITHMETIC)&vector_ram + (DMA_UART0_TRANSMIT_CH*sizeof(DMA_CONTROL_STRUCTURE)));
                iMasks |= fnHandleTxUART_DMA(ptrDmaControl, 0, ptrCnt);
                ulNewActions |= SEND_COM_0;                              // inform UART simulator that data should be sent
                if (!(iMasks & DMA_UART0_TRANSMIT_BIT)) {                // if DMA transmission has completed
                    iDMA &= ~DMA_UART0_TRANSMIT_BIT;
                    DMAENASET &= ~DMA_UART0_TRANSMIT_BIT;                // DMA disabled on this channel
                    fnUART_Tx_int(0);
                }
            }
            else {
                iMasks |= DMA_UART0_TRANSMIT_BIT;
            }
        }
        else {
            iDMA &= ~DMA_UART0_TRANSMIT_BIT;
        }
    }
    #if CHIP_HAS_UARTS > 1
    if ((iDMA & DMA_UART1_TRANSMIT_BIT) != 0) {                          // UART 1 Tx DMA is active
        if ((UARTDMACTL_1 & TXDMAE) && (DMAENASET & DMA_UART1_TRANSMIT_BIT)) { // only when UART DMA operation is connected and channel enabled
            int *ptrCnt = (int *)argv[1];                                // the maximum number of characters that can be sent in a TICK period
            if (*ptrCnt != 0) {
                ptrDmaControl = (DMA_CONTROL_STRUCTURE *)((CAST_POINTER_ARITHMETIC)&vector_ram + (DMA_UART1_TRANSMIT_CH*sizeof(DMA_CONTROL_STRUCTURE)));
                iMasks |= fnHandleTxUART_DMA(ptrDmaControl, 1, ptrCnt);
                ulNewActions |= SEND_COM_1;                              // inform UART simulator that data should be sent
                if (!(iMasks & DMA_UART1_TRANSMIT_BIT)) {                // if DMA transmission has completed
                    iDMA &= ~DMA_UART1_TRANSMIT_BIT;
                    DMAENASET &= ~DMA_UART1_TRANSMIT_BIT;                // DMA disabled on this channel
                    fnUART_Tx_int(1);
                }
            }
            else {
                iMasks |= DMA_UART0_TRANSMIT_BIT;
            }
        }
        else {
            iDMA &= ~DMA_UART1_TRANSMIT_BIT;
        }
    }
    #endif
    #if CHIP_HAS_UARTS > 2
    if ((iDMA & DMA_ALT_UART2_TRANSMIT_BIT_2) != 0) {                    // UART 2 Tx DMA is active
        if ((UARTDMACTL_2 & TXDMAE) && (DMAENASET & DMA_ALT_UART2_TRANSMIT_BIT_2)) { // only when UART DMA operation is connected and channel enabled
            int *ptrCnt = (int *)argv[2];                                // the maximum number of characters that can be sent in a TICK period
            if (*ptrCnt != 0) {
                ptrDmaControl = (DMA_CONTROL_STRUCTURE *)((CAST_POINTER_ARITHMETIC)&vector_ram + (DMA_ALT_UART2_TRANSMIT_CH_2*sizeof(DMA_CONTROL_STRUCTURE)));
                iMasks |= fnHandleTxUART_DMA(ptrDmaControl, 2, ptrCnt);
                ulNewActions |= SEND_COM_2;                              // inform UART simulator that data should be sent
                if (!(iMasks & DMA_ALT_UART2_TRANSMIT_BIT_2)) {          // if DMA transmission has completed
                    iDMA &= ~DMA_ALT_UART2_TRANSMIT_BIT_2;
                    DMAENASET &= ~DMA_ALT_UART2_TRANSMIT_BIT_2;          // DMA disabled on this channel
                    fnUART_Tx_int(2);
                }
            }
            else {
                iMasks |= DMA_UART0_TRANSMIT_BIT;
            }
        }
        else {
            iDMA &= ~DMA_ALT_UART2_TRANSMIT_BIT_2;
        }
    }
    #endif
#endif
    return ulNewActions;
}



extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState) // {14}
{
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect
#if defined SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);
    switch (iPort) {
    #ifdef CTS_0_PIN
    case 0:
        if (ulChange & MS_CTS_ON) {                                      // CTS change - set the state in the corresponding register and generate interrupt, if enabled
            unsigned char ucPortBit = CTS_0_PIN;
            unsigned char ucInput = 7;
            while (!(ucPortBit & 0x01)) {
                ucInput--;
                ucPortBit >>= 1;
            }
            if (ulNewState & MS_CTS_ON) {                                // CTS has just been activated
                fnSimulateInputChange(CTS_0_PORT, ucInput, CLEAR_INPUT);
            }
            else {                                                       // CTS has just been deactivated
                fnSimulateInputChange(CTS_0_PORT, ucInput, SET_INPUT);
            }
        }
        break;
    #endif
    #ifdef CTS_1_PIN
    case 1:
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
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {21}
    default:
        if (ulChange & MS_CTS_ON) {
            extern int fnExt_CTS_change(int iUART, int iChange);
            if (fnExt_CTS_change((iPort - NUMBER_SERIAL), ((ulNewState & MS_CTS_ON) != 0)) != 0) {
        #if NUMBER_EXTERNAL_SERIAL > 2
                if ((iPort - NUMBER_SERIAL) >= 2) {
                    fnSimulateInputChange(EXT_UART_2_3_INT_PORT, (7 - EXT_UART_2_3_INT_BIT), TOGGLE_INPUT_NEG);
                }
                else {
                    fnSimulateInputChange(EXT_UART_0_1_INT_PORT, (7 - EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG);
                }
        #else
                fnSimulateInputChange(EXT_UART_0_1_INT_PORT, (7 - EXT_UART_0_1_INT_BIT), TOGGLE_INPUT_NEG);
        #endif
            }
        }
        break;
    #endif
    }
#endif
}

// Tick - no longer used since simulated via timer simulation
//
extern void RealTimeInterrupt(void)
{
}

#if defined I2C_INTERFACE
extern void fnSimulateI2C(int iPort, unsigned char *ptrDebugIn, unsigned short usLen, int iRepeatedStart)
{
    _EXCEPTION("To do!!!");
}
#endif


// simulate buffers for Ethernet use 
//
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

unsigned char  ucRxFIFO[2*1024] = {0};
unsigned char *ptrFIFO_put = ucRxFIFO;
unsigned char *ptrFIFO_get = ucRxFIFO;
unsigned short usBytesInFIFO = 0;
unsigned short usFrameSizes[31] = {0};
unsigned int   iFrameSizePut = 0;
unsigned int   iFrameSizeGet = 0;
static unsigned short usLastFrameLength = 0;

static int fnPutFifo(unsigned char ucByte)
{
    usLastFrameLength++;                                                 // count the number of bytes being added to this frame

    if (usBytesInFIFO >= sizeof(ucRxFIFO)) {
        MACRIS_IACK |= FOV;
        ptrFIFO_put -= usLastFrameLength;
        if (ptrFIFO_put < ucRxFIFO) {
            ptrFIFO_put = &ucRxFIFO[sizeof(ucRxFIFO)] - (ucRxFIFO - ptrFIFO_put);
        }
        usBytesInFIFO -= usLastFrameLength;                              // flush bad frame
        usLastFrameLength = 0;
        return 0;                                                        // fifo is full - overrun
    }
    ++usBytesInFIFO;
    *ptrFIFO_put = ucByte;
    if (++ptrFIFO_put >= &ucRxFIFO[sizeof(ucRxFIFO)]) {                  // wrap around
        ptrFIFO_put = ucRxFIFO;
    }
    return 1;                                                            // OK
}

unsigned short fnLastFrameLength(void)
{
    unsigned short usReturn = usLastFrameLength;
    usLastFrameLength = 0;                                               // reset to count next frame length
    return usReturn;
}

static unsigned char fnGetFifo_byte(void)
{
    if (ptrFIFO_get >= &ucRxFIFO[sizeof(ucRxFIFO)]) {                    // wrap around
        ptrFIFO_get = ucRxFIFO;
    }
    if (usBytesInFIFO == 0) {                                            // nothing in FIFO
        return *ptrFIFO_get;
    }
    usBytesInFIFO--;
    if (--usFrameSizes[iFrameSizeGet] == 0) {
        (MACNP)--;                                                       // one less in fifo
        if (++iFrameSizeGet >= 31) {
            iFrameSizeGet = 0;
        }

    }
    return *ptrFIFO_get++;
}

// This routine reads 4 bytes from the FIFO and returns them as a long word. The use of a subroutine ensures the order of reading.
//
extern unsigned long fnGetFifo_long(void)                                // {5}
{
	unsigned long ulValue = fnGetFifo_byte();
	ulValue |= (fnGetFifo_byte() << 8);
	ulValue |= (fnGetFifo_byte() << 16);
	ulValue |= (fnGetFifo_byte() << 24);
	return ulValue;
}
#endif

extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce)
{
#ifdef ETH_INTERFACE                                                     // we feed frames in promiscuous mode and filter them according to their details
    unsigned short usFrameLength = 0;

    if (!(MACRCTL & MAC_RXEN)) {
        return 0;                                                        // receive not enabled
    }

    if ((!iForce) && (!(fnCheckEthernetMode(ucData, usLen)))) { 
        return 0;                                                        // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
    }

    usLen += 2;                                                          // include length field
    if (!(fnPutFifo((unsigned char)usLen))) {                            // put the frame length into the FIFO (LSB)
        return 0;                                                        // overrun
    }

    if (!(fnPutFifo((unsigned char)(usLen>>8)))) {                       // put the frame length into the FIFO (MSB)
        return 0;                                                        // overrun
    }
    usLen -= 2;                                                          // remove length field

    while (usLen--) {                                                    // put the received frame into the receive FIFO
        if (!(fnPutFifo(*ucData++))) {
            return 0;                                                    // overrun
        }
    }

    if (!(MACRCTL & MAC_BADCRC)) {
        usLen = 4;
        while (usLen--) {                                       
            if (!(fnPutFifo(0x55))) {                                    // put the frame length into the FIFO (LSB)
                return 0;                                                // overrun
            }
        }
    }
    usFrameSizes[iFrameSizePut] = fnLastFrameLength();
    if (++iFrameSizePut >= 31) {
        iFrameSizePut = 0;
    }
    (MACNP)++;                                                           // there is another frame in the rx FIFO
    MACRIS_IACK |= RXINT;                                                // set rx frame interrupt flag
    EMAC_Interrupt();                                                    // simulate frame received
    return 1;
#else
    return 0;
#endif
}

#if defined USB_INTERFACE

// Handle data sent by USB
//
extern void fnSimUSB(int iType, int iEndpoint, USB_HW *ptrUSB_HW)
{
    extern void fnChangeUSBState(int iNewState);
    switch (iType) {
    case USB_SIM_RESET:
        {
            int x;
            USBFADDR = 0;
            fnChangeUSBState(0);
            for (x = 0; x < NUMBER_OF_USB_ENDPOINTS; x++) {
                fnLogUSB(x, 0, (unsigned char *)0xffffffff, 0);          // log reset on each endpoint
            }
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
        switch (iEndpoint) {
        case 0:
            if (!(USBCSRL0 & EP0_DEVICE_TXRDY)) {                        // no data prepared so ignore
                return;
            }
            break;
        case 1:
            if (!(USBTXCSRL1 & DEVICE_TXRDY)) {                          // no data prepared so ignore
                return;
            }
            break;
        case 2:
            if (!(USBTXCSRL2 & DEVICE_TXRDY)) {                          // no data prepared so ignore
                return;
            }
            break;
        case 3:
            if (!(USBTXCSRL3 & DEVICE_TXRDY)) {                          // no data prepared so ignore
                return;
            }
            break;
        default:
            return;
        }
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


static unsigned short fnGetSize(unsigned short usSetting, int iDirection)
{
    if (iDirection == 0) {
        switch (usSetting) {
        case 0:
            return 8;
        case 1:
            return 16;
        case 2:
            return 32;
        case 3:
            return 64;
        case 4:
            return 128;
        case 5:
            return 256;
        case 6:
            return 512;
        case 7:
            return 1024;
        case 8:
            return 2048;
        default:
            return 0;
        }
    }
    else {
        switch (usSetting) {
        case 8:
            return 0;
        case 16:
            return 1;
        case 32:
            return 2;
        case 64:
            return 3;
        case 128:
            return 4;
        case 256:
            return 5;
        case 512:
            return 6;
        case 1024:
            return 7;
        case 2048:
            return 8;
        default:
            return 0;
        }
    }
}

// Log the FIFO use by endpoint 
//
extern void fnPutUSB_FIFO_config(void)
{
    USBEPIDX &= 0x000f;
    if (USBEPIDX >= NUMBER_OF_USB_ENDPOINTS) {
        return;                                                          // invalid index
    }
    USB_FIFOs[USBEPIDX].tx_add  = (unsigned short)((USBTXFIFOADD & 0x1fff)*8);
    USB_FIFOs[USBEPIDX].tx_size = fnGetSize((unsigned char)(USBTXFIFOSZ & 0x0f), 0);
    USB_FIFOs[USBEPIDX].rx_add  = (unsigned short)((USBRXFIFOADD & 0x1fff)*8);
    USB_FIFOs[USBEPIDX].rx_size = fnGetSize((unsigned char)(USBRXFIFOSZ & 0x0f), 0);
    if (USBTXFIFOSZ & FIFO_DOUBLE_BUFFERED) {
        USB_FIFOs[USBEPIDX].iTxDoubleBuffered = 1;
    }
    else {
        USB_FIFOs[USBEPIDX].iTxDoubleBuffered = 0;
    }
    if (USBRXFIFOSZ & FIFO_DOUBLE_BUFFERED) {
        USB_FIFOs[USBEPIDX].iRxDoubleBuffered = 1;
    }
    else {
        USB_FIFOs[USBEPIDX].iRxDoubleBuffered = 0;
    }
}

// Return the index register contents
//
extern void fnGetUSB_FIFO_config(void)
{
    USBEPIDX &= 0x000f;
    if (USBEPIDX >= NUMBER_OF_USB_ENDPOINTS) {
        return;                                                          // invalid index
    }
    USBTXFIFOADD = (USB_FIFOs[USBEPIDX].tx_add / 8);
    USBTXFIFOSZ  =  (unsigned char)fnGetSize(USB_FIFOs[USBEPIDX].tx_size, 1);
    USBRXFIFOADD = (USB_FIFOs[USBEPIDX].rx_add / 8);
    USBRXFIFOSZ  =  (unsigned char)fnGetSize(USB_FIFOs[USBEPIDX].rx_size, 1);
    if (USB_FIFOs[USBEPIDX].iTxDoubleBuffered != 0) {
        USBTXFIFOSZ |= FIFO_DOUBLE_BUFFERED; 
    }
    if (USB_FIFOs[USBEPIDX].iRxDoubleBuffered != 0) {
        USBRXFIFOSZ |= FIFO_DOUBLE_BUFFERED; 
    }
}

extern volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned long *ptrAdd, int iDirection)
{
    volatile unsigned char *ptrFIFO;
    if (iDirection != 0) {                                               // transmission buffer
        if (ptrAdd == USBFIFO0_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[0].tx_add]);
        }
        if (ptrAdd == USBFIFO1_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[1].tx_add]);
        }
        if (ptrAdd == USBFIFO2_ADD) {
            ptrFIFO = (volatile unsigned char *)&ulFIFO[USB_FIFOs[2].tx_add]; // first buffer
            if (USB_FIFOs[2].iTxDoubleBuffered == 2) {                   // if second buffer is used return a simulation pointer to it
                ptrFIFO += USB_FIFOs[2].tx_size;                         // move to second buffer
            }
            return ptrFIFO;
        }
        if (ptrAdd == USBFIFO3_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[3].tx_add]);
        }
    }
    else {                                                               // reception
        if (ptrAdd == USBFIFO0_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[0].rx_add]);
        }
        if (ptrAdd == USBFIFO1_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[1].rx_add]);
        }
        if (ptrAdd == USBFIFO2_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[2].rx_add]);
        }
        if (ptrAdd == USBFIFO3_ADD) {
            return ((volatile unsigned char *)&ulFIFO[USB_FIFOs[3].rx_add]);
        }
    }
    return (volatile unsigned char *)ptrAdd;
}

extern void fnPutUSB_FIFOLength(int iEndpoint, unsigned short usLength)
{
    if (USB_FIFOs[iEndpoint].iTxDoubleBuffered != 0) {
        USB_FIFOs[iEndpoint].last_tx[(USB_FIFOs[iEndpoint].iTxDoubleBuffered - 1)] = usLength;
        USB_FIFOs[iEndpoint].iTxDoubleBuffered++;
        if (USB_FIFOs[iEndpoint].iTxDoubleBuffered > USB_FIFO_BUFFER_DEPTH) {
            USB_FIFOs[iEndpoint].iTxDoubleBuffered = 1;
        }
    }
    else {
        USB_FIFOs[iEndpoint].last_tx[0] = usLength;
    }
}

// Transmission has just been started on an endpoint.
// If the endpoint is double buffered, handle the simulation of it here.
//
extern void fnFIFO_buffer_out(int iEndpoint)
{                                             
    switch (iEndpoint) {
    case 0:
        break;                                                           // no double buffer operation on endpoint 0
    case 2:
        if (USBTXCSRL2 & DEVICE_TXRDY) {                                 // transmission bit has been set
            if ((!(USBTXDPKTBUFDIS & (0x01 << iEndpoint))) && (USB_FIFOs[2].iTxDoubleBuffered != 0)) { // if double buffering has not been disabled on this endpoint
                if (USB_FIFOs[2].ucBuffered == 0) {                      // FIFOs presently empty
                    USBTXCSRL2 &= ~DEVICE_TXRDY;                         // immediately reset the flag and generate an interrupt
                    USBTXCSRL2 |= DEVICE_FIFONE;
                    USB_FIFOs[2].ucBuffered = 1;
                }
                else if (USB_FIFOs[2].ucBuffered == 1) {                 // one buffer is already in use
                    USB_FIFOs[2].ucBuffered = 2;
                }
                iInts |= USB_INT;                                        // flag that the interrupt should be handled
                ulEndpointInt |= (1 << iEndpoint);                       // flag endpoint
                                                                         // the device would also set the interrupt here, but this is done in the simulator handler due to the control flags
            }
        }
        break;
    }
}

static int iNextZeroExpected[NUMBER_OF_USB_ENDPOINTS] = {0};

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
    if ((!(RCGC2 & CGC_USB0)) || (SRCR2 & CGC_USB0) || (RCC2 & USBPWRDWN)) { // check that the USB interface is operating
        return -1;                                                       // ignore since not in operation
    }
    if (ptrDebugIn == 0) {
        switch (usLenEvent) {
        case USB_RESET_CMD:
           /*memset(iTxDataToggle, 0, sizeof(iTxDataToggle));
            memset(iRxDataBank, 0, sizeof(iRxDataBank));
            memset(ulTxDataBank, 0, sizeof(ulTxDataBank));*/
            memset(&USB_FIFOs[1], 0, sizeof(USB_FIFOs[1]));
            memset(&USB_FIFOs[2], 0, sizeof(USB_FIFOs[2]));
            memset(&USB_FIFOs[3], 0, sizeof(USB_FIFOs[3]));
            memset(iNextZeroExpected, 0, sizeof(iNextZeroExpected));
            USBIS = RESET_INT;                                           // flag that a USB bus reset has been detected
            break;
        case USB_SLEEP_CMD:
            USBIS = SUSPEND_INT;
            break;
        case USB_RESUME_CMD:
            USBIS = RESUME_INT;
            break;
        case USB_STALL_SUCCESS:
        case USB_IN_SUCCESS:
            break;
        default:
            return 0;
        }
    }
    else {                                                               // data being injected
        if ((USBFADDR & 0x7f) != (unsigned char)iDevice) {               // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        switch (iEndPoint) {
        case 0:                                                          // endpoint 0 - control/bulk/interrupt - max endpoint size 8 bytes - not dual-banked
            memcpy(&ulFIFO[USB_FIFOs[0].rx_add], ptrDebugIn, usLenEvent);
            USBTXIS |= EP0_TX_RX_INT;
            if (usLenEvent == 0) {
                if (iNextZeroExpected[0] != 0) {
                    USBCSRL0 &= ~(EP0_DATAEND | EP0_SETEND);             // expected so automatically clear 
                }
                else {
                    USBCSRL0 |= (EP0_SETEND);                            // zero terminator injected before device has sent last transmission
                }           
            }
            else {
                USBCSRL0 |= EP0_DEVICE_RXRDY;
            }
            iNextZeroExpected[0] = 0;
            USBCOUNT0 = (unsigned char)usLenEvent;
            break;

        case 1:                                                          // endpoint 1 - control/bulk/interrupt - dual-banked
            memcpy(&ulFIFO[USB_FIFOs[1].rx_add], ptrDebugIn, usLenEvent);
            USBRXIS |= EP1_RX_INT;
            USBRXCSRL1 |= DEVICE_RXRDY;                                  // data ready
            USBRXCOUNT1 = (unsigned char)usLenEvent;
            break;
        }
    }
    if ((USBIE & USBIS) || (USBRXIS & USBRXIE) || (USBTXIS & USBTXIE)) { // if USB interrupts enabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
        IRQ32_63_SPR |= (1 << (irq_USB_ID - 32));
        IRQ32_63_CPR = IRQ32_63_SPR;
        if (IRQ32_63_SER & (1 << (irq_USB_ID - 32))) {                   // if USB interrupt is not disabled
            ptrVect->processor_interrupts.irq_USB_Controller();          // call the interrupt handler
        }
    }
    return 0;
}


extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    extern int iGetGetBank(int iEndpoint);

    int iBank = 0;
    switch (iEndpoint) {
    case 0:                                                              // endpoint 0
        while (USBCSRL0 & (EP0_DEVICE_TXRDY | EP0_STALL)) {              // transmission bit set
            unsigned short usType;
            if (EP0_STALL & USBCSRL0) {
                USBCSRL0 &= ~EP0_STALL;
                usType = USB_STALL_SUCCESS;
            }
            else {
                iTxDataToggle[0] ^= 1;                                   // automatic data toggle on transmission since it is automatically controlled by the chip
                fnLogUSB(0, USB_FIFOs[0].last_tx[0], (unsigned char *)&ulFIFO[USB_FIFOs[0].tx_add], iTxDataToggle[0]);
                USB_FIFOs[0].last_tx[0] = 0;
                if (USBCSRL0 & EP0_DATAEND) {
                    iNextZeroExpected[0] = 1;
                }
                usType = USB_IN_SUCCESS;
            }
            USBCSRL0 &= ~(EP0_DEVICE_TXRDY | EP0_DATAEND);               // transmission complete
            USBTXIS |= EP0_TX_RX_INT;                                    // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, usType);             // generate tx interrupt
        }
        break;

    case 2:                                                              // endpoint 2
        while (USBTXCSRL2 & (DEVICE_TXRDY | DEVICE_STALL | DEVICE_FIFONE)) { // transmission bit set
            unsigned short usType;
            int iSecondCall = 0;
            if (DEVICE_STALL & USBTXCSRL2) {
                USBTXCSRL2 &= ~DEVICE_STALL;
                usType = USB_STALL_SUCCESS;
                iSecondCall = 0;
            }
            else if (USBTXCSRL2 & (DEVICE_TXRDY | DEVICE_FIFONE)) {
                iTxDataToggle[2] ^= 1;                                   // automatic data toggle on transmission since it is automatically controlled by the chip
                if (USB_FIFOs[2].iTxDoubleBuffered != 0) {
                    fnLogUSB(2, USB_FIFOs[2].last_tx[USB_FIFOs[2].txGet], (unsigned char *)&ulFIFO[USB_FIFOs[2].tx_add + ((USB_FIFOs[2].txGet * USB_FIFOs[2].tx_size) / sizeof(long))], iTxDataToggle[2]);
                    USB_FIFOs[2].last_tx[USB_FIFOs[2].txGet] = 0;
                    USB_FIFOs[2].txGet++;
                    if (USB_FIFOs[2].txGet >= USB_FIFO_BUFFER_DEPTH) {
                        USB_FIFOs[2].txGet = 0;
                    }
                    if (USB_FIFOs[2].ucBuffered == 2) {
                        USB_FIFOs[2].ucBuffered = 1;
                        iSecondCall = 1;
                    }
                    else {
                        USB_FIFOs[2].ucBuffered = 0;
                        USBTXCSRL2 &= ~DEVICE_FIFONE;
                        iSecondCall = 0;
                    }
                }
                else {
                    fnLogUSB(2, USB_FIFOs[2].last_tx[0], (unsigned char *)&ulFIFO[USB_FIFOs[2].tx_add], iTxDataToggle[2]);
                    USB_FIFOs[2].last_tx[0] = 0;
                }
                usType = USB_IN_SUCCESS;
            }
            USBTXCSRL2 &= ~(DEVICE_TXRDY);                               // transmission complete
            USBTXIS |= EP2_TX_INT;                                       // mark that the transmission has been successfully acknowledged            
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, usType);             // generate tx interrupt
            if (iSecondCall != 0) {
                USBTXIS |= EP2_TX_INT;                                   // mark that the second transmission has been successfully acknowledged
                USB_FIFOs[2].ucBuffered--;
                iSecondCall = 0;
                fnSimulateUSB(iDevice, iEndpoint, 0, 0, usType);         // generate tx interrupt for second buffer
            }
        }
        break;                
    }
}
#endif

// We can update port state displays if we want
//
extern void fnSimPorts(void)
{
#if CHIP_HAS_UARTS > 0 && defined RTS_0_PIN                              // RTS outputs
    if ((RTS_0_PORT_POWER & RCGC2) && (RTS_0_PORT_ENABLE & RTS_0_PIN) && (RTS_0_PORT_DDR & RTS_0_PIN)) { // user defined RTS pin configured as output
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
#if CHIP_HAS_UARTS > 1 && defined RTS_1_PIN
    if ((RTS_1_PORT_POWER & RCGC2) && (RTS_1_PORT_ENABLE & RTS_1_PIN) && (RTS_1_PORT_DDR & RTS_1_PIN)) { // user defined RTS pin configured as output
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

                fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                  // assert control lines
            }
        }
    }
#endif
#if CHIP_HAS_UARTS > 2 && defined RTS_2_PIN
    if ((RTS_2_PORT_POWER & RCGC2) && (RTS_2_PORT_ENABLE & RTS_2_PIN) && (RTS_2_PORT_DDR & RTS_2_PIN)) { // user defined RTS pin configured as output
        static unsigned char ucRTS2 = 1;
        if (RTS_2_PORT & RTS_2_PIN) {
            if (ucRTS2 == 0) {
                ucRTS2 = 1;
                fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);                  // negate control lines
            }
        }
        else {
            if (ucRTS2 != 0) {
                ucRTS2 = 0;
                fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);                  // assert control lines
            }
        }
    }
#endif
}

static unsigned char ucGetPortType(int portNr, int iRequest, int i)
{
    unsigned char ucPeripherals = 0;
    unsigned char ucBit = 0x01;

    switch (portNr) {
    case _PORTA:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
    case _PORTB:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
    case _PORTC:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
    case _PORTD:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
    case _PORTE:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
    case _PORTF:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
    case _PORTG:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    case _PORTH:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    case _PORTJ:
        while (i < 8) {
            ucPeripherals = 0xff;
            i++;
        }
        break;
#endif
    }
    return ucPeripherals;
}

static void fnUpdatePeripherals(int iPort, unsigned char ucFunctions, LM3XXXXX_PORT_CONTROL *ptrPort)
{
    int i = 0;
    unsigned char ucBit = 0x01;
#ifdef _TEMPEST_CLASS
    unsigned long ulFunction = ptrPort->GPIOPCTL;
#endif

    while (i < 8) {
        if (ucFunctions & ucBit) {                                   // alternative function programmed
#ifdef _TEMPEST_CLASS
            ucPortFunctions[iPort][i] = (unsigned char)(ulFunction & 0x0f);
#else
            ucPortFunctions[iPort][i] = 1;
#endif
        }
        else {
            ucPortFunctions[iPort][i] = 0;
        }
        i++;
        ucBit <<= 1;
#ifdef _TEMPEST_CLASS
        ulFunction >>= 4;
#endif
    }
}


extern int fnPortChanges(int iForce)                                     // {8}
{
    #if (PORTS_AVAILABLE != _PORTS_AVAILABLE)                            // {9}
    static unsigned long ulLastRCGC0 = 0;
    #endif
    static unsigned long ucGPIOA = 0, ucGPIOB = 0, ucGPIOC = 0, ucGPIOD = 0, ucGPIOE = 0, ucGPIOF = 0, ucGPIOG = 0;
    static unsigned long ucPort_out_A = 0, ucPort_out_B = 0, ucPort_out_C = 0, ucPort_out_D = 0, ucPort_out_E = 0, ucPort_out_F = 0, ucPort_out_G = 0, ucPort_out_H = 0, ucPort_out_J = 0;
    static unsigned long ucGPIODIRA = 0, ucGPIODIRB = 0, ucGPIODIRC = 0, ucGPIODIRD = 0, ucGPIODIRE = 0, ucGPIODIRF = 0, ucGPIODIRG = 0, ucGPIODIRH = 0, ucGPIODIRJ = 0;
    int iRtn = iFlagRefresh;
    unsigned long ulOriginalInput;
#if (PART_DC4 & GPIOD_PRESENT4 | GPIOD_PRESENT4)
    unsigned long _reg;
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {23}
    static unsigned long ulPortExtValue[_EXTERNAL_PORT_COUNT] = { 0 };
    int iExPort = 0;
#endif
#if PART_DC4 & GPIOA_PRESENT4
    if ((GPIODIR_A != ucGPIODIRA) || (GPIODATA_A != ucPort_out_A)) {
        ucPort_out_A = GPIODATA_A;
        ucGPIODIRA = GPIODIR_A;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_A != GPIOAFSEL_A) {                                   // port function changed
        ucGPIO_per_A = GPIOAFSEL_A;
        fnUpdatePeripherals(_PORTA, (unsigned char)GPIOAFSEL_A, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_A_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_A & ~GPIODIR_A);
    if ((ulOriginalInput | (GPIODATA_A & GPIODIR_A)) != GPIODATA_A) {    // {10}
  //if ((ucPort_in_A & ~GPIODIR_A) != (GPIODATA_A & GPIODIR_A)) {
        GPIODATA_A &= GPIODIR_A;
        GPIODATA_A |= (ucPort_in_A & ~GPIODIR_A);
        ucPort_in_A = (unsigned char)GPIODATA_A;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTA, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOB_PRESENT4
    if ((GPIODIR_B != ucGPIODIRB) || (GPIODATA_B != ucPort_out_B)) {
        ucPort_out_B = GPIODATA_B;
        ucGPIODIRB = GPIODIR_B;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_B != GPIOAFSEL_B) {                                   // port function changed
        unsigned long ulChanges = (ucGPIO_per_B ^ GPIOAFSEL_B);          // {17}
        ulChanges &= ~GPIOCR_B;
        GPIOAFSEL_B ^= ulChanges;                                        // revert any changes blocked by port protection
        ucGPIO_per_B = GPIOAFSEL_B;
        fnUpdatePeripherals(_PORTB, (unsigned char)GPIOAFSEL_B, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_B_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_B & ~GPIODIR_B);
  //if ((ucPort_in_B & ~GPIODIR_B) != (GPIODATA_B & GPIODIR_B)) {
    if ((ulOriginalInput | (GPIODATA_B & GPIODIR_B)) != GPIODATA_B) {    // {10}
        GPIODATA_B &= GPIODIR_B;
        GPIODATA_B |= (ucPort_in_B & ~GPIODIR_B);
        ucPort_in_B = (unsigned char)GPIODATA_B;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTB, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOC_PRESENT4
    if ((GPIODIR_C != ucGPIODIRC) || (GPIODATA_C != ucPort_out_C)) {
        unsigned long ulChanges = (ucGPIO_per_C ^ GPIOAFSEL_C);          // {17}
        ulChanges &= ~GPIOCR_C;
        GPIOAFSEL_C ^= ulChanges; 
        ucPort_out_C = GPIODATA_C;
        ucGPIODIRC = GPIODIR_C;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_C != GPIOAFSEL_C) {                                   // port function changed
        ucGPIO_per_C = GPIOAFSEL_C;
        fnUpdatePeripherals(_PORTC, (unsigned char)GPIOAFSEL_C, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_C_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_C & ~GPIODIR_C);
  //if ((ucPort_in_C & ~GPIODIR_C) != (GPIODATA_C & GPIODIR_C)) {
    if ((ulOriginalInput | (GPIODATA_C & GPIODIR_C)) != GPIODATA_C) {    // {10}
        GPIODATA_C &= GPIODIR_C;
        GPIODATA_C |= (ucPort_in_C & ~GPIODIR_C);
        ucPort_in_C = (unsigned char)GPIODATA_C;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTC, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOD_PRESENT4
    if ((GPIODIR_D != ucGPIODIRD) || (GPIODATA_D != ucPort_out_D)) {
        ucPort_out_D = GPIODATA_D;
        ucGPIODIRD = GPIODIR_D;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    #ifdef _DUST_DEVIL_CLASS
  //if (RCGC0 & CGC_SARADC0) {                                           // if ADC is enabled
        _reg = (GPIOAFSEL_D | GPIOAMSEL_D);
  //}
  //else {
  //    _reg = GPIOAFSEL_D;
  //}
    #else
    _reg = GPIOAFSEL_D;
    #endif
    if (ucGPIO_per_D != _reg) {                                          // port function changed
        ucGPIO_per_D = _reg;
        fnUpdatePeripherals(_PORTD, (unsigned char)(_reg), (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_D_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_D & ~GPIODIR_D);
    if ((ulOriginalInput | (GPIODATA_D & GPIODIR_D)) != GPIODATA_D) {    // {10}
  //if ((ucPort_in_D & ~GPIODIR_D) != (GPIODATA_D & GPIODIR_D)) {
        GPIODATA_D &= GPIODIR_D;
        GPIODATA_D |= (ucPort_in_D & ~GPIODIR_D);
        ucPort_in_D = (unsigned char)GPIODATA_D;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTD, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOE_PRESENT4
    if ((GPIODIR_E != ucGPIODIRE) || (GPIODATA_E != ucPort_out_E)) {
        ucPort_out_E = GPIODATA_E;
        ucGPIODIRE = GPIODIR_E;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    #ifdef _DUST_DEVIL_CLASS
  //if (RCGC0 & CGC_SARADC0) {                                           // if ADC is enabled
        _reg = (GPIOAFSEL_E | GPIOAMSEL_E);
  //}
  //else {
  //    _reg = GPIOAFSEL_E;
  //}
    #else
    _reg = GPIOAFSEL_E;
    #endif
    if (ucGPIO_per_E != _reg) {                                          // port function changed
        ucGPIO_per_E = _reg;
        fnUpdatePeripherals(_PORTE, (unsigned char)_reg, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_E_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_E & ~GPIODIR_E);
  //if ((ulOriginalInput) != (GPIODATA_E & GPIODIR_E)) {
    if ((ulOriginalInput | (GPIODATA_E & GPIODIR_E)) != GPIODATA_E) {    // {10}
        GPIODATA_E &= GPIODIR_E;
        GPIODATA_E |= (ucPort_in_E & ~GPIODIR_E);
        ucPort_in_E = (unsigned char)GPIODATA_E;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTE, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOF_PRESENT4
    if ((GPIODIR_F != ucGPIODIRF) || (GPIODATA_F != ucPort_out_F)) {
        ucPort_out_F = GPIODATA_F;
        ucGPIODIRF = GPIODIR_F;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_F != GPIOAFSEL_F) {                                   // port function changed
        ucGPIO_per_F = GPIOAFSEL_F;
        fnUpdatePeripherals(_PORTF, (unsigned char)GPIOAFSEL_F, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_F_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_F & ~GPIODIR_F);
    if ((ulOriginalInput | (GPIODATA_F & GPIODIR_F)) != GPIODATA_F) {    // {10}
  //if ((ucPort_in_F & ~GPIODIR_F) != (GPIODATA_F & GPIODIR_F)) {
        GPIODATA_F &= GPIODIR_F;
        GPIODATA_F |= (ucPort_in_F & ~GPIODIR_F);
        ucPort_in_F = (unsigned char)GPIODATA_F;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTF, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOG_PRESENT4
    if ((GPIODIR_G != ucGPIODIRG) || (GPIODATA_G != ucPort_out_G)) {
        ucPort_out_G = GPIODATA_G;
        ucGPIODIRG = GPIODIR_G;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_G != GPIOAFSEL_G) {                                   // port function changed
        ucGPIO_per_G = GPIOAFSEL_G;
        fnUpdatePeripherals(_PORTG, (unsigned char)GPIOAFSEL_G, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_G_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_G & ~GPIODIR_G);
  //if ((ucPort_in_G & ~GPIODIR_G) != (GPIODATA_G & GPIODIR_G)) {
    if ((ulOriginalInput | (GPIODATA_G & GPIODIR_G)) != GPIODATA_G) {    // {10}
        GPIODATA_G &= GPIODIR_G;
        GPIODATA_G |= (ucPort_in_G & ~GPIODIR_G);
        ucPort_in_G = (unsigned char)GPIODATA_G;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTG, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    if ((GPIODIR_H != ucGPIODIRH) || (GPIODATA_H != ucPort_out_H)) {
        ucPort_out_H = GPIODATA_H;
        ucGPIODIRH = GPIODIR_H;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_H != GPIOAFSEL_H) {                                   // port function changed
        ucGPIO_per_H = GPIOAFSEL_H;
        fnUpdatePeripherals(_PORTH, (unsigned char)GPIOAFSEL_H, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_H_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_H & ~GPIODIR_H);
  //if ((ucPort_in_H & ~GPIODIR_H) != (GPIODATA_H & GPIODIR_H)) {
    if ((ulOriginalInput | (GPIODATA_H & GPIODIR_H)) != GPIODATA_H) {    // {10}
        GPIODATA_H &= GPIODIR_H;
        GPIODATA_H |= (ucPort_in_H & ~GPIODIR_H);
        ucPort_in_H = (unsigned char)GPIODATA_H;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTH, (unsigned char)ulOriginalInput);
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    if ((GPIODIR_J != ucGPIODIRJ) || (GPIODATA_J != ucPort_out_J)) {
        ucPort_out_J = GPIODATA_J;
        ucGPIODIRJ = GPIODIR_J;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    if (ucGPIO_per_J != GPIOAFSEL_J) {                                   // port function changed
        ucGPIO_per_J = GPIOAFSEL_J;
        fnUpdatePeripherals(_PORTJ, (unsigned char)GPIOAFSEL_J, (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_J_BLOCK + 0x03fc));
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    ulOriginalInput = (ucPort_in_J & ~GPIODIR_J);
  //if ((ucPort_in_J & ~GPIODIR_J) != (GPIODATA_J & GPIODIR_J)) {
    if ((ulOriginalInput | (GPIODATA_J & GPIODIR_J)) != GPIODATA_J) {    // {10}
        GPIODATA_J &= GPIODIR_J;
        GPIODATA_J |= (ucPort_in_J & ~GPIODIR_J);
        ucPort_in_J = (unsigned char)GPIODATA_J;
        iRtn |= PORT_CHANGE;                                             // port changed so redraw
    }
    fnCheckPortInterrupts(_PORTJ, (unsigned char)ulOriginalInput);
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {23}
    while (iExPort < _EXTERNAL_PORT_COUNT) {                             // external ports extensions
        unsigned long ulNewValue = fnGetPresentPortState(iExPort + (_PORT_EXT_0 + 1));
        if (ulNewValue != ulPortExtValue[iExPort]) {
            ulPortExtValue[iExPort] = ulNewValue;
            iRtn |= PORT_CHANGE;
        }
        iExPort++;
    }
#endif
#if (PORTS_AVAILABLE != _PORTS_AVAILABLE)                                // {9} dedicated ADC port
    if ((RCGC0 & CGC_SARADC0) != ulLastRCGC0) {
        int iADC = 0;
        ulLastRCGC0 = (RCGC0 & CGC_SARADC0);
        if (ulLastRCGC0 != 0) {                                          // ADC clocked, therefore all inputs active
            while (iADC < ADC_CHANNELS) {
                ucPortFunctions[PORTS_AVAILABLE-1][iADC++] = 1;
            }
        }
        else {
            while (iADC < ADC_CHANNELS) {
                ucPortFunctions[PORTS_AVAILABLE-1][iADC++] = 0;
            }
        }
    }
#endif
    if (iForce != 0) {                                                   // {12} queue refreshes if forced
        iFlagRefresh = iRtn;
    }
    return iRtn;
}

extern unsigned long fnGetPresentPortState(int portNr)
{
    portNr--;
    switch (portNr) {
#if PART_DC4 & GPIOA_PRESENT4
    case _PORTA:
    if (((RCGC2 & CGC_GPIOA) == 0) || ((SRCR2 & CGC_GPIOA) != 0)) {
        return ucPort_in_A;                                              // not clocked or in reset
    }
    return ((GPIODATA_A & GPIODEN_A) | (~GPIODEN_A & ucPort_in_A));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOB_PRESENT4
    case _PORTB:
    if (((RCGC2 & CGC_GPIOB) == 0) || ((SRCR2 & CGC_GPIOB) != 0)) {
        return ucPort_in_B;                                              // not clocked or in reset
    }
    return ((GPIODATA_B & GPIODEN_B) | (~GPIODEN_B & ucPort_in_B));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOC_PRESENT4                               
    case _PORTC:
    if (((RCGC2 & CGC_GPIOC) == 0) || ((SRCR2 & CGC_GPIOC) != 0)) {
        return ucPort_in_C;                                              // not clocked or in reset
    }
    return ((GPIODATA_C & GPIODEN_C) | (~GPIODEN_C & ucPort_in_C));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOC_PRESENT4    
    case _PORTD:
    if (((RCGC2 & CGC_GPIOD) == 0) || ((SRCR2 & CGC_GPIOD) != 0)) {
        return ucPort_in_D;                                              // not clocked or in reset
    }
    return ((GPIODATA_D & GPIODEN_D) | (~GPIODEN_D & ucPort_in_D));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOE_PRESENT4
    case _PORTE:
    if (((RCGC2 & CGC_GPIOE) == 0) || ((SRCR2 & CGC_GPIOE) != 0)) {
        return ucPort_in_E;                                              // not clocked or in reset
    }
    return ((GPIODATA_E & GPIODEN_E) | (~GPIODEN_E & ucPort_in_E));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOF_PRESENT4
    case _PORTF:
    if (((RCGC2 & CGC_GPIOF) == 0) || ((SRCR2 & CGC_GPIOF) != 0)) {
        return ucPort_in_F;                                              // not clocked or in reset
    }
    return ((GPIODATA_F & GPIODEN_F) | (~GPIODEN_F & ucPort_in_F));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOG_PRESENT4
    case _PORTG:
    if (((RCGC2 & CGC_GPIOG) == 0) || ((SRCR2 & CGC_GPIOG) != 0)) {
        return ucPort_in_G;                                              // not clocked or in reset
    }
    return ((GPIODATA_G & GPIODEN_G) | (~GPIODEN_G & ucPort_in_G));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    case _PORTH:
    if (((RCGC2 & CGC_GPIOH) == 0) || ((SRCR2 & CGC_GPIOH) != 0)) {
        return ucPort_in_H;                                              // not clocked or in reset
    }
    return ((GPIODATA_H & GPIODEN_H) | (~GPIODEN_H & ucPort_in_H));      // only bits set to digital bits are driven
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    case _PORTJ:
    if (((RCGC2 & CGC_GPIOJ) == 0) || ((SRCR2 & CGC_GPIOJ) != 0)) {
        return ucPort_in_J;                                              // not clocked or in reset
    }
    return ((GPIODATA_J & GPIODEN_J) | (~GPIODEN_J & ucPort_in_J));      // only bits set to digital bits are driven
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {23}
    case _PORT_EXT_0:                                                    // external ports extensions
    case _PORT_EXT_1:
    case _PORT_EXT_2:
    case _PORT_EXT_3:
    case _PORT_EXT_4:
    case _PORT_EXT_5:
    case _PORT_EXT_6:
    case _PORT_EXT_7:
        return (fnGetExtPortState(portNr));                              // {8} pin states of external port
#endif
    default:
        return 0;
    }
}

extern unsigned long fnGetPresentPortDir(int portNr)
{
    switch (--portNr) {
#if PART_DC4 & GPIOA_PRESENT4
    case _PORTA:
    if ((!(RCGC2 & CGC_GPIOA)) || (SRCR2 & CGC_GPIOA)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_A & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOB_PRESENT4
    case _PORTB:
    if ((!(RCGC2 & CGC_GPIOB)) || (SRCR2 & CGC_GPIOB)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_B & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOC_PRESENT4
    case _PORTC:
    if ((!(RCGC2 & CGC_GPIOC)) || (SRCR2 & CGC_GPIOC)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_C & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOD_PRESENT4
    case _PORTD:
    if ((!(RCGC2 & CGC_GPIOD)) || (SRCR2 & CGC_GPIOD)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_D & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOE_PRESENT4
    case _PORTE:
    if ((!(RCGC2 & CGC_GPIOE)) || (SRCR2 & CGC_GPIOE)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_E & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOF_PRESENT4
    case _PORTF:
    if ((!(RCGC2 & CGC_GPIOF)) || (SRCR2 & CGC_GPIOF)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_F & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOG_PRESENT4
    case _PORTG:
    if ((!(RCGC2 & CGC_GPIOG)) || (SRCR2 & CGC_GPIOG)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_G & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    case _PORTH:
    if (((RCGC2 & CGC_GPIOH) == 0) || ((SRCR2 & CGC_GPIOH) != 0)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_H & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    case _PORTJ:
    if ((!(RCGC2 & CGC_GPIOJ)) || (SRCR2 & CGC_GPIOJ)) {
        return 0;                                                        // not clocked or in reset
    }
    return (GPIODIR_J & ucGetPortType(portNr, GET_OUTPUTS, 0));          // non outputs returned as zeros
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0
    case _PORT_EXT_0:                                                    // {23} external ports extensions
    case _PORT_EXT_1:
    case _PORT_EXT_2:
    case _PORT_EXT_3:
    case _PORT_EXT_4:
    case _PORT_EXT_5:
    case _PORT_EXT_6:
    case _PORT_EXT_7:
        return (fnGetExtPortDirection(portNr));
#endif
    default:
        return 0;
    }
}

extern unsigned long fnGetPresentPortPeriph(int portNr)
{
    portNr--;
    switch (portNr) {
#if PART_DC4 & GPIOA_PRESENT4
    case _PORTA:
        return GPIOAFSEL_A;
#endif
#if PART_DC4 & GPIOB_PRESENT4
    case _PORTB:
        return GPIOAFSEL_B;
#endif
#if PART_DC4 & GPIOC_PRESENT4
    case _PORTC:
        return GPIOAFSEL_C;
#endif
#if PART_DC4 & GPIOD_PRESENT4
    case _PORTD:
    #ifdef _DUST_DEVIL_CLASS
      //if (RCGC0 & CGC_SARADC0) {                                           // if ADC is enabled
            return (GPIOAFSEL_D | GPIOAMSEL_D);
      //}
      //else {
      //    return (GPIOAFSEL_D);
      //}
    #else
        return GPIOAFSEL_D;
    #endif
#endif
#if PART_DC4 & GPIOE_PRESENT4
    case _PORTE:
    #ifdef _DUST_DEVIL_CLASS
      //if (RCGC0 & CGC_SARADC0) {                                           // if ADC is enabled
            return (GPIOAFSEL_E | GPIOAMSEL_E);
      //}
      //else {
      //    return (GPIOAFSEL_E);
      //}
    #else
        return GPIOAFSEL_E;
    #endif
#endif
#if PART_DC4 & GPIOF_PRESENT4
    case _PORTF:
        return GPIOAFSEL_F;
#endif
#if PART_DC4 & GPIOG_PRESENT4
    case _PORTG:
        return GPIOAFSEL_G;
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    case _PORTH:
        return GPIOAFSEL_H;
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    case _PORTJ:
        return GPIOAFSEL_J;
#endif
#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) && (PORTS_AVAILABLE != _PORTS_AVAILABLE)
    case (PORTS_AVAILABLE):                                              // dedicated ADC port
        if ((RCGC0 & CGC_SARADC0) != 0) {
            return (0xffffffff);                                         // all ADC are active
        }
        else {
            return 0;                                                    // no ADC are active
        }
        break;
#endif
    }
    return 0;
}


#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)

static unsigned long ulADC_fifo[2][4][8] = {0};

extern unsigned long fnGetADCfifo(int adc, int seq)
{
    unsigned long ulPut = ((ADCSSFSTAT0_0 & HEAD_POINTER_MASK) >> 4);
    unsigned long ulGet = (ADCSSFSTAT0_0 & TAIL_POINTER_MASK);
    unsigned long ulSample = ulADC_fifo[adc][seq][ulGet];
    ADCSSFSTAT0_0 &= ~FIFO_FULL;
    if (++ulGet >= 8) {
        ulGet = 0;
    }
    if (ulGet == ulPut) {
        ADCSSFSTAT0_0 |= FIFO_EMPTY;                                     // if no more samples mark empty
    }
    ADCSSFSTAT0_0 = ((ADCSSFSTAT0_0 & ~TAIL_POINTER_MASK) | ulGet);      // update the tail pointer
    return ulSample;
}

static void fnPutADCfifo(int adc, int seq, unsigned long ulValue)
{
    unsigned long ulPut = ((ADCSSFSTAT0_0 & HEAD_POINTER_MASK) >> 4);
    unsigned long ulGet = (ADCSSFSTAT0_0 & TAIL_POINTER_MASK);
    ADCSSFSTAT0_0 &= ~FIFO_EMPTY;
    ulADC_fifo[adc][seq][ulPut++] = ulValue;                             // insert the sample in the FIFO
    if (ulPut >= 8) {
        ulPut = 0;
    }
    if (ulGet == ulPut) {
        ADCSSFSTAT0_0 |= FIFO_FULL;                                      // if no more space in FIFO mark full
    }
    ADCSSFSTAT0_0 = ((ADCSSFSTAT0_0 & ~HEAD_POINTER_MASK) | (ulPut << 4)); // update the head pointer
}

static void fnSampleChannel(int ADC, unsigned char ucChannel, unsigned long ulDifferential)
{
    if ((ulDifferential & DIF_SAMPLE_1) != 0) {
        unsigned long ulPositiveInput = ulADC_values[2*ucChannel];
        unsigned long ulNegativeInput = ulADC_values[(2*ucChannel) + 1];
        unsigned long ulDiffValue;
        if (ulPositiveInput >= ulNegativeInput) {
            ulDiffValue = (ulPositiveInput - ulNegativeInput);
            if (ulDiffValue > (ADC_FULL_SCALE/2)) {
                ulDiffValue = (ADC_FULL_SCALE/2);
            }
            ulDiffValue = (ADC_FULL_SCALE/2) + ulDiffValue;
        }
        else {
            ulDiffValue = (ulNegativeInput - ulPositiveInput);
            if (ulDiffValue > (ADC_FULL_SCALE/2)) {
                ulDiffValue = (ADC_FULL_SCALE/2);
            }
            ulDiffValue = (ADC_FULL_SCALE/2) - ulDiffValue;
        }

        fnPutADCfifo(0, 0, ulDiffValue);                                 // put the differntial value into the ADC FIFO
    }
    else {
        if ((ulDifferential & TEMP_SAMPLE_1)) {                          // {14}
            fnPutADCfifo(0, 0, 0x200);                                   // put the internal temperature value into the ADC FIFO (35°C)
        }
        else {
    #if defined SUPPORT_TOUCH_SCREEN
    #endif
            fnPutADCfifo(0, 0, ulADC_values[ucChannel]);                 // put the present pin value into the ADC FIFO
        }
    }
}

static void fnADC_sequenceTriggered(int iSequencer)
{
    unsigned long ulSequence = ADCSSMUX0_0;
    unsigned long ulControl = ADCSSCTL0_0;
    unsigned long ulStop;
    if (!(RCGC0 & CGC_SARADC0)) {                                        // ADC not enabled
        return;
    }

    do {
        fnSampleChannel(0, (unsigned char)(ulSequence & 0xf), (ulControl));
        ulStop = (ulControl & END_SAMPLE_1);
        if (ulControl & INT_SAMPLE_1) {
            if (IRQ0_31_SER & (1 << irq_ADC_Sequence_0_ID)) {            // if ADC sequencer 0 interrupt is not disabled
                if (ADCIM_0 & MASK_SS0) {
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                    ptrVect->processor_interrupts.irq_ADC0_Sequence_0(); // call the interrupt handler
                }
            }
        }   
        ulSequence >>= 4;
        ulControl >>= 4;
    } while (ulStop == 0);
}

#endif

// This routine is called after an input change to check whether there has been a port interrupt generated
//
static void fnCheckPortInterrupts(unsigned char ucPort, unsigned char ucOriginalInput)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
    unsigned char ucBit = 0x80;
    int iInterrupt;
    while (ucBit != 0) {
        switch (ucPort) {
#if PART_DC4 & GPIOA_PRESENT4
        case _PORTA:
            if (!(GPIOIM_A & ucBit)) {
                if (!GPIOIM_A) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_A & ucBit) {                                      // level sensitive
                if (GPIOIEV_A & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_A) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_A) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_A & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_A ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_A & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_A & (ucPort_in_A ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_A & (ucPort_in_A ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_A |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_A |= (ucBit & GPIOIM_A);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_A();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOB_PRESENT4
        case _PORTB:
            if (!(GPIOIM_B & ucBit)) {
                if (!GPIOIM_B) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_B & ucBit) {                                      // level sensitive
                if (GPIOIEV_B & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_B) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_B) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_B & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_B ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_B & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_B & (ucPort_in_B ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_B & (ucPort_in_B ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_B |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_B |= (ucBit & GPIOIM_B);                         // set the masked interrupt status
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                if (ucBit & PORTB_BIT4) {                                // check possible ADC trigger
                    if ((ADCEMUX_0 & SS0_MASK) == TRIG_GPIO_PB4) {       // SS0 set up to be triggered on PB4 interrupt (even if masked to code)
                        fnADC_sequenceTriggered(0);
                    }
                }
    #endif
                if (IRQ0_31_SER & (0x1 << irq_GPIO_Port_B_ID)) {
                    ptrVect->processor_interrupts.irq_GPIO_Port_B();     // call the interrupt handler
                }
            }
            break;
#endif
#if PART_DC4 & GPIOC_PRESENT4
        case _PORTC:
            if (!(GPIOIM_C & ucBit)) {
                if (!GPIOIM_C) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_C & ucBit) {                                      // level sensitive
                if (GPIOIEV_C & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_C) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_C) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_C & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_C ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_C & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_C & (ucPort_in_C ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_C & (ucPort_in_C ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_C |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_C |= (ucBit & GPIOIM_C);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_C();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOD_PRESENT4
        case _PORTD:
            if (!(GPIOIM_D & ucBit)) {
                if (!GPIOIM_D) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_D & ucBit) {                                      // level sensitive
                if (GPIOIEV_D & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_D) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_D) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_D & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_D ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_D & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_D & (ucPort_in_D ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_D & (ucPort_in_D ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_D |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_D |= (ucBit & GPIOIM_D);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_D();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOE_PRESENT4
        case _PORTE:
            if (!(GPIOIM_E & ucBit)) {
                if (!GPIOIM_E) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_E & ucBit) {                                      // level sensitive
                if (GPIOIEV_E & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_E) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_E) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_E & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_E ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_E & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_E & (ucPort_in_E ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_E & (ucPort_in_E ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_E |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_E |= (ucBit & GPIOIM_E);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_E();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOF_PRESENT4
        case _PORTF:
            if (!(GPIOIM_F & ucBit)) {
                if (!GPIOIM_F) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_F & ucBit) {                                      // level sensitive
                if (GPIOIEV_F & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_F) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_F) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_F & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_F ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_F & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_F & (ucPort_in_F ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_F & (ucPort_in_F ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_F |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_F |= (ucBit & GPIOIM_F);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_F();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOG_PRESENT4
        case _PORTG:
            if (!(GPIOIM_G & ucBit)) {
                if (!GPIOIM_G) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_G & ucBit) {                                      // level sensitive
                if (GPIOIEV_G & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_G) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_G) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_G & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_G ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_G & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_G & (ucPort_in_G ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_G & (ucPort_in_G ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_G |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_G |= (ucBit & GPIOIM_G);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_G();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
         case _PORTH:
            if (!(GPIOIM_H & ucBit)) {
                if (!GPIOIM_H) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_H & ucBit) {                                      // level sensitive
                if (GPIOIEV_H & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_H) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_H) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_H & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_H ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_H & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_H & (ucPort_in_H ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_H & (ucPort_in_H ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_H |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_H |= (ucBit & GPIOIM_H);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_H();         // call the interrupt handler
            }
            break;
#endif
#if PART_DC4 & GPIOJ_PRESENT4
         case _PORTJ:
            if (!(GPIOIM_J & ucBit)) {
                if (!GPIOIM_J) {                                         // no interrupts enabled on this prot
                    return;
                }
                else {
                    break;                                               // try next one
                }
            }
            iInterrupt = 0;
            if (GPIOIS_J & ucBit) {                                      // level sensitive
                if (GPIOIEV_J & ucBit) {                                 // high sensitive
                    if (ucBit & ucPort_in_J) {                           // high level detected
                        iInterrupt = 1;
                    }
                }
                else {                                                   // low sensitive
                    if (ucBit & ~ucPort_in_J) {                          // low level detected
                        iInterrupt = 1;
                    }
                }
            }
            else {                                                       // edge sensitive
                if (GPIOIBE_J & ucBit) {                                 // both edges enabled
                    if (ucBit & (ucPort_in_J ^ ucOriginalInput)) {
                        iInterrupt = 1;
                    }
                }
                else {
                    if (GPIOIEV_J & ucBit) {                             // rising edge
                        if (ucBit & ucPort_in_J & (ucPort_in_J ^ ucOriginalInput)) { // rising edge detected
                            iInterrupt = 1;
                        }
                    }
                    else {                                               // falling edge
                        if (ucBit & ~ucPort_in_J & (ucPort_in_J ^ ucOriginalInput)) { // falling edge detected
                            iInterrupt = 1;
                        }
                    }
                }
            }
            if (iInterrupt != 0) {
                GPIORIS_J |= ucBit;                                      // set RAW interrupt flag
                GPIOMIS_J |= (ucBit & GPIOIM_J);                         // set the masked interrupt status
                ptrVect->processor_interrupts.irq_GPIO_Port_J();         // call the interrupt handler
            }
            break;
#endif
        }
        ucBit >>= 1;
    }
}

#if defined HANDLE_EXT_PORT
    #if defined _EXT_PORT_32_BIT && PORT_WIDTH < 32
        #define BLANK_PROCESSOR_PORTS (32 - PORT_WIDTH)
    #elif defined _EXT_PORT_28_BIT && PORT_WIDTH < 28
        #define BLANK_PROCESSOR_PORTS (28 - PORT_WIDTH)
    #elif defined _EXT_PORT_16_BIT && PORT_WIDTH < 16
        #define BLANK_PROCESSOR_PORTS (16 - PORT_WIDTH)
    #else
        #define BLANK_PROCESSOR_PORTS 0
    #endif
#endif
extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    unsigned char ucBit;
    unsigned char ucOriginalInput;
#if BLANK_PROCESSOR_PORTS != 0
    unsigned long ulBit = (0x01 << ((BLANK_PROCESSOR_PORTS + 7) - ucPortBit));
#endif
    ucPortBit = (7 - ucPortBit);
    ucBit = (0x01 << ucPortBit);

    switch (ucPort) {
#if PART_DC4 & GPIOA_PRESENT4
    case _PORTA:
        if (((RCGC2 & CGC_GPIOA) == 0) || ((SRCR2 & CGC_GPIOA) != 0)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_A & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_A;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_A ^= ucBit;                                        // toggle the input state
            GPIODATA_A ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_A |= ucBit;                                        // set the input high
            GPIODATA_A |= ucBit;
        }
        else {
            ucPort_in_A &= ~ucBit;                                       // set the input low
            GPIODATA_A &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTA, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOB_PRESENT4
    case _PORTB:
    #if defined SUPPORT_ADC && (PART_DC1 & ADC1_PRESENT1)                // {18}
        if (GPIOAMSEL_B & ucBit) {                                       // analogue pin isolation set
            if ((ucPortBit == 4) || (ucPortBit == 5)) {
                if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                    switch (ucPortBit) {
                    case 4:
                        ucPortBit = 10;
                        break;
                    case 5:
                        ucPortBit = 11;
                        break;
                    }
                    if (TOGGLE_INPUT_NEG & iChange) {
                        if (ulADC_values[ucPortBit] >= usStepSize) {
                            ulADC_values[ucPortBit] -= usStepSize;
                        }
                    }
                    else {
                        if ((ulADC_values[ucPortBit] + usStepSize) <= ADC_FULL_SCALE) {
                            ulADC_values[ucPortBit] += usStepSize;
                        }
                    }
                }
            }
            break;
        }
    #endif
        if ((!(RCGC2 & CGC_GPIOB)) || (SRCR2 & CGC_GPIOB)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_B & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_B;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_B ^= ucBit;                                        // toggle the input state
            GPIODATA_B ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_B |= ucBit;                                        // set the input high
            GPIODATA_B |= ucBit;
        }
        else {
            ucPort_in_B &= ~ucBit;                                       // set the input low
            GPIODATA_B &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTB, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOC_PRESENT4
    case _PORTC:
        if ((!(RCGC2 & CGC_GPIOC)) || (SRCR2 & CGC_GPIOC)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_C & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_C;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_C ^= ucBit;                                        // toggle the input state
            GPIODATA_C ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_C |= ucBit;                                        // set the input high
            GPIODATA_C |= ucBit;
        }
        else {
            ucPort_in_C &= ~ucBit;                                       // set the input low
            GPIODATA_C &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTC, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOD_PRESENT4
    case _PORTD:
    #if (defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS) && defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) // {18}
        if (GPIOAMSEL_D & ucBit) {                                       // analogue pin isolation set
        #if !(PART_DC1 & ADC1_PRESENT1)
            if (ucPortBit >= 4) {
        #endif
                if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                    switch (ucPortBit) {
                    case 4:
                        ucPortBit = 7;
                        break;
                    case 5:
                        ucPortBit = 6;
                        break;
                    case 6:
                        ucPortBit = 5;
                        break;
                    case 7:
                        ucPortBit = 4;
                        break;
        #if (PART_DC1 & ADC1_PRESENT1)                                   // {18}
                    case 0:
                        ucPortBit = 15;
                        break;
                    case 1:
                        ucPortBit = 14;
                        break;
                    case 2:
                        ucPortBit = 13;
                        break;
                    case 3:
                        ucPortBit = 12;
                        break;
        #endif
                    }
                    if ((TOGGLE_INPUT_NEG & iChange) != 0) {
                        if (ulADC_values[ucPortBit] >= usStepSize) {
                            ulADC_values[ucPortBit] -= usStepSize;
                        }
                    }
                    else {
                        if ((ulADC_values[ucPortBit] + usStepSize) <= ADC_FULL_SCALE) {
                            ulADC_values[ucPortBit] += usStepSize;
                        }
                    }
                }
        #if !(PART_DC1 & ADC1_PRESENT1)
            }
        #endif
            break;
        }
    #endif
        if ((!(RCGC2 & CGC_GPIOD)) || (SRCR2 & CGC_GPIOD)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_D & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_D;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_D ^= ucBit;                                        // toggle the input state
            GPIODATA_D ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_D |= ucBit;                                        // set the input high
            GPIODATA_D |= ucBit;
        }
        else {
            ucPort_in_D &= ~ucBit;                                       // set the input low
            GPIODATA_D &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTD, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOE_PRESENT4
    case _PORTE:
    #if (defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS) && defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) // {18}
        if (GPIOAMSEL_E & ucBit) {                                       // analogue pin isolation set
        #if (PART_DC1 & ADC1_PRESENT1)
            if (ucPortBit >= 2)
        #else
            if (ucPortBit >= 4)
        #endif
            {
                if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
                    switch (ucPortBit) {
                    case 4:
                        ucPortBit = 3;
                        break;
                    case 5:
                        ucPortBit = 2;
                        break;
                    case 6:
                        ucPortBit = 1;
                        break;
                    case 7:
                        ucPortBit = 0;
                        break;
        #if (PART_DC1 & ADC1_PRESENT1)                                   // {18}
                    case 2:
                        ucPortBit = 9;
                        break;
                    case 3:
                        ucPortBit = 8;
                        break;
        #endif
                    }
                    if (TOGGLE_INPUT_NEG & iChange) {
                        if (ulADC_values[ucPortBit] >= usStepSize) {
                            ulADC_values[ucPortBit] -= usStepSize;
                        }
                    }
                    else {
                        if ((ulADC_values[ucPortBit] + usStepSize) <= ADC_FULL_SCALE) {
                            ulADC_values[ucPortBit] += usStepSize;
                        }
                    }
                }
            }
            break;
        }
    #endif
        if ((!(RCGC2 & CGC_GPIOE)) || (SRCR2 & CGC_GPIOE)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_E & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_E;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_E ^= ucBit;                                        // toggle the input state
            GPIODATA_E ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_E |= ucBit;                                        // set the input high
            GPIODATA_E |= ucBit;
        }
        else {
            ucPort_in_E &= ~ucBit;                                       // set the input low
            GPIODATA_E &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTE, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOF_PRESENT4
    case _PORTF:
        if ((!(RCGC2 & CGC_GPIOF)) || (SRCR2 & CGC_GPIOF)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_F & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_F;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_F ^= ucBit;                                        // toggle the input state
            GPIODATA_F ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_F |= ucBit;                                        // set the input high
            GPIODATA_F |= ucBit;
        }
        else {
            ucPort_in_F &= ~ucBit;                                       // set the input low
            GPIODATA_F &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTF, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOG_PRESENT4
    case _PORTG:
        if ((!(RCGC2 & CGC_GPIOG)) || (SRCR2 & CGC_GPIOG)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_G & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_G;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_G ^= ucBit;                                        // toggle the input state
            GPIODATA_G ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_G |= ucBit;                                        // set the input high
            GPIODATA_G |= ucBit;
        }
        else {
            ucPort_in_G &= ~ucBit;                                       // set the input low
            GPIODATA_G &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTG, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOH_PRESENT4                                            // {3}
    case _PORTH:
        if ((!(RCGC2 & CGC_GPIOH)) || (SRCR2 & CGC_GPIOH)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_H & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_H;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_H ^= ucBit;                                        // toggle the input state
            GPIODATA_H ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_H |= ucBit;                                        // set the input high
            GPIODATA_H |= ucBit;
        }
        else {
            ucPort_in_H &= ~ucBit;                                       // set the input low
            GPIODATA_H &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTH, ucOriginalInput);
        break;
#endif
#if PART_DC4 & GPIOJ_PRESENT4
    case _PORTJ:
        if ((!(RCGC2 & CGC_GPIOJ)) || (SRCR2 & CGC_GPIOJ)) {
            return;                                                      // port not clocked or in reset
        }
        if (GPIODIR_J & (1 << ucPortBit)) {
            return;                                                      // pin not programmed as input
        }
        ucOriginalInput = ucPort_in_J;
        if (iChange == TOGGLE_INPUT) {
            ucPort_in_J ^= ucBit;                                        // toggle the input state
            GPIODATA_J ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_J |= ucBit;                                        // set the input high
            GPIODATA_J |= ucBit;
        }
        else {
            ucPort_in_J &= ~ucBit;                                       // set the input low
            GPIODATA_J &= ~ucBit;
        }
        fnCheckPortInterrupts(_PORTJ, ucOriginalInput);
        break;
#endif
#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) && (PORTS_AVAILABLE != _PORTS_AVAILABLE)
    case (PORTS_AVAILABLE):                                              // dedicated ADC port
        if ((iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG | TOGGLE_INPUT_POS | SET_INPUT)) != 0) {
            unsigned short usStepSize;
            if ((TOGGLE_INPUT_ANALOG & iChange) != 0) {
                usStepSize = (ADC_FULL_SCALE/3);
            }
            else {
                usStepSize = ((ADC_SIM_STEP_SIZE) / ADC_REFERENCE_VOLTAGE);
            }
            if ((TOGGLE_INPUT_NEG & iChange) != 0) {
                if (ulADC_values[ucPortBit] >= usStepSize) {
                    ulADC_values[ucPortBit] -= usStepSize;
                }
            }
            else {
                if ((ulADC_values[ucPortBit] + usStepSize) <= ADC_FULL_SCALE) {
                    ulADC_values[ucPortBit] += usStepSize;
                }
            }
            break;
        }
        break;
#endif
    case _PORT_EXT_0:                                                    // {23} external ports extensions
    case _PORT_EXT_1:
    case _PORT_EXT_2:
    case _PORT_EXT_3:
    case _PORT_EXT_4:
    case _PORT_EXT_5:
    case _PORT_EXT_6:
    case _PORT_EXT_7:
#if defined HANDLE_EXT_PORT
        HANDLE_EXT_PORT(ucPort, iChange, ulBit);
#endif
        break;
    }
}

#if defined SUPPORT_KEY_SCAN

#if KEY_COLUMNS == 0                                                     // {11}
    #define _NON_MATRIX
    #undef KEY_COLUMNS
    #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
    #undef KEY_ROWS
    #define KEY_ROWS VIRTUAL_KEY_ROWS
#endif


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
#if !defined _NON_MATRIX
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

    iFlagRefresh = fnPortChanges(1);                                     // {8} synchronise with present settings
    for (i = 0; i < KEY_COLUMNS; i++) {                                  // check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);
            }
        }
    }
}
#endif

extern void fnClearInt(void *reg, int iModule, unsigned long bit)        // {7}
{
    if (reg == GP_TIMER_0_BLOCK) {                                       // general purpose timers
        LM3Sxxxx.GPT[iModule].GPTMRIS &= ~bit;
        LM3Sxxxx.GPT[iModule].GPTMMIS &= ~bit;
    }
    else if (reg == GPIO_PORT_A_BLOCK) {                                 // port interrupts
        switch (iModule) {
        case A:
            LM3Sxxxx.GPIO_PORT_A.GPIORIS &= ~bit;
            LM3Sxxxx.GPIO_PORT_A.GPIOMIS &= ~bit;
            break;
        case B:
            LM3Sxxxx.GPIO_PORT_B.GPIORIS &= ~bit;
            LM3Sxxxx.GPIO_PORT_B.GPIOMIS &= ~bit;
            break;
        case C:
            LM3Sxxxx.GPIO_PORT_C.GPIORIS &= ~bit;                        // {15}
            LM3Sxxxx.GPIO_PORT_C.GPIOMIS &= ~bit;
            break;
        case D:
            LM3Sxxxx.GPIO_PORT_D.GPIORIS &= ~bit;                        // {15}
            LM3Sxxxx.GPIO_PORT_D.GPIOMIS &= ~bit;
            break;
        case E:
            LM3Sxxxx.GPIO_PORT_E.GPIORIS &= ~bit;                        // {15}
            LM3Sxxxx.GPIO_PORT_E.GPIOMIS &= ~bit;
            break;
        case F:
            LM3Sxxxx.GPIO_PORT_F.GPIORIS &= ~bit;                        // {15}
            LM3Sxxxx.GPIO_PORT_F.GPIOMIS &= ~bit;
            break;
        case G:
            LM3Sxxxx.GPIO_PORT_G.GPIORIS &= ~bit;                        // {15}
            LM3Sxxxx.GPIO_PORT_G.GPIOMIS &= ~bit;
            break;
        case H:
            LM3Sxxxx.GPIO_PORT_H.GPIORIS &= ~bit;                        // {15}
            LM3Sxxxx.GPIO_PORT_H.GPIOMIS &= ~bit;
            break;
        }
    }
    else if (reg == UART_0_BLOCK) {
            LM3Sxxxx.UART[iModule].UARTRIS &= ~bit;
            LM3Sxxxx.UART[iModule].UARTMIS &= ~bit;
    }
}

extern void PortB_Interrupt(void);

extern void fnSimulateLinkUp(void)
{
#ifdef ETH_INTERFACE
    MACRIS_IACK |= PHYINT;
    if (MACIM & PHYINT) {                                                // PHY interrupt is enabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));// simulate link up 
        ptrVect->processor_interrupts.irq_Ethernet_Controller();
    }                                                  
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}



// We can simulate timers during a tick here if required
//
extern int fnSimTimers(void)
{
#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)                    // {16}
    if (((RCGC0 & CGC_SARADC0) != 0) && ((ADCACTSS_0 & ASEN0) != 0)) {   // ADC sequencer active
        if (((ADCEMUX_0 & SS0_MASK) == TRIG_CONTROLLER) && (ADCPSSI_0 & INITIATE_SS0)) {// triggered by controller so perform ADC operation
            ADCPSSI_0 &= ~INITIATE_SS0;
            fnADC_sequenceTriggered(0);
        }
    }
#endif
    if ((SYSTICK_CSR & SYSTICK_ENABLE) != 0) {                           // SysTick is enabled
        unsigned long ulTickCount = 0;
        if ((SYSTICK_CSR & SYSTICK_CORE_CLOCK) != 0) {
            ulTickCount = ((TICK_RESOLUTION/1000) * (MASTER_CLOCK/1000));// count per tick period from internal clock
        }
        if (ulTickCount < SYSTICK_CURRENT) {
            SYSTICK_CURRENT -= ulTickCount;
        }
        else {
            SYSTICK_CURRENT = SYSTICK_RELOAD;
            if ((SYSTICK_CSR & SYSTICK_TICKINT) != 0) {                  // if interrupt enabled
                INT_CONT_STATE_REG |= PENDSTSET;
                if (!(LM3Sxxxx.CORTEX_M3_REGS.ulPRIMASK & INTERRUPT_MASKED)) { // if interrupt have been enabled, call interrupt handler
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                    ptrVect->ptrSysTick();
                }
            }
        }
    }

    if ((RCGC0 | CGC_WDT) && (!(SRCR0 & CGC_WDT)) && (WDTCTL & WD_INTEN)) { // if watchdog activated
        static int iWatchdogInterrupt = 0;
        if (WDTICR == 0) {                                               // has watchdog been retriggered?
            WDTICR = 0x55555555;                                         // set a value so that a timeout can be determined
            WDTVALUE = WDTLOAD;                                          // latch the reload value again
            iWatchdogInterrupt = 0;
        }
        else {                                                           // not retriggered
            unsigned long ulWatchdogCount = (TICK_RESOLUTION/1000) * (MASTER_CLOCK/1000);
            if (ulWatchdogCount < WDTVALUE) {
                WDTVALUE -= ulWatchdogCount;
            }
            else {
                WDTVALUE = 0;                                            // timeout
            }
        }
        if (WDTVALUE == 0) {                                             // if watchdog has timed out
            if (iWatchdogInterrupt != 0) {                               // second timeout ?
                return RESET_CARD_WATCHDOG;                              // signal watchdog reset
            }
            else {
                iWatchdogInterrupt = 1;                                  // first timeout is watchdog
                if (WDTCTL & WD_INTEN) {
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                    WDTVALUE = WDTLOAD;                                  // reload for second try
                    if (ptrVect->processor_interrupts.irq_Watchdog_Timer == 0) { // {20}
                        return RESET_CARD_WATCHDOG;
                    }
                    ptrVect->processor_interrupts.irq_Watchdog_Timer();  // call watchdog timer interupt
                }
            }
        }
    }
                                                                         // {6} handle general purpose timers (presently 32 bit mode single shot only)
    if ((RCGC1 & CGC_TIMER0) && (!(SRCR1 & CGC_TIMER0))) {               // timer 0 enabled and not held in reset
        if ((GPTMCTL_0 & TAEN) != 0) {                                   // timer running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000)*(TICK_RESOLUTION/1000)); // counts in a TICK period
            if (((GPTMTAMR_0 & 0x3) == TMR_ONE_SHOT) || ((GPTMTAMR_0 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTAR_0) {
                    if ((GPTMTAMR_0 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_0 &= ~TAEN;
                    }
                    GPTMTAR_0 = GPTMTAILR_0;
                    GPTMRIS_0 |= TATORIS;
                    GPTMMIS_0 = (GPTMRIS_0 & GPTMIMR_0);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_0 & TAOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_0 & TATOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer0_A();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTAR_0 -= ulCount;
                }
            }
        }
        if ((GPTMCTL_0 & TBEN) != 0) {                                   // {22} timer channel B running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000) * (TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTBMR_0 & 0x3) == TMR_ONE_SHOT) || ((GPTMTBMR_0 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTBR_0) {
                    if ((GPTMTBMR_0 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_0 &= ~TBEN;
                    }
                    GPTMTBR_0 = GPTMTBILR_0;
                    GPTMRIS_0 |= TBTORIS;
                    GPTMMIS_0 = (GPTMRIS_0 & GPTMIMR_0);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_0 & TBOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_0 & TBTOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer0_B();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTBR_0 -= ulCount;
                }
            }
        }
    } 
    if ((RCGC1 & CGC_TIMER1) && (!(SRCR1 & CGC_TIMER1))) {               // timer 1 enabled and not held in reset
        if (GPTMCTL_1 & TAEN) {                                          // timer running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000) * (TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTAMR_1 & 0x3) == TMR_ONE_SHOT) || ((GPTMTAMR_1 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTAR_1) {
                    if ((GPTMTAMR_1 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_1 &= ~TAEN;
                    }
                    GPTMTAR_1 = GPTMTAILR_1;
                    GPTMRIS_1 |= TATORIS;
                    GPTMMIS_1 = (GPTMRIS_1 & GPTMIMR_1);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_1 & TAOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_1 & TATOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer1_A();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTAR_1 -= ulCount;
                }
            }
        }
        if ((GPTMCTL_1 & TBEN) != 0) {                                     // {22} timer channel B running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000) * (TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTBMR_1 & 0x3) == TMR_ONE_SHOT) || ((GPTMTBMR_1 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTBR_1) {
                    if ((GPTMTBMR_1 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_1 &= ~TBEN;
                    }
                    GPTMTBR_1 = GPTMTBILR_1;
                    GPTMRIS_1 |= TBTORIS;
                    GPTMMIS_1 = (GPTMRIS_1 & GPTMIMR_1);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_1 & TBOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_1 & TBTOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer1_B();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTBR_1 -= ulCount;
                }
            }
        }
    }

#if CHIP_HAS_GP_TIMER > 2
    if ((RCGC1 & CGC_TIMER2) && (!(SRCR1 & CGC_TIMER2))) {               // timer 2 enabled and not held in reset
        if (GPTMCTL_2 & TAEN) {                                          // timer running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000) * (TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTAMR_2 & 0x3) == TMR_ONE_SHOT) || ((GPTMTAMR_2 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTAR_2) {
                    if ((GPTMTAMR_2 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_2 &= ~TAEN;
                    }
                    GPTMTAR_2 = GPTMTAILR_2;
                    GPTMRIS_2 |= TATORIS;
                    GPTMMIS_2 = (GPTMRIS_2 & GPTMIMR_2);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_2 & TAOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_2 & TATOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer2_A();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTAR_2 -= ulCount;
                }
            }
        }
        if (GPTMCTL_2 & TBEN) {                                          // {22} timer channel B running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000) * (TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTBMR_2 & 0x3) == TMR_ONE_SHOT) || ((GPTMTBMR_2 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTBR_2) {
                    if ((GPTMTBMR_2 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_2 &= ~TBEN;
                    }
                    GPTMTBR_2 = GPTMTBILR_2;
                    GPTMRIS_2 |= TBTORIS;
                    GPTMMIS_2 = (GPTMRIS_2 & GPTMIMR_2);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_2 & TBOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_2 & TBTOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer2_B();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTBR_2 -= ulCount;
                }
            }
        }
    }
#endif
#if CHIP_HAS_GP_TIMER > 3
    if ((RCGC1 & CGC_TIMER3) && (!(SRCR1 & CGC_TIMER3))) {               // timer 3 enabled and not held in reset
        if (GPTMCTL_3 & TAEN) {                                          // timer running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000)*(TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTAMR_3 & 0x3) == TMR_ONE_SHOT) || ((GPTMTAMR_3 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTAR_3) {
                    if ((GPTMTAMR_3 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_3 &= ~TAEN;
                    }
                    GPTMTAR_3 = GPTMTAILR_3;
                    GPTMRIS_3 |= TATORIS;
                    GPTMMIS_3 = (GPTMRIS_3 & GPTMIMR_3);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
  //                if (GPTMCTL_3 & TAOTE) {                             // check possible ADC trigger
  //                    if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
  //                        fnADC_sequenceTriggered(0);
  //                    }
  //                }                                                    // Timer 3 trigger doesn't work on the Luminary-Micro chips so removed here
    #endif
                    if (GPTMMIS_3 & TATOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer3_A();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTAR_3 -= ulCount;
                }
            }
        }
        if (GPTMCTL_3 & TBEN) {                                          // {22} timer channel B running - counting down
            unsigned long ulCount = ((MASTER_CLOCK/1000) * (TICK_RESOLUTION/1000)); // count in a TICK period
            if (((GPTMTBMR_3 & 0x3) == TMR_ONE_SHOT) || ((GPTMTBMR_3 & 0x3) == TMR_PERIODIC)) {
                if (ulCount > GPTMTBR_3) {
                    if ((GPTMTBMR_3 & 0x3) == TMR_ONE_SHOT) {
                        GPTMCTL_3 &= ~TBEN;
                    }
                    GPTMTBR_3 = GPTMTBILR_3;
                    GPTMRIS_3 |= TBTORIS;
                    GPTMMIS_3 = (GPTMRIS_3 & GPTMIMR_3);
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
                    if (GPTMCTL_3 & TBOTE) {                             // check possible ADC trigger
                        if ((ADCEMUX_0 & SS0_MASK) == TRIG_TIMER) {      // SS0 set up to be triggered on timer
                            fnADC_sequenceTriggered(0);
                        }
                    }
    #endif
                    if (GPTMMIS_3 & TBTOIM) {                            // check that the timeout interrupt is enabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
                        ptrVect->processor_interrupts.irq_Timer3_B();    // call the interrupt handler
                    }
                }
                else {
                    GPTMTBR_3 -= ulCount;
                }
            }
        }
    }
#endif
    return 0;
}

#ifdef SUPPORT_TOUCH_SCREEN
static int iPenLocationX = 0;                                            // last sample ADC input value
static int iPenLocationY = 0;
static int iPenDown = 0;

#define MIN_X_TOUCH          0x00dd                                      // reference values for 4-wire touch on Kitronix display on LM3S board
#define MAX_X_TOUCH          0x0374
#define MIN_Y_TOUCH          0x0120
#define MAX_Y_TOUCH          0x0318

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


#if 1 //defined RUN_IN_FREE_RTOS
extern unsigned long *fnGetRegisterAddress(unsigned long ulAddress)
{
    ulAddress -= 0xe000e000;
    ulAddress += (unsigned long)CORTEX_M3_BLOCK;
    return (unsigned long *)ulAddress;
}

extern void fnSetReg(int iRef, unsigned long ulValue)
{
    switch (iRef) {
    case 0:
        LM3Sxxxx.CORTEX_M3_REGS.ulR0 = ulValue;
        break;
    case 14:
        LM3Sxxxx.CORTEX_M3_REGS.ulPSP = ulValue;
        break;
    case 15:
        LM3Sxxxx.CORTEX_M3_REGS.ulMSP = ulValue;
        break;
    case 19:
        LM3Sxxxx.CORTEX_M3_REGS.ulPRIMASK = ulValue;
        break;
    case 20:
        LM3Sxxxx.CORTEX_M3_REGS.ulFAULTMASK = ulValue;
        break;
    case 22:
        LM3Sxxxx.CORTEX_M3_REGS.ulCONTROL = ulValue;
        break;
    }
}
#endif

// Prepare a string to be displayed in the simulator status bar          // {8}
//
extern void fnUpdateOperatingDetails(void)
{
    #if !defined NO_STATUS_BAR
    extern void fnPostOperatingDetails(char *ptrDetails);
  //unsigned long ulHCLK;
  //unsigned long ulAPB1;
  //unsigned long ulAPB2;
    CHAR cOperatingDetails[256];
    CHAR *ptrBuffer = cOperatingDetails;
    ptrBuffer = uStrcpy(ptrBuffer, "FLASH = ");
    ptrBuffer = fnBufferDec((SIZE_OF_FLASH/1024), 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    ptrBuffer = fnBufferDec((SIZE_OF_RAM/1024), 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, "k, HCLK = ");
    /*
    ulHCLK = (PLL_OUTPUT_FREQ >> ((RCC_CFGR & RCC_CFGR_HPRE_SYSCLK_DIV512) >> 4)); // HCLK according to register setting
    if ((RCC_CFGR & RCC_CFGR_PPRE1_HCLK_DIV2) != 0) {                    // if divide enabled
        #if defined _STM32L432
        ulAPB1 = (ulHCLK >> (((RCC_CFGR >> 8) & 0x03) + 1));             // APB1 clock according to register settings
        #else
        ulAPB1 = (ulHCLK >> (((RCC_CFGR >> 10) & 0x03) + 1));            // APB1 clock according to register settings
        #endif
    }
    else {
        ulAPB1 = ulHCLK;
    }
    if ((RCC_CFGR & RCC_CFGR_PPRE2_HCLK_DIV2) != 0) {                    // if divide enabled
        #if defined _STM32L432
        ulAPB2 = (ulHCLK >> (((RCC_CFGR >> 11) & 0x03) + 1));            // APB2 clock according to register settings
        #else
        ulAPB2 = (ulHCLK >> (((RCC_CFGR >> 13) & 0x03) + 1));            // APB2 clock according to register settings
        #endif
    }
    else {
        ulAPB2 = ulHCLK;
    }
    ptrBuffer = fnBufferDec(ulHCLK, 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, ", APB1 = ");
    ptrBuffer = fnBufferDec(ulAPB1, 0, ptrBuffer);
    ptrBuffer = uStrcpy(ptrBuffer, ", APB2 = ");
    ptrBuffer = fnBufferDec(ulAPB2, 0, ptrBuffer);*/
    fnPostOperatingDetails(cOperatingDetails);
    #endif
}

#endif

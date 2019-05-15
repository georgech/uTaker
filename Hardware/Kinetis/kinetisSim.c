/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetisSim.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    04.03.2012 Add NAND Flash controller                                 {1}
    18.03.2012 Add ADC                                                   {2}
    07.04.2012 Extend DMA simulation to multiple channels and include interrupt {3}
    08.04.2012 UART TX/RX DMA simulation                                 {4}
    17.04.2012 Add touch port references                                 {5}
    18.06.2012 Add CRC initialisation                                    {6}
    13.08.2012 Add 64 bytes of program once memory                       {7}
    09.10.2012 Add external ports                                        {8}
    27.10.2012 Change ADC port reference when varying value              {9}
    29.11.2012 Synchronise GPIOx_PDIR state with output values           {10}
    14.01.2013 Correct present leap year days                            {11}
    04.02.2013 Add USBHS                                                 {12}
    07.02.2013 Ignore USB simulation input when the USB controller is not enabled {13}
    24.05.2013 Add DDR1/2/LP SDRAM controller                            {14}
    29.05.2013 Add MCM                                                   {15}
    25.07.2013 Add PDB                                                   {16}
    06.08.2013 Add IMMEDIATE_MEMORY_ALLOCATION compatibility of fnStartTelnet() use {17}
    06.08.2013 Add free running DMA Rx UART support                      {18}
    04.10.2013 Allow ports to be read and port interrupts to operate on them when not configured as GPIO {19}
    27.10.2013 Add ADC triggering by PDB                                 {20}
    26.11.2013 Correct flash address checking                            {21}
    01.12.2013 Drop Ethernet reception frame if all buffer descriptors are in use {22}
    26.12.2013 Add RANDOM_NUMBER_GENERATOR_B for devices without RNGA    {23}
    25.01.2014 Add Kinetis KL support                                    {24}
    02.02.2014 Add USB host mode                                         {25}
    15.02.2014 Add mask revision                                         {26}
    18.02.2014 Correct RTC date/time priming                             {27}
    31.03.2014 Add watchdog clock prescaler                              {28}
    05.04.2014 Add FlexTimer interrupts                                  {29}
    17.04.2014 Add RTC seconds interrupt support                         {30}
    20.04.2014 Define KL25 disabled ports                                {31}
    06.05.2014 Add KL DMA based DMA simulation                           {32}
    11.05.2014 Add FlexNVM data flash support                            {33}
    08.06.2014 Add Crossbar Switch                                       {34}
    15.06.2014 Add LPTMR                                                 {35}
    17.06.2014 Extend K part DMA to respect short word and long word transfer sizes {36}
    25.06.2014 Add hardware operating details to simulator status bar    {37}
    22.07.2014 Respect TPM clock source selections                       {38}
    19.08.2014 Respect clock divider registers when displaying clock rates {39}
    14.05.2015 Add ADC compare function                                  {40}
    17.01.2016 Add fnResetENET()                                         {41}
    02.02.2017 Adapt for us tick resolution
    24.03.2017 Reset all host endpoints data frame types                 {42}
    03.05.2017 Improve LPUART and UART rx DMA operation
    18.05.2017 Add optional logging of UART reception to a simulation file {43}
    13.07.2017 Define KL82 disabled ports                                {44}
    04.08.2017 Add fnSetBitBandPeripheralValue() and fnClearBitBandPeripheralValue()
    11.08.2017 Add PCC support                                           {45}
    12.09.2017 Add INTMUX support and LPUART2 using its interrupts       {46}
    26.09.2017 Add LPIT support                                          {47}
    04.11.2017 Add true rando number generator registers initialisation  {48}
    08.05.2018 Added simulation of all FTM/TPM channel interrupts        {49}
    29.08.2018 Add input capture simulation                              {50}
    03.09.2018 Extend CAN support to 3 CAN controllers
    18.09.2018 Add key input port defaut override and extended port default states {51}
 
*/  
                          

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined _KINETIS                                                     // only on Kinetis parts

#if defined CAN_INTERFACE
    #if defined SIM_KOMODO
        #include "..\..\WinSim\Komodo\komodo.h" 
    #else
        #define Komodo int 
    #endif
#endif

#define PROGRAM_ONCE_AREA_SIZE   64                                      // 64 bytes in 8 blocks of 8 bytes

KINETIS_PERIPH  kinetis = {0};

unsigned char uninitialisedRAM[16];

unsigned long vector_ram[(sizeof(VECTOR_TABLE))/sizeof(unsigned long)];  // long word aligned

#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    extern unsigned long fnGetExtPortFunction(int iExtPortReference);
    extern unsigned long fnGetExtPortState(int iExtPortReference);
#endif
extern int fnCrystalFonzKey(unsigned char ucPort, int iChange, unsigned long ulBit);

static unsigned long ulPort_in_A, ulPort_in_B, ulPort_in_C, ulPort_in_D, ulPort_in_E;
#if PORTS_AVAILABLE > 5
    static unsigned long ulPort_in_F;
#endif
#if PORTS_AVAILABLE > 6
    static unsigned long ulPort_in_G;
#endif
#if PORTS_AVAILABLE > 7
    static unsigned long ulPort_in_H;
#endif
#if PORTS_AVAILABLE > 8
    static unsigned long ulPort_in_I;
#endif
#if defined SERIAL_INTERFACE
    static int iUART_rx_Active[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {0};
#endif
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    static unsigned char ucPortFunctions[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {0};
#else
    static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = {0};
#endif
static unsigned long ulPeripherals[PORTS_AVAILABLE] = {0};
#if defined SUPPORT_ADC
    static unsigned short usADC_values[ADC_CONTROLLERS][32] = {{0}};
    static void *ptrPorts[2] = {(void *)ucPortFunctions, (void *)usADC_values}; // {2}
#endif
#if defined RUN_IN_FREE_RTOS
    extern void fnExecutePendingInterrupts(int iRecursive);
#endif

static const unsigned long ulDisabled[PORTS_AVAILABLE] = {
#if defined KINETIS_K00
    0x00003020,                                                          // port A disabled default pins
    0x000f0000,                                                          // port B disabled default pins
    0x00000c30,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x00000003                                                           // port E disabled default pins
#elif defined KINETIS_K27 || defined KINETIS_K28
    (PORTA_BIT5 | PORTA_BIT6 | PORTA_BIT9 | PORTA_BIT10 | PORTA_BIT11 | PORTA_BIT12 | PORTA_BIT13 | PORTA_BIT14 | PORTA_BIT15 | PORTA_BIT16 | PORTA_BIT17 | PORTA_BIT20 | PORTA_BIT21 | PORTA_BIT22 | PORTA_BIT23 | PORTA_BIT24 | PORTA_BIT25 | PORTA_BIT26 | PORTA_BIT27 | PORTA_BIT28 | PORTA_BIT29 | PORTA_BIT30 | PORTA_BIT31), // port A disabled default pins
    (PORTB_BIT4 | PORTB_BIT5 | PORTB_BIT6 | PORTB_BIT7 | PORTB_BIT8 | PORTB_BIT9 | PORTB_BIT10 | PORTB_BIT11 | PORTB_BIT12 | PORTB_BIT13 | PORTB_BIT14 | PORTB_BIT15 | PORTB_BIT16 | PORTB_BIT17 | PORTB_BIT18 | PORTB_BIT19 | PORTB_BIT20 | PORTB_BIT21 | PORTB_BIT22 | PORTB_BIT23), // port B disabled default pins
    (PORTC_BIT4 | PORTC_BIT5 | PORTC_BIT10 | PORTC_BIT11 | PORTC_BIT12 | PORTC_BIT13 | PORTC_BIT14 | PORTC_BIT15 | PORTC_BIT16 | PORTC_BIT17 | PORTC_BIT18 | PORTC_BIT19 | PORTC_BIT20 | PORTC_BIT21 | PORTC_BIT22 | PORTC_BIT23 | PORTC_BIT24 | PORTC_BIT25 | PORTC_BIT26 | PORTC_BIT27 | PORTC_BIT28 | PORTC_BIT29), // port C disabled default pins
    (PORTD_BIT0 | PORTD_BIT2 | PORTD_BIT3 | PORTD_BIT4 | PORTD_BIT7 | PORTD_BIT8 | PORTD_BIT9 | PORTD_BIT10 | PORTD_BIT11 | PORTD_BIT12 | PORTD_BIT13 | PORTD_BIT14 | PORTD_BIT15),                              // port D disabled default pins
    (PORTE_BIT0 | PORTE_BIT1 | PORTE_BIT2 | PORTE_BIT3 | PORTE_BIT4 | PORTE_BIT5 | PORTE_BIT6 | PORTE_BIT7 | PORTE_BIT8 | PORTE_BIT9 | PORTE_BIT10 | PORTE_BIT11 | PORTE_BIT12 | PORTE_BIT13 | PORTE_BIT20 | PORTE_BIT21 | PORTE_BIT22 | PORTE_BIT23)                                            // port E disabled default pins
#elif defined KINETIS_K12
    (PORTA_BIT17 | PORTA_BIT16 | PORTA_BIT15 | PORTA_BIT14 | PORTA_BIT13 | PORTA_BIT12 | PORTA_BIT5), // port A disabled default pins
    (PORTB_BIT19 | PORTB_BIT18 | PORTB_BIT17 | PORTB_BIT16 | PORTB_BIT13 | PORTB_BIT12 | PORTB_BIT11 | PORTB_BIT10), // port B disabled default pins
    (PORTC_BIT17 | PORTC_BIT16 | PORTC_BIT13 | PORTC_BIT12 | PORTC_BIT11 | PORTC_BIT10 | PORTC_BIT5 | PORTC_BIT4), // port C disabled default pins
    (PORTD_BIT3 | PORTD_BIT2 | PORTD_BIT0),                              // port D disabled default pins
    (PORTE_BIT5 | PORTE_BIT4)                                            // port E disabled default pins
#elif defined KINETIS_K10
    0x3f01ce60,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K20
    #if defined KINETIS_K22
    0x3f01ce60,                                                          // port A disabled default pins
    0x00ff8300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
    #elif PIN_COUNT == PIN_COUNT_64_PIN
    0x00003020,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x00000830,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0xffffffff                                                           // port E disabled default pins
    #else
    0x3f01ce60,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
    #endif
#elif defined KINETIS_K30
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K40
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K50
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K51
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K52
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K53
    0x3f01ce40,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000000,                                                          // port C disabled default pins
    0x0000fc00,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K64
    (PORTA_BIT29 | PORTA_BIT28 | PORTA_BIT27 | PORTA_BIT26 | PORTA_BIT25 | PORTA_BIT24 | PORTA_BIT16 | PORTA_BIT15 | PORTA_BIT14 | PORTA_BIT11 | PORTA_BIT10 | PORTA_BIT9 | PORTA_BIT6 | PORTA_BIT5), // port A disabled default pins
    (PORTB_BIT23 | PORTB_BIT22 | PORTB_BIT21 | PORTB_BIT20 | PORTB_BIT19 | PORTB_BIT18 | PORTB_BIT17 | PORTB_BIT16 | PORTB_BIT13 | PORTB_BIT12 | PORTB_BIT9 | PORTB_BIT8), // port B disabled default pins
    (PORTC_BIT19 | PORTC_BIT18 | PORTC_BIT17 | PORTC_BIT16 | PORTC_BIT15 | PORTC_BIT14 | PORTC_BIT13 | PORTC_BIT12 | PORTC_BIT5 | PORTC_BIT4), // port C disabled default pins
    (PORTD_BIT15 | PORTD_BIT14 | PORTD_BIT13 | PORTD_BIT12 | PORTD_BIT11 | PORTD_BIT10 | PORTD_BIT9 | PORTD_BIT8 | PORTD_BIT7 | PORTD_BIT4 | PORTD_BIT3 | PORTD_BIT2 | PORTD_BIT0), // port D disabled default pins
    (PORTE_BIT28 | PORTE_BIT27 | PORTE_BIT26 | PORTE_BIT12 | PORTE_BIT11 | PORTE_BIT10 | PORTE_BIT9 | PORTE_BIT8 | PORTE_BIT7 | PORTE_BIT6 | PORTE_BIT5 | PORTE_BIT4) // port E disabled default pins
#elif defined KINETIS_K60
    0x3f01ce40,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K61
    0x00000020,                                                          // port A disabled default pins
    0x00c00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x000004f0,                                                          // port E disabled default pins
    0x001ff900                                                           // port F disabled default pins
#elif defined KINETIS_K64
    0x3f01ce40,                                                          // port A disabled default pins
    0x00ff0300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_K70
    0x00000020,                                                          // port A disabled default pins
    0x00c00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x000004f0,                                                          // port E disabled default pins
    0x001ff900                                                           // port F disabled default pins
#elif defined KINETIS_K80
    0x3f01ce40,                                                          // port A disabled default pins
    0x00f00300,                                                          // port B disabled default pins
    0x000ff030,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0x1c001ff0                                                           // port E disabled default pins
#elif defined KINETIS_KE15 || defined KINETIS_KE18
    0x0003ff00,                                                          // port A disabled default pins
    0x00038f00,                                                          // port B disabled default pins
    0x00003f00,                                                          // port C disabled default pins
    0x0003fd00,                                                          // port D disabled default pins
    0x0001f080                                                           // port E disabled default pins
#elif defined KINETIS_KE                                                 // not used by most KE
    0x00000000,
    #if PORTS_AVAILABLE > 1
    0x00000000
    #endif
#elif defined KINETIS_KL                                                 // {24}
    #if defined KINETIS_KL02
    0x00002c60,                                                          // port A disabled default pins
    0x000030d8,                                                          // port B disabled default pins
    #elif defined KINETIS_KL03
    0x000000e0,                                                          // port A disabled default pins
    0x00002cd8,                                                          // port B disabled default pins
    #elif defined KINETIS_KL05
    0x000fcc60,                                                          // port A disabled default pins
    0x001f80d8,                                                          // port B disabled default pins
    #elif defined KINETIS_KL25                                           // {31}
    0x0003f026,                                                          // port A disabled default pins
    0x00000f00,                                                          // port B disabled default pins
    0x00033c38,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x8300003f                                                           // port E disabled default pins
    #elif defined KINETIS_KL26
    0x0003f0e6,                                                          // port A disabled default pins
    0x00f01f00,                                                          // port B disabled default pins
    0x00f73c38,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x8700003f                                                           // port E disabled default pins
    #elif defined KINETIS_KL17 || defined KINETIS_KL27
    0x00003026,                                                          // port A disabled default pins
    0x000f0000,                                                          // port B disabled default pins
    0x00000c38,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x83000003                                                           // port E disabled default pins
    #elif defined KINETIS_KL28
    (PORTA_BIT29 | PORTA_BIT16 | PORTA_BIT15 | PORTA_BIT14 | PORTA_BIT13 | PORTA_BIT12 | PORTA_BIT11 | PORTA_BIT10 | PORTA_BIT7 | PORTA_BIT6 | PORTA_BIT5 | PORTA_BIT4 | PORTA_BIT2 | PORTA_BIT1), // port A disabled default pins
    (PORTB_BIT23 | PORTB_BIT22 | PORTB_BIT21 | PORTB_BIT20 | PORTB_BIT11 | PORTB_BIT10 | PORTB_BIT9 | PORTB_BIT8 | PORTB_BIT7 | PORTB_BIT6), // port B disabled default pins
    (PORTC_BIT23 | PORTC_BIT22 | PORTC_BIT19 | PORTC_BIT18 | PORTC_BIT17 | PORTC_BIT16 | PORTC_BIT15 | PORTC_BIT14 | PORTC_BIT13 | PORTC_BIT12 | PORTC_BIT11 | PORTC_BIT10 | PORTC_BIT5 | PORTC_BIT4), // port C disabled default pins
    (PORTD_BIT15 | PORTD_BIT14 | PORTD_BIT13 | PORTD_BIT12 | PORTD_BIT11 | PORTD_BIT10 | PORTD_BIT9 | PORTD_BIT8 | PORTD_BIT7 | PORTD_BIT4 | PORTD_BIT3 | PORTD_BIT2 | PORTD_BIT0), // port D disabled default pins
    (PORTE_BIT31 | PORTE_BIT26 | PORTE_BIT6 | PORTE_BIT5 | PORTE_BIT4)   // port E disabled default pins
    #elif defined KINETIS_KL82                                           // {44}
    0x2003fc00,                                                          // port A disabled default pins
    0x00f00ff0,                                                          // port B disabled default pins
    0x000ffc38,                                                          // port C disabled default pins
    0x0000ff9d,                                                          // port D disabled default pins
    0xffffffff                                                           // port E disabled default pins
    #else                                                                // KL20/KL40
    0x0003f0e6,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
        #if PORTS_AVAILABLE > 2
    0x00000000,                                                          // port C disabled default pins
    0x00000000,                                                          // port D disabled default pins
    0x8700007f                                                           // port E disabled default pins
        #endif
    #endif
#elif defined KINETIS_KV10
    0x00000006,                                                          // port A disabled default pins
    0x00030000,                                                          // port B disabled default pins
    0x00000030,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x03000000                                                           // port E disabled default pins
#elif defined KINETIS_KV30 || defined KINETIS_KV40 || defined KINETIS_KV50
    0x0001f020,                                                          // port A disabled default pins
    0x00ff0200,                                                          // port B disabled default pins
    0x0007f030,                                                          // port C disabled default pins
    0x0000009d,                                                          // port D disabled default pins
    0x04000070                                                           // port E disabled default pins
#elif defined KINETIS_KW2X
    0x00000000,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000030,                                                          // port C disabled default pins
    0x0000000d,                                                          // port D disabled default pins
    0x00000010                                                           // port E disabled default pins
#elif defined KINETIS_KW4X
    (PORTA_BIT16 | PORTA_BIT17 | PORTA_BIT18 | PORTA_BIT19),             // port A disabled default pins
    (PORTB_BIT0 | PORTB_BIT1 | PORTB_BIT2 | PORTB_BIT3),                 // port B disabled default pins
    (PORTC_BIT0 | PORTC_BIT1 | PORTC_BIT2 | PORTC_BIT3 | PORTC_BIT4 | PORTC_BIT5 | PORTC_BIT6 | PORTC_BIT7 | PORTC_BIT16 | PORTC_BIT17 | PORTC_BIT18 | PORTC_BIT19), // port C disabled default pins
#elif defined KINETIS_KM
    0x00000000,                                                          // port A disabled default pins
    0x00000000,                                                          // port B disabled default pins
    0x00000030,                                                          // port C disabled default pins
    0x0000000d,                                                          // port D disabled default pins
    0x00000010                                                           // port E disabled default pins
#endif
};

static void fnPortInterrupt(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig);

static int iFlagRefresh = 0;

#if defined USB_INTERFACE
    #define _fnLE_add(x) (void *)x

    #if defined USB_HS_INTERFACE                                         // {12}
        static unsigned char ucHSTxBuffer[NUMBER_OF_USBHS_ENDPOINTS] = {0}; // monitor the high speed controller's transmission buffer use
        static unsigned long ulHSEndpointInt = 0;
    #endif
    static unsigned char ucTxBuffer[NUMBER_OF_USB_ENDPOINTS] = {0};      // monitor the controller's transmission buffer use
    static unsigned char ucRxBank[NUMBER_OF_USB_ENDPOINTS];              // monitor the buffer to inject to
    static unsigned long ulEndpointInt = 0;
    #if defined USB_HOST_SUPPORT
    static int iData1Frame[NUMBER_OF_USB_ENDPOINTS] = {0};
    #endif
#endif
#if defined KINETIS_KL || defined KINETIS_KW4X                           // {24}
    static unsigned long ulCOPcounter = 0;
#endif
#if defined KINETIS_KM
    #define PORT_CAST unsigned char
#else
    #define PORT_CAST unsigned long
#endif

// Initialisation of all non-zero registers in the device
//
static void fnSetDevice(unsigned long *port_inits)
{
    extern void fnEnterHW_table(void *hw_table);

#if defined SUPPORT_ADC
    int i;
    int j;
#endif
    unsigned long initial_input_state;

    kinetis.CORTEX_M4_REGS.ulPRIMASK = INTERRUPT_MASKED;                 // interrupts are masked out of reset

#if !defined KINETIS_KL
    FMC_PFAPR  = 0x00f8003f;                                             // flash memory controller
    #if defined KINETIS_K65 || defined KINETIS_K66
    FMC_PFB01CR = 0x3004001f;
    FMC_PFB23CR = 0x3004001f;
    #else
    FMC_PFB0CR = 0x3002001f;
    FMC_PFB1CR = 0x3002001f;
    #endif
#endif

#if defined FLASH_CONTROLLER_FTMRE
    FTMRH_FSTAT = FTMRH_STAT_CCIF;
    FTMRH_FSEC = KINETIS_FLASH_CONFIGURATION_SECURITY;
    FTMRH_FPROT = KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT;
    FTMRH_FOPT = KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION;
#else
    FTFL_FSTAT = FTFL_STAT_CCIF;                                        // flash controller
    FTFL_FSEC = KINETIS_FLASH_CONFIGURATION_SECURITY;
    FTFL_FOPT = KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION;
    FTFL_FEPROT = KINETIS_FLASH_CONFIGURATION_EEPROM_PROT;
    FTFL_FDPROT = KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT;
    FTFL_FPROT0 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION >> 24);
    FTFL_FPROT1 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION >> 16);
    FTFL_FPROT2 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION >> 8);
    FTFL_FPROT3 = (unsigned char)(KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION);
#endif
#if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_KM || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000) // {26}
    #if defined KINETIS_KL28
    RCM_VERID = 0x03000003;
    RCM_PARAM = 0x00002eef;
    RCM_SRS = 0x00000082;
    RCM_SSRS = 0x00000082;
    #else
        #if defined KINETIS_KL && defined RTC_USES_LPO_1kHz
    RCM_SRS0 = RCM_SRS0_PIN;                                             // simulate external reset
        #else
    RCM_SRS0 = (RCM_SRS0_POR | RCM_SRS0_LVD);                            // reset control module - reset status due to power on reset
        #endif
    #endif
    #if defined KINETIS_KL || defined KINETIS_K22 || defined KINETIS_K65 || defined KINETIS_K66
    SMC_STOPCTRL = SMC_STOPCTRL_VLLSM_VLLS3;
    #else
    SMC_VLLSCTRL = SMC_VLLSCTRL_VLLSM_VLLS3;
    #endif
    SMC_PMSTAT = SMC_PMSTAT_RUN;
#elif !defined KINETIS_KE && !defined KINETIS_KEA
    MC_SRSL = (MC_SRSL_POR | MC_SRSL_LVD);                               // mode control - reset status due to power on reset
#endif
#if defined KINETIS_WITH_WDOG32
    WDOG0_CS = (0xffff0000 | WDOG_CS_CMD32EN | WDOG_CS_CLK_1kHz | WDOG_CS_EN);
    WDOG0_CNT = 0x00000002;
    WDOG0_TOVAL = 0x00000400;
#elif (defined KINETIS_KL && !defined KINETIS_KL82) || defined KINETIS_KW4X
    SIM_COPC = SIM_COPC_COPT_LONGEST;                                    // COP (computer operating properly) rather than watchdog
#elif defined KINETIS_KE
    WDOG_CS1 = WDOG_CS1_EN;
    WDOG_CS2 = WDOG_CS2_CLK_1kHz;
    WDOG_TOVALL = 0x04;
#else
    WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_CLKSRC | WDOG_STCTRLH_WDOGEN); // watchdog
    WDOG_STCTRLL = 0x0001;
    WDOG_TOVALH = 0x004c;
    WDOG_TOVALL = 0x4b4c;
    WDOG_WINL = 0x0010;
    WDOG_REFRESH = 0xb480;
    WDOG_UNLOCK = 0xd928;
    WDOG_PRESC = 0x0400;
#endif
#if defined KINETIS_WITH_SCG
    SCG_VERID = 0x01000000;
    SCG_PARAM = (SCG_PARAM_CLKPRES_SOSC | SCG_PARAM_CLKPRES_SIRC | SCG_PARAM_CLKPRES_FIRC | SCG_PARAM_CLKPRES_SPLL | SCG_PARAM_DIVPRES_DIVSLOW | SCG_PARAM_DIVPRES_DIVCORE);
    SCG_CSR = (SCG_CSR_SCS_SIRC_CLK | ((1 - 1) << SCG_CSR_DIVCORE_SHIFT) | ((2 - 1) << SCG_CSR_DIVSLOW_SHIFT));
    SCG_RCCR = (SCG_RCCR_SCS_SIRC_CLK | ((1 - 1) << SCG_RCCR_DIVCORE_SHIFT) | ((2 - 1) << SCG_RCCR_DIVSLOW_SHIFT));
    SCG_VCCR = (SCG_VCCR_SCS_SIRC_CLK | ((1 - 1) << SCG_VCCR_DIVCORE_SHIFT) | ((2 - 1) << SCG_VCCR_DIVSLOW_SHIFT));
    SCG_HCCR = (SCG_HCCR_SCS_SIRC_CLK | ((1 - 1) << SCG_HCCR_DIVCORE_SHIFT) | ((2 - 1) << SCG_HCCR_DIVSLOW_SHIFT));
    SCG_CLKOUTCNFG = SCG_CLKOUTCNFG_SIRC_CLK;
    SCG_SOSCCFG = SCG_SOSCCFG_RANGE_LOW;
    SCG_SIRCCSR = (SCG_SIRCCSR_SIRCEN | SCG_SIRCCSR_SIRLPEN | SCG_SIRCCSR_SIRCEN | SCG_SIRCCSR_SIRCEN);
    SCG_SIRCCFG = 0x00000001;
#elif defined KINETIS_KL                                                 // {24}
    #if defined KINETIS_WITH_MCG_LITE
    MCG_C1 = MCG_C1_CLKS_LIRC;
    MCG_C2 = MCG_C2_IRCS;
    MCG_S  = MCG_S_CLKST_LICR;
    #else
    MCG_C1 = MCG_C1_IREFS;
    #endif
#elif defined KINETIS_KE
    SIM_SRSID = (SIM_SRSID_LVD | SIM_SRSID_POR);
    ICS_C1 = ICS_C1_IREFS;
    ICS_C2 = ICS_C2_BDIV_2;
    ICS_S = ICS_S_IREFST;
#else
    EWM_CMPH = 0xff;                                                     // external watchdog monitor
    MCG_C1 = MCG_C1_IREFS;                                               // multi-purpose clock generator
    MCG_C2 = MCG_C2_LOCRE0;
    MCG_S  = MCG_S_IREFST;
    #if defined MCG_SC
    MCG_SC = MCG_SC_FCRDIV_2;
    #endif
    #if defined MCG_C10 && (defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000))
    MCG_C10 = 0x80;
    #endif
    #if defined KINETIS_REVISION_2
    SIM_SDID = 0x114a;                                                   // K60 ID (revision 2)
    #else
    SIM_SDID = 0x014a;                                                   // K60 ID (revision 1)
    #endif
#endif
#if defined KINETIS_KL28
    PMC_VERID = 0x04000000;
    PMC_PARAM = 0x00000003;
    PMC_HVDSC1 = 0x00000001;
#endif
    PMC_LVDSC1 = PMC_LVDSC1_LVDRE;                                       // low voltage detect reset enabled by default
    PMC_REGSC = PMC_REGSC_REGONS;                                        // regulator is in run regulation
#if defined MPU_AVAILABLE
    MPU_CESR  = (0x0081820 | MPU_CESR_VLD);                              // memory protection unit
    MPU_RGD0_WORD1  = 0x0000001f;
    MPU_RGD1_WORD1  = 0x0000001f;
    MPU_RGD2_WORD1  = 0x0000001f;
    MPU_RGD3_WORD1  = 0x0000001f;
    MPU_RGD4_WORD1  = 0x0000001f;
    MPU_RGD5_WORD1  = 0x0000001f;
    MPU_RGD6_WORD1  = 0x0000001f;
    MPU_RGD7_WORD1  = 0x0000001f;
    MPU_RGD8_WORD1  = 0x0000001f;
    MPU_RGD9_WORD1  = 0x0000001f;
    MPU_RGD10_WORD1 = 0x0000001f;
    MPU_RGD11_WORD1 = 0x0000001f;
    MPU_RGD12_WORD1 = 0x0000001f;
    MPU_RGD13_WORD1 = 0x0000001f;
    MPU_RGD14_WORD1 = 0x0000001f;
    MPU_RGD15_WORD1 = 0x0000001f;
#endif
#if !defined KINETIS_WITHOUT_PIT
    #if defined LPITS_AVAILABLE                                          // {47}
    LPIT0_VERID = 0x01000000;
    LPIT0_PARAM = (LPIT_CHANNELS | (LPIT_INPUT_TRIGGERS << 8));
    LPIT0_CVAL0 = 0xffffffff;
    LPIT0_CVAL1 = 0xffffffff;
    LPIT0_CVAL2 = 0xffffffff;
    LPIT0_CVAL3 = 0xffffffff;
    #else
    PIT_MCR = PIT_MCR_MDIS;                                              // PITs disabled
    #endif
#endif
#if defined KINETIS_WITH_PCC                                             // {45}
    #if defined KINETIS_KE15 || defined KINETIS_KE18
    PCC_DMA0 = (PCC_PR | PCC_CGC);
    PCC_FLASH = (PCC_PR | PCC_CGC);
    PCC_DMAMUX0 = PCC_PR;
    PCC_ADC1 = (PCC_PR | PCC_CGC);
    PCC_LPSPI0 = PCC_PR;
    PCC_LPSPI1 = PCC_PR;
    PCC_CRC = PCC_PR;
    PCC_PDB0 = PCC_PR;
    PCC_LPIT0 = PCC_PR;
    PCC_FLEXTMR0 = PCC_PR;
    PCC_FLEXTMR1 = PCC_PR;
    PCC_FLEXTMR2 = PCC_PR;
    PCC_ADC0 = (PCC_PR | PCC_CGC);
    PCC_RTC = PCC_PR;
    PCC_LPTMR0 = PCC_PR;
    #if !defined KINETIS_KE18
    PCC_TSI = PCC_PR;
    #endif
    PCC_PORTA = PCC_PR;
    PCC_PORTB = PCC_PR;
    PCC_PORTC = PCC_PR;
    PCC_PORTD = PCC_PR;
    PCC_PORTE = PCC_PR;
    PCC_PWT = PCC_PR;
    PCC_FLEXIO = PCC_PR;
    PCC_OSC32 = PCC_PR;
    PCC_EWM = PCC_PR;
    PCC_LPI2C0 = PCC_PR;
    PCC_LPI2C1 = PCC_PR;
    PCC_LPUART0 = PCC_PR;
    PCC_LPUART1 = PCC_PR;
    PCC_LPUART2 = PCC_PR;
    PCC_CMP0 = PCC_PR;
    PCC_CMP1 = PCC_PR;
    #else
    PCC_DMA0 = PCC_PR;
    PCC_FLASH = (PCC_PR | PCC_CGC);
    PCC_DMAMUX0 = PCC_PR;
    PCC_INTMUX0 = PCC_PR;
    PCC_TPM2    = PCC_PR;
    PCC_LPIT0   = PCC_PR;
    PCC_LPTMR0  = PCC_PR;
    PCC_RTC     = PCC_PR;
    PCC_LPSPI2  = PCC_PR;
    PCC_LPI2C2  = PCC_PR;
    PCC_LPUART2 = PCC_PR;
    PCC_SAI0    = PCC_PR;
    PCC_EMVSIM0 = PCC_PR;
    PCC_USB0FS  = PCC_PR;
    PCC_PORTA   = PCC_PR;
    PCC_PORTB   = PCC_PR;
    PCC_PORTC   = PCC_PR;
    PCC_PORTD   = PCC_PR;
    PCC_PORTE   = PCC_PR;
    PCC_TSI0    = PCC_PR;
    PCC_ADC0    = PCC_PR;
    PCC_DAC0    = PCC_PR;
    PCC_CMP0    = PCC_PR;
    PCC_VREF    = PCC_PR;
    PCC_CRC     = PCC_PR;
    PCC_TRNG    = PCC_PR;
    PCC_TPM0    = PCC_PR;
    PCC_TPM1    = PCC_PR;
    PCC_LPTMR1  = PCC_PR;
    PCC_LPSPI0  = PCC_PR;
    PCC_LPSPI1  = PCC_PR;
    PCC_LPI2C0  = PCC_PR;
    PCC_LPI2C1  = PCC_PR;
    PCC_LPUART0 = PCC_PR;
    PCC_LPUART1 = PCC_PR;
    PCC_FLEXIO0 = PCC_PR;
    PCC_CMP1    = PCC_PR;
    #endif
#endif
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    SIM_SCGC = (SIM_SCGC_FLASH | SIM_SCGC_SWD);
    SIM_SOPT0 = (SIM_SOPT_NMIE | SIM_SOPT_RSTPE | SIM_SOPT_SWDE);        // PTB4 functions as NMI, PTA5 pin functions as RESET, PTA4 and PTC4 function as single wire debug
#elif defined KINETIS_KV
    SIM_SCGC4 = 0xf0000030;
    SIM_SCGC5 = 0x00040180;
    SIM_SCGC6 = SIM_SCGC6_FTFL;
    SIM_SCGC7 = 0x00000100;
    #if defined KINETIS_KV40
    if ((FTFL_FOPT & FTFL_FOPT_LPBOOT_CLK_DIV_1) != 0) {
        SIM_CLKDIV1 = (SIM_CLKDIV1_SYSTEM_1 | SIM_CLKDIV1_FAST_BUS_1 | SIM_CLKDIV1_BUS_2); // fast boot
    }
    else {
        SIM_CLKDIV1 = (SIM_CLKDIV1_SYSTEM_8 | SIM_CLKDIV1_FAST_BUS_8 | SIM_CLKDIV1_BUS_16); // low power boot
    }
    #else
    SIM_CLKDIV1 = (SIM_CLKDIV1_BUS_2 | SIM_CLKDIV5_ADC_2);
    #endif
#elif !defined KINETIS_WITH_PCC
    SIM_SCGC6 = (0x40000000 | SIM_SCGC6_FTFL);
    #if defined KINETIS_HAS_IRC48M && !defined KINETIS_KL
    SIM_SOPT2 = (SIM_SOPT2_TRACECLKSEL);
    #elif defined KINETIS_K_FPU || (KINETIS_MAX_SPEED > 100000000)
    SIM_SOPT2 = (SIM_SOPT2_TRACECLKSEL | 0x04000000 | SIM_SOPT2_NFCSRC_MCGPLL0CLK);
    SIM_CLKDIV4 = 0x00000002;
    #elif !defined KINETIS_KL
    SIM_SOPT2 = SIM_SOPT2_TRACECLKSEL;
    #endif
#endif
#if defined KINETIS_KL
    #if defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL05
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT3);              // default SWD_CLK
    PORTA_PCR1 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT3);    // default reset input
    PORTA_PCR2 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT3);              // default SWD_DIO
    PORTA_PCR3 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT0);              // default EXTAL0
    PORTA_PCR4 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT0);              // default XTAL0
        #if defined KINETIS_KL03
    PORTB_PCR5 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT3);    // default NMI input
        #endif
    #else
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT7);              // port configuration defaults (JTAG/SWD)
    PORTA_PCR20 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);   // default reset input
    PORTA_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    #endif
#elif defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    GPIOA_PIDR = 0xffffffff;                                             // port input disable registers
    #if PORTS_AVAILABLE > 1
    GPIOB_PIDR = 0xffffffff;
    #endif
    #if (defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
    PORT_IOFLT0 = 0x00c00000;
    PORT_PUE0   = 0x00100000;
    #else
    PORT_IOFLT = 0x00c00000;
        #if (defined KINETIS_KE04 && (SIZE_OF_FLASH <= (8 * 1024)))
    PORT_PUEL  = 0x00000001;
        #else
    PORT_PUEL  = 0x00100000;
        #endif
    #endif
#elif defined KINETIS_KV10
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT7);              // port configuration defaults (JTAG/SWD)
    PORTA_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR20 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
#elif defined KINETIS_KE15 || defined KINETIS_KE18
    PORTD_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);    // NMI
    PORTA_PCR5 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);    // reset
    PORTC_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);    // SWD_CLK
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);    // SWD_DIO
#else
    PORTA_PCR0 = (PORT_PE | PORT_DSE_HIGH | PORT_MUX_ALT7);              // port configuration defaults (JTAG/SWD)
    PORTA_PCR1 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR2 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR3 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    PORTA_PCR4 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    if ((ulDisabled[0] & 0x20) == 0) {
        PORTA_PCR5 = (PORT_PS_UP_ENABLE | PORT_DSE_HIGH | PORT_MUX_ALT7);
    }
#endif
#if defined USB_INTERFACE
    PER_ID = 0x04;                                                       // USB-OTG
    ID_COMP = 0xfb;
    REV = 0x33;
    ADD_INFO = IEHOST;
    OTG_INT_STAT = MSEC_1;
    #if defined KINETIS_KL28
    USB_CLK_RECOVER_INT_EN = USB_CLK_RECOVER_INT_EN_OVF_ERROR_EN;
    #endif
#endif
#if (LPUARTS_AVAILABLE > 0)
    LPUART0_BAUD = (LPUART_BAUD_OSR_16 | 0x00000004);
    LPUART0_STAT = (LPUART_STAT_TDRE | LPUART_STAT_TC);
    LPUART0_DATA = (LPUART_DATA_RXEMPT);
#endif
#if ((UARTS_AVAILABLE > 0) && (LPUARTS_AVAILABLE == 0)) || (defined LPUARTS_PARALLEL && (UARTS_AVAILABLE > 0))
    UART0_BDL    = 0x04;                                                 // UARTs
    UART0_S1     = (UART_S1_TDRE | UART_S1_TC);
    #if defined KINETIS_KL
    UART0_C4 = UART_C4_OSR_16;
    #endif
    #if !defined UART_WITHOUT_MODEM_CONTROL
    UART0_SFIFO  = 0xc0;
    UART0_RWFIFO = 0x01;
    UART0_WP7816T0 = 0x0a;
    UART0_WF7816 = 0x01;
    #endif
#endif
#if LPUARTS_AVAILABLE > 1
    LPUART1_BAUD = (LPUART_BAUD_OSR_16 | 0x00000004);
    LPUART1_STAT = (LPUART_STAT_TDRE | LPUART_STAT_TC);
    LPUART1_DATA = (LPUART_DATA_RXEMPT);
#elif UARTS_AVAILABLE > 1
    UART1_BDL    = 0x04;   
    UART1_S1     = 0xc0;
    #if defined KINETIS_KL
    UART1_C4 = UART_C4_OSR_16;
    #endif
    #if !defined UART_WITHOUT_MODEM_CONTROL
    UART1_SFIFO  = 0xc0;
    UART1_RWFIFO = 0x01;
    UART1_WP7816T0 = 0x0a;
    UART1_WF7816 = 0x01;
    #endif
#endif
#if UARTS_AVAILABLE > 2 || (LPUARTS_AVAILABLE == 2 && UARTS_AVAILABLE == 1)
    UART2_BDL    = 0x04;   
    UART2_S1     = 0xc0;
    #if defined KINETIS_KL
    UART2_C4 = UART_C4_OSR_16;
    #endif
    #if !defined UART_WITHOUT_MODEM_CONTROL
    UART2_SFIFO  = 0xc0;
    UART2_RWFIFO = 0x01;
    UART2_WP7816T0 = 0x0a;
    UART2_WF7816 = 0x01;
    #endif
#elif LPUARTS_AVAILABLE > 2
    LPUART2_BAUD = (LPUART_BAUD_OSR_16 | 0x00000004);
    LPUART2_STAT = (LPUART_STAT_TDRE | LPUART_STAT_TC);
    LPUART2_DATA = (LPUART_DATA_RXEMPT);
#endif
#if UARTS_AVAILABLE > 3
    UART3_BDL    = 0x04;   
    UART3_S1     = 0xc0;
    #if !defined UART_WITHOUT_MODEM_CONTROL
    UART3_SFIFO  = 0xc0;
    UART3_RWFIFO = 0x01;
    UART3_WP7816T0 = 0x0a;
    UART3_WF7816 = 0x01;
    #endif
#endif
#if UARTS_AVAILABLE > 4
    UART4_BDL    = 0x04;   
    UART4_S1     = 0xc0;
    #if !defined UART_WITHOUT_MODEM_CONTROL
    UART4_SFIFO  = 0xc0;
    UART4_RWFIFO = 0x01;
    UART4_WP7816T0 = 0x0a;
    UART4_WF7816 = 0x01;
    #endif
#endif
#if UARTS_AVAILABLE > 5
    UART5_BDL    = 0x04;   
    UART5_S1     = 0xc0;
    UART5_SFIFO  = 0xc0;
    UART5_RWFIFO = 0x01;
    UART5_WP7816T0 = 0x0a;
    UART5_WF7816 = 0x01;
#endif
#if defined LPSPI_SPI
    LPSPI0_VERID = 0x01000004;
    LPSPI0_SR = 0x00000001;
    LPSPI0_TCR = 0x0000001f;
    LPSPI0_RSR = 0x00000002;
    #if SPI_AVAILABLE > 1
    LPSPI1_VERID = 0x01000004;
    LPSPI1_SR = 0x00000001;
    LPSPI1_TCR = 0x0000001f;
    LPSPI1_RSR = 0x00000002;
    #endif
    #if SPI_AVAILABLE > 2
    LPSPI2_VERID = 0x01000004;
    LPSPI2_SR = 0x00000001;
    LPSPI2_TCR = 0x0000001f;
    LPSPI2_RSR = 0x00000002;
    #endif
#elif defined DSPI_SPI
    SPI0_MCR    = (SPI_MCR_DOZE | SPI_MCR_HALT);                         // DSPI
    SPI0_CTAR0  = 0x78000000;
    SPI0_CTAR1  = 0x78000000;
    SPI0_SR     = 0x02000000;
    #if SPI_AVAILABLE > 1
    SPI1_MCR    = (SPI_MCR_DOZE | SPI_MCR_HALT);
    SPI1_CTAR0  = 0x78000000;
    SPI1_CTAR1  = 0x78000000;
    SPI1_SR     = 0x02000000;
    #endif
    #if SPI_AVAILABLE > 2
    SPI2_MCR    = (SPI_MCR_DOZE | SPI_MCR_HALT);
    SPI2_CTAR0  = 0x78000000;
    SPI2_CTAR1  = 0x78000000;
    SPI2_SR     = 0x02000000;
    #endif
#else
    SPI0_C1     = SPI_C1_CPHA;
    SPI0_S      = SPI_S_SPTEF;
    #if !defined KINETIS_KL03
    SPI1_C1     = SPI_C1_CPHA;
    SPI1_S      = SPI_S_SPTEF;
    #endif
#endif
#if defined KINETIS_KL || defined KINETIS_KE
    #if defined MSCAN_CAN_INTERFACE
    MSCAN_CANCTL0 = MSCAN_CANCTL0_INITRQ;
    MSCAN_CANCTL1 = (MSCAN_CANCTL1_INITAK | MSCAN_CANCTL1_LISTEN);
    MSCAN_CANTFLG = (MSCAN_CANTFLG_TXE0 | MSCAN_CANTFLG_TXE1 | MSCAN_CANTFLG_TXE2);
    #endif
#else
    SDHC_PROCTL    = SDHC_PROCTL_EMODE_LITTLE;                           // SDHC
    SDHC_SYSCTL    = (SDHC_SYSCTL_SDCLKFS_256 | SDHC_SYSCTL_SDCLKEN);
    SDHC_IRQSTATEN = 0x117f013f;
    SDHC_WML       = 0x00100010;
    SDHC_VENDOR    = SDHC_VENDOR_EXTDMAEN;
    SDHC_HOSTVER   = 0x00001201;
    #if NUMBER_OF_CAN_INTERFACES > 0
    CAN0_MCR       = 0xd890000f;                                         // FlexCAN
    CAN0_RXGMASK   = 0xffffffff;
    CAN0_RX14MASK  = 0xffffffff;
    CAN0_RX15MASK  = 0xffffffff;
    CAN0_CTRL2     = 0x00c00000;
    CAN0_RXFGMASK  = 0xffffffff;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 1
    CAN1_MCR       = 0xd890000f;
    CAN1_RXGMASK   = 0xffffffff;
    CAN1_RX14MASK  = 0xffffffff;
    CAN1_RX15MASK  = 0xffffffff;
    CAN1_CTRL2     = 0x00c00000;
    CAN1_RXFGMASK  = 0xffffffff;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
    CAN2_MCR       = 0xd890000f;
    CAN2_RXGMASK   = 0xffffffff;
    CAN2_RX14MASK  = 0xffffffff;
    CAN2_RX15MASK  = 0xffffffff;
    CAN2_CTRL2     = 0x00c00000;
    CAN2_RXFGMASK  = 0xffffffff;
    #endif
#endif
#if defined KINETIS_K80                                                  // QSPI
    QuadSPI0_FLSHCR = 0x00000303;
    QuadSPI0_DLPR   = 0xaa553443;
    QuadSPI0_LUTKEY = 0x5af05af0;
    QuadSPI0_LCKCR  = 0x00000002;
#endif
#if !defined KINETIS_KL02
    #if !defined KINETIS_WITHOUT_RTC
        #if !defined KINETIS_KE || defined KINETIS_WITH_SRTC
            #if defined SUPPORT_RTC                                      // RTC
  //RTC_SR      = 0;                                                     // assume running
    RTC_SR      = RTC_SR_TIF;                                            // normally it would be running but we signal it as not so that the initialisation routine is exercised
            #else
    RTC_SR      = RTC_SR_TIF;
            #endif
    RTC_LR      = (RTC_LR_TCL | RTC_LR_CRL | RTC_LR_SRL | RTC_LR_LRL);
    RTC_IER     = (RTC_IER_TIIE | RTC_IER_TOIE | RTC_IER_TAIE);
        #endif
        #if defined KINETIS_KL
    SIM_SOPT1   = SIM_SOPT1_OSC32KSEL_LPO_1kHz;                          // assume retained over reset
        #elif !defined KINETIS_KE || defined KINETIS_WITH_SRTC
    RTC_RAR     = (RTC_RAR_TSRW | RTC_RAR_TPRW | RTC_RAR_TARW| RTC_RAR_TCRW | RTC_RAR_CRW | RTC_RAR_SRW | RTC_RAR_LRW | RTC_RAR_IERW);
        #endif
    #endif
    #if !defined KINETIS_KL && !defined KINETIS_KM && !defined KINETIS_KE && !defined CROSSBAR_SWITCH_LITE
    AXBS_CRS0 = 0x76543210;                                              // {34} default crossbar switch settings
    AXBS_CRS1 = 0x76543210;
    AXBS_CRS2 = 0x76543210;
    AXBS_CRS3 = 0x76543210;
    AXBS_CRS4 = 0x76543210;
    AXBS_CRS5 = 0x76543210;
    AXBS_CRS6 = 0x76543210;

    DMA_DCHPRI0    = 0;                                                  // default DMA channel priorities
    DMA_DCHPRI1    = 1;
    DMA_DCHPRI2    = 2;
    DMA_DCHPRI3    = 3;
    DMA_DCHPRI4    = 4;
    DMA_DCHPRI5    = 5;
    DMA_DCHPRI6    = 6;
    DMA_DCHPRI7    = 7;
    DMA_DCHPRI8    = 8;
    DMA_DCHPRI9    = 9;
    DMA_DCHPRI10   = 10;
    DMA_DCHPRI11   = 11;
    DMA_DCHPRI12   = 12;
    DMA_DCHPRI13   = 13;
    DMA_DCHPRI14   = 14;
    DMA_DCHPRI15   = 15;
    #endif
#endif
#if defined INTMUX0_AVAILABLE                                            // {46}
    INTMUX0_CH0_CSR = 0x000;
    INTMUX0_CH1_CSR = 0x100;
    INTMUX0_CH2_CSR = 0x200;
    INTMUX0_CH3_CSR = 0x300;
#endif
#if defined KINETIS_KE15 || defined KINETIS_KE18
    ADC0_SC1A = ADC_SC1A_ADCH_OFF;                                       // ADC0
    ADC0_SC1B = ADC_SC1A_ADCH_OFF;
    ADC0_CFG2 = 0x0000000c;
    ADC0_BASE_OFS = 0x00000040;
    ADC0_XOFS = 0x00000030;
    ADC0_YOFS = 0x00000037;
    ADC0_G = 0x000002f0;
    ADC0_UG = 0x00000004;
    ADC0_CLPX_OFS = 0x00000440;
    ADC0_CLP9_OFS = 0x00000240;
#elif defined KINETIS_KE
    ADC0_SC1 = ADC_SC1A_ADCH_OFF;
    ADC0_SC2 = ADC_SC2_FEMPTY;
#else
    ADC0_SC1A   = ADC_SC1A_ADCH_OFF;                                     // ADC0
    ADC0_SC1B   = ADC_SC1A_ADCH_OFF;
    ADC0_OFS    = 0x00000004;
    ADC0_PG     = 0x00008200;
    ADC0_MG     = 0x00008200;
    ADC0_CLPD   = 0x0000000a;
    ADC0_CLPS   = 0x00000020;
    ADC0_CLP4   = 0x00000200;
    ADC0_CLP3   = 0x00000100;
    ADC0_CLP2   = 0x00000080;
    ADC0_CLP1   = 0x00000040;
    ADC0_CLP0   = 0x00000020;
    ADC0_CLMD   = 0x0000000a;
    ADC0_CLMS   = 0x00000020;
    ADC0_CLM4   = 0x00000200;
    ADC0_CLM3   = 0x00000100;
    ADC0_CLM2   = 0x00000080;
    ADC0_CLM1   = 0x00000040;
    ADC0_CLM0   = 0x00000020;
#endif
#if ADC_CONTROLLERS > 1
    #if defined KINETIS_KE15 || defined KINETIS_KE18
    ADC1_SC1A = ADC_SC1A_ADCH_OFF;                                     // ADC1
    ADC1_SC1B = ADC_SC1A_ADCH_OFF;
    ADC1_CFG2 = 0x0000000c;
    ADC1_BASE_OFS = 0x00000040;
    ADC1_XOFS = 0x00000030;
    ADC1_YOFS = 0x00000037;
    ADC1_G = 0x000002f0;
    ADC1_UG = 0x00000004;
    ADC1_CLPX_OFS = 0x00000440;
    ADC1_CLP9_OFS = 0x00000240;
    #else
    ADC1_SC1A   = ADC_SC1A_ADCH_OFF;
    ADC1_SC1B   = ADC_SC1A_ADCH_OFF;
    ADC1_OFS    = 0x00000004;
    ADC1_PG     = 0x00008200;
    ADC1_MG     = 0x00008200;
    ADC1_CLPD   = 0x0000000a;
    ADC1_CLPS   = 0x00000020;
    ADC1_CLP4   = 0x00000200;
    ADC1_CLP3   = 0x00000100;
    ADC1_CLP2   = 0x00000080;
    ADC1_CLP1   = 0x00000040;
    ADC1_CLP0   = 0x00000020;
    ADC1_CLMD   = 0x0000000a;
    ADC1_CLMS   = 0x00000020;
    ADC1_CLM4   = 0x00000200;
    ADC1_CLM3   = 0x00000100;
    ADC1_CLM2   = 0x00000080;
    ADC1_CLM1   = 0x00000040;
    ADC1_CLM0   = 0x00000020;
    #endif
#endif
#if DAC_CONTROLLERS > 0 && !defined KINETIS_KE18
    DAC0_SR     = DAC_SR_DACBFRPTF;                                      // DAC
    DAC0_C2     = 0x0f;
    #if DAC_CONTROLLERS > 1
    DAC1_SR     = DAC_SR_DACBFRPTF;
    DAC1_C2     = 0x0f;
    #endif
#endif
#if defined RNG_AVAILABLE                                                // random number generator
    #if defined RANDOM_NUMBER_GENERATOR_B                                // {64}
    RNG_VER     = (RNG_VER_RNGB | 0x00000280);
    RNG_SR      = (RNG_SR_FIFO_SIZE_5 | RNG_SR_RS | RNG_SR_SLP | 0x00000001);
    #elif defined RANDOM_NUMBER_GENERATOR_A
    RNGA_SR     = (RNGA_SR_OREG_SIZE);
    #elif defined TRUE_RANDOM_NUMBER_GENERATOR                           // {48}
    TRNG0_MCTL = (TRNG_MCTL_PRGM | TRNG_MCTL_TSTOP_OK | TRNG_MCTL_SAMP_MODE_RAW);
    TRNG0_SCMISC = 0x00010022;
    TRNG0_PKRRNG = 0x000009a3;
    TRNG0_PKRMAX = 0x00006920;
    TRNG0_SDCTL = 0x0c8009c4;
    TRNG0_SBLIM = 0x0000003f;
    TRNG0_FRQMIN = 0x00000640;
    TRNG0_FRQMAX = 0x00006400;
    TRNG0_SCML = 0x010c0568;
    TRNG0_SCR1L = 0x00b20195;
    TRNG0_SCR2L = 0x007a00dc;
    TRNG0_SCR3L = 0x0058007d;
    TRNG0_SCR4L = 0x0040004b;
    TRNG0_SCR5L = 0x002e002f;
    TRNG0_SCR6L = 0x002e002f;
    TRNG0_INT_CTRL = 0xffffffff;
    TRNG0_VID1 = 0x00300100;
    #endif
#endif
    MCM_PLASC  = 0x001f;                                                  // {15}
    MCM_PLAMC  = 0x003f;
#if defined ETHERNET_AVAILABLE
    ECR        = 0xf0000000;                                             // ENET
    MIBC       = 0xc0000000;
    RCR        = 0x05ee0001;
    PAUR       = 0x00008808;
    OPD        = 0x00010000;
    ENET_RAEM  = 0x00000004;
    ENET_RAFL  = 0x00000004;
    ENET_TAEM  = 0x00000004;
    ENET_TAFL  = 0x00000008;
    ENET_TIPG  = 0x0000000c;
    ENET_FTRL  = 0x000007ff;
    ENET_ATPER = 0x3b9aca00;
#endif
#if PDB_AVAILABLE > 0
    PDB0_MOD   = 0x0000ffff;                                             // PDB {16}
    PDB0_IDLY  = 0x0000ffff;
#endif
#if I2C_AVAILABLE > 0
    I2C0_S      = I2C_TCF;                                               // I2C
    I2C0_A2     = 0xc2;
#endif
#if I2C_AVAILABLE > 1
    I2C1_S      = 0x80;
    I2C1_A2     = 0xc2;
#endif
#if I2C_AVAILABLE > 2
    I2C2_S      = 0x80;
    I2C2_A2     = 0xc2;
#endif
#if defined DEVICE_WITH_SLCD
    LCD_GCR     = 0x08350003;                                            // SLCD
#endif
    initial_input_state = fnKeyPadState(*port_inits++, _PORTA);          // {51} allow key input defaults override port input defaults
    GPIOA_PDIR  = (PORT_CAST)(ulPort_in_A = initial_input_state);        // set port inputs to default states
#if PORTS_AVAILABLE > 1
    initial_input_state = fnKeyPadState(*port_inits++, _PORTB);
    GPIOB_PDIR  = (PORT_CAST)(ulPort_in_B = initial_input_state);
#endif
#if PORTS_AVAILABLE > 2
    initial_input_state = fnKeyPadState(*port_inits++, _PORTC);
    GPIOC_PDIR  = (PORT_CAST)(ulPort_in_C = initial_input_state);
#endif
#if PORTS_AVAILABLE > 3
    initial_input_state = fnKeyPadState(*port_inits++, _PORTD);
    GPIOD_PDIR  = (PORT_CAST)(ulPort_in_D = initial_input_state);
#endif
#if PORTS_AVAILABLE > 4
    initial_input_state = fnKeyPadState(*port_inits++, _PORTE);
    GPIOE_PDIR  = (PORT_CAST)(ulPort_in_E = initial_input_state);
#endif
#if PORTS_AVAILABLE > 5
    initial_input_state = fnKeyPadState(*port_inits++, _PORTF);
    GPIOF_PDIR  = (PORT_CAST)(ulPort_in_F = initial_input_state);
#endif
#if PORTS_AVAILABLE > 6
    initial_input_state = fnKeyPadState(*port_inits++, _PORTG);
    GPIOG_PDIR = (PORT_CAST)(ulPort_in_G = initial_input_state);
#endif
#if PORTS_AVAILABLE > 7
    initial_input_state = fnKeyPadState(*port_inits++, _PORTH);
    GPIOH_PDIR = (PORT_CAST)(ulPort_in_H = initial_input_state);
#endif
#if PORTS_AVAILABLE > 8
    initial_input_state = fnKeyPadState(*port_inits++, _PORTI);
    GPIOI_PDIR = (PORT_CAST)(ulPort_in_I = initial_input_state);
#endif
#if defined _EXTERNAL_PORT_COUNT && (_EXTERNAL_PORT_COUNT > 0) && defined PORT_EXT0_DEFAULT_INPUT // {51}
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
#if !defined KINETIS_KL && !defined KINETIS_KE && (FLEX_TIMERS_AVAILABLE > 0)
    FTM0_MODE = FTM_MODE_WPDIS;                                          // FlexTimer
    FTM1_MODE = FTM_MODE_WPDIS;
    FTM2_MODE = FTM_MODE_WPDIS;
#endif
#if defined KINETIS_KL28
    FTM0_VERID = 0x05000007;
    FTM0_PARAM = 0x00101006;
    FTM1_VERID = 0x05000007;
    FTM1_PARAM = 0x00101006;
    FTM2_VERID = 0x05000007;
    FTM2_PARAM = 0x00101006;
#endif
#if defined SDRAM_CONTROLLER_AVAILABLE
    NFC_CMD1  = 0x30ff0000;                                              // {1} NAND flash controller
    NFC_CMD2  = 0x007ee000;
    NFC_RAR   = 0x11000000;
    NFC_SWAP  = 0x0ffe0ffe;

    DDR_CR00 = 0x20400000;                                               // {14} DDR1/2/LP SDRAM controller
    DDR_CR01 = 0x00020b10;
    DDR_CR20 = 0x0c000000;
    DDR_CR21 = 0x00000400;
    DDR_CR22 = 0x00000400;
    DDR_CR51 = 0x00000400;
    DDR_PAD_CTRL = 0x00000200;
#endif
#if defined KINETIS_K70                                                  // LCD
    LCDC_LCWHB = 0x10100ff;
    LCDC_LHCR = 0x04000000;
    LCDC_LVCR = 0x04000000;
    LCDC_LDCR = 0x80040060;
    LCDC_LGWDCR = 0x80040060;
#endif
#if defined HS_USB_AVAILABLE                                             // {12}
    USBHS_ID = 0xe461fa05;
    USBHS_HWGENERAL = 0x00000085;
    USBHS_HWHOST = 0x10020001;
    USBHS_HWDEVICE = 0x00000009;
    USBHS_HWTXBUF = 0x80070908;
    USBHS_HWRXBUF = 0x00000808;
    USBHS_HCIVERSION = 0x01000040;
    USBHS_HCSPARAMS = 0x00010011;
    USBHS_HCCPARAMS = 0x00000006;
    USBHS_DCIVERSION = 0x0001;
    USBHS_USBCMD = 0x00080000;
    USBHS_BURSTSIZE = 0x00008080;
    USBHS_PORTSC1 = 0x80000000;
    USBHS_OTGSC = 0x00001020;
    USBHS_USBMODE = 0x00005000;
    USBHS_EPCR0 = 0x00800080;
    #if defined KINETIS_WITH_USBPHY
    USBPHY_PWD = 0x001e1c00;
    USBPHY_TX = 0x10060607;
    USBPHY_CTRL = 0xc0000000;
    USBPHY_DEBUG = 0x7f180000;
    USBPHY_DEBUG1 = 0x00001000;
    USBPHY_VERSION = 0x04030000;
    USBPHY_PLL_SIC = 0x00012000;
    USBPHY_USB1_VBUS_DETECT = 0x00700004;
    USBPHY_ANACTRL = 0x00000402;
    USBPHY_USB1_LOOPBACK = 0x00550000;
    USBPHY_USB1_LOOPBACK_HSFSCNT = 0x00040010;
    #endif
#endif
#if defined CHIP_HAS_FLEXIO
    FLEXIO0_VERID = 0x01010001;
    FLEXIO0_PARAM = 0x04200808;
#endif
#if !defined KINETIS_KL && !defined KINETIS_KE
    CRC_CRC   = 0xffffffff;                                              // {6} CRC
    CRC_GPOLY = 0x00001021;                                              // default CRC-16 polynomial
#endif
#if defined LLWU_AVAILABLE && !defined KINETIS_KL03 && !defined KINETIS_KL05
    LLWU_RST = LLWU_RST_LLRSTE;
#endif
    fnSimPers();                                                         // synchronise peripheral states with default settings
#if defined SUPPORT_ADC
    for (j = 0; j < ADC_CONTROLLERS; j++) {
        for (i = 0; i < ADC_CHANNELS; i++) {
            usADC_values[j][i] = (unsigned short)*port_inits++;          // {2} prime initial ADC values
        }
        usADC_values[j][ADC_TEMP_SENSOR] = (unsigned short)((VTEMP_25_MV * 0xffff) / ADC_REFERENCE_VOLTAGE); // 25°C
        usADC_values[j][ADC_BANDGAP] = (unsigned short)((1800 / ADC_REFERENCE_VOLTAGE) * 0xffff); // 1.8V
        usADC_values[j][ADC_VREFH] = 0xffff;
    }
    fnEnterHW_table(ptrPorts);                                           
#else
    fnEnterHW_table(ucPortFunctions);
#endif
}

#if defined SIZE_OF_EEPROM  && (SIZE_OF_EEPROM == 256)                   // KE and KEA parts with EEPROM
    #define SIZE_OF_SIM_FLASH (SIZE_OF_FLASH + SIZE_OF_EEPROM)
#else
    #define SIZE_OF_SIM_FLASH (SIZE_OF_FLASH + PROGRAM_ONCE_AREA_SIZE)   // {7} internal memory plus program-once flash content
#endif
unsigned char ucFLASH[SIZE_OF_SIM_FLASH];

extern void fnInitialiseDevice(void *port_inits)
{
    memset(ucFLASH, 0xff, sizeof(ucFLASH));                              // we start with deleted FLASH memory contents (use memset() rather than uMemset() since the DMA controller may not be initialised yet)
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned long *)port_inits);                            // set device registers to startup condition (if not zerod)
}

extern void fnDeleteFlashSector(unsigned char *ptrSector)
{
#if (defined KINETIS_KE && defined KINETIS_KE_EEPROM && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0))
    if (ptrSector >= &ucFLASH[SIZE_OF_FLASH]) {                          // access is in EEPROM memory
        uMemset(ptrSector, 0xff, KE_EEPROM_GRANULARITY);
        return;
    }
#endif
    uMemset(ptrSector, 0xff, FLASH_GRANULARITY);                         // access in data flash
}


extern unsigned char *fnGetEEPROM(unsigned short usOffset);

extern unsigned char *fnGetFileSystemStart(int iMemory)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    return (fnGetEEPROM(uFILE_START));
#elif defined (SAVE_COMPLETE_FLASH)
    return (&ucFLASH[0]);
#else
    return (&ucFLASH[uFILE_START - FLASH_START_ADDRESS]);
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



// Transform physical access address to address in simulated FLASH
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)               
{
    unsigned char *ucSimulatedFlash;
#if defined FLEXFLASH_DATA                                               // {33}
    if (ucAdd >= (unsigned char *)FLEXNVM_START_ADDRESS) {               // access may be in FlexNMV area
        ucAdd -= (FLEXNVM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)); // move to contiguous simulation flash area
    }
#elif defined KINETIS_KE && defined KINETIS_KE_EEPROM && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)
    if (ucAdd >= (unsigned char *)KE_EEPROM_START_ADDRESS) {             // access may be in EEPROM area
        ucAdd -= (KE_EEPROM_START_ADDRESS - SIZE_OF_FLASH);              // move to contiguous simulation flash area
    }
#endif
    ucSimulatedFlash = ucAdd + (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    if (ucSimulatedFlash >= &ucFLASH[SIZE_OF_SIM_FLASH/* - PROGRAM_ONCE_AREA_SIZE*/]) { // {21} check flash bounds
        _EXCEPTION("Attempted access outside of internal Flash bounds!!!");
    }
    return (ucSimulatedFlash);                                           // location in simulated internal FLASH memory
}

// Transform simulated address in simulated FLASH back to physical address in real FLASH
//
extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)
{
    unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    return (ucAdd - sim_add);
}

extern unsigned char fnMapPortBit(unsigned long ulRealBit)
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

// Periodic tick (on timer 1) - dummy since timer simulation used now
//
extern void RealTimeInterrupt(void)
{
}

#if defined INTMUX0_AVAILABLE                                            // {46}
static void fnCallINTMUX(int iChannel, int iPeripheralReference, unsigned char *ptrVectLocation)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    KINETIS_INTMUX *ptrINTMUX = (KINETIS_INTMUX *)INTMUX0_BLOCK;;
    ptrINTMUX += iChannel;
    if ((ptrINTMUX->INTMUX_CHn_IER_31_0 & (1 << iPeripheralReference)) != 0) {
        ptrINTMUX->INTMUX_CHn_IPR_31_0 |= (1 << iPeripheralReference);
        ptrINTMUX->INTMUX_CHn_CSR |= (INTMUX_CSR_IRQP);                  // interrupt pending
        ptrINTMUX->INTMUX_CHn_VEC = (ptrVectLocation - (unsigned char *)ptrVect); // calculate the vector offset location
        switch (iChannel) {
        case 0:
            ptrVect->processor_interrupts.irq_INTMUX0_0();               // call the interrupt handler
            break;
        case 1:
            ptrVect->processor_interrupts.irq_INTMUX0_1();               // call the interrupt handler
            break;
        case 2:
            ptrVect->processor_interrupts.irq_INTMUX0_2();               // call the interrupt handler
            break;
        case 3:
            ptrVect->processor_interrupts.irq_INTMUX0_3();               // call the interrupt handler
            break;
        default:
            _EXCEPTION("Invalid INTMUX0 channel!");
            break;
        }
    }
}
#endif

// Check whether a particular interrupt is enabled in the NVIC (and set its pending flag)
//
static int fnGenInt(int iIrqID)
{
    if (iIrqID < 32) {
        IRQ0_31_SPR |= (1 << iIrqID);
        IRQ0_31_CPR = IRQ0_31_SPR;
        if ((IRQ0_31_SER & (1 << iIrqID)) != 0) {                        // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 64) {
        IRQ32_63_SPR |= (1 << (iIrqID - 32));
        IRQ32_63_CPR = IRQ32_63_SPR;
        if ((IRQ32_63_SER & (1 << (iIrqID - 32))) != 0) {                // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 96) {
        IRQ64_95_SPR |= (1 << (iIrqID - 64));
        IRQ64_95_CPR = IRQ64_95_SPR;
        if ((IRQ64_95_SER & (1 << (iIrqID - 64))) != 0) {                // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 128) {
        IRQ96_127_SPR |= (1 << (iIrqID - 96));
        IRQ96_127_CPR = IRQ96_127_SPR;
        if ((IRQ96_127_SER & (1 << (iIrqID - 96))) != 0) {               // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 160) {
        IRQ128_159_SPR |= (1 << (iIrqID - 128));
        IRQ128_159_CPR = IRQ128_159_SPR;
        if ((IRQ128_159_SER & (1 << (iIrqID - 128))) != 0) {             // if interrupt is not disabled
            return 1;
        }
    }
    else if (iIrqID < 192) {
        IRQ160_191_SPR |= (1 << (iIrqID - 164));
        IRQ160_191_CPR = IRQ160_191_SPR;
        if ((IRQ160_191_SER & (1 << (iIrqID - 164))) != 0) {             // if interrupt is not disabled
            return 1;
        }
    }
    return 0;
}

// Check whether a particular interrupt is enabled in the NVIC
//
extern int fnResetNVICInt(int iIrqID)
{
    if (iIrqID < 32) {
        IRQ0_31_SPR &= ~(1 << iIrqID);
    }
    else if (iIrqID < 64) {
        IRQ32_63_SPR &= ~(1 << (iIrqID - 32));
    }
    else if (iIrqID < 96) {
        IRQ64_95_SPR &= ~(1 << (iIrqID - 64));
    }
    else if (iIrqID < 128) {
        IRQ96_127_SPR &= ~(1 << (iIrqID - 96));
    }
    else if (iIrqID < 160) {
        IRQ128_159_SPR |= (1 << (iIrqID - 128));
    }
    else if (iIrqID < 192) {
        IRQ160_191_SPR &= ~(1 << (iIrqID - 164));
    }
    return 0;
}

// Get the present state of a port
//
extern unsigned long fnGetPresentPortState(int portNr)
{
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    int iShift;
#else
    #define iShift 0
#endif
    portNr--;
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    iShift = ((portNr % 4) * 8);
    portNr /= 4;                                                         // convert 8 bit port to 32 bit port
#endif
    switch (portNr) {
    case _PORTA:
        return (((GPIOA_PDDR & GPIOA_PDOR) | (~GPIOA_PDDR & ulPort_in_A)) >> iShift);
#if PORTS_AVAILABLE > 1
    case _PORTB:
        return (((GPIOB_PDDR & GPIOB_PDOR) | (~GPIOB_PDDR & ulPort_in_B)) >> iShift);
#endif
#if PORTS_AVAILABLE > 2
    case _PORTC:
        return ((GPIOC_PDDR & GPIOC_PDOR) | (~GPIOC_PDDR & ulPort_in_C));  
#endif
#if PORTS_AVAILABLE > 3
    case _PORTD:
        return ((GPIOD_PDDR & GPIOD_PDOR) | (~GPIOD_PDDR & ulPort_in_D));
#endif
#if PORTS_AVAILABLE > 4
    case _PORTE:
        return ((GPIOE_PDDR & GPIOE_PDOR) | (~GPIOE_PDDR & ulPort_in_E));
#endif
#if PORTS_AVAILABLE > 5
    case _PORTF:
        return ((GPIOF_PDDR & GPIOF_PDOR) | (~GPIOF_PDDR & ulPort_in_F));
#endif
#if PORTS_AVAILABLE > 6
    case _PORTG:
        return ((GPIOG_PDDR & GPIOG_PDOR) | (~GPIOG_PDDR & ulPort_in_G));
#endif
#if PORTS_AVAILABLE > 7
    case _PORTH:
        return ((GPIOH_PDDR & GPIOH_PDOR) | (~GPIOH_PDDR & ulPort_in_H));
#endif
#if PORTS_AVAILABLE > 8
    case _PORTI:
        return ((GPIOI_PDDR & GPIOI_PDOR) | (~GPIOI_PDDR & ulPort_in_I));
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
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

// Get the present state of a port direction
//
extern unsigned long fnGetPresentPortDir(int portNr)
{
    unsigned long ulConnectedGPIO;
    unsigned long ulCheckedPin;
    unsigned long ulBit = 0x00000001;
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    int iShift;
#else
    unsigned long *ptrPCR = (unsigned long *)PORT0_BLOCK;
#endif
    portNr--;
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    iShift = ((portNr % 4) * 8);
    portNr /= 4;                                                         // convert 8 bit port to 32 bit port
#else
    ptrPCR += (portNr * (sizeof(KINETIS_PORT)/sizeof(unsigned long)));
#endif
    switch (portNr) {
    case _PORTA:
        ulCheckedPin = ulConnectedGPIO = (GPIOA_PDDR >> iShift);
        break;
#if PORTS_AVAILABLE > 1
    case _PORTB:
        ulCheckedPin = ulConnectedGPIO = (GPIOB_PDDR >> iShift);
        break;
#endif
#if PORTS_AVAILABLE > 2
    case _PORTC:
        ulCheckedPin = ulConnectedGPIO = GPIOC_PDDR;
        break;
#endif
#if PORTS_AVAILABLE > 3
    case _PORTD:
        ulCheckedPin = ulConnectedGPIO = GPIOD_PDDR;
        break;
#endif
#if PORTS_AVAILABLE > 4
    case _PORTE:
        ulCheckedPin = ulConnectedGPIO = GPIOE_PDDR;
        break;
#endif
#if PORTS_AVAILABLE > 5
    case _PORTF:
        ulCheckedPin = ulConnectedGPIO = GPIOF_PDDR;
        break;
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    case _PORT_EXT_0:                                                    // {8} external ports extensions
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
    while (ulCheckedPin != 0) {
#if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
        if ((*ptrPCR & PORT_MUX_MASK) != PORT_MUX_GPIO) {                // only connected port bits are considered as outputs
            ulConnectedGPIO &= ~ulBit;
        }
        ptrPCR++;
#endif
        ulCheckedPin &= ~ulBit;
        ulBit <<= 1;
    }
    return (ulConnectedGPIO);                                            // connected ports configured as GPIO outputs
}


extern unsigned long fnGetPresentPortPeriph(int portNr)
{
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    int iShift;
#endif
    portNr--;
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    iShift = ((portNr % 4) * 8);
    portNr /= 4;                                                         // convert 8 bit port to 32 bit port
#endif
    switch (portNr) {
    case _PORTA:
        return (ulPeripherals[0] >> iShift);
    case _PORTB:
        return (ulPeripherals[1] >> iShift);
    case _PORTC:
        return (ulPeripherals[2]);
    case _PORTD:
        return (ulPeripherals[3]);
    case _PORTE:
        return (ulPeripherals[4]);
#if PORTS_AVAILABLE > 5
    case _PORTF:
        return (ulPeripherals[5]);
#endif
#if PORTS_AVAILABLE > 6
    case _PORTG:
        return (ulPeripherals[6]);
#endif
#if PORTS_AVAILABLE > 7
    case _PORTH:
        return (ulPeripherals[7]);
#endif
#if PORTS_AVAILABLE > 8
    case _PORTI:
        return (ulPeripherals[8]);
#endif
    default:
        return 0;
    }
}

extern unsigned long fnCheckPortRead(int iPortRef, unsigned long ulValue)
{
#if defined KINETIS_WITH_PCC
    volatile unsigned long *ptrPCC = PCC_PORT_ADDR;
    ptrPCC += iPortRef;
    if ((*ptrPCC & PCC_CGC) == 0) {                                      // if the port is not powered up
        _EXCEPTION("Reading from a port that is not powered up!!!!!");
    }
#elif defined SIM_SCGC5
    if ((SIM_SCGC5 & (SIM_SCGC5_PORTA << iPortRef)) == 0) {              // if the port is not powered up
        _EXCEPTION("Reading from a port that is not powered up!!!!!");
    }
#endif
    return ulValue;
}


// See whether there has been a port change which should be displayed
//
extern int fnPortChanges(int iForce)
{
    int iRtn = iFlagRefresh;
    static unsigned long ulPortVal0 = 0, ulPortVal1 = 0, ulPortVal2 = 0, ulPortVal3 = 0, ulPortVal4 = 0;
    static unsigned long ulPortFunction0 = 0, ulPortFunction1 = 0, ulPortFunction2 = 0, ulPortFunction3 = 0, ulPortFunction4 = 0;
#if PORTS_AVAILABLE > 5
    static unsigned long ulPortFunction5 = 0; 
    static unsigned long ulPortVal5 = 0;
#endif
#if PORTS_AVAILABLE > 6
    static unsigned long ulPortFunction6 = 0;
    static unsigned long ulPortVal6 = 0;
#endif
#if PORTS_AVAILABLE > 7
    static unsigned long ulPortFunction7 = 0;
    static unsigned long ulPortVal7 = 0;
#endif
#if PORTS_AVAILABLE > 8
    static unsigned long ulPortFunction8 = 0;
    static unsigned long ulPortVal8 = 0;
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    static unsigned long ulPortExtValue[_EXTERNAL_PORT_COUNT] = {0};
    int iExPort = 0;
#endif
    unsigned long ulNewValue;
    unsigned long ulNewPortPer;
    iFlagRefresh = 0;

    ulNewValue = fnGetPresentPortState(_PORTA + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTA + 1);
    if ((ulNewValue != ulPortVal0) || (ulNewPortPer != ulPortFunction0)) {
        ulPortVal0 = ulNewValue;
        ulPortFunction0 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
    ulNewValue = fnGetPresentPortState(_PORTB + 1);
    ulNewPortPer = fnGetPresentPortPeriph(PORTB + 1);
    if ((ulNewValue != ulPortVal1) || (ulNewPortPer != ulPortFunction1)) {
        ulPortVal1 = ulNewValue;
        ulPortFunction1 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORTC + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTC + 1);
    if ((ulNewValue != ulPortVal2) || (ulNewPortPer != ulPortFunction2)) {
        ulPortVal2 = ulNewValue;
        ulPortFunction2 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORTD + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTD + 1);
    if ((ulNewValue != ulPortVal3) || (ulNewPortPer != ulPortFunction3)) {
        ulPortVal3 = ulNewValue;
        ulPortFunction3 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }   
    ulNewValue = fnGetPresentPortState(_PORTE + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTE + 1);
    if ((ulNewValue != ulPortVal4) || (ulNewPortPer != ulPortFunction4)) {
        ulPortVal4 = ulNewValue;
        ulPortFunction4 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#if PORTS_AVAILABLE > 5
    ulNewValue = fnGetPresentPortState(_PORTF + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTF + 1);
    if ((ulNewValue != ulPortVal5) || (ulNewPortPer != ulPortFunction5)) {
        ulPortVal5 = ulNewValue;
        ulPortFunction5 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#endif
#if PORTS_AVAILABLE > 6
    ulNewValue = fnGetPresentPortState(_PORTG + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTG + 1);
    if ((ulNewValue != ulPortVal6) || (ulNewPortPer != ulPortFunction6)) {
        ulPortVal6 = ulNewValue;
        ulPortFunction6 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#endif
#if PORTS_AVAILABLE > 7
    ulNewValue = fnGetPresentPortState(_PORTH + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTH + 1);
    if ((ulNewValue != ulPortVal7) || (ulNewPortPer != ulPortFunction7)) {
        ulPortVal7 = ulNewValue;
        ulPortFunction7 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#endif
#if PORTS_AVAILABLE > 8
    ulNewValue = fnGetPresentPortState(_PORTI + 1);
    ulNewPortPer = fnGetPresentPortPeriph(_PORTI + 1);
    if ((ulNewValue != ulPortVal8) || (ulNewPortPer != ulPortFunction8)) {
        ulPortVal8 = ulNewValue;
        ulPortFunction8 = ulNewPortPer;
        iRtn |= PORT_CHANGE;
    }
#endif
#if defined _EXTERNAL_PORT_COUNT && _EXTERNAL_PORT_COUNT > 0             // {8}
    while (iExPort < _EXTERNAL_PORT_COUNT) {                             // external ports extensions
        ulNewValue = fnGetPresentPortState(iExPort + (_PORT_EXT_0 + 1));
        if (ulNewValue != ulPortExtValue[iExPort]) {
            ulPortExtValue[iExPort] = ulNewValue;
            iRtn |= PORT_CHANGE;
        }
        iExPort++;
    }
#endif
    return iRtn;
}

#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
static void fnHandleIRQ(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
    switch (iPort) {                                                     // check ports that have potential interrupt functions
    case KE_PORTA:
        if ((ulChangedBit & KE_PORTA_BIT5) != 0) {
            if ((SIM_SOPT0 & SIM_SOPT_RSTPE) == 0) {                     // PTA5 not programmed as reset pin
    #if defined SIM_PINSEL_IRQPS_PTI6
                if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) != SIM_PINSEL_IRQPS_PTA5) {
                    return;
                }
    #endif
                break;                                                   // valid IRQ input so we check the input settings
            }
        }
        return;
    #if defined SIM_PINSEL_IRQPS_PTI6
    case KE_PORTI:
        switch (SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) {
        case SIM_PINSEL_IRQPS_PTI0:
            if (ulChangedBit != KE_PORTI_BIT0) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI1:
            if (ulChangedBit != KE_PORTI_BIT1) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI2:
            if (ulChangedBit != KE_PORTI_BIT2) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI3:
            if (ulChangedBit != KE_PORTI_BIT3) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI4:
            if (ulChangedBit != KE_PORTI_BIT4) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI5:
            if (ulChangedBit != KE_PORTI_BIT5) {
                return;
            }
            break;
        case SIM_PINSEL_IRQPS_PTI6:
            if (ulChangedBit != KE_PORTI_BIT6) {
                return;
            }
            break;
        default:
            return;
        }
        break;
    #endif
    default:
        return;
    }
    // IRQ input has changed so we check to see whether the change/state matches with the programmed one
    //
    if ((IRQ_SC & IRQ_SC_IRQIE) != 0) {                                  // interrupt is enabled
        if ((IRQ_SC & IRQ_SC_IRQEDG) != 0) {                             // high or rising edge sensitive
            if ((ulNewState & ulChangedBit) == 0) {                      // input has changed to '0'
                return;
            }
        }
        else {                                                           // low or falling edge sensitive
            if ((ulNewState & ulChangedBit) != 0) {                      // input has changed to '1'
                return;
            }
        }
        IRQ_SC |= IRQ_SC_IRQF;                                           // set the interrupt flag
        if (fnGenInt(irq_IRQ_ID) != 0) {                                 // if the interrupt is enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_IRQ();                     // call port interrupt handler
        }
    }
}

static void fnHandleKBI(int iController, int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
    #if KBI_WIDTH == 32
    unsigned long KBI_input = 0;
    #else
    unsigned char KBI_input = 0;
    #endif
    switch (iPort) {                                                     // check ports that have potential interrupt functions
    case KE_PORTA:
        if (iController == 0) {
            if ((ulChangedBit & KE_PORTA_BIT0) != 0) {
                if (ucPortFunctions[iPort][0] == PA_0_KBI0_P0) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x01;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTA_BIT1) != 0) {
                if (ucPortFunctions[iPort][1] == PA_1_KBI0_P1) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x02;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTA_BIT2) != 0) {
                if (ucPortFunctions[iPort][2] == PA_2_KBI0_P2) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x04;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTA_BIT3) != 0) {
                if (ucPortFunctions[iPort][3] == PA_3_KBI0_P3) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x08;
                    break;
                }
            }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
            else if ((ulChangedBit & KE_PORTA_BIT4) != 0) {
                if (ucPortFunctions[iPort][4] == PA_4_KBI0_P4) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTA_BIT5) != 0) {
                if (ucPortFunctions[iPort][5] == PA_5_KBI0_P5) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTA_BIT6) != 0) {
                if (ucPortFunctions[iPort][6] == PA_6_KBI0_P6) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTA_BIT7) != 0) {
                if (ucPortFunctions[iPort][7] == PA_7_KBI0_P7) {         // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
            }
    #endif
            else if ((ulChangedBit & KE_PORTB_BIT0) != 0) {
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort + 1][0] == PB_0_KBI0_P8) {     // this input is programmed as keyboard interrupt
                    KBI_input = 0x100;
                    break;
                }
    #else
                if (ucPortFunctions[iPort + 1][0] == PB_0_KBI0_P4) {     // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
    #endif
            }
            else if ((ulChangedBit & KE_PORTB_BIT1) != 0) {
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort + 1][1] == PB_1_KBI0_P9) {     // this input is programmed as keyboard interrupt
                    KBI_input = 0x200;
                    break;
                }
    #else
                if (ucPortFunctions[iPort + 1][1] == PB_1_KBI0_P5) {     // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
    #endif
            }
            else if ((ulChangedBit & KE_PORTB_BIT2) != 0) {
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort + 1][2] == PB_2_KBI0_P10) {    // this input is programmed as keyboard interrupt
                    KBI_input = 0x400;
                    break;
                }
    #else
                if ((ucPortFunctions[iPort + 1][2] == PB_2_KBI0_P6) != 0) { // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
    #endif
            }
            else if ((ulChangedBit & KE_PORTB_BIT3) != 0) {
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                if (ucPortFunctions[iPort + 1][3] == PB_3_KBI0_P11) {    // this input is programmed as keyboard interrupt
                    KBI_input = 0x800;
                    break;
                }
    #else
                if (ucPortFunctions[iPort + 1][3] == PB_3_KBI0_P7) {     // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
    #endif
            }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
            else if ((ulChangedBit & KE_PORTB_BIT4) != 0) {
                if (ucPortFunctions[iPort + 1][4] == PB_4_KBI0_P12) {    // this input is programmed as keyboard interrupt
                    KBI_input = 0x1000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTB_BIT5) != 0) {
                if (ucPortFunctions[iPort + 1][5] == PB_5_KBI0_P13) {    // this input is programmed as keyboard interrupt
                    KBI_input = 0x2000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTB_BIT6) != 0) {
                if (ucPortFunctions[iPort + 1][6] == PB_6_KBI0_P14) {    // this input is programmed as keyboard interrupt
                    KBI_input = 0x4000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTB_BIT7) != 0) {
                if (ucPortFunctions[iPort + 1][7] == PB_7_KBI0_P15) {    // this input is programmed as keyboard interrupt
                    KBI_input = 0x8000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT0) != 0) {
                if (ucPortFunctions[iPort][16] == PC_0_KBI0_P16) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x10000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT1) != 0) {
                if (ucPortFunctions[iPort][17] == PC_1_KBI0_P17) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x20000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT2) != 0) {
                if (ucPortFunctions[iPort][18] == PC_2_KBI0_P18) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x40000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT3) != 0) {
                if (ucPortFunctions[iPort][19] == PC_3_KBI0_P19) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x80000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT4) != 0) {
                if (ucPortFunctions[iPort][20] == PC_4_KBI0_P20) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x100000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT5) != 0) {
                if (ucPortFunctions[iPort][21] == PC_5_KBI0_P21) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x200000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT6) != 0) {
                if (ucPortFunctions[iPort][22] == PC_6_KBI0_P22) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x400000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT7) != 0) {
                if (ucPortFunctions[iPort][23] == PC_7_KBI0_P23) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x800000;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT0) {
                if (ucPortFunctions[iPort][24] == PD_0_KBI0_P24) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x1000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT1) != 0) {
                if (ucPortFunctions[iPort][25] == PD_1_KBI0_P25) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x2000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT2) != 0) {
                if (ucPortFunctions[iPort][26] == PD_2_KBI0_P26) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x4000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT3) != 0) {
                if (ucPortFunctions[iPort][27] == PD_3_KBI0_P27) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x8000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT4) != 0) {
                if (ucPortFunctions[iPort][28] == PD_4_KBI0_P28) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x10000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT5) != 0) {
                if (ucPortFunctions[iPort][29] == PD_5_KBI0_P29) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x20000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT6) != 0) {
                if (ucPortFunctions[iPort][30] == PD_6_KBI0_P30) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x40000000;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT7) != 0) {
                if (ucPortFunctions[iPort][31] == PD_7_KBI0_P31) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x80000000;
                    break;
                }
            }
    #endif
        }
    #if KBIS_AVAILABLE > 1 &&  !defined KINETIS_KE04 && !defined KINETIS_KE06 && !defined KINETIS_KEA64 && !defined KINETIS_KEA128
        else {
        #if defined KINETIS_KEA8
            if ((ulChangedBit & KE_PORTB_BIT4) != 0) {
                if (ucPortFunctions[iPort][12] == PB_4_KBI1_P6) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTB_BIT5) != 0) {
                if (ucPortFunctions[iPort][13] == PB_5_KBI1_P7) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT0) != 0) {
                if (ucPortFunctions[iPort][16] == PC_0_KBI1_P2) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x04;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT1) != 0) {
                if (ucPortFunctions[iPort][17] == PC_1_KBI1_P3) {       // this input is programmed as keyboard interrupt
                    KBI_input = 0x08;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT2) != 0) {
                if (ucPortFunctions[iPort][18] == PC_2_KBI1_P4) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT3) != 0) {
                if (ucPortFunctions[iPort][19] == PC_3_KBI1_P5) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT4) != 0) {
                if (ucPortFunctions[iPort][20] == PC_4_KBI1_P0) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x01;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTC_BIT5) != 0) {
                if (ucPortFunctions[iPort][21] == PC_5_KBI1_P1) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x02;
                    break;
                }
            }
        #endif
            if ((ulChangedBit & KE_PORTD_BIT0) != 0) {
                if (ucPortFunctions[iPort][24] == PD_0_KBI1_P0) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x01;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT1) != 0) {
                if (ucPortFunctions[iPort][25] == PD_1_KBI1_P1) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x02;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT2) {
                if (ucPortFunctions[iPort][26] == PD_1_KBI1_P1) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x04;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT3) != 0) {
                if (ucPortFunctions[iPort][27] == PD_3_KBI1_P3) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x08;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT4) != 0) {
                if (ucPortFunctions[iPort][28] == PD_4_KBI1_P4) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x10;
                    break;
                }
            }
            else if (ulChangedBit & KE_PORTD_BIT5) {
                if (ucPortFunctions[iPort][29] == PD_5_KBI1_P5) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x20;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT6) != 0) {
                if (ucPortFunctions[iPort][30] == PD_6_KBI1_P6) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x40;
                    break;
                }
            }
            else if ((ulChangedBit & KE_PORTD_BIT7) != 0) {
                if (ucPortFunctions[iPort][31] == PD_7_KBI1_P7) {        // this input is programmed as keyboard interrupt
                    KBI_input = 0x80;
                    break;
                }
            }
        }
    #endif
        return;
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
    case KE_PORTE:
        if ((ulChangedBit & KE_PORTE_BIT0) != 0) {
            if (ucPortFunctions[iPort][0] == PE_0_KBI1_P0) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000001;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTE_BIT1) != 0) {
            if (ucPortFunctions[iPort][1] == PE_1_KBI1_P1) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000002;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTE_BIT2) != 0) {
            if (ucPortFunctions[iPort][2] == PE_2_KBI1_P2) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000004;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTE_BIT3) != 0) {
            if (ucPortFunctions[iPort][3] == PE_3_KBI1_P3) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000008;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTE_BIT4) != 0) {
            if (ucPortFunctions[iPort][4] == PE_4_KBI1_P4) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000010;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTE_BIT5) != 0) {
            if (ucPortFunctions[iPort][5] == PE_5_KBI1_P5) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000020;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTE_BIT6) != 0) {
            if (ucPortFunctions[iPort][6] == PE_6_KBI1_P6) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000040;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTE_BIT7) {
            if (ucPortFunctions[iPort][7] == PE_7_KBI1_P7) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000080;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT0) {
            if (ucPortFunctions[iPort][8] == PF_0_KBI1_P8) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000100;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT1) {
            if (ucPortFunctions[iPort][9] == PF_1_KBI1_P9) {             // this input is programmed as keyboard interrupt
                KBI_input = 0x00000200;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT2) {
            if (ucPortFunctions[iPort][10] == PF_2_KBI1_P10) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000400;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT3) {
            if (ucPortFunctions[iPort][11] == PF_3_KBI1_P11) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00000800;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT4) {
            if (ucPortFunctions[iPort][12] == PF_4_KBI1_P12) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00001000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT5) {
            if (ucPortFunctions[iPort][13] == PF_5_KBI1_P13) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00002000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT6) {
            if (ucPortFunctions[iPort][14] == PF_6_KBI1_P14) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00004000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTF_BIT7) {
            if (ucPortFunctions[iPort][15] == PF_7_KBI1_P15) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00008000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT0) {
            if (ucPortFunctions[iPort][16] == PG_0_KBI1_P16) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00010000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT1) {
            if (ucPortFunctions[iPort][17] == PG_1_KBI1_P17) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00020000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT2) {
            if (ucPortFunctions[iPort][18] == PG_2_KBI1_P18) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00040000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT3) {
            if (ucPortFunctions[iPort][19] == PG_3_KBI1_P19) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00080000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT4) {
            if (ucPortFunctions[iPort][20] == PG_4_KBI1_P20) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00100000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT5) {
            if (ucPortFunctions[iPort][21] == PG_5_KBI1_P21) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00200000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT6) {
            if (ucPortFunctions[iPort][22] == PG_6_KBI1_P22) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00400000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTG_BIT7) {
            if (ucPortFunctions[iPort][23] == PG_7_KBI1_P23) {           // this input is programmed as keyboard interrupt
                KBI_input = 0x00800000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT0) != 0) {
            if (ucPortFunctions[7][0] == PH_0_KBI1_P24) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x01000000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT1) != 0) {
            if (ucPortFunctions[7][1] == PH_1_KBI1_P25) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x02000000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT2) != 0) {
            if (ucPortFunctions[7][2] == PH_2_KBI1_P26) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x04000000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT3) != 0) {
            if (ucPortFunctions[7][3] == PH_3_KBI1_P27) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x08000000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT4) != 0) {
            if (ucPortFunctions[7][4] == PH_4_KBI1_P28) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x10000000;
                break;
            }
        }
        else if (ulChangedBit & KE_PORTH_BIT5) {
            if (ucPortFunctions[7][5] == PH_5_KBI1_P29) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x20000000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT6) != 0) {
            if (ucPortFunctions[7][6] == PH_6_KBI1_P30) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x40000000;
                break;
            }
        }
        else if ((ulChangedBit & KE_PORTH_BIT7) != 0) {
            if (ucPortFunctions[7][7] == PH_7_KBI1_P31) {                // this input is programmed as keyboard interrupt
                KBI_input = 0x80000000;
                break;
            }
        }
        return;
#endif
    default:
        return;
    }
    // A valid keyboard interrupt input has changed so we check to see whether the change/state matches with the programmed one
    //
    if (iController == 0) {
        if ((KBI0_SC & KBI_SC_KBIE) != 0) {                              // main KBI interrupt enabled
            if ((KBI0_ES & KBI_input) != 0) {                            // high or rising edge sensitive
                if ((ulNewState & ulChangedBit) == 0) {                  // input has changed to '0'
                    return;
                }
            }
            else {                                                       // low or falling edge sensitive
                if ((ulNewState & ulChangedBit) != 0) {                  // input has changed to '1'
                    return;
                }
            }
        }
        KBI0_SC |= KBI_SC_KBF;                                           // interrupt request detected
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        KBI0_SP |= ulChangedBit;                                         // flag the source that caused the interrupt
    #endif
        if (fnGenInt(irq_KBI0_ID) != 0) {                                // if the interrupt is enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_KBI0();                    // call port interrupt handler
        }
    }
    #if KBIS_AVAILABLE > 1
    else {
        if ((KBI1_SC & KBI_SC_KBIE) != 0) {                              // main KBI interrupt enabled
            if ((KBI1_ES & KBI_input) != 0) {                            // high or rising edge sensitive
                if ((ulNewState & ulChangedBit) == 0) {                  // input has changed to '0'
                    return;
                }
            }
            else {                                                       // low or falling edge sensitive
                if ((ulNewState & ulChangedBit) != 0) {                  // input has changed to '1'
                    return;
                }
            }
        }
        KBI1_SC |= KBI_SC_KBF;                                           // interrupt request detected
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
        KBI1_SP |= ulChangedBit;                                         // flag the source that caused the interrupt
    #endif
        if (fnGenInt(irq_KBI1_ID) != 0) {                                // if the interrupt is enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_KBI1();                    // call port interrupt handler
        }
    }
    #endif
}
#endif

#if defined SUPPORT_ADC                                                  // {2}
static unsigned short fnConvertSimADCvalue(KINETIS_ADC_REGS *ptrADC, unsigned short usStandardValue)
{
    #if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    switch (ptrADC->ADC_SC3 & (ADC_CFG1_MODE_MASK)) {
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit
        usStandardValue >>= 4;
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit
        usStandardValue >>= 6;
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit
        usStandardValue >>= 8;
        break;
    }
    #else
    switch (ptrADC->ADC_CFG1 & ADC_WORD_LENGTH_MASK) {
        #if defined KINETIS_KE15
    default:
        _EXCEPTION("KE15's ADC doesn't support 16 bit mode");
        break;
        #else
    case ADC_CFG1_MODE_16:                                               // conversion mode - single-ended 16 bit or differential 16 bit
        break;
        #endif
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit or differential 13 bit
        usStandardValue >>= 4;                                           // right aligned
        if ((ptrADC->ADC_SC1A & ADC_SC1A_DIFF) != 0) {                   // differential mode
            if ((usStandardValue & 0x0800) != 0) {
                usStandardValue |= 0xf000;                               // sign extend
            }
        }
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit or differential 11 bit
        usStandardValue >>= 6;                                           // right aligned
        if ((ptrADC->ADC_SC1A & ADC_SC1A_DIFF) != 0) {                   // differential mode
            if ((usStandardValue & 0x0200) != 0) {
                usStandardValue |= 0xfc00;                               // sign extend
            }
        }
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit or differential 9 bit
        usStandardValue >>= 8;                                           // right aligned
        if ((ptrADC->ADC_SC1A & ADC_SC1A_DIFF) != 0) {                   // differential mode
            if ((usStandardValue & 0x0080) != 0) {
                usStandardValue |= 0xff00;                               // sign extend
            }
        }
        break;
    }
    #endif
    return usStandardValue;
}

static void fnSimADC(int iChannel)
{
    KINETIS_ADC_REGS *ptrADC;
    int iValue = 0;
    unsigned short usADCvalue;
    if (iChannel == 0) {
        ptrADC = (KINETIS_ADC_REGS *)ADC0_BLOCK;
    }
    #if ADC_CONTROLLERS > 1
    else if (iChannel == 1) {
        ptrADC = (KINETIS_ADC_REGS *)ADC1_BLOCK;
    }
    #endif
    #if ADC_CONTROLLERS > 2
    else if (iChannel == 2) {
        ptrADC = (KINETIS_ADC_REGS *)ADC2_BLOCK;
    }
    #endif
    #if ADC_CONTROLLERS > 3
    else if (iChannel == 3) {
        ptrADC = (KINETIS_ADC_REGS *)ADC3_BLOCK;
    }
    #endif
    else {
        return;
    }
    iValue += (ptrADC->ADC_SC1A & ADC_SC1A_ADCH_OFF);                    // the input being converted
    usADCvalue = fnConvertSimADCvalue(ptrADC, usADC_values[iChannel][iValue]); // convert the standard value to the format used by the present mode
    if ((ptrADC->ADC_SC2 & ADC_SC2_ACFE) != 0) {                         // {40} if the compare function is enabled
    #if !defined KINETIS_KE
        if ((ptrADC->ADC_SC2 & ADC_SC2_ACREN) != 0) {                    // range enabled (uses CV1 and CV2)
            if ((ptrADC->ADC_SC2 & ADC_SC2_ACFGT) != 0) {                // greater or equal
                if (ptrADC->ADC_CV1 <= ptrADC->ADC_CV2) {                // inside range inclusive
                    if ((usADCvalue < ptrADC->ADC_CV1) || (usADCvalue > ptrADC->ADC_CV2)) {
                        return;
                    }
                }
                else {                                                   // outside range inclusive
                    if ((usADCvalue < ptrADC->ADC_CV1) && (usADCvalue > ptrADC->ADC_CV2)) {
                        return;
                    }
                }
            }
            else {
                if (ptrADC->ADC_CV1 <= ptrADC->ADC_CV2) {                // outside range not inclusive
                    if ((usADCvalue >= ptrADC->ADC_CV1) && (usADCvalue <= ptrADC->ADC_CV2)) {
                        return;
                    }
                }
                else {                                                   // inside range not inclusive
                    if ((usADCvalue >= ptrADC->ADC_CV1) || (usADCvalue <= ptrADC->ADC_CV2)) {
                        return;
                    }
                }
            }
        }
        else {                                                           // uses only CV1
    #endif
            if (ptrADC->ADC_SC2 & ADC_SC2_ACFGT) {                       // greater or equal
                if (usADCvalue < ptrADC->ADC_CV1) {                      // if the range is not true the result is not copied to the data register and the conversion is not complete
                    return;
                }
            }
            else {                                                       // less
                if (usADCvalue >= ptrADC->ADC_CV1) {                     // if the range is not true the result is not copied to the data register and the conversion is not complete
                    return;
                }
            }
        }
    #if !defined KINETIS_KE
    }
    #endif
    ptrADC->ADC_RA = usADCvalue;                                         // set the latest voltage to the result register
    ptrADC->ADC_SC1A |= ADC_SC1A_COCO;                                   // set end of conversion flag
}
#endif

#if defined SUPPORT_LLWU && defined LLWU_AVAILABLE
static const unsigned char cWakeupPorts[PORTS_AVAILABLE][PORT_WIDTH] = {
    #if defined KINETIS_KL03
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    #elif defined KINETIS_KL05
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P0, LLWU_P1, LLWU_P2, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, LLWU_P5, NO_WAKEUP, LLWU_P6, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },

    #elif defined KINETIS_KL46
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P0, LLWU_P1, LLWU_P2, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, LLWU_P5, NO_WAKEUP, LLWU_P6, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #elif defined KINETIS_KL
    // PTA0..PTA31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P5, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, LLWU_P6, NO_WAKEUP, LLWU_P7, LLWU_P8, LLWU_P9, LLWU_P10, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P14, NO_WAKEUP, LLWU_P15, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #else
    // PTA0..PTA31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P5, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, LLWU_P6, NO_WAKEUP, LLWU_P7, LLWU_P8, LLWU_P9, LLWU_P10, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P11, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { LLWU_P12, NO_WAKEUP, LLWU_P13, NO_WAKEUP, LLWU_P14, NO_WAKEUP, LLWU_P15, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, LLWU_P0, LLWU_P1, NO_WAKEUP, LLWU_P2, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #endif
};
static void fnWakeupInterrupt(int iPortReference, unsigned long ulPortState, unsigned long ulBit, unsigned char ucPortBit)
{
    ucPortBit = ((PORT_WIDTH - 1) - ucPortBit);
    if (cWakeupPorts[iPortReference][ucPortBit] == NO_WAKEUP) {
        return;                                                          // this pin doesn't support low-leakage wakup
    }
    else {
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        volatile unsigned char *ptrFlagRegister = LLWU_FLAG_ADDRESS + (cWakeupPorts[iPortReference][ucPortBit]/8);
        unsigned char *ptrWakeupEnable = (unsigned char *)LLWU_BLOCK + (cWakeupPorts[iPortReference][ucPortBit]/4);
        unsigned int iPositionShift = ((cWakeupPorts[iPortReference][ucPortBit]%4) * LLWU_PE_WUPE_SHIFT); // shift in the register required to extract the input configuration
        switch ((*ptrWakeupEnable >> iPositionShift) & LLWU_PE_WUPE_MASK) {
        case LLWU_PE_WUPE_FALLING:
            if ((ulPortState & ulBit) != 0) {
                return;                                                  // a rising edge doesn't match
            }
            break;
        case LLWU_PE_WUPE_RISING:
            if ((ulPortState & ulBit) == 0) {
                return;                                                  // a falling edge doesn't match
            }
            break;
        case LLWU_PE_WUPE_CHANGE:                                        // a change always matches
            break;
        case LLWU_PE_WUPE_OFF:
            return;                                                      // function is disable on this pin
        }
        // The input change matches
        //
        *ptrFlagRegister = (LLWU_F_WUF0 << (cWakeupPorts[iPortReference][ucPortBit]%8)); // set source flag
        ptrVect->processor_interrupts.irq_LL_wakeup();                   // call wakeup interrupt handler
    }
}
#endif

#if defined SUPPORT_ADC
extern int fnGetADC_sim_channel(int iPort, int iBit);
static int fnHandleADCchange(int iChange, int iPort, unsigned char ucPortBit)
{
    if ((iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG | TOGGLE_INPUT_POS | SET_INPUT)) != 0) {
        int iADC = 0;
        unsigned short usStepSize;
        signed int iAdcChannel = fnGetADC_sim_channel(iPort, (/*31 - */ucPortBit)); // {9}
        if (iAdcChannel < 0) {                                           // {9} ignore if not valid ADC port
            return -1;                                                   // not analoge input so ignore
        }
        while (iAdcChannel >= 32) {
            iADC++;
            iAdcChannel -= 32;
        }
        if ((TOGGLE_INPUT_ANALOG & iChange) != 0) {
            usStepSize = (0xffff/3);
        }
        else {
            usStepSize = ((ADC_SIM_STEP_SIZE * 0xffff) / ADC_REFERENCE_VOLTAGE);
        }
        if ((TOGGLE_INPUT_NEG & iChange) != 0) {                         // force a smaller voltage
            if (usADC_values[iADC][iAdcChannel] >= usStepSize) {
                usADC_values[iADC][iAdcChannel] -= usStepSize;           // decrease the voltage on the pin
            }
        }
        else {
            if ((usADC_values[iADC][iAdcChannel] + usStepSize) <= 0xffff) {
                usADC_values[iADC][iAdcChannel] += usStepSize;           // increase the voltage on the pin
            }
        }
    }
    return 0;
}
#endif

#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
static int fnFlexTimerPowered(int iTimer)
{
    switch (iTimer) {
    case 0:
        return (IS_POWERED_UP(6, FTM0));
    #if FLEX_TIMERS_AVAILABLE > 1
    case 1:
        return (IS_POWERED_UP(6, FTM1));
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    case 2:
        #if defined KINETIS_KL || defined KINETIS_K22_SF7 || defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66 || defined KINETIS_KV50
        return (IS_POWERED_UP(6, FTM2));
        #else
        return (IS_POWERED_UP(3, FTM2));
        #endif
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    case 3:
        #if defined KINETIS_K22_SF7
        return (IS_POWERED_UP(6, FTM3));
        #else
        return (IS_POWERED_UP(3, FTM3));
        #endif
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO          // TPM1
    case 4:
        return (IS_POWERED_UP(2, TPM1));
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5 && defined TPMS_AVAILABLE_TOO          // TPM2
    case 5:
        return (IS_POWERED_UP(2, TPM2));
    #endif
    default:
        return 0;
    }
}

static FLEX_TIMER_MODULE *fnGetFlexTimerReg(int iChannel)
{
    switch (iChannel) {
    case 0:
        return (FLEX_TIMER_MODULE *)FTM_BLOCK_0;
    #if defined FTM_BLOCK_1
    case 1:
        return (FLEX_TIMER_MODULE *)FTM_BLOCK_1;
    #endif
    #if defined FTM_BLOCK_2
    case 2:
        return (FLEX_TIMER_MODULE *)FTM_BLOCK_2;
    #endif
    #if defined FTM_BLOCK_3
    case 3:
        return (FLEX_TIMER_MODULE *)FTM_BLOCK_3;
    #endif
    #if defined FTM_BLOCK_4
    case 4:
        return (FLEX_TIMER_MODULE *)FTM_BLOCK_4;
    #endif
    #if defined FTM_BLOCK_5
    case 5:
        return (FLEX_TIMER_MODULE *)FTM_BLOCK_5;
    #endif
    default:
        return 0;
    }
}

unsigned long fnGetFlexTimer_clock(int iChannel)
{
    unsigned long ulClockSpeed = 0;
    FLEX_TIMER_MODULE *ptrTimer = fnGetFlexTimerReg(iChannel);
    #if (FLEX_TIMERS_AVAILABLE == 6) && (TPMS_AVAILABLE_TOO == 2)        // devices with 4 FTMs and 2 TPMs
    if ((iChannel == 4) || (iChannel == 5)) {                            // TPM
        ulClockSpeed = TPM_PWM_CLOCK;
    }
    else {
        switch (ptrTimer->FTM_SC & (FTM_SC_CLKS_MASK)) {                 // switch on the clock source selected for the flextimer
        case FTM_SC_CLKS_EXT:                                            // external clock source
            switch (iChannel) {
        #if defined FLEX_TIMER_0_CLOCK_SPEED
            case 0:
                return FLEX_TIMER_CH0_CLOCK_SPEED;
        #endif
        #if defined FLEX_TIMER_CH1_CLOCK_SPEED && (FLEX_TIMERS_AVAILABLE > 1)
            case 1:
                return FLEX_TIMER_1_CLOCK_SPEED;
        #endif
        #if defined FLEX_TIMER_CH2_CLOCK_SPEED && (FLEX_TIMERS_AVAILABLE > 2)
            case 2:
                return FLEX_TIMER_2_CLOCK_SPEED;
        #endif
        #if defined FLEX_TIMER_CH3_CLOCK_SPEED && (FLEX_TIMERS_AVAILABLE > 3)
            case 3:
                return FLEX_TIMER_3_CLOCK_SPEED;
        #endif
            default:
                _EXCEPTION("External clock speed unknown!");
                break;
            }
            break;
        case FTM_SC_CLKS_FIX:                                            // internal fixed clock source
            if ((MCG_C1 & MCG_C1_IREFS) != 0) {                          // 32kHz IRC is being used as MCGFFCLK
                if ((MCG_C1 & MCG_C1_IRCLKEN) != 0) {                    // check that the IRC 32kHz is enabled
                    ulClockSpeed = SLOW_ICR;
                }
                else {
                    _EXCEPTION("FTM is using MCGFFCLK derived from 32kHz IRC but the source is disabled!");
                    ulClockSpeed = 0;
                }
            }
            else {                                                       // external path is selected
                switch (MCG_C7 & MCG_C7_OSCSEL_MASK) {
                case MCG_C7_OSCSEL_OSCCLK:
                    ulClockSpeed =_EXTERNAL_CLOCK;                       // crystal or external oscillator speed
                    break;
        #if defined MCG_C7_OSCSEL_32K
                case MCG_C7_OSCSEL_32K:
                    ulClockSpeed = 32768;
                    break;
        #endif
        #if defined MCG_C7_OSCSEL_IRC48MCLK
                case MCG_C7_OSCSEL_IRC48MCLK:
                    ulClockSpeed = 48000000;
                    break;
        #endif
                }
        #if defined MCGFFLCLK_FRDIV
                ulClockSpeed /= (MCGFFLCLK_FRDIV);                       // divide the input by FRDIV
        #else
                _EXCEPTION("MCGFFLCLK_FRDIV is not specified!!");        // the FRDIV divide value used needs to be specified
        #endif
            }
            break;
        case FTM_SC_CLKS_SYS:                                            // system clock
            ulClockSpeed = BUS_CLOCK;                                    // bus clock
            break;
        }
    }
    #elif defined KINETIS_KE15 || defined KINETIS_KE18
    switch (ptrTimer->FTM_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) {
    case FTM_SC_CLKS_EXT:
        _EXCEPTION("Not supported");
        break;
    case FTM_SC_CLKS_FIX:
        if ((SCG_SOSCCSR & SCG_SOSCCSR_SOSCERCLKEN) != 0) {
            _EXCEPTION("Not supported");
        }
        else {
            _EXCEPTION("SOSC_CLK needs to be enable for this use!");
        }
        break;
    case FTM_SC_CLKS_SYS:
        ulClockSpeed = DIVCORE_CLK;                                      // system clock
        break;
    }
    #elif defined KINETIS_WITH_PCC
    ulClockSpeed = fnGetPCC_clock((KINETIS_PERIPHERAL_FTM0 + iChannel)); // get the speed of the clock used by this module
    #elif defined KINETIS_KL
    switch (SIM_SOPT2 & SIM_SOPT2_TPMSRC_MCGIRCLK) {                     // {38}
    case SIM_SOPT2_TPMSRC_MCGIRCLK:
        ulClockSpeed = MCGIRCLK;
      //ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)MCGIRCLK) / 1000000); // bus clocks in a period
        if ((MCG_C2 & MCG_C2_IRCS) != 0) {                               // if fast clock
            int prescaler = ((MCG_SC >> 1) & 0x7);                       // FCRDIV value
            while (prescaler-- != 0) {
                ulClockSpeed /= 2;                                       // FCRDIV prescale
            }
        }
        break;
    case SIM_SOPT2_TPMSRC_OSCERCLK:
        #if defined OSCERCLK
        ulClockSpeed = OSCERCLK;
        #else
        _EXCEPTION("No OSCERCLK available");
        #endif
        break;
    case SIM_SOPT2_TPMSRC_MCG:
        #if defined FLL_FACTOR || defined KINETIS_WITH_MCG_LITE
        ulClockSpeed = MCGFLLCLK;
        #else
        ulClockSpeed = (MCGFLLCLK/2);
        #endif
        break;
    }
    #else
    ulClockSpeed = PWM_CLOCK;
    #endif
    ulClockSpeed /= (1 << (ptrTimer->FTM_SC & FTM_SC_PS_128));           // apply pre-scaler
    return ulClockSpeed;
}

static int fnHandleFlexTimer(int iTimerRef, FLEX_TIMER_MODULE *ptrTimer, int iFlexTimerChannels, int iTPM_type) // {49}
{
    static int iChannelFired[FLEX_TIMERS_AVAILABLE][8] = {{0}};
    unsigned long ulCountIncrease = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)fnGetFlexTimer_clock(iTimerRef))/1000000);
    int iTimerInterrupt = 0;
    int iChannel = 0;
    ulCountIncrease += ptrTimer->FTM_CNT;                                // new counter value (assume up counting)
    // Check for channel matches
    //
    while (iChannel < iFlexTimerChannels) {                              // for each channel
        if (((ptrTimer->FTM_channel[iChannel].FTM_CSC & (FTM_CSC_MSB | FTM_CSC_MSA)) != 0) || ((ptrTimer->FTM_SC & FTM_SC_CPWMS) != 0)) { // {50} not in input capture mode
            if ((iChannelFired[iTimerRef][iChannel] == 0) && (ulCountIncrease >= ptrTimer->FTM_channel[iChannel].FTM_CV)) { // channel match
                ptrTimer->FTM_channel[iChannel].FTM_CSC |= FTM_CSC_CHF;  // set the channel event flag
                ptrTimer->FTM_STATUS |= (FTM_STATUS_CH0F << iChannel);   // set the event flag in the global status register
                iChannelFired[iTimerRef][iChannel] = 1;                  // block this channel from firing again before a new period starts
                if ((ptrTimer->FTM_channel[iChannel].FTM_CSC & FTM_CSC_CHIE) != 0) { // if channel interrupt is enabled
                    if ((ptrTimer->FTM_channel[iChannel].FTM_CSC & FTM_CSC_DMA) != 0) { // if DMA is enabled
                        _EXCEPTION("TO DO");
                    }
                    else {
                        iTimerInterrupt = 1;                             // mark that an interrupt is pending
                    }
                }
            }
        }
        iChannel++;
    }
    if (ulCountIncrease >= ptrTimer->FTM_MOD) {                          // period match
        memset(iChannelFired[iTimerRef], 0, sizeof(iChannelFired[iTimerRef])); // reset individual channel fired flags
        ptrTimer->FTM_SC |= FTM_SC_TOF;                                  // set overflow flag
        if (iTPM_type != 0) {
            ulCountIncrease -= ptrTimer->FTM_MOD;
        }
        else {
    #if defined FTM0_CNTIN
            ptrTimer->FTM_CNT = ptrTimer->FTM_CNTIN;                     // respect the counter initial value
            if (ulCountIncrease > (0xffff + (ptrTimer->FTM_MOD - ptrTimer->FTM_CNTIN))) {
                ulCountIncrease = (0xffff + (ptrTimer->FTM_MOD - ptrTimer->FTM_CNTIN));
            }
            while (ulCountIncrease >= ptrTimer->FTM_MOD) {
                ulCountIncrease -= (ptrTimer->FTM_MOD - ptrTimer->FTM_CNTIN);
            }
    #else
            ulCountIncrease -= ptrTimer->FTM_MOD;
    #endif
        }
    }
    ptrTimer->FTM_CNT = ulCountIncrease;                                 // new counter value
    if (((ptrTimer->FTM_SC & FTM_SC_TOIE) != 0) && ((ptrTimer->FTM_SC & FTM_SC_TOF) != 0)) { // if overflow occurred and interrupt enabled
        iTimerInterrupt = 1;
    }
    return iTimerInterrupt;
}

static void fnExecuteTimerInterrupt(int iTimerRef)
{
    switch (iTimerRef) {
    case 0:
    #if defined KINETIS_KL
        if (fnGenInt(irq_TPM0_ID) != 0) {                                // if timer/PWM module 0 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_TPM0();                    // call the interrupt handler
        }
    #else
        if (fnGenInt(irq_FTM0_ID) != 0) {                                // if FlexTimer 0 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_FTM0();                    // call the interrupt handler
        }
    #endif
        break;
    #if FLEX_TIMERS_AVAILABLE > 1
    case 1:
        #if defined KINETIS_KL
        if (fnGenInt(irq_TPM1_ID) != 0) {                                // if timer/PWM module 1 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_TPM1();                    // call the interrupt handler
        }
        #else
        if (fnGenInt(irq_FTM1_ID) != 0) {                                // if FlexTimer 1 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_FTM1();                    // call the interrupt handler
        }
        #endif
        break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    case 2:
        #if defined KINETIS_KL
        if (fnGenInt(irq_TPM2_ID) != 0) {                                // if timer/PWM module 2 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_TPM2();                    // call the interrupt handler
        }
        #else
        if (fnGenInt(irq_FTM2_ID) != 0) {                                // if FlexTimer 2 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_FTM2();                    // call the interrupt handler
        }
        #endif
        break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    case 3:
        if (fnGenInt(irq_FTM3_ID) != 0) {                                // if FlexTimer 3 interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_FTM3();                    // call the interrupt handler
        }
        break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO          // TPM1
    case 4:
        if (fnGenInt(irq_TPM1_ID) != 0) {                                // if FlexTimer 4 (TPM1) interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_TPM1();                    // call the interrupt handler
        }
        break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5 && defined TPMS_AVAILABLE_TOO          // TPM2
    case 5:
        if (fnGenInt(irq_TPM2_ID) != 0) {                                // if FlexTimer 5 (TPM2) interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_TPM2();                    // call the interrupt handler
        }
        break;
    #endif
    }
}

static unsigned long ulTimerPinEnabled[PORTS_AVAILABLE] = {0};
static unsigned char ucPinTimerDetails[PORTS_AVAILABLE][PORT_WIDTH] = {0};

extern void fnEnterTimer(int iPortRef, int iPinRef, int iTimer, int iChannel, int iOnOff)
{
    if (iOnOff != 0) {
        ulTimerPinEnabled[iPortRef] |= (1 << iPinRef);                   // mark that this pin has been programmed for timer operation
        ucPinTimerDetails[iPortRef][iPinRef] = (unsigned char)((iTimer << 4) | iChannel);
    }
    else {
        ulTimerPinEnabled[iPortRef] &= ~(1 << iPinRef);                  // mark that this pin has not been programmed for timer operation
    }
}

extern int fnGetPWM_sim_channel(int iPort, int iPin, unsigned long *ptr_ulFrequency, unsigned char *ptr_ucMSR)
{
    if ((ulTimerPinEnabled[iPort] & (1 << iPin)) != 0) {                 // if this pin has been programmed as a timer function
        int iTimer = (ucPinTimerDetails[iPort][iPin] >> 4);
        int iChannel = (ucPinTimerDetails[iPort][iPin] & 0x0f);
        if (fnFlexTimerPowered(iTimer) != 0) {                           // the timer must be powered
            FLEX_TIMER_MODULE *ptrTimer = fnGetFlexTimerReg(iTimer);
            unsigned long ulMOD_value;
            unsigned long ulMatchValue;
            if ((ptrTimer->FTM_SC & FTM_SC_CPWMS) != 0) {                // if center-aligned
                ulMOD_value = (ptrTimer->FTM_MOD * 2);
                ulMatchValue = (ptrTimer->FTM_channel[iChannel].FTM_CV * 2);
            }
            else {
                ulMOD_value = (ptrTimer->FTM_MOD + 1);
                ulMatchValue = ptrTimer->FTM_channel[iChannel].FTM_CV;
            }
            if ((ptrTimer->FTM_channel[iChannel].FTM_CSC & (FTM_CSC_ELSA | FTM_CSC_ELSB | FTM_CSC_MSA | FTM_CSC_MSB)) == FTM_CSC_MS_ELS_PWM_LOW_TRUE_PULSES) { // polarity inverted
                ulMatchValue = (*ptr_ulFrequency - *ptr_ucMSR);
            }
            if (ulMOD_value == 0) {
                ulMOD_value = 1;
            }
            *ptr_ucMSR = (unsigned char)((float)((float)(ulMatchValue * 100) / (float)ulMOD_value) + (float)0.5); // MSR in percent
            *ptr_ulFrequency = (fnGetFlexTimer_clock(iTimer)/ulMOD_value);
            return 0;
        }
    }
    return -1;
}


// Handle timer capture inputs {50}
//
static void fnHandleTimerInput(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
    if ((ulTimerPinEnabled[iPort] & ulChangedBit) != 0) {                // handle input changes only when the pin is configured as a timer inputs
        int iChannel;
        int iTimer;
        int iPinBit = 0;
        while ((ulChangedBit & 1) == 0) {
            ulChangedBit >>= 1;
            iPinBit++;
        }
        ulChangedBit <<= iPinBit;
        iTimer = (ucPinTimerDetails[iPort][iPinBit] >> 4);
        iChannel = (ucPinTimerDetails[iPort][iPinBit] & 0x0f);
        // If the timer is powered up
        //
        if (fnFlexTimerPowered(iTimer) != 0) {
            FLEX_TIMER_MODULE *ptrTimer = fnGetFlexTimerReg(iTimer);
            // The input is attached to a timer so we capture its present input value if the sense matches
            //
            switch (ptrTimer->FTM_channel[iChannel].FTM_CSC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) {
            case 0:                                                      // not in input capture mode
                return;
            case FTM_CSC_ELSA:                                           // rising edge capture
                if ((ulNewState & ulChangedBit) == 0) {
                    return;                                              // ignore a falling edge
                }
                break;
            case FTM_CSC_ELSB:                                           // falling edge capture
                if ((ulNewState & ulChangedBit) != 0) {
                    return;                                              // ignore a rising edge
                }
                break;
            case (FTM_CSC_ELSA | FTM_CSC_ELSB):
                break;                                                   // accept any edge
            }
            // Capture event has take place
            //
            ptrTimer->FTM_channel[iChannel].FTM_CV = ptrTimer->FTM_CNT;  // capture the present timer count vlue
            ptrTimer->FTM_channel[iChannel].FTM_CSC |= FTM_CSC_CHF;      // set the channel event flag
            if ((ptrTimer->FTM_channel[iChannel].FTM_CSC & FTM_CSC_CHIE) != 0) { // if the channel's interrupt is enabled
                fnExecuteTimerInterrupt(iTimer);
            }
        }
    }
}
#endif

// Simulate setting, clearing or toggling of an input pin
//
extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
#if defined KINETIS_KL
    static unsigned long ulTSI[61] = {0};
#endif
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    unsigned long ulBit;
#else
    unsigned long ulBit = (0x80000000 >> ucPortBit);
    unsigned long *ptrPCR = (unsigned long *)PORT0_BLOCK;
    ptrPCR += (ucPort * sizeof(KINETIS_PORT)/sizeof(unsigned long));
#endif
    iFlagRefresh = PORT_CHANGE;
#if defined SUPPORT_ADC
    if (fnHandleADCchange(iChange, ucPort, ucPortBit) == 0) {            // handle possible ADC function on pin
        return;                                                          // if ADC we do not handle digital functions
    }
#endif
#if defined PORTS_AVAILABLE_8_BIT                                        // KE uses byte terminology but physically have long word ports
    ulBit = (0x80000000 >> (ucPortBit + ((3 - (ucPort % 4)) * 8)));      // convert to long word port representation
    ucPort /= 4;
#endif
    switch (ucPort) {
    case _PORTA:
        if ((~GPIOA_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_A;
#if defined KINETIS_WITH_PCC
            if ((PCC_PORTA & PCC_CGC) == 0) {                            // ignore if port is not clocked
                return;
            }
#elif !defined KINETIS_KE
            if (IS_POWERED_UP(5, PORTA) == 0) {                          // ignore if port is not clocked
                return;
            }
#endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_A ^= ulBit;                                    // set new pin state
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
#endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOA_PDIR &= ~ulBit;
                GPIOA_PDIR |= (ulPort_in_A & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_A |= ulBit;
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
#endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOA_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_A &= ~ulBit;
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
#endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOA_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_A != ulOriginal_port_state) {                  // if a change took place
#if defined SUPPORT_LLWU && defined LLWU_AVAILABLE
                fnWakeupInterrupt(_PORTA, ulPort_in_A, ulBit, ucPortBit);// handle wakeup events on the pin
#endif
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
                fnPortInterrupt(_PORTA, ulPort_in_A, ulBit, 0);          // handle interrupts on the pin
#else
                fnPortInterrupt(_PORTA, ulPort_in_A, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
        #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
                fnHandleTimerInput(_PORTA, ulPort_in_A, ulBit, ptrPCR);  // {50}
        #endif
#endif
            }
        }
        break;
#if PORTS_AVAILABLE > 1
    case _PORTB:
        if ((~GPIOB_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_B;
    #if defined KINETIS_WITH_PCC
            if ((PCC_PORTB & PCC_CGC) == 0) {                            // ignore if port is not clocked
                return;
            }
    #elif !defined KINETIS_KE
            if (IS_POWERED_UP(5, PORTB) == 0) {                          // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_B ^= ulBit;                                    // set new pin state
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOB_PDIR &= ~ulBit;
                GPIOB_PDIR |= (ulPort_in_B & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_B |= ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOB_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_B &= ~ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOB_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_B != ulOriginal_port_state) {                  // if a change took place
    #if defined SUPPORT_LLWU && defined LLWU_AVAILABLE
                fnWakeupInterrupt(_PORTB, ulPort_in_B, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
    #if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
                fnPortInterrupt(_PORTB, ulPort_in_B, ulBit, 0);          // handle interrupts on the pin
    #else
                fnPortInterrupt(_PORTB, ulPort_in_B, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
        #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
                fnHandleTimerInput(_PORTB, ulPort_in_B, ulBit, ptrPCR);  // {50}
        #endif
    #endif
            }
        }
        break;
#endif
#if PORTS_AVAILABLE > 2
    case _PORTC:
        if (~GPIOC_PDDR & ulBit) {                                       // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_C;
    #if defined KINETIS_WITH_PCC
            if ((PCC_PORTC & PCC_CGC) == 0) {                            // ignore if port is not clocked
                return;
            }
    #elif !defined KINETIS_KE
            if (IS_POWERED_UP(5, PORTC) == 0) {                          // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_C ^= ulBit;                                    // set new pin state
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOC_PDIR &= ~ulBit;
                GPIOC_PDIR |= (ulPort_in_C & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_C |= ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOC_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_C &= ~ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOC_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_C != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTC, ulPort_in_C, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
    #if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
                fnPortInterrupt(_PORTC, ulPort_in_C, ulBit, 0);          // handle interrupts on the pin
    #else
                fnPortInterrupt(_PORTC, ulPort_in_C, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
        #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
                fnHandleTimerInput(_PORTC, ulPort_in_C, ulBit, ptrPCR);  // {50}
        #endif
    #endif
            }
        }
        break;
#endif
#if PORTS_AVAILABLE > 3
    case _PORTD:
        if (~GPIOD_PDDR & ulBit) {                                       // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_D;
    #if defined KINETIS_WITH_PCC
            if ((PCC_PORTD & PCC_CGC) == 0) {                            // ignore if port is not clocked
                return;
            }
    #elif !defined KINETIS_KE
            if (IS_POWERED_UP(5, PORTD) == 0) {                          // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_D ^= ulBit;                                    // set new pin state
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOD_PDIR &= ~ulBit;
                GPIOD_PDIR |= (ulPort_in_D & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_D |= ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOD_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_D &= ~ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOD_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_D != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTD, ulPort_in_D, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
                fnPortInterrupt(_PORTD, ulPort_in_D, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
                fnHandleTimerInput(_PORTD, ulPort_in_D, ulBit, ptrPCR);  // {50}
    #endif
            }
        }
        break;
    case _PORTE:
        if ((~GPIOE_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_E;
    #if defined KINETIS_WITH_PCC
            if ((PCC_PORTE & PCC_CGC) == 0) {                            // ignore if port is not clocked
                return;
            }
    #elif !defined KINETIS_KE
            if (IS_POWERED_UP(5, PORTE) == 0) {                          // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_E ^= ulBit;                                    // set new pin state
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOE_PDIR &= ~ulBit;
                GPIOE_PDIR |= (ulPort_in_E & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_E |= ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOE_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_E &= ~ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOE_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_E != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTE, ulPort_in_E, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
                fnPortInterrupt(_PORTE, ulPort_in_E, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
                fnHandleTimerInput(_PORTE, ulPort_in_E, ulBit, ptrPCR);  // {50}
    #endif
            }
        }
        break;
#endif
#if PORTS_AVAILABLE > 5
    case _PORTF:
        if ((~GPIOF_PDDR & ulBit) != 0) {                                // if configured as input
            unsigned long ulOriginal_port_state = ulPort_in_F;
    #if defined KINETIS_WITH_PCC
            if ((PCC_PORTF & PCC_CGC) == 0) {                            // ignore if port is not clocked
                return;
            }
    #elif !defined KINETIS_KE
            if (IS_POWERED_UP(5, PORTF) == 0) {                          // ignore if port is not clocked
                return;
            }
    #endif
            if (iChange == TOGGLE_INPUT) {
                ulPort_in_F ^= ulBit;                                    // set new pin state
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOF_PDIR &= ~ulBit;
                GPIOF_PDIR |= (ulPort_in_F & ulBit);                     // set new input register state
            }
            else if (iChange == SET_INPUT) {
                ulPort_in_F |= ulBit;
    #if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
                ptrPCR += (31 - ucPortBit);
    #endif
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOF_PDIR |= ulBit;                                     // set new input register state
            }
            else {
                ulPort_in_F &= ~ulBit;
                ptrPCR += (31 - ucPortBit);
              //if ((*ptrPCR & PORT_MUX_ALT7) != PORT_MUX_GPIO) {        // {19} ignore register state if not connected
              //    return;
              //}
                GPIOF_PDIR &= ~ulBit;                                    // set new input register state
            }
            if (ulPort_in_F != ulOriginal_port_state) {                  // if a change took place
    #if defined LLWU_AVAILABLE && defined SUPPORT_LLWU
                fnWakeupInterrupt(_PORTF, ulPort_in_F, ulBit, ucPortBit);// handle wakeup events on the pin
    #endif
                fnPortInterrupt(_PORTF, ulPort_in_F, ulBit, ptrPCR);     // handle interrupts and DMA on the pin
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
                fnHandleTimerInput(_PORTF, ulPort_in_F, ulBit, ptrPCR);  // {50}
    #endif
            }
        }
        break;
#endif
#if TSI_AVAILABLE > 0
#if defined KINETIS_KL
    case _TOUCH_PORTB:
        if (ulBit == PORTB_BIT16) {                                       // channel 9 on PTB16
            if (iChange == TOGGLE_INPUT) {
                ulTSI[9] = ~ulTSI[9];
            }
            else if (iChange == SET_INPUT) {
                ulTSI[9] = 0;
            }
            else {
                ulTSI[9] = 1;
            }
        }
        else if (ulBit == PORTB_BIT17) {                                 // channel 10 on PTB17
            if (iChange == TOGGLE_INPUT) {
                ulTSI[10] = ~ulTSI[10];
            }
            else if (iChange == SET_INPUT) {
                ulTSI[10] = 0;
            }
            else {
                ulTSI[10] = 1;
            }
        }
        break;
#else
    case _TOUCH_PORTA:                                                   // {5} touch sensor
        if (ulBit == PORTA_BIT0) {                                       // channel 1 on PTA0
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR1 = ((~TSI0_CNTR1 & 0xffff0000) | (TSI0_CNTR1 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR1 = (TSI0_CNTR1 & 0x0000ffff);
            }
            else {
                TSI0_CNTR1 = (TSI0_CNTR1 | 0xffff0000);
            }
        }
        else if (ulBit == PORTA_BIT1) {                                  // channel 2 on PTA1
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR3 = ((~TSI0_CNTR3 & 0x0000ffff) | (TSI0_CNTR3 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR3 = (TSI0_CNTR3 & 0xffff0000);
            }
            else {
                TSI0_CNTR3 = (TSI0_CNTR3 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTA_BIT2) {                                  // channel 3 on PTA2
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR3 = ((~TSI0_CNTR3 & 0xffff0000) | (TSI0_CNTR3 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR3 = (TSI0_CNTR3 & 0x0000ffff);
            }
            else {
                TSI0_CNTR3 = (TSI0_CNTR3 | 0xffff0000);
            }
        }
        else if (ulBit == PORTA_BIT3) {                                  // channel 4 on PTA3
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR5 = ((~TSI0_CNTR5 & 0x0000ffff) | (TSI0_CNTR5 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR5 = (TSI0_CNTR5 & 0xffff0000);
            }
            else {
                TSI0_CNTR5 = (TSI0_CNTR5 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTA_BIT4) {                                  // channel 5 on PTA4
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR5 = ((~TSI0_CNTR5 & 0xffff0000) | (TSI0_CNTR5 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR5 = (TSI0_CNTR5 & 0x0000ffff);
            }
            else {
                TSI0_CNTR5 = (TSI0_CNTR5 | 0xffff0000);
            }
        }
        if (iChange == TOGGLE_INPUT) {
            ulPort_in_A ^= ulBit;
        }
        else if (iChange == SET_INPUT) {
            ulPort_in_A |= ulBit;
        }
        else {
            ulPort_in_A &= ~ulBit;
        }
        break;
    case _TOUCH_PORTB:
        if (ulBit == PORTB_BIT0) {                                       // channel 0 on PTB0
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR1 = ((~TSI0_CNTR1 & 0x0000ffff) | (TSI0_CNTR1 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR1 = (TSI0_CNTR1 & 0xffff0000);
            }
            else {
                TSI0_CNTR1 = (TSI0_CNTR1 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT1) {                                  // channel 6 on PTB1
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR7 = ((~TSI0_CNTR7 & 0x0000ffff) | (TSI0_CNTR7 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR7 = (TSI0_CNTR7 & 0xffff0000);
            }
            else {
                TSI0_CNTR7 = (TSI0_CNTR7 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT2) {                                  // channel 7 on PTB2
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR7 = ((~TSI0_CNTR7 & 0xffff0000) | (TSI0_CNTR7 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR7 = (TSI0_CNTR7 & 0x0000ffff);
            }
            else {
                TSI0_CNTR7 = (TSI0_CNTR7 | 0xffff0000);
            }
        }
        else if (ulBit == PORTB_BIT3) {                                  // channel 8 on PTB3
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR9 = ((~TSI0_CNTR9 & 0x0000ffff) | (TSI0_CNTR9 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR9 = (TSI0_CNTR9 & 0xffff0000);
            }
            else {
                TSI0_CNTR9 = (TSI0_CNTR9 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT16) {                                 // channel 9 on PTB16
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR9 = ((~TSI0_CNTR9 & 0xffff0000) | (TSI0_CNTR9 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR9 = (TSI0_CNTR9 & 0x0000ffff);
            }
            else {
                TSI0_CNTR9 = (TSI0_CNTR9 | 0xffff0000);
            }
        }
        else if (ulBit == PORTB_BIT17) {                                 // channel 10 on PTB17
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR11 = ((~TSI0_CNTR11 & 0x0000ffff) | (TSI0_CNTR11 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR11 = (TSI0_CNTR11 & 0xffff0000);
            }
            else {
                TSI0_CNTR11 = (TSI0_CNTR11 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTB_BIT18) {                                 // channel 11 on PTB18
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR11 = ((~TSI0_CNTR9 & 0xffff0000) | (TSI0_CNTR11 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR11 = (TSI0_CNTR11 & 0x0000ffff);
            }
            else {
                TSI0_CNTR11 = (TSI0_CNTR11 | 0xffff0000);
            }
        }
        else if (ulBit == PORTB_BIT19) {                                 // channel 12 on PTB19
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR13 = ((~TSI0_CNTR13 & 0x0000ffff) | (TSI0_CNTR13 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR13 = (TSI0_CNTR13 & 0xffff0000);
            }
            else {
                TSI0_CNTR13 = (TSI0_CNTR13 | 0x0000ffff);
            }
        }
        if (iChange == TOGGLE_INPUT) {
            ulPort_in_B ^= ulBit;
        }
        else if (iChange == SET_INPUT) {
            ulPort_in_B |= ulBit;
        }
        else {
            ulPort_in_B &= ~ulBit;
        }
        break;
#if PORTS_AVAILABLE > 2
    case _TOUCH_PORTC:
        if (ulBit == PORTC_BIT0) {                                       // channel 13 on PTC0
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR13 = ((~TSI0_CNTR13 & 0xffff0000) | (TSI0_CNTR13 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR13 = (TSI0_CNTR13 & 0x0000ffff);
            }
            else {
                TSI0_CNTR13 = (TSI0_CNTR13 | 0xffff0000);
            }
        }
        else if (ulBit == PORTC_BIT1) {                                  // channel 14 on PTC1
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR15 = ((~TSI0_CNTR15 & 0x0000ffff) | (TSI0_CNTR15 & 0xffff0000));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR15 = (TSI0_CNTR15 & 0xffff0000);
            }
            else {
                TSI0_CNTR15 = (TSI0_CNTR15 | 0x0000ffff);
            }
        }
        else if (ulBit == PORTC_BIT2) {                                  // channel 15 on PTC2
            if (iChange == TOGGLE_INPUT) {
                TSI0_CNTR15 = ((~TSI0_CNTR15 & 0xffff0000) | (TSI0_CNTR15 & 0x0000ffff));
            }
            else if (iChange == SET_INPUT) {
                TSI0_CNTR15 = (TSI0_CNTR15 & 0x0000ffff);
            }
            else {
                TSI0_CNTR15 = (TSI0_CNTR15 | 0xffff0000);
            }
        }
        if (iChange == TOGGLE_INPUT) {
            ulPort_in_C ^= ulBit;
        }
        else if (iChange == SET_INPUT) {
            ulPort_in_C |= ulBit;
        }
        else {
            ulPort_in_C &= ~ulBit;
        }
        break;
#endif
#endif
#endif
    case _PORT_EXT_0:                                                    // {8} external ports extensions
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

// if either PORTx_GPCLR of PORTx_GPCHR have been written to, set the PCD registers accordingly
//
static void fnSetPinCharacteristics(int iPortRef, unsigned long ulHigh, unsigned long ulLow)
{
    unsigned long ulBit = 0x00010000;
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
    unsigned long *ptrPCR = (unsigned long *)(PORT0_BLOCK + (iPortRef * 0x1000));
#endif
    while (ulBit != 0) {
        if ((ulLow & ulBit) != 0) {
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
            *ptrPCR = (ulLow & 0x0000ffff);
#endif
        }
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
        ptrPCR++;
#endif
        ulBit <<= 1;
    }
    ulBit = 0x00010000;
    while (ulBit != 0) {
        if ((ulHigh & ulBit) != 0) {
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
            *ptrPCR = (ulHigh & 0x0000ffff);
#endif
        }
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
        ptrPCR++;
#endif
        ulBit <<= 1;
    }
#if !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
    *ptrPCR++ = 0;                                                       // clear the PORTx_GPCLR and PORTx_GPCHR registers, which read always 0
    *ptrPCR = 0;
#endif
}

// Update ports based on present register settings
//
extern void fnSimPorts(int iPort)
{
#if !defined KINETIS_KM
    unsigned long ulNewState;
#endif
    switch (iPort) {
    case PORTA:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTA & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTA) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOA_PDIR = ((unsigned char)(ulPort_in_A & ~GPIOA_PDDR) | (GPIOA_PDOR & GPIOA_PDDR)); // input state
    #else
            ulNewState = (GPIOA_PDOR | GPIOA_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOA_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOA_PTOR;                                    // toggle bits from toggle register
            GPIOA_PDOR = ulNewState;
            GPIOA_PDIR = ((ulPort_in_A & ~GPIOA_PDDR) | (GPIOA_PDOR & GPIOA_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTA_GPCLR != 0) || (PORTA_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTA, PORTA_GPCHR, PORTA_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port A access without port A powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOA_PTOR = GPIOA_PSOR = GPIOA_PCOR = 0;                        // registers always read 0
    #endif
        return;
#if PORTS_AVAILABLE > 1
    case PORTB:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTB & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTB) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOB_PDIR = ((unsigned char)(ulPort_in_B & ~GPIOB_PDDR) | (GPIOB_PDOR & GPIOB_PDDR)); // input state
    #else
            ulNewState = (GPIOB_PDOR | GPIOB_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOB_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOB_PTOR;                                    // toggle bits from toggle register
            GPIOB_PDOR = ulNewState;
            GPIOB_PDIR = ((ulPort_in_B & ~GPIOB_PDDR) | (GPIOB_PDOR & GPIOB_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTB_GPCLR != 0) || (PORTB_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTB, PORTB_GPCHR, PORTB_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port B access without port B powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOB_PTOR = GPIOB_PSOR = GPIOB_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 2
    case PORTC:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTC & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTC) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOC_PDIR = ((unsigned char)(ulPort_in_C & ~GPIOC_PDDR) | (GPIOC_PDOR & GPIOC_PDDR)); // input state
    #else
            ulNewState = (GPIOC_PDOR | GPIOC_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOC_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOC_PTOR;                                    // toggle bits from toggle register
            GPIOC_PDOR = ulNewState;
            GPIOC_PDIR = ((ulPort_in_C & ~GPIOC_PDDR) | (GPIOC_PDOR & GPIOC_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTC_GPCLR != 0) || (PORTC_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTC, PORTC_GPCHR, PORTC_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port C access without port C powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOC_PTOR = GPIOC_PSOR = GPIOC_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 3
    case PORTD:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTD & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTD) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOD_PDIR = ((unsigned char)(ulPort_in_D & ~GPIOD_PDDR) | (GPIOD_PDOR & GPIOD_PDDR)); // input state
    #else
            ulNewState = (GPIOD_PDOR | GPIOD_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOD_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOD_PTOR;                                    // toggle bits from toggle register
            GPIOD_PDOR = ulNewState;
            GPIOD_PDIR = ((ulPort_in_D & ~GPIOD_PDDR) | (GPIOD_PDOR & GPIOD_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTD_GPCLR != 0) || (PORTD_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTD, PORTD_GPCHR, PORTD_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port D access without port D powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOD_PTOR = GPIOD_PSOR = GPIOD_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 4
    case PORTE:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTE & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTE) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOE_PDIR = ((unsigned char)(ulPort_in_E & ~GPIOE_PDDR) | (GPIOE_PDOR & GPIOE_PDDR)); // input state
    #else
            ulNewState = (GPIOE_PDOR | GPIOE_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOE_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOE_PTOR;                                    // toggle bits from toggle register
            GPIOE_PDOR = ulNewState;
            GPIOE_PDIR = ((ulPort_in_E & ~GPIOE_PDDR) | (GPIOE_PDOR & GPIOE_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTE_GPCLR != 0) || (PORTE_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTE, PORTE_GPCHR, PORTE_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port E access without port E powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOE_PTOR = GPIOE_PSOR = GPIOE_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 5
    case PORTF:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTF & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTF) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOF_PDIR = ((unsigned char)(ulPort_in_F & ~GPIOF_PDDR) | (GPIOF_PDOR & GPIOF_PDDR)); // input state
    #else
            ulNewState = (GPIOF_PDOR | GPIOF_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOF_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOF_PTOR;                                    // toggle bits from toggle register
            GPIOF_PDOR = ulNewState;
            GPIOF_PDIR = ((ulPort_in_F & ~GPIOF_PDDR) | (GPIOF_PDOR & GPIOF_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTF_GPCLR != 0) || (PORTF_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTF, PORTF_GPCHR, PORTF_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port F access without port F powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOF_PTOR = GPIOF_PSOR = GPIOF_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 6
    case PORTG:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTG & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTG) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOG_PDIR = ((unsigned char)(ulPort_in_G & ~GPIOG_PDDR) | (GPIOG_PDOR & GPIOG_PDDR)); // input state
    #else
            ulNewState = (GPIOG_PDOR | GPIOG_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOG_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOG_PTOR;                                    // toggle bits from toggle register
            GPIOG_PDOR = ulNewState;
            GPIOG_PDIR = ((ulPort_in_G & ~GPIOG_PDDR) | (GPIOG_PDOR & GPIOG_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTG_GPCLR != 0) || (PORTG_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTG, PORTG_GPCHR, PORTG_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port G access without port G powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOG_PTOR = GPIOG_PSOR = GPIOG_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 7
    case PORTH:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTH & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTH) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOH_PDIR = ((unsigned char)(ulPort_in_H & ~GPIOH_PDDR) | (GPIOH_PDOR & GPIOH_PDDR)); // input state
    #else
            ulNewState = (GPIOH_PDOR | GPIOH_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOH_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOH_PTOR;                                    // toggle bits from toggle register
            GPIOH_PDOR = ulNewState;
            GPIOH_PDIR = ((ulPort_in_H & ~GPIOH_PDDR) | (GPIOH_PDOR & GPIOH_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTH_GPCLR != 0) || (PORTH_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTH, PORTH_GPCHR, PORTH_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port H access without port H powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOH_PTOR = GPIOH_PSOR = GPIOH_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
#if PORTS_AVAILABLE > 8
    case PORTI:
    #if defined KINETIS_WITH_PCC
        if ((PCC_PORTI & PCC_CGC) != 0)                                  // if port is clocked
    #elif defined SIM_SCGC5
        if ((SIM_SCGC5 & SIM_SCGC5_PORTI) != 0)                          // if port is clocked
    #endif
        {
    #if defined KINETIS_KM
            GPIOI_PDIR = ((unsigned char)(ulPort_in_I & ~GPIOI_PDDR) | (GPIOI_PDOR & GPIOI_PDDR)); // input state
    #else
            ulNewState = (GPIOI_PDOR | GPIOI_PSOR);                      // set bits from set register
            ulNewState &= ~(GPIOI_PCOR);                                 // clear bits from clear register
            ulNewState ^= GPIOI_PTOR;                                    // toggle bits from toggle register
            GPIOI_PDOR = ulNewState;
            GPIOI_PDIR = ((ulPort_in_I & ~GPIOI_PDDR) | (GPIOI_PDOR & GPIOI_PDDR)); // input state
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
            if ((PORTI_GPCLR != 0) || (PORTI_GPCHR != 0)) {
                fnSetPinCharacteristics(_PORTI, PORTI_GPCHR, PORTI_GPCLR);
            }
        #endif
    #endif
        }
    #if defined KINETIS_WITH_PCC || defined SIM_SCGC5
        else {
            _EXCEPTION("Port I access without port I powered up!!!!");
        }
    #endif
    #if !defined KINETIS_KM
        GPIOI_PTOR = GPIOI_PSOR = GPIOI_PCOR = 0;                        // registers always read 0
    #endif
        return;
#endif
    default:
        break;
    }
#if defined KINETIS_WITH_PCC
    if ((PCC_PORTA & PCC_CGC) != 0)                                      // if port is clocked
#elif defined SIM_SCGC5
    if ((SIM_SCGC5 & SIM_SCGC5_PORTA) != 0)                              // if port is clocked
#endif
    {
#if defined KINETIS_KM
        GPIOA_PDIR = ((unsigned char)(ulPort_in_A & ~GPIOA_PDDR) | (GPIOA_PDOR & GPIOA_PDDR)); // input state
#else
        ulNewState = (GPIOA_PDOR | GPIOA_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOA_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOA_PTOR;                                        // toggle bits from toggle register
        GPIOA_PDOR = ulNewState;
        GPIOA_PDIR = ((ulPort_in_A & ~GPIOA_PDDR) | (GPIOA_PDOR & GPIOA_PDDR)); // input state {10}
    #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
        if ((PORTA_GPCLR != 0) || (PORTA_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTA, PORTA_GPCHR, PORTA_GPCLR);
        }
    #endif
#endif
    }
#if !defined KINETIS_KM
    GPIOA_PTOR = GPIOA_PSOR = GPIOA_PCOR = 0;                            // registers always read 0
#endif
#if PORTS_AVAILABLE > 1
    #if defined KINETIS_WITH_PCC
    if ((PCC_PORTB & PCC_CGC) != 0)                                      // if port is clocked
    #elif !defined KINETIS_KE
    if ((SIM_SCGC5 & SIM_SCGC5_PORTB) != 0)                              // if port is clocked
    #endif
    {
    #if defined KINETIS_KM
        GPIOB_PDIR = ((unsigned char)(ulPort_in_B & ~GPIOB_PDDR) | (GPIOB_PDOR & GPIOB_PDDR)); // input state
    #else
        ulNewState = (GPIOB_PDOR | GPIOB_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOB_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOB_PTOR;                                        // toggle bits from toggle register
        GPIOB_PDOR = ulNewState;
        GPIOB_PDIR = ((ulPort_in_B & ~GPIOB_PDDR) | (GPIOB_PDOR & GPIOB_PDDR)); // input state {10}
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
        if ((PORTB_GPCLR != 0) || (PORTB_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTB, PORTB_GPCHR, PORTB_GPCLR);
        }
        #endif
    #endif
    }
    #if !defined KINETIS_KM
    GPIOB_PTOR = GPIOB_PSOR = GPIOB_PCOR = 0;                            // registers always read 0
    #endif
#endif
#if PORTS_AVAILABLE > 2
    #if defined KINETIS_WITH_PCC
    if ((PCC_PORTC & PCC_CGC) != 0)                                      // if port is clocked
    #elif !defined KINETIS_KE
    if ((SIM_SCGC5 & SIM_SCGC5_PORTC) != 0)                              // if port is clocked
    #endif
    {
    #if defined KINETIS_KM
        GPIOC_PDIR = ((unsigned char)(ulPort_in_C & ~GPIOC_PDDR) | (GPIOC_PDOR & GPIOC_PDDR)); // input state
    #else
        ulNewState = (GPIOC_PDOR | GPIOC_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOC_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOC_PTOR;                                        // toggle bits from toggle register
        GPIOC_PDOR = ulNewState;
        GPIOC_PDIR = ((ulPort_in_C & ~GPIOC_PDDR) | (GPIOC_PDOR & GPIOC_PDDR)); // input state {10}
        #if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
        if ((PORTC_GPCLR != 0) || (PORTC_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTC, PORTC_GPCHR, PORTC_GPCLR);
        }
        #endif
    #endif
    }
    #if !defined KINETIS_KM
    GPIOC_PTOR = GPIOC_PSOR = GPIOC_PCOR = 0;                            // registers always read 0
    #endif
#endif
#if PORTS_AVAILABLE > 3
    #if defined KINETIS_WITH_PCC
    if ((PCC_PORTD & PCC_CGC) != 0)                                      // if port is clocked
    #else
    if ((SIM_SCGC5 & SIM_SCGC5_PORTD) != 0)                              // if port is clocked
    #endif
    {
    #if defined KINETIS_KM
        GPIOD_PDIR = ((unsigned char)(ulPort_in_D & ~GPIOD_PDDR) | (GPIOD_PDOR & GPIOD_PDDR)); // input state
    #else
        ulNewState = (GPIOD_PDOR | GPIOD_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOD_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOD_PTOR;                                        // toggle bits from toggle register
        GPIOD_PDOR = ulNewState;
        GPIOD_PDIR = ((ulPort_in_D & ~GPIOD_PDDR) | (GPIOD_PDOR & GPIOD_PDDR)); // input state {10}
        if ((PORTD_GPCLR != 0) || (PORTD_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTD, PORTD_GPCHR, PORTD_GPCLR);
        }
    #endif
    }
    #if !defined KINETIS_KM
    GPIOD_PTOR = GPIOD_PSOR = GPIOD_PCOR = 0;                            // registers always read 0
    #endif
    #if defined KINETIS_WITH_PCC
    if ((PCC_PORTE & PCC_CGC) != 0)                                      // if port is clocked
    #else
    if ((SIM_SCGC5 & SIM_SCGC5_PORTE) != 0)                              // if port is clocked
    #endif
    {
    #if defined KINETIS_KM
        GPIOE_PDIR = ((unsigned char)(ulPort_in_E & ~GPIOE_PDDR) | (GPIOE_PDOR & GPIOE_PDDR)); // input state
    #else
        ulNewState = (GPIOE_PDOR | GPIOE_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOE_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOE_PTOR;                                        // toggle bits from toggle register
        GPIOE_PDOR = ulNewState;
        GPIOE_PDIR = ((ulPort_in_E & ~GPIOE_PDDR) | (GPIOE_PDOR & GPIOE_PDDR)); // input state {10}
        if ((PORTE_GPCLR != 0) || (PORTE_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTE, PORTE_GPCHR, PORTE_GPCLR);
        }
    #endif
    }
    #if !defined KINETIS_KM
    GPIOE_PTOR = GPIOE_PSOR = GPIOE_PCOR = 0;                            // registers always read 0
    #endif
#endif
#if PORTS_AVAILABLE > 5
    if ((SIM_SCGC5 & SIM_SCGC5_PORTF) != 0) {                            // if port is clocked
    #if defined KINETIS_KM
        GPIOF_PDIR = ((unsigned char)(ulPort_in_F & ~GPIOF_PDDR) | (GPIOF_PDOR & GPIOF_PDDR)); // input state
    #else
        ulNewState = (GPIOF_PDOR | GPIOF_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOF_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOF_PTOR;                                        // toggle bits from toggle register
        GPIOF_PDOR = ulNewState;
        GPIOF_PDIR = ((ulPort_in_F & ~GPIOF_PDDR) | (GPIOF_PDOR & GPIOF_PDDR)); // input state {10}
        if ((PORTF_GPCLR != 0) || (PORTF_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTF, PORTF_GPCHR, PORTF_GPCLR);
        }
    #endif
    }
    #if !defined KINETIS_KM
    GPIOF_PTOR = GPIOF_PSOR = GPIOF_PCOR = 0;                            // registers always read 0
    #endif
#endif
#if PORTS_AVAILABLE > 6
    if ((SIM_SCGC5 & SIM_SCGC5_PORTG) != 0) {                            // if port is clocked
    #if defined KINETIS_KM
        GPIOG_PDIR = ((unsigned char)(ulPort_in_G & ~GPIOG_PDDR) | (GPIOG_PDOR & GPIOG_PDDR)); // input state
    #else
        ulNewState = (GPIOG_PDOR | GPIOG_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOG_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOG_PTOR;                                        // toggle bits from toggle register
        GPIOG_PDOR = ulNewState;
        GPIOG_PDIR = ((ulPort_in_G & ~GPIOG_PDDR) | (GPIOG_PDOR & GPIOG_PDDR)); // input state {10}
        if ((PORTG_GPCLR != 0) || (PORTG_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTG, PORTG_GPCHR, PORTG_GPCLR);
        }
    #endif
    }
    #if !defined KINETIS_KM
    GPIOG_PTOR = GPIOG_PSOR = GPIOG_PCOR = 0;                            // registers always read 0
    #endif
#endif
#if PORTS_AVAILABLE > 7
    if ((SIM_SCGC5 & SIM_SCGC5_PORTH) != 0) {                            // if port is clocked
    #if defined KINETIS_KM
        GPIOH_PDIR = ((unsigned char)(ulPort_in_H & ~GPIOH_PDDR) | (GPIOH_PDOR & GPIOH_PDDR)); // input state
    #else
        ulNewState = (GPIOH_PDOR | GPIOH_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOH_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOH_PTOR;                                        // toggle bits from toggle register
        GPIOH_PDOR = ulNewState;
        GPIOH_PDIR = ((ulPort_in_H & ~GPIOH_PDDR) | (GPIOH_PDOR & GPIOH_PDDR)); // input state {10}
        if ((PORTH_GPCLR != 0) || (PORTH_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTH, PORTH_GPCHR, PORTH_GPCLR);
        }
    #endif
    }
    #if !defined KINETIS_KM
    GPIOH_PTOR = GPIOH_PSOR = GPIOH_PCOR = 0;                            // registers always read 0
    #endif
#endif
#if PORTS_AVAILABLE > 8
    if ((SIM_SCGC5 & SIM_SCGC5_PORTI) != 0) {                            // if port is clocked
    #if defined KINETIS_KM
        GPIOI_PDIR = ((unsigned char)(ulPort_in_I & ~GPIOI_PDDR) | (GPIOI_PDOR & GPIOI_PDDR)); // input state
    #else
        ulNewState = (GPIOI_PDOR | GPIOI_PSOR);                          // set bits from set register
        ulNewState &= ~(GPIOI_PCOR);                                     // clear bits from clear register
        ulNewState ^= GPIOI_PTOR;                                        // toggle bits from toggle register
        GPIOI_PDOR = ulNewState;
        GPIOI_PDIR = ((ulPort_in_I & ~GPIOI_PDDR) | (GPIOI_PDOR & GPIOI_PDDR)); // input state {10}
        if ((PORTI_GPCLR != 0) || (PORTI_GPCHR != 0)) {
            fnSetPinCharacteristics(_PORTI, PORTI_GPCHR, PORTI_GPCLR);
        }
    #endif
    }
    #if !defined KINETIS_KM
    GPIOI_PTOR = GPIOI_PSOR = GPIOI_PCOR = 0;                            // registers always read 0
    #endif
#endif
}


// Update peripherals based on present port register settings - this is only called when a peripheral setting has been changed
//
extern void fnSimPers(void)
{
    int iPort = 0;
    int iPin = 0;
#if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
    unsigned long *ptrPortPin;
#endif
    unsigned long ulBit;
    for (iPort = 0; iPort < PORTS_AVAILABLE; iPort++) {
#if !defined KINETIS_KE || defined KINETIS_KE15 || defined KINETIS_KE18
        ptrPortPin = (unsigned long *)(PORT0_BLOCK + (iPort * sizeof(KINETIS_PORT)));
#endif
        ulPeripherals[iPort] = 0;
        ulBit = 0x00000001;
        for (iPin = 0; iPin < 32; iPin++) {
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
            // KE devices' peripheral functions are selected by the various peripherals that can use the pins. Each peripheral pin function has a priority and so the the one with highest is valid in case of multiple enabled peripherals on a pin
            // Peripheral can also have pin multiplexing options
            //
            if (iPort == 0) {
                switch (ulBit) {
                case KE_PORTA_BIT0:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD0) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_ADC0_SE0;
                        break;
                    }
    #endif
    #if (SIZE_OF_FLASH <= (8 * 1024))
                    if ((SIM_SOPT0 & SIM_SOPT_SWDE) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_SWD_CLK;
                        break;
                    }
    #endif
                    if (((FTM0_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_FTM0PS0) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_FTM0_CH0;
                    }
                    else if ((KBI0_PE & 0x01) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_0_KBI0_P0;
                    }
                    break;
                case KE_PORTA_BIT1:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD1) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_ADC0_SE1;
                        break;
                    }
    #endif
                    if ((FTM0_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && ((SIM_PINSEL0 & SIM_PINSEL_FTM0PS1) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_FTM0_CH1;                    
                    }
                    else if (KBI0_PE & 0x02) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_KBI0_P1;
                    }
    #if defined PA_1_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_1_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTA_BIT2:
                    if (((UART0_C2 & UART_C2_RE) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_UART0PS) != 0) && ((UART0_C1 & UART_C1_LOOPS) == 0)) { // UART0 rx enabled and mapped to PTA2 and PTA3 rather than PTB0 and PTB1 (and not in sigle-wire/loop back mode)
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_2_UART0_RX;
                    }
                    else if (((I2C0_C1 & I2C_IEN) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_I2C0PS) == 0)) { // if I2C is enabled and I2C0 not mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_2_I2C0_SDA;
                    }
                    else if ((KBI0_PE & 0x04) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_2_KBI0_P2;
                    }
                    break;
                case KE_PORTA_BIT3:
                    if (((UART0_C2 & UART_C2_TE) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_UART0PS) != 0)) { // UART0 tx enabled and mapped to PTA2 and PTA3 rather than PTB0 and PTB1
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_3_UART0_TX;
                    }
                    else if (((I2C0_C1 & I2C_IEN) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_I2C0PS) == 0)) { // if I2C is enabled and I2C0 not mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_3_I2C0_SCL;
                    }
                    else if ((KBI0_PE & 0x08) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_3_KBI0_P3;
                    }
                    break;
                case KE_PORTA_BIT4:
                    if ((SIM_SOPT0 & SIM_SOPT_SWDE) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_4_SWD_DIO;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x10) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_4_KBI0_P4;
                    }
    #endif
                    break;
                case KE_PORTA_BIT5:
                    if ((SIM_SOPT0 & SIM_SOPT_RSTPE) != 0) {             // default pin function as reset
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_5_RESET;
                    }
                    else if ((IRQ_SC & IRQ_SC_IRQPE) != 0) {             // IRQ pin function enabled
    #if defined SIM_PINSEL_IRQPS_PTI6
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTA5) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTA][iPin] = PA_5_IRQ;
                        }
    #else
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[iPort][iPin] = PA_5_IRQ;
    #endif
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x20) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_5_KBI0_P5;
                    }
    #endif
                    break;
                case KE_PORTA_BIT6:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD6) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_6_ADC0_SE2;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI0_PE & 0x40) != 0) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_6_KBI0_P6;
                        break;
                    }
    #endif
    #if defined PA_6_DEFAULT
                    ulPeripherals[iPort] |= ulBit;
                    ucPortFunctions[_PORTA][iPin] = PA_6_DEFAULT;
    #endif
                    break;
                case KE_PORTA_BIT7:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD7) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_7_ADC0_SE3;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI0_PE & 0x80) != 0) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTA][iPin] = PA_7_KBI0_P7;
                        break;
                    }
    #endif
    #if defined PA_7_DEFAULT
                    ulPeripherals[iPort] |= ulBit;
                    ucPortFunctions[_PORTA][iPin] = PA_7_DEFAULT;
    #endif
                    break;
                case KE_PORTB_BIT0:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD4) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_ADC0_SE4;
                        break;
                    }
    #endif
                    if (((UART0_C2 & UART_C2_RE) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_UART0PS) == 0) && ((UART0_C1 & UART_C1_LOOPS) == 0)) { // UART0 rx enabled and not mapped to PTA2 and PTA3 rather than PTB0 and PTB1 (and not in sigle-wire/loop back mode)
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_UART0_RX;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x100) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_KBI0_P8;
                    }
    #else
                    else if ((KBI0_PE & 0x10) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_0_KBI0_P4;
                    }
    #endif
                    break;
                case KE_PORTB_BIT1:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD5) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_ADC0_SE5;
                        break;
                    }
    #endif
                    if ((UART0_C2 & UART_C2_TE) && ((SIM_PINSEL0 & SIM_PINSEL_UART0PS) == 0)) { // UART0 tx enabled and not mapped to PTA2 and PTA3 rather than PTB0 and PTB1
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_UART0_TX;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x200) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_KBI0_P9;
                    }
    #else
                    else if ((KBI0_PE & 0x20) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_1_KBI0_P5;
                    }
    #endif
                    break;
                case KE_PORTB_BIT2:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD6) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_ADC0_SE6;
                        break;
                    }
    #endif
                    if ((FTM0_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM0PS0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_FTM0_CH0;                    
                    }
                    else if ((SPI0_C1 & SPI_C1_MSTR) && (!(SIM_PINSEL0 & SIM_PINSEL_SPI0PS))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_SPI0_SCK;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x400) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_KBI0_P10;
                    }
    #else
                    else if (KBI0_PE & 0x40) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_KBI0_P6;
                    }
    #endif
    #if defined PB_2_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_2_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT3:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD7) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_ADC0_SE7;
                        break;
                    }
    #endif
                    if ((FTM0_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM0PS1)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_FTM0_CH1;                    
                    }
                    else if ((SPI0_C1 & SPI_C1_MSTR) && (!(SIM_PINSEL0 & SIM_PINSEL_SPI0PS))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_SPI0_MOSI;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x800) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_KBI0_P11;
                    }
    #else
                    else if (KBI0_PE & 0x80) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_KBI0_P7;
                    }
    #endif
    #if defined PB_3_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_3_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT4:
                    if (SIM_SOPT0 & SIM_SOPT_NMIE) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_NMI;
                    }
                    else if ((SPI0_C1 & SPI_C1_MSTR) && ((SIM_PINSEL0 & SIM_PINSEL_SPI0PS) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_SPI0_MISO;
                    }
                    else if ((FTM2_C4SC & (FTM_CSC_ELSA | FTM_CSC_ELSB))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_FTM2_CH4;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x1000) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_KBI0_P12;
                    }
    #endif
    #if defined PB_4_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_4_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT5:
                    if (((SPI0_C1 & SPI_C1_MSTR) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_SPI0PS) == 0) && ((SPI0_C1 & SPI_C1_SSOE) != 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_SPI0_PCS0;
                    }
                    else if ((FTM2_C5SC & (FTM_CSC_ELSA | FTM_CSC_ELSB))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_FTM2_CH5;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x2000) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_KBI0_P13;
                    }
    #endif
    #if defined PB_5_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_5_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTB_BIT6:
                    if ((I2C0_C1 & I2C_IEN) && (SIM_PINSEL0 & SIM_PINSEL_I2C0PS)) { // if I2C is enabled and I2C0 is mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_6_I2C0_SDA;
                    }
                    else if ((OSC0_CR & OSC_CR_OSCEN) != 0) {            // crystal oscillator enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_6_XTAL;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x4000) != 0) {                  // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_6_KBI0_P14;
                    }
    #endif
                    break;
                case KE_PORTB_BIT7:
                    if (((I2C0_C1 & I2C_IEN) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_I2C0PS) != 0)) { // if I2C is enabled and I2C0 is mapped to PTB7 and PTB6 rather than PTA3 and PTA2
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_7_I2C0_SCL;
                    }
                    else if (OSC0_CR & OSC_CR_OSCEN) {                   // crystal oscillator enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_7_EXTAL;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x8000) != 0) {                  // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTB][iPin - 8] = PB_7_KBI0_P15;
                    }
    #endif
                    break;
                case KE_PORTC_BIT0:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD8) != 0) {           // if the ADC function is enabled it has priority
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_0_ADC0_SE8;
                        break;
                    }
    #endif
                    if (((FTM2_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) &&
    #if defined KINETIS_KE06 || defined KINETIS_KEA128 || (defined KINETIS_KEA64 && !defined KINETIS_KEAN64)
                    ((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS0_MASK) == SIM_PINSEL1_FTM2PS0_PTC0)
    #else
                        ((SIM_PINSEL0 & SIM_PINSEL_FTM2PS0) == 0)
    #endif
                    ) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_0_FTM2_CH0;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x10000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_0_KBI0_P16;
                    }
    #endif
    #if defined PC_0_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_0_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT1:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD9) != 0) {           // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_1_ADC0_SE9;
                        break;
                    }
    #endif
                    if (((FTM2_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && (0 == (SIM_PINSEL0 & SIM_PINSEL_FTM1PS1))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_1_FTM2_CH1;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x20000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_1_KBI0_P17;
                    }
    #endif
    #if defined PC_1_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_1_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT2:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD10) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_2_ADC0_SE10;
                        break;
                    }
    #endif
    #if ((defined SIM_PINSEL1) && ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 ||  defined KINETIS_KEA128))
                    if (((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && ((SIM_PINSEL1 & (SIM_PINSEL1_FTM2PS2_PTD0 | SIM_PINSEL1_FTM2PS2_PTG4)) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_2_FTM2_CH2;                    
                    }
    #else
                    if (((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_FTM2PS2) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[iPort + 2][iPin - 16] = PC_2_FTM2_CH2;                    
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x40000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_2_KBI0_P18;
                    }
    #endif
    #if defined PC_2_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_2_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT3:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD11) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 16] = PC_3_ADC0_SE11;
                        break;
                    }
    #endif
    #if ((defined SIM_PINSEL1) && ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                    if (((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && ((SIM_PINSEL1 & (SIM_PINSEL1_FTM2PS3_PTD1 | SIM_PINSEL1_FTM2PS3_PTG5)) == 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_FTM2_CH3;                    
                    }
    #else
                    if ((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM2PS3))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_FTM2_CH3;                    
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x80000) {                        // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_KBI0_P19;
                    }
    #endif
    #if defined PC_3_DEFAULT
                    else {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_3_DEFAULT;
                    }
    #endif
                    break;
                case KE_PORTC_BIT4:
                    if ((FTM1_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (!(SIM_PINSEL0 & SIM_PINSEL_FTM1PS0))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_FTM1_CH0;                    
                    }
    #if (SIZE_OF_FLASH > (8 * 1024))
                    else if (SIM_SOPT0 & SIM_SOPT_SWDE) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_SWD_CLK;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x100000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_KBI0_P20;
                    }
    #elif defined KINETIS_KEA8
                    else if (KBI1_PE & 0x000001) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_4_KBI1_P0;
                    }
    #endif
                    break;
                case KE_PORTC_BIT5:
                    if (((FTM1_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && (0 == (SIM_PINSEL0 & SIM_PINSEL_FTM1PS1))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_5_FTM1_CH1;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x200000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_5_KBI0_P21;
                    }
    #elif defined KINETIS_KEA8
                    else if (KBI1_PE & 0x000002) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_5_KBI1_P1;
                    }
    #endif
                    break;
                case KE_PORTC_BIT6:
    #if ((defined SIM_PINSEL1) && (defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                    if (((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) != 0) && ((SIM_PINSEL1 & SIM_PINSEL1_MSCANPS) == 0)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_6_CAN0_RX;   
                    }
                    else
    #endif
    #if UARTS_AVAILABLE > 1
                    if (((UART1_C2 & UART_C2_RE) != 0) && ((UART1_C1 & UART_C1_LOOPS) == 0)) { // UART1 rx enabled and not in single-wire/loop back mode
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_6_UART1_RX; 
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x400000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_6_KBI0_P22;
                    }
    #endif
                    break;
                case KE_PORTC_BIT7:
    #if ((defined SIM_PINSEL1) && (defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                    if (((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) != 0) && ((SIM_PINSEL1 & SIM_PINSEL1_MSCANPS) == 0)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_7_CAN0_TX;   
                    }
                    else
    #endif
    #if UARTS_AVAILABLE > 1
                    if (UART1_C2 & UART_C2_TE) {                         // UART1 tx enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_7_UART1_TX; 
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x800000) {                       // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTC][iPin - 16] = PC_7_KBI0_P23;
                    }
    #endif
                    break;
                case KE_PORTD_BIT0:
                    if (SPI1_C1 & SPI_C1_MSTR) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_SPI1_SCK; 
                    }
    #if ((defined SIM_PINSEL1) && ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                    else if (((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && (((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS2_PTD0) != 0) && ((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS2_PTG4) == 0))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_FTM2_CH2;                    
                    }
    #else
                    else if ((FTM2_C2SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM2PS2)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_FTM2_CH2;                    
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x1000000) {                      // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_KBI0_P24;
                    }
    #else
                    else if (KBI1_PE & 0x01) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_0_KBI1_P0;
                    }
    #endif
                    break;
                case KE_PORTD_BIT1:
                    if (SPI1_C1 & SPI_C1_MSTR) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_SPI1_MOSI; 
                    }
    #if ((defined SIM_PINSEL1) && ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 ||  defined KINETIS_KEA128))
                    else if (((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && (((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS3_PTD1) != 0) && ((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS3_PTG5) == 0))) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_FTM2_CH3;                    
                    }
    #else
                    else if ((FTM2_C3SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM2PS3)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_FTM2_CH3;                    
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI0_PE & 0x2000000) {                      // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_KBI0_P25;
                    }
    #else
                    else if (KBI1_PE & 0x02) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_1_KBI1_P1;
                    }
    #endif
                    break;
                case KE_PORTD_BIT2:
                    if ((SPI1_C1 & SPI_C1_MSTR) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_2_SPI1_MISO;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x4000000) != 0) {               // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_2_KBI0_P26;
                    }
    #else
                    else if ((KBI1_PE & 0x04) != 0) {                    // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_2_KBI1_P2;
                    }
    #endif
                    break;
                case KE_PORTD_BIT3:
                    if ((SPI1_C1 & SPI_C1_MSTR) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_3_SPI1_PCS0;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI0_PE & 0x8000000) != 0) {               // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_3_KBI0_P27;
                    }
    #else
                    else if (KBI1_PE & 0x08) {                           // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_3_KBI1_P3;
                    }
    #endif
                    break;
                case KE_PORTD_BIT4:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI0_PE & 0x10000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_4_KBI0_P28;
                    }
    #else
                    if (KBI1_PE & 0x10) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_4_KBI1_P4;
                    }
    #endif
                    break;
                case KE_PORTD_BIT5:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI0_PE & 0x20000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_5_KBI0_P29;
                    }
    #else
                    if (KBI1_PE & 0x20) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_5_KBI1_P5;
                    }
    #endif
                    break;
                case KE_PORTD_BIT6:
    #if UARTS_AVAILABLE > 2
                    if (((UART2_C2 & UART_C2_RE) != 0) && ((UART2_C1 & UART_C1_LOOPS) == 0)) { // UART2 rx enabled and not in single-wire/loop back mode
        #if defined SIM_PINSEL1
                        if ((SIM_PINSEL1 & SIM_PINSEL1_UART2PS) == 0) { // if not alternate pinout selected
        #endif
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTD][iPin - 24] = PD_6_UART2_RX;
                            break;
        #if defined SIM_PINSEL1
                        }
        #endif
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI0_PE & 0x40000000) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_6_KBI0_P30;
                        break;
                    }
    #else
                    if (KBI1_PE & 0x40) {                                // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_6_KBI1_P6;
                    }
    #endif
                    break;
                case KE_PORTD_BIT7:
    #if UARTS_AVAILABLE > 2
                    if ((UART2_C2 & UART_C2_TE) != 0) {                  // UART2 tx enabled
        #if defined SIM_PINSEL1
                        if ((SIM_PINSEL1 & SIM_PINSEL1_UART2PS) == 0) {  // if not alternate pinout selected
        #endif
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTD][iPin - 24] = PD_7_UART2_TX;
                            break;
        #if defined SIM_PINSEL1
                        }
        #endif
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI0_PE & 0x80000000) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_7_KBI0_P31;
                        break;
                    }
    #else
                    if ((KBI1_PE & 0x80) != 0) {                         // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTD][iPin - 24] = PD_7_KBI1_P7;
                    }
    #endif
                    break;
                }
            }
            else if (iPort == 1) {
                switch (ulBit) {
                case KE_PORTE_BIT0:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_0_SPI0_SCK;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000001) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_0_KBI1_P0;
                    }
    #endif
                    break;
                case KE_PORTE_BIT1:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_1_SPI0_MOSI;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000002) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_1_KBI1_P1;
                    }
    #endif
                    break;
                case KE_PORTE_BIT2:
                    if (((SPI0_C1 & SPI_C1_MSTR) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_SPI0PS) != 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_2_SPI0_MISO;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000004) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_2_KBI1_P2;
                    }
    #endif
                    break;
                case KE_PORTE_BIT3:
                    if ((SPI0_C1 & SPI_C1_MSTR) && (SPI0_C1 & SPI_C1_SSOE) && (SIM_PINSEL0 & SIM_PINSEL_SPI0PS)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_3_SPI0_PCS0;
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000008) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_3_KBI1_P3;
                    }
    #endif
                    break;
                case KE_PORTE_BIT4:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000010) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_4_KBI1_P4;
                    }
    #endif
                    break;
                case KE_PORTE_BIT5:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x00000020) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_5_KBI1_P5;
                    }
    #endif
                    break;
                case KE_PORTE_BIT6:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x00000040) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_6_KBI1_P6;
                    }
    #endif
                    break;
                case KE_PORTE_BIT7:
    #if ((defined SIM_PINSEL1) && (defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                    if (((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) != 0) && ((SIM_PINSEL1 & SIM_PINSEL1_MSCANPS) != 0)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_7_CAN0_TX;   
                    }
                    else
    #endif
                    if ((FTM1_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) && (SIM_PINSEL0 & SIM_PINSEL_FTM1PS1)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_7_FTM1_CH1;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if (KBI1_PE & 0x00000080) {                     // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTE][iPin] = PE_7_KBI1_P7;
                    }
    #endif
                    break;
                case KE_PORTF_BIT0:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000100) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_0_KBI1_P8;
                    }
    #endif
                    break;
                case KE_PORTF_BIT1:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000200) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_1_KBI1_P9;
                    }
    #endif
                    break;
                case KE_PORTF_BIT2:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00000400) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_2_KBI1_P10;
                    }
    #endif
                    break;
                case KE_PORTF_BIT3:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x00000800) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_3_KBI1_P11;
                    }
    #endif
                    break;
                case KE_PORTF_BIT4:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD12) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_4_ADC0_SE12;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x00001000) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_4_KBI1_P12;
                    }
    #endif
                    break;
                case KE_PORTF_BIT5:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD13) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_5_ADC0_SE13;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00002000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_5_KBI1_P13;
                    }
    #endif
                    break;
                case KE_PORTF_BIT6:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD14) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_6_ADC0_SE14;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00004000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_6_KBI1_P14;
                    }
    #endif
                    break;
                case KE_PORTF_BIT7:
    #if defined KINETIS_KE02 || defined KINETIS_KE06
                    if ((ADC0_APCTL1 & ADC_APCTL1_AD15) != 0) {          // if the ADC function is enabled
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_7_ADC0_SE15;
                        break;
                    }
    #endif
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00008000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTF][iPin - 8] = PF_7_KBI1_P15;
                    }
    #endif
                    break;
                case KE_PORTG_BIT0:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00010000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_0_KBI1_P16;
                    }
    #endif
                    break;
                case KE_PORTG_BIT1:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00020000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_1_KBI1_P17;
                    }
    #endif
                    break;
                case KE_PORTG_BIT2:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00040000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_2_KBI1_P18;
                    }
    #endif
                    break;
                case KE_PORTG_BIT3:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00080000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_3_KBI1_P19;
                    }
    #endif
                    break;
                case KE_PORTG_BIT4:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00100000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_4_KBI1_P20;
                    }
    #endif
                    break;
                case KE_PORTG_BIT5:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00200000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_5_KBI1_P21;
                    }
    #endif
                    break;
                case KE_PORTG_BIT6:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00400000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_6_KBI1_P22;
                    }
    #endif
                    break;
                case KE_PORTG_BIT7:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x00800000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTG][iPin - 16] = PG_7_KBI1_P23;
                    }
    #endif
                    break;
                case KE_PORTH_BIT0:
                    if (((FTM2_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) &&
    #if defined KINETIS_KE06 || defined KINETIS_KEA128 || (defined KINETIS_KEA64 && !defined KINETIS_KEAN64)
                        ((SIM_PINSEL1 & SIM_PINSEL1_FTM2PS0_MASK) == SIM_PINSEL1_FTM2PS0_PTH0)
    #else
                        ((SIM_PINSEL0 & SIM_PINSEL_FTM2PS0) != 0)
    #endif
                        ) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_0_FTM2_CH0;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI1_PE & 0x01000000) != 0) {              // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_0_KBI1_P24;
                    }
    #endif
                    break;
                case KE_PORTH_BIT1:
                    if (((FTM2_C1SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_FTM1PS1) != 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_1_FTM2_CH1;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI1_PE & 0x02000000) != 0) {              // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_1_KBI1_P25;
                    }
    #endif
                    break;
                case KE_PORTH_BIT2:
    #if ((defined SIM_PINSEL1) && (defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128))
                    if (((MSCAN_CANCTL1 & MSCAN_CANCTL1_CANE) != 0) && ((SIM_PINSEL1 & SIM_PINSEL1_MSCANPS) != 0)) { // pin is enabled as CAN function
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_2_CAN0_RX;   
                    }
                    else
    #endif
                    if (((FTM1_C0SC & (FTM_CSC_ELSA | FTM_CSC_ELSB)) != 0) && ((SIM_PINSEL0 & SIM_PINSEL_FTM1PS0) != 0)) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_2_FTM1_CH0;                    
                    }
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    else if ((KBI1_PE & 0x04000000) != 0) {              // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_2_KBI1_P26;
                    }
    #endif
                    break;
                case KE_PORTH_BIT3:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if ((KBI1_PE & 0x08000000) != 0) {                   // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_3_KBI1_P27;
                    }
    #endif
                    break;
                case KE_PORTH_BIT4:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x10000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_4_KBI1_P28;
                    }
    #endif
                    break;
                case KE_PORTH_BIT5:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x20000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_5_KBI1_P29;
                    }
    #endif
                    break;
                case KE_PORTH_BIT6:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x40000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_6_KBI1_P30;
                    }
    #endif
                    break;
                case KE_PORTH_BIT7:
    #if (defined KINETIS_KE04 && !(SIZE_OF_FLASH <= (8 * 1024))) || defined KINETIS_KE06 || defined KINETIS_KEA64 || defined KINETIS_KEA128
                    if (KBI1_PE & 0x80000000) {                          // pin is enabled as keyboard interrupt
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTH][iPin - 24] = PH_7_KBI1_P31;
                    }
    #endif
                    break;
                }
            }
    #if defined SIM_PINSEL_IRQPS_PTI6
            else {
                switch (ulBit) {
                case KE_PORTI_BIT0:
                    if ((IRQ_SC & IRQ_SC_IRQPE) != 0) {                  // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI0) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_0_IRQ;
                            break;
                        }
                    }
                    if ((SIM_PINSEL1 & SIM_PINSEL1_UART2PS) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTI][iPin] = PI_0_UART2_RX;  
                    }
                    break;
                case KE_PORTI_BIT1:
                    if ((IRQ_SC & IRQ_SC_IRQPE) != 0) {                  // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI1) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_1_IRQ;
                            break;
                        }
                    }
                    if ((SIM_PINSEL1 & SIM_PINSEL1_UART2PS) != 0) {
                        ulPeripherals[iPort] |= ulBit;
                        ucPortFunctions[_PORTI][iPin] = PI_1_UART2_TX;
                    }
                    break;
                case KE_PORTI_BIT2:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI2) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_2_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT3:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI3) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_3_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT4:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI4) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_4_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT5:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI5) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_5_IRQ;
                        }
                    }
                    break;
                case KE_PORTI_BIT6:
                    if (IRQ_SC & IRQ_SC_IRQPE) {                         // IRQ pin function enabled
                        if ((SIM_PINSEL0 & SIM_PINSEL_IRQPS_PTI6) == SIM_PINSEL_IRQPS_PTI6) {
                            ulPeripherals[iPort] |= ulBit;
                            ucPortFunctions[_PORTI][iPin] = PI_6_IRQ;
                        }
                    }
                    break;
                }
            }
    #endif
#else
            ucPortFunctions[iPort][iPin] = (unsigned char)((*ptrPortPin++ & PORT_MUX_MASK) >> 8);
            if (ucPortFunctions[iPort][iPin] != (PORT_MUX_GPIO >> 8)) {
                if ((ucPortFunctions[iPort][iPin] != 0) || ((ulDisabled[iPort] & ulBit) == 0)) { // not ALT 0 function (usually default) or default peripheral is not disabled
                    ulPeripherals[iPort] |= ulBit;
                }
            }
    #if defined ERRATA_ID_3402
            if ((iPort == XTAL0_PORT) && (iPin == XTAL0_PIN)) {
                if ((OSC0_CR & OSC_CR_ERCLKEN) != 0) {                   // if OSC is enabled the XTAL pin is overridden by the oscillator functions
                    ucPortFunctions[iPort][iPin] = 0;
                    ulPeripherals[iPort] |= ulBit;
                }
            }
    #endif
#endif
            ulBit <<= 1;
        }
    }
    iFlagRefresh = PORT_CHANGE;
}

extern int fnSimulateDMA(int channel, unsigned char ucTriggerSource)     // {3}
{
#if !defined DEVICE_WITHOUT_DMA
#if (defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA // {32}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += channel;
    if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {                          // peripheral trigger
        if (ucTriggerSource != 0) {
            unsigned char *ptrDMUX = (DMAMUX0_CHCFG_ADD + channel);
            // Check that the trigger source is correctly connected to the DMA channel
            //
            if ((*ptrDMUX & ~(DMAMUX_CHCFG_TRIG | DMAMUX_CHCFG_ENBL)) != ucTriggerSource) {
                _EXCEPTION("DMUX source is not connected!!!");
            }
        }
    }
    if (((ptrDMA->DMA_DCR & (DMA_DCR_START | DMA_DCR_ERQ)) != 0) && ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) != 0)) { // sw commanded start or source request (ignore if no count value remaining)
        ptrDMA->DMA_DSR_BCR |= DMA_DSR_BCR_BSY;
        while ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) != 0) {      // while bytes to be transferred
            if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_8) != 0) {              // 8 bit transfers
                *(unsigned char *)ptrDMA->DMA_DAR = *(unsigned char *)ptrDMA->DMA_SAR; // byte transfer
                ptrDMA->DMA_DSR_BCR = ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) - 1);
            }
            else if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_16) != 0) {        // 16 bit transfers
                *(unsigned short *)ptrDMA->DMA_DAR = *(unsigned short *)ptrDMA->DMA_SAR; // short word transfer
                ptrDMA->DMA_DSR_BCR = ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) - 2);
            }
            else {                                                       // 32 bit transfers
                *(unsigned long *)ptrDMA->DMA_DAR = *(unsigned long *)ptrDMA->DMA_SAR; // long word transfer
                ptrDMA->DMA_DSR_BCR = ((ptrDMA->DMA_DSR_BCR & DMA_DSR_BCR_BCR_MASK) - 4);
            }
            if ((ptrDMA->DMA_DCR & DMA_DCR_DINC) != 0) {                 // destination increment enabled
                if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_8) != 0) {
                    ptrDMA->DMA_DAR = (ptrDMA->DMA_DAR + 1);
                }
                else if ((ptrDMA->DMA_DCR & DMA_DCR_DSIZE_16) != 0) {
                    ptrDMA->DMA_DAR = (ptrDMA->DMA_DAR + 2);
                }
                else {
                    ptrDMA->DMA_DAR = (ptrDMA->DMA_DAR + 4);
                }
            }
            if ((ptrDMA->DMA_DCR & DMA_DCR_SINC) != 0) {                 // source increment enabled
                if ((ptrDMA->DMA_DCR & DMA_DCR_SSIZE_8) != 0) {
                    ptrDMA->DMA_SAR  = (ptrDMA->DMA_SAR + 1);
                }
                else if ((ptrDMA->DMA_DCR & DMA_DCR_SSIZE_16) != 0) {
                    ptrDMA->DMA_SAR  = (ptrDMA->DMA_SAR + 2);
                }
                else {
                    ptrDMA->DMA_SAR  = (ptrDMA->DMA_SAR + 4);
                }
            }
            if ((ptrDMA->DMA_DCR & DMA_DCR_CS) != 0) {                   // if in cycle-steal mode only one transfer is performed at a time
                unsigned long ulLength = 0;
                switch (ptrDMA->DMA_DCR & DMA_DCR_SMOD_256K) {           // handle automatic source modulo buffer operation
                case DMA_DCR_SMOD_16:
                    ulLength = 16;
                    break;
                case DMA_DCR_SMOD_32:
                    ulLength = 32;
                    break;
                case DMA_DCR_SMOD_64:
                    ulLength = 64;
                    break;
                case DMA_DCR_SMOD_128:
                    ulLength = 128;
                    break;
                case DMA_DCR_SMOD_256:
                    ulLength = 256;
                    break;
                case DMA_DCR_SMOD_512:
                    ulLength = 512;
                    break;
                case DMA_DCR_SMOD_1K:
                    ulLength = 1024;
                    break;
                case DMA_DCR_SMOD_2K:
                    ulLength = (2 * 1024);
                    break;
                case DMA_DCR_SMOD_4K:
                    ulLength = (4 * 1024);
                    break;
                case DMA_DCR_SMOD_8K:
                    ulLength = (8 * 1024);
                    break;
                case DMA_DCR_SMOD_16K:
                    ulLength = (16 * 1024);
                    break;
                case DMA_DCR_SMOD_32K:
                    ulLength = (32 * 1024);
                    break;
                case DMA_DCR_SMOD_64K:
                    ulLength = (64 * 1024);
                    break;
                case DMA_DCR_SMOD_128K:
                    ulLength = (128 * 1024);
                    break;
                case DMA_DCR_SMOD_256K:
                    ulLength = (256 * 1024);
                    break;
                default:
                    break;
                }
                if (ulLength != 0) {
                    if ((ptrDMA->DMA_SAR % ulLength) == 0) {
                        ptrDMA->DMA_SAR -= ulLength;
                    }
                }
                switch (ptrDMA->DMA_DCR & DMA_DCR_DMOD_256K) {           // handle automatic destination modulo buffer operation
                case DMA_DCR_DMOD_16:
                    ulLength = 16;
                    break;
                case DMA_DCR_DMOD_32:
                    ulLength = 32;
                    break;
                case DMA_DCR_DMOD_64:
                    ulLength = 64;
                    break;
                case DMA_DCR_DMOD_128:
                    ulLength = 128;
                    break;
                case DMA_DCR_DMOD_256:
                    ulLength = 256;
                    break;
                case DMA_DCR_DMOD_512:
                    ulLength = 512;
                    break;
                case DMA_DCR_DMOD_1K:
                    ulLength = 1024;
                    break;
                case DMA_DCR_DMOD_2K:
                    ulLength = (2 * 1024);
                    break;
                case DMA_DCR_DMOD_4K:
                    ulLength = (4 * 1024);
                    break;
                case DMA_DCR_DMOD_8K:
                    ulLength = (8 * 1024);
                    break;
                case DMA_DCR_DMOD_16K:
                    ulLength = (16 * 1024);
                    break;
                case DMA_DCR_DMOD_32K:
                    ulLength = (32 * 1024);
                    break;
                case DMA_DCR_DMOD_64K:
                    ulLength = (64 * 1024);
                    break;
                case DMA_DCR_DMOD_128K:
                    ulLength = (128 * 1024);
                    break;
                case DMA_DCR_DMOD_256K:
                    ulLength = (256 * 1024);
                    break;
                default:
                    ulLength = 0;
                    break;
                }
                if (ulLength != 0) {
                    if ((ptrDMA->DMA_DAR % ulLength) == 0) {
                        ptrDMA->DMA_DAR -= ulLength;
                    }
                }
//                break;
            }
            if (ptrDMA->DMA_DSR_BCR != 0) {
                return 1;                                                // still active
            }
        }
        ptrDMA->DMA_DSR_BCR |= DMA_DSR_BCR_DONE;
        if ((ptrDMA->DMA_DCR & DMA_DCR_EINT) != 0) {                     // if interrupt is enabled
            if (fnGenInt(irq_DMA0_ID + channel) != 0) {                  // if DMA channel interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                switch (channel) {
                case 0:
                    ptrVect->processor_interrupts.irq_DMA0();            // call the interrupt handler for DMA channel 0
                    break;
                case 1:
                    ptrVect->processor_interrupts.irq_DMA1();            // call the interrupt handler for DMA channel 1
                    break;
                case 2:
                    ptrVect->processor_interrupts.irq_DMA2();            // call the interrupt handler for DMA channel 2
                    break;
                case 3:
                    ptrVect->processor_interrupts.irq_DMA3();            // call the interrupt handler for DMA channel 3
                    break;
                }
            }
        }
        return 0;
    }
#elif !defined KINETIS_KE || defined DEVICE_WITH_eDMA
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += channel;

    if (channel >= DMA_CHANNEL_COUNT) {
        _EXCEPTION("Warning - invalid DMA channel being used!!");
    }
    if (ucTriggerSource != 0) {
        ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_ACTIVE;
    }
    if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_ACTIVE) != 0) {           // peripheral trigger
      //if (ucTriggerSource != 0) {
            unsigned char *ptrDMUX = (DMAMUX0_CHCFG_ADD + channel);
            // Check that the trigger source is correctly connected to the DMA channel
            //
            if ((*ptrDMUX & ~(DMAMUX_CHCFG_TRIG | DMAMUX_CHCFG_ENBL)) != ucTriggerSource) {
                _EXCEPTION("DMUX source is not connected!!!");
            }
      //}
    }
    if ((ptrDMA_TCD->DMA_TCD_CSR & (DMA_TCD_CSR_START | DMA_TCD_CSR_ACTIVE)) != 0) { // sw commanded start or active
        int interrupt = 0;
        ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_ACTIVE;                   // mark active 
        ptrDMA_TCD->DMA_TCD_CSR &= ~DMA_TCD_CSR_START;                   // clear the start bit
        if (ptrDMA_TCD->DMA_TCD_CITER_ELINK != 0) {                      // main loop iterations
            unsigned long ulMinorLoop = ptrDMA_TCD->DMA_TCD_NBYTES_ML;   // the number of bytes to transfer
            if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_DSIZE_32) != 0) { // {36} handle long word transfers
                if ((ptrDMA_TCD->DMA_TCD_DOFF & 0x3) != 0) {
                    _EXCEPTION("DMA destination offset error!!");
                }
                if ((ptrDMA_TCD->DMA_TCD_SOFF & 0x3) != 0) {
                    _EXCEPTION("DMA source offset error!!");
                }
                if ((ulMinorLoop & 0x3) != 0) {
                    _EXCEPTION("DMA copy size error!!");
                }
                while (ulMinorLoop != 0) {
                    *(unsigned long *)ptrDMA_TCD->DMA_TCD_DADDR = *(unsigned long *)ptrDMA_TCD->DMA_TCD_SADDR; // long word transfer
                    ptrDMA_TCD->DMA_TCD_DADDR = ptrDMA_TCD->DMA_TCD_DADDR + ptrDMA_TCD->DMA_TCD_DOFF;
                    ptrDMA_TCD->DMA_TCD_SADDR = ptrDMA_TCD->DMA_TCD_SADDR + ptrDMA_TCD->DMA_TCD_SOFF;
                    ulMinorLoop -= sizeof(unsigned long);
                }
            }
            else if ((ptrDMA_TCD->DMA_TCD_ATTR & DMA_TCD_ATTR_DSIZE_16) != 0) { // {36} handle short word transfers
                if ((ptrDMA_TCD->DMA_TCD_DOFF & 0x1) != 0) {
                    _EXCEPTION("DMA destination offset error!!");
                }
                if ((ptrDMA_TCD->DMA_TCD_SOFF & 0x1) != 0) {
                    _EXCEPTION("DMA source offset error!!");
                }
                if ((ulMinorLoop & 0x1) != 0) {
                    _EXCEPTION("DMA copy size error!!");
                }
                while (ulMinorLoop != 0) {
                    *(unsigned short *)ptrDMA_TCD->DMA_TCD_DADDR = *(unsigned short *)ptrDMA_TCD->DMA_TCD_SADDR; // short word transfer
                    ptrDMA_TCD->DMA_TCD_DADDR = (ptrDMA_TCD->DMA_TCD_DADDR + ptrDMA_TCD->DMA_TCD_DOFF);
                    ptrDMA_TCD->DMA_TCD_SADDR = (ptrDMA_TCD->DMA_TCD_SADDR + ptrDMA_TCD->DMA_TCD_SOFF);
                    if (ulMinorLoop <= sizeof(unsigned short)) {
                        ulMinorLoop = 0;
                    }
                    else {
                        ulMinorLoop -= sizeof(unsigned short);
                    }
                }
            }
            else {
                while (ulMinorLoop-- != 0) {                             // minor loop count
                    *(unsigned char *)ptrDMA_TCD->DMA_TCD_DADDR = *(unsigned char *)ptrDMA_TCD->DMA_TCD_SADDR; // byte transfer
                    ptrDMA_TCD->DMA_TCD_DADDR = ptrDMA_TCD->DMA_TCD_DADDR + ptrDMA_TCD->DMA_TCD_DOFF;
                    ptrDMA_TCD->DMA_TCD_SADDR = ptrDMA_TCD->DMA_TCD_SADDR + ptrDMA_TCD->DMA_TCD_SOFF;
                }
            }
            (ptrDMA_TCD->DMA_TCD_CITER_ELINK)--;
            if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {                  // major loop completed
                if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_INTMAJOR) != 0) { // {18}
                    interrupt = 1;                                       // possible interrupt
                }
                if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_DREQ) != 0) { // disable on completion of major loop
                    DMA_ERQ &= ~(DMA_ERQ_ERQ0 << channel);
                    ptrDMA_TCD->DMA_TCD_CSR &= ~DMA_TCD_CSR_ACTIVE;      // completed
                }
                else {                                                   // continuous operation
                    ptrDMA_TCD->DMA_TCD_CITER_ELINK = ptrDMA_TCD->DMA_TCD_BITER_ELINK; // set back the main loop count value
                }
                ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_DONE;
                ptrDMA_TCD->DMA_TCD_DADDR += ptrDMA_TCD->DMA_TCD_DLASTSGA;
                ptrDMA_TCD->DMA_TCD_SADDR += ptrDMA_TCD->DMA_TCD_SLAST;
            }
            else if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == (ptrDMA_TCD->DMA_TCD_BITER_ELINK/2)) { // half complete
                if ((ptrDMA_TCD->DMA_TCD_CSR & DMA_TCD_CSR_INTHALF) != 0) { // check whether half-buffer interrupt has been configured
                    interrupt = 1;
                }
            }

            if (interrupt != 0) {                                        // if possible interrupt to generate
                DMA_INT |= (DMA_INT_INT0 << channel);
    #if defined eDMA_SHARES_INTERRUPTS
                if (fnGenInt(irq_DMA0_ID + (channel%(DMA_CHANNEL_COUNT/2))) != 0)
    #else
                if (fnGenInt(irq_DMA0_ID + channel) != 0)
    #endif
                {                                                        // if DMA channel interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    switch (channel) {
                    case 0:
    #if defined eDMA_SHARES_INTERRUPTS
                    case (0 + (DMA_CHANNEL_COUNT / 2)):
    #endif
                        ptrVect->processor_interrupts.irq_DMA0();    // call the interrupt handler for DMA channel 0 (and possibly shared channel)
                        break;
                    case 1:
    #if defined eDMA_SHARES_INTERRUPTS
                    case (1 + (DMA_CHANNEL_COUNT / 2)):
    #endif
                        ptrVect->processor_interrupts.irq_DMA1();        // call the interrupt handler for DMA channel 1 (and possibly shared channel)
                        break;
                    case 2:
    #if defined eDMA_SHARES_INTERRUPTS
                    case (2 + (DMA_CHANNEL_COUNT / 2)):
    #endif
                        ptrVect->processor_interrupts.irq_DMA2();        // call the interrupt handler for DMA channel 2 (and possibly shared channel)
                        break;
                    case 3:
    #if defined eDMA_SHARES_INTERRUPTS
                    case (3 + (DMA_CHANNEL_COUNT / 2)):
    #endif
                        ptrVect->processor_interrupts.irq_DMA3();        // call the interrupt handler for DMA channel 3 (and possibly shared channel)
                        break;
    #if (DMA_CHANNEL_COUNT > 4) && defined irq_DMA4_ID
                    case 4:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (4 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA4();        // call the interrupt handler for DMA channel 4 (and possibly shared channel)
                        break;
                    case 5:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (5 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA5();        // call the interrupt handler for DMA channel 5 (and possibly shared channel)
                        break;
                    case 6:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (6 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA6();        // call the interrupt handler for DMA channel 6 (and possibly shared channel)
                        break;
                    case 7:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (7 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA7();        // call the interrupt handler for DMA channel 7 (and possibly shared channel)
                        break;
    #endif
    #if (DMA_CHANNEL_COUNT > 8) && defined irq_DMA8_ID
                    case 8:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (8 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA8();        // call the interrupt handler for DMA channel 8 (and possibly shared channel)
                        break;
                    case 9:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (9 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA9();        // call the interrupt handler for DMA channel 9 (and possibly shared channel)
                        break;
                    case 10:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (10 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA10();       // call the interrupt handler for DMA channel 10 (and possibly shared channel)
                        break;
                    case 11:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (11 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA11();       // call the interrupt handler for DMA channel 11 (and possibly shared channel)
                        break;
                    case 12:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (12 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA12();       // call the interrupt handler for DMA channel 12 (and possibly shared channel)
                        break;
                    case 13:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (13 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA13();       // call the interrupt handler for DMA channel 13 (and possibly shared channel)
                        break;
                    case 14:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (14 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA14();       // call the interrupt handler for DMA channel 14 (and possibly shared channel)
                        break;
                    case 15:
        #if defined eDMA_SHARES_INTERRUPTS
                    case (15 + (DMA_CHANNEL_COUNT / 2)):
        #endif
                        ptrVect->processor_interrupts.irq_DMA15();       // call the interrupt handler for DMA channel 15 (and possibly shared channel)
                        break;
    #endif
                    }
                }
            }
            if (ptrDMA_TCD->DMA_TCD_CITER_ELINK == 0) {
                return 0;                                                // completed
            }
        }
        return 1;                                                        // not completed
    }
#endif
#endif
    return -1;                                                           // no operation
}

#if !defined DEVICE_WITHOUT_DMA
// Handler peripheral DMA triggers
//
static void fnHandleDMA_triggers(int iTriggerSource, int iDMAmux)
{
    #if defined KINETIS_KM
    int iMuxChannels = 1;                                                // KM DMA has one DMAMUX per channel
    #else
    int iMuxChannels = DMA_CHANNEL_COUNT;                                // DMAMUX has the same channel count as there are DMA channels
    #endif
    unsigned char *ptrMux;
    int iChannel = 0;
    switch (iDMAmux) {
    case 0:
        ptrMux = DMAMUX0_CHCFG_ADD;
        break;
    #if defined DMAMUX1_CHCFG_ADD
    case 1:
        ptrMux = DMAMUX1_CHCFG_ADD;
        break;
    #endif
    #if defined DMAMUX2_CHCFG_ADD
    case 2:
        ptrMux = DMAMUX2_CHCFG_ADD;
        break;
    #endif
    #if defined DMAMUX3_CHCFG_ADD
    case 3:
        ptrMux = DMAMUX3_CHCFG_ADD;
        break;
    #endif
    default:
        return;
    }

    #if defined TRGMUX_AVAILABLE
    if ((iTriggerSource & DMAMUX_CHCFG_TRIG) != 0) {                     // periodic trigger (LPIT source)
        iTriggerSource >>= 8;                                            // the LPIT that is being checked for
        if ((TRGMUX_DMAMUX0 & TRGMUX_SEL0) == (TRGMUX_SEL_LPIT0_CHANNEL_0 + iTriggerSource)) { // check that the LPIT trigger is connected to the DMAMUX
            iTriggerSource = DMAMUX0_CHCFG_SOURCE_DMAMUX0;
            iMuxChannels = 4;                                            // period triggers are limited to the first 4 channels
        }
        else {
            return;                                                      // trigger is not connected so it can't trigger a DMA request
        }
    }
    #endif
    iTriggerSource &= ~(DMAMUX_CHCFG_TRIG);
    while (iChannel < iMuxChannels) {
        if ((*ptrMux++ & ~(DMAMUX_CHCFG_TRIG)) == (DMAMUX_CHCFG_ENBL | (unsigned char)iTriggerSource)) { // matching enabled trigger
    #if defined _WINDOWS && !defined TRGMUX_AVAILABLE && !defined KINETIS_KM
            if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT0 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 0) {
                    _EXCEPTION("PIT0 trigger only operates on DMA channel 0!!");
                }
            }
        #if !defined KINETIS_KL82                                        // not yet supported
            else if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT1 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 1) {
                    _EXCEPTION("PIT1 trigger only operates on DMA channel 1!!");
                }
            }
            else if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT2 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 2) {
                    _EXCEPTION("PIT2 trigger only operates on DMA channel 2!!");
                }
            }
            else if ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT3 & ~(DMAMUX_CHCFG_TRIG)) == (unsigned char)iTriggerSource) {
                if (iChannel != 3) {
                    _EXCEPTION("PIT3 trigger only operates on DMA channel 3!!");
                }
            }
        #endif
    #endif
    #if (!defined KINETIS_KL && !defined KINETIS_KM) || defined DEVICE_WITH_eDMA
            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << iChannel)) != 0) {           // if the DMA channel is enabled
                KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
                ptrDMA_TCD += iChannel;
                ptrDMA_TCD->DMA_TCD_CSR |= (DMA_TCD_CSR_ACTIVE);         // trigger
                fnSimulateDMA(iChannel, (unsigned char)iTriggerSource);
            }
    #else
            fnSimulateDMA(iChannel, (unsigned char)iTriggerSource);
    #endif
        }
        iChannel++;
    }
}
#endif


// Handle port interrupts on input changes
//
static void fnPortInterrupt(int iPort, unsigned long ulNewState, unsigned long ulChangedBit, unsigned long *ptrPortConfig)
{
#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    if ((SIM_SCGC_IRQ & SIM_SCGC) != 0) {                                // if the IRQ module is enabled
        fnHandleIRQ(iPort, ulNewState, ulChangedBit, ptrPortConfig);
    }
    #if KBIS_AVAILABLE > 0
    if ((SIM_SCGC_KBI0 & SIM_SCGC) != 0) {                               // if the KBI0 module is enabled
        fnHandleKBI(0, iPort, ulNewState, ulChangedBit, ptrPortConfig);
    }
    #endif
    #if KBIS_AVAILABLE > 1
    if ((SIM_SCGC_KBI1 & SIM_SCGC) != 0) {                               // if the KBI1 module is enabled
        fnHandleKBI(1, iPort, ulNewState, ulChangedBit, ptrPortConfig);
    }
    #endif
#else
    int iPortInterruptSource;
    if ((*ptrPortConfig & PORT_IRQC_INT_MASK) == 0) {                    // no interrupt/DMA function programmed
        return;
    }
    switch (*ptrPortConfig & PORT_IRQC_INT_MASK)  {                      // interrupt/DMA property of the pin
    case PORT_IRQC_BOTH:
    case PORT_IRQC_DMA_BOTH:
        break;
    case PORT_IRQC_LOW_LEVEL:
    case PORT_IRQC_FALLING:
    case PORT_IRQC_DMA_FALLING:
        if ((ulNewState & ulChangedBit) != 0) {
            return;                                                      // not correct level
        }
        break;
    case PORT_IRQC_HIGH_LEVEL:
    case PORT_IRQC_RISING:
    case PORT_IRQC_DMA_RISING:
        if ((ulNewState & ulChangedBit) == 0) {
            return;                                                      // not correct level
        }
        break;
    }
    #if !defined DEVICE_WITHOUT_DMA
    if ((*ptrPortConfig & PORT_IRQC_LOW_LEVEL) == 0) {                   // DMA trigger and not interrupt
        #if defined DMAMUX1_CHCFG_SOURCE_PORTF
        if (iPort == _PORTF) {
            fnHandleDMA_triggers(DMAMUX1_CHCFG_SOURCE_PORTF, 1);         // port F is on second DMAMUX
        }
        else {
        #endif
            fnHandleDMA_triggers((DMAMUX0_CHCFG_SOURCE_PORTA + iPort), 0);
        #if defined DMAMUX1_CHCFG_SOURCE_PORTF
        }
        #endif
        return;
    }
    #endif
    *ptrPortConfig |= PORT_ISF;
    #if defined irq_PORTA_ID
    iPortInterruptSource = (irq_PORTA_ID + iPort);
    #endif
    switch (iPort) {
    case _PORTA:
        PORTA_ISFR |= ulChangedBit;
    #if defined irq_PORT_A_E_ID
        iPortInterruptSource = irq_PORT_A_E_ID;
    #endif
        break;
    #if PORTS_AVAILABLE > 1
    case _PORTB:
        PORTB_ISFR |= ulChangedBit;
        #if defined irq_PORT_B_C_D_ID                                    // shared port B/C/D
        iPortInterruptSource = irq_PORT_B_C_D_ID;
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 2
    case _PORTC:
        PORTC_ISFR |= ulChangedBit;
        #if defined irq_PORTC_D_ID                                       // shared port C/D interrupt vector
        iPortInterruptSource = irq_PORTC_D_ID;
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 3
    case _PORTD:
        PORTD_ISFR |= ulChangedBit;
        #if defined irq_PORTC_D_ID                                       // shared port C/D interrupt vector
        iPortInterruptSource = irq_PORTC_D_ID;
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 4
    case _PORTE:
        PORTE_ISFR |= ulChangedBit;
        #if defined irq_PORTBCD_E_ID                                     // shared port B/C/D/E
        iPortInterruptSource = irq_PORTBCD_E_ID;
        #elif defined irq_PORT_A_E_ID
        iPortInterruptSource = irq_PORT_A_E_ID;
        #endif
        break;
    #endif
    #if PORTS_AVAILABLE > 5
    case _PORTF:
        PORTF_ISFR |= ulChangedBit;
        break;
    #endif
    #if PORTS_AVAILABLE > 6
    case _PORTG:
        PORTG_ISFR |= ulChangedBit;
        break;
    #endif
    #if PORTS_AVAILABLE > 7
    case _PORTH:
        PORTH_ISFR |= ulChangedBit;
        break;
    #endif
    #if PORTS_AVAILABLE > 8
    case _PORTI:
        PORTI_ISFR |= ulChangedBit;
        break;
    #endif
    }
    #if defined irq_PT_ID
    iPortInterruptSource = irq_PT_ID;
    #endif
    if (fnGenInt(iPortInterruptSource) != 0) {                           // if port interrupt is not disabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        switch (iPort) {
        case _PORTA:
    #if defined irq_PT_ID
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
    #elif defined irq_PORT_A_E_ID
            ptrVect->processor_interrupts.irq_PORTA_E();                 // call port interrupt handler
    #else
            ptrVect->processor_interrupts.irq_PORTA();                   // call port interrupt handler
    #endif
            break;
        case _PORTB:
    #if defined irq_PT_ID
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
    #elif defined irq_PORT_B_C_D_ID                                      // shared port B/C/D interrupt vector
            ptrVect->processor_interrupts.irq_PORTB_C_D();               // call port interrupt handler
    #elif defined irq_PORTBCD_E_ID                                       // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
    #elif defined irq_PORTB_ID
            ptrVect->processor_interrupts.irq_PORTB();                   // call port interrupt handler
    #endif
            break;
    #if PORTS_AVAILABLE > 2
        case _PORTC:
        #if defined irq_PT_ID
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
        #elif defined irq_PORTC_ID
            ptrVect->processor_interrupts.irq_PORTC();                   // call port interrupt handler
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
        #elif defined irq_PORTC_D_ID                                     // shared port C/D interrupt vector
            ptrVect->processor_interrupts.irq_PORTC_D();                 // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 3
        case _PORTD:
        #if defined irq_PT_ID
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
        #elif defined irq_PORTC_ID
            ptrVect->processor_interrupts.irq_PORTD();                   // call port interrupt handler
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
        #elif defined irq_PORTC_D_ID                                     // shared port C/D interrupt vector
            ptrVect->processor_interrupts.irq_PORTC_D();                 // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 4
        case _PORTE:
        #if defined irq_PT_ID
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
        #elif defined irq_PORTE_ID
            ptrVect->processor_interrupts.irq_PORTE();                   // call port interrupt handler
        #elif defined irq_PORT_A_E_ID                                    // shared port A and E interrupt vector
            ptrVect->processor_interrupts.irq_PORTA_E();                 // call port interrupt handler
        #elif defined irq_PORTBCD_E_ID                                   // shared port B/C/D/E interrupt vector
            ptrVect->processor_interrupts.irq_PORTBCD_E();               // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 5
        case _PORTF:
        #if defined irq_PT_ID
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
        #elif defined irq_PORTF_ID
            ptrVect->processor_interrupts.irq_PORTF();                   // call port interrupt handler
        #endif
            break;
    #endif
    #if PORTS_AVAILABLE > 6
        case _PORTG:
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
            break;
    #endif
    #if PORTS_AVAILABLE > 7
        case _PORTH:
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
            break;
    #endif
    #if PORTS_AVAILABLE > 8
        case _PORTI:
            ptrVect->processor_interrupts.irq_PT();                      // call port interrupt handler
            break;
    #endif
        }
    } 
#endif
}

#if defined I2C_INTERFACE

static void fnInterruptI2C(int irq_I2C_ID)
{
    if (fnGenInt(irq_I2C_ID) != 0){                                      // if I2C interrupt is not disabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        switch (irq_I2C_ID) {
    #if defined irq_LPI2C0_ID                                            // low power I2C
        case irq_LPI2C0_ID:
            ptrVect->processor_interrupts.irq_LPI2C0();                  // call the interrupt handler
            break;
    #else
        case irq_I2C0_ID:
            ptrVect->processor_interrupts.irq_I2C0();                    // call the interrupt handler
            break;
    #endif
    #if I2C_AVAILABLE > 1
        #if defined irq_LPI2C0_ID                                        // low power I2C
        case irq_LPI2C1_ID:
            ptrVect->processor_interrupts.irq_LPI2C1();                  // call the interrupt handler
            break;
        #elif defined irq_I2C1_EXTENDED_ID
        case irq_I2C1_EXTENDED_ID:
            ptrVect->processor_interrupts.irq_I2C1();                    // call the interrupt handler
            break;
        #else
        case irq_I2C1_ID:
            ptrVect->processor_interrupts.irq_I2C1();                    // call the interrupt handler
            break;
        #endif
    #endif
    #if I2C_AVAILABLE > 2
        #if !defined irq_I2C2_ID
        case INTMUX0_PERIPHERAL_I2C2:
            fnCallINTMUX(INTMUX_I2C2, INTMUX0_PERIPHERAL_I2C2, (unsigned char *)&ptrVect->processor_interrupts.irq_LPI2C2);
        #else
        case irq_I2C2_ID:
            ptrVect->processor_interrupts.irq_I2C2();                    // call the interrupt handler
            break;
        #endif
    #endif
    #if I2C_AVAILABLE > 3
        case irq_I2C3_ID:
            ptrVect->processor_interrupts.irq_I2C3();                    // call the interrupt handler
            break;
    #endif
        }
    }
}

extern void fnSimulateI2C(int iPort, unsigned char *ptrDebugIn, unsigned short usLen, int iRepeatedStart)
{
#if I2C_AVAILABLE > 0
    KINETIS_I2C_CONTROL *ptrI2C = 0;
    int iI2C_irq = 0;
    switch (iPort) {
    case 0:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C0_BLOCK;
    #if defined irq_LPI2C0_ID
        iI2C_irq = irq_LPI2C0_ID;
    #else
        iI2C_irq = irq_I2C0_ID;
    #endif
        break;
    #if I2C_AVAILABLE > 1
    case 1:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C1_BLOCK;
        #if defined irq_LPI2C1_ID
        iI2C_irq = irq_LPI2C1_ID;
        #elif !defined irq_I2C1_ID
        iI2C_irq = irq_I2C1_EXTENDED_ID;
        #else
        iI2C_irq = irq_I2C1_ID;
        #endif
        break;
    #endif
    #if I2C_AVAILABLE > 2
    case 2:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C2_BLOCK;
        #if defined irq_LPI2C2_EXTENDED_ID
        iI2C_irq = irq_LPI2C2_EXTENDED_ID;
        #else
        iI2C_irq = irq_I2C2_ID;
        #endif
        break;
    #endif
    #if I2C_AVAILABLE > 3
    case 3:
        ptrI2C = (KINETIS_I2C_CONTROL *)I2C3_BLOCK;
        iI2C_irq = irq_I2C3_ID;
        break;
    #endif
    default:
        _EXCEPTION("Injecting to invalid I2C channel!");
        break;
    }
    ptrI2C->I2C_S = (I2C_IBB);                                           // bus is busy
    #if defined I2C_START_CONDITION_INTERRUPT || defined DOUBLE_BUFFERED_I2C
    ptrI2C->I2C_FLT |= (I2C_FLT_FLT_STARTF);                             // start condition detect flag
    if ((ptrI2C->I2C_FLT & I2C_FLT_FLT_SSIE) != 0) {                     // if the start/stop condition interrupt is enabled
        ptrI2C->I2C_S |= I2C_IIF;                                        // interrupt flag set
        fnInterruptI2C(iI2C_irq);                                        // generate the I2C interrupt
    }
    #endif
    if ((*ptrDebugIn & 0xfe) == (ptrI2C->I2C_A1 & 0xfe)) {               // if the address matches the slave address
        ptrI2C->I2C_S = (I2C_IBB | I2C_IAAS | I2C_TCF);                  // bus is busy, addressed as slave and transfer is complete
        ptrI2C->I2C_D = *ptrDebugIn++;                                   // the address is put to the data register
        if ((ptrI2C->I2C_D & 0x01) != 0) {                               // addressed for read
            ptrI2C->I2C_S |= I2C_SRW;
            fnInterruptI2C(iI2C_irq);                                    // generate the I2C interrupt
            usLen = *ptrDebugIn;                                         // the number of bytes to be read
            while (usLen-- > 0) {
                if (usLen == 0) {
                    ptrI2C->I2C_S |= I2C_RXACK;                          // the master doesn't acknowledge the final byte
                }
                else {
                    ptrI2C->I2C_S &= ~I2C_RXACK;
                }
                fnInterruptI2C(iI2C_irq);                                // generate the I2C interrupt
            }
            ptrI2C->I2C_S = I2C_SRW;                                     // slave transmit
        }
        else {
            fnInterruptI2C(iI2C_irq);                                    // generate the I2C interrupt
            while (usLen-- > 1) {
                ptrI2C->I2C_D = *ptrDebugIn++;                           // next byte is put to the data register
                fnInterruptI2C(iI2C_irq);                                // generate the I2C interrupt
            }
        }
    }
    else {
        ptrI2C->I2C_S = (I2C_IBB | I2C_TCF);                             // bus is busy and transfer is complete
    }
    ptrI2C->I2C_S = 0;
    #if defined DOUBLE_BUFFERED_I2C
    ptrI2C->I2C_S = (I2C_TCF);                                           // transfer completed flag is set automatically in double-buffered mode
    #endif
    if (iRepeatedStart == 0) {
        ptrI2C->I2C_FLT |= (I2C_FLT_FLT_STOPF);                          // stop condition detect flag
        if ((ptrI2C->I2C_FLT & I2C_FLT_FLT_INT) != 0) {                  // if the start/stop condition interrupt is enabled
            ptrI2C->I2C_S |= I2C_IIF;                                    // interrupt flag set
            fnInterruptI2C(iI2C_irq);                                    // generate the I2C interrupt
        }
    }
#endif
}
#endif

#if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0                         // device contains both UART and LPUART
#define UART_TYPE_LPUART 0
#define UART_TYPE_UART   1
static const unsigned char uart_type[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {
    #if defined LPUARTS_PARALLEL
    UART_TYPE_UART,                                                      // UART0
    UART_TYPE_UART,                                                      // UART1
    UART_TYPE_UART,                                                      // UART2
    #if ((LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3)
            #if UARTS_AVAILABLE == 3
    UART_TYPE_LPUART,                                                    // LPUART0 (numbered 3)
            #else
    UART_TYPE_UART,                                                      // UART3
            #endif
    #endif
    #if ((LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 4)
            #if UARTS_AVAILABLE == 4
    UART_TYPE_LPUART,                                                    // LPUART0 (numbered 4)
            #else
    UART_TYPE_UART,                                                      // UART4
            #endif
    #endif
    #if ((LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 5)
            #if UARTS_AVAILABLE == 5
    UART_TYPE_LPUART,                                                    // LPUART0 (numbered 5)
            #else
    UART_TYPE_UART,                                                      // UART5
            #endif
    #endif
    #else                                                                // KL43
    UART_TYPE_LPUART,                                                    // LPUART0
    UART_TYPE_LPUART,                                                    // LPUART1
    UART_TYPE_UART,                                                      // UART2
    #endif
};
#endif

#if LPUARTS_AVAILABLE > 0
    #if defined LPUARTS_PARALLEL                                         // see http://www.utasker.com/kinetis/UART_LPUART.html for details of UART/LPUART indexing
        #define LPUART0_CH_NUMBER     UARTS_AVAILABLE                    // LPUARTs are indexed after UARTs
        #define LPUART1_CH_NUMBER     (UARTS_AVAILABLE + 1)
        #define LPUART2_CH_NUMBER     (UARTS_AVAILABLE + 2)
        #define LPUART3_CH_NUMBER     (UARTS_AVAILABLE + 3)
        #define LPUART4_CH_NUMBER     (UARTS_AVAILABLE + 4)
        #define LPUART5_CH_NUMBER     (UARTS_AVAILABLE + 5)

        #if UARTS_AVAILABLE == 0
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART1_RX)
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART2_RX)
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART3_RX)
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART4_RX)
        #elif UARTS_AVAILABLE == 1
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART0_RX)
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART1_RX)
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART2_RX)
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART3_RX)
        #elif UARTS_AVAILABLE == 2
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART0_RX)
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART1_RX)
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART1_RX)
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART2_RX)
        #elif UARTS_AVAILABLE == 3
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART0_RX)
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART1_RX)
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART2_RX)
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART1_RX)
        #elif UARTS_AVAILABLE == 4
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART0_RX)
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART1_RX)
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART2_RX)
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART3_RX)
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
        #elif UARTS_AVAILABLE == 5
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART0_RX)
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART1_RX)
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART2_RX)
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART3_RX)
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART4_RX)
            #define DMUX_UART5_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
        #endif
    #else
        #define LPUART0_CH_NUMBER     0
        #define LPUART1_CH_NUMBER     1
        #define LPUART2_CH_NUMBER     2
        #define LPUART3_CH_NUMBER     3
        #define LPUART4_CH_NUMBER     4
        #define LPUART5_CH_NUMBER     5

        #if defined DMAMUX0_CHCFG_SOURCE_LPUART0_RX
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART0_RX)
        #else
            #define DMUX_UART0_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART0_RX)
        #endif
        #if defined DMAMUX0_CHCFG_SOURCE_LPUART1_RX
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART1_RX)
        #else
            #define DMUX_UART1_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART1_RX)
        #endif
        #if defined DMAMUX0_CHCFG_SOURCE_LPUART2_RX
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART2_RX)
        #else
            #define DMUX_UART2_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART2_RX)
        #endif
        #if defined DMAMUX0_CHCFG_SOURCE_LPUART3_RX
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART3_RX)
        #else
            #define DMUX_UART3_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART3_RX)
        #endif
        #if defined DMAMUX0_CHCFG_SOURCE_LPUART4_RX
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART4_RX)
        #else
            #define DMUX_UART4_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART4_RX)
        #endif
        #if defined DMAMUX0_CHCFG_SOURCE_LPUART5_RX
            #define DMUX_UART5_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_LPUART5_RX)
        #else
            #define DMUX_UART5_RX_CHANNEL (DMAMUX0_CHCFG_SOURCE_UART5_RX)
        #endif
    #endif

    #if defined SERIAL_INTERFACE && !defined DEVICE_WITHOUT_DMA
static const unsigned char ucUART_channel[] = {
    DMA_UART0_RX_CHANNEL,
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
    DMA_UART1_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
    DMA_UART2_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
    DMA_UART3_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 4
    DMA_UART4_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 5
    DMA_UART5_RX_CHANNEL
        #endif
};

static const unsigned char ucUART_DMUX_channel[] = {
    DMUX_UART0_RX_CHANNEL,
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
    DMUX_UART1_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
    DMUX_UART2_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
    DMUX_UART3_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 4
    DMUX_UART4_RX_CHANNEL,
        #endif
        #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 5
    DMUX_UART5_RX_CHANNEL
        #endif
};
    #endif
#endif

extern void fnSimulateSPIIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
#if defined SPI_INTERFACE
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    switch (iPort) {
    case 0:
        while (usLen-- != 0) {                                           // for each byte
        #if defined DSPI_SPI
            SPI0_POPR = *ptrDebugIn++;
            SPI0_SR |= SPI_SR_RFDF;
            if ((SPI0_RSER & SPI_SRER_RFDF_RE) != 0) {
                ptrVect->processor_interrupts.irq_SPI0();                // call the interrupt handler
            }
        #else
            _EXCEPTION("To do...");
        #endif
        }
        break;
    #if SPI_AVAILABLE > 1
    case 1:
        while (usLen-- != 0) {                                           // for each byte
        #if defined DSPI_SPI
            SPI1_POPR = *ptrDebugIn++;
            SPI1_SR |= SPI_SR_RFDF;
            if ((SPI1_RSER & SPI_SRER_RFDF_RE) != 0) {
                ptrVect->processor_interrupts.irq_SPI1();                // call the interrupt handler
            }
        #else
            _EXCEPTION("To do...");
        #endif
        }
        break;
    #endif
    #if SPI_AVAILABLE > 2
    case 2:
        while (usLen-- != 0) {                                           // for each byte
        #if defined DSPI_SPI
            SPI2_POPR = *ptrDebugIn++;
            SPI2_SR |= SPI_SR_RFDF;
            if ((SPI2_RSER & SPI_SRER_RFDF_RE) != 0) {
                ptrVect->processor_interrupts.irq_SPI2();                // call the interrupt handler
            }
        #else
            _EXCEPTION("To do...");
        #endif
        }
        break;
    #endif
    }
#endif
}

// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
#if defined SERIAL_INTERFACE
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    #if defined LOG_UART_RX
    if (ptrDebugIn != 0) {
        fnLogRx(iPort, ptrDebugIn, usLen);                               // {43}
    }
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (iPort >= NUMBER_SERIAL) {
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
        return;
    }
    #endif
    #if defined SERIAL_INTERFACE
    if (usLen != 0) {
        iUART_rx_Active[iPort] = 1;                                      // mark that the reception line is not idle
    }
    #endif
    #if LPUARTS_AVAILABLE > 0                                            // parts with LPUART
        #if UARTS_AVAILABLE > 0                                          // parts with LPUART and UART
    if (uart_type[iPort] == UART_TYPE_LPUART) {
        #endif
        switch (iPort) {
        #if defined LPUARTS_PARALLEL
        case UARTS_AVAILABLE:
        #else
        case 0:                                                          // LPUART0
        #endif
            if ((LPUART0_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                if (ptrDebugIn == 0) {                                   // idle line detection
                    LPUART0_STAT |= LPUART_STAT_IDLE;                    // mark idle line status
                    if ((LPUART0_CTRL & LPUART_CTRL_ILIE) != 0) {        // if the idle line interrupt is enabled
        #if defined irq_LPUART0_RX_ID
                        if (fnGenInt(irq_LPUART0_RX_ID) != 0) {         // if LPUART0 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_LPUART0_RX(); // call the interrupt handler
                        }
        #else
                        if (fnGenInt(irq_LPUART0_ID) != 0) {             // if LPUART0 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_LPUART0(); // call the interrupt handler
                        }
        #endif
                    }
                    return;
                }
                while (usLen-- != 0) {                                   // for each reception character
                    LPUART0_DATA = *ptrDebugIn++;
                    LPUART0_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART0_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
        #if !defined DEVICE_WITHOUT_DMA                                  // if the device supports DMA
                        if ((LPUART0_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the LPUART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA
                #if defined KINETIS_KL && !defined DEVICE_WITH_eDMA
                            KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                            ptrDMA += ucUART_channel[LPUART0_CH_NUMBER];
                            if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) { // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART0_CH_NUMBER], ucUART_DMUX_channel[LPUART0_CH_NUMBER]); // trigger DMA transfer on the UART's channel
                                LPUART0_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #else
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << ucUART_channel[LPUART0_CH_NUMBER])) != 0) { // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART0_CH_NUMBER], ucUART_DMUX_channel[LPUART0_CH_NUMBER]); // trigger DMA transfer on the UART's channel
                                LPUART0_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #endif
            #endif
                        }
                        else {
        #endif
        #if defined irq_LPUART0_RX_ID
                            if (fnGenInt(irq_LPUART0_RX_ID) != 0) {      // if LPUART0 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART0_RX(); // call the interrupt handler
                            }
        #else
                            if (fnGenInt(irq_LPUART0_ID) != 0) {         // if LPUART0 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART0(); // call the interrupt handler
                            }
        #endif
        #if !defined DEVICE_WITHOUT_DMA                                  // if the device supports DMA
                        }
        #endif
                    }
                }
            }
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case 1:                                                          // LPUART1
            #endif
            if ((LPUART1_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                if (ptrDebugIn == 0) {                                   // idle line detection
                    LPUART1_STAT |= LPUART_STAT_IDLE;                    // mark idle line status
                    if ((LPUART1_CTRL & LPUART_CTRL_ILIE) != 0) {        // if the idle line interrupt is enabled
            #if defined irq_LPUART1_RX_ID
                        if (fnGenInt(irq_LPUART1_RX_ID) != 0) {         // if LPUART1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_LPUART1_RX(); // call the interrupt handler
                        }
            #else
                        if (fnGenInt(irq_LPUART1_ID) != 0) {             // if LPUART1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_LPUART1(); // call the interrupt handler
                        }
            #endif
                    }
                    return;
                }
                while ((usLen--) != 0) {                                 // for each reception character
                    LPUART1_DATA = *ptrDebugIn++;
                    LPUART1_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART1_CTRL & LPUART_CTRL_RIE) != 0) {
                                                                         // if reception interrupt is enabled
            #if !defined DEVICE_WITHOUT_DMA                              // if the device supports DMA
                        if ((LPUART1_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the UART is operating in DMA reception mode
                #if defined SERIAL_SUPPORT_DMA
                    #if defined KINETIS_KL && !defined DEVICE_WITH_eDMA
                            KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                            ptrDMA += ucUART_channel[LPUART1_CH_NUMBER];
                            if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {  // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART1_CH_NUMBER], ucUART_DMUX_channel[LPUART1_CH_NUMBER]); // trigger DMA transfer on the UART's channel
                                LPUART1_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                    #else
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << ucUART_channel[LPUART1_CH_NUMBER])) != 0) { // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART1_CH_NUMBER], ucUART_DMUX_channel[LPUART1_CH_NUMBER]); // trigger DMA transfer on the UART's channel
                                LPUART1_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                    #endif
                #endif
                        }
                        else {
            #endif
            #if defined irq_LPUART1_RX_ID
                            if (fnGenInt(irq_LPUART1_RX_ID) != 0) {      // if LPUART0 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART1_RX(); // call the interrupt handler
                            }
            #else
                            if (fnGenInt(irq_LPUART1_ID) != 0) {         // if LPUART1 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART1(); // call the interrupt handler
                            }
            #endif
            #if !defined DEVICE_WITHOUT_DMA                              // if the device supports DMA
                        }
            #endif
                    }
                }
            }
            break;
        #endif
        #if LPUARTS_AVAILABLE > 2
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 2):
            #else
        case 2:                                                          // LPUART2
            #endif
            if ((LPUART2_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                if (ptrDebugIn == 0) {                                   // idle line detection
                    LPUART2_STAT |= LPUART_STAT_IDLE;                    // mark idle line status
                    if ((LPUART2_CTRL & LPUART_CTRL_ILIE) != 0) {        // if the idle line interrupt is enabled
            #if !defined irq_LPUART2_ID && defined INTMUX0_AVAILABLE
                        if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_LPUART2) != 0) // {46}
            #elif defined irq_LPUART2_RX_ID
                        if (fnGenInt(irq_LPUART2_RX_ID) != 0)            // if LPUART2 interrupt is not disabled
            #else
                        if (fnGenInt(irq_LPUART2_ID) != 0)
            #endif
                        {                                                // if LPUART2 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            #if defined irq_LPUART2_RX_ID
                            ptrVect->processor_interrupts.irq_LPUART2_RX(); // call the interrupt handler
            #elif !defined irq_LPUART2_ID
                            fnCallINTMUX(INTMUX_LPUART2, INTMUX0_PERIPHERAL_LPUART2, (unsigned char *)&ptrVect->processor_interrupts.irq_LPUART2);
            #else
                            ptrVect->processor_interrupts.irq_LPUART2(); // call the interrupt handler
            #endif
                        }
                    }
                    return;
                }
                while (usLen-- != 0) {                                   // for each reception character
                    LPUART2_DATA = *ptrDebugIn++;
                    LPUART2_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART2_CTRL & LPUART_CTRL_RIE) != 0) {         // if reception interrupt is enabled
                        if ((LPUART2_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the LPUART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA
                #if defined KINETIS_KL && !defined DEVICE_WITH_eDMA
                            KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                            ptrDMA += cUART_channel[LPUART2_CH_NUMBER];
                            if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {  // if source enabled
                                fnSimulateDMA(cUART_channel[LPUART2_CH_NUMBER], ucUART_DMUX_channel[LPUART2_CH_NUMBER]); // trigger DMA transfer on the LPUART's channel
                                LPUART2_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #else
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << ucUART_channel[LPUART2_CH_NUMBER])) != 0) { // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART2_CH_NUMBER], ucUART_DMUX_channel[LPUART2_CH_NUMBER]); // trigger DMA transfer on the LPUART's channel
                                LPUART2_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #endif
            #endif
                        }
                        else {
            #if !defined irq_LPUART2_ID && defined INTMUX0_AVAILABLE
                            if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_LPUART2) != 0) // {46}
            #elif defined irq_LPUART2_RX_ID
                            if (fnGenInt(irq_LPUART2_RX_ID) != 0)        // if LPUART2 interrupt is not disabled
            #else
                            if (fnGenInt(irq_LPUART2_ID) != 0)
            #endif
                            {                                            // if LPUART2 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            #if defined irq_LPUART2_RX_ID
                                ptrVect->processor_interrupts.irq_LPUART2_RX(); // call the interrupt handler
            #elif !defined irq_LPUART2_ID
                                fnCallINTMUX(INTMUX_LPUART2, INTMUX0_PERIPHERAL_LPUART2, (unsigned char *)&ptrVect->processor_interrupts.irq_LPUART2);
            #else
                                ptrVect->processor_interrupts.irq_LPUART2(); // call the interrupt handler
            #endif
                            }
                        }
                    }
                }
            }
            break;
        #endif
        #if LPUARTS_AVAILABLE > 3
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 3):
            #else
        case 3:
            #endif
            if ((LPUART3_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while (usLen-- != 0) {                                   // for each reception character
                    LPUART3_DATA = *ptrDebugIn++;
                    LPUART3_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART3_CTRL & LPUART_CTRL_RIE) != 0) {         // if reception interrupt is enabled
                        if ((LPUART3_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the LPUART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_UART3_RX_CHANNEL
                #if defined KINETIS_KL && !defined DEVICE_WITH_eDMA
                            KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                            ptrDMA += cUART_channel[LPUART3_CH_NUMBER];
                            if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {  // if source enabled
                                fnSimulateDMA(cUART_channel[LPUART3_CH_NUMBER], ucUART_DMUX_channel[LPUART3_CH_NUMBER]); // trigger DMA transfer on the LPUART's channel
                                LPUART3_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #else
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << ucUART_channel[LPUART3_CH_NUMBER])) != 0) { // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART3_CH_NUMBER], ucUART_DMUX_channel[LPUART3_CH_NUMBER]); // trigger DMA transfer on the LPUART's channel
                                LPUART3_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #endif
            #endif
                        }
                        else {
                            if (fnGenInt(irq_LPUART3_ID) != 0) {             // if LPUART3 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART3(); // call the interrupt handler
                            }
                        }
                    }
                }
            }
            break;
        #endif
        #if LPUARTS_AVAILABLE > 4
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 4):
            #else
        case 4:
            #endif
            if ((LPUART4_CTRL & LPUART_CTRL_RE) != 0) {                  // if receiver enabled
                while (usLen-- != 0) {                                   // for each reception character
                    LPUART4_DATA = *ptrDebugIn++;                        // save the received byte into the reception data register
                    LPUART4_STAT |= LPUART_STAT_RDRF;                    // set interrupt cause
                    if ((LPUART4_CTRL & LPUART_CTRL_RIE) != 0) {         // if reception interrupt is enabled
                        if ((LPUART4_BAUD & LPUART_BAUD_RDMAE) != 0) {   // if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_UART4_RX_CHANNEL
                #if defined KINETIS_KL && !defined DEVICE_WITH_eDMA
                            KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                            ptrDMA += cUART_channel[LPUART4_CH_NUMBER];
                            if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {  // if source enabled
                                fnSimulateDMA(cUART_channel[LPUART4_CH_NUMBER], ucUART_DMUX_channel[LPUART4_CH_NUMBER]); // trigger DMA transfer on the LPUART's channel
                                LPUART4_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #else
                            if ((DMA_ERQ & (DMA_ERQ_ERQ0 << ucUART_channel[LPUART4_CH_NUMBER])) != 0) { // if source enabled
                                fnSimulateDMA(ucUART_channel[LPUART4_CH_NUMBER], ucUART_DMUX_channel[LPUART4_CH_NUMBER]); // trigger DMA transfer on the LPUART's channel
                                LPUART4_STAT &= ~LPUART_STAT_RDRF;       // remove interrupt cause
                            }
                #endif
            #endif
                        }
                        else {
                            if (fnGenInt(irq_LPUART4_ID) != 0) {             // if LPUART4 interrupt is not disabled
                                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                                ptrVect->processor_interrupts.irq_LPUART4(); // call the interrupt handler
                            }
                        }
                    }
                }
            }
            break;
        #endif
        }
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif

    #if UARTS_AVAILABLE > 0                                              // UARTs
    switch (iPort) {
        #if LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL
    case 0:                                                              // UART0
        if ((UART0_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen-- != 0) {                                       // for each reception character
                UART0_D = *ptrDebugIn++;
                if ((UART0_S2 & UART_S2_LBKDE) == 0) {                   // don't set reception flag if break detection is enabled
                    UART0_S1 |= UART_S1_RDRF;                            // set interrupt cause
                }
                if (((UART0_S1 & UART_S1_RDRF) != 0) && ((UART0_C2 & UART_C2_RIE) != 0)) { // if reception interrupt (or DMA) is enabled
            #if !defined DEVICE_WITHOUT_DMA                              // if the device supports DMA
                    if ((UART0_C5 & UART_C5_RDMAS) != 0) {               // {4} if the UART is operating in DMA reception mode
                #if defined SERIAL_SUPPORT_DMA && defined DMA_UART0_RX_CHANNEL
                    #if ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
                        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                        ptrDMA += DMA_UART0_RX_CHANNEL;
                        if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {      // if source enabled
                            fnSimulateDMA(DMA_UART0_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART0_RX); // trigger DMA transfer on the UART's channel
                            UART0_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
                    #else
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART0_RX_CHANNEL)) != 0) { // if source enabled
                            fnSimulateDMA(DMA_UART0_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART0_RX); // trigger DMA transfer on the UART's channel
                            UART0_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
                    #endif
                #endif
                    }
                    else {
            #endif
            #if defined irq_UART0_1_ID                                   // when UARTs 0 and 1 share an interrupt
                        if (fnGenInt(irq_UART0_1_ID) != 0) {             // if UART0/1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART0_1(); // call the interrupt handler
                        }
            #else
                        if (fnGenInt(irq_UART0_ID) != 0) {               // if UART0 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART0();   // call the interrupt handler
                        }
           #endif
            #if !defined DEVICE_WITHOUT_DMA                              // if the device supports DMA
                    }
            #endif
                }
            }
        }
        break;
        #endif
    #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
    case 1:
        if ((UART1_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen-- != 0) {                                       // for each reception character
                UART1_D = *ptrDebugIn++;                                 // save the received byte to the UART data register
                if ((UART1_S2 & UART_S2_LBKDE) == 0) {                   // don't set reception flag if break detection is enabled
                    UART1_S1 |= UART_S1_RDRF;                            // set interrupt cause
                }
                if (((UART1_S1 & UART_S1_RDRF) != 0) && ((UART1_C2 & UART_C2_RIE) != 0)) { // if reception interrupt (or dma) is enabled
        #if !defined DEVICE_WITHOUT_DMA                                  // if the device supports DMA
            #if defined KINETIS_KL
                    if ((UART1_C4 & UART_C4_RDMAS) != 0)                 // DMA mode
            #else
                    if ((UART1_C5 & UART_C5_RDMAS) != 0)                 // DMA mode
            #endif
                    {                                                    // {4} if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_UART1_RX_CHANNEL
                #if ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
                        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                        ptrDMA += DMA_UART1_RX_CHANNEL;
                        if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {      // if source enabled
                            fnSimulateDMA(DMA_UART1_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART1_RX); // trigger DMA transfer on the UART's channel
                            UART1_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
                #else
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART1_RX_CHANNEL)) != 0) { // if source enabled
                            fnSimulateDMA(DMA_UART1_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART1_RX); // trigger DMA transfer on the UART's channel
                            UART1_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
                #endif
            #endif
                    }
                    else {
        #endif
        #if defined irq_UART0_1_ID                                       // when UARTs 0 and 1 share an interrupt
                        if (fnGenInt(irq_UART0_1_ID) != 0) {             // if UART0/1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART0_1(); // call the interrupt handler
                        }
        #else
                        if (fnGenInt(irq_UART1_ID) != 0) {               // if UART1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART1();   // call the interrupt handler
                        }
        #endif
        #if !defined DEVICE_WITHOUT_DMA                                  // if the device supports DMA
                    }
        #endif
                }
            }
        }
        break;
    #endif
    #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || ((UARTS_AVAILABLE == 1) && (LPUARTS_AVAILABLE == 2))
    case 2:
        if ((UART2_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            if (ptrDebugIn == 0) {                                       // idle line detection
                UART2_S1 |= UART_S1_IDLE;                                // mark idle line status
                if ((UART2_C2 & UART_C2_ILIE) != 0) {                    // if the idle line interrupt is enabled
        #if defined irq_UART2_3_ID                                       // when UARTs 2 and 3 share an interrupt
                    if (fnGenInt(irq_UART2_3_ID) != 0) {                 // if UART2/3 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_UART2_3(); // call the interrupt handler
                    }
        #else
                    if (fnGenInt(irq_UART2_ID) != 0) {                   // if UART2 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_UART2();       // call the interrupt handler
                    }
        #endif
                }
                return;
            }
            UART2_S1 &= ~(UART_S1_IDLE);
            while (usLen-- != 0) {                                       // for each reception character
                UART2_D = *ptrDebugIn++;
                if ((UART2_S2 & UART_S2_LBKDE) == 0) {                   // don't set reception flag if break detection is enabled
                    UART2_S1 |= UART_S1_RDRF;                            // set interrupt cause
                }
                if (((UART2_S1 & UART_S1_RDRF) != 0) && ((UART2_C2 & UART_C2_RIE) != 0)) { // if reception interrupt (or dma) is enabled
        #if !defined KINETIS_KE
            #if defined KINETIS_KL &&  (UARTS_AVAILABLE > 1)
                    if ((UART2_C4 & UART_C4_RDMAS) != 0)                 // if DMA mode is enabled
            #else
                    if ((UART2_C5 & UART_C5_RDMAS) != 0)                 // if DMA mode is enabled
            #endif
                    {                                                    // {4} if the UART is operating in DMA reception mode
            #if defined SERIAL_SUPPORT_DMA && defined DMA_UART2_RX_CHANNEL
                #if ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
                        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                        ptrDMA += DMA_UART2_RX_CHANNEL;
                        if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {      // if source enabled
                            fnSimulateDMA(DMA_UART2_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART2_RX); // trigger DMA transfer on the UART's channel
                            UART2_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
                #else
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART2_RX_CHANNEL)) != 0) { // if source enabled
                            fnSimulateDMA(DMA_UART2_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART2_RX); // trigger DMA transfer on the UART's channel
                            UART2_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
               #endif
            #endif
                    }
                    else {
        #endif
        #if defined irq_UART2_3_ID                                       // when UARTs 2 and 3 share an interrupt
                        if (fnGenInt(irq_UART2_3_ID) != 0) {             // if UART2/3 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART2_3();   // call the interrupt handler
                        }
        #else
                        if (fnGenInt(irq_UART2_ID) != 0) {               // if UART2 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART2();   // call the interrupt handler
                        }
        #endif
        #if !defined KINETIS_KE
                    }
        #endif
                }
            }
        }
        break;
    #endif
    #if UARTS_AVAILABLE > 3
    case 3:
        if ((UART3_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen-- != 0) {                                       // for each reception character
                UART3_D = *ptrDebugIn++;
                if ((UART3_S2 & UART_S2_LBKDE) == 0) {                   // don't set reception flag if break detection is enabled
                    UART3_S1 |= UART_S1_RDRF;                            // set interrupt cause
                }
                if (((UART3_S1 & UART_S1_RDRF) != 0) && ((UART3_C2 & UART_C2_RIE) != 0)) { // if reception interrupt (or dma) is enabled
                    if ((UART3_C5 & UART_C5_RDMAS) != 0) {               // {4} if the UART is operating in DMA reception mode
        #if defined SERIAL_SUPPORT_DMA && defined DMA_UART3_RX_CHANNEL
            #if ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
                        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
                        ptrDMA += DMA_UART3_RX_CHANNEL;
                        if ((ptrDMA->DMA_DCR & DMA_DCR_ERQ) != 0) {      // if source enabled
                            fnSimulateDMA(DMA_UART3_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART3_RX); // trigger DMA transfer on the UART's channel
                            UART3_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
            #else
                        if (DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART3_RX_CHANNEL)) { // if source enabled
                            fnSimulateDMA(DMA_UART3_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART3_RX); // trigger DMA transfer on the UART's channel
                            UART3_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
            #endif
        #endif
                    }
                    else {
        #if defined irq_UART2_3_ID                                       // when UARTs 2 and 3 share an interrupt
                        if (fnGenInt(irq_UART2_3_ID) != 0) {             // if UART2/3 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART2_3();   // call the interrupt handler
                        }
        #else
                        if (fnGenInt(irq_UART3_ID) != 0) {               // if UART3 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART3();   // call the interrupt handler
                        }
        #endif
                    }
                }
            }
        }
        break;
    #endif
    #if UARTS_AVAILABLE > 4
    case 4:
        if ((UART4_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen-- != 0) {                                       // for each reception character
                UART4_D = *ptrDebugIn++;
                if ((UART4_S2 & UART_S2_LBKDE) == 0) {                   // don't set reception flag if break detection is enabled
                    UART4_S1 |= UART_S1_RDRF;                            // set interrupt cause
                }
                if (((UART4_S1 & UART_S1_RDRF) != 0) && ((UART4_C2 & UART_C2_RIE) != 0)) { // if reception interrupt (or dma) is enabled
                    if ((UART4_C5 & UART_C5_RDMAS) != 0) {               // {4} if the UART is operating in DMA reception mode
        #if defined SERIAL_SUPPORT_DMA && defined DMA_UART4_RX_CHANNEL
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART4_RX_CHANNEL)) != 0) { // if source enabled
                            fnSimulateDMA(DMA_UART4_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART4_RX); // trigger DMA transfer on the UART's channel
                            UART4_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
        #endif
                    }
                    else {
                        if (fnGenInt(irq_UART4_ID) != 0) {               // if UART4 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART4();   // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    #if UARTS_AVAILABLE > 5
    case 5:
        if ((UART5_C2 & UART_C2_RE) != 0) {                              // if receiver enabled
            while (usLen-- != 0) {                                       // for each reception character
                UART5_D = *ptrDebugIn++;
                if ((UART5_S2 & UART_S2_LBKDE) == 0) {                   // don't set reception flag if break detection is enabled
                    UART5_S1 |= UART_S1_RDRF;                            // set interrupt cause
                }
                if (((UART5_S1 & UART_S1_RDRF) != 0) && ((UART5_C2 & UART_C2_RIE) != 0)) { // if reception interrupt (or dma) is enabled
                    if ((UART5_C5 & UART_C5_RDMAS) != 0) {               // {4} if the UART is operating in DMA reception mode
        #if defined SERIAL_SUPPORT_DMA && defined DMA_UART5_RX_CHANNEL
                        if ((DMA_ERQ & (DMA_ERQ_ERQ0 << DMA_UART5_RX_CHANNEL)) != 0) { // if source enabled
                            fnSimulateDMA(DMA_UART5_RX_CHANNEL, DMAMUX0_CHCFG_SOURCE_UART5_RX); // trigger DMA transfer on the UART's channel
                            UART5_S1 &= ~UART_S1_RDRF;                   // remove interrupt cause
                        }
        #endif
                    }
                    else {
                        if (fnGenInt(irq_UART5_ID) != 0) {               // if UART5 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_UART5();   // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    }
    #endif
#endif
}


// Handle UART transmission interrupts
//
static void fnUART_Tx_int(int iChannel)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[iChannel] == UART_TYPE_LPUART) {
        #endif
        switch (iChannel) {
        #if defined LPUARTS_PARALLEL
        case UARTS_AVAILABLE:
        #else
        case 0:                                                          // LPUART0
        #endif
            if ((LPUART0_CTRL & LPUART_CTRL_TE) != 0) {                  // if transmitter enabled
                LPUART0_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);     // set interrupt cause
                if ((LPUART0_CTRL & LPUART0_STAT) != 0) {                // if transmit interrupt type enabled
        #if defined irq_LPUART0_TX_ID
                    if (fnGenInt(irq_LPUART0_TX_ID) != 0) {              // if LPUART0 tx interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART0_TX();  // call the interrupt handler
                    }
        #else
                    if (fnGenInt(irq_LPUART0_ID) != 0) {                 // if LPUART0 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART0();     // call the interrupt handler
                    }
        #endif
                }
            }
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case 1:
            #endif
            if ((LPUART1_CTRL & LPUART_CTRL_TE) != 0) {                  // if transmitter enabled
                LPUART1_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);     // set interrupt cause
                if ((LPUART1_CTRL & LPUART1_STAT) != 0) {                // if transmit interrupt type enabled
            #if defined irq_LPUART1_TX_ID
                    if (fnGenInt(irq_LPUART1_TX_ID) != 0) {              // if LPUART1 tx interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART1_TX();  // call the interrupt handler
                    }
            #else
                    if (fnGenInt(irq_LPUART1_ID) != 0) {                 // if LPUART1 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART1();     // call the interrupt handler
                    }
            #endif
                }
            }
            break;
        #endif
        #if (LPUARTS_AVAILABLE > 2) && (UARTS_AVAILABLE == 0)
        case 2:
            if ((LPUART2_CTRL & LPUART_CTRL_TE) != 0) {                  // if transmitter enabled
                LPUART2_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);     // set interrupt cause
                if ((LPUART2_CTRL & LPUART2_STAT) != 0) {                // if transmit interrupt type enabled
            #if !defined irq_LPUART2_ID && defined INTMUX0_AVAILABLE
                    if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_LPUART2) != 0)// {46}
            #elif defined irq_LPUART2_TX_ID
                    if (fnGenInt(irq_LPUART2_TX_ID) != 0)                // if LPUART2 tx interrupt is not disabled
            #else
                    if (fnGenInt(irq_LPUART2_ID) != 0)
            #endif
                    {                                                    // if LPUART2 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            #if defined irq_LPUART2_TX_ID
                        ptrVect->processor_interrupts.irq_LPUART2_TX();  // call the interrupt handler
            #elif !defined irq_LPUART2_ID
                        fnCallINTMUX(INTMUX_LPUART2, INTMUX0_PERIPHERAL_LPUART2, (unsigned char *)&ptrVect->processor_interrupts.irq_LPUART2);
            #else
                        ptrVect->processor_interrupts.irq_LPUART2();     // call the interrupt handler
            #endif
                    }
                }
            }
            break;
        #endif
        #if (LPUARTS_AVAILABLE > 3) && (UARTS_AVAILABLE == 0)
        case 3:
            if ((LPUART3_CTRL & LPUART_CTRL_TE) != 0) {                  // if transmitter enabled
                LPUART3_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);     // set interrupt cause
                if ((LPUART3_CTRL & LPUART3_STAT) != 0) {                // if transmit interrupt type enabled
                    if (fnGenInt(irq_LPUART3_ID) != 0) {                 // if LPUART3 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART3();     // call the interrupt handler
                    }
                }
            }
            break;
        #endif
        #if (LPUARTS_AVAILABLE > 4) && (UARTS_AVAILABLE == 0)
        case 4:
            if ((LPUART4_CTRL & LPUART_CTRL_TE) != 0) {                  // if transmitter enabled
                LPUART4_STAT |= (LPUART_STAT_TDRE | LPUART_STAT_TC);     // set interrupt cause
                if ((LPUART4_CTRL & LPUART4_STAT) != 0) {                // if transmit interrupt type enabled
                    if (fnGenInt(irq_LPUART4_ID) != 0) {                 // if LPUART3 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPUART4();     // call the interrupt handler
                    }
                }
            }
            break;
        #endif
        }
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    switch (iChannel) {
        #if LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL
    case 0:
        if ((UART0_C2 & UART_C2_TE) != 0) {                              // if transmitter enabled
            UART0_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if ((UART0_C2 & UART0_S1) != 0) {                            // if transmit interrupt type enabled
            #if defined irq_UART0_1_ID                                   // when UARTs 0 and 1 share an interrupt
                if (fnGenInt(irq_UART0_1_ID) != 0) {                     // if UART0/1 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART0_1();         // call the interrupt handler
                }
            #else
                if (fnGenInt(irq_UART0_ID) != 0) {
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART0();           // call the interrupt handler
                }
            #endif
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
    case 1:
        if (UART1_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART1_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if ((UART1_C2 & UART1_S1) != 0) {                            // if transmit interrupt type enabled
            #if defined irq_UART0_1_ID                                   // when UARTs 0 and 1 share an interrupt
                if (fnGenInt(irq_UART0_1_ID) != 0) {                     // if UART0/1 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART0_1();         // call the interrupt handler
                }
            #else
                if (fnGenInt(irq_UART1_ID) != 0) {
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART1();           // call the interrupt handler
                }
            #endif
            }
        }
        break;
        #endif
        #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (LPUARTS_AVAILABLE == 2 && UARTS_AVAILABLE == 1)
    case 2:
        if ((UART2_C2 & UART_C2_TE) != 0) {                              // if transmitter enabled
            UART2_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if ((UART2_C2 & UART2_S1) != 0) {                            // if transmit interrupt type enabled
            #if defined irq_UART2_3_ID                                   // when UARTs 2 and 3 share an interrupt
                if (fnGenInt(irq_UART2_3_ID) != 0) {                     // if UART2/3 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART2_3();         // call the interrupt handler
                }
            #else
                if (fnGenInt(irq_UART2_ID) != 0) {                       // if UART2 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART2();           // call the interrupt handler
                }
            #endif
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 3
    case 3:
        if (UART3_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART3_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART3_C2 & UART3_S1) {                                   // if transmit interrupt type enabled
            #if defined irq_UART2_3_ID                                   // when UARTs 2 and 3 share an interrupt
                if (fnGenInt(irq_UART2_3_ID) != 0) {                     // if UART2/3 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART2_3();         // call the interrupt handler
                }
            #else
                if (fnGenInt(irq_UART3_ID) != 0) {                       // if UART3 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART3();           // call the interrupt handler
                }
            #endif
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 4
    case 4:
        if (UART4_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART4_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART4_C2 & UART4_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART4_ID) != 0) {                       // if UART4 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART4();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
        #if UARTS_AVAILABLE > 5
    case 5:
        if (UART5_C2 & UART_C2_TE) {                                     // if transmitter enabled
            UART5_S1 |= (UART_S1_TDRE | UART_S1_TC);                     // set interrupt cause
            if (UART5_C2 & UART5_S1) {                                   // if transmit interrupt type enabled
                if (fnGenInt(irq_UART5_ID) != 0) {                       // if UART5 interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_UART5();           // call the interrupt handler
                }
            }
        }
        break;
        #endif
    }
    #endif
}



// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{
#if defined SERIAL_INTERFACE
    extern unsigned char ucTxLast[NUMBER_SERIAL];
#endif
    unsigned long ulNewActions = 0;
    int *ptrCnt;

    if (((iInts & CHANNEL_0_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART0];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx0(ucTxLast[0]);
                ulNewActions |= SEND_COM_0;
                fnUART_Tx_int(0);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_1_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART1];
        if (*ptrCnt != 0) {
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

    if (((iInts & CHANNEL_2_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART2];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx2(ucTxLast[2]);
                ulNewActions |= SEND_COM_2;
                fnUART_Tx_int(2);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_3_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART3];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_3_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_3_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx3(ucTxLast[3]);
                ulNewActions |= SEND_COM_3;
                fnUART_Tx_int(3);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_4_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART4];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_4_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_4_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx4(ucTxLast[4]);
                ulNewActions |= SEND_COM_4;
                fnUART_Tx_int(4);
#endif
            }
        }
	}

    if (((iInts & CHANNEL_5_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_UART5];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_5_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_5_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx5(ucTxLast[5]);
                ulNewActions |= SEND_COM_5;
                fnUART_Tx_int(5);
#endif
            }
        }
	}

#if NUMBER_EXTERNAL_SERIAL > 0
	if (((iInts & CHANNEL_0_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART0];
        if (*ptrCnt != 0) {
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
	if (((iInts & CHANNEL_1_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART1];
        if (*ptrCnt != 0) {
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
	if (((iInts & CHANNEL_2_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART2];
        if (*ptrCnt != 0) {
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
	if (((iInts & CHANNEL_3_EXT_SERIAL_INT) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_EXT_UART3];
        if (*ptrCnt != 0) {
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

#if defined I2C_INTERFACE
    if (((iInts & I2C_INT0) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT0;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT0;                                      // interrupt has been handled
		        iInts &= ~I2C_INT0;
    #if I2C_AVAILABLE > 0
                if ((I2C0_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
                    if (fnGenInt(irq_I2C0_ID) != 0) {                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_I2C0();        // call the interrupt handler
                    }
                }
    #elif LPI2C_AVAILABLE > 0
                if (((LPI2C0_MIER & LPI2C0_MSR) & (LPI2C_MIER_TDIE | LPI2C_MIER_RDIE | LPI2C_MIER_EPIE | LPI2C_MIER_SDIE | LPI2C_MIER_NDIE | LPI2C_MIER_ALIE | LPI2C_MIER_FEIE | LPI2C_MIER_PLTIE | LPI2C_MIER_DMIE)) != 0) { // if an enabled interrupt source is active
                    if (fnGenInt(irq_LPI2C0_ID) != 0) {
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPI2C0();      // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif

#if NUMBER_I2C > 1
    if (((iInts & I2C_INT1) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C1];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT1;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT1;                                      // interrupt has been handled
    #if I2C_AVAILABLE > 1
                if ((I2C1_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
        #if !defined irq_I2C1_ID && defined INTMUX0_AVAILABLE
                    if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_I2C1) != 0)
        #else
                    if (fnGenInt(irq_I2C1_ID) != 0)                      // if I2C interrupt is not disabled
        #endif
                    {
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        #if !defined irq_I2C1_ID && defined INTMUX0_AVAILABLE
                        fnCallINTMUX(INTMUX_I2C1, INTMUX0_PERIPHERAL_I2C1, (unsigned char *)&ptrVect->processor_interrupts.irq_I2C1);
        #else
                        ptrVect->processor_interrupts.irq_I2C1();       // call the interrupt handler
        #endif
                    }
                }
    #elif LPI2C_AVAILABLE > 1
                if (((LPI2C1_MIER & LPI2C1_MSR) & (LPI2C_MIER_TDIE | LPI2C_MIER_RDIE | LPI2C_MIER_EPIE | LPI2C_MIER_SDIE | LPI2C_MIER_NDIE | LPI2C_MIER_ALIE | LPI2C_MIER_FEIE | LPI2C_MIER_PLTIE | LPI2C_MIER_DMIE)) != 0) { // if an enabled interrupt source is active
                    if (fnGenInt(irq_LPI2C1_ID) != 0) {
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPI2C1();      // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif
#if NUMBER_I2C > 2
    if (((iInts & I2C_INT2) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C2];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT2;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT2;                                      // interrupt has been handled
    #if I2C_AVAILABLE > 2
                if ((I2C2_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
        #if !defined irq_I2C2_ID
                    if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_I2C2) != 0)
        #else
                    if (fnGenInt(irq_I2C2_ID) != 0)
        #endif
                    {                                                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        #if !defined irq_I2C2_ID
                        fnCallINTMUX(INTMUX_I2C2, INTMUX0_PERIPHERAL_I2C2, (unsigned char *)&ptrVect->processor_interrupts.irq_LPI2C2);
        #else
                        ptrVect->processor_interrupts.irq_I2C2();        // call the interrupt handler
        #endif
                    }
                }
    #elif LPI2C_AVAILABLE > 2
                if (((LPI2C2_MIER & LPI2C2_MSR) & (LPI2C_MIER_TDIE | LPI2C_MIER_RDIE | LPI2C_MIER_EPIE | LPI2C_MIER_SDIE | LPI2C_MIER_NDIE | LPI2C_MIER_ALIE | LPI2C_MIER_FEIE | LPI2C_MIER_PLTIE | LPI2C_MIER_DMIE)) != 0) { // if an enabled interrupt source is active
        #if !defined irq_LPI2C2_ID
                    if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_LPI2C2) != 0)
        #else
                    if (fnGenInt(irq_LPI2C2_ID) != 0)
        #endif
                    {
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPI2C2();      // call the interrupt handler
                    }
                }
    #endif
            }
        }
	}
#endif
#if NUMBER_I2C > 3
    if (((iInts & I2C_INT3) != 0) && (argv != 0)) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C3];
        if (*ptrCnt != 0) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT3;                                      // enough I2C interupts handled in this tick period
            }
            else {
		        iInts &= ~I2C_INT3;                                      // interrupt has been handled
                if ((I2C3_C1 & I2C_IEN) != 0) {                          // if I2C interrupt enabled
                    if (fnGenInt(irq_I2C3_ID) != 0) {                    // if I2C interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_I2C3();       // call the interrupt handler
                    }
                }
            }
        }
	}
#endif
#if defined SPI_INTERFACE
    if ((iInts & CHANNEL_0_SPI_INT) != 0) {
        iInts &= ~CHANNEL_0_SPI_INT;                                     // interrupt has been handled
    #if defined DSPI_SPI
        SPI0_SR |= SPI_SR_TFFF;
        if ((SPI0_RSER & SPI_SRER_TFFF_RE) != 0) {                       // if transmitter fifo not full interrupt enabled
            if (fnGenInt(irq_SPI0_ID) != 0) {                            // if SPI0 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_SPI0();                // call the interrupt handler
            }
        }
    #else
        _EXCEPTION("To do..");
    #endif
    }
    #if SPI_AVAILABLE > 1
    if ((iInts & CHANNEL_1_SPI_INT) != 0) {
        iInts &= ~CHANNEL_1_SPI_INT;                                     // interrupt has been handled
        #if defined DSPI_SPI
        SPI1_SR |= SPI_SR_TFFF;
        if ((SPI1_RSER & SPI_SRER_TFFF_RE) != 0)
        #elif defined LPSPI_SPI
        if (0)
        #else
        SPI1_S |= (SPI_S_SPTEF | SPI_S_SPRF);                            // set tranmitter empty and receiver not empty flags
        if ((SPI1_C1 & (SPI_C1_SPTIE | SPI_C1_SPIE)) != 0)               // if either of the interrupt sources are enabled
        #endif
        {                                                                // if transmitter fifo not full interrupt enabled
            #if !defined irq_SPI1_ID && defined INTMUX0_AVAILABLE
            if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_SPI1) != 0)
            #else
            if (fnGenInt(irq_SPI1_ID) != 0)                              // if SPI1 interrupt is not disabled
            #endif
            {
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            #if !defined irq_SPI1_ID && defined INTMUX0_AVAILABLE
                fnCallINTMUX(INTMUX_SPI1, INTMUX0_PERIPHERAL_SPI1, (unsigned char *)&ptrVect->processor_interrupts.irq_SPI1);
            #else
                ptrVect->processor_interrupts.irq_SPI1();                // call the interrupt handler
            #endif
            }
        }
    }
    #endif
    #if SPI_AVAILABLE > 2
    if ((iInts & CHANNEL_2_SPI_INT) != 0) {
        iInts &= ~CHANNEL_2_SPI_INT;                                     // interrupt has been handled
        #if defined DSPI_SPI
        SPI2_SR |= SPI_SR_TFFF;
        if ((SPI2_RSER & SPI_SRER_TFFF_RE) != 0) {                       // if transmitter fifo not full interrupt enabled
            if (fnGenInt(irq_SPI2_ID) != 0) {                            // if SPI2 interrupt is not disabled
                VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                ptrVect->processor_interrupts.irq_SPI2();                // call the interrupt handler
            }
        }
        #else
        _EXCEPTION("To do..");
        #endif
    }
    #endif
#endif
#if defined USB_INTERFACE
    #if defined USB_HS_INTERFACE                                         // {12}
    if ((iInts & USBHS_INT) != 0) {
        int iEndpoint = 0;
        iInts &= ~USBHS_INT;
        while ((iEndpoint < NUMBER_OF_USBHS_ENDPOINTS) && (ulHSEndpointInt != 0)) {
            if (ulHSEndpointInt & (1 << iEndpoint)) {
                ulHSEndpointInt &= ~(1 << iEndpoint);
                fnCheckUSBOut(0, iEndpoint);
            }
            iEndpoint++;
        }
    }
    #endif
    if (((iInts & USB_INT) & ~iMasks) != 0) {                            // full-speed USB
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

#if defined USB_INTERFACE

#define _fnLE_add(x) (void *)x

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
#if defined USB_HS_INTERFACE                                             // {12}
    static unsigned char ucHSRxBank[NUMBER_OF_USB_ENDPOINTS];            // monitor the buffer to inject to
#endif
    unsigned char *ptrData;
#if defined USB_HOST_SUPPORT
    int iRealEndpoint = iEndPoint;
#endif
    KINETIS_USB_ENDPOINT_BD *ptrBDT = (KINETIS_USB_ENDPOINT_BD *)((BDT_PAGE_01 << 8) + (BDT_PAGE_02 << 16) + (BDT_PAGE_03 << 24)); // address of buffer descriptors in RAM
#if defined USB_HS_INTERFACE                                             // {12}
    if ((USBHS_USBCMD & USBHS_USBCMD_RS) != 0) {                         // if the high speed USB controller is in the running state
        if (ptrDebugIn == 0) {                                           // bus state changes
            USBHS_USBSTS = 0;
            if ((usLenEvent & USB_RESET_CMD) != 0) {
                USBHS_USBSTS |= (USBHS_USBINTR_URE);                     // set USB reset interrupt flag
                USBHS_PORTSC1 = USBHS_PORTSC1_PR;
                memset(ucHSRxBank,   0, sizeof(ucHSRxBank));             // default is even bank
                memset(ucHSTxBuffer, 0, sizeof(ucHSTxBuffer));           // default is even buffer
            }
            if ((usLenEvent & USB_SLEEP_CMD) != 0) {
                USBHS_PORTSC1 |= USBHS_PORTSC1_SUSP;
                USBHS_USBSTS |= USBHS_USBINTR_SLE;
            }
            if ((usLenEvent & USB_RESUME_CMD) != 0) {
                USBHS_PORTSC1 &= ~USBHS_PORTSC1_SUSP;
                USBHS_USBSTS |= USBHS_USBINTR_PCE;
            }
            if ((usLenEvent & USB_IN_SUCCESS) != 0) {
                USBHS_EPCOMPLETE |= (USBHS_EPCOMPLETE_ETCE0 << iEndPoint); // transmission complete
                USBHS_USBSTS |= USBHS_USBINTR_UE;                        // transfer complete status
            }
    #if defined USB_HOST_SUPPORT
            if ((USB_HIGHSPEED_ATTACH_CMD & usLenEvent) != 0) {
                USBHS_PORTSC1 |= (USBHS_PORTSC1_PSPD_HS);
                if ((USBHS_PORTSC1 & USBHS_PORTSC1_CSC) == 0) {          // if not connected
                    USBHS_PORTSC1 |= (USBHS_PORTSC1_CCS | USBHS_PORTSC1_CSC); // change connection status to connectd
                }
                USBHS_USBSTS |= USBHS_USBINTR_PCE;
            }
            else if ((USB_FULLSPEED_ATTACH_CMD & usLenEvent) != 0) {
                USBHS_PORTSC1 &= ~(USBHS_PORTSC1_PSPD_HS);
                USBHS_PORTSC1 |= (USBHS_PORTSC1_PSPD_FS);
                if ((USBHS_PORTSC1 & USBHS_PORTSC1_CSC) == 0) {          // if not connected
                    USBHS_PORTSC1 |= (USBHS_PORTSC1_CCS | USBHS_PORTSC1_CSC); // change connection status to connectd
                }
                USBHS_USBSTS |= USBHS_USBINTR_PCE;
            }
            else if ((USB_LOWSPEED_ATTACH_CMD & usLenEvent) != 0) {
                USBHS_PORTSC1 &= ~(USBHS_PORTSC1_PSPD_HS);
                USBHS_PORTSC1 |= (USBHS_PORTSC1_PSPD_LS);
                if ((USBHS_PORTSC1 & USBHS_PORTSC1_CSC) == 0) {          // if not connected
                    USBHS_PORTSC1 |= (USBHS_PORTSC1_CCS | USBHS_PORTSC1_CSC); // change connection status to connectd
                }
                USBHS_USBSTS |= USBHS_USBINTR_PCE;
            }
    #endif
        }
        else {
            KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR + (2 * iEndPoint); // endpoint's reception queue header
            if ((unsigned char)(USBHS_DEVICEADDR >> USBHS_DEVICEADDR_USBADR_SHIFT) != (unsigned char)iDevice) { // not our device address so ignore
                if (iDevice != 0xff) {                                   // special broadcast for simulator use so that it doesn't have to know the USB address
                    return 1;
                }
            }
            USBHS_USBSTS |= USBHS_USBINTR_UE;                            // transfer complete status

            switch (ucPID) {
            case SETUP_PID:
                uMemcpy(ptrQueueHeader->ucSetupBuffer, ptrDebugIn, usLenEvent); // setup data content (always 8 bytes in length) is stored directly in the queue header
                USBHS_EPSETUPSR |= (USBHS_EPSETUPSR_SETUP0 << iEndPoint); // setup complete
                break;
            case OUT_PID:                                                // this presently generates an end of reception on each packet received - the USBHS controller does it only on complete data reception
                if (ptrQueueHeader->CurrentdTD_pointer != 0) {
                    if (ptrQueueHeader->CurrentdTD_pointer->ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_ACTIVE) { // transfer buffer enabled for reception
                        if (ptrQueueHeader->CurrentdTD_pointer->ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_IOC) { // if an interrupt is to be generated on completion
                            USBHS_EPCOMPLETE |= (USBHS_EPCOMPLETE_ERCE0 << iEndPoint); // reception complete
                        }
                        uMemcpy((void *)ptrQueueHeader->CurrentdTD_pointer->ulBufferPointerPage[0], ptrDebugIn, usLenEvent); // copy data
                        ptrQueueHeader->CurrentdTD_pointer->ul_dtToken -= (usLenEvent << ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT);
                        ptrQueueHeader->CurrentdTD_pointer->ul_dtToken &= ~(ENDPOINT_QUEUE_HEADER_TOKEN_STATUS_MASK);
                    }
                }
                break;
            }
            ucRxBank[iEndPoint] ^= ODD_BANK;                             // the next one to be used - toggle mechanism
        }
        if (fnGenInt(irq_USB_HS_ID) != 0) {                              // if USB HS interrupt is not disabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_USB_HS();                  // call the interrupt handler
        } 
    }
#endif
#if defined USB_HOST_SUPPORT                                             // {25}
    if ((CTL & (USB_EN_SOF_EN | HOST_MODE_EN)) == 0) {                   // if the USB controller is not enabled in host or device mode ignore inputs
        return 0;                                                        // ignore
    }
    if ((CTL & HOST_MODE_EN) != 0) {                                     // if host mode of operation
        iEndPoint = 0;                                                   // in host mode all reception is on endpoint 0
    }
#else
    if ((CTL & USB_EN_SOF_EN) == 0) {                                    // {13} if the USB controller is not enabled ignore inputs
        return 0;
    }
#endif

    STAT = ((unsigned char)iEndPoint << END_POINT_SHIFT);                // set the endpoint to the STAT register
    if (ptrDebugIn == 0) {                                               // bus state changes
        INT_STAT = 0;
        if ((usLenEvent & USB_RESET_CMD) != 0) {
            INT_STAT |= USB_RST;
            memset(ucRxBank,   0, sizeof(ucRxBank));                     // default is even bank
            memset(ucTxBuffer, 0, sizeof(ucTxBuffer));                   // default is even buffer
        }
        if ((usLenEvent & USB_SLEEP_CMD) != 0) {
            INT_STAT |= SLEEP;
        }
        if ((usLenEvent & USB_RESUME_CMD) != 0) {
            INT_STAT |= RESUME;
        }
        if ((usLenEvent & USB_IN_SUCCESS) != 0) {
            INT_STAT |= (TOK_DNE);                                       // interrupt status
            STAT |= TX_TRANSACTION;                                      // set status bit to indicate that a transmission was successful
            STAT |= (ucTxBuffer[iEndPoint] ^ ODD_BANK);                  // signal which buffer the frame is in
        }
    #if defined USB_HOST_SUPPORT                                         // {25}
        if ((USB_FULLSPEED_ATTACH_CMD & usLenEvent) != 0) {
            INT_STAT |= ATTACH;                                          // attach interrupt
            CTL |= JSTATE;                                               // full speed state (D+ is pulled up by device)
        }
        else if ((USB_LOWSPEED_ATTACH_CMD & usLenEvent) != 0) {
            INT_STAT |= ATTACH;                                          // attach interrupt
            CTL &= ~JSTATE;                                              // low speed state (D- is pulled up by device and D+ is low)
        }
        else if ((USB_SOF_EVENT & usLenEvent) != 0) {                    // SOF interrupt
            INT_STAT |= SOF_TOK;
        }
    #endif
    }
    else {                                                               // data being injected
        unsigned short usLength;
        if ((ADDR & ~LS_EN) != iDevice) {                                // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        INT_STAT |= TOK_DNE;                                             // interrupt status                                           
        STAT |= ucRxBank[iEndPoint];                                     // the buffer used for this reception

        ptrBDT += iEndPoint;
        if ((ucRxBank[iEndPoint] & ODD_BANK) == 0) {                     // even buffer
            if ((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & OWN) == 0) {   // check whether it is owned by the USB controller
                _EXCEPTION("Rx buffer not ready!!");
                return 1;                                                // no controller ownership so ignore
            }
            if ((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & KEEP_OWNERSHIP) == 0) {
                ptrBDT->usb_bd_rx_even.ulUSB_BDControl &= ~OWN;          // mark that the buffer is no longer owned by the USB controller
            }
            usLength = (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // the size that this endpoint can receive
            if (usLength < usLenEvent) {                                 // limit the injected length to the endpoint limit
                usLenEvent = usLength;
            }
    #if defined USB_HOST_SUPPORT
            if ((CTL & HOST_MODE_EN) != 0) {
                if (ptrBDT->usb_bd_rx_even.ulUSB_BDControl & DTS) {      // if data toggle synchronisation is being used
                    if (((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & DATA_1) == 0) == iData1Frame[iRealEndpoint]) { // check that the receive buffer accepts this data token
                        _EXCEPTION("Wrong Rx buffer!!");
                    }
                }
            }
    #endif
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl &= ~(RX_PID_MASK | USB_BYTE_CNT_MASK | DATA_1);
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (ucPID << RX_PID_SHIFT);
            ptrData = ptrBDT->usb_bd_rx_even.ptrUSB_BD_Data;
            ptrData = _fnLE_add((unsigned long)ptrData);
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= SET_FRAME_LENGTH(usLenEvent); // add length
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {                             // in host mode
                if (STALL_PID == ucPID) {                                // if stall being injected on this endpoint
                    ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (STALL_PID << RX_PID_SHIFT);
                }
                else if (iData1Frame[iRealEndpoint] != 0) {              // if the endpoint is expecting a DATA 1 frame
                    ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= ((DATA1_PID << RX_PID_SHIFT) | DATA_1); // DATA1 frame
                }
                else {
                    ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (DATA0_PID << RX_PID_SHIFT); // DATA0 frame
                }
                iData1Frame[iRealEndpoint] ^= 1;                         // toggle the DATA type
                CTL &= ~(TXSUSPEND_TOKENBUSY);                           // token no longer in progress
            }
    #endif
        }
        else {                                                           // odd buffer
            if ((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & OWN) == 0) {
                _EXCEPTION("Rx buffer not ready!!");
                return 1;                                                // no controller ownership so ignore
            }
            if ((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & KEEP_OWNERSHIP) == 0) {
                ptrBDT->usb_bd_rx_odd.ulUSB_BDControl &= ~OWN;           // the buffer descriptor is now owned by the controller
            }
            usLength = (unsigned short)((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // the size that this endpoint can receive
            if (usLength < usLenEvent) {                                 // limit the injected length to the endpoint limit
                usLenEvent = usLength;
            }
    #if defined USB_HOST_SUPPORT
            if ((CTL & HOST_MODE_EN) != 0) {
                if ((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & DTS) != 0) {// if data toggle synchronisation is being used
                    if (((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & DATA_1) == 0) == iData1Frame[iRealEndpoint]) { // check that the receive buffer accepts this data token
                        _EXCEPTION("Wrong Rx buffer!!");
                    }
                }
            }
    #endif
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl &= ~(RX_PID_MASK | USB_BYTE_CNT_MASK);
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (ucPID << RX_PID_SHIFT);
            ptrData = ptrBDT->usb_bd_rx_odd.ptrUSB_BD_Data;
            ptrData = _fnLE_add((unsigned long)ptrData);
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= SET_FRAME_LENGTH(usLenEvent); // add length
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {
                if (STALL_PID == ucPID) {
                    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (STALL_PID << RX_PID_SHIFT);
                }
                else if (iData1Frame[iRealEndpoint] != 0) {
                    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= ((DATA1_PID << RX_PID_SHIFT) | DATA_1); // DATA1 frame
                }
                else {
                    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (DATA0_PID << RX_PID_SHIFT); // DATA0 frame
                }
                iData1Frame[iRealEndpoint] ^= 1;
                CTL &= ~(TXSUSPEND_TOKENBUSY);                           // token no longer in progress
            }
    #endif
        }
        memcpy(ptrData, ptrDebugIn, usLenEvent);
        ucRxBank[iEndPoint] ^= ODD_BANK;                                 // the next rx buffer to be used - toggle mechanism
    }
    if (fnGenInt(irq_USB_OTG_ID) != 0) {                                 // if USB interrupt is not disabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        ptrVect->processor_interrupts.irq_USB_OTG();                     // call the interrupt handler
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
    case USB_SIM_SOF:
        if ((CTL & HOST_MODE_EN) && (INT_ENB & SOF_TOK_EN)) {            // if host mode and SOF enabled
            iInts |= USB_INT;;
            ulEndpointInt = 1;                                           // flag endpoint - always endpoint 0 used
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
#if defined USB_HS_INTERFACE                                             // {12}
        if (ptrUSB_HW->ucDeviceType == USB_DEVICE_HS) {
            KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
            ptrQueueHeader += ((2 * iEndpoint) + 1);                     // move to the transmitter queue header
            if (ptrQueueHeader->CurrentdTD_pointer != 0) {
                uMemcpy((void *)ptrQueueHeader->CurrentdTD_pointer, &ptrQueueHeader->dTD, sizeof(USB_HS_TRANSFER_OVERLAY)); // transfer the transfer block content to the overlay block
                if (ptrQueueHeader->CurrentdTD_pointer->ul_dtToken & ENDPOINT_QUEUE_HEADER_TOKEN_IOC) { // if transfer termination should generate an interrupt
                    iInts |= USBHS_INT;                                  // flag that the interrupt should be handled
                    ulHSEndpointInt |= (1 << iEndpoint);                 // flag endpoint
                }
            }
            break;
        }
#endif
        if ((OWN & *ptrUSB_HW->ptr_ulUSB_BDControl) == 0) {              // if the ownership has not been passed to the USB controller ignore it
            _EXCEPTION("Ignored frame due to incorrect buffer ownership");
            return;
        }
        iInts |= USB_INT;                                                // flag that the interrupt should be handled
        ulEndpointInt |= (1 << iEndpoint);                               // flag endpoint
        break;
    case USB_SIM_ENUMERATED:                                             // flag that we have completed enumeration
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


#if defined USB_HOST_SUPPORT                                             // {25}

#define MAX_TOKENS 10
static unsigned char ucTokenQueue[MAX_TOKENS] = {0};
static int iTokenIn = 0;
static int iTokenOut = 0;

extern void fnAddToken(unsigned char ucToken)
{
    ucTokenQueue[iTokenIn++] = ucToken;
    if (iTokenIn >= MAX_TOKENS) {
        iTokenIn = 0;
    }
    iInts |= USB_INT;                                                    // there will generally be an interrupt after this so set the interrupt flag
    ulEndpointInt |= (0x1 << (ucToken & 0x0f));
}

static unsigned char ucGetToken(int iNoIN)
{
    unsigned char ucToken;
    do {
        ucToken = ucTokenQueue[iTokenOut];
        if (ucToken != 0) {
            ucTokenQueue[iTokenOut] = 0;
            ++iTokenOut;
            if (iTokenOut >= MAX_TOKENS) {
                iTokenOut = 0;
            }
        }
    } while ((iNoIN != 0) && ((ucToken >> 4) == IN_PID));                // skip IN tokens if so requested
    return ucToken;
}

#if defined USB_MSD_HOST
#define MEMORY_STICK_USES_SHARED_ENDPOINTS
#define DEVICE_ENPOINT0_LENGTH   64
static const unsigned char ucDeviceDescriptor[] = {                      // constant device descriptor (example of memory stick)
    0x12,
    0x01,
    0x00, 0x02,
    0x00, 0x00, 0x00,
    DEVICE_ENPOINT0_LENGTH,
    0x0d, 0x1d,
    0x13, 0x02,
    0x10, 0x01,
    0x01, 0x02, 0x03,
    0x01
};

static const unsigned char ucConfigDescriptor[] = {                      // constant configuration descriptor (example of memory stick)
    0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x64,                // configuration descriptor
    0x09, 0x04, 0x00, 0x00, 0x03, 0x08, 0x06, 0x50, 0x00,                // interface descriptor
    #if defined MEMORY_STICK_USES_SHARED_ENDPOINTS
    0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,                            // bulk IN endpoint - 64 bytes on endpoint 2
    #else
    0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00,                            // bulk IN endpoint - 64 bytes on endpoint 1
    #endif
    0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,                            // bulk OUT endpoint - 64 bytes on endpoint 2
    0x07, 0x05, 0x83, 0x03, 0x02, 0x00, 0x01                             // interrupt IN endpoint - 2 bytes - 1ms polling rate
};


static unsigned char ucDataTransport[512] = {0};
static unsigned short usDataTransportLength = 0;
static int iStall = 0;
static unsigned long ulFrameCount = 0;
static unsigned char ucLastOpCode = 0;
static unsigned char ucPresentLUN = 0;
static unsigned long ulLBA = 0;
static unsigned char ucMemoryStickSector[512] = {0};
static int iSectorOffset = 0;

static unsigned char ucStatusTransport[] = {
    0x55, 0x53, 0x42, 0x53,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00
};

static const unsigned char ucMaxLum[1] = {0};

static int fnMsdModel(unsigned char *ptrUSBData, unsigned short usUSBLength)
{
    USB_MASS_STORAGE_CBW_LW *ptrBlockWrapper = (USB_MASS_STORAGE_CBW_LW *)ptrUSBData;
    if (ucLastOpCode == UFI_WRITE_10) {                                  // OUT data that belongs to a write is not a block wrapper
        if (usUSBLength != 64) {
            _EXCEPTION("Endpoint size of 64 always expected!!");
        }
        memcpy(&ucMemoryStickSector[iSectorOffset], ptrUSBData, usUSBLength); // collect the data into the temporary buffer
        iSectorOffset += usUSBLength;                                    // the new length of data collected
        if (--ulFrameCount == 0) {
            ucLastOpCode = 0;                                            // complete write has been received so reset the op code
        }
        if (iSectorOffset >= 512) {                                      // if a complete sector is available
            fnPutSimDiskData(ucMemoryStickSector, ucPresentLUN, ulLBA++);// save it to the memory stick
            iSectorOffset = 0;
        }
        return 1;                                                        // OUT belonging to a write has been handled
    }
    if (uMemcmp(ptrUSBData, "USBC", 4) != 0) {
        _EXCEPTION("USBC expected!");
    }
    ptrUSBData += 4;
    uMemcpy(&ucStatusTransport[4], ptrUSBData, 4);                       // tag ID for status stage
    ulFrameCount = 1;                                                    // default is that the content fits in a single frame
    ucLastOpCode = ptrBlockWrapper->CBWCB[0];
    switch (ucLastOpCode) {                                              // op code
    case UFI_INQUIRY:
        {
            static const unsigned char ucUFI_INQUIRY[] = {               // reference response to UFI_INQUIRY
                0x00, 0x80, 0x02, 0x02,                                  // removable SCSI-2
                0x1f, 0x00, 0x00, 0x00,                                  // length 31
                0x53, 0x57, 0x49, 0x53,                                  // SWISSBIT
                0x53, 0x42, 0x49, 0x54,
                0x56, 0x69, 0x63, 0x74,                                  // Victorinox 2.0
                0x6f, 0x72, 0x69, 0x6e,
                0x6f, 0x78, 0x20, 0x32,
                0x2e, 0x30, 0x20, 0x20,
                0x32, 0x2e, 0x30, 0x30
            };
            memcpy(ucDataTransport, ucUFI_INQUIRY, sizeof(ucUFI_INQUIRY));
            usDataTransportLength = sizeof(ucUFI_INQUIRY);
//iStall = 1; // test stalling
        }
        break;
    case UFI_REQUEST_SENSE:
        {
            static const unsigned char ucUFI_REQUEST_SENSE[] = {         // reference response to UFI_REQUEST_SENSE
                0x70, 0x00, 0x06, 0x00,                                  // unit attention
                0x00, 0x00, 0x00, 0x0a,
                0x00, 0x00, 0x00, 0x00,
                0x28, 0x00, 0x00, 0x00,
                0x00, 0x00
            };
            memcpy(ucDataTransport, ucUFI_REQUEST_SENSE, sizeof(ucUFI_REQUEST_SENSE));
            usDataTransportLength = sizeof(ucUFI_REQUEST_SENSE);
        }
        break;
    case UFI_READ_FORMAT_CAPACITY:
        {
            static const unsigned char ucUFI_READ_FORMAT_CAPACITY[] = {  // reference response to UFI_READ_FORMAT_CAPACITY
                0x00, 0x00, 0x00, 0x08,                                  // capacity list length 8
                0x01, 0x00, 0x00, 0x00,                                  // number of blocks 16777216
                0x03, 0x00, 0x02, 0x00                                   // block size 512
            };
            memcpy(ucDataTransport, ucUFI_READ_FORMAT_CAPACITY, sizeof(ucUFI_READ_FORMAT_CAPACITY));
            usDataTransportLength = sizeof(ucUFI_READ_FORMAT_CAPACITY);
        }
        break;
    case UFI_READ_CAPACITY:
        {
            static const unsigned char ucUFI_READ_CAPACITY[] = {         // reference response to UFI_READ_CAPACITY
                0x00, 0x0f, 0x82, 0xff,                                  // logical bock address 1016575
                0x01, 0x00, 0x20, 0x00                                   // logical block length 512 (496.392 MB capacity)
            };
            memcpy(ucDataTransport, ucUFI_READ_CAPACITY, sizeof(ucUFI_READ_CAPACITY));
            usDataTransportLength = sizeof(ucUFI_READ_CAPACITY);
        }
        break;
    case UFI_WRITE_10:
    case UFI_READ_10:
        {
            if (ptrBlockWrapper->dCBWCBLength != 10) {
                _EXCEPTION("Incorrect read length!!");
            }
            ulLBA = ((ptrBlockWrapper->CBWCB[2] << 24) | (ptrBlockWrapper->CBWCB[3] << 16) | (ptrBlockWrapper->CBWCB[4] << 8) | (ptrBlockWrapper->CBWCB[5])); // the block number where the read starts
            ulFrameCount = ((ptrBlockWrapper->CBWCB[6] << 16) | (ptrBlockWrapper->CBWCB[7] << 8) | (ptrBlockWrapper->CBWCB[8])); // the number of bocks to be read
            ulFrameCount *= (512/64);                                    // the total amount of USB frames that will be returned
            ucPresentLUN = ptrBlockWrapper->dCBWLUN;
            if (UFI_READ_10 == ucLastOpCode) {
                fnGetSimDiskData(ucMemoryStickSector, ucPresentLUN, ulLBA); // read the sector's content from the simulated memory stick
                usDataTransportLength = 64;                              // endpoint assumed to be 64 bytes in length
                memcpy(ucDataTransport, ucMemoryStickSector, usDataTransportLength); // prepare the frame to be returned
                iSectorOffset = 64;
            }
            else {
                usDataTransportLength = 0;
                iSectorOffset = 0;
                return 1;                                                // write
            }
        }
        break;
    default:
        _EXCEPTION("Unexpected Op Code!");
        break;
    }
    return 0;
}
#elif defined USB_CDC_HOST
#define DEVICE_ENPOINT0_LENGTH   8
static const unsigned char ucDeviceDescriptor[] = {                      // constant device descriptor (example of CDC)
    0x12, 0x01, 0x00, 0x02, 0xef, 0x02, 0x01, DEVICE_ENPOINT0_LENGTH, 0xA2, 0x15, 0x44, 0x00, 0x00, 0x01,
    0x01, 0x02, 0x03, 0x01 
};

static const unsigned char ucConfigDescriptor[] = {                      // constant configuration descriptor (example of CDC)
#if 0                                                                    // CDC with two interfaces
    0x09, 0x02, 0x8D, 0x00, 0x04, 0x01, 0x04, 0xC0, 0x00, 0x08, 0x0B, 0x00, 0x02, 0x02,
    0x02, 0x00, 0x00, 0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x05, 0x05, 0x24,
    0x00, 0x10, 0x01, 0x05, 0x24, 0x01, 0x01, 0x00, 0x04, 0x24, 0x02, 0x02, 0x05, 0x24,
    0x06, 0x00, 0x01, 0x07, 0x05, 0x82, 0x03, 0x40, 0x00, 0x0A, 0x09, 0x04, 0x01, 0x00,
    0x02, 0x0A, 0x00, 0x00, 0x05, 0x07, 0x05, 0x01, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05,
    0x81, 0x02, 0x40, 0x00, 0x00, 0x08, 0x0B, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x09,
    0x04, 0x02, 0x00, 0x01, 0x02, 0x02, 0x00, 0x05, 0x05, 0x24, 0x00, 0x10, 0x01, 0x05,
    0x24, 0x01, 0x01, 0x00, 0x04, 0x24, 0x02, 0x02, 0x05, 0x24, 0x06, 0x00, 0x01, 0x07,
    0x05, 0x84, 0x03, 0x40, 0x00, 0x0A, 0x09, 0x04, 0x03, 0x00, 0x02, 0x0A, 0x00, 0x00,
    0x05, 0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x83, 0x02, 0x40, 0x00,
    0x00
#elif 1                                                                  // CDC + MSD composite
    0x09, 0x02, 0x79, 0x00, 0x04, 0x01, 0x00, 0xC0, 0xE8, 0x08, 0x0B, 0x00, 0x02, 0x02,
    0x03, 0x00, 0x02, 0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x00, 0x05, 0x24,
    0x00, 0x10, 0x01, 0x05, 0x24, 0x01, 0x01, 0x01, 0x04, 0x24, 0x02, 0x06, 0x05, 0x24,
    0x06, 0x00, 0x01, 0x07, 0x05, 0x85, 0x03, 0x10, 0x00, 0x0A, 0x09, 0x04, 0x01, 0x00,
    0x02, 0x0A, 0x00, 0x00, 0x00, 0x07, 0x05, 0x83, 0x02, 0x10, 0x00, 0x00, 0x07, 0x05,
    0x04, 0x02, 0x10, 0x00, 0x00, 0x09, 0x04, 0x02, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0x04,
    0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,
    0x09, 0x04, 0x03, 0x00, 0x02, 0x08, 0x06, 0x50, 0x00, 0x07, 0x05, 0x86, 0x02, 0x40,
    0x00, 0x00, 0x07, 0x05, 0x07, 0x02, 0x40, 0x00, 0x00
#else
    0x09, 0x02, 0x4b, 0x00, 0x02, 0x01, 0x04, 0xc0, 0x00,                // configuration descriptor
    0x08, 0x0b, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00,                      // interface association descriptor
    0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x00, 0x05,                // interface descriptor
    0x05, 0x24, 0x00, 0x00, 0x02,                             §           // class descriptor
    0x05, 0x24, 0x01, 0x01, 0x00,                                        // class descriptor
    0x04, 0x24, 0x02, 0x02,                                              // class descriptor
    0x05, 0x24, 0x06, 0x00, 0x01,                                        // class descriptor
    0x07, 0x05, 0x81, 0x03, 0x20, 0x00, 0x0a,                            // interrupt IN endpoint - 32 bytes on endpoint 1
    0x09, 0x04, 0x01, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x05,                // interface descriptor
    0x07, 0x05, 0x02, 0x02, 0x20, 0x00, 0x00,                            // bulk OUT endpoint - 32 bytes on endpoint 2
    0x07, 0x05, 0x82, 0x02, 0x20, 0x00, 0x00,                            // bulk OUT endpoint - 32 bytes on endpoint 2
#endif
};
#endif


static unsigned char ucStringDescriptor[] = {                            // string descriptor
    0x00,                                                                // length
    0x03,                                                                // type
    0x00, 0x00,                                                          // variable length content
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
};

#define QUEUE_DEVICE_DESCRIPTOR 1
#define QUEUE_ZERO_DATA         2
#define QUEUE_CONFIG_DESCRIPTOR 3
#define QUEUE_STRING_DESCRIPTOR 4
#define QUEUE_MAX_LUN           5

static void fnSimulateUSB_ep0(int iAdd, unsigned char *ptrData, unsigned short usSize)
{
    unsigned short usSent = 0;
    unsigned short usRemaining = usSize;
    while (usRemaining != 0) {
        if (usRemaining > DEVICE_ENPOINT0_LENGTH) {
            fnSimulateUSB(iAdd, 0, 0, ptrData, DEVICE_ENPOINT0_LENGTH);
            usRemaining -= DEVICE_ENPOINT0_LENGTH;
            ptrData += DEVICE_ENPOINT0_LENGTH;
        }
        else {
            fnSimulateUSB(iAdd, 0, 0, ptrData, usRemaining);
            if (usRemaining == DEVICE_ENPOINT0_LENGTH) {
                fnSimulateUSB(iAdd, 0, 0, ptrData, 0);                   // terminate with zero length frame
            }
            break;
        }
    }
}

// This routine reacts to embedded host mode transfers as a device would do
//
static void fnUSBHostModel(int iEndpoint, unsigned char ucPID, unsigned short usUSBLength, unsigned char *ptrUSBData)
{
    static unsigned char ucStringRef = 0;
    static int iHostQueue = 0;

    switch (ucPID) {
    case 0:                                                              // reset
        iHostQueue = 0;
        break;
    case SETUP_PID:                                                      // the host is sending setup data
        {
            USB_HOST_DESCRIPTOR *ptrDescriptor = (USB_HOST_DESCRIPTOR *)ptrUSBData;
            if (usUSBLength != 8) {
                _EXCEPTION("Bad descriptor length detected!");
            }
            if (ptrDescriptor->bmRecipient_device_direction == (STANDARD_DEVICE_TO_HOST | REQUEST_INTERFACE_CLASS)) { // class spefific interface request
    #if defined USB_MSD_HOST
                if (GET_MAX_LUN == ptrDescriptor->bRequest) {            // requesting maximum LUN
                    iHostQueue = QUEUE_MAX_LUN;                          // send the data
                }
    #endif
                break;
            }
            if (USB_REQUEST_GET_DESCRIPTOR == ptrDescriptor->bRequest) { // host is requesting a descriptor 
                if (ptrDescriptor->wValue[1] == (STANDARD_DEVICE_DESCRIPTOR >> 8)) { // requesting the standard device descriptor
                    iHostQueue = QUEUE_DEVICE_DESCRIPTOR;                // send the device descriptor on next IN
                }
                else if (ptrDescriptor->wValue[1] == (STANDARD_CONFIG_DESCRIPTOR >> 8)) { // requesting the standard configuration descriptor
                    iHostQueue = QUEUE_CONFIG_DESCRIPTOR;                // send the configuration descriptor on next IN
                }
                else if (ptrDescriptor->wValue[1] == (STANDARD_STRING_DESCRIPTOR >> 8)) { // host requesting a string
                    iHostQueue = QUEUE_STRING_DESCRIPTOR;                // send the configuration descriptor on next IN
                    switch (ptrDescriptor->wValue[0]) {                  // the string referenced
                    case 0:                                              // string language ID
                        ucStringDescriptor[0] = 4;                       // length
                        ucStringDescriptor[2] = 0x09;                    // English (US)
                        ucStringDescriptor[3] = 0x04;
                        break;
                    case 1:                                              // manufacturer
                        ucStringDescriptor[0] = 0x12;                    // length
                        ucStringDescriptor[2] = 0x54;
                        ucStringDescriptor[3] = 0x00;
                        ucStringDescriptor[4] = 0x44;
                        ucStringDescriptor[5] = 0x00;
                        ucStringDescriptor[6] = 0x4b;
                        ucStringDescriptor[7] = 0x00;
                        ucStringDescriptor[8] = 0x4d;
                        ucStringDescriptor[9] = 0x00;
                        ucStringDescriptor[10] = 0x65;
                        ucStringDescriptor[11] = 0x00;
                        ucStringDescriptor[12] = 0x64;
                        ucStringDescriptor[13] = 0x00;
                        ucStringDescriptor[14] = 0x69;
                        ucStringDescriptor[15] = 0x00;
                        ucStringDescriptor[16] = 0x61;
                        ucStringDescriptor[17] = 0x00;
                        break;
                    case 2:                                              // product
                        ucStringDescriptor[0] = 0x10;                    // length
                        ucStringDescriptor[2] = 'C';
                        ucStringDescriptor[3] = 0x00;
                        ucStringDescriptor[4] = 'D';
                        ucStringDescriptor[5] = 0x00;
                        ucStringDescriptor[6] = 'C';
                        ucStringDescriptor[7] = 0x00;
                        ucStringDescriptor[8] = ' ';
                        ucStringDescriptor[9] = 0x00;
                        ucStringDescriptor[10] = ' ';
                        ucStringDescriptor[11] = 0x00;
                        ucStringDescriptor[12] = ' ';
                        ucStringDescriptor[13] = 0x00;
                        ucStringDescriptor[14] = ' ';
                        ucStringDescriptor[15] = 0x00;
                        break;
                    case 3:                                              // serial number
                        ucStringDescriptor[0] = 0x0c;                    // length
                        ucStringDescriptor[2] = 0x31;
                        ucStringDescriptor[3] = 0x00;
                        ucStringDescriptor[4] = 0x32;
                        ucStringDescriptor[5] = 0x00;
                        ucStringDescriptor[6] = 0x33;
                        ucStringDescriptor[7] = 0x00;
                        ucStringDescriptor[8] = 0x34;
                        ucStringDescriptor[9] = 0x00;
                        ucStringDescriptor[10] = 0x35;
                        ucStringDescriptor[11] = 0x00;
                        break;
                    }
                }
            }
            else if (USB_REQUEST_SET_ADDRESS == ptrDescriptor->bRequest) { // address being set
                static unsigned char ucDeviceAddress;
                ucDeviceAddress = ptrDescriptor->wValue[0];
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else if (USB_REQUEST_SET_CONFIGURATION == ptrDescriptor->bRequest) { // configuration being set
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else if (USB_REQUEST_CLEAR_FEATURE == ptrDescriptor->bRequest) { // clear feature
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else if (USB_REQUEST_SET_INTERFACE == ptrDescriptor->bRequest) { // set interface
                iHostQueue = QUEUE_ZERO_DATA;                            // device responds with a zero data IN
            }
            else {
                _EXCEPTION("Unknown request");
            }
        }
        break;
    case IN_PID:                                                         // the device can insert data since the host is sending IN PID
        switch (iHostQueue) {
        case QUEUE_DEVICE_DESCRIPTOR:                                    // inject the device descriptor
            fnSimulateUSB_ep0((ADDR & ~LS_EN),(unsigned char *)&ucDeviceDescriptor, sizeof(ucDeviceDescriptor));
            break;
        case QUEUE_CONFIG_DESCRIPTOR:
            fnSimulateUSB_ep0((ADDR & ~LS_EN),(unsigned char *)&ucConfigDescriptor, sizeof(ucConfigDescriptor));
            break;
        case QUEUE_STRING_DESCRIPTOR:
            fnSimulateUSB_ep0((ADDR & ~LS_EN), (unsigned char *)&ucStringDescriptor, ucStringDescriptor[0]);
            break;
    #if defined USB_MSD_HOST
        case QUEUE_MAX_LUN:
            fnSimulateUSB((ADDR & ~LS_EN), 0, 0, (unsigned char *)&ucMaxLum, sizeof(ucMaxLum));
            break;
    #endif
        case QUEUE_ZERO_DATA:
            fnSimulateUSB((ADDR & ~LS_EN), 0, 0, (unsigned char *)1, 0); // inject a zero data frame
            break;
        default:
    #if defined USB_MSD_HOST
        #if defined MEMORY_STICK_USES_SHARED_ENDPOINTS
            if (iEndpoint == 2)                                          // IN is on endpoint 2
        #else
            if (iEndpoint == 1)                                          // IN is on endpoint 1
        #endif
            {
                if (iStall != 0) {
                    // Stall the endoint
                    //
                    iStall = 0;
                    fnSimulateUSB((ADDR & ~LS_EN), iEndpoint, STALL_PID, (unsigned char *)1, 0); // inject a stall token on this IN endpoint
                    break;
                }
                if (ulFrameCount != 0) {
                    fnSimulateUSB((ADDR & ~LS_EN), iEndpoint, 0, (unsigned char *)&ucDataTransport, usDataTransportLength);
                    ulFrameCount--;
                    if (ulFrameCount != 0) {                             // if the data occupies multiple frames
                        if (ucLastOpCode == UFI_READ_10) {               // we are presently reading sector content
                            if (iSectorOffset >= 512) {                  // if a complete sector has been read
                                iSectorOffset = 0;
                                ulLBA++;                                 // move to the next sector
                                fnGetSimDiskData(ucMemoryStickSector, ucPresentLUN, ulLBA); // read the sector's content from the simulated memory stick
                            }
                            usDataTransportLength = 64;                  // endpoint assumed to be 64 bytes in length
                            memcpy(ucDataTransport, (ucMemoryStickSector + iSectorOffset), usDataTransportLength); // prepare the frame to be returned
                            iSectorOffset += 64;
                        }
                        else {
                            _EXCEPTION("Add further op codes!!");
                        }

                    }
                }
                else {
                    fnSimulateUSB((ADDR & ~LS_EN), iEndpoint, 0, (unsigned char *)&ucStatusTransport, sizeof(ucStatusTransport));
                }
            }
    #endif
            break;
        }
        iHostQueue = 0;
        break;
    case OUT_PID:
    #if defined USB_MSD_HOST
        if (2 == iEndpoint) {                                            // we expect bulk out on this endpoint only
            fnMsdModel(ptrUSBData, usUSBLength);                         // allow MSD model to handle the data
        }
        else if ((0 == iEndpoint) && (usUSBLength == 0)) {               // expect only zero terminations on endpoint 0
        }
        else {
            _EXCEPTION("Unexpected");
        }
    #endif
        break;
    }
}

// Synchronise to correct data frame for all endpoints
//
extern void fnResetUSB_buffers(void)
{
    int i = 0;
    iData1Frame[i++] = 1;                                                // reset endpoint 0 to DATA1
    while (i < NUMBER_OF_USB_ENDPOINTS) {                                // {42}
        iData1Frame[i++] = 0;                                            // reset additional endpoint data frames to DATA0
    }
}
#endif

// Check whether data has been prepared for transmission
//
extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    KINETIS_USB_BD *bufferDescriptor;
    unsigned long ulAddress = ((BDT_PAGE_01 << 8) | (BDT_PAGE_02 << 16) | (BDT_PAGE_03 << 24));
    KINETIS_USB_ENDPOINT_BD *bdt = (KINETIS_USB_ENDPOINT_BD *)ulAddress;
    #if TICK_RESOLUTION >= 1000
    int iMaxUSB_ints = (TICK_RESOLUTION/1000);
    #else
    int iMaxUSB_ints = 1;
    #endif
    #if defined USB_HOST_SUPPORT
    int iRealEndpoint = iEndpoint;
    #endif
    #if defined USB_HS_INTERFACE                                         // {12}
    if ((USBHS_USBCMD & USBHS_USBCMD_RS) != 0) {                         // if USB HS controller operating
        KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
        ptrQueueHeader += ((2 * iEndpoint) + 1);                         // move to the transmitter queue header
        if (ptrQueueHeader->CurrentdTD_pointer != 0) {
            unsigned short usDataLength = (unsigned short)(ptrQueueHeader->CurrentdTD_pointer->ul_dtToken >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT);
            unsigned short usMaxPacket = (unsigned short)(ptrQueueHeader->ulCapabilities >> 16);
            unsigned char *ptrUSBData = (unsigned char *)ptrQueueHeader->CurrentdTD_pointer->ulBufferPointerPage[0];
            do {                                                         // the USB HS controller automatically sends complete buffers as single frames and there is only a single interrupt on completion
                if (usDataLength < usMaxPacket) {
                    usMaxPacket = usDataLength;
                }
                fnLogUSB(iEndpoint, 0, usMaxPacket, ptrUSBData, ((ucHSTxBuffer[iEndpoint] & ODD_BANK) != 0));
                ucHSTxBuffer[iEndpoint] ^= ODD_BANK;                     // toggle buffer
                usDataLength -= usMaxPacket;
                ptrUSBData += usMaxPacket;
            } while (usDataLength != 0);
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
            ptrQueueHeader->CurrentdTD_pointer->ulBufferPointerPage[0] = (unsigned long)ptrUSBData;
        }
    }
    #endif
    if ((CTL & USB_EN_SOF_EN) == 0) {                                    // {13} if the FS USB controller is not enabled ignore inputs
        return;
    }
    bdt += iEndpoint;

    do {
    #if defined USB_HOST_SUPPORT                                         // {25}
        if ((CTL & HOST_MODE_EN) != 0) {
            iEndpoint = 0;
            bdt = (KINETIS_USB_ENDPOINT_BD *)ulAddress;                  // in host mode only endpoint 0 buffers are used
            if ((INT_ENB & SOF_TOK_EN) != 0) {                           // if host mode and SOF enabled
                fnSimulateUSB(iDevice, 0, 0, 0, USB_SOF_EVENT);          // generate SOF interrupt
            }
        }
    #endif
        if (ucTxBuffer[iEndpoint] != 0) {                                // decide which is the active output buffer descriptor
            bufferDescriptor = &bdt->usb_bd_tx_odd;                      // the odd one is to be used
        }
        else {
            bufferDescriptor = &bdt->usb_bd_tx_even;                     // the even one is to be used
        }
        if ((bufferDescriptor->ulUSB_BDControl & OWN) != 0) {            // owned by USB controller so interpret it
            unsigned short usUSBLength;
            unsigned char *ptrUSBData = 0;
            usUSBLength  = (unsigned char)((bufferDescriptor->ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // get the length from the control register
            if (usUSBLength != 0) {
                ptrUSBData = _fnLE_add((CAST_POINTER_ARITHMETIC)bufferDescriptor->ptrUSB_BD_Data); // the data to be sent
            }
            if ((bufferDescriptor->ulUSB_BDControl & KEEP_OWNERSHIP) == 0) { // if the KEEP bit is not set
                bufferDescriptor->ulUSB_BDControl &= ~OWN;               // remove SIE ownership
            }
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {                             // if in host mode
                unsigned char ucToken = ucGetToken(1);                   // the token that was sent (skip INs)
                if ((ucToken >> 4) == SETUP_PID) {                       // a SETUP token was sent (will always be on control endpoint 0
                    fnLogUSB(iRealEndpoint, SETUP_PID, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0)); // log the transmitted data
                    fnUSBHostModel((ucToken & 0x0f), SETUP_PID, usUSBLength, ptrUSBData); // let the host model handle the data
                }
                else if ((ucToken >> 4) == OUT_PID) {                    // an OUT token was sent
                    fnLogUSB(iRealEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
                    fnUSBHostModel((ucToken & 0x0f), OUT_PID, usUSBLength, ptrUSBData); // let the host model handle the data
                }
                else {
                    _EXCEPTION("Unexpected token queued");               // IN tokens are not expected to be returned since they should be skipped
                    return;
                }
                bufferDescriptor->ulUSB_BDControl &= ~(RX_PID_MASK);
                bufferDescriptor->ulUSB_BDControl |= (ACK_PID << RX_PID_SHIFT); // insert ACK
                ucTxBuffer[0] ^= ODD_BANK;                               // toggle buffer (always on endpoint 0)
                CTL &= ~(TXSUSPEND_TOKENBUSY);                           // token no longer in progress
                fnSimulateUSB(iDevice, 0, 0, 0, USB_IN_SUCCESS);         // generate tx interrupt (always on endpoint 0)
            }
            else {                                                       // device mode
                fnLogUSB(iRealEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
                ucTxBuffer[iRealEndpoint] ^= ODD_BANK;                   // toggle buffer
                fnSimulateUSB(iDevice, iRealEndpoint, 0, 0, USB_IN_SUCCESS); // generate tx interrupt
            }
    #else
            fnLogUSB(iEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
            ucTxBuffer[iEndpoint] ^= ODD_BANK;                           // toggle buffer
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
    #endif
        }                                                                // handle further buffer if available
        else {                                                           // no data to be sent - the buffer descrptior is not owned
    #if defined USB_HOST_SUPPORT                                         // {25}
            if ((CTL & HOST_MODE_EN) != 0) {                             // if in host mode
                unsigned char ucToken = ucGetToken(0);                   // we expect an IN
                if ((ucToken >> 4) == IN_PID) {                          // an IN token is to be sent so that the device can return data
                    fnUSBHostModel((ucToken & 0x0f), IN_PID, 0, 0);      // let the host model prepare the data
                }
                else if (ucToken != 0) {                                 // an OUT_PID is never expected here since the buffer descrptior would be owned if it were set as token
                    _EXCEPTION("Unexpected token");
                }
            }
    #endif
            break;
        }
        if (--iMaxUSB_ints == 0) {                                       // limit the number of interrupts handled in a single call (eg. to avoid isochronous INs causing an infinite loop)
            iMasks |= USB_INT;
            return;
        }
    } FOREVER_LOOP();
}

// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    #if defined USB_HS_INTERFACE                                         // {12}
    KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *ptrQueueHeader = (KINETIS_USBHS_ENDPOINT_QUEUE_HEADER *)USBHS_EPLISTADDR;
    ptrQueueHeader += (2 * iEndpoint);                                   // move to the receiver queue header
    return (unsigned short)(ptrQueueHeader->ulCapabilities >> ENDPOINT_QUEUE_HEADER_TOKEN_TOTAL_BYTES_SHIFT); // the endpoint reception buffer size
    #else
    KINETIS_USB_ENDPOINT_BD *ptrBDT = (KINETIS_USB_ENDPOINT_BD *)((BDT_PAGE_01 << 8) + (BDT_PAGE_02 << 16) + (BDT_PAGE_03 << 24));
    unsigned short usBufferSize;
    ptrBDT += iEndpoint;
    usBufferSize =  (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT); // get the length from the control register
    return usBufferSize;
    #endif
}
#endif

extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState)
{
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
//
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect
#if defined SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);
#endif
}

#if LPUARTS_AVAILABLE > 0
static void fnGenericLPUARTBreakHandling(KINETIS_LPUART_CONTROL *ptrLPUART, int iPort, int LPUART_irq_ID, void (*irq_LPUART)(void), int iUseIntMux, int iMuxChannel, int iInterruptAssignment)
{
    ptrLPUART->LPUART_STAT |= (LPUART_STAT_LBKDIF | LPUART_STAT_RDRF);   // set the status flags
    if ((ptrLPUART->LPUART_BAUD & LPUART_BAUD_LBKDIE) != 0) {            // if the break interrupt is enabled
        ptrLPUART->LPUART_DATA = 0;                                      // a break character is seen as a data reception of 0
        if (iUseIntMux != 0) {                                           // if the interrupt is assigned via the interrupt multiplexer
    #if defined INTMUX0_AVAILABLE
            fnCallINTMUX(iMuxChannel, iInterruptAssignment, (unsigned char *)&irq_LPUART);
    #else
            _EXCEPTION("Interrupt multiplexer not available!");
    #endif
        }
        else if (fnGenInt(LPUART_irq_ID) != 0) {                         // if LPUART interrupt is not disabled
            irq_LPUART();                                                // call the interrupt handler
        }
    }
    else {
        unsigned char ucBreakChar = 0;
        fnSimulateSerialIn(iPort, (unsigned char *)&ucBreakChar, 1);     // a break chactater is seen as a data reception of 0
    }
}
#endif
#if UARTS_AVAILABLE > 0
static void fnGenericUARTBreakHandling(KINETIS_UART_CONTROL *ptrUART, int iPort, int UART_irq_ID, void (*irq_UART)(void), int iUseIntMux, int iMuxChannel, int iInterruptAssignment)
{
    if ((ptrUART->UART_BDH & UART_BDH_LBKDIE) != 0) {                    // if break detection is enabled
        ptrUART->UART_S1 |= (UART_S1_FE | UART_S1_RDRF);                 // set framing error flag
    }
    if (((ptrUART->UART_S1 & UART_S1_FE) != 0) && ((ptrUART->UART_BDH & UART_BDH_LBKDIE) != 0)) { // if break detection interrupt is enabled
        ptrUART->UART_D = 0;                                             // a break character is seen as a data reception of 0
        if (fnGenInt(UART_irq_ID) != 0) {                                // if UART interrupt is not disabled
            irq_UART();                                                  // call the interrupt handler
        }
    }
    else {
        unsigned char ucBreakChar = 0;
        fnSimulateSerialIn(iPort, (unsigned char *)&ucBreakChar, 1);     // a break chactater is seen as a data reception of 0
    }
}
#endif

// UART Break detection simulation
//
extern void fnSimulateBreak(int iPort)
{
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    switch (iPort) {
#if LPUARTS_AVAILABLE > 0
    #if defined LPUARTS_PARALLEL
        #define LPUART0_CH_NUMBER     UARTS_AVAILABLE
    #else
        #define LPUART0_CH_NUMBER     0
    #endif
    case LPUART0_CH_NUMBER:                                              // LPUART 0
    #if defined irq_LPUART0_RX_ID
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART0_BLOCK, iPort, irq_LPUART0_RX_ID, ptrVect->processor_interrupts.irq_LPUART0_RX, 0, 0, 0);
    #else
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART0_BLOCK, iPort, irq_LPUART0_ID, ptrVect->processor_interrupts.irq_LPUART0, 0, 0, 0);
    #endif
        break;
#endif
#if LPUARTS_AVAILABLE > 1
    #if defined LPUARTS_PARALLEL
        #define LPUART1_CH_NUMBER     (UARTS_AVAILABLE + 1)
    #else
        #define LPUART1_CH_NUMBER     1
    #endif
    case LPUART1_CH_NUMBER:                                              // LPUART 1
    #if defined irq_LPUART1_RX_ID
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART1_BLOCK, iPort, irq_LPUART1_RX_ID, ptrVect->processor_interrupts.irq_LPUART1_RX, 0, 0, 0);
    #else
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART1_BLOCK, iPort, irq_LPUART1_ID, ptrVect->processor_interrupts.irq_LPUART1, 0, 0, 0);
    #endif
        break;
#endif
#if LPUARTS_AVAILABLE > 2
    #if defined LPUARTS_PARALLEL
        #define LPUART2_CH_NUMBER     (UARTS_AVAILABLE + 2)
    #else
        #define LPUART2_CH_NUMBER     2
    #endif
    case LPUART2_CH_NUMBER:                                              // LPUART 2
    #if !defined irq_LPUART2_ID && defined INTMUX0_AVAILABLE
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART2_BLOCK, iPort, (irq_INTMUX0_0_ID + INTMUX_LPUART2), ptrVect->processor_interrupts.irq_LPUART2, 1, INTMUX_LPUART2, INTMUX0_PERIPHERAL_LPUART2);
    #elif defined irq_LPUART2_RX_ID
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART2_BLOCK, iPort, irq_LPUART2_RX_ID, ptrVect->processor_interrupts.irq_LPUART2_RX, 0, 0, 0);
    #else
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART2_BLOCK, iPort, irq_LPUART2_ID, ptrVect->processor_interrupts.irq_LPUART2, 0, 0, 0);
    #endif
        break;
#endif
#if LPUARTS_AVAILABLE > 3
    #if defined LPUARTS_PARALLEL
        #define LPUART3_CH_NUMBER     (UARTS_AVAILABLE + 3)
    #else
        #define LPUART3_CH_NUMBER     3
    #endif
    case LPUART3_CH_NUMBER:                                              // LPUART 3
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART3_BLOCK, iPort, irq_LPUART3_ID, ptrVect->processor_interrupts.irq_LPUART3, 0, 0, 0);
        break;
#endif
#if LPUARTS_AVAILABLE > 4
    #if defined LPUARTS_PARALLEL
        #define LPUART4_CH_NUMBER     (UARTS_AVAILABLE + 4)
    #else
        #define LPUART4_CH_NUMBER     4
    #endif
    case LPUART4_CH_NUMBER:                                              // LPUART 4
        fnGenericLPUARTBreakHandling((KINETIS_LPUART_CONTROL *)LPUART4_BLOCK, iPort, irq_LPUART4_ID, ptrVect->processor_interrupts.irq_LPUART4, 0, 0, 0);
        break;
#endif
#if UARTS_AVAILABLE > 0
    #if defined UART0_BLOCK
    case 0:
        #if defined irq_UART0_1_ID                                       // when UARTs 0 and 1 share an interrupt
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART0_BLOCK, iPort, irq_UART0_1_ID, ptrVect->processor_interrupts.irq_UART0_1, 0, 0, 0);
        #else
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART0_BLOCK, iPort, irq_UART0_ID, ptrVect->processor_interrupts.irq_UART0, 0, 0, 0);
        #endif
        break;
    #endif
    #if UARTS_AVAILABLE > 1
    case 1:
        #if defined irq_UART0_1_ID                                       // when UARTs 0 and 1 share an interrupt
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART1_BLOCK, iPort, irq_UART0_1_ID, ptrVect->processor_interrupts.irq_UART0_1, 0, 0, 0);
        #else
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART1_BLOCK, iPort, irq_UART1_ID, ptrVect->processor_interrupts.irq_UART1, 0, 0, 0);
        #endif
        break;
    #endif
    #if UARTS_AVAILABLE > 2
    case 2:
        #if defined irq_UART2_3_ID                                       // when UARTs 2 and 3 share an interrupt
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART2_BLOCK, iPort, irq_UART2_3_ID, ptrVect->processor_interrupts.irq_UART2_3, 0, 0, 0);
        #else
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART2_BLOCK, iPort, irq_UART2_ID, ptrVect->processor_interrupts.irq_UART2, 0, 0, 0);
        #endif
        break;
    #endif
    #if UARTS_AVAILABLE > 3
    case 3:
        #if defined irq_UART2_3_ID                                       // when UARTs 2 and 3 share an interrupt
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART3_BLOCK, iPort, irq_UART2_3_ID, ptrVect->processor_interrupts.irq_UART2_3, 0, 0, 0);
        #else
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART3_BLOCK, iPort, irq_UART3_ID, ptrVect->processor_interrupts.irq_UART3, 0, 0, 0);
        #endif
        break;
    #endif
    #if UARTS_AVAILABLE > 4
    case 4:
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART4_BLOCK, iPort, irq_UART4_ID, ptrVect->processor_interrupts.irq_UART4, 0, 0, 0);
        break;
    #endif
    #if UARTS_AVAILABLE > 5
    case 5:
        fnGenericUARTBreakHandling((KINETIS_UART_CONTROL *)UART5_BLOCK, iPort, irq_UART5_ID, ptrVect->processor_interrupts.irq_UART5, 0, 0, 0);
        break;
    #endif
#endif
    }
}


// Process simulated DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    unsigned long ulNewActions = 0;
    unsigned long ulChannel = 0x00000001;
    unsigned long iChannel = 0;
    int _iDMA = iDMA;
    int iNonPeripheralDMA = iDMA;
    #if defined SERIAL_INTERFACE && defined SERIAL_SUPPORT_DMA
    int *ptrCnt;
    #endif

    while (_iDMA != 0) {                                                 // while DMA operations to be performed
        if ((_iDMA & ulChannel) != 0) {                                  // DMA request on this channel
            _iDMA &= ~ulChannel;                                         // this channel will have been handled after this loop
            switch (iChannel) {
    #if defined SERIAL_INTERFACE && defined SERIAL_SUPPORT_DMA           // {4}
            case DMA_UART0_TX_CHANNEL:                                   // handle UART DMA transmission on LPUART/UART 0
        #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
                if ((LPUART0_BAUD & LPUART_BAUD_TDMAE) != 0)             // if DMA operation is enabled
        #else
                if ((UART0_C5 & UART_C5_TDMAS) != 0)                     // if DMA operation is enabled
        #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART0];              // the number of characters in each tick period
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
        #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART0_TX) > 0)
        #else
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_UART0_TX) > 0)
        #endif
                            {                                            // process the trigger
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(0);                        // handle possible pending interrupt after DMA completion
                            }
        #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
	                        fnLogTx0((unsigned char)LPUART0_DATA);
        #else
	                        fnLogTx0(UART0_D);
        #endif
                            ulNewActions |= SEND_COM_0;
                        }
                    }
                }
                break;
        #if ((UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1)
            case DMA_UART1_TX_CHANNEL:                                   // handle UART DMA transmission on UART 1
            #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
                if ((LPUART1_BAUD & LPUART_BAUD_TDMAE) != 0)             // if DMA operation is enabled
            #elif defined KINETIS_KL
                if ((UART1_C4 & UART_C4_TDMAS) != 0)
            #else
                if ((UART1_C5 & UART_C5_TDMAS) != 0)
            #endif
                {                                                        // if DMA operation is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_UART1];              // maximum UART throughput during a tick interval
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
            #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART1_TX) > 0)
            #else
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_UART1_TX) > 0)
            #endif
                            {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(1);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
	                        fnLogTx1((unsigned char)LPUART1_DATA);
            #else
	                        fnLogTx1(UART1_D);
            #endif
                            ulNewActions |= SEND_COM_1;
                        }
                    }
                }
                break;
        #endif
        #if ((UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2)
            case DMA_UART2_TX_CHANNEL:                                   // handle UART DMA transmission on UART 2
            #if defined KINETIS_KL && (!defined K_STYLE_UART2 && !((LPUARTS_AVAILABLE == 3 && (UARTS_AVAILABLE == 0))))
                if ((UART2_C4 & UART_C4_TDMAS) != 0)
            #elif LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
                if ((LPUART2_BAUD & LPUART_BAUD_TDMAE) != 0)             // if DMA operation is enabled
            #else
                if ((UART2_C5 & UART_C5_TDMAS) != 0)
            #endif
                {                                                        // if DMA operation is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_UART2];
                    if (*ptrCnt) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
            #if LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART2_TX) > 0)
            #else
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_UART2_TX) > 0)
            #endif
                            {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(2);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
	                        fnLogTx2((unsigned char)LPUART2_DATA);
            #else
	                        fnLogTx2(UART2_D);
            #endif
                            ulNewActions |= SEND_COM_2;
                        }
                    }
                }
                break;
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case DMA_UART3_TX_CHANNEL:                                   // handle UART DMA transmission on UART 3
            #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
                if (LPUART3_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #elif UARTS_AVAILABLE == 3 && LPUARTS_AVAILABLE == 1 && defined LPUARTS_PARALLEL
                if (LPUART0_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #else
                if (UART3_C5 & UART_C5_TDMAS)                            // if DMA operation is enabled
            #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART3];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
            #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART3_TX) > 0)
            #elif UARTS_AVAILABLE == 3 && LPUARTS_AVAILABLE == 1 && defined LPUARTS_PARALLEL
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART0_TX) > 0)
            #else
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_UART3_TX) > 0)
            #endif
                            {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(3);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
                            fnLogTx3((unsigned char)LPUART3_DATA);
            #elif UARTS_AVAILABLE == 3 && LPUARTS_AVAILABLE == 1 && defined LPUARTS_PARALLEL
	                        fnLogTx3((unsigned char)LPUART0_DATA);
            #else
	                        fnLogTx3(UART3_D);
            #endif
                            ulNewActions |= SEND_COM_3;
                        }
                    }
                }
                break;
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case DMA_UART4_TX_CHANNEL:                                   // handle UART DMA transmission on UART 4
            #if LPUARTS_AVAILABLE > 4 && !defined LPUARTS_PARALLEL
                if (LPUART4_BAUD & LPUART_BAUD_TDMAE)                    // if DMA operation is enabled
            #else
                if (UART4_C5 & UART_C5_TDMAS)                            // if DMA operation is enabled
            #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART4];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
            #if LPUARTS_AVAILABLE > 4 && !defined LPUARTS_PARALLEL
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART4_TX) > 0)
            #else
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_UART4_TX) > 0)
            #endif
                            {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(4);                        // handle possible pending interrupt after DMA completion
                            }
            #if LPUARTS_AVAILABLE > 4 && !defined LPUARTS_PARALLEL
                            fnLogTx4((unsigned char)LPUART4_DATA);
            #else
	                        fnLogTx4(UART4_D);
            #endif
                            ulNewActions |= SEND_COM_4;
                        }
                    }
                }
                break;
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case DMA_UART5_TX_CHANNEL:                                   // handle UART DMA transmission on UART 5
            #if UARTS_AVAILABLE == 5
                if ((LPUART0_BAUD & LPUART_BAUD_TDMAE) != 0)             // if DMA operation is enabled
            #else
                if ((UART5_C5 & UART_C5_TDMAS) != 0)                     // if DMA operation is enabled
            #endif
                {
                    ptrCnt = (int *)argv[THROUGHPUT_UART5];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
            #if UARTS_AVAILABLE == 5
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPUART0_TX) > 0)
            #else
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_UART5_TX) > 0)
            #endif
                            {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                                fnUART_Tx_int(5);                        // handle possible pending interrupt after DMA completion
                            }
            #if UARTS_AVAILABLE == 5
                            fnLogTx5((unsigned char)LPUART0_DATA);
            #else
	                        fnLogTx5(UART5_D);
            #endif
                            ulNewActions |= SEND_COM_5;
                        }
                    }
                }
                break;
        #endif
    #endif
            default:                                                     // not a channel used by LPUART/UART
                break;
            }
    #if defined I2C_INTERFACE && LPI2C_AVAILABLE > 0 && defined I2C_DMA_SUPPORT
        #if LPI2C_AVAILABLE > 0
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C0_RX_CHANNEL)) { // handle I2C DMA reception on I2C0
                if ((LPI2C0_MDER & LPI2C_MDER_RDDE) != 0) {              // if reception DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C0];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            LPI2C0_MRDR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)(LPI2C0_MRDR));
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPI2C0_RX) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C0_TX_CHANNEL)) { // handle I2C DMA transmission on I2C0
                if ((LPI2C0_MDER & LPI2C_MDER_TDDE) != 0) {             // if transmission DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C0];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPI2C0_TX) > 0) {
                                fnSimI2C_devices(I2C_TX_DATA, (unsigned char)(LPI2C0_MTDR));
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
        #if LPI2C_AVAILABLE > 1
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C1_RX_CHANNEL)) { // handle I2C DMA reception on I2C1
                if ((LPI2C1_MDER & LPI2C_MDER_RDDE) != 0) {             // if reception DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C1];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            LPI2C1_MRDR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)(LPI2C1_MRDR));
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPI2C1_RX) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C1_TX_CHANNEL)) { // handle I2C DMA transmission on I2C1
                if ((LPI2C1_MDER & LPI2C_MDER_TDDE) != 0) {             // if transmission DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C1];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPI2C1_TX) > 0) {
                                fnSimI2C_devices(I2C_TX_DATA, (unsigned char)(LPI2C1_MTDR));
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
        #if LPI2C_AVAILABLE > 2
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C2_RX_CHANNEL)) { // handle I2C DMA reception on I2C2
                if ((LPI2C2_MDER & LPI2C_MDER_RDDE) != 0) {             // if reception DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C2];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            LPI2C2_MRDR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)(LPI2C2_MRDR));
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPI2C2_RX) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C2_TX_CHANNEL)) { // handle I2C DMA transmission on I2C2
                if ((LPI2C2_MDER & LPI2C_MDER_TDDE) != 0) {             // if transmission DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C2];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, DMAMUX0_CHCFG_SOURCE_LPI2C2_TX) > 0) {
                                fnSimI2C_devices(I2C_TX_DATA, (unsigned char)(LPI2C2_MTDR));
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
    #endif
    #if defined I2C_INTERFACE && LPI2C_AVAILABLE == 0 && defined I2C_DMA_SUPPORT
        #if I2C_AVAILABLE > 0
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C0_RX_CHANNEL)) { // handle I2C DMA reception on I2C0
                if ((I2C0_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C0];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C0
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C0;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C0_3
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C0_3;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            I2C0_D = fnSimI2C_devices(I2C_RX_DATA, I2C0_D);
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C0_TX_CHANNEL)) { // handle I2C DMA transmission on I2C0
                if ((I2C0_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C0];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C0
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C0;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C0_3
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C0_3;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                I2C0_D = fnSimI2C_devices(I2C_TX_DATA, I2C0_D);
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
        #if I2C_AVAILABLE > 1
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C1_RX_CHANNEL)) { // handle I2C DMA reception on I2C1
                if ((I2C1_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C1];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C1
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C1;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C1_2
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C1_2;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            I2C1_D = fnSimI2C_devices(I2C_RX_DATA, I2C1_D);
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C1_TX_CHANNEL)) { // handle I2C DMA transmission on I2C1
                if ((I2C1_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C1];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C1
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C1;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C1_2
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C1_2;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                I2C1_D = fnSimI2C_devices(I2C_TX_DATA, I2C1_D);
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
        #if I2C_AVAILABLE > 2
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C2_RX_CHANNEL)) { // handle I2C DMA reception on I2C2
                if ((I2C2_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C2];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C2
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C2;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C1_2
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C1_2;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            I2C2_D = fnSimI2C_devices(I2C_RX_DATA, I2C2_D);
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C2_TX_CHANNEL)) { // handle I2C DMA transmission on I2C2
                if ((I2C2_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C2];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C2
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C2;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C1_2
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C1_2;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                I2C2_D = fnSimI2C_devices(I2C_TX_DATA, I2C2_D);
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
        #if I2C_AVAILABLE > 3
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C3_RX_CHANNEL)) { // handle I2C DMA reception on I2C3
                if ((I2C3_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C3];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C3
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C3;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C0_3
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C0_3;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            I2C3_D = fnSimI2C_devices(I2C_RX_DATA, I2C3_D);
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
            if (((iDMA & ulChannel) != 0) && (iChannel == DMA_I2C3_TX_CHANNEL)) { // handle I2C DMA transmission on I2C3
                if ((I2C3_C1 & (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) == (I2C_MTX | I2C_IIEN | I2C_IEN | I2C_DMAEN)) { // if DMA is enabled
                    ptrCnt = (int *)argv[THROUGHPUT_I2C3];
                    if (*ptrCnt != 0) {
                        if (--(*ptrCnt) == 0) {
                            iMasks |= ulChannel;                         // enough serial DMA transfers handled in this tick period
                        }
                        else {
            #if defined DMAMUX0_CHCFG_SOURCE_I2C3
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C3;
            #elif defined DMAMUX0_CHCFG_SOURCE_I2C0_3
                            unsigned char ucDMA_Trigger = DMAMUX0_CHCFG_SOURCE_I2C0_3;
            #endif
                            iDMA &= ~ulChannel;
                            iNonPeripheralDMA &= ~ulChannel;             // peripheral DMA handled
                            if (fnSimulateDMA(iChannel, ucDMA_Trigger) > 0) {
                                I2C3_D = fnSimI2C_devices(I2C_TX_DATA, I2C3_D);
                                iDMA |= ulChannel;                       // further DMA triggers
                            }
                            else {
                            }
                        }
                    }
                }
            }
        #endif
            if ((iNonPeripheralDMA & ulChannel & ~iMasks) != 0) {       // if not handled by a peripheral
                iDMA &= ~ulChannel;
        #if !defined KINETIS_KL || defined DEVICE_WITH_eDMA
                if (fnSimulateDMA(iChannel, 0) > 1) {                   // process the trigger
                    iDMA |= ulChannel;                                  // further DMA triggers
                }
        #endif
            }
    #endif
        }
        ulChannel <<= 1;
        iChannel++;
    }
    return ulNewActions;
}



extern void fnSimulateLinkUp(void)
{
#if defined ETH_INTERFACE
    #if defined PHY_INTERRUPT
    unsigned long ulBit = PHY_INTERRUPT;
    unsigned char ucPortBit = 0;
    while ((ulBit & 0x80000000) == 0) {
        ucPortBit++;
        ulBit <<= 1;
    }
    MMFR = PHY_LINK_UP_INT;
    fnSimulateInputChange(PHY_INTERRUPT_PORT, ucPortBit, CLEAR_INPUT);   // clear level sensitive interrupt input
    #endif
    fnUpdateIPConfig();                                                  // update display in simulator
#elif defined USB_CDC_RNDIS || defined USE_PPP
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}

#if (defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET)
extern void fec_txf_isr(void)
{
    EIR |= (TXF | TXB);                                                  // set frame and buffer interrupt events
    if ((EIMR & TXF) != 0) {                                             // if interrupt is enabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        ptrVect->processor_interrupts.irq_ETH_TX();                      // call the interrupt handler
	}
}
#endif

#if (defined SUPPORT_RTC && !defined KINETIS_WITHOUT_RTC) || defined SUPPORT_SW_RTC
#define NTP_TO_1970_TIME 2208988800u
#define LEAP_YEAR(year) ((year%4)==0)                                    // valid from 1970 to 2038
static const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
// Synchronise the internal RTC to the PC time when simulating
//
extern void fnInitInternalRTC(char *argv[])
{
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned short RTC_YEAR, RTC_MONTH, RTC_DOW, RTC_DOM, RTC_HOUR, RTC_MIN, RTC_SEC;
    unsigned long ulKinetisTime;
    unsigned long ulLeapYears = 1970;
    RTC_YEAR = *ptrShort++;
    RTC_MONTH = *ptrShort++;
    RTC_DOW = *ptrShort++;
    RTC_DOM = *ptrShort++;
    RTC_HOUR = *ptrShort++;
    RTC_MIN = *ptrShort++;
    RTC_SEC = *ptrShort++;

    ulKinetisTime = ((RTC_YEAR - 1970) * 365);                           // years since reference time, represented in days without respecting leap years
    while (ulLeapYears <= RTC_YEAR) {                                    // {11}
        if (LEAP_YEAR(ulLeapYears) != 0) {                               // count leap years
            if (ulLeapYears == RTC_YEAR) {                               // presently in a leap year
                if ((RTC_MONTH > 2) && (RTC_DOM > 28)) {                 // {27} past February 28 so count extra leap day in this year
                    ulKinetisTime++;
                }
            }
            else {
                ulKinetisTime++;                                         // count extra days in passed leap years
            }
        }
        ulLeapYears++;
    }
    while (--RTC_MONTH != 0) {
        ulKinetisTime += monthDays[RTC_MONTH - 1];                       // {27} add past days of previous months of this year
    }
    ulKinetisTime += (RTC_DOM - 1);                                      // add past number of days in present month
    ulKinetisTime *= 24;                                                 // convert days to hours
    ulKinetisTime += RTC_HOUR;                                           // add hours passed in present day
    ulKinetisTime *= 60;                                                 // convert hours to minutes
    ulKinetisTime += RTC_MIN;                                            // add minutes in present hour
    ulKinetisTime *= 60;                                                 // convert minutes to seconds
    ulKinetisTime += RTC_SEC;                                            // add seconds in present minute
    #if !defined KINETIS_WITHOUT_RTC && !(defined KINETIS_KE && !defined KINETIS_WITH_SRTC)
        #if defined KINETIS_KL && defined RTC_USES_LPO_1kHz
    RTC_TSR = ulKinetisTime;                                             // set the initial seconds count value (since 1970)
    *RTC_SECONDS_LOCATION = ulKinetisTime;                               // set time information to non-initialised ram
    *RTC_PRESCALER_LOCATION = 0;
    *RTC_ALARM_LOCATION = 0;
    *RTC_VALID_LOCATION = RTC_VALID_PATTERN;                             // pass the time using variables
        #else
    RTC_TSR = ulKinetisTime;                                             // set the initial seconds count value (since 1970)
    RTC_SR = 0;
        #endif
    #else
    *RTC_SECONDS_LOCATION = ulKinetisTime;                               // set time information to non-initialised ram
    *RTC_PRESCALER_LOCATION = 0;
    *RTC_ALARM_LOCATION = 0;
    *RTC_VALID_LOCATION = RTC_VALID_PATTERN;
    #endif
}
#endif

#if defined SUPPORT_ADC                                                  // {2}
static void fnTriggerADC(int iADC, int iHW_trigger)
{
    switch (iADC) {
    case 0:                                                              // ADC0
        if (IS_POWERED_UP(6, ADC0) && ((ADC0_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC0 powered up and operating
            if ((iHW_trigger != 0) || ((ADC0_SC2 & ADC_SC2_ADTRG_HW) == 0)) { // hardware or software trigger
                fnSimADC(0);                                             // perform ADC conversion
                if ((ADC0_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
    #if !defined DEVICE_WITHOUT_DMA
                    fnHandleDMA_triggers(DMAMUX0_CHCFG_SOURCE_ADC0, 0);  // handle DMA triggered on ADC0 conversion
    #endif
                    if ((ADC0_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
    #if defined irq_ADCA_ID
                        if (fnGenInt(irq_ADCA_ID) != 0) {                // if ADCA interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADCA();    // call the interrupt handler
                        }
    #else
                        if (fnGenInt(irq_ADC0_ID) != 0) {                // if ADC0 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC0();    // call the interrupt handler
                        }
    #endif
                    }
                }
            }
        }
        break;
    #if ADC_CONTROLLERS > 1
    case 1:                                                              // ADC1
        #if defined KINETIS_K22_SF7
        if (IS_POWERED_UP(6, ADC1) && ((ADC1_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF))
        #else
        if (IS_POWERED_UP(3, ADC1) && ((ADC1_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF))
        #endif
        {                                                                // ADC1 powered up and operating
            if ((iHW_trigger != 0) || ((ADC1_SC2 & ADC_SC2_ADTRG_HW) == 0)) { // hardware or software trigger mode
                fnSimADC(1);                                             // perform ADC conversion
                if ((ADC1_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
                    fnHandleDMA_triggers(DMAMUX0_CHCFG_SOURCE_ADC1, 0);  // handle DMA triggered on ADC1 conversion
                    if ((ADC1_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
        #if defined irq_ADCB_ID
                        if (fnGenInt(irq_ADCB_ID) != 0) {                // if ADCB interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADCB();    // call the interrupt handler
                        }
        #else
                        if (fnGenInt(irq_ADC1_ID) != 0) {                // if ADC1 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC1();    // call the interrupt handler
                        }
        #endif
                    }
                }
            }
        }
        break;
    #endif
    #if ADC_CONTROLLERS > 2
    case 2:                                                              // ADC2
        if ((IS_POWERED_UP(6, ADC2)) && ((ADC2_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC2 powered up and operating
            if ((iHW_trigger != 0) || ((ADC2_SC2 & ADC_SC2_ADTRG_HW) == 0)) { // hardware or software trigger mode
                fnSimADC(2);                                             // perform ADC conversion
                if ((ADC2_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
        #if !defined KINETIS_KE18                                        // to be done
                    fnHandleDMA_triggers(DMAMUX1_CHCFG_SOURCE_ADC2, 1);  // handle DMA triggered on ADC2 conversion
                    if ((ADC2_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
                        if (fnGenInt(irq_ADC2_ID) != 0) {                // if ADC2 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC2();    // call the interrupt handler
                        }
                    }
        #endif
                }
            }
        }
        break;
    #endif
    #if ADC_CONTROLLERS > 3
    case 3:                                                              // ADC3
        if ((IS_POWERED_UP(3, ADC3)) && ((ADC3_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF)) { // ADC3 powered up and operating
            if ((iHW_trigger != 0) || ((ADC3_SC2 & ADC_SC2_ADTRG_HW) == 0)) { // hardware or software trigger mode
                fnSimADC(3);                                             // perform ADC conversion
                if ((ADC3_SC1A & ADC_SC1A_COCO) != 0) {                  // {40} if conversion has completed
                    fnHandleDMA_triggers(DMAMUX1_CHCFG_SOURCE_ADC3, 1);  // handle DMA triggered on ADC3 conversion
                    if ((ADC3_SC1A & ADC_SC1A_AIEN) != 0) {              // end of conversion interrupt enabled
                        if (fnGenInt(irq_ADC3_ID) != 0) {                // if ADC3 interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_ADC3();    // call the interrupt handler
                        }
                    }
                }
            }
        }
        break;
    #endif
    }
}
#endif


// We can simulate timers during a tick here if required
//
extern int fnSimTimers(void)
{
#if defined SERIAL_INTERFACE
    int iUART = 0;
#endif
#if PDB_AVAILABLE > 0                                                    // {24}
    static int iPDB = 0;
    static int iPDB_interrupt_triggered = 0;
    static int iPDB_ch0_0_triggered = 0;
    static int iPDB_ch0_1_triggered = 0;
    static int iPDB_ch1_0_triggered = 0;
    static int iPDB_ch1_1_triggered = 0;
#endif
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;

#if defined CAN_INTERFACE && defined SIM_KOMODO
    fnSimCAN(0, 0, CAN_SIM_CHECK_RX);                                    // poll the CAN interface at the tick rate
#endif

    if ((APPLICATION_INT_RESET_CTR_REG & SYSRESETREQ) != 0) {
        return RESET_SIM_CARD;                                           // commanded reset
    }
    // Watchdog
    //
#if defined KINETIS_WITH_WDOG32
    if ((WDOG0_CS & WDOG_CS_EN) != 0) {                                  // if the watchdog is enabled
    #if TICK_RESOLUTION >= 1000
        #if defined KINETIS_KE15 || defined KINETIS_KE18
        unsigned long ulCounter = ((128 * TICK_RESOLUTION)/1000);        // assume 128000Hz LPO clock
        #else
        unsigned long ulCounter = (TICK_RESOLUTION / 1000);              // assume 1000Hz LPO clock
        #endif
    #else
        #if defined KINETIS_KE15 || defined KINETIS_KE18
        unsigned long ulCounter = 128;                                   // assume 128000Hz LPO clock
        #else
        unsigned long ulCounter = 1;                                     // assume 1000Hz LPO clock
        #endif
    #endif
        unsigned long ulWdogCnt = WDOG0_CNT;                             // present watchdog count value
        unsigned long ulWdogTimeout = WDOG0_TOVAL;                       // timeout value
        if ((WDOG0_CS & WDOG_CS_PRES_256) != 0) {                        // if the fixed 256 prescaler is enabled
            ulCounter /= 256;
        }
        ulWdogCnt += ulCounter;                                          // next value
        if (ulWdogCnt >= ulWdogTimeout) {
            return RESET_CARD_WATCHDOG;                                  // watchdog reset
        }
        WDOG0_CNT = (unsigned short)ulWdogCnt;                           // new watchdog count value
    }
#elif (defined KINETIS_KL && !defined KINETIS_KL82) || defined KINETIS_KW4X // {24}
    if ((SIM_COPC & SIM_COPC_COPT_LONGEST) != SIM_COPC_COPT_DISABLED) {  // check only when COP is enabled 
        if (SIM_SRVCOP == SIM_SRVCOP_2) {                                // assume retriggered
            ulCOPcounter = 0;
        }
        else {
            unsigned long ulCOP_trigger = 0;
            if ((SIM_COPC & SIM_COPC_COPCLKS_BUS) != 0) {                // COP clocked from bus clock
                ulCOPcounter += (unsigned long)(((unsigned long long)BUS_CLOCK * (unsigned long long)TICK_RESOLUTION)/1000000); // clocks in a tick period
            }
            else {                                                       // COP clocked from 1kHz clock
    #if TICK_RESOLUTION >= 1000
                ulCOPcounter += (TICK_RESOLUTION/1000);
    #else
                ulCOPcounter++;
    #endif
            }
            switch (SIM_COPC & SIM_COPC_COPT_LONGEST) {                  // COP mode
            case SIM_COPC_COPT_SHORTEST:
                if (SIM_COPC & SIM_COPC_COPCLKS_BUS) {                   // COP clocked from bus clock
                    ulCOP_trigger = (2 << (13 - 1));
                }
                else {                                                   // COP clocked from 1kHz clock
                    ulCOP_trigger = (2 << (5 - 1));
                }
                break;
            case SIM_COPC_COPT_MEDIUM:
                if (SIM_COPC & SIM_COPC_COPCLKS_BUS) {                   // COP clocked from bus clock
                    ulCOP_trigger = (2 << (16 - 1));
                }
                else {                                                   // COP clocked from 1kHz clock
                    ulCOP_trigger = (2 << (8 - 1));
                }
                break;
            case SIM_COPC_COPT_LONGEST:
                if (SIM_COPC & SIM_COPC_COPCLKS_BUS) {                   // COP clocked from bus clock
                    ulCOP_trigger = (2 << (18 - 1));
                }
                else {                                                   // COP clocked from 1kHz clock
                    ulCOP_trigger = (2 << (10 - 1));
                }
                break;
            }
            if (ulCOPcounter >= ulCOP_trigger) {
                return RESET_CARD_WATCHDOG;                              // watchdog reset
            }
        }
        SIM_SRVCOP = SIM_SRVCOP_1;                                       // reset to detect next retrigger
    }
#elif defined KINETIS_KE
    if ((WDOG_CS1 & WDOG_CS1_EN) != 0) {
    #if TICK_RESOLUTION >= 1000
        unsigned long ulCounter = (TICK_RESOLUTION/1000);                // assume 1000Hz LPO clock
    #else
        unsigned long ulCounter = 1;                                     // assume 1000Hz LPO clock
    #endif
        unsigned long ulWdogCnt = ((WDOG_CNTH << 8) | WDOG_CNTL);        // present watchdog count value
        unsigned long ulWdogTimeout = ((WDOG_TOVALH << 8) | WDOG_TOVALL);// timeout value
        ulWdogCnt += ulCounter;                                          // next value
        if (ulWdogCnt >= ulWdogTimeout) {
            if ((WDOG_CS1 & WDOG_CS1_INT) != 0) {                        // if an interrupt is enabled we first call it
                WDOG_CS2 |= WDOG_CS2_FLG;                                // set iterrupt flag
                if (fnGenInt(irq_WDOG_ID) != 0) {                        // if watchdog interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_WDOG0();           // call the interrupt handler (the wathdog unconditionally resets after 128 bus clocks)
                }
            }
            return RESET_CARD_WATCHDOG;                                  // watchdog reset
        }
        WDOG_CNTH = (unsigned char)(ulWdogCnt >> 8);                     // new watchdog count value
        WDOG_CNTL = (unsigned char)(ulWdogCnt);
    }
#else
    if ((WDOG_STCTRLH & WDOG_STCTRLH_WDOGEN) != 0) {                     // watchdog enabled
    #if TICK_RESOLUTION >= 1000
        unsigned long ulCounter = (TICK_RESOLUTION/1000);                // {28} assume 1000Hz LPO clock
    #else
        unsigned long ulCounter = 1;                                     // assume 1000Hz LPO clock
    #endif
        unsigned long ulWatchdogCount = ((WDOG_TMROUTH << 16) | (WDOG_TMROUTL)); // present watchdog count
        unsigned long ulWatchdogTimeout = ((WDOG_TOVALH << 16) | (WDOG_TOVALL)); // watchdog timeout value        
        if ((WDOG_STCTRLH & WDOG_STCTRLH_CLKSRC) != 0) {                 // not sure which source is which at the moment
        }
        else {
            ulCounter /= (((WDOG_PRESC >> 8) & 0x7) + 1);                // {28} respect LPO clock presecaler
        }        
        if ((ulWatchdogCount + ulCounter) >= ulWatchdogTimeout) {
            if ((WDOG_STCTRLH & WDOG_STCTRLH_IRQRSTEN) != 0) {           // if an interrupt is enabled we first call it
                WDOG_STCTRLL |= WDOG_STCTRLL_INTFLG;
    #if !defined irq_WDOG_ID && defined INTMUX0_AVAILABLE
                if (fnGenInt(irq_INTMUX0_0_ID) != 0)                     // {46}
    #else
                if (fnGenInt(irq_WDOG_ID) != 0)
    #endif
                {                                                        // if watchdog interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    #if !defined irq_WDOG_ID
                    fnCallINTMUX(INTMUX_WDOG0, INTMUX0_PERIPHERAL_WDOG0_EWM, (unsigned char *)&ptrVect->processor_interrupts.irq_WDOG0);
    #else
                    ptrVect->processor_interrupts.irq_WDOG0();           // call the interrupt handler
    #endif
                }
            }
            return RESET_CARD_WATCHDOG;                                  // watchdog reset
        }
        else {
            ulWatchdogCount += ulCounter;
            WDOG_TMROUTH = (unsigned short)(ulWatchdogCount >> 16);
            WDOG_TMROUTL = (unsigned short)(ulWatchdogCount);
        }
    }
#endif
    // SysTick
    //
    if ((SYSTICK_CSR & SYSTICK_ENABLE) != 0) {                           // SysTick is enabled
        unsigned long ulTickCount = 0;
        if ((SYSTICK_CSR & SYSTICK_CORE_CLOCK) != 0) {
            ulTickCount = (unsigned long)((unsigned long long)((unsigned long long)TICK_RESOLUTION * (unsigned long long)SYSTEM_CLOCK)/1000000); // count per tick period from internal clock
        }
        if (ulTickCount < SYSTICK_CURRENT) {
            SYSTICK_CURRENT -= ulTickCount;
        }
        else {
            SYSTICK_CURRENT = SYSTICK_RELOAD;
            if ((SYSTICK_CSR & SYSTICK_TICKINT) != 0) {                  // if interrupt enabled
                INT_CONT_STATE_REG |= PENDSTSET;                         // set the systick as pending
#if defined RUN_IN_FREE_RTOS
                fnExecutePendingInterrupts(0);
#else
                if ((kinetis.CORTEX_M4_REGS.ulPRIMASK & INTERRUPT_MASKED) == 0) { // if global interrupts have been enabled, call interrupt handler
                    ptrVect->ptrSysTick();                               // call the systick interrupt service routine
                }
#endif
            }
        }
    }
    // PIT
    //
#if !defined KINETIS_WITHOUT_PIT
    #if defined LPITS_AVAILABLE                                          // {47}
    if (((PCC_LPIT0 & PCC_CGC) != 0) && ((LPIT0_MCR & (LPIT_MCR_M_CEN | LPIT_MCR_SW_RST)) == LPIT_MCR_M_CEN)) { // if the LPIT is enabled and not in reset state
        unsigned long ulCount;
        switch (PCC_LPIT0 & PCC_PCS_MASK) {
        case PCC_PCS_SCGFIRCLK:
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)FIRC_CLK) / 1000000); // count in a tick period
            break;
        case PCC_PCS_OSCCLK:
        case PCC_PCS_SCGIRCLK:
        case PCC_PCS_SCGPCLK:
            _EXCEPTION("To do!");
            break;
        case PCC_PCS_CLOCK_OFF:
        default:
            _EXCEPTION("No LPIT clock!");
            break;

        }
        if ((LPIT0_TCTRL0 & LPIT_TCTRL_T_EN) != 0) {                     // if channel 0 is enabled
            if (LPIT0_CVAL0 <= ulCount) {
                ulCount -= LPIT0_CVAL0;
                LPIT0_CVAL0 = LPIT0_TVAL0;                               // reload
                if (ulCount < LPIT0_TVAL0) {
                    LPIT0_CVAL0 -= ulCount;
                }
                LPIT0_MSR |= LPIT_MSR_TIF0;                              // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT0, 0); // handle DMA triggered on LPIT0
                if ((LPIT0_MIER & LPIT_MIER_TIE0) != 0) {                // if PIT interrupt is enabled
        #if defined irq_LPIT0_CH0_ID
                    if (fnGenInt(irq_LPIT0_CH0_ID) != 0) {               // if PIT channel interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0_CH0();   // call the interrupt handler
                    }
        #else
                    if (fnGenInt(irq_LPIT0_ID) != 0) {                   // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0();       // call the shared interrupt handler
                    }
        #endif
                }
            }
            else {
                LPIT0_CVAL0 -= ulCount;
            }
        }
        if ((LPIT0_TCTRL1 & LPIT_TCTRL_T_EN) != 0) {                     // if channel 1 is enabled
            if (LPIT0_CVAL1 <= ulCount) {
                ulCount -= LPIT0_CVAL1;
                LPIT0_CVAL1 = LPIT0_TVAL1;                               // reload
                if (ulCount < LPIT0_TVAL1) {
                    LPIT0_CVAL1 -= ulCount;
                }
                LPIT0_MSR |= LPIT_MSR_TIF1;                              // flag that a reload occurred
        #if !defined KINETIS_KL82                                        // not yet supported
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT1, 0); // handle DMA triggered on LPIT1
        #endif
                if ((LPIT0_MIER & LPIT_MIER_TIE1) != 0) {                // if PIT interrupt is enabled
        #if defined irq_LPIT0_CH1_ID
                    if (fnGenInt(irq_LPIT0_CH1_ID) != 0) {               // if PIT channel interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0_CH1();   // call the interrupt handler
                    }
        #else
                    if (fnGenInt(irq_LPIT0_ID) != 0) {                   // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0();       // call the shared interrupt handler
                    }
        #endif
                }
            }
            else {
                LPIT0_CVAL1 -= ulCount;
            }
        }
        if ((LPIT0_TCTRL2 & LPIT_TCTRL_T_EN) != 0) {                     // if channel 2 is enabled
            if (LPIT0_CVAL2 <= ulCount) {
                ulCount -= LPIT0_CVAL2;
                LPIT0_CVAL2 = LPIT0_TVAL2;                               // reload
                if (ulCount < LPIT0_TVAL2) {
                    LPIT0_CVAL2 -= ulCount;
                }
                LPIT0_MSR |= LPIT_MSR_TIF2;                              // flag that a reload occurred
        #if !defined KINETIS_KL82                                        // not yet supported
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT2, 0); // handle DMA triggered on LPIT2
        #endif
                if ((LPIT0_MIER & LPIT_MIER_TIE2) != 0) {                // if PIT interrupt is enabled
        #if defined irq_LPIT0_CH2_ID
                    if (fnGenInt(irq_LPIT0_CH2_ID) != 0) {               // if PIT channel interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0_CH2();   // call the interrupt handler
                    }
        #else
                    if (fnGenInt(irq_LPIT0_ID) != 0) {                   // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0();       // call the shared interrupt handler
                    }
        #endif
                }
            }
            else {
                LPIT0_CVAL2 -= ulCount;
            }
        }
        if ((LPIT0_TCTRL3 & LPIT_TCTRL_T_EN) != 0) {                     // if channel 3 is enabled
            if (LPIT0_CVAL3 <= ulCount) {
                ulCount -= LPIT0_CVAL3;
                LPIT0_CVAL3 = LPIT0_TVAL3;                               // reload
                if (ulCount < LPIT0_TVAL3) {
                    LPIT0_CVAL3 -= ulCount;
                }
                LPIT0_MSR |= LPIT_MSR_TIF3;                              // flag that a reload occurred
        #if !defined KINETIS_KL82                                        // not yet supported
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT3, 0); // handle DMA triggered on LPIT3
        #endif
                if ((LPIT0_MIER & LPIT_MIER_TIE3) != 0) {                // if PIT interrupt is enabled
        #if defined irq_LPIT0_CH3_ID
                    if (fnGenInt(irq_LPIT0_CH3_ID) != 0) {               // if PIT channel interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0_CH3();   // call the interrupt handler
                    }
        #else
                    if (fnGenInt(irq_LPIT0_ID) != 0) {                   // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPIT0();       // call the shared interrupt handler
                    }
        #endif
                }
            }
            else {
                LPIT0_CVAL3 -= ulCount;
            }
        }
    }
    #else
    if ((PIT_MCR & PIT_MCR_MDIS) == 0) {                                 // if PIT module is not disabled
        if ((PIT_TCTRL0 & PIT_TCTRL_TEN) != 0) {                         // if PIT0 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL0 <= ulCount) {
                ulCount -= PIT_CVAL0;
                if (ulCount >= PIT_LDVAL0) {
                    ulCount = 0;
                }
                PIT_CVAL0 = PIT_LDVAL0;                                  // reload
                PIT_CVAL0 -= ulCount;
                PIT_TFLG0 = PIT_TFLG_TIF;                                // flag that a reload occurred
    #if !defined KINETIS_KM && !defined DEVICE_WITHOUT_DMA
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT0, 0); // handle DMA triggered on PIT0
    #endif
    #if defined KINETIS_KE
                if ((SIM_SOPT0 & SIM_SOPT_ADHWT_PIT0) != 0) {            // if PIT0 overflow is programmed to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                }
    #elif !defined KINETIS_WITH_PCC
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT0:                          // if PIT0 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
    #endif
                if ((PIT_TCTRL0 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
    #if defined KINETIS_KL || defined KINETIS_KW4X || defined KINETIS_KM // {24}
                    if (fnGenInt(irq_PIT_ID) != 0) {                     // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT();         // call the shared interrupt handler
                    }
    #else
                    if (fnGenInt(irq_PIT0_ID) != 0) {                    // if PIT0 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT0();        // call the interrupt handler
                    }
    #endif
                }
            }
            else {
                PIT_CVAL0 -= ulCount;
            }
        }
        if ((PIT_TCTRL1 & PIT_TCTRL_TEN) != 0) {                         // if PIT1 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL1 <= ulCount) {
                ulCount -= PIT_CVAL1;
                if (ulCount >= PIT_LDVAL1) {
                    ulCount = 0;
                }
                PIT_CVAL1 = PIT_LDVAL1;                                  // reload
                PIT_CVAL1 -= ulCount;
                PIT_TFLG1 = PIT_TFLG_TIF;                                // flag that a reload occurred
    #if !defined DEVICE_WITHOUT_DMA && !defined KINETIS_KL28 && !defined KINETIS_KL82 && !defined KINETIS_KM // not yet supported
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT1, 0); // handle DMA triggered on PIT1
    #endif
    #if !defined KINETIS_KE && !defined KINETIS_WITH_PCC
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT1:                          // if PIT1 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
    #endif
                if ((PIT_TCTRL1 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
    #if defined KINETIS_KL || defined KINETIS_KW4X || defined KINETIS_KM // {24}
                    if (fnGenInt(irq_PIT_ID) != 0) {                     // if general PIT interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT();         // call the shared interrupt handler
                    }
    #else
                    if (fnGenInt(irq_PIT1_ID) != 0) {                    // if PIT1 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT1();        // call the interrupt handler
                    }
    #endif
                }
            }
            else {
                PIT_CVAL1 -= ulCount;
            }
        }
    #if !defined KINETIS_KL && !defined KINETIS_KW4X && !defined KINETIS_KE && !defined KINETIS_KM // {24}
        if ((PIT_TCTRL2 & PIT_TCTRL_TEN) != 0) {                         // if PIT2 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL2 <= ulCount) {
                ulCount -= PIT_CVAL2;
                if (ulCount >= PIT_LDVAL2) {
                    ulCount = 0;
                }
                PIT_CVAL2 = PIT_LDVAL2;                                  // reload
                PIT_CVAL2 -= ulCount;
                PIT_TFLG2 = PIT_TFLG_TIF;                                // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT2, 0); // handle DMA triggered on PIT2
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT2:                          // if PIT2 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
                if ((PIT_TCTRL2 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
                    if (fnGenInt(irq_PIT2_ID) != 0) {                    // if PIT2 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT2();        // call the interrupt handler
                    }
                }
            }
            else {
                PIT_CVAL2 -= ulCount;
            }
        }
        if ((PIT_TCTRL3 & PIT_TCTRL_TEN) != 0) {                         // if PIT3 is enabled
            unsigned long ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // count in a tick period
            if (PIT_CVAL3 <= ulCount) {
                ulCount -= PIT_CVAL3;
                if (ulCount >= PIT_LDVAL3) {
                    ulCount = 0;
                }
                PIT_CVAL3 = PIT_LDVAL3;                                  // reload
                PIT_CVAL3 -= ulCount;
                PIT_TFLG3 = PIT_TFLG_TIF;                                // flag that a reload occurred
                fnHandleDMA_triggers(DMAMUX0_DMA0_CHCFG_SOURCE_PIT3, 0); // handle DMA triggered on PIT3
                switch ((SIM_SOPT7 & SIM_SOPT7_ADC0TRGSEL_CMP3)) {
                case SIM_SOPT7_ADC0TRGSEL_PIT3:                          // if PIT3 is configured to trigger ADC0 conversion
        #if defined SUPPORT_ADC
                    fnTriggerADC(0, 1);
        #endif
                    break;
                }
                if ((PIT_TCTRL3 & PIT_TCTRL_TIE) != 0) {                 // if PIT interrupt is enabled
                    if (fnGenInt(irq_PIT3_ID) != 0) {                    // if PIT3 interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PIT3();        // call the interrupt handler
                    }
                }
            }
            else {
                PIT_CVAL3 -= ulCount;
            }
        }
    #endif
    }
    #endif
#endif
    // RTC
    //
#if defined SUPPORT_RTC && !defined KINETIS_WITHOUT_RTC && !(defined KINETIS_KE && !defined KINETIS_WITH_SRTC)
    if ((RTC_SR & RTC_SR_TCE) != 0) {                                    // RTC is enabled
        if ((RTC_SR & RTC_SR_TIF) == 0) {                                // if invalid flag not set
            unsigned long ulCounter;
    #if defined KINETIS_KE15 || defined KINETIS_KE18
            if ((RTC_CR & RTC_CR_LPOS_LPO) != 0) {
        #if TICK_RESOLUTION >= 1000
                ulCounter = ((TICK_RESOLUTION/1000));                    // approximately 1kHz clock pulses in a TICK period (128kHz LPO divided by 128)
        #else
                ulCounter = 1;
        #endif
            }
            else {
                switch (SIM_CHIPCTL & SIM_CHIPCTL_RTC32CLKSEL_MASK) {
                case SIM_CHIPCTL_RTC32CLKSEL_OSC32:                      // RTC32CLK input selected from OSC32 clock output
                case SIM_CHIPCTL_RTC32CLKSEL_RTC_CLKIN:                  // RTC32CLK input selected from RTC_CLKIN pin
                    ulCounter = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)32768) / 1000000); // 32kHz clock pulses in a TICK period
                    break;
                default:
                    _EXCEPTION("Invalid RTC clock selection!");
                    break;
                }
            }
    #else
            switch (SIM_SOPT1 & SIM_SOPT1_OSC32KSEL_MASK) {
            case SIM_SOPT1_OSC32KSEL_SYS_OSC:                            // 32kHz oscillator
                if ((RTC_CR & RTC_CR_OSCE) != 0) {                       // if oscillator is enabled
                    ulCounter = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)32768) / 1000000); // 32kHz clock pulses in a TICK period
                }
                else {
                    ulCounter = 0;                                       // no clock
                }
                break;
            case SIM_SOPT1_OSC32KSEL_LPO_1kHz:
        #if TICK_RESOLUTION >= 1000
                ulCounter = ((TICK_RESOLUTION/1000));                    // approximately 1kHz clock pulses in a TICK period
        #else
                ulCounter = 1;
        #endif
                break;
        #if defined KINETIS_KL && !defined KINETIS_WITH_RTC_CRYSTAL
            case SIM_SOPT1_OSC32KSEL_RTC_CLKIN:                          // 32kHz clock input assumed
        #else
            case SIM_SOPT1_OSC32KSEL_32k:
        #endif
                ulCounter = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)32768)/1000000); // 32kHz clock pulses in a TICK period
                break;
            }
    #endif
            RTC_TPR += ulCounter;
            if (RTC_TPR >= 32768) {
                RTC_TPR = (RTC_TPR - 32768);                             // handle second overflow
    #if defined irq_RTC_SECONDS_ID                                       // {30} if the RTC in the device has seconds interrupt capability
                if ((RTC_IER & RTC_IER_TSIE) != 0) {                     // if seconds interrupt is enabled
                    if (fnGenInt(irq_RTC_SECONDS_ID) != 0) {             // if RTC seconds interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_RTC_SECONDS(); // call the interrupt handler
                    }
                }
    #endif
                if (RTC_TAR == RTC_TSR) {                                // alarm match
                    RTC_TSR = (RTC_TSR + 1);
                    RTC_SR |= RTC_SR_TAF;
                    if ((RTC_IER & RTC_IER_TAIE) != 0) {                 // interrupt on alarm enabled
    #if !defined irq_RTC_ALARM_ID && defined INTMUX0_AVAILABLE
                        if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_RTC_ALARM) != 0) // {46}
    #else
                        if (fnGenInt(irq_RTC_ALARM_ID) != 0)            // if RTC interrupt is not disabled
    #endif
                        {
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    #if !defined irq_RTC_ALARM_ID && defined INTMUX0_AVAILABLE
                            fnCallINTMUX(INTMUX_RTC_ALARM, INTMUX0_PERIPHERAL_RTC_ALARM, (unsigned char *)&ptrVect->processor_interrupts.irq_RTC_Alarm);
    #else
                            ptrVect->processor_interrupts.irq_RTC_ALARM(); // call the interrupt handler
    #endif
                        }
                    }
                }
                else {
                    RTC_TSR = (RTC_TSR + 1);                             // increment the seconds count value
                }
            }
        }
    }
#endif
#if defined KINETIS_KE && !defined KINETIS_WITH_SRTC
    if ((SIM_SCGC & SIM_SCGC_RTC) != 0) {
        unsigned long ulCount = 0;
        switch (RTC_SC & RTC_SC_RTCLKS_BUS) {                            // RTC clock source
        case RTC_SC_RTCLKS_EXT:                                          // external clock
    #if !defined _EXTERNAL_CLOCK                                         // if no external clock is available the OSCERCLK is not valid
        #define _EXTERNAL_CLOCK 0
    #endif
            ulCount = (unsigned long)((((unsigned long long)TICK_RESOLUTION) * (unsigned long long)_EXTERNAL_CLOCK)/1000000); // prescaler count in tick interval
            switch (RTC_SC & RTC_SC_RTCPS_1000) {
            case RTC_SC_RTCPS_1:
                break;
            case RTC_SC_RTCPS_2:
                ulCount /= 2;
                break;
            case RTC_SC_RTCPS_4:
                ulCount /= 4;
                break;
            case RTC_SC_RTCPS_8:
                ulCount /= 8;
                break;
            case RTC_SC_RTCPS_16:
                ulCount /= 16;
                break;
            case RTC_SC_RTCPS_32:
                ulCount /= 32;
                break;
            case RTC_SC_RTCPS_64:
                ulCount /= 64;
                break;
            }
            break;
        case RTC_SC_RTCLKS_INT:                                          // internal 32kHz clock
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)ICSIRCLK)/1000000); // count value in a tick period
            switch (RTC_SC & RTC_SC_RTCPS_1000) {                        // apply prescaler
            case RTC_SC_RTCPS_1:
                break;
            case RTC_SC_RTCPS_2:
                ulCount /= 2;
                break;
            case RTC_SC_RTCPS_4:
                ulCount /= 4;
                break;
            case RTC_SC_RTCPS_8:
                ulCount /= 8;
                break;
            case RTC_SC_RTCPS_16:
                ulCount /= 16;
                break;
            case RTC_SC_RTCPS_32:
                ulCount /= 32;
                break;
            case RTC_SC_RTCPS_64:
                ulCount /= 64;
                break;
            }
            break;
        case RTC_SC_RTCLKS_1K:                                           // internal 1kHz clock
    #if TICK_RESOLUTION >= 1000
            ulCount = (TICK_RESOLUTION/1000);
    #else
            ulCount = 1;
    #endif
            switch (RTC_SC & RTC_SC_RTCPS_1000) {
            case RTC_SC_RTCPS_128:
                ulCount /= 128;
                break;
            case RTC_SC_RTCPS_256:
                ulCount /= 256;
                break;
            case RTC_SC_RTCPS_512:
                ulCount /= 512;
                break;
            case RTC_SC_RTCPS_1024:
                ulCount /= 1024;
                break;
            case RTC_SC_RTCPS_2048:
                ulCount /= 2048;
                break;
            case RTC_SC_RTCPS_100:
                ulCount /= 100;
                break;
            case RTC_SC_RTCPS_1000:
                ulCount /= 1000;
                break;
            }
            break;
        case RTC_SC_RTCLKS_BUS:                                          // bus clock
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000);
            switch (RTC_SC & RTC_SC_RTCPS_1000) {
            case RTC_SC_RTCPS_128:
                ulCount /= 128;
                break;
            case RTC_SC_RTCPS_256:
                ulCount /= 256;
                break;
            case RTC_SC_RTCPS_512:
                ulCount /= 512;
                break;
            case RTC_SC_RTCPS_1024:
                ulCount /= 1024;
                break;
            case RTC_SC_RTCPS_2048:
                ulCount /= 2048;
                break;
            case RTC_SC_RTCPS_100:
                ulCount /= 100;
                break;
            case RTC_SC_RTCPS_1000:
                ulCount /= 1000;
                break;
            }
            break;
        }
        ulCount += RTC_CNT;
        if (ulCount > RTC_MOD) {
            ulCount -= RTC_MOD;
            RTC_CNT = ulCount;
            if ((RTC_SC & RTC_SC_RTIE) != 0) {                           // if interrupt enabled
                if (fnGenInt(irq_RTC_OVERFLOW_ID) != 0) {                // if core interrupt interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_RTC_OVERFLOW();    // call the interrupt handler
                }
            }
        }
        else {
            RTC_CNT = ulCount;
        }
    }
#endif
#if defined SUPPORT_LPTMR && (LPTMR_AVAILABLE > 0)                       // {35}
    #if defined KINETIS_KM
    if (((IS_POWERED_UP(6, LPTMR0)) != 0) && ((LPTMR0_CSR & LPTMR_CSR_TEN) != 0))
    #else
    if (((IS_POWERED_UP(5, LPTMR0)) != 0) && ((LPTMR0_CSR & LPTMR_CSR_TEN) != 0))
    #endif
    {                                                                    // if the low power timer is enabled and running
        unsigned long ulCount = 0;                                       // count in a tick period
        switch (LPTMR0_PSR & LPTMR_PSR_PCS_OSC0ERCLK) {
        case LPTMR_PSR_PCS_LPO:                                          // LPO source
    #if TICK_RESOLUTION >= 1000
            ulCount = (TICK_RESOLUTION/1000);                            // counts in a tick interval
    #else
            ulCount = 1;
    #endif
            break;
    #if defined KINETIS_WITH_SCG
        case LPTMR_PSR_PCS_SIRCCLK:
            _EXCEPTION("To add");
            break;
    #else
        case LPTMR_PSR_PCS_MCGIRCLK:
            if ((MCG_C2 & MCG_C2_IRCS) != 0) {                           // using 4MHz RC
                ulCount = (TICK_RESOLUTION * (4000000/1000000));
            }
            else {
                ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)35000)/1000000); // using 35kHz RC
            }
            break;
    #endif
        case LPTMR_PSR_PCS_ERCLK32K:
            ulCount = ((TICK_RESOLUTION * 32768)/1000000);               // counts in a tick interval
            break;
        case LPTMR_PSR_PCS_OSC0ERCLK:
    #if defined _EXTERNAL_CLOCK
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)_EXTERNAL_CLOCK)/1000000); // external clocks in a tick period (assuming no pre-scaler)
    #else
            _EXCEPTION("no external clock defined so this selection should not be used");
    #endif
            break;
        }
        if ((LPTMR0_PSR & LPTMR_PSR_PBYP) == 0) {                        // if the prescaler bypass hasn't been disabled
            ulCount >>= (((LPTMR0_PSR & LPTMR_PSR_PRESCALE_MASK) >> LPTMR_PSR_PRESCALE_SHIFT) + 1);
        }
        if (LPTMR0_CNR <= LPTMR0_CMR) {                                  // timer count has not yet reached the match value
            ulCount = (LPTMR0_CNR + ulCount);                            // the next count value
            if (ulCount > LPTMR0_CMR) {
                if ((LPTMR0_CSR & LPTMR_CSR_TFC_FREERUN) == 0) {
                    ulCount = (ulCount - LPTMR0_CMR);
                    if (ulCount > LPTMR0_CMR) {
                        ulCount = LPTMR0_CMR;
                    }
                }
                if ((LPTMR0_CSR & LPTMR_CSR_TIE) != 0) {                 // if LPTMR interrupt is enabled
    #if defined irq_LPTMR_PWT_ID
                    if (fnGenInt(irq_LPTMR_PWT_ID) != 0) {                  // if LPTMR interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPTMR_PWT();      // call the interrupt handler
                    }
    #else
                    if (fnGenInt(irq_LPTMR0_ID) != 0) {                  // if LPTMR interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_LPTMR0();      // call the interrupt handler
                    }
    #endif
                }
            }
        }
        else {
            ulCount = (LPTMR0_CNR + ulCount);
        }
        if (ulCount > 0xffff) {
            ulCount = (ulCount - 0xffff);
        }
        LPTMR0_CNR = ulCount;
    }
    #if LPTMR_AVAILABLE > 1
        if (((IS_POWERED_UP(5, LPTMR1)) != 0) && ((LPTMR1_CSR & LPTMR_CSR_TEN) != 0)) { // if the low power timer is enabled and running
        unsigned long ulCount = 0;                                       // count in a tick period
        switch (LPTMR1_PSR & LPTMR_PSR_PCS_OSC0ERCLK) {
        case LPTMR_PSR_PCS_LPO:
    #if TICK_RESOLUTION >= 1000
            ulCount = (TICK_RESOLUTION/1000);                            // counts in a tick interval
    #else
            ulCount = 1;
    #endif
            break;
    #if defined KINETIS_WITH_SCG
        case LPTMR_PSR_PCS_SIRCCLK:
            _EXCEPTION("To add");
            break;
    #else
        case LPTMR_PSR_PCS_MCGIRCLK:
            if ((MCG_C2 & MCG_C2_IRCS) != 0) {
                ulCount = (TICK_RESOLUTION * (4000000/1000000));
            }
            else {
                ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)35000)/1000000);
            }
            break;
    #endif
        case LPTMR_PSR_PCS_ERCLK32K:
            ulCount = ((TICK_RESOLUTION * 32768)/1000000);               // counts in a tick interval
            break;
        case LPTMR_PSR_PCS_OSC0ERCLK:
    #if defined _EXTERNAL_CLOCK
            ulCount = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)_EXTERNAL_CLOCK)/1000000); // external clocks in a tick period (assuming no pre-scaler)
    #else
            _EXCEPTION("no external clock defined so this selection should not be used");
    #endif
            break;
        }
        if ((LPTMR1_PSR & LPTMR_PSR_PBYP) == 0) {                        // if the prescaler bypass hasn't been disabled
            ulCount >>= (((LPTMR1_PSR & LPTMR_PSR_PRESCALE_MASK) >> LPTMR_PSR_PRESCALE_SHIFT) + 1);
        }
        if (LPTMR1_CNR <= LPTMR1_CMR) {                                  // timer count has not yet reached the match value
            ulCount = (LPTMR1_CNR + ulCount);                            // the next count value
            if (ulCount > LPTMR1_CMR) {
                if ((LPTMR1_CSR & LPTMR_CSR_TFC_FREERUN) == 0) {
                    ulCount = (ulCount - LPTMR1_CMR);
                    if (ulCount > LPTMR1_CMR) {
                        ulCount = LPTMR1_CMR;
                    }
                }
                if ((LPTMR1_CSR & LPTMR_CSR_TIE) != 0) {                 // if LPTMR interrupt is enabled
        #if !defined irq_LPTMR1_ID && defined INTMUX0_AVAILABLE
                    if (fnGenInt(irq_INTMUX0_0_ID + INTMUX_LPTMR1) != 0)
        #else
                    if (fnGenInt(irq_LPTMR1_ID) != 0)
        #endif
                    {                                                    // if LPTMR interrupt is not disabled
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
        #if !defined irq_LPTMR1_ID
                        fnCallINTMUX(INTMUX_LPTMR1, INTMUX0_PERIPHERAL_LPTMR1, (unsigned char *)&ptrVect->processor_interrupts.irq_LPTMR1);
        #else
                        ptrVect->processor_interrupts.irq_LPTMR1();      // call the interrupt handler
        #endif
                    }
                }
            }
        }
        else {
            ulCount = (LPTMR1_CNR + ulCount);
        }
        if (ulCount > 0xffff) {
            ulCount = (ulCount - 0xffff);
        }
        LPTMR1_CNR = ulCount;
    }
    #endif
#endif
#if PDB_AVAILABLE > 0 && !defined KINETIS_KE15 && !defined KINETIS_KE18  // {24}
    if ((IS_POWERED_UP(6, PDB0)) && ((PDB0_SC & PDB_SC_PDBEN) != 0)) {   // {16} PDB powered and enabled
        if ((PDB0_SC & PDB_SC_TRGSEL_SW) == PDB_SC_TRGSEL_SW) {          // software triggered
            if ((PDB0_SC & PDB_SC_SWTRIG) != 0) {
                PDB0_SC &= ~(PDB_SC_SWTRIG);                             // clear the software trigger
                iPDB = 1;                                                // triggered and running
                PDB0_CNT = 0;
            }
        }
        if (iPDB != 0) {                                                 // if running
            unsigned long ulPDB_count = (unsigned long)(((unsigned long long)TICK_RESOLUTION * (unsigned long long)BUS_CLOCK)/1000000); // the count in a tick period (with no prescaer)
            switch (PDB0_SC & PDB_SC_MULT_40) {                          // respect the pre-scaler multiplier
            case PDB_SC_MULT_1:
                break;
            case PDB_SC_MULT_10:
                ulPDB_count /= 10;
                break;
            case PDB_SC_MULT_20:
                ulPDB_count /= 20;
                break;
            case PDB_SC_MULT_40:
                ulPDB_count /= 40;
                break;
            }
            switch (PDB0_SC & PDB_SC_PRESCALER_128) {                    // respect the pre-scaler
            case PDB_SC_PRESCALER_1:
                break;
            case PDB_SC_PRESCALER_2:
                ulPDB_count /= 2;
                break;
            case PDB_SC_PRESCALER_4:
                ulPDB_count /= 4;
                break;
            case PDB_SC_PRESCALER_8:
                ulPDB_count /= 8;
                break;
            case PDB_SC_PRESCALER_16:
                ulPDB_count /= 16;
                break;
            case PDB_SC_PRESCALER_32:
                ulPDB_count /= 32;
                break;
            case PDB_SC_PRESCALER_64:
                ulPDB_count /= 64;
                break;
            case PDB_SC_PRESCALER_128:
                ulPDB_count /= 128;
                break;
            }
            ulPDB_count += PDB0_CNT;                                     // new count value
            if ((iPDB_interrupt_triggered == 0) && (ulPDB_count >= (PDB0_IDLY & 0xffff))) { // interrupt trigger reached
                iPDB_interrupt_triggered = 1;
                if ((PDB0_SC & PDB_SC_PDBIE) != 0) {                     // if period interrupt is enabled
                    PDB0_SC |= PDB_SC_PDBIF;                             // set the interrupt flag
                    if (fnGenInt(irq_PDB0_ID) != 0) {
                        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                        ptrVect->processor_interrupts.irq_PDB0();        // call the interrupt handler
                    }
                }
            }
            if ((iPDB_ch0_0_triggered == 0) && (ulPDB_count >= (PDB0_CH0DLY0 & 0xffff))) { // channel 0 delay 0 trigger reached
                iPDB_ch0_0_triggered = 1;
                if ((PDB0_CH0C1 & PDB_C1_EN_0) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH0C1 & PDB_C1_TOS_0) != 0) {              // if ADC0 A trigger is enabled
                        // ADC0 A conversion is triggered
                        //
    #if defined SUPPORT_ADC
                        fnTriggerADC(0, 1);
    #endif
                    }
                }
            }
            if ((iPDB_ch0_1_triggered == 0) && (ulPDB_count >= (PDB0_CH0DLY1 & 0xffff))) { // channel 0 delay 1 trigger reached
                iPDB_ch0_1_triggered = 1;
                if ((PDB0_CH0C1 & PDB_C1_EN_1) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH0C1 & PDB_C1_TOS_1) != 0) {              // if ADC0 B trigger is enabled
                        // ADC0 B conversion is triggered
                        //
    #if defined SUPPORT_ADC
                        fnTriggerADC(0, 1);
    #endif
                    }
                }
            }
    #if ADC_CONTROLLERS > 1
            if ((iPDB_ch1_0_triggered == 0) && (ulPDB_count >= (PDB0_CH1DLY0 & 0xffff))) { // channel 1 delay 0 trigger reached
                iPDB_ch1_0_triggered = 1;
                if ((PDB0_CH1C1 & PDB_C1_EN_0) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH1C1 & PDB_C1_TOS_0) != 0) {              // if ADC1 A trigger is enabled
                        // ADC1 A conversion is triggered
                        //
        #if defined SUPPORT_ADC
                        fnTriggerADC(1, 1);
        #endif
                    }
                }
            }
            if ((iPDB_ch1_1_triggered == 0) && (ulPDB_count >= (PDB0_CH1DLY1 & 0xffff))) { // channel 1 delay 1 trigger reached
                iPDB_ch1_1_triggered = 1;
                if ((PDB0_CH1C1 & PDB_C1_EN_1) != 0) {                   // if pre-trigger enabled
                    if ((PDB0_CH1C1 & PDB_C1_TOS_1) != 0) {              // if ADC1 B trigger is enabled
                        // ADC1 B conversion is triggered
                        //
        #if defined SUPPORT_ADC
                        fnTriggerADC(1, 1);
        #endif
                    }
                }
            }
    #endif
            if (ulPDB_count >= (PDB0_MOD & 0xffff)) {                    // cycle complete
                iPDB_interrupt_triggered = 0;
                iPDB_ch0_0_triggered = 0;
                iPDB_ch0_1_triggered = 0;
                iPDB_ch1_0_triggered = 0;
                iPDB_ch1_1_triggered = 0;
                PDB0_CNT = 0;
                if ((PDB0_SC & PDB_SC_CONT) == 0) {                      // single shot
                    iPDB = 0;                                            // stop the operation
                }
            }
            else {
                PDB0_CNT = ulPDB_count;
            }
        }
    }
    else {
        iPDB = 0;
    }
#endif
#if defined SUPPORT_ADC                                                  // {2}
    fnTriggerADC(0, 0);                                                  // handle software triggered ADC0
    #if ADC_CONTROLLERS > 1
    fnTriggerADC(1, 0);                                                  // handle software triggered ADC1
    #endif
    #if ADC_CONTROLLERS > 2
    fnTriggerADC(2, 0);                                                  // handle software triggered ADC2
    #endif
    #if ADC_CONTROLLERS > 3
    fnTriggerADC(3, 0);                                                  // handle software triggered ADC3
    #endif
#endif
#if defined SUPPORT_TIMER && !defined KINETIS_KM                         // {29}
    if ((fnFlexTimerPowered(0) != 0) && ((FTM0_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the TPM/FlexTimer is powered and clocked
        #if defined KINETIS_KL || defined KINETIS_KE
        int iTPM_Type = 1;
        #else
        int iTPM_Type = 0;
        #endif
        int iTimerInterrupt = fnHandleFlexTimer(0, (FLEX_TIMER_MODULE *)FTM_BLOCK_0, FLEX_TIMERS_0_CHANNELS, iTPM_Type);
        if (iTimerInterrupt != 0) {
            fnExecuteTimerInterrupt(0);
        }
    }
    #if FLEX_TIMERS_AVAILABLE > 1
    if ((fnFlexTimerPowered(1) != 0) && ((FTM1_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the TPM/FlexTimer is powered and clocked
        #if defined KINETIS_KL || defined KINETIS_KE
        int iTPM_Type = 1;
        #else
        int iTPM_Type = 0;
        #endif
        int iTimerInterrupt = fnHandleFlexTimer(1, (FLEX_TIMER_MODULE *)FTM_BLOCK_1, FLEX_TIMERS_1_CHANNELS, iTPM_Type);
        if (iTimerInterrupt != 0) {
            fnExecuteTimerInterrupt(1);
        }
        #if defined KINETIS_KL && defined SUPPORT_ADC && !defined KINETIS_WITH_PCC
        // Check for ADC triggers
        //
        if ((SIM_SOPT7 & SIM_SOPT7_ADC0ALTTRGEN) == 0) {                 // if the default hardware trigger source is used
            if (FTM1_CNT >= FTM0_C0V) {                                  // TPM1 channel 0 can trigger ADC0 input A
                fnTriggerADC(0, 1);
            }
            if (FTM1_CNT >= FTM0_C1V) {                                  // TPM1 channel 1 can trigger ADC0 input B
                fnTriggerADC(0, 1);
            }
        }
        #endif
    }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
        #if defined KINETIS_KL || defined KINETIS_K22_SF7 || defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66
    if ((fnFlexTimerPowered(2) != 0) &&((FTM2_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) // if the timer/PWM module is powered and clocked
        #else
    if ((fnFlexTimerPowered(2) != 0) &&((FTM2_SC & FTM_SC_CLKS_EXT) != 0)) // if the FlexTimer 2 is powered and clocked
        #endif
    {
        #if defined KINETIS_KL || defined KINETIS_KE
        int iTPM_Type = 1;
        #else
        int iTPM_Type = 0;
        #endif
        int iTimerInterrupt = fnHandleFlexTimer(2, (FLEX_TIMER_MODULE *)FTM_BLOCK_2, FLEX_TIMERS_2_CHANNELS, iTPM_Type);
        if (iTimerInterrupt != 0) {
            fnExecuteTimerInterrupt(2);
        }
    }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3 && !defined KINETIS_KE18
    if ((fnFlexTimerPowered(3) != 0) && ((FTM3_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the FlexTimer 3 is powered and clocked
        #if defined KINETIS_KL || defined KINETIS_KE
        int iTPM_Type = 1;
        #else
        int iTPM_Type = 0;
        #endif
        int iTimerInterrupt = fnHandleFlexTimer(3, (FLEX_TIMER_MODULE *)FTM_BLOCK_3, FLEX_TIMERS_3_CHANNELS, iTPM_Type);
        if (iTimerInterrupt != 0) {
            fnExecuteTimerInterrupt(3);
        }
    }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE_TOO          // TPM1
    if ((fnFlexTimerPowered(4) != 0) && ((FTM4_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the FlexTimer 4 (TPM1) is powered and clocked
        int iTimerInterrupt = fnHandleFlexTimer(4, (FLEX_TIMER_MODULE *)FTM_BLOCK_4, FLEX_TIMERS_4_CHANNELS, 1);
        if (iTimerInterrupt != 0) {
            fnExecuteTimerInterrupt(4);
        }
    }
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5 && defined TPMS_AVAILABLE_TOO          // TPM2
    if ((fnFlexTimerPowered(5) != 0) && ((FTM5_SC & (FTM_SC_CLKS_EXT | FTM_SC_CLKS_SYS)) != 0)) { // if the FlexTimer 5 (TPM2) is powered and clocked
        int iTimerInterrupt = fnHandleFlexTimer(5, (FLEX_TIMER_MODULE *)FTM_BLOCK_5, FLEX_TIMERS_5_CHANNELS, 1);
        if (iTimerInterrupt != 0) {
            fnExecuteTimerInterrupt(5);
        }
    }
    #endif
#endif
#if defined SERIAL_INTERFACE
    // Idle line detection on UARTs
    //
    while (iUART < (LPUARTS_AVAILABLE + UARTS_AVAILABLE)) {
        if (iUART_rx_Active[iUART] != 0) {                               // if the UART's input has been active
            if (iUART_rx_Active[iUART] == 1) {                           // detect idle line
                fnSimulateSerialIn(iUART, 0, 0);                         // simulate idle line event
            }
            iUART_rx_Active[iUART]--;
        }
        iUART++;
    }
#endif
    return 0;
}

#if defined RUN_IN_FREE_RTOS
extern void fnExecutePendingInterrupts(int iRecursive)
{
    static int iExecuting = 0;
    static unsigned char ucPresentPriority = 255;                        // lowest priority that doesn't block anything
    unsigned char ucPreviousPriority = ucPresentPriority;
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    if (iExecuting != 0) {
        if (iRecursive == 0) {
            return;                                                      // ignore when called non-recursively when already executing
        }
    }
    iExecuting = 1;
    if ((kinetis.CORTEX_M4_REGS.ulPRIMASK & INTERRUPT_MASKED) != 0) {
        iExecuting = 0;
        return;                                                          // if the global interrupt is masked we quit
    }
    if ((INT_CONT_STATE_REG & PENDSTSET) != 0) {                         // systick is pending
        unsigned char ucPriority = (SYSTEM_HANDLER_12_15_PRIORITY_REGISTER >> (24 + __NVIC_PRIORITY_SHIFT)); // systick interrupt priority
        if (ucPriority < ucPresentPriority) {                            // check that the interrupt has adequate priority to be called
            ucPresentPriority = ucPriority;                              // set the new priority level
            ptrVect->ptrSysTick();                                       // call the systick interrupt service routine
            ucPresentPriority = ucPreviousPriority;
            fnExecutePendingInterrupts(1);                               // allow further pending interrupt to be executed if needed
        }
    }
    iExecuting = 0;
}
#endif

extern unsigned char *fnGetSimTxBufferAdd(void)
{
#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET
    return (fnGetTxBufferAdd(0));
#else
    return 0;
#endif
}


#if defined ETH_INTERFACE
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET // we feed frames in promiscuous mode and filter them according to their details
static KINETIS_FEC_BD *ptrSimRxBD = 0;
static int iFirstRec = 0;

extern void fnResetENET(void)                                            // {41}
{
    iFirstRec = 0;
}

static int fnCheckEthernet(unsigned char *ucData, unsigned short usLen, int iForce)
{
	unsigned char *ptrInput;
    unsigned short usCopyLen;
    unsigned short usFullLength;
    #if defined USE_MULTIPLE_BUFFERS
    unsigned short ucMaxRxBufLen = 256;
    #else
    unsigned short ucMaxRxBufLen = 0x600;
    #endif

    if ((iForce == 0) && (fnCheckEthernetMode(ucData, usLen) == 0)) {
        return 0;                                                        // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
    }

    if (iFirstRec == 0) {
        if ((ECR & ETHER_EN) == 0) {
            return 0;                                                    // ignore if the FEC has not yet been programmed
        }
        iFirstRec = 1;                                                   // we do this only once
        ptrSimRxBD = (KINETIS_FEC_BD *)ERDSR;                            // set to first BD
    }

    if ((ptrSimRxBD->usBDControl & EMPTY_BUFFER) == 0) {                 // {22} drop reception frame if there are no free buffer descriptors
        return 0;
    }

    usFullLength = usLen;

    while (usLen != 0) {
        ptrInput = ptrSimRxBD->ptrBD_Data;
        if (usLen > ucMaxRxBufLen) {
            ptrSimRxBD->usBDLength = ucMaxRxBufLen;
            usCopyLen = ucMaxRxBufLen;
        }
        else {
            usCopyLen = ptrSimRxBD->usBDLength = usLen;
            ptrSimRxBD->usBDLength = usFullLength;
            ptrSimRxBD->usBDControl |= LAST_IN_FRAME_RX;
        }
        usLen -= usCopyLen;
	    while (usCopyLen-- != 0) {
	        *ptrInput++ = *ucData++;                                     // put bytes in input buffer
	    }
        if (usLen == 0) {                                                // last buffer
            if ((RCR & CRCFWD) == 0) {                                   // if CRC stripping is not disabled
                ptrSimRxBD->usBDLength += 4;                             // add dummy CRC32 - this corrects the receive count and also tests that there is adequate buffer space
            }
            *ptrInput++ = 0x33;
            *ptrInput++ = 0x66;
            *ptrInput++ = 0xaa;
            *ptrInput   = 0x55;
        }
        ptrSimRxBD->usBDControl &= ~EMPTY_BUFFER;                        // mark that the buffer contains
        EIR |= RXF;                                                      // set receive frame interrupt event
        if (((EIMR & RXF) != 0) && (ptrSimRxBD->usBDControl & LAST_IN_FRAME_RX)) { // if interrupts are enabled
            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
            ptrVect->processor_interrupts.irq_ETH_RX();                  // call the interrupt handler
	    }
        if ((ptrSimRxBD->usBDControl & WRAP_BIT_RX) != 0) {
            ptrSimRxBD = (KINETIS_FEC_BD *)ERDSR;                        // set to first BD
        }
        else {
            ptrSimRxBD++;                                                // set to next RX BD
        }
    }
    return 1;
}
#endif




extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce)
{
    int iReturn = 1;
#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET // we feed frames in promiscuous mode and filter them according to their details
    iReturn = fnCheckEthernet(ucData, usLen, iForce);
#endif
#if defined ETH_INTERFACE && defined ENC424J600_INTERFACE
    iReturn &= ~(fnCheckENC424J600(ucData, usLen, iForce));
#endif
    return iReturn;
}


#if defined SUPPORT_KEY_SCAN

#if KEY_COLUMNS == 0
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
                if (iChange != 0)                                        // generally a key press is a '0' 
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
    #if defined KEY_ROWS
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

#if defined DEVICE_WITH_SLCD
static unsigned long lcd_wf3to0 = 0;
static unsigned long lcd_wf7to4 = 0;
static unsigned long lcd_wf11to8 = 0;
static unsigned long lcd_wf15to12 = 0;
static unsigned long lcd_wf19to16 = 0;
static unsigned long lcd_wf23to20 = 0;
static unsigned long lcd_wf27to24 = 0;
static unsigned long lcd_wf31to28 = 0;
static unsigned long lcd_wf35to32 = 0;
static unsigned long lcd_wf39to36 = 0;
static unsigned long lcd_wf43to40 = 0;
static unsigned long lcd_wf47to44 = 0;
static unsigned long lcd_wf51to48 = 0;
static unsigned long lcd_wf55to52 = 0;
static unsigned long lcd_wf59to56 = 0;
static unsigned long lcd_wf63to60 = 0;

// check for changes in SLCD segment registers and call display updates when necessary
//
extern void fnSimulateSLCD(void)
{
    #if defined SLCD_FILE
        #if defined KINETIS_KL || defined KINETIS_KL33 || defined KINETIS_KL43
    if (((SIM_SCGC5 & SIM_SCGC5_SLCD) == 0) || ((LCD_GCR & LCD_GCR_LCDEN) == 0)) { // if SLCD controller not enabled
        return;
    }
        #else
    if (((SIM_SCGC3 & SIM_SCGC3_SLCD) == 0) || ((LCD_GCR & LCD_GCR_LCDEN) != 0)) { // if SLCD controller not enabled
        return;
    }
        #endif
    if (LCD_WF3TO0 != lcd_wf3to0) {
        CollectCommand(0, LCD_WF3TO0);                                   // inform of the new value (register 0)
        lcd_wf3to0 = LCD_WF3TO0;
    }
    if (LCD_WF7TO4 != lcd_wf7to4) {
        CollectCommand(1, LCD_WF7TO4);                                   // inform of the new value (register 1)
        lcd_wf7to4 = LCD_WF7TO4;
    }
    if (LCD_WF11TO8 != lcd_wf11to8) {
        CollectCommand(2, LCD_WF11TO8);                                  // inform of the new value (register 2)
        lcd_wf11to8 = LCD_WF11TO8;
    }
    if (LCD_WF15TO12 != lcd_wf15to12) {
        CollectCommand(3, LCD_WF15TO12);                                 // inform of the new value (register 3)
        lcd_wf15to12 = LCD_WF15TO12;
    }
    if (LCD_WF19TO16 != lcd_wf19to16) {
        CollectCommand(4, LCD_WF19TO16);                                 // inform of the new value (register 4)
        lcd_wf19to16 = LCD_WF19TO16;
    }
    if (LCD_WF23TO20 != lcd_wf23to20) {
        CollectCommand(5, LCD_WF23TO20);                                 // inform of the new value (register 5)
        lcd_wf23to20 = LCD_WF23TO20;
    }
    if (LCD_WF27TO24 != lcd_wf27to24) {
        CollectCommand(6, LCD_WF27TO24);                                 // inform of the new value (register 6)
        lcd_wf27to24 = LCD_WF27TO24;
    }
    if (LCD_WF31TO28 != lcd_wf31to28) {
        CollectCommand(7, LCD_WF31TO28);                                 // inform of the new value (register 7)
        lcd_wf31to28 = LCD_WF31TO28;
    }
    if (LCD_WF35TO32 != lcd_wf35to32) {
        CollectCommand(8, LCD_WF35TO32);                                 // inform of the new value (register 8)
        lcd_wf35to32 = LCD_WF35TO32;
    }
    if (LCD_WF39TO36 != lcd_wf39to36) {
        CollectCommand(9, LCD_WF39TO36);                                 // inform of the new value (register 9)
        lcd_wf39to36 = LCD_WF39TO36;
    }
    if (LCD_WF43TO40 != lcd_wf43to40) {
        CollectCommand(10, LCD_WF43TO40);                                // inform of the new value (register 10)
        lcd_wf43to40 = LCD_WF43TO40;
    }
    if (LCD_WF47TO44 != lcd_wf47to44) {
        CollectCommand(11, LCD_WF47TO44);                                // inform of the new value (register 11)
        lcd_wf47to44 = LCD_WF47TO44;
    }
    if (LCD_WF51TO48 != lcd_wf51to48) {
        CollectCommand(12, LCD_WF51TO48);                                // inform of the new value (register 12)
        lcd_wf51to48 = LCD_WF51TO48;
    }
    if (LCD_WF55TO52 != lcd_wf55to52) {
        CollectCommand(13, LCD_WF55TO52);                                // inform of the new value (register 13)
        lcd_wf55to52 = LCD_WF55TO52;
    }
    if (LCD_WF59TO56 != lcd_wf59to56) {
        CollectCommand(14, LCD_WF59TO56);                                 // inform of the new value (register 14)
        lcd_wf59to56 = LCD_WF59TO56;
    }
    if (LCD_WF63TO60 != lcd_wf63to60) {
        CollectCommand(15, LCD_WF63TO60);                                 // inform of the new value (register 15)
        lcd_wf63to60 = LCD_WF63TO60;
    }
    #endif
}
#endif


#if defined BATTERY_BACKED_RAM
// Return all RTC content which is battery backed up
//
extern int fnGetBatteryRAMContent(unsigned char *ucData, unsigned long ulReference)
{
    return 0;                                                            // all data saved
}
extern int fnPutBatteryRAMContent(unsigned char ucData, unsigned long ulReference)
{
    return 0;                                                            // no more data accepted
}
#endif

#if (NUMBER_OF_CAN_INTERFACES > 0) && defined CAN_INTERFACE
static int iLastTxBuffer;


typedef struct stTCP_MESSAGE                                             // definition of a data frame structure
{
  //TCP_HEADER     tTCP_Header;                                          // reserve header space
    unsigned char  ucTCP_Message[50];                                    // space for content
} TCP_MESSAGE;

#define SIM_CAN_TX_OK           0
#define SIM_CAN_TX_REMOTE_OK    1
#define SIM_CAN_TX_FAIL         2
#define SIM_CAN_TX_REMOTE_FAIL  3
#define SIM_CAN_RX_READY        4


static void fnBufferSent(int iBuffer, int iRemote)
{
#if defined MSCAN_CAN_INTERFACE
#else
    int iChannel = (iBuffer >> 24);                                      // extract the channel number
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;

    iBuffer &= 0x00ffffff;                                               // remove channel number
    switch (iChannel) {
    case 0:
        ptrMessageBuffer = MBUFF0_ADD_0;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        break;
    #if NUMBER_OF_CAN_INTERFACES > 1
    case 1:
        ptrMessageBuffer = MBUFF0_ADD_1;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        break;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
    case 2:
        ptrMessageBuffer = MBUFF0_ADD_2;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN2_BASE_ADD;
        break;
    #endif
    }
    ptrMessageBuffer += iBuffer;

    switch (iRemote) {        
    case SIM_CAN_TX_OK:
        if (((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ONCE) && (ptrMessageBuffer->ulCode_Len_TimeStamp & RTR)) { // sending remote frame
            ptrMessageBuffer->ulCode_Len_TimeStamp = (((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_RX_EMPTY) | (ptrCAN_control->CAN_TIMER & 0x0000ffff)); // convert to temporary rx message buffer
        }
        else {
            ptrMessageBuffer->ulCode_Len_TimeStamp = (((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE) | (ptrCAN_control->CAN_TIMER & 0x0000ffff));
        }
        break;
    case SIM_CAN_TX_FAIL:
        {
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                ptrCAN_control->CAN_ESR1 |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (ptrCAN_control->CAN_CTRL1 & ERRMSK) {                // only generate interrupt when interrupt is enabled
                    switch (iChannel) {
                    case 0:
                        if (fnGenInt(irq_CAN0_ERROR_ID) != 0) {          // if CAN0 error interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_CAN0_ERROR(); // call the interrupt handler
                        }
                        break;
                    #if NUMBER_OF_CAN_INTERFACES > 1
                    case 1:
                        if (fnGenInt(irq_CAN1_ERROR_ID) != 0) {          // if CAN1 error interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_CAN1_ERROR(); // call the interrupt handler
                        }
                        break;
                    #endif
                    #if NUMBER_OF_CAN_INTERFACES > 2
                    case 2:
                        if (fnGenInt(irq_CAN2_ERROR_ID) != 0) {          // if CAN2 error interrupt is not disabled
                            VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                            ptrVect->processor_interrupts.irq_CAN2_ERROR(); // call the interrupt handler
                        }
                        break;
                    #endif
                    }
                }
            }
        }
        break;
    case SIM_CAN_TX_REMOTE_OK:                                           // don't clear the message buffer on remote transmissions
    case SIM_CAN_RX_READY:
        break;
    }
    switch (iBuffer) {                                                   // call transmission complete interrupt
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        ptrCAN_control->CAN_IFLAG1 |= (1 << iBuffer);                    // set interrupt flag for the buffer
        if (ptrCAN_control->CAN_IMASK1 & (1 << iBuffer)) {               // check whether the interrupt is enabled on this buffer
            switch (iChannel) {
            case 0:
                if (fnGenInt(irq_CAN0_MESSAGE_ID) != 0) {                // if CAN0 message interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_CAN0_MESSAGE();    // call the interrupt handler
                }
                break;
    #if NUMBER_OF_CAN_INTERFACES > 1
            case 1:
                if (fnGenInt(irq_CAN1_MESSAGE_ID) != 0) {                // if CAN1 message interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_CAN1_MESSAGE();    // call the interrupt handler
                }
                break;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
            case 2:
                if (fnGenInt(irq_CAN2_MESSAGE_ID) != 0) {                // if CAN2 message interrupt is not disabled
                    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
                    ptrVect->processor_interrupts.irq_CAN2_MESSAGE();    // call the interrupt handler
                }
                break;
    #endif
            }
        }
        break;
    default:                                                             // this happens when something is received
        break;
    }
#endif
}

    #if !defined _LOCAL_SIMULATION
// Komodo has received something on the CAN bus
//
static void fnCAN_reception(int iChannel, unsigned char ucDataLength, unsigned char *ptrData, unsigned long ulId, int iExtendedAddress, int iRemodeRequest, unsigned short usTimeStamp, Komodo km)
{
#if defined MSCAN_CAN_INTERFACE
#else
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;
    int i = 0;
    int iRxAvailable = 0;
    int iOverrun = 0;
    unsigned char ucBuffer;
    unsigned long ulRxFilter = 0xffffffff;
    if ((ucDataLength == 0) && (iRemodeRequest == 0)) {
        return;                                                          // ignore when reception without data
    }
    switch (iChannel) {
    case 0:
        ptrMessageBuffer = MBUFF0_ADD_0;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        break;
    #if NUMBER_OF_CAN_INTERFACES > 1
    case 1:
        ptrMessageBuffer = MBUFF0_ADD_1;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        break;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
    case 2:
        ptrMessageBuffer = MBUFF0_ADD_2;
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN2_BASE_ADD;
        break;
    #endif
    }

    if (iExtendedAddress == 0) {
        ulId <<= CAN_STANDARD_SHIFT;
        ulId &= CAN_STANDARD_BITMASK;
    }
    else {
        ulId = iExtendedAddress;
    }
    ulRxFilter = ptrCAN_control->CAN_RXGMASK;                            // filter mask used for all buffers apart from final two

    while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
        if ((((ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) != 0) == (iExtendedAddress != 0)) && (ptrMessageBuffer->ulID & ulRxFilter) == (ulId & ulRxFilter)) {
            iRxAvailable++;
            if (iRemodeRequest != 0) {                                   // remote request being received
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ON_REQ) { // remote message waiting to be sent
    #if defined SIM_KOMODO
                    int iResult;
                    km_can_packet_t pkt;
                    unsigned long arb_count = 0;
                    unsigned char ucTxDataLength;
                    unsigned char ucData[8];
                    pkt.id = ptrMessageBuffer->ulID;
                    // Send the CAN frame via remote simulator
                    // 
                    pkt.remote_req = 0;          
                    if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                        pkt.id |= CAN_EXTENDED_ID;                       // the address is to be handled as extended
                        pkt.extend_addr = 1;
                    }   
                    else {
                        pkt.id >>= CAN_STANDARD_SHIFT;                   // the address if a standard address
                        pkt.extend_addr = 0;
                    }
                    ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                    pkt.dlc = ucTxDataLength;
                    // Convert from long word, big-endian format
                    //
                    if (ucTxDataLength != 0) {
                        ucData[0] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 24);
                        if (ucTxDataLength > 1) {
                            ucData[1] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 16);
                            if (ucTxDataLength > 2) {
                                ucData[2] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 8);
                                if (ucTxDataLength > 3) {
                                    ucData[3] = (unsigned char)(ptrMessageBuffer->ulData[0]);
                                    if (ucTxDataLength > 4) {
                                        ucData[4] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 24);
                                        if (ucTxDataLength > 5) {
                                            ucData[5] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 16);
                                            if (ucTxDataLength > 6) {
                                                ucData[6] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 8);
                                                if (ucTxDataLength > 7) {
                                                    ucData[7] = (unsigned char)(ptrMessageBuffer->ulData[1]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ucData); // send
                    iResult = km_can_async_collect(km, 10, &arb_count);  // collect the result of the last transmission
                    switch (iResult) {
                    case KM_OK:
                        fnBufferSent(((iChannel << 24) | i), SIM_CAN_TX_REMOTE_OK);
                        break;
                    default:
                        fnBufferSent(((iChannel << 24) | i), SIM_CAN_TX_FAIL);
                        break;
                    }
                    return;
    #endif
                }
            }
            else if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_RX_EMPTY) {
                ucBuffer = (unsigned char)i;                             // we use this buffer for reception since it suits
                break;
            }
        }
        ptrMessageBuffer++;
        switch (++i) {
        case 14:
            ulRxFilter = ptrCAN_control->CAN_RX14MASK;
            break;
        case 15:
            ulRxFilter = ptrCAN_control->CAN_RX15MASK;
            break;
        }
    }

    switch (iChannel) {
    case 0:
        ptrMessageBuffer = MBUFF0_ADD_0;
        break;
    #if NUMBER_OF_CAN_INTERFACES > 1
    case 1:
        ptrMessageBuffer = MBUFF0_ADD_1;
        break;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
    case 2:
        ptrMessageBuffer = MBUFF0_ADD_2;
        break;
    #endif
    }

    if (iRxAvailable != 0) {
        ulRxFilter = ptrCAN_control->CAN_RXGMASK;
        i = 0;
        while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
            if ((((ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) != 0) == (iExtendedAddress != 0)) && (ptrMessageBuffer->ulID & ulRxFilter) == (ulId & ulRxFilter)) {
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == (MB_RX_FULL)) {
                    ucBuffer = (unsigned char)i;                         // we use this buffer for reception - it will set overrun...
                    iOverrun = 1;
                    break;
                }
            }
            ptrMessageBuffer++;
            switch (++i) {
            case 14:
                ulRxFilter = ptrCAN_control->CAN_RX14MASK;
                break;
            case 15:
                ulRxFilter = ptrCAN_control->CAN_RX15MASK;
                break;
            }
        }
    }
    else {
        return;                                                          // no reception buffer found so ignore
    }

    switch (iChannel) {
    case 0:
        ptrMessageBuffer = MBUFF0_ADD_0;
        break;
    #if NUMBER_OF_CAN_INTERFACES > 1
    case 1:
        ptrMessageBuffer = MBUFF0_ADD_1;
        break;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
    case 2:
        ptrMessageBuffer = MBUFF0_ADD_2;
        break;
    #endif
    }
    ptrMessageBuffer += ucBuffer;                                        // set the local simulate buffer correspondingly

    if (iOverrun != 0) {
        ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_OVERRUN | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME)));
    }
    else {
        ptrMessageBuffer->ulCode_Len_TimeStamp = ((MB_RX_FULL) | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME))); // clear out code, length and time stamp fields
    }

    ptrMessageBuffer->ulCode_Len_TimeStamp |= usTimeStamp;
    ptrMessageBuffer->ulCode_Len_TimeStamp |= (ucDataLength << 16);
    ptrMessageBuffer->ulID = ulId;

    i = 0;
    // Save in long word, big-endian format
    //
    ptrMessageBuffer->ulData[0] = ((*(ptrData) << 24) | (*(ptrData + 1) << 16) | (*(ptrData + 2) << 8) | (*(ptrData + 3)));
    ptrMessageBuffer->ulData[1] = ((*(ptrData + 4) << 24) | (*(ptrData + 5) << 16) | (*(ptrData + 6) << 8) | (*(ptrData + 7)));
    fnBufferSent((ucBuffer | (iChannel << 24)), SIM_CAN_RX_READY);       // generate interrupt
#endif
}
    #endif

#if defined CAN_INTERFACE
extern void fnSimulateCanIn(int iChannel, unsigned long ilID, int iRTR, unsigned char *ptrData, unsigned char ucDLC)
{
    static unsigned short usTimeStamp = 0;
    unsigned long ilID_extended;
    if ((ilID & 0x80000000) != 0) {
        ilID_extended = ilID & ~0x80000000;
        ilID = 0;
    }
    else {
        ilID_extended = 0;
    }
    fnCAN_reception(iChannel, ucDLC, ptrData, ilID, ilID_extended, iRTR, usTimeStamp++, 0);
}
#endif

extern void fnSimCAN(int iChannel, int iBufferNumber, int iSpecial)
{
#if defined MSCAN_CAN_INTERFACE
#else
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;
    #if !defined _LOCAL_SIMULATION
        #if defined USE_TELNET && defined UTASKER_SIM
    unsigned short usTxDataLength;
    TCP_MESSAGE tcp_message;
        #elif defined SIM_KOMODO
    unsigned char ucTxDataLength;
    static Komodo km = -1;
        #endif
    #endif

    #if defined SIM_KOMODO
    if (CAN_SIM_CHECK_RX == iSpecial) {
        u08 data_in[8];
        km_can_packet_t pkt;
        km_can_info_t   info;
        pkt.remote_req  = 0;
        pkt.extend_addr = 0;
        pkt.dlc         = 8;
        pkt.id          = 0xff;                                          // promiscuous

        if (km < 0) {
            return;
        }
        (CAN0_TIMER)++;                                                  // normally these only count when CAN controller is enabled and they would count at the CAN bit rate
        #if NUMBER_OF_CAN_INTERFACES > 1
        (CAN1_TIMER)++;
        #endif
        while ((km_can_read(km, &info, &pkt, 8, data_in)) >= 0) {        // has something been received?
            fnCAN_reception(info.channel, pkt.dlc, data_in, pkt.id, pkt.extend_addr, pkt.remote_req, (unsigned short)(info.timestamp), km);
        }
        return;
    }
    else if (CAN_SIM_TERMINATE == iSpecial) {
        if (km < 0) {
            return;
        }
        km_disable(km);
        km_close(KOMODO_USB_PORT);
        return;
    }
    #endif
    switch (iChannel) {
    case 0:
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
        break;
    #if NUMBER_OF_CAN_INTERFACES > 1
    case 1:
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_1;                                 // the first of 16 message buffers in the FlexCan module
        break;
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 2
    case 2:
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN2_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_2;                                 // the first of 16 message buffers in the FlexCan module
        break;
    #endif
    }

    // Configuration changes
    //
    if ((ptrCAN_control->CAN_MCR & (CAN_FRZ | CAN_HALT)) != 0x00) {
        return;                                                          // off so don't do anything
    }

    #if defined USE_TELNET && defined UTASKER_SIM
    if (SimSocket == -1) {
        #if defined IMMEDIATE_MEMORY_ALLOCATION                          // {17} extra parameter for tx buffer size to immediately allocate (uses default size)
        SimSocket = fnStartTelnet(usSimPort, 0xffff, 0, 0, 0, fnCANListener);
        #else
        SimSocket = fnStartTelnet(usSimPort, 0xffff, 0, 0, fnCANListener);
        #endif
        fnTelnet(SimSocket, TELNET_RAW_MODE);                            // set to RAW operating mode
        fnTCP_Connect(SimSocket, ucSimIP, usSimPort, 0, 0);              // attempt to establish a connection to remote  server
        return;
    }
    #elif defined SIM_KOMODO
    if (CAN_SIM_INITIALISE == iSpecial) {
        if (km < 0) {
            km = km_open(KOMODO_USB_PORT);                               // open a link to the Komodo via USB (channel 0 uses CAN A, channel 1 uses CAN B)
        }
        if (km >= 0) {
            int iCAN_speed = 0;
            if ((CLK_SRC_PERIPH_CLK & ptrCAN_control->CAN_CTRL1) != 0) { // CAN clock derived from external clock/crystal (lowest jitter)
                iCAN_speed = ((_EXTERNAL_CLOCK/25)/(ptrCAN_control->CAN_CTRL1 >> 24)); // assume that time quanta is 25
            }
            if (iChannel != 0) {                                         // CAN B
                km_disable(km);
                km_acquire(km, (KM_FEATURE_CAN_B_CONFIG | KM_FEATURE_CAN_B_CONTROL | KM_FEATURE_CAN_B_LISTEN)); // acquire features
                km_timeout(km, KM_TIMEOUT_IMMEDIATE);
                km_can_bitrate(km, KM_CAN_CH_B, iCAN_speed);             // set bitrate
              //km_can_target_power(km, KM_CAN_CH_B, 1);                 // set target power
                km_enable(km);
              //km_can_target_power(km, KM_CAN_CH_B, 0);                 // disable target power
            }
            else {                                                       // CAN A
                km_disable(km);
                km_acquire(km, (KM_FEATURE_CAN_A_CONFIG | KM_FEATURE_CAN_A_CONTROL | KM_FEATURE_CAN_A_LISTEN)); // acquire features
                km_timeout(km, KM_TIMEOUT_IMMEDIATE);
                km_can_bitrate(km, KM_CAN_CH_A, iCAN_speed);             // set bitrate
              //km_can_target_power(km, KM_CAN_CH_A, 1);                 // set target power
                km_enable(km);
              //km_can_target_power(km, KM_CAN_CH_A, 0);                 // disable target power
            }
        }
    }
    #endif

    ptrMessageBuffer += iBufferNumber;                                   // move to the buffer

    // Transmission
    //
    switch (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) {
    case MB_TX_INACTIVE:
        // Assume that we were previously active - for example sending remote frames - disable the HW simulator
        //
    #if !defined _LOCAL_SIMULATION
        #if defined USE_TELNET && defined UTASKER_SIM
        if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
            if (iSpecial == CAN_SIM_FREE_BUFFER) {
                tcp_message.ucTCP_Message[0] = 'd';
            }
            else  {
                tcp_message.ucTCP_Message[0] = 'D';
            }
            tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number
            fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, 2, TCP_BUF_SEND); // send data to HW simulator
        }
        #elif defined SIM_KOMODO
        if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
            if (iSpecial == CAN_SIM_FREE_BUFFER) {
                // Nothing to do in this case
                //
            }
            else  {
                unsigned char data[3] = {0x01, 0x02, 0x03};
                km_can_packet_t pkt;
                pkt.remote_req   = 0;
                pkt .extend_addr = 1;
                pkt.dlc          = 3;
                pkt.id           = 0x105;
                km_can_async_submit(km, iChannel, 0, &pkt, pkt.dlc, data);
            }
        }
        #endif
    #endif
        break;

    case MB_TX_SEND_ON_REQ:                                              // this buffer containes a queued message to be sent on a Remote Frame
    #if defined _LOCAL_SIMULATION
        // fall through
    #endif
    case MB_TX_SEND_ONCE:                                           
        if ((ptrMessageBuffer->ulCode_Len_TimeStamp & RTR) != 0) {       // remote frame is to be transmitted
    #if defined _LOCAL_SIMULATION
        #if defined _TX_OK
            // The buffer converts automatically to a receive buffer
            //
            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_FULL | RTR | (0x03<<16));
            ptrMessageBuffer->ucData[0] = 0x01;                          // receive some dummy data
            ptrMessageBuffer->ucData[1] = 0x02;
            ptrMessageBuffer->ucData[2] = 0x03;
            goto _rx_int;
        #else
            // The remote transmission failed
            //
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                ptrCAN_control->CAN_ESR1 |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (ptrCAN_control->CAN_CTRL1 & ERRMSK) {                // only generate interrupt when interrupt is enabled
                    CAN_error_Interrupt();                               // simulate a number of errors
                }
            }
        #endif
    #else
            if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {// send a remote frame
        #if defined USE_TELNET && defined UTASKER_SIM
                unsigned long ulID;
                tcp_message.ucTCP_Message[0] = 'r';
                tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number

                ulID = ptrMessageBuffer->ulID;
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    ulID |= CAN_EXTENDED_ID;                             // the address is to be handled as extended
                }   
                else {
                    ulID >>= CAN_STANDARD_SHIFT;                         // the address if a standard address
                }
                tcp_message.ucTCP_Message[2] = (unsigned char)(ulID >> 24);
                tcp_message.ucTCP_Message[3] = (unsigned char)(ulID >> 16);
                tcp_message.ucTCP_Message[4] = (unsigned char)(ulID >> 8);
                tcp_message.ucTCP_Message[5] = (unsigned char)(ulID);

                fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, 6, TCP_BUF_SEND); // send data to HW simulator
        #elif defined SIM_KOMODO
                int iResult;
                km_can_packet_t pkt;
                unsigned long arb_count = 0;
                pkt.id = ptrMessageBuffer->ulID;
                // Send the CAN frame via remote simulator
                // 
                pkt.remote_req = 1;          
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    pkt.id |= CAN_EXTENDED_ID;                           // the address is to be handled as extended
                    pkt.extend_addr = 1;
                }   
                else {
                    pkt.id >>= CAN_STANDARD_SHIFT;                       // the address if a standard address
                    pkt.extend_addr = 0;
                }
                iLastTxBuffer = iBufferNumber;
                iBufferNumber = 0;
                ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                pkt.dlc = ucTxDataLength;
                km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, 0, 0); // send
                iResult = km_can_async_collect(km, 10, &arb_count);      // collect the result of the last transmission
                switch (iResult) {
                case KM_OK:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_OK);
                    break;
                default:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_FAIL);
                    break;
                }
        #endif
            }
            else {
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_FULL | RTR | (0x03 << 16)); // the buffer converts automatically to a receive buffer
                ptrMessageBuffer->ulData[0] = 0x01020300;                // receive some dummy data

                fnBufferSent(iBufferNumber, SIM_CAN_TX_REMOTE_OK);
            }
    #endif
        }
        else {                                                           // this buffer contains a message to be transmitted once
    #if defined _LOCAL_SIMULATION                                        // simple simulation for simple testing of driver
        #if defined _TX_OK
            ptrMessageBuffer->ulCode_Len_TimeStamp = ((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE);
_rx_int:
            CAN_IFLAG1 |= (0x00000001 << iBufferNumber);                 // set interrupt flag
            if (CAN_IMASK1 & (0x00000001 << iBufferNumber)) {
                CAN_buf_Interrupt(iBufferNumber);                        // call interrupt routine
            }
        #else
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                ptrCAN_control->CAN_ESR1 |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (ptrCAN_control->CAN_CTRL1 & ERRMSK) {                // only generate interrupt when interrupt is enabled
                    CAN_error_Interrupt();                               // simulate a number of errors
                }
            }
        #endif
    #else
            if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
        #if defined USE_TELNET && defined UTASKER_SIM
                unsigned long ulID;
                // Send the CAN frame to remote simulator
                // [['T'] - BUFFER NUMBER - LENGTH - ID - DATA[...]]
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) ==  MB_TX_SEND_ON_REQ) {
                    tcp_message.ucTCP_Message[0] = 'R';                  // set remote transmission
                }
                else {
                    tcp_message.ucTCP_Message[0] = 'T';
                }
                tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number
                tcp_message.ucTCP_Message[2] = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                ulID = ptrMessageBuffer->ulID;
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    ulID |= CAN_EXTENDED_ID;                             // the address is to be handled as extended
                }   
                else {
                    ulID >>= CAN_STANDARD_SHIFT;                         // the address if a standard address
                }
                tcp_message.ucTCP_Message[3] = (unsigned char)(ulID >> 24);
                tcp_message.ucTCP_Message[4] = (unsigned char)(ulID >> 16);
                tcp_message.ucTCP_Message[5] = (unsigned char)(ulID >> 8);
                tcp_message.ucTCP_Message[6] = (unsigned char)(ulID);

                iLastTxBuffer = iBufferNumber;

                iBufferNumber = 0;
                usTxDataLength = tcp_message.ucTCP_Message[2] + 7;
                while (iBufferNumber++ < tcp_message.ucTCP_Message[2]) {
                    tcp_message.ucTCP_Message[iBufferNumber+6] = ptrMessageBuffer->ucData[iBufferNumber-1];
                }
                fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, usTxDataLength, TCP_BUF_SEND); // send data to HW simulator
        #elif defined SIM_KOMODO
                int iResult;
                km_can_packet_t pkt;
                unsigned long arb_count = 0;
                unsigned char ucData[8];
                pkt.id = ptrMessageBuffer->ulID;
                // Send the CAN frame via remote simulator
                // 
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ON_REQ) {
                    return;                                              // the message is not sent in this case but will be sent on any remote frame request receptions
                }
                else {
                    pkt.remote_req = 0;
                }             
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    pkt.id |= CAN_EXTENDED_ID;                           // the address is to be handled as extended
                    pkt.extend_addr = 1;
                }   
                else {
                    pkt.id >>= CAN_STANDARD_SHIFT;                       // the address if a standard address
                    pkt.extend_addr = 0;
                }

                iLastTxBuffer = iBufferNumber;
                iBufferNumber = 0;
                ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                pkt.dlc = ucTxDataLength;
                // Convert from long word, big-endian format
                //
                if (ucTxDataLength != 0) {
                    ucData[0] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 24);
                    if (ucTxDataLength > 1) {
                        ucData[1] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 16);
                        if (ucTxDataLength > 2) {
                            ucData[2] = (unsigned char)(ptrMessageBuffer->ulData[0] >> 8);
                            if (ucTxDataLength > 3) {
                                ucData[3] = (unsigned char)(ptrMessageBuffer->ulData[0]);
                                if (ucTxDataLength > 4) {
                                    ucData[4] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 24);
                                    if (ucTxDataLength > 5) {
                                        ucData[5] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 16);
                                        if (ucTxDataLength > 6) {
                                            ucData[6] = (unsigned char)(ptrMessageBuffer->ulData[1] >> 8);
                                            if (ucTxDataLength > 7) {
                                                ucData[7] = (unsigned char)(ptrMessageBuffer->ulData[1]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ucData); // send
                iResult = km_can_async_collect(km, 10, &arb_count);      // collect the result of the last transmission
iResult = KM_OK;
                switch (iResult) {
                case KM_OK:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_OK);
                    break;
                default:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_FAIL);
                    break;
                }
        #endif
            }
            else {
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) ==  MB_TX_SEND_ON_REQ) {
                    fnBufferSent(((iChannel << 24) | iBufferNumber), SIM_CAN_TX_REMOTE_OK);
                }
                else {
                    fnBufferSent(((iChannel << 24) | iBufferNumber), SIM_CAN_TX_OK);
                }
                return;
            }
    #endif
        }
        break;
    default:
        break;
    }
#endif
}
#endif

#if defined SUPPORT_TOUCH_SCREEN
static int iPenLocationX = 0;                                            // last sample ADC input value
static int iPenLocationY = 0;
static int iPenDown = 0;

    #if defined K70F150M_12M
        #define MIN_X_TOUCH          0x0230                              // tuned values - for calibration these should be taken from parameters
        #define MAX_X_TOUCH          0x0e90
        #define MIN_Y_TOUCH          0x03c5
        #define MAX_Y_TOUCH          0x0d30
    #elif defined TOUCH_FT6206
        #define MIN_X_TOUCH          0x0230                              // tuned values - for calibration these should be taken from parameters
        #define MAX_X_TOUCH          0x0e90
        #define MIN_Y_TOUCH          0x03c5
        #define MAX_Y_TOUCH          0x0d30
    #endif

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
    if (iPenDown == 0) {                                                 // if the pen is not applied a high voltage is expected - higher than the normal maximum pressed value
        *ptrX = (MAX_X_TOUCH + 1);
        *ptrY = (MAX_Y_TOUCH + 1);
    }
    else {
        *ptrX = (MIN_X_TOUCH + ((iPenLocationX * ((MAX_X_TOUCH - MIN_X_TOUCH)))/GLCD_X));
        *ptrY = (MIN_Y_TOUCH + ((iPenLocationY * ((MAX_Y_TOUCH - MIN_Y_TOUCH)))/GLCD_Y));
    }
}
#endif

// Calculate a bit-banded peripheral address back to its original register and bit - then set the bit in the register
//
extern void fnSetBitBandPeripheralValue(unsigned long *bit_band_address)
{
    unsigned long ulRegAddress;
    unsigned long ulBit;
    ulRegAddress = ((unsigned long)bit_band_address - 0x42000000);
    ulBit = ((ulRegAddress / 4) % 32);
    ulBit = (1 << ulBit);
    ulRegAddress /= 32;
    ulRegAddress &= ~0x3;
    *(unsigned long *)ulRegAddress |= ulBit;
}

// Calculate a bit-banded peripheral address back to its original register and bit - then clear the bit in the register
//
extern void fnClearBitBandPeripheralValue(unsigned long *bit_band_address)
{
    unsigned long ulRegAddress;
    unsigned long ulBit;
    ulRegAddress = ((unsigned long)bit_band_address - 0x42000000);
    ulBit = ((ulRegAddress / 4) % 32);
    ulBit = (1 << ulBit);
    ulRegAddress /= 32;
    ulRegAddress &= ~0x3;
    *(unsigned long *)ulRegAddress &= ~ulBit;
}

// Calculate a bit-banded peripheral address back to its original register and bit - then check the bit in the register
//
extern int fnCheckBitBandPeripheralValue(unsigned long *bit_band_address)
{
    unsigned long ulRegAddress;
    unsigned long ulBit;
    ulRegAddress = ((unsigned long)bit_band_address - 0x42000000);
    ulBit = ((ulRegAddress / 4) % 32);
    ulBit = (1 << ulBit);
    ulRegAddress /= 32;
    ulRegAddress &= ~0x3;
    return ((*(unsigned long *)ulRegAddress & ulBit) != 0);
}

#if defined KINETIS_WITH_PCC

static unsigned long fnGetPCC_source(unsigned long ulPCC_setting)
{
    unsigned long ulClockSpeed = 0;
    if ((ulPCC_setting & PCC_CGC) == 0) {
        return 0;                                                        // clocks to the module are not enabled
    }
    switch (ulPCC_setting & PCC_PCS_MASK) {
    case PCC_PCS_CLOCK_OFF:
        return 0;                                                        // no clock source is selected
    case PCC_PCS_SCGPCLK:                                                // system PLL clock
    case PCC_PCS_OSCCLK:                                                 // system oscillator bus clock
    case PCC_PCS_SCGIRCLK:                                               // slow IRC clock
        _EXCEPTION("To add!");
        break;
    case PCC_PCS_SCGFIRCLK:                                              // fast IRC clock
        if ((SCG_FIRCCSR & SCG_FIRCCSR_FIRCEN) == 0) {                   // check that the fast IRC is enabled
            return 0;                                                    // no clock source enabled
        }
        else {
            unsigned long ulSourceDivider;
            switch (SCG_FIRCCFG) {
            case SCG_FIRCCFG_RANGE_60MHz:
                ulClockSpeed = 60000000;
                break;
            case SCG_FIRCCFG_RANGE_56MHz:
                ulClockSpeed = 56000000;
                break;
            case SCG_FIRCCFG_RANGE_52MHz:
                ulClockSpeed = 52000000;
                break;
            case SCG_FIRCCFG_RANGE_48MHz:
                ulClockSpeed = 48000000;
                break;
            default:
                _EXCEPTION("Invalid FIRC selected!");
                break;
            }
            ulSourceDivider = ((SCG_FIRCDIV >> PERIPHERAL_CLOCK_DIV_SHIFT) & 0x7);
            switch (ulSourceDivider) {
            case 0:
                return 0;                                            // peripheral clock source enabled
            case 1:                                                  // divide by 1
                break;
            case 2:                                                  // divide by 2
                ulClockSpeed /= 2;
                break;
            case 3:                                                  // divide by 4
                ulClockSpeed /= 4;
                break;
            case 4:                                                  // divide by 8
                ulClockSpeed /= 8;
                break;
            case 5:                                                  // divide by 16
                ulClockSpeed /= 16;
                break;
            case 6:                                                  // divide by 32
                ulClockSpeed /= 32;
                break;
            case 7:                                                  // divide by 64
                ulClockSpeed /= 64;
                break;
            }
        }
        break;
    default:
        _EXCEPTION("Invalid clock source!");
        break;
    }
    return ulClockSpeed;
}

// Calculate the selected clock speed at a peripheral connected by PCC
//
extern unsigned long fnGetPCC_clock(int iReference)
{
    unsigned long ulClockSpeed = 0;
    switch (iReference) {
    #if defined PCC_ADC0
    case KINETIS_PERIPHERAL_ADC0:
        return fnGetPCC_source(PCC_ADC0);
    #endif
    #if defined PCC_ADC1
    case KINETIS_PERIPHERAL_ADC1:
        return fnGetPCC_source(PCC_ADC1);
    #endif
    #if defined PCC_ADC2
    case KINETIS_PERIPHERAL_ADC2:
        return fnGetPCC_source(PCC_ADC2);
    #endif
    #if defined PCC_ADC3
    case KINETIS_PERIPHERAL_ADC3:
        return fnGetPCC_source(PCC_ADC3);
    #endif
    #if defined PCC_FTM0
    case KINETIS_PERIPHERAL_FTM0:
        return fnGetPCC_source(PCC_FTM0);
    #endif
    #if defined PCC_FTM1
    case KINETIS_PERIPHERAL_FTM1:
        return fnGetPCC_source(PCC_FTM1);
    #endif
    #if defined PCC_FTM2
    case KINETIS_PERIPHERAL_FTM2:
        return fnGetPCC_source(PCC_FTM2);
    #endif
    #if defined PCC_FTM3
    case KINETIS_PERIPHERAL_FTM3:
        return fnGetPCC_source(PCC_FTM3);
    #endif
    }
    return ulClockSpeed;
}
#endif

#if 1 //defined RUN_IN_FREE_RTOS
extern unsigned long *fnGetRegisterAddress(unsigned long ulAddress)
{
    ulAddress -= 0xe000e000;
    ulAddress += (unsigned long)CORTEX_M4_BLOCK;
    return (unsigned long *)ulAddress;
}

extern void fnSetReg(int iRef, unsigned long ulValue)
{
    switch (iRef) {
    case 0:
        kinetis.CORTEX_M4_REGS.ulR0 = ulValue;
        break;
    case 14:
        kinetis.CORTEX_M4_REGS.ulPSP = ulValue;
        break;
    case 15:
        kinetis.CORTEX_M4_REGS.ulMSP = ulValue;
        break;
    case 19:
        kinetis.CORTEX_M4_REGS.ulPRIMASK = ulValue;
        break;
    case 20:
        kinetis.CORTEX_M4_REGS.ulFAULTMASK = ulValue;
        break;
    case 22:
        kinetis.CORTEX_M4_REGS.ulCONTROL = ulValue;
        break;
    }
}
#endif

// Prepare a string to be displayed in the simulator status bar          // {37}
//
extern void fnUpdateOperatingDetails(void)
{
#if !defined NO_STATUS_BAR
    extern void fnPostOperatingDetails(char *ptrDetails);
    unsigned long ulBusClockSpeed;
    #if defined FLEXBUS_AVAILABLE && defined MCGOUTCLK && defined SIM_CLKDIV1
    unsigned long ulFlexbusClockSpeed;
    #endif
    #if !defined BUS_FLASH_CLOCK_SHARED
    unsigned long ulFlashClockSpeed;
    #endif
    CHAR cOperatingDetails[256];
    CHAR *ptrBuffer = cOperatingDetails;
    ptrBuffer = uStrcpy(ptrBuffer, "FLASH = ");
    ptrBuffer = fnBufferDec((SIZE_OF_FLASH/1024), 0, ptrBuffer);
    #if defined HIGH_SPEED_RUN_MODE_AVAILABLE && defined HIGH_SPEED_RUN_MODE_REQUIRED
    if ((SMC_PMCTRL & SMC_PMCTRL_RUNM_HSRUN) == SMC_PMCTRL_RUNM_HSRUN) {
        ptrBuffer = uStrcpy(ptrBuffer, "k (read-only), SRAM");
    }
    else {
        ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    }
    #else
    ptrBuffer = uStrcpy(ptrBuffer, "k, SRAM = ");
    #endif
    ptrBuffer = fnBufferDec((SIZE_OF_RAM/1024), 0, ptrBuffer);
    #if defined BUS_FLASH_CLOCK_SHARED
    ptrBuffer = uStrcpy(ptrBuffer, "k, BUS/FLASH CLOCK = ");
    #else
    ptrBuffer = uStrcpy(ptrBuffer, "k, BUS CLOCK = ");
    #endif
    #if defined KINETIS_WITH_SCG
    ulBusClockSpeed = (DIVSLOW_CLK);                                     // flash and bus clock
    #elif defined KINETIS_KL
        #if defined BUS_FLASH_CLOCK_SHARED
    ulBusClockSpeed = (SYSTEM_CLOCK/(((SIM_CLKDIV1 >> 16) & 0xf) + 1));
        #else
    ulBusClockSpeed = (MCGOUTCLK / (((SIM_CLKDIV1 >> 24) & 0xf) + 1));
        #endif
    #elif defined KINETIS_KM
    if ((SIM_CLKDIV1 & SIM_CLKDIV1_SYSCLKMODE) != 0) {
        ulBusClockSpeed = (SYSTEM_CLOCK / 2);
    }
    else {
        ulBusClockSpeed = (SYSTEM_CLOCK);
    }
    #elif defined KINETIS_KV10
    ulBusClockSpeed = (SYSTEM_CLOCK/(((SIM_CLKDIV1 >> 16) & 0x7) + 1));
    #elif defined KINETIS_KV40
    ulBusClockSpeed = (MCGOUTCLK/(((SIM_CLKDIV1 >> 16) & 0xf) + 1));
    #elif defined KINETIS_KE
        #if defined SIM_CLKDIV
	ulBusClockSpeed = ICSOUT_CLOCK;
	switch (SIM_CLKDIV & SIM_CLKDIV_OUTDIV1_4) {
	case SIM_CLKDIV_OUTDIV1_1:
		break;
	case SIM_CLKDIV_OUTDIV1_2:
		ulBusClockSpeed /= 2;
		break;
	case SIM_CLKDIV_OUTDIV1_3:
		ulBusClockSpeed /= 3;
		break;
	case SIM_CLKDIV_OUTDIV1_4:
		ulBusClockSpeed /= 4;
		break;
	}
    if ((SIM_CLKDIV & SIM_CLKDIV_OUTDIV2_2) != 0) {
        ulBusClockSpeed /= 2;
    }
        #else
    if ((SIM_BUSDIV & SIM_BUSDIVBUSDIV) != 0) {
        ulBusClockSpeed = (SYSTEM_CLOCK/2);
    }
    else {
        ulBusClockSpeed = SYSTEM_CLOCK;
    }
        #endif
    #else
    ulBusClockSpeed = (MCGOUTCLK/(((SIM_CLKDIV1 >> 24) & 0xf) + 1));
    #endif
    ptrBuffer = fnBufferDec(ulBusClockSpeed, 0, ptrBuffer);
    #if defined HIGH_SPEED_RUN_MODE_AVAILABLE && defined HIGH_SPEED_RUN_MODE_REQUIRED
    if ((SMC_PMCTRL & SMC_PMCTRL_RUNM_HSRUN) == SMC_PMCTRL_RUNM_HSRUN) {
        ptrBuffer = uStrcpy(ptrBuffer, " [HS-RUN]");
    }
    #endif
    #if !defined BUS_FLASH_CLOCK_SHARED                                   // in these devices the bus and flash clock are independent
    ptrBuffer = uStrcpy(ptrBuffer, ", FLASH CLOCK = ");
    ulFlashClockSpeed = (MCGOUTCLK/(((SIM_CLKDIV1 >> 16) & 0xf) + 1));
    ptrBuffer = fnBufferDec(ulFlashClockSpeed, 0, ptrBuffer);
    #endif
    #if defined FLEXBUS_AVAILABLE && defined MCGOUTCLK && defined SIM_CLKDIV1
    ptrBuffer = uStrcpy(ptrBuffer, ", FLEXBUS CLOCK = ");
    ulFlexbusClockSpeed = (MCGOUTCLK/(((SIM_CLKDIV1 >> 20) & 0xf) + 1));
    ptrBuffer = fnBufferDec(ulFlexbusClockSpeed, 0, ptrBuffer);
    #endif
    #if defined FAST_PERIPHERAL_CLOCK_DIVIDE
    ptrBuffer = uStrcpy(ptrBuffer, ", FAST PERIPHERAL CLOCK = ");
    ptrBuffer = fnBufferDec((MCGOUTCLK/(((SIM_CLKDIV1 >> 24) & 0xf) + 1)), 0, ptrBuffer);
    #endif
    fnPostOperatingDetails(cOperatingDetails);
#endif
}
#endif

/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5223X.h (supports also M5213, M5221X, M5222X, M5225X)
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    15.05.2007 Add LOW_RES_MS, MED_RES_MS, HIGH_RES_MS hardware resolution control
    09.06.2007 Add Random Number Generator Accelerator support
    25.06.2007 Add Edge port module 1 registers and interrupt masks
    11.08.2007 Add RTC registers and extend the RTC setup to use it      {1}
    05.09.2007 Add address IRQ_ICR_8_15_START                            {2}
    09.09.2007 Add General Purpose Timer Module                          
    10.09.2007 Add M5222X support (_M5222X)
    12.09.2007 UART status registers declared volatile                   {3}
    22.09.2007 Add M5221X support (_M5221X)
    22.09.2007 Correct address of MCF_CLOCK_LPDR                         {4}
    26.09.2007 Made ADC power control register volatile and extended bit defines {5}
    12.10.2007 Add USB registers
    29.11.2007 Add PIT_1_PIF_INT_H
    18.03.2008 Add _SETBITS and _CLEARBITS macros                        {6}
    31.03.2008 Add improved PLL configuration and control                {7}
    10.04.2008 Add ADC interface support and extra register bits in ADC block {8}
    12.04.2008 Correct PDSR register names                               {9}
    19.04.2008 Add additional registers                                  {10}
    20.04.2008 Add module power-up and power-down macros                 {11}
    20.04.2008 Add PIT_INTERRUPT configuration                           {12}
    21.04.2008 Add DMA_TIMER_INTERRUPT configuration                     {13}
    21.04.2008 Add DMA_TIMER struct and packed attribute                 {14}
    21.04.2008 Add IC_DTIM_0_ADD                                         {15}
    07.06.2008 Add second I2C controller interrupts                      {16}
    07.06.2008 Add I2C control register addresses                        {17}
    09.06.2008 Configure M5223X project to save complete FLASH when simulating {18}
    22.06.2008 Automatically enable HW random number support if the M52235 is specified {19}
    29.06.2008 Modify _CLEARBITS macro to use positive logic             {20}
    05.07.2008 Optional prototype for external interrupt configuration   {21}
    19.07.2008 Add DMA_MASK                                              {22}
    06.08.2008 Correct General Purpose Timer register names              {23}
    13.08.2008 Add M5225X support                                        {24}
    19.09.2008 Extended USB defines for host mode                        {25}
    17.10.2008 Add SET_CONTROL_DIRECTION, CLEAR_SETUP and FIFO_READ macros for USB compatibility {26}
    28.10.2008 Add general purpose timer control interface               {27}
    17.12.2008 PORTS_AVAILABLE and PORT_WIDTH moved here from WinSim.c   {28}
    22.12.2008 Add ADC_SAMPLING_SPEED macro                              {29}
    20.01.2009 Add MICREL PHY registers                                  {30}
    20.01.2009 Correct M5225X xPAR register widths                       {31}
    27.01.2009 Add M5225X FlexBus                                        {32}
    28.01.2009 Extend Backup Watchdog Timer support                      {33}
    30.01.2009 Extend Reset controller flags and declare volatile        {34}
    14.02.2009 Change CFMCMD to volatile to avoid GCC changing command sequence ordering {35}
    07.03.2009 Exchange fnLE_add() for fnLE_add_F                        {36}
    13.03.2009 Add PWM module                                            {37}
    25.03.2009 Add M521XX configuration
    26.03.2009 Add M521X configuration (replacing _HW_M5213)
    01.04.2009 Add CLKOUTPAR defines for CLKOUT and FB_CLK on M5225X     {38}
    01.05.2009 DDR registers set volatile                                {39}
    13.05.2009 Add GP port bit defines                                   {40}
    13.06.2009 Add additional port macros for overall project compatibility {41}
    24.06.2009 Modify M52232 and M52236 IDs                              {42}
    25.08.2009 Add port simulation to port macros                        {43}
    22.09.2009 Only enable RNG when M52255 or M52259 Kirin3 types        {44}
    15.10.2009 Add peripheral port functions                             {45}
    24.10.2009 Correct ADC_SAMPLING_SPEED macro                          {46}
    16.11.2009 Rename SECRET_BIT in RTC to EXTALEN                       {47}
    24.12.2009 Extended I2C status bits defines                          {48}
    05.02.2010 Change ADC_CTRL1 and ADC_CTRL2 to volatile to avoid potential byte access {49}
    06.02.2010 Correct RNG vector in M5225x                              {50}
    07.02.2010 Extend Interrupt Controller Registers                     {51}
    02.04.2010 Add VALIDATE_NEW_CONFIGURATION()                          {52}
    10.04.2010 Add M5214, M5216, M5280, M5281 and M5282                  {53}
    21.05.2010 Add M5207 and M5208                                       {54}
    23.08.2010 Add SDCARD_MALLOC()                                       {55}
    29.08.2010 Add IRQ port function defines                             {56}
    30.09.2010 Add PWM interface                                         {57}
    02.05.2011 Correct M5225X CAN base address                           {58}
    19.05.2011 Remove __interrupt define                                 {59}
    17.06.2011 Add M523X                                                 {60}
    29.06.2011 Add I2C1 pins for _M5221X and _M521XX                     {61}
    21.08.2011 Adapt RTC_SETUP for time keeping compatibility            {62}
    02.10.2011 Add _EXCEPTION(x)                                         {63}
    08.10.2011 MIBRAM made volatile                                      {64}
    02.01.2012 Add NUMBER_OF_CAN_INTERFACES and COLDFIRE_CAN_CONTROL     {65}
    08.01.2012 Add _fnMIIread() and _fnMIIwrite() for debugging PHY      {66}
    04.04.2012 Add RTC_INITIALISATION for compatibility                  {67}
    28.07.2012 Add capture interrupt modes to the DMA timers             {68}
    12.10.2012 Add AS_FB_ALE_FUNCTION and AS_FB_CS1_FUNCTION for M5225x  {69}
    12.10.2012 Add additional port bit defines for M5225x                {70}
    15.03.2013 Modify CLEARx registers to have volatile properties       {71}
    03.07.2013 Add PHY_MULTI_PORT support                                {72}
    10.08.2013 Add PIT status register byte access                       {73}
    12.10.2013 Moved RANDOM_SEED_LOCATION to this header file and add BOOT_MAIL_BOX {74}
    20.10.2015 Add UTC seconds value to RTC_SETUP                        {75}
    12.09.2018 Add ptrRxDatBuffer to USBHW struct                        {76}

*/


#if defined _GNU
    #define _PACK          __attribute__((__packed__))
#else
    #define _PACK               
#endif
#define __PACK_ON
#define __PACK_OFF

#define SAVE_COMPLETE_FLASH                                              // {18} when simulating save complete FLASH content rather than only file system content

#define RUN_MODE                      0                                  // no low power operation (normal run)
#define WAIT_MODE                     1                                  // automatic wait mode operation (normal wait)

typedef struct stCOLDFIRE_CAN_BUF
{
    volatile unsigned long ulCode_Len_TimeStamp;
    unsigned long          ulID;
    unsigned char          ucData[8];
} COLDFIRE_CAN_BUF;

#define NON_INITIALISED_RAM_SIZE    4                                    // this number of bytes are located above the initial stack pointer so that they can be used as uninitialised memory

#if defined _M5214 || defined _M5216 || defined _M5280 || defined _M5281 || defined _M5282 || defined _M520X // {53}{54}
    #if !defined _M520X
        #define _M52XX_SDRAM                                             // these types are all in 256 MAPBGA housing and have SDRAM interface
        #if defined _M5214 || defined _M5216
            #define _M521X_SDRAM
        #endif
    #endif
    #define RCON_ASSERTED            0x02                                // asserted (0) causes the states of D26:24, 21, 19:16 to determine the chip mode of operation
#elif defined  _M523X
    #define RCON_ASSERTED            0x02                                // asserted (0) causes the states of D25:24, 21:21, 19:16 to determine the chip mode of operation
#endif

// Memory set up for this target
//
#define FLASH_START_ADDRESS (0x00000000)                                 // renamed from START_OF_FLASH for compatibility
#define RAM_START_ADDRESS   (0x20000000)                                 // renamed from START_OF_SRAM for compatibility

#if defined _WINDOWS
    #include "SimM5223X.h"
    #define _EXCEPTION(x)            *(unsigned char *)0 = 0             // generate exception when simulating {63}
    #define _SIM_PORTS fnSimPorts()
#else
    #define _EXCEPTION(x)                                                // ignore on target
    #define _SIM_PORTS
#endif
#if defined _CODE_WARRIOR
  //#define __interrupt __declspec(interrupt)                            // {59}
#else
  //#define __interrupt                                                  // {59} dummy define
#endif

#if defined _EXTERNAL_INTERRUPTS                                         // {21}
    extern unsigned char *fnSetIntHandler(int iVectNumber, unsigned char *new_handler);
#endif


#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary
#define SDCARD_MALLOC(x) uMalloc(x)                                      // {55} use standard uMalloc()

#define BUS_CLOCK (PLL_OUTPUT_FREQ_INT/POST_DIV)                         // {7}
#define PLL_OUTPUT_FREQ  BUS_CLOCK
#if defined _M520X || defined _M523X                                     // {60}
    #define UART_CLOCK       (BUS_CLOCK/2)
#else
    #define UART_CLOCK       BUS_CLOCK
#endif

#define CAST_POINTER_ARITHMETIC unsigned long                            // M5223X uses 32 bit pointers

#define IPSBAR                (0x40000000)                               // address of peripheral block in memory

#if defined _M5221X || defined _M521XX || defined _M5222X || defined _M521X || defined  _M5225X || defined _M52XX_SDRAM  // {7}{24}{53}
    #define MAX_PLL_FREQUENCY  80000000
#elif defined _M523X
    #define MAX_PLL_FREQUENCY  150000000                                 // {60}
#elif defined _M520X                                                     // {54}
    #define MAX_PLL_FREQUENCY  166666666
#else
    #if defined _M52232 || defined _M52236
        #define MAX_PLL_FREQUENCY  50000000
    #else
        #define MAX_PLL_FREQUENCY  60000000
    #endif
#endif

#define RELAXATION_OSCILLATOR_FREQ 8000000                               // {33}

#if defined _M5221X || defined _M521XX || defined _M5225X || defined _M52XX_SDRAM || defined _M520X || defined _M523X // {33}{53}{54}{60}
    #define BACKUP_WATCHDOG_TIMER_AVAILABLE
#endif

#if defined _M52210 || defined _M52212 || defined _M52213
    #define UARTS_AVAILABLE (2)                                          // the number of UARTs available in the device
#else
    #define UARTS_AVAILABLE (3)                                          // the number of UARTs available in the device
#endif
#define LPI2C_AVAILABLE     0
#define I2C_AVAILABLE       2



#if defined _WINDOWS
    #define SCM_ADD             ((unsigned char *)(&ucM5223X.SimSCM))    // System Control Module
    #if defined _M5225X || defined _M520X || defined _M523X              // {54}
        #define FLEXBUS_ADD     ((unsigned char *)(&ucM5223X.SimFlexBus))// {32} FlexBus Module
	#endif
    #define DMA_ADD             ((unsigned char *)(&ucM5223X.SimDMAS))   // DMA Module (4 channels)
    #define UART_ADD            ((unsigned char *)(&ucM5223X.SimUARTS))  // UART Modules (3 channels)
    #define I2C_ADD             ((unsigned char *)(&ucM5223X.SimI2C))    // I2C module
    #define QSPI_ADD            ((unsigned char *)(&ucM5223X.SimQSPI))   // QSPI module
    #if defined _M5221X || defined _M5225X || defined _M521XX            // M5221X, M521XX and M5225X have 2 I2C bus modules {24}
        #define I2C_ADD_1       ((unsigned char *)(&ucM5223X.SimI2C_1))  // I2C module - 1
    #endif
    #define RTC_ADD             ((unsigned char *)(&ucM5223X.SimRTC))    // RTC module
    #define DMA_TIMER_0_ADD     ((unsigned char *)(&ucM5223X.DMATimer0)) // DMA timer 0
    #define DMA_TIMER_1_ADD     ((unsigned char *)(&ucM5223X.DMATimer1)) // DMA timer 1
    #define DMA_TIMER_2_ADD     ((unsigned char *)(&ucM5223X.DMATimer2)) // DMA timer 2
    #define DMA_TIMER_3_ADD     ((unsigned char *)(&ucM5223X.DMATimer3)) // DMA timer 3
    #define INT_CTRL_0_ADD      ((unsigned char *)(&ucM5223X.SimINT_CTRL0)) // Interrupt Controller
    #define INT_CTRL_1_ADD      ((unsigned char *)(&ucM5223X.SimINT_CTRL1)) // Interrupt Controller
    #define EMAC_BASE_ADD       ((unsigned char *)(&ucM5223X.SimEMAC))   // Fast Ethernet Controller
    #define EMAC_FIFO_ADD       ((unsigned char *)(&ucM5223X.SimEMACFifo)) // Fast Ethernet Controller - FIFO
    #define PORT_MODULE_ADD     ((unsigned char *)(&ucM5223X.SimPORT))   // Port Module
    #define RESET_CTRL_ADD      ((unsigned char *)(&ucM5223X.SimRESET))  // Reset controller Module
    #define CLOCK_MODULE_ADD    ((unsigned char *)(&ucM5223X.SimCMR))    // Clock Module
    #define EPORT_MODULE_0_ADD  ((unsigned char *)(&ucM5223X.SimEPORT))  // Edge Port Module 0
    #if defined BACKUP_WATCHDOG_TIMER_AVAILABLE
        #define BWTR_MODULE_ADD     ((unsigned char *)(&ucM5223X.SimBWTR)) // Backup Watchdog Timer Module
    #else
        #define EPORT_MODULE_1_ADD  ((unsigned char *)(&ucM5223X.SimEPORT1))// Edge Port Module 1
    #endif
    #define PIT_0_ADD           ((unsigned char *)(&ucM5223X.SimPIT0))   // Periodic Interrupt Timer 0
    #define PIT_1_ADD           ((unsigned char *)(&ucM5223X.SimPIT1))   // Periodic Interrupt Timer 1
    #define PIT_2_ADD           ((unsigned char *)(&ucM5223X.SimPIT2))   // Periodic Interrupt Timer 2
    #define PIT_3_ADD           ((unsigned char *)(&ucM5223X.SimPIT3))   // Periodic Interrupt Timer 3
    #define ADC_ADD             ((unsigned char *)(&ucM5223X.SimADC))    // ADC
    #define GPT_ADD             ((unsigned char *)(&ucM5223X.SimGTP))    // General Purpose Timer Module 0
    #if defined _M52XX_SDRAM                                             // {53}
        #define GPT1_ADD        ((unsigned char *)(&ucM5223X.SimGTP1))   // General Purpose Timer Module 1
    #else
        #define PWM_ADD         ((unsigned char *)(&ucM5223X.SimPWM))    // Pulse Width Modulation (PWM) Module {37}
    #endif
    #if defined _M5222X || defined _M5221X || defined _M5225X
        #define USB_BASE_ADD    ((unsigned char *)(&ucM5223X.SimUSB))    // USB-OTG module
    #endif
    #if !defined _M5222X && !defined _M5221X && !defined _M521XX
        #define CAN_BASE_ADD    ((unsigned char *)(&ucM5223X.SimCAN))    // FLEXCAN module
    #endif
    #define CFM_BASE_ADD        ((unsigned char *)(&ucM5223X.SimCFM))    // FLASH configuration module
    #define EPHY_BASE_ADD       ((unsigned char *)(&ucM5223X.SimEPHY))   // Ethernet Physical Transceiver
    #if defined RND_HW_SUPPORT
        #define RNG_BASE_ADD    ((unsigned char *)(&ucM5223X.SimRNG))    // Random Number Generator Accelerator
    #endif
    #define CFC_BASE_ADD        ((unsigned char *)(&ucM5223X.SimCFC))    // FLASH configuration field
    #if defined _M520X || defined _M523X
        #define SDRAM_MODULE_ADD ((unsigned char *)(&ucM5223X.SimSDRAMC))// SDRAM controller
    #endif
    #if defined _M520X                                                   // {54}
        #define SCM2_ADD        ((unsigned char *)(&ucM5223X.SimSCM2))   // System Control Module 2
    #endif
#elif defined _M520X                                                     // {54}
    #define SCM_ADD             (0xfc000000)                             // System Control Module
    #define CROSSBAR_ADD        (0xfc004000)                             // Cross-bar switch
    #define FLEXBUS_ADD         (0xfc008000)                             // FlexBus Module
    #define EMAC_BASE_ADD       (0xfc030000)                             // Fast Ethernet Controller
    #define SCM2_ADD            (0xfc040000)                             // System Control Module 2
    #define DMA_ADD             (0xfc044000)                             // eDMA Module (16 channels)
    #define INT_CTRL_0_ADD      (0xfc048000)                             // Interrupt Controller 0
    #define INT_CTRL_IACK_ADD   (0xfc054000)                             // Interrupt Controller IACK
    #define I2C_ADD             (0xfc058000)                             // I2C module
    #define QSPI_ADD            (0xfc05c000)                             // QSPI module
    #define UART_ADD            (0xfc060000)                             // UART Module (3 channels)
    #define DMA_TIMER_0_ADD     (0xfc070000)                             // DMA timer 0
    #define DMA_TIMER_1_ADD     (0xfc074000)                             // DMA timer 1
    #define DMA_TIMER_2_ADD     (0xfc078000)                             // DMA timer 2
    #define DMA_TIMER_3_ADD     (0xfc07c000)                             // DMA timer 3
    #define PIT_0_ADD           (0xfc080000)                             // Periodic Interrupt Timer 0
    #define PIT_1_ADD           (0xfc084000)                             // Periodic Interrupt Timer 1
    #define EPORT_MODULE_0_ADD  (0xfc088000)                             // Edge Port Module 0
    #define BWTR_MODULE_ADD     (0xfc08c000)                             // Backup Watchdog Timer Module
    #define CLOCK_MODULE_ADD    (0xfc090000)                             // Clock Module - PLL
    #define RESET_CTRL_ADD      (0xfc0a0000)                             // CCM, Reset, Power management
    #define PORT_MODULE_ADD     (0xfc0a4000)                             // Port Module
    #define SDRAM_MODULE_ADD    (0xfc0a8000)                             // SDRAM controller
#else
    #define SCM_ADD             (IPSBAR + 0x0)                           // System Control Module
    #if defined _M523X
        #define SDRAM_MODULE_ADD (IPSBAR + 0x040)                        // SDRAM controller
    #endif
    #if defined _M5225X || defined _M523X
        #define FLEXBUS_ADD     (IPSBAR + 0x080)                         // {32} FlexBus Module
    #endif
    #define DMA_ADD             (IPSBAR + 0x100)                         // DMA Module (4 channels)
    #define UART_ADD            (IPSBAR + 0x200)                         // UART Module (3 channels)
    #define I2C_ADD             (IPSBAR + 0x300)                         // I2C module
    #define QSPI_ADD            (IPSBAR + 0x340)                         // QSPI module
    #if defined _M5221X || defined _M5225X || defined _M521XX            // M5221X, M521XX and M5225X have 2 I2C bus modules {24}
        #define I2C_ADD_1       (IPSBAR + 0x380)                         // I2C module - 1
    #endif
    #ifndef _M5225X
        #define RTC_ADD         (IPSBAR + 0x3c0)                         // RTC module
    #endif
    #define DMA_TIMER_0_ADD     (IPSBAR + 0x400)                         // DMA timer 0
    #define DMA_TIMER_1_ADD     (IPSBAR + 0x440)                         // DMA timer 1
    #define DMA_TIMER_2_ADD     (IPSBAR + 0x480)                         // DMA timer 2
    #define DMA_TIMER_3_ADD     (IPSBAR + 0x4c0)                         // DMA timer 3
    #define INT_CTRL_0_ADD      (IPSBAR + 0xc00)                         // Interrupt Controller 0
    #define INT_CTRL_1_ADD      (IPSBAR + 0xd00)                         // Interrupt Controller 1
    #define EMAC_BASE_ADD       (IPSBAR + 0x1000)                        // Fast Ethernet Controller
    #define EMAC_FIFO_ADD       (IPSBAR + 0x1400)                        // Fast Ethernet Controller - FIFO
    #define PORT_MODULE_ADD     (IPSBAR + 0x100000)                      // Port Module
    #define RESET_CTRL_ADD      (IPSBAR + 0x110000)                      // Reset controller Module
    #define CLOCK_MODULE_ADD    (IPSBAR + 0x120000)                      // Clock Module
    #define EPORT_MODULE_0_ADD  (IPSBAR + 0x130000)                      // Edge Port Module 0
    #if defined BACKUP_WATCHDOG_TIMER_AVAILABLE
        #define BWTR_MODULE_ADD (IPSBAR + 0x140000)                      // Backup Watchdog Timer Module
    #else
        #define EPORT_MODULE_1_ADD  (IPSBAR + 0x140000)                  // Edge Port Module 1
    #endif
    #define PIT_0_ADD           (IPSBAR + 0x150000)                      // Periodic Interrupt Timer 0
    #define PIT_1_ADD           (IPSBAR + 0x160000)                      // Periodic Interrupt Timer 1
    #if defined _M5225X                                                  // {58}
        #define CAN_BASE_ADD    (IPSBAR + 0x170000)                      // CAN configuration module
    #else
        #define PIT_2_ADD       (IPSBAR + 0x170000)                      // Periodic Interrupt Timer 2
    #endif
    #if defined _M5225X
        #define RTC_ADD         (IPSBAR + 0x180000)                      // RTC module
    #else
        #define PIT_3_ADD       (IPSBAR + 0x180000)                      // Periodic Interrupt Timer 3
    #endif
    #define ADC_ADD             (IPSBAR + 0x190000)                      // ADC
    #define GPT_ADD             (IPSBAR + 0x1a0000)                      // General Purpose Timer Module 0
    #if defined _M52XX_SDRAM                                             // {53}
        #define GPT1_ADD        (IPSBAR + 0x1b0000)                      // General Purpose Timer Module 1
    #else
        #define PWM_ADD         (IPSBAR + 0x1b0000)                      // Pulse Width Modulation (PWM) Module {37}
    #endif
    #if defined _M5222X || defined _M5221X || defined _M5225X
        #define USB_BASE_ADD    (IPSBAR + 0x1c0000)                      // USB-OTG module
    #endif
    #if !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X // {58}
        #define CAN_BASE_ADD    (IPSBAR + 0x1c0000)                      // CAN configuration module
    #endif
    #define CFM_BASE_ADD        (IPSBAR + 0x1d0000)                      // FLASH configuration module
    #define EPHY_BASE_ADD       (IPSBAR + 0x1e0000)                      // Ethernet Physical Transceiver
    #if defined RND_HW_SUPPORT
        #define RNG_BASE_ADD    (IPSBAR + 0x1f0000)                      // Random Number Generator Accelerator
    #endif
    #define CFC_BASE_ADD        (FLASH_START_ADDRESS + 0x400)            // FLASH configuration field
#endif


#if defined _WINDOWS
    extern unsigned char uninitialisedRAM[4];
    #define BOOT_MAIL_BOX           (unsigned short *)&uninitialisedRAM[0]
    #define RANDOM_SEED_LOCATION    (unsigned short *)&uninitialisedRAM[2]

#else
    #define BOOT_MAIL_BOX           (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 2)) // {26}
    #define RANDOM_SEED_LOCATION    (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 4)) // {26} location of a long word which is never initialised and so has a random power on value
#endif


// System Control Module
//
#define MCF_SCM_IPSBAR      *(unsigned long *)(SCM_ADD + 0x0)            // IPS Base Address Register
#define MCF_SCM_RAMBAR      *(unsigned long *)(SCM_ADD + 0x8)            // Memory Base Address Register
  #define BACK_DOOR_ACCESS  0x00000200                                   // CPU space/Interrupt acknowledge cycle can access SRAM
  #define RAMBAR_ADD_MASK   0xffff0000
#if !defined _M52XX_SDRAM && !defined _M523X                             // {53}{60}
#define PPMRH               *(volatile unsigned long *)(SCM_ADD + 0x0c)  // Peripheral Power Management Register High {10}
  #define CDRNGA            0x00002000                                   // Clock disable - random number generator
  #define CDEPHY            0x00001000                                   // Clock disable - EPHY
  #define CDCFM             0x00000800                                   // Clock disable - Common Flash Module
  #define CDFCAN            0x00000400                                   // Clock disable - Flex CAN module
  #define CDPWM             0x00000200                                   // Clock disable - PWM
  #define CDGPT             0x00000100                                   // Clock disable - General Purpose Timer
  #define CDADC             0x00000080                                   // Clock disable - ADC
  #define CDPIT1            0x00000010                                   // Clock disable - PIT1
  #define CDPIT0            0x00000008                                   // Clock disable - PIT0
  #define CDEPORT           0x00000002                                   // Clock disable - EPORT
  #define CDPORTS           0x00000001                                   // Clock disable - Ports
#endif
#define CRSR                *(volatile unsigned char *)(SCM_ADD + 0x10)
  #define CRSR_RESET_EXT    0x80                                         // External device caused last reset (reset by writing '1')
#define CWCR                *(unsigned char *)(SCM_ADD + 0x11)           // Core Watchdog Control Register
  #define CWE               0x80                                         // Core watchdog enable
  #define WATCHDOG_2_SEC    0x30                                         // about 2 seconds at 60MHz bus
  #define CWTA              0x04                                         // Core Watchdog Transfer Acknowledge enable
  #define CWTAVA            0x02                                         // Core Watchdog Tranfer Acknowledge Valid
  #define CWTIF             0x01                                         // Core Watchdog Timer Interrupt Flag
#define LPICR               *(unsigned char *)(SCM_ADD + 0x12)           // {10} Low-Power Interrupt Control Register
  #define ENBSTOP           0x80                                         // Enable STOP mode
#define XLPM_IPL_ANY        0x00                                         // any interrupt level exits stop mode
#define XLPM_IPL_2_7        0x10                                         // above level 1 interrupt exist stop mode
#define XLPM_IPL_3_7        0x20                                         // above level 2 interrupt exist stop mode
#define XLPM_IPL_4_7        0x30                                         // above level 3 interrupt exist stop mode
#define XLPM_IPL_5_7        0x40                                         // above level 4 interrupt exist stop mode
#define XLPM_IPL_6_7        0x50                                         // above level 5 interrupt exist stop mode
#define XLPM_IPL_ONLY_7     0x60                                         // above level 6 interrupt exist stop mode
#define CWSR                *(volatile unsigned char *)(SCM_ADD + 0x13)
#define DMAREQC             *(unsigned long *)(SCM_ADD + 0x14)
  #define DMA_TIMER_0       0x4
  #define DMA_TIMER_1       0x5
  #define DMA_TIMER_2       0x6
  #define DMA_TIMER_3       0x7
  #define DMA_UART_0_RX     0x8
  #define DMA_UART_1_RX     0x9
  #define DMA_UART_2_RX     0xa
  #define DMA_UART_0_TX     0xc
  #define DMA_UART_1_TX     0xd
  #define DMA_UART_2_TX     0xe
  #define DMA_MASK          0x0f                                         // {22}

  #define DMAC_0_SHIFT      0
  #define DMAC_1_SHIFT      4
  #define DMAC_2_SHIFT      8
  #define DMAC_3_SHIFT      12
#if !defined _M52XX_SDRAM && !defined _M523X                             // {53}{60}
#define PPMRL               *(volatile unsigned long *)(SCM_ADD + 0x18)  // Peripheral Power Management Register Low {10}
  #define CDFEC0            0x00200000                                   // Clock disable - FEC module
  #define CDINTC1           0x00040000                                   // Clock disable - INTC1 module
  #define CDINTC0           0x00020000                                   // Clock disable - INTC0 module
  #define CDTMR3            0x00010000                                   // Clock disable - DTIM3 module
  #define CDTMR2            0x00008000                                   // Clock disable - DTIM2 module
  #define CDTMR1            0x00004000                                   // Clock disable - DTIM1 module
  #define CDTMR0            0x00002000                                   // Clock disable - DTIM0 module
  #define CDRTC             0x00001000                                   // Clock disable - RTC module
  #define CDI2C1            0x00000800                                   // Clock disable - I2C1 module
  #define CDQSPI            0x00000400                                   // Clock disable - QSPI module
  #define CDI2C             0x00000200                                   // Clock disable - I2C module
  #define CDUART2           0x00000080                                   // Clock disable - UART2 module
  #define CDUART1           0x00000040                                   // Clock disable - UART1 module
  #define CDUART0           0x00000020                                   // Clock disable - UART0 module
  #define CDDMA             0x00000010                                   // Clock disable - DMA module
  #define CDMFB             0x00000008                                   // Clock disable - Mini-FlexBus
  #define CDG               0x00000002                                   // Clock disable - Global off platform
#endif
#define MPARK               *(unsigned long *)(SCM_ADD + 0x1c)
#define MPR                 *(unsigned char *)(SCM_ADD + 0x20)
#if !defined _M52XX_SDRAM && !defined _M523X                             // {53}{60}
#define PPMRS               *(volatile unsigned char *)(SCM_ADD + 0x21)  // {10} Peripheral Power Management Set Register - write-only
  #define DISABLE_ALL_CLOCKS 64
#define PPMRC               *(volatile unsigned char *)(SCM_ADD + 0x22)  // {10} Peripheral Power Management Clear Register - write-only
  #define ENABLE_ALL_CLOCKS  64
#endif
#if !defined _M523X                                                      // {60}
#define IPSBMT              *(unsigned char *)(SCM_ADD + 0x23)
  #define IPS_BME           0x08                                         // Enable IPS Bus Timeout
  #define IPS_BMT_1024      0x00                                         // Bus Monitor Timeout 1024 bus cycles
  #define IPS_BMT_512       0x01                                         // Bus Monitor Timeout 512 bus cycles
  #define IPS_BMT_256       0x02                                         // Bus Monitor Timeout 256 bus cycles
  #define IPS_BMT_128       0x03                                         // Bus Monitor Timeout 128 bus cycles
  #define IPS_BMT_64        0x04                                         // Bus Monitor Timeout 64 bus cycles
  #define IPS_BMT_32        0x05                                         // Bus Monitor Timeout 32 bus cycles
  #define IPS_BMT_16        0x06                                         // Bus Monitor Timeout 16 bus cycles
  #define IPS_BMT_8         0x07                                         // Bus Monitor Timeout 8 bus cycles
#endif
#define PACR0               *(unsigned char *)(SCM_ADD + 0x24)
#define PACR1               *(unsigned char *)(SCM_ADD + 0x25)
#define PACR2               *(unsigned char *)(SCM_ADD + 0x26)
#define PACR3               *(unsigned char *)(SCM_ADD + 0x27)
#define PACR4               *(unsigned char *)(SCM_ADD + 0x28)
#define PACR5               *(unsigned char *)(SCM_ADD + 0x29)
#define PACR6               *(unsigned char *)(SCM_ADD + 0x2a)
#define PACR7               *(unsigned char *)(SCM_ADD + 0x2b)
#define PACR8               *(unsigned char *)(SCM_ADD + 0x2c)
  #define LOCK_DMA_ACCESS_CONTROL       0x08
  #define LOCK_SCM_ACCESS_CONTROL       0x80
  #define LOCK_UART0_ACCESS_CONTROL     0x80
  #define LOCK_UART1_ACCESS_CONTROL     0x08
  #define LOCK_UART2_ACCESS_CONTROL     0x80
  #define LOCK_I2C_ACCESS_CONTROL       0x80
  #define LOCK_QSPI_ACCESS_CONTROL      0x08
  #define LOCK_DTIM0_ACCESS_CONTROL     0x80
  #define LOCK_DTIM1_ACCESS_CONTROL     0x08
  #define LOCK_DTIM2_ACCESS_CONTROL     0x80
  #define LOCK_DTIM3_ACCESS_CONTROL     0x08
  #define LOCK_INTCO_ACCESS_CONTROL     0x80

  #define PACR_SCM                      PACR0
  #define PACR_DMA                      PACR1
  #define DMA_ACCESS_SHIFT              0
  #define PACR_UART0                    PACR2
  #define UART0_ACCESS_SHIFT            4
  #define PACR_UART1                    PACR2
  #define UART1_ACCESS_SHIFT            0
  #define PACR_UART2                    PACR3
  #define UART2_ACCESS_SHIFT            4
  #define PACR_I2C                      PACR4
  #define PACR_QSPI                     PACR4
  #define PACR_DTIM0                    PACR6
  #define PACR_DTIM1                    PACR6
  #define PACR_DTIM2                    PACR7
  #define PACR_DTIM3                    PACR7
  #define PACR_INTCO                    PACR8

#if defined _M52XX_SDRAM || defined _M520X || defined _M523X             // {53}{60} peripheral power not controlled in these devices
    #define POWER_UP(module)  
    #define POWER_DOWN(module)
#else
    #if defined _WINDOWS
    extern void _POWER_UP(unsigned char);                                // simulation routine
    extern void _POWER_DOWN(unsigned char);                              // simulation routine
    #else
    #define _POWER_UP(module)
    #define _POWER_DOWN(module)
    #endif
#define POWER_UP(module)                PPMRC = module; _POWER_UP(module)  // {11} power up a module (apply clock to it) 
#define POWER_DOWN(module)              PPMRS = module; _POWER_DOWN(module) // power down a module (disable clock to it)
#endif
  #define POWER_GLOBAL_OFF_PLATFORM     1
  #define POWER_MINI_FLEXBUS            3
  #define POWER_DMA                     4
  #define POWER_UART0                   5
  #define POWER_UART1                   6
  #define POWER_UART2                   7
  #define POWER_I2C                     9
  #define POWER_QSPI                    10
  #define POWER_I2C1                    11
  #define POWER_RTC                     12
  #define POWER_TMR0                    13
  #define POWER_TMR1                    14
  #define POWER_TMR2                    15
  #define POWER_TMR3                    16
  #define POWER_INTC0                   17
  #define POWER_INTC1                   18
  #define POWER_FEC0                    21
  #define POWER_PORTS                   32
  #define POWER_EPORT                   33
  #define POWER_PIT0                    35
  #define POWER_PIT1                    36
  #define POWER_ADC                     39
  #define POWER_GPT                     40
  #define POWER_PWM                     41
  #define POWER_FCAN                    42
  #define POWER_CFM                     43
  #define POWER_EPHY                    44
  #define POWER_USB_OTG                 44
  #define POWER_RNGA                    45


#define GPACR0              *(unsigned char *)(SCM_ADD + 0x30)
  #define LOCK_ACCESS_CONTROL           0x80                             // do not allow subsequent access
  #define SUP_FULL_ACCESS               0x0
  #define SUP_READ_ONLY                 0x1
  #define SUP_USER_READ_ONLY            0x2
  #define SUP_USER_FULL_ACCESS          0x4
  #define SUP_FULL_USER_READONLY_ACCESS 0x5
  #define NO_ACCESS                     0x7
#define GPACR1              *(unsigned char *)(SCM_ADD + 0x31)


// FlexBus Module                                                        {32}
//
#define CSAR0               *(unsigned long *)(FLEXBUS_ADD + 0x00)       // Chip Select Address Register - CS 0
  #define CSA_MASK          0xffff0000
#define CSMR0               *(unsigned long *)(FLEXBUS_ADD + 0x04)       // Chip Select Mask Register - CS 0
  #define BAM_MASK          0xffff0000                                   // Base Address mask
  #define FB_WP             0x00000010                                   // Write Protect
  #define FB_CS_VALID       0x00000001                                   // Chip Select Valid
#define CSCR0               *(unsigned long *)(FLEXBUS_ADD + 0x08)       // Chip Select Control Register - CS 0
  #define SECONDARY_WAIT_STATE_SHIFT 26                                  // Secondary Wait States
  #define SWSEN             0x00800000                                   // Secondary Wait State Enable
  #define ASET_FIRST_EDGE   0x00000000                                   // Address set up - assert CSx on first rising clock edge after address is asserted (reduced by 1 when FB_AA is set)
  #define ASET_SECOND_EDGE  0x00100000                                   // Address set up - assert CSx on second rising clock edge after address is asserted (reduced by 1 when FB_AA is set)
  #define ASET_THIRD_EDGE   0x00200000                                   // Address set up - assert CSx on third rising clock edge after address is asserted (reduced by 1 when FB_AA is set)
  #define ASET_FOURTH_EDGE  0x00300000                                   // Address set up - assert CSx on fourth rising clock edge after address is asserted (reduced by 1 when FB_AA is set)
  #define RDAH_1            0x00000000                                   // Read Address Hold - one cycle after CS deselect
  #define RDAH_2            0x00040000                                   // Read Address Hold - two cycles after CS deselect
  #define RDAH_3            0x00080000                                   // Read Address Hold - three cycles after CS deselect
  #define RDAH_4            0x000c0000                                   // Read Address Hold - four cycles after CS deselect
  #define WRAH_1            0x00000000                                   // Write Address Hold - one cycle after CS deselect
  #define WRAH_2            0x00010000                                   // Write Address Hold - two cycles after CS deselect
  #define WRAH_3            0x00020000                                   // Write Address Hold - three cycles after CS deselect
  #define WRAH_4            0x00030000                                   // Write Address Hold - four cycles after CS deselect
  #define WAIT_STATE_MASK   0x0000fc00                                   // Wait State mask
  #define WAIT_STATE_SHIFT  10                                           // Wait States
  #define FB_MUX            0x00000200                                   // Multiplexed Mode
  #define FB_AA             0x00000100                                   // Auto-acknowledge enable
  #define PORT_SIZE_32      0x00000000                                   // Port Size 32 bits wide
  #define PORT_SIZE_16      0x00000080                                   // Port Size 16 bits wide
  #define PORT_SIZE_8       0x00000040                                   // Port Size 8 bits wide
  #define BEM               0x00000020                                   // Byte Enable Mode (note that these are no longer noted in the user's manual)
  #define BSTR              0x00000010                                   // Burst Read Enable (")
  #define BSTW              0x00000008                                   // Burst Write Enable(")
  #define MAX_WAIT_STATES   63
  #define MAX_SEC_WAIT_STATES 31
#define CSAR1               *(unsigned long *)(FLEXBUS_ADD + 0x0c)       // Chip Select Address Register - CS 1
#define CSMR1               *(unsigned long *)(FLEXBUS_ADD + 0x10)       // Chip Select Mask Register - CS 1
#define CSCR1               *(unsigned long *)(FLEXBUS_ADD + 0x14)       // Chip Select Control Register - CS 1
#if defined _M520X                                                       // {54}
    #define CSAR2           *(unsigned long *)(FLEXBUS_ADD + 0x18)       // Chip Select Address Register - CS 2
    #define CSMR2           *(unsigned long *)(FLEXBUS_ADD + 0x1c)       // Chip Select Mask Register - CS 2
    #define CSCR2           *(unsigned long *)(FLEXBUS_ADD + 0x20)       // Chip Select Control Register - CS 2
    #define CSAR3           *(unsigned long *)(FLEXBUS_ADD + 0x24)       // Chip Select Address Register - CS 3
    #define CSMR3           *(unsigned long *)(FLEXBUS_ADD + 0x28)       // Chip Select Mask Register - CS 3
    #define CSCR3           *(unsigned long *)(FLEXBUS_ADD + 0x2c)       // Chip Select Control Register - CS 3
    #define CSAR4           *(unsigned long *)(FLEXBUS_ADD + 0x30)       // Chip Select Address Register - CS 4
    #define CSMR4           *(unsigned long *)(FLEXBUS_ADD + 0x34)       // Chip Select Mask Register - CS 4
    #define CSCR4           *(unsigned long *)(FLEXBUS_ADD + 0x38)       // Chip Select Control Register - CS 4
    #define CSAR5           *(unsigned long *)(FLEXBUS_ADD + 0x3c)       // Chip Select Address Register - CS 5
    #define CSMR5           *(unsigned long *)(FLEXBUS_ADD + 0x40)       // Chip Select Mask Register - CS 5
    #define CSCR5           *(unsigned long *)(FLEXBUS_ADD + 0x44)       // Chip Select Control Register - CS 5
#endif


// DMA Channel Modules
//
#define DMA_CHAN_0_OFFSET   0
#define DMA_SAR0            *(unsigned long *)(DMA_ADD + 0x00)           // DMA Source Address Register - DMA Channel 0
#define DMA_DAR0            *(unsigned long *)(DMA_ADD + 0x04)           // DMA Destination Address Register - DMA Channel 0
#define DMA_SR_BCR0         *(volatile unsigned long *)(DMA_ADD + 0x08)  // DMA Status register and Byte Count Register - DMA Channel 0
  #define DMA_ABORTED       0x80000000                                   // Pseudo bit - only for simulator use!!
  #define DSR_CE            0x40000000                                   // Configuration Error has occurred
  #define DSR_BES           0x20000000                                   // Termination due to bus error on source
  #define DSR_BED           0x10000000                                   // Termination due to bus error on destination
  #define DSR_REQ           0x04000000                                   // Transfer remaining and channel not selected
  #define DSR_BSY           0x02000000                                   // Channel busy
  #define DSR_DONE          0x01000000                                   // DMA Control terminated (cleared by writing '1')
  #define DMA_BYTE_COUNT_MASK 0x00ffffff

#define DMA_DCR0            *(volatile unsigned long *)(DMA_ADD + 0x0c)  // DMA Control Register - DMA Channel 0
  #define DCR_INT           0x80000000                                   // Enable internal interrupt signal
  #define DCR_EEXT          0x40000000                                   // Enable external initiation request
  #define DCR_CS            0x20000000                                   // Single read/write transfer per request
  #define DCR_AA            0x10000000                                   // Auto-align
  #define DCR_BWC_PRIORITY  0x00000000                                   // DMA has priority
  #define DCR_BWC_16K       0x02000000                                   // DMA has 16kbyte bandwidth
  #define DCR_BWC_1024K     0x0e000000                                   // DMA has 1024kbyte bandwidth
  #define DCR_SINC          0x00400000                                   // DMA Source Increment
  #define DCR_SSIZE_LONG    0x00000000                                   // DMA Source size - long
  #define DCR_SSIZE_BYTE    0x00100000                                   // DMA Source size - byte
  #define DCR_SSIZE_WORD    0x00200000                                   // DMA Source size - word
  #define DCR_SSIZE_LINE    0x00300000                                   // DMA Source size - 16 byte burst
  #define DCR_DINC          0x00080000                                   // DMA Destination Increment
  #define DCR_DSIZE_LONG    0x00000000                                   // DMA Destination size - long
  #define DCR_DSIZE_BYTE    0x00020000                                   // DMA Destination size - byte
  #define DCR_DSIZE_WORD    0x00040000                                   // DMA Destination size - word
  #define DCR_DSIZE_LINE    0x00060000                                   // DMA Destination size - 16 byte burst
  #define DCR_START         0x00010000                                   // DMA Start transfer

#define DMA_CHAN_1_OFFSET   0x10
#define DMA_SAR1            *(unsigned long *)(DMA_ADD + 0x10)           // DMA Source Address Register - DMA Channel 1
#define DMA_DAR1            *(unsigned long *)(DMA_ADD + 0x14)           // DMA Destination Address Register - DMA Channel 1
#define DMA_SR_BCR1         *(volatile unsigned long *)(DMA_ADD + 0x18)  // DMA Status register and Byte Count Register - DMA Channel 1
#define DMA_DCR1            *(volatile unsigned long *)(DMA_ADD + 0x1c)  // DMA Control Register Register - DMA Channel 1

#define DMA_CHAN_2_OFFSET   0x20
#define DMA_SAR2            *(unsigned long *)(DMA_ADD + 0x20)           // DMA Source Address Register - DMA Channel 2
#define DMA_DAR2            *(unsigned long *)(DMA_ADD + 0x24)           // DMA Destination Address Register - DMA Channel 2
#define DMA_SR_BCR2         *(volatile unsigned long *)(DMA_ADD + 0x28)  // DMA Status register and Byte Count Register - DMA Channel 2
#define DMA_DCR2            *(volatile unsigned long *)(DMA_ADD + 0x2c)  // DMA Control Register Register - DMA Channel 2

#define DMA_CHAN_3_OFFSET   0x30
#define DMA_SAR3            *(unsigned long *)(DMA_ADD + 0x30)           // DMA Source Address Register - DMA Channel 3
#define DMA_DAR3            *(unsigned long *)(DMA_ADD + 0x34)           // DMA Destination Address Register - DMA Channel 3
#define DMA_SR_BCR3         *(volatile unsigned long *)(DMA_ADD + 0x38)  // DMA Status register and Byte Count Register - DMA Channel 3
#define DMA_DCR3            *(volatile unsigned long *)(DMA_ADD + 0x3c)  // DMA Control Register Register - DMA Channel 3

#define DMA_REVMEMCPY_NOT_POSSIBLE                                       // since the DMA controller doesn't support decrementing the source/destination pointers it is not possible to implement uReverseMemcpy() controlled by it

// UART Modules
//
#define UMR1_2_0_OFFSET     0
#define UMR1_2_0            *(unsigned char *)(UART_ADD + 0x0)           // UART Mode register - UART 0
  #define RXRTS             0x80                                         // Mode register 1
  #define RXIRQ_FFULL       0x40
  #define UART_ERR          0x20
  #define UART_WITH_PARITY  0x00
  #define UART_FORCE_PARITY 0x08
  #define UART_NO_PARITY    0x10
  #define UART_MULTIDROP    0x18
  #define ODD_PARITY        0x04
  #define HIGH_PARITY       0x04
  #define ADD_CHAR          0x04
  #define UART_5_BIT        0x00
  #define UART_6_BIT        0x01
  #define UART_7_BIT        0x02
  #define UART_8_BIT        0x03

  #define UART_NORMAL_MODE  0x00                                         // Mode register 2
  #define UART_AUTO_ECHO    0x40
  #define UART_LOCAL_LOOP   0x80
  #define UART_REMOTE_LOOP  0xc0
  #define TXRTS             0x20
  #define TXCTS             0x10
  #define UART_ONE_STOP     0x07                                         // Valid for 6 to 8 bit chars
  #define UART_ONE_HALF_STOP 0x08                                        // Valid for 6 to 8 bit chars
  #define UART_TWO_STOP     0x0f                                         // Valid for 6 to 8 bit chars
#define USR_UCSR_0_OFFSET   0x04
#define USR_UCSR_0          *(volatile unsigned char *)(UART_ADD + 0x4)  // UART Status register (Read only) - UART 0 {3}
                                                                         // UART Clock Select register (Write only) - UART 0
  #define UART_RB           0x80                                         // Receive Break (read)
  #define UART_FE           0x40                                         // Framing Error
  #define UART_PE           0x20                                         // Parity Error
  #define UART_OE           0x10                                         // Overrun error
  #define UART_TEMP         0x08                                         // Transmitter Empty
  #define UART_TXRDY        0x04                                         // Transmitter Ready
  #define UART_FFULL        0x02                                         // FIFO Full
  #define UART_RXRDY        0x01                                         // Receiver Ready

  #define UART_RX_BUS_CLK   0xd0                                         // Rx Clock from prescaled internal bus (write)
  #define UART_RX_DTIN16_CLK 0xe0                                        // Rx Clock from DTIN pin / 16
  #define UART_RX_DTIN_CLK  0xf0                                         // Rx Clock from DTIN pin
  #define UART_TX_BUS_CLK   0x0d                                         // Tx Clock from prescaled internal bus
  #define UART_TX_DTIN16_CLK 0x0e                                        // Tx Clock from DTIN pin / 16
  #define UART_TX_DTIN_CLK  0x0f                                         // Tx Clock from DTIN pin
#define UCR_0_OFFSET        0x08
#define UCR_0               *(unsigned char *)(UART_ADD + 0x8)           // UART Command register (Write only) - UART 0
  #define UART_RESET_CMD_PTR 0x10                                        // Command block 1 (Multiple command blocks which do not interfere are allowed)
  #define UART_RESET_RX     0x20
  #define UART_RESET_TX     0x30
  #define UART_RESET_ERROR  0x40
  #define UART_RESET_BRK    0x50
  #define UART_START_BRK    0x60
  #define UART_STOP_BRK     0x70

  #define UART_TX_ENABLE    0x04                                         // Command block 2
  #define UART_TX_DISABLE   0x08

  #define UART_RX_ENABLE    0x01                                         // Command block 3
  #define UART_RX_DISABLE   0x02
#define UTB_RB_0_OFFSET     0x0c
#define UTB_RB_0            *(volatile unsigned char *)(UART_ADD + 0xc)  // UART Receive buffer (Read only) / Transmit buffer (Write only) - UART 0
#define UIPCR_UACR_0        *(volatile unsigned char *)(UART_ADD + 0x10) // UART Port Change Register  (Read only) / Aux. Control (Write only) - UART 
  #define UART_COS          0x10                                         // Change of State detected (Read)
  #define UART_CTS_IN       0x01                                         // Current CTS state deasserted

  #define UART_IEC          0x01                                         // Input enable control (Write)
#define UIMR_UISR_0_OFFSET  0x14
#define UIMR_UISR_0         *(volatile unsigned char *)(UART_ADD + 0x14) // UART Interrupt Status / Mask register  - UART 0
  #define UART_COS_MASK     0x80
  #define UART_BD_MASK      0x04
  #define UART_RXRDY_MASK   0x02
  #define UART_TXRDY_MASK   0x01
#define UBG1_0_OFFSET       0x18
#define UBG1_0              *(unsigned char *)(UART_ADD + 0x18)          // UART Baud Rate Generator register (MSB) (Write only) - UART 0
#define UBG2_0_OFFSET       0x1c
#define UBG2_0              *(unsigned char *)(UART_ADD + 0x1c)          // UART Baud Rate Generator register (LSB) (Write only) - UART 0
#define UIP_0               *(volatile unsigned char *)(UART_ADD + 0x34) // UART Input Port register (Read only) - UART 0
  #define UART_CTS_STATE    0x01                                         // Current CTS state logic '1'
#define UOP1_0              *(unsigned char *)(UART_ADD + 0x38)          // UART Output Port bit set register (Write only) - UART 0
#define UOP0_0              *(unsigned char *)(UART_ADD + 0x3c)          // UART Output Port bit reset register (Write only) - UART 0
  #define UART_SET_RESET_RTS 0x01                                        // Set or reset the state of RTS output

#if defined _M520X                                                       // {54}
    #define SCI_BLOCK_SIZE 0x4000                                        // add this to address second block
#else
    #define SCI_BLOCK_SIZE 0x40                                          // add this to address second block
#endif

#define UMR1_2_1            *(unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x00) // UART Mode register - UART 1
#define USR_UCSR_1          *(volatile unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x4) // UART Status register (Read only) - UART 1 {3}
#define UCR_1               *(unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x8)  // UART Command register (Write only) - UART 1
#define UTB_RB_1            *(volatile unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0xc) // UART Receive buffer (Read only) / Transmit buffer (Write only) - UART 1
#define UIPCR_UACR_1        *(volatile unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x10) // UART Port Change Register  (Read only) / Aux. Control (Write only) - UART 1
#define UIMR_UISR_1         *(volatile unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x14) // UART Interrupt Status / Mask register  - UART 1
#define UBG1_1              *(unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x18)  // UART Baud Rate Generator register (MSB) (Write only) - UART 1
#define UBG2_1              *(unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x1c)  // UART Baud Rate Generator register (LSB) (Write only) - UART 1
#define UIP_1               *(volatile unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x34) // UART Input Port register (Read only) - UART 1
#define UOP1_1              *(unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x38)  // UART Output Port bit set register (Write only) - UART 1
#define UOP0_1              *(unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + 0x3c)  // UART Output Port bit reset register (Write only) - UART 1

#define UMR1_2_2            *(unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x0) // UART Mode register - UART 2
#define USR_UCSR_2          *(volatile unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x4) // UART Status register (Read only) - UART 2 {3}
#define UCR_2               *(unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x08)// UART Command register (Write only) - UART 2
#define UTB_RB_2            *(volatile unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0xc) // UART Receive buffer (Read only) / Transmit buffer (Write only) - UART 2
#define UIPCR_UACR_2        *(volatile unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x10) // UART Port Change Register  (Read only) / Aux. Control (Write only) - UART 2
#define UIMR_UISR_2         *(volatile unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x14) // UART Interrupt Status / Mask register  - UART 2
#define UBG1_2              *(unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x18) // UART Baud Rate Generator register (MSB) (Write only) - UART 2
#define UBG2_2              *(unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x1c) // UART Baud Rate Generator register (LSB) (Write only) - UART 2
#define UIP_2               *(volatile unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x34) // UART Input Port register (Read only) - UART 2
#define UOP1_2              *(unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x38) // UART Output Port bit set register (Write only) - UART 2
#define UOP0_2              *(unsigned char *)(UART_ADD + (2 * SCI_BLOCK_SIZE) + 0x3c) // UART Output Port bit reset register (Write only) - UART 2

// I2C module
//
#define I2ADR               *(unsigned char *)(I2C_ADD + 0x0)            // I2C Address Register 
#define I2FDR               *(unsigned char *)(I2C_ADD + 0x4)            // I2C Frequency Divider Register 
#define I2C_CR_ADD          (volatile unsigned char *)(I2C_ADD + 0x8)    // I2C Control Register address {17}
#define I2CR                *(volatile unsigned char *)(I2C_ADD + 0x8)   // I2C Control Register 
  #define I2C_IEN           0x80                                         // I2C enable
  #define I2C_IIEN          0x40                                         // I2C interrupt enable
  #define I2C_MSTA          0x20                                         // Master mode
  #define I2C_MTX           0x10                                         // Transmit mode
  #define I2C_TXAK          0x08                                         // Transmit acknowledge enable
  #define I2C_RSTA          0x04                                         // Repeat start (always read as zero)
#define I2SR                *(volatile unsigned char *)(I2C_ADD + 0xc)   // I2C Status Register
  #define I2C_ICF           0x80                                         // transfer complete
  #define I2C_IAAS          0x40                                         // addressed as slave
  #define I2C_IBB           0x20                                         // {48} bus busy status
  #define I2C_IAL           0x10                                         // arbitration lost
  #define I2C_SRW           0x04                                         // slave transmit
  #define I2C_IIF           0x02                                         // I2C interrupt
  #define I2C_RXACK         0x01                                         // no ack signal detected
#define I2DR                *(volatile unsigned char *)(I2C_ADD + 0x10)  // I2C Data IO Register 

#if defined _M5221X || defined _M5225X || defined _M521XX                // M5221X, M521XX and M5225X have 2 I2C bus modules {24}
#define I2ADR_1             *(unsigned char *)(I2C_ADD_1 + 0x0)          // I2C Address Register - 1
#define I2FDR_1             *(unsigned char *)(I2C_ADD_1 + 0x4)          // I2C Frequency Divider Register - 1
#define I2C_CR_1_ADD        (volatile unsigned char *)(I2C_ADD_1 + 0x8)  // I2C Control Register address - 1 {17}
#define I2CR_1              *(volatile unsigned char *)(I2C_ADD_1 + 0x8) // I2C Control Register - 1
#define I2SR_1              *(volatile unsigned char *)(I2C_ADD_1 + 0xc) // I2C Status Register - 1
#define I2DR_1              *(volatile unsigned char *)(I2C_ADD_1 + 0x10)// I2C Data IO Register - 1
#endif

// QSPI module
//
#define QMR                 *(unsigned short *)(QSPI_ADD + 0x0)          // QSPI Mode register 
  #define QSPI_MSTR         0x8000                                       // Master operating mode (must be set)
  #define QSPI_DOHIE        0x4000                                       // Data Output Hi-Z between transfers
  #define QSPI_8BITS        0x2000                                       // 8 Bit transfer mode
  #define QSPI_10BITS       0x2800                                       // 10 bit transfer mode
  #define QSPI_11BITS       0x4c00                                       // 11 bit transfer mode
  #define QSPI_13BITS       0x3400                                       // 13 bit transfer mode
  #define QSPI_16BITS       0x0000                                       // 16 Bit transfer mode
  #define QSPI_CPOL         0x0200                                       // Clock polarity - inactive '1'
  #define QSPI_CPHA         0x0100                                       // Clock phase
  #define QSPI_BAUD_MASK    0x00ff                                       // Baud rate mask

#define QDLYR               *(unsigned short *)(QSPI_ADD + 0x4)          // QSPI Delay register
  #define QSPI_SPE          0x8000                                       // QSPI enable
#define QWR                 *(volatile unsigned short *)(QSPI_ADD + 0x8) // QSPI Wrap register 
  #define QSPI_HALT         0x8000                                       // halt transfers
  #define QSPI_WREN         0x4000                                       // wrap enable
  #define QSPI_WRTO         0x2000                                       // wrap to
  #define QSPI_CSIV         0x1000                                       // CS inactive level '1'
  #define QSPI_END_SHIFT    8
  #define QSPI_START_SHIFT  0
#define QIR                 *(volatile unsigned short *)(QSPI_ADD + 0xc) // QSPI Interrupt register 
  #define QSPI_SPIF         0x0001                                       // Finished flag
  #define QSPI_SPIFE        0x0100                                       // Interrupt enable
#define QAR                 *(volatile unsigned short *)(QSPI_ADD + 0x10)// QSPI Address register
  #define QSPI_TRANSMIT_RAM_0  0x00
  #define QSPI_TRANSMIT_RAM_1  0x01
  #define QSPI_TRANSMIT_RAM_2  0x02
  #define QSPI_TRANSMIT_RAM_3  0x03
  #define QSPI_TRANSMIT_RAM_4  0x04
  #define QSPI_TRANSMIT_RAM_5  0x05
  #define QSPI_TRANSMIT_RAM_6  0x06
  #define QSPI_TRANSMIT_RAM_7  0x07
  #define QSPI_TRANSMIT_RAM_8  0x08
  #define QSPI_TRANSMIT_RAM_9  0x09
  #define QSPI_TRANSMIT_RAM_10 0x0a
  #define QSPI_TRANSMIT_RAM_11 0x0b
  #define QSPI_TRANSMIT_RAM_12 0x0c
  #define QSPI_TRANSMIT_RAM_13 0x0d
  #define QSPI_TRANSMIT_RAM_14 0x0e
  #define QSPI_TRANSMIT_RAM_15 0x0f
  
  #define QSPI_RECEIVE_RAM_0   0x10
  #define QSPI_RECEIVE_RAM_1   0x11
  #define QSPI_RECEIVE_RAM_2   0x12
  #define QSPI_RECEIVE_RAM_3   0x13
  #define QSPI_RECEIVE_RAM_4   0x14
  #define QSPI_RECEIVE_RAM_5   0x15
  #define QSPI_RECEIVE_RAM_6   0x16
  #define QSPI_RECEIVE_RAM_7   0x17
  #define QSPI_RECEIVE_RAM_8   0x18
  #define QSPI_RECEIVE_RAM_9   0x19
  #define QSPI_RECEIVE_RAM_10  0x1a
  #define QSPI_RECEIVE_RAM_11  0x1b
  #define QSPI_RECEIVE_RAM_12  0x1c
  #define QSPI_RECEIVE_RAM_13  0x1d
  #define QSPI_RECEIVE_RAM_14  0x1e
  #define QSPI_RECEIVE_RAM_15  0x1f

  #define QSPI_COMMAND_RAM_0   0x20
  #define QSPI_COMMAND_RAM_1   0x21
  #define QSPI_COMMAND_RAM_2   0x22
  #define QSPI_COMMAND_RAM_3   0x23
  #define QSPI_COMMAND_RAM_4   0x24
  #define QSPI_COMMAND_RAM_5   0x25
  #define QSPI_COMMAND_RAM_6   0x26
  #define QSPI_COMMAND_RAM_7   0x27
  #define QSPI_COMMAND_RAM_8   0x28
  #define QSPI_COMMAND_RAM_9   0x29
  #define QSPI_COMMAND_RAM_10  0x2a
  #define QSPI_COMMAND_RAM_11  0x2b
  #define QSPI_COMMAND_RAM_12  0x2c
  #define QSPI_COMMAND_RAM_13  0x2d
  #define QSPI_COMMAND_RAM_14  0x2e
  #define QSPI_COMMAND_RAM_15  0x2f
#define QDR                 *(volatile unsigned short *)(QSPI_ADD + 0x14)  // QSPI Data register 

// Command RAM register defines
//
#define QSPI_CONT           0x8000                                       // CS continuous
#define QSPI_BITSE          0x4000                                       // Number of bits defined in QMR (otherwise 8)
#define QSPI_QUEUE_8BIT     0x0000                                       // 8 bit transfer
#define QSPI_DT             0x2000                                       // Delay after transfer 
#define QSPI_DSCK           0x1000                                       // Chip select delay enable
#define QSPI_CS_3           0x0800                                       // CS 3 state '1' during transfer
#define QSPI_CS_2           0x0400                                       // CS 2 state '1' during transfer
#define QSPI_CS_1           0x0200                                       // CS 1 state '1' during transfer
#define QSPI_CS_0           0x0100                                       // CS 0 state '1' during transfer

// RTC registers
//
#define HOURMIN             *(volatile unsigned long *)(RTC_ADD + 0x00)  // RTC hours and Minutes Counter Register 
  #define MINUTES_MASK      0x0000003f
  #define HOURS_MASK        0x00001f00
#define SECONDS             *(volatile unsigned long *)(RTC_ADD + 0x04)  // RTC Seconds Counter Register 
  #define SECONDS_MASK      0x0000003f
#define ALRM_HM             *(unsigned long *)(RTC_ADD + 0x08)           // RTC Hours and Minutes Alarm Register 
#define ALRM_SEC            *(unsigned long *)(RTC_ADD + 0x0c)           // RTC Seconds Alarm Register 
#define RTCCTL              *(volatile unsigned long *)(RTC_ADD + 0x10)  // RTC Control Register 
  #define RTC_ENABLE        0x00000080                                   // enable the RTC
  #define RTC_SWR           0x00000001                                   // Software Reset of RTC - doesn't modify RTC_ENABLE bit
#define RTCISR              *(volatile unsigned long *)(RTC_ADD + 0x14)  // RTC Interrupt Status Register 
  #define RTC_STOPWATCH     0x00000001                                   // Stopwatch timer out
  #define RTC_MINUTE_INT    0x00000002                                   // Minute interrupt
  #define RTC_ALARM_INT     0x00000004                                   // Alarm interrupt
  #define RTC_DAY_INT       0x00000008                                   // Day interrupt
  #define RTC_SECOND_INT    0x00000010                                   // Second interrupt
  #define RTC_HOUR_INT      0x00000020                                   // Hour interrupt
#define RTCIENR             *(unsigned long *)(RTC_ADD + 0x18)           // RTC Interrupt Enable Register 
#define STPWCH              *(volatile unsigned long *)(RTC_ADD + 0x1c)  // RTC Stopwatch Minutes Register 
#define DAYS                *(volatile unsigned short *)(RTC_ADD + 0x22) // RTC Days Register (only short word accesses allowed)
#define ALRM_DAY            *(unsigned long *)(RTC_ADD + 0x24)           // RTC Day Alarm Register 
  #define DAY_MASK          0x0000ffff
#if defined _M5221X || defined _M521XX || defined _M5225X                // Additional registers on these devices
#define RTCGOCU            *(unsigned long *)(RTC_ADD + 0x34)            // RTC General Oscillator Count Upper Register (these are 16 bit registers but they must be written as 32bit values)
#define RTCGOCL            *(unsigned long *)(RTC_ADD + 0x38)            // RTC General Oscillator Count Lower Register
#endif


// DMA Timers
//
#define DTMR0                *(unsigned short *)(DMA_TIMER_0_ADD + 0x0)  // DMA Timer 0 Mode Register (read/write)
  #define ENABLE_DMA_TIMER   0x0001
  #define BUS_CLOCK_DIRECT   0x0002
  #define BUS_CLOCK_16       0x0004
  #define CLOCK_DTIN         0x0006
  #define DMA_TIMER_STOP_COUNT_MASK 0x0006
  #define DMA_TIM_FREERUN    0x0000
  #define DMA_TIM_RESTART    0x0008
  #define ORRI               0x0010
  #define TOGGLE_OUTPUT      0x0020
  #define CAPTURE_RISING     0x0040
  #define CAPTURE_FALLING    0x0080
  #define CAPTURE_BOTH_EDGES 0x00c0
  #define PRESCALER_SHIFT    8
#define DTXMR0               *(unsigned char *)(DMA_TIMER_0_ADD + 0x2)   // DMA Timer 0 Extended Mode Register (read/write)
  #define DMA_TIMER_MODE16   0x0001
  #define DMA_TIMER_HALT     0x0040
  #define DMA_TIMER_DMAEN    0x0080
#define DTER0                *(volatile unsigned char *)(DMA_TIMER_0_ADD + 0x3) // DMA Timer 0 Event Register (read/write)
  #define DMA_TIM_EVENT_CAP  0x0001
  #define DMA_TIM_EVENT_REF  0x0002
#define DTRR0                *(unsigned long *)(DMA_TIMER_0_ADD + 0x4)   // DMA Timer 0 Reference Register (read/write)
#define DTCR0                *(volatile unsigned long *)(DMA_TIMER_0_ADD + 0x8) // DMA Timer 0 Capture Register (read/write)
#define DTCN0                *(volatile unsigned long *)(DMA_TIMER_0_ADD + 0xc) // DMA Timer 0 Count Register (read only)

#define DTMR1                *(unsigned short *)(DMA_TIMER_1_ADD + 0x0)  // DMA Timer 1 Mode Register (read/write)
#define DTXMR1               *(unsigned char *)(DMA_TIMER_1_ADD + 0x2)   // DMA Timer 1 Extended Mode Register (read/write)
#define DTER1                *(volatile unsigned char *)(DMA_TIMER_1_ADD + 0x3) // DMA Timer 1 Event Register (read/write)
#define DTRR1                *(unsigned long *)(DMA_TIMER_1_ADD + 0x4)   // DMA Timer 1 Reference Register (read/write)
#define DTCR1                *(volatile unsigned long *)(DMA_TIMER_1_ADD + 0x8) // DMA Timer 1 Capture Register (read/write)
#define DTCN1                *(volatile unsigned long *)(DMA_TIMER_1_ADD + 0xc) // DMA Timer 1 Count Register (read only)

#define DTMR2                *(unsigned short *)(DMA_TIMER_2_ADD + 0x0)  // DMA Timer 2 Mode Register (read/write)
#define DTXMR2               *(unsigned char *)(DMA_TIMER_2_ADD + 0x2)   // DMA Timer 2 Extended Mode Register (read/write)
#define DTER2                *(volatile unsigned char *)(DMA_TIMER_2_ADD + 0x3) // DMA Timer 2 Event Register (read/write)
#define DTRR2                *(unsigned long *)(DMA_TIMER_2_ADD + 0x4)   // DMA Timer 2 Reference Register (read/write)
#define DTCR2                *(volatile unsigned long *)(DMA_TIMER_2_ADD + 0x8) // DMA Timer 2 Capture Register (read/write)
#define DTCN2                *(volatile unsigned long *)(DMA_TIMER_2_ADD + 0xc) // DMA Timer 2 Count Register (read only)

#define DTMR3                *(unsigned short *)(DMA_TIMER_3_ADD + 0x0)  // DMA Timer 3 Mode Register (read/write)
#define DTXMR3               *(unsigned char *)(DMA_TIMER_3_ADD + 0x2)   // DMA Timer 3 Extended Mode Register (read/write)
#define DTER3                *(volatile unsigned char *)(DMA_TIMER_3_ADD + 0x3) // DMA Timer 3 Event Register (read/write)
#define DTRR3                *(unsigned long *)(DMA_TIMER_3_ADD + 0x4)   // DMA Timer 3 Reference Register (read/write)
  #define LOW_RES_MS         (BUS_CLOCK / (16*256) / 1000)               // slowest clock rate to timer, giving longest period but minimum resolution
  #define MED_RES_MS         (BUS_CLOCK / (16*16) / 1000)                // medium clock rate to timer, giving medium period and medium resolution
  #define HIGH_RES_MS        (BUS_CLOCK / 16 / 1000)                     // high clock rate to timer, giving minimum period and maximum resolution
#define DTCR3                *(volatile unsigned long *)(DMA_TIMER_3_ADD + 0x8) // DMA Timer 3 Capture Register (read/write)
#define DTCN3                *(volatile unsigned long *)(DMA_TIMER_3_ADD + 0xc) // DMA Timer 3 Count Register (read only)

typedef struct _PACK stDMA_TIMER                                          // {14}
{
    unsigned short          usDTMR;
    unsigned char           ucDTXMR;
    volatile unsigned char  vucDTER;
    unsigned long           ulDTRR;
    volatile unsigned long  vulDTCR;
    volatile unsigned long  vulDTCN;
    volatile unsigned long  vulRes[12];
} DMA_TIMER;

// Interrupt Controller - 0
//
#define IC_IPRH_0           *(unsigned long *)(INT_CTRL_0_ADD + 0x0)     // Interrupt Pending Register High
#define IC_IPRL_0           *(unsigned long *)(INT_CTRL_0_ADD + 0x4)     // Interrupt Pending Register Low
#define IC_IMRH_0           *(unsigned long *)(INT_CTRL_0_ADD + 0x8)     // Interrupt Mask Register High
#define IC_IMRL_0           *(unsigned long *)(INT_CTRL_0_ADD + 0xC)     // Interrupt Mask Register Low
#define IC_INTFRCH_0        *(unsigned long *)(INT_CTRL_0_ADD + 0x10)    // Interrupt Force Register High
#define IC_INTFRCL_0        *(unsigned long *)(INT_CTRL_0_ADD + 0x14)    // Interrupt Force Register Low

#if defined _M520X                                                       // {54}
      #define MASK_ALL_INT      0x00000000                               // dummy since not used
      #define IRQ1_PIF_INT_L    0x00000002
      #define IRQ4_PIF_INT_L    0x00000004
      #define IRQ7_PIF_INT_L    0x00000008
      #define PIT_0_PIF_INT_L   0x00000010
      #define PIT_1_PIF_INT_L   0x00000020
      #define DMA0_PIF_INT_L    0x00000100
      #define DMA1_PIF_INT_L    0x00000200
      #define DMA2_PIF_INT_L    0x00000300
      #define DMA3_PIF_INT_L    0x00000800
      #define DMA4_PIF_INT_L    0x00001000
      #define DMA5_PIF_INT_L    0x00002000
      #define DMA6_PIF_INT_L    0x00003000
      #define DMA7_PIF_INT_L    0x00008000
      #define DMA8_PIF_INT_L    0x00010000
      #define DMA9_PIF_INT_L    0x00020000
      #define DMA10_PIF_INT_L   0x00030000
      #define DMA11_PIF_INT_L   0x00080000
      #define DMA12_PIF_INT_L   0x00100000
      #define DMA13_PIF_INT_L   0x00200000
      #define DMA14_PIF_INT_L   0x00300000
      #define DMA15_PIF_INT_L   0x00800000
      #define DMAERR_PIF_INT_L  0x01000000
      #define SW_WDG_PIF_INT_L  0x02000000
      #define UART0_PIF_INT_L   0x04000000
      #define UART1_PIF_INT_L   0x08000000
      #define UART2_PIF_INT_L   0x10000000
      #define I2C_PIF_INT_L     0x40000000
      #define QSPI_PIF_INT_L    0x80000000

      #define DTIM0_PIF_INT_H   0x00000001
      #define DTIM1_PIF_INT_H   0x00000002
      #define DTIM2_PIF_INT_H   0x00000004
      #define DTIM3_PIF_INT_H   0x00000008

      #define FEC_TX_FRAME_PIF_INT_H 0x00000010
      #define FEC_TX_BUF_PIF_INT_H   0x00000020
      #define FEC_RX_FRAME_PIF_INT_H 0x00000100

      #define BUSERROR_PIF_INT_H  0x40000000
#elif defined _M523X                                                     // {60}
      #define MASK_ALL_INT      0x00000001
      #define IRQ1_PIF_INT_L    0x00000002
      #define IRQ2_PIF_INT_L    0x00000004
      #define IRQ3_PIF_INT_L    0x00000008
      #define IRQ4_PIF_INT_L    0x00000010
      #define IRQ5_PIF_INT_L    0x00000020
      #define IRQ6_PIF_INT_L    0x00000040
      #define IRQ7_PIF_INT_L    0x00000080
      #define SW_WDG_PIF_INT_L  0x00000100
      #define DMA0_PIF_INT_L    0x00000200
      #define DMA1_PIF_INT_L    0x00000400
      #define DMA2_PIF_INT_L    0x00000800
      #define DMA3_PIF_INT_L    0x00001000
      #define UART0_PIF_INT_L   0x00002000
      #define UART1_PIF_INT_L   0x00004000
      #define UART2_PIF_INT_L   0x00008000
      #define I2C_PIF_INT_L     0x00020000
      #define QSPI_PIF_INT_L    0x00040000
      #define DTIM0_PIF_INT_L   0x00080000
      #define DTIM1_PIF_INT_L   0x00100000
      #define DTIM2_PIF_INT_L   0x00200000
      #define DTIM3_PIF_INT_L   0x00400000
      #define FEC_TX_FRAME_PIF_INT_L 0x00800000
      #define FEC_TX_BUF_PIF_INT_L   0x01000000
      #define FEC_RX_FRAME_PIF_INT_L 0x08000000

      #define PIT_0_PIF_INT_H    0x00000010
      #define PIT_1_PIF_INT_H    0x00000020
      #define PIT_2_PIF_INT_H    0x00000040
      #define PIT_3_PIF_INT_H    0x00000080
      #define RNGA_INT_H         0x00000100
#else
      #define MASK_ALL_INT      0x00000001
      #define IRQ1_PIF_INT_L    0x00000002
      #define IRQ2_PIF_INT_L    0x00000004
      #define IRQ3_PIF_INT_L    0x00000008
      #define IRQ4_PIF_INT_L    0x00000010
      #define IRQ5_PIF_INT_L    0x00000020
      #define IRQ6_PIF_INT_L    0x00000040
      #define IRQ7_PIF_INT_L    0x00000080
      #define SW_WDG_PIF_INT_L  0x00000100
      #define DMA0_PIF_INT_L    0x00000200
      #define DMA1_PIF_INT_L    0x00000400
      #define DMA2_PIF_INT_L    0x00000800
      #define DMA3_PIF_INT_L    0x00001000
      #define UART0_PIF_INT_L   0x00002000
      #define UART1_PIF_INT_L   0x00004000
      #define UART2_PIF_INT_L   0x00008000
      #define I2C_PIF_INT_L     0x00020000
      #define QSPI_PIF_INT_L    0x00040000
      #define DTIM0_PIF_INT_L   0x00080000
      #define DTIM1_PIF_INT_L   0x00100000
      #define DTIM2_PIF_INT_L   0x00200000
      #define DTIM3_PIF_INT_L   0x00400000

    #if defined _M521X
      #define CAN_TX_INTS_L     0x7f800000
      #define CAN_RX_INTS_L     0x80000000
      #define CAN_RX_INTS_H     0x0000007f
      #define CAN_ERR_INT_H     0x00000080
      #define CAN_BOFF_INT_H    0x00000100
    #else
      #define FEC_TX_FRAME_PIF_INT_L 0x00800000
      #define FEC_TX_BUF_PIF_INT_L   0x01000000
      #define FEC_RX_FRAME_PIF_INT_L 0x08000000

      #define EPHY_PIF_INT_H     0x00000010
    #endif
    #if defined _M5225X
      #define RNGA_INT_H         0x00000100                              // {50}
    #endif
      #define GPTTOF_PIF_INT_H   0x00000200                              // {27}
      #define GPTPAIF_PIF_INT_H  0x00000400                              // {27}
      #define GPTPAOVF_PIF_INT_H 0x00000800                              // {27}
      #define GPTC0F_PIF_INT_H   0x00001000                              // {27}
      #define GPTC1F_PIF_INT_H   0x00002000                              // {27}
      #define GPTC2F_PIF_INT_H   0x00004000                              // {27}
      #define GPTC3F_PIF_INT_H   0x00008000                              // {27}
      #define ADCA_PIF_INT_H     0x00020000                              // {8}
      #define ADCB_PIF_INT_H     0x00040000                              // {8}
      #define ADCINT_PIF_INT_H   0x00080000                              // {8}
    #if !defined _M5225X                                                 // {50}
      #define RNGA_INT_H         0x00200000
    #endif
      #define USB_OTG_INT_H      0x00200000
      #define PIT_0_PIF_INT_H    0x00800000
      #define PIT_1_PIF_INT_H    0x01000000
      #define I2C_1_INT_H        0x40000000                              // {16} M5221X
      #define RTC_INT_H          0x80000000
#endif

#if defined _M520X                                                       // {54}
    #define IC_ICONFIG_0    *(unsigned short *)(INT_CTRL_0_ADD + 0x1a)   // Interrupt Configuration Register
    #define IC_SIMR_0       *(volatile unsigned char *)(INT_CTRL_0_ADD + 0x1c) // Set Interrupt Mask Register (write-only)
    #define IC_CIMR_0       *(volatile unsigned char *)(INT_CTRL_0_ADD + 0x1d) // Clear Interrupt Mask Register (write-only)
    #define IC_CLMASK_0     *(unsigned char *)(INT_CTRL_0_ADD + 0x1e)    // Current Level Mask Register
    #define IC_SLMASK_0     *(unsigned char *)(INT_CTRL_0_ADD + 0x1f)    // Saved Level Mask Register
#else
    #define IC_IRLR_0       *(unsigned char *)(INT_CTRL_0_ADD + 0x18)    // {51} Interrupt Request Level Register (read-only)
    #define IC_IACKLPR_0    *(unsigned char *)(INT_CTRL_0_ADD + 0x19)    // Interrupt Acknowledge Level and Priority Register (read-only)
#endif
#define IRQ_ICR_1_7_START    (unsigned char *)(INT_CTRL_0_ADD + 0x41)
#define IC_ICR_0_1          *(unsigned char *)(INT_CTRL_0_ADD + 0x41)    // Interrupt Control Register - IRQ1 [M5208 = IRQ1]
#define IC_ICR_0_2          *(unsigned char *)(INT_CTRL_0_ADD + 0x42)    // Interrupt Control Register - IRQ2 [M5208 = IRQ4]
#define IC_ICR_0_3          *(unsigned char *)(INT_CTRL_0_ADD + 0x43)    // Interrupt Control Register - IRQ3 [M5208 = IRQ7]
#define IC_ICR_0_4          *(unsigned char *)(INT_CTRL_0_ADD + 0x44)    // Interrupt Control Register - IRQ4 [M5208 = PIT0]
#define IC_ICR_0_5          *(unsigned char *)(INT_CTRL_0_ADD + 0x45)    // Interrupt Control Register - IRQ5 [M5208 = PIT1]
#define IC_ICR_0_6          *(unsigned char *)(INT_CTRL_0_ADD + 0x46)    // Interrupt Control Register - IRQ6 [M5208 = NA]
#define IC_ICR_0_7          *(unsigned char *)(INT_CTRL_0_ADD + 0x47)    // Interrupt Control Register - IRQ7 [M5208 = NA]
#define IC_ICR_0_8          *(unsigned char *)(INT_CTRL_0_ADD + 0x48)    // Interrupt Control Register - SW Watchdog [M5208 = eDMA0]
#define IC_ICR_0_9          *(unsigned char *)(INT_CTRL_0_ADD + 0x49)    // Interrupt Control Register - DMA0 [M5208 = eDMA1]
#define IC_ICR_0_10         *(unsigned char *)(INT_CTRL_0_ADD + 0x4a)    // Interrupt Control Register - DMA1 [M5208 = eDMA]
#define IC_ICR_0_11         *(unsigned char *)(INT_CTRL_0_ADD + 0x4b)    // Interrupt Control Register - DMA2 [M5208 = eDMA3]
#define IC_ICR_0_12         *(unsigned char *)(INT_CTRL_0_ADD + 0x4c)    // Interrupt Control Register - DMA3 [M5208 = eDMA4]
#define IC_ICR_0_13         *(unsigned char *)(INT_CTRL_0_ADD + 0x4d)    // Interrupt Control Register - UART 0 [M5208 = eDMA5]
#define IC_ICR_0_14         *(unsigned char *)(INT_CTRL_0_ADD + 0x4e)    // Interrupt Control Register - UART 1 [M5208 = eDMA6]
#define IC_ICR_0_15         *(unsigned char *)(INT_CTRL_0_ADD + 0x4f)    // Interrupt Control Register - UART 2 [M5208 = eDMA7]
#define IC_ICR_0_16         *(unsigned char *)(INT_CTRL_0_ADD + 0x50)    // Interrupt Control Register  [M5208 = eDMA8]
#define IC_ICR_0_17         *(unsigned char *)(INT_CTRL_0_ADD + 0x51)    // Interrupt Control Register - I2C Interrupt [M5208 = eDMA9]
#define IC_ICR_0_18         *(unsigned char *)(INT_CTRL_0_ADD + 0x52)    // Interrupt Control Register - QSPI  [M5208 = eDMA10]
#if !defined _M520X
  #define IC_DTIM_0_ADD     (unsigned char *)(INT_CTRL_0_ADD + 0x53)     // {15}
#endif
#define IC_ICR_0_19         *(unsigned char *)(INT_CTRL_0_ADD + 0x53)    // Interrupt Control Register - DTIM 0 [M5208 = eDMA11]
#define IC_ICR_0_20         *(unsigned char *)(INT_CTRL_0_ADD + 0x54)    // Interrupt Control Register - DTIM 1 [M5208 = eDMA12]
#define IC_ICR_0_21         *(unsigned char *)(INT_CTRL_0_ADD + 0x55)    // Interrupt Control Register - DTIM 2 [M5208 = eDMA13]
#define IC_ICR_0_22         *(unsigned char *)(INT_CTRL_0_ADD + 0x56)    // Interrupt Control Register - DTIM 3 [M5208 = eDMA14]
#define IC_ICR_0_23         *(unsigned char *)(INT_CTRL_0_ADD + 0x57)    // Interrupt Control Register - FEC TX FRAME [M5208 = eDMA15]
#define IC_ICR_0_24         *(unsigned char *)(INT_CTRL_0_ADD + 0x58)    // Interrupt Control Register - FEC TX Buffer [M5208 = eDMAERR]
#define IC_ICR_0_25         *(unsigned char *)(INT_CTRL_0_ADD + 0x59)    // Interrupt Control Register - FEC TX underrun [M5208 = WDOG]
#define IC_ICR_0_26         *(unsigned char *)(INT_CTRL_0_ADD + 0x5a)    // Interrupt Control Register - FEC Collision Retry Limit [M5208 = UART0]
#define IC_ICR_0_27         *(unsigned char *)(INT_CTRL_0_ADD + 0x5b)    // Interrupt Control Register - FEC RX FRAME  [M5208 = UART1]
#define IC_ICR_0_28         *(unsigned char *)(INT_CTRL_0_ADD + 0x5c)    // Interrupt Control Register - FEC RX Buffer [M5208 = UART2]
#define IC_ICR_0_29         *(unsigned char *)(INT_CTRL_0_ADD + 0x5d)    // Interrupt Control Register - FEC MII [M5208 = NA]
#define IC_ICR_0_30         *(unsigned char *)(INT_CTRL_0_ADD + 0x5e)    // Interrupt Control Register - FEC Late Collision [M5208 = I2C]
#define IC_ICR_0_31         *(unsigned char *)(INT_CTRL_0_ADD + 0x5f)    // Interrupt Control Register - FEC Heart beat error [M5208 = QSPI]
#if defined _M520X
  #define IC_DTIM_0_ADD     (unsigned char *)(INT_CTRL_0_ADD + 0x60)
#endif
#define IC_ICR_0_32         *(unsigned char *)(INT_CTRL_0_ADD + 0x60)    // Interrupt Control Register - FEC Graceful stop complete [M5208 = DTIM0]
#define IC_ICR_0_33         *(unsigned char *)(INT_CTRL_0_ADD + 0x61)    // Interrupt Control Register - FEC Ethernet Bus Error [M5208 = DTIM1]
#define IC_ICR_0_34         *(unsigned char *)(INT_CTRL_0_ADD + 0x62)    // Interrupt Control Register - FEC Babbling TX Error [M5208 = DTIM2]
#define IC_ICR_0_35         *(unsigned char *)(INT_CTRL_0_ADD + 0x63)    // Interrupt Control Register - FEC Babbling RX Error [M5208 = DTIM3]
#define IC_ICR_0_36         *(unsigned char *)(INT_CTRL_0_ADD + 0x64)    // Interrupt Control Register - EPHY [M5208 = FEC TXF]
#define IC_ICR_0_37         *(unsigned char *)(INT_CTRL_0_ADD + 0x65)    // Interrupt Control Register  [M5208 = FEC TXB]
#define IC_ICR_0_38         *(unsigned char *)(INT_CTRL_0_ADD + 0x66)    // Interrupt Control Register  [M5208 = FEC UM]
#define IC_ICR_0_39         *(unsigned char *)(INT_CTRL_0_ADD + 0x67)    // Interrupt Control Register  [M5208 = FEC RL]
#define IC_ICR_0_40         *(unsigned char *)(INT_CTRL_0_ADD + 0x68)    // Interrupt Control Register - RNG in M5225X [M5208 = FEC RXF]
#define IC_ICR_GPT_ADD      (unsigned char *)(INT_CTRL_0_ADD + 0x69)     // {27}
#define IC_ICR_0_41         *(unsigned char *)(INT_CTRL_0_ADD + 0x69)    // Interrupt Control Register - Timer Overflow [M5208 = FEC TXB]
#define IC_ICR_0_42         *(unsigned char *)(INT_CTRL_0_ADD + 0x6a)    // Interrupt Control Register - Pulse Accumulator Input [M5208 = FEC MII]
#define IC_ICR_0_43         *(unsigned char *)(INT_CTRL_0_ADD + 0x6b)    // Interrupt Control Register - Pulse Accumulator Overflow [M5208 = FEC LC]
#define IC_ICR_0_44         *(unsigned char *)(INT_CTRL_0_ADD + 0x6c)    // Interrupt Control Register - Time Channel 0 [M5208 = FEC HBERR]
#define IC_ICR_0_45         *(unsigned char *)(INT_CTRL_0_ADD + 0x6d)    // Interrupt Control Register - Time Channel 1  [M5208 = FEC GRA]
#define IC_ICR_0_46         *(unsigned char *)(INT_CTRL_0_ADD + 0x6e)    // Interrupt Control Register - Time Channel 2  [M5208 = FEC EBERR]
#define IC_ICR_0_47         *(unsigned char *)(INT_CTRL_0_ADD + 0x6f)    // Interrupt Control Register - Time Channel 3  [M5208 = FEC BABT]
#define IC_ICR_0_48         *(unsigned char *)(INT_CTRL_0_ADD + 0x70)    // Interrupt Control Register - LVD [M5208 = FEC BABR]
#define IC_ICR_0_49         *(unsigned char *)(INT_CTRL_0_ADD + 0x71)    // Interrupt Control Register - ADCA Conversion complete
#define IC_ICR_0_50         *(unsigned char *)(INT_CTRL_0_ADD + 0x72)    // Interrupt Control Register - ADCB Conversion complete
#define IC_ICR_0_51         *(unsigned char *)(INT_CTRL_0_ADD + 0x73)    // Interrupt Control Register - ADC
#define IC_ICR_0_52         *(unsigned char *)(INT_CTRL_0_ADD + 0x74)    // Interrupt Control Register - PWM
#define IC_ICR_0_53         *(unsigned char *)(INT_CTRL_0_ADD + 0x75)    // Interrupt Control Register - RNGA / USB in M5222X/M5221X
#define IC_ICR_0_54         *(unsigned char *)(INT_CTRL_0_ADD + 0x76)    // Interrupt Control Register 
#define IC_ICR_0_55         *(unsigned char *)(INT_CTRL_0_ADD + 0x77)    // Interrupt Control Register - PIT0
#define IC_ICR_0_56         *(unsigned char *)(INT_CTRL_0_ADD + 0x78)    // Interrupt Control Register - PIT1
#define IC_ICR_0_57         *(unsigned char *)(INT_CTRL_0_ADD + 0x79)    // Interrupt Control Register 
#define IC_ICR_0_58         *(unsigned char *)(INT_CTRL_0_ADD + 0x7a)    // Interrupt Control Register 
#define IC_ICR_0_59         *(unsigned char *)(INT_CTRL_0_ADD + 0x7b)    // Interrupt Control Register - SGFM Buffer Empty
#define IC_ICR_0_60         *(unsigned char *)(INT_CTRL_0_ADD + 0x7c)    // Interrupt Control Register - SGFM Command Complete
#define IC_ICR_0_61         *(unsigned char *)(INT_CTRL_0_ADD + 0x7d)    // Interrupt Control Register - Protection violation
#define IC_ICR_0_62         *(unsigned char *)(INT_CTRL_0_ADD + 0x7e)    // Interrupt Control Register - Access Error (I2C 1 on M5221X) [M5208 = bus error interrupt]
#define IC_ICR_0_63         *(unsigned char *)(INT_CTRL_0_ADD + 0x7f)    // Interrupt Control Register - RTC
#if defined _M520X
  #define INTERRUPT_LEVEL_1    (0x01)
  #define INTERRUPT_LEVEL_2    (0x02)
  #define INTERRUPT_LEVEL_3    (0x03)
  #define INTERRUPT_LEVEL_4    (0x04)
  #define INTERRUPT_LEVEL_5    (0x05)
  #define INTERRUPT_LEVEL_6    (0x06)
  #define INTERRUPT_LEVEL_7    (0x07)
  #define LEVEL_MASK           0x07

  #define INTERRUPT_PRIORITY_0         0                                 // the M520X doesn't have programmable priorities (they are fixed depending on order in the interrupt list when multiple sources have the same level)
  #define INTERRUPT_PRIORITY_1         0                                 // these values are for code compatibility only
  #define INTERRUPT_PRIORITY_2         0
  #define INTERRUPT_PRIORITY_3         0
  #define INTERRUPT_PRIORITY_4         0
  #define INTERRUPT_PRIORITY_5         0
  #define INTERRUPT_PRIORITY_6         0
  #define INTERRUPT_PRIORITY_7         0
#else
  #define INTERRUPT_LEVEL_1    (0x01 << 3)
  #define INTERRUPT_LEVEL_2    (0x02 << 3)
  #define INTERRUPT_LEVEL_3    (0x03 << 3)
  #define INTERRUPT_LEVEL_4    (0x04 << 3)
  #define INTERRUPT_LEVEL_5    (0x05 << 3)
  #define INTERRUPT_LEVEL_6    (0x06 << 3)
  #define INTERRUPT_LEVEL_7    (0x07 << 3)
  #define LEVEL_MASK           (0x07 << 3)
  #define PRIORITY_MASK        0x07

  #define INTERRUPT_PRIORITY_0         0x0
  #define INTERRUPT_PRIORITY_1         0x1
  #define INTERRUPT_PRIORITY_2         0x2
  #define INTERRUPT_PRIORITY_3         0x3
  #define INTERRUPT_PRIORITY_4         0x4
  #define INTERRUPT_PRIORITY_5         0x5
  #define INTERRUPT_PRIORITY_6         0x6
  #define INTERRUPT_PRIORITY_7         0x7
  #define INTERRUPT_MID_POINT_PRIORITY 0x8                               // fixed level priority - only for IRQ1..IRQ7
#endif

#define IC_SWIACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xe0)    // {51} Software Interrupt Acknowledge (read-only)
#define IC_L1IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xe4)    // Level 1 Interrupt Acknowledge Register (read-only)
#define IC_L2IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xe8)    // Level 2 Interrupt Acknowledge Register (read-only)
#define IC_L3IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xec)    // Level 3 Interrupt Acknowledge Register (read-only)
#define IC_L4IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xf0)    // Level 4 Interrupt Acknowledge Register (read-only)
#define IC_L5IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xf4)    // Level 5 Interrupt Acknowledge Register (read-only)
#define IC_L6IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xf8)    // Level 6 Interrupt Acknowledge Register (read-only)
#define IC_L7IACK_0         *(unsigned char *)(INT_CTRL_0_ADD + 0xfc)    // Level 7 Interrupt Acknowledge Register (read-only)

// Software references used when setting up interrupts
//
#define QSPI_INTERRUPT      0
#define PORT_INTERRUPT      1
#define RTC_TIME_SETTING    2                                            // {1}
#define RTC_TICK_SEC        3
#define RTC_TICK_MIN        4
#define RTC_TICK_HOUR       5
#define RTC_TICK_DAY        6
#define RTC_ALARM_TIME      7
#define RTC_STOPWATCH_GO    8
#define RTC_GET_TIME        9
#define RTC_CONVERT_TO_UTC   0x0a                                        // {75}
#define RTC_CONVERT_FROM_UTC 0x0b                                        // {75}
#define RTC_GET_ALARM        0x0c

#define ADC_INTERRUPT       0x0d                                         // {8}
#define PIT_INTERRUPT       0x0e                                         // {12}
#define DMA_TIMER_INTERRUPT 0x0f                                         // {13}
#define GPT_TIMER_INTERRUPT 0x10                                         // {27}
#define PWM_INTERRUPT       0x11                                         // {57}
#define RTC_SET_UTC         0x20                                         // {75} use UTC rather than time and date when setting
#define RTC_INITIALISATION  0x40                                         // {67}
#define RTC_RETURNED_TIME   0x80
#define RTC_DISABLE         0x80

// Define interrupt setup structures to suit this processor
//
typedef struct stINTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set
  unsigned char    int_port_bit;                                         // The IRQ input number (1..15)
  unsigned char    int_port_sense;                                       // Level sensitive, falling, rising, both
} INTERRUPT_SETUP;

typedef struct stPIT_SETUP                                               // {12}
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set
  unsigned short   mode;                                                 // Periodic or single shot
  unsigned long    count_delay;                                          // The delay in us
} PIT_SETUP;

#define PIT_SINGLE_SHOT  0x0001                                          // configure for single interrupt
#define PIT_PERIODIC     0x0002                                          // configure for periodic interrupt
#define PIT_STOP         0x8000                                          // stop operation

#define PIT_US_DELAY(us_delay)  ((us_delay*(BUS_CLOCK/2000))/1000)       // macros for setting PIT interrupt times
#define PIT_MS_DELAY(ms_delay)  (ms_delay*(BUS_CLOCK/2000)) 
#define PIT_S_DELAY(s_delay)    (s_delay*(BUS_CLOCK/2))

typedef struct stDMA_TIMER_SETUP                                         // {13}
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set
  unsigned char    channel;                                              // DMA timer channel (0..3)
  unsigned char    clock_divider;                                        // 0..255 (1..256)
  unsigned long    mode;                                                 // DMA timer operation mode
  unsigned long    count_delay;                                          // count delay until interrupt (clocks or external pulses)
} DMA_TIMER_SETUP;

#define DMA_TIMER_INTERNAL_CLOCK         0x00000000                      // use internal clock source
#define DMA_TIMER_INTERNAL_CLOCK_DIV_16  0x00000001                      // use internal clock source / 16
#define DMA_TIMER_EXTERNAL_CLOCK         0x00000002                      // use external clock from DTIN
#define DMA_TIMER_FREERUN                0x00000000                      // time continues after match
#define DMA_TIMER_RESTART_ON_MATCH       0x00000004                      // time restarts on match
#define DMA_TIMER_SINGLE_SHOT_INTERRUPT  0x00000008                      // one interrupt required
#define DMA_TIMER_PERIODIC_INTERRUPT     0x00000010                      // periodic interrupt required
#define DMA_TIMER_TOGGLE_OUTPUT          0x00000020                      // configure DTOUT and toggle on match
#define DMA_TIMER_PULSE_OUTPUT           0x00000040                      // configure DTOUT and pulse low on match
#define DMA_TIMER_CAPTURE_FALLING        0x00000080                      // Configure DTIN and capture on falling edge
#define DMA_TIMER_CAPTURE_RISING         0x00000100                      // Configure DTIN and capture on rising edge
#define DMA_TIMER_BOTH_EDGES             (DMA_TIMER_CAPTURE_FALLING | DMA_TIMER_CAPTURE_RISING)
#define DMA_TIMER_ENABLE_CLOCK_DIVIDER   0x00000200                      // take clock divide value from clock_divider
#define DMA_TIMER_PERIODIC_DMA           0x00000400                      // periodic DMA rather than interrupt
#define DMA_TIMER_SINGLE_SHOT_DMA        0x00000800                      // single-shot DMA rather than interrupt
#define DMA_TIMER_CAPTURE_INTERRUPT      0x00001000                      // {68} interrupt on capture events
#define DMA_TIMER_SINGLE_CAPTURE_INTERRUPT 0x00002000                    // {68} interrupt on single capture event
#define DMA_TIMER_STOP                   0x80000000                      // stop timer operation

#if defined _M520X
    #define DMA_TIMER_US_DELAY(div, prescale, us_delay)  ((us_delay*(BUS_CLOCK/2000/div/prescale))/1000)  // macros for setting DMA interrupt times
    #define DMA_TIMER_MS_DELAY(div, prescale, ms_delay)  ((ms_delay*(BUS_CLOCK/2000/div/prescale)))  
    #define DMA_TIMER_S_DELAY(div,  prescale, s_delay)   ((s_delay*(BUS_CLOCK/2/div/prescale)))  
#else
    #define DMA_TIMER_US_DELAY(div, prescale, us_delay)  ((us_delay*(BUS_CLOCK/1000/div/prescale))/1000)  // macros for setting DMA interrupt times
    #define DMA_TIMER_MS_DELAY(div, prescale, ms_delay)  ((ms_delay*(BUS_CLOCK/1000/div/prescale)))  
    #define DMA_TIMER_S_DELAY(div,  prescale, s_delay)   ((s_delay*(BUS_CLOCK/div/prescale)))  
#endif

typedef struct stGPTIMER_SETUP                                           // {27}
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set
  unsigned char    channel;                                              // GTP timer channel (0..3)
  unsigned long    mode;                                                 // GTP timer operation mode
  unsigned long   *capture_list;                                         // pointer to a capture list
  unsigned short   usCaptureCount;                                       // the number of captures values to be recorded
} GPTIMER_SETUP;

//#define GPT_PRESCALE_1                 0x00000000                      // any changes to clock prescaler only take place when the present prescaled counter overruns to 0
//#define GPT_PRESCALE_2                 0x00000001                      // the prescale values correspond directly to the regsiter values
//#define GPT_PRESCALE_4                 0x00000002
//#define GPT_PRESCALE_8                 0x00000003
//#define GPT_PRESCALE_16                0x00000004
//#define GPT_PRESCALE_32                0x00000005
//#define GPT_PRESCALE_64                0x00000006
//#define GPT_PRESCALE_128               0x00000007
#define GPT_NO_CLOCK_CONFIGURATION       0x00000008
#define GPT_INTERNAL_CLOCK               0x00000000
#define GPT_PACNT_CLOCK                  0x00000010
#define GPT_PACNT_256_CLOCK              0x00000020
#define GPT_PACNT_65536_CLOCK            0x00000030
#define GPT_CAPTURE_FALLING_EDGE         0x00000200                      // falling and rising edges can both be set together
#define GPT_CAPTURE_RISING_EDGE          0x00000400
#define ENABLE_INPUT_PULLUP              0x00001000
#define DISABLE_INPUT_PULLUP             0x00000000

typedef struct stPWM_INTERRUPT_SETUP                                     // {57}
{
  void ( *int_handler )( void );                                         // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    pwm_reference;                                        // PWM channel to be used
  unsigned char    pwm_value;                                            // PWM percentage value
  unsigned char    pwm_frequency;                                        // base frequency
  unsigned char    pwm_mode;                                             // PWM mode of operation
} PWM_INTERRUPT_SETUP;

#define PWM_PRESCALER_128       0x07
#define PWM_PRESCALER_64        0x06
#define PWM_PRESCALER_32        0x05
#define PWM_PRESCALER_16        0x04
#define PWM_PRESCALER_8         0x03
#define PWM_PRESCALER_4         0x02
#define PWM_PRESCALER_2         0x01
#define PWM_PRESCALER_0         0x00

#define PWM_SCALED_CLOCK_INPUT  0x08
#define PWM_CENTER_ALIGNED      0x10
#define PWM_16_BIT_MODE         0x20
#define PWM_PCM_MODE            0x40
#define PWM_POLARITY            0x80

#define _PWM_PERCENT(percent_pwm, frequency_value)       (unsigned char)((frequency_value * percent_pwm)/100)
#define _PWM_TENTH_PERCENT(percent_pwm, frequency_value) (unsigned char)((frequency_value * percent_pwm)/1000)

#define PWM_US_DELAY(usec)           ((BUS_CLOCK/2/1000000)*usec) // {13}
#define PWM_FREQUENCY_VALUE(hertz)   (1000000 / (hertz))


typedef struct stADC_INTERRUPT_RESULT                                    // {8}
{
  signed short     sADC_value;                                           // present ADC sample value
  unsigned char    ucADC_channel;                                        // ADC channel status
  unsigned char    ucADC_flags;                                          // flags
} ADC_INTERRUPT_RESULT;

#define ADC_CHANNELS 8               

typedef struct stADC_RESULTS
{
  signed short     sADC_value[ADC_CHANNELS];                             // present ADC sample values
  unsigned char    ucADC_status[ADC_CHANNELS];                           // present ADC channel status
} ADC_RESULTS;

// Define interrupt setup structure to suit this processor
//
typedef struct stADC_SETUP
{
  void ( *int_handler )( ADC_INTERRUPT_RESULT* );                        // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set
  unsigned char    int_adc_bit;                                          // the ADC input number (0..7)
  unsigned char    int_adc_int_type;                                     // interrupt type (ADC_END_OF_SCAN_INT, ADC_ZERO_CROSSING_INT, ADC_LOW_LIMIT_INT, ADC_HIGH_LIMIT_INT)
  unsigned long    int_adc_mode;                                         // parallel, sequencial, single-ended, differential, start/stop, etc.
  unsigned short   int_adc_offset;                                       // offset for input
  unsigned short   int_high_level_trigger;                               // trigger when higher than this level
  unsigned short   int_low_level_trigger;                                // trigger when lower than this level
  unsigned char    int_adc_speed;                                        // operating speed value (register value)
  ADC_RESULTS      *int_adc_result;                                      // results structure to be filled
} ADC_SETUP;

#define ADC_FULL_SCALE                  (0xfff << 3)
#define ADC_VOLT                        (unsigned short)((ADC_FULL_SCALE * 1000) / ADC_REFERENCE_VOLTAGE)

#define ADC_ENABLE_INTS                 0x00
#define ADC_END_OF_SCAN_INT             0x01
#define ADC_ZERO_CROSSING_INT_POSITIVE  0x02
#define ADC_ZERO_CROSSING_INT_NEGATIVE  0x04
#define ADC_LOW_LIMIT_INT               0x08
#define ADC_SINGLE_SHOT_CROSSING_INT    0x10
#define ADC_HIGH_LIMIT_INT              0x20
#define ADC_SINGLE_SHOT_TRIGGER_INT     0x40
#define ADC_DISABLE_INTS                0x80

#define ADC_SEQUENTIAL_MODE             0x00000000
#define ADC_PARALLEL_MODE               0x00000001
#define ADC_SINGLE_ENDED_INPUT          0x00000000
#define ADC_DIFFERENTIAL                0x00000002
#define ADC_SINGLE_SHOT_MODE            0x00000000
#define ADC_LOOP_MODE                   0x00000004
#define ADC_TRIGGERED_MODE              0x00000008
#define ADC_START_OPERATION             0x00000010
#define ADC_STOP_OPERATION              0x00000020
#define ADC_SIMULT_MODE                 0x00000040
#define ADC_HIGH_REF_AN2                0x00000080
#define ADC_LOW_REF_AN6                 0x00000100
#define ADC_HIGH_REF_VRH                0x00000000
#define ADC_LOW_REF_VRL                 0x00000000
#define ADC_READ_ONLY                   0x00001000
#define ADC_GET_RESULT                  0x00002000
#define ADC_ALL_RESULTS                 0x00004000
#define ADC_CONFIGURE_ADC               0x00008000
#define ADC_AUTO_POWER_DOWN_MODE        0x00010000
#define ADC_AUTO_STANDBY_MODE           0x00020000
#define ADC_CONFIGURE_CHANNEL           0x00040000
#define ADC_DISABLE_CHANNEL             0x00080000
#define ADC_DISABLE_ADC                 0x00100000
#define ADC_SET_CHANNEL_OFFSET          0x00200000 

#define ADC_RESULT_NOT_READY            0x00
#define ADC_RESULT_VALID                0x01
#define ADC_INT_ZERO_CROSSING           0x02
#define ADC_INT_HIGH_LEVEL              0x04
#define ADC_INT_LOW_LEVEL               0x08

#define ADC_SAMPLING_SPEED(sample_rate) (((BUS_CLOCK/2)/sample_rate/2) - 1)  // {29}{46}

typedef struct stRTC_SETUP
{
    void (*int_handler)(void);                                           // interrupt handler to be configured
    unsigned short   usYear;                                             // {62}
    unsigned char    ucMonthOfYear;
    unsigned char    ucDayOfMonth;
    unsigned char    ucDayOfWeek;
    unsigned char    ucHours;
    unsigned char    ucMinutes;
    unsigned char    ucSeconds;
    unsigned char    command;                                            // identifier for command to perform
    unsigned long    ulLocalUTC;                                         // {75} optional UTC field

/*
    unsigned short   usDays;
    unsigned char    command;                                            // identifier for command to perform
    unsigned char    hours;
    unsigned char    ucMinutes;
    unsigned char    ucSeconds;  original */
} RTC_SETUP;

#define IRQ_LEVEL_SENSITIVE  0x00
#define IRQ_FALLING_EDGE     0x01
#define IRQ_RISING_EDGE      0x02
#define IRQ_BOTH_EDGES       0x03


// Interrupt Controller - 1
//
#define IC_IPRH_1           *(unsigned long *)(INT_CTRL_1_ADD + 0x0)     // Interrupt Pending Register High
#define IC_IPRL_1           *(unsigned long *)(INT_CTRL_1_ADD + 0x4)     // Interrupt Pending Register Low
#define IC_IMRH_1           *(unsigned long *)(INT_CTRL_1_ADD + 0x8)     // Interrupt Mask Register High
#define IC_IMRL_1           *(unsigned long *)(INT_CTRL_1_ADD + 0xC)     // Interrupt Mask Register Low
#define IC_INTFRCH_1        *(unsigned long *)(INT_CTRL_1_ADD + 0x10)    // Interrupt Force Register High
#define IC_INTFRCL_1        *(unsigned long *)(INT_CTRL_1_ADD + 0x14)    // Interrupt Force Register Low

#ifndef _M521X
  #define CAN_TX_INTS_L     0x0000ff00                                   // Interrupt controller 1
  #define CAN_RX_INTS_L     0x00ff0000                                   // Interrupt controller 1
  #define CAN_ERR_INT_L     0x01000000                                   // Interrupt controller 1
  #define CAN_BOFF_INT_L    0x02000000                                   // Interrupt controller 1

  #define IRQ8_PIF_INT_H    0x00000001
  #define IRQ9_PIF_INT_H    0x00000002
  #define IRQ10_PIF_INT_H   0x00000004
  #define IRQ11_PIF_INT_H   0x00000008
  #define IRQ12_PIF_INT_H   0x00000010
  #define IRQ13_PIF_INT_H   0x00000020
  #define IRQ14_PIF_INT_H   0x00000040
  #define IRQ15_PIF_INT_H   0x00000080
#endif

#define IC_ICR_1_1          *(unsigned char *)(INT_CTRL_1_ADD + 0x41)    // Interrupt Control Register 
#define IC_ICR_1_2          *(unsigned char *)(INT_CTRL_1_ADD + 0x42)    // Interrupt Control Register 
#define IC_ICR_1_3          *(unsigned char *)(INT_CTRL_1_ADD + 0x43)    // Interrupt Control Register 
#define IC_ICR_1_4          *(unsigned char *)(INT_CTRL_1_ADD + 0x44)    // Interrupt Control Register 
#define IC_ICR_1_5          *(unsigned char *)(INT_CTRL_1_ADD + 0x45)    // Interrupt Control Register 
#define IC_ICR_1_6          *(unsigned char *)(INT_CTRL_1_ADD + 0x46)    // Interrupt Control Register 
#define IC_ICR_1_7          *(unsigned char *)(INT_CTRL_1_ADD + 0x47)    // Interrupt Control Register 
#define IC_ICR_1_8          *(unsigned char *)(INT_CTRL_1_ADD + 0x48)    // Interrupt Control Register - FLEXCAN BUF0
#define IC_ICR_1_9          *(unsigned char *)(INT_CTRL_1_ADD + 0x49)    // Interrupt Control Register - FLEXCAN BUF1
#define IC_ICR_1_10         *(unsigned char *)(INT_CTRL_1_ADD + 0x4a)    // Interrupt Control Register - FLEXCAN BUF2
#define IC_ICR_1_11         *(unsigned char *)(INT_CTRL_1_ADD + 0x4b)    // Interrupt Control Register - FLEXCAN BUF3
#define IC_ICR_1_12         *(unsigned char *)(INT_CTRL_1_ADD + 0x4c)    // Interrupt Control Register - FLEXCAN BUF4
#define IC_ICR_1_13         *(unsigned char *)(INT_CTRL_1_ADD + 0x4d)    // Interrupt Control Register - FLEXCAN BUF5
#define IC_ICR_1_14         *(unsigned char *)(INT_CTRL_1_ADD + 0x4e)    // Interrupt Control Register - FLEXCAN BUF6
#define IC_ICR_1_15         *(unsigned char *)(INT_CTRL_1_ADD + 0x4f)    // Interrupt Control Register - FLEXCAN BUF7
#define IC_ICR_1_16         *(unsigned char *)(INT_CTRL_1_ADD + 0x50)    // Interrupt Control Register - FLEXCAN BUF8
#define IC_ICR_1_17         *(unsigned char *)(INT_CTRL_1_ADD + 0x51)    // Interrupt Control Register - FLEXCAN BUF9
#define IC_ICR_1_18         *(unsigned char *)(INT_CTRL_1_ADD + 0x52)    // Interrupt Control Register - FLEXCAN BUF10
#define IC_ICR_1_19         *(unsigned char *)(INT_CTRL_1_ADD + 0x53)    // Interrupt Control Register - FLEXCAN BUF11
#define IC_ICR_1_20         *(unsigned char *)(INT_CTRL_1_ADD + 0x54)    // Interrupt Control Register - FLEXCAN BUF12
#define IC_ICR_1_21         *(unsigned char *)(INT_CTRL_1_ADD + 0x55)    // Interrupt Control Register - FLEXCAN BUF13
#define IC_ICR_1_22         *(unsigned char *)(INT_CTRL_1_ADD + 0x56)    // Interrupt Control Register - FLEXCAN BUF14
#define IC_ICR_1_23         *(unsigned char *)(INT_CTRL_1_ADD + 0x57)    // Interrupt Control Register - FLEXCAN BUF15
#define IC_ICR_1_24         *(unsigned char *)(INT_CTRL_1_ADD + 0x58)    // Interrupt Control Register - FLEXCAN Error
#define IC_ICR_1_25         *(unsigned char *)(INT_CTRL_1_ADD + 0x59)    // Interrupt Control Register - FLEXCAN Bus Off
#define IC_ICR_1_26         *(unsigned char *)(INT_CTRL_1_ADD + 0x5a)    // Interrupt Control Register 
#define IC_ICR_1_27         *(unsigned char *)(INT_CTRL_1_ADD + 0x5b)    // Interrupt Control Register 
#define IC_ICR_1_28         *(unsigned char *)(INT_CTRL_1_ADD + 0x5c)    // Interrupt Control Register 
#define IC_ICR_1_29         *(unsigned char *)(INT_CTRL_1_ADD + 0x5d)    // Interrupt Control Register 
#define IC_ICR_1_30         *(unsigned char *)(INT_CTRL_1_ADD + 0x5e)    // Interrupt Control Register 
#define IC_ICR_1_31         *(unsigned char *)(INT_CTRL_1_ADD + 0x5f)    // Interrupt Control Register 
#define IRQ_ICR_8_15_START  (unsigned char *)(INT_CTRL_1_ADD + 0x60)     // {2}
#define IC_ICR_1_32         *(unsigned char *)(INT_CTRL_1_ADD + 0x60)    // Interrupt Control Register - Edge Port Flag 0
#define IC_ICR_1_33         *(unsigned char *)(INT_CTRL_1_ADD + 0x61)    // Interrupt Control Register - Edge Port Flag 1
#define IC_ICR_1_34         *(unsigned char *)(INT_CTRL_1_ADD + 0x62)    // Interrupt Control Register - Edge Port Flag 2
#define IC_ICR_1_35         *(unsigned char *)(INT_CTRL_1_ADD + 0x63)    // Interrupt Control Register - Edge Port Flag 3
#define IC_ICR_1_36         *(unsigned char *)(INT_CTRL_1_ADD + 0x64)    // Interrupt Control Register - Edge Port Flag 4
#define IC_ICR_1_37         *(unsigned char *)(INT_CTRL_1_ADD + 0x65)    // Interrupt Control Register - Edge Port Flag 5
#define IC_ICR_1_38         *(unsigned char *)(INT_CTRL_1_ADD + 0x66)    // Interrupt Control Register - Edge Port Flag 6
#define IC_ICR_1_39         *(unsigned char *)(INT_CTRL_1_ADD + 0x67)    // Interrupt Control Register - Edge Port Flag 7
#define IC_ICR_1_40         *(unsigned char *)(INT_CTRL_1_ADD + 0x68)    // Interrupt Control Register 
#define IC_ICR_1_41         *(unsigned char *)(INT_CTRL_1_ADD + 0x69)    // Interrupt Control Register 
#define IC_ICR_1_42         *(unsigned char *)(INT_CTRL_1_ADD + 0x6a)    // Interrupt Control Register 
#define IC_ICR_1_43         *(unsigned char *)(INT_CTRL_1_ADD + 0x6b)    // Interrupt Control Register 
#define IC_ICR_1_44         *(unsigned char *)(INT_CTRL_1_ADD + 0x6c)    // Interrupt Control Register 
#define IC_ICR_1_45         *(unsigned char *)(INT_CTRL_1_ADD + 0x6d)    // Interrupt Control Register 
#define IC_ICR_1_46         *(unsigned char *)(INT_CTRL_1_ADD + 0x6e)    // Interrupt Control Register 
#define IC_ICR_1_47         *(unsigned char *)(INT_CTRL_1_ADD + 0x6f)    // Interrupt Control Register 
#define IC_ICR_1_48         *(unsigned char *)(INT_CTRL_1_ADD + 0x70)    // Interrupt Control Register 
#define IC_ICR_1_49         *(unsigned char *)(INT_CTRL_1_ADD + 0x71)    // Interrupt Control Register 
#define IC_ICR_1_50         *(unsigned char *)(INT_CTRL_1_ADD + 0x72)    // Interrupt Control Register 
#define IC_ICR_1_51         *(unsigned char *)(INT_CTRL_1_ADD + 0x73)    // Interrupt Control Register 
#define IC_ICR_1_52         *(unsigned char *)(INT_CTRL_1_ADD + 0x74)    // Interrupt Control Register 
#define IC_ICR_1_53         *(unsigned char *)(INT_CTRL_1_ADD + 0x75)    // Interrupt Control Register 
#define IC_ICR_1_54         *(unsigned char *)(INT_CTRL_1_ADD + 0x76)    // Interrupt Control Register 
#define IC_ICR_1_55         *(unsigned char *)(INT_CTRL_1_ADD + 0x77)    // Interrupt Control Register 
#define IC_ICR_1_56         *(unsigned char *)(INT_CTRL_1_ADD + 0x78)    // Interrupt Control Register 
#define IC_ICR_1_57         *(unsigned char *)(INT_CTRL_1_ADD + 0x79)    // Interrupt Control Register 
#define IC_ICR_1_58         *(unsigned char *)(INT_CTRL_1_ADD + 0x7a)    // Interrupt Control Register 
#define IC_ICR_1_59         *(unsigned char *)(INT_CTRL_1_ADD + 0x7b)    // Interrupt Control Register 
#define IC_ICR_1_60         *(unsigned char *)(INT_CTRL_1_ADD + 0x7c)    // Interrupt Control Register 
#define IC_ICR_1_61         *(unsigned char *)(INT_CTRL_1_ADD + 0x7d)    // Interrupt Control Register 
#define IC_ICR_1_62         *(unsigned char *)(INT_CTRL_1_ADD + 0x7e)    // Interrupt Control Register 
#define IC_ICR_1_63         *(unsigned char *)(INT_CTRL_1_ADD + 0x7f)    // Interrupt Control Register

#if defined _M520X                                                       // {54}
    #define IRQ1_VECTOR         0x41
    #define IRQ4_VECTOR         0x42
    #define IRQ7_VECTOR         0x43
    #define PIT0_VECTOR         0x44
    #define PIT1_VECTOR         0x45
    #define DMA0_VECTOR         0x48
    #define DMA1_VECTOR         0x49
    #define DMA2_VECTOR         0x4a
    #define DMA3_VECTOR         0x4b
    #define DMA4_VECTOR         0x4c
    #define DMA5_VECTOR         0x4d
    #define DMA6_VECTOR         0x4e
    #define DMA7_VECTOR         0x4f
    #define DMA8_VECTOR         0x50
    #define DMA9_VECTOR         0x51
    #define DMA10_VECTOR        0x52
    #define DMA11_VECTOR        0x53
    #define DMA12_VECTOR        0x54
    #define DMA13_VECTOR        0x55
    #define DMA14_VECTOR        0x56
    #define DMA15_VECTOR        0x57
    #define DMAERR_VECTOR       0x58
    #define SW_WD_VECTOR        0x59
    #define UART0_VECTOR        0x5a
    #define UART1_VECTOR        0x5b
    #define UART2_VECTOR        0x5c
    #define I2C_VECTOR          0x5e
    #define QSPI_VECTOR         0x5f
    #define DTIM0_VECTOR        0x60
    #define DTIM1_VECTOR        0x61
    #define DTIM2_VECTOR        0x62
    #define DTIM3_VECTOR        0x63
    #define FEC_TX_F_VECTOR     0x64
    #define FEC_TX_B_VECTOR     0x65
    #define FEC_TX_UND_VECTOR   0x66
    #define FEC_COLL_VECTOR     0x67
    #define FEC_RX_F_VECTOR     0x68
    #define FEC_RX_B_VECTOR     0x69
    #define FEC_MII_VECTOR      0x6a
    #define FEC_LATE_VECTOR     0x6b
    #define FEC_HB_VECTOR       0x6c
    #define FEC_STOP_VECTOR     0x6d
    #define FEC_BERR_VECTOR     0x6e
    #define FEC_BAB_TX_VECTOR   0x6f
    #define FEC_BAB_RX_VECTOR   0x70
    #define BUSERROR_VECTOR     0x7e
#elif defined _M523X                                                     // {60}
    #define IRQ1_VECTOR         0x41
    #define IRQ2_VECTOR         0x42
    #define IRQ3_VECTOR         0x43
    #define IRQ4_VECTOR         0x44
    #define IRQ5_VECTOR         0x45
    #define IRQ6_VECTOR         0x46
    #define IRQ7_VECTOR         0x47

    #define SW_WD_VECTOR        0x48

    #define DMA0_VECTOR         0x49
    #define DMA1_VECTOR         0x4a
    #define DMA2_VECTOR         0x4b
    #define DMA3_VECTOR         0x4c
    #define UART0_VECTOR        0x4d
    #define UART1_VECTOR        0x4e
    #define UART2_VECTOR        0x4f

    #define I2C_VECTOR          0x51
    #define QSPI_VECTOR         0x52
    #define DTIM0_VECTOR        0x53
    #define DTIM1_VECTOR        0x54
    #define DTIM2_VECTOR        0x55
    #define DTIM3_VECTOR        0x56
    #define FEC_TX_F_VECTOR     0x57
    #define FEC_TX_B_VECTOR     0x58
    #define FEC_TX_UND_VECTOR   0x59
    #define FEC_COLL_VECTOR     0x5a
    #define FEC_RX_F_VECTOR     0x5b
    #define FEC_RX_B_VECTOR     0x5c
    #define FEC_MII_VECTOR      0x5d
    #define FEC_LATE_VECTOR     0x5e
    #define FEC_HB_VECTOR       0x5f
    #define FEC_STOP_VECTOR     0x60
    #define FEC_BERR_VECTOR     0x61
    #define FEC_BAB_TX_VECTOR   0x62
    #define FEC_BAB_RX_VECTOR   0x63
    #define PIT0_VECTOR         0x64
    #define PIT1_VECTOR         0x65
    #define PIT2_VECTOR         0x66
    #define PIT3_VECTOR         0x67
    #define RNGA_VECTOR         0x68

    #define CAN0_VECTOR         0x6b
    #define CAN1_VECTOR         0x6c
    #define CAN2_VECTOR         0x6d
    #define CAN3_VECTOR         0x6e
    #define CAN4_VECTOR         0x6f
    #define CAN5_VECTOR         0x70
    #define CAN6_VECTOR         0x71
    #define CAN7_VECTOR         0x72
    #define CAN8_VECTOR         0x73
    #define CAN9_VECTOR         0x74
    #define CAN10_VECTOR        0x75
    #define CAN11_VECTOR        0x76
    #define CAN12_VECTOR        0x77
    #define CAN13_VECTOR        0x78
	#define CAN14_VECTOR        0x79
    #define CAN15_VECTOR        0x7a
    #define CANERR_VECTOR       0x7b
    #define CANBOFF_VECTOR      0x7c
#elif defined _M521X
    #define IRQ1_VECTOR         0x41
    #define IRQ2_VECTOR         0x42
    #define IRQ3_VECTOR         0x43
    #define IRQ4_VECTOR         0x44
    #define IRQ5_VECTOR         0x45
    #define IRQ6_VECTOR         0x46
    #define IRQ7_VECTOR         0x47
    #define SW_WD_VECTOR        0x48
    #define DMA0_VECTOR         0x49
    #define DMA1_VECTOR         0x4a
    #define DMA2_VECTOR         0x4b
    #define DMA3_VECTOR         0x4c
    #define UART0_VECTOR        0x4d
    #define UART1_VECTOR        0x4e
    #define UART2_VECTOR        0x4f

    #define I2C_VECTOR          0x51
    #define QSPI_VECTOR         0x52
    #define DTIM0_VECTOR        0x53
    #define DTIM1_VECTOR        0x54
    #define DTIM2_VECTOR        0x55
    #define DTIM3_VECTOR        0x56
    #define CAN0_VECTOR         0x57
    #define CAN1_VECTOR         0x58
    #define CAN2_VECTOR         0x59
    #define CAN3_VECTOR         0x5a
    #define CAN4_VECTOR         0x5b
    #define CAN5_VECTOR         0x5c
    #define CAN6_VECTOR         0x5d
    #define CAN7_VECTOR         0x5e
    #define CAN8_VECTOR         0x5f
    #define CAN9_VECTOR         0x60
    #define CAN10_VECTOR        0x61
    #define CAN11_VECTOR        0x62
    #define CAN12_VECTOR        0x63
    #define CAN13_VECTOR        0x64
    #define CAN14_VECTOR        0x65
    #define CAN15_VECTOR        0x66
    #define CANERR_VECTOR       0x67
    #define CANBOFF_VECTOR      0x68
    #define GPT_TOF_VECTOR      0x69
    #define GPT_PAIF_VECTOR     0x6a
    #define GPT_PAOVF_VECTOR    0x6b
    #define GPT_C0F_VECTOR      0x6c
    #define GPT_C1F_VECTOR      0x6d
    #define GPT_C2F_VECTOR      0x6e
    #define GPT_C3F_VECTOR      0x6f
    #define PMM_LVDF_VECTOR     0x70
    #define ADC_ADCA_VECTOR     0x71
    #define ADC_ADCB_VECTOR     0x72
    #define ADC_ADCINT_VECTOR   0x73

    #define PIT0_VECTOR         0x77
    #define PIT1_VECTOR         0x78

    #define CFM_CBEIF_VECTOR    0x7b
    #define CFM_CCIF_VECTOR     0x7c
    #define CFM_PVIF_VECTOR     0x7d
    #define CFM_AEIF_VECTOR     0x7e
    #define PWM_INT_VECTOR      0x7f
#else
    #define IRQ1_VECTOR         0x41
    #define IRQ2_VECTOR         0x42
    #define IRQ3_VECTOR         0x43
    #define IRQ4_VECTOR         0x44
    #define IRQ5_VECTOR         0x45
    #define IRQ6_VECTOR         0x46
    #define IRQ7_VECTOR         0x47
    #define SW_WD_VECTOR        0x48
    #define DMA0_VECTOR         0x49
    #define DMA1_VECTOR         0x4a
    #define DMA2_VECTOR         0x4b
    #define DMA3_VECTOR         0x4c
    #define UART0_VECTOR        0x4d
    #define UART1_VECTOR        0x4e
    #define UART2_VECTOR        0x4f

    #define I2C_VECTOR          0x51
    #define QSPI_VECTOR         0x52
    #define DTIM0_VECTOR        0x53
    #define DTIM1_VECTOR        0x54
    #define DTIM2_VECTOR        0x55
    #define DTIM3_VECTOR        0x56
    #define FEC_TX_F_VECTOR     0x57
    #define FEC_TX_B_VECTOR     0x58
    #define FEC_TX_UND_VECTOR   0x59
    #define FEC_COLL_VECTOR     0x5a
    #define FEC_RX_F_VECTOR     0x5b
    #define FEC_RX_B_VECTOR     0x5c
    #define FEC_MII_VECTOR      0x5d
    #define FEC_LATE_VECTOR     0x5e
    #define FEC_HB_VECTOR       0x5f
    #define FEC_STOP_VECTOR     0x60
    #define FEC_BERR_VECTOR     0x61
    #define FEC_BAB_TX_VECTOR   0x62
    #define FEC_BAB_RX_VECTOR   0x63
    #define EPHY_VECTOR         0x64

    #define GPT_TOF_VECTOR      0x69
    #define GPT_PAIF_VECTOR     0x6a
    #define GPT_PAOVF_VECTOR    0x6b
    #define GPT_C0F_VECTOR      0x6c
    #define GPT_C1F_VECTOR      0x6d
    #define GPT_C2F_VECTOR      0x6e
    #define GPT_C3F_VECTOR      0x6f
    #define PMM_LVDF_VECTOR     0x70
    #define ADC_ADCA_VECTOR     0x71
    #define ADC_ADCB_VECTOR     0x72
    #define ADC_ADCINT_VECTOR   0x73
    #define PWM_INT_VECTOR      0x74
    #define RNGA_VECTOR         0x75
    #define USB_OTG_VECTOR      0x75                                     // M5222X / M5221X

    #define PIT0_VECTOR         0x77
    #define PIT1_VECTOR         0x78

    #define CFM_CBEIF_VECTOR    0x7b
    #define CFM_CCIF_VECTOR     0x7c
    #define CFM_PVIF_VECTOR     0x7d
    #define CFM_AEIF_VECTOR     0x7e
    #define I2C_1_VECTOR        0x7e                                     // {16} M5221X
    #define RTC_VECTOR          0x7f

    #define CAN0_VECTOR         0x88
    #define CAN1_VECTOR         0x89
    #define CAN2_VECTOR         0x8a
    #define CAN3_VECTOR         0x8b
    #define CAN4_VECTOR         0x8c
    #define CAN5_VECTOR         0x8d
    #define CAN6_VECTOR         0x8e
    #define CAN7_VECTOR         0x8f
    #define CAN8_VECTOR         0x90
    #define CAN9_VECTOR         0x91
    #define CAN10_VECTOR        0x92
    #define CAN11_VECTOR        0x93
    #define CAN12_VECTOR        0x94
    #define CAN13_VECTOR        0x95
    #define CAN14_VECTOR        0x96
    #define CAN15_VECTOR        0x97
    #define CANERR_VECTOR       0x98
    #define CANBOFF_VECTOR      0x99

    #define IRQ8_VECTOR         0xa0
    #define IRQ9_VECTOR         0xa1
    #define IRQ10_VECTOR        0xa2
    #define IRQ11_VECTOR        0xa3
    #define IRQ12_VECTOR        0xa4
    #define IRQ13_VECTOR        0xa5
    #define IRQ14_VECTOR        0xa6
    #define IRQ15_VECTOR        0xa7
#endif

// General Purpose Ports
//
#if defined _M52XX_SDRAM                                                 // {53}
    #define PORTA           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x00)    // 
    #define PORTB           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x01)    //
    #define PORTC           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x02)    // 
    #define PORTD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x03)    //  
    #define PORTE           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x04)    // 
    #define PORTF           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x05)    //
    #define PORTG           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x06)    // 
    #define PORTH           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x07)    //  
    #define PORTJ           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x08)    // 
    #define PORTDD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x09)    // 
    #ifndef _M521X_SDRAM
        #define PORTEH      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0a)    // 
    #endif
    #define PORTEL          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0b)    // 
    #define PORTAS          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0c)    // 
    #define PORTQS          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0d)    // 
    #define PORTSD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0e)    // 
    #define PORTTC          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0f)    // 
    #define PORTTD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x10)    // 
    #define PORTUA          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x11)    // 

    #define DDRA            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x14)    //
    #define DDRB            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x15)    //
    #define DDRC            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x16)    //
    #define DDRD            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x17)    //
    #define DDRE            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x18)    //
    #define DDRF            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x19)    //
    #define DDRG            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1a)    //
    #define DDRH            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1b)    //
    #define DDRJ            *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1c)    //
    #define DDRDD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1d)    //
    #ifndef _M521X_SDRAM
        #define DDREH       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1e)    //
    #endif
    #define DDREL           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1f)    //
    #define DDRAS           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x20)    //
    #define DDRQS           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x21)    //
    #define DDRSD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x22)    //
    #define DDRTC           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x23)    //
    #define DDRTD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x24)    //
    #define DDRUA           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x25)    //

    #define PORTIN_SETA     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x28)    //
    #define PORTIN_SETB     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x29)    //
    #define PORTIN_SETC     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2a)    //
    #define PORTIN_SETD     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2b)    //
    #define PORTIN_SETE     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2c)    //
    #define PORTIN_SETF     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2d)    //
    #define PORTIN_SETG     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2e)    //
    #define PORTIN_SETH     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2f)    //
    #define PORTIN_SETJ     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x30)    //
    #define PORTIN_SETDD    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x31)    //
    #ifndef _M521X_SDRAM
        #define PORTIN_SETEH *(volatile unsigned char *)(PORT_MODULE_ADD + 0x32)   //
    #endif
    #define PORTIN_SETEL    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x33)    //
    #define PORTIN_SETAS    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x34)    //
    #define PORTIN_SETQS    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x35)    //
    #define PORTIN_SETSD    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x36)    //
    #define PORTIN_SETTC    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x37)    //
    #define PORTIN_SETTD    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x38)    //
    #define PORTIN_SETUA    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x39)    //

    #define CLEARA          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3c)    // {71}
    #define CLEARB          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3d)    //
    #define CLEARC          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3e)    //
    #define CLEARD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3f)    //
    #define CLEARE          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x40)    //
    #define CLEARF          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x41)    //
    #define CLEARG          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x42)    //
    #define CLEARH          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x43)    //
    #define CLEARJ          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x44)    //
    #define CLEARDD         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x45)    //
    #ifndef _M521X_SDRAM
        #define CLEAREH     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x46)    //
    #endif
    #define CLEAREL         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x47)    //
    #define CLEARAS         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x48)    //
    #define CLEARQS         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x49)    //
    #define CLEARSD         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4a)    //
    #define CLEARTC         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4b)    //
    #define CLEARTD         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4c)    //
    #define CLEARUA         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4d)    //

    #define PBCDPAR         *(unsigned char *)(PORT_MODULE_ADD + 0x50)             //
      #define PBCDPAR_PCDPA 0x40
      #define PBCDPAR_PBPA  0x80
    #define PFPAR           *(unsigned char *)(PORT_MODULE_ADD + 0x51)             //
    #define PEPAR           *(unsigned short *)(PORT_MODULE_ADD + 0x52)            //
      #define PEPAR_SYNCB   0x0002
      #define PEPAR_TIP     0x0003
      #define PEPAR_SYNCA   0x0008
      #define PEPAR_TS      0x000c
      #define PEPAR_PEBIT2  0x0010
      #define PEPAR_PEBIT3  0x0040
      #define PEPAR_PEBIT4  0x0100
      #define PEPAR_PEBIT5  0x0400
      #define PEPAR_PEBIT6  0x1000
      #define PEPAR_PEBIT7  0x4000
    #define PJPAR           *(unsigned char *)(PORT_MODULE_ADD + 0x54)             //
    #define PSDPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x55)             //
      #define PSDPAR_PSDPA  0x80
    #define PASPAR          *(unsigned short *)(PORT_MODULE_ADD + 0x56)            //
      #define AS_FEC_MDIO_FUNCTION 0xc00
      #define AS_FEC_MDC_FUNCTION  0x300
    #define PEHLPAR         *(unsigned char *)(PORT_MODULE_ADD + 0x58)             //
      #define PEHLPAR_PELPA 0x40
      #define PEHLPAR_PEHPA 0x80
    #define PQSPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x59)             //
    #define PTCPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x5a)             //
    #define PTDPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x5b)             //
    #define PUAPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x5c)             //
#elif defined _M523X                                                               // {60}
    #define PORTADDR        *(unsigned char *)(PORT_MODULE_ADD + 0x00)             // Port Data Registers (PODR_xxx)
    #define PORTDATAH       *(unsigned char *)(PORT_MODULE_ADD + 0x01)             //
    #define PORTDATAL       *(unsigned char *)(PORT_MODULE_ADD + 0x02)             //
    #define PORTBUSCTL      *(unsigned char *)(PORT_MODULE_ADD + 0x03)             //
    #define PORTBS          *(unsigned char *)(PORT_MODULE_ADD + 0x04)             //
    #define PORTCS          *(unsigned char *)(PORT_MODULE_ADD + 0x05)             //
    #define PORTSDRAM       *(unsigned char *)(PORT_MODULE_ADD + 0x06)             //
    #define PORTFECI2C      *(unsigned char *)(PORT_MODULE_ADD + 0x07)             //
    #define PORTUARTH       *(unsigned char *)(PORT_MODULE_ADD + 0x08)             //
    #define PORTUARTL       *(unsigned char *)(PORT_MODULE_ADD + 0x09)             //
    #define PORTQSPI        *(unsigned char *)(PORT_MODULE_ADD + 0x0a)             //
    #define PORTTIMER       *(unsigned char *)(PORT_MODULE_ADD + 0x0b)             //
    #define PORTETPU        *(unsigned char *)(PORT_MODULE_ADD + 0x0c)             //

    #define DDRADDR         *(unsigned char *)(PORT_MODULE_ADD + 0x10)             // Port Data Direction Registers (PDDR_xxx)
    #define DDRDATAH        *(unsigned char *)(PORT_MODULE_ADD + 0x11)             //
    #define DDRDATAL        *(unsigned char *)(PORT_MODULE_ADD + 0x12)             //
    #define DDRBUSCTL       *(unsigned char *)(PORT_MODULE_ADD + 0x13)             //
    #define DDRBS           *(unsigned char *)(PORT_MODULE_ADD + 0x14)             //
    #define DDRCS           *(unsigned char *)(PORT_MODULE_ADD + 0x15)             //
    #define DDRSDRAM        *(unsigned char *)(PORT_MODULE_ADD + 0x16)             //
    #define DDRFECI2C       *(unsigned char *)(PORT_MODULE_ADD + 0x17)             //
    #define DDRUARTH        *(unsigned char *)(PORT_MODULE_ADD + 0x18)             //
    #define DDRUARTL        *(unsigned char *)(PORT_MODULE_ADD + 0x19)             //
    #define DDRQSPI         *(unsigned char *)(PORT_MODULE_ADD + 0x1a)             //
    #define DDRTIMER        *(unsigned char *)(PORT_MODULE_ADD + 0x1b)             //
    #define DDRETPU         *(unsigned char *)(PORT_MODULE_ADD + 0x1c)             //

    #define PORTIN_SETADDR   *(unsigned char *)(PORT_MODULE_ADD + 0x20)            // Port Pin Data/Set Data Registers (PPDSDR_xxx)
    #define PORTIN_SETDATAH  *(unsigned char *)(PORT_MODULE_ADD + 0x21)            //
    #define PORTIN_SETDATAL  *(unsigned char *)(PORT_MODULE_ADD + 0x22)            //
    #define PORTIN_SETBUSCTL *(unsigned char *)(PORT_MODULE_ADD + 0x23)            //
    #define PORTIN_SETBS     *(unsigned char *)(PORT_MODULE_ADD + 0x24)            //
    #define PORTIN_SETCS     *(unsigned char *)(PORT_MODULE_ADD + 0x25)            //
    #define PORTIN_SETSDRAM  *(unsigned char *)(PORT_MODULE_ADD + 0x26)            //
    #define PORTIN_SETFECI2C *(unsigned char *)(PORT_MODULE_ADD + 0x27)            //
    #define PORTIN_SETUARTH  *(unsigned char *)(PORT_MODULE_ADD + 0x28)            //
    #define PORTIN_SETUARTL  *(unsigned char *)(PORT_MODULE_ADD + 0x29)            //
    #define PORTIN_SETQSPI   *(unsigned char *)(PORT_MODULE_ADD + 0x2a)            //
    #define PORTIN_SETTIMER  *(unsigned char *)(PORT_MODULE_ADD + 0x2b)            //
    #define PORTIN_SETETPU   *(unsigned char *)(PORT_MODULE_ADD + 0x2c)            //

    #define PCLRR_ADDR      *(unsigned char *)(PORT_MODULE_ADD + 0x30)             // Port Clear Output Data Registers (PCLRR_xxx)
    #define PCLRR_DATAH     *(unsigned char *)(PORT_MODULE_ADD + 0x31)             //
    #define PCLRR_DATAL     *(unsigned char *)(PORT_MODULE_ADD + 0x32)             //
    #define PCLRR_BUSCTL    *(unsigned char *)(PORT_MODULE_ADD + 0x33)             //
    #define PCLRR_BS        *(unsigned char *)(PORT_MODULE_ADD + 0x34)             //
    #define PCLRR_CS        *(unsigned char *)(PORT_MODULE_ADD + 0x35)             //
    #define PCLRR_SDRAM     *(unsigned char *)(PORT_MODULE_ADD + 0x36)             //
    #define PCLRR_FECI2C    *(unsigned char *)(PORT_MODULE_ADD + 0x37)             //
    #define PCLRR_UARTH     *(unsigned char *)(PORT_MODULE_ADD + 0x38)             //
    #define PCLRR_UARTL     *(unsigned char *)(PORT_MODULE_ADD + 0x39)             //
    #define PCLRR_QSPI      *(unsigned char *)(PORT_MODULE_ADD + 0x3a)             //
    #define PCLRR_TIMER     *(unsigned char *)(PORT_MODULE_ADD + 0x3b)             //
    #define PCLRR_ETPU      *(unsigned char *)(PORT_MODULE_ADD + 0x3c)             //

    #define PAR_AD          *(unsigned char *)(PORT_MODULE_ADD + 0x40)             // Port Pin Assignment Registers (PAR_xxx)
      #define PAR_ADDR23    0x80
      #define PAR_ADDR22    0x40
      #define PAR_ADDR21    0x20
      #define PAR_DATAL     0x01
    #define PAR_BUSCTL      *(unsigned short *)(PORT_MODULE_ADD + 0x42)            //
    #define PAR_BS          *(unsigned char *)(PORT_MODULE_ADD + 0x44)             //
    #define PAR_CS          *(unsigned char *)(PORT_MODULE_ADD + 0x45)             //
    #define PAR_SDRAM       *(unsigned char *)(PORT_MODULE_ADD + 0x46)             //
    #define PAR_FECI2C      *(unsigned char *)(PORT_MODULE_ADD + 0x47)             //
      #define PAR_CAN0TX_F   0x02
      #define PAR_I2C_SDA_F0 0x03
      #define PAR_CAN0RX_F   0x08
      #define PAR_I2C_SCL_F0 0x0c
      #define PAR_I2C_SDA_F1 0x20
      #define PAR_FEC_EMDIO  0x30
      #define PAR_I2C_SCL_F1 0x80
      #define PAR_FEC_EMDC   0xc0
    #define PAR_UART        *(unsigned short *)(PORT_MODULE_ADD + 0x48)            //
      #define PAR_U0RTS     0x0001
      #define PAR_U0CTS     0x0002
      #define PAR_U0TXD     0x0004
      #define PAR_U0RXD     0x0008
      #define PAR_U2RTS     0x0020
      #define PAR_U1RTS     0x0030
      #define PAR_U2CTS     0x0080
      #define PAR_U1CTS     0x00c0
      #define PAR_CAN0TX    0x0200
      #define PAR_U1TXD     0x0300
      #define PAR_CAN0RX    0x0800
      #define PAR_U1RXD     0x0c00
      #define PAR_U2TXD     0x1000
      #define PAR_U2RXD     0x2000
      #define PAR_CAN1EN    0x4000
      #define PAR_DREQ2     0x8000
    #define PAR_QSPI        *(unsigned char *)(PORT_MODULE_ADD + 0x4a)             //
    #define PAR_TIMER       *(unsigned short *)(PORT_MODULE_ADD + 0x4c)            //
    #define PAR_ETPU        *(unsigned char *)(PORT_MODULE_ADD + 0x4e)             //

    #define DSCR_EIM        *(unsigned char *)(PORT_MODULE_ADD + 0x50)             // Drive Strength Control Registers
    #define DSCR_ETPU       *(unsigned char *)(PORT_MODULE_ADD + 0x51)             //
    #define DSCR_FECI2C     *(unsigned char *)(PORT_MODULE_ADD + 0x52)             //
    #define DSCR_UART       *(unsigned char *)(PORT_MODULE_ADD + 0x53)             //
    #define DSCR_QSPI       *(unsigned char *)(PORT_MODULE_ADD + 0x54)             //
    #define DSCR_TIMER      *(unsigned char *)(PORT_MODULE_ADD + 0x55)             //
#elif defined _M520X                                                               // {54}
    #define PORTBUSCTL      *(unsigned char *)(PORT_MODULE_ADD + 0x00)             // Port Data Registers
    #define PORTBE          *(unsigned char *)(PORT_MODULE_ADD + 0x01)             //
    #define PORTCS          *(unsigned char *)(PORT_MODULE_ADD + 0x02)             //
    #define PORTFECI2C      *(unsigned char *)(PORT_MODULE_ADD + 0x03)             //
    #define PORTQSPI        *(unsigned char *)(PORT_MODULE_ADD + 0x04)             //
    #define PORTTIMER       *(unsigned char *)(PORT_MODULE_ADD + 0x05)             //
    #define PORTUART        *(unsigned char *)(PORT_MODULE_ADD + 0x06)             //
    #define PORTFECH        *(unsigned char *)(PORT_MODULE_ADD + 0x07)             //
    #define PORTFECL        *(unsigned char *)(PORT_MODULE_ADD + 0x08)             //

    #define DDRBUSCTL       *(unsigned char *)(PORT_MODULE_ADD + 0x0c)             // Port Data Direction Registers
    #define DDRBE           *(unsigned char *)(PORT_MODULE_ADD + 0x0d)             //
    #define DDRCS           *(unsigned char *)(PORT_MODULE_ADD + 0x0e)             //
    #define DDRFECI2C       *(unsigned char *)(PORT_MODULE_ADD + 0x0f)             //
    #define DDRQSPI         *(unsigned char *)(PORT_MODULE_ADD + 0x10)             //
    #define DDRTIMER        *(unsigned char *)(PORT_MODULE_ADD + 0x11)             //
    #define DDRUART         *(unsigned char *)(PORT_MODULE_ADD + 0x12)             //
    #define DDRFECH         *(unsigned char *)(PORT_MODULE_ADD + 0x13)             //
    #define DDRFECL         *(unsigned char *)(PORT_MODULE_ADD + 0x14)             //

    #define PORTIN_SETCS     *(unsigned char *)(PORT_MODULE_ADD + 0x1a)            // Port Pin Data/Set Data Registers
    #define PORTIN_SETFECI2C *(unsigned char *)(PORT_MODULE_ADD + 0x1b)            //
    #define PORTIN_SETQSPI   *(unsigned char *)(PORT_MODULE_ADD + 0x1c)            //
    #define PORTIN_SETTIMER  *(unsigned char *)(PORT_MODULE_ADD + 0x1d)            //
    #define PORTIN_SETUART   *(unsigned char *)(PORT_MODULE_ADD + 0x1e)            //
    #define PORTIN_SETFECH   *(unsigned char *)(PORT_MODULE_ADD + 0x1f)            //
    #define PORTIN_SETFECL   *(unsigned char *)(PORT_MODULE_ADD + 0x20)            //

    #define PCLRR_BUSCTL    *(unsigned char *)(PORT_MODULE_ADD + 0x24)             // Port Clear Output Data Registers
    #define PCLRR_BE        *(unsigned char *)(PORT_MODULE_ADD + 0x25)             //
    #define PCLRR_CS        *(unsigned char *)(PORT_MODULE_ADD + 0x26)             //
    #define PCLRR_FECI2C    *(unsigned char *)(PORT_MODULE_ADD + 0x27)             //
    #define PCLRR_QSPI      *(unsigned char *)(PORT_MODULE_ADD + 0x28)             //
    #define PCLRR_TIMER     *(unsigned char *)(PORT_MODULE_ADD + 0x29)             //
    #define PCLRR_UART      *(unsigned char *)(PORT_MODULE_ADD + 0x2a)             //
    #define PCLRR_FECH      *(unsigned char *)(PORT_MODULE_ADD + 0x2b)             //
    #define PCLRR_FECL      *(unsigned char *)(PORT_MODULE_ADD + 0x2c)             //

    #define PAR_BUSCTL      *(unsigned char *)(PORT_MODULE_ADD + 0x30)             // Pin Assignment Registers
        #define PAR_OE          0x10
        #define PAR_TA          0x08
        #define PAR_RWB         0x04
        #define PAR_TS_DMA_ACK  0x02
        #define PAR_TS_FUNCTION 0x03
    #define PAR_BE          *(unsigned char *)(PORT_MODULE_ADD + 0x31)             //
        #define PAR_BE_BWE_0    0x01
        #define PAR_BE_BWE_1    0x02
        #define PAR_BE_BWE_2    0x03
        #define PAR_BE_BWE_3    0x04
    #define PAR_CS          *(unsigned char *)(PORT_MODULE_ADD + 0x32)             //
        #define PAR_SD_CS1      0x02
        #define PAR_FB_CS1      0x03
        #define PAR_FB_CS2      0x04
        #define PAR_FB_CS3      0x08
    #define PAR_FECI2C      *(unsigned char *)(PORT_MODULE_ADD + 0x33)             //
        #define PAR_SDA_U2RXD   0x01
        #define PAR_SDA_I2C_SDA 0x03
        #define PAR_SCL_U2TXD   0x04
        #define PAR_SCL_I2C_SCL 0x0c
        #define PAR_MDIO_U2RXD  0x10
        #define PAR_MDIO_I2C_SDA  0x20
        #define PAR_MDIO_FEC_MDIO 0x30
        #define PAR_MDC_U2TXD   0x40
        #define PAR_MDC_I2C_SCL 0x80
        #define PAR_MDC_FEC_MDC 0xc0
    #define PAR_QSPI        *(unsigned char *)(PORT_MODULE_ADD + 0x34)             //
        #define PAR_SCK_I2C_SCL     0x02
        #define PAR_SCK_QSPI_SCK    0x03
        #define PAR_DOUT_I2C_SDA    0x08
        #define PAR_DOUT_QSPI_DOUT  0x0c
        #define PAR_DIN_U2CTS       0x10
        #define PAR_DIN_DREQ0       0x20
        #define PAR_DIN_QSPI_DIN    0x30
        #define PAR_PCS2_U2RTS      0x40
        #define PAR_PCS2_DACK0      0x80
        #define PAR_PCS2_QSPI_PCS2  0xc0
    #define PAR_TIMER       *(unsigned char *)(PORT_MODULE_ADD + 0x35)             //
        #define PAR_T0IN_U2TXD      0x01
        #define PAR_T0IN_T0OUT      0x02
        #define PAR_T0IN            0x03
        #define PAR_T1IN_U2RXD      0x04
        #define PAR_T1IN_T1OUT      0x08
        #define PAR_T1IN            0x0c
        #define PAR_T2IN_U2RTS      0x10
        #define PAR_T2IN_T2OUT      0x20
        #define PAR_T2IN            0x30
        #define PAR_T3IN_U2CTS      0x40
        #define PAR_T3IN_T3OUT      0x80
        #define PAR_T3IN            0xc0
    #define PAR_UART        *(unsigned short *)(PORT_MODULE_ADD + 0x36)             //
        #define PAR_U0RXD           0x0001
        #define PAR_U0TXD           0x0002
        #define PAR_U0RTS_QSPI_PCS0 0x0004
        #define PAR_U0RTS_T0OUT     0x0008
        #define PAR_U0RTS           0x000c
        #define PAR_U0CTS_QSPI_PCS0 0x0010
        #define PAR_U0CTS_T0IN      0x0020
        #define PAR_U0CTS           0x0030
        #define PAR_U1RXD           0x0040
        #define PAR_U1TXD           0x0080
        #define PAR_U1RTS_QSPI_PCS1 0x0100
        #define PAR_U1RTS_T1OUT     0x0200
        #define PAR_U1RTS_U1RTS     0x0300
        #define PAR_U1CTS_QSPI_PCS1 0x0400
        #define PAR_U1CTS_T1IN      0x0800
        #define PAR_U1CTS_U1CTS     0x0c00
    #define PAR_FEC         *(unsigned char *)(PORT_MODULE_ADD + 0x38)             //
        #define PAR_U0CTS_U1CTS_U0RTS 0x01
        #define PAR_FEC_MII           0x03
        #define PAR_U1RTS             0x04
        #define PAR_FEC_7W            0x0c
    #define PAR_IRQ         *(unsigned char *)(PORT_MODULE_ADD + 0x39)             //
        #define PAR_IRQ4              0x01
    #define MSCR_FLEXBUS    *(unsigned char *)(PORT_MODULE_ADD + 0x3a)             // Mode Select Control Registers
    #define MSCR_SDRAM      *(unsigned char *)(PORT_MODULE_ADD + 0x3b)             //
    #define DSCR_I2C        *(unsigned char *)(PORT_MODULE_ADD + 0x3c)             // Drive Strength Control Registers
    #define DSCR_MISC       *(unsigned char *)(PORT_MODULE_ADD + 0x3d)             //
    #define DSCR_FEC        *(unsigned char *)(PORT_MODULE_ADD + 0x3e)             //
    #define DSCR_UART       *(unsigned char *)(PORT_MODULE_ADD + 0x3f)             //
    #define DSCR_QSPI       *(unsigned char *)(PORT_MODULE_ADD + 0x40)             //
#else
  #if defined _M521X
    #define PORTNQ          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x08)    // 
    #define PORTDD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x09)    //
    #define PORTAN          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0a)    // 
    #define PORTAS          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0b)    //  
    #define PORTQS          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0d)    // 
  #else                     // M5223X
    #if defined _M5225X                                                            // {24}
        #define PORTTE      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x00)    // 
        #define PORTTF      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x01)    //
        #define PORTTG      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x02)    // 
        #define PORTTH      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x03)    //
        #define PORTTI      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x04)    // 
        #define PORTTJ      *(volatile unsigned char *)(PORT_MODULE_ADD + 0x06)    //
    #endif
    #define PORTNQ          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x08)    // 
    #define PORTAN          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0a)    // 
    #define PORTAS          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0b)    // 
    #define PORTQS          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0c)    // 
  #endif
#define PORTTA              *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0e)    //
#define PORTTC              *(volatile unsigned char *)(PORT_MODULE_ADD + 0x0f)    //
  #if !defined _M5225X                                                             // {24}
    #define PORTTD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x10)    // 
  #endif
#define PORTUA              *(volatile unsigned char *)(PORT_MODULE_ADD + 0x11)    // 
#define PORTUB              *(volatile unsigned char *)(PORT_MODULE_ADD + 0x12)    // 
#define PORTUC              *(volatile unsigned char *)(PORT_MODULE_ADD + 0x13)    //
  #if !defined _M521X
    #define PORTDD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x14)    //
  #endif
  #if !defined _M521X && !defined _M5225X                                          // {24} 
    #define PORTLD          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x15)    // 
    #define PORTGP          *(volatile unsigned char *)(PORT_MODULE_ADD + 0x16)    // 
  #endif
  #if defined _M521X
    #define DDRNQ           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1c)    // {39}
    #define DDRDD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1d)    // 
    #define DDRAN           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1e)    // 
    #define DDRAS           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1f)    // 
    #define DDRQS           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x21)    // 
    #define DDRTA           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x22)    // 
    #define DDRTC           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x23)    //
    #define DDRTD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x24)    // 
    #define DDRUA           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x25)    // 
    #define DDRUB           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x26)    // 
    #define DDRUC           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x27)    //
    #define PORTIN_SETNQ    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x30)    //
    #define PORTIN_SETDD    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x31)    //  
    #define PORTIN_SETAN    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x32)    // 
    #define PORTIN_SETAS    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x33)    // 
    #define PORTIN_SETQS    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x35)    // 
    #define PORTIN_SETTA    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x36)    // 
    #define PORTIN_SETTC    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x37)    // 
    #define PORTIN_SETTD    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x38)    // 
    #define PORTIN_SETUA    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x39)    // 
    #define PORTIN_SETUB    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3a)    // 
    #define PORTIN_SETUC    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3b)    // 
    #define CLEARNQ         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x44)    // {71}
    #define CLEARDD         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x45)    //  
    #define CLEARAN         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x46)    // 
    #define CLEARAS         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x47)    // 
    #define CLEARQS         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x49)    // 
    #define CLEARTA         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4a)    // 
    #define CLEARTC         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4b)    // 
    #define CLEARTD         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4c)    // 
    #define CLEARUA         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4d)    // 
    #define CLEARUB         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4e)    // 
    #define CLEARUC         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4f)    // 
    #define PNQPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x50)    // 
    #define PDDPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x51)    // 
    #define PANPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x52)    // 
    #define PASPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x53)    // 
    #define PQSPAR          *(unsigned short *)(PORT_MODULE_ADD + 0x54)   // 
    #define PTAPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x56)    // 
    #define PTCPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x57)    // 
    #define PTDPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x58)    // 
    #define PUAPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x59)    // 
    #define PUBPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x5a)    // 
    #define PUCPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x5b)    // 
    #define PSRR            *(unsigned long *)(PORT_MODULE_ADD + 0x78)    // Pin Slew Rate Register
    #define PDSR            *(unsigned long *)(PORT_MODULE_ADD + 0x7c)    // Pin Drive Strength Register
  #else                     // M5223X
    #if defined _M5225X                                                           // {24}
        #define DDRTE       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x18)   // {39}
        #define DDRTF       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x19)   //
        #define DDRTG       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1a)   //
        #define DDRTH       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1b)   //
        #define DDRTI       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1c)   //
        #define DDRTJ       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x1e)   //
    #endif
    #define DDRNQ           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x20)   // 
    #define DDRAN           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x22)   // 
    #define DDRAS           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x23)   // 
    #define DDRQS           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x24)   // 
    #define DDRTA           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x26)   // 
    #define DDRTC           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x27)   // 
    #if !defined _M5225X                                                          // {24}
        #define DDRTD       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x28)   // 
    #endif
    #define DDRUA           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x29)   // 
    #define DDRUB           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2a)   // 
    #define DDRUC           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2b)   // 
    #define DDRDD           *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2c)   // 
    #if !defined _M5225X                                                          // {24}
        #define DDRLD       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2d)   // 
        #define DDRGP       *(volatile unsigned char *)(PORT_MODULE_ADD + 0x2e)   // 
    #endif
    #if defined _M5225X                                                           // {24}
        #define PORTIN_SETTE  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x30) // 
        #define PORTIN_SETTF  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x31) // 
        #define PORTIN_SETTG  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x32) // 
        #define PORTIN_SETTH  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x33) // 
        #define PORTIN_SETTI  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x34) // 
        #define PORTIN_SETTJ  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x36) // 
    #endif
    #define PORTIN_SETNQ    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x38)   // 
    #define PORTIN_SETAN    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3a)   // 
    #define PORTIN_SETAS    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3b)   // 
    #define PORTIN_SETQS    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3c)   // 
    #define PORTIN_SETTA    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3e)   // 
    #define PORTIN_SETTC    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x3f)   // 
    #if !defined _M5225X                                                          // {24}
        #define PORTIN_SETTD  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x40) // 
    #endif
    #define PORTIN_SETUA    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x41)   // 
    #define PORTIN_SETUB    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x42)   // 
    #define PORTIN_SETUC    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x43)   //
    #define PORTIN_SETDD    *(volatile unsigned char *)(PORT_MODULE_ADD + 0x44)   //
    #if !defined _M5225X                                                          // {24}  
        #define PORTIN_SETLD  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x45) // 
        #define PORTIN_SETGP  *(volatile unsigned char *)(PORT_MODULE_ADD + 0x46) // 
    #endif
    #if defined _M5225X                                                           // {24}
        #define CLEARTE     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x48)   // {71}
        #define CLEARTF     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x49)   //
        #define CLEARTG     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4a)   //
        #define CLEARTH     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4b)   //
        #define CLEARTI     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4c)   //
        #define CLEARTJ     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x4e)   //
    #endif
    #define CLEARNQ         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x50)   // {71}
    #define CLEARAN         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x52)   // 
    #define CLEARAS         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x53)   // 
    #define CLEARQS         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x54)   // 
    #define CLEARTA         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x56)   // 
    #define CLEARTC         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x57)   //
    #if !defined _M5225X                                                          // {24} 
        #define CLEARTD     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x58)   // 
    #endif
    #define CLEARUA         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x59)   // 
    #define CLEARUB         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x5a)   // 
    #define CLEARUC         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x5b)   //
    #define CLEARDD         *(volatile unsigned char *)(PORT_MODULE_ADD + 0x5c)   //
    #if !defined _M5225X                                                          // {24}  
        #define CLEARLD     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x5d)   // 
        #define CLEARGP     *(volatile unsigned char *)(PORT_MODULE_ADD + 0x5e)   //
    #endif
    #if defined _M5225X                                                           // {24}{31}
        #define PTEPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x60)   //
        #define PTFPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x61)   //
        #define PTGPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x62)   //
      //#define PTHPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x63)   // Not used since TH needs quad function - see other PTHPAR register
        #define PTIPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x64)   //
        #define PTJPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x66)   //
    #endif
    #define PNQPAR          *(unsigned short *)(PORT_MODULE_ADD + 0x68)  // 
    #define PANPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x6a)   // 
    #define PASPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x6b)   // 
    #define PQSPAR          *(unsigned short *)(PORT_MODULE_ADD + 0x6c)  // 
    #define PTAPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x6e)   // 
    #define PTCPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x6f)   // 
    #if !defined _M5225X                                                          // {24} 
        #define PTDPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x70)   // 
    #endif
    #define PUAPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x71)   // 
    #define PUBPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x72)   // 
    #define PUCPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x73)   //
    #define PDDPAR          *(unsigned char *)(PORT_MODULE_ADD + 0x74)   //
     #if !defined _M5225X                                                         // {24} 
        #define PLDPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x75)   // 
        #define PGPPAR      *(unsigned char *)(PORT_MODULE_ADD + 0x76)   // 
    #endif
    #if defined _M5225X                                                           // {24}
        #define CLKOUTPAR   *(unsigned char *)(PORT_MODULE_ADD + 0x77)   // 
          #define CLK_OFF   0x00000000                                   // {38} TCLK/PSTCLK disabled
          #define CLK_ON    0x00000001                                   // system clock drives TCLK/PSTCLK
          #define FB_CLK_ON 0x00000003                                   // FB_CLK drives TCLK/PSTCLK
        #define PSRR        *(unsigned long *)(PORT_MODULE_ADD + 0x78)   // 
        #define PDSR        *(unsigned long *)(PORT_MODULE_ADD + 0x7c)   // 
        #define PSRRL       *(unsigned long *)(PORT_MODULE_ADD + 0x80)   // 
        #define PSRRH       *(unsigned short *)(PORT_MODULE_ADD + 0x86)  //
        #define PDSRL       *(unsigned long *)(PORT_MODULE_ADD + 0x88)   //
        #define PDSRH       *(unsigned short *)(PORT_MODULE_ADD + 0x8c)  //  
        #define PTHPAR      *(unsigned short *)(PORT_MODULE_ADD + 0x90)  // quad function to select TH pin assignment (rather than 
    #else
        #define PWOR        *(unsigned short *)(PORT_MODULE_ADD + 0x78)  // Pin Wire-OR Register
        #define PDSR1       *(unsigned short *)(PORT_MODULE_ADD + 0x7a)  // {9} Pin Drive Strength Register 1
        #define PDSR0       *(unsigned long *)(PORT_MODULE_ADD + 0x7c)   // {9} Pin Drive Strength Register 0
    #endif
  #endif
#endif

#if defined _WINDOWS
    #define  _SIM_PORT() fnPortChanges(1)                                // {43}
#else
    #define  _SIM_PORT()
#endif


// Port macros                                                           // {41}
//
// Configure pins as output eg. _CONFIG_PORT_OUTPUT(AN, PORT_AN_BIT0);
//
#define _CONFIG_PORT_OUTPUT(ref, pins)        DDR##ref |= (pins); _SIM_PORT()
// Configure pins as input  eg. _CONFIG_PORT_INPUT(LD, PORT_LD_BIT2);
//
#define _CONFIG_PORT_INPUT(ref, pins)         DDR##ref &= ~(pins); _SIM_PORT()
// Configure a peripheral function
//
//#define _CONFIG_PERIPHERAL(ref, periph, peripheral_block, pins, functions) - not supported

// Write full port width
//
#define _WRITE_PORT(ref, value)         PORT##ref = (value); _SIM_PORT()
// Write to a port with a mask
//
#define _WRITE_PORT_MASK(ref, value, mask)  PORT##ref = ((PORT##ref & ~mask) | (value & mask)); _SIM_PORT()
// Toggle a port with a mask
//
#define _TOGGLE_PORT(ref, mask)         PORT##ref ^= (mask); _SIM_PORT()
// Read full port width
//
#define _READ_PORT(ref)                 (PORTIN_SET##ref)
// Read from a port with a mask
//
#define _READ_PORT_MASK(ref, mask)      (PORTIN_SET##ref & mask)
// Configure outputs and then set a value to them (value is set first in this chip)
//
#define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) PORT##ref = ((PORT##ref & ~(pins)) | ((value) & (pins))); DDR##ref |= (pins); _SIM_PORT()
// Set from inputs to outputs and set a value to them (value is set first in this chip and is also equivalent to _CONFIG_DRIVE_PORT_OUTPUT_VALUE)
//
#define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) PORT##ref = ((PORT##ref & ~(pins)) | ((value) & (pins))); DDR##ref |= (pins); _SIM_PORT()
// Set from outputs to inputs
//
#define _FLOAT_PORT(ref, pins)          DDR##ref &= ~(pins); _SIM_PORT()
// Set from inputs to outputs 
//
#define _DRIVE_PORT_OUTPUT(ref, pins)   DDR##ref |= (pins); _SIM_PORT()
// Set and clear individual bits of a port
//
#if defined _WINDOWS                                                     // simulator uses simulation routines for this
    extern void _SETQS(unsigned char ucValue);
    extern void _CLEARQS(unsigned char ucValue);
    extern void _SETAN(unsigned char ucValue);
    extern void _CLEARAN(unsigned char ucValue);
    extern void _SETLD(unsigned char ucValue);
    extern void _CLEARLD(unsigned char ucValue);
    extern void _SETNQ(unsigned char ucValue);
    extern void _CLEARNQ(unsigned char ucValue);      
    extern void _SETTA(unsigned char ucValue);
    extern void _CLEARTA(unsigned char ucValue);
    extern void _SETTC(unsigned char ucValue);
    extern void _CLEARTC(unsigned char ucValue);
    extern void _SETTD(unsigned char ucValue);
    extern void _CLEARTD(unsigned char ucValue);
    extern void _SETUA(unsigned char ucValue);
    extern void _CLEARUA(unsigned char ucValue);
    extern void _SETUB(unsigned char ucValue);
    extern void _CLEARUB(unsigned char ucValue);
    extern void _SETUC(unsigned char ucValue);
    extern void _CLEARUC(unsigned char ucValue);
    extern void _SETAS(unsigned char ucValue);
    extern void _CLEARAS(unsigned char ucValue);
    extern void _SETGP(unsigned char ucValue);
    extern void _CLEARGP(unsigned char ucValue);
    extern void _SETDD(unsigned char ucValue);
    extern void _CLEARDD(unsigned char ucValue);
    extern void _SETUART(unsigned char ucValue);
    extern void _CLEARUART(unsigned char ucValue);
    extern void _SETTIMER(unsigned char ucValue);
    extern void _CLEARTIMER(unsigned char ucValue);
  #if defined _M5225X                                                    // {70}
    extern void _SETTE(unsigned char ucValue);
    extern void _CLEARTE(unsigned char ucValue);
    extern void _SETTF(unsigned char ucValue);
    extern void _CLEARTF(unsigned char ucValue);
    extern void _SETTG(unsigned char ucValue);
    extern void _CLEARTG(unsigned char ucValue);
    extern void _SETTH(unsigned char ucValue);
    extern void _CLEARTH(unsigned char ucValue);
    extern void _SETTI(unsigned char ucValue);
    extern void _CLEARTI(unsigned char ucValue);
    extern void _SETTJ(unsigned char ucValue);
    extern void _CLEARTJ(unsigned char ucValue);
  #endif

	#define _SETBITS(port, x)   _SET##port(x); _SIM_PORT()
	#define _CLEARBITS(port, x) _CLEAR##port(~(x)); _SIM_PORT()          // {20} convert to positive logic by inverting
#else                                                                    // {6}
	#define _SETBITS(port, x)   PORTIN_SET##port = (x)
	#define _CLEARBITS(port, x) CLEAR##port = ~(x)                       // {20} convert to positive logic by inverting                       
#endif

#if !defined _M523X
    #define PORT_AS_BIT0    0x01
    #define PORT_AS_BIT1    0x02
    #define PORT_AS_BIT2    0x04
    #define PORT_AS_BIT3    0x08

    #define PORT_AN_BIT0    0x01
    #define PORT_AN_BIT1    0x02
    #define PORT_AN_BIT2    0x04
    #define PORT_AN_BIT3    0x08
    #define PORT_AN_BIT4    0x10
    #define PORT_AN_BIT5    0x20
    #define PORT_AN_BIT6    0x40
    #define PORT_AN_BIT7    0x80

    #define PORT_LD_BIT0    0x01
    #define PORT_LD_BIT1    0x02
    #define PORT_LD_BIT2    0x04
    #define PORT_LD_BIT3    0x08
    #define PORT_LD_BIT4    0x10
    #define PORT_LD_BIT5    0x20
    #define PORT_LD_BIT6    0x40

    //#define PORT_NQ_BIT0  0x01                                     // IRQ0 doesn't exist
    #define PORT_NQ_BIT1    0x02                                     // IRQ1..IRQ7
    #define PORT_NQ_BIT2    0x04
    #define PORT_NQ_BIT3    0x08
    #define PORT_NQ_BIT4    0x10
    #define PORT_NQ_BIT5    0x20
    #define PORT_NQ_BIT6    0x40
    #define PORT_NQ_BIT7    0x80

    #define PORT_QS_BIT0    0x01
    #define PORT_QS_BIT1    0x02
    #define PORT_QS_BIT2    0x04
    #define PORT_QS_BIT3    0x08
    #define PORT_QS_BIT4    0x10
    #define PORT_QS_BIT5    0x20
    #define PORT_QS_BIT6    0x40
    #define PORT_QS_BIT7    0x80

    #define PORT_TA_BIT0    0x01
    #define PORT_TA_BIT1    0x02
    #define PORT_TA_BIT2    0x04
    #define PORT_TA_BIT3    0x08

    #define PORT_TC_BIT0    0x01
    #define PORT_TC_BIT1    0x02
    #define PORT_TC_BIT2    0x04
    #define PORT_TC_BIT3    0x08

    #define PORT_TD_BIT0    0x01
    #define PORT_TD_BIT1    0x02
    #define PORT_TD_BIT2    0x04
    #define PORT_TD_BIT3    0x08

    #if defined _M5225X                                                  // {70}
        #define PORT_TE_BIT0    0x01
        #define PORT_TE_BIT1    0x02
        #define PORT_TE_BIT2    0x04
        #define PORT_TE_BIT3    0x08
        #define PORT_TE_BIT4    0x10
        #define PORT_TE_BIT5    0x20
        #define PORT_TE_BIT6    0x40
        #define PORT_TE_BIT7    0x80

        #define PORT_TF_BIT0    0x01
        #define PORT_TF_BIT1    0x02
        #define PORT_TF_BIT2    0x04
        #define PORT_TF_BIT3    0x08
        #define PORT_TF_BIT4    0x10
        #define PORT_TF_BIT5    0x20
        #define PORT_TF_BIT6    0x40
        #define PORT_TF_BIT7    0x80

        #define PORT_TG_BIT0    0x01
        #define PORT_TG_BIT1    0x02
        #define PORT_TG_BIT2    0x04
        #define PORT_TG_BIT3    0x08
      //#define PORT_TG_BIT4    0x10                                     // doesn't exist on any packages
        #define PORT_TG_BIT5    0x20
        #define PORT_TG_BIT6    0x40
        #define PORT_TG_BIT7    0x80
 
        #define PORT_TH_BIT0    0x01
        #define PORT_TH_BIT1    0x02
        #define PORT_TH_BIT2    0x04
        #define PORT_TH_BIT3    0x08
        #define PORT_TH_BIT4    0x10
        #define PORT_TH_BIT5    0x20
        #define PORT_TH_BIT6    0x40
        #define PORT_TH_BIT7    0x80

        #define PORT_TI_BIT0    0x01
        #define PORT_TI_BIT1    0x02
        #define PORT_TI_BIT2    0x04
        #define PORT_TI_BIT3    0x08
        #define PORT_TI_BIT4    0x10
        #define PORT_TI_BIT5    0x20
        #define PORT_TI_BIT6    0x40
        #define PORT_TI_BIT7    0x80

        #define PORT_TJ_BIT0    0x01
        #define PORT_TJ_BIT1    0x02
        #define PORT_TJ_BIT2    0x04
        #define PORT_TJ_BIT3    0x08
        #define PORT_TJ_BIT4    0x10
        #define PORT_TJ_BIT5    0x20
        #define PORT_TJ_BIT6    0x40
        #define PORT_TJ_BIT7    0x80
    #endif

    #define PORT_UA_BIT0    0x01
    #define PORT_UA_BIT1    0x02
    #define PORT_UA_BIT2    0x04
    #define PORT_UA_BIT3    0x08

    #define PORT_UB_BIT0    0x01
    #define PORT_UB_BIT1    0x02
    #define PORT_UB_BIT2    0x04
    #define PORT_UB_BIT3    0x08

    #define PORT_UC_BIT0    0x01
    #define PORT_UC_BIT1    0x02
    #define PORT_UC_BIT2    0x04
    #define PORT_UC_BIT3    0x08

    #define PORT_DD_BIT0    0x01
    #define PORT_DD_BIT1    0x02
    #define PORT_DD_BIT2    0x04
    #define PORT_DD_BIT3    0x08
    #define PORT_DD_BIT4    0x10
    #define PORT_DD_BIT5    0x20
    #define PORT_DD_BIT6    0x40
    #define PORT_DD_BIT7    0x80

    #define PORT_GP_BIT0    0x01                                     // {40}
    #define PORT_GP_BIT1    0x02
    #define PORT_GP_BIT2    0x04
    #define PORT_GP_BIT3    0x08
    #define PORT_GP_BIT4    0x10
    #define PORT_GP_BIT5    0x20
    #define PORT_GP_BIT6    0x40
    #define PORT_GP_BIT7    0x80
#endif

#define PORT_A_BIT0         0x01
#define PORT_A_BIT1         0x02
#define PORT_A_BIT2         0x04
#define PORT_A_BIT3         0x08
#define PORT_A_BIT4         0x10
#define PORT_A_BIT5         0x20
#define PORT_A_BIT6         0x40
#define PORT_A_BIT7         0x80

#define PORT_B_BIT0         0x01
#define PORT_B_BIT1         0x02
#define PORT_B_BIT2         0x04
#define PORT_B_BIT3         0x08
#define PORT_B_BIT4         0x10
#define PORT_B_BIT5         0x20
#define PORT_B_BIT6         0x40
#define PORT_B_BIT7         0x80

#define PORT_C_BIT0         0x01
#define PORT_C_BIT1         0x02
#define PORT_C_BIT2         0x04
#define PORT_C_BIT3         0x08
#define PORT_C_BIT4         0x10
#define PORT_C_BIT5         0x20
#define PORT_C_BIT6         0x40
#define PORT_C_BIT7         0x80

#define PORT_D_BIT0         0x01
#define PORT_D_BIT1         0x02
#define PORT_D_BIT2         0x04
#define PORT_D_BIT3         0x08
#define PORT_D_BIT4         0x10
#define PORT_D_BIT5         0x20
#define PORT_D_BIT6         0x40
#define PORT_D_BIT7         0x80

#define PORT_E_BIT0         0x01
#define PORT_E_BIT1         0x02
#define PORT_E_BIT2         0x04
#define PORT_E_BIT3         0x08
#define PORT_E_BIT4         0x10
#define PORT_E_BIT5         0x20
#define PORT_E_BIT6         0x40
#define PORT_E_BIT7         0x80

#define PORT_F_BIT0         0x01
#define PORT_F_BIT1         0x02
#define PORT_F_BIT2         0x04
#define PORT_F_BIT3         0x08
#define PORT_F_BIT4         0x10
#define PORT_F_BIT5         0x20
#define PORT_F_BIT6         0x40
#define PORT_F_BIT7         0x80

#define PORT_G_BIT0         0x01
#define PORT_G_BIT1         0x02
#define PORT_G_BIT2         0x04
#define PORT_G_BIT3         0x08
#define PORT_G_BIT4         0x10
#define PORT_G_BIT5         0x20
#define PORT_G_BIT6         0x40
#define PORT_G_BIT7         0x80

#define PORT_H_BIT0         0x01
#define PORT_H_BIT1         0x02
#define PORT_H_BIT2         0x04
#define PORT_H_BIT3         0x08
#define PORT_H_BIT4         0x10
#define PORT_H_BIT5         0x20
#define PORT_H_BIT6         0x40
#define PORT_H_BIT7         0x80

#define PORT_I_BIT0         0x01
#define PORT_I_BIT1         0x02
#define PORT_I_BIT2         0x04
#define PORT_I_BIT3         0x08
#define PORT_I_BIT4         0x10
#define PORT_I_BIT5         0x20
#define PORT_I_BIT6         0x40
#define PORT_I_BIT7         0x80

#define PORT_J_BIT0         0x01
#define PORT_J_BIT1         0x02
#define PORT_J_BIT2         0x04
#define PORT_J_BIT3         0x08
#define PORT_J_BIT4         0x10
#define PORT_J_BIT5         0x20
#define PORT_J_BIT6         0x40
#define PORT_J_BIT7         0x80

// M520X
//
#define PORT_TIMER_BIT0     0x01
#define PORT_TIMER_BIT1     0x02
#define PORT_TIMER_BIT2     0x04
#define PORT_TIMER_BIT3     0x08
#define PORT_TIMER_BIT4     0x10
#define PORT_TIMER_BIT5     0x20
#define PORT_TIMER_BIT6     0x40
#define PORT_TIMER_BIT7     0x80

#define PORT_UART_BIT0      0x01
#define PORT_UART_BIT1      0x02
#define PORT_UART_BIT2      0x04
#define PORT_UART_BIT3      0x08
#define PORT_UART_BIT4      0x10
#define PORT_UART_BIT5      0x20
#define PORT_UART_BIT6      0x40
#define PORT_UART_BIT7      0x80

#define PORT_FECH_BIT0      0x01
#define PORT_FECH_BIT1      0x02
#define PORT_FECH_BIT2      0x04
#define PORT_FECH_BIT3      0x08
#define PORT_FECH_BIT4      0x10
#define PORT_FECH_BIT5      0x20
#define PORT_FECH_BIT6      0x40
#define PORT_FECH_BIT7      0x80

#define PORT_FECL_BIT0      0x01
#define PORT_FECL_BIT1      0x02
#define PORT_FECL_BIT2      0x04
#define PORT_FECL_BIT3      0x08
#define PORT_FECL_BIT4      0x10
#define PORT_FECL_BIT5      0x20
#define PORT_FECL_BIT6      0x40
#define PORT_FECL_BIT7      0x80

#define PORT_BUSCTL_BIT0    0x01
#define PORT_BUSCTL_BIT1    0x02
#define PORT_BUSCTL_BIT2    0x04
#define PORT_BUSCTL_BIT3    0x08

#define PORT_BE_BIT0        0x01
#define PORT_BE_BIT1        0x02
#define PORT_BE_BIT2        0x04
#define PORT_BE_BIT3        0x08

#define PORT_CS_BIT0        0x01
#define PORT_CS_BIT1        0x02
#define PORT_CS_BIT2        0x04
#define PORT_CS_BIT3        0x08

#define PORT_FECI2C_BIT0    0x01
#define PORT_FECI2C_BIT1    0x02
#define PORT_FECI2C_BIT2    0x04
#define PORT_FECI2C_BIT3    0x08

#define PORT_QSPI_BIT0      0x01
#define PORT_QSPI_BIT1      0x02
#define PORT_QSPI_BIT2      0x04
#define PORT_QSPI_BIT3      0x08


// M523X
//
#if defined _M523X
    #define PORT_CS_BIT1    0x02
    #define PORT_CS_BIT2    0x04
    #define PORT_CS_BIT3    0x08
    #define PORT_CS_BIT4    0x10
    #define PORT_CS_BIT5    0x20
    #define PORT_CS_BIT6    0x40
    #define PORT_CS_BIT7    0x80

    #define PORT_IRQ_BIT1   0x02
    #define PORT_IRQ_BIT2   0x04
    #define PORT_IRQ_BIT3   0x08
    #define PORT_IRQ_BIT4   0x10
    #define PORT_IRQ_BIT5   0x20
    #define PORT_IRQ_BIT6   0x40
    #define PORT_IRQ_BIT7   0x80
#else
    #define PORT_IRQ_BIT1   0x02
    #define PORT_IRQ_BIT4   0x10
    #define PORT_IRQ_BIT7   0x80
#endif

#define QPIO_FUNCTION_D     0x0
#define PRIMARY_FUNCTION_D  0x1

#define BIT_0_D_SHIFT       0
#define BIT_1_D_SHIFT       1
#define BIT_2_D_SHIFT       2
#define BIT_3_D_SHIFT       3
#define BIT_4_D_SHIFT       4
#define BIT_5_D_SHIFT       5
#define BIT_6_D_SHIFT       6
#define BIT_7_D_SHIFT       7

#define QPIO_FUNCTION_Q     0x0
#define PRIMARY_FUNCTION_Q  0x1
#define ALT_1_FUNCTION_Q    0x2
#define ALT_2_FUNCTION_Q    0x3
#define CLEAR_FUNCTION_Q    0x03

#define BIT_0_Q_SHIFT       0
#define BIT_1_Q_SHIFT       2
#define BIT_2_Q_SHIFT       4
#define BIT_3_Q_SHIFT       6
#define BIT_4_Q_SHIFT       8
#define BIT_5_Q_SHIFT       10
#define BIT_6_Q_SHIFT       12
#define BIT_7_Q_SHIFT       14

#define CLEAR_FUNCTION_Q_BIT_0 (CLEAR_FUNCTION_Q << BIT_0_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_1 (CLEAR_FUNCTION_Q << BIT_1_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_2 (CLEAR_FUNCTION_Q << BIT_2_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_3 (CLEAR_FUNCTION_Q << BIT_3_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_4 (CLEAR_FUNCTION_Q << BIT_4_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_5 (CLEAR_FUNCTION_Q << BIT_5_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_6 (CLEAR_FUNCTION_Q << BIT_6_Q_SHIFT)
#define CLEAR_FUNCTION_Q_BIT_7 (CLEAR_FUNCTION_Q << BIT_7_Q_SHIFT)

#define _CLEAR_FUNCTION(port, port_bits) P##port##PAR &= ~(port_bits); _SIM_PORT()


// Peripheral Port Functions                                             {45}
//
#define UA_TX0_FUNCTION (PRIMARY_FUNCTION_Q << BIT_0_Q_SHIFT)
#define UA_RX0_FUNCTION (PRIMARY_FUNCTION_Q << BIT_1_Q_SHIFT)
#if defined _M52XX_SDRAM                                                 // {53}
    #define UA_RX1_FUNCTION (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)
    #define UA_TX1_FUNCTION (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)
#endif

#define UB_TX1_FUNCTION (PRIMARY_FUNCTION_Q << BIT_0_Q_SHIFT)
#define UB_RX1_FUNCTION (PRIMARY_FUNCTION_Q << BIT_1_Q_SHIFT)
#define QS_TX1_FUNCTION (ALT_2_FUNCTION_Q << BIT_0_Q_SHIFT)
#define QS_RX1_FUNCTION (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT)

#if defined _M5222X || defined _M5225X
    #define UC_TX2_FUNCTION (PRIMARY_FUNCTION_Q << BIT_0_Q_SHIFT)
    #define UC_RX2_FUNCTION (PRIMARY_FUNCTION_Q << BIT_1_Q_SHIFT)
#else
    #define UC_TX2_FUNCTION (PRIMARY_FUNCTION_D << BIT_0_D_SHIFT)
    #define UC_RX2_FUNCTION (PRIMARY_FUNCTION_D << BIT_1_D_SHIFT)
#endif
#if defined _M52XX_SDRAM
    #define AS_TX2_FUNCTION (ALT_1_FUNCTION_Q << BIT_0_Q_SHIFT)
    #define AS_RX2_FUNCTION (ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT)
    #define AS_TX2_FUNCTION_1 (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT)
    #define AS_RX2_FUNCTION_1 (ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT)
    #define AS_TX2_FUNCTION_2 (ALT_1_FUNCTION_Q << BIT_4_Q_SHIFT)
    #define AS_RX2_FUNCTION_2 (ALT_1_FUNCTION_Q << BIT_5_Q_SHIFT)
#else
    #define AS_TX2_FUNCTION (ALT_2_FUNCTION_Q << BIT_0_Q_SHIFT)
    #define AS_RX2_FUNCTION (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT)
#endif
#define UB_TX2_FUNCTION (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)
#define UB_RX2_FUNCTION (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)

#define UA_RTS0_FUNCTION (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)
#define QS_RTS1_FUNCTION (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)
#define UB_RTS1_FUNCTION (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)
#if defined _M5222X || defined _M5225X
    #define UC_RTS2_FUNCTION (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)
#else
    #define UC_RTS2_FUNCTION (PRIMARY_FUNCTION_D << BIT_2_D_SHIFT)
#endif

#define UA_CTS0_FUNCTION (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)
#define QS_CTS1_FUNCTION (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT);
#define UB_CTS1_FUNCTION (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)
#if defined _M5222X || defined _M5225X
    #define UC_CTS2_FUNCTION (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)
#else
    #define UC_CTS2_FUNCTION (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)
#endif

#define QS_QSPI_DOUT (PRIMARY_FUNCTION_Q << BIT_0_Q_SHIFT)
#define QS_QSPI_DIN  (PRIMARY_FUNCTION_Q << BIT_1_Q_SHIFT)
#define QS_QSPI_SCK  (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)

#define AS_I2C_SCL_0_FUNCTION (PRIMARY_FUNCTION_Q << BIT_0_Q_SHIFT)
#define AS_I2C_SDA_0_FUNCTION (PRIMARY_FUNCTION_Q << BIT_1_Q_SHIFT)

#if defined _M5225X
    #define AS_FB_ALE_FUNCTION (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)     // {69}
    #define AS_FB_CS1_FUNCTION (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT)
#endif

#define QS_I2C_SCL_0_FUNCTION (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT)
#define QS_I2C_SDA_0_FUNCTION (ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT)

#define TC_PWM0_FUNCTION        (ALT_2_FUNCTION_Q << BIT_0_Q_SHIFT)      // {57}
#define TC_PWM2_FUNCTION        (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT)
#define TC_PWM4_FUNCTION        (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)
#define TC_PWM6_FUNCTION        (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)

#define TA_PWM1_FUNCTION        (ALT_2_FUNCTION_Q << BIT_0_Q_SHIFT)
#define TA_PWM3_FUNCTION        (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT)
#define TA_PWM5_FUNCTION        (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)
#define TA_PWM7_FUNCTION        (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)

#if defined _M5225X
    #define QS_I2C_SCL_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_0_Q_SHIFT)
    #define QS_I2C_SDA_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT)

    #define UB_I2C_SCL_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_0_Q_SHIFT)
    #define UB_I2C_SDA_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT)

    #define UC_I2C_SCL_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT)
    #define UC_I2C_SDA_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT)

    #define TH_I2C_SCL_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT)
    #define TH_I2C_SDA_1_FUNCTION   (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT)

    #define TE_FB_ADD_0_FUNCTION    (PRIMARY_FUNCTION_D << BIT_0_D_SHIFT)
    #define TE_FB_ADD_1_FUNCTION    (PRIMARY_FUNCTION_D << BIT_1_D_SHIFT)
    #define TE_FB_ADD_2_FUNCTION    (PRIMARY_FUNCTION_D << BIT_2_D_SHIFT)
    #define TE_FB_ADD_3_FUNCTION    (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)
    #define TE_FB_ADD_4_FUNCTION    (PRIMARY_FUNCTION_D << BIT_4_D_SHIFT)
    #define TE_FB_ADD_5_FUNCTION    (PRIMARY_FUNCTION_D << BIT_5_D_SHIFT)
    #define TE_FB_ADD_6_FUNCTION    (PRIMARY_FUNCTION_D << BIT_6_D_SHIFT)
    #define TE_FB_ADD_7_FUNCTION    (PRIMARY_FUNCTION_D << BIT_7_D_SHIFT)

    #define TF_FB_ADD_8_FUNCTION    (PRIMARY_FUNCTION_D << BIT_0_D_SHIFT)
    #define TF_FB_ADD_9_FUNCTION    (PRIMARY_FUNCTION_D << BIT_1_D_SHIFT)
    #define TF_FB_ADD_10_FUNCTION   (PRIMARY_FUNCTION_D << BIT_2_D_SHIFT)
    #define TF_FB_ADD_11_FUNCTION   (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)
    #define TF_FB_ADD_12_FUNCTION   (PRIMARY_FUNCTION_D << BIT_4_D_SHIFT)
    #define TF_FB_ADD_13_FUNCTION   (PRIMARY_FUNCTION_D << BIT_5_D_SHIFT)
    #define TF_FB_ADD_14_FUNCTION   (PRIMARY_FUNCTION_D << BIT_6_D_SHIFT)
    #define TF_FB_ADD_15_FUNCTION   (PRIMARY_FUNCTION_D << BIT_7_D_SHIFT)

    #define TG_FB_ADD_16_FUNCTION   (PRIMARY_FUNCTION_D << BIT_0_D_SHIFT)
    #define TG_FB_ADD_17_FUNCTION   (PRIMARY_FUNCTION_D << BIT_1_D_SHIFT)
    #define TG_FB_ADD_18_FUNCTION   (PRIMARY_FUNCTION_D << BIT_2_D_SHIFT)
    #define TG_FB_ADD_19_FUNCTION   (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)
    #define TG_FB_CS0_FUNCTION      (PRIMARY_FUNCTION_D << BIT_5_D_SHIFT)
    #define TG_FB_OE_FUNCTION       (PRIMARY_FUNCTION_D << BIT_6_D_SHIFT)
    #define TG_FB_RW_FUNCTION       (PRIMARY_FUNCTION_D << BIT_7_D_SHIFT)

    #define TH_FB_D0_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_6_Q_SHIFT)
    #define TH_FB_D1_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_7_Q_SHIFT)
    #define TH_FB_D2_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_0_Q_SHIFT)
    #define TH_FB_D3_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_1_Q_SHIFT)
    #define TH_FB_D4_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)
    #define TH_FB_D5_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)
    #define TH_FB_D6_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_4_Q_SHIFT)
    #define TH_FB_D7_FUNCTION       (PRIMARY_FUNCTION_Q << BIT_5_Q_SHIFT)

    #define TI_FEC_COL_FUNCTION     (PRIMARY_FUNCTION_D << BIT_0_D_SHIFT)
    #define TI_FEC_CRS_FUNCTION     (PRIMARY_FUNCTION_D << BIT_1_D_SHIFT)
    #define TI_FEC_RXCLK_FUNCTION   (PRIMARY_FUNCTION_D << BIT_2_D_SHIFT)
    #define TI_FEC_RXD_0_FUNCTION   (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)
    #define TI_FEC_RXD_1_FUNCTION   (PRIMARY_FUNCTION_D << BIT_4_D_SHIFT)
    #define TI_FEC_RXD_2_FUNCTION   (PRIMARY_FUNCTION_D << BIT_5_D_SHIFT)
    #define TI_FEC_RXD_3_FUNCTION   (PRIMARY_FUNCTION_D << BIT_6_D_SHIFT)
    #define TI_FEC_RXDV_FUNCTION    (PRIMARY_FUNCTION_D << BIT_7_D_SHIFT)

    #define TJ_FEC_RXER_FUNCTION    (PRIMARY_FUNCTION_D << BIT_0_D_SHIFT)
    #define TJ_FEC_TXCLK_FUNCTION   (PRIMARY_FUNCTION_D << BIT_1_D_SHIFT)
    #define TJ_FEC_TXD_0_FUNCTION   (PRIMARY_FUNCTION_D << BIT_2_D_SHIFT)
    #define TJ_FEC_TXD_1_FUNCTION   (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)
    #define TJ_FEC_TXD_2_FUNCTION   (PRIMARY_FUNCTION_D << BIT_4_D_SHIFT)
    #define TJ_FEC_TXD_3_FUNCTION   (PRIMARY_FUNCTION_D << BIT_5_D_SHIFT)
    #define TJ_FEC_TXEN_FUNCTION    (PRIMARY_FUNCTION_D << BIT_6_D_SHIFT)
    #define TJ_FEC_TXER_FUNCTION    (PRIMARY_FUNCTION_D << BIT_7_D_SHIFT)

    #define NQ_IRQ1_FUNCTION        (PRIMARY_FUNCTION_Q << (2 * 1))      // {56}
    #define NQ_USB_ALTCLK_FUNCTION  (ALT_2_FUNCTION_Q << (2 * 1))
    #define NQ_IRQ3_FUNCTION        (PRIMARY_FUNCTION_Q << (2 * 3))
    #define NQ_FEC_MDIO_FUNCTION    (ALT_1_FUNCTION_Q << (2 * 3))
    #define NQ_IRQ5_FUNCTION        (PRIMARY_FUNCTION_Q << (2 * 5))
    #define NQ_FEC_MDC_FUNCTION     (ALT_1_FUNCTION_Q << (2 * 5))
    #define NQ_IRQ7_FUNCTION        (PRIMARY_FUNCTION_Q << (2 * 7))
#endif
#if defined _M5221X || defined _M521XX                                   // {61}
    #define QS_I2C_SCL_1_FUNCTION   (ALT_2_FUNCTION_Q << BIT_4_Q_SHIFT)
    #define QS_I2C_SDA_1_FUNCTION   (ALT_2_FUNCTION_Q << BIT_0_Q_SHIFT) 
#endif

#define ACT_LED_ENABLED        0x01
#define LINK_LED_ENABLED       0x02
#define SPEED_LED_ENABLED      0x04
#define DUPLEX_LED_ENABLED     0x08
#define COLLISION_LED_ENABLED  0x10
#define RX_LED_ENABLED         0x20
#define TX_LED_ENABLED         0x40

#define ENABLE_PHY_LEDS     (ACT_LED_ENABLED | LINK_LED_ENABLED | SPEED_LED_ENABLED | DUPLEX_LED_ENABLED | COLLISION_LED_ENABLED | RX_LED_ENABLED | TX_LED_ENABLED)


#if defined _M520X                                                       // {54}
    // SDRAM Controller
    //
    #define SDMR            *(unsigned long *)(SDRAM_MODULE_ADD + 0x000) // SDRAM Mode/Extended Mode Register
    #define SDCR            *(unsigned long *)(SDRAM_MODULE_ADD + 0x004) // SDRAM Control Register
    #define SDCFG1          *(unsigned long *)(SDRAM_MODULE_ADD + 0x008) // SDRAM Configuration Register 1
    #define SDCFG2          *(unsigned long *)(SDRAM_MODULE_ADD + 0x00c) // SDRAM Configuration Register 2
    #define SDCS0           *(unsigned long *)(SDRAM_MODULE_ADD + 0x110) // SDRAM Chip Select 0 Configuration
    #define SDCS1           *(unsigned long *)(SDRAM_MODULE_ADD + 0x114) // SDRAM Chip Select 1 Configuration
#elif defined _M523X
    // SDRAM Module
    //
    #define DCR              *(unsigned long *)(SDRAM_MODULE_ADD + 0x00) // DRAM control register
      #define NAM            0x2000
      #define COC            0x1000
      #define IS             0x0800
      #define RTIM_CLK_3     0x0000
      #define RTIM_CLK_6     0x0020
      #define RTIM_CLK_9     0x0040
      #define RC_MASK        0x001f 
    #define DACR0            *(unsigned long *)(SDRAM_MODULE_ADD + 0x08) // DRAM address and control register 0
      #define BA_MASK        0x3fff
      #define BA_SHIFT       18
      #define RE             0x00008000
      #define CASL_0         0x00000000
      #define CASL_1         0x00001000
      #define CASL_2         0x00002000
      #define CASL_3         0x00003000
      #define CBM_MASK       0x7
      #define CBM_SHIFT      8
      #define IMRS           0x00000040
      #define PS_32BIT       0x00000000
      #define PS_8BIT        0x00000010
      #define PS_16BIT       0x00000020
      #define IP             0x00000008
    #define DMR0             *(unsigned long *)(SDRAM_MODULE_ADD + 0x0c) // DRAM control mask register 0
      #define DRAM_BAM_MASK  0x3fff
      #define DRAM_BAM_SHIFT 18
      #define DRAM_WP        0x00000100
      #define DRAM_CS_VALID  0x00000001
#endif

// Reset Control Module 
//
#define RESET_RCR           *(volatile unsigned char *)(RESET_CTRL_ADD + 0x00) // {34} Reset Control Register
  #define SOFT_RST          0x80                                         // Software reset command
  #define FRCRSTOUT         0x40                                         // Assert RSTO signal
  #define LVDF              0x10                                         // Low voltage detected
  #define LVDIE             0x08                                         // Low Voltage Detect Interrupt Enable
  #define LVDRE             0x04                                         // Low Voltage Detect Reset Enable
  #define LVDE              0x01                                         // Low voltage detect is enabled
#define RESET_RSR           *(volatile unsigned char *)(RESET_CTRL_ADD + 0x01) // {34} Reset Status Register (read-only)
  #if defined BACKUP_WATCHDOG_TIMER_AVAILABLE
    #define RESET_BWT       0x80                                         // Last reset caused by Background Watchdog Timer
  #endif
  #define RESET_LVD         0x40                                         // Last reset caused by Low Voltage Detect
  #define RESET_SOFT        0x20                                         // Last reset caused by Software
  #if defined BACKUP_WATCHDOG_TIMER_AVAILABLE
    #define RESET_WDR       0x80                                         // Last reset caused by Background Watchdog Timer timeout
  #endif
  #define RESET_POR         0x08                                         // Last reset caused by Power-on
  #define RESET_EXT         0x04                                         // Last reset caused by External reset
  #define RESET_LOC         0x02                                         // Last reset caused by Loss Of Clock
  #define RESET_LOL         0x01                                         // Last reset caused by Loss Of Lock
#define CCM_CCR             *(volatile unsigned short *)(RESET_CTRL_ADD + 0x04) // Chip Configuration Register (supervisor - write once) {10}
#if defined _M5222X || defined _M5221X || defined _M521XX
  #define BMT_32K              0x0001                                    // Bus monitor timing 32k clocks (default)
  #define CCR_EZPORT_MODE      0x0500
  #define CCR_SINGLE_CHIP_MODE 0x0600
#else
  #define BMT_64K           0x0000                                       // Bus monitor timing 64k clocks
  #define BMT_32K           0x0001                                       // Bus monitor timing 32k clocks (default)
  #define BMT_16K           0x0002                                       // Bus monitor timing 16k clocks 
  #define BMT_8K            0x0003                                       // Bus monitor timing 8k clocks 
  #define BMT_4K            0x0004                                       // Bus monitor timing 4k clocks 
  #define BMT_2K            0x0005                                       // Bus monitor timing 2k clocks 
  #define BMT_1K            0x0006                                       // Bus monitor timing 1k clocks 
  #define BMT_512           0x0007                                       // Bus monitor timing 512 clocks 
  #define CCR_BME           0x0008                                       // Bus monitor enable
  #define CCR_PSTEN         0x0020                                       // PST[3:0]/DATA[3:0] enable
  #define CCR_TSIZ          0x0040                                       // TSIZ[1:0] enable
  #if defined _M52XX_SDRAM
    #define CCR_SINGLE_CHIP_MODE 0x0600
    #define CCR_MASTER_MODE 0x0700
    #define CCR_LOAD        0x8000                                       // pad driver load - full strength
  #else
    #define CCR_SINGLE_CHIP_MODE 0
  #endif
#endif
#if defined _M52XX_SDRAM
#define CCM_LPCR            *(unsigned short*)(RESET_CTRL_ADD + 0x06)    // {53} Low Power Control Register
#else
#define CCM_LPCR            *(unsigned char *)(RESET_CTRL_ADD + 0x07)    // Low Power Control Register {10}
  #define LVDSE             0x02                                         // LVD enabled on power down request
  #define STPMD_SYSTEM      0x00                                         // System clock disabled in STOP mode
  #define STPMD_SYS_CLKO    0x08                                         // System clock and CLOCKOUT disabled in STOP mode
  #define STPMD_SYS_CLKO_PLL 0x10                                        // System clock and CLOCKOUT and PLL disabled in STOP mode
  #define STPMD_SYS_CLKO_PLL_OSC 0x18                                    // System clock and CLOCKOUT and PLL and OSC disabled in STOP mode
  #define LPMD_RUN          0x00                                         // No clocks disabled on STOP instruction
  #define LPMD_DOZE         0x40                                         // Doze on STOP instruction
  #define LPMD_WAIT         0x80                                         // Wait on STOP instruction
  #define LPMD_STOP         0xc0                                         // Stop on STOP instruction
#endif
#define CCM_RCON            *(unsigned short *)(RESET_CTRL_ADD + 0x08)   // Reset Configuration Register (supervisor read-only) {10}
  #define RCON_SINGLE_CHIP  0x0000                                       // Single chip mode
  #define RCON_MODE         0x0001                                       // Master chip
  #define RCON_BOOTSEL      0x0004                                       // boot from external boot device (M528X)
  #define RCON_BOOTINTERN   0x0000                                       // (M528X)
  #define RCON_BOOT_16BITS  0x0008                                       // external 16 bits (M528X)
  #define RCON_BOOT_8BITS   0x0010                                       // external 8 bits (M528X)
  #define RCON_BOOT_32BITS  0x0018                                       // external 32 bits (M528X)
  #define RCON_RLOAD        0x0020                                       // Pad driver full strength
  #define RCON_RCSC_A23_21  0x0000                                       // chip select configuration (M528X)
  #define RCON_RCSC_CS6_A22_21 0x0100                                    // chip select configuration (M528X)
  #define RCON_RCSC_CS6_CS5_A21 0x0200                                   // chip select configuration (M528X)
  #define RCON_RCSC_CS6_CS5_CS4 0x0300                                   // chip select configuration (M528X)
#define CCM_CIR             *(unsigned short *)(RESET_CTRL_ADD + 0x0a)   // Chip Identification Register (read-only) {10}
  #if defined _M52110
  #define COLDFIRE_ID       (0x46 << 6)                                  // ID for 52100
  #elif defined _M52110
  #define COLDFIRE_ID       (0x47 << 6)                                  // ID for 52110
  #elif defined _M52230 || defined _M52232                               // {42}
  #define COLDFIRE_ID       (0x48 << 6)                                  // ID for 52230
  #elif defined _M52231
  #define COLDFIRE_ID       (0x49 << 6)                                  // ID for 52231
  #elif defined _M52233 || defined _M52236                               // {42}
  #define COLDFIRE_ID       (0x4a << 6)                                  // ID for 52233
  #elif defined _M52234
  #define COLDFIRE_ID       (0x4b << 6)                                  // ID for 52234
  #elif defined _M52235
  #define COLDFIRE_ID       (0x4c << 6)                                  // ID for 52235
  #elif defined _M52221
  #define COLDFIRE_ID       (0x50 << 6)                                  // ID for 52221
  #elif defined _M52223
  #define COLDFIRE_ID       (0x53 << 6)                                  // ID for 52223
  #elif defined _M52252
  #define COLDFIRE_ID       (0x83 << 6)                                  // ID for 52252 (512k FLASH / 64k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x84 << 6)                                  // ID for 52252 (512k FLASH / 32k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x85 << 6)                                  // ID for 52254 (256k FLASH / 64k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x86 << 6)                                  // ID for 52254 (256k FLASH / 32k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x87 << 6)                                  // ID for 52252C (512k FLASH / 64k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x88 << 6)                                  // ID for 52252C (512k FLASH / 32k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x89 << 6)                                  // ID for 52254C (256k FLASH / 64k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x8a << 6)                                  // ID for 52254C (256k FLASH / 32k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x8b << 6)                                  // ID for 52256 (512k FLASH / 64k SRAM 144 LQFP or MAPBGA)
//#define COLDFIRE_ID       (0x8c << 6)                                  // ID for 52256 (512k FLASH / 32k SRAM 144 LQFP or MAPBGA)
//#define COLDFIRE_ID       (0x8d << 6)                                  // ID for 52258 (256k FLASH / 64k SRAM 144 LQFP or MAPBGA)
//#define COLDFIRE_ID       (0x8e << 6)                                  // ID for 52258 (256k FLASH / 32k SRAM 144 LQFP or MAPBGA)
//#define COLDFIRE_ID       (0x8f << 6)                                  // ID for 52256C (512k FLASH / 64k SRAM 144 LQFP or MAPBGA)
//#define COLDFIRE_ID       (0x90 << 6)                                  // ID for 52258C (256k FLASH / 64k SRAM 144 LQFP or MAPBGA)
//#define COLDFIRE_ID       (0x91 << 6)                                  // ID for 52255C (512k FLASH / 64k SRAM 100 LQFP)
//#define COLDFIRE_ID       (0x92 << 6)                                  // ID for 52259C (512k FLASH / 64k SRAM 144 LQFP or MAPBGA)
  #elif defined _M52XX_SDRAM
  #define COLDFIRE_ID       (0x2000)                                     // ID for M528X
  #else
  #define COLDFIRE_ID       (0)                                          // ID for unknown device
  #endif
#if defined _M5225X
    #define CCM_CCE         *(unsigned short *)(RESET_CTRL_ADD + 0x10)   // Chip Identification Extended Register (write once only)
    #define USB_END         0x4000                                       // Big-endian enabled for USB
    #define MBMOD           0x8000                                       // Mini-Flex Bus in 1:1 mode
#endif

// Clock Module
//
#if defined _M520X                                                       // {54}
  #define PODR              *(unsigned short *)(CLOCK_MODULE_ADD + 0x0)  // PLL Output Divider Register
  #define PCR               *(unsigned short *)(CLOCK_MODULE_ADD + 0x2)  // PLL Control Register
  #define PMDR              *(unsigned short *)(CLOCK_MODULE_ADD + 0x4)  // PLL Modulation Divider Register
  #define PFDR              *(unsigned short *)(CLOCK_MODULE_ADD + 0x6)  // PLL Feedback Divider Register
#elif defined _M523X                                                     // {60}
  #define MCF_CLOCK_SYNCR   *(unsigned long *)(CLOCK_MODULE_ADD + 0x0)   // Synthersizer Control Register
  #define MCF_CLOCK_SYNSR   *(volatile unsigned long *)(CLOCK_MODULE_ADD + 0x4) // Synthersizer Status Register
    #define SYNSR_CALPASS   0x00000001
    #define SYNSR_CALDONE   0x00000002
    #define SYNSR_LOCF      0x00000004
    #define SYNSR_LOCK      0x00000008
    #define SYNSR_LOCKS     0x00000010
    #define SYNSR_PLLREF    0x00000020
    #define SYNSR_PLLSEL    0x00000040
    #define SYNSR_PLLMODE   0x00000080
    #define SYNSR_LOC       0x00000100
    #define SYNSR_LOLF      0x00000200
    #define PLLOCKED        SYNSR_LOCK
#else
#define MCF_CLOCK_SYNCR     *(unsigned short *)(CLOCK_MODULE_ADD + 0x0)  // Synthersizer Control Register
#if defined _M52XX_SDRAM
  #define STOP_MODE_PLL_OSC_CLKOUT_EN 0x0000
  #define STOP_MODE_PLL_OSC_EN        0x0004
  #define STOP_MODE_OSC_EN            0x0008
  #define STOP_MODE_NO_CLKS           0x000c
#else
  #define PLLEN             0x0001 
  #define PLLMODE           0x0002
  #define CLKSRC            0x0004 
#endif
  #define FWKUP             0x0020                                       // Fast wakeup - systen clocks enabled on wakeup regardless of PLL lock status
  #define DISCLK            0x0040                                       // Disable CLKOUT (holds low)
  #define LOCEN             0x0080                                       // Loss-of-Clock Function Enable
  #define LOCRE             0x0800                                       // Loss-of-Clock Reset Enable
  #define PLL_DIV_2         0x0100
  #define PLL_MUL_8         0x2000
  #define PLL_MUL_10        0x3000
  #define PLL_MUL_12        0x4000
  #define PLL_MUL_18        0x7000
  #define LOLRE             0x8000                                       // Loss-of-Lock Reset Enable
#define MCF_CLOCK_SYNSR     *(volatile unsigned char *)(CLOCK_MODULE_ADD + 0x2) // Synthersizer Status Register
    #if defined _M5221X || defined _M521XX || defined _M5222X
  #define CRYOSC            0x20                                         // reference clock is crystal clock reference
  #define OCOSC             0x40                                         // reference clock is on-chip relaxation oscillator
    #elif defined _M52XX_SDRAM                                           // {53}
  #define PLLREF            0x20
  #define PLLSEL            0x40
  #define PLLMODE           0x80
    #endif
  #define LOCS              0x04                                         // loss of clock detected since exiting reset or oscillator not yet recovered
  #define PLLOCKED          0x08
  #define LOCKS             0x10                                         // no unintentional PLL loss of lock since last system reset or MFD change
  #define EXTOSC            0x80                                         // indicates whether an external oscillator is providing the reference clock source
#if !defined _M521X && !defined _M52XX_SDRAM && !defined _M523X          // {53}{60}
    #if defined _M5221X || defined _M521XX || defined _M5222X || defined _M5225X
 #define MCF_CLOCK_ROCR     *(unsigned short *)(CLOCK_MODULE_ADD + 0x4)  // Relaxation Oscillator Control Register 
    #define TRIM_MASK       0x03ff                                       // Trim mask - value loaded from FLASH information row - bits [9:0] 
    #endif
 #define MCF_CLOCK_LPDR     *(unsigned char *)(CLOCK_MODULE_ADD + 0x7)   // Low Power Divider Register {4}
 #define MCF_CLOCK_LPCR     *(unsigned char *)(CLOCK_MODULE_ADD + 0x7)   // Low Power Control Register (same as LPDR)
    #define SYS_CLK_DIV_2     0x01
    #define SYS_CLK_DIV_4     0x02
    #define SYS_CLK_DIV_8     0x03
    #define SYS_CLK_DIV_16    0x04
    #define SYS_CLK_DIV_32    0x05
    #define SYS_CLK_DIV_64    0x06
    #define SYS_CLK_DIV_128   0x07
    #define SYS_CLK_DIV_256   0x08
    #define SYS_CLK_DIV_512   0x09
    #define SYS_CLK_DIV_1024  0x0a
    #define SYS_CLK_DIV_2048  0x0b
    #define SYS_CLK_DIV_4096  0x0c
    #define SYS_CLK_DIV_8192  0x0d
    #define SYS_CLK_DIV_16384 0x0e
    #define SYS_CLK_DIV_32768 0x0f
 #define MCF_CLOCK_CCHR     *(unsigned char *)(CLOCK_MODULE_ADD + 0x8)   // Clock Control High Register 
    #if defined _M5221X || defined _M521XX || defined _M5222X || defined _M5225X // M5223X doesn't have these {24}
 #define MCF_CLOCK_CCLR     *(unsigned char *)(CLOCK_MODULE_ADD + 0x9)   // Clock Control Low Register
    #define OSCSEL           0x01                                        // Relaxation oscillator selected for PLL input/bypass clock (default determined by reset configuration)
    #if defined _M5221X || defined _M521XX || defined _M5225X                               // {24}
    #define OSCSEL0          OSCSEL
    #define OSCSEL1          0x02                                        // Secondary oscillator selected for PLL input/bypass clock
    #endif
 #define MCF_CLOCK_OCHR     *(unsigned char *)(CLOCK_MODULE_ADD + 0xa)   // Oscillator Control High Register
    #define RELAX_STBY      0x40                                         // Relaxation oscillator runs in standby mode with output of 400kHz (rather that 8MHz in normal mode)
    #define RELAX_OCOEN     0x80                                         // Enable relaxation oscillator - default determined during reset configuration
 #define MCF_CLOCK_OCLR     *(unsigned char *)(CLOCK_MODULE_ADD + 0xb)   // Oscillator Control Low Register
    #define RANGE_32K       0x00                                         // external oscillator configured to run with 32k crystal
    #define RANGE           0x10                                         // configure external oscillator to run with 1..16MHz crystal
    #define LPEN            0x20                                         // external oscillator runs in low power mode rather than normal mode
    #define REFS            0x40                                         // external oscillator runs in external crystal mode, rather than external oscillator mode (default determined by reset configuration)
    #define OSCEN           0x80                                         // external oscillator is enabled (default determined by reset configuration)
    #endif
    #if !defined _M5221X && !defined _M521XX
 #define MCF_CLOCK_RTCDR    *(unsigned long *)(CLOCK_MODULE_ADD + 0xc)   // Real Time Clock Divide Register
    #endif
    #if defined _M5221X || defined _M521XX || defined _M5225X            // {24}
 #define MCF_CLOCK_RTCCR    *(unsigned char *)(CLOCK_MODULE_ADD + 0x12)  // Real Time Clock Control Register
    #define RTCSEL_SYS      0x00                                         // RTC clock sourced from system clock
    #define RTCSEL_OSC      0x01                                         // RTC clock sourced from RTC oscillator
    #define RTC_LPEN        0x02                                         // RTC oscillator runs in low power mode rather than normal mode
    #define RTC_REFS        0x04                                         // RTC oscillator runs in external crystal mode, rather than external oscillator mode
    #define KHZEN_KHZ       0x00                                         // RTC oscillator configured to operate in kHz range
    #define KHZEN_MHZ       0x08                                         // RTC oscillator configured to operate in MHz range
    #define RTC_OSCEN       0x10                                         // RTC oscillator is enabled
    #define EXTALEN         0x40                                         // {47} Enable 32k oscillator
 #define MCF_CLOCK_BWCR     *(unsigned char *)(CLOCK_MODULE_ADD + 0x13)  // Backup Watchdog Timer Control Register (write-once register)
    #define BWDSEL_SYS_DIV2 0x00                                         // Select half the system clock frequency as backup watchdog timer clock
    #define BWDSEL_RELAX    0x01                                         // Select the relaxation oscillator as backup watchdog timer clock
    #define BWDSTOP         0x02                                         // Continue clocking the backup watchdog from the relaxation clock in stop mode
    #endif
#endif
#endif                                                                   // endif not M520X

// Edge port Module 0
//
#define EPPAR0_ADR          (unsigned short *)EPORT_MODULE_0_ADD
#define EPPAR0              *(unsigned short *)(EPORT_MODULE_0_ADD + 0x0)// EPORT Pin Assignment Register
#define EPDDR0              *(volatile unsigned char *)(EPORT_MODULE_0_ADD + 0x2) // EPORT Data Direction Register {39}
#define EPIER0              *(unsigned char *)(EPORT_MODULE_0_ADD + 0x3) // EPORT Interrupt Enable Register
#define EPDR0               *(unsigned char *)(EPORT_MODULE_0_ADD + 0x4) // EPORT Data Register
#define EPPDR0              *(volatile unsigned char *)(EPORT_MODULE_0_ADD + 0x5) // EPORT Pin Data Register
#define EPFR0               *(volatile unsigned char *)(EPORT_MODULE_0_ADD + 0x6) // EPORT Flag Register

// Edge port Module 1
//
#define EPPAR1_ADR          (unsigned short *)EPORT_MODULE_1_ADD
#define EPPAR1              *(unsigned short *)(EPORT_MODULE_1_ADD + 0x0)// EPORT Pin Assignment Register
#define EPDDR1              *(volatile unsigned char *)(EPORT_MODULE_1_ADD + 0x2) // EPORT Data Direction Register {39}
#define EPIER1              *(unsigned char *)(EPORT_MODULE_1_ADD + 0x3) // EPORT Interrupt Enable Register
#define EPDR1               *(unsigned char *)(EPORT_MODULE_1_ADD + 0x4) // EPORT Data Register
#define EPPDR1              *(volatile unsigned char *)(EPORT_MODULE_1_ADD + 0x5) // EPORT Pin Data Register
#define EPFR1               *(volatile unsigned char *)(EPORT_MODULE_1_ADD + 0x6) // EPORT Flag Register


#if defined BACKUP_WATCHDOG_TIMER_AVAILABLE
// Backup Watchdog Timer Registers
//
#define WCR                 *(volatile unsigned short *)(BWTR_MODULE_ADD + 0x0) // Backup Watchdog Timer Control Register - write-once (only writable after a power on reset)
  #define BWT_EN            0x0001                                       // enable BWT
  #define BWT_DOZE          0x0004                                       // stop BWT when in DOZE mode
  #define BWT_WAIT          0x0008                                       // stop BWT when in WAIT mode
  #define BWT_STOP          0x0010                                       // stop BWT when in STOP mode
#define WMR                 *(unsigned short *)(BWTR_MODULE_ADD + 0x2)   // Backup Watchdog Timer Modulus Register - write-once (only writable after a power on reset)
#define WCNTR               *(volatile unsigned short *)(BWTR_MODULE_ADD + 0x4) // Backup Watchdog Timer Count Register ( Read only)
#define WSR                 *(volatile unsigned short *)(BWTR_MODULE_ADD + 0x6) // Backup Watchdog Timer Service Register

#define BWT_TIMEOUT_SYS_CLK(timeout)      (((((BUS_CLOCK/2) * timeout) - 4)/4096) - 1)   // {33}
#define BWT_TIMEOUT_RELAX_CLK(timeout)    (((((RELAXATION_OSCILLATOR_FREQ/2) * timeout) - 4)/4096) - 1)
#endif

// PIT Timers
//
#define PIT_PCSR_0          *(volatile unsigned short *)(PIT_0_ADD + 0x0)// PIT 0 Control and Status Register (ONLY SUPERVISOR)
#if defined _WINDOWS
    #define PIT_PCSR_NON_PRE_0  *(volatile unsigned char *)(PIT_0_ADD + 0x0) // {73} byte access without pre-scaler (little-endian)
#else
    #define PIT_PCSR_NON_PRE_0  *(volatile unsigned char *)(PIT_0_ADD + 0x1) // {73} byte access without pre-scaler
#endif
  #define PIT_EN            0x0001                                       // Enable operation
  #define PIT_RLD           0x0002                                       // Reload
  #define PIT_PIF           0x0004                                       // Interrupt flag
  #define PIT_PIE           0x0008                                       // Interrupt enable
  #define PIT_OVW           0x0010                                       // Overwrite
  #define PIT_DBG           0x0020                                       // Debug
  #define PIT_DOZE          0x0040                                       // Doze mode
  #define PIT_PRESCALE_32K  0x0f00                                       // Prescale divide by 32768
  #define PIT_PRESCALE_16K  0x0e00                                       // Prescale divide by 16384
  #define PIT_PRESCALE_8K   0x0d00                                       // Prescale divide by 8192
  #define PIT_PRESCALE_4K   0x0c00                                       // Prescale divide by 4096
  #define PIT_PRESCALE_2K   0x0b00                                       // Prescale divide by 2048
  #define PIT_PRESCALE_1K   0x0a00                                       // Prescale divide by 1024
  #define PIT_PRESCALE_2    0x0100                                       // Prescale divide by 2
#define PIT_PMR_0           *(unsigned short *)(PIT_0_ADD + 0x2)         // PIT 0 Modulus Register (ONLY SUPERVISOR)
#define PIT_PCNTR_0         *(volatile unsigned short *)(PIT_0_ADD + 0x4)// PIT 0 Count Register

#define PIT_PCSR_1          *(volatile unsigned short *)(PIT_1_ADD + 0x0)// PIT 1 Control and Status Register (ONLY SUPERVISOR)
#define PIT_PCSR_NON_PRE_1  *(volatile unsigned char *)(PIT_1_ADD + 0x1) // {73} byte access without pre-scaler
#define PIT_PMR_1           *(unsigned short *)(PIT_1_ADD + 0x2)         // PIT 1 Modulus Register (ONLY SUPERVISOR)
#define PIT_PCNTR_1         *(volatile unsigned short *)(PIT_1_ADD + 0x4)// PIT 1 Count Register

#define PIT_PCSR_2          *(volatile unsigned short *)(PIT_2_ADD + 0x0)// PIT 2 Control and Status Register (ONLY SUPERVISOR)
#define PIT_PCSR_NON_PRE_2  *(volatile unsigned char *)(PIT_2_ADD + 0x1) // {73} byte access without pre-scaler
#define PIT_PMR_2           *(unsigned short *)(PIT_2_ADD + 0x2)         // PIT 2 Modulus Register (ONLY SUPERVISOR)
#define PIT_PCNTR_2         *(volatile unsigned short *)(PIT_2_ADD + 0x4)// PIT 2 Count Register

#define PIT_PCSR_3          *(volatile unsigned short *)(PIT_3_ADD + 0x0)// PIT 3 Control and Status Register (ONLY SUPERVISOR)
#define PIT_PCSR_NON_PRE_3  *(volatile unsigned char *)(PIT_3_ADD + 0x1) // {73} byte access without pre-scaler
#define PIT_PMR_3           *(unsigned short *)(PIT_3_ADD + 0x2)         // PIT 3 Modulus Register (ONLY SUPERVISOR)
#define PIT_PCNTR_3         *(volatile unsigned short *)(PIT_3_ADD + 0x4)// PIT 3 Count Register

#if defined _M52XX_SDRAM                                                 // {53}
// QADC (provisional)
//
#define QADCMCR             *(volatile unsigned short *)(ADC_ADD + 0x00) // supervisor only
#define QADCTEST            *(volatile unsigned short *)(ADC_ADD + 0x02) // supervisor only
#define PORTQA              *(volatile unsigned char *)(ADC_ADD + 0x06)
#define PORTQB              *(volatile unsigned char *)(ADC_ADD + 0x07)
#define DDRQA               *(volatile unsigned char *)(ADC_ADD + 0x08)
#define DDRQB               *(volatile unsigned char *)(ADC_ADD + 0x09)
#define QACR0               *(volatile unsigned short *)(ADC_ADD + 0x0a)
#define QACR1               *(volatile unsigned short *)(ADC_ADD + 0x0c)
#define QACR2               *(volatile unsigned short *)(ADC_ADD + 0x0e)
#define QASR0               *(volatile unsigned short *)(ADC_ADD + 0x10)
#define QASR1               *(volatile unsigned short *)(ADC_ADD + 0x12)
#define QADC_CCW_ADR        (volatile unsigned short *)(ADC_ADD + 0x200)
#define QADC_RJURR_ADR      (volatile unsigned short *)(ADC_ADD + 0x280)
#define QADC_LJSRR_ADR      (volatile unsigned short *)(ADC_ADD + 0x300)
#define QADC_LJURR_ADR      (volatile unsigned short *)(ADC_ADD + 0x380)
#else
// ADC - note all registers are supervisor-mode access only
//
#define ADC_CTRL1           *(volatile unsigned short *)(ADC_ADD + 0x0)  // ADC Control Register 1
 #define STOP0              0x4000                                       // Stop ADC conversion
 #define START0             0x2000                                       // Start ADC conversion
 #define SYNC0              0x1000                                       // Start on synch trigger
 #define EOSIE0             0x0800                                       // End of scan interrupt enable
 #define ZCIE               0x0400                                       // Zero crossing interrupt enable
 #define LLMTIE             0x0200                                       // Low Level Limit interrupt enable
 #define HLMTIE             0x0100                                       // High Level Limit interrupt enable
 #define DIFF_AN0_AN1       0x0010                                       // AN0+ and AN1- configured as differential pair
 #define DIFF_AN2_AN3       0x0020                                       // AN2+ and AN3- configured as differential pair
 #define DIFF_AN4_AN5       0x0040                                       // AN4+ and AN5- configured as differential pair
 #define DIFF_AN6_AN7       0x0080                                       // AN6+ and AN7- configured as differential pair
 #define ALL_SINGLE_ENDED   0x0000                                       // All inputs single ended mode
 #define ONCE_SEQUENCIAL    0x0000                                       // Scan once sequentially on start
 #define ADC_TRIG_OPN       0x0004
 #define ADC_LOOP_OPN       0x0002
 #define ADC_PARALLEL_OPN   0x0001
#define ADC_CTRL2           *(volatile unsigned short *)(ADC_ADD + 0x2)  // ADC Control Register 2
 #define STOP1              0x4000                                       // Stop ADC conversion
 #define START1             0x2000                                       // Start ADC conversion
 #define SYNC1              0x1000                                       // Start on synch trigger
 #define EOSIE1             0x0800                                       // End of scan interrupt enable
 #define SIMULT             0x0020                                       // Simultaneous mode
 #define SLOWEST_ADC_CLOCK  0x001f
#define ADC_ADZCC           *(unsigned short *)(ADC_ADD + 0x4)           // ADC Zero Crossing Control Register
 #define ZERO_CROSS_BOTH    0x03
 #define ZERO_CROSS_NEG     0x01
 #define ZERO_CROSS_POS     0x02
#define ADC_ADLST1          *(unsigned short *)(ADC_ADD + 0x6)           // ADC Channel List Control Register 1
#define ADC_ADLST2          *(unsigned short *)(ADC_ADD + 0x8)           // ADC Channel List Control Register 2
#define ADC_ADSDIS          *(unsigned short *)(ADC_ADD + 0xa)           // ADC Sample Disable Register
 #define DISABLE_CH7        0x0080
 #define DISABLE_CH6        0x0040
 #define DISABLE_CH5        0x0020
 #define DISABLE_CH4        0x0010
 #define DISABLE_CH3        0x0008
 #define DISABLE_CH2        0x0004
 #define DISABLE_CH1        0x0002
 #define DISABLE_CH0        0x0001
#define ADC_ADSTAT          *(volatile unsigned short *)(ADC_ADD + 0xc)  // ADC Status Register
 #define CIP0               0x8000                                       // Conversion in progress 0
 #define CIP1               0x4000                                       // Conversion in progress 1
 #define EOSI1              0x1000
 #define EOSI0              0x0800
 #define RDY_CHANNEL7       0x0080
 #define RDY_CHANNEL6       0x0040
 #define RDY_CHANNEL5       0x0020
 #define RDY_CHANNEL4       0x0010
 #define RDY_CHANNEL3       0x0008
 #define RDY_CHANNEL2       0x0004
 #define RDY_CHANNEL1       0x0002
 #define RDY_CHANNEL0       0x0001
#define ADC_ADLSTAT         *(volatile unsigned short *)(ADC_ADD + 0xe)  // ADC Limit Status Register
 #define HLS_CHANNEL7       0x8000                                       // high limit status flags (write 1 to clear)
 #define HLS_CHANNEL6       0x4000
 #define HLS_CHANNEL5       0x2000
 #define HLS_CHANNEL4       0x1000
 #define HLS_CHANNEL3       0x0800
 #define HLS_CHANNEL2       0x0400
 #define HLS_CHANNEL1       0x0200
 #define HLS_CHANNEL0       0x0100
 #define LLS_CHANNEL7       0x0080                                       // low limit status flags (write 1 to clear)
 #define LLS_CHANNEL6       0x0040
 #define LLS_CHANNEL5       0x0020
 #define LLS_CHANNEL4       0x0010
 #define LLS_CHANNEL3       0x0008
 #define LLS_CHANNEL2       0x0004
 #define LLS_CHANNEL1       0x0002
 #define LLS_CHANNEL0       0x0001
#define ADC_ADZCSTAT        *(volatile unsigned short *)(ADC_ADD + 0x10) // ADC Zero Crossing Status Register
 #define ZCS_CHANNEL7       0x0080                                       // low limit status flags (write 1 to clear)
 #define ZCS_CHANNEL6       0x0040
 #define ZCS_CHANNEL5       0x0020
 #define ZCS_CHANNEL4       0x0010
 #define ZCS_CHANNEL3       0x0008
 #define ZCS_CHANNEL2       0x0004
 #define ZCS_CHANNEL1       0x0002
 #define ZCS_CHANNEL0       0x0001
#define ADC_ADRSLT_ADD      (unsigned short *)(ADC_ADD + 0x12)
#define ADC_ADRSLT0         *(volatile unsigned short *)(ADC_ADD + 0x12) // ADC Result Register 0
#define ADC_ADRSLT1         *(volatile unsigned short *)(ADC_ADD + 0x14) // ADC Result Register 1
#define ADC_ADRSLT2         *(volatile unsigned short *)(ADC_ADD + 0x16) // ADC Result Register 2
#define ADC_ADRSLT3         *(volatile unsigned short *)(ADC_ADD + 0x18) // ADC Result Register 3
#define ADC_ADRSLT4         *(volatile unsigned short *)(ADC_ADD + 0x1a) // ADC Result Register 4
#define ADC_ADRSLT5         *(volatile unsigned short *)(ADC_ADD + 0x1c) // ADC Result Register 5
#define ADC_ADRSLT6         *(volatile unsigned short *)(ADC_ADD + 0x1e) // ADC Result Register 6
#define ADC_ADRSLT7         *(volatile unsigned short *)(ADC_ADD + 0x20) // ADC Result Register 7
#define ADC_ADLLR_ADD       (unsigned short *)(ADC_ADD + 0x22)
#define ADC_ADLLR0          *(unsigned short *)(ADC_ADD + 0x22)          // ADC Low Limit Register 0
#define ADC_ADLLR1          *(unsigned short *)(ADC_ADD + 0x24)          // ADC Low Limit Register 1
#define ADC_ADLLR2          *(unsigned short *)(ADC_ADD + 0x26)          // ADC Low Limit Register 2
#define ADC_ADLLR3          *(unsigned short *)(ADC_ADD + 0x28)          // ADC Low Limit Register 3
#define ADC_ADLLR4          *(unsigned short *)(ADC_ADD + 0x2a)          // ADC Low Limit Register 4
#define ADC_ADLLR5          *(unsigned short *)(ADC_ADD + 0x2c)          // ADC Low Limit Register 5
#define ADC_ADLLR6          *(unsigned short *)(ADC_ADD + 0x2e)          // ADC Low Limit Register 6
#define ADC_ADLLR7          *(unsigned short *)(ADC_ADD + 0x30)          // ADC Low Limit Register 7
#define ADC_ADHLR0          *(unsigned short *)(ADC_ADD + 0x32)          // ADC High Limit Register 0
#define ADC_ADHLR1          *(unsigned short *)(ADC_ADD + 0x34)          // ADC High Limit Register 1
#define ADC_ADHLR2          *(unsigned short *)(ADC_ADD + 0x36)          // ADC High Limit Register 2
#define ADC_ADHLR3          *(unsigned short *)(ADC_ADD + 0x38)          // ADC High Limit Register 3
#define ADC_ADHLR4          *(unsigned short *)(ADC_ADD + 0x3a)          // ADC High Limit Register 4
#define ADC_ADHLR5          *(unsigned short *)(ADC_ADD + 0x3c)          // ADC High Limit Register 5
#define ADC_ADHLR6          *(unsigned short *)(ADC_ADD + 0x3e)          // ADC High Limit Register 6
#define ADC_ADHLR7          *(unsigned short *)(ADC_ADD + 0x40)          // ADC High Limit Register 7
#define ADC_ADOFS_ADD       (unsigned short *)(ADC_ADD + 0x42)
#define ADC_ADOFS0          *(unsigned short *)(ADC_ADD + 0x42)          // ADC Offset Register 0
#define ADC_ADOFS1          *(unsigned short *)(ADC_ADD + 0x44)          // ADC Offset Register 1
#define ADC_ADOFS2          *(unsigned short *)(ADC_ADD + 0x46)          // ADC Offset Register 2
#define ADC_ADOFS3          *(unsigned short *)(ADC_ADD + 0x48)          // ADC Offset Register 3
#define ADC_ADOFS4          *(unsigned short *)(ADC_ADD + 0x4a)          // ADC Offset Register 4
#define ADC_ADOFS5          *(unsigned short *)(ADC_ADD + 0x4c)          // ADC Offset Register 5
#define ADC_ADOFS6          *(unsigned short *)(ADC_ADD + 0x4e)          // ADC Offset Register 6
#define ADC_ADOFS7          *(unsigned short *)(ADC_ADD + 0x50)          // ADC Offset Register 7
#define ADC_POWER           *(volatile unsigned short *)(ADC_ADD + 0x52) // ADC Power Control register {5}
 #define PD0                0x0001                                       // Circuit A power down
 #define PD1                0x0002                                       // Circuit B power down
 #define PD2                0x0004                                       // Reference circuit power down
 #define APD                0x0008                                       // Auto-power down mode bit
 #define PSTS0              0x0400                                       // Converter A Power Status bit
 #define PSTS1              0x0800                                       // Converter B Power Status bit
 #define PSTS2              0x1000                                       // Reference circuit A Power Status bit
 #define ASB                0x8000                                       // Auto Standby Bit
#define ADC_CAL             *(unsigned short *)(ADC_ADD + 0x54)          // ADC Voltage Reference register
  #define SEL_VREFH_AN2     0x8000                                       // use AN2 pin as VREFH reference
  #define SEL_VREFL_AN6     0x4000                                       // use AN6 pin as VREFL reference
#endif

// General Purpose Timer 0
//
#define GPTIOS              *(unsigned char*)(GPT_ADD + 0x00)            // GPT IC/OC Select Register
  #define IOS0              0x01                                         // Output compare enabled - channel 0
  #define IOS1              0x02                                         // Output compare enabled - channel 1
  #define IOS2              0x04                                         // Output compare enabled - channel 2
  #define IOS3              0x08                                         // Output compare enabled - channel 3
#define GPTCFORC            *(unsigned char*)(GPT_ADD + 0x01)            // GPT Compare Force Register
  #define FOC0              0x01                                         // Force Output Compare - channel 0
  #define FOC1              0x02                                         // Force Output Compare - channel 1
  #define FOC2              0x04                                         // Force Output Compare - channel 2
  #define FOC3              0x08                                         // Force Output Compare - channel 3
#define GPTOC3M             *(unsigned char*)(GPT_ADD + 0x02)            // GPT Output Compare 3 Mask Register {23}
  #define OC3M0             0x01                                         // Output Compare 3 configured as output - channel 0
  #define OC3M1             0x02                                         // Output Compare 3 configured as output - channel 1
  #define OC3M2             0x04                                         // Output Compare 3 configured as output - channel 2
  #define OC3M3             0x08                                         // Output Compare 3 configured as output - channel 3
#define GPTOC3D             *(unsigned char*)(GPT_ADD + 0x03)            // GPT Output Compare 3 Data Register
  #define OC3D0             0x01                                         // Output Compare 3 Data transfer - channel 0                                            
  #define OC3D1             0x02                                         // Output Compare 3 Data transfer - channel 1
  #define OC3D2             0x04                                         // Output Compare 3 Data transfer - channel 2
  #define OC3D3             0x08                                         // Output Compare 3 Data transfer - channel 3
#define GPTCNT              *(volatile unsigned short*)(GPT_ADD + 0x04)  // GPT Counter Register (read only)
#define GPTSCR1             *(unsigned char*)(GPT_ADD + 0x06)            // GPT System Control Register 1
  #define GPTEN             0x80                                         // Timer enable 
  #define TFFCA             0x10                                         // Time fast flag clear all
#define GPTTOV              *(unsigned char*)(GPT_ADD + 0x08)            // GPT Toggle on Overflow Register
  #define TOV0              0x01                                         // Toggle output compare pin on overflow - channel 0                                            
  #define TOV1              0x02                                         // Toggle output compare pin on overflow - channel 1
  #define TOV2              0x04                                         // Toggle output compare pin on overflow - channel 2
  #define TOV3              0x08                                         // Toggle output compare pin on overflow - channel 3
#define GPTCTL1             *(unsigned char*)(GPT_ADD + 0x09)            // GPT Control Register 1
  #define OMOL_DISCON_0     0x00                                         // GPT disconnected from output logic pin - channel 0
  #define OMOL_TOGGLE_0     0x01                                         // Toggle OC0 output line
  #define OMOL_CLEAR_0      0x02                                         // Clear OC0 output line
  #define OMOL_SET_0        0x03                                         // Set OC0 output line
  #define OMOL_DISCON_1     0x00                                         // GPT disconnected from output logic pin - channel 1
  #define OMOL_TOGGLE_1     0x04                                         // Toggle OC1 output line
  #define OMOL_CLEAR_1      0x08                                         // Clear OC1 output line
  #define OMOL_SET_1        0x0c                                         // Set OC1 output line
  #define OMOL_DISCON_2     0x00                                         // GPT disconnected from output logic pin - channel 1
  #define OMOL_TOGGLE_2     0x10                                         // Toggle OC1 output line
  #define OMOL_CLEAR_2      0x20                                         // Clear OC1 output line
  #define OMOL_SET_2        0x30                                         // Set OC1 output line
  #define OMOL_DISCON_3     0x00                                         // GPT disconnected from output logic pin - channel 2
  #define OMOL_TOGGLE_3     0x40                                         // Toggle OC2 output line
  #define OMOL_CLEAR_3      0x80                                         // Clear OC2 output line
  #define OMOL_SET_3        0xc0                                         // Set OC2 output line
#define GPTCTL2             *(unsigned char*)(GPT_ADD + 0x0b)            // GPT Control Register 2
  #define EDG_DIS_0         0x00                                         // Input capture disabled - channel 0
  #define EDG_RISING_0      0x01                                         // Input capture on rising edges only - channel 0
  #define EDG_FALLING_0     0x02                                         // Input capture on falling edges only - channel 0
  #define EDG_BOTH_EDGES_0  0x03                                         // Input capture on both rising and falling edges - channel 0
  #define EDG_DIS_1         0x00                                         // Input capture disabled - channel 1
  #define EDG_RISING_1      0x04                                         // Input capture on rising edges only - channel 1
  #define EDG_FALLING_1     0x08                                         // Input capture on falling edges only - channel 1
  #define EDG_BOTH_EDGES_1  0x0c                                         // Input capture on both rising and falling edges - channel 1
  #define EDG_DIS_2         0x00                                         // Input capture disabled - channel 2
  #define EDG_RISING_2      0x10                                         // Input capture on rising edges only - channel 2
  #define EDG_FALLING_2     0x20                                         // Input capture on falling edges only - channel 2
  #define EDG_BOTH_EDGES_2  0x30                                         // Input capture on both rising and falling edges - channel 2
  #define EDG_DIS_3         0x00                                         // Input capture disabled - channel 3
  #define EDG_RISING_3      0x40                                         // Input capture on rising edges only - channel 3
  #define EDG_FALLING_3     0x80                                         // Input capture on falling edges only - channel 3
  #define EDG_BOTH_EDGES_3  0xc0                                         // Input capture on both rising and falling edges - channel 3
#define GPTIE               *(unsigned char*)(GPT_ADD + 0x0c)            // GPT Interrupt Enable Register  {23}
  #define C0I               0x01                                         // Interrupt request enabled - channel 0                                                   // timer channel interrupt mask
  #define C1I               0x02                                         // Interrupt request enabled - channel 1
  #define C2I               0x04                                         // Interrupt request enabled - channel 2
  #define C3I               0x08                                         // Interrupt request enabled - channel 3
#define GPTSCR2             *(unsigned char*)(GPT_ADD + 0x0d)            // GTP System Control Register 2
  #define GPT_PRESCALE_1    0x00                                         // timer clocked from system clock / 2 divided by 1
  #define GPT_PRESCALE_2    0x01                                         // timer clocked from system clock / 2 divided by 2
  #define GPT_PRESCALE_4    0x02                                         // timer clocked from system clock / 2 divided by 4
  #define GPT_PRESCALE_8    0x03                                         // timer clocked from system clock / 2 divided by 8
  #define GPT_PRESCALE_16   0x04                                         // timer clocked from system clock / 2 divided by 16
  #define GPT_PRESCALE_32   0x05                                         // timer clocked from system clock / 2 divided by 32
  #define GPT_PRESCALE_64   0x06                                         // timer clocked from system clock / 2 divided by 64
  #define GPT_PRESCALE_128  0x07                                         // timer clocked from system clock / 2 divided by 128
  #define TCRE              0x08                                         // Counter reset enabled (on channel 3 compare)
  #define RDPT              0x10                                         // Output drive reduction enabled
  #define PUPT              0x20                                         // Pull-up resistors enabled
  #define TOI               0x80                                         // Overflow interrupt requests enabled
#define GPTFLG1             *(volatile unsigned char*)(GPT_ADD + 0x0e)   // GPT Flag Register 1
  #define C0F               0x01                                         // Channel Flag - channel 0
  #define C1F               0x02                                         // Channel Flag - channel 1
  #define C2F               0x04                                         // Channel Flag - channel 2
  #define C3F               0x08                                         // Channel Flag - channel 3
#define GPTFLG2             *(volatile unsigned char*)(GPT_ADD + 0x0f)   // GPT Flag Register 2
  #define TOF               0x80                                         // Timer Overflow Flags
#define GPTC0               *(volatile unsigned short*)(GPT_ADD + 0x10)  // GPT Channel 0 Register
#define GPTC1               *(volatile unsigned short*)(GPT_ADD + 0x12)  // GPT Channel 1 Register
#define GPTC2               *(volatile unsigned short*)(GPT_ADD + 0x14)  // GPT Channel 2 Register
#define GPTC3               *(volatile unsigned short*)(GPT_ADD + 0x16)  // GPT Channel 3 Register
#define GPTPACTL            *(unsigned char*)(GPT_ADD + 0x18)            // GPT Pulse Accumulator Control Register
  #define PAI               0x01                                         // PAIF interrupt  request enabled
  #define PAOVI             0x02                                         // PAOVF interrupt  request enabled
  #define PGPT_PRESCALE_CLK 0x00                                         // GPT counter input clock from prescaler clock
  #define PGPT_PACLK        0x04                                         // GPT counter input clock from PACLK
  #define PGPT_PACLK_256    0x08                                         // GPT counter input clock from PACLK/256
  #define PGPT_PACLK_65536  0x0c                                         // GPT counter input clock from PACLK/65536
  #define PEDGE             0x10                                         // Pulse Accumulator Enabled (1 = rising edge in event counter mode or low gate)
  #define PAMOD             0x20                                         // Gated Time Accumulator Mode
  #define PAE               0x40                                         // Pulse Accumulator Enabled
#define GPTPAFLG            *(volatile unsigned char*)(GPT_ADD + 0x19)   // GPT Pulse Accumulator Flag Register
  #define PAIF              0x01                                         // active PAI input
  #define PAOVF             0x02                                         // Pulse Accumulator Overflow
#define GPTPACNT            *(volatile unsigned short*)(GPT_ADD + 0x1a)  // GPT Pulse Accumulator Count register
#define GPTPORT             *(volatile unsigned char*)(GPT_ADD + 0x1d)   // GPT Port Data Register
  #define PORTT_0           0x01
  #define PORTT_1           0x02
  #define PORTT_2           0x04
  #define PORTT_3           0x08
#define GPTDDR              *(unsigned char*)(GPT_ADD + 0x1e)            // GPT Port Data Direction Register
  #define DDRT_0            0x01
  #define DDRT_1            0x02
  #define DDRT_2            0x04
  #define DDRT_3            0x08
#define GPTTST              *(unsigned char*)(GPT_ADD + 0x1f)            // GPT Test register

#if defined _M52XX_SDRAM                                                 // {53}
// General Purpose Timer 1
//
#define GPT1IOS              *(unsigned char*)(GPT1_ADD + 0x00)          // GPT IC/OC Select Register
#define GPT1CFORC            *(unsigned char*)(GPT1_ADD + 0x01)          // GPT Compare Force Register
#define GPT1OC3M             *(unsigned char*)(GPT1_ADD + 0x02)          // GPT Output Compare 3 Mask Register
#define GPT1OC3D             *(unsigned char*)(GPT1_ADD + 0x03)          // GPT Output Compare 3 Data Register
#define GPT1CNT              *(volatile unsigned short*)(GPT1_ADD + 0x04)// GPT Counter Register (read only)
#define GPT1SCR1             *(unsigned char*)(GPT1_ADD + 0x06)          // GPT System Control Register 1
#define GPT1TOV              *(unsigned char*)(GPT1_ADD + 0x08)          // GPT Toggle on Overflow Register
#define GPT1CTL1             *(unsigned char*)(GPT1_ADD + 0x09)          // GPT Control Register 1
#define GPT1CTL2             *(unsigned char*)(GPT1_ADD + 0x0b)          // GPT Control Register 2
#define GPT1IE               *(unsigned char*)(GPT1_ADD + 0x0c)          // GPT Interrupt Enable Register
#define GPT1SCR2             *(unsigned char*)(GPT1_ADD + 0x0d)          // GTP System Control Register 2
#define GPT1FLG1             *(volatile unsigned char*)(GPT1_ADD + 0x0e) // GPT Flag Register 1
#define GPT1FLG2             *(volatile unsigned char*)(GPT1_ADD + 0x0f) // GPT Flag Register 2
#define GPT1C0               *(volatile unsigned short*)(GPT1_ADD + 0x10)// GPT Channel 0 Register
#define GPT1C1               *(volatile unsigned short*)(GPT1_ADD + 0x12)// GPT Channel 1 Register
#define GPT1C2               *(volatile unsigned short*)(GPT1_ADD + 0x14)// GPT Channel 2 Register
#define GPT1C3               *(volatile unsigned short*)(GPT1_ADD + 0x16)// GPT Channel 3 Register
#define GPT1PACTL            *(unsigned char*)(GPT1_ADD + 0x18)          // GPT Pulse Accumulator Control Register
#define GPT1PAFLG            *(volatile unsigned char*)(GPT1_ADD + 0x19) // GPT Pulse Accumulator Flag Register
#define GPT1PACNT            *(volatile unsigned short*)(GPT1_ADD + 0x1a)// GPT Pulse Accumulator Count register
#define GPT1PORT             *(volatile unsigned char*)(GPT1_ADD + 0x1d) // GPT Port Data Register
#define GPT1DDR              *(unsigned char*)(GPT1_ADD + 0x1e)          // GPT Port Data Direction Register
#define GPT1TST              *(unsigned char*)(GPT1_ADD + 0x1f)          // GPT Test register
#else
// Pulse Width Modulator Module                                          {37}
//
#define PWME                *(unsigned char*)(PWM_ADD + 0x00)            // PWM Enable Register
  #define PWME0             0x01                                         // enable PWM channel 0
  #define PWME1             0x02                                         // enable PWM channel 1
  #define PWME2             0x04                                         // enable PWM channel 2
  #define PWME3             0x08                                         // enable PWM channel 3
  #define PWME4             0x10                                         // enable PWM channel 4
  #define PWME5             0x20                                         // enable PWM channel 5
  #define PWME6             0x40                                         // enable PWM channel 6
  #define PWME7             0x80                                         // enable PWM channel 7
#define PWMPOL              *(unsigned char*)(PWM_ADD + 0x01)            // PWM Polarity Register
  #define PPOL0             0x01                                         // channel 0 polarity
  #define PPOL1             0x02                                         // channel 1 polarity
  #define PPOL2             0x04                                         // channel 2 polarity
  #define PPOL3             0x08                                         // channel 3 polarity
  #define PPOL4             0x10                                         // channel 4 polarity
  #define PPOL5             0x20                                         // channel 5 polarity
  #define PPOL6             0x40                                         // channel 6 polarity
  #define PPOL7             0x80                                         // channel 7 polarity
#define PWMCLK              *(unsigned char*)(PWM_ADD + 0x02)            // PWM Clock Select Register
  #define PWM_PCLK0         0x01                                         // channel 0 clock source
  #define PWM_PCLK1         0x02                                         // channel 1 clock source
  #define PWM_PCLK2         0x04                                         // channel 2 clock source
  #define PWM_PCLK3         0x08                                         // channel 3 clock source
  #define PWM_PCLK4         0x10                                         // channel 4 clock source
  #define PWM_PCLK5         0x20                                         // channel 5 clock source
  #define PWM_PCLK6         0x40                                         // channel 6 clock source
  #define PWM_PCLK7         0x80                                         // channel 7 clock source
#define PWMPRCLK            *(unsigned char*)(PWM_ADD + 0x03)            // PWM Prescale Clock Select Register
  #define PWMPRCLK_PCKA     0x07                                         // {57}
  #define PWMPRCLK_PCKB     0x70
  #define BUS_SPEED_CLOCK   0x00
  #define BUS_CLOCK_DIV_2   0x01
  #define BUS_CLOCK_DIV_4   0x02
  #define BUS_CLOCK_DIV_8   0x03
  #define BUS_CLOCK_DIV_16  0x04
  #define BUS_CLOCK_DIV_32  0x05
  #define BUS_CLOCK_DIV_64  0x06
  #define BUS_CLOCK_DIV_128 0x07
  #define BUS_CLK_SHIFT_A   0
  #define BUS_CLK_SHIFT_B   4
#define PWMCAE              *(unsigned char*)(PWM_ADD + 0x04)            // PWM Center Align Enable Register
  #define PWM_CAE0          0x01                                         // channel 0 center-aligned mode
  #define PWM_CAE1          0x02                                         // channel 1 center-aligned mode
  #define PWM_CAE2          0x04                                         // channel 2 center-aligned mode
  #define PWM_CAE3          0x08                                         // channel 3 center-aligned mode
  #define PWM_CAE4          0x10                                         // channel 4 center-aligned mode
  #define PWM_CAE5          0x20                                         // channel 5 center-aligned mode
  #define PWM_CAE6          0x40                                         // channel 6 center-aligned mode
  #define PWM_CAE7          0x80                                         // channel 7 center-aligned mode
#define PWMCTL              *(unsigned char*)(PWM_ADD + 0x05)            // PWM Control Register
  #define PWM_PFRZ          0x04                                         // disable prescaler clock when core is in debug mode
  #define PWM_PSWAI         0x08                                         // disable prescaler clock when core is in doze mode
  #define PWM_CON01         0x10                                         // concatenate PWM channels 0 and 1 to form one 16 bit PWM channel
  #define PWM_CON23         0x20                                         // concatenate PWM channels 2 and 3 to form one 16 bit PWM channel
  #define PWM_CON45         0x40                                         // concatenate PWM channels 4 and 5 to form one 16 bit PWM channel
  #define PWM_CON67         0x80                                         // concatenate PWM channels 6 and 7 to form one 16 bit PWM channel
#define PWMSCLA             *(unsigned char*)(PWM_ADD + 0x08)            // PWM Scale A Register
#define PWMSCLB             *(unsigned char*)(PWM_ADD + 0x09)            // PWM Scale B Register
#define PWMCNT0             *(volatile unsigned char*)(PWM_ADD + 0x0c)   // PWM Channel 0 Counter Register
#define PWMCNT1             *(volatile unsigned char*)(PWM_ADD + 0x0d)   // PWM Channel 1 Counter Register
#define PWMCNT2             *(volatile unsigned char*)(PWM_ADD + 0x0e)   // PWM Channel 2 Counter Register
#define PWMCNT3             *(volatile unsigned char*)(PWM_ADD + 0x0f)   // PWM Channel 3 Counter Register
#define PWMCNT4             *(volatile unsigned char*)(PWM_ADD + 0x10)   // PWM Channel 4 Counter Register
#define PWMCNT5             *(volatile unsigned char*)(PWM_ADD + 0x11)   // PWM Channel 5 Counter Register
#define PWMCNT6             *(volatile unsigned char*)(PWM_ADD + 0x12)   // PWM Channel 6 Counter Register
#define PWMCNT7             *(volatile unsigned char*)(PWM_ADD + 0x13)   // PWM Channel 7 Counter Register
#define PWMPER0_ADD         (unsigned char*)(PWM_ADD + 0x14)             // PWM Channel 0 Period Register address
#define PWMPER0             *(unsigned char*)(PWM_ADD + 0x14)            // PWM Channel 0 Period Register
#define PWMPER1             *(unsigned char*)(PWM_ADD + 0x15)            // PWM Channel 1 Period Register
#define PWMPER2             *(unsigned char*)(PWM_ADD + 0x16)            // PWM Channel 2 Period Register
#define PWMPER3             *(unsigned char*)(PWM_ADD + 0x17)            // PWM Channel 3 Period Register
#define PWMPER4             *(unsigned char*)(PWM_ADD + 0x18)            // PWM Channel 4 Period Register
#define PWMPER5             *(unsigned char*)(PWM_ADD + 0x19)            // PWM Channel 5 Period Register
#define PWMPER6             *(unsigned char*)(PWM_ADD + 0x1a)            // PWM Channel 6 Period Register
#define PWMPER7             *(unsigned char*)(PWM_ADD + 0x1b)            // PWM Channel 7 Period Register
#define PWMDTY0_ADD         (unsigned char*)(PWM_ADD + 0x1c)             // PWM Channel 0 Duty Register address
#define PWMDTY0             *(unsigned char*)(PWM_ADD + 0x1c)            // PWM Channel 0 Duty Register
#define PWMDTY1             *(unsigned char*)(PWM_ADD + 0x1d)            // PWM Channel 1 Duty Register
#define PWMDTY2             *(unsigned char*)(PWM_ADD + 0x1e)            // PWM Channel 2 Duty Register
#define PWMDTY3             *(unsigned char*)(PWM_ADD + 0x1f)            // PWM Channel 3 Duty Register
#define PWMDTY4             *(unsigned char*)(PWM_ADD + 0x20)            // PWM Channel 4 Duty Register
#define PWMDTY5             *(unsigned char*)(PWM_ADD + 0x21)            // PWM Channel 5 Duty Register
#define PWMDTY6             *(unsigned char*)(PWM_ADD + 0x22)            // PWM Channel 6 Duty Register
#define PWMDTY7             *(unsigned char*)(PWM_ADD + 0x23)            // PWM Channel 7 Duty Register
#define PWMSDN              *(volatile unsigned char*)(PWM_ADD + 0x24)   // PWM Shutdown Register
  #define SDNEN             0x01                                         // emergency shutdown enable
  #define PWM7IL            0x02                                         // PWM7 input level polarity
  #define PWM7IN            0x04                                         // current status of PWM7 pin
  #define PWMLVL            0x10                                         // PWM outputs forced to level 1 when shutdown asserted
  #define PWM_RESTART       0x20                                         // restart the PWM operation when coubter reaches zero - self clearing, always read as '0'
  #define PWMIE             0x40                                         // PWM interrupt enable
  #define PWMIF             0x80                                         // PWM interrupt flag - due to PWM7IN input change
  #if defined  _M5225X
    #define PCME            *(volatile unsigned char*)(PWM_ADD + 0x26)   // PWM PCM mode
        #define PCME0       0x01                                         // PCM enabled on channel 0
        #define PCME1       0x02                                         // PCM enabled on channel 1
        #define PCME2       0x04                                         // PCM enabled on channel 2
        #define PCME3       0x08                                         // PCM enabled on channel 3
        #define PCME4       0x10                                         // PCM enabled on channel 4
        #define PCME5       0x20                                         // PCM enabled on channel 5
        #define PCME6       0x40                                         // PCM enabled on channel 6
        #define PCME7       0x80                                         // PCM enabled on channel 7
  #endif
#endif

// Ethernet Physical Transceiver
//
#define EPHYCTL0            *(unsigned char *)(EPHY_BASE_ADD + 0)
  #define EPHYEN  0x80                                                   // enable EPHY
  #define ANDIS   0x40                                                   // disable autonegotiation
  #define DIS100  0x20                                                   // disable 100Mb clocks 
  #define DIS10   0x10                                                   // disable 10Mb clocks 
  #define LEDEN   0x08                                                   // enable LEDs from PHY module
  #define EPHYWAI 0x04                                                   // allow disable PHY in wait mode
  #define EPHYIEN 0x01                                                   // enable PHY interrupts

#define EPHYCTL1            *(unsigned char *)(EPHY_BASE_ADD + 1)
#define EPHYSR              *(unsigned char *)(EPHY_BASE_ADD + 2)
  #define EPHYIF         0x01                                            // PHY interrupt occurred
  #define EPHY_100DIS    0x20                                            // 100BASE-TX disabled
  #define EPHY_10DIS     0x10                                            // 10BASE-TX disabled


#if defined PHY_MULTI_PORT                                               // {72}
    extern unsigned short _fnMIIread(unsigned char ucPort, unsigned char _mradr);
    extern void _fnMIIwrite(unsigned char ucPort, unsigned char _mradr, unsigned short _mwdata);
#else
    extern unsigned short _fnMIIread(unsigned char _mradr);              // {66}
    extern void _fnMIIwrite(unsigned char _mradr, unsigned short _mwdata);
#endif

// Random Number Generator Accelerator
//
#define RNGCR                *(volatile unsigned long *)(RNG_BASE_ADD + 0x0) // Random Number Generator Control Register
  #define RND_GO             0x00000001                                  // Generate output data
  #define RND_HA             0x00000002                                  // High Assurance bit (enable notification of security violations)
  #define RND_IM             0x00000004                                  // Interrupt Masked
  #define RND_CI             0x00000008                                  // Clear Interrupt
  #define RND_SLM            0x00000010                                  // Sleep mode
#define RNGSR                *(volatile unsigned long *)(RNG_BASE_ADD + 0x4) // Random Number Generator Status Register (read only)
  #define RND_SV             0x00000001                                  // Security Violation
  #define RND_LRS            0x00000002                                  // Last Read Status Bit
  #define RND_OUF            0x00000004                                  // Output Register Undeflow bit
  #define RND_EI             0x00000008                                  // Error Interrupt bit
  #define RND_SLP            0x00000010                                  // In Sleep mode
  #define RND_ORL_MASK       0x0000ff00                                  // Output ready level (1 when output is available)
#define RNGER                *(unsigned long *)(RNG_BASE_ADD + 0x8)      // Random Number Generator Entropy Register (write only)
#define RNGOUT               *(volatile unsigned long *)(RNG_BASE_ADD + 0xc) // Random Number Generator Out Register (read only)

// Fast Ethernet Controller
//
#define EIR                  *(volatile unsigned long *)(EMAC_BASE_ADD + 0x4) // Ethernet Interrupt Event Register
#define EIMR                 *(unsigned long *)(EMAC_BASE_ADD + 0x8)     // Ethernet Interrupt Mask Register
  #define HBERR              0x80000000                                  // Heart beat error interrupt/mask
  #define BABR               0x40000000                                  // Babbling receive error interrupt/mask
  #define BABT               0x20000000                                  // Babbling transmit error interrupt/mask
  #define GRA                0x10000000                                  // Graceful Stop Complete interrupt/mask
  #define TXF                0x08000000                                  // Transmit Frame interrupt/mask
  #define TXB                0x04000000                                  // Transmit Buffer interrupt/mask
  #define RXF                0x02000000                                  // Receive Frame interrupt/mask
  #define RXB                0x01000000                                  // Receive Buffer interrupt/mask
  #define MII                0x00800000                                  // MII interrupt/mask
  #define EBERR              0x00400000                                  // Ethernet Bus Error interrupt/mask
  #define LC                 0x00200000                                  // Late Collision interrupt/mask
  #define RL                 0x00100000                                  // Collision Retry Limit interrupt/mask
  #define UN                 0x00080000                                  // Transmit FIFO Underrun
#define RDAR                 *(unsigned long *)(EMAC_BASE_ADD + 0x10)    // Receive Descriptor Active Register
#define TDAR                 *(unsigned long *)(EMAC_BASE_ADD + 0x14)    // Transmit Descriptor Active Register
#define ECR                  *(volatile unsigned long *)(EMAC_BASE_ADD + 0x24) // Ethernet Control Register
  #define RESET_FEC          0x00000001
  #define ETHER_EN           0x00000002
#define MMFR                 *(volatile unsigned long *)(EMAC_BASE_ADD + 0x40) // MII Data Register
  #define MII_TA             0x00020000
  #define MII_ST             0x40000000
  #define WRITE_OP           0x10000000
  #define READ_OP            0x20000000

  #define MII_WRITE (MII_TA | MII_ST | WRITE_OP)
  #define MII_READ  (MII_TA | MII_ST | READ_OP)
#define MSCR                 *(unsigned long *)(EMAC_BASE_ADD + 0x44)    // MII Speed Control Register
  #define DIS_PREAMBLE       0x00000080
  #define MII_SPEED_60MHZ    (0xd  << 1)                                 // Correct setting for 60MHz system
#define MIBC                 *(unsigned long *)(EMAC_BASE_ADD + 0x64)    // MII Control/Status Register
  #define MIB_DISABLE        0x80000000
#define RCR                  *(unsigned long *)(EMAC_BASE_ADD + 0x84)    // Receive Control Register
  #define MAX_FRAME_LENGTH   0x05ee0000                                  // 1518 bytes
  #define PROM               0x00000008                                  // Promiscuous
  #define BC_REJ             0x00000010                                  // Reject Broadcast
  #define DRT                0x00000002                                  // Half duplex
  #define FCE                0x00000020                                  // Pause flow control
  #define FEC_MII_MODE       0x00000004                                  // MII mode
  #define LOOP               0x00000001                                  // Loopback
#define TCR                  *(unsigned long *)(EMAC_BASE_ADD + 0xc4)    // Transmit Control Register
  #define FDEN               0x00000004                                  // Full duplex
#define PALR                 *(unsigned long *)(EMAC_BASE_ADD + 0xe4)    // Physical Address Low Register
#define PAUR                 *(unsigned long *)(EMAC_BASE_ADD + 0xe8)    // Physical Address High and Type Field Register
#define OPD                  *(unsigned long *)(EMAC_BASE_ADD + 0xec)    // Op-code and Pause Duration
  #define PAUSE_DURATION     0x00000001
#define IAUR                 *(unsigned long *)(EMAC_BASE_ADD + 0x118)   // Hash table upper 32 bits
#define IALR                 *(unsigned long *)(EMAC_BASE_ADD + 0x11c)   // Hash table lower 32 bits
#define GAUR                 *(unsigned long *)(EMAC_BASE_ADD + 0x120)   // Group Hash table upper 32 bits
#define GALR                 *(unsigned long *)(EMAC_BASE_ADD + 0x124)   // Group Hash table lower 32 bits
#define TFWR                 *(unsigned long *)(EMAC_BASE_ADD + 0x144)   // Transmit FIFO Watermark
#define FRBR                 *(unsigned long *)(EMAC_BASE_ADD + 0x14c)   // FIFO Receive Bound Register
#define FRSR                 *(unsigned long *)(EMAC_BASE_ADD + 0x150)   // Recieve FIFO Start Registers
#define ERDSR                *(unsigned long *)(EMAC_BASE_ADD + 0x180)   // Pointer to Receive Descriptor Ring
#define ETDSR                *(unsigned long *)(EMAC_BASE_ADD + 0x184)   // Pointer to Transmit Descriptor Ring
#define EMRBR                *(unsigned long *)(EMAC_BASE_ADD + 0x188)   // Maximum Receive Buffer Size


#define MIBRAM_ADD           (unsigned long *)(EMAC_BASE_ADD + 0x200)
#define MIB_RAM_SIZE         512

#define RMON_T_DROP          MIBRAM_ADD                                  // count of frames not counted correctly {64}
#define RMON_T_PACKETS       *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x04) // count of TX packets
#define RMON_T_BC_PKT        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x08) // count of TX broadcast packets
#define RMON_T_MC_PKT        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x0c) // count of TX multicast packets
#define RMON_T_CRC_ALIGN     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x10) // count of TX packets with CRC Align error
#define RMON_T_UNDERSIZE     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x14) // count of TX packets < 64 bytes with good CRC
#define RMON_T_OVERSIZE      *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x18) // count of TX packets > MAX_FL bytes with good CRC
#define RMON_T_FRAG          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x1c) // count of TX packets < 64 bytes with bad CRC
#define RMON_T_JAB           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x20) // count of TX packets > MAX_FL bytes with bad CRC
#define RMON_T_COL           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x24) // count of TX collisions
#define RMON_T_P64           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x28) // count of TX 64 byte packets
#define RMON_T_P65TO127      *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x2c) // count of TX 65 to 127 byte packets
#define RMON_T_P128TO255     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x30) // count of TX 128 to 255 byte packets
#define RMON_T_P256TO511     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x34) // count of TX 256 to 511 byte packets
#define RMON_T_P512TO1023    *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x38) // count of TX 512 to 1023 byte packets
#define RMON_T_P1024TO2047   *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x3c) // count of TX 1024 to 2047 byte packets
#define RMON_T_P_GTE2048     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x40) // count of TX >2048 byte packets
#define RMON_T_OCTETS        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x44) // count of TX octets
#define IEEE_T_DROP          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x48) // count of TX frame not counted correctly
#define IEEE_T_FRAME_OK      *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x4c) // count of TX frames transmitted OK
#define IEEE_T_1COL          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x50) // count of TX frames transmitted with single collision
#define IEEE_T_MCOL          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x54) // count of TX frames transmitted with multiple collision
#define IEEE_T_DEF           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x58) // count of TX frames transmitted after deferral delay
#define IEEE_T_LCOL          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x5c) // count of TX frames transmitted with late collision
#define IEEE_T_EXCOL         *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x60) // count of TX frames transmitted with excessive collisions
#define IEEE_T_MACERR        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x64) // count of TX frames transmitted with TX FIFO underrun
#define IEEE_T_CSERR         *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x68) // count of TX frames transmitted with carrier sense error
#define IEEE_T_SQE           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x6c) // count of TX frames transmitted with SQE error
#define IEEE_T_FDFXC         *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x70) // count of TX flow control pause frames transmitted
#define IEEE_T_OCTETS_OK     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x74) // count of TX octets transmitted without error

#define RMON_R_PACKETS       *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x84) // count of RX packets
#define RMON_R_BC_PKT        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x88) // count of RX broadcast packets
#define RMON_R_MC_PKT        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x8c) // count of RX multicast packets
#define RMON_R_CRC_ALIGN     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x90) // count of RX packets with CRC Align error
#define RMON_R_UNDERSIZE     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x94) // count of RX packets < 64 bytes with good CRC
#define RMON_R_OVERSIZE      *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x98) // count of RX packets > MAX_FL bytes with good CRC
#define RMON_R_FRAG          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0x9c) // count of RX packets < 64 bytes with bad CRC
#define RMON_R_JAB           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xa0) // count of RX packets > MAX_FL bytes with bad CRC
#define RMON_R_P64           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xa8) // count of RX 64 byte packets
#define RMON_R_P65TO127      *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xac) // count of RX 65 to 127 byte packets
#define RMON_R_P128TO255     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xb0) // count of RX 128 to 255 byte packets
#define RMON_R_P256TO511     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xb4) // count of RX 256 to 511 byte packets
#define RMON_R_P512TO1023    *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xb8) // count of RX 512 to 1023 byte packets
#define RMON_R_P1024TO2047   *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xbc) // count of RX 1024 to 2047 byte packets
#define RMON_R_P_GTE2048     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xc0) // count of RX >2048 byte packets
#define RMON_R_OCTETS        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xc4) // count of RX octets
#define IEEE_R_DROP          *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xc8) // count of RX frame not counted correctly
#define IEEE_R_FRAME_OK      *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xcc) // count of RX frames received OK
#define IEEE_R_CRC           *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xd0) // count of RX frames received with CRC error
#define IEEE_R_ALIGN         *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xd4) // count of RX frames received with alignment error
#define IEEE_R_MACERR        *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xd8) // count of RX receive FIFO overflows
#define IEEE_R_FDFXC         *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xdc) // count of RX flow control pause frames received
#define IEEE_R_OCTETS_OK     *(volatile unsigned long*)(EMAC_BASE_ADD + 0x200 + 0xe0) // count of RX octets received witho


typedef struct stM5223X_FEC_BD
{
  unsigned short usBDControl;
  unsigned short usBDLength;
  unsigned char *ptrBD_Data;
} M5223X_FEC_BD;

#define EMPTY_BUFFER         0x8000                                      // RX BD Control bits
#define WRAP_BIT             0x2000
#define LAST_IN_FRAME        0x0800
#define TRUNCATED_FRAME      0x0001
#define OVERRUN_FRAME        0x0002

#define READY_TX             0x8000                                      // TX BD Control bits
#define LAST_IN_FRAME        0x0800
#define TX_CRC               0x0400

// FLASH module configuration field
//
#define BACKDOOR_COMP_KEY    *(unsigned char *)(CFC_BASE_ADD + 0x0)      // Back door comparison key (8 bytes)
#define FLASH_PROTECTION     *(unsigned char *)(CFC_BASE_ADD + 0x8)      // FLASH protection bytes (4 bytes)
#define FLASH_SUPV           *(unsigned char *)(CFC_BASE_ADD + 0xc)      // FLASH supervisor access bytes (4 bytes)
#define FLASH_DATA           *(unsigned char *)(CFC_BASE_ADD + 0x10)     // FLASH data access bytes (4 bytes)
#define FLASH_SECURITY       *(unsigned char *)(CFC_BASE_ADD + 0x14)     // FLASH security word (4 bytes)


#if defined _M5222X || defined _M5221X || defined _M5225X
// USB Controller
//
#define PER_ID               *(unsigned char *)(USB_BASE_ADD + 0x000)    // Peripheral ID Register (read-only)
#define ID_COMP              *(unsigned char *)(USB_BASE_ADD + 0x004)    // Peripheral ID Compliment Register (read-only)
#define REV                  *(unsigned char *)(USB_BASE_ADD + 0x008)    // Peripheral Revision Register (read-only)
#define ADD_INFO             *(unsigned char *)(USB_BASE_ADD + 0x00c)    // Peripheral Additional Info Register (read-only)
  #define IEHOST             0x01
#define OTG_INT_STAT         *(volatile unsigned char *)(USB_BASE_ADD + 0x010) // OTG Interrupt Status Register
  #define A_VBUS_CHG         0x01
  #define BSESS_CHG          0x04
  #define SESS_VLD_CHG       0x08
  #define LINE_STAT_CHG      0x20
  #define MSEC_1             0x40
  #define ID_CHG             0x80
#define OTG_INT_EN           *(unsigned char *)(USB_BASE_ADD + 0x014)    // OTG Interrupt Control Register
#define OTG_STATUS           *(volatile unsigned char *)(USB_BASE_ADD + 0x018) // OTG Status Register
#define OTG_CTRL             *(unsigned char *)(USB_BASE_ADD + 0x01c)    // OTG Control Register
  #define VBUS_DSCHG         0x01                                        // VBUS signal discharged through a resistor
  #define VBUS_CHG           0x02                                        // VBUS signal charged through a resistor
  #define OTG_EN             0x04                                        // Pull up / down resistors enabled in the module
  #define VBUS_ON            0x08                                        // VBUS power is on
  #define DP_LOW_DM_LOW      0x30                                        // D+ and D- pull down resistors enabled
//#define DM_HIGH            0x40                                        // D- pull up resistor enabled
  #define DP_HIGH            0x80                                        // D+ pull up resistor enabled
#define INT_STAT             *(volatile unsigned char *)(USB_BASE_ADD + 0x080) // Interrupt Status Register
  #define USB_RST            0x01
  #define USB_ERROR          0x02
  #define SOF_TOK            0x04
  #define TOK_DNE            0x08
  #define SLEEP              0x10
  #define RESUME             0x20
  #define ATTACH             0x40
  #define STALL              0x80
#define INT_ENB              *(unsigned char *)(USB_BASE_ADD + 0x084)    // Interrupt Enable Register
  #define USB_RST_EN         0x01
  #define USB_ERROR_EN       0x02
  #define SOF_TOK_EN         0x04
  #define TOK_DNE_EN         0x08
  #define SLEEP_EN           0x10
  #define RESUME_EN          0x20
  #define ATTACH_EN          0x40
  #define STALL_EN           0x80
#define ERR_STAT             *(volatile unsigned char *)(USB_BASE_ADD + 0x088) // Error Interrupt Status Register
  #define PID_ERR            0x01
  #define CRC5_EOF           0x02
  #define CRC16              0x04
  #define DFN8               0x08
  #define BTO_ERR            0x10
  #define DMA_ERR            0x20
  #define BTS_ERR            0x80
#define ERR_ENB              *(unsigned char *)(USB_BASE_ADD + 0x08c)    // Error Interrupt Enable Register
  #define PID_ERR_EN         0x01
  #define CRC5_EOF_EN        0x02
  #define CRC16_EN           0x04
  #define DFN8_EN            0x08
  #define BTO_ERR_EN         0x10
  #define DMA_ERR_EN         0x20
  #define BTS_ERR_EN         0x80
#define STAT                 *(volatile unsigned char *)(USB_BASE_ADD + 0x090) // Status Register
  #define ODD_BANK           0x04
  #define TX_TRANSACTION     0x08
  #define END_POINT_SHIFT    4
#define CTL                  *(volatile unsigned char *)(USB_BASE_ADD + 0x094) // Control Register
  #define USB_EN_SOF_EN      0x01                                        // Enable USB module
  #define ODD_RST            0x02                                        // Reset all Odd BDTs
  #define CTL_RESUME         0x04                                        // Excecute resume signalling
  #define HOST_MODE_EN       0x08                                        // Host mode of operation
  #define USB_RESET          0x10                                        // Enable USB Reset signalling
  #define TXSUSPEND_TOKENBUSY 0x20                                       // 
  #define SE0                0x40                                        // Live USB Single ended Zero signal
  #define JSTATE             0x80                                        // Live USB differential receiver JSTATE signal
#define ADDR                 *(unsigned char *)(USB_BASE_ADD + 0x098)    // Address Register
  #define LS_EN              0x80                                        // Low Speed Enable Bit
#define BDT_PAGE_01          *(unsigned char *)(USB_BASE_ADD + 0x09c)    // BDT Page Register 1
#define FRM_NUML             *(unsigned char *)(USB_BASE_ADD + 0x0a0)    // Frame Number Register Low
#define FRM_NUMH             *(unsigned char *)(USB_BASE_ADD + 0x0a4)    // Frame Number Register High
#define TOKEN                *(unsigned char *)(USB_BASE_ADD + 0x0a8)    // Token Register
#define SOF_THLD             *(unsigned char *)(USB_BASE_ADD + 0x0ac)    // SOF Threshold Register
#define BDT_PAGE_02          *(unsigned char *)(USB_BASE_ADD + 0x0b0)    // BDT Page Register 2
#define BDT_PAGE_03          *(unsigned char *)(USB_BASE_ADD + 0x0b4)    // BDT Page Register 3
#define ENDPT0_ADD           (unsigned char *)(USB_BASE_ADD + 0x0c0)     // Endpoint Control Register 0 address
#define ENDPT0               *(volatile unsigned char *)(USB_BASE_ADD + 0x0c0)    // Endpoint Control Register 0
  #define EP_HSHK            0x01                                        // Enable Endpoint handshake during transaction
  #define EP_STALL           0x02                                        // Endpoint stalled
  #define EP_TX_ENABLE       0x04                                        // Enable tx transfers in endpoint
  #define EP_RX_ENABLE       0x08                                        // Enable rx transfers in endpoint
  #define EP_CTL_DIS         0x10                                        // Disable control transfers
  #define RETRY_DIS          0x20                                        // (only host mode in endpoint 0)
  #define HOST_WO_HUB        0x80                                        // (only host mode in endpoint 0)
#define ENDPT1               *(volatile unsigned char *)(USB_BASE_ADD + 0x0c4) // Endpoint Control Register 1
#define ENDPT2               *(volatile unsigned char *)(USB_BASE_ADD + 0x0c8) // Endpoint Control Register 2
#define ENDPT3               *(volatile unsigned char *)(USB_BASE_ADD + 0x0cc) // Endpoint Control Register 3
#define ENDPT4               *(volatile unsigned char *)(USB_BASE_ADD + 0x0d0) // Endpoint Control Register 4
#define ENDPT5               *(volatile unsigned char *)(USB_BASE_ADD + 0x0d4) // Endpoint Control Register 5
#define ENDPT6               *(volatile unsigned char *)(USB_BASE_ADD + 0x0d8) // Endpoint Control Register 6
#define ENDPT7               *(volatile unsigned char *)(USB_BASE_ADD + 0x0dc) // Endpoint Control Register 7
#define ENDPT8               *(volatile unsigned char *)(USB_BASE_ADD + 0x0e0) // Endpoint Control Register 8
#define ENDPT9               *(volatile unsigned char *)(USB_BASE_ADD + 0x0e4) // Endpoint Control Register 9
#define ENDPT10              *(volatile unsigned char *)(USB_BASE_ADD + 0x0e8) // Endpoint Control Register 10
#define ENDPT11              *(volatile unsigned char *)(USB_BASE_ADD + 0x0ec) // Endpoint Control Register 11
#define ENDPT12              *(volatile unsigned char *)(USB_BASE_ADD + 0x0f0) // Endpoint Control Register 12
#define ENDPT13              *(volatile unsigned char *)(USB_BASE_ADD + 0x0f4) // Endpoint Control Register 13
#define ENDPT14              *(volatile unsigned char *)(USB_BASE_ADD + 0x0f8) // Endpoint Control Register 14
#define ENDPT15              *(volatile unsigned char *)(USB_BASE_ADD + 0x0fc) // Endpoint Control Register 15
#define USB_CTRL             *(volatile unsigned char *)(USB_BASE_ADD + 0x100) // USB Control Register
  #define USB_OSCILLATOR_CLOCK 0x01
  #define USB_SYSTEM_CLOCK     0x03
  #define USB_ALT_CLOCK        0x00
  #define PDE                  0x40                                      // {25} enable weak pull down resistors
  #define SUSP                 0x80                                      // place the USB transceiver into the suspend state
#define USB_OTG_OBSERVE      *(unsigned char *)(USB_BASE_ADD + 0x104)    // USB OTG Observe Register
#define USB_OTG_CTRL         *(unsigned char *)(USB_BASE_ADD + 0x108)    // USB OTG Control Register

#define NUMBER_OF_USB_ENDPOINTS 16                                       // M5222X/M5221X support 16 end points

//#define _SUPRESS_REPEAT_DATA                                           // repeated data not supressed due to problems when flashing memory at the same time

typedef struct stM5222X_USB_BD
{
    unsigned long  ulUSB_BDControl;                                      // This is saved in little-endian format
    unsigned char *ptrUSB_BD_Data;                                       // This is saved in little-endian format
} M5222X_USB_BD;

#if defined _M5225X                                                      // this device supports big-endian mode so it is used for efficiency
    #define BE_TOK_PID_0         0x00000004                              // big-endian long word defines
    #define BE_BDT_STALL         0x00000004                              // issue STALL handschake
    #define BE_TOK_PID_1         0x00000008
    #define BE_DTS               0x00000008                              // Data Toggle Synchronisation
    #define BE_TOK_PID_2         0x00000010
    #define BE_NINC              0x00000010                              // No INCrement bit - DMA engine doesn't increment address after each byte transfer
    #define BE_TOK_PID_3         0x00000020
    #define BE_KEEP              0x00000020                              // KEEP ownership at USB-FS
    #define BE_DATA_1            0x00000040
    #define BE_OWN               0x00000080                              // USB-FS has exclusive ownership of the buffer descriptor
    #define BE_USB_BYTE_CNT_MASK 0x03ff0000    
    #define BE_USB_CNT_SHIFT     16
    #define BE_RX_PID_MASK       (TOK_PID_3 | TOK_PID_2 | TOK_PID_1 | TOK_PID_0)
    #define BE_RX_PID_SHIFT      2
#endif
#define TOK_PID_0                0x04000000                              // little-endian long word defines
#define BDT_STALL                0x04000000                              // issue STALL handschake
#define TOK_PID_1                0x08000000
#define DTS                      0x08000000                              // Data Toggle Synchronisation
#define TOK_PID_2                0x10000000
#define NINC                     0x10000000                              // No INCrement bit - DMA engine doesn't increment address after each byte transfer
#define TOK_PID_3                0x20000000
#define KEEP                     0x20000000                              // KEEP ownership at USB-FS
#define DATA_1                   0x40000000
#define OWN                      0x80000000                              // USB-FS has exclusive ownership of the buffer descriptor
#define USB_BYTE_CNT_MASK        0x0000ff03    
#define USB_BYTE_CNT_SHIFT_LSB   8
#define USB_BYTE_CNT_SHIFT_MSB   8
#define RX_PID_MASK              (TOK_PID_3 | TOK_PID_2 | TOK_PID_1 | TOK_PID_0)
#define RX_PID_SHIFT             26


// BDT construction
// 4 BDs per endpoint
// Rx - even
// Rx - odd
// Tx - even
// Tx - odd
#define USB_BD_SIZE          sizeof(M5222X_USB_BD)                       // size in bytes of a single USB buffer descriptor
#define ENDPOINT_BD_SIZE     (4 * USB_BD_SIZE)
#define OFFSET_RX_EVEN       0                                           // offset in long word steps
#define OFFSET_RX_ODD        1
#define OFFSET_TX_EVEN       2
#define OFFSET_TX_ODD        3

typedef struct stM5222X_USB_ENDPOINT_BD
{
    M5222X_USB_BD usb_bd_rx_even;
    M5222X_USB_BD usb_bd_rx_odd;
    M5222X_USB_BD usb_bd_tx_even;
    M5222X_USB_BD usb_bd_tx_odd;
} M5222X_USB_ENDPOINT_BD;

typedef struct stUSB_END_POINT
{
    unsigned long ulNextRxData0;
    unsigned long ulNextTxData0;
    unsigned long ulEndpointSize;                                        // contains size of endpoint plus some control flags
} USB_END_POINT;

#define ALTERNATE_TX_BUFFER    DATA_1                                    // any bit not clashing with the length in the buffer descriptor control word (used as SW flag)
#define RX_DATA_TOGGLE         OWN                                       // any bit not clashing with DATA_1 in the buffer descriptor control word (used as SW flag)

#define USB_FIFO_BUFFER_DEPTH  2                                         // M5222X/M5221X has two rx and tx buffers per endpoint

// This hardware specific structure allows generic USB routines to control specific hardware using macros
//
typedef struct stUSB_HW
{
    unsigned long  ulRxControl;
    unsigned long *ptr_ulUSB_BDControl;                                  // pointer to the presently valid buffer descriptor control entry
    USB_END_POINT *ptrEndpoint;
    unsigned char **ptrTxDatBuffer;                                      // pointer to the next transmission buffer pointer
    unsigned char **ptrRxDatBuffer;                                      // {76} pointer to the next reception buffer pointer
    unsigned short usLength;                                             // length information
    unsigned char  ucUSBAddress;                                         // our USB address                       
} USB_HW;

#if defined _M5225X && defined USB_BIGENDIAN                             // this device's controller can work in big-endian so we avoid conversions for efficiency
    #define fnLE_add_F(x) (void *)x                                      // {36}
#else
    extern void *fnLE_add_F(unsigned long long_word);                    // {36} conversion routine to little endian format as used by some USB pointers in the device, after converting input address in FLASH to their backdoor address
#endif

#if defined _M5225X && defined USB_BIGENDIAN                             // this device supports big-endian mode so it is used for efficiency
    // MACRO for transmitting USB data from generic USB routines
    //
    #define FNSEND_USB_DATA(pData, Len, iEndpoint, ptrUSB_HW) *ptrUSB_HW->ptrTxDatBuffer = fnLE_add_F((CAST_POINTER_ARITHMETIC)pData); \
            *ptrUSB_HW->ptr_ulUSB_BDControl = (unsigned long)(((Len & USB_BYTE_CNT_MASK) << USB_CNT_SHIFT) | (OWN | ptrUSB_HW->ptrEndpoint->ulNextTxData0)); _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW); ptrUSB_HW->ptrEndpoint->ulNextTxData0 ^= DATA_1; ptrUSB_HW->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;

    // MACRO for transmitting zero data packet to control endpoints from generic USB routines
    //
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint) *ptrUSB_HW->ptr_ulUSB_BDControl = (unsigned long)(OWN | DTS | ptrUSB_HW->ptrEndpoint->ulNextTxData0); _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW); ptrUSB_HW->ptrEndpoint->ulNextTxData0 ^= DATA_1; ptrUSB_HW->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;

    // MACROs for extracting frame length from a buffer descriptor
    //
    #define GET_FRAME_LENGTH()      (unsigned short)((unsigned short)((ptUSB_BD->ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_CNT_SHIFT))

    #define SET_FRAME_LENGTH(x)     (x << USB_CNT_SHIFT)
#else
    // MACRO for transmitting USB data from generic USB routines
    //
    #define FNSEND_USB_DATA(pData, Len, iEndpoint, ptrUSB_HW) *ptrUSB_HW->ptrTxDatBuffer = fnLE_add_F((CAST_POINTER_ARITHMETIC)pData); \
            *ptrUSB_HW->ptr_ulUSB_BDControl = (unsigned long)(((Len & 0xff) << USB_BYTE_CNT_SHIFT_LSB) | ((Len >> USB_BYTE_CNT_SHIFT_MSB) & 0x03) | (OWN | ptrUSB_HW->ptrEndpoint->ulNextTxData0)); _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW); ptrUSB_HW->ptrEndpoint->ulNextTxData0 ^= DATA_1; ptrUSB_HW->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;

    // MACRO for transmitting zero data packet to control endpoints from generic USB routines
    //
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint) *ptrUSB_HW->ptr_ulUSB_BDControl = (unsigned long)(OWN | DTS | ptrUSB_HW->ptrEndpoint->ulNextTxData0); _SIM_USB(USB_SIM_TX, iEndpoint, ptrUSB_HW); ptrUSB_HW->ptrEndpoint->ulNextTxData0 ^= DATA_1; ptrUSB_HW->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;

    // MACRO for extracting frame length from a buffer descriptor
    //
    #define GET_FRAME_LENGTH()      (unsigned short)((unsigned short)((ptUSB_BD->ulUSB_BDControl & USB_BYTE_CNT_MASK) >> USB_BYTE_CNT_SHIFT_LSB) | ((unsigned short)(ptUSB_BD->ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB)))

    #define SET_FRAME_LENGTH(x)     (((x & 0xff) << USB_BYTE_CNT_SHIFT_LSB) | (x >> USB_BYTE_CNT_SHIFT_MSB))
#endif
#define SET_CONTROL_DIRECTION(a,b)                                       // {26} direction control not needed by M522XX
#define CLEAR_SETUP(a)                                                   // for compatibility with generic driver
#define VALIDATE_NEW_CONFIGURATION()                                     // {52}
#endif                                                                   // endif USB device

#if !defined _M5222X && !defined _M5221X && !defined _M521XX
// CAN Configuration module
//
#define CANMCR               *(volatile unsigned long *)(CAN_BASE_ADD + 0x0) // CAN Module Configuration Register (supervisor only)
  #define CAN_MDIS           0x80000000
  #define CAN_FRZ            0x40000000
  #define CAN_HALT           0x10000000
  #define CAN_NOTRDY         0x08000000
  #define CAN_SOFTRST        0x02000000
  #define CAN_FRZACK         0x01000000
  #define CAN_SUPV           0x00800000
  #define CAN_LPMACK         0x00100000
#define CANCTRL              *(unsigned long *)(CAN_BASE_ADD + 0x4)      // CAN Control Register
  #define CAN_DIV_3          (0x02 << 24)
  #define EXTAL_CLK_SOURCE   0                                           // use crystal source for clock (lower jitter than PLL)
  #define RJW_0              (0 << 2)
  #define RJW_1              (1 << 22)
  #define RJW_2              (2 << 22)
  #define RJW_3              (3 << 22)
  #define PHASE_BUF_SEG1_LEN1 (0 << 19)
  #define PHASE_BUF_SEG1_LEN2 (1 << 19)
  #define PHASE_BUF_SEG1_LEN3 (2 << 19)
  #define PHASE_BUF_SEG1_LEN4 (3 << 19)
  #define PHASE_BUF_SEG1_LEN5 (4 << 19)
  #define PHASE_BUF_SEG1_LEN6 (5 << 19)
  #define PHASE_BUF_SEG1_LEN7 (6 << 19)
  #define PHASE_BUF_SEG1_LEN8 (7 << 19)
  #define PHASE_BUF_SEG2_LEN2 (1 << 16)
  #define PHASE_BUF_SEG2_LEN3 (2 << 16)
  #define PHASE_BUF_SEG2_LEN4 (3 << 16)
  #define PHASE_BUF_SEG2_LEN5 (4 << 16)
  #define PHASE_BUF_SEG2_LEN6 (5 << 16)
  #define PHASE_BUF_SEG2_LEN7 (6 << 16)
  #define PHASE_BUF_SEG2_LEN8 (7 << 16)
  #define BOFFMSK             0x00008000                                 // Bus off interrupt mask
  #define ERRMSK              0x00004000                                 // Error interrupt mask
  #define CLK_SRC_INT_BUS     0x00002000                                 // Clock source fsys/23
  #define LPB                 0x00001000                                 // Loop Back Mode
  #define SMP                 0x00000080                                 // Sampling Mode - three samples decide rx value
  #define BOFFREC             0x00000040                                 // Bus off recovery mode disable
  #define TSYN                0x00000020                                 // Timer synchronisation
  #define LBUF                0x00000010                                 // Lowest number buffer transmitted first
  #define LOM                 0x00000008                                 // Listen-only mode
  #define PROPSEG_BIT_TIME1   (0)
  #define PROPSEG_BIT_TIME2   (1)
  #define PROPSEG_BIT_TIME3   (2)
  #define PROPSEG_BIT_TIME4   (3)
  #define PROPSEG_BIT_TIME5   (4)
  #define PROPSEG_BIT_TIME6   (5)
  #define PROPSEG_BIT_TIME7   (6)
  #define PROPSEG_BIT_TIME8   (7)
#define CAN_TIMER            *(unsigned long *)(CAN_BASE_ADD + 0x8)      // CAN Free running timer

#define RXGMASK              *(unsigned long *)(CAN_BASE_ADD + 0x10)     // CAN Rx Global mask
#define RX14MASK             *(unsigned long *)(CAN_BASE_ADD + 0x14)     // CAN Rx Buffer 14 mask
#define RX15MASK             *(unsigned long *)(CAN_BASE_ADD + 0x18)     // CAN Rx Buffer 15 mask
#define CAN_ERRCNT           *(volatile unsigned long *)(CAN_BASE_ADD + 0x1c) // CAN Error Counter Register
#define CAN_ERRSTAT          *(volatile unsigned long *)(CAN_BASE_ADD + 0x20) // CAN Error and Status Register
  #define BIT1ERROR          0x00008000
  #define BIT0ERROR          0x00004000
  #define CAN_ACK_ERR        0x00002000
  #define CAN_CRC_ERR        0x00001000
  #define CAN_FRM_ERR        0x00000800
  #define CAN_STF_ERR        0x00000400
  #define TXWRN              0x00000200
  #define RXWRN              0x00000100
  #define CAN_BUS_IDLE       0x00000080
  #define CAN_ERROR_PASSIVE  0x00000010
  #define BOFFINT            0x00000004
  #define ERRINT             0x00000002
#define CAN_IMASK            *(unsigned long *)(CAN_BASE_ADD + 0x28)     // CAN Interrupt Mask Register
#define CAN_IFLAG            *(unsigned long *)(CAN_BASE_ADD + 0x30)     // CAN Interrupt Flag Register
  #define CAN_BUF_0_INT      0x00000001
  #define CAN_BUF_1_INT      0x00000002
  #define CAN_BUF_2_INT      0x00000004
  #define CAN_BUF_3_INT      0x00000008
  #define CAN_BUF_4_INT      0x00000010
  #define CAN_BUF_5_INT      0x00000020
  #define CAN_BUF_6_INT      0x00000040
  #define CAN_BUF_7_INT      0x00000080
  #define CAN_BUF_8_INT      0x00000100
  #define CAN_BUF_9_INT      0x00000200
  #define CAN_BUF_10_INT     0x00000400
  #define CAN_BUF_11_INT     0x00000800
  #define CAN_BUF_12_INT     0x00001000
  #define CAN_BUF_13_INT     0x00002000
  #define CAN_BUF_14_INT     0x00004000
  #define CAN_BUF_15_INT     0x00008000

  #define CAN_ALL_BUFFERS_INT (CAN_BUF_0_INT | CAN_BUF_1_INT | CAN_BUF_2_INT | CAN_BUF_3_INT | CAN_BUF_4_INT | CAN_BUF_5_INT | CAN_BUF_6_INT | CAN_BUF_7_INT | CAN_BUF_8_INT | CAN_BUF_9_INT | CAN_BUF_10_INT | CAN_BUF_11_INT | CAN_BUF_12_INT | CAN_BUF_13_INT | CAN_BUF_14_INT | CAN_BUF_15_INT)

#define MAX_TX_CAN_TRIES     5                                           // let the controller try this many times before declaring a transmission error

#define MB_RX_INACTIVE       0x00000000
#define MB_RX_EMPTY          0x04000000
#define MB_RX_FULL           0x02000000
#define MB_RX_OVERRUN        0x04000000
#define MB_RX_BUSY_BIT       0x01000000
#define MB_TX_INACTIVE       0x08000000
#define MB_TX_SEND_ONCE      0x0c000000
#define MB_TX_SEND_ON_REQ    0x0a000000

#define CAN_CODE_FIELD       0x0f000000                                  // CODE field mask
#define CAN_RX_CODE_FIELD    0x0a000000

#define SRR                  0x00400000                                  // Substitute remote request
#define IDE                  0x00200000                                  // ID extended bit
#define RTR                  0x00100000                                  // Remote transmission request

#define CAN_LENGTH_AND_TIME  0x000fffff
#define CAN_KEEP_CONTENTS    0x00ffffff
#define CAN_EXTENDED_MASK    0x1fffffff
#define CAN_STANDARD_MASK    0x7ff
#define CAN_STANDARD_BITMASK 0x1ffc0000
#define CAN_STANDARD_SHIFT   18

#define MBUFF0_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x80)   // CAN Message Buffer 0 (16 bytes)
#define MBUFF1_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x90)   // CAN Message Buffer 1 (16 bytes)
#define MBUFF2_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0xa0)   // CAN Message Buffer 2 (16 bytes)
#define MBUFF3_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0xb0)   // CAN Message Buffer 3 (16 bytes)
#define MBUFF4_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0xc0)   // CAN Message Buffer 4 (16 bytes)
#define MBUFF5_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0xd0)   // CAN Message Buffer 5 (16 bytes)
#define MBUFF6_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0xe0)   // CAN Message Buffer 6 (16 bytes)
#define MBUFF7_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0xf0)   // CAN Message Buffer 7 (16 bytes)
#define MBUFF8_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x100)  // CAN Message Buffer 8 (16 bytes)
#define MBUFF9_ADD           (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x110)  // CAN Message Buffer 9 (16 bytes)
#define MBUFF10_ADD          (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x120)  // CAN Message Buffer 10 (16 bytes)
#define MBUFF11_ADD          (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x130)  // CAN Message Buffer 11 (16 bytes)
#define MBUFF12_ADD          (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x140)  // CAN Message Buffer 12 (16 bytes)
#define MBUFF13_ADD          (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x150)  // CAN Message Buffer 13 (16 bytes)
#define MBUFF14_ADD          (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x160)  // CAN Message Buffer 14 (16 bytes)
#define MBUFF15_ADD          (COLDFIRE_CAN_BUF *)(CAN_BASE_ADD + 0x170)  // CAN Message Buffer 15 (16 bytes)

#define NUMBER_CAN_MESSAGE_BUFFERS 16

#define NUMBER_OF_CAN_INTERFACES 1                                       // {65}

typedef struct stCOLDFIRE_CAN_CONTROL                                    // {65}
{
    volatile unsigned long CAN_MCR;
    volatile unsigned long CAN_CTRL1;
    volatile unsigned long _CAN_TIMER;
    unsigned long res0;
    unsigned long CAN_RXGMASK;
    unsigned long CAN_RX14MASK;
    unsigned long CAN_RX15MASK;
    volatile unsigned long CAN_ECR;
    volatile unsigned long CAN_ESR1;
    unsigned long CAN_IMASK2;
    unsigned long CAN_IMASK1;
    volatile unsigned long CAN_IFLAG2;
    volatile unsigned long CAN_IFLAG1;
    unsigned long CAN_CTRL2;
    volatile unsigned long CAN_ESR2;
    unsigned long res1[2];
    volatile unsigned long CAN_CRCR;
    unsigned long CAN_RXFGMASK;
    volatile unsigned long CAN_RXFIR;
    unsigned long res2[524];
    unsigned long CAN_RXIMR0;
    unsigned long CAN_RXIMR1;
    unsigned long CAN_RXIMR2;
    unsigned long CAN_RXIMR3;
    unsigned long CAN_RXIMR4;
    unsigned long CAN_RXIMR5;
    unsigned long CAN_RXIMR6;
    unsigned long CAN_RXIMR7;
    unsigned long CAN_RXIMR8;
    unsigned long CAN_RXIMR9;
    unsigned long CAN_RXIMR10;
    unsigned long CAN_RXIMR11;
    unsigned long CAN_RXIMR12;
    unsigned long CAN_RXIMR13;
    unsigned long CAN_RXIMR14;
    unsigned long CAN_RXIMR15;
} COLDFIRE_CAN_CONTROL;
#endif

// FLASH Configuration module
//
#define CFMMCR               *(unsigned short *)(CFM_BASE_ADD + 0x0)     // CFM Module Configuration Register
  #define KEYACC             0x0020                                      // Enable security key writing
  #define CCIE               0x0040                                      // Command Complete Interrupt Enable
  #define CBEIE              0x0080                                      // Command Buffer Empty Interrupt Enable
  #define AEIE               0x0100                                      // Access Error Interrupt Enable
  #define PVIE               0x0200                                      // Protection Violation Interrupt Enable
  #define CFMLOCK            0x0400                                      // Lock writing (writable once)
#define CFMCLKD              *(unsigned char *)(CFM_BASE_ADD + 0x2)      // CFM Clock Divide Register
  #define DIV_MASK           0x3f                                        // 
  #define PRDIV8             0x40                                        // Enable Prescaler by 8
  #define DIVLD              0x80                                        // Clock Divider Loaded
#define CFMSEC               *(unsigned long *)(CFM_BASE_ADD + 0x8)      // CFM Security Register
  #define KEYEN              0x80000000                                  // Enable backdoor key access to FLASH module
  #define SECSTAT            0x40000000                                  // Flash security is enabled
  #define SECFLASH           0x00004ac8                                  // FLASH secured code (Coldfire HALT instruction)
#define CFMPROT              *(unsigned long *)(CFM_BASE_ADD + 0x10)     // CFM Protection register
#define CFMSACC              *(unsigned long *)(CFM_BASE_ADD + 0x14)     // CFM Supervisor Access Register
#define CFMDACC              *(unsigned long *)(CFM_BASE_ADD + 0x18)     // CFM Data Access Register
#define CFMUSTAT             *(volatile unsigned char *)(CFM_BASE_ADD + 0x20) // CFM User Status Register
  #define CBEIF              0x80                                        // Command buffer empty interrupt flag
  #define CCIF               0x40                                        // Command Complete Interrupt Flag
  #define PVIOL              0x20                                        // Protection Violation
  #define ACCERR             0x10                                        // Access Error
  #define BLANK              0x04                                        // Blank check verified
#define CFMCMD               *(volatile unsigned char *)(CFM_BASE_ADD + 0x24) // {35} CFM Command register
    #define CMD_BLANK_CHECK   0x05
    #define CMD_PAGE_ERASE_VERIFY 0x06
    #define CMD_PROGRAMM_WORD 0x20                                       // program a 32 bit word
    #define CMD_SECTOR_ERASE  0x40
    #define CMD_MASS_ERASE    0x41
#define CFMCLKSEL            *(unsigned short *)(CFM_BASE_ADD + 0x4a)    // 

#define CFM_FLASH            (IPSBAR + 0x4000000)                        // CFM FLASH for commands
#if defined _M5225X
    #define FLASH_GRANULARITY (4*1024)                                   // smallest sector which can be erased independently
#elif !defined _M523X && !defined _M520X                                 // these parts defend on external memory 
    #define FLASH_GRANULARITY (2*1024)                                   // smallest sector which can be erased independently
#endif
#define MAX_SECTOR_PARS      (FLASH_GRANULARITY - 2)                     // the number of user bytes fitting into first parameter block


// PHY registers symbolic names
// (located in MII memory map, accessible through MDIO)
#define PHY_REG_CR      0x00                                             // Control Register 
#define PHY_REG_SR      0x01                                             // Status Register
#define PHY_REG_ID1     0x02                                             // PHY Identification Register 1 
#define PHY_REG_ID2     0x03                                             // PHY Identification Register 2
#define PHY_REG_ANAR    0x04                                             // Auto-Negotiation Advertisement Register 
#define PHY_REG_ANLPAR  0x05                                             // Auto-Negotiation Link Partner Ability Register 
#define PHY_REG_ER      0x06                                             // Auto-Negotiation Expansion Register 
#define PHY_REG_NPTR    0x07                                             // Auto-Negotiation Next Page Transfer Register
#define PHY_REG_ICR     0x10                                             // Interrupt Control Register 
  #define PHY_ACKIE 0x4000                                               // acknowledge bit received interrupt enable
  #define PHY_PRIE  0x2000                                               // Page received interrupt enable
  #define PHY_LCIE  0x1000                                               // Link changed interrupt enable
  #define PHY_ANIE  0x0800                                               // Auto negotiate changed interrupt enable
  #define PHY_PDFIE 0x0400                                               // Parallel detect fault interrupt enable
  #define PHY_RFIE  0x0200                                               // Remote fault interrupt enable
  #define PHY_JABIE 0x0100                                               // Jabber interrupt enable

  #define PHY_ACKR  0x0040                                               // acknowledge bit received
  #define PHY_PGR   0x0020                                               // Page received 
  #define PHY_LKC   0x0010                                               // Link changed
  #define PHY_ANC   0x0008                                               // Auto negotiate changed
  #define PHY_PDF   0x0004                                               // Parallel detect fault 
  #define PHY_RMTF  0x0002                                               // Remote fault 
  #define PHY_JABI  0x0001                                               // Jabber interrupt 

#define PHY_REG_PSR     0x11                                             // Proprietary Status Register
#define PHY_REG_PCR     0x12                                             // Proprietary Control Register 
#define PHY_REG_10BTBC  0x13                                             // 10Base-T Bypass Control Register 
#define PHY_REG_100BXBC 0x14                                             // 100Base-X Bypass Control Register 
#define PHY_REG_ADDR    0x15                                             // Test & Trim Control Register 
#define PHY_REG_DSPRC   0x17                                             // DSP Reset Control 
#define PHY_REG_DSPRR1  0x18                                             // 100Base-X DSP Read Registers 
#define PHY_REG_DSPRR2  0x19
#define PHY_REG_DSPRR3  0x1A
#define PHY_REG_DSPWR1  0x1B                                             // 100Base-X DSP Write Registers 
#define PHY_REG_DSPWR2  0x1C                              
#define PHY_REG_DSPWR3  0x1D

// External PHY specifc registers (Micrel)                               // {30}
//
#define KS8721_BASIC_STATUS_REGISTER                  0x1
  #define PHY_SUPPORT_EXTENDED_CAPABILITIES           0x0001
  #define PHY_JABBER_DETECTED                         0x0002
  #define PHY_LINK_IS_UP                              0x0004
  #define PHY_AUTO_NEGOTIATION_CAPABLE                0x0008
  #define PHY_REMOTE_FAULT_DETECTED                   0x0010
  #define PHY_AUTO_NEGOTIATION_COMPLETE               0x0020
  #define PHY_NO_PREAMBLE                             0x0040
  #define PHY_10MEG_HALF_DUPLEX                       0x0800
  #define PHY_10MEG_FULL_DUPLEX                       0x1000
  #define PHY_100MEG_HALF_DUPLEX                      0x2000
  #define PHY_100MEG_FULL_DUPLEX                      0x4000
  #define PHY_100MEG_T4_CAPABLE                       0x8000

#define KS8721_RXER_COUNTER_REGISTER                  0x15
#define KS8721_INTERRUPT_CONTROL_STATUS_REGISTER      0x1b
  #define PHY_LINK_UP_INT_ENABLE                      0x0100
  #define PHY_REMOTE_FAULT_INT_ENABLE                 0x0200
  #define PHY_LINK_DOWN_INT_ENABLE                    0x0400
  #define PHY_LINK_PARTNER_ACK_INT_ENABLE             0x0800
  #define PHY_PARALLEL_DETECT_FAULT_INT_ENABLE        0x1000
  #define PHY_PAGE_RECEIVED_INT_ENABLE                0x2000
  #define PHY_RECEIVE_ERROR_INT_ENABLE                0x4000
  #define PHY_JABBER_INT_ENABLE                       0x8000
  #define PHY_LINK_UP_INT                             0x0001
  #define PHY_REMOTE_FAULT_INT                        0x0002
  #define PHY_LINK_DOWN_INT                           0x0004
  #define PHY_LINK_PARTNER_ACK_INT                    0x0008
  #define PHY_PARALLEL_DETECT_FAULT_INT               0x0010
  #define PHY_PAGE_RECEIVED_INT                       0x0020
  #define PHY_RECEIVE_ERROR_INT                       0x0040
  #define PHY_JABBER_INT                              0x0080
#define KS8721_100BASETX_PHY_CONTROL_REGISTER         0x1f


// 0 - Control Register
#define PHY_R0_RESET    0x8000                                           // Reset
#define PHY_R0_LB       0x4000                                           // Loop Back
#define PHY_R0_DR       0x2000                                           // Data Rate (100Mb/s)
#define PHY_R0_ANE      0x1000                                           // Auto-Negotiation Enable
#define PHY_R0_PD       0x0800                                           // Power Down
#define PHY_R0_ISOLATE  0x0400                                           // Isolate (MII is disconnected)
#define PHY_R0_RAN      0x0200                                           // Restart Auto-Negotiation
#define PHY_R0_DPLX     0x0100                                           // Duplex (Full duplex)
#define PHY_R0_CT       0x0080                                           // Collision Test (Enable)

// 4 - Auto Negotiation Advertisement Register
#define PHY_R4_NP       0x8000                                           // Next Page (capable of sending next pages)
#define PHY_R4_RF       0x2000                                           // Remote Fault
#define PHY_R4_FC       0x0400                                           // Flow Control
#define PHY_R4_100F     0x0100                                           // 100Base-TX Full Duplex Capable 
#define PHY_R4_100H     0x0080                                           // 100Base-TX Half Duplex Capable
#define PHY_R4_10F      0x0040                                           // 10Base-T Full Duplex Capable 
#define PHY_R4_10H      0x0020                                           // 10Base-T Half Duplex Capable 

////Proprietary Status Register
#define PHY_R17_LNK   0x4000    
#define PHY_R17_DPM   0x2000                                             // Duplex Mode
#define PHY_R17_SPD   0x1000                                             // Speed
#define PHY_R17_ANNC  0x0400                                             // Auto-Negotiation Complete
#define PHY_R17_PRCVD 0x0200
#define PHY_R17_ANCM  0x0100                                             // Auto-Negotiation (A-N) Common Operating Mode
#define PHY_R17_PLR   0x0020



// Watch dog activation defines
//
#define ACTIVE_WATCHDOG_2_SEC   (CWE | WATCHDOG_2_SEC | CWTA | CWTAVA | CWTIF); fnSetIntHandler(SW_WD_VECTOR, (unsigned char *)_sw_wdog_timeout); IC_ICR_0_8 = (INTERRUPT_LEVEL_7 | INTERRUPT_PRIORITY_7); IC_IMRL_0 &= ~(SW_WDG_PIF_INT_L | MASK_ALL_INT);
#define WATCHDOG_DISABLED       (0)



// Hardware dependent prototypes
//
#if !defined COMPILE_IAR
    extern void asm_int_off(void);
    extern void asm_int_on(void);
#endif
extern void mcf52235_init(void);
extern void mcf52235_boot_init(void);
extern void mcf5xxx_wr_vbr (unsigned long);

#if defined _CODE_WARRIOR_CF                                             // compiling with Code Warrior
    extern char __BSS_END[];                                             // the end of variables
#endif

#if defined _M5222X || defined _M5221X || defined _M521XX                // {28}
    #define PORTS_AVAILABLE  13                                          // use some dummy ports to keep compatible with m5223x
#else
    #if defined _M5225X                                                  // 
        #define PORTS_AVAILABLE 16                                       // 7 x 8 bit ports 2 x 7 bit port / 6 x 4 bit ports / 1 x 3 bit port
    #elif defined _M523X                                                 // {60}
        #define PORTS_AVAILABLE 14 
    #elif defined _M520X                                                 // {54}
        #define PORTS_AVAILABLE 10
    #elif defined _M52XX_SDRAM                                           // {53}
        #if defined _M521X_SDRAM
            #define PORTS_AVAILABLE 22                                   // port EK is not included
        #else
            #define PORTS_AVAILABLE 23
        #endif
    #elif defined CHIP_100_PIN || !defined CHIP_80_PIN
        #define PORTS_AVAILABLE 13                                       // 5 x 8 bit ports 1 x 7 bit port / 6 x 4 bit ports / 1 x 2 bit port
    #else                                                                // 80 pin
        #define PORTS_AVAILABLE 12                                       // 4 x 8 bit ports (no DD) 1 x 7 bit port / 6 x 4 bit ports / 1 x 2 bit port
    #endif
#endif
#define PORT_WIDTH           8

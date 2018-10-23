/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        AVR32.c [ATMEL AT32UC3A/B/C]
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

    15.09.2009 Add fnGetMultiDropByte()                                  {1}
    11.10.2009 Remove __interrupt from fnEnterAVRInterrupt parameter and cast all use for compatibility with IAR 3.20A compiler {2}
    18.11.2009 Add _AT32UC3B support                                     {3}
    20.11.2009 Enable Rx after configuring the Rx pin to avoid internal glitch and spurious input character after configuration {4}
    06.12.2009 Correction to avoid unnecessary FLASH buffer write when no buffer is open {5}
    05.06.2010 Add USB device                                            {6}
    10.10.2010 Add power of twos mode for AT45DBXXXD                     {7}
    10.05.2010 Add MANAGED_FILES support                                 {8}
    19.10.2010 Add hardware timer mode with period support               {9}
    08.06.2011 Power up the ADC before use                               {10}
    08.06.2011 Extend MANAGED_FILES to SST SPI Flash                     {11}
    10.06.2011 Add multi-drop mode support (9-bit mode)                  {12}
    13.06.2011 Modify location of random number to top of RAM            {13}
    19.06.2011 Add _AT32UC3C support                                     {14}
    01.09.2011 Add ETHERNET_RELEASE_AFTER_EVERY_FRAME support            {15}
    17.09.2011 Add external interrupt support to _AT32UC3C               {16}
    16.11.2011 Save memory in interrupt entry routine with _OPTIMISE_UC3C_INT {17}
    08.01.2012 SPI Flash page calculation adapted for all power-of-2 devices {18}
    27.05.2012 Adapt network parameters for compatibility with multiple networks {19}
    27.05.2012 Add return value to fnConfigEthernet()                    {20}
    27.05.2012 Add multicast Ethernet configuration and simulation acceptance {21}
    20.06.2012 Correct Ethernet MII mode selection                       {22}
    19.07.2012 Add RTC support                                           {23}
    05.08.2012 Add MONITOR_PERFORMANCE support                           {24}
    07.12.2012 Disable PHY interrupt when PHY_INTERRUPT is not defined   {25}
    07.12.2012 Add PHY reset control                                     {26}
    07.12.2012 Add fnDelayLoop()                                         {27}
    11.12.2012 Add _PHY_RTL8201E support to force RMII mode via register write {28}
    16.12.2012 Add PHY_POLL_LINK support                                 {29}
    17.12.2012 Add interface number to Ethernet when working with multiple IP interfaces {30}
    18.12.2012 Modify watchdog clear for UC3C                            {31}

*/

#if defined _HW_AVR32

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS
    #include "config.h"
    #include "../../WinSim\WinSim.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    extern void AVR32_LowLevelInit(void);
    #define _SIM_PORT_CHANGE   fnSimPorts();                             // make sure simulator knows of change
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define _SIM_PORT_CHANGE
    extern void  __set_evba(unsigned long ulEVBA);
    extern void  __set_count(unsigned long ulCountValue);
    extern void  __set_compare(unsigned long ulCompareValue);
    #include "config.h"
#endif


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */
#if defined _GNU
    #define __interrupt    __attribute__((__interrupt__))
    #define __exception    __attribute__((interrupt("full")))
#elif !defined COMPILE_IAR
    #define __interrupt
    #define __exception
#endif
#define _OPTIMISE_UC3C_INT                                               // {17} this saves the extended group location constants in exchange for a simple calculation of extended group values

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    static void fnConfigSPIFileSystem(unsigned char ucSpeed);
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_avr32_atmel.h"
            #include "spi_flash_avr32_stmicro.h"
            #include "spi_flash_avr32_sst25.h"
        #undef _SPI_DEFINES
    #endif
#endif

#if defined ETH_INTERFACE && defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS // {25}
    static void fnPhyInterrupt(void);
#endif

#if defined FLASH_FILE_SYSTEM
    static void fnCommandFlash(unsigned long ulCommand);
    static int fnCommitFLASH(unsigned long ulPage);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


#if defined ETH_INTERFACE && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME // {15}
    static const unsigned char MACB_RX_int_message[ HEADER_LENGTH ] = { 0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT };   // define fixed interrupt event
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#ifdef RANDOM_NUMBER_GENERATOR
    static unsigned short *ptrSeed;
#endif
#ifdef FLASH_FILE_SYSTEM
    static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)]; // FLASH row backup buffer (on word boundary)
#endif
#ifdef ETH_INTERFACE
    static AVR32_RX_BD *ptrRxBd, *ptrFirstRxBd, *ptrTxBd, *ptrFirstTxBd; // we set two pointers to the rx Ethernet buffers. The first is fixed but the second depends on buffer size
    static unsigned long ulEthernetInput[LAN_BUFFER_SIZE/sizeof(unsigned long)]; // a buffer adequate to hold a full Ethernet frame while being processed as linear buffer - aligned on long word boundary
#endif
#ifdef _WINDOWS
    static unsigned short usPhyMode = 0;
#endif
#ifdef USB_INTERFACE
    static USB_END_POINT *usb_endpoints;
#endif

#ifdef SUPPORT_PORT_INTERRUPTS
    static void (*gpio_handlers[PORTS_AVAILABLE][PORT_WIDTH])(void) = {{0}};// handler for each possible port line
#endif

#ifdef SUPPORT_EXTERNAL_INTERRUPTS
    #ifdef _AT32UC3B
        static void (*extint_handlers[8])(void) = {0};                   // handler for each possible external interrupt
    #else
        static void (*extint_handlers[9])(void) = {0};                   // handler for each possible external interrupt
    #endif
#endif

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];
    #else
    static int SPI_FLASH_Danger[1] = {0};
    static unsigned char ucSPI_FLASH_Type[1];
    #endif

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #ifdef CS2_LINE
            ,CS2_LINE
            #ifdef CS3_LINE
            ,CS3_LINE
            #endif
        #endif
        };
        #define EXTENDED_CS , &iChipSelect
        #define _EXTENDED_CS  iChipSelect,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif
#ifndef CS0_LINE                                                         // for compatibility when signals are not defined by user
    #define CS0_LINE    CS_OUTPUT
#endif
#if defined SERIAL_INTERFACE && defined UART_EXTENDED_MODE               // {12}
    #ifdef SERIAL_MULTIDROP_TX
        static unsigned char ulNextSent[NUMBER_SERIAL] = {0};            // next extended data to be sent
    #endif
    #ifdef SERIAL_MULTIDROP_RX
        static unsigned long ulLastReceived[NUMBER_SERIAL];              // backup of last read reception data
    #endif
#endif
static int iInterruptLevel = 0;

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */
#if defined ETH_INTERFACE
    extern ETHERNETQue *eth_tx_control;                                  // Ethernet structure pointers for Ethernet Interrupt handling
    extern ETHERNETQue *eth_rx_control;
#endif

#if defined I2C_INTERFACE
    extern I2CQue *I2C_rx_control[NUMBER_I2C];
    extern I2CQue *I2C_tx_control[NUMBER_I2C];
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#if !defined _WINDOWS
    static void fnInitHW(void);
#endif
#if defined GLOBAL_HARDWARE_TIMER
    static __interrupt void _hwtimer_interrupt(void);
#endif


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */



/* =================================================================== */
/*                      STARTUP CODE                                   */
/* =================================================================== */


#if !defined _WINDOWS
extern void __segment_init(void);

#if defined  COMPILE_IAR
    #pragma segment="DATA32_Z"
    #define HEAP_START_ADDRESS __segment_end("DATA32_Z")                 // IAR compiler - last location of static variables
#elif defined COMPILE_KEIL
    #define __root
    extern unsigned char Stack_Mem;
    #define HEAP_START_ADDRESS  &Stack_Mem                               // Keil - start of stack
#else
    #if defined _GNU || defined ROWLEY
        #define __root
        #define __irq_dis() asm("ssrf 16")                               // disable interrupt in assembler code
        #define __irq_en()  asm("csrf 16")                               // enable interrupt in assembler code
    #endif
    extern unsigned char __heap_end__;
    #define HEAP_START_ADDRESS &__heap_end__                             // GNU last location of static variables
#endif

#define RANDOM_SEED_LOCATION (unsigned short *)(SRAM_RAM_ADDRESS + SIZE_OF_RAM - 4) // last 4 locations reserved for random number use

// Main entry for the target code
//
__root int main(void)                                                    // __root forces the function to be linked in IAR project
{
#ifdef MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart = HEAP_START_ADDRESS;
#endif
#ifdef RANDOM_NUMBER_GENERATOR                                           // {13} top location in SRAM used as seed value - uninitialised for randomness
    ptrSeed = RANDOM_SEED_LOCATION;
#endif
    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)
#ifdef MULTISTART
    prtInfo = ptMultiStartTable;                                         // if the user has already set to alternative start configuration
    if (prtInfo == 0) {                                                  // no special start required
_abort_multi:
        fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                 // initialise heap
        uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system
        while ((prtInfo = (MULTISTART_TABLE*)uTaskerSchedule()) == 0) {} // schedule uTasker
    }

    do {
        pucHeapStart = HEAP_START_ADDRESS;
        if (prtInfo->new_hw_init) {                                      // info to next task configuration available
            pucHeapStart = prtInfo->new_hw_init(JumpTable);              // get heap details from next configuration
            if (!pucHeapStart) {
                goto _abort_multi;                                       // this can happen if the jump table version doesn't match - prefer to stay in boot mode than start an application which will crash
            }
        }

        fnInitialiseHeap(prtInfo->ptHeapNeed, pucHeapStart);             // initialise the new heap
                                                                         // start the operating system with next configuration
        uTaskerStart((UTASKTABLEINIT *)prtInfo->ptTaskTable, prtInfo->ptNodesTable, PHYSICAL_QUEUES);

        while ((prtInfo = (MULTISTART_TABLE*)uTaskerSchedule()) == 0) {} // schedule uTasker

    } while (1);
#else
    fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                     // initialise heap
    uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system

    while (1) {
        uTaskerSchedule();                                               // schedule uTasker
    }
#endif
    return 0;                                                            // we never return....
}
#endif                                                                   // end if not _WINDOWS

#define EXCEPTION_VECTOR_BASE_ADDRESS   START_OF_SRAM                    // warning - all interrupt code must be within a 16k block from this address!!! The exception table in SRAM removes this constraint in this implementation

static const unsigned char ucGroupLocation[] = {18, 21, 25, 
#if !defined _OPTIMISE_UC3C_INT
    29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61,
    #if defined _AT32UC3C
    65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 11
    #endif
#endif
}; // location of peripheral group handler jump in the exception table

// Function used to enter interrupts
//
static void fnEnterAVRInterrupt(int iIntGroup, unsigned char ucIntLevel, void (*InterruptFunc)(void)) // {2}
{
#if defined _WINDOWS
    unsigned long *ptrEventTable = (unsigned long *)AVR32_ExceptionRAM;
#else
    unsigned long *ptrEventTable = (unsigned long *)EXCEPTION_VECTOR_BASE_ADDRESS;
#endif
    unsigned long *ptrIntPriority = (unsigned long *)INTC_BLOCK;
#if defined _OPTIMISE_UC3C_INT                                           // {17}
    unsigned long ulGroupLocation;
#endif
    ptrIntPriority += iIntGroup;
#if defined _OPTIMISE_UC3C_INT
    if (iIntGroup >= 3) {
        ulGroupLocation = (29 + ((iIntGroup - 3) * 2));
        #if defined _AT32UC3C
        if (ulGroupLocation > 61) {
            ulGroupLocation += 2;
        }
        #endif
    }
    else {
        ulGroupLocation = ucGroupLocation[iIntGroup];
    }
    ptrEventTable += ulGroupLocation;
#else
    ptrEventTable += ucGroupLocation[iIntGroup];
#endif
    *ptrEventTable++ = LOAD_PC_WITH_NEXT_VALUE;
    *ptrEventTable = (unsigned long)InterruptFunc;
#if defined _OPTIMISE_UC3C_INT
    *ptrIntPriority = (((unsigned long)ucIntLevel << 24) | ((ulGroupLocation) << 2));
#else
    *ptrIntPriority = (((unsigned long)ucIntLevel << 24) | (((unsigned long)ucGroupLocation[iIntGroup]) << 2));
#endif
}


#ifdef RANDOM_NUMBER_GENERATOR
// How the random number seed is set depends on the hardware possibilities available.
//
extern void fnInitialiseRND(unsigned short *usSeedValue)
{
    if ((*usSeedValue = *ptrSeed) == 0) {                                // we hope that the content of the random seed is random after a power up
        *usSeedValue = 0x127b;                                           // we must not set a zero - so we set a fixed value
    }                                                                    // after a reset it should be well random since it will contain the value at the reset time
    *ptrSeed = fnRandom();                                               // ensure that the seed value is modified between resets
}
#endif

#if defined FLASH_FILE_SYSTEM && defined FLASH_ERRATA_WORKAROUND
// Check that special FLASH pages are zeroed
//
static int fnCheckZero(unsigned long *ptrFlashPage)
{
    int i = (FLASH_GRANULARITY/sizeof(unsigned long));
    while (i--) {
        if (*ptrFlashPage++ != 0x00000000) {
            return 1;
        }
    }
    return 0;
}
#endif

// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#if defined GLOBAL_HARDWARE_TIMER
    volatile unsigned long dummy;
#endif
    int i = 0;
#if defined _WINDOWS
    unsigned long *ulPtrEntries = AVR32_ExceptionRAM;
    unsigned long ulPortPullups[] = {
        PORT0_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_avr32.h
        PORT1_DEFAULT_INPUT,
    #if !defined _AT32UC3B                                                    // {3}
        PORT2_DEFAULT_INPUT,
        PORT3_DEFAULT_INPUT,
    #endif
    #if defined SUPPORT_ADC
        ((AN0_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),                   // initial voltages when simulating
        ((AN1_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN2_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN3_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN4_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN5_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN6_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN7_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
    #endif
    };
    #ifdef RANDOM_NUMBER_GENERATOR
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;
    #endif
    fnInitialiseDevice((void *)ulPortPullups);
    AVR32_LowLevelInit();
#else
    unsigned long *ulPtrEntries = (unsigned long *)EXCEPTION_VECTOR_BASE_ADDRESS; // create an exception table at the start of RAM
#endif
    while (i++ < (sizeof(EXCEPTION_TABLE)/sizeof(unsigned long))) {
        *ulPtrEntries++ = BRANCH_TO_SELF;                                // fill the event table with forever loops to catch unexpected exceptions
    }
#ifdef _WINDOWS
    ulAVR32_CPU.EVBA = 0;
#else
    __set_evba(EXCEPTION_VECTOR_BASE_ADDRESS);                           // set EVBA to the start of RAM
#endif
#ifdef FLASH_FILE_SYSTEM
    #ifdef FLASH_ERRATA_WORKAROUND                                       // it is assumed that code is always executed from the first half of FLASH and so only operations in the second half need to be worked around
    if (fnCheckZero((unsigned long *)fnGetFlashAdd((unsigned char *)FLASH_LOW_BLOCK_ZERO))) {
        fnCommandFlash(FLASH_KEY | FCMD_CPB);                            // clear the Flash page buffer
        uMemset_long((unsigned long *)fnGetFlashAdd((unsigned char *)FLASH_LOW_BLOCK_ZERO),  0, FLASH_GRANULARITY);
        fnCommitFLASH((FLASH_LOW_BLOCK_ZERO - FLASH_START_ADDRESS)/FLASH_GRANULARITY); // delete a block in lower FLASH range
    }
    if (fnCheckZero((unsigned long *)fnGetFlashAdd((unsigned char *)FLASH_HIGH_BLOCK_ZERO))) {
        fnCommandFlash(FLASH_KEY | FCMD_CPB);                            // clear the Flash page buffer
        uMemset_long((unsigned long *)fnGetFlashAdd((unsigned char *)FLASH_HIGH_BLOCK_ZERO), 0, FLASH_GRANULARITY);
        fnCommitFLASH((FLASH_HIGH_BLOCK_ZERO - FLASH_START_ADDRESS)/FLASH_GRANULARITY); // delete a block in higher FLASH range
    }
    #endif
    uMemset_long(ulFlashRow, 0xffffffff, FLASH_ROW_SIZE);                // flush the intermediate FLASH buffer
#endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
    INIT_WATCHDOG_DISABLE();                                             // initialise ready for checking of watchdog diabled
    if (!WATCHDOG_DISABLE()) {
        WDT_CTRL = (WATCHDOG_KEY_1 | WATCHDOG_ENABLE | WATCHDOG_TIMEOUT);// enable watchdog with two writes using the key sequence
        WDT_CTRL = (WATCHDOG_KEY_2 | WATCHDOG_ENABLE | WATCHDOG_TIMEOUT);
    }
    else {
        WDT_CTRL = (WATCHDOG_KEY_1);                                     // enable watchdog with two writes using the key sequence
        WDT_CTRL = (WATCHDOG_KEY_2);                                     // disable watchdog
    }
#ifdef _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif
    fnUserHWInit();                                                      // allow the user to initialise hardware specific things
#ifdef _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif
#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    fnConfigSPIFileSystem(MAX_SPI);                                      // configure SPI interface for maximum speed

    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
        #include "spi_flash_avr32_atmel.h"                               // check the chip(s) availablility
        #include "spi_flash_avr32_stmicro.h"
        #include "spi_flash_avr32_sst25.h"
    #undef _CHECK_SPI_CHIPS
    #else
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
    #endif
#endif

#ifdef GLOBAL_HARDWARE_TIMER                                             // initialise the timer for use with global hardware timers
    POWER_UP(HW_TIMER_TC);                                               // enable clocks to timer in PMC
    HW_TIMER_CCR = (TIM_CLKDIS | TIM_SWTRG);                             // disable timer clock and reset timer

    dummy = HW_TIMER_SR;                                                 // read status register to reset possible pending interrupt

    fnEnterInterrupt(HW_TIMER_TC, PRIORITY_HW_TIMER, hwtimer_interrupt);

    #if MILLISEC == LOW_RES_MS
    HW_TIMER_CMR  = (TIMER_CLOCK_5 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// MCK/1024, wave mode, trigger on register RC, stop after triggering
    #elif MILLISEC == MED_RES_MS
    HW_TIMER_CMR  = (TIMER_CLOCK_4 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// MCK/128, wave mode, trigger on register RC, stop after triggering
    #else
    HW_TIMER_CMR  = (TIMER_CLOCK_3 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// MCK/32, wave mode, trigger on register RC, stop after triggering
    #endif
#endif
}


// Support watchdog re-triggering of specific hardware
//
extern void fnRetriggerWatchdog(void)
{
#if defined _AT32UC3C                                                    // {31}
    WDT_CLR = (WATCHDOG_KEY_1 | WDT_CLR_WDTCLR); WDT_CLR = (WATCHDOG_KEY_2 | WDT_CLR_WDTCLR);
#else
    WDT_CLR = 0x0;                                                       // retrigger the watchdog by writing any value to it
#endif
    TOGGLE_WATCHDOG_LED();                                               // flash an LED for visualisation
}

// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)
{
#ifndef _WINDOWS
    #if defined (COMPILE_IAR)
    __disable_interrupt();                                               // IAR intrinsic routine
    #elif defined COMPILE_KEIL
    __disable_irq();                                                     // Keil intrinsic routine
    #elif defined (_GNU)
    __irq_dis();
    #endif
#endif
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
extern void uEnable_Interrupt(void)
{
#ifdef _WINDOWS
    if (!iInterruptLevel) {
        *(int *)0 = 0;                                                   // basic error - cause simulator exception
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
    }
#endif
    if (!(--iInterruptLevel)) {                                          // only when no more interrupt nesting,
    #if defined (COMPILE_IAR)
        __enable_interrupt();                                            // IAR intrinsic routine
    #elif defined COMPILE_KEIL
        __enable_irq();                                                  // Keil intrinsic routine
    #elif defined (_GNU)
        __irq_en();
    #endif
    }
}

// User handling own interrupts can signal it here to avoid driver routines opening interrupts too early
//
extern void fnInUserInterrupt(int iEntering)
{
    if (iEntering) {                                                     // entering user controlled interrupt routine
        iInterruptLevel = 1;                                             // mark that interrupts are disabled
    }
    else {
        iInterruptLevel = 0;                                             // mark interrupt exiting
    }
}


#define TICK_COMPARE_VALUE ((unsigned long)(((float)MASTER_CLOCK * (float)TICK_RESOLUTION/1000)/(float)1000))

/**************************** Real Time Clock interrupt ******************************************/
static __interrupt void _RealTimeInterrupt(void)
{
    fnRtmkSystemTick();                                                  // operating system tick
#ifdef _WINDOWS
    ulAVR32_CPU.COMPARE = TICK_COMPARE_VALUE;                            // reset interrupt be writing the compare value again
    IRR0 = 0;
#else
    __set_compare(TICK_COMPARE_VALUE);
#endif
}

// Routine to initialise the Real Time Tick interrupt
//
extern void fnStartTick(void)
{
    fnEnterAVRInterrupt(IR_GROUP_SYSBLOCK, PRIORITY_TICK_TIMER, (void (*)(void))_RealTimeInterrupt);
#if defined _WINDOWS
    ulAVR32_CPU.COUNT = 0;
    ulAVR32_CPU.COMPARE = TICK_COMPARE_VALUE;                            // cycle counter to achieve TICK interval
#else
    __set_count(0);
    __set_compare(TICK_COMPARE_VALUE);
#endif
#if defined MONITOR_PERFORMANCE                                          // {24} configure a timer that will be used to measure the duration of task operation
    INITIALISE_MONITOR_TIMER();
#endif
#if defined _GNU
    __irq_en();                                                          // enable interrupts from this point
#elif defined COMPILE_IAR
    __enable_interrupt();
#endif
#if defined _WINDOWS
    ulAVR32_CPU.SR &= ~GM;                                               // unmask global interrupt
#endif
}


#ifdef ETH_INTERFACE
/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */

// A long word version of memcpy since the LAN buffer copy is always on long word boundaries
//
static void uMemLongcpy(unsigned long *ptrLongDest, unsigned long *ptrLongSrc, size_t Size)
{
    while (Size--) {
        *ptrLongDest++ = *ptrLongSrc++;
    }
}

// Extract a received frame from possibly multiple buffers. Free the buffers and create one linear buffer for further processing.
//
static unsigned short fnExtractNextRx(void)
{
    unsigned long ulTemp;
    unsigned char *ptrInputBuffer;                                       // pointer to waiting message (segment)
    unsigned short usLength = 0;
    unsigned short usFrameLength = 0;
    int iInFrame = 0;

    while (1) {
        ulTemp = (unsigned long)ptrRxBd->ptrBlock;
        if (!(ulTemp & BD_OWNERSHIP)) {
            return 0;                                                    // no more frames waiting
        }
        ulTemp &= ~(BD_OWNERSHIP | BD_RX_WRAP);
        if (BD_INFO_START_OF_FRAME & ptrRxBd->bd_details) {              // we expect the start of frame bit to be set in the first frame.
            if (iInFrame) {                                              // if we encounter it during a frame, it represents a frame fragment (some sort of error) and we must ignore it
                if ((unsigned long)ptrRxBd->ptrBlock & BD_RX_WRAP) {
                    ptrRxBd->ptrBlock = (unsigned char*)(ulTemp | BD_RX_WRAP);// free buffer for further reception
                    ptrRxBd = ptrFirstRxBd;
                }
                else {
                    ptrRxBd->ptrBlock = (unsigned char*)(ulTemp);
                    ptrRxBd++;
                }
                usLength = 0;                                            // reset the input buffer
                usFrameLength = 0;
                iInFrame = 0;
                continue;
            }
            iInFrame = 1;                                                // start frame found
        }
        else if (!iInFrame) {                                            // we are still searching for a start frame
            if ((unsigned long)ptrRxBd->ptrBlock & BD_RX_WRAP) {
                ptrRxBd->ptrBlock = (unsigned char*)(ulTemp | BD_RX_WRAP); // free buffer for further reception
                ptrRxBd = ptrFirstRxBd;
            }
            else {
                ptrRxBd->ptrBlock = (unsigned char*)(ulTemp);
                ptrRxBd++;
            }
            continue;
        }

        ptrInputBuffer = (unsigned char *)ulTemp;                        // pointer to data in memory
        if (ptrRxBd->bd_details & BD_INFO_END_OF_FRAME) {                // last buffer in the frame
            usFrameLength = (unsigned short)(ptrRxBd->bd_details & BD_INFO_LENGTH_OF_FRAME_MASK);
            if (usFrameLength > LAN_BUFFER_SIZE) {                       // check that the complete frame is not too long for our buffer
                usFrameLength = 0;                                       // reject frame
                break;
            }
            uMemLongcpy(&ulEthernetInput[usLength/sizeof(unsigned long)], (unsigned long *)ptrInputBuffer, ((usFrameLength - usLength) + 3)/sizeof(unsigned long)); // copy the last bytes from last segment
            break;
        }
        else {                                                           // an individual block in frame - not last one
            if ((usLength + AVR32_FIXED_BUFFER_SIZE) <= LAN_BUFFER_SIZE) { // only copy if there is adequate space in the buffer
                uMemLongcpy(&ulEthernetInput[usLength/sizeof(unsigned long)], (unsigned long *)ptrInputBuffer, (AVR32_FIXED_BUFFER_SIZE/sizeof(unsigned long)));  // copy block
            }                                                            // if no copy is performed, the frame will be rejected when handling the last buffer
            usLength += AVR32_FIXED_BUFFER_SIZE;
        }

        if ((unsigned long)ptrRxBd->ptrBlock & BD_RX_WRAP) {
            ptrRxBd->ptrBlock = (unsigned char*)(ulTemp | BD_RX_WRAP);   // free buffer for further reception
            ptrRxBd = ptrFirstRxBd;
        }
        else {
            ptrRxBd->ptrBlock = (unsigned char*)(ulTemp);
            ptrRxBd++;
        }
    }

    if ((unsigned long)ptrRxBd->ptrBlock & BD_RX_WRAP) {
        ptrRxBd->ptrBlock = (unsigned char*)(ulTemp | BD_RX_WRAP);       // free buffer for further reception
        ptrRxBd = ptrFirstRxBd;
    }
    else {
        ptrRxBd->ptrBlock = (unsigned char*)(ulTemp);                    // free buffer for further reception
        ptrRxBd++;
    }
    return usFrameLength;
}

// This is called when an Ethernet event has been received. Generally it is due to a reception and returns channel and message details.
// We are more or less forced to copy a number of input buffers to a linear input buffer so that it can be handled by standard routines
// Therefore we step through one or a number of receive buffer descriptors, receiving the amount of data in them.
//
extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
    if (EMAC_RX_INTERRUPT == *ucEvent) {
        if ((rx_frame->frame_size = fnExtractNextRx()) == 0) {           // get frame details from present buffer descriptor
            return -1;                                                   // nothing (else) waiting
        }
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ulEthernetInput;     // return pointer to the fixed linear input buffer
#if IP_INTERFACE_COUNT > 1
        rx_frame->ucInterface = ETHERNET_INTERFACE;                      // {30}
#endif
        return 0;                                                        // channel received from 0 (we use only 0 since we have copied the received from to a local buffer)
    }
#if defined USE_IP_STATS && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME  // {15}
    else if (ETHERNET_RX_OVERRUN == *ucEvent) {
        fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES);                  // we lost a frame due to RX overrun so count the event
    }
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
#endif
    return -1;                                                           // invalid channel
}

#ifdef COMPILE_IAR                                                       // ensure IAR doesn't inline this because it otherwise makes a mistake and doesn't read the return value!
    #pragma optimize=no_inline
#endif
// Routine to read a word from the PHY
//
static unsigned short fnReadMII(unsigned long ulRegAdr)
{
    MACB_MAN = (READ_FROM_PHY | ulRegAdr);                               // command read from given address

#ifdef _WINDOWS
    MACB_NSR = (MDIO | PHY_LOGIC_RUNNING);
    MACB_MAN &= ~0x0000ffff;
    if (DP83848_PHY_IDENTIFIER_1 == ulRegAdr) {                          // simulate reads from identifier register
        MACB_MAN |= (PHY_IDENTIFIER >> 16);
    }
    else if (DP83848_PHY_IDENTIFIER_2 == ulRegAdr) {
        MACB_MAN |= (PHY_IDENTIFIER & 0x0000ffff);
    }
    else if (DP83848_MISR == ulRegAdr) {
    #if defined ETH_INTERFACE && defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS // {25}
        unsigned char ucBit = 31;
        unsigned long ulBit = PHY_INTERRUPT;
        while (ulBit > 1) {
            ulBit >>= 1;
            ucBit--;
        }
        fnSimulateInputChange(PORT_PHY_INT, ucBit, SET_INPUT);
    #endif
        MACB_MAN |= DP83848_LINK_INT;
    }
    #if defined _PHY_RTL8201E                                            // {28}
    else if (DP83848_BASIC_MODE_STATUS_REGISTER == ulRegAdr) {
        MACB_MAN |= PHY_LINK_IS_UP;
    }
    else if (DP83848_BASIC_MODE_CONTROL_REGISTER == ulRegAdr) {
        MACB_MAN |= (PHY_SELECT_100M_SPEED | PHY_FULL_DUPLEX_MODE);
    }
    #endif
    else if (DP83848_PHY_STATUS == ulRegAdr) {
        MACB_MAN |= (DP83848_LINK_STATUS_UP | DP83848_100BASET_DESCRAMBLE_LOCK | DP83848_100BASET_SIGNAL_DETECT);
    }
#endif
    while (!(MACB_NSR & PHY_LOGIC_RUNNING)) {}                           // wait until the transfer has completed

    return (unsigned short)MACB_MAN;                                     // return the value read
}

// Routine to write a word to the PHY
//
static void fnWriteMII(unsigned long ulRegAdr, unsigned short usData)
{
    MACB_MAN = (WRITE_TO_PHY | ulRegAdr | usData);                       // command read from given address

#ifdef _WINDOWS
    MACB_NSR = (MDIO | PHY_LOGIC_RUNNING);
    if (DP83848_BASIC_MODE_CONTROL_REGISTER == ulRegAdr) {
        usPhyMode = usData;
    }
#endif
    while (!(MACB_NSR & PHY_LOGIC_RUNNING)) {}                           // wait until the transfer has completed
}


// MACB interrupt dispatcher
//
static __interrupt void _MACB_Interrupt(void)
{
    volatile unsigned long ulInterrupts;
    while ((ulInterrupts = MACB_ISR) != 0) {                             // read the interrupt status register, which clears all interrupts
#if defined LAN_REPORT_ACTIVITY && defined INTERRUPT_TASK_LAN_EXCEPTIONS
        unsigned char MACB_int_message[HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT};
#endif
        if (ulInterrupts & TCOMP) {                                      // frame transmitted
            eth_tx_control->ETH_queue.chars = 0;                         // reset the transmission buffer for next use
            eth_tx_control->ETH_queue.put = eth_tx_control->ETH_queue.QUEbuffer;
            if (ptrTxBd->bd_details & BD_TX_WRAP) {
                ptrTxBd = ptrFirstTxBd;
            }
            else {
                ptrTxBd++;
            }
#if defined LAN_REPORT_ACTIVITY && defined INTERRUPT_TASK_LAN_EXCEPTIONS
            if (MACB_TSR & TSR_COL) {                                    // collision detection
                MACB_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_COLLISION;
                MACB_TSR = TSR_COL;                                      // remove collision indicator
            }
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, (unsigned char*)MACB_int_message, HEADER_LENGTH); // inform the task of event
            iInterruptLevel = 0;                                         // release
#endif
        }

        if (ulInterrupts & RCOMP) {                                      // complete reception frame available
            ((ETHERNETQue*)(eth_rx_control))->ETH_queue.chars = (QUEUE_TRANSFER)(ptrRxBd->bd_details & BD_INFO_LENGTH_OF_FRAME_MASK); // put the length of the received frame in the buffer
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
#ifdef ETHERNET_RELEASE_AFTER_EVERY_FRAME                                // {15}
            uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);         // schedule the Ethernet task
#else
            fnWrite(INTERNAL_ROUTE, (unsigned char*)MACB_RX_int_message, HEADER_LENGTH); // inform the Ethernet task
#endif
#if defined LAN_REPORT_ACTIVITY && defined INTERRUPT_TASK_LAN_EXCEPTIONS
            MACB_int_message[MSG_INTERRUPT_EVENT] = EMAC_RX_INTERRUPT;
            fnWrite(INTERNAL_ROUTE, (unsigned char*)MACB_int_message, HEADER_LENGTH); // inform the task of event
#endif
            iInterruptLevel = 0;                                         // release
        }

        ulInterrupts &= ~(TCOMP | RCOMP);

#ifdef _WINDOWS
        MACB_ISR = 0;                                                    // clear interrupt flag when simulating
        IRR_MACB = 0;
#endif
    }
}

#ifdef USE_IPV6                                                          // {21}
extern void fnConfigureMulticastIPV6(void)
{
    unsigned char ucHashIndex;                                           // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
  //ucHashIndex = (0x33 ^ (0x33 << 2) ^ (0x33 >> 4) ^ (unsigned char)(0xff00 >> 4) ^ (unsigned char)(0xff00 >> 10) ^ network[DEFAULT_NETWORK].ucOurIPV6[13] ^ (network[DEFAULT_NETWORK].ucOurIPV6[13] >> 6) ^ (network[DEFAULT_NETWORK].ucOurIPV6[14] << 2) ^ (network[DEFAULT_NETWORK].ucOurIPV6[14] >> 4) ^ (network[DEFAULT_NETWORK].ucOurIPV6[15] << 4) ^ (network[DEFAULT_NETWORK].ucOurIPV6[15] >> 2));
    ucHashIndex = (0x33 ^ (0x33 << 2) ^ (0x33 >> 4) ^ (unsigned char)(0xff00 >> 4) ^ (unsigned char)(0xff00 >> 10) ^ ucLinkLocalIPv6Address[13] ^ (ucLinkLocalIPv6Address[13] >> 6) ^ (ucLinkLocalIPv6Address[14] << 2) ^ (ucLinkLocalIPv6Address[14] >> 4) ^ (ucLinkLocalIPv6Address[15] << 4) ^ (ucLinkLocalIPv6Address[15] >> 2));
    ucHashIndex &= 0x3f;
    if (ucHashIndex < 32) {
        MACB_HRB = (0x00000001 << ucHashIndex);
    }
    else {
        ucHashIndex -= 32;
        MACB_HRT = (0x00000001 << ucHashIndex);
    }
    MACB_NCFGR |= MTI;                                                   // enable multicast hash operation
}
#endif

unsigned long ulPhyIdentifier = 0;                                       // using a global variable ensures that the compilers don't optimise away accesses based on it


// Ethernet configuration routine
//
extern int fnConfigEthernet(ETHTABLE *pars)                              // {20}
{
#if defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS             // {25}
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
#endif
    unsigned short usMIIData;
    volatile int i;
    AVR32_RX_BD *ptrBd;
    unsigned char *ptrBuffer;
    #define ulTemp ulPhyIdentifier                                       // reuse ulPhyIdentifier for intermediate storage

    POWER_UP(HSB, HSBMASK_MACB);                                         // enable high speed clock to MACB module
    POWER_UP(PBB, PBBMASK_MACB);                                         // enable peripheral clock to MACB module
#ifdef PHY_RMII
    #ifdef _AT32UC3C
    _CONFIG_PERIPHERAL_E(3, (MACB_TX_CLK_3_E | MACB_TX_EN_3_E | MACB_TXDATA0_3_E | MACB_TXDATA1_3_E | MACB_RXDATA0_3_E | MACB_RXDATA1_3_E | MACB_RX_ER_3_E | MACB_RX_DV_3_E));
    _CONFIG_PERIPHERAL_E(2, (MACB_MDC_2_E | MACB_MDIO_2_E));
    #else
    _CONFIG_PERIPHERAL_A(1, (MACB_TX_CLK_A | MACB_TX_EN_A | MACB_TXDATA0_A | MACB_TXDATA1_A | MACB_RXDATA0_A | MACB_RXDATA1_A | MACB_RX_ER_A | MACB_RX_MDC_A | MACB_RX_MDIO_A | MACB_RX_DV_A));
    #endif
#else
    #ifdef _AT32UC3C
    _CONFIG_PERIPHERAL_E(0, (MACB_SPEED_0_E));
    _CONFIG_PERIPHERAL_E(1, (MACB_RX_CLK_1_E | MACB_TX_EN_1_E | MACB_TXDATA0_1_E | MACB_TXDATA1_1_E | MACB_TXDATA2_1_E | MACB_TXDATA3_1_E | MACB_CRS_1_E | MACB_RXDATA0_1_E | MACB_RXDATA1_1_E | MACB_RXDATA2_1_E | MACB_RXDATA3_1_E | MACB_RX_ER_1_E | MACB_TX_ER_1_E | MACB_MDC_1_E | MACB_MDIO_1_E | MACB_RX_DV_1_E | MACB_COL_1_E));
    _CONFIG_PERIPHERAL_E(3, (MACB_TX_CLK_3_E));
    #else
    _CONFIG_PERIPHERAL_A(1, (MACB_TX_CLK_A | MACB_RX_CLK_A | MACB_TX_EN_A | MACB_TXDATA0_A | MACB_TXDATA1_A | MACB_TXDATA2_A | MACB_TXDATA3_A | MACB_CRS_A | MACB_RXDATA0_A | MACB_RXDATA1_A | MACB_RXDATA2_A | MACB_RXDATA3_A | MACB_RX_ER_A | MACB_TX_ER_A | MACB_RX_MDC_A | MACB_RX_MDIO_A | MACB_RX_DV_A | MACB_COL_A | MACB_SPEED_A));
    #endif
#endif
#ifdef SUPPORT_LAN_WAKEUP
    #ifdef _AT32UC3C
    _CONFIG_PERIPHERAL_E(0, MACB_WOL_0_E);                               // configure wake up on LAN port
    #else
    _CONFIG_PERIPHERAL_C(0, MACB_WOL_C);                                 // configure wake up on LAN port
    #endif
#endif
#if defined RESET_PHY                                                    // {26}
    DEASSERT_PHY_RST();
#endif

#if MASTER_CLOCK <= 20000000
    MACB_NCFGR = (PAE | SPD | MCLK_8);                                   // set correct clock speed for maintenance interface (valid up to 20MHz)
#elif MASTER_CLOCK <= 40000000
    MACB_NCFGR = (PAE | SPD | MCLK_16);                                  // set correct clock speed for maintenance interface (valid up to 40MHz)
#elif MASTER_CLOCK <= 80000000
    MACB_NCFGR = (PAE | SPD | MCLK_32);                                  // set correct clock speed for maintenance interface (valid up to 80MHz)
#elif MASTER_CLOCK <= 160000000
    MACB_NCFGR = (PAE | SPD | MCLK_64);                                  // set correct clock speed for maintenance interface (valid up to 160MHz)
#endif

#ifdef PHY_RMII
    MACB_USRIO = 0;                                                      // RMII mode
#else
    MACB_USRIO = MACB_USRIO_MII;                                         // {22} enable MII (not RMII)
#endif
    MACB_NCR   = MPE;                                                    // enable the MDIO (management channel to PHY) and enable pause frames

    ulPhyIdentifier = fnReadMII(DP83848_PHY_IDENTIFIER_1);               // check that the PHY is working correctly by reading its identifier
    ulPhyIdentifier <<= 16;
    ulPhyIdentifier |= fnReadMII(DP83848_PHY_IDENTIFIER_2);              // check that the PHY is working correctly by reading its identifier

    if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {
        return -1;                                                       // {20} if the identifier is incorrect the phy isn't responding correctly - return error                                                   // if the identifier is incorrect the phy isn't responding correctly
    }

#if defined _PHY_RTL8201E && defined PHY_RMII                            // {28}
    usMIIData = fnReadMII(DP83848_PHY_CONTROL_REGISTER);
    usMIIData |= 0x0400;                                                 // force RMII mode
    usMIIData &= ~0x0800;                                                // ensure that the PHY generates the 50MHz RMII clock
    fnWriteMII(DP83848_PHY_CONTROL_REGISTER, usMIIData);
#endif

    if (pars->usMode & (LAN_10M | LAN_100M)) {                           // if fixed speed
        if (pars->usMode & LAN_100M)  {
            usMIIData = PHY_SELECT_100M_SPEED;
        }
        else {
            usMIIData = 0;
            MACB_NCFGR &= ~SPD;
        }
        if (pars->usMode & FULL_DUPLEX) {
            MACB_NCFGR |= FD;
            usMIIData |= PHY_FULL_DUPLEX_MODE;
        }
    }
    else {                                                               // else auto-negotiate
        usMIIData = (PHY_ENABLE_AUTO_NEGOTIATION | PHY_RESTART_AUTO_NEGOTIATION);
    }

    if (pars->usMode & LOOPBACK) {
        usMIIData |= PHY_LOOP_BACK;                                      // test loop back at PHY
      //MACB_NCR |= (LLB);                                               // test loop back at MII
    }

    fnWriteMII(DP83848_BASIC_MODE_CONTROL_REGISTER, usMIIData);          // write the PHY operating mode

/***************************************** MAC set up ****************************************/

    i = NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * pars->usSizeRx;        // set up receiver buffer memory
    ptrBuffer = uMallocAlign((MAX_MALLOC)i, 8);                          // allocate a receive buffer pool - which should consist of AVR32_FIXED_BUFFER_SIZE byte blocks
    i /= AVR32_FIXED_BUFFER_SIZE;                                        // the number of BDs required to control the receive buffer pool, consisting of AVR32_FIXED_BUFFER_SIZE byte blocks
    ptrBd = uMallocAlign((MAX_MALLOC)(sizeof(AVR32_RX_BD) * i), 8);      // create a smaller pool of buffer descriptors

    MACB_RBQP = (unsigned long)ptrBd;                                    // inform the MAC where the first receive buffer is located
    ptrFirstRxBd = ptrRxBd = ptrBd;                                      // remember where it starts
    while (--i) {                                                        // initialise the buffer descriptors
        ptrBd->ptrBlock = ptrBuffer;                                     // initialise the buffer descriptors
        ptrBuffer += AVR32_FIXED_BUFFER_SIZE;                            // the receive buffer size MUST be the fixed buffer size..
        ptrBd++;
    }
    ptrBd->ptrBlock = (unsigned char *)((unsigned long)ptrBuffer | BD_RX_WRAP);// set wrap bit in last buffer

    i = (NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE * pars->usSizeTx);      // set up transmitter buffer memory
    ptrBuffer = uMallocAlign((MAX_MALLOC)i, 8);                          // allocate a receive buffer pool - which should consist of AVR32_FIXED_BUFFER_SIZE byte blocks

    i = NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE;                         // the number of BDs required to control the receive buffer pool, consisting of AVR32_FIXED_BUFFER_SIZE byte blocks
    ptrBd = uMallocAlign((MAX_MALLOC)(sizeof(AVR32_RX_BD) * i), 8);      // create a smaller pool of buffer descriptors

    MACB_TBQP = (unsigned long)ptrBd;                                    // inform the MAC where the first receive buffer is located
    ptrFirstTxBd = ptrTxBd = ptrBd;                                      // remember where it starts
    while (--i) {                                                        // initialise the buffer descriptors
        ptrBd->ptrBlock = ptrBuffer;                                     // initialise the buffer descriptors
        ptrBd->bd_details = BD_INFO_USED;                                // NOTE: we presently use one large transmit buffer only
        ptrBuffer += AVR32_FIXED_BUFFER_SIZE;                            // assuming AVR32_FIXED_BUFFER_SIZE byte block
        ptrBd++;
    }
    ptrBd->ptrBlock = ptrBuffer;
    ptrBd->bd_details = (BD_INFO_USED | BD_TX_WRAP);                     // set wrap bit in last buffer

    #if defined USE_IPV6                                                 // {19}
    ulTemp =  (network[DEFAULT_NETWORK].ucOurMAC[3] << 24);              // set the MAC address
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[2] << 16);
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[1] << 8);
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[0]);
    MACB_SA1B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  (network[DEFAULT_NETWORK].ucOurMAC[5] << 8);
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[4]);
    MACB_SA1T = ulTemp;
    #else
    ulTemp =  (pars->ucMAC[3] << 24);                                    // set the MAC address
    ulTemp |= (pars->ucMAC[2] << 16);
    ulTemp |= (pars->ucMAC[1] << 8);
    ulTemp |= (pars->ucMAC[0]);
    MACB_SA1B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  (pars->ucMAC[5] << 8);
    ulTemp |= (pars->ucMAC[4]);
    MACB_SA1T = ulTemp;
    #endif

    #ifdef SUPPORT_DISTRIBUTED_NODES
    ulTemp =  ucNodeMac[OurNetworkNumber-1][3]<<24;                      // set the uNetwork MAC address as second address
    ulTemp |= ucNodeMac[OurNetworkNumber-1][2]<<16;
    ulTemp |= ucNodeMac[OurNetworkNumber-1][1]<<8;
    ulTemp |= ucNodeMac[OurNetworkNumber-1][0];
    MACB_SA2B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  ucNodeMac[OurNetworkNumber-1][5]<<8;
    ulTemp |= ucNodeMac[OurNetworkNumber-1][4];
    MACB_SA2T = ulTemp;
        #ifdef UNET_BROADCAST_MAC_ADDRESS
    ulTemp =  cucuNetworkBroadcast[3]<<24;                               // set the uNetwork broadcast MAC address as third address
    ulTemp |= cucuNetworkBroadcast[2]<<16;
    ulTemp |= cucuNetworkBroadcast[1]<<8;
    ulTemp |= cucuNetworkBroadcast[0];
    MACB_SA3B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  cucuNetworkBroadcast[5]<<8;
    ulTemp |= cucuNetworkBroadcast[4];
    MACB_SA3T = ulTemp;
        #endif
    #endif

    if (pars->usMode & PROMISCUOUS) {
        MACB_NCFGR |= CAF;                                               // no destination address checking
    }
    #ifdef USE_IPV6
    else if (pars->usMode & CON_MULTICAST) {                             // {21} if multicast reception is to be configured
        fnConfigureMulticastIPV6();                                      // set IPV6 multicast address
    }
    #endif

    fnEnterAVRInterrupt(IR_GROUP_MACB, PRIORITY_MACB, (void (*)(void))_MACB_Interrupt); // enter the MACB interrupt handler

    MACB_IER = (TCOMP | RCOMP);                                          // enable MACB interrupts

    MACB_NCR |= (MACB_TE | MACB_RE | CLRSTAT);                           // enable transmitter and receiver

    #if defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS         // {25} when working with autonegotiation port interrupts should be used to enable the correct speed settings to be achieved in the interrupt handler
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = fnPhyInterrupt;                        // handling function
    interrupt_setup.int_priority = PRIORITY_GPIO;                        // port interrupt priority
    interrupt_setup.int_port = PORT_PHY_INT;                             // the port used
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | IRQ_ENABLE_GLITCH_FILER);// interrupt on this edge
    interrupt_setup.int_port_bits = PHY_INTERRUPT;                       // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure phy interrupt
    fnWriteMII(DP83848_MICR, (DP83848_INTOE | DP83848_INTEN));
    fnWriteMII(DP83848_MISR, DP83848_LINK_INT_EN);
    ulPhyIdentifier = fnReadMII(DP83848_MISR);                           // read the cause(s) of interrupts, which resets any pending bits which may otherwise be missed, returning the interrupt line to '1'
    #endif
    #ifdef _WINDOWS
    fnOpenDefaultHostAdapter();                                          // simulate link up
    #endif
    return 0;                                                            // {20} ethernet interface was successfully configured
}



#ifdef _WINDOWS

// Check whether the received MAC address matches a HASH address
//
static int fnMulticastHASH(unsigned char *ucData)                        // {21}
{
    if (MACB_NCFGR & (MTI | UNI)) {
        unsigned char ucHashIndex;
        ucHashIndex = (*ucData ^ ((*ucData >> 6) ^ (*(ucData + 1) << 2)) ^ ((*(ucData + 1) >> 4) ^ (*(ucData + 2) << 4)) ^ (*(ucData + 2) >> 2) ^ *(ucData + 3) ^ (*(ucData + 3) >> 6) ^ (*(ucData + 4) << 2) ^ (*(ucData + 4) >> 4) ^ (*(ucData + 5) << 4) ^ (*(ucData + 5) >> 2));
        ucHashIndex &= 0x3f;
        if (ucHashIndex <= 32) {
            if (MACB_HRB & (0x00000001 << ucHashIndex)) {
                if (*ucData & 0x01) {                                    // multicast address
                    if (MACB_NCFGR & MTI) {
                        return 1;                                        // multicast hash address match
                    }
                }
                else {                                                   // unicast address
                    if (MACB_NCFGR & UNI) {
                        return 2;                                        // unicast hash address match
                    }
                }
            }
        }
        else {
            ucHashIndex -= 32;
            if (MACB_HRT & (0x00000001 << ucHashIndex)) {
                if (*ucData & 0x01) {                                    // multicast address
                    if (MACB_NCFGR & MTI) {
                        return 1;                                        // multicast hash address match
                    }
                }
                else {                                                   // unicast address
                    if (MACB_NCFGR & UNI) {
                        return 2;                                        // unicast hash address match
                    }
                }
            }
        }
    }
    return 0;
}

// This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
// on the programmed MAC address and the Ethernet operating mode
//
extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
{
    int iHashType;
    unsigned char ucProgrammedMAC[MAC_LENGTH];
    #ifdef SUPPORT_DISTRIBUTED_NODES
    unsigned char ucProgrammedMAC2[MAC_LENGTH];
        #ifdef UNET_BROADCAST_MAC_ADDRESS
    unsigned char ucProgrammedMAC3[MAC_LENGTH];
        #endif
    #endif

    // This function checks to see whether we can receive the simulated frame and can block it if wanted
    //
    if (usLen < (2*MAC_LENGTH + 2)) {
        return 0;                                                        // too short to be a valid Ethernet frame
    }

    #ifdef SUPPORT_DISTRIBUTED_NODES
    ucProgrammedMAC2[5] = (unsigned char)(MACB_SA2T >> 8);
    ucProgrammedMAC2[4] = (unsigned char)(MACB_SA2T);
    ucProgrammedMAC2[3] = (unsigned char)(MACB_SA2B >> 24);
    ucProgrammedMAC2[2] = (unsigned char)(MACB_SA2B >> 16);
    ucProgrammedMAC2[1] = (unsigned char)(MACB_SA2B >> 8);
    ucProgrammedMAC2[0] = (unsigned char)(MACB_SA2B);

        #ifdef UNET_BROADCAST_MAC_ADDRESS
    ucProgrammedMAC3[5] = (unsigned char)(MACB_SA3T >> 8);
    ucProgrammedMAC3[4] = (unsigned char)(MACB_SA3T);
    ucProgrammedMAC3[3] = (unsigned char)(MACB_SA3B >> 24);
    ucProgrammedMAC3[2] = (unsigned char)(MACB_SA3B >> 16);
    ucProgrammedMAC3[1] = (unsigned char)(MACB_SA3B >> 8);
    ucProgrammedMAC3[0] = (unsigned char)(MACB_SA3B);
        #endif
    #endif

    ucProgrammedMAC[5] = (unsigned char)(MACB_SA1T >> 8);                // we check only first mac address
    ucProgrammedMAC[4] = (unsigned char)(MACB_SA1T);
    ucProgrammedMAC[3] = (unsigned char)(MACB_SA1B >> 24);
    ucProgrammedMAC[2] = (unsigned char)(MACB_SA1B >> 16);
    ucProgrammedMAC[1] = (unsigned char)(MACB_SA1B >> 8);
    ucProgrammedMAC[0] = (unsigned char)(MACB_SA1B);

    if (!uMemcmp(&ucData[MAC_LENGTH], ucProgrammedMAC, MAC_LENGTH)) {
        return 0;                                                        // we never accept echos of our own transmissions
    }

    #ifdef SUPPORT_DISTRIBUTED_NODES
    if (!uMemcmp((ucData + MAC_LENGTH), ucProgrammedMAC2, MAC_LENGTH)) {
        return 0;                                                        // ignore broadcast frames which we have just sent
    }
        #ifdef UNET_BROADCAST_MAC_ADDRESS
    if (!uMemcmp((ucData + MAC_LENGTH), ucProgrammedMAC3, MAC_LENGTH)) {
        return 0;                                                        // ignore broadcast frames which we have just sent
    }
        #endif
    #endif

    if (!(uMemcmp(ucData, cucBroadcast, MAC_LENGTH))) {
        return 1;                                                        // broadcast - always accept it
    }

    if (!(uMemcmp(ucData, ucProgrammedMAC, MAC_LENGTH))) {
        return 2;                                                        // our MAC
    }

    #ifdef SUPPORT_DISTRIBUTED_NODES
    if (!(uMemcmp(ucData, ucProgrammedMAC2, MAC_LENGTH))) {
        return 2;                                                        // our uNetwork MAC
    }
        #ifdef UNET_BROADCAST_MAC_ADDRESS
    if (!(uMemcmp(ucData, ucProgrammedMAC3, MAC_LENGTH))) {
        return 2;                                                        // our uNetwork broadcast MAC
    }
        #endif
    #endif

    if ((iHashType = fnMulticastHASH(ucData)) != 0) {                    // {21}
        if (iHashType == 1) {
            return 4;                                                    // multicast hash match
        }
        else {
            return 5;                                                    // unicast hash match
        }
    }

    if (MACB_NCFGR & CAF) {
        return 3;                                                        // in promiscuous mode we accept everything
    }

    return 0;                                                            // no match so reject
}
#endif


// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    volatile int iMaxWait = 0xffff;

    while (!(ptrTxBd->bd_details & BD_INFO_USED)) {
        if (!(--iMaxWait)) {
            return 1;                                                    // abort because we have waited too long
        }
    }
    return 0;
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return 1;          // NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE;       // the AVR32 driver doesn't need to know
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return 0;                                                            // for compatibility - the AVR32 uses buffer descriptors for tracking buffers
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    return (unsigned char *)ptrTxBd->ptrBlock;                           // return pointer to bd which is to be used next
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
// we don't use this in AVR32 since we copy input to a linear buffer
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
#ifdef _WINDOWS
    while (DataLen < 60) {                                               // the AVR32 performs automatic zero padding to increase shorter frames to minimum 60 byte length
        *ptr_put++ = 0x00;                                               // pad with zeros if smaller than 60 [chip must send at least 60]
        DataLen++;
    }
#endif

    ptrTxBd->bd_details &= ~(BD_INFO_USED | BD_TX_FRAME_LENGTH | BD_LAST_FRAME_BUFFER); // clear flags and the old length
    ptrTxBd->bd_details |= DataLen;                                      // set new frame length
    ptrTxBd->bd_details |= BD_LAST_FRAME_BUFFER;                         // if the frame fits in one buffer, set flag to say so
    MACB_NCR |= TSTART;                                                  // initiate transmission

#ifdef _WINDOWS
    fnSimulateEthTx((QUEUE_TRANSFER)(ptrTxBd->bd_details & BD_TX_FRAME_LENGTH));

    if (usPhyMode & PHY_LOOP_BACK) {                                     // if the PHY is in loop back mode, simulate reception of sent frame
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
    #ifdef PSEUDO_LOOPBACK                                               // if we detect an IP frame being sent to our own address we loop it back to the input
    else if (((*(ptr_put - DataLen + 12)) == 0x08) && (!(uMemcmp(&network[DEFAULT_NETWORK].ucOurIP[0], (ptr_put - DataLen + 26), IPV4_LENGTH)))) { // {19}
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
    #endif

    MACB_ISR |= TCOMP;
    MACB_TSR |= TSR_COMP;                                                // set so that interrupt type is recognised when using workaround
    ptrTxBd->bd_details |= (BD_INFO_USED);                               // simulate the MACB setting the used bit after transmission
    _MACB_Interrupt();                                                   // simulate end of transmission - tx interrupt
#endif
    return (DataLen);
}
#endif                                                                   // end of Ethernet option


#ifdef USB_INTERFACE                                                     // {6}
    #ifdef _WINDOWS
// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
    return endpoint->usEndpointSize;
}
    #endif


// This routine is called from the USB driver with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    /*
    UDP_IER = (0x1 << ucEndpointRef);                                    // enable the interrupt handling again, which will free up the buffer
    return USB_BUFFER_NO_DATA;                                           // report no more data so that the driver doesn't call again. The interrupt routine will handle all waiting buffers itself.
    */
    return 0;
}

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW *ptr_usb_hardware)
{
    if (!(UERST & (EPEN0 << iEndpoint))) {                               // check whether the endpoint is active
        return ENDPOINT_NOT_ACTIVE;
    }
    ptr_usb_hardware->ptr_ulUSB_BDControl = (ADD_UESTA0 + (iEndpoint * sizeof(unsigned long))); // set the corresponding control address
    if (iEndpoint == 0) {                                                // end point 0 has single buffer
        if ((*(ptr_usb_hardware->ptr_ulUSB_BDControl) & NBUSYBK_3) != 0) { // if in use
            return ENDPOINT_NOT_FREE;
        }        
    }
    //ptr_usb_hardware->ptrDatBuffer = (USBB_HSB_0 + (iEndpoint * 10000)); // and its DPRAM address

    /*
    volatile unsigned long *ptrCtr = (UDP_CSR0_ADD + iEndpoint);
    if (!(*ptrCtr & EPEDS)) {
        return ENDPOINT_NOT_ACTIVE;
    }
    if (*ptrCtr & TXPKTRDY) {                                            // transmission presently busy
        USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
        if ((endpoint->ucEndpointTxFlags & (ENDPOINT_DUAL_BANK | ENDPOINT_SECOND_BANK_BUSY)) != ENDPOINT_DUAL_BANK) { // check whether the endpoint is using double buffered transmission and the second buffer is free
            return ENDPOINT_NOT_FREE;
        }
    }
    ptr_usb_hardware->ptr_ulUSB_BDControl = ptrCtr;                      // set the corresponding control address
    ptr_usb_hardware->ptrDatBuffer = fnGetUSB_FIFOAdd((volatile unsigned char *)(ptrCtr + ((UDP_FDR0_ADD - (volatile unsigned char *)UDP_CSR0_ADD)/sizeof(unsigned long))), 1); // and the FIFO address
    */
    return ENDPOINT_FREE;
}

/*
static void fnClearBuffer(void)
{
    unsigned char ucDummy;
    unsigned short ulBufferContent;
    volatile unsigned char *ptrDPRAM = fnGetUSB_FIFOAdd(USBB_HSB_0, 0);
    CLEAR_STATUS(0, (RXSTPI | RXOUTI | NAKOUTI | NAKINI));
    ulBufferContent = (unsigned short)((UESTA0 & BYCT_MASK) >> BYCT_SHIFT);
    while (ulBufferContent--) {
        ucDummy = *ptrDPRAM;  // clear any unexpected data
    }
}*/

// Copy the data to be transmitted to the FIFO. This may only be called when there is a buffer to accept the data
//
extern void fnSendUSB_FIFO(unsigned char *ptrData, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW)
{
    if (iEndpoint == 0) {
        volatile unsigned char *ptrDPRAM = fnGetUSB_FIFOAdd(USBB_HSB_0, 0);
        unsigned short ulBufferContent;
        unsigned short usBytesWritten = 0;
        CLEAR_STATUS(0, (RXSTPI | RXOUTI | NAKOUTI | NAKINI | SHORTPACKET));
        
       // fnClearBuffer();// clear any unexpected data

ulBufferContent = (unsigned short)((UESTA0 & BYCT_MASK) >> BYCT_SHIFT);
if (ulBufferContent != 0) {
    *ptrDPRAM++ = 0x55;
    }
else {

        while (usLength--) {
            *ptrDPRAM++ = *ptrData++;
            usBytesWritten++;
#ifdef _WINDOWS
            UESTA0 &= ~(BYCT_MASK);
            UESTA0 |= ((usBytesWritten << BYCT_SHIFT) | NBUSYBK_1);
#endif
            ulBufferContent = (unsigned short)((UESTA0 & BYCT_MASK) >> BYCT_SHIFT);
            if (ulBufferContent > usBytesWritten) {
                ptrDPRAM -= usBytesWritten;
                ptrData -= usBytesWritten;
                usLength += usBytesWritten;
                usBytesWritten = 0;

              //SET_EPCONTROL(0, KILLBK); // kill the buffer
              //while (UECON0 & KILLBK) { // wait for the operation to complete
              //}
              //UERST |= EPRST0;
              //UERST &= ~EPRST0;
                UERST  &= ~EPEN0;
                UERST  |= EPEN0;
    #if ENDPOINT_0_SIZE == 8
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_8); // configure the control endpoint to match its size
    #elif ENDPOINT_0_SIZE == 16
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_16);
    #elif ENDPOINT_0_SIZE == 32
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_32);
    #elif ENDPOINT_0_SIZE == 64
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_64);
    #endif
             // fnClearBuffer();
             // *ptrDPRAM = (unsigned char)ulBufferContent; // the bad value
             // break; // something has gone wrong -. send what we have to see
            }
        }
}

      //uMemcpy((unsigned char *)fnGetUSB_FIFOAdd(USBB_HSB_0, 0), ptrData, usLength); // copy the data to the DPRAM corresponding to the endpoint
#ifdef _WINDOWS
      //UESTA0 |= ((usLength << BYCT_SHIFT) | NBUSYBK_1);                // when the DPRAM is written to the byte count is automatically incremented with the length to be sent
        *(ptrUSB_HW->ptr_ulUSB_BDControl) &= ~TXINE;
#else
        *(ptrUSB_HW->ptr_ulUSB_BDControl + (0x30/sizeof(unsigned long))) = TXINE; // clear the transmit flag to start transmission
#endif
        SET_EPCONTROL(0, TXINE);                                         // enable acknowledge interrupt
    }
    /*
    #ifdef _WINDOWS
    USB_END_POINT *sim_endpoint = (usb_endpoints + iEndpoint);
    volatile unsigned char *ptrFIFO_buffer = ptrUSB_HW->ptrDatBuffer;
    ptrFIFO_buffer = _SIM_SET_TX_PUT(iEndpoint, ptrFIFO_buffer);         // correct the simulated tx buffer to the correct bank
    sim_endpoint->ulSimFlags ^= PUT_BANK_1;                              // toggle the tx buffer for dual banked endpoints
    memcpy((unsigned char *)ptrFIFO_buffer, (unsigned char *)&usLength, sizeof(unsigned short));
    ptrFIFO_buffer += sizeof(unsigned short);
    while (usLength--) {
        *ptrFIFO_buffer++ = *ptrData++;                                  // write each byte to the tx FIFO
    }
    #else
    while (usLength--) {
        *ptrUSB_HW->ptrDatBuffer = *ptrData++;                           // write each byte to the tx FIFO
    }
    #endif
    if (*ptrUSB_HW->ptr_ulUSB_BDControl & TXPKTRDY) {                    // transmission presently busy - this will only happen when dual-buffered endpoint is being used
        USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
        endpoint->ucEndpointTxFlags = (ENDPOINT_SECOND_BANK_BUSY | ENDPOINT_DUAL_BANK); // mark that the next buffer content has been prepared
    }
    else {
        SYNCHRONISE_CONTROL_SET(TXPKTRDY);                               // start transmission
    }*/
}



// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine {79}
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    /*
    volatile unsigned long *ulEndpointCommand = UDP_CSR0_ADD + (ucEndpoint & ~IN_ENDPOINT);
    while (*ulEndpointCommand & (STALLSENT | FORCESTALL)) {
        *ulEndpointCommand &= ~(STALLSENT | FORCESTALL);
    }*/
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength)
{
    USB_END_POINT *endpoint = (usb_endpoints + ucEndpointRef);
    unsigned long endpoint_config;
    unsigned long ulEndpointBit = (EPEN0 << ucEndpointRef);

    if (ENDPOINT_DISABLE == ucEndpointType) {
      //UDP_IDR = (RXSUSP | RXRSM | ulEndpointBit);                      // disable suspend and resume interrupts when not configured
        UERST &= ~ulEndpointBit;                                         // disable the endpoint
        DISABLE_USB_INT(UDINT_EP0 << ucEndpointRef);                     // disable interrupt for endpoint
        *(UECFG0_ADR + ucEndpointRef) = 0;                               // free endpoint memory
        return;
    }
  //UDP_IER = RXSUSP;                                                    // since we have just been configured, enable suspend interrupt
  //#ifdef _WINDOWS
  //UDP_IMR |= RXSUSP;
  //#endif
    switch (ucEndpointType & ~IN_ENDPOINT) {
    case ENDPOINT_ISOCHRONOUS:
        endpoint_config = (UECFG_ALLOC | UECFG_EPBK_DOUBLE_BANK | UECFG_EPTYPE_ISO | UECFG_AUTOSW);
        break;
    case ENDPOINT_BULK:
        endpoint_config = (UECFG_ALLOC | UECFG_EPBK_DOUBLE_BANK | UECFG_EPTYPE_BULK | UECFG_AUTOSW);
        break;
    case ENDPOINT_INTERRUPT:
        endpoint_config = (UECFG_ALLOC | UECFG_EPBK_DOUBLE_BANK | UECFG_EPTYPE_INTERRUPT | UECFG_AUTOSW);
        break;
    default:                                                             // control endpoint
        endpoint_config = (UECFG_ALLOC | UECFG_EPBK_DOUBLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_AUTOSW);
        break;
    }
    if (((endpoint_config & UECFG_EPTYPE_MASK) != UECFG_EPTYPE_CONTROL) && (ucEndpointType & IN_ENDPOINT)) { // if IN type
        endpoint_config |= UECFG_EPDIR_IN;                               // change the OUT mode to IN mode
    }
    endpoint->usEndpointSize = usEndpointLength;                         // enter the endpoint length for later reference
    switch (usEndpointLength) {
    case 8:
        break;
    case 16:
        endpoint_config |= UECFG_EPSIZE_16;
        break;
    case 32:
        endpoint_config |= UECFG_EPSIZE_32;
        break;
    case 64:
        endpoint_config |= UECFG_EPSIZE_64;
        break;
    case 128:
        if (ucEndpointRef < 5) {
            _EXCEPTION("illegal size on this endpoint");
        }
        endpoint_config |= UECFG_EPSIZE_128;
        break;
    case 256:
        if (ucEndpointRef < 5) {
            _EXCEPTION("illegal size on this endpoint");
        }
        endpoint_config |= UECFG_EPSIZE_256;
        break;
    default:
        _EXCEPTION("illegal endpoint size");
    }
    if (ucEndpointRef != 0) {                                            // not endpoints with no dual bank
        endpoint->ucEndpointRxFlags = ENDPOINT_DUAL_BANK;                // flag that dual bank is in use on this endpoint
        endpoint->ucEndpointTxFlags = ENDPOINT_DUAL_BANK;
    }
    else {
        endpoint->ucEndpointRxFlags = 0;                                 // reset flags
        endpoint->ucEndpointTxFlags = 0;
        endpoint_config &= ~endpoint_config;
    }
    *(UECFG0_ADR + ucEndpointRef) = endpoint_config;                     // configure the endpoint mode
    UERST |= ulEndpointBit;                                              // enable the endpoint
    ENABLE_USB_INT(UDINT_EP0 << ucEndpointRef);                          // enable endpoint's interrupts
    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simulator that USB enumeration has completed
}

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *ptrUSB_HW, unsigned char ucFrameType)
{
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting messages to queue
    switch (fnUSB_handle_frame(ucFrameType, (unsigned char *)ptrUSB_HW->ptrDatBuffer, iEndpoint_ref, ptrUSB_HW)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref);
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        iInterruptLevel = 0;
        return MAINTAIN_OWNERSHIP;
    case STALL_ENDPOINT:                                                 // send stall
      //SYNCHRONISE_CONTROL_CLEAR(RXSETUP);
        if (iEndpoint_ref == 0) {                                        // check whether control 0 endpoint
            SET_EPCONTROL(0, STALLRQ);                                   // stall the control endpoint
        }
        else {
            unsigned long *ulOutControl = (unsigned long *)ptrUSB_HW->ptr_ulUSB_BDControl;
            iEndpoint_ref = fnGetPairedIN(iEndpoint_ref);                // get the paired IN endpoint reference
            ptrUSB_HW->ptr_ulUSB_BDControl = 0; //UDP_CSR0_ADD;
            ptrUSB_HW->ptr_ulUSB_BDControl += iEndpoint_ref;
//            SYNCHRONISE_CONTROL_SET(FORCESTALL);                         // stall/halt the corresponding IN endpoint
            ptrUSB_HW->ptr_ulUSB_BDControl = ulOutControl;               // return the OUT control pointer so that the receiver can be cleared
        }
        fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);      // mark that the endpoint is stalled
        _SIM_USB(USB_SIM_STALL, iEndpoint_ref, ptrUSB_HW);
        break;
    }
  //SYNCHRONISE_CONTROL_CLEAR(ptrUSB_HW->ulRxControl);                   // free receive buffer just handled
    iInterruptLevel = 0;
    return 0;
}



// USBB interrupt handler
//
static __interrupt void _USBB_Interrupt(void)
{
    static USB_HW usb_hardware = {0};
    unsigned long ulGlobalState;
    while ((ulGlobalState = UDINT) & UDINTE) {                           // while enabled interrupt are detected
        if (ulGlobalState & UDINT_EORST) {                               // USB reset completed
            UDINTCLR = UDINT_EORST;                                      // clear the interrupt
            UERST = (EPRST0 | EPRST1 | EPRST2 | EPRST3 | EPRST4 | EPRST5 | EPRST6); // ensure all endpoints are reset
            fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware); // generic handler routine
            UERST = EPEN0;                                               // enable the control endpoint
            usb_hardware.ucUSBAddress = 0;                               // reset the address to revert back to the default state
            UDCON |= UDCON_ADDEN;                                        // address is cleared by reset
            CLEAR_EPCONTROL(0, TXINE);
            SET_EPCONTROL(0, (RXSTPE | RXOUTE));                         // interrupts on received SETUP tokens on control endpoint
        }
        else if (ulGlobalState & UDINT_EP0) {                            // setup on control endpoint
          //USB_END_POINT *endpoint = (usb_endpoints + 0);
            UDINTCLR = UDINT_EP0;                                        // clear the endpoint interrupt
            usb_hardware.ptr_ulUSB_BDControl = ADD_UESTA0;               // set pointer to status register of this endpoint
            if (*usb_hardware.ptr_ulUSB_BDControl & RXSTPI) {            // SETUP token received
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(USBB_HSB_0, 0); // set pointer to DPRAM address for this endpoint
              //endpoint->ucEndpointRxFlags |= ENDPOINT_RX_DATA1;        // DATA1 OUT expected after a setup
                CLEAR_STATUS(0, RXSTPI);                                 // reset setup token flag and free the bank (this needs to be performed before handling the data so that transmission is not blocked)
                fnProcessInput(0, &usb_hardware, USB_SETUP_FRAME);       // handle setup data
            }
            else if (*usb_hardware.ptr_ulUSB_BDControl & RXOUTI) {
                /*
                unsigned char ucOriginalState = endpoint->ucEndpointRxFlags;
    #ifdef _SUPRESS_REPEAT_DATA
                unsigned char ucDataCheck = (ucOriginalState & ENDPOINT_RX_DATA1);
    #endif*/
                usb_hardware.usLength = (unsigned short)(*usb_hardware.ptr_ulUSB_BDControl >> BYCT_SHIFT); // the received data length
                CLEAR_STATUS(0, RXOUTI);                                 // acknowledge the interrupt and free the bank
                fnProcessInput(0 /*iEndpoint_ref*/, &usb_hardware, USB_CONTROL_OUT_FRAME); // control endpoint
                /*
                if (ucOriginalState & ENDPOINT_DUAL_BANK) {          // additional handling for dual buffer receivers
                    if (*usb_hardware.ptr_ulUSB_BDControl & RX_DATA_BK1) { // reception in bank 1
                        if (*usb_hardware.ptr_ulUSB_BDControl & RX_DATA_BK0) { // reception also waiting in bank 0 (two buffers)
    #ifdef _SUPRESS_REPEAT_DATA
                            ucDataCheck ^= ENDPOINT_RX_DATA1;        // invert DATA flag when both buffers are occupied
    #endif
                            if (ucOriginalState & ENDPOINT_RX_BANK1) {// bank1 was handled last time around so handle bank 0
                                endpoint->ucEndpointRxFlags = ENDPOINT_DUAL_BANK; // bank 0 handled (RX_DATA_BK0 already set per default)
                            }
                            else {
                                usb_hardware.ulRxControl = RX_DATA_BK1; // handle bank 1 buffer and mark that it was handled
                                endpoint->ucEndpointRxFlags = (ENDPOINT_DUAL_BANK | ENDPOINT_RX_BANK1); // bank 1 handled
                            }
                        }
                        else {
                            usb_hardware.ulRxControl = RX_DATA_BK1;  // handle bank 1 buffer and mark that it was handled
                            endpoint->ucEndpointRxFlags = (ENDPOINT_DUAL_BANK | ENDPOINT_RX_BANK1); // bank 1 to be handled
                        }
                    }
                    else {
                        endpoint->ucEndpointRxFlags = ENDPOINT_DUAL_BANK; // bank 0 to be handled (RX_DATA_BK0 already set per default)
                    }
                }
    #ifdef _SUPRESS_REPEAT_DATA                                          // check the validity of the data toggle flag
                if (((*usb_hardware.ptr_ulUSB_BDControl & DTGLE) == 0) == (ucDataCheck == 0)) {
        #ifdef RESET_USB_EP_INT_LATER                                    // clear the endpoint's interrupt
            UDP_ICR = (0x1 << iEndpoint_ref);                        // reset interrupt flag
            #ifdef _WINDOWS
            UDP_ISR &= ~(0x1 << iEndpoint_ref);
            #endif
        #endif
                    SYNCHRONISE_CONTROL_CLEAR_(usb_hardware.ulRxControl);// free repeat receive buffer so that it can receive further data
                    continue;                                        // false data so ignore
                }
                endpoint->ucEndpointRxFlags |= ((ucOriginalState ^ ENDPOINT_RX_DATA1) & ENDPOINT_RX_DATA1);
    #endif
                if ((*usb_hardware.ptr_ulUSB_BDControl & EPTYPE_MASK) != EPTYPE_CONTROL) {
                    if (fnProcessInput(iEndpoint_ref, &usb_hardware, USB_OUT_FRAME) == MAINTAIN_OWNERSHIP) { // non-control endpoint
                        UDP_IDR = (0x1 << iEndpoint_ref);            // the application cannot accept the data at the moment so disable interrupts on the endpoint
                        endpoint->ucEndpointRxFlags = ucOriginalState; // return the original state since the frame was not handled and is still in the FIFO
                    }
                }
                else {
                     fnProcessInput(iEndpoint_ref, &usb_hardware, USB_CONTROL_OUT_FRAME); // control endpoint
                }*/
            }
            else if (*usb_hardware.ptr_ulUSB_BDControl & TXINI) {        // data has been successfully transmitted
                USB_END_POINT *endpoint = (usb_endpoints/* + iEndpoint_ref*/);
                CLEAR_EPCONTROL(0, TXINE);                               // disable further interrupts from the set flag
/*                if (endpoint->ucEndpointTxFlags == (ENDPOINT_DUAL_BANK | ENDPOINT_SECOND_BANK_BUSY)) { // a second buffer is queued so start it not
                    endpoint->ucEndpointTxFlags = ENDPOINT_DUAL_BANK;
                    SYNCHRONISE_CONTROL_SET_(TXPKTRDY);                  // start transmission of second buffer, which has already been queued
                    _SIM_USB(USB_SIM_TX, iEndpoint_ref, &usb_hardware);
                }*/
              //SYNCHRONISE_CONTROL_CLEAR_(TXCOMP);                      // clear the flag and interrupt
                iInterruptLevel = 1;                                     // ensure interrupts remain blocked when putting message to queue
                fnUSB_handle_frame(USB_TX_ACKED, 0, 0/*iEndpoint_ref*/, &usb_hardware); // handle ack event
                iInterruptLevel = 0;
                UDCON |= (usb_hardware.ucUSBAddress | UDCON_ADDEN);      // program and enable the address to be used
            }
        }
        else if (ulGlobalState & UDINT_SUSP) {                           // suspend
            UDINTCLR = UDINT_SUSP;                                       // clear the interrupt
            fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);           // generic handler routine
        }
        else if (ulGlobalState & UDINT_EORSM) {                          // resume
            UDINTCLR = UDINT_EORSM;                                      // clear the interrupt
            fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);            // generic handler routine
        }
    #ifdef _WINDOWS
        UDINT &= ~(UDINTCLR);
        UDINTCLR = 0;
    #endif
    }
}

// This routine reads rx FIFO into an intermediate buffer. It returns a pointer to the start of the static buffer
//
extern unsigned char *fnReadUSB_FIFO(volatile unsigned char *fifo_addr, unsigned short length, unsigned short Offset)
{
    static unsigned char ucFIFO[MAX_FIFO_LENGTH];                        // intermediate FIFO data buffer for simpler and compatible interface with the application
    unsigned short usBufOffset = Offset;
    while (length--) {
        ucFIFO[usBufOffset++] = *fifo_addr;
    #ifdef _WINDOWS
        fifo_addr++;
    #endif
    }
    return ucFIFO;
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    #if defined _AT32UC3C
    PM_GEN_USB_CTRL = (PM_GCCTRL_PLLSEL | PM_GCCTRL_CEN) ;               // enable USBB generic clock from PLL0 which is assumed to be 48MHz
    #if defined _WINDOWS
        #if PLL_OUTPUT_FREQ_INT != 48000000                              // warn in case the USB clock doesn't have the correct freqnecy
            #error USBB clock is not 48MHz !!
        #endif
    #endif
    POWER_UP(HSB, HSBMASK_USBC);
    POWER_UP(PBB, PBBMASK_USBC);                                         // power up the USBC module
    #else
    PM_GEN_USB_CTRL = (PM_GCCTRL_PLLSEL | PM_GCCTRL_CEN) ;               // enable USBB generic clock from PLL0 which is assumed to be 48MHz
    #if defined _WINDOWS
        #if PLL_OUTPUT_FREQ_INT != 48000000                              // warn in case the USB clock doesn't have the correct freqnecy
            #error USBB clock is not 48MHz !!
        #endif
    #endif
    POWER_UP(HSB, HSBMASK_USBB);
    POWER_UP(PBB, PBBMASK_USBB);                                         // power up the USBB module
    #endif
    USBCON = (USBCON_UIMOD | USBCON_FRZCLK);                             // force device mode of operation

    if (++pars->ucEndPoints > NUMBER_OF_USB_ENDPOINTS) {                 // add endpoint 0
        pars->ucEndPoints = NUMBER_OF_USB_ENDPOINTS;                     // limit to maximum available in device
    }

    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * pars->ucEndPoints)); // get zeroed endpoint control structures
    USBCON = (USBCON_UIMOD | USBCON_USBEN | USBCON_OTGPADE | USBCON_VBUSHWC); // enable operation - latching device mode

    usb_endpoints[0].usEndpointSize = ENDPOINT_0_SIZE;                   // fixed endpoint size on control endpoint 0
    #if ENDPOINT_0_SIZE == 8
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_8); // configure the control endpoint to match its size
    #elif ENDPOINT_0_SIZE == 16
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_16);
    #elif ENDPOINT_0_SIZE == 32
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_32);
    #elif ENDPOINT_0_SIZE == 64
    UECFG0 = (UECFG_ALLOC | UECFG_EPBK_SINGLE_BANK | UECFG_EPTYPE_CONTROL | UECFG_EPSIZE_64);
    #else
        #error invalid enpoint 0 size!!
    #endif
    fnEnterAVRInterrupt(IR_GROUP_USBB, PRIORITY_USBB, (void (*)(void))_USBB_Interrupt); // enable USB interrupt
    if (pars->usConfig & USB_FULL_SPEED) {                               // control pull ups and pull downs to select speed
        UDCON = (UDCON_DETACH | UDCON_ADDEN);                            // enable pull up on D+ for full speed operation
    }
    else {
        UDCON = (UDCON_DETACH | UDCON_LS | UDCON_ADDEN);                 // enable pull up on D- for low speed operation
    }
    UERST = EPEN0;                                                       // enable the control endpoint
    UDCON &= ~UDCON_DETACH;                                              // connect the device to the bus
    UDINTCLR = (UDINT_EORST | UDINT_SUSP | UDINT_EORSM | UDINT_EP0);     // clear pending interrupts
    SET_EPCONTROL(0, RXSTPE);                                            // interrupts on received SETUP tokens on control endpoint
    ENABLE_USB_INT(UDINT_EORST | UDINT_SUSP | UDINT_EORSM | UDINT_EP0);  // enable interrupts for enumeration and state changes
}
#endif                                                                   // end USB_INTERFACE


#ifdef SERIAL_INTERFACE
/* =================================================================== */
/*                   Serial Interface - USART                          */
/* =================================================================== */

static unsigned long *fnSelectChannel(QUEUE_HANDLE Channel)
{
    switch (Channel) {
    default:
        return (unsigned long *)USART0_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
#if NUMBER_SERIAL > 1
    case 1:
        return (unsigned long *)USART1_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
#endif
#if NUMBER_SERIAL > 2
    case 2:
        return (unsigned long *)USART2_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
#endif
#if NUMBER_SERIAL > 3
    case 3:
        return (unsigned long *)USART3_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
#endif
#if NUMBER_SERIAL > 4
    case 4:
        return (unsigned long *)USART4_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
#endif
    }
}


#ifdef SERIAL_SUPPORT_DMA
// Interrupt from one of the peripheral DMA channels
//
static __interrupt void _PDMA_Interrupt(void)
{
    register volatile unsigned long ulDMA_channels;
    while ((ulDMA_channels = IRR3) != 0) {                               // while there are pending DMA channel interrpts
        if (ulDMA_channels & IR_PDCA0) {                                 // UART 0 TX
            US_IDR_0 = ENDTX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_0 &= ~ENDTX;
            IRR3 &= ~IR_PDCA0;
    #endif
            fnSciTxByte(0);                                              // transmit block complete interrupt - write next block
        }
    }
}
#endif

// Configure the SCI hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    unsigned long *ulReg = fnSelectChannel(Channel);
#ifdef SERIAL_SUPPORT_DMA
    UART_PDCA *ptrDMA = (UART_PDCA *)PDCA_BLOCK;
    ptrDMA += Channel;
#endif
    switch (Channel) {
    case 0:
        POWER_UP(PBA, PBAMASK_USART0);                                   // enable clocks to UART0
        break;
#if NUMBER_SERIAL > 1
    case 1:
    #ifdef _AT32UC3C                                                     // {14}
        POWER_UP(PBC, PBCMASK_USART1);                                   // enable clocks to UART        
    #else
        POWER_UP(PBA, PBAMASK_USART1);                                   // enable clocks to UART
    #endif
        break;
#endif
#if NUMBER_SERIAL > 2
    case 2:
        POWER_UP(PBA, PBAMASK_USART2);                                   // enable clocks to UART2
        break;
#endif
#if NUMBER_SERIAL > 3
    case 3:
        POWER_UP(PBA, PBAMASK_USART3);                                   // enable clocks to UART3
        break;
#endif
#if NUMBER_SERIAL > 4
    case 4:
        POWER_UP(PBC, PBCMASK_USART4);                                   // enable clocks to UART4
        break;
#endif
    default:
        return;
    }

#ifdef SERIAL_SUPPORT_DMA
    if (pars->ucDMAConfig & (UART_TX_DMA | UART_RX_DMA)) {
        POWER_UP(HSB, HSBMASK_PDCA);                                     // apply power to the peripheral DMA module
        HSB_MCFG0 = SINGLE_ACCESS;                                       // this ensures that CPU instruction bursts don't block DMA operation
        fnEnterAVRInterrupt(IR_GROUP_PDCA, PRIORITY_DMA, (void (*)(void))_PDMA_Interrupt); // enter the DMA interrupt handler
        if (pars->ucDMAConfig & UART_TX_DMA) {                           // prepare the DMA channel for use by the transmitter
            ptrDMA->DMA_PSR = (PDC_HANDSHAKE_USART0_TX + Channel);       // defined the handshake signals for use by the DMA channel
        }
        if (pars->ucDMAConfig & UART_RX_DMA) {                           // prepare the DMA channel for use by the receiver
            ptrDMA += NUMBER_SERIAL,
            ptrDMA->DMA_PSR = (PDC_HANDSHAKE_USART0_RX + Channel);       // defined the handshake signals for use by the DMA channel
        }
        ptrDMA->DMA_MR  = DMA_BYTE_TRANSFER;                             // transfers are always single bytes in size
        ptrDMA->DMA_IER = DMA_TRC;                                       // enable transfer complete interrupt
    #ifdef _WINDOWS
        ptrDMA->DMA_IMR |= ptrDMA->DMA_IER;
        ptrDMA->DMA_IER = 0;
    #endif
    }
#endif

    #ifdef SUPPORT_HW_FLOW
    if (pars->Config & RTS_CTS) {                                        // HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    *ulReg = (AVR32_TX_OFF | AVR32_RX_OFF);                              // start with uart disabled
    ulReg += (US_BRGR_OFFSET/sizeof(unsigned long));                     // set to baud rate generator register

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        *ulReg = ((PBA_CLOCK/16 + 300/2)/300);                           // set 300
        break;
    case SERIAL_BAUD_600:
        *ulReg = ((PBA_CLOCK/16 + 600/2)/600);                           // set 600
        break;
    case SERIAL_BAUD_1200:
        *ulReg = ((PBA_CLOCK/16 + 1200/2)/1200);                         // set 1200
        break;
    case SERIAL_BAUD_2400:
        *ulReg = ((PBA_CLOCK/16 + 2400/2)/2400);                         // set 2400
        break;
    case SERIAL_BAUD_4800:
        *ulReg = ((PBA_CLOCK/16 + 4800/2)/4800);                         // set 4800
        break;
    case SERIAL_BAUD_9600:
        *ulReg = ((PBA_CLOCK/16 + 9600/2)/9600);                         // set 9600
        break;
    case SERIAL_BAUD_14400:
        *ulReg = ((PBA_CLOCK/16 + 14400/2)/14400);                       // set 14400
        break;
    default:                                                             // if not valid value set this one
    case SERIAL_BAUD_19200:
        *ulReg = ((PBA_CLOCK/16 + 19200/2)/19200);                       // set 19200
        break;
    case SERIAL_BAUD_38400:
        *ulReg = ((PBA_CLOCK/16 + 38400/2)/38400);                       // set 38400
        break;
    case SERIAL_BAUD_57600:
        *ulReg = ((PBA_CLOCK/16 + 57600/2)/57600);                       // set 57600
        break;
    case SERIAL_BAUD_115200:
        *ulReg = ((PBA_CLOCK/16 + 115200/2)/115200);                     // set 115200
        break;
    case SERIAL_BAUD_230400:
        *ulReg = ((PBA_CLOCK/16 + 230400/2)/230400);                     // set 230400
        break;
    }

    ulReg -= ((US_BRGR_OFFSET - US_MR_OFFSET)/sizeof(unsigned long));    // set to mode register

    if (pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {
        if (pars->Config & RS232_ODD_PARITY) {
          *ulReg = AVR32_ODD_PARITY;
        }
        else {
          *ulReg = AVR32_EVEN_PARITY;
        }
    }
    else {
        *ulReg = AVR32_NO_PARITY;
    }

    if (pars->Config & TWO_STOPS) {
        *ulReg |= AVR32_2_STOPS;
    }
    else if (pars->Config & ONE_HALF_STOPS) {
        *ulReg |= AVR32_ONE_HALF_STOPS;
    }

#if defined UART_EXTENDED_MODE && (defined SERIAL_MULTIDROP_TX || defined SERIAL_MULTIDROP_RX) // {12}
    if (pars->Config & (MULTIDROP_MODE_RX | MULTIDROP_MODE_TX)) {        // 9-bit - multi-drop mode
        *ulReg |= (USART_MODE9);
    }
    else if (pars->Config & CHAR_7) {
        *ulReg |= AVR32_CHAR_7;
    }
    else {
        *ulReg |= AVR32_CHAR_8;
    }
#else
    if (pars->Config & CHAR_7) {
        *ulReg |= AVR32_CHAR_7;
    }
    else {
        *ulReg |= AVR32_CHAR_8;
    }
#endif

#ifdef MODBUS_RTU
    if (pars->Config & RTU_RX_MODE) {                                    // use internal idle timeout on the USARTs
        *(ulReg + 8) = 17;                                               // timeout after 17 bit period with no chanacter reception (T1.5) [US_RTOR]
        *(ulReg + 1) = RX_TIMEOUT;                                       // enable interrupt on idle timeout [US_IER]
    }
    else {
        *(ulReg + 8) = 0;                                                // disable timeouts [US_RTOR]
    }
#endif
#ifdef _WINDOWS
    ulReg += ((US_BRGR_OFFSET - US_MR_OFFSET)/sizeof(unsigned long));    // set back to speed setting
    if (Channel == 2) {
        pars->Config &= ~CHAR_7;                                         // the DBGU only supports 8 bit characters
    }
    fnConfigSimSCI(Channel, MASTER_CLOCK/(*ulReg*16), pars);             // open a serial port on PC if desired
#endif
}


// USART0 interrupt
//
static __interrupt void _USART0_Interrupt(void)
{
#ifdef SUPPORT_HW_FLOW
    unsigned long ulStatus;
#endif
    while (IRR_UART0 & IR_USART0) {                                      // while interrupts present from UART0
        iInterruptLevel = 1;
#ifdef SUPPORT_HW_FLOW
        ulStatus = US_CSR_0;                                             // read and reset the input change states
    #ifdef _WINDOWS
        US_CSR_0 &= ~CTSIC;
    #endif
#endif
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_0 & RXRDY) && (US_CSR_0 & RXRDY))
#else
        if (US_CSR_0 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {12}
    #ifdef _WINDOWS
            ulLastReceived[0] = US_RHR_0;                                // read the byte (clearing the interrupt) and storing the state of the 9th bit
            fnSciRxByte((unsigned char)ulLastReceived[0], 0);            // pass 8 data bits
    #else                                                                // target is big endian
            if (US_MR_0 & USART_MODE9) {                                 // if the channel is in 9 bit mode
                ulLastReceived[0] = US_RHR_0;                            // read the byte (clearing the interrupt) and storing the data
                if (ulLastReceived[0] & US_RHR_9TH_BIT) {
                    fnSciRxByte(0x01, 0);                                // 9th bit is '1'
                }
                else {
                    fnSciRxByte(0x00, 0);                                // 9th bit is '0'
                }
            }
            else {
                fnSciRxByte((unsigned char)US_RHR_0, 0);                 // read and pass 8 bit data on (clearing the interrupt)
            }
    #endif
#else
            fnSciRxByte((unsigned char)US_RHR_0, 0);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#ifdef _WINDOWS
            US_CSR_0 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_0 & ENDRX) && (US_CSR_0 & ENDRX)) {
            US_IDR_0 = ENDRX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_0 &= ~ENDRX;
            US_CSR_0 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 0);
        }
        if ((US_IMR_0 & ENDTX) && (US_CSR_0 & ENDTX)) {
            US_IDR_0 = ENDTX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_0 &= ~ENDTX;
    #endif
            fnSciTxByte(0);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_0 & TXRDY) && (US_CSR_0 & TXRDY)) {
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
        }
#ifdef SUPPORT_HW_FLOW
        if ((US_IMR_0 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(0, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#ifdef MODBUS_RTU
        if (((US_IMR_0 & RX_TIMEOUT) & US_CSR_0)) {                      // idle timeout
            int iNext = fnSciRxIdle(0);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_0 = (unsigned long)iNext;                        // set the new value
                US_CR_0 = (AVR32_STTTO | AVR32_RETTO);                   // reset and retrigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_0 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_0 = AVR32_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#ifdef _WINDOWS
        IRR_UART0 = 0;                                                   // simulate reset of interrupt flag
#endif
        iInterruptLevel = 0;
    }
}

#if NUMBER_SERIAL > 1
// USART1 interrupt
//
static __interrupt void _USART1_Interrupt(void)
{
#ifdef SUPPORT_HW_FLOW
    unsigned long ulStatus;
#endif
    while (IRR_UART1 & IR_USART1) {                                      // while interrupts present from UART1
        iInterruptLevel = 1;
#ifdef SUPPORT_HW_FLOW
        ulStatus = US_CSR_1;                                             // read and reset the input change states
    #ifdef _WINDOWS
        US_CSR_1 &= ~CTSIC;
    #endif
#endif
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_1 & RXRDY) && (US_CSR_1 & RXRDY))
#else
        if (US_CSR_1 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {12}
    #ifdef _WINDOWS
            ulLastReceived[1] = US_RHR_1;                                // read the byte (clearing the interrupt) and storing the state of the 9th bit
            fnSciRxByte((unsigned char)ulLastReceived[1], 1);            // pass 8 data bits
    #else                                                                // target is big endian
            if (US_MR_1 & USART_MODE9) {                                 // if the channel is in 9 bit mode
                ulLastReceived[1] = US_RHR_1;                            // read the byte (clearing the interrupt) and storing the data
                if (ulLastReceived[1] & US_RHR_9TH_BIT) {
                    fnSciRxByte(0x01, 1);                                // 9th bit is '1'
                }
                else {
                    fnSciRxByte(0x00, 1);                                // 9th bit is '0'
                }
            }
            else {
                fnSciRxByte((unsigned char)US_RHR_1, 1);                 // read and pass 8 bit data on (clearing the interrupt)
            }
    #endif
#else
            fnSciRxByte((unsigned char)US_RHR_1, 1);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#ifdef _WINDOWS
            US_CSR_1 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_1 & ENDRX) && (US_CSR_1 & ENDRX)) {
            US_IDR_1 = ENDRX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_1 &= ~ENDRX;
            US_CSR_1 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 1);
        }
        if ((US_IMR_1 & ENDTX) && (US_CSR_1 & ENDTX)) {
            US_IDR_1 = ENDTX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_1 &= ~ENDTX;
    #endif
            fnSciTxByte(1);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_1 & TXRDY) && (US_CSR_1 & TXRDY)) {
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
        }
#ifdef SUPPORT_HW_FLOW
        if ((US_IMR_1 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(1, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#ifdef MODBUS_RTU
        if (((US_IMR_1 & RX_TIMEOUT) & US_CSR_1)) {                      // idle timeout
            int iNext = fnSciRxIdle(1);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_1 = (unsigned long)iNext;                        // set the new value
                US_CR_1 = (AVR32_STTTO | AVR32_RETTO);                   // reset and retrigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_1 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_1 = AVR32_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#ifdef _WINDOWS
        IRR_UART1 = 0;                                                   // simulate reset of interrupt flag
#endif
        iInterruptLevel = 0;
    }
}
#endif
#if NUMBER_SERIAL > 2
// USART2 interrupt
//
static __interrupt void _USART2_Interrupt(void)
{
#ifdef SUPPORT_HW_FLOW
    unsigned long ulStatus;
#endif
    while (IRR_UART2 & IR_USART2) {                                      // while interrupts present from UART2
        iInterruptLevel = 1;
#ifdef SUPPORT_HW_FLOW
        ulStatus = US_CSR_2;                                             // read and reset the input change states
    #ifdef _WINDOWS
        US_CSR_2 &= ~CTSIC;
    #endif
#endif
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_2 & RXRDY) && (US_CSR_2 & RXRDY))
#else
        if (US_CSR_2 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {12}
    #ifdef _WINDOWS
            ulLastReceived[2] = US_RHR_2;                                // read the byte (clearing the interrupt) and storing the state of the 9th bit
            fnSciRxByte((unsigned char)ulLastReceived[2], 2);            // pass 8 data bits
    #else                                                                // target is big endian
            if (US_MR_2 & USART_MODE9) {                                 // if the channel is in 9 bit mode
                ulLastReceived[2] = US_RHR_2;                            // read the byte (clearing the interrupt) and storing the data
                if (ulLastReceived[2] & US_RHR_9TH_BIT) {
                    fnSciRxByte(0x01, 2);                                // 9th bit is '1'
                }
                else {
                    fnSciRxByte(0x00, 2);                                // 9th bit is '0'
                }
            }
            else {
                fnSciRxByte((unsigned char)US_RHR_2, 2);                 // read and pass 8 bit data on (clearing the interrupt)
            }
    #endif
#else
            fnSciRxByte((unsigned char)US_RHR_2, 2);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#ifdef _WINDOWS
            US_CSR_2 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_2 & ENDRX) && (US_CSR_2 & ENDRX)) {
            US_IDR_2 = ENDRX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_2 &= ~ENDRX;
            US_CSR_2 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 2);
        }
        if ((US_IMR_2 & ENDTX) && (US_CSR_2 & ENDTX)) {
            US_IDR_2 = ENDTX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_2 &= ~ENDTX;
    #endif
            fnSciTxByte(2);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_2 & TXRDY) && (US_CSR_2 & TXRDY)) {
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
        }
#ifdef SUPPORT_HW_FLOW
        if ((US_IMR_2 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(2, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#ifdef MODBUS_RTU
        if (((US_IMR_2 & RX_TIMEOUT) & US_CSR_2)) {                      // idle timeout
            int iNext = fnSciRxIdle(2);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_2 = (unsigned long)iNext;                        // set the new value
                US_CR_2 = (AVR32_STTTO | AVR32_RETTO);                   // reset and retrigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_2 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_2 = AVR32_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#ifdef _WINDOWS
        IRR_UART2 = 0;                                                   // simulate reset of interrupt flag
#endif
        iInterruptLevel = 0;
    }
}
#endif
#if NUMBER_SERIAL > 3
// USART3 interrupt
//
static __interrupt void _USART3_Interrupt(void)
{
#ifdef SUPPORT_HW_FLOW
    unsigned long ulStatus;
#endif
    while (IRR_UART3 & IR_USART3) {                                      // while interrupts present from UART3
        iInterruptLevel = 1;
#ifdef SUPPORT_HW_FLOW
        ulStatus = US_CSR_3;                                             // read and reset the input change states
    #ifdef _WINDOWS
        US_CSR_3 &= ~CTSIC;
    #endif
#endif
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_3 & RXRDY) && (US_CSR_3 & RXRDY))
#else
        if (US_CSR_3 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {12}
    #ifdef _WINDOWS
            ulLastReceived[3] = US_RHR_3;                                // read the byte (clearing the interrupt) and storing the state of the 9th bit
            fnSciRxByte((unsigned char)ulLastReceived[3], 3);            // pass 8 data bits
    #else                                                                // target is big endian
            if (US_MR_3 & USART_MODE9) {                                 // if the channel is in 9 bit mode
                ulLastReceived[3] = US_RHR_3;                            // read the byte (clearing the interrupt) and storing the data
                if (ulLastReceived[3] & US_RHR_9TH_BIT) {
                    fnSciRxByte(0x01, 3);                                // 9th bit is '1'
                }
                else {
                    fnSciRxByte(0x00, 3);                                // 9th bit is '0'
                }
            }
            else {
                fnSciRxByte((unsigned char)US_RHR_3, 3);                 // read and pass 8 bit data on (clearing the interrupt)
            }
    #endif
#else
            fnSciRxByte((unsigned char)US_RHR_3, 3);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#ifdef _WINDOWS
            US_CSR_3 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_3 & ENDRX) && (US_CSR_3 & ENDRX)) {
            US_IDR_3 = ENDRX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_3 &= ~ENDRX;
            US_CSR_3 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 3);
        }
        if ((US_IMR_3 & ENDTX) && (US_CSR_3 & ENDTX)) {
            US_IDR_3 = ENDTX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_3 &= ~ENDTX;
    #endif
            fnSciTxByte(3);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_3 & TXRDY) && (US_CSR_3 & TXRDY)) {
            fnSciTxByte(3);                                              // transmit data empty interrupt - write next byte
        }
#ifdef SUPPORT_HW_FLOW
        if ((US_IMR_3 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(3, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#ifdef MODBUS_RTU
        if (((US_IMR_3 & RX_TIMEOUT) & US_CSR_3)) {                      // idle timeout
            int iNext = fnSciRxIdle(3);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_3 = (unsigned long)iNext;                        // set the new value
                US_CR_3 = (AVR32_STTTO | AVR32_RETTO);                   // reset and retrigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_3 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_3 = AVR32_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#ifdef _WINDOWS
        IRR_UART3 = 0;                                                   // simulate reset of interrupt flag
#endif
        iInterruptLevel = 0;
    }
}
#endif

#if NUMBER_SERIAL > 4
// USART3 interrupt
//
static __interrupt void _USART4_Interrupt(void)
{
#ifdef SUPPORT_HW_FLOW
    unsigned long ulStatus;
#endif
    while (IRR_UART4 & IR_USART4) {                                      // while interrupts present from UART4
        iInterruptLevel = 1;
#ifdef SUPPORT_HW_FLOW
        ulStatus = US_CSR_4;                                             // read and reset the input change states
    #ifdef _WINDOWS
        US_CSR_4 &= ~CTSIC;
    #endif
#endif
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_4 & RXRDY) && (US_CSR_4 & RXRDY))
#else
        if (US_CSR_4 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX
    #ifdef _WINDOWS
            ulLastReceived[4] = US_RHR_4;                                // read the byte (clearing the interrupt) and storing the state of the 9th bit
            fnSciRxByte((unsigned char)ulLastReceived[4], 4);            // pass 8 data bits
    #else                                                                // target is big endian
            if (US_MR_4 & USART_MODE9) {                                 // if the channel is in 9 bit mode
                ulLastReceived[4] = US_RHR_4;                            // read the byte (clearing the interrupt) and storing the data
                if (ulLastReceived[4] & US_RHR_9TH_BIT) {
                    fnSciRxByte(0x01, 4);                                // 9th bit is '1'
                }
                else {
                    fnSciRxByte(0x00, 4);                                // 9th bit is '0'
                }
            }
            else {
                fnSciRxByte((unsigned char)US_RHR_4, 4);                 // read and pass 8 bit data on (clearing the interrupt)
            }
    #endif
#else
            fnSciRxByte((unsigned char)US_RHR_4, 4);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#ifdef _WINDOWS
            US_CSR_4 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#ifdef SERIAL_SUPPORT_DMA
        if ((US_IMR_4 & ENDRX) && (US_CSR_4 & ENDRX)) {
            US_IDR_4 = ENDRX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_4 &= ~ENDRX;
            US_CSR_4 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 4);
        }
        if ((US_IMR_4 & ENDTX) && (US_CSR_4 & ENDTX)) {
            US_IDR_4 = ENDTX;                                            // disable to avoid further
    #ifdef _WINDOWS
            US_IMR_4 &= ~ENDTX;
    #endif
            fnSciTxByte(4);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_4 & TXRDY) && (US_CSR_4 & TXRDY)) {
            fnSciTxByte(4);                                              // transmit data empty interrupt - write next byte
        }
#ifdef SUPPORT_HW_FLOW
        if ((US_IMR_4 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(4, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#ifdef MODBUS_RTU
        if (((US_IMR_4 & RX_TIMEOUT) & US_CSR_4)) {                      // idle timeout
            int iNext = fnSciRxIdle(4);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_4 = (unsigned long)iNext;                        // set the new value
                US_CR_4 = (AVR32_STTTO | AVR32_RETTO);                   // reset and retrigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_4 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_4 = AVR32_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#ifdef _WINDOWS
        IRR_UART4 = 0;                                                   // simulate reset of interrupt flag
#endif
        iInterruptLevel = 0;
    }
}
#endif


#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel)
{
#ifdef _WINDOWS
    if (ulLastReceived[channel] & US_RHR_9TH_BIT) {                      // {12} return 9th data bit value
        return 0x01;
    }  
    return 0x00;
#else                                                                    // hardware is big endian
    return (unsigned char)ulLastReceived[channel];
#endif
}
#endif
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX
extern void fnSetMultiDropByte(QUEUE_HANDLE channel, unsigned char ucMSB)// {12}
{
    ulNextSent[channel] = ucMSB;                                         // prepare the next extended data for transmission
}
#endif

// Enable reception on the defined channel
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    unsigned long *ulReg = fnSelectChannel(Channel);
    volatile unsigned long ulDummy;

    switch (Channel) {
    case 0:
    #ifdef _WINDOWS
        US_CSR_0 |= _RX_ENABLED;                                         // mark that the receiver is enabled for simulator use
    #endif
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART0_ON_3_1
        _CONFIG_PERIPHERAL_A(3, USART0_RXD_3_A);                         // configure Rx0 line
        #elif defined USART0_ON_2
        _CONFIG_PERIPHERAL_D(2, USART0_RXD_2_D);                         // configure Rx0 line
        #else
        _CONFIG_PERIPHERAL_E(3, USART0_RXD_3_E);                         // configure Rx0 line
        #endif
    #else
        #if defined USART0_ON_3 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(3, USART0_RX_3_B);                          // configure Rx0 line
        #elif defined USART0_ON_1 && defined _AT32UC3B && !defined CHIP_48_PIN
        _CONFIG_PERIPHERAL_C(1, USART0_RX_1_C);                          // configure Rx0 line
        #else
        _CONFIG_PERIPHERAL_A(0, USART0_RX_0_A);                          // configure Rx0 line
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART0, PRIORITY_UART0, (void (*)(void))_USART0_Interrupt);
        break;
#if NUMBER_SERIAL > 1
    case 1:
    #ifdef _WINDOWS
        US_CSR_1 |= _RX_ENABLED;                                         // mark that the receiver is enabled for simulator use
    #endif
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART1_ON_3
        _CONFIG_PERIPHERAL_E(3, USART1_RXD_3_A);                         // configure Rx1 line
        #else
        _CONFIG_PERIPHERAL_A(1, USART1_RXD_1_A);                         // configure Rx1 line
        #endif
    #else
        #if defined USART1_ON_3 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(3, USART1_RX_3_B);                          // configure Rx1 line
        #elif defined USART0_ON_1 && defined _AT32UC3B && !defined CHIP_48_PIN
        _CONFIG_PERIPHERAL_C(1, USART1_RX_1_C);                          // configure Rx1 line
        #else
        _CONFIG_PERIPHERAL_A(0, USART1_RX_0_A);                          // configure Rx1 line
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART1, PRIORITY_UART1, (void (*)(void))_USART1_Interrupt);
        break;
#endif
#if NUMBER_SERIAL > 2 && (!defined _AT32UC3B || !defined CHIP_48_PIN)
    case 2:
    #ifdef _WINDOWS
        US_CSR_2 |= _RX_ENABLED;                                         // mark that the receiver is enabled for simulator use
    #endif
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART2_ON_PC2_3
        _CONFIG_PERIPHERAL_C(2, USART2_RXD_2_0_C);                      // configure Rx2 line
        #else
        _CONFIG_PERIPHERAL_C(2, USART2_RXD_2_1_C);                      // configure Rx2 line
        #endif
    #else
        #ifdef _AT32UC3B                                                 // {3}
            #if defined USART2_ON_PINS_26_27
        _CONFIG_PERIPHERAL_B(0, USART2_RX_0_B);                          // configure Rx2 line
            #else
        _CONFIG_PERIPHERAL_C(0, USART2_RX_0_C);                          // configure Rx2 line
            #endif
        #else
            #if defined USART2_ON_3 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(2, USART2_RX_2_B);                          // configure Rx2 line
            #else
        _CONFIG_PERIPHERAL_A(1, USART2_RX_1_A);                          // configure Rx2 line
            #endif
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART2, PRIORITY_UART2, (void (*)(void))_USART2_Interrupt);
        break;
#endif
#if NUMBER_SERIAL > 3
    case 3:
    #ifdef _WINDOWS
        US_CSR_3 |= _RX_ENABLED;                                         // mark that the receiver is enabled for simulator use
    #endif
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART3_ON_2
        _CONFIG_PERIPHERAL_F(2, USART3_RXD_2_F);                         // configure Rx3 line
        #elif defined USART3_ON_3_1
        _CONFIG_PERIPHERAL_B(3, USART3_RXD_3_B);                         // configure Rx3 line
        #else
        _CONFIG_PERIPHERAL_A(3, USART3_RXD_3_A);                         // configure Rx3 line
        #endif
    #else
        #if defined USART3_ON_2 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(2, USART3_RX_2_B);                          // configure Rx3 line
        #else
        _CONFIG_PERIPHERAL_B(1, USART3_RX_1_B);                          // configure Rx3 line
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART3, PRIORITY_UART3, (void (*)(void))_USART3_Interrupt);
        break;
#endif
#if NUMBER_SERIAL > 4
    case 4:
    #ifdef _WINDOWS
        US_CSR_4 |= _RX_ENABLED;                                         // mark that the receiver is enabled for simulator use
    #endif
    #if defined USART4_ON_3
        _CONFIG_PERIPHERAL_E(3, USART4_RXD_3_2_E);                       // configure Rx4 line
    #elif defined USART4_ON_2_1
        _CONFIG_PERIPHERAL_E(2, USART4_RXD_2_1_E);                       // configure Rx4 line
    #else
        _CONFIG_PERIPHERAL_E(2, USART4_RXD_2_0_E);                       // configure Rx4 line
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART4, PRIORITY_UART4, (void (*)(void))_USART4_Interrupt);
        break;
#endif
    default:
        return;
    }
    *ulReg = AVR32_RX_ON;                                                // {4} enable the receiver after configuring the receive pin
#ifdef SERIAL_SUPPORT_DMA
    if ((*(ulReg+2) & ENDRX) != 0) {                                     // don't enable receive ready interrupt when in DMA mode
        return;
    }
#endif
#ifdef _WINDOWS
    *(ulReg+4) |= *(ulReg+2);
#endif
    ulDummy = *(ulReg+6);                                                // read the receive holding register to clear any spurious input character (this is possible if the rx was disabled while receiving a character)
    *(ulReg+2) = RXRDY;                                                  // enable interrupts when character received
#ifdef _WINDOWS
    *(ulReg+4) |= *(ulReg+2);                                            // set mask appropriately
#endif
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    unsigned long *ulReg = fnSelectChannel(Channel);
    *ulReg = AVR32_RX_OFF;
    #ifdef _WINDOWS
    *(ulReg + (0x14/sizeof(unsigned long))) &= ~_RX_ENABLED;             // mark that the receiver is disabled for simulator use
    #endif
}

// Enable transmission on the defined channel
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    unsigned long *ulReg = fnSelectChannel(Channel);
    switch (Channel) {
    case 0:
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART0_ON_3_1
        _CONFIG_PERIPHERAL_A(3, USART0_TXD_3_A);                          // configure Tx0 line
        #elif defined USART0_ON_2
        _CONFIG_PERIPHERAL_D(2, USART0_TXD_2_D);                          // configure Tx0 line
        #else
        _CONFIG_PERIPHERAL_E(3, USART0_TXD_3_E);                          // configure Tx0 line
        #endif
    #else
        #if defined USART0_ON_3 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(3, USART0_TX_3_B);                          // configure Tx0 line
        #elif defined USART0_ON_1 && defined _AT32UC3B && !defined CHIP_48_PIN
        _CONFIG_PERIPHERAL_C(1, USART0_TX_1_C);                          // configure Tx0 line
        #else
        _CONFIG_PERIPHERAL_A(0, USART0_TX_0_A);                          // configure Tx0 line
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART0, PRIORITY_UART0, (void (*)(void))_USART0_Interrupt); // set the interrupt handling routine
        break;
#if NUMBER_SERIAL > 1
    case 1:
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART1_ON_3
        _CONFIG_PERIPHERAL_A(3, USART1_TXD_3_A);                          // configure Tx1 line
        #else
        _CONFIG_PERIPHERAL_A(1, USART1_TXD_1_A);                          // configure Tx1 line
        #endif
    #else
        #if defined USART1_ON_3 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(2, USART1_TX_2_B);                          // configure Tx1 line
        #elif defined USART1_ON_1 && defined _AT32UC3B && !defined CHIP_48_PIN
        _CONFIG_PERIPHERAL_C(1, USART1_TX_1_C);                          // configure Tx1 line
        #else
        _CONFIG_PERIPHERAL_A(0, USART1_TX_0_A);                          // configure Tx1 line
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART1, PRIORITY_UART1, (void (*)(void))_USART1_Interrupt); // set the interrupt handling routine
        break;
#endif
#if NUMBER_SERIAL > 2 && (!defined _AT32UC3B || !defined CHIP_48_PIN)
    case 2:
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART2_ON_PC2_3
        _CONFIG_PERIPHERAL_C(2, USART2_TXD_2_0_C);                       // configure Tx2 line
        #else
        _CONFIG_PERIPHERAL_C(2, USART2_TXD_2_1_C);                       // configure Tx2 line
        #endif
    #else
        #ifdef _AT32UC3B                                                 // {3}
            #if defined USART2_ON_PINS_26_27
        _CONFIG_PERIPHERAL_B(0, USART2_TX_0_B);                          // configure Tx2 line
            #else
        _CONFIG_PERIPHERAL_C(0, USART2_TX_0_C);                          // configure Tx2 line
            #endif
        #else
            #if defined USART2_ON_3 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(3, USART2_TX_3_B);                          // configure Tx2 line
            #else
        _CONFIG_PERIPHERAL_A(1, USART2_TX_1_A);                          // configure Tx2 line
            #endif
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART2, PRIORITY_UART2, (void (*)(void))_USART2_Interrupt); // set the interrupt handling routine
        break;
#endif
#if NUMBER_SERIAL > 3
    case 3:
    #ifdef _AT32UC3C                                                     // {14}
        #if defined USART3_ON_2
        _CONFIG_PERIPHERAL_F(2, USART3_TXD_2_F);                         // configure Tx3 line
        #elif defined USART3_ON_3_1
        _CONFIG_PERIPHERAL_B(3, USART3_TXD_3_B);                         // configure Tx3 line
        #else
        _CONFIG_PERIPHERAL_A(3, USART3_TXD_3_A);                         // configure Tx3 line
        #endif
    #else
        #if defined USART3_ON_2 && defined CHIP_144_PIN
        _CONFIG_PERIPHERAL_B(2, USART3_TX_2_B);                          // configure Tx3 line
        #else
        _CONFIG_PERIPHERAL_B(1, USART3_TX_1_B);                          // configure Tx3 line
        #endif
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART3, PRIORITY_UART3, (void (*)(void))_USART3_Interrupt); // set the interrupt handling routine
        break;
#endif
#if NUMBER_SERIAL > 4
    case 4:
    #if defined USART4_ON_3
        _CONFIG_PERIPHERAL_E(3, USART4_TXD_3_0_E);                       // configure Tx4 line
    #elif defined USART4_ON_2_1
        _CONFIG_PERIPHERAL_E(2, USART4_TXD_2_1_E);                       // configure Tx4 line
    #else
        _CONFIG_PERIPHERAL_E(2, USART4_TXD_2_0_E);                       // configure Tx4 line
    #endif
        fnEnterAVRInterrupt(IR_GROUP_USART4, PRIORITY_UART4, (void (*)(void))_USART4_Interrupt); // set the interrupt handling routine
        break;
#endif
    }

    *(ulReg+2) = TXRDY;                                                  // enable interrupts when tx buffer empty (when enabling this will cause an interrupt)
    *ulReg = AVR32_TX_ON;                                                // enable the transmitter

#ifdef _WINDOWS
    switch (Channel) {
    case 0:
        US_CSR_0 |= TXRDY;
        break;
    case 1:
        US_CSR_1 |= TXRDY;
        break;
    case 2:
        US_CSR_2 |= TXRDY;
        break;
    case 3:
        US_CSR_3 |= TXRDY;
        break;
    }
#endif
}

// Disable transmission on the defined channel
//
void fnTxOff(QUEUE_HANDLE Channel)
{
    unsigned long *ulReg = fnSelectChannel(Channel);
    *ulReg = AVR32_TX_OFF;
}

#ifdef UART_BREAK_SUPPORT
extern void fnStartBreak(QUEUE_HANDLE channel)
{
    unsigned long *ulReg = fnSelectChannel(channel);
    *ulReg = AVR32_START_BRK;
    #ifdef _WINDOWS
    fnControlBreak(channel, 1);
    #endif
}

extern void fnStopBreak(QUEUE_HANDLE channel)
{
    unsigned long *ulReg = fnSelectChannel(channel);
    *ulReg = AVR32_STOP_BRK;
    #ifdef _WINDOWS
    fnControlBreak(channel, 0);
    #endif
}
#endif

// The TTY driver uses this call to disable the transmit interrupt of the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE channel)
{
    switch (channel) {
    case 0:
        US_IDR_0 = TXRDY;                                                // disable tx interrupt
    #ifdef _WINDOWS
        US_IMR_0 &= ~(TXRDY);                                            // reflect in status register
    #endif
        break;
#if NUMBER_SERIAL > 1
    case 1:
        US_IDR_1 = TXRDY;                                                // disable tx interrupt
    #ifdef _WINDOWS
        US_IMR_1 &= ~(TXRDY);                                            // reflect in status register
    #endif
        break;
#endif
#if NUMBER_SERIAL > 2
    case 2:
        US_IDR_2 = TXRDY;                                                // disable tx interrupt
    #ifdef _WINDOWS
        US_IMR_2 &= ~(TXRDY);                                            // reflect in status register
    #endif
        break;
#endif
#if NUMBER_SERIAL > 3
    case 3:
        US_IDR_3 = TXRDY;                                                // disable tx interrupt
    #ifdef _WINDOWS
        US_IMR_3 &= ~(TXRDY);                                            // reflect in status register
    #endif
        break;
#endif
    }
}

#ifdef SERIAL_SUPPORT_DMA
// Set up the PDC for reception of the defined character block
//
extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
    /*
    unsigned long *ulReg = (fnSelectChannel(channel) + (PDC_CHANNEL_OFFSET/sizeof(unsigned long)));
    *ulReg++ = (unsigned long)ptrStart;                                  // transfer start location
    *ulReg = (unsigned short)rx_length;                                  // transfer count US_RCR
    ulReg += 7;                                                          // move to US_PTCR
    *ulReg = PDC_RXTEN;                                                  // enable transfer
    ulReg -= ((PDC_CHANNEL_OFFSET/sizeof(unsigned long)) + 6);           // set US_IER
    *ulReg = ENDRX;                                                      // enable interrupt on transfer complete
    #ifdef _WINDOWS
    *(ulReg+2) |= ENDRX;                                                 // set mask register accordingly
    switch (channel) {
    case 0:
        US_PTSR_0 |= PDC_RXTEN;
        break;
    case 1:
        US_PTSR_1 |= PDC_RXTEN;
        break;
    case 2:
        US_PTSR_2 |= PDC_RXTEN;
        break;
    case 3:
        US_PTSR_3 |= PDC_RXTEN;
        break;
    }
    #endif*/
}

// Start transfer of a block via DMA
//
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length)
{
    unsigned long *ulReg = fnSelectChannel(channel);

    UART_PDCA *ptrDMA = (UART_PDCA *)PDCA_BLOCK;
    ptrDMA += channel;
    ptrDMA->DMA_MAR = (unsigned long)ptrStart;                           // the address of the first byte to be transmitted
    ptrDMA->DMA_TCR = tx_length;                                         // the total number of bytes to be transmitted
    ptrDMA->DMA_CR = (DMA_TEN | DMA_ECLR);                               // start transmission (clearing any error flags)

    ulReg += 2;                                                          // set US_IER
    *ulReg = ENDTX;                                                      // enable interrupt on transfer complete
    #ifdef _WINDOWS
    *(ulReg+2) |= ENDTX;                                                 // set mask register accordingly
    *(ulReg+3) &= ~ENDTX;                                                // reset ENDTX in status register
    switch (channel) {
    case 0:
        iDMA |= DMA_CONTROLLER_0;
        break;
    case 1:
        iDMA |= DMA_CONTROLLER_1;
        break;
    case 2:
        iDMA |= DMA_CONTROLLER_2;
        break;
    case 3:
        iDMA |= DMA_CONTROLLER_3;
        break;
    }
    #endif
    return tx_length;
}

    #define SERIAL_SUPPORT_XON_XOFF
extern QUEUE_TRANSFER fnRemainingDMA_tx(QUEUE_HANDLE channel)
{
    return 0;
}
    #endif

extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue)
{
    /*
    unsigned long *ulReg = (fnSelectChannel(channel) + (PDC_CHANNEL_OFFSET/sizeof(unsigned long)));
    *(ulReg + 8) = PDC_TXTDIS;                                           // disable transmitter DMA operation (US_PTCR)
    #ifdef _WINDOWS
    *(ulReg + 8) = 0;
    *(ulReg + 9) &= ~PDC_TXTEN;                                          // disable status in US_PTSR
    #endif
    return (QUEUE_TRANSFER)(*(ulReg + 3));                               // return the aborted length
    */
    return 0;
}


// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {12}
    unsigned long ulTransmitData;
        #ifdef _WINDOWS
    if (ulNextSent[channel] != 0) {
        ulTransmitData = (US_THR_9TH_BIT | ucTxByte);                    // prepare 9th set to '1' in case in 9-bit mode
        ulNextSent[channel] = 0;
    }
    else {
        ulTransmitData = ucTxByte;
    }
        #else                                                            // hardware is big-endian
    if (ucTxByte & 0x01) {
        ulTransmitData = (US_THR_9TH_BIT | ulNextSent[channel]);         // prepare 9th set to '1' in case in 9-bit mode
    }
    else {
        ulTransmitData = ulNextSent[channel];                            // 9th bit is '0'
    }
        #endif
    #endif
    switch (channel) {
    case 0:
        if (US_CSR_0 & TXRDY) {                                          // tx not busy (channel 0)
            US_IER_0 = TXRDY;                                            // ensure tx interrupt enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {12}
        #ifndef _WINDOWS                                                 // target is big-endian
            if (!(US_MR_0 & USART_MODE9)) {                              // if the channel is not in 9 bit mode set the 8 data
                ulTransmitData = ucTxByte;
            }
        #endif 
            US_THR_0 = ulTransmitData;
    #else
            US_THR_0 = ucTxByte;                                         // send byte
    #endif
    #ifdef _WINDOWS
            US_IMR_0 |= TXRDY;
            US_CSR_0 &= ~(TXRDY);
            iInts |= CHANNEL_0_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;
#if NUMBER_SERIAL > 1
    case 1:
        if (US_CSR_1 & TXRDY) {                                          // tx not busy (channel 1)
            US_IER_1 = TXRDY;                                            // ensure tx interrupt enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {12}
        #ifndef _WINDOWS                                                 // target is big-endian
            if (!(US_MR_1 & USART_MODE9)) {                              // if the channel is not in 9 bit mode set the 8 data
                ulTransmitData = ucTxByte;
            }
        #endif
            US_THR_1 = ulTransmitData;
    #else
            US_THR_1 = ucTxByte;                                         // send byte
    #endif
    #ifdef _WINDOWS
            US_IMR_1 |= TXRDY;
            US_CSR_1 &= ~(TXRDY);
            iInts |= CHANNEL_1_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;
#endif
#if NUMBER_SERIAL > 2
    case 2:
        if (US_CSR_2 & TXRDY) {                                          // tx not busy (channel 2)
            US_IER_2 = TXRDY;                                            // ensure tx interrupt enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {12}
        #ifndef _WINDOWS                                                 // target is big-endian
            if (!(US_MR_1 & USART_MODE9)) {                              // if the channel is not in 9 bit mode set the 8 data
                ulTransmitData = ucTxByte;
            }
        #endif
            US_THR_1 = ulTransmitData;
    #else
            US_THR_2 = ucTxByte;                                         // send byte
    #endif
    #ifdef _WINDOWS
            US_IMR_2 |= TXRDY;
            US_CSR_2 &= ~(TXRDY);
            iInts |= CHANNEL_2_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;
#endif
#if NUMBER_SERIAL > 3
    case 3:
        if (US_CSR_3 & TXRDY) {                                          // tx not busy (channel 3)
            US_IER_3 = TXRDY;                                            // ensure tx interrupt enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {12}
        #ifndef _WINDOWS                                                 // target is big-endian
            if (!(US_MR_1 & USART_MODE9)) {                              // if the channel is not in 9 bit mode set the 8 data
                ulTransmitData = ucTxByte;
            }
        #endif
            US_THR_3 = ulTransmitData;
    #else
            US_THR_3 = ucTxByte;                                         // send byte
    #endif
    #ifdef _WINDOWS
            US_IMR_3 |= TXRDY;
            US_CSR_3 &= ~(TXRDY);
            iInts |= CHANNEL_3_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;
#endif
    }
    return 1;                                                            // not sent because we're busy
}

    #ifdef SUPPORT_HW_FLOW

static void fnSetRTS(QUEUE_HANDLE channel, int iState)
{
    switch (channel) {
    case 0:
        if (iState != 0) {
            US_CR_0 = AVR32_RTSEN;                                       // assert RTS
        }
        else {
            US_CR_0 = AVR32_RTSDIS;                                      // negate RTS
        }
        break;
    case 1:
        if (iState != 0) {
            US_CR_1 = AVR32_RTSEN;                                       // assert RTS
        }
        else {
            US_CR_1 = AVR32_RTSDIS;                                      // negate RTS
        }
        break;
    case 2:
        if (iState != 0) {
            US_CR_2 = AVR32_RTSEN;                                       // assert RTS
        }
        else {
            US_CR_2 = AVR32_RTSDIS;                                      // negate RTS
        }
        break;
    case 3:
        if (iState != 0) {
            US_CR_3 = AVR32_RTSEN;                                       // assert RTS
        }
        else {
            US_CR_3 = AVR32_RTSDIS;                                      // negate RTS
        }
        break;
    }
}

// The TTY driver uses this routine to configure and control UART control lines
//
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    if (usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) {
        if (usModifications & CONFIG_RTS_PIN) {
            switch (channel) {
            case 0:
        #if defined USART0_RTS_ON_3 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(3, USART0_RTS_3_B);                 // configure the RTS0 pin for USART0 use - alternative pin
        #else
                _CONFIG_PERIPHERAL_A(0, USART0_RTS_0_A);                 // configure the RTS0 pin for USART0 use
        #endif
                if (usModifications & SET_RS485_MODE) {                  // the AVR32 supports RS485 mode which automatically controls the RTS line for RS485 direction control
                    US_MR_0 = ((US_MR_0 & ~0x7) | USART_MODE_RS485);
                }
                else {
                    fnSetRTS(channel, 0);                                // prepare to drive the RTS line in negated state
                }
                break;
            case 1:
        #if defined USART1_RTS_ON_2 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(2, USART1_RTS_2_B);                 // configure the RTS1 pin for USART1 use - alternative pin
        #elif defined _AT32UC3B
            #if defined USART1_RTS_ON_1 && !defined CHIP_48_PIN
                _CONFIG_PERIPHERAL_A(1, USART1_RTS_1_A);                 // configure the RTS1 pin for USART1 use
            #else
                _CONFIG_PERIPHERAL_C(0, USART1_RTS_0_B);                 // configure the RTS1 pin for USART1 use
            #endif
        #else
                _CONFIG_PERIPHERAL_A(0, USART1_RTS_0_A);                 // configure the RTS1 pin for USART1 use
        #endif
                if (usModifications & SET_RS485_MODE) {                  // the AVR32 supports RS485 mode which automatically controls the RTS line for RS485 direction control
                    US_MR_1 = ((US_MR_1 & ~0x7) | USART_MODE_RS485);
                }
                else {
                    fnSetRTS(channel, 0);                                // prepare to drive the RTS line in negated state
                }
                break;
        #if NUMBER_SERIAL > 2 && (!defined _AT32UC3B || !defined CHIP_48_PIN)
            case 2:
            #if defined USART2_RTS_ON_3 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(3, USART2_RTS_3_B);                 // configure the RTS2 pin for USART2 use - alternative pin
            #elif defined USART2_RTS_ON_1
                _CONFIG_PERIPHERAL_B(1, USART2_RTS_1_B);                 // configure the RTS2 pin for USART2 use - alternative pin
            #elif defined _AT32UC3B
                _CONFIG_PERIPHERAL_C(1, USART2_RTS_1_C);                 // configure the RTS2 pin for USART2 use
            #else
                _CONFIG_PERIPHERAL_B(0, USART2_RTS_0_B);                 // configure the RTS2 pin for USART2 use
            #endif
                if (usModifications & SET_RS485_MODE) {                  // the AVR32 supports RS485 mode which automatically controls the RTS line for RS485 direction control
                    US_MR_2 = ((US_MR_2 & ~0x7) | USART_MODE_RS485);
                }
                else {
                    fnSetRTS(channel, 0);                                // prepare to drive the RTS line in negated state
                }
                break;
        #endif
        #if NUMBER_SERIAL > 3
            case 3:
            #if defined USART3_RTS_ON_2 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(2, USART3_RTS_2_B);                 // configure the RTS3 pin for USART3 use
            #else
                _CONFIG_PERIPHERAL_C(1, USART3_RTS_1_C);                 // configure the RTS3 pin for USART3 use
            #endif
                if (usModifications & SET_RS485_MODE) {                  // the AVR32 supports RS485 mode which automatically controls the RTS line for RS485 direction control
                    US_MR_3 = ((US_MR_3 & ~0x7) | USART_MODE_RS485);
                }
                else {
                    fnSetRTS(channel, 0);                                // prepare to drive the RTS line in negated state
                }
                break;
        #endif
            }
        }
        if (usModifications & CONFIG_CTS_PIN) {
            switch (channel) {
            case 0:
        #if defined USART0_CTS_ON_3 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(3, USART0_CTS_3_B);                 // configure the CTS0 pin for USART0 use - alternative pin
        #else
                _CONFIG_PERIPHERAL_A(0, USART0_CTS_0_A);                 // configure the CTS0 pin for USART0 use
        #endif
                break;
            case 1:
        #if defined USART1_CTS_ON_3 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(2, USART1_CTS_2_B);                 // configure the CTS1 pin for USART1 use - alternative pin
        #elif defined _AT32UC3B
            #if defined USART1_CTS_ON_1 && !defined CHIP_48_PIN
                _CONFIG_PERIPHERAL_A(1, USART1_CTS_0_A);                 // configure the CTS1 pin for USART1 use
            #else
                _CONFIG_PERIPHERAL_C(0, USART1_CTS_0_C);                 // configure the CTS1 pin for USART1 use
            #endif
        #else
                _CONFIG_PERIPHERAL_A(0, USART1_CTS_0_A);                 // configure the CTS1 pin for USART1 use
        #endif
                break;
        #if NUMBER_SERIAL > 2 && (!defined _AT32UC3B || !defined CHIP_48_PIN)
            case 2:
            #if defined USART2_CTS_ON_3 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(3, USART2_CTS_3_B);                 // configure the CTS2 pin for USART2 use - alternative pin
            #elif defined USART2_CTS_ON_1
                _CONFIG_PERIPHERAL_B(1, USART2_CTS_1_B);                 // configure the CTS2 pin for USART2 use - alternative pin
            #elif defined _AT32UC3B
                _CONFIG_PERIPHERAL_C(1, USART2_CTS_1_C);                 // configure the CTS2 pin for USART2 use - alternative pin
            #else
                _CONFIG_PERIPHERAL_B(0, USART2_CTS_0_B);                 // configure the CTS2 pin for USART2 use
            #endif
                break;
        #endif
        #if NUMBER_SERIAL > 3
            case 3:
            #if defined USART3_CTS_ON_2 && defined CHIP_144_PIN
                _CONFIG_PERIPHERAL_B(2, USART3_CTS_2_B);                 // configure the CTS3 pin for USART3 use - alternative pin
            #else
                _CONFIG_PERIPHERAL_C(1, USART3_CTS_1_C);                 // configure the CTS3 pin for USART3 use
            #endif
                break;
        #endif
            }
        }
    }

    if (usModifications & (SET_RTS | SET_CTS)) {                         // SET_CTS may be used in DCE mode and results in the same action as SET_RTS
        fnSetRTS(channel, 1);                                            // drive asserted RTS
    }
    else if (usModifications & (CLEAR_RTS | CLEAR_CTS)) {                // deactivate RTS
        fnSetRTS(channel, 0);                                            // drive negated RTS
    }
}

// Modify control line interrupt operation
//
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    QUEUE_TRANSFER ControlLines = 0;
    if (ENABLE_CTS_CHANGE & usModifications) {
        switch (channel) {
        case 0:
            US_IER_0 = CTSIC;                                            // enable interrupt on input change
        #ifdef _WINDOWS
            US_IER_0 = 0;
            US_IMR_0 |= CTSIC;
        #endif
            break;
        case 1:
            US_IER_1 = CTSIC;                                            // enable interrupt on input change
        #ifdef _WINDOWS
            US_IER_1 = 0;
            US_IMR_1 |= CTSIC;
        #endif
            break;
        case 2:
            US_IER_2 = CTSIC;                                            // enable interrupt on input change
        #ifdef _WINDOWS
            US_IER_2 = 0;
            US_IMR_2 |= CTSIC;
        #endif
            break;
        case 3:
            US_IER_3 = CTSIC;                                            // enable interrupt on input change
        #ifdef _WINDOWS
            US_IER_3 = 0;
            US_IMR_3 |= CTSIC;
        #endif
            break;
        }
    }
    if (DISABLE_CTS_CHANGE & usModifications) {
        switch (channel) {
        case 0:
            US_IDR_0 = CTSIC;                                            // disable interrupt on input change
        #ifdef _WINDOWS
            US_IDR_0 = 0;
            US_IMR_0 &= ~CTSIC;
        #endif
            break;
        case 1:
            US_IDR_1 = CTSIC;                                            // disable interrupt on input change
        #ifdef _WINDOWS
            US_IDR_1 = 0;
            US_IMR_1 &= ~CTSIC;
        #endif
            break;
        case 2:
            US_IDR_2 = CTSIC;                                            // disable interrupt on input change
        #ifdef _WINDOWS
            US_IDR_2 = 0;
            US_IMR_2 &= ~CTSIC;
        #endif
            break;
        case 3:
            US_IDR_3 = CTSIC;                                            // disable interrupt on input change
        #ifdef _WINDOWS
            US_IDR_3 = 0;
            US_IMR_3 &= ~CTSIC;
        #endif
            break;
        }
    }
    switch (channel) {
    case 0:
        if (!(US_CSR_0 & CTS_HIGH)) {
            ControlLines |= SET_CTS;                                     // input is presently asserted
        }
        break;
    case 1:
        if (!(US_CSR_1 & CTS_HIGH)) {
            ControlLines |= SET_CTS;                                     // input is presently asserted
        }
        break;
    case 2:
        if (!(US_CSR_2 & CTS_HIGH)) {
            ControlLines |= SET_CTS;                                     // input is presently asserted
        }
        break;
    case 3:
        if (!(US_CSR_3 & CTS_HIGH)) {
            ControlLines |= SET_CTS;                                     // input is presently asserted
        }
        break;
    }
    return ControlLines;
}
    #endif

#endif                                                                   // end serial UART support



/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // SPI FLASH for SW upload

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_avr32_atmel.h"                               // check the chip(s) availablility
        #include "spi_flash_avr32_stmicro.h"
        #include "spi_flash_avr32_sst25.h"
    #undef _SPI_FLASH_INTERFACE


// Routine to request local SPI FLASH type
//
extern unsigned char fnSPI_Flash_available(void)
{
    return ucSPI_FLASH_Type[0];
}

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
extern unsigned char fnSPI_FlashExt_available(int iExtension)
{
    if (iExtension > SPI_FLASH_DEVICE_COUNT) {
        return 0;
    }
    return ucSPI_FLASH_Type[iExtension];
}
    #endif


#ifdef SPI_FLASH_MULTIPLE_CHIPS
// Calculate the device to access from the absolute address. It is assumed that all devices are operational.
//
static int fnGetSPI_Device(unsigned char **ptrFlash)                     // define the device to be accessed
{
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE)) {
        return 0;
    }
    #ifdef QSPI_CS2_LINE
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;
    }
        #ifdef QSPI_CS3_LINE
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
        return 2;
    }
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 3;
        #else
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 2;
        #endif
    #else
    *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
    return 1;
    #endif

}
#endif

#ifndef SPI_FLASH_SST25
// Return the page number and optionally the address offset in the page
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset, int *iChipSelect)
    #else
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
    #endif
{
    unsigned short usPageNumber;
    #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
    #else
    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address
    #endif
    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    #if (SPI_FLASH_PAGE_LENGTH != 256) && (SPI_FLASH_PAGE_LENGTH != 512) && (SPI_FLASH_PAGE_LENGTH != 1024) // {18} B-device or D-device set to 264 page mode
    usPageNumber = 0;
    while (ptrSector >= (unsigned char *)(SPI_FLASH_PAGE_LENGTH*100)) {  // large hops to get close to the page if at high FLASH address
        usPageNumber += 100;
        ptrSector -= (SPI_FLASH_PAGE_LENGTH*100);
    }
    while (ptrSector >= (unsigned char *)SPI_FLASH_PAGE_LENGTH) {        // page sizes to reach actual page
        usPageNumber++;
        ptrSector -= SPI_FLASH_PAGE_LENGTH;
    }
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector); // remainder is offset in the page
    }
    #else
    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    #endif
    return usPageNumber;
}
#endif

#elif defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
extern QUEUE_HANDLE SPI_handle;                                          // EEPROM commands
#define M95XXX_WRITE_ENABLE   0x06                                       // send this prior to write or write status register command
#define M95XXX_WRITE_DISABLE  0x04
#define M95XXX_READ_STAT_REG  0x05
#define M95XXX_WRITE_STAT_REG 0x01
#define M95XXX_READ_MEMORY    0x03
#define M95XXX_WRITE_MEMORY   0x02

static unsigned char ucEEPROMDanger = 0;

int fnWriteBytesEEPROM(unsigned char *ucDestination, unsigned char *ucData, unsigned short usLength)
{
    unsigned char usByteWrite[4];                                        // our file sytem is in external EEPROM via SPI bus.
    usByteWrite[0] = M95XXX_WRITE_MEMORY;
    usByteWrite[1] = (unsigned char)((CAST_POINTER_ARITHMETIC)(ucDestination)>> 8);
    usByteWrite[2] = (unsigned char)((CAST_POINTER_ARITHMETIC)ucDestination);
    usByteWrite[3] = PREPARE_PAGE_WRITE;
#if defined SPI_INTERFACE
    fnWrite(SPI_handle, usByteWrite, sizeof(usByteWrite));               // set the byte write address
    fnWrite(SPI_handle, ucData, usLength);                               // write the data
#else
    fnSendSPIMessage(usByteWrite, sizeof(usByteWrite));                  // send immediately
    fnSendSPIMessage(ucData, usLength);                                  // write the data immediately
#endif
    ucEEPROMDanger = 1;                                                  // we should avoid reading until we know that the write has terminated
    return 0;
}


// SPI reserved exclusively for a EEPROM file system. Write byte via SPI
//
void fnSendSPIMessage(unsigned char *ptrData, QUEUE_TRANSFER Length)     // master transmission
{
    volatile unsigned long ulDummy;                                      // dummy variable for clearing flags
    unsigned long ulCS_state = (CS_PORT & CS_OUTPUT);                    // check the state of the CS line on entry
    int iDelete = (ptrData == 0);                                        // a page delete has been commanded

    if (ulCS_state) {                                                    // If this is the first entry it has a control byte at the end
        Length--;                                                        // reduce the count so we don't send the control byte
        if (Length) {
            CS_PORT_CLR = CS_OUTPUT;                    _SIM_PORT_CHANGE // assert CS
        }
    }

    ulDummy = SPI_RDR_0;                                                 // reset receive data flag with dummy read - the rx data is not interesting here

    while (Length--) {                                                   // for each byte we want to send
        if (iDelete) {
            SPI_TDR = 0xff;
        }
        else {
            SPI_TDR = (*ptrData++);                                      // start byte transmission
        }
#ifdef _WINDOWS
        fnSimM95xxx(M95XXX_WRITE, (unsigned char)SPI_TDR, (unsigned char)((CS_PORT & CS_OUTPUT) != 0)); // simulate the SPI EEPROM device
        SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                              // simulate tx and rx interrupt flags being set
#endif
        while (!(SPI_SR & SPI_RDRF)) {};                                 // wait until tx byte has been sent and rx byte has been completely received
        ulDummy = SPI_RDR;                                               // reset receive data flag with dummy read - the rx data is not interesting here
    }

    if ((!ulCS_state || (PREPARE_READ != *(ptrData)))) {
        CS_PORT_SET = CS_OUTPUT;                        _SIM_PORT_CHANGE // deassert SS on exit
    }
#ifdef _WINDOWS
    fnSimM95xxx(M95XXX_CHECK_SS, 0, (unsigned char)((CS_PORT & CS_OUTPUT) != 0)); // simulate the SPI EEPROM device
#endif
}


// We use this in master mode to get the next SPI byte as fast as possible
//
void fnGetSPI_bytes(unsigned char *ptrBuffer, MAX_FILE_LENGTH Length)
{                                                                        // master reception - we assume SSEL asserted before entry (commanded previously)
    while (!(SPI_SR & SPI_TXEMPTY)) {};                                  // wait until free - should be very short wait

    while (Length--) {
        SPI_TDR = (0xff);                                                // send a dummy byte to cause reception transfer
#ifdef _WINDOWS
        SPI_RDR = fnSimM95xxx(M95XXX_READ, (unsigned char)SPI_TDR, (unsigned char)((CS_PORT & CS_OUTPUT) != 0));
        SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                              // simulate tx and rx interrupt flags being set
#endif
        while (!(SPI_SR & SPI_RDRF)) {};                                 // wait for the byte to be transmitted / received (check rx)
        *ptrBuffer++ = (unsigned char)SPI_RDR;                           // copy the received byte to the input buffer
#ifdef _WINDOWS
        SPI_SR &= ~(SPI_RDRF);
#endif
    }

    CS_PORT_SET = CS_OUTPUT;;                          _SIM_PORT_CHANGE  // always deassert CS on exit

#ifdef _WINDOWS
    fnSimM95xxx(M95XXX_CHECK_SS, 0, (unsigned char)((CS_PORT & CS_OUTPUT) != 0));
#endif
}

// Wait until any outstanding write has terminated
//
static void fnWaitWriteComplete(void)
{
    #define WIP 0x01
    static const unsigned char ucRead[] = {M95XXX_READ_STAT_REG, PREPARE_READ}; // read from status register
    unsigned char ucStatus;

    if (!ucEEPROMDanger) return;                                         // no point in checking since we are sure no write is in progress

    do {
 #if defined SPI_INTERFACE
        fnWrite(SPI_handle, (unsigned char *)ucRead, sizeof(ucRead));
        fnRead(SPI_handle, &ucStatus, 1);                                // read the present status
 #else
        fnSendSPIMessage((unsigned char *)ucRead, sizeof(ucRead));
        fnGetSPI_bytes(&ucStatus, 1);
 #endif
    } while (ucStatus & WIP);                                            // wait until an in progress write is complete
    ucEEPROMDanger = 0;                                                  // mark eeprom not writimg so no danger reading data from it
 #ifdef _WINDOWS
    fnEEPROM_safe();
 #endif
}
#endif                                                                   // endif SPI_FILE_SYSTEM

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
static void fnConfigSPIFileSystem(unsigned char ucSpeed)
{
    CONFIG_SPI_PORTS();
    POWER_SPI();                                                         // enable clocks to SPI in PMC

    SPI_MR = (SPI_MSTR | MODFDIS | SPI_CS_0);                            // master mode with no clock divider and control defined by CS0 configuration

    switch (ucSpeed) {
    case MAX_SPI:                                                        // set maximum possible SPI speed
	    SPI_CSR0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 16000000/2)/16000000)<<8)); // 16MHz
	    break;

    case SPI_2MEG:
	    SPI_CSR0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 2000000/2)/2000000)<<8));
	    break;

    case SPI_SLAVE:
	    return;
    }
    SPI_CR = SPIEN;                                                      // enable SPI
}
#endif


#ifdef FLASH_FILE_SYSTEM

// This routine is used to command FLASH - we put it in SRAM
//
static void fnFlashFlash(unsigned long ulCommand)
{
    FC_CMD = ulCommand;                                                  // execute command
    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH ready again
    #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
    #endif
    }
    #ifdef FLASH_ERRATA_WORKAROUND                                       // it is assumed that code is always executed from the first half of FLASH and so only operations in the second half need to be worked around
    if (((ulCommand >> 8) & 0x0000ffff) >= ((256*1024)/FLASH_GRANULARITY)) { // an access in the second half of 512k FLASH
        ulCommand &= 0x000000ff;                                         // mask the command
        if ((ulCommand == FCMD_EP) || (ulCommand == FCMD_WP)) {          // perform workaround in these cases
            unsigned long *ptrFlash = (unsigned long *)FLASH_LOW_BLOCK_ZERO;
            volatile unsigned long ulDummy;
            do {
                ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from lower half of FLASH (page is zeroed)
            } while (ptrFlash < (unsigned long *)(FLASH_LOW_BLOCK_ZERO + FLASH_GRANULARITY));
            ptrFlash = (unsigned long *)FLASH_LOW_BLOCK_ZERO;            // repeat
            do {
                ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from lower half of FLASH (page is zeroed)
            } while (ptrFlash < (unsigned long *)(FLASH_LOW_BLOCK_ZERO + FLASH_GRANULARITY));
            ptrFlash = (unsigned long *)FLASH_HIGH_BLOCK_ZERO;           // same for high block
            do {
                ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from higher half of FLASH (page is zeroed)
            } while (ptrFlash < (unsigned long *)(FLASH_HIGH_BLOCK_ZERO + FLASH_GRANULARITY));
            ptrFlash = (unsigned long *)FLASH_HIGH_BLOCK_ZERO;           // repeat
            do {
                ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from higher half of FLASH (page is zeroed)
            } while (ptrFlash < (unsigned long *)(FLASH_HIGH_BLOCK_ZERO + FLASH_GRANULARITY));
        }
    }
    #endif
}

// Cover routine for FLASH command execution
//
static void fnCommandFlash(unsigned long ulCommand)
{
#ifdef FLASH_ERRATA_WORKAROUND
    #define PROG_WORD_SIZE 150                                           // adequate space for the small program
#else
    #define PROG_WORD_SIZE 40                                            // adequate space for the small program
#endif

    static unsigned long ulProgSpace[PROG_WORD_SIZE];                    // make space for the routine (this will have a word boundary)
    static void (*fnRAM_code)(unsigned long) = 0;

    if (!fnRAM_code) {                                                   // real address of code
        uMemcpy(ulProgSpace, (void const *)fnFlashFlash, sizeof(ulProgSpace));
        fnRAM_code = (void(*)(unsigned long))(ulProgSpace);
    }

    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH is ready for programming
    #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
    #endif
    }

    uDisable_Interrupt();                                                // protect this region from interrupts
    #ifdef _WINDOWS
    fnFlashFlash(ulCommand);
    #else
    fnRAM_code(ulCommand);                                               // execute code from SRAM
    #endif
    uEnable_Interrupt();                                                 // safe to accept interrupts again
}
#endif


#ifdef ACTIVE_FILE_SYSTEM
    #if defined SPI_FLASH_SST25
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination, int *iChipSelect)
        #else
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
        #endif
{
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    ucDestination -= (SPI_FLASH_START);                                  // location relative to the start of the SPI FLASH chip address
        #else
    ucDestination -= (uFILE_START + FILE_SYSTEM_SIZE);                   // location relative to the start of the SPI FLASH chip address
        #endif
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
        #endif
    return ucDestination;
}
    #endif                                                               // end SST25

// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
    #ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
    #endif
#ifdef MANAGED_FILES                                                     // {8}
    MAX_FILE_LENGTH OriginalLength = Length;
#endif
    MAX_FILE_LENGTH BlockLength = FLASH_GRANULARITY;
    do {
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
        static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
        unsigned short ucDeleted = 0;
        unsigned short usTemp = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector);

        usTemp &= ~(EEPROM_PAGE_SIZE-1);                                 // we want to erase a block (program them to 0xff)
        Length += ((unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector) - usTemp);
        ptrSector = (unsigned char *)usTemp;
        while (ucDeleted < FLASH_GRANULARITY) {
            fnWaitWriteComplete();                                       // wait until free to write
        #if defined SPI_INTERFACE
            fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrSector, ucDel, EEPROM_PAGE_SIZE);      // delete a page
        #else
            fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrSector, 0, EEPROM_PAGE_SIZE);          // delete a page
        #endif
            ucDeleted += EEPROM_PAGE_SIZE;
            ptrSector += EEPROM_PAGE_SIZE;
        }
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    #else                                                                // end SPI_FILE_SYSTEM
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)))
            #else
        if ((ptrSector >= uFILE_SYSTEM_END) && (ptrSector < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
            #endif
        {                                                                // we are working from external SPI FLASH memory
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            #if !defined SPI_FLASH_ST
            unsigned char  ucCommand;
            #endif
            #ifdef SPI_FLASH_SST25
            unsigned long ulSectorAlign;
            unsigned long ulLocation = (CAST_POINTER_ARITHMETIC)fnGetSPI_FLASH_address(ptrSector EXTENDED_CS);   // convert to virtual SPI Flash memory location
            ulSectorAlign = (ulLocation & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1));
            Length += (ulLocation - ulSectorAlign);
            ptrSector -= (ulLocation - ulSectorAlign);
            #else
            unsigned short usPageNumber, usPageOffset;
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset EXTENDED_CS);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page
            #endif
            #if defined SPI_FLASH_ST
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // enable the write
                #ifdef SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
                #ifdef MANAGED_FILES                                     // {8}
            BlockLength = SPI_FLASH_SECTOR_LENGTH;
                #endif
            #elif defined SPI_FLASH_SST25
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
                #ifndef SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && (ulSectorAlign == (ulLocation & (SPI_FLASH_SECTOR_LENGTH - 1)))) { // if a complete 64k sector can be deleted
                ucCommand = SECTOR_ERASE;                                // delete block of 64k
                BlockLength = SPI_FLASH_SECTOR_LENGTH;
            }
            else
                #endif
            if ((Length >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
                ucCommand = HALF_SECTOR_ERASE;                           // delete block of 32k
                BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
            }
            else {
                ucCommand = SUB_SECTOR_ERASE;                            // delete smallest sector of 4k
                BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
            }
            fnSPI_command(ucCommand, ulLocation, _EXTENDED_CS 0, 0);
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #else
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && ((usPageNumber % 8) == 0)) { // if delete range corresponds to a block, use faster block delete
                BlockLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                BlockLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);   // delete appropriate page/block
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #endif
            continue;
        }
        #endif
        {                                                                // delete in internal FLASH
            unsigned long ulCommand = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // set to sector boundary
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)));
            ulCommand -= FLASH_START_ADDRESS;
            ulCommand /= FLASH_GRANULARITY;
            ulCommand <<= 8;                                             // page to be deleted
            ulCommand |= (FLASH_KEY | FCMD_EP);
        #ifdef RETRIG_WDOG_DURING_ERASE                                  // when a large amount of pages are erased at one time it can require a long time (512 pages - 374k file system - was measured at about 3s). This option retriggers the watchdog during this period.
            WDT_CLR = 0x0;                                               // retrigger the watchdog by writing any value to it
        #endif
            fnCommandFlash(ulCommand);                                   // command individual page erase
        #ifdef _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion
        #endif
            if (FC_FSR & (LOCKE | PROGE)) {
                return 1;                                                // error
            }
            ptrSector += FLASH_GRANULARITY;
            if (Length <= FLASH_GRANULARITY) {
                break;
            }
            Length -= FLASH_GRANULARITY;
        }
    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);

    #ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
    #endif
#ifdef MANAGED_FILES                                                     // {123}
    if (OriginalLength == 0) {                                           // if a single page erase was called, return the page size
	    return BlockLength;
	}
#endif
    return 0;
}


    #ifdef FLASH_FILE_SYSTEM
// Commit the present write buffer to FLASH
//
static int fnCommitFLASH(unsigned long ulPage)
{
    fnCommandFlash(FLASH_KEY | FCMD_WP | (ulPage << 8));
    return (FC_FSR & (LOCKE | PROGE));                                   // zero when no error
}

extern void fnProtectFlash(unsigned char *ptrSector, unsigned char ucProtection)
{
    unsigned long ulPage = (((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS)/FLASH_GRANULARITY);
    if (ucProtection == PROTECT_SECTOR) {
        fnCommandFlash(FLASH_KEY | FCMD_LP | (ulPage << 8));
        #ifdef _WINDOWS
        FC_FSR &= ~(0x1 << ((ulPage/(LOCK_REGION_SIZE/FLASH_GRANULARITY)) + 16));
        #endif
    }
    else {                                                               // unprotect sector
        fnCommandFlash(FLASH_KEY | FCMD_UP | (ulPage << 8));
        #ifdef _WINDOWS
        FC_FSR &= ~(0x1 << ((ulPage/(LOCK_REGION_SIZE/FLASH_GRANULARITY)) + 16));
        #endif
    }
}
    #endif


#ifdef _WINDOWS
extern void uFlashMemcpy_long(unsigned long *ptrTo, const unsigned long *ptrFrom, size_t Size)
{
    Size /= sizeof(unsigned long);
    while (Size-- != 0) {
        *ptrTo &= *ptrFrom++;                                            // and the result with present FLASH content
        ptrTo++;
    }
}
#else
    #define uFlashMemcpy_long  uMemcpy_long
#endif

// Write a buffer to FLASH.
// The AVR32 does not support multiple writes to a single long word, so we collect data to be saved to a block of FLASH_ROW_SIZE bytes
// until a virtual block boundary is crossed or else a closing command is received. The larger the size of block (up to the flash granularity size)
// the less FLASH writes are performed but the larger the block buffer needs to be
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    #ifdef FLASH_FILE_SYSTEM
    static unsigned char *ptrOpenBuffer = 0;
    unsigned long ulBufferOffset; // = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1)); {5}
    unsigned char *ptrFlashBuffer;
    MAX_FILE_LENGTH BufferCopyLength;
    int iRtn = 0;
    #endif

    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)))
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
        #endif
    {                                                                    // we are working from external SPI FLASH memory
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        #if defined SPI_FLASH_SST25
        int iMultipleWrites = 0;
        #endif
        unsigned short usDataLength;
        #if defined SPI_FLASH_SST25
            #ifdef MANAGED_FILES                                         // {11}
        unsigned char ucStatus = 10;                                     // allow this many polling trials
        ucDestination = fnGetSPI_FLASH_address(ucDestination EXTENDED_CS); // convert to virtual SPI Flash memory location
        fnSPI_command(CHECK_SPI_FLASH_BUSY, 0, _EXTENDED_CS &ucStatus, 1);
        if (ucStatus == 0) {                                             // device still busy so quit for the moment
            return MEDIA_BUSY;
        }
        else if (Length == 0) {                                          // new check on state called
            return 0;
        }

            #else
        if (Length == 0) {
            return 0;                                                    // ignore if length is zero
        }
        ucDestination = fnGetSPI_FLASH_address(ucDestination EXTENDED_CS); // convert to virtual SPI Flash memory location
            #endif
        if ((CAST_POINTER_ARITHMETIC)ucDestination & 0x1) {              // start at odd SPI address, requires an initial byte write
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
            fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, 1); // program last byte
            if (--Length == 0) {                                         // single byte write so complete
                return 0;
            }
            ucDestination++;
            ucData++;
        }
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // command write enable to allow programming
        #else                                                            // ST or ATMEL
        unsigned short usPageNumber;
        unsigned short usPageOffset;
            #ifdef MANAGED_FILES                                         // {8}
        unsigned char ucStatus = 10;                                     // allow this many polling trials
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
        fnSPI_command(CHECK_SPI_FLASH_BUSY, 0, _EXTENDED_CS &ucStatus, 1);
        if (ucStatus == 0) {                                             // device still busy so quit for the moment
            return MEDIA_BUSY;
        }
        else if (ucData == 0) {                                          // new check on state called
            return 0;
        }
            #else
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
            #endif
        #endif
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
        #if defined SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #elif defined SPI_FLASH_SST25
            #ifdef SST25_A_VERSION
            #define AAI_LENGTH 1
            #else
            #define AAI_LENGTH 2
            #endif
            if (usDataLength > 1) {
                if (iMultipleWrites == 0) {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                    iMultipleWrites = 1;                                 // mark that we are in a AAI sequence
                }
                else {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
                }
                ucDestination += AAI_LENGTH;
                Length -= AAI_LENGTH;
                ucData += AAI_LENGTH;
            }
            else {
                if (iMultipleWrites != 0) {
                    fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);  // first close AAI sequence
                    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);   // command write enable to allow byte programming
                }
                fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, 1); // program last byte
                break;
            }
        #else
            if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) { // are we writing a partial page?
                fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
            }
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #endif
        }
        #if defined SPI_FLASH_SST25
        fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);              // disable writes
        #endif
        return 0;
    }
    #endif
    #ifdef FLASH_FILE_SYSTEM
    if (ucData == 0) {                                                   // close an open buffer
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & (FLASH_ROW_SIZE-1));
        if (ulBufferOffset == 0) {
            return 0;                                                    // no open buffer so nothing to do {5}
        }
        ulBufferOffset = FLASH_ROW_SIZE;                                 // cause the open buffer to be saved without copying any input data
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & ~(FLASH_ROW_SIZE-1));
    }
    else {
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ucDestination & ~(FLASH_ROW_SIZE-1));
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1));
    }
    do {
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {
            BufferCopyLength = Length;
        }
        ptrFlashBuffer = (unsigned char *)ulFlashRow + ulBufferOffset;   // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= ((unsigned char *)ulFlashRow + FLASH_ROW_SIZE)) { // a complete backup buffer is ready to be copied to FLASH
//            if (ptrOpenBuffer == (unsigned char *)0x80047000) {           {5}
//              uFlashMemcpy_long(ulFlashRow, ulFlashRow, FLASH_ROW_SIZE);
//          }
            fnCommandFlash(FLASH_KEY | FCMD_CPB);                        // clear the Flash page buffer
            uFlashMemcpy_long((unsigned long *)fnGetFlashAdd(ptrOpenBuffer), ulFlashRow, FLASH_ROW_SIZE); // copy the data to the Flash page using long word accesses
            ptrFlashBuffer = (unsigned char *)ulFlashRow;                // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            iRtn |= fnCommitFLASH(((CAST_POINTER_ARITHMETIC)ptrOpenBuffer - FLASH_START_ADDRESS)/FLASH_GRANULARITY); // commit the page buffer to Flash
        #ifdef CHECK_FLASH_WRITE                                         // generally used only during debugging to verify that the written FLASH content equals the expected contents in FLASH
            if (uMemcmp(ulFlashRow, fnGetFlashAdd(ptrOpenBuffer), FLASH_ROW_SIZE) != 0) {
                iRtn = -1;                                               // mark that an error was identified
            }
        #endif
            ptrOpenBuffer += FLASH_ROW_SIZE;
            uMemset_long(ulFlashRow, 0xffffffff, FLASH_ROW_SIZE);        // flush the intermediate buffer
        }
        else {                                                           // {5} incomplete buffer collected
            ptrOpenBuffer += BufferCopyLength;
        }
    } while (Length != 0);
    #endif
    return iRtn;
}
#endif

#if (defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES) && defined _WINDOWS
    extern int iFetchingInternalMemory = 0;
#endif

// Retrieve file contents from FLASH
//
extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef ACTIVE_FILE_SYSTEM
    #ifdef _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
    #endif
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    unsigned char usRead[4];                                             // our file sytem is in external EEPROM via SPI bus.
    usRead[0] = M95XXX_READ_MEMORY;
    usRead[1] = (unsigned char)((CAST_POINTER_ARITHMETIC)(ParLocation)>> 8);
    usRead[2] = (unsigned char)((CAST_POINTER_ARITHMETIC)ParLocation);
    usRead[3] = PREPARE_READ;
    fnWaitWriteComplete();                                               // ensure the chip is ready to be read
        #if defined SPI_INTERFACE
    fnWrite(SPI_handle, usRead, sizeof(usRead));                         // set the read address
    fnRead(SPI_handle, ptrValue, usSize);                                // read from the device to the return buffer
        #else
    fnSendSPIMessage(usRead, sizeof(usRead));                            // send immediately
    fnGetSPI_bytes(ptrValue, Size);                                      // fetch immediately
        #endif
    #else
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ParLocation >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
            #else
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
            #endif
    {                                                                    // we are working from external SPI FLASH memory
            #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
            #endif
            #ifdef SPI_FLASH_SST25
        ParLocation = fnGetSPI_FLASH_address(ParLocation EXTENDED_CS);
            #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
            #endif
            #if defined SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #elif defined SPI_FLASH_SST25
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, _EXTENDED_CS ptrValue, Size);
            #else
                #if SPI_FLASH_PAGE_LENGTH >= 1024                        // {7}
                    #if SPI_FLASH_PAGE_LENGTH == 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #elif SPI_FLASH_PAGE_LENGTH >= 512
                    #if SPI_FLASH_PAGE_LENGTH == 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9)  | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #else
                    #if SPI_FLASH_PAGE_LENGTH == 256
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #endif
            #endif
        return;
    }
        #endif
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the AVR32 uses a file system in FLASH with no access restrictions so we can simply copy the data
    #endif
#endif
}



#ifdef USE_PARAMETER_BLOCK

#define PARAMETER_BLOCK_1 (unsigned long *)(PARAMETER_BLOCK_START)
#define PARAMETER_BLOCK_2 (unsigned long *)(PARAMETER_BLOCK_START + PARAMETER_BLOCK_SIZE)


// The AVR32 has 512 byte blocks which can be individually modified on a long word basis
//
extern unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned long ulValidUse[2];
    unsigned long ulCompare;

    fnGetParsFile((unsigned char *)PARAMETER_BLOCK_1, (unsigned char *)ulValidUse, sizeof(ulValidUse)); // the first 2 long word locations define the validity of the block. 0x55555555 0x55555555 means it is fully valid. 0x55555555 0xffffffff means it contains temporary unvalidated data

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ulCompare = 0x55555555;                                          // we are looking for validated pattern
    }
    else {
        ulCompare = 0xffffffff;                                          // we are looking for temporary pattern
    }

    if ((ulValidUse[0] == 0x55555555) && (ulValidUse[1] == ulCompare)) {
        return 1;                                                        // first block matched request
    }
    #ifdef USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile((unsigned char *)PARAMETER_BLOCK_2, (unsigned char *)ulValidUse, sizeof(ulValidUse));
        if ((ulValidUse[0] == 0x55555555) && (ulValidUse[1] == ulCompare)) {
            return 2;                                                    // second block matched request
        }
    }
    #endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}

// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(unsigned char *ptrAddInPar)
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}

// Retrieve parameters from the specified parameter block
//
extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned long *ptrPar = PARAMETER_BLOCK_1;

    #ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_SIZE/sizeof(unsigned long));
    }
    #endif

    ptrPar += (usParameterReference);
    ptrPar += 2;                                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile((unsigned char *)ptrPar++, ucValue++, 1);
    }
    return 0;
}

// Write parameters to the defined block
//
extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned long *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned long ulValid = 0x55555555;
    int iBlockUse = 0;
    int iCommitNewStatus = 0;
    unsigned long ulValidCheck;
    unsigned long ulCheck;

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {                                  // no valid parameter blocks have been found so we can use the first for saving the data.
        fnDeleteParBlock((unsigned char *)PARAMETER_BLOCK_1);            // we delete it to be sure it is fresh
    #ifdef USE_PAR_SWAP_BLOCK
        fnDeleteParBlock((unsigned char *)PARAMETER_BLOCK_2);
    #endif
        ucValidBlock = 1;
    }

    #ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_SIZE/sizeof(unsigned long));
    }
    #endif
    ptrStart = ptrPar;
    ptrPar += (usParameterReference);
    ptrPar += 2;                                                         // first parameter starts after validation information

    if (!usLength) {                                                     // command rather than normal write
        if (iBlockUse) {                                                 // if we have a new set waiting, set it to use state
            iBlockUse = 0;
    #ifdef USE_PAR_SWAP_BLOCK
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock((unsigned char *)ptrPar);
    #endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock((unsigned char *)ptrStart);
            if (usParameterReference) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
        while (usLength--) {
#if defined _WINDOWS || defined _LITTLE_ENDIAN
            ulCheck = (0xffffff00 | *ucValue++);
#else
            ulCheck = (0x00ffffff | (*ucValue++ << 24));
#endif
            fnWriteBytesFlash((unsigned char *)ptrPar++, (unsigned char *)&ulCheck, sizeof(ulCheck)); // write each parameter to FLASH (each byte occupies a long word)
        }
        if (((CAST_POINTER_ARITHMETIC)ptrPar & (FLASH_ROW_SIZE-1)) != 0) {
            if (ptrPar > (ptrStart + (FLASH_ROW_SIZE/sizeof(unsigned long)))) {
                fnWriteBytesFlash(0, 0, 0);                              // commit any open FLASH row buffer
            }
        }
    }
    fnGetParsFile((unsigned char *)ptrStart, (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
    if (ulValidCheck != ulValid) {
        fnWriteBytesFlash((unsigned char *)ptrStart, (unsigned char *)&ulValid, sizeof(ulValid)); // validate data
        iCommitNewStatus = 1;
    }
    if (!iBlockUse) {
        fnGetParsFile((unsigned char *)(ptrStart + 1), (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
        if (ulValidCheck != ulValid) {
            fnWriteBytesFlash((unsigned char *)(ptrStart + 1), (unsigned char *)&ulValid, sizeof(ulValid)); // use data
            iCommitNewStatus = 1;
        }
    }
    if (iCommitNewStatus != 0) {
        fnWriteBytesFlash(0, 0, 0);                                      // commit the parameter block status
    }
    return 0;
}
#endif



#if defined I2C_INTERFACE
static __interrupt void _TWI_Interrupt(void)
{
    volatile unsigned long ulStatus = TWI_SR;                            // clear the interrupt flag

    if (I2C_tx_control[0]->ucState & RX_ACTIVE) {
        if (I2C_tx_control[0]->ucPresentLen != 0) {
            I2C_tx_control[0]->ucPresentLen--;
            if (I2C_tx_control[0]->ucPresentLen == 0) {
                TWI_CR = (TWI_MSEN | TWI_STOP);                          // send stop condition so that last byte is not acknowledged
            }
            *I2C_rx_control[0]->I2C_queue.put++ = (unsigned char)TWI_RHR;// read the byte
            I2C_rx_control[0]->I2C_queue.chars++;                        // and put it into the rx buffer
            if (I2C_rx_control[0]->I2C_queue.put >= I2C_rx_control[0]->I2C_queue.buffer_end) {
                I2C_rx_control[0]->I2C_queue.put = I2C_rx_control[0]->I2C_queue.QUEbuffer;
            }
    #if defined _WINDOWS
            if (I2C_tx_control[0]->ucPresentLen != 0) {
                TWI_RHR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)TWI_RHR);// simulate the interrupt directly
            }
            TWI_SR |= TWI_RXRDY;
            iInts |= I2C_INT0;
    #endif
        }
        else {
            ulStatus = TWI_RHR;                                          // dummy read after stop condition
            TWI_IDR = (TWI_TXRDY | TWI_RXRDY);                           // disable interrupts
    #ifdef _WINDOWS
            TWI_IMR &= ~TWI_IDR; TWI_IDR = 0;                            // simulate the interrupt mask setting
    #endif
            I2C_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            I2C_rx_control[0]->msgs++;
            if (I2C_rx_control[0]->wake_task != 0) {                     // wake up the receiver task if desired
               uTaskerStateChange(I2C_rx_control[0]->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        return;
    }

    if (I2C_tx_control[0]->ucPresentLen-- != 0) {                        //  send next byte if available
        TWI_THR = *I2C_tx_control[0]->I2C_queue.get++;
        if (I2C_tx_control[0]->I2C_queue.get >= I2C_tx_control[0]->I2C_queue.buffer_end) {
            I2C_tx_control[0]->I2C_queue.get = I2C_tx_control[0]->I2C_queue.QUEbuffer; // handle the ring buffer
        }
    #if defined _WINDOWS
        TWI_SR |= TWI_TXRDY;                                             // simulate the interrupt directly
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)TWI_THR);
        iInts |= I2C_INT0;
    #endif
    }
    else {
        TWI_IDR = (TWI_TXRDY | TWI_RXRDY);                               // disable interrupts
    #ifdef _WINDOWS
        TWI_IMR &= ~TWI_IDR; TWI_IDR = 0;                                // simulate the interrupt mask setting
    #endif
        if (!(TWI_IMR & TWI_TXCOMP)) {
            TWI_IER = (TWI_TXCOMP);                                      // enable complete interrupt
    #if defined _WINDOWS
            TWI_IMR |= TWI_IER; TWI_IER = 0;                             // simulate the interrupt mask setting
            TWI_SR = TWI_TXCOMP;
            iInts |= I2C_INT0;
    #endif
            I2C_tx_control[0]->ucPresentLen++;
            TWI_CR = (TWI_MSEN | TWI_STOP);                              // send stop condition
        }
        else {                                                           // waiting for stop to terminate
            TWI_IDR = (TWI_TXCOMP);                                      // disable interrupts
    #if defined _WINDOWS
            TWI_IMR &= ~TWI_IDR; TWI_IDR = 0;                            // simulate the interrupt mask setting
    #endif
            if (I2C_tx_control[0]->I2C_queue.chars == 0) {               // all transmission complete
                I2C_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);

                if (I2C_tx_control[0]->wake_task != 0) {
                   uTaskerStateChange(I2C_tx_control[0]->wake_task, UTASKER_ACTIVATE); // wake up owner task since the transmission has terminated
                }
            }
            else {
                fnTxI2C(I2C_tx_control[0], 0);                           // we have another message to send so we can send a repeated start condition
            }
        }
    }
}

// Configure the I2C hardware
//
extern void fnConfigI2C(I2CTABLE *pars)
{
    POWER_UP(PBA, PBAMASK_TWI);                                          // enable clocks to TWI
    TWI_CR = (TWI_SWRST);                                                // reset I2C module
    _CONFIG_PERIPHERAL_A(0, (TWI_SDA_A | TWI_SCL_A));                    // configure TWI port pins - these are automatically open-drain with slew-rate limited output and with glitch-filter on input
    fnEnterAVRInterrupt(IR_GROUP_TWI, PRIORITY_TWI, (void (*)(void))_TWI_Interrupt);     // set the interrupt handling routine

    if (pars->usSpeed >= 400) {                                          // maximum speed 400kHz
        #define TWI_SPEED_400 400000
        #define TWI_CKDIV_400 0
        TWI_CWGR = ((((PBA_CLOCK/TWI_SPEED_400/2) - 3)/(1 << TWI_CKDIV_400)) | ((((PBA_CLOCK/TWI_SPEED_400/2) - 3)/(1 << TWI_CKDIV_400)) << 8) | (TWI_CKDIV_400 << 16)); // 400kHz
    }
    else if (pars->usSpeed < 100) {                                      // 50kHz
        #define TWI_SPEED_50 50000
        #define TWI_CKDIV_50 3
        TWI_CWGR = ((((PBA_CLOCK/TWI_SPEED_50/2) - 3)/(1 << TWI_CKDIV_50)) | ((((PBA_CLOCK/TWI_SPEED_50/2) - 3)/(1 << TWI_CKDIV_50)) << 8) | (TWI_CKDIV_50 << 16)); // 50kHz
    }
    else {                                                               // 100kHz
        #define TWI_SPEED_100 100000
        #define TWI_CKDIV_100 2
        TWI_CWGR = ((((PBA_CLOCK/TWI_SPEED_100/2) - 3)/(1 << TWI_CKDIV_100)) | ((((PBA_CLOCK/TWI_SPEED_100/2) - 3)/(1 << TWI_CKDIV_100)) << 8) | (TWI_CKDIV_100 << 16)); // 100kHz
    }

    TWI_CR = (TWI_MSEN);                                                 // enable master mode
    #ifdef _WINDOWS
    fnConfigSimI2C(pars->Channel, (pars->usSpeed * 1000));
    #endif
}

// Send a first byte to I2C bus
//
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    unsigned long ulAddress;

    ptI2CQue->ucPresentLen = *ptI2CQue->I2C_queue.get++;                 // get present length
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }

    ulAddress = *ptI2CQue->I2C_queue.get++;
    ulAddress <<= 15;                                                    // move device address into position
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }

    if (ulAddress & 0x0008000) {                                         // reading
        TWI_MMR = ((BYTE_COMMAND_PROT | TWI_MREAD | ulAddress) & ~0x0008000);
        TWI_IER = (TWI_RXRDY);                                           // enable interrupt on ready
        TWI_CR = (TWI_MSEN | TWI_START);                                 // start transfer
    #if defined _WINDOWS
        TWI_IMR |= TWI_IER; TWI_IER = 0;                                 // simulate the interrupt mask setting
        fnSimI2C_devices(I2C_ADDRESS, (unsigned char)((TWI_MMR | 0x0008000)>>15));
        iInts |= I2C_INT0;
    #endif
        I2C_tx_control[0]->ucState |= (RX_ACTIVE | TX_ACTIVE);
        ptI2CQue->I2C_queue.chars -= 3;
        I2C_rx_control[0]->wake_task = *ptI2Que->I2C_queue.get++;       // enter task to be woken when reception has completed
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
    #if defined _WINDOWS
        TWI_SR |= TWI_RXRDY;                                             // simulate the interrupt directly
        TWI_RHR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)TWI_RHR);
        iInts |= I2C_INT0;
    #endif
    }
    else {
        TWI_MMR = (BYTE_COMMAND_PROT | ulAddress);
        TWI_THR = *ptI2CQue->I2C_queue.get++;                            // send first data byte
        TWI_CR = (TWI_MSEN | TWI_START);                                 // start transfer
        TWI_IER = (TWI_TXRDY);                                           // enable interrupt on ready
    #if defined _WINDOWS
        TWI_IMR |= TWI_IER; TWI_IER = 0;                                 // simulate the interrupt mask setting
        fnSimI2C_devices(I2C_ADDRESS, (unsigned char)(TWI_MMR>>15));
        iInts |= I2C_INT0;
    #endif
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
        I2C_tx_control[0]->ucState |= (TX_ACTIVE);                       // writing
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->ucPresentLen+1);
        ptI2CQue->ucPresentLen--;
    #if defined _WINDOWS
        TWI_SR |= TWI_TXRDY;                                             // simulate the interrupt directly
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)TWI_THR);
        iInts |= I2C_INT0;
    #endif
    }
}
#endif


#ifdef SUPPORT_EXTERNAL_INTERRUPTS
// External interrupt
//
    #ifdef _AT32UC3C                                                     // {16}
static __interrupt void _EXT_Interrupt_1_4(void)
{
    register volatile unsigned long ulSource;
    while ((ulSource = IRR15) != 0) {                                    // dispatch GPIO port interrupts
        register unsigned long ulExtInt = 0x00000002;
        register int iPort = 0;
        do {
            if (ulExtInt & ulSource) {
                ulSource &= ~ulExtInt;
                EIC_ICR = ulExtInt;                                      // clear the interrupt
        #ifdef _WINDOWS
                IRR15 &= ~ulExtInt;
        #endif
                iInterruptLevel = 1;
                extint_handlers[iPort]();                                // handle interrupts for specific ports
                iInterruptLevel = 0;
            }
            iPort++;
            ulExtInt <<= 1;
        } while (ulSource != 0);
    }
}

static __interrupt void _EXT_Interrupt_5_8(void)
{
    register volatile unsigned long ulSource;
    while ((ulSource = IRR16) != 0) {                                    // dispatch GPIO port interrupts
        register unsigned long ulExtInt = 0x00000020;
        register int iPort = 4;
        do {
            if (ulExtInt & ulSource) {
                ulSource &= ~ulExtInt;
                EIC_ICR = ulExtInt;                                      // clear the interrupt
        #ifdef _WINDOWS
                IRR16 &= ~ulExtInt;
        #endif
                iInterruptLevel = 1;
                extint_handlers[iPort]();                                // handle interrupts for specific ports
                iInterruptLevel = 0;
            }
            iPort++;
            ulExtInt <<= 1;
        } while (ulSource != 0);
    }
}

static __exception void _EXT_Interrupt_NMI(void)
{
    EIC_ICR = 0x00000001;                                                // clear the interrupt
        #ifdef _WINDOWS
    EIC_ISR &= ~EIC_ICR;
    EIC_ICR = 0;
        #endif
    extint_handlers[0]();                                                // call the user's handler - this is NMI !!
}
    #else
        #if defined SUPPORT_RTC
static void _rtc_handler(void);
        #endif

static __interrupt void _EXT_Interrupt(void)
{
    register volatile unsigned long ulSource;
#if defined SUPPORT_RTC
    if (IRR1 & IR_REAL_TIME_COUNTER) {                                   // RTC shared with external interrupts
        _rtc_handler();                                                  // call RTC handler
    }
#endif
    while ((ulSource = (IRR1 & (IR_EXTERNAL_INT_CONTROLLER_0 | IR_EXTERNAL_INT_CONTROLLER_1 | IR_EXTERNAL_INT_CONTROLLER_2 | IR_EXTERNAL_INT_CONTROLLER_3 | IR_EXTERNAL_INT_CONTROLLER_4 | IR_EXTERNAL_INT_CONTROLLER_5 | IR_EXTERNAL_INT_CONTROLLER_6 | IR_EXTERNAL_INT_CONTROLLER_7))) != 0) { // dispatch GPIO port interrupts
        register unsigned long ulExtInt = 0x00000001;
        register int iPort = 0;
        do {
            if (ulExtInt & ulSource) {
                ulSource &= ~ulExtInt;
                EIC_ICR = ulExtInt;                                      // clear the interrupt
        #ifdef _WINDOWS
                IRR1 &= ~ulExtInt;
        #endif
                iInterruptLevel = 1;
                extint_handlers[iPort]();                                // handle interrupts for specific ports
                iInterruptLevel = 0;
            }
            iPort++;
            ulExtInt <<= 1;
        } while (ulSource != 0);
    }
}

        #ifndef _AT32UC3B
static __exception void _EXT_Interrupt_NMI(void)
{
    EIC_ICR = 0x00000100;                                                // clear the interrupt
            #ifdef _WINDOWS
    EIC_ISR &= ~EIC_ICR;
    EIC_ICR = 0;
            #endif
    extint_handlers[8]();                                                // call the user's handler - this is NMI !!
}
        #endif
    #endif
#endif

#if defined SUPPORT_PORT_INTERRUPTS || defined SUPPORT_EXTERNAL_INTERRUPTS
// This routine enters the user handler for a port interrupt. The handler can be assigned to multiple inputs
//
static void fnEnterHandler(int iPortRef, unsigned long ulPortBits, void ( *int_handler )( void ))
{
    int port_bit = 0;
    unsigned long ulBit = 0x00000001;
    #ifdef SUPPORT_EXTERNAL_INTERRUPTS
    if (iPortRef == EXT_INT_CONTROLLER) {
        ulPortBits &= 0x000000ff;
        while (ulPortBits != 0) {
            if (ulPortBits & ulBit) {
                extint_handlers[port_bit] = int_handler;
                ulPortBits &= ~ulBit;
            }
            ulBit <<= 1;
            port_bit++;
        }
        return;
    }
    #endif
    #ifdef SUPPORT_PORT_INTERRUPTS
    while (ulPortBits != 0) {
        if (ulPortBits & ulBit) {
            gpio_handlers[iPortRef][port_bit] = int_handler;             // enter the application handler
            ulPortBits &= ~ulBit;
        }
        ulBit <<= 1;
        port_bit++;
    }
    #endif
}
#endif

#ifdef SUPPORT_PORT_INTERRUPTS

// Handler specific to GPIO 0
//
static void fnHandleGPIO_0(void)
{
    #ifdef SUPPORT_PORT_0_INTERRUPTS
    unsigned long ulInputChanges = IFR_0;
    unsigned long ulBit = 0x40000000;
    int iInterrupt = 30;
    ulInputChanges &= IER_0;                                             // only treat enabled inputs
    do {                                                                 // for each input change that has been detected
        if (ulInputChanges & ulBit) {                                    // an enabled input has changed
            iInterruptLevel = 1;
            IFRC_0 = ulBit;
        #ifdef _WINDOWS
            IFR_0 &= ~IFRC_0;
            if (IFRC_0 & 0x000000ff) {                                   // reset the status flag
                IRR_GPIO &= ~IR_GPIO_0;
            }
            else if (IFRC_0 & 0x0000ff00) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_1;
            }
            else if (IFRC_0 & 0x00ff0000) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_2;
            }
            else {                                                       // reset the status flag
                IRR_GPIO &= ~IR_GPIO_3;
            }
            IFRC_0 = 0;
        #endif
            (gpio_handlers[0][iInterrupt])();                            // call the application handler
            iInterruptLevel = 0;
            ulInputChanges &= ~ulBit;
        }
        iInterrupt--;
        ulBit >>= 1;
    } while (ulInputChanges != 0);
    #endif
}

    #if PORTS_AVAILABLE > 1
// Handler specific to GPIO 1
//
static void fnHandleGPIO_1(void)
{
        #ifdef SUPPORT_PORT_1_INTERRUPTS
    unsigned long ulInputChanges = IFR_1;
    unsigned long ulBit = 0x80000000;
    int iInterrupt = 31;
    ulInputChanges &= IER_1;                                             // only treat enabled inputs
    do {                                                                 // for each input change that has been detected
        if (ulInputChanges & ulBit) {                                    // an enabled input has changed
            iInterruptLevel = 1;
            IFRC_1 = ulBit;
            #ifdef _WINDOWS
            IFR_1 &= ~IFRC_1;
            if (IFRC_1 & 0x000000ff) {                                   // reset the status flag
                IRR_GPIO &= ~IR_GPIO_4;
            }
            else if (IFRC_1 & 0x0000ff00) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_5;
            }
            else if (IFRC_1 & 0x00ff0000) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_6;
            }
            else {                                                       // reset the status flag
                IRR_GPIO &= ~IR_GPIO_7;
            }
            IFRC_1 = 0;
            #endif
            (gpio_handlers[1][iInterrupt])();                            // call the application handler
            iInterruptLevel = 0;
            ulInputChanges &= ~ulBit;
        }
        iInterrupt--;
        ulBit >>= 1;
    } while (ulInputChanges != 0);
        #endif
}
    #endif
    #if PORTS_AVAILABLE > 2
// Handler specific to GPIO 2
//
static void fnHandleGPIO_2(void)
{
        #ifdef SUPPORT_PORT_2_INTERRUPTS
    unsigned long ulInputChanges = IFR_2;
    unsigned long ulBit = 0x80000000;
    int iInterrupt = 31;
    ulInputChanges &= IER_2;                                             // only treat enabled inputs
    do {                                                                 // for each input change that has been detected
        if (ulInputChanges & ulBit) {                                    // an enabled input has changed
            iInterruptLevel = 1;
            IFRC_2 = ulBit;
            #ifdef _WINDOWS
            IFR_2 &= ~IFRC_2;
            if (IFRC_2 & 0x000000ff) {                                   // reset the status flag
                IRR_GPIO &= ~IR_GPIO_8;
            }
            else if (IFRC_2 & 0x0000ff00) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_9;
            }
            else if (IFRC_2 & 0x00ff0000) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_10;
            }
            else {                                                       // reset the status flag
                IRR_GPIO &= ~IR_GPIO_11;
            }
            IFRC_2 = 0;
            #endif
            (gpio_handlers[2][iInterrupt])();                            // call the application handler
            iInterruptLevel = 0;
            ulInputChanges &= ~ulBit;
        }
        iInterrupt--;
        ulBit >>= 1;
    } while (ulInputChanges != 0);
        #endif
}
    #endif
    #if PORTS_AVAILABLE > 3
// Handler specific to GPIO 3
//
static void fnHandleGPIO_3(void)
{
        #ifdef SUPPORT_PORT_2_INTERRUPTS
    unsigned long ulInputChanges = IFR_3;
    unsigned long ulBit = 0x00002000;
    int iInterrupt = 13;
    ulInputChanges &= IER_3;                                             // only treat enabled inputs
    do {                                                                 // for each input change that has been detected
        if (ulInputChanges & ulBit) {                                    // an enabled input has changed
            iInterruptLevel = 1;
            IFRC_3 = ulBit;
            #ifdef _WINDOWS
            IFR_3 &= ~IFRC_3;
            if (IFRC_3 & 0x000000ff) {                                   // reset the status flag
                IRR_GPIO &= ~IR_GPIO_12;
            }
            else if (IFRC_3 & 0x0000ff00) {                              // reset the status flag
                IRR_GPIO &= ~IR_GPIO_13;
            }
            IFRC_3 = 0;
            #endif
            (gpio_handlers[3][iInterrupt])();                            // call the application handler
            iInterruptLevel = 0;
            ulInputChanges &= ~ulBit;
        }
        iInterrupt--;
        ulBit >>= 1;
    } while (ulInputChanges != 0);
        #endif
}
    #endif


static void (*fnHandleGPIO[PORTS_AVAILABLE])( void ) = {                 // dedicated handler for each possible port
    fnHandleGPIO_0,
#if PORTS_AVAILABLE > 1
    fnHandleGPIO_1,
#endif
#if PORTS_AVAILABLE > 2
    fnHandleGPIO_2,
#endif
#if PORTS_AVAILABLE > 3
    fnHandleGPIO_3
#endif
};

// GPIO interrupt
//
static __interrupt void _GPIO_Interrupt(void)
{
    register volatile unsigned long ulSource;
    while ((ulSource = IRR_GPIO) != 0) {                                 // dispatch GPIO port interrupts
        register unsigned long ulGPIO = 0x0000000f;
        register int iPort = 0;
        do {
            if (ulGPIO & ulSource) {
                ulSource &= ~ulGPIO;
                fnHandleGPIO[iPort]();                                   // handle interrupts for specific ports
            }
            iPort++;
            ulGPIO <<= 4;
        } while (ulSource != 0);
    }
}

#if defined _PHY_RTL8201E                                                // {28}
    #define LINK_STATE_BIT           (PHY_LINK_IS_UP)
    #define LINK_SPEED_IS_100M       (PHY_SELECT_100M_SPEED)
    #define LINK_MODE_IS_FULL_DUPLEX (PHY_FULL_DUPLEX_MODE)
#else
    #define LINK_STATE_BIT           (DP83848_LINK_STATUS_UP)
    #define LINK_SPEED_IS_100M       (DP83848_100BASET_DESCRAMBLE_LOCK | DP83848_100BASET_SIGNAL_DETECT)
    #define LINK_MODE_IS_FULL_DUPLEX (DP83848_DUPLEX_STATUS)
#endif

    #if defined ETH_INTERFACE && (defined PHY_POLL_LINK || (defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS)) // {25}{29}
extern void fnCheckEthLinkState(void)
{
        #ifdef INTERRUPT_TASK_PHY
    unsigned char int_phy_message[HEADER_LENGTH] = {0, 0, INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT}; // define standard interrupt event
        #endif
    volatile unsigned short usInterrupt;
        #if defined _PHY_RTL8201E                                        // {28}
    usInterrupt = (volatile unsigned short)fnReadMII(DP83848_BASIC_MODE_STATUS_REGISTER);// check the  details of link
        #else
    usInterrupt = fnReadMII(DP83848_MISR);                               // read the cause(s) of the interrupt, which resets the bits
    usInterrupt = (short)fnReadMII(DP83848_PHY_STATUS);
    usInterrupt = (volatile unsigned short)fnReadMII(DP83848_PHY_STATUS);// check the  details of link - double read required
        #endif
    if (LINK_STATE_BIT & usInterrupt) {                                  // check whether the link is up or down
        #if defined _PHY_RTL8201E                                        // {28}
        usInterrupt = (volatile unsigned short)fnReadMII(DP83848_BASIC_MODE_CONTROL_REGISTER);// check the  details of link
        #endif
        if (usInterrupt & LINK_SPEED_IS_100M) {
            MACB_NCFGR |= SPD;                                           // set 100M operation in MACB
        #if defined INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
        #endif
        }
        else {
            MACB_NCFGR &= ~SPD;                                          // set 10M operation
        #ifdef INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
        #endif
        }
        if (usInterrupt & LINK_MODE_IS_FULL_DUPLEX) {                    // set duplex operation accordingly
            MACB_NCFGR |= FD;
        #ifdef INTERRUPT_TASK_PHY
            (int_phy_message[MSG_INTERRUPT_EVENT])++;                    // {29} change to full duplex event
        #endif
        }
        else {
            MACB_NCFGR &= ~FD;
        }
    }
    else {                                                               // link is down
        #ifdef INTERRUPT_TASK_PHY
        int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
        #endif
    }
        #ifdef INTERRUPT_TASK_PHY
    fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // inform the corresponding task
        #endif
}
    #endif

    #if defined ETH_INTERFACE && (defined PHY_INTERRUPT && defined SUPPORT_PORT_INTERRUPTS) // {25}{29}
// PHY interrupt handler. Based on DP83848 on EVK1105
//
static void fnPhyInterrupt(void)
{
    while (PHY_INPUT_LOW()) {                                            // while the interrupt line remains low
        fnCheckEthLinkState();                                           // {29}
    }
}
    #endif
#endif

#ifdef SUPPORT_TIMER

static void ( *timer_handler[HW_TIMERS_AVAILABLE] )( void ) = {0};
    #ifdef _HW_TIMER_MODE
    static unsigned char _hw_timer_mode[HW_TIMERS_AVAILABLE] = {0};      // {9}
    #endif

// Interrupt dispatcher for timers
//
static __interrupt void _timer_handler(void)
{
    register volatile unsigned long ulTimerModules;
    while ((ulTimerModules = IRR14) != 0) {
        register unsigned long ulTimer = 0x00000001;
        register int iRoutine = 0;
        TIMER_MODULE *ptrTimer = (TIMER_MODULE *)TIMER0_PERIPHERAL_BLOCK;
        do {
            if (ulTimerModules & ulTimer) {
                volatile unsigned int dummy = ptrTimer->TC_SR;           // read the status to reset interrupt
    #ifdef _HW_TIMER_MODE                                                // {9}
                if (!(_hw_timer_mode[iRoutine] & TIMER_PERIODIC)) {      // don't disable if in periodic mode
                    ptrTimer->TC_IDR = TIM_CPCS;                         // disable further timer interrupt
        #ifdef _WINDOWS
                    ptrTimer->TC_SR  &= ~TIM_CLKSTA;
                    ptrTimer->TC_IMR &= ~TIM_CPCS;                       // flag that the timer is not running
        #endif
                }
        #ifdef _WINDOWS
                IRR14 &= ~ulTimerModules;
        #endif
    #else
                ptrTimer->TC_IDR = TIM_CPCS;                             // disable further timer interrupt
        #ifdef _WINDOWS
                ptrTimer->TC_SR  &= ~TIM_CLKSTA;
                ptrTimer->TC_IMR &= ~TIM_CPCS;                           // flag that the timer is not running
                IRR14 &= ~ulTimerModules;
        #endif
    #endif
                ulTimerModules &= ~ulTimer;
                if (timer_handler[iRoutine] != 0) {
                    iInterruptLevel = 1;                                 // ensure interrupts remain blocked during subroutines
                    (timer_handler[iRoutine])();
                    iInterruptLevel = 0;                                 // release
                }
            }
            iRoutine++;
            ulTimer <<= 1;
            ptrTimer++;
        } while (ulTimerModules != 0);
    }
}
#endif

#if defined SUPPORT_ADC
static void (*_adc_handler)(ADC_INTERRUPT_RESULT*) = 0;
static unsigned long adc_trigger_source;
static unsigned short ucLevels[ADC_CHANNELS][3];
  #define ZERO_CROSS_VALUE   0
  #define LOW_LEVEL_VALUE    1
  #define HIGH_LEVEL_VALUE   2
static unsigned short ucPreviousLevels[ADC_CHANNELS];
static unsigned char  ucADCmode[ADC_CHANNELS] = {0};

// Interrupt after DMA sampling sequence has completed
//
static __interrupt void _adc_sequence_complete(void)
{
    ADC_IDR = ADC_ENDRX;                                                 // no more PDC interrupt
#ifdef _WINDOWS
    ADC_IMR &= ~ADC_IDR; ADC_IDR = 0;
#endif
    if (_adc_handler != 0) {
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked during subroutines
        (_adc_handler)(0);                                               // call user handler
        iInterruptLevel = 0;                                             // release
    }
}

// Interrupt after DMA sampling sequence has completed
//
static __interrupt void _adc_sample_complete(void)
{
    unsigned long ulStatus;
    unsigned long ulBit = 0x00000001;
    int iChannel = 0;
    #ifdef ADC_AUTO_TRIGGERS
    int iComplete = 0;
    #endif
    ADC_INTERRUPT_RESULT adc_res;
    while ((ulStatus = ADC_SR) & (EOC0 | EOC1 | EOC2 | EOC3 | EOC4 | EOC5 | EOC6 | EOC7)) {
        if (ulBit & ulStatus) {                                          // sample ready
            adc_res.sADC_value = (signed short)*(ADC_CDR0_ADD + iChannel);// read the value, which clears the interrupt from this channel
            adc_res.ucADC_flags = ADC_RESULT_VALID;
    #ifdef _WINDOWS
            ADC_SR &= ~(ulBit);                                          // clear the sample ready bit
    #endif
    #ifdef ADC_AUTO_TRIGGERS
            if (ucADCmode[iChannel] != 0) {                              // this channel is operating in trigger mode
                if (ucPreviousLevels[iChannel] != 0xffff) {
                    if (ucADCmode[iChannel] & ADC_ZERO_CROSSING_INT_POSITIVE) {
                        if ((ucPreviousLevels[iChannel] <= ucLevels[iChannel][ZERO_CROSS_VALUE]) && ((unsigned short)adc_res.sADC_value > ucLevels[iChannel][ZERO_CROSS_VALUE])) {
                            if (ucADCmode[iChannel] & ADC_SINGLE_SHOT_CROSSING_INT) {
                                ucADCmode[iChannel] &= ~(ADC_SINGLE_SHOT_CROSSING_INT | ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE);
                            }
                            adc_res.ucADC_flags = ADC_INT_ZERO_CROSSING;
                        }
                    }
                    if (ucADCmode[iChannel] & ADC_ZERO_CROSSING_INT_NEGATIVE) {
                        if ((ucPreviousLevels[iChannel] >= ucLevels[iChannel][ZERO_CROSS_VALUE]) && ((unsigned short)adc_res.sADC_value < ucLevels[iChannel][ZERO_CROSS_VALUE])) {
                            if (ucADCmode[iChannel] & ADC_SINGLE_SHOT_CROSSING_INT) {
                                ucADCmode[iChannel] &= ~(ADC_SINGLE_SHOT_CROSSING_INT | ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE);
                            }
                            adc_res.ucADC_flags = ADC_INT_ZERO_CROSSING;
                        }
                    }
                    if (ucADCmode[iChannel] & ADC_LOW_LIMIT_INT) {
                        if ((ucPreviousLevels[iChannel] >= ucLevels[iChannel][LOW_LEVEL_VALUE]) && ((unsigned short)adc_res.sADC_value < ucLevels[iChannel][LOW_LEVEL_VALUE])) {
                            if (ucADCmode[iChannel] & ADC_SINGLE_SHOT_TRIGGER_INT) {
                                ucADCmode[iChannel] &= ~(ADC_SINGLE_SHOT_TRIGGER_INT | ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT);
                            }
                            adc_res.ucADC_flags |= ADC_INT_LOW_LEVEL;
                        }
                    }
                    if (ucADCmode[iChannel] & ADC_HIGH_LIMIT_INT) {
                        if ((ucPreviousLevels[iChannel] <= ucLevels[iChannel][HIGH_LEVEL_VALUE]) && ((unsigned short)adc_res.sADC_value > ucLevels[iChannel][HIGH_LEVEL_VALUE])) {
                            if (ucADCmode[iChannel] & ADC_SINGLE_SHOT_TRIGGER_INT) {
                                ucADCmode[iChannel] &= ~(ADC_SINGLE_SHOT_TRIGGER_INT | ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT);
                            }
                            adc_res.ucADC_flags |= ADC_INT_HIGH_LEVEL;
                        }
                    }
                    if ((ucADCmode[iChannel] & ~ADC_TRIGGERS_TERMINATED) == 0) {// if no more single shot triggers, disable ADC trigger sources
                        ucADCmode[iChannel] = ADC_TRIGGERS_TERMINATED;   // avoid terminated channel from being handled as normal scan interrupt
                        iComplete |= 1;                                  // mark that at least one channel has no more triggers active
                    }
                    else {
                        iComplete |= 2;                                  // mark that at least this channel still has active triggers waiting
                    }
                }
                ucPreviousLevels[iChannel] = (unsigned short)adc_res.sADC_value; // back up the last sample for this channel for later comparison
                if (adc_res.ucADC_flags == ADC_RESULT_VALID) {
                    ulBit <<= 1;
                    iChannel++;
                    continue;                                            // no interrupt
                }
            }
    #endif
            if ((ADC_IMR & ulBit) && (_adc_handler != 0)) {              // interrupt enabled on this channel
                adc_res.ucADC_channel = (unsigned char)iChannel;
                if (ADC_MR & LOWRES) {
                    adc_res.ucADC_flags |= (ADC_8BIT_MODE);
                }
                if (ulStatus & (ulBit << 8)) {                           // check whether there was an overrun on this channel
                    adc_res.ucADC_flags |= (ADC_CHANNEL_OVERRUN);
                }
                iInterruptLevel = 1;                                     // ensure interrupts remain blocked during subroutines
                (_adc_handler)(&adc_res);                                // call user handler
                iInterruptLevel = 0;                                     // release
            }
        }
        ulBit <<= 1;
        iChannel++;
    }
    #ifdef ADC_AUTO_TRIGGERS
    if (iComplete == 1) {                                                // last single shot trigger fired so disable operation
        if (adc_trigger_source != 0) {                                   // timer operating
          //POWER_DOWN(adc_trigger_source);                              // disable clocks to timer in PMC to stop further ADC sample triggering
        }
    }
    #endif
    #ifdef _WINDOWS
    IRR15 = 0;
    #endif
}
#endif

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
    case PORT_INTERRUPT:
        {
    #if defined SUPPORT_PORT_INTERRUPTS || defined SUPPORT_EXTERNAL_INTERRUPTS
        #if defined _AT32UC3C
            unsigned char ucPortNumber = ((((INTERRUPT_SETUP *)ptrSettings)->int_port) & 0x0f);
            int iInterruptPinOption = ((((INTERRUPT_SETUP *)ptrSettings)->int_port) >> 4);
        #else
            unsigned char ucPortNumber = (((INTERRUPT_SETUP *)ptrSettings)->int_port);
        #endif
            unsigned long ulPortBits = (((INTERRUPT_SETUP *)ptrSettings)->int_port_bits);
        #ifdef SUPPORT_PORT_INTERRUPTS
            GPIO_REGS *ptrReg = (GPIO_REGS *)GPIO_BLOCK;
        #endif
            switch (ucPortNumber) {
        #ifdef SUPPORT_PORT_0_INTERRUPTS
            case PORT_0:
                _CONFIG_PORT_INPUT(0, ulPortBits);                       // ensure that the pin(s) are inputs
                break;
        #endif
        #ifdef SUPPORT_PORT_1_INTERRUPTS
            case PORT_1:
                _CONFIG_PORT_INPUT(1, ulPortBits);                       // ensure that the pin(s) are inputs
                break;
        #endif
        #ifdef SUPPORT_PORT_2_INTERRUPTS
            case PORT_2:
                _CONFIG_PORT_INPUT(2, ulPortBits);                       // ensure that the pin(s) are inputs
                break;
        #endif
        #ifdef SUPPORT_PORT_3_INTERRUPTS
            case PORT_3:
                _CONFIG_PORT_INPUT(3, ulPortBits);                       // ensure that the pin(s) are inputs
                break;
        #endif

        #ifdef SUPPORT_EXTERNAL_INTERRUPTS
            case EXT_INT_CONTROLLER:                                     // configure the pins for external interrupt use
            #ifdef _AT32UC3C                                             // {16}
                POWER_UP(PBA, PBAMASK_EIC);                              // ensure the external interrupt controller is enabled
            #else
                POWER_UP(PBA, PBAMASK_RM_RTC_EIC);                       // ensure the external interrupt controller is enabled
            #endif
                if (ulPortBits & EXT_INT_0) {                            // ensure that ports are inputs
            #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PA25-C / PA29-B / PC27-D / PD21-B - this is the NMI external interrupt on this chip
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_C(0, EIC_0_0_C);             // configure as EIC0
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(0, EIC_0_0_B);             // configure as EIC0
                        break;
                    case 2:
                        _CONFIG_PERIPHERAL_D(2, EIC_0_2_D);             // configure as EIC0
                        break;
                    case 3:
                        _CONFIG_PERIPHERAL_B(3, EIC_0_3_B);             // configure as EIC0
                        break;
                    }
            #else 
                    _CONFIG_PORT_INPUT(0, EIC_0_X);                      // ensure that the pin is an input
                #if defined _WINDOWS
                    #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_A(0, EIC_0_A);
                    #else
                    _CONFIG_PERIPHERAL_B(0, EIC_0_B);
                    #endif
                #endif
            #endif
                }
                if (ulPortBits & EXT_INT_1) {
            #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PA03-B / PA19-B / PA26-B / PC03-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_B(0, EIC_1_0A_B);             // configure as EIC1
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(0, EIC_1_0B_B);             // configure as EIC1
                        break;
                    case 2:
                        _CONFIG_PERIPHERAL_B(0, EIC_1_0C_B);             // configure as EIC1
                        break;
                    case 3:
                        _CONFIG_PERIPHERAL_B(2, EIC_1_2_B);              // configure as EIC1
                        break;
                    }
            #else 
                    _CONFIG_PORT_INPUT(0, EIC_1_X);                      // ensure that the pin is an input
                #ifdef _WINDOWS
                    #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_A(0, EIC_1_A);
                    #else
                    _CONFIG_PERIPHERAL_B(0, EIC_1_B);
                    #endif
                #endif
            #endif
                }
                if (ulPortBits & EXT_INT_2) {
            #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PA08-C / PA27-B / PB07-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_C(0, EIC_2_0_C);             // configure as EIC2
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(0, EIC_2_0_B);             // configure as EIC2
                        break;
                    case 2:
                        _CONFIG_PERIPHERAL_B(1, EIC_2_1_B);             // configure as EIC2
                        break;
                    }
            #else 
                    _CONFIG_PORT_INPUT(0, EIC_2_X);                      // ensure that the pin is an input
                #ifdef _WINDOWS
                    #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_C(0, EIC_2_C);
                    #else
                    _CONFIG_PERIPHERAL_B(0, EIC_2_B);
                    #endif
                #endif
            #endif
                }
                if (ulPortBits & EXT_INT_3) {
            #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PA28-B / PC04-B / PD30-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_B(0, EIC_3_0_B);              // configure as EIC3
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(2, EIC_3_2_B);              // configure as EIC3
                        break;
                    case 2:
                        _CONFIG_PERIPHERAL_B(3, EIC_3_3_B);              // configure as EIC3
                        break;
                    }
            #else 
                    _CONFIG_PORT_INPUT(0, EIC_3_X);                      // ensure that the pin is an input
                #ifdef _WINDOWS
                    #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_C(0, EIC_3_C);
                    #else
                    _CONFIG_PERIPHERAL_B(0, EIC_3_B);
                    #endif
                #endif
            #endif
                }
                if (ulPortBits & EXT_INT_4) {
            #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PA10-B / PB18-C / PC05-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_B(0, EIC_4_0_B);              // configure as EIC4
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_C(1, EIC_4_1_C);              // configure as EIC4
                        break;
                    case 2:
                        _CONFIG_PERIPHERAL_B(2, EIC_4_2_B);              // configure as EIC4
                        break;
                    }
            #else 
                    _CONFIG_PORT_INPUT(0, EIC_4_X);                      // ensure that the pin is an input
                #ifdef _WINDOWS
                    #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_C(0, EIC_4_C);
                    #else
                    _CONFIG_PERIPHERAL_B(0, EIC_4_B);
                    #endif
                #endif
            #endif
                }
                if (ulPortBits & EXT_INT_5) {
            #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PC18-B / PD07-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_B(2, EIC_5_2_B);              // configure as EIC5
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(3, EIC_5_3_B);              // configure as EIC5
                        break;
                    }
            #else 
                    _CONFIG_PORT_INPUT(0, EIC_5_X);                      // ensure that the pin is an input
                #ifdef _WINDOWS
                    #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_C(0, EIC_5_C);
                    #else
                    _CONFIG_PERIPHERAL_B(0, EIC_5_B);
                    #endif
                #endif
            #endif
                }
            #if !defined _AT32UC3B || !defined CHIP_48_PIN
                if (ulPortBits & EXT_INT_6) {
                #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PD08-B / PD29-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_B(3, EIC_6_3A_B);             // configure as EIC6
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(3, EIC_6_3B_B);             // configure as EIC6
                        break;
                    }
                #else 
                    #ifdef _AT32UC3B
                    _CONFIG_PORT_INPUT(1, EIC_6_A);                      // ensure that the pin is an input
                    #else
                    _CONFIG_PORT_INPUT(0, EIC_6_B);                      // ensure that the pin is an input
                    #endif
                    #ifdef _WINDOWS
                        #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_A(1, EIC_6_A);
                        #else
                    _CONFIG_PERIPHERAL_B(0, EIC_6_B);
                        #endif
                    #endif
                 #endif
                }
                if (ulPortBits & EXT_INT_7) {
                #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PC13-B / PD14-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_B(2, EIC_7_2_B);              // configure as EIC7
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(3, EIC_7_3_B);              // configure as EIC7
                        break;
                    }
                #else 
                    #ifdef _AT32UC3B
                    _CONFIG_PORT_INPUT(1, EIC_7_A);                      // ensure that the pin is an input
                    #else
                    _CONFIG_PORT_INPUT(0, EIC_7_C);                      // ensure that the pin is an input
                    #endif
                    #ifdef _WINDOWS
                        #ifdef _AT32UC3B
                    _CONFIG_PERIPHERAL_A(1, EIC_7_A);
                        #else
                    _CONFIG_PERIPHERAL_C(0, EIC_7_C);
                        #endif
                    #endif
                #endif
                }
            #endif
            #ifndef _AT32UC3B
                if (ulPortBits & EXT_INT_8) {
                #if defined _AT32UC3C
                    switch (iInterruptPinOption) {                       // PB00-C / PD24-B
                    case 0:
                    default:
                        _CONFIG_PERIPHERAL_C(1, EIC_8_1_C);              // configure as EIC8
                        break;
                    case 1:
                        _CONFIG_PERIPHERAL_B(3, EIC_8_3_B);              // configure as EIC8
                        break;
                    }
                #else
                    _CONFIG_PORT_INPUT(0, EIC_8_A);                      // ensure that the pin is an input
                    #ifdef _WINDOWS
                    _CONFIG_PERIPHERAL_A(0, EIC_8_A);
                    #endif
                #endif
                }
            #endif
                EIC_EN = ulPortBits;                                     // enable EIC on the pin(s)
            #ifdef _WINDOWS
                EIC_CTRL |= EIC_EN;
                EIC_EN = 0;
            #endif
                break;
        #endif
            default:                                                     // invalid port
                return;
            }
        #ifdef SUPPORT_PORT_INTERRUPTS
            ptrReg += ucPortNumber;
        #endif
            switch ((((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) & (IRQ_BOTH_EDGES | IRQ_LOW_LEVEL_SENSITIVE)) {
            case IRQ_RISING_EDGE:
        #ifdef SUPPORT_EXTERNAL_INTERRUPTS
                if (ucPortNumber == EXT_INT_CONTROLLER) {
                    EIC_MODE &= ~ulPortBits;                             // edge sensitive
                    EIC_EDGE |= ulPortBits;                              // rising edge
                    break;
                }
        #endif
        #ifdef SUPPORT_PORT_INTERRUPTS
                ptrReg->IMR0S = ulPortBits;
                ptrReg->IMR1C = ulPortBits;
        #endif
                break;
            case IRQ_FALLING_EDGE:
        #ifdef SUPPORT_EXTERNAL_INTERRUPTS
                if (ucPortNumber == EXT_INT_CONTROLLER) {
                    EIC_MODE &= ~ulPortBits;                             // edge sensitive
                    EIC_EDGE &= ~ulPortBits;                             // falling edge
                    break;
                }
        #endif
        #ifdef SUPPORT_PORT_INTERRUPTS
                ptrReg->IMR0C = ulPortBits;
                ptrReg->IMR1S = ulPortBits;
        #endif
                break;
        #ifdef SUPPORT_PORT_INTERRUPTS
            case (IRQ_BOTH_EDGES):                                       // both edges only possible on port interrupts
                if (ucPortNumber != EXT_INT_CONTROLLER) {
                    ptrReg->IMR0C = ulPortBits;
                    ptrReg->IMR1C = ulPortBits;
                }
                break;
        #endif
        #ifdef SUPPORT_EXTERNAL_INTERRUPTS
            case IRQ_HIGH_LEVEL_SENSITIVE:
                if (ucPortNumber == EXT_INT_CONTROLLER) {                // only available on external interrupts
                    EIC_MODE |= ulPortBits;                              // level sensitive
                    EIC_LEVEL |= ulPortBits;                             // high level sensitive
                    break;
                }
                break;
            case IRQ_LOW_LEVEL_SENSITIVE:
                if (ucPortNumber == EXT_INT_CONTROLLER) {                // only available on external interrupts
                    EIC_MODE |= ulPortBits;                              // level sensitive
                    EIC_LEVEL &= ~ulPortBits;                            // low level sensitive
                    break;
                }
                break;

        #endif
            default:
                return;
            }
        #ifdef SUPPORT_EXTERNAL_INTERRUPTS
            if (ucPortNumber == EXT_INT_CONTROLLER) {
                if (IRQ_ASYNC & (((INTERRUPT_SETUP *)ptrSettings)->int_port_sense)) {
                    EIC_ASYNC |= ulPortBits;
                }
                else {
                    EIC_ASYNC &= ~ulPortBits;
                }
                if ((((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) & IRQ_ENABLE_GLITCH_FILER) {
                    EIC_FILTER |= ulPortBits;
                }
                else {
                    EIC_FILTER &= ~ulPortBits;
                }
                EIC_ICR = ulPortBits;                                    // clear possible pending interrupts
            #ifdef _AT32UC3C                                             // {16}
                if (ulPortBits & 0x0000001f) {
                    fnEnterAVRInterrupt(IR_GROUP_INTC_1_4, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), (void (*)(void))_EXT_Interrupt_1_4);
                }
                if (ulPortBits & 0x00000001) {                           // external interrupt 0 is NMI
                #ifdef _WINDOWS
                    unsigned long *ptrEventTable = (unsigned long *)(AVR32_ExceptionRAM + 0x10);
                #else
                    unsigned long *ptrEventTable = (unsigned long *)(EXCEPTION_VECTOR_BASE_ADDRESS + 0x10);
                #endif
                    *ptrEventTable++ = LOAD_PC_WITH_NEXT_VALUE;
                    *ptrEventTable = (unsigned long)_EXT_Interrupt_NMI;                    
                }
                if (ulPortBits & 0x000001e0) {
                    fnEnterAVRInterrupt(IR_GROUP_INTC_5_8, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), (void (*)(void))_EXT_Interrupt_5_8);
                }
            #else
                #ifndef _AT32UC3B
                if (ulPortBits & 0x00000100) {                           // external interrupt 8 is NMI
                    #ifdef _WINDOWS
                    unsigned long *ptrEventTable = (unsigned long *)(AVR32_ExceptionRAM + 0x10);
                    #else
                    unsigned long *ptrEventTable = (unsigned long *)(EXCEPTION_VECTOR_BASE_ADDRESS + 0x10);
                    #endif
                    *ptrEventTable++ = LOAD_PC_WITH_NEXT_VALUE;
                    *ptrEventTable = (unsigned long)_EXT_Interrupt_NMI; 
                }
                else {
                    fnEnterAVRInterrupt(IR_GROUP_INTC_RTC_POW_FREQ, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), (void (*)(void))_EXT_Interrupt);
                }
                #else
                fnEnterAVRInterrupt(IR_GROUP_INTC_RTC_POW_FREQ, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), (void (*)(void))_EXT_Interrupt);
                #endif
            #endif
                fnEnterHandler(ucPortNumber, ulPortBits, (((INTERRUPT_SETUP *)ptrSettings)->int_handler));
                EIC_IER = ulPortBits;                                    // enable external interrupt(s)
            #ifdef _WINDOWS
                EIC_IMR |= EIC_IER;
                EIC_IER = 0;
            #endif
                break;
            }
        #endif
        #if defined SUPPORT_PORT_INTERRUPTS
            if ((((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) & IRQ_ENABLE_GLITCH_FILER) {
                ptrReg->GFERS = ulPortBits;                              // enable glitch filter on port
            }
            else {
                ptrReg->GFERC = ulPortBits;                              // enable glitch filter on port
            }
            fnEnterAVRInterrupt(IR_GROUP_GPIO, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), (void (*)(void))_GPIO_Interrupt);
            fnEnterHandler(ucPortNumber, ulPortBits, (((INTERRUPT_SETUP *)ptrSettings)->int_handler));
            ptrReg->IERS = ulPortBits;                                   // enable specific interrupt lines
            #ifdef _WINDOWS
            ptrReg->IMR0 |= ptrReg->IMR0S;
            ptrReg->IMR1 |= ptrReg->IMR1S;
            ptrReg->IMR0 &= ~ptrReg->IMR0C;
            ptrReg->IMR1 &= ~ptrReg->IMR1C;
            ptrReg->IER |= ptrReg->IERS;
            ptrReg->IMR0S = 0;
            ptrReg->IMR0C = 0;
            ptrReg->IMR1S = 0;
            ptrReg->IMR1C = 0;
            ptrReg->IERS = 0;
            #endif
        #endif
    #endif
        }
        break;

#ifdef SUPPORT_TIMER
    case TIMER_INTERRUPT:
        {
            volatile unsigned long dummy;
            volatile unsigned long *ptrTimeBlock;
            unsigned short usMode = (WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS); // default for single shot timer mode
            int iTimer = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_reference); // the timer to use
            if (iTimer >= HW_TIMERS_AVAILABLE) {
                return;
            }
            switch (iTimer) {
    #if HW_TIMER_CHANNEL != 0 || !defined GLOBAL_HARDWARE_TIMER          // protect from collision with global hardware timer on channel
            case 0:
                ptrTimeBlock = (unsigned long *)TIMER0_PERIPHERAL_BLOCK;
                break;
    #endif
    #if HW_TIMER_CHANNEL != 1 || !defined GLOBAL_HARDWARE_TIMER          // protect from collision with global hardware timer on channel
            case 1:
                ptrTimeBlock = (unsigned long *)TIMER1_PERIPHERAL_BLOCK;
                break;
    #endif
    #if HW_TIMER_CHANNEL != 2 || !defined GLOBAL_HARDWARE_TIMER          // protect from collision with global hardware timer on channel
            case 2:
                ptrTimeBlock = (unsigned long *)TIMER2_PERIPHERAL_BLOCK;
                break;
    #endif
            default:
                return;                                                  // ignore non-supported timer channels
            }

            POWER_UP(PBA, PBAMASK_TC);                                   // enable clocks to timers
            *ptrTimeBlock = (TIM_CLKDIS | TIM_SWTRG);                    // disable timer clock and reset timer in TC_CCR_X
            dummy = *(ptrTimeBlock + (0x20/sizeof(unsigned long)));      // read status register to reset possible pending interrupt
            timer_handler[iTimer] = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->int_handler);
            fnEnterAVRInterrupt(IR_GROUP_TIMERS, (((TIMER_INTERRUPT_SETUP *)ptrSettings)->int_priority), (void (*)(void))_timer_handler);
    #ifdef _HW_TIMER_MODE
            _hw_timer_mode[iTimer] = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_mode; // {9}
    #endif
    #ifdef SUPPORT_US_TIMER
            if ((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value) != 0) { // ms value
                *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_5 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// PBACLK/128, wave mode, trigger on register RC, stop after triggering in TC_CMR_X
                *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (unsigned short)(((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value) * (PBA_CLOCK/128))/1000);  // set ms match value in TC_RC_X
            }
            else {
                *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_4 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// PBACLK/32, wave mode, trigger on register RC, stop after triggering in TC_CMR_X
                *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (unsigned short)(((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_us_value) * (PBA_CLOCK/32000))/1000);  // set ms match value in TC_RC_X
            }
    #else
        #ifdef _HW_TIMER_MODE
            if (_hw_timer_mode[iTimer] & TIMER_PERIODIC) {
                usMode &= ~(CPCSTOP | CPCDIS);                           // don't stop on match
            }
        #endif
            *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_5 | usMode); // PBACLK/128, wave mode, trigger on register RC, stop after triggering in TC_CMR_X
            *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (unsigned short)(((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value) * (PBA_CLOCK/128))/1000);  // set ms match value in TC_RC_X
    #endif
            *ptrTimeBlock = (TIM_CLKEN | TIM_SWTRG);                     // start timer by writing to TC_CCR_X
            *(ptrTimeBlock + (0x24/sizeof(unsigned long))) = TIM_CPCS;   // enable interrupt on RC Compare Interrupt in TC_IER_X
    #ifdef _WINDOWS
            *(ptrTimeBlock + (0x20/sizeof(unsigned long))) |= TIM_CLKSTA; *(ptrTimeBlock + (0x2c/sizeof(unsigned long))) |= TIM_CPCS; // flag that the timer is running (SR and IMR)
    #endif
        }
        break;
#endif
#if defined SUPPORT_ADC
    case ADC_INTERRUPT:
        {
            ADC_SETUP *adc_setup = (ADC_SETUP *)ptrSettings;
            if (adc_setup->int_adc_mode & ADC_READ_ONLY) {
                if ((adc_setup->int_adc_mode & (ADC_GET_RESULT | ADC_ALL_RESULTS)) && (adc_setup->int_adc_result != 0)) { // if there is a result structure
                    int iLastValue = (ADC_CHANNELS - 1);
                    int iFirstValue = 0;
                    if (!(adc_setup->int_adc_mode & ADC_ALL_RESULTS)) {
                        unsigned char ucBits = adc_setup->int_adc_bits;
                        if (ucBits != 0) {
                            while (!(ucBits & 0x1)) {
                                iFirstValue++;
                                ucBits >>= 1;
                            }
                            ucBits = adc_setup->int_adc_bits;
                            if (ucBits != 0) {
                                while (!(ucBits & 0x80)) {
                                    iLastValue--;
                                    ucBits <<= 1;
                                }
                            }
                        }
                    }
                    while (iFirstValue <= iLastValue) {
    #ifdef _WINDOWS
                        fnUpdateADC(iFirstValue);                        // cause the simulated input value to be captured
    #endif
                        ((ADC_RESULTS *)(adc_setup->int_adc_result))->sADC_value[iFirstValue] = (signed short)*(ADC_CDR0_ADD + iFirstValue); // the last conversion result
                        ((ADC_RESULTS *)(adc_setup->int_adc_result))->ucADC_status[iFirstValue] = ADC_RESULT_VALID;
    #ifdef _WINDOWS
                        ADC_SR &= ~(0x01 << iFirstValue);                // clear the sample ready bit
    #endif
                        if (ADC_MR & LOWRES) {
                            ((ADC_RESULTS *)(adc_setup->int_adc_result))->ucADC_status[iFirstValue] |= ADC_8BIT_MODE;
                        }
                        iFirstValue++;
                    }

                }
                break;
            }
            if (adc_setup->int_adc_mode & ADC_CONFIGURE_ADC) {           // general configuration
                unsigned long ulMode = 0;
                POWER_UP(PBA, PBAMASK_ADC);                              // {10} ensure power applied to ADC
                if (adc_setup->int_adc_mode & (ADC_TRIGGERED_TIM0 | ADC_TRIGGERED_TIM1 | ADC_TRIGGERED_TIM2 | ADC_TRIGGERED_EXT)) { // hardware triggered mode
                    if (adc_setup->int_adc_mode & ADC_TRIGGERED_EXT) {
                        ulMode = (TRGEN | TRGSEL_EXT);
    #ifdef _AT32UC3B
                        _CONFIG_PERIPHERAL_C(0, ADC_TRIG_C);             // configure trigger input
    #else
                        _CONFIG_PERIPHERAL_B(1, ADC_TRIG_B);             // configure trigger input
    #endif
                    }
                    else if (adc_setup->int_adc_mode & ADC_TRIGGERED_TIM0) {
                        ulMode = (TRGEN | TRGSEL_T0);
                    }
                    else if (adc_setup->int_adc_mode & ADC_TRIGGERED_TIM1) {
                        ulMode = (TRGEN | TRGSEL_T1);
                    }
                    else {
                        ulMode = (TRGEN | TRGSEL_T2);
                    }
                }
                if (adc_setup->int_adc_mode & ADC_8_BIT_MODE) {
                    ulMode |= LOWRES;                                    // set 8 bit mode as opposed to 10 bit mode
                }
                if (adc_setup->int_adc_mode & ADC_AUTO_POWER_DOWN_MODE) {
                    ulMode |= SLEEP;                                     // automatically sleep between conversions
                }
                ADC_MR = (ulMode | adc_setup->int_adc_speed);
            }
            if (adc_setup->int_adc_mode & ADC_CONFIGURE_CHANNEL) {
                if (adc_setup->int_adc_mode & ADC_DISABLE_CHANNEL) {
                    ADC_IDR = adc_setup->int_adc_bits;                   // disable interrupts from specified channel(s)
                    ADC_CHDR = ~adc_setup->int_adc_bits;                 // disable the specified channel(s)
    #ifdef _WINDOWS
                    ADC_CHSR &= ~ADC_CHDR;
                    ADC_CHDR = 0;
    #endif
                }
                else {
                    //volatile unsigned long ulDummy = ADC_SR;             // read status register to clear pending interrupts
    #ifdef _WINDOWS
                    ADC_SR = (ADC_ENDRX | ADC_RXBUFF);
    #endif
                    if (adc_setup->int_adc_int_type & ADC_DISABLE_INTS) {
                        ADC_IDR = adc_setup->int_adc_bits;               // disable interrupts from specified channel(s)
    #ifdef _WINDOWS
                        ADC_IMR &= ~ADC_IDR;
                        ADC_IDR = 0;
    #endif
                        break;
                    }
                    else if (adc_setup->int_adc_mode & ADC_LOOP_MODE) {
                        if (adc_setup->int_adc_result != 0) {            // sampling buffer supplied
                            _adc_handler = adc_setup->int_handler;       // enter the user handler
                            fnEnterAVRInterrupt(IR_GROUP_ADC, adc_setup->int_priority, (void (*)(void))_adc_sequence_complete);
/*
                                                                         // set up peripheral DMA channel
                            ADC_RPR = (unsigned long)adc_setup->int_adc_result; // transfer start location
                            ADC_RCR = adc_setup->int_samples;            // transfer count
                            ADC_PTCR = PDC_RXTEN;                        // enable transfer
                            ADC_IER = ADC_ENDRX;                         // enable interrupt on transfer complete
    #ifdef _WINDOWS
                            ADC_IMR |= ADC_IER;                          // set mask register accordingly
                            ADC_IER = 0;
                            ADC_PTSR |= PDC_RXTEN;
    #endif*/
                        }
                        if ((adc_setup->int_adc_int_type & (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE | ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) && (adc_setup->int_adc_bits != 0)) {
                            int iChannel = 0;
                            unsigned char ucBit = 0x01;
                            while (!(adc_setup->int_adc_bits & ucBit)) {
                                iChannel++;
                                ucBit <<= 1;
                            }
                            ucADCmode[iChannel] = (adc_setup->int_adc_int_type & (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE | ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT | ADC_SINGLE_SHOT_TRIGGER_INT | ADC_SINGLE_SHOT_CROSSING_INT));
                            if (adc_setup->int_adc_int_type & (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE)) {
                                ucLevels[iChannel][ZERO_CROSS_VALUE] = adc_setup->int_zero_crossing_trigger;
                            }
                            if (adc_setup->int_adc_int_type & ADC_LOW_LIMIT_INT) {
                                ucLevels[iChannel][LOW_LEVEL_VALUE] = adc_setup->int_low_level_trigger;
                            }
                            if (adc_setup->int_adc_int_type & ADC_HIGH_LIMIT_INT) {
                                ucLevels[iChannel][HIGH_LEVEL_VALUE] = adc_setup->int_high_level_trigger;
                            }
                            ucPreviousLevels[iChannel] = 0xffff;         // mark that at least one sample value is needed before triggering starts
                        }
                        if (adc_setup->int_adc_mode & (ADC_TRIGGERED_TIM0 | ADC_TRIGGERED_TIM1 | ADC_TRIGGERED_TIM2)) { // timer driven
                            volatile unsigned long *ptrTimeBlock;
                            if (adc_setup->int_adc_mode & ADC_TRIGGERED_TIM0) {
                                adc_trigger_source = TC0;
                                ptrTimeBlock = (unsigned long *)TIMER0_PERIPHERAL_BLOCK;
                            }
                            else if (adc_setup->int_adc_mode & ADC_TRIGGERED_TIM1) {
                                adc_trigger_source = TC1;
                                ptrTimeBlock = (unsigned long *)TIMER1_PERIPHERAL_BLOCK;
                            }
                            else {
                                adc_trigger_source = TC2;
                                ptrTimeBlock = (unsigned long *)TIMER2_PERIPHERAL_BLOCK;
                            }
                            POWER_UP(PBA, PBAMASK_TC);                   // ensure clocks are enable to timer
                            *ptrTimeBlock = (TIM_CLKDIS | TIM_SWTRG);    // disable timer clock and reset timer in TC_CCR_X
                            *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_5 | WAVE | WAVSEL_UP_TRG_RC | ACPC_RC_TOGGLE /*| CPCSTOP | CPCDIS*/);// MCK/1024, wave mode, trigger on register RC, toggle TIOA on match
                            *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (((MASTER_CLOCK/1024/2)/(adc_setup->int_adc_sample_rate))); // set match value in TC_RC_X
                            *ptrTimeBlock = (TIM_CLKEN | TIM_SWTRG);     // start timer by writing to TC_CCR_X
                            *(ptrTimeBlock + (0x24/sizeof(unsigned long))) = TIM_CPCS; // enable interrupt on RC Compare Interrupt in TC_IER_X
    #ifdef _WINDOWS
                            *(ptrTimeBlock + (0x20/sizeof(unsigned long))) |= TIM_CLKSTA;
                            *(ptrTimeBlock + (0x2c/sizeof(unsigned long))) |= TIM_CPCS; // flag that the timer is running (SR and IMR)
    #endif
                        }
                        else {                                           // external hardware trigger input
                            adc_trigger_source = 0;
                        }
                    }
                    if ((adc_setup->int_adc_int_type & (ADC_END_OF_SCAN_INT)) || (adc_setup->int_adc_int_type & (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE | ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT))) {
                        _adc_handler = adc_setup->int_handler;           // enter the user handler
                        fnEnterAVRInterrupt(IR_GROUP_ADC, adc_setup->int_priority, (void (*)(void))_adc_sample_complete);
                        if ((adc_setup->int_adc_int_type & (ADC_SINGLE_SHOT_TRIGGER_INT | ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE | ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) && (adc_setup->int_adc_bits != 0)) { // single interrupt for all channels
                            unsigned char ucBits = 0x80;
                            while (!((adc_setup->int_adc_bits | ADC_IMR) & ucBits)) {
                                ucBits >>= 1;
                            }
                            ADC_IDR = 0xff;
                            ADC_IER = ucBits;                            // interrupt on last used channel
                        }
                        else {
                            ADC_IER = adc_setup->int_adc_bits;           // interrupt for each channel
                        }
    #ifdef _WINDOWS
                        ADC_IMR &= ADC_IDR; ADC_IDR = 0;
                        ADC_IMR |= ADC_IER; ADC_IER = 0;
    #endif
                    }
                    ADC_CHER = adc_setup->int_adc_bits;                  // enable the specified channel(s)
                    PUERC_0 = ((adc_setup->int_adc_bits & 0xff) << 21);  // ensure pull-up resistors are disabled on ADC inputs
    #ifdef _WINDOWS
                    PUER_0 |= PUERC_0; PUERC_0 = 0;
                    ADC_CHSR |= ADC_CHER; ADC_CHER = 0; _SIM_PORT_CHANGE;
    #endif
                }
            }
            if (adc_setup->int_adc_mode & ADC_START_OPERATION) {         // a software triggered conversion should be started
                ADC_CR = START;                                          // start a conversion
            }
        }
        break;
#endif
    default:
        _EXCEPTION("Attempting configuration of interrupt interface without appropriate support enabled!!");
        break;
    }
}


// This routine switches to low power mode. It is always called with disabled interrupts
//
extern void fnDoLowPower(void)
{
#if !defined _WINDOWS
  //PMC_SCDR = PROCESSOR_CLK_ENABLE; TO DO for AVR32                     // disable the processor clock to enter idle mode
#endif
                                                                         // any enabled interrupt will cause the clock to run and the code to continue
    uEnable_Interrupt();                                                 // enable waiting interrupt routine
}

// Determine the present low power mode that is configured (not yet implemented)
//
extern int fnGetLowPowerMode(void)
{
    return 0;
}

// Set a new low power mode (not yet implemented)
//
extern void fnSetLowPowerMode(int new_lp_mode)
{
}


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    WDT_CTRL = (WATCHDOG_KEY_1 | WATCHDOG_ENABLE | (12 << WATCHDOG_PRE_SHIFT)); // enable watchdog with two writes using the key sequence
    WDT_CTRL = (WATCHDOG_KEY_2 | WATCHDOG_ENABLE | (12 << WATCHDOG_PRE_SHIFT)); // watchdog will fire after about 70ms (this also gives the start-up code enough time to disable the watchdog after the reset, if needed)
#ifdef _WINDOWS
    WDT_CTRL = (WATCHDOG_KEY_2 | WATCHDOG_ENABLE);                       // ensure simulator recognises reset
#else
    while (1) {}                                                         // wait until watchdog fires
#endif
}


#if defined SUPPORT_RTC                                                   // {23}

static void (*rtc_interrupt_handler[6])(void) = {0};                     // RTC interrupt handlers

static unsigned long  ulTotalSeconds = 0;
static unsigned short usYear = 1970;                                     // local counters for maintaining simplified time and date
static unsigned char  ucMonthOfYear = 1;
static unsigned char  ucDayOfMonth = 1;
static unsigned char  ucDayOfWeek = 4;                                   // 1970 was a common year (not leap year) starting on a Thursday
static unsigned char  ucSeconds = 0;
static unsigned char  ucMinutes = 0;
static unsigned char  ucHours = 0;

static int rtc_interrupts = 0;
static unsigned long rtc_alarm = 0;
static CHAR rtc_stopwatch = -1;

// Virtual interupts with compatibility with Coldfire RTCs
//
#define RTC_STOPWATCH     0x00000001                                     // stopwatch timer out interrupt
#define RTC_MINUTE_INT    0x00000002                                     // minute interrupt
#define RTC_ALARM_INT     0x00000004                                     // alarm interrupt
#define RTC_DAY_INT       0x00000008                                     // day interrupt
#define RTC_SECOND_INT    0x00000010                                     // second interrupt
#define RTC_HOUR_INT      0x00000020                                     // hour interrupt

#define NTP_TO_1970_TIME 2208988800u
#define LEAP_YEAR_SECONDS (366 * 24 * 60 * 60)
#define NORMAL_YEAR_SECONDS (365 * 24 * 60 * 60)
#define LEAP_YEAR(year) ((year%4)==0)                                    // valid from 1970 to 2038
static const unsigned char monthDays[] = {
    (31),
    (28),                                                                // February without leap day
    (31),
    (30),
    (31),
    (30),
    (31),
    (31),
    (30),
    (31),
    (30),
    (31)
};

// Add week days and keep track of the day of the week (0..6 for Sunday, Monday, ... Saturday)
//
static unsigned char fnIncDayOfWeek(unsigned char ucDayOfWeek, unsigned char ucInc)
{
    ucDayOfWeek += ucInc;
    while (ucDayOfWeek > 6) {
        ucDayOfWeek -= 7;
    }
    return ucDayOfWeek;
}

// Second interrupt
//
#if defined SUPPORT_EXTERNAL_INTERRUPTS
static void _rtc_handler(void)
#else
static __interrupt void _rtc_handler(void)
#endif
{
    RTC_ICR = RTC_TOPI;                                                  // reset interrupt flag 
#if defined _WINDOWS
    RTC_ISR = 0;
    IRR1 &= ~(IR_REAL_TIME_COUNTER);
#endif
    ulTotalSeconds++;      
    if (++ucSeconds >= 60) {                                             // maintain simplified time counters along with the RTC's seconds counter
        ucSeconds = 0;
        if (++ucMinutes >= 60) {
            ucMinutes = 0;
            if (++ucHours >= 24) {
                ucHours = 0;
                ++ucDayOfMonth;
                ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, 1);
                if ((ucDayOfMonth == 29) && (ucMonthOfYear == 2)) {      // 29th Februar
                    if (!(LEAP_YEAR(usYear))) {
                        ucDayOfMonth = 1;
                        ucMonthOfYear = 3;
                    }
                    else {
                        if (ucDayOfMonth > monthDays[ucMonthOfYear - 1]) {
                            ucDayOfMonth = 1;
                            ucMonthOfYear++;
                            if (ucMonthOfYear > 12) {
                                ucMonthOfYear = 1;                       // roll over to new year
                                usYear++;
                            }
                        }
                    }
                }
                if (rtc_interrupts & RTC_DAY_INT) {                      // days interrupt enabled
                    if (rtc_interrupt_handler[3] != 0) {                 // days interrupt call back
                        uDisable_Interrupt();
                        rtc_interrupt_handler[3]();                      // call handling function (minutes interrupt)
                        uEnable_Interrupt();
                    }
                }
            }
            if (rtc_interrupts & RTC_HOUR_INT) {                         // hours interrupt enabled
                if (rtc_interrupt_handler[5] != 0) {                     // hours interrupt call back
                    uDisable_Interrupt();
                    rtc_interrupt_handler[5]();                          // call handling function (minutes interrupt)
                    uEnable_Interrupt();
                }
            }
        }
        if (rtc_interrupts & RTC_MINUTE_INT) {                           // minutes interrupt enabled
            if (rtc_interrupt_handler[1] != 0) {                         // minutes interrupt call back
                uDisable_Interrupt();
                rtc_interrupt_handler[1]();                              // call handling function (minutes interrupt)
                uEnable_Interrupt();
            }
        }
        if (rtc_stopwatch >= 0) {
            rtc_stopwatch--;
            if (rtc_interrupts & RTC_STOPWATCH) {                        // stopwatch interrupt enabled
                if (rtc_stopwatch == -1) {
                    if (rtc_interrupt_handler[0] != 0) {                 // stopwatch interrupt call back
                        uDisable_Interrupt();
                        rtc_interrupt_handler[0]();                      // call handling function (minutes interrupt)
                        uEnable_Interrupt();
                    }
                }
            }
        }
    }
    if (rtc_interrupt_handler[4] != 0) {                                 // seconds interrupt call back
        uDisable_Interrupt();
        rtc_interrupt_handler[4]();                                      // call handling function (seconds interrupt)
        uEnable_Interrupt();
    }
    if (rtc_interrupts & RTC_ALARM_INT) {
        if (ulTotalSeconds == rtc_alarm) {                               // alarm date / time match
            if (rtc_interrupt_handler[2] != 0) {
                uDisable_Interrupt();
                rtc_interrupt_handler[2]();                              // call handling function (alarm match interrupt)
                uEnable_Interrupt();
            }
        }
    }
}


// Convert seconds from 1970 to present time and date
//
static void fnConvertSecondsTime(unsigned long ulSecondsTime)
{
    int iLeapYear = 0;
    ucMonthOfYear = 0;
    ucDayOfMonth = 1;
    ucDayOfWeek = 4;                                                     // 1970 was a common year (not leap year) starting on a Thursday
    ucSeconds = 0;
    ucMinutes = 0;
    ucHours = 0;
    usYear = 1970;
    while (1) {
        iLeapYear = LEAP_YEAR(usYear);
        if (iLeapYear != 0) {                                            // if present year is aleap year
            if (ulSecondsTime > LEAP_YEAR_SECONDS) {
                ulSecondsTime -= LEAP_YEAR_SECONDS;
                usYear++;                                                // count the years past
                ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, 2);
                continue;
            }
        }
        else {                                                           // common year
            if (ulSecondsTime > NORMAL_YEAR_SECONDS) {
                ulSecondsTime -= NORMAL_YEAR_SECONDS;
                usYear++;                                                // count the years past
                ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, 1);
                continue;
            }
        }

        // In final year (remaining seconds value is in the year)
        //
        while (1) {
            if ((iLeapYear != 0) && (ucMonthOfYear == 1)) {              // February of a leap year
                if (ulSecondsTime > (29 * 24 * 60 * 60)) {
                    ulSecondsTime -= (29 * 24 * 60 * 60);
                    ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, 29);
                    ucMonthOfYear++;
                    continue;
                }
            }
            else {
                if (ulSecondsTime > ((unsigned long)monthDays[ucMonthOfYear] * 24 * 60 * 60)) {
                    ulSecondsTime -= ((unsigned long)monthDays[ucMonthOfYear] * 24 * 60 * 60);
                    ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, monthDays[ucMonthOfYear]);
                    ucMonthOfYear++;
                    continue;
                }
            }

            // In final month (remaining seconds value is in the month)
            //
            ucMonthOfYear++;                                             // change from index to month (1..12)
            while (1) {
                if (ulSecondsTime > (60 * 60 * 24)) {
                    ulSecondsTime -= (60 * 60 * 24);                     // for each day in month
                    ucDayOfMonth++;
                    ucDayOfWeek = fnIncDayOfWeek(ucDayOfWeek, 1);
                    continue;
                }

                // In final day (remaining seconds value is in the day)
                //
                while (1) {
                    if (ulSecondsTime > (60 * 60)) {
                        ulSecondsTime -= (60 * 60);                      // for each hour in day
                        ucHours++;
                        continue;
                    }

                    // In final hour (remaining seconds value is in the hour)
                    //
                    while (1) {
                        if (ulSecondsTime > (60)) {
                            ulSecondsTime -= (60);                       // for each minute in hour
                            ucMinutes++;
                            continue;
                        }

                        // In final minute (remaining seconds can be used directly)
                        //
                        ucSeconds = (unsigned char)ulSecondsTime;
                        return;                                          // conversion completed
                    }
                }
            }
        }
    }
}

// Convert time and date to seconds since 1970
//
static unsigned long fnConvertTimeSeconds(RTC_SETUP *ptr_rtc_setup, int iSetTime)
{
    unsigned long ulPassedSeconds = 0;
    unsigned long ulUnit = 1970;
    unsigned char ucNewWeekDay = 4;
    int iLeapYear;
    while (ulUnit < ptr_rtc_setup->usYear) {                             // for each passed year
        iLeapYear = LEAP_YEAR(ulUnit++);
        if (iLeapYear != 0) {
            ulPassedSeconds += LEAP_YEAR_SECONDS;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 2);
        }
        else {
            ulPassedSeconds += NORMAL_YEAR_SECONDS;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
        }
    }
    iLeapYear = LEAP_YEAR(ulUnit);                                       // check whether present year is a leap year
    ulUnit = (ptr_rtc_setup->ucMonthOfYear - 1);                         // the full months passed in this year
    while (ulUnit != 0) {
        ulPassedSeconds += ((unsigned long)(monthDays[ulUnit - 1]) * (60 * 60 * 24)); // count seconds in full months passed
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, monthDays[ulUnit-- - 1]);
    }
    if ((iLeapYear != 0) && (ptr_rtc_setup->ucMonthOfYear > 2))  {       // leap year and data past February so count leap day
        ulPassedSeconds += (60 * 60 * 24);
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
    }
    ulUnit = (ptr_rtc_setup->ucDayOfMonth - 1);                          // passed days in month
    ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, (unsigned char)ulUnit);
    while (ulUnit--) {
        ulPassedSeconds += (60 * 60 * 24);                               // count passed days in present month
    }
    ulUnit = ptr_rtc_setup->ucHours;                                     // passed hours in day
    while (ulUnit--) {
        ulPassedSeconds += (60 * 60);                                    // count passed hours in present day
    }
    ulUnit = ptr_rtc_setup->ucMinutes;                                   // passed minutes in day
    while (ulUnit--) {
        ulPassedSeconds += (60);                                         // count passed minutes in present hour
    }
    ulPassedSeconds += ptr_rtc_setup->ucSeconds;                         // finally add the present seconds count
    if (iSetTime != 0) {
        usYear = ptr_rtc_setup->usYear;                                  // set the new time
        ucMonthOfYear = ptr_rtc_setup->ucMonthOfYear;
        ucDayOfMonth = ptr_rtc_setup->ucDayOfMonth;
        ucHours = ptr_rtc_setup->ucHours;
        ucMinutes = ptr_rtc_setup->ucMinutes;
        ucSeconds = ptr_rtc_setup->ucSeconds;
        ucDayOfWeek = ucNewWeekDay;
    }
    return ulPassedSeconds;
}


// This routine attempts a high level of compatibility with the Coldfire V2 RTC - it presently handles only seconds interrupt
//
extern int fnConfigureRTC(void *ptrSettings)
{
    int iIRQ = 0;
    RTC_SETUP *ptr_rtc_setup = (RTC_SETUP *)ptrSettings;
    unsigned long ulCntr;
    if (ptr_rtc_setup->command & RTC_INITIALISATION) {
        POWER_UP(PBA, PBAMASK_RM_RTC_EIC);                               // ensure the RTC is powered
        RTC_CTRL = RTC_CTRL_CLKEN;                                       // ensure stopped but enable clock to module
        if (ptr_rtc_setup->command & RTC_32K) {                          // clock from 32kHz
            RTC_TOP = 32768;
            ulCntr = (RTC_CTRL_CLKEN | RTC_CTRL_EN | RTC_CTRL_CLK32);
        }
        else {                                                           // else use slow clock (115kHz RC oscillator)
            RTC_TOP = SLOW_CLOCK;
            ulCntr = (RTC_CTRL_CLKEN | RTC_CTRL_EN);
        }
    }
    else {
        ulCntr = RTC_CTRL;
    }
    switch (ptr_rtc_setup->command & ~(RTC_DISABLE | RTC_INITIALISATION | RTC_32K)) {
    case RTC_TIME_SETTING:                                               // set time to RTC
        ulTotalSeconds = fnConvertTimeSeconds(ptr_rtc_setup, 1);         // set the present time and date as seconds since 1970
        while (RTC_CTRL & RTC_CTRL_BUSY) {}
        RTC_CTRL = ulCntr;
        break;

    case RTC_GET_TIME:                                                   // get the present time
        do {                                                             // repeat in case seconds increment while copying
            ptr_rtc_setup->ucSeconds = ucSeconds;
            ptr_rtc_setup->usYear = usYear;
            ptr_rtc_setup->ucMonthOfYear = ucMonthOfYear;
            ptr_rtc_setup->ucDayOfMonth = ucDayOfMonth;
            ptr_rtc_setup->ucDayOfWeek = ucDayOfWeek;
            ptr_rtc_setup->ucHours = ucHours;
            ptr_rtc_setup->ucMinutes = ucMinutes;
        } while (ptr_rtc_setup->ucSeconds != ucSeconds);
        break;

    case RTC_TICK_HOUR:                                                  // interrupt on each hour
        iIRQ++;
    case RTC_TICK_SEC:                                                   // interrupt on each second
        iIRQ++;
    case RTC_TICK_DAY:                                                   // interrupt on each day
        iIRQ++;
    case RTC_ALARM_TIME:                                                 // interrupt at specific date and time
        if (iIRQ == 0) {                                                 // RTC_ALARM_TIME
            rtc_interrupts &= ~RTC_ALARM_INT;                            // disable ALARM interrupt
            rtc_alarm = fnConvertTimeSeconds(ptr_rtc_setup, 0);          // set alarm match in seconds
        }
        iIRQ++;
    case RTC_TICK_MIN:                                                   // interrupt each minute
        iIRQ++;
    case RTC_STOPWATCH_GO:                                               // interrupt after a certain number of minutes
        if (iIRQ == 0) {                                                 // RTC_STOPWATCH_GO            
            rtc_stopwatch = (CHAR)((ptr_rtc_setup->ucMinutes - 1) & 0x3f); // set a stop watch interrupt after this many additional minutes (1 to 64 minutes as for the Coldfire)
        }
        if (RTC_DISABLE & (ptr_rtc_setup->command)) {
            rtc_interrupts &= ~(0x01 << iIRQ);                           // disable interrupt
            return 0;                                                    // disable function rather than enable
        }
        POWER_UP(PBA, PBAMASK_RM_RTC_EIC);                               // ensure the RTC is powered
        rtc_interrupt_handler[iIRQ] = ptr_rtc_setup->int_handler;        // enter the handling interrupt
    #if defined SUPPORT_EXTERNAL_INTERRUPTS
        fnEnterAVRInterrupt(IR_GROUP_INTC_RTC_POW_FREQ, PRIORITY_GPIO, (void (*)(void))_EXT_Interrupt); // share RTC interrupt with external interrupt handler
    #else
        fnEnterAVRInterrupt(IR_GROUP_INTC_RTC_POW_FREQ, PRIORITY_GPIO, (void (*)(void))_rtc_handler);
    #endif
        rtc_interrupts |= (0x01 << iIRQ);                                // flag interrupt(s) enabled
        RTC_IER = RTC_TOPI;                                              // enable interrupt
    #if defined _WINDOWS
        RTC_IMR = RTC_TOPI;
        RTC_IER = 0;
    #endif
        while (RTC_CTRL & RTC_CTRL_BUSY) {}
        RTC_CTRL = ulCntr;
        break;
    }
    return 0;
}

    #if defined SDCARD_SUPPORT && defined SUPPORT_FILE_TIME_STAMP
// This routine supports date and time stamping of files and directories. If the information is not available it could return 1
// so that a fixed stamp is set. It is assumed that the RTC in the AVR32 will be used if the support is activated
//
extern int fnGetLocalFileTime(unsigned short *ptr_usCreationTime, unsigned short *ptr_usCreationDate)
{
    unsigned short usSec;
    unsigned short usMinutes;
    unsigned short usHours;
    unsigned short usDayofMonth;
    unsigned short usMonthofYear;
    unsigned short _usYear;

    do {
        usSec = ucSeconds;                                               // get a snap shot from RTC variables
        usMinutes = ucMinutes;
        usHours = ucHours;
        usDayofMonth = ucDayOfMonth;
        usMonthofYear = ucMonthOfYear;
        _usYear = usYear;
    } while (usSec != ucSeconds);                                        // if there was a second overflow while collecting the date repeat (saves disabling interrupts)
    if (usYear < 2011) {                                                 // if the year is invalid report that there is no valid time/date
        return 1;                                                        // utFAT will take fixed timestamp instead
    }
    *ptr_usCreationTime = (usSec | (usMinutes << 5) | (usHours << 11));
    *ptr_usCreationDate = (usDayofMonth | (usMonthofYear << 5) | ((_usYear - 1980) << 9));
    return 0;                                                            // successful
}
    #endif
#endif



#ifdef GLOBAL_HARDWARE_TIMER
static __interrupt void _hwtimer_interrupt(void)
{
    static const unsigned char hw_timer_int_message[HEADER_LENGTH] = { INTERNAL_ROUTE, INTERNAL_ROUTE, TASK_TIMER, INTERRUPT_EVENT, 0 };
    volatile unsigned int dummy;

    dummy = HW_TIMER_SR;                                                 // read the status to reset interrupt
    HW_TIMER_IDR = TIM_CPCS;                                             // disable interrupt
    #ifdef _WINDOWS
    HW_TIMER_SR &= ~TIM_CLKSTA; HW_TIMER_IMR &= ~TIM_CPCS;               // flag that the timer is not running
    #endif
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting message to queue
    fnWrite(INTERNAL_ROUTE, (unsigned char*)hw_timer_int_message, HEADER_LENGTH); // inform the global interrupt task
    iInterruptLevel = 0;                                                 // release
}

// In order to support a global hardware timer a milli-second timer is required.
//
extern CLOCK_LIMIT fnSetHardwareTimer(CLOCK_LIMIT *hw_delay)
{                                                                        // timer set in ms
    CLOCK_LIMIT Adjusted = 0;
    CLOCK_LIMIT NewDelay = *hw_delay;                                    // clock count desired

    uDisable_Interrupt();                                                // protect region against interrupts

    if (HW_TIMER_IMR & TIM_CPCS) {                                       // timer already running
        CLOCK_LIMIT Remaining = (CLOCK_LIMIT)(HW_TIMER_RC - HW_TIMER_CV);// time remaining before next timeout (in clock pulses)
        if (NewDelay >= Remaining) {
            uEnable_Interrupt();                                         // enable interrupt and quit
            return 0;
        }
        HW_TIMER_CCR = TIM_CLKDIS;                                       // freeze the timer and disable interrupt
        Adjusted = (CLOCK_LIMIT)HW_TIMER_CV;                             // note the frozen value
    }
    HW_TIMER_RC = NewDelay;                                              // set reference
    HW_TIMER_CCR = (TIM_CLKEN | TIM_SWTRG);                              // (re)start timer
    HW_TIMER_IER  = TIM_CPCS;                                            // enable interrupt on RC Compare Interrupt
    #ifdef _WINDOWS
    HW_TIMER_SR |= TIM_CLKSTA; HW_TIMER_IMR |= TIM_CPCS;                 // flag that the timer is running
    #endif

    uEnable_Interrupt();                                                 // enable interrupts again

    return Adjusted;
}

#endif

extern void fnDelayLoop(unsigned long ulDelay_us)                        // {27}
{
    #define LOOP_FACTOR 12200000
    volatile unsigned long ulDelay = ((MASTER_CLOCK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay--) {}                                                 // simple loop tuned to perform us timing
}



#if defined (_GNU)                                                       // GCC initialisation of variables
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

extern void __init_gnu_data(void)
{
    unsigned char *ptrData = &__data_start__;
    const unsigned char *ptrFlash = &__data_load_start__;
    unsigned long ulInitDataLength = (&__data_end__ - &__data_start__);
    while (ulInitDataLength--) {                                         // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrData != ptrFlash) {
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength--) {                                     // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }

    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif


// Perform very low level AVR32 initialisation - called by the start up code
//
extern void AVR32_LowLevelInit(void)
{
#if !defined RUN_FROM_RC_OSCILLATOR                                      // configure the oscillator
    #if defined _AT32UC3C                                                // {14}
    SCIF_UNLOCK = (SCIF_UNLOCK_KEY | SCIF_OSCCTRL0_SHORT_ADD);           // unlock the SCIF_OSCCTRL0 for following access
        #if CRYSTAL_FREQ < 900000
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_142M | SCIF_OSCCTRL_GAIN_0 | SCIF_OSCCTRL_OSCEN);
        #elif CRYSTAL_FREQ < 3000000
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_71M | SCIF_OSCCTRL_GAIN_1 | SCIF_OSCCTRL_OSCEN);
        #elif CRYSTAL_FREQ < 8000000
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_36M | SCIF_OSCCTRL_GAIN_2 | SCIF_OSCCTRL_OSCEN);
        #else
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_18M | SCIF_OSCCTRL_GAIN_3 | SCIF_OSCCTRL_OSCEN);
        #endif
    while (!(SCIF_PCLKSR & SCIF_PCLKSR_OSC0RDY)) {                       // wait for oscillator to become ready
        #ifdef _WINDOWS
        SCIF_PCLKSR |= SCIF_PCLKSR_OSC0RDY;
        #endif
    }
        #ifdef _WINDOWS
    fnSimPortsAVR(PORTS_AVAILABLE);
        #endif
        #ifdef DISABLE_PLL
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_MCCTRL_SHORT_ADD);                   // unlock the PM_MCCTRL for following access
    PM_MCCTRL = MCSEL_OSC0;                                              // select oscillator 0 as main source
        #endif
    #else
        #if CRYSTAL_FREQ < 900000
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_400_900KHZ | OSC_STARTUP_TIME);
        #else
            #if CRYSTAL_FREQ < 3000000
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_900_3000KHZ | OSC_STARTUP_TIME);
            #else
                #if CRYSTAL_FREQ < 8000000
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_3_8MHZ | OSC_STARTUP_TIME);
                #else
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_ABOVE_8MHZ | OSC_STARTUP_TIME);
                #endif
            #endif
        #endif
    PM_MCCTRL = OSC0EN;                                                  // enable the oscillator
    while (!(PM_POSCSR & OSC0RDY)) {                                     // wait until the oscillator has stabilised
        #ifdef _WINDOWS
        fnSimPortsAVR(PORTS_AVAILABLE);                                  // enter oscillator mapping
        PM_POSCSR = OSC0RDY;
        #endif
    }
        #if defined DISABLE_PLL
    PM_MCCTRL = (OSC0EN | MCSEL_OSC0);                                   // switch to oscillator clock
        #endif
    #endif
#endif
#if !defined DISABLE_PLL && !defined RUN_FROM_RC_OSCILLATOR
    #if defined _AT32UC3C                                                // {14}
    SCIF_UNLOCK = (SCIF_UNLOCK_KEY | SCIF_PLL0_SHORT_ADD);               // unlock the SCIF_PLL0 for following access
    SCIF_PLL0 = (SCIF_PLL_PLLOSC_0 | SCIF_PLL_PLLEN | PLL_SETTING);      // configure PLL0    
    while (!(SCIF_PCLKSR & SCIF_PCLKSR_PLL0_LOCK)) {                     // wait for PLL to lock
        #ifdef _WINDOWS
        SCIF_PCLKSR |= SCIF_PCLKSR_PLL0_LOCK;
        #endif
    }
        #if MASTER_CLOCK > MAX_CLOCK_WITHOUT_FLASH_WAITSTATE
    FC_FCR = FWS_ONE_WAIT_STATE;                                         // set 1 wait state for FLASH accesses
        #endif
    // Configure the various bus clocks
    //
        #if CPU_HSB_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_CPUSEL_SHORT_ADD);                   // unlock the PM_CPUSEL for following access
    PM_CPUSEL = (PM_CLKSEL_CPUDIV | CPU_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if HSB_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_HSBSEL_SHORT_ADD);                   // unlock the PM_HSBSEL for following access
    PM_HSBSEL = (PM_CLKSEL_CPUDIV | HSB_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if PBB_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_PBBSEL_SHORT_ADD);                   // unlock the PM_PBBSEL for following access
    PM_PBBSEL = (PM_CLKSEL_CPUDIV | PBB_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if PBA_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_PBASEL_SHORT_ADD);                   // unlock the PM_PBASEL for following access
    PM_PBASEL = (PM_CLKSEL_CPUDIV | PBA_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if PBC_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_PBCSEL_SHORT_ADD);                   // unlock the PM_PBCSEL for following access
    PM_PBCSEL = (PM_CLKSEL_CPUDIV | PBC_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH controller is ready to receive the following command
        #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
        #endif
    }
        #if MASTER_CLOCK > MIN_CLOCK_WITH_HIGHSPEED_READ_MODE
    FC_CMD = FCMD_HSEN;                                                  // execute high speed enable command
        #else
    FC_CMD = FCMD_HSDIS;                                                 // execute high speed disable command    
        #endif
    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH ready again
        #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
        #endif
    }
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_MCCTRL_SHORT_ADD);                   // unlock the PM_MCCTRL for following access
    PM_MCCTRL = MCSEL_PLL0;                                              // select PLL 0 as main source
    #else
    PM_PLL0 = (PLLEN | PLLOSC_SOURCE_0 | PLL_SETTING);                   // configure and enable PLL 0
    while (!(PM_POSCSR & LOCK0)) {                                       // wait for PLL to lock
        #ifdef _WINDOWS
        PM_POSCSR |= LOCK0;
        #endif
    }
        #if MASTER_CLOCK > MAX_CLOCK_WITHOUT_FLASH_WAITSTATE             // add one wait state to FLASH accesses when the CPU is operating above the single-access max. speed
    FC_FCR = FWS_ONE_WAIT_STATE;
        #endif
    PM_CKSEL = (BUS_DIVIDES);                                            // set CPU and the internal bus speeds (this write clears the CKRDY flag)
    while (!(PM_POSCSR & CKRDY)) {
        #ifdef _WINDOWS
        PM_POSCSR |= CKRDY;
        #endif
    }
    PM_MCCTRL |= MCSEL_PLL0;                                             // switch to PLL output for CPU clock
    #endif
#else
    #if BUS_DIVIDES != 0
    PM_CKSEL = (BUS_DIVIDES);                                            // set CPU and the internal bus speeds (this write clears the CKRDY flag)
    while (!(PM_POSCSR & CKRDY)) {
        #ifdef _WINDOWS
        PM_POSCSR |= CKRDY;
        #endif
    }
    #endif
#endif
#if defined _AT32UC3C                                                    // {14}
    POWER_DOWN(PBA, (PBAMASK_FCIF | PBAMASK_AST | PBAMASK_WDT | PBAMASK_EIC | PBAMASK_FREQM  | PBAMASK_USART0 | PBAMASK_USART2 | PBAMASK_USART3 | PBAMASK_SPI1 | PBAMASK_TWIM0 | PBAMASK_TWIM1 | PBAMASK_TWIS0 | PBAMASK_TWIS1 | PBAMASK_IISC | PBAMASK_PWM | PBAMASK_QDEC0 | PBAMASK_QDEC1 | PBAMASK_TC1 | PBAMASK_PEVC | PBAMASK_ACIFA0 | PBAMASK_ACIFA1 | PBAMASK_DACIFB0 | PBAMASK_DACIFB1 | PBAMASK_AW)); // disable all peripheral A clocks by default, except from the interrupt controller, GPIO and power management
    POWER_DOWN(PBB, (PBBMASK_USBC | PBBMASK_SAU | PBBMASK_SMC | PBBMASK_SDRAMC | PBBMASK_MACB));// disable all peripheral B clocks by default, apart from the FLASH controller and HSB matrix
    POWER_DOWN(PBC, (PBCMASK_PDCA | PBCMASK_MDMA | PBCMASK_USART1 | PBCMASK_SPI0 | PBCMASK_CANIF | PBCMASK_TC0 | PBCMASK_ADCIFA | PBCMASK_USART4 | PBCMASK_TWIM2 | PBCMASK_TWIS2));// disable all peripheral C clocks by default
    POWER_DOWN(HSB, (HSBMASK_SAU | HSBMASK_PDCA | HSBMASK_MDMA | HSBMASK_USBC | HSBMASK_CANIF | HSBMASK_EBI | HSBMASK_MACB | HSBMASK_PEVC)); // disable all HSB clocks apart from to the FLASH controller, RAM and bridges
#else
    POWER_DOWN(PBA, (PBAMASK_PDCA | PBAMASK_ADC  | PBAMASK_SPI0 | PBAMASK_SPI1 | PBAMASK_TWI | PBAMASK_USART0 | PBAMASK_USART1 | PBAMASK_USART2 | PBAMASK_USART3 | PBAMASK_PWM | PBAMASK_SSC | PBAMASK_TC | PBAMASK_ABDAC)); // disable all peripheral A clocks by default, except from the interrupt controller, GPIO and power management
    POWER_DOWN(PBB, (PBBMASK_USBB | PBBMASK_MACB | PBBMASK_SMC  | PBBMASK_SDRAMC));// disable all peripheral B clocks by default, apart from the FLASH controller and HSB matrix
    POWER_DOWN(HSB, (HSBMASK_USBB | HSBMASK_MACB | HSBMASK_PDCA | HSBMASK_EBI)); // disable all HSB clocks apart from to the FLASH controller and bridges
#endif
#if defined (_GNU)
    __init_gnu_data();                                                   // initialise variables in SRAM
    main();                                                              // main and never return
#endif
}
#endif

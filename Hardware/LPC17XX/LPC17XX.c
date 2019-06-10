/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC17XX.c
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    22.10.2011 Add return value to fnConfigureRTC() for compatibility    {1}
    23.10.2011 Add fnStartBreak() and fnStopBreak()                      {2}
    13.11.2011 Correct managed file delete when using SPI Flash          {3}
    16.11.2011 Force GCC to link the reset vectors (newer GCC versions optimise them out) {4}
    17.11.2011 Add LPC1788 support                                       {5}
    17.11.2011 Add ETHERNET_RELEASE_AFTER_EVERY_FRAME support            {6}
    17.11.2011 Modify RTC support                                        {7}
    18.11.2011 Correct managed file delete size with zero length         {8}
    21.11.2011 New peripheral clock configuration                        {9}
    30.11.2011 Ensure LPC1788 power boost is enabled when operating at 100MHz and above {10}
    20.03.2012 Add return to fnConfigEthernet()                          {11}
    12.09.2012 Modify fnGetFlashSectorSize() to fnGetFlashSectorRestSize() {12}
    06.10.2012 Allow periodic timer interrupt as well as mono-stable     {13}
    06.10.2012 Add ADC                                                   {14}
    07.10.2012 Add SD card interface                                     {15}
    07.10.2012 Add fnDelayLoop()                                         {16}
    07.10.2012 Adapted port interrupts to LPC1788                        {17}
    07.10.2012 Use link-local IPv6 address for multicast configuration   {18}
    07.10.2012 Add revised Flash handing as defined by STORAGE_REV_1     {19}
    15.11.2012 Add LPC1788 USB configuration                             {20}
    14.05.2014 Add optional LPC1758 Ethernet errata workaround           {21}

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#ifdef _WINDOWS
    #include "config.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    #define START_CODE 0
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #if defined _COMPILE_KEIL
        extern void __main(void);                                        // Keil library initialisation routine
        #define START_CODE __main
    #elif defined COMPILE_IAR5
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE __iar_program_start
        #define _main main
    #elif defined ROWLEY && !defined ROWLEY_2                            // if project uses Rowley < V1.7 build 17
        #define START_CODE _main2
    #else
        #define START_CODE _main
    #endif
#endif

#ifdef _LPC17XX

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define SPI_FLASH_ENABLED                                            // SPI driver required
#endif

#define __interrupt

// USB requires that the APB clock be at least 18MHz. By default it is MCLK/4
//
#if defined USB_INTERFACE && (MASTER_CLOCK/4 < 18000000)
    #if MASTER_CLOCK/2 < 18000000
        #if MASTER_CLOCK < 18000000
            #error Clock is too slow for USB use!
        #else
            #define APBCLOCK_DIVIDE  APBCLOCK_1                          // APB clock equal to master clock
            #define APB_DIV     1
        #endif
    #else
        #define APBCLOCK_DIVIDE  APBCLOCK_2;                             // APB clock equal to half the master clock
        #define APB_DIV     2
    #endif
#endif 


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
    #if defined SPI_FLASH_ENABLED
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_lpc_atmel.h"
            #include "spi_flash_lpc_stmicro.h"
            #include "spi_flash_lpc_sst25.h"
        #undef _SPI_DEFINES
    #endif
#endif

static void _LowLevelInit(void);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined LPC1758_ETHERNET_WORKAROUND                              // {21}
    static int iTxFrames = 0;                                        // count to transmitted frames
#endif
#ifdef RANDOM_NUMBER_GENERATOR
    unsigned short *ptrSeed;
#endif
#if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA
    static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)]; // FLASH row backup buffer (on word boundary)
    static unsigned char *ucFlashRow = (unsigned char *)ulFlashRow;      // pointer to array of bytes
#endif
#if defined SPI_FLASH_ENABLED
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

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
        #if defined STORAGE_REV_1                                        // {19}
            #define _EXTENDED_CS  ptrAccessDetails->ucDeviceNumber,
        #else
            #define _EXTENDED_CS  iChipSelect,
        #endif
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif
#if defined ETH_INTERFACE && defined _WINDOWS
    static unsigned short usPhyMode = 0;
#endif
static int iInterruptLevel = 0;

#if defined STORAGE_REV_1 && !defined ONLY_INTERNAL_FLASH_STORAGE        // {19}
    #if defined EXT_FLASH_FILE_SYSTEM                                    // devices without internal flash - if external Flash enabled put default to it
    static const STORAGE_AREA_ENTRY default_flash = {           
        0,                                                               // end of list
        (unsigned char *)(EXTERNAL_FLASH_START_ADDRESS),                 // start address of external flash
        (unsigned char *)(EXTERNAL_FLASH_START_ADDRESS + (SIZE_OF_EXTERNAL_FLASH - 1)), // end of external flash
        _STORAGE_PARALLEL_FLASH,                                         // type
        EXTERNAL_FLASH_DEVICE_COUNT                                      // number of devices
    };
    #else
    static const STORAGE_AREA_ENTRY default_flash = {
        0,                                                               // end of list
        (unsigned char *)(FLASH_START_ADDRESS),                          // start address of internal flash
        (unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH - 1)),    // end of internal flash
        _STORAGE_INTERNAL_FLASH,                                         // type
        0                                                                // not multiple devices
    };
    #endif

    STORAGE_AREA_ENTRY *UserStorageListPtr = (STORAGE_AREA_ENTRY *)&default_flash; // default entry
#endif

#if (defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES) && defined _WINDOWS
    extern int iFetchingInternalMemory = 0;
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */
#ifdef ETH_INTERFACE
    extern ETHERNETQue *eth_tx_control;                                  // Ethernet structure pointers for Ethernet Interrupt handling
#endif
#ifdef IIC_INTERFACE
    extern IICQue *IIC_rx_control[NUMBER_IIC];
    extern IICQue *IIC_tx_control[NUMBER_IIC];
#endif

#ifdef _WINDOWS                              
    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#ifndef _WINDOWS
    static void fnInitHW(void);
#endif


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

#ifndef _WINDOWS
    extern __interrupt void _start(void);                                // reset vector location
#endif


/* =================================================================== */
/*                      STARTUP CODE                                   */
/* =================================================================== */


// This is the interrupt vector table for the processor including the reset vector (entrypoint)
// It is at a fixed address
typedef void (*tIsrFunc)(void);

#ifndef _WINDOWS
    extern void __segment_init(void);
    #if defined  COMPILE_IAR
        #if defined COMPILE_IAR5
            #pragma segment=".data"
            #pragma segment=".bss"
            static unsigned char *ptrTopOfUsedMemory = 0;
            #define HEAP_START_ADDRESS ptrTopOfUsedMemory                // IAR5 compiler - last location of static variables
        #else
            #pragma segment="DATA_Z"
            #define HEAP_START_ADDRESS __segment_end("DATA_Z")           // IAR compiler - last location of static variables
        #endif
        extern void __disable_interrupt(void);
        extern void __enable_interrupt(void);
        extern void __sleep_mode(void);
    #elif defined _COMPILE_KEIL
        #define HEAP_START_ADDRESS    _keil_ram_size()
        #define __disable_interrupt() __disable_irq()                    // KEIL intrinsics
        #define __enable_interrupt()  __enable_irq()
      //#define __sleep_mode()                                           //__asm("wfi"); Keil doesn't support inline assembly in Thumb mode so this is defined in the startup assembler file
        extern void __sleep_mode(void);
    #else                                                                // disable interrupt in assembler code
        extern unsigned char __heap_end__;
        #ifdef ROWLEY_2
            #define asm(x) __asm__(x)
        #endif
        #define HEAP_START_ADDRESS &__heap_end__                         // GNU last location of static variables
        #define __disable_interrupt() asm("cpsid   i")
        #define __enable_interrupt()  asm("cpsie   i")
        #define __sleep_mode()        asm("wfi")
    #endif

#ifdef _COMPILE_KEIL

typedef struct stREGION_TABLE
{
    unsigned char *ptrConst;                                             // const variables belonging to block
    unsigned char *ptrDestination;                                       // destination in RAM
    unsigned long  ulLength;                                             // length of the block in SRAM
    unsigned long  ulUnknown;                                            // entry unknown
} REGION_TABLE;

// Calculate the end of used SRAM from the Keil linker information
//
static unsigned char *_keil_ram_size(void)
{
    extern REGION_TABLE Region$$Table$$Base;                            // table location supplied by linker
    extern REGION_TABLE Region$$Table$$Limit;                           // end of table list
    REGION_TABLE *ptrEntries = &Region$$Table$$Base;                    // first block
    unsigned char *ptrRam = ptrEntries->ptrDestination;                 // RAM address
    do {
        ptrRam += ptrEntries->ulLength;                                 // add length
        ptrEntries++;                                                   // move to next block
    } while (ptrEntries != &Region$$Table$$Limit);
    return ptrRam;
}
#endif

#define RANDOM_SEED_LOCATION (unsigned short *)(RAM_START_ADDRESS + SIZE_OF_RAM - IAP_STACK_SIZE - 4) // location of a long word which is never initialised and so has a random power on value

// main entry for the target code
//
extern int _main(void)
{
#ifdef MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart = HEAP_START_ADDRESS;
#endif
#if defined (_BOOT_PROGRAM) && defined (COMPILE_IAR)
    __segment_init();                                                    // initialisation of local variables
#elif defined COMPILE_IAR5
    if (__sfe(".bss") > __sfe(".data")) {                                // set last used SRAM address
        ptrTopOfUsedMemory = __sfe(".bss");
    }
    else {
        ptrTopOfUsedMemory = __sfe(".data");
    }
#endif
    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)
#ifdef RANDOM_NUMBER_GENERATOR
    ptrSeed = RANDOM_SEED_LOCATION;
#endif
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

#if defined _COMPILE_KEIL
// Keil demands the use of a main function with int return value
//
extern int main(void)
{
    _main();
    return 0;
}
#endif

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


// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               //perform hardware initialisation
{
#ifdef _WINDOWS
    unsigned long ulPortPullups[] = {
        PORT0_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_lpc17xx.h
        PORT1_DEFAULT_INPUT,
        PORT2_DEFAULT_INPUT,
        PORT3_DEFAULT_INPUT,
        PORT4_DEFAULT_INPUT,
        PORT5_DEFAULT_INPUT                                              // LPC1788
    };
  #ifdef RANDOM_NUMBER_GENERATOR
    static unsigned short usRandomSeed = 0;                              // note that the target works differently
    ptrSeed = &usRandomSeed;
  #endif
    fnInitialiseDevice(ulPortPullups);
#endif
    _LowLevelInit();
#ifdef _WINDOWS
    fnSimPorts(-1);                                                      // ensure port states are recognised
#endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
#ifndef _DEBUG_CODE_
    INIT_WATCHDOG_DISABLE();
    if (!WATCHDOG_DISABLE()) {
        ACTIVATE_WATCHDOG();                                             // allow user configuration of internal watch dog timer
    }
#endif
#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    #if defined SPI_FLASH_ENABLED
    fnConfigSPIFileSystem(MAX_SPI);                                      // configure SPI interface for maximum speed
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
        #include "spi_flash_lpc_atmel.h"
        #include "spi_flash_lpc_stmicro.h"
        #include "spi_flash_lpc_sst25.h"
    #undef _CHECK_SPI_CHIPS
    #else
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
    #endif
#endif

#if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA
    uMemset(ucFlashRow, 0xff, FLASH_ROW_SIZE);                           // flush the FLASH ROW backup buffer when starting
#endif
    fnUserHWInit();                                                      // allow the user to initialise hardware specific things
#ifdef _WINDOWS
    fnSimPorts(-1);                                                      // ensure port states are recognised
#endif
}


// Support watchdog re-triggering of specific hardware
//
void fnRetriggerWatchdog(void)
{
#ifndef _DEBUG_CODE_
    #ifdef _WINDOWS
    if (WDFEED == 0) return;                                             // during commanded reset with watchdog
    #endif
    uDisable_Interrupt();                                                // protect the feed sequence from interrupts
    WDFEED = 0xaa;                                                       // watchdog feed sequence
    WDFEED = 0x55;
    uEnable_Interrupt();                                                 // flash an LED for visualisation
#endif
    TOGGLE_WATCHDOG_LED();
}

// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)
{
#ifdef _WINDOWS
    ucLPC17xx.CORTEX_M3_REGS.ulPRIMASK = INTERRUPT_MASKED;               // mark that interrupts are masked
#else
    __disable_interrupt();                                               // disable interrupts to core
#endif
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
extern void uEnable_Interrupt(void)
{
#ifdef _WINDOWS
    if (!iInterruptLevel) {
        _EXCEPTION("Unsymmetrical use of interrupt disable/enable detected!!");
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
    }
#endif
    if (!(--iInterruptLevel)) {                                          // only when no more interrupt nesting,
#ifdef _WINDOWS
        ucLPC17xx.CORTEX_M3_REGS.ulPRIMASK = 0;                          // mark that interrupts are not masked
#else
        __enable_interrupt();                                            // enable processor interrupts
#endif
    }
}


// Function used to enter processor interrupts
//
static void fnEnterInterrupt(int iInterruptID, unsigned char ucPriority, void (*InterruptFunc)(void))
{
    unsigned long *ptrIntSet = IRQ0_31_SER_ADD;
    unsigned char *ptrPriority = IRQ0_3_PRIORITY_REGISTER_ADD;
    VECTOR_TABLE *ptrVect;
    void ( **processor_ints )( void );
#ifdef _WINDOWS                                                          // back up the present enabled interrupt registers
    unsigned long ulState0 = IRQ0_31_SER;
    unsigned long ulState1 = IRQ32_63_SER;
    IRQ0_31_SER = IRQ32_63_SER = IRQ0_31_CER = IRQ32_63_CER = 0;         // reset registers
#endif
#ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
#else
    ptrVect = (VECTOR_TABLE *)(NVIC_RAM_START_ADDRESS);
#endif
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;
    processor_ints += iInterruptID;
    *processor_ints = InterruptFunc;

    ptrIntSet += (iInterruptID/32);
    *ptrIntSet = (0x01 << (iInterruptID%32));                            // enable the interrupt

    ptrPriority += iInterruptID;
    *ptrPriority = ucPriority;                                           // define the interrupt's priority
#ifdef _WINDOWS
    IRQ0_31_SER  |= ulState0;                                            // synchronise the interrupt masks
    IRQ32_63_SER |= ulState1;
    IRQ0_31_CER   = IRQ0_31_SER;
    IRQ32_63_CER  = IRQ32_63_SER;
#endif
}




/**************************** TICK interrupt ******************************************/


#if defined COMPILE_IAR && !defined COMPILE_IAR5                         // disable optimising away the reading of the timer (to reset interrupt)
   #pragma optimize=2
#endif
static __interrupt void _RealTimeInterrupt(void)
{
    INT_CONT_STATE_REG = PENDSTCLR;                                      // reset interrupt
#ifdef _WINDOWS
    INT_CONT_STATE_REG &= ~(PENDSTSET | PENDSTCLR);
#endif
    uDisable_Interrupt();                                                // ensure tick handler can not be interrupted
    fnRtmkSystemTick();                                                  // operating system tick
    uEnable_Interrupt();
}


// Routine to initialise the Tick interrupt (uses Cortex M3 SysTick timer)
//
#define REQUIRED_MS ((1000/TICK_RESOLUTION))                             // the TICK frequency we require in kHz
#define TICK_DIVIDE (((MASTER_CLOCK + REQUIRED_MS/2)/REQUIRED_MS) - 1)   // the divide ratio required

extern void fnStartTick(void)
{
    VECTOR_TABLE *ptrVect;
#ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
#else
    ptrVect = (VECTOR_TABLE *)(NVIC_RAM_START_ADDRESS);
#endif
    SYSTICK_RELOAD = TICK_DIVIDE;                                        // set reload value to determine the period
    ptrVect->ptrSysTick = _RealTimeInterrupt;                            // enter interrupt handler
    SYSTEM_HANDLER_12_15_PRIORITY_REGISTER |= (SYSTICK_PRIORITY << 24);  // enter the SYSTICK priority
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE | SYSTICK_TICKINT); // enable timer and its interrupt
#ifdef _WINDOWS
    SYSTICK_RELOAD &= SYSTICK_COUNT_MASK;                                // mask any values which are out of range
    SYSTICK_CURRENT = SYSTICK_RELOAD;                                    // prime the reload count
#endif
#ifdef MONITOR_PERFORMANCE                                               // configure a timer that will be used to measure the duration of task operation
    INITIALISE_MONITOR_TIMER();
#endif
}


#ifdef ETH_INTERFACE
/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */

    #ifndef ETHERNET_RELEASE_AFTER_EVERY_FRAME                           // {6}
static const unsigned char EMAC_RX_int_message[HEADER_LENGTH] = {0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT};   // define fixed interrupt event
    #endif


extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
    #ifdef LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_rx_int_message[HEADER_LENGTH] = {0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_RX_INTERRUPT};
    #endif
    if (EMAC_RX_INTERRUPT == *ucEvent) {
        LPC17XX_RX_STATUS *ptrRxStatus = (LPC17XX_RX_STATUS *)EMAC_RxStatus;
        ptrRxStatus += EMAC_RxConsumeIndex;                              // set present buffer status register

        if (!(ptrRxStatus->ulStatusInfo & RX_LAN_SIZE_MASK)) {
            return -1;                                                   // no complete frame ready
        }
        else {
            rx_frame->frame_size = (unsigned short)((ptrRxStatus->ulStatusInfo & RX_LAN_SIZE_MASK) + 1);
            ptrRxStatus->ulStatusInfo &= ~RX_LAN_SIZE_MASK;
    #ifdef LAN_REPORT_ACTIVITY
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_rx_int_message, HEADER_LENGTH);
    #endif
            if (ptrRxStatus->ulStatusInfo & (RX_LAN_OVERRUN | RX_NO_DESCRIPTOR | ALIGNMENT_ERROR)) {
    #ifdef USE_IP_STATS
                if (ptrRxStatus->ulStatusInfo & (RX_LAN_OVERRUN | RX_NO_DESCRIPTOR)) {
                    fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES);
                }
                else {
                    fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);        // count other unexpected events
                }
    #endif
                fnFreeEthernetBuffer((int)EMAC_RxConsumeIndex);
                return -1;                                               // bad frame
            }
            else {
                LPC17XX_RX_BD *ptrRxBd = (LPC17XX_RX_BD *)EMAC_RxDescriptor;
                ptrRxBd += EMAC_RxConsumeIndex;                          // set present rx consumer buffer descriptor
                rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ptrRxBd->ptrBlock; // return pointer to the fixed linear input buffer
            }
        }
        return (unsigned char)EMAC_RxConsumeIndex;                       // buffer index received from
    }
    #if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME && defined USE_IP_STATS // {6}
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
    #endif
    return -1;                                                           // invalid channel
}



// EMAC interrupt dispatcher
//
static __interrupt void EMAC_Interrupt(void)
{
    while (EMAC_IntStatus & (EMAC_RxOverrunInt | EMAC_RxErrorInt | EMAC_RxFinishedInt | EMAC_RxDoneInt)) { // read the interrupt status register
        if (EMAC_IntStatus & (EMAC_RxFinishedInt | EMAC_RxDoneInt)) {
            EMAC_IntClear = (EMAC_RxOverrunInt | EMAC_RxErrorInt | EMAC_RxFinishedInt | EMAC_RxDoneInt);
    #ifdef ETHERNET_RELEASE_AFTER_EVERY_FRAME                            // {6}
            uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);         // schedule the Ethernet task
    #else
            uDisable_Interrupt();                                        // ensure message can not be interrupted
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_RX_int_message, HEADER_LENGTH); // inform the Ethernet task
            uEnable_Interrupt();                                         // release
    #endif
        }
        else {
            EMAC_IntClear = (EMAC_RxOverrunInt | EMAC_RxErrorInt | EMAC_RxFinishedInt | EMAC_RxDoneInt); // clear the interrupt status bit
        }
    #ifdef _WINDOWS
        EMAC_IntStatus &= ~(EMAC_RxOverrunInt | EMAC_RxErrorInt | EMAC_RxFinishedInt | EMAC_RxDoneInt); // clear interrupt flag when simulating
    #endif
    }
}


static unsigned short fnReadMII(unsigned char ucRegAdr)
{
    MCMD = MCMD_READ;                                                    // prepare read
    MADR = (unsigned long)(ucRegAdr | PHY_ADDRESS_);                     // define address and start

    #ifdef _WINDOWS
    if (DP83848_PHY_IDENTIFIER_1 == ucRegAdr) {                          // simulate reads from identifier register
        MRDD = (PHY_IDENTIFIER>>16);
    }
    else if (DP83848_PHY_IDENTIFIER_2 == ucRegAdr) {
        MRDD = (PHY_IDENTIFIER & 0x0000ffff);
    }
        #ifdef _LAN8700
    else if (LAN8700_INTERRUPT_FLAG == ucRegAdr)
        #else
    else if (DP83848_INTERRUPT_CONTROL_STATUS_REGISTER == ucRegAdr)
        #endif
    {
        #ifdef PHY_INTERRUPT
        unsigned char ucBit = 31;
        unsigned long ulBit = PHY_INT_PIN;
        while (ulBit > 1) {
            ulBit >>= 1;
            ucBit--;
        }
        fnSimulateInputChange(PHY_INT_PORT, ucBit, SET_INPUT);           // remove IRQ when this register is read
        #endif
        #ifdef _LAN8700
        MRDD = PHY_LAN8700_AUTO_NEG_COMPLETE;
        #else
        MRDD = PHY_LINK_UP_INT;
        #endif
    }
    else if (DP83848_100BASETX_PHY_CONTROL_REGISTER == ucRegAdr) {
        MRDD = PHY_OP_MODE_100_BASE_T_FULL_DUPLEX;
    }
    #endif

    while (MIND & MIND_BUSY) {                                           // wait for read to be executed
        if (MIND & MIND_MII_LINK_FAIL) {
            return 0;
        }
    }
    MCMD = 0;
    return (unsigned short)MRDD;                                         // return the value read
}

static int fnWriteMII(unsigned char ucRegAdr, unsigned short usData)
{
    MCMD = 0;                                                            // write access
    MADR = (unsigned long)(ucRegAdr | PHY_ADDRESS_);                     // define address
    MWTD = usData;                                                       // write the data

    #ifdef _WINDOWS
    if (DP83848_BASIC_CONTROL_REGISTER == ucRegAdr) {
        usPhyMode = usData;
    }
    #endif

    while (MIND & MIND_BUSY) {                                           // wait for read to be executed
        if (MIND & MIND_MII_LINK_FAIL) {
            return -1;
        }
    }
    return 0;
}


#ifdef PHY_INTERRUPT
    #if PHY_INT_PORT == PORT_0                                           // it is assumed that the PHY input is only on port 0 or port 2 since these have interrupt capabilities
        #define PHY_INT_INPUT_PORT   FIO0PIN
    #else
        #define PHY_INT_INPUT_PORT   FIO2PIN
    #endif
    #ifdef _LAN8700
        #define READ_INTERRUPT_FLAGS   LAN8700_INTERRUPT_FLAG
        #define LINK_UP_FLAG           PHY_LAN8700_AUTO_NEG_COMPLETE
        #define LINK_DOWN_FLAG         PHY_LAN8700_LINK_DOWN
    #else
        #define READ_INTERRUPT_FLAGS   DP83848_INTERRUPT_CONTROL_STATUS_REGISTER
        #define LINK_UP_FLAG           PHY_LINK_UP_INT
        #define LINK_DOWN_FLAG         PHY_LINK_DOWN_INT
    #endif
static void fnPhyInterrupt(void)
{
    #ifdef INTERRUPT_TASK_PHY
    unsigned char int_phy_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT }; // define standard interrupt event
    #endif

    while (!(PHY_INT_INPUT_PORT & PHY_INT_PIN)) {                        // while the interrupt line remains low
        volatile unsigned short usInterrupt;
        usInterrupt = fnReadMII(READ_INTERRUPT_FLAGS);                   // read the cause(s) of the interrupt, which resets the bits
        if (LINK_UP_FLAG & usInterrupt) {
            usInterrupt = fnReadMII(DP83848_100BASETX_PHY_CONTROL_REGISTER);// check the  details of link
            if (usInterrupt & (PHY_OP_MODE_100_BASE_T_HALF_DUPLEX)) {    // if at least 100 half duplex it indicates a 100MHz link
                SUPP = SUPP_SPEED;                                       // ensure EMAC is set to 100MHz mode
    #ifdef INTERRUPT_TASK_PHY
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
    #endif
            }
            else {
                SUPP = 0;                                                // ensure EMAC is set to 10MHz mode
    #ifdef INTERRUPT_TASK_PHY
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
    #endif
            }
        }
    #ifdef INTERRUPT_TASK_PHY
        else if (LINK_DOWN_FLAG & usInterrupt) {
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
        }
    #endif
    #ifdef INTERRUPT_TASK_PHY
        fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // inform the corresponding task
    #endif
    }
}
#endif

#ifdef USE_IPV6
// Configure a multicast address for IPV6 use
//
extern void fnConfigureMulticastIPV6(void)
{
    #define CRC_POLY 0xedb88320                                          // CRC-32 polynomial in reverse direction
    unsigned long ulCRC32 = 0xffffffff;                                  // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    int i, bit;
    unsigned char ucByte;
	for (i = 0; i < MAC_LENGTH; i++) {
        switch (i) {
        case 0:
        case 1:
            ucByte = 0x33;
            break;
        case 2:
            ucByte = 0xff;
            break;
        default:
            ucByte = ucLinkLocalIPv6Address[i + (IPV6_LENGTH - MAC_LENGTH)]; // {18}
		  //ucByte = network[DEFAULT_NETWORK].ucOurIPV6[i + (IPV6_LENGTH - MAC_LENGTH)];
            break;
        }
        for (bit = 0; bit < 8; bit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
	}
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 detemine the hash entry location

    if (ulCRC32 < 32) {
        EMAC_HashFilterL = (0x00000001 << ulCRC32);                      // enter single group hash entry
    }
    else {
        ulCRC32 -= 32;
        EMAC_HashFilterH = (0x00000001 << ulCRC32);
    }

}
#endif

#if defined SIZE_OF_AHB_RAM
// Memory allocation and alignment in LAN RAM block
//
static void *uMallocLAN_Align(unsigned short __size, unsigned char ucAlign)
{
    #ifdef _WINDOWS
    static unsigned char ucLAN_Memory[ETHERNET_RAM_SIZE];
    static unsigned char *pucBottomOfHeap = ucLAN_Memory;
    static unsigned char *pucTopOfHeap = (ucLAN_Memory + ETHERNET_RAM_SIZE);
    #else
    static unsigned char *pucBottomOfHeap = (unsigned char *)(ETHERNET_RAM_START_ADDRESS);
    static unsigned char *pucTopOfHeap = (unsigned char *)(ETHERNET_RAM_START_ADDRESS + ETHERNET_RAM_SIZE);
    #endif
    static unsigned short present_HeapSize = 0;

    unsigned char *ptr = pucBottomOfHeap + present_HeapSize;
    if ((unsigned long)ptr & (ucAlign - 1)) {                            // ensure all memory is aligned on specified byte boundary
        ptr = (unsigned char *)((unsigned long)ptr & ~(ucAlign - 1));
        ptr += ucAlign;
    }

    if ((ptr + __size) >= pucTopOfHeap) {
        _EXCEPTION("LAN heap allocation failure!!");
        return 0;                                                        // no more room - it will be necessary to increase heap size
    }

    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed

    present_HeapSize = (ptr - pucBottomOfHeap) + __size;
    return ((void *)ptr);
}
#else                                                                    // devices without AHB-lite RAM use standard heap space
    #define uMallocLAN_Align(__size, ucAlign) uMalloc_Align(__size, ucAlign)
#endif


// Ethernet configuration routine
//
extern int fnConfigEthernet(ETHTABLE *pars)                              // {11}
{
#ifdef PHY_INTERRUPT
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
#endif
    unsigned long ulPhyIdentifier;
    unsigned short usMIIData;
    volatile int i;
    LPC17XX_RX_BD *ptrBd;
    unsigned char *ptrBuffer;

/********************************* Check and configure PHY **********************************************/
#ifdef RESET_PHY
    DEASSERT_PHY_RST();                                                  // allow PHY to run and latch in its boot strap options
#endif

    POWER_UP(PCENET);                                                    // enable power to Ethernet controller
#if defined LPC1788
    IOCON_P1_00 = _ENET_P1_0_TXD0;
    IOCON_P1_01 = _ENET_P1_1_TXD1;
    IOCON_P1_04 = _ENET_P1_4_TXEN;
    IOCON_P1_08 = _ENET_P1_8_CRS_DV;
    IOCON_P1_09 = _ENET_P1_9_RXD0;
    IOCON_P1_10 = _ENET_P1_10_RXD1;
    IOCON_P1_14 = _ENET_P1_14_RXER;
    IOCON_P1_15 = _ENET_P1_15_REFCK;
    IOCON_P1_16 = _ENET_P1_16_MDC;
    IOCON_P1_17 = _ENET_P1_17_MDIO;
#else
    PINSEL2 &= ~(PINSEL2_P1_6_ENET_TX_CLK);                              // ensure disabled
    PINSEL2 |= (PINSEL2_P1_0_ENET_TXD0 | PINSEL2_P1_1_ENET_TXD0 | PINSEL2_P1_4_ENET_TX_EN | PINSEL2_P1_8_ENET_CRS | PINSEL2_P1_9_ENET_RXD0 | PINSEL2_P1_10_ENET_RXD1 | PINSEL2_P1_14_ENET_RX_ER | PINSEL2_P1_15_ENET_REF_CLK);
    PINSEL3 |= (PINSEL3_P1_16_ENET_MDC | PINSEL3_P1_17_ENET_MDIO);       // configure RMII pins
#endif
    MAC1     = (SOFT_RESET | SIMULATION_RESET | RESET_TX | RESET_MCS_TX | RESET_RX | RESET_MCS_RX);
    i = 10;
    while (i--) {}                                                       // short delay for reset to complete
    MAC1     = PASS_ALL_RECEIVE_FRAMES;                                  // remove  soft resets
    MAC2     = (CRC_ENABLE | PAD_CRC_ENABLE);
    MAXF     = 1536;                                                     // maximum Ethrenet frame length
    IPGT     = IPGR_TIME;
    CLRT     = DEFAULT_CLRT;
    SUPP     = RESET_RMII;                                               // reset RMII logic
    SUPP     = SUPP_SPEED;                                               // set RMII speed for 100MHz operation
    MCFG     = HOST_CLK_DIV_28;                                          // configure 2.5MHz MDIO clock
    EMAC_COMMAND = (EMAC_RMII | EMAC_PASS_RUNT_FRAME);                   // enable RMII

    fnWriteMII(DP83848_BASIC_CONTROL_REGISTER, PHY_SOFTWARE_RESET);      // command a software reset of PHY
    i = 1000000;
    while (fnReadMII(DP83848_BASIC_CONTROL_REGISTER) & PHY_SOFTWARE_RESET) {// wait until reset completed
        if (--i == 0) {                                                  // quit if no response
            return - 1;                                                  // {11}
        }
    }

    ulPhyIdentifier = fnReadMII(DP83848_PHY_IDENTIFIER_1);               // check that the PHY is working correctly by reading its identifier
    ulPhyIdentifier <<= 16;
    ulPhyIdentifier |= fnReadMII(DP83848_PHY_IDENTIFIER_2);              // check that the PHY is working correctly by reading its identifier

    if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {
        return -1;                                                       // {11} if the identifier is incorrect the phy isn't responding correctly
    }

    usMIIData = 0;                                                       // configure the mode which we could like to use
    if (pars->usMode & (LAN_10M | LAN_100M)) {                           // fixed speed setting
        if (pars->usMode & LAN_100M)  {
            usMIIData |= PHY_SELECT_100M_SPEED;
        }
        else {
            SUPP = 0;                                                    // 10MHz
        }
    }
    else {
        usMIIData |= (PHY_ENABLE_AUTO_NEGOTIATION | PHY_RESTART_AUTO_NEGOTIATION);
    }

    if (pars->usMode & FULL_DUPLEX) {
        usMIIData |= PHY_FULL_DUPLEX_MODE;
        IPGT = FULL_DUPLEX_IPG_TIME;
        MAC2 |= MAC2_FULL_DUPLEX;
        EMAC_COMMAND |= EMAC_FULL_DUPLEX;
    }
    else {
        IPGT = HALF_DUPLEX_IPG_TIME;
    }

    if (pars->usMode & LOOPBACK) {
        usMIIData |= PHY_LOOP_BACK;                                      // test loop back at PHY
      //MAC1 |= (MAC1_LOOPBACK);                                         // test loop back at MII
    }

    fnWriteMII(DP83848_BASIC_CONTROL_REGISTER, usMIIData);               // write the PHY operating mode

    /******************************* MAC buffer descriptor configuration ****************************************/

    i = LAN_RX_BUF_COUNT;                                                // set up receiver buffer memory
    EMAC_RxDescriptorNumber = (LAN_RX_BUF_COUNT - 1);
    ptrBuffer = uMallocLAN_Align((unsigned short)(LAN_RX_BUF_COUNT * pars->usSizeRx), 1); // allocate a receive buffer pool in the LAN RAM
    EMAC_RxStatus = (unsigned long)uMallocLAN_Align((unsigned short)(sizeof(LPC17XX_RX_STATUS) * i), 8); // create a smaller pool of buffer descriptors
    ptrBd = uMallocLAN_Align((MAX_MALLOC)(sizeof(LPC17XX_RX_BD) * i), 4);// create a smaller pool of buffer descriptors
    EMAC_RxDescriptor = (unsigned long)ptrBd;                            // address of the first descriptor
    while (i--) {                                                        // initialise the buffer descriptors
        ptrBd->ptrBlock = ptrBuffer;
        ptrBd->bd_details = (RX_LAN_INTERRUPT_ON_DONE | (pars->usSizeRx - 1));
        ptrBuffer += pars->usSizeRx;
        ptrBd++;
    }
    EMAC_RxConsumeIndex = EMAC_RxProduceIndex;

    i = LAN_TX_BUF_COUNT;                                                // set up transmitter buffer memory
    EMAC_TxDescriptorNumber = (LAN_TX_BUF_COUNT - 1);
    ptrBuffer = uMallocLAN_Align((unsigned short)(LAN_TX_BUF_COUNT * pars->usSizeTx), 1);// allocate a transmitter buffer pool
    EMAC_TxStatus = (unsigned long)uMallocLAN_Align((unsigned short)(sizeof(LPC17XX_TX_STATUS) * i), 4); // create a smaller pool of buffer descriptors
    ptrBd = uMallocLAN_Align((unsigned short)(sizeof(LPC17XX_RX_BD) * i), 4);// create a smaller pool of buffer descriptors

    EMAC_TxDescriptor = (unsigned long)ptrBd;                            // inform the MAC where the first transmit buffer is located
    while (i--) {                                                        // initialise the buffer descriptors
        ptrBd->ptrBlock = ptrBuffer;                                     // initialise the buffer descriptors
        ptrBuffer += pars->usSizeTx;
        ptrBd++;
    }
    EMAC_TxProduceIndex = EMAC_TxConsumeIndex;                           // this will be 0 initially

#if defined USE_IPV6
    SA0 = ((network[DEFAULT_NETWORK].ucOurMAC[5] << 8) | network[DEFAULT_NETWORK].ucOurMAC[4]); // set the unicast MAC address
    SA1 = ((network[DEFAULT_NETWORK].ucOurMAC[3] << 8) | network[DEFAULT_NETWORK].ucOurMAC[2]);
    SA2 = ((network[DEFAULT_NETWORK].ucOurMAC[1] << 8) | network[DEFAULT_NETWORK].ucOurMAC[0]);
#else
    SA0 = ((pars->ucMAC[5] << 8) | pars->ucMAC[4]);                      // set the MAC address
    SA1 = ((pars->ucMAC[3] << 8) | pars->ucMAC[2]);
    SA2 = ((pars->ucMAC[1] << 8) | pars->ucMAC[0]);
#endif

    if (pars->usMode & PROMISCUOUS) {
        EMAC_RxFilterControl = (ENABLE_ACCEPT_UNICAST | ENABLE_ACCEPT_BROADCAST | ENABLE_ACCEPT_MULTICAST);
    }
    else {
        if (pars->usMode & NO_BROADCAST) {
            EMAC_RxFilterControl = (ENABLE_ACCEPT_PERFECT);              // do not accept broadcast frames
        }
        else {
            EMAC_RxFilterControl = (ENABLE_ACCEPT_PERFECT | ENABLE_ACCEPT_BROADCAST);
        }
#ifdef USE_IPV6
        if (pars->usMode & CON_MULTICAST) {                              // if multicast reception is to be configured
            fnConfigureMulticastIPV6();                                  // set IPV6 multicast address
            EMAC_RxFilterControl |= ENABLE_ACCEPT_MULTICAST_HASH;        // enable multicast hash acceptance
        }
#endif
    }

    EMAC_IntClear = 0xffffffff;                                          // clear all possibly waiting interrupts
    fnEnterInterrupt(irq_Ethernet_Controller_ID, PRIORITY_EMAC, EMAC_Interrupt);// configure and enter the handling interrupt routine in the vector table
    EMAC_IntEnable = (EMAC_RxOverrunInt | EMAC_RxErrorInt | EMAC_RxFinishedInt | EMAC_RxDoneInt);    // enable interrupts to be handled
    EMAC_COMMAND |= (EMAC_RX_ENABLE | EMAC_TX_ENABLE);                   // enable transmit and receive DMA engines in RMII mode
    MAC1 |= (RECEIVE_ENABLE);                                            // enable receiver data path

#ifdef PHY_INTERRUPT                                                     // if PHY interrupt line is connected use it to detect link state changes and control auto-negotiated speed
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = fnPhyInterrupt;                        // handling function
    interrupt_setup.int_port = PHY_INT_PORT;                             // the port used
    interrupt_setup.int_priority = PHY_INT_PRIORITY;                     // port interrupt priority
    interrupt_setup.int_port_bits = PHY_INT_PIN;                         // the input connected
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE);                 // interrupt on this edge
    fnConfigureInterrupt(&interrupt_setup);                              // configure test interrupt

    #ifdef _LAN8700
    fnWriteMII(LAN8700_INTERRUPT_MASK, (PHY_LAN8700_LINK_DOWN | PHY_LAN8700_AUTO_NEG_COMPLETE)); // enable interrupts on link state changes
    #else
    fnWriteMII(DP83848_INTERRUPT_CONTROL_STATUS_REGISTER,                // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
    );
    #endif
#endif

    #ifdef _WINDOWS
    fnOpenDefaultHostAdapter();
    #endif
    return 0;                                                            // {11}
}



    #ifdef _WINDOWS

    #ifdef USE_IPV6
static int fnIsHashMulticast(unsigned char *ucData)
{
    unsigned long ulCRC32 = 0xffffffff;                                  // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    int i, bit;
    unsigned char ucByte;
    if (!(EMAC_RxFilterControl & ENABLE_ACCEPT_MULTICAST_HASH)) {        // multicast hash not enabled
        return 0;
    }
    if (!(*ucData & 0x01)) {                                             // if not multicast, don't check
        return 0;
    }
	for (i = 0; i < MAC_LENGTH; i++) {
        ucByte = *ucData++;;
        for (bit = 0; bit < 8; bit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
	}
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 detemine the hash entry location

    if (ulCRC32 < 32) {
        if (EMAC_HashFilterL & (0x00000001 << ulCRC32)) {
            return 1;                                                    // multicast hash match
        }
    }
    else {
        ulCRC32 -= 32;
        if (EMAC_HashFilterH & (0x00000001 << ulCRC32)) {
            return 1;                                                    // multicast hash match
        }
    }
    return 0;                                                            // not a match
}
    #endif

    // This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
    // on the programmed MAC address and the Ethernet operating mode
    //
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
    {
        unsigned char ucProgrammedMAC[MAC_LENGTH];

        if (!(MAC1 & RECEIVE_ENABLE) || (!(EMAC_COMMAND & EMAC_RX_ENABLE))) {
            return 0;                                                    // Ethernet interface not receiving
        }

        // This function checks to see whether we can receive the simulated frame and can block it if wanted
        if (usLen < (2*MAC_LENGTH + 2)) {
            return 0;                                                    // too short to be a valid Ethernet frame
        }

        if ((EMAC_RxFilterControl & ENABLE_ACCEPT_BROADCAST) && (!(uMemcmp(ucData, cucBroadcast, MAC_LENGTH)))) {
            return 1;                                                    // broadcast
        }

        ucProgrammedMAC[0] = (unsigned char)(SA2);
        ucProgrammedMAC[1] = (unsigned char)(SA2 >> 8);
        ucProgrammedMAC[2] = (unsigned char)(SA1);
        ucProgrammedMAC[3] = (unsigned char)(SA1>>8);
        ucProgrammedMAC[4] = (unsigned char)(SA0);
        ucProgrammedMAC[5] = (unsigned char)(SA0 >> 8);

        if ((EMAC_RxFilterControl & ENABLE_ACCEPT_PERFECT) && (!(uMemcmp(ucData, ucProgrammedMAC, MAC_LENGTH)))) {
            return 2;                                                    // our MAC
        }

        if (!uMemcmp(&ucData[MAC_LENGTH], ucProgrammedMAC, MAC_LENGTH)) {
            return 0;                                                    // we never accept echos of our own transmissions
        }

        if ((EMAC_RxFilterControl & (ENABLE_ACCEPT_UNICAST | ENABLE_ACCEPT_BROADCAST | ENABLE_ACCEPT_MULTICAST)) == (ENABLE_ACCEPT_UNICAST | ENABLE_ACCEPT_BROADCAST | ENABLE_ACCEPT_MULTICAST)) {
            return 3;                                                    // in promiscuous mode we accept everything
        }
    #ifdef USE_IPV6
        if (fnIsHashMulticast(&ucData[0])) {
            return 4;                                                    // multicast hash match
        }
    #endif
        return 0;                                                        // no match so reject
    }
    #endif


// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    unsigned long ulConsumeCheck;
    unsigned long ulConsumeIndex = EMAC_TxConsumeIndex;                  // snap shot of consume index
#if defined LPC1758_ETHERNET_WORKAROUND                                  // {21}
  //if ((ulConsumeIndex == 0) && (iTxFrames == 1)) {                     // a single frame has been transmitted so correct the TxConsumeIndex value (which sticks at zero)
  //    ulConsumeIndex = 1;
  //}
#endif
    if (ulConsumeIndex == 0) {
        ulConsumeCheck = EMAC_TxDescriptorNumber;                        // initial buffer descriptor
    }
    else {
        ulConsumeCheck = (ulConsumeIndex - 1);
    }

    if (EMAC_TxProduceIndex == ulConsumeCheck) {                         // if all transmit buffers are presently being used
        volatile int iMaxWait = 5000;                                    // allow a short wait before rejecting due to lack of free tx buffers
        while (EMAC_TxConsumeIndex == ulConsumeIndex) {
            if (!(--iMaxWait)) {
                return 1;                                                // abort because we have waited too long
            }
        }
    }
    return 0;                                                            // OK - buffer free
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return 1;                                                            // the LPC17XX driver doesn't need to know
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return 0;                                                            // for compatibility - the LPC17XX uses buffer descriptors for tracking buffers
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    LPC17XX_RX_BD *ptrBd = (LPC17XX_RX_BD *)EMAC_TxDescriptor;
    ptrBd += EMAC_TxProduceIndex;

    return (unsigned char *)ptrBd->ptrBlock;                             // return pointer to bd which is to be used next
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
    register unsigned long ulConsume = EMAC_RxConsumeIndex;
    if (ulConsume >= EMAC_RxDescriptorNumber) {
        ulConsume = 0;
    }
    else {
        ulConsume++;
    }
    EMAC_RxConsumeIndex = ulConsume;
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    #ifdef LAN_REPORT_ACTIVITY
    const unsigned char tx_act_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT };
    #endif
    LPC17XX_RX_BD *ptrBd = (LPC17XX_RX_BD *)EMAC_TxDescriptor;
    ptrBd += EMAC_TxProduceIndex;
    ptrBd->bd_details = (TX_LAN_INTERRUPT_ON_DONE | TX_LAST_FRAG | TX_APPEND_CRC | TX_PAD_FRAME | (DataLen - 1)); // prepare to start transmission

    #if defined _WINDOWS                                                 // LPC17XX performs zero automatic zero padding due to flag above
    while (DataLen < 60) {
        *ptr_put++ = 0x00;                                               // pad with zeros if smaller than 60 [chip must send at least 60]
        DataLen++;
    }

    fnSimulateEthTx((QUEUE_TRANSFER)((ptrBd->bd_details & RX_LAN_SIZE_MASK) + 1));

    if (usPhyMode & PHY_LOOP_BACK) {                                     // if the PHY is in loop back mode, simulate reception of sent frame
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
    if (EMAC_TxConsumeIndex >= EMAC_TxDescriptorNumber) {                // handle wrap-around
        EMAC_TxConsumeIndex = 0;
    }
    else {
        #if defined LPC1758_ETHERNET_WORKAROUND                          // {21}
        static int iSimulateErrate = 0;
        if (iSimulateErrate == 0) {
            // The TxConsumeIndex doesn't increment correctly the first time a frame is transmitted and stays at 0
            //
            iSimulateErrate++;
        }
        else if (iSimulateErrate == 1) {
            // The TxConsumeIndex jumps to 2 the second time a frame is transmitted
            //
            iSimulateErrate++;
            EMAC_TxConsumeIndex = (EMAC_TxConsumeIndex + 2);             // LAN_TX_BUF_COUNT should be >= 3
        }
        else {                                                           // otherwise it operates normally
            ++(EMAC_TxConsumeIndex);                                     // normal operation
        }
        #else
        ++(EMAC_TxConsumeIndex);
        #endif
    }
    #endif
    if (EMAC_TxProduceIndex >= EMAC_TxDescriptorNumber) {                // hand over descriptor to hardware by incrementing the produce index - this starts transmission of a first frame
        EMAC_TxProduceIndex = 0;
    #if defined LPC1758_ETHERNET_WORKAROUND                              // {21}
        iTxFrames = 2;                                                   // limit the frame trasnmission counter value
    #endif
    }
    else {
        ++(EMAC_TxProduceIndex);
    #if defined LPC1758_ETHERNET_WORKAROUND                              // {21}
        iTxFrames++;                                                     // increment count to transmitted frames
    #endif
    }
    eth_tx_control->ETH_queue.chars = 0;                                 // reset the transmission buffer for next use
    eth_tx_control->ETH_queue.put = eth_tx_control->ETH_queue.QUEbuffer;
    #ifdef LAN_REPORT_ACTIVITY
    fnWrite(INTERNAL_ROUTE, (unsigned char*)tx_act_message, HEADER_LENGTH); // inform the task of event
    #endif
    return (DataLen);                                                    // return the amount of bytes transmitted in the Ethernet frame
}

#endif                                                                   // endif ETH_INTERFACE



#ifdef SERIAL_INTERFACE
/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */
    #ifdef _WINDOWS
    extern unsigned char ucTxLast[NUMBER_SERIAL] = {0};
    #endif


// Collect a pointer to defined UART control block
//
static LPC17XX_UART_CONTROL *fnSelectChannel(QUEUE_HANDLE Channel)
{
    switch (Channel) {
    case 0:
        return (LPC17XX_UART_CONTROL *)UART_BLOCK_0;
    case 1:
        return (LPC17XX_UART_CONTROL *)UART_BLOCK_1;
    case 2:
        return (LPC17XX_UART_CONTROL *)UART_BLOCK_2;
    case 3:
        return (LPC17XX_UART_CONTROL *)UART_BLOCK_3;
    #if defined LPC1788
    case 4:
        return (LPC17XX_UART_CONTROL *)UART_BLOCK_4;                     // 5th UART available om the LPC1788
    #endif
    }
    return 0;                                                            // invalid
}


// Configure the SCI (UART) hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
    unsigned short usDivider;

    switch (Channel) {
    case 0:
        POWER_UP(PCUART0);                                               // power up the UART
    #if defined LPC1788
        #ifdef UART0_PORT0_LOW
        IOCON_P0_00 = _UART0_P0_0_TXD;
        IOCON_P0_01 = _UART0_P0_1_RXD;
        #else
        IOCON_P0_02 = _UART0_P0_2_TXD;
        IOCON_P0_03 = _UART0_P0_3_RXD;
        #endif
    #else
        PINSEL0 |= (PINSEL0_P0_2_TXD0 | PINSEL0_P0_3_RXD0);              // TXD0 and RXD0 on P0.2 and P0.3
    #endif
        break;
    case 1:
        POWER_UP(PCUART1);                                               // power up the UART
    #if defined LPC1788
        #if defined UART1_PORT2
        IOCON_P2_00 = _UART1_P2_0_TXD;
        IOCON_P2_01 = _UART1_P2_1_RXD;
        #elif defined UART1_PORT3
        IOCON_P3_16 = _UART1_P3_16_TXD;
        IOCON_P3_17 = _UART1_P3_17_RXD;
        #else
        IOCON_P0_15 = _UART1_P0_15_TXD;
        IOCON_P0_16 = _UART1_P0_16_RXD;
        #endif
    #else
        #ifdef UART1_PORT2
        PINSEL4 |= (PINSEL4_P2_0_TXD1 | PINSEL4_P2_1_RXD1);              // TXD1 and RXD1 on P2.0 and P2.1
        #else
        PINSEL0 |= (PINSEL0_P0_15_TXD1);                                 // TXD1 on P0.15
        PINSEL1 |= (PINSEL1_P0_16_RXD1);                                 // RXD1 on P0.16
        #endif
    #endif
        break;
    case 2:
        POWER_UP(PCUART2);                                               // power up the UART
    #if defined LPC1788
        #ifdef UART2_PORT2
        IOCON_P2_08 = _UART2_P2_8_TXD;
        IOCON_P2_09 = _UART2_P2_9_RXD;
        #elif defined UART2_PORT4
        IOCON_P4_22 = _UART2_P4_22_TXD;
        IOCON_P4_23 = _UART2_P4_23_RXD;
        #else
        IOCON_P0_10 = _UART2_P0_10_TXD;
        IOCON_P0_11 = _UART2_P0_11_RXD;
        #endif
    #else
        #ifdef UART2_PORT2
        PINSEL4 |= (PINSEL4_P2_8_TXD2 | PINSEL4_P2_9_RXD2);              // TXD2 and RXD2 on P2.8 and P2.9
        #else
        PINSEL0 |= (PINSEL0_P0_10_TXD2 | PINSEL0_P0_11_RXD2);            // TXD2 and RXD2 on P0.10 and P0.11
        #endif
    #endif
        break;
    case 3:
        POWER_UP(PCUART3);                                               // power up the UART
    #if defined LPC1788
        #if defined UART3_PORT4
        IOCON_P4_28 = _UART3_P4_28_TXD;
        IOCON_P4_29 = _UART3_P4_29_RXD;
        #elif defined UART3_PORT0_HIGH
        IOCON_P0_25 = _UART3_P0_25_TXD;
        IOCON_P0_26 = _UART3_P0_26_RXD;
        #elif defined UART3_PORT0_MID
        IOCON_P0_02 = _UART3_P0_2_TXD;
        IOCON_P0_03 = _UART3_P0_3_RXD;
        #else
        IOCON_P0_00 = _UART3_P0_0_TXD;
        IOCON_P0_01 = _UART3_P0_1_RXD;
        #endif
    #else
        #if defined UART3_PORT0_HIGH
        PINSEL1 |= (PINSEL1_P0_25_TXD3 | PINSEL1_P0_26_RXD3);            // TXD3 and RXD3 on P0.25 and P0.26
        #elif defined UART3_PORT4
        PINSEL9 |= (PINSEL9_P4_28_TXD3 | PINSEL9_P4_29_RXD3);            // TXD2 and RXD2 on P4.28 and P4.29
        #else
        PINSEL0 |= (PINSEL0_P0_0_TXD3 | PINSEL0_P0_1_RXD3);              // TXD3 and RXD3 on P0.0 and P0.1
        #endif
    #endif
        break;
    #if defined LPC1788
        #if defined UART4_PORT0
        IOCON_P0_22 = _UART4_P0_22_TXD;
        IOCON_P2_09 = _UART4_P2_9_RXD;
        #elif defined UART4_PORT1
        IOCON_P1_29 = _UART4_P1_29_TXD;
        IOCON_P2_09 = _UART4_P2_9_RXD;
        #else
        IOCON_P5_04 = _UART4_P5_4_TXD;
        IOCON_P5_03 = _UART4_P5_3_RXD;
        #endif
    #endif
    default:
        return;
    }

    #ifdef SUPPORT_HW_FLOW
    if (pars->Config & RTS_CTS) {                                        // HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    uart_reg->IIR_FCR = FIFO_ENABLE;                                     // set the FIFO enable bit in the FCR register. This bit must be set for proper UART operation
    uart_reg->IIR_FCR = (FIFO_ENABLE | RX_FIFO_RESET | TX_FIFO_RESET | RX_TRIGGER_1_CHAR); // ensure FIFOs reset when starting
    uart_reg->LCR = DLAB;                                                // set access to divisor latch registers

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        usDivider = ((UART_CLOCK/16 + 300/2)/300);                       // set 300
        break;
    case SERIAL_BAUD_600:
        usDivider = ((UART_CLOCK/16 + 600/2)/600);                       // set 600
        break;
    case SERIAL_BAUD_1200:
        usDivider = ((UART_CLOCK/4 + 1200/2)/1200);                      // set 1200
        break;
    case SERIAL_BAUD_2400:
        usDivider = ((UART_CLOCK/16 + 2400/2)/2400);                     // set 2400
        break;
    case SERIAL_BAUD_4800:
        usDivider = ((UART_CLOCK/16 + 4800/2)/4800);                     // set 4800
        break;
    case SERIAL_BAUD_9600:
        usDivider = ((UART_CLOCK/16 + 9600/2)/9600);                     // set 9600
        break;
    case SERIAL_BAUD_14400:
        usDivider = ((UART_CLOCK/16 + 14400/2)/14400);                   // set 14400
        break;
    default:                                                             // if not valid value set this speed
    case SERIAL_BAUD_19200:
        usDivider = ((UART_CLOCK/16 + 19200/2)/19200);                   // set 19200
        break;
    case SERIAL_BAUD_38400:
        usDivider = ((UART_CLOCK/16 + 38400/2)/38400);                   // set 38400
        break;
    case SERIAL_BAUD_57600:
        usDivider = ((UART_CLOCK/16 + 57600/2)/57600);                   // set 57600
        break;
    case SERIAL_BAUD_115200:
        usDivider = ((UART_CLOCK/16 + 115200/2)/115200);                 // set 115200
        break;
    case SERIAL_BAUD_230400:
        usDivider = ((UART_CLOCK/16 + 230400/2)/230400);                 // set 230400
        break;
    }
    uart_reg->RBR_THR_DLL = (unsigned char)usDivider;                    // program speed to divisor latch registers
    uart_reg->DLM_IER = (unsigned char)(usDivider >> 8);

    if (pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {
        if (pars->Config & RS232_ODD_PARITY) {
            uart_reg->LCR = (UART_PARITY_ENABLE | UART_PARITY_ODD);      // program odd parity
        }
        else {
            uart_reg->LCR = (UART_PARITY_ENABLE | UART_PARITY_EVEN);     // program even parity
        }
    }
    else {
        uart_reg->LCR = UART_PARITY_DISABLED;                            // no parity
    }

    if (pars->Config & TWO_STOPS) {
        uart_reg->LCR |= UART_2_STOP_BITS;                               // set 2 stop bits rather than 1 stop bit
    }

    if (pars->Config & CHAR_7) {
        uart_reg->LCR |= UART_7_BIT_CHAR;                                // set 7 bit characters
    }
    else {
        uart_reg->LCR |= UART_8_BIT_CHAR;                                // set 8 bit characters
    }

    #ifdef _WINDOWS
    uart_reg->IIR_FCR &= ~(TX_FIFO_RESET | RX_FIFO_RESET);
    fnConfigSimSCI(Channel, UART_CLOCK/((unsigned short)((uart_reg->DLM_IER << 8) | uart_reg->RBR_THR_DLL)*16), pars); // open a serial port on PC if desired
    #endif
}

/**************************** SCI0 interrupt ******************************************/

static __interrupt void SCI0_Interrupt(void)
{
    volatile unsigned long ulStatus;

    while (((ulStatus = U0IIR) & UART_NO_INT_PENDING) == 0) {            // while interrupts pending
        switch (ulStatus & UART_INT_MASK) {                              // get the interrupt type
        case UART_RLS_INT_PENDING:                                       // line stats change or error
            ulStatus = U0LSR;                                            // read the status - clearing interrupt source
            if (ulStatus & (UART_OE | UART_PE | UART_FE)) {              // error
                break;
            }
            if (ulStatus & UART_BI) {
                // break condition change active
            }
            else {
                // break condition changed inactive
            }
            break;

        case UART_RDA_INT_PENDING:                                       // reception
        case UART_CTI_INT_PENDING:                                       // reception with timeout due to no read
            fnSciRxByte((unsigned char)U0RBR_THR, 0);                    // receive data interrupt - read the byte (clearing the interrupt)
            break;

        case UART_THRE_INT_PENDING:                                      // transmission interrupt (read of IIR will have cleared the interrupt)
    #ifdef _WINDOWS
            U0LSR |= UART_THRE;                                          // mark transmitter empty again
    #endif
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
            break;
        }
    #ifdef _WINDOWS
        U0IIR &= ~(UART_INT_MASK);
        U0IIR |= UART_NO_INT_PENDING;
        U0LSR &= ~(UART_RDR | UART_OE | UART_PE | UART_FE | UART_BI | UART_RXFE);
    #endif
    }
}

/**************************** SCI1 interrupt ******************************************/

static __interrupt void SCI1_Interrupt(void)
{
    volatile unsigned long ulStatus;

    while (((ulStatus = U1IIR) & UART_NO_INT_PENDING) == 0) {            // while interrupts pending
        switch (ulStatus & UART_INT_MASK) {                              // get the interrupt type
        case UART_RLS_INT_PENDING:                                       // line stats change or error
            ulStatus = U1LSR;                                            // read the status - clearing interrupt source
            if (ulStatus & (UART_OE | UART_PE | UART_FE)) {              // error
                break;
            }
            if (ulStatus & UART_BI) {
                // break condition change active
            }
            else {
                // break condition changed inactive
            }
            break;

        case UART_RDA_INT_PENDING:                                       // reception
        case UART_CTI_INT_PENDING:                                       // reception with timeout due to no read
            fnSciRxByte((unsigned char)U1RBR_THR, 1);                    // receive data interrupt - read the byte (clearing the interrupt)
            break;

        case UART_THRE_INT_PENDING:                                      // transmission interrupt (read of IIR will have cleared the interrupt)
    #ifdef _WINDOWS
            U1LSR |= UART_THRE;                                          // mark transmitter empty again
    #endif
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
            break;
        }
    #ifdef _WINDOWS
        U1IIR &= ~(UART_INT_MASK);
        U1IIR |= UART_NO_INT_PENDING;
        U1LSR &= ~(UART_RDR | UART_OE | UART_PE | UART_FE | UART_BI | UART_RXFE);
    #endif
    }
}

/**************************** SCI2 interrupt ******************************************/

static __interrupt void SCI2_Interrupt(void)
{
    volatile unsigned long ulStatus;

    while (((ulStatus = U2IIR) & UART_NO_INT_PENDING) == 0) {            // while interrupts pending
        switch (ulStatus & UART_INT_MASK) {                              // get the interrupt type
        case UART_RLS_INT_PENDING:                                       // line stats change or error
            ulStatus = U2LSR;                                            // read the status - clearing interrupt source
            if (ulStatus & (UART_OE | UART_PE | UART_FE)) {              // error
                break;
            }
            if (ulStatus & UART_BI) {
                // break condition change active
            }
            else {
                // break condition changed inactive
            }
            break;

        case UART_RDA_INT_PENDING:                                       // reception
        case UART_CTI_INT_PENDING:                                       // reception with timeout due to no read
            fnSciRxByte((unsigned char)U2RBR_THR, 2);                    // receive data interrupt - read the byte (clearing the interrupt)
            break;

        case UART_THRE_INT_PENDING:                                      // transmission interrupt (read of IIR will have cleared the interrupt)
        #ifdef _WINDOWS
            U2LSR |= UART_THRE;                                          // mark transmitter empty again
        #endif
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
            break;
        }
        #ifdef _WINDOWS
        U2IIR &= ~(UART_INT_MASK);
        U2IIR |= UART_NO_INT_PENDING;
        U2LSR &= ~(UART_RDR | UART_OE | UART_PE | UART_FE | UART_BI | UART_RXFE);
        #endif
    }
}

/**************************** SCI3 interrupt ******************************************/

static __interrupt void SCI3_Interrupt(void)
{
    volatile unsigned long ulStatus;

    while (((ulStatus = U3IIR) & UART_NO_INT_PENDING) == 0) {            // while interrupts pending
        switch (ulStatus & UART_INT_MASK) {                              // get the interrupt type
        case UART_RLS_INT_PENDING:                                       // line stats change or error
            ulStatus = U3LSR;                                            // read the status - clearing interrupt source
            if (ulStatus & (UART_OE | UART_PE | UART_FE)) {              // error
                break;
            }
            if (ulStatus & UART_BI) {
                // break condition change active
            }
            else {
                // break condition changed inactive
            }
            break;

        case UART_RDA_INT_PENDING:                                       // reception
        case UART_CTI_INT_PENDING:                                       // reception with timeout due to no read
            fnSciRxByte((unsigned char)U3RBR_THR, 3);                    // receive data interrupt - read the byte (clearing the interrupt)
            break;

        case UART_THRE_INT_PENDING:                                      // transmission interrupt (read of IIR will have cleared the interrupt)
        #ifdef _WINDOWS
            U3LSR |= UART_THRE;                                          // mark transmitter empty again
        #endif
            fnSciTxByte(3);                                              // transmit data empty interrupt - write next byte
            break;
        }
        #ifdef _WINDOWS
        U3IIR &= ~(UART_INT_MASK);
        U3IIR |= UART_NO_INT_PENDING;
        U3LSR &= ~(UART_RDR | UART_OE | UART_PE | UART_FE | UART_BI | UART_RXFE);
        #endif
    }
}

#if defined LPC1788
/**************************** SCI4 interrupt ******************************************/

static __interrupt void SCI4_Interrupt(void)
{
    volatile unsigned long ulStatus;

    while (((ulStatus = U4IIR) & UART_NO_INT_PENDING) == 0) {            // while interrupts pending
        switch (ulStatus & UART_INT_MASK) {                              // get the interrupt type
        case UART_RLS_INT_PENDING:                                       // line stats change or error
            ulStatus = U4LSR;                                            // read the status - clearing interrupt source
            if (ulStatus & (UART_OE | UART_PE | UART_FE)) {              // error
                break;
            }
            if (ulStatus & UART_BI) {
                // break condition change active
            }
            else {
                // break condition changed inactive
            }
            break;

        case UART_RDA_INT_PENDING:                                       // reception
        case UART_CTI_INT_PENDING:                                       // reception with timeout due to no read
            fnSciRxByte((unsigned char)U4RBR_THR, 4);                    // receive data interrupt - read the byte (clearing the interrupt)
            break;

        case UART_THRE_INT_PENDING:                                      // transmission interrupt (read of IIR will have cleared the interrupt)
        #ifdef _WINDOWS
            U4LSR |= UART_THRE;                                          // mark transmitter empty again
        #endif
            fnSciTxByte(4);                                              // transmit data empty interrupt - write next byte
            break;
        }
        #ifdef _WINDOWS
        U4IIR &= ~(UART_INT_MASK);
        U4IIR |= UART_NO_INT_PENDING;
        U4LSR &= ~(UART_RDR | UART_OE | UART_PE | UART_FE | UART_BI | UART_RXFE);
        #endif
    }
}
#endif


#ifdef UART_EXTENDED_MODE
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel)            // dummy
{
    return 0;
}
#endif

static void fnConfigUARTInterrupt(QUEUE_HANDLE Channel)
{
    switch (Channel) {                                                   // enter the interrupt handler
    case 0:
        fnEnterInterrupt(irq_UART0_ID, PRIORITY_UART0, SCI0_Interrupt);  // configure and enter the handling interrupt routine in the vector table
        break;
    case 1:
        fnEnterInterrupt(irq_UART1_ID, PRIORITY_UART1, SCI1_Interrupt);  // configure and enter the handling interrupt routine in the vector table
        break;
    case 2:
        fnEnterInterrupt(irq_UART2_ID, PRIORITY_UART2, SCI2_Interrupt);  // configure and enter the handling interrupt routine in the vector table
        break;
    case 3:
        fnEnterInterrupt(irq_UART3_ID, PRIORITY_UART3, SCI3_Interrupt);  // configure and enter the handling interrupt routine in the vector table
        break;
#if defined LPC1788
    case 4:
        fnEnterInterrupt(irq_UART4_ID, PRIORITY_UART4, SCI4_Interrupt);  // configure and enter the handling interrupt routine in the vector table
        break;
#endif
    }
}

// Enable reception on the defined channel
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);

    uart_reg->IIR_FCR = (FIFO_ENABLE | RX_FIFO_RESET | RX_TRIGGER_1_CHAR); // ensure rx FIFOs reset before enabling receive interrupts
    fnConfigUARTInterrupt(Channel);
    uart_reg->DLM_IER |= UART_RDA_INT;                                   // enable reception interrupt
    #ifdef _WINDOWS
    uart_reg->IIR_FCR &= ~(TX_FIFO_RESET | RX_FIFO_RESET);
    #endif
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
    uart_reg->DLM_IER &= ~UART_RDA_INT;                                  // disable reception interrupt
}

// Enable transmission on the defined channel
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);

    uart_reg->IIR_FCR = (FIFO_ENABLE | TX_FIFO_RESET | RX_TRIGGER_1_CHAR); // ensure tx FIFOs reset before enabling receive interrupts
    fnConfigUARTInterrupt(Channel);
    uart_reg->DLM_IER |= UART_THRE_INT;                                  // enable transmission interrupt
    #ifdef _WINDOWS
    uart_reg->IIR_FCR &= ~(TX_FIFO_RESET | RX_FIFO_RESET);
    #endif
}

// Disable transmission on the defined channel
//
extern void fnTxOff(QUEUE_HANDLE Channel)
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
    uart_reg->DLM_IER &= ~UART_THRE_INT;                                 // disable transmission interrupt
}

// The TTY driver uses this call to disable the transmit interrupt of the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE channel)
{
    // the LPC17xx doesn't need to disable transmit interrupts during idle periods so this function is left empty
}

// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(channel);

    if (!(uart_reg->LSR & UART_THRE)) {                                  // transmitter is presently active
        return 1;                                                        // can't at the moment send so inform this
    }
    uart_reg->RBR_THR_DLL = ucTxByte;                                    // send the character
    #ifdef _WINDOWS
    ucTxLast[channel] = (unsigned char)uart_reg->RBR_THR_DLL;            // back up the data written so that it can't get lost when rx data uses the simulated register
    uart_reg->LSR &= ~(UART_THRE);                                       // mark transmitter presently not empty
    switch (channel) {                                                   // activate first interrupt
    case 0:
        iInts |= CHANNEL_0_SERIAL_INT;
        break;
    case 1:
        iInts |= CHANNEL_1_SERIAL_INT;
        break;
    case 2:
        iInts |= CHANNEL_2_SERIAL_INT;
        break;
    case 3:
        iInts |= CHANNEL_3_SERIAL_INT;
        break;
        #if defined LPC1788
    case 4:
        iInts |= CHANNEL_4_SERIAL_INT;
        break;
        #endif
    }
    #endif
    return 0;                                                            // transmission started
}


    #ifdef SUPPORT_HW_FLOW
static void fnSetRTS(QUEUE_HANDLE channel, int iState)
{
    switch (channel) {
    #if defined RTS_0_PIN
    case 0:
        if (iState != 0) {
            RTS_0_PORT_CLR = RTS_0_PIN;                                  // assert RTS
        }
        else {
            RTS_0_PORT_SET = RTS_0_PIN;                                  // negate RTS
        }
        break;
    #endif
    #if CHIP_HAS_UARTS > 1 && defined RTS_1_PIN                          // only if second UART is available
    case 1:
        if (iState != 0) {
            RTS_1_PORT_CLR = RTS_1_PIN;                                  // assert RTS
        }
        else {
            RTS_1_PORT_SET = RTS_1_PIN;                                  // negate RTS
        }
        break;
    #endif
    #if CHIP_HAS_UARTS > 2 && defined RTS_2_PIN                          // only if third UART is available
    case 2:
        if (iState != 0) {
            RTS_2_PORT_CLR = RTS_2_PIN;                                  // assert RTS
        }
        else {
            RTS_2_PORT_SET = RTS_2_PIN;                                  // negate RTS
        }
        break;
    #endif
    #if CHIP_HAS_UARTS > 3 && defined RTS_3_PIN                          // only if fourth UART is available
    case 3:
        if (iState != 0) {
            RTS_3_PORT_CLR = RTS_3_PIN;                                  // assert RTS
        }
        else {
            RTS_3_PORT_SET = RTS_3_PIN;                                  // negate RTS
        }
        break;
    #endif
    }
}
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    if (usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) {
        if (usModifications & CONFIG_RTS_PIN) {
            switch (channel) {
        #if defined RTS_0_PIN
            case 0:
                RTS_0_PORT_DDR |= RTS_0_PIN;                             // configure as digital output
                break;
        #endif
        #if CHIP_HAS_UARTS > 1 && defined RTS_1_PIN                      // only if second UART is available
            case 1:
                RTS_1_PORT_DDR |= RTS_1_PIN;                             // configure as digital output
                break;
        #endif
        #if CHIP_HAS_UARTS > 2 && defined RTS_2_PIN                      // only if third UART is available
            case 2:
                RTS_2_PORT_DDR |= RTS_2_PIN;                             // configure as digital output
                break;
        #endif
        #if CHIP_HAS_UARTS > 3 && defined RTS_3_PIN                      // only if fourth UART is available
            case 3:
                RTS_3_PORT_DDR |= RTS_3_PIN;                             // configure as digital output
                break;
        #endif
            }
            fnSetRTS(channel, 0);                                        // drive the RTS line in negated state
        }
        if (usModifications & CONFIG_CTS_PIN) {
            switch (channel) {
        #if defined CTS_0_PIN
            case 0:
                CTS_0_PORT_DDR &= ~CTS_0_PIN;                            // configure as digital input
                break;
        #endif
        #if CHIP_HAS_UARTS > 1 && defined CTS_1_PIN                      // only if second UART is available
            case 1:
                CTS_1_PORT_DDR &= ~CTS_1_PIN;                            // configure as digital input
                break;
        #endif
        #if CHIP_HAS_UARTS > 2 && defined CTS_2_PIN                      // only if third UART is available
            case 2:
                CTS_2_PORT_DDR &= ~CTS_2_PIN;                            // configure as digital input
                break;
        #endif
        #if CHIP_HAS_UARTS > 3 && defined CTS_3_PIN                      // only if fourth UART is available
            case 3:
                CTS_3_PORT_DDR &= ~CTS_3_PIN;                            // configure as digital input
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


static void cts0_change(void)
{
    #if defined CTS_0_PIN
    fnRTS_change(0, ((CTS_0_STATE & CTS_0_PIN) == 0));                   // control transmission according to state
    #endif
}

#if CHIP_HAS_UARTS > 1
static void cts1_change(void)
{
    #if defined CTS_1_PIN
    fnRTS_change(1, ((CTS_1_STATE & CTS_1_PIN) == 0));                   // control transmission according to state
    #endif
}
#endif
#if CHIP_HAS_UARTS > 2
static void cts2_change(void)
{
    #if defined CTS_2_PIN
    fnRTS_change(2, ((CTS_2_STATE & CTS_2_PIN) == 0));                   // control transmission according to state
    #endif
}
#endif
#if CHIP_HAS_UARTS > 3
static void cts3_change(void)
{
    #if defined CTS_3_PIN
    fnRTS_change(3, ((CTS_3_STATE & CTS_3_PIN) == 0));                   // control transmission according to state
    #endif
}
#endif

static void ( *cts_change[CHIP_HAS_UARTS] )( void ) = {
    cts0_change,
#if CHIP_HAS_UARTS > 1
    cts1_change,
#endif
#if CHIP_HAS_UARTS > 2
    cts2_change,
#endif
#if CHIP_HAS_UARTS > 3
    cts3_change,
#endif
};

static const unsigned char cts_irq_priority[CHIP_HAS_UARTS] = {
#if defined CTS_0_PIN
    CTS_0_INT_PRIORITY,
#else
    0,
#endif
#if CHIP_HAS_UARTS > 1
    #if defined CTS_0_PIN
    CTS_1_INT_PRIORITY,
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 2
    #if defined CTS_0_PIN
    CTS_2_INT_PRIORITY,
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 3
    #if defined CTS_3_PIN
    CTS_3_INT_PRIORITY,
    #else
    0,
    #endif
#endif
};

static const unsigned char cts_port[CHIP_HAS_UARTS] = {
#if defined CTS_0_PIN
    CTS_0_PORT,
#else
    0,
#endif
#if CHIP_HAS_UARTS > 1
    #if defined CTS_1_PIN
    CTS_1_PORT,
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 2
    #if defined CTS_2_PIN
    CTS_2_PORT,
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 3
    #if defined CTS_3_PIN
    CTS_3_PORT,
    #else
    0,
    #endif
#endif
};

static const unsigned long cts_port_bit[CHIP_HAS_UARTS] = {
#if defined CTS_0_PIN
    CTS_0_PIN,
#else
    0,
#endif
#if CHIP_HAS_UARTS > 1
    #if defined CTS_1_PIN
    CTS_1_PIN,
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 2
    #if defined CTS_2_PIN
    CTS_2_PIN,
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 3
    #if defined CTS_3_PIN
    CTS_3_PIN,
    #else
    0,
    #endif
#endif
};

extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    QUEUE_TRANSFER ControlLines = 0;
    if (usModifications & (ENABLE_CTS_CHANGE | DISABLE_CTS_CHANGE)) {
        INTERRUPT_SETUP interrupt_setup;                                 // interrupt configuration parameters
        interrupt_setup.int_type = PORT_INTERRUPT;                       // identifier when configuring
        if (usModifications & ENABLE_CTS_CHANGE) {
            interrupt_setup.int_handler = cts_change[channel];           // handling function
        }
        else {
            interrupt_setup.int_handler = 0;                             // disable interrupts
        }
        interrupt_setup.int_port = cts_port[channel];                    // the port used
        interrupt_setup.int_priority = cts_irq_priority[channel];        // port interrupt priority
        interrupt_setup.int_port_bits = cts_port_bit[channel];           // the input connected
        interrupt_setup.int_port_sense = IRQ_BOTH_EDGES;                 // interrupt on both edges
        fnConfigureInterrupt(&interrupt_setup);                          // configure test interrupt
    }
    switch (channel) {
#if defined CTS_0_PIN
    case 0:
        if ((CTS_0_STATE & CTS_0_PIN) == 0) {
            ControlLines |= SET_CTS;                                     // CTS line is active
        }
        break;
#endif
#if CHIP_HAS_UARTS > 1 && defined CTS_1_PIN
    case 1:
        if ((CTS_1_STATE & CTS_1_PIN) == 0) {
            ControlLines |= SET_CTS;                                     // CTS line is active
        }
        break;
#endif
#if CHIP_HAS_UARTS > 2 && defined CTS_2_PIN
    case 2:
        if ((CTS_2_STATE & CTS_2_PIN) == 0) {
            ControlLines |= SET_CTS;                                     // CTS line is active
        }
        break;
#endif
#if CHIP_HAS_UARTS > 3 && defined CTS_3_PIN
    case 3:
        if ((CTS_3_STATE & CTS_3_PIN) == 0) {
            ControlLines |= SET_CTS;                                     // CTS line is active
        }
        break;
#endif
    }
    return ControlLines;
}
    #endif


    #ifdef UART_BREAK_SUPPORT
extern void fnStartBreak(QUEUE_HANDLE Channel)                           // {2}
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
    uart_reg->LCR |= BREAK_CONTROL;
}


extern void fnStopBreak(QUEUE_HANDLE Channel)                            // {2}
{
    LPC17XX_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
    uart_reg->LCR &= ~BREAK_CONTROL;
}
    #endif
#endif                                                                   // end if SERIAL_INTERFACE




/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */

#if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM

// Configure the SPI interface for file system use - SSP used
//
static void fnConfigSPIFileSystem(unsigned char ucSpeed)
{
    POWER_UP(PCSSP_X);                                                   // {10} power up the SSP used
    CONFIGURE_SPI_PINS();
    CONFIGURE_CS_LINES();

    SSPCR1_X = 0;                                                        // ensure disabled (master mode)
    SSPCPSR_X = 2;                                                       // set clock prescaler (even prescale 2..254)

    switch (ucSpeed) {
    case MAX_SPI:                                                        // set maximum possible SPI speed
        SSPCR0_X = (FRS_FREESCALE_SPI | DSS_8_BIT | (0 << SSP_SCR_SHIFT)); // set exact frequency and mode
	    break;

    case SPI_2MEG:
        SSPCR0_X = (FRS_FREESCALE_SPI | DSS_8_BIT | ((((SSP_CLOCK/2 + 2000000/2)/2000000) - 1) << SSP_SCR_SHIFT)); // set exact frequency and mode
	    break;

    case SPI_SLAVE:
	    return;
    }
    SSPCR1_X = SSP_SSE;                                                  // enable
}
#endif



#if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA              // internal flash driver required
static unsigned long fnGetFlashSector(unsigned long FlashAddress)
{
    FlashAddress -= FLASH_START_ADDRESS;
    if (FlashAddress < FLASH_SECTOR_16_OFFSET) {                         // one of the first small sectors
        return (FlashAddress/FLASH_GRANULARITY_SMALL);
    }
    else {                                                               // one of the large sectors
        FlashAddress -= FLASH_SECTOR_16_OFFSET;
        return ((FlashAddress/FLASH_GRANULARITY_LARGE) + 16);
    }
}

    #ifdef MANAGED_FILES
static int fnGetFlashSectorRestSize(unsigned long FlashAddress)          // {12} return remaining size of sector (from the location in the present sector)
{
    unsigned long ulSectorSize;
    FlashAddress -= FLASH_START_ADDRESS;                                 // reference to start of flash content
    if (FlashAddress < FLASH_SECTOR_16_OFFSET) {                         // one of the first small sectors
        ulSectorSize = (FLASH_GRANULARITY_SMALL);
    }
    else {                                                               // one of the large sectors
        FlashAddress -= FLASH_SECTOR_16_OFFSET;                          // reference to start of large sectors
        ulSectorSize = (FLASH_GRANULARITY_LARGE);
    }
    while (FlashAddress >= ulSectorSize) {                               // calculate the size of the sector after the location
        FlashAddress -= ulSectorSize;
    }
    return (ulSectorSize - FlashAddress);                                // the rest size
}
    #endif

// IAP flash command
//
static int fnFlashCommand(int iCommand, unsigned char *ptrFlashAddress, MAX_FILE_LENGTH Length)
{
    #define WRITE_FLASH_BUFFER 0
    #define DELETE_FLASH       1
    IAP iap_entry = IAP_LOCATION;
    unsigned long command[5];                                            // command buffer to be passed to IAP routine
    unsigned long result[3];                                             // result buffer for use by the IAP routine
    switch (iCommand) {
    case WRITE_FLASH_BUFFER:                                             // write a present FLASH row backup buffer to the FLASH memory
        {
            unsigned long ulSector = fnGetFlashSector((unsigned long)ptrFlashAddress);
            command[0] = FLASH_PREPARE_SECTOR_TO_WRITE;                  // prepare the sector for the command
            command[1] = ulSector;
            command[2] = ulSector;
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // sector invalid or busy
            }
            command[0] = FLASH_COPY_RAM_TO_FLASH;
            command[1] = (unsigned long)ptrFlashAddress;                 // flash destination start address (256 byte boundard)
            command[2] = (unsigned long)ucFlashRow;                      // source ram address is always the ROW backup buffer (word aligned)
            command[3] = FLASH_ROW_SIZE;                                 // allowed: 256 | 512 | 1024 | 4096 | 8192 - we use always the row size
            command[4] = (MASTER_CLOCK/1000);                            // system clock frequency (CCLK) in kHz
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // write error
            }
        }
        break;

    case DELETE_FLASH:                                                   // delete a sector or several sectors in FLASH
        {
            unsigned long ulSectorStart = fnGetFlashSector((unsigned long)ptrFlashAddress); // get the first sector to be deleted
            unsigned long ulSectorEnd   = fnGetFlashSector((unsigned long)(ptrFlashAddress + (Length - 1)));// get the last sector to be deleted
    #if defined MANAGED_FILES
            if (Length == 0) {                                           // delete one sector if no length is defined
                ulSectorEnd = ulSectorStart;                             // end sector equals start sector to delete just one block
            }
    #else
            if (Length == 0) {                                           // don't delete if length 0 is given
                break;
            }
    #endif
            command[0] = FLASH_PREPARE_SECTOR_TO_WRITE;                  // prepare the sector for the command
            command[1] = ulSectorStart;
            command[2] = ulSectorEnd;
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // sector invalid or busy
            }
            command[0] = FLASH_ERASE_SECTORS;
            command[3] = (MASTER_CLOCK/1000);                            // system clock frequency (CCLK) in kHz
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // erase error
            }
        }
        break;
    }
    return 0;
}
#endif                                                                   // end of internal flash IAP code


#if defined SPI_FLASH_ENABLED
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code required by the SPI flash driver
        #include "spi_flash_lpc_atmel.h"
        #include "spi_flash_lpc_stmicro.h"
        #include "spi_flash_lpc_sst25.h"                                 // {23}
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

// Calculate the device to access from the absolute address. It is assumed that all devices are operational.
//
static int fnGetSPI_Device(unsigned char **ptrFlash)                     // define the device to be accessed
{
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE)) {
        return 0;                                                        // address is in the first SPI device
    }
    #if SPI_FLASH_DEVICE_COUNT >= 2
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;                                                        // address is in the second SPI device
    }
        #if SPI_FLASH_DEVICE_COUNT >= 3
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
        return 2;                                                        // address is in the third SPI device
    }
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    return 3;                                                            // address is in the fourth and last SPI device

        #else
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 2;                                                            // address is in the third and last SPI device
        #endif
    #else
    *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
    return 1;                                                            // address is in the second and last SPI device
    #endif
}
    #endif

    #ifdef SPI_FLASH_SST25                                               // {23}
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
    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif
    return ucDestination;
}
    #else
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

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
    #endif
#endif                                                                   // end of SPI Flash driver code


    #if defined STORAGE_REV_1                                            // {53}
    #if defined SPI_FLASH_ENABLED && !defined ONLY_INTERNAL_FLASH_STORAGE// multiple storage medium interface

// The routine is used to delete an area in SPI Flash, whereby the caller has set the address to the start of a page and limited the erase to a single storage area and device
//
static MAX_FILE_LENGTH fnDeleteSPI(ACCESS_DETAILS *ptrAccessDetails)
{
    MAX_FILE_LENGTH BlockLength = SPI_FLASH_PAGE_LENGTH;
    #if !defined SPI_FLASH_ST
    unsigned char   ucCommand;
    #endif
    #if !defined SPI_FLASH_SST25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    #endif
    #if defined SPI_FLASH_ST
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // enable the write
        #ifdef SPI_DATA_FLASH
    fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
    BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
        #else
    fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
    BlockLength = SPI_FLASH_SECTOR_LENGTH;
        #endif
    #elif defined SPI_FLASH_SST25
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow byte programming
        #ifndef SST25_A_VERSION
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_SECTOR_LENGTH - 1) == 0))) { // if a complete 64k sector can be deleted
        ucCommand = SECTOR_ERASE;                                        // delete block of 64k
        BlockLength = SPI_FLASH_SECTOR_LENGTH;
    }
    else 
        #endif
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
        ucCommand = HALF_SECTOR_ERASE;                                   // delete block of 32k
        BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
    }
    else {
        ucCommand = SUB_SECTOR_ERASE;                                    // delete smallest sector of 4k
        BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
    }
    fnSPI_command(ucCommand, ptrAccessDetails->ulOffset, _EXTENDED_CS 0, 0);    
    #else                                                                // ATMEL
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber % 8 == 0)) { // if delete range corresponds to a block, use faster block delete
        BlockLength = SPI_FLASH_BLOCK_LENGTH;
        ucCommand = BLOCK_ERASE;
    }
    else {
        BlockLength = SPI_FLASH_PAGE_LENGTH;
        ucCommand = PAGE_ERASE;
    }
    fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);           // delete appropriate page/block
    #endif
    return (BlockLength);
}

// Search for the memory type that the starting address is in, return the type and restrict the largest length that can be read,written, erased from that location
//
extern unsigned char fnGetStorageType(unsigned char *memory_pointer, ACCESS_DETAILS *ptrAccessDetails)
{
    unsigned long ulMaximumLength;
    STORAGE_AREA_ENTRY *ptrStorageList = UserStorageListPtr;
    #ifdef _WINDOWS                                                      // this is only used when reading with the simulator
    if (iFetchingInternalMemory != 0) {                                  // if the access is to program memory when simulating
        iFetchingInternalMemory = 0;                                     // reset single-shot program access
        return _STORAGE_PROGRAM;
    }
    #endif
    while (ptrStorageList != 0) {                                        // while there are storage areas defined
        if ((memory_pointer >= ptrStorageList->ptrMemoryStart) && (memory_pointer <= ptrStorageList->ptrMemoryEnd)) { // determine in which memory the access starts in
            if (ptrAccessDetails == 0) {
                return ptrStorageList->ucStorageType;
            }
            ptrAccessDetails->ucDeviceNumber = 0;
            if (ptrStorageList->ucDeviceCount > 1) {                     // if storage area is made up of multiple devices (assumed to be all the same size)
                unsigned long ulDeviceRangeLength = ((unsigned long)((ptrStorageList->ptrMemoryEnd - ptrStorageList->ptrMemoryStart) + 1)/ptrStorageList->ucDeviceCount);
                unsigned char *ptrStart = ptrStorageList->ptrMemoryStart;
                unsigned char *ptrEnd = (ptrStorageList->ptrMemoryStart + ulDeviceRangeLength);                
                while (ptrEnd < memory_pointer) {
                    ptrStart += ulDeviceRangeLength;
                    ptrEnd += ulDeviceRangeLength;
                    ptrAccessDetails->ucDeviceNumber++;                  // the device number that the access is in
                }
                ulMaximumLength = (unsigned long)(ptrEnd - memory_pointer);
                ptrAccessDetails->ulOffset = (unsigned long)(memory_pointer - ptrStart); // offset from start of memory
            }
            else {
                ulMaximumLength = (unsigned long)(ptrStorageList->ptrMemoryEnd - memory_pointer);
                ulMaximumLength++;                                       // compensate the fact that the end pointer is to the end of the last used address (see reason in STORAGE_AREA_ENTRY definition)
                ptrAccessDetails->ulOffset = (unsigned long)(memory_pointer - ptrStorageList->ptrMemoryStart); // offset from start of memory
            }
            if (ulMaximumLength < ptrAccessDetails->BlockLength) {
                ptrAccessDetails->BlockLength = (MAX_FILE_LENGTH)ulMaximumLength; // limit the length to the present storage device
            }
            return ptrStorageList->ucStorageType;                        // the storage area type
        }
        ptrStorageList = (STORAGE_AREA_ENTRY *)(ptrStorageList->ptrNext);// move to next storage area
    };
    _EXCEPTION("Attempted access to unsupported storage area!!!!");
    return _STORAGE_INVALID;                                             // attempted access outside of the defined storage areas
}
    #endif

// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
    #ifdef MANAGED_FILES
    MAX_FILE_LENGTH OriginalLength = Length;
    #endif
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    ACCESS_DETAILS AccessDetails;
    do {
        AccessDetails.BlockLength = Length;
        switch (fnGetStorageType(ptrSector, &AccessDetails)) {           // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        #if defined NVRAM && defined _M5225X                             // FlexBus
        case _STORAGE_PARALLEL_NVRAM:
            uMemset(fnGetFlashAdd(ptrSector), 0xff, AccessDetails.BlockLength);
            break;
        #endif
        case _STORAGE_INTERNAL_FLASH:
            fnFlashCommand(DELETE_FLASH, ptrSector, Length);             // delete the area requested (if the length is 0 it will delete just one sector)
            break;
        #if defined SPI_FLASH_ENABLED
        case _STORAGE_SPI_FLASH:
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(SPI_FLASH_PAGE_LENGTH - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(SPI_FLASH_PAGE_LENGTH - 1)); // set to sector boundary
            AccessDetails.BlockLength = fnDeleteSPI(&AccessDetails);     // delete page/block in SPI flash 
            break;
        #endif
        #if defined SPI_EEPROM_FILE_SYSTEM
        case _STORAGE_SPI_EEPROM:
            break;
        #endif
        #ifdef I2C_EEPROM_FILE_SYSTEM
        case _STORAGE_I2C_EEPROM:
            {
                CAST_POINTER_ARITHMETIC align_length = (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXTENSION_FILE_SIZE - 1)));
                Length += align_length;
                AccessDetails.BlockLength += align_length;
                ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXTENSION_FILE_SIZE - 1)); // set to sector boundary
                if (fnDeleteI2C_eeprom(&AccessDetails) < 0) {            // delete data in I2C EEPROM
                    return -1;
                }
            }
            break;
        #endif
        #ifdef EXT_FLASH_FILE_SYSTEM
        case _STORAGE_PARALLEL_FLASH:
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXT_FLASH_PAGE_LENGTH - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXT_FLASH_PAGE_LENGTH - 1)); // set to sector boundary
            AccessDetails.BlockLength = fnDeleteParallelFlash(&AccessDetails); // delete page/block in parallel flash 
            if (AccessDetails.BlockLength == 0) {
                return -1;                                               // error
            }
            break;
        #endif
        default:
            return -1;                                                   // invalid
        }
        ptrSector += AccessDetails.BlockLength;
        if (Length <= AccessDetails.BlockLength) {                       // if completed
        #ifdef MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size of the storage area
	            return AccessDetails.BlockLength;                        // return the page size that was deleted in the storage media
	        }
        #endif
            break;
        }
        Length -= AccessDetails.BlockLength;
    } while (1);
    #else                                                                // case when only internal Flash is available
    fnFlashCommand(DELETE_FLASH, ptrSector, Length);                     // delete the area requested (if the length is 0 it will delete just one sector)
    #endif
        #ifdef MANAGED_FILES
    if (OriginalLength == 0) {                                           // if a single page erase was called, return the page size of the storage area
        return fnGetFlashSectorRestSize((unsigned long)ptrSector);       // return the size of the sector that was deleted (from the location to the end of the sector)
	}
        #endif
    return 0;                                                            // success when the length of sector was not requested
}

    #if defined SPI_FLASH_ENABLED
// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    #if !defined SPI_FLASH_SST25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(ptrAccessDetails->ulOffset - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif

    #if defined SPI_FLASH_ST
    fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #elif defined SPI_FLASH_SST25
    fnSPI_command(READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #else                                                                // ATMEL
        #if SPI_FLASH_PAGE_LENGTH >= 1024
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #elif SPI_FLASH_PAGE_LENGTH >= 512
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #endif
    #endif
}

// This routine writes data from a buffer to an area in SPI Flash (the caller has already defined the data to a particular area and device)
//
static void fnWriteSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned long Destination = ptrAccessDetails->ulOffset;
    unsigned short usDataLength;
    #if defined SPI_FLASH_SST25
    int iMultipleWrites = 0;
    if (Length == 0) {
        return;                                                          // ignore if length is zero
    }
    if (Destination & 0x1) {                                             // start at odd SPI address, requires an initial byte write
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // command write enable to allow byte programming
        fnSPI_command(BYTE_PROG, Destination++, _EXTENDED_CS ptrBuffer++, 1);// program last byte 
        if (--Length == 0) {                                             // single byte write so complete
            return ;
        }
    }
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow programming
    #else
    unsigned short usPageNumber = (unsigned short)(Destination/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(Destination - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif
    while (Length != 0) {
        usDataLength = (unsigned short)Length;
    #if defined SPI_FLASH_ST
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // write enable
        fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength);// copy new content
        Length -= usDataLength;
        ptrBuffer += usDataLength;
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
                fnSPI_command(AAI_WORD_PROG, Destination, _EXTENDED_CS ptrBuffer, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                iMultipleWrites = 1;                                     // mark that we are in a AAI sequence
            }
            else {
                fnSPI_command(AAI_WORD_PROG, Destination, _EXTENDED_CS ptrBuffer, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
            }
            Destination += AAI_LENGTH;
            Length -= AAI_LENGTH;
            ptrBuffer += AAI_LENGTH;
        }
        else {
            if (iMultipleWrites != 0) {
                fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);      // first close AAI sequence
                fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);       // command write enable to allow byte programming
            }
            fnSPI_command(BYTE_PROG, Destination, _EXTENDED_CS ptrBuffer, 1); // program last byte 
            break;               
        }
    #else                                                                // ATMEL
        if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) {   // are we writing a partial page?
            fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
        }
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength);// copy new content
        fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        usPageNumber++;
        usPageOffset = 0;
    #endif
    }
    #if defined SPI_FLASH_SST25
    fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);                  // disable writes on exist
    #endif
}
    #endif

extern void fnGetParsFile(MEMORY_RANGE_POINTER ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
#endif
#if !defined NO_FLASH_SUPPORT
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    {
        ACCESS_DETAILS AccessDetails;
        AccessDetails.BlockLength = Size;
        while (Size != 0) {
            switch (fnGetStorageType(ParLocation, &AccessDetails)) {     // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        #ifndef _WINDOWS
            case _STORAGE_INVALID:                                       // read from unknown area is allowed on the HW so that SRAM (for example) could be read without explicity declaring it
        #endif
            case _STORAGE_INTERNAL_FLASH:
                uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), AccessDetails.BlockLength); // read from internal FLASH with no access restrictions so we can simply copy the data
                break;
            #if defined SPI_FLASH_ENABLED
            case _STORAGE_SPI_FLASH:
                fnReadSPI(&AccessDetails, ptrValue);                     // read from the SPI device
                break;
            #endif
            #if defined SPI_EEPROM_FILE_SYSTEM
            case _STORAGE_SPI_EEPROM:
                break;
            #endif
            #ifdef I2C_EEPROM_FILE_SYSTEM
            case _STORAGE_I2C_EEPROM:
                fnReadI2C_eeprom(&AccessDetails, ptrValue);              // read the data from the external device
                break;
            #endif
            #ifdef EXT_FLASH_FILE_SYSTEM
            case _STORAGE_PARALLEL_FLASH:
                uMemcpy(ptrValue, fnGetExtFlashAdd(ParLocation), AccessDetails.BlockLength); // read from external parallel FLASH with no access restrictions so we can simply copy the data
                break;
            #endif
            #ifdef _WINDOWS                                              // only when simulating
            case _STORAGE_PROGRAM:
                uMemcpy(ptrValue, ParLocation, Size);                            
                return;
            #endif
            default:
                _EXCEPTION("Invalid storage type");
                return;                                                  // invalid
            }
            if (Size == AccessDetails.BlockLength) {                     // if read is complete
                return;
            }
            Size -= AccessDetails.BlockLength;                           // decrease the amount of data remaining
            ParLocation += AccessDetails.BlockLength;                    // increase the destination address accordingly
            ptrValue += AccessDetails.BlockLength;                       // increase the buffer address accordingly
        }
    }
    #else
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // directly copy memory since this must be a pointer to code (embedded file)
    #endif
#endif
}

static int fnWriteInternalFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ucData)
{
    static unsigned char *ptrOpenBuffer = 0;
    unsigned char *ptrFlashBuffer;
    unsigned long ulBufferOffset;
    MAX_FILE_LENGTH BufferCopyLength;
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;    

    if (ucData == 0) {                                                   // close an open buffer
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & (FLASH_ROW_SIZE - 1));
        if (ulBufferOffset == 0) {
            return 0;                                                    // no open buffer so nothing to do
        }
        ulBufferOffset = FLASH_ROW_SIZE;                                 // cause the open buffer to be saved without copying any input data
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & ~(FLASH_ROW_SIZE - 1));
    }
    else {
        ptrOpenBuffer = (unsigned char *)(ptrAccessDetails->ulOffset & ~(FLASH_ROW_SIZE - 1));
        ulBufferOffset = (ptrAccessDetails->ulOffset & (FLASH_ROW_SIZE - 1));
    }
    do {
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {
            BufferCopyLength = Length;
        }
        ptrFlashBuffer = ucFlashRow + ulBufferOffset;                    // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= (ucFlashRow + FLASH_ROW_SIZE)) {           // a complete backup buffer is ready to be copied to FLASH
            fnFlashCommand(WRITE_FLASH_BUFFER, ptrOpenBuffer, 0);        // perform the write command
            uMemset(ucFlashRow, 0xff, FLASH_ROW_SIZE);                   // flush ready for next buffer use
            ptrFlashBuffer = ucFlashRow;                                 // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            ptrOpenBuffer += FLASH_ROW_SIZE;
        }
        else {                                                           // incomplete buffer collected
            ptrOpenBuffer += BufferCopyLength;
        }
    } while (Length != 0);
    return 0;
}

extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    ACCESS_DETAILS AccessDetails;
    AccessDetails.BlockLength = Length;
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    while (Length != 0) {
        switch (fnGetStorageType(ucDestination, &AccessDetails)) {       // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        case _STORAGE_INTERNAL_FLASH:
            if (fnWriteInternalFlash(&AccessDetails, ucData) != 0) {
                return 1;
            }
            break;
        #if defined SPI_FLASH_ENABLED
        case _STORAGE_SPI_FLASH:
            fnWriteSPI(&AccessDetails, ucData);
            break;
        #endif
        #if defined SPI_EEPROM_FILE_SYSTEM
        case _STORAGE_SPI_EEPROM:
            break;
        #endif
        #ifdef I2C_EEPROM_FILE_SYSTEM
        case _STORAGE_I2C_EEPROM:
            fnWriteI2C_eeprom(&AccessDetails, ucData);
            break;
        #endif
        #ifdef EXT_FLASH_FILE_SYSTEM
        case _STORAGE_PARALLEL_FLASH:
            if (fnWriteParallelFlash(&AccessDetails, ucData) != 0) {
                return 1;                                                // error
            }
            break;
        #endif
        default:
            return 1;                                                    // invalid
        }
        if (Length == AccessDetails.BlockLength) {                       // if write is complete
            return 0;
        }
        Length -= AccessDetails.BlockLength;                             // decrease the amount of data remaining
        ucDestination += AccessDetails.BlockLength;                      // increase the destination address accordingly
        ucData += AccessDetails.BlockLength;                             // increase the buffer address accordingly
    }
    return 0;
    #else
    AccessDetails.ulOffset = (unsigned long)ucDestination;
    return (fnWriteInternalFlash(&AccessDetails, ucData));
    #endif
}

#else                                                                    // revision 0 interface (used when STORAGE_REV_1 is disabled)

extern void fnGetParsFile(MEMORY_RANGE_POINTER ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef ACTIVE_FILE_SYSTEM
    #ifdef _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
    #endif
    #if defined SPI_FLASH_ENABLED
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ParLocation >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
        #else
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory
        #endif
    {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        #ifdef SPI_FLASH_SST25                                           // {23}
        ParLocation = fnGetSPI_FLASH_address(ParLocation EXTENDED_CS);
        #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
        #endif
        #if defined SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #elif defined SPI_FLASH_SST25                                    // {23}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, _EXTENDED_CS ptrValue, Size);
        #else
            #if SPI_FLASH_PAGE_LENGTH >= 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #elif SPI_FLASH_PAGE_LENGTH >= 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #endif
        #endif
        return;
    }
    #endif
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the LPC17xx uses a file system in FLASH with no access restrictions so we can simply copy the data
#endif
}

// The LPC17xx writes blocks of 256 (FLASH_ROW_SIZE) bytes therefore these are collected here until a block boundary is crossed or else a closing command is received
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    #if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA          // {15}
    static unsigned char *ptrOpenBuffer = 0;
    unsigned long ulBufferOffset; // = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1)); {16}
    unsigned char *ptrFlashBuffer;
    MAX_FILE_LENGTH BufferCopyLength;
    #endif

    #if defined SPI_FLASH_ENABLED                                        // {9}{29}
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)       // {29}
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))  // we are working from external SPI FLASH memory
        #endif
    {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
    #if defined SPI_FLASH_SST25                                          // {23}
        int iMultipleWrites = 0;
    #endif
        unsigned short usDataLength;
    #if defined SPI_FLASH_SST25                                          // {23}
        if (Length == 0) {
            return 0;                                                    // ignore if length is zero
        }
        ucDestination = fnGetSPI_FLASH_address(ucDestination EXTENDED_CS); // convert to virtual SPI Flash memory location
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
    #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;

        #ifdef FLASH_LINE_SIZE
        if (ucData == 0) {                                               // ignore command to close an open line buffer (compatibility with internal FLASH)
            return 0;
        }
        #endif
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
    #endif
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
    #if defined SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);        // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
    #elif defined SPI_FLASH_SST25                                        // {23}
        #ifdef SST25_A_VERSION
            #define AAI_LENGTH 1
        #else
            #define AAI_LENGTH 2
        #endif
            if (usDataLength > 1) {
                if (iMultipleWrites == 0) {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                    iMultipleWrites = 1;                                 // mark that we are in an AAI sequence
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
    #if defined SPI_FLASH_SST25                                          // {23}
        fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);              // disable writes
    #endif
        return 0;
    }
    #endif
    #if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA          // {15}
    if (ucData == 0) {                                                   // close an open buffer
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & (FLASH_ROW_SIZE - 1));
        if (ulBufferOffset == 0) {
            return 0;                                                    // no open buffer so nothing to do {16}
        }
        ulBufferOffset = FLASH_ROW_SIZE;                                 // cause the open buffer to be saved without copying any input data
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & ~(FLASH_ROW_SIZE - 1));
    }
    else {
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ucDestination & ~(FLASH_ROW_SIZE - 1));
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE - 1));
    }
    do {
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {
            BufferCopyLength = Length;
        }
        ptrFlashBuffer = ucFlashRow + ulBufferOffset;                    // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= (ucFlashRow + FLASH_ROW_SIZE)) {           // a complete backup buffer is ready to be copied to FLASH
            fnFlashCommand(WRITE_FLASH_BUFFER, ptrOpenBuffer, 0);        // perform the write command
            uMemset(ucFlashRow, 0xff, FLASH_ROW_SIZE);                   // flush ready for next buffer use
            ptrFlashBuffer = ucFlashRow;                                 // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            ptrOpenBuffer += FLASH_ROW_SIZE;
        }
        else {                                                           // {16} incomplete buffer collected
            ptrOpenBuffer += BufferCopyLength;
        }
    } while (Length != 0);
    #endif
    return 0;
}

// Erase FLASH sectors. The pointer can be anywhere in the first sector to be erased
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
#ifdef MANAGED_FILES                                                     // {45}
    MAX_FILE_LENGTH OriginalLength = Length;
#endif
#ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
#endif
#if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA              // {15}
    unsigned long ulPage = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // ensure page boundary
    ulPage -= FLASH_START_ADDRESS;
#endif
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
        #ifdef SPI_INTERFACE
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
        #if defined SPI_FLASH_ENABLED
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
            #else
        if ((ptrSector >= (uFILE_SYSTEM_END)) && (ptrSector < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory
            #endif
        {
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            #ifndef SPI_FLASH_ST
            MAX_FILE_LENGTH BlockLength;
            unsigned char  ucCommand;
            #endif
            #ifdef SPI_FLASH_SST25                                       // {23}
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
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #elif defined SPI_FLASH_SST25                                // {23}
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
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                BlockLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                BlockLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);   // delete appropriate page/block
            if (Length <= BlockLength) {
           #ifdef MANAGED_FILES                                          // {45}
                if (OriginalLength == 0) {                               // if a single block erase was called, return the page size of the storage area that was just deleted
                    return BlockLength;
	            }
           #endif
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #endif
            continue;
        }
        #endif
        #if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_AREA      // {15}
        fnFlashCommand(DELETE_FLASH, ptrSector, Length);
        #endif
        #ifdef MANAGED_FILES
        if (Length == 0) {                                               // completed
            if (OriginalLength == 0) {                                   // {45} if a single page erase was called, return the page size of the storage area
                return fnGetFlashSectorRestSize((unsigned long)ptrSector); // {50} return the size of the sector that was deleted (from the location to the end of the sector)
	        }
		}
        #endif
        break;
    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);
    #ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
    #endif
    return 0;
}
#endif                                                                   // end of revision 1 methods


#ifdef USE_PARAMETER_BLOCK

#define PARAMETER_BLOCK_1 (unsigned char*)(PARAMETER_BLOCK_START)
#define PARAMETER_BLOCK_2 (PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)
#define FLASH_SECTORS_IN_PAR_BLOCK (PARAMETER_BLOCK_SIZE/PARAMETER_BLOCK_GRANULARITY)

// The LPC17xx can not change lines of less that quadword length (16 bytes)
//
unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned long ulValidUse[(2 * FLASH_LINE_SIZE)/sizeof(unsigned long)];
    unsigned long ulCompare;

    // The first 2 long word locations of the first two FLASH lines defines the validity of the block. 0x55555555 0x55555555 means it is fully valid. 0xffffffff 0x55555555 means it contains temporary unvalidated data
    //
    fnGetParsFile(PARAMETER_BLOCK_1, (unsigned char*)ulValidUse, sizeof(ulValidUse));

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ulCompare = 0x55555555;                                          // we are looking for validated pattern
    }
    else {
        ulCompare = 0xffffffff;                                          // we are looking for temporary pattern
    }

    if ((ulValidUse[0] == 0x55555555) && (ulValidUse[FLASH_LINE_SIZE/sizeof(unsigned long)] == ulCompare)) {
        return 1;                                                        // first block matched request
    }
    #ifdef USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile(PARAMETER_BLOCK_2, (unsigned char*)ulValidUse, sizeof(ulValidUse));
        if ((ulValidUse[0] == 0x55555555) && (ulValidUse[FLASH_LINE_SIZE/sizeof(unsigned long)] == ulCompare)) {
            return 2;                                                    // second block matched request
        }
    }
    #endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}

static int fnDeleteParBlock(unsigned char *ptrAddInPar)
{
    #ifdef FLASH_FILE_SYSTEM
    return (fnFlashCommand(DELETE_FLASH, ptrAddInPar, PARAMETER_BLOCK_SIZE));
    #else
    return 0;
    #endif
}

// Due to the fact that individual bytes in quadwords (16 bytes) can not be modified more that once, one quadword is used for each byte to be stored.
// This allows the parameter system to fulfil its function but is not very efficient due to the high overhead.
//
extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1;

    #ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    #endif
    ptrPar += (usParameterReference * FLASH_LINE_SIZE);
    ptrPar += (2 * FLASH_LINE_SIZE);                                     // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile(ptrPar, ucValue++, 1);
        ptrPar += FLASH_LINE_SIZE;
    }
    return 0;
}

extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned long ulValid = 0x55555555;
    unsigned long ulValidCheck;
    unsigned char ucCheck[FLASH_LINE_SIZE];
    int iBlockUse = 0;
    int iCommitNewStatus = 0;
    #ifdef _WINDOWS                                                      // when simulating use a recognisable pattern
    uMemset(ucCheck, 0x33, FLASH_LINE_SIZE);
    #else
    uMemset(ucCheck, 0xff, FLASH_LINE_SIZE);
    #endif

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {                                  // no valid parameter blocks have been found so we can use the first for saving the data
        fnDeleteParBlock(PARAMETER_BLOCK_1);                             // we delete it to be absolutely sure it is fresh
    #ifdef USE_PAR_SWAP_BLOCK
        fnDeleteParBlock(PARAMETER_BLOCK_2);
    #endif
        ucValidBlock = 1;
    }

    #ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    #endif
    ptrStart = ptrPar;
    ptrPar += (usParameterReference * FLASH_LINE_SIZE);
    ptrPar += (2*FLASH_LINE_SIZE);                                       // first parameter starts after validation information (each occupying one quadword line)

    if (!usLength) {
        if (iBlockUse) {                                                 // if we have a new set waiting, set it to use state
            iBlockUse = 0;
    #ifdef USE_PAR_SWAP_BLOCK
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock(ptrPar);
    #endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock(ptrStart);
            if (usParameterReference) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
        while (usLength--) {
            ucCheck[0] = *ucValue;
            fnWriteBytesFlash(ptrPar, ucCheck, FLASH_LINE_SIZE);         // write each parameter to FLASH
            ucValue++;
            ptrPar += FLASH_LINE_SIZE;
        }
        if (((CAST_POINTER_ARITHMETIC)ptrPar & (FLASH_ROW_SIZE-1)) != 0) {
            if (ptrPar > (ptrStart + FLASH_ROW_SIZE)) {
                fnWriteBytesFlash(0, 0, 0);                              // commit any open FLASH row buffer
            }
        }
    }
                                                                         // now we must validate the parameter block which we have just written to
    fnGetParsFile(ptrStart, (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
    if (ulValidCheck != ulValid) {
        fnWriteBytesFlash(ptrStart, (unsigned char *)&ulValid, sizeof(ulValid)); // validate data
        iCommitNewStatus = 1;
    }
    if (!iBlockUse) {
        fnGetParsFile((ptrStart + FLASH_LINE_SIZE), (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
        if (ulValidCheck != ulValid) {
            fnWriteBytesFlash((ptrStart + FLASH_LINE_SIZE), (unsigned char *)&ulValid, sizeof(ulValid)); // use data
            iCommitNewStatus = 1;
        }
    }
    if (iCommitNewStatus != 0) {
        fnWriteBytesFlash(0, 0, 0);                                      // commit the parameter block status
    }
    return 0;
}
#endif





#ifdef CAN_INTERFACE

/**************************** CAN interrupt ******************************************/

static __interrupt void CAN_Interrupt(void)
{
    while (AIC_IPR & CAN_CTL) {                                          // while interrupts present from CAN
    }
}

// Hardware configuration of CAN controller
//
void fnConfigCAN(QUEUE_HANDLE DriverID, CANTABLE *pars)
{
    PMC_PCER = CAN_CTL;                                                  // enable the CAN clock in PMC

    AIC_SVR15 = (unsigned long)CAN_Interrupt;                            // enter the handling interrupt routine in the vector table
    AIC_SMR15 = 5;                                                       // give this priority 5 and level sensitive (0)
    AIC_IDCR = CAN_CTL;                                                  // disable the interrupt
    AIC_ICCR = CAN_CTL;                                                  // clear the interrupt
    AIC_IECR = CAN_CTL;                                                  // enable interrupt to core

    PIO_ASR_A  = (CAN_TX | CAN_TX);                                      // configure the pins for use as CAN
    PIO_PDR_A  = (CAN_TX | CAN_TX);

#ifdef _WINDOWS
    AIC_IMR  |= CAN_CTL;                                                 // set mask bit in simulator
#endif

    CAN_BR = CAN_SPEED_1MEG;                                             // configure CAN baud register before activating the CAN controller


    CAN_MAM0 = 0;                                                        // configure mail box identifiers
    CAN_MID0 = 0;
    CAN_MMR0 = RECEPTION_MAIL_BOX;

    CAN_IER = (CAN_BERR | CAN_FERR | CAN_AERR | CAN_SERR | CAN_CERR | CAN_TSTP | CAN_TOVF | CAN_WAKEUP | CAN_SLEEP | CAN_BOFF |
               CAN_ERRP | CAN_WARN | CAN_ERRA | CAN_MB7 | CAN_MB6 | CAN_MB5 | CAN_MB4 | CAN_MB3 | CAN_MB2 | CAN_MB1 | CAN_MB0);
    CAN_MR = CANEN;                                                      // enable operation
}


#endif

#ifdef IIC_INTERFACE
// Collect a pointer to defined I2C control block
//
static LPCXXX_IIC_CONTROL *fnSelectIICChannel(QUEUE_HANDLE Channel)
{
    switch (Channel) {
    case 0:
        return (LPCXXX_IIC_CONTROL *)I2C_BLOCK_0;
    case 1:
        return (LPCXXX_IIC_CONTROL *)I2C_BLOCK_1;
    case 2:
        return (LPCXXX_IIC_CONTROL *)I2C_BLOCK_2;
    }
    return 0;                                                            // invalid
}

// I2C interrupt handler
//
static void fnI2CInt(QUEUE_HANDLE Channel, LPCXXX_IIC_CONTROL *iic, IICQue *ptIICQue)
{
    unsigned long ulState = iic->I2CSTAT;
    switch (ulState) {
    case MASTER_REPEATED_START_SENT:                                     // (0x10) repeated start has been sent
    case MASTER_START_SENT:                                              // (0x08) start bit sent, now send address
        {
            unsigned long ulAddress = *ptIICQue->IIC_queue.get++;        // the address
            iic->I2CDAT = ulAddress;                                     // send the address of slave to write
            iic->I2CCONCLR = (IIC_STAC | IIC_SIC);                       // clear start bit and pending interrupt, which released transmission of address
            if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
                ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer; // handle circular buffer
            }
            if (ulAddress & 0x00000001) {                                // reading
                ptIICQue->ucState = (RX_ACTIVE | TX_ACTIVE);
                ptIICQue->IIC_queue.chars -= 3;                          // remove header (complete transmission message)
                IIC_rx_control[Channel]->wake_task = *ptIICQue->IIC_queue.get++; // enter task to be woken when reception has completed
                if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
                    ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer; // handle circular buffer
                }
        #ifdef _WINDOWS
                fnSimIIC_devices(IIC_ADDRESS, (unsigned char)iic->I2CDAT);
                iic->I2CSTAT = MASTER_RX_ADDRESS_ACKED;
                iInts |= (IIC_INT0 << Channel);
        #endif
            }
            else {
                ptIICQue->IIC_queue.chars--;                             // decrement the total length
        #ifdef _WINDOWS
                fnSimIIC_devices(IIC_ADDRESS, (unsigned char)iic->I2CDAT);
                iic->I2CSTAT = MASTER_TX_ADDRESS_ACKED;                  // next interrupt state
                iInts |= (IIC_INT0 << Channel);
        #endif
            }
        }
        break;

    case MASTER_TX_ADDRESS_NOT_ACKED:                                    // (0x20) the slave address has been sent but no ack was received
    case MASTER_TX_DATA_NOT_ACKED:                                       // (0x30) a byte of data has been sent but was not acked
    case MASTER_TX_DATA_ACKED:                                           // (0x28) a byte of data has been sent and acked
    case MASTER_TX_ADDRESS_ACKED:                                        // (0x18) the slave address was sent and we have received an ack - continue with the TX data
        if (ptIICQue->ucPresentLen != 0) {
            iic->I2CDAT = *ptIICQue->IIC_queue.get++;                    // send first data byte
            iic->I2CCONCLR = IIC_SIC;                                    // clear pending interrupt
            ptIICQue->ucPresentLen--;
            if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
                ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer; // handle circular buffer
            }
            ptIICQue->IIC_queue.chars--;                                 // decrement the total length
    #ifdef _WINDOWS
            fnSimIIC_devices(IIC_TX_DATA, (unsigned char)iic->I2CDAT);
            iic->I2CSTAT = MASTER_TX_DATA_ACKED;
            iInts |= (IIC_INT0 << Channel);
    #endif
        }
        else {
            iic->I2CCONSET = IIC_STO;                                    // set stop bit
            if (!(ptIICQue->IIC_queue.chars)) {                          // all transmission complete
                ptIICQue->ucState = 0;
                if ( ptIICQue->wake_task ) {
                   uTaskerStateChange( ptIICQue->wake_task, UTASKER_ACTIVATE);// wake up owner task since the transmission has terminated
                }
    #ifdef _WINDOWS
                iic->I2CSTAT = I2C_IDLE_STATE;
    #endif
            }
            else {
                fnTxIIC(ptIICQue, Channel);                              // we have another message to send so we can send a repeated start condition
            }
            iic->I2CCONCLR = IIC_SIC;                                    // clear pending interrupt
        }
        break;
    case MASTER_RX_ADDRESS_NOT_ACKED:                                    // (0x48) address has been sent but no ack received
    case MASTER_RX_ADDRESS_ACKED:                                        // (0x40) address has been sent and an ack received
        if (ptIICQue->ucPresentLen == 1) {
            iic->I2CCONCLR = (IIC_AAC);                                  // don't ack the single byte
    #ifdef _WINDOWS
            iic->I2CSTAT = MASTER_RX_DATA_NOT_ACKED;
    #endif
        }
        else {
            iic->I2CCONSET = IIC_AA;                                     // ensure reception is acknowledged
    #ifdef _WINDOWS
            iic->I2CSTAT = MASTER_RX_DATA_ACKED;
    #endif
        }
        iic->I2CCONCLR = IIC_SIC;                                        // clear pending interrupt and start read of first byte
    #ifdef _WINDOWS
        iic->I2CDAT = fnSimIIC_devices(IIC_RX_DATA, (unsigned char)iic->I2CDAT);// simulate the interrupt directly
        iInts |= (IIC_INT0 << Channel);
    #endif
        break;
    case MASTER_RX_DATA_ACKED:                                           // (0x50) data byte with ack has been received
    case MASTER_RX_DATA_NOT_ACKED:                                       // (0x58) data byte without ack has been received
        if (ptIICQue->ucPresentLen) {
            ptIICQue->ucPresentLen--;
            if (ptIICQue->ucPresentLen == 0) {
                iic->I2CCONCLR = (IIC_AAC);                              // don't ack last byte
     #ifdef _WINDOWS
                iic->I2CSTAT = MASTER_RX_DATA_NOT_ACKED;
     #endif
            }
            else {
                iic->I2CCONSET = IIC_AA;                                 // ensure reception is acknowledged
     #ifdef _WINDOWS
                iic->I2CSTAT = MASTER_RX_DATA_ACKED;
     #endif
            }
            *IIC_rx_control[Channel]->IIC_queue.put++ = (unsigned char)iic->I2CDAT;// read the byte
            iic->I2CCONCLR = IIC_SIC;                                    // clear pending interrupt and start next transfer
            IIC_rx_control[Channel]->IIC_queue.chars++;                  // and put it into the rx buffer
            if (IIC_rx_control[Channel]->IIC_queue.put >= IIC_rx_control[Channel]->IIC_queue.buffer_end) {
                IIC_rx_control[Channel]->IIC_queue.put = IIC_rx_control[Channel]->IIC_queue.QUEbuffer;
            }
     #ifdef _WINDOWS
            if (ptIICQue->ucPresentLen != 0) {
                iic->I2CDAT = fnSimIIC_devices(IIC_RX_DATA, (unsigned char)iic->I2CDAT);// simulate the interrupt directly
            }
            iInts |= (IIC_INT0 << Channel);
     #endif
        }
        else {
            iic->I2CCONSET = IIC_STO;                                    // send stop
            iic->I2CCONCLR = IIC_SIC;                                    // clear pending interrupt
     #ifdef _WINDOWS
            iic->I2CSTAT = I2C_IDLE_STATE;
     #endif
            ptIICQue->ucState = 0;
            IIC_rx_control[Channel]->msgs++;
            if ( IIC_rx_control[Channel]->wake_task ) {                  // wake up the receiver task if desired
               uTaskerStateChange(IIC_rx_control[Channel]->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        break;

    default:                                                             // no other states are expected
        iic->I2CCONSET = IIC_STO;                                        // send stop
        iic->I2CCONCLR = IIC_SIC;                                        // clear pending interrupt
        break;
    }
}

static __interrupt void IIC_Interrupt_0(void)
{
    fnI2CInt(0, (LPCXXX_IIC_CONTROL *)I2C_BLOCK_0, IIC_tx_control[0]);
}

static __interrupt void IIC_Interrupt_1(void)
{
    fnI2CInt(1, (LPCXXX_IIC_CONTROL *)I2C_BLOCK_1, IIC_tx_control[1]);
}

static __interrupt void IIC_Interrupt_2(void)
{
    fnI2CInt(2, (LPCXXX_IIC_CONTROL *)I2C_BLOCK_2, IIC_tx_control[2]);
}

// Configure the IIC hardware
//
extern void fnConfigIIC(IICTABLE *pars)
{
    LPCXXX_IIC_CONTROL *iic = fnSelectIICChannel(pars->Channel);
    switch (pars->Channel) {
    case 0:
        POWER_UP(PCI2C0);                                                // power up the I2C module
      //PCLKSEL0 |= PCLK_I2C0_CCLK_8;                                    // apply clock to it {9} performed in the main initialisation
    #if defined LPC1788
        #if defined I2C0_PORT1
        IOCON_P1_30 = _I2C0_P1_30_SDA;
        IOCON_P1_31 = _I2C0_P1_31_SCL;
        #elif defined I2C0_PORT5
        IOCON_P5_02 = _I2C0_P5_2_SDA;
        IOCON_P5_30 = _I2C0_P5_3_SCL;
        #else
        IOCON_P0_27 = _I2C0_P0_27_SDA;
        IOCON_P0_28 = _I2C0_P0_28_SCL;
        #endif
    #else
        PINSEL1 |= (PINSEL1_P0_27_SDA0 | PINSEL1_P0_28_SCL0);            // SDA0 and SCL0 on P0.27 and P0.28
    #endif
        fnEnterInterrupt(irq_I2C0_ID, PRIORITY_IIC0, IIC_Interrupt_0);   // configure and enter the handling interrupt routine in the vector table
        break;
    case 1:
        POWER_UP(PCI2C1);                                                // power up the I2C module
      //PCLKSEL1 |= PCLK_I2C1_CCLK_8;                                    // apply clock to it {9} performed in the main initialisation
    #if defined LPC1788
        #if defined I2C1_PORT2
        IOCON_P2_14 = _I2C1_P2_14_SDA;
        IOCON_P2_15 = _I2C1_P2_15_SCL;
        #elif defined I2C1_PORT0_HIGH
        IOCON_P0_19 = _I2C1_P0_19_SDA;
        IOCON_P0_20 = _I2C1_P0_20_SCL;
        #else
        IOCON_P0_00 = _I2C1_P0_0_SDA;
        IOCON_P0_01 = _I2C1_P0_1_SCL;
        #endif
    #else
        #if defined I2C1_PORT0_HIGH
        PINSEL1 |= (PINSEL1_P0_19_SDA1 | PINSEL1_P0_20_SCL1);            // SDA1 and SCL1 on P0.19 and P0.20
        #else
        PINSEL0 |= (PINSEL0_P0_0_SDA1 | PINSEL0_P0_1_SCL1);              // SDA1 and SCL1 on P0.0 and P0.1
        #endif
    #endif
        fnEnterInterrupt(irq_I2C1_ID, PRIORITY_IIC1, IIC_Interrupt_1);   // configure and enter the handling interrupt routine in the vector table
        break;
    case 2:
        POWER_UP(PCI2C2);                                                // power up the I2C module
      //PCLKSEL1 |= PCLK_I2C2_CCLK_8;                                    // apply clock to it {9} performed in the main initialisation
    #if defined LPC1788
        #if defined I2C2_PORT2
        IOCON_P2_30 = _I2C2_P2_30_SDA;
        IOCON_P2_31 = _I2C2_P2_31_SCL;
        #elif defined I2C2_PORT4
        IOCON_P4_20 = _I2C2_P4_20_SDA;
        IOCON_P4_21 = _I2C2_P4_21_SCL;
        #elif defined I2C2_PORT1_4
        IOCON_P1_15 = _I2C2_P1_15_SDA;
        IOCON_P4_29 = _I2C2_P4_29_SCL;
        #else
        IOCON_P0_10 = _I2C2_P0_10_SDA;
        IOCON_P0_11 = _I2C2_P0_11_SCL;
        #endif
    #else
        PINSEL0 |= (PINSEL0_P0_10_SDA2 | PINSEL0_P0_11_SCL2);            // SDA2 and SCL2 on P0.10 and P0.11
    #endif
        fnEnterInterrupt(irq_I2C2_ID, PRIORITY_IIC2, IIC_Interrupt_2);   // configure and enter the handling interrupt routine in the vector table
        break;
    default:
        return;                                                          // invalid channel
    }

    switch (pars->usSpeed) {
    case 400:                                                            // high speed IIC
        iic->I2CSCLL = ((IIC_CLOCK*6)/(400000*10));
        iic->I2CSCLH = ((IIC_CLOCK*4)/(400000*10));
        break;
    case 100:
    default:                                                             // default to 100kHz
        iic->I2CSCLL = ((IIC_CLOCK*6)/(100000*10));
        iic->I2CSCLH = ((IIC_CLOCK*4)/(100000*10));
        break;
    case 50:
        iic->I2CSCLL = ((IIC_CLOCK*6)/(50000*10));
        iic->I2CSCLH = ((IIC_CLOCK*4)/(50000*10));
        break;
    }
    #ifdef _WINDOWS
    fnConfigSimIIC(pars->Channel, (pars->usSpeed * 1000));
    #endif
}

// Start transmission to IIC bus
//
extern void fnTxIIC(IICQue *ptIICQue, QUEUE_HANDLE Channel)
{
    LPCXXX_IIC_CONTROL *iic = fnSelectIICChannel(Channel);
    iic->I2CCONSET = (IIC_I2EN + IIC_STA);                               // enable function and send start bit
    ptIICQue->ucPresentLen = *ptIICQue->IIC_queue.get++;                 // get present message length from the header
    if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
        ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;         // handle circular buffer
    }
    ptIICQue->ucState = (TX_ACTIVE);
    #ifdef _WINDOWS
    iic->I2CSTAT = MASTER_START_SENT;
    iInts |= (IIC_INT0 << Channel);
    #endif
}
#endif

#ifdef USB_INTERFACE

static unsigned long ulEndpointPriorities = 0;

// This routine is called from the USB driver with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    ulEndpointPriorities &= ~(0x00000001 << (ucEndpointRef * 2));
    USBEpIntPri = ulEndpointPriorities;                                  // move endpoint's interrupt back to slow priority so that it is handled
    return USB_BUFFER_NO_DATA;                                           // report no more data so that the driver doesn't call again. The interrupt routine will handle all waiting buffers itself.
}

// USB Macros
//
#define _USBMaxPSize(endpoint, size) USBDevIntClr = DEVICE_STATUS_EP_RLZED; USBEpInd = endpoint; USBMaxPSize = size; fnSetEndpointSize(); while (!(USBDevIntSt & DEVICE_STATUS_EP_RLZED)) {}// select the endpoint index and set its size
#define _USB_CMD_DATA(data, data_phase) ((data << 16) | data_phase)

#ifdef _WINDOWS
    extern unsigned long fnUSB_command_interpreter(unsigned long ulCommand, unsigned long ulData);
    extern unsigned long fnUSB_command_command(unsigned long ulCommand, unsigned long ulData);
    extern unsigned long fnGetUSB_from_FIFO(void);
#else
    #define fnGetUSB_from_FIFO() USBRxData
#endif

#ifdef _WINDOWS
static unsigned short usEndpointSizes[NUMBER_OF_USB_ENDPOINTS * 2] = {8, 8, 0}; // control endpoints default to 8 [OUT / IN]

static void fnSetEndpointSize(void)
{
    usEndpointSizes[USBEpInd & 0x1f] = (unsigned short)USBMaxPSize;      // set the endpoint size and recalculate the internal RAM buffer address
    USBDevIntSt |= DEVICE_STATUS_EP_RLZED;                               // once the process has completed the realised flag will be set
}
#else
    #define fnSetEndpointSize()
#endif

#if (defined USB_DMA_RX || defined USB_DMA_TX) && defined USB_RAM_START
    #ifdef _WINDOWS
static unsigned char ucUSB_Memory[USB_RAM_SIZE];
extern unsigned char *fnGetUSB_mem(void)
{
    return ucUSB_Memory;
}
    #endif


// Memory allocation and alignment in LAN RAM block
//
static void *uMallocUSB(unsigned short __size)
{
    #ifdef _WINDOWS
    static unsigned char *pucBottomOfHeap = ucUSB_Memory;
    static unsigned char *pucTopOfHeap = (ucUSB_Memory + USB_RAM_SIZE);
    #else
    static unsigned char *pucBottomOfHeap = (unsigned char *)(USB_RAM_START);
    static unsigned char *pucTopOfHeap = (unsigned char *)(USB_RAM_START + USB_RAM_SIZE);
    #endif
    static unsigned short present_HeapSize = 0;

    unsigned char *ptr = (pucBottomOfHeap + present_HeapSize);
    if ((ptr + __size) >= pucTopOfHeap) {
        _EXCEPTION("USB heap allocation failure!!");
        return 0;                                                        // no more room in USB ram space
    }

    uMemset(ptr, 0, __size);                                             // ensure new memory chunk is always zeroed

    present_HeapSize = (ptr - pucBottomOfHeap) + __size;
    return ((void *)ptr);
}

    #ifdef _WINDOWS
static unsigned char *_putUSB_RAM(unsigned char *ptrUSB_RAM)
{
    return &ucUSB_Memory[ptrUSB_RAM - (unsigned char *)USB_RAM_START];
}
    #else
        #define _putUSB_RAM(x) x
    #endif


// Allocate USB buffer in USB memory
//
extern int fnAllocateUSBBuffer(QUEUE_HANDLE DriverID, USB_ENDPOINT *usb_endpoint_queue, QUEUE_DIMENSIONS *ptrQueueDimensions)
{
    unsigned char *new_memory;
    IDINFO *ptQueID = &que_ids[DriverID - 1];
    #if defined USB_DMA_RX && defined USB_RAM_START
    if ((ptrQueueDimensions->RxQueueSize != 0) && (ptQueID->input_buffer_control != 0)) {
        DMA_DESCRIPTOR_ISO *buffer_descriptor;
        unsigned long *ptrDMA_desc = (unsigned long *)USB_RAM_START;

        if (NO_MEMORY == (new_memory = (unsigned char *)uMallocUSB(ptrQueueDimensions->RxQueueSize))) {
            return (NO_MEMORY);
        }        

        ptQueID->input_buffer_control->buffer_end = ptQueID->input_buffer_control->QUEbuffer = (unsigned char *)new_memory;
        ptQueID->input_buffer_control->get = new_memory;
        ptQueID->input_buffer_control->put = new_memory;
      //ptQueID->input_buffer_control->chars = 0;
        ptQueID->input_buffer_control->buffer_end += ptrQueueDimensions->RxQueueSize;
        ptQueID->input_buffer_control->buf_length = ptrQueueDimensions->RxQueueSize;

        ptrDMA_desc += (usb_endpoint_queue->ucEndpointNumber * 2);
        buffer_descriptor = (DMA_DESCRIPTOR_ISO *)uMallocUSB(sizeof(DMA_DESCRIPTOR_ISO)); // allocate a single descriptor for this endpoint
        *(CAST_POINTER_ARITHMETIC *)_putUSB_RAM((unsigned char *)ptrDMA_desc) = (CAST_POINTER_ARITHMETIC)buffer_descriptor;
        buffer_descriptor->dma_mode_status = ((usb_endpoint_queue->usMax_frame_length << 5) | (ptrQueueDimensions->RxQueueSize << 16));
        buffer_descriptor->buffer_start_address = buffer_descriptor->ptrBufferStart = new_memory;
        buffer_descriptor->ptrBufferEnd = (new_memory + ptrQueueDimensions->RxQueueSize);
    }
    #endif
    #if defined USB_DMA_TX && defined USB_RAM_START
    if ((ptrQueueDimensions->TxQueueSize != 0) && (ptQueID->output_buffer_control != 0)) {
        DMA_DESCRIPTOR_ISO *buffer_descriptor;
        unsigned long *ptrDMA_desc = (unsigned long *)USB_RAM_START;
        if (NO_MEMORY == (new_memory = (unsigned char *)uMallocUSB(ptrQueueDimensions->TxQueueSize))) {
            return (NO_MEMORY);
        }

        ptQueID->output_buffer_control->buffer_end = ptQueID->output_buffer_control->QUEbuffer = (unsigned char *)new_memory;
        ptQueID->output_buffer_control->get = new_memory;
        ptQueID->output_buffer_control->put = new_memory;
      //ptQueID->output_buffer_control->chars = 0;
        ptQueID->output_buffer_control->buffer_end += ptrQueueDimensions->TxQueueSize;
        ptQueID->output_buffer_control->buf_length = ptrQueueDimensions->TxQueueSize;

        if (usb_endpoint_queue->ucPaired_IN != 0) {
            ptrDMA_desc += ((usb_endpoint_queue->ucPaired_IN * 2) + 1);
        }
        else {
            ptrDMA_desc += ((usb_endpoint_queue->ucEndpointNumber * 2) + 1);
        }
        buffer_descriptor = (DMA_DESCRIPTOR_ISO *)uMallocUSB(sizeof(DMA_DESCRIPTOR_ISO)); // allocate a first descriptor for this endpoint
        *(CAST_POINTER_ARITHMETIC *)_putUSB_RAM((unsigned char *)ptrDMA_desc) = (CAST_POINTER_ARITHMETIC)buffer_descriptor;
        buffer_descriptor->dma_mode_status = (usb_endpoint_queue->usMax_frame_length << 5);
        buffer_descriptor->ptrBufferStart = new_memory;
        buffer_descriptor->ptrBufferEnd = (new_memory + ptrQueueDimensions->TxQueueSize);
        buffer_descriptor->next_dd_pointer = (unsigned long)uMallocUSB(sizeof(DMA_DESCRIPTOR_ISO)); // allocate a second descriptor for this endpoint to allow warapping from circular buffer
        buffer_descriptor = (DMA_DESCRIPTOR_ISO *)buffer_descriptor->next_dd_pointer;
        buffer_descriptor->dma_mode_status = (usb_endpoint_queue->usMax_frame_length << 5);
        buffer_descriptor->buffer_start_address = buffer_descriptor->ptrBufferStart = new_memory;
        buffer_descriptor->ptrBufferEnd = (new_memory + ptrQueueDimensions->TxQueueSize);
    }
    #endif
    return 1;
}


    #if defined USB_DMA_TX && defined USB_RAM_START
// Start DMA transmission from USB buffer - called with disabled interrupts
//
extern QUEUE_TRANSFER fnPrepareUSBOutData(USBQUE *ptrQueue, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW)
{
    DMA_DESCRIPTOR_ISO *buffer_descriptor;
    unsigned long ulEndpointFlag = (0x00000001 << ((iEndpoint * 2) + 1));
    unsigned long *ptrDMA_desc = (unsigned long *)USB_RAM_START;
    ptrDMA_desc += ((iEndpoint * 2) + 1);
    buffer_descriptor = (DMA_DESCRIPTOR_ISO *)*(unsigned long *)_putUSB_RAM((unsigned char *)ptrDMA_desc);
    buffer_descriptor->dd_mode_status = 0;
    USBNDDRIntClr = ulEndpointFlag;
    buffer_descriptor->buffer_start_address = ptrQueue->USB_queue.get;
    buffer_descriptor->dma_mode_status &= ~(DD_PRESENT_DMA_COUNT | NEXT_DD_VALID);
    if ((ptrQueue->USB_queue.get + usLength) <= ptrQueue->USB_queue.buffer_end) { // single DMA descriptor can be used
        buffer_descriptor->dma_mode_status |= (usLength << 16);
    }
    else {
        ((DMA_DESCRIPTOR_ISO *)(buffer_descriptor->next_dd_pointer))->buffer_start_address = buffer_descriptor->ptrBufferStart;
        ((DMA_DESCRIPTOR_ISO *)(buffer_descriptor->next_dd_pointer))->dma_mode_status = (buffer_descriptor->dma_mode_status | ((((unsigned short)((ptrQueue->USB_queue.get + usLength) - ptrQueue->USB_queue.buffer_end) << 16))));
        ((DMA_DESCRIPTOR_ISO *)(buffer_descriptor->next_dd_pointer))->dd_mode_status = 0;
        buffer_descriptor->dma_mode_status |= ((((unsigned short)(ptrQueue->USB_queue.buffer_end - ptrQueue->USB_queue.get)) << 16) | NEXT_DD_VALID);        
    }
    USBEpDMAEn = ulEndpointFlag;
    USBDMARSet = ulEndpointFlag;                                         // trigger DMA transmission
    return usLength;
}
    #endif
#endif


// USB command 
//
extern void fnUSB_command(unsigned long ulUSB_CMD, unsigned long ulCommandData)
{
    USBCmdCode = ulUSB_CMD;
    while (!(USBDevIntSt & DEVICE_STATUS_CCEMPT)) {
#ifdef _WINDOWS
        USBDevIntSt |= DEVICE_STATUS_CCEMPT;
#endif
    }
    USBDevIntClr = (DEVICE_STATUS_CCEMPT);
    if (ulCommandData != 0) {                                            // optional data phase
        USBCmdCode = ulCommandData;
        while (!(USBDevIntSt & DEVICE_STATUS_CCEMPT)) {
    #ifdef _WINDOWS
            USBDevIntSt |= DEVICE_STATUS_CCEMPT;
    #endif
        }
        USBDevIntClr = (DEVICE_STATUS_CCEMPT);
    }
#ifdef _WINDOWS                                                          // interpret the command
    fnUSB_command_command(ulUSB_CMD, ulCommandData);
#endif
}


// USB status read 
//
static unsigned char fnUSB_status(unsigned long ulUSB_CMD, unsigned long ulCommandData)
{
    USBCmdCode = ulUSB_CMD;
    while (!(USBDevIntSt & DEVICE_STATUS_CCEMPT)) {
#ifdef _WINDOWS
        USBDevIntSt |= DEVICE_STATUS_CCEMPT;
#endif
    }
    USBDevIntClr = (DEVICE_STATUS_CCEMPT | DEVICE_STATUS_CDFULL);
    USBCmdCode = ulCommandData;
    while (!(USBDevIntSt & DEVICE_STATUS_CCEMPT)) {
#ifdef _WINDOWS
        USBDevIntSt |= DEVICE_STATUS_CCEMPT;
#endif
    }
    USBDevIntClr = (DEVICE_STATUS_CCEMPT);
    while (!(USBDevIntSt & DEVICE_STATUS_CDFULL)) {
#ifdef _WINDOWS
        USBDevIntSt |= DEVICE_STATUS_CDFULL;
#endif
    }
#ifdef _WINDOWS                                                          // interpret the command
    USBCmdData = fnUSB_command_interpreter(ulUSB_CMD, ulCommandData);
#endif
    return (unsigned char)USBCmdData;
}


// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *ptrUSB_HW, unsigned char ucFrameType)
{
    switch (fnUSB_handle_frame(ucFrameType, (unsigned char *)ptrUSB_HW->ptrDatBuffer, iEndpoint_ref, ptrUSB_HW)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref);
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        return MAINTAIN_OWNERSHIP;
    case STALL_ENDPOINT:                                                 // send stall
        if (iEndpoint_ref == 0) {                                        // check whether control 0 endpoint
            fnUSB_command((USB_CMD_SET_EP_STATUS | (((iEndpoint_ref * 2) + 1) << 16)), USB_CMD_SET_EP_STATUS_STALL); // stall the control endpoint - this will be automatically cleard by next SETUP token
        }
        else {                                                           // assume IN endpoint is always one higher than OUT endpoint
            iEndpoint_ref = fnGetPairedIN(iEndpoint_ref);
            fnUSB_command((USB_CMD_SET_EP_STATUS | (((iEndpoint_ref * 2) + 1) << 16)), USB_CMD_SET_EP_STATUS_STALL); // stall the control endpoint
            fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);  // mark that the endpoint is stalled
        }
        _SIM_USB(USB_SIM_STALL, iEndpoint_ref, ptrUSB_HW);
        break;
    case CRITICAL_OUT:
        ulEndpointPriorities |= (0x00000001 << (iEndpoint_ref * 2));
        USBEpIntPri = ulEndpointPriorities;                              // move the endpoint interrupt to fast priority, which is disabled
        break;
    }
    return 0;
}


static void fnUSB_reset(int iInitialise)
{
    _USBMaxPSize(0, ENDPOINT_0_SIZE);                                    // set the size of logical control endpoint 0
    _USBMaxPSize(1, ENDPOINT_0_SIZE);
    USBEpIntClr     = ALL_ENDPOINTS;                                     // clear all physical endpoint interrupts
    USBDevIntClr    = ALL_USED_USB_INTERRUPTS;                           // clear all device interrupts
#if (defined USB_DMA_RX || defined USB_DMA_TX) && defined USB_RAM_START  // {28} if the processor supports DMA activate it
    USBDMAIntEn     = 0;
    if (iInitialise != 0) {
        USBUDCAH = USB_RAM_START;                                        // set head to start of USB RAM, which is automatically aligned to 128 bytes
        uMallocUSB(32 * sizeof(unsigned long));                          // allocate DMA descriptors for each physical endpoint
    }
    USBEpDMADis     = DMA_ENDPOINTS;                                     // disable DMA operation on all DMA capable endpoints
    USBDMARClr      = DMA_ENDPOINTS;                                     // clear and pending DMA requests
    USBEoTIntClr    = ALL_ENDPOINTS;
    USBNDDRIntClr   = ALL_ENDPOINTS;
    USBSysErrIntClr = ALL_ENDPOINTS;
#endif
    USBEpIntEn      = (EP0RX | EP0TX);                                   // enable interrupts on the control endpoint
    USBDevIntEn     = (ALL_USED_USB_INTERRUPTS & ~DEVICE_STATUS_EP_FAST);// enable used device interrupts
}

// Read FIFO input to a buffer
//
static void fnReadUSB(unsigned char *ptrBuffer, unsigned short usLength)
{
    register unsigned long ulTemp;
    while (1) {
        ulTemp = fnGetUSB_from_FIFO();
        *ptrBuffer++ = (unsigned char)ulTemp;
        *ptrBuffer++ = (unsigned char)(ulTemp >> 8);
        *ptrBuffer++ = (unsigned char)(ulTemp >> 16);
        *ptrBuffer++ = (unsigned char)(ulTemp >> 24);
        if (usLength <= 4) {
            break;
        }
        usLength -= 4;
    }
}


// USB Device interrupt handler
//
static __interrupt void USB_Interrupt(void)
{
    static USB_HW usb_hardware = {0};
#if (defined USB_DMA_TX || defined USB_DMA_RX) && defined USB_RAM_START 
    while (USBDMAIntSt & USBDMAIntEn) {                                  // DMA interrupts given precedence 
        if (USBDMAIntSt & USB_DMA_EOT) {
            register unsigned long ulEndpoints = USBEoTIntSt;
            register unsigned long ulRequest = 0x00000004;               // first possible DMA reques
            register int iEndpoint = 2;
            unsigned long *ptrDMA_desc = (unsigned long *)(USB_RAM_START + 8);
    #if defined USB_DMA_RX
            DMA_DESCRIPTOR_ISO *buffer_descriptor;
            unsigned short usRxLength;
    #endif
            while (ulEndpoints != 0) {
    #if defined USB_DMA_RX
                if (ulRequest & ulEndpoints) {                           // OUT endpoints
                    USBEoTIntClr = ulRequest;                            // clear the request
        #ifdef _WINDOWS
                    USBEoTIntSt &= ~USBEoTIntClr; USBEoTIntClr = 0;
        #endif
                    ulEndpoints &= ~ulRequest;
                    buffer_descriptor = (DMA_DESCRIPTOR_ISO *)*(unsigned long *)_putUSB_RAM((unsigned char *)ptrDMA_desc);
                    usRxLength = (unsigned short)(buffer_descriptor->dd_mode_status >> 16);
                    fnEndpointData((iEndpoint/2), 0, usRxLength, OUT_DATA_RECEPTION);
                    buffer_descriptor->dma_mode_status &= ~DD_PRESENT_DMA_COUNT;
                    if (buffer_descriptor->ptrBufferEnd == buffer_descriptor->buffer_start_address) {
                        buffer_descriptor->buffer_start_address = buffer_descriptor->ptrBufferStart;
                    }
                    usRxLength = (unsigned short)(buffer_descriptor->ptrBufferEnd - buffer_descriptor->buffer_start_address);
                    buffer_descriptor->dma_mode_status |= (usRxLength << 16);
                    buffer_descriptor->dd_mode_status = 0;               // allow the DMA descriptor to be re-used
                }
    #endif
                ulRequest <<= 1;
                iEndpoint++;
                ptrDMA_desc++;
    #if defined USB_DMA_TX
                if (ulRequest & ulEndpoints) {                           // IN endpoints
                    USBEoTIntClr = ulRequest;                            // clear the request
        #ifdef _WINDOWS
                    USBEoTIntSt &= ~USBEoTIntClr; USBEoTIntClr = 0;
        #endif
                    ulEndpoints &= ~ulRequest;
                  //fnUSB_handle_frame(USB_TX_ACKED, 0, (iEndpoint/2), &usb_hardware);
                }
                ulRequest <<= 1;
                iEndpoint++;
                ptrDMA_desc++;
    #endif
            }
    #ifdef _WINDOWS
            USBDMAIntSt = 0;
    #endif
        }
    }
#endif
    while (USBDevIntSt & USBDevIntEn) {                                  // while enabled device interrupts pending
        if (DEVICE_STATUS_DEV_STAT & USBDevIntSt) {
            unsigned char ucDeviceStatus;
            USBDevIntClr = DEVICE_STATUS_DEV_STAT;
    #ifdef _WINDOWS
            USBDevIntSt &= ~USBDevIntClr; USBDevIntClr = 0;
    #endif
            ucDeviceStatus = fnUSB_status(USB_CMD_GET_DEVICE_STATUS, USB_CMD_GET_DEVICE_STATUS_DATA_PHASE);
            if (ucDeviceStatus & USB_STATUS_RST) {                       // reset detected
                fnUSB_reset(0);
                fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware);// generic handler routine
                usb_hardware.ucUSBAddressValid = 0;
                usb_hardware.ucUSBAddress = 0;
            }
            else if (ucDeviceStatus & USB_STATUS_SUS_CH) {               // suspend state changed
                if (ucDeviceStatus & USB_STATUS_SUS) {                   // suspended -  - 3ms of idle USB bus detected (low speed D+ = 0, D- = 1 / high speed D+ = 1, D- = 0)
                    fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);   // generic handler routine
                }
                else {                                                   // resume - 10ms inverted idle USB bus state (low speed D+ = 1, D- = 0 / high speed D+ = 0, D- = 1)
                    fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);    // generic handler routine
                }
            }
            else if (ucDeviceStatus & USB_STATUS_COM_CH) {               // connection state changed
                if (ucDeviceStatus & USB_STATUS_COM) {                   // connected
                }
                else {                                                   // disconnected
                }
            }
        }
        else if ((/*DEVICE_STATUS_EP_FAST | */DEVICE_STATUS_EP_SLOW) & USBDevIntSt) {
            register unsigned long ulEndpoints = USBEpIntSt;             // see which endpoint(s) is/are requesting service
            register unsigned long ulEndpoint = 0x00000001;
            register unsigned long ulEndpointRef = 0x00000000;
            int iEndpointNumber = 0;
            unsigned char uctype;
            while (ulEndpoints != 0) {
                if (ulEndpoints & ulEndpoint) {                          // OUT endpoint
                    uctype = fnUSB_status((USB_CMD_SELECT_EP | ulEndpointRef), (USB_CMD_SELECT_EP_DATA_PHASE | ulEndpointRef));
                    USBEpIntClr = ulEndpoint;                            // clear the endpoint interrupt
#ifdef _WINDOWS
                    USBEpIntSt &= ~USBEpIntClr; USBEpIntClr = 0;
#endif
                    ulEndpoints &= ~ulEndpoint;                          // this endpoint interrupt no longer needs handling
                    if (ulEndpointRef == 0) {                            // OUT on control endpoint 0
                        unsigned long ulLength;
                        unsigned char ucRxData[64];                      // temporary buffer
                        USBCtrl = CTRL_RD_EN;                            // enable read on the endpoint input buffer
                        do {
                            ulLength = USBRxPLen;
                        } while ((ulLength & PKT_RDY) == 0);             // get the length of data waiting in the FIFO
                        ulLength &= (PKT_LENGTH_MASK);
                        fnReadUSB(ucRxData, (unsigned short)ulLength);   // read the data from FIFO to temporary buffer
                        USBCtrl = 0;                                     // disable endpoint read on completion
                        USBCmdCode = USB_CMD_CLEAR_EP_BUFFER;            // free the buffer
                        while (!(USBDevIntSt & DEVICE_STATUS_CCEMPT)) {
#ifdef _WINDOWS
                            USBDevIntSt |= DEVICE_STATUS_CCEMPT;
#endif
                        }
                        USBDevIntClr = (DEVICE_STATUS_CCEMPT);
                        usb_hardware.ptrDatBuffer = ucRxData;
                        usb_hardware.usLength = (unsigned short)ulLength;
                        if (uctype & USB_EP_SETUP_PACKET_RECEIVE) {      // setup packet received
                            if (fnProcessInput(0, &usb_hardware, USB_SETUP_FRAME) != MAINTAIN_OWNERSHIP) {
//                              usb_endpoints[0].ulNextRxData0 |= (RX_DATA_TOGGLE);
                            }
                        }
                        else {
                            if (fnProcessInput(0, &usb_hardware, USB_OUT_FRAME) != MAINTAIN_OWNERSHIP) {
//                              usb_endpoints[0].ulNextRxData0 |= (RX_DATA_TOGGLE);
                            }
                        }
                    }
                    else {
                        unsigned long ulLength;
                        unsigned char ucRxData[64];

                        USBCtrl = ((iEndpointNumber << 2) | CTRL_RD_EN); // enable read on the endpoint input buffer
                        do {
                            ulLength = USBRxPLen;
                        } while ((ulLength & PKT_RDY) == 0);
                        ulLength &= (PKT_LENGTH_MASK);
                        fnReadUSB(ucRxData, (unsigned short)ulLength);
                        USBCtrl = 0;                                 // disable endpoint read
                        USBCmdCode = USB_CMD_CLEAR_EP_BUFFER;
                        while (!(USBDevIntSt & DEVICE_STATUS_CCEMPT)) {
#ifdef _WINDOWS
                            USBDevIntSt |= DEVICE_STATUS_CCEMPT;
#endif
                        }
                        USBDevIntClr = (DEVICE_STATUS_CCEMPT);
                        usb_hardware.ptrDatBuffer = ucRxData;
                        usb_hardware.usLength = (unsigned short)ulLength;

                        if (fnProcessInput(iEndpointNumber, &usb_hardware, USB_OUT_FRAME) != MAINTAIN_OWNERSHIP) {
//                          usb_endpoints[0].ulNextRxData0 |= (RX_DATA_TOGGLE);
                        }
                    }
                }
                ulEndpoint <<= 1;                                        // move to IN endpoint
                ulEndpointRef += EP_INC;                                 // increment endpoint reference in command
                if (ulEndpoints & ulEndpoint) {                          // IN endpoint
                    uctype = fnUSB_status((USB_CMD_SELECT_EP | ulEndpointRef), (USB_CMD_SELECT_EP_DATA_PHASE | ulEndpointRef));
                    USBEpIntClr = ulEndpoint;
#ifdef _WINDOWS
                    USBEpIntSt &= ~USBEpIntClr; USBEpIntClr = 0;
#endif
                    ulEndpoints &= ~ulEndpoint;

#if defined USB_DMA_TX
                    if (iEndpointNumber == 0) {
                        fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpointNumber, &usb_hardware); // handle ack event
                        if ((usb_hardware.ucUSBAddressValid == 0) && (usb_hardware.ucUSBAddress != 0)) { // if a new address is waiting program it now
                            fnUSB_command(USB_CMD_SET_ADDRESS, _USB_CMD_DATA((usb_hardware.ucUSBAddress | DEV_EN), USB_CMD_SET_ADDRESS_DATA_PHASE)); // set address
                            fnUSB_command(USB_CMD_SET_ADDRESS, _USB_CMD_DATA((usb_hardware.ucUSBAddress | DEV_EN), USB_CMD_SET_ADDRESS_DATA_PHASE)); // set address with immediate effect
                            usb_hardware.ucUSBAddressValid = 1;
                        }
                    }
                    else {
                        fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpointNumber, &usb_hardware);
                    }
#else
                    fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpointNumber, &usb_hardware); // handle ack event
                    if ((usb_hardware.ucUSBAddressValid == 0) && (usb_hardware.ucUSBAddress != 0)) { // if a new address is waiting program it now
                        fnUSB_command(USB_CMD_SET_ADDRESS, _USB_CMD_DATA((usb_hardware.ucUSBAddress | DEV_EN), USB_CMD_SET_ADDRESS_DATA_PHASE)); // set address
                        fnUSB_command(USB_CMD_SET_ADDRESS, _USB_CMD_DATA((usb_hardware.ucUSBAddress | DEV_EN), USB_CMD_SET_ADDRESS_DATA_PHASE)); // set address with immediate effect
                        usb_hardware.ucUSBAddressValid = 1;
                    }
#endif
                }
                ulEndpoint <<= 1;                                        // move to OUT endpoint
                ulEndpointRef += EP_INC;                                 // increment endpoint reference in command
                iEndpointNumber++;
            }
            USBDevIntClr = (DEVICE_STATUS_EP_SLOW);
#ifdef _WINDOWS
            USBDevIntSt &= ~USBDevIntClr; USBDevIntClr = 0;
#endif
        }
    }
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    POWER_UP(PCUSB);                                                     // power up the USB controller module
    if (pars->ucClockSource == EXTERNAL_USB_CLOCK) {                     // use PLL0 to generate the clock from the external crystal
                                                                         // configure PLL1 for 48MHz
        PLL1CFG = PLL_USB_FREQUENCY_CONFIGURATION;                       // PLL setting to generate required frequency
        #if !defined LPC1788
        PLL1FEED = 0xaa;PLL1FEED = 0x55;                                 // perform feed sequence so that register changes are accepted
        #endif
        PLL1CON = PLLE;                                                  // enable the PLL to search for lock
        PLL1FEED = 0xaa;PLL1FEED = 0x55;                                 // perform feed sequence so that register changes are accepted
        while (!(PLL1STAT & PLL1LOCKED)) {                               // wait for lock to be achieved
        #ifdef _WINDOWS
            PLL1STAT |= PLL1LOCKED;
        #endif
        }
        #if defined LPC1788                                              // {20}
        USBCLKCFG = USBCLKCFG_USBSEL_ALT_PLL;                            // select the alternative PLL (divide by 1 - assume input is 48Mhz and not a multiple of it)
        #else
        PLL1CON = (PLLE | PLLC);                                         // connect PLL to clock CPU
        PLL1FEED = 0xaa;PLL1FEED = 0x55;                                 // perform feed sequence so that register changes are accepted
        #endif
    }
    else {
        USBCLKCFG = USB_FREQUENCY_CONFIGURATION;                         // generate 48MHz USB clock from main clock
    }    
    while (USBClkSt != (DEV_CLK_EN | AHB_CLK_EN)) {                      // enable device controller clocks and wait for AHB and device clocks to be on
        USBClkCtrl = (DEV_CLK_EN | AHB_CLK_EN);
    #if defined _WINDOWS
        USBClkSt = (DEV_CLK_EN | AHB_CLK_EN);
    #endif
    }
    #if defined LPC1788                                                  // {20}
    while (!(USBClkSt & PORTSEL_CLK_EN)) {
        USBClkCtrl |= PORTSEL_CLK_EN;                                    // enable selection of USB port
    #if defined _WINDOWS
        USBClkSt |= (PORTSEL_CLK_EN);
    #endif
    }
    if (pars->usConfig & USB_ALT_PORT_MAPPING) {                         // use USB 2 mapping instead of USB 1 mapping (select USB pins)
        USBPortSel = PORT_SELECT_MAP_TO_2;
        IOCON_P0_31 = _USB2_P0_31_DPLUS;                                 // alternative D- is a dedicated pin
        IOCON_P0_13 = _USB2_P0_13_UP_LED;
        IOCON_P0_14 = _USB2_P0_14_CONNECT;
    }
    else {                                                               // use USB 1
        USBPortSel = PORT_SELECT_MAP_TO_1;
        IOCON_P0_29 = _USB1_P0_29_DPLUS;
        IOCON_P0_30 = _USB1_P0_30_DMINUS;
        IOCON_P1_18 = _USB1_P1_18_UP_LED;
        IOCON_P2_09 = _USB1_P2_9_CONNECT;
    }
    IOCON_P1_30 = _USB1_P1_30_VBUS;                                      // disable pull-up/downs on the VBUS pin
    USBClkCtrl &= ~PORTSEL_CLK_EN;                                       // disable selection of USB port
    #else
    PINSEL3 |= (PINSEL3_P1_30_VBUS | PINSEL3_P1_18_USB_UP_LED1);         // select VBUS, UP_LED and soft connect functions
    PINMODE3 |= PINMODE_NO_PULLS_30;                                     // disable pull-up/downs on the VBUS pin
    PINSEL4 |= (PINSEL4_P2_9_USB_CONNECT1);             
    PINSEL1 |= (PINSEL1_P0_29_U1D_PLUS | PINSEL1_P0_30_U1D_MINUS);       // select USB pins
    #endif
    if (++pars->ucEndPoints > NUMBER_OF_USB_ENDPOINTS) {                 // add endpoint 0
        pars->ucEndPoints = NUMBER_OF_USB_ENDPOINTS;                     // limit to maximum available in device
    }

    fnUSB_reset(1);
    fnEnterInterrupt(irq_USB_ID, USB_PRIORITY, USB_Interrupt);
    USBDevIntClr = (DEVICE_STATUS_CCEMPT | DEVICE_STATUS_CDFULL);
    fnUSB_command(USB_CMD_SET_DEVICE_STATUS, _USB_CMD_DATA(USB_STATUS_COM, USB_CMD_SET_DEVICE_STATUS_DATA_PHASE)); // assert the SoftConnect pin
}

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW *ptr_usb_hardware)
{
    unsigned char ucEndpointState;
    unsigned long ulEndpointRef = (((iEndpoint * 2) + 1) << 16);         // set to physical IN endpoint   
    ucEndpointState = fnUSB_status((USB_CMD_SELECT_EP | ulEndpointRef), (USB_CMD_SELECT_EP_DATA_PHASE | ulEndpointRef));
    if (ucEndpointState & USB_EP_FULL_EMPTY) {
        return ENDPOINT_NOT_FREE;                                        // the endpoint buffer(s) are full so no more data can be added
    }
    ptr_usb_hardware->ptr_ulUSB_BDControl = 0;
    ptr_usb_hardware->ptrDatBuffer = 0;
    return ENDPOINT_FREE;
}

// Copy the data to be transmitted to the FIFO. This may only be called when there is a buffer to accept the data
//
extern void fnSendUSB_FIFO(unsigned char *ptrData, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW)
{
    int n;
    unsigned long ulTemp;

    USBCtrl = ((iEndpoint & 0x0F) << 2) | CTRL_WR_EN;

    USBTxPLen = usLength;

    for (n = 0; n < (usLength + 3) / 4; n++) {
        ulTemp = *ptrData++;
        ulTemp |= (*ptrData++ << 8);
        ulTemp |= (*ptrData++ << 16);
        ulTemp |= (*ptrData++ << 24);
        USBTxData = ulTemp;
    }

    USBCtrl = 0;

    iEndpoint *= 2;
    iEndpoint++;
    iEndpoint <<= 16;

    fnUSB_command((USB_CMD_SELECT_EP | iEndpoint), 0);                   // command with no data phase
    fnUSB_command(USB_CMD_VALID_EP_BUFFER, 0);                           // command with no data phase
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength)
{
    unsigned long ulEndpointInterrupt = (EP0RX << (2 * ucEndpointRef));
    if (ucEndpointType & IN_ENDPOINT) {
        ulEndpointInterrupt <<= 1;
    }
    if (ENDPOINT_DISABLE == ucEndpointType) {
#if (defined USB_DMA_TX || defined USB_DMA_RX) && defined USB_RAM_START  // {28}
        USBEpDMADis = ulEndpointInterrupt;                               // disable DMA operation on the endpoint
    #ifdef _WINDOWS
        USBEpDMASt &= ~USBEpDMADis; USBEpDMADis = 0;
    #endif
        if (USBEpDMASt == 0) {                                           // if no more DMA channels enabled
            USBDMAIntEn = 0;                                             // disable DMA interrupts
        }
#else
        USBEpIntEn &= ~ulEndpointInterrupt;                              // disable interrupts on the endpoint
        USBReEp &= ~ulEndpointInterrupt;
#endif
        fnUSB_command(USB_CMD_CONFIG_DEVICE, USB_CMD_CONFIG_DEVICE_DATA_PHASE_DIS);
        return;
    }
    else {
        unsigned long ulEndpoint = (2 * ucEndpointRef);
        USBReEp |= ulEndpointInterrupt;                                  // realise the endpoint
        if (ucEndpointType & IN_ENDPOINT) {
            ulEndpoint++;
        }
        _USBMaxPSize(ulEndpoint, usEndpointLength);                      // set the size of physical endpoints
        fnUSB_command((USB_CMD_SET_EP_STATUS | (ulEndpoint << 16)), USB_CMD_SET_EP_STATUS_DATA_PHASE); // clear status
#if defined USB_DMA_RX && defined USB_RAM_START                          // {28} DMA on reception
        if (!(ucEndpointType & IN_ENDPOINT)) {                           // OUT endpoint
            USBEpDMAEn = ulEndpointInterrupt;                            // enable DMA operation on the endpoint
    #ifdef _WINDOWS
            USBEpDMASt |= USBEpDMAEn; USBEpDMAEn = 0;
    #endif
            USBDMAIntEn = USB_DMA_EOT;                                   // enable interrupt when a DMA descriptor has been used
            _SIM_USB(USB_SIM_ENUMERATED, 0, 0);                          // inform the simulator that USB enumeration has completed
            return;
        }
#endif
#if defined USB_DMA_TX && defined USB_RAM_START                          // DMA on transmission
        if (ucEndpointType & IN_ENDPOINT) {                              // OUT endpoint
            USBDMAIntEn = USB_DMA_EOT;                                   // enable interrupt when a DMA descriptor has been used
        }
#endif
        USBEpIntClr = ulEndpointInterrupt;                               // clear pending interrupts when this endpoint i soperating in slave mode
        USBEpIntEn |= ulEndpointInterrupt;                               // enable interrupts on the endpoint
        _SIM_USB(USB_SIM_ENUMERATED, 0, 0);                              // inform the simulator that USB enumeration has completed
        return;
    }
}

// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    unsigned long ulCommand;
    if (ucEndpoint & IN_ENDPOINT) {
        ucEndpoint <<= 1;                                                // multiply the endpoint number by 2 and also shift out the IN_ENDPOINT flag
        ulCommand = (USB_CMD_SET_EP_STATUS | ((ucEndpoint + 1) << 16));  // IN endpoint
    }
    else {
        ucEndpoint <<= 1;                                                // multiply the endpoint number by 2 and also shift out the IN_ENDPOINT flag
        ulCommand = (USB_CMD_SET_EP_STATUS | (ucEndpoint << 16));        // OUT endpoint
    }
    fnUSB_command(ulCommand, USB_CMD_SET_EP_STATUS_ENABLE_EP);           // uninstall the IN endpoint (control endpoints are automatically unstalled shen a SETUP token is received)

}



#ifdef _WINDOWS
// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    return usEndpointSizes[iEndpoint * 2];
}
#endif

#endif


#ifdef SUPPORT_PORT_INTERRUPTS
static void ( *gpio_handlers_0[32] )( void ) = {0};                      // a handler for each possible port 0 pin
static void ( *gpio_handlers_2[14] )( void ) = {0};                      // a handler for each possible port 2 pin

// This routine enters the user handler for a port interrupt. The handler can be assigned to multiple inputs
//
static void fnEnterHandler(unsigned long ulPortRef, unsigned long ulPortBits, void ( *int_handler )( void ))
{
    int port_bit = 0;
    unsigned long ulBit = 0x00000001;
    while (ulPortBits >= ulBit) {
        if (ulPortBits & ulBit) {
            if (ulPortRef == PORT_2) {
                gpio_handlers_2[port_bit] = int_handler;                 // enter the application handler
            }
            else {
                gpio_handlers_0[port_bit] = int_handler;                 // enter the application handler
            }
            ulPortBits &= ~ulBit;
        }
        ulBit <<= 1;
        port_bit++;
    }
}
#endif

#if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT0)
static void ( *eint0_handler )( void ) = 0;                              // a handler for external interrupt 0
static __interrupt void EINT0_Interrupt(void)
{
    #ifdef _WINDOWS
    EXTINT &= ~EINT0;
    #else
    EXTINT = EINT0;                                                      // clear the interrupt flag so that further events are detected (only clears in edge mode)
    #endif
    uDisable_Interrupt();                                                // protect the call from interrupts
    eint0_handler();                                                     // call the user handler
    uEnable_Interrupt();                                                  // enable interrupts again
}
#endif

#if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT1)
static void ( *eint1_handler )( void ) = 0;                              // a handler for external interrupt 1
static __interrupt void EINT1_Interrupt(void)
{
    #ifdef _WINDOWS
    EXTINT &= ~EINT1;
    #else
    EXTINT = EINT1;                                                      // clear the interrupt flag so that further events are detected (only clears in edge mode)
    #endif
    uDisable_Interrupt();                                                // protect the call from interrupts
    eint1_handler();                                                     // call the user handler
    uEnable_Interrupt();                                                 // release
}
#endif

#if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT2)
static void ( *eint2_handler )( void ) = 0;                              // a handler for external interrupt 2
static __interrupt void EINT2_Interrupt(void)
{
    #ifdef _WINDOWS
    EXTINT &= ~EINT2;
    #else
    EXTINT = EINT2;                                                      // clear the interrupt flag so that further events are detected (only clears in edge mode)
    #endif
    uDisable_Interrupt();                                                // protect the call from interrupts
    eint2_handler();                                                     // call the user handler
    uEnable_Interrupt();                                                 // release
}
#endif

#if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT3) || defined SUPPORT_PORT_INTERRUPTS
    #if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT3)
static void (*eint3_handler)(void) = 0;                                  // a handler for external interrupt 3 and GPIO interrupts
    #endif
// GPIO and EINT3 interrupt routine
//
static __interrupt void EINT3_Interrupt(void)
{
    #if defined SUPPORT_PORT_INTERRUPTS
    unsigned long ulInterrupts;
    unsigned long ulBit;
    int iInterrupt;
    #endif
    #if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT3)
        #if defined SUPPORT_PORT_INTERRUPTS
    if (EXTINT & EINT3) {
        #endif
        #ifdef _WINDOWS
        EXTINT &= ~EINT3;
        #else
        EXTINT = EINT3;                                                  // clear the interrupt flag so that further events are detected (only clears in edge mode)
        #endif
        uDisable_Interrupt();                                            // protect the call from interrupts
        if (eint3_handler != 0) {
            eint3_handler();                                             // call the user handler
        }
        uEnable_Interrupt();                                             // release
        #if defined SUPPORT_PORT_INTERRUPTS
    }
        #endif
    #endif
    #if defined SUPPORT_PORT_INTERRUPTS
    if (IOIntStatus & P0int) {                                           // an interrupt from port 0 is pending
        ulInterrupts = IO0IntStatR;                                      // get rising edge interrupts on port 0
        ulInterrupts |= IO0IntStatF;                                     // also falling edge interrupts
        IO0IntClr = ulInterrupts;                                        // clear the ones which will be handled
        #ifdef _WINDOWS
        IO0IntStatR &= ~IO0IntClr;
        IO0IntStatF &= ~IO0IntClr;
        IO0IntClr = 0;
        IOIntStatus &= ~P0int;
        #endif
        ulBit = 0x00000001;
        iInterrupt = 0;
        while (ulInterrupts >= ulBit) {                                  // for each input change that has been detected on port 0
            if (ulInterrupts & ulBit) {                                  // an enabled input has changed
                uDisable_Interrupt();                                    // protect the call from interrupts
                (gpio_handlers_0[iInterrupt])();                         // call the application handler
                uEnable_Interrupt();                                     // release
                ulInterrupts &= ~ulBit;
            }
            iInterrupt++;
            ulBit <<= 1;
        }
    }
    if (IOIntStatus & P2int) {                                           // an interrupt from port 2 is pending
        ulInterrupts = IO2IntStatR;                                      // get rising edge interrupts on port 2
        ulInterrupts |= IO2IntStatF;                                     // also falling edge interrupts
        IO2IntClr = ulInterrupts;                                        // clear the ones which will be handled
        #ifdef _WINDOWS
        IO2IntStatR &= ~IO2IntClr;
        IO2IntStatF &= ~IO2IntClr;
        IO2IntClr = 0;
        IOIntStatus &= ~P2int;
        #endif
        ulBit = 0x00000001;
        iInterrupt = 0;
        while (ulInterrupts >= ulBit) {                                  // for each input change that has been detected on port 0
            if (ulInterrupts & ulBit) {                                  // an enabled input has changed
                uDisable_Interrupt();                                    // protect the call from interrupts
                (gpio_handlers_2[iInterrupt])();                         // call the application handler
                uEnable_Interrupt();                                     // release
                ulInterrupts &= ~ulBit;
            }
            iInterrupt++;
            ulBit <<= 1;
        }
    }
    #endif
}
#endif

#ifdef SUPPORT_TIMER
static void ( *timer_int[TIMERS_AVAILABLE] )( void ) = {0};

static __interrupt void timer_0_int(void)
{
    #if defined TICK_TIMER_1 || defined TICK_TIMER_2 || defined TICK_TIMER_3
    T0IR = MR0_INTERRUPT;                                                // clear match interrupt
  //T0TCR = COUNTER_RESET;                                               // {13} reset timer to achieve single shot function
    if (timer_int[0] != 0) {
        uDisable_Interrupt();                                            // protect the call from interrupts
        timer_int[0]();                                                  // call the user handler
        uEnable_Interrupt();                                             // release
    }
    #endif
}

static __interrupt void timer_1_int(void)
{
    #if !defined TICK_TIMER_1
    T1IR = MR0_INTERRUPT;                                                // clear match interrupt
  //T1TCR = COUNTER_RESET;                                               // {13} reset timer to achieve single shot function
    if (timer_int[1] != 0) {
        uDisable_Interrupt();                                            // protect the call from interrupts
        timer_int[1]();                                                  // call the user handler
        uEnable_Interrupt();                                             // release
    }
    #endif
}

static __interrupt void timer_2_int(void)
{
    T2IR = MR0_INTERRUPT;                                                // clear match interrupt
    if (timer_int[2] != 0) {
        uDisable_Interrupt();                                            // protect the call from interrupts
        timer_int[2]();                                                  // call the user handler
        uEnable_Interrupt();                                             // release
    }
}

static __interrupt void timer_3_int(void)
{
    T3IR = MR0_INTERRUPT;                                                // clear match interrupt
  //T3TCR = COUNTER_RESET;                                               // {13} reset timer to achieve single shot function
    if (timer_int[3] != 0) {
        uDisable_Interrupt();                                            // protect the call from interrupts
        timer_int[3]();                                                  // call the user handler
        uEnable_Interrupt();                                             // release
    }
}

static void ( *_timer_int[TIMERS_AVAILABLE] )( void ) = {
    (void (*)(void))timer_0_int,
    (void (*)(void))timer_1_int,
    (void (*)(void))timer_2_int,
    (void (*)(void))timer_3_int,
};
#endif

#if defined SUPPORT_RIT
static unsigned short usRIT_mode = 0;
static void ( *RIT_user_handler )( void ) = 0;
static __interrupt void RIT_Interrupt(void)                              // RIT interrupt handler
{
    if (usRIT_mode & RIT_SINGLE_SHOT) {                                  // in single-shot mode
        RICTRL = RITINT;                                                 // clear interrupt and disable RIT
        POWER_DOWN(PCRIT);                                               // disable power since the single shot interrupt has completed
    }
    else {                                                               // periodic mode of operation
        RICTRL = (RITENCLR | RITINT | RITENBR | RITEN);                  // clear interrupt but continue counting operation
    }
    #ifdef _WINDOWS
        RICTRL &= ~RITINT;
    #endif
    if (RIT_user_handler != 0) {
        uDisable_Interrupt();                                            // protect the call from interrupts
        RIT_user_handler();                                              // call the user handler
        uEnable_Interrupt();                                             // release
    }
}
#endif

#ifdef SUPPORT_ADC                                                       // {38}
static void (*_adc0_handler)(void) = 0;

static __interrupt void ADC0_Interrupt(void)
{
}
#endif

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
#if defined SUPPORT_RIT
    case RIT_INTERRUPT:                                                  // repetitive interrupt timer
        usRIT_mode = (((RIT_SETUP *)ptrSettings)->mode);                 // operating mode
        if (usRIT_mode & RIT_STOP) {
            POWER_DOWN(PCRIT);
            return;
        }
        POWER_UP(PCRIT);                                                 // apply power to the RIT module
        RICTRL = RITINT;                                                 // disable RIT and clear pending interrupts
        RICOUNTER = 0;                                                   // ensure that the counter is reset
        RIMASK = 0;                                                      // no mask of comparison bits
        RICOMPVAL = (((RIT_SETUP *)ptrSettings)->count_delay);           // match value
        RIT_user_handler = ((RIT_SETUP *)ptrSettings)->int_handler;
        fnEnterInterrupt(irq_RITINT_ID, ((RIT_SETUP *)ptrSettings)->int_priority, (void (*)( void ))RIT_Interrupt);
        RICTRL = (RITENCLR | RITINT | RITENBR | RITEN);                  // start counting, with reset on match
    #ifdef _WINDOWS
        RICTRL &= ~RITINT;
    #endif
        break;
#endif
    case PORT_INTERRUPT:
        {
#if defined SUPPORT_EXTERNAL_INTERRUPTS || defined SUPPORT_PORT_INTERRUPTS
            INTERRUPT_SETUP *port_interrupt = (INTERRUPT_SETUP *)ptrSettings;
            unsigned long ulPortInputs = port_interrupt->int_port_bits;
    #if defined SUPPORT_PORT_INTERRUPTS
        #if defined LPC1788                                              // {17}
            unsigned long *pin_mode = (IOCON_P0_ADD + (port_interrupt->int_port * 32));
        #else
            unsigned long *pin_mode = (PINMODE0_ADD + (port_interrupt->int_port * 2));
        #endif
            LPC17XX_PORT_INT *port_control = IOIntStatR_ADD;
    #endif
    #if defined SUPPORT_EXTERNAL_INTERRUPTS
            if (port_interrupt->int_port >= EXTERNALINT) {               // external interrupt rather than a GPIO interrupt
              //unsigned long ulExternalInterrupts = ((ulPortInputs & (EINT0 | EINT1 | EINT2 | EINT3)) << 14);
              //VICIntEnClr = ulExternalInterrupts;                      // disable interrupt in interrupt controller
        #ifdef _WINDOWS
              //VICIntEnable &= ~VICIntEnClr; VICIntEnClr = 0;
        #endif
                if (port_interrupt->int_port_sense & IRQ_DISABLE_INT) {
                    return;                                              // interrupt disabled, so work done
                }
                if (port_interrupt->int_port_sense & (IRQ_FALLING_EDGE | IRQ_RISING_EDGE)) {
                    EXTMODE |= ulPortInputs;                             // edge sensitive
                }
                else {
                    EXTMODE &= ~ulPortInputs;                            // level sensitive
                }
                if (port_interrupt->int_port_sense & (IRQ_RISING_EDGE | IRQ_HIGH_LEVEL)) {
                    EXTPOLAR |= ulPortInputs;                            // high level or rising edge
                }
                else {
                    EXTPOLAR &= ~ulPortInputs;                           // low level or falling edge
                }
        #ifdef _WINDOWS
                EXTINT &= ~ulPortInputs;                                 // clear possible pending interrupts before enabling interrupts
        #else
                EXTINT = ulPortInputs;                                   // clear possible pending interrupts before enabling interrupts
        #endif
        #if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT0)
                if ((ulPortInputs & EINT0) != 0) {
                    PINSEL4 &= ~PINSEL4_P2_10_RESET;
                    PINSEL4 |= (PINSEL4_P2_10_EINT0);                    // configure EINT0 pin
                    eint0_handler = port_interrupt->int_handler;
                    fnEnterInterrupt(irq_EINT0_ID, port_interrupt->int_priority, (void (*)( void ))EINT0_Interrupt); // configure and enter the handling interrupt routine in the vector table
                }
        #endif
        #if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT1)
                if ((ulPortInputs & EINT1) != 0) {
                    PINSEL4 &= ~PINSEL4_P2_11_RESET;
                    PINSEL4 |= (PINSEL4_P2_11_EINT1);                    // configure EINT1 pin
                    eint1_handler = port_interrupt->int_handler;
                    fnEnterInterrupt(irq_EINT1_ID, port_interrupt->int_priority, (void (*)( void ))EINT1_Interrupt); // configure and enter the handling interrupt routine in the vector table
                }
        #endif
        #if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT2)
                if ((ulPortInputs & EINT2) != 0) {
                    PINSEL4 &= ~PINSEL4_P2_12_RESET;
                    PINSEL4 |= (PINSEL4_P2_12_EINT2);                    // configure EINT2 pin
                    eint2_handler = port_interrupt->int_handler;
                    fnEnterInterrupt(irq_EINT2_ID, port_interrupt->int_priority, (void (*)( void ))EINT2_Interrupt); // configure and enter the handling interrupt routine in the vector table
                }
        #endif
        #if (defined SUPPORT_EXTERNAL_INTERRUPTS && defined SUPPORT_EINT3)
                if ((ulPortInputs & EINT3) != 0) {
                    PINSEL4 &= ~PINSEL4_P2_13_RESET;
                    PINSEL4 |= (PINSEL4_P2_13_EINT3);                    // configure EINT3 pin
                    eint3_handler = port_interrupt->int_handler;
                    fnEnterInterrupt(irq_EINT3_ID, port_interrupt->int_priority, (void (*)( void ))EINT3_Interrupt); // configure and enter the handling interrupt routine in the vector table
                }
        #endif
                return;
            }
    #endif
    #if defined SUPPORT_PORT_INTERRUPTS
            if (port_interrupt->int_port_sense & (PULLUP_ON | PULLDOWN_ON | PULLUP_DOWN_OFF)) { // change in pull-up/downs required
        #if !defined LPC1788                                              // {17}
                unsigned long ulPullDownMask = 0x00000003;
                unsigned long ulNoMask = 0x00000002;
        #endif
                unsigned long ulBit  = 0x00000001;
                while (ulPortInputs >= ulBit) {
                    if (ulBit & ulPortInputs) {                          // this bit is to be change
        #if defined LPC1788                                              // {17}
                        *pin_mode &= ~(IOCON_MODE_PULL_UP | IOCON_MODE_PULL_DOWN); // default to no pull-up/down
        #else
                        *pin_mode &= ~ulPullDownMask;                    // mask out and set internal pull up (avoid illegal state 0x01)
        #endif
                        if (port_interrupt->int_port_sense & (PULLDOWN_ON | PULLUP_DOWN_OFF)) {
                            if (port_interrupt->int_port_sense & PULLDOWN_ON) {
        #if defined LPC1788                                              // {17}
                                *pin_mode |= (IOCON_MODE_PULL_DOWN);
        #else
                                *pin_mode |= ulPullDownMask;
        #endif
                            }
                            else {
        #if defined LPC1788                                              // {17}
                                *pin_mode |= (IOCON_MODE_PULL_UP);
        #else
                                *pin_mode |= ulNoMask;
        #endif
                            }
                        }
                    }
                    ulBit <<= 1;
        #if defined LPC1788                                              // {17}
                    pin_mode++;
        #else
                    if (ulBit & 0x00010000) {
                        pin_mode++;                                      // move to second mode register
                    }
                    ulPullDownMask <<= 2;
                    ulNoMask <<= 2;
        #endif
                }
            }
            if (!(port_interrupt->int_port_sense & (IRQ_DISABLE_INT | IRQ_RISING_EDGE | IRQ_FALLING_EDGE))) {
                return;                                                  // no interrupt configuration required so quit
            }
            switch (port_interrupt->int_port) {
            case PORT_2:
                port_control++;                                          // move to the port 2 control block
            case PORT_0:
                port_control->IOIntClr = ulPortInputs;                   // clear possible interrupts on the defined port inputs
        #ifdef _WINDOWS
                port_control->IOIntClr = 0;
        #endif
                fnEnterInterrupt(irq_EINT3_ID, port_interrupt->int_priority, (void (*)(void))EINT3_Interrupt);// configure and enter the handling interrupt routine in the vector table
                fnEnterHandler(port_interrupt->int_port, ulPortInputs, port_interrupt->int_handler);
                if (port_interrupt->int_port_sense & IRQ_RISING_EDGE) {
                    port_control->IOIntEnR |= ulPortInputs;              // enable rising edge interrupt on the defined bits
                }
                else {
                    port_control->IOIntEnR &= ~(ulPortInputs);           // disable rising edge interrupt on the defined bits
                }
                if (port_interrupt->int_port_sense & IRQ_FALLING_EDGE) {
                    port_control->IOIntEnF |= ulPortInputs;              // enable falling edge interrupt on the defined bits
                }
                else {
                    port_control->IOIntEnF &= ~(ulPortInputs);           // disable falling edge interrupt on the defined bits
                }
                break;
            default:
                return;                                                  // no interrupts on other ports
            }
    #endif
#endif
        }
        break;
#ifdef SUPPORT_TIMER
    case TIMER_INTERRUPT:
        {
            TIMER_INTERRUPT_SETUP *ptrTimInt = (TIMER_INTERRUPT_SETUP *)ptrSettings;
            TIMER_CONTROL *ptrTimerCtl;
            int iIntVector;
            switch (ptrTimInt->timer_reference) {
            case 0:
                ptrTimerCtl = (TIMER_CONTROL *)TIMER_PERIPHERAL_BLOCK_0;
                POWER_UP(PCTIM0);                                        // power up the timer for use
                iIntVector = irq_Timer0_ID;
                break;
            case 1:
                ptrTimerCtl = (TIMER_CONTROL *)TIMER_PERIPHERAL_BLOCK_1;
                POWER_UP(PCTIM1);                                        // power up the timer for use
                iIntVector = irq_Timer1_ID;
                break;
            case 2:
                ptrTimerCtl = (TIMER_CONTROL *)TIMER_PERIPHERAL_BLOCK_2;
                POWER_UP(PCTIM2);                                        // power up the timer for use
                iIntVector = irq_Timer2_ID;
                break;
            case 3:
                ptrTimerCtl = (TIMER_CONTROL *)TIMER_PERIPHERAL_BLOCK_3;
                POWER_UP(PCTIM3);                                        // power up the timer for use
                iIntVector = irq_Timer3_ID;
                break;
            default:
                return;
            }
            if (!(ptrTimInt->timer_mode & TIMER_DONT_DISTURB)) {
                ptrTimerCtl->TTCR = COUNTER_RESET;                       // ensure timer stopped
                ptrTimerCtl->TPR  = 0;                                   // no prescaler
            }
            if (ptrTimInt->timer_mode & (TIMER_PWM_MAT0 | TIMER_PWM_MAT1 | TIMER_PWM_MAT2)) { // generate a PWM output on the specified MAT pin
                switch (ptrTimInt->timer_mode & (TIMER_PWM_MAT0 | TIMER_PWM_MAT1 | TIMER_PWM_MAT2)) {
                case TIMER_PWM_MAT0:
                    break;
                case TIMER_PWM_MAT1:
                    break;
                case TIMER_PWM_MAT2:
                    break;
                }
                break;
            }
            else if (ptrTimInt->timer_mode & TIMER_PERIODIC) {
                ptrTimerCtl->TMCR = (MR0I | MR0R);                       // generate periodic interrupt with the count reset on each match
            }
            else {
                ptrTimerCtl->TMCR = (MR0I | MR0S);                       // generate single interrupt with the timer stopped on match
            }
            ptrTimerCtl->TMR0 = ptrTimInt->timer_value;;
            ptrTimerCtl->TIR  = 0xff;                                    // reset any waiting interrupt
            timer_int[ptrTimInt->timer_reference] = ptrTimInt->int_handler; // enter the user handler
            fnEnterInterrupt(iIntVector, ptrTimInt->int_priority, (void (*)(void))_timer_int[ptrTimInt->timer_reference]);
            ptrTimerCtl->TTCR =  COUNTER_ENABLE;                         // enable timer operation
        }
        break;
#endif
#if defined SUPPORT_ADC                                                   // {14}
    case ADC_INTERRUPT:
        {
            ADC_SETUP *ptrADCSettings = (ADC_SETUP *)ptrSettings;
            unsigned long ulChannels = (ptrADCSettings->int_adc_single_ended_inputs & 0xff);
            POWER_UP(PCPADC);                                            // ensure that the ADC module is powered
#if !defined LPC1788
            PCLKSEL0 |= (PCLK_ADC_CCLK_8);                               // ADC clock is PCLK/8 since faster speeds don't make any sense
#endif
            AD0CR = (ulChannels | ADCR_PDN | ptrADCSettings->SamplingSpeed); // slowest possible conversion rate
            if (ptrADCSettings->int_adc_mode & ADC_CONTINUOUS) {
                AD0CR |= ADCR_BURST;                                     // burst mode for continuous operation
            }
            else {
                switch (ulChannels) {                                    // presently for LPC1788
                case ADCR_SEL_CHAN0:
                    IOCON_P0_23 = _ADC0_P0_23_AIN0;                      // select pin as ADC input
                    break;
                case ADCR_SEL_CHAN1:
                    IOCON_P0_24 = _ADC0_P0_24_AIN1;                      // select pin as ADC input
                    break;
                case ADCR_SEL_CHAN2:
                    IOCON_P0_25 = _ADC0_P0_25_AIN2;                      // select pin as ADC input
                    break;
                case ADCR_SEL_CHAN3:
                    IOCON_P0_26 = _ADC0_P0_26_AIN3;                      // select pin as ADC input
                    break;
                case ADCR_SEL_CHAN4:
                    IOCON_P1_30 = _ADC0_P1_30_AIN4;                      // select pin as ADC input                    
                    break;
                case ADCR_SEL_CHAN5:
                    IOCON_P1_31 = _ADC0_P1_31_AIN5;                      // select pin as ADC input
                    break;
                case ADCR_SEL_CHAN6:
                    IOCON_P0_12 = _ADC0_P0_12_AIN6;                      // select pin as ADC input
                    break;
                case ADCR_SEL_CHAN7:
                    IOCON_P0_13 = _ADC0_P0_13_AIN7;                      // select pin as ADC input
                    break;
                default:
                    _EXCEPTION("More that one channel is selected in software mode - not allowed! !!");
                    break;
                }
                if (ptrADCSettings->int_handler != 0) {
                    _adc0_handler = ptrADCSettings->int_handler;         // enter the user's interrup callback
                    fnEnterInterrupt(irq_ADC_ID, ptrADCSettings->int_priority, (void (*)(void))ADC0_Interrupt); // enter the interrupt handler
                    AD0INTEN = (ulChannels);                             // interrupt on the single channel completion which will always take place after 11 clocks
                }
                else {
                    AD0INTEN = 0;                                        // no interrupt
                }
                AD0CR |= (ADCR_START_START_CONVERSION);                  // start conversion
            }
        }
    break;
#endif
    default:
        _EXCEPTION("Attempting configuration of interrupt interface without appropriate support enabled!!");
        break;
    }
}

#ifdef SUPPORT_RTC                                                       // {7}

static void (*rtc_interrupt_handler[5])(void) = {0};                     // RTC interrupt handlers


// Virtual interupts with compatibility with Coldfire RTCs
//
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


// General RTC interrupt handler
//
static void _rtc_handler(void)
{
    if (RTC_ILR & RTCCIF) {                                              // counter increment
        if (RTC_CIIR & CIIR_IMSEC) {                                     // second increment
            if (rtc_interrupt_handler[0] != 0) {                         // seconds interrupt call back
                rtc_interrupt_handler[0]();                              // call handling function (seconds interrupt)
            }
        }
        if (RTC_CIIR & CIIR_IMMIN) {                                     // minute increment
            if (rtc_interrupt_handler[1] != 0) {                         // minute interrupt call back
                rtc_interrupt_handler[1]();                              // call handling function (minute interrupt)
            }
        }
        if (RTC_CIIR & CIIR_IMHOUR) {                                    // hour increment
            if (rtc_interrupt_handler[2] != 0) {                         // hour interrupt call back
                rtc_interrupt_handler[2]();                              // call handling function (hour interrupt)
            }
        }
        if (RTC_CIIR & CIIR_IMDOM) {                                     // day increment
            if (rtc_interrupt_handler[3] != 0) {                         // day interrupt call back
                rtc_interrupt_handler[3]();                              // call handling function (day interrupt)
            }
        }
    }
    if (RTC_ILR & RTCALF) {                                              // alarm match
        if (rtc_interrupt_handler[4] != 0) {                             // alarm interrupt call back
            rtc_interrupt_handler[4]();                                  // call handling function (alarm interrupt)
        }
    }
    RTC_ILR = (RTCCIF | RTCALF);                                         // clear the interrupt
    #ifdef _WINDOWS
    RTC_ILR = 0;
    #endif
}


// Convert time and date to seconds since 1970 (the RTC is disabled when this is called)
//
static void fnConvertTimeSeconds(RTC_SETUP *ptr_rtc_setup, int iSetTime)
{
    if (iSetTime != 0) {
        unsigned long ulUnit = 1970;
        unsigned short usNewYearDay = 1;
        unsigned char ucNewWeekDay = 4;
        int iLeapYear;
        while (ulUnit < ptr_rtc_setup->usYear) {                             // for each passed year
            iLeapYear = LEAP_YEAR(ulUnit++);
            if (iLeapYear != 0) {
                ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 2);
            }
            else {
                ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
            }
        }
        iLeapYear = LEAP_YEAR(ulUnit);                                   // check whether present year is a leap year
        ulUnit = (ptr_rtc_setup->ucMonthOfYear - 1);                     // the full months passed in this year
        while (ulUnit != 0) {
            usNewYearDay += monthDays[ulUnit - 1];
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, monthDays[ulUnit-- - 1]);
        }
        if ((iLeapYear != 0) && (ptr_rtc_setup->ucMonthOfYear > 2))  {   // leap year and date past February so count leap day
            usNewYearDay++;
            ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, 1);
        }
        ulUnit = (ptr_rtc_setup->ucDayOfMonth - 1);                      // passed days in month
        ucNewWeekDay = fnIncDayOfWeek(ucNewWeekDay, (unsigned char)ulUnit);

        RTC_YEAR = ptr_rtc_setup->usYear;                                // set the new time to the appropriate RTC registers
        RTC_DOY = usNewYearDay;
        RTC_MONTH = ptr_rtc_setup->ucMonthOfYear;
        RTC_DOM = ptr_rtc_setup->ucDayOfMonth;
        RTC_HOUR = ptr_rtc_setup->ucHours;
        RTC_MIN = ptr_rtc_setup->ucMinutes;
        RTC_SEC = ptr_rtc_setup->ucSeconds;
        RTC_DOW = ucNewWeekDay;
    }
    else {                                                               // set alarm time
        RTC_AMR = (AMRSEC | AMRMIN | AMRHOUR | AMRDOM | AMRDOW | AMRDOY | AMRMON | AMRYEAR); // disable all alarm matches
        RTC_ALSEC = ptr_rtc_setup->ucSeconds;
        RTC_ALMIN = ptr_rtc_setup->ucMinutes;
        RTC_ALHOUR = ptr_rtc_setup->ucHours;
        RTC_ALDOM = ptr_rtc_setup->ucDayOfMonth;
        RTC_ALMON = ptr_rtc_setup->ucMonthOfYear;
        RTC_AMR = (AMRDOW | AMRDOY | AMRYEAR);                           // match only time of day and date of year
    }
}

// This routine attempts a high level of compatibility with the Coldfire V2 RTC but doesn't implement minute stopwatch
//
extern int fnConfigureRTC(void *ptrSettings)
{
    int iIRQ = 0;
    RTC_SETUP *ptr_rtc_setup = (RTC_SETUP *)ptrSettings;
    switch (ptr_rtc_setup->command & ~RTC_DISABLE) {
    case RTC_TIME_SETTING:                                               // set time to RTT
        RTC_CCR = CCR_CTCRST;                                            // disable RTC  and reset internal dividers
        fnConvertTimeSeconds(ptr_rtc_setup, 1);                          // set the present time and date (as seconds since 1970 - the seconds value is not needed in the LPC17xx)
        RTC_CCR = (CCR_CLKEN);                                           // enable RTC
        break;

    case RTC_GET_TIME:                                                   // get the present time
        do {                                                             // repeat in case seconds increment while copying
            ptr_rtc_setup->ucSeconds = (unsigned char)RTC_SEC;
            ptr_rtc_setup->usYear = (unsigned short)RTC_YEAR;
            ptr_rtc_setup->ucMonthOfYear = (unsigned char)RTC_MONTH;
            ptr_rtc_setup->ucDayOfMonth = (unsigned char)RTC_DOM;
            ptr_rtc_setup->ucDayOfWeek = (unsigned char)RTC_DOW;
            ptr_rtc_setup->ucHours = (unsigned char)RTC_HOUR;
            ptr_rtc_setup->ucMinutes = (unsigned char)RTC_MIN;
        } while (ptr_rtc_setup->ucSeconds != (unsigned char)RTC_SEC);    // if the seconds increment while reading repeat to get stable values
        break;

    case RTC_ALARM_TIME:                                                 // interrupt at specific date and time
        iIRQ++;
    case RTC_TICK_DAY:                                                   // interrupt on each day
        iIRQ++;
    case RTC_TICK_HOUR:                                                  // interrupt on each hour
        iIRQ++;
    case RTC_TICK_MIN:                                                   // interrupt each minute
        iIRQ++;
    case RTC_TICK_SEC:                                                   // interrupt on each second
        iIRQ++;
    case RTC_STOPWATCH_GO:                                               // interrupt after a certain number of minutes
        if (iIRQ == 0) {                                                 // RTC_STOPWATCH_GO            
            break;                                                       // not supported
        }
        iIRQ--;
        if (RTC_DISABLE & (ptr_rtc_setup->command)) {
            RTC_CIIR &= ~(CIIR_IMSEC << iIRQ);                           // disable interrupt
            break;                                                       // disable function rather than enable
        }
        fnEnterInterrupt(irq_RTC_ID, PRIORITY_RTC, _rtc_handler);        // enter RTC interrupt handler
        rtc_interrupt_handler[iIRQ] = ((INTERRUPT_SETUP *)ptrSettings)->int_handler; // enter the handling interrupt
        if (iIRQ == 4) {                                                 // RTC_ALARM_TIME
            fnConvertTimeSeconds(ptr_rtc_setup, 0);                      // set alarm match
        }
        else {
            RTC_CIIR |= (CIIR_IMSEC << iIRQ);                            // enable corresponding counter increment interrupt
        }
        RTC_CCR = (CCR_CLKEN);                                           // ensure RTC is enabled
        break;
    }
    return 0;
}

    #if defined SDCARD_SUPPORT && defined SUPPORT_FILE_TIME_STAMP
// This routine supports date and time stamping of files and directories. If the information is not available it could return 1
// so that a fixed stamp is set. It is assumed that the RTC in the LPC17xx will be used if the support is activated
//
extern int fnGetLocalFileTime(unsigned short *ptr_usCreationTime, unsigned short *ptr_usCreationDate)
{
    unsigned short usSec;
    unsigned short usMinutes;
    unsigned short usHours;
    unsigned short usDayofMonth;
    unsigned short usMonthofYear;
    unsigned short usYear;

    do {
        usSec = (unsigned short)RTC_SEC;                                 // get a snap shot from RTC variables
        usMinutes = (unsigned short)RTC_MIN;
        usHours = (unsigned short)RTC_HOUR;
        usDayofMonth = (unsigned short)RTC_DOM;
        usMonthofYear = (unsigned short)RTC_MONTH;
        usYear = (unsigned short)RTC_YEAR;
    } while (usSec != (unsigned short)RTC_SEC);                          // if there was a second overflow while collecting the date repeat (saves disabling interrupts)
    if (usYear < 2012) {                                                 // if the year is invalid report that there is no valid time/date
        return 1;                                                        // utFAT will take fixed timestamp instead
    }
    *ptr_usCreationTime = (usSec | (usMinutes << 5) | (usHours << 11));
    *ptr_usCreationDate = (usDayofMonth | (usMonthofYear << 5) | ((usYear - 1980) << 9));
    return 0;                                                            // successful
}
    #endif
#endif


extern void fnDelayLoop(unsigned long ulDelay_us)                        // {16}
{
    #define LOOP_FACTOR 10000000                                         // tuned by trial and error - may be compiler dependent!
    volatile unsigned long ulDelay = ((CCLK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay--) {}                                                 // simple loop tuned to perform us timing
}


#if defined SDCARD_SUPPORT && defined SD_CONTROLLER_AVAILABLE            // {15}

    #ifdef SD_CONTROLLER_DMA
static unsigned long *ulTempBuffer = 0;                                  // intermediate buffer used by DMA when the it can't work with the user's buffer directly
    #endif

// Initialise the SD controllers interface with low speed clock during initailiation sequence
//
extern void fnInitSDCardInterface(void)
{
    #ifdef SD_CONTROLLER_DMA
    POWER_UP(PCSDC | PCGPDMA);                                           // power up the SD controller interface and general purpose DMA controller
    #else
    POWER_UP(PCSDC);                                                     // power up the SD controller interface
    #endif
    CONFIGURE_SDCARD_INTERFACE();                                        // configure the pins for the hardware
    MCICommand = 0;
    MCIDataCtrl = 0;
    MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr); // clear all status flags
    MCIPower = MCI_PWR_Ctrl_PowerUp;
    MCIClock = (MCI_CLK_PwrSave | MCI_CLK_Enable | ((PCLK/4/2/300000 - 1))); // 300kHz initial speed in 1-bit mode
    #ifdef SD_CONTROLLER_DMA
    if (ulTempBuffer == 0) {                                             // on first initialisation get working memory in the USB memory (memory that GPDMA controller can use)
        ulTempBuffer = (unsigned long *)_getUSB_RAM((unsigned char *)uMallocUSB(512));
    }
    #endif
}

// Send a command to the SD-card and return a result, plus optional returned arguments
//
extern int fnSendSD_command(const unsigned char ucCommand[6], unsigned char *ucResult, unsigned char *ptrReturnData)
{
    #define MAX_POLL_COUNT   10000
    static int iCommandYieldCount = 0;
    static int iCommandState = 0;
    int iRtn = 0;

    if (iCommandYieldCount > MAX_POLL_COUNT) {                           // the present access is taking too long - quit with SD card error
        fnDebugMsg("TIMEOUT!!!\r\n");
        iRtn = ERROR_CARD_TIMEOUT;
    }
    else {
        switch (iCommandState) {
        case 0:
            while (MCIStatus & CmdActive) {                              // clear any command activity
                MCICommand = 0;
                MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr);
    #ifdef _WINDOWS
                MCIStatus = 0;
    #endif
            }
            fnDelayLoop(10);                                             // to tune
            MCIArgument = ((ucCommand[1] << 24) | (ucCommand[2] << 16) | (ucCommand[3] << 8) | ucCommand[4]); // set the argument of the command
            switch (ucCommand[0]) {
            case GO_IDLE_STATE_CMD0:
                MCICommand  = (ucCommand[0] | MCICmd_Enable);            // launch the command (no response)
    #ifdef _WINDOWS
                MCIStatus = CmdSent;                                     // since there is no response, just set the sent flag
    #endif
                break;
            case SEND_CSD_CMD9:
            case SEND_CID_CMD2:
                MCICommand  = (ucCommand[0] | MCICmd_Response | MCICmd_LongRsp | MCICmd_Enable); // launch the command with long response
    #ifdef _WINDOWS
                MCIStatus = CmdRespEnd;                                  // set the successful response flag
                MCIRespCmd = ucCommand[0];
    #endif
                break;
            default:
                MCICommand  = (ucCommand[0] | MCICmd_Response | MCICmd_Enable); // launch the command (short response)
    #ifdef _WINDOWS
                if (SEND_OP_COND_ACMD_CMD41 == ucCommand[0]) {
                    MCIStatus = CmdCrcFail;
                }
                else {
                    MCIStatus = CmdRespEnd;                              // set the successful response flag
                }
                MCIRespCmd = ucCommand[0];
    #endif
                break;
            }
    #ifdef _WINDOWS
            _fnSimSD_write((unsigned char)(ucCommand[0] | 0x40));_fnSimSD_write(ucCommand[1]);_fnSimSD_write(ucCommand[2]);_fnSimSD_write(ucCommand[3]);_fnSimSD_write(ucCommand[4]);_fnSimSD_write(0);_fnSimSD_write(0xff);
    #endif
        case 6:
            do {
                if (MCIStatus & CmdTimeOut) {                            // timeout occurred while waiting for command to complete
                    MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr);
                    MCICommand = 0;
                    MCIArgument = 0xffffffff;
                    iCommandYieldCount++;                                // monitor the maximum number of timeouts
                    iCommandState = 0;                                   // allow resend of command
                    uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_GO);   // switch to polling mode of operation
    #ifdef _WINDOWS
                    MCIStatus = CmdRespEnd;
    #endif
                    return CARD_BUSY_WAIT;
                }
                else if (MCIStatus & CmdCrcFail) {                       // CRC error detected
                    MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr);
                    switch (ucCommand[0]) {
                    case SEND_OP_COND_ACMD_CMD41:                        // this responds with command value 0x3f and CRC-7 0xff
                        MCICommand = 0;
                        MCIArgument = 0xffffffff;
                        break;
                    default:                                             // other cases could be real CRC error
                        break;
                    }
                    *ucResult = 0;                                       // for compatibility
                    break;
                }
                else if (MCIStatus & CmdRespEnd) {                       // command has successfully completed and response received
                    MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr);
                    if (ucCommand[0] == (unsigned char)MCIRespCmd) {     // if matching response
                        switch (ucCommand[0]) {
                        case SEND_IF_COND_CMD8:
                            *ucResult = SDC_CARD_VERSION_2PLUS;          // for compatibility
                            break;
                        case APP_CMD_CMD55:
                            *ucResult = R1_IN_IDLE_STATE;                // for compatibility
                            break;
                        default:
                            *ucResult = 0;                               // for compatibility
                            break;
                        }
                    }
                    break;
                }
                else if ((ucCommand[0] == GO_IDLE_STATE_CMD0) && (MCIStatus & CmdSent)) {
                    MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr);
                    *ucResult = R1_IN_IDLE_STATE;                        // dummy response for compatibility
                }
                else {                                                   // the command/response is still being executed
                    iCommandState = 6;
                    if (iCommandYieldCount == 0) {                       // just count once as timeout
                        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_GO); // switch to polling mode of operation
                        iCommandYieldCount = 1;
                    }
                    return CARD_BUSY_WAIT;                               // poll up to 20 times before yielding
                }
            } while (*ucResult & SD_CARD_BUSY);                          // poll the card until it is no longer indicating busy and returns the value
            if (ptrReturnData != 0) {                                    // if the caller requests data, read it here
    #ifdef _WINDOWS
                _fnSimSD_write(0xff);_fnSimSD_write(0xff);               // for compatibility
                MCIResponse0  = (_fnSimSD_write(0xff) << 24);
                MCIResponse0 |= (_fnSimSD_write(0xff) << 16);
                MCIResponse0 |= (_fnSimSD_write(0xff) << 8);
                MCIResponse0 |=  _fnSimSD_write(0xff);
    #endif
                *ptrReturnData++ = (unsigned char)(MCIResponse0 >> 24);
                *ptrReturnData++ = (unsigned char)(MCIResponse0 >> 16);
                *ptrReturnData++ = (unsigned char)(MCIResponse0 >> 8);
                *ptrReturnData   = (unsigned char)MCIResponse0;
                if ((ucCommand[0] == SEND_CSD_CMD9) || (ucCommand[0] == SEND_CID_CMD2)) { // exception requiring 16 bytes
    #ifdef _WINDOWS
                    MCIResponse1  = (_fnSimSD_write(0xff) << 24);
                    MCIResponse1 |= (_fnSimSD_write(0xff) << 16);
                    MCIResponse1 |= (_fnSimSD_write(0xff) << 8);
                    MCIResponse1 |=  _fnSimSD_write(0xff);
    #endif
                    ptrReturnData++;
                    *ptrReturnData++ = (unsigned char)(MCIResponse1 >> 24);
                    *ptrReturnData++ = (unsigned char)(MCIResponse1 >> 16);
                    *ptrReturnData++ = (unsigned char)(MCIResponse1 >> 8);
                    *ptrReturnData++ = (unsigned char)MCIResponse1;
    #ifdef _WINDOWS
                    MCIResponse2  = (_fnSimSD_write(0xff) << 24);
                    MCIResponse2 |= (_fnSimSD_write(0xff) << 16);
                    MCIResponse2 |= (_fnSimSD_write(0xff) << 8);
                    MCIResponse2 |=  _fnSimSD_write(0xff);
    #endif
                    *ptrReturnData++ = (unsigned char)(MCIResponse2 >> 24);
                    *ptrReturnData++ = (unsigned char)(MCIResponse2 >> 16);
                    *ptrReturnData++ = (unsigned char)(MCIResponse2 >> 8);
                    *ptrReturnData++ = (unsigned char)MCIResponse2;
    #ifdef _WINDOWS
                    MCIResponse3  = (_fnSimSD_write(0xff) << 24);
                    MCIResponse3 |= (_fnSimSD_write(0xff) << 16);
                    MCIResponse3 |= (_fnSimSD_write(0xff) << 8);
                    MCIResponse3 |=  _fnSimSD_write(0xff);
    #endif
                    *ptrReturnData++ = (unsigned char)(MCIResponse3 >> 24);
                    *ptrReturnData++ = (unsigned char)(MCIResponse3 >> 16);
                    *ptrReturnData++ = (unsigned char)(MCIResponse3 >> 8);
                    *ptrReturnData   = (unsigned char)MCIResponse3;
                }
            }
            break;
        }
    }
    if (iCommandYieldCount != 0) {
        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_STOP);             // switch to event mode of operation since write has completed
        iCommandYieldCount = 0;
    }
    iCommandState = 0;
    return iRtn;
}


// This local function is always fed with an aligned buffer to put data into, which will also be in DMA accessible RAM in case of DMA operation
//
static int _fnGetSector(unsigned char *ptrBuf)
{
    #define DATA_TIMEOUT_VALUE     0x20000
    MCIClear = (CmdCrcFailClr | DataCrcFailClr | CmdTimeOutClr | DataTimeOutClr | TxUnderrunClr | RxOverrunClr | CmdRespEndClr | CmdSentClr | DataEndClr | StartBitErrClr | DataBlockEndClr);
    MCIDataCtrl = 0;
    fnDelayLoop(10);                                                     // to tune
    MCIDataTimer = DATA_TIMEOUT_VALUE;
    MCIDataLength = 512;
    #ifdef SD_CONTROLLER_DMA                                             // if DMA is not used the data is received in the FIFO and should be copied to the input buffer
    DMACC1SrcAddr  = (unsigned long)MCIFIFO0_ADD;                        // DMA channel 1 transfer from FIFO to memory
    DMACC1DestAddr = (unsigned long)ptrBuf;                              // 
    DMACC1Control  = (512 | DMACCControl_SBSize_8 | DMACCControl_DBSize_32 | DMACCControl_SWidth_word | DMACCControl_DWidth_word | DMACCControl_DI);
    DMACConfiguration = DMACCConfiguration_E;                            // enable DMA controller in little-endian mode
    while (!(DMACConfiguration & DMACCConfiguration_E)) {}               // wait until it is enabled
	DMACC1Configuration = (DMACCConfiguration_L | DMACCConfiguration_E | DMACCConfiguration_SrcP_SD_MMC | DMACCConfiguration_P2MP);
	MCIDataCtrl = (MCIDataCnt_Enable | MCIDataCnt_Direction_from_card | MCIDataCnt_DMAEnable | MCIDataCnt_block_size_512);
    fnDelayLoop(1);                                                      // to tune
        #ifdef _WINDOWS
    _fnSimSD_write(0xff);                                                // dummy for simulation compatibility
    MCIStatus = 0;
        #endif
    while (!(MCIStatus & DataEnd)) {                                     // wait until the DMA has completed the transfer to the destination buffer
        #ifdef _WINDOWS
        if (MCIDataLength != 0) {
            MCIFIFO0 =   _fnSimSD_write(0xff);
            MCIFIFO0 |= (_fnSimSD_write(0xff) << 8);
            MCIFIFO0 |= (_fnSimSD_write(0xff) << 16);
            MCIFIFO0 |= (_fnSimSD_write(0xff) << 24);
            *(unsigned long *)ptrBuf = MCIFIFO0;                         // it is assumed that the buffer is long word aligned and the format is little-endian
            ptrBuf += sizeof(unsigned long);
            MCIDataLength -= 4;
        }
        if (MCIDataLength == 0) {
            MCIStatus = DataEnd;                                         // read complete
            _fnSimSD_write(0xff);_fnSimSD_write(0xff);                   // dummy for simulation compatibility
        }
        #endif
    }
    DMACConfiguration = 0;                                               // disable DMA on completion
    #else
	MCIDataCtrl = (MCIDataCnt_Enable | MCIDataCnt_Direction_from_card | MCIDataCnt_block_size_512);
    fnDelayLoop(1);                                                      // to tune  
        #ifdef _WINDOWS
    _fnSimSD_write(0xff);                                                // dummy for simulation compatibility
        #endif
    while ((MCIStatus & (DataEnd | RxDataAvailbl)) != DataEnd) {         // while data is still being received
        #ifdef _WINDOWS
        if (MCIDataLength != 0) {
            MCIStatus = RxDataAvailbl;
            MCIFIFO0 =   _fnSimSD_write(0xff);
            MCIFIFO0 |= (_fnSimSD_write(0xff) << 8);
            MCIFIFO0 |= (_fnSimSD_write(0xff) << 16);
            MCIFIFO0 |= (_fnSimSD_write(0xff) << 24);
            MCIDataLength -= 4;
        }
        #endif
        if (MCIStatus & RxDataAvailbl) {                                 // at least one word ready
            *(unsigned long *)ptrBuf = MCIFIFO0;                         // it is assumed that ptrBuf is long-word aligned and storage little-endian
            ptrBuf += sizeof(unsigned long);
        }
        #ifdef _WINDOWS
        if (MCIDataLength == 0) {
            MCIStatus = DataEnd;                                         // read complete
            _fnSimSD_write(0xff);_fnSimSD_write(0xff);                   // dummy for simulation compatibility
        }
        #endif
    }                                                                    // warning - FIFO mode of operation is not suitable for high clock rates since receive overruns can easily take place
    #endif
    return UTFAT_SUCCESS;                                                // read successfully terminated
}


// Read a sector from SD card into the specified data buffer
//
extern int fnGetSector(unsigned char *ptrBuf)
{
    #ifdef SD_CONTROLLER_DMA
    if (((CAST_POINTER_ARITHMETIC)ptrBuf & 0x03) || NOT_USB_RAM(ptrBuf)) { // DMA buffer must be aligned in USB RAM
        int iRtn = _fnGetSector(_putUSB_RAM((unsigned char *)ulTempBuffer)); // fill the temporary buffer with the sector data
        uMemcpy(ptrBuf, _putUSB_RAM((unsigned char *)ulTempBuffer), 512); // copy from aligned to non-aligned location
        return iRtn;
    }
    #else
    if ((CAST_POINTER_ARITHMETIC)ptrBuf & 0x03) {                        // when working with FIFO the buffer must be long word aligned. If it is not, a temporary intermediate buffer is used and then copied from
        unsigned long ulTempBuffer[512/sizeof(unsigned long)];
        int iRtn = _fnGetSector((unsigned char *)ulTempBuffer);          // fill the temporary buffer with the sector data
        uMemcpy(ptrBuf, ulTempBuffer, 512);                              // copy from aligned to non-aligned location
        return iRtn;
    }
    #endif
    else {
        return (_fnGetSector(ptrBuf));                                   // since the destination memory is long-word aligned and can hold a complete sector no intermediate buffer is required
    }
}

// Read a specified amount of data from present SD card sector into the specified data buffer (usStart and usStop are offset from start of sector and avoid other data outseide of thsi range being overwritted)
//
extern int fnReadPartialSector(unsigned char *ptrBuf, unsigned short usStart, unsigned short usStop)
{
    #ifdef SD_CONTROLLER_DMA
    _fnGetSector(_putUSB_RAM((unsigned char *)ulTempBuffer));            // fill the temporary buffer with the sector data
    uMemcpy(ptrBuf, (_putUSB_RAM((unsigned char *)ulTempBuffer) + usStart), (usStop - usStart)); // just copy the part required
    #else
    unsigned long ulTempBuffer[512/sizeof(unsigned long)];               // since the SD controller will always read a complete sector we use an intermediate buffer
    _fnGetSector((unsigned char *)ulTempBuffer);                         // fill the temporary buffer with the sector data
    uMemcpy(ptrBuf, (((unsigned char *)ulTempBuffer) + usStart), (usStop - usStart)); // just copy the part required
    #endif
    return UTFAT_SUCCESS;                                                // read terminated
}

// This local function is always fed with an aligned input buffer which will also be in DMA accessible RAM in case of DMA operation
//
static int _fnPutSector(unsigned char *ptrBuf)
{
    MCIDataTimer  = DATA_TIMEOUT_VALUE;
    MCIDataLength = 512;
    #ifdef SD_CONTROLLER_DMA
    DMACC1SrcAddr  = (unsigned long)ptrBuf;                              // DMA 1 channel copy from buffer to FIFO
    DMACC1DestAddr = (unsigned long)MCIFIFO0_ADD;
    DMACC1Control  = (DMACCControl_SBSize_32 | DMACCControl_DBSize_8 | DMACCControl_SWidth_word | DMACCControl_DWidth_word | DMACCControl_SI | 512);
    DMACConfiguration = DMACCConfiguration_E;                            // enable DMA controller in little-endian mode
    while (!(DMACConfiguration & DMACCConfiguration_E)) {}               // wait until it is enabled
    DMACC1Configuration = (DMACCConfiguration_L | DMACCConfiguration_E | DMACCConfiguration_DstP_SD_MMC | DMACCConfiguration_M2PP);
    MCIDataCtrl = (MCIDataCnt_Enable | MCIDataCnt_DMAEnable | MCIDataCnt_block_size_512 | MCIDataCnt_Direction_to_card);
    fnDelayLoop(1);                                                      // to tune  
        #ifdef _WINDOWS
    _fnSimSD_write(0xfe);                                                // for compatibility
    MCIStatus = 0;
        #endif
    while (!(MCIStatus & DataEnd)) {                                     // wait until the DMA has completed the transfer to the destination buffer
        #ifdef _WINDOWS
        MCIFIFO0 = *(unsigned long *)ptrBuf;                             // it is assumed that the buffer is long word aligned and the format is little-endian
        ptrBuf += sizeof(unsigned long);
        MCIDataLength -= 4;
        _fnSimSD_write((unsigned char)MCIFIFO0);
        _fnSimSD_write((unsigned char)(MCIFIFO0 >> 8));
        _fnSimSD_write((unsigned char)(MCIFIFO0 >> 16));
        _fnSimSD_write((unsigned char)(MCIFIFO0 >> 24));
        if (MCIDataLength == 0) {
            MCIStatus = DataEnd;
            _fnSimSD_write(0xff);_fnSimSD_write(0xff);_fnSimSD_write(0xff); // for compatibility
        }
        #endif
    }
    DMACConfiguration = 0;                                               // disable DMA on completion
    #else                                                                // copy the buffer to the FIFO area
	MCIDataCtrl = (MCIDataCnt_Enable | MCIDataCnt_block_size_512 | MCIDataCnt_Direction_to_card); // activate transmission
    fnDelayLoop(1);                                                      // to tune  
        #ifdef _WINDOWS
    _fnSimSD_write(0xfe);                                                // for compatibility
        #endif
    while ((MCIStatus & DataEnd) == 0) {                                 // while data has not been completely transmitted
        if ((MCIStatus & TxFifoFull) == 0) {                             // if there is space in the FIFO
            MCIFIFO0 = *(unsigned long *)ptrBuf;                         // it is assumed that the buffer is long word aligned and the format is little-endian
            ptrBuf += sizeof(unsigned long);
        #ifdef _WINDOWS
            MCIDataLength -= 4;
            _fnSimSD_write((unsigned char)MCIFIFO0);
            _fnSimSD_write((unsigned char)(MCIFIFO0 >> 8));
            _fnSimSD_write((unsigned char)(MCIFIFO0 >> 16));
            _fnSimSD_write((unsigned char)(MCIFIFO0 >> 24));
            if (MCIDataLength == 0) {
                MCIStatus = DataEnd;
                _fnSimSD_write(0xff);_fnSimSD_write(0xff);_fnSimSD_write(0xff); // for compatibility
            }
        #endif
        }
    }
    #endif
    return UTFAT_SUCCESS;                                                // write successfully completed
}

// Write present sector with buffer data
//
extern int fnPutSector(unsigned char *ptrBuf)
{
    #ifdef SD_CONTROLLER_DMA
    if (((CAST_POINTER_ARITHMETIC)ptrBuf & 0x03) || NOT_USB_RAM(ptrBuf)) { // DMA buffer must be aligned in USB RAM
        uMemcpy(_putUSB_RAM((unsigned char *)ulTempBuffer), ptrBuf, 512); // copy from non-aligned to aligned location
        ptrBuf = _putUSB_RAM((unsigned char *)ulTempBuffer);             // feed the intermediate buffer to _fnPutSector() in this case
    }
    #else
    if ((CAST_POINTER_ARITHMETIC)ptrBuf & 0x03) {                        // when working with FIFO the buffer must be long word aligned. If it is not, a temporary intermediate buffer is used
        unsigned long ulTempBuffer[512/sizeof(unsigned long)];
        uMemcpy(ulTempBuffer, ptrBuf, 512);                              // copy from non-aligned to aligned location
        ptrBuf = (unsigned char *)ulTempBuffer;                          // feed the intermediate buffer to _fnPutSector() in this case
    }
    #endif
    return (_fnPutSector(ptrBuf));
}
#endif

// This routine switches to low power mode. It is always called with disabled interrupts
//
extern void fnDoLowPower(void)
{
    PCON = PCON_IDL;                                                     // switch to low power mode - the CPU will stop but peripherals still execute
                                                                         // any enabled peripheral or external interrupt causes the processor to continue exectution
    uEnable_Interrupt();                                                 // enable interrupt so the waiting one will be handled
}

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    uDisable_Interrupt();
    WDMOD  = (WDEN | WDRESET);                                           // ensure watchdog is enabled
    WDFEED = 0xaa;                                                       // command reset to core and peripherals
    WDFEED = 0x55;
    WDFEED = 0xaa;
    WDFEED = 0;                                                          // bad feed sequence causes immediate reset
#ifdef _WINDOWS
    WDTV   = 0;
#endif
}


#if !defined _MINIMUM_IRQ_INITIALISATION
// Serious error interrupts
//
static void irq_hard_fault(void)
{
}

static void irq_memory_man(void)
{
}

static void irq_bus_fault(void)
{
}

static void irq_usage_fault(void)
{
}

static void irq_debug_monitor(void)
{
}

static void irq_NMI(void)
{
}

static void irq_pend_sv(void)
{
}

static void irq_SVCall(void)
{
}
#endif


// Default handler to indicate processor input from unconfigured source
//
static void irq_default(void)
{
}


#if defined (_GNU)
extern const VECTOR_TABLE_MINIMUM reset_vect;                            // {4}
volatile void *ptrTest;

extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

extern void __init_gnu_data(void)
{
    unsigned char *ptrData;
    unsigned long ulInitDataLength;
    #ifndef _RAM_DEBUG
    const unsigned char *ptrFlash = &__data_load_start__;
    ulInitDataLength = (&__data_end__ - &__data_start__);
    ptrData = &__data_start__;
    while (ulInitDataLength--) {                                         // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrFlash != ptrData) {                                           // if a move is required
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength--) {                                     // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }
    #endif
    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif


// Perform very low level initialisation - called by the start up code
//
static void _LowLevelInit(void)
{
    VECTOR_TABLE *ptrVect;
#if !defined _MINIMUM_IRQ_INITIALISATION
    void ( **processor_ints )( void );
#endif
#if defined _GNU
    ptrTest = (volatile void *)&reset_vect;                              // {4} force GCC to link the reset table and flash configuration
#endif   
#ifdef SUPPORT_RTC
    PCONP = (PCGPIO | PCRTC);                                            // disable all peripheral power apart from RTC and GPIO - the peripherals will be powered on later as used
#else
    PCONP = (PCGPIO);                                                    // disable all peripheral power apart from GPIO - the peripherals will be powered on later as used
#endif
    // Configure the PLL for the desired operating frequency - note the exact sequence is very important
    //
#ifdef LPC1788
    CCLKCFG = 0x00000001;                                                // ensure that system clock is used rather than PLL
#else
    if (PLL0STAT & PLLCONNECTED) {                                       // if PLL main is already connected (can be connected by ISP)
        PLL0CON  = PLLE;                                                 // disconnect PLL, but leave enabled
        PLL0FEED = 0xaa;PLL0FEED = 0x55;                                 // perform feed sequence so that register changes are accepted
    }
#endif
    PLL0CON  = 0;                                                        // disable PLL and disconnect
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted

#if CRYSTAL_FREQ > 15000000
    SCS |= (OSCRANGE | OSCEN);                                           // enable the oscillator
#else
    SCS |= (OSCEN);                                                      // enable the oscillator
#endif
    while (!(SCS & OSCSTAT)) {                                           // wait for it to stabilise
#ifdef _WINDOWS
        SCS |= OSCSTAT;
#endif
    }

#if defined USE_RC_OSCILLATOR
    CLKSRCSEL = PLL_SOURCE_4M_RC;                                        // ensure RC source
#elif defined USE_32K_OSCILLATOR
    CLKSRCSEL = PLL_SOURCE_RTC;                                          // RTC crystal as sourcey
#else
    CLKSRCSEL = PLL_SOURCE_OSCILLATOR;                                   // oscillator as PLL clock source
#endif
#if defined LPC1788
    #if MASTER_CLOCK >= 100000000
    PBOOST = BOOST;                                                      // enable power boost when operating from 100MHz {10}
    #else
    PBOOST = 0;                                                          // disable power boost when operating at less than 100MHz
    #endif
#endif
#ifndef DISABLE_PLL
    PLL0CFG = PLL_FREQUENCY_CONFIGURATION;                               // PLL setting to generate required frequency
    #if !defined LPC1788
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted
    #endif
    PLL0CON = PLLE;                                                      // enable the PLL to search for lock
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted
    CCLKCFG = CPU_CLOCK_CONFIGURATION;                                   // set CPU clock frequency from PLL output
    while (!(PLL0STAT & PLLLOCKED)) {                                    // wait for lock to be achieved
    #ifdef _WINDOWS
        PLL0STAT = (PLLLOCKED | PLL_FREQUENCY_CONFIGURATION);
    #endif
    }
    #ifdef LPC1788
    CCLKCFG  = (CCLKSEL | CPU_CLOCK_CONFIGURATION);                      // connect PLL to clock CPU
    #else
    PLL0CON  = (PLLE | PLLC);                                            // connect PLL to clock CPU
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted
    #endif
#endif
    FLASHCFG = (FLASHCFG_FIXED | FLASH_CPU_CLOCKS);                      // set optimum FLASH wait states

#ifdef LPC1788
    PCLKSEL = (PCLK_DIVIDE);                                             // set the PCLK divide value
#else
    PCLKSEL0 = CONFIG_PCLKSEL0;                                          // peripheral clocks set according to configuration
    PCLKSEL1 = CONFIG_PCLKSEL1;

    PINSEL0  = 0x00000000;                                               // set all pin states and ports to default
    PINSEL1  = 0x00000000;
    PINSEL2  = 0x00000000;
    PINSEL3  = 0x00000000;
    PINSEL4  = 0x00000000;
    PINSEL5  = 0x00000000;
    PINSEL6  = 0x00000000;
    PINSEL7  = 0x00000000;
    PINSEL8  = 0x00000000;
    PINSEL9  = 0x00000000;
    PINSEL10 = 0x00000000;
#endif
    FIO0DIR  = 0x00000000;
    FIO1DIR  = 0x00000000;
    FIO2DIR  = 0x00000000;
    FIO3DIR  = 0x00000000;
    FIO4DIR  = 0x00000000;
    FIO0MASK = 0x00000000;
    FIO1MASK = 0x00000000;
    FIO2MASK = 0x00000000;
    FIO3MASK = 0x00000000;
    FIO4MASK = 0x00000000;
#ifdef LPC1788
    FIO5DIR  = 0x00000000;
    FIO5MASK = 0x00000000;
#endif

    #ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
    #else
    ptrVect = (VECTOR_TABLE *)(NVIC_RAM_START_ADDRESS);
    #endif
    VECTOR_TABLE_OFFSET_REG   = (TBLBASE_IN_RAM | NVIC_RAM_START_ADDRESS); // position the vector table at the bottom of RAM
#if !defined _MINIMUM_IRQ_INITIALISATION
    ptrVect->ptrHardFault     = irq_hard_fault;
    ptrVect->ptrMemManagement = irq_memory_man;
    ptrVect->ptrBusFault      = irq_bus_fault;
    ptrVect->ptrUsageFault    = irq_usage_fault;
    ptrVect->ptrDebugMonitor  = irq_debug_monitor;
    ptrVect->ptrNMI           = irq_NMI;
    ptrVect->ptrPendSV        = irq_pend_sv;
    ptrVect->ptrSVCall        = irq_SVCall;
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;    // fill all processor specific interrupts with a default handler
    do {
        *processor_ints = irq_default;
        if (processor_ints == (void (**)(void))&ptrVect->processor_interrupts.LAST_PROCESSOR_IRQ) {
            break;
        }
        processor_ints++;
    } while (1);
#else
    ptrVect->ptrHardFault     = irq_default;
    ptrVect->ptrMemManagement = irq_default;
    ptrVect->ptrBusFault      = irq_default;
    ptrVect->ptrUsageFault    = irq_default;
    ptrVect->ptrDebugMonitor  = irq_default;
    ptrVect->ptrNMI           = irq_default;
    ptrVect->ptrPendSV        = irq_default;
    ptrVect->ptrSVCall        = irq_default;
#endif

#if !defined (_WINDOWS) && defined (COMPILE_IAR) && !defined COMPILE_IAR5
    __segment_init();                                                    // initialise variables
#elif defined (_GNU)
    __init_gnu_data();
#endif
#ifdef _WINDOWS                                                          // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Define of CHECK_VECTOR_SIZE is incorrect !!");
    }
#endif
#if defined APBCLOCK_DIVIDE
    APBDIV = APBCLOCK_DIVIDE;
#endif                                                                   // default is MASTER_CLOCK/4 but needs to be changed to respect USB requirements
}

#ifndef _COMPILE_KEIL                                                    // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #ifndef _WINDOWS
    asm(" ldr sp, [r0,#0]");
    asm(" ldr pc, [r0,#4]");
    #endif
}
#endif

#if defined ROWLEY && !defined ROWLEY_2                                  // Rowley project requires extra initialisation for debugger to work correctly before V1.7 build 17
static void _main2(void)
{
    asm("mov lr, #0");
    asm("b _main");
}
#endif

// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR5
__root const VECTOR_TABLE_MINIMUM __vector_table @ ".intvec"               // __root forces the function to be linked in IAR project
#elif defined COMPILE_IAR
__root const VECTOR_TABLE_MINIMUM reset_vect @ "RESETVECT"                 // __root forces the function to be linked in IAR project
#elif defined _GNU
const VECTOR_TABLE_MINIMUM __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
__attribute__((section("RESET"))) const VECTOR_TABLE_MINIMUM reset_vect
#else
const VECTOR_TABLE_MINIMUM reset_vect
#endif
= {
    {
        (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - IAP_STACK_SIZE - 4),  // stack pointer to top of RAM - leaving space for IAP and random number
        (void (*)(void))START_CODE,                                      // start address
    },
    0x53415475,                                                          // dummy vector values (for recognition)
    0x2052454b,
    0x3143504c,
    0x20585837,
    0x2e522e43,
#ifndef COMPILE_IAR5
    0xffffffff,                                                          // blank for check sum
#endif
};

    #if defined COMPILE_IAR5
    __root const unsigned long __vector_table_0x1c @ ".intvec" = 0xffffffff;
    #endif
#endif

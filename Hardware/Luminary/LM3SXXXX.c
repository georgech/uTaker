/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3SXXXX.c [Luminary]
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   *********************************************************************
   04.12.2008 Add IAR5 support
   07.12.2008 Increase Ethernet buffer sizes to account for length in content and to ensure acceptance of long word writes {1}
   07.12.2008 Remove usEthRxValid[] and add usEthernetInputSize. Don't allow overlarge input frames from overwriting buffer, but extract all from FIFO
   07.12.2008 Retrigger the watchdog during erase operation since a page erase takes at least 20ms. 156k delete thus takes over 6s to complete {2}
   16.01.2009 SST SPI FLASH support                                      {3}
   04.02.2009 Change GET_FIFO_LONG to use fnGetFifo_long() when simulating {4}
   05.02.2009 Add single-shot timer interrupt                            {5}
   27.02.2009 Add RTS/CTS control                                        {6}
   04.03.2009 Change use of random number seed                           {7}
   14.03.2009 Fast GPIOs configured if available                         {8}
   24.03.2009 Add uDMA support to UARTs                                  {9}
   31.03.2009 Add ADC support                                            {10}
   11.04.2009 Add RTS/CTS hardware flow control                          {11}
   05.06.2009 Correct I2C reception                                      {12}
   29.08.2009 Add timer frequency and PWM outputs                        {13}
   08.09.2009 Add internal temperature source to single-ended inputs     {14}
   11.09.2009 Add MULTIDROP_MODE support                                 {15}
   07.11.2009 Adjust PWM to 1/10 % from 1% steps using macro based value {16}
   11.11.2009 Add extended PLL configuration for use with LM3S9BXX devices to achieve more operating frequencies {17}
   29.11.2009 Adapt Ethernet interface to support LM3S9BXX devices       {18}
   27.01.2010 Protect adc_int_complete_handler() from interrupts         {19}
   09.04.2010 Add multicast support for IPV6                             {20}
   15.05.2010 Move UART reset to after enabling peripheral clock         {21}
   15.05.2010 Allow ADC sequencer without interrupt                      {22}
   02.07.2010 Correct syntax on port J                                   {23}
   11.07.2010 Extend ADC support for Tempest parts                       {24}
   05.08.2010 Correct fnWriteBytesFlash() for SPI FLASH with non-Tempest parts {25}
   05.08.2010 Allow fnConfigSPIFileSystem() to be activated with SPI_SW_UPLOAD {26}
   11.08.2010 Correct flash erase moving from internal to external SPI FLASH {27}
   21.09.2010 Correct SPI FLASH page addressing when ATMEL FLASH is in power of twos mode {28}
   28.09.2010 Add external UART support                                  {29}
   06.10.2010 Add 16 bit timer modes                                     {30}
   10.10.2010 Clear possibly pending NVIC interrupt when entering new ones (when working with debugger they can otherwise immediately fire) {31}
   27.12.2010 Add PWM controller support                                 {32}
   04.03.2011 Add small delay after ADC power up so that it can become ready before first access {33}
   01.04.2011 Add CC Studio support                                      {34}
   02.04.2011 Remove random seed from stack to a fixed location          {35}
   07.09.2018 Add hardware operating details for simulator display       {36}

*/

#if defined _LM3SXXXX

//#define _RUN_FLASH_FROM_RAM                                            // program FLASH from RAM - it is possible to program while operating in FLASH but it takes longer

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS
    #include "config.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    extern void fnUpdateOperatingDetails(void);                          // {36}
    #define _SIM_PORT_CHANGE   fnSimPorts();                             // make sure simulator knows of change
    #define START_CODE 0
    #define _fnRAM_code fnCommandFlash
#else
    #ifdef _GNU
        #define __root
    #endif
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define _SIM_PORT_CHANGE
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
    #elif defined _CCSTUDIO                                              // {34}
        extern void _c_int00(void);                                      // CC Studio library initialisation routine
        #define START_CODE _c_int00
        #define _main main    
    #else
        #define START_CODE _main
    #endif
    #define _fnRAM_code fnRAM_code
#endif

#if defined SERIAL_INTERFACE && (NUMBER_EXTERNAL_SERIAL > 0)             // {29}
    #if defined EXT_UART_SC16IS7XX
        #include "spi_sc16IS7xx.h"                                       // include driver
    #endif
#endif

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define __interrupt

#if defined CHIP_HAS_uDMA && defined SERIAL_SUPPORT_DMA
    #ifdef PING_PONG_DMA                                                 // uDMA control block size at start of SRAM
        #define RESERVE_DMA_MEMORY   1024                                // assumes maximum 32 DMA channels
    #else
        #define RESERVE_DMA_MEMORY   512                                 // assumes maximum 32 DMA channels
    #endif
#else
    #define RESERVE_DMA_MEMORY       0
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
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25            // {3}
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_lm3s_atmel.h"
            #include "spi_flash_lm3s_stmicro.h"
            #include "spi_flash_lm3s_sst25.h"                            // {3}
        #undef _SPI_DEFINES
    #endif
#endif
#if defined FLASH_FILE_SYSTEM
    static void fnInitFlash(void);
#endif
static void LM3SXXXX_LowLevelInit(void);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


#if defined ETH_INTERFACE
    static const unsigned char EMAC_RX_int_message[ HEADER_LENGTH ] = { 0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT };   // define fixed interrupt event
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */
#ifdef _CCSTUDIO                                                         // {34}
    static unsigned char __attribute__((section(".vect_tab"))) vectorTable[(sizeof(VECTOR_TABLE)) + RESERVE_DMA_MEMORY]; // reserve space at the start SRAM for the vector table and optionally for uDMA work space  
#endif

#ifdef RANDOM_NUMBER_GENERATOR
    static unsigned short *ptrSeed;
#endif
#if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)]; // FLASH row backup buffer (on word boundary)
#endif

#ifdef ETH_INTERFACE
    static unsigned char *ptrEthTxBuffer;
    static unsigned char *ptrEthRxBuffer[NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE]; // pointer to a number of LAN frame reception buffers
    static int iRxEthBufferPut = 0;
    static int iRxEthBufferGet = 0;
    static unsigned short usEthernetInputSize;
#endif

#ifdef USB_INTERFACE
    static USB_END_POINT *usb_endpoints;
    static unsigned short usFIF0address;
    static USB_HW usb_hardware = {0};
#endif

#ifdef _WINDOWS
    static unsigned short usPhyMode = 0;
#endif

#if defined FLASH_FILE_SYSTEM && defined _RUN_FLASH_FROM_RAM
    static void (*fnRAM_code)(unsigned long, unsigned long, unsigned long) = 0;
#endif

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        unsigned char ucChipSelect[SPI_FLASH_DEVICE_COUNT] = {
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

#ifdef _WINDOWS                              
    extern unsigned char vector_ram[(RESERVE_DMA_MEMORY + sizeof(VECTOR_TABLE))];// vector table in simulated RAM (long word aligned)
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


/* =================================================================== */
/*                      STARTUP CODE                                   */
/* =================================================================== */


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
        #ifdef ROWLEY_2
            #define asm(x) __asm__(x)
        #endif
        extern unsigned char __heap_end__;        
        #define HEAP_START_ADDRESS &__heap_end__                         //  last location of static variables
        #define __disable_interrupt() asm(" cpsid   i")                  // note that the space before the assembler is required by CCStudio (TI compiler)
        #define __enable_interrupt()  asm(" cpsie   i")
        #define __sleep_mode()        asm(" wfi")
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

// main entry for the target code
//
extern int _main(void)
{	
#if defined MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart = HEAP_START_ADDRESS;
#endif
#if defined RANDOM_NUMBER_GENERATOR
  //unsigned short usRandomSeed;                                         // we put an uninitialised variable on to the stack for use as a random seed
    ptrSeed = RANDOM_SEED_LOCATION; // &usRandomSeed;                    // {35}
#endif
#if defined COMPILE_IAR5
    if (__sfe(".bss") > __sfe(".data")) {                                // set last used SRAM address
        ptrTopOfUsedMemory = __sfe(".bss");
    }
    else {
        ptrTopOfUsedMemory = __sfe(".data");
    }
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
                                                                         // we never return....
    return 0;
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

// Function used to enter processor interrupts
//
static void fnEnterInterrupt(int iInterruptID, unsigned char ucPriority, void (*InterruptFunc)(void))
{
    unsigned long *ptrIntSet = IRQ0_31_SER_ADD;
    unsigned char *ptrPriority = IRQ0_3_PRIORITY_REGISTER_ADD;
    VECTOR_TABLE *ptrVect;
    void ( **processor_ints )( void );
    unsigned long ulInterruptBit;
#ifdef _WINDOWS                                                          // back up the present enabled interrupt registers
    unsigned long ulState0 = IRQ0_31_SER;
    unsigned long ulState1 = IRQ32_63_SER;
    IRQ0_31_SER = IRQ32_63_SER = IRQ0_31_CER = IRQ32_63_CER = 0;         // reset registers
#endif
#ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
#else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS + RESERVE_DMA_MEMORY);
#endif
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;
    processor_ints += iInterruptID;
    *processor_ints = InterruptFunc;

    ptrIntSet += (iInterruptID/32);                                      // move to the corresponding set enable register address
    ulInterruptBit = (0x01 << (iInterruptID%32));
    *(ptrIntSet + (IRQ0_31_CPR_ADD - IRQ0_31_SER_ADD)) = ulInterruptBit; // {31} clear possibly pending bit (can occur when working with debugger)
    *ptrIntSet = ulInterruptBit;                                         // enable the interrupt

    ptrPriority += iInterruptID;
    *ptrPriority = ucPriority;                                           // define the interrupt's priority
#ifdef _WINDOWS
    IRQ0_31_SER  |= ulState0;                                            // synchronise the interrupt masks
    IRQ32_63_SER |= ulState1;
    IRQ0_31_CER   = IRQ0_31_SER;
    IRQ32_63_CER  = IRQ32_63_SER;
#endif
}


#ifdef RANDOM_NUMBER_GENERATOR
// How the random number seed is set depends on the hardware possibilities available.
//
extern void fnInitialiseRND(unsigned short *usSeedValue)                 // {7}
{
    if ((*usSeedValue = *ptrSeed) == 0) {                                // we hope that the content of the random seed is random after a power up
        *usSeedValue = 0x127b;                                           // we must not set a zero - so we set a fixed value
    }                                                                    // after a reset it should be well random since it will contain the value at the reset time
    *ptrSeed = fnRandom();                                               // ensure that the seed value is modified between resets
}
#endif


// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#ifdef GLOBAL_HARDWARE_TIMER
  static __interrupt void hwtimer_interrupt(void);
  volatile unsigned int dummy;
#endif
#ifdef _WINDOWS
    unsigned long ulPortPullups[] = {
    #if PART_DC4 & GPIOA_PRESENT4
        PORTA_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_lm3sxxxx.h
    #endif
    #if PART_DC4 & GPIOB_PRESENT4
        PORTB_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOC_PRESENT4
        PORTC_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOD_PRESENT4
        PORTD_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOE_PRESENT4
        PORTE_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOF_PRESENT4
        PORTF_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOG_PRESENT4
        PORTG_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOH_PRESENT4
        PORTH_DEFAULT_INPUT,
    #endif
    #if PART_DC4 & GPIOJ_PRESENT4
        PORTJ_DEFAULT_INPUT,                                             // {23}
    #endif
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)
        ((AN0_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),                   // initial voltages when simulating
        ((AN1_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN2_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN3_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN4_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN5_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN6_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN7_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
    #endif
    #if defined SUPPORT_ADC && (PART_DC1 & ADC1_PRESENT1)                // second ADC - assume 16 ADC inputs available
        ((AN8_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),                   // initial voltages when simulating
        ((AN9_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN10_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN11_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN12_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN13_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN14_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
        ((AN15_START_VOLTAGE) / ADC_REFERENCE_VOLTAGE),
    #endif
    };
    #ifdef RANDOM_NUMBER_GENERATOR
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;                                             // {35} note that the target works differently
    #endif
    fnInitialiseDevice((void *)ulPortPullups);
#endif
    LM3SXXXX_LowLevelInit();
#if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    uMemset(ulFlashRow, 0xff, FLASH_ROW_SIZE);                           // flush the intermediate FLASH buffer
#endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
    INIT_WATCHDOG_DISABLE();                                             // initialise ready for checking of watchdog diabled
    if (WATCHDOG_DISABLE() == 0) {
        RCGC0 |= CGC_WDT;                                                // enable clocks to the watchdog module
        SRCR0 &= ~CGC_WDT;                                               // ensure watchdog module is not in reset
        WDTLOCK = WD_ACCESS;                                             // ensure we can access the watchdog registers
        WDTTEST = WD_STALL;                                              // allow watchdog counter stalling when debugging
        WDTLOAD = (WATCHDOG_TIMEOUT_MS * (MASTER_CLOCK/1000/2));         // allow user configuration of timeout in ms (times out twice before reset)
        WDTCTL = WD_RESEN;                                               // enable reset on second timeout
        WDTCTL = (WD_RESEN | WD_INTEN);                                  // enable watchdog and interrupt
        WDTICR = 0;                                                      // retrigger the watchdog, causing reload from load register
#ifdef _WINDOWS
        WDTVALUE = WDTLOAD;                                              // latch the reload value to the counter
#endif
        WDTLOCK = 0;                                                     // lock against further uncontrolled accesses
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
        #include "spi_flash_lm3s_atmel.h"
        #include "spi_flash_lm3s_stmicro.h"
        #include "spi_flash_lm3s_sst25.h"                                // {3}
    #undef _CHECK_SPI_CHIPS
    #else
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
    #endif
#endif
#ifdef FLASH_FILE_SYSTEM
    fnInitFlash();
#endif

#ifdef GLOBAL_HARDWARE_TIMER                                             // initialise the timer for use with global hardware timers
    PMC_PCER = TC2;                                                      // enable clocks to timer 2 in PMC
    TC_CCR_2 = (TIM_CLKDIS | TIM_SWTRG);                                 // disable timer clock and reset timer

    dummy = TC_SR_2;                                                     // read status register to reset possible pending interrupt

    fnEnterInterrupt(TC2, PRIORITY_HW_TIMER, hwtimer_interrupt);

    #if MILLISEC == LOW_RES_MS
    TC_CMR_2  = (TIMER_CLOCK_5 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// MCK/1024, wave mode, trigger on register RC, stop after triggering
    #elif MILLISEC == MED_RES_MS
    TC_CMR_2  = (TIMER_CLOCK_4 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// MCK/128, wave mode, trigger on register RC, stop after triggering
    #else
    TC_CMR_2  = (TIMER_CLOCK_3 | WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS);// MCK/32, wave mode, trigger on register RC, stop after triggering
    #endif
#endif
}


// Support watchdog re-triggering of specific hardware
//
extern void fnRetriggerWatchdog(void)
{
    WDTICR = 0;                                                          // retrigger the watchdog, causing reload from load register
    TOGGLE_WATCHDOG_LED();                                               // blink an LED for visualisation
}

// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)
{
#ifdef _WINDOWS
    LM3Sxxxx.CORTEX_M3_REGS.ulPRIMASK = INTERRUPT_MASKED;                // mark that interrupts are masked
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
        *(int *)0 = 0;                                                   // basic error - cause simulator exception
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
    }
#endif
    if (!(--iInterruptLevel)) {                                          // only when no more interrupt nesting,
#ifdef _WINDOWS
        LM3Sxxxx.CORTEX_M3_REGS.ulPRIMASK = 0;                           // mark that interrupts are not masked
#else
        __enable_interrupt();                                            // enable processor interrupts
#endif
    }
}

// TICK interrupt
//
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

// Routine to initialise the Real Time Tick interrupt (uses Cortex M3 SysTick timer)
//
#define REQUIRED_MS ((1000000/TICK_RESOLUTION))                          // the TICK frequency we require in kHz
#define TICK_DIVIDE (((MASTER_CLOCK + REQUIRED_MS/2)/REQUIRED_MS) - 1)   // the divide ratio required


extern void fnStartTick(void)
{
    VECTOR_TABLE *ptrVect;
#ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
#else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS + RESERVE_DMA_MEMORY);
#endif

    SYSTICK_RELOAD = TICK_DIVIDE;                                        // set reload value to determine the period
    ptrVect->ptrSysTick = _RealTimeInterrupt;                            // enter interrupt handler
    SYSTEM_HANDLER_12_15_PRIORITY_REGISTER |= (SYSTICK_PRIORITY << 24);  // enter the SYSTICK priority
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE | SYSTICK_TICKINT); // enable timer and its interrupt
#ifdef _WINDOWS
    SYSTICK_RELOAD &= SYSTICK_COUNT_MASK;                                // mask any values which are out of range
    SYSTICK_CURRENT = SYSTICK_RELOAD;                                    // prime the reload count
#endif
}


#ifdef ETH_INTERFACE
/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */

// This is called when an Ethernet event has been received. Generally it is due to a reception and returns channel and message details.
// We are more or less forced to copy a number of input buffers to a linear input buffer so that it can be handled by standard routines
// Therefore we step through one or a number of receive buffer descriptors, receiving the amound of data in them.
//
extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
    if (EMAC_RX_INTERRUPT == *ucEvent) {
        if (*(unsigned short *)ptrEthRxBuffer[iRxEthBufferGet] == 0) {
            return -1;                                                   // nothing (else) waiting
        }
        rx_frame->frame_size = *(unsigned short *)ptrEthRxBuffer[iRxEthBufferGet];
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)(ptrEthRxBuffer[iRxEthBufferGet] + sizeof(unsigned short)); // return pointer to the input buffer - after increment to beyond the stored length
        return 0;                                                        // channel received from 0 (we use only 0)
    }
    #ifdef USE_IP_STATS
    else if (ETHERNET_RX_OVERRUN == *ucEvent) {
        fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES);                  // we lost a frame due to RX overrun so count the event
    }
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
    #endif
    return -1;                                                           // invalid channel
}


// Routine to read a word from the PHY
//
static unsigned short fnReadMII(unsigned long ulRegAdr)
{
    MACRIS_IACK = MDINT;                                                 // clear MDI interrupt
    MACMCTL = (READ_FROM_PHY | ulRegAdr);                                // command read from given address

    #ifdef _WINDOWS
    if (LM3SXXXX_PHY_IDENTIFIER_1 == ulRegAdr) {                         // simulate reads from identifier register
        MACMRXD = (PHY_IDENTIFIER >> 16);
    }
    else if (LM3SXXXX_PHY_IDENTIFIER_2 == ulRegAdr) {
        MACMRXD = (PHY_IDENTIFIER & 0x0000ffff);
    }
        #ifdef _TEMPEST_CLASS                                            // {18}
    else if (LM3SXXXX_PHY_INTERRUPT_STATUS == ulRegAdr) {
        MACMRXD = PHY_ENERGYON_INT;
    }    
        #else
    else if (LM3SXXXX_INTERRUPT_CONTROL_STATUS_REGISTER == ulRegAdr) {
        MACMRXD = PHY_LINK_UP_INT;
    }
        #endif
    else if (LM3SXXXX_BASIC_STATUS_REGISTER == ulRegAdr) {
        MACMRXD = PHY_100MEG_FULL_DUPLEX;
    }
    #endif

    while (!(MACRIS_IACK & MDINT)) {}                                    // wait for reception to complete

    return (unsigned short)MACMRXD;                                      // return the value read
}

// Routine to write a word to the PHY
//
static void fnWriteMII(unsigned long ulRegAdr, unsigned short usData)
{
    MACMTXD = usData;
    MACRIS_IACK = MDINT;                                                 // clear MDI interrupt
    MACMCTL = (WRITE_TO_PHY | ulRegAdr);                                 // command read from given address

    #ifdef _WINDOWS
    if (LM3SXXXX_BASIC_CONTROL_REGISTER == ulRegAdr) {
        usPhyMode = usData;
    }
    #endif
    while (!(MACRIS_IACK & MDINT)) {}                                    // wait for reception to complete
}


// EMAC interrupt dispatcher
//
__interrupt void EMAC_Interrupt(void)
{
    unsigned long ulMask = (unsigned long)MACIM;                         // EMAC interrupt mask
    #ifdef _WINDOWS
    extern unsigned long fnGetFifo_long(void);
    #define GET_FIFO_LONG() fnGetFifo_long();                            // {4}
    #else
    #define GET_FIFO_LONG() MACDATA
    #endif
    while ((MACRIS_IACK & ulMask) != 0) {                                // while enabled interrupts are waiting
    #ifdef LAN_REPORT_ACTIVITY
        unsigned char EMAC_int_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT };
    #endif
        if (MACRIS_IACK & (RXINT | RXER | FOV)) {                        // reception frame available
            if (MACRIS_IACK & (RXER | FOV)) {                            // error
                unsigned char EMAC_err_message[ HEADER_LENGTH ];         // = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, ETHERNET_RX_ERROR };
                EMAC_err_message[MSG_DESTINATION_NODE] = EMAC_err_message[MSG_SOURCE_NODE] = 0;
                EMAC_err_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_LAN_EXCEPTIONS;
                EMAC_err_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
                EMAC_err_message[MSG_INTERRUPT_EVENT] = ETHERNET_RX_ERROR;
                if (MACRIS_IACK & FOV) {
                    EMAC_err_message[MSG_INTERRUPT_EVENT] = ETHERNET_RX_OVERRUN;
                }
                uDisable_Interrupt();                                    // ensure message can not be interrupted
                fnWrite(INTERNAL_ROUTE, EMAC_err_message, HEADER_LENGTH);// inform the Ethernet task
                uEnable_Interrupt();                                     // release
            }
            else {
                unsigned long *ptrRxBuffer = (unsigned long *)ptrEthRxBuffer[iRxEthBufferPut]; // buffer is aligned
                unsigned long ulMacRxFrameLength = GET_FIFO_LONG();
                if (*(unsigned short *)ptrEthRxBuffer[iRxEthBufferPut] != 0) { // if the user has not yet retrieved last buffer, we reject latest frame and reset the Rx FIFO
                    unsigned char EMAC_err_message[ HEADER_LENGTH ];     // = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, ETHERNET_RX_ERROR };
                    EMAC_err_message[MSG_DESTINATION_NODE] = EMAC_err_message[MSG_SOURCE_NODE] = 0;
                    EMAC_err_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_LAN_EXCEPTIONS;
                    EMAC_err_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
                    EMAC_err_message[MSG_INTERRUPT_EVENT] = ETHERNET_RX_ERROR;
                    MACRCTL |= MAC_RSTFIFO;                              // flush FIFO
                    uDisable_Interrupt();                                // ensure message can not be interrupted
                    fnWrite(INTERNAL_ROUTE, EMAC_err_message, HEADER_LENGTH);// inform the Ethernet task
                    uEnable_Interrupt();                                 // release
                    MACRCTL &= ~MAC_RSTFIFO;
                }
                else {
                    unsigned long ulExtraInput;
                    *ptrRxBuffer = ulMacRxFrameLength - sizeof(unsigned short); // put the length and first 2 bytes into the buffer
                    ulMacRxFrameLength &= 0x0000ffff;                    // mask out the length
                    if (ulMacRxFrameLength > usEthernetInputSize) {
                        ulExtraInput = (ulMacRxFrameLength - usEthernetInputSize); // discard extra bytes
                        ulMacRxFrameLength = usEthernetInputSize;        // the size that we can accept
                        *(unsigned short *)ptrRxBuffer = usEthernetInputSize;
                    }
                    else {
                        ulExtraInput = 0;
                    }

                    while (1) {
                        *(++ptrRxBuffer) = GET_FIFO_LONG();              // copy all data from RX FIFO
                        if (ulMacRxFrameLength <= sizeof(unsigned long)) {
                            while (ulExtraInput) {
                                ulMacRxFrameLength = GET_FIFO_LONG();    // flush extra input
                                if (ulExtraInput <= 4) {
                                    break;
                                }
                                ulExtraInput -= sizeof(unsigned long);
                            }
                            break;
                        }
                        ulMacRxFrameLength -= sizeof(unsigned long);
                    }
                    if (++iRxEthBufferPut >= NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE) {
                        iRxEthBufferPut = 0;
                    }
                    uDisable_Interrupt();                                // ensure message can not be interrupted
                    fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_RX_int_message, HEADER_LENGTH); // inform the Ethernet task that there is a valid input buffer waiting
    #ifdef LAN_REPORT_ACTIVITY
                    EMAC_int_message[MSG_INTERRUPT_EVENT] = EMAC_RX_INTERRUPT;
                    fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
    #endif
                    uEnable_Interrupt();                                 // release
                }
            }
            MACRIS_IACK = (RXINT | RXER | FOV);                          // reset rx flags
    #ifdef _WINDOWS
            MACRIS_IACK &= ~(RXINT | RXER | FOV);                        // clear interrupt flags when simulating
    #endif
        }

        if (MACRIS_IACK & (TXEMP | TXER)) {                              // frame transmitted
            eth_tx_control->ETH_queue.chars = 0;                         // reset the transmission buffer for next use
            eth_tx_control->ETH_queue.put = eth_tx_control->ETH_queue.QUEbuffer;
    #ifdef LAN_REPORT_ACTIVITY
            if (MACRIS_IACK & TXER) {                                    // collision detection
                EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_COLLISION;
            }
            uDisable_Interrupt();                                        // ensure message can not be interrupted
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
            uEnable_Interrupt();                                         // release
    #endif
            MACRIS_IACK = (TXEMP | TXER);                                // reset tx flags
    #ifdef _WINDOWS
            MACRIS_IACK &= ~(TXEMP | TXER);                              // clear interrupt flags when simulating
            MACTR &= ~NEWTX;                                             // transmitter no longer in use
    #endif
        }

        if (MACRIS_IACK & PHYINT) {                                      // PHY interrupt
    #ifdef INTERRUPT_TASK_PHY
            unsigned short usInterrupt = fnReadMII(LM3SXXXX_PHY_INTERRUPT_STATUS); // read the cause(s) of the interrupt, which resets the bits
            unsigned char int_phy_message[HEADER_LENGTH]; // = { 0, 0 , INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT };    // define standard interrupt event
            int_phy_message[MSG_DESTINATION_NODE] = int_phy_message[MSG_SOURCE_NODE] = 0;
            int_phy_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_PHY;
            int_phy_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
            int_phy_message[MSG_INTERRUPT_EVENT] = UNKNOWN_INTERRUPT;
            if ((PHY_ENERGYON_INT | PHY_AUTONEG_COMPLETE_INT) & usInterrupt) {
                usInterrupt = fnReadMII(LM3SXXXX_BASIC_STATUS_REGISTER); // check the  details of link
                if (usInterrupt & (PHY_100MEG_HALF_DUPLEX | PHY_100MEG_FULL_DUPLEX)) {
                    int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
                }
                else {
                    int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
                }
            }
            else if (PHY_LINK_DOWN_INT & usInterrupt) {
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
            }
        #ifdef _TEMPEST_CLASS                                            // {18}
        #else
            unsigned short usInterrupt = fnReadMII(LM3SXXXX_INTERRUPT_CONTROL_STATUS_REGISTER);// read the cause(s) of the interrupt, which resets the bits
            unsigned char int_phy_message[HEADER_LENGTH]; // = { 0, 0 , INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT };    // define standard interrupt event
            int_phy_message[MSG_DESTINATION_NODE] = int_phy_message[MSG_SOURCE_NODE] = 0;
            int_phy_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_PHY;
            int_phy_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
            int_phy_message[MSG_INTERRUPT_EVENT] = UNKNOWN_INTERRUPT;
            if (PHY_LINK_UP_INT & usInterrupt) {
                usInterrupt = fnReadMII(LM3SXXXX_BASIC_STATUS_REGISTER); // check the  details of link
                if (usInterrupt & (PHY_100MEG_HALF_DUPLEX | PHY_100MEG_FULL_DUPLEX)) {
                    int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
                }
                else {
                    int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
                }
            }
            else if (PHY_LINK_DOWN_INT & usInterrupt) {
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
            }
    #endif
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // Inform the corresponding task
            uEnable_Interrupt();                                         // release
    #else
        #ifdef _TEMPEST_CLASS                                            // {18}
            fnReadMII(LM3SXXXX_PHY_INTERRUPT_STATUS);                    // read the cause(s) of the interrupt, which resets the bits
        #else
            fnReadMII(LM3SXXXX_INTERRUPT_CONTROL_STATUS_REGISTER);       // read the cause(s) of the interrupt, which resets the bits
        #endif
    #endif
            MACRIS_IACK = PHYINT;                                        // reset interrupt flag
    #ifdef _WINDOWS
            MACRIS_IACK &= ~(PHYINT);                                    // clear interrupt flag when simulating
    #endif
        }
    }
}


// Ethernet configuration routine
//
extern void fnConfigEthernet(ETHTABLE *pars)
{
    unsigned long  ulPhyIdentifier;
    unsigned long  ulTemp;
    unsigned short usMIIData;

    RCGC2 |= (CGC_EPHY0 | CGC_EMAC0);                                    // enable power to the EMAC and PHY
    SRCR2 &= ~(CGC_EPHY0 | CGC_EMAC0);                                   // ensure they are not held in reset

    MACMDV = ((MASTER_CLOCK / 5) - 1);                                   // set the MII clock speed to 2.5MHz

    #ifdef USE_IPV6                                                      // {20}
    ulTemp =  network.ucOurMAC[3] << 24;                                 // set the MAC address
    ulTemp |= network.ucOurMAC[2] << 16;
    ulTemp |= network.ucOurMAC[1] << 8;
    ulTemp |= network.ucOurMAC[0];
    MACIA0 = ulTemp;
    ulTemp =  network.ucOurMAC[5] << 8;
    ulTemp |= network.ucOurMAC[4];
    MACIA1 = ulTemp;
    #else
    ulTemp =  pars->ucMAC[3] << 24;                                      // set the MAC address
    ulTemp |= pars->ucMAC[2] << 16;
    ulTemp |= pars->ucMAC[1] << 8;
    ulTemp |= pars->ucMAC[0];
    MACIA0 = ulTemp;
    ulTemp =  pars->ucMAC[5] << 8;
    ulTemp |= pars->ucMAC[4];
    MACIA1 = ulTemp;
    #endif

    if (pars->usMode & FULL_DUPLEX) {                                    // program the transmission operating mode
        MACTCTL = (MAC_TX_CRC | MAC_DUPLEX | MAC_PADEN);                 // full duplex operation
    }
    else {
        MACTCTL = (MAC_TX_CRC | MAC_PADEN);                              // half duplex operation
    }
    if (pars->usMode & PROMISCUOUS) {                                    // program the reception operating mode
        MACRCTL = (MAC_RSTFIFO | MAC_BADCRC | MAC_PRMS);                 // promiscuous mode
    }
    #ifdef USE_IPV6                                                      // {20}
    else if (pars->usMode & CON_MULTICAST) {                             // if multicast reception is to be configured
        MACRCTL = (MAC_RSTFIFO | MAC_BADCRC | MAC_AMUL);
    }
    #endif
    else {
        MACRCTL = (MAC_RSTFIFO | MAC_BADCRC);                            // non-promiscuous mode
    }

    ulPhyIdentifier = fnReadMII(LM3SXXXX_PHY_IDENTIFIER_1);              // check that the PHY is working correctly by reading its identifier
    ulPhyIdentifier <<= 16;
    ulPhyIdentifier |= fnReadMII(LM3SXXXX_PHY_IDENTIFIER_2);             // check that the PHY is working correctly by reading its identifier

    if ((ulPhyIdentifier & PHY_MASK) != PHY_IDENTIFIER) {
        return;                                                          // if the identifier is incorrect the phy isn't resonding correctly
    }

    if (pars->usMode & LAN_LEDS) {                                       // enable LEDs to be controlled by PHY
        _CONFIG_PERIPHERAL(F, EMAC0, 2, (PORTF_BIT2 | PORTF_BIT3), (PF3_LED0 | PF2_LED1)); // connect PHY control outputs to LED port lines
    }

    if (pars->usMode & (LAN_10M | LAN_100M)) {                           // if fixed speed
        usMIIData = 0;
        if (pars->usMode & LAN_100M)  {
            usMIIData |= PHY_SELECT_100M_SPEED;
        }
        if (pars->usMode & FULL_DUPLEX) {
            usMIIData |= PHY_FULL_DUPLEX_MODE;
        }
    }
    else {                                                               // else autonegotiate
        usMIIData = (PHY_ENABLE_AUTO_NEGOTIATION | PHY_RESTART_AUTO_NEGOTIATION);
    }

    if (pars->usMode & LOOPBACK) {
        usMIIData |= PHY_LOOP_BACK;                                      // test loop back at PHY
    }

    fnWriteMII(LM3SXXXX_BASIC_CONTROL_REGISTER, usMIIData);              // write the PHY operating mode

    ulTemp = 0;
    usEthernetInputSize = pars->usSizeRx;                                // save the length of input buffer used
    usMIIData = pars->usSizeRx + sizeof(unsigned short);                 // {1} ensure the buffer size has space to hold the length and is divisable by 4 (reuse variable usMIIData)
    while (usMIIData & 0x3) {
        usMIIData++;
    }
    while (ulTemp < NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE) {
        ptrEthRxBuffer[ulTemp++] = uMalloc((MAX_MALLOC)usMIIData);       // allocate a receive buffer pool (aligned addresses)
    }
    ptrEthTxBuffer = uMalloc((MAX_MALLOC)(pars->usSizeTx + sizeof(unsigned short))); // allocate a single tx message buffer (aligned address)
    ptrEthTxBuffer += sizeof(unsigned short);                            // make space for length when sending - the start of length is aligned

    MACIM = 0;                                                           // disable all interrupts
    MACRIS_IACK = (PHYINT | RXINT | TXEMP | TXER | FOV | RXER);          // reset pending interrupts

    fnEnterInterrupt(irq_Ethernet_Controller_ID, PRIORITY_EMAC, EMAC_Interrupt);

    MACIM = (PHYINT | RXINT | TXEMP | TXER | FOV | RXER);                // enable EMAC and PHY interrupts
#ifdef _TEMPEST_CLASS                                                    // {18}
    fnWriteMII(LM3SXXXX_PHY_INTERRUPT_MASK,                              // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_ENERGYON_INT_ENABLE | PHY_AUTONEG_COMPLETE_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
#else
    fnWriteMII(LM3SXXXX_INTERRUPT_CONTROL_STATUS_REGISTER,               // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
#endif

    MACRCTL &= ~MAC_RSTFIFO;
    MACTCTL |= MAC_TXEN;                                                 // enable transmitter
    MACRCTL |= MAC_RXEN;                                                 // enable receiver

    #ifdef _WINDOWS
    fnOpenDefaultHostAdapter();                                          // simulate link up
    #endif
    return;
}



#ifdef _WINDOWS
// This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
// on the programmed MAC address and the Ethernet operating mode
//
extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
{
    unsigned char ucProgrammedMAC[MAC_LENGTH];
    
    if (usLen < (2*MAC_LENGTH + 2)) {
        return 0;                                                        // too short to be a valid Ethernet frame
    }

    ucProgrammedMAC[5] = (unsigned char)(MACIA1>>8);                     // check mac address
    ucProgrammedMAC[4] = (unsigned char)(MACIA1);
    ucProgrammedMAC[3] = (unsigned char)(MACIA0>>24);
    ucProgrammedMAC[2] = (unsigned char)(MACIA0>>16);
    ucProgrammedMAC[1] = (unsigned char)(MACIA0>>8);
    ucProgrammedMAC[0] = (unsigned char)(MACIA0);

    if (!uMemcmp(&ucData[MAC_LENGTH], ucProgrammedMAC, MAC_LENGTH)) {
        return 0;                                                        // we never accept echos of our own transmissions
    }

    if (!(uMemcmp(ucData, cucBroadcast, MAC_LENGTH))) {
        return 1;                                                        // broadcast - always accept it
    }

    if (!(uMemcmp(ucData, ucProgrammedMAC, MAC_LENGTH))) {
        return 2;                                                        // our MAC
    }

    if (MACRCTL & MAC_PRMS) {
        return 3;                                                        // in promiscuous mode we accept everything
    }

    if ((MACRCTL & MAC_AMUL) && (*(ucData + 1) & 0x01)) {                // {20} if multicast frames are enabled accept multicast frames
        return 4;
    }

    return 0;                                                            // no match so reject
}
#endif


// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    volatile int iMaxWait = 0xffff;

    while (MACTR & NEWTX) {
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
    return NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return (ptrEthRxBuffer[iBufNr]);
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    return ptrEthTxBuffer;
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
    *(unsigned short*)ptrEthRxBuffer[iRxEthBufferGet] = 0;               // free the buffer
    if (++iRxEthBufferGet >= NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE) {  // set to next
        iRxEthBufferGet = 0;
    }
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    #ifdef _WINDOWS
    #define PUT_FIFO_LONG(x)
    QUEUE_TRANSFER length;
    #else
    #define PUT_FIFO_LONG(x) MACDATA = x
    #endif
    #ifdef _WINDOWS
    while (DataLen < 60) {                                               // device performs automatic zero padding
        *ptr_put++ = 0x00;                                               // pad with zeros if smaller than 60 [chip must send at least 60]
        DataLen++;
    }
    length = DataLen;
    #endif

    ptr_put -= (DataLen + 2);                                            // move back to long word boundary and put length in the buffer
    *ptr_put++ = (unsigned char)(DataLen - ETH_HEADER_LEN);              // put length (LSB) - only data without Ethernet header
    *ptr_put++ = (unsigned char)((DataLen - ETH_HEADER_LEN) >> 8);       // put length (MSB) - only data without Ethernet header
    ptr_put -= 2;                                                        // put back to start (on long word boundary)
    DataLen += 2;                                                        // include length

    if (fnWaitTxFree() != 0) {                                           // ensure output FIFO is free
        return 0;                                                        // waited too long, failed
    }

    while (1) {
        PUT_FIFO_LONG(*(unsigned long *)ptr_put);
        if (DataLen <= sizeof(unsigned long)) {
            break;
        }
        ptr_put += sizeof(unsigned long);
        DataLen -= sizeof(unsigned long);
    }

    MACTR = NEWTX;                                                       // start transmission

    #ifdef _WINDOWS
    fnSimulateEthTx(length);
    ptr_put -= length;
    ptr_put += DataLen;
    if (usPhyMode & PHY_LOOP_BACK) {                                     // if the PHY is in loop back mode, simulate reception of sent frame
        fnSimulateEthernetIn(ptr_put, length, 1);
    }
        #ifdef PSEUDO_LOOPBACK                                           // if we detect an IP frame being sent to our own address we loop it back to the input
    else if (((*(ptr_put + 12)) == 0x08) && (!(uMemcmp(&network.ucOurIP[0], (ptr_put + 26), IPV4_LENGTH)))) {
        fnSimulateEthernetIn(ptr_put, length, 1);
    }
        #endif
    MACRIS_IACK |= TXEMP;
    if (MACIM & TXEMP) {                                                 // if interrupt enabled
        VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
        ptrVect->processor_interrupts.irq_Ethernet_Controller();         // simulate end of transmission - tx interrupt
    }
    #endif
    return (DataLen);
}
#endif                                                                   // end of Ethernet option


#if defined SUPPORT_PORT_INTERRUPTS || defined SUPPORT_HW_FLOW
static LM3XXXXX_PORT_CONTROL *fnGetPortBlock(unsigned char ucPortReference)
{
    switch (ucPortReference) {
    #if (PART_DC4 & GPIOA_PRESENT4)
    case PORTA:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_A_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOB_PRESENT4)
    case PORTB:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_B_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOC_PRESENT4)
    case PORTC:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_C_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOD_PRESENT4)
    case PORTD:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_D_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOE_PRESENT4)
    case PORTE:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_E_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOF_PRESENT4)
    case PORTF:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_F_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOG_PRESENT4)
    case PORTG:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_G_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOH_PRESENT4)
    case PORTH:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_H_BLOCK + 0x3fc);
    #endif
    #if (PART_DC4 & GPIOJ_PRESENT4)
    case PORTJ:
        return (LM3XXXXX_PORT_CONTROL *)(GPIO_PORT_J_BLOCK + 0x3fc);
    #endif
    default:
        return 0;
    }
}
#endif


#ifdef SERIAL_INTERFACE
/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */

    #ifdef _WINDOWS
    extern unsigned char ucTxLast[CHIP_HAS_UARTS] = {0};
    #endif

// Collect a pointer to defined UART control block
//
static LM3XXXXX_UART_CONTROL *fnSelectChannel(QUEUE_HANDLE Channel)
{
    switch (Channel) {
    case 0:
        return (LM3XXXXX_UART_CONTROL *)UART_0_BLOCK;
    #if CHIP_HAS_UARTS > 1
    case 1:
        return (LM3XXXXX_UART_CONTROL *)UART_1_BLOCK;
    #endif
    #if CHIP_HAS_UARTS > 2
    case 2:
        return (LM3XXXXX_UART_CONTROL *)UART_2_BLOCK;
    #endif
    }
    return 0;                                                            // invalid
}

#if defined SERIAL_SUPPORT_DMA && defined CHIP_HAS_uDMA                  // {9}
    #ifdef _WINDOWS
        #define fnGetDMA_control(x)  (DMA_CONTROL_STRUCTURE *)((CAST_POINTER_ARITHMETIC)&vector_ram + (x*sizeof(DMA_CONTROL_STRUCTURE)));
    #else
        #define fnGetDMA_control(x)  (DMA_CONTROL_STRUCTURE *)(RAM_START_ADDRESS + (x*sizeof(DMA_CONTROL_STRUCTURE)));
    #endif
#endif

// configure the SCI hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    LM3XXXXX_UART_CONTROL *uart;
    unsigned short usSpeedInteger;
    unsigned char  ucSpeedFraction;

    switch (Channel) {                                                   // enable clocks to the peripheral and configure pins
    case 0:
        _CONFIG_PERIPHERAL(A, UART0, 1, (PORTA_BIT0 | PORTA_BIT1), (PA0_U0Rx | PA1_U0Tx));
    #if defined SERIAL_SUPPORT_DMA && defined CHIP_HAS_uDMA
        if (pars->ucDMAConfig & UART_TX_DMA) {
            DMA_CONTROL_STRUCTURE *ptrDmaControl = fnGetDMA_control(DMA_UART0_TRANSMIT_CH);
            ptrDmaControl->DMADSTENDP = (volatile unsigned char *)UARTDR_0_ADDR; // set fixed transfer destination address
            DMAALTCLR = DMA_UART0_TRANSMIT_BIT;                          // set to primary DMA control block
            DMAUSEBURSTCLR = DMA_UART0_TRANSMIT_BIT;                     // respond to single and burst requests
            DMAREQMASKCLR = DMA_UART0_TRANSMIT_BIT;                      // enable DMA requests for the channel
            UARTDMACTL_0 = TXDMAE;                                       // enable DMA transmit operation on UART
        }
    #endif
        break;
    #if CHIP_HAS_UARTS > 1
    case 1:
        #if defined UART1_ON_PORT_C
        _CONFIG_PERIPHERAL(C, UART1, 1, (PORTC_BIT6 | PORTC_BIT7), (PC6_U1Rx | PC7_U1Tx));
        #else
        _CONFIG_PERIPHERAL(D, UART1, 1, (PORTD_BIT2 | PORTD_BIT3), (PD2_U1Rx | PD3_U1Tx));
        #endif
        #if defined SERIAL_SUPPORT_DMA && defined CHIP_HAS_uDMA
        if (pars->ucDMAConfig & UART_TX_DMA) {
            DMA_CONTROL_STRUCTURE *ptrDmaControl = fnGetDMA_control(DMA_UART1_TRANSMIT_CH);
            ptrDmaControl->DMADSTENDP = (volatile unsigned char *)UARTDR_1_ADDR; // set fixed transfer destination address
            DMAALTCLR = DMA_UART1_TRANSMIT_BIT;                          // set to primary DMA control block
            DMAUSEBURSTCLR = DMA_UART1_TRANSMIT_BIT;                     // respond to single and burst requests
            DMAREQMASKCLR = DMA_UART1_TRANSMIT_BIT;                      // enable DMA requests for the channel
            UARTDMACTL_1 = TXDMAE;                                       // enable DMA transmit operation on UART
        }
        #endif
        break;
    #endif
    #if CHIP_HAS_UARTS > 2
    case 2:
        _CONFIG_PERIPHERAL(G, UART2, 1, (PORTG_BIT0 | PORTG_BIT1), (PG0_U2Rx | PG1_U2Tx));
        #if defined  SERIAL_SUPPORT_DMA && defined CHIP_HAS_uDMA
        if (pars->ucDMAConfig & UART_TX_DMA) {
            DMA_CONTROL_STRUCTURE *ptrDmaControl = fnGetDMA_control(DMA_UART2_TRANSMIT_CH);
            ptrDmaControl->DMADSTENDP = (volatile unsigned char *)UARTDR_2_ADDR; // set fixed transfer destination address
            DMAALTCLR = DMA_UART2_TRANSMIT_BIT;                          // set to primary DMA control block
            DMAUSEBURSTCLR = DMA_UART2_TRANSMIT_BIT;                     // respond to single and burst requests
            DMAREQMASKCLR = DMA_UART2_TRANSMIT_BIT;                      // enable DMA requests for the channel
            UARTDMACTL_2 = TXDMAE;                                       // enable DMA transmit operation on UART
        }
        #endif
        break;
    default:
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {29}
        fnConfigExtSCI((QUEUE_HANDLE)(Channel - NUMBER_SERIAL), pars);   // pass on to external UART driver
        #ifdef SUPPORT_HW_FLOW
        if ((pars->Config & RTS_CTS) != 0) {                             // HW flow control defined so configure RTS/CTS pins
            fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
        }
        #endif
    #endif
        return;
    #endif
    }
    uart = fnSelectChannel(Channel);
    uart->UARTCTL = 0;                                                   // {21} disable before configuring
    #ifdef SUPPORT_HW_FLOW                                               // {6}
    if (pars->Config & RTS_CTS) {                                        // HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        usSpeedInteger = ((MASTER_CLOCK/16)/300);                        // set 300
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)300) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)300))) + (float)0.5));
        break;
    case SERIAL_BAUD_600:
        usSpeedInteger = ((MASTER_CLOCK/16)/600);                        // set 600
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)600) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)600))) + (float)0.5));
        break;
    case SERIAL_BAUD_1200:
        usSpeedInteger = ((MASTER_CLOCK/16)/1200);                       // set 1200
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)1200) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)1200))) + (float)0.5));
        break;
    case SERIAL_BAUD_2400:
        usSpeedInteger = ((MASTER_CLOCK/16)/2400);                       // set 1200
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)2400) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)2400))) + (float)0.5));
        break;
    case SERIAL_BAUD_4800:
        usSpeedInteger = ((MASTER_CLOCK/16)/4800);                       // set 4800
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)4800) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)4800))) + (float)0.5));
        break;
    case SERIAL_BAUD_9600:
        usSpeedInteger = ((MASTER_CLOCK/16)/9600);                       // set 9600
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)9600) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)9600))) + (float)0.5));
        break;
    case SERIAL_BAUD_14400:
        usSpeedInteger = ((MASTER_CLOCK/16)/14400);                      // set 14400
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)14400) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)14400))) + (float)0.5));
        break;
    default:                                                             // if not valid value set this one
    case SERIAL_BAUD_19200:
        usSpeedInteger = ((MASTER_CLOCK/16)/19200);                      // set 19200
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)19200) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)19200))) + (float)0.5));
        break;
    case SERIAL_BAUD_38400:
        usSpeedInteger = ((MASTER_CLOCK/16)/38400);                      // set 38400
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)38400) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)38400))) + (float)0.5));
        break;
    case SERIAL_BAUD_57600:
        usSpeedInteger = ((MASTER_CLOCK/16)/57600);                      // set 57600
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)57600) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)57600))) + (float)0.5));
        break;
    case SERIAL_BAUD_115200:
        usSpeedInteger = ((MASTER_CLOCK/16)/115200);                     // set 115200
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)115200) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)115200))) + (float)0.5));
        break;
    case SERIAL_BAUD_230400:
        usSpeedInteger = ((MASTER_CLOCK/16)/230400);                     // set 230400
        ucSpeedFraction = (unsigned char)((float)((64 * ((float)(((float)MASTER_CLOCK/(float)16)/(float)230400) - (int)(float)(((float)MASTER_CLOCK/(float)16)/(float)230400))) + (float)0.5));
        break;
    }

    uart->UARTIBRD = usSpeedInteger;                                     // prepare the baud rate settings
    uart->UARTFBRD = ucSpeedFraction;

#if defined UART_EXTENDED_MODE
    if (pars->Config & MULTIDROP_MODE_TX) {                              // {15}
        uart->UARTLCRH = (UART_STICK_PARITY | UART_PARITY_ENABLE | UART_PARITY_EVEN); // stick parity '0' The 9th bits is always sent as '0' and checked for '0'
    }
    else {
#endif
        if (pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {
            if (pars->Config & RS232_ODD_PARITY) {
                uart->UARTLCRH = (UART_PARITY_ENABLE | UART_PARITY_ODD); // program odd parity
            }
            else {
                uart->UARTLCRH = (UART_PARITY_ENABLE | UART_PARITY_EVEN);// program even parity
            }
        }
        else {
            uart->UARTLCRH = UART_PARITY_DISABLED;                       // no parity
        }
#ifdef UART_EXTENDED_MODE
    }
#endif

    if (pars->Config & TWO_STOPS) {
        uart->UARTLCRH |= UART_2_STOP_BITS;                              // set 2 stop bits rather than 1 stop bit
    }

    if (pars->Config & CHAR_7) {
        uart->UARTLCRH |= UART_7_BIT_CHAR;                               // set 7 bit characters
    }
    else {
        uart->UARTLCRH |= UART_8_BIT_CHAR;                               // set 8 bit characters
    }

    #ifdef _WINDOWS
    fnConfigSimSCI(Channel, MASTER_CLOCK/(uart->UARTIBRD*16), pars);     // open a serial port on PC if desired
    #endif
    uart->UARTCTL = UARTEN;                                              // enable UART
}

// SCI0 interrupt
//
static __interrupt void SCI0_Interrupt(void)
{
    while (UARTMIS_0 != 0) {                                             // while interrupts present from UART0
        if (UARTMIS_0 & UART_RXIM) {                                     // reception interrupt pending
            _CLEAR_INTERRUPT(UARTICR, 0, UART_RXIM);                     // clear the interrupt
            fnSciRxByte((unsigned char)UARTDR_0, 0);                     // receive data interrupt - read the byte
        }

        if (UARTMIS_0 & UART_TXIM) {                                     // transmission interrupt pending
            _CLEAR_INTERRUPT(UARTICR, 0, UART_TXIM);                     // clear the interrupt
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
        }
    }
    #ifdef _WINDOWS
    UARTICR_0 = 0;                                                       // clear interrupt clear register
    #endif
}

#ifdef UART_EXTENDED_MODE
// The Luminary Micro UART doesn't inherently support multi-drop mode so we use the parity bit as extended bit
//
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel)            // {15}
{
    volatile unsigned long *ptrStatus = (UARTDR_0_ADDR + 1);
    ptrStatus += (channel * ((UART_1_BLOCK - UART_0_BLOCK)/sizeof(unsigned long))); // set to UARTRSR_ECR_x
    if (*ptrStatus & UART_PE) {                                          // check for parity error (expecting '0' for no parity)
        return 0x01;
    }
    else {
        return 0x00;
    }
}
#endif

#if CHIP_HAS_UARTS > 1
// SCI1 interrupt
//
static __interrupt void SCI1_Interrupt(void)
{
    while (UARTMIS_1 != 0) {                                             // while interrupts present from UART1
        if (UARTMIS_1 & UART_RXIM) {                                     // reception interrupt pending
            _CLEAR_INTERRUPT(UARTICR, 1, UART_RXIM);                     // clear the interrupt
            fnSciRxByte((unsigned char)UARTDR_1, 1);                     // receive data interrupt - read the byte
        }

        if (UARTMIS_1 & UART_TXIM) {                                     // transmission interrupt pending
            _CLEAR_INTERRUPT(UARTICR, 1, UART_TXIM);                     // clear the interrupt
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
        }
    }
    #ifdef _WINDOWS
    UARTICR_1 = 0;                                                       // clear interrupt clear register
    #endif
}
#endif
#if CHIP_HAS_UARTS > 2
// SCI2 interrupt
//
static __interrupt void SCI2_Interrupt(void)
{
    while (UARTMIS_2 != 0) {                                             // while interrupts present from UART2
        if (UARTMIS_2 & UART_RXIM) {                                     // reception interrupt pending
            _CLEAR_INTERRUPT(UARTICR, 2, UART_RXIM);                     // clear the interrupt
            fnSciRxByte((unsigned char)UARTDR_2, 2);                     // receive data interrupt - read the byte
        }

        if (UARTMIS_2 & UART_TXIM) {                                     // transmission interrupt pending
            _CLEAR_INTERRUPT(UARTICR, 2, UART_TXIM);                     // clear the interrupt
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
        }
    }
    #ifdef _WINDOWS
    UARTICR_2 = 0;                                                       // clear interrupt clear register
    #endif
}
#endif

static void fnConfigUARTInterrupt(QUEUE_HANDLE Channel)
{
    switch (Channel) {                                                   // enter the interrupt handler
    case 0:
        fnEnterInterrupt(irq_UART0_ID, PRIORITY_UART0, SCI0_Interrupt);  // configure and enter the handling interrupt routine in the vector table
        break;
    #if CHIP_HAS_UARTS > 1
    case 1:
        fnEnterInterrupt(irq_UART1_ID, PRIORITY_UART1, SCI1_Interrupt);; // configure and enter the handling interrupt routine in the vector table
        break;
    #endif
    #if CHIP_HAS_UARTS > 2
    case 2:
        fnEnterInterrupt(irq_UART2_ID, PRIORITY_UART2, SCI2_Interrupt);; // configure and enter the handling interrupt routine in the vector table
        break;
    #endif
    }
}

#if defined SERIAL_SUPPORT_DMA                                           // {9}

// Set up the uDMA for reception of the defined character block
//
extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
    #if defined CHIP_HAS_uDMA
    #endif
}

// Start transfer of a block via uDMA
//
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length)
{
    #if defined CHIP_HAS_uDMA
    DMA_CONTROL_STRUCTURE *ptrDmaControl;
    unsigned long ulChannel;
    if (tx_length == 0) {
        return 0;
    }
    else if (tx_length > MAX_TX_DMA) {                                   // limit to the uDMA length
        tx_length = MAX_TX_DMA;
    }
    switch (channel) {
    case 0:
        ptrDmaControl = fnGetDMA_control(DMA_UART0_TRANSMIT_CH);
        ulChannel = DMA_UART0_TRANSMIT_BIT;
            #ifdef _WINDOWS
        iDMA |= DMA_UART0_TRANSMIT_BIT;
            #endif
        break;
        #if CHIP_HAS_UARTS > 1
    case 1:
        ptrDmaControl = fnGetDMA_control(DMA_UART1_TRANSMIT_CH);
        ulChannel = DMA_UART1_TRANSMIT_BIT;
            #ifdef _WINDOWS
        iDMA |= DMA_UART1_TRANSMIT_BIT;
            #endif
        break;
        #endif
        #if CHIP_HAS_UARTS > 2
    case 2:
        ptrDmaControl = fnGetDMA_control(DMA_UART2_TRANSMIT_CH);
        ulChannel = DMA_UART2_TRANSMIT_BIT;
            #ifdef _WINDOWS
        iDMA |= DMA_UART2_TRANSMIT_BIT;
            #endif
        break;
        #endif
    default:
        return 0;
    }
    tx_length--;
    ptrDmaControl->DMASRCENDP = (ptrStart + tx_length);                  // set the last source address (inclusive)
    ptrDmaControl->DMACHCTL = ((XFERMODE_BASIC | ARBSIZE_4 | SRCSIZE_8_BIT | DSTSIZE_8_BIT | SRCINC_BYTE | DSTINC_NO_INC) | (tx_length << XFERSIZE_SHIFT));
    #ifdef _WINDOWS
    DMAENASET |= ulChannel;                                              // start the transmission
    #else
    DMAENASET = ulChannel;                                               // start the transmission
    #endif
    return (tx_length + 1);                                              // return the length of data accepted
    #else
    return 0;
    #endif
}
    #ifdef SERIAL_SUPPORT_XON_XOFF
extern QUEUE_TRANSFER fnRemainingDMA_tx(QUEUE_HANDLE channel)
{
    return 0;
}
    #endif

extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue)
{
    #if defined CHIP_HAS_uDMA
    DMA_CONTROL_STRUCTURE *ptrDmaControl;
    switch (channel) {
    case 0:
        DMAENACLR = DMA_UART0_TRANSMIT_BIT;                              // stop DMA activity on this transmission channel
        #ifdef _WINDOWS
        DMAENASET &= ~DMA_UART0_TRANSMIT_BIT;
        #endif
        ptrDmaControl = fnGetDMA_control(DMA_UART0_TRANSMIT_CH);
        break;
        #if CHIP_HAS_UARTS > 1
    case 1:
        DMAENACLR = DMA_UART1_TRANSMIT_BIT;                              // stop DMA activity on this transmission channel
        #ifdef _WINDOWS
        DMAENASET &= ~DMA_UART1_TRANSMIT_BIT;
        #endif
        ptrDmaControl = fnGetDMA_control(DMA_UART1_TRANSMIT_CH);
        break;
        #endif
        #if CHIP_HAS_UARTS > 2
    case 2:
        DMAENACLR = DMA_UART2_TRANSMIT_BIT;                              // stop DMA activity on this transmission channel
        #ifdef _WINDOWS
        DMAENASET &= ~DMA_UART2_TRANSMIT_BIT;
        #endif
        ptrDmaControl = fnGetDMA_control(DMA_UART2_TRANSMIT_CH);
        break;
        #endif
    default:
        return 0;
    }
    if ((ptrDmaControl->DMACHCTL & XFERMODE_MASK) == 0) {                // return the number of transfers still open
        return 0;
    }
    else {
        return (unsigned short)(((unsigned short)((ptrDmaControl->DMACHCTL & XFERSIZE_MASK) >> XFERSIZE_SHIFT)) + 1);
    }
    return 0;
    #else
    return 0;
    #endif
}
#endif

// Enable reception on the defined channel
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    LM3XXXXX_UART_CONTROL *uart;

#if NUMBER_EXTERNAL_SERIAL > 0                                           // {29}
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
    }
#endif
    uart = fnSelectChannel(Channel);
    fnConfigUARTInterrupt(Channel);                                      // ensure interrupt is configured
    uart->_UARTICR = UART_RXIM;                                          // clear any pending interrupts
    uart->UARTIM |= UART_RXIM;                                           // enable interrupt on reception
    uart->UARTCTL |= UARTRXEN;                                           // enable the receiver
    #ifdef _WINDOWS
    uart->_UARTICR = 0;
    #endif
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    LM3XXXXX_UART_CONTROL *uart;
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {29}
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
#endif
    uart = fnSelectChannel(Channel);
    uart->UARTIM &= ~UART_RXIM;                                          // disable interrupt on reception
    uart->UARTCTL &= ~UARTRXEN;                                          // disable the receiver
}

// Enable transmission on the defined channel
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    LM3XXXXX_UART_CONTROL *uart;
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {29}
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
#endif
    uart = fnSelectChannel(Channel);
    fnConfigUARTInterrupt(Channel);                                      // ensure interrupt is configured
    uart->_UARTICR = UART_TXIM;                                          // clear any pending interrupts
    uart->UARTIM |= UART_TXIM;                                           // enable interrupt on transmission
    uart->UARTCTL |= UARTTXEN;                                           // enable the transmitter
    #ifdef _WINDOWS
    uart->_UARTICR = 0;
    #endif
}

// Disable transmission on the defined channel
//
extern void fnTxOff(QUEUE_HANDLE Channel)
{
    LM3XXXXX_UART_CONTROL *uart;
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {29}
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
#endif
    uart = fnSelectChannel(Channel);
    uart->UARTIM &= ~UART_TXIM;                                          // disable interrupt on transmission
    uart->UARTCTL &= ~UARTTXEN;                                          // disable the transmitter
}

#if defined UART_BREAK_SUPPORT
extern void fnStartBreak(QUEUE_HANDLE channel)
{
}

extern void fnStopBreak(QUEUE_HANDLE channel)
{
}
#endif

// The TTY driver uses this call to disable the transmit interrupt of the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE channel)
{
}


// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    LM3XXXXX_UART_CONTROL *uart;

    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {29}
    if (channel >= NUMBER_SERIAL) {
        fnExtSCI_send((QUEUE_HANDLE)(channel - NUMBER_SERIAL), ucTxByte);// pass on to the external interface for transmission
        return 0;
    }
    #endif

    uart = fnSelectChannel(channel);
    if ((uart->UARTFR & UARTTXFE) == 0) {
        return 1;                                                        // presently busy
    }
    if (ucTxByte == 0x13) {
        ucTxByte = 0x13;
    }
    uart->UARTDR = ucTxByte;                                             // transmit the data

    #ifdef _WINDOWS
    uart->UARTFR &= ~(UARTTXFE);                                         // mark transmitter presently not empty
    ucTxLast[channel] = (unsigned char)uart->UARTDR;                     // back up the data written so that it can't get lost when rx data uses the simulated register
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
    }
    #endif
    return 0;                                                            // OK, we were not busy
}

#ifdef SUPPORT_HW_FLOW
static void fnSetRTS(QUEUE_HANDLE channel, int iState)                   // {6}
{
    switch (channel) {
    #if defined RTS_0_PIN
    case 0:
        if (iState != 0) {
            RTS_0_PORT &= ~RTS_0_PIN;                                    // assert RTS
        }
        else {
            RTS_0_PORT |= RTS_0_PIN;                                     // negate RTS
        }            
        break;
    #endif
    #if CHIP_HAS_UARTS > 1 && defined RTS_1_PIN                          // only if second UART is available
    case 1:
        if (iState != 0) {
            RTS_1_PORT &= ~RTS_1_PIN;                                    // assert RTS
        }
        else {
            RTS_1_PORT |= RTS_1_PIN;                                     // negate RTS
        }           
        break;
    #endif
    #if CHIP_HAS_UARTS > 2 && defined RTS_2_PIN                          // only if third UART is available
    case 2:
        if (iState != 0) {
            RTS_2_PORT &= ~RTS_2_PIN;                                    // assert RTS
        }
        else {
            RTS_2_PORT |= RTS_2_PIN;                                     // negate RTS
        }   
        break;
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {29}
    default:
        fnSetExtRTS((QUEUE_HANDLE)(channel - NUMBER_SERIAL), iState);
        break;
    #endif
    }
}


// The Luminary-Micro devices (generally) do not have RTS/CTS pins and so these are controlled by user defined ports
//
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    if (usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) {           // {6}
        if (usModifications & CONFIG_RTS_PIN) {
            switch (channel) {
        #if CHIP_HAS_UARTS > 0 && defined RTS_0_PIN
            case 0:
                RCGC2 |= RTS_0_PORT_POWER;                               // power up the port and configure as digital output
                RTS_0_PORT_ENABLE |= RTS_0_PIN;
                RTS_0_PORT_DDR |= RTS_0_PIN;  
                break;
        #endif
        #if CHIP_HAS_UARTS > 1 && defined RTS_1_PIN                      // only if second UART is available
            case 1:
                RCGC2 |= RTS_1_PORT_POWER;                               // power up the port and configure as digital output
                RTS_1_PORT_ENABLE |= RTS_1_PIN;
                RTS_1_PORT_DDR |= RTS_1_PIN; 
                break;
        #endif
        #if CHIP_HAS_UARTS > 2 && defined RTS_2_PIN                      // only if third UART is available
            case 2:
                RCGC2 |= RTS_2_PORT_POWER;                               // power up the port and configure as digital output
                RTS_2_PORT_ENABLE |= RTS_2_PIN;
                RTS_2_PORT_DDR |= RTS_2_PIN; 
                break;
        #endif
            }
        #if NUMBER_EXTERNAL_SERIAL > 0                                   // {29}
            if ((channel >= CHIP_HAS_UARTS) && (usModifications & SET_RS485_MODE)) {
                fnSetRS485Mode((QUEUE_HANDLE)(channel - CHIP_HAS_UARTS), (usModifications & SET_RS485_NEG));
            }
            else {
                fnSetRTS(channel, 0);                                    // drive the RTS line in negated state
            }
        #else
            fnSetRTS(channel, 0);                                        // drive the RTS line in negated state
        #endif
        }
        if (usModifications & CONFIG_CTS_PIN) {
            switch (channel) {
        #if defined CTS_0_PIN
            case 0:
                RCGC2 |= CTS_0_PORT_POWER;                               // power up the port and configure as digital output
                CTS_0_PORT_ENABLE |= CTS_0_PIN;
                CTS_0_PORT_DDR &= ~CTS_0_PIN;
                break;
        #endif
        #if CHIP_HAS_UARTS > 1 && defined CTS_1_PIN                      // only if second UART is available
            case 1:
                RCGC2 |= CTS_1_PORT_POWER;                               // power up the port and configure as digital output
                CTS_1_PORT_ENABLE |= CTS_1_PIN;
                CTS_1_PORT_DDR &= ~CTS_1_PIN;
                break;
        #endif
        #if CHIP_HAS_UARTS > 2 && defined CTS_2_PIN                      // only if third UART is available
            case 2:
                RCGC2 |= CTS_2_PORT_POWER;                               // power up the port and configure as digital output
                CTS_2_PORT_ENABLE |= CTS_2_PIN;
                CTS_2_PORT_DDR &= ~CTS_2_PIN;
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
};

static const unsigned char cts_irq_priority[CHIP_HAS_UARTS] = {
#if defined CTS_0_PIN
    CTS_0_INT_PRIORITY, 
#else
    0,
#endif
#if CHIP_HAS_UARTS > 1
    #if defined CTS_1_PIN
    CTS_1_INT_PRIORITY, 
    #else
    0,
    #endif
#endif
#if CHIP_HAS_UARTS > 2
    #if defined CTS_2_PIN
    CTS_2_INT_PRIORITY, 
    #else
    0,
    #endif
#endif
};

static const unsigned char cts_port_bit[CHIP_HAS_UARTS] = {
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
};

static void cts0_change(void)
{
    #if defined CTS_0_PIN
    LM3XXXXX_PORT_CONTROL *port = fnGetPortBlock(cts_port[0]);
    fnRTS_change(0, ((port->GPIODATA & cts_port_bit[0]) == 0));         // control transmission according to state of CTS line
    #endif
}

#if CHIP_HAS_UARTS > 1
static void cts1_change(void)
{
    #if defined CTS_1_PIN
    LM3XXXXX_PORT_CONTROL *port = fnGetPortBlock(cts_port[1]);
    fnRTS_change(1, ((port->GPIODATA & cts_port_bit[1]) == 0));         // control transmission according to state of CTS line
    #endif
}
#endif
#if CHIP_HAS_UARTS > 2
static void cts2_change(void)
{
    #if defined CTS_2_PIN
    LM3XXXXX_PORT_CONTROL *port = fnGetPortBlock(cts_port[2]);
    fnRTS_change(2, ((port->GPIODATA & cts_port_bit[2]) == 0));         // control transmission according to state of CTS line
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
};


// Modify control line interrupt operation
//
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    QUEUE_TRANSFER ControlLines = 0;
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {29}
    if (channel >= NUMBER_SERIAL) {
        return (fnControlExtLineInterrupt((QUEUE_HANDLE)(channel - NUMBER_SERIAL), usModifications, OperatingMode));
    }
    #endif
    if (usModifications & (ENABLE_CTS_CHANGE | DISABLE_CTS_CHANGE)) {    // {11}
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
        interrupt_setup.int_port_bit = cts_port_bit[channel];            // the input connected
        interrupt_setup.int_port_characteristic = PULLUP_ON;             // enable pull-up resistor at input
        interrupt_setup.int_port_sense = IRQ_BOTH_EDGES;                 // interrupt on both edges
        fnConfigureInterrupt(&interrupt_setup);                          // configure test interrupt
    }
    if (RCGC2 & (0x1 << cts_port[channel])) {                            // only read when the port is powered
        LM3XXXXX_PORT_CONTROL *port = fnGetPortBlock(cts_port[channel]);
        if ((port != 0) && (!(port->GPIODATA & cts_port_bit[channel]))) {
            ControlLines |= SET_CTS;                                     // CTS line is active
        }
    }
    return ControlLines;
}
#endif
#endif                                                                   // end serial UART support


/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */

#ifdef FLASH_FILE_SYSTEM

// This routine to command FLASH - we put it in SRAM and run it with disabled interrupts
//
static void fnCommandFlash(unsigned long ulCommand, unsigned long ptrAdd, unsigned long ulValue)
{
    FMD = ulValue;                                                       // data value
    FMA = ptrAdd;                                                        // address in flash
    FMC = (WRKEY | ulCommand);                                           // command
        #ifdef _WINDOWS
    switch (FMC & 0x0000000f) {
    case FLASH_WRITE:
        if ((~(*(unsigned long *)FMA)) & ulValue) {
            *(unsigned char *)0 = 0;                                     // signal attempt to program bits from 0 to 1
        }
        *(unsigned long *)FMA = FMD;
        break;

    case FLASH_ERASE:
        fnDeleteFlashSector(fnGetFlashAdd((unsigned char *)ptrAdd));      // simulate deletion
        break;
    }
    FMC &= ~0x0000000f;
        #endif
    while (FMC & ulCommand) {}                                           // wait until the command has been executed (about 50us)
}

// Initialise flash and routines in RAM ready for later use
//
static void fnInitFlash(void)
{
    #ifdef _RUN_FLASH_FROM_RAM
    #define PROG_WORD_SIZE 100                                           // adequate space for the small program
    static unsigned short usProgSpace[PROG_WORD_SIZE];                   // make space for the routine on stack (this will have an even boundary)
    unsigned char *ptrThumb2 = (unsigned char *)fnCommandFlash;
    int i = 0;

    ptrThumb2--;                                                         // thumb 2 address
    while (i < PROG_WORD_SIZE) {                                         // copy program to SRAM
        usProgSpace[i++] = *(unsigned short *)ptrThumb2;
        ptrThumb2 += sizeof (unsigned short);
    }
    ptrThumb2 = (unsigned char *)usProgSpace;
    ptrThumb2++;                                                         // create a thumb 2 call
    fnRAM_code = (void(*)(unsigned long, unsigned long, unsigned long))(ptrThumb2);
    #endif

    USECRL = ((MASTER_CLOCK/1000000)-1);                                 // set FLASH programming usec value to suit system clock rate
}
#endif

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD) // {26}
// Configure the SPI interface for file system use
//
static void fnConfigSPIFileSystem(unsigned char ucSpeed)
{
    _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT4 | PORTA_BIT5), (PA2_SSI0Clk | PA4_SSI0Rx | PA5_SSI0Tx)); // use SSIRX,SSITX,SSICLK
             
    CONFIGURE_CS_LINES();                                                // configure chip select outputs
 
    SSICR1_0 = 0;                                                        // ensure disabled (master mode)
    SSICPSR_0 = 2;                                                       // set clock prescaler (even prescale 2..254)

    switch (ucSpeed) {
    case MAX_SPI:                                                        // set maximum possible SPI speed
        SSICR0_0 = (FRS_FREESCALE_SPI | DSS_8_BIT | (1 << SSI_SCR_SHIFT)); // set exact frequency and mode
        break;

    case SPI_2MEG:
        SSICR0_0 = (FRS_FREESCALE_SPI | DSS_8_BIT | ((((MASTER_CLOCK/2 + 2000000/2)/2000000) - 1) << SSI_SCR_SHIFT)); // set exact frequency and mode
        break;

    case SPI_SLAVE:
        return;
    }
    SSICR1_0 = SSI_SSE;                                                  // enable
}
#endif

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_lm3s_atmel.h"
        #include "spi_flash_lm3s_stmicro.h"
        #include "spi_flash_lm3s_sst25.h"                                // {3}
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
        #ifdef CS2_LINE
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;                                                        // address is in the second SPI device
    }
            #ifdef CS3_LINE
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

    #ifndef SPI_FLASH_SST25                                              // {3}
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

        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
        #endif

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
    #endif                                                               // end not SST25
#endif


#ifdef ACTIVE_FILE_SYSTEM
    #if defined SPI_FLASH_SST25                                          // {3}
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

extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
    #if defined FLASH_FILE_SYSTEM
    unsigned long ulPage = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // ensure page boundary
    ulPage -= FLASH_START_ADDRESS;
    #endif
    #ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
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
            #ifdef SPI_FLASH_SST25                                       // {3}
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
            #ifdef SPI_FLASH_ST
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
            #elif defined SPI_FLASH_SST25                                // {3}
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
                #ifndef SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_SECTOR_LENGTH - 1)) == 0)) { // if a complete 64k sector can be deleted
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
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #endif
            continue;
        }
        #endif

        WDTICR = 0;                                                       // {2} re-trigger the watchdog during this loop since a page erase requires at least 20ms. 156k erase can thus take over 6s!

        #ifdef _RUN_FLASH_FROM_RAM
        uDisable_Interrupt();                                             // protect the call from interrupts
        _fnRAM_code(FLASH_ERASE, ulPage, 0);                              // command page erase
        uEnable_Interrupt();                                              // enable interrupts again
        #else
        fnCommandFlash(FLASH_ERASE, ulPage, 0);                           // command page erase
        #endif
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        ulPage += FLASH_GRANULARITY;
        Length -= FLASH_GRANULARITY;
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {27}
        ptrSector += FLASH_GRANULARITY;
        #endif
    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);

    #ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
    #endif
    return 0;
}

// Write a buffer to FLASH
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    #if defined FLASH_FILE_SYSTEM && defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    static unsigned char *ptrOpenBuffer = 0;
    unsigned long ulBufferOffset;
    unsigned char *ptrFlashBuffer;
    MAX_FILE_LENGTH BufferCopyLength;
    int iRtn = 0;
    #else
    unsigned long *ptrLong;                                              // {25}
    unsigned long ulValue;
    int iByteInLong;
    #endif
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
    unsigned short usPageLength;
    while (Length) {
        usPageLength = ((unsigned short)ucDestination) & (EEPROM_PAGE_SIZE-1); // calculate the page length based on EEPROM page boundaries
        usPageLength = EEPROM_PAGE_SIZE - usPageLength;
        if (Length < usPageLength) {
            usPageLength = (unsigned short)Length;
        }
        fnWaitWriteComplete();
        #if defined SPI_INTERFACE
        fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);         // write a page
        #else
        fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);         // write a page
        #endif
        Length -= usPageLength;
        ucData += usPageLength;
        ucDestination += usPageLength;
    }
    return 0;
    #else                                                                // we have already checked that it is possible to modify the byte requested but we have to be sure that we only write 32 bits to long word addresses
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))  // we are working from external SPI FLASH memory
        #endif
    {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        #if defined SPI_FLASH_SST25                                      // {3}
        int iMultipleWrites = 0;
        #endif
        unsigned short usDataLength;
        #if defined SPI_FLASH_SST25                                      // {3}
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
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
        #endif
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
        #ifdef SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);        // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #elif defined SPI_FLASH_SST25                                    // {3}
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
        #if defined SPI_FLASH_SST25                                      // {3}
        fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);              // disable writes
        #endif
        return 0;
    }
    #endif

    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
        #ifdef FLASH_FILE_SYSTEM
    if (ucData == 0) {                                                   // close an open buffer
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & (FLASH_ROW_SIZE-1));
        if (ulBufferOffset == 0) {
            return 0;                                                    // no open buffer so nothing to do
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
            ptrFlashBuffer = (unsigned char *)ulFlashRow;                // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            #ifdef _RUN_FLASH_FROM_RAM
            uDisable_Interrupt();                                        // protect call from interrupts
            _fnRAM_code(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrOpenBuffer++, ulValue); // program the long word
            uEnable_Interrupt();                                         // re-enable interrupts
            #else
            fnCommandFlash(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)fnGetFlashAdd(ptrOpenBuffer), ulFlashRow[0]);// program the long word
            #endif
            ptrOpenBuffer += FLASH_ROW_SIZE;
            uMemset(ulFlashRow, 0xff, FLASH_ROW_SIZE);                   // flush the intermediate buffer
        }
        else {                                                           // incomplete buffer collected
            ptrOpenBuffer += BufferCopyLength;
        }
    } while (Length != 0);
        #endif
    return iRtn;
    #else
        #ifdef _WINDOWS
    ptrLong = (unsigned long *)((unsigned long)((unsigned long)ucDestination + (unsigned long)ucFLASH - FLASH_START_ADDRESS) & ~0x3);  // long address starting on
    iByteInLong = (ucDestination - FLASH_START_ADDRESS + (unsigned long)ucFLASH) - (unsigned char *)ptrLong;
    ulValue = *ptrLong;                                                  // present value at location
        #else
    ptrLong = (unsigned long *)((unsigned long)ucDestination & ~0x3);    // long address starting on
    iByteInLong = ucDestination - (unsigned char *)ptrLong;              // long word align
    ulValue = 0xffffffff;                                                // deleted state
        #endif

    while (Length--) {
        #if defined (_LITTLE_ENDIAN) || defined (_WINDOWS)
        ulValue &= ~(0xff << (iByteInLong*8));                           // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong*8));                       // insert new byte
        #else
        ulValue &= ~(0xff << ((3 - iByteInLong)*8));                     // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong)*8));                 // insert new byte
        #endif
        if (iByteInLong == 3) {                                          // can we write a long?
        #ifdef _RUN_FLASH_FROM_RAM
            uDisable_Interrupt();                                        // protect call from interrupts
            _fnRAM_code(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong++, ulValue); // program the long word
            uEnable_Interrupt();                                         // re-enable interrupts
        #else
            fnCommandFlash(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong++, ulValue);// program the long word
        #endif
            if (!Length) {
                return 0;                                                // completed
            }
        #ifdef _WINDOWS
            ulValue = *ptrLong;
        #else
            ulValue = 0xffffffff;                                        // deleted state
        #endif
            iByteInLong = 0;
        }
        else {
            iByteInLong++;
        }
    }
        #ifdef _RUN_FLASH_FROM_RAM
    uDisable_Interrupt();                                                // protect call from interrupts
    _fnRAM_code(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong, ulValue); // program the long word
    uEnable_Interrupt();                                                 // re-enable interrupts
        #else
    fnCommandFlash(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong, ulValue); // program the long word
        #endif
    return 0;
    #endif
#endif
}
#endif

#if (defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES) && defined _WINDOWS
    extern int iFetchingInternalMemory = 0;
#endif

// Retrieve file contents from FLASH
//
extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#if defined ACTIVE_FILE_SYSTEM
    #if defined _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        iFetchingInternalMemory = 0;
        return;
    }
    #endif
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    unsigned char usRead[4];                                             // our file sytem is in external EEPROM via SPI bus.
    usRead[0] = M95XXX_READ_MEMORY;
    usRead[1] = (unsigned char)((unsigned short)(ParLocation)>> 8);
    usRead[2] = (unsigned char)((unsigned short)ParLocation);
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
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory
        #endif
    {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        #ifdef SPI_FLASH_SST25                                           // {3}
        ParLocation = fnGetSPI_FLASH_address(ParLocation EXTENDED_CS);
        #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
        #endif
        #ifdef SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #elif defined SPI_FLASH_SST25                                    // {3}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, _EXTENDED_CS ptrValue, Size);
        #else
            #if SPI_FLASH_PAGE_LENGTH >= 1024                            // {28}
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
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the LM3X uses a file system in FLASH with no access restrictions so we can simply copy the data
    #endif
#endif
}



#ifdef USE_PARAMETER_BLOCK

#ifdef _TEMPEST_CLASS
    #define PARAMETER_BLOCK_1 (unsigned long *)(PARAMETER_BLOCK_START)
    #define PARAMETER_BLOCK_2 (unsigned long *)(PARAMETER_BLOCK_START + PARAMETER_BLOCK_SIZE)
#else
    #define PARAMETER_BLOCK_1 (unsigned char *)(PARAMETER_BLOCK_START)
    #define PARAMETER_BLOCK_2 (unsigned char *)(PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)
#endif

#ifdef _TEMPEST_CLASS
// The LM3S9BXX stores each byte in a long word
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
#else
// The LM3SXXXX has 1k byte blocks which can be individually modified so we use one or more of these, and a backup if desired
//
extern unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned char ucValidUse[2];
    unsigned char ucCompare;

    // The first 2 byte locations define the validity of the block. 0x55 0x55 means it is fully valid. 0x55 0xff means it contains temporary unvalidated data
    fnGetParsFile(PARAMETER_BLOCK_1, ucValidUse, sizeof(ucValidUse));

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ucCompare = 0x55;                                                // we are looking for validated pattern
    }
    else {
        ucCompare = 0xff;                                                // we are looking for temporary pattern
    }

    if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
        return 1;                                                        // first block matched request
    }
    #ifdef USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile(PARAMETER_BLOCK_2, (unsigned char*)ucValidUse, sizeof(ucValidUse));
        if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
            return 2;                                                    // second block matched request
        }
    }
    #endif

    return 0;                                                            // all other cases indicate the searched block doesn't exist
}
#endif

// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(unsigned char *ptrAddInPar)
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}

extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
#ifdef _TEMPEST_CLASS
    unsigned long *ptrPar = PARAMETER_BLOCK_1;
#else
    unsigned char *ptrPar = PARAMETER_BLOCK_1;
#endif

    #ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    #endif

    ptrPar += (usParameterReference);
    ptrPar += 2;                                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile((unsigned char *)ptrPar, ucValue++, 1);
        ptrPar++;
    }
    return 0;
}

// Write parameters to the defined block
//
#ifdef _TEMPEST_CLASS
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
#else
extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned char ucValid = 0x55;
    int iBlockUse = 0;

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {                                  // no valid parameter blocks have been found so we can use the first for saving the data.
        fnDeleteParBlock(PARAMETER_BLOCK_1);                             // we delete it to be sure it is fresh
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
            fnDeleteParBlock(ptrPar);
    #endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock(ptrStart);
            if (usParameterReference) return 0;                          // we don't validate it again
        }
    }
    else {
        fnWriteBytesFlash(ptrPar, ucValue, usLength);                    // the LM3SXXXX can do accumulative writes so we can simply call the normal write function
    }
                                                                         // now we must validate the parameter block which we have just written to
    fnWriteBytesFlash(ptrStart, &ucValid, sizeof(ucValid));              // validate data
    if (!iBlockUse) {
        fnWriteBytesFlash(ptrStart+sizeof(ucValid), &ucValid, sizeof(ucValid)); // use data
    }
    return 0;
}
#endif

#endif




#if defined I2C_INTERFACE
/* =================================================================== */
/*                            I2C Interface                            */
/* =================================================================== */

// Collect a pointer to defined I2C control block
//
static LM3XXXXX_I2C_CONTROL *fnSelectI2CChannel(QUEUE_HANDLE Channel)
{
    #if CHIP_HAS_I2C > 1
    switch (Channel) {
    case 0:
        return (LM3XXXXX_I2C_CONTROL *)I2C0_BLOCK;
    case 1:
        return (LM3XXXXX_I2C_CONTROL *)I2C1_BLOCK;
    }
    return 0;                                                            // invalid
    #else
    return (LM3XXXXX_I2C_CONTROL *)I2C0_BLOCK;
    #endif
}

static void fnI2CInt(QUEUE_HANDLE Channel, LM3XXXXX_I2C_CONTROL *iic, I2CQue *ptI2CQue)
{
    if (ptI2CQue->ucState & RX_ACTIVE) {                                 // receiving
        *I2C_rx_control[Channel]->I2C_queue.put++ = (unsigned char)iic->I2CMDR; // {12} read the received byte (not dependent on ptI2CQue->ucPresentLen)
        I2C_rx_control[Channel]->I2C_queue.chars++;                      // and put it into the rx buffer
        if (I2C_rx_control[Channel]->I2C_queue.put >= I2C_rx_control[Channel]->I2C_queue.buffer_end) {
            I2C_rx_control[Channel]->I2C_queue.put = I2C_rx_control[Channel]->I2C_queue.QUEbuffer;
        }
    #ifdef _WINDOWS
        if (ptI2CQue->ucPresentLen != 0) {
            iic->I2CMDR = fnSimI2C_devices(I2C_RX_DATA, 0);              // simulate the interrupt directly
        }
        iic->I2CMCS =(I2C_DATACK);                                       // simulate the interrupt directly
        iic->I2CMMIS = iic->I2CMRIS = I2C_INTERRUPT_BIT;                 // simulate the interrupt mask setting
        iInts |= (I2C_INT0 << Channel);
    #endif

        if (ptI2CQue->ucPresentLen) {                                    // more to be received
            ptI2CQue->ucPresentLen--;
            if (ptI2CQue->ucPresentLen == 0) {                           // last read
                iic->I2CMCS = (I2C_RUN | I2C_STOP);                      // start transfer of last byte with no ack, followed by a stop condition
            }
            else {
                iic->I2CMCS = (I2C_RUN | I2C_DATACK);                    // start transfer of further acked byte
            }
        }
        else {
            I2C_rx_control[Channel]->msgs++;                             // a reception message ready
            if (I2C_rx_control[Channel]->wake_task) {                    // if a task is defined to receive this reception transfer
                uDisable_Interrupt();                                    // ensure interrupts remain blocked during subroutines
                uTaskerStateChange(I2C_rx_control[Channel]->wake_task, UTASKER_ACTIVATE);// wake up owner task since a reception message is ready
                uEnable_Interrupt();                                     // release
            }
            if (!(ptI2CQue->I2C_queue.chars)) {                          // all transmission complete
                iic->I2CMIMR = 0;                                        // disable further interrupts
                ptI2CQue->ucState = 0;
            }
            else {
                fnTxI2C(ptI2CQue, Channel);                              // we have another message to send so we can send a repeated start condition
            }
        }
        return;
    }

    if (ptI2CQue->ucPresentLen) {                                        // send next byte if available
        ptI2CQue->ucPresentLen--;
        iic->I2CMDR = *ptI2CQue->I2C_queue.get++;                        // prepare next byte
        if (ptI2CQue->ucPresentLen == 0) {                               // last byte
            iic->I2CMCS = (I2C_RUN | I2C_STOP);                          // send byte followed by stop condition
        }
        else {
            iic->I2CMCS = (I2C_RUN);                                     // send byte without start/stop
        }
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle the ring buffer
        }
    #ifdef _WINDOWS
        iic->I2CMCS =(I2C_DATACK);                                       // simulate the interrupt directly
        iic->I2CMMIS = iic->I2CMRIS = I2C_INTERRUPT_BIT;                 // simulate the interrupt mask setting
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)iic->I2CMDR);
        iInts |= (I2C_INT0 << Channel);
    #endif
    }
    else {
        if (0 == ptI2CQue->I2C_queue.chars) {                            // all transmission complete
            iic->I2CMIMR = 0;                                            // disable further interrupts
            ptI2CQue->ucState = 0;
            if (ptI2CQue->wake_task ) {
                uDisable_Interrupt();                                    // ensure interrupts remain blocked during subroutines
                uTaskerStateChange(ptI2CQue->wake_task, UTASKER_ACTIVATE);// wake up owner task since all transmission has terminated
                uEnable_Interrupt();                                     // release
            }
        }
        else {
            fnTxI2C(ptI2CQue, Channel);                                  // we have another message to send so we can send a repeated start condition
        }
    }
}

static __interrupt void I2C_Interrupt_0(void)
{
    I2CMICR_0 = I2C_INTERRUPT_BIT;                                       // clear pending interrupt
    #ifdef _WINDOWS
    I2CMMIS_0 = I2CMRIS_0 = 0;
    #endif
    fnI2CInt(0, (LM3XXXXX_I2C_CONTROL *)I2C0_BLOCK, I2C_tx_control[0]);
}

    #if CHIP_HAS_I2C > 1
static __interrupt void I2C_Interrupt_1(void)
{
    I2CMICR_1 = I2C_INTERRUPT_BIT;                                       // clear pending interrupt
        #ifdef _WINDOWS
    I2CMMIS_1 = I2CMRIS_1 = 0;
        #endif
    fnI2CInt(1, (LM3XXXXX_I2C_CONTROL *)I2C1_BLOCK, I2C_tx_control[1]);
}
    #endif

// Configure the I2C hardware
//
void fnConfigI2C(I2CTABLE *pars)
{
    LM3XXXXX_I2C_CONTROL *iic = fnSelectI2CChannel(pars->Channel);
    #if CHIP_HAS_I2C > 1
    if (pars->Channel == 0)
    #endif
    {
        _CONFIG_PERIPHERAL(B, I2C0, 1, (PORTB_BIT2 | PORTB_BIT3), (PB2_I2C0SCL | PB3_I2C0SDA));

        GPIOODR_B |= (PORTB_BIT2 | PORTB_BIT3);                          // select open drain function
        I2CMICR_0 = I2C_INTERRUPT_BIT;                                   // clear pending interrupts
        fnEnterInterrupt(irq_I2C0_ID, PRIORITY_I2C0, I2C_Interrupt_0);
    }
    #if CHIP_HAS_I2C > 1
    else {
        _CONFIG_PERIPHERAL(A, I2C1, 1, (PORTA_BIT6 | PORTA_BIT7), (PA6_I2C1SCL | PA7_I2C1SDA));

        GPIOODR_A |= (PORTA_BIT6 | PORTA_BIT7);                          // select open drain function
        I2CMICR_1 = I2C_INTERRUPT_BIT;                                   // clear pending interrupts
        fnEnterInterrupt(irq_I2C1_ID, PRIORITY_I2C1, I2C_Interrupt_1);
    }
    #endif

    iic->I2CMCR = I2C_MFE;                                               // enable master mode of operation
    switch (pars->usSpeed) {
    case 400:                                                            // high speed I2C
        iic->I2CMTPR = ((MASTER_CLOCK / (2 * (SCL_LP + SCL_HP) * 400000)) - 1);
        break;
    case 100:
    default:                                                             // default to 100kHz
        iic->I2CMTPR = ((MASTER_CLOCK / (2 * (SCL_LP + SCL_HP) * 100000)) - 1);
        break;
    case 50:
        iic->I2CMTPR = ((MASTER_CLOCK / (2 * (SCL_LP + SCL_HP) * 50000)) - 1);
        break;
    }
    #if defined _WINDOWS
    fnConfigSimI2C(pars->Channel, (pars->usSpeed * 1000));
    #endif
}

// Send a first byte to I2C bus
//
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    LM3XXXXX_I2C_CONTROL *iic = fnSelectI2CChannel(Channel);

    ptI2CQue->ucPresentLen = *ptI2CQue->I2C_queue.get++;                 // get present length
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }
    iic->I2CMSA = *ptI2CQue->I2C_queue.get++;                            // the slave address
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }
    if (iic->I2CMSA & 0x01) {                                            // reading
        if (--ptI2CQue->ucPresentLen == 0) {                             // {12}
            iic->I2CMCS = (I2C_RUN | I2C_START | I2C_STOP);              // start transfer of single byte followed by a stop condition
        }
        else {
            iic->I2CMCS = (I2C_RUN | I2C_START | I2C_DATACK);            // start transfer of first of multiple bytes with acks
        }
        iic->I2CMIMR = I2C_INTERRUPT_BIT;                                // {12} enable interrupt on completion
    #ifdef _WINDOWS
        fnSimI2C_devices(I2C_ADDRESS, (unsigned char)iic->I2CMSA);
    #endif
        ptI2CQue->ucState = (RX_ACTIVE | TX_ACTIVE);                     // flag active receiving
        ptI2CQue->I2C_queue.chars -= 3;
        I2C_rx_control[Channel]->wake_task = *ptI2CQue->I2C_queue.get++; // enter task to be woken when reception has completed
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
    #ifdef _WINDOWS
        iic->I2CMCS =(I2C_ADRACK | I2C_DATACK);                          // simulate the interrupt directly
        iic->I2CMDR = fnSimI2C_devices(I2C_RX_DATA, 0);
        iInts |= (I2C_INT0 << Channel);
    #endif
    }
    else {                                                               // writing
        iic->I2CMDR = *ptI2CQue->I2C_queue.get++;                        // send first data byte
        if (ptI2CQue->ucPresentLen == 1) {
            iic->I2CMCS = (I2C_RUN | I2C_START | I2C_STOP);              // start transfer of single byte followed by a stop condition
        }
        else {
            iic->I2CMCS = (I2C_RUN | I2C_START);                         // start transfer of first of multiple bytes
        }
        iic->I2CMIMR = I2C_INTERRUPT_BIT;                                // enable interrupt on completion
    #ifdef _WINDOWS
        iic->I2CMMIS = iic->I2CMRIS = I2C_INTERRUPT_BIT;                 // simulate the interrupt mask setting
        fnSimI2C_devices(I2C_ADDRESS, (unsigned char)iic->I2CMSA);
    #endif
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
        ptI2CQue->ucState = (TX_ACTIVE);                                 // flag active transmitting
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->ucPresentLen+1);
        ptI2CQue->ucPresentLen--;
    #ifdef _WINDOWS
        iic->I2CMCS =(I2C_ADRACK | I2C_DATACK);                          // simulate the interrupt directly
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)iic->I2CMDR);
        iInts |= (I2C_INT0 << Channel);
    #endif
    }
}
#endif


#if defined USB_INTERFACE

// This routine is called from the USB driver with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    return USB_BUFFER_NO_DATA;                                           // report no more data so that the driver doesn't call again. The interrupt routine will handle all waiting buffers itself.
}

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *ptrUSB_HW, unsigned char ucFrameType)
{
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting messages to queue
    switch (fnUSB_handle_frame(ucFrameType, (unsigned char *)ptrUSB_HW->ptrFIFO, iEndpoint_ref, ptrUSB_HW)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
      //CLEAR_RXRDY();
        FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref);
        if (iEndpoint_ref == 0) {
            ptrUSB_HW->usEP0State = USB_STATE_ADDRESSING;                // expect ACK
        }
        break;
    case BUFFER_CONSUMED_EXPECT_MORE:                                    // control endpoint expects extra OUT frames (only on endpoint 0)
        PARTIAL_CLEAR_RXRDY();
        ptrUSB_HW->usEP0State = USB_STATE_OUT;
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        uEnable_Interrupt();                                             // release
        return MAINTAIN_OWNERSHIP;
    case STALL_ENDPOINT:                                                 // send stall (only on endpoint 0)
      //CLEAR_RXRDY();
        USBCSRL0 = EP0_STALL;                                               
        ptrUSB_HW->usEP0State = USB_STATE_STALL;
        fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);
        _SIM_USB(USB_SIM_STALL, iEndpoint_ref, ptrUSB_HW);
        break;
    default:
      //CLEAR_RXRDY();
        break;
    }
    uEnable_Interrupt();                                                 // release
    return 0;
}

// Copy the data to be transmitted to the FIFO. This may only be called when there is a buffer to accept the data.
//
extern void fnSendUSB_FIFO(unsigned char *ptrData, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW)
{
    unsigned short usPacketLength = usLength;
#ifdef _WINDOWS    
    volatile unsigned char *ptrFIFO_buffer = (unsigned char *)ptrUSB_HW->ptrFIFO;
#endif
    if ((iEndpoint == 0) && (USBCSRL0 & EP0_DEVICE_RXRDY)) {
        CLEAR_RXRDY();
    }
#ifdef _WINDOWS
    fnPutUSB_FIFOLength(iEndpoint, usLength);
    while (usLength--) {
        *ptrFIFO_buffer++ = *ptrData++;                                  // write each byte to the tx FIFO
    }
#else
    while (usLength--) {
        *ptrUSB_HW->ptrFIFO = *ptrData++;                                // write each byte to the tx FIFO
    }
#endif
    if (iEndpoint == 0) {
        if (usPacketLength == ENDPOINT_0_SIZE) {                         // this is not the last packet
            USBCSRL0 = EP0_DEVICE_TXRDY;
            usb_hardware.usEP0State = USB_STATE_TX;                      // sending multiple packets
        }
        else {
            USBCSRL0 = (EP0_DEVICE_TXRDY | EP0_DATAEND);                 // this is the last packet
            usb_hardware.usEP0State = USB_STATE_LAST_TX;                 // sending last packet
        }
    }
    else {
        if (usPacketLength == ptrUSB_HW->ptrEndpoint->usEndpointSize) {  // this is not the last packet
            *ptrUSB_HW->ptr_ucUSB_BDControl = DEVICE_TXRDY;              // sending multiple packets
        }
        else {
            *ptrUSB_HW->ptr_ucUSB_BDControl = DEVICE_TXRDY;              // this is the last packet
        }
    }

#ifdef _WINDOWS
    fnFIFO_buffer_out(iEndpoint);                                        // simulate double buffer operation
#endif
}

/****************************** INT TEST BUFFER ********************/
int iTest = 0;
unsigned char ucInts[1000];

#define DEBUG_USB_INT(x)   ucInts[iTest++] = x
#define DEBUG_USB_HW(x)    ucInts[iTest++] = x
#define DEBUG_USB_EP0(x)   ucInts[iTest++] = x
#define DEBUG_USB_EP123(x) ucInts[iTest++] = x
/*******************************************************************/


// USB OTG Interrupt handler
//
static __interrupt void usb_otg_isr(void)
{
    unsigned short usUSB_Int_status; 
DEBUG_USB_INT('I');
    while (1) {
        if ((usUSB_Int_status = (unsigned char)(USBIS & USBIE)) != 0) {  // read present status (clearing interrupt flags)
    #ifdef _WINDOWS
            USBIS = 0;
    #endif
DEBUG_USB_HW('H');
            if (usUSB_Int_status & RESET_INT) {                          // reset detected - D+ and D- in SE0 (single ended logic 0) state for > 2.5us
                usFIF0address = 64;                                      // only endpoint 0 fifo exists after a reset
                USBIE = (RESET_INT);                                     // disable resume and suspend interrupts
                uDisable_Interrupt();                                    // ensure message can not be interrupted
                fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware);// generic handler routine
                uEnable_Interrupt();                                     // release
                usb_hardware.ucUSBAddress = 0;                           // reset the address to revert back to the default state
                usb_hardware.usEP0State = 0;
DEBUG_USB_HW('R');
            }
            if (usUSB_Int_status & SUSPEND_INT) {                        // supend state detected - 3ms of idle USB bus detected (low speed D+ = 0, D- = 1 / high speed D+ = 1, D- = 0)
                USBIE = (RESET_INT | RESUME_INT);                        // disable suspend and enable resume interrupt
                uDisable_Interrupt();                                    // ensure message can not be interrupted
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic handler routine
                uEnable_Interrupt();                                     // release
DEBUG_USB_HW('S');
            }
            if (usUSB_Int_status & RESUME_INT) {                         // resume detected - 10ms inverted idle USB bus state (low speed D+ = 1, D- = 0 / high speed D+ = 0, D- = 1)
                USBIE = (RESET_INT | SUSPEND_INT);                       // disable resume and enable suspend interrupt
                uDisable_Interrupt();                                    // ensure message can not be interrupted
                fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);        // generic handler routine
                uEnable_Interrupt();                                     // release
DEBUG_USB_HW('r');
            }
        }
        else if (((usUSB_Int_status = USBTXIS) & USBTXIE) != 0) {        // transmit (or endpoint 0 rx) interrupt(s) [read clears flags]
    #ifdef _WINDOWS
            USBTXIS = 0;
    #endif
            if (usUSB_Int_status & EP0_TX_RX_INT) {                      // handle endpoint 0
DEBUG_USB_EP0('0');
DEBUG_USB_EP0((unsigned char)(usUSB_Int_status >> 8));
DEBUG_USB_EP0((unsigned char)(usUSB_Int_status));
DEBUG_USB_EP0(USBCSRL0);
DEBUG_USB_EP0(usb_hardware.usEP0State);
                if (usb_hardware.usEP0State & (USB_STATE_TX | USB_STATE_LAST_TX)) {
                    if (USBCSRL0 & EP0_SETEND) {                         // host has prematurely terminated a transfer
                        USBCSRL0 = EP0_SETENDC;                          // reset the flag
    #ifdef _WINDOWS
                        USBCSRL0 = 0;
    #endif
                        usb_hardware.usEP0State = USB_STATE_IDLE;
DEBUG_USB_EP0('X');
                        continue;
                    }
                }
                switch (usb_hardware.usEP0State) {
                case USB_STATE_IDLE:                                     // endpoint 0 is idle and waiting for a SETUP token
                case USB_STATE_OUT:
                    if (USBCSRL0 & EP0_DEVICE_RXRDY) {
DEBUG_USB_EP0('C');
                        if ((usb_hardware.usLength = USBCOUNT0) != 0) {
DEBUG_USB_EP0((unsigned char)(usb_hardware.usLength + '0'));
                            usb_hardware.ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO0_ADD, 0);
                            usb_hardware.ptr_ucUSB_BDControl = USBTXCTRADD0;
                            usb_hardware.ptrEndpoint = &usb_endpoints[0];
                            if (usb_hardware.usEP0State == USB_STATE_OUT) { // expecting further data
                                fnProcessInput(0, &usb_hardware, USB_CONTROL_OUT_FRAME);
                            }
                            else {
                                fnProcessInput(0, &usb_hardware, USB_SETUP_FRAME);
                            }
                        }
                        else {
DEBUG_USB_EP0('0');
                        }
                    }
                    break;
                case USB_STATE_TX:                                       // endpoint 0 is sending multiple packets. Interrupts are interpreted as ACKs
DEBUG_USB_EP0('A');
                    usb_hardware.ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO0_ADD, 0);
                    usb_hardware.ptr_ucUSB_BDControl = USBTXCTRADD0;
                    usb_hardware.ptrEndpoint = &usb_endpoints[0];
                    uDisable_Interrupt();                                // ensure message can not be interrupted
                    fnUSB_handle_frame(USB_TX_ACKED, 0, 0, &usb_hardware);// handle ack event
                    uEnable_Interrupt();                                 // release
                    break;
                case USB_STATE_RX:                                       // endpoint 0 has received a SETUP token and is now expecting fouther OUT data
DEBUG_USB_EP0('R');
                    usb_hardware.usLength = USBCOUNT0;
DEBUG_USB_EP0((unsigned char)(usb_hardware.usLength));
                    fnProcessInput(0, &usb_hardware, USB_CONTROL_OUT_FRAME); // control endpoint
                    break;
                case USB_STATE_LAST_TX:
DEBUG_USB_EP0('T');
                    usb_hardware.usEP0State = USB_STATE_STATUS;
                    break;
                case USB_STATE_STATUS:                                   // endpoint 0 is sending a single packet or the last of multiple packtets and expecting the status stage reception
DEBUG_USB_EP0('S');
                    if ((usb_hardware.usLength = USBCOUNT0) == 0) {
DEBUG_USB_EP0((unsigned char)(usb_hardware.usLength));
                        fnProcessInput(0, &usb_hardware, USB_CONTROL_OUT_FRAME); // control endpoint
                        usb_hardware.usEP0State = USB_STATE_IDLE;
                    }
                    break;
                case USB_STATE_ADDRESSING:
DEBUG_USB_EP0('a');
                    USBFADDR = usb_hardware.ucUSBAddress;                // program the address to be used
                    // fall through
                case USB_STATE_STALL:                                    // endpoint 0 has sent a stall and the status handshake is expected
                    usb_hardware.usEP0State = USB_STATE_IDLE;
                    break;
                }
            }
            if (usUSB_Int_status & (EP1_TX_INT | EP2_TX_INT | EP3_TX_INT)) { // handle tx interrupt(s) on endpoint(s) 1..3
DEBUG_USB_EP123('t');
                if (usUSB_Int_status & EP1_TX_INT) {
DEBUG_USB_EP123('1');
                }
                if (usUSB_Int_status & EP2_TX_INT) {
DEBUG_USB_EP123('2');
DEBUG_USB_EP123(USBTXCSRL2);
                    usb_hardware.ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO2_ADD, 1);
                    usb_hardware.ptr_ucUSB_BDControl = USBTXCTRADD2;
                    usb_hardware.ptrEndpoint = &usb_endpoints[2];
                    uDisable_Interrupt();                                // ensure message can not be interrupted
                    fnUSB_handle_frame(USB_TX_ACKED, 0, 2, &usb_hardware);// handle ack event
                    uEnable_Interrupt();                                 // release
                }
                if (usUSB_Int_status & EP3_TX_INT) {
DEBUG_USB_EP123('3');
                }
            }
        }
        else if (((usUSB_Int_status = USBRXIS) & USBRXIE) != 0) {        // rx interrupt(s) on endpoint(s) 1..3
    #ifdef _WINDOWS
            USBRXIS = 0;                                                 // read of USBRXRIS resets all interrupt flags
    #endif
DEBUG_USB_EP123('r');
            if (usUSB_Int_status & EP1_RX_INT) {
DEBUG_USB_EP123('1');
DEBUG_USB_EP123(USBRXCSRL1);
                if (USBRXCSRL1 & DEVICE_RXRDY) {                         // packet received
                    usb_hardware.ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO1_ADD, 0);
                    usb_hardware.ptr_ucUSB_BDControl = USBRXCTRADD1;
                    usb_hardware.ptrEndpoint = &usb_endpoints[1];
                    usb_hardware.usLength = USBRXCOUNT1;
DEBUG_USB_EP123((unsigned char)usb_hardware.usLength);
                    if (fnProcessInput(1, &usb_hardware, USB_OUT_FRAME) != MAINTAIN_OWNERSHIP) {
                        USBRXCSRL1 = 0;                                  // reset DEVICE_RXRDY flag
                    }
                }
            }
            if (usUSB_Int_status & EP2_RX_INT) {
DEBUG_USB_EP123('2');
/*                if (USBRXCSRL2 & DEVICE_RXRDY) { // packet received
                    usb_hardware.ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO2_ADD, 0);
                    usb_hardware.ptr_ucUSB_BDControl = USBRXCTRADD2;
                    usb_hardware.ptrEndpoint = &usb_endpoints[2];
                    usb_hardware.usLength = USBRXCOUNT2;
                    fnProcessInput(2, &usb_hardware, USB_OUT_FRAME);
                    USBRXCSRL2 = 0;                                      // reset DEVICE_RXRDY flag
                }*/
            }
            if (usUSB_Int_status & EP3_RX_INT) {
DEBUG_USB_EP123('3');
/*                if (USBRXCSRL3 & DEVICE_RXRDY) { // packet received
                    usb_hardware.ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO3_ADD, 0);
                    usb_hardware.ptr_ucUSB_BDControl = USBRXCTRADD3;
                    usb_hardware.ptrEndpoint = &usb_endpoints[3];
                    usb_hardware.usLength = USBRXCOUNT3;
                    fnProcessInput(3, &usb_hardware, USB_OUT_FRAME);
                    USBRXCSRL3 = 0;                                      // reset DEVICE_RXRDY flag
                }*/
            }
        }
        else {
            break;
        }
    }
DEBUG_USB_INT(' ');
    if (iTest > 950) {                                                   // ensure no overrun in debug buffer
        iTest = 0;
    }
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    volatile unsigned long ulDummy;
    RCGC2 |= CGC_USB0;                                                   // enable clocks to USB
    SRCR2 &= ~CGC_USB0;                                                  // ensure not held in reset
    RCC2 &= ~USBPWRDWN;                                                  // enable USB PLL

#if defined USB_BUS_SWITCH
    #if defined _LM3S3748 || defined _LM3S3768
    RCGC2 |= CGC_GPIOH;                                                  // port H must be enabled to use USB control lines on it
    SRCR2 &= ~CGC_GPIOH;
    GPIOAFSEL_H |= (PORTH_BIT3 | PORTH_BIT4);                            // connect USB0EPEN and USB0PFLT pins
    GPIODEN_H |= (PORTH_BIT3 | PORTH_BIT4);
    GPIOPUR_H |= (PORTH_BIT4);                                           // enable pullup on input
    #elif defined _LM3S5732
    RCGC2 |= CGC_GPIOC;                                                  // port C must be enabled to use USB control lines on it
    SRCR2 &= ~CGC_GPIOC;
    GPIOAFSEL_C |= (PORTC_BIT5 | PORTC_BIT6);                            // connect USB0EPEN and USB0PFLT pins
    GPIODEN_C |= (PORTC_BIT5 | PORTC_BIT6);
    GPIOPUR_C |= (PORTH_BIT6);                                           // enable pullup on input
    #endif
#endif

    if (++pars->ucEndPoints > NUMBER_OF_USB_ENDPOINTS) {                 // add endpoint 0
        pars->ucEndPoints = NUMBER_OF_USB_ENDPOINTS;                     // limit to maximum available in device
    }

    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * pars->ucEndPoints)); // get zeroed endpoint control structures

    usb_endpoints[0].usEndpointSize = ENDPOINT_0_SIZE;                   // endpoint size on control endpoint 0

    USBTXIE = EP0_TX_RX_INT;                                             // enable tx/rx interrupts on endpoint 0
    ulDummy = USBIS;                                                     // clear possible interrupts
    ulDummy = USBTXIS;                                                   // clear possible tx interrupts
    ulDummy = USBRXIS;                                                   // clear possible rx interrupts
    fnEnterInterrupt(irq_USB_ID, PRIORITY_USB_OTG, usb_otg_isr);         // enter the interrupt handler
    USBIE = (RESET_INT);                                                 // enable reset interrupt

    while (!(RIS & USBPLLLRIS)) {                                        // wait for USBPLL to lock
#ifdef _WINDOWS
        RIS |= USBPLLLRIS;
#endif
    }

    if (!(pars->usConfig & USB_HOST_MODE)) {
#ifdef EK_LM3S3748                                                       // set HOST/DEVICE muliplexer accoringly
        GPIODIR_H  |= (PORTH_BIT2);
        GPIODATA_H |= (PORTH_BIT2);                                      // drive '1' for device (see also errata 10.3 for A0 device)
        GPIODEN_H  |= (PORTH_BIT2);
        USBGPCS = DEVMOD;                                                // force device mode
#endif
        USBPOWER = SOFTCONN;                                             // connect USB D+/D- lines. The device will now only respond to a USB reset.
    }
#ifdef EK_LM3S3748                                                       // set HOST/DEVICE muliplexor accoringly
    else {
        GPIODIR_H  |= (PORTH_BIT2);
        GPIODATA_H &= ~(PORTH_BIT2);                                     // driver '0' for host
        GPIODEN_H  |= (PORTH_BIT2);
        USBGPCS = 0;                                                     // force host mode
    }
#endif
}

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW *ptr_usb_hardware)
{
    switch (iEndpoint) {
    case 0:
        if (USBCSRL0 & EP0_DEVICE_TXRDY) {
            return ENDPOINT_NOT_FREE;
        }
        ptr_usb_hardware->ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO0_ADD, 1);
        break;
    case 1:
        if (USBTXCSRL1 & DEVICE_TXRDY) {
            return ENDPOINT_NOT_FREE;
        }
        ptr_usb_hardware->ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO1_ADD, 1);
        ptr_usb_hardware->ptrEndpoint = &usb_endpoints[1];
        ptr_usb_hardware->ptr_ucUSB_BDControl = USBTXCTRADD1;

        break;
    case 2:
        if (USBTXCSRL2 & DEVICE_TXRDY) {
            return ENDPOINT_NOT_FREE;
        }
        ptr_usb_hardware->ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO2_ADD, 1);
        ptr_usb_hardware->ptrEndpoint = &usb_endpoints[2];
        ptr_usb_hardware->ptr_ucUSB_BDControl = USBTXCTRADD2;
        break;
    case 3:
        if (USBTXCSRL3 & DEVICE_TXRDY) {
            return ENDPOINT_NOT_FREE;
        }
        ptr_usb_hardware->ptrFIFO = (unsigned char *)fnGetUSB_FIFOAdd(USBFIFO3_ADD, 1);
        ptr_usb_hardware->ptrEndpoint = &usb_endpoints[3];
        ptr_usb_hardware->ptr_ucUSB_BDControl = USBTXCTRADD3;
        break;
    }
    return ENDPOINT_FREE;
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength)
{
  //#define DOUBLE_BUFFERED
    USB_END_POINT *endpoint = (usb_endpoints + ucEndpointRef);
    unsigned char ucSize = 8;
    unsigned char ucMode = 0;
    unsigned char ucEndpointBit = (0x01 << ucEndpointRef);
    int iRegisterOffset;

    if (ENDPOINT_DISABLE == ucEndpointType) {
        USBIE = (RESET_INT);                                             // disable suspend and resume interrupts when not configured
        USBTXIE &= ~ucEndpointBit;                                       // disable tx interrupts on the endpoint
        USBRXIE &= ~ucEndpointBit;                                       // disable rx interrupts on the endpoint
        return;
    }
    USBIE = (RESET_INT | SUSPEND_INT);                                   // since we have just been configured, enable suspend interrupt
    USBEPIDX = ucEndpointRef;                                            // set configuration index
    ucEndpointRef--;                                                     // for subsequent address offset use
    iRegisterOffset = ((USBTXMAXP2ADD - USBTXMAXP1ADD) * ucEndpointRef);
#ifdef _WINDOWS
    fnGetUSB_FIFO_config();                                              // get the present FIFO configuration from the simulator
#endif
    endpoint->usEndpointSize = usEndpointLength;                         // enter the endpoint length for later reference
    while (usEndpointLength < 2048) {                                    // calculate the size setting
        usEndpointLength *= 2;
        ucSize--;
    }
    switch (ucEndpointType & ~IN_ENDPOINT) {
    case ENDPOINT_ISOCHRONOUS:
        ucMode = USB_ISO;
        break;
    case ENDPOINT_BULK:
    case ENDPOINT_INTERRUPT:
        if (!(ucEndpointType & IN_ENDPOINT)) {
            ucMode = DISNYET_PIDERR;
        }
        break;
    default:                                                             // control endpoint
        break;
    }
    if (ucEndpointType & IN_ENDPOINT) {
#ifdef DOUBLE_BUFFERED
        USBTXFIFOSZ = (ucSize | FIFO_DOUBLE_BUFFERED);                   // set the size of the transmission buffers in the FIFO
        USBTXDPKTBUFDIS &= ~ucEndpointBit;                               // ensure that double buffering is not disabled on this endpoint
#else
        USBTXFIFOSZ = ucSize;                                            // set the size of the transmission buffers in the FIFO
        USBTXDPKTBUFDIS |= ucEndpointBit;                                // ensure that double buffering is disabled on this endpoint
#endif
        USBTXFIFOADD = (usFIF0address/8);                                // set the starting address of the first transmission FIFO
        *(USBTXMAXP1ADD + (iRegisterOffset/sizeof(unsigned short))) = endpoint->usEndpointSize;
        *(USBRXCSRH1ADD + iRegisterOffset) = 0;
#ifdef USB_DMA_MODE
        *(USBTXCSRH1ADD + iRegisterOffset) = (ucMode | (USB_DMA | USB_TX));
#else
        *(USBTXCSRH1ADD + iRegisterOffset) = (ucMode | USB_TX);
#endif
        USBTXIE |= ucEndpointBit;                                        // enable tx interrupts on the endpoint
    }
    else {
#ifdef DOUBLE_BUFFERED
        USBRXFIFOSZ = (ucSize | FIFO_DOUBLE_BUFFERED);                   // set the size of the reception buffers in the FIFO
        USBRXDPKTBUFDIS &= ~ucEndpointBit;                               // ensure that double buffering is not disabled on this endpoint
#else
        USBRXFIFOSZ = ucSize;                                            // set the size of the reception buffers in the FIFO
        USBRXDPKTBUFDIS |= ucEndpointBit;                                // ensure that double buffering is disabled on this endpoint
#endif
        USBRXFIFOADD = (usFIF0address/8);                                // set the starting address of the first reception FIFO
        *(USBRXMAXP1ADD + (iRegisterOffset/sizeof(unsigned short))) = endpoint->usEndpointSize;
        *(USBTXCSRH1ADD + iRegisterOffset) = 0;
#ifdef USB_DMA_MODE
        *(USBRXCSRH1ADD + iRegisterOffset) = (ucMode | USB_DMA_RX_EN);
#else
        *(USBRXCSRH1ADD + iRegisterOffset) = ucMode;
#endif
        USBRXIE |= ucEndpointBit;                                        // enable rx interrupts on the endpoint
    }

#ifdef _WINDOWS
    fnPutUSB_FIFO_config();                                              // log the FIFO configuration in the simulator
#endif
#ifdef DOUBLE_BUFFERED
    usFIF0address += (endpoint->usEndpointSize * 2);                     // two buffers have been added to the FIFO with the endpoint buffer size
    endpoint->ucEndpointRxFlags = ENDPOINT_DUAL_BANK;                    // flag that dual bank is in use on this endpoint
    endpoint->ucEndpointTxFlags = ENDPOINT_DUAL_BANK;
#else
    usFIF0address += endpoint->usEndpointSize;                           // one buffers have been added to the FIFO with the endpoint buffer size
    endpoint->ucEndpointRxFlags = 0;
    endpoint->ucEndpointTxFlags = 0;
#endif

    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simultor that USB enumeration has completed
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

    #ifdef _WINDOWS
// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
    return endpoint->usEndpointSize;
}
    #endif

#endif                                                                   // end if defined USB_INTERFACE



#if defined CAN_INTERFACE
// Hardware configuration of CAN controller
//
extern void fnConfigCAN(QUEUE_HANDLE DriverID, CANTABLE *pars)
{
}

// Hardware initialisation of CAN controller
//
extern void fnInitCAN(CANTABLE *pars)
{
}

// The CAN driver has called this to collect a received message
//
extern unsigned char fnCAN_get_rx(QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter)
{
return 0;
}

// The CAN driver has called this to send a message
//
unsigned char fnCAN_tx(QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter)
{
    return 0;
}
#endif


#ifdef SUPPORT_PORT_INTERRUPTS                                           // ports A..H
    #ifdef SUPPORT_PORT_A_INTERRUPTS
        static void (*port_a_handler[8])( void ) = {0};
        static __interrupt void port_A_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_A != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_A) {
                    _CLEAR_INTERRUPT(GPIOICR, A, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_a_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_B_INTERRUPTS
        static void (*port_b_handler[8])( void ) = {0};
        static __interrupt void port_B_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_B != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_B) {
                    _CLEAR_INTERRUPT(GPIOICR, B, ulBit);                 // clear the interrupt
                    if (port_b_handler[iHandler] != 0) {                 // allow a dummy interrupt when used only for triggering ADC
                        uDisable_Interrupt();                            // ensure interrupts remain blocked during subroutines
                        port_b_handler[iHandler]();                      // call the user's handler
                        uEnable_Interrupt();                             // release
                    }
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_C_INTERRUPTS
        static void (*port_c_handler[8])( void ) = {0};
        static __interrupt void port_C_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_C != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_C) {
                    _CLEAR_INTERRUPT(GPIOICR, C, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_c_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_D_INTERRUPTS
        static void (*port_d_handler[8])( void ) = {0};
        static __interrupt void port_D_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_D != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_D) {
                    _CLEAR_INTERRUPT(GPIOICR, D, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_d_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_E_INTERRUPTS
        static void (*port_e_handler[8])( void ) = {0};
        static __interrupt void port_E_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_E != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_E) {
                    _CLEAR_INTERRUPT(GPIOICR, E, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_e_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_F_INTERRUPTS
        static void (*port_f_handler[8])( void ) = {0};
        static __interrupt void port_F_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_F != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_F) {
                    _CLEAR_INTERRUPT(GPIOICR, F, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_f_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_G_INTERRUPTS
        static void (*port_g_handler[8])( void ) = {0};
        static __interrupt void port_G_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_G != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_G) {
                    _CLEAR_INTERRUPT(GPIOICR, G, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_g_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_H_INTERRUPTS
        static void (*port_h_handler[8])( void ) = {0};
        static __interrupt void port_H_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_H != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_H) {
                    _CLEAR_INTERRUPT(GPIOICR, H, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_h_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release                
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif

    #ifdef SUPPORT_PORT_J_INTERRUPTS
        static void (*port_j_handler[8])( void ) = {0};
        static __interrupt void port_J_irq(void)
        {
            unsigned long ulBit = 0x80;
            int iHandler = 7;
            while ((ulBit != 0) && (GPIOMIS_J != 0)) {                   // while interrupts on this port
                if (ulBit & GPIOMIS_J) {
                    _CLEAR_INTERRUPT(GPIOICR, J, ulBit);                 // clear the interrupt
                    uDisable_Interrupt();                                // ensure interrupts remain blocked during subroutines
                    port_j_handler[iHandler]();                          // call the user's handler
                    uEnable_Interrupt();                                 // release
                }
                ulBit >>= 1;
                iHandler--;
            }
        }
    #endif
#endif

#if defined SUPPORT_TIMER                                                 // {5}
static void (*timer_handler[CHIP_HAS_GP_TIMER * 2])( void ) = {0};       // {30}
static __interrupt void timer_0_irq(void)
{
    _CLEAR_INTERRUPT(GPTMICR, 0, TATOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[0]();
    uEnable_Interrupt();                                                 // release
}     

static __interrupt void timer_0b_irq(void)                                // {30}
{
    _CLEAR_INTERRUPT(GPTMICR, 0, TBTOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[1]();
    uEnable_Interrupt();                                                 // release
}     


static __interrupt void timer_1_irq(void)
{
    _CLEAR_INTERRUPT(GPTMICR, 1, TATOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[2]();
    uEnable_Interrupt();                                                 // release
}

static __interrupt void timer_1b_irq(void)                               // {30}
{
    _CLEAR_INTERRUPT(GPTMICR, 1, TBTOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[3]();
    uEnable_Interrupt();                                                 // release
}      


    #if CHIP_HAS_GP_TIMER > 2
static __interrupt void timer_2_irq(void)
{
    _CLEAR_INTERRUPT(GPTMICR, 2, TATOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[4]();
    uEnable_Interrupt();                                                 // release
}

static __interrupt void timer_2b_irq(void)                               // {30}
{
    _CLEAR_INTERRUPT(GPTMICR, 2, TBTOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[5]();
    uEnable_Interrupt();                                                 // release
}   
    #endif

    #if CHIP_HAS_GP_TIMER > 3
static __interrupt void timer_3_irq(void)
{
    _CLEAR_INTERRUPT(GPTMICR, 3, TATOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[6]();
    uEnable_Interrupt();                                                 // release
}

static __interrupt void timer_3b_irq(void)                               // {30}
{
    _CLEAR_INTERRUPT(GPTMICR, 3, TBTOCINT);                              // clear the interrupt
    uDisable_Interrupt();                                                // ensure interrupts remain blocked during subroutines
    timer_handler[7]();
    uEnable_Interrupt();                                                 // release
}
    #endif

static void (*_timer_irq[CHIP_HAS_GP_TIMER * 2])( void ) = {             // {30}
timer_0_irq, timer_0b_irq,
timer_1_irq, timer_1b_irq,
    #if CHIP_HAS_GP_TIMER > 2
timer_2_irq, timer_2b_irq,
    #endif
    #if CHIP_HAS_GP_TIMER > 3
timer_3_irq, timer_3b_irq
    #endif
};

#endif


#if defined SUPPORT_ADC && PART_DC1 & ADC0_PRESENT1

static void ( *adc_int_complete_handler)( void ) = 0;
static unsigned short usSampleCount = 0;
static unsigned short *ptrSamples = 0;

static __interrupt void _ADC_sequence_0_complete(void)
{
    int iSamples = 0;
    ADCISC_0 = INT_SS0;                                                  // clear the interrupt
    while (!(ADCSSFSTAT0_0 & FIFO_EMPTY)) {
        if (ptrSamples != 0) {
            *ptrSamples++ = (signed short)GET_ADC_FIFO(0, 0, ADCSSFIFO0_0); // read, clear and store the samples
            iSamples++;
        }
    }
    if (usSampleCount <= 1) {
        RCGC0 &= ~CGC_SARADC0;                                           // disable clocks to module
        uDisable_Interrupt();                                            // {19} protect the call from interrupts
        adc_int_complete_handler();                                      // call the interrupt handler
        uEnable_Interrupt();
    }
    else {
        if (adc_int_complete_handler != 0) {
            usSampleCount--;
        }
        else {                                                           // no interrupt routine so infinite sampling to fixed buffer
            ptrSamples -= iSamples;                                      // reset sample array for next sequence
        }
        if ((ADCEMUX_0 & SS0_MASK) == TRIG_CONTROLLER) {
            ADCPSSI_0 = INITIATE_SS0;                                    // trigger next sequence sampling by software
        }
    }
}


#if defined SUPPORT_ADC && PART_DC1 & ADC1_PRESENT1

static void ( *adc1_int_complete_handler)( void ) = 0;
static unsigned short usSample1Count = 0;
static unsigned short *ptrSamples1 = 0;

static __interrupt void _ADC_sequence_1_complete(void)
{
    int iSamples = 0;
    ADCISC_1 = INT_SS0;                                                  // clear the interrupt
    while (!(ADCSSFSTAT0_1 & FIFO_EMPTY)) {
        if (ptrSamples1 != 0) {
            *ptrSamples1++ = (signed short)GET_ADC_FIFO(1, 0, ADCSSFIFO0_1); // read, clear and store the samples
            iSamples++;
        }
    }
    if (usSample1Count <= 1) {
        RCGC0 &= ~CGC_SARADC1;                                           // disable clocks to module
        uDisable_Interrupt();                                            // protect the call from interrupts
        adc1_int_complete_handler();                                     // call the interrupt handler
        uEnable_Interrupt();
    }
    else {
        if (adc1_int_complete_handler != 0) {
            usSample1Count--;
        }
        else {                                                           // no interrupt routine so infinite sampling to fixed buffer
            ptrSamples1 -= iSamples;                                     // reset sample array for next sequence
        }
        if ((ADCEMUX_1 & SS0_MASK) == TRIG_CONTROLLER) {
            ADCPSSI_1 = INITIATE_SS0;                                    // trigger next sequence sampling by software
        }
    }
}
#endif

#ifdef _TEMPEST_CLASS                                                    // {24}
static void fnConfigADC(ADC_SETUP *ptrADC_settings, int iChannel)        // used only by Tempest class devices with more that 8 ADC inputs (more than 1 ADC module)
{
    LM3XXXXX_ADC *ptrADC_reg;
    int iInputs = 0;
    unsigned long ulConfig = 0;
    unsigned long ulControl = 0;
    unsigned long ulInternalTemp = 0;
    unsigned long ulInputNumber = 0;
    unsigned long ulInputPinNumber;
    unsigned long ulBit = 0x00000001;
    unsigned long ulDifInputs  = ptrADC_settings->int_adc_differential_inputs;
    unsigned long ulSingInputs = ptrADC_settings->int_adc_single_ended_inputs;
    if (iChannel == 0) {
        ptrADC_reg = (LM3XXXXX_ADC *)ADC_0_BLOCK;
        ulInputPinNumber = 0;
    }
    else {
        ptrADC_reg = (LM3XXXXX_ADC *)ADC_1_BLOCK;
        ulSingInputs >>= 8;
        ulDifInputs  >>= 8;
        ulInputPinNumber = 8;
    }
    ptrADC_reg->ADCACTSS &= ~ASEN0;                                      // ensure sequencer is disabled before configuring
    ptrADC_reg->ADCEMUX  &= ~SS0_MASK;                                   // clear sequencer mode
    if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING | ADC_TRIGGER_COMPARATOR_0 | ADC_TRIGGER_COMPARATOR_1 | ADC_TRIGGER_COMPARATOR_2 | ADC_TRIGGER_TIMER | ADC_TRIGGER_PWM_0 | ADC_TRIGGER_PWM_1 | ADC_TRIGGER_PWM_2 | ADC_TRIGGER_PWM_3)) {
        if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING)) {
            INTERRUPT_SETUP interrupt_setup;                             // interrupt configuration parameters
            interrupt_setup.int_type = PORT_INTERRUPT;                   // identifier when configuring
            interrupt_setup.int_handler = 0;                             // no specific handling function
            switch (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING)) {
            case ADC_TRIGGER_GPIO_PB4_RISING:
                interrupt_setup.int_port_sense = IRQ_RISING_EDGE;        // trigger on rising edge
                break;
            case ADC_TRIGGER_GPIO_PB4_FALLING:
                interrupt_setup.int_port_sense = IRQ_FALLING_EDGE;       // trigger on falling edge
                break;
            case (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING):
                interrupt_setup.int_port_sense = IRQ_BOTH_EDGES;         // trigger on both edges
                break;
            }
            interrupt_setup.int_port = PORTB;                            // the port used
            interrupt_setup.int_priority = PORTB_INTERRUPT_PRIORITY;     // port interrupt priority
            interrupt_setup.int_port_bit = PORTB_BIT4;                   // the input connected
            interrupt_setup.int_port_characteristic = SET_ADC_TRIGGER;   // don't change configuration
            fnConfigureInterrupt(&interrupt_setup);                      // configure PB4 accordingly
            ptrADC_reg->ADCEMUX |= TRIG_GPIO_PB4;                        // trigger on PB4 input
        }
        else if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_COMPARATOR_0 | ADC_TRIGGER_COMPARATOR_1 | ADC_TRIGGER_COMPARATOR_2)) {
            switch (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_COMPARATOR_0 | ADC_TRIGGER_COMPARATOR_1 | ADC_TRIGGER_COMPARATOR_2)) {
            case ADC_TRIGGER_COMPARATOR_0:
                ptrADC_reg->ADCEMUX |= TRIG_AN_COMP_0;
                break;
            case ADC_TRIGGER_COMPARATOR_1:
                ptrADC_reg->ADCEMUX |= TRIG_AN_COMP_1;
                break;
            case ADC_TRIGGER_COMPARATOR_2:
                ptrADC_reg->ADCEMUX |= TRIG_AN_COMP_2;
                break;
            }
        }
        else if (ptrADC_settings->int_adc_mode & ADC_TRIGGER_TIMER) {
            ptrADC_reg->ADCEMUX |= TRIG_TIMER;                           // trigger from any enabled timer source
        }
        else if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_PWM_0 | ADC_TRIGGER_PWM_1 | ADC_TRIGGER_PWM_2 | ADC_TRIGGER_PWM_3)) {
            switch (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_PWM_0 | ADC_TRIGGER_PWM_1 | ADC_TRIGGER_PWM_2 | ADC_TRIGGER_PWM_3)) {
            case ADC_TRIGGER_PWM_0:
                ptrADC_reg->ADCEMUX |= TRIG_PMW_0;
                break;
            case ADC_TRIGGER_PWM_1:
                ptrADC_reg->ADCEMUX |= TRIG_PMW_1;
                break;
            case ADC_TRIGGER_PWM_2:
                ptrADC_reg->ADCEMUX |= TRIG_PMW_2;
                break;
            case ADC_TRIGGER_PWM_3:
                ptrADC_reg->ADCEMUX |= TRIG_PMW_3;
                break;
            }
        }                        
    }
    else {
        ptrADC_reg->ADCEMUX |= TRIG_CONTROLLER;                          // started by software
    }
    ptrADC_reg->ADCSAC = ptrADC_settings->int_adc_averaging;
    while (ulBit <= 0x80) {                                              // count the number of inputs to be sampled
        if (ulBit & (ulSingInputs | ulDifInputs)) {
            if (ulBit & ulDifInputs) {
                if (!(ulInputNumber & 0x1)) {                            // enter only one of the differential pairs
                    ulControl |= (DIF_SAMPLE_1 << (iInputs * 4));        // mark that this belongs to a differential input pair
                    ulConfig |= (ulInputNumber/2 << (iInputs * 4));      // enter the differential pair in the sampling sequence
                    iInputs++;
                }
            }
            else {
                if (ulSingInputs & (ulBit << 16)) {                      // the channel is from the internal temperature
                    ulInternalTemp |= (TEMP_SAMPLE_1 << (iInputs * 4));
                }
                else {
                    ulConfig |= ((ulInputNumber + ulInputPinNumber) << (iInputs * 4));  // enter the channel in the sampling sequence
                }
                iInputs++;
            }
            if (ulInputPinNumber >= 12) {                                // 12 = PD3, 14 = PD2, 14 = PD1, 15 = PD0
                _CONFIG_PERIPHERAL(D, SARADC1, 0, (PORTD_BIT3 >> (ulInputPinNumber - 12)), 0);
                GPIOAMSEL_D |= (0x01 << (3 - (ulInputPinNumber - 12)));  // disable analogue isolation on pin
            }
            else if (ulInputPinNumber >= 10) {                           // 10 = PB4, 11 = PB5
                _CONFIG_PERIPHERAL(B, SARADC1, 0, (PORTB_BIT4 << (ulInputPinNumber - 10)), 0);
                GPIOAMSEL_B |= (0x01 << (4 + (ulInputPinNumber - 10)));  // disable analogue isolation on pin
            }
            else if (ulInputPinNumber >= 8) {                            // 8 = PE3, 9 = PE2
                _CONFIG_PERIPHERAL(E, SARADC1, 0, (PORTE_BIT3 >> (ulInputPinNumber - 8)), 0);
                GPIOAMSEL_E |= (0x01 << (3 - (ulInputPinNumber - 8)));   // disable analogue isolation on pin
            }
            else if (ulInputPinNumber >= 4) {                            // 4 = PD7, 5 = PD6, 6 = PD5, 7 = PD4
                _CONFIG_PERIPHERAL(D, SARADC0, 0, (PORTD_BIT7 >> (ulInputPinNumber - 4)), 0);
                GPIOAMSEL_D |= (0x01 << (7 - (ulInputPinNumber - 4)));   // disable analogue isolation on pin
            }
            else {                                                       // 0 = PE7, 1 = PE6, 2 = PE5, 3 = PE4
                _CONFIG_PERIPHERAL(E, SARADC0, 0, (PORTE_BIT7 >> ulInputPinNumber), 0);
                GPIOAMSEL_E |= (0x01 << (7 - ulInputPinNumber));         // disable analogue isolation on pin
            }
        }
        else {
            if (ulInputPinNumber >= 12) {                                // 12 = PD3, 14 = PD2, 14 = PD1, 15 = PD0
                GPIOAMSEL_D &= ~(0x01 << (3 - (ulInputPinNumber - 12))); // enable analogue isolation on pin
            }
            else if (ulInputPinNumber >= 10) {                           // 10 = PB4, 11 = PB5
                GPIOAMSEL_B &= ~(0x01 << (4 + (ulInputPinNumber - 10))); // enable analogue isolation on pin
            }
            else if (ulInputPinNumber >= 8) {                            // 8 = PE3, 9 = PE2
                GPIOAMSEL_E &= ~(0x01 << (3 - (ulInputPinNumber - 8)));  // enable analogue isolation on pin
            }
            else if (ulInputPinNumber >= 4) {                            // 4 = PD7, 5 = PD6, 6 = PD5, 7 = PD4
                GPIOAMSEL_D &= ~(0x01 << (7 - (ulInputPinNumber - 4)));  // enable analogue isolation on pin
            }
            else {                                                       // 0 = PE7, 1 = PE6, 2 = PE5, 3 = PE4
                GPIOAMSEL_E &= ~(0x01 << (7 - ulInputPinNumber));        // enable analogue isolation on pin
            }
        }
        ulBit <<= 1;
        ulInputNumber++;
        ulInputPinNumber++;
    }
    if (iInputs == 0) {
        return;                                                          // no inputs found
    }
    iInputs--;
    ulControl |= (((END_SAMPLE_1 | INT_SAMPLE_1) << (iInputs * 4)) | ulInternalTemp); // mark last sample which will interrupt 
    ptrADC_reg->ADCSSMUX0 = ulConfig;                                    // program the sequencer
    ptrADC_reg->ADCSSCTL0 = ulControl;
}
#endif
#endif

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
#ifdef SUPPORT_PORT_INTERRUPTS
    case PORT_INTERRUPT:
        {   
            LM3XXXXX_PORT_CONTROL *port;
            unsigned char ucIRQ_bit = (((INTERRUPT_SETUP *)ptrSettings)->int_port_bit);
            int iRef = 0;
            unsigned long irq_ID;
            unsigned long ulPortEnable;
            void (**port_handler)(void);
            void (*port_irq)(void);
            if (ucIRQ_bit == 0) {
                return;                                                  // invalid port bit
            }
            while ((ucIRQ_bit & (1 << iRef)) == 0) {
                iRef++;
            }
            port = fnGetPortBlock(((INTERRUPT_SETUP *)ptrSettings)->int_port);
            switch (((INTERRUPT_SETUP *)ptrSettings)->int_port) {
    #if defined SUPPORT_PORT_A_INTERRUPTS && (PART_DC4 & GPIOA_PRESENT4)
            case PORTA:
                ulPortEnable = CGC_GPIOA;
                irq_ID = irq_GPIO_Port_A_ID;
                port_irq = port_A_irq;
                port_handler = &port_a_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_B_INTERRUPTS
            case PORTB:
                ulPortEnable = CGC_GPIOB;
                irq_ID = irq_GPIO_Port_B_ID;
                port_irq = port_B_irq;
                port_handler = &port_b_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_C_INTERRUPTS
            case PORTC:
                ulPortEnable = CGC_GPIOC;
                irq_ID = irq_GPIO_Port_C_ID;
                port_irq = port_C_irq;
                port_handler = &port_c_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_D_INTERRUPTS
            case PORTD:
                ulPortEnable = CGC_GPIOD;
                irq_ID = irq_GPIO_Port_D_ID;
                port_irq = port_D_irq;
                port_handler = &port_d_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_E_INTERRUPTS
            case PORTE:
                ulPortEnable = CGC_GPIOE;
                irq_ID = irq_GPIO_Port_E_ID;
                port_irq = port_E_irq;
                port_handler = &port_e_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_F_INTERRUPTS
            case PORTF:
                ulPortEnable = CGC_GPIOF;
                irq_ID = irq_GPIO_Port_F_ID;
                port_irq = port_F_irq;
                port_handler = &port_f_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_G_INTERRUPTS
            case PORTG:
                ulPortEnable = CGC_GPIOG;
                irq_ID = irq_GPIO_Port_G_ID;
                port_irq = port_G_irq;
                port_handler = &port_g_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_H_INTERRUPTS
            case PORTH:
                ulPortEnable = CGC_GPIOH;
                irq_ID = irq_GPIO_Port_H_ID;
                port_irq = port_H_irq;
                port_handler = &port_h_handler[iRef];
                break;
    #endif
    #ifdef SUPPORT_PORT_J_INTERRUPTS
            case PORTJ:
                ulPortEnable = CGC_GPIOJ;
                irq_ID = irq_GPIO_Port_J_ID;
                port_irq = port_J_irq;
                port_handler = &port_j_handler[iRef];
                break;
    #endif
            default:                                                     // invalid port
                return;
            }
            if (((((INTERRUPT_SETUP *)ptrSettings)->int_handler) == 0) && (((INTERRUPT_SETUP *)ptrSettings)->int_port_characteristic != SET_ADC_TRIGGER)) {  // disable interrupt
                port->GPIOIM &= ~ucIRQ_bit;                              // mask interrupt
                if (port->GPIOIM == 0) {                                 // if no more interrupts are required on the port
    #ifdef _WINDOWS
                    unsigned long ulState0 = IRQ0_31_SER;
    #endif
                    IRQ0_31_CER = (0x01 << irq_GPIO_Port_B_ID);          // disable them to the core
    #ifdef _WINDOWS
                    ulState0 &= ~IRQ0_31_CER;
                    IRQ0_31_SER   = ulState0;                            // synchronise the interrupt masks
                    IRQ0_31_CER   = IRQ0_31_SER;
    #endif
                }
                return;
            }
            RCGC2 |= ulPortEnable;                                       // ensure port is enabled
            SRCR2 &= ~ulPortEnable;
            port->GPIOAFSEL &= ~(ucIRQ_bit);                             // GPIO pin use
            port->GPIODIR &= ~(ucIRQ_bit);                               // input
            port->GPIODEN |= (ucIRQ_bit);                                // digital function

            if (((INTERRUPT_SETUP *)ptrSettings)->int_port_sense >= IRQ_FALLING_EDGE) { // edge sentivive
                port->GPIOIS &= ~ucIRQ_bit;                              // disable level sensitivity
                if (IRQ_BOTH_EDGES == ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) {
                    port->GPIOIBE |= ucIRQ_bit;                          // interrupt on both edges
                }
                else {
                    port->GPIOIBE &= ~ucIRQ_bit;                         // not both edges
                    if (IRQ_RISING_EDGE == ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) { // set the required edge only
                        port->GPIOIEV |= ucIRQ_bit;                      // rising edge sensitive
                    }
                    else {
                        port->GPIOIEV &= ~ucIRQ_bit;                     // falling edge sensitive
                    }
                }
            }
            else {                                                       // level sensitive
                port->GPIOIBE &= ~ucIRQ_bit;                             // no edge sensitivity
                port->GPIOIS |= ucIRQ_bit;                               // level sensitive
                if (IRQ_LEVEL_SENSITIVE_HIGH == ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) {
                    port->GPIOIEV |= ucIRQ_bit;                          // high level sensitive
                }
                else {
                    port->GPIOIEV &= ~ucIRQ_bit;                         // low level sensitive
                }
            }
            switch (((INTERRUPT_SETUP *)ptrSettings)->int_port_characteristic) {
            case PULLUP_ON:
                port->GPIOPUR |= ucIRQ_bit;
    #ifdef _WINDOWS
                port->GPIOPDR &= ~ucIRQ_bit;                             // pull down is automatically disabled
    #endif
                break;
            case PULLDOWN_ON:
                port->GPIOPDR |= ucIRQ_bit;
    #ifdef _WINDOWS
                port->GPIOPUR &= ~ucIRQ_bit;                             // pull up is automatically disabled
    #endif
            case SET_ADC_TRIGGER:                                        // input being configured for ADC trigger use (only valid for Port B)
                if (!(IRQ0_31_SER & (1 << irq_GPIO_Port_B_ID))) {        // port B interrupt is not enabled at the code
                    _CLEAR_INTERRUPT(GPIOICR, B, PORTB_BIT4);            // ensure interrupt cleared if not handled by general port interrupts
                }
                port->GPIOIM |= ucIRQ_bit;                               // unmask interrupt to allow ADC triggering, but it won't cause interrupts to occur unless they are already being used as normal port interrupts
                return;
            default:
                break;
            }
            port->_GPIOICR = ucIRQ_bit;                                  // clear pending
    #ifdef _WINDOWS
            port->_GPIOICR = 0;
    #endif
            fnEnterInterrupt(irq_ID, (((INTERRUPT_SETUP *)ptrSettings)->int_priority), port_irq);
            *port_handler = (((INTERRUPT_SETUP *)ptrSettings)->int_handler);
            port->GPIOIM |= ucIRQ_bit;                                   // unmask interrupt
        }
        break;
#endif
#if defined SUPPORT_TIMER                                                 // {5}
    case TIMER_INTERRUPT:                                                // {12}
        {
            int iTimer = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_reference); // the timer to use
            if (iTimer < CHIP_HAS_GP_TIMER) {                            // only handle when the referenced timer is available
                TIMER_INTERRUPT_SETUP *ptrSetup = (TIMER_INTERRUPT_SETUP *)ptrSettings;
                LM3XXXXX_GPT_CONTROL *timer_control = (LM3XXXXX_GPT_CONTROL *)GP_TIMER_0_BLOCK;
                if ((TIMER_DONT_DISTURB & ptrSetup->timer_mode) == 0) {
                    SRCR1 |= (CGC_TIMER0 << iTimer);                     // reset timer
                    timer_control += iTimer;                             // set to the timer's register block
                    if (ptrSetup->timer_mode & (TIMER_STOP_PWM_A | TIMER_STOP_PWM_B)) {
                        RCGC1 &= ~(CGC_TIMER0 << iTimer);                // power down timer
                        return;
                    }
                    RCGC1 |= (CGC_TIMER0 << iTimer);                     // ensure timer is enabled
                }
                else {
                    timer_control += iTimer;                             // set to the timer's register block
                    if (ptrSetup->timer_mode & (TIMER_STOP_PWM_A | TIMER_STOP_PWM_B)) {
                        if (ptrSetup->timer_mode & TIMER_STOP_PWM_A) {   // stop PWM A output
                            timer_control->GPTMCTL &= ~(TAEN);
                        }
                        if (ptrSetup->timer_mode & TIMER_STOP_PWM_B) {   // stop PWM B output
                            timer_control->GPTMCTL &= ~(TBEN);
                        }
                        return;
                    }
                }
 
                SRCR1 &= ~(CGC_TIMER0 << iTimer);                        // take out of reset
                if (ptrSetup->int_handler != 0) {
                    int iIrq = irq_Timer0_A_ID;
    #if CHIP_HAS_GP_TIMER > 3
                    if (iTimer < 3) {
                        iIrq += (iTimer * 2);
                    }
                    else {
                        iIrq = irq_Timer3_A_ID;
                    }
    #else
                    iIrq += (iTimer * 2);
    #endif
                    if (ptrSetup->timer_mode & TIMER_16BIT_CHANNEL_B) {  // 16 bit mode using channel B {30}
                        fnEnterInterrupt((iIrq + 1), ptrSetup->int_priority, (void (*)( void ))_timer_irq[(iTimer * 2) + 1]);
                        timer_handler[(iTimer * 2) + 1] = ptrSetup->int_handler;
                        timer_control->GPTMIMR |= TBTOIM;                // interrupt on timeout
                    }
                    else {
                        fnEnterInterrupt(iIrq, ptrSetup->int_priority, (void (*)( void ))_timer_irq[iTimer * 2]);
                        timer_handler[iTimer * 2] = ptrSetup->int_handler;
                        timer_control->GPTMIMR |= TATOIM;                // interrupt on timeout
                    }
                }
                else {
                    if (ptrSetup->timer_mode & TIMER_16BIT_CHANNEL_B) {  // 16 bit mode using channel B {30}
                        timer_control->GPTMIMR &= ~TBTOIM;               // no interrupt on timeout
                    }
                    else {
                        timer_control->GPTMIMR &= ~TATOIM;               // no interrupt on timeout
                    }
                }
                if (ptrSetup->timer_mode & (TIMER_PWM_A | TIMER_PWM_B)) {// {13}
                    timer_control->GPTMCFG = GPTMCFG_16_BIT_MODE;
                    if (ptrSetup->timer_mode & TIMER_PWM_A) {
                        switch (iTimer) {
                        case 0:
        #if defined _LM3S8962 || defined _LM3S6965
                            _CONFIG_PERIPHERAL(D, TIMER0, 1, (PORTD_BIT4), 0); // CCP0
        #else
                            _CONFIG_PERIPHERAL(B, TIMER0, 1, (PORTB_BIT0), (PB0_CCP0));
        #endif
                            break;
                        case 1:
                            _CONFIG_PERIPHERAL(B, TIMER1, 1, (PORTB_BIT1), (PB1_CCP2)); // CCP2
                            break;
        #if CHIP_HAS_GP_TIMER > 2
                        case 2:
                            _CONFIG_PERIPHERAL(A, TIMER2, 1, (PORTA_BIT7), (PA7_CCP4)); // CCP4
                            break;
        #endif
        #if CHIP_HAS_GP_TIMER > 3
                        case 3:
                            _CONFIG_PERIPHERAL(H, TIMER3, 1, (PORTH_BIT0), (PH0_CCP6)); // CCP6
                            break;
        #endif
                        }
                        timer_control->GPTMTAMR = (TAMS_PWM_MODE | TMR_PERIODIC);// set PWM mode
                        timer_control->GPTMTAMATCHR = ptrSetup->pwm_value; //((ptrSetup->timer_value * ptrSetup->pwm_value)/1000); // {16} set the PWM match value
                        timer_control->GPTMTAILR = ptrSetup->timer_value;// set the base frequence
                        timer_control->GPTMCTL |= (TAEN | TAPWML);       // enable operation - the timer will count down
                    }
                    else if (ptrSetup->timer_mode & TIMER_PWM_B) {
                        switch (iTimer) {
                        case 0:
        #if defined _LM3S8962 || defined _LM3S6965
                            _CONFIG_PERIPHERAL(D, TIMER0, 1, (PORTD_BIT7), PD7_CCP1); // CCP1
        #else
                            _CONFIG_PERIPHERAL(A, TIMER0, 1, (PORTA_BIT6), PA6_CCP1); // CCP1
        #endif
                            break;
                        case 1:
                            _CONFIG_PERIPHERAL(D, TIMER1, 1, (PORTD_BIT4), (PD4_CCP3)); // CCP3
                            break;
        #if CHIP_HAS_GP_TIMER > 2
                        case 2:
                            _CONFIG_PERIPHERAL(C, TIMER2, 1, (PORTC_BIT4), (PC4_CCP5)); // CCP5
                            break;
        #endif
        #if CHIP_HAS_GP_TIMER > 3
                        case 3:
                            _CONFIG_PERIPHERAL(B, TIMER3, 1, (PORTB_BIT6), (PB6_CCP7)); // CCP7
                            break;
        #endif
                        }
                        timer_control->GPTMTBMR = (TAMS_PWM_MODE | TMR_PERIODIC);// set PWM mode
                        timer_control->GPTMTBMATCHR = ptrSetup->pwm_value; //((ptrSetup->timer_value * ptrSetup->pwm_value)/1000); // {16} set the PWM match value
                        timer_control->GPTMTBILR = ptrSetup->timer_value;// set the base frequence
                        timer_control->GPTMCTL |= (TBEN | TBPWML);       // enable operation - the timer will count down
                    }
                }
                else {
                    if (ptrSetup->timer_mode & (TIMER_16BIT_CHANNEL_A | TIMER_16BIT_CHANNEL_B)) { // 16 bit mode using a single channel {30}
                        timer_control->GPTMCFG = GPTMCFG_16_BIT_MODE;
                        if (ptrSetup->timer_mode & TIMER_16BIT_CHANNEL_A) { // channel A in 16 bit mode
                            if (ptrSetup->timer_mode & (TIMER_PERIODIC)) {
                                timer_control->GPTMTAMR = (TMR_PERIODIC);// set to periodic mode
                            }
                            else {
                                timer_control->GPTMTAMR = (TMR_ONE_SHOT);// set to single-shot mode
                            }
                            if (ptrSetup->timer_mode & TIMER_MS_VALUE) { // ms value
                                timer_control->GPTMTAILR = ((MASTER_CLOCK/1000)*ptrSetup->timer_value); // set the ms timeout value
                            }
                            else {
                                timer_control->GPTMTAILR = ((MASTER_CLOCK/1000000)*ptrSetup->timer_value); // set the us timeout value
                            }
        #ifdef _WINDOWS
                            timer_control->GPTMTAILR &= 0xffff;          // ensure only lowest 16 bits are used
                            timer_control->GPTMTAR = timer_control->GPTMTAILR; // set initial timer count
        #endif
                            if (ptrSetup->timer_mode & TIMER_TRIGGER_ADC) {
                                timer_control->GPTMCTL = (TAOTE | TAEN); // enable operation - the timer will count down and trigger the ADC on reaching zero
                            }
                            else {
                                timer_control->GPTMCTL = (TAEN);         // enable operation - the timer will count down
                            }
                        }
                        else {                                           // channel B in 16 bit mode
                            if (ptrSetup->timer_mode & (TIMER_PERIODIC)) {
                                timer_control->GPTMTBMR = (TMR_PERIODIC);// set to periodic mode
                            }
                            else {
                                timer_control->GPTMTBMR = (TMR_ONE_SHOT);// set to single-shot mode
                            }
                            if (ptrSetup->timer_mode & TIMER_MS_VALUE) { // ms value
                                timer_control->GPTMTBILR = ((MASTER_CLOCK/1000)*ptrSetup->timer_value); // set the ms timeout value
                            }
                            else {
                                timer_control->GPTMTBILR = ((MASTER_CLOCK/1000000)*ptrSetup->timer_value); // set the us timeout value
                            }
        #ifdef _WINDOWS
                            timer_control->GPTMTBILR &= 0xffff;          // ensure only lowest 16 bits are used
                            timer_control->GPTMTBR = timer_control->GPTMTBILR; // set initial timer count
        #endif
                            if (ptrSetup->timer_mode & TIMER_TRIGGER_ADC) {
                                timer_control->GPTMCTL = (TBOTE | TBEN); // enable operation - the timer will count down and trigger the ADC on reaching zero
                            }
                            else {
                                timer_control->GPTMCTL = (TBEN);         // enable operation - the timer will count down
                            }
                        }
                    }
                    else {
                        if (ptrSetup->timer_mode & (TIMER_PERIODIC)) {
                            timer_control->GPTMTAMR = (TMR_PERIODIC);    // set to periodic mode
                        }
                        else {
                            timer_control->GPTMTAMR = (TMR_ONE_SHOT);    // set to single-shot mode
                        }
                        if (ptrSetup->timer_mode & TIMER_MS_VALUE) {     // ms value
                            timer_control->GPTMTAILR = ((MASTER_CLOCK/1000)*ptrSetup->timer_value); // set the ms timeout value
                        }
                        else {
                            timer_control->GPTMTAILR = ((MASTER_CLOCK/1000000)*ptrSetup->timer_value); // set the us timeout value
                        }
        #ifdef _WINDOWS
                        timer_control->GPTMTAR = timer_control->GPTMTAILR; // set initial timer count
        #endif
                        if (ptrSetup->timer_mode & TIMER_TRIGGER_ADC) {
                            timer_control->GPTMCTL = (TAOTE | TAEN);     // enable operation - the timer will count down and trigger the ADC on reaching zero
                        }
                        else {
                            timer_control->GPTMCTL = (TAEN);             // enable operation - the timer will count down
                        }
                    }
                }
            }
        }
        break;
#endif
#if defined SUPPORT_ADC && PART_DC1 & ADC0_PRESENT1
    case ADC_INTERRUPT:
        {
            ADC_SETUP *ptrADC_settings = (ADC_SETUP *)ptrSettings; 
            if (ADC_DISABLE_ADC & ptrADC_settings->int_adc_mode) {
                ADCIM_0 = 0;                                             // mask interrupt sources
    #if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS
                GPIOAMSEL_D = 0;                                         // disable isolation on all analogue pins
                GPIOAMSEL_E = 0;
        #ifdef _TEMPEST_CLASS                                            // {24}
                GPIOAMSEL_B = 0;
        #endif
    #endif
    #ifdef _TEMPEST_CLASS                                                // {24}
                RCGC0 &= ~(CGC_SARADC0 | CGC_SARADC1);                   // disable clocks to ADC modules
    #else
                RCGC0 &= ~CGC_SARADC0;                                   // disable clocks to ADC module
    #endif
                return;
            }
            if ((ptrADC_settings->int_adc_mode & ADC_ALL_RESULTS) && (ptrADC_settings->int_adc_result != 0)) {// if there is a result structure
                unsigned short usResultsToReturn = ptrADC_settings->int_sequence_count;
                unsigned long ulSample;
                unsigned short *usResult = ptrADC_settings->int_adc_result;
                if (RCGC0 & CGC_SARADC0) {
                    while (!(ADCSSFSTAT0_0 & FIFO_EMPTY)) {
                        ulSample = GET_ADC_FIFO(0, 0, ADCSSFIFO0_0);     // read and clear the sample
                        if (usResultsToReturn != 0) {
                            *usResult++ = (unsigned short)ulSample;      // store
                            usResultsToReturn--;
                        }
                    }
                }
    #ifdef _TEMPEST_CLASS                                                // {24}
                if (RCGC0 & CGC_SARADC1) {
                    while (!(ADCSSFSTAT0_1 & FIFO_EMPTY)) {
                        ulSample = GET_ADC_FIFO(1, 0, ADCSSFIFO0_1);     // read and clear the sample
                        if (usResultsToReturn != 0) {
                            *usResult++ = (unsigned short)ulSample;      // store
                            usResultsToReturn--;
                        }
                    }
                }
    #endif
            }
            if (!(ptrADC_settings->int_adc_mode & ADC_READ_ONLY)) {
                unsigned long ulDifInputs  = ptrADC_settings->int_adc_differential_inputs;
                unsigned long ulSingInputs = ptrADC_settings->int_adc_single_ended_inputs;
                if (ptrADC_settings->int_adc_mode & ADC_CONFIGURE_ADC) { // main configuration
    #ifndef _TEMPEST_CLASS                                               // {24}
                    int iInputs = 0;
                    unsigned long ulConfig = 0;
                    unsigned long ulControl = 0;
                    unsigned long ulInternalTemp = 0;                    // {14}
                    unsigned long ulInputNumber = 0;
                    unsigned long ulBit = 0x00000001;
    #endif
    #ifdef _TEMPEST_CLASS                                                // {24}
                    if ((ulDifInputs | ulSingInputs) & (ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3 | ADC_CHANNEL_4 | ADC_CHANNEL_5 | ADC_CHANNEL_6 | ADC_CHANNEL_7)) {
                        RCGC0 |= (CGC_SARADC0);                          // enable clocks to ADC0 module
                        RCGC0 |= CGC_SARADC0;                            // {33} enable clocks to ADC module (slow down slightly so that the ADC is ready for the first access)
                        fnConfigADC(ptrADC_settings, 0);
                        fnEnterInterrupt(irq_ADC_Sequence_0_ID, ptrADC_settings->int_priority, _ADC_sequence_0_complete); // enter the interrupt
                        adc_int_complete_handler = ptrADC_settings->int_handler;
                        usSampleCount = ptrADC_settings->int_sequence_count;
                        ptrSamples = ptrADC_settings->int_adc_result;
                        if (adc_int_complete_handler != 0) {
                            ADCIM_0 |= MASK_SS0;                         // enable interrupt from sequencer
                        }
                        ADCACTSS_0 |= ASEN0;                             // enable sequencer operation
                    }
                    else if ((ulDifInputs | ulSingInputs) & (ADC_CHANNEL_8 | ADC_CHANNEL_9 | ADC_CHANNEL_10 | ADC_CHANNEL_11 | ADC_CHANNEL_12 | ADC_CHANNEL_13 | ADC_CHANNEL_14 | ADC_CHANNEL_15)) {
                        RCGC0 |= (CGC_SARADC1);                          // enable clocks to ADC1 module
                        fnConfigADC(ptrADC_settings, 1);
                        fnEnterInterrupt(irq_ADC1_Sequence_0_ID, ptrADC_settings->int_priority, _ADC_sequence_1_complete); // enter the interrupt
                        adc1_int_complete_handler = ptrADC_settings->int_handler;
                        usSample1Count = ptrADC_settings->int_sequence_count;
                        ptrSamples1 = ptrADC_settings->int_adc_result;
                        if (adc1_int_complete_handler != 0) {
                            ADCIM_1 |= MASK_SS0;                         // enable interrupt from sequencer
                        }
                        ADCACTSS_1 |= ASEN0;                             // enable sequencer operation
                    }
    #else
                    RCGC0 |= CGC_SARADC0;                                // enable clocks to ADC module
                    RCGC0 |= CGC_SARADC0;                                // {33} enable clocks to ADC module (slow down slightly so that the ADC is ready for the first access)
                    ADCACTSS_0 &= ~ASEN0;                                // ensure sequencer is disabled before configuring
                    ADCEMUX_0 &= ~SS0_MASK;                              // clear sequencer mode
                    if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING | ADC_TRIGGER_COMPARATOR_0 | ADC_TRIGGER_COMPARATOR_1 | ADC_TRIGGER_COMPARATOR_2 | ADC_TRIGGER_TIMER | ADC_TRIGGER_PWM_0 | ADC_TRIGGER_PWM_1 | ADC_TRIGGER_PWM_2 | ADC_TRIGGER_PWM_3)) {
                        if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING)) {
                            INTERRUPT_SETUP interrupt_setup;             // interrupt configuration parameters
                            interrupt_setup.int_type = PORT_INTERRUPT;   // identifier when configuring
                            interrupt_setup.int_handler = 0;             // no specific handling function
                            switch (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING)) {
                            case ADC_TRIGGER_GPIO_PB4_RISING:
                                interrupt_setup.int_port_sense = IRQ_RISING_EDGE; // trigger on rising edge
                                break;
                            case ADC_TRIGGER_GPIO_PB4_FALLING:
                                interrupt_setup.int_port_sense = IRQ_FALLING_EDGE; // trigger on falling edge
                                break;
                            case (ADC_TRIGGER_GPIO_PB4_FALLING | ADC_TRIGGER_GPIO_PB4_RISING):
                                interrupt_setup.int_port_sense = IRQ_BOTH_EDGES; // trigger on both edges
                                break;
                            }
                            interrupt_setup.int_port = PORTB;            // the port used
                            interrupt_setup.int_priority = PORTB_INTERRUPT_PRIORITY; // port interrupt priority
                            interrupt_setup.int_port_bit = PORTB_BIT4;   // the input connected
                            interrupt_setup.int_port_characteristic = SET_ADC_TRIGGER; // don't change configuration
                            fnConfigureInterrupt(&interrupt_setup);      // configure PB4 accordingly
                            ADCEMUX_0 |= TRIG_GPIO_PB4;                  // trigger on PB4 input
                        }
                        else if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_COMPARATOR_0 | ADC_TRIGGER_COMPARATOR_1 | ADC_TRIGGER_COMPARATOR_2)) {
                            switch (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_COMPARATOR_0 | ADC_TRIGGER_COMPARATOR_1 | ADC_TRIGGER_COMPARATOR_2)) {
                            case ADC_TRIGGER_COMPARATOR_0:
                                ADCEMUX_0 |= TRIG_AN_COMP_0;
                                break;
                            case ADC_TRIGGER_COMPARATOR_1:
                                ADCEMUX_0 |= TRIG_AN_COMP_1;
                                break;
                            case ADC_TRIGGER_COMPARATOR_2:
                                ADCEMUX_0 |= TRIG_AN_COMP_2;
                                break;
                            }
                        }
                        else if (ptrADC_settings->int_adc_mode & ADC_TRIGGER_TIMER) {
                            ADCEMUX_0 |= TRIG_TIMER;                     // trigger from any enabled timer source
                        }
                        else if (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_PWM_0 | ADC_TRIGGER_PWM_1 | ADC_TRIGGER_PWM_2 | ADC_TRIGGER_PWM_3)) {
                            switch (ptrADC_settings->int_adc_mode & (ADC_TRIGGER_PWM_0 | ADC_TRIGGER_PWM_1 | ADC_TRIGGER_PWM_2 | ADC_TRIGGER_PWM_3)) {
                            case ADC_TRIGGER_PWM_0:
                                ADCEMUX_0 |= TRIG_PMW_0;
                                break;
                            case ADC_TRIGGER_PWM_1:
                                ADCEMUX_0 |= TRIG_PMW_1;
                                break;
                            case ADC_TRIGGER_PWM_2:
                                ADCEMUX_0 |= TRIG_PMW_2;
                                break;
                            case ADC_TRIGGER_PWM_3:
                                ADCEMUX_0 |= TRIG_PMW_3;
                                break;
                            }
                        }                        
                    }
                    else {
                        ADCEMUX_0 |= TRIG_CONTROLLER;                    // started by software
                    }
                    ADCSAC_0 = ptrADC_settings->int_adc_averaging;
                    while (ulBit <= 0x80) {                              // count the number of inputs to be sampled
                        if (ulBit & (ulSingInputs | ulDifInputs)) {
                            if (ulBit & ulDifInputs) {
                                if (!(ulInputNumber & 0x1)) {            // enter only one of the differential pairs
                                    ulControl |= (DIF_SAMPLE_1 << (iInputs * 4)); // mark that this belongs to a differential input pair
                                    ulConfig |= (ulInputNumber/2 << (iInputs * 4)); // enter the differential pair in the sampling sequence
                                    iInputs++;
                                }
                            }
                            else {
                                if (ulSingInputs & (ulBit << 16)) {      // {14} the channel is from the internal temperature
                                    ulInternalTemp |= (TEMP_SAMPLE_1 << (iInputs * 4));
                                }
                                else {
                                    ulConfig |= (ulInputNumber << (iInputs * 4)); // enter the channel in the sampling sequence
                                }
                                iInputs++;
                            }
        #if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS
                            if (ulInputNumber > 3) {
                                RCGC2 |= CGC_GPIOD;                      // clock the appropriate GPIO module
                                GPIOAMSEL_D |= (0x01 << (7 - (ulInputNumber - 4)));  // disable analogue isolation on pin
                            }
                            else {
                                RCGC2 |= CGC_GPIOE;
                                GPIOAMSEL_E |= (0x01 << (7 - ulInputNumber));
                            }
        #endif
                        }
        #if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS
                        else {
                            if (ulInputNumber > 3) {
                                if (RCGC2 & CGC_GPIOD) {
                                    GPIOAMSEL_D &= ~(0x01 << (7 - (ulInputNumber - 4))); // enable analogue isolation on pin
                                }
                            }
                            else {
                                if (RCGC2 & CGC_GPIOE) {
                                    GPIOAMSEL_E &= ~(0x01 << (7 - ulInputNumber));
                                }
                            }
                        }
        #endif
                        ulBit <<= 1;
                        ulInputNumber++;
                    }
                    if (iInputs == 0) {
                        return;                                          // no inputs found
                    }
                    iInputs--;
                    ulControl |= (((END_SAMPLE_1 | INT_SAMPLE_1) << (iInputs * 4)) | ulInternalTemp); // {14} mark last sample which will interrupt 
                    ADCSSMUX0_0 = ulConfig;                              // program the sequencer
                    ADCSSCTL0_0 = ulControl;

                    fnEnterInterrupt(irq_ADC_Sequence_0_ID, ptrADC_settings->int_priority, _ADC_sequence_0_complete); // enter the interrupt
                    adc_int_complete_handler = ptrADC_settings->int_handler;

                    usSampleCount = ptrADC_settings->int_sequence_count;
                    ptrSamples = ptrADC_settings->int_adc_result;
                    if (adc_int_complete_handler != 0) {                 // {22}
                        ADCIM_0 |= MASK_SS0;                             // enable interrupt from sequencer
                    }
                    ADCACTSS_0 |= ASEN0;                                 // enable sequencer operation
    #endif
                }
                if (ptrADC_settings->int_adc_mode & ADC_START_OPERATION) {
    #ifdef _TEMPEST_CLASS
                    if ((ulDifInputs | ulSingInputs) & (ADC_CHANNEL_0 | ADC_CHANNEL_1 | ADC_CHANNEL_2 | ADC_CHANNEL_3 | ADC_CHANNEL_4 | ADC_CHANNEL_5 | ADC_CHANNEL_6 | ADC_CHANNEL_7)) {
                        ADCPSSI_0 = INITIATE_SS0;                        // trigger sampling of ADC0 by software
                    }
                    else if ((ulDifInputs | ulSingInputs) & (ADC_CHANNEL_8 | ADC_CHANNEL_9 | ADC_CHANNEL_10 | ADC_CHANNEL_11 | ADC_CHANNEL_12 | ADC_CHANNEL_13 | ADC_CHANNEL_14 | ADC_CHANNEL_15)) {
                        ADCPSSI_1 = INITIATE_SS0;                        // trigger sampling of ADC1 by software
                    }
    #else
                    ADCPSSI_0 = INITIATE_SS0;                            // trigger sampling by software
    #endif
                }
            }
        }
        break;
#endif
#ifdef SUPPORT_PWM_CONTROLLER                                            // {32}
    case PWM_CONFIGURATION:                                              // configure PWM controller
        {
            int iTimer = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_reference); // the PWM channel to use
            unsigned short usMode = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_mode);
            if (iTimer >= (PWM_AVAILABLE * 2)) {                         // check for valid PWM controller channel
                return;
            }
            if (usMode & TIMER_STOP_PWM) {                               // PWM is to be stopped
                PWMENABLE &= ~(usMode & (TIMER_PWM_START_0 | TIMER_PWM_START_1 | TIMER_PWM_START_2 | TIMER_PWM_START_3 | TIMER_PWM_START_4 | TIMER_PWM_START_5 | TIMER_PWM_START_6 | TIMER_PWM_START_7)); // disable outputs
                if ((usMode & (TIMER_PWM_START_0 | TIMER_PWM_START_1)) && (!(PWMENABLE & (TIMER_PWM_START_0 | TIMER_PWM_START_1)))) {
                    PWM0CTL = 0;
                }
                if ((usMode & (TIMER_PWM_START_2 | TIMER_PWM_START_3)) && (!(PWMENABLE & (TIMER_PWM_START_2 | TIMER_PWM_START_3)))) {
                    PWM1CTL = 0;
                }
                if ((usMode & (TIMER_PWM_START_4 | TIMER_PWM_START_5)) && (!(PWMENABLE & (TIMER_PWM_START_4 | TIMER_PWM_START_5)))) {
                    PWM2CTL = 0;
                }
                if ((usMode & (TIMER_PWM_START_6 | TIMER_PWM_START_7)) && (!(PWMENABLE & (TIMER_PWM_START_6 | TIMER_PWM_START_7)))) {
                    PWM3CTL = 0;
                }
                if (!(PWMENABLE & (TIMER_PWM_START_0 | TIMER_PWM_START_1 | TIMER_PWM_START_2 | TIMER_PWM_START_3 | TIMER_PWM_START_4 | TIMER_PWM_START_5 | TIMER_PWM_START_6 | TIMER_PWM_START_7))) { // no more channels are active
                    RCGC0 &= ~CGC_PWM;                                   // power down the PWM controller
                }
                return;
            }
            if (!(TIMER_PWM_NO_CONFIG & usMode)) {                       // if configuration is not expressly disabled
                if (usMode & PWM_DIV_64) {                               // PWM clock is to be divided from system clock
                    unsigned long ulDivide = ((usMode & PWM_DIV_64) - PWM_DIV_2);
                    ulDivide <<= 9;
                    ulDivide |= USEPWMDIV;
                    RCC = ((RCC & ~(PWMDIV_64)) | ulDivide);             // program the divide value
                }
                else {
                    RCC &= ~(USEPWMDIV);                                 // no PWM divide
                }
                switch (iTimer) {                                        // configure the PWM channel output
                case 0:                                                  // PWM0
    #ifdef _LM3S1776
                    _CONFIG_PERIPHERAL(D, PWM, 0, (PORTD_BIT0), (PD0_PWM0)); // PWM0 on PD0 - configure port and power up module
    #else
                    _CONFIG_PERIPHERAL(F, PWM, 0, (PORTF_BIT0), (PF0_PWM0)); // PWM0 on PF0
    #endif
                    PWM0GENA = (ACT_CMPAD_SET | ACT_LOAD_CLEAR);         // clear A output when counter value matches the load value and set output when counter matches comparator A
                    PWM0LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM0CMPA = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 1:                                                  // PWM1
    #ifdef _LM3S1776
                    _CONFIG_PERIPHERAL(D, PWM, 0, (PORTD_BIT1), (PD1_PWM1)); // PWM1 on PD1 - configure port and power up module
    #else
                    _CONFIG_PERIPHERAL(G, PWM, 0, (PORTG_BIT1), (PG1_PWM1)); // PWM1 on PG1
    #endif
                    PWM0GENB = (ACT_CMPBD_SET | ACT_LOAD_CLEAR);         // clear B output when counter value matches the load value and set output when counter matches comparator B
                    PWM0LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM0CMPB = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 2:                                                  // PWM2
                    _CONFIG_PERIPHERAL(B, PWM, 0, (PORTB_BIT0), (PB0_PWM2)); // PWM2 on PB0
                    PWM1GENA = (ACT_CMPAD_SET | ACT_LOAD_CLEAR);         // clear A output when counter value matches the load value and set output when counter matches comparator A
                    PWM1LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM1CMPA = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 3:                                                  // PWM3
                    _CONFIG_PERIPHERAL(B, PWM, 0, (PORTB_BIT1), (PB1_PWM3)); // PWM3 on PB1
                    PWM1GENB = (ACT_CMPBD_SET | ACT_LOAD_CLEAR);         // clear B output when counter value matches the load value and set output when counter matches comparator B
                    PWM1LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM1CMPB = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 4:                                                  // PWM4
    #ifdef _LM3S1776
                    _CONFIG_PERIPHERAL(A, PWM, 0, (PORTA_BIT6), (PA6_PWM4)); // PWM4 on PA6 - configure port and power up module
    #else
                    _CONFIG_PERIPHERAL(E, PWM, 0, (PORTE_BIT0), (PE0_PWM4)); // PWM4 on PE0
    #endif
                    PWM2GENA = (ACT_CMPAD_SET | ACT_LOAD_CLEAR);         // clear A output when counter value matches the load value and set output when counter matches comparator A
                    PWM2LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM2CMPA = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 5:                                                  // PWM5
    #ifdef _LM3S1776
                    _CONFIG_PERIPHERAL(A, PWM, 0, (PORTA_BIT7), (PA7_PWM5)); // PWM5 on PA7 - configure port and power up module
    #else
                    _CONFIG_PERIPHERAL(E, PWM, 0, (PORTE_BIT1), (PE1_PWM5)); // PWM5 on PE1
    #endif
                    PWM2GENB = (ACT_CMPBD_SET | ACT_LOAD_CLEAR);         // clear B output when counter value matches the load value and set output when counter matches comparator B
                    PWM2LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM2CMPB = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 6:                                                  // PWM6
    #ifdef _LM3S1776
                    _CONFIG_PERIPHERAL(C, PWM, 0, (PORTC_BIT4), (PC4_PWM6)); // PWM6 on PC4 - configure port and power up module
    #else
    #endif
                    PWM3GENA = (ACT_CMPAD_SET | ACT_LOAD_CLEAR);         // clear A output when counter value matches the load value and set output when counter matches comparator A
                    PWM3LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM3CMPA = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                case 7:                                                  // PWM7
    #ifdef _LM3S1776
                    _CONFIG_PERIPHERAL(C, PWM, 0, (PORTC_BIT6), (PC6_PWM7)); // PWM7 on PC6 - configure port and power up module
    #else
    #endif
                    PWM3GENB = (ACT_CMPBD_SET | ACT_LOAD_CLEAR);         // clear B output when counter value matches the load value and set output when counter matches comparator B
                    PWM3LOAD = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value;
                    PWM3CMPB = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value;
                    break;
                }
            }
            if (usMode & (TIMER_PWM_START_0 | TIMER_PWM_START_1)) {
                PWM0CTL = PWM_BLOCK_ENABLE;
            }
            if (usMode & (TIMER_PWM_START_2 | TIMER_PWM_START_3)) {
                PWM1CTL = PWM_BLOCK_ENABLE;
            }
            if (usMode & (TIMER_PWM_START_4 | TIMER_PWM_START_5)) {
                PWM2CTL = PWM_BLOCK_ENABLE;
            }
            if (usMode & (TIMER_PWM_START_6 | TIMER_PWM_START_7)) {
                PWM3CTL = PWM_BLOCK_ENABLE;
            }
            PWMENABLE |= (usMode & (TIMER_PWM_START_0 | TIMER_PWM_START_1 | TIMER_PWM_START_2 | TIMER_PWM_START_3 | TIMER_PWM_START_4 | TIMER_PWM_START_5 | TIMER_PWM_START_6 | TIMER_PWM_START_7)); // enable outputs
        }
        break;
#endif
    }
}

#if defined SUPPORT_LOW_POWER
// This routine switches to low power mode. It is always called with disabled interrupts
//
extern void fnDoLowPower(void)
{
    #if !defined _WINDOWS
    uEnable_Interrupt();
    __sleep_mode();                                                      // enter low power mode using wait for interrupt processor state
    #else
    uEnable_Interrupt();
    #endif
}

// Set a new low power mode (not yet implemented)
//
extern void fnSetLowPowerMode(int new_lp_mode)
{
}
#endif


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
  //HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ; other method?
    RCGC0 |= CGC_WDT;                                                    // enable clocks to the watchdog module, if not presently active
    WDTLOCK = WD_ACCESS;                                                 // ensure we can access the watchdog registers
    WDTLOAD = 0;                                                         // immediate timeout
    WDTCTL = WD_RESEN;                                                   // enable reset on second timeout
    WDTCTL = (WD_RESEN | WD_INTEN);                                      // enable watchdog and interrupt
#ifndef _WINDOWS
    while (1) {}                                                         // wait for watchdog to fire
#endif
}


#ifdef GLOBAL_HARDWARE_TIMER

static __interrupt void hwtimer_interrupt(void)
{
    static const unsigned char hw_timer_int_message[HEADER_LENGTH] = { INTERNAL_ROUTE, INTERNAL_ROUTE, TASK_TIMER, INTERRUPT_EVENT, 0 };
    volatile unsigned int dummy;

    dummy = TC_SR_2;                                                     // read the status to reset interrupt
    TC_IDR_2 = TIM_CPCS;                                                 // disable interrupt
#ifdef _WINDOWS
    TC_SR_2 &= ~TIM_CLKSTA; TC_IMR_2 &= ~TIM_CPCS;                       // flag that the timer is not running
#endif
    uDisable_Interrupt();                                                // ensure interrupts remain blocked when putting message to queue
    fnWrite(INTERNAL_ROUTE, (unsigned char*)hw_timer_int_message, HEADER_LENGTH); // Inform the global interrupt task
    uEnable_Interrupt();                                                 // release
}

// In order to support a global hardware timer a milli-second timer is required. We use the 16 bit Timer 2
//
extern CLOCK_LIMIT fnSetHardwareTimer(CLOCK_LIMIT *hw_delay)
{                                                                        // timer set in ms
    CLOCK_LIMIT Adjusted = 0;
    CLOCK_LIMIT NewDelay = *hw_delay;                                    // clock count desired

    uDisable_Interrupt();                                                // protect region against interrupts

    if (TC_IMR_2 & TIM_CPCS) {                                           // timer already running
        CLOCK_LIMIT Remaining = (CLOCK_LIMIT)(TC_RC_2 - TC_CV_2);        // time remaining before next timeout (in clock pulses)
        if (NewDelay >= Remaining) {
            uEnable_Interrupt();                                         // enable interrupt and quit
            return 0;
        }
        TC_CCR_2 = TIM_CLKDIS;                                           // freeze the timer and disable interrupt
        Adjusted = (CLOCK_LIMIT)TC_CV_2;                                 // note the frozen value
    }
    TC_RC_2 = NewDelay;                                                  // set reference
    TC_CCR_2 = (TIM_CLKEN | TIM_SWTRG);                                  // (re)start timer
    TC_IER_2  = TIM_CPCS;                                                // enable interrupt on RC Compare Interrupt
#ifdef _WINDOWS
    TC_SR_2 |= TIM_CLKSTA; TC_IMR_2 |= TIM_CPCS;                         // flag that the timer is running
#endif

    uEnable_Interrupt();                                                 // enable interrupts again

    return Adjusted;
}

#endif

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

// Delay loop for simple but accurate short delays (eg. for stabilisation delays)
//
extern void fnDelayLoop(unsigned long ulDelay_us)
{
    #define LOOP_FACTOR 9938400                                          // tuned for GCC on Fury/Dust-Devil Class devices
    volatile unsigned long ulDelay = ((MASTER_CLOCK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay--) {}                                                 // simple loop tuned to perform us timing
}



#if defined (COMPILE_IAR) && !defined COMPILE_IAR5

#pragma segment="DATA_ID"
#pragma segment="DATA_I"
#pragma segment="DATA_Z"

static void __segment_init(void)
{
    unsigned long *pulSrc, *pulDest, *pulEnd;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = __segment_begin("DATA_ID");
    pulDest = __segment_begin("DATA_I");
    pulEnd = __segment_end("DATA_I");
    while(pulDest < pulEnd)
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    pulDest = __segment_begin("DATA_Z");
    pulEnd = __segment_end("DATA_Z");
    while(pulDest < pulEnd)
    {
        *pulDest++ = 0;
    }
    }
#elif defined (_GNU)
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
    ulInitDataLength = (&__text_end__ - &__text_start__);
    while (ulInitDataLength--) {                                         // initialise text
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif

#if 1 /*defined RUN_IN_FREE_RTOS || defined _WINDOWS*/ // to satisfy FreeRTOS callbacks - even when FreeRTOS not linked
extern void *pvPortMalloc(int iSize)
{
    return uMalloc((MAX_MALLOC)iSize);                                   // use uMalloc() which assumes that memory is never freed
}
extern void vPortFree(void *free)
{
    _EXCEPTION("Unexpected free call!!");
}
extern void vApplicationStackOverflowHook(void)
{
}
extern void vApplicationTickHook(void)                                   // FreeRTOS tick interrupt
{
}
extern void vMainConfigureTimerForRunTimeStats(void)
{
}
extern unsigned long ulMainGetRunTimeCounterValue(void)
{
    return uTaskerSystemTick;
}
extern void vApplicationIdleHook(void)
{
}
extern void prvSetupHardware(void)
{
}
#endif


// Perform very low level initialisation - called by the start up code
//
static void LM3SXXXX_LowLevelInit( void)
{
    VECTOR_TABLE *ptrVect;
#if !defined _MINIMUM_IRQ_INITIALISATION
    void ( **processor_ints )( void );
#endif

    RCC &= ~USESYSDIV;
    RCC |= BYPASS;                                                       // ensure clocking is from raw clock source
    RCC &= ~(XTAL_8_1920 | OSCSRC_MASK | SYSDIV_32);                     // mask out XTAL frequency and divider fields and set main oscillator source
    #ifdef EXTENDED_PLL_SETTING                                          // {17}
    RCC2 |= (USERCC2 | DIV400 | BYPASS);                                 // control using RCC2
    RCC2 &= ~(XTAL_8_1920 | OSCSRC_MASK | SYSDIV_32 | SYSDIV2LSB);       // note that SYSDIV2LSB is disabled here since it can only be disabled after DIV400 has been set. There is never a need to enable it since the non-extended method can be used to achieve the standard frequencies
    RCC2 |= (PLL_OUTPUT_VALUE);                                          // select crystal and required PLL output frequency
    RCC |= (CRYSTAL_SPEED_VALUE);
    RCC2 &= ~(PWRDN);                                                    // clear PLL power down bit to allow it to operate
    #else
    RCC |= (CRYSTAL_SPEED_VALUE | PLL_OUTPUT_VALUE);                     // select crystal and required PLL output frequency
    RCC &= ~(PWRDN);                                                     // clear PLL power down bit to allow it to operate
    RCC |= USESYSDIV;                                                    // enable system divide
    #endif

    while (!(RIS & PLLLRIS)) {                                           // wait for PLL to lock
#if defined _WINDOWS
        RIS |= PLLLRIS;
#endif
    }
    #ifdef EXTENDED_PLL_SETTING
    RCC2 &= ~BYPASS;                                                     // switch to PLL source
    #else
    RCC &= ~BYPASS;                                                      // switch to PLL source
    #endif
    #ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram + RESERVE_DMA_MEMORY));
    #else
        #ifdef _CCSTUDIO                                                 // {34}
    ptrVect = (VECTOR_TABLE *)(vectorTable + RESERVE_DMA_MEMORY);        // this is the same as (RAM_START_ADDRESS + RESERVE_DMA_MEMORY) but forces the area to be linked
        #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS + RESERVE_DMA_MEMORY);
        #endif
    #endif
    VECTOR_TABLE_OFFSET_REG   = (TBLBASE_IN_RAM + (RESERVE_DMA_MEMORY & ~0x7f));// position the vector table at the bottom of RAM
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
#if RESERVE_DMA_MEMORY > 0                                               // uDMA is activated so initialise the controller
    RCGC2 = CGC_uDMA;                                                    // clock the module
#endif
#if defined (COMPILE_IAR) && !defined COMPILE_IAR5
    __segment_init();                                                    // initialise variables
#elif defined (_GNU)
    __init_gnu_data();
#endif
#if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS                  // {8}
    GPIOHSCTL = 0xffffffff;                                              // all possible ports set to use high speed aperture
#endif
#if RESERVE_DMA_MEMORY > 0                                               // uDMA is activated so initialise the controller
    DMACFG = uDMA_MASTEN;                                                // enable the module
    DMACTLBASE = RAM_START_ADDRESS;                                      // enter the DMA control table with a 1024 byte aligned address
    #ifdef _WINDOWS
    DMAALTBASE += RAM_START_ADDRESS;
    #endif
#endif
#if defined _WINDOWS                                                     // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        *(unsigned char *)0 = 0;
    }
#endif
#if defined _WINDOWS
    fnUpdateOperatingDetails();                                          // {36} update operating details to be displayed in the simulator
#endif
}


#if defined ROWLEY && !defined ROWLEY_2                                  // Rowley project requires extra initialisation for debugger to work correctly before V1.7 build 17
static void _main2(void)
{
    asm("mov lr, #0");
    asm("b _main");
}
#endif

#if defined _GNU
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    asm("ldr sp, [r0,#0]");
    asm("ldr pc, [r0,#4]");
}
#endif


// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR5
__root const RESET_VECTOR __vector_table @ ".intvec"                     // __root forces the function to be linked in IAR project
#elif defined COMPILE_IAR
__root const RESET_VECTOR reset_vect @ "RESETVECT"                       // __root forces the function to be linked in IAR project
#elif defined _GNU || defined _CCSTUDIO
const RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
__attribute__((section("RESET"))) const RESET_VECTOR reset_vect
#else
const RESET_VECTOR reset_vect
#endif
= {
    (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - 8),                       // stack pointer to top of RAM (leaving space for non-initialised random number)
    (void (*)(void))START_CODE
};

#endif                                                                   // end _LM3SXXXX

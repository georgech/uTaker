/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland
    
    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      iMX.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS
    #include "config.h"
    extern void fnUpdateOperatingDetails(void);
    #define INITHW  extern
    extern void fec_txf_isr(void);
    extern void fnSimulateDMA(int channel, unsigned char ucTriggerSource);
    #define __disable_interrupt()
    #define __enable_interrupt()
    #define START_CODE 0
    #if defined MMDVSQ_AVAILABLE
        #include <math.h>
    #endif
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #define  fnSimulateDMA(x, y)
    #if defined _COMPILE_KEIL
        extern void __main(void);
        #define START_CODE _init
    #elif defined _COMPILE_IAR
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE disable_watchdog
    #elif defined _COMPILE_GHS
        extern void _start_T(void);                                      // GHS library initialisation routine
        #define START_CODE disable_watchdog
    #else
        #define START_CODE main
    #endif
#endif

#if defined _iMX

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define USER_STARTUP _LowLevelInit

#if defined _COMPILE_COSMIC
    #define __interrupt @interrupt
#else
    #define __interrupt
#endif

#if defined _APPLICATION_VALIDATION
    #define _RESET_VECTOR  RESET_VECTOR_VALIDATION
#elif defined INTERRUPT_VECTORS_IN_FLASH
    #define _RESET_VECTOR   VECTOR_TABLE
    const _RESET_VECTOR __vector_table;
#else
    #define _RESET_VECTOR  RESET_VECTOR
#endif

#define UART0_TX_CLK_REQUIRED 0x00000001
#define UART1_TX_CLK_REQUIRED 0x00000002
#define UART2_TX_CLK_REQUIRED 0x00000004
#define UART3_TX_CLK_REQUIRED 0x00000008
#define UART4_TX_CLK_REQUIRED 0x00000010
#define UART5_TX_CLK_REQUIRED 0x00000020
#define UART6_TX_CLK_REQUIRED 0x00000040
#define UART7_TX_CLK_REQUIRED 0x00000080
#define UART0_RX_CLK_REQUIRED 0x00000100
#define UART1_RX_CLK_REQUIRED 0x00000200
#define UART2_RX_CLK_REQUIRED 0x00000400
#define UART3_RX_CLK_REQUIRED 0x00000800
#define UART4_RX_CLK_REQUIRED 0x00001000
#define UART5_RX_CLK_REQUIRED 0x00002000
#define UART6_RX_CLK_REQUIRED 0x00004000
#define UART7_RX_CLK_REQUIRED 0x00008000


/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */

#if defined RUN_IN_FREE_RTOS
    extern void xPortPendSVHandler(void);                                // PendSV interrupt handler
    extern void vPortSVCHandler(void);                                   // SCV interrupt handler
    extern void xPortSysTickHandler(void);                               // FreeRTOS tick handler callback
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void _LowLevelInit(void);

#if defined SUPPORT_LOW_POWER
    static int fnPresentLP_mode(void);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if !defined ONLY_INTERNAL_FLASH_STORAGE
    static const STORAGE_AREA_ENTRY default_flash = {
        0,                                                               // end of list
        (unsigned char *)(FLASH_START_ADDRESS),                          // start address of internal flash
        (unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH - 1)),    // end of internal flash
        _STORAGE_INTERNAL_FLASH,                                         // type
        0                                                                // not multiple devices
    };

    STORAGE_AREA_ENTRY *UserStorageListPtr = (STORAGE_AREA_ENTRY *)&default_flash; // default entry
#endif

#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K && !defined SPI_FLASH_MX25L
        #define SPI_FLASH_ATMEL                                          // default if not otherwise defined
    #endif
    #define _SPI_DEFINES
        #include "spi_flash_iMX_atmel.h"
        #include "spi_flash_iMX_stmicro.h"
        #include "spi_flash_iMX_sst25.h"
        #include "spi_flash_iMX_w25q.h"
        #include "spi_flash_iMX_s25fl1-k.h"
        #include "spi_flash_iMX_MX25L.h"
    #undef _SPI_DEFINES
#endif
#if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define _SPI_EEPROM_DEFINES
        #include "spi_eeprom_iMX_25AA160.h"
    #undef _SPI_EEPROM_DEFINES
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static int iInterruptLevel = 0;                                          // present level of disable nesting

#if defined SUPPORT_LOW_POWER
    static unsigned long ulPeripheralNeedsClock = 0;                     // stop mode is blocked if a peripheral is in use that needs a clock that will be stopped
#endif
#if defined RANDOM_NUMBER_GENERATOR && !defined RND_HW_SUPPORT
    unsigned short *ptrSeed;
#endif
#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
    static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)] = {0}; // FLASH row backup buffer (on long word boundary)
#endif
#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static unsigned long SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0}; // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT] = {0}; // list of attached FLASH devices

    #if defined SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #if defined CS2_LINE
            ,CS2_LINE
            #if defined CS3_LINE
            ,CS3_LINE
            #endif
        #endif
        };
        #define EXTENDED_CS , &iChipSelect
        #define _EXTENDED_CS  ptrAccessDetails->ucDeviceNumber,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif
#if (defined SPI_EEPROM_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_EEPROM_DEVICE_COUNT
        #define SPI_EEPROM_DEVICE_COUNT 1
    #endif
    static unsigned long SPI_EEPROM_Danger[SPI_EEPROM_DEVICE_COUNT] = {0}; // signal that the FLASH status should be checked before using since there is a danger that it is still busy
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined _WINDOWS                              
    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
    #if defined SERIAL_INTERFACE
        extern unsigned char ucTxLast[NUMBER_SERIAL] = {0};
    #endif
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#if !defined _WINDOWS
    static void fnInitHW(void);
#endif


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

#if !defined _WINDOWS
    extern __interrupt void _start(void);                                // reset vector location
#endif

#define _SPI_FLASH_INTERFACE                                             // insert manufacturer dependent SPI Flash driver code
    #include "spi_flash_iMX_atmel.h"
    #include "spi_flash_iMX_stmicro.h"
    #include "spi_flash_iMX_sst25.h"
    #include "spi_flash_iMX_w25q.h"
    #include "spi_flash_iMX_s25fl1-k.h"
    #include "spi_flash_iMX_MX25L.h"
#undef _SPI_FLASH_INTERFACE
#define _SPI_EEPROM_INTERFACE
    #include "spi_eeprom_iMX_25AA160.h"
#undef _SPI_EEPROM_INTERFACE


/* =================================================================== */
/*                            STARTUP CODE                             */
/* =================================================================== */

static void fnConfigWdogs(void)
{
    // The watchdog power down counter is enabled out of reset and fires after 16s if not stopped here
    //
    WDOG1_WMCR = 0;                                                      // stop power down watchdog
    WDOG2_WMCR = 0;

    // We presently use WDOG3 only
    //
    WDOG1_WCR = (WDOG_WCR_WDA | WDOG_WCR_SRS);                           // disable watchdog timer 1
    WDOG2_WCR = (WDOG_WCR_WDA | WDOG_WCR_SRS);                           // disable watchdog timer 2

    INIT_WATCHDOG_DISABLE();                                             // configure an input used to control watchdog operation
    if (WATCHDOG_DISABLE() == 0) {                                       // if the watchdog disable input is not active
        ACTIVATE_WATCHDOG();                                             // allow user configuration of internal watchdog timer
    }
    else {                                                               // disable the watchdog
        UNLOCK_WDOG3();                                                  // open a window to write to watchdog 3
        WDOG3_CS = WDOG_CS_UPDATE;                                       // disable watchdog but allow updates
        WDOG3_TOVAL = 0xffff;
        WDOG3_WIN = 0;
    }
}

static void fnDisable_iMX_RT_Clocks(void)
{
#if defined iMX_RT106X
    CCM_CCGR0 = (CCM_CCGR0_GPIO2_CLOCK_OFF | CCM_CCGR0_LPUART2_CLOCK_OFF | CCM_CCGR0_LPUART3_CLOCK_OFF | CCM_CCGR0_GPT2_SERIAL_CLOCKS_OFF | CCM_CCGR0_GPT2_BUS_CLOCKS_OFF | CCM_CCGR0_TRACE_CLOCK_OFF | CCM_CCGR0_CAN2_SERIAL_CLOCK_OFF | CCM_CCGR0_CAN2_CLOCK_OFF | CCM_CCGR0_CAN1_SERIAL_CLOCK_OFF | CCM_CCGR0_CAN1_CLOCK_OFF | CCM_CCGR0_DCP_CLOCK_OFF | CCM_CCGR0_SIM_M_CLK_R_CLK_OFF | CCM_CCGR0_MQS_CLOCK_OFF | CCM_CCGR0_AIPS_TZ2_CLOCK2_OFF | CCM_CCGR0_AIPS_TZ1_CLOCK2_OFF);
#else
    CCM_CCGR0 = (CCM_CCGR0_GPIO2_CLOCK_OFF | CCM_CCGR0_LPUART2_CLOCK_OFF | CCM_CCGR0_LPUART3_CLOCK_OFF | CCM_CCGR0_FLEXSPI_EXSC_CLOCK_OFF | CCM_CCGR0_GPT2_SERIAL_CLOCKS_OFF | CCM_CCGR0_GPT2_BUS_CLOCKS_OFF | CCM_CCGR0_TRACE_CLOCK_OFF | CCM_CCGR0_CAN2_SERIAL_CLOCK_OFF | CCM_CCGR0_CAN2_CLOCK_OFF | CCM_CCGR0_CAN1_SERIAL_CLOCK_OFF | CCM_CCGR0_CAN1_CLOCK_OFF | CCM_CCGR0_DCP_CLOCK_OFF | CCM_CCGR0_SIM_M_CLK_R_CLK_OFF | CCM_CCGR0_MQS_CLOCK_OFF | CCM_CCGR0_AIPS_TZ2_CLOCK2_OFF | CCM_CCGR0_AIPS_TZ1_CLOCK2_OFF);
#endif
#if defined iMX_RT106X
    CCM_CCGR1 = (CCM_CCGR1_AOI2_CLOCKS_OFF | CCM_CCGR1_CSU_CLOCK_OFF | CCM_CCGR1_GPIO1_CLOCK_OFF | CCM_CCGR1_LPUART4_CLOCK_OFF | CCM_CCGR1_GPT1_SERIAL_CLOCK_OFF | CCM_CCGR1_GPT1_BUS_CLOCK_OFF | CCM_CCGR1_SEMC_EXSC_CLOCK_OFF | CCM_CCGR1_ADC1_CLOCK_OFF | CCM_CCGR1_PIT_CLOCKS_OFF | CCM_CCGR1_ENET_CLOCK_OFF | CCM_CCGR1_ADC2_CLOCK_OFF | CCM_CCGR1_LPSPI4_CLOCKS_OFF | CCM_CCGR1_LPSPI3_CLOCKS_OFF | CCM_CCGR1_LPSPI2_CLOCKS_OFF | CCM_CCGR1_LPSPI1_CLOCKS_OFF);
#else
    CCM_CCGR1 = (CCM_CCGR1_CSU_CLOCK_OFF | CCM_CCGR1_GPIO1_CLOCK_OFF | CCM_CCGR1_LPUART4_CLOCK_OFF | CCM_CCGR1_GPT1_SERIAL_CLOCK_OFF | CCM_CCGR1_GPT1_BUS_CLOCK_OFF | CCM_CCGR1_SEMC_EXSC_CLOCK_OFF | CCM_CCGR1_ADC1_CLOCK_OFF | CCM_CCGR1_PIT_CLOCKS_OFF | CCM_CCGR1_ENET_CLOCK_OFF | CCM_CCGR1_ADC2_CLOCK_OFF | CCM_CCGR1_LPSPI4_CLOCKS_OFF | CCM_CCGR1_LPSPI3_CLOCKS_OFF | CCM_CCGR1_LPSPI2_CLOCKS_OFF | CCM_CCGR1_LPSPI1_CLOCKS_OFF);
#endif
    CCM_CCGR2 = (CCM_CCGR2_GPIO3_CLOCK_OFF | CCM_CCGR2_XBAR2_CLOCK_OFF | CCM_CCGR2_XBAR1_CLOCK_OFF | CCM_CCGR2_OCOTP_CTRL_CLOCK_OFF | CCM_CCGR2_LPI2C3_CLOCK_OFF | CCM_CCGR2_LPI2C2_CLOCK_OFF | CCM_CCGR2_LPI2C1_CLOCK_OFF | CCM_CCGR2_IMUX_SNVS_CLOCK_OFF | CCM_CCGR2_OCRAM_EXSC_CLOCK_OFF);
#if defined iMX_RT106X
    CCM_CCGR3 = (CCM_CCGR3_GPIO4_CLOCK_OFF | CCM_CCGR3_IMUX_SNVS_GRP_CLOCK_OFF | CCM_CCGR3_OCRAM_CLOCK_OFF | CCM_CCGR3_ACMP4_CLOCKS_OFF | CCM_CCGR3_ACMP3_CLOCKS_OFF | CCM_CCGR3_ACMP2_CLOCKS_OFF | CCM_CCGR3_ACMP2_CLOCKS_STOP | CCM_CCGR3_ACMP1_CLOCKS_OFF | CCM_CCGR3_FLEXRAM1_CLOCK_OFF | CCM_CCGR3_WDOG1_CLOCK_OFF | CCM_CCGR3_EWM_CLOCK_OFF | CCM_CCGR3_LPUART6_CLOCK_OFF | CCM_CCGR3_SEMC_CLOCKS_OFF | CCM_CCGR3_LPUART5_CLOCK_OFF);
#else
    CCM_CCGR3 = (CCM_CCGR3_IMUX_SNVS_GRP_CLOCK_OFF | CCM_CCGR3_OCRAM_CLOCK_OFF | CCM_CCGR3_ACMP4_CLOCKS_OFF | CCM_CCGR3_ACMP3_CLOCKS_OFF | CCM_CCGR3_ACMP2_CLOCKS_OFF | CCM_CCGR3_ACMP2_CLOCKS_STOP | CCM_CCGR3_ACMP1_CLOCKS_OFF | CCM_CCGR3_FLEXRAM1_CLOCK_OFF | CCM_CCGR3_WDOG1_CLOCK_OFF | CCM_CCGR3_EWM_CLOCK_OFF | CCM_CCGR3_AOI1_CLOCK_OFF | CCM_CCGR3_LPUART6_CLOCK_OFF | CCM_CCGR3_SEMC_CLOCKS_OFF | CCM_CCGR3_LPUART5_CLOCK_OFF);
#endif
    CCM_CCGR4 = (CCM_CCGR4_ENC2_CLOCKS_OFF | CCM_CCGR4_ENC1_CLOCKS_OFF | CCM_CCGR4_PWM2_CLOCKS_OFF | CCM_CCGR4_PWM1_CLOCKS_OFF | CCM_CCGR4_SIM_EMS_CLOCKS_OFF | CCM_CCGR4_SIM_M_CLOCKS_OFF | CCM_CCGR4_SIM_M7_CLOCK_OFF | CCM_CCGR4_BEE_CLOCK_OFF | CCM_CCGR4_IOMUX_GRP_CLOCK_OFF | CCM_CCGR4_IOMUX_CLOCK_OFF | CCM_CCGR4_SIM_M7_CLK_R_OFF);
    CCM_CCGR5 = (CCM_CCGR5_SNVS_LP_CLOCK_OFF | CCM_CCGR5_SNVS_HP_CLOCK_OFF | CCM_CCGR5_LPUART7_CLOCK_OFF | CCM_CCGR5_LPUART1_CLOCK_OFF | CCM_CCGR5_SAI3_CLOCK_OFF | CCM_CCGR5_SAI2_CLOCK_OFF | CCM_CCGR5_SAI1_CLOCK_OFF | CCM_CCGR5_SPDIF_CLOCK_OFF | CCM_CCGR5_AIPSTZ4_CLOCKS_OFF | CCM_CCGR5_WDOG2_CLOCK_OFF | CCM_CCGR5_KPP_CLOCK_OFF | CCM_CCGR5_DMA_CLOCK_OFF | CCM_CCGR5_WDOG3_CLOCK_OFF | CCM_CCGR5_FLEXIO1_CLOCK_OFF | CCM_CCGR5_ROM_CLOCK_OFF);
    CCM_CCGR6 = (CCM_CCGR6_TIMER2_CLOCKS_OFF | CCM_CCGR6_TIMER1_CLOCKS_OFF | CCM_CCGR6_LPI2C4_SERIAL_CLOCK_OFF | CCM_CCGR6_ANADIG_CLOCKS_OFF | CCM_CCGR6_SIM_PER_CLOCK_OFF | CCM_CCGR6_AIPS_TZ3_CLOCK_OFF | CCM_CCGR6_LPUART8_CLOCK_OFF | CCM_CCGR6_TRNG_CLOCK_OFF);
}


#if !defined _WINDOWS
    extern void __segment_init(void);
    #if defined  _COMPILE_IAR
        #pragma segment=".data"
        #pragma segment=".bss"
        static unsigned char *ptrTopOfUsedMemory = 0;
        #define HEAP_START_ADDRESS ptrTopOfUsedMemory                    // IAR compiler - last location of static variables
        #define  __sleep_mode() __WFI()                                  // IAR intrinsic
    #elif defined _COMPILE_KEIL
        #define HEAP_START_ADDRESS    _keil_ram_size(0)
        #define __disable_interrupt() __disable_irq()                    // KEIL intrinsics
        #define __enable_interrupt()  __enable_irq()
        #define __sleep_mode()        __wfi()
    #else                                                                // disable interrupt in assembler code
        extern unsigned char __heap_end__;
        #define HEAP_START_ADDRESS &__heap_end__                         // GNU last location of static variables
        #if defined ROWLEY || defined _KDS
            #define asm(x) __asm__(x)
        #elif defined _COMPILE_COSMIC
            #define asm(x) _asm(x)
        #endif
        #if defined _COMPILE_GHS
            #define __disable_interrupt() asm("cpsid i")                 // __DI() intrinsics are not used because they are asm("cpsid if") which doesn't allow correct low power mode operation
            #define __enable_interrupt()  asm("cpsie i")                 // __EI()
            #define __sleep_mode()        asm("wfi")
        #else
            #define __disable_interrupt() asm("cpsid   i")
            #define __enable_interrupt()  asm("cpsie   i")
            #define __sleep_mode()        asm("wfi")
        #endif
    #endif

    #if defined _COMPILE_KEIL

typedef struct stREGION_TABLE
{
    unsigned char *ptrConst;                                             // const variables belonging to block
    unsigned char *ptrDestination;                                       // destination in RAM
    unsigned long  ulLength;                                             // length of the block in SRAM
    unsigned long  ulUnknown;                                            // entry unknown
} REGION_TABLE;

// Calculate the end of used SRAM from the Keil linker information and optionally initialise variables
//
static unsigned char *_keil_ram_size(int iInit)
{
    extern REGION_TABLE Region$$Table$$Base;                            // table location supplied by linker
    extern REGION_TABLE Region$$Table$$Limit;                           // end of table list
    REGION_TABLE *ptrEntries = &Region$$Table$$Base;                    // first block
    unsigned char *ptrRam = ptrEntries->ptrDestination;                 // RAM address
    do {
        if (iInit != 0) {
            if (ptrEntries->ulUnknown == 0x60) {                        // not valid for uVision4
                uMemset(ptrRam, 0, ptrEntries->ulLength);               // zero data
            }
            else {
                uMemcpy(ptrRam, ptrEntries->ptrConst, ptrEntries->ulLength); // intialise data (uVision4 uses compressed data in Flash and so can't be used like this
            }
        }
        ptrRam += ptrEntries->ulLength;                                 // add length
        ptrEntries++;                                                   // move to next block
    } while (ptrEntries != &Region$$Table$$Limit);
    return ptrRam;
}
    #endif

    #if defined _COMPILE_IAR || defined _COMPILE_GHS
// This is the first function called so that it can immediately disable the watchdog so that it doesn't fire during variable initialisation
//
static void disable_watchdog(void)
{
    // Configure the watchdogs
    //
    fnConfigWdogs();
    // Disable all clocks by default - they will be enabled only when needed
    //
    fnDisable_iMX_RT_Clocks();

        #if defined SUPPORT_LOW_POWER && (defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000))
            #if defined SUPPORT_LPTMR                                    // ensure no interrupts pending after waking from VLLS modes via LPTMR
    POWER_UP_ATOMIC(5, LPTMR0);                                          // power up the low power timer
    PMC_REGSC = PMC_REGSC_ACKISO;                                        // acknowledge the isolation mode to set certain peripherals and I/O pads back to normal run state
    LPTMR0_CSR = 0;                                                      // clear possible pending interrupt and stop the timer
    POWER_DOWN_ATOMIC(5, LPTMR0);                                        // power down the low power timer
            #else
    PMC_REGSC = PMC_REGSC_ACKISO;                                        // acknowledge the isolation mode to set certain peripherals and I/O pads back to normal run state
            #endif
        #endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
        #if defined USER_STARTUP_CODE                                    // allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
        #endif
        #if defined _COMPILE_GHS
    _start_T();                                                          // now call the GHS initialisation code which initialises variables and then calls main() 
        #else
    __iar_program_start();                                               // now call the IAR initialisation code which initialises variables and then calls main() 
        #endif
}
    #endif


/* =================================================================== */
/*                                main()                               */
/* =================================================================== */

#if !defined RUN_IN_FREE_RTOS && !defined _WINDOWS
    static void fn_uTasker_main(void);
#elif defined RUN_IN_FREE_RTOS && !defined _WINDOWS
    extern void fnFreeRTOS_main(void);
#endif

// Main entry for the target code
//
extern int main(void)
{
#if defined (_COMPILE_IAR)
    if (__sfe(".bss") > __sfe(".data")) {                                // set last used SRAM address
        ptrTopOfUsedMemory = __sfe(".bss");
    }
    else {
        ptrTopOfUsedMemory = __sfe(".data");
    }
#endif
    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)
#if defined RANDOM_NUMBER_GENERATOR && !defined RND_HW_SUPPORT
    ptrSeed = RANDOM_SEED_LOCATION;
#endif
    fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                     // initialise heap
#if defined RUN_IN_FREE_RTOS
    fnFreeRTOS_main();                                                   // never return in normal situations
    FOREVER_LOOP() {
        // This only happens when there was a failure to initialise and start FreeRTOS (usually not enough heap)
        //
        _EXCEPTION("FreeRTOS failed to initialise");
    }
#else
    fn_uTasker_main();                                                   // never return
#endif
    return 0;                                                            // we never return....
}
#endif                                                                   // end if not _WINDOWS

#if defined RUN_IN_FREE_RTOS
    extern void fn_uTasker_main(void *pars)
#else
    static void fn_uTasker_main(void)
#endif
{
#if !defined _WINDOWS && defined MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart;
    prtInfo = ptMultiStartTable;                                         // if the user has already set to alternative start configuration
    if (prtInfo == 0) {                                                  // no special start required
    _abort_multi:
        fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                 // initialise heap
        uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system (and TICK interrupt)
        while ((prtInfo = (MULTISTART_TABLE *)uTaskerSchedule()) == 0) {}// schedule uTasker
    }

    do {
        pucHeapStart = HEAP_START_ADDRESS;
        if (prtInfo->new_hw_init) {                                      // info to next task configuration available
            pucHeapStart = prtInfo->new_hw_init(JumpTable);              // get heap details from next configuration
            if (0 == pucHeapStart) {
                goto _abort_multi;                                       // this can happen if the jump table version doesn't match - prefer to stay in boot mode than start an application which will crash
            }
        }

        fnInitialiseHeap(prtInfo->ptHeapNeed, pucHeapStart);             // initialise the new heap
                                                                         // start the operating system with next configuration
        uTaskerStart((UTASKTABLEINIT *)prtInfo->ptTaskTable, prtInfo->ptNodesTable, PHYSICAL_QUEUES);
        while ((prtInfo = (MULTISTART_TABLE *)uTaskerSchedule()) == 0) {}// schedule uTasker

    } FOREVER_LOOP();
#elif !defined _WINDOWS || defined RUN_IN_FREE_RTOS
  //fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                     // initialise heap
    uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system (and TICK interrupt)
    FOREVER_LOOP() {
        uTaskerSchedule();                                               // schedule uTasker
    }
#endif
}

#if defined _COMPILE_KEIL
// Keil demands the use of a __main() call to correctly initialise variables - it then calls main()
//
extern void _init(void)
{
    _LowLevelInit();                                                     // configure watchdog and set the CPU to operating speed
    __main();                                                            // Keil initialises variables and then calls main()
}
#endif


#if defined RANDOM_NUMBER_GENERATOR
    #if defined RND_HW_SUPPORT
extern void fnInitialiseRND(unsigned short *usSeedValue)
{
    #if defined RANDOM_NUMBER_GENERATOR_B                                // support revison 1 types
    POWER_UP_ATOMIC(3, RNGB);                                            // power up RNGB
        #if defined RANDOM_NUMBER_GENERATOR_A                            // support revison 2 types too
    if (((SIM_SDID & SIM_SDID_REVID_MASK) >> SIM_SDID_REVID_SHIFT) > 0) {// if from revision 2 part is detected (clock enable is compatible)
        RNGA_CR = (RNGA_CR_INTM | RNGA_CR_HA | RNGA_CR_GO);              // start first conversion in RNGA module
        return;
    }
        #endif
    RNG_CR = (RNG_CR_FUFMODE_TE | RNG_CR_AR);                            // automatic re-seed mode and generate a bus error on FIFO underrun
    RNG_CMD = (RNG_CMD_GS | RNG_CMD_CI | RNG_CMD_CE);                    // start the initial seed process
                                                                         // the initial seeding takes some time but we don't wait for it to complete here - if it hasn't completed when we first need a value we will wait for it then
    #elif defined RANDOM_NUMBER_GENERATOR_A
    POWER_UP_ATOMIC(3, RNGA);                                            // power up RNGA
    RNGA_CR = (RNGA_CR_INTM | RNGA_CR_HA | RNGA_CR_GO);                  // start first conversion
    #elif defined TRUE_RANDOM_NUMBER_GENERATOR
        #if defined SIM_SCGC3
    POWER_UP_ATOMIC(3, TRNG0);                                           // power up TRNG0
        #else
    POWER_UP_ATOMIC(6, TRNG0);                                           // power up TRNG0
        #endif
    TRNG0_MCTL = (TRNG_MCTL_PRGM | TRNG_MCTL_RST_DEF);                   // ensure we are in programming mode with defaults set
    TRNG0_FRQMIN = 0;
    TRNG0_FRQMAX = 0x03ffff;                                             // the default maximum value is too low and causes a frequency count failure if not increased
    TRNG0_SDCTL = ((1600 << 16) | (2500));                               // entropy delay and sample size reduced to half of default
    TRNG0_MCTL = (TRNG_MCTL_PRGM_RUN | TRNG_MCTL_SAMP_MODE_VON_NEUMANN | TRNG_MCTL_TRNG_ACC | TRNG_MCTL_OSC_DIV_1); // set to run mode with TRNG access
    #endif
}

// Get a random number from the RNG
//
extern unsigned short fnGetRndHW(void)
{
    #if defined RANDOM_NUMBER_GENERATOR_B                                // support revison 1 types
    unsigned long ulRandomNumber;
        #if defined _WINDOWS
    if (IS_POWERED_UP(3, RNGB) == 0) {
        _EXCEPTION("Warning: RNG being used before initialised!!!");
    }
        #endif
        #if defined RANDOM_NUMBER_GENERATOR_A                            // support revison 2 types too
    if (((SIM_SDID & SIM_SDID_REVID_MASK) >> SIM_SDID_REVID_SHIFT)== 0) {// if not from revision 2 part
        #endif
        while ((RNG_SR & RNG_SR_BUSY) != 0) {}                           // wait for the RNGB to become ready (it may be seeding)
        while ((RNG_SR & RNG_SR_FIFO_LVL_MASK) == 0) {                   // wait for at least one output word to become available
        #if defined _WINDOWS
            RNG_OUT = rand();
            RNG_SR |= 0x00000100;                                        // put one result in the FIFO
        #endif
        }
        ulRandomNumber = RNG_OUT;                                        // read from the FIFO
        #if defined _WINDOWS
        RNG_SR &= ~RNG_SR_FIFO_LVL_MASK;
        #endif
        return (unsigned short)(ulRandomNumber);                         // return 16 bits of output
        #if defined RANDOM_NUMBER_GENERATOR_A
    }
        #endif
    #endif
    #if defined RANDOM_NUMBER_GENERATOR_A                                // RNGA
        #if !defined RANDOM_NUMBER_GENERATOR_B
    unsigned long ulRandomNumber;
        #endif
        #if defined _WINDOWS
    if (IS_POWERED_UP(3, RNGA) == 0) {
        _EXCEPTION("Warning: RNGA being used before initialised!!!");
    }
        #endif
    while ((RNGA_SR & RNGA_SR_OREG_LVL) == 0) {                          // wait for an output to become available
        #if defined _WINDOWS
        RNGA_SR |= RNGA_SR_OREG_LVL;
        RNGA_OR = rand();
        #endif
    }
        #if defined _WINDOWS
    RNGA_SR &= ~RNGA_SR_OREG_LVL;
        #endif
    ulRandomNumber = RNGA_OR;                                            // read output value that has been generated
    return (unsigned short)(ulRandomNumber);                             // return 16 bits of output
    #endif
    #if defined TRUE_RANDOM_NUMBER_GENERATOR
    static int iEntropyIndex = 0;
    volatile unsigned long *ptrEntropy = TRNG0_ENT0_ADD;                 // the first entropy register
        #if defined _WINDOWS
            #if defined SIM_SCGC3
    if (IS_POWERED_UP(3, TRNG0) == 0)
            #else
    if (IS_POWERED_UP(6, TRNG0) == 0)
            #endif
    {
        _EXCEPTION("Warning: TRNG0 being used before initialised!!!");
    }
        #endif
    ptrEntropy += iEntropyIndex;                                         // read entropy registers 0..15
    while ((TRNG0_MCTL & TRNG_MCTL_ENT_VAL) == 0) {                      // wait until random value is ready
        #if defined _WINDOWS
        TRNG0_ENT0 = rand();
        TRNG0_ENT1 = rand();
        TRNG0_ENT2 = rand();
        TRNG0_ENT3 = rand();
        TRNG0_ENT4 = rand();
        TRNG0_ENT5 = rand();
        TRNG0_ENT6 = rand();
        TRNG0_ENT7 = rand();
        TRNG0_ENT8 = rand();
        TRNG0_ENT9 = rand();
        TRNG0_ENT10 = rand();
        TRNG0_ENT11 = rand();
        TRNG0_ENT12 = rand();
        TRNG0_ENT13 = rand();
        TRNG0_ENT14 = rand();
        TRNG0_ENT15 = rand();
        TRNG0_MCTL |= TRNG_MCTL_ENT_VAL;                                 // flag that the entropy values are valid
        #endif
        if ((TRNG0_MCTL & TRNG_MCTL_ERR) != 0) {                         // if an error is signalled
            break;
        }
    }
    if (++iEntropyIndex > 15) {
        iEntropyIndex = 0;
        #if defined _WINDOWS
        TRNG0_MCTL &= ~(TRNG_MCTL_ENT_VAL);                              // reading the last entropy register causes the next conversion to be started
        #endif
    }
    return (unsigned short)(*ptrEntropy);
    #endif
}
    #else
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
#endif

#if defined DEVICE_WITH_SLCD
// The SLCD controller may retain segment settings across resets so this initialisation routine is used to ensure all are cleared when starting
//
extern void fnClearSLCD(void)
{
    unsigned long *ptrSegments = LCD_WF3TO0_ADDR;
    int i = 0;
    while (i++ < SEGMENT_REGISTER_COUNT) {
        *ptrSegments++ = 0;
    }
    #if defined _WINDOWS && defined SLCD_FILE
    fnSimulateSLCD();                                                    // allow any SLCD display updates to be made
    #endif
}
#endif

extern void fnDelayLoop(unsigned long ulDelay_us)
{
#if defined TSTMR_AVAILABLE
    // Use the time stamp timer module to count us
    //
    unsigned long ulMatchTimeHigh;
    unsigned long ulMatchTimeLow;
    unsigned long ulPresentTimeStamp = TSTMR0_L;                         // get lowest 32 bits of timer
    (void)TSTMR0_H;                                                      // dummy read of highest 20 bits so that the next low work read is unlocked
    ulMatchTimeLow = ulPresentTimeStamp;
    ulMatchTimeHigh = (ulPresentTimeStamp + ulDelay_us);                 // the count value after the delay (rounded up)
    if (ulMatchTimeHigh > ulPresentTimeStamp) {                          // no long word overflow will normally occur
        FOREVER_LOOP() {
            ulPresentTimeStamp = TSTMR0_L;                               // get lowest 32 bits of timer
            (void)TSTMR0_H;                                              // dummy read of highest 20 bits so that the next low work read is unlocked
            if ((ulPresentTimeStamp >= ulMatchTimeHigh) || (ulPresentTimeStamp < ulMatchTimeLow)) {
                // The time has been reached, or passed (including overflow after the match was passed)
                //
                return;
            }
    #if defined _WINDOWS
            TSTMR0_L = (TSTMR0_L + 1);
            if (TSTMR0_L == 0) {
                TSTMR0_H = (TSTMR0_H + 1);
            }
    #endif
        }
    }
    else {                                                               // a long word overfow will occur before the match
        FOREVER_LOOP() {
            ulPresentTimeStamp = TSTMR0_L;                               // get lowest 32 bits of timer
            (void)TSTMR0_H;                                              // dummy read of highest 20 bits so that the next low work read is unlocked
            if ((ulPresentTimeStamp >= ulMatchTimeHigh) && (ulPresentTimeStamp < ulMatchTimeLow)) {
                // The time has been reached, or passed
                //
                return;
            }
    #if defined _WINDOWS
            TSTMR0_L = (TSTMR0_L + 1);
            if (TSTMR0_L == 0) {
                TSTMR0_H = (TSTMR0_H + 1);
            }
    #endif
        }
    }
#elif !defined TICK_USES_LPTMR && !defined TICK_USES_RTC                 // if the SYSTICK is operating we use it as a us timer for best independence of code execution speed and compiler (KL typically +15% longer then requested value between 100us and 10ms)
    #define CORE_US (CORE_CLOCK/1000000)                                 // the number of core clocks in a us
    #if !defined _WINDOWS
    register unsigned long ulPresentSystick;
    #endif
    register unsigned long ulMatch;
    register unsigned long _ulDelay_us = ulDelay_us;                     // ensure that the compiler puts the variable in a register rather than work with it on the stack
    if (_ulDelay_us == 0) {                                              // minimum delay is 1us
        _ulDelay_us = 1;
    }
    (void)SYSTICK_CSR;                                                   // clear the SysTick reload flag
    ulMatch = (SYSTICK_CURRENT - CORE_US);                               // next 1us match value (SysTick counts down)
    do {
    #if !defined _WINDOWS
        while ((ulPresentSystick = SYSTICK_CURRENT) > ulMatch) {         // wait until a us period has expired
            if ((SYSTICK_CSR & SYSTICK_COUNTFLAG) != 0) {                // if we missed a reload
                (void)SYSTICK_CSR;
                break;                                                   // assume a us period expired
            }
        }
        ulMatch = (ulPresentSystick - CORE_US);
    #endif
    } while (--_ulDelay_us != 0);
#else
    volatile register unsigned long _ulDelay_us = ulDelay_us;            // ensure that the compiler puts the variable in a register rather than work with it on the stack
    volatile register unsigned long ul_us;
    while (_ulDelay_us-- != 0) {                                         // for each us required        
        ul_us = (CORE_CLOCK/8000000);                                    // tuned but may be slightly compiler dependent - interrupt processing may increase delay
        while (ul_us-- != 0) {}                                          // simple loop tuned to perform us timing
    }
#endif
}

// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#if defined _WINDOWS
    unsigned long ulPortPullups[] = {
        PORT0_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_kinetis.h
    #if PORTS_AVAILABLE > 1
        PORT1_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 2
        PORT2_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 3
        PORT3_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 4
        PORT4_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 5
        PORT5_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 6
        PORT6_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 7
        PORT7_DEFAULT_INPUT,
    #endif
    #if PORTS_AVAILABLE > 8
        PORT8_DEFAULT_INPUT,
    #endif
    #if defined SUPPORT_ADC
        ((ADC0_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC0_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #if ADC_CONTROLLERS > 1
        ((ADC1_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC1_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #endif
        #if ADC_CONTROLLERS > 2
        ((ADC2_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC2_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #endif
        #if ADC_CONTROLLERS > 3
        ((ADC3_0_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_1_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_2_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_3_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_4_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_5_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_6_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_7_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_8_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_9_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_10_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_11_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_12_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_13_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_14_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_15_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_16_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_17_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_18_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_19_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_20_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_21_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_22_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        ((ADC3_23_START_VOLTAGE * 0xffff) / ADC_REFERENCE_VOLTAGE),
        #endif
    #endif
    };
    #if defined RANDOM_NUMBER_GENERATOR && !defined RND_HW_SUPPORT
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;
    #endif
    fnInitialiseDevice(ulPortPullups);
#endif
#if !defined _COMPILE_KEIL
    _LowLevelInit();                                                     // configure watchdog and set the CPU to operating speed (system variables will be initialised at latest here)
#endif
#if defined RUN_LOOPS_IN_RAM
    fnInitDriver();                                                      // initialise driver code in SRAM (must be first)
    #if defined USE_IP && ((!defined IP_RX_CHECKSUM_OFFLOAD && !defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined _WINDOWS)
    fnInitIP();                                                          // initialise IP routines to run from SRAM
    #endif
#endif
#if !defined DEVICE_WITHOUT_DMA && ((!defined KINETIS_KL && !defined KINETIS_KM) || defined DEVICE_WITH_eDMA)
    #if defined KINETIS_WITH_PCC && !defined KINETIS_KE15                // powered up by default in KE15
    POWER_UP_ATOMIC(0, DMA0);                                            // power up the DMA module
    #endif
    #if defined DEVICE_WITH_TWO_DMA_GROUPS
    DMA_CR = (DMA_CR_GRP0PRI_0 | DMA_CR_GRP1PRI_1);                      // set the two DMA groups to non-conflicting priorities
    #else
    DMA_CR = 0;
    #endif
#endif
#if defined KINETIS_KE15                                                 // ADCs in KE15 are powered up by default
    PCC_ADC0 = PCC_PR;                                                   // power down ADCs so that their clock source can be configured when needed
    PCC_ADC1 = PCC_PR;
#endif
#if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA                // set the eDMA registers to a known zero state
    {
    #if ((!defined KINETIS_KL && !defined KINETIS_KM) || defined DEVICE_WITH_eDMA)
        unsigned long *ptr_eDMAdes = (unsigned long *)eDMA_DESCRIPTORS;
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL;                                // the DMA channel used for memory copy DMA
        while (ptr_eDMAdes < eDMA_DESCRIPTORS_END) {
            *ptr_eDMAdes++ = 0;                                          // clear out DMA descriptors after reset
        }
        ptrDMA_TCD->DMA_TCD_SOFF = 4;                                    // source increment one long word for uMemcpy()
        ptrDMA_TCD->DMA_TCD_DOFF = 4;                                    // destination increment one long word
        ptrDMA_TCD->DMA_TCD_BITER_ELINK = 1;
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // default transfer sizes long words
        #if defined DMA_MEMCPY_CHANNEL_ALT
        ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += DMA_MEMCPY_CHANNEL_ALT;                            // move to the alternate channel (may be used by interrupts)
        ptrDMA_TCD->DMA_TCD_SOFF = 4;                                    // source increment one long word for uMemcpy()
        ptrDMA_TCD->DMA_TCD_DOFF = 4;                                    // destination increment one long word
        ptrDMA_TCD->DMA_TCD_BITER_ELINK = 1;
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // default transfer sizes long words
        #endif
        #if defined DMA_CHANNEL_0_PRIORITY                               // user defined channel priorities
        _SET_DMA_CHANNEL_PRIORITY(0, DMA_CHANNEL_0_PRIORITY);            // DMA priority, whereby channel can suspend a lower priority channel
        _SET_DMA_CHANNEL_PRIORITY(1, DMA_CHANNEL_1_PRIORITY);            // all channel priorities are set before use since it can be dangerous to change them later when DMA operations could take place during the process
        _SET_DMA_CHANNEL_PRIORITY(2, DMA_CHANNEL_2_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(3, DMA_CHANNEL_3_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(4, DMA_CHANNEL_4_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(5, DMA_CHANNEL_5_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(6, DMA_CHANNEL_6_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(7, DMA_CHANNEL_7_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(8, DMA_CHANNEL_8_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(9, DMA_CHANNEL_9_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(10, DMA_CHANNEL_10_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(11, DMA_CHANNEL_11_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(12, DMA_CHANNEL_12_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(13, DMA_CHANNEL_13_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(14, DMA_CHANNEL_14_PRIORITY);
        _SET_DMA_CHANNEL_PRIORITY(15, DMA_CHANNEL_15_PRIORITY);
        _SET_DMA_CHANNEL_CHARACTERISTIC(DMA_MEMCPY_CHANNEL, (DMA_DCHPRI_ECP | DMA_DCHPRI_DPA)); // can be pre-empted by higher priority channel - it is expected that this channel will normally have priority 0
            #if defined DMA_MEMCPY_CHANNEL_ALT
        _SET_DMA_CHANNEL_CHARACTERISTIC(DMA_MEMCPY_CHANNEL_ALT, (DMA_DCHPRI_ECP | DMA_DCHPRI_DPA)); // can be pre-empted by higher priority channel - it is expected that this channel will normally have priority 1
            #endif
        #else                                                            // leave default channel priority (equal to the corresponding channel number)
        _SET_DMA_CHANNEL_PRIORITY(DMA_MEMCPY_CHANNEL, (DMA_DCHPRI_DPA | DMA_DCHPRI_ECP | 0)); // {137} lowest DMA priority and can be pre-empted by higher priority channel
            #if DMA_MEMCPY_CHANNEL != 0
        _SET_DMA_CHANNEL_PRIORITY(0, (DMA_MEMCPY_CHANNEL));              // no two priorities may ever be the same when the controller is used - switch priorities to avoid
            #endif
            #if defined DMA_MEMCPY_CHANNEL_ALT
        _SET_DMA_CHANNEL_PRIORITY(DMA_MEMCPY_CHANNEL_ALT, (DMA_DCHPRI_DPA | DMA_DCHPRI_ECP | 1)); // {137} second lowest DMA priority and can be pre-empted by higher priority channel
                #if DMA_MEMCPY_CHANNEL_ALT != 1
        _SET_DMA_CHANNEL_PRIORITY(1, (DMA_MEMCPY_CHANNEL_ALT));          // no two priorities may ever be the same when the controller is used - switch priorities to avoid
                #endif
            #endif
        #endif
    #endif
    }
#endif
#if defined CONFIGURE_CROSSBAR_SWITCH
    CONFIGURE_CROSSBAR_SWITCH();
#endif
#if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
    uMemset(ulFlashRow, 0xff, FLASH_ROW_SIZE);                           // initialise intermediate phrase memory
#endif
#if !defined TICK_USES_LPTMR && !defined TICK_USES_RTC
    SYSTICK_RELOAD = SYSTICK_COUNT_MASK;                                 // temporarily set maximum reload value
    SYSTICK_CURRENT = SYSTICK_COUNT_MASK;                                // write to the current value to cause the counter value to be reset to 0 and the reload value be set
    (void)SYSTICK_CSR;                                                   // ensure that the SYSTICK_COUNTFLAG flag is cleared
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE);                 // allow SYSTICK to run so that loop delays can already use it
  //while ((SYSTICK_CSR & SYSTICK_COUNTFLAG) == 0) {                     // wait for the reload to take place (should be instantaneous)
  //#if defined _WINDOWS
  //    SYSTICK_CSR |= SYSTICK_COUNTFLAG;
  //#endif
  //}
#endif
    fnUserHWInit();                                                      // allow the user to initialise hardware specific things - note that heap cannot be used by this routine
#if defined _WINDOWS
    fnSimPorts(-1);                                                      // ensure port states are recognised
#endif
#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || ((defined SPI_FILE_SYSTEM || defined SPI_EEPROM_FILE_SYSTEM) && defined FLASH_FILE_SYSTEM)
    // Power up the SPI interface, configure the pins used and select the mode and speed
    //
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();                                     // configure SPI interface for maximum possible speed (after TICK has been configured due to potential use of delay routine)
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
    #if defined SPI_FILE_SYSTEM
        #include "spi_flash_iMX_atmel.h"
        #include "spi_flash_iMX_stmicro.h"
        #include "spi_flash_iMX_sst25.h"
        #include "spi_flash_iMX_w25q.h"
        #include "spi_flash_iMX_s25fl1-k.h"
        #include "spi_flash_iMX_MX25L.h"
    #endif
    #if defined SPI_EEPROM_FILE_SYSTEM
        #include "spi_eeprom_iMX_25AA160.h"
    #endif
    #undef _CHECK_SPI_CHIPS
#endif
}


/* =================================================================== */
/*                    General Interrupt Control                        */
/* =================================================================== */


// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)
{
#if defined _WINDOWS
    kinetis.CORTEX_M4_REGS.ulPRIMASK = INTERRUPT_MASKED;                 // mark that interrupts are masked
#else
    #if defined SYSTEM_NO_DISABLE_LEVEL
    uMask_Interrupt(SYSTEM_NO_DISABLE_LEVEL << __NVIC_PRIORITY_SHIFT);   // interrupts with higher priorities are not disabled
    #else
    __disable_interrupt();                                               // disable interrupts to core
    #endif
#endif
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
extern void uEnable_Interrupt(void)
{
#if defined _WINDOWS
    if (iInterruptLevel == 0) {                                          // it is expected that this routine is only called when interrupts are presently disabled
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
        //
        _EXCEPTION("Unsymmetrical use of interrupt disable/enable detected!!");
    }
#endif
    if ((--iInterruptLevel) == 0) {                                      // only when no more interrupt nesting,
#if defined _WINDOWS
        extern void fnExecutePendingInterrupts(int iRecursive);
        kinetis.CORTEX_M4_REGS.ulPRIMASK = 0;                            // unmask global interrupts
    #if defined RUN_IN_FREE_RTOS
        fnExecutePendingInterrupts(0);                                   // pending interrupts that were blocked by the main task can be executed now
    #endif
#else
    #if defined SYSTEM_NO_DISABLE_LEVEL
        uMask_Interrupt(LOWEST_PRIORITY_PREEMPT_LEVEL);                  // allow all interrupts again
    #else
        __enable_interrupt();                                            // enable processor interrupts
    #endif
#endif
    }
}

// Routine to change interrupt level mask
//
#if !defined _COMPILE_KEIL
    #if defined _GNU
        #define DONT_INLINE __attribute__((noinline))
    #else
        #define DONT_INLINE
    #endif
extern void DONT_INLINE uMask_Interrupt(unsigned char ucMaskLevel)
{
    #if !defined ARM_MATH_CM0PLUS                                         // mask not supported by Cortex-m0+
        #if defined _WINDOWS
    kinetis.CORTEX_M4_REGS.ulBASEPRI = ucMaskLevel;                       // value 0 has no  - non-zero defines the base priority for exception processing (the processor does not process any exception with a priority value greater than or equal to BASEPRI))
        #else
    asm("msr basepri, r0");                                               // modify the base priority to block interrupts with a lower priority than this level
    asm("bx lr");                                                         // return
        #endif
    #endif
}
#endif


#if 0                                                                    // example of code that may be usable for Cortex-M0+ to disable all interrupts apart from specified ones
#define NVIC_SET_REGISTERS  3

static unsigned long _SYSTICK_CSR = 0;
static unsigned long NVICIntSet[NVIC_SET_REGISTERS] = {0};

#define DISABLEMASK_0_31    0xffffffff                                   // all to be disabled
#define DISABLEMASK_32_63   0xfffffffe                                   // all to be disabled apart from IRQ32
#define DISABLEMASK_64_95   0xffffffff                                   // all to be disabled
#define DISABLEMASK_96_127  0xffffffff                                   // all to be disabled
#define DISABLEMASK_128_159 0xffffffff                                   // all to be disabled
#define DISABLEMASK_160_191 0xffffffff                                   // all to be disabled
#define DISABLEMASK_192_223 0xffffffff                                   // all to be disabled
#define DISABLEMASK_224_239 0xffffffff                                   // all to be disabled

extern void fnDisableOtherInterrupts(void)
{
    _SYSTICK_CSR = (SYSTICK_CSR & SYSTICK_TICKINT);                      // save original systick interrupt mask
    SYSTICK_CSR &= ~(SYSTICK_TICKINT);                                   // disable systick interrupt
    NVICIntSet[0] = IRQ0_31_SER;                                         // save original NVIC interrupt flags
    IRQ0_31_CER = DISABLEMASK_0_31;                                      // disable interrupts
    NVICIntSet[1] = IRQ32_63_SER;
    IRQ32_63_CER = DISABLEMASK_32_63;
    #if NVIC_SET_REGISTERS > 2
    NVICIntSet[2] = IRQ64_95_SER;
    IRQ64_95_CER = DISABLEMASK_64_95;
    #endif
    #if NVIC_SET_REGISTERS > 3
    NVICIntSet[3] = IRQ96_127_SER
    IRQ96_127_CER = DISABLEMASK_96_127;
    #endif
    #if NVIC_SET_REGISTERS > 4
    NVICIntSet[4] = IRQ128_159_SER
    IRQ128_159_CER = DISABLEMASK_128_159;
    #endif
    #if NVIC_SET_REGISTERS > 5
    NVICIntSet[5] = IRQ160_191_SER
    IRQ160_191_CER = DISABLEMASK_160_191;
    #endif
    #if NVIC_SET_REGISTERS > 6
    NVICIntSet[6] = IRQ192_223_SER
    IRQ192_223_CER = DISABLEMASK_192_223;
    #endif
    #if NVIC_SET_REGISTERS > 7
    NVICIntSet[7] = IRQ224_239_SER
    IRQ224_239_CER = DISABLEMASK_224_239;
    #endif
}

extern void fnReenableInterrupts(void)
{
    SYSTICK_CSR |= _SYSTICK_CSR;                                         // re-enable systick interrupt if it was previously enabled
    IRQ0_31_SER = NVICIntSet[0];                                         // re-enable previously enabled interrupts
    IRQ32_63_SER = NVICIntSet[1];
    #if NVIC_SET_REGISTERS > 2
    IRQ64_95_SER = NVICIntSet[2];
    #endif
    #if NVIC_SET_REGISTERS > 3
    IRQ96_127_SER = NVICIntSet[3];
    #endif
    #if NVIC_SET_REGISTERS > 4
    IRQ128_159_SER = NVICIntSet[4];
    #endif
    #if NVIC_SET_REGISTERS > 5
    IRQ160_191_SER = NVICIntSet[5];
    #endif
    #if NVIC_SET_REGISTERS > 6
    IRQ192_223_SER = NVICIntSet[6];
    #endif
    #if NVIC_SET_REGISTERS > 7
    IRQ224_239_SER = NVICIntSet[7];
    #endif
}
#endif

#if defined INTMUX0_AVAILABLE

// Dispatch INTMUX0 source interrupt handler
//
static void fnDispatchINTMUX(unsigned long ulInterruptOffset)
{
    if (ulInterruptOffset != 0) {                                        // ignore spurious interrupts since they are not latched in the INTMUX
        void(*ptrIRQ)(void);
        unsigned char *ptrVect = (unsigned char *)VECTOR_TABLE_OFFSET_REG; // vector table
        ptrVect += ulInterruptOffset;                                    // move to the offset
        ptrIRQ = (void(*)(void ))*(unsigned long *)ptrVect;              // load the handler address
        ptrIRQ();                                                        // call the interrupt handler
    }
}

// INTMUX0 channel 0 interrupt
//
static void fnINTMUX0(void)
{
    fnDispatchINTMUX(INTMUX0_CH0_VEC);                                   // get the highest priority pending interrupt on this channel
    #if defined _WINDOWS                                                 // assume that the interrupt source was cleared
    INTMUX0_CH0_IPR_31_0 = 0;
    INTMUX0_CH0_CSR &= ~(INTMUX_CSR_IRQP);
    INTMUX0_CH0_VEC = 0;
    #endif
}

// INTMUX0 channel 1 interrupt
//
static void fnINTMUX1(void)
{
    fnDispatchINTMUX(INTMUX0_CH1_VEC);                                   // get the highest priority pending interrupt on this channel
    #if defined _WINDOWS                                                 // assume that the interrupt source was cleared
    INTMUX0_CH1_IPR_31_0 = 0;
    INTMUX0_CH1_CSR &= ~(INTMUX_CSR_IRQP);
    INTMUX0_CH1_VEC = 0;
    #endif
}

// INTMUX0 channel 2 interrupt
//
static void fnINTMUX2(void)
{
    fnDispatchINTMUX(INTMUX0_CH2_VEC);                                   // get the highest priority pending interrupt on this channel
    #if defined _WINDOWS                                                 // assume that the interrupt source was cleared
    INTMUX0_CH2_IPR_31_0 = 0;
    INTMUX0_CH2_CSR &= ~(INTMUX_CSR_IRQP);
    INTMUX0_CH2_VEC = 0;
    #endif
}

// INTMUX0 channel 3 interrupt
//
static void fnINTMUX3(void)
{
    fnDispatchINTMUX(INTMUX0_CH3_VEC);                                   // get the highest priority pending interrupt on this channel
    #if defined _WINDOWS                                                 // assume that the interrupt source was cleared
    INTMUX0_CH3_IPR_31_0 = 0;
    INTMUX0_CH3_CSR &= ~(INTMUX_CSR_IRQP);
    INTMUX0_CH3_VEC = 0;
    #endif
}
#endif

// Function used to enter processor interrupts
//
extern void fnEnterInterrupt(int iInterruptID, unsigned char ucPriority, void (*InterruptFunc)(void))
{
    volatile unsigned long *ptrIntSet = IRQ0_31_SER_ADD;
#if defined ARM_MATH_CM0PLUS                                             // only long word accesses are possible to the priority registers
    volatile unsigned long *ptrPriority = (unsigned long *)IRQ0_3_PRIORITY_REGISTER_ADD;
    int iShift;
#else
    volatile unsigned char *ptrPriority = IRQ0_3_PRIORITY_REGISTER_ADD;
#endif
#if !defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE *ptrVect = (VECTOR_TABLE *)VECTOR_TABLE_OFFSET_REG;
    void (**processor_ints)(void);
#endif
#if defined _WINDOWS                                                     // back up the present enabled interrupt registers
    unsigned long ulState0 = IRQ0_31_SER;
    unsigned long ulState1 = IRQ32_63_SER;
    unsigned long ulState2 = IRQ64_95_SER;
    IRQ0_31_SER = IRQ32_63_SER = IRQ64_95_SER = IRQ0_31_CER = IRQ32_63_CER = IRQ64_95_CER = 0; // reset registers
#endif
#if defined INTMUX0_AVAILABLE
    if (iInterruptID >= irq_INTMUX0_0_ID) {
        KINETIS_INTMUX *ptrINTMUX = (KINETIS_INTMUX *)INTMUX0_BLOCK;
        int iChannel = (iInterruptID - irq_INTMUX0_0_ID);
    #if defined KINETIS_WITH_PCC
        PCC_INTMUX0 |= PCC_CGC;
    #else
        POWER_UP_ATOMIC(6, INTMUX0);                                     // power up the INTMUX0 module
    #endif
        ptrINTMUX += iChannel;                                           // moved to the channel to be used
        ptrINTMUX->INTMUX_CHn_IER_31_0 |= (1 << ucPriority);             // enable the peripheral source interrupt to the INTMUX module
    #if !defined INTERRUPT_VECTORS_IN_FLASH
        processor_ints = (void(**)(void))&ptrVect->processor_interrupts; // first processor interrupt location in the vector table
        processor_ints += (irq_INTMUX0_3_ID + 1 + ucPriority);           // move the pointer to the location used by this interrupt number
        *processor_ints = InterruptFunc;                                 // enter the interrupt handler into the (extended) vector table
    #endif
        iInterruptID = (irq_INTMUX0_0_ID + iChannel);
        switch (iChannel) {
        case 0:
            InterruptFunc = fnINTMUX0;
            ucPriority = PRIORITY_INTMUX0_0_INT;
            break;
        case 1:
            InterruptFunc = fnINTMUX1;
            ucPriority = PRIORITY_INTMUX0_1_INT;
            break;
        case 2:
            InterruptFunc = fnINTMUX2;
            ucPriority = PRIORITY_INTMUX0_2_INT;
            break;
        case 3:
            InterruptFunc = fnINTMUX3;
            ucPriority = PRIORITY_INTMUX0_3_INT;
            break;
        default:
            _EXCEPTION("Illegal INTMUX0 channel!");
            break;
        }
    }
#endif
#if defined _WINDOWS                                                     // check for valid interrupt priority range
    #if defined ARM_MATH_CM0PLUS
    if (ucPriority >= 4) {
        _EXCEPTION("Invalid Cortex-M0+ priority being used!!");
    }
    #else
    if (ucPriority >= 16) {
        _EXCEPTION("Invalid Cortex-M4/M7 priority being used!!");
    }
    #endif
#endif
#if !defined INTERRUPT_VECTORS_IN_FLASH
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;    // first processor interrupt location in the vector table
    processor_ints += iInterruptID;                                      // move the pointer to the location used by this interrupt number
    *processor_ints = InterruptFunc;                                     // enter the interrupt handler into the vector table
#endif
#if defined ARM_MATH_CM0PLUS
    ptrPriority += (iInterruptID/4);                                     // move to the priority location used by this interrupt
    iShift = ((iInterruptID % 4) * 8);
    *ptrPriority = ((*ptrPriority & ~(0xff << iShift)) | (ucPriority << (iShift + __NVIC_PRIORITY_SHIFT)));
#else
    ptrPriority += iInterruptID;                                         // move to the priority location used by this interrupt
    *ptrPriority = (ucPriority << __NVIC_PRIORITY_SHIFT);                // define the interrupt's priority (16 levels for Cortex-m4 and 4 levels for Cortex-m0+)
#endif
    ptrIntSet += (iInterruptID/32);                                      // move to the interrupt enable register in which this interrupt is controlled
    *ptrIntSet = (0x01 << (iInterruptID % 32));                          // enable the interrupt
#if defined _WINDOWS
    IRQ0_31_SER  |= ulState0;                                            // synchronise the interrupt masks
    IRQ32_63_SER |= ulState1;
    IRQ64_95_SER |= ulState2;
    IRQ0_31_CER   = IRQ0_31_SER;
    IRQ32_63_CER  = IRQ32_63_SER;
    IRQ64_95_CER  = IRQ64_95_SER;
#endif
}

extern void fnMaskInterrupt(int iInterruptID)
{
    volatile unsigned long *ptrIntClr = IRQ0_31_CER_ADD;
    ptrIntClr += (iInterruptID/32);                                      // move to the clear enable register in which this interrupt is controlled
    *ptrIntClr = (0x01 << (iInterruptID % 32));                          // disable the interrupt
}

extern void fnClearPending(int iInterruptID)
{
    volatile unsigned long *ptrIntClr = IRQ0_31_CPR_ADD;                 // the first clear pending register
    ptrIntClr += (iInterruptID/32);                                      // move to the clear pending interrupt enable register in which this interrupt is controlled
    *ptrIntClr = (0x01 << (iInterruptID % 32));                          // clear the pending interrupt
}

extern int fnIsPending(int iInterruptID)
{
    volatile unsigned long *ptrIntActive = IRQ0_31_CPR_ADD;              // the first clear pending register, which also shows pending interrupts
    ptrIntActive += (iInterruptID/32);                                   // move to the clear pending interrupt enable register in which this interrupt is controlled
    return ((*ptrIntActive & (0x01 << (iInterruptID % 32))) != 0);       // return the pending state of this interrupt
}


/* =================================================================== */
/*                                 TICK                                */
/* =================================================================== */

// Tick interrupt
//
static __interrupt void _RealTimeInterrupt(void)
{
#if defined TICK_USES_LPTMR                                              // tick interrupt from low power timer
    LPTMR0_CSR = LPTMR0_CSR;                                             // clear pending interrupt
#elif defined TICK_USES_RTC                                              // tick interrupt from RTC
    RTC_SC |= RTC_SC_RTIF;                                               // clear pending interrupt
#else                                                                    // tick interrupt from systick
    INT_CONT_STATE_REG = PENDSTCLR;                                      // reset interrupt
    #if defined _WINDOWS
    INT_CONT_STATE_REG &= ~(PENDSTSET | PENDSTCLR);
    #endif
#endif
#if defined RUN_IN_FREE_RTOS && !defined _WINDOWS
    xPortSysTickHandler();
#endif
    uDisable_Interrupt();                                                // ensure tick handler cannot be interrupted
        fnRtmkSystemTick();                                              // operating system tick
    uEnable_Interrupt();
}

#if defined SUPPORT_LOW_VOLTAGE_DETECTION                                // enable low voltage detection interrupt warning
// Interrupt to warn that the voltage is close to the reset threshold
// - the interrupt is disabled since in the case of power loss in progress it will not be possible to clear the interrupt flag
// - the user callback can request the interrupt to be re-enabled and a clear be attempted by returing a non-zero value if it is prepared to handle multiple interrupts
//
static __interrupt void _low_voltage_irq(void)
{
    PMC_LVDSC2 &= ~(PMC_LVDSC2_LVWIE);                                   // disable further interrupts so that the processor can continue operation (it can decide to re-enable interrupts if desired)
    if (fnPowerFailureWarning() != 0) {                                  // user supplied routine to handle power faiure (interrupt call back)
        OR_ONE_TO_CLEAR(PMC_LVDSC2, (PMC_LVDSC2_LVWACK));                // acknowledge the interrupt and attempt to clear the flag (in the case of power loss in progress this will never be able to clear the flag)
        PMC_LVDSC2 |= (PMC_LVDSC2);                                      // re-enable the interrupt
    }
}
#endif

// Routine to initialise the tick interrupt (uses Cortex M7/M4/M0+ SysTick timer, RTC or low power timer)
//
extern void fnStartTick(void)
{
#if defined TICK_USES_LPTMR                                              // use the low power timer to derive the tick interrupt from
    POWER_UP_ATOMIC(5, LPTMR0);                                          // ensure that the timer can be accessed
    LPTMR0_CSR = LPTMR_CSR_TCF;                                          // reset the timer and ensure no pending interrupts
    #if defined LPTMR_CLOCK_LPO                                          // define the low power clock speed for calculations
    LPTMR0_PSR = (LPTMR_PSR_PCS_LPO | LPTMR_PSR_PBYP);
    #elif defined LPTMR_CLOCK_INTERNAL_30_40kHz
    MCG_C2 &= ~MCG_C2_IRCS;                                              // select slow internal reference clock
    LPTMR0_PSR = (LPTMR_PSR_PCS_MCGIRCLK; | LPTMR_PSR_PBYP);
    #elif defined LPTMR_CLOCK_INTERNAL_4MHz
    MCG_C2 |= MCG_C2_IRCS;                                               // select fast internal reference clock
    LPTMR0_PSR = (LPTMR_PSR_PCS_MCGIRCLK | LPTMR_PSR_PBYP);
    #elif defined LPTMR_CLOCK_RTC_32kHz
    POWER_UP_ATOMIC(6, RTC);                                             // enable access and interrupts to the RTC
    if ((RTC_SR & RTC_SR_TIF) != 0) {                                    // if timer invalid
        RTC_SR = 0;                                                      // ensure stopped
        RTC_TSR = 0;                                                     // write to clear RTC_SR_TIF in status register when not yet enabled
        #if !defined KINETIS_KL
        RTC_CR = (RTC_CR_OSCE);                                          // enable oscillator and supply it to other peripherals
        #endif
    }
    SIM_SOPT1 = ((SIM_SOPT1 & ~SIM_SOPT1_OSC32KSEL_MASK) | SIM_SOPT1_OSC32KSEL_32k); // select ERCLK32K from the RTC 32k clock
        #if defined KINETIS_K22
  //SIM_SOPT1 = ((SIM_SOPT1 & ~SIM_SOPT1_OSC32KOUT_MASK) | SIM_SOPT1_OSC32KOUT_PTE0); // 32kHz output to CLKOUT32k pin
        #endif
        #if defined LPTMR_PRESCALE
    LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | ((LPTMR_PRESCALE_VALUE) << LPTMR_PSR_PRESCALE_SHIFT)); // program prescaler
        #else
    LPTMR0_PSR = (LPTMR_PSR_PCS_ERCLK32K | LPTMR_PSR_PBYP);
        #endif
    #else                                                                // LPTMR_CLOCK_EXTERNAL_32kHz or LPTMR_CLOCK_OSCERCLK
    OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                       // enable the external reference clock and keep it enabled in stop mode
        #if defined LPTMR_CLOCK_EXTERNAL_32kHz                           // 32kHz crystal
            #if defined LPTMR_PRESCALE
    LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | ((LPTMR_PRESCALE_VALUE) << LPTMR_PSR_PRESCALE_SHIFT)); // program prescaler
            #else
    LPTMR0_PSR = (LPTMR_PSR_PCS_ERCLK32K | LPTMR_PSR_PBYP);
            #endif
        #else
            #if defined LPTMR_PRESCALE
    LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | ((LPTMR_PRESCALE_VALUE) << LPTMR_PSR_PRESCALE_SHIFT)); // program prescaler
            #else
    LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | LPTMR_PSR_PBYP);
            #endif
        #endif
    #endif
    fnEnterInterrupt(irq_LPTMR0_ID, LPTMR0_INTERRUPT_PRIORITY, (void (*)(void))_RealTimeInterrupt); // enter interrupt handler
    LPTMR0_CSR |= LPTMR_CSR_TIE;                                         // enable timer interrupt
    LPTMR0_CMR = LPTMR_US_DELAY((TICK_RESOLUTION));                      // TICK period
    #if defined _WINDOWS
    if (LPTMR0_CMR > 0xffff) {
        _EXCEPTION("LPTMR0_CMR value too large (16 bits)");
    }
    #endif
    LPTMR0_CSR |= LPTMR_CSR_TEN;                                         // enable the low power timer
#elif defined TICK_USES_RTC                                              // use RTC to derive the tick interrupt from
    POWER_UP_ATOMIC(6, RTC);                                             // ensure the RTC is powered
    fnEnterInterrupt(irq_RTC_OVERFLOW_ID, PRIORITY_RTC, (void (*)(void))_RealTimeInterrupt); // enter interrupt handler
    #if defined RTC_USES_EXT_CLK
    RTC_MOD = (unsigned long)((((unsigned long long)((unsigned long long)TICK_RESOLUTION * (unsigned long long)_EXTERNAL_CLOCK)/RTC_CLOCK_PRESCALER_1)/1000000) - 1); // set the match value
    #elif defined RTC_USES_INT_REF
    RTC_MOD = ((((TICK_RESOLUTION * ICSIRCLK)/RTC_CLOCK_PRESCALER_1)/1000000) - 1); // set the match value
    #else
    RTC_MOD = ((((TICK_RESOLUTION * ICSIRCLK) / RTC_CLOCK_PRESCALER_2) / 1000000) - 1); // set the match value
    #endif
    RTC_SC = (RTC_SC_RTIE | RTC_SC_RTIF | _RTC_CLOCK_SOURCE | _RTC_PRESCALER); // clock the RTC from the defined clock source/pre-scaler and enable interrupt
    #if defined _WINDOWS
    if (RTC_MOD > 0xffff) {
        _EXCEPTION("MOD value too large (16 bits)");
    }
    #endif
#else                                                                    // use systick to derive the tick interrupt from
    #define REQUIRED_US (1000000/(TICK_RESOLUTION))                      // the TICK frequency we require in MHz
    #define TICK_DIVIDE (((CORE_CLOCK + REQUIRED_US/2)/REQUIRED_US) - 1) // the divide ratio required (for systick)

    #if TICK_DIVIDE > 0x00ffffff
        #error "TICK value cannot be achieved with SYSTICK at this core frequency!!"
    #endif
    #if !defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE *ptrVect;
        #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
        #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
        #endif
    ptrVect->ptrSysTick = _RealTimeInterrupt;                            // enter interrupt handler
    #endif
    SYSTICK_RELOAD = TICK_DIVIDE;                                        // set reload value to determine the period
    SYSTICK_CURRENT = TICK_DIVIDE;
    SYSTEM_HANDLER_12_15_PRIORITY_REGISTER |= (unsigned long)(SYSTICK_PRIORITY << (24 + __NVIC_PRIORITY_SHIFT)); // {116} enter the SYSTICK priority
    SYSTICK_CSR = (SYSTICK_CORE_CLOCK | SYSTICK_ENABLE | SYSTICK_TICKINT); // enable timer and its interrupt
    #if defined _WINDOWS
    SYSTICK_RELOAD &= SYSTICK_COUNT_MASK;                                // mask any values which are out of range
    SYSTICK_CURRENT = SYSTICK_RELOAD;                                    // prime the reload count
    #endif
#endif
#if defined MONITOR_PERFORMANCE                                          // configure a timer that will be used to measure the duration of task operation
    INITIALISE_MONITOR_TIMER();
#endif
#if defined SUPPORT_LOW_VOLTAGE_DETECTION                                // enable low voltage detection interrupt warning
    #if !defined LOW_VOLTAGE_DETECTION_VOLTAGE_mV                        // if no value is defined we delault to 2.10V warning threshold and 1.6V reset threshold
        #define LOW_VOLTAGE_DETECTION_VOLTAGE_mV   2100
    #endif
    fnEnterInterrupt(irq_LOW_VOLTAGE_ID, 0, _low_voltage_irq);           // enter highest priority interrupt to warn of failing voltage
    #if LOW_VOLTAGE_DETECTION_VOLTAGE_mV > 2400                          // sensitive detection level
    // K64 reference: high reset threshold is typically 2.56V
    //
    PMC_LVDSC1 = (PMC_LVDSC1_LVDV | PMC_LVDSC1_LVDRE | PMC_LVDSC1_LVDACK); // high voltage level trip with reset enabled (clear flag)
        #if LOW_VOLTAGE_DETECTION_VOLTAGE_mV >= 3000
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_HIGH | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 3.00V typical
        #elif LOW_VOLTAGE_DETECTION_VOLTAGE_mV >= 2950
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_MID2 | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 2.90V typical
        #elif LOW_VOLTAGE_DETECTION_VOLTAGE_mV >= 2850
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_MID1 | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 2.80V typical
        #else
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_LOW | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 2.70V typical
        #endif
    #else
    // K64 reference: low reset threshold is typically 1.6V
    //
    PMC_LVDSC1 = (PMC_LVDSC1_LVDRE | PMC_LVDSC1_LVDACK);                 // low voltage level trip with reset enabled (clear flag)
        #if LOW_VOLTAGE_DETECTION_VOLTAGE_mV > 2050
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_HIGH | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 2.10V typical
        #elif LOW_VOLTAGE_DETECTION_VOLTAGE_mV >= 1950
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_MID2 | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 2.00V typical
        #elif LOW_VOLTAGE_DETECTION_VOLTAGE_mV >= 1850
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_MID1 | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 1.90V typical
        #else
    PMC_LVDSC2 = (PMC_LVDSC2_LVWV_LOW | PMC_LVDSC2_LVWIE | PMC_LVDSC2_LVWACK); // enable low voltage warning interrupt (clear flag) 1.80V typical
        #endif
    #endif
#endif
}

/* =================================================================== */
/*                             Watchdog                                */
/* =================================================================== */

// Support watchdog re-triggering of specific hardware
//
extern void fnRetriggerWatchdog(void)
{
    if ((WDOG1_WCR & WDOG_WCR_WDE) != 0) {                               // if the WDOG1 is enabled retrigger it
        KICK_WATCHDOG_1();
    }
    if ((WDOG2_WCR & WDOG_WCR_WDE) != 0) {                               // if the WDOG2 is enabled retrigger it
        KICK_WATCHDOG_2();
    }
    if ((WDOG3_CS & WDOG_CS_EN) != 0) {                                  // if the WDOG3 is enabled retrigger it
        KICK_WATCHDOG_3();
    }
    TOGGLE_WATCHDOG_LED();                                               // optionally flash a watchdog (heart-beat) LED
}

#if !defined DEVICE_WITHOUT_DMA
/* =================================================================== */
/*                                 DMA                                 */
/* =================================================================== */
    #define _DMA_SHARED_CODE
        #include "../Kinetis/kinetis_DMA.h"                              // include driver code for peripheral/buffer DMA
    #undef _DMA_SHARED_CODE
    #define _DMA_MEM_TO_MEM
        #include "../Kinetis/kinetis_DMA.h"                              // include memory-memory transfer code 
    #undef _DMA_MEM_TO_MEM
#endif

#if (defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET)
/* =================================================================== */
/*                          Ethernet Controller                        */
/* =================================================================== */
    #include "../Kinetis/kinetis_ENET.h"                                 // include Ethernet controller hardware driver code
#endif


#if defined USB_INTERFACE
/* =================================================================== */
/*                                USB                                  */
/* =================================================================== */
    #if defined USB_HS_INTERFACE
        #include "iMX_USB_HS_Device.h"                                   // include USB controller hardware HS device driver code
    #else
        #include "iMX_USB_OTG.h"                                         // include USB controller hardware OTG driver code
    #endif
#endif

#if defined SERIAL_INTERFACE
/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */
    #include "../Kinetis/kinetis_UART.h"                                 // include LPUART hardware driver code (driver shared with kinetis)
#endif

#if defined SPI_INTERFACE
/* =================================================================== */
/*                            SPI Interface                            */
/* =================================================================== */
    #include "iMX_SPI.h"                                                 // include SPI hardware driver code
#endif

#if defined CAN_INTERFACE && (NUMBER_OF_CAN_INTERFACES > 0)
/* =================================================================== */
/*                            FlexCAN/MSCAN                            */
/* =================================================================== */
    #include "iMX_CAN.h"                                                 // include FlexCAN/MSCAN hardware driver code
#endif


#if defined I2C_INTERFACE
/* =================================================================== */
/*                                  I2C                                */
/* =================================================================== */
    #if I2C_AVAILABLE > 0
        #include "iMX_I2C.h"                                             // include I2C hardware driver code
    #elif LPI2C_AVAILABLE > 0
        #include "iMX_LPI2C.h"                                           // include LPI2C hardware driver code
    #endif
#endif


#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */
    #include "iMX_FLASH.h"                                               // include FLASH driver code
#endif


#if defined SDCARD_SUPPORT && defined SD_CONTROLLER_AVAILABLE
/* =================================================================== */
/*                                SDHC                                 */
/* =================================================================== */
    #include "iMX_SDHC.h"                                                // include SDHC driver code
#endif

#if (defined SUPPORT_RTC && !defined KINETIS_WITHOUT_RTC) || defined SUPPORT_SW_RTC
/* =================================================================== */
/*                           Real Time Clock                           */
/* =================================================================== */
    #include "iMX_RTC.h"                                                 // include RTC driver code
#endif

#if (defined SUPPORT_PITS || defined USB_HOST_SUPPORT) && !defined KINETIS_WITHOUT_PIT
/* =================================================================== */
/*                     Periodic Interrupt Timer (PIT)                  */
/* =================================================================== */
#define _PIT_CODE
    #include "../Kinetis/kinetis_PIT.h"                                  // include PIT driver code
#undef _PIT_CODE
#endif


#if defined SUPPORT_LPTMR && !defined TICK_USES_LPTMR && !defined KINETIS_KE
/* =================================================================== */
/*                       Low Power Timer (LPTMR)                       */
/* =================================================================== */
#define _LPTMR_CODE
    #include "iMX_LPTMR.h"                                               // include LPTMR driver code
#undef _LPTMR_CODE
#endif

#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0) && (defined SUPPORT_PWM_MODULE || defined SUPPORT_CAPTURE)
/* =================================================================== */
/*                       FlexTimer/TPM pin connections                 */
/* =================================================================== */
    #include "iMX_timer_pins.h"
#endif

#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)                 // basic timer support based on FlexTimer
/* =================================================================== */
/*                              FlexTimer                              */
/* =================================================================== */
#define _FLEXTIMER_CODE
    #include "iMX_FLEXTIMER.h"                                           // include FlexTimer driver code
#undef _FLEXTIMER_CODE
#endif

#if defined SUPPORT_TIMER && defined SUPPORT_PWM_MODULE && (FLEX_TIMERS_AVAILABLE > 0)
/* =================================================================== */
/*                                 PWM                                 */
/* =================================================================== */
#define _PWM_CODE
    #include "iMX_PWM.h"                                                 // include PWM configuration code
#undef _PWM_CODE
#endif


#if defined SUPPORT_PDB && defined PDB_AVAILABLE
/* =================================================================== */
/*                    Programmable Delay Block (PDB)                   */
/* =================================================================== */
#define _PDB_CODE
    #include "iMX_PDB.h"                                                 // include PDB driver code
#undef _PDB_CODE
#endif


#if defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 0)
/* =================================================================== */
/*                        Key Board Interrupt                          */
/* =================================================================== */
#define _KBI_INTERRUPT_CODE
    #include "iMX_KBI.h"                                                 // include KBI interrupt driver code
#undef _KBI_INTERRUPT_CODE
#endif


#if defined SUPPORT_LOW_POWER && defined LLWU_AVAILABLE && defined SUPPORT_LLWU
/* =================================================================== */
/*                       Low Leakage Wake Up (LLWU)                    */
/* =================================================================== */
#define _LLWU_INTERRUPT_CODE
    #include "iMX_LLWU.h"                                                // include LLWU interrupt driver code
#undef _LLWU_INTERRUPT_CODE
#endif

#if defined SUPPORT_I2S_SAI && (I2S_AVAILABLE > 0)
/* =================================================================== */
/*                                I2S / SAI                            */
/* =================================================================== */
#define _I2S_CODE
    #include "iMX_I2S.h"                                                 // include I2S/SAI driver code
#undef _I2S_CODE
#endif


#if defined SUPPORT_PORT_INTERRUPTS
/* =================================================================== */
/*                           Port Interrupts                           */
/* =================================================================== */
#define _PORT_INTERRUPT_CODE
    #include "iMX_PORTS.h"                                               // include port interrupt driver code
#undef _PORT_INTERRUPT_CODE
#endif

#define _PORT_MUX_CODE
    #include "iMX_PORTS.h"                                               // include port pin multiplexing code
#undef _PORT_MUX_CODE


#if defined SUPPORT_ADC
/* =================================================================== */
/*                                 ADC                                 */
/* =================================================================== */
#define _ADC_INTERRUPT_CODE
    #include "iMX_ADC.h"                                                 // include driver code for ADC
#undef _ADC_INTERRUPT_CODE
#endif

#if defined SUPPORT_COMPARATOR
/* =================================================================== */
/*                            Comparator                               */
/* =================================================================== */
#define _CMP_INTERRUPT_CODE
    #include "iMX_CMP.h"                                                 // include driver code for comparator
#undef _CMP_INTERRUPT_CODE
#endif

#if defined MMDVSQ_AVAILABLE
/* =================================================================== */
/*                 Memory-Mapped Divide and Square Root                */
/* =================================================================== */
static volatile unsigned char ucMMDVSQ_in_use = 0;

// Warning - do not use from interrupt if it could disturb use already in operation (not protected)
//
extern unsigned short fnIntegerSQRT(unsigned long ulInput)
{
    register volatile unsigned long ulResult;
    MMDVSQ0_RCND = ulInput;                                              // write the radicand, which starts the calculation
    while ((MMDVSQ0_CSR & MMDVSQ0_CSR_BUSY) != 0) {                      // wait until the result is ready
    }
    #if defined _WINDOWS
    MMDVSQ0_RES = (unsigned long)sqrt(MMDVSQ0_RCND);                     // perfrom a traditional square root operation
    #endif
    ulResult = MMDVSQ0_RES;                                              // long word read is needed of the result of the square root calculation
    return ((unsigned short)ulResult);                                   // return the short word result
}

// This is safe to be used in interrupt routines since it will not disturb an operation in progress
//
extern unsigned long fnFastUnsignedModulo(unsigned long ulValue, unsigned long ulMod)
{
    if (ucMMDVSQ_in_use != 0) {                                          // if the module is busy we use a traditional calculation
        return (ulValue % ulMod);
    }
    else {
        unsigned long ulModulo;
        ucMMDVSQ_in_use = 1;                                             // protect the module from interrupts that may want to use it too
        MMDVSQ0_CSR = (MMDVSQ0_CSR_REM_REMAINDER | MMDVSQ0_CSR_USGN_UNSIGNED); // perform unsigned division and request the remainder (fast mode is enabled so we don't need to command a start)
        MMDVSQ0_DEND = ulValue;
        MMDVSQ0_DSOR = ulMod;
        while ((MMDVSQ0_CSR & MMDVSQ0_CSR_BUSY) != 0) {                  // wait until the calculation has completed
        }
    #if defined _WINDOWS
        MMDVSQ0_RES = (MMDVSQ0_DEND / MMDVSQ0_DSOR);
        MMDVSQ0_RES = (MMDVSQ0_DEND - (MMDVSQ0_RES * MMDVSQ0_DSOR));
    #endif
        ulModulo = MMDVSQ0_RES;                                          // remainder of the divide calculation
        ucMMDVSQ_in_use = 0;                                             // module free to use again
        return ulModulo;
    }
}

// This is safe to be used in interrupt routines since it will not disturb an operation in progress
//
extern signed long fnFastSignedModulo(signed long slValue, signed long slMod)
{
    if (ucMMDVSQ_in_use != 0) {                                          // if the module is busy we use a traditional calculation
        return (slValue % slMod);
    }
    else {
        signed long slModulo;
        ucMMDVSQ_in_use = 1;                                             // protect the module from interrupts that may want to use it too
        MMDVSQ0_CSR = (MMDVSQ0_CSR_REM_REMAINDER | MMDVSQ0_CSR_USGN_SIGNED); // perform signed division and request the remainder (fast mode is enabled so we don't need to command a start)
        MMDVSQ0_DEND = (signed long)slValue;
        MMDVSQ0_DSOR = (signed long)slMod;
        while ((MMDVSQ0_CSR & MMDVSQ0_CSR_BUSY) != 0) {                  // wait until the calculation has completed
        }
#if defined _WINDOWS
        MMDVSQ0_RES = ((signed long)MMDVSQ0_DEND / (signed long)MMDVSQ0_DSOR);
        MMDVSQ0_RES = (signed long)((signed long)MMDVSQ0_DEND - ((signed long)MMDVSQ0_RES * (signed long)MMDVSQ0_DSOR));
#endif
        slModulo = (signed long)MMDVSQ0_RES;                             // remainder of the divide calculation
        ucMMDVSQ_in_use = 0;                                             // module free to use again
        return slModulo;
    }
}

// This is safe to be used in interrupt routines since it will not disturb an operation in progress
//
extern unsigned long fnFastUnsignedIntegerDivide(unsigned long ulDivide, unsigned long ulBy)
{
    if (ucMMDVSQ_in_use != 0) {                                          // if the module is busy we use a traditional calculation
        return (ulDivide / ulBy);
    }
    else {
        unsigned long ulResult;
        ucMMDVSQ_in_use = 1;                                             // protect the module from interrupts that may want to use it too
        MMDVSQ0_CSR = (MMDVSQ0_CSR_REM_QUOTIENT | MMDVSQ0_CSR_USGN_UNSIGNED); // perform unsigned division and request the quotient (fast mode is enabled so we don't need to command a start)
        MMDVSQ0_DEND = ulDivide;
        MMDVSQ0_DSOR = ulBy;
        while ((MMDVSQ0_CSR & MMDVSQ0_CSR_BUSY) != 0) {                  // wait until the calculation has completed
        }
    #if defined _WINDOWS
        MMDVSQ0_RES = (MMDVSQ0_DEND / MMDVSQ0_DSOR);
    #endif
        ulResult = MMDVSQ0_RES;                                          // result of the divide calculation
        ucMMDVSQ_in_use = 0;                                             // module free to use again
        return ulResult;
    }
}

// This is safe to be used in interrupt routines since it will not disturb an operation in progress
//
extern signed long fnFastSignedIntegerDivide(signed long slDivide, signed long slBy)
{
    if (ucMMDVSQ_in_use != 0) {                                          // if the module is busy we use a traditional calculation
        return (slDivide / slBy);
    }
    else {
        signed long slResult;
        ucMMDVSQ_in_use = 1;                                             // protect the module from interrupts that may want to use it too
        MMDVSQ0_CSR = (MMDVSQ0_CSR_REM_QUOTIENT | MMDVSQ0_CSR_USGN_SIGNED); // perform signed division and request the quotient (fast mode is enabled so we don't need to command a start)
        MMDVSQ0_DEND = (unsigned long)slDivide;
        MMDVSQ0_DSOR = (unsigned long)slBy;
        while ((MMDVSQ0_CSR & MMDVSQ0_CSR_BUSY) != 0) {                  // wait until the calculation has completed
        }
    #if defined _WINDOWS
        MMDVSQ0_RES = ((signed long)MMDVSQ0_DEND / (signed long)MMDVSQ0_DSOR);
    #endif
        slResult = (signed long)MMDVSQ0_RES;                             // result of the divide calculation
        ucMMDVSQ_in_use = 0;                                             // module free to use again
        return slResult;
    }
}
#else
// If the command are used by processors that do not have co-processor for integer square root it will cause an exception when simulating
//
extern unsigned short fnIntegerSQRT(unsigned long ulInput)
{
    _EXCEPTION("Not supported on devices without co-processor!");
    return 0;
}

// Traditional calculation for compatibility
//
extern unsigned long fnFastUnsignedModulo(unsigned long ulValue, unsigned long ulMod)
{
    return (ulValue % ulMod);
}

// Traditional calculation for compatibility
//
extern signed long fnFastSignedModulo(signed long slValue, signed long slMod)
{
    return (slValue % slMod);
}

// Traditional calculation for compatibility
//
extern unsigned long fnFastUnsignedIntegerDivide(unsigned long ulDivide, unsigned long ulBy)
{
    return (ulDivide / ulBy);
}

// Traditional calculation for compatibility
//
extern signed long fnFastSignedIntegerDivide(signed long slDivide, signed long slBy)
{
    return (slDivide / slBy);
}
#endif

/* =================================================================== */
/*                General Peripheral/Interrupt Interface               */
/* =================================================================== */

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
#if defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 0)
    case KEYBOARD_INTERRUPT:
    #define _KBI_CONFIG_CODE
        #include "iMX_KBI.h"                                             // include KBI configuration code
    #undef _KBI_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_PORT_INTERRUPTS
    case PORT_INTERRUPT:
    #define _PORT_INT_CONFIG_CODE
        #include "iMX_PORTS.h"                                           // include port interrupt configuration code
    #undef _PORT_INT_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_LOW_POWER && defined LLWU_AVAILABLE && defined SUPPORT_LLWU
    case WAKEUP_INTERRUPT:
    #define _LLWU_CONFIG_CODE
        #include "iMX_LLWU.h"                                            // include LLWU configuration code
    #undef _LLWU_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_I2S_SAI && (I2S_AVAILABLE > 0)
    case I2S_SAI_INTERRUPT:
    #define _I2S_SAI_CONFIG_CODE
        #include "iMX_I2S.h"                                             // include I2S/SAI configuration code
    #undef _I2S_SAI_CONFIG_CODE
        break;
#endif
#if (defined SUPPORT_PITS || defined USB_HOST_SUPPORT) && !defined KINETIS_WITHOUT_PIT // up to 4 x 32bit PITs in most Kinetis devices
    case PIT_INTERRUPT:
    #define _PIT_CONFIG_CODE
        #include "iMX_PIT.h"                                             // include PIT configuration code
    #undef _PIT_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_TIMER && defined SUPPORT_PWM_MODULE && (FLEX_TIMERS_AVAILABLE > 0)
    case PWM_INTERRUPT:
    #define _PWM_CONFIG_CODE
        #include "iMX_PWM.h"                                             // include PWM configuration code
    #undef _PWM_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
    case TIMER_INTERRUPT:                                                // FlexTimer used in periodic or monostable interrupt mode (bus clock source)
    #define _FLEXTIMER_CONFIG_CODE
        #include "iMX_FLEXTIMER.h"                                       // include FlexTimer configuration code
    #undef _FLEXTIMER_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_PDB
    case PDB_INTERRUPT:
    #define _PDB_CONFIG_CODE
        #include "iMX_PDB.h"                                             // include PDM configuration code
    #undef _PDB_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_LPTMR && !defined TICK_USES_LPTMR && !defined KINETIS_KE // [prescale bypassed]
    case LPTMR_INTERRUPT:
    #define _LPTMR_CONFIG_CODE
        #include "iMX_LPTMR.h"                                           // include LPTMR configuration code
    #undef _LPTMR_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_DAC && (DAC_CONTROLLERS > 0)
    case DAC_INTERRUPT:
    #define _DAC_CONFIG_CODE
        #include "iMX_DAC.h"                                             // include DAC configuration code
    #undef _DAC_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_ADC
    case ADC_INTERRUPT:
    #define _ADC_CONFIG_CODE
        #include "iMX_ADC.h"                                             // include ADC configuration code
    #undef _ADC_CONFIG_CODE
        break;
#endif
#if defined SUPPORT_COMPARATOR
    case COMPARATOR_INTERRUPT:
    #define _CMP_CONFIG_CODE
        #include "iMX_CMP.h"                                             // include comparator configuration code
    #undef _CMP_CONFIG_CODE
        break;
#endif
    default: 
        _EXCEPTION("Attempting configuration of interrupt interface without appropriate support enabled!!");
        break;
    }
}


/* =================================================================== */
/*                     Dynamic Low Power Interface                     */
/* =================================================================== */

#if defined SUPPORT_LOW_POWER
    #include "iMX_low_power.h"                                           // include driver code for low power mode
#endif


/* =================================================================== */
/*                                 Reset                               */
/* =================================================================== */

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);             // request Cortex core reset, which will cause the software reset bit to be set in the mode controller for recognition after restart
#if !defined _WINDOWS
    FOREVER_LOOP() {}
#endif
}

#if defined CLKOUT_AVAILABLE && !defined KINETIS_WITH_PCC
extern int fnClkout(int iClockSource)
{
    unsigned long ulSIM_SOPT2 = (SIM_SOPT2 & ~(SIM_SOPT2_CLKOUTSEL_MASK)); // original control register value with clock source masked
    switch (iClockSource) {                                              // set the required clock source to be output on CLKOUT
    case FLASH_CLOCK_OUT:
    case BUS_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_FLASH;
        break;
    case LOW_POWER_OSCILLATOR_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_LPO;
        break;
    case INTERNAL_LIRC_CLOCK_OUT:                                        // same as INTERNAL_MCGIRCLK_CLOCK_OUT
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_MCGIRCLK;
        break;
    case EXTERNAL_OSCILLATOR_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_OSCERCLK0;
        break;
    #if defined KINETIS_HAS_IRC48M
    case INTERNAL_IRC48M_CLOCK_OUT:
        ulSIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL_IRC48M;
        break;
    #endif
    case RTC_CLOCK_OUT:
    #if defined KINETIS_KL03
        _CONFIG_PERIPHERAL(B, 13, (PB_13_RTC_CLKOUT | PORT_SRE_SLOW | PORT_DSE_LOW)); // configure the RTC_CLKOUT pin
    #elif defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66
        #if defined RTC_CLKOUT_ON_PTE_LOW
            _CONFIG_PERIPHERAL(E, 0, (PE_0_RTC_CLKOUT | PORT_SRE_SLOW | PORT_DSE_LOW)); // configure the RTC_CLKOUT pin (alt. 7)
        #else
            _CONFIG_PERIPHERAL(E, 26, (PE_26_RTC_CLKOUT | PORT_SRE_SLOW | PORT_DSE_LOW)); // configure the RTC_CLKOUT pin (alt. 6)
        #endif
    #endif
        return 0;
    #if defined KINETIS_K64
    case FLEXBUS_CLOCK_OUT:
        ulSIM_SOPT2 &= ~(SIM_SOPT2_CLKOUTSEL_IRC48M);
    #endif
    default:
        return -1;                                                       // invalid clock source
    }
    SIM_SOPT2 = ulSIM_SOPT2;                                             // select the clock source to be driven to the CLKOUT pin
    #if defined KINETIS_KL03
    _CONFIG_PERIPHERAL(A, 12, (PA_12_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin (PA_4_CLKOUT would be an alternative possibility)
    #elif defined KINETIS_KL05
    _CONFIG_PERIPHERAL(A, 15, (PA_15_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin (PA_4_CLKOUT would be an alternative possibility)
    #elif defined KINETIS_K64 && (PIN_COUNT == PIN_COUNT_144_PIN)
    _CONFIG_PERIPHERAL(A, 6, (PA_6_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin
    #else
    _CONFIG_PERIPHERAL(C, 3, (PC_3_CLKOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); // configure the CLKOUT pin
    #endif
    return 0;                                                            // valid clock source has been selected
}
#endif

#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
// Routine to request local SPI FLASH type
//
extern unsigned char fnSPI_Flash_available(void)
{
    return ucSPI_FLASH_Type[0];
}

    #if defined SPI_FLASH_MULTIPLE_CHIPS
extern unsigned char fnSPI_FlashExt_available(int iExtension)
{
    if (iExtension > SPI_FLASH_DEVICE_COUNT) {
        return 0;
    }
    return ucSPI_FLASH_Type[iExtension];
}
    #endif
#endif


#if defined USE_SDRAM
static void fnConfigureSDRAM(void)
{
/*  The following SDRAM devices are supported
    MT47H64M16HR-25                                                         used on K70 tower kit board
    MT46H32M16LFBF-5                                                        default
    MT46H32M16LFBF-6                                                        (compatible)
*/
    POWER_UP_ATOMIC(3, DDR);                                             // power up the DDR controller

    // The DDR controller requires MCGDDRCLK2, which is generated by PLL1 ({90} moved to _LowLevelInit() so that it can be shared by FS USB)
    //
    #define KINETIS_DDR_SPEED ((_EXTERNAL_CLOCK * CLOCK_MUL_1)/CLOCK_DIV_1/2) // speed of DDR clock assuming external oscillator used

    #if defined MT47H64M16HR                                             // synchronous DDR2
    SIM_MCR |= (SIM_MCR_DDRCFG_DDR2 | SIM_MCR_DDRPEN);                   // enable all DDR I/O pins at full-strength (slew-rate)
    DDR_RCR = (DDR_RCR_RST);                                             // command reset of the DDR PHY (the register always read 0x00000000)
    DDR_PAD_CTRL = (DDR_PAD_CTRL_FIXED | DDR_PAD_CTRL_SPARE_DLY_CTRL_10_BUFFERS | DDR_PAD_CTRL_PAD_ODT_CS0_75_OHMS | 0x00030000); // note that 0x00030000 is not documented but taken from freescale example
    DDR_CR00 = DDR_CR00_DDRCLS_DDR2;                                      // set the class type to DDR2
    DDR_CR02 = ((2 << DDR_CR02_INITAREF_SHIFT) | (49 << DDR_CR02_TINIT_SHIFT));
    DDR_CR03 = ((2 << DDR_CR03_TCCD_SHIFT) | (2 << DDR_CR03_WRLAT_SHIFT) | (5 << DDR_CR03_LATGATE_SHIFT) | (6 << DDR_CR03_LATLIN_SHIFT));
    DDR_CR04 = ((2 << DDR_CR04_TBINT_SHIFT) | (2 << DDR_CR04_TRRD_SHIFT) | (9 << DDR_CR04_TRC_SHIFT) | (6 << DDR_CR04_TRASMIN_SHIFT));
    DDR_CR05 = ((2 << DDR_CR05_TWTR_SHIFT) | (3 << DDR_CR05_TRP_SHIFT) | (2 << DDR_CR05_TRTP_SHIFT) | (2 << DDR_CR05_TMRD_SHIFT));
    DDR_CR06 = ((2 << DDR_CR06_TMOD_SHIFT) | (36928 << DDR_CR06_TRASMAX_SHIFT) | 0x02000000); // note that 0x02000000 is not documented but taken from freescale example
    DDR_CR07 = (DDR_CR07_CCAPEN | (3 << DDR_CR07_TCKESR_SHIFT) | (3 << DDR_CR07_CLKPW_SHIFT));
    DDR_CR08 = ((5 << DDR_CR08_TDAL_SHIFT) | (3 << DDR_CR08_TWR_SHIFT) | (2 << DDR_CR08_TRASDI_SHIFT) | DDR_CR08_TRAS);
    DDR_CR09 = ((2 << DDR_CR09_BSTLEN_SHIFT) | (200 << DDR_CR09_TDLL_SHIFT));
    DDR_CR10 = ((3 << DDR_CR10_TRPAB_SHIFT) | (50 << DDR_CR10_TCPD_SHIFT) | (7 << DDR_CR10_TFAW_SHIFT));
    DDR_CR11 = DDR_CR11_TREFEN;                                          // enable refresh commands
    DDR_CR12 = ((49 << DDR_CR12_TRFC_SHIFT) | (1170 << DDR_CR12_TREF_SHIFT));
    DDR_CR13 = (5 << DDR_CR13_TREFINT_SHIFT);
    DDR_CR14 = ((200 << DDR_CR14_TXSR_SHIFT) | (2 << DDR_CR14_TPDEX_SHIFT));
    DDR_CR15 = (50 << DDR_CR15_TXSNR_SHIFT);
    DDR_CR16 = DDR_CR16_QKREF;
    DDR_CR20 = ((3 << DDR_CR20_CKSRX_SHIFT) | (3 << DDR_CR20_CKSRE_SHIFT));
    DDR_CR21 = 0x00040232;
    DDR_CR22 = 0;
    DDR_CR23 = 0x00040302;
    DDR_CR25 = ((10 << DDR_CR25_APREBIT_SHIFT) | (1 << DDR_CR25_COLSIZ_SHIFT) | (2 << DDR_CR25_ADDPINS_SHIFT) | DDR_CR25_BNK8);
    DDR_CR26 = (DDR_CR26_ADDCOL | DDR_CR26_BNKSPT | (255 << DDR_CR26_CMDAGE_SHIFT) | (255 << DDR_CR26_AGECNT_SHIFT));
    DDR_CR27 = (DDR_CR27_PLEN | DDR_CR27_PRIEN | DDR_CR27_RWEN | DDR_CR27_SWPEN);
    DDR_CR28 = (DDR_CR28_CSMAP | 0x00000002);                            // note that 0x00000002 is not documented but taken from freescale example
    DDR_CR29 = 0;
    DDR_CR30 = 1;
    DDR_CR34 = 0x02020101;
    DDR_CR36 = 0x01010201;
    DDR_CR37 = ((0 << DDR_CR37_R2RSAME_SHIFT) | (2 << DDR_CR37_R2WSAME_SHIFT) | (0 << DDR_CR37_W2RSAME_SHIFT) | (0 << DDR_CR37_W2WSAME_SHIFT));
    DDR_CR38 = (32 << DDR_CR38_PWRCNT_SHIFT);
    DDR_CR39 = ((1 << DDR_CR39_WP0_SHIFT) | (1 << DDR_CR39_RP0_SHIFT) | (32 << DDR_CR39_P0RDCNT_SHIFT));
    DDR_CR40 = ((32 << DDR_CR40_P1WRCNT_SHIFT) | DDR_CR40_P0TYP_ASYNC);
    DDR_CR41 = ((1 << DDR_CR41_WP1_SHIFT) | (1 << DDR_CR41_RP1_SHIFT) | (32 << DDR_CR41_P1RDCNT_SHIFT));
    DDR_CR42 = ((32 << DDR_CR42_P2WRCNT_SHIFT) | DDR_CR42_P1TYP_ASYNC);
    DDR_CR43 = ((1 << DDR_CR43_WP2_SHIFT) | (1 << DDR_CR43_RP2_SHIFT) | (32 << DDR_CR43_P2RDCNT_SHIFT));
    DDR_CR44 = (DDR_CR44_P2TYP_ASYNC);
    DDR_CR45 = ((3 << DDR_CR45_P0PRI0_SHIFT) | (3 << DDR_CR45_P0PRI1_SHIFT) | (3 << DDR_CR45_P0PRI2_SHIFT) | (3 << DDR_CR45_P0PRI3_SHIFT));
    DDR_CR46 = ((2 << DDR_CR46_P1PRI0_SHIFT) | (100 << DDR_CR46_P0PRIRLX_SHIFT) | (1 << DDR_CR46_P0ORD_SHIFT));
    DDR_CR47 = ((2 << DDR_CR47_P1PRI1_SHIFT) | (2 << DDR_CR47_P1PRI2_SHIFT) | (2 << DDR_CR47_P1PRI3_SHIFT) | (1 << DDR_CR47_P1ORD_SHIFT));
    DDR_CR48 = ((1 << DDR_CR48_P2PRI0_SHIFT) | (100 << DDR_CR48_P1PRIRLX_SHIFT) | (1 << DDR_CR48_P2PRI1_SHIFT));
    DDR_CR49 = ((2 << DDR_CR49_P2ORD_SHIFT) | (1 << DDR_CR49_P2PRI2_SHIFT) | (1 << DDR_CR49_P2PRI3_SHIFT));
    DDR_CR50 = (100 << DDR_CR50_P2PRIRLX_SHIFT);
    DDR_CR52 = ((2 << DDR_CR52_RDDTENBAS_SHIFT) | (6 << DDR_CR52_PHYRDLAT_SHIFT) | (2 << DDR_CR52_PYWRLTBS_SHIFT));
    DDR_CR53 = (968 << DDR_CR53_CRTLUDMX_SHIFT);
    DDR_CR54 = ((968 << DDR_CR54_PHYUPDTY0_SHIFT) | (968 << DDR_CR54_PHYUPDTY1_SHIFT));
    DDR_CR55 = ((968 << DDR_CR55_PHYUPDTY2_SHIFT) | (968 << DDR_CR55_PHYUPDTY3_SHIFT));
    DDR_CR56 = ((2 << DDR_CR56_WRLATADJ_SHIFT) | (3 << DDR_CR56_RDLATADJ_SHIFT) | (968 << DDR_CR56_PHYUPDRESP_SHIFT));
    DDR_CR57 = ((1 << DDR_CR57_CLKENDLY_SHIFT) | (2 << DDR_CR57_CMDDLY_SHIFT) | DDR_CR57_ODTALTEN);
    #else                                                                // MT46H32M16LFBF-5/6 - low power DDR
    SIM_MCR |= (SIM_MCR_DDRCFG_LPDDR_2 | SIM_MCR_DDRPEN);                // enable all DDR I/O pins at low-power half-strength (slew-rate)
    DDR_RCR = (DDR_RCR_RST);                                             // command reset of the DDR PHY (the register always read 0x00000000)
    DDR_PAD_CTRL = (DDR_PAD_CTRL_FIXED | DDR_PAD_CTRL_SPARE_DLY_CTRL_10_BUFFERS | DDR_PAD_CTRL_PAD_ODT_CS0_ODT_DISABLED);
    DDR_CR00 = DDR_CR00_DDRCLS_LPDDR;                                    // set the class type to low power DDR
    DDR_CR02 = ((2 << DDR_CR02_INITAREF_SHIFT) | (50 << DDR_CR02_TINIT_SHIFT));
    DDR_CR03 = ((2 << DDR_CR03_TCCD_SHIFT) | (1 << DDR_CR03_WRLAT_SHIFT) | (6 << DDR_CR03_LATGATE_SHIFT) | (6 << DDR_CR03_LATLIN_SHIFT));
        #if KINETIS_DDR_SPEED >= 150000000                               // suitable settings for 150MHz operation
    DDR_CR04 = ((1 << DDR_CR04_TBINT_SHIFT) | (2 << DDR_CR04_TRRD_SHIFT) | (10 << DDR_CR04_TRC_SHIFT) | (7 << DDR_CR04_TRASMIN_SHIFT));
    DDR_CR06 = ((0 << DDR_CR06_TMOD_SHIFT) | (10500 << DDR_CR06_TRASMAX_SHIFT)); // 70us time row access maximum
    DDR_CR08 = ((6 << DDR_CR08_TDAL_SHIFT) | (3 << DDR_CR08_TWR_SHIFT) | (3 << DDR_CR08_TRASDI_SHIFT));
    DDR_CR10 = ((3 << DDR_CR10_TRPAB_SHIFT) | (30000 << DDR_CR10_TCPD_SHIFT) | (0 << DDR_CR10_TFAW_SHIFT));
    DDR_CR12 = ((15 << DDR_CR12_TRFC_SHIFT) | (1170 << DDR_CR12_TREF_SHIFT)); // 7.8us DRAM cycles between refresh commands
    DDR_CR14 = ((19 << DDR_CR14_TXSR_SHIFT) | (2 << DDR_CR14_TPDEX_SHIFT));
    DDR_CR15 = (19 << DDR_CR15_TXSNR_SHIFT);
        #elif (KINETIS_DDR_SPEED == 120000000) || (KINETIS_DDR_SPEED == 125000000) // suitable settings for 120MHz/125MHz operation
    DDR_CR04 = ((1 << DDR_CR04_TBINT_SHIFT) | (2 << DDR_CR04_TRRD_SHIFT) | (8 << DDR_CR04_TRC_SHIFT) | (6 << DDR_CR04_TRASMIN_SHIFT));
    DDR_CR06 = ((0 << DDR_CR06_TMOD_SHIFT) | (8400 << DDR_CR06_TRASMAX_SHIFT)); // 70us time row access maximum
    DDR_CR08 = ((4 << DDR_CR08_TDAL_SHIFT) | (2 << DDR_CR08_TWR_SHIFT) | (3 << DDR_CR08_TRASDI_SHIFT));
    DDR_CR10 = ((2 << DDR_CR10_TRPAB_SHIFT) | (24000 << DDR_CR10_TCPD_SHIFT) | (0 << DDR_CR10_TFAW_SHIFT));
    DDR_CR12 = ((12 << DDR_CR12_TRFC_SHIFT) | (936 << DDR_CR12_TREF_SHIFT)); // 7.8us DRAM cycles between refresh commands
    DDR_CR14 = ((15 << DDR_CR14_TXSR_SHIFT) | (2 << DDR_CR14_TPDEX_SHIFT));
    DDR_CR15 = (15 << DDR_CR15_TXSNR_SHIFT);
        #else
            #error "DDR speed should be 120MHz, 125MHz or 150MHz so that correct settings can be made "
        #endif
    DDR_CR05 = ((2 << DDR_CR05_TWTR_SHIFT) | (3 << DDR_CR05_TRP_SHIFT) | (2 << DDR_CR05_TRTP_SHIFT) | (2 << DDR_CR05_TMRD_SHIFT));
    DDR_CR07 = (DDR_CR07_CCAPEN | (1 << DDR_CR07_TCKESR_SHIFT) | (1 << DDR_CR07_CLKPW_SHIFT));
    DDR_CR09 = ((3 << DDR_CR09_BSTLEN_SHIFT) | (0 << DDR_CR09_TDLL_SHIFT));
    DDR_CR11 = DDR_CR11_TREFEN;                                          // enable refresh commands
    DDR_CR16 = DDR_CR16_QKREF;
    DDR_CR20 = ((3 << DDR_CR20_CKSRX_SHIFT) | (3 << DDR_CR20_CKSRE_SHIFT));

    // Memory chip mode register settings
    //
    #define LPDDR_MR_CL         3
    #define LPDDR_MR_BURST_TYPE 0
    #define LPDDR_MR_BL         3
    #define MT46H_CHIP_MODE_REGISTER ((LPDDR_MR_CL << 4) | (LPDDR_MR_BURST_TYPE << 3) | (LPDDR_MR_BL << 0))

    #define LPDDR_EMR_STRENGTH 1
    #define LPDDR_EMR_PASR     0
    #define MT46H_EXTENDED_MODE_REGISTER ((LPDDR_EMR_STRENGTH << 5) | (LPDDR_EMR_PASR << 0))

    DDR_CR21 = ((0 << DDR_CR21_MR1DATA0_SHIFT) | (MT46H_CHIP_MODE_REGISTER << DDR_CR21_MR0DATA0_SHIFT));
    DDR_CR22 = ((0 << DDR_CR22_MR3DATA0_SHIFT) | (MT46H_EXTENDED_MODE_REGISTER << DDR_CR22_MR2DATA0_SHIFT));
    DDR_CR25 = ((10 << DDR_CR25_APREBIT_SHIFT) | (1 << DDR_CR25_COLSIZ_SHIFT) | (3 << DDR_CR25_ADDPINS_SHIFT));
    DDR_CR26 = (DDR_CR26_ADDCOL | DDR_CR26_BNKSPT | (255 << DDR_CR26_CMDAGE_SHIFT) | (255 << DDR_CR26_AGECNT_SHIFT));
    DDR_CR27 = (DDR_CR27_PLEN | DDR_CR27_PRIEN | DDR_CR27_RWEN | DDR_CR27_SWPEN);
    DDR_CR28 = (DDR_CR28_CSMAP);
    DDR_CR29 = 0;
    DDR_CR30 = 0;
    DDR_CR34 = 0;
    DDR_CR37 = ((3 << DDR_CR37_R2RSAME_SHIFT) | (2 << DDR_CR37_R2WSAME_SHIFT) | (0 << DDR_CR37_W2RSAME_SHIFT) | (0 << DDR_CR37_W2WSAME_SHIFT));
    DDR_CR38 = (32 << DDR_CR38_PWRCNT_SHIFT);
    DDR_CR39 = ((1 << DDR_CR39_WP0_SHIFT) | (1 << DDR_CR39_RP0_SHIFT) | (32 << DDR_CR39_P0RDCNT_SHIFT));
    DDR_CR41 = ((1 << DDR_CR41_WP1_SHIFT) | (1 << DDR_CR41_RP1_SHIFT) | (32 << DDR_CR41_P1RDCNT_SHIFT));
    DDR_CR43 = ((1 << DDR_CR43_WP2_SHIFT) | (1 << DDR_CR43_RP2_SHIFT) | (32 << DDR_CR43_P2RDCNT_SHIFT));
        #if defined DDRPORT_SYNC
    DDR_CR40 = ((32 << DDR_CR40_P1WRCNT_SHIFT) | DDR_CR40_P0TYP_SYNC);
    DDR_CR42 = ((32 << DDR_CR42_P2WRCNT_SHIFT) | DDR_CR42_P1TYP_SYNC);
    DDR_CR44 = (DDR_CR44_P2TYP_SYNC);
        #else
    DDR_CR40 = ((32 << DDR_CR40_P1WRCNT_SHIFT) | DDR_CR40_P0TYP_ASYNC);
    DDR_CR42 = ((32 << DDR_CR42_P2WRCNT_SHIFT) | DDR_CR42_P1TYP_ASYNC);
    DDR_CR44 = (DDR_CR44_P2TYP_ASYNC);
        #endif
    DDR_CR45 = ((3 << DDR_CR45_P0PRI0_SHIFT) | (3 << DDR_CR45_P0PRI1_SHIFT) | (3 << DDR_CR45_P0PRI2_SHIFT) | (3 << DDR_CR45_P0PRI3_SHIFT));
    DDR_CR46 = ((2 << DDR_CR46_P1PRI0_SHIFT) | (100 << DDR_CR46_P0PRIRLX_SHIFT) | (1 << DDR_CR46_P0ORD_SHIFT));
    DDR_CR47 = ((2 << DDR_CR47_P1PRI1_SHIFT) | (2 << DDR_CR47_P1PRI2_SHIFT) | (2 << DDR_CR47_P1PRI3_SHIFT) | (1 << DDR_CR47_P1ORD_SHIFT));
    DDR_CR48 = ((1 << DDR_CR48_P2PRI0_SHIFT) | (100 << DDR_CR48_P1PRIRLX_SHIFT) | (1 << DDR_CR48_P2PRI1_SHIFT));
    DDR_CR49 = ((2 << DDR_CR49_P2ORD_SHIFT) | (1 << DDR_CR49_P2PRI2_SHIFT) | (1 << DDR_CR49_P2PRI3_SHIFT));
    DDR_CR50 = (100 << DDR_CR50_P2PRIRLX_SHIFT);
    DDR_CR52 = ((1 << DDR_CR52_RDDTENBAS_SHIFT) | (6 << DDR_CR52_PHYRDLAT_SHIFT) | (1 << DDR_CR52_PYWRLTBS_SHIFT));
    DDR_CR53 = (968 << DDR_CR53_CRTLUDMX_SHIFT);
    DDR_CR54 = ((968 << DDR_CR54_PHYUPDTY0_SHIFT) | (968 << DDR_CR54_PHYUPDTY1_SHIFT));
    DDR_CR55 = ((968 << DDR_CR55_PHYUPDTY2_SHIFT) | (968 << DDR_CR55_PHYUPDTY3_SHIFT));
    DDR_CR56 = ((2 << DDR_CR56_WRLATADJ_SHIFT) | (3 << DDR_CR56_RDLATADJ_SHIFT) | (968 << DDR_CR56_PHYUPDRESP_SHIFT));
    DDR_CR57 = ((1 << DDR_CR57_CLKENDLY_SHIFT) | (2 << DDR_CR57_CMDDLY_SHIFT));
    #endif

    fnDelayLoop(0);

    DDR_CR00 = (DDR_CR00_DDRCLS_DDR2 | DDR_CR00_START);                  // start command processing
    while ((DDR_CR30 & DDR_CR30_INT_DRAM_INIT) == 0) {                   // wait for completion
    #if defined _WINDOWS
        DDR_CR30 |= DDR_CR30_INT_DRAM_INIT;
    #endif
    }
    MCM_CR |= (MCM_CR_DDR_SIZE_128MB);                                   // enable DDR address size translation
}
#endif

#if !defined _MINIMUM_IRQ_INITIALISATION && !defined NMI_IN_FLASH
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

    #if !defined RUN_IN_FREE_RTOS
static void irq_SVCall(void)
{
}

static void irq_pend_sv(void)
{
}
    #endif
#endif
#if !defined _MINIMUM_IRQ_INITIALISATION || defined NMI_IN_FLASH
static void irq_NMI(void)
{
}
#endif


// Default handler to indicate processor input from unconfigured source
//
static void irq_default(void)
{
    _EXCEPTION("undefined interrupt!!!");
}


#if defined (_GNU)
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

// Variable initialisation
//
extern void __init_gnu_data(void)
{
    unsigned char *ptrData;
    unsigned long ulInitDataLength;
    #if !defined _RAM_DEBUG
    const unsigned char *ptrFlash = &__data_load_start__;
    ulInitDataLength = (&__data_end__ - &__data_start__);
    ptrData = &__data_start__;
    while (ulInitDataLength-- != 0) {                                    // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrFlash != ptrData) {                                           // if a move is required
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength-- != 0) {                                // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }
    #endif
    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength-- != 0) {                                    // initialise bss
        *ptrData++ = 0;
    }
}
#endif

#if defined WDOG_STCTRLL || defined WDOG_CS1                             // watchdog interrupt
static void wdog_irq(void)
{
    #if defined WDOG_STCTRLL
    WDOG_STCTRLL = (WDOG_STCTRLL_INTFLG | WDOG_STCTRLL_RES1);            // clear interrupt flag
        #if defined _WINDOWS
    WDOG_STCTRLL = 0;
        #endif
    #elif defined WDOG_CS1
    WDOG_CS2 |= WDOG_CS2_FLG;                                            // clear interrupt flag
        #if defined _WINDOWS
    WDOG_CS2 &= ~WDOG_CS2_FLG;
        #endif
    #endif
    *BOOT_MAIL_BOX = 0x9876;                                             // set a pattern to the boot mailbox to show that the watchdog interrupt took place
}
#endif


// Perform very low level initialisation - called by the start-up code
//
static void _LowLevelInit(void)
{
#if !defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE *ptrVect;
    #if !defined _MINIMUM_IRQ_INITIALISATION
    void (**processor_ints)(void);
    #endif
#endif
#if (defined KINETIS_K64 || (defined KINETIS_K24 && (SIZE_OF_FLASH == (1024 * 1024)))) && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC_FLL || defined RUN_FROM_HIRC) // older K64 devices require the IRC48M to be switched on by the USB module
    #define IRC48M_TIMEOUT 1000
    int iIRC48M_USB_control = 0;
#endif
#if !defined _COMPILE_IAR
    fnConfigWdogs();
    // Disable all clocks by default - they will be enabled only when needed
    //
    fnDisable_iMX_RT_Clocks();
    #if defined SUPPORT_LOW_POWER && (defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000))
        #if defined SUPPORT_LPTMR && LPTMR_AVAILABLE > 0                 // ensure no interrupts pending after waking from VLLS modes via LPTMR
    POWER_UP_ATOMIC(5, LPTMR0);                                          // power up the low power timer
    PMC_REGSC = PMC_REGSC_ACKISO;                                        // acknowledge the isolation mode to set certain peripherals and I/O pads back to normal run state
    LPTMR0_CSR = 0;                                                      // clear possible pending interrupt and stop the timer
    POWER_DOWN_ATOMIC(5, LPTMR0);                                        // power down the low power timer
    fnClearPending(irq_LPTMR0_ID);
        #else
    PMC_REGSC = PMC_REGSC_ACKISO;                                        // acknowledge the isolation mode to set certain peripherals and I/O pads back to normal run state
        #endif
    #endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
#endif
#if defined KINETIS_KW2X
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, RST_B, 0, (PORT_SRE_FAST | PORT_DSE_LOW)); // drive the modem reset input low
#endif
#if defined USER_STARTUP_CODE                                            // allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
#endif
#if defined ERRATA_ID_2644 && !defined KINETIS_FLEX
    FMC_PFB0CR &= ~(BANK_DPE | BANKIPE);                                 // disable speculation logic
    FMC_PFB1CR &= ~(BANK_DPE | BANKIPE);
#endif
#if defined ERRATA_ID_2647 && !defined KINETIS_FLEX && (SIZE_OF_FLASH > (256 * 1024))
    FMC_PFB0CR &= ~(BANKDCE | BANKICE | BANKSEBE);                       // disable cache
    FMC_PFB1CR &= ~(BANKDCE | BANKICE | BANKSEBE);
#endif
#if defined ERRATA_ID_9005
    CORTEX_ACTLR = CORTEX_ACTLR_DISDEFWBUF;                              // disable write buffer to avoid potential problem of a mismatch between the interrupt acknowledged by the interrupt controller and the vector fetched by the processor (only when MCU not used)
#endif
// Configure clock generator
//
#if defined _KINETIS && defined HIGH_SPEED_RUN_MODE_AVAILABLE && !defined USE_HIGH_SPEED_RUN_MODE
    if (SMC_PMSTAT == SMC_PMSTAT_HSRUN) {                                // if in high speed run mode we switch back to run mode
        SMC_PMCTRL = (SMC_PMCTRL_STOPM_NORMAL | SMC_PMCTRL_RUNM_NORMAL);
    }
#endif
// Include clock configuration code
//
#include "iMX_K_CLOCK.h"                                                 // clock configuration
#if defined CLKOUT_AVAILABLE && !defined KINETIS_WITH_PCC                // select the clock signal to be driven on CLKOUT pin
    #if defined KINETIS_K64
      //fnClkout(FLEXBUS_CLOCK_OUT);                                     // select the clock to monitor on CLKOUT
    #endif
    #if defined LOW_POWER_OSCILLATOR_CLOCK_OUT
      //fnClkout(LOW_POWER_OSCILLATOR_CLOCK_OUT);
    #endif
  //fnClkout(INTERNAL_IRC48M_CLOCK_OUT);
  //fnClkout(INTERNAL_LIRC_CLOCK_OUT);                                   // equivalent to INTERNAL_MCGIRCLK_CLOCK_OUT
  //fnClkout(EXTERNAL_OSCILLATOR_CLOCK_OUT);
  //fnClkout(RTC_CLOCK_OUT);
#endif
#if defined KINETIS_KL && defined ROM_BOOTLOADER && defined BOOTLOADER_ERRATA
    if ((RCM_MR & (RCM_MR_BOOTROM_BOOT_FROM_ROM_BOOTCFG0 | RCM_MR_BOOTROM_BOOT_FROM_ROM_FOPT7)) != 0) { // if the reset was via the ROM loader
        // PORT clock gate, pin mux and peripheral registers are not reset to default values on ROM exit
        //
        if (IS_POWERED_UP(5, PORTA) != 0) {
            PORTA_PCR1 = PORT_PS_UP_ENABLE;                              // set LPUART0, I2C0 and SPI0 ports back to defaults
            PORTA_PCR2 = PORT_PS_UP_ENABLE;
        }
        if (IS_POWERED_UP(5, PORTB) != 0) {
            PORTB_PCR0 = PORT_PS_UP_ENABLE;
            PORTB_PCR1 = PORT_PS_UP_ENABLE;
        }
    #if PORTS_AVAILABLE > 2
        if (IS_POWERED_UP(5, PORTC) != 0) {
            PORTC_PCR4 = PORT_PS_UP_ENABLE;
            PORTC_PCR5 = PORT_PS_UP_ENABLE;
            PORTC_PCR6 = PORT_PS_UP_ENABLE;
            PORTC_PCR7 = PORT_PS_UP_ENABLE;
        }
    #endif
        POWER_DOWN(5, (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB | SIM_SCGC5_PORTC | SIM_SCGC5_PORTD | SIM_SCGC5_PORTE | SIM_SCGC5_LPUART0)); // power down ports and LPUART0
        POWER_DOWN(4, (SIM_SCGC4_I2C0 | SIM_SCGC4_SPI0));                // power down I2C0 and SPI0
    #if !defined KINETIS_WITH_PCC
        SIM_SOPT2 = 0;                                                   // set default LPUART0 clock source 
    #endif
        IRQ0_31_CER = 0xffffffff;                                        // disable and clear all possible pending interrupts
        IRQ32_63_CER = 0xffffffff;
        IRQ64_95_CER = 0xffffffff;
        IRQ0_31_CPR = 0xffffffff;
        IRQ32_63_CPR = 0xffffffff;
        IRQ64_95_CPR = 0xffffffff;
        INIT_WATCHDOG_DISABLE();                                         // reinitialise ports that may have been disabled
        INIT_WATCHDOG_LED();
        // UART cannot work at 57600 baudrate with default core clock (not worked around)
        //
        // COP is disabled in ROM and it cannot be re-enabled by an application that is jumped to from ROM
        // - cannot be worked around; advise to use FTFL_FOPT_BOOTSRC_SEL_FLASH together with FTFL_FOPT_BOOTPIN_OPT_ENABLE
        //
    }
#endif
#if defined INTERRUPT_VECTORS_IN_FLASH
    VECTOR_TABLE_OFFSET_REG = ((unsigned long)&__vector_table);
#else
    #if defined _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
    #else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
    #endif
    #if !defined _MINIMUM_IRQ_INITIALISATION
    ptrVect->ptrNMI           = irq_NMI;
    ptrVect->ptrHardFault     = irq_hard_fault;
    ptrVect->ptrMemManagement = irq_memory_man;
    ptrVect->ptrBusFault      = irq_bus_fault;
    ptrVect->ptrUsageFault    = irq_usage_fault;
    ptrVect->ptrDebugMonitor  = irq_debug_monitor;
    ptrVect->ptrSysTick       = irq_default;
        #if defined RUN_IN_FREE_RTOS
    ptrVect->ptrPendSV        = xPortPendSVHandler;                      // FreeRTOS's PendSV handler
    ptrVect->ptrSVCall        = vPortSVCHandler;                         // FreeRTOS's SCV handler
    ptrVect->reset_vect.ptrResetSP = (void *)(RAM_START_ADDRESS + (SIZE_OF_RAM - NON_INITIALISED_RAM_SIZE)); // the stack pointer value will be taken from the vector base area so enter it in SRAM
        #else
    ptrVect->ptrPendSV        = irq_pend_sv;
    ptrVect->ptrSVCall        = irq_SVCall;
        #endif
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;    // fill all processor specific interrupts with a default handler
    do {
        *processor_ints = irq_default;
        if (processor_ints == (void (**)(void))&ptrVect->processor_interrupts.LAST_PROCESSOR_IRQ) {
            break;
        }
        processor_ints++;
    } FOREVER_LOOP();
    #else
        #if defined NMI_IN_FLASH
    ptrVect->ptrNMI           = irq_NMI;
        #else
    ptrVect->ptrNMI           = irq_default;
        #endif
    ptrVect->ptrHardFault     = irq_default;
    ptrVect->ptrMemManagement = irq_default;
    ptrVect->ptrBusFault      = irq_default;
    ptrVect->ptrUsageFault    = irq_default;
    ptrVect->ptrDebugMonitor  = irq_default;
    ptrVect->ptrPendSV        = irq_default;
    ptrVect->ptrSVCall        = irq_default;
    #endif
    VECTOR_TABLE_OFFSET_REG   = (unsigned long)ptrVect;                  // position the vector table at the bottom of RAM
#endif
#if defined (_GNU)
    __init_gnu_data();                                                   // initialise variables
#endif
#if defined USE_SDRAM
    fnConfigureSDRAM();                                                  // configure SDRAM
#endif
#if defined _WINDOWS && !defined INTERRUPT_VECTORS_IN_FLASH              // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the vector table size setting!!");
    }
    #if defined USE_SECTION_PROGRAMMING
    memset(ucFlexRam, 0xff, sizeof(ucFlexRam));                          // when used as data flash the flex ram is initialised to all 0xff
    #endif
#endif
    CPACR |= (0xf << 20);                                                // enable access to FPU
#if defined SUPPORT_LOW_POWER
    #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_KM || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
        #if !defined SMC_PMPROT_LOW_POWER_LEVEL
            #if defined HIGH_SPEED_RUN_MODE_AVAILABLE
                #define SMC_PMPROT_LOW_POWER_LEVEL (SMC_PMPROT_AVLLS | SMC_PMPROT_ALLS | SMC_PMPROT_AVLP | SMC_PMPROT_AHSRUN) // allow all low power modes if nothing else defined
            #else
                #define SMC_PMPROT_LOW_POWER_LEVEL (SMC_PMPROT_AVLLS | SMC_PMPROT_ALLS | SMC_PMPROT_AVLP) // allow all low power modes if nothing else defined
            #endif
        #endif
    SMC_PMPROT = SMC_PMPROT_LOW_POWER_LEVEL;
    #elif !defined KINETIS_KE && !defined KINETIS_KEA
        #if !defined MC_PMPROT_LOW_POWER_LEVEL
            #define MC_PMPROT_LOW_POWER_LEVEL (MC_PMPROT_AVLP | MC_PMPROT_ALLS | MC_PMPROT_AVLLS1 | MC_PMPROT_AVLLS2 | MC_PMPROT_AVLLS3) // allow all low power modes if nothing else defined
        #endif
    MC_PMPROT = MC_PMPROT_LOW_POWER_LEVEL;
    #endif
    #if defined ERRATA_ID_8068 && !defined SUPPORT_RTC                   // if low power mode is to be used but the RTC will not be initialised clear the RTC invalid flag to avoid the low power mode being blocked when e8068 is present
    POWER_UP_ATOMIC(6, RTC);                                             // temporarily enable the RTC module
    RTC_TSR = 0;                                                         // clear the RTC invalid flag with a write of any value to RTC_TSR
    POWER_DOWN_ATOMIC(6, RTC);                                           // power down the RTC again
    #endif
#endif
#if defined _WINDOWS
    fnUpdateOperatingDetails();                                          // update operating details to be displayed in the simulator
#endif
#if defined SET_POWER_MODE
    SET_POWER_MODE();
#endif
 #if defined WDOG_STCTRLL || defined WDOG_CS1
    #if !defined irq_WDOG_ID && defined INTMUX0_AVAILABLE
    fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_WDOG0), INTMUX0_PERIPHERAL_WDOG0_EWM, wdog_irq);// test WDOG interrupt - based on INTMUX
    #else
    fnEnterInterrupt(irq_WDOG_ID, 0, wdog_irq);                          // test WDOG interrupt (highest priority)
    #endif
#endif
#if defined DWT_CYCCNT && defined USE_CORTEX_CYCLE_COUNTER
    DEMCR |= DHCSR_TRCENA;                                               // enable trace for DWT features
    #if defined ARM_MATH_CM7
    DWT_LAR = DWT_LAR_UNLOCK;                                            // unlock access to DWT registers
    #endif
    DWT_CYCCNT = 0;                                                      // reset the cycle count value
    DWT_CTRL |= DWT_CTRL_CYCCNTENA;                                      // enable the cycle counter
#endif
}


#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #if !defined _WINDOWS
        #if defined ARM_MATH_CM0PLUS                                     // {67} cortex-M0+ assembler code
    asm(" ldr r1, [r0,#0]");                                             // get the stack pointer value from the program's reset vector
    asm(" mov sp, r1");                                                  // copy the value to the stack pointer
    asm(" ldr r0, [r0,#4]");                                             // get the program counter value from the program's reset vector
    asm(" blx r0");                                                      // jump to the start address
        #else                                                            // cortex-M3/M4/M7 assembler code
    asm(" ldr sp, [r0,#0]");                                             // load the stack pointer value from the program's reset vector
    asm(" ldr pc, [r0,#4]");                                             // load the program counter value from the program's reset vector to cause operation to continue from there
        #endif
    #endif
}
#endif

#if !defined FREERTOS_NOT_COMPILED /*defined RUN_IN_FREE_RTOS || defined _WINDOWS*/ // to satisfy FreeRTOS callbacks - even when FreeRTOS not linked
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


// The initial stack pointer and PC value - this is usually linked at address 0x00000000 (or to application start location when working with a boot loader)
//
#if defined _COMPILE_IAR
__root const _RESET_VECTOR __vector_table @ ".intvec"                    // __root forces the function to be linked in IAR project
#elif defined _GNU
const _RESET_VECTOR __attribute__((section(".vectors"))) __vector_table
#elif defined _COMPILE_KEIL
__attribute__((section("VECT"))) const _RESET_VECTOR __vector_table
#else
const _RESET_VECTOR __vector_table
#endif
= {
#if defined INTERRUPT_VECTORS_IN_FLASH
    {
#endif
    (void *)(RAM_START_ADDRESS + (SIZE_OF_RAM - NON_INITIALISED_RAM_SIZE)), // stack pointer to top of RAM
    (void (*)(void))START_CODE,                                          // start address
#if defined NMI_IN_FLASH
    irq_NMI
#endif
#if defined INTERRUPT_VECTORS_IN_FLASH
    },
#endif
#if defined _APPLICATION_VALIDATION
    {0x87654321, 0xffffffff},                                            // signal that this application supports validation
    {0xffffffff, 0xffffffff},                                            // overwrite first location with 0x55aa33cc to validate the application
    #if defined _GNU && !defined _BM_BUILD
   {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff},
    {
        KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY,
        KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION,
        KINETIS_FLASH_CONFIGURATION_SECURITY,
        KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION,
        KINETIS_FLASH_CONFIGURATION_EEPROM_PROT,
        KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT
    }
    #endif
#elif defined INTERRUPT_VECTORS_IN_FLASH                                 // presently used only by the KE04, KEA8 and KL03 (vectors in flash to save space when little SRAM resources available)
    #if defined _MINIMUM_IRQ_INITIALISATION
    irq_default,
    irq_default,
    irq_default,
    irq_default,
    irq_default,
    #else
    irq_NMI,
    irq_hard_fault,
    irq_memory_man,
    irq_bus_fault,
    irq_usage_fault,
    #endif
    0,
    0,
    0,
    0,
    #if defined RUN_IN_FREE_RTOS
    vPortSVCHandler,                                                     // FreeRTOS's SCV handler
    irq_debug_monitor,
    0,
    xPortPendSVHandler,                                                  // FreeRTOS's PendSV handler
    #elif defined _MINIMUM_IRQ_INITIALISATION
    irq_default,
    irq_default,
    0,
    irq_default,
    #else
    irq_SVCall,
    irq_debug_monitor,
    0,
    irq_pend_sv,
    #endif
    _RealTimeInterrupt,                                                  // systick
    {                                                                    // processor specific interrupts
    irq_default,                                                         // 0
    irq_default,                                                         // 1
    irq_default,                                                         // 2
    irq_default,                                                         // 3
    irq_default,                                                         // 4
    irq_default,                                                         // 5
    irq_default,                                                         // 6
    #if defined SUPPORT_LOW_POWER && defined LLWU_AVAILABLE && defined SUPPORT_LLWU
    _wakeup_isr,                                                         // 7
    #else
    irq_default,                                                         // 7
    #endif
    #if defined I2C_INTERFACE
    _I2C_Interrupt_0,                                                    // 8
    #else
    irq_default,                                                         // 8
    #endif
    irq_default,                                                         // 9
    irq_default,                                                         // 10
    irq_default,                                                         // 11
    #if defined SERIAL_INTERFACE
        #if defined KINETIS_KL03
    _LPSCI0_Interrupt,                                                   // 12 LPUART 0
        #else
    _SCI0_Interrupt,                                                     // 12 UART 0
        #endif
    #else
        irq_default,                                                     // 12
    #endif
    irq_default,                                                         // 13
    irq_default,                                                         // 14
    #if defined SUPPORT_ADC
        _ADC_Interrupt_0,                                                // 15 ADC0
    #else
        irq_default,                                                     // 15
    #endif
    irq_default,                                                         // 16
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 0)
	_flexTimerInterrupt_0,                                               // 17
    #else
    irq_default,                                                         // 17
    #endif
    #if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 1 && !defined NO_FLEX_TIMER_2)
	_flexTimerInterrupt_1,                                               // 18
    #else
    irq_default,                                                         // 18
    #endif
	#if defined SUPPORT_TIMER && (FLEX_TIMERS_AVAILABLE > 2)
	_flexTimerInterrupt_2,                                               // 19
    #else
    irq_default,                                                         // 19
    #endif
    #if defined SUPPORT_RTC && defined KINETIS_KE
    _rtc_handler,                                                        // 20
    irq_default,                                                         // 21
    #elif defined SUPPORT_RTC && !defined KINETIS_KL02
    _rtc_alarm_handler,                                                  // 20
    _rtc_handler,                                                        // 21
    #else
    irq_default,                                                         // 20
    irq_default,                                                         // 21
    #endif
    #if (defined SUPPORT_PITS || defined USB_HOST_SUPPORT) && !defined KINETIS_WITHOUT_PIT
        #if defined KINETIS_KL
	_PIT_Interrupt,                                                      // 22
	irq_default,                                                         // 23
        #else
	_PIT0_Interrupt,                                                     // 22
	_PIT1_Interrupt,                                                     // 23
        #endif
    #else
    irq_default,                                                         // 22
	irq_default,                                                         // 23
    #endif
    #if defined USB_INTERFACE
    _usb_otg_isr,                                                        // 24
    #else
        #if defined KINETIS_KE && defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 0)
    _KBI0_isr,                                                           // 24
        #else
    irq_default,                                                         // 24
        #endif
    #endif
        #if defined KINETIS_KE && defined SUPPORT_KEYBOARD_INTERRUPTS && (KBIS_AVAILABLE > 1)
    _KBI1_isr,                                                           // 25
        #else
    irq_default,                                                         // 25
        #endif
    irq_default,                                                         // 26
    irq_default,                                                         // 27
    #if defined TICK_USES_LPTMR
    _RealTimeInterrupt,                                                  // 28
    #elif defined SUPPORT_LPTMR && !defined TICK_USES_LPTMR && !defined KINETIS_KE
    _LPTMR0_handler,                                                     // 28
    #else
    irq_default,                                                         // 28
    #endif
    #if defined KINETIS_KL03
    irq_default,                                                         // 29
        #if defined SUPPORT_PORT_INTERRUPTS && !defined NO_PORT_INTERRUPTS_PORTA
    _port_A_isr,                                                         // 30
        #else
    irq_default,                                                         // 30
        #endif
        #if defined SUPPORT_PORT_INTERRUPTS && !defined NO_PORT_INTERRUPTS_PORTB
    _port_B_isr,                                                         // 30
        #else
    irq_default,                                                         // 30
        #endif
    #endif
    }
#endif
};

#if !defined _BM_BUILD && !(defined _APPLICATION_VALIDATION && defined _GNU)
typedef struct _lut_sequence
{
    unsigned char seqNum; //!< Sequence Number, valid number: 1-16
    unsigned char seqId;  //!< Sequence Index, valid number: 0-15
    unsigned char reserved[2];
} flexspi_lut_seq_t;

typedef struct _FlexSPIConfig
{
    unsigned long tag;               //!< [0x000-0x003] Tag, fixed value 0x42464346UL
    unsigned long version;           //!< [0x004-0x007] Version,[31:24] -'V', [23:16] - Major, [15:8] - Minor, [7:0] - bugfix
    unsigned long reserved0;         //!< [0x008-0x00b] Reserved for future use
    unsigned char readSampleClkSrc;   //!< [0x00c-0x00c] Read Sample Clock Source, valid value: 0/1/3
    unsigned char csHoldTime;         //!< [0x00d-0x00d] CS hold time, default value: 3
    unsigned char csSetupTime;        //!< [0x00e-0x00e] CS setup time, default value: 3
    unsigned char columnAddressWidth; //!< [0x00f-0x00f] Column Address with, for HyperBus protocol, it is fixed to 3, For
                                //! Serial NAND, need to refer to datasheet
    unsigned char deviceModeCfgEnable; //!< [0x010-0x010] Device Mode Configure enable flag, 1 - Enable, 0 - Disable
    unsigned char deviceModeType; //!< [0x011-0x011] Specify the configuration command type:Quad Enable, DPI/QPI/OPI switch,
                            //! Generic configuration, etc.
    unsigned short waitTimeCfgCommands; //!< [0x012-0x013] Wait time for all configuration commands, unit: 100us, Used for
                                  //! DPI/QPI/OPI switch or reset command
    flexspi_lut_seq_t deviceModeSeq; //!< [0x014-0x017] Device mode sequence info, [7:0] - LUT sequence id, [15:8] - LUt
                                     //! sequence number, [31:16] Reserved
    unsigned long deviceModeArg;    //!< [0x018-0x01b] Argument/Parameter for device configuration
    unsigned char configCmdEnable;   //!< [0x01c-0x01c] Configure command Enable Flag, 1 - Enable, 0 - Disable
    unsigned char configModeType[3]; //!< [0x01d-0x01f] Configure Mode Type, similar as deviceModeTpe
    flexspi_lut_seq_t configCmdSeqs[3]; //!< [0x020-0x02b] Sequence info for Device Configuration command, similar as deviceModeSeq
    unsigned long reserved1;   //!< [0x02c-0x02f] Reserved for future use
    unsigned long configCmdArgs[3];     //!< [0x030-0x03b] Arguments/Parameters for device Configuration commands
    unsigned long reserved2;            //!< [0x03c-0x03f] Reserved for future use
    unsigned long controllerMiscOption; //!< [0x040-0x043] Controller Misc Options, see Misc feature bit definitions for more
                                   //! details
    unsigned char deviceType;    //!< [0x044-0x044] Device Type:  See Flash Type Definition for more details
    unsigned char sflashPadType; //!< [0x045-0x045] Serial Flash Pad Type: 1 - Single, 2 - Dual, 4 - Quad, 8 - Octal
    unsigned char serialClkFreq; //!< [0x046-0x046] Serial Flash Frequencey, device specific definitions, See System Boot
                           //! Chapter for more details
    unsigned char lutCustomSeqEnable; //!< [0x047-0x047] LUT customization Enable, it is required if the program/erase cannot
                                //! be done using 1 LUT sequence, currently, only applicable to HyperFLASH
    unsigned long reserved3[2];           //!< [0x048-0x04f] Reserved for future use
    unsigned long sflashA1Size;           //!< [0x050-0x053] Size of Flash connected to A1
    unsigned long sflashA2Size;           //!< [0x054-0x057] Size of Flash connected to A2
    unsigned long sflashB1Size;           //!< [0x058-0x05b] Size of Flash connected to B1
    unsigned long sflashB2Size;           //!< [0x05c-0x05f] Size of Flash connected to B2
    unsigned long csPadSettingOverride;   //!< [0x060-0x063] CS pad setting override value
    unsigned long sclkPadSettingOverride; //!< [0x064-0x067] SCK pad setting override value
    unsigned long dataPadSettingOverride; //!< [0x068-0x06b] data pad setting override value
    unsigned long dqsPadSettingOverride;  //!< [0x06c-0x06f] DQS pad setting override value
    unsigned long timeoutInMs;            //!< [0x070-0x073] Timeout threshold for read status command
    unsigned long commandInterval;        //!< [0x074-0x077] CS deselect interval between two commands
    unsigned short dataValidTime[2]; //!< [0x078-0x07b] CLK edge to data valid time for PORT A and PORT B, in terms of 0.1ns
    unsigned short busyOffset;       //!< [0x07c-0x07d] Busy offset, valid value: 0-31
    unsigned short busyBitPolarity;  //!< [0x07e-0x07f] Busy flag polarity, 0 - busy flag is 1 when flash device is busy, 1 -
                               //! busy flag is 0 when flash device is busy
    unsigned long lookupTable[64];           //!< [0x080-0x17f] Lookup table holds Flash command sequences
    flexspi_lut_seq_t lutCustomSeq[12]; //!< [0x180-0x1af] Customizable LUT Sequences
    unsigned long reserved4[4];              //!< [0x1b0-0x1bf] Reserved for future use
} flexspi_mem_config_t;

typedef struct stFLEXSPI_NOR_BOOT_CONFIGURATION
{
    flexspi_mem_config_t memConfig; //!< Common memory configuration info via FlexSPI
    unsigned long pageSize;              //!< Page size of Serial NOR
    unsigned long sectorSize;            //!< Sector size of Serial NOR
    unsigned char ipcmdSerialClkFreq;     //!< Clock frequency for IP command
    unsigned char isUniformBlockSize;     //!< Sector/Block size is the same
    unsigned char reserved0[2];           //!< Reserved for future use
    unsigned char serialNorType;          //!< Serial NOR Flash type: 0/1/2/3
    unsigned char needExitNoCmdMode;      //!< Need to exit NoCmd mode before other IP command
    unsigned char halfClkForNonReadCmd;   //!< Half the Serial Clock for non-read command: true/false
    unsigned char needRestoreNoCmdMode;   //!< Need to Restore NoCmd mode after IP commmand execution
    unsigned long blockSize;             //!< Block size
    unsigned long reserve2[11];          //!< Reserved for future use
} FLEXSPI_NOR_BOOT_CONFIGURATION;

#define FLEXSPI_CFG_BLK_TAG (0x42464346UL)     // ascii "FCFB" Big Endian
#define FLEXSPI_CFG_BLK_VERSION (0x56010400UL) // V1.4.0

#define kFlexSPIReadSampleClk_LoopbackInternally       0
#define kFlexSPIReadSampleClk_LoopbackFromDqsPad       1
#define kFlexSPIReadSampleClk_LoopbackFromSckPad       2
#define kFlexSPIReadSampleClk_ExternalInputFromDqsPad  3


#define kSerialFlash_1Pad          1
#define kSerialFlash_2Pads         2
#define kSerialFlash_4Pads         4
#define kSerialFlash_8Pads         8

#define kFlexSpiSerialClk_30MHz    1
#define kFlexSpiSerialClk_50MHz    2
#define kFlexSpiSerialClk_60MHz    3
#define kFlexSpiSerialClk_75MHz    4
#define kFlexSpiSerialClk_80MHz    5
#define kFlexSpiSerialClk_100MHz   6
#define kFlexSpiSerialClk_133MHz   7
#define kFlexSpiSerialClk_166MHz   8
#define kFlexSpiSerialClk_200MHz   9


#define FLEXSPI_LUT_OPERAND0_MASK                (0xFFU)
#define FLEXSPI_LUT_OPERAND0_SHIFT               (0U)
#define FLEXSPI_LUT_OPERAND0(x)                  (((unsigned long)(((unsigned long)(x)) << FLEXSPI_LUT_OPERAND0_SHIFT)) & FLEXSPI_LUT_OPERAND0_MASK)
#define FLEXSPI_LUT_NUM_PADS0_MASK               (0x300U)
#define FLEXSPI_LUT_NUM_PADS0_SHIFT              (8U)
#define FLEXSPI_LUT_NUM_PADS0(x)                 (((unsigned long)(((unsigned long)(x)) << FLEXSPI_LUT_NUM_PADS0_SHIFT)) & FLEXSPI_LUT_NUM_PADS0_MASK)
#define FLEXSPI_LUT_OPCODE0_MASK                 (0xFC00U)
#define FLEXSPI_LUT_OPCODE0_SHIFT                (10U)
#define FLEXSPI_LUT_OPCODE0(x)                   (((unsigned long)(((unsigned long)(x)) << FLEXSPI_LUT_OPCODE0_SHIFT)) & FLEXSPI_LUT_OPCODE0_MASK)
#define FLEXSPI_LUT_OPERAND1_MASK                (0xFF0000U)
#define FLEXSPI_LUT_OPERAND1_SHIFT               (16U)
#define FLEXSPI_LUT_OPERAND1(x)                  (((unsigned long)(((unsigned long)(x)) << FLEXSPI_LUT_OPERAND1_SHIFT)) & FLEXSPI_LUT_OPERAND1_MASK)
#define FLEXSPI_LUT_NUM_PADS1_MASK               (0x3000000U)
#define FLEXSPI_LUT_NUM_PADS1_SHIFT              (24U)
#define FLEXSPI_LUT_NUM_PADS1(x)                 (((unsigned long)(((unsigned long)(x)) << FLEXSPI_LUT_NUM_PADS1_SHIFT)) & FLEXSPI_LUT_NUM_PADS1_MASK)
#define FLEXSPI_LUT_OPCODE1_MASK                 (0xFC000000U)
#define FLEXSPI_LUT_OPCODE1_SHIFT                (26U)
#define FLEXSPI_LUT_OPCODE1(x)                   (((unsigned long)(((unsigned long)(x)) << FLEXSPI_LUT_OPCODE1_SHIFT)) & FLEXSPI_LUT_OPCODE1_MASK)

#define FLEXSPI_LUT_SEQ(cmd0, pad0, op0, cmd1, pad1, op1)  (FLEXSPI_LUT_OPERAND0(op0) | FLEXSPI_LUT_NUM_PADS0(pad0) | FLEXSPI_LUT_OPCODE0(cmd0) | FLEXSPI_LUT_OPERAND1(op1) | FLEXSPI_LUT_NUM_PADS1(pad1) | FLEXSPI_LUT_OPCODE1(cmd1))

#define CMD_SDR 0x01
#define CMD_DDR 0x21
#define RADDR_SDR 0x02
#define RADDR_DDR 0x22
#define CADDR_SDR 0x03
#define CADDR_DDR 0x23
#define MODE1_SDR 0x04
#define MODE1_DDR 0x24
#define MODE2_SDR 0x05
#define MODE2_DDR 0x25
#define MODE4_SDR 0x06
#define MODE4_DDR 0x26
#define MODE8_SDR 0x07
#define MODE8_DDR 0x27
#define WRITE_SDR 0x08
#define WRITE_DDR 0x28
#define READ_SDR 0x09
#define READ_DDR 0x29
#define LEARN_SDR 0x0A
#define LEARN_DDR 0x2A
#define DATSZ_SDR 0x0B
#define DATSZ_DDR 0x2B
#define DUMMY_SDR 0x0C
#define DUMMY_DDR 0x2C
#define DUMMY_RWDS_SDR 0x0D
#define DUMMY_RWDS_DDR 0x2D
#define JMP_ON_CS 0x1F
#define STOP 0

#define FLEXSPI_1PAD 0
#define FLEXSPI_2PAD 1
#define FLEXSPI_4PAD 2
#define FLEXSPI_8PAD 3


// FlexSPI NOR configuration block (located at start of the FlexSPI Flash)
//
#if defined _COMPILE_IAR
__root const FLEXSPI_NOR_BOOT_CONFIGURATION __boot_config @ ".boot_hdr.conf" // __root forces the function to be linked in IAR project
#elif defined _GNU
const FLEXSPI_NOR_BOOT_CONFIGURATION __attribute__((section(".boot_hdr.conf"))) __boot_config
#elif defined _COMPILE_KEIL
__attribute__((section(".boot_hdr.conf"))) const FLEXSPI_NOR_BOOT_CONFIGURATION __boot_config
#else
const FLEXSPI_NOR_BOOT_CONFIGURATION __boot_config
#endif
= {
    .memConfig =                                                         // for IS25LP064A on MIMXRT1020
        {
            .tag = FLEXSPI_CFG_BLK_TAG,
            .version = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
            .csHoldTime = 3u,
            .csSetupTime = 3u,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            //
            .sflashPadType = kSerialFlash_4Pads,
            .serialClkFreq = kFlexSpiSerialClk_100MHz,
            .sflashA1Size = (8u * 1024u * 1024u),
            .lookupTable =
                {
                    // Read LUTs
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
                    FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x04),
                },
        },
    .pageSize = 256u,
    .sectorSize = (4u * 1024u),
    .blockSize = (256u * 1024u),
    .isUniformBlockSize = 0,
};


typedef struct stBOOT_DATA
{
    unsigned long start;           // boot start location 
    unsigned long size;            // size 
    unsigned long plugin;          // plugin flag - 1 if downloaded application is plugin
    unsigned long placeholder;		// placehoder to make even 0x10 size
} BOOT_DATA;

#define FLEXSPI_FLASH_BASE          0x60000000
#define FLEXSPI_FLASH_SIZE  (0x800000U)
#define PLUGIN_FLAG           (unsigned long)0

#if defined _COMPILE_IAR
__root const BOOT_DATA __boot_data @ ".boot_hdr.boot_data" // __root forces the function to be linked in IAR project
#elif defined _GNU
const BOOT_DATA __attribute__((section(".boot_hdr.boot_data"))) __boot_data
#elif defined _COMPILE_KEIL
__attribute__((section(".boot_hdr.boot_data"))) const BOOT_DATA __boot_data
#else
const BOOT_DATA __boot_data
#endif
= {
    FLEXSPI_FLASH_BASE,            /* boot start location */
    FLEXSPI_FLASH_SIZE,         /* size */
    PLUGIN_FLAG,                /* Plugin flag*/
    0xFFFFFFFF  				  /* empty - extra data word */
};



#define IVT_MAJOR_VERSION           0x4
#define IVT_MAJOR_VERSION_SHIFT     0x4
#define IVT_MAJOR_VERSION_MASK      0xF
#define IVT_MINOR_VERSION           0x1
#define IVT_MINOR_VERSION_SHIFT     0x0
#define IVT_MINOR_VERSION_MASK      0xF

#define IVT_VERSION(major, minor)   ((((major) & IVT_MAJOR_VERSION_MASK) << IVT_MAJOR_VERSION_SHIFT) | (((minor) & IVT_MINOR_VERSION_MASK) << IVT_MINOR_VERSION_SHIFT))

#define IVT_TAG_HEADER        0xD1       /**< Image Vector Table */
#define IVT_SIZE              0x2000
#define IVT_PAR               IVT_VERSION(IVT_MAJOR_VERSION, IVT_MINOR_VERSION)
#define IVT_HEADER           (IVT_TAG_HEADER | (IVT_SIZE << 8) | (IVT_PAR << 24))

typedef struct stIMAGE_VECTOR_TABLE {
    /** @ref hdr with tag #HAB_TAG_IVT, length and HAB version fields
    *  (see @ref data)
    */
    unsigned long hdr;
    /** Absolute address of the first instruction to execute from the
    *  image
    */
    unsigned long entry;
    /** Reserved in this version of HAB: should be NULL. */
    unsigned long reserved1;
    /** Absolute address of the image DCD: may be NULL. */
    unsigned long dcd;
    /** Absolute address of the Boot Data: may be NULL, but not interpreted
    *  any further by HAB
    */
    unsigned long boot_data;
    /** Absolute address of the IVT.*/
    unsigned long self;
    /** Absolute address of the image CSF.*/
    unsigned long csf;
    /** Reserved in this version of HAB: should be zero. */
    unsigned long reserved2;
} IMAGE_VECTOR_TABLE;



static const unsigned char __dcd_data[1] = {0x00};

// Image Vector Table (located at 0x1000 offset in the FlexSPI Flash)
//
#if defined _COMPILE_IAR
__root const IMAGE_VECTOR_TABLE __image_vector_table @ ".boot_hdr.ivt" // __root forces the function to be linked in IAR project
#elif defined _GNU
const IMAGE_VECTOR_TABLE __attribute__((section(".boot_hdr.ivt"))) __image_vector_table
#elif defined _COMPILE_KEIL
__attribute__((section(".boot_hdr.ivt"))) const IMAGE_VECTOR_TABLE __image_vector_table
#else
const IMAGE_VECTOR_TABLE __image_vector_table
#endif
= {
    IVT_HEADER,                         // IVT Header
    (unsigned long)&__vector_table,     // Image Entry Function
    0,                           // Reserved = 0
    (unsigned long)&__dcd_data,           // Address where DCD information is stored
    (unsigned long)&__boot_data,        // Address where BOOT Data Structure is stored
    (unsigned long)&__image_vector_table, // Pointer to IVT Self (absolute address)
    (unsigned long)0,              // Address where CSF file is stored
    0                            // Reserved = 0
};
#endif
#endif

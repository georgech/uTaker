/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        SAM7X.c [ATMEL AT91SAM7X128 / 256 / 512]
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    22.02.2007 Add UART break transmission support                       {1}
    01.03.2007 Correct possible Ethernet error                           {2}
    22.03.2007 Use uEnable_Interrupt() and uDisable_Interrupt() together with iInterruptLevel to call actual enabling and disabling {3}
    24.03.2007 Support Rowley GCC compiler
    28.04.2007 Add missing function fnConfigSPIFileSystem()              {4}
    04.05.2007 Corrected invalid pause frame flag in EMAC_NCR            {5}
    11.07.2007 Ensure page buffer is empty when deleting flash sector    {6}
    18.09.2007 Ensure page buffer is empty when writing flash sector     {7}
    22.09.2007 Add SPI FLASH support                                     {8}
    23.09.2007 Adapt fnEraseFlashSector() and its use in fnDeleteParBlock() {9}
    28.09.2007 Add fnConfigureInterrupt() and interrupt support          {10}
    19.10.2007 Remove dummy write to flash buffer since it is cured by 7 {11}
    03.11.2007 Add general purpose timer interrupt support               {12}
    03.11.2007 Make global hardware timer configurable                   {13}
    14.11.2007 Update interrupt mask in simulator using port simulation routine {14}
    18.11.2007 Remove unnecessary zero padding in Ethernet transmission  {15}
    25.11.2007 Enable accurate distinction between simulated and Window's memory {16}
    15.12.2007 Inform simulator of IIC speed                             {17}
    16.12.2007 Allow SPI and FLASH files system to work together         {18}
    20.12.2007 fnInUserInterrupt()                                       {19}
    22.12.2007 Optimise rx Ethernet buffer copies by using long word type memcpy {20}
    23.12.2007 Add USART DMA support                                     {21}
    24.12.2007 Add iInterruptLevel use to UART irqs                      {22}
    06.01.2008 Add DISABLE_WATCHDOG() macro                              {23}
    07.01.2008 Don't reset simulator flag iFetchingInternalMemory (caller does it) {24}
    01.03.2008 Multiple-chip SPI FLASH chip select control size increased in unsigned long {25}
    01.03.2008 Add macros to allow changing SPI mode when entering and leaving SPI FLASH command routine - moved to spi_flash_sam7x_atmel.h {26}
    01.03.2008 Set CS0 control on SPI FLASH SPI config                   {27}
    18.03.2008 Remove chip select parameter from fnSimAT45DBXXX() - moved to spi_flash_sam7x_atmel.h
    25.03.2008 Optimise use of SPI_FLASH_Danger[iChipSelect] - moved to spi_flash_sam7x_atmel.h {28}
    23.04.2008 Mask PHY_IDENTIFIER when checking PHY                     {29}
    11.05.2008 Add fnProtectFlash() routine                              {30}
    09.06.2008 Add optional specific uNetwork broadcast address          {31}
    14.06.2008 Add EMAC workarounds for version A devices (PART_A_WORKAROUNDS) {32}
    02.07.2008 Rework port interrupt setup                               {33}
    27.08.2008 Modify fnTxIIC Channel parameter to QUEUE_HANDLE          {34}
    05.10.2008 Add DBGU UART support                                     {35}
    06.10.2008 Add USB support                                           {36}
    06.10.2008 Introduce POWER_UP and POWER_DOWN macros                  {37}
    08.10.2008 Reset PIT status when reading register (simulation)       {38}
    08.10.2008 Flexible PLL setting added - controlled from app_hw_sam7x.h {39}
    21.10.2008 SPI FLASH channel configurable                            {40}
    21.10.2008 Reorganise SPI Flash support to use include files         {41}
    21.10.2008 Add more flexible parameter block configuration using PARAMETER_BLOCK_START {42}
    22.10.2008 Add ST and SST SPI FLASH support                          {43}
    01.12.2008 Add Keil support                                          {44}
    01.12.2008 Enter FIQ port interrupt before configuring               {45}
    01.12.2008 Add IAR5 support                                          {46}
    22.12.2008 Add ADC support                                           {47}
    27.12.2008 Reset ENDTX in status register when simulating DMA transmission {48}
    06.01.2009 Correct conditional define for Rowley                     {49}
    06.01.2009 Remove prototype to start of file                         {50}
    14.01.2009 Add IRQ1 support                                          {51}
    14.01.2009 Configure port interrupts automatically as inputs         {52}
    25.01.2009 Correct 32k and 64k SST SPI FLASH delete decision         {53}
    01.02.2009 Improve TICK accuracy using float                         {54}
    03.02.2009 Add conditional define SUPPORT_TIMER                      {55}
    04.02.2009 Add us support to timer interrupt                         {56}
    04.02.2009 Remove unused variable                                    {57}
    12.02.2009 Add IP loop back support for simulations                  {58}
    28.02.2009 fnControlLine() and fnControlLineInterrupt() operating mode parameter adapted to UART_MODE_CONFIG {59}
    28.02.2009 Reworked RTS/CTS control                                  {60}
    04.03.2009 Change use of random number seed                          {61}
    24.03.2009 fnTxByteDMA() returns the transmission length achieved    {62}
    17.04.2009 Add fnRemainingDMA_tx() and fnAbortTxDMA()                {63}
    26.05.2009 Correct PHY maintenance interface polling bit             {64}
    10.06.2009 Rename usConfig to Config in UART configuration           {65}
    17.08.2009 Add SSC support                                           {66}
    07.09.2009 Add hardware timer mode with period and fractional support{67}
    11.09.2009 Add fnGetMultiDropByte()                                  {68}
    15.10.2009 Add external UART support                                 {69}
    28.10.2009 Add IRQ_PULLUP_ENABLE and IRQ_GLITCH_ENABLE options for port interrupts {70}
    01.12.2009 Add managed memory support                                {71}
    10.01.2010 Add multicast Ethernet configuration and simulation acceptance {72}
    12.01.2010 Add PWM controller support                                {73}
    03.02.2010 Correct PLL configuration to respect new known restrictions{74}
    13.02.2010 Add SAM7Sxxx                                              {75}
    16.02.2010 Allow Ethernet simulation without PHY interrupt           {76}
    03.03.2010 Add external timer source mode and allow freerun timer without interrupt {77}
    25.03.2010 Handle endpoint stalls on IN endpoint                     {78}
    27.03.2010 Add fnUnhaltEndpoint()                                    {79}
    30.03.2010 Support FLASH controller 1 in devices with more that 256k FLASH {80}
    31.03.2010 Move reset of USB address to after generic reset call to allow RESET event to be sent {81}
    10.05.2010 Adapt MANAGED_FILES support to include internal FLASH     {82}
    12.10.2010 Add SAM7SExxx                                             {83}
    04.02.2011 Correct USB transmission simulation with swap buffer operation {84}
    08.02.2011 Add USB transceiver and pull-up initialisation for SAM7SE {85}
    09.02.2011 Add MONITOR_PERFORMANCE support                           {86}
    13.02.2011 Reset USB FIFOs after feature clear                       {87}
    14.02.2011 Remove random seed from stack to a fixed location         {88}
    14.02.2011 Protect USB reset, suspend and resume events              {89}
    10.06.2011 Add multi-drop mode support (9-bit mode) for USARTs       {90}
    27.10.2011 Add SUPPORT_RTC and fnGetLocalFileTime()                  {91}
    27.10.2011 Add ETHERNET_RELEASE_AFTER_EVERY_FRAME support            {92}
    27.10.2011 Modify the debug UART interrupt to share TICK interrupt and priority {93}
    28.10.2011 Add revised Flash handing as defined by STORAGE_REV_1     {94}
    18.11.2011 fnEraseFlashSector() returns negative value on failure and the sector size of media when called with zero {95}
    18.11.2011 Allow invalid storage media reads to memory map the access when not simulating (this allows handling retrieval from SRAM without having to define it as a storage area) {96}
    10.12.2011 Correct TIMER_DISABLE to correctly stop hardware timer    {97}
    04.01.2012 Add return value to fnConfigEthernet()                    {98}
    18.01.2013 Add RTC_INITIALISATION for compatibility                  {99}
    26.01.2013 Add SAM7L                                                 {100}
    23.11.2013 Modify some USB details to avoid possibility of errors on SETUP receptions with following data {101}
    16.06.2015 Udapted parameters to fnSimulateEthTx()                   {102}
    16.06.2015 Add USB error counters and clear errors in the USB interrupt routine {103}
    16.06.2015 Add fnModifyMulticastFilter() for IGMP use                {104}
    16.06.2015 Add hardware operating details for simulator display      {105}
    23.11.2015 Add volatile to ensure that interrupt flags are not optimised away when code in-lining takes place {106}

*/

#if defined _HW_SAM7X


#define RESET_USB_EP_INT_LATER                                           // test resetting endpoint interrupt after frame handling has completed and not before (no evidence of differences but this method makes more sense when analysed logically)

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS
    #include "config.h"
    #include "../../WinSim/WinSim.h"
    #define INITHW  extern
    void AT91F_LowLevelInit( void);
    extern void fnOpenDefaultHostAdapter(void);
    extern void fnUpdateOperatingDetails(void);                          // {105}
    extern unsigned char fnMapPortBit(unsigned long ulRealBit);
    #define _SIM_PORT_CHANGE   fnSimPorts(-1);                           // make sure simulator knows of change
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define _SIM_PORT_CHANGE
    #include "config.h"
#endif

#if defined SERIAL_INTERFACE && (NUMBER_EXTERNAL_SERIAL > 0)             // {69}
    #if defined EXT_UART_SC16IS7XX
        #include "spi_sc16IS7xx.h"                                       // include driver
    #endif
#endif
/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define __interrupt

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define SPI_FLASH_ENABLED 
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

typedef struct stUSB_ERRORS                                              // {103}
{
    unsigned long ulUSB_errors_CRC5;
    unsigned long ulUSB_errors_CRC16;
} USB_ERRORS;

/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined SUPPORT_PORT_INTERRUPTS && defined INTERRUPT_TASK_PHY        // {50}
    static void fnPhyInterrupt(void);
#endif
#if defined SUPPORT_RTC                                                  // {91}
    static void _rtc_handler(void);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


#if defined ETH_INTERFACE && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME // {92}
    static const unsigned char EMAC_RX_int_message[HEADER_LENGTH] = {0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT};   // define fixed interrupt event
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined RANDOM_NUMBER_GENERATOR
    static unsigned short *ptrSeed;
#endif
#if defined ETH_INTERFACE
    static SAM7X_RX_BD *ptrRxBd, *ptrFirstRxBd, *ptrTxBd, *ptrFirstTxBd; // we set two pointers to the rx Ethernet buffers. The first is fixed but the second depends on buffer size
    static unsigned long ulEthernetInput[LAN_BUFFER_SIZE/sizeof(unsigned long)]; // a buffer adequate to hold a full Ethernet frame while being processed as linear buffer {20} - aligned on long word boundary
    #if defined USE_IGMP && !defined NO_INTERNAL_ETHERNET                // {104}
        #if defined USE_IPV6
            #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 2)
        #else
            #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 1)
        #endif
        static unsigned char ucGroupHashes[MAX_MULTICAST_FILTERS] = {0};
    #endif
#endif
#if defined _WINDOWS
    static unsigned short usPhyMode = 0;
#endif
#if defined USB_INTERFACE                                                // {36}
    static USB_END_POINT *usb_endpoints;
    static USB_ERRORS USB_errors = {0};                                  // {103}
#endif

#if defined STORAGE_REV_1 && !defined ONLY_INTERNAL_FLASH_STORAGE        // {94}
    static const STORAGE_AREA_ENTRY default_flash = {
        0,                                                               // end of list
        (unsigned char *)(FLASH_START_ADDRESS),                          // start address of internal flash
        (unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH - 1)),    // end of internal flash
        _STORAGE_INTERNAL_FLASH,                                         // type
        0                                                                // not multiple devices
    };

    STORAGE_AREA_ENTRY *UserStorageListPtr = (STORAGE_AREA_ENTRY *)&default_flash; // default entry
#endif

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD) // {8}
    static void fnConfigSPIFileSystem(unsigned char ucSpeed);
    #if defined SPI_FLASH_ENABLED                                        // {18}
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25            // {43}
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined {41}
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_sam7x_atmel.h"                           // {41}
            #include "spi_flash_sam7x_stmicro.h"                         // {43}
            #include "spi_flash_sam7x_sst25.h"                           // {43}
        #undef _SPI_DEFINES
    #endif
#endif

static void ( *irq_handler )( void ) = 0;                                // {10}
static void ( *irq_handler1)( void ) = 0;                                // {51}
static void ( *fiq_handler )( void ) = 0;                                // {10}

#if defined SUPPORT_PORT_INTERRUPTS
    static void ( *gpio_handlers_a[PORT_WIDTH] )( void ) = {0};          // {10} a handler for each possible port a line
    static unsigned char gpio_config_a[PORT_WIDTH];                      // configuration of each handler
    #ifndef _HW_SAM7S                                                    // {75}
    static void ( *gpio_handlers_b[PORT_WIDTH] )( void ) = {0};          // a handler for each possible port b line
    static unsigned char gpio_config_b[PORT_WIDTH];                      // configuration of each handler
    #endif
#endif

#if defined SPI_FLASH_ENABLED                                            // {8}{18}
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];
    #else
    static int SPI_FLASH_Danger[1] = {0};
    static unsigned char ucSPI_FLASH_Type[1];
    #endif

    #if defined SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {           // {25}
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
        #if defined STORAGE_REV_1                                        // {94}
            #define _EXTENDED_CS  ptrAccessDetails->ucDeviceNumber,
        #else
            #define _EXTENDED_CS  iChipSelect,
        #endif
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif
#ifndef CS0_LINE                                                         // for compatibility when signals are not defined by user
    #define CS0_LINE    CS_OUTPUT
#endif

#if defined SERIAL_INTERFACE && defined UART_EXTENDED_MODE               // {90}
    #if defined SERIAL_MULTIDROP_TX
        static unsigned char ulNextSent[NUMBER_SERIAL] = {0};            // next extended data to be sent
    #endif
    #if defined SERIAL_MULTIDROP_RX
        static unsigned long ulLastReceived[NUMBER_SERIAL];              // backup of last read reception data
    #endif
#endif

#if defined SUPPORT_RTC                                                  // {91}
    static unsigned long ulReferenceTime = 0;                            // the reference time when the RTT was started
    static int iRTT_interrupt_blocking = 0;                              // interrupt blocking counter required to synchronise slow and fast clocks in processor
#endif

static int iInterruptLevel = 0;

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */
#if defined ETH_INTERFACE
    extern ETHERNETQue *eth_tx_control;                                  // Ethernet structure pointers for Ethernet Interrupt handling
    extern ETHERNETQue *eth_rx_control;
#endif

#if defined IIC_INTERFACE
    extern IICQue *IIC_rx_control[NUMBER_IIC];
    extern IICQue *IIC_tx_control[NUMBER_IIC];
#endif

#if (defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES) && defined _WINDOWS
    extern int iFetchingInternalMemory = _ACCESS_NOT_IN_CODE;
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#ifndef _WINDOWS
    static void fnInitHW(void);
#endif
#if defined GLOBAL_HARDWARE_TIMER
    static __interrupt void hwtimer_interrupt(void);
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
    #if defined COMPILE_IAR5                                             // {46}
        #pragma segment=".data"
        #pragma segment=".bss"
        #if defined COMPILE_IAR_DEBUG
            #pragma segment=".rodata"                                    // constants
            #pragma segment=".text"                                      // program code
            #pragma segment="Region$$Table"                              // initialisation table
        #endif
        static unsigned char *ptrTopOfUsedMemory = 0;
        #define HEAP_START_ADDRESS ptrTopOfUsedMemory                    // IAR5 compiler - last location of static variables
    #else
        #pragma segment="DATA16_Z"
        #define HEAP_START_ADDRESS __segment_end("DATA16_Z")             // IAR compiler - last location of static variables
    #endif
#elif defined COMPILE_KEIL                                               // {44}
    #define __root
    extern unsigned char Stack_Mem;
    #define HEAP_START_ADDRESS  &Stack_Mem                               // Keil - start of stack
#else
    #if defined _GNU || defined ROWLEY                                   // {49}
        #define __root
    #endif
    extern void __irq_en(void);                                          // enable interrupt in assembler code
    extern void __irq_dis(void);                                         // disable interrupt in assembler code
    extern unsigned char __heap_end__;
    #define HEAP_START_ADDRESS &__heap_end__                             // GNU last location of static variables
#endif


// Main entry for the target code
//
#if defined COMPILE_IAR
    #pragma location="APPCODE"
#endif

__root int main(void)                                                    // __root forces the function to be linked in IAR project
{
#if defined MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart = HEAP_START_ADDRESS;
#endif
#if defined _HW_SAM7SE
    int i;
    unsigned char *ptrRAM = (unsigned char *)0x200000;
    unsigned char *ptrSDRAM = (unsigned char *)0x20000000;
#endif
#if defined RANDOM_NUMBER_GENERATOR
  //unsigned short usRandomSeed;                                         // {88} we put an uninitialised variable on to the stack for use as a random seed
    ptrSeed = RANDOM_SEED_LOCATION; // &usRandomSeed; 
#endif

#if defined (_BOOT_PROGRAM) && defined (COMPILE_IAR)
    __segment_init();                                                    // initialisation of local variables
#elif defined COMPILE_IAR5                                               // {46}
    if (__sfe(".bss") > __sfe(".data")) {                                // set last used SRAM address
        ptrTopOfUsedMemory = __sfe(".bss");
    }
    else {
        ptrTopOfUsedMemory = __sfe(".data");
    }
    #if defined COMPILE_IAR_DEBUG
    if (__sfe(".rodata") > ptrTopOfUsedMemory) {
        ptrTopOfUsedMemory = __sfe(".rodata");
    }
    if (__sfe(".text") > ptrTopOfUsedMemory) {
        ptrTopOfUsedMemory = __sfe(".text");
    }
    if (__sfe("Region$$Table") > ptrTopOfUsedMemory) {
        ptrTopOfUsedMemory = __sfe("Region$$Table");
    }
    #endif
#endif
#if defined _HW_SAM7SE
    for (i = 0; i < 512; i++) {
        *ptrRAM++ = *ptrSDRAM++;
    }
    i = ~(*(volatile int *)0x14);                                        // read the unused vector and invert its value
    *(volatile int *)0x14 = i;                                           // try to modify the location to see whether it is RAM or FLASH
    if (*(volatile int *)0x14 != i) {                                    // if the value hasn't changed it is in FLASH
        MC_RCR = RCB;                                                    // remap internal SRAM to appear at 0x00000000 and handle interrupts
    }
    else {
        *(volatile int *)0x14 = ~i;                                      // return the orignial value since it is in SRAM
    }
#endif

    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)
#if defined MULTISTART
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


// Function used to enter interrupts
//
static void fnEnterInterrupt(unsigned long ulInterruptSource, unsigned char ucPriority, void (*InterruptFunc)(void))
{
    volatile unsigned long *ptrIntReg = ADD_AIC_SMR0;                    // {106}
    volatile unsigned long ulMask = 0x01;                                // {106}

    while ((ulInterruptSource & ulMask) == 0) {
        ulMask <<= 1;
        ptrIntReg++;
    }

    *ptrIntReg = ucPriority;                                             // set the priority (and level sensitivity)
    ptrIntReg += (ADD_AIC_SVR0 - ADD_AIC_SMR0);
    *ptrIntReg = (unsigned long)InterruptFunc;                           // enter the handling interrupt routine in the vector table
    AIC_IDCR = ulInterruptSource;                                        // disable the interrupt
#if defined _WINDOWS
    fnSimPorts(-1);                                                      // {14}
#endif
    AIC_ICCR = ulInterruptSource;                                        // clear the interrupt
    AIC_IECR = ulInterruptSource;                                        // enable interrupt to core
#if defined _WINDOWS
    fnSimPorts(-1);                                                      // {14}
#endif
}


#if defined RANDOM_NUMBER_GENERATOR
// How the random number seed is set depends on the hardware possibilities available.
//
extern void fnInitialiseRND(unsigned short *usSeedValue)                 // {61}
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
#if defined GLOBAL_HARDWARE_TIMER
    volatile unsigned long dummy;
#endif
#if defined _WINDOWS
    unsigned long ulPortPullups[] = {
        PORTA_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_sam7x.h
        PORTB_DEFAULT_INPUT,
    #if defined _HW_SAM7SE || defined _HW_SAM7L                          // {83}{100}
        PORTC_DEFAULT_INPUT,
    #endif
    #if defined SUPPORT_ADC                                              // {47}
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
    #if defined RANDOM_NUMBER_GENERATOR
    static unsigned short usRandomSeed = 0;                              // {88} note that the target works differently
    ptrSeed = &usRandomSeed;
    #endif
    fnInitialiseDevice((void *)ulPortPullups);
    AT91F_LowLevelInit();
#endif

    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
    INIT_WATCHDOG_DISABLE();                                             // initialise ready for checking of watchdog disabled
    if (!WATCHDOG_DISABLE()) {
        WDT_MR = CONFIGURE_WATCHDOG;                                     // allow user configuration of internal watch dog timer
    }
    else {
        DISABLE_WATCHDOG();                                              // {23} disable the watchdog
    }

#if defined _WINDOWS
    fnSimPorts(-1);                                                      // ensure port states are recognised
#endif

    fnUserHWInit();                                                      // allow the user to initialise hardware specific things

#if defined _WINDOWS
    fnSimPorts(-1);                                                      // ensure port states are recognised
#endif


#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    #if defined SPI_FLASH_ENABLED                                        // {18}
    fnConfigSPIFileSystem(MAX_SPI);                                      // configure SPI interface for maximum speed {8}

    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
        #include "spi_flash_sam7x_atmel.h"                               // {41} - check the chip(s) availablility
        #include "spi_flash_sam7x_stmicro.h"                             // {43}
        #include "spi_flash_sam7x_sst25.h"                               // {43}
    #undef _CHECK_SPI_CHIPS
    #else
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
    #endif
#endif

#if defined GLOBAL_HARDWARE_TIMER                                        // initialise the timer for use with global hardware timers {3}
    POWER_UP(HW_TIMER_TC);                                               // {37} enable clocks to timer in PMC
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
    WDT_CR = (WATCHDOG_KEY | WDRSTT);                                    // re-trigger the watchdog
    TOGGLE_WATCHDOG_LED();                                               // flash an LED for visualisation
}

// Routine to disable interrupts during critical region
//
extern void uDisable_Interrupt(void)                                     // {3}
{
#ifndef _WINDOWS
    #if defined (COMPILE_IAR)
    __disable_interrupt();                                               // IAR intrinsic routine
    #elif defined COMPILE_KEIL                                           // {44}
    __disable_irq();                                                     // Keil intrinsic routine
    #elif defined (_GNU)
    __irq_dis();
    #endif
#endif
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
extern void uEnable_Interrupt(void)                                      // {3}
{
#if defined _WINDOWS
    if (!iInterruptLevel) {
        _EXCEPTION("check interrupt disabling/enabling symmetry");       // basic error - cause simulator exception
        // A routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
    }
#endif
    if (!(--iInterruptLevel)) {                                          // only when no more interrupt nesting,
    #if defined (COMPILE_IAR)
        __enable_interrupt();                                            // IAR intrinsic routine
    #elif defined COMPILE_KEIL                                           // {44}
        __enable_irq();                                                  // Keil intrinsic routine
    #elif defined (_GNU)
        __irq_en();
    #endif
    }
}

// User handling own interrupts can signal it here to avoid driver routines opening interrupts too early
//
extern void fnInUserInterrupt(int iEntering)                             // {19}
{
    if (iEntering) {                                                     // entering user controlled interrupt routine
        iInterruptLevel = 1;                                             // mark that interrupts are disabled
    }
    else {
        iInterruptLevel = 0;                                             // mark interrupt exiting
    }
}





/**************************** TICK interrupt ******************************************/
#if defined COMPILE_IAR && !defined COMPILE_IAR5                         // {46} disable optimising away the reading of the timer (to reset interrupt)
   #pragma optimize=2
#endif


#if defined DBGU_UART || defined SUPPORT_RTC                             // {91}{93} the SYSINT is shared by multiple sources
static __interrupt void _RealTimeInterrupt(void)
{
    iInterruptLevel = 1;
    while (AIC_IPR & SYSIRQ) {                                           // while interrupts present from any of the sources
    #if defined DBGU_UART
        #if defined SERIAL_SUPPORT_DMA
        if ((DBGU_IMR & RXRDY) && (DBGU_SR & RXRDY))
        #else
        if (DBGU_SR & RXRDY)                                             // DBGU receive interrupt
        #endif
        {
            fnSciRxByte((unsigned char)DBGU_RHR, 2);                     // receive data interrupt - read the byte (clearing the interrupt)
        #if defined _WINDOWS
            DBGU_SR &= ~(RXRDY);                                         // simulate reset of interrupt flag
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
        if ((DBGU_IMR & ENDRX) && (DBGU_SR & ENDRX)) {
            DBGU_IDR = ENDRX;                                            // disable to avoid further
            #if defined _WINDOWS
            DBGU_IMR &= ~ENDRX;
            DBGU_SR &= ~ENDRX;
            #endif
            fnSciRxByte(0, 2);
        }
        if ((DBGU_IMR & ENDTX) && (DBGU_SR & ENDTX)) {
            DBGU_IDR = ENDTX;                                            // disable to avoid further
            #if defined _WINDOWS
            DBGU_IMR &= ~ENDTX;
            #endif
            fnSciTxByte(2);                                              // transmit block complete interrupt - write next block
        }
        #endif
        if ((DBGU_IMR & TXRDY) && (DBGU_SR & TXRDY)) {
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
        }
    #endif
        if (PIT_SR & PITS) {                                             // check TICK interrupt as interrupt source
            fnRtmkSystemTick();                                          // operating system tick
            (void)PIT_PIVR;                                             // reset interrupt by reading the timer value
    #if defined _WINDOWS
            PIT_SR &= ~PITS;
    #endif
    #if defined SUPPORT_RTC
            if (iRTT_interrupt_blocking != 0) {
                if (--iRTT_interrupt_blocking == 0) {                    // recognise the end of the 2 x TICK RTT interrupt blocking period
        #if defined _WINDOWS
                    RTT_SR &= ~RTT_SR_RTTINC;                            // simulate delayed resetting of flag
        #endif
                    RTT_MR = (0x00008000 | RTT_MR_RTTINCIEN);            // re-enable RTT second interrupt
                }
            }
    #endif
        }
    #if defined SUPPORT_RTC
        if ((RTT_SR & RTT_SR_RTTINC) && (RTT_MR & RTT_MR_RTTINCIEN)) {   // second increment - reading the status register clears the flag if it was set
            // Note that the RTT is clocked from the slow clock and it takes 2 x slow clocks before the status flag is reset. Thsi can result in the interrupt being called a multiple
            // number of times. For this reason the interrupt is masked here and re-enabled by the TICK interrupt after 2 further ticks (it is assumed that two TICK periods will be less that 1s)
            //
            RTT_MR = 0x00008000;                                         // allow RTT to contnue running with 1s period but disable interrupt
            iRTT_interrupt_blocking = 2;                                 // block further interrupts for 2 TICKs to ensure that it remains disabled long enough to be cleraed by the slow clock
            _rtc_handler();                                              // perform 1s RTC processing
        }
    #endif
    #if defined _WINDOWS
        AIC_IPR &= ~SYSIRQ;                                              // simulate reset of interrupt flag
    #endif
    }
    iInterruptLevel = 0;
}
#else
static __interrupt void _RealTimeInterrupt(void)
{
    volatile unsigned long ulTimerValue;
    fnRtmkSystemTick();                                                  // operating system tick
    ulTimerValue = PIT_PIVR;                                             // reset interrupt by reading the timer value
    #if defined _WINDOWS                                                 // {38}
    PIT_SR &= ~PITS;
    #endif
}
#endif

// Routine to initialise the Real Time Tick interrupt
//
#define REQUIRED_MS (float)((float)1000/(float)TICK_RESOLUTION)          // {54} the TICK frequency we require in kHz
#define TICK_DIVIDE (float)(((float)(MASTER_CLOCK/16) + REQUIRED_MS)/REQUIRED_MS) // {54} the divide ratio required
#define REGISTER_VALUE (((unsigned long)TICK_DIVIDE - 1) & PIV_MASK)


#if defined COMPILE_IAR && !defined COMPILE_IAR5                         // {46} disable optimising away the reading of the timer (to reset interrupt)
   #pragma optimize=2
#endif
extern void fnStartTick(void)
{
    PIT_MR = 0;                                                          // ensure timer stopped
    (void)PIT_PIVR;                                                      // reset any waiting interrupt (useful after new start without a real reset)
    AIC_EOICR = 0;                                                       // reset interrupt in controller (seems to be necessary)

    fnEnterInterrupt(SYSIRQ, PRIORITY_TICK_TIMER, _RealTimeInterrupt);

    PIT_MR = (REGISTER_VALUE | PITEN | PITIEN);                          // set periodic timer, enable timer and its interrupt

    #if defined _GNU || defined COMPILE_KEIL                             // {80}
        #if MASTER_CLOCK > 30000000
    MC_FMR = (ONE_WAIT_STATE | FLASH_TIME_NORMAL | NEBP);                // set one wait in FLASH since we will be working faster than 30MHz
            #if SIZE_OF_FLASH > (256 * 1024)
    MC_FMR1 = (ONE_WAIT_STATE | FLASH_TIME_NORMAL | NEBP);               // configure in second FLASH controller in larger parts
            #endif
        #else
    MC_FMR = (FLASH_TIME_NORMAL | NEBP);                                 // no wait states in FLASH when not working higher than 30MHz
            #if SIZE_OF_FLASH > (256 * 1024)
    MC_FMR1 = (FLASH_TIME_NORMAL | NEBP);                                // configure in second FLASH controller in larger parts
            #endif
        #endif
        #if defined _GNU
    __irq_en();                                                          // enable interrupts from this point
        #else
    __enable_irq();                                                      // Keil intrinsic routine
        #endif
    #endif
    #if defined MONITOR_PERFORMANCE                                      // {86} configure a timer that will be used to measure the duration of task operation
    INITIALISE_MONITOR_TIMER();
    #endif
}

extern unsigned long fnGetRTT_delta(void)
{
    static unsigned long ulLastValue = 0;
    unsigned long ulDifference = (RTT_VR - ulLastValue);
    ulLastValue = RTT_VR;
    if (ulDifference > 0xf0000000) {                                     // after reset a value of 0xffffffff was sometimes encountered so filter this
        return 1;
    }
    return ulDifference;
}

#if defined ETH_INTERFACE
/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */

// A long word version of memcpy since the LAN buffer copy is always on long word boundaries {20}
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
        else if (iInFrame == 0) {                                        // we are still searching for a start frame
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
                usFrameLength = 0;                                       // reject frame {2}
                break;
            }
            uMemLongcpy(&ulEthernetInput[usLength/sizeof(unsigned long)], (unsigned long *)ptrInputBuffer, ((usFrameLength - usLength) + 3)/sizeof(unsigned long)); // copy the last bytes from last segment {20}
            break;
        }
        else {                                                           // an individual block in frame - not last one
            if ((usLength + SAM7X_FIXED_BUFFER_SIZE) <= LAN_BUFFER_SIZE) { // {2} only copy if there is adequate space in the buffer
                uMemLongcpy(&ulEthernetInput[usLength/sizeof(unsigned long)], (unsigned long *)ptrInputBuffer, (SAM7X_FIXED_BUFFER_SIZE/sizeof(unsigned long)));  // copy block
            }                                                            // if no copy is performed, the frame will be rejected when handling the last buffer
            usLength += SAM7X_FIXED_BUFFER_SIZE;
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
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ulEthernetInput;     // return pointer to the fixed linear input buffer {20}
        return 0;                                                        // channel received from 0 (we use only 0 since we have copied the received from to a local buffer)
    }
#if defined USE_IP_STATS && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME  // {92}
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
  //volatile int iWait = 0;

    EMAC_MAN = (READ_FROM_PHY | ulRegAdr);                               // command read from given address

#if defined _WINDOWS
    EMAC_NSR = (MDIO | PHY_LOGIC_RUNNING);                               // {64}    EMAC_NSR = MDIO;
    EMAC_MAN &= ~0x0000ffff;
    if (KS8721_PHY_IDENTIFIER_1 == ulRegAdr) {                           // simulate reads from identifier register
        EMAC_MAN |= (PHY_IDENTIFIER >> 16);
    }
    else if (KS8721_PHY_IDENTIFIER_2 == ulRegAdr) {
        EMAC_MAN |= (PHY_IDENTIFIER & 0x0000ffff);
    }
    else if (KS8721_INTERRUPT_CONTROL_STATUS_REGISTER == ulRegAdr) {
    #if defined PHY_INTERRUPT                                            // {76}
        unsigned char ucBit = (PORT_WIDTH - 1);
        unsigned long ulBit = PHY_INTERRUPT;
        while (ulBit > 1) {
            ulBit >>= 1;
            ucBit--;
        }
        EMAC_MAN |= PHY_LINK_UP_INT;
        fnSimulateInputChange(1, ucBit, SET_INPUT);
    #endif
    }
    else if (KS8721_BASIC_STATUS_REGISTER == ulRegAdr) {
        EMAC_MAN |= PHY_100MEG_FULL_DUPLEX;
    }
#endif
    while (!(EMAC_NSR & PHY_LOGIC_RUNNING)) {}                           // {64} wait until the transfer has completed
  //while (++iWait < 400) {};                                            // wait long enough for read to be executed
  //while (!(EMAC_NSR & MDIO)) { }                                       // this always terminates immediately so what's wrong with the bit??

    return (unsigned short)EMAC_MAN;                                     // return the value read
}

// Routine to write a word to the PHY
//
static void fnWriteMII(unsigned long ulRegAdr, unsigned short usData)
{
  //volatile int iWait = 0;

    EMAC_MAN = (WRITE_TO_PHY | ulRegAdr | usData);                       // command read from given address

#if defined _WINDOWS
    EMAC_NSR = (MDIO | PHY_LOGIC_RUNNING);                               // {64}
    if (KS8721_BASIC_CONTROL_REGISTER == ulRegAdr) {
        usPhyMode = usData;
    }
#endif
    while (!(EMAC_NSR & PHY_LOGIC_RUNNING)) {}                           // {64} wait until the transfer has completed
  //while (++iWait < 400) {};                                            // wait long enough for read to be executed
  //while (!(EMAC_NSR & MDIO)) { }                                       // this always terminates immediately so what's wrong with the bit??
}


// EMAC interrupt displatcher
//
__interrupt void EMAC_Interrupt(void)
{
    volatile unsigned long ulInterrupts;
#if defined PART_A_WORKAROUNDS                                           // {32} interrupts in EMAC_ISR can be lost when reading so use status registers for decisions
    ulInterrupts = EMAC_ISR;                                             // read the interrupt status register, which clears all interrupts (possibility of loss of status bit here)
    if (EMAC_TSR & TSR_COMP) {                                           // transmission complete
        eth_tx_control->ETH_queue.chars = 0;                             // reset the transmission buffer for next use
        eth_tx_control->ETH_queue.put = eth_tx_control->ETH_queue.QUEbuffer;
        if (ptrTxBd->bd_details & BD_TX_WRAP) {
            ptrTxBd = ptrFirstTxBd;
        }
        else {
            ptrTxBd++;
        }
    #if defined LAN_REPORT_ACTIVITY
        if (EMAC_TSR & TSR_COL) {                                        // collision detection
            EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_COLLISION;
            EMAC_TSR = TSR_COL;                                          // remove collision indicator
        }
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
        iInterruptLevel = 0;                                             // release
    #endif
        EMAC_TSR = (TSR_COL | TSR_COMP);                                 // reset the status flags
    }
    if (EMAC_RSR & RSR_REC) {                                            // frame(s) received
        EMAC_RSR = RSR_REC;                                              // reset the status flag
        ((ETHERNETQue*)(eth_rx_control))->ETH_queue.chars = (QUEUE_TRANSFER)(ptrRxBd->bd_details & BD_INFO_LENGTH_OF_FRAME_MASK); // put the length of the received frame in the buffer
    #if defined _WINDOWS
        iInterruptLevel++;                                               // windows can inject from protected region
    #else
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked when putting message to queue
    #endif
        fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_RX_int_message, HEADER_LENGTH); // inform the Ethernet task
    #if defined LAN_REPORT_ACTIVITY
        EMAC_int_message[MSG_INTERRUPT_EVENT] = EMAC_RX_INTERRUPT;
        fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
    #endif
    #if defined _WINDOWS
        iInterruptLevel--;
    #else
        iInterruptLevel = 0;                                             // release
    #endif
    }
    #if defined _WINDOWS
    EMAC_ISR = 0;                                                        // clear interrupt flag when simulating
    EMAC_TSR = 0;
    EMAC_RSR = 0;
    #endif
#else
    while ((ulInterrupts = EMAC_ISR) != 0) {                             // read the interrupt status register, which clears all interrupts
    #if defined LAN_REPORT_ACTIVITY
        unsigned char EMAC_int_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT };
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
    #if defined LAN_REPORT_ACTIVITY
            if (EMAC_TSR & TSR_COL) {                                    // collision detection
                EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_COLLISION;
                EMAC_TSR = TSR_COL;                                      // remove collision indicator
            }
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
            iInterruptLevel = 0;                                         // release
    #endif
        }

        if (ulInterrupts & RCOMP) {                                      // complete reception frame available
            ((ETHERNETQue*)(eth_rx_control))->ETH_queue.chars = (QUEUE_TRANSFER)(ptrRxBd->bd_details & BD_INFO_LENGTH_OF_FRAME_MASK); // put the length of the received frame in the buffer
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
    #if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME
            uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);         // schedule the Ethernet task
    #else
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_RX_int_message, HEADER_LENGTH); // inform the Ethernet task
    #endif
    #if defined LAN_REPORT_ACTIVITY
            EMAC_int_message[MSG_INTERRUPT_EVENT] = EMAC_RX_INTERRUPT;
            fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
    #endif
            iInterruptLevel = 0;                                         // release
        }

        ulInterrupts &= ~(TCOMP | RCOMP);

    #if defined _WINDOWS
        EMAC_ISR = 0;                                                    // clear interrupt flag when simulating
    #endif
    }
#endif
}

#if defined USE_IGMP || defined USE_IPV6
static unsigned char fnCalculateHash(unsigned char *ucData)
{
    unsigned char ucHashIndex = (*ucData ^ ((*ucData >> 6) ^ (*(ucData + 1) << 2)) ^ ((*(ucData + 1) >> 4) ^ (*(ucData + 2) << 4)) ^ (*(ucData + 2) >> 2) ^ *(ucData + 3) ^ (*(ucData + 3) >> 6) ^ (*(ucData + 4) << 2) ^ (*(ucData + 4) >> 4) ^ (*(ucData + 5) << 4) ^ (*(ucData + 5) >> 2));
    return (ucHashIndex & 0x3f);
}

static void fnEnableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        EMAC_HRB |= (0x00000001 << ucHashRef);                           // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        EMAC_HRT |= (0x00000001 << ucHashRef);
    }
    EMAC_NCFGR |= MTI;                                                   // enable multicast hash operation
}
#endif

#if defined USE_IPV6
extern void fnConfigureMulticastIPV6(void)
{
    unsigned char ucHashRef;
    unsigned char ucMac[MAC_LENGTH];                                     // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    ucMac[0] = 0x33;
    ucMac[1] = 0x33;
    ucMac[2] = 0xff;
    ucMac[3] = ucLinkLocalIPv6Address[DEFAULT_NETWORK][3 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[4] = ucLinkLocalIPv6Address[DEFAULT_NETWORK][4 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[5] = ucLinkLocalIPv6Address[DEFAULT_NETWORK][5 + (IPV6_LENGTH - MAC_LENGTH)];
    ucHashRef = fnCalculateHash(ucMac);
    fnEnableMulticastHash(ucHashRef);
    #if defined USE_IGMP
    ucGroupHashes[0] = ucHashRef;                                        // mark that this hash is required by IPv6
    #endif
}
#endif

#if defined USE_IGMP                                                     // {74}
static void fnDisableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        EMAC_HRB &= ~(0x00000001 << ucHashRef);                          // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        EMAC_HRT &= ~(0x00000001 << ucHashRef);
    }
    if ((EMAC_HRT == 0) && (EMAC_HRB == 0)) {
        EMAC_NCFGR &= ~MTI;                                              // disable multicast operation
    }
}

// IGMP adds and removes multicast host groups and requires the receiver's multicast filtering to be modified in the process
//
extern void fnModifyMulticastFilter(QUEUE_TRANSFER action, unsigned char *ptrIP)
{
    int iFree = -1;
    int iMatch = -1;
    int i;
    int iHashCount = 0;
    unsigned char ucMac[MAC_LENGTH];
    unsigned char ucHashRef;
    switch (action) {
    case REMOVE_IPV4_FILTER:
    case ADD_IPV4_FILTER:                                                // a filter has to be enabled for this IPv4 address
        ucMac[0] = 0x01;                                                 // IANA owned multicast ethernet address block
        ucMac[1] = 0x00;
        ucMac[2] = 0x5e;                                                 // enter the multicast MAC address belonging to IANA for this purpose
        ucMac[3] = (*(ptrIP + 1) & 0x7f);                                // add 23 bits of the destination IP address
        ucMac[4] = *(ptrIP + 2);
        ucMac[5] = *(ptrIP + 3);
        ucHashRef = fnCalculateHash(ucMac);
        break;
    default:                                                             // ignore any other calls
        return;
    }
    for (i = 0; i < MAX_MULTICAST_FILTERS; i++) {                        // check whether the entry exists
        if (ucGroupHashes[i] == ucHashRef) {                             // existing hash entry found
            iMatch = i;
            iHashCount++;                                                // count the number of groups requiring this hash
        }
        else if (ucGroupHashes[i] == 0) {                                // free location
            iFree = i;                                                   // remember the free entry
        }
    }
    if (action == ADD_IPV4_FILTER) {                                     // the entry doesn't exist so we add it
        if (iFree < 0) {
            _EXCEPTION("No more hash entry space");
        }
        else {
            ucGroupHashes[iFree] = ucHashRef;                            // note that this hash is required
            fnEnableMulticastHash(ucHashRef);                            // enable the hash entry
        }
    }
    else if (action == REMOVE_IPV4_FILTER) {
        if (iMatch >= 0) {                                               // remove an entry for this hash
            ucGroupHashes[iMatch] = 0;                                   // entry free
            if (iHashCount == 1) {                                       // single entry so it can be disabled
                fnDisableMulticastHash(ucHashRef);
            }
        }
    }
}
#endif


// Ethernet configuration routine
//
extern int fnConfigEthernet(ETHTABLE *pars)
{
#if defined SUPPORT_PORT_INTERRUPTS
    #if defined INTERRUPT_TASK_PHY
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    #endif
#else
    __interrupt void PortB_Interrupt(void);                              // phy interrupt
#endif
    unsigned long ulPhyIdentifier;
    unsigned short usMIIData;
    volatile int i = 0;
    SAM7X_RX_BD *ptrBd;
    unsigned char *ptrBuffer;
    unsigned long ulTemp;

    POWER_UP(PIOB);                                                      // {37} ensure peripheral clock to PIO channel B enabled

    DRIVE_PHY_OPTION_LINES();                                            // set the option lines as we would like them to be for the PHY being used

    RSTC_MR = (RESET_KEY | RESET_60US | URSTEN);                         // activate reset signal with 60us drive pulse
    RSTC_CR = (RESET_KEY | EXTRST);

    while (i++ < 2100) {};                                               // delay to ensure reset pulse long enough

    PIO_ASR_B  = MII_INTERFACE;                                          // configure the MII pins (this overrides the option outputs)
    PIO_PDR_B  = MII_INTERFACE;
    POWER_UP(EMAC);                                                      // {37} enable peripheral clock to EMAC module
    EMAC_NCFGR = (PAE | MCLK_32);                                        // set correct clock speed (valid between 40 and 80MHz) {PAE added}
    EMAC_USRIO = TRANSCEIVER_CLKEN;                                      // enable MII (not RMII)
    EMAC_NCR   = MPE;                                                    // enable the MDIO (management channel to PHY) and enable pause frames {5}

    ulPhyIdentifier = fnReadMII(KS8721_PHY_IDENTIFIER_1);                // check that the PHY is working correctly by reading its identifier
    ulPhyIdentifier <<= 16;
    ulPhyIdentifier |= fnReadMII(KS8721_PHY_IDENTIFIER_2);               // check that the PHY is working correctly by reading its identifier

    if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {   // {29}
        return -1;                                                       // {98} if the identifier is incorrect the phy isn't responding correctly
    }

    if (pars->usMode & (LAN_10M | LAN_100M)) {                           // if fixed speed
        usMIIData = 0;
        if (pars->usMode & LAN_100M)  {
            EMAC_NCFGR |= SPD;
            usMIIData |= PHY_SELECT_100M_SPEED;
        }
        if (pars->usMode & FULL_DUPLEX) {
            EMAC_NCFGR |= FD;
            usMIIData |= PHY_FULL_DUPLEX_MODE;
        }
    }
    else {                                                               // else auto-negotiate
        usMIIData = (PHY_ENABLE_AUTO_NEGOTIATION | PHY_RESTART_AUTO_NEGOTIATION);
    }

    if (pars->usMode & LOOPBACK) {
        usMIIData |= PHY_LOOP_BACK;                                      // test loop back at PHY
      //EMAC_NCR |= (LLB);                                               // test loop back at MII
    }

    fnWriteMII(KS8721_BASIC_CONTROL_REGISTER, usMIIData);                // write the PHY operating mode

/***************************************** MAC set up ****************************************/

    i = (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * pars->usSizeRx);      // set up receiver buffer memory
    ptrBuffer = uMallocAlign((MAX_MALLOC)i, 8);                          // allocate a receive buffer pool - which should consist of SAM7X_FIXED_BUFFER_SIZE byte blocks
    i /= SAM7X_FIXED_BUFFER_SIZE;                                        // the number of BDs required to control the receive buffer pool, consisting of SAM7X_FIXED_BUFFER_SIZE byte blocks
    ptrBd = uMallocAlign((MAX_MALLOC)(sizeof(SAM7X_RX_BD)*i), 8);        // create a smaller pool of buffer descriptors

    EMAC_RBQP = (unsigned long)ptrBd;                                    // inform the MAC where the first receive buffer is located
    ptrFirstRxBd = ptrRxBd = ptrBd;                                      // remember where it starts
    while (--i) {                                                        // initialise the buffer descriptors
        ptrBd->ptrBlock = ptrBuffer;                                     // initialise the buffer descriptors
        ptrBuffer += SAM7X_FIXED_BUFFER_SIZE;                            // the receive buffer size MUST be the fixed buffer size..
        ptrBd++;
    }
    ptrBd->ptrBlock = (unsigned char *)((unsigned long)ptrBuffer | BD_RX_WRAP);// set wrap bit in last buffer

    i = (NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE * pars->usSizeTx);      // set up transmitter buffer memory
    ptrBuffer = uMallocAlign((MAX_MALLOC)i, 8);                          // allocate a receive buffer pool - which should consist of SAM7X_FIXED_BUFFER_SIZE byte blocks

    i = NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE;                         // the number of BDs required to control the receive buffer pool, consisting of SAM7X_FIXED_BUFFER_SIZE byte blocks
    ptrBd = uMallocAlign((MAX_MALLOC)(sizeof(SAM7X_RX_BD)*i), 8);        // create a smaller pool of buffer descriptors

    EMAC_TBQP = (unsigned long)ptrBd;                                    // inform the MAC where the first receive buffer is located
    ptrFirstTxBd = ptrTxBd = ptrBd;                                      // remember where it starts
    while (--i) {                                                        // initialise the buffer descriptors
        ptrBd->ptrBlock = ptrBuffer;                                     // initialise the buffer descriptors
        ptrBd->bd_details = BD_INFO_USED;                                // NOTE: we presently use one large transmit buffer only
        ptrBuffer += SAM7X_FIXED_BUFFER_SIZE;                            // assuming SAM7X_FIXED_BUFFER_SIZE byte block
        ptrBd++;
    }
    ptrBd->ptrBlock = ptrBuffer;
    ptrBd->bd_details = (BD_INFO_USED | BD_TX_WRAP);                     // set wrap bit in last buffer

    #if defined USE_IPV6
    ulTemp =  (network[DEFAULT_NETWORK].ucOurMAC[3] << 24);              // set the MAC address
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[2] << 16);
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[1] << 8);
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[0]);
    EMAC_SA1B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  (network[DEFAULT_NETWORK].ucOurMAC[5] << 8);
    ulTemp |= (network[DEFAULT_NETWORK].ucOurMAC[4]);
    EMAC_SA1T = ulTemp;
    #else
    ulTemp =  (pars->ucMAC[3] << 24);                                    // set the MAC address
    ulTemp |= (pars->ucMAC[2] << 16);
    ulTemp |= (pars->ucMAC[1] << 8);
    ulTemp |= (pars->ucMAC[0]);
    EMAC_SA1B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  (pars->ucMAC[5] << 8);
    ulTemp |= (pars->ucMAC[4]);
    EMAC_SA1T = ulTemp;
    #endif

    #if defined SUPPORT_DISTRIBUTED_NODES
    ulTemp =  ucNodeMac[OurNetworkNumber-1][3]<<24;                      // set the uNetwork MAC address as second address
    ulTemp |= ucNodeMac[OurNetworkNumber-1][2]<<16;
    ulTemp |= ucNodeMac[OurNetworkNumber-1][1]<<8;
    ulTemp |= ucNodeMac[OurNetworkNumber-1][0];
    EMAC_SA2B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  ucNodeMac[OurNetworkNumber-1][5]<<8;
    ulTemp |= ucNodeMac[OurNetworkNumber-1][4];
    EMAC_SA2T = ulTemp;
        #if defined UNET_BROADCAST_MAC_ADDRESS                           // {31}
    ulTemp =  cucuNetworkBroadcast[3]<<24;                               // set the uNetwork broadcast MAC address as third address
    ulTemp |= cucuNetworkBroadcast[2]<<16;
    ulTemp |= cucuNetworkBroadcast[1]<<8;
    ulTemp |= cucuNetworkBroadcast[0];
    EMAC_SA3B = ulTemp;                                                  // start writing bottom, which disables the compare, until also top is written
    ulTemp =  cucuNetworkBroadcast[5]<<8;
    ulTemp |= cucuNetworkBroadcast[4];
    EMAC_SA3T = ulTemp;
        #endif
    #endif

    if (pars->usMode & PROMISCUOUS) {
        EMAC_NCFGR |= CAF;                                               // no destination address checking
    }
    #if defined USE_IPV6
    else if (pars->usMode & CON_MULTICAST) {                             // {72} if multicast reception is to be configured
        fnConfigureMulticastIPV6();                                      // set IPV6 multicast address
    }
    #endif

    fnEnterInterrupt(EMAC, PRIORITY_EMAC, EMAC_Interrupt);

    EMAC_IER = (TCOMP | RCOMP);                                          // enable EMAC interrupts

    EMAC_NCR |= (EMAC_TE | EMAC_RE | CLRSTAT);                           // enable transmitter and receiver

#if defined SUPPORT_PORT_INTERRUPTS
    #if defined INTERRUPT_TASK_PHY
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = fnPhyInterrupt;                        // handling function
    interrupt_setup.int_priority = PRIORITY_PIOB;                        // port interrupt priority
    interrupt_setup.int_port = PORT_B;                                   // the port used
    interrupt_setup.int_port_sense = IRQ_FALLING_EDGE;                   // interrupt on this edge
    interrupt_setup.int_port_bits = PHY_INTERRUPT;                       // the input connected
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt

    fnWriteMII(KS8721_INTERRUPT_CONTROL_STATUS_REGISTER,                 // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
    #endif
#else
    fnEnterInterrupt(PIOB, PRIORITY_PIOB, PortB_Interrupt);
    PIO_IER_B = PHY_INTERRUPT;                                           // enable interrupt line from PHY
    #if defined _WINDOWS
    PIO_IMR_B |= PIO_IER_B;
    #endif

    fnWriteMII(KS8721_INTERRUPT_CONTROL_STATUS_REGISTER,                 // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
#endif
#if defined _WINDOWS                                                     // {76}
    fnOpenDefaultHostAdapter();                                          // simulate link up
#endif
    return 0;                                                            // {98}
}



#if defined _WINDOWS

// Check whether the received MAC address matches a HASH address
//
static int fnMulticastHASH(unsigned char *ucData)                        // {72}
{
    if (EMAC_NCFGR & (MTI | UNI)) {
        unsigned char ucHashIndex;
        ucHashIndex = (*ucData ^ ((*ucData >> 6) ^ (*(ucData + 1) << 2)) ^ ((*(ucData + 1) >> 4) ^ (*(ucData + 2) << 4)) ^ (*(ucData + 2) >> 2) ^ *(ucData + 3) ^ (*(ucData + 3) >> 6) ^ (*(ucData + 4) << 2) ^ (*(ucData + 4) >> 4) ^ (*(ucData + 5) << 4) ^ (*(ucData + 5) >> 2));
        ucHashIndex &= 0x3f;
        if (ucHashIndex <= 32) {
            if (EMAC_HRB & (0x00000001 << ucHashIndex)) {
                if (*ucData & 0x01) {                                    // multicast address
                    if (EMAC_NCFGR & MTI) {
                        return 1;                                        // multicast hash address match
                    }
                }
                else {                                                   // unicast address
                    if (EMAC_NCFGR & UNI) {
                        return 2;                                        // unicast hash address match
                    }
                }
            }
        }
        else {
            ucHashIndex -= 32;
            if (EMAC_HRT & (0x00000001 << ucHashIndex)) {
                if (*ucData & 0x01) {                                    // multicast address
                    if (EMAC_NCFGR & MTI) {
                        return 1;                                        // multicast hash address match
                    }
                }
                else {                                                   // unicast address
                    if (EMAC_NCFGR & UNI) {
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
    #if defined SUPPORT_DISTRIBUTED_NODES
    unsigned char ucProgrammedMAC2[MAC_LENGTH];
        #if defined UNET_BROADCAST_MAC_ADDRESS                           // {31}
    unsigned char ucProgrammedMAC3[MAC_LENGTH];
        #endif
    #endif

    // This function checks to see whether we can receive the simulated frame and can block it if wanted
    //
    if (usLen < (2*MAC_LENGTH + 2)) {
        return 0;                                                        // too short to be a valid Ethernet frame
    }

    #if defined SUPPORT_DISTRIBUTED_NODES
    ucProgrammedMAC2[5] = (unsigned char)(EMAC_SA2T>>8);
    ucProgrammedMAC2[4] = (unsigned char)(EMAC_SA2T);
    ucProgrammedMAC2[3] = (unsigned char)(EMAC_SA2B>>24);
    ucProgrammedMAC2[2] = (unsigned char)(EMAC_SA2B>>16);
    ucProgrammedMAC2[1] = (unsigned char)(EMAC_SA2B>>8);
    ucProgrammedMAC2[0] = (unsigned char)(EMAC_SA2B);

        #if defined UNET_BROADCAST_MAC_ADDRESS                           // {31}
    ucProgrammedMAC3[5] = (unsigned char)(EMAC_SA3T>>8);
    ucProgrammedMAC3[4] = (unsigned char)(EMAC_SA3T);
    ucProgrammedMAC3[3] = (unsigned char)(EMAC_SA3B>>24);
    ucProgrammedMAC3[2] = (unsigned char)(EMAC_SA3B>>16);
    ucProgrammedMAC3[1] = (unsigned char)(EMAC_SA3B>>8);
    ucProgrammedMAC3[0] = (unsigned char)(EMAC_SA3B);
        #endif
    #endif

    ucProgrammedMAC[5] = (unsigned char)(EMAC_SA1T>>8);                  // we check only first mac address
    ucProgrammedMAC[4] = (unsigned char)(EMAC_SA1T);
    ucProgrammedMAC[3] = (unsigned char)(EMAC_SA1B>>24);
    ucProgrammedMAC[2] = (unsigned char)(EMAC_SA1B>>16);
    ucProgrammedMAC[1] = (unsigned char)(EMAC_SA1B>>8);
    ucProgrammedMAC[0] = (unsigned char)(EMAC_SA1B);

    if (!uMemcmp(&ucData[MAC_LENGTH], ucProgrammedMAC, MAC_LENGTH)) {
        return 0;                                                        // we never accept echoes of our own transmissions
    }

    #if defined SUPPORT_DISTRIBUTED_NODES
    if (!uMemcmp((ucData + MAC_LENGTH), ucProgrammedMAC2, MAC_LENGTH)) {
        return 0;                                                        // ignore broadcast frames which we have just sent
    }
        #if defined UNET_BROADCAST_MAC_ADDRESS                           // {31}
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

    #if defined SUPPORT_DISTRIBUTED_NODES
    if (!(uMemcmp(ucData, ucProgrammedMAC2, MAC_LENGTH))) {
        return 2;                                                        // our uNetwork MAC
    }
        #if defined UNET_BROADCAST_MAC_ADDRESS                           // {31}
    if (!(uMemcmp(ucData, ucProgrammedMAC3, MAC_LENGTH))) {
        return 2;                                                        // our uNetwork broadcast MAC
    }
        #endif
    #endif

    if ((iHashType = fnMulticastHASH(ucData)) != 0) {                    // {72}
        if (iHashType == 1) {
            return 4;                                                    // multicast hash match
        }
        else {
            return 5;                                                    // unicast hash match
        }
    }

    if (EMAC_NCFGR & CAF) {
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

// This can be called by the Ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return 1;          // NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE;       // the sam7x driver doesn't need to know
}

// This can be called by the Ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return 0;                                                            // for compatibility - the SAM7X uses buffer descriptors for tracking buffers
}

// This can be called by the Ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    return (unsigned char *)ptrTxBd->ptrBlock;                           // return pointer to bd which is to be used next
}

// This can be called by the Ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
// we don't use this in SAM7X since we copy input to a linear buffer
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
#if defined _WINDOWS                                                     // {15}
    while (DataLen < 60) {                                               // the SAM7x performs automatic zero padding to increase shorter frames to minimum 60 byte length
        *ptr_put++ = 0x00;                                               // pad with zeros if smaller than 60 [chip must send at least 60]
        DataLen++;
    }
#endif

    ptrTxBd->bd_details &= ~(BD_INFO_USED | BD_TX_FRAME_LENGTH | BD_LAST_FRAME_BUFFER); // clear flags and the old length
    ptrTxBd->bd_details |= DataLen;                                      // set new frame length
    ptrTxBd->bd_details |= BD_LAST_FRAME_BUFFER;                         // if the frame fits in one buffer, set flag to say so
    EMAC_NCR |= TSTART;                                                  // initiate transmission

#if defined _WINDOWS
    fnSimulateEthTx((QUEUE_TRANSFER)(ptrTxBd->bd_details & BD_TX_FRAME_LENGTH), (ptr_put - DataLen)); // {102}

    if (usPhyMode & PHY_LOOP_BACK) {                                     // if the PHY is in loop back mode, simulate reception of sent frame
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
    #if defined PSEUDO_LOOPBACK                                          // {58} if we detect an IP frame being sent to our own address we loop it back to the input
    else if (((*(ptr_put - DataLen + 12)) == 0x08) && (!(uMemcmp(&network[DEFAULT_NETWORK].ucOurIP[0], (ptr_put - DataLen + 26), IPV4_LENGTH)))) {
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
    #endif

    EMAC_ISR |= TCOMP;
    EMAC_TSR |= TSR_COMP;                                                // {32} set so that interrupt type is recognised when using workaround
    ptrTxBd->bd_details |= (BD_INFO_USED);                               // simulate the EMAC setting the used bit after transmission
    EMAC_Interrupt();                                                    // simulate end of transmission - tx interrupt
#endif
    return (DataLen);
}
#endif                                                                   // end of Ethernet option




#if defined SERIAL_INTERFACE

/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */

static unsigned long *fnSelectChannel(QUEUE_HANDLE Channel)
{
    if (!Channel) {
        return (unsigned long *)USART0_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
    }
    else {
    #if defined DBGU_UART                                                // {35}
        if (Channel == 2) {
            return (unsigned long *)DBGU_PERIPHERAL_BLOCK;               // set register pointer to corresponding block
        }
    #endif
        return (unsigned long *)USART1_PERIPHERAL_BLOCK;                 // set register pointer to corresponding block
    }
}

// Configure the SCI hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    unsigned long *ulReg;
    switch (Channel) {
    case 0:
        POWER_UP(USART0);                                                // {37} enable clocks to UART0
        break;
    case 1:
        POWER_UP(USART1);                                                // {37} enable clocks to UART1
        break;
    #if defined DBGU_UART                                                // {35}
    case 2:
        POWER_UP(SYSIRQ);                                                // {37} enable clocks to UART2 (DBGU)
        break;
    #endif
    default:
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {69}
        fnConfigExtSCI((QUEUE_HANDLE)(Channel - NUMBER_SERIAL), pars);   // pass on to external UART driver
        #if defined SUPPORT_HW_FLOW
        if (pars->Config & RTS_CTS) {                                     // HW flow control defined so configure RTS/CTS pins
            fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
        }
        #endif
    #endif
        return;
    }
    #if defined SUPPORT_HW_FLOW                                          // {60}
    if (pars->Config & RTS_CTS) {                                        // {65} HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    ulReg = fnSelectChannel(Channel);
    *ulReg = (SAM7_TX_OFF | SAM7_RX_OFF);                                // start with uart disabled
    ulReg += (US_BRGR_OFFSET/sizeof(unsigned long));                     // set to baud rate generator register

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        *ulReg = ((MASTER_CLOCK/16 + 300/2)/300);                        // set 300
        break;
    case SERIAL_BAUD_600:
        *ulReg = ((MASTER_CLOCK/16 + 600/2)/600);                        // set 600
        break;
    case SERIAL_BAUD_1200:
        *ulReg = ((MASTER_CLOCK/16 + 1200/2)/1200);                      // set 1200
        break;
    case SERIAL_BAUD_2400:
        *ulReg = ((MASTER_CLOCK/16 + 2400/2)/2400);                      // set 2400
        break;
    case SERIAL_BAUD_4800:
        *ulReg = ((MASTER_CLOCK/16 + 4800/2)/4800);                      // set 4800
        break;
    case SERIAL_BAUD_9600:
        *ulReg = ((MASTER_CLOCK/16 + 9600/2)/9600);                      // set 9600
        break;
    case SERIAL_BAUD_14400:
        *ulReg = ((MASTER_CLOCK/16 + 14400/2)/14400);                    // set 14400
        break;
    default:                                                             // if not valid value set this one
    case SERIAL_BAUD_19200:
        *ulReg = ((MASTER_CLOCK/16 + 19200/2)/19200);                    // set 19200
        break;
    case SERIAL_BAUD_38400:
        *ulReg = ((MASTER_CLOCK/16 + 38400/2)/38400);                    // set 38400
        break;
    case SERIAL_BAUD_57600:
        *ulReg = ((MASTER_CLOCK/16 + 57600/2)/57600);                    // set 57600
        break;
    case SERIAL_BAUD_115200:
        *ulReg = ((MASTER_CLOCK/16 + 115200/2)/115200);                  // set 115200
        break;
    case SERIAL_BAUD_230400:
        *ulReg = ((MASTER_CLOCK/16 + 230400/2)/230400);                  // set 230400
        break;
    }

    ulReg -= ((US_BRGR_OFFSET - US_MR_OFFSET)/sizeof(unsigned long));    // set to mode register

    if (pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {         // {65}
        if (pars->Config & RS232_ODD_PARITY) {                           // {65}
            *ulReg = SAM7_ODD_PARITY;
        }
        else {
            *ulReg = SAM7_EVEN_PARITY;
        }
    }
    else {
        *ulReg = SAM7_NO_PARITY;
    }

    if (pars->Config & TWO_STOPS) {                                      // {65}
        *ulReg |= SAM7_2_STOPS;
    }
    else if (pars->Config & ONE_HALF_STOPS) {                            // {65}
        *ulReg |= SAM7_ONE_HALF_STOPS;
    }
#if defined UART_EXTENDED_MODE && (defined SERIAL_MULTIDROP_TX || defined SERIAL_MULTIDROP_RX) // {90}
    if (pars->Config & (MULTIDROP_MODE_RX | MULTIDROP_MODE_TX)) {        // 9-bit - multi-drop mode
        *ulReg |= USART_MODE9;
    }
    else if (pars->Config & CHAR_7) {
        *ulReg |= SAM7_CHAR_7;
    }
    else {
        *ulReg |= SAM7_CHAR_8;
    }
#else
    if (pars->Config & CHAR_7) {                                         // {65}
        *ulReg |= SAM7_CHAR_7;
    }
    else {
        *ulReg |= SAM7_CHAR_8;
    }
#endif

#if defined MODBUS_RTU
    if (Channel < 2) {
        if (pars->Config & RTU_RX_MODE) {                                // {65} use internal idle timeout on the USARTs
            *(ulReg + 8) = 17;                                           // timeout after 17 bit period with no character reception (T1.5) [US_RTOR]
            *(ulReg + 1) = RX_TIMEOUT;                                   // enable interrupt on idle timeout [US_IER]
        }
        else {
            *(ulReg + 8) = 0;                                            // disable timeouts [US_RTOR]
        }
    }
#endif
#if defined _WINDOWS
    ulReg += ((US_BRGR_OFFSET - US_MR_OFFSET)/sizeof(unsigned long));    // set back to speed setting
    if (Channel == 2) {
        pars->Config &= ~CHAR_7;                                         // {65} the DBGU only supports 8 bit characters
    }
    fnConfigSimSCI(Channel, MASTER_CLOCK/(*ulReg*16), pars);             // open a serial port on PC if desired
#endif
}

// SCI0 interrupt
//
static __interrupt void SCI0_Interrupt(void)
{
#if defined SUPPORT_HW_FLOW                                              // {60}
    unsigned long ulStatus;
#endif
    iInterruptLevel = 1;                                                 // {22}
    while (AIC_IPR & USART0) {                                           // while interrupts present from UART0
#if defined SUPPORT_HW_FLOW                                              // {60}
        ulStatus = US_CSR_0;                                             // read and reset the input change states
    #if defined _WINDOWS
        US_CSR_0 &= ~CTSIC;
    #endif
#endif
#if defined SERIAL_SUPPORT_DMA
        if ((US_IMR_0 & RXRDY) && (US_CSR_0 & RXRDY))
#else
        if (US_CSR_0 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {90}
            ulLastReceived[0] = US_RHR_0;                                // read the byte (clearing the interrupt)
            fnSciRxByte((unsigned char)ulLastReceived[0], 0);
#else
            fnSciRxByte((unsigned char)US_RHR_0, 0);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#if defined _WINDOWS
            US_CSR_0 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#if defined SERIAL_SUPPORT_DMA
        if ((US_IMR_0 & ENDRX) && (US_CSR_0 & ENDRX)) {
            US_IDR_0 = ENDRX;                                            // disable to avoid further
    #if defined _WINDOWS
            US_IMR_0 &= ~ENDRX;
            US_CSR_0 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 0);
        }
        if ((US_IMR_0 & ENDTX) && (US_CSR_0 & ENDTX)) {
            US_IDR_0 = ENDTX;                                            // disable to avoid further
    #if defined _WINDOWS
            US_IMR_0 &= ~ENDTX;
    #endif
            fnSciTxByte(0);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_0 & TXRDY) && (US_CSR_0 & TXRDY)) {
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
        }
#if defined SUPPORT_HW_FLOW                                              // {60}
        if ((US_IMR_0 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(0, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#if defined MODBUS_RTU
        if ((US_IMR_0 & RX_TIMEOUT & US_CSR_0)) {                        // idle timeout
            int iNext = fnSciRxIdle(0);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_0 = (unsigned long)iNext;                        // set the new value
                US_CR_0 = (SAM7X_STTTO | SAM7X_RETTO);                   // reset and re-trigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_0 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_0 = SAM7X_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#if defined _WINDOWS
        AIC_IPR &= ~USART0;                                              // simulate reset of interrupt flag
#endif
    }
    iInterruptLevel = 0;                                                 // {22}
}

// SCI1 interrupt
//
static __interrupt void SCI1_Interrupt(void)
{
#if defined SUPPORT_HW_FLOW                                              // {60}
    unsigned long ulStatus;
#endif
    iInterruptLevel = 1;                                                 // {22}
    while (AIC_IPR & USART1) {                                           // while interrupts present from UART1
#if defined SUPPORT_HW_FLOW                                              // {60}
        ulStatus = US_CSR_1;                                             // read and reset the input change states
    #if defined _WINDOWS
        US_CSR_1 &= ~CTSIC;
    #endif
#endif
#if defined SERIAL_SUPPORT_DMA
        if ((US_IMR_1 & RXRDY) && (US_CSR_1 & RXRDY))
#else
        if (US_CSR_1 & RXRDY)
#endif
        {
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {90}
            ulLastReceived[1] = US_RHR_1;                                // read the byte (clearing the interrupt)
            fnSciRxByte((unsigned char)ulLastReceived[1], 1);
#else
            fnSciRxByte((unsigned char)US_RHR_1, 1);                     // receive data interrupt - read the byte (clearing the interrupt)
#endif
#if defined _WINDOWS
            US_CSR_1 &= ~(RXRDY);                                        // simulate reset of interrupt flag
#endif
        }
#if defined SERIAL_SUPPORT_DMA
        if ((US_IMR_1 & ENDRX) && (US_CSR_1 & ENDRX)) {
            US_IDR_1 = ENDRX;                                            // disable to avoid further
    #if defined _WINDOWS
            US_IMR_1 &= ~ENDRX;
            US_CSR_1 &= ~ENDRX;
    #endif
            fnSciRxByte(0, 1);
        }
        if ((US_IMR_1 & ENDTX) && (US_CSR_1 & ENDTX)) {
            US_IDR_1 = ENDTX;                                            // disable to avoid further
    #if defined _WINDOWS
            US_IMR_1 &= ~ENDTX;
    #endif
            fnSciTxByte(1);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((US_IMR_1 & TXRDY) && (US_CSR_1 & TXRDY)) {
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
        }
#if defined SUPPORT_HW_FLOW                                              // {60}
        if ((US_IMR_1 & CTSIC) && (ulStatus & CTSIC)) {                  // input change active and CTS changed
            fnRTS_change(1, ((ulStatus & CTS_HIGH) == 0));               // control transmission according to state
        }
#endif
#if defined MODBUS_RTU
        if ((US_IMR_1 & RX_TIMEOUT & US_CSR_1)) {                        // idle timeout
            int iNext = fnSciRxIdle(1);
            if (iNext > 0) {                                             // new value requested
                US_RTOR_1 = (unsigned long)iNext;                        // set the new value
                US_CR_1 = (SAM7X_STTTO | SAM7X_RETTO);                   // reset and re-trigger idle time
            }
            else {
                if (iNext != 0) {
                     US_RTOR_1 = (unsigned long)(-iNext);                // set the new value
                }
                US_CR_1 = SAM7X_STTTO;                                   // reset and wait for next character followed by an idle time
            }
        }
#endif
#if defined _WINDOWS
        AIC_IPR &= ~USART1;                                              // simulate reset of interrupt flag
#endif
    }
    iInterruptLevel = 0;                                                 // {22}
}

#if defined DBGU_UART__REMOVED                                           // {35} {93} - share TICK interrupt
// DBGU interrupt
//
static __interrupt void SCI2_Interrupt(void)
{
    iInterruptLevel = 1;
    while (AIC_IPR & SYSIRQ) {                                           // while interrupts present from UART2 (DBGU)
    #if defined SERIAL_SUPPORT_DMA
        if ((DBGU_IMR & RXRDY) && (DBGU_SR & RXRDY))
    #else
        if (DBGU_SR & RXRDY)
    #endif
        {
            fnSciRxByte((unsigned char)DBGU_RHR, 2);                     // receive data interrupt - read the byte (clearing the interrupt)
    #if defined _WINDOWS
            DBGU_SR &= ~(RXRDY);                                         // simulate reset of interrupt flag
    #endif
        }
    #if defined SERIAL_SUPPORT_DMA
        if ((DBGU_IMR & ENDRX) && (DBGU_SR & ENDRX)) {
            DBGU_IDR = ENDRX;                                            // disable to avoid further
        #if defined _WINDOWS
            DBGU_IMR &= ~ENDRX;
            DBGU_SR &= ~ENDRX;
        #endif
            fnSciRxByte(0, 2);
        }
        if ((DBGU_IMR & ENDTX) && (DBGU_SR & ENDTX)) {
            DBGU_IDR = ENDTX;                                            // disable to avoid further
        #if defined _WINDOWS
            DBGU_IMR &= ~ENDTX;
        #endif
            fnSciTxByte(2);                                              // transmit block complete interrupt - write next block
        }
    #endif
        if ((DBGU_IMR & TXRDY) && (DBGU_SR & TXRDY)) {
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
        }
        if (PIT_SR & PITS) {
            _RealTimeInterrupt();                                        // if the PIT has fired, call the handler
        }
    #if defined _WINDOWS
        AIC_IPR &= ~SYSIRQ;                                              // simulate reset of interrupt flag
    #endif
    }
    iInterruptLevel = 0;
}
#endif


#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel)
{
    if (ulLastReceived[channel] & US_RHR_RXSYNH) {                       // {90} return 9th data bit value
        return 0x01;
    }  
    return 0x00;
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

    switch (Channel) {
    case 0:
        fnEnterInterrupt(USART0, PRIORITY_UART0, SCI0_Interrupt);
    #if defined _HW_SAM7L                                                // {100}
        PIO_ASR_C  = (UART_RX_0);                                        // configure the RX pin for UART0 use
        PIO_PDR_C  = (UART_RX_0);
    #else
        PIO_ASR_A  = (UART_RX_0);                                        // configure the RX pin for UART0 use
        PIO_PDR_A  = (UART_RX_0);
    #endif
        break;
    case 1:
        fnEnterInterrupt(USART1, PRIORITY_UART1, SCI1_Interrupt);
    #if defined _HW_SAM7L                                                // {100}
        PIO_ASR_C  = (UART_RX_1);                                        // configure the RX pin for UART1 use
        PIO_PDR_C  = (UART_RX_1);
    #else
        PIO_ASR_A  = (UART_RX_1);                                        // configure the RX pin for UART1 use
        PIO_PDR_A  = (UART_RX_1);
    #endif
        break;
#if defined DBGU_UART                                                    // {35}
    case 2:
      //fnEnterInterrupt(SYSIRQ, PRIORITY_UART2, SCI2_Interrupt);        // {93} - share TICK interrupt and don't modify interrupt priority
    #if defined _HW_SAM7L                                                // {100}
        PIO_ASR_C = (UART_RX_2);                                         // configure the RX pin for UART2 (DBGU) use
        PIO_PDR_C = (UART_RX_2);
    #else
        PIO_ASR_A = (UART_RX_2);                                         // configure the RX pin for UART2 (DBGU) use
        PIO_PDR_A = (UART_RX_2);
    #endif
        break;
#endif
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {69}
    default:
        fnExtSCIRxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
#endif
    }                                                _SIM_PORT_CHANGE    // ensure simulator registers port changes

    *ulReg = SAM7_RX_ON;                                                 // enable the receiver

#if defined SERIAL_SUPPORT_DMA                                           // {21}
    if ((*(ulReg+2) & ENDRX) != 0) {                                     // don't enable receive ready interrupt when in DMA mode
        return;
    }
#endif
#if defined _WINDOWS
    *(ulReg+4) |= *(ulReg+2);
#endif
    *(ulReg+2) = RXRDY;                                                  // enable interrupts when character received
#if defined _WINDOWS
    *(ulReg+4) |= *(ulReg+2);                                            // {21} set mask appropriately
#endif
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {69}
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
    }
    else {
        unsigned long *ulReg = fnSelectChannel(Channel);
        *ulReg = SAM7_RX_OFF;
    }
#else
    unsigned long *ulReg = fnSelectChannel(Channel);
    *ulReg = SAM7_RX_OFF;
#endif
}

// Enable transmission on the defined channel
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    unsigned long *ulReg = fnSelectChannel(Channel);

    switch (Channel) {
    case 0:
        fnEnterInterrupt(USART0, PRIORITY_UART0, SCI0_Interrupt);        // set the interrupt handling routine
    #if defined _HW_SAM7L                                                // {100}
        PIO_ASR_C  = (UART_TX_0);                                        // configure the TX pin for UART0 use
        PIO_PDR_C  = (UART_TX_0);
    #else
        PIO_ASR_A  = (UART_TX_0);                                        // configure the TX pin for UART0 use
        PIO_PDR_A  = (UART_TX_0);
    #endif
        break;
    case 1:
        fnEnterInterrupt(USART1, PRIORITY_UART1, SCI1_Interrupt);
    #if defined _HW_SAM7L                                                // {100}
        PIO_ASR_C  = (UART_TX_1);                                        // configure the TX pin for UART1 use
        PIO_PDR_C  = (UART_TX_1);
    #else
        PIO_ASR_A  = (UART_TX_1);                                        // configure the TX pin for UART1 use
        PIO_PDR_A  = (UART_TX_1);
    #endif
        break;
#if defined DBGU_UART                                                    // {35}
    case 2:
      //fnEnterInterrupt(SYSIRQ, PRIORITY_UART2, SCI2_Interrupt);        // {93} - share TICK interrupt and don't modify interrupt priority
    #if defined _HW_SAM7L                                                // {100}
        PIO_ASR_C  = (UART_TX_2);                                        // configure the TX pin for UART2 (DBGU) use
        PIO_PDR_C  = (UART_TX_2);
    #else
        PIO_ASR_A  = (UART_TX_2);                                        // configure the TX pin for UART2 (DBGU) use
        PIO_PDR_A  = (UART_TX_2);
    #endif
        break;
#endif
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {69}
    default:
        fnExtSCITxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
#endif
    }                                               _SIM_PORT_CHANGE     // ensure simulator registers port changes

    *(ulReg+2) = TXRDY;                                                  // enable interrupts when tx buffer empty (when enabling this will cause an interrupt)
    *ulReg = SAM7_TX_ON;                                                 // enable the transmitter

#if defined _WINDOWS
    switch (Channel) {
    case 0:
        US_CSR_0 |= TXRDY;
        break;
    case 1:
        US_CSR_1 |= TXRDY;
        break;
    #if defined DBGU_UART                                                // {35}
    case 2:
        DBGU_SR  |= TXRDY;
        break;
    #endif
    }
#endif
}

// Disable transmission on the defined channel
//
void fnTxOff(QUEUE_HANDLE Channel)
{
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {69}
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
    }
    else {
        unsigned long *ulReg = fnSelectChannel(Channel);
        *ulReg = SAM7_TX_OFF;
    }
#else
    unsigned long *ulReg = fnSelectChannel(Channel);
    *ulReg = SAM7_TX_OFF;
#endif
}

#if defined UART_BREAK_SUPPORT                                           // {1}
extern void fnStartBreak(QUEUE_HANDLE channel)
{
    unsigned long *ulReg = fnSelectChannel(channel);
    *ulReg = SAM7_START_BRK;
    #if defined _WINDOWS
    fnControlBreak(channel, 1);
    #endif
}

extern void fnStopBreak(QUEUE_HANDLE channel)
{
    unsigned long *ulReg = fnSelectChannel(channel);
    *ulReg = SAM7_STOP_BRK;
    #if defined _WINDOWS
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
    #if defined _WINDOWS
        US_IMR_0 &= ~(TXRDY);                                            // reflect in status register
    #endif
        break;
    case 1:
        US_IDR_1 = TXRDY;                                                // disable tx interrupt
    #if defined _WINDOWS
        US_IMR_1 &= ~(TXRDY);                                            // reflect in status register
    #endif
        break;
    #if defined DBGU_UART                                                // {35}
    case 2:
        DBGU_IDR = TXRDY;                                                // disable tx interrupt
        #if defined _WINDOWS
        DBGU_IMR &= ~(TXRDY);                                            // reflect in status register
        #endif
        break;
    #endif
    }
}

#if defined SERIAL_SUPPORT_DMA                                           // {21}
// Set up the PDC for reception of the defined character block
//
extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
    unsigned long *ulReg = (fnSelectChannel(channel) + (PDC_CHANNEL_OFFSET/sizeof(unsigned long)));
    *ulReg++ = (unsigned long)ptrStart;                                  // transfer start location
    *ulReg = (unsigned short)rx_length;                                  // transfer count US_RCR
    ulReg += 7;                                                          // move to US_PTCR
    *ulReg = PDC_RXTEN;                                                  // enable transfer
    ulReg -= ((PDC_CHANNEL_OFFSET/sizeof(unsigned long)) + 6);           // set US_IER
    *ulReg = ENDRX;                                                      // enable interrupt on transfer complete
    #if defined _WINDOWS
    *(ulReg+2) |= ENDRX;                                                 // set mask register accordingly
    switch (channel) {
    case 0:
        US_PTSR_0 |= PDC_RXTEN;
        break;
    case 1:
        US_PTSR_1 |= PDC_RXTEN;
        break;
    case 2:                                                              // {35}
        DBGU_PTSR |= PDC_RXTEN;
        break;
    }
    #endif
}

// Start transfer of a block via DMA
//
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length) // {62}
{
    unsigned long *ulReg = (fnSelectChannel(channel) + (PDC_CHANNEL_OFFSET/sizeof(unsigned long)));
    ulReg += 2;                                                          // move to US_TPR
    *ulReg++ = (unsigned long)ptrStart;                                  // transfer start location
    *ulReg = (unsigned short)tx_length;                                  // transfer count US_TCR
    ulReg += 5;                                                          // move to US_PTCR
    *ulReg = PDC_TXTEN;                                                  // start transfer
    ulReg -= ((PDC_CHANNEL_OFFSET/sizeof(unsigned long)) + 6);           // set US_IER
    *ulReg = ENDTX;                                                      // enable interrupt on transfer complete
    #if defined _WINDOWS
    *(ulReg+2) |= ENDTX;                                                 // set mask register accordingly
    *(ulReg+3) &= ~ENDTX;                                                // {48} reset ENDTX in status register
    switch (channel) {
    case 0:
        US_PTSR_0 |= PDC_TXTEN;
        iDMA |= DMA_CONTROLLER_0;
        break;
    case 1:
        US_PTSR_1 |= PDC_TXTEN;
        iDMA |= DMA_CONTROLLER_2;
        break;
    case 2:                                                              // {35}
        DBGU_PTSR |= PDC_TXTEN;
        iDMA |= DMA_CONTROLLER_4;
        break;
    }
    #endif
    return tx_length;
}

    #define SERIAL_SUPPORT_XON_XOFF
extern QUEUE_TRANSFER fnRemainingDMA_tx(QUEUE_HANDLE channel)            // {63}
{
    return 0;
}
    #endif

extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue) // {63}
{
    unsigned long *ulReg = (fnSelectChannel(channel) + (PDC_CHANNEL_OFFSET/sizeof(unsigned long)));
    *(ulReg + 8) = PDC_TXTDIS;                                           // disable transmitter DMA operation (US_PTCR)
    #if defined _WINDOWS
    *(ulReg + 8) = 0;
    *(ulReg + 9) &= ~PDC_TXTEN;                                          // disable status in US_PTSR
    #endif
    return (QUEUE_TRANSFER)(*(ulReg + 3));                               // return the aborted length
}


// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {90}
    unsigned long ulTransmitData;
    if (ulNextSent[channel] != 0) {
        ulTransmitData = (US_THR_TXSYNH | ucTxByte);                     // prepare 9th set to '1' in case in 9-bit mode
        ulNextSent[channel] = 0;
    }
    else {
        ulTransmitData = ucTxByte;
    }
    #endif
    switch (channel) {
    case 0:
        if (US_CSR_0 & TXRDY) {                                          // tx not busy (channel 0)
            US_IER_0 = TXRDY;                                            // ensure tx interrupt enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {90}
            US_THR_0 = ulTransmitData;
    #else
            US_THR_0 = ucTxByte;                                         // send byte
    #endif
    #if defined _WINDOWS
            US_IMR_0 |= TXRDY;
            US_CSR_0 &= ~(TXRDY);
            iInts |= CHANNEL_0_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;

    case 1:
        if (US_CSR_1 & TXRDY) {                                          // tx not busy (channel 1)
            US_IER_1 = TXRDY;                                            // ensure tx interrupt enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {90}
            US_THR_1 = ulTransmitData;
    #else
            US_THR_1 = ucTxByte;                                         // send byte
    #endif
    #if defined _WINDOWS
            US_IMR_1 |= TXRDY;
            US_CSR_1 &= ~(TXRDY);
            iInts |= CHANNEL_1_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;
    #if defined DBGU_UART                                                // {35}
    case 2:
        if (DBGU_SR & TXRDY) {                                           // tx not busy (channel 2 - DBGU)
            DBGU_IER = TXRDY;                                            // ensure tx interrupt enabled
            DBGU_THR = ucTxByte;                                         // send byte
    #if defined _WINDOWS
            DBGU_IMR |= TXRDY;
            DBGU_SR &= ~(TXRDY);
            iInts |= CHANNEL_2_SERIAL_INT;                               // simulate interrupt
    #endif
            return 0;                                                    // OK, we were not busy
        }
        break;
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {69}
    default:
        fnExtSCI_send((QUEUE_HANDLE)(channel - NUMBER_SERIAL), ucTxByte);// pass on to the external interface for transmission
        return 0;
    #endif
    }
    return 1;                                                            // not sent because we're busy
}

    #if defined SUPPORT_HW_FLOW

        #if defined DBGU_UART && defined CTS_2_PIN
// CTS interrupt
//
static void CTS_Interrupt(void)
{
    fnRTS_change(2, ((CTS_2_PORT_STATE & CTS_2_PIN) == 0));              // control transmission according to state
}
        #endif

static void fnSetRTS(QUEUE_HANDLE channel, int iState)                   // {60}
{
    switch (channel) {
    case 0:
        if (iState != 0) {
            US_CR_0 = SAM7_RTSEN;                                        // assert RTS
        }
        else {
            US_CR_0 = SAM7_RTSDIS;                                       // negate RTS
        }
        break;
    case 1:
        if (iState != 0) {
            US_CR_1 = SAM7_RTSEN;                                        // assert RTS
        }
        else {
            US_CR_1 = SAM7_RTSDIS;                                       // negate RTS
        }
        break;
        #if defined DBGU_UART && defined RTS_2_PIN                       // DBGU RTS port line
    case 2:
        if (iState != 0) {
            RTS_2_PORT_CLEAR = RTS_2_PIN;                                // assert RTS
        }
        else {
            RTS_2_PORT_SET = RTS_2_PIN;                                  // negate RTS
        }
        break;
        #endif
        #if NUMBER_EXTERNAL_SERIAL > 0                                   // {69}
    default:
        fnSetExtRTS((QUEUE_HANDLE)(channel - NUMBER_SERIAL), iState);
        break;
        #endif
    }
}

// The TTY driver uses this routine to configure and control UART control lines
//
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode) // {59}
{
    if (usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) {           // {60}
        if (usModifications & CONFIG_RTS_PIN) {
            switch (channel) {
            case 0:
                POWER_UP(PIOA);                                          // ensure peripheral clock to PIO channel A enabled
                PIO_ASR_A  = (RTS_0);                                    // configure the RTS0 pin for UART0 use
                PIO_PDR_A  = (RTS_0);
                if (usModifications & SET_RS485_MODE) {                  // the SAM7X supports RS485 mode which automatically controls the RTS line for RS485 direction control
                    US_MR_0 = ((US_MR_0 & ~0x7) | USART_MODE_RS485);
                }
                else {
                    fnSetRTS(channel, 0);                                // prepare to drive the RTS line in negated state
                }
                break;
            case 1:
                POWER_UP(PIOA);                                          // ensure peripheral clock to PIO channel A enabled
                PIO_ASR_A  = (RTS_1);                                    // configure the RTS0 pin for UART1 use
                PIO_PDR_A  = (RTS_1);
                if (usModifications & SET_RS485_MODE) {                  // the SAM7X supports RS485 mode which automatically controls the RTS line for RS485 direction control
                    US_MR_1 = ((US_MR_1 & ~0x7) | USART_MODE_RS485);
                }
                else {
                    fnSetRTS(channel, 0);                                // prepare to drive the RTS line in negated state
                }
                break;
        #if defined DBGU_UART && defined RTS_2_PIN                       // debug unit optionally uses ports for RTS/CTS lines
            case 2:
                POWER_UP(RTS_2_PORT);                                    // power up the port and configure as output
                fnSetRTS(channel, 0);                                    // prepare to drive the RTS line in negated state
                RTS_2_PORT_OUT = RTS_2_PIN;
                break;
        #endif
        #if NUMBER_EXTERNAL_SERIAL > 0                                   // {69}
            default:
                fnSetRTS(channel, 0);                                    // prepare to drive the RTS line in negated state
                break;
        #endif
            }
        }
        if (usModifications & CONFIG_CTS_PIN) {
            switch (channel) {
            case 0:
                POWER_UP(PIOA);                                          // ensure peripheral clock to PIO channel A enabled
                PIO_ASR_A  = (CTS_0);                                    // configure the CTS0 pin for UART0 use
                PIO_PDR_A  = (CTS_0);                                    _SIM_PORT_CHANGE
                break;
            case 1:
                POWER_UP(PIOA);                                          // ensure peripheral clock to PIO channel A enabled
                PIO_ASR_A  = (CTS_1);                                    // configure the CTS1 pin for UART1 use
                PIO_PDR_A  = (CTS_1);                                    _SIM_PORT_CHANGE
                break;
        #if defined DBGU_UART && defined CTS_2_PIN                       // debug unit optionally uses ports for RTS/CTS lines
            case 2:
                POWER_UP(CTS_2_PORT);                                    // power up the port and configure as digital input
                CTS_2_PORT_IN = CTS_2_PIN;                               _SIM_PORT_CHANGE
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
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode) // {59}
{
    QUEUE_TRANSFER ControlLines = 0;                                     // {60}
        #if NUMBER_EXTERNAL_SERIAL > 0                                   // {69}
    if (channel >= NUMBER_SERIAL) {
        return (fnControlExtLineInterrupt((QUEUE_HANDLE)(channel - NUMBER_SERIAL), usModifications, OperatingMode));
    }
        #endif

    if (ENABLE_CTS_CHANGE & usModifications) {
        switch (channel) {
        case 0:
            US_IER_0 = CTSIC;                                            // enable interrupt on input change
        #if defined _WINDOWS
            US_IER_0 = 0;
            US_IMR_0 |= CTSIC;
        #endif
            break;
        case 1:
            US_IER_1 = CTSIC;                                            // enable interrupt on input change
        #if defined _WINDOWS
            US_IER_1 = 0;
            US_IMR_1 |= CTSIC;
        #endif
            break;
        #if defined DBGU_UART && defined CTS_2_PIN
        case 2:
            {
                INTERRUPT_SETUP interrupt_setup;                         // interrupt configuration parameters
                interrupt_setup.int_type = PORT_INTERRUPT;               // identifier when configuring
                interrupt_setup.int_handler = CTS_Interrupt;             // handling function
            #if CTS_2_PORT == PIOA
                interrupt_setup.int_priority = PRIORITY_PIOA;            // port interrupt priority
                interrupt_setup.int_port = PORT_A;                       // the port used
            #else
                interrupt_setup.int_priority = PRIORITY_PIOB;            // port interrupt priority
                interrupt_setup.int_port = PORT_B;                       // the port used
            #endif
                interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | IRQ_RISING_EDGE); // interrupt on both edges
                interrupt_setup.int_port_bits = CTS_2_PIN;               // the input connected
                fnConfigureInterrupt((void *)&interrupt_setup);          // configure test interrupt
            }
            break;
        #endif
        }
    }
    if (DISABLE_CTS_CHANGE & usModifications) {
        switch (channel) {
        case 0:
            US_IDR_0 = CTSIC;                                            // disable interrupt on input change
        #if defined _WINDOWS
            US_IDR_0 = 0;
            US_IMR_0 &= ~CTSIC;
        #endif
            break;
        case 1:
            US_IDR_1 = CTSIC;                                            // disable interrupt on input change
        #if defined _WINDOWS
            US_IDR_1 = 0;
            US_IMR_1 &= ~CTSIC;
        #endif
            break;
        case 2:
            break;
        }
    }
    switch (channel) {
    case 0:
        if (!(US_CSR_0 & CTS_HIGH)) {
            ControlLines = SET_CTS;                                      // input is presently asserted
        }
        break;
    case 1:
        if (!(US_CSR_1 & CTS_HIGH)) {
            ControlLines = SET_CTS;                                      // input is presently asserted
        }
        break;
        #if defined DBGU_UART
    case 2:
        break;
        #endif
    }
    return ControlLines;
}
    #endif

#endif                                                                   // end serial UART support



/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */

#if defined SPI_FLASH_ENABLED                                            // {8}{18} SPI FLASH for SW upload

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_sam7x_atmel.h"                               // {41} - check the chip(s) availability
        #include "spi_flash_sam7x_stmicro.h"                             // {43}
        #include "spi_flash_sam7x_sst25.h"                               // {43}
    #undef _SPI_FLASH_INTERFACE


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


#if defined SPI_FLASH_MULTIPLE_CHIPS && !defined STORAGE_REV_1
// Calculate the device to access from the absolute address. It is assumed that all devices are operational.
//
static int fnGetSPI_Device(unsigned char **ptrFlash)                     // define the device to be accessed
{
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE)) {
        return 0;
    }
    #if defined QSPI_CS2_LINE
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;
    }
        #if defined QSPI_CS3_LINE
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

#if !defined SPI_FLASH_SST25 && !defined STORAGE_REV_1
// Return the page number and optionally the address offset in the page
//
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset, int *iChipSelect)
    #else
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
    #endif
{
    unsigned short usPageNumber;
    #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)           // {18}
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
    #else
    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address
    #endif
    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    #if SPI_FLASH_PAGE_LENGTH != 256                                     // B-device or D-device set to 264 page mode
    usPageNumber = 0;
    while (ptrSector >= (unsigned char *)(SPI_FLASH_PAGE_LENGTH * 100)) {// large hops to get close to the page if at high FLASH address
        usPageNumber += 100;
        ptrSector -= (SPI_FLASH_PAGE_LENGTH * 100);
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

#elif defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM              // {18}
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
    unsigned char usByteWrite[4];                                        // our file system is in external EEPROM via SPI bus.
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
#if defined _WINDOWS
        fnSimM95xxx(M95XXX_WRITE, (unsigned char)SPI_TDR, (unsigned char)((CS_PORT & CS_OUTPUT) != 0)); // simulate the SPI EEPROM device
        SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                              // simulate tx and rx interrupt flags being set
#endif
        while (!(SPI_SR & SPI_RDRF)) {};                                 // wait until tx byte has been sent and rx byte has been completely received
        ulDummy = SPI_RDR;                                               // reset receive data flag with dummy read - the rx data is not interesting here
    }

    if ((!ulCS_state || (PREPARE_READ != *(ptrData)))) {
        CS_PORT_SET = CS_OUTPUT;                        _SIM_PORT_CHANGE // negate SS on exit
    }
#if defined _WINDOWS
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
#if defined _WINDOWS
        SPI_RDR = fnSimM95xxx(M95XXX_READ, (unsigned char)SPI_TDR, (unsigned char)((CS_PORT & CS_OUTPUT) != 0));
        SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                              // simulate tx and rx interrupt flags being set
#endif
        while (!(SPI_SR & SPI_RDRF)) {};                                 // wait for the byte to be transmitted / received (check rx)
        *ptrBuffer++ = (unsigned char)SPI_RDR;                           // copy the received byte to the input buffer
#if defined _WINDOWS
        SPI_SR &= ~(SPI_RDRF);
#endif
    }

    CS_PORT_SET = CS_OUTPUT;;                          _SIM_PORT_CHANGE  // always deassert CS on exit

#if defined _WINDOWS
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
    ucEEPROMDanger = 0;                                                  // mark eeprom not writing so no danger reading data from it
 #if defined _WINDOWS
    fnEEPROM_safe();
 #endif
}
#endif                                                                   // endif SPI_FILE_SYSTEM

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
static void fnConfigSPIFileSystem(unsigned char ucSpeed)                 // {4}
{
    CONFIG_SPI_PORTS();                                                  // {40}
    POWER_SPI();                                                         // enable clocks to SPI in PMC

    SPI_MR = (SPI_MSTR | MODFDIS | SPI_CS_0);                            // master mode with no clock divider and control defined by CS0 configuration {27}

    switch (ucSpeed) {
    case MAX_SPI:                                                        // set maximum possible SPI speed
	    SPI_CSR0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 16000000/2)/16000000) << 8)); // 16MHz
	    break;

    case SPI_2MEG:
	    SPI_CSR0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 2000000/2)/2000000) << 8));
	    break;

    case SPI_SLAVE:
	    return;
    }
    SPI_CR = SPIEN;                                                      // enable SPI
}
#endif


#if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES

// This routine is used to command FLASH - we put it in SRAM
//
static void fnFlashFlash(unsigned long ulCommand)
{
    MC_FCR = ulCommand;                                                  // execute command
    while ((MC_FSR & FLASH_READY) == 0) {}                               // wait until FLASH ready again
}

// Warning: this is designed for little endian Thumb mode ONLY
// Cover routine for FLASH command execution
//
static void fnCommandFlash(unsigned long ulCommand)
{
    #define PROG_WORD_SIZE 100                                           // adequate space for the small program

    static unsigned short usProgSpace[PROG_WORD_SIZE];                   // make space for the routine on stack (this will have a word boundary)
    static void (*fnRAM_code)(unsigned long) = 0;

    if (fnRAM_code == 0) {                                               // real address of code in thumb mode
        unsigned char *ptrThumb = (unsigned char *)fnFlashFlash;
        int i = 0;

        ptrThumb--;                                                      // real address of thumb routine

        while (i < PROG_WORD_SIZE) {                                     // copy program to SRAM
            usProgSpace[i++] = *(unsigned short*)ptrThumb;
            ptrThumb += sizeof(unsigned short);
        }
        ptrThumb = (unsigned char *)usProgSpace;
        ptrThumb++;                                                      // create a thumb call
        fnRAM_code = (void(*)(unsigned long))(ptrThumb);
    #if defined _WINDOWS
        fnRAM_code = fnFlashFlash;
    #endif
    }

    while ((MC_FSR & FLASH_READY) == 0) {}                               // wait until FLASH ready for programming

    uDisable_Interrupt();                                                // protect this region from interrupts
        fnRAM_code(ulCommand);                                           // execute code from SRAM
    uEnable_Interrupt();                                                 // safe to accept interrupts again
}

    #if SIZE_OF_FLASH > (256 * 1024)                                     // {80}
// This routine is used to command FLASH - we put it in SRAM
//
static void fnFlashFlash1(unsigned long ulCommand)
{
    MC_FCR1 = ulCommand;                                                 // execute command
    while (!(MC_FSR1 & FLASH_READY)) {}                                  // wait until FLASH ready again
}

// Warning: this is designed for little endian Thumb mode ONLY
// Cover routine for FLASH command execution
//
static void fnCommandFlash1(unsigned long ulCommand)
{
    #define PROG_WORD_SIZE 100                                           // adequate space for the small program

    static unsigned short usProgSpace1[PROG_WORD_SIZE];                  // make space for the routine on stack (this will have a word boundary)
    static void (*fnRAM_code1)(unsigned long) = 0;

    if (!fnRAM_code1) {                                                  // real address of code in thumb mode
        unsigned char *ptrThumb1 = (unsigned char *)fnFlashFlash1;
        int i = 0;

        ptrThumb1--;                                                     // real address of thumb routine

        while (i < PROG_WORD_SIZE) {                                     // copy program to SRAM
            usProgSpace1[i++] = *(unsigned short*)ptrThumb1;
            ptrThumb1 += sizeof(unsigned short);
        }
        ptrThumb1 = (unsigned char *)usProgSpace1;
        ptrThumb1++;                                                     // create a thumb call
        fnRAM_code1 = (void(*)(unsigned long))(ptrThumb1);
    }

    while (!(MC_FSR1 & FLASH_READY)) {}                                  // wait until FLASH ready for programming

    uDisable_Interrupt();                                                // protect this region from interrupts

    fnRAM_code1(ulCommand);                                              // execute code from SRAM

    uEnable_Interrupt();                                                 // safe to accept interrupts again
}
    #endif
#endif


#if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES
    #if !defined STORAGE_REV_1                                           // {94}
    #if defined SPI_FLASH_SST25                                          // {43}
        #if defined SPI_FLASH_MULTIPLE_CHIPS
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
        #if defined SPI_FLASH_MULTIPLE_CHIPS                             // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
        #endif
    return ucDestination;
}
    #endif                                                               // end SST25
    #endif


#if defined STORAGE_REV_1                                                // {94}

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
        #if defined SST25_A_VERSION
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
        #if defined SPI_DATA_FLASH
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
    #endif

    #if !defined ONLY_INTERNAL_FLASH_STORAGE

// Search for the memory type that the starting address is in, return the type and restrict the largest length that can be read,written, erased from that location
//
extern unsigned char fnGetStorageType(unsigned char *memory_pointer, ACCESS_DETAILS *ptrAccessDetails)
{
    unsigned long ulMaximumLength;
    STORAGE_AREA_ENTRY *ptrStorageList = UserStorageListPtr;
    #if defined _WINDOWS                                                 // this is only used when reading with the simulator
    if (iFetchingInternalMemory != _ACCESS_NOT_IN_CODE) {                // if the access is to program memory when simulating
        iFetchingInternalMemory = _ACCESS_NOT_IN_CODE;                   // reset single-shot program access
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

static int fnDeleteInternalFlashPage(unsigned char *ptrSector)
{
    #ifndef _WINDOWS
    uMemset(ptrSector, 0xff, FLASH_GRANULARITY);                         // ensure page buffer is empty before erasing {6}
    #endif

    #if defined _WINDOWS
    MC_FSR |= FLASH_READY;
        #if SIZE_OF_FLASH > (256 * 1024)                                 // {80}
    MC_FSR1 |= FLASH_READY;
        #endif
    #endif

    #if SIZE_OF_FLASH > (256 * 1024)                                     // {80}
    if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (256 * 1024))) {
        MC_FMR1 &= ~NEBP;                                                // enable delete before write
        fnCommandFlash1(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - (FLASH_START_ADDRESS + (256 * 1024))));
        MC_FMR1 |= NEBP;                                                 // generally we want to be in this mode so that we can modify the contents of a page

        #if defined _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                   // simulate deletion
        #endif

        if (MC_FSR1 & (LOCKE | PROGE)) {
            return 1;                                                    // error
        }
    }
    else {
        MC_FMR &= ~NEBP;                                                 // enable delete before write
        fnCommandFlash(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS));
        MC_FMR |= NEBP;                                                  // generally we want to be in this mode so that we can modify the contents of a page

        #if defined _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                   // simulate deletion
        #endif

        if (MC_FSR & (LOCKE | PROGE)) {
            return 1;                                                    // error
        }
    }
    #else
    MC_FMR &= ~NEBP;                                                     // enable delete before write
    fnCommandFlash(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS));
    MC_FMR |= NEBP;                                                      // generally we want to be in this mode so that we can modify the contents of a page

        #if defined _WINDOWS
    fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                       // simulate deletion
        #endif

    if (MC_FSR & (LOCKE | PROGE)) {
        return 1;                                                        // error
    }
    #endif
    return 0;
}

// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
    #if defined MANAGED_FILES
    MAX_FILE_LENGTH OriginalLength = Length;
    #endif
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    ACCESS_DETAILS AccessDetails;
    do {
        AccessDetails.BlockLength = Length;
        switch (fnGetStorageType(ptrSector, &AccessDetails)) {           // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        case _STORAGE_INTERNAL_FLASH:
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // set to sector boundary            
            if (fnDeleteInternalFlashPage(ptrSector) != 0) {
                return -1;                                               // {95}
            }
            AccessDetails.BlockLength = FLASH_GRANULARITY;
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
        #if defined I2C_EEPROM_FILE_SYSTEM
        case _STORAGE_I2C_EEPROM:
            {
                CAST_POINTER_ARITHMETIC align_length = (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXTENSION_FILE_SIZE - 1)));
                Length += align_length;
                AccessDetails.BlockLength += align_length;
                ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXTENSION_FILE_SIZE - 1)); // set to sector boundary
                if (fnDeleteI2C_eeprom(&AccessDetails) < 0) {            // delete data in I2C EEPROM
                    return -1;                                           // {95}
                }
            }
            break;
        #endif
        #if defined EXT_FLASH_FILE_SYSTEM
        case _STORAGE_PARALLEL_FLASH:
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXT_FLASH_PAGE_LENGTH - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXT_FLASH_PAGE_LENGTH - 1)); // set to sector boundary
            AccessDetails.BlockLength = fnDeleteParallelFlash(&AccessDetails); // delete page/block in parallel flash 
            if (AccessDetails.BlockLength == 0) {
                return -1;                                               // error {95}
            }
            break;
        #endif
        default:
            return -1;                                                   // invalid {95}
        }
        ptrSector += AccessDetails.BlockLength;
        if (Length <= AccessDetails.BlockLength) {                       // if completed
        #if defined MANAGED_FILES
            if (OriginalLength == 0) {                                   // {95} if a single page erase was called, return the page size of the storage area
	            return AccessDetails.BlockLength;
	        }
        #endif
            break;
        }
        Length -= AccessDetails.BlockLength;
    } while (1);
    #else                                                                // case when only internal Flash is available
    Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)));
    ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // set to sector boundary
    do {
        if (fnDeleteInternalFlashPage(ptrSector) != 0) {
            return -1;
        }
        if (Length <= FLASH_GRANULARITY) {                               // check whether entire deletion has completed
        #if defined MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size of the storage area
	            return FLASH_GRANULARITY;
	        }
        #endif
            break;
        }
        ptrSector += FLASH_GRANULARITY;                                  // advance sector point to next internal flash sector
        Length -= FLASH_GRANULARITY;
    } while (1);
    #endif
    return 0;
}
#else
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length) // {9}
{
#if defined PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
#endif
#if defined MANAGED_FILES                                                // {82}
    MAX_FILE_LENGTH OriginalLength = Length;
#endif
    do {
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {18}
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
        #if defined SPI_FLASH_ENABLED                                    // {8}{18}
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)   // {18}
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)))
            #else
        if ((ptrSector >= uFILE_SYSTEM_END) && (ptrSector < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
            #endif
        {                                                                // we are working from external SPI FLASH memory
            #if defined SPI_FLASH_MULTIPLE_CHIPS                         // delete in SPI FLASH
            int iChipSelect;
            #endif
            #if !defined SPI_FLASH_ST                                    // {43}
            MAX_FILE_LENGTH BlockLength;
            unsigned char  ucCommand;
            #endif
            #if defined SPI_FLASH_SST25                                  // {43}
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
            #if defined SPI_FLASH_ST                                     // {43}
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // enable the write
                #if defined SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #elif defined SPI_FLASH_SST25                                // {43}
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
                #ifndef SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && (ulSectorAlign == (ulLocation & (SPI_FLASH_SECTOR_LENGTH - 1)))) { // {53} if a complete 64k sector can be deleted
                ucCommand = SECTOR_ERASE;                                // delete block of 64k
                BlockLength = SPI_FLASH_SECTOR_LENGTH;
            }
            else
                #endif
            if ((Length >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) { // {53}
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
                                                                         // delete in internal FLASH
        Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)));
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // set to sector boundary

        #ifndef _WINDOWS
        uMemset(ptrSector, 0xff, FLASH_GRANULARITY);                     // ensure page buffer is empty before erasing {6}
        #endif

        #if defined _WINDOWS
        MC_FSR |= FLASH_READY;
            #if SIZE_OF_FLASH > (256 * 1024)                             // {80}
        MC_FSR1 |= FLASH_READY;
            #endif
        #endif

        #if SIZE_OF_FLASH > (256 * 1024)                                 // {80}
        if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (256 * 1024))) {
            MC_FMR1 &= ~NEBP;                                            // enable delete before write
            fnCommandFlash1(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - (FLASH_START_ADDRESS + (256 * 1024))));
            MC_FMR1 |= NEBP;                                             // generally we want to be in this mode so that we can modify the contents of a page

            #if defined _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion
            #endif

            if (MC_FSR1 & (LOCKE | PROGE)) {
                return 1;                                                // error
            }
        }
        else {
            MC_FMR &= ~NEBP;                                             // enable delete before write
            fnCommandFlash(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS));
            MC_FMR |= NEBP;                                              // generally we want to be in this mode so that we can modify the contents of a page

            #if defined _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion
            #endif

            if (MC_FSR & (LOCKE | PROGE)) {
                return 1;                                                // error
            }
        }
        #else
        MC_FMR &= ~NEBP;                                                 // enable delete before write
        fnCommandFlash(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS));
        MC_FMR |= NEBP;                                                  // generally we want to be in this mode so that we can modify the contents of a page

            #if defined _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                   // simulate deletion
            #endif

        if (MC_FSR & (LOCKE | PROGE)) {
            return 1;                                                    // error
        }
        #endif

        ptrSector += FLASH_GRANULARITY;
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);

#if defined PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
#endif
#if defined MANAGED_FILES                                                // {82}
    if (OriginalLength == 0) {                                           // if a single page erase was called, return the page size
	    return FLASH_GRANULARITY;
	}
#endif
    return 0;
}
#endif


#if defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES
// Commit the present write buffer to FLASH
//
static int fnCommitFLASH(unsigned long ulPage)
{
    #if SIZE_OF_FLASH > (256 * 1024)                                     // {80}
    if (ulPage >= ((256 * 1024)/FLASH_GRANULARITY)) {                    // check whether in the second FLASH block
        #if defined _WINDOWS
        MC_FSR1 |= FLASH_READY;
        #endif
        ulPage -= ((256 * 1024)/FLASH_GRANULARITY);                      // page reference for the second FLASH controller
        fnCommandFlash1(FLASH_KEY | FCMD_WP | (ulPage << 8));
        if (MC_FSR1 & (LOCKE | PROGE)) {
            return 1;                                                    // FLASH error - quit
        }
        return 0;
    }
    #endif
    #if defined _WINDOWS
    MC_FSR |= FLASH_READY;
    #endif
    fnCommandFlash(FLASH_KEY | FCMD_WP | (ulPage << 8));
    if (MC_FSR & (LOCKE | PROGE)) {
        return 1;                                                        // FLASH error - quit
    }
    return 0;
}

extern void fnProtectFlash(unsigned char *ptrSector, unsigned char ucProtection) // {30}
{
    unsigned long ulPage = (((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS)/FLASH_GRANULARITY);
    #if defined _WINDOWS
    MC_FSR |= FLASH_READY;
    #endif
    if (ucProtection == PROTECT_SECTOR) {
        fnCommandFlash(FLASH_KEY | FCMD_SLB | (ulPage<<8));
    #if defined _WINDOWS
        MC_FSR &= ~(0x1 << ((ulPage/(LOCK_REGION_SIZE/FLASH_GRANULARITY)) + 16));
    #endif
    }
    else {                                                               // un-protect sector
        fnCommandFlash(FLASH_KEY | FCMD_CLB | (ulPage<<8));
    #if defined _WINDOWS
        MC_FSR &= ~(0x1 << ((ulPage/(LOCK_REGION_SIZE/FLASH_GRANULARITY)) + 16));
    #endif
    }
}
#endif


// Write a buffer to FLASH
//
#if defined STORAGE_REV_1                                                // {94}

static int fnWriteInternalFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ucData)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;


    unsigned long *ptrLong;
    unsigned long ulValue;
    int iByteInLong;
    unsigned long ulPage;
    unsigned long *ulPageEnd;

    // We  have to be sure that we only write 32 bits to long word addresses
    //
    ptrLong = (unsigned long *)fnGetFlashAdd((unsigned char *)(ptrAccessDetails->ulOffset & ~0x03));
    ulValue = *ptrLong;                                                  // present value at location
    iByteInLong = (ptrAccessDetails->ulOffset & 0x03);
    ulPage = (unsigned long)(ptrAccessDetails->ulOffset - FLASH_START_ADDRESS)/FLASH_GRANULARITY; // the page offset in internal flash
    ulPageEnd = (unsigned long *)(ptrAccessDetails->ulOffset & ~(FLASH_GRANULARITY - 1)) + (FLASH_GRANULARITY/sizeof(unsigned long)); // the address after the end of this page
    #ifndef _WINDOWS
    uMemset((ulPageEnd - (FLASH_GRANULARITY/sizeof(unsigned long))), 0xff, FLASH_GRANULARITY); // ensure flash buffer deleted on entry {7}
    #endif

    while (Length--) {
    #if defined (_LITTLE_ENDIAN) || defined (_WINDOWS)
        ulValue &= ~(0xff << (iByteInLong * 8));                         // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong * 8));                     // insert new byte
    #else
        ulValue &= ~(0xff << ((3 - iByteInLong) * 8));                   // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong) * 8));               // insert new byte
    #endif
        if (iByteInLong == 3) {                                          // can we write a long?
            if (!Length) {                                               // {7}
                break;
            }
            *ptrLong++ = ulValue;
            if ((unsigned long *)fnPutFlashAdd((unsigned char *)ptrLong) >= ulPageEnd) {
                if (fnCommitFLASH(ulPage++) != 0) {                      // we must commit the write buffer to flash
                    return 1;                                            // failed to write
                }
    #ifndef _WINDOWS
                uMemset(ulPageEnd, 0xff, FLASH_GRANULARITY);             // ensure flash buffer deleted ready for next block {7}
    #endif
                ulPageEnd += (FLASH_GRANULARITY/sizeof(unsigned long));  // set next sector for comparison
            }
            ulValue = *ptrLong;
            iByteInLong = 0;
        }
        else {
            iByteInLong++;
        }
    }
    *ptrLong++ = ulValue;
    return (fnCommitFLASH(ulPage));                                      // commit to FLASH after copying to write buffer
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
        #if defined I2C_EEPROM_FILE_SYSTEM
        case _STORAGE_I2C_EEPROM:
            fnWriteI2C_eeprom(&AccessDetails, ucData);
            break;
        #endif
        #if defined EXT_FLASH_FILE_SYSTEM
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
#else
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // {18} simulate the same using SPI EEPROM
    static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
    unsigned short usPageLength;
    while (Length) {
        usPageLength = ((unsigned short)((CAST_POINTER_ARITHMETIC)ucDestination)) & (EEPROM_PAGE_SIZE-1); // calculate the page length based on EEPROM page boundaries
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
#else
    unsigned long *ptrLong;
    unsigned long ulValue;
    int iByteInLong;
    unsigned long ulPage;
    unsigned long *ulPageEnd;

    #if defined SPI_FLASH_ENABLED                                        // {8}{18}
        #if defined SPI_FLASH_ENABLED                                    // {18}
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)))
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
        #endif
    {                                                                    // we are working from external SPI FLASH memory
    #if defined SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
    #endif
    #if defined SPI_FLASH_SST25                                          // {43}
        int iMultipleWrites = 0;
    #endif
        unsigned short usDataLength;
    #if defined SPI_FLASH_SST25                                          // {43}
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
        #if defined MANAGED_FILES                                        // {71}
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
    #if defined SPI_FLASH_ST                                             // {43}
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
    #elif defined SPI_FLASH_SST25                                        // {43}
        #if defined SST25_A_VERSION
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
    #if defined SPI_FLASH_SST25                                          // {43}
        fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);              // disable writes
    #endif
        return 0;
    }
    #endif
    // we have already checked that it is possible to modify the byte requested but we have to be sure that we only write 32 bits to long word addresses
    #if defined _WINDOWS
    ptrLong = (unsigned long*)((unsigned long)((unsigned long)ucDestination + (unsigned long)ucFLASH - FLASH_START_ADDRESS) & ~0x3);  // long address starting on
    #else
    ptrLong = (unsigned long*)((unsigned long)ucDestination & ~0x3);     // long address starting on
    #endif
    ulValue = *ptrLong;                                                  // present value at location
    #if defined _WINDOWS
    iByteInLong = (ucDestination - FLASH_START_ADDRESS + (unsigned long)ucFLASH) - (unsigned char *)ptrLong;
    #else
    iByteInLong = ucDestination - (unsigned char *)ptrLong;
    #endif
    ulPage = (unsigned long)(ucDestination - FLASH_START_ADDRESS)/FLASH_GRANULARITY;
    ulPageEnd = (unsigned long *)((unsigned long)ucDestination & ~(FLASH_GRANULARITY-1)) + (FLASH_GRANULARITY/ sizeof(unsigned long));
    #if defined _WINDOWS
    ulPageEnd += ((unsigned long)ucFLASH - FLASH_START_ADDRESS)/sizeof(unsigned long);
    #else
    uMemset((ulPageEnd - (FLASH_GRANULARITY/sizeof(unsigned long))), 0xff, FLASH_GRANULARITY); // Ensure flash buffer deleted on entry {7}
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
            if (!Length) {                                               // {7}
                break;
            }
            *ptrLong++ = ulValue;
            if (ptrLong >= ulPageEnd) {
                if (fnCommitFLASH(ulPage++) != 0) {                      // we must commit the write buffer to flash
                    return 1;                                            // failed to write
                }
    #ifndef _WINDOWS
                uMemset(ulPageEnd, 0xff, FLASH_GRANULARITY);             // ensure flash buffer deleted ready for next block {7}
    #endif
                ulPageEnd += FLASH_GRANULARITY/sizeof(unsigned long);    // set next sector for comparison
            }
            ulValue = *ptrLong;
            iByteInLong = 0;
        }
        else {
            iByteInLong++;
        }
    }
    *ptrLong++ = ulValue;
    return (fnCommitFLASH(ulPage));                                      // commit to FLASH after copying to write buffer
#endif
}
#endif
#endif


#if defined STORAGE_REV_1                                                // {94}
// Retrieve file contents from FLASH
//
extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
    #if defined _WINDOWS
    if (iFetchingInternalMemory != _ACCESS_NOT_IN_CODE) {
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
        #ifndef _WINDOWS                                                 // {96}
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
            #if defined I2C_EEPROM_FILE_SYSTEM
            case _STORAGE_I2C_EEPROM:
                fnReadI2C_eeprom(&AccessDetails, ptrValue);              // read the data from the external device
                break;
            #endif
            #if defined EXT_FLASH_FILE_SYSTEM
            case _STORAGE_PARALLEL_FLASH:
                uMemcpy(ptrValue, fnGetExtFlashAdd(ParLocation), AccessDetails.BlockLength); // read from external parallel FLASH with no access restrictions so we can simply copy the data
                break;
            #endif
            #if defined _WINDOWS                                         // only when simulating
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
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the SAM7x uses a file system in FLASH with no access restrictions so we can simply copy the data
    #endif
#endif
}
#else
extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#if defined ACTIVE_FILE_SYSTEM
    #if defined _WINDOWS
    if (iFetchingInternalMemory != 0) {                                  // {16}
        uMemcpy(ptrValue, ParLocation, Size);
      //iFetchingInternalMemory = 0;                                     // {24} the user must do this
        return;
    }
    #endif
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {18}
    unsigned char usRead[4];                                             // our file system is in external EEPROM via SPI bus.
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
        #if defined SPI_FLASH_ENABLED                                    // {8}{18}
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)   // {18}
    if (ParLocation >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
            #else
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
            #endif
    {                                                                    // we are working from external SPI FLASH memory
            #if defined SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
            #endif
            #if defined SPI_FLASH_SST25                                  // {43}
        ParLocation = fnGetSPI_FLASH_address(ParLocation EXTENDED_CS);
            #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
            #endif
            #if defined SPI_FLASH_ST                                     // {43}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #elif defined SPI_FLASH_SST25                                // {43}
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
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the SAM7x uses a file system in FLASH with no access restrictions so we can simply copy the data
    #endif
#endif
}
#endif



#if defined USE_PARAMETER_BLOCK

    #define PARAMETER_BLOCK_1 (unsigned char*)(PARAMETER_BLOCK_START)    // {42}
    #define PARAMETER_BLOCK_2 (PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)


// The SAM7X has 256 byte blocks which can be individually modified so we use one or more of these, and a backup if desired
//
extern unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned char ucValidUse[2] = {0};                                   // initialised only to quieten compiler
    unsigned char ucCompare;

    fnGetParsFile(PARAMETER_BLOCK_1, ucValidUse, sizeof(ucValidUse));    // the first 2 byte locations define the validity of the block. 0x55 0x55 means it is fully valid. 0x55 0xff means it contains temporary unvalidated data

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ucCompare = 0x55;                                                // we are looking for validated pattern
    }
    else {
        ucCompare = 0xff;                                                // we are looking for temporary pattern
    }

    if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
        return 1;                                                        // first block matched request
    }
    #if defined USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile(PARAMETER_BLOCK_2, (unsigned char*)ucValidUse, sizeof(ucValidUse));
        if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
            return 2;                                                    // second block matched request
        }
    }
    #endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}

// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(unsigned char *ptrAddInPar)                  // {9}
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}


extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1;

    #if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    #endif

    ptrPar += (usParameterReference);
    ptrPar += 2;                                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile(ptrPar, ucValue++, 1);
        ptrPar++;
    }
    return 0;
}

// Write parameters to the defined block
//
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
    #if defined USE_PAR_SWAP_BLOCK
        fnDeleteParBlock(PARAMETER_BLOCK_2);
    #endif
        ucValidBlock = 1;
    }

    #if defined USE_PAR_SWAP_BLOCK
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
    #if defined USE_PAR_SWAP_BLOCK
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
            if (usParameterReference != 0) {                             // we don't validate it again
                return 0;
            }
        }
    }
    else {
        fnWriteBytesFlash(ptrPar, ucValue, usLength);                    // the SAM7X can do accumulative writes so we can simply call the normal write function
    }
                                                                         // now we must validate the parameter block which we have just written to
    fnWriteBytesFlash(ptrStart, &ucValid, sizeof(ucValid));              // validate data
    if (!iBlockUse) {
        fnWriteBytesFlash(ptrStart+sizeof(ucValid), &ucValid, sizeof(ucValid)); // use data
    }
    return 0;
}
#endif                                                                   // endif USE_PARAMETER_BLOCK



#if defined IIC_INTERFACE
static __interrupt void TWI_Interrupt(void)
{
    volatile unsigned long ulStatus = TWI_SR;                            // clear the interrupt flag

    if (IIC_tx_control[0]->ucState & RX_ACTIVE) {
        if (IIC_tx_control[0]->ucPresentLen) {
            IIC_tx_control[0]->ucPresentLen--;
            if (IIC_tx_control[0]->ucPresentLen == 0) {
                TWI_CR = (TWI_MSEN | TWI_STOP);                          // send stop condition so that last byte is not acknowledged
            }
            *IIC_rx_control[0]->IIC_queue.put++ = (unsigned char)TWI_RHR;// read the byte
            IIC_rx_control[0]->IIC_queue.chars++;                        // and put it into the rx buffer
            if (IIC_rx_control[0]->IIC_queue.put >= IIC_rx_control[0]->IIC_queue.buffer_end) {
                IIC_rx_control[0]->IIC_queue.put = IIC_rx_control[0]->IIC_queue.QUEbuffer;
            }
    #if defined _WINDOWS
            if (IIC_tx_control[0]->ucPresentLen != 0) {
                TWI_RHR = fnSimIIC_devices(IIC_RX_DATA, (unsigned char)TWI_RHR);// simulate the interrupt directly
            }
            TWI_SR |= TWI_RXRDY;
            iInts |= IIC_INT0;
    #endif
        }
        else {
            ulStatus = TWI_RHR;                                          // dummy read after stop condition
            TWI_IDR = (TWI_TXRDY | TWI_RXRDY);                           // disable interrupts
    #if defined _WINDOWS
            TWI_IMR &= ~TWI_IDR; TWI_IDR = 0;                            // simulate the interrupt mask setting
    #endif
            IIC_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            IIC_rx_control[0]->msgs++;
            if ( IIC_rx_control[0]->wake_task ) {                        // wake up the receiver task if desired
               uTaskerStateChange(IIC_rx_control[0]->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        return;
    }

    if (IIC_tx_control[0]->ucPresentLen--) {                             //  send next byte if available
        TWI_THR = *IIC_tx_control[0]->IIC_queue.get++;
        if (IIC_tx_control[0]->IIC_queue.get >= IIC_tx_control[0]->IIC_queue.buffer_end) {
            IIC_tx_control[0]->IIC_queue.get = IIC_tx_control[0]->IIC_queue.QUEbuffer; // handle the ring buffer
        }
    #if defined _WINDOWS
        TWI_SR |= TWI_TXRDY;                                             // simulate the interrupt directly
        fnSimIIC_devices(IIC_TX_DATA, (unsigned char)TWI_THR);
        iInts |= IIC_INT0;
    #endif
    }
    else {
        TWI_IDR = (TWI_TXRDY | TWI_RXRDY);                               // disable interrupts
    #if defined _WINDOWS
        TWI_IMR &= ~TWI_IDR; TWI_IDR = 0;                                // simulate the interrupt mask setting
    #endif
        if (!(TWI_IMR & TWI_TXCOMP)) {
            TWI_IER = (TWI_TXCOMP);                                      // enable complete interrupt
    #if defined _WINDOWS
            TWI_IMR |= TWI_IER; TWI_IER = 0;                             // simulate the interrupt mask setting
            TWI_SR = TWI_TXCOMP;
            iInts |= IIC_INT0;
    #endif
            IIC_tx_control[0]->ucPresentLen++;
            TWI_CR = (TWI_MSEN | TWI_STOP);                              // send stop condition
        }
        else {                                                           // waiting for stop to terminate
            TWI_IDR = (TWI_TXCOMP);                                      // disable interrupts
    #if defined _WINDOWS
            TWI_IMR &= ~TWI_IDR; TWI_IDR = 0;                            // simulate the interrupt mask setting
    #endif
            if (!(IIC_tx_control[0]->IIC_queue.chars)) {                 // all transmission complete
                IIC_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);

                if ( IIC_tx_control[0]->wake_task ) {
                   uTaskerStateChange( IIC_tx_control[0]->wake_task, UTASKER_ACTIVATE);// wake up owner task since the transmission has terminated
                }
            }
            else {
                fnTxIIC(IIC_tx_control[0], 0);                           // we have another message to send so we can send a repeated start condition
            }
        }
    }
}

// Configure the IIC hardware (SAM7X supports only MASTER mode)
//
extern void fnConfigIIC(IICTABLE *pars)
{
    TWI_CR = (TWI_SWRST);                                                // reset IIC module

    POWER_UP(TWI);                                                       // {37} enable clocks to TWI
    PIO_MDER_A  = (TWD | TWCK);                                          // configure as open drain
    PIO_ASR_A   = (TWD | TWCK);                                          // configure the pin for peripheral use
    PIO_PDR_A   = (TWD | TWCK);

    fnEnterInterrupt(TWI, PRIORITY_TWI, TWI_Interrupt);

    if (pars->usSpeed >= 400) {                                          // maximum speed 400kHz
        #define TWI_SPEED_400 400000
        #define TWI_CKDIV_400 0
        TWI_CWGR = ((((MASTER_CLOCK/TWI_SPEED_400/2) - 3)/(1<<TWI_CKDIV_400)) | ((((MASTER_CLOCK/TWI_SPEED_400/2) - 3)/(1<<TWI_CKDIV_400))<<8) | (TWI_CKDIV_400<<16)); // 400kHz
    }
    else if (pars->usSpeed < 100) {                                      // 50kHz
        #define TWI_SPEED_50 50000
        #define TWI_CKDIV_50 3
        TWI_CWGR = ((((MASTER_CLOCK/TWI_SPEED_50/2) - 3)/(1<<TWI_CKDIV_50)) | ((((MASTER_CLOCK/TWI_SPEED_50/2) - 3)/(1<<TWI_CKDIV_50))<<8) | (TWI_CKDIV_50<<16)); // 50kHz
    }
    else {                                                               // 100kHz
        #define TWI_SPEED_100 100000
        #define TWI_CKDIV_100 2
        TWI_CWGR = ((((MASTER_CLOCK/TWI_SPEED_100/2) - 3)/(1<<TWI_CKDIV_100)) | ((((MASTER_CLOCK/TWI_SPEED_100/2) - 3)/(1<<TWI_CKDIV_100))<<8) | (TWI_CKDIV_100<<16)); // 100kHz
    }

    TWI_CR = (TWI_MSEN);                                                 // enable master mode
    #if defined _WINDOWS
    fnConfigSimIIC(pars->Channel, (pars->usSpeed * 1000));               // {17}
    #endif
}

// Send a first byte to IIC bus
//
extern void fnTxIIC(IICQue *ptIICQue, QUEUE_HANDLE Channel)              // {34}
{
    unsigned long ulAddress;

    ptIICQue->ucPresentLen = *ptIICQue->IIC_queue.get++;                 // get present length
    if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
        ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;         // handle circular buffer
    }

    ulAddress = *ptIICQue->IIC_queue.get++;
    ulAddress <<= 15;                                                    // move device address into position
    if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
        ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;         // handle circular buffer
    }

    if (ulAddress & 0x0008000) {                                         // reading
        TWI_MMR = ((BYTE_COMMAND_PROT | TWI_MREAD | ulAddress) & ~0x0008000);
        TWI_IER = (TWI_RXRDY);                                           // enable interrupt on ready
        TWI_CR = (TWI_MSEN | TWI_START);                                 // start transfer
    #if defined _WINDOWS
        TWI_IMR |= TWI_IER; TWI_IER = 0;                                 // simulate the interrupt mask setting
        fnSimIIC_devices(IIC_ADDRESS, (unsigned char)((TWI_MMR | 0x0008000)>>15));
        iInts |= IIC_INT0;
    #endif
        IIC_tx_control[0]->ucState |= (RX_ACTIVE | TX_ACTIVE);
        ptIICQue->IIC_queue.chars -= 3;
        IIC_rx_control[0]->wake_task = *ptIICQue->IIC_queue.get++;       // enter task to be woken when reception has completed
        if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
            ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;     // handle circular buffer
        }
    #if defined _WINDOWS
        TWI_SR |= TWI_RXRDY;                                             // simulate the interrupt directly
        TWI_RHR = fnSimIIC_devices(IIC_RX_DATA, (unsigned char)TWI_RHR);
        iInts |= IIC_INT0;
    #endif
    }
    else {
        TWI_MMR = (BYTE_COMMAND_PROT | ulAddress);
        TWI_THR = *ptIICQue->IIC_queue.get++;                            // send first data byte
        TWI_CR = (TWI_MSEN | TWI_START);                                 // start transfer
        TWI_IER = (TWI_TXRDY);                                           // enable interrupt on ready
    #if defined _WINDOWS
        TWI_IMR |= TWI_IER; TWI_IER = 0;                                 // simulate the interrupt mask setting
        fnSimIIC_devices(IIC_ADDRESS, (unsigned char)(TWI_MMR>>15));
        iInts |= IIC_INT0;
    #endif
        if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
            ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;     // handle circular buffer
        }
        IIC_tx_control[0]->ucState |= (TX_ACTIVE);                       // writing
        ptIICQue->IIC_queue.chars -= (ptIICQue->ucPresentLen+1);
        ptIICQue->ucPresentLen--;
    #if defined _WINDOWS
        TWI_SR |= TWI_TXRDY;                                             // simulate the interrupt directly
        fnSimIIC_devices(IIC_TX_DATA, (unsigned char)TWI_THR);
        iInts |= IIC_INT0;
    #endif
    }
}
#endif                                                                   // endif IIC_INTERFACE



#if defined USB_INTERFACE                                                // {36}

extern unsigned long fnUSB_error_counters(int iValue)                    // {103}
{
    switch (iValue) {
    case USB_CRC_5_ERROR_COUNTER:
        return USB_errors.ulUSB_errors_CRC5;
    case USB_CRC_16_ERROR_COUNTER:
        return USB_errors.ulUSB_errors_CRC16;
    case USB_ERRORS_RESET:
        uMemset(&USB_errors, 0, sizeof(USB_errors));
        break;
    }
    return 0;
}

// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *ptrUSB_HW, unsigned char ucFrameType)
{
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting messages to queue
    switch (fnUSB_handle_frame(ucFrameType, (unsigned char *)ptrUSB_HW->ptrDatBuffer, iEndpoint_ref, ptrUSB_HW)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        SYNCHRONISE_CONTROL_CLEAR(ptrUSB_HW->ulRxControl);               // {101} free receive buffer just handled so that it can accept further data (before sending the zero data termination)
        FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref);                      // send a zero data frame to confirm data termination
        iInterruptLevel = 0;                                             // {101}
        return 0;                                                        // {101}
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        iInterruptLevel = 0;
        return MAINTAIN_OWNERSHIP;
    case STALL_ENDPOINT:                                                 // send stall
        SYNCHRONISE_CONTROL_CLEAR(RXSETUP);
        if (iEndpoint_ref == 0) {                                        // {78} check whether control 0 endpoint
            SYNCHRONISE_CONTROL_SET(FORCESTALL);                         // stall the control endpoint
        }
        else {
            unsigned long *ulOutControl = (unsigned long *)ptrUSB_HW->ptr_ulUSB_BDControl;
            iEndpoint_ref = fnGetPairedIN(iEndpoint_ref);                // get the paired IN endpoint reference
            ptrUSB_HW->ptr_ulUSB_BDControl = UDP_CSR0_ADD;
            ptrUSB_HW->ptr_ulUSB_BDControl += iEndpoint_ref;
            SYNCHRONISE_CONTROL_SET(FORCESTALL);                         // stall/halt the corresponding IN endpoint
            ptrUSB_HW->ptr_ulUSB_BDControl = ulOutControl;               // return the OUT control pointer so that the receiver can be cleared
        }
        fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);      // mark that the endpoint is stalled
        _SIM_USB(USB_SIM_STALL, iEndpoint_ref, ptrUSB_HW);
        break;
    case CRITICAL_OUT:
        UDP_IDR = (0x1 << iEndpoint_ref);                                // the input cannot accept further data at the moment so disable further interrupts on the endpoint
        break;
    }
    if (USB_SETUP_FRAME != ucFrameType) {                                // {101} since the setup was cleared during processing we don't clear the interrupt and don't free the buffer
        #if defined RESET_USB_EP_INT_LATER                               // clear the endpoint's interrupt
        UDP_ICR = (0x1 << iEndpoint_ref);                                // reset interrupt flag
            #if defined _WINDOWS
        UDP_ISR &= ~(0x1 << iEndpoint_ref);
            #endif
        #endif
        SYNCHRONISE_CONTROL_CLEAR(ptrUSB_HW->ulRxControl);               // free receive buffer just handled so that it can accept further data
    }
    iInterruptLevel = 0;
    return 0;
}




// USB Interrupt handler
//
static __interrupt void _usb_isr(void)
{
    static USB_HW usb_hardware = {0};
    unsigned long ulInterrupts;
    while ((ulInterrupts = (UDP_ISR & UDP_IMR)) != 0) {                  // while valid UDP interrupts waiting
        if (ulInterrupts & ENDBUSRES) {                                  // USB bus reset detected
            UDP_FADDR = FEN;                                             // default address 0
            UDP_CSR0 = 0;                                                // disable all endpoints
            UDP_CSR1 = 0;
            UDP_CSR2 = 0;
            UDP_CSR3 = 0;
            UDP_CSR4 = 0;
            UDP_CSR5 = 0;
    #if defined _HW_SAM7SE                                               // {83}
            UDP_CSR6 = 0;
            UDP_CSR7 = 0;
            UDP_RST_EP = (EP0RST | EP1RST | EP2RST | EP3RST | EP4RST | EP5RST | EP6RST | EP7RST); // reset all FIFOs
    #else
            UDP_RST_EP = (EP0RST | EP1RST | EP2RST | EP3RST | EP4RST | EP5RST); // reset all FIFOs
    #endif
            UDP_GBL_STAT = 0;                                            // not in the addresses state
            UDP_ICR = (BUS_WAKEUP_CLR | ENDBUSRES_CLR | SOFINT_CLR | RXRSM_CLR | RXSUSP_CLR); // clear all interrupt flags
            UDP_IER = (EP0INT);                                          // enable interrupt for endpoint 0 (reset interrupt mask is always set)
    #if defined _WINDOWS
            UDP_IMR = (UDP_IER | ENDBUSRES);
            UDP_ISR = 0;
    #endif
            iInterruptLevel = 1;                                         // {89} ensure interrupts remain blocked when putting message to queue
            fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware); // generic handler routine
            iInterruptLevel = 0;
            usb_hardware.ucUSBAddress = 0;                               // {81} reset the address to revert back to the default state
            UDP_RST_EP = 0;                                              // allow FIFOs to operate again
            UDP_CSR0 = (EPEDS | EPTYPE_CONTROL);                         // enable endpoint 0 as control endpoint
        }
    #if defined _HW_SAM7SE                                               // {83}
        else if (ulInterrupts & (EP0INT | EP1INT | EP2INT | EP3INT | EP4INT | EP5INT | EP6INT | EP7INT))
    #else
        else if (ulInterrupts & (EP0INT | EP1INT | EP2INT | EP3INT | EP4INT | EP5INT))
    #endif
        {                                                                // an endpoint interrupt
            int iEndpoint_ref = 0;
            usb_hardware.ulRxControl = RX_DATA_BK0;                      // some endpoints always receive in a single buffer
            if (ulInterrupts & EP0INT) {                                 // endpoint 0
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR0_ADD, (UDP_CSR0 & TXCOMP)); // set the data pointer to the fifo that the reception is to be read from
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR0_ADD;
    #ifndef RESET_USB_EP_INT_LATER                                       // don't reset until it is known that it will be handled
                UDP_ICR = EP0INT;                                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~EP0INT;
        #endif
    #endif
            }
            else if (ulInterrupts & EP1INT) {                            // endpoint 1 (dual buffered)
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR1_ADD, (UDP_CSR1 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR1_ADD;
                iEndpoint_ref = 1;
    #ifndef RESET_USB_EP_INT_LATER                                       // don't reset until it is known that it will be handled
                UDP_ICR = EP1INT;                                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~EP1INT;
        #endif
    #endif
            }
            else if (ulInterrupts & EP2INT) {                            // endpoint 2
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR2_ADD, (UDP_CSR2 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR2_ADD;
                iEndpoint_ref = 2;
    #ifndef RESET_USB_EP_INT_LATER                                       // don't reset until it is known that it will be handled
                UDP_ICR = EP2INT;                                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~EP2INT;
        #endif
    #endif
            }
            else if (ulInterrupts & EP3INT) {                            // endpoint 3
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR3_ADD, (UDP_CSR3 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR3_ADD;
                iEndpoint_ref = 3;
    #ifndef RESET_USB_EP_INT_LATER                                       // don't reset until it is known that it will be handled
                UDP_ICR = EP3INT;                                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~EP3INT;
        #endif
    #endif
            }
            else if (ulInterrupts & EP4INT) {                            // endpoint 4
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR4_ADD, (UDP_CSR4 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR4_ADD;
                iEndpoint_ref = 4;
    #ifndef RESET_USB_EP_INT_LATER                                       // don't reset until it is known that it will be handled
                UDP_ICR = EP4INT;                                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~EP4INT;
        #endif
    #endif
            }
            else if (ulInterrupts & EP5INT) {                            // endpoint 5
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR5_ADD, (UDP_CSR5 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR5_ADD;
                iEndpoint_ref = 5;
    #ifndef RESET_USB_EP_INT_LATER                                       // don't reset until it is known that it will be handled
                UDP_ICR = EP5INT;                                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~EP5INT;
        #endif
    #endif
            }
    #if defined _HW_SAM7SE                                               // {83}
            else if (ulInterrupts & EP6INT) {                            // endpoint 6
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR6_ADD, (UDP_CSR6 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR6_ADD;
                iEndpoint_ref = 6;
        #ifndef RESET_USB_EP_INT_LATER                                   // don't reset until it is known that it will be handled
                UDP_ICR = EP6INT;                                        // reset interrupt flag
            #if defined _WINDOWS
                UDP_ISR &= ~EP6INT;
            #endif
        #endif
            }
            else if (ulInterrupts & EP7INT) {                            // endpoint 7
                usb_hardware.ptrDatBuffer = fnGetUSB_FIFOAdd(UDP_FDR7_ADD, (UDP_CSR7 & TXCOMP));
                usb_hardware.ptr_ulUSB_BDControl = UDP_CSR7_ADD;
                iEndpoint_ref = 7;
        #ifndef RESET_USB_EP_INT_LATER                                   // don't reset until it is known that it will be handled
                UDP_ICR = EP7INT;                                        // reset interrupt flag
            #if defined _WINDOWS
                UDP_ISR &= ~EP7INT;
            #endif
        #endif
            }
    #endif

            if (*usb_hardware.ptr_ulUSB_BDControl & TXCOMP) {            // IN transfer has been successfully acknowledged
                USB_END_POINT *endpoint = (usb_endpoints + iEndpoint_ref);
                if (endpoint->ucEndpointTxFlags == (ENDPOINT_DUAL_BANK | ENDPOINT_SECOND_BANK_BUSY)) { // a second buffer is queued so start it now
                    endpoint->ucEndpointTxFlags = ENDPOINT_DUAL_BANK;
                    SYNCHRONISE_CONTROL_SET_(TXPKTRDY);                  // start transmission of second buffer, which has already been queued
                    _SIM_USB(USB_SIM_TX, iEndpoint_ref, &usb_hardware);
                }
    #if defined RESET_USB_EP_INT_LATER                                   // clear the endpoint's interrupt
                UDP_ICR = (0x1 << iEndpoint_ref);                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~(0x1 << iEndpoint_ref);
        #endif
    #endif
                SYNCHRONISE_CONTROL_CLEAR_(TXCOMP);                      // clear the flag and interrupt
                iInterruptLevel = 1;                                     // ensure interrupts remain blocked when putting message to queue
                fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpoint_ref, &usb_hardware); // handle ack event
                iInterruptLevel = 0;
                UDP_FADDR = (usb_hardware.ucUSBAddress | FEN);           // program the address to be used
            }
            else if (*usb_hardware.ptr_ulUSB_BDControl & STALLSENT) {    // a transmitted stall has been acknowledged by the host
    #if defined RESET_USB_EP_INT_LATER                                   // clear the endpoint's interrupt
                UDP_ICR = (0x1 << iEndpoint_ref);                        // reset interrupt flag
        #if defined _WINDOWS
                UDP_ISR &= ~(0x1 << iEndpoint_ref);
        #endif
    #endif
                if (ulInterrupts & EP0INT) {                             // {79} only automatically unstall control endpoint 0
                    SYNCHRONISE_CONTROL_CLEAR_(STALLSENT | FORCESTALL);
                }
            }
            else {                                                       // SETUP or OUT
                USB_END_POINT *endpoint = (usb_endpoints + iEndpoint_ref);
                if (*usb_hardware.ptr_ulUSB_BDControl & RXSETUP) {
                    endpoint->ucEndpointRxFlags |= ENDPOINT_RX_DATA1;    // DATA1 OUT expected after a setup                   
                    fnProcessInput(iEndpoint_ref, &usb_hardware, USB_SETUP_FRAME);
                }
                else {                                                   // OUT
                    unsigned char ucOriginalState = endpoint->ucEndpointRxFlags;
    #if defined _SUPRESS_REPEAT_DATA
                    unsigned char ucDataCheck = (ucOriginalState & ENDPOINT_RX_DATA1);
    #endif
                    usb_hardware.usLength = (unsigned short)(*usb_hardware.ptr_ulUSB_BDControl >> RXBYTECNT_SHIFT); // the received data length
                    if (ucOriginalState & ENDPOINT_DUAL_BANK) {          // additional handling for dual buffer receivers
                        if (*usb_hardware.ptr_ulUSB_BDControl & RX_DATA_BK1) { // reception in bank 1
                            if (*usb_hardware.ptr_ulUSB_BDControl & RX_DATA_BK0) { // reception also waiting in bank 0 (two buffers)
    #if defined _SUPRESS_REPEAT_DATA
                                ucDataCheck ^= ENDPOINT_RX_DATA1;        // invert DATA flag when both buffers are occupied
    #endif
                                if (ucOriginalState & ENDPOINT_RX_BANK1) { // bank1 was handled last time around so handle bank 0
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
    #if defined _SUPRESS_REPEAT_DATA                                     // check the validity of the data toggle flag
                    if (((*usb_hardware.ptr_ulUSB_BDControl & DTGLE) == 0) == (ucDataCheck == 0)) {
        #if defined RESET_USB_EP_INT_LATER                               // clear the endpoint's interrupt
                UDP_ICR = (0x1 << iEndpoint_ref);                        // reset interrupt flag
            #if defined _WINDOWS
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
                    }
                }
            }
        }
        else if (ulInterrupts & RXSUSP) {                                // suspend interrupt
            iInterruptLevel = 1;                                         // {89} ensure interrupts remain blocked when putting message to queue
            fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);           // generic handler routine
            iInterruptLevel = 0;
            UDP_IDR = RXSUSP;                                            // disable suspend and enable resume interrupt
            UDP_ICR = RXSUSP_CLR;
            UDP_IER = RXRSM;
    #if defined _WINDOWS
            UDP_IMR &= ~RXSUSP;
            UDP_IMR |= RXRSM;
            UDP_ISR &= ~RXRSM;
    #endif
        }
        else if (ulInterrupts & RXRSM) {                                 // resume interrupt
            iInterruptLevel = 1;                                         // {89} ensure interrupts remain blocked when putting message to queue
            fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);            // generic handler routine
            iInterruptLevel = 0;                                         // ensure interrupts remain blocked when putting message to queue
            UDP_IDR = RXRSM;                                             // disable resume and enable suspend interrupt
            UDP_ICR = RXRSM_CLR;
            UDP_IER = RXSUSP;
    #if defined _WINDOWS
            UDP_IMR &= ~RXRSM;
            UDP_IMR |= RXSUSP;
            UDP_ISR &= ~RXRSM;
    #endif
        }
    }
}

    #if defined _WINDOWS                                                 // USB simulation code
static volatile unsigned char *_SIM_SET_TX_PUT(int iEndpoint, volatile unsigned char *ptrBuffer)
{
    USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
    if (endpoint->ucEndpointTxFlags & ENDPOINT_DUAL_BANK) {
        if (endpoint->ulSimFlags & PUT_BANK_1) {
            unsigned short usBufferSize = 0;
            switch (iEndpoint) {
            case 0:
            case 3:
                usBufferSize = 8;
                break;
            case 1:
            case 2:
                usBufferSize = 64;
                break;
        #if defined _HW_SAM7SE                                           // {83}
            case 4:
            case 5:
            case 6:
            case 7:
                usBufferSize = 64;
                break;
        #else
            case 4:
            case 5:
                usBufferSize = 256;
                break;
        #endif
            }
            ptrBuffer += (usBufferSize + 2);                             // modify the pointer to the one to be copied to ({84} add 2 extra bytes since the simulator adds two extra bytes to save the transmission length)
        }
    }
    return ptrBuffer;
}

extern int iGetGetBank(int iEndpoint)
{
    USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
    if (endpoint->ucEndpointTxFlags & ENDPOINT_DUAL_BANK) {
        if (endpoint->ulSimFlags & GET_BANK_1) {
            endpoint->ulSimFlags ^= GET_BANK_1;
            return 1;
        }
        else {
            endpoint->ulSimFlags ^= GET_BANK_1;
            return 0;
        }
    }
    return 0;
}

// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    USB_END_POINT *endpoint = (usb_endpoints + iEndpoint);
    return endpoint->usEndpointSize;
}
    #endif                                                               // endif _WINDOWS

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW *ptr_usb_hardware)
{
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
    return ENDPOINT_FREE;
}


// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength)
{
    USB_END_POINT *endpoint = (usb_endpoints + ucEndpointRef);
    unsigned long  endpoint_config = (EPTYPE_CONTROL | EPEDS);
    unsigned long ulEndpointBit = (EP0INT << ucEndpointRef);

    if (ENDPOINT_DISABLE == ucEndpointType) {
        UDP_IDR = (RXSUSP | RXRSM | ulEndpointBit);                      // disable suspend and resume interrupts when not configured
        *(UDP_CSR0_ADD + ucEndpointRef) = 0;                             // disable the endpoint
        UDP_IDR = (ulEndpointBit);                                       // disable interrupt for endpoint
    #if defined _WINDOWS
        UDP_IMR &= ~(RXSUSP | RXRSM | ulEndpointBit);
    #endif
        return;
    }
    UDP_IER = RXSUSP;                                                    // since we have just been configured, enable suspend interrupt
    #if defined _WINDOWS
    UDP_IMR |= RXSUSP;
    #endif
    switch (ucEndpointType & ~IN_ENDPOINT) {
    case ENDPOINT_ISOCHRONOUS:
        endpoint_config = (EPTYPE_ISO_OUT | EPEDS);
        break;
    case ENDPOINT_BULK:
        endpoint_config = (EPTYPE_BULK_OUT | EPEDS);
        break;
    case ENDPOINT_INTERRUPT:
        endpoint_config = (EPTYPE_INTERRUPT_OUT | EPEDS);
        break;
    default:                                                             // control endpoint
        break;
    }
    if ((endpoint_config != EPTYPE_CONTROL) && (ucEndpointType & IN_ENDPOINT)) { // if IN type
        endpoint_config |= EPTYPE_IN;                                    // change the OUT mode to IN mode
    }
    endpoint->usEndpointSize = usEndpointLength;                         // enter the endpoint length for later reference
    if ((ucEndpointRef != 0) && (ucEndpointRef != 3)) {                  // not endpoints with no dual bank
        endpoint->ucEndpointRxFlags = ENDPOINT_DUAL_BANK;                // flag that dual bank is in use on this endpoint
        endpoint->ucEndpointTxFlags = ENDPOINT_DUAL_BANK;
    }
    else {
        endpoint->ucEndpointRxFlags = 0;                                 // reset flags
        endpoint->ucEndpointTxFlags = 0;
    }
    *(UDP_CSR0_ADD + ucEndpointRef) = endpoint_config;                   // configure the endpoint mode
    UDP_IER = (ulEndpointBit);                                           // enable interrupt for endpoint
    #if defined _WINDOWS
    UDP_IMR |= (ulEndpointBit);
    endpoint->ulSimFlags = 0;
    #endif
    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simulator that USB enumeration has completed
}

// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine {79}
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    volatile unsigned long *ulEndpointCommand;
    ucEndpoint &= ~IN_ENDPOINT;
    ulEndpointCommand = UDP_CSR0_ADD + ucEndpoint;
    while (*ulEndpointCommand & (STALLSENT | FORCESTALL)) {
        *ulEndpointCommand &= ~(STALLSENT | FORCESTALL);
    }
    UDP_RST_EP |= (1 << ucEndpoint);                                     // {87} reset endpoint
    UDP_RST_EP &= ~(1 << ucEndpoint);                                    // allow endpoint to operate again
}


// This routine is called from the USB driver with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    UDP_IER = (0x1 << ucEndpointRef);                                    // enable the interrupt handling again, which will free up the buffer
    return USB_BUFFER_NO_DATA;                                           // report no more data so that the driver doesn't call again. The interrupt routine will handle all waiting buffers itself.
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
    CKGR_PLLR |= UDP_DIV_2;                                              // divide 95.8464 MHz PLL output to 48MHz
    PMC_SCER = UDP_ENABLE;                                               // enable the 48MHz USB clock
    POWER_UP(UDP);                                                       // power up the USB controller module

    if (++pars->ucEndPoints > NUMBER_OF_USB_ENDPOINTS) {                 // add endpoint 0
        pars->ucEndPoints = NUMBER_OF_USB_ENDPOINTS;                     // limit to maximum available in device
    }
    #if defined _HW_SAM7SE                                               // {85}
    UDP_TXVC = UDP_TXVC_UDP_PUON;                                        // enable transceiver and pull-up (SAM7X and SAM7S default to transceiver on and don't have a pull-up option)
    #endif

    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * pars->ucEndPoints)); // get zeroed endpoint control structures

    UDP_FADDR = FEN;                                                     // default address 0

    if (pars->usConfig & USB_FULL_SPEED) {                               // the following is optional in case ports are used to control pull ups and pull downs
        ENABLE_DPLUS_PULLUP();                                           // enable pull up on D+ for full speed operation
    }
    else {
        ENABLE_DMINUS_PULLDOWN();                                        // enable pull up on D- for low speed operation
    }

    usb_endpoints[0].usEndpointSize = ENDPOINT_0_SIZE;                   // fixed endpoint size on control endpoint 0
    UDP_CSR0 = (EPEDS | EPTYPE_CONTROL);                                 // enable endpoint 0
    #if ENDPOINT_0_SIZE != 8
        #error ENDPOINT_0_SIZE must be 8 for the SAM7X!!
    #endif

    UDP_ICR = (BUS_WAKEUP_CLR | ENDBUSRES_CLR | SOFINT_CLR | RXRSM_CLR | RXSUSP_CLR); // clear all interrupt flags
    UDP_IER = (EP0INT | RXRSM);                                          // enable interrupts (reset interrupt mask is always set)
    #if defined _WINDOWS
    UDP_IMR = (UDP_IER | ENDBUSRES);
    #endif
    fnEnterInterrupt(UDP, PRIORITY_USB, _usb_isr);                       // enable USB interrupt
}

// Copy the data to be transmitted to the FIFO. This may only be called when there is a buffer to accept the data
//
extern void fnSendUSB_FIFO(unsigned char *ptrData, unsigned short usLength, int iEndpoint, USB_HW *ptrUSB_HW)
{
    #if defined _WINDOWS
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
    #if defined _WINDOWS
        fifo_addr++;
    #endif
    }
    return ucFIFO;
}

#endif                                                                   // end USB_INTERFACE



// External IRQ0
//
static __interrupt void irq_interrupt(void)                              // {10}
{
    if (irq_handler != 0) {
        iInterruptLevel = 1;
        (irq_handler)();
        iInterruptLevel = 0;
    }
}

// External IRQ1
//
static __interrupt void irq_interrupt_1(void)                            // {51}
{
    if (irq_handler1 != 0) {
        iInterruptLevel = 1;
        (irq_handler1)();
        iInterruptLevel = 0;
    }
}

// External FIQ
//
static __interrupt void fiq_interrupt(void)                              // {10}
{
    if (fiq_handler != 0) {
        iInterruptLevel = 1;
        (fiq_handler)();
        iInterruptLevel = 0;
    }
}

#if defined SUPPORT_PORT_INTERRUPTS

#define BOTH_EDGES   0x01

// This routine enters the user handler for a port interrupt. The handler can be assigned to multiple inputs
//
static void fnEnterHandler(unsigned long ulPortRef, unsigned long ulPortBits, void ( *int_handler )( void ), unsigned char ucConfiguration)
{

    int port_bit = 0;
    unsigned long ulBit = 0x00000001;
    #ifndef _HW_SAM7S                                                    // {75}
    ulPortBits &= ~0x80000000;
    #endif
    while (ulPortBits != 0) {
        if (ulPortBits & ulBit) {
            if (ulPortRef == PIOA) {
                gpio_handlers_a[port_bit] = int_handler;                 // enter the application handler
                gpio_config_a[port_bit] = ucConfiguration;
            }
    #ifndef _HW_SAM7S                                                    // {75}
            else {
                gpio_handlers_b[port_bit] = int_handler;                 // enter the application handler
                gpio_config_b[port_bit] = ucConfiguration;
            }
    #endif
            ulPortBits &= ~ulBit;
        }
        ulBit <<= 1;
        port_bit++;
    }
}

// Port A interrupt
//
static __interrupt void PortA_Interrupt(void)                            // {10}
{
    unsigned long ulInputChanges = PIO_ISR_A;                            // the inputs that have changed
    unsigned long ulInputState   = PIO_PDSR_A;                           // the momentary input state
#if defined _HW_SAM7S                                                    // {75}
    unsigned long ulBit = 0x80000000;
#else
    unsigned long ulBit = 0x40000000;
#endif
    int iInterrupt = (PORT_WIDTH - 1);
    ulInputChanges &= PIO_IMR_A;                                         // only treat enabled inputs
#if defined _WINDOWS
    PIO_ISR_A = 0;                                                       // reset the status flags due to read
#endif
    iInterruptLevel = 1;
    while (ulInputChanges != 0) {                                        // for each input change that has been detected
        if (ulInputChanges & ulBit) {                                    // an enabled input has changed
            if ((gpio_config_a[iInterrupt] & BOTH_EDGES)                 // any edge accepted
                || ((gpio_config_a[iInterrupt] & HIGH_LEVEL_SENSITIVE) && (ulInputState & ulInputChanges)) // positive edge accepted
                || (!(gpio_config_a[iInterrupt] & HIGH_LEVEL_SENSITIVE) && (~ulInputState & ulInputChanges))) { // negative edge accepted
                (gpio_handlers_a[iInterrupt])();                         // call the application handler
            }
            ulInputChanges &= ~ulBit;
        }
        iInterrupt--;
        ulBit >>= 1;
    }
    iInterruptLevel = 0;
}

#ifndef _HW_SAM7S                                                        // {75}
// Port B interrupt
//
static __interrupt void PortB_Interrupt(void)
{
    unsigned long ulInputChanges = PIO_ISR_B;                            // the inputs that have changed
    unsigned long ulInputState   = PIO_PDSR_B;                           // the momentary input state
    unsigned long ulBit = 0x40000000;
    int iInterrupt = (PORT_WIDTH - 1);
    ulInputChanges &= PIO_IMR_B;                                         // only treat enabled inputs
    #if defined _WINDOWS
    PIO_ISR_B = 0;                                                       // reset the status flags due to read
    #endif
    iInterruptLevel = 1;
    while (ulInputChanges != 0) {                                        // for each input change that has been detected
        if (ulInputChanges & ulBit) {                                    // an enabled input has changed
            if ((gpio_config_b[iInterrupt] & BOTH_EDGES)                 // any edge accepted
                || ((gpio_config_b[iInterrupt] & HIGH_LEVEL_SENSITIVE) && (ulInputState & ulInputChanges)) // positive edge accepted
                || (!(gpio_config_b[iInterrupt] & HIGH_LEVEL_SENSITIVE) && (~ulInputState & ulInputChanges))) { // negative edge accepted
                (gpio_handlers_b[iInterrupt])();                         // call the application handler
            }
            ulInputChanges &= ~ulBit;
        }
        iInterrupt--;
        ulBit >>= 1;
    }
    iInterruptLevel = 0;
}
#endif

    #if defined ETH_INTERFACE && defined INTERRUPT_TASK_PHY
static void fnPhyInterrupt(void)
{
    unsigned char int_phy_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT }; // define standard interrupt event

    while (!(PIO_PDSR_B & PHY_INTERRUPT)) {                              // while the interrupt line remains low
        volatile unsigned short usInterrupt;
        usInterrupt = fnReadMII(KS8721_INTERRUPT_CONTROL_STATUS_REGISTER);// read the cause(s) of the interrupt, which resets the bits
        if (PHY_LINK_UP_INT & usInterrupt) {
            usInterrupt = fnReadMII(KS8721_BASIC_STATUS_REGISTER);       // check the  details of link
            if (usInterrupt & (PHY_100MEG_HALF_DUPLEX | PHY_100MEG_FULL_DUPLEX | PHY_100MEG_T4_CAPABLE)) {
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
            }
            else {
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
            }
        }
        else if (PHY_LINK_DOWN_INT & usInterrupt) {
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
        }
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // inform the corresponding task
        iInterruptLevel = 0;                                             // release
    }
}
    #endif
#elif !defined _HW_SAM7S
// Port B interrupt (including PHY interrupt)
//
#if defined COMPILE_IAR && !defined COMPILE_IAR5                         // {46} disable optimising away reading (to reset interrupt)
 #pragma optimize=2
#endif

extern __interrupt void PortB_Interrupt(void)
{
#if defined INTERRUPT_TASK_PHY
    unsigned char int_phy_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT }; // define standard interrupt event
#endif
#if defined SUPPORT_HW_FLOW
    #if defined INTERRUPT_TASK_SERIAL_CONTROL
    unsigned char int_serial_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_SERIAL_CONTROL, INTERRUPT_EVENT, UNKNOWN_INTERRUPT };// define standard interrupt event
    #endif
#endif
    volatile unsigned long ulDummy;

    while ((ulDummy = PIO_ISR_B) != 0) {                                 // clear pending interrupt(s) and handle only enabled interrupts
        if ((ulDummy & PIO_IMR_B) == 0) {
            break;
        }
#if defined SUPPORT_HW_FLOW
        if (ulDummy & DATA_TERMINAL_READY) {                             // handle DTR changes
    #if defined INTERRUPT_TASK_SERIAL_CONTROL
            if (PIO_PDSR_B & DATA_TERMINAL_READY) {                      // read state of pin (which has just changed)
                int_serial_message[MSG_INTERRUPT_EVENT] = DTR_CHANGE_ACTIVE;
                //PIO_SODR_B = (DATA_SET_READY);                         // DSR follows DTR - for test
            }
            else {
                int_serial_message[MSG_INTERRUPT_EVENT] = DTR_CHANGE_INACTIVE;
                //PIO_CODR_B = (DATA_SET_READY);                         // DSR follows DTR - for test
            }
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, (unsigned char*)int_serial_message, HEADER_LENGTH); // inform the corresponding task
            iInterruptLevel = 0;                                         // release
    #endif
      }
#endif
#if defined ETH_INTERFACE
        while (!(PIO_PDSR_B & PHY_INTERRUPT)) {                          // while the interrupt line remains low
            volatile unsigned short usInterrupt;
            usInterrupt = fnReadMII(KS8721_INTERRUPT_CONTROL_STATUS_REGISTER);// read the cause(s) of the interrupt, which resets the bits
    #if defined INTERRUPT_TASK_PHY
            if (PHY_LINK_UP_INT & usInterrupt) {
                usInterrupt = fnReadMII(KS8721_BASIC_STATUS_REGISTER);   // check the  details of link
                if (usInterrupt & (PHY_100MEG_HALF_DUPLEX | PHY_100MEG_FULL_DUPLEX | PHY_100MEG_T4_CAPABLE)) {
                    int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
                }
                else {
                    int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
                }
            }
            else if (PHY_LINK_DOWN_INT & usInterrupt) {
                int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
            }
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // Inform the corresponding task
            iInterruptLevel = 0;                                         // release
    #endif

    #if defined _WINDOWS
          PIO_PDSR_B |= PHY_INTERRUPT;                                   // interrupt line is cleared
          PIO_ISR_B &= ~PHY_INTERRUPT;                                   // interrupt no longer pending
    #endif
        }
#endif
    }
}
#endif

#if defined SSC_INTERFACE                                                // {66}

// SSC interrupt handler
//
static __interrupt void SSC_Interrupt(void)
{
    iInterruptLevel = 1;
    while (AIC_IPR & SSC) {                                              // while interrupts present from SSC
        if ((SSC_IMR & SSC_RX_READY) && (SSC_SR & SSC_RX_READY)) {
            SSC_IDR = SSC_RX_READY;                                      // disable to avoid further
    #if defined _WINDOWS
            SSC_IMR &= ~SSC_IDR;
            SSC_IDR = 0;
    #endif
        }
#if defined SSC_SUPPORT_DMA
        if ((SSC_IMR & SSC_RX_END) && (SSC_SR & SSC_RX_END)) {
            fnSSCRxWord(0, 0);                                           // frame reception complete
        }
        if ((SSC_IMR & SSC_TX_END) && (SSC_SR & SSC_TX_END)) {
            SSC_IDR = SSC_TX_END;                                        // disable to avoid further
    #if defined _WINDOWS
            SSC_IMR &= ~SSC_IDR;
            SSC_IDR = 0;
    #endif
            fnSSCTxWord(0);                                              // transmit block complete interrupt - write next block
        }
#endif
        if ((SSC_IMR & SSC_TX_READY) && (SSC_SR & SSC_TX_READY)) {
            fnSSCTxWord(0);                                              // transmit data empty interrupt - write next byte
        }
#if defined _WINDOWS
        AIC_IPR &= ~SSC;                                                 // simulate reset of interrupt flag
#endif
    }
    iInterruptLevel = 0;
}

// Configure the SSC hardware
//
extern void fnConfigSSC(QUEUE_HANDLE Channel, SSCTABLE *pars)
{
    unsigned long ulConfig = (SSC_TX_CLK_INTERNAL | SSC_TX_CLK_OUT_CONTINOUS |
                              SSC_TX_CLK_NO_GATING | SSC_TX_START_TF_RISING | (pars->ucTxDataShift << SSC_TX_START_DELAY_SHIFT) |
                              (((pars->ucWordSize * pars->ucFrameLength)/2) << SSC_TX_PERIOD_SHIFT));

    POWER_UP(SSC);                                                       // enable clocks to SSC
    SSC_CMR = (pars->usSpeed);                                           // set clock divider
    if (pars->usConfig & TX_CLOCK_INVERTED) {
        ulConfig |= SSC_TX_CLK_INVERTED;
    }
    SSC_TCMR = ulConfig;                                                 // configure TX clock
    ulConfig = (((pars->ucWordSize - 1) & 0x1f) | SSC_TX_PIN_DEFAULT_LEVEL_HIGH | (((pars->ucFrameLength - 1) & 0x0f) << SSC_TX_DATA_PER_FRAME_SHIFT) | (0 << SSC_TX_FSLEN_SHIFT) | SSC_TF_BEFORE_DATA);
    if (pars->usConfig & TX_MSB_FIRST) {
        ulConfig |= SSC_TX_MSB_FIRST;
    }
    if (pars->usConfig & TX_NEGATIVE_FRAME_PULSE) {
        ulConfig |= SSC_TF_OUT_NEG_PULSE;
    }
    else {
        ulConfig |= SSC_TF_OUT_POS_PULSE;
    }
    SSC_TFMR = ulConfig;                                                 // configure TX framing
    ulConfig = (SSC_RX_CLK_RX_CLK_PIN | SSC_RX_CLK_NO_GATING | (pars->ucRxDataShift << SSC_RX_START_DELAY_SHIFT));
    if (pars->usConfig & RX_CLOCK_INVERTED) {
        ulConfig |= SSC_RX_CLK_INVERTED;
    }
    if (pars->usConfig & RX_SYNC_FALLING) {
        ulConfig |= SSC_RX_START_RF_FALLING;
    }
    else {
        ulConfig |= SSC_RX_START_RF_RISING;
    }
    SSC_RCMR = ulConfig;                                                 // configure RX clock
    ulConfig = (((pars->ucWordSize - 1) & 0x1f) | (((pars->ucFrameLength - 1) & 0x0f) << SSC_RX_DATA_PER_FRAME_SHIFT));
    if (pars->usConfig & RX_MSB_FIRST) {
        ulConfig |= SSC_RX_MSB_FIRST;
    }
    SSC_RFMR = ulConfig;                                                 // configure RX framing
}

// Enable transmission on the defined channel
//
extern void fnSSC_TxOn(QUEUE_HANDLE Channel)
{
    fnEnterInterrupt(SSC, PRIORITY_SSC, SSC_Interrupt);                  // set the interrupt handling routine
    PIO_ASR_A  = (TD | TF | TK);                                         // configure the TX pins for SSC use
    PIO_PDR_A  = (TD | TF | TK);
    #if defined _WINDOWS
    SSC_SR |= (SSC_TX_ENABLED | SSC_TX_READY);
    SSC_IMR |= SSC_IER;
    SSC_IER = 0;
    #endif
}

// Disable transmission on the defined SSC channel
//
void fnSSC_TxOff(QUEUE_HANDLE Channel)
{
    SSC_CR = SSC_TXDIS;                                                  // disable transmitter
    #if defined _WINDOWS
    SSC_SR &= ~SSC_TX_ENABLED;
    #endif
}

// Enable reception on the defined channel
//
extern void fnSSC_RxOn(QUEUE_HANDLE Channel)
{
    fnEnterInterrupt(SSC, PRIORITY_SSC, SSC_Interrupt);                  // set the interrupt handling routine
    PIO_ASR_A  = (RD | RF | RK);                                         // configure the RX pins for SSC use
    PIO_PDR_A  = (RD | RF | RK);
    SSC_IER = SSC_RX_READY;                                              // enable interrupts when rx buffer is not empty
    SSC_CR  = SSC_RXEN;                                                  // enable receiver
    #if defined _WINDOWS
    SSC_SR |= SSC_RX_ENABLED;
    SSC_IMR |= SSC_IER;
    SSC_IER = 0;
    #endif
}

// Disable reception on the defined SSC channel
//
void fnSSC_RxOff(QUEUE_HANDLE Channel)
{
    SSC_CR = SSC_RXDIS;                                                  // disable transmitter
    #if defined _WINDOWS
    SSC_SR &= ~SSC_RX_ENABLED;
    #endif
}

// The SSC driver uses this call to send a data word over the SSC interface
//
extern int fnTxSSCword(QUEUE_HANDLE channel, unsigned long ulTxDate)
{
    if (!(SSC_SR & SSC_TX_ENABLED)) {
        SSC_CR  = SSC_TXEN;                                              // enable transmitter
    }
    if (SSC_SR & SSC_TX_READY) {                                         // tx not busy
        SSC_THR = ulTxDate;                                              // send data word
        SSC_IER = SSC_TX_READY;                                          // ensure tx interrupt enabled
    #if defined _WINDOWS
        SSC_SR &= ~(SSC_TX_READY | SSC_TX_END | SSC_TX_READY);
        SSC_IMR |= SSC_IER;
        SSC_IER = 0;
        iInts |= CHANNEL_0_SSC_INT;                                      // simulate interrupt
    #endif
        return 0;                                                        // OK, we were not busy
    }

    return 1;                                                            // not sent because we're busy
}

// The SSC driver uses this call to disable the transmit interrupt of the SSC port
//
extern void fnClearSSCTxInt(QUEUE_HANDLE channel)
{
    SSC_CR = SSC_TXDIS;                                                  // disable transmitter
    SSC_IDR = SSC_TX_READY;                                              // disable tx interrupt
    #if defined _WINDOWS
    SSC_SR &= ~SSC_TX_ENABLED;
    SSC_IMR &= ~SSC_IDR;                                                 // reflect in status register
    SSC_IDR = 0;
    #endif
}

    #if defined SSC_SUPPORT_DMA
// Set up the PDC for reception of the defined character block
//
extern void fnPrepareSSCRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
    if (!(SSC_SR & SSC_RX_ENABLED)) {
        #if defined SSC_DMA_DOUBLE_BUF_RX
        unsigned char ucWordWidth = ((((unsigned char)SSC_RFMR & 0x1f) + 8)/8);
        if (ucWordWidth >= 3) {
            ucWordWidth = 4;
        }
        #endif
        fnSSC_RxOn(channel);
        SSC_IDR = SSC_RX_READY;                                          // disable word interrupt
        #if defined SSC_DMA_DOUBLE_BUF_RX
        SSC_RPR = (unsigned long)ptrStart;                               // transfer start location
        SSC_RCR = (unsigned short)rx_length;                             // transfer count
        ptrStart += (rx_length * ucWordWidth);
        #endif
    }
        #if defined SSC_DMA_DOUBLE_BUF_RX
    SSC_RNPR = (unsigned long)ptrStart;                                  // next transfer start location
    SSC_RNCR = (unsigned short)rx_length;                                // next transfer count
        #else
    SSC_RPR = (unsigned long)ptrStart;                                   // next transfer start location
    SSC_RCR = (unsigned short)rx_length;                                 // next transfer count
        #endif
    SSC_PTCR = PDC_RXTEN;                                                // enable transfer
    SSC_IER = SSC_RX_END;                                                // enable interrupt on transfer complete
        #if defined _WINDOWS
    SSC_IMR &= ~SSC_IDR;
    SSC_IMR |= SSC_IER;
    SSC_IER = SSC_IDR = 0;
    SSC_PTSR |= PDC_RXTEN;
        #endif
}

// Start transfer of a block via DMA
//
extern QUEUE_TRANSFER fnTxSCCWordDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length)
{
    if (!(SSC_SR & SSC_TX_ENABLED)) {
        SSC_CR  = SSC_TXEN;                                              // enable transmitter if presently disabled
    }
    SSC_TPR = (unsigned long)ptrStart;                                   // transfer start location
    SSC_TCR = (unsigned short)tx_length;                                 // transfer count
    SSC_PTCR = PDC_TXTEN;                                                // start transfer
    SSC_IER = SSC_TX_END;                                                // enable interrupt on transfer complete
        #if defined _WINDOWS
    SSC_IMR |= SSC_IER;
    SSC_IER = 0;
    SSC_PTSR |= PDC_TXTEN;
    iDMA |= DMA_CONTROLLER_6;
        #endif
    return tx_length;
}
    #endif
#endif

#if defined SUPPORT_TIMER                                                // {55}
#define HW_TIMERS_AVAILABLE   3                                          // SAM7X has 3 x 16 bit timer / counters

static void ( *timer_handler[HW_TIMERS_AVAILABLE] )( void ) = {0};
    #if defined _HW_TIMER_MODE
    static unsigned char _hw_timer_mode[HW_TIMERS_AVAILABLE] = {0};      // {67}
    #endif

static __interrupt void timer0_Interrupt(void)
{
    (void)TC_SR_0;                                                       // read the status to reset interrupt
    #if defined _HW_TIMER_MODE                                           // {67}
    if (!(_hw_timer_mode[0] & TIMER_PERIODIC)) {                         // don't disable if in periodic mode
        TC_IDR_0 = TIM_CPCS;                                             // disable interrupt
        #if defined _WINDOWS
        TC_SR_0 &= ~TIM_CLKSTA; TC_IMR_0 &= ~TIM_CPCS;                   // flag that the timer is not running
        #endif
    }
    #else
    TC_IDR_0 = TIM_CPCS;                                                 // disable interrupt
        #if defined _WINDOWS
    TC_SR_0 &= ~TIM_CLKSTA; TC_IMR_0 &= ~TIM_CPCS;                       // flag that the timer is not running
        #endif
    #endif
    if (timer_handler[0] != 0) {
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked during subroutines
        (timer_handler[0])();
        iInterruptLevel = 0;                                             // release
    }
}

static __interrupt void timer1_Interrupt(void)
{
    (void)TC_SR_1;                                                       // read the status to reset interrupt
    #if defined _HW_TIMER_MODE                                           // {67}
    if (!(_hw_timer_mode[1] & TIMER_PERIODIC)) {                         // don't disable if in periodic mode
        TC_IDR_1 = TIM_CPCS;                                             // disable interrupt
        #if defined _WINDOWS
        TC_SR_1 &= ~TIM_CLKSTA; TC_IMR_1 &= ~TIM_CPCS;                   // flag that the timer is not running
        #endif
    }
    #else
    TC_IDR_1 = TIM_CPCS;                                                 // disable interrupt
        #if defined _WINDOWS
    TC_SR_1 &= ~TIM_CLKSTA; TC_IMR_1 &= ~TIM_CPCS;                       // flag that the timer is not running
        #endif
    #endif
    if (timer_handler[1] != 0) {
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked during subroutines
        (timer_handler[1])();
        iInterruptLevel = 0;                                             // release
    }
}

static __interrupt void timer2_Interrupt(void)
{
    (void)TC_SR_2;                                                       // read the status to reset interrupt
    #if defined _HW_TIMER_MODE                                           // {67}
    if (!(_hw_timer_mode[2] & TIMER_PERIODIC)) {                         // don't disable if in periodic mode
        TC_IDR_2 = TIM_CPCS;                                             // disable interrupt
        #if defined _WINDOWS
        TC_SR_2 &= ~TIM_CLKSTA; TC_IMR_2 &= ~TIM_CPCS;                   // flag that the timer is not running
        #endif
    }
    #else
    TC_IDR_2 = TIM_CPCS;                                                 // disable interrupt
        #if defined _WINDOWS
    TC_SR_2 &= ~TIM_CLKSTA; TC_IMR_2 &= ~TIM_CPCS;                       // flag that the timer is not running
        #endif
    #endif
    if (timer_handler[2] != 0) {
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked during subroutines
        (timer_handler[2])();
        iInterruptLevel = 0;                                             // release
    }
}

static const unsigned long _timer_handler[HW_TIMERS_AVAILABLE] = {
    (unsigned long)timer0_Interrupt,
    (unsigned long)timer1_Interrupt,
    (unsigned long)timer2_Interrupt
};
#endif

#if defined SUPPORT_ADC                                                  // {47}
static void ( *_adc_handler)( ADC_INTERRUPT_RESULT* ) = 0;
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
#if defined _WINDOWS
    ADC_IMR &= ~ADC_IDR; ADC_IDR = 0;
#endif
    if (adc_trigger_source != 0) {
        POWER_DOWN(adc_trigger_source);                                  // disable clocks to timer in PMC
    }
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
    #if defined ADC_AUTO_TRIGGERS
    int iComplete = 0;
    #endif
    ADC_INTERRUPT_RESULT adc_res;
    while ((ulStatus = ADC_SR) & (EOC0 | EOC1 | EOC2 | EOC3 | EOC4 | EOC5 | EOC6 | EOC7)) {
        if (ulBit & ulStatus) {                                          // sample ready
            adc_res.sADC_value = (signed short)*(ADC_CDR0_ADD + iChannel);// read the value, which clears the interrupt from this channel
            adc_res.ucADC_flags = ADC_RESULT_VALID;
    #if defined _WINDOWS
            ADC_SR &= ~(ulBit);                                          // clear the sample ready bit
    #endif
    #if defined ADC_AUTO_TRIGGERS
            if (ucADCmode[iChannel] != 0) {                              // this channel is operating in trigger mode
                if (ucPreviousLevels[iChannel] != 0xffff) {
                  //int iInterrupt = 0;                                  // {57}
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
    #if defined ADC_AUTO_TRIGGERS
    if (iComplete == 1) {                                                // last single shot trigger fired so disable operation
        if (adc_trigger_source != 0) {                                   // timer operating
            POWER_DOWN(adc_trigger_source);                              // disable clocks to timer in PMC to stop further ADC sample triggering
        }
    }
    #endif
}
#endif

#if defined SUPPORT_RTC                                                  // {91}

static int rtc_interrupts = 0;
static unsigned long rtc_alarm = 0;
static unsigned long ulRTC_stopwatch = 0;

static void (*rtc_interrupt_handler[6])(void) = {0};                     // RTC interrupt handlers


// Called at a 1 second rate
//
static void _rtc_handler(void)
{
    #if defined SUPPORT_SW_RTC
    unsigned long ulNewSeconds = *RTC_SECONDS_LOCATION;
    ulNewSeconds++;
    *RTC_SECONDS_LOCATION = ulNewSeconds;                                // increment the seconds count in non-initialised ram
    if (rtc_interrupts & RTC_ALARM_INT) {
        if (ulNewSeconds == rtc_alarm) {                                 // alarm date / time match
            rtc_interrupts |= RTC_ALARM_INT_MATCH;
        }
    }
    #endif
    if ((ulRTC_stopwatch != 0) && (--ulRTC_stopwatch == 0)) {
        if (rtc_interrupts & RTC_STOPWATCH) {                            // stopwatch interrupt enabled
            rtc_interrupts |= RTC_STOPWATCH_MATCH;
        }
    }
    fnSecondsTick(rtc_interrupt_handler, rtc_interrupts);                // generic handling
    rtc_interrupts &= ~(RTC_ALARM_INT_MATCH | RTC_STOPWATCH_MATCH);      // ensure that alarm and stopwatch interrupts are single-shot interrupts
}



// This routine attempts a high level of compatibility with the Coldfire V2 RTC - operation based on 1s interrupt
//
extern int fnConfigureRTC(void *ptrSettings)
{
    int iIRQ = 0;
    RTC_SETUP *ptr_rtc_setup = (RTC_SETUP *)ptrSettings;
    switch (ptr_rtc_setup->command & ~(RTC_DISABLE | RTC_INITIALISATION)) { // {99}
    case RTC_TIME_SETTING:                                               // set time to RTT
        POWER_UP(SYSIRQ);                                                // ensure RTT clocks are enabled for interrupt operation
        RTT_MR = RTT_MR_RTTRST;                                          // restart the timer, which also resets its count value
        ulReferenceTime = fnConvertTimeSeconds(ptr_rtc_setup, 1);        // set the present time and date as seconds since 1970
    #if defined _WINDOWS
        RTT_VR = 0;
    #endif
        RTT_MR = (0x00008000 | RTT_MR_RTTINCIEN);                        // set periodic interrupt (32k prescaler for 1s interrupt) and allow RTT to run
        break;

    case RTC_GET_TIME:                                                   // get the present time
        fnGetRTC(ptr_rtc_setup);
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
            ulRTC_stopwatch = ((ptr_rtc_setup->ucHours * 60 * 60) + (ptr_rtc_setup->ucMinutes * 60) + ptr_rtc_setup->ucSeconds); // set a stop watch interrupt after this many additional minutes (1 to 64 minutes for Coldfire compatibility but extended to hours and seconds too)
        }
        if (RTC_DISABLE & (ptr_rtc_setup->command)) {
            rtc_interrupts &= ~(0x01 << iIRQ);                           // disable interrupt
            return 0;                                                    // disable function rather than enable
        }
        rtc_interrupt_handler[iIRQ] = ((INTERRUPT_SETUP *)ptrSettings)->int_handler; // enter the handling interrupt
      //fnConvertSecondsTime(RTT_VR + ulReferenceTime);                  // take the present seconds count value and convert to time and date
        POWER_UP(SYSIRQ);                                                // ensure RTT clocks are enabled for interrupt operation
        RTT_MR = (0x00008000 | RTT_MR_RTTINCIEN);                        // set periodic interrupt (32k prescaler for 1s interrupt)
        rtc_interrupts |= (0x01 << iIRQ);                                // flag interrupt(s) enabled
        break;
    }
    return 0;
}

    #if defined SDCARD_SUPPORT && defined SUPPORT_FILE_TIME_STAMP
// This routine supports date and time stamping of files and directories. If the information is not available it could return 1
// so that a fixed stamp is set. It is assumed that the RTC in the Kinetis will be used if the support is activated
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


// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)                      // {10}
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
    case PORT_INTERRUPT:                                                 // {33}
        {
            unsigned char ucTrigger = 0;
            switch ((((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) & ~(IRQ_PULLUP_ENABLE | IRQ_GLITCH_ENABLE)) { // {70}
            case IRQ_HIGH_LEVEL_SENSITIVE:                               // not valid for port interrupts
                ucTrigger = HIGH_LEVEL_SENSITIVE;
                break;
            case IRQ_LOW_LEVEL_SENSITIVE:                                // not valid for port interrupts
                ucTrigger = LOW_LEVEL_SENSITIVE;
                break;
            case IRQ_RISING_EDGE:
                ucTrigger = POSITIVE_EDGE_TRIGGERED;
                break;
            case IRQ_FALLING_EDGE:
                ucTrigger = NEGATIVE_EDGE_TRIGGERED;
                break;
    #if defined SUPPORT_PORT_INTERRUPTS
            case (IRQ_RISING_EDGE | IRQ_FALLING_EDGE):                   // this is only possible on port interrupts
                ucTrigger = BOTH_EDGES;
                break;
    #endif
            default:
                return;
            }
    #if defined SUPPORT_PORT_INTERRUPTS
            if ((((INTERRUPT_SETUP *)ptrSettings)->int_port) == PORT_A) {
                unsigned long ulPortBits = (((INTERRUPT_SETUP *)ptrSettings)->int_port_bits);
                POWER_UP(PIOA);                                          // {37} ensure the port is powered
                PIO_PER_A = ulPortBits;                                  // {52} ensure enabled as port
                PIO_ODR_A = ulPortBits;                  _SIM_PORT_CHANGE// ensure an input
                if (IRQ_PULLUP_ENABLE & ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) { // {70}
                    _ENABLE_PULLUP(A, ulPortBits);
                }
                if (IRQ_GLITCH_ENABLE & ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) { // {70}
                    _ENABLE_GLITCH_FILTER(A, ulPortBits);
                }
                fnEnterInterrupt(PIOA, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), PortA_Interrupt);
                fnEnterHandler(PIOA, (((INTERRUPT_SETUP *)ptrSettings)->int_port_bits), (((INTERRUPT_SETUP *)ptrSettings)->int_handler), ucTrigger);
                PIO_IER_A = ulPortBits;                                  // enable specific interrupt lines
        #if defined _WINDOWS
                PIO_IMR_A |= PIO_IER_A; PIO_IER_A = 0;
        #endif
                break;
            }
            else if ((((INTERRUPT_SETUP *)ptrSettings)->int_port) == PORT_B) {
        #ifndef _HW_SAM7S                                                // {75}
                unsigned long ulPortBits = (((INTERRUPT_SETUP *)ptrSettings)->int_port_bits);
                POWER_UP(PIOB);                                          // {37} ensure the port is powered
                PIO_PER_B = ulPortBits;                                  // {52} ensure enabled as port
                PIO_ODR_B = ulPortBits;                  _SIM_PORT_CHANGE// ensure an input
                if (IRQ_PULLUP_ENABLE & ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) { // {70}
                    _ENABLE_PULLUP(B, ulPortBits);
                }
                if (IRQ_GLITCH_ENABLE & ((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) { // {70}
                    _ENABLE_GLITCH_FILTER(B, ulPortBits);
                }
                fnEnterInterrupt(PIOB, ((((INTERRUPT_SETUP *)ptrSettings)->int_priority)), PortB_Interrupt);
                fnEnterHandler(PIOB, (((INTERRUPT_SETUP *)ptrSettings)->int_port_bits), (((INTERRUPT_SETUP *)ptrSettings)->int_handler), ucTrigger);
                PIO_IER_B = ulPortBits;                                  // enable specific interrupt lines
            #if defined _WINDOWS
                PIO_IMR_B |= PIO_IER_B; PIO_IER_B = 0;
            #endif
        #endif
                break;
            }
    #endif
            if ((((INTERRUPT_SETUP *)ptrSettings)->int_port) == PORTA_IRQ0) {
                irq_handler = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
                fnEnterInterrupt(AIC_IRQ0, (unsigned char)(PRIORITY_IRQ | ucTrigger), irq_interrupt);
    #if defined _HW_SAM7S                                                // {75}
                PIO_BSR_A  = (IRQ0_B);                                   // configure the pin for IRQ0 use
                PIO_PDR_A  = (IRQ0_B);                                   _SIM_PORT_CHANGE
    #elif defined _HW_SAM7SE                                             // {83}
                PIO_ASR_B  = (IRQ0);                                     // configure the pin for IRQ0 use
                PIO_PDR_B  = (IRQ0);                                     _SIM_PORT_CHANGE
    #elif defined _HW_SAM7L                                              // {100}
                PIO_BSR_C  = (IRQ0_B);                                   // configure the pin for IRQ0 use
                PIO_PDR_C  = (IRQ0_B);                                   _SIM_PORT_CHANGE
    #else
                PIO_ASR_A  = (IRQ0);                                     // configure the pin for IRQ0 use
                PIO_PDR_A  = (IRQ0);                                     _SIM_PORT_CHANGE
    #endif
            }
            else if ((((INTERRUPT_SETUP *)ptrSettings)->int_port) == PORTA_IRQ1) { // {51}
                irq_handler1 = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
                fnEnterInterrupt(AIC_IRQ1, (unsigned char)(PRIORITY_IRQ1 | ucTrigger), irq_interrupt_1);
    #if defined _HW_SAM7S || defined _HW_SAM7SE                          // {75}{83}
                PIO_ASR_A  = (IRQ1);                                     // configure the pin for IRQ1 use
                PIO_PDR_A  = (IRQ1);                                     _SIM_PORT_CHANGE
    #elif defined _HW_SAM7L                                              // {100}
                PIO_ASR_C  = (IRQ1);                                     // configure the pin for IRQ1 use
                PIO_PDR_C  = (IRQ1);                                     _SIM_PORT_CHANGE
    #else
                PIO_BSR_A  = (IRQ1B);                                    // configure the pin for IRQ1 use
                PIO_PDR_A  = (IRQ1B);                                    _SIM_PORT_CHANGE
    #endif
            }
            else {                                                       // assume PORTA_FIQ
                fiq_handler = ((INTERRUPT_SETUP *)ptrSettings)->int_handler; // {45}
                fnEnterInterrupt(AIC_FIQ, ucTrigger, fiq_interrupt);
            }
        }
        break;
#if defined SUPPORT_PWM_CONTROLLER                                       // {73}
    case PWM_CONFIGURATION:                                              // configure PWM controller
        {
            int iTimer = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_reference); // the PWM channel to use
            unsigned char ucMode = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_mode);
            if (iTimer >= 4) {                                           // check for valid PWM controller channel
                return;
            }
            if (ucMode & TIMER_STOP_PWM) {                               // PWM is to be stopped
                PWM_DIS = (ucMode & (TIMER_PWM_START_0 | TIMER_PWM_START_1 | TIMER_PWM_START_2 | TIMER_PWM_START_3));
                if (PWM_SR == 0) {                                       // no channels are active
                    POWER_DOWN(PWMC);                                    // power down the PWM controller - its state remains valid
                }
                return;
            }
            POWER_UP(PWMC);                                              // ensure PWM controller is powered
            if (!(TIMER_PWM_NO_CONFIG & ucMode)) {                       // if configuration is not expressly disabled
                unsigned long *ptrChannel = (PWM_CMR_ADD + (iTimer * (0x20/sizeof(unsigned long))));
                if (ucMode & TIMER_PWM_ALT) {                            // configure the pin for use
    #if defined _HW_SAM7S                                                // {75}
                    PIO_BSR_A  = (PWM0_B << iTimer);                     // configure the secondary pin for PMW use
                    PIO_PDR_A  = (PWM0_B << iTimer);     _SIM_PORT_CHANGE// disable GPIO use
    #elif defined _HW_SAM7SE                                             // {83}
                    PIO_ASR_B  = (PWM0b << iTimer);                      // configure the secondary pin for PMW use
                    PIO_PDR_B  = (PWM0b << iTimer);      _SIM_PORT_CHANGE// disable GPIO use
    #else
                    PIO_BSR_B  = (PWM0_B << iTimer);                     // configure the secondary pin for PMW use
                    PIO_PDR_B  = (PWM0_B << iTimer);     _SIM_PORT_CHANGE// disable GPIO use
    #endif
                }
                else {
    #if defined _HW_SAM7S                                                // {75}
                    PIO_ASR_A  = (PWM0 << iTimer);                       // configure the primary pin for PMW use
                    PIO_PDR_A  = (PWM0 << iTimer);       _SIM_PORT_CHANGE// disable GPIO use
    #else
                    PIO_ASR_B  = (PWM0 << iTimer);                       // configure the primary pin for PMW use
                    PIO_PDR_B  = (PWM0 << iTimer);       _SIM_PORT_CHANGE// disable GPIO use
    #endif
                }
                *ptrChannel++ = PWM_CPOL;                                // set MCLK as clock source and waveform starts '1'
                *ptrChannel++ = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->pwm_value; // set duty cycle
                *ptrChannel   = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value; // set the frequency
            }
            PWM_ENA = (ucMode & (TIMER_PWM_START_0 | TIMER_PWM_START_1 | TIMER_PWM_START_2 | TIMER_PWM_START_3)); // start the PWM channels as defined
        }
        break;
#endif

#if defined SUPPORT_TIMER                                                // {55}
    case TIMER_INTERRUPT:                                                // {12}
        {
    #if defined SUPPORT_TIMER_EXT_CLOCK
            static unsigned short usXC_connected = 0;
            #define XC0_IN_USE_BY_0   0x001
            #define XC0_IN_USE_BY_1   0x002
            #define XC0_IN_USE_BY_2   0x004
            #define XC1_IN_USE_BY_0   0x008
            #define XC1_IN_USE_BY_1   0x010
            #define XC1_IN_USE_BY_2   0x020
            #define XC2_IN_USE_BY_0   0x040
            #define XC2_IN_USE_BY_1   0x080
            #define XC2_IN_USE_BY_2   0x100
            #define XC0_IN_USE        (XC0_IN_USE_BY_0 | XC0_IN_USE_BY_1| XC0_IN_USE_BY_2)
            #define XC1_IN_USE        (XC1_IN_USE_BY_0 | XC1_IN_USE_BY_1| XC1_IN_USE_BY_2)
            #define XC2_IN_USE        (XC2_IN_USE_BY_0 | XC2_IN_USE_BY_1| XC2_IN_USE_BY_2)
    #endif
            unsigned long ulInterruptSource;
            volatile unsigned long *ptrTimeBlock;
            unsigned short usMode = (WAVE | WAVSEL_UP_TRG_RC | CPCSTOP | CPCDIS); // default for single shot timer mode
            int iTimer = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_reference); // the timer to use
            if (iTimer >= HW_TIMERS_AVAILABLE) {
                return;
            }
            switch (iTimer) {
    #if HW_TIMER_CHANNEL != 0 || !defined GLOBAL_HARDWARE_TIMER          // protect from collision with global hardware timer on channel
            case 0:
                ulInterruptSource = TC0;
                ptrTimeBlock = (unsigned long *)TIMER0_PERIPHERAL_BLOCK;
                break;
    #endif
    #if HW_TIMER_CHANNEL != 1 || !defined GLOBAL_HARDWARE_TIMER          // protect from collision with global hardware timer on channel
            case 1:
                ulInterruptSource = TC1;
                ptrTimeBlock = (unsigned long *)TIMER1_PERIPHERAL_BLOCK;
                break;
    #endif
    #if HW_TIMER_CHANNEL != 2 || !defined GLOBAL_HARDWARE_TIMER          // protect from collision with global hardware timer on channel
            case 2:
                ulInterruptSource = TC2;
                ptrTimeBlock = (unsigned long *)TIMER2_PERIPHERAL_BLOCK;
                break;
    #endif
            default:
                return;                                                  // ignore non-supported timer channels
            }
    #if defined _HW_TIMER_MODE
            if (((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_mode == TIMER_DISABLE) { // {97}
                TC_BMR &= ~(BMR_XC0_MASK << (2 * iTimer));
        #if defined SUPPORT_TIMER_EXT_CLOCK
                usXC_connected &= ~(XC0_IN_USE_BY_0 | XC1_IN_USE_BY_0 | XC2_IN_USE_BY_0);
        #endif
                POWER_DOWN(ulInterruptSource);
                return;
            }
    #endif
            POWER_UP(ulInterruptSource);                                 // enable clocks to timer in PMC
            *ptrTimeBlock = (TIM_CLKDIS | TIM_SWTRG);                    // disable timer clock and reset timer in TC_CCR_X
            (void)*(ptrTimeBlock + (0x20/sizeof(unsigned long)));        // read status register to reset possible pending interrupt
            timer_handler[iTimer] = (((TIMER_INTERRUPT_SETUP *)ptrSettings)->int_handler);
            fnEnterInterrupt(ulInterruptSource, (((TIMER_INTERRUPT_SETUP *)ptrSettings)->int_priority), (void (*)(void))_timer_handler[iTimer]);
    #if defined _HW_TIMER_MODE
            _hw_timer_mode[iTimer] = ((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_mode; // {67}
    #endif
    #if defined SUPPORT_US_TIMER
            if ((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value) != 0) { // {56} ms timer has preference for compatibility
                *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_5 | usMode);// MCK/1024, wave mode, trigger on register RC, stop after triggering in TC_CMR_X
                *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value) * (MASTER_CLOCK/1024))/1000);  // set ms match value in TC_RC_X
            }
            else {
                *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_3 | usMode);// MCK/32, wave mode, trigger on register RC, stop after triggering in TC_CMR_X
                *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_us_value) * (MASTER_CLOCK/32000))/1000);  // set ms match value in TC_RC_X
            }
    #else
        #if defined SUPPORT_TIMER_EXT_CLOCK
            if (_hw_timer_mode[iTimer] & EXTERNAL_TIMER_SOURCE) {        // {77} external time source required
                switch (_hw_timer_mode[iTimer] & EXTERNAL_TIMER_SOURCE) {
                case TIMER_SOURCE_TCLK0:
                    if ((TC_BMR & BMR_XC0_MASK) > XC0_TCLK0) {
                        return;                                          // already in use so not possible
                    }
            #if defined _HW_SAM7S
                    PIO_BSR_A  = (TCLK0_B);
                    PIO_PDR_A  = (TCLK0_B);
            #elif defined _HW_SAM7SE                                     // {83}
                    PIO_ASR_B  = (TCLK0);
                    PIO_PDR_B  = (TCLK0);
            #else
                    PIO_BSR_B  = (TCLK0_B);
                    PIO_PDR_B  = (TCLK0_B);
            #endif
                    TC_BMR &= ~(BMR_XC0_MASK);                           _SIM_PORT_CHANGE
                    usXC_connected |= (XC0_IN_USE_BY_0 << iTimer);
                    usMode = CLOCK_SOURCE_XC0;
                    break;
                case TIMER_SOURCE_TCLK1:
                    if ((TC_BMR & BMR_XC1_MASK) > XC1_TCLK1) {
                        return;                                          // already in use so not possible
                    }
            #if defined _HW_SAM7S
                    PIO_BSR_A  = (TCLK1_B);
                    PIO_PDR_A  = (TCLK1_B);
            #elif defined _HW_SAM7SE                                     // {83}
                    PIO_ASR_B  = (TCLK1);
                    PIO_PDR_B  = (TCLK1);
            #else
                    PIO_BSR_B  = (TCLK1_B);
                    PIO_PDR_B  = (TCLK1_B);
            #endif
                    TC_BMR &= ~(BMR_XC1_MASK);                           _SIM_PORT_CHANGE
                    usXC_connected |= (XC1_IN_USE_BY_0 << iTimer);
                    usMode = CLOCK_SOURCE_XC1;
                    break;
                case TIMER_SOURCE_TCLK2:
                    if ((TC_BMR & BMR_XC2_MASK) > XC2_TCLK2) {
                        return;                                          // already in use so not possible
                    }
            #if defined _HW_SAM7SE                                       // {83}
                    PIO_ASR_B  = (TCLK2);
                    PIO_PDR_B  = (TCLK2);
            #else
                    PIO_BSR_A  = (TCLK2_B);
                    PIO_PDR_A  = (TCLK2_B);
            #endif
                    TC_BMR &= ~(BMR_XC2_MASK);                           _SIM_PORT_CHANGE
                    usXC_connected |= (XC2_IN_USE_BY_0 << iTimer);
                    usMode = CLOCK_SOURCE_XC2;
                    break;
                case TIMER_SOURCE_TIOA0:
                    if ((usXC_connected & XC1_IN_USE) && ((TC_BMR & BMR_XC1_MASK) != XC1_TIOA0)) { // preference from XC1
                        if ((usXC_connected & XC2_IN_USE) && ((TC_BMR & BMR_XC2_MASK) != XC2_TIOA0)) { // else XC2
                            return;                                      // connection not possible since all inputs are in use
                        }
                        TC_BMR |= XC2_TIOA0;
                        usXC_connected |= (XC2_IN_USE_BY_0 << iTimer);
                        usMode = CLOCK_SOURCE_XC2;
                    }
                    else {
                        TC_BMR |= XC1_TIOA0;
                        usXC_connected |= (XC1_IN_USE_BY_0 << iTimer);
                        usMode = CLOCK_SOURCE_XC1;
                    }
            #if defined _HW_SAM7S
                    PIO_BSR_A  = (TIOA0_B);
                    PIO_PDR_A  = (TIOA0_B);
            #else
                    PIO_ASR_B  = (TIOA0);
                    PIO_PDR_B  = (TIOA0);
            #endif
                                                                         _SIM_PORT_CHANGE
                    break;
                case TIMER_SOURCE_TIOA1:
                    if ((usXC_connected & XC2_IN_USE) && ((TC_BMR & BMR_XC2_MASK) != XC2_TIOA1)) { // preference from XC2
                        if ((usXC_connected & XC0_IN_USE) && ((TC_BMR & BMR_XC0_MASK) != XC0_TIOA1)) { // else XC0
                            return;                                      // connection not possible since all inputs are in use
                        }
                        TC_BMR |= XC0_TIOA1;
                        usXC_connected |= (XC0_IN_USE_BY_0 << iTimer);
                        usMode = CLOCK_SOURCE_XC0;
                    }
                    else {
                        TC_BMR |= XC2_TIOA1;
                        usXC_connected |= (XC2_IN_USE_BY_0 << iTimer);
                        usMode = CLOCK_SOURCE_XC2;
                    }
            #if defined _HW_SAM7S
                    PIO_BSR_A  = (TIOA1_B);
                    PIO_PDR_A  = (TIOA1_B);
            #else
                    PIO_ASR_B  = (TIOA1);
                    PIO_PDR_B  = (TIOA1);
            #endif
                                                                         _SIM_PORT_CHANGE
                    break;
                case TIMER_SOURCE_TIOA2:
                    if ((usXC_connected & XC0_IN_USE) && ((TC_BMR & BMR_XC0_MASK) != XC0_TIOA2)) { // preference from XC0
                        if ((usXC_connected & XC1_IN_USE) && ((TC_BMR & BMR_XC1_MASK) != XC1_TIOA2)) { // else XC1
                            return;                                      // connection not possible since all inputs are in use
                        }
                        TC_BMR |= XC1_TIOA2;
                        usXC_connected |= (XC1_IN_USE_BY_0 << iTimer);
                        usMode = CLOCK_SOURCE_XC1;

                    }
                    else {
                        TC_BMR |= XC0_TIOA2;
                        usXC_connected |= (XC0_IN_USE_BY_0 << iTimer);
                        usMode = CLOCK_SOURCE_XC0;
                    }
            #if defined _HW_SAM7S
                    PIO_BSR_A  = (TIOA2_B);
                    PIO_PDR_A  = (TIOA2_B);
            #else
                    PIO_ASR_B  = (TIOA2);
                    PIO_PDR_B  = (TIOA2);
            #endif
                                                                         _SIM_PORT_CHANGE
                    break;
                default:
                    return;
                }
                if (_hw_timer_mode[iTimer] & TIMER_SOURCE_FALLING_EDGE) {
                    usMode |= CLKI;
                }
                *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = usMode;
            }
            else {
        #endif
        #if defined _HW_TIMER_MODE
                if (_hw_timer_mode[iTimer] & TIMER_PERIODIC) {
                    usMode &= ~(CPCSTOP | CPCDIS);                           // don't stop on match
                }
        #endif
                *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_5 | usMode);// MCK/1024, wave mode, trigger on register RC, stop after triggering in TC_CMR_X
        #if defined  SUPPORT_TIMER_EXT_CLOCK
            }
        #endif
            *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (((((TIMER_INTERRUPT_SETUP *)ptrSettings)->timer_value) * (MASTER_CLOCK/1024))/1000);  // set ms match value in TC_RC_X
    #endif
            *ptrTimeBlock = (TIM_CLKEN | TIM_SWTRG);                     // start timer by writing to TC_CCR_X
    #if defined _WINDOWS
            *(ptrTimeBlock + (0x20/sizeof(unsigned long))) |= TIM_CLKSTA;// flag that the timer is running (SR)
    #endif
            if (timer_handler[iTimer] != 0) {                            // {77} if handler defined
    #if defined SUPPORT_TIMER_EXT_CLOCK
                if (_hw_timer_mode[iTimer] & EXTERNAL_TIMER_SOURCE) {    // {77} external timer clock source
                    *(ptrTimeBlock + (0x24/sizeof(unsigned long))) = TIM_COVSF; // enable overflow interrupt in TC_IER_X
        #if defined _WINDOWS
                    *(ptrTimeBlock + (0x2c/sizeof(unsigned long))) |= TIM_COVSF; // flag that the interrupt is enabled (IMR)
        #endif
                }
                else {
                    *(ptrTimeBlock + (0x24/sizeof(unsigned long))) = TIM_CPCS; // enable interrupt on RC Compare Interrupt in TC_IER_X
        #if defined _WINDOWS
                    *(ptrTimeBlock + (0x2c/sizeof(unsigned long))) |= TIM_CPCS; // flag that the interrupt is enabled (IMR)
        #endif
                }
    #else
                *(ptrTimeBlock + (0x24/sizeof(unsigned long))) = TIM_CPCS; // enable interrupt on RC Compare Interrupt in TC_IER_X
        #if defined _WINDOWS
                *(ptrTimeBlock + (0x2c/sizeof(unsigned long))) |= TIM_CPCS; // flag that the interrupt is enabled (IMR)
        #endif
    #endif
            }
        }
        break;
#endif
#if defined SUPPORT_ADC                                                  // {47}
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
    #if defined _WINDOWS
                        fnUpdateADC(iFirstValue);                        // cause the simulated input value to be captured
    #endif
                        ((ADC_RESULTS *)(adc_setup->int_adc_result))->sADC_value[iFirstValue] = (signed short)*(ADC_CDR0_ADD + iFirstValue); // the last conversion result
                        ((ADC_RESULTS *)(adc_setup->int_adc_result))->ucADC_status[iFirstValue] = ADC_RESULT_VALID;
    #if defined _WINDOWS
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
                if (adc_setup->int_adc_mode & (ADC_TRIGGERED_TIM0 | ADC_TRIGGERED_TIM1 | ADC_TRIGGERED_TIM2 | ADC_TRIGGERED_EXT)) { // hardware triggered mode
                    if (adc_setup->int_adc_mode & ADC_TRIGGERED_EXT) {
                        ulMode = (TRGEN | TRGSEL_EXT);
    #if defined _HW_SAM7S                                                // {75}
                        PIO_BSR_A  = (ADTRG_B);                          // configure the ADTRG pin
                        PIO_PDR_A  = (ADTRG_B);         _SIM_PORT_CHANGE // ensure simulator registers port changes
    #elif defined _HW_SAM7SE                                             // {83}
                        PIO_ASR_B  = (ADTRG);
                        PIO_PDR_B  = (ADTRG);           _SIM_PORT_CHANGE
    #else
                        PIO_BSR_B  = (ADTRG_B);                          // configure the ADTRG pin
                        PIO_PDR_B  = (ADTRG_B);         _SIM_PORT_CHANGE // ensure simulator registers port changes
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
    #if defined _WINDOWS
                    ADC_CHSR &= ~ADC_CHDR;
                    ADC_CHDR = 0;
    #endif
                }
                else {
                    /*volatile unsigned long ulDummy =*/ ADC_SR;         // read status register to clear pending interrupts
    #if defined _WINDOWS
                    ADC_SR = (ADC_ENDRX | ADC_RXBUFF);
    #endif
                    if (adc_setup->int_adc_int_type & ADC_DISABLE_INTS) {
                        ADC_IDR = adc_setup->int_adc_bits;               // disable interrupts from specified channel(s)
    #if defined _WINDOWS
                        ADC_IMR &= ~ADC_IDR;
                        ADC_IDR = 0;
    #endif
                        break;
                    }
                    else if (adc_setup->int_adc_mode & ADC_LOOP_MODE) {
                        if (adc_setup->int_adc_result != 0) {            // sampling buffer supplied
                            _adc_handler = adc_setup->int_handler;       // enter the user handler
                            fnEnterInterrupt(ADC, adc_setup->int_priority, _adc_sequence_complete);
                                                                         // set up peripheral DMA channel
                            ADC_RPR = (unsigned long)adc_setup->int_adc_result; // transfer start location
                            ADC_RCR = adc_setup->int_samples;            // transfer count
                            ADC_PTCR = PDC_RXTEN;                        // enable transfer
                            ADC_IER = ADC_ENDRX;                         // enable interrupt on transfer complete
    #if defined _WINDOWS
                            ADC_IMR |= ADC_IER;                          // set mask register accordingly
                            ADC_IER = 0;
                            ADC_PTSR |= PDC_RXTEN;
    #endif
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
                            POWER_UP(adc_trigger_source);                // enable clocks to timer in PMC
                            *ptrTimeBlock = (TIM_CLKDIS | TIM_SWTRG);    // disable timer clock and reset timer in TC_CCR_X
                            *(ptrTimeBlock + (0x04/sizeof(unsigned long))) = (TIMER_CLOCK_5 | WAVE | WAVSEL_UP_TRG_RC | ACPC_RC_TOGGLE /*| CPCSTOP | CPCDIS*/);// MCK/1024, wave mode, trigger on register RC, toggle TIOA on match
                            *(ptrTimeBlock + (0x1c/sizeof(unsigned long))) = (((MASTER_CLOCK/1024/2)/(adc_setup->int_adc_sample_rate))); // set match value in TC_RC_X
                            *ptrTimeBlock = (TIM_CLKEN | TIM_SWTRG);     // start timer by writing to TC_CCR_X
                            *(ptrTimeBlock + (0x24/sizeof(unsigned long))) = TIM_CPCS; // enable interrupt on RC Compare Interrupt in TC_IER_X
    #if defined _WINDOWS
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
                        fnEnterInterrupt(ADC, adc_setup->int_priority, _adc_sample_complete);
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
    #if defined _WINDOWS
                        ADC_IMR &= ADC_IDR; ADC_IDR = 0;
                        ADC_IMR |= ADC_IER; ADC_IER = 0;
    #endif
                    }
                    ADC_CHER = adc_setup->int_adc_bits;                  // enable the specified channel(s)
    #if defined _HW_SAM7S                                                // {75}
                    PIO_PUDR_A = ((adc_setup->int_adc_bits & 0x0f) << 17);// if AD inputs on shared lines, disable their pull-up resistors
        #if defined _WINDOWS
                    PIO_PUSR_A |= PIO_PUDR_A; PIO_PUDR_A = 0;
                    ADC_CHSR |= ADC_CHER;     ADC_CHER = 0;
        #endif
    #else
                    PIO_PUDR_B = ((adc_setup->int_adc_bits & 0x0f) << 27);// if AD inputs on shared lines, disable their pull-up resistors
        #if defined _WINDOWS
                    PIO_PUSR_B |= PIO_PUDR_B; PIO_PUDR_B = 0;
                    ADC_CHSR |= ADC_CHER;     ADC_CHER = 0;
        #endif
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
#ifndef _WINDOWS
    PMC_SCDR = PROCESSOR_CLK_ENABLE;                                     // disable the processor clock to enter idle mode
#endif
                                                                         // any enabled interrupt will cause the clock to run and the code to continue
    uEnable_Interrupt();                                                 // enable waiting interrupt routine
}


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    RSTC_CR = (PROCRST | PERRST | RESET_KEY);                            // command reset to core and peripherals
}


#if defined GLOBAL_HARDWARE_TIMER                                        // {3}
static __interrupt void hwtimer_interrupt(void)
{
    static const unsigned char hw_timer_int_message[HEADER_LENGTH] = { INTERNAL_ROUTE, INTERNAL_ROUTE, TASK_TIMER, INTERRUPT_EVENT, 0 };
    volatile unsigned int dummy;

    dummy = HW_TIMER_SR;                                                 // read the status to reset interrupt
    HW_TIMER_IDR = TIM_CPCS;                                             // disable interrupt
    #if defined _WINDOWS
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
    #if defined _WINDOWS
    HW_TIMER_SR |= TIM_CLKSTA; HW_TIMER_IMR |= TIM_CPCS;                 // flag that the timer is running
    #endif

    uEnable_Interrupt();                                                 // enable interrupts again

    return Adjusted;
}

#endif

#ifndef _BOOT_PROGRAM
// The following functions must be writen in ARM mode since they are called directly
// by exception vector
static void AT91F_Spurious_handler(void)
{
}

static void AT91F_Default_IRQ_handler(void)
{
}

static void AT91F_Default_FIQ_handler(void)
{
}

#if defined (_GNU)
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

// Perform very low level SAM7X initialisation - called by the start up code
//
#if defined COMPILE_IAR
    extern void AT91F_LowLevelInit( void) @ "ICODE"
#else
    extern void AT91F_LowLevelInit( void)
#endif
{
    unsigned long *ptrInt;
    int i;

#if MASTER_CLOCK > 30000000
    MC_FMR = (ONE_WAIT_STATE | FLASH_TIME_NORMAL | NEBP);                // set one wait in FLASH since we will be working faster than 30MHz
    #if SIZE_OF_FLASH > (256 * 1024)                                     // {80}
    MC_FMR1 = (ONE_WAIT_STATE | FLASH_TIME_NORMAL | NEBP);               // configure in second FLASH controller in larger parts
    #endif
#else
    MC_FMR = (FLASH_TIME_NORMAL | NEBP);                                 // no wait states in FLASH when not working higher than 30MHz
    #if SIZE_OF_FLASH > (256 * 1024)                                     // {80}
    MC_FMR1 = (FLASH_TIME_NORMAL | NEBP);                                // configure in second FLASH controller in larger parts
    #endif
#endif
    RSTC_MR = (RESET_KEY | URSTEN);                                      // reset input causes a user reset

    if (!(PMC_SR & PLL_LOCK)) {                                          // it may be that the debugger has programmed the PLL so we don't change it
                                                                         // enable the main oscillator in the power management controller
        CKGR_MOR = ((6 << 8) | MOSCEN);                                  // 6 x 8 slow clocks
#if defined _WINDOWS
        PMC_SR |= (MOSCS | PLL_LOCK);
#endif
        while (!(PMC_SR & MOSCS)) {}                                     // wait for start up time to pass before continuing
                                                                         // set the PLL to generate required PLL frequency
#if defined USE_PLL_B
        CKGR_PLLR = (OUT_B | OSC_DIVIDE | ((PLL_MUL - 1) << 16) | (63 << 8)); // {39} allow 63 slow clocks to clock
#else
        CKGR_PLLR = (OSC_DIVIDE | ((PLL_MUL - 1) << 16) | (63 << 8));    // {39} allow 63 slow clocks to clock
#endif
        while (!(PMC_SR & PLL_LOCK)) {}                                  // wait for PLL lock before continuing
        PMC_MCKR = CLK_PRES;                                             // {74} first program the prescale and wait for ready
        while (!(PMC_SR & MCKRDY)) {}                                    // {74} wait for the master clock status to indicate ready
        PMC_MCKR = (SELECT_PLL | CLK_PRES);                              // {39} finally set the master clock to half the PLL value
        while (!(PMC_SR & MCKRDY)) {}                                    // {74} wait for the master clock status to indicate ready
    }

    ptrInt = ADD_AIC_SVR0;                                               // set up the default interrupts handler vectors
    *ptrInt++ = (unsigned long)AT91F_Default_FIQ_handler;                // enter fast interrupt
    for (i = 0; i < 31; i++) {
        *ptrInt++ = (unsigned long)AT91F_Default_IRQ_handler;            // set default handler to all locations
    }
    AIC_SPU = (unsigned long)AT91F_Spurious_handler;                     // enter spurious interrupt handler
#if !defined (_BOOT_PROGRAM) && !defined (_WINDOWS) && defined (COMPILE_IAR) && !defined COMPILE_IAR5 // {46}
    __segment_init();                                                    // initialise variables
#elif defined (_GNU)
    __init_gnu_data();
#endif
#if defined _WINDOWS
    fnUpdateOperatingDetails();                                          // {105} update operating details to be displayed in the simulator
#endif
}
#endif
#endif

/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5223X.c [FREESCALE Coldfire]
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    01.02.2007 Improved Ethernet Duplex and auto-negotiation set up      {1}
    15.03.2007 Add VLAN rx frame length option                           {2}
    16.03.2007 Changes SPI file delete granularity from FILE_GRANULARITY to FLASH_GRANULARITY {3}
    16.03.2007 Use new fnDeleteParBlock instead of fnEraseFlashSector when erasing parameter blocks to increase size flexibility {4}
    01.04.2007 Adapted for GNU compatibility (see define _GNU) and corrected Interrupt mask error {5}
    15.05.2007 Changed to MILLISEC hardware timer method                 {6}
    09.06.2007 Interrupt levels and priorities configurable at project level (app_hw_m5223x.h)
    09.06.2007 RND_HW_SUPPORT enables random numbers from the Random Number Generator Accelerator
    12.06.2007 Changed uMalloc heap start location to support malloc heap{7}
    25.06.2007 IRQ support extended to IRQ1..15 (11 for 80 pin device)   {8}
    10.07.2007 All calls of fnIncrementEthernetStats made conditional on USE_IP_STATS
    11.08.2007 Add SPI FLASH support                                     {9}
    11.08.2007 Add RTC support                                           {10}
    23.08.2007 Rework auto-negotiation mode to control full-duplex mode in PHY {11}
    24.08.2007 Alternative UART pins defines. Alternative forced for UART2 when 80 pin package used {12}
    25.08.2007 Correct I2C mult-read sequence                            {13}
    28.08.2007 Correct RTC interrupt for GNU                             {14}
    05.09.2007 Correct Edge Port IRQ mask configuration                  {15}
    07.09.2007 Correct prototype define to static and an invalid pre-processor systax {16}
    10.09.2007 Correct I2C interrupt blocking                            {16}
    12.09.2007 Add start up delay when using SPI FLASH                   {17}
    12.09.2007 Add extended SPI FLASH support - multiple chips (SPI_FLASH_MULTIPLE_CHIPS) {18}
    15.09.2007 Modify edge port priorities from IRQ8..IRQ15              {19}
    17.09.2007 Correct CAN bus off interrupt priority/level setting      {20} 
    17.09.2007 Remove IRQ1..7 priority/level setting since this is fixed {21}  
    22.09.2007 Add M5221X support
    23.09.2007 Adapt fnEraseFlashSector() and its use in fnDeleteParBlock() {22}
    23.09.2007 Adjust SPI FLASH interface for large devices              {23}
    12.10.2007 Add USB support                                           {24}
    30.10.2007 Add RTS/CTS support                                       {25}
    02.11.2007 Improve status register backup when changing interrupt enables {26}
    02.11.2007 Back up tx data byte so that it can't get corrupted by rx data before simulator use {27}
    15.11.2007 Correct CAN bus frequency for 1MHz M5223X                 {28}
    17.11.2007 Allow SPI and FLASH files system to work together         {29}
    17.11.2007 Enable accurate distinction between simulated and Window's memory {30}
    12.12.2007 Alternative I2C pins defines and more I2C speeds          {31}
    15.12.2007 Inform simulator of I2C speed                             {32}
    27.12.2007 Improve HW Timer operation and make DMA channel configurable {33}
    20.01.2008 Correct Flash state-machine frequency and improve accuracy{34}
    28.01.2008 Don't disable duplex operation in auto-negotiation mode   {35}
    03.02.2008 Never use watchdog when debugging in RAM                  {36}
    22.02.2008 Correct filling SPI FLASH output buffer (optimisation)    {37}
    22.02.2008 Corrected chip select recognition for multiple chips      {38}
    22.02.2008 Reorganise SPI Flash support to use an include file       {39}
    22.02.2008 Add support for ST SPI FLASH chips                        {40}
    23.02.2008 Correct PHY interrupt mask to not unmask GRA interrupt    {41}
    31.03.2008 Add improved PLL configuration and control                {42}
    10.04.2008 Add fnStopHW_Timer() and initialise its count on start up {43}
    10.04.2008 Add ADC support                                           {44}
    18.04.2008 Correct TICK divide value (PIT)                           {45}
    19.04.2008 Improve power management support                          {46}
    19.04.2008 Reject own Ethernet frames when simulating                {47}
    21.04.2008 Add SUPPORT_EPORT to remove EPORT code if not required and SUPPORT_PIT1 for PIT1
    29.04.2008 Add ADC input initial voltages when simulating            {48}
    04.05.2008 Correct CTS/RTS port configurations (thanks Dean!)        {49}
    23.05.2008 UART2 defaults to port AS on 64 pin devices               {50}
    23.05.2008 Add fnGetDMACount() and add UART break support in DMA mode {51}
    23.05.2008 Add 32k crystal support to RTC for M5221X family          {52}
    31.05.2008 Enable first ADC channel when only second ADC channel used, not in parallel mode {53}
    02.06.2008 Set ADC conversion complete after one poll (simulator)    {54}
    04.06.2008 Power up edge port module on use                          {55}
    07.06.2008 M5221X RTC corrections                                    {56}
    07.06.2008 Extend I2C to support multiple interfaces                 {57}
    09.06.2008 Add more flexible parameter block configuration using PARAMETER_BLOCK_START {58}
    09.06.2008 Improve FLASH simulation using fnGetFlashAdd()            {59}
    28.06.2008 Initialise QSPI control registers and remove from driver  {60}
    29.06.2008 Add SST SPI FLASH support                                 {61}
    05.07.2008 Add fnInUserInterrupt()                                   {62}
    05.07.2008 Optionally enable external interrupt configuration        {63}
    31.07.2008 Made SEL_VREFH_AN2 and SEL_VREFL_AN6 settings independent {64}
    13.08.2008 Add M5225X support                                        {65}
    27.08.2008 Modify fnTxI2C Channel parameter to QUEUE_HANDLE          {66}
    08.09.2008 Add additional FLASH memory address write validity when simulating {67}
    14.09.2008 Enable parameter and flash routines when parameter system is enabled without file system support {68}
    17.09.2008 Add USB host mode support                                 {69}
    17.10.2008 Add DMA timer interrupt routine forward declaration for GCC{70}
    25.10.2008 Finalise SST SPI FLASH implementation                     {71}
    29.10.2008 Add general purpose timer interface support               {72}
    03.01.2009 Correct SPI EEPROM code                                   {73}
    07.01.2009 Added UART1 DMA on Tx                                     {74}
    19.01.2009 Changes for Kirin3 operation on EVM/DEMO board            {75}
    25.01.2009 Correct 32k and 64k SST SPI FLASH delete decision         {76}
    27.01.2009 NVRAM support via FlexBus (M5225X)                        {77}
    28.01.2009 Add Backup Watchdog Timer support                         {78}
    12.02.2009 Add IP loop back support for simulations                  {79}
    12.02.2009 Add fnDelayLoop() for simple but accurate us delay loops  {80}
    24.02.2009 Add commandable configuration of individual RTS and CTS pins and use it for general configuration {81}
    24.02.2009 fnControlLine() and fnControlLineInterrupt() operating mode parameter adapted to UART_MODE_CONFIG {82}
    25.02.2009 Modify parameter block dependencies                       {83}
    04.03.2009 Change use of random number seed                          {84}
    05.03.2009 Reset GTP extended counter on GTP power down              {85}
    07.03.2009 Add local static endpoint count to allow fixed config in FLASH {86}
    07.03.2009 Add USB_BDT_FIXED option                                  {87}
    07.03.2009 Correct little/big endian USB conversion to avoid incorrectly handling backdoor conversion. Change fnLE_add() to local macro {88}
    15.03.2009 Add support of relaxation and external oscillator operation {89}
    21.03.2009 Correct UART2 tx DMA code                                 {90}
    21.03.2009 Make use of byterev instruction to convert between little and big-endian addresses {91}
    24.03.2009 fnTxByteDMA() returns the transmission length achieved    {92}
    25.03.2009 Replace OSCCLK by CRYSTAL_FREQ to correct crystal derived frequencies {93}
    25.03.2009 Add M521XX configuration
    26.03.2009 Enable GPT capture input to be used as simple interrupt   {94}
    26.03.2009 Add M521X configuration (replacing _HW_M5213)             {95}
    31.03.2009 Change I2C1_ON_QS to I2C1_ON_QS for consistency           {96}
    03.04.2009 Set automatic UART transmitter deactivation in DMA mode   {97}
    05.04.2009 Warn in the simulator if the user sets a MAC address with no zero first byte {98}
    10.04.2009 Add fnRemainingDMA_tx() and fnAbortTxDMA()                {99}
    10.04.2009 Set busy flag when simulating UART DMA transmission and clear done flag on write {100}
    01.05.2009 Add fnMassEraseFlash() for unsecuring a chip              {101}
    10.05.2009 Increment CAN interrupt protection to support simulation  {102}
    09.06.2009 UART channel pointer made volatile to ensure no problems with optimisation {103}
    10.06.2009 Rename usConfig to Config in UART configuration           {104}
    05.08.2009 Enable transmitter when setting break condition / disable when clearing {105}
    11.09.2009 Add fnGetMultiDropByte()                                  {106}
    02.10.2009 Add support for variables in external RAM                 {107}
    08.10.2009 Enable interrupt before fnUserHWInit()                    {108}
    15.10.2009 Only initialise UART Rx/Tx direction used                 {109}
    17.10.2009 Correct RTC clock frequency (_M5221X, _M521XX, _M5225X) when running from bus clock {110}
    24.10.2009 ADC_CTRL2 value taken directly from passed variable without manipulation (full calculation in ADC_SAMPLING_SPEED macro) {111}
    16.11.2009 Rename SECRET_BIT in RTC to EXTALEN                       {112}
    07.12.2009 Avoid blocking interrupt when sending LAN_REPORT_ACTIVITY event messages {113}
    20.12.2009 Add SD-card support                                       {114}
    20.12.2009 Don't enable RTC until the counter divider has been set   {115}
    23.12.2009 Don't allow I2C transmission to start when the previous telegram's stop condition hasn't completed {116}
    01.02.2010 Add multicast Ethernet configuration and simulation acceptance {117}
    07.02.2010 Protect interrupt masking to avoid possibility of spurious interrupts {118}
    07.03.2010 Remove burst modes from MiniFlexBus setup - should be set to 0 according to latest user's manual {119}
    07.03.2010 Change decision to use USB backdoor address based on end of FLASH rather than start of RAM {120}
    25.03.2010 Handle endpoint stalls on IN endpoint and add fnUnhaltEndpoint() {121}
    10.04.2010 Add M5214, M5216, M5280, M5281 and M5282                  {122}
    10.05.2010 Add MANAGED_FILES support                                 {123}
    13.05.2010 Remove CRC32 length from end of received Ethernet frames  {124}
    19.05.2010 Add opional FNFORCE_PHY_CONFIG() to drive MII lines to configure at reset and allow Ethernet to optionally operate when no PHY detected {125}
    19.05.2010 Add configurable PHY management clock speed and only enable clock while reading/writing to save power and avoid unnecessary clock signal {126}
    10.04.2010 Add M520X                                                 {127}
    26.05.2010 Allow fnGetParsFile() to return embedded files when no file system operation is defined {128}
    02.07.2010 Remove random seed from stack to a fixed location (note that linker script changed accordingly) {129}
    02.08.2010 Ensure autonegotiation disabled in PHY when fixed speed is used {130}
    25.09.2010 Add exception when ADC speed can not be set to desired value and improve ADC interrupt channel used {131}
    30.09.2010 Add PWM interface                                         {132}
    10.01.2011 Handle IRQ7 specially to avoid modification of iInterruptLevel counter {133}
    21.02.2011 Add MONITOR_PERFORMANCE support                           {134}
    20.03.2011 Don't program Flash content with value that is already saved {135}
    23.02.2011 Add IAR compiler support                                  {136}
    27.04.2011 Add Codewarrior 10.1 support                              {137}
    02.05.2011 Workaround for CW7.2 compiler error                       {138}
    04.05.2011 User crystal frequency to set general CAN speed           {139}
    17.06.2011 Add M523X                                                 {140}
    21.08.2011 Adapt RTC interface for full Gregorian time operation     {141}
    21.08.2011 Add fnGetLocalFileTime() to allow utFAT time stamping from RTC {142}
    23.08.2011 Add I2C_EEPROM_FILE_SYSTEM support                        {143}
    25.08.2011 Add revised ETHERNET_RELEASE_AFTER_EVERY_FRAME support    {144}
    14.09.2011 Add M523X Ethernet PHY and add M523X IRQ support          {145}
    19.09.2011 Correct extended file read/write overflow from internal Flash to EEPROM based I2C FLASH {146}
    01.10.2011 Correct reading and writing blocks of data 'straddling' multiple SPI Flash devices {147}
    01.10.2011 Add revised Flash handing as defined by STORAGE_REV_1     {148}
    02.10.2011 Add EXT_FLASH_FILE_SYSTEM support                         {149}
    24.10.2011 Correct internal flash delete in revised flash handling (add sector loop) {150}
    10.11.2011 Add return value to fnConfigEthernet()                    {151}
    17.11.2011 fnEraseFlashSector() returns negative value on failure and the sector size of media when called with zero {152}
    17.11.2011 Allow invalid storage media reads to memory map the access when not simulating (this allows handling retrieval from SRAM without having to define it as a storage area) {153}
    03.12.2011 Add channel number to fnSimCAN()                          {154}
    04.12.2011 Add channel number to fnCAN_tx() and fnCAN_get_rx()       {155}
    02.01.2012 CAN modifications                                         {156}
    08.01.2012 Check internal EPHY on Ethernet initialisation            {157}
    08.01.2012 Add _fnMIIread() and _fnMIIwrite() for debugging PHY      {158}
    29.01.2012 Correct CAN clock setting calculation                     {159}
    20.02.2012 Allow simulator to run flash routines directly rather than from RAM {160}
    25.03.2012 Use link-local IPv6 address for multicast configuration   {161}
    15.05.2012 Correct managed mode delete when working with multiple storage medium {162}
    11.06.2012 Modify compare to avoid CW7.2 bug                         {163}
    28.07.2012 Add DMA timer capture interrupt configuration             {164}
    31.08.2012 Correct DMA timer interval divide setting                 {165}
    10.10.2012 Avoid potential buffer overflow when receiving over-large Ethernet frames using USE_MULTIPLE_BUFFERS {166}
    16.10.2012 Synchronise general timer port inputs to port state       {167}
    30.10.2012 Correct first write to 16 bit external parallel flash when multiple devices are available {168}
    19.10.2012 Exception only on multicast MAC address                   {169}
    10.01.2013 Enter NVRAM to storage area list                          {170}
    20.01.2013 Support mass erase in simulator                           {171}
    20.01.2013 Add managed memory support                                {172}
    15.03.2013 Correct ADC interrupt definitions for GCC                 {173}
    03.07.2013 Add PHY_MULTI_PORT support                                {174}
    03.07.2013 Add PHY SMI mode support                                  {175}
    08.07.2013 Add PHY_POLL_LINK support                                 {176}
    10.08.2013 Avoid resetting PIT pre-scaler when clearing interrupt flag - see https://community.freescale.com/message/71442#71442 {177}
    12.10.2013 Add _MAGIC_RESET_FRAME support                            {178}
    16.10.2013 Disable next page advertisement when internal PHY is used to work around an errata and potential problems with gigabit switches {179} - SECF128
    20.10.2013 Enable UART receiver when configuring DMA reception for the first time {180}
    05.03.2014 Add USB error counters and clear errors in the USB interrupt routine {181}
    06.11.2014 Add automatic I2C lockup detection and recovery           {182}
	26.02.2015 Add power of 2s ATMEL flash support                       {183}
    20.10.2015 Add fnModifyMulticastFilter() for IGMP use                {184}
    20.10.2015 Add UTC option when setting and retrieving time/date      {185}
    20.10.2015 RTC interface reworked and partly moved to time_keeper.c  {186}
    02.11.2015 Modify fnGetUSB_HW() to be compatible with host parameter passing {187}
    02.11.2015 Add simplex USB endpoint support                          {188}
    16.11.2015 Add flash code control using FLASH_ROUTINES
    09.10.2017 Corrected fnSetParameters() to operate with the updated method of committing and initial set of parameters to flash when they are blank. {189}
    16.10.2017 Allow DMA timer to be used without user callback          {190}
    06.06.2018 Change uMemset() to match memset() parameters             {191}
    12.09.2018 Set next USB reception buffer to usb_hardware.ptrRxDatBuffer {192}

*/ 


#if defined _M5223X

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _WINDOWS 
    #include "config.h"
    extern void fnOpenDefaultHostAdapter(void);
    #define __interrupt__ 
    #define SIM_LOCK MCF_CLOCK_SYNSR |= PLLOCKED;
    #define _LITTLE_ENDIAN
    #define SIM_DMA_3 fnSimDMA(0);
    #define BACKDOOR_FLASH 0
    #define EXTERNSTATIC extern
    extern void fec_txf_isr(void);
    #define asm_int_on()
    #define asm_int_off()
    #if defined GLOBAL_HARDWARE_TIMER
    static __interrupt__ void _hwtimer_interrupt(void);                  // {43}
    #endif
    extern unsigned char ucPort_in_TA;                                   // {167}
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define EXTERNSTATIC static
    #define SIM_LOCK
    #define SIM_DMA_3
    #define BACKDOOR_FLASH (CFM_FLASH)
    #include "config.h"

    #if defined _CODE_WARRIOR_10
        #define __interrupt__   __declspec(interrupt)                    // {137}
    #endif

    #if defined _GNU                                                     // GNU interrupt list
        #define __interrupt__ 
        #if !defined BACKUP_WATCHDOG_TIMER                               // {78} not used when BWT configured
        static void _sw_wdog_timeout(void) __attribute__((interrupt_handler));
        #endif
        static void _RealTimeInterrupt(void) __attribute__((interrupt_handler));
        #if defined SUPPORT_PIT1
            static void _PIT1_Interrupt(void) __attribute__((interrupt_handler));
        #endif
        #if defined GLOBAL_HARDWARE_TIMER
            static void _hwtimer_interrupt(void) __attribute__((interrupt_handler));
        #endif
        #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM        // {29}
            static void _QSPI_Interrupt(void) __attribute__((interrupt_handler));
        #endif
        #if defined SUPPORT_RTC && !defined _M521X && !defined _M52XX_SDRAM && !defined _M520X // {14}{122}{127}
            static void _rtc_handler(void) __attribute__((interrupt_handler));
        #endif
        #if defined SUPPORT_ADC && !defined _M52XX_SDRAM && !defined _M520X // {122}{127}{173}
            static void _ADC_convertA_interrupt(void)  __attribute__((interrupt_handler));
            static void _ADC_convertB_interrupt(void)  __attribute__((interrupt_handler));
            static void _ADC_level_interrupt(void)  __attribute__((interrupt_handler));
        #endif
        #if defined SUPPORT_DMA_TIMER                                    // {70}
            static void _dma_timer_int_0(void)  __attribute__((interrupt_handler));
            static void _dma_timer_int_1(void)  __attribute__((interrupt_handler));
            static void _dma_timer_int_2(void)  __attribute__((interrupt_handler));
            static void _dma_timer_int_3(void)  __attribute__((interrupt_handler));
        #endif
        #if defined SUPPORT_GENERAL_PURPOSE_TIMER && !defined _M520X     // {72}
            static void _gp_timer_int_0(void)  __attribute__((interrupt_handler));
            static void _gp_timer_int_1(void)  __attribute__((interrupt_handler));
            static void _gp_timer_int_2(void)  __attribute__((interrupt_handler));
            static void _gp_timer_int_3(void)  __attribute__((interrupt_handler));
            #if defined GPT_EXTENDED_COUNTER
                 static void _GPT_overflow_interrupt(void)  __attribute__((interrupt_handler));
            #endif
        #endif
        #if defined SUPPORT_EPORT
            static void _irq1_handler(void) __attribute__((interrupt_handler));
            static void _irq2_handler(void) __attribute__((interrupt_handler));
            static void _irq3_handler(void) __attribute__((interrupt_handler));
            static void _irq4_handler(void) __attribute__((interrupt_handler));
            static void _irq5_handler(void) __attribute__((interrupt_handler));
            static void _irq6_handler(void) __attribute__((interrupt_handler));    
            static void _irq7_handler(void) __attribute__((interrupt_handler));
            #if !defined CHIP_80_PIN && !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X && !defined _M52XX_SDRAM // {8}{122}
                static void _irq8_handler(void) __attribute__((interrupt_handler));
                static void _irq9_handler(void) __attribute__((interrupt_handler));
                static void _irq10_handler(void) __attribute__((interrupt_handler));
            #endif
            #if !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X && !defined _M52XX_SDRAM // {8}{122}
                static void _irq11_handler(void) __attribute__((interrupt_handler));
            #endif
            #if !defined CHIP_80_PIN && !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X && !defined _M52XX_SDRAM // {8}{122}
                static void _irq12_handler(void) __attribute__((interrupt_handler));
                static void _irq13_handler(void) __attribute__((interrupt_handler));
                static void _irq14_handler(void) __attribute__((interrupt_handler));
                static void _irq15_handler(void) __attribute__((interrupt_handler));
            #endif
        #endif
        #if defined ETH_INTERFACE
            static void _fec_txf_isr(void) __attribute__((interrupt_handler));
            static void _fec_rx_frame_isr(void) __attribute__((interrupt_handler));    
            #if !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X // {127}
                static void _ephy_isr(void) __attribute__((interrupt_handler)); 
            #endif
        #endif
        #if defined SERIAL_SUPPORT_DMA
            static void _DMA0_Tx_handler(void) __attribute__((interrupt_handler));
            static void _DMA1_Tx_handler(void) __attribute__((interrupt_handler)); // {74}
            static void _DMA2_Tx_handler(void) __attribute__((interrupt_handler));
            static void _DMA1_Rx_handler(void) __attribute__((interrupt_handler));    
        #endif
        #if defined SERIAL_INTERFACE
            static void _SCI0_Interrupt(void) __attribute__((interrupt_handler));
            static void _SCI1_Interrupt(void) __attribute__((interrupt_handler));
            static void _SCI2_Interrupt(void) __attribute__((interrupt_handler));
        #endif
        #if defined I2C_INTERFACE
            static void _I2C_Interrupt(void) __attribute__((interrupt_handler));
            #if NUMBER_I2C > 1                                           // {57}
            static void _I2C_Interrupt_1(void) __attribute__((interrupt_handler));            
            #endif
        #endif
        #if defined CAN_INTERFACE
            static void _CAN_buf_Interrupt_0(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_1(void) __attribute__((interrupt_handler));        
            static void _CAN_buf_Interrupt_2(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_3(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_4(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_5(void) __attribute__((interrupt_handler));       
            static void _CAN_buf_Interrupt_6(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_7(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_8(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_9(void) __attribute__((interrupt_handler));        
            static void _CAN_buf_Interrupt_10(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_11(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_12(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_13(void) __attribute__((interrupt_handler));                 
            static void _CAN_buf_Interrupt_14(void) __attribute__((interrupt_handler));
            static void _CAN_buf_Interrupt_15(void) __attribute__((interrupt_handler));
            static void _CAN_bus_off_Interrupt(void) __attribute__((interrupt_handler));
            static void _CAN_error_Interrupt(void) __attribute__((interrupt_handler));
        #endif
        #if defined USB_INTERFACE
            static void _usb_otg_isr(void) __attribute__((interrupt_handler));
        #endif
    #elif defined _COMPILE_IAR                                           // {136}
        #include <intrinsics.h>
        #define __interrupt__  __interrupt
        #define asm_int_off()  __disable_interrupts()                    // IAR intrinsic
        #define asm_int_on()   __enable_interrupts()                     // IAR intrinsic
        __no_init unsigned long __VECTOR_RAM[256] @ "VECTOR_RAM";        // vector table reserved at start of SRAM
    #else
        #if defined GLOBAL_HARDWARE_TIMER
        static __interrupt__ void _hwtimer_interrupt(void);              // {43}
        #endif
    #endif
#endif  

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define SPI_FLASH_ENABLED 
#endif


#if defined I2C_EEPROM_FILE_SYSTEM
    #define _I2C_EEPROM_DEFINES
    #include "i2C_eeprom_m24m01.h"
    #undef _I2C_EEPROM_DEFINES
#endif

#if !defined SIZE_OF_NVRAM                                               // {77}
    #define SIZE_OF_NVRAM 0                                              // if no NVRAM defined set size to zero
#endif

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

// CAN interface queues
//
typedef struct stCANQue
{
    unsigned long  ulPartnerID;                                          // default transmission ID (msb signifies extended)
    unsigned long  ulOwnID;                                              // {156} our own ID (msb signifies extended)
    QUEUE_HANDLE   DriverID;                                             // handle this buffer belongs to
    unsigned char  ucMode;
    unsigned char  ucErrors;                                             // tx error counter per buffer
    UTASK_TASK     TaskToWake;                                           // task responsible for processing rx frames, errors or tx frame complete
} CANQUE;

typedef struct stUSB_ERRORS                                              // {181}
{
    unsigned long ulUSB_errors_CRC5;
    unsigned long ulUSB_errors_CRC16;
} USB_ERRORS;

#if !(defined _M5225X || defined _M52XX_SDRAM || defined _M523X)         // don't stop MDIO clock when using EPHY since it stops interrupt generation
    #define PHY_IDENTIFIER (0x002c1811)                                  // expected ID of internal EPHY
#else
    #define _STOP_MDIO_CLOCK                                             // {126} the ethernet MDIO clock is stopped between data transfers (only remove for testing with clock always on)
#endif

#if !defined USB_FS_MALLOC
    #define USB_FS_MALLOC(x) uMalloc((MAX_MALLOC)(x)) 
#endif
#if !defined USB_FS_MALLOC_ALIGN
    #define USB_FS_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined ETH_INTERFACE && !defined INTERRUPT_TASK_PHY
    static UTASK_TASK EthernetWake = 0;                                  // task to wake on ethernet events
#endif
#if defined ETH_INTERFACE && defined _MAGIC_RESET_FRAME/* && !defined USE_MULTIPLE_BUFFERS*/ // {178}
    static volatile M5223X_FEC_BD *ptrMagicRxBd = 0;                    // initially disabled
#endif
#if defined ETH_INTERFACE
    #if defined USE_IGMP && !defined NO_INTERNAL_ETHERNET                // {184}
        #if defined USE_IPV6
            #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 2)
        #else
            #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 1)
        #endif
        static unsigned char ucGroupHashes[MAX_MULTICAST_FILTERS] = {0};
    #endif
#endif
#if defined CAN_INTERFACE
    static CANQUE can_queue[NUMBER_CAN_MESSAGE_BUFFERS] = {{0}};
#endif
static int iInterruptLevel = 0;

#if defined (RANDOM_NUMBER_GENERATOR) && !defined (RND_HW_SUPPORT)
    static unsigned short *ptrSeed;                                      // uninitialised variable for random value
#endif

#if defined SERIAL_INTERFACE
    EXTERNSTATIC unsigned char ucEnabledState[NUMBER_SERIAL] = {0};      // for each possible UART maintain a backup of present state
    #if defined _WINDOWS
    extern unsigned char ucTxLast[NUMBER_SERIAL] = {0};                  // {27}
    #endif
#endif

#if defined SPI_FLASH_ENABLED                                            // {9} SPI FLASH for SW upload {29}
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // {18}
        unsigned char ucChipSelect[SPI_FLASH_DEVICE_COUNT] = {
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
        #if defined STORAGE_REV_1                                        // {148}
            #define _EXTENDED_CS  ptrAccessDetails->ucDeviceNumber,
        #else
            #define _EXTENDED_CS  iChipSelect,
        #endif
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif

#if defined USB_INTERFACE                                                // {24}
    static M5222X_USB_ENDPOINT_BD *ptrBDT;
    static USB_END_POINT *usb_endpoints;
    static USB_ERRORS USB_errors = {0};                                  // {181}
#endif

#if defined STORAGE_REV_1 && !defined ONLY_INTERNAL_FLASH_STORAGE        // {148}
    #if (defined _M523X || defined _M520X) && defined EXT_FLASH_FILE_SYSTEM // devices without internal flash - if external Flash enabled put default to it
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

    #if defined NVRAM                                                    // {170}
    static const STORAGE_AREA_ENTRY nvram_storage = {
        (void *)&default_flash,                                          // link to internal flash
        (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),          // nvram area starts after internal flash
        (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SIZE_OF_NVRAM - 1)),
        _STORAGE_NVRAM,                                                  // type
        1                                                                // single device
    };
    #endif
#endif


#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    static void fnConfigSPIFileSystem(unsigned char ucSpeed); 
    #if defined SPI_FLASH_ENABLED                                        // {29}
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined {40}
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_m5223x_atmel.h"                          // {39}
            #include "spi_flash_m5223x_stmicro.h"                        // {40}
            #include "spi_flash_m5223x_sst25.h"                          // {61}
        #undef _SPI_DEFINES
    #endif
#endif
/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */
#if defined ETH_INTERFACE
    extern ETHERNETQue *eth_tx_control;                                  // ethernet structure pointers for Ethernet Interrupt handling
    extern ETHERNETQue *eth_rx_control;

    static volatile M5223X_FEC_BD *ptrRxBd, *ptrTxBd;
#endif


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#if !defined _WINDOWS
    static void fnInitHW(void);
#endif


/********************************************************************/



/* =================================================================== */
/*                      MAIN ENTRY CODE                                */
/* =================================================================== */


#if !defined _WINDOWS
    #if defined (_CODE_WARRIOR)
        #if defined EXTERNAL_RAM && defined _M5225X                      // {107}  
            extern unsigned char __INTERNAL_RAM;
            #define HEAP_START_ADDRESS &__INTERNAL_RAM                   // heap is positioned in internal SRAM whereas variables are in external RAM
        #else
            extern unsigned char __HEAP_END;
            #define HEAP_START_ADDRESS &__HEAP_END                       // {7}                 
        #endif
    #elif defined _COMPILE_IAR                                           // {136}
        #pragma segment="FAR_Z"          
        #define HEAP_START_ADDRESS __sfe("FAR_Z")                        // IAR compiler - last location of static variables
    #else
        extern unsigned char _end; 
        #if defined GNU_MALLOC_SIZE
            #define HEAP_START_ADDRESS (&_end + GNU_MALLOC_SIZE)         // GNU last location of static variables
            void *__heap_limit = HEAP_START_ADDRESS;                     // inform GCC malloc where the heap limit is {7}
        #else
            #define HEAP_START_ADDRESS &_end
        #endif
    #endif 

// Main entry for the target code
//
extern int main(void) 
{
#if defined MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart = HEAP_START_ADDRESS;
#endif
#if defined (RANDOM_NUMBER_GENERATOR) && !defined (RND_HW_SUPPORT)
  //unsigned short usRandomSeed;                                         // we put an uninitialised variable on to the stack for use as a random seed
    ptrSeed = RANDOM_SEED_LOCATION; // &usRandomSeed;                    // {129}
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
#endif 
 
#if defined RANDOM_NUMBER_GENERATOR
// How the random number seed is set depends on the hardware possibilities available.
//
extern void fnInitialiseRND(unsigned short *usSeedValue)                 // {84}
{
    #if defined RND_HW_SUPPORT                                           // use the Random Number Generator Accelerator to do this
    RNGER = 0x1234;                                                      // additional entropy (not really needed)
        #if defined RND_HW_SLEEP
    RNGCR = (RND_CI | RND_IM | RND_SLM);                                 // set sleep mode
        #else
    RNGCR = (RND_CI | RND_IM | RND_GO);                                  // start generation of first number (ready after 256 cycles)
        #endif
    #else
    if ((*usSeedValue = *ptrSeed) == 0) {                                // we hope that the content of the random seed is random after a power up
        *usSeedValue = 0x127b;                                           // we must not set a zero - so we set a fixed value
    }                                                                    // after a reset it should be well random since it will contain the value at the reset time
    *ptrSeed = fnRandom();                                               // ensure that the seed value is modified between resets
    #endif
}

    #if defined RND_HW_SUPPORT
// Get a random number from the RNGA
//
extern unsigned short fnGetRndHW(void)
{
#if defined RND_HW_SLEEP
    unsigned short usRndNumber;
    #if defined _WINDOWS
    RNGOUT = rand();
    RNGSR |= RND_ORL_MASK;                                               // simulate number ready
    #endif
    RNGCR = (RND_CI | RND_IM | RND_GO);                                  // remove from sleep mode
    while (!(RNGSR & RND_ORL_MASK)) {}                                   // wait for a random number to be available (usually already waiting - max. 256 cycles after GO)
    usRndNumber = (unsigned short)RNGOUT;
    RNGCR = (RND_CI | RND_IM | RND_GO | RND_SLM);                        // set sleep mode again
    return usRndNumber;
#else
    #if defined _WINDOWS
    RNGOUT = rand();
    RNGSR |= RND_ORL_MASK;                                               // simulate number ready
    #endif
    while (!(RNGSR & RND_ORL_MASK)) {}                                   // wait for a random number to be available (usually already waiting - max. 256 cycles after GO)

    return ((unsigned short)RNGOUT);                                     // return the random value as 16 bit value
#endif
}
    #endif
#endif

#if defined _WINDOWS
    extern void mcf52235_init(void);
#endif

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    RESET_RCR = SOFT_RST;                                                // command a soft reset of the board
}

/**************************** Software Watchdog has timed out ************************************/
#if !defined BACKUP_WATCHDOG_TIMER                                       // {78} not used when BWT configured
static __interrupt__ void _sw_wdog_timeout(void)
{
    RESET_RCR = SOFT_RST;                                                // command a soft reset of the board
}
#endif

extern void fnDelayLoop(unsigned long ulDelay_us)                        // {80}
{
    #if defined FLASH_SPECULATION_ERROR_SOLVED
        #define LOOP_FACTOR 10310000
    #else
        #define LOOP_FACTOR 12000000
    #endif
    volatile unsigned long ulDelay = ((BUS_CLOCK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay--) {}                                                 // simple loop tuned to perform us timing
}

// Enter a vector in the vector table in SRAM, returning old value
//
#if defined _EXTERNAL_INTERRUPTS                                         // {63}
    extern unsigned char *fnSetIntHandler(int iVectNumber, unsigned char *new_handler)
#else
    static unsigned char *fnSetIntHandler(int iVectNumber, unsigned char *new_handler)
#endif
{
    extern unsigned long __VECTOR_RAM[];
    unsigned char *old_handler;
    
    old_handler = (unsigned char *)__VECTOR_RAM[iVectNumber];
    __VECTOR_RAM[iVectNumber] = (unsigned long)new_handler;
    return old_handler;     
}

#if (defined NVRAM || defined EXTERNAL_RAM) && defined _M5225X           // {77}{107} FlexBus
// Configuration of FlexBus for MRAM use. This serves as reference in case of a requirement for a more general interface
//
static void fnConfigureBus(int iSpeed, int iWaitStates)
{
    unsigned long ulMask = 0x0000ffff;
    unsigned long ulMaskPower = 0x00010000;
    #if defined NVRAM
    unsigned long ulSize = SIZE_OF_NVRAM;
    #else
    unsigned long ulSize = SIZE_OF_EXTERNAL_RAM;                         // {107}
    #endif
    #if defined USB_BIGENDIAN
    unsigned short usExtendedMode = USB_END;
    #else
    unsigned short usExtendedMode = 0;
    #endif
    POWER_UP(POWER_MINI_FLEXBUS);                                        // power up the mini-flex bus module
    if (iSpeed != 0) {                                                   // FlexBus should be clocked from system clock rather than half clock
        usExtendedMode |= MBMOD;
    }
                                                                         // configure the port lines for non-multiplexed FlexBus (A0..A7)
    PTEPAR = (TE_FB_ADD_0_FUNCTION | TE_FB_ADD_1_FUNCTION | TE_FB_ADD_2_FUNCTION | TE_FB_ADD_3_FUNCTION | TE_FB_ADD_4_FUNCTION | TE_FB_ADD_5_FUNCTION | TE_FB_ADD_6_FUNCTION | TE_FB_ADD_7_FUNCTION);
                                                                         // address A8..A15
    PTFPAR = (TF_FB_ADD_8_FUNCTION | TF_FB_ADD_9_FUNCTION | TF_FB_ADD_10_FUNCTION | TF_FB_ADD_11_FUNCTION | TF_FB_ADD_12_FUNCTION | TF_FB_ADD_13_FUNCTION | TF_FB_ADD_14_FUNCTION | TF_FB_ADD_15_FUNCTION);
                                                                         // A16..A19, OE, RW, CS_0
    PTGPAR = (TG_FB_ADD_16_FUNCTION | TG_FB_ADD_17_FUNCTION | TG_FB_ADD_18_FUNCTION | TG_FB_ADD_19_FUNCTION | TG_FB_CS0_FUNCTION | TG_FB_OE_FUNCTION | TG_FB_RW_FUNCTION);
                                                                         // D0..D7
    PTHPAR = (TH_FB_D0_FUNCTION | TH_FB_D1_FUNCTION | TH_FB_D2_FUNCTION | TH_FB_D3_FUNCTION | TH_FB_D4_FUNCTION | TH_FB_D5_FUNCTION | TH_FB_D6_FUNCTION | TH_FB_D7_FUNCTION);
                                                                         // configure the chip select
    CSAR0 = (FLASH_START_ADDRESS + SIZE_OF_FLASH);                       // position immediately after internal FLASH in the memory map
    while (ulSize > ulMaskPower) {
        ulMask <<= 1;
        ulSize -= ulMaskPower;
        ulMaskPower <<= 1;
    }
    CSMR0 = ((ulMask & BAM_MASK) | FB_CS_VALID);
    CSCR0 = (PORT_SIZE_8 | FB_AA /*| BSTW | BSTR*/ | ((iWaitStates << WAIT_STATE_SHIFT) & WAIT_STATE_MASK)); // {119}

    CCM_CCE = usExtendedMode;                                            // set the extended setting - note that this register can only be written once after a reset!
}
#endif

#if defined EXT_FLASH_FILE_SYSTEM                                        // {149}
    #if EXT_PARALLEL_FLASH_OFFSET == 0 && !defined _WINDOWS              // if the external parallel Flash has no virtual offset it can be accessed directly
        #define fnGetExtFlashAdd(x) x
        #define fnPutExtFlashAdd(x) x
    #else
// Routine to map real external hardware address to simulated external FLASH
//
extern unsigned char *fnGetExtFlashAdd(unsigned char *ucAdd)
{
    #if defined _WINDOWS
    unsigned char *ptrSimFlash = fnGetExtFlashStart();
    ptrSimFlash = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSimFlash + (CAST_POINTER_ARITHMETIC)ucAdd);
    ptrSimFlash -= EXTERNAL_FLASH_START_ADDRESS;
    return (ptrSimFlash);
    #else
    return (ucAdd + EXT_PARALLEL_FLASH_OFFSET);
    #endif
}
// Inverse of fnGetExtFlashAdd 
//
extern unsigned char *fnPutExtFlashAdd(unsigned char *ucAdd)
{
    #if defined _WINDOWS
    unsigned char *ptrSimFlash = fnGetExtFlashStart();
    ptrSimFlash = (unsigned char *)((CAST_POINTER_ARITHMETIC)ucAdd - (CAST_POINTER_ARITHMETIC)ptrSimFlash);
    ptrSimFlash += EXTERNAL_FLASH_START_ADDRESS;
    return (ptrSimFlash);
    #else
    return (ucAdd - EXT_PARALLEL_FLASH_OFFSET);
    #endif
}
#endif

#define _M29W160EB_BLOCK_ADDRESS_0  0x00000000                           // 16k boot code sector
#define _M29W160EB_BLOCK_ADDRESS_1  0x00004000                           // 8k parameter sector
#define _M29W160EB_BLOCK_ADDRESS_2  0x00006000                           // 8k parameter sector
#define _M29W160EB_BLOCK_ADDRESS_3  0x00008000                           // 32k parameter sector
#define _M29W160EB_BLOCK_ADDRESS_4  0x00010000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_5  0x00020000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_6  0x00030000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_7  0x00040000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_8  0x00050000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_9  0x00060000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_10 0x00070000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_11 0x00080000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_12 0x00090000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_13 0x000a0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_14 0x000b0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_15 0x000c0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_16 0x000d0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_17 0x000e0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_18 0x000f0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_19 0x00100000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_20 0x00110000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_21 0x00120000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_22 0x00130000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_23 0x00140000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_24 0x00150000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_25 0x00160000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_26 0x00170000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_27 0x00180000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_28 0x00190000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_29 0x001a0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_30 0x001b0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_31 0x001c0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_32 0x001d0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_33 0x001e0000                           // 64k code sector
#define _M29W160EB_BLOCK_ADDRESS_34 0x001f0000                           // 64k code sector

#define _M29W160EB_BOOT_SECTOR_SIZE            (16 * 1024)
#define _M29W160EB_SMALL_PARAMETER_SECTOR_SIZE (8 * 1024)
#define _M29W160EB_LARGE_PARAMETER_SECTOR_SIZE (32 * 1024)
#define _M29W160EB_CODE_SECTOR_SIZE            (64 * 1024)

#define _M29W160_MANUFACTURER_CODE  0x0020
#define _M29W160EB_DEVICE_CODE      0x2249

#define _M29W160EB_SIZE             (2 * 1024 * 1024)                    // 2 Meg x 8 bits (or 1Meg x 16 bits)

#define _S29JL032_32_BLOCK_ADDRESS_0  0x00000000                         // 8k boot code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_1  0x00002000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_2  0x00004000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_3  0x00006000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_4  0x00008000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_5  0x0000a000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_6  0x0000c000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_7  0x0000e000                         // 8k parameter sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_8  0x00010000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_9  0x00020000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_10 0x00030000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_11 0x00040000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_12 0x00050000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_13 0x00060000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_14 0x00070000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_16 0x00080000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_17 0x00090000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_18 0x000a0000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_19 0x000b0000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_20 0x000c0000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_21 0x000d0000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_22 0x000e0000                         // 64k code sector - bank 1
#define _S29JL032_32_BLOCK_ADDRESS_23 0x000f0000                         // 64k code sector - bank 1

#define _S29JL032_32_BLOCK_ADDRESS_24 0x00100000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_25 0x00110000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_26 0x00120000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_27 0x00130000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_28 0x00140000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_29 0x00150000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_30 0x00160000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_31 0x00170000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_32 0x00180000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_33 0x00190000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_34 0x001a0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_35 0x001b0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_36 0x001c0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_37 0x001d0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_38 0x001e0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_39 0x001f0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_40 0x00200000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_41 0x00210000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_42 0x00220000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_43 0x00230000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_44 0x00240000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_45 0x00250000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_46 0x00260000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_47 0x00270000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_48 0x00280000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_49 0x00290000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_50 0x002a0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_51 0x002b0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_52 0x002c0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_53 0x002d0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_54 0x002e0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_55 0x002f0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_56 0x00300000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_57 0x00310000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_58 0x00320000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_59 0x00330000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_60 0x00340000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_61 0x00350000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_62 0x00360000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_63 0x00370000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_64 0x00380000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_65 0x00390000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_66 0x003a0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_67 0x003b0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_68 0x003c0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_69 0x003d0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_70 0x003e0000                         // 64k code sector - bank 2
#define _S29JL032_32_BLOCK_ADDRESS_71 0x003f0000                         // 64k code sector - bank 2

#define _S29JL032_32_PARAMETER_SECTOR_SIZE (8 * 1024)
#define _S29JL032_32_CODE_SECTOR_SIZE      (64 * 1024)

#define _S29JL032_32_MANUFACTURER_CODE 0x0001
#define _S29JL032_32_DEVICE_CODE       0x2253

#define _S29JL032_32_SIZE             (4 * 1024 * 1024)                  // 4 Meg x 8 bits (or 2Meg x 16 bits) - 1Meg in bank 1 - 3 Megs in bank 2

#define EXT_FLASH_STATUS_REGISTER_ERASE TOGGLE   0x04
#define EXT_FLASH_STATUS_REGISTER_DQ3            0x08
#define EXT_FLASH_STATUS_REGISTER_PROG_ERR       0x20
#define EXT_FLASH_STATUS_REGISTER_TOGGLE         0x40
#define EXT_FLASH_STATUS_REGISTER_NOT_DQ7        0x80

#define EXT_FLASH_BYTE_PROGRAM_CMD_0_ADDR        0x0aaa                  // byte programming can be used when the flash is configured in word or byte mode, however the width of the actual unit programmed is accoring to the mode
#define EXT_FLASH_BYTE_PROGRAM_CMD_0_DATA        0xaa
#define EXT_FLASH_BYTE_PROGRAM_CMD_1_ADDR        0x0555
#define EXT_FLASH_BYTE_PROGRAM_CMD_1_DATA        0x55
#define EXT_FLASH_BYTE_PROGRAM_CMD_2_ADDR        0x0aaa
#define EXT_FLASH_BYTE_PROGRAM_CMD_2_DATA        0xa0

#define EXT_FLASH_WORD_PROGRAM_CMD_0_ADDR        0x0555                  // word programming is only possible when the flash chip is configured in word mode
#define EXT_FLASH_WORD_PROGRAM_CMD_0_DATA        0xaa
#define EXT_FLASH_WORD_PROGRAM_CMD_1_ADDR        0x02aa
#define EXT_FLASH_WORD_PROGRAM_CMD_1_DATA        0x55
#define EXT_FLASH_WORD_PROGRAM_CMD_2_ADDR        0x0555
#define EXT_FLASH_WORD_PROGRAM_CMD_2_DATA        0xa0

#define EXT_FLASH_BLOCK_ERASE_CMD_0_ADDR         0x0aaa
#define EXT_FLASH_BLOCK_ERASE_CMD_0_DATA         0xaa
#define EXT_FLASH_BLOCK_ERASE_CMD_1_ADDR         0x0555
#define EXT_FLASH_BLOCK_ERASE_CMD_1_DATA         0x55
#define EXT_FLASH_BLOCK_ERASE_CMD_2_ADDR         0x0aaa
#define EXT_FLASH_BLOCK_ERASE_CMD_2_DATA         0x80
#define EXT_FLASH_BLOCK_ERASE_CMD_3_ADDR         0x0aaa
#define EXT_FLASH_BLOCK_ERASE_CMD_3_DATA         0xaa
#define EXT_FLASH_BLOCK_ERASE_CMD_4_ADDR         0x0555
#define EXT_FLASH_BLOCK_ERASE_CMD_4_DATA         0x55
#define EXT_FLASH_BLOCK_ERASE_CMD_5_DATA         0x30

#define EXT_FLASH_READ_RESET                     0xf0


#if defined EXT_FLASH_16_BIT
    typedef unsigned short    EXT_FLASH_ACCESS_UNIT;                     // writes to flash are 16 bit
#else
    typedef unsigned char     EXT_FLASH_ACCESS_UNIT;
#endif

static int (*_fnWaitExtFlashOperation)(volatile EXT_FLASH_ACCESS_UNIT *, EXT_FLASH_ACCESS_UNIT) = 0;
static int (*_fnExtFlashErase)(volatile unsigned char *, volatile unsigned char *) = 0;
static int (*_fnExtFlashWrite)(volatile unsigned char *, volatile EXT_FLASH_ACCESS_UNIT *, EXT_FLASH_ACCESS_UNIT) = 0;


// This algorithm uses the polling algorithm to detect programming/erase completion and recognise programming/erase errors
// Reference code - copied to RAM and executed from there
//
static int fnWaitExtFlashOperation(volatile EXT_FLASH_ACCESS_UNIT *ptrData, EXT_FLASH_ACCESS_UNIT Check)
{
    EXT_FLASH_ACCESS_UNIT Status;
    while (((Status = *ptrData) & EXT_FLASH_STATUS_REGISTER_NOT_DQ7) != (Check & EXT_FLASH_STATUS_REGISTER_NOT_DQ7)) { // programming not completed
        if (Status & EXT_FLASH_STATUS_REGISTER_PROG_ERR) {               // check for possible programming/erase error
            if ((*ptrData & EXT_FLASH_STATUS_REGISTER_NOT_DQ7) != (Check & EXT_FLASH_STATUS_REGISTER_NOT_DQ7)) { // verify that it is really an error
                *ptrData = EXT_FLASH_READ_RESET;                         // reset the flash back to the read mode
                return -1;                                               // programming error
            }
            else {
                break;                                                   // successful
            }
        }
    }
    return 0;
}

// Reference code - copied to RAM and executed from there
//
static int fnExtFlashWrite(volatile unsigned char *ptrCmdAdd, volatile EXT_FLASH_ACCESS_UNIT *ptrData, EXT_FLASH_ACCESS_UNIT Value)
{
    #if !defined _WINDOWS
        #if defined EXT_FLASH_16_BIT
    *(volatile unsigned short *)(ptrCmdAdd + (EXT_FLASH_WORD_PROGRAM_CMD_0_ADDR * 2)) = EXT_FLASH_WORD_PROGRAM_CMD_0_DATA;
    *(volatile unsigned short *)(ptrCmdAdd + (EXT_FLASH_WORD_PROGRAM_CMD_1_ADDR * 2)) = EXT_FLASH_WORD_PROGRAM_CMD_1_DATA;
    *(volatile unsigned short *)(ptrCmdAdd + (EXT_FLASH_WORD_PROGRAM_CMD_2_ADDR * 2)) = EXT_FLASH_WORD_PROGRAM_CMD_2_DATA;
        #else
    *(ptrCmdAdd + EXT_FLASH_BYTE_PROGRAM_CMD_0_ADDR) = EXT_FLASH_BYTE_PROGRAM_CMD_0_DATA;
    *(ptrCmdAdd + EXT_FLASH_BYTE_PROGRAM_CMD_1_ADDR) = EXT_FLASH_BYTE_PROGRAM_CMD_1_DATA;
    *(ptrCmdAdd + EXT_FLASH_BYTE_PROGRAM_CMD_2_ADDR) = EXT_FLASH_BYTE_PROGRAM_CMD_2_DATA;
        #endif
    *ptrData = Value;                                                    // write the byte/word to be programmed
    #else
        #if defined EXT_FLASH_16_BIT
    if (Value & ~(*ptrData)) {                                           // simulator check to ensure no programmed bits are being attempted to be set back to '1'
        _EXCEPTION("attempt to write a flash bit from '0' to '1'!!");
    }
    *ptrData &= Value;                                                   // program the byte/word to simulated flash (only program bits to zero)
    Value = *ptrData;
    ptrData = (volatile EXT_FLASH_ACCESS_UNIT *)fnPutExtFlashAdd((unsigned char *)ptrData);
        #else
    *fnGetExtFlashAdd((unsigned char *)ptrData) = Value;                 // program the byte to simulated flash
        #endif
    #endif

    // Wait for command to complete
    //
    if (_fnWaitExtFlashOperation((volatile EXT_FLASH_ACCESS_UNIT *)fnGetExtFlashAdd((unsigned char *)ptrData), Value) < 0) {
        return -1;
    }
    return 0;
}

// Reference code - copied to RAM and executed from there
//
static int fnExtFlashErase(volatile unsigned char *ptrCmdAdd, volatile unsigned char *ptrData)
{
    #if !defined _WINDOWS
    *(ptrCmdAdd + EXT_FLASH_BLOCK_ERASE_CMD_0_ADDR) = EXT_FLASH_BLOCK_ERASE_CMD_0_DATA;
    *(ptrCmdAdd + EXT_FLASH_BLOCK_ERASE_CMD_1_ADDR) = EXT_FLASH_BLOCK_ERASE_CMD_1_DATA;
    *(ptrCmdAdd + EXT_FLASH_BLOCK_ERASE_CMD_2_ADDR) = EXT_FLASH_BLOCK_ERASE_CMD_2_DATA;
    *(ptrCmdAdd + EXT_FLASH_BLOCK_ERASE_CMD_3_ADDR) = EXT_FLASH_BLOCK_ERASE_CMD_3_DATA;
    *(ptrCmdAdd + EXT_FLASH_BLOCK_ERASE_CMD_4_ADDR) = EXT_FLASH_BLOCK_ERASE_CMD_4_DATA;
    *ptrData = EXT_FLASH_BLOCK_ERASE_CMD_5_DATA;                         // start block erase command
    #else
    unsigned long ulOffset = (unsigned long)ptrData;
    unsigned long ulChip = (unsigned long)ptrCmdAdd;
    unsigned long ulSectorLength;
    do {
        ulChip -= EXTERNAL_FLASH_START_ADDRESS;
    } while (ulChip > SIZE_OF_EXTERNAL_FLASH_CHIP);
        #if defined _S29JL032_32
    if (ulOffset < _S29JL032_32_BLOCK_ADDRESS_8) {
        ulSectorLength = (_S29JL032_32_PARAMETER_SECTOR_SIZE);
    }
    else {
        ulSectorLength = (_S29JL032_32_CODE_SECTOR_SIZE);
    }
        #else
    if (ulOffset < _M29W160EB_BLOCK_ADDRESS_4) {
        if (ulOffset < _M29W160EB_BLOCK_ADDRESS_1) {
            ulSectorLength = (_M29W160EB_BOOT_SECTOR_SIZE);
        }
        else if (ulOffset < _M29W160EB_BLOCK_ADDRESS_3) {
            ulSectorLength = (_M29W160EB_SMALL_PARAMETER_SECTOR_SIZE);
        }
        else {
            ulSectorLength = (_M29W160EB_LARGE_PARAMETER_SECTOR_SIZE);
        }
    }
    else {
        ulSectorLength = (_M29W160EB_CODE_SECTOR_SIZE);
    }
        #endif
    uMemset(fnGetExtFlashAdd((unsigned char *)ptrData), 0xff, ulSectorLength); // delete the sector in simulated flash
    #endif

    // Wait for command to complete
    //
    if (_fnWaitExtFlashOperation((volatile EXT_FLASH_ACCESS_UNIT *)fnGetExtFlashAdd((unsigned char *)ptrData), (EXT_FLASH_ACCESS_UNIT)0xffff) < 0) {
        return -1;                                                       // erase error
    }
    return 0;
}

// Called once to initialise the flash routines in RAM
//
static void fnInitExtFlashRoutines(void)
{
    #define WAIT_PROG_SIZE  100
    #define ERASE_PROG_SIZE 200
    #define WRITE_PROG_SIZE 200
    static unsigned long ulProgSpace1[WAIT_PROG_SIZE/sizeof(unsigned long)]; // RAM space on long boundary
    static unsigned long ulProgSpace2[ERASE_PROG_SIZE/sizeof(unsigned long)]; // RAM space on long boundary
    static unsigned long ulProgSpace3[WRITE_PROG_SIZE/sizeof(unsigned long)]; // RAM space on long boundary

    uMemcpy(ulProgSpace1, (unsigned char*)fnWaitExtFlashOperation, WAIT_PROG_SIZE);
    uMemcpy(ulProgSpace2, (unsigned char*)fnExtFlashErase, ERASE_PROG_SIZE);
    uMemcpy(ulProgSpace3, (unsigned char*)fnExtFlashWrite, WRITE_PROG_SIZE);
    #if defined _WINDOWS
    _fnWaitExtFlashOperation = fnWaitExtFlashOperation;
    _fnExtFlashErase = fnExtFlashErase;
    _fnExtFlashWrite = fnExtFlashWrite;
    #else
    _fnWaitExtFlashOperation = (int(*)(volatile EXT_FLASH_ACCESS_UNIT *, EXT_FLASH_ACCESS_UNIT))ulProgSpace1;
    _fnExtFlashErase = (int(*)(volatile unsigned char *, volatile unsigned char *))ulProgSpace2;
    _fnExtFlashWrite = (int(*)(volatile unsigned char *, volatile EXT_FLASH_ACCESS_UNIT *, EXT_FLASH_ACCESS_UNIT))ulProgSpace3;
    #endif
}

// Write a buffer of data to external parallel Flash
//
static int fnWriteParallelFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    // The code must not be executed from the same device and bank as the one that is being programmed - generally run from RAM
    //
    MAX_FILE_LENGTH data_length = ptrAccessDetails->BlockLength;
    volatile unsigned char *ptrData = (unsigned char *)(EXTERNAL_FLASH_START_ADDRESS + ptrAccessDetails->ulOffset); // address in the first chip
    volatile unsigned char *ptrCmdAdd = (volatile unsigned char *)EXTERNAL_FLASH_START_ADDRESS;
    #if defined EXT_FLASH_16_BIT
    volatile unsigned short *ptrDataUnit = (volatile unsigned short *)fnGetExtFlashAdd((unsigned char *)(CAST_POINTER_ARITHMETIC)ptrData);
    unsigned short usValue; // = *(volatile unsigned short *)fnGetExtFlashAdd((unsigned char *)((CAST_POINTER_ARITHMETIC)ptrData & ~0x01)); // {168}
    #endif

    if (ptrAccessDetails->ucDeviceNumber != 0) {
        ptrData += (SIZE_OF_EXTERNAL_FLASH_CHIP * ptrAccessDetails->ucDeviceNumber);
        ptrCmdAdd += (SIZE_OF_EXTERNAL_FLASH_CHIP * ptrAccessDetails->ucDeviceNumber);
    }
    #if defined EXT_FLASH_16_BIT
    usValue = *(volatile unsigned short *)fnGetExtFlashAdd((unsigned char *)((CAST_POINTER_ARITHMETIC)ptrData & ~0x01)); // {168} read initial value after the pointer has been set to the device
    #endif
    while (data_length-- != 0) {                                         // for each byte to be programmed
    #if defined EXT_FLASH_16_BIT                                         // 16 bit programming
        if ((CAST_POINTER_ARITHMETIC)ptrData & 0x01) {                   // odd address
            ptrDataUnit = (volatile unsigned short *)fnGetExtFlashAdd((unsigned char *)((CAST_POINTER_ARITHMETIC)ptrData & ~0x01));
        #if defined _LITTLE_ENDIAN
            usValue &= ~(0xff00);
            usValue |= ((unsigned short)*ptrBuffer << 8);                // insert the data byte to be programmed
        #else
            usValue &= ~(0x00ff);
            usValue |= *ptrBuffer;                                       // insert the data byte to be programmed
        #endif
        }
        else {                                                           // word aligned
            ptrDataUnit = (volatile unsigned short *)fnGetExtFlashAdd((unsigned char *)(CAST_POINTER_ARITHMETIC)ptrData);
            usValue = *ptrDataUnit;
        #if defined _LITTLE_ENDIAN
            usValue &= ~(0x00ff);
            usValue |= *ptrBuffer;                                       // insert the new byte
        #else
            usValue &= ~(0xff00);
            usValue |= (((unsigned short)*ptrBuffer) << 8);              // insert the new byte
        #endif
            if (data_length != 0) {                                      // as long as not final byte in the buffer
                goto _advance_buffer;                                    // collect bytes until a short word write can be made
            }
        }
        if (*ptrDataUnit == usValue) {                                   // if the destination is already programmed to the value there is no need to write
            goto _advance_buffer;
        }
        uDisable_Interrupt();                                            // protect from interrupts
        if (_fnExtFlashWrite(ptrCmdAdd, ptrDataUnit, usValue) != 0) {    // run the word programming routine in RAM
            uEnable_Interrupt();
            return -1;
        }    
        uEnable_Interrupt();                                             // re-enable interrupts
    #else
        if (*ptrData == *ptrBuffer) {                                    // if the destination is already programmed to the value there is no need to write
            goto _advance_buffer;
        }
        uDisable_Interrupt();                                            // protect from interrupts
        if (_fnExtFlashWrite(ptrCmdAdd, ptrData, *ptrBuffer) != 0) {     // run the byte programming routine in RAM
            uEnable_Interrupt();
            return -1;
        }    
        uEnable_Interrupt();                                             // re-enable interrupts
    #endif
_advance_buffer:
        ptrData++;
        ptrBuffer++;
    }
    return 0;
}

// Delete page/block in parallel flash 
//
static MAX_FILE_LENGTH fnDeleteParallelFlash(ACCESS_DETAILS *ptrAccessDetails)
{
    // The code must not be executed from the same device and bank as the one that is being programmed - generally run from RAM
    //
    volatile unsigned char *ptrData = (unsigned char *)(EXTERNAL_FLASH_START_ADDRESS + ptrAccessDetails->ulOffset); // address in the first chip
    volatile unsigned char *ptrCmdAdd = (volatile unsigned char *)EXTERNAL_FLASH_START_ADDRESS;

    if (ptrAccessDetails->ucDeviceNumber != 0) {
        ptrData += (SIZE_OF_EXTERNAL_FLASH_CHIP * ptrAccessDetails->ucDeviceNumber);
        ptrCmdAdd += (SIZE_OF_EXTERNAL_FLASH_CHIP * ptrAccessDetails->ucDeviceNumber);
    }
    uDisable_Interrupt();                                                // protect from interrupts
    if (_fnExtFlashErase(ptrCmdAdd, ptrData)) {                          // run the erase routine from RAM
        uEnable_Interrupt();
        return 0;                                                        // erase error
    }
    uEnable_Interrupt();

    // Return the size fo the block that was deleted
    //
    #if defined _S29JL032_32
    if (ptrAccessDetails->ulOffset < _S29JL032_32_BLOCK_ADDRESS_8) {
        return (_S29JL032_32_PARAMETER_SECTOR_SIZE);
    }
    else {
        return (_S29JL032_32_CODE_SECTOR_SIZE);
    }
    #else
    if (ptrAccessDetails->ulOffset < _M29W160EB_BLOCK_ADDRESS_4) {
        if (ptrAccessDetails->ulOffset < _M29W160EB_BLOCK_ADDRESS_1) {
            return (_M29W160EB_BOOT_SECTOR_SIZE);
        }
        if (ptrAccessDetails->ulOffset < _M29W160EB_BLOCK_ADDRESS_3) {
            return (_M29W160EB_SMALL_PARAMETER_SECTOR_SIZE);
        }
        else {
            return (_M29W160EB_LARGE_PARAMETER_SECTOR_SIZE);
        }
    }
    else {
        return (_M29W160EB_CODE_SECTOR_SIZE);
    }
    #endif
}
#endif

#if defined NVRAM && defined _M5225X
// Since the contents of a fresh MRAM chip are not absolutely defined a string is positioned
// at the end of the memory range which signals that its contents are of valid format.
// If the string is not correct the complete contents are set to 0xff so that it is compatible with FLASH operation and the string is then set.
// For this reason the use of the last 16 bytes at the end of the memory should be avoided.
//
static void fnFormatNVRAM(void)
{
    static const CHAR cValid[] = "!NVRAM-FORMATTED";
    CHAR *ptrCheck = (CHAR *)fnGetFlashAdd((unsigned char *)(uFILE_START + SIZE_OF_NVRAM));
    ptrCheck -= (sizeof(cValid) - 1);
    if (uMemcmp(cValid, ptrCheck, (sizeof(cValid) - 1)) != 0) {
        uMemset(fnGetFlashAdd((unsigned char *)uFILE_START), 0xff, (SIZE_OF_NVRAM - (sizeof(cValid) - 1)));// set contents to 0xff - like blank FLASH memory
        uMemcpy(ptrCheck, cValid, (sizeof(cValid) - 1));                 // mark that the contents are formatted
    }
    #if defined STORAGE_REV_1                                            // {170}
    UserStorageListPtr = (STORAGE_AREA_ENTRY *)&nvram_storage;           // insert nvram as storage medium
    #endif
}
#endif

// Basic hardware initialisation of specific hardware
//
EXTERNSTATIC void fnInitHW(void)                                         // perform hardware initialisation (heap not yet available)
{
#if defined _WINDOWS
    unsigned short usPortPullups[] = {
    #if defined _M520X                                                   // {127}
        FECH_DEFAULT_INPUT,
        FECL_DEFAULT_INPUT,
        BUSCTL_DEFAULT_INPUT,
        BE_DEFAULT_INPUT,
        CS_DEFAULT_INPUT,
        FECI2C_DEFAULT_INPUT,
        UART_DEFAULT_INPUT,
        QSPI_DEFAULT_INPUT,
        TIMER_DEFAULT_INPUT,
        IRQ_DEFAULT_INPUT,
        CHIP_CONFIGURATION,
        FORCE_D9_D1,
    #elif defined _M523X                                                 // {140}
        ADDR_DEFAULT_INPUT,
        BS_DEFAULT_INPUT,
        BUSCTL_DEFAULT_INPUT,
        CS_DEFAULT_INPUT,
        DATAH_DEFAULT_INPUT,
        DATAL_DEFAULT_INPUT,
        ETPU_DEFAULT_INPUT,
        FECI2C_DEFAULT_INPUT,
        IRQ_DEFAULT_INPUT,
        QSPI_DEFAULT_INPUT,
        SDRAM_DEFAULT_INPUT,
        TIMER_DEFAULT_INPUT,
        UARTH_DEFAULT_INPUT,
        UARTL_DEFAULT_INPUT,
        CHIP_CONFIGURATION,
        FORCE_CLK_D25_D16,
    #else
        QS_DEFAULT_INPUT,                                                // set the port states out of reset in the project file app_hw_m5223x.h
        #if !defined _M52XX_SDRAM                                        // {122}
        AN_DEFAULT_INPUT,
        #endif
        #if !defined _M5225X && !defined _M521X && !defined _M52XX_SDRAM // {65}{122}
        LD_DEFAULT_INPUT,
        #endif
        NQ_DEFAULT_INPUT,
        TA_DEFAULT_INPUT,
        TC_DEFAULT_INPUT,
        #if !defined _M5225X                                             // {65}
        TD_DEFAULT_INPUT,
        #endif
        UA_DEFAULT_INPUT,
        #if !defined _M52XX_SDRAM                                        // {122}
        UB_DEFAULT_INPUT,
        UC_DEFAULT_INPUT,
        #endif
        AS_DEFAULT_INPUT,
        #if !defined _M5225X && !defined _M52XX_SDRAM                    // {65}{122}
        GP_DEFAULT_INPUT,
        #endif
        DD_DEFAULT_INPUT,
        #if defined _M5225X                                              // {65}
        TE_DEFAULT_INPUT,
        TF_DEFAULT_INPUT,
        TG_DEFAULT_INPUT,
        TH_DEFAULT_INPUT,
        TI_DEFAULT_INPUT,
        TJ_DEFAULT_INPUT,
        #endif
        #if defined _M52XX_SDRAM                                         // {122}
        A_DEFAULT_INPUT,
        B_DEFAULT_INPUT,
        C_DEFAULT_INPUT,
        D_DEFAULT_INPUT,
        E_DEFAULT_INPUT,
        F_DEFAULT_INPUT,
        G_DEFAULT_INPUT,
        H_DEFAULT_INPUT,
        J_DEFAULT_INPUT,
            #if !defined _M521X_SDRAM
        EH_DEFAULT_INPUT,
            #endif
        EL_DEFAULT_INPUT,
        QA_DEFAULT_INPUT,
        QB_DEFAULT_INPUT,
        SD_DEFAULT_INPUT,
        TB_DEFAULT_INPUT,
        CHIP_CONFIGURATION,
        #endif
    #endif
    #if defined SUPPORT_ADC                                              // {48}
        ((AN0_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN1_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN2_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN3_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN4_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN5_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN6_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
        ((AN7_START_VOLTAGE * 0xfff) / ADC_REFERENCE_VOLTAGE),
    #endif
    };
    #if defined (RANDOM_NUMBER_GENERATOR) && !defined (RND_HW_SUPPORT)
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;                                             // {129} note that the target works differently
    #endif 
    fnInitialiseDevice((void *)usPortPullups);
    mcf52235_init();                                                     // call initialisation routine for code checking only
#endif
#if defined SUPPORT_ADC && !defined _M52XX_SDRAM && !defined _M520X      // {48}{122}{127}
    ADC_ADSDIS = (DISABLE_CH0 | DISABLE_CH1 | DISABLE_CH2 | DISABLE_CH3 | DISABLE_CH4 | DISABLE_CH5 | DISABLE_CH6 | DISABLE_CH7); // disable all sampling by default
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {122}{127}{140}
    PPMRH = (CDADC | CDPIT1 | CDFCAN | CDPWM | CDGPT | CDEPORT | CDEPHY);// {46} default some modules to off and power up only when used
    #if defined EXTERNAL_RAM && defined _M5225X                          // {107} when using external RAM don't disable the mini-flex bus
        #define _CDMFB 0
    #else
        #define _CDMFB CDMFB
    #endif
    #if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD || defined SDCARD_SUPPORT) // {114}
    PPMRL = (CDUART0 | CDUART1 | CDUART2 | CDI2C | CDI2C1 | CDFEC0 | CDTMR0 | CDTMR1 | CDTMR2 | CDTMR3 | _CDMFB);
    #else
    PPMRL = (CDUART0 | CDUART1 | CDUART2 | CDI2C | CDI2C1 | CDFEC0 | CDTMR0 | CDTMR1 | CDTMR2 | CDTMR3 | CDQSPI | _CDMFB);
    #endif
#endif
    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
#if !defined _RAM_DEBUG                                                  // {36}
    INIT_WATCHDOG_DISABLE();
    if (!WATCHDOG_DISABLE()) {
    #if defined BACKUP_WATCHDOG_TIMER                                    // {78} the BWT is used in preference to core watchdog
        CONFIGURE_BACKUP_WATCHDOG();
        #if defined _WINDOWS
        WCNTR = WMR;                                                     // synchronise the initial count value
        #endif
    #else
        CWCR = CONFIGURE_WATCHDOG;                                       // allow user configuration of internal watch dog timer
    #endif
    }
    else {
    #if defined BACKUP_WATCHDOG_TIMER_AVAILABLE                          // {140}
        WCR = 0;
    #endif
        CWCR = WATCHDOG_DISABLED;
    }
#else
    CWCR = WATCHDOG_DISABLED;
#endif
#if defined DMA_MEMCPY_SET || defined USB_INTERFACE
    PACR1 = 0x04;                                                        // enable DMA to access FLASH (also by USB DMA controller)
    GPACR1 = 0x04;
#else
    POWER_DOWN(POWER_DMA);                                               // {46}
#endif
#if defined RUN_LOOPS_IN_RAM
    fnInitDriver();                                                      // initialise driver code in SRAM (must be first)
    #if defined USE_IP
    fnInitIP();                                                          // initialse IP routines to run from SRAM
    #endif
#endif
#if defined EXT_FLASH_FILE_SYSTEM                                        // {149}
    fnInitExtFlashRoutines();                                            // initialise routines that write and erase external flash
#endif
    asm_int_on();                                                        // {108} enable interrupts so that they could be used by fnUserHWInit()
    fnUserHWInit();                                                      // allow user hardware configuration - note that heap is not yet available
#if defined GLOBAL_HARDWARE_TIMER
    POWER_UP(HW_TIMER_POWER);                                            // {46}
    fnSetIntHandler(HW_TIMER_VECTOR, (unsigned char *)_hwtimer_interrupt); // {43}
    HW_TIMER_COUNTER = 0;                                                // ensure reset on start
    HW_TIMER_INTERRUPT_CONFIG_REGISTER = HW_TIMER_INTERRUPT_PRIORITY;    // define interrupts level and priority
    IC_IMRL_0 &= ~(HW_TIMER_INTERRUPT_FLAG | MASK_ALL_INT);              // unmask interrupt source
#endif
#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    #if defined SPI_FLASH_ENABLED                                        // {29}
    fnConfigSPIFileSystem(MAX_SPI);                                      // configure SPI interface for maximum speed {9}
        #define _CHECK_SPI_CHIPS                                         // insert manufacturer dependent code
        #include "spi_flash_m5223x_atmel.h"                              // {39} - check the chip(s) availablility
        #include "spi_flash_m5223x_stmicro.h"                            // {40}
        #include "spi_flash_m5223x_sst25.h"                              // {61}
        #undef _CHECK_SPI_CHIPS
    #else         
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
    #endif
#endif
#if defined I2C_EEPROM_FILE_SYSTEM                                       // {143} configure I2C interface and check for optional I2C EEPROMs
    #define _CHECK_I2C_EEPROM
    #include "i2C_eeprom_m24m01.h"
    #undef _CHECK_I2C_EEPROM
#endif
#if defined NVRAM
    #if defined _M5225X                                                  // {77} FlexBus
    fnConfigureBus(0, 2);                                                // configure the FlexBus interface accordingly with 1:2 clocking and 2 wait states
    fnFormatNVRAM();                                                     // check that the contents are formatted and format is required
    #else                                                                // space to add for other processor types if required
    // Set up the NVRAM port interface here
    //
    // Configure the ports as inputs / outputs are required by the NVRAM interface and set the default state
    //
    #endif
#else
    #if defined _M5225X  && defined USB_BIGENDIAN                        // switch the USB controller to USB mode to improve efficiency
                                                                         // this bit is hard coded in the chip and cannot be changed (at the moment)
  //CCM_CCE = USB_END;                                                   // set the USB controller to big-endian mode - note that this register can only be written once after a reset!
    #endif
#endif
}


// Support watchdog re-triggering of specific hardware
//
extern void fnRetriggerWatchdog(void)
{
    //if (!DEFAULT_INPUT_SET) {                                          // test only to simulate watchdog timeout
#if defined BACKUP_WATCHDOG_TIMER                                        // {78} the BWT is used in preference to core watchdog
        WSR = 0x5555;                                                    // retrigger the software watchdog
        WSR = 0xaaaa;                                                    // using this two word sequence (no need to disable interrupt between writes)
#else
        CWSR = 0x55;                                                     // retrigger the software watchdog
        CWSR = 0xaa;                                                     // using this two byte sequence (no need to disable interrupt between writes)
#endif
    //}
    TOGGLE_WATCHDOG_LED();                                               // define in project defines whether this is performed and where
}


// Routine to disable interrupts during critical regions
//
extern void uDisable_Interrupt(void)
{
    asm_int_off();
    iInterruptLevel++;                                                   // monitor the level of disable nesting
}

// Routine to re-enable interrupts on leaving a critical region
//
extern void uEnable_Interrupt(void)                                      // interrupts are ALWAYS off when entering
{
#if defined _WINDOWS
    if (!iInterruptLevel) {
       _EXCEPTION("Check symmetry of disabling and enabling interrupts!");
    }                                                                    // a routine is enabling interrupt although they are presently off. This may not be a serious error but it is unexpected so best check why...
#endif
    if (!(--iInterruptLevel)) {                                          // only when no more interrupt nesting,
        asm_int_on();
    }
}
 

// User handling own interrupts can signal it here to avoid driver routines opening interrupts too early
//
extern void fnInUserInterrupt(int iEntering)                             // {62}
{
    if (iEntering) {                                                     // entering user controlled interrupt routine
        iInterruptLevel = 1;                                             // mark that interrupts are disabled
    }
    else {
        iInterruptLevel = 0;                                             // mark interrupt exiting
    }
}


// Default interrupt handler - any undefined interrupts will arrive here
//
static __interrupt__ void undef_int(void)
{
    while (1) {}                                                         // wait here and analyse with debugger....
}




// Routine to initialise the Real Time Tick interrupt
//
#define REQUIRED_MS ((1000/TICK_RESOLUTION))                             // the TICK frequency we require in kHz
#if TICK_RESOLUTION > 4
    #if TICK_RESOLUTION > 64
        #define TICK_DIVIDE (((BUS_CLOCK/2/32768) + REQUIRED_MS/2)/REQUIRED_MS) // the divide ratio required (32k prescaler assumed)
        #define PIT_PRESCALE PIT_PRESCALE_32K
    #else
        #define TICK_DIVIDE (((BUS_CLOCK/2/4096) + REQUIRED_MS/2)/REQUIRED_MS)  // the divide ratio required (4k prescaler assumed)
        #define PIT_PRESCALE PIT_PRESCALE_4K
    #endif
#else
    #define TICK_DIVIDE (((BUS_CLOCK/2/1048) + REQUIRED_MS/2)/REQUIRED_MS) // the divide ratio required (1k prescaler assumed)
    #define PIT_PRESCALE  PIT_PRESCALE_1K
#endif


/**************************** Real Time Clock interrupt ******************************************/

static __interrupt__ void _RealTimeInterrupt(void)
{
  //PIT_PCSR_0 = (PIT_PRESCALE | PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE | PIT_EN); // reset interrupt request flag
    PIT_PCSR_NON_PRE_0 = (PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE | PIT_EN); // {177} reset interrupt request flag without writing to pre-scaler (and resetting its value)
    fnRtmkSystemTick();                                                  // operating system tick
}

// TICK uses PIT 0
//
extern void fnStartTick(void)
{
    PIT_PCSR_0 = (PIT_PRESCALE | PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD); // prepare for load
    PIT_PMR_0 = (TICK_DIVIDE - 1);                                       // load interval value {45} - 1 subtracted from divide value

    fnSetIntHandler(PIT0_VECTOR, (unsigned char *)_RealTimeInterrupt);

#if defined _M520X                                                       // {127}
    IC_ICR_0_4 = (TICK_INTERRUPT_PRIORITY & LEVEL_MASK);                 // define interrupt level
    IC_IMRL_0 &= ~(PIT_0_PIF_INT_L);                                     // unmask interrupt source
#elif defined _M523X                                                     // {140}
    IC_ICR_0_36 = TICK_INTERRUPT_PRIORITY;                               // define interrupt level and priority
    IC_IMRH_0 &= ~(PIT_0_PIF_INT_H);                                     // unmask interrupt source
    IC_IMRL_0 &= ~(MASK_ALL_INT);                                        // clear global mask
#else
    IC_ICR_0_55 = TICK_INTERRUPT_PRIORITY;                               // define interrupt level and priority
    IC_IMRH_0 &= ~(PIT_0_PIF_INT_H);                                     // unmask interrupt source
    IC_IMRL_0 &= ~(MASK_ALL_INT);                                        // clear global mask {5}
#endif

    PIT_PCSR_0 = (PIT_PRESCALE | PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE | PIT_EN); // start PIT with interrupt enabled
#if defined _WINDOWS
    PIT_PCNTR_0 = PIT_PMR_0;                                             // load the start value to PIT counter
#endif
#if defined _GNU
  //asm_int_on();                                                        // {108} no longer required
#endif
#if defined MONITOR_PERFORMANCE                                          // {134} configure a timer that will be used to measure the duration of task operation
    INITIALISE_MONITOR_TIMER();
#endif
}



#if defined ETH_INTERFACE
/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */
#if defined SCAN_PHY_ADD                                                 // {75} allow overrides to suit external PHY
    static unsigned char PHY_ADDRESS = 1;                                // initial PHY scan address
#else
    #if !defined PHY_ADDRESS
        #define PHY_ADDRESS 0x5                                          // any address from 1..31 can be used for internal PHY
    #endif
#endif

#if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME
    static const unsigned char EMAC_RXA_int_message[HEADER_LENGTH] = {0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RX_INTERRUPT}; // define fixed interrupt event
#endif
static unsigned short fnMIIread(unsigned char _mpadr, unsigned char _mradr);


#if defined USE_MULTIPLE_BUFFERS
static unsigned char ucEthernetInput[LAN_BUFFER_SIZE + 4];               // {166} linear buffer for collecting a complete Ethernet reception frame

// We are more or less forced to copy a number of input buffers to a linear input buffer so that it can be handled by standard routines
// Therefore we step through one or a number of receive buffer descriptors, receiving a complete frame spread out within them
//
static unsigned short fnExtractNextRx(void)
{
    unsigned short usFrameLength = 0;
    int iBuffers = 0;
    volatile M5223X_FEC_BD *rx_bd = ptrRxBd;

    while (1) {
        if (rx_bd->usBDLength == 0) {
            break;                                                       // frame reception not complete
        }
        if (rx_bd->usBDControl & LAST_IN_FRAME) {                        // complete message available
            unsigned char *ptrInputBuffer = ucEthernetInput;
            while (iBuffers--) {
                usFrameLength += 256;                                    // frame size after the copy
                if (usFrameLength <= sizeof(ucEthernetInput)) {          // {166} only copy if there is space for it in the linear buffer
                    uMemcpy(ptrInputBuffer, ptrRxBd->ptrBD_Data, 256);
                }
                ptrInputBuffer += 256;
    #if defined USE_IP_STATS
                if (ptrRxBd->usBDControl & OVERRUN_FRAME) {
                    fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES, DEFAULT_NETWORK); // we lost a frame due to RX overrun so count the event
                }
    #endif
                ptrRxBd->usBDLength = 0;
                fnFreeEthernetBuffer(-1);
            }
            if (ptrRxBd->usBDLength <= sizeof(ucEthernetInput)) {        // {166} only copy if there is space for it in the linear buffer
                uMemcpy(ptrInputBuffer, ptrRxBd->ptrBD_Data, (ptrRxBd->usBDLength - usFrameLength)); // final copy or remaing content
              //usFrameLength += ptrRxBd->usBDLength;                    // {166} - correct frame reception length
                usFrameLength = ptrRxBd->usBDLength;                     // complete frame length
                usFrameLength -= 4;                                      // {124} remove the CRC32 which is saved at the end of a received frame
            }
            else {
                usFrameLength = 0;                                       // since the received frame didn't fit in the linear buffer we silently drop it
            }
    #if defined USE_IP_STATS
            if (ptrRxBd->usBDControl & OVERRUN_FRAME) {
                fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES, DEFAULT_NETWORK); // we lost a frame due to RX overrun so count the event
            }
    #endif
            ptrRxBd->usBDLength = 0;
            fnFreeEthernetBuffer(-1);
            break;
        }
        else {
            iBuffers++;                                                  // count the number of full buffers
            if (rx_bd->usBDControl & WRAP_BIT) {
                rx_bd = (M5223X_FEC_BD*)ERDSR;                           // set to first BD
            }
            else {
                rx_bd++;                                                 // increment to next
            }
        }
    }
    return usFrameLength;
}
#endif


// This is called when an Ethernet event has been received. Generally it is due to a reception and returns channel and message details.
//
extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
#if defined LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_int_message[HEADER_LENGTH] = {0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_RX_INTERRUPT};
#endif
    if (EMAC_RX_INTERRUPT == *ucEvent) {                                 // check whether there is a waiting reception frame
#if defined USE_MULTIPLE_BUFFERS
        if ((rx_frame->frame_size = fnExtractNextRx()) == 0) {           // get frame details from present buffer descriptor
            return -1;                                                   // nothing (else) waiting
        }
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ucEthernetInput;     // return pointer to the fixed linear input buffer
#else
        if (ptrRxBd->usBDControl & EMPTY_BUFFER) {
            return -1;                                                   // nothing else waiting
        }
        if (ptrRxBd->usBDControl & (TRUNCATED_FRAME | OVERRUN_FRAME)) {
            if ((ptrRxBd->usBDControl & TRUNCATED_FRAME) == 0) {
    #if defined USE_IP_STATS
                fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES, DEFAULT_NETWORK); // we lost a frame due to RX overrun so count the event
    #endif
                rx_frame->frame_size = (ptrRxBd->usBDLength - 4);        // we haven't actually lost this frame though {124}
            }
            rx_frame->frame_size = 0;                                    // reject further frame processing
        }
        else {
            rx_frame->frame_size = (ptrRxBd->usBDLength - 4);            // {124} remove the CRC32 which is saved at the end of a received frame
        }
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ptrRxBd->ptrBD_Data; // set pointer to the buffer descriptor data pointer where the frame content is located
#endif
#if defined LAN_REPORT_ACTIVITY
      //iInterruptLevel = 1;                                             // {113} ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
      //iInterruptLevel = 0;                                             // {113}
#endif
       return 0;                                                         // channel received from 0 (fixed value for compatibility with other devices)
    }
#if defined USE_IP_STATS && !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME  // {144}
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
#endif
    return -1;                                                           // channel invalid
}



/**************************** transmission complete Ethernet interrupt ***********************************/

static __interrupt__ void _fec_txf_isr(void)
{
#if defined LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_int_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT };
#endif
    //eth_tx_control->ETH_queue.chars = 0;                               // reset the transmission buffer for next use
    //eth_tx_control->ETH_queue.put = eth_tx_control->ETH_queue.QUEbuffer;
#if defined LAN_REPORT_ACTIVITY
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting message to queue
    fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // inform the task of event
    iInterruptLevel = 0;                                                 // release     
#endif
#if defined _WINDOWS
    EIR &= ~TXF;
#else
    EIR = TXF;                                                           // reset interrupt
#endif
}


/**************************** valid receive buffer Ethernet interrupt ************************************/

#if defined _MAGIC_RESET_FRAME                                           // {178}
extern void fnEnableMagicFrames(int iOnOff)
{
    uDisable_Interrupt();                                                // protect from ethernet reception interrupts
    if (iOnOff == 0) {
        ptrMagicRxBd = 0;                                                // disable
    }
    else {
        ptrMagicRxBd = ptrRxBd;                                          // enabled by synchronising the magic frame buffer with the receiver
    }
    uEnable_Interrupt();                                                 // allow further interrupt processing
}
#endif

static __interrupt__ void _fec_rx_frame_isr(void)
{
#if defined _MAGIC_RESET_FRAME  && !defined USE_MULTIPLE_BUFFERS         // {178}
    if (ptrMagicRxBd != 0) {
        if ((ptrMagicRxBd->usBDControl & EMPTY_BUFFER) == 0) {           // ignore if buffer is empty
            if ((ptrMagicRxBd->usBDControl & (TRUNCATED_FRAME | OVERRUN_FRAME)) == 0) { // only handle valid frames
                unsigned short usLength = ptrMagicRxBd->usBDLength;      // length in the buffer
                if (usLength == (93 + 4)) {                              // payload length must be 93 bytes in length (plus CRC-32)
                    unsigned char *ptrData = &((ETHERNET_FRAME_CONTENT *)ptrMagicRxBd->ptrBD_Data)->ethernet_frame_type[0];
                    int i;
                    for (i = 0; i < 24; i++) {                           // remaining header must be (unsigned short)_MAGIC_RESET_FRAME repeated 24 times
                        if (*ptrData++ != (unsigned char)(_MAGIC_RESET_FRAME >> 8)) {
                            goto _not_magic_frame;
                        }
                        if (*ptrData++ != (unsigned char)(_MAGIC_RESET_FRAME)) {
                            goto _not_magic_frame;
                        }
                    }
                    // A magic frame has been received - the 61st byte is used to signal the type and 32 bytes of optional data are passed
                    //
                    fnMagicFrame(*ptrData, (ptrData + 1));               // allow the user to handle the magic frame
                }
            }
_not_magic_frame:
            if ((ptrMagicRxBd->usBDControl & WRAP_BIT) != 0) {
                ptrMagicRxBd = (M5223X_FEC_BD *)ERDSR;                   // set to first BD
            }
            else {
                ptrMagicRxBd++;                                          // increment to next buffer
            }
        }
    }
#endif
#if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                           // {144}
    uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);                 // schedule the Ethernet task
#else
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting message to queue
    fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_RXA_int_message, HEADER_LENGTH); // inform the Ethernet task that a new frame is waiting
    iInterruptLevel = 0;                                                 // release  
#endif
#if defined _WINDOWS
    EIR &= ~RXF; 
#else
    EIR = RXF;                                                           // reset interrupt flag
#endif  
}


/**************************** PHY Ethernet interrupt ****************************************************/
#if !(defined _M5225X || (defined _M52XX_SDRAM && !defined _M521X_SDRAM) || defined _M520X || defined _M523X) || defined PHY_POLL_LINK || defined PHY_INTERRUPT                       // {176}
extern void fnCheckEthLinkState(void)
{
    #if defined _M5225X || (defined _M52XX_SDRAM && !defined _M521X_SDRAM) || defined _M520X || defined _M523X // external PHY
        #if defined INTERRUPT_TASK_PHY
    unsigned char int_phy_message[HEADER_LENGTH];
    volatile unsigned short usInterrupt = fnMIIread(PHY_ADDRESS, KS8721_INTERRUPT_CONTROL_STATUS_REGISTER); // read the cause(s) of the interrupt, which resets the bits
        #else
    fnMIIread(PHY_ADDRESS, KS8721_INTERRUPT_CONTROL_STATUS_REGISTER);    // read the cause(s) of the interrupt, which resets the bits
        #endif
        #if defined INTERRUPT_TASK_PHY
    if (PHY_LINK_UP_INT & usInterrupt) {
        usInterrupt = fnMIIread(PHY_ADDRESS, KS8721_BASIC_STATUS_REGISTER); // check the  details of link
        if (usInterrupt & (PHY_100MEG_HALF_DUPLEX | PHY_100MEG_FULL_DUPLEX | PHY_100MEG_T4_CAPABLE)) {
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
        }
        else {
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
        }
        if (usInterrupt & (PHY_100MEG_FULL_DUPLEX | PHY_10MEG_FULL_DUPLEX)) {
            int_phy_message[MSG_INTERRUPT_EVENT]++;                      // {176} full duplex rather than half-duplex
        }
    }
    else if (PHY_LINK_DOWN_INT & usInterrupt) {
        int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
    }
    else {
        int_phy_message[MSG_INTERRUPT_EVENT] = UNKNOWN_INTERRUPT;
    }
    int_phy_message[MSG_DESTINATION_NODE] = int_phy_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    int_phy_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_PHY;
    int_phy_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
    fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // inform the corresponding task
        #endif
    #else                                                                // internal PHY
        #if defined INTERRUPT_TASK_PHY
    unsigned char int_phy_message[HEADER_LENGTH];                        // standard interrupt event
        #else
    unsigned char int_message[HEADER_LENGTH];                            // standard interrupt event
        #endif
    unsigned short usInterrupt;
    unsigned char ucEvent = UNKNOWN_INTERRUPT;

    usInterrupt = fnMIIread(PHY_ADDRESS, PHY_REG_ICR);                   // read interrupt source
 
    if (usInterrupt & PHY_LKC) {                                         // a change in link status
        usInterrupt = fnMIIread(PHY_ADDRESS, PHY_REG_PSR);               // get the details

        switch (usInterrupt & (PHY_R17_LNK | PHY_R17_SPD)) {
        case PHY_R17_SPD:
            if (usInterrupt & PHY_R17_DPM) {                             // {176}
                ucEvent = LAN_LINK_UP_100_FD;
            }
            else {
                ucEvent = LAN_LINK_UP_100;
            }
            break;
        case 0:
            if (usInterrupt & PHY_R17_DPM) {                             // {176}
                ucEvent = LAN_LINK_UP_10_FD;
            }
            else {
                ucEvent = LAN_LINK_UP_10;
            }
            break;
        default:
            ucEvent = LAN_LINK_DOWN;
            break;
        }

        #if defined INTERRUPT_TASK_PHY
        int_phy_message[MSG_DESTINATION_NODE] = int_phy_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
        int_phy_message[MSG_DESTINATION_TASK] = INTERRUPT_TASK_PHY;
        int_phy_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
        int_phy_message[MSG_INTERRUPT_EVENT] = ucEvent;
        #else
        int_message[MSG_DESTINATION_NODE] = int_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
        int_message[MSG_DESTINATION_TASK] = EthernetWake;
        int_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
        int_message[MSG_INTERRUPT_EVENT] = ucEvent;
        #endif
    }

    EPHYSR = EPHYIF;                                                     // clear interrupt

        #if defined INTERRUPT_TASK_PHY
    if (ucEvent != UNKNOWN_INTERRUPT) {
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, int_phy_message, HEADER_LENGTH);
        iInterruptLevel = 0;                                               
    }
        #else
    if (EthernetWake) {
        if (ucEvent != UNKNOWN_INTERRUPT) {
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, int_message, HEADER_LENGTH);
            iInterruptLevel = 0;                                          
        }
    }
        #endif
    #endif
}
#endif

#if defined _M5225X || (defined _M52XX_SDRAM && !defined _M521X_SDRAM) || defined _M520X || defined _M523X // {122}{127}{140}
    #if defined INTERRUPT_TASK_PHY && defined PHY_INTERRUPT && PHY_INTERRUPT != 0
static void fnPhyInterrupt(void)                                         // interrupt call-back
{
    fnCheckEthLinkState();                                               // {176}
}
    #endif
#else
static __interrupt__ void _ephy_isr(void)                                // interrupt routine
{
    fnCheckEthLinkState();                                               // {176}
}
#endif



/**************************** END of Ethernet Interrupts *************************************************/

// Function to read from the MII interface
//
static unsigned short fnMIIread(unsigned char _mpadr, unsigned char _mradr)
{
#if defined PHY_MICREL_SMI
    if (_mpadr == 0) {                                                   // {175}
		int i;
        #define MDIO_LINE      PORT_NQ_BIT3
        #define MDC_LINE       PORT_NQ_BIT5
        #define SET_MDC_HIGH()  _SETBITS(NQ, MDC_LINE);_SETBITS(NQ, MDC_LINE)
        #define SET_MDC_LOW()   _CLEARBITS(NQ, MDC_LINE);_CLEARBITS(NQ, MDC_LINE)
        // Bit bang to achieve the operation since the read doesn't work with the M522xx in SMI mode
        //
        _CLEAR_FUNCTION(NQ, (CLEAR_FUNCTION_Q_BIT_3 | CLEAR_FUNCTION_Q_BIT_5)); // remove peripheral function on pins
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(NQ, (MDIO_LINE | MDC_LINE), (MDIO_LINE)); // GPIO driving with initial value
		for (i = 0; i < 32; i++) {
            SET_MDC_HIGH();                                              // preamble with 32 clocks
            SET_MDC_LOW();
		}
        _CLEARBITS(NQ, MDIO_LINE);                                       // '0'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        _SETBITS(NQ, MDIO_LINE);                                         // '1'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        _CLEARBITS(NQ, MDIO_LINE);                                       // '0'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        _SETBITS(NQ, MDIO_LINE);                                         // '1'
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
        SET_MDC_HIGH();                                                  // clock
        SET_MDC_LOW();
		for (i = 0; i < 8; i++) {                                        // register value
			if (_mradr & 0x80) {
                _SETBITS(NQ, MDIO_LINE);                                 // '1'
			}
			else {
                _CLEARBITS(NQ, MDIO_LINE);                               // '0'
			}
            SET_MDC_HIGH();                                              // clock
			_mradr <<= 1;
            SET_MDC_LOW();
		}
        SET_MDC_HIGH();                                                  // clock
        _FLOAT_PORT(NQ, MDIO_LINE);                                      // set data line as input
        SET_MDC_LOW();

		for (i = 0; i < 9; i++) {                                        // 9 clocks reading in '0'
            SET_MDC_HIGH();                                              // clock
            SET_MDC_LOW();
		}
		_mradr = 0;
		for (i = 0; i < 8; i++) {                                        // 8 clocks reading in data
            SET_MDC_HIGH();                                              // clock
			_mradr <<= 1;
            if (_READ_PORT_MASK(NQ, MDIO_LINE)) {                        // if '1'
				_mradr |= 1;
			}
            SET_MDC_LOW();
		}
        // Return pin mode when completed
        //
        PNQPAR |= (NQ_FEC_MDIO_FUNCTION | NQ_FEC_MDC_FUNCTION);          // set back MII MDC and MDIO functions to NQ3 and NQ5 pins
		return _mradr;
	}
	else {
    #if defined _STOP_MDIO_CLOCK
        MSCR = (((PLL_OUTPUT_FREQ_INT/(2 * MII_MANAGEMENT_CLOCK_SPEED)) + 1) << 1); // {126} enable the clock
    #endif
        MMFR = (MII_READ | (_mpadr << 23) | (_mradr << 18));             // command read from given address
	}
#else
    #if defined _STOP_MDIO_CLOCK
    MSCR = (((PLL_OUTPUT_FREQ_INT/(2 * MII_MANAGEMENT_CLOCK_SPEED)) + 1) << 1); // {126} enable the clock
    #endif
    MMFR = (MII_READ | (_mpadr << 23) | (_mradr << 18));
#endif

    while ((EIR & MII) == 0) {};                                         // wait until the read has completed
    EIR = MII;                                                           // reset the interrupt event

#if defined _WINDOWS
    if (PHY_REG_ICR == _mradr) {      
        MMFR = PHY_LKC;                                                  // link state change
    }
    else if (PHY_REG_PSR == _mradr) {
        MMFR = PHY_R17_SPD;                                              // state link up 100M
    }
    else if (PHY_REG_ANAR == _mradr) {
        MMFR = (PHY_R4_NP | PHY_R4_100F | PHY_R4_100H | PHY_R4_10F | PHY_R4_10H | 1); // capabilities
    }
    #if defined PHY_IDENTIFIER
    else if (PHY_REG_ID1 == _mradr) {                                    // {75}
        MMFR = (unsigned short)(PHY_IDENTIFIER >> 16);                   // ID 1
    }
    else if (PHY_REG_ID2 == _mradr) {
        MMFR = (unsigned short)(PHY_IDENTIFIER);                         // ID 2
    }
    #endif
    #if (defined _M5225X || defined _M52XX_SDRAM || defined _M523X) && defined PHY_INTERRUPT // {122}{145}
    else if (KS8721_INTERRUPT_CONTROL_STATUS_REGISTER == _mradr) {
        MMFR = PHY_LINK_UP_INT;
        #if defined _M523X
        fnSimulateInputChange(_PORT_IRQ, (7 - PHY_INTERRUPT), SET_INPUT);// clear level sensitive interrupt input
        #else
        fnSimulateInputChange(_PORT_NQ, (7 - PHY_INTERRUPT), SET_INPUT); // clear level sensitive interrupt input
        #endif
    }
    else if (KS8721_BASIC_STATUS_REGISTER == _mradr) {
        MMFR = PHY_100MEG_HALF_DUPLEX;
    }
    #endif
#endif
#if defined _STOP_MDIO_CLOCK
    MSCR = 0;                                                            // {126} disable clock
#endif
    return (unsigned short)MMFR;                                         // return the data read
}

// Function to write to the MII interface
//
static void fnMIIwrite(unsigned char _mpadr, unsigned char _mradr, unsigned short _mwdata)
{
#if defined _STOP_MDIO_CLOCK
    MSCR = (((PLL_OUTPUT_FREQ_INT/(2 * MII_MANAGEMENT_CLOCK_SPEED)) + 1) << 1); // {126} enable the clock
#endif
#if defined PHY_MICREL_SMI
    if (_mpadr == 0) {                                                   // {165a}
        MMFR = ((MII_TA | MII_ST) | (_mradr << 18) | _mwdata);           // command SMI write to given SMI register
	}
	else {
        MMFR = (MII_WRITE | (_mpadr << 23) | (_mradr << 18) | _mwdata);  // command write to given address
	}
#else
    MMFR = (MII_WRITE | (_mpadr << 23) | (_mradr << 18) | _mwdata);
#endif
    while (!(EIR & MII)) {};                                             // wait until the write has completed
    EIR = MII;                                                           // reset the interrupt event
#if defined _STOP_MDIO_CLOCK
    MSCR = 0;                                                            // {126} disable clock
#endif
}


#if defined PHY_MULTI_PORT                                               // {174}
extern unsigned short _fnMIIread(unsigned char ucPort, unsigned char _mradr)
{
    return (fnMIIread(ucPort, _mradr));
}

extern void _fnMIIwrite(unsigned char ucPort, unsigned char _mradr, unsigned short _mwdata)
{
    fnMIIwrite(ucPort, _mradr, _mwdata);
}
#else
extern unsigned short _fnMIIread(unsigned char _mradr)                   // {158}
{
    return (fnMIIread(PHY_ADDRESS, _mradr));
}

extern void _fnMIIwrite(unsigned char _mradr, unsigned short _mwdata)    // {158}
{
    fnMIIwrite(PHY_ADDRESS, _mradr, _mwdata);
}
#endif

#if defined USE_IGMP || defined USE_IPV6
static unsigned char fnCalculateHash(unsigned char *ptrMac)
{
    #define CRC_POLY 0xedb88320                                          // CRC-32 polynomial in reverse direction
    unsigned long ulCRC32 = 0xffffffff;
    int i, iBit;
    unsigned char ucByte;
	for (i = 0; i < MAC_LENGTH; i++) {
        ucByte = *ptrMac++;
        for (iBit = 0; iBit < 8; iBit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
    }
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 determine the hash entry location
    return (unsigned char)ulCRC32;
}

static void fnEnableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        GALR |= (0x00000001 << ucHashRef);                               // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        GAUR |= (0x00000001 << ucHashRef);
    }
}
#endif

#if defined USE_IGMP                                                     // {184}
static void fnDisableMulticastHash(unsigned char ucHashRef)
{
    if (ucHashRef < 32) {
        GALR &= ~(0x00000001 << ucHashRef);                              // enter single group hash entry
    }
    else {
        ucHashRef -= 32;
        GAUR &= ~(0x00000001 << ucHashRef);
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

#if defined USE_IPV6
// Configure a multicast address for IPV6 use
//
static void fnConfigureMulticastIPV6(void)
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

// Ethernet configuration routine
//
extern int fnConfigEthernet(ETHTABLE *pars)                              // {151}
{
    unsigned long ulPhyIdentifier;
#if defined _M5225X || defined _M52XX_SDRAM || defined _M520X || defined _M523X // {75}{122}{127}{145}
    #if defined INTERRUPT_TASK_PHY && defined PHY_INTERRUPT && PHY_INTERRUPT != 0
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    #endif
#endif
    unsigned short usMIIData;
    unsigned char *ptrBuffer;
    M5223X_FEC_BD *ptrBD;
    int i;
#if defined USE_MULTIPLE_BUFFERS
    unsigned char ucMultiBuf;
#endif
#if !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {65}{122}{127}{140}
    volatile unsigned long  ulDelay;
    POWER_UP(POWER_EPHY);                                                // {46} enable clock to modules
#endif
    POWER_UP(POWER_FEC0);
#if defined _M5225X                                                      // {65}
    #if defined FORCE_PHY_CONFIG                                         // {125}
    FNFORCE_PHY_CONFIG();                                                // drive configuration lines and reset
    #endif
                                                                         // enable all connections to external PHY
    PTIPAR = (TI_FEC_COL_FUNCTION | TI_FEC_CRS_FUNCTION | TI_FEC_RXCLK_FUNCTION | TI_FEC_RXD_0_FUNCTION | TI_FEC_RXD_1_FUNCTION | TI_FEC_RXD_2_FUNCTION | TI_FEC_RXD_3_FUNCTION | TI_FEC_RXDV_FUNCTION);
    PTJPAR = (TJ_FEC_RXER_FUNCTION | TJ_FEC_TXCLK_FUNCTION | TJ_FEC_TXD_0_FUNCTION | TJ_FEC_TXD_1_FUNCTION | TJ_FEC_TXD_2_FUNCTION | TJ_FEC_TXD_3_FUNCTION | TJ_FEC_TXEN_FUNCTION | TJ_FEC_TXER_FUNCTION);
    _CLEAR_FUNCTION(NQ, (CLEAR_FUNCTION_Q_BIT_3 | CLEAR_FUNCTION_Q_BIT_5)); // clear default IRQ function
  //PNQPAR &= ~((ALT_2_FUNCTION_Q << (2 * 3)) | (ALT_2_FUNCTION_Q << (2 * 5)));// clear default IRQ function
    PNQPAR |= (NQ_FEC_MDIO_FUNCTION | NQ_FEC_MDC_FUNCTION);              // set MII MDC and MDIO functions to NQ3 and NQ5 pins
    FNRESETPHY();                                                        // control the reset line to the PHY to respect any power up delays if required
#elif defined _M52XX_SDRAM                                               // {122}
    PEHLPAR = (PEHLPAR_PELPA | PEHLPAR_PEHPA);                           // configure ports EL and EH as Ethernet
    PASPAR |= (AS_FEC_MDIO_FUNCTION | AS_FEC_MDC_FUNCTION);              // set MII MDC and MDIO functions to AS4 and AS5 pins
#elif defined _M5208                                                     // {127}
    PAR_FECI2C = (PAR_MDIO_FEC_MDIO | PAR_MDC_FEC_MDC);                  // configure FEC interface for MII use
    PAR_FEC = (PAR_FEC_MII | PAR_FEC_7W);
#elif defined _M523X                                                     // {140}
	PAR_FECI2C |= (PAR_FEC_EMDC | PAR_FEC_EMDIO);                        // configure FEC interface for MII use
#else
                                                                         // configure the MII interface and PHY
    EPHYCTL1 = PHY_ADDRESS;                                              // configure Physical layer and its interrupts
                                                                         // enable EPHY module with PHY clocks disabled
                                                                         // do not turn on PHY clocks until both FEC and EPHY are completely setup
    EPHYCTL0 = (DIS100 | DIS10 | ANDIS);                                 // disable PHY clocks until initialisation is complete

    if ((pars->usMode & (LAN_10M | LAN_100M)) == 0) {
        EPHYCTL0 &= ~ANDIS;                                              // enable autonegotiate
    }

    if ((pars->usMode & LAN_LEDS) != 0) {                                // enable hardware LED support                                                    
        PLDPAR = ENABLE_PHY_LEDS;
    }

    EPHYCTL0 |= (EPHYEN | EPHYIEN);                                      // enable the PHY
    EPHYCTL0 &= ~(DIS100 | DIS10);                                       // let PHY PLL be determined by PHY

    for (ulDelay = 200000; ulDelay > 0; ulDelay--) {};                   // delay before continuing (specified as 360us according to latest Freescale info - this has proved to be always adequate)
#endif
                                                                         // EMAC initialisation
    ECR = RESET_FEC;                                                     // EMAC software reset
#if !defined _WINDOWS
    while (ECR & RESET_FEC) {}                                           // short wait until reset is complete
#endif

    EIMR = (BABR | BABT | GRA | TXF | RXF | EBERR | LC | RL | UN);       // interrupt sources
    EIR = 0xffffffff;                                                    // reset all pending interrupts
#if defined USE_IPV6                                                     // {117}
    PALR = ((network[DEFAULT_NETWORK].ucOurMAC[0] << 24) | (network[DEFAULT_NETWORK].ucOurMAC[1] << 16) | (network[DEFAULT_NETWORK].ucOurMAC[2] << 8) | (network[DEFAULT_NETWORK].ucOurMAC[3])); // set the MAC address
    PAUR = ((network[DEFAULT_NETWORK].ucOurMAC[4] << 24) | (network[DEFAULT_NETWORK].ucOurMAC[5] << 16));
#else
    PALR = ((pars->ucMAC[0] << 24) | (pars->ucMAC[1] << 16) | (pars->ucMAC[2] << 8) | (pars->ucMAC[3])); // set the MAC address
    PAUR = ((pars->ucMAC[4] << 24) | (pars->ucMAC[5] << 16));

    IALR = 0;                                                            // reset individual and group hash entries
    IAUR = 0;
    GAUR = 0;
    GALR = 0;
#endif
#if defined _WINDOWS
    if ((PALR & 0x01000000) != 0) {                                      // {98}{169}
        _EXCEPTION("check that the own MAC address doesn't set the multicast bit!!");
    }
#endif

    OPD = PAUSE_DURATION;

#if defined SUPPORT_VLAN                                                 // {2}
    if (pars->usSizeRx > 1522) {                                         // allow full length frame with VLAN tag
        pars->usSizeRx = 1522;                                           // maximum frame size supported
    }
#else
    if (pars->usSizeRx > 1518) {
        pars->usSizeRx = 1518;                                           // maximum frame size supported
    }
#endif
    RCR = (pars->usSizeRx << 16) | FEC_MII_MODE;                         // set max frame size to be received and MII mode

    if ((pars->usMode & PROMISCUOUS) != 0) {
        RCR |= PROM;                                                     // no destination address checking
    }
    else {
#if defined USE_IPV6
        if (pars->usMode & CON_MULTICAST) {                              // {117} if multicast reception is to be configured
            fnConfigureMulticastIPV6();                                  // set IPV6 multicast address
        }
#endif
        if ((pars->usMode & NO_BROADCAST) != 0) {
            RCR |= BC_REJ;                                               // reject broadcast frames
        }
    }

    if ((pars->usMode & FULL_DUPLEX) != 0) {                             // define network control {1}
        TCR = FDEN;
        if ((pars->usMode & RX_FLOW_CONTROL) != 0) {
            RCR |= FCE;                                                  // uses pause flow control
        }
    }
    else {
        TCR = 0;
        RCR |= DRT;                                                      // half duplex
    }

    if ((pars->usMode & LOOPBACK) != 0) {                                // optional Loopback for testing
        RCR |= LOOP;
    }
#if !defined _STOP_MDIO_CLOCK
    MSCR = (((PLL_OUTPUT_FREQ_INT/(2 * MII_MANAGEMENT_CLOCK_SPEED)) - 1) << 1); // {126}{75} - general formular added
#endif
    MIBC = MIB_DISABLE;                                                  // ensure MIB is disabled while reseting
    uMemset(MIBRAM_ADD, 0, MIB_RAM_SIZE);                                // reset all counters in the maintenance block RAM
    MIBC = 0;

#if defined _M5225X || defined _M52XX_SDRAM || defined _M520X || defined _M523X // {75}{122}{127}{145} check that the external PHY is responding correctly
    #if defined SCAN_PHY_ADD || defined POLL_PHY
    i = 0;
    do {
    #endif
        ulPhyIdentifier = fnMIIread(PHY_ADDRESS, PHY_REG_ID1);           // check that the PHY is working correctly by reading its identifier - part 1
        ulPhyIdentifier <<= 16;
        ulPhyIdentifier |= fnMIIread(PHY_ADDRESS, PHY_REG_ID2);          // check that the PHY is working correctly by reading its identifier - part 2

        if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {
    #if defined SCAN_PHY_ADD || defined POLL_PHY
        #if defined SCAN_PHY_ADD
            PHY_ADDRESS++;
            if (PHY_ADDRESS > 31)                                        // all possible PHY addresses have been checked but no match found
        #else
            if (++i > POLL_PHY)
        #endif
            {
    #endif
    #if !defined ALLOW_PHY_ERROR                                         // {125}
                MSCR = 0;                                                // {126} disable clock
                POWER_DOWN(POWER_FEC0);
                return -1;                                               // if the identifier is incorrect the phy isn't responding correctly - return error
    #endif
    #if defined SCAN_PHY_ADD || defined POLL_PHY
            }
    #endif
        }
    #if defined SCAN_PHY_ADD || defined POLL_PHY
        else {
            break;                                                       // the present PHY address has resulted in a match so use this address
        }
    } while (1);
    #endif
#else                                                                    // {157}
    ulPhyIdentifier = fnMIIread(PHY_ADDRESS, PHY_REG_ID1);               // check that the PHY is working correctly by reading its identifier - part 1
    ulPhyIdentifier <<= 16;
    ulPhyIdentifier |= fnMIIread(PHY_ADDRESS, PHY_REG_ID2);              // check that the PHY is working correctly by reading its identifier - part 2
    if ((ulPhyIdentifier & PHY_MASK) != (PHY_IDENTIFIER & PHY_MASK)) {
    #if !defined ALLOW_PHY_ERROR
        MSCR = 0;                                                        // disable clock
        POWER_DOWN(POWER_FEC0);
        return -1;                                                       // if the identifier is incorrect the phy isn't responding correctly - return error
    #endif

    }
#endif

    if ((pars->usSizeRx & 0x0f) != 0) {                                  // ensure size is suitable (divisible by 16)
        pars->usSizeRx &= ~0x0f;
        pars->usSizeRx += 0x10;
    }
#if defined USE_MULTIPLE_BUFFERS
    EMRBR = 256;                                                         // use small multiple buffers for reception
    ucMultiBuf = ((pars->usSizeRx + 128)/256);
    ptrRxBd = ptrBD = (M5223X_FEC_BD *)uMallocAlign((MAX_MALLOC)(sizeof(M5223X_FEC_BD)*NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE*ucMultiBuf), 16); // define memory for rx buffer descriptors (aligned on 16 byte boundary)
#else
    EMRBR = pars->usSizeRx;                                              // set receive buffer size
    ptrRxBd = ptrBD = (M5223X_FEC_BD *)uMallocAlign((MAX_MALLOC)(sizeof(M5223X_FEC_BD) * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 16); // define memory for rx buffer descriptors (aligned on 16 byte boundary)
#endif
    ERDSR = (unsigned long)ptrBD;                                        // set pointer to receive descriptor ring
#if defined USE_MULTIPLE_BUFFERS
    ptrBuffer = uMallocAlign((MAX_MALLOC)(ucMultiBuf*256*NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 16); // get rx buffer memory
    for (i = 0; i < (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf); i++) { // initialise each Rx buffer descriptor
        if (i == ((NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE * ucMultiBuf) - 1)) {
            ptrBD->usBDControl = (EMPTY_BUFFER | WRAP_BIT);
        }
        else {
            ptrBD->usBDControl = EMPTY_BUFFER;
        }
        ptrBD->ptrBD_Data = ptrBuffer;
        ptrBuffer += 256;
        ptrBD++;
    }
#else
    ptrBuffer = (unsigned char *)uMallocAlign((MAX_MALLOC)(pars->usSizeRx * NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE), 16); // get rx buffer memory
    for (i = 0; i < NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE; i++) {      // initialise each Rx buffer descriptor
        if (i == (NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE - 1)) {
            ptrBD->usBDControl = (EMPTY_BUFFER | WRAP_BIT);
        }
        else {
            ptrBD->usBDControl = EMPTY_BUFFER;
        }
        ptrBD->ptrBD_Data = ptrBuffer;
        ptrBuffer += pars->usSizeRx;
        ptrBD++;
    }
#endif

    if ((pars->usSizeTx & 0x03) != 0) {                                  // ensure size is suitable (divisible by 4)
        pars->usSizeTx &= ~0x03;
        pars->usSizeTx += 0x04;
    }
    ptrTxBd = ptrBD = (M5223X_FEC_BD *)uMallocAlign((MAX_MALLOC)(sizeof(M5223X_FEC_BD)*NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE), 16);
    ETDSR = (unsigned long)ptrBD;
    ptrBuffer = (unsigned char *)uMallocAlign((MAX_MALLOC)(pars->usSizeTx*NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE), 4); // get tx buffer memory
    for (i = 0; i < NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE; i++) {
        if (i == (NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE - 1)) {
            ptrBD->usBDControl = (WRAP_BIT);
        }
        else {
            ptrBD->usBDControl = 0;
        }
        ptrBD->ptrBD_Data = ptrBuffer;
        ptrBuffer += pars->usSizeTx;
        ptrBD++;
    }  

#if !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {75}{122}{127}{140}
    IC_ICR_0_36 = EPHY_INTERRUPT_PRIORITY;                               // define EPHY interrupt
    fnSetIntHandler(EPHY_VECTOR, (unsigned char *)_ephy_isr);            // enter the handler routine
    IC_IMRH_0 &= ~(EPHY_PIF_INT_H/* | MASK_ALL_INT*/);                   // unmask interrupt source {41} - the mask all interrupt is only in the low register, this may have been incorrectly opening an unhandled GRA interrupt
#endif
#if defined _M520X                                                       // {127}
    IC_ICR_0_40 = (RXFRAME_INTERRUPT_PRIORITY & LEVEL_MASK);             // define RX Frame interrupt level
    fnSetIntHandler(FEC_RX_F_VECTOR, (unsigned char *)_fec_rx_frame_isr);// enter the handler routine
    IC_IMRH_0 &= ~(FEC_RX_FRAME_PIF_INT_H);                              // unmask interrupt source

    IC_ICR_0_36 = (TXFRAME_INTERRUPT_PRIORITY & LEVEL_MASK);             // define TX Frame interrupt level
    fnSetIntHandler(FEC_TX_F_VECTOR, (unsigned char *)_fec_txf_isr);     // enter the handler routine
    IC_IMRH_0 &= ~(FEC_TX_FRAME_PIF_INT_H);                              // unmask interrupt source
#else
    IC_ICR_0_27 = RXFRAME_INTERRUPT_PRIORITY;                            // define RX Frame interrupt
    fnSetIntHandler(FEC_RX_F_VECTOR, (unsigned char *)_fec_rx_frame_isr);// enter the handler routine
    IC_IMRL_0 &= ~(FEC_RX_FRAME_PIF_INT_L | MASK_ALL_INT);               // unmask interrupt source

    IC_ICR_0_23 = TXFRAME_INTERRUPT_PRIORITY;                            // define TX Frame interrupt
    fnSetIntHandler(FEC_TX_F_VECTOR, (unsigned char *)_fec_txf_isr);     // enter the handler routine
    IC_IMRL_0 &= ~(FEC_TX_FRAME_PIF_INT_L | MASK_ALL_INT);               // unmask interrupt source
#endif
    ECR = ETHER_EN;                                                      // enable FEC for operation

    if (!(pars->usMode & (LAN_10M | LAN_100M))) {                        // {1} when in auto-negotiation mode
        usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_ANAR);                // get auto neg. mode supported by PHY
        if (pars->usMode & RX_FLOW_CONTROL) {
            usMIIData |= PHY_R4_FC;                                      // set flow control
        }
    //  if (!(pars->usMode & FULL_DUPLEX)) {                             // if not full duplex, remove these modes
    //      usMIIData &= ~(PHY_R4_100H | PHY_R4_10H);                    // {11}{35} removed
    //  }
#if !(defined _M5225X || defined _M52XX_SDRAM || defined _M520X || defined _M523X) // {179}
        usMIIData &= ~PHY_R4_NP;                                         // disable this since it is an errata which will probably not be corrected in future silicon
#endif
        fnMIIwrite(PHY_ADDRESS, PHY_REG_ANAR, usMIIData);                // write PHY_REG_ANAR to configure PHY Link abilities advertisement
    }
#if !(defined _M5225X || defined _M52XX_SDRAM || defined _M520X || defined _M523X) // {179}
    else {                                                               // ensure that next page advertisement is disabled to work around an errata and potential problems with gigabit switches
        usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_ANAR);                // get auto neg. mode supported by PHY
        if (usMIIData & PHY_R4_NP) {                                     // if next page advertisement is enabled
            usMIIData &= ~PHY_R4_NP;                                     // disable this since it is an errata which will probably not be corrected in future silicon
            fnMIIwrite(PHY_ADDRESS, PHY_REG_ANAR, usMIIData);            // write PHY_REG_ANAR to configure PHY Link abilities advertisement
        }
    }
#endif
#if defined _M5225X || defined _M52XX_SDRAM || defined  _M523X           // {75}{122}{145}
    #if defined INTERRUPT_TASK_PHY && defined PHY_INTERRUPT && PHY_INTERRUPT != 0
    fnMIIwrite(PHY_ADDRESS, KS8721_INTERRUPT_CONTROL_STATUS_REGISTER,    // enable various PHY interrupts
//             PHY_REMOTE_FAULT_INT_ENABLE
//             PHY_LINK_PARTNER_ACK_INT_ENABLE
//             PHY_PARALLEL_DETECT_FAULT_INT_ENABLE
//             PHY_PAGE_RECEIVED_INT_ENABLE
//             PHY_RECEIVE_ERROR_INT_ENABLE
//             PHY_JABBER_INT_ENABLE
              (PHY_LINK_UP_INT_ENABLE | PHY_LINK_DOWN_INT_ENABLE)
               );
    interrupt_setup.int_type     = PORT_INTERRUPT;                       // identifier to configure edge port interrupt
    interrupt_setup.int_handler  = fnPhyInterrupt;                       // handling function
    interrupt_setup.int_priority = (INTERRUPT_LEVEL_1);                  // interrupt priority level (this cannot be modified for IRQ1..IRQ7 so the value is not really relevant)
    interrupt_setup.int_port_bit = PHY_INTERRUPT;                        // the IRQ input connected
    interrupt_setup.int_port_sense = IRQ_LEVEL_SENSITIVE;                // interrupt is to be level sensitive
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
    #endif
#else                                                                    // enable PHY interrupts in Reg 16 (PHY Interrupt Control Register) and set PHY Interrupt Control Register
    fnMIIwrite(PHY_ADDRESS, PHY_REG_ICR, (PHY_ACKIE | PHY_PRIE | PHY_LCIE | PHY_ANIE | PHY_PDFIE | PHY_RFIE | PHY_JABIE));
#endif

    usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_CR);                      // read PHY Control Register 0

    if (!(pars->usMode & (LAN_10M | LAN_100M))) {
        usMIIData |= (PHY_R0_ANE | PHY_R0_RAN);                          // configure and start auto-negotiation
    }
    else {
        usMIIData &= ~(PHY_R0_ANE | PHY_R0_RAN);                         // {130} ensure that the PHY doesn't perform auto-negotiation
        if (pars->usMode & FULL_DUPLEX) {
            usMIIData |= PHY_R0_DPLX;                                    // set full duplex operation
        }
        else {
            usMIIData &= ~PHY_R0_DPLX;                                   // set half-duplex operation
        }

        if (pars->usMode & LAN_100M) {
            usMIIData |= PHY_R0_DR;                                      // set manual 100Mb
        }
        else {
            usMIIData &= ~PHY_R0_DR;                                     // set manual 10Mb
        }
    }
    fnMIIwrite(PHY_ADDRESS, PHY_REG_CR, usMIIData);                      // command operating mode of PHY
    #if !defined INTERRUPT_TASK_PHY
    EthernetWake = pars->Task_to_wake;                                   // the driver can wake this task to inform of PHY events
    #endif
    RDAR = 0;                                                            // enable RX BD polling

#if defined _WINDOWS
    fnOpenDefaultHostAdapter();
#endif
    return 0;                                                            // {151} ethernet interface was successfully configured
}

#if defined _WINDOWS
    #if !defined USE_IP
    const unsigned char cucBroadcast[MAC_LENGTH] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };  // used also for broadcase IP
    #endif

    #if defined USE_IPV6 || defined USE_IGMP
static int fnIsHashMulticast(unsigned char *ucData)
{
    unsigned long ulCRC32 = 0xffffffff;                                  // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    int i, bit;
    unsigned char ucByte;
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
        if (GALR & (0x00000001 << ulCRC32)) {
            return 1;                                                    // multicast hash match
        }
    }
    else {
        ulCRC32 -= 32;
        if (GAUR & (0x00000001 << ulCRC32)) {
            return 1;                                                    // multicast hash match
        }
    }
    return 0;                                                            // not a match
}
    #endif

    static int fnIsOurMAC(unsigned char *ucData)
    {
        if ((*ucData == (unsigned char)(PALR >> 24)) && (*(ucData + 1) == (unsigned char)(PALR >> 16)) && (*(ucData + 2) == (unsigned char)(PALR >> 8)) && (*(ucData + 3) == (unsigned char)(PALR)) &&
            (*(ucData + 4) == (unsigned char)(PAUR >> 24)) && (*(ucData + 5) == (unsigned char)(PAUR >> 16))) {
            return 1;
        }
        return 0;
    }

    // This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
    // on the programmed MAC address and the Ethernet operating mode
    //
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
    {
        // This function checks to see whether we can receive the simulated frame and can block it if wanted
        if (usLen < ((2 * MAC_LENGTH) + 2)) {
            return 0;                                                    // too short to be a valid Ethernet frame
        }
    #if !defined _DEVELOP_SIM
        if (fnIsOurMAC(&ucData[MAC_LENGTH])) {
            return 0;                                                    // we never accept echos of our own transmissions - {47} check before broadcast
        }
    #endif
        if (!(uMemcmp(ucData, cucBroadcast, MAC_LENGTH))) {
            return 1;                                                    // broadcast
        }
        if (RCR & PROM) {
            return 1;                                                    // in promiscuous mode we accept everything
        }
        if (fnIsOurMAC(&ucData[0])) {
            return 1;                                                    // we accept our mac address
        }
    #if defined USE_IPV6 || defined USE_IGMP
        if (fnIsHashMulticast(&ucData[0])) {
            return 1;                                                    // multicast hash match
        }
    #endif
        return 0;                                                        // otherwise ignore
    }
#endif


// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    volatile int iWait = 5000;                                           // wait until the transmitter is free
    while (ptrTxBd->usBDControl & READY_TX) {
        if (--iWait == 0) {
            return 1;                                                    // we have waited long enough - give up...
        }
    }
    return 0;
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return 1;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return 0;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    if (ptrTxBd == 0) {
        return 0;
    }
    return ptrTxBd->ptrBD_Data;
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
    #if defined USE_MULTIPLE_BUFFERS
    if (iBufNr >= 0) {
        return;                                                          // when using multi-small buffers all processing is internal
    }
    #endif
                                                                         // the buffer has been completely used so free it for use by the ethernet controller
    ptrRxBd->usBDControl |= EMPTY_BUFFER;                                // free the buffer
    #if defined _WINDOWS
    ptrRxBd->usBDControl &= ~(LAST_IN_FRAME | TRUNCATED_FRAME | OVERRUN_FRAME);
    #endif
    if (ptrRxBd->usBDControl & WRAP_BIT) {
        ptrRxBd = (M5223X_FEC_BD*)ERDSR;                                 // set to first BD
    }
    else {
        ptrRxBd++;                                                       // increment to next buffer
    }
    RDAR = 0;                                                            // re-enable buffer operation
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    #define RESYNC_DELAY  4
    static int iUnsynchronised = 0;
    volatile M5223X_FEC_BD *ptrNextBuffer;
    if (ptrTxBd->usBDControl & WRAP_BIT) {                               // prepare local pointer for next write
        ptrNextBuffer = (M5223X_FEC_BD*)ETDSR;
    }
    else {
        ptrNextBuffer = (ptrTxBd + 1);                                   // warning - only valid for 2 Tx buffer case!!!
    }
    if (ptrNextBuffer->usBDControl & READY_TX) {                         // check whether the last buffer is still waiting to be sent
        iUnsynchronised += 2;                                            // we suspect that there is a synchronisation problem becasue the previous frame should have been sent
    }
    else if (iUnsynchronised != 0) {
        iUnsynchronised--;
    }

    while (DataLen < 60) {
        *ptr_put++ = 0x00;                                               // pad with zeros if smaller than 60 [chip must send at least 60]
        DataLen++;
    }

    ptrTxBd->usBDLength = DataLen;                                       // mark length of data to send
    ptrTxBd->usBDControl |= (READY_TX | LAST_IN_FRAME | TX_CRC);         // start transmission
    TDAR = 0;                                                            // enable TX BD operation

    #if defined _WINDOWS
    fnSimulateEthTx(DataLen);
        #if defined PSEUDO_LOOPBACK                                      // {79} if we detect an IP frame being sent to our own address we loop it back to the input
    if (((*(ptr_put - DataLen + 12)) == 0x08) && (!(uMemcmp(&network[DEFAULT_NETWORK].ucOurIP[0], (ptr_put - DataLen + 26), IPV4_LENGTH)))) {
        fnSimulateEthernetIn((ptr_put - DataLen), DataLen, 1);
    }
        #endif
    fec_txf_isr();                                                       // simulate end of transmission - tx interrupt
    ptrTxBd->usBDControl &= ~READY_TX;                                   // free buffer for next transmission
    #endif
    if (ptrTxBd->usBDControl & WRAP_BIT) {                               // prepare local pointer for next write
        ptrTxBd = (M5223X_FEC_BD*)ETDSR;
    }
    else {
        ptrTxBd++;
    }
    if (iUnsynchronised >= RESYNC_DELAY) {
        iUnsynchronised = 0;                                             // reset
        if (ptrTxBd->usBDControl & WRAP_BIT) {                           // correct synchronisation
            ptrTxBd = (M5223X_FEC_BD*)ETDSR;
        }
        else {
            ptrTxBd++;
        }
    }
    return (DataLen);
}
#endif





#if defined SERIAL_INTERFACE
/* =================================================================== */
/*                       Serial Interface - UART                       */
/* =================================================================== */

// Return a pointer to the corresponding UART control block
//
static volatile unsigned char *fnSelectChannel(QUEUE_HANDLE Channel)     // {103}
{
    volatile unsigned char *ptrReg = (unsigned char *)(UART_ADD + UCR_0_OFFSET);
    while (Channel--) {
        ptrReg += SCI_BLOCK_SIZE;
    }
    return (ptrReg);                                                     // set to the command register
}

    #if defined SERIAL_SUPPORT_DMA
// Return a pointer to the corresponding DMA control block [UART0 uses DMA channel 0, UART1 uses DMA channel 1, etc.]
//
static volatile unsigned long *fnSelectDMA(unsigned char ucChannel)
{
    unsigned char *ptrReg = (unsigned char *)DMA_ADD;
    while (ucChannel--) {
        ptrReg += DMA_CHAN_1_OFFSET;
    }
    return ((unsigned long*)ptrReg);                                     // set to the source address register
}
    #endif


// Enable reception on the defined chanel
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    #if (defined CHIP_64_PIN || defined CHIP_80_PIN) && !defined UART2_ON_UB  // {12}{50}
    #define UART2_ON_AS                                                  // default for 80 pin is UART2 on AS port
    #endif

    #if defined _WINDOWS
    unsigned char ucUART_SR;                                             // {26}
    #endif
    volatile unsigned char *ucReg = fnSelectChannel(Channel);            // {103} set register pointer to corresponding block (command register)
    switch (Channel) {                                                   // {109}
    case 0:
    #if defined _M520X                                                   // {127}
        PAR_UART |= PAR_U0RXD;                                           // set RX on UA (no alternative pins available) 
    #elif defined _M523X                                                 // {140}
        PAR_UART |= PAR_U0RXD;
    #else
        PUAPAR |= UA_RX0_FUNCTION;                                       // set RX on UA (no alternative pins available) 
    #endif
        break;
    case 1:
    #if defined _M52XX_SDRAM                                             // {122}
        PUAPAR |= UA_RX1_FUNCTION;
    #elif defined _M520X                                                 // {127}
        PAR_UART |= PAR_U1RXD;
    #elif defined _M523X                                                 // {140}
        PAR_UART |= PAR_U1RXD;
    #else
        #if defined UART1_ON_QS                                          // {12}
        PQSPAR |= QS_RX1_FUNCTION;                                       // set RX on QS
        #else
        PUBPAR |= UB_RX1_FUNCTION;                                       // RX on UB
        #endif
    #endif
        break;
    #if NUMBER_SERIAL > 2                                                // only if third UART is available
    case 2:
        #if defined _M52XX_SDRAM                                         // {122}
            #if defined  UART2_RXON_AS_5
        PASPAR |= AS_RX2_FUNCTION_2;
            #elif defined UART2_RXON_AS_3
        PASPAR |= AS_RX2_FUNCTION_1;
            #else
        PASPAR |= AS_RX2_FUNCTION;
            #endif
        #elif defined _M520X                                             // {127}
            #if defined UART2_ON_TIMER
        PAR_TIMER |= PAR_T1IN_U2RXD;
            #elif defined UART2_ON_FECI2C_2
        PAR_FECI2C |= PAR_MDIO_U2RXD;
            #else
        PAR_FECI2C |= PAR_SDA_U2RXD;
            #endif
        #elif defined _M523X                                             // {140}
        PAR_UART |= PAR_U2RXD;
        #else
            #if defined UART2_ON_AS                                      // {12}
        PASPAR |= AS_RX2_FUNCTION;                                       // set RX on AS
            #elif defined UART2_ON_UB
        PUBPAR |= UB_RX2_FUNCTION;                                       // set RX on UB
            #else
        PUCPAR |= UC_RX2_FUNCTION;                                       // set RX on UC
            #endif
        #endif
        break;
    #endif
    }
    *ucReg = UART_RX_ENABLE;                                             // enable RX interrupt and Rx
    ucReg += (UIMR_UISR_0_OFFSET - UCR_0_OFFSET);
    #if defined _WINDOWS
    ucUART_SR = *ucReg;                                                  // back up the write only register
    #endif
    ucEnabledState[Channel] |= (UART_RXRDY_MASK);                        // we must make a backup since the register cannot be read
    *ucReg = ucEnabledState[Channel];
    #if defined _WINDOWS
    *ucReg = ucUART_SR;                                                  // write only - so remove information
    #endif
}

// Disable reception on the defined chanel (this is called from entry_tty() with disabled interrupts)
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    #if defined _WINDOWS
    unsigned char ucUART_SR;                                             // {26}
    #endif
    volatile unsigned char *ucReg = fnSelectChannel(Channel);            // {103} set register pointer to corresponding block (command register)
    *ucReg = UART_RX_DISABLE;                                            // disable RX interrupt and Rx
    ucReg += (UIMR_UISR_0_OFFSET - UCR_0_OFFSET);
    #if defined _WINDOWS
    ucUART_SR = *ucReg;                                                  // back up the write only register
    #endif
    ucEnabledState[Channel] &= ~UART_RXRDY_MASK;                         // we must make a backup since the register cannot be read
    *ucReg = ucEnabledState[Channel];
    #if defined _WINDOWS
    *ucReg = ucUART_SR;                                                  // write only - so remove information
    #endif
}


// Enable transmission on the defined channel
//
extern void fnTxOn(QUEUE_HANDLE Channel)                                 // only used to configure Tx pins since tx is enabled automatically when sending
{
    switch (Channel) {                                                   // {109}
    case 0:
    #if defined _M520X                                                   // {127}
        PAR_UART |= PAR_U0TXD;
    #elif defined _M523X                                                 // {140}
        PAR_UART |= PAR_U0TXD;
    #else
        PUAPAR |= UA_TX0_FUNCTION;                                       // set TX on UA (no alternative pins available)
    #endif
        break;
    case 1:
    #if defined _M52XX_SDRAM                                             // {122}
        PUAPAR |= UA_TX1_FUNCTION;
    #elif defined _M520X                                                 // {127}
        PAR_UART |= PAR_U1TXD;
    #elif defined _M523X                                                 // {140}
        PAR_UART |= PAR_U1TXD;
    #else
        #if defined UART1_ON_QS                                          // {12}
        PQSPAR |= QS_TX1_FUNCTION;                                       // set TX on QS
        #else
        PUBPAR |= UB_TX1_FUNCTION;                                       // TX on UB
        #endif
    #endif
        break;
    #if NUMBER_SERIAL > 2                                                // only if third UART is available
    case 2:
        #if defined _M52XX_SDRAM                                         // {122}
            #if defined UART2_TXON_AS_4 && defined _M52XX_SDRAM
        PASPAR |= AS_TX2_FUNCTION_2;
            #elif defined UART2_TXON_AS_2
        PASPAR |= AS_TX2_FUNCTION_1;
            #else
        PASPAR |= AS_TX2_FUNCTION;
            #endif
        #elif defined _M520X                                             // {127}
            #if defined UART2_ON_TIMER
        PAR_TIMER |= PAR_T0IN_U2TXD;
            #elif defined UART2_ON_FECI2C_2
        PAR_FECI2C |= PAR_MDC_U2TXD;
            #else
        PAR_FECI2C |= PAR_SCL_U2TXD;
            #endif
        #elif defined _M523X                                             // {140}
        PAR_UART |= PAR_U2TXD;
        #else
            #if defined UART2_ON_AS                                      // {12}
        PASPAR |= AS_TX2_FUNCTION;                                       // set TX on AS
            #elif defined UART2_ON_UB
        PUBPAR |= UB_TX2_FUNCTION;                                       // set TX on UB
            #else
        PUCPAR |= UC_TX2_FUNCTION;                                       // set TX on UC
            #endif
        #endif
        break;
    #endif
    }
}

// Disable transmission on the defined channel
//
extern void fnTxOff(QUEUE_HANDLE Channel)                                // not used since tx is enabled/disabled automatically when sending/completing
{
}

    #if defined SERIAL_SUPPORT_DMA
// Start transfer of a block via DMA
//
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length) // {92}
{ 
    volatile unsigned char *ptrReg = fnSelectChannel(channel);           // {103} set register pointer to corresponding block (command register)
    volatile unsigned long *ptrDMA = fnSelectDMA(channel);               // set DMA register pointer to corresponding block (source address register)

    *ptrDMA++  = (unsigned long)ptrStart;                                // address of first byte to be transferred  (SARn)
    *ptrDMA++  = (unsigned long)(ptrReg + UTB_RB_0_OFFSET - UCR_0_OFFSET); // address of the transmission register  (DARn)
    *ptrDMA    = (DSR_DONE);                                             // clear all status bits
    *ptrDMA++  = (unsigned long)tx_length;                               // the number of byte transfers to be made (SR_BCRn)
    *ptrDMA    = (DCR_INT | DCR_EEXT | DCR_CS | DCR_SINC | DCR_SSIZE_BYTE | DCR_DSIZE_BYTE); // set up DMA operation (DCRn)

    #if defined USER_CODE_TXBYTE_DMA
    USER_CODE_TXBYTE_DMA;
    #endif
    *ptrReg = UART_TX_ENABLE;                                            // enable the transmitter to initiate DMA transfer
        #if defined _WINDOWS
    *(--ptrDMA) |= DSR_BSY;                                              // {100} mark that DMA channel is busy
    switch (channel) {
    case 0:
        iDMA |= DMA_CONTROLLER_0;
        break;
    case 1:
        iDMA |= DMA_CONTROLLER_1;
        break;
    #if NUMBER_SERIAL > 2                                                // only if third UART is available
    case 2:
        iDMA |= DMA_CONTROLLER_2;
        break;
    #endif
    }
        #endif
    return tx_length;
}

        #if defined SERIAL_SUPPORT_XON_XOFF
extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue) // {99}
{
    volatile unsigned long *ptrDMA = fnSelectDMA(channel);               // set DMA register pointer to corresponding block (source address register)
            #if defined _WINDOWS
    unsigned long ulRemaining_Length = (*(ptrDMA + 2) & DMA_BYTE_COUNT_MASK);
    *(ptrDMA + 2) = (DMA_ABORTED | DSR_CE);                              // stop DMA activity, clearing any pending interrupt. This is a pseudo bit for simulator use onl
            #else
    *(ptrDMA + 2) = DSR_DONE;                                            // stop DMA activity, clearing any pending interrupt
            #endif
    if (*ptrDMA == 0) {                                                  // not yet programmed
        return 0;
    }
    if (ptrQueue->get < (unsigned char *)*ptrDMA) {
        return ((QUEUE_TRANSFER)(ptrQueue->buffer_end - (unsigned char *)*ptrDMA)); // return the number of characters of the programmed DMA transfer not yet transmitted
    }
    else {
        return ((QUEUE_TRANSFER)(ptrQueue->get - (unsigned char *)*ptrDMA));
    }
}
        #endif

extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
    volatile unsigned char *ptrReg = fnSelectChannel(channel);           // {103} set register pointer to corresponding block (command register)
    volatile unsigned long *ptrDMA = fnSelectDMA(channel);               // set DMA register pointer to corresponding block (source address register)

    if ((ucEnabledState[channel] & UART_RXRDY_MASK) == 0) {              // {180} if receiver not yet enabled
        fnRxOn(channel);                                                 // configure receiver pin and enable reception and its interrupt/DMA
    }

    *ptrDMA++  = (unsigned long)(ptrReg + UTB_RB_0_OFFSET - UCR_0_OFFSET); // address of source register (SARn)
    *ptrDMA++  = (unsigned long)ptrStart;                                // address of the destination (DARn)
    *ptrDMA    = (DSR_DONE);                                             // clear all status bits
    *ptrDMA++  = (unsigned long)rx_length;                               // the number of byte transfers to be made (SR_BCRn)
    *ptrDMA    = (DCR_INT | DCR_EEXT | DCR_CS | DCR_DINC | DCR_SSIZE_BYTE | DCR_DSIZE_BYTE); // set up DMA operation and enable (DCRn)
        #if defined UART_BREAK_SUPPORT                                   // {51}
    *ptrReg = UART_RX_ENABLE;                                            // enable RX with break detect interrupt
    ptrReg += (UIMR_UISR_0_OFFSET - UCR_0_OFFSET);
    ucEnabledState[channel] |= (UART_BD_MASK);                           // we must make a backup since the register cannot be read
    *ptrReg = ucEnabledState[channel];
            #if defined _WINDOWS
    *ptrReg &= ~UART_BD_MASK;                                            // write only - so remove information
            #endif
        #endif
}

        #if defined UART_BREAK_SUPPORT                                        
extern QUEUE_TRANSFER fnGetDMACount(QUEUE_HANDLE channel, QUEUE_TRANSFER max_count) // {51}
{
    volatile unsigned long *ptrDMA = fnSelectDMA(channel);               // set DMA register pointer to corresponding block (source address register)
    QUEUE_TRANSFER DMA_remaining;

    ptrDMA += 2;                                                         // set to count remaining
    DMA_remaining = (QUEUE_TRANSFER)(*ptrDMA & DMA_BYTE_COUNT_MASK);
    if (max_count == DMA_remaining) {
        return 0;
    }
    *ptrDMA = (DSR_DONE);                                                // stop DMA activity
    return (QUEUE_TRANSFER)(max_count - DMA_remaining);                  // the number of characters actually waiting
}
        #endif

static __interrupt__ void _DMA0_Tx_handler(void)                         // tx buffer transmission completed on SCI0
{
    DMA_DCR0 = 0;                                                        // disable DMA
    UCR_0 = UART_TX_DISABLE;                                             // disable tx 
    DMA_SR_BCR0 = DSR_DONE;                                              // reset DMA status
        #if defined _WINDOWS
    DMA_SR_BCR0 &= ~DSR_DONE;                                            // {100}
        #endif
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutine
    fnSciTxByte(0);                                                      // tty block transferred, send next, if available
    iInterruptLevel = 0;
}

static __interrupt__ void _DMA1_Tx_handler(void)                         // {74} tx buffer transmission completed on SCI1
{
    DMA_DCR1 = 0;                                                        // disable DMA
    UCR_1 = UART_TX_DISABLE;                                             // disable tx 
    DMA_SR_BCR1 = DSR_DONE;                                              // reset DMA status
        #if defined _WINDOWS
    DMA_SR_BCR1 &= ~DSR_DONE;                                            // {100}
        #endif
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutine
    fnSciTxByte(1);                                                      // tty block transferred, send next, if available
    iInterruptLevel = 0;
}

static __interrupt__ void _DMA2_Tx_handler(void)                         // tx buffer transmission completed on SCI2
{
    DMA_DCR2 = 0;                                                        // disable DMA
    UCR_2 = UART_TX_DISABLE;                                             // {90} disable tx 
    DMA_SR_BCR2 = DSR_DONE;                                              // reset DMA status
        #if defined _WINDOWS
    DMA_SR_BCR2 &= ~DSR_DONE;                                            // {100}
        #endif
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutine
    fnSciTxByte(2);                                                      // tty block transferred, send next, if available
    iInterruptLevel = 0;
}

static __interrupt__ void _DMA1_Rx_handler(void)                         // rx buffer reception completed on SCI1
{
    DMA_SR_BCR1 = DSR_DONE;
        #if defined _WINDOWS
    DMA_SR_BCR1 &= ~DSR_DONE;                                            // {100}
        #endif
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutine
    fnSciRxByte(0, 1);                                                   // tty block ready for read
    iInterruptLevel = 0;
} 

        #if defined SERIAL_SUPPORT_XON_XOFF
extern QUEUE_TRANSFER fnRemainingDMA_tx(QUEUE_HANDLE channel)            // {99}
{
    /*
    volatile unsigned long *ptrDMA = fnSelectDMA(channel);               // set DMA register pointer to corresponding block (source address register)
    unsigned long ulCount = *(ptrDMA + 2);                               // sample status register with byte count
    if (ulCount & (DSR_BSY | DSR_DONE)) {                                // transfer is either still in progress or terminated, but its interrupt not handled
        if (ulCount & DSR_CE) {                                          // an error status due to XOFF abort
            *(ptrDMA + 2) = DSR_DONE;                                    // clear
        #if defined _WINDOWS
            *(ptrDMA + 2) = 0;
        #endif
            return 0;                                                    // not in user
        }
        ulCount &= DMA_BYTE_COUNT_MASK;
        if (ulCount == 0) {                                              // terminated so report one byte (probably still being transmitted)
            ulCount++;
        }
        return ((QUEUE_TRANSFER)ulCount);                                // return the remaining count value
    }*/
    return 0;                                                            // not in use
}
        #endif
    #endif                                                               // end SERIAL_SUPPORT_DMA


#if defined SUPPORT_HW_FLOW                                              // {25}
    #if defined UART_BREAK_SUPPORT                                       // {51}
        #define UART_INTERRUPTS (UART_RXRDY_MASK | UART_TXRDY_MASK | UART_COS_MASK | UART_BD_MASK)
        #define UART_RX_INTERRUPTS  (UART_RXRDY_MASK | UART_BD_MASK)
    #else
        #define UART_INTERRUPTS (UART_RXRDY_MASK | UART_TXRDY_MASK | UART_COS_MASK)
        #define UART_RX_INTERRUPTS  UART_RXRDY_MASK
    #endif
#else
    #if defined UART_BREAK_SUPPORT                                       // {51}
        #define UART_INTERRUPTS (UART_RXRDY_MASK | UART_TXRDY_MASK | UART_BD_MASK)
        #define UART_RX_INTERRUPTS  (UART_RXRDY_MASK | UART_BD_MASK)
    #else
        #define UART_INTERRUPTS (UART_RXRDY_MASK | UART_TXRDY_MASK)
        #define UART_RX_INTERRUPTS  UART_RXRDY_MASK
    #endif
#endif

/**************************** SCI0 interrupt ******************************************/

static __interrupt__ void _SCI0_Interrupt(void)
{
    unsigned char ucState;
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutines
    while ((ucState = (unsigned char)(UIMR_UISR_0 & UART_INTERRUPTS)) != 0) { // while interrupts present
        if (ucState & UART_RX_INTERRUPTS) {
    #if defined UART_BREAK_SUPPORT                                       // {51}
            if (USR_UCSR_0 & (UART_FE | UART_PE | UART_RB)) {            // receive byte error - don't put it into the buffer
                volatile unsigned char ucDummy = UTB_RB_0;               // read to clear interrupt
                if (USR_UCSR_0 & UART_RB) {                              // receive break character received
                     fnSciRxMsg(0);
                }
            }
            else {
                if (USR_UCSR_0 & UART_RXRDY) {
                    fnSciRxByte(UTB_RB_0, 0);                            // receive data interrupt - read the byte
                }
                else {                                                   // must be break (change - either beginning or end)
                    UCR_0 = UART_RESET_BRK;                              // reset break change interrupt
                    fnSciRxMsg(0);                                      
                }
            }
    #else
            fnSciRxByte(UTB_RB_0, 0);                                    // receive data interrupt - read the byte
        #if defined _WINDOWS
            UIMR_UISR_0 &= ~(UART_RXRDY_MASK);                           // simulate reset of interrupt flag
        #endif
    #endif
        }

        if (ucState & UART_TXRDY_MASK) {
    #if defined _WINDOWS
            USR_UCSR_0 |= UART_TXRDY;                                    // simulate tx buffer being freed
    #endif
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
    #if defined _WINDOWS
            UIMR_UISR_0 &= ~UART_TXRDY_MASK;                             // simulate tx interrupt being cleared
    #endif
        }

    #if defined SUPPORT_HW_FLOW                                          // {25}
        if (ucState & UART_COS_MASK) {
            fnRTS_change(0, ((UIPCR_UACR_0 & UART_CTS_IN) == 0));        // control transmission according to state         
        #if defined _WINDOWS
            UIMR_UISR_0 &= ~UART_COS_MASK;                               // simulate CTS change interrupt being cleared
        #endif
        }
    #endif
    }
    iInterruptLevel = 0;
}

/**************************** SCI1 interrupt ******************************************/

static __interrupt__ void _SCI1_Interrupt(void)
{
    volatile unsigned char ucState;
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutines
    while ((ucState = (unsigned char)(UIMR_UISR_1 & UART_INTERRUPTS)) != 0) { // while interrupts present
        if (ucState & UART_RX_INTERRUPTS) {
    #if defined UART_BREAK_SUPPORT                                       // {51}
            if (USR_UCSR_1 & (UART_FE | UART_PE | UART_RB)) {            // receive byte error - don't put it into the buffer
                volatile unsigned char ucDummy = UTB_RB_1;               // read to clear interrupt
                if (USR_UCSR_1 & UART_RB) {                              // receive break character received
                     fnSciRxMsg(1);
                }
            }
            else {
                if (USR_UCSR_1 & UART_RXRDY) {
                    fnSciRxByte(UTB_RB_1, 1);                            // receive data interrupt - read the byte
                }
                else {                                                   // must be break (change - either beginning or end)
                    UCR_1 = UART_RESET_BRK;                              // reset break change interrupt
                    fnSciRxMsg(1);                                      
                }
            }
    #else
            fnSciRxByte(UTB_RB_1, 1);                                    // receive data interrupt - read the byte
        #if defined _WINDOWS
            UIMR_UISR_1 &= ~(UART_RXRDY_MASK);                           // simulate reset of interrupt flag
        #endif
    #endif
        }

        if (ucState & UART_TXRDY_MASK) {
    #if defined _WINDOWS
            USR_UCSR_1 |= UART_TXRDY;                                    // simulate tx buffer being freed
    #endif
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
    #if defined _WINDOWS
            UIMR_UISR_1 &= ~UART_TXRDY_MASK;                             // simulate tx interrupt being cleared
    #endif
        }

    #if defined SUPPORT_HW_FLOW                                          // {25}
        if (ucState & UART_COS_MASK) {
            fnRTS_change(1, ((UIPCR_UACR_1 & UART_CTS_IN) == 0));        // control transmission according to state         
        #if defined _WINDOWS
            UIMR_UISR_1 &= ~UART_COS_MASK;                               // simulate CTS change interrupt being cleared
        #endif
        }
    #endif
    }
    iInterruptLevel = 0;
}

/**************************** SCI2 interrupt ******************************************/
#if NUMBER_SERIAL > 2
static __interrupt__ void _SCI2_Interrupt(void)
{
    unsigned char ucState;
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutines
    while ((ucState = (unsigned char)(UIMR_UISR_2 & UART_INTERRUPTS)) != 0) { // while interrupts present
        if (ucState & UART_RX_INTERRUPTS) {
    #if defined UART_BREAK_SUPPORT                                       // {51}
            if (USR_UCSR_2 & (UART_FE | UART_PE | UART_RB)) {            // receive byte error - don't put it into the buffer
                volatile unsigned char ucDummy = UTB_RB_2;               // read to clear interrupt
                if (USR_UCSR_2 & UART_RB) {                              // receive break character received
                     fnSciRxMsg(2);
                }
            }
            else {
                if (USR_UCSR_2 & UART_RXRDY) {
                    fnSciRxByte(UTB_RB_2, 2);                            // receive data interrupt - read the byte
                }
                else {                                                   // must be break (change - either beginning or end)
                    UCR_2 = UART_RESET_BRK;                              // reset break change interrupt
                    fnSciRxMsg(2);                                      
                }
            }
    #else
            fnSciRxByte(UTB_RB_2, 2);                                    // receive data interrupt - read the byte
        #if defined _WINDOWS
            UIMR_UISR_2 &= ~(UART_RXRDY_MASK);                           // simulate reset of interrupt flag
        #endif
    #endif
        }

        if (ucState & UART_TXRDY_MASK) {
    #if defined _WINDOWS
            USR_UCSR_2 |= UART_TXRDY;                                    // simulate tx buffer being freed
    #endif
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
    #if defined _WINDOWS
            UIMR_UISR_2 &= ~UART_TXRDY_MASK;                             // simulate tx interrupt being cleared
    #endif
        }

    #if defined SUPPORT_HW_FLOW                                          // {25}
        if (ucState & UART_COS_MASK) {
            fnRTS_change(2, ((UIPCR_UACR_2 & UART_CTS_IN) == 0));        // control transmission according to state         
        #if defined _WINDOWS
            UIMR_UISR_2 &= ~UART_COS_MASK;                               // simulate CTS change interrupt being cleared
        #endif
        }
    #endif
    }
    iInterruptLevel = 0;
}
#endif

#if defined UART_EXTENDED_MODE
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel)            // {106} dummy
{
    return 0;
}
#endif

// The TTY driver uses this call to disable the transmit interrupt of the serial port (this is called from entry_tty() with disabled interrupts)
//
extern void fnClearTxInt(QUEUE_HANDLE Channel)
{
    unsigned char *ptrReg = (unsigned char *)(UART_ADD + UIMR_UISR_0_OFFSET); // set to the interrupt mask register
  //unsigned char ucEnabled = (ucEnabledState[Channel] &= ~UART_TXRDY_MASK);  // {138} we must make a backup since the register cannot be read
    unsigned char ucEnabled;                                             // {138}
    #if defined _WINDOWS
    unsigned char ucTemp;
    #endif
    ucEnabledState[Channel] &= ~UART_TXRDY_MASK;                         // {138}
    ucEnabled = ucEnabledState[Channel];                                 // {138}
    while (Channel--) {
        ptrReg += SCI_BLOCK_SIZE;
    }
    #if defined _WINDOWS
    ucTemp = *ptrReg;                                                    // backup the status
    #endif
    *ptrReg = ucEnabled;                                                 // disable the interrupt
    #if defined _WINDOWS
    *ptrReg = ucTemp;                                                    // return the status
    #endif
    ptrReg -= (UIMR_UISR_0_OFFSET - UCR_0_OFFSET);
    *ptrReg = UART_TX_DISABLE;                                           // disable TX in order to remove main interrupt
}

// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE Channel, unsigned char ucTxByte)
{
    volatile unsigned char *ptrReg = fnSelectChannel(Channel);           // {103} set register pointer to corresponding block (command register)
    #if defined _WINDOWS
    unsigned char ucTemp;
    #endif

    if (!(ucEnabledState[Channel] & UART_TXRDY_MASK)) {
        *ptrReg = UART_TX_ENABLE;                                        // enable the transmitter
    #if defined _WINDOWS
        ptrReg -= (UCR_0_OFFSET - USR_UCSR_0_OFFSET);                    // simulate the TXRDY bit being set on first enable
        *ptrReg |= UART_TXRDY;
        ptrReg += (UCR_0_OFFSET - USR_UCSR_0_OFFSET);
    #endif
    }

    ptrReg -= (UCR_0_OFFSET - USR_UCSR_0_OFFSET);

    if (!(UART_TXRDY & *ptrReg)) {
        return 1;                                                        // busy for the moment
    }
    #if defined _WINDOWS
    *ptrReg &= ~UART_TXRDY;
    #endif 

    ptrReg += (UTB_RB_0_OFFSET - USR_UCSR_0_OFFSET);                     // set to transmit byte register
    *ptrReg = ucTxByte;                                                  // send byte
    ptrReg += (UIMR_UISR_0_OFFSET - UTB_RB_0_OFFSET);                    // move to interrupt mask register
    ucEnabledState[Channel] |= UART_TXRDY_MASK;                          // we must make a backup since the register cannot be read
    #if defined _WINDOWS
    ucTemp = *ptrReg;                                                    // backup original status
    #endif
    *ptrReg = ucEnabledState[Channel];                                   // enable the interrupt

    #if defined _WINDOWS
    *ptrReg = ucTemp;                                                    // restore original status
    switch (Channel) {
    case 0:
        iInts |= CHANNEL_0_SERIAL_INT;                                   // simulate interrupt
        ucTxLast[0] = UTB_RB_0;                                          // back up the data written so that it can't get lost when rx data uses the simulated register {27}
        break;
    case 1:
        iInts |= CHANNEL_1_SERIAL_INT;                                   // simulate interrupt
        ucTxLast[1] = UTB_RB_1;                                          // back up the data written so that it can't get lost when rx data uses the simulated register {27}
        break;
        #if NUMBER_SERIAL > 2                                            // only if third UART is available
    case 2:
        iInts |= CHANNEL_2_SERIAL_INT;                                   // simulate interrupt
        ucTxLast[2] = UTB_RB_2;                                          // back up the data written so that it can't get lost when rx data uses the simulated register {27}
        break;
        #endif
    }
    #endif
    return 0;
}

    #if defined UART_BREAK_SUPPORT
extern void fnStartBreak(QUEUE_HANDLE channel)
{
   volatile unsigned char *ucReg = fnSelectChannel(channel);             // {103} set register pointer to corresponding block (command register)

    #if defined USER_CODE_START_BREAK
    USER_CODE_START_BREAK;
    #endif

  //*ucReg = UART_START_BRK;                                             // start sending break on tx line
    *ucReg = (UART_START_BRK | UART_TX_ENABLE);                          // {105} start sending break on tx line

        #if defined _WINDOWS
    fnControlBreak(channel, 1);
        #endif
}

extern void fnStopBreak(QUEUE_HANDLE channel)
{
    volatile unsigned char *ucReg = fnSelectChannel(channel);            // {103} set register pointer to corresponding block (command register)
  //*ucReg = UART_STOP_BRK;                                              // start sending break on tx line
    *ucReg = (UART_STOP_BRK | UART_TX_DISABLE);                          // {105} start sending break on tx line

        #if defined _WINDOWS
    fnControlBreak(channel, 0);
        #endif
}
    #endif                                                               // end UART_BREAK_SUPPORT


// Configure the SCI hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    volatile unsigned char *ucReg = fnSelectChannel(Channel);            // {103} set register pointer to corresponding block (command register)
    unsigned short usSpeed;
    unsigned char ucBits = UART_8_BIT;
    #if defined SERIAL_SUPPORT_DMA
    unsigned char ucUseDMA = pars->ucDMAConfig & (UART_TX_DMA | UART_RX_DMA);
    if (ucUseDMA) {                                                      // enable backdoor access and secondary port valid (RAMBAR)
        GPACR0 = SUP_USER_FULL_ACCESS;                                   // enable peripheral SRAM access
        POWER_UP(POWER_DMA);                                             // {46}
    }
    #endif

    switch (Channel) {                                                   // first configure the GPIO pins for UART use
    case 0:
        POWER_UP(POWER_UART0);                                           // enable clock to module {46}
    #if defined SERIAL_SUPPORT_DMA
        if (ucUseDMA & UART_RX_DMA) {
            // add if required (limited DMA channels available)
        }
        if (ucUseDMA & UART_TX_DMA) {
            DMAREQC |= (DMA_UART_0_TX << DMAC_0_SHIFT);                  // select DMA matrix for Tx on DMA channel 0
            PACR_UART0 |= (SUP_USER_FULL_ACCESS << UART0_ACCESS_SHIFT);  // enable DMA access to UART0
    #if defined _M520X
            IC_ICR_0_8 = (DMA0_INTERRUPT_PRIORITY & LEVEL_MASK);         // define interrupt level
    #else
            IC_ICR_0_9 = DMA0_INTERRUPT_PRIORITY;                        // define interrupt level and priority
    #endif
            fnSetIntHandler(DMA0_VECTOR, (unsigned char *)_DMA0_Tx_handler);// enter the handler routine
            IC_IMRL_0 &= ~(DMA0_PIF_INT_L | MASK_ALL_INT);               // unmask interrupt source
        }
        if ((ucUseDMA & (UART_TX_DMA | UART_RX_DMA)) == (UART_TX_DMA | UART_RX_DMA)) {
            break;                                                       // no UART interrupt handling required
        }
    #endif
    #if defined _M520X
        IC_ICR_0_26 = (UART0_INTERRUPT_PRIORITY & LEVEL_MASK);           // define interrupt level
    #else
        IC_ICR_0_13 = UART0_INTERRUPT_PRIORITY;                          // define interrupt level and priority
    #endif
        fnSetIntHandler(UART0_VECTOR, (unsigned char *)_SCI0_Interrupt); // enter the handler routine
        IC_IMRL_0 &= ~(UART0_PIF_INT_L | MASK_ALL_INT);                  // unmask interrupt source
        break;

    case 1:
        POWER_UP(POWER_UART1);                                           // enable clock to module {46}
    #if defined SERIAL_SUPPORT_DMA
        if (ucUseDMA & UART_RX_DMA) {
            DMAREQC |= (DMA_UART_1_RX << DMAC_1_SHIFT);                  // select DMA matrix for Rx on DMA channel 1
            PACR_UART1  |= (SUP_USER_FULL_ACCESS << UART1_ACCESS_SHIFT); // enable DMA access to UART1
        #if defined _M520X
            IC_ICR_0_9 = (DMA1_INTERRUPT_PRIORITY & LEVEL_MASK);         // define interrupt level
        #else
            IC_ICR_0_10 = DMA1_INTERRUPT_PRIORITY;                       // define interrupts level and priority
        #endif
            fnSetIntHandler(DMA1_VECTOR, (unsigned char *)_DMA1_Rx_handler);// enter the handler routine
            IC_IMRL_0 &= ~(DMA1_PIF_INT_L | MASK_ALL_INT);               // unmask interrupt source
        }
        if (ucUseDMA) {
            if (ucUseDMA & UART_TX_DMA) {                                // {74}
                DMAREQC |= (DMA_UART_1_TX << DMAC_1_SHIFT);              // select DMA matrix for Tx on DMA channel 1
                PACR_UART1 |= (SUP_USER_FULL_ACCESS << UART1_ACCESS_SHIFT); // enable DMA access to UART0
        #if defined _M520X
                IC_ICR_0_9 = (DMA1_INTERRUPT_PRIORITY & LEVEL_MASK);     // define interrupt level
        #else
                IC_ICR_0_10 = DMA1_INTERRUPT_PRIORITY;                    // define interrupts level and priority
        #endif
                fnSetIntHandler(DMA1_VECTOR, (unsigned char *)_DMA1_Tx_handler);// enter the handler routine
                IC_IMRL_0 &= ~(DMA1_PIF_INT_L | MASK_ALL_INT);           // unmask interrupt source
            }
        }
        if ((ucUseDMA & (UART_TX_DMA | UART_RX_DMA)) == (UART_TX_DMA | UART_RX_DMA)) {
            break;                                                       // no UART interrupt handling required
        }
    #endif
    #if defined _M520X
        IC_ICR_0_27 = (UART1_INTERRUPT_PRIORITY & LEVEL_MASK);           // define interrupt level
    #else
        IC_ICR_0_14 = UART1_INTERRUPT_PRIORITY;                          // define interrupt level and priority
    #endif
        fnSetIntHandler(UART1_VECTOR, (unsigned char *)_SCI1_Interrupt); // enter the handler routine
        IC_IMRL_0 &= ~(UART1_PIF_INT_L | MASK_ALL_INT);                  // unmask interrupt source
        break;

    #if NUMBER_SERIAL > 2                                                // only if third UART is available
    case 2:
        POWER_UP(POWER_UART2);                                           // enable clock to module {46}
        #if defined SERIAL_SUPPORT_DMA
        if (ucUseDMA & UART_RX_DMA) {
            // add if required (limited DMA channels available)
        }
        if (ucUseDMA & UART_TX_DMA) {
            DMAREQC |= (DMA_UART_2_TX << DMAC_2_SHIFT);                  // select DMA matrix for Tx on DMA channel 2
            PACR_UART2  |= (SUP_USER_FULL_ACCESS << UART2_ACCESS_SHIFT); // enable DMA access to UART2
            #if defined _M520X
            IC_ICR_0_10 = (DMA2_INTERRUPT_PRIORITY & LEVEL_MASK);        // define interrupt level
            #else
            IC_ICR_0_11 = DMA2_INTERRUPT_PRIORITY;                       // define interrupts level and priority
            #endif
            fnSetIntHandler(DMA2_VECTOR, (unsigned char *)_DMA2_Tx_handler);// enter the handler routine
            IC_IMRL_0 &= ~(DMA2_PIF_INT_L | MASK_ALL_INT);               // unmask interrupt source
        }
        if ((ucUseDMA & (UART_TX_DMA | UART_RX_DMA)) == (UART_TX_DMA | UART_RX_DMA)) {
            break;                                                       // no UART interrupt handling required
        }
        #endif
        #if defined _M520X
        IC_ICR_0_28 = (UART2_INTERRUPT_PRIORITY & LEVEL_MASK);           // define interrupt level
        #else
        IC_ICR_0_15 = UART2_INTERRUPT_PRIORITY;                          // define interrupt level and priority
        #endif
        fnSetIntHandler(UART2_VECTOR, (unsigned char *)_SCI2_Interrupt); // enter the handler routine
        IC_IMRL_0 &= ~(UART2_PIF_INT_L | MASK_ALL_INT);                  // unmask interrupt source
        break;
    #endif

    default:
        return;                                                          // invalid UART channel
    }

    #if defined SUPPORT_HW_FLOW                                          // {81}
    if (pars->Config & RTS_CTS) {                                        // {104} HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    *ucReg = UART_RESET_RX;                                              // reset rx and tx and ensure internal command pointer at start
    *ucReg = UART_RESET_TX;
    *ucReg = UART_RESET_CMD_PTR;

    ucReg -= (UCR_0_OFFSET - USR_UCSR_0_OFFSET);                         // set to clock select register
    *ucReg = (UART_RX_BUS_CLK | UART_TX_BUS_CLK);                        // use internal bus clock
    #if defined _WINDOWS
    *ucReg = 0;                                                          // clear since write only
    #endif
    ucReg += (UBG1_0_OFFSET - USR_UCSR_0_OFFSET);                        // move to BRG register

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        usSpeed = (UART_CLOCK/32 + 300/2)/300;                           // set 300
        break;
    case SERIAL_BAUD_600:
        usSpeed = (UART_CLOCK/32 + 600/2)/600;                           // set 600
        break;
    case SERIAL_BAUD_1200:
        usSpeed = (UART_CLOCK/32 + 1200/2)/1200;                         // set 1200
        break;
    case SERIAL_BAUD_2400:
        usSpeed = (UART_CLOCK/32 + 2400/2)/2400;                         // set 2400
        break;
    case SERIAL_BAUD_4800:
        usSpeed = (UART_CLOCK/32 + 4800/2)/4800;                         // set 4800
        break;
    case SERIAL_BAUD_9600:
        usSpeed = (UART_CLOCK/32 + 9600/2)/9600;                         // set 9600
        break;
    case SERIAL_BAUD_14400:
        usSpeed = (UART_CLOCK/32 + 14400/2)/14400;                       // set 14400
        break;
    default:                                                             // if not valid value set this one
    case SERIAL_BAUD_19200:
        usSpeed = (UART_CLOCK/32 + 19200/2)/19200;                       // set 19200
        break;
    case SERIAL_BAUD_38400:
        usSpeed = (UART_CLOCK/32 + 38400/2)/38400;                       // set 38400
        break;
    case SERIAL_BAUD_57600:
        usSpeed = (UART_CLOCK/32 + 57600/2)/57600;                       // set 57600
        break;
    case SERIAL_BAUD_115200:
        usSpeed = (UART_CLOCK/32 + 115200/2)/115200;                     // set 115200
        break;
    case SERIAL_BAUD_230400:
        usSpeed = (UART_CLOCK/32 + 230400/2)/230400;                     // set 230400
        break;
    case SERIAL_BAUD_250K:
        usSpeed = (UART_CLOCK/32 + 250000/2)/250000;                     // set 250k
        break;
    }
    *ucReg = (unsigned char)(usSpeed >> 8);
    ucReg += (UBG2_0_OFFSET - UBG1_0_OFFSET);
    *ucReg = (unsigned char)(usSpeed);                                   // baud rate set
    ucReg -= (UBG2_0_OFFSET - UMR1_2_0_OFFSET);                          // set to Mode register

    if (pars->Config & CHAR_7) {                                         // {104}
        ucBits = UART_7_BIT;
    }

    #if defined SUPPORT_HW_FLOW                                          // {25}
    if (pars->Config & RTS_CTS) {                                        // {104}
        ucBits |= RXRTS;                                                 // enable automatic negation of RTS when the input FIFO is full
    }
    #endif

    if (pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {         // {104}
        if (pars->Config & RS232_ODD_PARITY) {                           // {104}
          *ucReg = (unsigned char)(UART_WITH_PARITY | ucBits | ODD_PARITY); // 7/8 bits - odd parity
        }
        else {
          *ucReg = (unsigned char)(UART_WITH_PARITY | ucBits);           // 7/8 bits - even parity
        }
    }
    else {
        *ucReg = (unsigned char)(UART_NO_PARITY | ucBits/* | 0x0f*/);  /* TEST MULTIDROP */             // 7/8 bits - no parity
    }

    if (pars->Config & TWO_STOPS) {                                      // {104} the mode register 2 has been selected by previous write
        ucBits = UART_TWO_STOP;
    }
    else if (pars->Config & ONE_HALF_STOPS) {                            // {104}
        ucBits = UART_ONE_HALF_STOP;
    }
    else {
        ucBits = UART_ONE_STOP;
    }

    #if defined SUPPORT_HW_FLOW && defined SERIAL_SUPPORT_DMA            // {97}
    if ((pars->Config & RTS_CTS) && (ucUseDMA & UART_TX_DMA)) {          // {104} RTS/CTS flow control used with DMA transmission
        ucBits |= TXCTS;                                                 // enable automatic negation of RTS when the input FIFO is full
    }
    #endif
    *ucReg = ucBits;

    #if defined _WINDOWS
    fnConfigSimSCI(Channel, UART_CLOCK/(usSpeed*32), pars);              // open a serial port on PC if desired
    #endif
}


    #if defined SUPPORT_HW_FLOW                                          // {25}
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode) // {82}
{
    volatile unsigned char *ucReg = fnSelectChannel(channel);            // {103}

    if (usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) {           // {81}
        if (usModifications & CONFIG_RTS_PIN) {
            switch (channel) {
            case 0:
                PUAPAR |= UA_RTS0_FUNCTION;
                break;
            case 1:
        #if defined UART1_ON_QS
                PQSPAR |= QS_RTS1_FUNCTION;
        #else
                PUBPAR |= UB_RTS1_FUNCTION;
        #endif
                break;
        #if NUMBER_SERIAL > 2                                            // only if third UART is available
            #if !defined CHIP_80_PIN && !defined CHIP_64_PIN && !defined CHIP_81_PIN  // HW flow control not possible on 80 pin device
            case 2:
                PUCPAR |= UC_RTS2_FUNCTION;
                break;
            #endif
        #endif
            }
        }
        if (usModifications & CONFIG_CTS_PIN) {
            switch (channel) {
            case 0:
                PUAPAR |= UA_CTS0_FUNCTION;
                break;
            case 1:
        #if defined UART1_ON_QS
                PQSPAR |= QS_CTS1_FUNCTION;
        #else
                PUBPAR |= UB_CTS1_FUNCTION;
        #endif
                break;
        #if NUMBER_SERIAL > 2                                            // only if third UART is available
            #if !defined CHIP_80_PIN && !defined CHIP_64_PIN && !defined CHIP_81_PIN  // HW flow control not possible on 80 pin device
            case 2:
                PUCPAR |= UC_CTS2_FUNCTION;
                break;
            #endif
        #endif
            }
        }
    }
                                                                         // SET_CTS may be used in DCE mode and results in the same action as SET_RTS
    if (usModifications & (SET_RTS | SET_CTS)) {                         // assert RTS output signal
        *(ucReg + (0x38 - UCR_0_OFFSET)) = UART_SET_RESET_RTS;           // write corresponding UOP1 register
    }
    if (usModifications & (CLEAR_RTS | CLEAR_CTS)) {                     // negate RTS output signal
        *(ucReg + (0x3c - UCR_0_OFFSET)) = UART_SET_RESET_RTS;           // write corresponding UOP0 register
    }
}
 
// Modify control line interrupt operation (this is called from entry_tty() with disabled interrupts)
//
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode) // {82}
{
    #if defined _WINDOWS
    unsigned char ucUART_SR;
    #endif
    QUEUE_TRANSFER ControlLines = 0;
    volatile unsigned char *ucReg = fnSelectChannel(channel);            // {103} set register pointer to corresponding block (command register)

    if (usModifications & ENABLE_CTS_CHANGE) {
        ucEnabledState[channel] |= (UART_COS_MASK);                      // we must make a backup since the register cannot be read
    }
    if (usModifications & DISABLE_CTS_CHANGE) {
        ucEnabledState[channel] &= ~(UART_COS_MASK);                     // we must make a backup since the register cannot be read
    }
    #if defined _WINDOWS
    ucUART_SR = *(ucReg + (0x10 - UCR_0_OFFSET));                        // present port change register contents (shares register)
    #endif
    *(ucReg + (0x10 - UCR_0_OFFSET)) = UART_IEC;                         // enable control line change to generate an interrupt
    #if defined _WINDOWS
    *(ucReg + (0x10 - UCR_0_OFFSET)) = ucUART_SR;                        // present port change register contents (shares register)
    ucUART_SR = *(ucReg + (UIMR_UISR_0_OFFSET - UCR_0_OFFSET));          // present status register contents
    #endif
    *(ucReg + (UIMR_UISR_0_OFFSET - UCR_0_OFFSET)) = ucEnabledState[channel];
    #if defined _WINDOWS
    *(ucReg + (UIMR_UISR_0_OFFSET - UCR_0_OFFSET)) = ucUART_SR;          // restore original content since write only
    #endif
    
    if (!(*(ucReg + (0x34 - UCR_0_OFFSET)) & UART_CTS_STATE)) {          // check corresponding UIP register
        ControlLines |= SET_CTS;                                         // input is presently asserted
    }

    return ControlLines;
}
    #endif                                                               // end SUPPORT_HW_FLOW

#endif                                                                   // end SERIAL_INTERFACE



#if (defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES) && !defined _M520X && !defined _M523X // {68}{127}
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */

static void fnConfigFlash(void)
{
    CFMUSTAT = (ACCERR | PVIOL);                                         // clear error flags

    if (CFMCLKD & DIVLD) return;                                         // already configured

#if BUS_CLOCK > (12800000)                                               // {34} divider required above this frequency (required clock is between 150kHz and 200kHz - incorrect value > 200kHz could reduce programming reliability. incorrect value < 150kHz could result in FLASH damage)
    #define FLASH_DIV   ((BUS_CLOCK/200000/8/2) - 1) 
    #define FLASH_ROUND (((BUS_CLOCK + ((200000*8*2) - 1))/200000/8/2) - 1)
    #if FLASH_ROUND > FLASH_DIV
        #undef FLASH_DIV  
        #define FLASH_DIV FLASH_ROUND
    #endif
    CFMCLKD = (PRDIV8 | FLASH_DIV);                                      // set the FLASH state machine frequency as close to, but not higher than 200kHz
#else
    #define FLASH_DIV   ((BUS_CLOCK/200000/2) - 1) 
    #define FLASH_ROUND (((BUS_CLOCK + ((200000*2) - 1))/200000/2) - 1)
    #if FLASH_ROUND > FLASH_DIV
        #undef FLASH_DIV  
        #define FLASH_DIV FLASH_ROUND
    #endif
    CFMCLKD = FLASH_DIV;                                                 // set the FLASH state machine frequency as close to, but not higher than 200kHz
#endif

#if defined _WINDOWS
    CFMCLKD |= DIVLD;                                                    // set the loaded bit in the divide register
    CFMUSTAT |= CBEIF;                                                   // no command queued
#endif
}

#if defined _WINDOWS
    #define PROG_SIZE  100
#else
    #define PROG_SIZE  80                                                // optimised from experience
#endif

static void fnFlashRoutine(void)
{
#if defined _WINDOWS
    CFMUSTAT = (CBEIF | CCIF);
#else
    CFMUSTAT = CBEIF;                                                    // launch command
#endif
    while ((CFMUSTAT & CBEIF) == 0) {}                                   // wait until buffer free
    while ((CFMUSTAT & CCIF) == 0) {}                                    // wait until execution complete
}


static int fnFlashNow(unsigned char ucCommand, unsigned long *ptrWord, unsigned long ulWord)
{
    static unsigned long ulProgSpace[PROG_SIZE/sizeof(unsigned long)];   // SRAM space on long boundary
    static void (*fnRAM_code)(void) = 0;

    if (!fnRAM_code) {                                                   // the first time this is used it will load the program to SRAM
        uMemcpy(ulProgSpace, (unsigned char*)fnFlashRoutine, PROG_SIZE);
        fnRAM_code = (void(*)(void))ulProgSpace;
#if defined _WINDOWS
        fnRAM_code = fnFlashRoutine;                                     // {160} allow simulator to work with original code rather than RAM based code
#endif
    }

    uDisable_Interrupt();                                                // protect this region from interrupts

#if defined _WINDOWS                                                     // command sequence - set the word to the address (in the sector)
    if (CMD_MASS_ERASE == ucCommand) {                                   // {171}
        uMemset(fnGetFlashAdd(0), 0xff, SIZE_OF_FLASH);                  // delete the flash content
        fnResetBoard();                                                  // if the internal Flash is deleted the board will reset
    }
    else if (ucCommand != CMD_SECTOR_ERASE) {
        *ptrWord = ulWord;                                               // value not important for deletes
    }
#else
    ptrWord += (CFM_FLASH/sizeof(unsigned long));                        // position the write into CFM FLASH (a copy of real FLASH for commands)
    *ptrWord = ulWord;                                                   // value not important for deletes
#endif
    CFMCMD = ucCommand;                                                  // the command

    fnRAM_code();                                                        // execute from SRAM

    uEnable_Interrupt();                                                 // safe to accept interrupts again

    return (CFMUSTAT & (ACCERR | PVIOL));                                // if there was an error this will be non-zero
}
#endif                                                                   // #endif FLASH_FILE_SYSTEM or USE_PARAMETER_BLOCK

#if defined I2C_EEPROM_FILE_SYSTEM
    #define _I2C_EEPROM_INTERFACE                                         // insert manufacturer dependent code
    #include "i2C_eeprom_m24m01.h"
    #undef _I2C_EEPROM_INTERFACE
#endif

#if defined SPI_FLASH_ENABLED                                            // {9} SPI FLASH for SW upload {29}
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_m5223x_atmel.h"                              // {39} - check the chip(s) availablility
        #include "spi_flash_m5223x_stmicro.h"                            // {40}
        #include "spi_flash_m5223x_sst25.h"                              // {61}
    #undef _SPI_FLASH_INTERFACE

// Routine to request local SPI FLASH type
//
extern unsigned char fnSPI_Flash_available(void)
{
    return ucSPI_FLASH_Type[0];
}


#if defined SPI_FLASH_MULTIPLE_CHIPS                                     // {18}

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
    #if defined CS2_LINE                                                 // {38}
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;                                                        // address is in the second SPI device
    }
        #if defined CS3_LINE
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

    #if !defined STORAGE_REV_1                                           // {148}
    #if defined SPI_FLASH_SST25                                          // {71}
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
    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif
    return ucDestination;
}
    #else
// Return the page number and optionally the address offset in the page
//
    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // {18}
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset, int *iChipSelect)
    #else
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
    #endif
{
    unsigned short usPageNumber;
    #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)           // {29}
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
    #else
    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address
    #endif

    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // {18} determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
    #endif
    #endif
#elif defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM              // uses external file system in M95XXX EEPROM {16}{29}
    #if defined SPI_INTERFACE
    extern QUEUE_HANDLE SPI_handle;
    #endif

// EEPROM commands
//
#define M95XXX_WRITE_ENABLE   0x06                                       // send this prior to write or write status register command
#define M95XXX_WRITE_DISABLE  0x04
#define M95XXX_READ_STAT_REG  0x05
#define M95XXX_WRITE_STAT_REG 0x01
#define M95XXX_READ_MEMORY    0x03
#define M95XXX_WRITE_MEMORY   0x02

static unsigned char ucEEPROMDanger = 0;

static int fnWriteBytesEEPROM(unsigned char *ucDestination, unsigned char *ucData, unsigned short usLength) // {16} static function
{
    unsigned char usByteWrite[4];                                        // our file sytem is in external EEPROM via SPI bus.
    usByteWrite[0] = M95XXX_WRITE_MEMORY;
    usByteWrite[1] = (unsigned char)((unsigned short)(ucDestination)>> 8);
    usByteWrite[2] = (unsigned char)((unsigned short)ucDestination);
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
 #if defined _WINDOWS
    fnEEPROM_safe();
 #endif
}
#endif                                                                   // #endif SPI_FILE_SYSTEM

#if (defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES || defined FLASH_ROUTINES) && defined _WINDOWS // {83}
    extern int iFetchingInternalMemory = 0;                              // {30}
#endif


#if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES // {68}

#if defined STORAGE_REV_1                                                // {148}

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
            #if SPI_FLASH_PAGE_LENGTH == 1024                            // {183}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #elif SPI_FLASH_PAGE_LENGTH >= 512
            #if SPI_FLASH_PAGE_LENGTH == 512                            // {183}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #else
            #if SPI_FLASH_PAGE_LENGTH == 256                            // {183}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
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
        #if !defined SST25_A_VERSION
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

    #if defined MANAGED_FILES                                            // {172}
static int fnCheckSPI_FlashBusy(void)
{
    unsigned char ucStatus = 10;                                         // allow this many polling trials by the SPI flash interface
    fnSPI_command(CHECK_SPI_FLASH_BUSY, 0, _EXTENDED_CS &ucStatus, 1);   // check the busy state
    if (ucStatus == 0) {                                                 // device still busy so quit for the moment
        return MEDIA_BUSY;
    }
    return 0;                                                            // the SPI flash is presently not busy
}
    #endif
    #endif

    #if !defined ONLY_INTERNAL_FLASH_STORAGE

// Search for the memory type that the starting address is in, return the type and restrict the largest length that can be read,written, erased from that location
//
extern unsigned char fnGetStorageType(unsigned char *memory_pointer, ACCESS_DETAILS *ptrAccessDetails)
{
    unsigned long ulMaximumLength;
    STORAGE_AREA_ENTRY *ptrStorageList = UserStorageListPtr;
    #if defined _WINDOWS                                                 // this is only used when reading with the simulator
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
    #if defined MANAGED_FILES
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
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // set to sector boundary
        #if !defined _M520X && !defined _M523X
            fnConfigFlash();
            #if defined _WINDOWS
            CFMUSTAT |= CBEIF;                                           // no command queued
            #endif
            if ((CFMUSTAT & CBEIF) == 0) {
                return -1;                                               // the command buffers are full - we cannot proceed {152}
            }
            #if defined _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion of single sector
            #endif
            if ((fnFlashNow(CMD_SECTOR_ERASE, (unsigned long *)((unsigned long)fnGetFlashAdd(ptrSector) & ~0x3), (unsigned long)0)) != 0) { // ensure long word address
                return -1;                                               // error {152}
            }
        #endif
            AccessDetails.BlockLength = FLASH_GRANULARITY;
            break;
        #if defined SPI_FLASH_ENABLED
        case _STORAGE_SPI_FLASH:
            #if defined MANAGED_FILES                                    // {172}
            if ((OriginalLength == 0) && (fnCheckSPI_FlashBusy() != 0)) {// if single sector erase is called it is considered to be a managed delete and the state of the SPI Flash is first checked
                return MEDIA_BUSY;                                       // the media is presently busy so the write can not be started
            }
            #endif
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
                    return -1;                                           // {152}
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
                return -1;                                               // error {152}
            }
            break;
        #endif
        #if defined NVRAM
        case _STORAGE_NVRAM:                                             // {170} non-volatile memory that can be treated as RAM
            #if defined MANAGED_FILES
            if (AccessDetails.BlockLength == 0) {
                AccessDetails.BlockLength = FILE_GRANULARITY;            // NVRAM has no sector size so the file granularity is used as sector size
            }
            #endif
            uMemset(fnGetFlashAdd(ptrSector), 0xff, AccessDetails.BlockLength);
            break;
        #endif
        default:
            return -1;                                                   // invalid {152}
        }
        ptrSector += AccessDetails.BlockLength;
        if (Length <= AccessDetails.BlockLength) {                       // if completed
        #if defined MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size of the storage area
	            return AccessDetails.BlockLength;                        // return the page size that was deleted in the storage media
	        }
        #endif
            break;                                                       // {162}
        }
        Length -= AccessDetails.BlockLength;
    } while (1);
    #else                                                                // case when only internal Flash is available
    Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)));
    ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // set to sector boundary
    do {                                                                 // {150}
        #if !defined _M520X && !defined _M523X
        fnConfigFlash();
            #if defined _WINDOWS
        CFMUSTAT |= CBEIF;                                               // no command queued
            #endif
        if ((CFMUSTAT & CBEIF) == 0) {
            return -1;                                                   // the command buffers are full - we cannot proceed {152}
        }
            #if defined _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                   // simulate deletion of single sector
            #endif
        if ((fnFlashNow(CMD_SECTOR_ERASE, (unsigned long *)((unsigned long)fnGetFlashAdd(ptrSector) & ~0x3), (unsigned long)0)) != 0) { // ensure long word address
            return -1;                                                   // error {152}
        }
        #endif
        if (Length <= FLASH_GRANULARITY) {                               // check whether entire deletion has completed
            break;
        }
        ptrSector += FLASH_GRANULARITY;                                  // advance sector point to next internal flash sector
        Length -= FLASH_GRANULARITY;
    } while (1);
        #if defined MANAGED_FILES
    if (OriginalLength == 0) {                                           // if a single page erase was called, return the page size of the storage area
	    return FLASH_GRANULARITY;
	}
        #endif
    #endif
    return 0;                                                            // success when the length of sector was not requested
}
#else
// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length) // {22}
{
    #if defined PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
    #endif
    #if defined MANAGED_FILES                                            // {123}
    MAX_FILE_LENGTH OriginalLength = Length;
    #endif
    do {
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {29}
        static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
        unsigned short ucDeleted = 0;
        unsigned short usTemp = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector);

        usTemp &= ~(EEPROM_PAGE_SIZE - 1);                               // we want to erase a block (program them to 0xff)
        Length += ((unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector) - usTemp);
        ptrSector = (unsigned char *)usTemp;
        while (ucDeleted < FLASH_GRANULARITY) {                          // {3}
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
        #if defined I2C_EEPROM_FILE_SYSTEM                               // {143}
            #define I2C_EEPROM_END_ADDRESS   (I2C_EEPROM_START_ADDRESS + I2C_EEPROM_MEMORY_SIZE)
        if ((ptrSector >= (unsigned char *)I2C_EEPROM_START_ADDRESS) && (ptrSector < (unsigned char *)I2C_EEPROM_END_ADDRESS)) { // read starts in I2C EEPROM memory space
            return (fnDeleteI2C_eeprom(ptrSector, Length));              // delete data in I2C EEPROM
        }
        #endif
        #if defined SPI_FLASH_ENABLED                                    // {9}{29}
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)   // {29}
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + SIZE_OF_NVRAM))) // {77} we are working from external SPI FLASH memory
            #else
        if ((ptrSector >= (uFILE_SYSTEM_END)) && (ptrSector < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory 
            #endif
        {            
            #if defined SPI_FLASH_MULTIPLE_CHIPS                         // delete in SPI FLASH
            int iChipSelect;
            #endif
            #if !defined SPI_FLASH_ST                                    // {71}
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
            #if defined SPI_FLASH_ST
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // enable the write
                #if defined SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #elif defined SPI_FLASH_SST25                                // {71}
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
                #if !defined SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && (ulSectorAlign == (ulLocation & (SPI_FLASH_SECTOR_LENGTH - 1)))) { // {76} if a complete 64k sector can be deleted
                ucCommand = SECTOR_ERASE;                                // delete block of 64k
                BlockLength = SPI_FLASH_SECTOR_LENGTH;
            }
            else 
                #endif
            if ((Length >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) { // {76}
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
        #if defined NVRAM && defined _M5225X                             // {77} FlexBus
        if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) {
            uMemset(fnGetFlashAdd(ptrSector), 0xff, Length);
            break;
        }
        #endif
        #if !defined _M520X && !defined _M523X                           // {127}
        fnConfigFlash();
            #if defined _WINDOWS
        CFMUSTAT |= CBEIF;                                               // no command queued
            #endif
        if (!(CFMUSTAT & CBEIF)) {
            return 1;                                                    // the command buffers are full - we cannot proceed
        }

            #if defined _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                   // simulate deletion
            #endif

        if ((fnFlashNow(CMD_SECTOR_ERASE, (unsigned long *)((unsigned long)fnGetFlashAdd(ptrSector) & ~0x3), (unsigned long)0)) != 0) { // ensure long word address
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
    #if defined MANAGED_FILES                                            // {123}
    if (OriginalLength == 0) {                                           // if a single page erase was called, return the page size
	    return FLASH_GRANULARITY;
	}
    #endif
    return 0;
}
#endif

#endif                                                                   // #endif ACTIVE_FILE_SYSTEM or USE_PARAMETER_BLOCK

#if !defined NO_FLASH_SUPPORT && !defined _M520X && !defined _M523X      // {127}
// Mass erase for use when unsecuring a device
//
extern int fnMassEraseFlash(void)                                        // {101}
{
    fnConfigFlash();
  #if defined _WINDOWS
    CFMUSTAT |= CBEIF;                                                   // no command queued
  #endif
    if (!(CFMUSTAT & CBEIF)) return 1;                                   // the command buffers are full - we cannot proceed

    return (fnFlashNow(CMD_MASS_ERASE, 0, 0));
}
#endif

#if (defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES) // {68}
// The address must be on a word boundary or it will fail
//
static int fnWriteLongWordFlash(unsigned long *ptrAdd, unsigned long ulValue)
{
    #if defined _M520X || defined _M523X                                 // {127}
    return 0;
    #else
    #if defined STORAGE_REV_1 && defined _WINDOWS
    ptrAdd = (unsigned long *)fnPutFlashAdd((unsigned char *)ptrAdd);
    #endif
    if (*(unsigned long *)fnGetFlashAdd((unsigned char *)ptrAdd) == ulValue) { // {134} if the value is already in FLASH there is no need to program it
        return 0;
    }
    fnConfigFlash();                                                     // check that the FLASH controller has been configured before programming
        #if defined _WINDOWS                                             // only when simulating
    if (ulValue & ~(*(unsigned long *)fnGetFlashAdd((unsigned char *)ptrAdd))) { // we check that we are not trying to program a bit from '0' to '1'
        *(unsigned char*)0 = 0;                                          // this will fail so signal it when debugging
    }
    CFMUSTAT |= CBEIF;                                                   // no command queued
        #endif
    if (!(CFMUSTAT & CBEIF)) {                                           // check that there is no command presently being executed - this should never be the case
        return 1;                                                        // the command buffers are full - we cannot program yet
    }
    return (fnFlashNow(CMD_PROGRAMM_WORD, (unsigned long *)fnGetFlashAdd((unsigned char *)ptrAdd), ulValue)); // program the long word value and return the result of the operation (0 when no errors)
    #endif
}
#endif                                                                   // #endif FLASH_FILE_SYSTEM

#if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined FLASH_ROUTINES // {68}
#if defined STORAGE_REV_1                                                // {148}

static int fnWriteInternalFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ucData)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned long *ptrLong = (unsigned long *)((CAST_POINTER_ARITHMETIC)ptrAccessDetails->ulOffset & ~0x3); // long address starting on
    int iByteInLong = (ptrAccessDetails->ulOffset - (CAST_POINTER_ARITHMETIC)ptrLong); // long word align
    #if !defined _WINDOWS
    unsigned long ulValue = 0xffffffff;                                  // deleted state
    #else
    unsigned long ulValue = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrLong); // present value at location
    #endif

    while (Length--) {
    #if defined _LITTLE_ENDIAN                                           // used only by simulator
        ulValue &= ~(0xff << (iByteInLong * 8));                         // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong * 8));                     // insert new byte
    #else
        ulValue &= ~(0xff << ((3 - iByteInLong) * 8));                   // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong) * 8));               // insert new byte
    #endif
        if (iByteInLong == 3) {                                          // can we write a long ?
            if (fnWriteLongWordFlash((unsigned long *)fnGetFlashAdd((unsigned char *)ptrLong), ulValue) != 0) {
                return 1;
            }
            if (0 == Length) {
                return 0;                                                // completed
            }
            ++ptrLong;
    #if !defined _WINDOWS
            ulValue = 0xffffffff;                                        // deleted state
    #else
            ulValue = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrLong); // load the next existing value                                      
    #endif
            iByteInLong = 0;                                             // long word aligned
        }
        else {
            iByteInLong++;                                               // advance byte in the long word
        }
    }
    return (fnWriteLongWordFlash((unsigned long *)fnGetFlashAdd((unsigned char *)ptrLong), ulValue)); // program final long word
}

#if defined MANAGED_FILES && defined MANAGED_FILE_WRITE                  // {172}
extern int fnWriteBytesFlashNonBlocking(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
#else
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
#endif
{
    ACCESS_DETAILS AccessDetails;
    #if defined MANAGED_FILES && defined MANAGED_FILE_WRITE              // {172}
    int iBusyCheckDone = 0;
    #endif
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
            #if defined MANAGED_FILES && defined MANAGED_FILE_WRITE      // {172}
            if ((iBusyCheckDone == 0) && (fnCheckSPI_FlashBusy() != 0)) {// when managed mode is enabled there is always first a check on the state ofthe SPI flash - the caller should be prepared that a busy state could be returned
                return MEDIA_BUSY;                                       // the media is presently busy so the write can not be started
            }
            #endif
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
        #if defined NVRAM
        case _STORAGE_NVRAM:                                             // {170} non-volatile memory that can be treated as RAM
            uMemcpy(fnGetFlashAdd(ucDestination), ucData, AccessDetails.BlockLength);
            break;
        #endif
        default:
            _EXCEPTION("Invalid storage type");
            return 1;                                                    // invalid
        }
        if (Length == AccessDetails.BlockLength) {                       // if write is complete
            return 0;
        }
        Length -= AccessDetails.BlockLength;                             // decrease the amount of data remaining
        ucDestination += AccessDetails.BlockLength;                      // increase the destination address accordingly
        ucData += AccessDetails.BlockLength;                             // increase the buffer address accordingly
        AccessDetails.BlockLength = Length;                              // synchronise individual accesses to remaining length
    }
    return 0;
    #else
    AccessDetails.ulOffset = (unsigned long)ucDestination;
    return (fnWriteInternalFlash(&AccessDetails, ucData));
    #endif
}

    #if defined MANAGED_FILES && defined MANAGED_FILE_WRITE                // {172}
// When managed files are in operation the FLASH driver will return MEDIA_BUSY if writes can not yet be performed.
// This module waits for the write to complete by recalling if necessary
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    int iRtn;
    do {
        iRtn = fnWriteBytesFlashNonBlocking(ucDestination, ucData, Length);
    } while (iRtn == MEDIA_BUSY);
    return iRtn;
}
    #endif
#else
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {29}
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
    #else
    // we have already checked that it is possible to modify the byte requested but we have to be sure that we only write 32 bits to long word addresses
        #if defined _WINDOWS
    unsigned char *ptr;
        #endif
    unsigned long *ptrLong;
    unsigned long ulValue;
    int iByteInLong;

        #if defined I2C_EEPROM_FILE_SYSTEM                               // {143}
    if ((ucDestination >= (unsigned char *)I2C_EEPROM_START_ADDRESS) && (ucDestination < (unsigned char *)I2C_EEPROM_END_ADDRESS)) { // read starts in I2C EEPROM memory space
        return (fnWriteI2C_eeprom(ucDestination, ucData, Length));
    }
        #endif

        #if defined SPI_FLASH_ENABLED                                    // {9}{29}
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)   // {29}
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + SIZE_OF_NVRAM))) // {77} we are working from external SPI FLASH memory 
            #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))  // we are working from external SPI FLASH memory
            #endif
    {
            #if defined SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
            #endif
            #if defined SPI_FLASH_SST25                                  // {71}
        int iMultipleWrites = 0;
            #endif 
        unsigned short usDataLength;
            #if defined SPI_FLASH_SST25                                  // {71}
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
                #if defined SPI_FLASH_MULTIPLE_CHIPS                     // {147}
            if (usPageNumber >= SPI_FLASH_PAGES) {                       // on reaching the end of an SPI Flash chip, move to next
                usPageNumber = 0;
                iChipSelect++;
            }
                #endif
            usPageOffset = 0;
            #endif
        }
            #if defined SPI_FLASH_SST25                                  // {71}
        fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);              // disable writes
            #endif
        return 0;
    }
        #endif
        #if defined NVRAM && defined _M5225X                             // {77} FlexBus
    if (ucDestination >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) {
        uMemcpy(fnGetFlashAdd(ucDestination), ucData, Length);
        return 0;
    }
        #endif
        #if defined _WINDOWS
    if (ucDestination > (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // monitor incorrect writing outside fo FLASH memory range {67}
        _EXCEPTION("Attempt to write to a value outside of the flash memory range!!");
    }
    ptrLong = (unsigned long *)((unsigned long)((CAST_POINTER_ARITHMETIC)ucDestination + (CAST_POINTER_ARITHMETIC)ucFLASH - FLASH_START_ADDRESS) & ~0x3);  // long address starting on
    iByteInLong = (ucDestination - FLASH_START_ADDRESS + (CAST_POINTER_ARITHMETIC)ucFLASH) - (unsigned char *)ptrLong;
        #else
    ptrLong = (unsigned long *)((CAST_POINTER_ARITHMETIC)ucDestination & ~0x3); // long address starting on
    iByteInLong = ucDestination - (unsigned char *)ptrLong;              // long word align
        #endif
        #if !defined _WINDOWS
    ulValue = 0xffffffff;                                                // deleted state
        #else
    ulValue = *ptrLong;                                                  // present value at location
        #endif

    while (Length--) {
        #if defined I2C_EEPROM_FILE_SYSTEM                               // {146}
        if ((ucDestination >= (unsigned char *)I2C_EEPROM_START_ADDRESS) && (ucDestination < (unsigned char *)I2C_EEPROM_END_ADDRESS)) { // read starts in I2C EEPROM memory space
            return (fnWriteI2C_eeprom(ucDestination, ucData, (Length + 1)));
        }
        ucDestination++;
        #endif

        #if defined _LITTLE_ENDIAN                                       // used only by simulator
        ulValue &= ~(0xff << (iByteInLong*8));                           // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong*8));                       // insert new byte
        #else
        ulValue &= ~(0xff << ((3 - iByteInLong) * 8));                   // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong) * 8));               // insert new byte
        #endif
        if (iByteInLong == 3) {                                          // can we write a long ?
        #if defined _WINDOWS
            ptr = (unsigned char *)ptrLong;
            ptr -= (unsigned long)ucFLASH;
            ptr += FLASH_START_ADDRESS;
            if (ptr > (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // monitor incorrect writing outside fo FLASH memory range {67}
                _EXCEPTION("Attempt to write to a value outside of the flash memory range!!");
            }
            if (fnWriteLongWordFlash((unsigned long *)ptr, ulValue) != 0) {
                return 1;
            }
        #else
            if (fnWriteLongWordFlash(ptrLong, ulValue) != 0) {
                return 1;
            }
        #endif
            if (!Length) {
                return 0;                                                // completed
            }
            ++ptrLong;
        #if !defined _WINDOWS
            ulValue = 0xffffffff;                                        // deleted state
        #else
            ulValue = *ptrLong;                                          // load the next existing value
        #endif
            iByteInLong = 0;                                             // long word aligned
        }
        else {
            iByteInLong++;                                               // advance byte in the long word
        }
    }
        #if defined _WINDOWS
    ptr = (unsigned char *)ptrLong;
    ptr -= (CAST_POINTER_ARITHMETIC)ucFLASH;
    ptr += FLASH_START_ADDRESS;
    if (ptr > (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) {  // monitor incorrect writing outside fo FLASH memory range {67}
        _EXCEPTION("Attempt to write to a value outside of the flash memory range!!");
    }
    return (fnWriteLongWordFlash((unsigned long *)ptr, ulValue));        // program final long word
        #else
    return (fnWriteLongWordFlash(ptrLong, ulValue));                     // program final long word
        #endif
    #endif
}
#endif

#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // SPI reserved exclusively for a EEPROM file system {29}
#define CS0_LINE  PORT_QS_BIT3
void fnSendSPIMessage(unsigned char *ptrData, QUEUE_TRANSFER Length)     // master transmission
{
    int iDelete = (ptrData == 0);
    unsigned short usMode = (PORTQS & CS0_LINE);                         // check the state of the SS line on entry

    if (usMode) {                                                        // if this is the first entry it has a control byte at the end
        Length--;                                                        // reduce the count so we don't send the control byte
    }

    PORTQS &= ~CS0_LINE;                                                 // assert SS low before starting

    while (Length--) {                                                   // for each byte we want to send
      //QAR = QSPI_COMMAND_RAM_0;                                        // set address to first control location {60}
      //QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); // no chip select since we control it via port - 8 bit access {60}
        QAR = QSPI_TRANSMIT_RAM_0;                                       // set address to first transmit location
        if (iDelete) {
            QDR = 0xff;
        }
        else {
            QDR = *ptrData++;                                            // prepare data
        }
        QWR = ((0 << QSPI_START_SHIFT) | (0 << QSPI_END_SHIFT));         // starting at queue 0 send/receive one entry
        QDLYR = QSPI_SPE;                                                // start transfer - byte will be sent and 8 bits read in
#if defined _WINDOWS
        fnSimM95xxx(M95XXX_WRITE, (unsigned char)QDR, (PORTQS & CS0_LINE)); // simulate the SPI EEPROM device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
#endif
        while (!(QIR & QSPI_SPIF)) {};                                   // wait for transfer to complete
        QIR = QSPI_SPIF;                                                 // clear interrupt flag
    }

    if ((!usMode) || (PREPARE_READ != *ptrData)) {
        PORTQS |= CS0_LINE;                                              // deassert SS when complete
    }
#if defined _WINDOWS
    fnSimM95xxx(M95XXX_CHECK_SS, 0, (PORTQS & CS0_LINE));                // simulate the SPI EEPROM device
#endif
}


// We use this in master mode to get the next SPI byte as fast as possible
//
void fnGetSPI_bytes(unsigned char *ptrBuffer, MAX_FILE_LENGTH Length)
{   // master reception - we assume SSEL asserted before entry (commanded previously)

    while (Length--) {
      //QAR = QSPI_COMMAND_RAM_0;                                        // set address to first control location {60}
      //QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); // no chip select since we control it via port - 8 bit access {60}
        QAR = QSPI_TRANSMIT_RAM_0;                                       // set address to first transmit location
        QDR = 0xff;                                                      // prepare dummy data
        QWR = ((0<<QSPI_START_SHIFT) | (0<<QSPI_END_SHIFT));             // starting at queue 0 send/receive one entry
        QDLYR = QSPI_SPE;                                                // start transfer - byte will be sent and 8 bits read in
#if defined _WINDOWS
        QDR = fnSimM95xxx(M95XXX_READ, (unsigned char)QDR, (PORTQS & CS0_LINE)); // simulate the SPI EEPROM device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
#endif
        while (!(QIR & QSPI_SPIF)) {};                                   // wait for transfer to complete
        QIR = QSPI_SPIF;                                                 // clear interrupt flag

        QAR = QSPI_RECEIVE_RAM_0;                                        // set address to first receive location
        *ptrBuffer++ = (unsigned char)QDR;                               // read the received byte
    }

    PORTQS |= CS0_LINE;                                                  // deassert SS when complete
  #if defined _WINDOWS
    fnSimM95xxx(M95XXX_CHECK_SS, 0, (PORTQS & CS0_LINE));                // simulate the SPI EEPROM device
  #endif
}
#endif                                                                   // #endif SPI_FILE_SYSTEM
#endif                                                                   // #endif ACTIVE_FILE_SYSTEM




/************************************************************************************************************/

// QSPI Interrupt
//
static void (*qspi_handler)(void) = 0;

static __interrupt__ void _QSPI_Interrupt(void)
{
    QIR = (QSPI_SPIFE | QSPI_SPIF);                                      // clear interrupt flag and ensure QSPI remains enabled
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during callback
    if (qspi_handler) (qspi_handler)();                                  // call the handler, if available
    iInterruptLevel = 0;
}

#if defined SPI_INTERFACE
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {29}
void fnConfigSPI(SPITABLE *pars)
{
}
    #else
// We support only master mode here
//
void fnConfigSPI(SPITABLE *pars)
{
}
    #endif



extern SPIQue *spi_rx_control;

static __interrupt__ void SPI_Interrupt(void)
{
}

extern int fnTxSPIByte(QUEUE_HANDLE channel, unsigned short usTxByte, int iLast)
{
    return 0;
}

#else                                                                    // not general SPI interface
    #if defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD                 // only a file system on SPI
static void fnConfigSPIFileSystem(unsigned char ucSpeed)
{
    unsigned short usQMR_Value;

        #if defined SPI_FLASH_MULTIPLE_CHIPS                             // {18}
            #if defined CS3_LINE
    PORTQS |= (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE);                                          
    DDRQS  |= (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE);               // drive CS lines high
            #elif defined CS2_LINE
    PORTQS |= (CS0_LINE | CS1_LINE | CS2_LINE);                                          
    DDRQS  |= (CS0_LINE | CS1_LINE | CS2_LINE);                          // drive CS lines high
            #else
    PORTQS |= (CS0_LINE | CS1_LINE);                                          
    DDRQS  |= (CS0_LINE | CS1_LINE);                                     // drive CS lines high
            #endif
        #else
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, CS0_LINE, CS0_LINE);             // drive CS line high                                    
        #endif
    PQSPAR |= (QS_QSPI_SCK | QS_QSPI_DIN | QS_QSPI_DOUT);                // set QSPI_SCK, QSPI_DIN and QSPI_DOUT on QS port

    switch (ucSpeed) {
    case MAX_SPI:
	    usQMR_Value = 2;                                                 // set maximum possible SPI speed (16MHz with 60MHz clock)
	    break;
/*
    case SPI_8MEG:
	    usQMR_Value = ((BUS_CLOCK/2/8000000) & QSPI_BAUD_MASK);
	    break;

    case SPI_4MEG:
	    usQMR_Value = ((BUS_CLOCK/2/4000000) & QSPI_BAUD_MASK);
	    break;*/

    case SPI_2MEG:
	    usQMR_Value = ((BUS_CLOCK/2/2000000) & QSPI_BAUD_MASK);
	    break;
/*
    case SPI_1MEG:
	    usQMR_Value = ((BUS_CLOCK/2/1000000) & QSPI_BAUD_MASK);
	    break;*/
    }
        #if defined SPI_FLASH_ENABLED                                    // {29}
    QMR = ((QSPI_MSTR | QSPI_8BITS) | usQMR_Value);                      // set mode and speed {9}
        #else
    QMR = ((QSPI_MSTR | QSPI_8BITS | QSPI_CPOL | QSPI_CPHA) | usQMR_Value);
        #endif

    usQMR_Value = 16;                                                    // the depth of FIFO 
    QAR = QSPI_COMMAND_RAM_0;                                            // set address to first control location {60}
    while (usQMR_Value--) {                                              // initialise all control registers
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0);  // no chip selects since we control via port - 8 bit access
    }
}
    #endif
#endif

#if defined I2C_INTERFACE

extern I2CQue *I2C_rx_control[NUMBER_I2C];                               // {57}
extern I2CQue *I2C_tx_control[NUMBER_I2C];

static __interrupt__ void _I2C_Interrupt(void)                           // I2C0 interrupt
{
    I2SR = 0;                                                            // clear the interrupt flag
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutines
    if (I2C_tx_control[0]->ucState & RX_ACTIVE) {
        unsigned char ucFirstRead = (I2CR & I2C_MTX); 
        if (I2C_tx_control[0]->ucPresentLen == 1) {
            I2CR = (I2C_IEN | I2C_IIEN | I2C_MSTA | I2C_TXAK);           // we don't acknowledge last byte
        }
        else if (I2C_tx_control[0]->ucPresentLen == 0) {                 // we have completed the read
            I2CR = (I2C_IEN | I2C_TXAK);                                 // send end condition and disable interrupts
            I2C_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            I2C_rx_control[0]->msgs++;
            if (I2C_rx_control[0]->wake_task) {                          // wake up the receiver task if desired
               uTaskerStateChange(I2C_rx_control[0]->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        else {
            I2CR = (I2C_IEN | I2C_IIEN | I2C_MSTA);                      // ensure we acknowledge multibyte reads
        }

        if (ucFirstRead) {                                               // have we just sent the slave address?
            volatile unsigned char ucRx;
            ucRx = I2DR;                                                 // dummy read
        }
        else {
            *I2C_rx_control[0]->I2C_queue.put++ = I2DR;                  // read the byte
            I2C_rx_control[0]->I2C_queue.chars++;                        // and put it into the rx buffer
            if (I2C_rx_control[0]->I2C_queue.put >= I2C_rx_control[0]->I2C_queue.buffer_end) {
                I2C_rx_control[0]->I2C_queue.put = I2C_rx_control[0]->I2C_queue.QUEbuffer;
            }
        }

        if (I2C_tx_control[0]->ucPresentLen) {
          I2C_tx_control[0]->ucPresentLen--;
 #if defined _WINDOWS
          I2DR = fnSimI2C_devices(I2C_RX_DATA, I2DR);                    // simulate the interrupt directly
          I2SR |= I2C_IIF;
          iInts |= I2C_INT0;
 #endif
        }
        else {                                                           // read sequence complete. Continue with next write if something is waiting. {13}
            if (I2C_tx_control[0]->I2C_queue.chars != 0) {
                fnTxI2C(I2C_tx_control[0], 0);                           // we have another message to send so we can send a repeated start condition
            }
        }
        iInterruptLevel = 0;                                             // {16}
        return;
    }
    else if (I2C_tx_control[0]->ucPresentLen--) {                        // TX_ACTIVE - send next byte to send if available  
        I2DR = *I2C_tx_control[0]->I2C_queue.get++;
        if (I2C_tx_control[0]->I2C_queue.get >= I2C_tx_control[0]->I2C_queue.buffer_end) {
            I2C_tx_control[0]->I2C_queue.get = I2C_tx_control[0]->I2C_queue.QUEbuffer; // handle the ring buffer
        }
 #if defined _WINDOWS
        I2SR |= I2C_IIF;                                                 // simulate the interrupt directly
        fnSimI2C_devices(I2C_TX_DATA, I2DR);
        iInts |= I2C_INT0;
 #endif
    }
    else {                                                               // last byte in TX_ACTIVE
        if (I2C_tx_control[0]->I2C_queue.chars == 0) {                   // transmission complete
            I2CR = (I2C_IEN | I2C_MTX);                                  // send stop condition and disable interrupts
            I2C_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            if (I2C_tx_control[0]->wake_task ) {
               uTaskerStateChange(I2C_tx_control[0]->wake_task, UTASKER_ACTIVATE);// wake up owner task since the transmission has terminated
            }
        }
        else {        
            fnTxI2C(I2C_tx_control[0], 0);                               // we have another message to send so we can send a repeated start condition
        }
    }
    iInterruptLevel = 0;
}

#if NUMBER_I2C > 1                                                       // {57}
static __interrupt__ void _I2C_Interrupt_1(void)                         // I2C1 interrupt
{
    I2SR_1 = 0;                                                          // clear the interrupt flag
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked during subroutines
    if (I2C_tx_control[1]->ucState & RX_ACTIVE) {
        unsigned char ucFirstRead = (I2CR_1 & I2C_MTX);
        if (I2C_tx_control[1]->ucPresentLen == 1) {
            I2CR_1 = (I2C_IEN | I2C_IIEN | I2C_MSTA | I2C_TXAK);         // we don't acknowledge last byte
        }
        else if (!I2C_tx_control[1]->ucPresentLen) {                     // we have completed the read
            I2CR_1 = (I2C_IEN | I2C_TXAK);                               // send end condition and enable interrupts                                          // 
            I2C_tx_control[1]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            I2C_rx_control[1]->msgs++;
            if (I2C_rx_control[1]->wake_task ) {                         // wake up the receiver task if desired
               uTaskerStateChange(I2C_rx_control[1]->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        else {
            I2CR_1 = (I2C_IEN | I2C_IIEN | I2C_MSTA);                    // ensure we acknowledge multibyte reads
        }

        if (ucFirstRead) {                                               // have we just sent the slave address?
            volatile unsigned char ucRx;
            ucRx = I2DR_1;                                               // dummy read
        }
        else {
            *I2C_rx_control[1]->I2C_queue.put++ = I2DR_1;                // read the byte
            I2C_rx_control[1]->I2C_queue.chars++;                        // and put it into the rx buffer
            if (I2C_rx_control[1]->I2C_queue.put >= I2C_rx_control[1]->I2C_queue.buffer_end) {
                I2C_rx_control[1]->I2C_queue.put = I2C_rx_control[1]->I2C_queue.QUEbuffer;
            }
        }

        if (I2C_tx_control[1]->ucPresentLen) {
          I2C_tx_control[1]->ucPresentLen--;
     #if defined _WINDOWS
          I2DR_1 = fnSimI2C_devices(I2C_RX_DATA, I2DR_1);                // simulate the interrupt directly
          I2SR_1 |= I2C_IIF;
          iInts |= I2C_INT1;
     #endif
        }
        else {                                                           // read sequence complete. Continue with next write if something is waiting. {13}
            if (I2C_tx_control[1]->I2C_queue.chars != 0) {
                fnTxI2C(I2C_tx_control[1], 1);                           // we have another message to send so we can send a repeated start condition
            }
        }
        iInterruptLevel = 0;                                             // {16}
        return;
    }
    else if (I2C_tx_control[1]->ucPresentLen--) {                        // TX_ACTIVE - send next byte to send if available
        I2DR_1 = *I2C_tx_control[1]->I2C_queue.get++;
        if (I2C_tx_control[1]->I2C_queue.get >= I2C_tx_control[1]->I2C_queue.buffer_end) {
            I2C_tx_control[1]->I2C_queue.get = I2C_tx_control[1]->I2C_queue.QUEbuffer; // handle the ring buffer
        }
    #if defined _WINDOWS
        I2SR_1 |= I2C_IIF;                                               // simulate the interrupt directly
        fnSimI2C_devices(I2C_TX_DATA, I2DR_1);
        iInts |= I2C_INT1;
    #endif
    }
    else {                                                               // last byte in TX_ACTIVE
        if (!(I2C_tx_control[1]->I2C_queue.chars)) {                     // transmission complete
            I2CR_1 = (I2C_IEN | I2C_MTX);                                // send stop condition and disable interrupts

            I2C_tx_control[1]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);

            if (I2C_tx_control[1]->wake_task ) {
               uTaskerStateChange(I2C_tx_control[1]->wake_task, UTASKER_ACTIVATE);// wake up owner task since the transmission has terminated
            }
        }
        else {
            fnTxI2C(I2C_tx_control[1], 1);                               // we have another message to send so we can send a repeated start condition
        }
    }
    iInterruptLevel = 0;
}
#endif

// Initially the I2C pins were configured as inputs to allow an I2C bus lockup state to be detected
// - this routine checks for this state and generates clocks if needed to recover from it before setting I2C pin functions
//
static void fnConfigI2C_pins(QUEUE_HANDLE Channel)                       // {182}
{
    if (Channel == 0)  {
        #if defined I2C0_ON_QS && defined _M52259
        while (_READ_PORT_MASK(QS, PORT_QS_BIT3) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, PORT_QS_BIT2, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(QS, PORT_QS_BIT2);
            fnDelayLoop(10);
        }
        PQSPAR |= (QS_I2C_SCL_0_FUNCTION | QS_I2C_SDA_0_FUNCTION);       // configure the SDA and SCL pins on QS
        #else
        while (_READ_PORT_MASK(AS, PORT_AS_BIT1) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(AS, PORT_AS_BIT0, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(AS, PORT_AS_BIT0);
            fnDelayLoop(10);
        }
        PASPAR |= (AS_I2C_SCL_0_FUNCTION | AS_I2C_SDA_0_FUNCTION);       // configure the SDA and SCL pins on AS
        #endif
    }
    #if CHIP_HAS_I2C > 1
    else {
        #if defined _M5225X && !defined I2C1_ON_UB && !defined I2C1_ON_UC && !defined I2C1_ON_TH
            #define I2C1_ON_QS                                           // {96}
        #endif
        #if defined _M5225X && defined I2C1_ON_QS                        // {96}
        while (_READ_PORT_MASK(QS, PORT_QS_BIT1) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, PORT_QS_BIT0, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(QS, PORT_QS_BIT0);
            fnDelayLoop(10);
        }
        PQSPAR |= (QS_I2C_SCL_1_FUNCTION | QS_I2C_SDA_1_FUNCTION);
        #elif defined _M5225X && defined I2C1_ON_UB
        while (_READ_PORT_MASK(UB, PORT_UB_BIT1) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(UB, PORT_UB_BIT0, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(UB, PORT_UB_BIT0);
            fnDelayLoop(10);
        }
        PUBPAR |= (UB_I2C_SCL_1_FUNCTION | UB_I2C_SDA_1_FUNCTION);
        #elif defined _M5225X && defined I2C1_ON_UC
        while (_READ_PORT_MASK(UC, PORT_UC_BIT2) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(UC, PORT_UC_BIT3, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(UC, PORT_UC_BIT3);
            fnDelayLoop(10);
        }
        _CONFIG_PORT_INPUT(UC, (PORT_UC_BIT3 | PORT_UC_BIT2));           // {182}
        #elif defined _M5225X && defined I2C1_ON_TH
        while (_READ_PORT_MASK(TH, PORT_TH_BIT2) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TH, PORT_TH_BIT3, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(TH, PORT_TH_BIT3);
            fnDelayLoop(10);
        }
        PTHPAR |= (TH_I2C_SCL_1_FUNCTION | TH_I2C_SDA_1_FUNCTION);
        #else
            #if defined #if defined _M5225X
        while (_READ_PORT_MASK(QS, PORT_QS_BIT1) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, PORT_QS_BIT0, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(QS, PORT_QS_BIT0);
            fnDelayLoop(10);
        }
            #elif defined _M5221X || defined _M521XX
        while (_READ_PORT_MASK(QS, PORT_QS_BIT0) == 0) {                 // if the SDA line is low we clock the SCL line to free it
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, PORT_QS_BIT4, 0);        // set output '0'
            fnDelayLoop(10);
            _CONFIG_PORT_INPUT(QS, PORT_QS_BIT4);
            fnDelayLoop(10);
        }
        PQSPAR |= (QS_I2C_SCL_1_FUNCTION | QS_I2C_SDA_1_FUNCTION);       // configure port use
            #endif
        #endif
    }
    #endif
}

// Configure the I2C hardware
//
extern void fnConfigI2C(I2CTABLE *pars)
{
    unsigned char ucSpeed;
    #if NUMBER_I2C > 1                                                   // {57}
    if (pars->Channel == 0) {
        POWER_UP(POWER_I2C);                                             // enable clock to module
        #if defined I2C0_ON_QS && defined _M52259
      //PQSPAR |= (QS_I2C_SCL_0_FUNCTION | QS_I2C_SDA_0_FUNCTION);       // configure the SDA and SCL pins on QS
        _CONFIG_PORT_INPUT(QS, (PORT_QS_BIT2 | PORT_QS_BIT3));           // {182}
        #else
      //PASPAR |= (AS_I2C_SCL_0_FUNCTION | AS_I2C_SDA_0_FUNCTION);       // configure the SDA and SCL pins on AS
        _CONFIG_PORT_INPUT(AS, (PORT_AS_BIT0 | PORT_AS_BIT1));           // {182}
        #endif
    }
    else {                                                               // I2C channel 1
        POWER_UP(POWER_I2C1);                                            // enable clock to module
        #if defined _M5225X && defined I2C1_ON_QS                        // {96}
      //PQSPAR |= (QS_I2C_SCL_1_FUNCTION | QS_I2C_SDA_1_FUNCTION);
        _CONFIG_PORT_INPUT(QS, (PORT_QS_BIT0 | PORT_QS_BIT1));           // {182}
        #elif defined _M5225X && defined I2C1_ON_UB
      //PUBPAR |= (UB_I2C_SCL_1_FUNCTION | UB_I2C_SDA_1_FUNCTION);
        _CONFIG_PORT_INPUT(UB, (PORT_UB_BIT0 | PORT_UB_BIT1));           // {182}
        #elif defined _M5225X && defined I2C1_ON_UC
      //PUCPAR |= (UC_I2C_SCL_1_FUNCTION | UC_I2C_SDA_1_FUNCTION);
        _CONFIG_PORT_INPUT(UC, (PORT_UC_BIT3 | PORT_UC_BIT2));           // {182}
        #elif defined _M5225X && defined I2C1_ON_TH
      //PTHPAR |= (TH_I2C_SCL_1_FUNCTION | TH_I2C_SDA_1_FUNCTION);
        _CONFIG_PORT_INPUT(TH, (PORT_TH_BIT3 | PORT_TH_BIT2));           // {182}
        #else
      //PQSPAR |= (QS_I2C_SCL_1_FUNCTION | QS_I2C_SDA_1_FUNCTION);       // configure port use
            #if defined #if defined _M5225X
        _CONFIG_PORT_INPUT(QS, (PORT_QS_BIT0 | PORT_QS_BIT1));           // {182}
            #elif defined _M5221X || defined _M521XX
        _CONFIG_PORT_INPUT(QS, (PORT_QS_BIT4 | PORT_QS_BIT0));           // {182}
            #endif
        #endif
    }
    #else
    POWER_UP(POWER_I2C);                                                 // enable clock to module {46} 
        #if defined I2C_ON_QS                                            // {31}
  //PQSPAR |= (QS_I2C_SCL_0_FUNCTION | QS_I2C_SDA_0_FUNCTION);           // configure the SDA and SCL pins on QS
    _CONFIG_PORT_INPUT(QS, (PORT_QS_BIT2 | PORT_QS_BIT3));               // {182}
        #else
  //PASPAR |= (AS_I2C_SCL_0_FUNCTION | AS_I2C_SDA_0_FUNCTION);           // configure the SDA and SCL pins on AS
    _CONFIG_PORT_INPUT(AS, (PORT_AS_BIT0 | PORT_AS_BIT1));               // {182}
        #endif
    #endif

    // The calculation of the correct divider ratio doesn't follow a formular but is best taken from a table.
    // The required divider value is ((BUS_CLOCK/1000)/pars->usSpeed). Various typical speeds are supported here.
    switch (pars->usSpeed) {                                             // {31}
    case 400:                                                            // high speed I2C
    #if BUS_CLOCK > 60000000                                             // 80MHz
        ucSpeed = 0x32;                                                  // set about 400k with 80MHz bus frequency
    #elif BUS_CLOCK > 50000000                                           // 60MHz
        ucSpeed = 0x0d;                                                  // set about 400k with 60MHz bus frequency
    #elif BUS_CLOCK > 40000000                                           // 50MHz
        ucSpeed = 0x0b;                                                  // set about 400k with 50MHz bus frequency
    #else                                                                // assume 40MHz
        ucSpeed = 0x0a;                                                  // set about 400k with 40MHz bus frequency
    #endif
        break;
    case 100:
    default:                                                             // default to 100kHz
    #if BUS_CLOCK > 60000000                                             // 80MHz
        ucSpeed = 0x3a;                                                  // set about 100k with 80MHz bus frequency
    #elif BUS_CLOCK > 50000000                                           // 60MHz
        ucSpeed = 0x15;                                                  // set about 100k with 60MHz bus frequency
    #elif BUS_CLOCK > 40000000                                           // 50MHz
        ucSpeed = 0x37;                                                  // set about 100k with 50MHz bus frequency
    #else                                                                // assume 40MHz
        ucSpeed = 0x36;                                                  // set about 100k with 40MHz bus frequency
    #endif
        break;

    case 50:
    #if BUS_CLOCK > 60000000                                             // 80MHz
        ucSpeed = 0x3e;                                                  // set about 50k with 80MHz bus frequency
    #elif BUS_CLOCK > 50000000                                           // 60MHz
        ucSpeed = 0x19;                                                  // set about 50k with 60MHz bus frequency
    #elif BUS_CLOCK > 40000000                                           // 50MHz
        ucSpeed = 0x3b;                                                  // set about 50k with 50MHz bus frequency
    #else                                                                // assume 40MHz
        ucSpeed = 0x3a;                                                  // set about 50k with 40MHz bus frequency
    #endif
        break;
    }
    #if NUMBER_I2C > 1                                                   // {57}
    if (pars->Channel == 0) {
        I2FDR = ucSpeed;
        I2CR = (I2C_IEN);                                                // enable I2C controller
        IC_ICR_0_17 = I2C_INTERRUPT_PRIORITY;                            // define interrupts level and priority
        fnSetIntHandler(I2C_VECTOR, (unsigned char *)_I2C_Interrupt);    // enter the handler routine
        IC_IMRL_0 &= ~(I2C_PIF_INT_L | MASK_ALL_INT);
    }
    else {
        I2FDR_1 = ucSpeed;
        I2CR_1 = (I2C_IEN);                                              // enable I2C controller
        IC_ICR_0_62 = I2C_1_INTERRUPT_PRIORITY;                          // define interrupts level and priority
        fnSetIntHandler(I2C_1_VECTOR, (unsigned char *)_I2C_Interrupt_1);// enter the handler routine
        IC_IMRH_0 &= ~(I2C_1_INT_H);                                     // unmask interrupt source
        IC_IMRL_0 &= ~(MASK_ALL_INT); 
    }
    #else
    I2FDR = ucSpeed;
    I2CR = (I2C_IEN);                                                    // enable I2C controller
    IC_ICR_0_17 = I2C_INTERRUPT_PRIORITY;                                // define interrupts level and priority
    fnSetIntHandler(I2C_VECTOR, (unsigned char *)_I2C_Interrupt);        // enter the handler routine
    IC_IMRL_0 &= ~(I2C_PIF_INT_L | MASK_ALL_INT);
    #endif
    #if defined _WINDOWS
    fnConfigSimI2C(pars->Channel, (pars->usSpeed * 1000));               // {32}
    #endif
}

// Send a first byte to I2C bus
//
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)              // {66}
{
    static int iCheckTxI2C[NUMBER_I2C] = {0};                            // {182}
    unsigned char ucAddress;
    #if NUMBER_I2C > 1                                                   // {57}
    volatile unsigned char *register_ptr;
    if (Channel == 0) {
        register_ptr = I2C_CR_ADD;
    }
    else {
        register_ptr = I2C_CR_1_ADD;
    }
    #else
    volatile unsigned char *register_ptr = I2C_CR_ADD;
    #endif

    ptI2CQue->ucPresentLen = *ptI2CQue->I2C_queue.get++;                 // get present length
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }

    if (ptI2CQue->ucState & TX_ACTIVE) {                                 // restart since we are hanging a second telegram on to previous one
        *register_ptr = (I2C_IEN | I2C_IIEN | I2C_MSTA | I2C_MTX | I2C_RSTA);
    }
    else {
        register_ptr += sizeof(unsigned long);                           // {116} move to status register
        if (iCheckTxI2C[Channel] == 0) {                                 // {182} on first use we check that the bus is not held in a busy state (can happen when a reset took place during an acknowledge period and the slave is holding the bus)
            iCheckTxI2C[Channel] = 1;                                    // checked only once
            uEnable_Interrupt();
            fnConfigI2C_pins(Channel);                                   // check and configure pins for I2C use
            uDisable_Interrupt();
        }
        while (*register_ptr & I2C_IBB) {}                               // {116} wait until the stop has actually been sent to avoid a further transmission being started in the mean time
        register_ptr -= sizeof(unsigned long);                           // {116} move back to CR
        *register_ptr = (I2C_IEN | I2C_IIEN | I2C_MTX);                  // set transmit mode with interrupt enabled
        *register_ptr = (I2C_IEN | I2C_IIEN | I2C_MSTA | I2C_MTX);       // set master mode to cause start condition to be sent
    }
    ucAddress = *ptI2CQue->I2C_queue.get++;
    *(register_ptr + 8) = ucAddress;                                     // send the slave address (this includes the rd/wr bit) - I2DR
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }

    if (ucAddress & 0x01) {                                              // reading
        I2C_tx_control[Channel]->ucState |= (RX_ACTIVE | TX_ACTIVE);
        ptI2CQue->I2C_queue.chars -= 3;
        I2C_rx_control[Channel]->wake_task = *ptI2CQue->I2C_queue.get++; // enter task to be woken when reception has completed
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
    }
    else {
        I2C_tx_control[Channel]->ucState |= (TX_ACTIVE);                 // writing
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->ucPresentLen + 1);
    }

    #if defined _WINDOWS
    if (Channel == 0) {
        I2SR |= I2C_IIF;                                                 // simulate the interrupt directly
        fnSimI2C_devices(I2C_ADDRESS, I2DR);
        iInts |= I2C_INT0;
    }
        #if NUMBER_I2C > 1
    else {
        I2SR_1 |= I2C_IIF;                                               // simulate the interrupt directly
        fnSimI2C_devices(I2C_ADDRESS, I2DR_1);
        iInts |= I2C_INT1;
    }
        #endif
    #endif
}
#endif



#if defined STORAGE_REV_1                                                // {148}
    #if !defined MANAGED_FILES || !defined MANAGED_FILE_READ             // {172}
extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
    #else
extern int  fnReadBytesFlashNonBlocking(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
    #endif
{
    #if !defined NO_FLASH_SUPPORT
        #if defined _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
        #endif
        #if !defined ONLY_INTERNAL_FLASH_STORAGE
    {
        ACCESS_DETAILS AccessDetails;
        while (Size != 0) {
            AccessDetails.BlockLength = Size;                            // synchronise individual accesses to remaining length
            switch (fnGetStorageType(ParLocation, &AccessDetails)) {     // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
            #if !defined _WINDOWS                                        // {153}
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
            #if defined NVRAM
            case _STORAGE_NVRAM:                                         // {170} non-volatile memory that can be treated as RAM
                uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), AccessDetails.BlockLength);
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
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // directly copy memory since this must be a pointer to code (embedded file)
        #endif
    #endif
}
#else
extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {73}
    unsigned char usRead[4];                                             // our file sytem is in external EEPROM via SPI bus.
    #endif
    #if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK        // {83}
        #if defined _WINDOWS
    if (iFetchingInternalMemory != 0) {                                  // {30}
        uMemcpy(ptrValue, ParLocation, Size);                            
        iFetchingInternalMemory = 0;
        return;
    }
        #endif
        #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM        // {29}
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
            #if defined I2C_EEPROM_FILE_SYSTEM                           // {143}
            #define I2C_EEPROM_END_ADDRESS   (I2C_EEPROM_START_ADDRESS + I2C_EEPROM_MEMORY_SIZE)
    if ((ParLocation >= (unsigned char *)I2C_EEPROM_START_ADDRESS) && (ParLocation < (unsigned char *)I2C_EEPROM_END_ADDRESS)) { // read starts in I2C EEPROM memory space
        fnReadI2C_eeprom(ParLocation, ptrValue, Size);                   // read the data from the external device
        return;
    }
            #endif
            #if defined SPI_FLASH_ENABLED                                // {9}{29}
                 #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {29}
    if (ParLocation >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + SIZE_OF_NVRAM))) // {77} we are working from external SPI FLASH memory 
                #else
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory 
                #endif
    {
                #if defined SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        MAX_FILE_LENGTH SPI_FlashLength;
        do {                                                             // {147}
                #endif
                #if defined SPI_FLASH_SST25                              // {71}
            unsigned char *Address_SST = fnGetSPI_FLASH_address(ParLocation EXTENDED_CS);
                #else
            unsigned short usPageNumber;
            unsigned short usPageOffset;
            usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
                #endif
                #if defined SPI_FLASH_MULTIPLE_CHIPS                     // {147}
                    #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            SPI_FlashLength = ((FLASH_START_ADDRESS + SIZE_OF_FLASH + SIZE_OF_NVRAM + ((iChipSelect + 1) * SPI_DATA_FLASH_0_SIZE)) - (CAST_POINTER_ARITHMETIC)ParLocation); // the maximum amount of data that can be read from the present SPI Flash
                    #else
            SPI_FlashLength = ((uFILE_START + FILE_SYSTEM_SIZE + ((iChipSelect + 1) * SPI_DATA_FLASH_0_SIZE)) - (CAST_POINTER_ARITHMETIC)ParLocation); // the maximum amount of data that can be read from the present SPI Flash
                    #endif
            if (Size > SPI_FlashLength) {                                // if the read will overflow the end of the present SPI Flash device
                MAX_FILE_LENGTH RemainingLength = (Size - SPI_FlashLength);
                Size = SPI_FlashLength;                                  // reduce the first read length to the end of the device
                SPI_FlashLength = RemainingLength;                       // the size remaining to be read from following SPI Flash
            }
            else {
                SPI_FlashLength = 0;                                     // single read only is required
            }
                #endif
                #if defined SPI_FLASH_ST
            fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                #elif defined SPI_FLASH_SST25                            // {71}
            fnSPI_command(READ_DATA_BYTES, (unsigned long)Address_SST, _EXTENDED_CS ptrValue, Size);
                #else
                    #if SPI_FLASH_PAGE_LENGTH >= 1024                    // {23}
            fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #elif SPI_FLASH_PAGE_LENGTH >= 512
            fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
            fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #endif
                #if defined SPI_FLASH_MULTIPLE_CHIPS                     // {147}
            ParLocation += Size;
            ptrValue += Size;
            Size = SPI_FlashLength;
        } while (SPI_FlashLength != 0);
                #endif
        return;
    }
            #endif
            #if defined I2C_EEPROM_FILE_SYSTEM                           // {146}
    if ((ParLocation + Size) >= (unsigned char *)I2C_EEPROM_START_ADDRESS) { // the read overflows from internal FLASH to external EEPROM
        MAX_FILE_LENGTH Size1 = ((unsigned char *)I2C_EEPROM_START_ADDRESS - ParLocation);
        uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size1);
        Size -= Size1;
        ParLocation += Size1;
        ptrValue += Size1;
        fnReadI2C_eeprom(ParLocation, ptrValue, Size);                   // read the data from the external device
    }
    else {
        uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);
    }
            #else
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the Coldfire uses a file system in FLASH with no access restrictions so we can simply copy the data {59}
            #endif
        #endif
    #else
    uMemcpy(ptrValue, ParLocation, Size);                                // {128} directly copy memory since this must be a pointer to code
    #endif
}
#endif

#if defined USE_PARAMETER_BLOCK //&& defined ACTIVE_FILE_SYSTEM          {68}

#define PARAMETER_BLOCK_1 (unsigned char*)(PARAMETER_BLOCK_START)        // {58}
#define PARAMETER_BLOCK_2 (PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)


// The M5223X has 2k byte blocks which can be individually modified so we use one of these, and a backup if desired
//
extern unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned char ucValidUse[2];
    unsigned char ucCompare;

    // The first 2 byte locations define the validity of the block. 0x55 0x55 means it is fully valid. 0x55 0xff means it contains temporary unvalidated data
    fnGetParsFile(PARAMETER_BLOCK_1, (unsigned char *)ucValidUse, sizeof(ucValidUse));

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
        fnGetParsFile(PARAMETER_BLOCK_2, (unsigned char *)ucValidUse, sizeof(ucValidUse));
        if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
            return 2;                                                    // second block matched request
        }
    }
#endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}

// Retrieve parameter from the defined block
//
extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1;

#if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
#endif

    ptrPar += (usParameterReference + 2);                                // first parameter starts after validation information

    fnGetParsFile(ptrPar, ucValue, usLength);
    return 0;
}

// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(unsigned char *ptrAddInPar)                  // {4}{22}
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}


extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned char ucValid = 0x55;
    int iBlockUse = 0;

    if ((TEMP_PARS & ucValidBlock) != 0) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if ((ucValidBlock & BLOCK_INVALID) != 0) {                           // no valid parameter blocks have been found so we can use the first for saving the data to.
        fnDeleteParBlock((unsigned char *)PARAMETER_BLOCK_1);            // we delete it to be sure it is fresh
#if defined USE_PAR_SWAP_BLOCK
        fnDeleteParBlock((unsigned char *)PARAMETER_BLOCK_2);
#endif
        ucValidBlock = 1;
    } 
#if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar = PARAMETER_BLOCK_2;
    }
#endif
    ptrStart = ptrPar;                                                   // first parameter starts after validation information
    ptrPar += (usParameterReference + 2);                                // we save each byte as a byte                                           

    if (usLength == 0) {                                                 // {189} command rather than normal write
        if (iBlockUse != 0) {                                            // if we have a new set waiting, set it to use state
            iBlockUse = 0;
#if defined USE_PAR_SWAP_BLOCK
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
            if (usParameterReference != 0) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
        fnWriteBytesFlash(ptrPar, ucValue, usLength);                    // the M5223X can do accumulative writes so we can simply call the normal write function
    }                                                                    // now we must validate the parameter block which we have just written to
    fnWriteBytesFlash(ptrStart, (unsigned char *)&ucValid, sizeof(ucValid)); // validate data
    if (iBlockUse == 0) {
        fnWriteBytesFlash((ptrStart + sizeof(ucValid)), (unsigned char *)&ucValid, sizeof(ucValid)); // use data
    }
    return 0;
}

#endif

#if defined GLOBAL_HARDWARE_TIMER

static __interrupt__ void _hwtimer_interrupt(void)
{
    static const unsigned char hw_timer_int_message[HEADER_LENGTH] = { INTERNAL_ROUTE, INTERNAL_ROUTE, TASK_TIMER, INTERRUPT_EVENT, 0 };
    HW_TIMER_MODE_REGISTER &= ~(DMA_TIMER_STOP_COUNT_MASK);              // stop timer running {33}
    HW_TIMER_COUNTER = 0;                                                // reset timer - also indicating it is not in use {33}
    HW_TIMER_EVENT_REGISTER = DMA_TIM_EVENT_REF;                         // reset interrupt request flag {33 - corrected from DTER3}
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting message to queue
    fnWrite(INTERNAL_ROUTE, (unsigned char*)hw_timer_int_message, HEADER_LENGTH); // inform the global interrupt task
    iInterruptLevel = 0;                                                 // release
}


// In order to support a global hardware timer a second timer is required. We use DMA Timer 3 - a 32 bit timer.
//
extern CLOCK_LIMIT fnSetHardwareTimer(CLOCK_LIMIT *hw_delay)
{
    CLOCK_LIMIT Adjusted = 0;
    CLOCK_LIMIT NewDelay = *hw_delay;                                    // clock count desired {6}

    uDisable_Interrupt();                                                // protect region

/*  if (HW_TIMER_REFERENCE_REGISTER == 0xffffffff) {                     // never been used before {43} - moved to initialisation
        fnSetIntHandler(HW_TIMER_VECTOR, (unsigned char *)_hwtimer_interrupt);
        HW_TIMER_INTERRUPT_CONFIG_REGISTER = HW_TIMER_INTERRUPT_PRIORITY;// define interrupts level and priority
        IC_IMRL_0 &= ~(HW_TIMER_INTERRUPT_FLAG | MASK_ALL_INT);          // unmask interrupt source
        HW_TIMER_COUNTER = 0;                                            // ensure reset on start {33}
    } */

    if (HW_TIMER_COUNTER != 0) {                                         // timer already running {33}
        CLOCK_LIMIT Remaining = (HW_TIMER_REFERENCE_REGISTER - HW_TIMER_COUNTER); // time remaining before next timeout (in clocks)
        if (NewDelay >= Remaining) {
            *hw_delay = (NewDelay - Remaining);                          // leave the present timer running but adjust the next setting
            uEnable_Interrupt();
            return 0;
        }
        HW_TIMER_MODE_REGISTER &= ~(DMA_TIMER_STOP_COUNT_MASK | ORRI);   // freeze the timer and disable interrupt
        Adjusted = HW_TIMER_REFERENCE_REGISTER;                          // note the frozen value
    }
    HW_TIMER_REFERENCE_REGISTER = NewDelay;                              // set ms unit compare rate
    HW_TIMER_EVENT_REGISTER = (DMA_TIM_EVENT_CAP | DMA_TIM_EVENT_REF);   // clear possible events

#if MILLISEC == LOW_RES_MS                                               // configurable resolution {6}
    HW_TIMER_MODE_REGISTER = (BUS_CLOCK_16 | ORRI | ENABLE_DMA_TIMER | ((256-1) << PRESCALER_SHIFT)); // bus clock / 16 / 256, enable interrupt and enable timer (again) {33 - don't reset on match}
#elif MILLISEC == MED_RES_MS
    HW_TIMER_MODE_REGISTER = (BUS_CLOCK_16 | ORRI | ENABLE_DMA_TIMER | ((16-1) << PRESCALER_SHIFT));  // bus clock / 16 / 16, enable interrupt and enable timer (again) {33 - don't reset on match}
#else
    HW_TIMER_MODE_REGISTER = (BUS_CLOCK_16 | ORRI | ENABLE_DMA_TIMER);   // bus clock, enable interrupt and enable timer (again) {33 - don't reset on match}
#endif
    *hw_delay = NewDelay;                                                // this is the value in clock pulses
    uEnable_Interrupt();                                                 // enable interrupts again
#if defined _WINDOWS
    if (HW_TIMER_COUNTER == 0) {                                         // {33}
        (HW_TIMER_COUNTER)++;                                            // ensure timer is incrementing when simulating {33}
    }
#endif
    return Adjusted;
}

// Stop the hardware timer
//
extern CLOCK_LIMIT fnStopHW_Timer(void)                                  // {43}
{
    CLOCK_LIMIT count_value;
    HW_TIMER_MODE_REGISTER &= ~(DMA_TIMER_STOP_COUNT_MASK);              // stop timer running
    count_value = HW_TIMER_COUNTER;                                      // save present count value
    HW_TIMER_EVENT_REGISTER = DMA_TIM_EVENT_REF;                         // reset possible interrupt request
    HW_TIMER_COUNTER = 0;
    return count_value;                                                  // return the value when stopped
}
#endif


#if defined CAN_INTERFACE


/**************************** CAN interrupt ******************************************/

// This is the general interrupt routine called when a buffer interrupt occurs
//
static void fnCanInt(int iIntBuf)
{
    unsigned char can_int_message[HEADER_LENGTH] = {INTERNAL_ROUTE, INTERNAL_ROUTE, 0, INTERRUPT_EVENT, 0};
    CANQUE *ptrCanQue = &can_queue[iIntBuf];
    int iNoWake = 0;

    can_int_message[MSG_DESTINATION_TASK] = ptrCanQue->TaskToWake;

    if (ptrCanQue->ucMode & CAN_TX_BUF) {                                // determine whether it is an rx or tx buffer
        if (CAN_RX_REMOTE_RX & ptrCanQue->ucMode) {                      // this buffer has just transmitted a remote frame request
            if (CAN_TX_BUF_ACTIVE & ptrCanQue->ucMode) {                 // interrupt due to transmission
                ptrCanQue->ucMode &= ~CAN_TX_BUF_ACTIVE;                 
                iNoWake = 1;
            }
            else {                                                       // a response has been detected
                can_int_message[MSG_INTERRUPT_EVENT] = CAN_RX_REMOTE_MSG;// rx message received
                ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_REMOTE_RX | CAN_RX_BUF_FULL); // indicate message waiting
            }
        }
        else if (CAN_TX_BUF_REMOTE & ptrCanQue->ucMode) {
            if (ptrCanQue->ucMode & CAN_TX_ACK_ON) {
                can_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_REMOTE_ACK;// remote frame transmission message successfully acked
            }
            else {
                iNoWake = 1;
            }
            ptrCanQue->ucMode &= ~CAN_TX_ACK_ON;                         // we only ack once
        }
        else {
            if (CAN_TX_ACK_ON & ptrCanQue->ucMode) {
                can_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_ACK;       // tx message successfully acked
            }
            else {
                iNoWake = 1;                                             // no acknowledgement required
            }
            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);          // indicate successful transmission, buffer free for further use
        }
    }
    else {                                                               // receive frame
        COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;
        ptrMessageBuffer += iIntBuf;
        *(unsigned char *)ptrMessageBuffer = 0;                          // deactivate buffer so that it doesn't get overwritten

        can_int_message[MSG_INTERRUPT_EVENT] = CAN_RX_MSG;               // rx message received
        ptrCanQue->ucMode |= CAN_RX_BUF_FULL;                            // indicate message waiting
    }
    CAN_IFLAG = (0x0001 << iIntBuf);                                     // clear interrupt flag
#if defined _WINDOWS
    CAN_IFLAG &= ~(0x0001 << iIntBuf);
#endif
    if (!iNoWake) {
        iInterruptLevel++;                                               // {102} ensure interrupts remain blocked when putting message to queue
        fnWrite(INTERNAL_ROUTE, (unsigned char *)can_int_message, HEADER_LENGTH); // inform the handling task of event
        iInterruptLevel--;
    }
}


static __interrupt__ void _CAN_buf_Interrupt_0(void)
{
    fnCanInt(0);
}

static __interrupt__ void _CAN_buf_Interrupt_1(void)
{
    fnCanInt(1);
}

static __interrupt__ void _CAN_buf_Interrupt_2(void)
{
    fnCanInt(2);
}

static __interrupt__ void _CAN_buf_Interrupt_3(void)
{
    fnCanInt(3);
}

static __interrupt__ void _CAN_buf_Interrupt_4(void)
{
    fnCanInt(4);
}

static __interrupt__ void _CAN_buf_Interrupt_5(void)
{
    fnCanInt(5);
}

static __interrupt__ void _CAN_buf_Interrupt_6(void)
{
    fnCanInt(6);
}

static __interrupt__ void _CAN_buf_Interrupt_7(void)
{
    fnCanInt(7);
}

static __interrupt__ void _CAN_buf_Interrupt_8(void)
{
    fnCanInt(8);
}

static __interrupt__ void _CAN_buf_Interrupt_9(void)
{
    fnCanInt(9);
}

static __interrupt__ void _CAN_buf_Interrupt_10(void)
{
    fnCanInt(10);
}

static __interrupt__ void _CAN_buf_Interrupt_11(void)
{
    fnCanInt(11);
}

static __interrupt__ void _CAN_buf_Interrupt_12(void)
{
    fnCanInt(12);
}

static __interrupt__ void _CAN_buf_Interrupt_13(void)
{
    fnCanInt(13);
}

static __interrupt__ void _CAN_buf_Interrupt_14(void)
{
    fnCanInt(14);
}

static __interrupt__ void _CAN_buf_Interrupt_15(void)
{
    fnCanInt(15);
}

static __interrupt__ void _CAN_bus_off_Interrupt(void)
{
    CAN_ERRSTAT = BOFFINT;
}

// Error interrupt
//
static __interrupt__ void _CAN_error_Interrupt(void)
{
    volatile unsigned long ulError = CAN_ERRSTAT;                        // read the error status register, clearing error bits
    unsigned char can_error_int_message[HEADER_LENGTH];

    can_error_int_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
    can_error_int_message[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;
    can_error_int_message[MSG_DESTINATION_TASK] = CAN_ERROR_TASK;
    can_error_int_message[MSG_SOURCE_TASK]      = INTERRUPT_EVENT;

    while (ulError & (BIT1ERROR | BIT0ERROR | CAN_CRC_ERR | CAN_ACK_ERR | CAN_FRM_ERR | CAN_STF_ERR)) {
#if defined _WINDOWS
        CAN_ERRSTAT &= ~ (BIT1ERROR | BIT0ERROR | CAN_CRC_ERR | CAN_ACK_ERR | CAN_FRM_ERR | CAN_STF_ERR);
#endif
        if (ulError & CAN_ACK_ERR) {                                     // a transmission received no ack
            COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;
            CANQUE *ptrCanQue = can_queue;                               // we need to search for buffers which are transmitting
            int i = NUMBER_CAN_MESSAGE_BUFFERS;
            can_error_int_message[MSG_INTERRUPT_EVENT] = 0;
            while (i--) {
                if (ptrCanQue->ucMode & CAN_TX_BUF_ACTIVE) {             // this buffer is presently transmitting a message
                    if (++(ptrCanQue->ucErrors) >= MAX_TX_CAN_TRIES) {   // we allow a few attempts before quitting (it also filters counting normal active buffers)
                        ptrMessageBuffer->ulCode_Len_TimeStamp = ((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE); // stop transmission
                        can_error_int_message[MSG_DESTINATION_TASK] = ptrCanQue->TaskToWake;
                        if (ptrCanQue->ucMode & CAN_TX_BUF_REMOTE) {
                            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_REMOTE_ERROR;
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_BUF_FULL | CAN_TX_BUF_REMOTE); // mark that it is an errored transmission buffer
                        }
                        else {
                            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_ERROR;
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_BUF_FULL); // mark that it is an errored transmission buffer
                        }
                        break;
                    }
                }
                ptrCanQue++;
                ptrMessageBuffer++;
            }
        }
        else {
            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_OTHER_ERROR;
        }
        CAN_ERRSTAT = ERRINT;                                            // clear the error interrupt
#if defined _WINDOWS
        CAN_ERRSTAT &= ~(ERRINT);
#endif
        if (can_error_int_message[MSG_INTERRUPT_EVENT] != 0) {
            iInterruptLevel++;                                           // ensure interrupts remain blocked when putting message to queue
            fnWrite(INTERNAL_ROUTE, can_error_int_message, HEADER_LENGTH); // inform the handling task of event
            iInterruptLevel--;
        }
        ulError = CAN_ERRSTAT;                                           // check the status again
    }    
}

#if defined _WINDOWS
extern UTASK_TASK fnGetCANOwner(int iChannel, int i)
{
    return(can_queue[i].TaskToWake);
}
#endif


// The best choice of clock input is from the external crystal (lowest jitter), however this may not always enable the best settings to achieve the required speed.
// The choice of clock source is user-defined but this routine tries to achieve best settings using highest time quanta resolution. 
//
// There are up to 25 time quanta in a CAN bit time and the bit frequency is equal to the clock frequency divided by the quanta number (8..25 time quanta range)
// There is always a single time quanta at the start of a bit period called the SYNC_SEG which can not be changed (transitions are expected to occur on the bus during this period)
// The sampling occurs after time segment 1, which is made up of a propagation segment (1..8 time quanta) plus a phase buffer segment 1 (1..8 time quanta),
// followed by time segment 2, made up of phase buffer segment 2 (2..8 time quanta)
//
// CAN standard compliant bit segment settings give the following ranges (re-synchronisation jump width of 2 is used since it is complient with all))
// Time segment 1 should be 5..10 when time segment 2 is 2 (min/max time quanta per bit is 8/13)
// Time segment 1 should be 4..11 when time segment 2 is 3 (min/max time quanta per bit is 8/15)
// Time segment 1 should be 5..12 when time segment 2 is 4 (min/max time quanta per bit is 10/17)
// Time segment 1 should be 6..13 when time segment 2 is 5 (min/max time quanta per bit is 12/19)
// Time segment 1 should be 7..14 when time segment 2 is 6 (min/max time quanta per bit is 14/21)
// Time segment 1 should be 8..15 when time segment 2 is 7 (min/max time quanta per bit is 16/23)
// Time segment 1 should be 9..16 when time segment 2 is 8 (min/max time quanta per bit is 18/25)
//
static unsigned long fnOptimalCAN_clock(unsigned short usMode, unsigned long ulSpeed) // {156}
{
    unsigned long ulClockSourceFlag = EXTAL_CLK_SOURCE;
    unsigned long ulClockSpeed;
    unsigned long ulLowestError = 0xffffffff;
    unsigned long ulCanSpeed;
    unsigned long ulError;
    unsigned long ulPrescaler;
    int iTimeQuanta = 25;                                                // highest value for highest control resolution
    int iBestTimeQuanta = 25;
    unsigned long ulBestPrescaler;
    if (CAN_USER_SETTINGS & usMode) {
        return ulSpeed;                                                  // the user is passing optimal configuration settings directly
    }
    if (CAN_PLL_CLOCK & usMode) {
        ulClockSpeed = (BUS_CLOCK / 2);
        ulClockSourceFlag = CLK_SRC_INT_BUS;
    }
    else {
        ulClockSpeed = CRYSTAL_FREQ;
    }
    while (iTimeQuanta >= 8) {                                           // test for best time quanta
        ulCanSpeed = (ulClockSpeed/iTimeQuanta);                         // speed without prescaler
        ulPrescaler = ((ulCanSpeed + (ulSpeed/2))/ulSpeed);              // best prescale value
        if (ulPrescaler > 256) {
            ulPrescaler = 256;                                           // maximum possible prescale divider
        }
        ulCanSpeed /= ulPrescaler;
        if (ulCanSpeed >= ulSpeed) {                                     // determine the absolute error value with this quanta setting
            ulError = (ulCanSpeed - ulSpeed);
        }
        else {
            ulError = (ulSpeed - ulCanSpeed);
        }
        if (ulError < ulLowestError) {                                   // if this is an improvement
            ulLowestError = ulError;
            iBestTimeQuanta = iTimeQuanta;                               // best time quanta value
            ulBestPrescaler = ulPrescaler;
        }
        iTimeQuanta--;
    }
    ulBestPrescaler--;
    ulBestPrescaler <<= 24;                                              // move the prescale value into position
    if (iBestTimeQuanta >= 18) {                                         // determine the phase buffer length value
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN8;
        iBestTimeQuanta -= (8 + 1);                                      // remaining time quanta (time segment 1) after removal of the time segment 2 and the SYN_SEG
    }
    else if (iBestTimeQuanta >= 16) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN7;
        iBestTimeQuanta -= (7 + 1);
    }
    else if (iBestTimeQuanta >= 14) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN6;
        iBestTimeQuanta -= (6 + 1);
    }
    else if (iBestTimeQuanta >= 12) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN5;
        iBestTimeQuanta -= (5 + 1);
    }
    else if (iBestTimeQuanta >= 10) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN4;
        iBestTimeQuanta -= (4 + 1);
    }
    else {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN3;
        iBestTimeQuanta -= (3 + 1);
    }
    if (iBestTimeQuanta & 0x1) {                                         // odd
        iBestTimeQuanta /= 2;                                            // PROP_SEG and PSEG1 to achieve time segment 1
        ulBestPrescaler |= iBestTimeQuanta;                              // set propogation bit time (1 more than phase buffer segment 1)
        iBestTimeQuanta--;
        ulBestPrescaler |= (iBestTimeQuanta << 19);                      // set phase buffer segment 1
    }
    else {                                                               // even
        iBestTimeQuanta /= 2;                                            // PROP_SEG and PSEG1 to achieve time segment 1 and phase buffer segment 1
        iBestTimeQuanta--;
        ulBestPrescaler |= ((iBestTimeQuanta << 19) | (iBestTimeQuanta));// {159} set equal propogation bit times
    }
    return (RJW_2 | ulClockSourceFlag | ulBestPrescaler);                // initialise the CAN controller with the required speed and parameters
}

// Hardware initialisation of CAN controller
//
extern void fnInitCAN(CANTABLE *pars)
{
    COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;                     // the first of 16 message buffers in the FlexCan module

    POWER_UP(POWER_FCAN);                                                // {46} enable clock to the module

    uMemset(ptrMessageBuffer, 0x00, (sizeof(COLDFIRE_CAN_BUF) * NUMBER_CAN_MESSAGE_BUFFERS)); // the buffers are not reset so clear here  

    CANMCR &= ~CAN_MDIS;                                                 // move from disabled to freeze mode

#if defined _M521X
    #if defined M5213EVB
        #if defined REAL_BOARD
      PUAPAR |= 0xa0;                                                    // configure UA port 2,3 as CAN
        #else
      PASPAR |= 0x0a;                                                    // configure AS port 0,1 as CAN
        #endif
    #else
    PUAPAR |= 0xa0;                                                      // configure UA port 2,3 as CAN
    #endif 
#else
    #if defined _M5225X
        #if defined CAN_ON_TH                                            // CAN pins on TH4,5
    PTHPAR |= ((ALT_1_FUNCTION_Q << BIT_5_Q_SHIFT) | (ALT_1_FUNCTION_Q << BIT_4_Q_SHIFT));
        #else                                                            // default CAN pins on UC0,1
    PUCPAR |= ((ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT) | (ALT_1_FUNCTION_Q << BIT_0_Q_SHIFT));
        #endif
    #else                                                                // M5223X parts
        #if defined CAN_ON_AS2                                           // CAN pins on AS2,3
    PASPAR |= ((ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT) | (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT));
        #elif defined CAN_ON_QS                                          // CAN pins on QS0,1
    PQSPAR |= ((ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT) | (ALT_1_FUNCTION_Q << BIT_0_Q_SHIFT));
        #elif defined CAN_ON_UA                                          // CAN pins on UA2,3
    PUAPAR |= ((ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT) | (ALT_1_FUNCTION_Q << BIT_2_Q_SHIFT));
        #else                                                            // default CAN pins on AS0,1
    PASPAR |= ((ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT) | (ALT_1_FUNCTION_Q << BIT_0_Q_SHIFT));
        #endif
    #endif
#endif
    CANCTRL = fnOptimalCAN_clock(pars->usMode, pars->ulSpeed);           // {156}

    if (pars->usMode & CAN_LOOPBACK) {
        CANCTRL |= LPB;                                                  // set loopback mode
    }

#if defined _M521X                                                       // configure interrupts (tx and rx)
    IC_ICR_0_23 = CAN0_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN0_VECTOR, (unsigned char *)_CAN_buf_Interrupt_0); // enter the handler routine
    IC_ICR_0_24 = CAN1_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN1_VECTOR, (unsigned char *)_CAN_buf_Interrupt_1); // enter the handler routine
    IC_ICR_0_25 = CAN2_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN2_VECTOR, (unsigned char *)_CAN_buf_Interrupt_2); // enter the handler routine
    IC_ICR_0_26 = CAN3_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN3_VECTOR, (unsigned char *)_CAN_buf_Interrupt_3); // enter the handler routine
    IC_ICR_0_27 = CAN4_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN4_VECTOR, (unsigned char *)_CAN_buf_Interrupt_4); // enter the handler routine
    IC_ICR_0_28 = CAN5_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN5_VECTOR, (unsigned char *)_CAN_buf_Interrupt_5); // enter the handler routine
    IC_ICR_0_29 = CAN6_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN6_VECTOR, (unsigned char *)_CAN_buf_Interrupt_6); // enter the handler routine
    IC_ICR_0_30 = CAN7_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN7_VECTOR, (unsigned char *)_CAN_buf_Interrupt_7); // enter the handler routine
    IC_ICR_0_31 = CAN8_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN8_VECTOR, (unsigned char *)_CAN_buf_Interrupt_8); // enter the handler routine
    IC_ICR_0_32 = CAN9_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN9_VECTOR, (unsigned char *)_CAN_buf_Interrupt_9); // enter the handler routine
    IC_ICR_0_33 = CAN10_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN10_VECTOR, (unsigned char *)_CAN_buf_Interrupt_10);// enter the handler routine
    IC_ICR_0_34 = CAN11_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN11_VECTOR, (unsigned char *)_CAN_buf_Interrupt_11);// enter the handler routine
    IC_ICR_0_35 = CAN12_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN12_VECTOR, (unsigned char *)_CAN_buf_Interrupt_12);// enter the handler routine
    IC_ICR_0_36 = CAN13_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN13_VECTOR, (unsigned char *)_CAN_buf_Interrupt_13);// enter the handler routine
    IC_ICR_0_37 = CAN14_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN14_VECTOR, (unsigned char *)_CAN_buf_Interrupt_14);// enter the handler routine
    IC_ICR_0_38 = CAN15_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN15_VECTOR, (unsigned char *)_CAN_buf_Interrupt_15);// enter the handler routine
    IC_ICR_0_39 = CANERR_INTERRUPT_PRIORITY;                             // define interrupts level and priority
    fnSetIntHandler(CANERR_VECTOR, (unsigned char *)_CAN_error_Interrupt);// enter the handler routine
    IC_ICR_0_40 = CANBOFF_INTERRUPT_PRIORITY;                            // define interrupts level and priority
    fnSetIntHandler(CANBOFF_VECTOR, (unsigned char *)_CAN_bus_off_Interrupt);// enter the handler routine
#else
    IC_ICR_1_8  = CAN0_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN0_VECTOR, (unsigned char *)_CAN_buf_Interrupt_0); // enter the handler routine
    IC_ICR_1_9  = CAN1_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN1_VECTOR, (unsigned char *)_CAN_buf_Interrupt_1); // enter the handler routine
    IC_ICR_1_10 = CAN2_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN2_VECTOR, (unsigned char *)_CAN_buf_Interrupt_2); // enter the handler routine
    IC_ICR_1_11 = CAN3_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN3_VECTOR, (unsigned char *)_CAN_buf_Interrupt_3); // enter the handler routine
    IC_ICR_1_12 = CAN4_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN4_VECTOR, (unsigned char *)_CAN_buf_Interrupt_4); // enter the handler routine
    IC_ICR_1_13 = CAN5_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN5_VECTOR, (unsigned char *)_CAN_buf_Interrupt_5); // enter the handler routine
    IC_ICR_1_14 = CAN6_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN6_VECTOR, (unsigned char *)_CAN_buf_Interrupt_6); // enter the handler routine
    IC_ICR_1_15 = CAN7_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN7_VECTOR, (unsigned char *)_CAN_buf_Interrupt_7); // enter the handler routine
    IC_ICR_1_16 = CAN8_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN8_VECTOR, (unsigned char *)_CAN_buf_Interrupt_8); // enter the handler routine
    IC_ICR_1_17 = CAN9_INTERRUPT_PRIORITY;                               // define interrupts level and priority
    fnSetIntHandler(CAN9_VECTOR, (unsigned char *)_CAN_buf_Interrupt_9); // enter the handler routine
    IC_ICR_1_18 = CAN10_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN10_VECTOR, (unsigned char *)_CAN_buf_Interrupt_10);// enter the handler routine
    IC_ICR_1_19 = CAN11_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN11_VECTOR, (unsigned char *)_CAN_buf_Interrupt_11);// enter the handler routine
    IC_ICR_1_20 = CAN12_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN12_VECTOR, (unsigned char *)_CAN_buf_Interrupt_12);// enter the handler routine
    IC_ICR_1_21 = CAN13_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN13_VECTOR, (unsigned char *)_CAN_buf_Interrupt_13);// enter the handler routine
    IC_ICR_1_22 = CAN14_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN14_VECTOR, (unsigned char *)_CAN_buf_Interrupt_14);// enter the handler routine
    IC_ICR_1_23 = CAN15_INTERRUPT_PRIORITY;                              // define interrupts level and priority
    fnSetIntHandler(CAN15_VECTOR, (unsigned char *)_CAN_buf_Interrupt_15);// enter the handler routine
    IC_ICR_1_24 = CANERR_INTERRUPT_PRIORITY;                             // define interrupts level and priority
    fnSetIntHandler(CANERR_VECTOR, (unsigned char *)_CAN_error_Interrupt);// enter the handler routine
    IC_ICR_1_25 = CANBOFF_INTERRUPT_PRIORITY;                            // define interrupts level and priority {20}
    fnSetIntHandler(CANBOFF_VECTOR, (unsigned char *)_CAN_bus_off_Interrupt);// enter the handler routine
#endif

    CAN_IFLAG = (CAN_ALL_BUFFERS_INT);                                   // ensure no pending interrupts
    CAN_IMASK = (CAN_ALL_BUFFERS_INT);                                   // enable buffer interrupts

    CAN_ERRSTAT = (ERRINT | BOFFINT);                                    // ensure no pending error interrupts 
    CANCTRL |= (ERRMSK | BOFFMSK);

#if defined _M521X
    IC_IMRL_0 &= ~(CAN_TX_INTS_L | CAN_RX_INTS_L | MASK_ALL_INT);        // unmask interrupt sources
    IC_IMRH_0 &= ~(CAN_RX_INTS_H | CAN_ERR_INT_H | CAN_BOFF_INT_H | MASK_ALL_INT); // unmask interrupt sources 
#else
    IC_IMRL_1 &= ~(CAN_TX_INTS_L | CAN_RX_INTS_L | CAN_ERR_INT_L | CAN_BOFF_INT_L | MASK_ALL_INT); // unmask interrupt sources
#endif

    CANMCR &= ~(CAN_FRZ | CAN_HALT);                                     // leave freeze mode and start synchronisation

#if defined _WINDOWS
    fnSimCAN(0, 0, CAN_SIM_INITIALISE);                                  // {154}
#endif
}



// Hardware configuration of CAN controller
//
extern void fnConfigCAN(QUEUE_HANDLE DriverID, CANTABLE *pars)
{
    int i;
    unsigned char ucTxCnt = pars->ucTxBuffers;                           // {156} avoid modifying passed parameters
    unsigned char ucRxCnt = pars->ucRxBuffers;
    CANQUE *ptrCanQue = can_queue;
    COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;                     // the first of 16 message buffers in the FlexCan module

    CANMCR |= (CAN_FRZ | CAN_HALT);                                      // move to freeze mode

    for (i = 0; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {                   // initialise the requested number of transmit buffers
        if (ucTxCnt == 0) {
            break;
        }
        if (!(ptrCanQue->DriverID)) {                                    // not yet allocated
            ptrCanQue->DriverID = DriverID;
            ptrCanQue->TaskToWake = pars->Task_to_wake;
            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);
            if (pars->ulTxID & CAN_EXTENDED_ID) {
                ptrCanQue->ulPartnerID = pars->ulTxID;                   // enter default destination ID
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | IDE); // enable extended ID transmission
            }
            else {
                ptrCanQue->ulPartnerID = ((pars->ulTxID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK); // enter reception ID for the buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = MB_TX_INACTIVE;
            }
            if (pars->ulRxID & CAN_EXTENDED_ID) {                        // {156}
                ptrCanQue->ulOwnID = pars->ulRxID;
            }
            else {
                ptrCanQue->ulOwnID = ((pars->ulRxID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK);
            }
            ucTxCnt--;
        }
        ptrMessageBuffer++;
        ptrCanQue++;
    }

    ptrMessageBuffer = MBUFF15_ADD;
    ptrCanQue = &can_queue[NUMBER_CAN_MESSAGE_BUFFERS-1];
    for (i = 0; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {                   // initialise the requested number of receive buffers
        if (ucRxCnt == 0) {
            break;
        }
        if (!(ptrCanQue->DriverID)) {                                    // not yet allocated
            ucRxCnt--;
            ptrCanQue->DriverID = DriverID;
            ptrCanQue->TaskToWake = pars->Task_to_wake;
            if (pars->ulRxID & CAN_EXTENDED_ID) {
                if (MBUFF15_ADD == ptrMessageBuffer) {
                    RX15MASK = pars->ulRxIDMask;                         // first allocated receiver buffer has special mask
                }
                else if (MBUFF14_ADD == ptrMessageBuffer) {
                    RX14MASK = pars->ulRxIDMask;                         // second allocated receiver buffer has special mask
                }
                else {
                    RXGMASK = pars->ulRxIDMask;                          // initialise general acceptance mask for use with subsequent buffers
                }
                ptrMessageBuffer->ulID = (pars->ulRxID & CAN_EXTENDED_MASK); // enter reception ID for the buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_EMPTY | IDE);// enable extended ID reception
            }
            else {
                unsigned long ulMask = ((pars->ulRxIDMask << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK);
                if (MBUFF15_ADD == ptrMessageBuffer) {
                    RX15MASK = ulMask;                                   // first allocated receiver buffer has special mask
                }
                else if (MBUFF14_ADD == ptrMessageBuffer) {
                    RX14MASK = ulMask;                                   // second allocated receiver buffer has special mask
                }
                else {
                    RXGMASK = ulMask;                                    // initialise general acceptance mask for use with subsequent buffers
                }
                ptrMessageBuffer->ulID = ((pars->ulRxID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK); // enter reception ID for the buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = MB_RX_EMPTY;    // enable reception
            }
        }
        ptrMessageBuffer--;
        ptrCanQue--;
    }
    CANMCR &= ~(CAN_FRZ | CAN_HALT);                                     // leave freeze mode and start synchronisation
}


// The CAN driver has called this to send a message
//
extern unsigned char fnCAN_tx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter) // {155}{156}
{
    int i = 0;
    COLDFIRE_CAN_BUF *ptrMessageBuffer;
    unsigned char ucTxMode = (unsigned char)Counter & CAN_TX_MSG_MASK;
    CANQUE *ptrCanQue;
    unsigned char ucRtnCnt; 
    unsigned long ulExtendedID = 0;

    ptrMessageBuffer = MBUFF0_ADD;                                       // the first of 16 message buffers in the FlexCan module
    ptrCanQue = can_queue;

    Counter &= ~CAN_TX_MSG_MASK;
    ucRtnCnt = (unsigned char)Counter;

    if (ucTxMode & (TX_REMOTE_FRAME | TX_REMOTE_STOP)) {                 // only one remote transmit buffer allowed
        for (; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) { 
            if (ptrCanQue->DriverID == DriverID) {                       // find a buffer belonging to us
                if (ptrCanQue->ucMode & CAN_TX_BUF_REMOTE) {             // active remote buffer found
                    if (ucTxMode & TX_REMOTE_STOP) {
                        ptrMessageBuffer->ulCode_Len_TimeStamp = MB_TX_INACTIVE; // disable and free buffer
                        ptrMessageBuffer->ulID = (ptrCanQue->ulPartnerID & ~CAN_EXTENDED_ID); // set tx message buffer to default id
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // buffer remain inactive tx buffer
    #if defined _WINDOWS
                        fnSimCAN(Channel, i, CAN_SIM_FREE_BUFFER);
    #endif
                        return 1;                                        // remote frame transmission disabled
                    }
                    else {
                        ptrMessageBuffer->ulCode_Len_TimeStamp = MB_TX_INACTIVE; // disable so that changes can be made
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // buffer remain inactive tx buffer
                    }
                    break;                                               // the remote buffer has been found
                }
            }
            ptrMessageBuffer++;
            ptrCanQue++;
        }
        if (i >= NUMBER_CAN_MESSAGE_BUFFERS) {                           // no remote buffer present
            i = 0;                                                       // reset ready for restart
            ptrCanQue = can_queue;
            ptrMessageBuffer = MBUFF0_ADD;
        }
    }

    for (; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {
        if (ptrCanQue->DriverID == DriverID) {                           // find a buffer belonging to us
            if ((ptrCanQue->ucMode & CAN_TX_BUF_FREE) && (!(TX_REMOTE_STOP & ucTxMode))) {  // if the transmit buffer is free and not stopping transmission
                if (TX_REMOTE_FRAME & ucTxMode) {                        // depositing a message to be sent on a remote request
                    if (ptrCanQue->ulPartnerID & CAN_EXTENDED_ID) {
                        ulExtendedID = (IDE | MB_TX_SEND_ON_REQ | ((unsigned long)Counter << 16)); // use extended destination ID                           
                    }
                    else {
                        ulExtendedID = (MB_TX_SEND_ON_REQ | ((unsigned long)Counter << 16));
                    }
                    ptrMessageBuffer->ulID = (ptrCanQue->ulPartnerID & ~CAN_EXTENDED_ID); // set tx message buffer to default ID address
                    ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_ACTIVE | CAN_TX_BUF_REMOTE | (CAN_TX_ACK_ON & ucTxMode)); // mark that the buffer is in use
                }
                else {
                    if (ucTxMode & SPECIFIED_ID) {                       // is the user specifying a destination ID or can default be used?
                        unsigned long ulPartnerID = 0;
                        ulPartnerID = *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        if (ulPartnerID & CAN_EXTENDED_ID) {
                            ulExtendedID = IDE;
                            ptrMessageBuffer->ulID = (ulPartnerID & ~CAN_EXTENDED_ID); // send to specified extended ID address
                        }
                        else {
                            ptrMessageBuffer->ulID = ((ulPartnerID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK); // send to specified standard ID address
                        }
                        Counter -= 4;
                    }
                    else {                                               // transmission to default ID
                        if ((ptBuffer == 0) && (Counter == 0)) {         // remote frame request -  after transmission the buffer will become a receiver until read or freed
                            if (ptrCanQue->ulOwnID & CAN_EXTENDED_ID) {
                                ulExtendedID = (MB_TX_SEND_ONCE | RTR | IDE); // use extended destination ID
                            }
                            else {
                                ulExtendedID = (MB_TX_SEND_ONCE | RTR);
                            }
                            ptrMessageBuffer->ulID = (ptrCanQue->ulOwnID & ~CAN_EXTENDED_ID); // request from our default ID address
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_REMOTE_RX | CAN_TX_BUF_ACTIVE | CAN_TX_BUF_REMOTE);
                        }
                        else {
                            if (ptrCanQue->ulPartnerID & CAN_EXTENDED_ID) {
                                ulExtendedID = IDE;                          // use extended destination ID
                            }
                            ptrMessageBuffer->ulID = (ptrCanQue->ulPartnerID & ~CAN_EXTENDED_ID); // send to default ID address
                        }
                    }
                    if (Counter > 8) {                                   // limit the count to maximum
                        Counter = 8;
                    }
                    if (Counter != 0) {                                  // data so it is not a remote frame request   
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_ACTIVE | (CAN_TX_ACK_ON & ucTxMode)); // mark that the buffer is in use                    
                        ulExtendedID |= (MB_TX_SEND_ONCE | ((unsigned long)Counter << 16));
                    }
                }
                // The data needs to be stored as long words in big-endian format
                //
                if (ptBuffer != 0) {
                    uMemcpy(ptrMessageBuffer->ucData, ptBuffer, Counter);// copy message to buffer
                }
                ptrCanQue->ucErrors = 0;
                ptrMessageBuffer->ulCode_Len_TimeStamp = ulExtendedID;   // start the process
    #if defined _WINDOWS
                fnSimCAN(Channel, i, 0);                                 // simulate the buffer operation
    #endif
                return (unsigned char)ucRtnCnt;
            }
        }
        ptrMessageBuffer++;
        ptrCanQue++;
    }
    return 0;                                                            // no free buffer was found...
}

// The CAN driver has called this to collect a received message
//
extern unsigned char fnCAN_get_rx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter) // {155}{156}
{
    int i;
    unsigned char ucLength;
    COLDFIRE_CAN_BUF *ptrMessageBuffer;
    CANQUE *ptrCanQue;
    COLDFIRE_CAN_CONTROL *ptrCAN_control;
    unsigned char ucCommand = (unsigned char)Counter;

    ptrCAN_control = (COLDFIRE_CAN_CONTROL *)CAN_BASE_ADD;
    ptrMessageBuffer = MBUFF15_ADD;                                      // the last of 16 message buffers in the FlexCan module
    ptrCanQue = &can_queue[NUMBER_CAN_MESSAGE_BUFFERS - 1];

    for (i = 0; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {                   // search through receive buffers
        if (ptrCanQue->DriverID == DriverID) {                           // find a buffer belonging to us
            if ((ucCommand & FREE_CAN_RX_REMOTE) && (ptrCanQue->ucMode & CAN_RX_REMOTE_RX)) { // release a waiting remote frame receiver buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_KEEP_CONTENTS));
                ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);      // buffer is now a normal tx buffer again
    #if defined _WINDOWS
                fnSimCAN(Channel, ((NUMBER_CAN_MESSAGE_BUFFERS - 1) - i), CAN_SIM_FREE_BUFFER);
    #endif
                return 1;
            }

            if (ptrCanQue->ucMode & CAN_RX_BUF_FULL) {                   // if the buffer contains data
                if ((!(ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR | GET_CAN_RX_REMOTE | FREE_CAN_RX_REMOTE))) // normal rx buffer read
                     || ((ucCommand & GET_CAN_TX_REMOTE_ERROR) && (ptrCanQue->ucMode & CAN_TX_BUF_REMOTE)) // requesting remote error details
                     || ((ucCommand & GET_CAN_RX_REMOTE) && (ptrCanQue->ucMode & CAN_RX_REMOTE_RX)) // specifically requesting a remote reception buffer
                     || ((ucCommand & GET_CAN_TX_ERROR) && (ptrCanQue->ucMode & CAN_TX_BUF))) { // requesting errored tx buffer
                    volatile unsigned long ulDummy;

                    if (!(ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR))) { // reception
                        while (ptrMessageBuffer->ulCode_Len_TimeStamp & MB_RX_BUSY_BIT) {} // wait until buffer update has terminated

                        if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_RX_CODE_FIELD) == MB_RX_OVERRUN) { // the buffer is now locked
                            *ptBuffer = CAN_RX_OVERRUN;                  // receiver overrun - we have lost an intermediate message
                        }
                        else {
                            *ptBuffer = 0;
                        }

                        if (ptrCanQue->ucMode & CAN_RX_REMOTE_RX) {      // temporary rx buffer - so set back to tx
                            *ptBuffer++ |= CAN_REMOTE_MSG_RX; 
                            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_LENGTH_AND_TIME));
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // buffer is now a normal tx buffer again
                        }
                        else {                                                 
                            *ptBuffer++ |= CAN_MSG_RX;
                        }
                        if (GET_CAN_RX_TIME_STAMP && ucCommand) {
                            *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulCode_Len_TimeStamp >> 8); // write time stamp to input buffer
                            *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulCode_Len_TimeStamp);
                        }
                    }

                    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR | GET_CAN_RX_ID)) {
                        unsigned long ulID = ptrMessageBuffer->ulID;
                        if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                            ulID |= CAN_EXTENDED_ID;                     // mark that it is an extended address
                        }
                        else {
                            ulID >>= CAN_STANDARD_SHIFT;                 // the address if a standard address
                        }
                        *ptBuffer++ = (unsigned char)(ulID >> 24);       // write failed ID to buffer or rx ID
                        *ptBuffer++ = (unsigned char)(ulID >> 16); 
                        *ptBuffer++ = (unsigned char)(ulID >> 8);
                        *ptBuffer++ = (unsigned char)(ulID);
                    }

                    ucLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                    uMemcpy(ptBuffer, ptrMessageBuffer->ucData, ucLength); // copy data content directly to input buffer

                    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR)) {
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);// the buffer may be used again since the lost data has been rescued
                    }
                    else {
                        if (GET_CAN_RX_TIME_STAMP && ucCommand) {
                            ucLength += 3;                               // status and time stamp
                        }
                        else {
                            ucLength++;                                  // status only
                        }
                        if (!(ptrCanQue->ucMode & CAN_TX_BUF_FREE)) {    // if not a remote reception read
                            ptrCanQue->ucMode &= (~CAN_RX_BUF_FULL);
                            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_EMPTY | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD));// free up buffer for further use
                        }
                    }
                    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR | GET_CAN_RX_ID)) {
                        ucLength += 4;                                   // id length
                    }
                    ulDummy = ptrCAN_control->_CAN_TIMER;                // read the free running timer to release internal lock
                    return ucLength;                                     // the number of bytes copied, including OK byte and timestamp
                }
            }
        }
        ptrMessageBuffer--;
        ptrCanQue--;
    }
    return 0;                                                            // no data was found...
}
#endif


#if defined USB_INTERFACE                                                // {24}
#define fnGetEndPointCtr(iEndPoint) (unsigned char *)(ENDPT0_ADD + ((iEndPoint) * sizeof(unsigned long)))

// The Buffer Descriptor Tables work in Little-endian mode so we have to convert some addresses
//
    #if !(defined _M5225X && defined USB_BIGENDIAN)                      // this device can handle big-endian mode so we don't use this function
        #if !defined _WINDOWS                                            // make use of the byterev instruction to perform little/big-endian conversion
     extern unsigned long byte_rev(unsigned long);                       // {91}
    #define fnLE_add(x) byte_rev(x)
        #else
    #define fnLE_add(x) ((x >> 24) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | ((x << 24) & 0xff000000)) // {88}
        #endif
extern void *fnLE_add_F(unsigned long long_word)                         // {88} convert to backdoor address if the input address is not in RAM
{
        #if !defined _WINDOWS
    if (long_word < /*START_OF_SRAM*/ (FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // {120} if the address is in FLASH we use FLASH backdoor address to allow USB controller to access it
        long_word += BACKDOOR_FLASH;
    }
        #endif
    return (void *)fnLE_add(long_word);
}
    #endif


// This routine handles all SETUP and OUT frames. It can send an empty data frame if required by control endpoints or stall on errors.
// It usually clears the handled input buffer when complete, unless the buffer is specified to remain owned by the processor.
//
static int fnProcessInput(int iEndpoint_ref, USB_HW *usb_hardware, unsigned char ucFrameType, M5222X_USB_BD *ptUSB_BD_rx, M5222X_USB_ENDPOINT_BD *ptEndpointBD)
{
    iInterruptLevel = 1;                                                 // ensure interrupts remain blocked when putting messages to queue

    usb_hardware->ptrEndpoint = &usb_endpoints[iEndpoint_ref];           // mark the present transmit endpoint information for possible subroutine or response use
    if (!(usb_endpoints[iEndpoint_ref].ulEndpointSize & ALTERNATE_TX_BUFFER)) {  // set the next transmit pointer details
        usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl; // prepare hardware relevant data for the generic handler's use
        usb_hardware->ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    }
    else {
        usb_hardware->ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl; // prepare hardware relevant data for the generic handler's use
        usb_hardware->ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
    }

    switch (fnUSB_handle_frame(ucFrameType, (unsigned char *)fnLE_add((CAST_POINTER_ARITHMETIC)ptUSB_BD_rx->ptrUSB_BD_Data), iEndpoint_ref, usb_hardware)) { // generic handler routine
    case TERMINATE_ZERO_DATA:                                            // send zero data packet to complete status stage of control transfer
        *usb_hardware->ptr_ulUSB_BDControl = (OWN | usb_hardware->ptrEndpoint->ulNextTxData0); // transmit a zero data packet on control endpoint
        _SIM_USB(USB_SIM_TX, iEndpoint_ref, usb_hardware);
        usb_hardware->ptrEndpoint->ulNextTxData0 ^= DATA_1;              // toggle the data packet
        usb_hardware->ptrEndpoint->ulEndpointSize ^= ALTERNATE_TX_BUFFER;
        ptUSB_BD_rx->ulUSB_BDControl |= (OWN);                           // re-enable reception for this endpoint buffer since it has been freed
        break;
    case MAINTAIN_OWNERSHIP:                                             // don't free the buffer - the application will do this later
        iInterruptLevel = 0;
        return MAINTAIN_OWNERSHIP;;
    case STALL_ENDPOINT:                                                 // send stall
        if (iEndpoint_ref == 0) {                                        // {121} check whether control 0 endpoint
            ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL);// force stall handshake on both control 0 buffers
            ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
            fnSetUSBEndpointState(0, USB_ENDPOINT_STALLED);       
            _SIM_USB(USB_SIM_STALL, 0, usb_hardware);
        }
        else {
            M5222X_USB_ENDPOINT_BD *ptOUT_BD = ptEndpointBD;
            int iIN_ref = fnGetPairedIN(iEndpoint_ref);                  // get the paired IN endpoint reference
            ptEndpointBD = ptrBDT;
            ptEndpointBD += iIN_ref;
            ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL); // stall/halt the corresponding IN endpoint
            ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
            ptEndpointBD = ptOUT_BD;
            fnSetUSBEndpointState(iIN_ref, USB_ENDPOINT_STALLED);        // mark the stall at the IN endpoint
            _SIM_USB(USB_SIM_STALL, iIN_ref, usb_hardware);
        }                       
        // fall through to free buffer
    default:
        usb_hardware->ulRxControl |= (usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS)); // reception buffer size
        ptUSB_BD_rx->ulUSB_BDControl = (OWN | usb_hardware->ulRxControl); // re-enable reception on this endpoint
        usb_endpoints[iEndpoint_ref].ulNextRxData0 = usb_hardware->ulRxControl; // last buffer processed
        break;
    }
    iInterruptLevel = 0;
    return 0;
}

// When the clear feature is received for a stalled endpoint it is cleared in the hardware by calling this routine {121}
//
extern void fnUnhaltEndpoint(unsigned char ucEndpoint)
{
    M5222X_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;
    unsigned char *ptrEndPointCtr = fnGetEndPointCtr(ucEndpoint & ~IN_ENDPOINT);
    ptEndpointBD += (ucEndpoint & ~IN_ENDPOINT);
    if (ucEndpoint & IN_ENDPOINT) {
        usb_endpoints[ucEndpoint & ~IN_ENDPOINT].ulNextTxData0 &= ~DATA_1;
        ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = 0;
        ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = 0;
        *ptrEndPointCtr &= ~EP_STALL;
    }
    else {
        ptEndpointBD->usb_bd_rx_even.ulUSB_BDControl &= ~BDT_STALL;
        ptEndpointBD->usb_bd_rx_odd.ulUSB_BDControl  &= ~BDT_STALL;
    }
}

    #if defined USB_HOST_SUPPORT                                         // {69}
static USB_HW usb_hardware = {0};

// A single-shot HW timer reserved for USB host timing use. It uses PIT1
//
static void fnUSB_delay(void ( *timeout_handler )( void ), unsigned long ulDelay)
{
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.int_handler = timeout_handler;
    pit_setup.int_priority = PIT1_INTERRUPT_PRIORITY;
    pit_setup.count_delay = ulDelay;                                     // delay
    pit_setup.mode = PIT_SINGLE_SHOT;                                    // one-shot interrupt
    fnConfigureInterrupt((void *)&pit_setup);                            // enter interrupt
}

// USB host reset pulse complete
//
static void usb_reset_timeout(void)
{
    M5222X_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;                       // start of BDT
    CTL &= ~USB_RESET;                                                   // remove USB bus reset
    CTL |= USB_EN_SOF_EN;                                                // start sending SOF packets
    usb_hardware.ptrEndpoint = &usb_endpoints[0]; 
    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;// first transmission is from even buffer
    usb_hardware.ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
    if (fnUSB_handle_frame(USB_DEVICE_DETECTED, 0, ((CTL & JSTATE) != 0), &usb_hardware) == SEND_SETUP) { // generic handler routine
        while (CTL & TXSUSPEND_TOKENBUSY) {                              // wait if there is already a token in process
            if (INT_STAT & USB_RST) {                                    // reset state detected (don't know why)
                CTL &= ~TXSUSPEND_TOKENBUSY;
                return;                                                  // quit
            }
        }
        INT_ENB = (TOK_DNE_EN | STALL_EN | USB_ERROR_EN);                // interrupt when complete
        TOKEN = (SETUP_PID << 4);                                        // start transmission of SETUP token to endpoint 0 address at address 0
    }
}

// after a short delay after a possible attach, check the state and decide whether we need to continue with the attached device
//
static void usb_possible_attach(void)
{
    if (INT_STAT & ATTACH) {                                             // a device has really been attached
        if (!(CTL & JSTATE)) {                                           // low speed device connected
            ADDR = LS_EN;                                                // select low speed mode
            ENDPT0 = (HOST_WO_HUB | EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);
        }
        CTL |= USB_RESET;                                                // send reset to USB bus for 10ms
        fnUSB_delay(usb_reset_timeout, PIT_MS_DELAY(10));
    }
    else {                                                               // spurious attach - activate the interrupt again
        INT_ENB = ATTACH_EN;                                             // enable an interrupt on the attach of a device
    }
}
    #endif

extern unsigned long fnUSB_error_counters(int iValue)                    // {181}
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

// USB OTG Interrupt handler
//
static __interrupt__ void _usb_otg_isr(void)
{
    #if !defined USB_HOST_SUPPORT                                        // {69}
    static USB_HW usb_hardware = {0};
    #endif
    unsigned char ucUSB_Int_status; 

    
    while ((ucUSB_Int_status = (unsigned char)(INT_STAT & INT_ENB)) != 0) { // read present status
        if (ucUSB_Int_status & (USB_RST | USB_ERROR | SOF_TOK | SLEEP | RESUME | ATTACH | STALL)) { // {163) - from (ucUSB_Int_status & ~TOK_DNE)
            iInterruptLevel = 1;                                         // ensure interrupts remain blocked when putting messages to queue
    #if defined USB_HOST_SUPPORT                                         // {69}
            if (ucUSB_Int_status & ATTACH) {                             // device has been attached
                INT_STAT = ATTACH;                                       // reset flag
                INT_ENB  = 0;                                            // disable further interrupts
                fnUSB_delay(usb_possible_attach, PIT_MS_DELAY(1));       // wait a short time before checking whether this was a real attach
            }
    #endif
            if (ucUSB_Int_status & USB_RST) {                            // reset detected - D+ and D- in SE0 (single ended logic 0) state for > 2.5us
                INT_STAT = (USB_RST | SLEEP | RESUME_EN | USB_ERROR);    // reset flags
                CTL |= ODD_RST;                                          // reset all odd BDTs which specifies the even bank
                INT_ENB &= ~(RESUME_EN | SLEEP_EN);                      // disable resume and suspend interrupts
                usb_endpoints[0].ulEndpointSize &= ~ALTERNATE_TX_BUFFER;
                usb_endpoints[0].ulNextRxData0 = 0;                      // DATA 0 is always first reception (in SETUP)
                CTL &= ~ODD_RST;
                ADDR = 0;                                                // reset device address
                ENDPT0 = EP_HSHK;                                        // disable transmission and reception
                ENDPT1 = 0; ENDPT2 = 0; ENDPT2 = 0; ENDPT4 = 0; ENDPT5 = 0; ENDPT6 = 0; ENDPT7 = 0; ENDPT8 = 0; ENDPT9 = 0; ENDPT10 = 0;
                ENDPT11 = 0; ENDPT2 = 0; ENDPT13 = 0; ENDPT14 = 0; ENDPT15 = 0;// disable all other endpoints
                while (ENDPT0 & EP_TX_ENABLE) {}                         // wait for tx disable to complete
                                                                         // reconfigure all packet buffers of the control endpoint
                ptrBDT->usb_bd_rx_odd.ulUSB_BDControl  = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS | OWN);  // set control endpoint rx size and configure Data Toggle Synchronisation (receive DATA 0 frames)
                ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1); // set control endpoint rx size and configure Data Toggle Synchronisation (Receive DATA 1 frames)
                fnUSB_handle_frame(USB_RESET_DETECTED, 0, 0, &usb_hardware);// generic handler routine
                usb_hardware.ucUSBAddress = 0;                           // reset the address to revert back to the default state
                ENDPT0 = (EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);        // enable control endpoint reception and transmission
            }
            if (ucUSB_Int_status & USB_ERROR) {                          // error detected
                if (ERR_STAT & CRC16) {                                  // {181} CRC-16 error
                    USB_errors.ulUSB_errors_CRC16++;            		 // count errors
            	}
                if (ERR_STAT & CRC5_EOF) {                               // CRC-5 error
                    USB_errors.ulUSB_errors_CRC5++;            		     // count errors
            	}
            	ERR_STAT = (CRC16 | CRC5_EOF);                           // clear error sources
                INT_STAT = USB_ERROR;                                    // reset flag
            }
            if (ucUSB_Int_status & SLEEP) {                              // suspend state detected - 3ms of idle USB bus detected (low speed D+ = 0, D- = 1 / high speed D+ = 1, D- = 0)
                INT_STAT = (SLEEP | RESUME);                             // reset flag
                INT_ENB |= RESUME_EN;                                    // enable resume interrupt
                fnUSB_handle_frame(USB_SUSPEND_DETECTED, 0, 0, 0);       // generic handler routine
            }
            if (ucUSB_Int_status & RESUME) {                             // resume detected - 10ms inverted idle USB bus state (low speed D+ = 1, D- = 0 / high speed D+ = 0, D- = 1)
                INT_STAT = (SLEEP | RESUME);                             // reset flag
                INT_ENB &= ~RESUME_EN;                                   // disable resume interrupt
                fnUSB_handle_frame(USB_RESUME_DETECTED, 0, 0, 0);        // generic handler routine
            }
            iInterruptLevel = 0;
            INT_STAT = (unsigned char)(ucUSB_Int_status & ~TOK_DNE);     // reset all other flags
    #if defined _WINDOWS
            INT_STAT &= (~(ucUSB_Int_status & ~TOK_DNE) | ATTACH);       // {69} - leave ATTACH status in the register
    #endif
        }
        if (ucUSB_Int_status & TOK_DNE) {                                // current processed token complete
    #if defined USB_HOST_SUPPORT                                         // {69}
            if (1) {                           // need mode?
                INT_STAT = TOK_DNE;                                      // clear the flag
                CTL &= ~TXSUSPEND_TOKENBUSY;
                // IN after a setup
#define TOKEN_RESULT  0x3c
#define TOKEN_ACKED   0x08
#define TOKEN_NACK    0x28
#define TOKEN_STALLED 0x38
                switch (ptUSB_BD->ulUSB_BDControl & TOKEN_RESULT) {
                case TOKEN_ACKED:                                        // setup has been acked
                    // receive data (IN after a setup)
                    // setup rx buffer
                    // setup rx control
                    // give ownership and wait for the next token to complete
                    break;
                case TOKEN_NACK:
                default:
                    break;
                }
            }
    #endif
            M5222X_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;               // start of BDT
            M5222X_USB_BD *ptUSB_BD;                                     // specific BD
            int iEndpoint_ref = (STAT >> END_POINT_SHIFT);               // the endpoint belonging to this event                    
            ptEndpointBD += iEndpoint_ref;                               // set to endpoint bd block as indicated by the status register
            if (STAT & TX_TRANSACTION) {                                 // determine the specific BD for the transaction
                usb_hardware.ptrEndpoint = &usb_endpoints[iEndpoint_ref];            
                if (!(usb_hardware.ptrEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER)) { // set a pointer to the buffer descriptor of the next transmission buffer to use
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;
                    usb_hardware.ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
                }
                else {
                    usb_hardware.ptr_ulUSB_BDControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
                    usb_hardware.ptrTxDatBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
                }
                iInterruptLevel = 1;                                     // ensure interrupts remain blocked when handling the next possible transmission
                fnUSB_handle_frame(USB_TX_ACKED, 0, iEndpoint_ref, &usb_hardware); // handle ack event
                iInterruptLevel = 0;
                ADDR = usb_hardware.ucUSBAddress;                        // program the address to be used 
            }
            else {                                                       // receive packet
                if (STAT & ODD_BANK) {                                   // check whether odd or even bank and update local flag
                    ptUSB_BD = &ptEndpointBD->usb_bd_rx_odd;             // received data in odd buffer
                    usb_hardware.ulRxControl = DATA_1;
                }
                else {
                    ptUSB_BD = &ptEndpointBD->usb_bd_rx_even;            // data in even buffer
                    usb_hardware.ulRxControl = 0;
                }
    #if defined USB_DEVICE_SUPPORT
                usb_hardware.ptrRxDatBuffer = &(ptUSB_BD->ptrUSB_BD_Data); // {192}
    #endif
                usb_hardware.usLength = GET_FRAME_LENGTH();              // the length of the present frame
                switch (ptUSB_BD->ulUSB_BDControl & RX_PID_MASK) {
                case (OUT_PID << RX_PID_SHIFT):                          // OUT frame - for any endpoint
                    {
                        unsigned long ulDataToggle;
                        if (usb_endpoints[iEndpoint_ref].ulEndpointSize & DTS) { // data toggling active (not isochronous)
                            ulDataToggle = (usb_endpoints[iEndpoint_ref].ulNextRxData0 & RX_DATA_TOGGLE); // the expected DATA
                            if ((ulDataToggle != 0) != ((ptUSB_BD->ulUSB_BDControl & DATA_1) != 0)) { // if the received is not synchron to the data toggling it represents repeated data since an ACK from us has been lost
    #if defined _SUPRESS_REPEAT_DATA
                                unsigned long ulNextRxData0_required = usb_endpoints[iEndpoint_ref].ulNextRxData0;
                                fnProcessInput(iEndpoint_ref, &usb_hardware, USB_DATA_REPEAT, ptUSB_BD, ptEndpointBD);
                                usb_endpoints[iEndpoint_ref].ulNextRxData0 = ulNextRxData0_required; // ensure we remain synchron with read stream
                                break;
    #endif
                            }
                        }
                        else {
                            ulDataToggle = RX_DATA_TOGGLE;
                        }

                        if (*fnGetEndPointCtr(iEndpoint_ref) & EP_CTL_DIS) {
                             fnProcessInput(iEndpoint_ref, &usb_hardware, USB_OUT_FRAME, ptUSB_BD, ptEndpointBD);  // non-control endpoint
                        }
                        else {
                             fnProcessInput(iEndpoint_ref, &usb_hardware, USB_CONTROL_OUT_FRAME, ptUSB_BD, ptEndpointBD); // control endpoint
                        }
                        if (ulDataToggle) {                              // synchronise the data toggle to detect repeated data
                            usb_endpoints[iEndpoint_ref].ulNextRxData0 &= ~(RX_DATA_TOGGLE);
                        }
                        else {
                            usb_endpoints[iEndpoint_ref].ulNextRxData0 |= RX_DATA_TOGGLE; 
                        }
                    }
                    break;

                case (SETUP_PID << RX_PID_SHIFT):                        // set up PID - on control endpoint 
                    ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = 0;    // disable all packet buffers
                    ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = 0;
                    if (!(usb_hardware.ulRxControl & DATA_1)) {          // synchronise the reception data buffers
                        ptrBDT->usb_bd_rx_even.ulUSB_BDControl = ((usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS))); // prepare buffer for next reception but don't free until consumed
                        ptrBDT->usb_bd_rx_odd.ulUSB_BDControl = (ptrBDT->usb_bd_rx_even.ulUSB_BDControl | DATA_1 | OWN); // free alternate buffer ready for next reception
                    }
                    else {
                        ptrBDT->usb_bd_rx_odd.ulUSB_BDControl = ((usb_endpoints[iEndpoint_ref].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS))); // prepare buffer for next reception but don't free until consumed
                        ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1 | OWN); // free alternate buffer ready for next reception
                    }
                    usb_endpoints[iEndpoint_ref].ulNextTxData0 = (DATA_1 | DTS); // always a data1 packet following a setup packet

                    CTL &= ~TXSUSPEND_TOKENBUSY;                         // allow SIE to continue token processing

                    if (fnProcessInput(iEndpoint_ref, &usb_hardware, USB_SETUP_FRAME, ptUSB_BD, ptEndpointBD) != MAINTAIN_OWNERSHIP) {
                        usb_endpoints[iEndpoint_ref].ulNextRxData0 |= (RX_DATA_TOGGLE);
                    }
                    break;

                default:                                                 // if anything else is received it indicates an error so stall
                    ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl = (OWN | BDT_STALL); // stall the endpoint
                    ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl  = (OWN | BDT_STALL);
                    fnSetUSBEndpointState(iEndpoint_ref, USB_ENDPOINT_STALLED);
                    break;
                }
            }
            INT_STAT = TOK_DNE;                                          // clear the flag (don't clear until processing has completed to ensure that the STAT register stays valid)
    #if defined _WINDOWS
            INT_STAT &= ~(TOK_DNE);
    #endif
        }
    }
}

// Set the transmit control and buffer to next active one and check whether it is free
//
extern int fnGetUSB_HW(int iEndpoint, USB_HW **ptr_usb_hardware)         // {187}
{
    M5222X_USB_ENDPOINT_BD *ptEndpointBD;
    if ((*fnGetEndPointCtr(iEndpoint) & EP_TX_ENABLE) == 0) {            // if the endpoint is not enabled for transmission
        return ENDPOINT_NOT_ACTIVE;
    }
    else {
        USB_END_POINT *ptrThisEndpoint;
        unsigned long *ptrThisControl;
        unsigned char **ptrThisBuffer;
        ptEndpointBD = ptrBDT;                                           // start of BDT
        ptEndpointBD += iEndpoint;                                       // move to the buffer descriptor for this endpoint
        ptrThisEndpoint = &usb_endpoints[iEndpoint];
        (*ptr_usb_hardware)->ptrEndpoint = ptrThisEndpoint;              // this is modified even if the endpoint is not free
        uDisable_Interrupt();                                            // protect from interrupts
            if ((ptrThisEndpoint->ulEndpointSize & ALTERNATE_TX_BUFFER) == 0) { // the transmit buffer to be used
                ptrThisControl = &ptEndpointBD->usb_bd_tx_even.ulUSB_BDControl;
                ptrThisBuffer = &ptEndpointBD->usb_bd_tx_even.ptrUSB_BD_Data;
            }
            else {
                ptrThisControl = &ptEndpointBD->usb_bd_tx_odd.ulUSB_BDControl;
                ptrThisBuffer = &ptEndpointBD->usb_bd_tx_odd.ptrUSB_BD_Data;
            }
            if ((*ptrThisControl & OWN) != 0) {                          // check whether the USB controller owns the transmit buffer descriptor
                uEnable_Interrupt();
                return ENDPOINT_NOT_FREE;                                // endpoint is presently busy so no data may be copied
            }    
            (*ptr_usb_hardware)->ptr_ulUSB_BDControl = ptrThisControl;   // update the hardware pointers (only when the endpoint is free)
            (*ptr_usb_hardware)->ptrTxDatBuffer = ptrThisBuffer;
            #if defined USB_HOST_SUPPORT
            (*ptr_usb_hardware)->ucModeType = usb_hardware.ucModeType;
            #endif
        uEnable_Interrupt();
    }
    return ENDPOINT_FREE;
}

// The hardware interface used to activate USB endpoints
//
extern void fnActivateHWEndpoint(unsigned char ucEndpointType, unsigned char ucEndpointRef, unsigned short usEndpointLength, unsigned short usMaxEndpointLength, USB_ENDPOINT *ptrEndpoint)
{
    M5222X_USB_ENDPOINT_BD *ptrEndpointBDT = ptrBDT;
    unsigned long  ulControlContent = (DTS | DATA_1);                    // prepare Data Toggle Synchronisation as default
    unsigned char *ptrEndPointCtr = fnGetEndPointCtr(ucEndpointRef);
    unsigned char  endpoint_config;
    ptrEndpointBDT += ucEndpointRef;
        #if defined USB_SIMPLEX_ENDPOINTS                                // {188}
    if (ENDPOINT_DISABLE == ucEndpointType) {                            // if this endpoint is to be disabled
        *ptrEndPointCtr = 0;                                             // disable endpoint transmitter and receiver
        while (*ptrEndPointCtr & EP_TX_ENABLE) {}                        // wait for tx disable to complete
        ptrEndpointBDT->usb_bd_tx_odd.ulUSB_BDControl = 0;               // reset endpoint buffer descriptor control entries
        ptrEndpointBDT->usb_bd_tx_even.ulUSB_BDControl = 0;
        ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = 0;
        ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = 0;
        usb_endpoints[ucEndpointRef].ulEndpointSize = 0;
        usb_endpoints[ucEndpointRef].ulNextRxData0 = 0;
        INT_ENB &= ~(SLEEP_EN | RESUME_EN);                              // disable suspend and resume interrupts when not configured
        *ptrEndPointCtr = 0;
        return;                                                          // disabled so return
    }
    INT_ENB |= SLEEP_EN;                                                 // since we have just been configured, enable suspend interrupt
    endpoint_config = *ptrEndPointCtr;                                   // get the original configuration setting

    if ((ucEndpointType & ~IN_ENDPOINT) == ENDPOINT_ISOCHRONOUS) {       // is this an isochronous endpoint?
        endpoint_config = (RETRY_DIS | EP_TX_ENABLE);                    // no retries on isochronous endpoint
        ulControlContent = 0;                                            // disable data toggle synchronisation on receiver
        usb_endpoints[ucEndpointRef].ulNextTxData0 = 0;                  // disable data toggle synchronisation on transmitter
    }
    else {
        usb_endpoints[ucEndpointRef].ulNextTxData0 = DTS;                // set data toggle synchronisation on transmission
        endpoint_config |= (EP_HSHK);                                    // bulk/control/interrupt endpoint - handshake enabled
    }
        #if defined USB_HOST_SUPPORT
    if (usb_hardware.ucModeType & USB_HOST_MODE) {
        ucEndpointType ^= IN_ENDPOINT;                                   // in host mode reverse the direction of the trasnmission/reception
    }
        #endif
    if ((ucEndpointType & IN_ENDPOINT) == 0) {                           // OUT type endpoint
        if (ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data == 0) {        // if the endpoint is being used for the first time
            if (usMaxEndpointLength > usEndpointLength) {                // if no larger specified take the specified value
                usEndpointLength = usMaxEndpointLength;
            }
            ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data = (unsigned char *)USB_FS_MALLOC(usEndpointLength); // alloc buffer memory for control endpoint
            ptrEndpointBDT->usb_bd_rx_odd.ptrUSB_BD_Data  = (unsigned char *)USB_FS_MALLOC(usEndpointLength);
        }
        endpoint_config |= EP_RX_ENABLE;                                 // enable reception on this endpoint
        ulControlContent |= SET_FRAME_LENGTH(usEndpointLength);          // set endpoint rx size                                     
        ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = (ulControlContent | OWN); // odd buffer for DATA1 frames
        ulControlContent &= ~DATA_1;                                     // even buffer for DATA 0 frames
        ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = (ulControlContent | OWN); // allow endpoints to receive
        usb_endpoints[ucEndpointRef].ulEndpointSize = ulControlContent;
    }
    else {
        endpoint_config |= EP_TX_ENABLE;                                 // enable transmission on IN endpoint
    }
    if ((ucEndpointType & ~(IN_ENDPOINT)) != ENDPOINT_CONTROL) {         // non-control endpoint
        endpoint_config |= EP_CTL_DIS;                                   // not control endpoint
    }
        #else
    *ptrEndPointCtr &= ~(EP_TX_ENABLE | EP_RX_ENABLE);                   // disable endpoint transmitter and receiver
    while (*ptrEndPointCtr & EP_TX_ENABLE) {}                            // wait for tx disable to complete
    ptrEndpointBDT->usb_bd_tx_odd.ulUSB_BDControl = 0;
    ptrEndpointBDT->usb_bd_tx_even.ulUSB_BDControl = 0;
    ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl = 0;
    ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = 0;
    usb_endpoints[ucEndpointRef].ulEndpointSize = 0;
    usb_endpoints[ucEndpointRef].ulNextRxData0 = 0;

    if (ENDPOINT_DISABLE == ucEndpointType) {
        INT_ENB &= ~(SLEEP_EN | RESUME_EN);                              // disable suspend and resume interrupts when not configured
        *ptrEndPointCtr = 0;
        return;
    }
    INT_ENB |= SLEEP_EN;                                                 // since we have just been configured, enable suspend interrupt

    if ((ucEndpointType & ~IN_ENDPOINT) == ENDPOINT_ISOCHRONOUS) {       // is this an isochronous endpoint?
        endpoint_config = (RETRY_DIS | EP_TX_ENABLE);                    // no retries on isochronous endpoint
        ulControlContent = 0;                                            // disable data toggle synchronisation on receiver
        usb_endpoints[ucEndpointRef].ulNextTxData0 = 0;                  // disable data toggle synchronisation on transmitter
    }
    else {
        usb_endpoints[ucEndpointRef].ulNextTxData0 = DTS;                // set data toggle synchronisation on transmission
        endpoint_config = (EP_HSHK | EP_TX_ENABLE);                      // bulk/control/interrupt endpoint - handshake enabled
    }
    if (!(ucEndpointType & IN_ENDPOINT)) {                               // output type endpoint
        if (ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data == 0) {        // if the end point is being used for the first time
            if (usMaxEndpointLength > usEndpointLength) {                // if no larger specified take the specified value
                usEndpointLength = usMaxEndpointLength;
            }
            ptrEndpointBDT->usb_bd_rx_even.ptrUSB_BD_Data = (unsigned char *)fnLE_add((CAST_POINTER_ARITHMETIC)uMalloc((MAX_MALLOC)(usEndpointLength))); // alloc buffer memory for control endpoint
            ptrEndpointBDT->usb_bd_rx_odd.ptrUSB_BD_Data  = (unsigned char *)fnLE_add((CAST_POINTER_ARITHMETIC)uMalloc((MAX_MALLOC)(usEndpointLength)));
        }
        endpoint_config |= EP_RX_ENABLE;                                 // enable reception on this endpoint
        ulControlContent |= SET_FRAME_LENGTH(usEndpointLength);          // set endpoint rx size                                     
        ptrEndpointBDT->usb_bd_rx_odd.ulUSB_BDControl  = (ulControlContent | OWN); // odd buffer for DATA1 frames
        ulControlContent &= ~DATA_1;                                     // even buffer for DATA 0 frames
        ptrEndpointBDT->usb_bd_rx_even.ulUSB_BDControl = (ulControlContent | OWN); // allow endpoints to receive
        usb_endpoints[ucEndpointRef].ulEndpointSize = ulControlContent;
    }
    if ((ucEndpointType & ~IN_ENDPOINT) != ENDPOINT_CONTROL) {           // non-control endpoint
        endpoint_config |= EP_CTL_DIS;                                   // not control endpoint
        if (!(ucEndpointType & IN_ENDPOINT)) {                           // out type
            endpoint_config &= ~EP_TX_ENABLE;                            // disable transmission
        }
    }
        #endif
    *ptrEndPointCtr = endpoint_config;                                   // set the endpoint mode
    _SIM_USB(USB_SIM_ENUMERATED,0,0);                                    // inform the simulator that USB enumeration has completed
}

// This routine is called with interrupts blocked for deferred read of a reception buffer
//
extern int fnConsumeUSB_out(unsigned char ucEndpointRef)
{
    unsigned long ulBuffer;
    M5222X_USB_BD *ptUSB_BD;                                             // specific BD
    M5222X_USB_ENDPOINT_BD *ptEndpointBD = ptrBDT;                       // start of BDT
    unsigned short usLength;
    ptEndpointBD += ucEndpointRef;
    if (usb_endpoints[ucEndpointRef].ulNextRxData0 & DATA_1) {
        ptUSB_BD = &ptEndpointBD->usb_bd_rx_even;
        ulBuffer = 0;
    }
    else {
        ptUSB_BD = &ptEndpointBD->usb_bd_rx_odd;
        ulBuffer = DATA_1;
    }
    if (ptUSB_BD->ulUSB_BDControl & OWN) {                               // no data available
        return USB_BUFFER_NO_DATA;
    }
    usLength = GET_FRAME_LENGTH();                                       // the number of bytes waiting in the buffer
    if (fnEndpointData(ucEndpointRef, (unsigned char *)fnLE_add((CAST_POINTER_ARITHMETIC)ptUSB_BD->ptrUSB_BD_Data), usLength, OUT_DATA_RECEPTION, 0) != MAINTAIN_OWNERSHIP) {
        ulBuffer |= (usb_endpoints[ucEndpointRef].ulEndpointSize & (USB_BYTE_CNT_MASK | DTS));// reception buffer size
        ptUSB_BD->ulUSB_BDControl = (ulBuffer | OWN);                    // re-enable reception on this endpoint
        usb_endpoints[ucEndpointRef].ulNextRxData0 &= (RX_DATA_TOGGLE);  // don't disturb this flag
        usb_endpoints[ucEndpointRef].ulNextRxData0 |= ulBuffer;          // last buffer processed
        return USB_BUFFER_FREED;                                         // buffer consumed and freed
    }
    return USB_BUFFER_BLOCKED;
}

// USB Configuration
//
extern void fnConfigUSB(QUEUE_HANDLE Channel, USBTABLE *pars)
{
#if defined USB_BDT_FIXED                                                // {87}
    #if defined _WINDOWS
    static M5222X_USB_ENDPOINT_BD USB_BDT_RAM[NUMBER_OF_USB_ENDPOINTS * 2];
    static M5222X_USB_ENDPOINT_BD *__USB_BDT_RAM = USB_BDT_RAM;
    #else
    extern M5222X_USB_ENDPOINT_BD __USB_BDT_RAM[];
    #endif
#endif
    unsigned char ucEndpoints = (pars->ucEndPoints + 1);                 // {86} endpoint count, including endpoint 0
    #if defined USE_USB_OTG_CHARGE_PUMP
    QUEUE_HANDLE iic_handle;
    #endif
    POWER_UP(POWER_USB_OTG);                                             // power up the USB controller module
    switch (pars->ucClockSource) {                                       // set USB clock source
    case SPECIAL_USB_CLOCK:                                              // use alternative clock input pin
        PNQPAR |= (ALT_2_FUNCTION_Q << (2 * 1));                         // set function to NQ1 pin
        USB_CTRL = USB_ALT_CLOCK;                                        // select the input
        break;
    case INTERNAL_USB_CLOCK:
        USB_CTRL = USB_SYSTEM_CLOCK;                                     // driven directly by system clock (which should be 48MHz). This is not recommended due to PLL jitter!!
        break;
    case EXTERNAL_USB_CLOCK:
    default:
        USB_CTRL = USB_OSCILLATOR_CLOCK;                                 // take clock from external 48MHz crystal
        break;
    }

    if (ucEndpoints > NUMBER_OF_USB_ENDPOINTS) {                         // {86} limit endpoint count
        ucEndpoints = NUMBER_OF_USB_ENDPOINTS;                           // limit to maximum available in device
    }

    usb_endpoints = uMalloc((MAX_MALLOC)(sizeof(USB_END_POINT) * ucEndpoints)); // get endpoint control structures

    CTL = ODD_RST;                                                       // reset all odd BDTs which specifies the even bank

#if defined USB_BDT_FIXED
    #if defined _WINDOWS                                                 // {87} set buffer to 512 byte aligned boundary
    __USB_BDT_RAM = (M5222X_USB_ENDPOINT_BD *)(((CAST_POINTER_ARITHMETIC)__USB_BDT_RAM) & 0xffffff00);
    if (__USB_BDT_RAM < USB_BDT_RAM)  {
        __USB_BDT_RAM = (M5222X_USB_ENDPOINT_BD *)(((CAST_POINTER_ARITHMETIC)(__USB_BDT_RAM + NUMBER_OF_USB_ENDPOINTS)) & 0xffffff00);
    }
    #endif
    ptrBDT = __USB_BDT_RAM;                                              // {87}
    uMemset(ptrBDT, 0x00, (ucEndpoints * ENDPOINT_BD_SIZE));
#else
    ptrBDT = uMallocAlign((MAX_MALLOC)(ucEndpoints * ENDPOINT_BD_SIZE), 512); // get BDT - buffer descriptor table memory (zeroed) - aligned to 512 byte boundars
#endif

    BDT_PAGE_01 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrBDT >> 8); // enter the BDT location
    BDT_PAGE_02 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrBDT >> 16);
    BDT_PAGE_03 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrBDT >> 24);

    ADDR = 0;                                                            // set default address
    CTL = 0;                                                             // reset complete
                                                                         
    ENDPT0 = 0;                                                          // configure endpoint 0 - the only one used during enumeration: USB then in default state
    while (ENDPT0 & EP_TX_ENABLE) {}                                     // wait for tx disable to complete
                                                                         // configure rx side of endpoint
    ptrBDT->usb_bd_rx_even.ptrUSB_BD_Data  = (unsigned char *)fnLE_add((CAST_POINTER_ARITHMETIC)uMalloc(ENDPOINT_0_SIZE)); // alloc buffer memory for control endpoint
    ptrBDT->usb_bd_rx_odd.ptrUSB_BD_Data   = (unsigned char *)fnLE_add((CAST_POINTER_ARITHMETIC)uMalloc(ENDPOINT_0_SIZE));
    ptrBDT->usb_bd_rx_odd.ulUSB_BDControl  = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS | OWN);  // set endpoint rx size and configure Data Toggle Synchronisation (receive DATA 0 frames)
    ptrBDT->usb_bd_rx_even.ulUSB_BDControl = (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl | DATA_1); // set endpoint rx size and configure Data Toggle Synchronisation (receive DATA 1 frames)
    usb_endpoints[0].ulEndpointSize = (SET_FRAME_LENGTH(ENDPOINT_0_SIZE) | DTS);
    ENDPT0 = (EP_HSHK | EP_RX_ENABLE | EP_TX_ENABLE);                    // enable control endpoint transmission and reception

    OTG_INT_STAT = (A_VBUS_CHG | BSESS_CHG | SESS_VLD_CHG | LINE_STAT_CHG | MSEC_1 | ID_CHG); // reset any pending OTG interrupts
    INT_STAT = (STALL | ATTACH | RESUME | SLEEP | TOK_DNE | SOF_TOK | USB_ERROR | USB_RST);   // reset any pending USB interrupts
    ERR_STAT = (PID_ERR | CRC5_EOF | CRC16 | DFN8 | BTO_ERR | DMA_ERR | BTS_ERR); // reset any pending USB error interrupts
    #if defined _WINDOWS
    INT_STAT = ERR_STAT = 0;
    #endif

    IC_ICR_0_53 = USB_OTG_INTERRUPT_PRIORITY;                            // define level and priority for this interrupt
    fnSetIntHandler(USB_OTG_VECTOR, (unsigned char *)_usb_otg_isr);      // enter the interrupt handler
    IC_IMRH_0 &= ~(USB_OTG_INT_H);                                       // unmask interrupt source
    IC_IMRL_0 &= ~(MASK_ALL_INT);                                        // clear global mask

    #if defined USB_HOST_SUPPORT                                         // {69}
    if (pars->usConfig & USB_HOST_MODE) {
        CTL = HOST_MODE_EN;                                              // enable host mode operation
        USB_CTRL |= PDE;
        OTG_CTRL = (DP_LOW_DM_LOW | OTG_EN);                             // pull down D+ and D- with weak pulldowns
        PULL_DOWN_15K();                                                 // pull down with 15k resistors
        SOF_THLD = 74;    // temp worst case threshold value for 64 bytes
        INT_ENB = ATTACH_EN;                                             // enable an interrupt on the attach of a device
    }
    else {
        INT_ENB = (STALL_EN | USB_ERROR_EN | TOK_DNE_EN | USB_RST_EN);// enable USB interrupts
        ERR_ENB = (CRC5_EOF_EN | CRC16_EN);                              // enable some error sources, for statistic use
        CTL = USB_EN_SOF_EN;                                             // enable USB module device operation
        if (pars->usConfig & USB_FULL_SPEED) {
            OTG_CTRL = (DP_HIGH | OTG_EN);                               // enable pull up on D+ for full speed operation            
        }
        else {
          //OTG_CTRL = (DM_HIGH | OTG_EN);                               // enable pull up on D- for low speed operation (this doesn't seem possible)
        }
    }
    #else
    INT_ENB = (STALL_EN | USB_ERROR_EN | TOK_DNE_EN | USB_RST_EN);   // enable USB interrupts
    ERR_ENB = (CRC5_EOF_EN | CRC16_EN);                                  // enable some error sources, for statistic use

    CTL = USB_EN_SOF_EN;                                                 // enable USB module operation
    if (pars->usConfig & USB_FULL_SPEED) {
        OTG_CTRL = (DP_HIGH | OTG_EN);                                   // enable pull up on D+ for full speed operation
    }
    else {
      //OTG_CTRL = (DM_HIGH | OTG_EN);                                   // enable pull up on D- for low speed operation (this doesn't seem possible)
    }
    #endif

    #if defined USE_USB_OTG_CHARGE_PUMP
    if (pars->OTG_I2C_Channel == 0) {                                    // if no I2C interface opened, open it now
        I2CTABLE tI2CParameters;

	    tI2CParameters.Channel = CHARGE_PUMP_I2C_CHANNEL;
	    tI2CParameters.usSpeed = 100;                                    // 100k
        tI2CParameters.Rx_tx_sizes.TxQueueSize = 10;
        tI2CParameters.Rx_tx_sizes.RxQueueSize = 0;
	    tI2CParameters.Task_to_wake = 0;                                 // no wake on transmission

        iic_handle = fnOpen(TYPE_I2C, FOR_I_O, &tI2CParameters);         // open the channel with defined configurations
    }
    else {
        iic_handle = pars->OTG_I2C_Channel;                              // share already available I2C interface
    }
    if (iic_handle != 0) {
        #define CONTROL_REGISTER_1  0x10
        #define CONTROL_REGISTER_2  0x11
        #define  BDISC_ACONN        0x04
        #define  DP_PULLUP          0x10
        #define  DM_PULLUP          0x20
        const static unsigned char ucEnableDevice[] = {OTG_CHARGE_PUMP_I2C_ADD, CONTROL_REGISTER_2, 0x00}; // this assumes the MAX3353 
        unsigned char ucFS_Device[]    = {OTG_CHARGE_PUMP_I2C_ADD, CONTROL_REGISTER_1, (BDISC_ACONN | DP_PULLUP)};
        fnWrite(iic_handle, (unsigned char *)ucEnableDevice, sizeof(ucEnableDevice)); // turn on device
        if (!(pars->usConfig & USB_FULL_SPEED)) {
            ucFS_Device[2] = (BDISC_ACONN | DM_PULLUP);                  // low speed mode
        }
        fnWrite(iic_handle, ucFS_Device, sizeof(ucFS_Device));           // configure for device mode
    }
    #endif
}
#endif                                                                   // end USB_INTERFACE

#if defined SUPPORT_EPORT
// IRQ 1..15 requests
//
    #if !defined CHIP_80_PIN && !defined _M5222X && !defined _M521XX && !defined _M5221X // {8}                                                    
        #define EDGE_INTERRUPTS 15                                       // IRQ1..15
    #else
        #if !defined _M5222X && !defined _M5221X && !defined _M521XX
        #define EDGE_INTERRUPTS 8                                        // IRQ1..7 + IRQ11
        #else
        #define EDGE_INTERRUPTS 7                                        // IRQ1..7
        #endif
    #endif
static void ( *eport_handler[EDGE_INTERRUPTS] )( void ) = {0};

static void fnIRQ(int iIRQ)
{
    if (eport_handler[iIRQ]) {
        iInterruptLevel = 1;                                             // ensure interrupts remain blocked during subroutines
        (eport_handler[iIRQ])();                                         // call the handler, if available
        iInterruptLevel = 0;
    }
}

static __interrupt__ void _irq1_handler(void)
{
    EPFR0 = 0x02;                                                        // clear interrupt flag
    fnIRQ(0);                                                            // call the handler, if available
}

static __interrupt__ void _irq2_handler(void)
{  
    EPFR0 = 0x04;                                                        // clear interrupt flag
    fnIRQ(1);                                                            // call the handler, if available
}

static __interrupt__ void _irq3_handler(void)
{  
    EPFR0 = 0x08;                                                        // clear interrupt flag
    fnIRQ(2);                                                            // call the handler, if available
}

static __interrupt__ void _irq4_handler(void)
{
    EPFR0 = 0x10;                                                        // clear interrupt flag
    fnIRQ(3);                                                            // call the handler, if available   
}

static __interrupt__ void _irq5_handler(void)
{  
    EPFR0 = 0x20;                                                        // clear interrupt flag
    fnIRQ(4);                                                            // call the handler, if available
}

static __interrupt__ void _irq6_handler(void)
{  
    EPFR0 = 0x40;                                                        // clear interrupt flag
    fnIRQ(5);                                                            // call the handler, if available
}

// IRQ7 has dedicated NMI level and so should not call any driver routines which require protected regions.
// The flag iInterruptLevel is not modified since this routine can also not be interrupted and doesn't need to control the value
// due to the fact that it doesn't call any code with protected region entry/exit requirements (this avoids the risk of 
// disturbing lower priority interrupts that may be in a protected region and avoids related counter corruption)
//
static __interrupt__ void _irq7_handler(void)                            // {133}
{  
    EPFR0 = 0x80;                                                        // clear interrupt flag
    if (eport_handler[6]) {
        (eport_handler[6])();                                            // call the handler, if available
    }
}

    #if !defined CHIP_80_PIN && !defined _M5222X && !defined _M521XX && !defined _M5221X && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {8}{122}{127}{145}
    static __interrupt__ void _irq8_handler(void)
    {  
        EPFR1 = 0x01;                                                    // clear interrupt flag
        fnIRQ(7);                                                        // call the handler, if available
    }

    static __interrupt__ void _irq9_handler(void)
    {  
        EPFR1 = 0x02;                                                    // clear interrupt flag
        fnIRQ(8);                                                        // call the handler, if available
    }

    static __interrupt__ void _irq10_handler(void)
    {  
        EPFR1 = 0x04;                                                    // clear interrupt flag
        fnIRQ(9);                                                        // call the handler, if available
    }

    static __interrupt__ void _irq11_handler(void)
    {  
        EPFR1 = 0x08;                                                    // clear interrupt flag
        fnIRQ(10);                                                       // call the handler, if available
    }

    static __interrupt__ void _irq12_handler(void)
    {  
        EPFR1 = 0x10;                                                    // clear interrupt flag
        fnIRQ(11);                                                       // call the handler, if available
    }

    static __interrupt__ void _irq13_handler(void)
    {  
        EPFR1 = 0x20;                                                    // clear interrupt flag
        fnIRQ(12);                                                       // call the handler, if available
    }

    static __interrupt__ void _irq14_handler(void)
    {  
        EPFR1 = 0x40;                                                    // clear interrupt flag
        fnIRQ(13);                                                       // call the handler, if available
    }

    static __interrupt__ void _irq15_handler(void)
    {  
        EPFR1 = 0x80;                                                    // clear interrupt flag
        fnIRQ(14);                                                       // call the handler, if available
    }

    static const unsigned char *irq_int_handlers[EDGE_INTERRUPTS] = {
        (unsigned char *)_irq1_handler,
        (unsigned char *)_irq2_handler,
        (unsigned char *)_irq3_handler,
        (unsigned char *)_irq4_handler,
        (unsigned char *)_irq5_handler,
        (unsigned char *)_irq6_handler,
        (unsigned char *)_irq7_handler,
        (unsigned char *)_irq8_handler,
        (unsigned char *)_irq9_handler,
        (unsigned char *)_irq10_handler,
        (unsigned char *)_irq11_handler,
        (unsigned char *)_irq12_handler,
        (unsigned char *)_irq13_handler,
        (unsigned char *)_irq14_handler,
        (unsigned char *)_irq15_handler,
    };
    #else                                                                // 80 pin chip has only IRQ11 from second byte
        #if !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {122}{127}{145}
    static __interrupt__ void _irq11_handler(void)
    {  
        EPFR1 = 0x08;                                                    // clear interrupt flag
        fnIRQ(EDGE_INTERRUPTS-1);                                        // call the handler, if available
    }
        #endif

    static const unsigned char *irq_int_handlers[EDGE_INTERRUPTS] = {
        (unsigned char *)_irq1_handler,
        (unsigned char *)_irq2_handler,
        (unsigned char *)_irq3_handler,
        (unsigned char *)_irq4_handler,
        (unsigned char *)_irq5_handler,
        (unsigned char *)_irq6_handler,
        (unsigned char *)_irq7_handler,
        #if !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {122}{127}{145}
        (unsigned char *)_irq11_handler
        #endif
    };
    #endif                                                               // endif SUPPORT_EPORT
#endif


#if defined SUPPORT_ADC && !defined _M52XX_SDRAM && !defined _M520X      // {44}{122}{127}

// Interrupt handlers for zero crossing, high and low levels
//
static void ( *adc_int_zero_handlers[ADC_CHANNELS] )( ADC_INTERRUPT_RESULT* ) = {0};
static void ( *adc_int_level_handlers[ADC_CHANNELS * 2])( ADC_INTERRUPT_RESULT* ) = {0};
static void ( *adc_int_complete_handlerA)( ADC_INTERRUPT_RESULT* ) = 0;
static void ( *adc_int_complete_handlerB)( ADC_INTERRUPT_RESULT* ) = 0;

static unsigned short usSingleShotTrigger = 0;

#define ADC_CHAN_A_SINGLE_SHOT    0x4000
#define ADC_CHAN_B_SINGLE_SHOT    0x8000

static __interrupt__ void _ADC_level_interrupt(void)                     // interrupt due to specific analogue level change
{
    ADC_INTERRUPT_RESULT adc_result;
    do {
        unsigned short usCrossingMask = ZERO_CROSS_BOTH;
        unsigned short usBit = 0x0001;
        int iInterrupt = 0;
        while (usBit < 0x0100) {                                         // check all zero crossing interrupts
            if (ADC_ADZCSTAT == 0) {
                break;                                                   // no (more) zero crossing interrupts
            }
            if (ADC_ADZCSTAT & usBit) {
                ADC_ADZCSTAT = usBit;                                    // clear interrupt
    #if defined _WINDOWS
                ADC_ADZCSTAT &= ~usBit;
    #endif
                if (usSingleShotTrigger & usBit) {                       // if single short type, disable further for this channel
                    ADC_ADZCC &= ~usCrossingMask;
                    usSingleShotTrigger &= ~usBit;
                }
                if (adc_int_zero_handlers[iInterrupt] != 0) {
                    iInterruptLevel = 1;
                    adc_result.ucADC_channel = (unsigned char)iInterrupt; // channel causing interrupt
                    adc_result.sADC_value = *(ADC_ADRSLT_ADD + iInterrupt); // present sample value
                    adc_result.ucADC_flags = ADC_INT_ZERO_CROSSING;      // mark the interrupt cause
                    adc_int_zero_handlers[iInterrupt](&adc_result);      // call the interrupt handler
                    iInterruptLevel = 0;
                }
            }
            usBit <<= 1;
            usCrossingMask <<= 2;
            iInterrupt++;
        }

        usBit = 0x0001;
        iInterrupt = 0;
        while (usBit != 0) {                                             // check all 
            if (ADC_ADLSTAT == 0) {
                break;                                                   // no (more) level trigger interrupts
            }
            if (ADC_ADLSTAT & usBit) {
                ADC_ADLSTAT = usBit;                                     // clear interrupt
    #if defined _WINDOWS
                ADC_ADLSTAT &= ~usBit;
    #endif
                if (adc_int_level_handlers[iInterrupt] != 0) {
                    int iSource = iInterrupt;
                    if (iSource >= ADC_CHANNELS) {
                        *(ADC_ADLLR_ADD + iInterrupt) = 0x7ff8;          // disable high limit checking
                        iSource -= ADC_CHANNELS;
                        adc_result.ucADC_flags = ADC_INT_HIGH_LEVEL;     // mark the interrupt cause
                    }
                    else {
                        *(ADC_ADLLR_ADD + iInterrupt) = 0;               // disable low limit checking
                        adc_result.ucADC_flags = ADC_INT_LOW_LEVEL;      // mark the interrupt cause
                    }
                    iInterruptLevel = 1;
                    adc_result.ucADC_channel = (unsigned char)iSource;   // channel causing interrupt
                    adc_result.sADC_value = *(ADC_ADRSLT_ADD + iSource); // present sample value
                    adc_int_level_handlers[iInterrupt](&adc_result);     // call the interrupt handler
                    iInterruptLevel = 0;
                }
            }
            usBit <<= 1;
            iInterrupt++;
        }
    } while ((ADC_ADZCSTAT != 0) || (ADC_ADLSTAT != 0));                 // while level change interrupts present
}


// Conversion complete at ADC A module
//
static __interrupt__ void _ADC_convertA_interrupt(void) 
{
#if defined _WINDOWS
    ADC_ADSTAT &= ~(EOSI0); 
#else
    ADC_ADSTAT = EOSI0;
#endif
    if (usSingleShotTrigger & ADC_CHAN_A_SINGLE_SHOT) {                  // disable further interrupt in single shot mode
        ADC_CTRL1 &= ~EOSIE0;
        usSingleShotTrigger &= ~ADC_CHAN_A_SINGLE_SHOT;
    }
    iInterruptLevel = 1;
    adc_int_complete_handlerA(0);                                        // call the interrupt handler
    iInterruptLevel = 0;
}

// Conversion complete at ADC B module
//
static __interrupt__ void _ADC_convertB_interrupt(void) 
{
#if defined _WINDOWS
    ADC_ADSTAT &= ~(EOSI1);
#else
    ADC_ADSTAT = EOSI1;
#endif
    if (usSingleShotTrigger & ADC_CHAN_B_SINGLE_SHOT) {                  // disable further interrupt in single shot mode
        ADC_CTRL2 &= ~EOSIE1;
        usSingleShotTrigger &= ~ADC_CHAN_B_SINGLE_SHOT;
    }
    iInterruptLevel = 1;
    adc_int_complete_handlerB(0);                                        // call the interrupt handler
    iInterruptLevel = 0;
}

#endif

#if defined SUPPORT_PIT1
static void ( *pit1_interrupt_handler)(void) = 0;
static unsigned short usPITPrescaler;

static __interrupt__ void _PIT1_Interrupt(void)
{
    if (usPITPrescaler & PIT_EN) {                                       // if used in periodic mode
      //PIT_PCSR_1 = (unsigned short)(usPITPrescaler | PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE); // clear interrupt and continue running in periodic mode
        PIT_PCSR_NON_PRE_1 = (PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE | PIT_EN); // {177} clear interrupt and continue running in periodic mode without writing to pre-scaler (and resetting its value)
    }
    else {                                                               // one-shot mode
        PIT_PCSR_1 = (unsigned short)(PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE); // clear interrupt and stop if in single-shot mode
        POWER_DOWN(POWER_PIT1);                                          // automatically power down when in single shot mode
    }
    iInterruptLevel = 1;
    pit1_interrupt_handler();                                            // call handling function
    iInterruptLevel = 0;
}
#endif

#if defined SUPPORT_DMA_TIMER

#define DMA_TIMERS 4

static void ( *dma_timer_handler[DMA_TIMERS] )( void ) = {0};
static unsigned char dma_timer_mode[DMA_TIMERS];

static __interrupt__ void _dma_timer_int_0(void)
{
    if (dma_timer_mode[0] & ENABLE_DMA_TIMER) {                          // periodic mode - leave running
        DTER0 = (DMA_TIM_EVENT_REF | DMA_TIM_EVENT_CAP);                 // clear interrupt
    #if defined _WINDOWS
        DTER0 = 0;
    #endif
    }
    else {
        DTMR0 = 0;                                                       // stop timer
        POWER_DOWN(POWER_TMR0);                                          // power down module
    #if defined _WINDOWS                                                 // simulate timer reset
        DTER0  = 0;
        DTRR0  = 0xffffffff;
        DTCN0  = 0;
        DTCR0  = 0;
        DTXMR0 = 0;
    #endif
    }
    if (dma_timer_handler[0] != 0) {                                     // {190}
        iInterruptLevel = 1;
            dma_timer_handler[0]();
        iInterruptLevel = 0;
    }
}

static __interrupt__ void _dma_timer_int_1(void)
{
    if (dma_timer_mode[1] & ENABLE_DMA_TIMER) {                          // periodic mode - leave running
        DTER1 = (DMA_TIM_EVENT_REF | DMA_TIM_EVENT_CAP);                 // clear interrupt
    #if defined _WINDOWS
        DTER1 = 0;
    #endif
    }
    else {
        DTMR1 = 0;                                                       // stop timer
        POWER_DOWN(POWER_TMR1);                                          // power down module
    #if defined _WINDOWS                                                 // simulate timer reset
        DTER1  = 0;
        DTRR1  = 0xffffffff;
        DTCN1  = 0;
        DTCR1  = 0;
        DTXMR1 = 0;
    #endif
    }
    if (dma_timer_handler[1] != 0) {                                     // {190}
        iInterruptLevel = 1;
            dma_timer_handler[1]();
        iInterruptLevel = 0;
    }
}

static __interrupt__ void _dma_timer_int_2(void)
{
    if (dma_timer_mode[2] & ENABLE_DMA_TIMER) {                          // periodic mode - leave running
        DTER2 = (DMA_TIM_EVENT_REF | DMA_TIM_EVENT_CAP);                 // clear interrupt
    #if defined _WINDOWS
        DTER2 = 0;
    #endif
    }
    else {
        DTMR2 = 0;                                                       // stop timer
        POWER_DOWN(POWER_TMR2);                                          // power down module
    #if defined _WINDOWS                                                 // simulate timer reset
        DTER2  = 0;
        DTRR2  = 0xffffffff;
        DTCN2  = 0;
        DTCR2  = 0;
        DTXMR2 = 0;
    #endif
    }
    if (dma_timer_handler[2] != 0) {                                     // {190}
        iInterruptLevel = 1;
            dma_timer_handler[2]();
        iInterruptLevel = 0;
    }
}

static __interrupt__ void _dma_timer_int_3(void)
{
    if (dma_timer_mode[3] & ENABLE_DMA_TIMER) {                          // periodic mode - leave running
        DTER3 = (DMA_TIM_EVENT_REF | DMA_TIM_EVENT_CAP);                 // clear interrupt
    #if defined _WINDOWS
        DTER3 = 0;
    #endif
    }
    else {
        DTMR3 = 0;                                                       // stop timer
        POWER_DOWN(POWER_TMR3);                                          // power down module
    #if defined _WINDOWS                                                 // simulate timer reset
        DTER3  = 0;
        DTRR3  = 0xffffffff;
        DTCN3  = 0;
        DTCR3  = 0;
        DTXMR3 = 0;
    #endif
    }
    if (dma_timer_handler[3] != 0) {                                     // {190}
        iInterruptLevel = 1;
            dma_timer_handler[3]();
        iInterruptLevel = 0;
    }
}

static const unsigned char *_DMA_TIMER_Interrupt[DMA_TIMERS] = {
    (unsigned char *)_dma_timer_int_0,
    (unsigned char *)_dma_timer_int_1,
    (unsigned char *)_dma_timer_int_2,
    (unsigned char *)_dma_timer_int_3
}; 

#endif

#if defined SUPPORT_GENERAL_PURPOSE_TIMER && !defined _M520X             // {72}{127}
#define GPT_CHANNELS 4

typedef struct stGTP_CONTROL
{
  unsigned long   *capture_list;                                         // pointer to a capture list
  unsigned short  usCaptureTrigger;                                      // the number of capture values required
  unsigned short  usCaptureCount;                                        // the number of captures values already recorded
} GTP_CONTROL;

static void ( *GPT_count_interrupt_callback[GPT_CHANNELS] )( void ) = {0};
static GTP_CONTROL gpt[GPT_CHANNELS] = {{0}};
#if defined GPT_EXTENDED_COUNTER
    static unsigned short GPT_ExtendedCount = 0;                         // extended GPT counter
#endif

static void gp_timer_int(int iChannel, unsigned short usChannelCounter)
{
    if (gpt[iChannel].usCaptureTrigger) {                                // if we are handling captures
    #if defined GPT_EXTENDED_COUNTER
        *gpt[iChannel].capture_list++ = (unsigned long)((GPT_ExtendedCount << 16) | usChannelCounter); // save the capture time to the buffer
    #else
        *gpt[iChannel].capture_list++ = usChannelCounter;                // save the capture time to the buffer
    #endif
#if defined _WINDOWS
        if (GPTSCR1 & TFFCA) {                                           // if the fast clearing flag is set
            GPTFLG1 &=  ~(0x01 << iChannel);                             // clear the interrupt due to previous read
        }
#endif
        gpt[iChannel].usCaptureCount++;                                  // count the values captured
        if (gpt[iChannel].usCaptureCount >= gpt[iChannel].usCaptureTrigger) { // if all collected
            GPTIE &= ~(0x01 << iChannel);                                // disable further capture interrupts
            if (GPTIE == 0) {                                            // no more capture timers operating
                GPTSCR2 &= ~(TOI);                                       // disable also the overflow interrupt
                GPTSCR1 = 0;                                             // disable the timer and power down the module
                POWER_DOWN(POWER_GPT);
#if defined GPT_EXTENDED_COUNTER
                GPT_ExtendedCount = 0;                                   // {85}
#endif
            }
            iInterruptLevel = 1;
            GPT_count_interrupt_callback[iChannel]();                    // call the user handler
            iInterruptLevel = 0;
        }
    }
#if defined GPT_SIMPLE_INTERRUPT                                         // {94} 
    else {
    #if defined _WINDOWS
        if (GPTSCR1 & TFFCA) {                                           // if the fast clearing flag is set
            GPTFLG1 &=  ~(0x01 << iChannel);                             // clear the interrupt due to previous read
        }
    #endif
        iInterruptLevel = 1;
        GPT_count_interrupt_callback[iChannel]();                        // call the user handler
        iInterruptLevel = 0;
    }
#endif
}

static __interrupt__ void _gp_timer_int_0(void)
{
    gp_timer_int(0, GPTC0);                                              // clear interrupt and call handler
}

static __interrupt__ void _gp_timer_int_1(void)
{
    gp_timer_int(1, GPTC1);                                              // clear interrupt and call handler
}

static __interrupt__ void _gp_timer_int_2(void)
{
    gp_timer_int(2, GPTC2);                                              // clear interrupt and call handler
}

static __interrupt__ void _gp_timer_int_3(void)
{
    gp_timer_int(3, GPTC3);                                              // clear interrupt and call handler
}

static const unsigned char *_GPT_capture_interrupt[GPT_CHANNELS] = {
    (unsigned char *)_gp_timer_int_0,
    (unsigned char *)_gp_timer_int_1,
    (unsigned char *)_gp_timer_int_2,
    (unsigned char *)_gp_timer_int_3
};

    #if defined GPT_EXTENDED_COUNTER
static __interrupt__ void _GPT_overflow_interrupt(void)
{
    /*volatile unsigned short dummy = */GPTCNT;                          // clear the interrupt (no dummy copy to avoid compiler warnings)
    GPT_ExtendedCount++;                                                 // increment the extended count value
#if defined _WINDOWS
    GPTFLG2 = 0;
#endif
}
    #endif
#endif

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
    case QSPI_INTERRUPT:
#if defined _M520X                                                       // {127}
        IC_ICR_0_31 = ((INTERRUPT_SETUP *)ptrSettings)->int_priority;    // define level and priority for this interrupt
        fnSetIntHandler(QSPI_VECTOR, (unsigned char *)_QSPI_Interrupt);  // enter the handler routine
        IC_IMRL_0 &= ~(QSPI_PIF_INT_L);                                  // unmask interrupt source
#else
        IC_ICR_0_18 = ((INTERRUPT_SETUP *)ptrSettings)->int_priority;    // define level and priority for this interrupt
        fnSetIntHandler(QSPI_VECTOR, (unsigned char *)_QSPI_Interrupt);  // enter the handler routine
        IC_IMRL_0 &= ~(QSPI_PIF_INT_L | MASK_ALL_INT);                   // unmask interrupt source
#endif
        qspi_handler = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
        QIR = (QSPI_SPIF);                                               // clear pending interrupts
        QIR = (QSPI_SPIFE | QSPI_SPIF);                                  // enable an interrupt on SPI completion
        break;

#if defined SUPPORT_PIT1
    case PIT_INTERRUPT:
        {
            PIT_SETUP *PIT_settings = (PIT_SETUP *)ptrSettings;
            POWER_UP(POWER_PIT1);                                        // ensure the PIT1 module is clocked
            PIT_PCSR_1 = PIT_PIF;                                        // stop any possible activity and reset pending interrupts
            if (PIT_settings->mode & PIT_STOP) {                         // if the PIT is to be stopped the work has been done
                POWER_DOWN(POWER_PIT1);                                  // disable clocks to module since work is done
                return;
            }
            pit1_interrupt_handler = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
            if (pit1_interrupt_handler == 0) {                           // a handler must be supplied
                return;
            }
            usPITPrescaler = 0;
    #if defined _M520X                                                   // {127}
            IC_ICR_0_5 = ((INTERRUPT_SETUP *)ptrSettings)->int_priority; // define level and priority for this interrupt
            fnSetIntHandler(PIT1_VECTOR, (unsigned char *)_PIT1_Interrupt);// enter the handler routine
            IC_IMRL_0 &= ~(PIT_1_PIF_INT_L);                             // unmask interrupt source
    #else
            IC_ICR_0_56 = ((INTERRUPT_SETUP *)ptrSettings)->int_priority;// define level and priority for this interrupt
            fnSetIntHandler(PIT1_VECTOR, (unsigned char *)_PIT1_Interrupt);// enter the handler routine
            IC_IMRH_0 &= ~(PIT_1_PIF_INT_H);                             // unmask interrupt source
            IC_IMRL_0 &= ~(MASK_ALL_INT);                                // clear global mask
    #endif
            while (PIT_settings->count_delay > 0x10000) {                // scale prescaler to achieve best accuracy
                 usPITPrescaler += PIT_PRESCALE_2;
                 PIT_settings->count_delay /= 2;
            }
            PIT_PCSR_1 = (usPITPrescaler | PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD); // prepare for load
            PIT_PMR_1 = (unsigned short)(PIT_settings->count_delay - 1); // load interval value
            if (PIT_settings->mode & PIT_PERIODIC) {
                usPITPrescaler |= PIT_EN;                                // set periodic mode
            }
            PIT_PCSR_1 = (usPITPrescaler | PIT_DBG | PIT_OVW | PIT_PIF | PIT_RLD | PIT_PIE | PIT_EN); // start PIT with interrupt enabled
    #if defined _WINDOWS
            PIT_PCNTR_1 = PIT_PMR_1;
    #endif
        }
        break;
#endif
#if defined SUPPORT_EPORT
    case PORT_INTERRUPT:                                                 // 80 pin version has IRQ 1..7 + IRQ11
        {
            unsigned char ucIRQ_bit = (((INTERRUPT_SETUP *)ptrSettings)->int_port_bit);
            unsigned short usMask, usValue;
          //unsigned char *irq_ctl = (IRQ_ICR_1_7_START - 1);            // {22}
            unsigned short *EPPAR = EPPAR0_ADR;                          // assume eport 0
            if (ucIRQ_bit == 0) {                                        // no IRQ0 so ignore
                return;
            }
            POWER_UP(POWER_EPORT);                                       // {55} apply power to edge port module
            if (ucIRQ_bit >= 8) {                                        // Eport 1 {8}
    #if defined _M5222X || defined _M5221X || defined _M521XX || defined _M5225X || defined _M52XX_SDRAM || defined _M520X || defined _M523X // {122}{127}{145}
                return;                                                  // invalid IRQs on M5222X / M5221X / M521XX / M523X so ignore
    #else
                ucIRQ_bit -= 8;
                EPPAR = EPPAR1_ADR;                                      // change to eport 1
    #endif
            }
            usMask = (ALT_2_FUNCTION_Q << (2 * ucIRQ_bit));
            usValue = (PRIMARY_FUNCTION_Q << (2 * ucIRQ_bit));
            if (EPPAR0_ADR == EPPAR) {                                   // {8}
    #if defined _M521X                                                   // {95}
                if (ucIRQ_bit == 1) {                                    // special case IRQ1 pin has quad function
                    PNQPAR &= ~0x03;
                    PNQPAR |= 0x01;
                }
                else {
                    PNQPAR |= (0x01 << ucIRQ_bit);                       // other pins have dual function
                }
    #elif !defined _M52XX_SDRAM && !defined _M523X                       // {122}{145}
                PNQPAR &= ~usMask;                                       // set interrupt function on port NQ
                PNQPAR |= usValue;
    #endif
                EPDDR0 &= ~(0x01 << ucIRQ_bit);                          // ensure that the edge port is configured as an input
                EPPAR0 &= ~usMask;                                       // clear the interrupt type mask
            }
    #if !defined _M5222X && !defined _M5221X && !defined _M521XX && !defined _M5225X && !defined _M521X && !defined _M52XX_SDRAM && !defined _M523X // {65}{122}{145}
            else {                                  
                PGPPAR |= (0x01 << ucIRQ_bit);                           // set interrupt function on port GP
                EPDDR1 &= ~(0x01 << ucIRQ_bit);
                EPPAR1 &= ~usMask;
            }
    #endif
            switch (((INTERRUPT_SETUP *)ptrSettings)->int_port_sense) {
            case IRQ_LEVEL_SENSITIVE:                                    // nothing to set in this case
                break;
            case IRQ_FALLING_EDGE:
                *EPPAR |= (usValue << 1);
                break;
            case IRQ_RISING_EDGE:
                *EPPAR |= usValue;
                break;
            case IRQ_BOTH_EDGES:
                *EPPAR |= (usValue | (usValue << 1));
                break;
            }
            if (EPPAR0_ADR == EPPAR) {
                eport_handler[ucIRQ_bit - 1] = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
                fnSetIntHandler((IRQ1_VECTOR + (ucIRQ_bit - 1)), (unsigned char *)irq_int_handlers[ucIRQ_bit - 1]); // enter the handler routine
                EPFR0 = (0x01 << ucIRQ_bit);                             // clear pending interrupts
                EPIER0 |= (0x01 << ucIRQ_bit);                           // enable interrupts

              //irq_ctl += ucIRQ_bit;                                    // {21}
                IC_IMRL_0 &= ~((IRQ1_PIF_INT_L << (ucIRQ_bit - 1)) | MASK_ALL_INT); // unmask interrupt source
            }
    #if !defined _M5221X && !defined _M5222X && !defined _M521XX && !defined _M5225X && !defined _M521X && !defined _M52XX_SDRAM && !defined _M523X // {122}{145} M5222X / M5221X / M521XX / M521X / M528x / M523X have only IRQ1..IRQ7. M5225X only IRQ1,3,5 and 7
            else {                                                       // {8}
                unsigned char *irq_ctl;                                  // {21}
        #if defined CHIP_80_PIN                                          // can only be IRQ11
                eport_handler[EDGE_INTERRUPTS-1] = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
                fnSetIntHandler(IRQ11_VECTOR, (unsigned char *)irq_int_handlers[EDGE_INTERRUPTS-1]); // enter the handler routine
        #else
                eport_handler[ucIRQ_bit + 7] = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
                fnSetIntHandler((IRQ8_VECTOR + ucIRQ_bit), (unsigned char *)irq_int_handlers[ucIRQ_bit + 7]); // enter the handler routine
        #endif
                EPFR1 = (0x01 << ucIRQ_bit);                             // clear pending interrupts
                EPIER1 |= (0x01 << ucIRQ_bit);                           // enable interrupts
                irq_ctl = IRQ_ICR_8_15_START + ucIRQ_bit;                // {15}
                *irq_ctl = (((INTERRUPT_SETUP *)ptrSettings)->int_priority); // define level and priority for this interrupt {19}
                IC_IMRL_1 &= ~(MASK_ALL_INT);                            // ensure global mask removed {15}
                IC_IMRH_1 &= ~(IRQ8_PIF_INT_H << ucIRQ_bit);             // unmask interrupt source {15}
            }
    #endif
        }         
        break;
#endif
#if defined SUPPORT_DMA_TIMER
    case DMA_TIMER_INTERRUPT:
        {
            DMA_TIMER_SETUP *ptrDMA_timer_settings = (DMA_TIMER_SETUP *)ptrSettings;
            int iChannel = ptrDMA_timer_settings->channel;
            if ((iChannel >= DMA_TIMERS)
    #if defined GLOBAL_TIMER_TASK && defined GLOBAL_HARDWARE_TIMER
                || (iChannel == HW_TIMER_USES_DMA_TIMER)
    #endif
                ) {
                return;                                                  // invalid channel or conflict with global hardware timer
            }
            else {
                DMA_TIMER *dma_timer = (DMA_TIMER *)DMA_TIMER_0_ADD;
                unsigned short usConfiguration = BUS_CLOCK_DIRECT;
                POWER_UP((unsigned char)(POWER_TMR0 + iChannel));        // enable clocks to the module
                dma_timer += iChannel;                                   // set to register set belonging to this channel
                dma_timer->usDTMR = 0;                                   // ensure stopped and counter reset
    #if defined _WINDOWS                                                 // simulate timer reset
                dma_timer->vucDTER = 0;
                dma_timer->ulDTRR  = 0xffffffff;
                dma_timer->vulDTCN = 0;
                dma_timer->vulDTCR = 0;
                dma_timer->ucDTXMR = 0;
    #endif
                if (DMA_TIMER_STOP & ptrDMA_timer_settings->mode) {
                    POWER_DOWN((unsigned char)(POWER_TMR0 + iChannel));  // disable clocks to the module
                    return;
                }
                if (ptrDMA_timer_settings->mode & DMA_TIMER_EXTERNAL_CLOCK) {
    #if defined _M520X                                                   // {127}
                    PAR_TIMER &= ~(ALT_2_FUNCTION_Q << (iChannel * 2));  // mask first
                    PAR_TIMER |= (PRIMARY_FUNCTION_Q << (iChannel * 2));
    #else
                    PTCPAR &= ~(ALT_2_FUNCTION_Q << (iChannel * 2));     // mask first
                    PTCPAR |= (PRIMARY_FUNCTION_Q << (iChannel * 2));    // configure DTIN for clock input
    #endif
                    usConfiguration = CLOCK_DTIN;
                }
                else {
                    if (DMA_TIMER_INTERNAL_CLOCK_DIV_16 & ptrDMA_timer_settings->mode) {
                       usConfiguration = BUS_CLOCK_16;
                    }
                    if ((DMA_TIMER_TOGGLE_OUTPUT | DMA_TIMER_PULSE_OUTPUT) & ptrDMA_timer_settings->mode) {
    #if defined _M520X                                                   // {127}
                        PAR_TIMER &= ~(ALT_2_FUNCTION_Q << (iChannel * 2)); // mask first
                        PAR_TIMER |= (ALT_1_FUNCTION_Q << (iChannel * 2));
    #else
                        PTCPAR &= ~(ALT_2_FUNCTION_Q << (iChannel * 2)); // mask first
                        PTCPAR |= (ALT_1_FUNCTION_Q << (iChannel * 2));  // configure DTOUT
    #endif
                        if (DMA_TIMER_TOGGLE_OUTPUT & ptrDMA_timer_settings->mode) {
                            usConfiguration |= TOGGLE_OUTPUT;            // output toggle on match (or else an active low pulse)
                        }
                    }
                    else if ((DMA_TIMER_CAPTURE_FALLING | DMA_TIMER_CAPTURE_RISING) & ptrDMA_timer_settings->mode) {
    #if defined _M520X                                                   // {127}
                        PAR_TIMER &= ~(ALT_2_FUNCTION_Q << (iChannel * 2)); // mask first
                        PAR_TIMER |= (PRIMARY_FUNCTION_Q << (iChannel * 2));
    #else
                        PTCPAR &= ~(ALT_2_FUNCTION_Q << (iChannel * 2)); // mask first
                        PTCPAR |= (PRIMARY_FUNCTION_Q << (iChannel * 2));// configure DTIN for capture input
    #endif
                        if (DMA_TIMER_CAPTURE_RISING & ptrDMA_timer_settings->mode) {
                            usConfiguration |= CAPTURE_RISING;
                        }
                        if (DMA_TIMER_CAPTURE_FALLING & ptrDMA_timer_settings->mode) {
                            usConfiguration |= CAPTURE_FALLING;
                        }
                    }
                }
                if (DMA_TIMER_RESTART_ON_MATCH & ptrDMA_timer_settings->mode) {
                    usConfiguration |= DMA_TIM_RESTART;                  // reset count value on match
                }
                if (DMA_TIMER_ENABLE_CLOCK_DIVIDER & ptrDMA_timer_settings->mode) {
                    usConfiguration |= ((ptrDMA_timer_settings->clock_divider - 1) << PRESCALER_SHIFT); // {165}
                }
                if ((DMA_TIMER_CAPTURE_INTERRUPT | DMA_TIMER_SINGLE_CAPTURE_INTERRUPT | DMA_TIMER_PERIODIC_INTERRUPT | DMA_TIMER_SINGLE_SHOT_INTERRUPT | DMA_TIMER_PERIODIC_DMA | DMA_TIMER_SINGLE_SHOT_DMA) & ptrDMA_timer_settings->mode) {
                    if (!((DMA_TIMER_CAPTURE_INTERRUPT | DMA_TIMER_SINGLE_CAPTURE_INTERRUPT) & ptrDMA_timer_settings->mode)) { // {164} if capture interrupt no reference match is enabled
                        usConfiguration |= ORRI;                             // enable interrupt/DMA on reference match
                    }
                    if ((DMA_TIMER_PERIODIC_DMA | DMA_TIMER_SINGLE_SHOT_DMA) & ptrDMA_timer_settings->mode) {
                        dma_timer->ucDTXMR = (DMA_TIMER_DMAEN | DMA_TIMER_HALT);
                    }
                    else {
                        if ((DMA_TIMER_SINGLE_SHOT_INTERRUPT | DMA_TIMER_SINGLE_CAPTURE_INTERRUPT) & ptrDMA_timer_settings->mode) { // {164}
                            dma_timer_mode[iChannel] = 0;
                        }
                        else {
                            dma_timer_mode[iChannel] = ENABLE_DMA_TIMER; // don't stop on match or capture event
                        }
                        dma_timer->ucDTXMR = (DMA_TIMER_HALT);
                        *(IC_DTIM_0_ADD + iChannel) = ptrDMA_timer_settings->int_priority; // define level and priority for this interrupt
                        fnSetIntHandler((DTIM0_VECTOR + iChannel), (unsigned char *)_DMA_TIMER_Interrupt[iChannel]);// enter the handler routine
                      //if (ptrDMA_timer_settings->int_handler == 0) {   // a handler must be supplied {190}
                      //    return;
                      //}
                        dma_timer_handler[iChannel] = ptrDMA_timer_settings->int_handler;
    #if defined _M520X                                                   // {127}
                        IC_IMRH_0 &= ~(DTIM0_PIF_INT_H << iChannel);     // unmask interrupt source
                        IC_IMRL_0 &= ~(MASK_ALL_INT);                    // and clear global mask
    #else
                        IC_IMRL_0 &= ~((DTIM0_PIF_INT_L << iChannel) | MASK_ALL_INT); // unmask interrupt source and clear global mask
    #endif
                    }
                }
                dma_timer->ulDTRR = (ptrDMA_timer_settings->count_delay - 1); // set reference value
                dma_timer->usDTMR = (usConfiguration | ENABLE_DMA_TIMER);// configure and start
            }
        }
        break;
#endif
#if defined SUPPORT_GENERAL_PURPOSE_TIMER && !defined _M520X && !defined _M523X // {72}{127}{140}
    case GPT_TIMER_INTERRUPT:
        {
            static unsigned char gpios_backup = 0;                       // we keep a backup of this register since it cannot be read
            GPTIMER_SETUP *ptrGPT_settings = (GPTIMER_SETUP *)ptrSettings;
            unsigned char ucChannelBit = (0x01 << ptrGPT_settings->channel);
            if (ptrGPT_settings->channel > 3) {
                return;                                                  // invalid channel
            }
            POWER_UP(POWER_GPT);                                         // power up the GTP module
            if (!(ptrGPT_settings->mode & GPT_NO_CLOCK_CONFIGURATION)) {
                GPTSCR2 = ((GPTSCR2 & ~GPT_PRESCALE_128) | (unsigned short)(ptrGPT_settings->mode & GPT_PRESCALE_128)); // set the prescaler value - note that this will only take place when the counter rolls over to 0x0000 (if already running)
            }
            if (ptrGPT_settings->mode & (GPT_CAPTURE_FALLING_EDGE | GPT_CAPTURE_RISING_EDGE)) { // a capture function is required
                unsigned char ucEdgeValue = 0;
                unsigned char ucChannelMask = (0x03 << (ptrGPT_settings->channel * 2));
                if (ENABLE_INPUT_PULLUP & ptrGPT_settings->mode) {
                    GPTSCR2 |= PUPT;
                }
                else {
                    GPTSCR2 &= ~PUPT;
                }
    #if defined GPT_EXTENDED_COUNTER                                     // enable
                fnSetIntHandler(GPT_TOF_VECTOR, (unsigned char *)_GPT_overflow_interrupt);
                IC_ICR_0_41 = GPTOV_INTERRUPT_PRIORITY;                  // define level and priority for overflow interrupt
                IC_IMRH_0 &= ~(GPTTOF_PIF_INT_H);                        // unmask interrupt source
                GPTSCR2 |= (TOI);                                        // enable timer overflow interrupt to extend the capture counter in software to 32bits
    #endif
                if (ptrGPT_settings->mode & GPT_CAPTURE_FALLING_EDGE) {
                    ucEdgeValue |= EDG_FALLING_0;
                }
                if (ptrGPT_settings->mode & GPT_CAPTURE_RISING_EDGE) {
                    ucEdgeValue |= EDG_RISING_0;
                }
                ucEdgeValue <<= (ptrGPT_settings->channel * 2);
                gpios_backup &= ~ucChannelBit;
                GPTIOS = gpios_backup;                                   // enable capture function on the input
    #if !defined _M52XX_SDRAM && !defined _M523X                         // {122}{140}
                PTAPAR = ((PTAPAR & ~(ucChannelMask)) | (PRIMARY_FUNCTION_Q << (ptrGPT_settings->channel * 2))); // set GPT pin function
    #endif
                GPTDDR &= ~ucChannelBit;                                 // ensure an input
    #if defined _WINDOWS
                GPTPORT = ((~(GPTDDR) & 0x0f) & ucPort_in_TA);           // {167} synchronise the general purpose timer's input data register
    #endif
                GPTCTL2 = ((GPTCTL2 & ~(ucChannelMask)) | (ucEdgeValue));// set capture edge
                gpt[ptrGPT_settings->channel].usCaptureTrigger = ptrGPT_settings->usCaptureCount;
                gpt[ptrGPT_settings->channel].usCaptureCount = 0;
                gpt[ptrGPT_settings->channel].capture_list = ptrGPT_settings->capture_list;
                fnSetIntHandler((GPT_C0F_VECTOR + ptrGPT_settings->channel), (unsigned char *)_GPT_capture_interrupt[ptrGPT_settings->channel]);
                GPT_count_interrupt_callback[ptrGPT_settings->channel] = ptrGPT_settings->int_handler;
                *(IC_ICR_GPT_ADD + 3 + ptrGPT_settings->channel) = ptrGPT_settings->int_priority; // define level and priority for GPT channel
                IC_IMRH_0 &= ~(GPTC0F_PIF_INT_H << ptrGPT_settings->channel); // unmask interrupt source
                IC_IMRL_0 &= ~(MASK_ALL_INT);                            // ensure global mask clear 
                GPTIE |= ucChannelBit;                                   // enable interrupt on capture event
                GPTSCR1 = (GPTEN | TFFCA);                               // enable GPT operation
            }
        }
        break;
#endif
#if defined SUPPORT_PWM_MODULE                                           // {132}
    case PWM_INTERRUPT:
        {
            PWM_INTERRUPT_SETUP *ptrPWM_settings = (PWM_INTERRUPT_SETUP *)ptrSettings; 
            unsigned char ucChannel;
            unsigned char *ptrRegister = PWMPER0_ADD;
            if (ptrPWM_settings->pwm_reference >= 8) {
                return;                                                  // invalid channel
            }
            POWER_UP(POWER_PWM);                                         // ensure that the PWM module is powered up
            ptrRegister += ptrPWM_settings->pwm_reference;               // period register
            ucChannel = (0x01 << ptrPWM_settings->pwm_reference);
            switch (ucChannel) {                                         // configure appropriate pin
            case PWME0:                
                PTCPAR |= TC_PWM0_FUNCTION;
                break;
            case PWME1:
                PTAPAR |= TA_PWM1_FUNCTION;
                break;
            case PWME2:
                PTCPAR |= TC_PWM2_FUNCTION;
                break;
            case PWME3:
                PTAPAR |= TA_PWM3_FUNCTION;
                break;
            case PWME4:
                PTCPAR |= TC_PWM4_FUNCTION;
                break;
            case PWME5:
                PTAPAR |= TA_PWM5_FUNCTION;
                break;
            case PWME6:
                PTCPAR |= TC_PWM6_FUNCTION;
                break;
            case PWME7:
                PTAPAR |= TA_PWM7_FUNCTION;
                break;
            }
            if (ptrPWM_settings->pwm_mode & PWM_POLARITY) {              // polarity
                PWMPOL |= ucChannel;
            }
            else {
                PWMPOL &= ~ucChannel;
            }
            if (ptrPWM_settings->pwm_mode & PWM_CENTER_ALIGNED) {        // center alignment
                PWMCAE |= ucChannel;
            }
            else {
                PWMCAE &= ~ucChannel;
            }
            if (ptrPWM_settings->pwm_mode & PWM_16_BIT_MODE) {           // concatenation to 16 bit channel
                PWMCTL |= (PWM_CON01 << (ptrPWM_settings->pwm_reference/2));
            }
            else {
                PWMCTL &= ~(PWM_CON01 << (ptrPWM_settings->pwm_reference/2));
            }
    #if defined _M5225X
            if (ptrPWM_settings->pwm_mode & PWM_PCM_MODE) {
                PCME |= ucChannel;                                       // enable the PCM mode
            }
            else {
                PCME &= ~ucChannel;                                      // disable PCM mode
            }
    #endif
            if (ptrPWM_settings->pwm_mode & PWM_SCALED_CLOCK_INPUT) {    // clock source
                if (ucChannel & (PWME0 | PWME1 | PWME4 | PWME5)) {       // channels 0,1,4 or 5
                    PWMPRCLK = ((PWMPRCLK & ~PWMPRCLK_PCKA) | (ptrPWM_settings->pwm_mode & PWMPRCLK_PCKA)); // set the prescaler for SA
                    PWMSCLA = 1;
                }
                else {                                                   // channels 2,3,6 or 7
                    PWMPRCLK = ((PWMPRCLK & ~PWMPRCLK_PCKB) | ((ptrPWM_settings->pwm_mode << BUS_CLK_SHIFT_B) & PWMPRCLK_PCKB)); // set the prescaler for SB
                    PWMSCLB = 1;
                } 
                PWMCLK |= ucChannel;                                     // clock source SA/SB
            }
            else {
                PWMCLK &= ~ucChannel;                                    // clock source A/B
            }
            *ptrRegister = ptrPWM_settings->pwm_frequency;               // set the base frequency
            ptrRegister += (PWMDTY0_ADD - PWMPER0_ADD);                  // move to the duty cycle register
            *ptrRegister = ptrPWM_settings->pwm_value;                   // set the duty cycle
            PWME |= ucChannel;                                           // enable the PWM channel
        }
        break;
#endif
#if defined SUPPORT_ADC && !defined _M520X                               // {44}{127}
    case ADC_INTERRUPT:
    #if !defined _M52XX_SDRAM                                            // {122}
        {
            ADC_SETUP *ptrADC_settings = (ADC_SETUP *)ptrSettings; 
            unsigned char ucADC_bit = ptrADC_settings->int_adc_bit;      // the channel to be configured
            if (ADC_DISABLE_ADC & ptrADC_settings->int_adc_mode) {
                uDisable_Interrupt();                                    // {118} temporarily disable interrupts while masking interrupt source to avoid possibility of spurious interrupt
                IC_IMRH_0 |= (ADCINT_PIF_INT_H);                         // mask interrupt source
                uEnable_Interrupt();
                PANPAR = 0;                                              // return all ADC inputs back to GPIO
                ADC_ADSDIS = (DISABLE_CH0 | DISABLE_CH1 | DISABLE_CH2 | DISABLE_CH3 | DISABLE_CH4 | DISABLE_CH5 | DISABLE_CH6 | DISABLE_CH7); // disable all sampling by default
                POWER_DOWN(POWER_ADC);
                return;
            }
            if (ucADC_bit >= ADC_CHANNELS) {
                return;                                                  // channel invalid
            }
            if ((ptrADC_settings->int_adc_mode & (ADC_GET_RESULT | ADC_ALL_RESULTS)) && (ptrADC_settings->int_adc_result != 0)) { // if there is a result structure
                int iLastValue = ucADC_bit;
                int iFirstValue = ucADC_bit;
                if (ptrADC_settings->int_adc_mode & ADC_ALL_RESULTS) {
                    iLastValue = (ADC_CHANNELS - 1);                     // return results for all ADC channels
                    iFirstValue = 0;
                }
                while (iFirstValue <= iLastValue) { 
                    if ((ADC_ADSTAT & (RDY_CHANNEL0 << iFirstValue)) != 0) {
                        ptrADC_settings->int_adc_result->sADC_value[iFirstValue] = *(ADC_ADRSLT_ADD + iFirstValue); // the last conversion result
                        ptrADC_settings->int_adc_result->ucADC_status[iFirstValue] = ADC_RESULT_VALID;
                    }
                    else {
                        ptrADC_settings->int_adc_result->ucADC_status[iFirstValue] = ADC_RESULT_NOT_READY;
    #if defined _WINDOWS                                                 // {54}
                        fnUpdateADC(ucADC_bit);                          // cause the simulated input value to be captured
    #endif
                    }
                    iFirstValue++;
                }
            }
            if (!(ptrADC_settings->int_adc_mode & ADC_READ_ONLY)) {
                unsigned short usADC_Control_Reg = ADC_CTRL1;            // original value
                unsigned short usADC_Zero_Reg = 0;
                if (ptrADC_settings->int_adc_mode & ADC_CONFIGURE_ADC) { // main configuration
                    unsigned short usCal = 0;
                    POWER_UP(POWER_ADC);                                 // enable clocks to module
    #if defined _WINDOWS                                                 // {131}
                    if (ptrADC_settings->int_adc_speed > SLOWEST_ADC_CLOCK ) {
                        _EXCEPTION("The ADC speed is not obtainable!!!!"); // warn that the requested speed can not be achieved since the divider can not be set high enough
                    }
    #endif
                    ADC_CTRL2 = ptrADC_settings->int_adc_speed;          // {111} set the ADC clock rate (100kHz..5MHz range)
                    if (ptrADC_settings->int_adc_mode & ADC_AUTO_POWER_DOWN_MODE) {
                        ADC_POWER |= APD;                                // auto-power down mode
                    }
                    else if (ptrADC_settings->int_adc_mode & ADC_AUTO_STANDBY_MODE) {
                        ADC_POWER |= ASB;                                // auto-standby mode
                    }
                    if (ptrADC_settings->int_adc_mode & ADC_PARALLEL_MODE) {
                        usADC_Control_Reg |= ADC_PARALLEL_OPN;
                        if (ptrADC_settings->int_adc_mode & ADC_TRIGGERED_MODE) {
                            usADC_Control_Reg &= ~ADC_LOOP_OPN;
                            usADC_Control_Reg |= (SYNC0 | ADC_TRIG_OPN);
                            if (ptrADC_settings->int_adc_mode & ADC_SIMULT_MODE) {
                                ADC_CTRL2 |= SIMULT;
                            }
                            else {
                                ADC_CTRL2 |= SYNC1;
                                ADC_CTRL2 &= ~(SIMULT | STOP1);
                            }
                        }
                        else {
                            usADC_Control_Reg &= ~(SYNC0 | ADC_TRIG_OPN);
                            ADC_CTRL2 &= ~SYNC1;
                            if (ptrADC_settings->int_adc_mode & ADC_SIMULT_MODE) {
                                ADC_CTRL2 |= SIMULT;
                            }
                            else {
                                ADC_CTRL2 &= ~(SIMULT);
                            }
                        }
                    }
                    else {
                        usADC_Control_Reg &= ~ADC_PARALLEL_OPN;
                        ADC_CTRL2 &= ~(SIMULT |SYNC1);
                        if (ptrADC_settings->int_adc_mode & ADC_TRIGGERED_MODE) {
                            usADC_Control_Reg |= (SYNC0 | ADC_TRIG_OPN);
                            usADC_Control_Reg &= ~(STOP0);
                        }
                        else {
                            usADC_Control_Reg &= ~(SYNC0 | ADC_TRIG_OPN);
                        }
                    }
                    if ((!(ptrADC_settings->int_adc_mode & ADC_TRIGGERED_MODE)) && (ptrADC_settings->int_adc_mode & ADC_LOOP_MODE)) {
                        usADC_Control_Reg |= ADC_LOOP_OPN;
                    }
                    else {
                        usADC_Control_Reg &= ~ADC_LOOP_OPN;
                    }
                    if (ADC_HIGH_REF_AN2 & ptrADC_settings->int_adc_mode) {
                        usCal = SEL_VREFH_AN2;
                    }
                    if (ADC_LOW_REF_AN6 & ptrADC_settings->int_adc_mode) {
                        usCal |= SEL_VREFL_AN6;                          // {64}
                    }
                    ADC_CAL = usCal;                                     // configure reference inputs
                }
                if (ADC_SET_CHANNEL_OFFSET & ptrADC_settings->int_adc_mode) {
                    *(ADC_ADOFS_ADD + ucADC_bit) = (ptrADC_settings->int_adc_offset & 0x7ff8); // enter the offset value for this channel
                }
                if (ptrADC_settings->int_adc_mode & ADC_CONFIGURE_CHANNEL) {
                    unsigned short usModulePower;
                    unsigned short usModulePowerCheck;
                    unsigned short usEnable = 0;
                    unsigned short usEnableBit = DISABLE_CH0;
                    if (ptrADC_settings->int_adc_mode & ADC_DIFFERENTIAL) {
                        usADC_Control_Reg |= (DIFF_AN0_AN1 << (ucADC_bit/2)); // configure for differential mode
                        PANPAR |= (0x01 << (ucADC_bit & ~0x01));         // configure pins as differential ADC inputs
                        PANPAR |= (0x01 << (ucADC_bit | 0x01));          // configure pins as differential ADC inputs
                    }
                    else {
                        usADC_Control_Reg &= ~(DIFF_AN0_AN1 << (ucADC_bit/2)); // single ended
                        PANPAR |= (0x01 << ucADC_bit);                   // configure pin as ADC input
                    }
                    if (ucADC_bit < (ADC_CHANNELS/2)) {
                        usModulePower = (PD0 | PD2);                     // first ADC module
                        usModulePowerCheck = (PSTS0 | PSTS2);
                    }
                    else {
                        usModulePower = (PD1 | PD2);                     // second ADC module
                        usModulePowerCheck = (PSTS1 | PSTS2);
                        if (ptrADC_settings->int_adc_mode & ADC_PARALLEL_MODE) { // {53}
                            usEnableBit = DISABLE_CH4;
                        }
                        else {
                            usModulePower |= PD0;                        // when not operating in parallel, also first ADC module must be powered
                            usModulePowerCheck |= PSTS0;
                        }
                    }
                    ADC_POWER &= ~usModulePower;                         // power the required module
                    if (!(ADC_POWER & APD)) {                            // if not auto-power mode, wait for power to be applied
                        while (ADC_POWER & usModulePowerCheck) {}        // wait for ADC power to be stable before continuing
                    }
                    do {
                        usEnable |= usEnableBit;
                        if ((0x01 << ucADC_bit) & usEnableBit) {
                            break;
                        }
                        usEnableBit <<= 1;
                    } while (1);
                    ADC_ADSDIS &= ~usEnable;                             // enable as many channels as necessary
                }
                else if (ADC_DISABLE_CHANNEL & ptrADC_settings->int_adc_mode) {
                    unsigned short usDisable = 0;
                    unsigned short usDisableBit = DISABLE_CH7;
                    if (ADC_CTRL1 & (DIFF_AN0_AN1 << (ucADC_bit/2))) {   // configured as differential inputs
                        PANPAR &= ~(0x01 << (ucADC_bit & ~0x01));        // configure original differential pins as GPIO
                        PANPAR &= ~(0x01 << (ucADC_bit |= 0x01));        // configure original differential pins as GPIO
                    }
                    else {
                        PANPAR &= ~(0x01 << ucADC_bit);                  // configure pin as GPIO
                    }
                    if (!(PANPAR & (PORT_AN_BIT0 | PORT_AN_BIT1 | PORT_AN_BIT2 | PORT_AN_BIT3))) {
                        ADC_POWER |= PD0;                                // power down circuit A
                    }
                    if (!(PANPAR & (PORT_AN_BIT4 | PORT_AN_BIT5 | PORT_AN_BIT6 | PORT_AN_BIT7))) {
                        ADC_POWER |= PD1;                                // power down circuit B
                    }
                    do {
                        if (PANPAR & usDisableBit) {
                            break;
                        }
                        usDisable |= usDisableBit;
                        usDisableBit >>= 1;
                    } while (usDisableBit != 0x00);
                    ADC_ADSDIS |= usDisable;                             // disable as many channels as possible
                }
                if (((usADC_Control_Reg & SYNC0) && (PANPAR & (PORT_AN_BIT0 | PORT_AN_BIT1 | PORT_AN_BIT2 | PORT_AN_BIT3))) ||
                    ((ADC_CTRL2 & SIMULT) & (PANPAR & (PORT_AN_BIT4 | PORT_AN_BIT5 | PORT_AN_BIT6 | PORT_AN_BIT7)))) { // trigger mode on an active channel A input (or B when simultaneous mode) - enable SYNCA pin
    #if defined SYNCA_ON_PAS 
                    unsigned short NewPASPAR = (unsigned short)(PASPAR & ~(ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT));
                    PASPAR = (unsigned short)(NewPASPAR | (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT));
    #elif defined SYNCA_ON_PNQ
        #if defined _M521X                                               // {95}
                    PNQPAR = ((PNQPAR & ~0x03) | 0x02);
        #else
                    unsigned short NewPNQPAR = (unsigned short)(PNQPAR & ~(ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT));
                    PNQPAR = (unsigned short)(NewPNQPAR | (ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT));
        #endif
    #elif defined SYNCA_ON_PQS
                    unsigned short NewPQSPAR = (unsigned short)(PQSPAR & ~(ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT));
                    PQSPAR = (unsigned short)(NewPQSPAR | (ALT_1_FUNCTION_Q << BIT_6_Q_SHIFT));                   
    #elif defined SYNCA_ON_PUB
                    unsigned short NewPUBPAR = (unsigned short)(PUBPAR & ~(ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT));
                    PUBPAR = (unsigned short)(NewPUBPAR | (ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT));
    #endif
                }
    #if defined SYNCA_ON_PAS 
                else if ((PASPAR & (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)) {
                    PASPAR &= ~(ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT);  // set back GPIO function if it was SYNCA
                }
    #elif defined SYNCA_ON_PNQ
        #if defined _M521X                                               // {95}
                else if ((PNQPAR & 0x03) == 0x01) {
                    PNQPAR &= ~(0x03);                                   // set back GPIO function if it was SYNCA
                }
        #else
                else if ((PNQPAR & (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT)) == (ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT)) {
                    PNQPAR &= ~(ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT);  // set back GPIO function if it was SYNCA
                }
        #endif
    #elif defined SYNCA_ON_PQS
                else if ((PQSPAR & (ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT)) == (ALT_1_FUNCTION_Q << BIT_6_Q_SHIFT)) {
                    PQSPAR &= ~(ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT);  // set back GPIO function if it was SYNCA
                }
    #elif defined SYNCA_ON_PUB
                else if ((PUBPAR & (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)) == (ALT_1_FUNCTION_Q << BIT_3_Q_SHIFT)) {
                    PUBPAR &= ~(ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT);  // set back GPIO function if it was SYNCA
                }
    #endif
                if ((ADC_CTRL2 & SYNC1) & (PANPAR & (PORT_AN_BIT4 | PORT_AN_BIT5 | PORT_AN_BIT6 | PORT_AN_BIT7))) { // trigger mode on an active channel B input - enable SYNCB pin
    #if defined SYNCB_ON_PAS 
                    unsigned short NewPASPAR = (unsigned short)(PASPAR & ~(ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT));
                    PASPAR = (unsigned short)(NewPASPAR | (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT));
    #elif defined SYNCB_ON_PQS
                    PQSPAR |= (ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT);
    #elif defined SYNCB_ON_PUB
                    unsigned char NewPUBPAR = (unsigned char)(PUBPAR & ~(ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT));
                    PUBPAR = (unsigned char)(NewPUBPAR | (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT));
    #endif
                }
    #if defined SYNCB_ON_PAS 
                else if ((PASPAR & (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)) {
                    PASPAR &= ~(ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT);      // set back GPIO function if it was SYNCB
                }
    #elif defined SYNCB_ON_PQS
                else if ((PQSPAR & (ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT)) == (ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT)) {
                    PQSPAR &= ~(ALT_2_FUNCTION_Q << BIT_6_Q_SHIFT);      // set back GPIO function if it was SYNCB
                }
    #elif defined SYNCB_ON_PUB
                else if ((PUBPAR & (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)) {
                    PUBPAR &= ~(ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT);      // set back GPIO function if it was SYNCB
                }
    #endif
                if (ADC_DISABLE_CHANNEL & ptrADC_settings->int_adc_mode) {
                    return;
                }
                if (ptrADC_settings->int_adc_int_type & (ADC_HIGH_LIMIT_INT | ADC_LOW_LIMIT_INT | ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE)) {
                    if (ptrADC_settings->int_adc_int_type & ADC_HIGH_LIMIT_INT) {
                        if (ptrADC_settings->int_adc_int_type & ADC_DISABLE_INTS) {
                            *(ADC_ADLLR_ADD + ADC_CHANNELS + ucADC_bit) = 0x7ff8;// disable high limit checking
                            usADC_Control_Reg &= ~HLMTIE;                // disable high level trigger interrupt
                            adc_int_level_handlers[ucADC_bit + ADC_CHANNELS] = 0;
                        }
                        else {
                            *(ADC_ADLLR_ADD + ADC_CHANNELS + ucADC_bit) = ptrADC_settings->int_high_level_trigger;
                            usADC_Control_Reg |= HLMTIE;                 // enable high level trigger interrupt
                            adc_int_level_handlers[ucADC_bit + ADC_CHANNELS] = ptrADC_settings->int_handler;
                        }
                    }
                    if (ptrADC_settings->int_adc_int_type & ADC_LOW_LIMIT_INT) {
                        if (ptrADC_settings->int_adc_int_type & ADC_DISABLE_INTS) {
                            *(ADC_ADLLR_ADD + ucADC_bit) = 0;            // disable low limit checking
                            usADC_Control_Reg &= ~LLMTIE;                // disable low level trigger interrupt
                            adc_int_level_handlers[ucADC_bit] = 0;
                        }
                        else {
                            *(ADC_ADLLR_ADD + ucADC_bit) = ptrADC_settings->int_low_level_trigger;
                            usADC_Control_Reg |= LLMTIE;                 // enable low level trigger interrupt
                            adc_int_level_handlers[ucADC_bit] = ptrADC_settings->int_handler;
                        }
                    }
                    if (ptrADC_settings->int_adc_int_type & (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE)) {
                        switch (ptrADC_settings->int_adc_int_type & (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE)) {
                        case ADC_ZERO_CROSSING_INT_POSITIVE:
                            usADC_Zero_Reg = (ZERO_CROSS_POS << (ucADC_bit * 2));
                            break;
                        case ADC_ZERO_CROSSING_INT_NEGATIVE:
                            usADC_Zero_Reg = (ZERO_CROSS_NEG << (ucADC_bit * 2));
                            break;
                        case (ADC_ZERO_CROSSING_INT_POSITIVE | ADC_ZERO_CROSSING_INT_NEGATIVE):
                            usADC_Zero_Reg = (ZERO_CROSS_BOTH << (ucADC_bit * 2));
                            break;
                        default:
                            break;
                        }
                        if (ptrADC_settings->int_adc_int_type & ADC_DISABLE_INTS) {
                            usSingleShotTrigger &= ~(1 << ucADC_bit);
                            ADC_ADZCC &= ~usADC_Zero_Reg;
                            usADC_Control_Reg &= ~ZCIE;                  // disable zero-crossing interrupt
                            adc_int_zero_handlers[ucADC_bit] = 0;
                        }
                        else {
                            if (ptrADC_settings->int_adc_int_type & ADC_SINGLE_SHOT_CROSSING_INT) {
                                usSingleShotTrigger |= (1 << ucADC_bit); // enable single short trigger for this channel
                            }
                            adc_int_zero_handlers[ucADC_bit] = ptrADC_settings->int_handler;
                            usADC_Zero_Reg |= ADC_ADZCC & ~(ZERO_CROSS_BOTH << (ucADC_bit * 2));
                            ADC_ADZCC = usADC_Zero_Reg;
                            usADC_Control_Reg |= ZCIE;                   // enable zero-crossing interrupt
                        }
                    }
                    if (usADC_Control_Reg & (LLMTIE | HLMTIE | ZCIE)) {
                        if (ptrADC_settings->int_handler != 0) {
                            fnSetIntHandler(ADC_ADCINT_VECTOR, (unsigned char *)_ADC_level_interrupt);
                            if (usADC_Control_Reg & (LLMTIE | HLMTIE)) {
                                ADC_ADLSTAT = ((LLS_CHANNEL0 | HLS_CHANNEL0) << ucADC_bit); // clear pending interrupts
        #if defined _WINDOWS
                                ADC_ADLSTAT &= ~((LLS_CHANNEL0 | HLS_CHANNEL0) << ucADC_bit);
        #endif
                            }
                            if (usADC_Control_Reg & ZCIE) {
                                ADC_ADZCSTAT = (ZCS_CHANNEL0 << ucADC_bit);  // clear pending interrupts
        #if defined _WINDOWS
                                ADC_ADZCSTAT &= ~(ZCS_CHANNEL0 << ucADC_bit);
        #endif
                            }
                            if (usADC_Control_Reg & (LLMTIE | HLMTIE | ZCIE)) {
                                IC_ICR_0_51 = ptrADC_settings->int_priority; // define level and priority for ADC
                                IC_IMRH_0 &= ~(ADCINT_PIF_INT_H);        // unmask interrupt source
                                IC_IMRL_0 &= ~(MASK_ALL_INT);            // ensure global mask clear                                 
                            }
                        }
                    }
                    else {
                        uDisable_Interrupt();                            // {118} temporarily disable interrupts while masking interrupt source to avoid possibility of spurious interrupt
                        IC_IMRH_0 |= (ADCINT_PIF_INT_H);                 // mask interrupt source
                        uEnable_Interrupt();
                    }
                }
                else {
                    if (ptrADC_settings->int_adc_int_type & ADC_END_OF_SCAN_INT) {
                        if (ptrADC_settings->int_adc_int_type & ADC_DISABLE_INTS) {
                            usSingleShotTrigger &= ~(ADC_CHAN_A_SINGLE_SHOT | ADC_CHAN_B_SINGLE_SHOT);
                            usADC_Control_Reg &= ~EOSIE0;
                            ADC_CTRL2 &= ~EOSIE1;
                            uDisable_Interrupt();                        // {118} temporarily disable interrupts while masking interrupt source to avoid possibility of spurious interrupt
                            IC_IMRH_0 |= (ADCA_PIF_INT_H);               // mask interrupt sources
                            IC_IMRH_0 |= (ADCB_PIF_INT_H);
                            uEnable_Interrupt();
                        }
                        else {
                            if ((ucADC_bit < (ADC_CHANNELS/2)) || (ptrADC_settings->int_adc_mode & (ADC_SIMULT_MODE | ADC_SEQUENTIAL_MODE)) || (!(ptrADC_settings->int_adc_mode & ADC_PARALLEL_MODE))) { // channel A or when not parallel B only{131}
                                usADC_Control_Reg |= EOSIE0;
                                if (ADC_SINGLE_SHOT_TRIGGER_INT & ptrADC_settings->int_adc_int_type) {
                                    usSingleShotTrigger |= ADC_CHAN_A_SINGLE_SHOT;
                                }
                                fnSetIntHandler(ADC_ADCA_VECTOR, (unsigned char *)_ADC_convertA_interrupt);
                                adc_int_complete_handlerA = ptrADC_settings->int_handler;
                                IC_ICR_0_49 = ptrADC_settings->int_priority; // define level and priority for ADC - channel A
                                IC_IMRH_0 &= ~(ADCA_PIF_INT_H);          // unmask interrupt source
                                IC_IMRL_0 &= ~(MASK_ALL_INT);            // ensure global mask clear 
                            }
                            else {
                                ADC_CTRL2 |= EOSIE1;                     // enable interrupt when channel B conversion completes
                                if (ADC_SINGLE_SHOT_TRIGGER_INT & ptrADC_settings->int_adc_int_type) {
                                    usSingleShotTrigger |= ADC_CHAN_B_SINGLE_SHOT;
                                }
                                fnSetIntHandler(ADC_ADCB_VECTOR, (unsigned char *)_ADC_convertB_interrupt);
                                adc_int_complete_handlerB = ptrADC_settings->int_handler;
                                IC_ICR_0_50 = ptrADC_settings->int_priority; // define level and priority for ADC - channel B
                                IC_IMRH_0 &= ~(ADCB_PIF_INT_H);          // unmask interrupt source
                                IC_IMRL_0 &= ~(MASK_ALL_INT);            // ensure global mask clear 
                            }
                        }
                    }
                }
                if (ptrADC_settings->int_adc_mode & ADC_START_OPERATION) {                    
                    usADC_Control_Reg &= ~STOP0;                         // remove stop bit
                    usADC_Control_Reg |= START0;                         // start conversion
                    if (usADC_Control_Reg & ADC_PARALLEL_OPN) {
                        if (ptrADC_settings->int_adc_mode & ADC_SIMULT_MODE) {
                            ADC_CTRL2 &= ~(STOP1);
                            ADC_CTRL2 |= (START1 | SIMULT);
                        }
                        else {
                            ADC_CTRL2 &= ~(STOP1 | SIMULT);
                            ADC_CTRL2 |= START1;
                        }
                    }
                }
                ADC_CTRL1 = usADC_Control_Reg;                           // set new mode (and enable interrupts)
            }
        }
    #endif
        break;
#endif
    default:
        _EXCEPTION("Attempting configuration of interrupt interface without appropriate support enabled!!");
        break;
    }
}

#if defined SUPPORT_RTC && !defined _M521X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {10}{122}{127}{140}

static void (*RTC_handler[6])(void) = {0};                               // all possible RTC interrupts


// RTC interrupt handler
//
static __interrupt__ void _rtc_handler(void)
{  
    int iIRQBit = 0;
    while ((RTCISR & RTCIENR) != 0) {                                    // while enabled interrupts exist
        if ((0x01 << iIRQBit) & RTCISR & RTCIENR) {
    #if defined _WINDOWS
            RTCISR &= ~(0x01 << iIRQBit);
    #else
            RTCISR = (0x01 << iIRQBit);                                  // clear interrupt
    #endif
            if ((0x01 << iIRQBit) & (RTC_ALARM_INT | RTC_STOPWATCH)) {
                RTCIENR &= ~(0x01 << iIRQBit);                           // ensure no second interrupt can occur
            }
            if (iIRQBit == 4) {                                          // {144} second interrupt
                fnSecondsTick(RTC_handler, 0);                           // {186} generic handling of seconds interrupt
            }
            else if (RTC_handler[iIRQBit] != 0) {                        // advanced hardware interrupt handling (alarm, stopwatch, etc.)
                iInterruptLevel = 1;                                     // ensure interrupts remain blocked during subroutines
                    RTC_handler[iIRQBit]();                              // call the interrupt handler
                iInterruptLevel = 0;
            }
        }
        if (++iIRQBit >= 6) {                                            // end of RTC hardware interrupts
            iIRQBit = 0;                                                 // set back to first
        }
    }
}

extern int fnConfigureRTC(void *ptrSettings)                             // {141}
{
    int iIRQ = 0;
    RTC_SETUP *ptr_rtc_setup = (RTC_SETUP *)ptrSettings;
    switch (ptr_rtc_setup->command & ~(RTC_DISABLE | RTC_INITIALISATION | RTC_SET_UTC)) { // {185}
    case RTC_TIME_SETTING:                                               // set time to RTC
        RTCCTL = 0;                                                      // temporarily disable RTC to avoid potentially interrupt
        if (ptr_rtc_setup->command & RTC_SET_UTC) {                      // {185} allow setting from UTC seconds value
            fnConvertSecondsTime(ptr_rtc_setup, ptr_rtc_setup->ulLocalUTC);          // convert to time for internal use
        }
        DAYS = fnConvertTimeDays(ptr_rtc_setup, 1);                      // set the present time and date as days since 1970
        HOURMIN = (((ptr_rtc_setup->ucHours << 8) & HOURS_MASK) | ((ptr_rtc_setup->ucMinutes & MINUTES_MASK)));
        SECONDS = (ptr_rtc_setup->ucSeconds & SECONDS_MASK);
        RTCCTL = RTC_ENABLE;                                             // enable counter again
        break;

    case RTC_GET_TIME:
        fnGetRTC(ptr_rtc_setup);                                         // {186}
        break;
    case RTC_CONVERT_TO_UTC:                                             // {186}
        ptr_rtc_setup->ulLocalUTC = fnConvertTimeSeconds(ptr_rtc_setup, 0); // take time/date as input and return UTC value
        break;
    case RTC_CONVERT_FROM_UTC:                                           // {186}
        fnConvertSecondsTime(ptr_rtc_setup, ptr_rtc_setup->ulLocalUTC);  // take UTC value and convert it to time/date
        break;
    case RTC_TICK_HOUR:
        iIRQ++;
    case RTC_TICK_SEC:
        iIRQ++;
    case RTC_TICK_DAY:
        iIRQ++;
    case RTC_ALARM_TIME:
        if (iIRQ == 0) {                                                 // RTC_ALARM_TIME
            RTCIENR &= ~RTC_ALARM_INT;                                   // disable ALARM interrupt
            ALRM_DAY = fnConvertTimeDays(ptr_rtc_setup, 0);              // set alarm day
            ALRM_HM  = (((ptr_rtc_setup->ucHours << 8) & HOURS_MASK) | ((ptr_rtc_setup->ucMinutes & MINUTES_MASK))); // set alarm hour and minute
            ALRM_SEC = (ptr_rtc_setup->ucSeconds & SECONDS_MASK);        // set alarm second
        }
        iIRQ++;
    case RTC_TICK_MIN:
        iIRQ++;
    case RTC_STOPWATCH_GO:
        {
            unsigned long ulTotalSeconds;
            unsigned long ulSeconds;
            if (iIRQ == 0) {                                             // RTC_STOPWATCH_GO
                RTCIENR &= ~RTC_STOPWATCH;                               // disable STOPWATCH interrupt
                STPWCH = ((ptr_rtc_setup->ucMinutes & MINUTES_MASK) - 1); // set Stopwatch counter
            }
            if (RTC_DISABLE & (ptr_rtc_setup->command)) {
                RTCIENR &= ~(0x01 << iIRQ);                              // disable interrupt
                return 0;                                                // disable function rather than enable
            }
            RTC_handler[iIRQ] = ((INTERRUPT_SETUP *)ptrSettings)->int_handler;
            IC_ICR_0_63 = RTC_INTERRUPT_PRIORITY;
            RTCISR = (0x01 << iIRQ);                                     // clear possible waiting interrupt
            fnSetIntHandler(RTC_VECTOR, (unsigned char *)_rtc_handler);
            do {                                                         // {141}
                ulSeconds = SECONDS;
                ulTotalSeconds = (ulSeconds + ((HOURMIN & MINUTES_MASK) * 60) + (((HOURMIN & HOURS_MASK) >> 8) * 60 * 60) + (DAYS * 60 * 60 * 24));
            } while (ulSeconds != SECONDS);
            fnConvertSecondsTime(0, ulTotalSeconds);                     // take the present seconds value and convert to time and date
            RTCIENR |= (0x01 << iIRQ);                                   // enable interrupt
            IC_IMRH_0 &= ~(RTC_INT_H);                                   // unmask interrupt source
            IC_IMRL_0 &= ~(MASK_ALL_INT);                                // clear global mask
        #if defined _WINDOWS
            RTCISR &= ~(0x01 << iIRQ);
        #endif
        }
        break;
    }
    return 0;
}

    #if defined SDCARD_SUPPORT && defined SUPPORT_FILE_TIME_STAMP        // {142}
// This routine supports date and time stamping of files and directories. If the information is not available it could return 1
// so that a fixed stamp is set. It is assumed that the RTC in the Coldfire will be used if the support is activated
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


// This routine switches to low power mode. It is always called with disabled interrupts and wakes with interrupts enabled again
//
extern void fnDoLowPower(void)
{
    // switch to low power mode
#if defined _WINDOWS
    uEnable_Interrupt();
#else
    // WAIT or DOZE mode - commanded by STOP instruction (STOP #<data> - writes data to SR and enters stop mode with enabled interrupts)
    iInterruptLevel = 0;                                                 // mark that no nested interrupts after wake up
    #if defined _GNU
    asm ("stop #0x2000");
    #elif defined _COMPILE_IAR                                           // {136}
    __stop(0x2000);                                                      // IAR intrinsic
    #else
    asm { stop #0x2000 }
    #endif  
#endif

    // Wait here until the processor is woken by an interrupt
}

// Set a new low power mode (dummy)
//
extern void fnSetLowPowerMode(int new_lp_mode)
{
}

#if defined DMA_MEMCPY_SET
#define SMALLEST_DMA_COPY 20                                             // smaller copies have no DMA advantage
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;

    if ((Size >= SMALLEST_DMA_COPY) && (!DMA_SR_BCR3)) {                 // if large enough to be worthwhile and if not already in use
        DMA_SR_BCR3 = ((unsigned long)(Size));                           // the number of byte transfers to be made (SR_BCRn)

        if ((unsigned long)ptrFrom < (FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // {77} modified from < START_OF_SRAM to allow operation in NVRAM
            DMA_SAR3 = (BACKDOOR_FLASH + (unsigned long)ptrFrom);        // if from FLASH, set to backdoor
        }
        else {
            DMA_SAR3  = (unsigned long)ptrFrom;                          // address of first byte to be transferred  (SARn)
        }

        DMA_DAR3  = (unsigned long)ptrTo;                                // address of first destination byte       (DARn)
        DMA_DCR3  = (DCR_BWC_16K | DCR_SINC | DCR_DINC | DCR_SSIZE_BYTE | DCR_DSIZE_BYTE | DCR_START); // set up DMA operation (DCRn) and start DMA transfer
        while (!(DMA_SR_BCR3 & DSR_DONE)) { SIM_DMA_3 };                 // wait until the transfer has terminated

        DMA_SR_BCR3 = DSR_DONE;                                          // clear all status bits ready for future transfer
  #if defined _WINDOWS
        DMA_SR_BCR3 = 0;
  #endif
    }
    else {
        unsigned char *ptr1 = (unsigned char *)ptrTo;                    // normal memcpy method
        unsigned char *ptr2 = (unsigned char *)ptrFrom;
        while (Size--) {
            *ptr1++ = *ptr2++;
        }
    }
    return buffer; 
}


// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)               // {191}
{
    void *buffer = ptrTo;
    register unsigned char ucValue = (unsigned char)iValue;              // {191}

    if ((Size >= SMALLEST_DMA_COPY) && (!DMA_SR_BCR3) && ((unsigned long)ptrTo < IPSBAR)) { // if large enought to be worth while and if not already in use - don't try to transfer to peripheral block locations
        volatile unsigned char ucToCopy = ucValue;
        DMA_SR_BCR3 = ((unsigned long)(Size));                           // the number of byte transfers to be made (SR_BCRn)
        DMA_SAR3  = (unsigned long)&ucToCopy;                            // address of byte to be transferred  (SARn)

        DMA_DAR3  = (unsigned long)ptrTo;                                // address of first destination byte       (DARn)
        DMA_DCR3  = (DCR_BWC_16K | DCR_DINC | DCR_SSIZE_BYTE | DCR_DSIZE_BYTE | DCR_START); // set up DMA operation (DCRn) and start DMA transfer
        while (!(DMA_SR_BCR3 & DSR_DONE)) { SIM_DMA_3 };                 // wait until the transfer has terminated

        DMA_SR_BCR3 = DSR_DONE;                                          // clear all status bits ready for future transfer
  #if defined _WINDOWS
        DMA_SR_BCR3 = 0;
  #endif
    }
    else {
        unsigned char *ptr = (unsigned char *)ptrTo;                     // normal memset method

        while (Size--) {
            *ptr++ = ucValue;
        }
    }
    return buffer;
}
#endif

#if defined _GNU                                                        // map between GNU and Codewarrior conventions
    #define __DATA_ROM    __data_load 
    #define __DATA_RAM    __data_start
    #define __DATA_END    __bss_start
    #define __BSS_START   __bss_start
    #define __BSS_END     __end
#endif

extern void mcf52235_init(void)
{
#if !defined _COMPILE_IAR
    extern char __DATA_ROM[];
    extern char __DATA_RAM[];
    extern char __DATA_END[];
    extern char __BSS_START[];
    extern char __BSS_END[];
#endif
    extern unsigned long __VECTOR_RAM[];
#if !defined _WINDOWS && !defined _GNU && !defined _COMPILE_IAR && !defined _M520X // {127}
    extern unsigned long FLASH_CONFIG[];
#endif
    register unsigned long n;
#if !defined _COMPILE_IAR && !defined _WINDOWS
    register unsigned char *dp, *sp;
#endif
#if defined _M5225X                                                      // {75} enable crystal and switch off relaxation oscillator
    #if defined RELAXATION_OSCILLATOR                                    // {89}
    MCF_CLOCK_OCHR = RELAX_OCOEN;                                        // enable relaxation oscillator
    MCF_CLOCK_CCLR = OSCSEL;                                             // switch to relaxation oscillator source
    MCF_CLOCK_OCLR = 0x00;                                               // disable external crystal oscillator
    #elif defined EXTERNAL_OSCILLATOR                                    // {89}
    MCF_CLOCK_OCLR = (OSCEN | RANGE);
    MCF_CLOCK_CCLR = 0;                                                  // switch to external source
    MCF_CLOCK_OCHR = 0;                                                  // disable relaxation oscillator
    #else
    MCF_CLOCK_OCLR = (OSCEN | REFS | RANGE);                             // enable external crystal oscillator
    MCF_CLOCK_CCLR = 0x00;                                               // switch to crystal source
    MCF_CLOCK_OCHR = 0x00;                                               // disable relaxation oscillator
    #endif
#endif

#if !defined _M521X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X  // {122}{127}{140} M521X/M528X/M520X/M523X have no RTC
    #if defined _M5221X || defined _M521XX || defined _M5225X
        #if defined SUPPORT_RTC
            #if defined RTC_32K_CRYSTAL                                  // {52}{112}
    if (MCF_CLOCK_RTCCR != (EXTALEN | RTCSEL_OSC | RTC_LPEN | RTC_REFS | KHZEN_KHZ | RTC_OSCEN)) { // only on first start initialise 32k
        RTCCTL = RTC_ENABLE;
        MCF_CLOCK_RTCCR = (EXTALEN | RTCSEL_OSC | RTC_LPEN | RTC_REFS | KHZEN_KHZ/* | RTC_OSCEN*/); // {56}{115}
        RTCGOCL = (unsigned short)(32768/4);
        MCF_CLOCK_RTCCR = (EXTALEN | RTCSEL_OSC | RTC_LPEN | RTC_REFS | KHZEN_KHZ | RTC_OSCEN);
    }
            #else
    RTCCTL = RTC_ENABLE;
    MCF_CLOCK_RTCCR = 0;                                                 // select system clock for RTC
    RTCGOCU = (unsigned short)((BUS_CLOCK/4) >> 16);                     // {110}
    RTCGOCL = (unsigned short)(BUS_CLOCK/4);
            #endif
        #else
    RTCCTL  = 0;
    RTCGOCU = 0;                                                         // disable RTC
    RTCGOCL = 0;
    MCF_CLOCK_RTCCR   = 0;
        #endif
    #else
        #if defined SUPPORT_RTC
	MCF_CLOCK_RTCDR = (CRYSTAL_FREQ - 1);                                // set RTC 1Hz frequency {10}{93}
        #else
	RTCCTL = 0;                                                          // disable RTC
        #endif
    #endif
#endif

#if !defined DISABLE_PLL && !defined _M520X                              // {42}{127}
    #if defined _M52XX_SDRAM                                             // {122}
    MCF_CLOCK_SYNCR = ((((PLL_MUL - 4)/2) << 12) | (POST_DIVIDE << 8));  // set up PLL to generate the required frequency
    #elif defined _M523X                                                 // {140}
    MCF_CLOCK_SYNCR = ((((PLL_MUL - 4)/2) << 24) | (POST_DIVIDE << 19)); // set up PLL to generate the required frequency
    #else
        #if defined PRE_DIVIDER
    MCF_CLOCK_CCHR  = (PRE_DIVIDER - 1);                                 // values from 1 to 8 possible - should generate 1..10MHz input
        #endif
    MCF_CLOCK_SYNCR = ((((PLL_MUL - 4)/2) << 12) | CLKSRC | PLLMODE | PLLEN | (POST_DIVIDE << 8)); // set up PLL to generate the required frequency
    #endif

    while (!(MCF_CLOCK_SYNSR & PLLOCKED)) { SIM_LOCK }                   // wait for PLL to lock
#endif
#if defined _M520X || defined _M523X                                     // {127}{140}
    CONFIG_EXT_MEMORY();                                                 // external memory (SDRAM) configured here
#endif
#if !defined _M520X
    MCF_SCM_RAMBAR = ((RAM_START_ADDRESS & RAMBAR_ADD_MASK) | BACK_DOOR_ACCESS); // enable SRAM access by peripherals
    #if defined _M52XX_SDRAM                                             // {122}
    CCM_CCR |= BMT_1K;                                                   // reduce bus monitor timing to 1k clocks
    #endif
#endif
#if !defined _WINDOWS && !defined _GNU && !defined _COMPILE_IAR && !defined _M520X && !defined _M523X // {127}{140}
    __VECTOR_RAM[0] = FLASH_CONFIG[0];                                   // force flash config table to be linked	
#endif
    for (n = 0; n < 256; n++) {
            __VECTOR_RAM[n] = (unsigned long)undef_int;                  // set undefined interrupt for all interrupts at start
    }
    
#if !defined _COMPILE_IAR && !defined _WINDOWS
    mcf5xxx_wr_vbr((unsigned long)__VECTOR_RAM);                         // move vector to RAM position
    #if defined EXTERNAL_RAM && defined _M5225X                          // {107} when using external RAM configure the external memory interface ready to copy teh initialised variables to
    fnConfigureBus(0, 2);                                                // configure the FlexBus interface accordingly with 1:2 clocking and 2 wait states
    #endif

    if (__DATA_ROM != __DATA_RAM) {                                      // move initialized data from ROM to RAM
		dp = (unsigned char *)__DATA_RAM;
		sp = (unsigned char *)__DATA_ROM;
		n = (unsigned long)(__DATA_END - __DATA_RAM);
        while (n--) {
			*dp++ = *sp++;
        }
    }

    if (__BSS_START != __BSS_END) {                                      // zero uninitialised data
        sp = (unsigned char *)__BSS_START;
        n = (unsigned long)(__BSS_END - __BSS_START);
        while (n--) {
            *sp++ = 0;
        }
    }
#endif
}
#endif

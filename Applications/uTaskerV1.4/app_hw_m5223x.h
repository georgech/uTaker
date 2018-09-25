/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      app_hw_m5223x.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    26.02.2007 Added SMTP LOGIN defines                                  {1}
    26.02.2007 Add TFTP message contents                                 {2}
    17.02.2007 Move Watchdog disable input from IRQ3 to IRQ4 for M52233DEMO board compatibility {3}
    15.05.2007 Add Hardware Timer test configuration and MILLISEC define {4}
    09.06.2007 New interrupt definition table added                      {5}
    11.08.2007 Add RTC configuration                                     {6}
    25.08.2007 Add additional UART pin configuration options             {7}
    28.08.2007 Add SPI FLASH configuration                               {8}    
    04.09.2007 Remove conditional use of DMA_MEMCPY_SET on SPI_SW_UPLOAD {9}
    15.09.2007 Add configuration for M5222X
    15.09.2007 Add IRQ8..IRQ15 level/priority settings                   {10}
    09.10.2007 Add LCD delay                                             {11}
    13.10.2007 Add USB Interrupt priority (for M5222X/M5221X)            {12}
    13.10.2007 Add CHARGE_PUMP_I2C_CHANNEL / OTG_CHARGE_PUMP_I2C_ADD to define USB OTG charge pump interface {13}
    17.11.2007 Allow SPI and FLASH files system to work together         {14}
    12.12.2007 Add additional I2C pin configuration options              {15}
    29.12.2007 Add DMA HW timer channel configuration                    {16}
    08.02.2008 Add USB user configuration (later removed to config.h)    {17}
    22.02.2008 Add support for SPI FLASH STM25P64                        {18}
    26.03.2008 Modifications for M52220DEMO                              {19}
    31.03.2008 Add improved PLL configuration and control                {20}
    10.04.2008 Add ADC_ERR_PRIORITY, ADC_REFERENCE_VOLTAGE, SUPPORT_ADC  {21}
    19.04.2008 Add exact chip type                                       {22}
    19.04.2008 Add power control to HW timer                             {23}
    20.04.2008 Add PIT1 interrupt priority                               {23}
    21.04.2008 Add SUPPORT_EPORT to enable EPORT code and SUPPORT_PIT1 for PIT1 timer {24}
    21.04.2008 Add SUPPORT_DMA_TIMER to enable DMA timer code            {25}
    21.04.2008 Add DMA_TIMER1_INTERRUPT_PRIORITY                         {26}
    27.04.2008 Add CONFIG_TEST_OUTPUT() and TOGGLE_TEST_OUTPUT()         {27}
    28.04.2008 Add ADC initial analogue values for when simulating       {28}
    22.05.2008 Improved FLASH and SRAM size configuration                {29}
    23.05.2008 M52211 and M52213 PLL disabled to achieve 48MHz           {30}
    23.05.2008 Improve ATMEL SPI FLASH type configuration                {31}
    24.05.2008 Add define _ALIGN_HEAP_4 to align all heap memory - ensuring access efficiency {32}
    07.06.2008 Add second I2C interface to M5221X devices                {33}
    09.06.2008 Add PARAMETER_BLOCK_START address and modify file system start accordingly {34}
    09.06.2008 Restict number of UARTs for certain chips                 {35}
    28.06.2008 Adjust SPI FLASH file size to respect large granularity devices {36}
    28.06.2008 Add SST SPI FLASH support                                 {37}
    13.08.2008 Add M5225X device configuration support                   {38}
    16.08.2008 Add USB firmware upload file name                         {39}
    14.09.2008 Enable parameter and flash routines when parameter system is enabled without file system support {40}
    17.09.2008 Automatically enable PIT1 when USB HOST mode is used      {41}
    25.10.2008 Adapted SST SPI FLASH configuration for final implementation {42}
    29.10.2008 Add general purpose timer support                         {43}
    31.10.2008 Add MODBUS configuration                                  {44}
    19.01.2009 Add Kirin3 configurations                                 {45}
    07.03.2009 Add USB_BDT_FIXED                                         {46}
    15.03.2009 Add relaxation and external oscillator configurations     {47}
    25.03.2009 Add M521XX configuration                                  {48}
    26.03.2009 Add M521X configuration (replacing _HW_M5213)             {49}
    17.04.2009 Change to extended UART/COM mapping                       {50}
    10.04.2009 Add DMA_TIMER2_INTERRUPT_PRIORITY                         {51}
    12.04.2009 Change watchdog input to use IRQ register in some configurations {52}
    12.04.2009 Integrate use of port macros                              {53}
    25.08.2009 Add directly connected key scan (non-matrix)              {54}
    22.09.2009 Move "../../Hardware/M5223X/M5223X.h" include from types.h to this file, after detailed chip defines {55}
    02.10.2009 Add SIZE_OF_EXTERNAL_RAM for use when variables are positioned in external memory with the M5225X {56}
    07.10.2009 Add GLCD configuration for Samsung based GLCD             {57}
    15.10.2009 Add additional ST SPI FLASH devices                       {58}
    16.10.2009 Add NUMBER_EXTERNAL_SERIAL when serial interface active   {59}
    17.10.2009 Start SPI FLASH file system directly after FLASH size     {60}
    01.11.2009 Add dummy TICK_INTERRUPT() for compatibility              {61}
    20.12.2009 Add SD-card interface                                     {62}
    06.02.2010 Adjust USB_OTG_INTERRUPT_PRIORITY to avoid being same as Ethernet Rx on M5225x {63}
    08.02.2010 Add CS_SD_LINE for SD card chip select to allow working together with SPI FLASH {64}
    15.02.2010 Add M52259 Tower configuration                            {65}
    22.02.2010 Correct SPI_FLASH_AT45DB161, SPI_FLASH_AT45DB321 and SPI_FLASH_AT45DB642 sizes {66}
    25.03.2010 Correct external extended RAM to 512k                     {67}
    10.04.2010 Add M5214, M5216, M5280, M5281, M5282                     {68}
    19.05.2010 Add M520X                                                 {69}
    25.09.2010 Add Tower LCD support                                     {70}
    21.02.2011 Add INITIALISE_MONITOR_TIMER() and EXECUTION_DURATION()   {71}
    28.02.2011 Add GET_SDCARD_WP_STATE()                                 {72}
    17.06.2011 Add M5232, M5233, M5234, M5235 and M5235BCC               {73}
    25.07.2011 Correct M52210DEMO processor name                         {74}
    14.09.2011 Add M523X SDRAM configuration                             {75}
    24.09.2011 Control of use of HW random number generation moved from M5223X.h to this configuration file {76}
    11.02.2012 Add M52211EVB and M52259EVB configurations for HID mouse keys {77}
    02.04.2012 Add T_CHECK_CARD_REMOVAL                                  {78}
    12.08.2012 Add SD card detection input                               {79}
    20.01.2013 Option to remove ST SPI flash protection by default       {80}
    12.10.2013 Add support for magic reset Ethernet reception frame      {81}

    Application specific hardware configuration
*/

#if defined (_M5223X) && !defined (__APP_HW_M5223X__)
#define __APP_HW_M5223X__

#define _ALIGN_HEAP_4                                                    // {32} all heap blocks are aligned

#if defined (M52233DEMO)
    #define CHIP_80_PIN                                                  // the 80 pin device has various non-connected ports
    #define _M52233    
#elif defined (M52235EVB)                                                // larger device has no restrictions
    #define _M52235
#elif defined M52223EVB
    #define CHIP_100_PIN
    #define _M52223    
#elif defined (M52221DEMO)
    #define CHIP_64_PIN
    #define _M52221     
#elif defined (M52210DEMO)                                               // {19}
    #define CHIP_64_PIN
    #define _M52210                                                      // {74}
#elif defined (_M52212)
    #define CHIP_64_PIN
#elif defined (_M52213)
    #define CHIP_64_PIN
#elif defined M52211EVB
    #define CHIP_100_PIN
    #define _M52221
#elif defined M52110BOARD
    #define CHIP_64_PIN
    #define _M52110
#elif defined M5213EVB                                                   // {49}
    #define CHIP_100_PIN
    #define _M5213
#elif defined M52259EVB || defined M52259DEMO || defined M52259_TOWER    // {38}{65}
#define CHIP_100_PIN
  //#define CHIP_144_PIN
    #define _M52259
#elif defined M5282EVB || defined M5282LITE                              // {68}
    #define CHIP_256_PIN
    #define _M5282                                                       // used also for evaluation of M5214 and M5216
#elif defined M5208EVB || defined M5282CU5                               // {69}
    #define CHIP_196_PIN                                                 // 196 MAPBGA
    #define _M5208
#elif defined M5235BCC                                                   // {73}
  //#define CHIP_160_PIN                                                 // 160 QFP
  //#define CHIP_196_PIN                                                 // 196 MAPBGA (else 256 MAPBGA)
    #define _M5235
#else                                                                    // configuration for other variations
    #define CHIP_64_PIN
  //#define CHIP_81_PIN                                                  // MAPBGA version
  //#define CHIP_256_PIN                                                 // 256 MAPBGA version
    #define CHIP_100_PIN
  //#define CHIP_144_PIN                                                 // various M5225x housings {38}
  //#define CHIP_160_PIN                                                 // 160 QFP
  //#define CHIP_196_PIN                                                 // 196 MAPBGA

  //#define _M5208
  //#define _M5211
  //#define _M5212
  //#define _M5213
  //#define _M5214                                                       // 256 MAPBGA
  //#define _M5216                                                       // 256 MAPBGA
  //#define _M5232                                                       // 196 MAPBGA ot 160 QFP
  //#define _M5233                                                       // 256 MAPBGA
  //#define _M5234                                                       // 256 MAPBGA
  //#define _M5280                                                       // 256 MAPBGA
  //#define _M5281                                                       // 256 MAPBGA - no internal FLASH
  //#define _M5282                                                       // 256 MAPBGA
  //#define _M5207

  //#define _M52100    
  //#define _M52110
  //#define _M52210                                                      // {22}
  //#define _M52211
  //#define _M52212
  //#define _M52213
  //#define _M52221 
  //#define _M52223
  //#define _M52230 
  //#define _M52231
  //#define _M52232
  //#define _M52233
  //#define _M52234
    #define _M52235
  //#define _M52236
  //#define _M52252                                                      // {38}
  //#define _M52254
  //#define _M52255
  //#define _M52256
  //#define _M52258
  //#define _M52259
#endif


#if defined _M52235 || defined _M52255 || defined _M52259                // {76} enable HW random number support if the M52235/M52255 or M52259 is specified
    #define RND_HW_SUPPORT                                               // enable the use of the hardware resources in this chip
#endif

#include "../../Hardware/Coldfire/M5223X.h"                              // {55}

// Memory set up for this target
//
#define FLASH_START_ADDRESS (0x00000000)
#define START_OF_FLASH FLASH_START_ADDRESS
#if defined _M520X                                                       // {69}
    #define START_OF_SRAM  (0x80000000)
#else
    #define START_OF_SRAM  (0x20000000)
#endif
#define BOOT_CODE_SIZE 0x1000
#define SIZE_OF_EXTERNAL_RAM (512*1024)                                  // {56}{67}

// Internal FLASH size                                                   // {29}
//
#if defined _M52210 || defined _M52212 || defined _M52100
    #define SIZE_OF_FLASH              (64 * 1024)
#elif defined _M52211 || defined _M52213 || defined _M52221 || defined _M52230 || defined _M52231 || defined _M52232 || defined _M52110 || defined _M5211
    #define SIZE_OF_FLASH              (128 * 1024)
#elif defined _M52233 || defined _M52234 || defined _M52235 || defined _M52236 || defined _M52252 || defined _M52256 || defined _M5212 || defined _M5213
    #define SIZE_OF_FLASH              (256*1024)
#else
    #define SIZE_OF_FLASH              (512 * 1024)                      // {38}
#endif

// Internal RAM size                                                     // {29}
//
#if defined _M52212 || defined _M52213
    #define SIZE_OF_RAM                (8*1024)
#elif defined _M52210 || defined _M52211 || defined _M52221 || defined _M52100 || defined _M52110 || defined _M5211 || defined _M520X // {69}
    #define SIZE_OF_RAM                (16*1024)
#elif defined _M52230 || defined _M52231 || defined _M52232 || defined _M52233 || defined _M52234 || defined _M52235 || defined _M52236 || defined _M52252 || defined _M52256 || defined _M5212 || defined _M5213
    #define SIZE_OF_RAM                (32*1024)
#else
    #define SIZE_OF_RAM                (64*1024)                         // {38}
#endif

#if defined _M52212 || defined _M52213                                   // {30} maximum 50MHz so run from crystal (assuming USB required)
    #define DISABLE_PLL
    #define CRYSTAL_FREQ                       48000000                  // 48 MHz oscillator / crystal input
#else
  //#define RELAXATION_OSCILLATOR                                        // {47} use relaxation oscillator rather than clock
  //#define EXTERNAL_OSCILLATOR                                          // {47}
  //#define DISABLE_PLL                                                  // drive from oscillator input / crystal frequency without using the PLL
    #if defined DISABLE_PLL
        #if !defined RELAXATION_OSCILLATOR                                    // {47}
            #if defined M52259EVB
                #define CRYSTAL_FREQ           48000000                  // 48 MHz oscillator / crystal input
            #else
                #define CRYSTAL_FREQ           25000000                  // 25 MHz oscillator / crystal input
            #endif
        #endif
    #endif
#endif
#if defined _M5221X || defined _M521XX || defined _M5225X                // {38}{48}
    #define RTC_32K_CRYSTAL                    32768                     // run RTC from this crystal frequency (disable to run from system clock) 
#endif

#if !defined DISABLE_PLL
    #if defined _M5221X || defined _M521XX || defined _M5222X || defined _M5225X || defined _M521X || defined _M523X // {20}{38}{48}{49}{73}
        #if defined RELAXATION_OSCILLATOR                                // {47} relaxation oscillator rather than clock
            #define CRYSTAL_FREQ               RELAXATION_OSCILLATOR_FREQ// 8 MHz
            #define PRE_DIVIDER                1
            #define OSCCLK                     (CRYSTAL_FREQ / PRE_DIVIDER)
            #define PLL_MUL                    10                        // 80MHz operation
        #else
            #if defined _M521X
                #define CRYSTAL_FREQ           8000000                   // 8 MHz oscillator / crystal input
                #define OSCCLK                 CRYSTAL_FREQ
                #define PLL_MUL                10                        // 80MHz operation
            #elif defined _M523X                                         // {73}
                #define CRYSTAL_FREQ           25000000                  // 25 MHz oscillator / crystal input
                #define OSCCLK                 CRYSTAL_FREQ
                #define PLL_MUL                4                         // 100MHz operation (up to 150MHz should be possible)
            #elif defined M52259EVB
                #define CRYSTAL_FREQ           25000000                  // 25 MHz oscillator
                #define PRE_DIVIDER            5                         // 5MHz input
                #define OSCCLK                 (CRYSTAL_FREQ / PRE_DIVIDER)
                #define PLL_MUL                16                        // 80MHz operation 
            #else
                #define CRYSTAL_FREQ           48000000                  // 48 MHz oscillator / crystal input
                #define PRE_DIVIDER            6
                #define OSCCLK                 (CRYSTAL_FREQ / PRE_DIVIDER)
                #define PLL_MUL                10                        // 80MHz operation
            #endif
        #endif
    #elif defined _M52XX_SDRAM                                           // {68}
        #define CRYSTAL_FREQ                   8000000                   // 8 MHz oscillator / crystal input
        #define OSCCLK                         CRYSTAL_FREQ
        #define PLL_MUL                        10                        // 80MHz operation
    #elif defined _M520X                                                 // {69}
        #define CRYSTAL_FREQ                   16000000                  // must be 16.000 or 16.667MHz - the PLL setting depends on configuration (see FORCE_D9_D1)
    #else                                                                // M5223X Ethernet family
        #define CRYSTAL_FREQ                   25000000                  // 25 MHz oscillator / crystal input
        #define PRE_DIVIDER                    5                         // warning: in first silicon this cannot be changed!
        #define OSCCLK                         (CRYSTAL_FREQ / PRE_DIVIDER)
        #define PLL_MUL                        12                        // 60MHz operation
    #endif

    #if !defined _M520X
        #if defined _M523X
            #if OSCCLK < 8000000 || OSCCLK > 25000000                    // the PLL requires an input in the 8..25MHz range
                #error Clock frequency out of range: must be between 2 and 10MHz
            #endif
            #define PLL_OUTPUT_FREQ_INT                (OSCCLK * PLL_MUL)
        #else
            #if OSCCLK < 2000000 || OSCCLK > 10000000                    // the PLL requires an input in the 2..10MHz range
                #error Clock frequency out of range: must be between 2 and 10MHz
            #endif

            #define PLL_OUTPUT_FREQ_INT                (OSCCLK * PLL_MUL)
                                                                             // rules for setting the frequency

            #if PLL_MUL != 4 && PLL_MUL != 6 && PLL_MUL != 8 && PLL_MUL != 10 && PLL_MUL != 12 && PLL_MUL != 14 && PLL_MUL != 16 && PLL_MUL != 18
                #error Multiplication value must be 4,6,8,10,12,14,16 or 18
            #endif
            #if defined PRE_DIVIDER
                #if PRE_DIVIDER < 1 || PRE_DIVIDER > 8
                    #error Pre-divider value must be 1,2,3,4,5,6,7 or 8
                #endif
            #endif
        #endif

        #if PLL_OUTPUT_FREQ_INT > MAX_PLL_FREQUENCY
            #error PLL frequency too high!!
        #endif
    #endif
#else
    #if defined RELAXATION_OSCILLATOR                                         // {47}
        #define OSCCLK                         RELAXATION_OSCILLATOR_FREQ
        #define PLL_OUTPUT_FREQ_INT            RELAXATION_OSCILLATOR_FREQ
    #else
        #define OSCCLK                         CRYSTAL_FREQ
        #define PLL_OUTPUT_FREQ_INT            CRYSTAL_FREQ
    #endif
#endif

#define POST_DIV                           1                             // generally this is left at 1

#if POST_DIV == 1
    #define POST_DIVIDE 0
#elif POST_DIV == 2
    #define POST_DIVIDE 1
#elif POST_DIV == 4
    #define POST_DIVIDE 2
#elif POST_DIV == 8
    #define POST_DIVIDE 3
#elif POST_DIV == 16
    #define POST_DIVIDE 4
#elif POST_DIV == 32
    #define POST_DIVIDE 5
#elif POST_DIV == 64
    #define POST_DIVIDE 6
#elif POST_DIV == 128
    #define POST_DIVIDE 7
#else
    #error Post-divide value must be 1,2,4,8,16,32,64 or 128
#endif



// SPI FLASH system setup
//
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // {14}
    #define uFILE_START 0x0 
    #define EEPROM_32K                                                   // device size used
#else
    #define FILES_ADDRESSABLE                                            // file system is addressable (doesn't have to be retrieved)
#endif
#if defined EEPROM_16K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device
    #define FILE_GRANULARITY (1024)                                      // file made up of 1k byte FLASH blocks
    #define FILE_SYSTEM_SIZE (16*FILE_GRANULARITY)                       // 16k reserved for file system (including parameter blocks)
#elif defined EEPROM_32K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device
    #define FILE_GRANULARITY (2 *1024)                                   // file made up of 2k byte FLASH blocks
    #define FILE_SYSTEM_SIZE (16*FILE_GRANULARITY)                       // 32k reserved for file system (including parameter blocks)
#elif defined EEPROM_64K
    #define EEPROM_PAGE_SIZE 128                                         // respect larger page size in SPI EEPROM
    #define FILE_GRANULARITY (2 * 1024)                                  // file made up of 2k byte FLASH blocks
    #define FILE_SYSTEM_SIZE (32 * FILE_GRANULARITY)                     // 64k reserved for file system (including parameter blocks)
#endif

//#define SPI_FLASH_MULTIPLE_CHIPS                                       // activate when multiple physical chips are used
#define CS0_LINE            PORT_QS_BIT5                                 // CS0 line used when SPI FLASH is enabled
#define CS1_LINE            PORT_QS_BIT4                                 // CS1 line used when extended SPI FLASH is enabled
#define CS2_LINE            PORT_QS_BIT5                                 // CS2 line used when extended SPI FLASH is enabled
#define CS3_LINE            PORT_QS_BIT6                                 // CS3 line used when extended SPI FLASH is enabled
#define SPI_CS0_PORT        PORTQS
#define SPI_CS1_PORT        PORTQS
#define SPI_CS2_PORT        PORTQS
#define SPI_CS3_PORT        PORTQS
                                                                         // {8}
#if defined SPI_FLASH_ST                                                 // {18} ST SPI FLASH used
  //#define SPI_FLASH_STM25P05                                           // the available ST chips
  //#define SPI_FLASH_STM25P10
  //#define SPI_FLASH_STM25P20
    #define SPI_FLASH_STM25P40
  //#define SPI_FLASH_STM25P80                                               
  //#define SPI_FLASH_STM25P16                                               
  //#define SPI_FLASH_STM25P32                                               
  //#define SPI_FLASH_STM25P64
  //#define SPI_FLASH_STM25P128

  //#define REMOVE_SPI_FLASH_PROTECTION                                  // {80} automatically remove SPI flash protection when initialising

    #if defined SPI_FLASH_STM25P05                                       // 64k {58}
        #define SPI_FLASH_PAGES             (1 * 256)                          
    #elif defined SPI_FLASH_STM25P10                                     // 128k
        #define SPI_FLASH_PAGES             (2 * 256)
    #elif defined SPI_FLASH_STM25P20                                     // 256k
        #define SPI_FLASH_PAGES             (4 * 256)
    #elif defined SPI_FLASH_STM25P40                                     // 512k
        #define SPI_FLASH_PAGES             (8 * 256)
    #elif defined SPI_FLASH_STM25P80                                     // 1M
        #define SPI_FLASH_PAGES             (16 * 256)
    #elif defined SPI_FLASH_STM25P16                                     // 2M
        #define SPI_FLASH_PAGES             (32 * 256)
    #elif defined SPI_FLASH_STM25P32                                     // 4M
        #define SPI_FLASH_PAGES             (64 * 256)
    #elif defined SPI_FLASH_STM25P64                                     // 8M
        #define SPI_FLASH_PAGES             (128 * 256)
    #elif defined SPI_FLASH_STM25P128                                    // 16M
        #define SPI_FLASH_PAGES             (256 * 256)
    #endif

    #define SPI_FLASH_PAGE_LENGTH 256
    #if defined SPI_DATA_FLASH
        #define SPI_FLASH_SECTOR_LENGTH     (16 * SPI_FLASH_PAGE_LENGTH) // sub-sector size of data FLASH
    #else
        #define SPI_FLASH_SECTOR_LENGTH     (256 * SPI_FLASH_PAGE_LENGTH)// sector size of code FLASH
    #endif
    #define SPI_FLASH_BLOCK_LENGTH          SPI_FLASH_SECTOR_LENGTH
#elif defined SPI_FLASH_SST25                                            // {37}{42}
  //#define SPI_FLASH_SST25VF010A                                        // the supported SST chips  
  //#define SPI_FLASH_SST25LF020A
  //#define SPI_FLASH_SST25LF040A
  //#define SPI_FLASH_SST25VF040B                                             
  //#define SPI_FLASH_SST25VF080B                                               
  //#define SPI_FLASH_SST25VF016B                                               
    #define SPI_FLASH_SST25VF032B                                               

    #if defined SPI_FLASH_SST25VF010A                                    // 1Mbit (128k)
        #define SST25_A_VERSION
        #define SPI_FLASH_PAGES             (32)
    #elif defined SPI_FLASH_SST25LF020A                                  // 2Mbit (256k)
        #define SST25_A_VERSION
        #define SPI_FLASH_PAGES             (64)
    #elif defined SPI_FLASH_SST25LF040A                                  // 4Mbit (512k)
        #define SST25_A_VERSION
        #define SPI_FLASH_PAGES             (128)
    #elif defined SPI_FLASH_SST25VF040B                                  // 4Mbit (512k)
        #define SPI_FLASH_PAGES             (128)
    #elif defined SPI_FLASH_SST25VF080B                                  // 8Mbit (1M)
        #define SPI_FLASH_PAGES             (256)
    #elif defined SPI_FLASH_SST25VF016B                                  // 16Mbit (2M)
        #define SPI_FLASH_PAGES             (512)
    #elif defined SPI_FLASH_SST25VF032B                                  // 32Mbit (4M)
        #define SPI_FLASH_PAGES             (1024)
    #endif

    #define SPI_FLASH_SUB_SECTOR_LENGTH  (4 * 1024)                      // sub-sector size of SPI FLASH
    #define SPI_FLASH_HALF_SECTOR_LENGTH (32 * 1024)                     // half-sector size of SPI FLASH
    #define SPI_FLASH_SECTOR_LENGTH      (64 * 1024)                     // sector size of SPI FLASH

    #define SPI_FLASH_PAGE_LENGTH        SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - smallest erasable block
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - file system granularity
#else
  //#define SPI_FLASH_AT45DB011                                          // {31} define the ATMEL type used here
  //#define SPI_FLASH_AT45DB021
    #define SPI_FLASH_AT45DB041
  //#define SPI_FLASH_AT45DB081
  //#define SPI_FLASH_AT45DB161
  //#define SPI_FLASH_AT45DB321
  //#define SPI_FLASH_AT45DB642
    #define SPI_FLASH_AT45XXXXD_TYPE                                     // specify that a D-type rather than a B type is being used

    #if defined SPI_FLASH_AT45DB642                                      // define whether used in power of 2 mode or not
        #define SPI_FLASH_PAGE_LENGTH 1056                               // standard page size (B-device only allows 1056)
      //#define SPI_FLASH_PAGE_LENGTH 1024                               // size when power of 2 mode selected (only possible on D-device) WARNING: this mode will be automatically programmed to the SPI Flash and cannot be reversed!!
        #define SPI_FLASH_PAGES             (8 * 1024)                   // 8Meg part {66}
    #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
        #define SPI_FLASH_PAGE_LENGTH 528                                // standard page size (B-device only allows 528)
      //#define SPI_FLASH_PAGE_LENGTH 512                                // size when power of 2 mode selected (only possible on D-device) WARNING: this mode will be automatically programmed to the SPI Flash and cannot be reversed!!
        #if defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 2Meg part {66}
        #else
            #define SPI_FLASH_PAGES         (8 * 1024)                   // 4Meg part {66}
        #endif
    #else
        #define SPI_FLASH_PAGE_LENGTH 264                                // standard page size (B-device only allows 264)
      //#define SPI_FLASH_PAGE_LENGTH 256                                // size when power of 2 mode selected (only possible on D-device) WARNING: this mode will be automatically programmed to the SPI Flash and cannot be reversed!!
        #if defined SPI_FLASH_AT45DB011
            #define SPI_FLASH_PAGES         (512)                        // 128k part
        #elif defined SPI_FLASH_AT45DB021
            #define SPI_FLASH_PAGES         (1024)                       // 256k part
        #elif defined SPI_FLASH_AT45DB041
            #define SPI_FLASH_PAGES         (2 * 1024)                   // 512k part
        #elif defined SPI_FLASH_AT45DB081
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 1Meg part
        #endif
    #endif

    #define SPI_FLASH_BLOCK_LENGTH (8 * SPI_FLASH_PAGE_LENGTH)           // block size - a block can be deleted
    #define SPI_FLASH_SECTOR_LENGTH (64 * 4 * SPI_FLASH_PAGE_LENGTH)     // exception sector 0a is 2k and sector 0b is 62k
#endif

#define SPI_DATA_FLASH_0_SIZE   (SPI_FLASH_PAGES * SPI_FLASH_PAGE_LENGTH) 
#define SPI_DATA_FLASH_1_SIZE   0 //SPI_DATA_FLASH_0_SIZE 
#define SPI_DATA_FLASH_2_SIZE   0 //SPI_DATA_FLASH_0_SIZE
#define SPI_DATA_FLASH_3_SIZE   0 //SPI_DATA_FLASH_0_SIZE
#define SPI_DATA_FLASH_4_SIZE   0 //SPI_DATA_FLASH_0_SIZE
#define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE + SPI_DATA_FLASH_3_SIZE + SPI_DATA_FLASH_4_SIZE)
#if defined SPI_FLASH_MULTIPLE_CHIPS
    #define SPI_FLASH_DEVICE_COUNT  4                                    // only define QSPI_X_LINE for this amount of devices
#endif

#define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH)     // SPI FLASH starts immediately after FLASH
#define SW_UPLOAD_FILE()       (*ucIp_Data == 'H') && (*(ucIp_Data+1) == 'S') && (*(ucIp_Data+2) == '.')  && (fnSPI_Flash_available() != 0)

#if defined _M5225X
    #define USB_DOWNLOAD_FILE       "z"                                  // {39}
#else
    #define USB_DOWNLOAD_FILE       "0"
#endif

// FLASH based File System setup
//
#if defined FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM                                          // this is a test setup for external SPI FLASH, with the parameters at the end of internal FLASH
        #define PARAMETER_BLOCK_START (SIZE_OF_FLASH - PAR_BLOCK_SIZE)   // {34} FLASH location at the parameter block size short of end of internal FLASH
        #define uFILE_START            SIZE_OF_FLASH                     // {60} immediately after internal FLASH

        #if FILE_GRANULARITY < 20000                                     // {36}
            #if SPI_DATA_FLASH_SIZE < (64 * FILE_GRANULARITY)            // {42} allow testing smallest SPI FLASH
                #define FILE_SYSTEM_SIZE  SPI_DATA_FLASH_SIZE
            #else
                #define FILE_SYSTEM_SIZE (64 * FILE_GRANULARITY)         // 1Meg reserved for file system (assuming 8k file size) {34} [PAR_BLOCK_SIZE removed]
            #endif
        #else
            #define FILE_SYSTEM_SIZE (4 * FILE_GRANULARITY)              // 0.5Meg reserved for file system
        #endif
    #else                                                                // this is the normal set up for internal FLASH
        #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)                 // each file a multiple of 2k (4k kirin3 devices)
        #if defined M52210DEMO                                           // {19}
            #define PARAMETER_BLOCK_START 0xe000                         // {34} FLASH location at 56k start
            #define uFILE_START 0xf000                                   // FLASH location at 60k start
            #define FILE_SYSTEM_SIZE (4 * FILE_GRANULARITY)              // 8k reserved for file system
        #elif defined M52211EVB || defined M52221DEMO                    // 64k file system to allow USB SW upload to be tested
            #define PARAMETER_BLOCK_START 0xf000                         // FLASH location at 60k start
            #define uFILE_START 0x10000                                  // FLASH location at 64k start
            #define FILE_SYSTEM_SIZE (32 * FILE_GRANULARITY)             // 64k reserved for file system
        #elif defined M52259DEMO || defined M52259EVB || defined M52259_TOWER || defined M5282EVB || defined M5282LITE || defined M5208EVB || defined M5282CU5
            #if defined M52259EVB && defined NVRAM                       // file system is in NVRAM
                #define SIZE_OF_NVRAM (256 * 1024)
                #define PARAMETER_BLOCK_START (SIZE_OF_FLASH - PAR_BLOCK_SIZE) // parameters at end of internal FLASH
                #define uFILE_START SIZE_OF_FLASH                        // NVRAM located after internal FLASH
                #define FILE_SYSTEM_SIZE (SIZE_OF_NVRAM)                 // 256k reserved for file system
            #else
                #if defined I2C_EEPROM_FILE_SYSTEM                       // test case using external I2C EEPROM
                    #define PARAMETER_BLOCK_START 0x2e000                // FLASH location at 184k start
                    #define uFILE_START 0x30000                          // FLASH location at 192k start
                    #define FILE_SYSTEM_SIZE (16 * FILE_GRANULARITY)     // 64k reserved for default file system

                    #define EXTENSION_FILE_COUNT_VARIABLE                // variable number of devices (detected at initialisation)
                    #define I2C_EEPROM_DEVICE_SIZE   (128 * 1024)        // each optional I2C EEPROM has 128k memory
                    #define _M24M01_0   0xa0                             // first optional EEPROM address
                    #define _M24M01_1   0xa4                             // second optional EEPROM address
                    #define _M24M01_COUNT 2                              // up to 2 extension devices supported
                    #define I2C_EEPROM_MEMORY_SIZE   (_M24M01_COUNT * I2C_EEPROM_DEVICE_SIZE) // total I2C EEPROM memory area size
                    #define I2C_EEPROM_START_ADDRESS 0x40000             // start address fo the I2C EEPROM memory area
                    #define EXTENSION_FILE_COUNT (I2C_EEPROM_MEMORY_SIZE / EXTENSION_FILE_SIZE) // the number of files of file granularity size that are supported                    
                #else
                    #define PARAMETER_BLOCK_START 0x21000                // {45} FLASH location at 132k start
                    #define uFILE_START 0x23000                          // FLASH location at 140k start
                    #define FILE_SYSTEM_SIZE (93*FILE_GRANULARITY)       // 374k reserved for file system
                #endif
            #endif
        #elif defined M5235BCC
            #define FLASH_GRANULARITY (64 * 1024)                        // external flash granularity defines
            #define PARAMETER_BLOCK_START (EXTERNAL_FLASH_START_ADDRESS + 0x30000)
            #define uFILE_START      (PARAMETER_BLOCK_START + (2 * FLASH_GRANULARITY))
            #define FILE_SYSTEM_SIZE (20 * FILE_GRANULARITY)             // 1280k reserved for file system            
        #else
            #define PARAMETER_BLOCK_START 0x18000                        // {34} FLASH location at 96k start
            #define uFILE_START 0x19000                                  // FLASH location at 100k start
            #define FILE_SYSTEM_SIZE (78 * FILE_GRANULARITY)             // 156k reserved for file system
        #endif
    #endif
#else
    #define PARAMETER_BLOCK_START 0x3f000                                // {40} FLASH location at 2 parameter blocks short of end of internal FLASH (suitable for 25k FLASH size)
#endif

#if defined USE_PARAMETER_BLOCK
    #define PARAMETER_BLOCK_SIZE    FLASH_GRANULARITY                    // use the smallest size for the device

    #if defined USE_PAR_SWAP_BLOCK 
        #define PAR_BLOCK_SIZE      (2 * PARAMETER_BLOCK_SIZE)
    #else
        #define PAR_BLOCK_SIZE      (1 * PARAMETER_BLOCK_SIZE)
    #endif
#else
    #define PAR_BLOCK_SIZE  (0)
#endif

#define SUPPORT_GENERAL_PURPOSE_TIMER                                    // {43}
#if defined SUPPORT_GENERAL_PURPOSE_TIMER
    #define GPT_EXTENDED_COUNTER                                         // extend the general purpose timer capture value to 32bits using overflow interrupt
    #define GPT_SIMPLE_INTERRUPT                                         // allow use a simple edge triggered interrupt
#endif
#if defined USE_MODBUS
    #define _TIMER_INTERRUPT_SETUP    DMA_TIMER_SETUP
    #define SUPPORT_DMA_TIMER                                            // {44} always activate DMA timer support for use by MODBUS
    #define MODBUS0_DMA_TIMER_CHANNEL              3                     // use DMA timer channel 3 for first MODBUS serial port
    #define MODBUS0_DMA_TIMER_INTERRUPT_PRIORITY   DTIM3_INTERRUPT_PRIORITY
    #define MODBUS1_DMA_TIMER_CHANNEL              1                      // use DMA timer channel 1 for second MODBUS serial port
    #define MODBUS1_DMA_TIMER_INTERRUPT_PRIORITY   DMA_TIMER1_INTERRUPT_PRIORITY
    #define MODBUS2_DMA_TIMER_CHANNEL              2                      // use DMA timer channel 2 for third MODBUS serial port
    #define MODBUS2_DMA_TIMER_INTERRUPT_PRIORITY   DMA_TIMER2_INTERRUPT_PRIORITY
#else
    #define SUPPORT_DMA_TIMER                                            // {25} include DMA timer code
#endif
//#define SUPPORT_PWM_MODULE
#if defined USB_HOST_SUPPORT                                             // {41}
    #define SUPPORT_PIT1                                                 // enable PIT1 support when USB host mode is supported to allow USB timing
    #define PULL_DOWN_15K() _CLEARBITS(QS, (PORT_QS_BIT5 | PORT_QS_BIT6)); DDRQS |= (PORT_QS_BIT5 | PORT_QS_BIT6) // QS5 and QS6 are used on the EVB to pull the D+ and D- with 15k resistors to ground
#else
    #define SUPPORT_PIT1                                                 // {24} include PIT1 timer code
#endif

//#define USB_BDT_FIXED                                                  // {46} used a fixed address in SRAM for the USB buffer descriptor table. The linker file needs to be set up accordingly
#define PROTECT_USB_TX_BUFFER                                            // use additional tx buffer protection to avoid ping-pong buffer overrun. Register status alone is not reliable.


#define SUPPORT_EPORT                                                    // {24} include EPORT code for interrupts IRQ1..IRQ15

#define SUPPORT_ADC                                                      // {21} enable ADC support
#define ADC_REFERENCE_VOLTAGE                      3300                  // {21} ADC uses 3.3V reference
#define ADC_SIM_STEP_SIZE                          200                   // 200mV steps when simulating

#if defined SUPPORT_ADC                                                  // {28}
    #define AN0_START_VOLTAGE                      33                    // initial voltages when simulating (mV)
    #define AN1_START_VOLTAGE                      500
    #define AN2_START_VOLTAGE                      1000
    #define AN3_START_VOLTAGE                      1500
    #define AN4_START_VOLTAGE                      2000
    #define AN5_START_VOLTAGE                      2500
    #define AN6_START_VOLTAGE                      3000
    #define AN7_START_VOLTAGE                      3300

  //#define SYNCA_ON_PAS                                                 // ADC SYNCA pin is configured on this port (not M5222X/M5221X)
    #define SYNCA_ON_PNQ                                                 // ADC SYNCA pin is configured on this port
  //#define SYNCA_ON_PQS                                                 // ADC SYNCA pin is configured on this port (not on 80/64 pin packages!!)
  //#define SYNCA_ON_PUB                                                 // ADC SYNCA pin is configured on this port
  //#define SYNCB_ON_PAS                                                 // ADC SYNCB pin is configured on this port (not M5222X/M5221X)
  //#define SYNCB_ON_PQS                                                 // ADC SYNCB pin is configured on this port (not M5222X or 80 pin packages!!)
    #define SYNCB_ON_PUB                                                 // ADC SYNCB pin is configured on this port
#endif

#define TICK_INTERRUPT()                                                 // {61} dummy user tick handler

#define SUPPORT_RTC                                                      // enable RTC support {6}
                                                                         // {71} use PIT1 as performance monitor timer (17us at 60MHz)
#define INITIALISE_MONITOR_TIMER()   POWER_UP(POWER_PIT1); PIT_PCSR_1 = (PIT_PRESCALE_1K | PIT_DBG | PIT_OVW | PIT_PIF | PIT_EN);
#define EXECUTION_DURATION()  (0xffff - PIT_PCNTR_1); PIT_PMR_1 = 0xffff;// read the elapsed count value and reset the counter back to 0xffff

// Serial interfaces
//
#if defined SERIAL_INTERFACE
    #define WELCOME_MESSAGE_UART   "\r\n\nHello, world... Coldfire V2 MCU\r\n"
    #define NUMBER_EXTERNAL_SERIAL 0                                     // {59}
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255 {50}
    #define SERIAL_PORT_0     4                                          // if we open UART channel 0 we simulate using comx on the PC
    #define SERIAL_PORT_1     6                                          // if we open UART channel 1 we simulate using comx on the PC
    #define SERIAL_PORT_2     8                                          // if we open UART channel 2 we simulate using comx on the PC
    #if defined M52210DEMO || defined M52211EVB
        #define DEMO_UART     1                                          // use UART 1 (the UART connector on the board)
        #define PPP_UART      0                                          // use UART 0 for PPP
        #define MODBUS_UART_0 0                                          // {44}
        #define MODBUS_UART_1 1                                          // {44}
    #else
        #if defined M52259_TOWER || defined M5208EVB                     // {65}{69}
            #define DEMO_UART 1                                          // use UART 1 - fixed for tower serial card
        #else
            #define DEMO_UART 0                                          // use UART 0
        #endif
        #define PPP_UART      1                                          // use UART 1 for PPP
        #define MODBUS_UART_0 1                                          // {44}
        #define MODBUS_UART_1 2                                          // {44}
        #define MODBUS_UART_2 0                                          // {44}
    #endif
    #define SERIAL_SUPPORT_DMA                                           // enable UART DMA support (available on this processor)

    #define TX_BUFFER_SIZE   (1536)                                      // the size of demo RS232 input and output buffers
    #define RX_BUFFER_SIZE   (64)

  //#define UART1_ON_QS                                                  // alternative UART pin mapping {7}
  //#define UART2_ON_AS                                                  // alternative UART pin mapping (set automatically with 64 and 80 pin package)
  //#define UART2_ON_UB                                                  // alternative UART pin mapping

    // Alternative UART mapping for M52XX
    //
  //#define UART2_TXON_AS_4                                              // (only M5214 or M5216)
  //#define UART2_TXON_AS_2                                              // UTXD2 default is otherwise on AS[0]
  //#define UART2_RXON_AS_5
  //#define UART2_RXON_AS_3                                              // URXD2 default is otherwise on AS[1]
#else
    #define TX_BUFFER_SIZE   (64)
    #define RX_BUFFER_SIZE   (64)
#endif

// I2C Interface
//
#if defined I2C_INTERFACE
    #define CHARGE_PUMP_I2C_CHANNEL 0                                    // {13}
    #define OTG_CHARGE_PUMP_I2C_ADD 0x58                                 // {13}
  //#define I2C_ON_QS                                                    // {15} alternative I2C pin mapping

    #if defined _M5221X || defined _M521XX || defined _M5225X            // {45}{48}
        #define NUMBER_I2C          2                                    // {33} define support for 2 I2C interfaces
        #define OUR_I2C_CHANNEL     0

      //#define I2C0_ON_QS                                               // alternative I2C0 pin mapping

      //#define I2C1_ON_QS                                               // alternative I2C1 pin mapping (default if no other defined)
      //#define I2C1_ON_UB                                               // alternative I2C1 pin mapping
        #define I2C1_ON_UC                                               // alternative I2C1 pin mapping
      //#define I2C1_ON_TH                                               // alternative I2C1 pin mapping - only on 144 pin package!
    #else
        #define NUMBER_I2C          1                                    // one I2C available
        #define OUR_I2C_CHANNEL     0
    #endif
#endif

#if defined SDCARD_SUPPORT                                               // {62}
    #define SDCARD_SIM_SIZE   SDCARD_SIZE_2G                             // the size of SD card when simulating
  //#define _NO_SD_CARD_INSERTED                                         // simulate no SD card inserted initially

    #define CS_SD_LINE   PORT_QS_BIT3                                    // {64}
    // Configure to suit SD card SPI mode at between 100k and 400k
    //
    // configure and drive CS line high 
    // set QSPI_SCK, QSPI_DIN and QSPI_DOUT on QS port
    // set mode and speed and finally initialise all 16 deep FIFOs
    #define INITIALISE_SPI_SD_INTERFACE() \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(QS, CS_SD_LINE, CS_SD_LINE); \
        PQSPAR |= (QS_QSPI_SCK | QS_QSPI_DIN | QS_QSPI_DOUT);    \
        QMR = ((QSPI_MSTR | QSPI_8BITS) | ((BUS_CLOCK/2/300000) & QSPI_BAUD_MASK)); \
        QAR = QSPI_COMMAND_RAM_0; \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); \
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0);


    #define SET_SD_CARD_MODE()
    #define ENABLE_SPI_SD_OPERATION()

    // Set maximum speed
    //
    #define SET_SPI_SD_INTERFACE_FULL_SPEED()  QMR = ((QSPI_MSTR | QSPI_8BITS) | ((BUS_CLOCK/2/8000000) & QSPI_BAUD_MASK));

    #if defined _WINDOWS
        #define WRITE_SPI_CMD(byte)    QAR = QSPI_TRANSMIT_RAM_0; QDR = (unsigned char)byte; QWR = ((0 << QSPI_START_SHIFT) | (0 << QSPI_END_SHIFT)); QDLYR = QSPI_SPE; QDR = _fnSimSD_write((unsigned char)QDR)
        #define WAIT_TRANSMISSON_END() while (!(QIR & QSPI_SPIF)) {QIR = QSPI_SPIF;}; QAR = QSPI_RECEIVE_RAM_0; 
    #else
        #define WRITE_SPI_CMD(byte)    QAR = QSPI_TRANSMIT_RAM_0; QDR = (unsigned char)byte; QWR = ((0 << QSPI_START_SHIFT) | (0 << QSPI_END_SHIFT)); QDLYR = QSPI_SPE
        #define WAIT_TRANSMISSON_END() while (!(QIR & QSPI_SPIF)) {}; QIR = QSPI_SPIF; QAR = QSPI_RECEIVE_RAM_0;
    #endif
    #define READ_SPI_DATA()            (unsigned char)QDR

    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate
    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface
    #define SET_SD_DI_CS_HIGH()  _SETBITS(QS, CS_SD_LINE)                // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(QS, CS_SD_LINE)              // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(QS, CS_SD_LINE)                // negate the CS line of the SD card to be read

    #define GET_SDCARD_WP_STATE() 0                                      // {72} write protection disabled (change to read switch is required)

    #define T_CHECK_CARD_REMOVAL    ((DELAY_LIMIT)(SEC * 10))            // {78} if the card has no detection switch it can be polled to detect removal
  //#define SDCARD_DETECT_INPUT_POLL                                     // {79} use card detect switch for detection polling (use together with T_CHECK_CARD_REMOVAL)
  //#define SDCARD_DETECT_INPUT_INTERRUPT                                // {79} use card detect switch for detection by interrupt (T_CHECK_CARD_REMOVAL and SDCARD_DETECT_INPUT_POLL should be disabled)
    #if defined SDCARD_DETECT_INPUT_INTERRUPT && !defined SUPPORT_PORT_INTERRUPTS
        #define SUPPORT_PORT_INTERRUPTS
    #endif

    #define SDCARD_DETECTION()   (!(EPPDR0 & PORT_NQ_BIT6))              // card detection input (GPIO left as IRQ6 default configuration)
    #define PRIORITY_SDCARD_DETECT_PORT_INT   INTERRUPT_LEVEL_6          // this interrupt priority can't actually be set since IRQs use fixed values
    #define SDCARD_DETECT_PIN    6                                       // use IRQ6 input
#endif


// LAN interface
//
#if defined ETH_INTERFACE
  //#define LAN_REPORT_ACTIVITY                                          // transmissions/receptions and errors are reported to chosen task (for link LED control, etc)

  //#define _MAGIC_RESET_FRAME        0x1234                             // {81} allow a magic Ethernet reception frame to reset the board to boot loader mode (Ethernet protocol used) followed by three repetitions of the same value (total frame length 61 bytes)

    #if defined LAN_REPORT_ACTIVITY
        #if defined _M5225X || defined M5282EVB || defined M5282LITE || defined M5208EVB || defined M5282CU5 || defined M5235BCC // {45}{68}{73} - no LEDs since external PHY controls them
            #define ACTIVITY_LED_OFF()     
            #define ACTIVITY_LED_ON()      
            #define TURN_LINK_LED_ON()     
            #define SPEED_LED_ON()         
            #define SPEED_LED_OFF()        
            #define LINK_DOWN_LEDS()       

            #define CONFIGURE_LAN_LEDS()   
        #else
            #define ACTIVITY_LED_OFF()     PORTLD |= LAN_ACTIVITY_LED;
            #define ACTIVITY_LED_ON()      PORTLD &= ~LAN_ACTIVITY_LED;
            #define TURN_LINK_LED_ON()     PORTLD &= ~(LAN_LINK_LED);
            #define SPEED_LED_ON()         PORTLD &= ~(LAN_SPEED_LED);
            #define SPEED_LED_OFF()        PORTLD |=  (LAN_SPEED_LED);
            #define LINK_DOWN_LEDS()       PORTLD = (PORT_LD_BIT0 | PORT_LD_BIT1 | PORT_LD_BIT2 | PORT_LD_BIT3 | PORT_LD_BIT4 | PORT_LD_BIT5 | PORT_LD_BIT6);

            #define CONFIGURE_LAN_LEDS()   DDRLD = (PORT_LD_BIT0 | PORT_LD_BIT1 | PORT_LD_BIT2 | PORT_LD_BIT3 | PORT_LD_BIT4 | PORT_LD_BIT5 | PORT_LD_BIT6); \
                                           LINK_DOWN_LEDS();
        #endif
    #endif
#endif

#if defined USE_IP
    #define LAN_BUFFER_SIZE           1536                               // M5223X has ample space for full tx buffer
#else
    #define LAN_BUFFER_SIZE           256                                // if using Ethernet without IP the buffer size can be set here
#endif

// Note that the maximum TCP payload size if 1460 when IPv4 is used alone. When IPv6 is in use it must be reduced to maximum 1440.
// If IPv6 6in4 tunnelling is used it must be reduced to maximum 1420
//
#if defined USE_BUFFERED_TCP                                             // if using a buffer for TCP to allow interactive data applications (like TELNET)
    #define TCP_BUFFER                2800                               // size of TCP buffer (with USE_BUFFERED_TCP) - generous with M5223X
    #define TCP_BUFFER_FRAME          1400                               // allow this max. TCP frame size
#endif

#if defined USE_HTTP
    #define HTTP_BUFFER_LENGTH        1400                               // we send frames with this maximum amount of payload data - generous with M5223X
#endif

#if defined USE_FTP                                                      // specify FTP support details
    #if defined _M5225X                                                  // {45}
        #define FTP_WELCOME_RESPONSE  "220 Welcome M5225X FTP.\r\n"
    #else
        #define FTP_WELCOME_RESPONSE  "220 Welcome M5223X FTP.\r\n"
    #endif
    //                                        ^^^^^^^^^^^^^^^^^^^^          customise the FTP welcome here
#endif

#define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE         2                // allocate 2 buffers for transmission
#define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE         6                // allocate 6 (full) buffers for reception
#define USE_MULTIPLE_BUFFERS                                             // construct the receive buffer space using multiple 256 byte buffers (improved overrun performance for total memory requirement but reduced efficiency due to extra copy)


// Special support for this processor type
//
#define DMA_MEMCPY_SET                                                   // {9} memcpy and memset functions performed by DMA (if supported by processor - uses one DMA channel)
//#define RUN_LOOPS_IN_RAM                                               // allow certain routines with tight loops to run from SRAM where it is generally faster than from FLASH - uses slightly more SRAM

// Global Hardware Timer setup
//
#if defined GLOBAL_TIMER_TASK && defined GLOBAL_HARDWARE_TIMER           // {4}{16} resolutions assuming 60MHz clock
    #define MILLISEC               LOW_RES_MS                            // 1ms about 0% accurate ms values, 71us resolution and limit of about 81 hours max. It uses a 32 bit DMA timer
  //#define MILLISEC               MED_RES_MS                            // 1ms about 0% accurate ms values, 4us resolution and limit of about 305 minutes max. It uses a 32 bit DMA timer
  //#define MILLISEC               HIGH_RES_MS                           // 1ms about 0% accurate ms values, 0,26us resolution and limit of about 19 minutes max.

    #define HW_TIMER_USES_DMA_TIMER          3                           // {16} define DMA Timer 3 for HW timer use [0..3 available]

    #if HW_TIMER_USES_DMA_TIMER == 0
        #define HW_TIMER_MODE_REGISTER              DTMR0
        #define HW_TIMER_COUNTER                    DTCN0
        #define HW_TIMER_EVENT_REGISTER             DTER0
        #define HW_TIMER_REFERENCE_REGISTER         DTRR0
        #define HW_TIMER_INTERRUPT_CONFIG_REGISTER  IC_ICR_0_19
        #define HW_TIMER_VECTOR                     DTIM0_VECTOR
        #define HW_TIMER_INTERRUPT_PRIORITY         DTIM0_INTERRUPT_PRIORITY
        #define HW_TIMER_INTERRUPT_FLAG             DTIM0_PIF_INT_L
        #define HW_TIMER_POWER                      POWER_TMR0           // {23}
    #elif HW_TIMER_USES_DMA_TIMER == 1
        #define HW_TIMER_MODE_REGISTER              DTMR1
        #define HW_TIMER_COUNTER                    DTCN1
        #define HW_TIMER_EVENT_REGISTER             DTER1
        #define HW_TIMER_REFERENCE_REGISTER         DTRR1
        #define HW_TIMER_INTERRUPT_CONFIG_REGISTER  IC_ICR_0_20
        #define HW_TIMER_VECTOR                     DTIM1_VECTOR
        #define HW_TIMER_INTERRUPT_PRIORITY         DTIM1_INTERRUPT_PRIORITY
        #define HW_TIMER_INTERRUPT_FLAG             DTIM1_PIF_INT_L
        #define HW_TIMER_POWER                      POWER_TMR1           // {23}
    #elif HW_TIMER_USES_DMA_TIMER == 2
        #define HW_TIMER_MODE_REGISTER              DTMR2
        #define HW_TIMER_COUNTER                    DTCN2
        #define HW_TIMER_EVENT_REGISTER             DTER2
        #define HW_TIMER_REFERENCE_REGISTER         DTRR2
        #define HW_TIMER_INTERRUPT_CONFIG_REGISTER  IC_ICR_0_21
        #define HW_TIMER_VECTOR                     DTIM2_VECTOR
        #define HW_TIMER_INTERRUPT_PRIORITY         DTIM2_INTERRUPT_PRIORITY
        #define HW_TIMER_INTERRUPT_FLAG             DTIM2_PIF_INT_L
        #define HW_TIMER_POWER                      POWER_TMR2           // {23}
    #else
        #define HW_TIMER_MODE_REGISTER              DTMR3
        #define HW_TIMER_COUNTER                    DTCN3
        #define HW_TIMER_EVENT_REGISTER             DTER3
        #define HW_TIMER_REFERENCE_REGISTER         DTRR3
        #define HW_TIMER_INTERRUPT_CONFIG_REGISTER  IC_ICR_0_22
        #define HW_TIMER_VECTOR                     DTIM3_VECTOR
        #define HW_TIMER_INTERRUPT_PRIORITY         DTIM3_INTERRUPT_PRIORITY
        #define HW_TIMER_INTERRUPT_FLAG             DTIM3_PIF_INT_L
        #define HW_TIMER_POWER                      POWER_TMR3           // {23}
    #endif
#endif


#if defined M52259DEMO                                                   // {45}
    #define PHY_ADDRESS            0x01                                  // address of external PHY on board
    #define PHY_INTERRUPT          1                                     // IRQ1 is used as PHY interrupt input
    #define PHY_IDENTIFIER         0x00221512                            // MICREL KSZ8041NL identifier
    #define FNRESETPHY()           RESET_RCR |= FRCRSTOUT; fnDelayLoop(11000); RESET_RCR &= ~FRCRSTOUT; fnDelayLoop(200); // > 10ms reset asserted and > 100us before using MIIM
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#elif defined M52259_TOWER                                               // {65}
    #define FORCE_PHY_CONFIG                                             // activate forced configuration
    #define POLL_PHY               100000                                // PHY detection is unreliable on this board so allow this many attempts
    #define FNFORCE_PHY_CONFIG()   RESET_RCR |= FRCRSTOUT; \
                                   DDRTI = (PORT_I_BIT0 | PORT_I_BIT1 | PORT_I_BIT3 | PORT_I_BIT4 | PORT_I_BIT5 | PORT_I_BIT6 | PORT_I_BIT7); PORTTI = (PORT_I_BIT6); \
                                   DDRTJ = (PORT_J_BIT7); PORTTJ = 0; \
                                   fnDelayLoop(10000); RESET_RCR &= ~FRCRSTOUT; fnDelayLoop(100); DDRTI = 0; DDRTJ = 0; fnDelayLoop(1000);
                                                                         // > 10ms reset asserted and > 1ms before using MIIM
    #define PHY_ADDRESS            0x01                                  // address of external PHY on board
    #define PHY_INTERRUPT          1                                     // IRQ1 is used as PHY interrupt input (set J6 to position 7-8)
    #define PHY_IDENTIFIER         0x00221512                            // MICREL KSZ8041NL identifier
    #define MII_MANAGEMENT_CLOCK_SPEED    800000                         // reduced speed due to weak data line pull up resistor and long back-plane distance (warning - too low results in a divider overflow in MSCR)
#elif defined M52259EVB
    #define SCAN_PHY_ADD                                                 // PHY address is not defined exactly so it is scanned
  //#define PHY_INTERRUPT          0                                     // not used
    #define PHY_IDENTIFIER         0x20005ce1                            // National Semiconductor DP83640 identifier
    #define FNRESETPHY()           RESET_RCR &= ~FRCRSTOUT;              // no hardware delay since it is very long - just release reset line
    #define PHY_POWERUP_DELAY      (0.150 * SEC)                         // plus 50ms task delay to give > 170ms
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#elif defined M5282EVB                                                   // {68}
    #define PHY_ADDRESS            0x01                                  // address of external PHY on board
    #define PHY_INTERRUPT          2                                     // IRQ2 is used as PHY interrupt input
    #define PHY_IDENTIFIER         0x0022561b                            // AMD Am79C875VC identifier
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#elif defined M5282LITE                                                  // {68}
    #define PHY_ADDRESS            0x01                                  // address of external PHY on board
    #define PHY_INTERRUPT          6                                     // IRQ6 is used as PHY interrupt input
    #define PHY_IDENTIFIER         0x00221619                            // MICREL KS8721B identifier
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#elif defined M5208EVB                                                   // {69}
    #define SCAN_PHY_ADD                                                 // PHY address is not defined exactly so it is scanned
  //#define PHY_ADDRESS            0x01                                  // address of external PHY on board
  //#define PHY_INTERRUPT          1                                     // not used
    #define PHY_IDENTIFIER         0x20005c90                            // National DP83848 identifier
    #define FNRESETPHY()         //RESET_RCR &= ~FRCRSTOUT;              // no hardware delay since it is very long - just release reset line
    #define PHY_POWERUP_DELAY      (0.150 * SEC)                         // plus 50ms task delay to give > 167ms
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#elif defined M5282CU5
    #define SCAN_PHY_ADD                                                 // PHY address is not defined exactly so it is scanned
  //#define PHY_ADDRESS            0x01                                  // address of external PHY on board
  //#define PHY_INTERRUPT          1                                     // not used
    #define PHY_IDENTIFIER         0x20005c90                            // National DP83848 identifier
    #define FNRESETPHY()         //RESET_RCR &= ~FRCRSTOUT;              // no hardware delay since it is very long - just release reset line
    #define PHY_POWERUP_DELAY      (0.150 * SEC)                         // plus 50ms task delay to give > 167ms
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#elif defined M5235BCC
    #define PHY_ADDRESS            0x01                                  // address of external PHY on board
    #define PHY_INTERRUPT          6                                     // IRQ6 is used as PHY interrupt input
    #define PHY_IDENTIFIER         0x00221619                            // MICREL KS8721B identifier
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#else
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // standard speed
#endif
#define PHY_MASK                   0xfffffff0

// If a design doesn't have the phy interrupt line connected, the following can be used to cause the network indicator task to poll the state of the link and synchronise the Ethernet controller accordingly
//
//#define PHY_POLL_LINK
//#define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR                  // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)


#if defined CAN_INTERFACE
    #if defined _M52259
      //#define CAN_ON_TH                                                // alternative CAN pins on TH4,5, otherwise default UC0,1
    #else
        #define CAN_ON_AS2                                               // alternative CAN pins on AS2,3
      //#define CAN_ON_QS                                                // alternative CAN pins on QS0,1
      //#define CAN_ON_UA                                                // alternative CAN pins on UA2,3, otherwise default AS0,1
    #endif
#endif

// Ports
//
#define DEMO_LED_1                     0x01
#define DEMO_LED_2                     0x02

#define LAN_COLLISION_LED              PORT_LD_BIT4
#define LAN_DUPLEX_LED                 PORT_LD_BIT3
#define LAN_SPEED_LED                  PORT_LD_BIT2
#define LAN_LINK_LED                   PORT_LD_BIT1
#define LAN_ACTIVITY_LED               PORT_LD_BIT0


#if defined M52259EVB                                                    // {45}
    #define USER_SWITCH_SW1            PORT_DD_BIT5
    #define USER_SWITCH_SW2            PORT_DD_BIT6
    #define USER_SWITCH_SW3            PORT_DD_BIT7

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(TC, PORT_TC_BIT0) // {53} DDRTC |= PORT_TC_BIT0 
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(TC, PORT_TC_BIT0)    // {53} PORTTC ^= PORT_TC_BIT0
    #define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_BACKUP_WATCHDOG() MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define INIT_WATCHDOG_DISABLE()                                      // no initialisation required since the used port is default input
    #define WATCHDOG_DISABLE()         !((PORTIN_SETDD & PORT_DD_BIT6))  // pull this input down to disable watchdog

    #define CONFIGURE_MOUSE_INPUTS()   _CONFIG_PORT_INPUT(DD, (USER_SWITCH_SW1 | USER_SWITCH_SW2 | USER_SWITCH_SW3)) // {77} 
    #define MOUSE_LEFT_CLICK()         (!_READ_PORT_MASK(DD, USER_SWITCH_SW2)) // left mouse click using switch 2
    #define MOUSE_UP()                 0                                 // not used
    #define MOUSE_DOWN()               0                                 // not used
    #define MOUSE_LEFT()               (!_READ_PORT_MASK(DD, USER_SWITCH_SW1)) // press this button to move mouse left
    #define MOUSE_RIGHT()              (!_READ_PORT_MASK(DD, USER_SWITCH_SW3)) // press this button to move mouse right
#elif defined M52259DEMO                                                 // {45}
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(TC, PORT_TC_BIT0) // {53} DDRTC |= PORT_TC_BIT0 
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(TC, PORT_TC_BIT0)    // {53} PORTTC ^= PORT_TC_BIT0
    #define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_BACKUP_WATCHDOG() MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define INIT_WATCHDOG_DISABLE()                                      // {52} leave input as default IRQ function and read using EPPRD
    #define WATCHDOG_DISABLE()         (!(EPPDR0 & PORT_NQ_BIT1))        // pull this input down to disable watchdog

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()         0
    #define MOUSE_UP()                 0
    #define MOUSE_DOWN()               0
    #define MOUSE_LEFT()               0
    #define MOUSE_RIGHT()              0
#elif defined M52259_TOWER                                               // {65}
    #define DIP1                       PORT_DD_BIT4
    #define DIP2                       PORT_DD_BIT5
    #define DIP3                       PORT_DD_BIT6
    #define DIP4                       PORT_DD_BIT7
    #define LED1                       PORT_TC_BIT0
    #define LED2                       PORT_TC_BIT1
    #define LED3                       PORT_TC_BIT2
    #define LED4                       PORT_TC_BIT3
    #define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_BACKUP_WATCHDOG() MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define INIT_WATCHDOG_DISABLE()                                      // port DD defaults to input
    #define WATCHDOG_DISABLE()         !(PORTIN_SETDD & DIP1)            // set DIP1 to disable watchdog
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(TC, LED1)
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(TC, LED1)

    #define CONFIGURE_MOUSE_INPUTS()                                     // can be added
    #define MOUSE_LEFT_CLICK()         0                                 // can be added
    #define MOUSE_UP()                 0                                 // can be added
    #define MOUSE_DOWN()               0                                 // can be added
    #define MOUSE_LEFT()               0                                 // can be added
    #define MOUSE_RIGHT()              0                                 // can be added
#elif defined _M5282                                                     // {68}
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(TD, PORT_TD_BIT0)
    #define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_BACKUP_WATCHDOG() /*MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; */WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(TD, PORT_TD_BIT0)
    #define INIT_WATCHDOG_DISABLE()                                      // leave input as default IRQ function and read using EPPRD
    #define WATCHDOG_DISABLE()         (!(EPPDR0 & PORT_NQ_BIT1))        // pull this input down to disable watchdog
#elif defined M5208EVB
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(TIMER, PORT_TIMER_BIT3)
    #define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_BACKUP_WATCHDOG() /*MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; */WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(TIMER, PORT_TIMER_BIT3)
    #define INIT_WATCHDOG_DISABLE()                                      // leave input as default IRQ function and read using EPPRD
    #define WATCHDOG_DISABLE()         (!(EPPDR0 & PORT_NQ_BIT1))        // pull this input down to disable watchdog

                                                                         // configure SDRAM and FLASh chip select lines plus the SDRAM controller
    #define CONFIG_EXT_MEMORY()/* \ no configuration when working with installed boot loader 
    CSAR1 = 0xc0000000; CSCR1 = (PORT_SIZE_16 | FB_AA | FB_MUX | ((1 << WAIT_STATE_SHIFT) & WAIT_STATE_MASK));  CSMR1 = (0x00070000 | FB_CS_VALID); \
    CSCR0 = (PORT_SIZE_16 | BEM | FB_AA | FB_MUX | ((7 << WAIT_STATE_SHIFT) & WAIT_STATE_MASK)); CSMR0 = (0x001f0000 | FB_CS_VALID); \
    SDMR  = 0x018c0000; \
    SDCR  = 0x71092c00; \
    SDCFG1 = 0x43711630; \
    SDCFG2 = 0x56670000; \
    SDCS0 = 0x40000018; \
    SDCS1 = 0;*/
#elif defined M5282CU5
    #define INIT_WATCHDOG_LED()        
    #define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_BACKUP_WATCHDOG() /*MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; */WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define TOGGLE_WATCHDOG_LED()      //_TOGGLE_PORT(TD, PORT_TD_BIT0)
    #define INIT_WATCHDOG_DISABLE()                                      // leave input as default IRQ function and read using EPPRD
    #define WATCHDOG_DISABLE()         (!(EPPDR0 & PORT_NQ_BIT1))        // pull this input down to disable watchdog
#elif defined M5235BCC                                                   // {73}
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(CS, PORT_CS_BIT7)
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(CS, PORT_CS_BIT7)
  //#define BACKUP_WATCHDOG_TIMER                                        // use BWT in preference to core watchdog - 2 sec timeout
    #define CONFIGURE_WATCHDOG   0
  //#define CONFIGURE_BACKUP_WATCHDOG() MCF_CLOCK_BWCR = BWDSEL_SYS_DIV2; WMR = BWT_TIMEOUT_SYS_CLK(2); WCR = (BWT_STOP | BWT_DOZE | BWT_EN)// stop watchdog in DOZE or STOP but not in WAIT modes (note WMR must be written first). Clocked by sys/2/4096
    #define INIT_WATCHDOG_DISABLE()                                      // leave input as default IRQ function and read using EPPRD
    #define WATCHDOG_DISABLE()         (1) //(!(EPPDR0 & PORT_IRQ_BIT2)) // pull this input down to disable watchdog
    // Configure SDRAM controller for 32 bit SDRAM - 16 MByte, 25MHz bus clock {75}
    //
    #define DRAM_AUTO_REFRESH		0.064                                // s
    #define DRAM_AUTO_REFRESH_ROWS	4096
    #define DRAM_RC	(int)(((BUS_CLOCK * DRAM_AUTO_REFRESH / DRAM_AUTO_REFRESH_ROWS)/16) - 1)
    #if defined _WINDOWS                                                 // write to initialize, some data to some sdram address 
        #define _DUMMY_WRITE()
    #else
	   #define  _DUMMY_WRITE()      *(unsigned long *)(0x00000000) = 0x12345678
    #endif
    // Set PAR_SDRAM to allow SDRAM signals to be enabled
    // Set PAR_AD to allow 32-bit SDRAM if the external boot device is 16-bits
    // 2MB FLASH on CS0 at 0xFFE00000
    //
    #define CONFIG_EXT_MEMORY()        PAR_SDRAM = 0x3f; PAR_AD = (PAR_ADDR23 | PAR_ADDR22 | PAR_ADDR21 | PAR_DATAL); \
    CSAR0 = 0xffe00000; CSMR0 = 0x001f0001; CSCR0 = 0x1980; fnDelayLoop(13); \
	DCR = (RTIM_CLK_3 | DRAM_RC) << 16;	DACR0 = 0x00001300; DMR0 = 0x00fc0001; DACR0 = 0x00001308; \
    _DUMMY_WRITE(); fnDelayLoop(13); \
    DACR0 = 0x00009300; DACR0 = 0x00009340; \
    _DUMMY_WRITE(); fnDelayLoop(78);

    #if defined EXT_FLASH_FILE_SYSTEM
      //#define _S29JL032_32
        #define M29W160EB
        #define EXT_FLASH_16_BIT                                         // flash is configured for 16 bit width
        #define EXT_FLASH_PAGE_LENGTH        (64 * 1024)
        #define EXTERNAL_FLASH_DEVICE_COUNT  1                           // the number of chips 
        #define EXTERNAL_FLASH_START_ADDRESS (0xffe00000)
        #define SIZE_OF_EXTERNAL_FLASH_CHIP  (2 * 1024 * 1024)           // individual chip size
        #define SIZE_OF_EXTERNAL_FLASH       (EXTERNAL_FLASH_DEVICE_COUNT * SIZE_OF_EXTERNAL_FLASH_CHIP)
        #define EXT_PARALLEL_FLASH_OFFSET    (0)
    #endif
#elif defined M52211EVB                                                  // {77}
    #define IRQ1_INPUT               PORT_NQ_BIT1
    #define IRQ5_INPUT               PORT_NQ_BIT5
    #define SWITCH_SW1               IRQ5_INPUT
    #define SWITCH_SW2               IRQ1_INPUT
    #define INIT_WATCHDOG_LED()      _CONFIG_PORT_OUTPUT(TC, PORT_TC_BIT0) // DDRTC |= PORT_TC_BIT0 
    #define TOGGLE_WATCHDOG_LED()    _TOGGLE_PORT(TC, PORT_TC_BIT0)      // PORTTC ^= PORT_TC_BIT0
    #define CONFIGURE_WATCHDOG       ACTIVE_WATCHDOG_2_SEC               // watchdog enabled with 2 second period (set WATCHDOG_DISABLED to disable)
    #define INIT_WATCHDOG_DISABLE()  (PNQPAR &= ~(ALT_2_FUNCTION_Q << BIT_5_Q_SHIFT)) // ensure IRQ5 is an input
    #define WATCHDOG_DISABLE()       !(PORTIN_SETNQ & SWITCH_SW1)        // pull this input down to disable watchdog (enable debugging) - SW1
    #define CONFIGURE_MOUSE_INPUTS() (PNQPAR &= ~((ALT_2_FUNCTION_Q << BIT_5_Q_SHIFT) | (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT))); _CONFIG_PORT_INPUT(NQ, (SWITCH_SW1 | SWITCH_SW2))
    #define MOUSE_UP()               0                                   // not used
    #define MOUSE_DOWN()             0                                   // not used
    #define MOUSE_LEFT()             (!_READ_PORT_MASK(NQ, SWITCH_SW2))  // press this button to move mouse left
    #define MOUSE_RIGHT()            (!_READ_PORT_MASK(NQ, SWITCH_SW1))  // press this button to move mouse right
#else
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(TC, PORT_TC_BIT0) // {53} DDRTC |= PORT_TC_BIT0 
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(TC, PORT_TC_BIT0)    // {53} PORTTC ^= PORT_TC_BIT0
    #define CONFIGURE_WATCHDOG         ACTIVE_WATCHDOG_2_SEC             // watchdog enabled with 2 second period (set WATCHDOG_DISABLED to disable)
    #if defined _M521X                                                   // {49}
        #define INIT_WATCHDOG_DISABLE()                                  // {52} leave input as default IRQ function and read using EPPRD
        #define WATCHDOG_DISABLE()         (!(EPPDR0 & PORT_NQ_BIT4))    // pull this input down to disable watchdog
      //#define INIT_WATCHDOG_DISABLE()    (PNQPAR &= ~(PRIMARY_FUNCTION_D << BIT_4_D_SHIFT)) // ensure IRQ4 is an input
      //#define WATCHDOG_DISABLE()         !(PORTIN_SETNQ & PORT_NQ_BIT4) // pull this input (SW1 on M5213EVB) down to disable watchdog (enable debugging) {3}
    #else
        #define INIT_WATCHDOG_DISABLE()    (PNQPAR &= ~(ALT_2_FUNCTION_Q << BIT_4_Q_SHIFT)) // ensure IRQ4 is an input
        #define WATCHDOG_DISABLE()         !(PORTIN_SETNQ & PORT_NQ_BIT4)  // pull this input down to disable watchdog (enable debugging)
    #endif

    #define CONFIGURE_MOUSE_INPUTS()                                     // can be added
    #define MOUSE_LEFT_CLICK()         0                                 // can be added
    #define MOUSE_UP()                 0                                 // can be added
    #define MOUSE_DOWN()               0                                 // can be added
    #define MOUSE_LEFT()               0                                 // can be added
    #define MOUSE_RIGHT()              0                                 // can be added
#endif

#define SET_USB_SYMBOL()                                                 // dummy - can be use to indicate when USB is active
#define DEL_USB_SYMBOL()                                                 // dummy - can be use to indicate when USB has been deactivated
 
#if defined M52259_TOWER
    #define CONFIG_TEST_OUTPUT()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TC, LED3, LED3)
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(TC, LED3)
#elif defined M5235BCC
    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()
#else
    #define CONFIG_TEST_OUTPUT()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TC, PORT_TC_BIT3, PORT_TC_BIT3) // {53} PORTTC |= PORT_TC_BIT3; DDRTC |= PORT_TC_BIT3; // {27}
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(TC, PORT_TC_BIT3) // {53} PORTTC ^= PORT_TC_BIT3;
#endif
  
// Hardware specific port reset states - if the hardware has pullups set '1' for each bit, else '0'
// (assumes all inputs have pullups)
#define QS_DEFAULT_INPUT           0xff                                       
#define AN_DEFAULT_INPUT           0xff
#define LD_DEFAULT_INPUT           0xff
#define NQ_DEFAULT_INPUT           0x33
#define TA_DEFAULT_INPUT           0xff
#define TC_DEFAULT_INPUT           0xff
#define TD_DEFAULT_INPUT           0xff
#define UA_DEFAULT_INPUT           0xff
#define UB_DEFAULT_INPUT           0xff
#define UC_DEFAULT_INPUT           0xff
#define AS_DEFAULT_INPUT           0xff
#define GP_DEFAULT_INPUT           0xff
#define DD_DEFAULT_INPUT           0xff

#define TE_DEFAULT_INPUT           0xff                                  // {38} _M5225X
#define TF_DEFAULT_INPUT           0xff
#define TG_DEFAULT_INPUT           0xff
#define TH_DEFAULT_INPUT           0xff
#define TI_DEFAULT_INPUT           0xff
#define TJ_DEFAULT_INPUT           0xff

#define A_DEFAULT_INPUT            0xfc                                  // {68} _M5282
#define B_DEFAULT_INPUT            0xf3
#define C_DEFAULT_INPUT            0xff
#define D_DEFAULT_INPUT            0xff
#define E_DEFAULT_INPUT            0xff
#define F_DEFAULT_INPUT            0xff
#define G_DEFAULT_INPUT            0xff
#define H_DEFAULT_INPUT            0xff
#define J_DEFAULT_INPUT            0xff
#define EH_DEFAULT_INPUT           0xff
#define EL_DEFAULT_INPUT           0xff
#define QA_DEFAULT_INPUT           0xff
#define QB_DEFAULT_INPUT           0xff
#define SD_DEFAULT_INPUT           0xff
#define TB_DEFAULT_INPUT           0xff

#define FECH_DEFAULT_INPUT         0xff                                  // {69} _M520X
#define FECL_DEFAULT_INPUT         0xff
#define BUSCTL_DEFAULT_INPUT       0xff
#define BE_DEFAULT_INPUT           0xff
#define CS_DEFAULT_INPUT           0xff
#define FECI2C_DEFAULT_INPUT       0xff
#define UART_DEFAULT_INPUT         0xff
#define QSPI_DEFAULT_INPUT         0xff
#define TIMER_DEFAULT_INPUT        0xff
#define IRQ_DEFAULT_INPUT          0xff

#define ADDR_DEFAULT_INPUT         0xff                                  // {73} _M523X
#define BS_DEFAULT_INPUT           0xff
#define DATAH_DEFAULT_INPUT        0xff
#define DATAL_DEFAULT_INPUT        0xff
#define ETPU_DEFAULT_INPUT         0xff
#define SDRAM_DEFAULT_INPUT        0xff
#define UARTH_DEFAULT_INPUT        0xff
#define UARTL_DEFAULT_INPUT        0xff

#define CHIP_CONFIGURATION         (RCON_ASSERTED)                       // {68} _M5282, _M5235

#define FORCE_D9_D1                (0x95)                                // {69} _M5282 data lines driven with this value when RCON is asserted [D9, D7, D6, D5, D4, D3, D2, D1]
#define FORCE_CLK_D25_D16          (0x0b)                                // _M5235 data lines driven with this value when RCON is asserted [CLKMOD1, CLKMOD2, D25, D24, D21, D20, D19, D16]


#if defined _M520X
    #if (FORCE_D9_D1 & 0x01)
        #define PLL_OUTPUT_FREQ_INT    166666666
    #else
        #define PLL_OUTPUT_FREQ_INT    83333333
    #endif
    #define POST_DIV                   1
#endif


// User port mapping
//
#if defined _M5282                                                       // {68}
    #define USER_PORT_1_BIT            PORT_A_BIT0                       // first 8 on the A port
    #define USER_PORT_2_BIT            PORT_A_BIT1
    #define USER_PORT_3_BIT            PORT_A_BIT2
    #define USER_PORT_4_BIT            PORT_A_BIT3
    #define USER_PORT_5_BIT            PORT_A_BIT4
    #define USER_PORT_6_BIT            PORT_A_BIT5
    #define USER_PORT_7_BIT            PORT_A_BIT6
    #define USER_PORT_8_BIT            PORT_A_BIT7

    #define USER_PORT_9_BIT            PORT_B_BIT0                       // next 8 on the B port
    #define USER_PORT_10_BIT           PORT_B_BIT1
    #define USER_PORT_11_BIT           PORT_B_BIT2
    #define USER_PORT_12_BIT           PORT_B_BIT3
    #define USER_PORT_13_BIT           PORT_B_BIT4
    #define USER_PORT_14_BIT           PORT_B_BIT5
    #define USER_PORT_15_BIT           PORT_B_BIT6
    #define USER_PORT_16_BIT           PORT_B_BIT7
#else
    #define USER_PORT_1_BIT            PORT_AN_BIT0                      // first 8 on the AN port
    #define USER_PORT_2_BIT            PORT_AN_BIT1
    #define USER_PORT_3_BIT            PORT_AN_BIT2
    #define USER_PORT_4_BIT            PORT_AN_BIT3
    #define USER_PORT_5_BIT            PORT_AN_BIT4
    #define USER_PORT_6_BIT            PORT_AN_BIT5
    #define USER_PORT_7_BIT            PORT_AN_BIT6
    #define USER_PORT_8_BIT            PORT_AN_BIT7
    #if defined M52259_TOWER
        #define USER_PORT_9_BIT        0                                 // next 4 on the TA port (not all used)
        #define USER_PORT_10_BIT       0
        #define USER_PORT_11_BIT       PORT_TA_BIT2
        #define USER_PORT_12_BIT       PORT_TA_BIT3
    #else
        #define USER_PORT_9_BIT        PORT_TA_BIT0                      // next 4 on the TA port
        #define USER_PORT_10_BIT       PORT_TA_BIT1
        #define USER_PORT_11_BIT       PORT_TA_BIT2
        #define USER_PORT_12_BIT       PORT_TA_BIT3
    #endif

    #if defined _M5225X                                                  // {38} no TD port on this device
        #define USER_PORT_13_BIT       PORT_DD_BIT0                      // last 4 on the TD port
        #define USER_PORT_14_BIT       PORT_DD_BIT1
        #define USER_PORT_15_BIT       PORT_DD_BIT2
        #define USER_PORT_16_BIT       PORT_DD_BIT3
    #else
        #define USER_PORT_13_BIT       PORT_TD_BIT0                      // last 4 on the TD port
        #define USER_PORT_14_BIT       PORT_TD_BIT1
        #define USER_PORT_15_BIT       PORT_TD_BIT2
        #define USER_PORT_16_BIT       PORT_TD_BIT3
    #endif
#endif

// Port use definitions
//
#if defined _M5282                                                       // {68}
    #define USER_PORT_1                PORTA
    #define USER_DDR_1                 DDRA
    #define USER_PORT_2                PORTA
    #define USER_DDR_2                 DDRA
    #define USER_PORT_3                PORTA
    #define USER_DDR_3                 DDRA
    #define USER_PORT_4                PORTA
    #define USER_DDR_4                 DDRA
    #define USER_PORT_5                PORTA
    #define USER_DDR_5                 DDRA
    #define USER_PORT_6                PORTA
    #define USER_DDR_6                 DDRA
    #define USER_PORT_7                PORTA
    #define USER_DDR_7                 DDRA
    #define USER_PORT_8                PORTA
    #define USER_DDR_8                 DDRA

    #define USER_PORT_9                PORTB
    #define USER_DDR_9                 DDRB
    #define USER_PORT_10               PORTB
    #define USER_DDR_10                DDRB
    #define USER_PORT_11               PORTB
    #define USER_DDR_11                DDRB
    #define USER_PORT_12               PORTB
    #define USER_DDR_12                DDRB
    #define USER_PORT_13               PORTB
    #define USER_DDR_13                DDRB
    #define USER_PORT_14               PORTB
    #define USER_DDR_14                DDRB
    #define USER_PORT_15               PORTB
    #define USER_DDR_15                DDRB
    #define USER_PORT_16               PORTB
    #define USER_DDR_16                DDRB
#else
    #define USER_PORT_1                PORTAN
    #define USER_DDR_1                 DDRAN
    #define USER_PORT_2                PORTAN
    #define USER_DDR_2                 DDRAN
    #define USER_PORT_3                PORTAN
    #define USER_DDR_3                 DDRAN
    #define USER_PORT_4                PORTAN
    #define USER_DDR_4                 DDRAN
    #define USER_PORT_5                PORTAN
    #define USER_DDR_5                 DDRAN
    #define USER_PORT_6                PORTAN
    #define USER_DDR_6                 DDRAN
    #define USER_PORT_7                PORTAN
    #define USER_DDR_7                 DDRAN
    #define USER_PORT_8                PORTAN
    #define USER_DDR_8                 DDRAN

    #define USER_PORT_9                PORTTA
    #define USER_DDR_9                 DDRTA
    #define USER_PORT_10               PORTTA
    #define USER_DDR_10                DDRTA
    #define USER_PORT_11               PORTTA
    #define USER_DDR_11                DDRTA
    #define USER_PORT_12               PORTTA
    #define USER_DDR_12                DDRTA

    #if defined _M5225X                                                  // {38} no TD port on this device
        #define USER_PORT_13           PORTDD
        #define USER_DDR_13            DDRDD
        #define USER_PORT_14           PORTDD
        #define USER_DDR_14            DDRDD
        #define USER_PORT_15           PORTDD
        #define USER_DDR_15            DDRDD
        #define USER_PORT_16           PORTDD
        #define USER_DDR_16            DDRDD
    #else
        #define USER_PORT_13           PORTTD
        #define USER_DDR_13            DDRTD
        #define USER_PORT_14           PORTTD
        #define USER_DDR_14            DDRTD
        #define USER_PORT_15           PORTTD
        #define USER_DDR_15            DDRTD
        #define USER_PORT_16           PORTTD
        #define USER_DDR_16            DDRTD
    #endif
#endif
                                                                         // {4}
#if defined M52259_TOWER
    #define CONFIG_TIMER_TEST_LEDS()   
    #define TIMER_TEST_LED_ON()        
    #define TIMER_TEST_LED2_ON()       
    #define TIMER_TEST_LED_OFF()       
    #define TIMER_TEST_LED2_OFF()      
#else
    #define CONFIG_TIMER_TEST_LEDS()   DDRTC |= (PORT_TC_BIT3 | PORT_TC_BIT2)
    #define TIMER_TEST_LED_ON()        PORTTC |= PORT_TC_BIT2
    #define TIMER_TEST_LED2_ON()       PORTTC |= PORT_TC_BIT3
    #define TIMER_TEST_LED_OFF()       PORTTC &= ~PORT_TC_BIT2
    #define TIMER_TEST_LED2_OFF()      PORTTC &= ~PORT_TC_BIT3
#endif

#define MEASURE_LOW_POWER_ON()     _SETBITS(TC, PORT_TC_BIT1)            // visualise low power state on LED
#define MEASURE_LOW_POWER_OFF()    _CLEARBITS(TC, PORT_TC_BIT1)


// Keypad
//
#if defined KEY_COLUMNS && KEY_COLUMNS > 0                               // matrix keypad
    #define KEY_ROW_IN_1              PORT_AN_BIT0 
    #define KEY_ROW_IN_PORT_1         PORTIN_SETAN 
    #define KEY_ROW_IN_DDR_1          DDRAN 
    #define KEY_ROW_IN_PORT_1_REF     _PORT_AN

    #define KEY_ROW_IN_2              PORT_AN_BIT1
    #define KEY_ROW_IN_PORT_2         PORTIN_SETAN
    #define KEY_ROW_IN_DDR_2          DDRAN
    #define KEY_ROW_IN_PORT_2_REF     _PORT_AN

    #define KEY_ROW_IN_3              PORT_AN_BIT2
    #define KEY_ROW_IN_PORT_3         PORTIN_SETAN
    #define KEY_ROW_IN_DDR_3          DDRAN 
    #define KEY_ROW_IN_PORT_3_REF     _PORT_AN

    #define KEY_ROW_IN_4              PORT_AN_BIT3
    #define KEY_ROW_IN_PORT_4         PORTIN_SETAN
    #define KEY_ROW_IN_DDR_4          DDRAN 
    #define KEY_ROW_IN_PORT_4_REF     _PORT_AN

    #define KEY_COL_OUT_1             PORT_AN_BIT4
    #define KEY_COL_OUT_PORT_1        PORTAN
    #define KEY_COL_OUT_DDR_1         DDRAN 
    #define KEY_ROW_OUT_PORT_1_REF    _PORT_AN

    #define KEY_COL_OUT_2             PORT_AN_BIT5
    #define KEY_COL_OUT_PORT_2        PORTAN
    #define KEY_COL_OUT_DDR_2         DDRAN
    #define KEY_ROW_OUT_PORT_2_REF    _PORT_AN 

    #define KEY_COL_OUT_3             PORT_AN_BIT6
    #define KEY_COL_OUT_PORT_3        PORTAN
    #define KEY_COL_OUT_DDR_3         DDRAN 
    #define KEY_ROW_OUT_PORT_3_REF    _PORT_AN

    #define KEY_COL_OUT_4             PORT_AN_BIT7
    #define KEY_COL_OUT_PORT_4        PORTAN
    #define KEY_COL_OUT_DDR_4         DDRAN 
    #define KEY_ROW_OUT_PORT_4_REF    _PORT_AN
                                                                         // drive each column low
    #define DRIVE_COLUMN_1()     KEY_COL_OUT_DDR_1 |=  KEY_COL_OUT_1;    // drive output low (column 1)
    #define DRIVE_COLUMN_2()     KEY_COL_OUT_DDR_2 |=  KEY_COL_OUT_2;    // drive output low (column 2)
    #define DRIVE_COLUMN_3()     KEY_COL_OUT_DDR_3 |=  KEY_COL_OUT_3;    // drive output low (column 3)
    #define DRIVE_COLUMN_4()     KEY_COL_OUT_DDR_4 |=  KEY_COL_OUT_4;    // drive output low (column 4)

                                                                         // drive high (to avoid slow rise time) then set back as input
    #define RELEASE_COLUMN_1()   KEY_COL_OUT_PORT_1 |= KEY_COL_OUT_1; KEY_COL_OUT_DDR_1 &=  ~KEY_COL_OUT_1;
    #define RELEASE_COLUMN_2()   KEY_COL_OUT_PORT_2 |= KEY_COL_OUT_2; KEY_COL_OUT_DDR_2 &=  ~KEY_COL_OUT_2;
    #define RELEASE_COLUMN_3()   KEY_COL_OUT_PORT_3 |= KEY_COL_OUT_3; KEY_COL_OUT_DDR_3 &=  ~KEY_COL_OUT_3;  
    #define RELEASE_COLUMN_4()   KEY_COL_OUT_PORT_4 |= KEY_COL_OUT_4; KEY_COL_OUT_DDR_4 &=  ~KEY_COL_OUT_4;

                                                                         // reset any changes ready for next scan sequence
    #define RESET_SCAN()         KEY_COL_OUT_PORT_1 &= ~KEY_COL_OUT_1; \
                                 KEY_COL_OUT_PORT_2 &= ~KEY_COL_OUT_2; \
                                 KEY_COL_OUT_PORT_3 &= ~KEY_COL_OUT_3; \
                                 KEY_COL_OUT_PORT_3 &= ~KEY_COL_OUT_4;   // prepare outputs low for next time

    #define INIT_KEY_SCAN()      RESET_SCAN()                            // use AN port for scanning keypad: AN
#elif defined KEY_COLUMNS && KEY_COLUMNS == 0                            // {54}
    #define KEY_ROW_IN_1         PORT_AN_BIT0
    #define KEY_ROW_IN_2         PORT_AN_BIT1
    #define KEY_ROW_IN_3         PORT_AN_BIT2
    #define KEY_ROW_IN_4         PORT_AN_BIT3

    #define KEY_1_PORT_REF       _PORT_AN
    #define KEY_1_PORT_PIN       KEY_ROW_IN_1

    #define KEY_5_PORT_REF       _PORT_AN
    #define KEY_5_PORT_PIN       KEY_ROW_IN_2

    #define KEY_2_PORT_REF       _PORT_AN
    #define KEY_2_PORT_PIN       KEY_ROW_IN_4

    #define KEY_9_PORT_REF       _PORT_AN
    #define KEY_9_PORT_PIN       KEY_ROW_IN_3


    #define INIT_KEY_STATE       0x0000000f

    #define READ_KEY_INPUTS()    _READ_PORT_MASK(AN, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4))

    #define INIT_KEY_SCAN()      _CONFIG_PORT_INPUT(AN, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4));
#endif


// LCD interface: Backlight control Port AS bit 0 : Data bus (4 Bit) Port TA 0..3 : RS Port AS bit 1, RW Port AS bit 2, E Port AS bit 3
//
typedef unsigned char LCD_BUS_PORT_SIZE;                                 // we use 8 bit ports
typedef unsigned char LCD_CONTROL_PORT_SIZE;
//#define LCD_BUS_8BIT                                                   // data bus in 8 bit mode
#define LCD_BUS_4BIT                                                     // data bus in 4 bit mode

#if defined LCD_BUS_8BIT
    #define LCD_BUS_MASK         0xff
    #define DATA_SHIFT_RIGHT     0                                       // no shift required to bring data into position
    #define DATA_SHIFT_LEFT      0
#else
    #define LCD_BUS_MASK         0x0f
    #define DATA_SHIFT_RIGHT     4                                       // nibble shift down required to bring data into position
    #define DATA_SHIFT_LEFT      0
#endif

#define O_CONTROL_RS             PORT_AS_BIT1
#define O_WRITE_READ             PORT_AS_BIT2
#define O_CONTROL_EN             PORT_AS_BIT3
#define O_LCD_BACKLIGHT          PORT_AS_BIT0

#define O_CONTROL_LINES          (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
#define IO_BUS_PORT_DDR          DDRTA
#define O_CONTROL_PORT_DDR       DDRAS
#define IO_BUS_PORT_DAT          PORTTA
#define IO_BUS_PORT_DAT_IN       PORTIN_SETTA
#define O_CONTROL_PORT_DAT       PORTAS

// Drive the control lines R/W + LCD Backlight '1', RS + E '0'  and the data lines with all high impedance at start up
#define INITIALISE_LCD_CONTROL_LINES()       IO_BUS_PORT_DDR = 0; IO_BUS_PORT_DDR = 0; \
                                             O_CONTROL_PORT_DDR &= ~(O_CONTROL_LINES | O_LCD_BACKLIGHT); O_CONTROL_PORT_DAT &= ~(O_CONTROL_LINES | O_LCD_BACKLIGHT); O_CONTROL_PORT_DAT |= (O_LCD_BACKLIGHT | O_WRITE_READ); \
                                             O_CONTROL_PORT_DDR |= (O_CONTROL_LINES | O_LCD_BACKLIGHT);


#define LCD_DRIVE_DATA()          IO_BUS_PORT_DDR |= LCD_BUS_MASK;    IO_BUS_PORT_DDR |= LCD_BUS_MASK;
                                  // ensure data bus outputs (delay) by repetitions according to processor speed

#define CLOCK_EN_HIGH()           O_CONTROL_PORT_DAT |= (O_CONTROL_EN); O_CONTROL_PORT_DAT |= (O_CONTROL_EN); 
                                  // clock EN to high state - repeat to slow down (delay)

#define DELAY_ENABLE_CLOCK_HIGH() O_CONTROL_PORT_DAT &= ~(O_CONTROL_EN); // {11}


// Graphic LCD
//
#if defined TFT2N0369_GLCD_MODE                                          // {70}
    #define GLCD_COMMAND_ADDR     0x400000
    #define GLCD_DATA_ADDR        0x410000

    #define BACK_LIGHT_MAX_INTENSITY()    PTCPAR &= ~(TC_PWM6_FUNCTION); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TC, PORT_TC_BIT3, PORT_TC_BIT3)
    #define BACK_LIGHT_MIN_INTENSITY()    PTCPAR &= ~(TC_PWM6_FUNCTION); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TC, PORT_TC_BIT3, 0)

    #define ENABLE_BACKLIGHT()    BACK_LIGHT_MAX_INTENSITY()
    // Tower kit uses 16 bit FlexBus interface on CS0 ( // The address range is set to 128K because the DC signal is connected on address wire. FlexBus setup as fast as possible in multiplexed mode
    //
    #define CONFIGURE_GLCD()      POWER_UP(POWER_MINI_FLEXBUS); \
                                  PTEPAR = (TE_FB_ADD_0_FUNCTION | TE_FB_ADD_1_FUNCTION | TE_FB_ADD_2_FUNCTION | TE_FB_ADD_3_FUNCTION | TE_FB_ADD_4_FUNCTION | TE_FB_ADD_5_FUNCTION | TE_FB_ADD_6_FUNCTION | TE_FB_ADD_7_FUNCTION); \
	 	                          PTFPAR = (TF_FB_ADD_8_FUNCTION | TF_FB_ADD_9_FUNCTION | TF_FB_ADD_10_FUNCTION | TF_FB_ADD_11_FUNCTION | TF_FB_ADD_12_FUNCTION | TF_FB_ADD_13_FUNCTION | TF_FB_ADD_14_FUNCTION | TF_FB_ADD_15_FUNCTION); \
	 	                          PTGPAR = (TG_FB_ADD_16_FUNCTION | TG_FB_CS0_FUNCTION | TG_FB_RW_FUNCTION); \
                                  CSAR0  = GLCD_COMMAND_ADDR; \
  	                              CSMR0  = (0x00010000 | FB_CS_VALID); \
  	                              CSCR0  = (FB_MUX | FB_AA | PORT_SIZE_16); \
                                  BACK_LIGHT_MIN_INTENSITY();

    #define GLCD_RST_H()
    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #endif

    #define SUPPORT_TOUCH_SCREEN                                         // touch screen operation
    #define GLCD_BACKLIGHT_CONTROL                                       // (requires SUPPORT_PWM_MODULE)
  //#define TEMP_HUM_TEST
  //#define VARIABLE_PIXEL_COLOUR                                        // support control of background and pixel colors

    #define _GLCD_BACKLIGHT_TIMER         6                              // PWM channel 6 (TC)
    #define _GLCD_TIMER_MODE_OF_OPERATION (PWM_PRESCALER_128 | PWM_SCALED_CLOCK_INPUT | PWM_POLARITY)
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY (unsigned char)(PWM_US_DELAY(PWM_FREQUENCY_VALUE(2000 * 128)))
#elif !defined _GLCD_SAMSUNG
    #define DATA_LINES                 (PORT_DD_BIT7 | PORT_DD_BIT6 | PORT_DD_BIT5 | PORT_DD_BIT4 | PORT_DD_BIT3 | PORT_DD_BIT2 | PORT_DD_BIT1 | PORT_DD_BIT0)
    #define GLCD_RST                   PORT_UC_BIT0
    #define GLCD_C_D                   PORT_UC_BIT1
    #define GLCD_WR                    PORT_UC_BIT2
    #define GLCD_RD                    PORT_UC_BIT3
    #define CONFIGURE_GLCD()           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(UC, (GLCD_RST | GLCD_WR | GLCD_RD | GLCD_C_D), (GLCD_WR | GLCD_RD | GLCD_C_D)); _CONFIG_PORT_INPUT(DD, DATA_LINES);

    #define GLCD_DATAASOUTPUT()        _DRIVE_PORT_OUTPUT(DD, DATA_LINES)
    #define GLCD_DATAASINPUT()         _FLOAT_PORT(DD, DATA_LINES)

    #define GLCD_DATAOUT(data)         _WRITE_PORT(DD, data)
    #define GLCD_DATAIN()              _READ_PORT(DD)

    #define SET_PULL_DOWNS()       
    #define REMOVE_PULL_DOWNS()    

    #define GLCD_WR_H()                _SETBITS(UC, GLCD_WR)
    #define GLCD_WR_L()                _CLEARBITS(UC, GLCD_WR)

    #define GLCD_CD_H()                _SETBITS(UC, GLCD_C_D)
    #define GLCD_CD_L()                _CLEARBITS(UC, GLCD_C_D)

    #define GLCD_RD_H()                _SETBITS(UC, GLCD_RD)
    #define GLCD_RD_L()                _CLEARBITS(UC, GLCD_RD)

    #define GLCD_DELAY_WRITE()         //GLCD_WR_L()                     // no write delay since the data is stable for long enough at full speed
    #define GLCD_DELAY_READ()          GLCD_RD_L()                       // one extra operation to ensure set up time of read

    #define GLCD_RST_H()               _SETBITS(UC, GLCD_RST)
    #define GLCD_RST_L()               _CLEARBITS(UC, GLCD_RST)      

    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   20                                // the maximum number of writes to the GLCD before the task yields
    #endif
#else                                                                    // {57}
    #define DATA_LINES                 (PORT_DD_BIT7 | PORT_DD_BIT6 | PORT_DD_BIT5 | PORT_DD_BIT4 | PORT_DD_BIT3 | PORT_DD_BIT2 | PORT_DD_BIT1 | PORT_DD_BIT0)

    #define GLCD_RST                   PORT_UA_BIT0                      // reset
    #define GLCD_RS                    PORT_UA_BIT1                      // GLCD Register Select
    #define GLCD_RW                    PORT_UA_BIT2                      // GLCD Read/Write
    #define GLCD_ENA                   PORT_UA_BIT3                      // GLCD Enable

    #define GLCD_CS0                   PORT_UB_BIT0                      // LCD Controller 0 Chip Select - 2 controller chips for 128 x 64
    #define GLCD_CS1                   PORT_UB_BIT1                      // LCD Controller 1 Chip Select

    #define SET_PULL_DOWNS()       
    #define REMOVE_PULL_DOWNS()    

    #define CONFIGURE_GLCD()           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(UA, (GLCD_RST | GLCD_RW | GLCD_ENA | GLCD_RS), (GLCD_RW)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(UB, (GLCD_CS0 | GLCD_CS1), (0)); _CONFIG_PORT_INPUT(DD, DATA_LINES);;

    #define GLCD_DATAASOUTPUT()        _DRIVE_PORT_OUTPUT(DD, 0xff)
    #define GLCD_DATAASINPUT()         _FLOAT_PORT(DD, 0xff)

    #define GLCD_DATAOUT(data)         _WRITE_PORT_MASK(DD, data, 0xff)
    #define GLCD_DATAIN()              _READ_PORT(DD)

    #define GLCD_RS_H()                _SETBITS(UA, GLCD_RS)
    #define GLCD_RS_L()                _CLEARBITS(UA, GLCD_RS)

    #define GLCD_RW_H()                _SETBITS(UA, GLCD_RW)
    #define GLCD_RW_L()                _CLEARBITS(UA, GLCD_RW)

    #define GLCD_CS0_H()               _SETBITS(UB, GLCD_CS0)  
    #define GLCD_CS0_L()               _CLEARBITS(UB, GLCD_CS0)

    #define GLCD_CS1_H()               _SETBITS(UB, GLCD_CS1)  
    #define GLCD_CS1_L()               _CLEARBITS(UB, GLCD_CS1)

    #define GLCD_DELAY_WRITE()                                           // no write delay since the data is stable for long enough at full speed
    #define GLCD_DELAY_READ()          GLCD_RW_H()                       // one extra operation to ensure set up time of read

    #define GLCD_RST_H()               _SETBITS(UA, GLCD_RST)  
    #define GLCD_RST_L()               _CLEARBITS(UA, GLCD_RST)

    #define GLCD_ENA_H()               _SETBITS(UA, GLCD_ENA)  
    #define GLCD_ENA_L()               _CLEARBITS(UA, GLCD_ENA)

    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   80                                // the maximum number of writes to the GLCD before the task yields
    #endif
#endif


// SMTP settings                                                         {1}
//
#if defined (M52235EVB)
    #define SENDERS_EMAIL_ADDRESS      "M52235EVB@uTasker.com"           // fictional Email address of the board being used
    #define EMAIL_SUBJECT              "M52235EVB Test"                  // Email subject
    #define EMAIL_CONTENT              "Hello!!\r\nThis is an email message from the M52235EVB.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your M52235EVB!!";
#elif defined M52259DEMO                                                 // {45}
    #define SENDERS_EMAIL_ADDRESS      "M52259DEMO@uTasker.com"          // fictional Email address of the board being used
    #define EMAIL_SUBJECT              "M52259DEMO Test"                 // Email subject
    #define EMAIL_CONTENT              "Hello!!\r\nThis is an email message from the M52259DEMO.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your M52259DEMO!!";
#elif defined M52259EVB
    #define SENDERS_EMAIL_ADDRESS      "M52259EVB@uTasker.com"           // fictional Email address of the board being used
    #define EMAIL_SUBJECT              "M52259EVB Test"                  // Email subject
    #define EMAIL_CONTENT              "Hello!!\r\nThis is an email message from the M52259EVB.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your M52259EVB!!";
#elif defined M52259_TOWER                                               // {65}
    #define SENDERS_EMAIL_ADDRESS      "M52259TWR@uTasker.com"           // fictional Email address of the board being used
    #define EMAIL_SUBJECT              "M52259TWR Test"                  // Email subject
    #define EMAIL_CONTENT              "Hello!!\r\nThis is an email message from the M52259TWR.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your M52259TWR!!";
#else
    #define SENDERS_EMAIL_ADDRESS      "M52233DEMO@uTasker.com"          // fictional Email address of the board being used
    #define EMAIL_SUBJECT              "M52233DEMO Test"                 // Email subject
    #define EMAIL_CONTENT              "Hello!!\r\nThis is an email message from the M52233DEMO.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your M52233DEMO!!";
#endif
#define SMTP_ACCOUNT_NAME              "smtp_user"
#define SMTP_PASSWORD                  "smtp_password"


// internal HTML message pages
//
#if FILE_SYSTEM_SIZE >= (156*1024)                                       // {34} activate only when the file system dimensions are adequate
    #define SUPPORT_INTERNAL_HTML_FILES                                  // enable the use of the following files
    #define UPLOAD_FAILED         "<html><head><title>SW Upload failed</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font></i></b></td><br></td><td align=left><br><br>Sorry but upload has failed.</font><br><br><a href=""javascript:history.back()"">Return</a></body></html>"
    #define SW_UPLOAD_COMPLETED   "<html><head><meta http-equiv=""refresh"" content=""10;URL=0Menu.htm""><title>M5223X SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font> - SW Update</i></b></td><br></td><td align=left><br><br>SW Upload successful. The M5223X target will now reset and start the new program. Please wait 10s...</body></html>"
#endif

#if defined USE_TFTP                                                     // {2}
    #define TFTP_ERROR_MESSAGE               "uTasker TFTP Error"        // fixed TFTP error test
#endif


// Interrupt priorities and levels in the project                        {5}
//
// Each interrupt in the system must be defined a unique level and priority.
// Highest priority at the top and lowest priority at the bottom.
// Level 7 interrupts are non-maskable. Do not (generally) use these in the uTasker project, except for the watchdog interrupt
// Lowest level 1
// Priorities 0..7
//

// Level 7 (NMI)
// Level 7 Priority 7 used by Watchdog
// Level 7 Priorities 6..0 - don't use (generally)

// Level 6
//
#define RXFRAME_INTERRUPT_PRIORITY          (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_7) // M5223X
#define USB_OTG_INTERRUPT_PRIORITY          (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_6) // M5222X / M5221X {12}{63}
#define IRQ15_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_5) // {10}
#define CAN15_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_4)
#define CAN14_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_3)
#define CAN13_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_2)
#define CAN12_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_1)
#define CAN11_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_6 | INTERRUPT_PRIORITY_0)

// Level 5
//
// Level 5 priorities
#define DMA_TIMER2_INTERRUPT_PRIORITY       (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_7) // {51}
#define IRQ14_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_6) // {10}
#define CAN10_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_5)
#define CAN9_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_4)
#define CAN8_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_3)
#define CAN7_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_2)
#define CAN6_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_1)
#define CAN5_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_5 | INTERRUPT_PRIORITY_0)

// Level 4
//
// Level 4 priorities
#define DMA_TIMER1_INTERRUPT_PRIORITY       (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_7) // {26}
#define IRQ13_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_6) // {10}
#define CAN4_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_5)
#define CAN3_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_4)
#define CAN2_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_3)
#define CAN1_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_2)
#define CAN0_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_1)
#define TXFRAME_INTERRUPT_PRIORITY          (INTERRUPT_LEVEL_4 | INTERRUPT_PRIORITY_0)

// Level 3
//
#define GPTOV_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_7) // {43}
#define IRQ12_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_6) // {10}
#define UART0_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_5)
#define DMA0_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_4) // DMA on TX UART 0
#define UART1_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_3)
#define DMA1_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_2) // DMA on RX UART 1
#define UART2_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_1)
#define DMA2_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_3 | INTERRUPT_PRIORITY_0) // DMA on TX UART 2

// Level 2
//
#define I2C_1_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_7) // {33}
#define IRQ11_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_6) // {10}
#define DTIM3_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_5)
#define TICK_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_4)
#define EPHY_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_3)
#define I2C_INTERRUPT_PRIORITY              (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_2)
#define ADC_ERR_PRIORITY                    (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_1) // {21}
#define PIT1_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_2 | INTERRUPT_PRIORITY_0) // {23}

// Level 1
//
#define GPTIMER0_INTERRUPT_PRIORITY         (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_7) // {43}
#define RTC_INTERRUPT_PRIORITY              (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_6) // {6}
#define CANERR_INTERRUPT_PRIORITY           (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_5)
#define CANBOFF_INTERRUPT_PRIORITY          (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_4)
#define IRQ10_INTERRUPT_PRIORITY            (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_3) // {10}
#define IRQ9_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_2) // {10}
#define IRQ8_INTERRUPT_PRIORITY             (INTERRUPT_LEVEL_1 | INTERRUPT_PRIORITY_1) // {10}
// Level 1 priorities 0 free

#endif                                                                   // end of #if defined (_M5223X) && !defined (__APP_HW_M5223X__)


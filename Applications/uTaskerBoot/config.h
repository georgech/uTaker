/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        config.h
    Project:     Single Chip Embedded Internet - boot loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    11.08.2007 Add M5223X SPI FLASH support                              {1}
    16.12.2007 Add SAM7X SPI FLASH support                               {2}
    16.12.2007 FLASH and SRAM size set up added                          {3}
    16.12.2007 Add BOOT_LOADER define                                    {4}
    16.12.2007 Add SPI_CS0_PORT define                                   {5}
    02.04.2008 Add LPC23XX support                                       {6}
    02.04.2008 Add new M5223X clock and PLL details                      {7}
    04.04.2008 Add LM3SXXXX support                                      {8}
    31.05.2008 Clean out all non-bootloader defines (M5223X)
    26.06.2008 Add ST SPI FLASH support                                  {9}
    19.08.2008 Add M5222X and M5221X support (USB boot loader)           {10}
    07.01.2009 Change setting to distinguish between M5221x and M5222x   {11}
    17.01.2009 Add various LM3S derivatives                              {12}
    17.01.2009 Add LM3S SPI FLASH support                                {13}
    19.01.2009 Add M5225X support                                        {14}
    25.01.2009 Add SST SPI FLASH                                         {15}
    07.06.2009 Add AVR32 support                                         {16}
    04.04.2010 Add LPC17XX support                                       {17}
    30.07.2010 Add dummy SET_SPI_FLASH_MODE() and REMOVE_SPI_FLASH_MODE() for compatibility and correct CONFIGURE_CS_LINES() for Luminary {18}
    29.11.2010 Add RX6XX support                                         {19}
    03.03.2011 Correct SPI_FLASH_AT45DB161, SPI_FLASH_AT45DB321 and SPI_FLASH_AT45DB642 sizes
    29.03.2011 Add Kinetis support                                       {20}
    26.10.2012 Make READ_SPI_FLASH_DATA() access volatile                {21} [removed later]
    24.08.2013 Add option for multiple intermediate locations            {22}
    25.08.2013 Add NET_KBED and NET_K60                                  {23}
    18.04.2016 Add specific FRDM-K64F target                             {24}
    02.02.2017 Adapt for us tick resolution (_TICK_RESOLUTION)           {25}
    13.07.2017 Add w25q (windbond) spi flash option and a target for the FRDM-KL82Z
    17.07.2017 Add target for the FRDM-KL25Z
    27.02.2018 Add Macronix SPI flash                                    {26}
    18.06.2018 Merge STM32 project
    30.08.2018 Merge Luminary project

    See this video for details of building and checking the "Bare-Minimum" boot loader: https://youtu.be/lm3M-ZlaFLQ

    */

#if !defined __CONFIG__
#define __CONFIG__

#define NO_STATUS_BAR                                                    // don't use status bar in simulator (to avoid string function requirements)

#define MY_PROJECT_NAME     "uTasker Bootloader"

#define BOOT_LOADER                                                      // {4}


//#define SPI_SW_UPLOAD                                                  // new SW is located in SPI FLASH {1}{2}
//#define SPI_FLASH_MX25L                                                // {26} use Macronix SPI flash rather than ATMEL
//#define SPI_FLASH_W25Q                                                 // use Winbond W25Q SPI flash rather than ATMEL
//#define SPI_FLASH_SST25                                                // {15} use SST SPI FLASH rather than ATMEL
//#define SPI_FLASH_ST                                                   // define that we are using ST FLASH rather than default ATMEL {9}
//#define SPI_DATA_FLASH                                                 // FLASH type is data FLASH supporting sub-sectors (relevant for ST types) {9}
//#define MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                           // {22} allow the intermediate code to be located at multiple possible addresses
#if !defined SPI_SW_UPLOAD
    #define ONLY_INTERNAL_FLASH_STORAGE                                  // use only internal flash drivers
#endif

#define SET_SPI_FLASH_MODE()                                             // {18}
#define REMOVE_SPI_FLASH_MODE()

#if !defined _BOOT_LOADER_ASSEMBLER
    #if defined _CODE_WARRIOR_CF
        #pragma const_strings on                                         // ensure strings are of const type
    #endif

    #define OUR_HEAP_SIZE (unsigned short)((25 * 1024))                  // for simulator compatibility only
    #define _TICK_RESOLUTION     TICK_UNIT_MS(50)                        // {25} for simulator compatibility only

    #if defined _KINETIS                                                 // {20}
        #define TARGET_HW           "Bare-Minimum Boot - Kinetis"
      //#define KINETIS_K12                                              // development board with K12
          //#define DEV2                                                 // temporary development version
      //#define FRDM_KL25Z
        #define FRDM_KL27Z
      //#define CAPUCCINO_KL27                                           // KL27 with 256k flash / 32k SRAM
          //#define DEV4                                                 // temporary development version
          //#define DEV5                                                 // temporary development version
      //#define FRDM_KL82Z
      //#define KINETIS_K40
      //#define KINETIS_K60
          //#define DEV3                                                 // temporary development version
      //#define FRDM_K64F                                                // {24} next generation K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area
      //#define KINETIS_K70

      //#define NET_KBED
      //#define NET_K60
        #if defined FRDM_K64F
            #define MASK_1N83J
            #define KINETIS_MAX_SPEED    120000000
            #define KINETIS_K_FPU                                        // part with floating point unit
            #define KINETIS_K60                                          // specify the sub-family
            #define KINETIS_REVISION_2
            #define KINETIS_K64                                          // extra sub-family type precision
          //#define RUN_FROM_DEFAULT_CLOCK                               // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
          //#define RUN_FROM_HIRC                                        // clock directly from internal 48MHz RC clock
            #define RUN_FROM_HIRC_PLL                                    // use 48MHz RC clock as input to the PLL
          //#define RUN_FROM_HIRC_FLL                                    // use 48MHz RC clock as input to the FLL
          //#define RUN_FROM_RTC_FLL                                     // use 32.76kHz crystal clock as input to the FLL
            #if defined RUN_FROM_DEFAULT_CLOCK
              //#define FLL_FACTOR           2929                        // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                #define FLEX_CLOCK_DIVIDE    1                           // approx. 22.5MHz
                #define FLASH_CLOCK_DIVIDE   1                           // approx. 22.5MHz 
                #define BUS_CLOCK_DIVIDE     1                           // approx. 22.5MHz
            #elif defined RUN_FROM_HIRC                                  // use IRC48M internal oscillator directly (no PLL or FLL)
                #define FLEX_CLOCK_DIVIDE    2                           // approx. 24MHz
                #define FLASH_CLOCK_DIVIDE   2                           // approx. 24MHz 
                #define BUS_CLOCK_DIVIDE     1                           // approx. 48MHz
            #elif defined RUN_FROM_HIRC_FLL
                #define EXTERNAL_CLOCK       48000000                    // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define FLL_FACTOR           2929                        // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                #define FLEX_CLOCK_DIVIDE    3                           // 120/3 to give 40MHz
                #define FLASH_CLOCK_DIVIDE   5                           // 120/5 to give 24MHz
            #elif defined RUN_FROM_RTC_FLL
                #define EXTERNAL_CLOCK       32768
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define FLL_FACTOR           2929                        // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                #define FLEX_CLOCK_DIVIDE    3                           // 96/2 to give 48MHz
                #define FLASH_CLOCK_DIVIDE   4                           // 96/4 to give 24MHz
            #elif defined RUN_FROM_HIRC_PLL
                #define EXTERNAL_CLOCK       48000000                    // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define CLOCK_DIV            20                          // input must be divided to 2MHz..4MHz range (/1 to /24)
                #define CLOCK_MUL            50                          // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
                #define FLEX_CLOCK_DIVIDE    3                           // 120/3 to give 40MHz
                #define FLASH_CLOCK_DIVIDE   5                           // 120/5 to give 24MHz
            #else                                                        // run from external clock (typical configuration when Ethernet is required)
                #define EXTERNAL_CLOCK       50000000                    // this must be 50MHz in order to use Ethernet in RMII mode
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define CLOCK_DIV            20                          // input must be divided to 2MHz..4MHz range (/1 to /24)
                #define CLOCK_MUL            48                          // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
                #define FLEX_CLOCK_DIVIDE    3                           // 120/3 to give 40MHz
                #define FLASH_CLOCK_DIVIDE   5                           // 120/5 to give 24MHz
            #endif
          //#define USB_CRYSTAL_LESS                                     // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
          //#define USB_CLOCK_GENERATED_INTERNALLY                       // use USB clock from internal source rather than external pin - 120MHz is suitable from PLL

            #define SIZE_OF_FLASH      (1024 * 1024)                     // K64 has 1M flash
            #define SIZE_OF_RAM        (256 * 1024)                      // and 256k SRAM

            #define FILE_GRANULARITY   (1 * FLASH_GRANULARITY)           // each file a multiple of 2k/4k

            #define uFILE_START        0x60000                           // FLASH location at 384k start
            #define FILE_SYSTEM_SIZE   (128 * 1024)                      // 128k reserved for file system

            #define PIN_COUNT          PIN_COUNT_100_PIN                 // used only by the simulator
            #define KINETIS_K_FPU                                        // 120MHz FPU version
        #elif defined FRDM_KL25Z
            #define OSC_LOW_GAIN_MODE                                    // oscillator without feedback resistor or load capacitors so use low gain mode
          //#define RUN_FROM_LIRC                                        // clock from internal 4MHz RC clock
          //#define RUN_FROM_DEFAULT_CLOCK                               // run from FLL default setting
          //#define RUN_FROM_EXTERNAL_CLOCK                              // run directly from external 8MHz clock (without PLL)
            #define CRYSTAL_FREQUENCY    8000000                         // 8 MHz crystal
            #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
            #define CLOCK_DIV            4                               // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
            #define CLOCK_MUL        48                                  // the PLL multiplication factor to achieve MCGPLLCLK operating frequency of 98MHz (x24 to x55 possible) (MCGPLLCLK/2 is 48MHz - required by USB)
            #define SYSTEM_CLOCK_DIVIDE 2                                // divide (1,2,3..16 possible) to get core clock of 48MHz
            #define BUS_CLOCK_DIVIDE    2                                // divide from core clock for bus and flash clock (1,2,3..8 possible) 24MHz
            #define FLASH_CLOCK_DIVIDE  2
            #define KINETIS_KL
            #define KINETIS_KL25
            #define MASK_2N97F
            #define PIN_COUNT           PIN_COUNT_80_PIN                 // 80 pin package
            #define PACKAGE_TYPE        PACKAGE_LQFP                     // LQFP
            #define SIZE_OF_FLASH       (128 * 1024)                     // 128k program Flash
            #define SIZE_OF_RAM         (16 * 1024)                      // 16k SRAM

            #define uFILE_START        (0)
            #define FILE_SYSTEM_SIZE   (SIZE_OF_FLASH)                   // 128k reserved for file system
            #define FILE_GRANULARITY   (1 * FLASH_GRANULARITY)           // each file a multiple of 1k
        #elif defined FRDM_KL27Z || defined CAPUCCINO_KL27
            #define KINETIS_KL
            #define KINETIS_KL27
            #define OSC_LOW_GAIN_MODE
            #define CRYSTAL_FREQUENCY    32768                           // 32kHz crystal
          //#define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
            #define RUN_FROM_HIRC                                        // clock from internal 48MHz RC clock
          //#define RUN_FROM_LIRC                                        // clock from internal 8MHz RC clock
              //#define RUN_FROM_LIRC_2M                                 // clock from internal 2MHz RC clock
            #define SYSTEM_CLOCK_DIVIDE  1                               // system clock divider value (1..16)
            #define BUS_CLOCK_DIVIDE     2                               // bus and flash clock divider value (1..8)
            #define USB_CRYSTAL_LESS                                     // use 48MHz HIRC as USB source (according to Freescale AN4905 - only possible in device mode) - rather than external pin
            #define ERRATE_1N87M
            #if defined CAPUCCINO_KL27
                #if defined DEV5
                    #define PIN_COUNT       PIN_COUNT_32_PIN
                    #define PACKAGE_TYPE    PACKAGE_QFN
                    #define SIZE_OF_FLASH   (256 * 1024)                 // 256k program Flash
                    #define SIZE_OF_RAM     (32 * 1024)                  // 32k SRAM
                    #define uFILE_START     (FLASH_START_ADDRESS + (214 * 1024)) // FLASH location at 0x35800 start
                    #define FILE_SYSTEM_SIZE (40 * 1024)                 // 40k application accepted
                #elif defined DEV4
                    #define PIN_COUNT       PIN_COUNT_48_PIN
                    #define PACKAGE_TYPE    PACKAGE_QFN
                    #define SIZE_OF_FLASH   (128 * 1024)                 // 128k program Flash
                    #define uFILE_START     (FLASH_START_ADDRESS + (64 * 1024)) // FLASH location at 0x10000 start
                #else
                    #define PIN_COUNT       PIN_COUNT_32_PIN
                    #define PACKAGE_TYPE    PACKAGE_QFN
                    #define SIZE_OF_FLASH   (256 * 1024)                 // 256k program Flash
                    #define uFILE_START     (FLASH_START_ADDRESS + (214 * 1024)) // FLASH location at 0x35800 start
                #endif
                #define SIZE_OF_RAM         (32 * 1024)                  // 32k SRAM
                #define FILE_SYSTEM_SIZE    (40 * 1024)                  // 40k application accepted
            #else
                #define PIN_COUNT           PIN_COUNT_64_PIN             // 64 pin package
                #define PACKAGE_TYPE        PACKAGE_LQFP                 // LQFP
                #define SIZE_OF_FLASH       (64 * 1024)                  // 64k program Flash
                #define SIZE_OF_RAM         (16 * 1024)                  // 16k SRAM
                #define FILE_SYSTEM_SIZE    (SIZE_OF_FLASH/2)            // half of the flash reserved for file system
                #define uFILE_START         (SIZE_OF_FLASH/2)            // FLASH location at half of the flash
            #endif
            #if defined DEV4
                #define FILE_GRANULARITY    (8 * SPI_FLASH_BLOCK_LENGTH) // 32k file granularity
            #else
                #define FILE_GRANULARITY    (1 * FLASH_GRANULARITY)      // each file a multiple of 1k
            #endif
        #elif defined FRDM_KL82Z
          //#define RUN_FROM_DEFAULT_CLOCK                               // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
          //#define RUN_FROM_HIRC                                        // clock directly from internal 48MHz RC clock
          //#define RUN_FROM_HIRC_PLL                                    // use 48MHz RC clock as input to the PLL
          //#define RUN_FROM_HIRC_FLL                                    // use 48MHz RC clock as input to the FLL
          //#define RUN_FROM_RTC_FLL                                     // use 32.76kHz crystal clock as input to the FLL
            #if defined RUN_FROM_DEFAULT_CLOCK
              //#define FLL_FACTOR           2929                        // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                #define FLEX_CLOCK_DIVIDE    1                           // approx. 22.5MHz
                #define FLASH_CLOCK_DIVIDE   1                           // approx. 22.5MHz 
                #define BUS_CLOCK_DIVIDE     1                           // approx. 22.5MHz
            #elif defined RUN_FROM_HIRC                                  // use IRC48M internal oscillator directly (no PLL or FLL)
                #define FLEX_CLOCK_DIVIDE    2                           // approx. 24MHz
                #define FLASH_CLOCK_DIVIDE   2                           // approx. 24MHz 
                #define BUS_CLOCK_DIVIDE     1                           // approx. 48MHz
            #elif defined RUN_FROM_HIRC_FLL
                #define EXTERNAL_CLOCK       48000000                    // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define FLL_FACTOR           2929                        // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                #define FLEX_CLOCK_DIVIDE    3                           // 120/3 to give 40MHz
                #define FLASH_CLOCK_DIVIDE   5                           // 120/5 to give 24MHz
            #elif defined RUN_FROM_RTC_FLL
                #define EXTERNAL_CLOCK       32768
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define FLL_FACTOR           2929                        // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                #define FLEX_CLOCK_DIVIDE    3                           // 96/2 to give 48MHz
                #define FLASH_CLOCK_DIVIDE   4                           // 96/4 to give 24MHz
            #elif defined RUN_FROM_HIRC_PLL
                #define EXTERNAL_CLOCK       48000000                    // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
                #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                #define CLOCK_DIV            20                          // input must be divided to 2MHz..4MHz range (/1 to /24)
                #define CLOCK_MUL            50                          // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
                #define FLEX_CLOCK_DIVIDE    3                           // 120/3 to give 40MHz
                #define FLASH_CLOCK_DIVIDE   5                           // 120/5 to give 24MHz
            #else
                #if defined FRDM_K66F || defined FRDM_KL82Z
                    #define CRYSTAL_FREQUENCY    12000000                // 12 MHz crystal
                    #define CLOCK_DIV            1                       // input must be divided to 8MHz..16MHz range (/1 to /8)
                #else
                    #define CRYSTAL_FREQUENCY    16000000                // 16 MHz crystal
                    #define CLOCK_DIV            2                       // input must be divided to 8MHz..16MHz range (/1 to /8)
                #endif
                #define OSC_LOW_GAIN_MODE
                #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
              //#define USE_HIGH_SPEED_RUN_MODE
                #if defined USE_HIGH_SPEED_RUN_MODE                      // high speed run mode allow faster operation but can't program/erase flash
                    #if defined FRDM_KL82Z
                        #define CLOCK_MUL        16                      // the PLL multiplication factor to achieve operating frequency of 96MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
                    #elif defined FRDM_K66F
                        #define CLOCK_MUL        30                      // the PLL multiplication factor to achieve operating frequency of 180MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
                    #else
                        #define CLOCK_MUL        45                      // the PLL multiplication factor to achieve operating frequency of 180MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
                    #endif
                    #if defined FRDM_KL82Z
                        #define BUS_CLOCK_DIVIDE     4                   // 96/4 to give 24MHz (max. 24MHz)
                        #define QSPI_CLOCK_DIVIDE    1                   // 96/1 to give 96MHz (max. 96MHz)
                        #define FLASH_CLOCK_DIVIDE   4                   // 96/4 to give 24MHz (max. ?MHz)
                    #else
                        #define BUS_CLOCK_DIVIDE     3                   // 180/3 to give 60MHz (max. 60MHz)
                        #define FLEX_CLOCK_DIVIDE    3                   // 180/3 to give 60MHz (max. 60MHz)
                        #define FLASH_CLOCK_DIVIDE   7                   // 180/7 to give 25.714MHz (max. 28MHz)
                    #endif
                #else                                                    // run mode has no functional restrictions but can't operate as fast as high speed run mode
                    #if defined FRDM_KL82Z
                        #define CLOCK_MUL        24                      // the PLL multiplication factor to achieve operating frequency of 144MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
                    #elif defined TEENSY_3_6 || defined TWR_K65F180M
                        #define CLOCK_MUL        30                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
                    #else
                        #define CLOCK_MUL        20                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
                    #endif
                    #if defined FRDM_KL82Z
                        #define SYSTEM_CLOCK_DIVIDE  2                   // 144/2 to give 72MHz
                        #define BUS_CLOCK_DIVIDE     6                   // 144/6 to give 24MHz (max. 24MHz)
                        #define QSPI_CLOCK_DIVIDE    2                   // 144/2 to give 72MHz (max. 72MHz)
                        #define FLASH_CLOCK_DIVIDE   6                   // 144/6 to give 24MHz (max. 24MHz)
                    #else
                        #define BUS_CLOCK_DIVIDE     2                   // 120/2 to give 60MHz (max. 60MHz)
                        #define FLEX_CLOCK_DIVIDE    2                   // 120/2 to give 60MHz (max. 60MHz)
                        #define FLASH_CLOCK_DIVIDE   5                   // 120/7 to give 24MHz (max. 28MHz)
                    #endif 
                #endif
            #endif
            #define KINETIS_KL
            #define KINETIS_KL82
            #define PIN_COUNT           PIN_COUNT_80_PIN                 // 80 pin package
            #define PACKAGE_TYPE        PACKAGE_LQFP                     // LQFP
            #define SIZE_OF_FLASH       (128 * 1024)                     // 128k program Flash
            #define SIZE_OF_RAM         (96 * 1024)                      // 96k SRAM

            #define uFILE_START        (0)
            #define FILE_SYSTEM_SIZE   (SIZE_OF_FLASH)                   // 128k reserved for file system
            #define FILE_GRANULARITY   (1 * FLASH_GRANULARITY)           // each file a multiple of 4k
        #elif defined KINETIS_K12
            #define KINETIS_K10                                          // specify the sub-family
            #define KINETIS_K12                                          // extra sub-family type precision

            #define KINETIS_MAX_SPEED    50000000
            #define RUN_FROM_DEFAULT_CLOCK                               // run from FLL default setting
            #define FLL_FACTOR           1464                            // adjust FLL to give 47.972MHz
            #define SYSTEM_CLOCK_DIVIDE  1                               // divide (1,2,3..16 possible) to get core clock of about 48MHz
            #define BUS_CLOCK_DIVIDE     2                               // divide from core clock for bus and flash clock (1,2,3..8 possible) 24MHz
            #define FLASH_CLOCK_DIVIDE   2

            #define MASK_0N36M                                           // enable errata workarounds for this mask
            #define PIN_COUNT            PIN_COUNT_48_PIN                // 48 pin LQFP package

            #define SIZE_OF_RAM         (16 * 1024)                      // 16k SRAM
            #define SIZE_OF_FLASH       (128 * 1024)                     // 128k program FLASH
            #define FILE_GRANULARITY    (1 * FLASH_GRANULARITY)          // each file a multiple of 2k/4k
            #define uFILE_START         0x14000                          // FLASH location at 80k start
            #define FILE_SYSTEM_SIZE    (48 * 1024)                      // 48k reserved for file system
        #else
            // Initialise for 100MHz(120MHz) from 50MHz external clock
            //
            #if defined KINETIS_K60 || defined KINETIS_K70
                #define KINETIS_MAX_SPEED        100000000
                #define RUN_FROM_DEFAULT_CLOCK                           // 32768Hz IRC
                #if defined RUN_FROM_DEFAULT_CLOCK
                    #define FLL_FACTOR           2929                    // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
                    #define SYSTEM_CLOCK_DIVIDE  1
                    #define BUS_CLOCK_DIVIDE     2
                    #define FLASH_CLOCK_DIVIDE   4
                    #define FLEX_CLOCK_DIVIDE    2
                    #define PIN_COUNT            PIN_COUNT_144_PIN
                #elif defined KINETIS_K_FPU
                    #define EXTERNAL_CLOCK       50000000                // this must be 50MHz in order to use Ethernet in RMII mode
                    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                    #define CLOCK_DIV            5                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
                    #define CLOCK_MUL            24                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCC output)
                    #define FLEX_CLOCK_DIVIDE    3                       // 120/3 to give 40MHz
                    #define FLASH_CLOCK_DIVIDE   5                       // 120/5 to give 24MHz
                    #if defined KINETIS_K70
                        #define PIN_COUNT        PIN_COUNT_256_PIN
                    #endif
                #else
                    #define EXTERNAL_CLOCK       50000000                // this must be 50MHz in order to use Ethernet in RMII mode
                    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
                    #define CLOCK_DIV            16                      // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
                    #define CLOCK_MUL            32                      // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
                    #define PIN_COUNT            PIN_COUNT_100_PIN
                #endif
            #else
                #define CRYSTAL_FREQUENCY    8000000                     // 8MHz crystal on K40 board
                #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
                #define CLOCK_DIV            4                           // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
                #define CLOCK_MUL            48                          // the PLL multiplication factor to achieve operating frequency of 96MHz (x24 to x55 possible)
                #define PIN_COUNT            PIN_COUNT_100_PIN
            #endif

            #define SIZE_OF_FLASH      (512 * 1024)                      // K60 has 512k
            #define SIZE_OF_RAM        (64 * 1024)                       // suitable for K40, K60 and K70

            #define FILE_GRANULARITY   (1 * FLASH_GRANULARITY)           // each file a multiple of 2k/4k
            #if defined KINETIS_K60 && defined DEV3
                #define uFILE_START        (250 * 1024)                  // FLASH location at 250k start
                #define FILE_SYSTEM_SIZE   ((124 - 4) * 1024)            // 122k reserved for file system - maximum upload size (after 4k boot)
            #else
                #define uFILE_START        0x60000                       // FLASH location at 384k start
                #define FILE_SYSTEM_SIZE   (128 * 1024)                  // 128k reserved for file system
            #endif
        #endif
        #if !defined SYSTEM_CLOCK_DIVIDE
            #define SYSTEM_CLOCK_DIVIDE  1                               // 1 to 16 - usually 1
        #endif
        #if !defined BUS_CLOCK_DIVIDE
            #define BUS_CLOCK_DIVIDE     2                               // 1 to 16
        #endif
        #if !defined FLEX_CLOCK_DIVIDE
            #define FLEX_CLOCK_DIVIDE    2                               // 1 to 16
        #endif
        #if !defined FLASH_CLOCK_DIVIDE
            #define FLASH_CLOCK_DIVIDE   4                               // 1 to 16
        #endif

        // Include the hardware header here
        // - beware that the header delivers rules for subsequent parts of this header file but also accepts some rules from previous parts,
        // therefore its position should only be moved after careful consideration of its consequences
        //
        #include "types.h"                                               // project specific type settings and the processor header at this location

        // FLASH configuration settings
        //
        #define BACKDOOR_KEY_0     0
        #define BACKDOOR_KEY_1     0
        #define BACKDOOR_KEY_2     0
        #define BACKDOOR_KEY_3     0
        #define BACKDOOR_KEY_4     0
        #define BACKDOOR_KEY_5     0
        #define BACKDOOR_KEY_6     0
        #define BACKDOOR_KEY_7     0

        #define KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY       {BACKDOOR_KEY_0, BACKDOOR_KEY_1, BACKDOOR_KEY_2, BACKDOOR_KEY_3, BACKDOOR_KEY_4, BACKDOOR_KEY_5, BACKDOOR_KEY_6, BACKDOOR_KEY_7}
        #define KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION (0xffffffff) // PROT[31:24]:PROT[23:16]:PROT[15:8]:PROT[7:0] - no protection when all are '1'
        #define KINETIS_FLASH_CONFIGURATION_SECURITY           (FTFL_FSEC_SEC_UNSECURE | FTFL_FSEC_FSLACC_GRANTED | FTFL_FSEC_MEEN_ENABLED | FTFL_FSEC_KEYEN_ENABLED)
        #if defined KINETIS_KL || defined KINETIS_KV || defined KINETIS_KE15
            #if defined ROM_BOOTLOADER
            #if !defined FRDM_KL28Z && !defined KINETIS_KE15
                #define BOOTLOADER_ERRATA
            #endif
                #if defined CAPUCCINO_KL27 && defined DEV5
                    #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_1 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_ENABLE | FTFL_FOPT_NMI_DISABLED) // use boot ROM if NMI is held low at reset
                #elif defined TWR_KL43Z48M || defined FRDM_KL43Z || defined FRDM_KL03Z || defined FRDM_KL27Z || defined FRDM_KL28Z || defined FRDM_KL82Z || defined CAPUCCINO_KL27 || defined KINETIS_KE15
                    #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_1 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_NMI_DISABLED) // never use boot ROM
                  //#define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_1 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_ENABLE | FTFL_FOPT_NMI_DISABLED) // use boot ROM if NMI is held low at reset
                  //#define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION (FTFL_FOPT_BOOTSRC_SEL_ROM | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_FAST_INIT | FTFL_FOPT_LPBOOT_CLK_DIV_1 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_NMI_DISABLED) // always use boot ROM
                #else
                    #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_1 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_NMI_ENABLED)
                #endif
            #else
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_8 | FTFL_FOPT_RESET_PIN_ENABLED)
            #endif
        #else
            #if defined KINETIS_REVISION_2
                #if defined FRDM_K64F
                    #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_DISABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_DISABLED) // there is a large capacitor on the NMI/EzP_CS input so these are disabled to allow it to start without requiring an NMI handler or moving to EzPort mode
                #else
                    #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_ENABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_DISABLED)
                #endif
            #else
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_ENABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_ENABLED)
            #endif
        #endif
        #define KINETIS_FLASH_CONFIGURATION_EEPROM_PROT        0xff
        #define KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT     0xff

        #if !defined KINETIS_KL || defined KINETIS_KL82
            #define CONFIGURE_WATCHDOG()  UNLOCK_WDOG(); WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_CLKSRC); // disable watchdog
          //#define CONFIGURE_WATCHDOG()  UNLOCK_WDOG(); WDOG_TOVALL = /2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN); // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
        #else
            #define CONFIGURE_WATCHDOG()  SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
        #endif
        #if defined NET_KBED || defined NET_K60                          // {23} these require the external PHY to be configured with the correct clock speed before continuing
            #define USER_STARTUP_CODE   PHY_RESET_20MS
            #if defined NET_KBED
                #define INIT_PHY_RESET()     _CONFIG_PORT_OUTPUT_FAST_HIGH(A, (PORTA_BIT29), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)) // PA29 is the PHY-/RESET 
                #define PHY_RESET_LOW()      _CLEARBITS(A, PORTA_BIT29)    
                #define PHY_RESET_HIGH()     _SETBITS(A, PORTA_BIT29)    
                #define PHY_RESET_20MS       INIT_PHY_RESET(); PHY_RESET_LOW(); fnDelayLoop((unsigned long)((float)20000 * (float)((float)25000000/(float)SYSTEM_CLOCK))); PHY_RESET_HIGH(); // 20ms PHY Reset (during this ext. CPU clock is not available) !    
            #elif defined NET_K60
                #define FORCE_PHY_CONFIG                                                                                         //     
                #define INIT_PHY_RESET()     _CONFIG_PORT_OUTPUT_FAST_HIGH(A, (PORTA_BIT25), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)) // PA25 is the PHY-/RESET 
                #define PHY_RESET_LOW()      _CLEARBITS(A, PORTA_BIT25)    
                #define PHY_RESET_HIGH()     _SETBITS(A, PORTA_BIT25)    
                #define PHY_RESET_20MS       INIT_PHY_RESET(); PHY_RESET_LOW(); fnDelayLoop((unsigned long)((float)20000 * (float)((float)25000000/(float)SYSTEM_CLOCK))); PHY_RESET_HIGH(); // 20ms PHY Reset (during this ext. CPU clock is not available) !
            #endif
        #endif

        // SPI FLASH system setup
        //
        #if defined NET_KBED || defined NET_K60                       // {23} KBED and NETK60 use SPI0
            #if defined NET_KBED
                #define CS0_LINE                     SPI_PUSHR_PCS5   // CS5 line used when SPI FLASH is enabled
            #else
                #define CS0_LINE                     SPI_PUSHR_PCS0   // CS0 line used when SPI FLASH is enabled
            #endif
            #define CS1_LINE                                             // CS1 line used when extended SPI FLASH is enabled
            #define CS2_LINE                                             // CS2 line used when extended SPI FLASH is enabled
            #define CS3_LINE                                             // CS3 line used when extended SPI FLASH is enabled

            #define SPI_CS0_PORT                     ~(SPI0_PUSHR)    // for simulator
            #define SPI_TX_BYTE                      SPI0_PUSHR       // for simulator
            #define SPI_RX_BYTE                      SPI0_POPR        // for simulator

            #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP_ATOMIC(6, SPI0)
            #if defined NET_KBED
            #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(B, 23, (PB_23_SPI0_PCS5 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                _CONFIG_PERIPHERAL(C, 5, (PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                _CONFIG_PERIPHERAL(C, 7, PC_7_SPI0_SIN);\
                _CONFIG_PORT_OUTPUT_FAST_HIGH(C, (PORTC_BIT16), (PORT_SRE_SLOW | PORT_DSE_HIGH));\
                _SETBITS(C, PORTC_BIT16);\
                SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
                //SPI0_CTAR0 = (/*SPI_CTAR_DBR | */ SPI_CTAR_BR_2 | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 6.25MHz speed and 140ns min de-select time (double speed bit removed and divider added)
            #else
                #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(C, 4, (PC_4_SPI0_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                _CONFIG_PERIPHERAL(C, 5, (PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                _CONFIG_PERIPHERAL(C, 7, PC_7_SPI0_SIN);\
                _CONFIG_PORT_OUTPUT_FAST_LOW(C, (PORTC_BIT13), (PORT_SRE_SLOW | PORT_DSE_HIGH));\
                _SETBITS(C, PORTC_BIT13);\
                SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
                //SPI0_CTAR0 = (/*SPI_CTAR_DBR | */ SPI_CTAR_BR_2 | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 6.25MHz speed and 140ns min de-select time (double speed bit removed and divider added)
            #endif

            #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN_ATOMIC(6, SPI0) // power down SPI interface if no SPI Flash detected

            #define FLUSH_SPI_FIFO_AND_FLAGS()       SPI0_MCR |= SPI_MCR_CLR_RXF; SPI0_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);

            #define WRITE_SPI_CMD0(byte)             SPI0_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
            #define WRITE_SPI_CMD0_LAST(byte)        SPI0_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
            #define READ_SPI_FLASH_DATA()            (unsigned char)SPI0_POPR
            #define WAIT_SPI_RECEPTION_END()         while (!(SPI0_SR & SPI_SR_RFDF)) {}
            #define CLEAR_RECEPTION_FLAG()           SPI0_SR |= SPI_SR_RFDF
        #elif defined FRDM_KL25Z
            // - SPI1_CS   PTE-4 (J9-13)
            // - SPI1_SCK  PTE-2 (J9-9)
            // - SPI1_MOSI PTE-1 (J2-20)
            // - SPI1_MISO PTE-3 (J9-11)
            //
            #define CS0_LINE                        PORTE_BIT4           // CS0 line used when SPI FLASH is enabled
            #define CS1_LINE                                             // CS1 line used when extended SPI FLASH is enabled
            #define CS2_LINE                                             // CS2 line used when extended SPI FLASH is enabled
            #define CS3_LINE                                             // CS3 line used when extended SPI FLASH is enabled

            #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(E, ulChipSelectLine)
            #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(E, ulChipSelectLine)

            #define SPI_CS0_PORT                    GPIOE_PDOR           // for simulator
            #define SPI_TX_BYTE                     SPI1_D               // for simulator
            #define SPI_RX_BYTE                     SPI1_D               // for simulator

            #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP_ATOMIC(4, SPI1)
            #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
                                                    _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                                    _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_MISO | PORT_PS_UP_ENABLE)); \
                                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, CS0_LINE, CS0_LINE, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                                    SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                                                    SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                                    (unsigned char)SPI1_S; (unsigned char)SPI1_D

            #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN_ATOMIC(4, SPI1) // power down SPI interface if no SPI Flash detected

            #define FLUSH_SPI_FIFO_AND_FLAGS()      

            #define WRITE_SPI_CMD0(byte)            SPI1_D = (byte)      // write a single byte
            #define WRITE_SPI_CMD0_LAST(byte)       SPI1_D = (byte)      // write final byte
            #define READ_SPI_FLASH_DATA()           (unsigned char)SPI1_D
            #if defined _WINDOWS
                #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {SPI1_S |= SPI_S_SPRF;}
            #else
                #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {}
            #endif
            #define CLEAR_RECEPTION_FLAG()          
            
            #define SET_SPI_FLASH_MODE()                                 // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
            #define REMOVE_SPI_FLASH_MODE()                              // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
        #elif defined CAPUCCINO_KL27
            // - SPI1_CS   PTC-5
            // - SPI1_SCK  PTD-5
            // - SPI1_MOSI PTD-7
            // - SPI1_MISO PTD-6
            //
            #define CS0_LINE                        PORTC_BIT5           // CS0 line used when SPI FLASH is enabled
            #define CS1_LINE                                             // CS1 line used when extended SPI FLASH is enabled
            #define CS2_LINE                                             // CS2 line used when extended SPI FLASH is enabled
            #define CS3_LINE                                             // CS3 line used when extended SPI FLASH is enabled

            #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(C, ulChipSelectLine)
            #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(C, ulChipSelectLine)

            #define SPI_CS0_PORT                    GPIOC_PDOR           // for simulator
            #define SPI_TX_BYTE                     SPI1_D               // for simulator
            #define SPI_RX_BYTE                     SPI1_D               // for simulator

            #if defined DEV4                        // hold FPGA in reset during loader operation
                #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP_ATOMIC(4, SPI1); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, PORTB_BIT17, 0, (PORT_SRE_SLOW | PORT_DSE_LOW))
            #else
                #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP_ATOMIC(4, SPI1)
            #endif
            #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(D, 5, PD_5_SPI1_SCK); \
                                                    _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                                    _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_MISO | PORT_PS_UP_ENABLE)); \
                                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, CS0_LINE, CS0_LINE, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                                    SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                                                    SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                                    (unsigned char)SPI1_S; (unsigned char)SPI1_D

            #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN_ATOMIC(4, SPI1) // power down SPI interface if no SPI Flash detected

            #define FLUSH_SPI_FIFO_AND_FLAGS()      

            #define WRITE_SPI_CMD0(byte)            SPI1_D = (byte)      // write a single byte
            #define WRITE_SPI_CMD0_LAST(byte)       SPI1_D = (byte)      // write final byte
            #define READ_SPI_FLASH_DATA()           (unsigned char)SPI1_D
            #if defined _WINDOWS
                #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {SPI1_S |= SPI_S_SPRF;}
            #else
                #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {}
            #endif
            #define CLEAR_RECEPTION_FLAG()          
            
            #define SET_SPI_FLASH_MODE()                                 // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
            #define REMOVE_SPI_FLASH_MODE()                              // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
        #elif defined FRDM_KL82Z
            // - SPI1_CS   PTE-5
            // - SPI1_SCK  PTE-1
            // - SPI1_SOUT PTE-2
            // - SPI1_SIN  PTE-4
            //
            #define CS0_LINE                        SPI_PUSHR_PCS0       // CS0 line used when SPI FLASH is enabled
            #define CS1_LINE                                             // CS1 line used when extended SPI FLASH is enabled
            #define CS2_LINE                                             // CS2 line used when extended SPI FLASH is enabled
            #define CS3_LINE                                             // CS3 line used when extended SPI FLASH is enabled

            #define SPI_CS0_PORT                    ~(SPI1_PUSHR)        // for simulator
            #define SPI_TX_BYTE                     SPI1_PUSHR           // for simulator
            #define SPI_RX_BYTE                     SPI1_POPR            // for simulator

            #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP_ATOMIC(6, SPI1)
            #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(E, 5, (PE_5_SPI1_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                                    _CONFIG_PERIPHERAL(E, 1, PE_1_SPI1_SCK); \
                                                    _CONFIG_PERIPHERAL(E, 2, (PE_2_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                                    _CONFIG_PERIPHERAL(E, 4, (PE_4_SPI1_SIN | PORT_PS_UP_ENABLE)); \
                                                    SPI1_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5); \
                                                    SPI1_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time

            #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN_ATOMIC(6, SPI1) // power down SPI interface if no SPI Flash detected

            #define FLUSH_SPI_FIFO_AND_FLAGS()      SPI1_MCR |= SPI_MCR_CLR_RXF; SPI1_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);

            #define WRITE_SPI_CMD0(byte)            SPI1_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
            #define WRITE_SPI_CMD0_LAST(byte)       SPI1_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
            #define READ_SPI_FLASH_DATA()           (unsigned char)(SPI1_POPR)
            #define WAIT_SPI_RECEPTION_END()        while ((SPI1_SR & SPI_SR_RFDF) == 0) {}
            #define CLEAR_RECEPTION_FLAG()          SPI1_SR |= SPI_SR_RFDF
            #define SET_SPI_FLASH_MODE()                                 // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
            #define REMOVE_SPI_FLASH_MODE()                              // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
        #else
            #define CS0_LINE                         SPI_PUSHR_PCS0      // CS0 line used when SPI FLASH is enabled

            #define SPI_CS0_PORT                     ~(SPI2_PUSHR)       // for simulator
            #define SPI_TX_BYTE                      SPI2_PUSHR          // for simulator
            #define SPI_RX_BYTE                      SPI2_POPR           // for simulator

            #define POWER_UP_SPI_FLASH_INTERFACE()   POWER_UP_ATOMIC(3, SPI2)
            #define CONFIGURE_SPI_FLASH_INTERFACE()  _CONFIG_PERIPHERAL(D, 11, (PD_11_SPI2_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                     _CONFIG_PERIPHERAL(D, 12, (PD_12_SPI2_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                     _CONFIG_PERIPHERAL(D, 13, (PD_13_SPI2_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                     _CONFIG_PERIPHERAL(D, 14, PD_14_SPI2_SIN);\
                                                     SPI2_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                                     SPI2_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time

            #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN_ATOMIC(3, SPI2) // power down SPI interface if no SPI Flash detected

            #define FLUSH_SPI_FIFO_AND_FLAGS()       SPI2_MCR |= SPI_MCR_CLR_RXF; SPI2_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);


            #define WRITE_SPI_CMD0(byte)             SPI2_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
            #define WRITE_SPI_CMD0_LAST(byte)        SPI2_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
            #define READ_SPI_FLASH_DATA()            (unsigned char)SPI2_POPR // {21}
            #define WAIT_SPI_RECEPTION_END()         while ((SPI2_SR & SPI_SR_RFDF) == 0) {}
            #define CLEAR_RECEPTION_FLAG()           SPI2_SR |= SPI_SR_RFDF
        #endif
            
        #define SET_SPI_FLASH_MODE()                                     // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
        #define REMOVE_SPI_FLASH_MODE()                                  // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices

        #define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH) // SPI FLASH starts immediately after FLASH

        #if defined SPI_FLASH_MULTIPLE_CHIPS
            #if SPI_FLASH_DEVICE_COUNT >= 4
                #define CONFIGURE_CS_LINES()    FIO0SET = (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE); FIO0DIR |= (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE); _SIM_PORTS
            #elif SPI_FLASH_DEVICE_COUNT >= 3
                #define CONFIGURE_CS_LINES()    FIO0SET = (CS0_LINE | CS1_LINE | CS2_LINE); FIO0DIR |= (CS0_LINE | CS1_LINE | CS2_LINE); _SIM_PORTS
            #else
                #define CONFIGURE_CS_LINES()    FIO0SET = (CS0_LINE | CS1_LINE); FIO0DIR |= (CS0_LINE | CS1_LINE); _SIM_PORTS
            #endif
        #else
            #define CONFIGURE_CS_LINES()        FIO0SET = CS0_LINE; FIO0DIR |= CS0_LINE; _SIM_PORTS
        #endif
    #elif defined _STM32
      //#define STM3210C_EVAL                                            // evaluation board with STM32F107VCT
      //#define STM3240G_EVAL                                            // evaluation board with STM32F407IGH6
      //#define ST_MB913C_DISCOVERY                                      // discovery board with STM32F100RB
     // #define ST_MB997A_DISCOVERY                                      // discovery board with STM32F407VGT6
        #define NUCLEO_F411RE                                            // evaluation board with STM32F411RET6 (cortex-m4 with FPU)
      //#define NUCLEO_F429ZI                                            // evaluation board with STM32F429ZIT6
        #if defined STM3210C_EVAL
            #define TARGET_HW       "STM3210C-EVAL (STM32F107VCT)"
        #elif defined STM3240G_EVAL
            #define TARGET_HW       "STM3240C-EVAL (STM32F407IGH6)"
        #elif defined ST_MB997A_DISCOVERY
            #define TARGET_HW       "MB997A DISCOVERY (STM32F407VGT6)"
        #elif defined ST_MB913C_DISCOVERY
            #define TARGET_HW       "MB913C DISCOVERY (STM32F100RBT6B)"
        #elif defined NUCLEO_F411RE
            #define TARGET_HW       "NUCLEO-F411RE (STM32F411RET6)"
        #elif defined NUCLEO_F429ZI
            #define TARGET_HW       "NUCLEO-F429ZI (STM32F429ZI)"
        #endif

        #if defined STM3210C_EVAL                                        // STM32F107VCT (72MHz)
            #define CRYSTAL_FREQ        25000000
          //#define DISABLE_PLL                                          // run from clock source directly
          //#define USE_HSI_CLOCK                                        // use internal HSI clock source
            #define USE_PLL2_CLOCK                                       // use the PLL2 output as PLL input (don't use USE_HSI_CLOCK in this configuration)
            #define PLL2_INPUT_DIV      5                                // clock input is divided by 5 to give 5MHz to the PLL2 input (range 1..16)
            #define PLL2_VCO_MUL        8                                // the pll2 frequency is multiplied by 8 to 40MHz (range 8..14 or 16 or 20)
            #define PLL_INPUT_DIV       5                                // 1..16 - should set the input to pll in the range 3..12MHz - not valid for HSI clock source
            #define PLL_VCO_MUL         9                                // 4..9 where PLL out must be 18..72MHz. Also 65 is accepted as x6.5 (special case)
            #define PIN_COUNT           PIN_COUNT_100_PIN
            #define PACKAGE_TYPE        PACKAGE_LQFP
            #define _STM32F107X                                          // part group
            #define SIZE_OF_RAM         (64 * 1024)                      // 64k SRAM
            #define SIZE_OF_FLASH       (256 * 1024)                     // 256k FLASH
            #define PCLK1_DIVIDE        2
            #define PCLK2_DIVIDE        1
        #elif defined STM3240G_EVAL                                      // STM32F407IGH6 (168MHz)
            #define CRYSTAL_FREQ        25000000
          //#define DISABLE_PLL                                          // run from clock source directly
          //#define USE_HSI_CLOCK                                        // use internal HSI clock source
            #define PLL_INPUT_DIV       25                               // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
            #define PLL_VCO_MUL         336                              // 64 ..432 where VCO must be 64..432MHz
            #define PLL_POST_DIVIDE     2                                // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
            #define PIN_COUNT           PIN_COUNT_176_PIN
            #define PACKAGE_TYPE        PACKAGE_BGA
            #define _STM32F4XX
            #define SIZE_OF_RAM         (128 * 1024)                     // 128k SRAM
            #define SIZE_OF_CCM         (64 * 1024)                      // 64k Core Coupled Memory
            #define SIZE_OF_FLASH       (1024 * 1024)                    // 1M FLASH
            #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                  // power supply is in the range 2.7V..3.6V
            #define PCLK1_DIVIDE        4
            #define PCLK2_DIVIDE        2
            #define HCLK_DIVIDE         1
        #elif defined ST_MB997A_DISCOVERY                                // STM32F407VGT6 (168MHz)
            #define CRYSTAL_FREQ        8000000
          //#define DISABLE_PLL                                          // run from clock source directly
          //#define USE_HSI_CLOCK                                        // use internal HSI clock source
            #define PLL_INPUT_DIV       4                                // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
            #define PLL_VCO_MUL         168                              // 64 ..432 where VCO must be 64..432MHz
            #define PLL_POST_DIVIDE     2                                // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
            #define PIN_COUNT           PIN_COUNT_100_PIN
            #define PACKAGE_TYPE        PACKAGE_LQFP
            #define _STM32F4XX
            #define SIZE_OF_RAM         (128 * 1024)                     // 128k SRAM
            #define SIZE_OF_CCM         (64 * 1024)                      // 64k Core Coupled Memory
            #define SIZE_OF_FLASH       (1024 * 1024)                    // 1M FLASH
            #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                  // power supply is in the range 2.7V..3.6V
            #define PCLK1_DIVIDE        4
            #define PCLK2_DIVIDE        2
            #define HCLK_DIVIDE         1
        #elif defined ST_MB913C_DISCOVERY                                // STM32F100RB (24MHz)
            #define CRYSTAL_FREQ        8000000
          //#define DISABLE_PLL                                          // run from clock source directly
          //#define USE_HSI_CLOCK                                        // use internal HSI clock source
            #define PLL_INPUT_DIV       2                                // 1..16 - should set the input to pll in the range 1..24MHz (with preference near to 8MHz) - not valid for HSI clock source
            #define PLL_VCO_MUL         6                                // 2 ..16 where PLL out must be 16..24MHz
            #define _STM32F100X                                          // part group
            #define SIZE_OF_RAM         (8 * 1024)                       // 8k SRAM
            #define SIZE_OF_FLASH       (128 * 1024)                     // 128k FLASH
            #define PIN_COUNT           PIN_COUNT_64_PIN
            #define PACKAGE_TYPE        PACKAGE_LQFP
            #define STM32F100RB                                          // exact processor type
            #define PCLK1_DIVIDE        2
            #define PCLK2_DIVIDE        1
        #elif defined NUCLEO_F411RE
            #define _STM32F4XX                                           // part family
            #define _STM32F411                                           // part group
            #define STM32_FPU                                            // FPU present
            #define CRYSTAL_FREQ        8000000                          // 4..26MHz possible
          //#define DISABLE_PLL                                          // run from clock source directly
          //#define USE_HSI_CLOCK                                        // use internal HSI clock source
            #define PLL_INPUT_DIV       4                                // 2..64 - should set the input to pll in the range 0.95..2.1MHz (with preference near to 2MHz)
            #define PLL_VCO_MUL         100                              // 64..432 where VCO must be 100..432MHz
            #define PLL_POST_DIVIDE     2                                // post divide VCO by 2, 4, 6, or 8 to get the system clock speed (range 24.. 100Hz)
            #define PIN_COUNT           PIN_COUNT_64_PIN
            #define PACKAGE_TYPE        PACKAGE_LQFP
            #define SIZE_OF_RAM         (128 * 1024)                     // 64k SRAM
            #define SIZE_OF_FLASH       (512 * 1024)                     // 512 FLASH
            #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                  // power supply is in the range 2.7V..3.6V
            #define PCLK1_DIVIDE        4
            #define PCLK2_DIVIDE        2
            #define HCLK_DIVIDE         1
            #if defined SPI_SW_UPLOAD
                #define CS0_LINE                    PORTC_BIT15          // CS0 line used when SPI FLASH is enabled
                #define CS1_LINE                                         // CS1 line used when extended SPI FLASH is enabled
                #define CS2_LINE                                         // CS2 line used when extended SPI FLASH is enabled
                #define CS3_LINE                                         // CS3 line used when extended SPI FLASH is enabled

                #define SPI_CS0_PORT                GPIOC_ODR            // for simulator
                #define __ASSERT_CS(cs_line)        _CLEARBITS(C, cs_line)
                #define __NEGATE_CS(cs_line)        _SETBITS(C, cs_line)

                #define SSPDR_X                     SPI1_DR
                #define SSPSR_X                     SPI1_SR

                // SPI 1 used for SPI Flash interface - speed set to 12.5MHz (PCLK1 100MHz/8)
                //
                #define POWER_UP_SPI_FLASH_INTERFACE()      POWER_UP(APB2, (RCC_APB2ENR_SPI1EN))
                #define POWER_DOWN_SPI_FLASH_INTERFACE()    POWER_DOWN(APB2, (RCC_APB2ENR_SPI1EN))
                #define CONFIGURE_SPI_FLASH_INTERFACE()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, CS0_LINE, (OUTPUT_FAST | OUTPUT_PUSH_PULL), CS0_LINE); \
                _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SPI1_2_I2S2ext), (SPI1_CLK_A_5 | SPI1_MOSI_A_7), (OUTPUT_FAST | OUTPUT_PUSH_PULL | INPUT_PULL_DOWN)); \
                _CONFIG_PERIPHERAL_INPUT(A,  (PERIPHERAL_SPI1_2_I2S2ext), (SPI1_MISO_A_6), INPUT_PULL_DOWN); \
                _RESET_CYCLE_PERIPHERAL(APB2, RCC_APB2RSTR_SPI1RST); \
                SPI1_CR1 = (SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM); \
                SPI1_I2SCFGR = 0; \
                SPI1_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM)
            #endif
        #elif defined NUCLEO_F429ZI
          //#define PT_427                                               // development variation
            #define _STM32F4XX                                           // part group
            #define _STM32F42X
            #if defined (PT_427)
                #define PIN_COUNT           PIN_COUNT_100_PIN
                #define CRYSTAL_FREQ        19660800                     // 19.6608MHz oscillator
                #define _EXTERNAL_CLOCK     CRYSTAL_FREQUENCY            // external clock rather than a crystal
                #define PLL_INPUT_DIV       12                           // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
                #define PLL_VCO_MUL         205                          // 64 ..432 where VCO must be 192..432MHz
                #define _STM32F427
              //#define FLASH_OPTION_SETTING                (/*FLASH_OPTCR_nWRP0 | */FLASH_OPTCR_nWRP1 | FLASH_OPTCR_nWRP2 | FLASH_OPTCR_nWRP3 | FLASH_OPTCR_nWRP4 | FLASH_OPTCR_nWRP5 | FLASH_OPTCR_nWRP6 | FLASH_OPTCR_nWRP7 | FLASH_OPTCR_nWRP8 | FLASH_OPTCR_nWRP9 | FLASH_OPTCR_nWRP10 | FLASH_OPTCR_nWRP11 | FLASH_OPTCR_BOR_LEV_1 | FLASH_OPTCR_RDP_LEVEL_0 | FLASH_OPTCR_USER_nRST_STDBY | FLASH_OPTCR_USER_nRST_STOP | FLASH_OPTCR_USER_WDG_SW)
              //#define FLASH_OPTION_MASK                   0
              //#define FLASH_OPTION_SETTING_1              (FLASH_OPTCR1_nWRP0 | FLASH_OPTCR1_nWRP1 | FLASH_OPTCR1_nWRP2 | FLASH_OPTCR1_nWRP3 | FLASH_OPTCR1_nWRP4 | FLASH_OPTCR1_nWRP5/* | FLASH_OPTCR1_nWRP6 | FLASH_OPTCR1_nWRP7 | FLASH_OPTCR1_nWRP8 | FLASH_OPTCR1_nWRP9*/ | FLASH_OPTCR1_nWRP10 | FLASH_OPTCR1_nWRP11)
                #define FLASH_PROGRAMMING_OPTION_SETTING    (/*FLASH_OPTCR_nWRP0 | */FLASH_OPTCR_nWRP1 | FLASH_OPTCR_nWRP2 | FLASH_OPTCR_nWRP3 | FLASH_OPTCR_nWRP4 | FLASH_OPTCR_nWRP5 | FLASH_OPTCR_nWRP6 | FLASH_OPTCR_nWRP7 | FLASH_OPTCR_nWRP8 | FLASH_OPTCR_nWRP9 | FLASH_OPTCR_nWRP10 | FLASH_OPTCR_nWRP11 | FLASH_OPTCR_BOR_LEV_1 | FLASH_OPTCR_RDP_LEVEL_0 | FLASH_OPTCR_USER_nRST_STDBY | FLASH_OPTCR_USER_nRST_STOP | FLASH_OPTCR_USER_WDG_SW)
                #define FLASH_PROGRAMMING_OPTION_MASK       (FLASH_OPTCR_BOR_LEV_OFF | FLASH_OPTCR_RDP_MASK) // don't change read level protection or brown-out settings
                #define FLASH_PROGRAMMING_OPTION_SETTING_1  (FLASH_OPTCR1_nWRP0 | FLASH_OPTCR1_nWRP1 | FLASH_OPTCR1_nWRP2 | FLASH_OPTCR1_nWRP3 | FLASH_OPTCR1_nWRP4 | FLASH_OPTCR1_nWRP5 | FLASH_OPTCR1_nWRP6 | FLASH_OPTCR1_nWRP7 | FLASH_OPTCR1_nWRP8 | FLASH_OPTCR1_nWRP9 | FLASH_OPTCR1_nWRP10 | FLASH_OPTCR1_nWRP11)
            #else
                #define PIN_COUNT           PIN_COUNT_144_PIN
                #define CRYSTAL_FREQ        8000000                      // 8MHz Crystal
                #define PLL_INPUT_DIV       4                            // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
                #define PLL_VCO_MUL         168                          // 64 ..432 where VCO must be 192..432MHz
                #define _STM32F429
            #endif
            #define PLL_POST_DIVIDE     2                                // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
            #define HCLK_DIVIDE         1                                // HCLK is divided by 1 (1, 2, 4, 8, 16, 64, 128 or 512 are possible) - max. 168MHz
            #define PCLK1_DIVIDE        4                                // PCLK1 is HCLK divided by 4 (1, 2, 4, 8, or 16 are possible) - max. 42MHz
            #define PCLK2_DIVIDE        2                                // PCLK2 is HCLK divided by 2 (1, 2, 4, 8, or 16 are possible) - max. 84MHz
            #define PACKAGE_TYPE        PACKAGE_LQFP
            #define SIZE_OF_RAM         (192 * 1024)                     // 192k SRAM (0x20000000)
            #define SIZE_OF_CCM         (64 * 1024)                      // 64k Core Coupled Memory (0x10000000)
            #define SIZE_OF_FLASH       (2 * 1024 * 1024)                // 2M FLASH
            #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                  // power supply is in the range 2.7V..3.6V
        #endif
        // Include the hardware header here
        // - beware that the header delivers rules for subsequent parts of this header file but also accepts some rules from previous parts,
        // therefore its position should only be moved after careful consideration of its consequences
        //
        #include "types.h"                                               // project specific type settings and the processor header at this location
    #elif defined _LM3SXXXX
        #define TARGET_HW           "Bare-Minimum Boot - Luminary"
      //#define _LM3S10X                                                 // Small package part
      //#define _LM3S6965                                                // LUMINARY EVAL board with Ethernet
      //#define _LM3S8962                                                // LUMINARY EVAL board with Ethernet and CAN
        #define _LM3S1968                                                // LUMINARY EVAL board without Ethernet
      //#define _LM3S5732                                                // LUMINARY EVAL board with USB HOST/DEVICE (64pin)
      //#define _LM3S3748                                                // LUMINARY EVAL board with USB HOST/DEVICE (100pin)
      //#define _LM3S3768                                                // LUMINARY EVAL board with USB HOST/DEVICE/OTG

        #if defined _LM3S1968
            #define _DEV1                                                // special configuration
            #define CRYSTAL_FREQ        8000000
            #define PLL_OUTPUT_FREQ     50000000                         // highest speed possible
            #define PART_CODE           CODE_LM3S1968
            #define PIN_COUNT           PIN_COUNT_100_PIN
            #define PACKAGE_TYPE        PACKAGE_LQFP
        #elif defined _LM3S6965
            #define _LM3S6965
            #define CRYSTAL_FREQ        8000000
            #define PLL_OUTPUT_FREQ     50000000                         // highest speed possible
            #define PART_CODE           CODE_LM3S6965
            #define PIN_COUNT           PIN_COUNT_100_PIN
            #define PACKAGE_TYPE        PACKAGE_LQFP
        #endif

      //#define CRYSTAL_FREQ        8000000
      //#define PLL_OUTPUT_FREQ     50000000                             // highest speed possible
      //#define SIZE_OF_FLASH       (256*1024)
      //#define SIZE_OF_RAM         (64*1024)

        #define USER_REG0_VALUE             0x00b61a00                   // MAC address in user registers (00-1a-b6-00-22-1c)
        #define USER_REG1_VALUE             0x001c2200

        #if defined SPI_SW_UPLOAD
            #define CS0_LINE            PORTA_BIT3                       // CS0 line used when SPI FLASH is enabled
            #define SPI_CS0_PORT        GPIODATA_A
            #define CONFIGURE_CS_LINES() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, CS0_LINE, CS0_LINE);
        #endif
        // Include the hardware header here
        // - beware that the header delivers rules for subsequent parts of this header file but also accepts some rules from previous parts,
        // therefore its position should only be moved after careful consideration of its consequences
        //
        #include "types.h"                                               // project specific type settings and the processor header at this location
    #endif



// General SPI Flash settings
//
#if defined SPI_SW_UPLOAD
    #if defined SPI_FLASH_ST                                             // ST SPI FLASH used
        //#define SPI_FLASH_STM25P05                                   // the available ST chips
        //#define SPI_FLASH_STM25P10
        //#define SPI_FLASH_STM25P20
        //#define SPI_FLASH_STM25P40
        //#define SPI_FLASH_STM25P80                                               
        //#define SPI_FLASH_STM25P16                                               
        //#define SPI_FLASH_STM25P32                                               
        #define SPI_FLASH_STM25P64
        //#define SPI_FLASH_STM25P128

        #if defined SPI_FLASH_STM25P05                                   // 64k
            #define SPI_FLASH_PAGES             (1 * 256)                          
        #elif defined SPI_FLASH_STM25P10                                 // 128k
            #define SPI_FLASH_PAGES             (2 * 256)
        #elif defined SPI_FLASH_STM25P20                                 // 256k
            #define SPI_FLASH_PAGES             (4 * 256)
        #elif defined SPI_FLASH_STM25P40                                 // 512k
            #define SPI_FLASH_PAGES             (8 * 256)
        #elif defined SPI_FLASH_STM25P80                                 // 1M
            #define SPI_FLASH_PAGES             (16 * 256)
        #elif defined SPI_FLASH_STM25P16                                 // 2M
            #define SPI_FLASH_PAGES             (32 * 256)
        #elif defined SPI_FLASH_STM25P32                                 // 4M
            #define SPI_FLASH_PAGES             (64 * 256)
        #elif defined SPI_FLASH_STM25P64                                 // 8M
            #define SPI_FLASH_PAGES             (128 * 256)
        #elif defined SPI_FLASH_STM25P128                                // 16M
            #define SPI_FLASH_PAGES             (256 * 256)
        #endif
        #define SPI_FLASH_PAGE_LENGTH 256
        #if defined SPI_DATA_FLASH
            #define SPI_FLASH_SECTOR_LENGTH (16 * SPI_FLASH_PAGE_LENGTH) // sub-sector size of data FLASH
        #else
            #define SPI_FLASH_SECTOR_LENGTH (256 * SPI_FLASH_PAGE_LENGTH)// sector size of code FLASH
        #endif
        #define SPI_FLASH_BLOCK_LENGTH  SPI_FLASH_SECTOR_LENGTH
    #elif defined SPI_FLASH_MX25L                                        // {26}
      //#define SPI_FLASH_MX25L12845E                                    // specific type used
        #define SPI_FLASH_MX25L1606E                                     // specific type used
        #if defined SPI_FLASH_MX25L12845E
            #define SPI_FLASH_SIZE           (16 * 1024 * 1024)          // 128 Mbits/16 MBytes
        #else
            #define SPI_FLASH_SIZE           (2 * 1024 * 1024)           // 16 Mbits/2 MBytes
        #endif
        #define SPI_FLASH_PAGE_LENGTH        (256)
        #define SPI_FLASH_PAGES              (SPI_FLASH_SIZE/SPI_FLASH_PAGE_LENGTH)
        #define SPI_FLASH_SECTOR_LENGTH      (4 * 1024)                  // sector size of SPI FLASH
        #define SPI_FLASH_SECTORS            (SPI_FLASH_SIZE/SPI_FLASH_SECTOR_LENGTH)
        #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SECTOR_LENGTH     // for compatibility - file system granularity
        //#define SUPPORT_ERASE_SUSPEND                                  // automatically suspend an erase that is in progress when a write or a read is performed in a different sector (advised when FAT used in SPI Flash with block management/wear-levelling)
    #elif defined SPI_FLASH_W25Q
        #define SPI_FLASH_W25Q128
        //#define SPI_FLASH_W25Q16
        #if defined SPI_FLASH_W25Q128
            #define SPI_FLASH_PAGES          (65536)
        #else
            #define SPI_FLASH_PAGES          (8192)
        #endif
        #define SPI_FLASH_PAGE_LENGTH        (256)
        #define SPI_FLASH_SUB_SECTOR_LENGTH  (4 * 1024)                  // sub-sector size of SPI FLASH
        #define SPI_FLASH_HALF_SECTOR_LENGTH (32 * 1024)                 // half-sector size of SPI FLASH
        #define SPI_FLASH_SECTOR_LENGTH      (64 * 1024)                 // sector size of SPI FLASH (not available on A-versions)
        #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_HALF_SECTOR_LENGTH // for compatibility - file system granularity
    #elif defined SPI_FLASH_SST25
        //#define SPI_FLASH_SST25VF010A                                  // the supported SST chips
        //#define SPI_FLASH_SST25LF020A
        //#define SPI_FLASH_SST25LF040A
        //#define SPI_FLASH_SST25VF040B
        //#define SPI_FLASH_SST25VF080B
        #define SPI_FLASH_SST25VF016B
        //#define SPI_FLASH_SST25VF032B

        #if defined SPI_FLASH_SST25VF010A                                // 1Mbit (128k)
            #define SST25_A_VERSION
            #define SPI_FLASH_PAGES             (32)
        #elif defined SPI_FLASH_SST25LF020A                              // 2Mbit (256k)
            #define SST25_A_VERSION
            #define SPI_FLASH_PAGES             (64)
        #elif defined SPI_FLASH_SST25LF040A                              // 4Mbit (512k)
            #define SST25_A_VERSION
            #define SPI_FLASH_PAGES             (128)
        #elif defined SPI_FLASH_SST25VF040B                              // 4Mbit (512k)
            #define SPI_FLASH_PAGES             (128)
        #elif defined SPI_FLASH_SST25VF080B                              // 8Mbit (1M)
            #define SPI_FLASH_PAGES             (256)
        #elif defined SPI_FLASH_SST25VF016B                              // 16Mbit (2M)
            #define SPI_FLASH_PAGES             (512)
        #elif defined SPI_FLASH_SST25VF032B                              // 32Mbit (4M)
            #define SPI_FLASH_PAGES             (1024)
        #endif

        #define SPI_FLASH_SUB_SECTOR_LENGTH  (4 * 1024)                  // sub-sector size of SPI FLASH
        #define SPI_FLASH_HALF_SECTOR_LENGTH (32 * 1024)                 // half-sector size of SPI FLASH
        #define SPI_FLASH_SECTOR_LENGTH      (64 * 1024)                 // sector size of SPI FLASH (not available on A-versions)

        #define SPI_FLASH_PAGE_LENGTH        SPI_FLASH_SUB_SECTOR_LENGTH // for compatibility - smallest erasable block
        #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SUB_SECTOR_LENGTH // for compatibility - file system granularity
    #else                                                                // AT45DBxxx 
        //#define SPI_FLASH_AT45DB011                                    // define the ATMEL type used here
        //#define SPI_FLASH_AT45DB021
        //#define SPI_FLASH_AT45DB041
        #define SPI_FLASH_AT45DB081
        //#define SPI_FLASH_AT45DB161
        //#define SPI_FLASH_AT45DB321
        //#define SPI_FLASH_AT45DB642
        #define SPI_FLASH_AT45XXXXD_TYPE                                 // specify that a D-type rather than a B/C type is being used

        #define SPI_FLASH_ATMEL                                          // default if nothing else defined
        #if defined SPI_FLASH_AT45DB642                                  // define whether used in power of 2 mode or not
            #define SPI_FLASH_PAGE_LENGTH 1056                           // standard page size (B-device only allows 1056)
            //#define SPI_FLASH_PAGE_LENGTH 1024                         // size when power of 2 mode selected (only possible on D-device)
            #define SPI_FLASH_PAGES             (32 * 1024)              // 8Meg part
        #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGE_LENGTH 528                            // standard page size (B/C-device only allows 528)
            //#define SPI_FLASH_PAGE_LENGTH 512                          // size when power of 2 mode selected (only possible on D-device)
            #if defined SPI_FLASH_AT45DB161
                #define SPI_FLASH_PAGES         (4 * 1024)               // 2Meg part
            #else
                #define SPI_FLASH_PAGES         (8 * 1024)               // 4Meg part
            #endif
        #else
            #define SPI_FLASH_PAGE_LENGTH 264                            // standard page size (B-device only allows 264)
            //#define SPI_FLASH_PAGE_LENGTH 256                          // size when power of 2 mode selected (only possible on D-device)
            #if defined SPI_FLASH_AT45DB011
                #define SPI_FLASH_PAGES         (512)                    // 128k part
            #elif defined SPI_FLASH_AT45DB021
                #define SPI_FLASH_PAGES         (1024)                   // 256k part
            #elif defined SPI_FLASH_AT45DB041
                #define SPI_FLASH_PAGES         (2 * 1024)               // 512k part
            #elif defined SPI_FLASH_AT45DB081
                #define SPI_FLASH_PAGES         (4 * 1024)               // 1Meg part
            #endif
        #endif

        #define SPI_FLASH_BLOCK_LENGTH (8 * SPI_FLASH_PAGE_LENGTH)       // block size - a block can be deleted
        #define SPI_FLASH_SECTOR_LENGTH (64 * 4 * SPI_FLASH_PAGE_LENGTH) // exception sector 0a is 2k and sector 0b is 62k
    #endif
    #define SPI_DATA_FLASH_0_SIZE   (SPI_FLASH_PAGES * SPI_FLASH_PAGE_LENGTH) 
    #define SPI_DATA_FLASH_1_SIZE   SPI_DATA_FLASH_0_SIZE 
    #define SPI_DATA_FLASH_2_SIZE   SPI_DATA_FLASH_0_SIZE
    #define SPI_DATA_FLASH_3_SIZE   SPI_DATA_FLASH_0_SIZE
    #if defined SPI_FLASH_MULTIPLE_CHIPS
        #define SPI_FLASH_DEVICE_COUNT  4
        #if SPI_FLASH_DEVICE_COUNT >= 4
            #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE + SPI_DATA_FLASH_3_SIZE)
        #elif SPI_FLASH_DEVICE_COUNT >= 3
            #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE)
        #else
            #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)
        #endif
    #else
        #define SPI_DATA_FLASH_SIZE         SPI_DATA_FLASH_0_SIZE
    #endif
#endif
/**************** Specify a file system for use by FTP, HTML and such functions *******************************/
//#define USE_PARAMETER_BLOCK                                            // enable a parameter block for storing and retrieving non-volatile information
    #define USE_PAR_SWAP_BLOCK                                           // we support a backup block which can be restored if desired

  //#define SPI_FILE_SYSTEM                                              // we have an external file system via SPI interface, rather than internal in FLASH
    #define FLASH_FILE_SYSTEM                                            // we have an internal file system in FLASH

#if defined FLASH_FILE_SYSTEM                                            // if either file system type is defined, ensure we have the file support
    #define ACTIVE_FILE_SYSTEM
#endif
#if defined SPI_FILE_SYSTEM
    #define ACTIVE_FILE_SYSTEM
#endif

#if !defined PT_427
    #define SUPPORT_MIME_IDENTIFIER                                      // if the file type is to be handled (eg. when mixing HTML with JPGs etc.) this should be set - note that the file system header will be adjusted
#endif

#if defined SPI_FILE_SYSTEM
    #define uFILE_START 0                                                // SPI starts at 0 offset

    #define EEPROM_32K                                                   // 16k, 32k or 64k EEPROM

  #if defined EEPROM_16K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device

    #define FILE_GRANULARITY (1024)                                      // file made up of 1k byte FLASH blocks
    #define FILE_SYSTEM_SIZE (16 * FILE_GRANULARITY)                     // 16k reserved for file system (including parameter blocks)
  #endif
  #if defined EEPROM_32K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device

    #define FILE_GRANULARITY (1024)                                      // file made up of 1k byte FLASH blocks
    #define FILE_SYSTEM_SIZE (32 * FILE_GRANULARITY)                     // 32k reserved for file system (including parameter blocks)
  #endif
  #if defined EEPROM_64K
    #define EEPROM_PAGE_SIZE 128                                         // respect larger page size in SPI EEPROM

    #define FILE_GRANULARITY (2 * 1024)                                  // file made up of 2k byte FLASH blocks
    #define FILE_SYSTEM_SIZE (32 * FILE_GRANULARITY)                     // 64k reserved for file system (including parameter blocks)
  #endif
#else
    #if defined _HW_SAM7X                                                // _HW_SAM7X
        #if defined MJB_BUILD_BC2
            #if defined SPI_SW_UPLOAD
                #define uFILE_START (0x100000)                           // FLASH location
                #define FILE_GRANULARITY (4 * FLASH_GRANULARITY)         // each file a multiple of 1k
                #define FILE_SYSTEM_SIZE (256 * FILE_GRANULARITY)        // 125k reserved for file system (including parameter blocks)
            #else
                #define uFILE_START (0x12a000)                           // FLASH location
                #define FILE_GRANULARITY (4 * FLASH_GRANULARITY)         // each file a multiple of 1k
                #define FILE_SYSTEM_SIZE (84 * FILE_GRANULARITY)         // 84k reserved for file system (including parameter blocks)
            #endif
        #elif defined SPI_SW_UPLOAD
            #define uFILE_START (0x100000)                               // FLASH location
            #define FILE_GRANULARITY (4 * FLASH_GRANULARITY)             // each file a multiple of 1k
            #define FILE_SYSTEM_SIZE (256 * FILE_GRANULARITY)            // 256k reserved for file system (including parameter blocks)
        #else
            #define uFILE_START 0x118000                                 // FLASH location at 96k start
            #define FILE_GRANULARITY (4 * FLASH_GRANULARITY)             // each file a multiple of 1k
            #define FILE_SYSTEM_SIZE (160 * FILE_GRANULARITY)            // 160k reserved for file system (including parameter blocks)
        #endif
    #endif
    #if defined _STR91XF                                                 // _STR91XF
        #define uFILE_START 0x80000                                      // FLASH location at 512k start

        #define FILE_GRANULARITY (FLASH_GRANULARITY)                     // each file a multiple of 64k
        #define FILE_SYSTEM_SIZE (8 * FILE_GRANULARITY + 4 * FLASH_GRANULARITY_BANK1) // 544k reserved for file system (including parameter blocks)
    #endif
    #if defined _LPC23XX || defined _LPC17XX                             // NXP
        #define uFILE_START (FLASH_START_ADDRESS)

        #define SUB_FILE_SIZE    (FILE_GRANULARITY / 8)                  // 4k sub file sizes
        #define FILE_GRANULARITY (1 * FLASH_GRANULARITY_LARGE)           // each file a multiple of 32k
        #define FILE_SYSTEM_SIZE (512 * 1024)                            // 512k devce size seen all as file system
    #endif
    #if defined _LM3SXXXX                                                // Luminary {8}
        #define uFILE_START 0x18000                                      // FLASH location at 96k start

        #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)                 // each file a multiple of 1k
        #define FILE_SYSTEM_SIZE (160 * FILE_GRANULARITY)                // 160k reserved for file system (including parameter blocks)
    #endif
#endif

#if defined USE_PARAMETER_BLOCK
  #define PARAMETER_BLOCK_SIZE    FLASH_GRANULARITY                      // use the smallest size for the device

  #if defined USE_PAR_SWAP_BLOCK
    #define PAR_BLOCK_SIZE  (2 * PARAMETER_BLOCK_SIZE)
  #else
    #define PAR_BLOCK_SIZE  (1 * PARAMETER_BLOCK_SIZE)
  #endif
#else
    #define PAR_BLOCK_SIZE  (0)
#endif

#define uFILE_SYSTEM_START     (unsigned char *)(uFILE_START + PAR_BLOCK_SIZE)
#define uFILE_SYSTEM_END       (unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE)
#define LAST_FILE_BLOCK        (unsigned char)((FILE_SYSTEM_SIZE - PAR_BLOCK_SIZE)/FILE_GRANULARITY) // last block in our file system


#if defined SUPPORT_MIME_IDENTIFIER
    #define FILE_HEADER (sizeof(MAX_FILE_LENGTH) + 1)                    // file length followed by MIME identifier
#else
    #define FILE_HEADER (sizeof(MAX_FILE_LENGTH))
#endif


#define PHYSICAL_QUEUES  0


/**********************************************************************************************************/

#include "../../uTasker\uTasker.h"
#include "../../uTasker\driver.h"
#include "../../stack\tcpip.h"
#if defined _WINDOWS
    #include "../../WinSim\WinSim.h"
#endif
#include "../../Hardware/hardware.h"

#if defined SPI_SW_UPLOAD
    extern int fnConfigSPIFileSystem(void);
#endif

/************ uTasker task table is defined here but only used by the hardware module initiating the system ***********/

#if defined OPSYS_CONFIG && defined _WINDOWS                             // this is only set in the hardware module
    #define DEFAULT_NODE_NUMBER 1
    CONFIG_LIMIT OurConfigNr = DEFAULT_NODE_NUMBER;                      // in single node system this can be initialised with a fixed value

    const HEAP_NEEDS ctOurHeap[] = {
        {DEFAULT_NODE_NUMBER, OUR_HEAP_SIZE},                            // our node requires this amount of heap space
        {0}                                                              // end
    };


    const UTASK_TASK ctNodes[] = {                                       // we use a single fixed configuration (single node)
        DEFAULT_NODE_NUMBER,                                             // configuration our single node
        0,                                                               // end of single configuration

        // insert more node configurations here if required
        0                                                                // end of configuration list
    };


    const UTASKTABLEINIT ctTaskTable[] = {
        0
    };
#endif                                                                   // end of task configuration


#if defined _COMPILE_KEIL
    extern int  uTaskerBoot(void);
#else
    extern void uTaskerBoot(void);
#endif
extern unsigned short fnCRC16(unsigned short usCRC, unsigned char *ptrInput, unsigned short usBlockSize);
extern unsigned long fnEraseFlash(unsigned char *ptrSector);
extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size);

#endif
#endif

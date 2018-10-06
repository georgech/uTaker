/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      app_hw_avr32.h
    Project:   uTasker Demonstration project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    09.09.2009 Add EVK1100 configuration                                 {1}
    03.10.2009 Move "../../Hardware/AVR32/AVR32.h" include from types.h to this file, after detailed chip defines {2}
    05.10.2009 Add CGLCD_GLCD_MODE configuration                         {3}
    06.10.2009 Add GLCD configuration for Samsung based GLCD             {4}
    16.10.2009 Add NUMBER_EXTERNAL_SERIAL when serial interface active   {5}
    18.11.2009 Add dummy TICK_INTERRUPT() for compatibility              {6}
    18.11.2009 Add EVK1101 setup and _AT32UC3B support                   {7}
    24.12.2009 Add SD-card interface                                     {8}
    22.02.2010 Correct SPI_FLASH_AT45DB161, SPI_FLASH_AT45DB321 and SPI_FLASH_AT45DB642 sizes {9}
    05.06.2010 Add USB device                                            {10}
    14.09.2010 Add SPI FLASH setup                                       {11}
    04.05.2011 Add GET_SDCARD_WP_STATE()                                 {12}
    19.06.2011 Add AVR32_AT32UC3C_EK                                     {13}
    19.07.2012 Add keypad and frontpanel support                         {14}
    19.07.2012 Add MONITOR_PERFORMANCE support                           {15}
    30.11.2012 Add AVR32_UC3_C2_XPLAINED support                         {16}

    Application specific hardware configuration
*/

#if defined _HW_AVR32
#if !defined __APP_HW_AVR32__
#define __APP_HW_AVR32__

#if defined _WINDOWS
    #define _SIM_PORTS fnSimPorts()
#else
    #define _SIM_PORTS
#endif

#if defined _AT32UC3B                                                    // {7}
  //#define CHIP_48_PIN
    #define CHIP_64_PIN
    #define SIZE_OF_FLASH          (256 * 1024)                          // modify to suit the device used
    #define SIZE_OF_RAM            (32 * 1024)
#elif defined _AT32UC3C                                                  // {13}
    #if defined AVR32_UC3_C2_XPLAINED
        #define CHIP_64_PIN
    #else
      //#define CHIP_64_PIN
      //#define CHIP_100_PIN
        #define CHIP_144_PIN
    #endif
    #define SIZE_OF_FLASH          (512 * 1024)                          // modify to suit the device used
    #define SIZE_OF_RAM            (64 * 1024)
#else
  //#define CHIP_100_PIN
    #define CHIP_144_PIN
    #define SIZE_OF_FLASH          (512 * 1024)                          // modify to suit the device used
    #define SIZE_OF_RAM            (64 * 1024)
#endif


#include "../../Hardware/AVR32/AVR32.h"


#define RETRIG_WDOG_DURING_ERASE                                         // if large files (or *.* by FTP) are used, it is desirable to retrigger the watchdog during the process

//#define RUN_FROM_RC_OSCILLATOR                                         // run from RC oscillator

#if defined RUN_FROM_RC_OSCILLATOR
    #define SLOW_CLOCK             115000                                // RC oscillator
    #define PLL_OUTPUT_FREQ_INT    SLOW_CLOCK
    #define BUS_DIVIDES            0
    #define CPU_HSB_DIVIDE         1
    #define PBA_DIVIDE             1
    #define PBB_DIVIDE             1
    #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
        #define HSB_DIVIDE         1
        #define PBC_DIVIDE         1
    #endif
#else
    #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
        #define CRYSTAL_FREQ       16000000                              // main clock
        #define OSC_STARTUP_TIME   STARTUP_18MS                          // recommended startup time for this crystal
    #else
        #define CRYSTAL_FREQ       12000000                              // main clock
        #define OSC_STARTUP_TIME   STARTUP_18MS                          // recommended startup time for this crystal
    #endif
  //#define DISABLE_PLL                                                  // don't enable the PLL and so run at input clock speed

    #define LOCK_COUNT             16                                    // the number of slow clocks before lock is declared - recommended value
    #define OSC_DIVIDE             1                                     // 1..15
    #ifdef USB_INTERFACE                                                 // {10} set 48MHz CPU speed to allow USB operation
        #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
            #define PLL_MUL        6                                     // 3..16
            #define POST_DIVIDE    2                                     // 1..2
        #else
            #define PLL_MUL        8                                     // 3..16
            #define POST_DIVIDE    2                                     // 1..2
        #endif
    #else
        #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED   // 66MHz maximum
            #define PLL_MUL        8                                     // 3..16
            #define POST_DIVIDE    1                                     // 1..2
        #elif defined _AT32UC3B                                          // {7} 60 MHz maximum
            #define PLL_MUL        10                                    // 3..16
            #define POST_DIVIDE    1                                     // 1..2
        #else                                                            // 66MHz maximum
            #define PLL_MUL        11                                    // 3..16
            #define POST_DIVIDE    1                                     // 1..2
        #endif
    #endif
    #if defined DISABLE_PLL
        #define CPU_HSB_DIVIDE     1
        #define PBB_DIVIDE         1
        #define PBA_DIVIDE         1
        #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
            #define HSB_DIVIDE     1
            #define PBC_DIVIDE     1
        #endif
    #elif defined USB_INTERFACE
        #define CPU_HSB_DIVIDE     1                                     // CPU runs with 48MHz to satisfy USB from single PLL
        #define PBB_DIVIDE         1                                     // PBB bus runs with 48MHz [reducing to unequal CPU_HSB_DIVIDE stops FLASH programming from working. It is generally recommended to keep PBB equal to HSB]
        #define PBA_DIVIDE         2                                     // PBA bus runs with 24MHz
        #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
            #define HSB_DIVIDE     1
            #define PBC_DIVIDE     2
        #endif
    #else
        #define CPU_HSB_DIVIDE     2                                     // CPU runs with 66MHz (assuming highest speed and AT32UC3A)
        #define PBB_DIVIDE         2                                     // PBB bus runs with 66MHz [reducing to unequal CPU_HSB_DIVIDE stops FLASH programming from working. It is generally recommended to keep PBB equal to HSB]
        #define PBA_DIVIDE         4                                     // PBA bus runs with 33MHz
        #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
            #define HSB_DIVIDE     2
            #define PBC_DIVIDE     2
        #endif
    #endif

    #if !defined DISABLE_PLL
        // Rules for setting the frequency
        // The crystal must be between 0.45 and 16MHz (0.4 and 20MHz for _AT32UC3C)
        // The input can be divided by between 1 and 15
        // The PLL can multiply by between 3 and 16
        // The PLL frequency must normally be in the range 80MHz and 240MHz
        // The PLL output frequency is equal to ((CRYSTAL_FREQ / OSC_DIVIDE) * PLL_MUL)
        // The system clock is equal to PLL output frequency divided by POST_DIVIDE
        // The possible CPU/HSB/PBA and PBB divide values are 1,2,4,8,16,32,64,128,256
        // The compiler throws an error if any setting is out of range

        #if (POST_DIVIDE != 2) && (POST_DIVIDE != 1)
            #error master clock post divider has an illegal value (must be 1 or 2)
        #else
            #if POST_DIVIDE == 2
                #define POST_DIVIDE_OPTION   PLLOPTION_OUTPUT_DIV2
            #else
                #define POST_DIVIDE_OPTION   0
            #endif
        #endif

        #if CRYSTAL_FREQ < 450000 || CRYSTAL_FREQ > 16000000
            #error Crystal frequency out of range: must be between 450kHz and 16MHz
        #endif

        #define _PLL_OUTPUT_FREQ     ((CRYSTAL_FREQ/OSC_DIVIDE)*PLL_MUL)
        #define PLL_OUTPUT_FREQ_INT  (_PLL_OUTPUT_FREQ/POST_DIVIDE)

        #if (_PLL_OUTPUT_FREQ < 80000000) || (_PLL_OUTPUT_FREQ > 240000000)
            #error PLL frequency out of range: PLL output frequency must be within 80 and 240MHz
        #endif

        #if (LOCK_COUNT > 63)
            #error PLL lock count range 0..63
        #endif

        #if _PLL_OUTPUT_FREQ >= START_HIGH_PLL_OUTPUT_RANGE
            #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
                #define PLL_SETTING (SCIF_PLL_PLLOPT_VCO_160_240MHZ | POST_DIVIDE_OPTION | SCIF_PLL_PLLOPT_WIDE_BANDWITH_MODE_ENABLED | ((PLL_MUL - 1) << 16) | (OSC_DIVIDE << 8) | (LOCK_COUNT << 24))     
            #else
                #define PLL_SETTING (PLLOPTION_RANGE_160_240MHZ | POST_DIVIDE_OPTION | ((PLL_MUL - 1) << 16) | (OSC_DIVIDE << 8) | (LOCK_COUNT << 24))
            #endif
        #else
            #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
                #define PLL_SETTING (SCIF_PLL_PLLOPT_VCO_80_180MHZ | POST_DIVIDE_OPTION | SCIF_PLL_PLLOPT_WIDE_BANDWITH_MODE_ENABLED | ((PLL_MUL - 1) << 16) | (OSC_DIVIDE << 8) | (LOCK_COUNT << 24))
            #else
                #define PLL_SETTING (PLLOPTION_RANGE_80_180MHZ | POST_DIVIDE_OPTION | ((PLL_MUL - 1) << 16) | (OSC_DIVIDE << 8) | (LOCK_COUNT << 24))
            #endif
        #endif
    #else
        #define PLL_OUTPUT_FREQ_INT    CRYSTAL_FREQ
    #endif

    #if defined _AT32UC3B && ((PLL_OUTPUT_FREQ_INT/CPU_HSB_DIVIDE) > 60000000) // {7}
        #error master clock frequency too high: maximum 60MHz for AT32UC3B
    #elif ((PLL_OUTPUT_FREQ_INT/CPU_HSB_DIVIDE) > 66000000)
        #error master clock frequency too high: maximum 66MHz
    #endif

    #if CPU_HSB_DIVIDE > 1
        #if CPU_HSB_DIVIDE == 2
            #define CPU_DIV  (CPU_DIV_2 | HSB_DIV_2)
        #elif CPU_HSB_DIVIDE == 4
            #define CPU_DIV  (CPU_DIV_4 | HSB_DIV_4)
        #elif CPU_HSB_DIVIDE == 8
            #define CPU_DIV  (CPU_DIV_8 | HSB_DIV_8)
        #elif CPU_HSB_DIVIDE == 16
            #define CPU_DIV  (CPU_DIV_16 | HSB_DIV_16)
        #elif CPU_HSB_DIVIDE == 32
            #define CPU_DIV  (CPU_DIV_32 | HSB_DIV_32)
        #elif CPU_HSB_DIVIDE == 64
            #define CPU_DIV  (CPU_DIV_64 | HSB_DIV_64)
        #elif CPU_HSB_DIVIDE == 128
            #define CPU_DIV  (CPU_DIV_128 | HSB_DIV_128)
        #elif CPU_HSB_DIVIDE == 256
            #define CPU_DIV  (CPU_DIV_256 | HSB_DIV_256)
        #else
             #error illegal CPU divide value: requires power of two and maximum 256
        #endif
    #else
        #define CPU_DIV  0
    #endif

    #if PBA_DIVIDE > 1
        #if PBA_DIVIDE == 2
            #define PBA_DIV  PBA_DIV_2
        #elif PBA_DIVIDE == 4
            #define PBA_DIV  PBA_DIV_4
        #elif PBA_DIVIDE == 8
            #define PBA_DIV  PBA_DIV_8
        #elif PBA_DIVIDE == 16
            #define PBA_DIV  PBA_DIV_16
        #elif PBA_DIVIDE == 32
            #define PBA_DIV  PBA_DIV_32
        #elif PBA_DIVIDE == 64
            #define PBA_DIV  PBA_DIV_64
        #elif PBA_DIVIDE == 128
            #define PBA_DIV  PBA_DIV_128
        #elif PBA_DIVIDE == 256
            #define PBA_DIV  PBA_DIV_256
        #else
            #error illegal PBA divide value: requires power of two and maximum 256
        #endif
    #else
        #define PBA_DIV  0
    #endif

    #if PBB_DIVIDE > 1
        #if PBB_DIVIDE == 2
            #define PBB_DIV  PBB_DIV_2
        #elif PBB_DIVIDE == 4
            #define PBB_DIV  PBB_DIV_4
        #elif PBB_DIVIDE == 8
            #define PBB_DIV  PBB_DIV_8
        #elif PBB_DIVIDE == 16
            #define PBB_DIV  PBB_DIV_16
        #elif PBB_DIVIDE == 32
            #define PBB_DIV  PBB_DIV_32
        #elif PBB_DIVIDE == 64
            #define PBB_DIV  PBB_DIV_64
        #elif PBB_DIVIDE == 128
            #define PBB_DIV  PBB_DIV_128
        #elif PBB_DIVIDE == 256
            #define PBB_DIV  PBB_DIV_256
        #else
            #error illegal PBA divide value: requires power of two and maximum 256
        #endif
    #else
        #define PBB_DIV  0
    #endif

    #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
        #if HSB_DIVIDE > 1
            #if HSB_DIVIDE == 2
                #define HSB_DIV  HSB_DIV_2
            #elif HSB_DIVIDE == 4
                #define HSB_DIV  HSB_DIV_4
            #elif HSB_DIVIDE == 8
                #define HSB_DIV  HSB_DIV_8
            #elif HSB_DIVIDE == 16
                #define HSB_DIV  HSB_DIV_16
            #elif HSB_DIVIDE == 32
                #define HSB_DIV  HSB_DIV_32
            #elif HSB_DIVIDE == 64
                #define HSB_DIV  HSB_DIV_64
            #elif HSB_DIVIDE == 128
                #define HSB_DIV  HSB_DIV_128
            #elif HSB_DIVIDE == 256
                #define HSB_DIV  HSB_DIV_256
            #else
                #error illegal PBA divide value: requires power of two and maximum 256
            #endif
        #else
            #define HSB_DIV  0
        #endif

        #if PBC_DIVIDE > 1
            #if PBC_DIVIDE == 2
                #define PBC_DIV  PBC_DIV_2
            #elif PBC_DIVIDE == 4
                #define PBC_DIV  PBC_DIV_4
            #elif PBC_DIVIDE == 8
                #define PBC_DIV  PBC_DIV_8
            #elif PBC_DIVIDE == 16
                #define PBC_DIV  PBC_DIV_16
            #elif PBC_DIVIDE == 32
                #define PBC_DIV  PBC_DIV_32
            #elif PBC_DIVIDE == 64
                #define PBC_DIV  PBC_DIV_64
            #elif PBC_DIVIDE == 128
                #define PBC_DIV  PBC_DIV_128
            #elif PBC_DIVIDE == 256
                #define PBC_DIV  PBC_DIV_256
            #else
                #error illegal PBA divide value: requires power of two and maximum 256
            #endif
        #else
            #define PBC_DIV  0
        #endif
    #endif

    #define BUS_DIVIDES  (CPU_DIV | PBA_DIV | PBB_DIV)                   // bus clock divider register setting (used by _AT32UC3A and _AT32UC3B)
#endif

// SPI FLASH system setup
//
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    #define uFILE_START 0x0
    #define EEPROM_32K                                                   // device size used
#else
    #define FILES_ADDRESSABLE                                            // file system is addressable (doesn't have to be retrieved)
#endif

#if defined AVR32_EVK1100 || defined AVR32_AT32UC3C_EK                   // these boards have SPI flash connected to SPI1 (others to SPI0)
    #define USE_SPI1_FOR_SPI_FLASH
#endif

// Use SPI0 and control CS0 as port output (also enable clocks to the port) {11}
//
#ifdef USE_SPI1_FOR_SPI_FLASH                                            // when SPI1 is connected to SPI FLASH
    #if defined AVR32_EVK1100                                            // CS on PA10
        #define CS_OUTPUT           PA10
        #define CONFIG_SPI_PORTS()  POWER_UP(PBA, PBAMASK_SPI1); \
                                    _CONFIG_PERIPHERAL_B(0, (SPI1_MISO_B | SPI1_MOSI_B | SPI1_SCK_B)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, CS_OUTPUT, CS_OUTPUT);
        #define SPI_FLASH_PORT_CLR  OVRC_0
        #define SPI_FLASH_PORT_SET  OVRS_0

        #define SPI_CS0_PORT        OVR_0                                // for simulator compatibility
    #elif defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED     // CS on PB24
        #define CS_OUTPUT           PB24
        #define CONFIG_SPI_PORTS()  POWER_UP(PBA, PBAMASK_SPI1); \
                                    _CONFIG_PERIPHERAL_B(0, (SPI1_MISO_B | SPI1_MOSI_B | SPI1_SCK_B)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(1, CS_OUTPUT, CS_OUTPUT);
        #define SPI_FLASH_PORT_CLR  OVRC_1
        #define SPI_FLASH_PORT_SET  OVRS_1

        #define SPI_CS0_PORT        OVR_1                                // for simulator compatibility
    #endif

    #define POWER_SPI()           //POWER_UP(PBA, PBAMASK_SPI1)          // enable clocks to SPI1
    #define REMOVE_POWER_SPI()      POWER_DOWN(PBA, PBAMASK_SPI1)        // disable clocks to SPI1

    #define SPI_MR                  SPI_MR_1                             // map to SPI1
    #define SPI_CSR0                SPI_CSR0_1
    #define SPI_CR                  SPI_CR_1
    #define SPI_TDR                 SPI_TDR_1
    #define SPI_SR                  SPI_SR_1
    #define SPI_RDR                 SPI_RDR_1
#else                                                                    // else SPI0 is connected to SPI FLASH
    #if defined AVR32_EVK1105
        #define CS_OUTPUT           PA10
    #elif defined AVR32_EVK1101
        #define CS_OUTPUT           PA16
    #elif defined AVR32_EVK1104
        #define CS_OUTPUT           PA14
    #elif defined AVR32_UC3_C2_XPLAINED
        #define CS_OUTPUT           PC16
    #endif
    #if defined AVR32_UC3_C2_XPLAINED
        #define CONFIG_SPI_PORTS()  POWER_UP(PBC, PBCMASK_SPI0); \
                                    _CONFIG_PERIPHERAL_A(3, (SPI0_MISO_0_A | SPI0_MOSI_0_A | SPI0_SCK_1_A)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(2, CS_OUTPUT, CS_OUTPUT);

        #define SPI_FLASH_PORT_CLR  OVRC_2
        #define SPI_FLASH_PORT_SET  OVRS_2

        #define POWER_SPI()         //POWER_UP(PBC, PBCMASK_SPI0)        // enable clocks to SPI0
        #define REMOVE_POWER_SPI()  POWER_DOWN(PBC, PBCMASK_SPI0)        // disable clocks to SPI0


        #define SPI_CS0_PORT        OVR_2                                // for simulator compatibility
    #else
        #define CONFIG_SPI_PORTS()  POWER_UP(PBA, PBAMASK_SPI0); \
                                    _CONFIG_PERIPHERAL_A(0, (SPI0_MISO_A | SPI0_MOSI_A | SPI0_SCK_A)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, CS_OUTPUT, CS_OUTPUT);

        #define SPI_FLASH_PORT_CLR  OVRC_0
        #define SPI_FLASH_PORT_SET  OVRS_0

        #define POWER_SPI()         //POWER_UP(PBA, PBAMASK_SPI0)        // enable clocks to SPI0
        #define REMOVE_POWER_SPI()  POWER_DOWN(PBA, PBAMASK_SPI0)        // disable clocks to SPI0

        #define SPI_CS0_PORT        OVR_0                                // for simulator compatibility
    #endif
    #define SPI_MR                  SPI_MR_0                             // map to SPI0
    #define SPI_CSR0                SPI_CSR0_0
    #define SPI_CR                  SPI_CR_0
    #define SPI_TDR                 SPI_TDR_0
    #define SPI_SR                  SPI_SR_0
    #define SPI_RDR                 SPI_RDR_0
#endif

#define CS0_LINE               CS_OUTPUT                                 // CS0 line used when SPI FLASH is enabled
//#define CS1_LINE             SPI_CS0_1                                 // CS1 line used when extended SPI FLASH is enabled
//#define CS2_LINE             SPI_CS0_2                                 // CS2 line used when extended SPI FLASH is enabled
//#define CS3_LINE             SPI_CS0_3                                 // CS3 line used when extended SPI FLASH is enabled

#if defined SPI_FLASH_ST                                                 // ST SPI FLASH used
  //#define SPI_FLASH_STM25P40                                           // the available ST chips
    #define SPI_FLASH_STM25P80
  //#define SPI_FLASH_STM25P16
  //#define SPI_FLASH_STM25P32
  //#define SPI_FLASH_STM25P64

    #if defined SPI_FLASH_STM25P40                                       // 512k
        #define SPI_FLASH_PAGES             (8 * 256)
    #elif defined SPI_FLASH_STM25P80                                     // 1M
        #define SPI_FLASH_PAGES             (16 * 256)
    #elif defined SPI_FLASH_STM25P16                                     // 2M
        #define SPI_FLASH_PAGES             (32 * 256)
    #elif defined SPI_FLASH_STM25P32                                     // 4M
        #define SPI_FLASH_PAGES             (64 * 256)
    #elif defined SPI_FLASH_STM25P64                                     // 8M
        #define SPI_FLASH_PAGES             (128 * 256)
    #endif

    #define SPI_FLASH_PAGE_LENGTH 256
    #ifdef SPI_DATA_FLASH
        #define SPI_FLASH_SECTOR_LENGTH (16 * SPI_FLASH_PAGE_LENGTH)     // sub-sector size of data FLASH
    #else
        #define SPI_FLASH_SECTOR_LENGTH (256 * SPI_FLASH_PAGE_LENGTH)    // sector size of code FLASH
    #endif
    #define SPI_FLASH_BLOCK_LENGTH  SPI_FLASH_SECTOR_LENGTH
#elif defined SPI_FLASH_SST25
    #define SPI_FLASH_SST25VF010A                                        // the supported SST chips
  //#define SPI_FLASH_SST25LF020A
  //#define SPI_FLASH_SST25LF040A
  //#define SPI_FLASH_SST25VF040B
  //#define SPI_FLASH_SST25VF080B
  //#define SPI_FLASH_SST25VF016B
  //#define SPI_FLASH_SST25VF032B

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
    #define SPI_FLASH_SECTOR_LENGTH      (64 * 1024)                     // sector size of SPI FLASH (not available on A-versions)

    #define SPI_FLASH_PAGE_LENGTH        SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - smallest erasable block
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - file system granularity
#else                                                                    // example set up for AT45DB321C on ATMEL Eval board
  //#define SPI_FLASH_AT45DB011                                          // define the ATMEL type used here
  //#define SPI_FLASH_AT45DB021
  //#define SPI_FLASH_AT45DB041
  //#define SPI_FLASH_AT45DB081
  //#define SPI_FLASH_AT45DB161
  //#define SPI_FLASH_AT45DB321
    #define SPI_FLASH_AT45DB642                                          // note that this is used the EVK1100
    #define SPI_FLASH_AT45XXXXD_TYPE                                     // specify that a D-type rather than a B/C type is being used

    #if defined SPI_FLASH_AT45DB642                                      // define whether used in power of 2 mode or not
        #define SPI_FLASH_PAGE_LENGTH 1056                               // standard page size (B-device only allows 1056)
      //#define SPI_FLASH_PAGE_LENGTH 1024                               // size when power of 2 mode selected (only possible on D-device) - warning: this mode will be set and is not reversible!! (requires a power-cycle the first time after being set)
        #define SPI_FLASH_PAGES             (8 * 1024)                   // 8Meg part {9}
    #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
        #define SPI_FLASH_PAGE_LENGTH 528                                // standard page size (B/C-device only allows 528)
      //#define SPI_FLASH_PAGE_LENGTH 512                                // size when power of 2 mode selected (only possible on D-device)
        #if defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 2Meg part {9}
        #else
            #define SPI_FLASH_PAGES         (8 * 1024)                   // 4Meg part {9}
        #endif
    #else
        #define SPI_FLASH_PAGE_LENGTH 264                                // standard page size (B-device only allows 264)
      //#define SPI_FLASH_PAGE_LENGTH 256                                // size when power of 2 mode selected (only possible on D-device)
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

    #define SPI_FLASH_BLOCK_LENGTH  (8 * SPI_FLASH_PAGE_LENGTH)          // block size - a block can be deleted
    #define SPI_FLASH_SECTOR_LENGTH (64 * 4 * SPI_FLASH_PAGE_LENGTH)     // exception sector 0a is 2k and sector 0b is 62k
#endif

#define SPI_FLASH_DEVICE_COUNT  1                                        // only define CSX_LINE for this amount of devices
#define SPI_DATA_FLASH_0_SIZE   (SPI_FLASH_PAGES*SPI_FLASH_PAGE_LENGTH)
#define SPI_DATA_FLASH_1_SIZE   (SPI_FLASH_PAGES*SPI_FLASH_PAGE_LENGTH)
#define SPI_DATA_FLASH_2_SIZE   (SPI_FLASH_PAGES*SPI_FLASH_PAGE_LENGTH)
#define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE /* + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE*/)

#define SW_UPLOAD_FILE()       (*ucIp_Data == 'z') && (*(ucIp_Data+1) == 'S') && (*(ucIp_Data+2) == '.')  && (fnSPI_Flash_available() != 0)


#define SET_SPI_FLASH_MODE()                                             // dummy for compatibility
#define REMOVE_SPI_FLASH_MODE()                                          // dummy for compatibility


#ifdef SDCARD_SUPPORT                                                    // {8}
    #define SDCARD_SIM_SIZE   SDCARD_SIZE_2G                             // the size of SD card when simulating
  //#define _NO_SD_CARD_INSERTED                                         // simulate no SD card inserted initially
    #define T_CHECK_CARD_REMOVAL    ((DELAY_LIMIT)(SEC * 10))            // if the card has no detection switch it can be polled to detect removal
  //#define SDCARD_DETECT_INPUT_POLL                                     // use card detect switch for detection polling (use together with T_CHECK_CARD_REMOVAL)
  //#define SDCARD_DETECT_INPUT_INTERRUPT                                // use card detect switch for detection by interrupt (T_CHECK_CARD_REMOVAL and SDCARD_DETECT_INPUT_POLL should be disabled)
    #if defined SDCARD_DETECT_INPUT_INTERRUPT && !defined SUPPORT_PORT_INTERRUPTS
        #define SUPPORT_PORT_INTERRUPTS
    #endif
    #if defined AVR32_EVK1100
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_1                  PA18
        #define INITIALISE_SPI_SD_INTERFACE() \
            POWER_UP(PBA, PBAMASK_SPI1); \
            _CONFIG_PERIPHERAL_B(0, (SPI1_MISO_B | SPI1_MOSI_B | SPI1_SCK_B)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, SPI_CS1_1, SPI_CS1_1); \
            SPI_MR_1 = (SPI_MSTR | MODFDIS | SPI_CS_0); \
            SPI_CSR0_1 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 300000/2)/300000) << 8));
          //SPI_CR_1 = SPIEN

        #define ENABLE_SPI_SD_OPERATION() SPI_CR_1 = SPIEN               // workaround for GCC problem (it otherwise puts this command at the start of the INITIALISE_SPI_SD_INTERFACE() sequence)
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED()  SPI_CSR0_1 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 6000000/2)/6000000) << 8));

        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI_TDR_1 = (unsigned char)byte; SPI_RDR_1 = _fnSimSD_write((unsigned char)SPI_TDR_1)
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_1 & SPI_RDRF)) {SPI_SR_1 |= SPI_RDRF;};
        #else
            #define WRITE_SPI_CMD(byte)    SPI_TDR_1 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_1 & SPI_RDRF)) {};
        #endif
        #define READ_SPI_DATA()        (unsigned char)SPI_RDR_1

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()  _SETBITS(0, SPI_CS1_1)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(0, SPI_CS1_1)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(0, SPI_CS1_1)              // negate the CS line of the SD card to be read
        #define GET_SDCARD_WP_STATE() 0                                  // {12} write protection disabled (change to read switch if required)
    #elif defined AVR32_EVK1101
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PA17
        #define SPI_CARD_DETECT            PB00
        #define SPI_WRITE_PROTECT_INPUT    PB01
        #define INITIALISE_SPI_SD_INTERFACE() \
            POWER_UP(PBA, PBAMASK_SPI0); \
            _CONFIG_PORT_INPUT(1, (SPI_WRITE_PROTECT_INPUT | SPI_CARD_DETECT)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, SPI_CS1_0, SPI_CS1_0); \
            _CONFIG_PERIPHERAL_A(0, (SPI0_MISO_A | SPI0_MOSI_A | SPI0_SCK_A)); \
            SPI_MR_0 = (SPI_MSTR | MODFDIS | SPI_CS_0); \
            SPI_CSR0_0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 300000/2)/300000) << 8));
          //SPI_CR_0 = SPIEN

        #define ENABLE_SPI_SD_OPERATION() SPI_CR_0 = SPIEN               // workaround for GCC problem (it otherwise puts this command at the start of the INITIALISE_SPI_SD_INTERFACE() sequence)
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED()  SPI_CSR0_0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 6000000/2)/6000000) << 8));

        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI_TDR_0 = (unsigned char)byte; SPI_RDR_0 = _fnSimSD_write((unsigned char)SPI_TDR_0)
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_0 & SPI_RDRF)) {SPI_SR_0 |= SPI_RDRF;};
        #else
            #define WRITE_SPI_CMD(byte)    SPI_TDR_0 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_0 & SPI_RDRF)) {};
        #endif
        #define READ_SPI_DATA()        (unsigned char)SPI_RDR_0

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()  _SETBITS(0, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(0, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(0, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define GET_SDCARD_WP_STATE() (!(_READ_PORT_MASK(1, SPI_WRITE_PROTECT_INPUT))) // write protection input
        #define SDCARD_DETECTION()   _READ_PORT_MASK(1, SPI_CARD_DETECT) // card detection input

        #define SDCARD_DETECT_PORT   PORT_1                              // card detection input defines when using interrupts
        #define SDCARD_DETECT_PIN    SPI_CARD_DETECT
    #elif defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED     // {13}
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PB22
        #define INITIALISE_SPI_SD_INTERFACE() \
            POWER_UP(PBA, PBAMASK_SPI1); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(1, SPI_CS1_0, SPI_CS1_0); \
            _CONFIG_PERIPHERAL_B(1, (SPI1_MISO_1_B | SPI1_MOSI_1_B | SPI1_SCK_1_B)); \
            SPI_MR_1 = (SPI_MSTR | MODFDIS | SPI_CS_0); \
            SPI_CSR0_1 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 300000/2)/300000) << 8))

        #define ENABLE_SPI_SD_OPERATION() SPI_CR_1 = SPIEN               // workaround for GCC problem (it otherwise puts this command at the start of the INITIALISE_SPI_SD_INTERFACE() sequence)
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED()  SPI_CSR0_1 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 6000000/2)/6000000) << 8));

        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI_TDR_1 = (unsigned char)byte; SPI_RDR_1 = _fnSimSD_write((unsigned char)SPI_TDR_1)
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_1 & SPI_RDRF)) {SPI_SR_1 |= SPI_RDRF;}
        #else
            #define WRITE_SPI_CMD(byte)    SPI_TDR_1 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_1 & SPI_RDRF)) {}
        #endif
        #define READ_SPI_DATA()        (unsigned char)SPI_RDR_1

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()  _SETBITS(1, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(1, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(1, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define GET_SDCARD_WP_STATE() 0                                  // write protection disabled (change to read switch if required)
    #else                                                                // EVK1105
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PA08
        #define SPI_CARD_DETECT            PA02
        #define SPI_WRITE_PROTECT_INPUT    PA18
        #define INITIALISE_SPI_SD_INTERFACE() \
            POWER_UP(PBA, PBAMASK_SPI0); \
            _CONFIG_PORT_INPUT(0, (SPI_WRITE_PROTECT_INPUT | SPI_CARD_DETECT)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, SPI_CS1_0, SPI_CS1_0); \
            _CONFIG_PERIPHERAL_A(0, (SPI0_MISO_A | SPI0_MOSI_A | SPI0_SCK_A)); \
            SPI_MR_0 = (SPI_MSTR | MODFDIS | SPI_CS_0); \
            SPI_CSR0_0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 300000/2)/300000) << 8));
          //SPI_CR_1 = SPIEN

        #define ENABLE_SPI_SD_OPERATION() SPI_CR_0 = SPIEN               // workaround for GCC problem (it otherwise puts this command at the start of the INITIALISE_SPI_SD_INTERFACE() sequence)
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED()  SPI_CSR0_0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 6000000/2)/6000000) << 8));

        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI_TDR_0 = (unsigned char)byte; SPI_RDR_0 = _fnSimSD_write((unsigned char)SPI_TDR_0)
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_0 & SPI_RDRF)) {SPI_SR_0 |= SPI_RDRF;};
        #else
            #define WRITE_SPI_CMD(byte)    SPI_TDR_0 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (!(SPI_SR_0 & SPI_RDRF)) {};
        #endif
        #define READ_SPI_DATA()        (unsigned char)SPI_RDR_0

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()  _SETBITS(0, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(0, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(0, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define GET_SDCARD_WP_STATE() (!(_READ_PORT_MASK(0, SPI_WRITE_PROTECT_INPUT))) // write protection input

        #define SDCARD_DETECTION()   _READ_PORT_MASK(0, SPI_CARD_DETECT) // card detection input

        #define SDCARD_DETECT_PORT   PORT_0                              // card detection input defines when using interrupts
        #define SDCARD_DETECT_PIN    SPI_CARD_DETECT
    #endif
#endif


#define TICK_INTERRUPT()                                                 // {6} dummy user tick handler

#ifdef MONITOR_PERFORMANCE                                               // {15} RTC is used to monitor task durations (clocked from 115k RC oscillator without pre-scale)
    #define INITIALISE_MONITOR_TIMER() POWER_UP(PBA, PBAMASK_RM_RTC_EIC); RTC_CTRL = (RTC_CTRL_CLKEN); while (RTC_CTRL & RTC_CTRL_BUSY) {} RTC_TOP = 0xffffffff; while (RTC_CTRL & RTC_CTRL_BUSY) {} RTC_CTRL = (RTC_CTRL_CLKEN | RTC_CTRL_EN);
    #define EXECUTION_DURATION()    RTC_VAL; RTC_VAL = 0                 // read the elapsed count value and reset the counter back to 0 (the busy bit is not checked since it would only be busy when trying to reset very quickly where the value would be 0 anyway)
#endif

//#define SUPPORT_RTC                                                    // enable RTC support

// Parameter system
//
#ifdef USE_PARAMETER_BLOCK
    #define PARAMETER_BLOCK_SIZE   (8 * FLASH_GRANULARITY)               // use 4K parameter block (made up of 8 pages)
    #ifdef USE_PAR_SWAP_BLOCK                                            // swap block uses two blocks to ensure updates with no possible data los
        #define PAR_BLOCK_SIZE     (2 * PARAMETER_BLOCK_SIZE)
    #else
        #define PAR_BLOCK_SIZE     (1 * PARAMETER_BLOCK_SIZE)
    #endif
#else
    #define PAR_BLOCK_SIZE  (0)
#endif

// FLASH based File System setup
//
#ifdef FLASH_FILE_SYSTEM
    #ifdef SPI_FILE_SYSTEM
        #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + (SIZE_OF_FLASH - PAR_BLOCK_SIZE)) // FLASH location just short of end of internal FLASH
        #define uFILE_START (FLASH_START_ADDRESS + SIZE_OF_FLASH)        // immediately after internal FLASH

        #if SPI_DATA_FLASH_SIZE < (64 * FILE_GRANULARITY)                // allow testing smallest SPI FLASH
            #define FILE_SYSTEM_SIZE  SPI_DATA_FLASH_SIZE
        #else
            #define FILE_SYSTEM_SIZE (128 * FILE_GRANULARITY)            // 2 Meg reserved for file system
        #endif
        #define SPI_FLASH_START    (FLASH_START_ADDRESS + SIZE_OF_FLASH) // SPI FLASH starts immediately after FLASH
    #else
        #if !defined _AT32UC3B && !defined _AT32UC3C
            #define FLASH_ERRATA_WORKAROUND                              // AT32UC3A parts before revision J require this workaround to avoid potential Flash write problems
        #endif
        #define _FILE_SYSTEM_START_OFFSET  (160 * 1024)
        #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + _FILE_SYSTEM_START_OFFSET) // FLASH location at 160k start
        #define uFILE_START (PARAMETER_BLOCK_START + PAR_BLOCK_SIZE)     // FLASH location at 168k start (swap block assumed)
        #define FILE_GRANULARITY (8 * FLASH_GRANULARITY)                 // each file a multiple of 4k
        #ifdef FLASH_ERRATA_WORKAROUND
            #define FILE_SYSTEM_SIZE ((SIZE_OF_FLASH - FILE_GRANULARITY) - (_FILE_SYSTEM_START_OFFSET + PAR_BLOCK_SIZE)) // 340k for 512k part
          //#define FILE_SYSTEM_SIZE (92 * FILE_GRANULARITY)             // 370k reserved for file system
        #else
            #define FILE_SYSTEM_SIZE (SIZE_OF_FLASH - (_FILE_SYSTEM_START_OFFSET + PAR_BLOCK_SIZE)) // 344k for 512k part
          //#define FILE_SYSTEM_SIZE (93 * FILE_GRANULARITY)             // 374k reserved for file system
        #endif
        #define FLASH_LOW_BLOCK_ZERO  (PARAMETER_BLOCK_START - FLASH_GRANULARITY) // for FLASH workaround - one FLASH page in the first half of the FLASH
        #define FLASH_HIGH_BLOCK_ZERO ((FLASH_START_ADDRESS + SIZE_OF_FLASH) - FLASH_GRANULARITY)
    #endif
#else
    #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + 0x3fc00)
#endif


// USB interface
//
#define USB_DOWNLOAD_FILE              "z"

#define ENABLE_DPLUS_PULLUP()
#define ENABLE_DMINUS_PULLDOWN()


// Serial interfaces
//
#ifdef SERIAL_INTERFACE
    #define WELCOME_MESSAGE_UART   "\r\n\nHello, world... AVR32\r\n"
    #define NUMBER_EXTERNAL_SERIAL 0                                     // {5}
    #ifdef _AT32UC3B                                                     // {7}
        #ifdef CHIP_48_PIN
            #define NUMBER_SERIAL  2                                     // maximum 2 USARTs
        #else
            #define NUMBER_SERIAL  3                                     // maximum 3 USARTs
        #endif
    #elif defined AVR32_AT32UC3C_EK
        #define NUMBER_SERIAL      5                                     // maximum 5 USARTs
    #else
        #define NUMBER_SERIAL      4                                     // maximum 4 USARTs
    #endif
    #define UARTS_AVAILABLE        NUMBER_SERIAL
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255
    #define SERIAL_PORT_0          7                                     // if we open UART channel 0 we simulate using this COM port on the PC
    #define SERIAL_PORT_1          9                                     // if we open UART channel 1 we simulate using this COM port on the PC
    #define SERIAL_PORT_2          9                                     // if we open UART channel 2 we simulate using this COM port on the PC
    #define SERIAL_PORT_3          9                                     // if we open UART channel 3 we simulate using this COM port on the PC
    #define SERIAL_PORT_4          9                                     // if we open UART channel 4 we simulate using this COM port on the PC

  //#define SERIAL_SUPPORT_DMA                                           // enable UART DMA support (available on this processor)
  //#define SUPPORT_HW_FLOW                                              // enable hardware flow control support

    #if defined AVR32_EVK1101
        #define DEMO_UART          1                                     // use UART - 1 connected to DSUB on EVK1101
    #elif defined AVR32_AT32UC3C_EK
        #define DEMO_UART          2                                     // use UART - 2 connected to USB on AT32UC3_EK
    #elif defined AVR32_UC3_C2_XPLAINED                                  // {16}
        #define DEMO_UART          3                                     // use UART - 3
    #else
        #define DEMO_UART          0                                     // use UART - 0 connected to USB on EVK1105
    #endif
    #define MODBUS_UART_0          0
    #define MODBUS_UART_1          2
    #define MODBUS_UART_2          3
    #define MODBUS_UART_3          0
    #define PPP_UART               1                                     // use UART 1 for PPP
    #define TX_BUFFER_SIZE         (4 * 1024)                            // the size of RS232 input and output buffers
    #define RX_BUFFER_SIZE         (64)

    #if defined _AT32UC3B
        #ifndef CHIP_48_PIN                                              // alternative USART pins only possible on 64 pin device
          //#define USART0_ON_1
          //#define USART2_ON_PINS_26_27
          //#define USART1_RTS_ON_1
          //#define USART1_CTS_ON_1
        #endif
    #elif defined _AT32UC3C
          //#define USART0_ON_3_1
          //#define USART0_ON_2
          //#define USART1_ON_3
          //#define USART2_ON_PC2_3
        #if defined AVR32_UC3_C2_XPLAINED                                // {16}
            #define USART3_ON_2
        #else
          //#define USART3_ON_2
        #endif
          //#define USART3_ON_3_1
          //#define USART4_ON_3
          //#define USART4_ON_2_1
    #else
        #ifdef CHIP_144_PIN                                              // alternative USART TX + RX pins - these are only possible with 144 pin parts
          //#define USART0_ON_3                                          // else on 1
          //#define USART1_ON_3                                          // else on 1
          //#define USART2_ON_3                                          // else on 1
          //#define USART3_ON_2                                          // else on 1

          //#define USART0_RTS_ON_3                                      // alternate USART RT/CTS pins
          //#define USART1_RTS_ON_2
          //#define USART2_RTS_ON_3
          //#define USART3_RTS_ON_2
          //#define USART0_CTS_ON_3
          //#define USART1_CTS_ON_3
          //#define USART2_CTS_ON_3
          //#define USART3_CTS_ON_2
        #endif
    #endif
  //#define USART2_RTS_ON_1                                              // alternate USART RT/CTS pins available on all devices
  //#define USART2_CTS_ON_1
#else
    #define TX_BUFFER_SIZE       (256)
    #define RX_BUFFER_SIZE       (256)
#endif

//#define SUPPORT_TIMER                                                  // support hardware timer interrupt configuration
#if defined MODBUS_RTU && !defined SUPPORT_TIMER
    #define SUPPORT_TIMER                                                // MODBUS required HW timer in RTU mode

  //#define TIMER_A0_PX                                                  // otherwise on port B
  //#define TIMER_B0_PX                                                  // otherwise on port B
  //#define TIMER_A1_PX                                                  // otherwise on port B
  //#define TIMER_B1_PX                                                  // otherwise on port B
  //#define TIMER_A2_PX                                                  // otherwise on port B
  //#define TIMER_B2_PX                                                  // otherwise on port B
#endif
#if defined MODBUS_RTU
    #define SUPPORT_US_TIMER                                             // activate us mode
#endif

#define _TIMER_INTERRUPT_SETUP  TIMER_INTERRUPT_SETUP
#define MODBUS0_TIMER_CHANNEL    0
#define MODBUS1_TIMER_CHANNEL    1
#define MODBUS2_TIMER_CHANNEL    2

// I2C Interface
//
#if defined I2C_INTERFACE
    #define OUR_I2C_CHANNEL  0                                           // AVR32 has only one I2C (TWI) interface
    #define NUMBER_I2C       1                                           // one I2C available
#endif

//#define SUPPORT_EXTERNAL_INTERRUPTS                                    // support handling external interrupts
#define SUPPORT_PORT_INTERRUPTS                                          // support handling port interrupts
#if defined SUPPORT_PORT_INTERRUPTS
    #define SUPPORT_PORT_0_INTERRUPTS                                    // support just defined ports if required
    #if !defined _AT32UC3B || !defined CHIP_48_PIN
        #define SUPPORT_PORT_1_INTERRUPTS
    #endif
    #ifndef _AT32UC3B
        #define SUPPORT_PORT_2_INTERRUPTS
        #define SUPPORT_PORT_3_INTERRUPTS
    #endif
#endif

//#define SUPPORT_ADC                                                    // enable ADC support
#define ADC_AUTO_TRIGGERS                                                // support automatic triggering mode of operation
#define ADC_REFERENCE_VOLTAGE                      3300                  // ADC uses 3.3V reference
#define ADC_SIM_STEP_SIZE                          (200 * ADC_FULL_SCALE_10BIT) // 200mV steps when simulating

#if defined SUPPORT_ADC
    #define AN0_START_VOLTAGE                      (33   * ADC_FULL_SCALE_10BIT) // initial voltages when simulating (mV)
    #define AN1_START_VOLTAGE                      (500  * ADC_FULL_SCALE_10BIT)
    #define AN2_START_VOLTAGE                      (1000 * ADC_FULL_SCALE_10BIT)
    #define AN3_START_VOLTAGE                      (1500 * ADC_FULL_SCALE_10BIT)
    #define AN4_START_VOLTAGE                      (2000 * ADC_FULL_SCALE_10BIT)
    #define AN5_START_VOLTAGE                      (2500 * ADC_FULL_SCALE_10BIT)
    #define AN6_START_VOLTAGE                      (3000 * ADC_FULL_SCALE_10BIT)
    #define AN7_START_VOLTAGE                      (3300 * ADC_FULL_SCALE_10BIT)

    #define ADC_ERR_PRIORITY                       0
#endif


// LAN interface
//
#if defined ETH_INTERFACE
  //#define LAN_REPORT_ACTIVITY                                          // transmissions/receptions and errors are reported to chosen task (for link LED control, etc)

    #if defined LAN_REPORT_ACTIVITY
        #define LNKLED                 PB30
        #define ACTLED                 PB29
        #define SPDLED                 PB28
        #define DUPLED                 PB27
        #define COLLED                 PB24
        #define ACTIVITY_LED_OFF()     _CLEARBITS(1, ACTLED)
        #define ACTIVITY_LED_ON()      _SETBITS(1, ACTLED)
        #define TURN_LINK_LED_ON()     _SETBITS(1, LNKLED)
        #define SPEED_LED_ON()         _SETBITS(1, (SPDLED | LNKLED))
        #define SPEED_LED_OFF()        _CLEARBITS(1, (SPDLED | LNKLED))
        #define LINK_DOWN_LEDS()       _CLEARBITS(1, (LNKLED | ACTLED | COLLED | SPDLED | DUPLED))

        #define CONFIGURE_LAN_LEDS()   _CONFIG_PORT_OUTPUT(1, (LNKLED | ACTLED | COLLED | SPDLED | DUPLED)); LINK_DOWN_LEDS()
    #endif

    #define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE 1
    #define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE 4
#endif

#ifdef USE_IP
    #define LAN_BUFFER_SIZE           1536                               // AVR32 has ample space for full tx buffer
#else
    #define LAN_BUFFER_SIZE           256                                // if using Ethernet without IP the buffer size can be set here
#endif


#ifdef USE_BUFFERED_TCP                                                  // if using a buffer for TCP to allow interractive data applications (like TELNET)
    #define TCP_BUFFER            2800                                   // size of TCP buffer (with USE_BUFFERED_TCP) - generous with AVR32
    #define TCP_BUFFER_FRAME      1400                                   // allow this max. TCP frame size
#endif

#ifdef USE_HTTP
    #define HTTP_BUFFER_LENGTH    1400                                   // we send frames with this maximum amount of payload data - generous with AVR32
#endif

#ifdef USE_FTP                                                           // specify FTP support details
    #define FTP_WELCOME_RESPONSE         "220 Welcome AVR32 FTP.\r\n"
    //                                        ^^^^^^^^^^^^^^^^^^            customise the FTP welcome here
#endif


// Define interrupt priorities in the system (levels 0..3 - lowest to highest)
//
#define PRIORITY_USBB              INT_LEVEL_3                           // {10}
#define PRIORITY_DMA               INT_LEVEL_2
#define PRIORITY_UART0             INT_LEVEL_2
#define PRIORITY_UART1             INT_LEVEL_2
#define PRIORITY_UART2             INT_LEVEL_2
#define PRIORITY_UART3             INT_LEVEL_2
#define PRIORITY_UART4             INT_LEVEL_2
#define PRIORITY_MACB              INT_LEVEL_2
#define PRIORITY_GPIO              INT_LEVEL_2
#define PRIORITY_TIMERS            INT_LEVEL_1
#define PRIORITY_HW_TIMER          PRIORITY_TIMERS
#define PRIORITY_TWI               INT_LEVEL_1
#define PRIORITY_TICK_TIMER        INT_LEVEL_1



// Ports
//
#define MEASURE_LOW_POWER_ON()
#define MEASURE_LOW_POWER_OFF()

#define PORT0_DEFAULT_INPUT        0xffffffff                            // default state of inputs when simulating
#if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
    #define PORT1_DEFAULT_INPUT    0xffffffff
#else
    #define PORT1_DEFAULT_INPUT    0xff7fffff                            // pull PB23 down so that the watchdog is enabled
#endif
#define PORT2_DEFAULT_INPUT        0xffffffff
#define PORT3_DEFAULT_INPUT        0xffffffff


#define CONFIG_TEST_OUTPUT()                                             // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
#define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(1, TEST_OUTPUT_LED)


#define WATCHDOG_TIMEOUT           (17 << WATCHDOG_PRE_SHIFT)            // watchdog fires after 2.2s without retrigger (2 to the power of (17 + 1) / 115000 [slow clock])


// User port mapping
//
#if defined AVR32_EVK1101                                                // {7}
    #define USER_PORT_1_BIT        PA09                                  // use free PA pins on Eval board
    #define USER_PORT_2_BIT        PA10
    #define USER_PORT_3_BIT        PA11
    #define USER_PORT_4_BIT        PA12
    #define USER_PORT_5_BIT        PA13
    #define USER_PORT_6_BIT        PA14
    #define USER_PORT_7_BIT        PA15
    #define USER_PORT_8_BIT        0 //PA16
    #define USER_PORT_9_BIT        PA17
    #define USER_PORT_10_BIT       PA25
    #define USER_PORT_11_BIT       PA26
    #define USER_PORT_12_BIT       PA27
    #define USER_PORT_13_BIT       PA28
    #define USER_PORT_14_BIT       PA29
    #define USER_PORT_15_BIT       PA30
    #define USER_PORT_16_BIT       PA31
#elif defined AVR32_AT32UC3C_EK                                          // {13}
    #define USER_PORT_1_BIT        PA12                                  // use free PA pins on Eval board
    #define USER_PORT_2_BIT        PA13
    #define USER_PORT_3_BIT        PA13                                  // keep PA14 and PA29 free
    #define USER_PORT_4_BIT        PA15
    #define USER_PORT_5_BIT        PA16
    #define USER_PORT_6_BIT        PA19
    #define USER_PORT_7_BIT        PA20
    #define USER_PORT_8_BIT        PA21
    #define USER_PORT_9_BIT        PA22
    #define USER_PORT_10_BIT       PA23
    #define USER_PORT_11_BIT       PA24
    #define USER_PORT_12_BIT       PA25
    #define USER_PORT_13_BIT       PA26
    #define USER_PORT_14_BIT       PA27
    #define USER_PORT_15_BIT       PA28
    #define USER_PORT_16_BIT       PA28
#elif defined AVR32_UC3_C2_XPLAINED                                      // {16}
    #define USER_PORT_1_BIT        0
    #define USER_PORT_2_BIT        0
    #define USER_PORT_3_BIT        0
    #define USER_PORT_4_BIT        0
    #define USER_PORT_5_BIT        0
    #define USER_PORT_6_BIT        0
    #define USER_PORT_7_BIT        0
    #define USER_PORT_8_BIT        0
    #define USER_PORT_9_BIT        0
    #define USER_PORT_10_BIT       0
    #define USER_PORT_11_BIT       0
    #define USER_PORT_12_BIT       0
    #define USER_PORT_13_BIT       0
    #define USER_PORT_14_BIT       0
    #define USER_PORT_15_BIT       0
    #define USER_PORT_16_BIT       0
#else
    #define USER_PORT_1_BIT        PA05                                  // use free PA pins on Eval board
    #define USER_PORT_2_BIT        PA06
    #define USER_PORT_3_BIT        PA02
    #define USER_PORT_4_BIT        PA03
    #define USER_PORT_5_BIT        PA04
    #define USER_PORT_6_BIT        PA07
    #define USER_PORT_7_BIT        PA08
    #define USER_PORT_8_BIT        PA09
    #define USER_PORT_9_BIT        0 //PA10
    #define USER_PORT_10_BIT       PA11
    #define USER_PORT_11_BIT       PA12
    #define USER_PORT_12_BIT       PA13
    #define USER_PORT_13_BIT       PA14
    #define USER_PORT_14_BIT       PA15
    #define USER_PORT_15_BIT       PA16
    #define USER_PORT_16_BIT       PA17
#endif
// Port use definitions
//
#define USER_SET_PORT_1            OVRS_0
#define USER_CLEAR_PORT_1          OVRC_0
#define USER_PORT_1                OVR_0
#define USER_DDR_1                 ODERS_0
#define USER_SET_PORT_2            OVRS_0
#define USER_CLEAR_PORT_2          OVRC_0
#define USER_PORT_2                OVR_0
#define USER_DDR_2                 ODERS_0
#define USER_SET_PORT_3            OVRS_0
#define USER_CLEAR_PORT_3          OVRC_0
#define USER_PORT_3                OVR_0
#define USER_DDR_3                 ODERS_0
#define USER_SET_PORT_4            OVRS_0
#define USER_CLEAR_PORT_4          OVRC_0
#define USER_PORT_4                OVR_0
#define USER_DDR_4                 ODERS_0
#define USER_SET_PORT_5            OVRS_0
#define USER_CLEAR_PORT_5          OVRC_0
#define USER_PORT_5                OVR_0
#define USER_DDR_5                 ODERS_0
#define USER_SET_PORT_6            OVRS_0
#define USER_CLEAR_PORT_6          OVRC_0
#define USER_PORT_6                OVR_0
#define USER_DDR_6                 ODERS_0
#define USER_SET_PORT_7            OVRS_0
#define USER_CLEAR_PORT_7          OVRC_0
#define USER_PORT_7                OVR_0
#define USER_DDR_7                 ODERS_0
#define USER_SET_PORT_8            OVRS_0
#define USER_CLEAR_PORT_8          OVRC_0
#define USER_PORT_8                OVR_0
#define USER_DDR_8                 ODERS_0
#define USER_SET_PORT_9            OVRS_0
#define USER_CLEAR_PORT_9          OVRC_0
#define USER_PORT_9                OVR_0
#define USER_DDR_9                 ODERS_0
#define USER_SET_PORT_10           OVRS_0
#define USER_CLEAR_PORT_10         OVRC_0
#define USER_PORT_10               OVR_0
#define USER_DDR_10                ODERS_0
#define USER_SET_PORT_11           OVRS_0
#define USER_CLEAR_PORT_11         OVRC_0
#define USER_PORT_11               OVR_0
#define USER_DDR_11                ODERS_0
#define USER_SET_PORT_12           OVRS_0
#define USER_CLEAR_PORT_12         OVRC_0
#define USER_PORT_12               OVR_0
#define USER_DDR_12                ODERS_0
#define USER_SET_PORT_13           OVRS_0
#define USER_CLEAR_PORT_13         OVRC_0
#define USER_PORT_13               OVR_0
#define USER_DDR_13                ODERS_0
#define USER_SET_PORT_14           OVRS_0
#define USER_CLEAR_PORT_14         OVRC_0
#define USER_PORT_14               OVR_0
#define USER_DDR_14                ODERS_0
#define USER_SET_PORT_15           OVRS_0
#define USER_CLEAR_PORT_15         OVRC_0
#define USER_PORT_15               OVR_0
#define USER_DDR_15                ODERS_0
#define USER_SET_PORT_16           OVRS_0
#define USER_CLEAR_PORT_16         OVRC_0
#define USER_PORT_16               OVR_0
#define USER_DDR_16                ODERS_0



// Evaluation board settings for ports and PHY interface
//
#if defined AVR32_EVK1100                                                // {1}
    #define SHIFT_PORT_TO_BYTE      27
    #define FIRST_USER_PORT         PB27
    #define DEMO_LED_1              (PB27 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_2              (PB28 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_3              (PB29 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_4              (PB30 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_USER_PORTS         (PB27 | PB28 | PB29 | PB30)
    #define TEST_OUTPUT_LED         PB28
    #define BLINK_LED               PB27

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(1, BLINK_LED);
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(1, BLINK_LED)        // blink the LED, if set as output
    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(1, PB23)       // ensure clocks to port
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(1, PB23))        // pull this input up to disable watchdog - hold DOWN KEY on EVK1105

    #define PHY_RMII                                                     // PHY MII interface is connected as reduced MII

    #define _PHY_DP83848
    #define PHY_ADDRESS_  0x01                                           // address of PHY on EVK1105 (RMII mode)
    #define VNDR_MDL      0x09                                           // Vendor model number
    #define MDL_REV       0x00                                           // Model revision number
    #define PHY_IDENTIFIER          (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
    #define PHY_MASK                0xfffffff0
    #define PHY_INTERRUPT           PA24
    #define PORT_PHY_INT            PORT_0
    #define PHY_INPUT_LOW()         (!(PVR_0 & PHY_INTERRUPT))
    #define PHY_POWERUP_DELAY      (0.150 * SEC)                         // plus 50ms task delay to give > 167ms - the time required for the PHY to stabilise after a power up

    // SMTP settings
    //
    #define SENDERS_EMAIL_ADDRESS             "EVK1105@uTasker.com"      // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "EVK1105 Test"             // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the AVR32 AT32UC3A on the EVK1105.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your EVK1105!!";
#elif defined AVR32_EVK1101                                              // {7}
    #define SWITCH_PB0              PB02
    #define SWITCH_PB1              PB03

    #define JOY_STICK_CTR           PA13
    #define JOY_STICK_A             PB06
    #define JOY_STICK_B             PB07
    #define JOY_STICK_C             PB08
    #define JOY_STICK_D             PB09
    #define SHIFT_PORT_TO_BYTE      7
    #define FIRST_USER_PORT         PA07
    #define DEMO_LED_1              (PA07 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_2              (PA08 >> SHIFT_PORT_TO_BYTE)
//  #define DEMO_LED_3              (PB29 >> SHIFT_PORT_TO_BYTE)
//  #define DEMO_LED_4              (PB30 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_USER_PORTS         (PA07 | PA08)
    #define TEST_OUTPUT_LED         PA08
    #define BLINK_LED               PA07

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(0, BLINK_LED);
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(0, BLINK_LED)        // blink the LED, if set as output
    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(1, PB02)       // ensure clocks to port and configure as input
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(1, PB02))        // pull this input up to disable watchdog - hold PB0 on EVK1101
#elif defined AVR32_EVK1105
    #define TOUCH_DETECT            PA22
    #define TOUCH_LEFT              PB25
    #define TOUCH_RIGHT             PB24
    #define TOUCH_UP                PB22
    #define TOUCH_DOWN              PB23
    #define TOUCH_ENTER             PB26

    #define SHIFT_PORT_TO_BYTE      27
    #define FIRST_USER_PORT         PB27
    #define DEMO_LED_1              (PB27 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_2              (PB28 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_3              (PB29 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_LED_4              (PB30 >> SHIFT_PORT_TO_BYTE)
    #define DEMO_USER_PORTS         (PB27 | PB28 | PB29 | PB30)
    #define TEST_OUTPUT_LED         PB28
    #define BLINK_LED               PB27

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(1, BLINK_LED);
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(1, BLINK_LED)        // blink the LED, if set as output
    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(1, PB23)       // ensure clocks to port
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(1, PB23))        // pull this input up to disable watchdog - hold DOWN KEY on EVK1105

    #define PHY_RMII                                                     // PHY MII interface is connected as reduced MII

    #define _PHY_DP83848
    #define PHY_ADDRESS_  0x01                                           // address of PHY on EVK1105 (RMII mode)
    #define VNDR_MDL      0x09                                           // vendor model number
    #define MDL_REV       0x00                                           // model revision number
    #define PHY_IDENTIFIER          (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
    #define PHY_MASK                0xfffffff0
    #define PHY_INTERRUPT           PA26
    #define PORT_PHY_INT            PORT_0
    #define PHY_INPUT_LOW()         (!(PVR_0 & PHY_INTERRUPT))
    #define PHY_POWERUP_DELAY       (0.150 * SEC)                        // plus 50ms task delay to give > 167ms - the time required for the PHY to stabilise after a power up

    // SMTP settings
    //
    #define SENDERS_EMAIL_ADDRESS             "EVK1105@uTasker.com"      // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "EVK1105 Test"             // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the AVR32 AT32UC3A on the EVK1105.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your EVK1105!!";
#elif defined AVR32_AT32UC3C_EK                                          // {13}
    #define SWITCH_PB0              (PA14)
    #define SWITCH_PB1              (PA29)
    #define DEMO_LED_1              (PA08)
    #define DEMO_LED_2              (PC13)
    #define DEMO_LED_3              (PD22)
    #define DEMO_LED_4              (PD23)
    #define TEST_OUTPUT_LED         DEMO_LED_4
    #define BLINK_LED               DEMO_LED_1

    #define SHIFT_DEMO_LED_1        8                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        13
    #define SHIFT_DEMO_LED_3        22
    #define SHIFT_DEMO_LED_4        23

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(0, BLINK_LED);
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(0, BLINK_LED)        // blink the LED, if set as output
    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(0, SWITCH_PB0) // ensure clocks to port
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(0, SWITCH_PB0))  // pull this input up to disable watchdog - hold PB0 on AT32C3C-EK

    #define PHY_RMII                                                     // PHY MII interface is connected as reduced MII

    #define _PHY_DP83848
    #define PHY_ADDRESS_  0x01                                           // address of PHY on EVK1105 (RMII mode)
    #define VNDR_MDL      0x09                                           // vendor model number
    #define MDL_REV       0x00                                           // model revision number
    #define PHY_IDENTIFIER          (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
    #define PHY_MASK                0xfffffff0
    #define PHY_INTERRUPT           PB31
    #define PORT_PHY_INT            PORT_1
    #define PHY_INPUT_LOW()         (!(PVR_1 & PHY_INTERRUPT))
    #define PHY_POWERUP_DELAY      (0.150 * SEC)                         // plus 50ms task delay to give > 167ms - the time required for the PHY to stabilise after a power up

    // SMTP settings
    //
    #define SENDERS_EMAIL_ADDRESS   "AT32UC3_EK@uTasker.com"             // fictional Email address of the board being used
    #define EMAIL_SUBJECT           "AT32UC3_EK Test"                    // email subject
    #define EMAIL_CONTENT           "Hello!!\r\nThis is an email message from the AVR32 AT32UC3C on the AC32UC3_EK.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your AC32UC3_EK!!";
#elif defined AVR32_UC3_C2_XPLAINED                                      // {16}
    #define ADCIN0                  (PA09)
    #define PHYRSTB                 ADCIN0                               // connected when J100 is not broken

    #define BUTTON0                 (PC15)
    #define BUTTON1                 (PA23)
    #define DEMO_LED_1              (PD21)
    #define DEMO_LED_2              (PC22)
    #define TEST_OUTPUT_LED         0
    #define BLINK_LED               DEMO_LED_1

    #define SHIFT_DEMO_LED_1        21                                    // since the port bits are spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2        21

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED() 
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_PORT_OUTPUT(3, BLINK_LED);
    #endif
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(3, BLINK_LED)           // blink the LED, if set as output
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(3, DEMO_LED_1)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(3, DEMO_LED_1))     // pull this input up to disable watchdog - hold PB0 on UC3-C2 X plained

    #define PHY_RMII                                                     // PHY MII interface is connected as reduced MII

    #define _PHY_RTL8201E
    #define PHY_ADDRESS_            0x01                                 // address of PHY on UC3-C2 X plained (RMII mode)
    #define VNDR_MDL                0x01                                 // vendor model number
    #define MDL_REV                 0x05                                 // model revision number
    #define PHY_IDENTIFIER          (0x001cc800 | (VNDR_MDL << 4) | MDL_REV) // realtek RTL8201E identifier
    #define PHY_MASK                0xfffffff0
    #define PHY_POLL_LINK                                                // since there is no interrupt line we poll the link state to ensure that the configuration is synchronised in the EMAC
    #define INTERRUPT_TASK_PHY      TASK_NETWORK_INDICATOR               // link status reported to this task
  //#define PHY_INTERRUPT           0                                    // no interrupt line connected
  //#define PORT_PHY_INT            PORT_1
  //#define PHY_INPUT_LOW()         (!(PVR_1 & PHY_INTERRUPT))
    #define PHY_POWERUP_DELAY       (0.250 * SEC)                        // plus 250ms task delay to give 300ms - to allow the PHY to become ready after reset

    #define RESET_PHY                                                    // enable reset control
    #define ASSERT_PHY_RST()        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, PHYRSTB, 0); fnDelayLoop(12000); _SETBITS(0, PHYRSTB);// assert PHY reset for 12ms
    #define CONFIG_PHY_STRAPS()                                          // the module configures straps itself
    #define DEASSERT_PHY_RST()                                           // release PHY from reset state (not used since it is integrated in the reset pulse on initialisation

    // SMTP settings
    //
    #define SENDERS_EMAIL_ADDRESS   "UC3-C2@uTasker.com"                 // fictional Email address of the board being used
    #define EMAIL_SUBJECT           "UC3-C2 X plained Test"              // email subject
    #define EMAIL_CONTENT           "Hello!!\r\nThis is an email message from the AVR32 AT32UC3C on the UC3-C2 X plained.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your UC3-C2 X plained!!";

#endif


// LCD interface: Backlight control PB4 : Data bus (4 Bit) PB313..PB10 : RS PB14, RW PB16, E PB17
//
typedef unsigned long LCD_BUS_PORT_SIZE;                                 // we use 32 bit ports on the AVR32
typedef unsigned long LCD_CONTROL_PORT_SIZE;
//#define LCD_BUS_8BIT                                                   // data bus in 8 bit mode
#define LCD_BUS_4BIT                                                     // data bus in 4 bit mode

#ifdef LCD_BUS_8BIT
    #define LCD_BUS_MASK      0x0003fc00
    #define DATA_SHIFT_RIGHT     0
    #define DATA_SHIFT_LEFT      10                                      // byte shift down required to bring data into position
#else
    #define LCD_BUS_MASK      0x00003c00
    #define DATA_SHIFT_RIGHT     0
    #define DATA_SHIFT_LEFT      6                                       // nibble shift down required to bring data into position
#endif

#define O_CONTROL_RS            PB14
#define O_WRITE_READ            PB16
#define O_CONTROL_EN            PB17
#define O_LCD_BACKLIGHT         PB04

#define O_CONTROL_LINES           (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
#define IO_BUS_PORT_DAT_IN        _READ_PORT(1)

#define SET_DATA_LINES_INPUT()    _FLOAT_PORT(1, LCD_BUS_MASK)
#define SET_DATA_LINES_OUTPUT()   _DRIVE_PORT_OUTPUT(1, LCD_BUS_MASK)
#define SET_BUS_DATA(x)           _WRITE_PORT(1, x)
#define SET_CONTROL_LINES(x)      _CLEARBITS(1, O_CONTROL_LINES); _SETBITS(1, x) // first clear mask and then set bits
#define O_SET_CONTROL_LOW(x)      _CLEARBITS(1, x)
#define O_SET_CONTROL_HIGH(x)     _SETBITS(1, x)

// Drive the control lines R/W + LCD Backlight '1', RS + E '0' and the data lines with all high impedance at start up
// enable clocks to port first
#define INITIALISE_LCD_CONTROL_LINES()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(1, (O_CONTROL_LINES | O_LCD_BACKLIGHT), (O_LCD_BACKLIGHT | O_WRITE_READ)); \
                                         _CONFIG_PORT_INPUT(1, LCD_BUS_MASK);

#define LCD_DRIVE_DATA()        _DRIVE_PORT_OUTPUT(1, LCD_BUS_MASK);_DRIVE_PORT_OUTPUT(1, LCD_BUS_MASK)
                                // ensure data bus outputs (delay) by repetitions according to processor speed

#define CLOCK_EN_HIGH()         _SETBITS(1, O_CONTROL_EN); _SETBITS(1, O_CONTROL_EN);
                                // clock EN to high state - repreat to slow down (delay)

#define DELAY_ENABLE_CLOCK_HIGH()


#if defined GLCD_COLOR || defined CGLCD_GLCD_MODE                        // {3}
    // CGLCD
    //
    #if defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED
        #define GLCD_RESET_LINE        PD16
        #define GLCD_BACKLIGHT         PD28
        #define GTE_INPUT              PD19
        #define CONFIGURE_GLCD()       _CONFIG_PORT_INPUT(3, PD19); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(3, (GLCD_RESET_LINE | GLCD_BACKLIGHT), 0);
        #define GLCD_RST_H()           _SETBITS(3, GLCD_RESET_LINE)
        #define ENABLE_BACKLIGHT()     _SETBITS(3, GLCD_BACKLIGHT)
    #else
        #define GLCD_RESET_LINE        PX22 
        #define GLCD_BACKLIGHT         PB18
        #define GLCD_SPI_CS            PX33
        #define GTE_INPUT              PX19
        #define CONFIGURE_GLCD()       _CONFIG_PORT_INPUT(2, PX19); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(1, GLCD_BACKLIGHT, 0); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(2, (GLCD_SPI_CS | GLCD_RESET_LINE), GLCD_SPI_CS);
        #define GLCD_RST_H()           _SETBITS(2, GLCD_RESET_LINE)
        #define ENABLE_BACKLIGHT()     _SETBITS(1, GLCD_BACKLIGHT)
    #endif
    #undef GLCD_X                                                        // overwrite the standard CGLCD size
    #undef GLCD_Y
      #define GLCD_X                   320                               // horizontal resolution of the LCD display in pixels
      #define GLCD_Y                   240                               // vertical resolution of the LCD display in pixels
    #undef BIG_PIXEL                                                     // large display so no need for double size pixels
    #define _HX8347                                                      // define the controller type being used
    #ifdef _WINDOWS
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   512                               // the maximum number of writes to the GLCD before the task yields
    #endif
#else
    // GLCD
    //
    #ifndef _GLCD_SAMSUNG
        #define GLCD_RST               PX36
        #define GLCD_C_D               PX35
        #define GLCD_WR                PX14
        #define GLCD_RD                PX13
        #define CONFIGURE_GLCD()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(3, (GLCD_RST | GLCD_WR | GLCD_RD | GLCD_C_D), (GLCD_WR | GLCD_RD | GLCD_C_D)); _CONFIG_PORT_INPUT(3, 0xff);

        #define GLCD_DATAASOUTPUT()    _DRIVE_PORT_OUTPUT(3, 0xff)
        #define GLCD_DATAASINPUT()     _FLOAT_PORT(3, 0xff)

        #define GLCD_DATAOUT(data)     _WRITE_PORT_MASK(3, data, 0xff)
        #define GLCD_DATAIN()          _READ_PORT(3)

        #define SET_PULL_DOWNS()       
        #define REMOVE_PULL_DOWNS()    

        #define GLCD_WR_H()            _SETBITS(3, GLCD_WR)
        #define GLCD_WR_L()            _CLEARBITS(3, GLCD_WR)

        #define GLCD_CD_H()            _SETBITS(3, GLCD_C_D)
        #define GLCD_CD_L()            _CLEARBITS(3, GLCD_C_D)

        #define GLCD_RD_H()             _SETBITS(3, GLCD_RD)
        #define GLCD_RD_L()            _CLEARBITS(3, GLCD_RD)

        #define GLCD_DELAY_WRITE()     //GLCD_WR_L()                     // no write delay since the data is stable for long enough at full speed
        #define GLCD_DELAY_READ()      GLCD_RD_L()                       // one extra operation to ensure set up time of read

        #define GLCD_RST_H()           _SETBITS(3, GLCD_RST) 
        #define GLCD_RST_L()           _CLEARBITS(3, GLCD_RST)

        #ifdef _WINDOWS
            #define MAX_GLCD_WRITE_BURST   1000                          // the maximum number of writes to the GLCD before the task yields
        #else
            #define MAX_GLCD_WRITE_BURST   20                            // the maximum number of writes to the GLCD before the task yields
        #endif
    #else                                                                // {4}
        #define GLCD_RST               PX36                              // reset
        #define GLCD_RS                PX35                              // GLCD Register Select
        #define GLCD_RW                PX14                              // GLCD Read/Write
        #define GLCD_ENA               PX13                              // GLCD Enable

        #define GLCD_CS0               PX12                              // LCD Controller 0 Chip Select - 2 controller chips for 128 x 64
        #define GLCD_CS1               PX11                              // LCD Controller 1 Chip Select

        #define SET_PULL_DOWNS()       
        #define REMOVE_PULL_DOWNS()    

        #define CONFIGURE_GLCD() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(3, (GLCD_RST | GLCD_RW | GLCD_ENA | GLCD_RS | GLCD_CS0 | GLCD_CS1), (GLCD_RW)); _CONFIG_PORT_INPUT(3, 0xff);

        #define GLCD_DATAASOUTPUT()    _DRIVE_PORT_OUTPUT(3, 0xff)
        #define GLCD_DATAASINPUT()     _FLOAT_PORT(3, 0xff)

        #define GLCD_DATAOUT(data)     _WRITE_PORT_MASK(3, data, 0xff)
        #define GLCD_DATAIN()          _READ_PORT(3)

        #define GLCD_RS_H()            _SETBITS(3, GLCD_RS)
        #define GLCD_RS_L()            _CLEARBITS(3, GLCD_RS)

        #define GLCD_RW_H()            _SETBITS(3, GLCD_RW)
        #define GLCD_RW_L()            _CLEARBITS(3, GLCD_RW)

        #define GLCD_CS0_H()           _SETBITS(3, GLCD_CS0)  
        #define GLCD_CS0_L()           _CLEARBITS(3, GLCD_CS0)
    
        #define GLCD_CS1_H()           _SETBITS(3, GLCD_CS1)  
        #define GLCD_CS1_L()           _CLEARBITS(3, GLCD_CS1)
    
        #define GLCD_DELAY_WRITE()                                       // no write delay since the data is stable for long enough at full speed
        #define GLCD_DELAY_READ()      GLCD_RW_H()                       // one extra operation to ensure set up time of read

        #define GLCD_RST_H()           _SETBITS(3, GLCD_RST)  
        #define GLCD_RST_L()           _CLEARBITS(3, GLCD_RST)

        #define GLCD_ENA_H()           _SETBITS(3, GLCD_ENA)  
        #define GLCD_ENA_L()           _CLEARBITS(3, GLCD_ENA)

        #ifdef _WINDOWS
            #define MAX_GLCD_WRITE_BURST   1000                          // the maximum number of writes to the GLCD before the task yields
        #else
            #define MAX_GLCD_WRITE_BURST   80                            // the maximum number of writes to the GLCD before the task yields
        #endif
    #endif
#endif

// Keypad
//
#if defined KEY_COLUMNS && KEY_COLUMNS > 0                               // matrix keypad {14}
    #define KEY_ROW_IN_1              PX5
    #define KEY_ROW_IN_PORT_1         _READ_PORT(2)
    #define KEY_ROW_IN_PORT_1_REF     _GPIO_2

    #define KEY_ROW_IN_2              PX6
    #define KEY_ROW_IN_PORT_2         _READ_PORT(2)
    #define KEY_ROW_IN_PORT_2_REF     _GPIO_2

    #define KEY_ROW_IN_3              PX7
    #define KEY_ROW_IN_PORT_3         _READ_PORT(2)
    #define KEY_ROW_IN_PORT_3_REF     _GPIO_2

    #define KEY_ROW_IN_4              PX8
    #define KEY_ROW_IN_PORT_4         _READ_PORT(2)
    #define KEY_ROW_IN_PORT_4_REF     _GPIO_2

    #define KEY_COL_OUT_1             PX11
    #define KEY_COL_OUT_PORT_1        OVR_3
    #define KEY_COL_OUT_DDR_1         ODER_3

    #define KEY_COL_OUT_2             PX12
    #define KEY_COL_OUT_PORT_2        OVR_3
    #define KEY_COL_OUT_DDR_2         ODER_3

    #define KEY_COL_OUT_3             PX13
    #define KEY_COL_OUT_PORT_3        OVR_3
    #define KEY_COL_OUT_DDR_3         ODER_3

    #define KEY_COL_OUT_4             PX14
    #define KEY_COL_OUT_PORT_4        OVR_3
    #define KEY_COL_OUT_DDR_4         ODER_3

    // Drive each column low
    //
    #define DRIVE_COLUMN_1()          _DRIVE_PORT_OUTPUT(3, KEY_COL_OUT_1)    // drive output low (column 1) - this will drive a '0' since the output has been prepared
    #define DRIVE_COLUMN_2()          _DRIVE_PORT_OUTPUT(3, KEY_COL_OUT_2)    // drive output low (column 2) - this will drive a '0' since the output has been prepared
    #define DRIVE_COLUMN_3()          _DRIVE_PORT_OUTPUT(3, KEY_COL_OUT_3)    // drive output low (column 3) - this will drive a '0' since the output has been prepared
    #define DRIVE_COLUMN_4()          _DRIVE_PORT_OUTPUT(3, KEY_COL_OUT_4)    // drive output low (column 4) - this will drive a '0' since the output has been prepared

    // Drive high (to avoid slow rise time) then set back as input
    //
    #define RELEASE_COLUMN_1()        _SETBITS(3, KEY_COL_OUT_1); _FLOAT_PORT(3, KEY_COL_OUT_1)
    #define RELEASE_COLUMN_2()        _SETBITS(3, KEY_COL_OUT_2); _FLOAT_PORT(3, KEY_COL_OUT_2)
    #define RELEASE_COLUMN_3()        _SETBITS(3, KEY_COL_OUT_3); _FLOAT_PORT(3, KEY_COL_OUT_3)
    #define RELEASE_COLUMN_4()        _SETBITS(3, KEY_COL_OUT_4); _FLOAT_PORT(3, KEY_COL_OUT_4)

    // Reset any changes ready for next scan sequence
    //
    #define RESET_SCAN()              _CLEARBITS(3, (KEY_COL_OUT_1 | KEY_COL_OUT_2 | KEY_COL_OUT_3 | KEY_COL_OUT_4)); // prepare outputs low for next time

    // Key scan initialisation
    //
    #define INIT_KEY_SCAN()          _CONFIG_PORT_INPUT(3, (KEY_COL_OUT_1 | KEY_COL_OUT_2 | KEY_COL_OUT_3 | KEY_COL_OUT_4)); \
                                     _CONFIG_PORT_INPUT(2, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4)); \
                                      RESET_SCAN()

    // LEDs
    //
    #define KEYPAD_LEDS  4

                                    // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  {RGB(215,215,215),RGB(255,0,0), 1, {265, 20,  0,   7 },   _GPIO_0, PA05}, \
                                    {RGB(215,215,215),RGB(0,255,0), 1, {265, 88,  0,   7 },   _GPIO_0, PA06}, \
                                    {RGB(215,215,215),RGB(0,0,255), 1, {265, 156, 0,   7 },   _GPIO_1, PA28}, \
                                    {RGB(215,215,215),RGB(255,0,0), 1, {260, 216, 270, 226 }, _GPIO_1, PA27}
#elif defined KEY_COLUMNS && KEY_COLUMNS == 0                            // linear keyboard (key connected directly to inputs) {14}
    #define KEY_ROW_IN_1            PX5
    #define KEY_1_PORT_REF          _GPIO_2

    #define KEY_ROW_IN_2            PX6
    #define KEY_2_PORT_REF          _GPIO_2

    #define KEY_ROW_IN_3            PX7
    #define KEY_3_PORT_REF          _GPIO_2

    #define KEY_ROW_IN_4            PX8
    #define KEY_4_PORT_REF          _GPIO_2

    #define INIT_KEY_STATE          ((KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4) >> 28)

    #define KEY_1_PORT_PIN          KEY_ROW_IN_1
    #define KEY_2_PORT_PIN          KEY_ROW_IN_2
    #define KEY_3_PORT_PIN          KEY_ROW_IN_3
    #define KEY_4_PORT_PIN          KEY_ROW_IN_4

    #define READ_KEY_INPUTS()       (_READ_PORT_MASK(2, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4)) >> 28)

    #define INIT_KEY_SCAN()         _CONFIG_PORT_INPUT(2, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4))

    // LEDs
    //
    #define KEYPAD_LEDS  4

                                    // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  {RGB(215,215,215),RGB(255,0,0), 1, {265, 20,  0,   7 },   _GPIO_0, PA05}, \
                                    {RGB(215,215,215),RGB(0,255,0), 1, {265, 88,  0,   7 },   _GPIO_0, PA06}, \
                                    {RGB(215,215,215),RGB(0,0,255), 1, {265, 156, 0,   7 },   _GPIO_1, PA28}, \
                                    {RGB(215,215,215),RGB(255,0,0), 1, {260, 216, 270, 226 }, _GPIO_1, PA27}
#else                                                                    // no keys but use LEDs when a frontpanel is defined
    // LEDs
    //
    #if defined AVR32_EVK1105
        #define KEYPAD_LEDS  4
        #define KEYPAD "KeyPads/EVK1105.bmp"

                                        // '0'            '1'    input state  center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(190,190,190),RGB(50,255,0), 1, {52,  296, 62,  304 }, _GPIO_1, PA27}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {87,  296, 97,  304 }, _GPIO_1, PA28}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {141, 296, 151, 304 }, _GPIO_0, PA06}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {188, 296, 198, 304 }, _GPIO_0, PA05}

        #define BUTTON_KEY_DEFINITIONS  {PORT_0, TOUCH_DETECT, {283, 259, 431, 407}}, \
                                        {PORT_1, TOUCH_LEFT,   {278, 314, 319, 349}}, \
                                        {PORT_1, TOUCH_RIGHT,  {391, 314, 429, 349}}, \
                                        {PORT_1, TOUCH_UP,     {335, 255, 373, 293}}, \
                                        {PORT_1, TOUCH_DOWN,   {335, 366, 373, 406}}, \
                                        {PORT_1, TOUCH_ENTER,  {335, 314, 373, 349}}

    #elif defined AVR32_EVK1101
        #define KEYPAD_LEDS  4
        #define KEYPAD "KeyPads/EVK1101.bmp"

                                        // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(190,190,190),RGB(50,255,0), 1, {70,  320, 83,  327 }, _GPIO_0, PA07}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {88,  320, 101, 327 }, _GPIO_0, PA08}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {109, 320, 122, 327 }, _GPIO_0, PA21}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {129, 320, 142, 327 }, _GPIO_0, PA22}

        #define BUTTON_KEY_DEFINITIONS  {PORT_1, SWITCH_PB0, {159, 333, 170, 344}}, \
                                        {PORT_1, SWITCH_PB1, {159, 365, 170, 377}}, \
                                        {PORT_0, JOY_STICK_CTR, {100, 351, 112, 364}}, \
                                        {PORT_1, JOY_STICK_A, {86, 341, 94, 374}}, \
                                        {PORT_1, JOY_STICK_B, {86, 341, 124,350}}, \
                                        {PORT_1, JOY_STICK_C, {116,338, 130,374}}, \
                                        {PORT_1, JOY_STICK_D, {86, 364, 126,374}},
    #elif defined AVR32_AT32UC3C_EK
        #define KEYPAD_LEDS  4
        #define KEYPAD "KeyPads/AT32UC3C-EK.bmp"

                                        // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(190,190,190),RGB(50,255,0), 1, {66,  231, 78,  237 }, _GPIO_0, DEMO_LED_1}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {93,  231, 106, 237 }, _GPIO_2, DEMO_LED_2}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {122, 231, 130, 237 }, _GPIO_3, DEMO_LED_3}, \
                                        {RGB(190,190,190),RGB(50,255,0), 1, {147, 231, 156, 237 }, _GPIO_3, DEMO_LED_4}

        #define BUTTON_KEY_DEFINITIONS  {PORT_0, SWITCH_PB0, {55,  342, 65,  353}}, \
                                        {PORT_0, SWITCH_PB1, {142, 342, 154, 353}}
    #elif defined AVR32_UC3_C2_XPLAINED                                  // {16}
        #define KEYPAD_LEDS  1
        #define KEYPAD "KeyPads/UC3-C2Xplained.bmp"

                                        // '0'            '1'    input state  center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(255,255,0),RGB(190,190,190), 1, {383, 55, 391, 70 }, _GPIO_3, PD21}

        #define BUTTON_KEY_DEFINITIONS  {PORT_2, BUTTON0, {16, 54, 50, 87}}, \
                                        {PORT_0, BUTTON1, {16, 194, 50, 228}}
    #endif
#endif

//#define BUZZER_SOUND                                                   // enable buzzer sound generation in the simulator (needs Winmm.lib added to the simulator linker setting)


// Internal HTML message pages
//
#define SUPPORT_INTERNAL_HTML_FILES                                      // enable the use of the following files
#define UPLOAD_FAILED             "<html><head><title>SW Upload failed</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font></i></b></td><br></td><td align=left><br><br>Sorry but upload has failed.</font><br><br><a href=""javascript:history.back()"">Return</a></body></html>"
#define SW_UPLOAD_COMPLETED       "<html><head><meta http-equiv=""refresh"" content=""10;URL=0Menu.htm""><title>AVR32 SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font> - SW Update</i></b></td><br></td><td align=left><br><br>SW Upload successful. The AVR32 target will now reset and start the new program. Please wait 10s...</body></html>"

#ifdef USE_TFTP
    #define TFTP_ERROR_MESSAGE    "uTasker AVR32 TFTP Error"             // fixed TFTP error test
#endif

#endif

#endif

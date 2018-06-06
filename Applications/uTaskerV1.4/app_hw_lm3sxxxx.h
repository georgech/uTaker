/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        app_hw_lm3sxxxx.h
   Project:     uTasker Demonstration project
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   *********************************************************************
   Application specific hardware configuration

   07.12.2008 Add SYSTICK priority and change peripheral priorities      {1}
   16.12.2008 Add LM3S3748/LM3S3768/LM3S5732 setup                       {2}
   16.01.2009 SST SPI FLASH support                                      {3}
   17.01.2009 Add LP3S2110 setup                                         {4}
   18.01.2009 Device UART number moved to LM3Sxxxx.h
   03.02.2009 Add TIMER interrupt and MODBUS timer configuration         {5}
   28.02.2009 Add timing test output port                                {6}
   12.03.2009 Add user definable SET_SPI_FLASH_MODE() and REMOVE_SPI_FLASH_MODE() {7}
   31.03.2009 Add ADC support                                            {8}
   09.05.2009 Add GLCD configuration                                     {9}
   16.05.2009 Add OLED configuration                                     {10}
   03.08.2009 Add _LM3S2139 board configuration                          {11}
   12.08.2009 Set up keyscan configuration                               {12}
   26.09.2009 Add GLCD configuration for Samsung based GLCD              {13}
   26.09.2009 Add OLED GLCD mode configuration                           {14}
   01.10.2009 Move "../../Hardware/Luminary/LM3SXXXX.h" include from types.h to this file, after detailed chip defines {15}
   05.10.2009 Add CGLCD_GLCD_MODE GLCD mode configuration                {16}
   15.10.2009 Add additional ST SPI FLASH devices                        {17}
   16.10.2009 Add NUMBER_EXTERNAL_SERIAL when serial interface active    {18}
   05.11.2009 Add EK_LM3S9B90 and TICK_INTERRUPT()                       {19}
   29.11.2009 Configure LAN_REPORT_ACTIVITY LEDs                         {20}
   29.11.2009 Adapt uFileSystem to LM3S9BXX                              {21}
   11.12.2009 Add SD-card interface                                      {22}
   22.02.2010 Correct SPI_FLASH_AT45DB161, SPI_FLASH_AT45DB321 and SPI_FLASH_AT45DB642 sizes {23}
   13.05.2010 Add IDM_L35_B inteligent display module with 3.5" TFT and touch screen (LM3S1958) {24}
   24.08.2010 Use port macro for SPI FLASH CS configuration              {25}
   28.09.2010 Add external UART configuration                            {26}
   10.10.2010 Add MODBUS_TIMER_16BIT to enable 16 bit timer channels to be used by MODBUS rather than full 32 bit timers {27}
   27.12.2010 Add #define SUPPORT_PWM_CONTROLLER                         {28}
   22.02.2011 Add INITIALISE_MONITOR_TIMER() and EXECUTION_DURATION()    {29}
   22.02.2011 Add DK_LM3S9B96                                            {30}

*/

#if defined _LM3SXXXX && !defined __APP_HW_LM3SXXXX__
#define __APP_HW_LM3SXXXX__

#ifdef _WINDOWS
    #define _SIM_PORTS fnSimPorts()
#else
    #define _SIM_PORTS
#endif

#define ROWLEY_2                                                         // enable when working with Rowley Crossworks 2

#ifdef EK_LM3S9B95
    #define _LM3S9B95
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     100000000                                // highest speed possible
    #define PART_CODE           CODE_LM3S9B95
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined DK_LM3S9B96                                                // {30}
    #define _LM3S9B96
    #define CRYSTAL_FREQ        16000000
    #define PLL_OUTPUT_FREQ     80000000                                 // highest speed possible
    #define PART_CODE           CODE_LM3S9B96
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined EK_LM3S9B90                                                // {19}
    #define _LM3S9B90
    #define CRYSTAL_FREQ        16000000
  //#define PLL_OUTPUT_FREQ     80000000                                 // highest speed possible
    #define PLL_OUTPUT_FREQ     50000000                                 // limited to 50MHz when FLASH writes are required due to errata - corrected from revision C devices
    #define PART_CODE           CODE_LM3S9B90
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined EK_LM3S6965
    #define _LM3S6965
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible
    #define PART_CODE           CODE_LM3S6965
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined EK_LM3S8962
    #define _LM3S8962
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible
    #define PART_CODE           CODE_LM3S8962
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
  //#define PIN_COUNT           PIN_COUNT_108_PIN                        // BGA
  //#define PACKAGE_TYPE        PACKAGE_BGA
#elif defined EK_LM3S1968
    #define _LM3S1968
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible
    #define PART_CODE           CODE_LM3S1968
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined IDM_L35_B                                                  // {24}
    #define _LM3S1958
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible
    #define PART_CODE           CODE_LM3S1958
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined _LM3S10X
  //#define _LM3S101
  //#define PART_CODE           CODE_LM3S101
    #define _LM3S102                                                     // with I2C
    #define PART_CODE           CODE_LM3S102
    #define PACKAGE_TYPE        PACKAGE_SOIC
    #define PIN_COUNT           PIN_COUNT_28_PIN
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     20000000                                 // highest speed possible for this chip
#elif defined EK_LM3S5732                                                // {2}
    #define _LM3S5732
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible for this chip
    #define PART_CODE           CODE_LM3S5732
    #define PIN_COUNT           PIN_COUNT_64_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined EK_LM3S3748                                                // {2}
    #define _LM3S3748
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible for this chip
    #define PART_CODE           CODE_LM3S3748
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define UART1_ON_PORT_C
#elif defined EK_LM3S3768                                                // {2}
    #define _LM3S3768
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     50000000                                 // highest speed possible for this chip
    #define PART_CODE           CODE_LM3S3768
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined EK_LM3S2110                                                // {4}
    #define _LM3S2110
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     25000000                                 // highest speed possible for this chip
    #define PART_CODE           CODE_LM3S2110
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#elif defined EK_LM3S2139                                                // {11}
    #define _LM3S2139
    #define CRYSTAL_FREQ        8000000
    #define PLL_OUTPUT_FREQ     25000000                                 // highest speed possible for this chip
    #define PART_CODE           CODE_LM3S2139
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
#else
                                                                         // other configurations can be added here
#endif

#include "../../Hardware/Luminary/LM3SXXXX.h"                            // {15}

// Memory set up for this target
//
#define BOOT_CODE_SIZE             0x800

#define APP_START_ADDRESS          (unsigned char *)(FLASH_START_ADDRESS + BOOT_CODE_SIZE) // from start of app code
#define APP_END_ADDRESS            (unsigned char *)(FLASH_START_ADDRESS + 0x18000)  // to the file system and parameters


// SPI FLASH system setup
//
#if defined SPI_FILE_SYSTEM  && !defined FLASH_FILE_SYSTEM
    #define uFILE_START 0x0
    #define EEPROM_32K                                                   // device size used
    #define CONFIG_SPI_PORTS()      
#else
    #define FILES_ADDRESSABLE                                            // file system is addressable (doesn't have to be retrieved)
#endif

#ifdef EEPROM_16K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device

    #define FILE_GRANULARITY (1024)                                      // File made up of 1k byte FLASH blocks
    #define SINGLE_FILE_SIZE (1*FILE_GRANULARITY)                        // each file a multiple of 1k
    #define FILE_SYSTEM_SIZE (16*SINGLE_FILE_SIZE)                       // 16k reserved for file system (including parameter blocks)
#endif
#ifdef EEPROM_32K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device

    #define FILE_GRANULARITY (1024)                                      // File made up of 1k byte FLASH blocks
    #define SINGLE_FILE_SIZE (1*FILE_GRANULARITY)                        // each file a multiple of 1k
    #define FILE_SYSTEM_SIZE (32*SINGLE_FILE_SIZE)                       // 32k reserved for file system (including parameter blocks)
#endif
#ifdef EEPROM_64K
    #define EEPROM_PAGE_SIZE 128                                         // respect larger page size in SPI EEPROM

    #define FILE_GRANULARITY (2*1024)                                    // File made up of 2k byte FLASH blocks
    #define SINGLE_FILE_SIZE (1*FILE_GRANULARITY)                        // each file a multiple of 2k
    #define FILE_SYSTEM_SIZE (32*SINGLE_FILE_SIZE)                       // 64k reserved for file system (including parameter blocks)
#endif


//#define SPI_FLASH_MULTIPLE_CHIPS                                       // activate when multiple physical chips are used
#define CS0_LINE            PORTA_BIT3                                   // CS0 line used when SPI FLASH is enabled
#define CS1_LINE            PORTA_BIT6                                   // CS1 line used when extended SPI FLASH is enabled
#define CS2_LINE            PORTA_BIT7                                   // CS2 line used when extended SPI FLASH is enabled
#define SPI_CS0_PORT        GPIODATA_A
#define SPI_CS1_PORT        GPIODATA_A
#define SPI_CS2_PORT        GPIODATA_A
                                                                         //
#ifdef SPI_FLASH_ST                                                      // ST SPI FLASH used
  //#define SPI_FLASH_STM25P05                                           // the available ST chips
  //#define SPI_FLASH_STM25P10
    #define SPI_FLASH_STM25P20
  //#define SPI_FLASH_STM25P40
  //#define SPI_FLASH_STM25P80                                               
  //#define SPI_FLASH_STM25P16                                               
  //#define SPI_FLASH_STM25P32                                               
  //#define SPI_FLASH_STM25P64
  //#define SPI_FLASH_STM25P128

    #if defined SPI_FLASH_STM25P05                                       // 64k {17}
        #define SPI_FLASH_PAGES             (1*256)                          
    #elif defined SPI_FLASH_STM25P10                                     // 128k
        #define SPI_FLASH_PAGES             (2*256)
    #elif defined SPI_FLASH_STM25P20                                     // 256k
        #define SPI_FLASH_PAGES             (4*256)
    #elif defined SPI_FLASH_STM25P40                                     // 512k
        #define SPI_FLASH_PAGES             (8*256)
    #elif defined SPI_FLASH_STM25P80                                     // 1M
        #define SPI_FLASH_PAGES             (16*256)
    #elif defined SPI_FLASH_STM25P16                                     // 2M
        #define SPI_FLASH_PAGES             (32*256)
    #elif defined SPI_FLASH_STM25P32                                     // 4M
        #define SPI_FLASH_PAGES             (64*256)
    #elif defined SPI_FLASH_STM25P64                                     // 8M
        #define SPI_FLASH_PAGES             (128*256)
    #elif defined SPI_FLASH_STM25P128                                    // 16M
        #define SPI_FLASH_PAGES             (256*256)
    #endif

    #define SPI_FLASH_PAGE_LENGTH 256
    #ifdef SPI_DATA_FLASH
        #define SPI_FLASH_SECTOR_LENGTH (16*SPI_FLASH_PAGE_LENGTH)       // sub-sector size of data FLASH
    #else
        #define SPI_FLASH_SECTOR_LENGTH (256*SPI_FLASH_PAGE_LENGTH)      // sector size of code FLASH
    #endif
    #define SPI_FLASH_BLOCK_LENGTH  SPI_FLASH_SECTOR_LENGTH
#elif defined SPI_FLASH_SST25                                            // {3}
  //#define SPI_FLASH_SST25VF010A                                        // the supported SST chips
    #define SPI_FLASH_SST25LF020A
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

    #define SPI_FLASH_SUB_SECTOR_LENGTH  (4*1024)                        // sub-sector size of SPI FLASH
    #define SPI_FLASH_HALF_SECTOR_LENGTH (32*1024)                       // half-sector size of SPI FLASH
    #define SPI_FLASH_SECTOR_LENGTH      (64*1024)                       // sector size of SPI FLASH (not available on A-versions)

    #define SPI_FLASH_PAGE_LENGTH        SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - smallest erasable block
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - file system granularity
#else
  //#define SPI_FLASH_AT45DB011                                          // {31} define the ATMEL type used here
  //#define SPI_FLASH_AT45DB021
  //#define SPI_FLASH_AT45DB041
    #define SPI_FLASH_AT45DB081
  //#define SPI_FLASH_AT45DB161
  //#define SPI_FLASH_AT45DB321
  //#define SPI_FLASH_AT45DB642
  //#define SPI_FLASH_AT45XXXXD_TYPE                                     // specify that a D-type rather than a B type is being used (only for the simulator)

    #if defined SPI_FLASH_AT45DB642                                      // define whether used in power of 2 mode or not
        #define SPI_FLASH_PAGE_LENGTH 1056                               // standard page size (B-device only allows 1056)
      //#define SPI_FLASH_PAGE_LENGTH 1024                               // size when power of 2 mode selected (only possible on D-device)
        #define SPI_FLASH_PAGES             (8*1024)                     // 8Meg part {23}
    #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
        #define SPI_FLASH_PAGE_LENGTH 528                                // standard page size (B-device only allows 528)
      //#define SPI_FLASH_PAGE_LENGTH 512                                // size when power of 2 mode selected (only possible on D-device)
        #if defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGES         (4*1024)                     // 2Meg part {23}
        #else
            #define SPI_FLASH_PAGES         (8*1024)                     // 4Meg part {23}
        #endif
    #else
        #define SPI_FLASH_PAGE_LENGTH 264                                // standard page size (B-device only allows 264)
      //#define SPI_FLASH_PAGE_LENGTH 256                                // size when power of 2 mode selected (only possible on D-device)
        #if defined SPI_FLASH_AT45DB011
            #define SPI_FLASH_PAGES         (512)                        // 128k part
        #elif defined SPI_FLASH_AT45DB021
            #define SPI_FLASH_PAGES         (1024)                       // 256k part
        #elif defined SPI_FLASH_AT45DB041
            #define SPI_FLASH_PAGES         (2*1024)                     // 512k part
        #elif defined SPI_FLASH_AT45DB081
            #define SPI_FLASH_PAGES         (4*1024)                     // 1Meg part
        #endif
    #endif

    #define SPI_FLASH_BLOCK_LENGTH (8*SPI_FLASH_PAGE_LENGTH)             // block size - a block can be deleted
    #define SPI_FLASH_SECTOR_LENGTH (64*4*SPI_FLASH_PAGE_LENGTH)         // exception sector 0a is 2k and sector 0b is 62k
#endif
#define SPI_DATA_FLASH_0_SIZE       (SPI_FLASH_PAGES*SPI_FLASH_PAGE_LENGTH) 
#define SPI_DATA_FLASH_1_SIZE       SPI_DATA_FLASH_0_SIZE 
#define SPI_DATA_FLASH_2_SIZE       SPI_DATA_FLASH_0_SIZE
#ifdef SPI_FLASH_MULTIPLE_CHIPS
    #define SPI_FLASH_DEVICE_COUNT  3                                    // only define CS_LINE for this amount of devices
    #if SPI_FLASH_DEVICE_COUNT > 2                                       // {25}
        #define SPI_DATA_FLASH_SIZE   (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE)
        #define CONFIGURE_CS_LINES()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (CS0_LINE | CS1_LINE | CS2_LINE), (CS0_LINE | CS1_LINE | CS2_LINE))
    #else
        #define SPI_DATA_FLASH_SIZE   (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)
        #define CONFIGURE_CS_LINES()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (CS0_LINE | CS1_LINE), (CS0_LINE | CS1_LINE))
    #endif
#else
    #define SPI_DATA_FLASH_SIZE       SPI_DATA_FLASH_0_SIZE                                  
    #define CONFIGURE_CS_LINES()      _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, CS0_LINE, CS0_LINE);
#endif


#define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH)     // SPI FLASH starts immediately after FLASH

#define SW_UPLOAD_FILE()       (*ucIp_Data == 'H') && (*(ucIp_Data+1) == 'S') && (*(ucIp_Data+2) == '.')  && (fnSPI_Flash_available() != 0)

#define SET_SPI_FLASH_MODE()                                             // {7}
#define REMOVE_SPI_FLASH_MODE()

#ifdef SDCARD_SUPPORT                                                    // {22}
    #if defined EK_LM3S6965
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PORTD_BIT0
        #define INITIALISE_SPI_SD_INTERFACE() _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT5), (PA2_SSI0Clk | PA5_SSI0Tx)); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT4, PORTA_BIT4); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0); \
                                           SSICR1_0 = 0; SSICPSR_0 = 2; \
                                           SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 300000/2)/300000) - 1) << SSI_SCR_SHIFT));

        #define ENABLE_SPI_SD_OPERATION()  SSICR1_0 = SSI_SSE
        #define SET_SD_CARD_MODE()         _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT4), (PA4_SSI0Rx))
        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 12000000/2)/12000000) - 1) << SSI_SCR_SHIFT))
        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte; SSIDR_0 = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) { SSISR_0 &= ~SSI_BSY; }
        #else
            #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) {}
        #endif
        #define READ_SPI_DATA()            (unsigned char)SSIDR_0

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()        _SETBITS(D, SPI_CS1_0)        // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()            _CLEARBITS(D, SPI_CS1_0)      // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()           _SETBITS(D, SPI_CS1_0)        // negate the CS line of the SD card to be read
    #elif defined EK_LM3S8962 
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PORTG_BIT0
        #define INITIALISE_SPI_SD_INTERFACE() _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT5), (PA2_SSI0Clk | PA5_SSI0Tx)); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT4, PORTA_BIT4); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(G, SPI_CS1_0, SPI_CS1_0); \
                                           SSICR1_0 = 0; SSICPSR_0 = 2; \
                                           SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 300000/2)/300000) - 1) << SSI_SCR_SHIFT));

        #define ENABLE_SPI_SD_OPERATION()  SSICR1_0 = SSI_SSE
        #define SET_SD_CARD_MODE()         _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT4), (PA4_SSI0Rx))
        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 6000000/2)/600000) - 1) << SSI_SCR_SHIFT))
        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte; SSIDR_0 = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) { SSISR_0 &= ~SSI_BSY; }
        #else
            #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) {}
        #endif
        #define READ_SPI_DATA()            (unsigned char)SSIDR_0

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()        _SETBITS(G, SPI_CS1_0)        // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()            _CLEARBITS(G, SPI_CS1_0)      // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()           _SETBITS(G, SPI_CS1_0)        // negate the CS line of the SD card to be read
    #elif defined EK_LM3S3748
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PORTA_BIT3
        #define INITIALISE_SPI_SD_INTERFACE() _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT5), (PA2_SSI0Clk | PA5_SSI0Tx)); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT4, PORTA_BIT4); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, SPI_CS1_0, SPI_CS1_0); \
                                           SSICR1_0 = 0; SSICPSR_0 = 2; \
                                           SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 300000/2)/300000) - 1) << SSI_SCR_SHIFT));

        #define ENABLE_SPI_SD_OPERATION()  SSICR1_0 = SSI_SSE
        #define SET_SD_CARD_MODE()         _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT4), (PA4_SSI0Rx))
        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 6000000/2)/600000) - 1) << SSI_SCR_SHIFT))
        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte; SSIDR_0 = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) { SSISR_0 &= ~SSI_BSY; }
        #else
            #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) {}
        #endif
        #define READ_SPI_DATA()            (unsigned char)SSIDR_0

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()        _SETBITS(A, SPI_CS1_0)        // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()            _CLEARBITS(A, SPI_CS1_0)      // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()           _SETBITS(A, SPI_CS1_0)        // negate the CS line of the SD card to be read
    #elif defined IDM_L35_B
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_1                  PORTE_BIT1
        #define INITIALISE_SPI_SD_INTERFACE() RCGC2 |= CGC_GPIOE; GPIOAFSEL_E = 0; \
                                           _CONFIG_PERIPHERAL(E, SSI1, 1, (PORTE_BIT0 | PORTE_BIT3), (PE0_SSI1Clk | PE3_SSI1Tx)); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTA_BIT2, PORTA_BIT2); \
                                           _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI_CS1_1, SPI_CS1_1); \
                                           SSICR1_1 = 0; SSICPSR_1 = 2; \
                                           SSICR0_1 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 300000/2)/300000) - 1) << SSI_SCR_SHIFT));

        #define ENABLE_SPI_SD_OPERATION()  SSICR1_1 = SSI_SSE
        #define SET_SD_CARD_MODE()         _CONFIG_PORT_PULLUP(E, PORTE_BIT2); _CONFIG_PERIPHERAL(E, SSI1, 1, (PORTE_BIT2), (PE2_SSI1Rx))
        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SSICR0_1 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 12000000/2)/12000000) - 1) << SSI_SCR_SHIFT))
        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)    SSIDR_1 = (unsigned char)byte; SSIDR_1 = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (SSISR_1 & SSI_BSY) { SSISR_1 &= ~SSI_BSY; }
        #else
            #define WRITE_SPI_CMD(byte)    SSIDR_1 = (unsigned char)byte
            #define WAIT_TRANSMISSON_END() while (SSISR_1 & SSI_BSY) {}
        #endif
        #define READ_SPI_DATA()            (unsigned char)SSIDR_1

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()        _SETBITS(E, SPI_CS1_1)        // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()            _CLEARBITS(E, SPI_CS1_1)      // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()           _SETBITS(E, SPI_CS1_1)        // negate the CS line of the SD card to be read
    #endif
#endif

// FLASH based File System setup
//
#if defined FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM                                          // this is a test setup for external SPI FLASH, with the parameters at the end of internal FLASH
        #define uFILE_START (SPI_FLASH_START)                            // FLASH location end of internal FLASH
        #define PARAMETER_BLOCK_START (SPI_FLASH_START - PAR_BLOCK_SIZE) // FLASH location at 2 parameter blocks short of end of internal FLASH
        #define SINGLE_FILE_SIZE (FILE_GRANULARITY)                      // each file a multiple of 16k (Atmel/ST data FLASH)/64k ST code FLASH
        #if SINGLE_FILE_SIZE < 20000                                     // {36}
            #if SPI_DATA_FLASH_SIZE < (64*SINGLE_FILE_SIZE)              // {42} allow testing smallest SPI FLASH
                #define FILE_SYSTEM_SIZE  SPI_DATA_FLASH_SIZE
            #else
                #define FILE_SYSTEM_SIZE (32*SINGLE_FILE_SIZE)           // 1Meg reserved for file system (assuming 8k file size) {34} [PAR_BLOCK_SIZE removed]
            #endif
        #else
            #define FILE_SYSTEM_SIZE (16*SINGLE_FILE_SIZE)               // 1Meg reserved for file system {35}
        #endif
    #else
        #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)                 // each file a multiple of 1k
        #if defined _TEMPEST_CLASS                                       // {21}
            #define PARAMETER_BLOCK_START 0x17000                        // FLASH location at 92k start
            #define uFILE_START 0x19000                                  // FLASH location at 100k start

            #define SINGLE_FILE_SIZE (4*FLASH_GRANULARITY)               // each file a multiple of 4k to ensure that files are made of 4 x 1k sectors to avoid problems with neighbours
            #define FILE_SYSTEM_SIZE (39*SINGLE_FILE_SIZE)               // 156k reserved for file system
        #else
            #define PARAMETER_BLOCK_START 0x18000                        // {34} FLASH location at 96k start
            #define uFILE_START 0x19000                                  // FLASH location at 100k start

            #define SINGLE_FILE_SIZE (1*FLASH_GRANULARITY)               // each file a multiple of 1k
            #define FILE_SYSTEM_SIZE (156*SINGLE_FILE_SIZE)              // 156k reserved for file system
        #endif
    #endif
#endif

#ifdef USE_PARAMETER_BLOCK
    #if defined _TEMPEST_CLASS                                           // {21}
        #define PARAMETER_BLOCK_SIZE    (4*FLASH_GRANULARITY)            // use 4k parameter block
    #else
        #define PARAMETER_BLOCK_SIZE    (2*FLASH_GRANULARITY)            // use 2k parameter block (made up of two sectors)
    #endif

    #ifdef USE_PAR_SWAP_BLOCK
        #define PAR_BLOCK_SIZE      (2*PARAMETER_BLOCK_SIZE)
    #else
        #define PAR_BLOCK_SIZE      (1*PARAMETER_BLOCK_SIZE)
    #endif
#else
    #define PAR_BLOCK_SIZE          (0)
#endif


#define USB_DOWNLOAD_FILE           "0"                                  // USB firmware uploads are saved here

#define USB_BUS_SWITCH                                                   // control USB bus power using USB0EPE and USB0PFLT

// Serial interfaces
//
#ifdef SERIAL_INTERFACE
    #define WELCOME_MESSAGE_UART   "\r\n\nHello, world... LM3Sxxxx\r\n"
    #define EXT_UART_SC16IS7XX                                           // {26} use an external SPI based UART to extend the number of channels
    #define NUMBER_EXTERNAL_SERIAL   2                                   // {18}
    #define NUMBER_SERIAL  CHIP_HAS_UARTS                                // the number of physical queue needed for serial interface(s)
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255
    #define SERIAL_PORT_0  3                                             // if we open UART channel 0 we simulate using this com port on the PC (0 for no mapping)
    #define SERIAL_PORT_1  4                                             // if we open UART channel 1 we simulate using this com port on the PC
    #define SERIAL_PORT_2  15                                            // if we open UART channel 2 we simulate using this com port on the PC
    #define SERIAL_PORT_EXT_0  3                                         // if we open UART channel 3 we simulate using this com port on the PC
    #define SERIAL_PORT_EXT_1  11                                        // if we open UART channel 4 we simulate using this com port on the PC

    #if defined _LM3S3748
        #define SERIAL_SUPPORT_DMA                                       // enable UART DMA support (only available on some processors - ensure that the linker script used keeps the start of SRAM [512 or 1024 bytes] free)
    #endif
    #define SUPPORT_HW_FLOW                                              // enable hardware flow control support

    #define DEMO_UART        3                                           // use UART 4 (second external)
  //#define PPP_UART         1                                           // use UART 1 for PPP
    #define MODBUS_UART_0    4
    #define MODBUS_UART_1    1
    #define MODBUS_UART_2    2
    #define TX_BUFFER_SIZE   (256)                                       // the size of RS232 input and output buffers
    #define RX_BUFFER_SIZE   (64)

    #ifdef SUPPORT_HW_FLOW                                               // define the ports for RTS/CTS use
        #define RTS_0_PORT         GPIODATA_D
        #define RTS_0_PIN          PORTD_BIT1
        #define RTS_0_PORT_POWER   CGC_GPIOD
        #define RTS_0_PORT_ENABLE  GPIODEN_D
        #define RTS_0_PORT_DDR     GPIODIR_D

        #define RTS_1_PORT         GPIODATA_D
        #define RTS_1_PIN          PORTD_BIT0
        #define RTS_1_PORT_POWER   CGC_GPIOD
        #define RTS_1_PORT_ENABLE  GPIODEN_D
        #define RTS_1_PORT_DDR     GPIODIR_D

        #define RTS_2_PORT         GPIODATA_A
      //#define RTS_2_PIN          PORTA_BIT2
        #define RTS_2_PORT_POWER   CGC_GPIOA
        #define RTS_2_PORT_ENABLE  GPIODEN_A
        #define RTS_2_PORT_DDR     GPIODIR_A

        #define CTS_0_PIN          PORTA_BIT3
        #define CTS_0_PORT_POWER   CGC_GPIOA
        #define CTS_0_PORT_ENABLE  GPIODEN_A
        #define CTS_0_PORT_DDR     GPIODIR_A
        #define CTS_0_PORT         PORTA
        #define CTS_0_INT_PRIORITY 3

        #define CTS_1_PIN          PORTA_BIT7
        #define CTS_1_PORT_POWER   CGC_GPIOA
        #define CTS_1_PORT_ENABLE  GPIODEN_A
        #define CTS_1_PORT_DDR     GPIODIR_A
        #define CTS_1_PORT         PORTA
        #define CTS_1_INT_PRIORITY 3

        #define CTS_2_PIN          PORTA_BIT6
        #define CTS_2_PORT_POWER   CGC_GPIOA
        #define CTS_2_PORT_ENABLE  GPIODEN_A
        #define CTS_2_PORT_DDR     GPIODIR_A
        #define CTS_2_PORT         PORTA
        #define CTS_2_INT_PRIORITY 3
    #endif

    // External SPI based UART configuration                             {26}
    //
    #define SC16IS7XX_CLOCK_FROM_TIMER                                   // generate the clock from a timer (the clock is generates in fnUserHWInit() and a port holds the device in reset until configuration begins)
    #define CONFIG_SC16IS7XX_CLOCK()             _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT0, 0); \
                                                 RCGC1 |= (CGC_TIMER0); SRCR1 &= ~(CGC_TIMER0); \
                                                 GPTMCFG_0 = GPTMCFG_16_BIT_MODE; \
                                                 _CONFIG_PERIPHERAL(D, TIMER0, 1, (PORTD_BIT7), PD7_CCP1); \
                                                 GPTMTBMR_0 = (TAMS_PWM_MODE | TMR_PERIODIC); \
                                                 GPTMTBMATCHR_0 = (26/2); GPTMTBILR_0 = 26; \
                                                 GPTMCTL_0 |= (TBEN | TBPWML);

    #define SC16IS7XX_CS                         PORTA_BIT3
    #define CONFIGURE_SPI_FOR_SC16IS7XX()        _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT5 | PORTA_BIT4), (PA2_SSI0Clk | PA5_SSI0Tx | PA4_SSI0Rx)); \
                                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, SC16IS7XX_CS, SC16IS7XX_CS); \
                                                 SSICR1_0 = 0; SSICPSR_0 = 2; \
                                                 SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPH | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 1000000/2)/1000000) - 1) << SSI_SCR_SHIFT)); \
                                                 SSICR1_0 = SSI_SSE; \
                                                 _SETBITS(A, PORTA_BIT0);

  //#define SC16IS7XX_CLOCK                      14745600                // clock rate of the external SC16IS7XX devices
    #define SC16IS7XX_CLOCK                     (PLL_OUTPUT_FREQ/27)     // timer clock speed (1.852MHz)

    #define ENABLE_CHIP_SELECT_SC16IS7XX_0()     _CLEARBITS(A, PORTA_BIT3)
    #define DISABLE_CHIP_SELECTS_SC16IS7XX()     _SETBITS(A, PORTA_BIT3)
    #define SC16IS7XX_SPI_TDR                    SSIDR_0
    #define SC16IS7XX_SPI_RDR                    SSIDR_0
    #define SC16IS7XX_SPI_SR                     SSISR_0
    #define EXT_UART_0_1_INT_PORT                PORTA
    #define EXT_UART_0_1_INT_BIT                 PORTA_BIT1
    #define CHECK_UART_0_1_INT_PENDING()         _READ_PORT_MASK(A, PORT_BIT1)

    #define PRIORITY_IRQ                         PORTA_INTERRUPT_PRIORITY
#else
    #define TX_BUFFER_SIZE   (256)
    #define RX_BUFFER_SIZE   (256)
#endif

#define SUPPORT_ADC                                                      // {8}
#ifdef SUPPORT_ADC
    #define ADC_SIM_STEP_SIZE                      (200  * ADC_FULL_SCALE) // 200mV steps when simulating

    #define AN0_START_VOLTAGE                      (33   * ADC_FULL_SCALE) // initial voltages when simulating (mV)
    #define AN1_START_VOLTAGE                      (500  * ADC_FULL_SCALE)
    #define AN2_START_VOLTAGE                      (1000 * ADC_FULL_SCALE)
    #define AN3_START_VOLTAGE                      (1500 * ADC_FULL_SCALE)
    #define AN4_START_VOLTAGE                      (2100 * ADC_FULL_SCALE)
    #define AN5_START_VOLTAGE                      (2400 * ADC_FULL_SCALE)
    #define AN6_START_VOLTAGE                      (2700 * ADC_FULL_SCALE)
    #define AN7_START_VOLTAGE                      (3000 * ADC_FULL_SCALE)
    #if (PART_DC1 & ADC1_PRESENT1)                                       // second ADC - assume 16 ADC inputs available
        #define AN8_START_VOLTAGE                  (55 * ADC_FULL_SCALE) // initial voltages when simulating (mV)
        #define AN9_START_VOLTAGE                  (82 * ADC_FULL_SCALE)
        #define AN10_START_VOLTAGE                 (102 * ADC_FULL_SCALE)
        #define AN11_START_VOLTAGE                 (235 * ADC_FULL_SCALE)
        #define AN12_START_VOLTAGE                 (789 * ADC_FULL_SCALE)
        #define AN13_START_VOLTAGE                 (1099 * ADC_FULL_SCALE)
        #define AN14_START_VOLTAGE                 (2087 * ADC_FULL_SCALE)
        #define AN15_START_VOLTAGE                 (2567 * ADC_FULL_SCALE)
    #endif
#endif

#define SUPPORT_TIMER                                                  // {5} support hardware timer interrupt configuration
#if defined MODBUS_RTU && !defined SUPPORT_TIMER
    #define SUPPORT_TIMER                                                // MODBUS required HW timer in RTU mode
#endif

#define _TIMER_INTERRUPT_SETUP  TIMER_INTERRUPT_SETUP

#define MODBUS_TIMER_16BIT                                               // {27}

#ifdef MODBUS_TIMER_16BIT
    #define MODBUS0_TIMER_CHANNEL    0                                   // MODBUS serial port 0 uses timer 0 (16 bit mode)
    #define MODBUS0_TIMER_MODE       (TIMER_SINGLE_SHOT | TIMER_US_VALUE | TIMER_16BIT_CHANNEL_A); // channel A
    #define MODBUS1_TIMER_CHANNEL    0                                   // MODBUS serial port 1 uses timer 0 (16 bit mode)
    #define MODBUS1_TIMER_MODE       (TIMER_SINGLE_SHOT | TIMER_US_VALUE | TIMER_16BIT_CHANNEL_B); // channel B
#else
    #define MODBUS0_TIMER_CHANNEL    0                                   // {5} MODBUS serial port 0 uses timer 0 (32 bit mode)
    #define MODBUS1_TIMER_CHANNEL    1                                   // MODBUS serial port 1 uses timer 1 (32 bit mode)
#endif


#define SUPPORT_PORT_INTERRUPTS                                          // support code for port interrupts
#ifdef SUPPORT_PORT_INTERRUPTS
    #define SUPPORT_PORT_A_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_B_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_C_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_D_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_E_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_F_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_G_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_H_INTERRUPTS                                    // specifically support interrupts on this port
    #define SUPPORT_PORT_J_INTERRUPTS                                    // specifically support interrupts on this port
#endif

#define SUPPORT_PWM_CONTROLLER                                           // {28} support PWM controller

#define TICK_INTERRUPT()                                                 // {19} user callback from system TICK

                                                                         // {29}
#define INITIALISE_MONITOR_TIMER() 
#define EXECUTION_DURATION()  0


// I2C Interface
//
#if defined I2C_INTERFACE
    #define OUR_I2C_CHANNEL         0                                    // use I2C0 for demo
    #define NUMBER_I2C              CHIP_HAS_I2C                         // I2C channels available
#endif

#define USER_REG0_VALUE             0x00b61a00                           // MAC address in user registers (00-1a-b6-00-22-1d)
#define USER_REG1_VALUE             0x001d2200
#define USER_REG2_VALUE             0xffffffff                           // Tempest class
#define USER_REG3_VALUE             0xffffffff                           // Tempest class

// LAN interface
//
#ifdef ETH_INTERFACE
    #define MAC_FROM_USER_REG                                            // use MAC address programmed in the user register
  //#define LAN_REPORT_ACTIVITY                                          // transmissions/receptions and errors are reported to chosen task (for link LED control, etc)

    #ifdef LAN_REPORT_ACTIVITY                                           // {20}
        #define LNKLED                 PORTH_BIT1
        #define ACTLED                 PORTH_BIT2
        #define SPDLED                 PORTH_BIT3
        #define DUPLED                 PORTH_BIT4
        #define COLLED                 PORTH_BIT5
        #define ACTIVITY_LED_OFF()     _CLEARBITS(H, ACTLED)
        #define ACTIVITY_LED_ON()      _SETBITS(H, ACTLED)
        #define TURN_LINK_LED_ON()     _SETBITS(H, LNKLED)
        #define SPEED_LED_ON()         _SETBITS(H, (SPDLED | LNKLED))
        #define SPEED_LED_OFF()        _CLEARBITS(H, (SPDLED | LNKLED))
        #define LINK_DOWN_LEDS()       _CLEARBITS(H, (LNKLED | ACTLED | COLLED | SPDLED | DUPLED))

        #define CONFIGURE_LAN_LEDS()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, (LNKLED | ACTLED | COLLED | SPDLED | DUPLED), 0)
    #endif

    #define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE 1
    #define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE 4
#endif

#ifdef USE_IP
    #define LAN_BUFFER_SIZE           1536                               // LM3Sxxxx has ample space for full tx buffer
#else
    #define LAN_BUFFER_SIZE           256                                // if using Ethernet without IP the buffer size can be set here
#endif

#ifdef USE_BUFFERED_TCP                                                  // if using a buffer for TCP to allow interractive data applications (like TELNET)
    #define TCP_BUFFER                2800                               // size of TCP buffer (with USE_BUFFERED_TCP) - generous with LM3Sxxxx
    #define TCP_BUFFER_FRAME          1400                               // allow this max. TCP frame size
#endif

#ifdef USE_HTTP
    #define HTTP_BUFFER_LENGTH        1400                               // we send frames with this maximum amount of payload data - generous with LM3Sxxxx
#endif

#ifdef USE_FTP                                                           // specify FTP support details
    #define FTP_WELCOME_RESPONSE         "220 Welcome LM3Sxxxx FTP.\r\n"
    //                                        ^^^^^^^^^^^^^^^^^^            customise the FTP welcome here
#endif

// SMTP settings
//
#if defined EK_LM3S9B95
    #define SENDERS_EMAIL_ADDRESS             "EK_LM3S9B95@uTasker.com"  // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "EK_LM3S9B95 Test"         // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the EK_LM3S9B95.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your EK_LM3S9B95!!";
#elif defined DK_LM3S9B96                                                // {30}
    #define SENDERS_EMAIL_ADDRESS             "DK_LM3S9B96@uTasker.com"  // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "DK_LM3S9B96 Test"         // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the DK_LM3S9B96.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your DK_LM3S9B96!!";
#elif defined EK_LM3S9B90
    #define SENDERS_EMAIL_ADDRESS             "EK_LM3S9B90@uTasker.com"  // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "EK_LM3S9B90 Test"         // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the EK_LM3S9B90.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your EK_LM3S9B90!!";
#elif defined EK_LM3S6965
    #define SENDERS_EMAIL_ADDRESS             "EK_LM3S6965@uTasker.com"  // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "EK_LM3S6965 Test"         // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the EK_LM3S6965.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your EK_LM3S6965!!";
#elif defined EK_LM3S8962
    #define SENDERS_EMAIL_ADDRESS             "EK_LM3S8962@uTasker.com"  // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "EK_LM3S8962 Test"         // Email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the EK_LM3S8962.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your EK_LM3S8962!!";
#endif

// internal HTML message pages
//
#define SUPPORT_INTERNAL_HTML_FILES                                      // enable the use of the following files
#define UPLOAD_FAILED         "<html><head><title>SW Upload failed</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font></i></b></td><br></td><td align=left><br><br>Sorry but upload has failed.</font><br><br><a href=""javascript:history.back()"">Return</a></body></html>"
#define SW_UPLOAD_COMPLETED   "<html><head><meta http-equiv=""refresh"" content=""10;URL=0Menu.htm""><title>LM3SXXXX SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font> - SW Update</i></b></td><br></td><td align=left><br><br>SW Upload successful. The LM3SXXXX target will now reset and start the new program. Please wait 10s...</body></html>"
#ifdef USE_TFTP
    #define TFTP_ERROR_MESSAGE               "uTasker TFTP Error"        // Fixed TFTP error test
#endif

// Global Hardware Timer setup
//
#ifdef GLOBAL_TIMER_TASK
    #define MILLISEC               LOW_RES_MS                            // 1ms about 2% accurate ms values, 22us resolution and limit of about 1.42s max. It uses a 16 bit timer at bus/1024 frequency
  //#define MILLISEC               MED_RES_MS                            // 1ms about 0.2% accurate ms values, 3us resolution and limit of about 177ms max. It uses a 16 bit timer at bus/128 frequency
  //#define MILLISEC               HIGH_RES_MS                           // 1ms about 0.05% accurate ms values, 1us resolution and limit of about 44ms max. It uses a 16 bit timer at bus/32 frequency
#endif

// Define interrupt priorities in the system
//
#define SYSTICK_PRIORITY           7                                     // {1}

#define PRIORITY_UART0             6
#define PRIORITY_UART1             6
#define PRIORITY_UART2             6
#define PORTA_INTERRUPT_PRIORITY   5
#define PRIORITY_HW_TIMER          5
#define PRIORITY_TIMERS            5
#define PRIORITY_USB_OTG           4
#define PRIORITY_I2C0              4
#define PRIORITY_I2C1              4
#define PRIORITY_TWI               4
#define PRIORITY_TICK_TIMER        3
#define PRIORITY_ADC               2
#define PORTB_INTERRUPT_PRIORITY   2                                     // default if PB4 ADC trigger configured
#define PRIORITY_EMAC              1


#if defined EK_LM3S6965 || defined EK_LM3S8962 || defined EK_LM3S9B95 || defined DK_LM3S9B96
    #define DEMO_LED_1                 PORTF_BIT0
    #define DEMO_LED_2                 PORTF_BIT1
    #define DEMO_LED_3                 PORTF_BIT2
    #define DEMO_LED_4                 PORTF_BIT3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  DEMO_LED_1
    #define DEMO_DIGITAL_ENABLE        GPIODEN_F
    #define DEMO_LED_DDR               GPIODIR_F
    #define DEMO_LED_PORT              GPIODATA_F
    #ifdef USE_MAINTENANCE
        #define ENABLE_LED_PORT()      RCGC2 |= CGC_GPIOF; SRCR2 &= ~CGC_GPIOF; // clock port and ensure not in reset
    #else
        #define ENABLE_LED_PORT()      RCGC2 |= CGC_GPIOF; SRCR2 &= ~CGC_GPIOF; DEMO_DIGITAL_ENABLE = BLINK_LED; DEMO_LED_DDR = BLINK_LED; // clock port and ensure not in reset
    #endif
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(F, BLINK_LED)        // blink the LED, if set as output

    #if defined SUPPORT_OLED || defined OLED_GLCD_MODE                   // {10}{14} 128 x 96 OLED 
        #if defined EK_LM3S6965
            #define ACTIVATE_OLED_POWER()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, PORTC_BIT6, PORTC_BIT6); // power OLDE
            #define CONFIGURE_OLED_DC()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, PORTC_BIT7, PORTC_BIT7); // DC pin
            #define OLED_DC_L()            _CLEARBITS(C, PORTC_BIT7)
            #define OLED_DC_H()            _SETBITS(C, PORTC_BIT7)
            #undef  GLCD_Y
            #undef  LCD_PIXEL_COLOUR                                     // smaller, yellow OLED on this board
            #define GLCD_Y                 64                            // vertical resolution of the OLED display in pixels
            #define LCD_PIXEL_COLOUR       (COLORREF)RGB(255,255,0)
            #undef  BIG_PIXEL
            #define SPI_SPEED              500000
        #else
            #define ACTIVATE_OLED_POWER()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT7, PORTA_BIT7); // power OLED
            #define CONFIGURE_OLED_DC()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, PORTA_BIT6, PORTA_BIT6); // DC pin
            #define OLED_DC_L()            _CLEARBITS(A, PORTA_BIT6)
            #define OLED_DC_H()            _SETBITS(A, PORTA_BIT6)
            #define SPI_SPEED              1000000
        #endif

        // configure SSI0 peripheral lines, ensure SPI disabled (master mode). FSS automatically controls the CS line
        // set clock prescaler (even prescale 2..254) 
        // configure SPI mode at 1MHz and final enable for operation
        #define CONFIGURE_SPI()        _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT3 | PORTA_BIT5), (PA2_SSI0Clk | PA5_SSI0Tx | PA3_SSI0Fss)); \
                                       SSICR1_0 = 0; SSICPSR_0 = 2; \
                                       SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + SPI_SPEED/2)/SPI_SPEED) - 1) << SSI_SCR_SHIFT)); \
                                       SSICR1_0 = SSI_SSE;
        #define WRITE_SPI_DATA(byte)   SSIDR_0 = (unsigned char)byte
        #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte
        #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) {}
        #define READ_SPI_DATA()        (unsigned char)SSIDR_0
    #endif
#elif defined EK_LM3S9B90                                                // {19}
    #define DEMO_LED_1                 PORTD_BIT0
    #define DEMO_LED_2                 PORTD_BIT1
    #define DEMO_LED_3                 PORTD_BIT2
    #define DEMO_LED_4                 PORTD_BIT3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  DEMO_LED_1
    #define DEMO_DIGITAL_ENABLE        GPIODEN_D
    #define DEMO_LED_DDR               GPIODIR_D
    #define DEMO_LED_PORT              GPIODATA_D
    #ifdef USE_MAINTENANCE
        #define ENABLE_LED_PORT()      RCGC2 |= CGC_GPIOD; SRCR2 &= ~CGC_GPIOD; // clock port and ensure not in reset
    #else
        #define ENABLE_LED_PORT()      RCGC2 |= CGC_GPIOD; SRCR2 &= ~CGC_GPIOD; DEMO_DIGITAL_ENABLE = BLINK_LED; DEMO_LED_DDR = BLINK_LED; // clock port and ensure not in reset
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(D, BLINK_LED)        // blink the LED, if set as output
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()
#elif defined EK_LM3S1968
    #define DEMO_LED_1                 PORTG_BIT2
    #define DEMO_LED_2                 PORTG_BIT1
    #define DEMO_LED_3                 PORTG_BIT0
    #define DEMO_LED_4                 PORTG_BIT3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define ENABLE_LED_PORT()          RCGC2 |= CGC_GPIOG; SRCR2 &= ~CGC_GPIOG; // clock port and ensure not in reset
    #define BLINK_LED                  DEMO_LED_1
    #define DEMO_DIGITAL_ENABLE        GPIODEN_G
    #define DEMO_LED_DDR               GPIODIR_G
    #define DEMO_LED_PORT              GPIODATA_G
    #if !defined USE_MAINTENANCE || defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(G, (BLINK_LED), (BLINK_LED))
    #endif

    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(G, BLINK_LED)        // blink the LED, if set as output

    #if defined SUPPORT_OLED                                             // {10} 128 x 96 OLED 
        #define CONFIGURE_OLED_DC()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, PORTH_BIT2, PORTH_BIT2); // DC pin
        #define ACTIVATE_OLED_POWER()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, PORTH_BIT3, PORTH_BIT3); // power OLDE

        // configure SSI0 peripheral lines, ensure SPI disabled (master mode). FSS automatically control sthe CS line
        // set clock prescaler (even prescale 2..254) 
        // configure SPI mode at 1MHz and final enable for operation
        #define CONFIGURE_SPI()        _CONFIG_PERIPHERAL(A, SSI0, 1, (PORTA_BIT2 | PORTA_BIT3 | PORTA_BIT5), (PA2_SSI0Clk | PA5_SSI0Tx | PA3_SSI0Fss)); \
                                       SSICR1_0 = 0; SSICPSR_0 = 2; \
                                       SSICR0_0 = (FRS_FREESCALE_SPI | SSI_SPO | DSS_8_BIT | ((((MASTER_CLOCK/2 + 1000000/2)/1000000) - 1) << SSI_SCR_SHIFT)); \
                                       SSICR1_0 = SSI_SSE;

        #define OLED_DC_L()            _CLEARBITS(H, PORTH_BIT2)
        #define OLED_DC_H()            _SETBITS(H, PORTH_BIT2)
        #define WRITE_SPI_DATA(byte)   SSIDR_0 = (unsigned char)byte
        #define WRITE_SPI_CMD(byte)    SSIDR_0 = (unsigned char)byte
        #define WAIT_TRANSMISSON_END() while (SSISR_0 & SSI_BSY) {}
        #define READ_SPI_DATA()        (unsigned char)SSIDR_0
    #endif
#elif defined IDM_L35_B
    #define DEMO_LED_1                 PORTG_BIT2
    #define DEMO_LED_2                 PORTG_BIT1
    #define DEMO_LED_3                 PORTG_BIT0
    #define DEMO_LED_4                 PORTG_BIT3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define ENABLE_LED_PORT()          RCGC2 |= CGC_GPIOG; SRCR2 &= ~CGC_GPIOG; // clock port and ensure not in reset
    #define BLINK_LED                  DEMO_LED_1
    #define DEMO_DIGITAL_ENABLE        GPIODEN_G
    #define DEMO_LED_DDR               GPIODIR_G
    #define DEMO_LED_PORT              GPIODATA_G
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(G, BLINK_LED)        // blink the LED, if set as output
#elif defined _LM3S10X
    #define DEMO_LED_1                 PORTB_BIT0
    #define DEMO_LED_2                 PORTB_BIT1
    #define DEMO_LED_3                 PORTB_BIT2
    #define DEMO_LED_4                 PORTB_BIT3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define ENABLE_LED_PORT()          RCGC2 |= CGC_GPIOB; SRCR2 &= ~CGC_GPIOB; // clock port and ensure not in reset
    #define BLINK_LED                  DEMO_LED_1
    #define DEMO_DIGITAL_ENABLE        GPIODEN_B
    #define DEMO_LED_DDR               GPIODIR_B
    #define DEMO_LED_PORT              GPIODATA_B
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(B, BLINK_LED)        // blink the LED, if set as output
#elif defined EK_LM3S3748 || defined EK_LM3S3768 || defined EK_LM3S5732 || defined _LM3S2110 || defined _LM3S2139 // {4}{11}
    #define DEMO_LED_1                 PORTF_BIT0
    #define DEMO_LED_2                 PORTF_BIT1
    #define DEMO_LED_3                 PORTF_BIT2
    #define DEMO_LED_4                 PORTF_BIT3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)
    #if defined _LM3S2110 || defined _LM3S2139                           // {4} {11}
        #define BLINK_LED              DEMO_LED_3
    #else
        #define BLINK_LED              DEMO_LED_1
    #endif
    #define DEMO_DIGITAL_ENABLE        GPIODEN_F
    #define DEMO_LED_DDR               GPIODIR_F
    #define DEMO_LED_PORT              GPIODATA_F
    #ifdef USE_MAINTENANCE
        #define ENABLE_LED_PORT()      _CONFIG_DRIVE_PORT_OUTPUT_VALUE(F, BLINK_LED, 0) // RCGC2 |= CGC_GPIOF; SRCR2 &= ~CGC_GPIOF; // clock port and ensure not in reset
    #else
        #define ENABLE_LED_PORT()      RCGC2 |= CGC_GPIOF; SRCR2 &= ~CGC_GPIOF; DEMO_DIGITAL_ENABLE = BLINK_LED; DEMO_LED_DDR = BLINK_LED; // clock port and ensure not in reset
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(F, BLINK_LED)        // blink the LED, if set as output
#endif

#define CONFIG_TIMER_TEST_LEDS()       DEMO_LED_DDR |= (DEMO_LED_3 | DEMO_LED_4)
#define TIMER_TEST_LED_ON()            DEMO_LED_PORT |= (DEMO_LED_3);
#define TIMER_TEST_LED2_ON()           DEMO_LED_PORT |= (DEMO_LED_4);
#define TIMER_TEST_LED_OFF()           DEMO_LED_PORT &= ~(DEMO_LED_3);
#define TIMER_TEST_LED2_OFF()          DEMO_LED_PORT &= ~(DEMO_LED_4);


#define CONFIG_TEST_OUTPUT()                                             // {6} we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
#define TOGGLE_TEST_OUTPUT()           DEMO_LED_PORT ^= DEMO_LED_2

// Ports
//
#if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
    #define INIT_WATCHDOG_LED()        ENABLE_LED_PORT();                // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
#endif
//#define TOGGLE_WATCHDOG_LED()        DEMO_LED_PORT ^= BLINK_LED        // blink the LED, if set as output

#define INIT_WATCHDOG_DISABLE()      //PMC_PCER = (PIOA);                // ensure clocks to port
#define WATCHDOG_DISABLE()             0                                 // enable watchdog
//#define WATCHDOG_DISABLE()           1                                 // don't enable watchdog
#define WATCHDOG_TIMEOUT_MS            1500                              // watchdog timeout of 1.5 seconds (maximum at 50MHz 85s)

// GLCD
//
#if defined EK_LM3S1968                                                  // port F used a data lines, configured as inputs, port D0 used as C/D, port G 5,6,7 used as RST, RD and WR
    #define GLCD_RST               PORTG_BIT5
    #define GLCD_C_D               PORTD_BIT0
    #define GLCD_WR                PORTG_BIT7
    #define GLCD_RD                PORTG_BIT6

    #define SET_PULL_DOWNS()       GPIOPDR_F = 0xff;
    #define REMOVE_PULL_DOWNS()    GPIOPDR_F = 0x00;
    #define CONFIGURE_GLCD()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(G, (GLCD_RST | GLCD_WR | GLCD_RD), (GLCD_WR | GLCD_RD)); _CONFIG_PORT_INPUT(F, 0xff); SET_PULL_DOWNS(); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, GLCD_C_D, GLCD_C_D)

    #define GLCD_DATAASOUTPUT()    GPIODIR_F = 0xff 
    #define GLCD_DATAASINPUT()     GPIODIR_F = 0x00 

    #define GLCD_DATAOUT(data)     GPIODATA_F = data  
    #define GLCD_DATAIN()          GPIODATA_F

    #define GLCD_WR_H()            GPIODATA_G |= GLCD_WR
    #define GLCD_WR_L()            GPIODATA_G &= ~(GLCD_WR)

    #define GLCD_CD_H()            GPIODATA_D |= GLCD_C_D
    #define GLCD_CD_L()            GPIODATA_D &= ~(GLCD_C_D)

    #define GLCD_RD_H()            GPIODATA_G |= GLCD_RD
    #define GLCD_RD_L()            GPIODATA_G &= ~(GLCD_RD)

    #define GLCD_DELAY_WRITE()     //GLCD_WR_L()                         // no write delay since the data is stable for long enough at full speed
    #define GLCD_DELAY_READ()      GLCD_RD_L()                           // one extra operation to ensure set up time of read

    #define GLCD_RST_H()           GPIODATA_G |= GLCD_RST 
    #define GLCD_RST_L()           GPIODATA_G &= ~(GLCD_RST)

    #ifdef _WINDOWS
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   20                                // the maximum number of writes to the GLCD before the task yields
    #endif
#elif defined EK_LM3S3748 && (defined GLCD_COLOR || defined CGLCD_GLCD_MODE) // {16} color graphic LCD: port G used a data lines, configured as inputs, port E0..3 used as controls
    #define GLCD_RST               PORTE_BIT0
    #define GLCD_C_D               PORTB_BIT2
    #define GLCD_WR                PORTC_BIT4
    #define GLCD_RD                PORTC_BIT5
    #define GLCD_BACKLIGHT         PORTF_BIT1
                                   // backlight off // LCD A0 // LCD read and write lines  // data bus
    #define CONFIGURE_GLCD()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(F, (GLCD_BACKLIGHT | PORTF_BIT0), 0);  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (GLCD_C_D), (GLCD_C_D)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (GLCD_WR | GLCD_RD), (GLCD_WR | GLCD_RD)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(G, 0xff, 0xff);
    #undef  GLCD_X                                                       // overwrite the display dimensions
    #undef  GLCD_Y
    #define GLCD_X                 128                                   // horizontal resolution of the LCD display in pixels
    #define GLCD_Y                 128                                   // vertical resolution of the LCD display in pixels
    #define ENABLE_BACKLIGHT()     _SETBITS(F, GLCD_BACKLIGHT)

    #define GLCD_DATAASOUTPUT()    GPIODIR_G = 0xff 
    #define GLCD_DATAASINPUT()     GPIODIR_G = 0x00 

    #define GLCD_DATAOUT(data)     GPIODATA_G = data  
    #define GLCD_DATAIN()          GPIODATA_G

    #define GLCD_WR_H()            GPIODATA_C |= GLCD_WR
    #define GLCD_WR_L()            GPIODATA_C &= ~(GLCD_WR)

    #define GLCD_CD_H()            GPIODATA_B &= ~GLCD_C_D               // note  - inverted to GLCD
    #define GLCD_CD_L()            GPIODATA_B |= (GLCD_C_D)

    #define GLCD_RD_H()            GPIODATA_C |= GLCD_RD
    #define GLCD_RD_L()            GPIODATA_C &= ~(GLCD_RD)

    #define GLCD_DELAY_WRITE()     //GLCD_WR_L()                         // no write delay since the data is stable for long enough at full speed
    #define GLCD_DELAY_READ()      //GLCD_RD_L()                         // no write delay since the data is stable for long enough at full speed
#else                                                                    // {9} port B used as data lines, configured as inputs, port A used as controls
    #if defined IDM_L35_B
        #define TFT_BACKLIGHT PORTH_BIT0
        #define TFT_RESET     PORTC_BIT6
        #define TFT_WRITE     PORTH_BIT1
        #define TFT_READ      PORTH_BIT2
        #define TFT_DC        PORTH_BIT3

        #define TFT_DATA_15_8 (PORTF_BIT7 | PORTF_BIT6 | PORTF_BIT5 | PORTF_BIT4 | PORTF_BIT3 | PORTF_BIT2 | PORTF_BIT1 | PORTF_BIT0)
        #define TFT_DATA_7_0  (PORTB_BIT7 | PORTB_BIT6 | PORTB_BIT5 | PORTB_BIT4 | PORTB_BIT3 | PORTB_BIT2 | PORTB_BIT1 | PORTB_BIT0)

        #define CONFIGURE_GLCD() \
            _UNLOCK_GPIO(B, PORTB_BIT7); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, (TFT_BACKLIGHT | TFT_WRITE | TFT_READ | TFT_DC), (TFT_WRITE | TFT_READ | TFT_DC)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, TFT_RESET, 0); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(F, TFT_DATA_15_8, TFT_DATA_15_8); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, TFT_DATA_7_0, TFT_DATA_7_0); \
            _LOCK_PORT(B, PORTB_BIT7);
        #define GLCD_RST_H()           _SETBITS(C, TFT_RESET)
        #define ENABLE_BACKLIGHT()     _SETBITS(H, TFT_BACKLIGHT);
        #ifdef _WINDOWS
            #define MAX_GLCD_WRITE_BURST   5000                          // the maximum number of writes to the GLCD before the task yields
        #else
            #define MAX_GLCD_WRITE_BURST   100                           // the maximum number of writes to the GLCD before the task yields
        #endif
        #define SUPPORT_TOUCH_SCREEN
        #define GLCD_BACKLIGHT_CONTROL
                                                                         // revert back from peripheral use in case it was already programmed
        #define BACK_LIGHT_MAX_INTENSITY()  _RESET_PORT_PERIPHERAL(H, PORTH_BIT0); \
                                            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, PORTH_BIT0, PORTH_BIT0);                                           
        #define BACK_LIGHT_MIN_INTENSITY()  _RESET_PORT_PERIPHERAL(H, PORTH_BIT0); \
                                            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, PORTH_BIT0, 0);
        #define _GLCD_BACKLIGHT_TIMER       3                            // generate backlight control signal with timer 3, PWM channel A
        #define _GLCD_TIMER_MODE_OF_OPERATION  (TIMER_PWM_A)
        #define _GLCD_BACKLIGHT_PWM_FREQUENCY  TIMER_US_DELAY(TIMER_FREQUENCY_VALUE(770)) // 770 Hz backlight frequency (lowest frequency possible and should not be higher than 1kHz for good linearity)
    #elif !defined _GLCD_SAMSUNG
        #define GLCD_RST               PORTA_BIT2
        #define GLCD_C_D               PORTA_BIT3
        #define GLCD_WR                PORTA_BIT4
        #define GLCD_RD                PORTA_BIT5

        #define SET_PULL_DOWNS()       GPIOPDR_B = 0xff;
        #define REMOVE_PULL_DOWNS()    GPIOPDR_B = 0x00;

        #define CONFIGURE_GLCD()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (GLCD_RST | GLCD_WR | GLCD_RD | GLCD_C_D), (GLCD_WR | GLCD_RD | GLCD_C_D)); _CONFIG_PORT_INPUT(B, 0xff); SET_PULL_DOWNS()

        #define GLCD_DATAASOUTPUT()    GPIODIR_B = 0xff 
        #define GLCD_DATAASINPUT()     GPIODIR_B = 0x00

        #define GLCD_DATAOUT(data)     GPIODATA_B = data  
        #define GLCD_DATAIN()          GPIODATA_B

        #define GLCD_WR_H()            _SETBITS(A, GLCD_WR)
        #define GLCD_WR_L()            _CLEARBITS(A, GLCD_WR)

        #define GLCD_CD_H()            _SETBITS(A, GLCD_C_D)
        #define GLCD_CD_L()            _CLEARBITS(A, GLCD_C_D)

        #define GLCD_RD_H()            _SETBITS(A, GLCD_RD)
        #define GLCD_RD_L()            _CLEARBITS(A, GLCD_RD)

        #define GLCD_DELAY_WRITE()     //GLCD_WR_L()                     // no write delay since the data is stable for long enough at full speed
        #define GLCD_DELAY_READ()      GLCD_RD_L()                       // one extra operation to ensure set up time of read

        #define GLCD_RST_H()           _SETBITS(A, GLCD_RST)
        #define GLCD_RST_L()           _CLEARBITS(A, GLCD_RST)

        #ifdef _WINDOWS
            #define MAX_GLCD_WRITE_BURST   1000                          // the maximum number of writes to the GLCD before the task yields
        #else
            #define MAX_GLCD_WRITE_BURST   20                            // the maximum number of writes to the GLCD before the task yields
        #endif
    #else                                                                // {13}
        #define GLCD_RST               PORTC_BIT4                        // reset
        #define GLCD_RS                PORTC_BIT5                        // GLCD Register Select
        #define GLCD_RW                PORTC_BIT6                        // GLCD Read/Write
        #define GLCD_ENA               PORTC_BIT7                        // GLCD Enable

        #define GLCD_CS0               PORTH_BIT0                        // LCD Controller 0 Chip Select - 2 controller chips for 128 x 64
        #define GLCD_CS1               PORTH_BIT1                        // LCD Controller 1 Chip Select

        #define SET_PULL_DOWNS()       GPIOPDR_F = 0xff;
        #define REMOVE_PULL_DOWNS()    GPIOPDR_F = 0x00;

        #define CONFIGURE_GLCD() \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, ( GLCD_RST | GLCD_RS  | \
                                                 GLCD_RW  | GLCD_ENA ),\
                                                 GLCD_RW ); \
            _CONFIG_PORT_INPUT(F, 0xff); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(H, ( GLCD_CS0 | GLCD_CS1 ), 0x00 );

        #define GLCD_DATAASOUTPUT()    GPIODIR_F = 0xff 
        #define GLCD_DATAASINPUT()     GPIODIR_F = 0x00 

        #define GLCD_DATAOUT(data)     GPIODATA_F = data  
        #define GLCD_DATAIN()          GPIODATA_F

        #define GLCD_RS_H()            GPIODATA_C |= GLCD_RS
        #define GLCD_RS_L()            GPIODATA_C &= ~(GLCD_RS)

        #define GLCD_RW_H()            GPIODATA_C |= GLCD_RW
        #define GLCD_RW_L()            GPIODATA_C &= ~(GLCD_RW)

        #define GLCD_CS0_H()           GPIODATA_H |= GLCD_CS0
        #define GLCD_CS0_L()           GPIODATA_H &= ~(GLCD_CS0)
    
        #define GLCD_CS1_H()           GPIODATA_H |= GLCD_CS1
        #define GLCD_CS1_L()           GPIODATA_H &= ~(GLCD_CS1)
    
        #define GLCD_DELAY_WRITE()                                       // no write delay since the data is stable for long enough at full speed
        #define GLCD_DELAY_READ()      GLCD_RW_H()                       // one extra operation to ensure set up time of read

        #define GLCD_RST_H()           GPIODATA_C |= GLCD_RST 
        #define GLCD_RST_L()           GPIODATA_C &= ~(GLCD_RST)

        #define GLCD_ENA_H()           GPIODATA_C |= GLCD_ENA
        #define GLCD_ENA_L()           GPIODATA_C &= ~(GLCD_ENA)

        #ifdef _WINDOWS
            #define MAX_GLCD_WRITE_BURST   1000                          // the maximum number of writes to the GLCD before the task yields
        #else
            #define MAX_GLCD_WRITE_BURST   80                            // the maximum number of writes to the GLCD before the task yields
        #endif
    #endif
#endif


#define PORTA_DEFAULT_INPUT        0xffffffff                            // initial port input states for simulator
#define PORTB_DEFAULT_INPUT        0xffffffff
#define PORTC_DEFAULT_INPUT        0xffffffff
#define PORTD_DEFAULT_INPUT        0xffffffff
#define PORTE_DEFAULT_INPUT        0xffffffff
#define PORTF_DEFAULT_INPUT        0xffffffff
#define PORTG_DEFAULT_INPUT        0xffffffff
#define PORTH_DEFAULT_INPUT        0xffffffff
#define PORTJ_DEFAULT_INPUT        0xffffffff

// User port mapping
//
#define USER_PORT_1_BIT            PORTB_BIT0                            // use free PA pins on Eval board
#define USER_PORT_2_BIT            PORTB_BIT1
#if defined EK_LM3S3748 && (defined GLCD_COLOR || defined CGLCD_GLCD_MODE) // {16}
    #define USER_PORT_3_BIT        0                                     // don't disturb CGLCD
#else
    #define USER_PORT_3_BIT        PORTB_BIT2
#endif
#define USER_PORT_4_BIT            PORTB_BIT3
#define USER_PORT_5_BIT            PORTB_BIT4
#define USER_PORT_6_BIT            PORTB_BIT5
#define USER_PORT_7_BIT            PORTB_BIT6
#define USER_PORT_8_BIT            PORTB_BIT7
#define USER_PORT_9_BIT            PORTC_BIT0
#define USER_PORT_10_BIT           PORTC_BIT1
#define USER_PORT_11_BIT           PORTC_BIT2
#define USER_PORT_12_BIT           PORTC_BIT3
#if defined EK_LM3S3748 && (defined GLCD_COLOR || defined CGLCD_GLCD_MODE) // {16}
    #define USER_PORT_13_BIT       0                                     // don't disturb CGLCD
    #define USER_PORT_14_BIT       0
#else
    #define USER_PORT_13_BIT       PORTC_BIT4
    #define USER_PORT_14_BIT       PORTC_BIT5
#endif
#define USER_PORT_15_BIT           PORTC_BIT6
#define USER_PORT_16_BIT           PORTC_BIT7

// Port use definitions
//
#define POWER_UP_USER_PORTS()      RCGC2 |= (CGC_GPIOB | CGC_GPIOC);

#define ENABLE_USER_PORT_1()       GPIODEN_B |= USER_PORT_1_BIT;
#define USER_PORT_1                GPIODATA_B
#define USER_DDR_1                 GPIODIR_B
#define ENABLE_USER_PORT_2()       GPIODEN_B |= USER_PORT_2_BIT;
#define USER_PORT_2                GPIODATA_B
#define USER_DDR_2                 GPIODIR_B
#define ENABLE_USER_PORT_3()       GPIODEN_B |= USER_PORT_3_BIT;
#define USER_PORT_3                GPIODATA_B
#define USER_DDR_3                 GPIODIR_B
#define ENABLE_USER_PORT_4()       GPIODEN_B |= USER_PORT_4_BIT;
#define USER_PORT_4                GPIODATA_B
#define USER_DDR_4                 GPIODIR_B
#define ENABLE_USER_PORT_5()       GPIODEN_B |= USER_PORT_5_BIT;
#define USER_PORT_5                GPIODATA_B
#define USER_DDR_5                 GPIODIR_B
#define ENABLE_USER_PORT_6()       GPIODEN_B |= USER_PORT_6_BIT;
#define USER_PORT_6                GPIODATA_B
#define USER_DDR_6                 GPIODIR_B
#define ENABLE_USER_PORT_7()       GPIODEN_B |= USER_PORT_7_BIT;
#define USER_PORT_7                GPIODATA_B
#define USER_DDR_7                 GPIODIR_B
#define ENABLE_USER_PORT_8()       GPIODEN_B |= USER_PORT_8_BIT;
#define USER_PORT_8                GPIODATA_B
#define USER_DDR_8                 GPIODIR_B
#define ENABLE_USER_PORT_9()       GPIODEN_C |= USER_PORT_9_BIT;
#define USER_PORT_9                GPIODATA_C
#define USER_DDR_9                 GPIODIR_C
#define ENABLE_USER_PORT_10()      GPIODEN_C |= USER_PORT_10_BIT;
#define USER_PORT_10               GPIODATA_C
#define USER_DDR_10                GPIODIR_C
#define ENABLE_USER_PORT_11()      GPIODEN_C |= USER_PORT_11_BIT;
#define USER_PORT_11               GPIODATA_C
#define USER_DDR_11                GPIODIR_C
#define ENABLE_USER_PORT_12()      GPIODEN_C |= USER_PORT_12_BIT;
#define USER_PORT_12               GPIODATA_C
#define USER_DDR_12                GPIODIR_C
#define ENABLE_USER_PORT_13()      GPIODEN_C |= USER_PORT_13_BIT;
#define USER_PORT_13               GPIODATA_C
#define USER_DDR_13                GPIODIR_C
#define ENABLE_USER_PORT_14()      GPIODEN_C |= USER_PORT_14_BIT;
#define USER_PORT_14               GPIODATA_C
#define USER_DDR_14                GPIODIR_C
#define ENABLE_USER_PORT_15()      GPIODEN_C |= USER_PORT_15_BIT;
#define USER_PORT_15               GPIODATA_C
#define USER_DDR_15                GPIODIR_C
#define ENABLE_USER_PORT_16()      GPIODEN_C |= USER_PORT_16_BIT;
#define USER_PORT_16               GPIODATA_C
#define USER_DDR_16                GPIODIR_C

#if KEY_COLUMNS > 0
    // Keypad (scan out D4..D7, scan in E0..E3)                          // {12}
    //
    #define KEY_ROW_IN_1               PORTE_BIT0
    #define KEY_ROW_IN_2               PORTE_BIT1
    #define KEY_ROW_IN_3               PORTE_BIT2
    #define KEY_ROW_IN_4               PORTE_BIT3

    #define KEY_ROW_IN_PORT_1_REF      E
    #define KEY_ROW_IN_PORT_2_REF      E
    #define KEY_ROW_IN_PORT_3_REF      E
    #define KEY_ROW_IN_PORT_4_REF      E

    #define KEY_COL_OUT_1              PORTD_BIT4
    #define KEY_COL_OUT_2              PORTD_BIT5
    #define KEY_COL_OUT_3              PORTD_BIT6
    #define KEY_COL_OUT_4              PORTD_BIT7

    #define KEY_COL_OUT_PORT_1         GPIODATA_D
    #define KEY_COL_OUT_DDR_1          GPIODIR_D
    #define KEY_COL_OUT_PORT_2         GPIODATA_D
    #define KEY_COL_OUT_DDR_2          GPIODIR_D
    #define KEY_COL_OUT_PORT_3         GPIODATA_D
    #define KEY_COL_OUT_DDR_3          GPIODIR_D
    #define KEY_COL_OUT_PORT_4         GPIODATA_D
    #define KEY_COL_OUT_DDR_4          GPIODIR_D

                                                                         // drive each column low
    #define DRIVE_COLUMN_1()           _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_1, 0) // drive output low (column 1)
    #define DRIVE_COLUMN_2()           _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_2, 0) // drive output low (column 2)
    #define DRIVE_COLUMN_3()           _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_3, 0) // drive output low (column 3)
    #define DRIVE_COLUMN_4()           _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_4, 0) // drive output low (column 4)

                                                                         // drive high (to avoid slow rise time) then set back as input
    #define RELEASE_COLUMN_1()         _SETBITS(D, KEY_COL_OUT_1); _FLOAT_PORT(D, KEY_COL_OUT_1) 
    #define RELEASE_COLUMN_2()         _SETBITS(D, KEY_COL_OUT_2); _FLOAT_PORT(D, KEY_COL_OUT_2)
    #define RELEASE_COLUMN_3()         _SETBITS(D, KEY_COL_OUT_3); _FLOAT_PORT(D, KEY_COL_OUT_3)
    #define RELEASE_COLUMN_4()         _SETBITS(D, KEY_COL_OUT_4); _FLOAT_PORT(D, KEY_COL_OUT_4)

    #define KEY_ROW_IN_PORT_1          GPIODATA_E
    #define KEY_ROW_IN_PORT_2          GPIODATA_E
    #define KEY_ROW_IN_PORT_3          GPIODATA_E
    #define KEY_ROW_IN_PORT_4          GPIODATA_E


                                                                         // reset any changes ready for next scan sequence
    #define RESET_SCAN()         

    #define INIT_KEY_SCAN()            _CONFIG_PORT_INPUT(E, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4)); \
                                       GPIOPUR_E |= (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4); \
                                       _CONFIG_PORT_INPUT(D, (KEY_COL_OUT_1 | KEY_COL_OUT_2 | KEY_COL_OUT_3 | KEY_COL_OUT_4));
#else                                                                    // not matrix scanning input
  #if defined EK_LM3S2139
    #define KEY_IN_1                   PORTG_BIT0
    #define KEY_IN_2                   PORTG_BIT1
    #define KEY_IN_3                   PORTG_BIT2
    #define KEY_IN_4                   PORTG_BIT3
    #define KEY_IN_5                   PORTG_BIT4
    #define KEY_IN_6                   PORTG_BIT5
    #define KEY_IN_7                   PORTG_BIT6
    #define KEY_IN_8                   PORTG_BIT7

    #define INIT_KEY_STATE             (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8) // all keys are expected to be pulled up at start

    #define INIT_KEY_SCAN()            _CONFIG_PORT_INPUT(G, (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8)); GPIOPUR_G |= (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8)
    #define READ_KEY_INPUTS()          _READ_PORT(G)


    #define KEY_1_PORT_REF             G                                 // to allow simulator to map key pad to inputs
    #define KEY_1_PORT_PIN             KEY_IN_1
    #define KEY_5_PORT_REF             G
    #define KEY_5_PORT_PIN             KEY_IN_2
    #define KEY_9_PORT_REF             G
    #define KEY_9_PORT_PIN             KEY_IN_3
    #define KEY_13_PORT_REF            G
    #define KEY_13_PORT_PIN            KEY_IN_4

    #define KEY_2_PORT_REF             G
    #define KEY_2_PORT_PIN             KEY_IN_5
    #define KEY_6_PORT_REF             G
    #define KEY_6_PORT_PIN             KEY_IN_6
    #define KEY_10_PORT_REF            G
    #define KEY_10_PORT_PIN            KEY_IN_7
    #define KEY_14_PORT_REF            G
    #define KEY_14_PORT_PIN            KEY_IN_8
  #elif defined EK_LM3S8962
    #define KEY_IN_1                   PORTE_BIT0
    #define KEY_IN_2                   PORTE_BIT2
    #define KEY_IN_3                   PORTE_BIT3
    #define KEY_IN_4                   PORTE_BIT1
    #define KEY_IN_5                   PORTF_BIT1


    #define INIT_KEY_STATE             (0x1f)                            // all keys are expected to be pulled up at start

    #define INIT_KEY_SCAN()            _CONFIG_PORT_INPUT(E, (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4)); GPIOPUR_E |= (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4); \
                                       _CONFIG_PORT_INPUT(F, (KEY_IN_5)); GPIOPUR_F |= (KEY_IN_5)

    #define READ_KEY_INPUTS()          _READ_PORT_MASK(E, (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4)) | (_READ_PORT_MASK(F, KEY_IN_5) << 3)


    #define KEY_1_PORT_REF             E                                 // to allow simulator to map key pad to inputs
    #define KEY_1_PORT_PIN             KEY_IN_1
    #define KEY_5_PORT_REF             E
    #define KEY_5_PORT_PIN             KEY_IN_2
    #define KEY_9_PORT_REF             E
    #define KEY_9_PORT_PIN             KEY_IN_3
    #define KEY_2_PORT_REF             E
    #define KEY_2_PORT_PIN             KEY_IN_4
    #define KEY_6_PORT_REF             F
    #define KEY_6_PORT_PIN             KEY_IN_5
  #else
    #define KEY_IN_1                   PORTD_BIT0
    #define KEY_IN_2                   PORTD_BIT1
    #define KEY_IN_3                   PORTD_BIT2
    #define KEY_IN_4                   PORTD_BIT3
    #define KEY_IN_5                   PORTD_BIT4
    #define KEY_IN_6                   PORTD_BIT5
    #define KEY_IN_7                   PORTD_BIT6
    #define KEY_IN_8                   PORTD_BIT7

    #define INIT_KEY_STATE             (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8) // all keys are expected to be pulled up at start

    #define INIT_KEY_SCAN()            _CONFIG_PORT_INPUT(D, (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8)); GPIOPUR_D |= (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8)
    #define READ_KEY_INPUTS()          _READ_PORT(D)


    #define KEY_1_PORT_REF             D                                 // to allow simulator to map key pad to inputs
    #define KEY_1_PORT_PIN             KEY_IN_1
    #define KEY_5_PORT_REF             D
    #define KEY_5_PORT_PIN             KEY_IN_2
    #define KEY_9_PORT_REF             D
    #define KEY_9_PORT_PIN             KEY_IN_3
    #define KEY_13_PORT_REF            D
    #define KEY_13_PORT_PIN            KEY_IN_4

    #define KEY_2_PORT_REF             D
    #define KEY_2_PORT_PIN             KEY_IN_5
    #define KEY_6_PORT_REF             D
    #define KEY_6_PORT_PIN             KEY_IN_6
    #define KEY_10_PORT_REF            D
    #define KEY_10_PORT_PIN            KEY_IN_7
    #define KEY_14_PORT_REF            D
    #define KEY_14_PORT_PIN            KEY_IN_8
  #endif
#endif

// LCD interface: Backlight control PORTE_BIT0 : Data bus (4 Bit) PORTA_BIT7..PORTA_BIT4 / (8 bit) PORTA_BIT7..PORTA_BIT0 : RS PORTE_BIT1, RW PORTE_BIT2, E PORTE_BIT3
//
typedef unsigned char LCD_BUS_PORT_SIZE;                                 // we use 8 bit ports
typedef unsigned char LCD_CONTROL_PORT_SIZE;
//#define LCD_BUS_8BIT                                                   // data bus in 8 bit mode
#define LCD_BUS_4BIT                                                     // data bus in 4 bit mode

#ifdef LCD_BUS_8BIT
    #define LCD_BUS_MASK         0xff
    #define DATA_SHIFT_RIGHT     0
    #define DATA_SHIFT_LEFT      0                                       // byte shift down required to bring data into position
#else
    #define LCD_BUS_MASK         0xf0
    #define DATA_SHIFT_RIGHT     0
    #define DATA_SHIFT_LEFT      0                                       // nibble shift down required to bring data into position
#endif

#define O_CONTROL_RS            PORTE_BIT1
#define O_WRITE_READ            PORTE_BIT2
#define O_CONTROL_EN            PORTE_BIT3
#define O_LCD_BACKLIGHT         PORTE_BIT0

#define O_CONTROL_LINES         (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
#define IO_BUS_PORT_DAT         GPIODATA_A
#define IO_BUS_PORT_DAT_IN      GPIODATA_A
#define O_CONTROL_PORT_DAT      GPIODATA_E
#define IO_BUS_PORT_DDR         GPIODIR_A

#define SET_DATA_LINES_INPUT()  IO_BUS_PORT_DDR &= ~LCD_BUS_MASK; _SIM_PORTS
#define SET_DATA_LINES_OUTPUT() IO_BUS_PORT_DDR |= LCD_BUS_MASK; _SIM_PORTS
#define SET_BUS_DATA(x)         IO_BUS_PORT_DAT = x; _SIM_PORTS;
#define O_SET_CONTROL_LOW(x)    O_CONTROL_PORT_DAT &= ~x; _SIM_PORTS;
#define O_SET_CONTROL_HIGH(x)   O_CONTROL_PORT_DAT |= x; _SIM_PORTS;

// Drive the control lines R/W + LCD Backlight '1', RS + E '0' and the data lines with all high impedance at start up
// enable clocks to port first
#define INITIALISE_LCD_CONTROL_LINES()       RCGC2 |= (CGC_GPIOA | CGC_GPIOE); \
                                             GPIODEN_A |= LCD_BUS_MASK; GPIODEN_E |= (O_CONTROL_LINES | O_LCD_BACKLIGHT); \
                                             SET_DATA_LINES_INPUT(); \
                                             GPIODIR_E = (O_CONTROL_LINES | O_LCD_BACKLIGHT); \
                                             O_SET_CONTROL_LOW(O_CONTROL_LINES); _SIM_PORTS; O_SET_CONTROL_HIGH(O_LCD_BACKLIGHT); \
                                             _SIM_PORTS;

#define LCD_DRIVE_DATA()          SET_DATA_LINES_OUTPUT();  SET_DATA_LINES_OUTPUT();
                                  // ensure data bus outputs (delay) by repetitions according to processor speed

#define CLOCK_EN_HIGH()           O_SET_CONTROL_HIGH(O_CONTROL_EN); O_SET_CONTROL_HIGH(O_CONTROL_EN); _SIM_PORTS;
                                  // clock EN to high state - repeat to slow down (delay)

#define DELAY_ENABLE_CLOCK_HIGH() O_CONTROL_PORT_DAT &= ~(O_CONTROL_EN);

#endif                                                                   // endif - defined _LM3SXXXX && !defined __APP_HW_LM3SXXXX__

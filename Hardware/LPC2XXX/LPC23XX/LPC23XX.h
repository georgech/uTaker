/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	    LPC23XX.h
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    22.04.2008 Add RTC
    03.12.2008 Support IAR5                                              {1}
    04.12.2008 All Read Only and Write Only registers declared volatile  {2}
    17.12.2008 PORTS_AVAILABLE and PORT_WIDTH moved here from WinSim.c   {3}
    07.02.2009 Add port interrupt support                                {4}
    01.03.2009 Add timer interrupt support                               {5}
    31.08.2009 Add semicolon after fnSimPorts()                          {6}
    02.10.2009 Add POWER_UP and POWER_DOWN macros                        {7}
    08.10.2009 Correct POST_DIVIDE on LPC21XX parts                      {8}
    02.11.2009 Use fast IO with LPC2104/5/6 since new revisions have fast ports {9}
    02.11.2009 Correct _WRITE_PORT and _WRITE_PORT_MASK macros for simulator {10}
    07.11.2009 Add option timer mode and new modes, plus LPC2103 PWM registers {11}
    03.12.2009 Extended part IDs                                         {12}
    07.01.2010 Conditionally set FIOxMASK registers as volatile          {13}
    26.01.2010 Add SD/MMC registers                                      {14}
    23.02.2010 Part IDs removed from app_hw_lpc23xx.h to here            {15}
    23.02.2010 Add LPC214x                                               {16}
    25.02.2010 Adjust LPC21XX VIC address                                {17}
    06.03.2010 Adjust 512K FLASH size to (512 - 8) to reflect the use by ISP {18}
    06.03.2010 Correct I2C1/CAP1 configuration                           {19}
    13.03.2010 Add USB device                                            {20}
    09.06.2010 Adapte 83848 PHY registers for interrupt support          {21}
    20.06.2010 Rename generic PHY registers                              {22}
    30.07.2010 Add general purpose DMA controller                        {23}
    02.08.2010 Add PWM module                                            {24}
    06.08.2010 Add SDCARD_MALLOC()                                       {25}
    06.08.2010 Add additional LPC24XX peripheral pins                    {26}
    11.08.2010 Add ADC registers                                         {27}
    29.08.2010 Add SPI registers                                         {28}
    03.09 2010 Correct _WRITE_PORT() macro                               {29}
    30.01 2011 Add LPC2131/32/34/36/38                                   {30}
    03.02 2011 Add CAN                                                   {31}
    25.02.2011 Ensure all GPIO registers are volatile and correct legacy port macros {32}
    19.04.2011 Correct _TOGGLE_PORT() and _WRITE_PORT_MASK() macros for legacy ports {33}
    19.04.2011 Declare UART LCR register as volatile                     {34}
    04.06.2011 Add IAR_STACK_SIZE                                        {35}
    08.06.2011 Correct bit definition of SIMULATION_RESET                {36}
    08.06.2011 SUPP set volatile                                         {37}
    03.08.2011 Add SIZE_OF_RAM for compatibility                         {38}
    18.11.2011 Add _EXCEPTION(x)                                         {39}
    17.02.2012 Include simLPC23XX.h after device defines have been set   {40}
    17.02.2012 Improve PCLK configuration for LPC213x/LPC2148            {41}
    18.02.2012 Modify RTC parameters for compatibility                   {42}
    10.07.2013 Add PHY interface for multi-port devices                  {43}
    10.10.2013 RANDOM_SEED_LOCATION relocated from LPC23XX.c to this file {44}
    10.10.2013 BOOT_MAIL_BOX added                                       {45}

*/

#ifdef _WINDOWS
    #define _SIM_PORT_CHANGE   fnSimPorts(-1);                           // {6}
    #define _EXCEPTION(x)            *(unsigned char *)0 = 0             // generate exception when simulating {39}
#else
    #define _EXCEPTION(x)                                                // ignore on target
    #ifdef COMPILE_IAR
        #if defined COMPILE_IAR5                                         // {1}
            #include <intrinsics.h>
        #else
            #include <intrinsic.h>
        #endif
    #endif
    #define _SIM_PORT_CHANGE
#endif


#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary
#define _ALIGN_HEAP_4                                                    // ensure long word alignment
#define _LITTLE_ENDIAN                                                   // compile project in LITTLE ENDIAN mode since the LPC23xx is fixed in this mode

#ifdef SD_CONTROLLER_DMA
    extern void *uMallocUSB(unsigned short __size);
    #define SDCARD_MALLOC(x) uMallocUSB(x)                               // {25} use USB memory area
#else
    #define SDCARD_MALLOC(x) uMalloc(x)                                  // use standard uMalloc()
#endif

#define IRC_FREQUENCY    4000000                                         // internal RC nominal frequncy

#define start_application(call_address)   ((void (*)(void))call_address)(); // call specified address

#ifdef DISABLE_PLL
    #define MASTER_CLOCK CRYSTAL_FREQ
#else
    #ifdef _LPC21XX
        #define MASTER_CLOCK  PLL_OUTPUT_FREQ_INT                        // {8}
    #else
        #define MASTER_CLOCK (PLL_OUTPUT_FREQ_INT/POST_DIVIDE)
    #endif
#endif
#define CCLK             MASTER_CLOCK
#define PLL_OUTPUT_FREQ  MASTER_CLOCK
#define USB_CLOCK        48000000                                        // for USB to work this must be the case. It is assumed that it is correct if used.


#define CAST_POINTER_ARITHMETIC unsigned long                            // LPC23XX uses 32 bit pointers
#define SAVE_COMPLETE_FLASH                                              // when simulating, save complete flash contents and not just the file system contents

#ifdef _LPC21XX
    #define PLL_FREQUENCY_CONFIGURATION      (((PLL_MUL - 1) & 0x1f) | (POST_DIVIDE << 5))
    #define PLL_USB_FREQUENCY_CONFIGURATION  (((PLL_USB_MUL - 1) & 0x1f) | (USB_POST_DIVIDE << 5))
#else
    #define PLL_FREQUENCY_CONFIGURATION      (((PLL_MUL - 1) & 0x7fff) | (((OSC_DIVIDE-1) & 0xff) << 16))
#endif
#define CPU_CLOCK_CONFIGURATION              ((POST_DIVIDE-1) & 0x0ff)
#define USB_FREQUENCY_CONFIGURATION          (((PLL_OUTPUT_FREQ_INT/USB_CLOCK) - 1) & 0x0f)


#if defined LPC2101                                                      // {15}
    #define SIZE_OF_FLASH (8*1024)                                       // 8k FLASH
    #define SIZE_OF_LOCAL_RAM (2 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2101            // part id which can be read via ISP/IAP
#elif defined LPC2102
    #define SIZE_OF_FLASH (16*1024)                                      // 16k FLASH
    #define SIZE_OF_LOCAL_RAM (4 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2102            // part id which can be read via ISP/IAP
#elif defined LPC2103
    #define SIZE_OF_FLASH (32*1024)                                      // 32k FLASH
    #define SIZE_OF_LOCAL_RAM (8 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2101            // part id which can be read via ISP/IAP
#elif defined LPC2104
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_LOCAL_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2104            // part id which can be read via ISP/IAP
#elif defined (LPC2105)
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2105            // part id which can be read via ISP/IAP
#elif defined (LPC2106)                                                  // (LPC2106)
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2106            // part id which can be read via ISP/IAP
#elif defined LPC2131                                                    // {30}
    #define SIZE_OF_FLASH (32*1024)                                      // 32k FLASH
    #define SIZE_OF_LOCAL_RAM (8 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2131            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC213X
    #define LPC21XX_LEGACY_PART
#elif defined LPC2132
    #define SIZE_OF_FLASH (64*1024)                                      // 64k FLASH
    #define SIZE_OF_LOCAL_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2132            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC213X
    #define LPC21XX_LEGACY_PART
#elif defined LPC2134
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_LOCAL_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2134            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC213X
    #define LPC21XX_LEGACY_PART
#elif defined LPC2136
    #define SIZE_OF_FLASH (256*1024)                                     // 256k FLASH
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2136            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC213X
    #define LPC21XX_LEGACY_PART
#elif defined LPC2138
    #define SIZE_OF_FLASH (512*1024)                                     // 512k FLASH
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2138            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC213X
    #define LPC21XX_LEGACY_PART
#elif defined LPC2141                                                    // {16}
    #define SIZE_OF_FLASH (32*1024)                                      // 32k FLASH
    #define SIZE_OF_LOCAL_RAM (8 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2141            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC214X
#elif defined LPC2142
    #define SIZE_OF_FLASH (64*1024)                                      // 64k FLASH
    #define SIZE_OF_LOCAL_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2142            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC214X
#elif defined LPC2144
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_LOCAL_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2144            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC214X
#elif defined LPC2146
    #define SIZE_OF_FLASH (256*1024)                                     // 256k FLASH
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2146            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC214X
#elif defined LPC2148
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // {18} 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2148            // part id which can be read via ISP/IAP
    #define DEVICE_64_PIN
    #define _LPC214X
#elif defined LPC2378FBD144
    #define DEVICE_144_PIN                                               // LPC2378 as opposed to LPC2364, LPC2366, LPC2368
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2378            // part id which can be read via ISP/IAP
#elif defined (LPC2387FBD100)
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2378            // part id which can be read via ISP/IAP
#elif defined (LPC2368FBD100)
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2368            // part id which can be read via ISP/IAP
#elif defined (LPC2366FBD100)
    #define SIZE_OF_FLASH (256*1024)                                     // 256k FLASH
    #define SIZE_OF_LOCAL_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2366            // part id which can be read via ISP/IAP
#elif defined LPC2364FBD100
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_LOCAL_RAM (8 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2364            // part id which can be read via ISP/IAP 
#elif defined (LPC2458FET180)
    #define DEVICE_180_PIN
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2458            // part id which can be read via ISP/IAP
#elif defined (LPC2468FET208)
    #define DEVICE_208_PIN
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2468            // part id which can be read via ISP/IAP
#elif defined (LPC2468FBD208)
    #define DEVICE_208_PIN
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2468            // part id which can be read via ISP/IAP
#elif defined (LPC2478FET208)
    #define DEVICE_208_PIN
    #define SIZE_OF_FLASH ((512 - 8)*1024)                               // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2478            // part id which can be read via ISP/IAP
#elif defined (LPC2478FBD208)
    #define DEVICE_208_PIN
    #define SIZE_OF_FLASH   ((512 - 8)*1024)                             // 512k FLASH (last 8k used by ISP)
    #define SIZE_OF_LOCAL_RAM (64 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2478            // part id which can be read via ISP/IAP
#endif

#define IAR_STACK_SIZE                       32                          // {35} top most 32 bytes of SRAM are reserved for IAP use

#define SIZE_OF_RAM                          SIZE_OF_LOCAL_RAM           // {38} for compatibility

#define RANDOM_SEED_LOCATION (unsigned short *)(RAM_START_ADDRESS + SIZE_OF_LOCAL_RAM - IAR_STACK_SIZE - 4) // {44} location of a long word which is never initialised and so has a random power on value
#define BOOT_MAIL_BOX        (unsigned short *)(RAM_START_ADDRESS + SIZE_OF_LOCAL_RAM - IAR_STACK_SIZE - 2) // {45}

#define PART_ID_LPC2101                      0x0004ff11
#define PART_ID_LPC2102                      0x0004ff11
#define PART_ID_LPC2103                      0x0004ff11
#define PART_ID_LPC2104                      0xfff0ff12
#define PART_ID_LPC2105                      0xfff0ff22
#define PART_ID_LPC2106                      0xfff0ff32
#define PART_ID_LPC2141                      0x0402ff01                  // {16}
#define PART_ID_LPC2142                      0x0402ff11
#define PART_ID_LPC2144                      0x0402ff12
#define PART_ID_LPC2146                      0x0402ff23
#define PART_ID_LPC2148                      0x0402ff25
#define PART_ID_LPC2361                      0x1600f701                  // {12}
#define PART_ID_LPC2362                      0x1600ff22
#define PART_ID_LPC2364                      0x1600f902
#define PART_ID_LPC2365                      0x1600e823
#define PART_ID_LPC2366                      0x1600f923
#define PART_ID_LPC2367                      0x1600e825
#define PART_ID_LPC2368                      0x1600f925
#define PART_ID_LPC2377                      0x1700e825
#define PART_ID_LPC2378                      0x1700fd25
#define PART_ID_LPC2387                      0x1700ff35                  // pre-date code 0840 = 0x1800f935
#define PART_ID_LPC2388                      0x1800ff35
#define PART_ID_LPC2458                      0x1500ff35
#define PART_ID_LPC2468                      0x1600ff35
#define PART_ID_LPC2478                      0x1701ff35


#if defined _LPC213X || defined _LPC214X                                 // {41}
    // USB requires that the APB clock be at least 18MHz. By default it is MCLK/4
    //
    #if defined USB_INTERFACE && (MASTER_CLOCK/4 < 18000000)
        #if MASTER_CLOCK/2 < 18000000
            #if MASTER_CLOCK < 18000000
                #error Clock is too slow for USB use!
            #else
                #define APBCLOCK_DIVIDE  APBCLOCK_1                      // APB clock equal to master clock
                #define APB_DIV     1
            #endif
        #else
            #define APBCLOCK_DIVIDE  APBCLOCK_2;                         // APB clock equal to half the master clock
            #define APB_DIV     2
        #endif
    #endif
    #if defined APB_DIV
        #define PCLK             (MASTER_CLOCK/APB_DIV)                  // adjusted PCLK
    #else
        #define PCLK             (MASTER_CLOCK/4)                        // default PCLK is one fourth of the processor clock
    #endif
    #define UART_CLOCK           (PCLK)                                  //all peripherals use PCLK
    #define SSP_CLOCK            (PCLK)
    #define IIC_CLOCK            (PCLK)
    #define ADC_CLOCK            (PCLK)
#else
    #define PCLK                 MASTER_CLOCK
    #define UART_CLOCK           (PCLK/4)
    #define SSP_CLOCK            (PCLK/4)
    #define IIC_CLOCK            (PCLK/8)
    #define ADC_CLOCK            (PCLK/8)
#endif


#if defined _WINDOWS
    #include "simLPC23XX.h"                                              // {40}
#endif

#if defined _LPC21XX && !defined _LPC214X                                // {16}
    #define INTERRUPT_COUNT  15
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        #define FLASH_START_ADDRESS          0                           // up to 32k
        #define START_OF_FLASH               FLASH_START_ADDRESS
        #define RAM_START_ADDRESS            0x40000000                  // up to 8k

        #define FLASH_GRANULARITY_SMALL      (4 * 1024)
        #define FLASH_GRANULARITY_LARGE      (4 * 1024)

        #define FLASH_SECTOR_0_OFFSET        0
        #define FLASH_SECTOR_SIZE_0          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_1_OFFSET        0x1000
        #define FLASH_SECTOR_SIZE_1          FLASH_GRANULARITY_LARGE     // last sector in LPC2101
        #define FLASH_SECTOR_2_OFFSET        0x2000
        #define FLASH_SECTOR_SIZE_2          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_3_OFFSET        0x3000
        #define FLASH_SECTOR_SIZE_3          FLASH_GRANULARITY_LARGE     // last sector in LPC2102
        #define FLASH_SECTOR_4_OFFSET        0x4000
        #define FLASH_SECTOR_SIZE_4          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_5_OFFSET        0x5000
        #define FLASH_SECTOR_SIZE_5          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_6_OFFSET        0x6000
        #define FLASH_SECTOR_SIZE_6          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_7_OFFSET        0x7000
        #define FLASH_SECTOR_SIZE_7          FLASH_GRANULARITY_LARGE     // last sector in LPC2103
    #else
        #define FLASH_START_ADDRESS          0                           // up to 128k
        #define START_OF_FLASH               FLASH_START_ADDRESS
        #define RAM_START_ADDRESS            0x40000000                  // up to 64k

        #define FLASH_GRANULARITY_SMALL      (8 * 1024)
        #define FLASH_GRANULARITY_LARGE      (8 * 1024)

        #define FLASH_SECTOR_0_OFFSET        0
        #define FLASH_SECTOR_SIZE_0          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_1_OFFSET        0x2000
        #define FLASH_SECTOR_SIZE_1          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_2_OFFSET        0x4000
        #define FLASH_SECTOR_SIZE_2          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_3_OFFSET        0x6000
        #define FLASH_SECTOR_SIZE_3          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_4_OFFSET        0x8000
        #define FLASH_SECTOR_SIZE_4          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_5_OFFSET        0xA000
        #define FLASH_SECTOR_SIZE_5          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_6_OFFSET        0xC000
        #define FLASH_SECTOR_SIZE_6          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_7_OFFSET        0xE000
        #define FLASH_SECTOR_SIZE_7          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_8_OFFSET        0x10000
        #define FLASH_SECTOR_SIZE_8          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_9_OFFSET        0x12000
        #define FLASH_SECTOR_SIZE_9          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_10_OFFSET       0x14000
        #define FLASH_SECTOR_SIZE_10         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_11_OFFSET       0x16000
        #define FLASH_SECTOR_SIZE_11         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_12_OFFSET       0x18000
        #define FLASH_SECTOR_SIZE_12         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_13_OFFSET       0x1A000
        #define FLASH_SECTOR_SIZE_13         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_14_OFFSET       0x1C000
        #define FLASH_SECTOR_SIZE_14         FLASH_GRANULARITY_LARGE     // last usable sector in 128k part
    #endif
#else
    #ifdef _LPC214X
        #define INTERRUPT_COUNT  15
    #else
        #define INTERRUPT_COUNT  31
    #endif
    #define FLASH_START_ADDRESS              0                           // up to 512k
    #define START_OF_FLASH                   FLASH_START_ADDRESS
    #define RAM_START_ADDRESS                0x40000000                  // up to 64k
    #define USB_RAM_START                    0x7fd00000                  // up to 16k
    #if defined _LPC214X && (defined LPC2146 || defined LPC2148)
        #define USB_RAM_SIZE                 (8 * 1024)
    #else
        #define ETHERNET_RAM_START_ADDRESS   0x7fe00000                  // 16k
        #define ETHERNET_RAM_SIZE            (16 * 1024)
        #if defined LPC2387FBD100
            #define USB_RAM_SIZE             (16 * 1024)
        #else
            #define USB_RAM_SIZE             (8 * 1024)
        #endif
    #endif

    #define FLASH_GRANULARITY_SMALL          (4 * 1024)                  // small blocks
    #define FLASH_GRANULARITY_LARGE          (32 * 1024)                 // large blocks

    #define FLASH_SECTOR_0_OFFSET            0
    #define FLASH_SECTOR_SIZE_0              FLASH_GRANULARITY_SMALL     // initially small blocks
    #define FLASH_SECTOR_1_OFFSET            0x1000
    #define FLASH_SECTOR_SIZE_1              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_2_OFFSET            0x2000
    #define FLASH_SECTOR_SIZE_2              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_3_OFFSET            0x3000
    #define FLASH_SECTOR_SIZE_3              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_4_OFFSET            0x4000
    #define FLASH_SECTOR_SIZE_4              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_5_OFFSET            0x5000
    #define FLASH_SECTOR_SIZE_5              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_6_OFFSET            0x6000
    #define FLASH_SECTOR_SIZE_6              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_7_OFFSET            0x7000
    #define FLASH_SECTOR_SIZE_7              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_8_OFFSET            0x8000                      // end large blocks

    #define FLASH_SECTOR_SIZE_8              FLASH_GRANULARITY_LARGE     // last sector in 64k part
    #define FLASH_SECTOR_9_OFFSET            0x10000
    #define FLASH_SECTOR_SIZE_9              FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_10_OFFSET           0x18000
    #define FLASH_SECTOR_SIZE_10             FLASH_GRANULARITY_LARGE     // last sector in 128k part
    #define FLASH_SECTOR_11_OFFSET           0x20000
    #define FLASH_SECTOR_SIZE_11             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_12_OFFSET           0x28000
    #define FLASH_SECTOR_SIZE_12             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_13_OFFSET           0x30000
    #define FLASH_SECTOR_SIZE_13             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_14_OFFSET           0x38000
    #define FLASH_SECTOR_SIZE_14             FLASH_GRANULARITY_LARGE     // last sector in 256k part
    #define FLASH_SECTOR_15_OFFSET           0x40000
    #define FLASH_SECTOR_SIZE_15             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_16_OFFSET           0x48000
    #define FLASH_SECTOR_SIZE_16             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_17_OFFSET           0x50000
    #define FLASH_SECTOR_SIZE_17             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_18_OFFSET           0x58000
    #define FLASH_SECTOR_SIZE_18             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_19_OFFSET           0x60000
    #define FLASH_SECTOR_SIZE_19             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_20_OFFSET           0x68000
    #define FLASH_SECTOR_SIZE_20             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_21_OFFSET           0x70000
    #define FLASH_SECTOR_SIZE_21             FLASH_GRANULARITY_LARGE

    #define FLASH_SECTOR_22_OFFSET           0x78000                     // further small blocks
    #define FLASH_SECTOR_SIZE_22             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_23_OFFSET           0x79000
    #define FLASH_SECTOR_SIZE_23             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_24_OFFSET           0x7a000
    #define FLASH_SECTOR_SIZE_24             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_25_OFFSET           0x7b000
    #define FLASH_SECTOR_SIZE_25             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_26_OFFSET           0x7c000
    #define FLASH_SECTOR_SIZE_26             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_27_OFFSET           0x7d000
    #define FLASH_SECTOR_SIZE_27             FLASH_GRANULARITY_SMALL     // last sector in 512k part - last 8k are occupied by ISP routines
#endif

#define FLASH_LINE_SIZE                      16                          // lines made up of quad words (128 bits)
#define FLASH_ROW_SIZE                       256                         // FLASH writes are performed in rows of this size

#define SDRAM_ADDR                           0xa0000000

typedef void (*IAP)(unsigned long [], unsigned long []);
#ifdef _WINDOWS
    extern void fnSimNXPFlash(unsigned long [], unsigned long []);
    #define IAP_LOCATION fnSimNXPFlash
#else
    #define IAP_LOCATION (IAP)0x7ffffff1                                 // Thumb mode execution address of In-Application Programming routine
#endif

//
//      Status Code                            Mnemonic                  Description
//
#define CMD_SUCCESS                                0                     // Command is executed successfully.
#define INVALID_COMMAND                            1                     // Invalid command.
#define SRC_ADDR_ERROR                             2                     // Source address is not on a word boundary.
#define DST_ADDR_ERROR                             3                     // Destination address is not on a correct boundary.
#define SRC_ADDR_NOT_MAPPED                        4                     // Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.
#define DST_ADDR_NOT_MAPPED                        5                     // Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.
#define COUNT_ERROR                                6                     // Byte count is not multiple of 4 or is not a permitted value.
#define INVALID_SECTOR                             7                     // Sector number is invalid.
#define SECTOR_NOT_BLANK                           8                     // Sector is not blank.
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION    9                     // Command to prepare sector for write operation was not executed.
#define COMPARE_ERROR                              10                    // Source and destination data is not same.
#define BUSY                                       11                    // Flash programming hardware interface is busy.
#define PARAM_ERROR                                12                    // Insufficient number of parameters or invalid parameter.
#define ADDR_ERROR                                 13                    // Address is not on word boundary.
#define ADDR_NOT_MAPPED                            14                    // Address is not mapped in the memory map. Count value is taken into condiferation where applicable.
#define CMD_LOCKED                                 15                    // Command is locked.
#define INVALID_CODE                               16                    // Unlock code is invalid.
#define INVALID_BAUDRATE                           17                    // Invalid baud rate setting.
#define INVALID_STOP_BIT                           18                    // Invalid stop bit setting.
#define CODE_READ_PROTECTION_ENABLED               19                    // Code read protection enabled.


//      IAP Command                            Command Code              Status Code(s)
//
#define FLASH_PREPARE_SECTOR_TO_WRITE              50                    // CMD_SUCCESS | BUSY | INVALID_SECTOR
#define FLASH_COPY_RAM_TO_FLASH                    51                    // CMD_SUCCESS |SRC_ADDR_ERROR (Address not on word boundary) |
                                                                         // DST_ADDR_ERROR (Address not on correct boundary) | SRC_ADDR_NOT_MAPPED |
                                                                         // DST_ADDR_NOT_MAPPED | COUNT_ERROR (Byte count is not 512 | 1024 | 4096 | 8192) |
                                                                         // SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION | BUSY
#define FLASH_ERASE_SECTORS                        52                    // CMD_SUCCESS | BUSY | SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION | INVALID_SECTOR
#define FLASH_BLANK_CHECK_SECTORS                  53                    // CMD_SUCCESS |BUSY | SECTOR_NOT_BLANK | INVALID_SECTOR
#define FLASH_READ_PART_ID                         54                    // CMD_SUCCESS
#define FLASH_READ_BOOT_CODE_VERSION               55                    // CMD_SUCCESS
#define FLASH_COMPARE                              56                    // CMD_SUCCESS | COMPARE_ERROR | COUNT_ERROR (Byte count is not multiple of 4) |
                                                                         // ADDR_ERROR | ADDR_NOT_MAPPED
#define FLASH_REVOKE_ISP                           57


// Peripheral Register Blocks
//
#ifdef _WINDOWS
  #if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {9} these do not have fast IO so use only legacy ports
      #define FAST_GPIO_BLOCK            ((unsigned char *)(&ucLPC23xx.ucFastGPIO))
  #endif
  #define WATCHDOG_BLOCK                 ((unsigned char *)(&ucLPC23xx.ucWDOG))
  #define TIMER_PERIPHERAL_BLOCK_0       ((unsigned char *)(&ucLPC23xx.ucSimTimer0))
  #define TIMER_PERIPHERAL_BLOCK_1       ((unsigned char *)(&ucLPC23xx.ucSimTimer1))
  #define UART_BLOCK_0                   ((unsigned char *)(&ucLPC23xx.ucSimUart0))
  #define UART_BLOCK_1                   ((unsigned char *)(&ucLPC23xx.ucSimUart1))
  #define PWM_BLOCK_0                    ((unsigned char *)(&ucLPC23xx.ucPWM0))
  #define PWM_BLOCK_1                    ((unsigned char *)(&ucLPC23xx.ucPWM1))
  #define I2C_BLOCK_0                    ((unsigned char *)(&ucLPC23xx.ucIIC0))
  #define SPI_BLOCK_0                    ((unsigned char *)(&ucLPC23xx.ucSPI0)) // {28}
  #define RTC_BLOCK                      ((unsigned char *)(&ucLPC23xx.ucRTC))
  #define LEGACY_GPIO_BLOCK              ((unsigned char *)(&ucLPC23xx.ucLegacyGPIO))
  #define PIN_CONNECT_BLOCK              ((unsigned char *)(&ucLPC23xx.ucPIN_CON))
  #define SSP_BLOCK_1                    ((unsigned char *)(&ucLPC23xx.ucSSP1))
  #define ADC_BLOCK                      ((unsigned char *)(&ucLPC23xx.ucADC))
  #define CAN_AFRAM                      ((unsigned char *)(&ucLPC23xx.ucCAN_AFRAM)) // CAN acceptance filer RAM {31}
  #define CAN_AF_BLOCK                   ((unsigned char *)(&ucLPC23xx.ucCAN_AF)) // CAN acceptance filer registers
  #define CAN_BLOCK                      ((unsigned char *)(&ucLPC23xx.ucCAN)) // central CAN registers
  #define CAN1_BLOCK                     ((unsigned char *)(&ucLPC23xx.ucCAN1)) // CAN1 control registers
  #define CAN2_BLOCK                     ((unsigned char *)(&ucLPC23xx.ucCAN2)) // CAN2 control registers
  #define I2C_BLOCK_1                    ((unsigned char *)(&ucLPC23xx.ucIIC1))
  #define SSP_BLOCK_0                    ((unsigned char *)(&ucLPC23xx.ucSSP0))
  #define TIMER_PERIPHERAL_BLOCK_2       ((unsigned char *)(&ucLPC23xx.ucSimTimer2)) // not LPC2148
  #define TIMER_PERIPHERAL_BLOCK_3       ((unsigned char *)(&ucLPC23xx.ucSimTimer3)) // not LPC2148
  #define UART_BLOCK_2                   ((unsigned char *)(&ucLPC23xx.ucSimUart2))
  #define UART_BLOCK_3                   ((unsigned char *)(&ucLPC23xx.ucSimUart3))
  #define I2C_BLOCK_2                    ((unsigned char *)(&ucLPC23xx.ucIIC2))
  #define RTC_RAM_BLOCK                  ((unsigned long *)(&ulLPC23xx.ulRTC_ram)) // 2k battery backed up SRAM - only long word accesses supported
  #define MCI_BLOCK                      ((unsigned char *)(&ucLPC23xx.ulMCI)) // SD/MMC interface
  #if defined _LPC24XX || defined LPC2378FBD144
      #define USB_CONTROLLER_BLOCK_LPC2378 ((unsigned char *)(&ucLPC23xx.ulLPC2378USB)) 
  #endif
  #define USB_CONTROLLER_BLOCK           ((unsigned char *)(&ucLPC23xx.ulUSB)) // USB device
  #define SYSTEM_CONTROL_MODULE          ((unsigned char *)(&ucLPC23xx.ucSCM))

  #define ETHERNET_CONTROLLER_BLOCK      ((unsigned char *)(&ucLPC23xx.ucEMAC))
  #define DMA_CONTROLLER_BLOCK           ((unsigned char *)(&ucLPC23xx.ucDMA))
  #define EXT_MEMORY_CONTROLLER_BLOCK    ((unsigned char *)(&ucLPC23xx.ucEMC))
  #define LCD_CONTROLLER_BLOCK           ((unsigned char *)(&ucLPC23xx.ucLCD))

  #define VIC_PERIPHERAL_BLOCK           ((unsigned char *)(&ucLPC23xx.ucVIC))
  #define VIC_ADDRESS_BLOCK              ((unsigned char *)(&ucLPC23xx.ucVICAddress))
#else
    #if (!defined LPC2104 && !defined LPC2105 && !defined LPC2106 && !defined _LPC213X) || !defined LPC21XX_LEGACY_PART // {9} these do not have fast IO so use only legacy ports
  #define FAST_GPIO_BLOCK                0x3fffc000
    #endif
  #define WATCHDOG_BLOCK                 0xe0000000
  #define TIMER_PERIPHERAL_BLOCK_0       0xe0004000
  #define TIMER_PERIPHERAL_BLOCK_1       0xe0008000
  #define UART_BLOCK_0                   0xe000c000
  #define UART_BLOCK_1                   0xe0010000
  #define PWM_BLOCK_0                    0xe0014000                      // LPC24XX {24}
  #define PWM_BLOCK_1                    0xe0018000                      // LPC23XX/LPC24XX {24}
  #define I2C_BLOCK_0                    0xe001c000
  #define SPI_BLOCK_0                    0xe0020000                      // {28}
  #define RTC_BLOCK                      0xe0024000
  #define LEGACY_GPIO_BLOCK              0xe0028000
  #define PIN_CONNECT_BLOCK              0xe002c000
  #define SSP_BLOCK_1                    0xe0030000
  #define ADC_BLOCK                      0xe0034000
  #define CAN_AFRAM                      0xe0038000                      // CAN acceptance filer RAM {31}
  #define CAN_AF_BLOCK                   0xe003c000                      // CAN acceptance filer registers
  #define CAN_BLOCK                      0xe0040000                      // central CAN registers
  #define CAN1_BLOCK                     0xe0044000                      // CAN1 control registers
  #define CAN2_BLOCK                     0xe0048000                      // CAN2 control registers
  #define I2C_BLOCK_1                    0xe005c000
  #define SSP_BLOCK_0                    0xe0068000
  #define TIMER_PERIPHERAL_BLOCK_2       0xe0070000                      // not LPC2148
  #define TIMER_PERIPHERAL_BLOCK_3       0xe0074000                      // not LPC2148
  #define UART_BLOCK_2                   0xe0078000
  #define UART_BLOCK_3                   0xe007c000
  #define I2C_BLOCK_2                    0xe0080000
  #define RTC_RAM_BLOCK                  0xe0084000                      // 2k battery backed up SRAM - only long word accesses supported
  #define MCI_BLOCK                      0xe008c000                      // SD/MMC interface
  #define SYSTEM_CONTROL_MODULE          0xe01fc000

  #define ETHERNET_CONTROLLER_BLOCK      0xffe00000
  #define DMA_CONTROLLER_BLOCK           0xffe04000
  #define EXT_MEMORY_CONTROLLER_BLOCK    0xffe08000
  #ifdef _LPC214X
    #define USB_CONTROLLER_BLOCK         0xe0090000
  #else
    #if defined _LPC24XX || defined LPC2378FBD144
        #define USB_CONTROLLER_BLOCK_LPC2378 0xffe0c110
    #endif
    #define USB_CONTROLLER_BLOCK         0xffe0c200
  #endif
  #define LCD_CONTROLLER_BLOCK           0xffe10000

  #define VIC_PERIPHERAL_BLOCK           0xfffff000
  #ifdef _LPC21XX
    #define VIC_ADDRESS_BLOCK            0xfffff030                      // {17}
  #else
    #define VIC_ADDRESS_BLOCK            0xffffff00
  #endif
#endif

#ifdef _LPC21XX
    #define UARTS_AVAILABLE              2
#else
    #define UARTS_AVAILABLE              4
#endif

// FAST GPIOs
//
#define FIO0DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x00)    // Port Direction Control Register - port 0
#define FIO0MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x10)    // {13} Fast Mask Register - port 0
#define FIO0PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x14)    // Fast Port Pin Register - port 0
#define FIO0SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x18)    // Fast Port Output Set Register - port 0
#define FIO0CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x1c)    // Fast Port Output Clear Register - port 0 (Write only)

#define FIO1DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x20)    // Port Direction Control Register - port 1
#define FIO1MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x30)    // {13} Fast Mask Register - port 1
#define FIO1PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x34)    // Fast Port Pin Register - port 1
#define FIO1SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x38)    // Fast Port Output Set Register - port 1
#define FIO1CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x3c)    // Fast Port Output Clear Register - port 1 (Write only)

#define FIO2DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x40)    // Port Direction Control Register - port 2
#define FIO2MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x50)    // {13} Fast Mask Register - port 2
#define FIO2PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x54)    // Fast Port Pin Register - port 2
#define FIO2SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x58)    // Fast Port Output Set Register - port 2
#define FIO2CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x5c)    // Fast Port Output Clear Register - port 2 (Write only)

#define FIO3DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x60)    // Port Direction Control Register - port 3
#define FIO3MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x70)    // {13} Fast Mask Register - port 3
#define FIO3PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x74)    // Fast Port Pin Register - port 3
#define FIO3SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x78)    // Fast Port Output Set Register - port 3
#define FIO3CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x7c)    // Fast Port Output Clear Register - port 3 (Write only)

#define FIO4DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x80)    // Port Direction Control Register - port 4
#define FIO4MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x90)    // {13} Fast Mask Register - port 4
#define FIO4PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x94)    // Fast Port Pin Register - port 4
#define FIO4SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x98)    // Fast Port Output Set Register - port 4
#define FIO4CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x9c)    // Fast Port Output Clear Register - port 4 (Write only)

  #define PORT0_BIT0                     0x00000001
  #define PORT0_BIT1                     0x00000002
  #define PORT0_BIT2                     0x00000004
  #define PORT0_BIT3                     0x00000008
  #define PORT0_BIT4                     0x00000010
  #define PORT0_BIT5                     0x00000020
  #define PORT0_BIT6                     0x00000040
  #define PORT0_BIT7                     0x00000080
  #define PORT0_BIT8                     0x00000100
  #define PORT0_BIT9                     0x00000200
  #define PORT0_BIT10                    0x00000400
  #define PORT0_BIT11                    0x00000800
  #define PORT0_BIT12                    0x00001000
  #define PORT0_BIT13                    0x00002000
  #define PORT0_BIT14                    0x00004000
  #define PORT0_BIT15                    0x00008000
  #define PORT0_BIT16                    0x00010000
  #define PORT0_BIT17                    0x00020000
  #define PORT0_BIT18                    0x00040000
  #define PORT0_BIT19                    0x00080000
  #define PORT0_BIT20                    0x00100000
  #define PORT0_BIT21                    0x00200000
  #define PORT0_BIT22                    0x00400000
  #define PORT0_BIT23                    0x00800000
  #define PORT0_BIT24                    0x01000000
  #define PORT0_BIT25                    0x02000000
  #define PORT0_BIT26                    0x04000000
  #define PORT0_BIT27                    0x08000000
  #define PORT0_BIT28                    0x10000000
  #define PORT0_BIT29                    0x20000000
  #define PORT0_BIT30                    0x40000000
  #define PORT0_BIT31                    0x80000000

  #define PORT1_BIT0                     0x00000001
  #define PORT1_BIT1                     0x00000002
  #define PORT1_BIT2                     0x00000004
  #define PORT1_BIT3                     0x00000008
  #define PORT1_BIT4                     0x00000010
  #define PORT1_BIT5                     0x00000020
  #define PORT1_BIT6                     0x00000040
  #define PORT1_BIT7                     0x00000080
  #define PORT1_BIT8                     0x00000100
  #define PORT1_BIT9                     0x00000200
  #define PORT1_BIT10                    0x00000400
  #define PORT1_BIT11                    0x00000800
  #define PORT1_BIT12                    0x00001000
  #define PORT1_BIT13                    0x00002000
  #define PORT1_BIT14                    0x00004000
  #define PORT1_BIT15                    0x00008000
  #define PORT1_BIT16                    0x00010000
  #define PORT1_BIT17                    0x00020000
  #define PORT1_BIT18                    0x00040000
  #define PORT1_BIT19                    0x00080000
  #define PORT1_BIT20                    0x00100000
  #define PORT1_BIT21                    0x00200000
  #define PORT1_BIT22                    0x00400000
  #define PORT1_BIT23                    0x00800000
  #define PORT1_BIT24                    0x01000000
  #define PORT1_BIT25                    0x02000000
  #define PORT1_BIT26                    0x04000000
  #define PORT1_BIT27                    0x08000000
  #define PORT1_BIT28                    0x10000000
  #define PORT1_BIT29                    0x20000000
  #define PORT1_BIT30                    0x40000000
  #define PORT1_BIT31                    0x80000000

  #define PORT2_BIT0                     0x00000001
  #define PORT2_BIT1                     0x00000002
  #define PORT2_BIT2                     0x00000004
  #define PORT2_BIT3                     0x00000008
  #define PORT2_BIT4                     0x00000010
  #define PORT2_BIT5                     0x00000020
  #define PORT2_BIT6                     0x00000040
  #define PORT2_BIT7                     0x00000080
  #define PORT2_BIT8                     0x00000100
  #define PORT2_BIT9                     0x00000200
  #define PORT2_BIT10                    0x00000400
  #define PORT2_BIT11                    0x00000800
  #define PORT2_BIT12                    0x00001000
  #define PORT2_BIT13                    0x00002000
  #define PORT2_BIT14                    0x00004000
  #define PORT2_BIT15                    0x00008000
  #define PORT2_BIT16                    0x00010000
  #define PORT2_BIT17                    0x00020000
  #define PORT2_BIT18                    0x00040000
  #define PORT2_BIT19                    0x00080000
  #define PORT2_BIT20                    0x00100000
  #define PORT2_BIT21                    0x00200000
  #define PORT2_BIT22                    0x00400000
  #define PORT2_BIT23                    0x00800000
  #define PORT2_BIT24                    0x01000000
  #define PORT2_BIT25                    0x02000000
  #define PORT2_BIT26                    0x04000000
  #define PORT2_BIT27                    0x08000000
  #define PORT2_BIT28                    0x10000000
  #define PORT2_BIT29                    0x20000000
  #define PORT2_BIT30                    0x40000000
  #define PORT2_BIT31                    0x80000000

  #define PORT3_BIT0                     0x00000001
  #define PORT3_BIT1                     0x00000002
  #define PORT3_BIT2                     0x00000004
  #define PORT3_BIT3                     0x00000008
  #define PORT3_BIT4                     0x00000010
  #define PORT3_BIT5                     0x00000020
  #define PORT3_BIT6                     0x00000040
  #define PORT3_BIT7                     0x00000080
  #define PORT3_BIT8                     0x00000100
  #define PORT3_BIT9                     0x00000200
  #define PORT3_BIT10                    0x00000400
  #define PORT3_BIT11                    0x00000800
  #define PORT3_BIT12                    0x00001000
  #define PORT3_BIT13                    0x00002000
  #define PORT3_BIT14                    0x00004000
  #define PORT3_BIT15                    0x00008000
  #define PORT3_BIT16                    0x00010000
  #define PORT3_BIT17                    0x00020000
  #define PORT3_BIT18                    0x00040000
  #define PORT3_BIT19                    0x00080000
  #define PORT3_BIT20                    0x00100000
  #define PORT3_BIT21                    0x00200000
  #define PORT3_BIT22                    0x00400000
  #define PORT3_BIT23                    0x00800000
  #define PORT3_BIT24                    0x01000000
  #define PORT3_BIT25                    0x02000000
  #define PORT3_BIT26                    0x04000000
  #define PORT3_BIT27                    0x08000000
  #define PORT3_BIT28                    0x10000000
  #define PORT3_BIT29                    0x20000000
  #define PORT3_BIT30                    0x40000000
  #define PORT3_BIT31                    0x80000000

  #define PORT4_BIT0                     0x00000001
  #define PORT4_BIT1                     0x00000002
  #define PORT4_BIT2                     0x00000004
  #define PORT4_BIT3                     0x00000008
  #define PORT4_BIT4                     0x00000010
  #define PORT4_BIT5                     0x00000020
  #define PORT4_BIT6                     0x00000040
  #define PORT4_BIT7                     0x00000080
  #define PORT4_BIT8                     0x00000100
  #define PORT4_BIT9                     0x00000200
  #define PORT4_BIT10                    0x00000400
  #define PORT4_BIT11                    0x00000800
  #define PORT4_BIT12                    0x00001000
  #define PORT4_BIT13                    0x00002000
  #define PORT4_BIT14                    0x00004000
  #define PORT4_BIT15                    0x00008000
  #define PORT4_BIT16                    0x00010000
  #define PORT4_BIT17                    0x00020000
  #define PORT4_BIT18                    0x00040000
  #define PORT4_BIT19                    0x00080000
  #define PORT4_BIT20                    0x00100000
  #define PORT4_BIT21                    0x00200000
  #define PORT4_BIT22                    0x00400000
  #define PORT4_BIT23                    0x00800000
  #define PORT4_BIT24                    0x01000000
  #define PORT4_BIT25                    0x02000000
  #define PORT4_BIT26                    0x04000000
  #define PORT4_BIT27                    0x08000000
  #define PORT4_BIT28                    0x10000000
  #define PORT4_BIT29                    0x20000000
  #define PORT4_BIT30                    0x40000000
  #define PORT4_BIT31                    0x80000000



// Port macros
//
#ifdef FAST_GPIO_BLOCK
    // Configure pins as output eg. _CONFIG_PORT_OUTPUT(0, PORT0_BIT16);
    //
    #define _CONFIG_PORT_OUTPUT(ref, pins)       FIO##ref##DIR |= (pins); _SIM_PORT_CHANGE
    // Configure pins as input  eg. _CONFIG_PORT_INPUT(1, PORT1_BIT4);
    //
    #define _CONFIG_PORT_INPUT(ref, pins)        FIO##ref##DIR &= ~(pins); _SIM_PORT_CHANGE
    // Configure a peripheral function
    //
    //#define _CONFIG_PERIPHERAL(ref, periph, peripheral_block, pins, functions) - not supported
    #ifdef _WINDOWS
        // Write full port width                                         {10}
        //
        #define _WRITE_PORT(ref, value)             FIO##ref##PIN = ((FIO##ref##PIN & ~(FIO##ref##DIR)) | ((value) & (FIO##ref##DIR))); FIO##ref##SET = (value); _SIM_PORT_CHANGE
        // Write to a port with a mask                                   {10}
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  FIO##ref##PIN = ((FIO##ref##PIN & ~((mask) & FIO##ref##DIR)) | ((value) & (mask & FIO##ref##DIR))); FIO##ref##SET &= ~(mask); FIO##ref##SET |= ((mask) & (value)); FIO##ref##CLR = ~FIO##ref##SET;  _SIM_PORT_CHANGE
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             FIO##ref##SET = (~(FIO##ref##PIN) & (mask)); FIO##ref##CLR = (FIO##ref##PIN & (mask)); _SIM_PORT_CHANGE
    #else
        // Write full port width
        //
      //#define _WRITE_PORT(ref, value)             FIO##ref##MASK = 0xffffffff; FIO##ref##PIN = (value); FIO##ref##MASK = 0 {29} it is assumed that masks are never left set
        #define _WRITE_PORT(ref, value)             FIO##ref##PIN = (value)
        // Write to a port with a mask
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  FIO##ref##MASK = ~(mask); FIO##ref##PIN = (value); FIO##ref##MASK = 0
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             FIO##ref##PIN ^= (mask);
    #endif
    // Read full port width
    //
    #define _READ_PORT(ref)                    (FIO##ref##PIN)
    // Read from a port with a mask
    //
    #define _READ_PORT_MASK(ref, mask)         (FIO##ref##PIN & (mask))
    #ifdef _WINDOWS
        // Configure outputs and then set a value to them - this device sets the value and then drives
        //
        #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) FIO##ref##DIR |= (pins); _SIM_PORT_CHANGE; FIO##ref##SET = ((pins) & (value)); FIO##ref##CLR = ((pins) & ~(value)); _SIM_PORT_CHANGE
        // Set from inputs to outputs and set a value to them - this is equivalent to _CONFIG_DRIVE_PORT_OUTPUT_VALUE on this device
        //
        #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value)        FIO##ref##DIR |= (pins); _SIM_PORT_CHANGE; FIO##ref##SET = ((pins) & (value)); FIO##ref##CLR = ((pins) & ~(value)); _SIM_PORT_CHANGE
    #else
        // Configure outputs and then set a value to them - this device sets the value and then drives
        //
        #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) FIO##ref##MASK = ~(pins); FIO##ref##PIN = (value); FIO##ref##MASK = 0; FIO##ref##DIR |= (pins)
        // Set from inputs to outputs and set a value to them - this is equivalent to _CONFIG_DRIVE_PORT_OUTPUT_VALUE on this device
        //
        #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value)        FIO##ref##MASK = ~(pins); FIO##ref##PIN = (value); FIO##ref##MASK = 0; FIO##ref##DIR |= (pins)
    #endif
    // Set from outputs to inputs
    //
    #define _FLOAT_PORT(ref, pins)              FIO##ref##DIR &= ~(pins); _SIM_PORT_CHANGE
    // Set from inputs to outputs 
    //
    #define _DRIVE_PORT_OUTPUT(ref, pins)       FIO##ref##DIR |= (pins); _SIM_PORT_CHANGE
    // Set and clear individual bits of a port
    //
    #define _SETBITS(ref, mask)                 FIO##ref##SET = (mask); _SIM_PORT_CHANGE 
    #define _CLEARBITS(ref, mask)               FIO##ref##CLR = (mask); _SIM_PORT_CHANGE
#else                                                                    // {32}
    // Configure pins as output eg. _CONFIG_PORT_OUTPUT(0, PORT0_BIT16);
    //
    #define _CONFIG_PORT_OUTPUT(ref, pins)       IO##ref##DIR |= (pins); _SIM_PORT_CHANGE
    // Configure pins as input  eg. _CONFIG_PORT_INPUT(1, PORT1_BIT4);
    //
    #define _CONFIG_PORT_INPUT(ref, pins)        IO##ref##DIR &= ~(pins); _SIM_PORT_CHANGE
    // Configure a peripheral function
    //
    //#define _CONFIG_PERIPHERAL(ref, periph, peripheral_block, pins, functions) - not supported
    #ifdef _WINDOWS
        // Write full port width                                         {10}
        //
        #define _WRITE_PORT(ref, value)             IO##ref##PIN = ((IO##ref##PIN & ~(IO##ref##DIR)) | ((value) & (IO##ref##DIR))); IO##ref##SET = (value);  _SIM_PORT_CHANGE
        // Write to a port with a mask                                   {10}
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  IO##ref##PIN = ((IO##ref##PIN & ~(mask & IO##ref##DIR)) | ((value) & ((mask) & IO##ref##DIR)));  IO##ref##SET &= ~(mask); IO##ref##SET |= ((mask) & (value)); IO##ref##CLR = ~IO##ref##SET; _SIM_PORT_CHANGE
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             IO##ref##SET = (~(IO##ref##PIN) & (mask)); IO##ref##CLR = (IO##ref##PIN & (mask)); _SIM_PORT_CHANGE
    #else
        // Write full port width
        //
        #define _WRITE_PORT(ref, value)             IO##ref##PIN = (value)
        // Write to a port with a mask
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  IO##ref##PIN = ((IO##ref##SET & ~(mask & IO##ref##DIR)) | ((value) & (mask & IO##ref##DIR)));  _SIM_PORT_CHANGE // {33}
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             IO##ref##PIN = (IO##ref##SET ^ (mask)); _SIM_PORT_CHANGE // {33}
    #endif
    // Read full port width
    //
    #define _READ_PORT(ref)                    (IO##ref##PIN)
    // Read from a port with a mask
    //
    #define _READ_PORT_MASK(ref, mask)         (IO##ref##PIN & (mask))
    #ifdef _WINDOWS
        // Configure outputs and then set a value to them - this device sets the value and then drives
        //
        #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) IO##ref##DIR |= (pins); _SIM_PORT_CHANGE; IO##ref##SET = ((pins) & (value)); IO##ref##CLR = ((pins) & ~(value)); _SIM_PORT_CHANGE
        // Set from inputs to outputs and set a value to them - this is equivalent to _CONFIG_DRIVE_PORT_OUTPUT_VALUE on this device
        //
        #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value)        IO##ref##DIR |= (pins); _SIM_PORT_CHANGE; IO##ref##SET = ((pins) & (value)); IO##ref##CLR = ((pins) & ~(value)); _SIM_PORT_CHANGE
    #else
        // Configure outputs and then set a value to them - this device sets the value and then drives
        //
        #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) IO##ref##PIN = ((IO##ref##PIN & ~(pins)) | ((value) & (pins))); IO##ref##DIR |= (pins)
        // Set from inputs to outputs and set a value to them - this is equivalent to _CONFIG_DRIVE_PORT_OUTPUT_VALUE on this device
        //
        #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value)        IO##ref##PIN = ((IO##ref##PIN & ~(pins)) | ((value) & (pins))); IO##ref##DIR |= (pins)
    #endif
    // Set from outputs to inputs
    //
    #define _FLOAT_PORT(ref, pins)              IO##ref##DIR &= ~(pins); _SIM_PORT_CHANGE
    // Set from inputs to outputs 
    //
    #define _DRIVE_PORT_OUTPUT(ref, pins)       IO##ref##DIR |= (pins); _SIM_PORT_CHANGE
    // Set and clear individual bits of a port
    //
    #define _SETBITS(ref, mask)                 IO##ref##SET = (mask); _SIM_PORT_CHANGE
    #define _CLEARBITS(ref, mask)               IO##ref##CLR = (mask); _SIM_PORT_CHANGE
#endif


// Watchdog
//
#define WDMOD                            *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x00) // Watchdog Mode Register
  #define WDEN                           0x00000001                               // Watchdog enable
  #define WDRESET                        0x00000002                               // Watchdog reset enable
  #define WDTOF                          0x00000004                               // Watchdog timeout flag
  #define WDINT                          0x00000008                               // Watchdog Interrupt flag (Read Only)
#define WDTC                             *(unsigned long*)(WATCHDOG_BLOCK + 0x04) // Watchdog Timer Constant Register
#define WDFEED                           *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x08) // Watchdog Feed Sequence Register (Write Only)
#define WDTV                             *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x0c) // Watchdog Timer Value Register (Read Only)
#define WDCLKSEL                         *(unsigned long*)(WATCHDOG_BLOCK + 0x10) // Watchdog Clock Source Select Register
  #define WD_CLOCK_INT_RC                0x00000000                               // clock watchdog from internal RC clock
  #define WD_CLOCK_PCLK                  0x00000001                               // clock watchdog from peripheral clock
  #define WD_CLOCK_RTC                   0x00000002                               // clock watchdog from RTC clock


// Timers 0..3
//
#define T0IR                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x00) // Interrupt Register (write bit with 1 to clear)
  #define MR0_INTERRUPT                  0x00000001
  #define MR1_INTERRUPT                  0x00000002
  #define MR2_INTERRUPT                  0x00000004
  #define MR3_INTERRUPT                  0x00000008
  #define CR0_INTERRUPT                  0x00000010
  #define CR1_INTERRUPT                  0x00000020
  #define CR2_INTERRUPT                  0x00000040
  #define CR3_INTERRUPT                  0x00000080
#define T0TCR                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x04) // Timer Control Register
  #define COUNTER_ENABLE                 0x00000001
  #define COUNTER_RESET                  0x00000002
#define T0TC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x08) // Timer Counter
#define T0PR                             *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x0c) // Prescale Register
#define T0PC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x10) // Prescale Counter
#define T0MCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x14) // Match Control Register
  #define MR0I                           0x00000001                                         // Interrupt on MR0 match
  #define MR0R                           0x00000002                                         // Reset on MR0 match
  #define MR0S                           0x00000004                                         // Stop on MR0 match
  #define MR1I                           0x00000008                                         // Interrupt on MR1 match
  #define MR1R                           0x00000010                                         // Reset on MR1 match
  #define MR1S                           0x00000020                                         // Stop on MR1 match
  #define MR2I                           0x00000040                                         // Interrupt on MR2 match
  #define MR2R                           0x00000080                                         // Reset on MR2 match
  #define MR2S                           0x00000100                                         // Stop on MR2 match
  #define MR3I                           0x00000200                                         // Interrupt on MR3 match
  #define MR3R                           0x00000400                                         // Reset on MR3 match
  #define MR3S                           0x00000800                                         // Stop on MR3 match
#define T0MR0                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x18) // Match Register 0
#define T0MR1                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x1c) // Match Register 1
#define T0MR2                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x20) // Match Register 2
#define T0MR3                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x24) // Match Register 3
#define T0CCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x28) // Capture Control Register
  #define CAP0RE                         0x00000001                                         // Capture on Rising edge CAPn.0
  #define CAP0FE                         0x00000002                                         // Capture on Falling edge CAPn.0
  #define CAP0I                          0x00000004                                         // Interrupt on capture event CAPn.0
  #define CAP1RE                         0x00000008                                         // Capture on Rising edge CAPn.1
  #define CAP1FE                         0x00000010                                         // Capture on Falling edge CAPn.1
  #define CAP1I                          0x00000020                                         // Interrupt on capture event CAPn.1
  #define CAP2RE                         0x00000040                                         // Capture on Rising edge CAPn.2
  #define CAP2FE                         0x00000080                                         // Capture on Falling edge CAPn.2
  #define CAP2I                          0x00000100                                         // Interrupt on capture event CAPn.2
  #define CAP3RE                         0x00000200                                         // Capture on Rising edge CAPn.3
  #define CAP3FE                         0x00000400                                         // Capture on Falling edge CAPn.3
  #define CAP3I                          0x00000800                                         // Interrupt on capture event CAPn.3
#define T0CR0                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x2c) // Capture Register 0 (Read Only)
#define T0CR1                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x30) // Capture Register 1 (Read Only)
#define T0CR2                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x34) // Capture Register 2 (Read Only)
#define T0CR3                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x38) // Capture Register 3 (Read Only)
#define T0EMR                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x3c) // External Match Register
  #define EM0                            0x00000001                                         // External Match 0
  #define EM1                            0x00000002                                         // External Match 1
  #define EM2                            0x00000004                                         // External Match 2
  #define EM3                            0x00000008                                         // External Match 3
  #define EMC0_DO_NOTHING                0x00000000
  #define EMC0_CLR_ON_MATCH              0x00000010                                         // Clear MATn.0 pin on match
  #define EMC0_SET_ON_MATCH              0x00000020                                         // Set MATn.0 pin on match
  #define EMC0_TOGGLE_ON_MATCH           0x00000030                                         // Toggle MATn.0 pin on match
  #define EMC1_DO_NOTHING                0x00000000
  #define EMC1_CLR_ON_MATCH              0x00000040                                         // Clear MATn.1 pin on match
  #define EMC1_SET_ON_MATCH              0x00000080                                         // Set MATn.1 pin on match
  #define EMC1_TOGGLE_ON_MATCH           0x000000c0                                         // Toggle MATn.1 pin on match
  #define EMC2_DO_NOTHING                0x00000000
  #define EMC2_CLR_ON_MATCH              0x00000100                                         // Clear MATn.2 pin on match
  #define EMC2_SET_ON_MATCH              0x00000200                                         // Set MATn.2 pin on match
  #define EMC2_TOGGLE_ON_MATCH           0x00000300                                         // Toggle MATn.2 pin on match
  #define EMC3_DO_NOTHING                0x00000000
  #define EMC3_CLR_ON_MATCH              0x00000400                                         // Clear MATn.3 pin on match
  #define EMC3_SET_ON_MATCH              0x00000800                                         // Set MATn.3 pin on match
  #define EMC3_TOGGLE_ON_MATCH           0x00000c00                                         // Toggle MATn.3 pin on match
#define T0CTCR                           *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x70) // Count Control Register Register
  #define TIMER_MODE                     0x00000000                                         // count on every rising PCLK edge
  #define COUNTER_MODE_RISING_EDGE       0x00000001
  #define COUNTER_MODE_FALLING_EDGE      0x00000002
  #define COUNTER_MODE_BOTH_EDGES        0x00000003
  #define COUNTER_MODE_INPUT_CAP0        0x00000000
  #define COUNTER_MODE_INPUT_CAP1        0x00000004
  #define COUNTER_MODE_INPUT_CAP2        0x00000008
  #define COUNTER_MODE_INPUT_CAP3        0x0000000c
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {11}
    #define PWM0CON                      *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_0 + 0x74) // PWM Control Register
      #define PWM_MAT0_ENABLE            0x00000001
      #define PWM_MAT1_ENABLE            0x00000002
      #define PWM_MAT2_ENABLE            0x00000004
      #define PWM_MAT3_ENABLE            0x00000008
#endif

#define T1IR                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x00) // Interrupt Register
#define T1TCR                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x04) // Timer Control Register
#define T1TC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x08) // Timer Counter
#define T1PR                             *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x0c) // Prescale Register
#define T1PC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x10) // Prescale Counter
#define T1MCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x14) // Match Control Register
#define T1MR0                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x18) // Match Register 0
#define T1MR1                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x1c) // Match Register 1
#define T1MR2                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x20) // Match Register 2
#define T1MR3                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x24) // Match Register 3
#define T1CCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x28) // Capture Control Register
#define T1CR0                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x2c) // Capture Register 0 (Read Only)
#define T1CR1                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x30) // Capture Register 1 (Read Only)
#define T1CR2                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x34) // Capture Register 2 (Read Only)
#define T1CR3                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x38) // Capture Register 3 (Read Only)
#define T1EMR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x3c) // External Match Register
#define T1CTCR                           *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x70) // Count Control Register
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {11}
    #define PWM1CON                      *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_1 + 0x74) // PWM Control Register
#endif

#ifndef _LPC214X                                                         // not LPC2148
#define T2IR                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x00) // Interrupt Register
#define T2TCR                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x04) // Timer Control Register
#define T2TC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x08) // Timer Counter
#define T2PR                             *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x0c) // Prescale Register
#define T2PC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x10) // Prescale Counter
#define T2MCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x14) // Match Control Register
#define T2MR0                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x18) // Match Register 0
#define T2MR1                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x1c) // Match Register 1
#define T2MR2                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x20) // Match Register 2
#define T2MR3                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x24) // Match Register 3
#define T2CCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x28) // Capture Control Register
#define T2CR0                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x2c) // Capture Register 0 (Read Only)
#define T2CR1                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x30) // Capture Register 1 (Read Only)
#define T2CR2                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x34) // Capture Register 2 (Read Only)
#define T2CR3                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x38) // Capture Register 3 (Read Only)
#define T2EMR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x3c) // External Match Register
#define T2CTCR                           *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x70) // Count Control Register Register
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {11}
    #define PWM2CON                      *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_2 + 0x74) // PWM Control Register
#endif

#define T3IR                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x00) // Interrupt Register
#define T3TCR                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x04) // Timer Control Register
#define T3TC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x08) // Timer Counter
#define T3PR                             *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x0c) // Prescale Register
#define T3PC                             *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x10) // Prescale Counter
#define T3MCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x14) // Match Control Register
#define T3MR0                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x18) // Match Register 0
#define T3MR1                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x1c) // Match Register 1
#define T3MR2                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x20) // Match Register 2
#define T3MR3                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x24) // Match Register 3
#define T3CCR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x28) // Capture Control Register
#define T3CR0                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x2c) // Capture Register 0 (Read Only)
#define T3CR1                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x30) // Capture Register 1 (Read Only)
#define T3CR2                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x34) // Capture Register 2 (Read Only)
#define T3CR3                            *(volatile unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x38) // Capture Register 3 (Read Only)
#define T3EMR                            *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x3c) // External Match Register
#define T3CTCR                           *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x70) // Count Control Register Register
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {11}
    #define PWM3CON                      *(unsigned long*)(TIMER_PERIPHERAL_BLOCK_3 + 0x74) // PWM Control Register
#endif
#endif

typedef struct stTIMER_CONTROL
{
unsigned long TIR;        
unsigned long TTCR;       
unsigned long TTC;        
unsigned long TPR;        
unsigned long TPC;        
unsigned long TMCR;       
unsigned long TMR0;       
unsigned long TMR1;       
unsigned long TMR2;       
unsigned long TMR3;       
unsigned long TCCR;       
unsigned long TCR0;       
unsigned long TCR1;       
unsigned long TCR2;       
unsigned long TCR3;       
unsigned long TEMR;       
unsigned long ucRes1[12];
unsigned long TCTCR;
#if defined LPC2101 || defined LPC2102 || defined LPC2103                // {11}
    unsigned long PWMCON;
#endif
} TIMER_CONTROL;

// UART
//
#define U0RBR_THR                        *(volatile unsigned long*)(UART_BLOCK_0 + 0x00) // Receive Buffer Register (read) / Transmit Holding Register (write)  (DLAB = 0)
#define U0DLL                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x00) // Divisor Latch LSB  (DLAB = 1)
#define U0DLM                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x04) // Divisor Latch MSB  (DLAB = 1)
#define U0IER                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x04) // Interrupt Enable Register (DLAB = 0)
  #define UART_RDA_INT                   0x00000001                                      // enable receive data available interrupt
  #define UART_THRE_INT                  0x00000002                                      // enable THRE interrupt
  #define UART_RXLINE_STATUS_INT         0x00000004                                      // enable Rx line status interrupt
  #define UART_ABTOIntEn                 0x00000100                                      // enable Auto-Baud timeout interrupt
  #define UART_ABEOIntEn                 0x00000200                                      // enable End of Auto-Baud interrupt
#define U0IIR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x08) // Interrupt ID Register (Read Only)
  #define UART_NO_INT_PENDING            0x00000001                                      // no interrupts presently pending
  #define UART_INT_MASK                  0x0000000e                                      // mask for interrupt source
  #define UART_THRE_INT_PENDING          0x00000002                                      // THRE interrupt is pending
  #define UART_RDA_INT_PENDING           0x00000004                                      // Receive Data Available interrupt is pending
  #define UART_RLS_INT_PENDING           0x00000006                                      // Receive Line Status interrupt is pending
  #define UART_CTI_INT_PENDING           0x0000000c                                      // Character Time-out Indicator interrupt is pending
  #define UART_FIFO_ENABLED              0x000000c0                                      // FIFOs are enabled
  #define UART_ABEO_INT_PENDING          0x00000100                                      // Auto-baud interrupt is pending
  #define UART_ABTO_INT_PENDING          0x00000200                                      // Auto-bacu Timeout interrupt is pending
#define U0FCR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x08) // FIFO Control Register (Write Only)
  #define FIFO_ENABLE                    0x01                                            // enable FIFO (changes clear FIFOs)
  #define RX_FIFO_RESET                  0x02                                            // Reset Rx FIFO and clear pointer logic (self-clearing bit)
  #define TX_FIFO_RESET                  0x04                                            // Reset Tx FIFO and clear pointer logic (self-clearing bit)
  #define RX_TRIGGER_1_CHAR              0x00                                            // Interrupt for every received character
  #define RX_TRIGGER_4_CHARS             0x40                                            // Interrupt for every 4 received characters
  #define RX_TRIGGER_8_CHARS             0x80                                            // Interrupt for every 8 received characters
  #define RX_TRIGGER_14_CHARS            0xc0                                            // Interrupt for every 14 received characters
#define U0LCR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x0C) // {34} Line Control Register
  #define UART_5_BIT_CHAR                0x00                                            // set 5 bit characters
  #define UART_6_BIT_CHAR                0x01                                            // set 6 bit characters
  #define UART_7_BIT_CHAR                0x02                                            // set 7 bit characters
  #define UART_8_BIT_CHAR                0x03                                            // set 8 bit characters
  #define UART_1_STOP_BITS               0x00                                            // set 1 stop bits
  #define UART_2_STOP_BITS               0x04                                            // set 2 stop bits (1.5 in 5 bit character mode)
  #define UART_PARITY_DISABLED           0x00                                            // disable parity
  #define UART_PARITY_ENABLE             0x08                                            // enable parity
  #define UART_PARITY_ODD                0x00                                            // set odd parity mode
  #define UART_PARITY_EVEN               0x10                                            // set even parity mode
  #define UART_PARITY_ONE                0x20                                            // set forced '1' stick parity mode
  #define UART_PARITY_ZERO               0x30                                            // set forced '0' stick parity mode
  #define BREAK_CONTROL                  0x40                                            // force break on TXD line
  #define DLAB                           0x80                                            // enable access to divisor latches
#define U0LSR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x14) // Line Status Register (Read Only)
  #define UART_RDR                       0x01                                            // receiver data ready
  #define UART_OE                        0x02                                            // overrun error
  #define UART_PE                        0x04                                            // parity error
  #define UART_FE                        0x08                                            // framing error
  #define UART_BI                        0x10                                            // break interrupt
  #define UART_THRE                      0x20                                            // transmit holding register empty
  #define UART_TEMT                      0x40                                            // transmitter empty
  #define UART_RXFE                      0x80                                            // error in receive FIFO
#define U0SCR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x1C) // Scratch Pad Register
#define U0ACR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x20) // Auto-baud Control Register
  #define UART_AUTOBAUD_START            0x00000001                                      // Auto-baud start / running
  #define UART_AUTOBAUD_MODE_0           0x00000000                                      // Auto-baud mode 0
  #define UART_AUTOBAUD_MODE_1           0x00000002                                      // Auto-baud mode 1
  #define UART_AUTOBAUD_AUTO_RESTART     0x00000004                                      // Auto-baud auto-restart in case of timeout
  #define UART_AUTOBAUD_ABEOIntClr       0x00000100                                      // Auto-baud interrupt clear
  #define UART_AUTOBAUD_ABTOIntClr       0x00000200                                      // Auto-baud time-out interrupt clear
#define U0FDR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x28) // Fractional Divider Register
#define U0TER                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x30) // Transmit Enable Register

typedef struct stLPC23XX_UART_CONTROL
{
volatile unsigned long RBR_THR_DLL;
volatile unsigned long DLM_IER;
volatile unsigned long IIR_FCR;
volatile unsigned long LCR;
volatile unsigned long U1MCR;                                            // only UART1
volatile unsigned long LSR;
volatile unsigned long U1MSR;                                            // only UART1
volatile unsigned long SCR;
volatile unsigned long ACR;
volatile unsigned long U3ICR;                                            // only UART3
volatile unsigned long FDR;
volatile unsigned long ulRes4;
volatile unsigned long TER;
} LPC23XX_UART_CONTROL;

#define U1RBR_THR                        *(volatile unsigned long*)(UART_BLOCK_1 + 0x00) // Receive Buffer Register (read) / Transmit Holding Register (write)
#define U1DLL                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x00) // Divisor Latch LSB  (DLAB = 1)
#define U1DLM                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x04) // Divisor Latch MSB  (DLAB = 1)
#define U1IER                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x04) // Interrupt Enable Register (DLAB = 0)
#define U1IIR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x08) // Interrupt ID Register (Read Only)
#define U1FCR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x08) // FIFO Control Register (Write Only)
#define U1LCR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x0C) // {34} Line Control Register
#define U1MCR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x10) // Modem Control Register
#define U1LSR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x14) // Line Status Register (Read Only)
#define U1MSR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x18) // Modem Status Register (Read Only)
#define U1SCR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x1C) // Scratch Pad Register
#define U1ACR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x20) // Auto-baud Control Register
#define U1FDR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x28) // Fractional Divider Register
#define U1TER                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x30) // Transmit Enable Register

#define U2RBR_THR                        *(volatile unsigned long*)(UART_BLOCK_2 + 0x00) // Receive Buffer Register (read) / Transmit Holding Register (write)
#define U2DLL                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x00) // Divisor Latch LSB  (DLAB = 1)
#define U2DLM                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x04) // Divisor Latch MSB  (DLAB = 1)
#define U2IER                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x04) // Interrupt Enable Register (DLAB = 0)
#define U2IIR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x08) // Interrupt ID Register (Read Only)
#define U2FCR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x08) // FIFO Control Register (Write Only)
#define U2LCR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x0C) // {34} Line Control Register
#define U2LSR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x14) // Line Status Register (Read Only)
#define U2SCR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x1C) // Scratch Pad Register
#define U2ACR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x20) // Auto-baud Control Register
#define U2FDR                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x28) // Fractional Divider Register
#define U2TER                            *(volatile unsigned long*)(UART_BLOCK_2 + 0x30) // Transmit Enable Register

#define U3RBR_THR                        *(volatile unsigned long*)(UART_BLOCK_3 + 0x00) // Receive Buffer Register (read) / Transmit Holding Register (write)
#define U3DLL                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x00) // Divisor Latch LSB  (DLAB = 1)
#define U3DLM                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x04) // Divisor Latch MSB  (DLAB = 1)
#define U3IER                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x04) // Interrupt Enable Register (DLAB = 0)
#define U3IIR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x08) // Interrupt ID Register (Read Only)
#define U3FCR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x08) // FIFO Control Register (Write Only)
#define U3LCR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x0C) // {34} Line Control Register
#define U3LSR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x14) // Line Status Register (Read Only)
#define U3SCR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x1C) // Scratch Pad Register
#define U3ACR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x20) // Auto-baud Control Register
#define U3ICR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x24) // IrDA Control Register
  #define UART_IrDAEN                    0x01                                            // IrDA Mode enable
  #define UART_IrDAINV                   0x02                                            // IrDA serial input inverted
  #define UART_FIX_PULSE_ENABLE          0x04                                            // IrDA Fixed Pulse Width Mode enable
  #define UART_PULSE_WIDTH_2_TPCLK       0x00                                            // IrDA pulse width fixed at 2 x Tpclk
  #define UART_PULSE_WIDTH_4_TPCLK       0x08                                            // IrDA pulse width fixed at 4 x Tpclk
  #define UART_PULSE_WIDTH_8_TPCLK       0x10                                            // IrDA pulse width fixed at 8 x Tpclk
  #define UART_PULSE_WIDTH_16_TPCLK      0x18                                            // IrDA pulse width fixed at 16 x Tpclk
  #define UART_PULSE_WIDTH_32_TPCLK      0x20                                            // IrDA pulse width fixed at 32 x Tpclk
  #define UART_PULSE_WIDTH_64_TPCLK      0x28                                            // IrDA pulse width fixed at 64 x Tpclk
  #define UART_PULSE_WIDTH_128_TPCLK     0x30                                            // IrDA pulse width fixed at 128 x Tpclk
  #define UART_PULSE_WIDTH_256_TPCLK     0x38                                            // IrDA pulse width fixed at 256 x Tpclk
#define U3FDR                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x28) // Fractional Divider Register
#define U3TER                            *(volatile unsigned long*)(UART_BLOCK_3 + 0x30) // Transmit Enable Register


// PWM module                                                            {24}
//
#define PWM0IR                           *(volatile unsigned long*)(PWM_BLOCK_0 + 0x00)  // PWM0 Interrupt Register
#define PWM0TCR                          *(unsigned long*)(PWM_BLOCK_0 + 0x04)           // PWM0 Timer Control Register Register
  #define PWMTCR_CounterEnable           0x00000001
  #define PWMTCR_CounterReset            0x00000002
  #define PWMTCR_PWMEnable               0x00000008
#define PWM0TC                           *(volatile unsigned long*)(PWM_BLOCK_0 + 0x08)  // PWM0 Timer Counter
#define PWM0PR                           *(unsigned long*)(PWM_BLOCK_0 + 0x0c)           // PWM0 Prescale Register
#define PWM0PC                           *(volatile unsigned long*)(PWM_BLOCK_0 + 0x10)  // PWM0 Prescale Counter
#define PWM0MCR                          *(unsigned long*)(PWM_BLOCK_0 + 0x14)           // PWM0 Match Control Register
  #define PWMMR0I                        0x00000001                                      // interrupt on channel 0 match
  #define PWMMR0R                        0x00000002                                      // reset on channel 0 match
  #define PWMMR0S                        0x00000004                                      // stop on channel 0 match
  #define PWMMR1I                        0x00000008                                      // interrupt on channel 1 match
  #define PWMMR1R                        0x00000010                                      // reset on channel 1 match
  #define PWMMR1S                        0x00000020                                      // stop on channel 1 match
  #define PWMMR2I                        0x00000040                                      // interrupt on channel 2 match
  #define PWMMR2R                        0x00000080                                      // reset on channel 2 match
  #define PWMMR2S                        0x00000100                                      // stop on channel 2 match
  #define PWMMR3I                        0x00000200                                      // interrupt on channel 3 match
  #define PWMMR3R                        0x00000400                                      // reset on channel 3 match
  #define PWMMR3S                        0x00000800                                      // stop on channel 3 match
  #define PWMMR4I                        0x00001000                                      // interrupt on channel 4 match
  #define PWMMR4R                        0x00002000                                      // reset on channel 4 match
  #define PWMMR4S                        0x00004000                                      // stop on channel 4 match
  #define PWMMR5I                        0x00008000                                      // interrupt on channel 5 match
  #define PWMMR5R                        0x00010000                                      // reset on channel 5 match
  #define PWMMR5S                        0x00020000                                      // stop on channel 5 match
  #define PWMMR6I                        0x00040000                                      // interrupt on channel 6 match
  #define PWMMR6R                        0x00080000                                      // reset on channel 6 match
  #define PWMMR6S                        0x00100000                                      // stop on channel 6 match
#define PWM0MR0                          *(unsigned long*)(PWM_BLOCK_0 + 0x18)           // PWM0 Match Register 0
#define PWM0MR1                          *(unsigned long*)(PWM_BLOCK_0 + 0x1c)           // PWM0 Match Register 1
#define PWM0MR2                          *(unsigned long*)(PWM_BLOCK_0 + 0x20)           // PWM0 Match Register 2
#define PWM0MR3                          *(unsigned long*)(PWM_BLOCK_0 + 0x24)           // PWM0 Match Register 3
#define PWM0CCR                          *(unsigned long*)(PWM_BLOCK_0 + 0x28)           // PWM0 Capture Control Register
  #define CAP_RISING_EDGE_0              0x00000001                                      // capture on CAPx.0 rising edge
  #define CAP_FALLING_EDGE_0             0x00000002                                      // capture on CAPx.0 falling edge
  #define CAP_EVENT_INTERRUPT_0          0x00000004                                      // interrupt on CAPx.0 capture event
  #define CAP_RISING_EDGE_1              0x00000008                                      // capture on CAPx.1 rising edge
  #define CAP_FALLING_EDGE_1             0x00000010                                      // capture on CAPx.1 falling edge
  #define CAP_EVENT_INTERRUPT_1          0x00000020                                      // interrupt on CAPx.1 capture event
  #define CAP_RISING_EDGE_2              0x00000040                                      // capture on CAPx.2 rising edge
  #define CAP_FALLING_EDGE_2             0x00000080                                      // capture on CAPx.2 falling edge
  #define CAP_EVENT_INTERRUPT_2          0x00000100                                      // interrupt on CAPx.2 capture event
  #define CAP_RISING_EDGE_3              0x00000200                                      // capture on CAPx.3 rising edge
  #define CAP_FALLING_EDGE_3             0x00000400                                      // capture on CAPx.3 falling edge
  #define CAP_EVENT_INTERRUPT_3          0x00000800                                      // interrupt on CAPx.3 capture event
#define PWM0CR0                          *(unsigned long*)(PWM_BLOCK_0 + 0x2c)           // PWM0 Capture Register 0
#define PWM0CR1                          *(unsigned long*)(PWM_BLOCK_0 + 0x30)           // PWM0 Capture Register 0
#define PWM0CR2                          *(unsigned long*)(PWM_BLOCK_0 + 0x34)           // PWM0 Capture Register 0
#define PWM0CR3                          *(unsigned long*)(PWM_BLOCK_0 + 0x38)           // PWM0 Capture Register 0

#define PWM0MR4                          *(unsigned long*)(PWM_BLOCK_0 + 0x40)           // PWM0 Match Register 4
#define PWM0MR5                          *(unsigned long*)(PWM_BLOCK_0 + 0x44)           // PWM0 Match Register 5
#define PWM0MR6                          *(unsigned long*)(PWM_BLOCK_0 + 0x48)           // PWM0 Match Register 6
#define PWM0PCR                          *(unsigned long*)(PWM_BLOCK_0 + 0x4c)           // PWM0 PWM Control Register
  #define PWMSEL2_DOUBLE_EDGE            0x00000004                                      // select double-edged control mode for PWM2 output
  #define PWMSEL3_DOUBLE_EDGE            0x00000008                                      // select double-edged control mode for PWM3 output
  #define PWMSEL4_DOUBLE_EDGE            0x00000010                                      // select double-edged control mode for PWM4 output
  #define PWMSEL5_DOUBLE_EDGE            0x00000020                                      // select double-edged control mode for PWM5 output
  #define PWMSEL6_DOUBLE_EDGE            0x00000040                                      // select double-edged control mode for PWM6 output
  #define PWMENA1                        0x00000200                                      // enable PWM1 output
  #define PWMENA2                        0x00000400                                      // enable PWM2 output
  #define PWMENA3                        0x00000800                                      // enable PWM3 output
  #define PWMENA4                        0x00001000                                      // enable PWM4 output
  #define PWMENA5                        0x00002000                                      // enable PWM5 output
  #define PWMENA6                        0x00004000                                      // enable PWM6 output
#define PWM0LER                          *(unsigned long*)(PWM_BLOCK_0 + 0x50)           // PWM0 Load Enable Register
  #define PWMMATCH0LATCH                 0x00000001                                      // enable PWM1 match 0 latch
  #define PWMMATCH1LATCH                 0x00000002                                      // enable PWM1 match 1 latch
  #define PWMMATCH2LATCH                 0x00000004                                      // enable PWM1 match 2 latch
  #define PWMMATCH3LATCH                 0x00000008                                      // enable PWM1 match 3 latch
  #define PWMMATCH4LATCH                 0x00000010                                      // enable PWM1 match 4 latch
  #define PWMMATCH5LATCH                 0x00000020                                      // enable PWM1 match 5 latch
  #define PWMMATCH6LATCH                 0x00000040                                      // enable PWM1 match 6 latch
#define PWM0CTCR                         *(unsigned long*)(PWM_BLOCK_0 + 0x70)           // PWM0 Count Control Register
  #define PWMCTCR_TimerMode              0x00000000
  #define PWMCTCR_RisingCounterMode      0x00000001
  #define PWMCTCR_FallingCounterMode     0x00000002
  #define PWMCTCR_BothEdgesCounterMode   0x00000003
  #define PWMCTCR_PCAP0                  0x00000000                                      // when capture input used to count, use PCAPx.0 input
  #define PWMCTCR_CAP1                   0x00000004                                      // when capture input used to count, use CAPx.1 input

#define PWM1IR                           *(volatile unsigned long*)(PWM_BLOCK_1 + 0x00)  // PWM1 Interrupt Register
#define PWM1TCR                          *(unsigned long*)(PWM_BLOCK_1 + 0x04)           // PWM1 Timer Control Register Register
#define PWM1TC                           *(volatile unsigned long*)(PWM_BLOCK_1 + 0x08)  // PWM1 Timer Counter
#define PWM1PR                           *(unsigned long*)(PWM_BLOCK_1 + 0x0c)           // PWM1 Prescale Register
#define PWM1PC                           *(volatile unsigned long*)(PWM_BLOCK_1 + 0x10)  // PWM1 Prescale Counter
#define PWM1MCR                          *(unsigned long*)(PWM_BLOCK_1 + 0x14)           // PWM1 Match Control Register
#define PWM1MR0                          *(unsigned long*)(PWM_BLOCK_1 + 0x18)           // PWM1 Match Register 0
#define PWM1MR1_ADD                      (unsigned long*)(PWM_BLOCK_1 + 0x1c)            // PWM1 Match Register 1 address
#define PWM1MR1                          *(unsigned long*)(PWM_BLOCK_1 + 0x1c)           // PWM1 Match Register 1
#define PWM1MR2                          *(unsigned long*)(PWM_BLOCK_1 + 0x20)           // PWM1 Match Register 2
#define PWM1MR3                          *(unsigned long*)(PWM_BLOCK_1 + 0x24)           // PWM1 Match Register 3
#define PWM1CCR                          *(unsigned long*)(PWM_BLOCK_1 + 0x28)           // PWM1 Capture Control Register
#define PWM1CR0                          *(unsigned long*)(PWM_BLOCK_1 + 0x2c)           // PWM1 Capture Register 0
#define PWM1CR1                          *(unsigned long*)(PWM_BLOCK_1 + 0x30)           // PWM1 Capture Register 0
#define PWM1CR2                          *(unsigned long*)(PWM_BLOCK_1 + 0x34)           // PWM1 Capture Register 0
#define PWM1CR3                          *(unsigned long*)(PWM_BLOCK_1 + 0x38)           // PWM1 Capture Register 0

#define PWM1MR4_ADD                      (unsigned long*)(PWM_BLOCK_1 + 0x40)            // PWM1 Match Register 4 address
#define PWM1MR4                          *(unsigned long*)(PWM_BLOCK_1 + 0x40)           // PWM1 Match Register 4
#define PWM1MR5                          *(unsigned long*)(PWM_BLOCK_1 + 0x44)           // PWM1 Match Register 5
#define PWM1MR6                          *(unsigned long*)(PWM_BLOCK_1 + 0x48)           // PWM1 Match Register 6
#define PWM1PCR                          *(unsigned long*)(PWM_BLOCK_1 + 0x4c)           // PWM1 PWM Control Register
#define PWM1LER                          *(unsigned long*)(PWM_BLOCK_1 + 0x50)           // PWM1 Load Enable Register

#define PWM1CTCR                         *(unsigned long*)(PWM_BLOCK_1 + 0x70)           // PWM1 Count Control Register



// SSP Controller
//
#define SSP0CR0                          *(unsigned long*)(SSP_BLOCK_0 + 0x00)              // SSP 0 Control Register 0
  #define DSS_4_BIT                      0x00000003
  #define DSS_5_BIT                      0x00000004
  #define DSS_6_BIT                      0x00000005
  #define DSS_7_BIT                      0x00000006
  #define DSS_8_BIT                      0x00000007
  #define DSS_9_BIT                      0x00000008
  #define DSS_10_BIT                     0x00000009
  #define DSS_11_BIT                     0x0000000a
  #define DSS_12_BIT                     0x0000000b
  #define DSS_13_BIT                     0x0000000c
  #define DSS_14_BIT                     0x0000000d
  #define DSS_15_BIT                     0x0000000e
  #define DSS_16_BIT                     0x0000000f
  #define FRS_FREESCALE_SPI              0x00000000
  #define FRS_TI_SSFF                    0x00000010
  #define FRS_MICROWIRE                  0x00000020
  #define SSP_SPO                        0x00000040                                         // serial clock polarity
  #define SSP_SPH                        0x00000080                                         // serial clock phase
  #define SSP_SCR_SHIFT                      8
#define SSP0CR1                          *(unsigned long*)(SSP_BLOCK_0 + 0x04)              // SSP 0 Control Register 1
  #define SSP_LBM                        0x00000001                                         // Loop-back mode
  #define SSP_SSE                        0x00000002                                         // SSI Enable
  #define SSP_MS                         0x00000004                                         // Slave mode
  #define SSP_SOD                        0x00000008                                         // Slave mode output disable
#define SSP0DR                           *(volatile unsigned long*)(SSP_BLOCK_0 + 0x08)     // SSP 0 Data Register
#define SSP0SR                           *(volatile unsigned long*)(SSP_BLOCK_0 + 0x0c)     // SSP 0 Status (Read Only)
  #define SSP_BSY                        0x00000010                                         // SSI Busy bit
  #define SSP_RFF                        0x00000008                                         // SSI Receive FIFO is full
  #define SSP_RNE                        0x00000004                                         // SSI Receive FIFO not empty
  #define SSP_TNF                        0x00000002                                         // SSI Transmit FIFO not full
  #define SSP_TFE                        0x00000001                                         // SSI Transmit FIFO is empty
#define SSP0CPSR                         *(unsigned long*)(SSP_BLOCK_0 + 0x10)              // SSP 0 Clock Prescale Register
#define SSP0IMSC                         *(volatile unsigned long*)(SSP_BLOCK_0 + 0x14)     // SSP 0 Interrupt Mask Set and Clear Register
  #define SSP_TXIM                       0x00000008                                         // SSI Transmit FIFO half full, or less interrupt not masked
  #define SSP_RXIM                       0x00000004                                         // SSI Receive FIFO half-full, or more interrupt not masked
  #define SSP_RTIM                       0x00000002                                         // SSI Receive FIFO timeout interrupt not masked
  #define SSP_RORIM                      0x00000001                                         // SSI Receive FIFO over-run interrupt not masked
#define SSP0RIS                          *(volatile unsigned long*)(SSP_BLOCK_0 + 0x18)     // SSP 0 RAW Interrupt Status Register
  #define SSP_TXRIS                      0x00000008                                         // SSI Transmit FIFO half full, or less RAW interrupt status
  #define SSP_RXRIS                      0x00000004                                         // SSI Receive FIFO half-full, or more RAW interrupt status
  #define SSP_RTRIS                      0x00000002                                         // SSI Receive FIFO timeout RAW interrupt status
  #define SSP_RORRIS                     0x00000001                                         // SSI Receive FIFO over-run RAW interrupt status
#define SSP0MIS                          *(volatile unsigned long*)(SSP_BLOCK_0 + 0x1c)     // SSP 0 Masked Interrupt Status Register
  #define SSP_TXMIS                      0x00000008                                         // SSI Transmit FIFO half full, or less masked interrupt status
  #define SSP_RXMIS                      0x00000004                                         // SSI Receive FIFO half-full, or more masked interrupt status
  #define SSP_RTMIS                      0x00000002                                         // SSI Receive FIFO timeout masked interrupt status
  #define SSP_RORMIS                     0x00000001                                         // SSI Receive FIFO over-run masked interrupt status
#define SSP0ICR                          *(volatile unsigned long*)(SSP_BLOCK_0 + 0x20)     // SSP 0 Interrupt Clear Register
  #define SSP_RTIC                       0x00000002                                         // SSI Receive FIFO timeout interrupt clear
  #define SSP_RORIC                      0x00000001                                         // SSI Receive FIFO over-run interrupt clear
#define SSP0DMACR                        *(unsigned long*)(SSP_BLOCK_0 + 0x24)              // SSP 0 DAM Control Register
  #define SSP_RXDMAE                     0x00000001                                         // SSI Enable DMA for receive FIFO
  #define SSP_TXDMAE                     0x00000002                                         // SSI Enable DMA for transmit FIFO

#define SSP1CR0                          *(unsigned long*)(SSP_BLOCK_1 + 0x00)              // SSP 1 Control Register 0
#define SSP1CR1                          *(unsigned long*)(SSP_BLOCK_1 + 0x04)              // SSP 1 Control Register 1
#define SSP1DR                           *(volatile unsigned long*)(SSP_BLOCK_1 + 0x08)     // SSP 1 Data Register
#define SSP1SR                           *(volatile unsigned long*)(SSP_BLOCK_1 + 0x0c)     // SSP 1 Status (Read Only)
#define SSP1CPSR                         *(unsigned long*)(SSP_BLOCK_1 + 0x10)              // SSP 1 Clock Prescale Register
#define SSP1IMSC                         *(volatile unsigned long*)(SSP_BLOCK_1 + 0x14)     // SSP 1 Interrupt Mask Set and Clear Register
#define SSP1RIS                          *(volatile unsigned long*)(SSP_BLOCK_1 + 0x18)     // SSP 1 RAW Interrupt Status Register
#define SSP1MIS                          *(volatile unsigned long*)(SSP_BLOCK_1 + 0x1c)     // SSP 1 Masked Interrupt Status Register
#define SSP1ICR                          *(volatile unsigned long*)(SSP_BLOCK_1 + 0x20)     // SSP 1 Interrupt Clear Register
#define SSP1DMACR                        *(unsigned long*)(SSP_BLOCK_1 + 0x24)              // SSP 1 DAM Control Register

// ADC                                                                                      {27}
//
#define AD0CR                            *(unsigned long*)(ADC_BLOCK + 0x00)                // ADC Control Register
  #define ADCR_SEL_CHAN0                 0x00000001
  #define ADCR_SEL_CHAN1                 0x00000002
  #define ADCR_SEL_CHAN2                 0x00000004
  #define ADCR_SEL_CHAN3                 0x00000008
  #define ADCR_SEL_CHAN4                 0x00000010
  #define ADCR_SEL_CHAN5                 0x00000020
  #define ADCR_SEL_CHAN6                 0x00000040
  #define ADCR_SEL_CHAN7                 0x00000080
  #define ADCR_CLKDIV_MASK               0x0000ff00                                         // maximum clock rate 4.5MHz
  #define ADCR_BURST                     0x00010000                                         // burst mode (START bits must be zero for conversion to start)
  #define ADCR_CLKS_10BITS               0x00000000                                         // 11 clocks for 10 bit resolution
  #define ADCR_CLKS_9BITS                0x00020000                                         // 10 clocks for 9 bit resolution
  #define ADCR_CLKS_8BITS                0x00040000                                         // 9 clocks for 8 bit resolution
  #define ADCR_CLKS_7BITS                0x00060000                                         // 8 clocks for 7 bit resolution
  #define ADCR_CLKS_6BITS                0x00080000                                         // 7 clocks for 6 bit resolution
  #define ADCR_CLKS_5BITS                0x000a0000                                         // 6 clocks for 5 bit resolution
  #define ADCR_CLKS_4BITS                0x000c0000                                         // 5 clocks for 4 bit resolution
  #define ADCR_CLKS_3BITS                0x000e0000                                         // 4 clocks for 3 bit resolution
  #define ADCR_PDN                       0x00200000                                         // ADC converter operational
  #define ADCR_START_NO_START            0x00000000
  #define ADCR_START_START_CONVERSION    0x01000000
  #define ADCR_START_START_P2_10_EINT0   0x02000000                                         // LPC23XX/LPC24XX
  #define ADCR_START_START_P0_16_EINT0_MAT0_2  0x02000000                                   // LPC210X
  #define ADCR_START_START_P1_27_CAP0_1  0x03000000                                         // LPC23XX/LPC24XX
  #define ADCR_START_START_P0_22         0x03000000                                         // LPC21XX
  #define ADCR_START_START_MAT0_1        0x04000000
  #define ADCR_START_START_MAT0_3        0x05000000
  #define ADCR_START_START_MAT1_0        0x06000000
  #define ADCR_START_START_MAT1_1        0x07000000
  #define ADCR_START_START_FALLING_EDGE  0x08000000
#define AD0GDR                           *(volatile unsigned long*)(ADC_BLOCK + 0x04)       // ADC Global Data Register
  #define ADGDR_V_VREF_MASK              0x0000ffc0
  #define ADGDR_CHANNEL0                 0x00000000
  #define ADGDR_CHANNEL1                 0x01000000
  #define ADGDR_CHANNEL2                 0x02000000
  #define ADGDR_CHANNEL3                 0x03000000
  #define ADGDR_CHANNEL4                 0x04000000
  #define ADGDR_CHANNEL5                 0x05000000
  #define ADGDR_CHANNEL6                 0x06000000
  #define ADGDR_CHANNEL7                 0x07000000
  #define ADGDR_OVERRUN                  0x40000000
  #define ADGDR_DONE                     0x80000000
#define AD0INTEN                         *(unsigned long*)(ADC_BLOCK + 0x0c)                // ADC Interrupt Enable Register
  #define ADCR_ADINTEN_CHAN0             0x00000001
  #define ADCR_ADINTEN_CHAN1             0x00000002
  #define ADCR_ADINTEN_CHAN2             0x00000004
  #define ADCR_ADINTEN_CHAN3             0x00000008
  #define ADCR_ADINTEN_CHAN4             0x00000010
  #define ADCR_ADINTEN_CHAN5             0x00000020
  #define ADCR_ADINTEN_CHAN6             0x00000040
  #define ADCR_ADINTEN_CHAN7             0x00000080
  #define ADCR_ADGINTEN                  0x00000100
#define AD0DR0                           *(volatile unsigned long*)(ADC_BLOCK + 0x10)       // ADC Channel 0 Data Register
#define AD0DR1                           *(volatile unsigned long*)(ADC_BLOCK + 0x14)       // ADC Channel 0 Data Register
#define AD0DR2                           *(volatile unsigned long*)(ADC_BLOCK + 0x18)       // ADC Channel 0 Data Register
#define AD0DR3                           *(volatile unsigned long*)(ADC_BLOCK + 0x1c)       // ADC Channel 0 Data Register
#define AD0DR4                           *(volatile unsigned long*)(ADC_BLOCK + 0x20)       // ADC Channel 0 Data Register
#define AD0DR5                           *(volatile unsigned long*)(ADC_BLOCK + 0x24)       // ADC Channel 0 Data Register
#define AD0DR6                           *(volatile unsigned long*)(ADC_BLOCK + 0x28)       // ADC Channel 0 Data Register
#define AD0DR7                           *(volatile unsigned long*)(ADC_BLOCK + 0x2c)       // ADC Channel 0 Data Register
#define AD0STAT                          *(volatile unsigned long*)(ADC_BLOCK + 0x30)       // ADC Status Register (read-only)
  #define ADCR_DONE_CHAN0                0x00000001
  #define ADCR_DONE_CHAN1                0x00000002
  #define ADCR_DONE_CHAN2                0x00000004
  #define ADCR_DONE_CHAN3                0x00000008
  #define ADCR_DONE_CHAN4                0x00000010
  #define ADCR_DONE_CHAN5                0x00000020
  #define ADCR_DONE_CHAN6                0x00000040
  #define ADCR_DONE_CHAN7                0x00000080
  #define ADCR_OVERRUN_CHAN0             0x00000100
  #define ADCR_OVERRUN_CHAN1             0x00000200
  #define ADCR_OVERRUN_CHAN2             0x00000400
  #define ADCR_OVERRUN_CHAN3             0x00000800
  #define ADCR_OVERRUN_CHAN4             0x00001000
  #define ADCR_OVERRUN_CHAN5             0x00002000
  #define ADCR_OVERRUN_CHAN6             0x00004000
  #define ADCR_OVERRUN_CHAN7             0x00008000
  #define ADCR_ADINT                     0x00010000

// CAN Controller                                                        {31}
//
#define AFMR                             *(unsigned long*)(CAN_AF_BLOCK + 0x00) // Acceptance Filter Register
  #define AFMR_AccOff                    0x00000001
  #define AFMR_AccBP                     0x00000002
  #define AFMR_eFCAN                     0x00000004
#define SFF_sa                           *(unsigned long*)(CAN_AF_BLOCK + 0x04) // Standard Frame Individual Start Address Register
#define SFF_GRP_sa                       *(unsigned long*)(CAN_AF_BLOCK + 0x08) // Standard Frame Group Start Address Register
#define EFF_sa                           *(unsigned long*)(CAN_AF_BLOCK + 0x0c) // Extended Frame Individual Start Address Register
#define EFF_GRP_sa                       *(unsigned long*)(CAN_AF_BLOCK + 0x10) // Extended Frame Group Start Address Register
#define ENDofTable                       *(unsigned long*)(CAN_AF_BLOCK + 0x14) // End of AF Tables Register
#define LUTerrAd                         *(volatile unsigned long*)(CAN_AF_BLOCK + 0x18) // LUT Error Address Register (read-only)
#define LUTerr                           *(volatile unsigned long*)(CAN_AF_BLOCK + 0x1c) // LUT Error Register (read-only)
#define FCANIE                           *(unsigned long*)(CAN_AF_BLOCK + 0x20) // FullCAN Interrupt Enable Register
#define FCANIC0                          *(unsigned long*)(CAN_AF_BLOCK + 0x24) // FullCAN Interrupt and Capture Register 0
#define FCANIC1                          *(unsigned long*)(CAN_AF_BLOCK + 0x28) // FullCAN Interrupt and Capture Register 1

#define CANTxSR                          *(volatile unsigned long*)(CAN_BLOCK + 0x00) // CAN Central Transmit Status Register (read-only)
#define CANRxSR                          *(volatile unsigned long*)(CAN_BLOCK + 0x04) // CAN Central Receive Status Register (read-only)
#define CANMSR                           *(volatile unsigned long*)(CAN_BLOCK + 0x08) // CAN Central Miscellaneous Register (read-only)

#define CAN1MOD                          *(unsigned long*)(CAN1_BLOCK + 0x00)   // CAN1 Mode Register
#define CAN1CMR                          *(volatile unsigned long*)(CAN1_BLOCK + 0x04) // CAN1 Command Register (write-only)
#define CAN1GSR                          *(volatile unsigned long*)(CAN1_BLOCK + 0x08) // CAN1 Global Controller Status and Error Counters Register (read-only)
#define CAN1ICR                          *(volatile unsigned long*)(CAN1_BLOCK + 0x0c) // CAN1 Interrupt Status, Arbitration Lost Capture, Error Code Capture (read-only)
#define CAN1IER                          *(unsigned long*)(CAN1_BLOCK + 0x10)   // CAN1 Interrupt Enable Register
#define CAN1BTR                          *(unsigned long*)(CAN1_BLOCK + 0x14)   // CAN1 Bus Timing Register
#define CAN1EWL                          *(unsigned long*)(CAN1_BLOCK + 0x18)   // CAN1 Error Warning Limit Register
#define CAN1SR                           *(volatile unsigned long*)(CAN1_BLOCK + 0x1c) // CAN1 Status Register (read-only)
#define CAN1RFS                          *(volatile unsigned long*)(CAN1_BLOCK + 0x20) // CAN1 Receive Frame Status Register
#define CAN1RID                          *(volatile unsigned long*)(CAN1_BLOCK + 0x24) // CAN1 Received Identifier
#define CAN1RDA                          *(volatile unsigned long*)(CAN1_BLOCK + 0x28) // CAN1 Received Data Bytes 1-4
#define CAN1RDB                          *(volatile unsigned long*)(CAN1_BLOCK + 0x2c) // CAN1 Received Data Bytes 5-8
#define CAN1TFI1                         *(volatile unsigned long*)(CAN1_BLOCK + 0x30) // CAN1 Transmit Frame Info (Tx Buffer 1)
#define CAN1TID1                         *(volatile unsigned long*)(CAN1_BLOCK + 0x34) // CAN1 Transmit Identifier (Tx Buffer 1)
#define CAN1TDA1                         *(volatile unsigned long*)(CAN1_BLOCK + 0x38) // CAN1 Transmit Data Bytes 1-4 (Tx Buffer 1)
#define CAN1TDB1                         *(volatile unsigned long*)(CAN1_BLOCK + 0x3c) // CAN1 Transmit Data Bytes 5-8 (Tx Buffer 1)
#define CAN1TFI2                         *(volatile unsigned long*)(CAN1_BLOCK + 0x40) // CAN1 Transmit Frame Info (Tx Buffer 2)
#define CAN1TID2                         *(volatile unsigned long*)(CAN1_BLOCK + 0x44) // CAN1 Transmit Identifier (Tx Buffer 2)
#define CAN1TDA2                         *(volatile unsigned long*)(CAN1_BLOCK + 0x48) // CAN1 Transmit Data Bytes 1-4 (Tx Buffer 2)
#define CAN1TDB2                         *(volatile unsigned long*)(CAN1_BLOCK + 0x4c) // CAN1 Transmit Data Bytes 5-8 (Tx Buffer 2)
#define CAN1TFI3                         *(volatile unsigned long*)(CAN1_BLOCK + 0x50) // CAN1 Transmit Frame Info (Tx Buffer 3)
#define CAN1TID3                         *(volatile unsigned long*)(CAN1_BLOCK + 0x54) // CAN1 Transmit Identifier (Tx Buffer 3)
#define CAN1TDA3                         *(volatile unsigned long*)(CAN1_BLOCK + 0x58) // CAN1 Transmit Data Bytes 1-4 (Tx Buffer 3)
#define CAN1TDB3                         *(volatile unsigned long*)(CAN1_BLOCK + 0x5c) // CAN1 Transmit Data Bytes 5-8 (Tx Buffer 3)

#define CAN2MOD                          *(unsigned long*)(CAN2_BLOCK + 0x00)   // CAN2 Mode Register
#define CAN2CMR                          *(volatile unsigned long*)(CAN2_BLOCK + 0x04) // CAN2 Command Register (write-only)
#define CAN2GSR                          *(volatile unsigned long*)(CAN2_BLOCK + 0x08) // CAN2 Global Controller Status and Error Counters Register (read-only)
#define CAN2ICR                          *(volatile unsigned long*)(CAN2_BLOCK + 0x0c) // CAN2 Interrupt Status, Arbitration Lost Capture, Error Code Capture (read-only)
#define CAN2IER                          *(unsigned long*)(CAN2_BLOCK + 0x10)   // CAN2 Interrupt Enable Register
#define CAN2BTR                          *(unsigned long*)(CAN2_BLOCK + 0x14)   // CAN2 Bus Timing Register
#define CAN2EWL                          *(unsigned long*)(CAN2_BLOCK + 0x18)   // CAN2 Error Warning Limit Register
#define CAN2SR                           *(volatile unsigned long*)(CAN2_BLOCK + 0x1c) // CAN2 Status Register (read-only)
#define CAN2RFS                          *(volatile unsigned long*)(CAN2_BLOCK + 0x20) // CAN2 Receive Frame Status Register
#define CAN2RID                          *(volatile unsigned long*)(CAN2_BLOCK + 0x24) // CAN2 Received Identifier
#define CAN2RDA                          *(volatile unsigned long*)(CAN2_BLOCK + 0x28) // CAN2 Received Data Bytes 1-4
#define CAN2RDB                          *(volatile unsigned long*)(CAN2_BLOCK + 0x2c) // CAN2 Received Data Bytes 5-8
#define CAN2TFI1                         *(volatile unsigned long*)(CAN2_BLOCK + 0x30) // CAN2 Transmit Frame Info (Tx Buffer 1)
#define CAN2TID1                         *(volatile unsigned long*)(CAN2_BLOCK + 0x34) // CAN2 Transmit Identifier (Tx Buffer 1)
#define CAN2TDA1                         *(volatile unsigned long*)(CAN2_BLOCK + 0x38) // CAN2 Transmit Data Bytes 1-4 (Tx Buffer 1)
#define CAN2TDB1                         *(volatile unsigned long*)(CAN2_BLOCK + 0x3c) // CAN2 Transmit Data Bytes 5-8 (Tx Buffer 1)
#define CAN2TFI2                         *(volatile unsigned long*)(CAN2_BLOCK + 0x40) // CAN2 Transmit Frame Info (Tx Buffer 2)
#define CAN2TID2                         *(volatile unsigned long*)(CAN2_BLOCK + 0x44) // CAN2 Transmit Identifier (Tx Buffer 2)
#define CAN2TDA2                         *(volatile unsigned long*)(CAN2_BLOCK + 0x48) // CAN2 Transmit Data Bytes 1-4 (Tx Buffer 2)
#define CAN2TDB2                         *(volatile unsigned long*)(CAN2_BLOCK + 0x4c) // CAN2 Transmit Data Bytes 5-8 (Tx Buffer 2)
#define CAN2TFI3                         *(volatile unsigned long*)(CAN2_BLOCK + 0x50) // CAN2 Transmit Frame Info (Tx Buffer 3)
#define CAN2TID3                         *(volatile unsigned long*)(CAN2_BLOCK + 0x54) // CAN2 Transmit Identifier (Tx Buffer 3)
#define CAN2TDA3                         *(volatile unsigned long*)(CAN2_BLOCK + 0x58) // CAN2 Transmit Data Bytes 1-4 (Tx Buffer 3)
#define CAN2TDB3                         *(volatile unsigned long*)(CAN2_BLOCK + 0x5c) // CAN2 Transmit Data Bytes 5-8 (Tx Buffer 3)


#define CAN_EXTENDED_MASK                0x1fffffff





// I2C Controller
//
#define I2C0CONSET                       *(volatile unsigned long*)(I2C_BLOCK_0 + 0x00)     // I2C 0 Control Set Register
  #define IIC_AA                         0x00000004                                         // Assert Acknowledge Flag
  #define IIC_SI                         0x00000008                                         // I2C Interrupt Flag
  #define IIC_STO                        0x00000010                                         // Stop Flag
  #define IIC_STA                        0x00000020                                         // Start Flag
  #define IIC_I2EN                       0x00000040                                         // Interface Enable
#define I2C0STAT                         *(volatile unsigned long*)(I2C_BLOCK_0 + 0x04)     // I2C 0 Status Register (Read Only)
  #define MASTER_START_SENT              0x00000008
  #define MASTER_REPEATED_START_SENT     0x00000010
  #define MASTER_TX_ADDRESS_ACKED        0x00000018
  #define MASTER_TX_ADDRESS_NOT_ACKED    0x00000020
  #define MASTER_TX_DATA_ACKED           0x00000028
  #define MASTER_TX_DATA_NOT_ACKED       0x00000030
  #define MASTER_RX_ADDRESS_ACKED        0x00000040
  #define MASTER_RX_ADDRESS_NOT_ACKED    0x00000048
  #define MASTER_RX_DATA_ACKED           0x00000050
  #define MASTER_RX_DATA_NOT_ACKED       0x00000058
  #define I2C_IDLE_STATE                 0x000000f8
#define I2C0DAT                          *(volatile unsigned long*)(I2C_BLOCK_0 + 0x08)     // I2C 0 Data Register
#define I2C0ADR                          *(unsigned long*)(I2C_BLOCK_0 + 0x0c)              // I2C 0 Slave Address Register
#define I2C0SCLH                         *(unsigned long*)(I2C_BLOCK_0 + 0x10)              // I2C 0 SCH Duty Cycle Register High Half Word
#define I2C0SCLL                         *(unsigned long*)(I2C_BLOCK_0 + 0x14)              // I2C 0 SCH Duty Cycle Register Low Half Word
#define I2C0CONCLR                       *(volatile unsigned long*)(I2C_BLOCK_0 + 0x18)     // I2C 0 Control Clear Register (Write Only)
  #define IIC_AAC                        0x00000004                                         // Assert Acknowledge Clear Bit
  #define IIC_SIC                        0x00000008                                         // I2C Interrupt Clear Bit
  #define IIC_STAC                       0x00000020                                         // Start Flag Clear Bit
  #define IIC_I2ENC                      0x00000040                                         // Interface Enable Disable Bit

#define I2C1CONSET                       *(volatile unsigned long*)(I2C_BLOCK_1 + 0x00)     // I2C 1 Control Set Register
#define I2C1STAT                         *(volatile unsigned long*)(I2C_BLOCK_1 + 0x04)     // I2C 1 Status Register (Read Only)
#define I2C1DAT                          *(volatile unsigned long*)(I2C_BLOCK_1 + 0x08)     // I2C 1 Data Register
#define I2C1ADR                          *(unsigned long*)(I2C_BLOCK_1 + 0x0c)              // I2C 1 Slave Address Register
#define I2C1SCLH                         *(unsigned long*)(I2C_BLOCK_1 + 0x10)              // I2C 1 SCH Duty Cycle Register High Half Word
#define I2C1SCLL                         *(unsigned long*)(I2C_BLOCK_1 + 0x14)              // I2C 1 SCH Duty Cycle Register Low Half Word
#define I2C1CONCLR                       *(volatile unsigned long*)(I2C_BLOCK_1 + 0x18)     // I2C 1 Control Clear Register (Write Only)

#define I2C2CONSET                       *(volatile unsigned long*)(I2C_BLOCK_2 + 0x00)     // I2C 2 Control Set Register
#define I2C2STAT                         *(volatile unsigned long*)(I2C_BLOCK_2 + 0x04)     // I2C 2 Status Register (Read Only)
#define I2C2DAT                          *(volatile unsigned long*)(I2C_BLOCK_2 + 0x08)     // I2C 2 Data Register
#define I2C2ADR                          *(unsigned long*)(I2C_BLOCK_2 + 0x0c)              // I2C 2 Slave Address Register
#define I2C2SCLH                         *(unsigned long*)(I2C_BLOCK_2 + 0x10)              // I2C 2 SCH Duty Cycle Register High Half Word
#define I2C2SCLL                         *(unsigned long*)(I2C_BLOCK_2 + 0x14)              // I2C 2 SCH Duty Cycle Register Low Half Word
#define I2C2CONCLR                       *(volatile unsigned long*)(I2C_BLOCK_2 + 0x18)     // I2C 2 Control Clear Register (Write Only)

typedef struct stLPCXXX_IIC_CONTROL
{
volatile unsigned long I2CCONSET;
volatile unsigned long I2CSTAT;
volatile unsigned long I2CDAT;
unsigned long I2CADR;
unsigned long I2CSCLH;
unsigned long I2CSCLL;
volatile unsigned long I2CCONCLR;
} LPCXXX_IIC_CONTROL;

// SPI                                                                   {28}
//
#define S0SPCR                           *(unsigned long*)(SPI_BLOCK_0 + 0x00)              // SPI Control Register
#define S0SPSR                           *(volatile unsigned long*)(SPI_BLOCK_0 + 0x04)     // SPI Status Register (read-only)
#define S0SPDR                           *(volatile unsigned long*)(SPI_BLOCK_0 + 0x08)     // SPI Data Register
#define S0SPCCR                          *(unsigned long*)(SPI_BLOCK_0 + 0x0c)              // SPI Clock Counter Register
#define S0SPINT                          *(volatile unsigned long*)(SPI_BLOCK_0 + 0x1c)     // SPI Interrupt Flag Register

// RTC registers
//
#define RTC_ILR                          *(volatile unsigned long*)(RTC_BLOCK + 0x000)      // Interrupt Location Register
  #define RTCCIF                         0x1                                                // Counter Increment Interrupt Block generated an interript
  #define RTCALF                         0x2                                                // Alarm Registers generated an interrupt
#if !defined LPC2101 && !defined LPC2102 && !defined LPC2103
  #define RTSSF                          0x4                                                // Counter Increment Sub-Seconds interrupt block generated an interript
#endif
#define RTC_CTC                          *(volatile unsigned long*)(RTC_BLOCK + 0x004)      // Clock Tick Counter Register (Read Only)
#define RTC_CCR                          *(unsigned long*)(RTC_BLOCK + 0x008)               // Clock Control Register
  #define CCR_CLKEN                      0x01                                               // Enable counters
  #define CCR_CTCRST                     0x02                                               // Clock Tick Counter Reset
  #define CCR_CLKSRC_32K                 0x10                                               // Clock derived from external 32k source
#define RTC_CIIR                         *(unsigned long*)(RTC_BLOCK + 0x00c)               // Counter Increment Interrupt Register
  #define CIIR_IMSEC                     0x01                                               // Interrupt on second increment
  #define CIIR_IMMIN                     0x02                                               // Interrupt on minute increment
  #define CIIR_IMHOUR                    0x04                                               // Interrupt on hour increment
  #define CIIR_IMDOM                     0x08                                               // Interrupt on day of month increment
  #define CIIR_IMDOW                     0x10                                               // Interrupt on day of week increment
  #define CIIR_IMDOY                     0x20                                               // Interrupt on day of year increment
  #define CIIR_IMMON                     0x40                                               // Interrupt on month increment
  #define CIIR_IMYEAR                    0x80                                               // Interrupt on year increment
#define RTC_AMR                          *(unsigned long*)(RTC_BLOCK + 0x010)               // Alarm Mask Register
  #define AMRSEC                         0x01                                               // No alarm compare on second value
  #define AMRMIN                         0x02                                               // No alarm compare on minute value
  #define AMRHOUR                        0x04                                               // No alarm compare on hour value
  #define AMRDOM                         0x08                                               // No alarm compare on day of month value
  #define AMRDOW                         0x10                                               // No alarm compare on day of week value
  #define AMRDOY                         0x20                                               // No alarm compare on day of year value
  #define AMRMON                         0x40                                               // No alarm compare month value
  #define AMRYEAR                        0x80                                               // No alarm compare year value
#define RTC_CTIME0                       *(volatile unsigned long*)(RTC_BLOCK + 0x014)      // Consolidated Time Register 0 (seconds, minutes, hours, day of week)
#define RTC_CTIME1                       *(volatile unsigned long*)(RTC_BLOCK + 0x018)      // Consolidated Time Register 1 (day of month, month, year)
#define RTC_CTIME2                       *(volatile unsigned long*)(RTC_BLOCK + 0x01c)      // Consolidated Time Register 2 (day of year)
#define RTC_SEC                          *(volatile unsigned long*)(RTC_BLOCK + 0x020)      // Seconds 0..59
#define RTC_MIN                          *(volatile unsigned long*)(RTC_BLOCK + 0x024)      // Minutes 0..59
#define RTC_HOUR                         *(volatile unsigned long*)(RTC_BLOCK + 0x028)      // Hours 0..23
#define RTC_DOM                          *(volatile unsigned long*)(RTC_BLOCK + 0x02c)      // Day of Month 1..28/29/30/31
#define RTC_DOW                          *(volatile unsigned long*)(RTC_BLOCK + 0x030)      // Day of Week 0..6
#define RTC_DOY                          *(volatile unsigned long*)(RTC_BLOCK + 0x034)      // Day of Year 1..365/366
#define RTC_MONTH                        *(volatile unsigned long*)(RTC_BLOCK + 0x038)      // Month 1..12
#define RTC_YEAR                         *(volatile unsigned long*)(RTC_BLOCK + 0x03c)      // Year 0..4095
#if defined LPC2101 || defined LPC2102 || defined LPC2103
    #define RTC_PWRCTRL                  *(volatile unsigned long*)(RTC_BLOCK + 0x040)      // Deep power down control register
      #define RTC_PMAIN                  0x01
      #define RTC_PSRAM                  0x02
      #define RTC_POSC                   0x04
    #define RTC_GPREG0                   *(volatile unsigned long*)(RTC_BLOCK + 0x044)      // General purpose register 0
    #define RTC_GPREG1                   *(volatile unsigned long*)(RTC_BLOCK + 0x048)      // General purpose register 1
    #define RTC_GPREG2                   *(volatile unsigned long*)(RTC_BLOCK + 0x04c)      // General purpose register 2
#else
    #define RTC_CISS                     *(volatile unsigned long*)(RTC_BLOCK + 0x040)      // Counter Increment Select Mask for sub-second interrupt
      #define CISS_16_COUNTS             0x00                                               // Interrupt each 16 counts of the Clock Tick Timer
      #define CISS_32_COUNTS             0x01                                               // Interrupt each 32 counts of the Clock Tick Timer
      #define CISS_64_COUNTS             0x02                                               // Interrupt each 64 counts of the Clock Tick Timer
      #define CISS_128_COUNTS            0x03                                               // Interrupt each 128 counts of the Clock Tick Timer
      #define CISS_256_COUNTS            0x04                                               // Interrupt each 256 counts of the Clock Tick Timer
      #define CISS_512_COUNTS            0x05                                               // Interrupt each 512 counts of the Clock Tick Timer
      #define CISS_1024_COUNTS           0x06                                               // Interrupt each 1024 counts of the Clock Tick Timer
      #define CISS_2048_COUNTS           0x07                                               // Interrupt each 2048 counts of the Clock Tick Timer
      #define CISS_SUBSECENA             0x80                                               // SubSec Interrupt enabled
#endif
#define RTC_ALSEC                        *(unsigned long*)(RTC_BLOCK + 0x060)               // Alarm value for seconds
#define RTC_ALMIN                        *(unsigned long*)(RTC_BLOCK + 0x064)               // Alarm value for minutes
#define RTC_ALHOUR                       *(unsigned long*)(RTC_BLOCK + 0x068)               // Alarm value for hours
#define RTC_ALDOM                        *(unsigned long*)(RTC_BLOCK + 0x06c)               // Alarm value for day of week
#define RTC_ALDOW                        *(unsigned long*)(RTC_BLOCK + 0x070)               // Alarm value for day of month
#define RTC_ALDOY                        *(unsigned long*)(RTC_BLOCK + 0x074)               // Alarm value for day of year
#define RTC_ALMON                        *(unsigned long*)(RTC_BLOCK + 0x078)               // Alarm value for months
#define RTC_ALYEAR                       *(unsigned long*)(RTC_BLOCK + 0x07c)               // Alarm value for years
#define RTC_PREINT                       *(unsigned long*)(RTC_BLOCK + 0x080)               // Prescale Value, Integer Portion
#define RTC_PREFRAC                      *(unsigned long*)(RTC_BLOCK + 0x084)               // Prescale Value, Fractional Portion


// Legacy GPIO registers
//
#define IO0PIN                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x00)// GPIO Pin value register
#define IO0SET                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x04)// GPIO Output Set register
#define IO0DIR                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x08)// GPIO Port Direction Control Register
#define IO0CLR                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x0c)// GPIO Pin Output Clear register (Write Only)

#define IO1PIN                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x10)// GPIO Pin value register
#define IO1SET                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x14)// GPIO Output Set register
#define IO1DIR                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x18)// GPIO Port Direction Control Register
#define IO1CLR                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x1c)// GPIO Pin Output Clear register (Write Only)
                                                                                             // {4}
#define IOIntStatus                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x80)// GPIO Overall Interrupt Status (read-only) - Port 0
  #define P0int                          0x00000001                                          // At least one pending interrupt on port 0
  #define P2int                          0x00000004                                          // At least one pending interrupt on port 2
#define IOIntStatR_ADD                   (LPC23XX_PORT_INT *)(LEGACY_GPIO_BLOCK + 0x84)
#define IO0IntStatR                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x84)// GPIO Interrupt Status for Rising Edge (read-only) - Port 0
  #define PORTINT_RISING_DETECTED        0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntStatF                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x88)// GPIO Interrupt Status for Falling Edge (read-only) - Port 0
  #define PORTINT_FALLING_DETECTED       0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntClr                        *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x8c)// GPIO Interrupt Clear (write-only) - Port 0
  #define PORTINT_CLEAR                  0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntEnR                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x90)         // GPIO Interrupt Enable for Rising Edge - Port 0
  #define PORTINT_RISING_ENABLE          0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntEnF                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x94)         // GPIO Interrupt Enable for Falling Edge - Port 0
  #define PORTINT_FALLING_ENABLE         0x00000001                                          // each port input has one bit - shift to the bit position to use

#define IO2IntStatR                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0xa4)// GPIO Interrupt Status for Rising Edge (read-only) - Port 0
#define IO2IntStatF                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0xa8)// GPIO Interrupt Status for Falling Edge (read-only) - Port 0
#define IO2IntClr                        *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0xac)// GPIO Interrupt Clear (write-only) - Port 0
#define IO2IntEnR                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0xb0)        // GPIO Interrupt Enable for Rising Edge - Port 0
#define IO2IntEnF                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0xb4)        // GPIO Interrupt Enable for Falling Edge - Port 0


typedef struct stLPC23XX_PORT_INT
{
unsigned long IOIntStatR;
unsigned long IOIntStatF;
unsigned long IOIntClr;
unsigned long IOIntEnR;
unsigned long IOIntEnF;
unsigned long ulRes[3];
} LPC23XX_PORT_INT;



// Pin Connect Block
//
#define PINSEL0                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x00)        // Pin Function Select Register 0
#ifdef _LPC21XX
  #ifdef _LPC214X                                                        // {16}
      #define PINSEL0_P0_0_GPIO              0x00000000
      #define PINSEL0_P0_0_TXD0              0x00000001
      #define PINSEL0_P0_0_PWM1              0x00000002
      #define PINSEL0_P0_0_RESET             0x00000003
      #define PINSEL0_P0_1_GPIO              0x00000000
      #define PINSEL0_P0_1_RXD0              0x00000004
      #define PINSEL0_P0_1_PWM3              0x00000008
      #define PINSEL0_P0_1_EINT0             0x0000000c
      #define PINSEL0_P0_1_RESET             0x0000000c
      #define PINSEL0_P0_2_GPIO              0x00000000
      #define PINSEL0_P0_2_SCL0              0x00000010
      #define PINSEL0_P0_2_CAP0_0            0x00000020
      #define PINSEL0_P0_2_RESET             0x00000030
      #define PINSEL0_P0_3_GPIO              0x00000000
      #define PINSEL0_P0_3_SDA0              0x00000040
      #define PINSEL0_P0_3_MAT0_0            0x00000080
      #define PINSEL0_P0_3_EINT1             0x000000c0
      #define PINSEL0_P0_3_RESET             0x000000c0
      #define PINSEL0_P0_4_GPIO              0x00000000
      #define PINSEL0_P0_4_SCK               0x00000100
      #define PINSEL0_P0_4_CAP0_1            0x00000200
      #define PINSEL0_P0_4_AD0_6             0x00000300
      #define PINSEL0_P0_4_RESET             0x00000300
      #define PINSEL0_P0_5_GPIO              0x00000000
      #define PINSEL0_P0_5_MISO_0            0x00000400
      #define PINSEL0_P0_5_MAT0_1            0x00000800
      #define PINSEL0_P0_5_AD0_7             0x00000c00
      #define PINSEL0_P0_5_RESET             0x00000c00
      #define PINSEL0_P0_6_GPIO              0x00000000
      #define PINSEL0_P0_6_MOSI_0            0x00001000
      #define PINSEL0_P0_6_CAP0_2            0x00002000
      #define PINSEL0_P0_6_AD1_0             0x00003000
      #define PINSEL0_P0_6_RESET             0x00003000
      #define PINSEL0_P0_7_GPIO              0x00000000
      #define PINSEL0_P0_7_SSEL_0            0x00004000
      #define PINSEL0_P0_7_PWM2              0x00008000
      #define PINSEL0_P0_7_EINT2             0x0000c000
      #define PINSEL0_P0_7_RESET             0x0000c000
      #define PINSEL0_P0_8_GPIO              0x00000000
      #define PINSEL0_P0_8_TXD1              0x00010000
      #define PINSEL0_P0_8_PWM4              0x00020000
      #define PINSEL0_P0_8_AD1_1             0x00030000
      #define PINSEL0_P0_8_RESET             0x00030000
      #define PINSEL0_P0_9_GPIO              0x00000000
      #define PINSEL0_P0_9_RXD1              0x00040000
      #define PINSEL0_P0_9_PWM6              0x00080000
      #define PINSEL0_P0_9_EINT3             0x000c0000
      #define PINSEL0_P0_9_RESET             0x000c0000
      #define PINSEL0_P0_10_GPIO             0x00000000
      #define PINSEL0_P0_10_RTS1             0x00100000
      #define PINSEL0_P0_10_CAP1_0           0x00200000
      #define PINSEL0_P0_10_AD1_2            0x00300000
      #define PINSEL0_P0_10_RESET            0x00300000
      #define PINSEL0_P0_11_GPIO             0x00000000
      #define PINSEL0_P0_11_CTS1             0x00400000
      #define PINSEL0_P0_11_CAP1_1           0x00800000
      #define PINSEL0_P0_11_SCL1             0x00c00000
      #define PINSEL0_P0_11_RESET            0x00c00000
      #define PINSEL0_P0_12_GPIO             0x00000000
      #define PINSEL0_P0_12_DSR1             0x02000000
      #define PINSEL0_P0_12_MAT1_0           0x03000000
      #define PINSEL0_P0_12_AD1_3            0x03000000
      #define PINSEL0_P0_12_RESET            0x03000000
      #define PINSEL0_P0_13_GPIO             0x00000000
      #define PINSEL0_P0_13_DTR1             0x04000000
      #define PINSEL0_P0_13_MAT1_1           0x08000000
      #define PINSEL0_P0_13_AD1_4            0x0c000000
      #define PINSEL0_P0_13_RESET            0x0c000000
      #define PINSEL0_P0_14_GPIO             0x00000000
      #define PINSEL0_P0_14_CD1              0x10000000
      #define PINSEL0_P0_14_EINT1            0x20000000
      #define PINSEL0_P0_14_SDA1             0x30000000
      #define PINSEL0_P0_14_RESET            0x30000000
      #define PINSEL0_P0_15_GPIO             0x00000000
      #define PINSEL0_P0_15_RI1              0x40000000
      #define PINSEL0_P0_15_EINT2            0x80000000
      #define PINSEL0_P0_15_AD1_5            0xc0000000
      #define PINSEL0_P0_15_RESET            0xc0000000
  #else
      #define PINSEL0_P0_0_GPIO              0x00000000
      #define PINSEL0_P0_0_TXD0              0x00000001
      #define PINSEL0_P0_0_PWM1              0x00000002
      #define PINSEL0_P0_0_MAT3_1            0x00000002
      #define PINSEL0_P0_0_RESET             0x00000003
      #define PINSEL0_P0_1_GPIO              0x00000000
      #define PINSEL0_P0_1_RXD0              0x00000004
      #define PINSEL0_P0_1_PWM3              0x00000008
      #define PINSEL0_P0_1_MAT3_2            0x00000008
      #define PINSEL0_P0_1_RESET             0x0000000c
      #define PINSEL0_P0_2_GPIO              0x00000000
      #define PINSEL0_P0_2_SCL0              0x00000010
      #define PINSEL0_P0_2_CAP0_0            0x00000020
      #define PINSEL0_P0_2_RESET             0x00000030
      #define PINSEL0_P0_3_GPIO              0x00000000
      #define PINSEL0_P0_3_SDA0              0x00000040
      #define PINSEL0_P0_3_MAT0_0            0x00000080
      #define PINSEL0_P0_3_RESET             0x000000c0
      #define PINSEL0_P0_4_GPIO              0x00000000
      #define PINSEL0_P0_4_SCK               0x00000100
      #define PINSEL0_P0_4_CAP0_1            0x00000200
      #define PINSEL0_P0_4_RESET             0x00000300
      #define PINSEL0_P0_5_GPIO              0x00000000
      #define PINSEL0_P0_5_MISO              0x00000400
      #define PINSEL0_P0_5_MAT0_1            0x00000800
      #define PINSEL0_P0_5_RESET             0x00000c00
      #define PINSEL0_P0_6_GPIO              0x00000000
      #define PINSEL0_P0_6_MOSI              0x00001000
      #define PINSEL0_P0_6_CAP0_2            0x00002000
      #define PINSEL0_P0_6_RESET             0x00003000
      #define PINSEL0_P0_7_GPIO              0x00000000
      #define PINSEL0_P0_7_SSEL              0x00004000
      #define PINSEL0_P0_7_PWM2              0x00008000
      #define PINSEL0_P0_7_MAT2_0            0x00008000
      #define PINSEL0_P0_7_RESET             0x0000c000
      #define PINSEL0_P0_8_GPIO              0x00000000
      #define PINSEL0_P0_8_TXD1              0x00010000
      #define PINSEL0_P0_8_PWM4              0x00020000
      #define PINSEL0_P0_8_MAT2_1            0x00020000
      #define PINSEL0_P0_8_RESET             0x00030000
      #define PINSEL0_P0_9_GPIO              0x00000000
      #define PINSEL0_P0_9_RXD1              0x00040000
      #define PINSEL0_P0_9_PWM6              0x00080000
      #define PINSEL0_P0_9_MAT2_2            0x00080000
      #define PINSEL0_P0_9_RESET             0x000c0000
      #define PINSEL0_P0_10_GPIO             0x00000000
      #define PINSEL0_P0_10_RTS1             0x00100000
      #define PINSEL0_P0_10_CAP1_0           0x00200000
      #define PINSEL0_P0_10_RESET            0x00300000
      #define PINSEL0_P0_11_GPIO             0x00000000
      #define PINSEL0_P0_11_CTS1             0x00400000
      #define PINSEL0_P0_11_CAP1_1           0x00800000
      #define PINSEL0_P0_11_RESET            0x00c00000
      #define PINSEL0_P0_12_GPIO             0x00000000
      #define PINSEL0_P0_12_DSR1             0x02000000
      #define PINSEL0_P0_12_MAT1_0           0x03000000
      #define PINSEL0_P0_12_RESET            0x03000000
      #define PINSEL0_P0_13_GPIO             0x00000000
      #define PINSEL0_P0_13_DTR1             0x04000000
      #define PINSEL0_P0_13_MAT1_1           0x08000000
      #define PINSEL0_P0_13_RESET            0x0c000000
      #define PINSEL0_P0_14_GPIO             0x00000000
      #define PINSEL0_P0_14_CD1              0x10000000
      #define PINSEL0_P0_14_EINT1            0x20000000
      #define PINSEL0_P0_14_RESET            0x30000000
      #define PINSEL0_P0_15_GPIO             0x00000000
      #define PINSEL0_P0_15_RI1              0x40000000
      #define PINSEL0_P0_15_EINT2            0x80000000
      #define PINSEL0_P0_15_RESET            0xc0000000
    #endif
#else
  #define PINSEL0_P0_0_GPIO              0x00000000
  #define PINSEL0_P0_0_RD1               0x00000001
  #define PINSEL0_P0_0_TXD3              0x00000002
  #define PINSEL0_P0_0_SDA1              0x00000003
  #define PINSEL0_P0_0_RESET             0x00000003
  #define PINSEL0_P0_1_GPIO              0x00000000
  #define PINSEL0_P0_1_TD1               0x00000004
  #define PINSEL0_P0_1_RXD3              0x00000008
  #define PINSEL0_P0_1_SCL1              0x0000000c
  #define PINSEL0_P0_1_RESET             0x0000000c
  #define PINSEL0_P0_2_GPIO              0x00000000
  #define PINSEL0_P0_2_TXD0              0x00000010
  #define PINSEL0_P0_2_RESET             0x00000030
  #define PINSEL0_P0_3_GPIO              0x00000000
  #define PINSEL0_P0_3_RXD0              0x00000040
  #define PINSEL0_P0_3_RESET             0x000000c0
  #define PINSEL0_P0_4_GPIO              0x00000000
  #define PINSEL0_P0_4_I2SRX_CLK         0x00000100
  #define PINSEL0_P0_4_LCD               0x00000100                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL0_P0_4_RD2               0x00000200
  #define PINSEL0_P0_4_CAP2_0            0x00000300
  #define PINSEL0_P0_4_RESET             0x00000300
  #define PINSEL0_P0_5_GPIO              0x00000000
  #define PINSEL0_P0_5_I2SRX_WS          0x00000400
  #define PINSEL0_P0_5_LCD               0x00000400                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL0_P0_5_TD2               0x00000800
  #define PINSEL0_P0_5_CAP2_1            0x00000c00
  #define PINSEL0_P0_5_RESET             0x00000c00
  #define PINSEL0_P0_6_GPIO              0x00000000
  #define PINSEL0_P0_6_I2SRX_SDA         0x00001000
  #define PINSEL0_P0_6_LCD               0x00001000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL0_P0_6_SSEL1             0x00002000
  #define PINSEL0_P0_6_MAT2_1            0x00003000
  #define PINSEL0_P0_6_RESET             0x00003000
  #define PINSEL0_P0_7_GPIO              0x00000000
  #define PINSEL0_P0_7_I2STX_CLK         0x00004000
  #define PINSEL0_P0_7_LCD               0x00004000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL0_P0_7_SCK1              0x00008000
  #define PINSEL0_P0_7_MAT2_1            0x0000c000
  #define PINSEL0_P0_7_RESET             0x0000c000
  #define PINSEL0_P0_8_GPIO              0x00000000
  #define PINSEL0_P0_8_I2STX_WS          0x00010000
  #define PINSEL0_P0_8_LCD               0x00010000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL0_P0_8_MISO1             0x00020000
  #define PINSEL0_P0_8_MAT2_2            0x00030000
  #define PINSEL0_P0_8_RESET             0x00030000
  #define PINSEL0_P0_9_GPIO              0x00000000
  #define PINSEL0_P0_9_I2STX_SDA         0x00040000
  #define PINSEL0_P0_9_LCD               0x00040000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL0_P0_9_MOSI1             0x00080000
  #define PINSEL0_P0_9_MAT2_3            0x000c0000
  #define PINSEL0_P0_9_RESET             0x000c0000
  #define PINSEL0_P0_10_GPIO             0x00000000
  #define PINSEL0_P0_10_TXD2             0x00100000
  #define PINSEL0_P0_10_SDA2             0x00200000
  #define PINSEL0_P0_10_MAT3_0           0x00300000
  #define PINSEL0_P0_10_RESET            0x00300000
  #define PINSEL0_P0_11_GPIO             0x00000000
  #define PINSEL0_P0_11_RXD2             0x00400000
  #define PINSEL0_P0_11_SCL2             0x00800000
  #define PINSEL0_P0_11_MAT3_1           0x00c00000
  #define PINSEL0_P0_11_RESET            0x00c00000
  #define PINSEL0_P0_12_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_12_USB_PPWR2        0x01000000                      // only LPC24xx
  #define PINSEL0_P0_12_MISO1            0x02000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_12_AD0_6            0x03000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_12_RESET            0x03000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_13_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_13_USB_UP_LED2      0x04000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_13_MOSI1            0x08000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_13_AD0_7            0x0c000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_13_RESET            0x0c000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_14_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_14_USB_HSTEN2       0x10000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_14_USB_CONNECT2     0x20000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_14_SSEL2            0x30000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_14_RESET            0x30000000                      // only LPC2378/LPC24xx
  #define PINSEL0_P0_15_GPIO             0x00000000
  #define PINSEL0_P0_15_TXD1             0x40000000
  #define PINSEL0_P0_15_SCK0             0x80000000
  #define PINSEL0_P0_15_SCK              0xc0000000
  #define PINSEL0_P0_15_RESET            0xc0000000
#endif

#define PINSEL1                          *(unsigned long*)(PIN_CONNECT_BLOCK +0x04)        // Pin Function Select Register 1
#ifdef _LPC21XX
  #ifdef _LPC214X                                                        // {16}
      #define PINSEL1_P0_16_GPIO             0x00000000
      #define PINSEL1_P0_16_EINT0            0x00000001
      #define PINSEL1_P0_16_MAT0_2           0x00000002
      #define PINSEL1_P0_16_CAP0_2           0x00000003
      #define PINSEL1_P0_16_RESET            0x00000003
      #define PINSEL1_P0_17_GPIO             0x00000000
      #define PINSEL1_P0_17_CAP1_2           0x00000004
      #define PINSEL1_P0_17_SCK1             0x00000008
      #define PINSEL1_P0_17_MAT1_2           0x0000000c
      #define PINSEL1_P0_17_RESET            0x0000000c
      #define PINSEL1_P0_18_GPIO             0x00000000
      #define PINSEL1_P0_18_CAP1_3           0x00000010
      #define PINSEL1_P0_18_MISO1            0x00000020
      #define PINSEL1_P0_18_MAT1_3           0x00000030
      #define PINSEL1_P0_18_RESET            0x00000030
      #define PINSEL1_P0_19_GPIO             0x00000000
      #define PINSEL1_P0_19_MAT1_2           0x00000040
      #define PINSEL1_P0_19_MOSI1            0x00000080
      #define PINSEL1_P0_19_CAP1_2           0x000000c0
      #define PINSEL1_P0_19_RESET            0x000000c0
      #define PINSEL1_P0_20_GPIO             0x00000000
      #define PINSEL1_P0_20_MAT1_3           0x00000100
      #define PINSEL1_P0_20_SSEL1            0x00000200
      #define PINSEL1_P0_20_EINT3            0x00000300
      #define PINSEL1_P0_20_RESET            0x00000300
      #define PINSEL1_P0_21_GPIO             0x00000000
      #define PINSEL1_P0_21_PWM5             0x00000400
      #define PINSEL1_P0_21_AD1_6            0x00000800
      #define PINSEL1_P0_21_CAP1_3           0x00000c00
      #define PINSEL1_P0_21_RESET            0x00000c00
      #define PINSEL1_P0_22_GPIO             0x00000000
      #define PINSEL1_P0_22_AD1_7            0x00001000
      #define PINSEL1_P0_22_CAP0_0           0x00002000
      #define PINSEL1_P0_22_MAT0_0           0x00003000
      #define PINSEL1_P0_22_RESET            0x00003000
      #define PINSEL1_P0_23_GPIO             0x00000000
      #define PINSEL1_P0_23_VBUS             0x00004000
      #define PINSEL1_P0_23_RESET            0x0000c000
      #define PINSEL1_P0_24_GPIO             0x00000000
      #define PINSEL1_P0_24_RESET            0x00030000
      #define PINSEL1_P0_25_GPIO             0x00000000
      #define PINSEL1_P0_25_AD0_4            0x00040000
      #define PINSEL1_P0_25_AOUT             0x00080000
      #define PINSEL1_P0_25_RESET            0x000c0000
      #define PINSEL1_P0_26_GPIO             0x00000000
      #define PINSEL1_P0_26_RESET            0x00300000
      #define PINSEL1_P0_27_GPIO             0x00000000
      #define PINSEL1_P0_27_RESET            0x00c00000
      #define PINSEL1_P0_28_GPIO             0x00000000
      #define PINSEL1_P0_28_AD0_1            0x01000000
      #define PINSEL1_P0_28_CAP0_2           0x02000000
      #define PINSEL1_P0_28_MAT0_2           0x03000000
      #define PINSEL1_P0_28_RESET            0x03000000
      #define PINSEL1_P0_29_GPIO             0x00000000
      #define PINSEL1_P0_29_AD0_2            0x04000000
      #define PINSEL1_P0_29_CAP0_3           0x08000000
      #define PINSEL1_P0_29_MAT0_3           0x0c000000
      #define PINSEL1_P0_29_RESET            0x0c000000
      #define PINSEL1_P0_30_GPIO             0x00000000
      #define PINSEL1_P0_30_AD0_3            0x10000000
      #define PINSEL1_P0_30_EINT3            0x20000000
      #define PINSEL1_P0_30_CAP0_0           0x30000000
      #define PINSEL1_P0_30_RESET            0x30000000
      #define PINSEL1_P0_31_GPIO             0x00000000
      #define PINSEL1_P0_31_UP_LED           0x40000000
      #define PINSEL1_P0_31_CONNECT          0x80000000
      #define PINSEL1_P0_31_RESET            0xc0000000
  #else
      #define PINSEL1_P0_16_GPIO             0x00000000
      #define PINSEL1_P0_16_EINT0            0x00000001
      #define PINSEL1_P0_16_MAT0_2           0x00000002
      #define PINSEL1_P0_16_RESET            0x00000003
      #define PINSEL1_P0_17_GPIO             0x00000000
    #if defined LPC2101 || defined LPC2102 || defined LPC2103            // {19}
      #define PINSEL1_P0_17_SCL1             0x00000004                  // LPC2101/2/3
      #define PINSEL1_P0_17_CAP1_2           0x00000008                  // LPC2101/2/3
    #else
      #define PINSEL1_P0_17_CAP1_2           0x00000004                  // LPC2104/5/6
    #endif
      #define PINSEL1_P0_17_RESET            0x0000000c
      #define PINSEL1_P0_18_GPIO             0x00000000
    #if defined LPC2101 || defined LPC2102 || defined LPC2103            // {19}
      #define PINSEL1_P0_18_SDA1             0x00000010                  // LPC2101/2/3
      #define PINSEL1_P0_18_CAP1_3           0x00000020                  // LPC2101/2/3
    #else
      #define PINSEL1_P0_18_CAP1_3           0x00000010                  // LPC2104/5/6
    #endif
      #define PINSEL1_P0_18_RESET            0x00000030
      #define PINSEL1_P0_19_GPIO             0x00000000
      #define PINSEL1_P0_19_MAT1_2           0x00000040
      #define PINSEL1_P0_19_RESET            0x000000c0
      #define PINSEL1_P0_20_GPIO             0x00000000
      #define PINSEL1_P0_20_MAT1_3           0x00000100
      #define PINSEL1_P0_20_RESET            0x00000300
      #define PINSEL1_P0_21_GPIO             0x00000000
      #define PINSEL1_P0_21_PWM5             0x00000400
      #define PINSEL1_P0_21_MAT3_0           0x00000400
      #define PINSEL1_P0_21_RESET            0x00000c00
      #define PINSEL1_P0_22_GPIO             0x00000000
      #define PINSEL1_P0_22_RESET            0x00003000
      #define PINSEL1_P0_23_GPIO             0x00000000
      #define PINSEL1_P0_23_RESET            0x0000c000
      #define PINSEL1_P0_24_GPIO             0x00000000
      #define PINSEL1_P0_24_RESET            0x00030000
      #define PINSEL1_P0_25_GPIO             0x00000000
      #define PINSEL1_P0_25_RESET            0x000c0000
      #define PINSEL1_P0_26_GPIO             0x00000000
      #define PINSEL1_P0_26_RESET            0x00300000
      #define PINSEL1_P0_27_GPIO             0x00000000
      #define PINSEL1_P0_27_TRST             0x00400000
      #define PINSEL1_P0_27_RESET            0x00c00000
      #define PINSEL1_P0_28_GPIO             0x00000000
      #define PINSEL1_P0_28_TMS              0x01000000
      #define PINSEL1_P0_28_RESET            0x03000000
      #define PINSEL1_P0_29_GPIO             0x00000000
      #define PINSEL1_P0_29_TCK              0x04000000
      #define PINSEL1_P0_29_RESET            0x0c000000
      #define PINSEL1_P0_30_GPIO             0x00000000
      #define PINSEL1_P0_30_TDI              0x10000000
      #define PINSEL1_P0_30_RESET            0x30000000
      #define PINSEL1_P0_31_GPIO             0x00000000
      #define PINSEL1_P0_31_TDO              0x40000000
      #define PINSEL1_P0_31_RESET            0xc0000000
  #endif
#else
  #define PINSEL1_P0_16_GPIO             0x00000000
  #define PINSEL1_P0_16_RXD1             0x00000001
  #define PINSEL1_P0_16_SSEL0            0x00000002
  #define PINSEL1_P0_16_SSEL             0x00000003
  #define PINSEL1_P0_16_RESET            0x00000003
  #define PINSEL1_P0_17_GPIO             0x00000000
  #define PINSEL1_P0_17_CTS1             0x00000004
  #define PINSEL1_P0_17_MISO0            0x00000008
  #define PINSEL1_P0_17_MISO             0x0000000c
  #define PINSEL1_P0_17_RESET            0x0000000c
  #define PINSEL1_P0_18_GPIO             0x00000000
  #define PINSEL1_P0_18_DCD1             0x00000010
  #define PINSEL1_P0_18_MOSI0            0x00000020
  #define PINSEL1_P0_18_MOSI             0x00000030
  #define PINSEL1_P0_18_RESET            0x00000030
  #define PINSEL1_P0_19_GPIO             0x00000000
  #define PINSEL1_P0_19_DSR1             0x00000040
  #define PINSEL1_P0_19_MCICLK           0x00000080
  #define PINSEL1_P0_19_SDA1             0x000000c0
  #define PINSEL1_P0_19_RESET            0x000000c0
  #define PINSEL1_P0_20_GPIO             0x00000000
  #define PINSEL1_P0_20_DTR1             0x00000100
  #define PINSEL1_P0_20_MCIMD            0x00000200
  #define PINSEL1_P0_20_SCL1             0x00000300
  #define PINSEL1_P0_20_RESET            0x00000300
  #define PINSEL1_P0_21_GPIO             0x00000000
  #define PINSEL1_P0_21_RI1              0x00000400
  #define PINSEL1_P0_21_MCIPWR           0x00000800
  #define PINSEL1_P0_21_RD1              0x00000c00
  #define PINSEL1_P0_21_RESET            0x00000c00
  #define PINSEL1_P0_22_GPIO             0x00000000
  #define PINSEL1_P0_22_RTS1             0x00001000
  #define PINSEL1_P0_22_MCIDATA0         0x00002000
  #define PINSEL1_P0_22_TD1              0x00003000
  #define PINSEL1_P0_22_RESET            0x00003000
  #define PINSEL1_P0_23_GPIO             0x00000000
  #define PINSEL1_P0_23_AD0_0            0x00004000
  #define PINSEL1_P0_23_I2SRX_CLK        0x00008000
  #define PINSEL1_P0_23_CAP3_0           0x0000c000
  #define PINSEL1_P0_23_RESET            0x0000c000
  #define PINSEL1_P0_24_GPIO             0x00000000
  #define PINSEL1_P0_24_AD0_1            0x00010000
  #define PINSEL1_P0_24_I2SRX_WS         0x00020000
  #define PINSEL1_P0_24_CAP3_1           0x00030000
  #define PINSEL1_P0_24_RESET            0x00030000
  #define PINSEL1_P0_25_GPIO             0x00000000
  #define PINSEL1_P0_25_AD0_2            0x00040000
  #define PINSEL1_P0_25_I2SRX_SDA        0x00080000
  #define PINSEL1_P0_25_TXD3             0x000c0000
  #define PINSEL1_P0_25_RESET            0x000c0000
  #define PINSEL1_P0_26_GPIO             0x00000000
  #define PINSEL1_P0_26_AD0_3            0x00100000
  #define PINSEL1_P0_26_AOUT             0x00200000
  #define PINSEL1_P0_26_RXD3             0x00300000
  #define PINSEL1_P0_26_RESET            0x00300000
  #define PINSEL1_P0_27_GPIO             0x00000000
  #define PINSEL1_P0_27_SDA0             0x00400000
  #define PINSEL1_P0_27_RESET            0x00c00000
  #define PINSEL1_P0_28_GPIO             0x00000000
  #define PINSEL1_P0_28_SCL0             0x01000000
  #define PINSEL1_P0_28_RESET            0x03000000
  #define PINSEL1_P0_29_GPIO             0x00000000
  #define PINSEL1_P0_29_U1D_PLUS         0x04000000
  #define PINSEL1_P0_29_RESET            0x0c000000
  #define PINSEL1_P0_30_GPIO             0x00000000
  #define PINSEL1_P0_30_U1D_MINUS        0x10000000
  #define PINSEL1_P0_30_RESET            0x30000000
  #define PINSEL1_P0_31_GPIO             0x00000000                      // only LPC2378/lpc24xx
  #define PINSEL1_P0_31_U2D_PLUS         0x40000000                      // only LPC2378/LPC24XX
  #define PINSEL1_P0_31_RESET            0xc0000000                      // only LPC2378/lpc24xx
#endif

#define PINSEL2                          *(unsigned long*)(PIN_CONNECT_BLOCK +0x08)        // Pin Function Select Register 2
  #define PINSEL2_P1_0_GPIO              0x00000000
  #define PINSEL2_P1_0_ENET_TXD0         0x00000001
  #define PINSEL2_P1_0_RESET             0x00000003
  #define PINSEL2_P1_1_GPIO              0x00000000
  #define PINSEL2_P1_1_ENET_TXD0         0x00000004
  #define PINSEL2_P1_1_RESET             0x0000000c
  #define PINSEL2_P1_2_GPIO              0x00000000
  #define PINSEL2_P1_2_ENET_TXD2         0x00000010                      // {26} LPC24XX
  #define PINSEL2_P1_2_MCICLK            0x00000020                      // {26} LPC24XX
  #define PINSEL2_P1_2_PWM0_1            0x00000030                      // {26} LPC24XX
  #define PINSEL2_P1_2_RESET             0x00000030
  #define PINSEL2_P1_3_GPIO              0x00000000
  #define PINSEL2_P1_3_ENET_TXD3         0x00000040                      // {26} LPC24XX
  #define PINSEL2_P1_3_MCICMD            0x00000080                      // {26} LPC24XX
  #define PINSEL2_P1_3_PWM0_2            0x000000c0                      // {26} LPC24XX
  #define PINSEL2_P1_3_RESET             0x000000c0
  #define PINSEL2_P1_4_GPIO              0x00000000
  #define PINSEL2_P1_4_ENET_TX_EN        0x00000100
  #define PINSEL2_P1_4_RESET             0x00000300
  #define PINSEL2_P1_5_GPIO              0x00000000
  #define PINSEL2_P1_5_ENET_TX_ER        0x00000400                      // {26} LPC24XX
  #define PINSEL2_P1_5_MCIPWR            0x00000800                      // {26} LPC24XX
  #define PINSEL2_P1_5_PWM0_3            0x00000c00                      // {26} LPC24XX
  #define PINSEL2_P1_5_RESET             0x00000c00
  #define PINSEL2_P1_6_GPIO              0x00000000
  #define PINSEL2_P1_6_ENET_TX_CLK       0x00001000
  #define PINSEL2_P1_6_MCIDAT0           0x00002000                      // {26} LPC24XX
  #define PINSEL2_P1_6_PWM0_4            0x00003000                      // {26} LPC24XX
  #define PINSEL2_P1_6_RESET             0x00003000
  #define PINSEL2_P1_7_GPIO              0x00000000
  #define PINSEL2_P1_7_ENET_COL          0x00004000                      // {26} LPC24XX
  #define PINSEL2_P1_7_MCIDAT1           0x00008000                      // {26} LPC24XX
  #define PINSEL2_P1_7_PWM0_5            0x0000c000                      // {26} LPC24XX
  #define PINSEL2_P1_7_RESET             0x0000c000
  #define PINSEL2_P1_8_GPIO              0x00000000
  #define PINSEL2_P1_8_ENET_CRS          0x00010000
  #define PINSEL2_P1_8_RESET             0x00030000
  #define PINSEL2_P1_9_GPIO              0x00000000
  #define PINSEL2_P1_9_ENET_RXD0         0x00040000
  #define PINSEL2_P1_9_RESET             0x000c0000
  #define PINSEL2_P1_10_GPIO             0x00000000
  #define PINSEL2_P1_10_ENET_RXD1        0x00100000
  #define PINSEL2_P1_10_RESET            0x00300000
  #define PINSEL2_P1_11_GPIO             0x00000000
  #define PINSEL2_P1_11_ENET_RXD2        0x00400000                      // {26} LPC24XX
  #define PINSEL2_P1_11_MCIDAT2          0x00800000                      // {26} LPC24XX
  #define PINSEL2_P1_11_PWM0_6           0x00c00000                      // {26} LPC24XX
  #define PINSEL2_P1_11_RESET            0x00c00000
  #define PINSEL2_P1_12_GPIO             0x00000000
  #define PINSEL2_P1_12_ENET_RXD3        0x01000000                      // {26} LPC24XX
  #define PINSEL2_P1_12_MCIDAT3          0x02000000                      // {26} LPC24XX
  #define PINSEL2_P1_12_PCAP0_0          0x03000000                      // {26} LPC24XX
  #define PINSEL2_P1_12_RESET            0x03000000
  #define PINSEL2_P1_14_GPIO             0x00000000
  #define PINSEL2_P1_14_ENET_RX_ER       0x10000000
  #define PINSEL2_P1_14_RESET            0x30000000
  #define PINSEL2_P1_15_GPIO             0x00000000
  #define PINSEL2_P1_15_ENET_REF_CLK     0x40000000
  #define PINSEL2_P1_15_RESET            0xc0000000

#define PINSEL3                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0c)        // Pin Function Select Register 3
  #define PINSEL3_P1_16_GPIO             0x00000000
  #define PINSEL3_P1_16_ENET_MDC         0x00000001
  #define PINSEL3_P1_16_RESET            0x00000003
  #define PINSEL3_P1_17_GPIO             0x00000000
  #define PINSEL3_P1_17_ENET_MDIO        0x00000004
  #define PINSEL3_P1_17_RESET            0x0000000c
  #define PINSEL3_P1_18_GPIO             0x00000000
  #define PINSEL3_P1_18_USB_UP_LED1      0x00000010
  #define PINSEL3_P1_18_PWM1_1           0x00000020
  #define PINSEL3_P1_18_CAP1_0           0x00000030
  #define PINSEL3_P1_18_RESET            0x00000030
  #define PINSEL3_P1_19_GPIO             0x00000000
  #define PINSEL3_P1_19_USB_TX_E1        0x00000040                      // only LPC24xx
  #define PINSEL3_P1_19_USB_PPWR1        0x00000080                      // only LPC24xx
  #define PINSEL3_P1_19_CAP1_1           0x000000c0
  #define PINSEL3_P1_19_RESET            0x000000c0
  #define PINSEL3_P1_20_GPIO             0x00000000
  #define PINSEL3_P1_20_USB_TX_DP1       0x00000100                      // only LPC24xx
  #define PINSEL3_P1_20_LCD              0x00000100                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_20_PWM1_2           0x00000200
  #define PINSEL3_P1_20_SCK0             0x00000300
  #define PINSEL3_P1_20_RESET            0x00000300
  #define PINSEL3_P1_21_GPIO             0x00000000
  #define PINSEL3_P1_21_USB_TX_DM1       0x00000400                      // only LPC24xx
  #define PINSEL3_P1_21_LCD              0x00000400                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_21_PWM1_3           0x00000800
  #define PINSEL3_P1_21_SSEL0            0x00000c00
  #define PINSEL3_P1_21_RESET            0x00000c00
  #define PINSEL3_P1_22_GPIO             0x00000000
  #define PINSEL3_P1_22_USB_RCV1         0x00001000                      // only LPC24xx
  #define PINSEL3_P1_22_LCD              0x00001000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_22_USB_PWRD1        0x00002000                      // only LPC24xx
  #define PINSEL3_P1_22_MAT1_0           0x00003000
  #define PINSEL3_P1_22_RESET            0x00003000
  #define PINSEL3_P1_23_GPIO             0x00000000
  #define PINSEL3_P1_23_USB_RX_DP1       0x00004000                      // only LPC24xx
  #define PINSEL3_P1_23_LCD              0x00004000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_23_PWM1_4           0x00008000
  #define PINSEL3_P1_23_MISO0            0x0000c000
  #define PINSEL3_P1_23_RESET            0x0000c000
  #define PINSEL3_P1_24_GPIO             0x00000000
  #define PINSEL3_P1_24_USB_RX_DM1       0x00010000                      // only LPC24xx
  #define PINSEL3_P1_24_LCD              0x00010000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_24_PWM1_5           0x00020000
  #define PINSEL3_P1_24_MOSI0            0x00030000
  #define PINSEL3_P1_24_RESET            0x00030000
  #define PINSEL3_P1_25_GPIO             0x00000000
  #define PINSEL3_P1_25_USB_LS1          0x00040000                      // only LPC24xx
  #define PINSEL3_P1_25_LCD              0x00040000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_25_USB_HSTEN1       0x00080000                      // only LPC24xx
  #define PINSEL3_P1_25_MAT1_1           0x000c0000
  #define PINSEL3_P1_25_RESET            0x000c0000
  #define PINSEL3_P1_26_GPIO             0x00000000
  #define PINSEL3_P1_26_USB_SSPND1       0x00100000                      // only LPC24xx
  #define PINSEL3_P1_26_LCD              0x00100000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_26_PWM1_6           0x00200000
  #define PINSEL3_P1_26_CAP0_0           0x00300000
  #define PINSEL3_P1_26_RESET            0x00300000
  #define PINSEL3_P1_27_GPIO             0x00000000
  #define PINSEL3_P1_27_USB_INT1         0x00400000                      // only LPC24xx
  #define PINSEL3_P1_27_LCD              0x00400000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_27_USB_OVRCR1       0x00800000                      // only LPC24xx
  #define PINSEL3_P1_27_CAP0_1           0x00c00000
  #define PINSEL3_P1_27_RESET            0x00c00000
  #define PINSEL3_P1_28_GPIO             0x00000000
  #define PINSEL3_P1_28_USB_SCL1         0x01000000                      // only LPC24xx
  #define PINSEL3_P1_28_LCD              0x01000000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_28_PCAP1_0          0x02000000
  #define PINSEL3_P1_28_MAT0_0           0x03000000
  #define PINSEL3_P1_28_RESET            0x03000000
  #define PINSEL3_P1_29_GPIO             0x00000000
  #define PINSEL3_P1_29_USB_SDA1         0x04000000                      // only LPC24xx
  #define PINSEL3_P1_29_LCD              0x04000000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL3_P1_29_PCAP1_1          0x08000000
  #define PINSEL3_P1_29_MAT0_1           0x0c000000
  #define PINSEL3_P1_29_RESET            0x0c000000
  #define PINSEL3_P1_30_GPIO             0x00000000
  #define PINSEL3_P1_30_USB_PWRD2        0x10000000                      // only LPC24xx
  #define PINSEL3_P1_30_VBUS             0x20000000
  #define PINSEL3_P1_30_AD0_4            0x30000000
  #define PINSEL3_P1_30_RESET            0x30000000
  #define PINSEL3_P1_31_GPIO             0x00000000                      // only LPC2378
  #define PINSEL3_P1_31_USB_OVRCR2       0x40000000                      // only LPC24XX
  #define PINSEL3_P1_31_SCK1             0x80000000                      // only LPC2378
  #define PINSEL3_P1_31_AD0_5            0xc0000000                      // only LPC2378
  #define PINSEL3_P1_31_RESET            0xc0000000                      // only LPC2378

#define PINSEL4                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x10)        // Pin Function Select Register 4
  #define PINSEL4_P2_0_GPIO              0x00000000
  #define PINSEL4_P2_0_PWM1_1            0x00000001
  #define PINSEL4_P2_0_TXD1              0x00000002
  #define PINSEL4_P2_0_TRACECLK          0x00000003
  #define PINSEL4_P2_0_LCD               0x00000003                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_0_RESET             0x00000003
  #define PINSEL4_P2_1_GPIO              0x00000000
  #define PINSEL4_P2_1_PWM1_2            0x00000004
  #define PINSEL4_P2_1_RXD1              0x00000008
  #define PINSEL4_P2_1_PIPESTAT0         0x0000000c
  #define PINSEL4_P2_1_LCD               0x0000000c                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_1_RESET             0x0000000c
  #define PINSEL4_P2_2_GPIO              0x00000000
  #define PINSEL4_P2_2_PWM1_3            0x00000010
  #define PINSEL4_P2_2_CTS1              0x00000020
  #define PINSEL4_P2_2_PIPESTAT1         0x00000030
  #define PINSEL4_P2_2_LCD               0x00000030                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_2_RESET             0x00000030
  #define PINSEL4_P2_3_GPIO              0x00000000
  #define PINSEL4_P2_3_PWM1_4            0x00000040
  #define PINSEL4_P2_3_DCD1              0x00000080
  #define PINSEL4_P2_3_PIPESTAT2         0x000000c0
  #define PINSEL4_P2_3_LCD               0x000000c0                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_3_RESET             0x000000c0
  #define PINSEL4_P2_4_GPIO              0x00000000
  #define PINSEL4_P2_4_PWM1_5            0x00000100
  #define PINSEL4_P2_4_DSR1              0x00000200
  #define PINSEL4_P2_4_TRACESYN          0x00000300
  #define PINSEL4_P2_4_LCD               0x00000300                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_4_RESET             0x00000300
  #define PINSEL4_P2_5_GPIO              0x00000000
  #define PINSEL4_P2_5_PWM1_6            0x00000400
  #define PINSEL4_P2_5_DTR1              0x00000800
  #define PINSEL4_P2_5_TRACEPKT0         0x00000c00
  #define PINSEL4_P2_5_LCD               0x00000c00                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_5_RESET             0x00000c00
  #define PINSEL4_P2_6_GPIO              0x00000000
  #define PINSEL4_P2_6_PCAP1_0           0x00001000
  #define PINSEL4_P2_6_RI1               0x00002000
  #define PINSEL4_P2_6_TRACEPKT1         0x00003000
  #define PINSEL4_P2_6_LCD               0x00003000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_6_RESET             0x00003000
  #define PINSEL4_P2_7_GPIO              0x00000000
  #define PINSEL4_P2_7_RD2               0x00004000
  #define PINSEL4_P2_7_RTS1              0x00008000
  #define PINSEL4_P2_7_TRACEPKT2         0x0000c000
  #define PINSEL4_P2_7_LCD               0x0000c000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_7_RESET             0x0000c000
  #define PINSEL4_P2_8_GPIO              0x00000000
  #define PINSEL4_P2_8_TD2               0x00010000
  #define PINSEL4_P2_8_TXD2              0x00020000
  #define PINSEL4_P2_8_TRACEPKT3         0x00030000
  #define PINSEL4_P2_8_LCD               0x00030000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_8_RESET             0x00030000
  #define PINSEL4_P2_9_GPIO              0x00000000
  #define PINSEL4_P2_9_USB_CONNECT1      0x00040000                      // not LPC2365/67
  #define PINSEL4_P2_9_RXD2              0x00080000
  #define PINSEL4_P2_9_EXTIN0            0x000c0000
  #define PINSEL4_P2_9_LCD               0x000c0000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_9_RESET             0x000c0000
  #define PINSEL4_P2_10_GPIO             0x00000000
  #define PINSEL4_P2_10_EINT0            0x00100000
  #define PINSEL4_P2_10_RESET            0x00300000
  #define PINSEL4_P2_11_GPIO             0x00000000
  #define PINSEL4_P2_11_EINT1            0x00400000
  #define PINSEL4_P2_11_LCD              0x00400000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_11_MCIDAT1          0x00800000
  #define PINSEL4_P2_11_I2STX_CLK        0x00c00000
  #define PINSEL4_P2_11_RESET            0x00c00000
  #define PINSEL4_P2_12_GPIO             0x00000000
  #define PINSEL4_P2_12_EINT2            0x01000000
  #define PINSEL4_P2_12_LCD              0x01000000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_12_MCIDAT2          0x02000000
  #define PINSEL4_P2_12_I2STX_WS         0x03000000
  #define PINSEL4_P2_12_RESET            0x03000000
  #define PINSEL4_P2_13_GPIO             0x00000000
  #define PINSEL4_P2_13_EINT3            0x04000000
  #define PINSEL4_P2_13_LCD              0x04000000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL4_P2_13_MCIDAT3          0x08000000
  #define PINSEL4_P2_13_I2STX_SDA        0x0c000000
  #define PINSEL4_P2_13_RESET            0x0c000000
  #define PINSEL4_P2_14_GPIO             0x00000000
  #define PINSEL4_P2_14_RESET            0x30000000
  #define PINSEL4_P2_15_GPIO             0x00000000
  #define PINSEL4_P2_15_RESET            0xc0000000

#define PINSEL5                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x14)        // Pin Function Select Register 5
  #define PINSEL5_P2_16_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_16_CAS              0x00000001                      // LPC24xx only
  #define PINSEL5_P2_16_RESET            0x00000003                      // LPC24xx only
  #define PINSEL5_P2_17_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_17_RAS              0x00000004                      // LPC24xx only
  #define PINSEL5_P2_17_RESET            0x0000000c                      // LPC24xx only
  #define PINSEL5_P2_18_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_18_CLKOUT0          0x00000010                      // LPC24xx only
  #define PINSEL5_P2_18_RESET            0x00000030                      // LPC24xx only
  #define PINSEL5_P2_19_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_19_CLKOUT1          0x00000040                      // LPC24xx only
  #define PINSEL5_P2_19_RESET            0x000000c0                      // LPC24xx only
  #define PINSEL5_P2_20_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_20_DYCS0            0x00000100                      // LPC24xx only
  #define PINSEL5_P2_20_RESET            0x00000300                      // LPC24xx only
  #define PINSEL5_P2_21_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_21_DYCS1            0x00000400                      // LPC24xx only
  #define PINSEL5_P2_21_RESET            0x00000c00                      // LPC24xx only
  #define PINSEL5_P2_22_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_22_DYCS2            0x00001000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_22_CAP3_0           0x00002000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_22_SCK0             0x00003000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_22_RESET            0x00003000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_23_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_23_DYCS3            0x00004000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_23_CAP3_1           0x00008000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_23_SSEL0            0x0000c000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_23_RESET            0x0000c000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_24_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_24_CKEOUT1          0x00010000                      // LPC24xx only
  #define PINSEL5_P2_24_RESET            0x00030000                      // LPC24xx only
  #define PINSEL5_P2_25_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_25_CKEOUT1          0x00040000                      // LPC24xx only
  #define PINSEL5_P2_25_RESET            0x000c0000                      // LPC24xx only
  #define PINSEL5_P2_26_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_26_CKEOUT2          0x00100000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_26_MAT3_0           0x00200000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_26_MISO0            0x00300000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_26_RESET            0x00300000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_27_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_27_CKEOUT3          0x00400000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_27_MAT3_1           0x00800000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_27_MOSI0            0x00c00000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_27_RESET            0x00c00000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_28_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_28_DQMOUT0          0x01000000                      // LPC24xx only
  #define PINSEL5_P2_28_RESET            0x03000000                      // LPC24xx only
  #define PINSEL5_P2_29_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL5_P2_29_DQMOUT1          0x04000000                      // LPC24xx only
  #define PINSEL5_P2_29_RESET            0x0c000000                      // LPC24xx only
  #define PINSEL5_P2_30_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_30_DQMOUT2          0x10000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_30_MAT3_2           0x20000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_30_SDA2             0x30000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_30_RESET            0x30000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_31_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_31_DQMOUT3          0x40000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_31_MAT3_3           0x80000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_31_SCL2             0xc0000000                      // LPC2460/68/70/78 only
  #define PINSEL5_P2_31_RESET            0xc0000000                      // LPC2460/68/70/78 only

#define PINSEL6                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x18)        // Pin Function Select Register 6
  #define PINSEL6_P3_0_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_0_D0                0x00000001                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_0_RESET             0x00000003                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_1_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_1_D1                0x00000004                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_1_RESET             0x0000000c                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_2_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_2_D2                0x00000010                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_2_RESET             0x00000030                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_3_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_3_D3                0x00000040                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_3_RESET             0x000000c0                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_4_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_4_D4                0x00000100                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_4_RESET             0x00000300                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_5_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_5_D5                0x00000400                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_5_RESET             0x00000c00                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_6_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_6_D6                0x00001000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_6_RESET             0x00003000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_7_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_7_D7                0x00004000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_7_RESET             0x0000c000                      // only LPC2378/LPC24xx
  #define PINSEL6_P3_8_GPIO              0x00000000                      // only LPC24xx
  #define PINSEL6_P3_8_D8                0x00010000                      // only LPC24xx
  #define PINSEL6_P3_8_RESET             0x00030000                      // only LPC24xx
  #define PINSEL6_P3_9_GPIO              0x00000000                      // only LPC24xx
  #define PINSEL6_P3_9_D9                0x00040000                      // only LPC24xx
  #define PINSEL6_P3_9_RESET             0x000c0000                      // only LPC24xx
  #define PINSEL6_P3_10_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL6_P3_10_D10              0x00100000                      // only LPC24xx
  #define PINSEL6_P3_10_RESET            0x00300000                      // only LPC24xx
  #define PINSEL6_P3_11_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL6_P3_11_D11              0x00400000                      // only LPC24xx
  #define PINSEL6_P3_11_RESET            0x00c00000                      // only LPC24xx
  #define PINSEL6_P3_12_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL6_P3_12_D12              0x01000000                      // only LPC24xx
  #define PINSEL6_P3_12_RESET            0x03000000                      // only LPC24xx
  #define PINSEL6_P3_13_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL6_P3_13_D13              0x04000000                      // only LPC24xx
  #define PINSEL6_P3_13_RESET            0x0c000000                      // only LPC24xx
  #define PINSEL6_P3_14_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL6_P3_14_D14              0x10000000                      // only LPC24xx
  #define PINSEL6_P3_14_RESET            0x30000000                      // only LPC24xx
  #define PINSEL6_P3_15_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL6_P3_15_D15              0x40000000                      // only LPC24xx
  #define PINSEL6_P3_15_RESET            0xc0000000                      // only LPC24xx

#define PINSEL7                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1c)        // Pin Function Select Register 7
  #define PINSEL7_P3_23_GPIO             0x00000000                      // only LPC2378
  #define PINSEL7_P3_23_CAP0_0           0x00008000                      // only LPC2378
  #define PINSEL7_P3_23_PCAP1_0          0x0000c000                      // only LPC2378
  #define PINSEL7_P3_23_RESET            0x0000c000                      // only LPC2378
  #define PINSEL7_P3_24_GPIO             0x00000000                      // only LPC2378
  #define PINSEL7_P3_24_CAP0_1           0x00020000                      // only LPC2378
  #define PINSEL7_P3_24_PCAP1_1          0x00030000                      // only LPC2378
  #define PINSEL7_P3_24_RESET            0x00030000                      // only LPC2378
  #define PINSEL7_P3_25_GPIO             0x00000000
  #define PINSEL7_P3_25_MAT0_1           0x00080000
  #define PINSEL7_P3_25_PWM1_2           0x000c0000
  #define PINSEL7_P3_25_RESET            0x000c0000
  #define PINSEL7_P3_26_GPIO             0x00000000
  #define PINSEL7_P3_26_MAT0_1           0x00200000
  #define PINSEL7_P3_26_PWM1_3           0x00300000
  #define PINSEL7_P3_26_RESET            0x00300000

#define PINSEL8                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x20)        // Pin Function Select Register 8
  #define PINSEL8_P4_0_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_0_A0                0x00000001                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_0_RESET             0x00000003                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_1_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_1_A1                0x00000004                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_1_RESET             0x0000000c                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_2_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_2_A2                0x00000010                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_2_RESET             0x00000030                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_3_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_3_A3                0x00000040                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_3_RESET             0x000000c0                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_4_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_4_A4                0x00000100                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_4_RESET             0x00000300                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_5_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_5_A5                0x00000400                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_5_RESET             0x00000c00                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_6_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_6_A6                0x00001000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_6_RESET             0x00003000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_7_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_7_A7                0x00004000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_7_RESET             0x0000c000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_8_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_8_A8                0x00010000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_8_RESET             0x00030000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_9_GPIO              0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_9_A9                0x00040000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_9_RESET             0x000c0000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_10_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_10_A10              0x00100000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_10_RESET            0x00300000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_11_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_11_A11              0x00400000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_11_RESET            0x00c00000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_12_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_12_A12              0x01000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_12_RESET            0x03000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_13_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_13_A13              0x04000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_13_RESET            0x0c000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_14_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_14_A14              0x10000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_14_RESET            0x30000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_15_GPIO             0x00000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_15_A15              0x40000000                      // only LPC2378/LPC24xx
  #define PINSEL8_P4_15_RESET            0xc0000000                      // only LPC2378/LPC24xx

#define PINSEL9                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x24)        // Pin Function Select Register 9
  #define PINSEL9_P4_16_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL9_P4_16_A16              0x00000001                      // LPC24xx only
  #define PINSEL9_P4_16_RESET            0x00000003                      // LPC24xx only
  #define PINSEL9_P4_17_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL9_P4_17_A17              0x00000004                      // LPC24xx only
  #define PINSEL9_P4_17_RESET            0x0000000c                      // LPC24xx only
  #define PINSEL9_P4_18_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL9_P4_18_A18              0x00000010                      // LPC24xx only
  #define PINSEL9_P4_18_RESET            0x00000030                      // LPC24xx only
  #define PINSEL9_P4_19_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL9_P4_19_A19              0x00000040                      // LPC24xx only
  #define PINSEL9_P4_19_RESET            0x000000c0                      // LPC24xx only
  #define PINSEL9_P4_20_GPIO             0x00000000                      // LPC24xx only
  #define PINSEL9_P4_20_A20              0x00000100                      // LPC24xx only
  #define PINSEL9_P4_20_SDA2             0x00000200                      // LPC24xx only
  #define PINSEL9_P4_20_SCK1             0x00000300                      // LPC24xx only
  #define PINSEL9_P4_20_RESET            0x00000300                      // LPC24xx only
  #define PINSEL9_P4_21_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_21_A21              0x00000400                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_21_SCL2             0x00000800                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_21_SSEL1            0x00000c00                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_21_RESET            0x00000c00                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_22_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_22_A22              0x00001000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_22_TXD2             0x00002000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_22_MISO1            0x00003000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_22_RESET            0x00003000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_23_GPIO             0x00000000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_23_A23              0x00004000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_23_RXD2             0x00008000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_23_MOSO1            0x0000c000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_23_RESET            0x0000c000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_24_GPIO             0x00000000                      // only LPC2378/LPC24xx
#ifdef _LPC24XX
  #define PINSEL9_P4_24_OE               0x00010000                      // only LPC24xx
#else
  #define PINSEL9_P4_24_A24              0x00010000                      // only LPC2378
  #define PINSEL9_P4_24_OE               0x00020000                      // only LPC2378
#endif
  #define PINSEL9_P4_24_RESET            0x00030000                      // only LPC2378/LPC24xx
  #define PINSEL9_P4_25_GPIO             0x00000000                      // only LPC2378/LPC24xx
#ifdef _LPC24XX
  #define PINSEL9_P4_25_WE               0x00040000                      // only LPC24xx
#else
  #define PINSEL9_P4_25_A25              0x00040000                      // only LPC2378
  #define PINSEL9_P4_25_WE               0x00080000                      // only LPC2378
#endif
  #define PINSEL9_P4_25_RESET            0x000c0000                      // only LPC2378/LPC24xx
  #define PINSEL9_P4_26_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL9_P4_26_BLS0             0x00100000                      // only LPC24xx
  #define PINSEL9_P4_26_RESET            0x00300000                      // only LPC24xx
  #define PINSEL9_P4_27_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL9_P4_27_BLS1             0x00400000                      // only LPC24xx
  #define PINSEL9_P4_27_RESET            0x00c00000                      // only LPC24xx
  #define PINSEL9_P4_28_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL9_P4_28_BSL2             0x01000000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_28_MAT2_0           0x02000000                      // only LPC24xx
  #define PINSEL9_P4_28_LCD              0x02000000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL9_P4_28_TXD3             0x03000000                      // only LPC24xx
  #define PINSEL9_P4_28_RESET            0x03000000                      // only LPC24xx
  #define PINSEL9_P4_29_GPIO             0x00000000                      // only LPC24xx
  #define PINSEL9_P4_29_BSL3             0x04000000                      // LPC2460/68/70/78 only
  #define PINSEL9_P4_29_MAT2_1           0x08000000                      // only LPC24xx
  #define PINSEL9_P4_29_LCD              0x08000000                      // only LPC2478FET208 and LPC2478FBD208
  #define PINSEL9_P4_29_RXD3             0x0c000000                      // only LPC24xx
  #define PINSEL9_P4_29_RESET            0x0c000000                      // only LPC24xx
  #define PINSEL9_P4_30_GPIO             0x00000000                      // only LPC2378
  #define PINSEL9_P4_30_CS0              0x10000000                      // only LPC2378
  #define PINSEL9_P4_30_RESET            0x30000000                      // only LPC2378
  #define PINSEL9_P4_31_GPIO             0x00000000                      // only LPC2378
  #define PINSEL9_P4_31_CS1              0x40000000                      // only LPC2378
  #define PINSEL9_P4_31_RESET            0xc0000000                      // only LPC2378

#define PINSEL10                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x28)        // Pin Function Select Register 10
  #define GPIO_TRACE                     0x00000008                      // set if RTCK is '0' at reset - ETM signals are then available regardless of PINSEL4 content

#define PINSEL11                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x2C)        // Pin Function Select Register 11
  #define LCDPE                          0x00000001                      // enable LCD port
  #define LCD_MODE_4_BIT_MONO_STN        0x00000000
  #define LCD_MODE_8_BIT_STN             0x00000002
  #define LCD_MODE_4_BIT_STN_DUAL        0x00000004
  #define LCD_MODE_8_BIT_MONO_STN_DUAL   0x00000006
  #define LCD_MODE_COLOR_STN_DUAL        0x00000006
  #define LCD_MODE_TFT_12_BIT_4_4_4      0x00000008
  #define LCD_MODE_TFT_16_BIT_5_6_5      0x0000000a
  #define LCD_MODE_TFT_16_BIT_1_5_5_5    0x0000000c
  #define LCD_MODE_TFT_24_BIT            0x0000000e

#define PINMODE0_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x40)
#define PINMODE0                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x40)        // Pin Mode Select Register 0
#define PINMODE1                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x44)        // Pin Mode Select Register 1
#define PINMODE2                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x48)        // Pin Mode Select Register 2
#define PINMODE3                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x4c)        // Pin Mode Select Register 3
#define PINMODE4                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x50)        // Pin Mode Select Register 4
#define PINMODE5                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x54)        // Pin Mode Select Register 5
#define PINMODE6                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x58)        // Pin Mode Select Register 6
#define PINMODE7                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x5c)        // Pin Mode Select Register 7
#define PINMODE8                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x60)        // Pin Mode Select Register 8
#define PINMODE9                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x64)        // Pin Mode Select Register 9
  #define PINMODE_PULLUP_0               0x00000000
  #define PINMODE_NO_PULLS_0             0x00000002
  #define PINMODE_PULLDOWN_0             0x00000003
  #define PINMODE_PULLUP_1               0x00000000
  #define PINMODE_NO_PULLS_1             0x00000008
  #define PINMODE_PULLDOWN_1             0x0000000c
  #define PINMODE_PULLUP_2               0x00000000
  #define PINMODE_NO_PULLS_2             0x00000020
  #define PINMODE_PULLDOWN_2             0x00000030
  #define PINMODE_PULLUP_3               0x00000000
  #define PINMODE_NO_PULLS_3             0x00000080
  #define PINMODE_PULLDOWN_3             0x000000c0
  #define PINMODE_PULLUP_4               0x00000000
  #define PINMODE_NO_PULLS_4             0x00000200
  #define PINMODE_PULLDOWN_4             0x00000300
  #define PINMODE_PULLUP_5               0x00000000
  #define PINMODE_NO_PULLS_5             0x00000800
  #define PINMODE_PULLDOWN_5             0x00000c00
  #define PINMODE_PULLUP_6               0x00000000
  #define PINMODE_NO_PULLS_6             0x00002000
  #define PINMODE_PULLDOWN_6             0x00003000
  #define PINMODE_PULLUP_7               0x00000000
  #define PINMODE_NO_PULLS_7             0x00008000
  #define PINMODE_PULLDOWN_7             0x0000c000
  #define PINMODE_PULLUP_8               0x00000000
  #define PINMODE_NO_PULLS_8             0x00020000
  #define PINMODE_PULLDOWN_8             0x00030000
  #define PINMODE_PULLUP_9               0x00000000
  #define PINMODE_NO_PULLS_9             0x00080000
  #define PINMODE_PULLDOWN_9             0x000c0000
  #define PINMODE_PULLUP_10              0x00000000
  #define PINMODE_NO_PULLS_10            0x00200000
  #define PINMODE_PULLDOWN_10            0x00300000
  #define PINMODE_PULLUP_11              0x00000000
  #define PINMODE_NO_PULLS_11            0x00800000
  #define PINMODE_PULLDOWN_11            0x00c00000
  #define PINMODE_PULLUP_12              0x00000000
  #define PINMODE_NO_PULLS_12            0x02000000
  #define PINMODE_PULLDOWN_12            0x03000000
  #define PINMODE_PULLUP_13              0x00000000
  #define PINMODE_NO_PULLS_13            0x08000000
  #define PINMODE_PULLDOWN_13            0x0c000000
  #define PINMODE_PULLUP_14              0x00000000
  #define PINMODE_NO_PULLS_14            0x20000000
  #define PINMODE_PULLDOWN_14            0x30000000
  #define PINMODE_PULLUP_15              0x00000000
  #define PINMODE_NO_PULLS_15            0x80000000
  #define PINMODE_PULLDOWN_15            0xc0000000
  #define PINMODE_PULLUP_16              0x00000000
  #define PINMODE_NO_PULLS_16            0x00000002
  #define PINMODE_PULLDOWN_16            0x00000003
  #define PINMODE_PULLUP_17              0x00000000
  #define PINMODE_NO_PULLS_17            0x00000008
  #define PINMODE_PULLDOWN_17            0x0000000c
  #define PINMODE_PULLUP_18              0x00000000
  #define PINMODE_NO_PULLS_18            0x00000020
  #define PINMODE_PULLDOWN_18            0x00000030
  #define PINMODE_PULLUP_19              0x00000000
  #define PINMODE_NO_PULLS_19            0x00000080
  #define PINMODE_PULLDOWN_19            0x000000c0
  #define PINMODE_PULLUP_20              0x00000000
  #define PINMODE_NO_PULLS_20            0x00000200
  #define PINMODE_PULLDOWN_20            0x00000300
  #define PINMODE_PULLUP_21              0x00000000
  #define PINMODE_NO_PULLS_21            0x00000800
  #define PINMODE_PULLDOWN_21            0x00000c00
  #define PINMODE_PULLUP_22              0x00000000
  #define PINMODE_NO_PULLS_22            0x00002000
  #define PINMODE_PULLDOWN_22            0x00003000
  #define PINMODE_PULLUP_23              0x00000000
  #define PINMODE_NO_PULLS_23            0x00008000
  #define PINMODE_PULLDOWN_23            0x0000c000
  #define PINMODE_PULLUP_24              0x00000000
  #define PINMODE_NO_PULLS_24            0x00020000
  #define PINMODE_PULLDOWN_24            0x00030000
  #define PINMODE_PULLUP_25              0x00000000
  #define PINMODE_NO_PULLS_25            0x00080000
  #define PINMODE_PULLDOWN_25            0x000c0000
  #define PINMODE_PULLUP_26              0x00000000
  #define PINMODE_NO_PULLS_26            0x00200000
  #define PINMODE_PULLDOWN_26            0x00300000
  #define PINMODE_PULLUP_27              0x00000000
  #define PINMODE_NO_PULLS_27            0x00800000
  #define PINMODE_PULLDOWN_27            0x00c00000
  #define PINMODE_PULLUP_28              0x00000000
  #define PINMODE_NO_PULLS_28            0x02000000
  #define PINMODE_PULLDOWN_28            0x03000000
  #define PINMODE_PULLUP_29              0x00000000
  #define PINMODE_NO_PULLS_29            0x08000000
  #define PINMODE_PULLDOWN_29            0x0c000000
  #define PINMODE_PULLUP_30              0x00000000
  #define PINMODE_NO_PULLS_30            0x20000000
  #define PINMODE_PULLDOWN_30            0x30000000
  #define PINMODE_PULLUP_31              0x00000000
  #define PINMODE_NO_PULLS_31            0x80000000
  #define PINMODE_PULLDOWN_31            0xc0000000

// Vectored Interrupt Controller
//
#define VICIRQStatus                     *(volatile unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x00) // IRQ Status Register (Read Only)
#define VICFIQStatus                     *(volatile unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x04) // FIQ Status Register (Read Only)
#define VICRawIntr                       *(volatile unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x08) // Raw Interrupt Status Register (Read Only)
    #define INTERRUPT_WDT                0x00000001
    #define INTERRUPT_ARM_CORE_RX        0x00000004
    #define INTERRUPT_ARM_CORE_TX        0x00000008
    #define INTERRUPT_TIMER0             0x00000010
    #define INTERRUPT_TIMER1             0x00000020
    #define INTERRUPT_UART0              0x00000040
    #define INTERRUPT_UART1              0x00000080
    #define INTERRUPT_PWM1               0x00000100
    #define INTERRUPT_I2C0               0x00000200
    #define INTERRUPT_SSP0               0x00000400
    #define INTERRUPT_SSP1               0x00000800
    #define INTERRUPT_PLL                0x00001000
    #define INTERRUPT_RTC                0x00002000
    #define INTERRUPT_EINT0              0x00004000
    #define INTERRUPT_EINT1              0x00008000
    #define INTERRUPT_EINT2              0x00010000
    #define INTERRUPT_EINT3              0x00020000
    #define INTERRUPT_ADC0               0x00040000
    #define INTERRUPT_I2C1               0x00080000
    #define INTERRUPT_BOD                0x00100000
    #define INTERRUPT_ETHERNET           0x00200000
    #define INTERRUPT_USB                0x00400000
    #define INTERRUPT_CAN                0x00800000
    #define INTERRUPT_SD_MMC             0x01000000
    #define INTERRUPT_GP_DMA             0x02000000
    #define INTERRUPT_TIMER2             0x04000000
    #define INTERRUPT_TIMER3             0x08000000
    #define INTERRUPT_UART2              0x10000000
    #define INTERRUPT_UART3              0x20000000
    #define INTERRUPT_I2C2               0x40000000
    #define INTERRUPT_I2S                0x80000000
#define VICIntSelect                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x0c)     // Interrupt Select Register
#define VICIntEnable                     *(volatile unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x10) // Interrupt Enable Register
#define VICIntEnClr                      *(volatile unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x14) // Interrupt Enable Clear Register (Write Only)
#define VICSoftInt                       *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x18)     // Software Interrupt Register
#define VICSoftIntClear                  *(volatile unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x1c) // Software Interrupt Clear Register (Write Only)
#define VICProtection                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x20)     // Protection Enable Register
#define VICSWPriorityMask                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x24)     // Software Priority Mask Register

#define VICVectAddr                      *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x30)     // vector address register for LPC21XX devices
#define VICDefVectAddr                   *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x34)     // default vector address register for LPC21XX devices

#define VICVECT_ADD                      (unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x100)     // address of start of table
#define VICVectAddr0                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x100)    // Vector Address 0 Register [WDT]
#define VICVectAddr1                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x104)    // Vector Address 1 Register [reserved for SW Interrupts only]
#define VICVectAddr2                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x108)    // Vector Address 2 Register [ARM_CORE_RX]
#define VICVectAddr3                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x10c)    // Vector Address 3 Register [ARM_CORE_TX]
#define VICVectAddr4                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x110)    // Vector Address 4 Register [TIMER0]
#define VICVectAddr5                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x114)    // Vector Address 5 Register [TIMER1]
#define VICVectAddr6                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x118)    // Vector Address 6 Register [UART0]
#define VICVectAddr7                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x11c)    // Vector Address 7 Register [UART1]
#define VICVectAddr8                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x120)    // Vector Address 8 Register [PWM1]
#define VICVectAddr9                     *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x124)    // Vector Address 9 Register [I2C0]
#define VICVectAddr10                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x128)    // Vector Address 10 Register [SSP0]
#define VICVectAddr11                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x12c)    // Vector Address 11 Register [SSP1]
#define VICVectAddr12                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x130)    // Vector Address 12 Register [PLL]
#define VICVectAddr13                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x134)    // Vector Address 13 Register [RTC]
#define VICVectAddr14                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x138)    // Vector Address 14 Register [EINT0]
#define VICVectAddr15                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x13c)    // Vector Address 15 Register [EINT1]
#define VICVectAddr16                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x140)    // Vector Address 16 Register [EINT2]
#define VICVectAddr17                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x144)    // Vector Address 17 Register [EINT3]
#define VICVectAddr18                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x148)    // Vector Address 18 Register [ADC0]
#define VICVectAddr19                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x14c)    // Vector Address 19 Register [I2C1]
#define VICVectAddr20                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x150)    // Vector Address 20 Register [BOD]
#define VICVectAddr21                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x154)    // Vector Address 21 Register [ETHERNET]
#define VICVectAddr22                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x158)    // Vector Address 22 Register [USB]
#define VICVectAddr23                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x15c)    // Vector Address 23 Register [CAN]
#define VICVectAddr24                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x160)    // Vector Address 24 Register [SD_MMC]
#define VICVectAddr25                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x164)    // Vector Address 25 Register [GP_DMA]
#define VICVectAddr26                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x168)    // Vector Address 26 Register [TIMER2]
#define VICVectAddr27                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x16c)    // Vector Address 27 Register [TIMER3]
#define VICVectAddr28                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x170)    // Vector Address 28 Register [UART2]
#define VICVectAddr29                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x174)    // Vector Address 29 Register [UART3]
#define VICVectAddr30                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x178)    // Vector Address 30 Register [I2C2]
#define VICVectAddr31                    *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x17c)    // Vector Address 31 Register [I2S]

#define VICPRIORITY_ADD                  (unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x200)
#define VICVectPriority0                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x200)    // Vector Priority 0 Register [WDT]
#define VICVectPriority1                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x204)    // Vector Priority 1 Register [reserved for SW Interrupts only]
#define VICVectPriority2                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x208)    // Vector Priority 2 Register [ARM_CORE_RX]
#define VICVectPriority3                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x20c)    // Vector Priority 3 Register [ARM_CORE_TX]
#define VICVectPriority4                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x210)    // Vector Priority 4 Register [TIMER0]
#define VICVectPriority5                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x214)    // Vector Priority 5 Register [TIMER1]
#define VICVectPriority6                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x218)    // Vector Priority 6 Register [UART0]
#define VICVectPriority7                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x21c)    // Vector Priority 7 Register [UART1]
#define VICVectPriority8                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x220)    // Vector Priority 8 Register [PWM1]
#define VICVectPriority9                 *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x224)    // Vector Priority 9 Register [I2C0]
#define VICVectPriority10                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x228)    // Vector Priority 10 Register [SSP0]
#define VICVectPriority11                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x22c)    // Vector Priority 11 Register [SSP1]
#define VICVectPriority12                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x230)    // Vector Priority 12 Register [PLL]
#define VICVectPriority13                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x234)    // Vector Priority 13 Register [RTC]
#define VICVectPriority14                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x238)    // Vector Priority 14 Register [EINT0]
#define VICVectPriority15                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x23c)    // Vector Priority 15 Register [EINT1]
#define VICVectPriority16                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x240)    // Vector Priority 16 Register [EINT2]
#define VICVectPriority17                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x244)    // Vector Priority 17 Register [EINT3]
#define VICVectPriority18                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x248)    // Vector Priority 18 Register [ADC0]
#define VICVectPriority19                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x24c)    // Vector Priority 19 Register [I2C1]
#define VICVectPriority20                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x250)    // Vector Priority 20 Register [BOD]
#define VICVectPriority21                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x254)    // Vector Priority 21 Register [ETHERNET]
#define VICVectPriority22                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x258)    // Vector Priority 22 Register [USB]
#define VICVectPriority23                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x25c)    // Vector Priority 23 Register [CAN]
#define VICVectPriority24                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x260)    // Vector Priority 24 Register [SD_MMC]
#define VICVectPriority25                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x264)    // Vector Priority 25 Register [GP_DMA]
#define VICVectPriority26                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x268)    // Vector Priority 26 Register [TIMER2]
#define VICVectPriority27                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x26c)    // Vector Priority 27 Register [TIMER3]
#define VICVectPriority28                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x270)    // Vector Priority 28 Register [UART2]
#define VICVectPriority29                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x274)    // Vector Priority 29 Register [UART3]
#define VICVectPriority30                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x278)    // Vector Priority 30 Register [I2C2]
#define VICVectPriority31                *(unsigned long*)(VIC_PERIPHERAL_BLOCK + 0x27c)    // Vector Priority 31 Register [I2S]

  #define VECTOR_IRQ_ENABLE              0x00000020                                         // LPC21XX

#define VICAddress                       *(volatile unsigned long*)(VIC_ADDRESS_BLOCK + 0x0)// Vector Address Register


// SD/MMC Interface
//
#define MCIPower                         *(unsigned long*)(MCI_BLOCK + 0x00)                // Power Control Register
  #define MCI_PWR_Ctrl_PowerOff          0x00000000
  #define MCI_PWR_Ctrl_PowerUp           0x00000002
  #define MCI_PWR_Ctrl_PowerOn           0x00000003
  #define MCI_PWR_OpenDrain              0x00000040
  #define MCI_PWR_Rod                    0x00000080
#define MCIClock                         *(unsigned long*)(MCI_BLOCK + 0x04)                // Clock Control Register
  #define MCI_CLK_ClkDiv                 0x000000ff                                         // MCLK divide mask MCLCLK = MCLK / [2 * (MCI_CLK_ClkDiv + 1)]
  #define MCI_CLK_Enable                 0x00000100                                         // enable MCI bus clock
  #define MCI_CLK_PwrSave                0x00000200                                         // disable MCI clock output when bus is idle
  #define MCI_CLK_Bypass                 0x00000400                                         // MCLK driven to card bus output
  #define MCI_CLK_WideBus                0x00000800                                         // widebus mode (D0..D3)
#define MCIArgument                      *(unsigned long*)(MCI_BLOCK + 0x08)                // Argument Register
#define MCICommand                       *(unsigned long*)(MCI_BLOCK + 0x0c)                // Command Register
  #define MCICmd_CmdIndex                0x0000003f                                         // command index mask
  #define MCICmd_Response                0x00000040                                         // wait for a response to the command
  #define MCICmd_LongRsp                 0x00000080                                         // 136 bit long response
  #define MCICmd_Interrupt               0x00000100                                         // disable command timer and wait for interrupt request
  #define MCICmd_Pending                 0x00000200                                         // wait for command pending before sending the command
  #define MCICmd_Enable                  0x00000400                                         // enable Command Path State Machine
#define MCIRespCmd                       *(volatile unsigned long*)(MCI_BLOCK + 0x10)       // Response Command Register (read-only)
#define MCIResponse0                     *(volatile unsigned long*)(MCI_BLOCK + 0x14)       // Response Register 0 (read-only)
#define MCIResponse1                     *(volatile unsigned long*)(MCI_BLOCK + 0x18)       // Response Register 1 (read-only)
#define MCIResponse2                     *(volatile unsigned long*)(MCI_BLOCK + 0x1c)       // Response Register 2 (read-only)
#define MCIResponse3                     *(volatile unsigned long*)(MCI_BLOCK + 0x20)       // Response Register 3 (read-only)
#define MCIDataTimer                     *(unsigned long*)(MCI_BLOCK + 0x24)                // Data Timer
#define MCIDataLength                    *(volatile unsigned long*)(MCI_BLOCK + 0x28)       // Data Length
#define MCIDataCtrl                      *(unsigned long*)(MCI_BLOCK + 0x2c)                // Data Control Register
  #define MCIDataCnt_Enable              0x00000001                                         // data transfer enable
  #define MCIDataCnt_Direction_to_card   0x00000000
  #define MCIDataCnt_Direction_from_card 0x00000002
  #define MCIDataCnt_Mode_Block          0x00000000
  #define MCIDataCnt_Mode_Stream         0x00000004
  #define MCIDataCnt_DMAEnable           0x00000008
  #define MCIDataCnt_block_size_512      0x00000090
#define MCIDataCnt                       *(unsigned long*)(MCI_BLOCK + 0x30)                // Data Counter
#define MCIStatus                        *(volatile unsigned long*)(MCI_BLOCK + 0x34)       // Status Register (read-only)
  #define CmdCrcFail                     0x00000001                                         // static status bits - these need clearing once set
  #define DataCrcFail                    0x00000002
  #define CmdTimeOut                     0x00000004
  #define DataTimeOut                    0x00000008
  #define TxUnderrun                     0x00000010
  #define RxOverrun                      0x00000020
  #define CmdRespEnd                     0x00000040
  #define CmdSent                        0x00000080
  #define DataEnd                        0x00000100
  #define StartBitErr                    0x00000200
  #define DataBlockEnd                   0x00000400
                                                                                            // dynamic status bits - these reflect the present status
  #define CmdActive                      0x00000800                                         // command transfer in progress
  #define TxActive                       0x00001000                                         // data transmit in progress
  #define RxActive                       0x00002000                                         // data receive in progress
  #define TxFifoHalfEmpty                0x00004000
  #define RxFifoHalfFull                 0x00008000
  #define TxFifoFull                     0x00010000
  #define RxFifoFull                     0x00020000
  #define TxFifoEmpty                    0x00040000
  #define RxFifoEmpty                    0x00080000
  #define TxDataAvailble                 0x00100000
  #define RxDataAvailble                 0x00200000
#define MCIClear                         *(volatile unsigned long*)(MCI_BLOCK + 0x38)       // Clear Register (write-only)
  #define CmdCrcFailClr                  0x00000001
  #define DataCrcFailClr                 0x00000002
  #define CmdTimeOutClr                  0x00000004
  #define DataTimeOutClr                 0x00000008
  #define TxUnderrunClr                  0x00000010
  #define RxOverrunClr                   0x00000020
  #define CmdRespEndClr                  0x00000040
  #define CmdSentClr                     0x00000080
  #define DataEndClr                     0x00000100
  #define StartBitErrClr                 0x00000200
  #define DataBlockEndClr                0x00000400
#define MCIMask0                         *(unsigned long*)(MCI_BLOCK + 0x3c)                // Interrupt 0 Mask Register
#define MCIFifoCnt                       *(volatile unsigned long*)(MCI_BLOCK + 0x48)       // FIFO Counter (read-only)
#define MCIFIFO0_ADD                     (volatile unsigned long*)(MCI_BLOCK + 0x80)
#define MCIFIFO0                         *(volatile unsigned long*)(MCI_BLOCK + 0x80)       // Data FIFO Registers
#define MCIFIFO1                         *(volatile unsigned long*)(MCI_BLOCK + 0x84)
#define MCIFIFO2                         *(volatile unsigned long*)(MCI_BLOCK + 0x88)
#define MCIFIFO3                         *(volatile unsigned long*)(MCI_BLOCK + 0x8c)
#define MCIFIFO4                         *(volatile unsigned long*)(MCI_BLOCK + 0x90)
#define MCIFIFO5                         *(volatile unsigned long*)(MCI_BLOCK + 0x94)
#define MCIFIFO6                         *(volatile unsigned long*)(MCI_BLOCK + 0x98)
#define MCIFIFO7                         *(volatile unsigned long*)(MCI_BLOCK + 0x9c)
#define MCIFIFO8                         *(volatile unsigned long*)(MCI_BLOCK + 0xa0)
#define MCIFIFO9                         *(volatile unsigned long*)(MCI_BLOCK + 0xa4)
#define MCIFIFO10                        *(volatile unsigned long*)(MCI_BLOCK + 0xa8)
#define MCIFIFO11                        *(volatile unsigned long*)(MCI_BLOCK + 0xac)
#define MCIFIFO12                        *(volatile unsigned long*)(MCI_BLOCK + 0xb0)
#define MCIFIFO13                        *(volatile unsigned long*)(MCI_BLOCK + 0xb4)
#define MCIFIFO14                        *(volatile unsigned long*)(MCI_BLOCK + 0xb8)
#define MCIFIFO15                        *(volatile unsigned long*)(MCI_BLOCK + 0xbc)


// System Control Module
//
#define MAMCR                            *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x00)    // Memory Acceleration Module Control Register
  #define MAM_DISABLED                   0x00000000
  #define MAM_PARTIALLY_ENABLED          0x00000001
  #define MAM_FULLY_ENABLED              0x00000002
#define MAMTIM                           *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x04)    // Memory Acceleration Module Timing Register
  #define MAM_FETCH_CLOCKS_1             0x00000001
  #define MAM_FETCH_CLOCKS_2             0x00000002
  #define MAM_FETCH_CLOCKS_3             0x00000003
#define MEMMAP                           *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x40)
#define PLLCON                           *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x80) // PLL Control Register
  #define PLLE                           0x00000001                                         // enable PLL
  #define PLLC                           0x00000002                                         // connect PLL
#define PLLCFG                           *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x84)    // PLL Configuration Register
#define PLLSTAT                          *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x88) // PLL Status Register (Read Only)
    #ifdef _LPC21XX
  #define PLLENABLED                     0x00000100                                         // status of enable bit
  #define PLLCONNECTED                   0x00000200                                         // status of connected bit
  #define PLLLOCKED                      0x00000400                                         // PLL lock status status
    #else
  #define PLLENABLED                     0x01000000                                         // status of enable bit
  #define PLLCONNECTED                   0x02000000                                         // status of connected bit
  #define PLLLOCKED                      0x04000000                                         // PLL lock status status
    #endif
#define PLLFEED                          *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x8c) // PLL Feed Register (Write Only)
#ifdef _LPC214X
    #define PLL1CON                      *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0xa0) // PLL1 Control Register
    #define PLL1CFG                      *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0xa4)    // PLL1 Configuration Register
    #define PLL1STAT                     *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0xa8) // PLL1 Status Register (Read Only)
    #define PLL1FEED                     *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0xac) // PLL1 Feed Register (Write Only)
#endif
#define PCON                             *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0xc0)    // Power Control Register
  #define PCON_IDL                       0x00000001                                         // Idle mode
  #define PCON_PD                        0x00000002                                         // Power down mode
#if !defined _LPC21XX
  #define PCON_BODPDM
  #define PCON_BOGD
  #define PCON_BORD
  #define PCON_PM2
#endif
#define PCONP                            *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0xc4)    // Power Control for Peripherals Register
  #define PCTIM0                         0x00000002
  #define PCTIM1                         0x00000004
  #define PCUART0                        0x00000008
  #define PCUART1                        0x00000010
  #define PCPWM0                         0x00000020
  #define PCPWM1                         0x00000040
  #define PCI2C0                         0x00000080
  #define PCSPI                          0x00000100
  #define PCRTC                          0x00000200
#ifdef _LPC214X
  #define PCSSP0                         0x00000400                                          // the LPC214X refers to the SSP0 as SP1
#else
  #define PCSSP1                         0x00000400
#endif
  #define PCEMC                          0x00000800
  #define PCPCAD                         0x00001000
  #define PCPCAN1                        0x00002000
  #define PCPCAN2                        0x00004000
  #define PCI2C1                         0x00080000
  #define PCLCD                          0x00100000                      // LPC247x
#ifndef _LPC214X
  #define PCSSP0                         0x00200000
#endif
  #define PCTIM2                         0x00400000
  #define PCTIM3                         0x00800000
  #define PCUART2                        0x01000000
  #define PCUART3                        0x02000000
  #define PCI2C2                         0x04000000
  #define PCI2S                          0x08000000
  #define PCSDC                          0x10000000
  #define PCGPDMA                        0x20000000
  #define PCENET                         0x40000000
  #define PCUSB                          0x80000000


#define POWER_UP(module)                 PCONP |= module;                // power up a module (apply clock to it)
#define POWER_DOWN(module)               PCONP &= ~module;               // power down a module (disable clock to it)
#if defined _LPC214X || defined _LPC213X                                 // {30}
  #define APBDIV                         *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x100)    // CPU Clock Configuration Register
    #define APBCLOCK_4                   0x00000000
    #define APBCLOCK_1                   0x00000001
    #define APBCLOCK_2                   0x00000002
#endif
#define CCLKCFG                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x104)    // CPU Clock Configuration Register
#define USBCLKCFG                        *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x108)    // USB Clock Configuration Register
#define CLKSRCSEL                        *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x10c)    // Clock Source Select Register
  #define PLL_SOURCE_4M_RC               0x00000000                                          // PLL source is 4MHz RC oscillator
  #define PLL_SOURCE_OSCILLATOR          0x00000001                                          // PLL source is main oscillator
  #define PLL_SOURCE_RTC                 0x00000002                                          // PLL source is RTC oscillator

#define EXTINT                           *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x140) // External Interrupt Flag Register
  #define EINT0                          0x00000001                                          // writing these to '1' clears them when not in level sensitive mode, where the flag is cleared only when the input level changed
  #define EINT1                          0x00000002
  #define EINT2                          0x00000004
  #define EINT3                          0x00000008
#define INTWAKE                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x144)    // Interrupt Wakeup Register
#define EXTMODE                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x148)    // External Interrupt Mode Register
  #define EXTMODE0                       0x00000001                                          // edge sensitive
  #define EXTMODE1                       0x00000002
  #define EXTMODE2                       0x00000004
  #define EXTMODE3                       0x00000008
#define EXTPOLAR                         *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x14c)    // External Interrupt Polarity Register
  #define EXTPOLAR0                      0x00000001                                          // polarity high or rising edge
  #define EXTPOLAR1                      0x00000002
  #define EXTPOLAR2                      0x00000004
  #define EXTPOLAR3                      0x00000008
#define RSIR                             *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x180)    // Reset Source Identification Register
  #define POR                            0x00000001                                          // Power On reset
  #define EXTR                           0x00000002                                          // External reset
  #define WDTR                           0x00000004                                          // Watchdog Timer reset
  #define BODR                           0x00000008                                          // Brownout reset

#define SCS                              *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a0) // System Control and Status
  #define GPIOM                          0x00000001                                          // High speed GPIO mode enable
  #define GPIO0M                         0x00000001                                          // High speed GPIO mode enabled on port 0
  #define GPIO1M                         0x00000002                                          // High speed GPIO mode enabled on port 1 - LPC2148
  #define EMC_RESET_DISABLE              0x00000002                                          // Preserve EMC state through warm reset
  #define MCIPWR                         0x00000008                                          // Active high level on this pin
  #define OSCRANGE                       0x00000010                                          // Main oscillator range set
  #define OSCEN                          0x00000020                                          // Enable external oscillator
  #define OSCSTAT                        0x00000040                                          // main oscillator status - ready for use (Read Only bit)
#define IRCTRIM                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a4)    // IRC Trim Register
#define PCLKSEL0                         *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a8)    // Peripheral Clock Selection Register 0
  #define PCLK_WDT_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_WDT_CCLK_1                0x00000001                                          // PCLK to this peripheral is CCLK
  #define PCLK_WDT_CCLK_2                0x00000002                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_WDT_CCLK_8                0x00000003                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_TIMER0_CCLK_4             0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_TIMER0_CCLK_1             0x00000004                                          // PCLK to this peripheral is CCLK
  #define PCLK_TIMER0_CCLK_2             0x00000008                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_TIMER0_CCLK_8             0x0000000c                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_TIMER1_CCLK_4             0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_TIMER1_CCLK_1             0x00000010                                          // PCLK to this peripheral is CCLK
  #define PCLK_TIMER1_CCLK_2             0x00000020                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_TIMER1_CCLK_8             0x00000030                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_UART0_CCLK_4              0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_UART0_CCLK_1              0x00000040                                          // PCLK to this peripheral is CCLK
  #define PCLK_UART0_CCLK_2              0x00000080                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_UART0_CCLK_8              0x000000c0                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_UART1_CCLK_4              0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_UART1_CCLK_1              0x00000100                                          // PCLK to this peripheral is CCLK
  #define PCLK_UART1_CCLK_2              0x00000200                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_UART1_CCLK_8              0x00000300                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_PWM0_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_PWM0_CCLK_1               0x00000400                                          // PCLK to this peripheral is CCLK
  #define PCLK_PWM0_CCLK_2               0x00000800                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_PWM0_CCLK_8               0x00000c00                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_PWM1_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_PWM1_CCLK_1               0x00001000                                          // PCLK to this peripheral is CCLK
  #define PCLK_PWM1_CCLK_2               0x00002000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_PWM1_CCLK_8               0x00003000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_I2C0_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_I2C0_CCLK_1               0x00004000                                          // PCLK to this peripheral is CCLK
  #define PCLK_I2C0_CCLK_2               0x00008000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_I2C0_CCLK_8               0x0000c000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_SPI_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_SPI_CCLK_1                0x00010000                                          // PCLK to this peripheral is CCLK
  #define PCLK_SPI_CCLK_2                0x00020000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_SPI_CCLK_8                0x00030000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_RTC_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_RTC_CCLK_2                0x00080000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_RTC_CCLK_8                0x000c0000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_SSP1_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_SSP1_CCLK_1               0x00100000                                          // PCLK to this peripheral is CCLK
  #define PCLK_SSP1_CCLK_2               0x00200000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_SSP1_CCLK_8               0x00300000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_DAC_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_DAC_CCLK_1                0x00400000                                          // PCLK to this peripheral is CCLK
  #define PCLK_DAC_CCLK_2                0x00800000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_DAC_CCLK_8                0x00c00000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_ADC_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_ADC_CCLK_1                0x01000000                                          // PCLK to this peripheral is CCLK
  #define PCLK_ADC_CCLK_2                0x02000000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_ADC_CCLK_8                0x03000000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_CAN1_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_CAN1_CCLK_1               0x04000000                                          // PCLK to this peripheral is CCLK
  #define PCLK_CAN1_CCLK_2               0x08000000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_CAN1_CCLK_6               0x0c000000                                          // PCLK to this peripheral is CCLK/6
  #define PCLK_CAN2_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_CAN2_CCLK_1               0x10000000                                          // PCLK to this peripheral is CCLK
  #define PCLK_CAN2_CCLK_2               0x20000000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_CAN2_CCLK_6               0x30000000                                          // PCLK to this peripheral is CCLK/6
  #define PCLK_ACF_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_ACF_CCLK_1                0x40000000                                          // PCLK to this peripheral is CCLK
  #define PCLK_ACF_CCLK_2                0x80000000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_ACF_CCLK_6                0xc0000000                                          // PCLK to this peripheral is CCLK/6
#define PCLKSEL1                         *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1ac)    // Peripheral Clock Selection Register 1
  #define PCLK_BAT_RAM_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_BAT_RAM_CCLK_1            0x00000001                                          // PCLK to this peripheral is CCLK
  #define PCLK_BAT_RAM_CCLK_2            0x00000002                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_BAT_RAM_CCLK_8            0x00000003                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_GPIO_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_GPIO_CCLK_1               0x00000004                                          // PCLK to this peripheral is CCLK
  #define PCLK_GPIO_CCLK_2               0x00000008                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_GPIO_CCLK_8               0x0000000c                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_PCB_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_PCB_CCLK_1                0x00000010                                          // PCLK to this peripheral is CCLK
  #define PCLK_PCB_CCLK_2                0x00000020                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_PCB_CCLK_8                0x00000030                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_I2C1_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_I2C1_CCLK_1               0x00000040                                          // PCLK to this peripheral is CCLK
  #define PCLK_I2C1_CCLK_2               0x00000080                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_I2C1_CCLK_8               0x000000c0                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_SSP0_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_SSP0_CCLK_1               0x00000400                                          // PCLK to this peripheral is CCLK
  #define PCLK_SSP0_CCLK_2               0x00000800                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_SSP0_CCLK_8               0x00000c00                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_TIMER2_CCLK_4             0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_TIMER2_CCLK_1             0x00001000                                          // PCLK to this peripheral is CCLK
  #define PCLK_TIMER2_CCLK_2             0x00002000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_TIMER2_CCLK_8             0x00003000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_TIMER3_CCLK_4             0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_TIMER3_CCLK_1             0x00004000                                          // PCLK to this peripheral is CCLK
  #define PCLK_TIMER3_CCLK_2             0x00008000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_TIMER3_CCLK_8             0x0000c000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_UART2_CCLK_4              0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_UART2_CCLK_1              0x00010000                                          // PCLK to this peripheral is CCLK
  #define PCLK_UART2_CCLK_2              0x00020000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_UART2_CCLK_8              0x00030000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_UART3_CCLK_4              0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_UART3_CCLK_1              0x00040000                                          // PCLK to this peripheral is CCLK
  #define PCLK_UART3_CCLK_2              0x00080000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_UART3_CCLK_8              0x000c0000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_I2C2_CCLK_4               0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_I2C2_CCLK_1               0x00100000                                          // PCLK to this peripheral is CCLK
  #define PCLK_I2C2_CCLK_2               0x00200000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_I2C2_CCLK_8               0x00300000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_I2S_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_I2S_CCLK_1                0x00400000                                          // PCLK to this peripheral is CCLK
  #define PCLK_I2S_CCLK_2                0x00800000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_I2S_CCLK_8                0x00c00000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_MCI_CCLK_4                0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_MCI_CCLK_1                0x01000000                                          // PCLK to this peripheral is CCLK
  #define PCLK_MCI_CCLK_2                0x02000000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_MCI_CCLK_8                0x03000000                                          // PCLK to this peripheral is CCLK/8
  #define PCLK_PCLK_SYSCON_CCLK_4        0x00000000                                          // PCLK to this peripheral is CCLK/4
  #define PCLK_PCLK_SYSCON_CCLK_1        0x10000000                                          // PCLK to this peripheral is CCLK
  #define PCLK_PCLK_SYSCON_CCLK_2        0x20000000                                          // PCLK to this peripheral is CCLK/2
  #define PCLK_PCLK_SYSCON_CCLK_8        0x30000000                                          // PCLK to this peripheral is CCLK/8
#define LCD_CFG                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1b8)    // Panel Clock Divide Register
#define USBIntSt                         *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1c0)  // USB interrupt status register (some bits read only)
  #define USB_INT_REQ_LP                 0x00000001                                          // low priority interrupt line status (read-only)
  #define USB_INT_REQ_HP                 0x00000002                                          // high priority interrupt line status (read-only)
  #define USB_INT_REQ_DMA                0x00000004                                          // DMA interrupt line status (read-only)
  #define USB_NEED_CLK                   0x00000100                                          // 
  #define EN_USB_INTS                    0x80000000                                          // enable all USB interrupts (default active)

// Ethernet Controller
//
#define MAC1                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x000)    // MAC Configuration Register 1
  #define RECEIVE_ENABLE                 0x00000001
  #define PASS_ALL_RECEIVE_FRAMES        0x00000002
  #define MAC1_RX_FLOW_CONTROL           0x00000004
  #define TX_FLOW_CONTROL                0x00000008
  #define MAC1_LOOPBACK                  0x00000010
  #define RESET_TX                       0x00000100
  #define RESET_MCS_TX                   0x00000200
  #define RESET_RX                       0x00000400
  #define RESET_MCS_RX                   0x00000800
  #define SIMULATION_RESET               0x00004000                      // {36}
  #define SOFT_RESET                     0x00008000
#define MAC2                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x004)    // MAC Configuration Register 2
  #define MAC2_FULL_DUPLEX               0x00000001
  #define FRAME_LENGTH_CHECKING          0x00000002
  #define HUGE_FRAME_ENABLED             0x00000004
  #define DELAYED_CRC                    0x00000008
  #define CRC_ENABLE                     0x00000010
  #define PAD_CRC_ENABLE                 0x00000020
  #define VLAN_PAD_ENABLE                0x00000040
  #define AUTO_DETECT_PAD_ENABLE         0x00000080
  #define PURE_PREAMBLE_ENFORCEMENT      0x00000100
  #define LONG_PREAMBLE_ENFORCEMENT      0x00000200
  #define NO_BACKOFF                     0x00001000
  #define BACK_PRESSURE_NO_BACKOFF       0x00002000
  #define EXCESS_DEFER                   0x00004000
#define IPGT                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x008)    // Back-to-back Inter Packet Gap Register
  #define HALF_DUPLEX_IPG_TIME           0x00000012
  #define FULL_DUPLEX_IPG_TIME           0x00000015
#define IPGR                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x00c)    // Non Back-to-back Inter Packet Gap Register
  #define IPGR_TIME                      0x00000012
#define CLRT                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x010)    // Collision Windows / Retry Register
  #define DEFAULT_CLRT                   0x370f                                                  // recommended default
#define MAXF                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x014)    // Maximum Frame Register
#define SUPP                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x018) // PHY Support Register {37}
  #define SUPP_SPEED                     0x00000100
  #define RESET_RMII                     0x00000800
#define TEST                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x01c)    // Test Register
#define MCFG                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x020)    // MII Management Configuration Register
  #define SCAN_INCREMENT                 0x00000001
  #define SUPPRESS_PREAMBLE              0x00000002
  #define HOST_CLK_DIV_4                 0x00000000
  #define HOST_CLK_DIV_6                 0x00000008
  #define HOST_CLK_DIV_8                 0x0000000c
  #define HOST_CLK_DIV_10                0x00000010
  #define HOST_CLK_DIV_14                0x00000014
  #define HOST_CLK_DIV_20                0x00000018
  #define HOST_CLK_DIV_28                0x0000001c
  #define RESET_MMI_MGMT                 0x00008000
#define MCMD                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x024)    // MII Management Command Register
  #define MCMD_READ                      0x00000001
  #define MCMD_SCAN                      0x00000002
#define MADR                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x028)    // MII Management Address Register
#define MWTD                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x02c) // MII Management Write Data Register (Write Only)
#define MRDD                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x030) // MII Management Read Data Register (Read Only)
#define MIND                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x034) // MII Management Indicators Register (Read Only)
  #define MIND_BUSY                      0x00000001
  #define MIND_SCANNING                  0x00000002
  #define MIND_NOT_VALID                 0x00000004
  #define MIND_MII_LINK_FAIL             0x00000008

#define SA0                              *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x040)    // Station Address 0 Register
#define SA1                              *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x044)    // Station Address 1 Register
#define SA2                              *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x048)    // Station Address 2 Register

#define EMAC_COMMAND                     *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x100)    // Command Register
  #define EMAC_RX_ENABLE                 0x00000001
  #define EMAC_TX_ENABLE                 0x00000002
  #define EMAC_REG_RESET                 0x00000008
  #define EMAC_TX_RESET                  0x00000010                      // set only (reads back 0)
  #define EMAC_RX_RESET                  0x00000020                      // set only (reads back 0)
  #define EMAC_PASS_RUNT_FRAME           0x00000040
  #define EMAC_PASS_RC_FILTER            0x00000080
  #define EMAC_TX_FLOW_CONTROL           0x00000100
  #define EMAC_RMII                      0x00000200
  #define EMAC_FULL_DUPLEX               0x00000400
#define EMAC_STATUS                      *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x104) // Status Register (Read Only)
  #define EMAC_RX_STATUS                 0x00000001
  #define EMAC_TX_STATUS                 0x00000002
#define EMAC_RxDescriptor                *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x108)    // Receive Descriptor Base Address Register
#define EMAC_RxStatus                    *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x10c) // Receive Status Base Address Register
#define EMAC_RxDescriptorNumber          *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x110) // Receive Number of Descriptors Register
#define EMAC_RxProduceIndex              *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x114) // Receive Produce Index Register (Read Only)
#define EMAC_RxConsumeIndex              *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x118) // Receive Consume Index Register
#define EMAC_TxDescriptor                *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x11c)    // Transmit Descriptor Base Address Register
#define EMAC_TxStatus                    *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x120) // Transmit Status Base Address Register
#define EMAC_TxDescriptorNumber          *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x124) // Transmit Number of Descriptors Register
#define EMAC_TxProduceIndex              *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x128) // Transmit Produce Index Register
#define EMAC_TxConsumeIndex              *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x12c) // Transmit Consume Index Register (Read Only)

#define TSV0                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x158)    // Transmit Status Vector 0 Register (Read Only)
#define TSV1                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x15c)    // Transmit Status Vector 1 Register (Read Only)
#define RSV                              *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x160)    // Transmit Status Vector 2 Register (Read Only)

#define EMAC_FlowControlCounter          *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x170)    // Flow Control Counter Register
#define EMAC_FlowControlStatus           *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x174) // Flow Control Status Register (Read Only)

#define EMAC_RxFilterControl             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x200)    // Receive Filter Control Register
  #define ENABLE_ACCEPT_UNICAST          0x00000001
  #define ENABLE_ACCEPT_BROADCAST        0x00000002
  #define ENABLE_ACCEPT_MULTICAST        0x00000004
  #define ENABLE_ACCEPT_UNICAST_HASH     0x00000008
  #define ENABLE_ACCEPT_MULTICAST_HASH   0x00000010
  #define ENABLE_ACCEPT_PERFECT          0x00000020
  #define ENABLE_ACCEPT_MAGIC_WAKEUP     0x00001000
  #define ENABLE_ACCEPT_WAKEUP           0x00002000
#define EMAC_RxFilterWoLStatus           *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x204)    // Receive Filter WoL Status Register
#define EMAC_RxFilterWoLClear            *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x208)    // Receive Filter WoL Clear Register

#define EMAC_HashFilterL                 *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x210)    // Hash Filter Table LSBs Register
#define EMAC_HashFilterH                 *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x214)    // Hash Filter Table MSBs Register

#define EMAC_IntStatus                   *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0xfe0) // Interrupt Status Register (Read Only)
  #define EMAC_RxOverrunInt              0x00000001
  #define EMAC_RxErrorInt                0x00000002
  #define EMAC_RxFinishedInt             0x00000004
  #define EMAC_RxDoneInt                 0x00000008
  #define EMAC_TxUnderrunInt             0x00000010
  #define EMAC_TxErrorInt                0x00000020
  #define EMAC_TxFinishedInt             0x00000040
  #define EMAC_TxDoneInt                 0x00000080
  #define EMAC_SoftInt                   0x00000100
  #define EMAC_WakeupInt                 0x00000200
#define EMAC_IntEnable                   *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0xfe4)    // Interrupt Enable Register
#define EMAC_IntClear                    *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0xfe8) // Interrupt Clear Register (Write Only)
#define EMAC_IntSet                      *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0xfec) // Interrupt Set Register (Write Only)

#define EMAC_PowerDown                   *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0xff4)    // Power Down Register
#define EMAC_MODULE_ID                   *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0xffc) // Module ID register (Read Only)
    #define OLD_MAC_MODULE_ID            ((0x3902 << 16) | 0x2000)                               // first version MAC ID


typedef struct stLPC23XX_RX_BD
{
volatile unsigned char   *ptrBlock;
volatile unsigned long    bd_details;
} LPC23XX_RX_BD;

#define RX_LAN_SIZE_MASK                  0x000007ff
#define RX_LAN_INTERRUPT_ON_DONE          0x80000000
#define TX_OVERRIDE                       0x04000000
#define TX_HUGE_FRAME                     0x08000000
#define TX_PAD_FRAME                      0x10000000
#define TX_APPEND_CRC                     0x20000000
#define TX_LAST_FRAG                      0x40000000
#define TX_LAN_INTERRUPT_ON_DONE          0x80000000

typedef struct stLPC23XX_RX_STATUS
{
volatile unsigned long    ulStatusInfo;
volatile unsigned long    ulStatusHASH_CRC;
} LPC23XX_RX_STATUS;

#define CONTROL_FRAME                     0x00040000
#define VLAN_FRAME                        0x00080000
#define FAIL_FILTER                       0x00100000
#define MULTICAST_FRAME                   0x00200000
#define BROADCAST_FRAME                   0x00400000
#define CRC_ERROR                         0x00800000
#define SYMBOL_ERROR                      0x01000000
#define LENGTH_ERROR                      0x02000000
#define RANGE_ERROR                       0x04000000
#define ALIGNMENT_ERROR                   0x08000000
#define RX_LAN_OVERRUN                    0x10000000
#define RX_NO_DESCRIPTOR                  0x20000000
#define LAST_FRAG                         0x40000000
#define RX_FRAME_ERROR                    0x80000000

typedef struct stLPC23XX_TX_STATUS
{
volatile unsigned long    ulStatusInfo;
} LPC23XX_TX_STATUS;

#define COLLISION_COUNT_SHIFT             21
#define COLLISION_COUNT_MASK              0x01e00000
#define DEFER                             0x02000000
#define EXCESSIVE_DEFER                   0x04000000
#define EXCESSIVE_COLLISION               0x08000000
#define LATE_COLLISION                    0x10000000
#define TX_LAN_OVERRUN                    0x20000000
#define TX_NO_DESCRIPTOR                  0x40000000
#define TX_FRAME_ERROR                    0x80000000


// PHYSICAL
//
#define GENERIC_BASIC_CONTROL_REGISTER                 (0x0)             // {22}
  #define PHY_DISABLE_TRANSMITTER                     0x0001
  #define PHY_ENABLE_COLLISION_TEST                   0x0080
  #define PHY_FULL_DUPLEX_MODE                        0x0100
  #define PHY_RESTART_AUTO_NEGOTIATION                0x0200
  #define PHY_ELECTRICALLY_ISOLATE                    0x0400
  #define PHY_POWER_DOWN_MODE                         0x0800
  #define PHY_ENABLE_AUTO_NEGOTIATION                 0x1000
  #define PHY_SELECT_100M_SPEED                       0x2000
  #define PHY_LOOP_BACK                               0x4000
  #define PHY_SOFTWARE_RESET                          0x8000
#define GENERIC_BASIC_STATUS_REGISTER                  (0x1)
  #define PHY_SUPPORT_EXTENDED_CAPABILITIES           0x0001
  #define PHY_JABBER_DETECTED                         0x0002
  #define PHY_LINK_IS_UP                              0x0004
  #define PHY_AUTO_NEGOTIATION_CAPABLE                0x0008
  #define PHY_REMOTE_FAULT_DETECTED                   0x0010
  #define PHY_AUTO_NEGOTIATION_COMPLETE               0x0020
  #define PHY_NO_PREAMBLE                             0x0040
  #define PHY_10MEG_HALF_DUPLEX                       0x0800
  #define PHY_10MEG_FULL_DUPLEX                       0x1000
  #define PHY_100MEG_HALF_DUPLEX                      0x2000
  #define PHY_100MEG_FULL_DUPLEX                      0x4000
  #define PHY_100MEG_T4_CAPABLE                       0x8000
#define GENERIC_PHY_IDENTIFIER_1                       (0x2)
#define GENERIC_PHY_IDENTIFIER_2                       (0x3)
#define GENERIC_AUTO_NEG_ADVERT_REG                    (0x4)
#define GENERIC_AUTO_NEG_LINK_PARTNER_ABIL_REG         (0x5)
#define GENERIC_AUTO_NEG_EXPANSION_REG                 (0x6)
#define GENERIC_AUTO_NEG_NEXT_PAGE_REG                 (0x7)

#define DP83848_PHY_STATUS_REGISTER                    (0x10)            // {21}
  #define VALID_LINK_ESTABLISHED                      0x0001
  #define SPEED_STATUS_10M                            0x0002
  #define DUPLEX_STATUS_FULL_DUPLEX                   0x0004
#define DP83848_MII_INTERRUPT_CONTROL_REGISTER         (0x11)
  #define DP83848_INT_OE                              0x0001
  #define DP83848_INT_EN                              0x0002
#define DP83848_MII_INTERRUPT_STATUS_REGISTER          (0x12)
  #define DP83848_ANC_INT_EN                          0x0004
  #define DP83848_LINK_INT_EN                         0x0020
  #define DP83848_ANC_INT                             0x0400
  #define DP83848_LINK_INT                            0x2000
#define DP83848_FALSE_CARRIER_SENSE COUNTER_REGISTER   (0x14)
#define DP83848_RXER_COUNTER_REGISTER                  (0x15)
#define DP83848_PCS_CONFIG_STATUS_REGISTER             (0x16)
#define DP83848_RMII_AND_BYPASS_REGISTER               (0x17)
#define DP83848_LED_DIRECT_CONTROL_REGISTER            (0x18)
#define DP83848_PHY_CONTROL_REGISTER                   (0x19)
#define DP83848_10BASET_STATUS_CONTROL_REGISTER        (0x1a)
#define DP83848_CD_TEST_CONTROL_REGISTER               (0x1b)
#define DP83848_ENERGY_DETECT_CONTROL_REGISTER         (0x1d)

#define LAN8700_INTERRUPT_FLAG                         (0x1d)
#define LAN8700_INTERRUPT_MASK                         (0x1e)
  #define PHY_LAN8700_AUTO_NEG_PAGE_RECEIVED          0x0002
  #define PHY_LAN8700_PARALLEL_DETECTION_FAULT        0x0004
  #define PHY_LAN8700_AUTO_NEG_LP_ACK                 0x0008
  #define PHY_LAN8700_LINK_DOWN                       0x0010
  #define PHY_LAN8700_REMOTE_FAULT_DETECTED           0x0020
  #define PHY_LAN8700_AUTO_NEG_COMPLETE               0x0040
  #define PHY_LAN8700_ENERGYON_GENERATED              0x0080


#define KS8721_INTERRUPT_CONTROL_STATUS_REGISTER      (0x1b)
  #define PHY_LINK_UP_INT_ENABLE                      0x0100
  #define PHY_REMOTE_FAULT_INT_ENABLE                 0x0200
  #define PHY_LINK_DOWN_INT_ENABLE                    0x0400
  #define PHY_LINK_PARTNER_ACK_INT_ENABLE             0x0800
  #define PHY_PARALLEL_DETECT_FAULT_INT_ENABLE        0x1000
  #define PHY_PAGE_RECEIVED_INT_ENABLE                0x2000
  #define PHY_RECEIVE_ERROR_INT_ENABLE                0x4000
  #define PHY_JABBER_INT_ENABLE                       0x8000
  #define PHY_LINK_UP_INT                             0x0001
  #define PHY_REMOTE_FAULT_INT                        0x0002
  #define PHY_LINK_DOWN_INT                           0x0004
  #define PHY_LINK_PARTNER_ACK_INT                    0x0008
  #define PHY_PARALLEL_DETECT_FAULT_INT               0x0010
  #define PHY_PAGE_RECEIVED_INT                       0x0020
  #define PHY_RECEIVE_ERROR_INT                       0x0040
  #define PHY_JABBER_INT                              0x0080

#define KS8721_100BASETX_PHY_CONTROL_REGISTER         (0x1f)
  #define PHY_DISABLE_SCRAMBLER                       0x0001
  #define PHY_ENABLE_SQE_TEST                         0x0002
  #define PHY_OP_MODE_MASK                            0x001c
  #define PHY_OP_MODE_STILL_AUTONEGOTIATING           0x0000
  #define PHY_OP_MODE_10_BASE_T_HALF_DUPLEX           0x0004
  #define PHY_OP_MODE_100_BASE_T_HALF_DUPLEX          0x0008
  #define PHY_OP_MODE_FULL_DUPLEX                     0x0010
  #define PHY_OP_MODE_10_BASE_T_FULL_DUPLEX           0x0014
  #define PHY_OP_MODE_100_BASE_T_FULL_DUPLEX          0x0018
  #define PHY_OP_MODE_ISOLATED                        0x001c
  #define PHY_IN_ISOLATE_MODE                         0x0020
  #define PHY_FLOW_CONTROL_CAPABLE                    0x0040
  #define PHY_AUTONEGOTIATION_COMPLETE                0x0080
  #define PHY_ENABLE_JABBER_COUNTER                   0x0100
  #define PHY_INTERRUPT_PIN_ACTIVE_HIGH               0x0200
  #define PHY_ENABLE_POWER_SAVING                     0x0400
  #define PHY_FORCE_LINK_PASS                         0x0800
  #define PHY_SIGNAL_PRESENCE_DETECTED                0x1000
  #define PHY_DISABLE_MDI_MDIX                        0x2000


// DMA controller                                                        {23}
//
#define DMACIntStatus                    *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x000)  // Interrupt Status Register (read-only)
#define DMACIntTCStatus                  *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x004)  // Interrupt Terminal Count Status Register (read-only)
#define DMACIntTCClear                   *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x008)  // Interrupt Terminal Count Clear Register (write-only)
#define DMACIntErrorStatus               *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x00c)  // Interrupt Error Status Register (read-only)
#define DMACIntErrClr                    *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x010)  // Interrupt Error Clear Register (write-only)
#define DMACRawIntTCStatus               *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x014)  // Raw Interrupt Terminal Count Status Register (read-only)
#define DMACRawIntErrorStatus            *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x018)  // Raw Interrupt Error Status Register (read-only)
#define DMACEnbldChns                    *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x01c)  // Enabled Channel Register (read-only)
#define DMACSoftBReq                     *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x020)           // Software Burst Request Register
#define DMACSoftSReq                     *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x024)           // Software Single Request Register
#define DMACSoftLBReq                    *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x028)           // Software Last Burst Request Register
#define DMACSoftLSReq                    *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x02c)           // Software Last Single Request Register
#define DMACConfiguration                *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x030)  // Configuration Register
//#define DMACConfiguration_E            0x00000001
#define DMACSync                         *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x034)           // Synchronisation Register

#define DMACC0SrcAddr                    *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x100)           // Channel 0 Source Address Register
#define DMACC0DestAddr                   *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x104)           // Channel 0 Destination Address Register
#define DMACC0LLI                        *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x108)           // Channel 0 Linked List Item Register
#define DMACC0Control                    *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x10c)           // Channel 0 Control Register
  #define DMACCControl_TransferSizeMask  0x00000fff
  #define DMACCControl_SBSize_1          0x00000000                                                // source burst sizes
  #define DMACCControl_SBSize_4          0x00001000
  #define DMACCControl_SBSize_8          0x00002000
  #define DMACCControl_SBSize_16         0x00003000
  #define DMACCControl_SBSize_32         0x00004000
  #define DMACCControl_SBSize_64         0x00005000
  #define DMACCControl_SBSize_128        0x00006000
  #define DMACCControl_SBSize_256        0x00007000
  #define DMACCControl_DBSize_1          0x00000000                                                // destination burst sizes
  #define DMACCControl_DBSize_4          0x00008000
  #define DMACCControl_DBSize_8          0x00010000
  #define DMACCControl_DBSize_16         0x00018000
  #define DMACCControl_DBSize_32         0x00020000
  #define DMACCControl_DBSize_64         0x00028000
  #define DMACCControl_DBSize_128        0x00030000
  #define DMACCControl_DBSize_256        0x00038000
  #define DMACCControl_SWidth_byte       0x00000000                                                // source transfer width
  #define DMACCControl_SWidth_halfword   0x00040000
  #define DMACCControl_SWidth_word       0x00080000
  #define DMACCControl_DWidth_byte       0x00000000                                                // destination transfer width
  #define DMACCControl_DWidth_halfword   0x00200000
  #define DMACCControl_DWidth_word       0x00400000
  #define DMACCControl_SI                0x04000000                                                // source increment
  #define DMACCControl_DI                0x08000000                                                // destination increment
  #define DMACCControl_Prot              0x70000000
  #define DMACCControl_I                 0x80000000

#define DMACC0Configuration              *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x110)  // Channel 0 Configuration Register (bit 17 is read-only
  #define DMACCConfiguration_E           0x00000001                                                // enable
  #define DMACCConfiguration_SrcP_SSP0Tx 0x00000000                                                // source peripheral - SSP0 Tx
  #define DMACCConfiguration_SrcP_SSP0Rx 0x00000002                                                // source peripheral - SSP0 Rx
  #define DMACCConfiguration_SrcP_SSP1Tx 0x00000004                                                // source peripheral - SSP1 Tx
  #define DMACCConfiguration_SrcP_SSP1Rx 0x00000006                                                // source peripheral - SSP1 Rx
  #define DMACCConfiguration_SrcP_SD_MMC 0x00000008                                                // source peripheral - SD/MMC
  #define DMACCConfiguration_SrcP_I2S0   0x0000000a                                                // source peripheral - I2S0
  #define DMACCConfiguration_SrcP_I2S1   0x0000000c                                                // source peripheral - I2S1
  #define DMACCConfiguration_DstP_SSP0Tx 0x00000000                                                // destination peripheral - SSP0 Tx
  #define DMACCConfiguration_DstP_SSP0Rx 0x00000040                                                // destination peripheral - SSP0 Rx
  #define DMACCConfiguration_DstP_SSP1Tx 0x00000080                                                // destination peripheral - SSP1 Tx
  #define DMACCConfiguration_DstP_SSP1Rx 0x000000c0                                                // destination peripheral - SSP1 Rx
  #define DMACCConfiguration_DstP_SD_MMC 0x00000100                                                // destination peripheral - SD/MMC
  #define DMACCConfiguration_DstP_I2S0   0x00000140                                                // destination peripheral - I2S0
  #define DMACCConfiguration_DstP_I2S1   0x00000180                                                // destination peripheral - I2S1
  #define DMACCConfiguration_M2M         0x00000000                                                // memory to memory transfer
  #define DMACCConfiguration_M2P         0x00000800                                                // memory to peripheral transfer
  #define DMACCConfiguration_P2M         0x00001000                                                // peripheral to memory transfer
  #define DMACCConfiguration_P2P         0x00001800                                                // source peripheral to destination peripheral transfer
  #define DMACCConfiguration_P2PD        0x00002000                                                // source peripheral to destination peripheral transfer - controlled by destination peripheral
  #define DMACCConfiguration_M2PP        0x00002800                                                // memory to peripheral transfer - controlled by peripheral
  #define DMACCConfiguration_P2MP        0x00003000                                                // peripheral to memory transfer - controlled by peripheral
  #define DMACCConfiguration_P2PS        0x00003800                                                // source peripheral to destination peripheral transfer - controlled by source peripheral
  #define DMACCConfiguration_ITC         0x00008000                                                // terminal count interrupt mask
  #define DMACCConfiguration_L           0x00010000                                                // locked transfers
  #define DMACCConfiguration_A           0x00020000                                                // active (read-only)
  #define DMACCConfiguration_H           0x00040000                                                // halt

#define DMACC1SrcAddr                    *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x120)           // Channel 1 Source Address Register
#define DMACC1DestAddr                   *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x124)           // Channel 1 Destination Address Register
#define DMACC1LLI                        *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x128)           // Channel 1 Linked List Item Register
#define DMACC1Control                    *(unsigned long*)(DMA_CONTROLLER_BLOCK + 0x12c)           // Channel 1 Control Register
#define DMACC1Configuration              *(volatile unsigned long*)(DMA_CONTROLLER_BLOCK + 0x130)  // Channel 1 Configuration Register (bit 17 is read-only


#if defined PHY_MULTI_PORT                                               // {43}
    extern unsigned short _fnMIIread(unsigned char ucPort, unsigned char _mradr);
    extern void _fnMIIwrite(unsigned char ucPort, unsigned char _mradr, unsigned short _mwdata);
#else
    extern unsigned short _fnMIIread(unsigned char _mradr);
    extern void _fnMIIwrite(unsigned char _mradr, unsigned short _mwdata);
#endif

// External Memory Controller
//
#define EMCControl                       *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x000)    // Controls operation of the memory controller
    #define EMC_ENABLE                   0x00000001
#define EMCStatus                        *(volatile unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x004) // Provides EMC status information
#define EMCConfig                        *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x008)    // Configures operation of the memory controller

#define EMCDynamicControl                *(volatile unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x020)    // Controls dynamic memory operation
  #define DYNAMIC_MEM_CLK_EN             0x00000001
  #define DYNAMIC_CLKOUT_CONTINUOUS      0x00000002 
  #define SDRAM_NORMAL_CMD               0x00000000
  #define SDRAM_MODE_CMD                 0x00000080
  #define SDRAM_PRECHARGE_ALL_CMD        0x00000100
  #define SDRAM_NOP_CMD                  0x00000180
#define EMCDynamicRefresh                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x024)    // Controls dynamic memory refresh operation
#define EMCDynamicReadConfig             *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x028)    // Controls the dynamic memory read strategy
  #define READ_DATA_STRATEGY_CLOCK_DLY   0x00000000
  #define READ_DATA_STRATEGY_CMD_DLY     0x00000001
  #define READ_DATA_STRATEGY_CMD_1_DLY   0x00000002
  #define READ_DATA_STRATEGY_CMD_2_DLY   0x00000003

#define EMCDynamicRP                     *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x030)    // Selects the precharge command period
#define EMCDynamicRAS                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x034)    // Selects the active to precharge command period
#define EMCDynamicSREX                   *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x038)    // Selects the self-refresh exit time
#define EMCDynamicAPR                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x03c)    // Selects the last-data-out to active command time
#define EMCDynamicDAL                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x040)    // Selects the data-in to active command time
#define EMCDynamicWR                     *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x044)    // Selects the write recovery time
#define EMCDynamicRC                     *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x048)    // Selects the active to active command period
#define EMCDynamicRFC                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x04c)    // Selects the auto-refresh period
#define EMCDynamicXSR                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x050)    // Selects the exit self-refresh to active command time
#define EMCDynamicRRD                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x054)    // Selects the active bank A to active bank B latency
#define EMCDynamicMRD                    *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x058)    // Selects the load mode register to active command time

#define EMCStaticExtendedWait            *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x080)    // Selects time for long static memory read and write transfers

#define EMCDynamicConfig0                *(volatile unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x100)    // Selects the configuration information for dynamic memory chip select 0
  #define ADDRESS_MAP_16M_16BIT          0x00000680
  #define CS_BUFFER_ENABLE               0x00080000
#define EMCDynamicRasCas0                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x104)    // Selects the RAS and CAS latencies for dynamic memory chip select 0
  #define RAS_LATENCY_ONE_CCLK           0x00000001
  #define RAS_LATENCY_TWO_CCLK           0x00000002
  #define RAS_LATENCY_THREE_CCLK         0x00000003
  #define CAS_LATENCY_ONE_CCLK           0x00000100
  #define CAS_LATENCY_TWO_CCLK           0x00000200
  #define CAS_LATENCY_THREE_CCLK         0x00000300
#define EMCDynamicConfig1                *(volatile unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x120)    // Selects the configuration information for dynamic memory chip select 1
#define EMCDynamicRasCas1                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x124)    // Selects the RAS and CAS latencies for dynamic memory chip select 1

#define EMCDynamicConfig2                *(volatile unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x140)    // Selects the configuration information for dynamic memory chip select 2
#define EMCDynamicRasCas2                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x144)    // Selects the RAS and CAS latencies for dynamic memory chip select 2

#define EMCDynamicConfig3                *(volatile unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x160)    // Selects the configuration information for dynamic memory chip select 3
#define EMCDynamicRasCas3                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x164)    // Selects the RAS and CAS latencies for dynamic memory chip select 3

#define EMCStaticConfig0                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x200)    // Selects the memory configuration for static chip select 0
#define EMCStaticWaitWen0                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x204)    // Selects the delay from chip select 0 to write enable
#define EMCStaticWaitOen0                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x208)    // Selects the delay from chip select 0 or address change, whichever is later, to output enable
#define EMCStaticWaitRd0                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x20c)    // Selects the delay from chip select 0 to a read access
#define EMCStaticWaitPage0               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x210)    // Selects the delay for asynchronous page mode sequential accesses for chip select 0
#define EMCStaticWaitWr0                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x214)    // Selects the delay from chip select 0 to a write access
#define EMCStaticWaitTurn0               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x218)    // Selects the number of bus turnaround cycles for chip select 0

#define EMCStaticConfig1                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x220)    // Selects the memory configuration for static chip select 0
#define EMCStaticWaitWen1                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x224)    // Selects the delay from chip select 0 to write enable
#define EMCStaticWaitOen1                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x228)    // Selects the delay from chip select 0 or address change, whichever is later, to output enable
#define EMCStaticWaitRd1                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x22c)    // Selects the delay from chip select 0 to a read access
#define EMCStaticWaitPage1               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x230)    // Selects the delay for asynchronous page mode sequential accesses for chip select 0
#define EMCStaticWaitWr1                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x234)    // Selects the delay from chip select 0 to a write access
#define EMCStaticWaitTurn1               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x238)    // Selects the number of bus turnaround cycles for chip select 0

#define EMCStaticConfig2                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x240)    // Selects the memory configuration for static chip select 0
#define EMCStaticWaitWen2                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x244)    // Selects the delay from chip select 0 to write enable
#define EMCStaticWaitOen2                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x248)    // Selects the delay from chip select 0 or address change, whichever is later, to output enable
#define EMCStaticWaitRd2                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x24c)    // Selects the delay from chip select 0 to a read access
#define EMCStaticWaitPage2               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x250)    // Selects the delay for asynchronous page mode sequential accesses for chip select 0
#define EMCStaticWaitWr2                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x254)    // Selects the delay from chip select 0 to a write access
#define EMCStaticWaitTurn2               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x258)    // Selects the number of bus turnaround cycles for chip select 0

#define EMCStaticConfig3                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x260)    // Selects the memory configuration for static chip select 0
#define EMCStaticWaitWen3                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x264)    // Selects the delay from chip select 0 to write enable
#define EMCStaticWaitOen3                *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x268)    // Selects the delay from chip select 0 or address change, whichever is later, to output enable
#define EMCStaticWaitRd3                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x26c)    // Selects the delay from chip select 0 to a read access
#define EMCStaticWaitPage3               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x270)    // Selects the delay for asynchronous page mode sequential accesses for chip select 0
#define EMCStaticWaitWr3                 *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x274)    // Selects the delay from chip select 0 to a write access
#define EMCStaticWaitTurn3               *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x278)    // Selects the number of bus turnaround cycles for chip select 0

#if defined _LPC24XX || defined LPC2378FBD144
    #define USBPortSel                   *(unsigned long*)(USB_CONTROLLER_BLOCK_LPC2378 + 0x000)   // USB port select register - only LPC2378
      #define PORT_SELECT_MAP_TO_1       0x00000000                                                // USB mapped to CONNECT_1, UP_LED1, D+1 and D-1
      #define PORT_SELECT_MAP_TO_2       0x00000003                                                // USB mapped to CONNECT_2, UP_LED2, D+2 and D-2
#endif

// USB device controller                                                 // {20}
//
#define USBDevIntSt                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x000) // USB Device Interrupt Status (read-only)
  #define DEVICE_STATUS_FRAME            0x00000001                                               // the frame interrupt occurs every 1ms and is used in isochronous packet transfer
  #define DEVICE_STATUS_EP_FAST          0x00000002
  #define DEVICE_STATUS_EP_SLOW          0x00000004
  #define DEVICE_STATUS_DEV_STAT         0x00000008                                               // device status interrupt
  #define DEVICE_STATUS_CCEMPT           0x00000010                                               // command code empty
  #define DEVICE_STATUS_CDFULL           0x00000020                                               // command data register is full
  #define DEVICE_STATUS_RxENDPKT         0x00000040
  #define DEVICE_STATUS_TxENDPKT         0x00000080
  #define DEVICE_STATUS_EP_RLZED         0x00000100                                               // endpoints realised
  #define DEVICE_STATUS_ERR_INT          0x00000200                                               // bus error interrupt
  #define ALL_USB_INTERRUPTS             (DEVICE_STATUS_FRAME | DEVICE_STATUS_EP_FAST | DEVICE_STATUS_EP_SLOW | DEVICE_STATUS_DEV_STAT | DEVICE_STATUS_CCEMPT | DEVICE_STATUS_CDFULL | DEVICE_STATUS_RxENDPKT | DEVICE_STATUS_TxENDPKT | DEVICE_STATUS_EP_RLZED | DEVICE_STATUS_ERR_INT)
  #define ALL_USED_USB_INTERRUPTS        (DEVICE_STATUS_EP_FAST | DEVICE_STATUS_EP_SLOW | DEVICE_STATUS_DEV_STAT)
#define USBDevIntEn                      *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x004)          // USB Device Interrupt Enable
#define USBDevIntClr                     *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x008) // USB Device Interrupt Clear (write-only)
#define USBDevIntSet                     *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x00c) // USB Device Interrupt Set (write-only)
#define USBCmdCode                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x010) // USB Command Code (write-only)
#define USBCmdData                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x014) // USB Command Data (read-only)
#define USBRxData                        *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x018) // USB Receive Data (read-only)
#define USBTxData                        *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x01c) // USB Transmit Data (write-only)
#define USBRxPLen                        *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x020) // USB Receive Packet Length (read-only)
  #define PKT_LENGTH_MASK                0x000003ff
  #define PKT_DV                         0x00000400                                               // data valid
  #define PKT_RDY                        0x00000800                                               // packet ready

#define USBTxPLen                        *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x024) // USB Transmit Packet length (write-only)
#define USBCtrl                          *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x028)          // USB Control
  #define CTRL_RD_EN                     0x00000001
  #define CTRL_WR_EN                     0x00000002

#define USBDevIntPri                     *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x02c) // USB Device Interrupt Priority (write-only)
#define USBEpIntSt                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x030) // USB Endpoint Interrupt Status (read-only)
#define USBEpIntEn                       *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x034)          // USB Endpoint Interrupt Enable
#define USBEpIntClr                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x038) // USB Endpoint Interrupt Clear (write-only)
#define USBEpIntSet                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x03c) // USB Endpoint Interrupt Set (write-only)
#define USBEpIntPri                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x040) // USB Endpoint Interrupt Priority (write-only)
  #define EP0RX                          0x00000001
  #define EP0TX                          0x00000002
  #define EP1RX                          0x00000004
  #define EP1TX                          0x00000008
  #define EP2RX                          0x00000010
  #define EP2TX                          0x00000020
  #define EP3RX                          0x00000040
  #define EP3TX                          0x00000080
  #define EP4RX                          0x00000100
  #define EP4TX                          0x00000200
  #define EP5RX                          0x00000400
  #define EP5TX                          0x00000800
  #define EP6RX                          0x00001000
  #define EP6TX                          0x00002000
  #define EP7RX                          0x00004000
  #define EP7TX                          0x00008000
  #define EP8RX                          0x00010000
  #define EP8TX                          0x00020000
  #define EP9RX                          0x00040000
  #define EP9TX                          0x00080000
  #define EP10RX                         0x00100000
  #define EP10TX                         0x00200000
  #define EP11RX                         0x00400000
  #define EP11TX                         0x00800000
  #define EP12RX                         0x01000000
  #define EP12TX                         0x02000000
  #define EP13RX                         0x04000000
  #define EP13TX                         0x08000000
  #define EP14RX                         0x10000000
  #define EP14TX                         0x20000000
  #define EP15RX                         0x40000000
  #define EP15TX                         0x80000000
  #define DMA_ENDPOINTS                  0xfffffffc
  #define ALL_ENDPOINTS                  0xffffffff
#define USBReEp                          *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x044)          // USB Realize Endpoint
#define USBEpInd                         *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x048) // USB Endpoint Index (write-only) - used to set the Maxpacket size for that endpoint
#define USBMaxPSize                      *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x04c)          // USB Maximum Packet Size
#define USBDMARSt                        *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x050) // USB DMA Request Status (read-only)
#define USBDMARClr                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x054) // USB DMA Request Clear (write-only)
#define USBDMARSet                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x058) // USB DMA Request Set (write-only)
#define USBUDCAH                         *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x080)          // USB UDCA Head
#define USBEpDMASt                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x084) // USB Endpoint DMA Status (read-only)
#define USBEpDMAEn                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x088) // USB Endpoint DMA Enable (write-only)
#define USBEpDMADis                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x08c) // USB Endpoint DMA Disable (write-only)
#define USBDMAIntSt                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x090) // USB DMA Interrupt Status (read-only)
#define USBDMAIntEn                      *(unsigned long*)(USB_CONTROLLER_BLOCK + 0x094)          // USB DMA Interrupt Enable
  #define USB_DMA_EOT                    0x00000001                                               // end of transmission interrupt is enabled
  #define USB_DMA_NDDR                   0x00000002                                               // new DD request interrupt is enabled
  #define USB_SYS_ERROR                  0x00000004                                               // system error interrupt is enabled

#define USBEoTIntSt                      *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0a0) // USB End of Transfer Interrupt Status (read-only)
#define USBEoTIntClr                     *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0a4) // USB End of Transfer Interrupt Clear (write-only)
#define USBEoTIntSet                     *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0a8) // USB End of Transfer Interrupt Set (write-only)
#define USBNDDRIntSt                     *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0ac) // USB New DD Request Interrupt Status (read-only)
#define USBNDDRIntClr                    *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0b0) // USB New DD Request Interrupt Clear (write-only)
#define USBNDDRIntSet                    *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0b4) // USB New DD Request Interrupt Set (write-only)
#define USBSysErrIntSt                   *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0b8) // USB System Error Interrupt Status (read-only)
#define USBSysErrIntClr                  *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0bc) // USB System Error Interrupt Clear (write-only)
#define USBSysErrIntSet                  *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0x0c0) // USB System Error Interrupt Set (write-only)

#define USBClkCtrl                       *(volatile unsigned long*)(USB_CONTROLLER_BLOCK + 0xdf4) // USB Clock Control
  #define DEV_CLK_EN                     0x00000002                                               // device clock enable
  #define PORTSEL_CLK_EN                 0x00000008                                               // only LPC2378
  #define AHB_CLK_EN                     0x00000010                                               // 
#define USBClkSt                         *(unsigned long*)(USB_CONTROLLER_BLOCK + 0xdf8)          // USB Clock Status

#define USB_FIFO_BUFFER_DEPTH 2


typedef struct stDMA_DESCRIPTOR_ISO
{
    unsigned long next_dd_pointer;                                       // pointer to the memory location where the next DMA descriptor will be fetched
    volatile unsigned long dma_mode_status;
    volatile unsigned char *buffer_start_address;
    volatile unsigned long dd_mode_status;
    volatile unsigned long iso_packetsize_mem_address;
    unsigned char          *ptrBufferStart;
    unsigned char          *ptrBufferEnd;
} DMA_DESCRIPTOR_ISO;


#define NORMAL_DMA_MODE               0x00000000
#define ATLE_DMA_MODE                 0x00000001
#define NEXT_DD_VALID                 0x00000002
#define ISOCHRONOUS_ENDPOINT          0x00000008
#define MAX_PACKET_SIZE_MASK          0x0000ffe0
#define DMA_BUFFER_LENGTH_MASK        0xffff0000

#define DD_RETIRED                    0x00000001
#define DD_STATUS_NOT_SERVICED        0x00000000
#define DD_STATUS_BEING_SERVICED      0x00000002
#define DD_STATUS_NORM_COMPLETION     0x00000004
#define DD_STATUS_DATA_UNDERRUN_SHORT 0x00000006
#define DD_STATUS_DATA_OVERRUN        0x00000010
#define DD_STATUS_SYSTEM_ERROR        0x00000012
#define DD_PACKET_VALID               0x00000020
#define DD_PACKET_LS_BYTE_EXTRACTED   0x00000040
#define DD_PACKET_MS_BYTE_EXTRACTED   0x00000080
#define DD_MESSAGE_LENGTH_POS_MASK    0x00003f00
#define DD_PRESENT_DMA_COUNT          0xffff0000


// USB Protocol engine command codes
//
#define USB_CMD_SET_ADDRESS                   0x00d00500
  #define DEV_EN                              0x80
#define USB_CMD_SET_ADDRESS_DATA_PHASE        0x00000100
#define USB_CMD_CONFIG_DEVICE                 0x00d80500
#define USB_CMD_CONFIG_DEVICE_DATA_PHASE_EN   0x00010100
#define USB_CMD_CONFIG_DEVICE_DATA_PHASE_DIS  0x00000100
#define USB_CMD_SET_DEVICE_STATUS             0x00fe0500
#define USB_CMD_SET_DEVICE_STATUS_DATA_PHASE  0x00000100
  #define USB_STATUS_COM                      0x00000001                                    // softconnect state
  #define USB_STATUS_COM_CH                   0x00000002
  #define USB_STATUS_SUS                      0x00000004
  #define USB_STATUS_SUS_CH                   0x00000008
  #define USB_STATUS_RST                      0x00000010
#define USB_CMD_GET_DEVICE_STATUS             0x00fe0500
#define USB_CMD_GET_DEVICE_STATUS_DATA_PHASE  0x00fe0200
#define USB_CMD_SET_EP_STATUS                 0x00400500
  #define USB_CMD_SET_EP_STATUS_DATA_PHASE    0x00000100
  #define USB_CMD_SET_EP_STATUS_STALL         0x00010100
  #define USB_CMD_SET_EP_STATUS_DISABLE_EP    0x00200100
  #define USB_CMD_SET_EP_STATUS_ENABLE_EP     0x00000100
#define USB_CMD_SELECT_EP                     0x00000500
  #define EP_INC                              0x00010000                                    // endpoint number increment
#define USB_CMD_SELECT_EP_DATA_PHASE          0x00000200
  #define USB_EP_FULL_EMPTY                   0x00000001
  #define USB_EP_STALLED                      0x00000002
  #define USB_EP_SETUP_PACKET_RECEIVE         0x00000004
  #define USB_EP_PACKET_OVERWRITTEN           0x00000008
  #define USB_EP_NACKED                       0x00000010
  #define USB_EP_BUFFER_1_FULL                0x00000020
  #define USB_EP_BUFFER_2_FULL                0x00000040
#define USB_CMD_CLEAR_EP_BUFFER               0x00f20500
#define USB_CMD_VALID_EP_BUFFER               0x00fa0500


typedef struct stUSB_END_POINT
{
    unsigned short usEndpointSize;                                       // contains size of endpoint plus some control flags
    unsigned char  ucEndpointRxFlags;                                    // rx flags
    unsigned char  ucEndpointTxFlags;                                    // tx flags
#ifdef _WINDOWS
    unsigned long ulSimFlags;
#endif
} USB_END_POINT;

#define TXPKTRDY                       0x00000010                      // Transmit packet ready to be sent on next IN transfer (set by firmware, cleared by USB)
#define UDP_DIR                        0x00000080                      // Data IN transfers enabled in control data stage
#define RXSETUP                        0x00000004                      // SETUP packet received and stored (set by USB, cleared by firmware)
#define NUMBER_OF_USB_ENDPOINTS        16                              // LPC2XXX supports 16 endpoints logical endpoints

extern void fnUSB_command(unsigned long ulUSB_CMD, unsigned long ulCommandData);

#define VALIDATE_NEW_CONFIGURATION() fnUSB_command(USB_CMD_CONFIG_DEVICE, USB_CMD_CONFIG_DEVICE_DATA_PHASE_EN)

// Macros for setting and clearing configuration bits and then waiting until they have been confirmed
//
#define SYNCHRONISE_CONTROL_SET(bits)    
#define SYNCHRONISE_CONTROL_CLEAR(bits)  

#define SYNCHRONISE_CONTROL_SET_(bits)   
#define SYNCHRONISE_CONTROL_CLEAR_(bits) 

#define SET_CONTROL_DIRECTION(ptrUSB_HW, ucFirstByte)
#define CLEAR_SETUP(ptrUSB_HW)


// Macro for sending a zero data token
// start the transmission and wait for the flag to be set, when simulating mark that the data buffer is empty
//
#define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref) fnSendUSB_FIFO(0, 0, iEndpoint_ref, ptrUSB_HW)



// This hardware specific structure allows generic USB routines to control specific hardware using macros
//
typedef struct stUSB_HW
{
    unsigned long  ulRxControl;
    volatile unsigned long *ptr_ulUSB_BDControl;                         // pointer to the presently valid buffer descriptor control entry
    USB_END_POINT *ptrEndpoint;
    volatile unsigned char  *ptrDatBuffer;                               // pointer to the next transmission buffer pointer
    unsigned short usLength;                                             // length information
    unsigned char  ucUSBAddress;                                         // our USB address
    unsigned char  ucUSBAddressValid;
} USB_HW;

#define FNSEND_USB_DATA(buffer_addr, length, Endpoint, hw)      fnSendUSB_FIFO(buffer_addr, length, Endpoint, hw); _SIM_USB(USB_SIM_TX, Endpoint, hw)


// LCD Controller
//
#define LCD_TIMH                         *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x000)    // Horizontal Timing Control register
#define LCD_TIMV                         *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x004)    // Vertical Timing Control register
#define LCD_POL                          *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x008)    // Clock and Signal Polarity Control register
  #define INVERT_VERT_SYNC               0x00000800
  #define INVERT_HORIZ_SYNC              0x00001000
  #define INVERT_PANEL_CLOCK             0x00002000
  #define BYPASS_PIXEL_CLOCK_DIVIDER     0x04000000
#define LCD_LE                           *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x00c)    // Line End Control register
#define LCD_UPBASE                       *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x010)    // Upper Panel Frame Base Address register
#define LCD_LPBASE                       *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x014)    // Lower Panel Frame Base Address register
#define LCD_CTRL                         *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0x018) // LCD Control Register
  #define LCD_EN                         0x00000001
  #define PIXEL_1                        0x00000000
  #define PIXEL_2                        0x00000002
  #define PIXEL_4                        0x00000004
  #define PIXEL_8                        0x00000006
  #define PIXEL_16                       0x00000008
  #define PIXEL_24                       0x0000000a                                         // TFT panel only
  #define PIXEL_16_5_6_5                 0x0000000c                                         // 5:6:5 mode
  #define PIXEL_12_4_4_4                 0x0000000e                                         // 4:4:4 mode
  #define LCD_STN_BW                     0x00000010                                         // STN monochrome
  #define TFT_DISPLAY                    0x00000020
  #define MONOCHROME_4BIT                0x00000000
  #define MONOCHROME_8BIT                0x00000040
  #define DUAL_PANEL                     0x00000080
  #define COLOUR_RGB                     0x00000000
  #define COLOUR_BRG                     0x00000100
  #define BEBO                           0x00000200                                         // Big Endian Byte Order
  #define BEPO                           0x00000400                                         // Big Endian Pixel Order
  #define LCD_PWR                        0x00000800                                         // LCD power enable
  #define VCOMP_START_VSYNC              0x00000000
  #define VCOMP_START_BACK_PORCH         0x00001000
  #define VCOMP_START_ACTIVE_VIDEO       0x00002000
  #define VCOMP_START_FRONT_PORCH        0x00003000
  #define WATERMARK_4                    0x00000000
  #define WATERMARK_8                    0x00010000
#define LCD_INTMSK                       *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0x01c)    // Interrupt Mask register
#define LCD_INTRAW                       *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0x020) // RAW Interrupt Status Mask register (Read-only)
#define LCD_INTSTAT                      *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0x024) // Masked Interrupt Status register (Read-only)
#define LCD_INTCLR                       *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0x028) // Interrupt Clear register (Write-only)
#define LCD_UPCURR                       *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0x02c) // Upper Panel Current Address Value register (Read-only)
#define LCD_LPCURR                       *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0x030) // Lower Panel Current Address Value register (Read-only)

#define LCD_PAL_ADD                      (unsigned long*)(LCD_CONTROLLER_BLOCK + 0x200)    // 256x16-bit Color Palette registers

#define CRSR_IMG_ADD                     (unsigned long*)(LCD_CONTROLLER_BLOCK + 0x800)    // Cursor Image registers

#define CRSR_CTRL                        *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc00)   // Cursor Control register
#define CRSR_CFG                         *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc04)   // Cursor Configuration register
#define CRSR_PAL0                        *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc08)   // Cursor Palette register 0
#define CRSR_PAL1                        *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc0c)   // Cursor Palette register 1
#define CRSR_XY                          *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc10)   // Cursor XY Position register
#define CRSR_CLIP                        *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc14)   // Cursor Clip Position register

#define CRSR_INTMSK                      *(unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc20)   // Cursor Interrupt Mask register
#define CRSR_INTCLR                      *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc24)   // Cursor Interrupt Clear register (Write-only)
#define CRSR_INTRAW                      *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc28)   // Cursor RAW Interrupt Status register (Read-only)
#define CRSR_INTSTAT                     *(volatile unsigned long*)(LCD_CONTROLLER_BLOCK + 0xc2c)   // Cursor Masked Interrupt Status register (Read-only)



// Software references used when setting up interrupts
//
#define PORT_INTERRUPT            0
#define TIMER_INTERRUPT           1                                      // {5}
#define ADC_INTERRUPT             2                                      // {27}

#define RTC_TIME_SETTING          2                                      // {1}
#define RTC_TICK_SEC              3
#define RTC_TICK_MIN              4
#define RTC_TICK_HOUR             5
#define RTC_TICK_DAY              6
#define RTC_ALARM_TIME            7
#define RTC_STOPWATCH_GO          8
#define RTC_GET_TIME              9
#define RTC_DISABLE               0x80
#define RTC_INITIALISATION        0x40
#define RTC_PCLK                  0x20                                   // run RTC from PCLK rather than from 32kHz oscillator

#define PWM0                      10                                     // {24}
#define PWM1                      11

#define TIMER_PWM_1               1
#define TIMER_PWM_2               2
#define TIMER_PWM_3               3
#define TIMER_PWM_4               4
#define TIMER_PWM_5               5
#define TIMER_PWM_6               6

#define PWM_OUTPUT_PORT_1         (1 << 3)
#define PWM_OUTPUT_PORT_2         (2 << 3)
#define PWM_OUTPUT_PORT_3         (3 << 3)



#define TIMER_SINGLE_SHOT         0x0000                                 // {11}
#define TIMER_PWM_MAT0            0x0001
#define TIMER_PWM_MAT1            0x0002
#define TIMER_PWM_MAT2            0x0004
#define TIMER_PERIODIC            0x0010
#define TIMER_US_VALUE            0x0020
#define TIMER_MS_VALUE            0x0000
#define TIMER_STOP_PWM_MAT0       0x0040
#define TIMER_STOP_PWM_MAT1       0x0080
#define TIMER_STOP_PWM_MAT2       0x0100
#define TIMER_DONT_DISTURB        0x0200


#define _PWM_PERCENT(percent_pwm, frequency_value)       ((frequency_value * percent_pwm)/100)
#define _PWM_TENTH_PERCENT(percent_pwm, frequency_value) ((frequency_value * percent_pwm)/1000)
#define TIMER_MS_DELAY(msec)           ((PCLK/4000)*msec)
#define TIMER_US_DELAY(usec)           ((PCLK/4000000)*usec)
#define TIMER_FREQUENCY_VALUE(hertz)   (1000000 / hertz)


// Define interrupt setup structures to suit this processor
//
typedef struct stINTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set

  unsigned char    int_port_sense;                                       // PULLUP_ON, PULLDOWN_ON, PULLUP_DOWN_OFF, IRQ_RISING_EDGE etc.
  unsigned char    int_port;                                             // The port. PORT_0 or PORT_2 have interrupt support - EXTINT also allowed
  unsigned long    int_port_bits;                                        // The input bits
} INTERRUPT_SETUP;

typedef struct stTIMER_INTERRUPT_SETUP                                   // {5}
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set

  unsigned long    timer_value;                                          // the ms delay value
#ifdef _HW_TIMER_MODE
  unsigned long    pwm_value;                                            // pwm value
  unsigned short   timer_mode;                                           // {11} timer mode
#else
  unsigned long    timer_us_value;                                       // us value when delay is under 1ms
#endif
  unsigned char    timer_reference;                                      // hardware timer to use (0, 1, 2, 3)
} TIMER_INTERRUPT_SETUP;

typedef struct stRTC_SETUP                                               // {42}
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned short   usYear;
  unsigned char    ucDayOfMonth;
  unsigned char    ucMonthOfYear;
  unsigned char    ucDayOfWeek;
  unsigned char    command;                                              // Identifier for command to perform
  unsigned char    ucHours;
  unsigned char    ucMinutes;
  unsigned char    ucSeconds;
} RTC_SETUP;



#define PORT_0              0                                            // {4}
#define PORT_1              1
#define PORT_2              2
#define PORT_3              3
#define PORT_4              4

#define EXTERNALINT         10                                           // external interrupt rather than a port interrupt

#define PULLUP_DOWN_OFF     0x01
#define PULLUP_ON           0x02 
#define PULLDOWN_ON         0x04
#define IRQ_RISING_EDGE     0x08
#define IRQ_FALLING_EDGE    0x10
#define IRQ_BOTH_EDGES      (IRQ_RISING_EDGE | IRQ_FALLING_EDGE)
#define IRQ_DISABLE_INT     0x20
#define IRQ_LOW_LEVEL       0x40                                         // level sensitive interrupts only possible on external interrupts
#define IRQ_HIGH_LEVEL      0x80

// Define interrupt setup structure to suit this processor               {27}
//
typedef struct stADC_SETUP
{
  void ( *int_handler )( void );                                         // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set
  unsigned long    int_adc_single_ended_inputs;                          // the ADC inputs to be sampled (one bit per channel, up to 8 channels possible)
  unsigned long    int_adc_mode;                                         // single or multi-shot and trigger type
  unsigned short   SamplingSpeed;                                        // use ADC_SAMPLING_SPEED(speed) to set speed - maximum speed 4500000 - slowest limited by divider
  unsigned short   int_sequence_count;                                   // the number of sequences to be sampled
  unsigned short  *int_adc_result;                                       // location to save samples to
} ADC_SETUP;

#define ADC_CHANNEL_0                    0x00000001
#define ADC_CHANNEL_1                    0x00000002
#define ADC_CHANNEL_2                    0x00000004
#define ADC_CHANNEL_3                    0x00000008
#define ADC_CHANNEL_4                    0x00000010
#define ADC_CHANNEL_5                    0x00000020
#define ADC_CHANNEL_6                    0x00000040
#define ADC_CHANNEL_7                    0x00000080
#define ADC_CONFIGURE_ADC                0x00000100
#define ADC_CONTINUOUS                   0x00000200
#define ADC_SINGLE_SHOT_SW               0x00000000

#define ADC_SAMPLING_SPEED(speed)        (unsigned short)((((ADC_CLOCK + (speed/2))/speed) - 1) << 8) // ADC sampling speed - maximum 4.5MHz - slowest limited by divider

#ifdef _LPC21XX                                                          // {3}
    #if defined _LPC214X || defined _LPC213X                             // {16}{30}
        #define PORTS_AVAILABLE  2
        #define TIMERS_AVAILABLE 2
        #define CHIP_HAS_UARTS   2
        #define CHIP_HAS_IIC     2
    #else                                                                // LPC2101..LPC2106
        #define PORTS_AVAILABLE  1
        #define TIMERS_AVAILABLE 4
        #define CHIP_HAS_UARTS   2
        #if defined LPC2101 || defined LPC2102 || defined LPC2103
            #define CHIP_HAS_IIC 2
        #else
            #define CHIP_HAS_IIC 1
        #endif
    #endif
#else
    #define PORTS_AVAILABLE  5
    #define TIMERS_AVAILABLE 4
    #define CHIP_HAS_UARTS   4
    #define CHIP_HAS_IIC     3
#endif
#define PORT_WIDTH           32

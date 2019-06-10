/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	    LPC17XX.h
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    25.10.2011 Add LPC1788
    17.11.2011 Modify RTC_SETUP                                          {1}
    18.11.2011 Add _EXCEPTION(x)                                         {2}
    24.02.2012 Make some UART control members volatile                   {3}
    05.09.2012 Add ADC registers                                         {4}
    06.10.2012 Add _HW_TIMER_MODE for compatibility with LPC2xxx projects{5}
    06.10.2012 Add ADC                                                   {6}
    07.10.2012 Add SD card interface                                     {7}
    15.11.2012 Correct LPC1788 USB address, add USBPortSel and LPC1788 USB ports {8}

*/

#ifdef _WINDOWS
    #include "simLPC17XX.h"
    #define _SIM_PORT_CHANGE   fnSimPorts(-1);
    #define _EXCEPTION(x)      *(unsigned char *)0 = 0                   // generate exception when simulating {2}
#else
    #define _EXCEPTION(x)                                                // ignore on target
    #ifdef COMPILE_IAR
        #if defined COMPILE_IAR5
            #include <intrinsics.h>
        #else
            #include <intrinsic.h>
        #endif
    #endif
    #define _SIM_PORT_CHANGE
#endif


#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary
#define _ALIGN_HEAP_4                                                    // ensure long word alignment
#define _LITTLE_ENDIAN                                                   // compile project in LITTLE ENDIAN mode since the LPC17xx is fixed in this mode
#define SDCARD_MALLOC(x) uMalloc(x)                                      // use standard uMalloc()

#define IRC_FREQUENCY    4000000                                         // internal RC nominal frequncy

#define SAVE_COMPLETE_FLASH                                              // when simulating, save complete flash contents and not just the file system contents

#ifdef DISABLE_PLL
    #define MASTER_CLOCK CRYSTAL_FREQ
#elif defined LPC1788
    #define MASTER_CLOCK (PLL_OUTPUT_FREQ_INT/(POST_DIVIDE * PLL_DIV))
#else
    #define MASTER_CLOCK (PLL_OUTPUT_FREQ_INT/POST_DIVIDE)
#endif

#if MASTER_CLOCK > 100000000                                            // define the optimal FLASH wait states depending on the system clock
    #define FLASH_CPU_CLOCKS FLASHTIM_6_CLOCKS
#elif MASTER_CLOCK > 80000000                                           // define the optimal FLASH wait states depending on the system clock
    #define FLASH_CPU_CLOCKS FLASHTIM_5_CLOCKS
#elif MASTER_CLOCK > 60000000
    #define FLASH_CPU_CLOCKS FLASHTIM_4_CLOCKS
#elif MASTER_CLOCK > 40000000
    #define FLASH_CPU_CLOCKS FLASHTIM_3_CLOCKS
#elif MASTER_CLOCK > 20000000
    #define FLASH_CPU_CLOCKS FLASHTIM_2_CLOCKS
#else
    #define FLASH_CPU_CLOCKS FLASHTIM_1_CLOCKS
#endif

#define CCLK             MASTER_CLOCK
#define PLL_OUTPUT_FREQ  MASTER_CLOCK
#define USB_CLOCK        48000000                                        // for USB to work this must be the case. It is assumed that it is correct if used.

#define PCLK             (MASTER_CLOCK/PCLK_DIVIDE)


#if defined LPC1788
    #define UART_CLOCK       (PCLK)
    #define SSP_CLOCK        (PCLK)
    #define IIC_CLOCK        (PCLK)
    #define CAN_CLOCK        (PCLK)
    #define GPIO_CLOCK       (PCLK)
    #define RIT_CLOCK        (PCLK)
    #define TIMER_CLOCK      (PCLK)
    #define ADC_CLOCK        (PCLK)                                      // {4}
#else
    #if UART_CLOCK_DIVIDE == 1
        #define PCLKSEL0_UART   (PCLK_UART0_CCLK_1 | PCLK_UART1_CCLK_1)
        #define PCLKSEL1_UART   (PCLK_UART2_CCLK_1 | PCLK_UART3_CCLK_1)
    #elif UART_CLOCK_DIVIDE == 2
        #define PCLKSEL0_UART   (PCLK_UART0_CCLK_2 | PCLK_UART1_CCLK_2)
        #define PCLKSEL1_UART   (PCLK_UART2_CCLK_2 | PCLK_UART3_CCLK_2)
    #elif UART_CLOCK_DIVIDE == 4
        #define PCLKSEL0_UART   (PCLK_UART0_CCLK_4 | PCLK_UART1_CCLK_4)
        #define PCLKSEL1_UART   (PCLK_UART2_CCLK_4 | PCLK_UART3_CCLK_4)
    #elif UART_CLOCK_DIVIDE == 8
        #define PCLKSEL0_UART   (PCLK_UART0_CCLK_8 | PCLK_UART1_CCLK_8)
        #define PCLKSEL1_UART   (PCLK_UART2_CCLK_8 | PCLK_UART3_CCLK_8)
    #else
        #error UART clock divider is not valid
    #endif
    #define UART_CLOCK       (PCLK/UART_CLOCK_DIVIDE)

    #if SSP_CLOCK_DIVIDE == 1
        #define PCLKSEL0_SSP   (PCLK_SSP1_CCLK_1)
        #define PCLKSEL1_SSP   (PCLK_SSP0_CCLK_1)
    #elif SSP_CLOCK_DIVIDE == 2
        #define PCLKSEL0_SSP   (PCLK_SSP1_CCLK_2)
        #define PCLKSEL1_SSP   (PCLK_SSP0_CCLK_2)
    #elif SSP_CLOCK_DIVIDE == 4
        #define PCLKSEL0_SSP   (PCLK_SSP1_CCLK_4)
        #define PCLKSEL1_SSP   (PCLK_SSP0_CCLK_4)
    #elif SSP_CLOCK_DIVIDE == 8
        #define PCLKSEL0_SSP   (PCLK_SSP1_CCLK_8)
        #define PCLKSEL1_SSP   (PCLK_SSP0_CCLK_8)
    #else
        #error SSP clock divider is not valid
    #endif
    #define SSP_CLOCK        (PCLK/SSP_CLOCK_DIVIDE)

    #if IIC_CLOCK_DIVIDE == 1
        #define PCLKSEL0_IIC   (PCLK_I2C0_CCLK_1)
        #define PCLKSEL1_IIC   (PCLK_I2C1_CCLK_1 | PCLK_I2C2_CCLK_1)
    #elif IIC_CLOCK_DIVIDE == 2
        #define PCLKSEL0_IIC   (PCLK_I2C0_CCLK_2)
        #define PCLKSEL1_IIC   (PCLK_I2C1_CCLK_2 | PCLK_I2C2_CCLK_2)
    #elif IIC_CLOCK_DIVIDE == 4
        #define PCLKSEL0_IIC   (PCLK_I2C0_CCLK_4)
        #define PCLKSEL1_IIC   (PCLK_I2C1_CCLK_4 | PCLK_I2C2_CCLK_4)
    #elif IIC_CLOCK_DIVIDE == 8
        #define PCLKSEL0_IIC   (PCLK_I2C0_CCLK_8)
        #define PCLKSEL1_IIC   (PCLK_I2C1_CCLK_8 | PCLK_I2C2_CCLK_8)
    #else
        #error I2C clock divider is not valid
    #endif
    #define IIC_CLOCK        (PCLK/IIC_CLOCK_DIVIDE)

    #if CAN_CLOCK_DIVIDE == 1
        #define PCLKSEL0_CAN   (PCLK_CAN1_CCLK_1 | PCLK_CAN2_CCLK_1)
    #elif CAN_CLOCK_DIVIDE == 2
        #define PCLKSEL0_CAN   (PCLK_CAN1_CCLK_2 | PCLK_CAN2_CCLK_2)
    #elif CAN_CLOCK_DIVIDE == 4
        #define PCLKSEL0_CAN   (PCLK_CAN1_CCLK_4 | PCLK_CAN2_CCLK_4)
    #elif CAN_CLOCK_DIVIDE == 6
        #define PCLKSEL0_CAN   (PCLK_CAN1_CCLK_6 | PCLK_CAN2_CCLK_6)
    #else
        #error CAN clock divider is not valid
    #endif
    #define CAN_CLOCK        (PCLK/CAN_CLOCK_DIVIDE)

    #if GPIO_CLOCK_DIVIDE == 1
        #define PCLKSEL1_GPIO   (PCLK_GPIO_CCLK_1)
    #elif GPIO_CLOCK_DIVIDE == 2
        #define PCLKSEL1_GPIO   (PCLK_GPIO_CCLK_2)
    #elif GPIO_CLOCK_DIVIDE == 4
        #define PCLKSEL1_GPIO   (PCLK_GPIO_CCLK_4)
    #elif GPIO_CLOCK_DIVIDE == 8
        #define PCLKSEL1_GPIO   (PCLK_GPIO_CCLK_8)
    #else
        #error GPIO clock divider is not valid
    #endif
    #define GPIO_CLOCK        (PCLK/GPIO_CLOCK_DIVIDE)

    #if RIT_CLOCK_DIVIDE == 1
        #define PCLKSEL1_RIT   (PCLK_RIT_CCLK_1)
    #elif RIT_CLOCK_DIVIDE == 2
        #define PCLKSEL1_RIT   (PCLK_RIT_CCLK_2)
    #elif RIT_CLOCK_DIVIDE == 4
        #define PCLKSEL1_RIT   (PCLK_RIT_CCLK_4)
    #elif RIT_CLOCK_DIVIDE == 8
        #define PCLKSEL1_RIT   (PCLK_RIT_CCLK_8)
    #else
        #error RIT clock divider is not valid
    #endif
    #define RIT_CLOCK        (PCLK/RIT_CLOCK_DIVIDE)

    #if TIMER_CLOCK_DIVIDE == 1
        #define PCLKSEL0_TIMER   (PCLK_TIMER0_CCLK_1 | PCLK_TIMER1_CCLK_1 | PCLK_PWM1_CCLK_1)
        #define PCLKSEL1_TIMER   (PCLK_TIMER2_CCLK_1 | PCLK_TIMER3_CCLK_1)
    #elif TIMER_CLOCK_DIVIDE == 2
        #define PCLKSEL0_TIMER   (PCLK_TIMER0_CCLK_2 | PCLK_TIMER1_CCLK_2 | PCLK_PWM1_CCLK_2)
        #define PCLKSEL1_TIMER   (PCLK_TIMER2_CCLK_2 | PCLK_TIMER3_CCLK_2)
    #elif TIMER_CLOCK_DIVIDE == 4
        #define PCLKSEL0_TIMER   (PCLK_TIMER0_CCLK_4 | PCLK_TIMER1_CCLK_4 | PCLK_PWM1_CCLK_4)
        #define PCLKSEL1_TIMER   (PCLK_TIMER2_CCLK_4 | PCLK_TIMER3_CCLK_4)
    #elif TIMER_CLOCK_DIVIDE == 8
        #define PCLKSEL0_TIMER   (PCLK_TIMER0_CCLK_8 | PCLK_TIMER1_CCLK_8 | PCLK_PWM1_CCLK_8)
        #define PCLKSEL1_TIMER   (PCLK_TIMER2_CCLK_8 | PCLK_TIMER3_CCLK_8)
    #else
        #error TIMER clock divider is not valid
    #endif
    #define TIMER_CLOCK      (PCLK/TIMER_CLOCK_DIVIDE)

    #define CONFIG_PCLKSEL0  (PCLKSEL0_UART | PCLKSEL0_SSP | PCLKSEL0_IIC | PCLKSEL0_CAN | PCLKSEL0_TIMER)
    #define CONFIG_PCLKSEL1  (PCLKSEL1_UART | PCLKSEL1_SSP | PCLKSEL1_IIC | PCLKSEL1_GPIO | PCLKSEL1_RIT | PCLKSEL1_TIMER)
    // Other peripherals are clocked at PCK/4
    //
#endif

#define CAST_POINTER_ARITHMETIC unsigned long                            // LPC17XX uses 32 bit pointers
#define SAVE_COMPLETE_FLASH                                              // when simulating, save complete flash contents and not just the file system contents

#ifdef LPC1788
    #define PLL_FREQUENCY_CONFIGURATION      (((PLL_MUL - 1) & 0x1f) | ((((PLL_DIV/2) - 1) & 0x3) << 5))
    #define CPU_CLOCK_CONFIGURATION          (POST_DIVIDE & 0x01f)
#else
    #define PLL_FREQUENCY_CONFIGURATION      (((PLL_MUL - 1) & 0x7fff) | (((OSC_DIVIDE - 1) & 0xff) << 16))
    #define CPU_CLOCK_CONFIGURATION          ((POST_DIVIDE - 1) & 0x0ff)
#endif
#define PLL_USB_FREQUENCY_CONFIGURATION      (((PLL_USB_MUL - 1) & 0x1f) | (USB_POST_DIVIDE << 5))
#define USB_FREQUENCY_CONFIGURATION          (((PLL_OUTPUT_FREQ_INT/USB_CLOCK) - 1) & 0x0f) // setting required to generate 48MHz USB clock from PLL0


#ifdef LPC1788
    #define DEVICE_208_PIN
  //#define DEVICE_144_PIN
    #define SIZE_OF_FLASH (512*1024)                                     // 512k FLASH
    #define SIZE_OF_RAM   (64 * 1024)                                    // 64k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_EEPROM  (4 * 1024)                                   // 4k EEPROM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1769            // Part id which can be read via ISP/IAP
#elif defined LPC1769
    #define DEVICE_100_PIN
    #define SIZE_OF_FLASH (512*1024)                                     // 512k FLASH
    #define SIZE_OF_RAM   (32 * 1024)                                    // 32k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1769            // Part id which can be read via ISP/IAP
#elif defined LPC1768
    #define DEVICE_100_PIN
    #define SIZE_OF_FLASH (512*1024)                                     // 512k FLASH
    #define SIZE_OF_RAM   (32 * 1024)                                    // 32k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1768            // Part id which can be read via ISP/IAP
#elif defined LPC1766
    #define DEVICE_100_PIN
    #define SIZE_OF_FLASH (256*1024)                                     // 256k FLASH
    #define SIZE_OF_RAM   (32 * 1024)                                    // 32k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1766            // Part id which can be read via ISP/IAP
#elif defined LPC1765
    #define DEVICE_100_PIN
    #define SIZE_OF_FLASH (256*1024)                                     // 256k FLASH - no Ethernet
    #define SIZE_OF_RAM   (32 * 1024)                                    // 32k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1765            // Part id which can be read via ISP/IAP
#elif defined LPC1764
    #define DEVICE_100_PIN
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH
    #define SIZE_OF_RAM   (16 * 1024)                                    // 16k CPU SRAM and 16k AHB SRAM
    #define SIZE_OF_AHB_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1764            // Part id which can be read via ISP/IAP
#elif defined LPC1759
    #define DEVICE_80_PIN
    #define SIZE_OF_FLASH (512*1024)                                     // 512k FLASH
    #define SIZE_OF_RAM   (32 * 1024)                                    // 32k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1759            // Part id which can be read via ISP/IAP
#elif defined LPC1758
    #define DEVICE_80_PIN
    #define SIZE_OF_FLASH (512*1024)                                     // 512k FLASH
    #define SIZE_OF_RAM   (32 * 1024)                                    // 32k CPU SRAM plus 32k peripheral AHB SRAM
    #define SIZE_OF_AHB_RAM (32 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1758            // Part id which can be read via ISP/IAP
#elif defined LPC1756
    #define DEVICE_80_PIN
    #define SIZE_OF_FLASH (256*1024)                                     // 256k FLASH - no Ethernet
    #define SIZE_OF_RAM   (16 * 1024)                                    // 16k CPU SRAM and 16k AHB SRAM
    #define SIZE_OF_AHB_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1756            // Part id which can be read via ISP/IAP
#elif defined LPC1754
    #define DEVICE_80_PIN
    #define SIZE_OF_FLASH (128*1024)                                     // 128k FLASH - no Ethernet
    #define SIZE_OF_RAM   (16 * 1024)                                    // 16k CPU SRAM and 16k AHB SRAM
    #define SIZE_OF_AHB_RAM (16 * 1024)
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1754            // Part id which can be read via ISP/IAP
#elif defined LPC1752
    #define DEVICE_80_PIN
    #define SIZE_OF_FLASH (64*1024)                                      // 64k FLASH - no Ethernet
    #define SIZE_OF_RAM   (16 * 1024)                                    // 16k CPU SRAM and no peripheral AHB SRAM
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1752            // Part id which can be read via ISP/IAP
#elif defined LPC1751
    #define DEVICE_80_PIN
    #define SIZE_OF_FLASH (32*1024)                                      // 32k FLASH - no Ethernet
    #define SIZE_OF_RAM   (8 * 1024)                                     // 8k CPU SRAM and no peripheral AHB SRAM
    #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC1751            // Part id which can be read via ISP/IAP
#endif

#define PART_ID_LPC1751                      0x25001110
#define PART_ID_LPC1752                      0x25001121
#define PART_ID_LPC1754                      0x25011722
#define PART_ID_LPC1756                      0x25011723
#define PART_ID_LPC1758                      0x25013f37
#define PART_ID_LPC1759                      0x25113737
#define PART_ID_LPC1764                      0x26011922
#define PART_ID_LPC1765                      0x26013733
#define PART_ID_LPC1766                      0x26013f33
#define PART_ID_LPC1768                      0x26013f37
#define PART_ID_LPC1769                      0x26113f37


#define INTERRUPT_COUNT                  31
#define FLASH_START_ADDRESS              0                               // up to 512k
#define START_OF_FLASH                   FLASH_START_ADDRESS
#define RAM_START_ADDRESS                0x10000000                      // up to 32k
#if defined SIZE_OF_AHB_RAM                                              // only 64k devices have additional SRAM on the AHB-lite buses
    #if defined LPC1788
        #define ETHERNET_RAM_START_ADDRESS   0x20000000                  // up to 32k
        #ifndef ETHERNET_RAM_SIZE
            #define ETHERNET_RAM_SIZE        (16 * 1024)                 // default is to use the first bank for Ethernet and the second for USB
        #endif
        #define USB_RAM_START                0x20004000                  // 16k
        #define USB_RAM_SIZE                 ((16 * 1024) - 1024)
    #else
        #define ETHERNET_RAM_START_ADDRESS   0x2007c000                  // up to 32k
        #ifndef ETHERNET_RAM_SIZE
            #define ETHERNET_RAM_SIZE        (16 * 1024)                 // default is to use the first bank for Ethernet and the second for USB
        #endif
        #define USB_RAM_START                0x20080000                  // 16k
        #define USB_RAM_SIZE                 ((16 * 1024) - 1024)
    #endif
    #define NVIC_RAM_START_ADDRESS       (USB_RAM_START + USB_RAM_SIZE)  // last 1024 bytes of USB space used for vector table
#endif

#define FLASH_GRANULARITY_SMALL          (4 * 1024)                      // small blocks
#define FLASH_GRANULARITY_LARGE          (32 * 1024)                     // large blocks

#define FLASH_SECTOR_0_OFFSET            0
#define FLASH_SECTOR_SIZE_0              FLASH_GRANULARITY_SMALL         // initially small blocks
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
#define FLASH_SECTOR_SIZE_7              FLASH_GRANULARITY_SMALL         // last sector in 32k part
#define FLASH_SECTOR_8_OFFSET            0x8000
#define FLASH_SECTOR_SIZE_8              FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_9_OFFSET            0x9000
#define FLASH_SECTOR_SIZE_9              FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_10_OFFSET           0xa000
#define FLASH_SECTOR_SIZE_10             FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_11_OFFSET           0xb000
#define FLASH_SECTOR_SIZE_11             FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_12_OFFSET           0xc000
#define FLASH_SECTOR_SIZE_12             FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_13_OFFSET           0xd000
#define FLASH_SECTOR_SIZE_13             FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_14_OFFSET           0xe000
#define FLASH_SECTOR_SIZE_14             FLASH_GRANULARITY_SMALL
#define FLASH_SECTOR_15_OFFSET           0xf000
#define FLASH_SECTOR_SIZE_15             FLASH_GRANULARITY_SMALL         // end small blocks and last sector in 64k part


#define FLASH_SECTOR_16_OFFSET           0x10000
#define FLASH_SECTOR_SIZE_16             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_17_OFFSET           0x18000
#define FLASH_SECTOR_SIZE_17             FLASH_GRANULARITY_LARGE         // last sector in 128k part
#define FLASH_SECTOR_18_OFFSET           0x20000
#define FLASH_SECTOR_SIZE_18             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_19_OFFSET           0x28000
#define FLASH_SECTOR_SIZE_19             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_20_OFFSET           0x30000
#define FLASH_SECTOR_SIZE_20             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_21_OFFSET           0x38000
#define FLASH_SECTOR_SIZE_21             FLASH_GRANULARITY_LARGE         // last sector in 256k part
#define FLASH_SECTOR_22_OFFSET           0x40000
#define FLASH_SECTOR_SIZE_22             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_23_OFFSET           0x48000
#define FLASH_SECTOR_SIZE_23             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_24_OFFSET           0x50000
#define FLASH_SECTOR_SIZE_24             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_25_OFFSET           0x58000
#define FLASH_SECTOR_SIZE_25             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_26_OFFSET           0x60000
#define FLASH_SECTOR_SIZE_26             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_27_OFFSET           0x68000
#define FLASH_SECTOR_SIZE_27             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_28_OFFSET           0x70000
#define FLASH_SECTOR_SIZE_28             FLASH_GRANULARITY_LARGE
#define FLASH_SECTOR_29_OFFSET           0x78000
#define FLASH_SECTOR_SIZE_29             FLASH_GRANULARITY_LARGE

#define FLASH_LINE_SIZE                      16                          // lines made up of quad words (128 bits)
#define FLASH_ROW_SIZE                       256                         // FLASH writes are performed in rows of this size

#define SDRAM_ADDR                           0xa0000000

typedef void (*IAP)(unsigned long [], unsigned long []);
#ifdef _WINDOWS
    extern void fnSimNXPFlash(unsigned long [], unsigned long []);
    #define IAP_LOCATION fnSimNXPFlash
#else
    #define IAP_LOCATION (IAP)0x1fff1ff1                                 // thumb mode execution address of In-circuit Application Programming routine
#endif

#define IAP_STACK_SIZE                            32                     // top 32 bytes in SRAM are reserved for use by the IAP routines

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




typedef struct stRESET_VECTOR
{
    void  *ptrResetSP;                                                   // initial stack pointer
    void  (*ptrResetPC)(void);                                           // initial program counter
} RESET_VECTOR;


// LPC17XX interrupts
//
typedef struct stPROCESSOR_IRQ
{
    void  (*irq_Watchdog_Timer)(void);                                   // 0
    void  (*irq_Timer0)(void);                                           // 1
    void  (*irq_Timer1)(void);                                           // 2
    void  (*irq_Timer2)(void);                                           // 3
    void  (*irq_Timer3)(void);                                           // 4
    void  (*irq_UART0)(void);                                            // 5
    void  (*irq_UART1)(void);                                            // 6
    void  (*irq_UART2)(void);                                            // 7
    void  (*irq_UART3)(void);                                            // 8
    void  (*irq_PWM1)(void);                                             // 9
    void  (*irq_I2C0)(void);                                             // 10
    void  (*irq_I2C1)(void);                                             // 11
    void  (*irq_I2C2)(void);                                             // 12
    void  (*irq_SPI)(void);                                              // 13
    void  (*irq_SSP0)(void);                                             // 14
    void  (*irq_SSP1)(void);                                             // 15
    void  (*irq_PLL0)(void);                                             // 16 - main PLL
    void  (*irq_RTC)(void);                                              // 17
    void  (*irq_EINT0)(void);                                            // 18
    void  (*irq_EINT1)(void);                                            // 19
    void  (*irq_EINT2)(void);                                            // 20
    void  (*irq_EINT3)(void);                                            // 21
    void  (*irq_ADC)(void);                                              // 22
    void  (*irq_BOD)(void);                                              // 23
    void  (*irq_USB)(void);                                              // 24
    void  (*irq_CAN)(void);                                              // 25
    void  (*irq_GPDMA)(void);                                            // 26
    void  (*irq_I2S)(void);                                              // 27
    void  (*irq_Ethernet_Controller)(void);                              // 28
    void  (*irq_RITINT)(void);                                           // 29
    void  (*irq_MotorControlPWM)(void);                                  // 30
    void  (*irq_QuadEncoder)(void);                                      // 31
    void  (*irq_PLL1)(void);                                             // 32 - USB PLL
    void  (*irq_USB_Activity)(void);                                     // 33
    void  (*irq_CAN_Activity)(void);                                     // 34
#ifdef LPC1788
    void  (*irq_UART4)(void);                                            // 35
    void  (*irq_SSP2)(void);                                             // 36
    void  (*irq_LCD)(void);                                              // 37
    void  (*irq_GPIO)(void);                                             // 38
    void  (*irq_PWM0)(void);                                             // 39
    void  (*irq_EEPROM)(void);                                           // 40
#endif
} PROCESSOR_IRQ;

#define LAST_PROCESSOR_IRQ  irq_CAN_Activity

typedef struct stVECTOR_TABLE
{
    RESET_VECTOR  reset_vect;
    void  (*ptrNMI)(void);
    void  (*ptrHardFault)(void);
    void  (*ptrMemManagement)(void);
    void  (*ptrBusFault)(void);
    void  (*ptrUsageFault)(void);
    unsigned long ptrReserved1[4];
    void  (*ptrSVCall)(void);
    void  (*ptrDebugMonitor)(void);
    unsigned long ptrReserved2;
    void  (*ptrPendSV)(void);
    void  (*ptrSysTick)(void);
    PROCESSOR_IRQ processor_interrupts;                                  // length is processor specific
} VECTOR_TABLE;

typedef struct stVECTOR_TABLE_MINIMUM
{
    RESET_VECTOR  reset_vect;
    unsigned long ulDummyNMI;
    unsigned long ulDummyHardFault;
    unsigned long ulDummyMemMan;
    unsigned long ulDummyBusFault;
    unsigned long ulDummyUsageFault;
#ifndef COMPILE_IAR5
    unsigned long ulLPC1XXX_CS;
#endif
} VECTOR_TABLE_MINIMUM;



#define VECTOR_SIZE                      (sizeof(VECTOR_TABLE))
#if defined LPC1788
    #define CHECK_VECTOR_SIZE            228                             // (16 + 40 + 1) = 57) * 4 - adequate for this processor
#else
    #define CHECK_VECTOR_SIZE            204                             // (16 + 34 + 1) = 50) * 4 - adequate for this processor
#endif




// Peripheral Register Blocks
//
#ifdef _WINDOWS
  #define FAST_GPIO_BLOCK                ((unsigned char *)(&ucLPC17xx.ucFastGPIO))
  #define WATCHDOG_BLOCK                 ((unsigned char *)(&ucLPC17xx.ucWDOG))
  #define TIMER_PERIPHERAL_BLOCK_0       ((unsigned char *)(&ucLPC17xx.ucSimTimer0))
  #define TIMER_PERIPHERAL_BLOCK_1       ((unsigned char *)(&ucLPC17xx.ucSimTimer1))
  #define UART_BLOCK_0                   ((unsigned char *)(&ucLPC17xx.ucSimUart0))
  #define UART_BLOCK_1                   ((unsigned char *)(&ucLPC17xx.ucSimUart1))
  #define I2C_BLOCK_0                    ((unsigned char *)(&ucLPC17xx.ucIIC0))
  #define RTC_BLOCK                      ((unsigned char *)(&ucLPC17xx.ucRTC))
  #define LEGACY_GPIO_BLOCK              ((unsigned char *)(&ucLPC17xx.ucLegacyGPIO))
  #define PIN_CONNECT_BLOCK              ((unsigned char *)(&ucLPC17xx.ucPIN_CON))
  #define SSP_BLOCK_1                    ((unsigned char *)(&ucLPC17xx.ucSSP1))
  #define ADC_BLOCK                      ((unsigned char *)(&ucLPC17xx.ucADC))
  #define I2C_BLOCK_1                    ((unsigned char *)(&ucLPC17xx.ucIIC1))
  #define SSP_BLOCK_0                    ((unsigned char *)(&ucLPC17xx.ucSSP0))
  #define TIMER_PERIPHERAL_BLOCK_2       ((unsigned char *)(&ucLPC17xx.ucSimTimer2))
  #define TIMER_PERIPHERAL_BLOCK_3       ((unsigned char *)(&ucLPC17xx.ucSimTimer3))
  #define UART_BLOCK_2                   ((unsigned char *)(&ucLPC17xx.ucSimUart2))
  #define UART_BLOCK_3                   ((unsigned char *)(&ucLPC17xx.ucSimUart3))
    #ifdef LPC1788
  #define UART_BLOCK_4                   ((unsigned char *)(&ucLPC17xx.ucSimUart4))
    #endif
  #define I2C_BLOCK_2                    ((unsigned char *)(&ucLPC17xx.ucIIC2))
  #define RIT_BLOCK                      ((unsigned char *)(&ucLPC17xx.ucRIT))
  #define RTC_RAM_BLOCK                  ((unsigned long *)(&ulLPC17xx.ulRTC_ram))   // 2k battery backed up SRAM - only long word accesses supported

  #define SYSTEM_CONTROL_MODULE          ((unsigned char *)(&ucLPC17xx.ucSCM))
    #ifdef LPC1788
      #define MCI_BLOCK                  ((unsigned char *)(&ucLPC17xx.ulMCI)) // {7} SD card interface
      #define LCD_CONTROLLER_BLOCK       ((unsigned char *)(&ucLPC17xx.ucLCD))
      #define EXT_MEMORY_CONTROLLER_BLOCK ((unsigned char *)(&ucLPC17xx.ucEMC))
    #endif
  #define ETHERNET_CONTROLLER_BLOCK      ((unsigned char *)(&ucLPC17xx.ucEMAC))
#if defined LPC1788
      #define USB_CONTROLLER_BLOCK_LPC1788 ((unsigned char *)(&ucLPC17xx.ulLPC1788USB)) // {8}
#endif
  #define USB_CONTROLLER_BLOCK           ((unsigned char *)(&ucLPC17xx.ucUSB))

  #define VIC_PERIPHERAL_BLOCK           ((unsigned char *)(&ucLPC17xx.ucVIC))
  #define VIC_ADDRESS_BLOCK              ((unsigned char *)(&ucLPC17xx.ucVICAddress))

  #define CORTEX_M3_BLOCK                ((unsigned char *)(&ucLPC17xx.CORTEX_M3))
#else
  #ifdef LPC1788
    #define DMA_CONTROLLER_BLOCK         0x20080000                      // AHB peripherals
    #define ETHERNET_CONTROLLER_BLOCK    0x20084000
    #define LCD_CONTROLLER_BLOCK         0x20088000
    #define USB_CONTROLLER_BLOCK_LPC1788 0x2008c110                      // {8}
    #define USB_CONTROLLER_BLOCK         0x2008c200                      // {8}
    #define CRC_ENGINE_BLOCK             0x20090000
    #define FAST_GPIO_BLOCK              0x20098000
    #define EXT_MEMORY_CONTROLLER_BLOCK  0x2009c000
  #else
    #define FAST_GPIO_BLOCK              0x2009c000
  #endif
  #define WATCHDOG_BLOCK                 0x40000000                      // APB peripherals
  #define TIMER_PERIPHERAL_BLOCK_0       0x40004000
  #define TIMER_PERIPHERAL_BLOCK_1       0x40008000
  #define UART_BLOCK_0                   0x4000c000
  #define UART_BLOCK_1                   0x40010000
  #define PWM_BLOCK_0                    0x40014000
  #define PWM_BLOCK_1                    0x40018000
  #define I2C_BLOCK_0                    0x4001c000
  #define RTC_BLOCK                      0x40024000
  #define LEGACY_GPIO_BLOCK              0x40028000
  #define PIN_CONNECT_BLOCK              0x4002c000
  #define SSP_BLOCK_1                    0x40030000
  #define ADC_BLOCK                      0x40034000
  #define CAN_ACCEPT_FILTER_RAM_BLOCK    0x40038000
  #define CAN_ACCEPT_FILTER_BLOCK        0x4003c000
  #define CAN_COMMON_REG_BLOCK           0x40040000
  #define CAN_BLOCK_1                    0x40044000
  #define CAN_BLOCK_2                    0x40048000
  #define I2C_BLOCK_1                    0x4005c000
  #if !defined LPC1788
    #define RTC_RAM_BLOCK                0x40084000                      // 2k battery backed up SRAM - only long word accesses supported
  #endif
  #define SSP_BLOCK_0                    0x40088000                      // APB1 peripherals
  #define DAC_BLOCK                      0x4008c000
  #define TIMER_PERIPHERAL_BLOCK_2       0x40090000
  #define TIMER_PERIPHERAL_BLOCK_3       0x40094000
  #define UART_BLOCK_2                   0x40098000
  #define UART_BLOCK_3                   0x4009c000
    #ifdef LPC1788
  #define UART_BLOCK_4                   0x400a4000
    #endif
  #define I2C_BLOCK_2                    0x400a0000
  #if defined LPC1788
    #define SSP_BLOCK_2                  0x400ac000
    #define MOT_PWM_BLOCK                0x400b8000
    #define QUAD_ENC_BLOCK               0x400bc000
    #define MCI_BLOCK                    0x400c0000
  #else
    #define RIT_BLOCK                    0x400b0000
  #endif
  #define SYSTEM_CONTROL_MODULE          0x400fc000
  #if !defined LPC1788
    #define ETHERNET_CONTROLLER_BLOCK    0x50000000
    #define DMA_CONTROLLER_BLOCK         0x50004000
    #define USB_CONTROLLER_BLOCK         0x5000c200
  #endif

  #define VIC_PERIPHERAL_BLOCK           0xfffff000
  #define VIC_ADDRESS_BLOCK              0xffffff00
  #define CORTEX_M3_BLOCK                0xe000e000
#endif


// FAST GPIOs
//
#define FIO0DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x00)    // Port Direction Control Register
#define FIO0MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x10)    // Fast Mask Register
#define FIO0PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x14)    // Fast Port Pin Register
#define FIO0SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x18)    // Fast Port Output Set Register
#define FIO0CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x1c)    // Fast Port Output Clear Register (Write only)

#define FIO1DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x20)    // Port Direction Control Register
#define FIO1MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x30)    // Fast Mask Register
#define FIO1PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x34)    // Fast Port Pin Register
#define FIO1SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x38)    // Fast Port Output Set Register
#define FIO1CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x3c)    // Fast Port Output Clear Register (Write only)

#define FIO2DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x40)    // Port Direction Control Register
#define FIO2MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x50)    // Fast Mask Register
#define FIO2PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x54)    // Fast Port Pin Register
#define FIO2SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x58)    // Fast Port Output Set Register
#define FIO2CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x5c)    // Fast Port Output Clear Register (Write only)

#define FIO3DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x60)    // Port Direction Control Register
#define FIO3MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x70)    // Fast Mask Register
#define FIO3PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x74)    // Fast Port Pin Register
#define FIO3SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x78)    // Fast Port Output Set Register
#define FIO3CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x7c)    // Fast Port Output Clear Register (Write only)

#define FIO4DIR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x80)    // Port Direction Control Register
#define FIO4MASK                         *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x90)    // Fast Mask Register
#define FIO4PIN                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x94)    // Fast Port Pin Register
#define FIO4SET                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x98)    // Fast Port Output Set Register
#define FIO4CLR                          *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0x9c)    // Fast Port Output Clear Register (Write only)

#ifdef LPC1788
    #define FIO5DIR                      *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0xa0)    // Port Direction Control Register
    #define FIO5MASK                     *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0xb0)    // Fast Mask Register
    #define FIO5PIN                      *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0xb4)    // Fast Port Pin Register
    #define FIO5SET                      *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0xb8)    // Fast Port Output Set Register
    #define FIO5CLR                      *(volatile unsigned long*)(FAST_GPIO_BLOCK + 0xbc)    // Fast Port Output Clear Register (Write only)
#endif

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

  #define PORT5_BIT0                     0x00000001                      // LPC1788
  #define PORT5_BIT1                     0x00000002
  #define PORT5_BIT2                     0x00000004
  #define PORT5_BIT3                     0x00000008
  #define PORT5_BIT4                     0x00000010


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
        // Write full port width
        //
        #define _WRITE_PORT(ref, value)             FIO##ref##PIN = ((FIO##ref##PIN & ~(FIO##ref##DIR)) | ((value) & (FIO##ref##DIR))); FIO##ref##SET = (value); _SIM_PORT_CHANGE
        // Write to a port with a mask
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  FIO##ref##PIN = ((FIO##ref##PIN & ~((mask) & FIO##ref##DIR)) | ((value) & (mask & FIO##ref##DIR))); FIO##ref##SET &= ~(mask); FIO##ref##SET |= ((mask) & (value)); FIO##ref##CLR = ~FIO##ref##SET;  _SIM_PORT_CHANGE
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             FIO##ref##SET = (~(FIO##ref##PIN) & (mask)); FIO##ref##CLR = (FIO##ref##PIN & (mask)); _SIM_PORT_CHANGE
    #else
        // Write full port width
        //
        #define _WRITE_PORT(ref, value)             FIO##ref##MASK = 0xffffffff; FIO##ref##PIN = (value); FIO##ref##MASK = 0
        // Write to a port with a mask
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  FIO##ref##MASK = ~(mask); FIO##ref##PIN = (value); FIO##ref##MASK = 0
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             FIO##ref##PIN ^= (mask);  _SIM_PORT_CHANGE
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
#else
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
        // Write full port width
        //
        #define _WRITE_PORT(ref, value)             IO##ref##PIN = ((IO##ref##PIN & ~(IO##ref##DIR)) | ((value) & (IO##ref##DIR))); FIO##ref##SET = (value);  _SIM_PORT_CHANGE
        // Write to a port with a mask
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  IO##ref##PIN = ((IO##ref##PIN & ~(mask & IO##ref##DIR)) | ((value) & ((mask) & IO##ref##DIR)));  IO##ref##SET &= ~(mask); IO##ref##SET |= ((mask) & (value)); IO##ref##CLR = ~IO##ref##SET _SIM_PORT_CHANGE
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             IO##ref##SET = (~(IO##ref##PIN) & (mask)); IO##ref##CLR = (IO##ref##PIN & (mask)); _SIM_PORT_CHANGE
    #else
        // Write full port width
        //
        #define _WRITE_PORT(ref, value)             IO##ref##PIN = (value)
        // Write to a port with a mask
        //
        #define _WRITE_PORT_MASK(ref, value, mask)  IO##ref##PIN = ((IO##ref##PIN & ~(mask & IO##ref##DIR)) | ((value) & (mask & IO##ref##DIR)));  _SIM_PORT_CHANGE
        // Toggle a port with a mask
        //
        #define _TOGGLE_PORT(ref, mask)             IO##ref##PIN ^= (mask); _SIM_PORT_CHANGE
    #endif
    // Read full port width
    //
    #define _READ_PORT(ref)                    (IO##ref##PIN)
    // Read from a port with a mask
    //
    #define _READ_PORT_MASK(ref, mask)         (IO##ref##PIN & (mask));
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
#if defined LPC1788
  #define WDWARNINT                      *(unsigned long*)(WATCHDOG_BLOCK + 0x14) // Watchdog Warning Interrupt compare value
  #define WDWINDOW                       *(unsigned long*)(WATCHDOG_BLOCK + 0x18) // Watchdog Warning compare value
#else
  #define WDCLKSEL                       *(unsigned long*)(WATCHDOG_BLOCK + 0x10) // Watchdog Clock Source Select Register
    #define WD_CLOCK_INT_RC              0x00000000                               // clock watchdog from internal RC clock
    #define WD_CLOCK_PCLK                0x00000001                               // clock watchdog from peripheral clock
    #define WD_CLOCK_RTC                 0x00000002                               // clock watchdog from RTC clock
#endif


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
#define U0LCR                            *(volatile unsigned long*)(UART_BLOCK_0 + 0x0C) // Line Control Register
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

typedef struct stLPC17XX_UART_CONTROL                                    // {3}
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
unsigned long ulRes4;
volatile unsigned long TER;
} LPC17XX_UART_CONTROL;

#define U1RBR_THR                        *(volatile unsigned long*)(UART_BLOCK_1 + 0x00) // Receive Buffer Register (read) / Transmit Holding Register (write)
#define U1DLL                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x00) // Divisor Latch LSB  (DLAB = 1)
#define U1DLM                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x04) // Divisor Latch MSB  (DLAB = 1)
#define U1IER                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x04) // Interrupt Enable Register (DLAB = 0)
#define U1IIR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x08) // Interrupt ID Register (Read Only)
#define U1FCR                            *(volatile unsigned long*)(UART_BLOCK_1 + 0x08) // FIFO Control Register (Write Only)
#define U1LCR                            *(unsigned long*)(UART_BLOCK_1 + 0x0C)          // Line Control Register
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
#define U2LCR                            *(unsigned long*)(UART_BLOCK_2 + 0x0C)          // Line Control Register
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
#define U3LCR                            *(unsigned long*)(UART_BLOCK_3 + 0x0C)          // Line Control Register
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

#ifdef LPC1788
    #define U4RBR_THR                    *(volatile unsigned long*)(UART_BLOCK_4 + 0x00) // Receive Buffer Register (read) / Transmit Holding Register (write)
    #define U4DLL                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x00) // Divisor Latch LSB  (DLAB = 1)
    #define U4DLM                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x04) // Divisor Latch MSB  (DLAB = 1)
    #define U4IER                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x04) // Interrupt Enable Register (DLAB = 0)
    #define U4IIR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x08) // Interrupt ID Register (Read Only)
    #define U4FCR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x08) // FIFO Control Register (Write Only)
    #define U4LCR                        *(unsigned long*)(UART_BLOCK_4 + 0x0C)          // Line Control Register
    #define U4LSR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x14) // Line Status Register (Read Only)
    #define U4SCR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x1C) // Scratch Pad Register
    #define U4ACR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x20) // Auto-baud Control Register
    #define U4ICR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x24) // IrDA Control Register
    #define U4FDR                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x28) // Fractional Divider Register
    #define U4TER                        *(volatile unsigned long*)(UART_BLOCK_4 + 0x30) // Transmit Enable Register
#endif
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

// ADC                                                                                      {4}
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


// Repetitive Interrupt Timer registers
//
#define RICOMPVAL                        *(unsigned long*)(RIT_BLOCK + 0x0)                 // RIT Compare Register
#define RIMASK                           *(unsigned long*)(RIT_BLOCK + 0x4)                 // RIT Mask Register
#define RICTRL                           *(volatile unsigned long*)(RIT_BLOCK + 0x8)        // RIT Control Register
  #define RITINT                         0x00000001                                         // interrupt flag (write 1 to clear)
  #define RITENCLR                       0x00000002                                         // reset counter to zero on compare match
  #define RITENBR                        0x00000004                                         // halt timer when processor halted for debugging
  #define RITEN                          0x00000008                                         // timer enable
#define RICOUNTER                        *(volatile unsigned long*)(RIT_BLOCK + 0xc)        // RIT Counter


// RTC registers
//
#define RTC_ILR                          *(volatile unsigned long*)(RTC_BLOCK + 0x000)      // Interrupt Location Register
  #define RTCCIF                         0x1                                                // Counter Increment Interrupt Block generated an interrupt
  #define RTCALF                         0x2                                                // Alarm Registers generated an interrupt
#ifdef _WINDOWS
    #define RTC_CTC                      *(volatile unsigned long*)(RTC_BLOCK + 0x004)      // Clock Tick Counter Register (Read Only)
#endif
#define RTC_CCR                          *(volatile unsigned long*)(RTC_BLOCK + 0x008)      // Clock Control Register
  #define CCR_CLKEN                      0x01                                               // Enable counters
  #define CCR_CTCRST                     0x02                                               // Clock Tick Counter Reset
  #define CCR_CCALEN                     0x10                                               // Calibration Counter Enable ('0' to enable, '1' to disable)
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
#define RTC_CALIBRATION                  *(unsigned long*)(RTC_BLOCK + 0x040)               // Calibration Value Register
  #define CAL_MASK                       0x0001ffff                                         // finest calibration 0.6591847s/day with maximum value
  #define CAL_DIR_FORWARDS               0x00000000
  #define CAL_DIR_BACKWARDS              0x00020000
#define RTC_GPREG0                       *(unsigned long*)(RTC_BLOCK + 0x044)               // General Purpose Register 0
#define RTC_GPREG1                       *(unsigned long*)(RTC_BLOCK + 0x048)               // General Purpose Register 1
#define RTC_GPREG2                       *(unsigned long*)(RTC_BLOCK + 0x04c)               // General Purpose Register 2
#define RTC_GPREG3                       *(unsigned long*)(RTC_BLOCK + 0x050)               // General Purpose Register 3
#define RTC_GPREG4                       *(unsigned long*)(RTC_BLOCK + 0x054)               // General Purpose Register 4
#define RTC_AUXEN                        *(unsigned long*)(RTC_BLOCK + 0x058)               // RTC Auxiliary Enable Register
#define RTC_AUX                          *(unsigned long*)(RTC_BLOCK + 0x05c)               // RTC Auxiliary Control Register
#define RTC_ALSEC                        *(unsigned long*)(RTC_BLOCK + 0x060)               // Alarm value for seconds
#define RTC_ALMIN                        *(unsigned long*)(RTC_BLOCK + 0x064)               // Alarm value for minutes
#define RTC_ALHOUR                       *(unsigned long*)(RTC_BLOCK + 0x068)               // Alarm value for hours
#define RTC_ALDOM                        *(unsigned long*)(RTC_BLOCK + 0x06c)               // Alarm value for day of week
#define RTC_ALDOW                        *(unsigned long*)(RTC_BLOCK + 0x070)               // Alarm value for day of month
#define RTC_ALDOY                        *(unsigned long*)(RTC_BLOCK + 0x074)               // Alarm value for day of year
#define RTC_ALMON                        *(unsigned long*)(RTC_BLOCK + 0x078)               // Alarm value for months
#define RTC_ALYEAR                       *(unsigned long*)(RTC_BLOCK + 0x07c)               // Alarm value for years


// Legacy GPIO registers
//
#define IO0PIN                           *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x00)        // GPIO Pin value register
#define IO0SET                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x04) // GPIO Output Set register
#define IO0DIR                           *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x08)        // GPIO Port Direction Control Register
#define IO0CLR                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x0c)// GPIO Pin Output Clear register (Write Only)

#define IO1PIN                           *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x10)        // GPIO Pin value register
#define IO1SET                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x14)// GPIO Output Set register
#define IO1DIR                           *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x18)        // GPIO Port Direction Control Register
#define IO1CLR                           *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x1c)// GPIO Pin Output Clear register (Write Only)

#define IOIntStatus                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x80)// GPIO Overall Interrupt Status (read-only) - Port 0
  #define P0int                          0x00000001                                          // At least one pending interrupt on port 0
  #define P2int                          0x00000004                                          // At least one pending interrupt on port 2
#define IOIntStatR_ADD                   (LPC17XX_PORT_INT *)(LEGACY_GPIO_BLOCK + 0x84)
#define IO0IntStatR                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x84)// GPIO Interrupt Status for Rising Edge (read-only) - Port 0
  #define PORTINT_RISING_DETECTED        0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntStatF                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x88)// GPIO Interrupt Status for Falling Edge (read-only) - Port 0
  #define PORTINT_FALLING_DETECTED       0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntClr                        *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0x8c)// GPIO Interrupt Clear (write-only) - Port 0
  #define PORTINT_CLEAR                  0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntEnR                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x90)        // GPIO Interrupt Enable for Rising Edge - Port 0
  #define PORTINT_RISING_ENABLE          0x00000001                                          // each port input has one bit - shift to the bit position to use
#define IO0IntEnF                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0x94)        // GPIO Interrupt Enable for Falling Edge - Port 0
  #define PORTINT_FALLING_ENABLE         0x00000001                                          // each port input has one bit - shift to the bit position to use

#define IO2IntStatR                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0xa4)// GPIO Interrupt Status for Rising Edge (read-only) - Port 0
#define IO2IntStatF                      *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0xa8)// GPIO Interrupt Status for Falling Edge (read-only) - Port 0
#define IO2IntClr                        *(volatile unsigned long*)(LEGACY_GPIO_BLOCK + 0xac)// GPIO Interrupt Clear (write-only) - Port 0
#define IO2IntEnR                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0xb0)        // GPIO Interrupt Enable for Rising Edge - Port 0
#define IO2IntEnF                        *(unsigned long*)(LEGACY_GPIO_BLOCK + 0xb4)        // GPIO Interrupt Enable for Falling Edge - Port 0


typedef struct stLPC17XX_PORT_INT
{
unsigned long IOIntStatR;
unsigned long IOIntStatF;
unsigned long IOIntClr;
unsigned long IOIntEnR;
unsigned long IOIntEnF;
unsigned long ulRes[3];
} LPC17XX_PORT_INT;


#if defined LPC1788
#define IOCON_P0_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x000)
#define IOCON_P0_00                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x000)       // I/O Control Register  for pin P0-00
#define IOCON_P0_01                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x004)       // I/O Control Register  for pin P0-01
#define IOCON_P0_02                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x008)       // I/O Control Register  for pin P0-02
#define IOCON_P0_03                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x00c)       // I/O Control Register  for pin P0-03
#define IOCON_P0_04                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x010)       // I/O Control Register  for pin P0-04
#define IOCON_P0_05                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x014)       // I/O Control Register  for pin P0-05
#define IOCON_P0_06                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x018)       // I/O Control Register  for pin P0-06
#define IOCON_P0_07                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x01c)       // I/O Control Register  for pin P0-07
#define IOCON_P0_08                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x020)       // I/O Control Register  for pin P0-08
#define IOCON_P0_09                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x024)       // I/O Control Register  for pin P0-09
#define IOCON_P0_10                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x028)       // I/O Control Register  for pin P0-10
#define IOCON_P0_11                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x02c)       // I/O Control Register  for pin P0-11
#define IOCON_P0_12                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x030)       // I/O Control Register  for pin P0-12
#define IOCON_P0_13                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x034)       // I/O Control Register  for pin P0-13
#define IOCON_P0_14                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x038)       // I/O Control Register  for pin P0-14
#define IOCON_P0_15                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x03c)       // I/O Control Register  for pin P0-15
#define IOCON_P0_16                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x040)       // I/O Control Register  for pin P0-16
#define IOCON_P0_17                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x044)       // I/O Control Register  for pin P0-17
#define IOCON_P0_18                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x048)       // I/O Control Register  for pin P0-18
#define IOCON_P0_19                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x04c)       // I/O Control Register  for pin P0-19
#define IOCON_P0_20                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x050)       // I/O Control Register  for pin P0-20
#define IOCON_P0_21                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x054)       // I/O Control Register  for pin P0-21
#define IOCON_P0_22                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x058)       // I/O Control Register  for pin P0-22
#define IOCON_P0_23                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x05c)       // I/O Control Register  for pin P0-23
#define IOCON_P0_24                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x060)       // I/O Control Register  for pin P0-24
#define IOCON_P0_25                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x064)       // I/O Control Register  for pin P0-25
#define IOCON_P0_26                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x068)       // I/O Control Register  for pin P0-26
#define IOCON_P0_27                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x06c)       // I/O Control Register  for pin P0-27
#define IOCON_P0_28                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x070)       // I/O Control Register  for pin P0-28
#define IOCON_P0_29                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x074)       // I/O Control Register  for pin P0-29
#define IOCON_P0_30                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x078)       // I/O Control Register  for pin P0-30
#define IOCON_P0_31                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x07c)       // I/O Control Register  for pin P0-31

#define IOCON_P1_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x080)
#define IOCON_P1_00                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x080)       // I/O Control Register  for pin P1-00
#define IOCON_P1_01                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x084)       // I/O Control Register  for pin P1-01
#define IOCON_P1_02                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x088)       // I/O Control Register  for pin P1-02
#define IOCON_P1_03                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x08c)       // I/O Control Register  for pin P1-03
#define IOCON_P1_04                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x090)       // I/O Control Register  for pin P1-04
#define IOCON_P1_05                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x094)       // I/O Control Register  for pin P1-05
#define IOCON_P1_06                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x098)       // I/O Control Register  for pin P1-06
#define IOCON_P1_07                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x09c)       // I/O Control Register  for pin P1-07
#define IOCON_P1_08                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0a0)       // I/O Control Register  for pin P1-08
#define IOCON_P1_09                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0a4)       // I/O Control Register  for pin P1-09
#define IOCON_P1_10                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0a8)       // I/O Control Register  for pin P1-10
#define IOCON_P1_11                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0ac)       // I/O Control Register  for pin P1-11
#define IOCON_P1_12                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0b0)       // I/O Control Register  for pin P1-12
#define IOCON_P1_13                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0b4)       // I/O Control Register  for pin P1-13
#define IOCON_P1_14                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0b8)       // I/O Control Register  for pin P1-14
#define IOCON_P1_15                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0bc)       // I/O Control Register  for pin P1-15
#define IOCON_P1_16                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0c0)       // I/O Control Register  for pin P1-16
#define IOCON_P1_17                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0c4)       // I/O Control Register  for pin P1-17
#define IOCON_P1_18                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0c8)       // I/O Control Register  for pin P1-18
#define IOCON_P1_19                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0cc)       // I/O Control Register  for pin P1-19
#define IOCON_P1_20                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0d0)       // I/O Control Register  for pin P1-20
#define IOCON_P1_21                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0d4)       // I/O Control Register  for pin P1-21
#define IOCON_P1_22                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0d8)       // I/O Control Register  for pin P1-22
#define IOCON_P1_23                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0dc)       // I/O Control Register  for pin P1-23
#define IOCON_P1_24                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0e0)       // I/O Control Register  for pin P1-24
#define IOCON_P1_25                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0e4)       // I/O Control Register  for pin P1-25
#define IOCON_P1_26                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0e8)       // I/O Control Register  for pin P1-26
#define IOCON_P1_27                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0ec)       // I/O Control Register  for pin P1-27
#define IOCON_P1_28                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0f0)       // I/O Control Register  for pin P1-28
#define IOCON_P1_29                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0f4)       // I/O Control Register  for pin P1-29
#define IOCON_P1_30                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0f8)       // I/O Control Register  for pin P1-30
#define IOCON_P1_31                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x0fc)       // I/O Control Register  for pin P1-31

#define IOCON_P2_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x100)
#define IOCON_P2_00                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x100)       // I/O Control Register  for pin P2-00
#define IOCON_P2_01                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x104)       // I/O Control Register  for pin P2-01
#define IOCON_P2_02                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x108)       // I/O Control Register  for pin P2-02
#define IOCON_P2_03                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x10c)       // I/O Control Register  for pin P2-03
#define IOCON_P2_04                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x110)       // I/O Control Register  for pin P2-04
#define IOCON_P2_05                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x114)       // I/O Control Register  for pin P2-05
#define IOCON_P2_06                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x118)       // I/O Control Register  for pin P2-06
#define IOCON_P2_07                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x11c)       // I/O Control Register  for pin P2-07
#define IOCON_P2_08                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x120)       // I/O Control Register  for pin P2-08
#define IOCON_P2_09                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x124)       // I/O Control Register  for pin P2-09
#define IOCON_P2_10                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x128)       // I/O Control Register  for pin P2-10
#define IOCON_P2_11                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x12c)       // I/O Control Register  for pin P2-11
#define IOCON_P2_12                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x130)       // I/O Control Register  for pin P2-12
#define IOCON_P2_13                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x134)       // I/O Control Register  for pin P2-13
#define IOCON_P2_14                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x138)       // I/O Control Register  for pin P2-14
#define IOCON_P2_15                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x13c)       // I/O Control Register  for pin P2-15
#define IOCON_P2_16                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x140)       // I/O Control Register  for pin P2-16
#define IOCON_P2_17                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x144)       // I/O Control Register  for pin P2-17
#define IOCON_P2_18                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x148)       // I/O Control Register  for pin P2-18
#define IOCON_P2_19                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x14c)       // I/O Control Register  for pin P2-19
#define IOCON_P2_20                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x150)       // I/O Control Register  for pin P2-20
#define IOCON_P2_21                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x154)       // I/O Control Register  for pin P2-21
#define IOCON_P2_22                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x158)       // I/O Control Register  for pin P2-22
#define IOCON_P2_23                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x15c)       // I/O Control Register  for pin P2-23
#define IOCON_P2_24                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x160)       // I/O Control Register  for pin P2-24
#define IOCON_P2_25                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x164)       // I/O Control Register  for pin P2-25
#define IOCON_P2_26                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x168)       // I/O Control Register  for pin P2-26
#define IOCON_P2_27                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x16c)       // I/O Control Register  for pin P2-27
#define IOCON_P2_28                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x170)       // I/O Control Register  for pin P2-28
#define IOCON_P2_29                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x174)       // I/O Control Register  for pin P2-29
#define IOCON_P2_30                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x178)       // I/O Control Register  for pin P2-30
#define IOCON_P2_31                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x17c)       // I/O Control Register  for pin P2-31

#define IOCON_P3_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x180)
#define IOCON_P3_00                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x180)       // I/O Control Register  for pin P3-00
#define IOCON_P3_01                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x184)       // I/O Control Register  for pin P3-01
#define IOCON_P3_02                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x188)       // I/O Control Register  for pin P3-02
#define IOCON_P3_03                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x18c)       // I/O Control Register  for pin P3-03
#define IOCON_P3_04                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x190)       // I/O Control Register  for pin P3-04
#define IOCON_P3_05                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x194)       // I/O Control Register  for pin P3-05
#define IOCON_P3_06                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x198)       // I/O Control Register  for pin P3-06
#define IOCON_P3_07                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x19c)       // I/O Control Register  for pin P3-07
#define IOCON_P3_08                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1a0)       // I/O Control Register  for pin P3-08
#define IOCON_P3_09                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1a4)       // I/O Control Register  for pin P3-09
#define IOCON_P3_10                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1a8)       // I/O Control Register  for pin P3-10
#define IOCON_P3_11                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1ac)       // I/O Control Register  for pin P3-11
#define IOCON_P3_12                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1b0)       // I/O Control Register  for pin P3-12
#define IOCON_P3_13                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1b4)       // I/O Control Register  for pin P3-13
#define IOCON_P3_14                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1b8)       // I/O Control Register  for pin P3-14
#define IOCON_P3_15                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1bc)       // I/O Control Register  for pin P3-15
#define IOCON_P3_16                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1c0)       // I/O Control Register  for pin P3-16
#define IOCON_P3_17                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1c4)       // I/O Control Register  for pin P3-17
#define IOCON_P3_18                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1c8)       // I/O Control Register  for pin P3-18
#define IOCON_P3_19                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1cc)       // I/O Control Register  for pin P3-19
#define IOCON_P3_20                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1d0)       // I/O Control Register  for pin P3-20
#define IOCON_P3_21                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1d4)       // I/O Control Register  for pin P3-21
#define IOCON_P3_22                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1d8)       // I/O Control Register  for pin P3-22
#define IOCON_P3_23                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1dc)       // I/O Control Register  for pin P3-23
#define IOCON_P3_24                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1e0)       // I/O Control Register  for pin P3-24
#define IOCON_P3_25                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1e4)       // I/O Control Register  for pin P3-25
#define IOCON_P3_26                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1e8)       // I/O Control Register  for pin P3-26
#define IOCON_P3_27                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1ec)       // I/O Control Register  for pin P3-27
#define IOCON_P3_28                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1f0)       // I/O Control Register  for pin P3-28
#define IOCON_P3_29                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1f4)       // I/O Control Register  for pin P3-29
#define IOCON_P3_30                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1f8)       // I/O Control Register  for pin P3-30
#define IOCON_P3_31                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1fc)       // I/O Control Register  for pin P3-31

#define IOCON_P4_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x200)
#define IOCON_P4_00                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x200)       // I/O Control Register  for pin P4-00
#define IOCON_P4_01                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x204)       // I/O Control Register  for pin P4-01
#define IOCON_P4_02                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x208)       // I/O Control Register  for pin P4-02
#define IOCON_P4_03                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x20c)       // I/O Control Register  for pin P4-03
#define IOCON_P4_04                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x210)       // I/O Control Register  for pin P4-04
#define IOCON_P4_05                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x214)       // I/O Control Register  for pin P4-05
#define IOCON_P4_06                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x218)       // I/O Control Register  for pin P4-06
#define IOCON_P4_07                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x21c)       // I/O Control Register  for pin P4-07
#define IOCON_P4_08                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x220)       // I/O Control Register  for pin P4-08
#define IOCON_P4_09                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x224)       // I/O Control Register  for pin P4-09
#define IOCON_P4_10                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x228)       // I/O Control Register  for pin P4-10
#define IOCON_P4_11                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x22c)       // I/O Control Register  for pin P4-11
#define IOCON_P4_12                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x230)       // I/O Control Register  for pin P4-12
#define IOCON_P4_13                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x234)       // I/O Control Register  for pin P4-13
#define IOCON_P4_14                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x238)       // I/O Control Register  for pin P4-14
#define IOCON_P4_15                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x23c)       // I/O Control Register  for pin P4-15
#define IOCON_P4_16                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x240)       // I/O Control Register  for pin P4-16
#define IOCON_P4_17                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x244)       // I/O Control Register  for pin P4-17
#define IOCON_P4_18                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x248)       // I/O Control Register  for pin P4-18
#define IOCON_P4_19                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x24c)       // I/O Control Register  for pin P4-19
#define IOCON_P4_20                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x250)       // I/O Control Register  for pin P4-20
#define IOCON_P4_21                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x254)       // I/O Control Register  for pin P4-21
#define IOCON_P4_22                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x258)       // I/O Control Register  for pin P4-22
#define IOCON_P4_23                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x25c)       // I/O Control Register  for pin P4-23
#define IOCON_P4_24                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x260)       // I/O Control Register  for pin P4-24
#define IOCON_P4_25                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x264)       // I/O Control Register  for pin P4-25
#define IOCON_P4_26                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x268)       // I/O Control Register  for pin P4-26
#define IOCON_P4_27                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x26c)       // I/O Control Register  for pin P4-27
#define IOCON_P4_28                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x270)       // I/O Control Register  for pin P4-28
#define IOCON_P4_29                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x274)       // I/O Control Register  for pin P4-29
#define IOCON_P4_30                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x278)       // I/O Control Register  for pin P4-30
#define IOCON_P4_31                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x27c)       // I/O Control Register  for pin P4-31

#define IOCON_P5_ADD                     (unsigned long*)(PIN_CONNECT_BLOCK + 0x280)
#define IOCON_P5_00                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x280)       // I/O Control Register  for pin P5-00
#define IOCON_P5_01                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x284)       // I/O Control Register  for pin P5-01
#define IOCON_P5_02                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x288)       // I/O Control Register  for pin P5-02
#define IOCON_P5_03                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x28c)       // I/O Control Register  for pin P5-03
#define IOCON_P5_04                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x290)       // I/O Control Register  for pin P5-04

  #define IOCON_FUNCTION_MASK            0x00000007                      // U type has only this
  #define IOCON_MODE_NO_PULLUP_DOWN      0x00000000                      // not U/I type
  #define IOCON_MODE_PULL_DOWN           0x00000008
  #define IOCON_MODE_PULL_UP             0x00000010
  #define IOCON_MODE_REPEATER            0x00000018                      // not A/I/U type
  #define IOCON_MODE_HYS                 0x00000020                      // not A/I/U type
  #define IOCON_MODE_INV                 0x00000040
  #define IOCON_MODE_ADMODE              0x00000080                      // only A type
  #define IOCON_MODE_FILTER_ON           0x00000000
  #define IOCON_MODE_FILTER_OFF          0x00000100                      // only A/I/W type
  #define IOCON_MODE_SLEW_FAST           0x00000200                      // not A/U type
  #define IOCON_MODE_OPEN_DRAIN          0x00000400                      // not U/I
  #define IOCON_MODE_DAC_ENABLED         0x00010000                      // only A type

  #define IOCON_TYPE_D_DEFAULT          (IOCON_MODE_HYS | IOCON_MODE_PULL_UP)
  #define IOCON_TYPE_W_DEFAULT          (IOCON_MODE_ADMODE | IOCON_MODE_HYS)
  #define IOCON_TYPE_A_DEFAULT          (IOCON_MODE_FILTER_OFF | IOCON_MODE_ADMODE | IOCON_MODE_HYS | IOCON_MODE_PULL_UP)
  #define IOCON_TYPE_I_DEFAULT          (IOCON_MODE_NO_PULLUP_DOWN)
  #define IOCON_TYPE_U_DEFAULT          (IOCON_MODE_NO_PULLUP_DOWN)


// Port 0 functions
//
#define _UART0_P0_0_TXD                  0x00000004
#define _UART0_P0_1_RXD                  0x00000004
#define _UART3_P0_0_TXD                  0x00000002
#define _UART3_P0_1_RXD                  0x00000002
#define _UART0_P0_2_TXD                  0x00000001
#define _UART0_P0_3_RXD                  0x00000001
#define _UART3_P0_2_TXD                  0x00000002
#define _UART3_P0_3_RXD                  0x00000002
#define _UART2_P0_10_TXD                 0x00000001
#define _UART2_P0_11_RXD                 0x00000001
#define _UART1_P0_15_TXD                 0x00000001
#define _UART1_P0_16_RXD                 0x00000001
#define _UART4_P0_22_TXD                 0x00000003
#define _UART3_P0_25_TXD                 0x00000003
#define _UART3_P0_26_RXD                 0x00000003

#define _LCD_P0_4_VD0                    0x00000007
#define _LCD_P0_5_VD1                    0x00000007
#define _LCD_P0_6_VD8                    0x00000007
#define _LCD_P0_7_VD9                    0x00000007
#define _LCD_P0_8_VD16                   0x00000007
#define _LCD_P0_9_VD17                   0x00000007

#define _ADC0_P0_23_AIN0                 0x00000001                      // {6}
#define _ADC0_P0_24_AIN1                 0x00000001
#define _ADC0_P0_25_AIN2                 0x00000001
#define _ADC0_P0_26_AIN3                 0x00000001
#define _ADC0_P0_12_AIN6                 0x00000003
#define _ADC0_P0_13_AIN7                 0x00000003

#define _I2C1_P0_0_SDA                  (0x00000003 | IOCON_MODE_OPEN_DRAIN)
#define _I2C1_P0_1_SCL                  (0x00000003 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P0_10_SDA                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P0_11_SCL                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C1_P0_19_SDA                 (0x00000003 | IOCON_MODE_OPEN_DRAIN)
#define _I2C1_P0_20_SCL                 (0x00000003 | IOCON_MODE_OPEN_DRAIN)
#define _I2C0_P0_27_SDA                 (0x00000001 | IOCON_MODE_OPEN_DRAIN)
#define _I2C0_P0_28_SCL                 (0x00000001 | IOCON_MODE_OPEN_DRAIN)

#define _USB2_P0_12_PWR                  0x00000001                      // {8}
#define _USB2_P0_13_UP_LED               0x00000001
#define _USB2_P0_14_CONNECT              0x00000003
#define _USB1_P0_29_DPLUS                0x00000001
#define _USB1_P0_30_DMINUS               0x00000001
#define _USB2_P0_31_DPLUS                0x00000001


// Port 1 functions
//
#define _LCD_P1_20_VD10                  0x00000007
#define _LCD_P1_21_VD11                  0x00000007
#define _LCD_P1_22_VD12                  0x00000007
#define _LCD_P1_23_VD13                  0x00000007
#define _LCD_P1_24_VD14                  0x00000007
#define _LCD_P1_25_VD15                  0x00000007
#define _LCD_P1_26_VD20                  0x00000007
#define _LCD_P1_27_VD21                  0x00000007
#define _LCD_P1_28_VD22                  0x00000007
#define _LCD_P1_29_VD23                  0x00000007

#define _ADC0_P1_30_AIN4                 0x00000003                      // {6}
#define _ADC0_P1_31_AIN5                 0x00000003


#define _UART4_P1_29_TXD                 0x00000005

#define _ENET_P1_0_TXD0                  0x00000001
#define _ENET_P1_1_TXD1                  0x00000001
#define _ENET_P1_4_TXEN                  0x00000001
#define _ENET_P1_8_CRS_DV                0x00000001
#define _ENET_P1_9_RXD0                  0x00000001
#define _ENET_P1_10_RXD1                 0x00000001
#define _ENET_P1_14_RXER                 0x00000001
#define _ENET_P1_15_REFCK                0x00000001
#define _ENET_P1_16_MDC                  0x00000001
#define _ENET_P1_17_MDIO                 0x00000001

#define _I2C0_P1_30_SDA                 (0x00000004 | IOCON_MODE_OPEN_DRAIN)
#define _I2C0_P1_31_SCL                 (0x00000004 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P1_15_SDA                 (0x00000003 | IOCON_MODE_OPEN_DRAIN)

#define _SDCARD_P1_2_SD_CLK              0x00000002                      // {7}
#define _SDCARD_P1_3_SD_CMD              0x00000002
#define _SDCARD_P1_5_SD_PWR              0x00000002
#define _SDCARD_P1_6_SD_DAT0             0x00000002
#define _SDCARD_P1_7_SD_DAT1             0x00000002
#define _SDCARD_P1_11_SD_DAT2            0x00000002
#define _SDCARD_P1_12_SD_DAT3            0x00000002

#define _SDCARD_P0_19_SD_CLK             0x00000002
#define _SDCARD_P0_20_SD_CMD             0x00000002
#define _SDCARD_P0_21_SD_PWR             0x00000002
#define _SDCARD_P0_22_SD_DAT0            0x00000002
#define _SDCARD_P2_11_SD_DAT1            0x00000002
#define _SDCARD_P2_12_SD_DAT2            0x00000002
#define _SDCARD_P2_13_SD_DAT3            0x00000002

#define _USB1_P1_18_UP_LED               0x00000001                      // {8}
#define _USB1_P1_19_PWR                  0x00000002
#define _USB1_P1_30_VBUS                (0x00000002 | IOCON_MODE_NO_PULLUP_DOWN)

//Port 2 functions
//
#define _EMC_P2_16_CAS                   0x00000001
#define _EMC_P2_17_RAS                   0x00000001
#define _EMC_P2_18_CLK0                  0x00000001
#define _EMC_P2_20_DYCS0                 0x00000001
#define _EMC_P2_24_CKE0                  0x00000001
#define _EMC_P2_28_DQM0                  0x00000001
#define _EMC_P2_29_DQM1                  0x00000001
#define _EMC_P2_30_DQM2                  0x00000001
#define _EMC_P2_31_DQM3                  0x00000001

#define _LCD_P2_0_PWR                    0x00000007
#define _LCD_P2_1_LE                     0x00000007
#define _LCD_P2_2_DCLK                   0x00000007
#define _LCD_P2_3_FP                     0x00000007
#define _LCD_P2_4_ENAB_M                 0x00000007
#define _LCD_P2_5_LP                     0x00000007
#define _LCD_P2_6_VD4                    0x00000007
#define _LCD_P2_7_VD5                    0x00000007
#define _LCD_P2_8_VD6                    0x00000007
#define _LCD_P2_9_VD7                    0x00000007
#define _LCD_P2_11_CLKIN                 0x00000007
#define _LCD_P2_12_VD18                  0x00000007
#define _LCD_P2_13_VD19                  0x00000007

#define _UART1_P2_0_TXD                  0x00000002
#define _UART1_P2_1_RXD                  0x00000002
#define _UART2_P2_8_TXD                  0x00000002
#define _UART2_P2_9_RXD                  0x00000002
#define _UART4_P2_9_RXD                  0x00000003

#define _I2C1_P2_14_SDA                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C1_P2_15_SCL                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P2_30_SDA                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P2_31_SCL                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)

#define _USB1_P2_9_CONNECT               0x00000001                      // {8}

// Port 3 functions
//
#define _EMC_P3_0_DATA_0                 0x00000001
#define _EMC_P3_1_DATA_1                 0x00000001
#define _EMC_P3_2_DATA_2                 0x00000001
#define _EMC_P3_3_DATA_3                 0x00000001
#define _EMC_P3_4_DATA_4                 0x00000001
#define _EMC_P3_5_DATA_5                 0x00000001
#define _EMC_P3_6_DATA_6                 0x00000001
#define _EMC_P3_7_DATA_7                 0x00000001
#define _EMC_P3_8_DATA_8                 0x00000001
#define _EMC_P3_9_DATA_9                 0x00000001
#define _EMC_P3_10_DATA_10               0x00000001
#define _EMC_P3_11_DATA_11               0x00000001
#define _EMC_P3_12_DATA_12               0x00000001
#define _EMC_P3_13_DATA_13               0x00000001
#define _EMC_P3_14_DATA_14               0x00000001
#define _EMC_P3_15_DATA_15               0x00000001
#define _EMC_P3_16_DATA_16               0x00000001
#define _EMC_P3_17_DATA_17               0x00000001
#define _EMC_P3_18_DATA_18               0x00000001
#define _EMC_P3_19_DATA_19               0x00000001
#define _EMC_P3_20_DATA_20               0x00000001
#define _EMC_P3_21_DATA_21               0x00000001
#define _EMC_P3_22_DATA_22               0x00000001
#define _EMC_P3_23_DATA_23               0x00000001
#define _EMC_P3_24_DATA_24               0x00000001
#define _EMC_P3_25_DATA_25               0x00000001
#define _EMC_P3_26_DATA_26               0x00000001
#define _EMC_P3_27_DATA_27               0x00000001
#define _EMC_P3_28_DATA_28               0x00000001
#define _EMC_P3_29_DATA_29               0x00000001
#define _EMC_P3_30_DATA_30               0x00000001
#define _EMC_P3_31_DATA_31               0x00000001

#define _UART1_P3_16_TXD                 0x00000003
#define _UART1_P3_17_RXD                 0x00000003

// Port 4 functions
//
#define _EMC_P4_0_ADDR_0                 0x00000001
#define _EMC_P4_1_ADDR_1                 0x00000001
#define _EMC_P4_2_ADDR_2                 0x00000001
#define _EMC_P4_3_ADDR_3                 0x00000001
#define _EMC_P4_4_ADDR_4                 0x00000001
#define _EMC_P4_5_ADDR_5                 0x00000001
#define _EMC_P4_6_ADDR_6                 0x00000001
#define _EMC_P4_7_ADDR_7                 0x00000001
#define _EMC_P4_8_ADDR_8                 0x00000001
#define _EMC_P4_9_ADDR_9                 0x00000001
#define _EMC_P4_10_ADDR_10               0x00000001
#define _EMC_P4_11_ADDR_11               0x00000001
#define _EMC_P4_12_ADDR_12               0x00000001
#define _EMC_P4_13_ADDR_13               0x00000001
#define _EMC_P4_14_ADDR_14               0x00000001
#define _EMC_P4_15_ADDR_15               0x00000001
#define _EMC_P4_16_ADDR_16               0x00000001
#define _EMC_P4_17_ADDR_17               0x00000001
#define _EMC_P4_18_ADDR_18               0x00000001
#define _EMC_P4_19_ADDR_19               0x00000001
#define _EMC_P4_20_ADDR_20               0x00000001
#define _EMC_P4_21_ADDR_21               0x00000001
#define _EMC_P4_22_ADDR_22               0x00000001
#define _EMC_P4_23_ADDR_23               0x00000001
#define _EMC_P4_24_OE                    0x00000001
#define _EMC_P4_25_WE                    0x00000001
#define _EMC_P4_26_BLS0                  0x00000001
#define _EMC_P4_27_BLS1                  0x00000001
#define _EMC_P4_28_BLS2                  0x00000001
#define _EMC_P4_29_BLS3                  0x00000001
#define _EMC_P4_30_CS0                   0x00000001
#define _EMC_P4_31_CS1                   0x00000001

#define _LCD_P4_28_VD2                   0x00000007
#define _LCD_P4_29_VD3                   0x00000007

#define _UART2_P4_22_TXD                 0x00000002
#define _UART2_P4_23_RXD                 0x00000002
#define _UART3_P4_28_TXD                 0x00000002
#define _UART3_P4_29_RXD                 0x00000002

#define _I2C2_P4_20_SDA                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P4_21_SCL                 (0x00000002 | IOCON_MODE_OPEN_DRAIN)
#define _I2C2_P4_29_SCL                 (0x00000004 | IOCON_MODE_OPEN_DRAIN)

// Port 5 functions
//
#define _UART4_P5_3_RXD                  0x00000004
#define _UART4_P5_4_TXD                  0x00000004

#define _I2C0_P5_2_SDA                  (0x00000005 | IOCON_MODE_OPEN_DRAIN)
#define _I2C0_P5_3_SCL                  (0x00000005 | IOCON_MODE_OPEN_DRAIN)

#else
// Pin Connect Block
//
#define PINSEL0                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x00)        // Pin Function Select Register 0
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
  #define PINSEL0_P0_4_LCD               0x00000100
  #define PINSEL0_P0_4_RD2               0x00000200
  #define PINSEL0_P0_4_CAP2_0            0x00000300
  #define PINSEL0_P0_4_RESET             0x00000300
  #define PINSEL0_P0_5_GPIO              0x00000000
  #define PINSEL0_P0_5_I2SRX_WS          0x00000400
  #define PINSEL0_P0_5_LCD               0x00000400
  #define PINSEL0_P0_5_TD2               0x00000800
  #define PINSEL0_P0_5_CAP2_1            0x00000c00
  #define PINSEL0_P0_5_RESET             0x00000c00
  #define PINSEL0_P0_6_GPIO              0x00000000
  #define PINSEL0_P0_6_I2SRX_SDA         0x00001000
  #define PINSEL0_P0_6_LCD               0x00001000
  #define PINSEL0_P0_6_SSEL1             0x00002000
  #define PINSEL0_P0_6_MAT2_1            0x00003000
  #define PINSEL0_P0_6_RESET             0x00003000
  #define PINSEL0_P0_7_GPIO              0x00000000
  #define PINSEL0_P0_7_I2STX_CLK         0x00004000
  #define PINSEL0_P0_7_LCD               0x00004000
  #define PINSEL0_P0_7_SCK1              0x00008000
  #define PINSEL0_P0_7_MAT2_1            0x0000c000
  #define PINSEL0_P0_7_RESET             0x0000c000
  #define PINSEL0_P0_8_GPIO              0x00000000
  #define PINSEL0_P0_8_I2STX_WS          0x00010000
  #define PINSEL0_P0_8_LCD               0x00010000
  #define PINSEL0_P0_8_MISO1             0x00020000
  #define PINSEL0_P0_8_MAT2_2            0x00030000
  #define PINSEL0_P0_8_RESET             0x00030000
  #define PINSEL0_P0_9_GPIO              0x00000000
  #define PINSEL0_P0_9_I2STX_SDA         0x00040000
  #define PINSEL0_P0_9_LCD               0x00040000
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
  #define PINSEL0_P0_12_GPIO             0x00000000
  #define PINSEL0_P0_12_USB_PPWR2        0x01000000
  #define PINSEL0_P0_12_MISO1            0x02000000
  #define PINSEL0_P0_12_AD0_6            0x03000000
  #define PINSEL0_P0_12_RESET            0x03000000
  #define PINSEL0_P0_13_GPIO             0x00000000
  #define PINSEL0_P0_13_USB_UP_LED2      0x04000000
  #define PINSEL0_P0_13_MOSI1            0x08000000
  #define PINSEL0_P0_13_AD0_7            0x0c000000
  #define PINSEL0_P0_13_RESET            0x0c000000
  #define PINSEL0_P0_14_GPIO             0x00000000
  #define PINSEL0_P0_14_USB_HSTEN2       0x10000000
  #define PINSEL0_P0_14_USB_CONNECT2     0x20000000
  #define PINSEL0_P0_14_SSEL2            0x30000000
  #define PINSEL0_P0_14_RESET            0x30000000
  #define PINSEL0_P0_15_GPIO             0x00000000
  #define PINSEL0_P0_15_TXD1             0x40000000
  #define PINSEL0_P0_15_SCK0             0x80000000
  #define PINSEL0_P0_15_SCK              0xc0000000
  #define PINSEL0_P0_15_RESET            0xc0000000

#define PINSEL1                          *(unsigned long*)(PIN_CONNECT_BLOCK +0x04)        // Pin Function Select Register 1
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
  #define PINSEL1_P0_31_GPIO             0x00000000
  #define PINSEL1_P0_31_U2D_PLUS         0x40000000
  #define PINSEL1_P0_31_RESET            0xc0000000

#define PINSEL2                          *(unsigned long*)(PIN_CONNECT_BLOCK +0x08)        // Pin Function Select Register 2
  #define PINSEL2_P1_0_GPIO              0x00000000
  #define PINSEL2_P1_0_ENET_TXD0         0x00000001
  #define PINSEL2_P1_0_RESET             0x00000003
  #define PINSEL2_P1_1_GPIO              0x00000000
  #define PINSEL2_P1_1_ENET_TXD0         0x00000004
  #define PINSEL2_P1_1_RESET             0x0000000c
  #define PINSEL2_P1_4_GPIO              0x00000000
  #define PINSEL2_P1_4_ENET_TX_EN        0x00000100
  #define PINSEL2_P1_4_RESET             0x00000300
  #define PINSEL2_P1_5_GPIO              0x00000000
  #define PINSEL2_P1_6_GPIO              0x00000000
  #define PINSEL2_P1_6_ENET_TX_CLK       0x00001000
  #define PINSEL2_P1_8_GPIO              0x00000000
  #define PINSEL2_P1_8_ENET_CRS          0x00010000
  #define PINSEL2_P1_8_RESET             0x00030000
  #define PINSEL2_P1_9_GPIO              0x00000000
  #define PINSEL2_P1_9_ENET_RXD0         0x00040000
  #define PINSEL2_P1_9_RESET             0x000c0000
  #define PINSEL2_P1_10_GPIO             0x00000000
  #define PINSEL2_P1_10_ENET_RXD1        0x00100000
  #define PINSEL2_P1_10_RESET            0x00300000
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
  #define PINSEL3_P1_19_USB_TX_E1        0x00000040
  #define PINSEL3_P1_19_USB_PPWR1        0x00000080
  #define PINSEL3_P1_19_CAP1_1           0x000000c0
  #define PINSEL3_P1_19_RESET            0x000000c0
  #define PINSEL3_P1_20_GPIO             0x00000000
  #define PINSEL3_P1_20_USB_TX_DP1       0x00000100
  #define PINSEL3_P1_20_LCD              0x00000100
  #define PINSEL3_P1_20_PWM1_2           0x00000200
  #define PINSEL3_P1_20_SCK0             0x00000300
  #define PINSEL3_P1_20_RESET            0x00000300
  #define PINSEL3_P1_21_GPIO             0x00000000
  #define PINSEL3_P1_21_USB_TX_DM1       0x00000400
  #define PINSEL3_P1_21_LCD              0x00000400
  #define PINSEL3_P1_21_PWM1_3           0x00000800
  #define PINSEL3_P1_21_SSEL0            0x00000c00
  #define PINSEL3_P1_21_RESET            0x00000c00
  #define PINSEL3_P1_22_GPIO             0x00000000
  #define PINSEL3_P1_22_USB_RCV1         0x00001000
  #define PINSEL3_P1_22_LCD              0x00001000
  #define PINSEL3_P1_22_USB_PWRD1        0x00002000
  #define PINSEL3_P1_22_MAT1_0           0x00003000
  #define PINSEL3_P1_22_RESET            0x00003000
  #define PINSEL3_P1_23_GPIO             0x00000000
  #define PINSEL3_P1_23_USB_RX_DP1       0x00004000
  #define PINSEL3_P1_23_LCD              0x00004000
  #define PINSEL3_P1_23_PWM1_4           0x00008000
  #define PINSEL3_P1_23_MISO0            0x0000c000
  #define PINSEL3_P1_23_RESET            0x0000c000
  #define PINSEL3_P1_24_GPIO             0x00000000
  #define PINSEL3_P1_24_USB_RX_DM1       0x00010000
  #define PINSEL3_P1_24_LCD              0x00010000
  #define PINSEL3_P1_24_PWM1_5           0x00020000
  #define PINSEL3_P1_24_MOSI0            0x00030000
  #define PINSEL3_P1_24_RESET            0x00030000
  #define PINSEL3_P1_25_GPIO             0x00000000
  #define PINSEL3_P1_25_USB_LS1          0x00040000
  #define PINSEL3_P1_25_LCD              0x00040000
  #define PINSEL3_P1_25_USB_HSTEN1       0x00080000
  #define PINSEL3_P1_25_MAT1_1           0x000c0000
  #define PINSEL3_P1_25_RESET            0x000c0000
  #define PINSEL3_P1_26_GPIO             0x00000000
  #define PINSEL3_P1_26_USB_SSPND1       0x00100000
  #define PINSEL3_P1_26_LCD              0x00100000
  #define PINSEL3_P1_26_PWM1_6           0x00200000
  #define PINSEL3_P1_26_CAP0_0           0x00300000
  #define PINSEL3_P1_26_RESET            0x00300000
  #define PINSEL3_P1_27_GPIO             0x00000000
  #define PINSEL3_P1_27_CLKOUT           0x00400000
  #define PINSEL3_P1_27_USB_OVRCR1       0x00800000
  #define PINSEL3_P1_27_CAP0_1           0x00c00000
  #define PINSEL3_P1_27_RESET            0x00c00000
  #define PINSEL3_P1_28_GPIO             0x00000000
  #define PINSEL3_P1_28_USB_SCL1         0x01000000
  #define PINSEL3_P1_28_LCD              0x01000000
  #define PINSEL3_P1_28_PCAP1_0          0x02000000
  #define PINSEL3_P1_28_MAT0_0           0x03000000
  #define PINSEL3_P1_28_RESET            0x03000000
  #define PINSEL3_P1_29_GPIO             0x00000000
  #define PINSEL3_P1_29_USB_SDA1         0x04000000
  #define PINSEL3_P1_29_LCD              0x04000000
  #define PINSEL3_P1_29_PCAP1_1          0x08000000
  #define PINSEL3_P1_29_MAT0_1           0x0c000000
  #define PINSEL3_P1_29_RESET            0x0c000000
  #define PINSEL3_P1_30_GPIO             0x00000000
  #define PINSEL3_P1_30_USB_PWRD2        0x10000000
  #define PINSEL3_P1_30_VBUS             0x20000000
  #define PINSEL3_P1_30_AD0_4            0x30000000
  #define PINSEL3_P1_30_RESET            0x30000000
  #define PINSEL3_P1_31_GPIO             0x00000000
  #define PINSEL3_P1_31_USB_OVRCR2       0x40000000
  #define PINSEL3_P1_31_SCK1             0x80000000
  #define PINSEL3_P1_31_AD0_5            0xc0000000
  #define PINSEL3_P1_31_RESET            0xc0000000

#define PINSEL4                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x10)        // Pin Function Select Register 4
  #define PINSEL4_P2_0_GPIO              0x00000000
  #define PINSEL4_P2_0_PWM1_1            0x00000001
  #define PINSEL4_P2_0_TXD1              0x00000002
  #define PINSEL4_P2_0_TRACECLK          0x00000003
  #define PINSEL4_P2_0_LCD               0x00000003
  #define PINSEL4_P2_0_RESET             0x00000003
  #define PINSEL4_P2_1_GPIO              0x00000000
  #define PINSEL4_P2_1_PWM1_2            0x00000004
  #define PINSEL4_P2_1_RXD1              0x00000008
  #define PINSEL4_P2_1_PIPESTAT0         0x0000000c
  #define PINSEL4_P2_1_LCD               0x0000000c
  #define PINSEL4_P2_1_RESET             0x0000000c
  #define PINSEL4_P2_2_GPIO              0x00000000
  #define PINSEL4_P2_2_PWM1_3            0x00000010
  #define PINSEL4_P2_2_CTS1              0x00000020
  #define PINSEL4_P2_2_PIPESTAT1         0x00000030
  #define PINSEL4_P2_2_LCD               0x00000030
  #define PINSEL4_P2_2_RESET             0x00000030
  #define PINSEL4_P2_3_GPIO              0x00000000
  #define PINSEL4_P2_3_PWM1_4            0x00000040
  #define PINSEL4_P2_3_DCD1              0x00000080
  #define PINSEL4_P2_3_PIPESTAT2         0x000000c0
  #define PINSEL4_P2_3_LCD               0x000000c0
  #define PINSEL4_P2_3_RESET             0x000000c0
  #define PINSEL4_P2_4_GPIO              0x00000000
  #define PINSEL4_P2_4_PWM1_5            0x00000100
  #define PINSEL4_P2_4_DSR1              0x00000200
  #define PINSEL4_P2_4_TRACESYN          0x00000300
  #define PINSEL4_P2_4_LCD               0x00000300
  #define PINSEL4_P2_4_RESET             0x00000300
  #define PINSEL4_P2_5_GPIO              0x00000000
  #define PINSEL4_P2_5_PWM1_6            0x00000400
  #define PINSEL4_P2_5_DTR1              0x00000800
  #define PINSEL4_P2_5_TRACEPKT0         0x00000c00
  #define PINSEL4_P2_5_LCD               0x00000c00
  #define PINSEL4_P2_5_RESET             0x00000c00
  #define PINSEL4_P2_6_GPIO              0x00000000
  #define PINSEL4_P2_6_PCAP1_0           0x00001000
  #define PINSEL4_P2_6_RI1               0x00002000
  #define PINSEL4_P2_6_TRACEPKT1         0x00003000
  #define PINSEL4_P2_6_LCD               0x00003000
  #define PINSEL4_P2_6_RESET             0x00003000
  #define PINSEL4_P2_7_GPIO              0x00000000
  #define PINSEL4_P2_7_RD2               0x00004000
  #define PINSEL4_P2_7_RTS1              0x00008000
  #define PINSEL4_P2_7_TRACEPKT2         0x0000c000
  #define PINSEL4_P2_7_LCD               0x0000c000
  #define PINSEL4_P2_7_RESET             0x0000c000
  #define PINSEL4_P2_8_GPIO              0x00000000
  #define PINSEL4_P2_8_TD2               0x00010000
  #define PINSEL4_P2_8_TXD2              0x00020000
  #define PINSEL4_P2_8_TRACEPKT3         0x00030000
  #define PINSEL4_P2_8_LCD               0x00030000
  #define PINSEL4_P2_8_RESET             0x00030000
  #define PINSEL4_P2_9_GPIO              0x00000000
  #define PINSEL4_P2_9_USB_CONNECT1      0x00040000
  #define PINSEL4_P2_9_RXD2              0x00080000
  #define PINSEL4_P2_9_EXTIN0            0x000c0000
  #define PINSEL4_P2_9_LCD               0x000c0000
  #define PINSEL4_P2_9_RESET             0x000c0000
  #define PINSEL4_P2_10_GPIO             0x00000000
  #define PINSEL4_P2_10_EINT0            0x00100000
  #define PINSEL4_P2_10_RESET            0x00300000
  #define PINSEL4_P2_11_GPIO             0x00000000
  #define PINSEL4_P2_11_EINT1            0x00400000
  #define PINSEL4_P2_11_LCD              0x00400000
  #define PINSEL4_P2_11_MCIDAT1          0x00800000
  #define PINSEL4_P2_11_I2STX_CLK        0x00c00000
  #define PINSEL4_P2_11_RESET            0x00c00000
  #define PINSEL4_P2_12_GPIO             0x00000000
  #define PINSEL4_P2_12_EINT2            0x01000000
  #define PINSEL4_P2_12_LCD              0x01000000
  #define PINSEL4_P2_12_MCIDAT2          0x02000000
  #define PINSEL4_P2_12_I2STX_WS         0x03000000
  #define PINSEL4_P2_12_RESET            0x03000000
  #define PINSEL4_P2_13_GPIO             0x00000000
  #define PINSEL4_P2_13_EINT3            0x04000000
  #define PINSEL4_P2_13_LCD              0x04000000
  #define PINSEL4_P2_13_MCIDAT3          0x08000000
  #define PINSEL4_P2_13_I2STX_SDA        0x0c000000
  #define PINSEL4_P2_13_RESET            0x0c000000
  #define PINSEL4_P2_14_GPIO             0x00000000
  #define PINSEL4_P2_14_RESET            0x30000000
  #define PINSEL4_P2_15_GPIO             0x00000000
  #define PINSEL4_P2_15_RESET            0xc0000000

#define PINSEL5                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x14)        // Pin Function Select Register 5
  #define PINSEL5_P2_16_GPIO             0x00000000
  #define PINSEL5_P2_16_CAS              0x00000001
  #define PINSEL5_P2_16_RESET            0x00000003
  #define PINSEL5_P2_17_GPIO             0x00000000
  #define PINSEL5_P2_17_RAS              0x00000004
  #define PINSEL5_P2_17_RESET            0x0000000c
  #define PINSEL5_P2_18_GPIO             0x00000000
  #define PINSEL5_P2_18_CLKOUT0          0x00000010
  #define PINSEL5_P2_18_RESET            0x00000030
  #define PINSEL5_P2_19_GPIO             0x00000000
  #define PINSEL5_P2_19_CLKOUT1          0x00000040
  #define PINSEL5_P2_19_RESET            0x000000c0
  #define PINSEL5_P2_20_GPIO             0x00000000
  #define PINSEL5_P2_20_DYCS0            0x00000100
  #define PINSEL5_P2_20_RESET            0x00000300
  #define PINSEL5_P2_21_GPIO             0x00000000
  #define PINSEL5_P2_21_DYCS1            0x00000400
  #define PINSEL5_P2_21_RESET            0x00000c00
  #define PINSEL5_P2_22_GPIO             0x00000000
  #define PINSEL5_P2_22_DYCS2            0x00001000
  #define PINSEL5_P2_22_CAP3_0           0x00002000
  #define PINSEL5_P2_22_SCK0             0x00003000
  #define PINSEL5_P2_22_RESET            0x00003000
  #define PINSEL5_P2_23_GPIO             0x00000000
  #define PINSEL5_P2_23_DYCS3            0x00004000
  #define PINSEL5_P2_23_CAP3_1           0x00008000
  #define PINSEL5_P2_23_SSEL0            0x0000c000
  #define PINSEL5_P2_23_RESET            0x0000c000
  #define PINSEL5_P2_24_GPIO             0x00000000
  #define PINSEL5_P2_24_CKEOUT1          0x00010000
  #define PINSEL5_P2_24_RESET            0x00030000
  #define PINSEL5_P2_25_GPIO             0x00000000
  #define PINSEL5_P2_25_CKEOUT1          0x00040000
  #define PINSEL5_P2_25_RESET            0x000c0000
  #define PINSEL5_P2_26_GPIO             0x00000000
  #define PINSEL5_P2_26_CKEOUT2          0x00100000
  #define PINSEL5_P2_26_MAT3_0           0x00200000
  #define PINSEL5_P2_26_MISO0            0x00300000
  #define PINSEL5_P2_26_RESET            0x00300000
  #define PINSEL5_P2_27_GPIO             0x00000000
  #define PINSEL5_P2_27_CKEOUT3          0x00400000
  #define PINSEL5_P2_27_MAT3_1           0x00800000
  #define PINSEL5_P2_27_MOSI0            0x00c00000
  #define PINSEL5_P2_27_RESET            0x00c00000
  #define PINSEL5_P2_28_GPIO             0x00000000
  #define PINSEL5_P2_28_DQMOUT0          0x01000000
  #define PINSEL5_P2_28_RESET            0x03000000
  #define PINSEL5_P2_29_GPIO             0x00000000
  #define PINSEL5_P2_29_DQMOUT1          0x04000000
  #define PINSEL5_P2_29_RESET            0x0c000000
  #define PINSEL5_P2_30_GPIO             0x00000000
  #define PINSEL5_P2_30_DQMOUT2          0x10000000
  #define PINSEL5_P2_30_MAT3_2           0x20000000
  #define PINSEL5_P2_30_SDA2             0x30000000
  #define PINSEL5_P2_30_RESET            0x30000000
  #define PINSEL5_P2_31_GPIO             0x00000000
  #define PINSEL5_P2_31_DQMOUT3          0x40000000
  #define PINSEL5_P2_31_MAT3_3           0x80000000
  #define PINSEL5_P2_31_SCL2             0xc0000000
  #define PINSEL5_P2_31_RESET            0xc0000000

#define PINSEL6                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x18)        // Pin Function Select Register 6
  #define PINSEL6_P3_0_GPIO              0x00000000
  #define PINSEL6_P3_0_D0                0x00000001
  #define PINSEL6_P3_0_RESET             0x00000003
  #define PINSEL6_P3_1_GPIO              0x00000000
  #define PINSEL6_P3_1_D1                0x00000004
  #define PINSEL6_P3_1_RESET             0x0000000c
  #define PINSEL6_P3_2_GPIO              0x00000000
  #define PINSEL6_P3_2_D2                0x00000010
  #define PINSEL6_P3_2_RESET             0x00000030
  #define PINSEL6_P3_3_GPIO              0x00000000
  #define PINSEL6_P3_3_D3                0x00000040
  #define PINSEL6_P3_3_RESET             0x000000c0
  #define PINSEL6_P3_4_GPIO              0x00000000
  #define PINSEL6_P3_4_D4                0x00000100
  #define PINSEL6_P3_4_RESET             0x00000300
  #define PINSEL6_P3_5_GPIO              0x00000000
  #define PINSEL6_P3_5_D5                0x00000400
  #define PINSEL6_P3_5_RESET             0x00000c00
  #define PINSEL6_P3_6_GPIO              0x00000000
  #define PINSEL6_P3_6_D6                0x00001000
  #define PINSEL6_P3_6_RESET             0x00003000
  #define PINSEL6_P3_7_GPIO              0x00000000
  #define PINSEL6_P3_7_D7                0x00004000
  #define PINSEL6_P3_7_RESET             0x0000c000
  #define PINSEL6_P3_8_GPIO              0x00000000
  #define PINSEL6_P3_8_D8                0x00010000
  #define PINSEL6_P3_8_RESET             0x00030000
  #define PINSEL6_P3_9_GPIO              0x00000000
  #define PINSEL6_P3_9_D9                0x00040000
  #define PINSEL6_P3_9_RESET             0x000c0000
  #define PINSEL6_P3_10_GPIO             0x00000000
  #define PINSEL6_P3_10_D10              0x00100000
  #define PINSEL6_P3_10_RESET            0x00300000
  #define PINSEL6_P3_11_GPIO             0x00000000
  #define PINSEL6_P3_11_D11              0x00400000
  #define PINSEL6_P3_11_RESET            0x00c00000
  #define PINSEL6_P3_12_GPIO             0x00000000
  #define PINSEL6_P3_12_D12              0x01000000
  #define PINSEL6_P3_12_RESET            0x03000000
  #define PINSEL6_P3_13_GPIO             0x00000000
  #define PINSEL6_P3_13_D13              0x04000000
  #define PINSEL6_P3_13_RESET            0x0c000000
  #define PINSEL6_P3_14_GPIO             0x00000000
  #define PINSEL6_P3_14_D14              0x10000000
  #define PINSEL6_P3_14_RESET            0x30000000
  #define PINSEL6_P3_15_GPIO             0x00000000
  #define PINSEL6_P3_15_D15              0x40000000
  #define PINSEL6_P3_15_RESET            0xc0000000

#define PINSEL7                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x1c)        // Pin Function Select Register 7
  #define PINSEL7_P3_23_GPIO             0x00000000
  #define PINSEL7_P3_23_CAP0_0           0x00008000
  #define PINSEL7_P3_23_PCAP1_0          0x0000c000
  #define PINSEL7_P3_23_RESET            0x0000c000
  #define PINSEL7_P3_24_GPIO             0x00000000
  #define PINSEL7_P3_24_CAP0_1           0x00020000
  #define PINSEL7_P3_24_PCAP1_1          0x00030000
  #define PINSEL7_P3_24_RESET            0x00030000
  #define PINSEL7_P3_25_GPIO             0x00000000
  #define PINSEL7_P3_25_MAT0_1           0x00080000
  #define PINSEL7_P3_25_PWM1_2           0x000c0000
  #define PINSEL7_P3_25_RESET            0x000c0000
  #define PINSEL7_P3_26_GPIO             0x00000000
  #define PINSEL7_P3_26_MAT0_1           0x00200000
  #define PINSEL7_P3_26_PWM1_3           0x00300000
  #define PINSEL7_P3_26_RESET            0x00300000

#define PINSEL8                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x20)        // Pin Function Select Register 8
  #define PINSEL8_P4_0_GPIO              0x00000000
  #define PINSEL8_P4_0_A0                0x00000001
  #define PINSEL8_P4_0_RESET             0x00000003
  #define PINSEL8_P4_1_GPIO              0x00000000
  #define PINSEL8_P4_1_A1                0x00000004
  #define PINSEL8_P4_1_RESET             0x0000000c
  #define PINSEL8_P4_2_GPIO              0x00000000
  #define PINSEL8_P4_2_A2                0x00000010
  #define PINSEL8_P4_2_RESET             0x00000030
  #define PINSEL8_P4_3_GPIO              0x00000000
  #define PINSEL8_P4_3_A3                0x00000040
  #define PINSEL8_P4_3_RESET             0x000000c0
  #define PINSEL8_P4_4_GPIO              0x00000000
  #define PINSEL8_P4_4_A4                0x00000100
  #define PINSEL8_P4_4_RESET             0x00000300
  #define PINSEL8_P4_5_GPIO              0x00000000
  #define PINSEL8_P4_5_A5                0x00000400
  #define PINSEL8_P4_5_RESET             0x00000c00
  #define PINSEL8_P4_6_GPIO              0x00000000
  #define PINSEL8_P4_6_A6                0x00001000
  #define PINSEL8_P4_6_RESET             0x00003000
  #define PINSEL8_P4_7_GPIO              0x00000000
  #define PINSEL8_P4_7_A7                0x00004000
  #define PINSEL8_P4_7_RESET             0x0000c000
  #define PINSEL8_P4_8_GPIO              0x00000000
  #define PINSEL8_P4_8_A8                0x00010000
  #define PINSEL8_P4_8_RESET             0x00030000
  #define PINSEL8_P4_9_GPIO              0x00000000
  #define PINSEL8_P4_9_A9                0x00040000
  #define PINSEL8_P4_9_RESET             0x000c0000
  #define PINSEL8_P4_10_GPIO             0x00000000
  #define PINSEL8_P4_10_A10              0x00100000
  #define PINSEL8_P4_10_RESET            0x00300000
  #define PINSEL8_P4_11_GPIO             0x00000000
  #define PINSEL8_P4_11_A11              0x00400000
  #define PINSEL8_P4_11_RESET            0x00c00000
  #define PINSEL8_P4_12_GPIO             0x00000000
  #define PINSEL8_P4_12_A12              0x01000000
  #define PINSEL8_P4_12_RESET            0x03000000
  #define PINSEL8_P4_13_GPIO             0x00000000
  #define PINSEL8_P4_13_A13              0x04000000
  #define PINSEL8_P4_13_RESET            0x0c000000
  #define PINSEL8_P4_14_GPIO             0x00000000
  #define PINSEL8_P4_14_A14              0x10000000
  #define PINSEL8_P4_14_RESET            0x30000000
  #define PINSEL8_P4_15_GPIO             0x00000000
  #define PINSEL8_P4_15_A15              0x40000000
  #define PINSEL8_P4_15_RESET            0xc0000000

#define PINSEL9                          *(unsigned long*)(PIN_CONNECT_BLOCK + 0x24)        // Pin Function Select Register 9
  #define PINSEL9_P4_16_GPIO             0x00000000
  #define PINSEL9_P4_16_A16              0x00000001
  #define PINSEL9_P4_16_RESET            0x00000003
  #define PINSEL9_P4_17_GPIO             0x00000000
  #define PINSEL9_P4_17_A17              0x00000004
  #define PINSEL9_P4_17_RESET            0x0000000c
  #define PINSEL9_P4_18_GPIO             0x00000000
  #define PINSEL9_P4_18_A18              0x00000010
  #define PINSEL9_P4_18_RESET            0x00000030
  #define PINSEL9_P4_19_GPIO             0x00000000
  #define PINSEL9_P4_19_A19              0x00000040
  #define PINSEL9_P4_19_RESET            0x000000c0
  #define PINSEL9_P4_20_GPIO             0x00000000
  #define PINSEL9_P4_20_A20              0x00000100
  #define PINSEL9_P4_20_SDA2             0x00000200
  #define PINSEL9_P4_20_SCK1             0x00000300
  #define PINSEL9_P4_20_RESET            0x00000300
  #define PINSEL9_P4_21_GPIO             0x00000000
  #define PINSEL9_P4_21_A21              0x00000400
  #define PINSEL9_P4_21_SCL2             0x00000800
  #define PINSEL9_P4_21_SSEL1            0x00000c00
  #define PINSEL9_P4_21_RESET            0x00000c00
  #define PINSEL9_P4_22_GPIO             0x00000000
  #define PINSEL9_P4_22_A22              0x00001000
  #define PINSEL9_P4_22_TXD2             0x00002000
  #define PINSEL9_P4_22_MISO1            0x00003000
  #define PINSEL9_P4_22_RESET            0x00003000
  #define PINSEL9_P4_23_GPIO             0x00000000
  #define PINSEL9_P4_23_A23              0x00004000
  #define PINSEL9_P4_23_RXD2             0x00008000
  #define PINSEL9_P4_23_MOSO1            0x0000c000
  #define PINSEL9_P4_23_RESET            0x0000c000
  #define PINSEL9_P4_24_GPIO             0x00000000
  #define PINSEL9_P4_24_A24              0x00010000
  #define PINSEL9_P4_24_OE               0x00020000
  #define PINSEL9_P4_24_RESET            0x00030000
  #define PINSEL9_P4_25_GPIO             0x00000000
  #define PINSEL9_P4_25_A25              0x00040000
  #define PINSEL9_P4_25_WE               0x00080000
  #define PINSEL9_P4_25_RESET            0x000c0000
  #define PINSEL9_P4_26_GPIO             0x00000000
  #define PINSEL9_P4_26_BLS0             0x00100000
  #define PINSEL9_P4_26_RESET            0x00300000
  #define PINSEL9_P4_27_GPIO             0x00000000
  #define PINSEL9_P4_27_BLS1             0x00400000
  #define PINSEL9_P4_27_RESET            0x00c00000
  #define PINSEL9_P4_28_GPIO             0x00000000
  #define PINSEL9_P4_28_BSL2             0x01000000
  #define PINSEL9_P4_28_MAT2_0           0x02000000
  #define PINSEL9_P4_28_LCD              0x02000000
  #define PINSEL9_P4_28_TXD3             0x03000000
  #define PINSEL9_P4_28_RESET            0x03000000
  #define PINSEL9_P4_29_GPIO             0x00000000
  #define PINSEL9_P4_29_BSL3             0x04000000
  #define PINSEL9_P4_29_MAT2_1           0x08000000
  #define PINSEL9_P4_29_LCD              0x08000000
  #define PINSEL9_P4_29_RXD3             0x0c000000
  #define PINSEL9_P4_29_RESET            0x0c000000
  #define PINSEL9_P4_30_GPIO             0x00000000
  #define PINSEL9_P4_30_CS0              0x10000000
  #define PINSEL9_P4_30_RESET            0x30000000
  #define PINSEL9_P4_31_GPIO             0x00000000
  #define PINSEL9_P4_31_CS1              0x40000000
  #define PINSEL9_P4_31_RESET            0xc0000000

#define PINSEL10                         *(unsigned long*)(PIN_CONNECT_BLOCK + 0x28)        // Pin Function Select Register 10
  #define GPIO_TRACE                     0x00000008                      // set if RTCK is '0' at reset - ETM signals are then available regardless of PINSEL4 content

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
#define PINMODE_OD0_ADD                  (unsigned long*)(PIN_CONNECT_BLOCK + 0x68)
#define PINMODE_OD0                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x68)        // Open-drain Mode Control Register 0
#define PINMODE_OD1                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x6c)        // Open-drain Mode Control Register 1
#define PINMODE_OD2                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x70)        // Open-drain Mode Control Register 2
#define PINMODE_OD3                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x74)        // Open-drain Mode Control Register 3
#define PINMODE_OD4                      *(unsigned long*)(PIN_CONNECT_BLOCK + 0x78)        // Open-drain Mode Control Register 4
#define I2CPADCFG                        *(unsigned long*)(PIN_CONNECT_BLOCK + 0x7c)        // I2C Pin Configuration Register
#endif


#if defined LPC1788                                                      // {7}
    // SD card interface
    //
    #define MCIPower                         *(unsigned long*)(MCI_BLOCK + 0x00)            // Power Control Register
      #define MCI_PWR_Ctrl_PowerOff          0x00000000
      #define MCI_PWR_Ctrl_PowerUp           0x00000002
      #define MCI_PWR_Ctrl_PowerOn           0x00000003
      #define MCI_PWR_OpenDrain              0x00000040
      #define MCI_PWR_Rod                    0x00000080
    #define MCIClock                         *(unsigned long*)(MCI_BLOCK + 0x04)            // Clock Control Register
      #define MCI_CLK_ClkDiv                 0x000000ff                                     // MCLK divide mask MCLCLK = MCLK / [2 * (MCI_CLK_ClkDiv + 1)]
      #define MCI_CLK_Enable                 0x00000100                                     // enable MCI bus clock
      #define MCI_CLK_PwrSave                0x00000200                                     // disable MCI clock output when bus is idle
      #define MCI_CLK_Bypass                 0x00000400                                     // MCLK driven to card bus output
      #define MCI_CLK_WideBus                0x00000800                                     // widebus mode (D0..D3)
    #define MCIArgument                      *(unsigned long*)(MCI_BLOCK + 0x08)            // Argument Register
    #define MCICommand                       *(unsigned long*)(MCI_BLOCK + 0x0c)            // Command Register
      #define MCICmd_CmdIndex                0x0000003f                                     // command index mask
      #define MCICmd_Response                0x00000040                                     // wait for a response to the command
      #define MCICmd_LongRsp                 0x00000080                                     // 136 bit long response
      #define MCICmd_Interrupt               0x00000100                                     // disable command timer and wait for interrupt request
      #define MCICmd_Pending                 0x00000200                                     // wait for command pending before sending the command
      #define MCICmd_Enable                  0x00000400                                     // enable Command Path State Machine
    #define MCIRespCmd                       *(volatile unsigned long*)(MCI_BLOCK + 0x10)   // Response Command Register (read-only)
    #define MCIResponse0                     *(volatile unsigned long*)(MCI_BLOCK + 0x14)   // Response Register 0 (read-only)
    #define MCIResponse1                     *(volatile unsigned long*)(MCI_BLOCK + 0x18)   // Response Register 1 (read-only)
    #define MCIResponse2                     *(volatile unsigned long*)(MCI_BLOCK + 0x1c)   // Response Register 2 (read-only)
    #define MCIResponse3                     *(volatile unsigned long*)(MCI_BLOCK + 0x20)   // Response Register 3 (read-only)
    #define MCIDataTimer                     *(unsigned long*)(MCI_BLOCK + 0x24)            // Data Timer
    #define MCIDataLength                    *(volatile unsigned long*)(MCI_BLOCK + 0x28)   // Data Length
    #define MCIDataCtrl                      *(unsigned long*)(MCI_BLOCK + 0x2c)            // Data Control Register
      #define MCIDataCnt_Enable              0x00000001                                     // data transfer enable
      #define MCIDataCnt_Direction_to_card   0x00000000
      #define MCIDataCnt_Direction_from_card 0x00000002
      #define MCIDataCnt_Mode_Block          0x00000000
      #define MCIDataCnt_Mode_Stream         0x00000004
      #define MCIDataCnt_DMAEnable           0x00000008
      #define MCIDataCnt_block_size_512      0x00000090
    #define MCIDataCnt                       *(unsigned long*)(MCI_BLOCK + 0x30)            // Data Counter
    #define MCIStatus                        *(volatile unsigned long*)(MCI_BLOCK + 0x34)   // Status Register (read-only)
      #define CmdCrcFail                     0x00000001                                     // static status bits - these need clearing once set
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
      #define CmdActive                      0x00000800                                     // command transfer in progress
      #define TxActive                       0x00001000                                     // data transmit in progress
      #define RxActive                       0x00002000                                     // data receive in progress
      #define TxFifoHalfEmpty                0x00004000
      #define RxFifoHalfFull                 0x00008000
      #define TxFifoFull                     0x00010000
      #define RxFifoFull                     0x00020000
      #define TxFifoEmpty                    0x00040000
      #define RxFifoEmpty                    0x00080000
      #define TxDataAvailbl                  0x00100000
      #define RxDataAvailbl                  0x00200000
    #define MCIClear                         *(volatile unsigned long*)(MCI_BLOCK + 0x38)   // Clear Register (write-only)
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
    #define MCIMask0                         *(unsigned long*)(MCI_BLOCK + 0x3c)            // Interrupt 0 Mask Register
    #define MCIFifoCnt                       *(volatile unsigned long*)(MCI_BLOCK + 0x48)   // FIFO Counter (read-only)
    #define MCIFIFO0_ADD                     (volatile unsigned long*)(MCI_BLOCK + 0x80)
    #define MCIFIFO0                         *(volatile unsigned long*)(MCI_BLOCK + 0x80)   // Data FIFO Registers
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
#endif


// System Control Module
//
#define FLASHCFG                         *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x00)    // Memory Acceleration Module Control Register
  #define FLASHCFG_FIXED                 0x0000003a                                         // this fixed configuration should never be changed
  #define FLASHTIM_1_CLOCKS              0x00000000                                         // for use up to 20MHz
  #define FLASHTIM_2_CLOCKS              0x00001000                                         // for use up to 40MHz
  #define FLASHTIM_3_CLOCKS              0x00002000                                         // for use up to 60MHz
  #define FLASHTIM_4_CLOCKS              0x00003000                                         // for use up to 80MHz
  #define FLASHTIM_5_CLOCKS              0x00004000                                         // for use up to 100MHz
  #define FLASHTIM_6_CLOCKS              0x00005000                                         // always safe
#define PLL0CON                          *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x80) // PLL Control Register
  #define PLLE                           0x00000001                                         // enable PLL
  #ifndef LPC1788
    #define PLLC                         0x00000002                                         // connect PLL
  #endif
#define PLL0CFG                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x84)    // main PLL Configuration Register
#define PLL0STAT                         *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x88) // main PLL Status Register (Read Only)
  #ifdef LPC1788
    #define PLLENABLED                   0x00000100                                         // status of enable bit
    #define PLLLOCKED                    0x00000400                                         // PLL lock status status
  #else
    #define PLLENABLED                   0x01000000                                         // status of enable bit
    #define PLLCONNECTED                 0x02000000                                         // status of connected bit
    #define PLLLOCKED                    0x04000000                                         // PLL lock status status
  #endif
#define PLL0FEED                         *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x8c) // main PLL Feed Register (Write Only)
#define PLL1CON                          *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0xa0) // USB PLL Control Register
#define PLL1CFG                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0xa4)    // USB PLL Configuration Register
#define PLL1STAT                         *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0xa8) // USB PLL Status Register (Read Only)
  #define PLL1ENABLED                    0x00000100                                         // status of enable bit
  #define PLL1CONNECTED                  0x00000200                                         // status of connected bit
  #define PLL1LOCKED                     0x00000400                                         // PLL lock status status
#define PLL1FEED                         *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0xac) // USB PLL Feed Register (Write Only)
#define PCON                             *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0xc0)    // Power Control Register
  #define PCON_IDL                       0x00000001                                         // Idle mode
  #define PCON_PD                        0x00000002                                         // Power down mode
  #define PCON_BODPDM
  #define PCON_BOGD
  #define PCON_BORD
  #define PCON_PM2
#define PCONP                            *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0xc4)    // Power Control for Peripherals Register
  #define PCLCD                          0x00000001                      // lpc1788
  #define PCTIM0                         0x00000002
  #define PCTIM1                         0x00000004
  #define PCUART0                        0x00000008
  #define PCUART1                        0x00000010
  #define PCPWM0                         0x00000020                      // lpc1788
  #define PCPWM1                         0x00000040
  #define PCI2C0                         0x00000080
#ifdef LPC1788
  #define PCUART4                        0x00000100                      // lpc1788
#else
  #define PCSPI                          0x00000100
#endif
  #define PCRTC                          0x00000200
  #define PCSSP1                         0x00000400
  #define PCEMC                          0x00000800                      // lpc1788
  #define PCPADC                         0x00001000
  #define PCPCAN1                        0x00002000
  #define PCPCAN2                        0x00004000
  #define PCGPIO                         0x00008000
#ifndef LPC1788
  #define PCRIT                          0x00010000
#endif
  #define PCMCPWM                        0x00020000
  #define PCQEI                          0x00040000
  #define PCI2C1                         0x00080000
  #define PCSSP2                         0x00100000                      // lpc1788
  #define PCSSP0                         0x00200000
  #define PCTIM2                         0x00400000
  #define PCTIM3                         0x00800000
  #define PCUART2                        0x01000000
  #define PCUART3                        0x02000000
  #define PCI2C2                         0x04000000
  #define PCI2S                          0x08000000
  #define PCSDC                          0x10000000                      // lpc1788
  #define PCGPDMA                        0x20000000
  #define PCENET                         0x40000000
  #define PCUSB                          0x80000000


#define POWER_UP(module)                 PCONP |= (module);              // power up a module (apply clock to it)
#define POWER_DOWN(module)               PCONP &= ~(module);             // power down a module (disable clock to it)

#ifdef LPC1788
    #define EMCCLKSEL                    *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x100)    // EMC Clock Selection Register
      #define EMCDIV                     0x00000001                                          // EMC clocked at half the rate fo the CPU
#endif
#define CCLKCFG                          *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x104)    // CPU Clock Configuration Register
  #ifdef LPC1788
    #define CCLKDIV_MASK                 0x0000001f
    #define CCLKSEL                      0x00000100
  #endif
#define USBCLKCFG                        *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x108)    // USB Clock Configuration Register
  #define USBCLKCFG_USBDIV_0             0x00000000                                          // no clock provided to USB subsystem
  #define USBCLKCFG_USBDIV_4             0x00000004                                          // PLL0 is divided by 4 - PLL0 output must be 192MHz
  #define USBCLKCFG_USBDIV_6             0x00000006                                          // PLL0 is divided by 6 - PLL0 output must be 288MHz
  #define USBCLKCFG_USBSEL_SYSCLK        0x00000000                                          // sysclk used as input to USB clock divided; USB can be accessed by software but can not perform USB functions
  #define USBCLKCFG_USBSEL_MAIN_PLL      0x00000100                                          // main PLL is used as USB clock divider input
  #define USBCLKCFG_USBSEL_ALT_PLL       0x00000200                                          // alternative PLL is used as USB clock divider input
#define CLKSRCSEL                        *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x10c) // Clock Source Select Register (volatile to ensure that the write order is not changed in the initialisation)
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
#define RSID                             *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x180)    // Reset Source Identification Register
  #define POR                            0x00000001                                          // Power On reset
  #define EXTR                           0x00000002                                          // External reset
  #define WDTR                           0x00000004                                          // Watchdog Timer reset
  #define BODR                           0x00000008                                          // Brownout reset
#ifdef LPC1788
    #define Matrix_Arb                   *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x188)    // Matrix Arbitration Register
#endif

#define SCS                              *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a0) // System Control and Status
  #ifdef LPC1788
    #define EMC_ShiftControl             0x00000001
    #define EMC_ResetDisable             0x00000002
    #define EMC_BurstControl             0x00000004
    #define MCIPWR_ActiveLevel           0x00000008
  #endif
  #define OSCRANGE                       0x00000010                                          // Main oscillator range set
  #define OSCEN                          0x00000020                                          // Enable external oscillator
  #define OSCSTAT                        0x00000040                                          // main oscillator status - ready for use (Read Only bit)
#ifndef LPC1788
    #define IRCTRIM                      *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a4)    // IRC Trim Register
    #define PCLKSEL0                     *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a8)    // Peripheral Clock Selection Register 0
      #define PCLK_WDT_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_WDT_CCLK_1            0x00000001                                          // PCLK to this peripheral is CCLK
      #define PCLK_WDT_CCLK_2            0x00000002                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_WDT_CCLK_8            0x00000003                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_TIMER0_CCLK_4         0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_TIMER0_CCLK_1         0x00000004                                          // PCLK to this peripheral is CCLK
      #define PCLK_TIMER0_CCLK_2         0x00000008                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_TIMER0_CCLK_8         0x0000000c                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_TIMER1_CCLK_4         0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_TIMER1_CCLK_1         0x00000010                                          // PCLK to this peripheral is CCLK
      #define PCLK_TIMER1_CCLK_2         0x00000020                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_TIMER1_CCLK_8         0x00000030                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_UART0_CCLK_4          0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_UART0_CCLK_1          0x00000040                                          // PCLK to this peripheral is CCLK
      #define PCLK_UART0_CCLK_2          0x00000080                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_UART0_CCLK_8          0x000000c0                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_UART1_CCLK_4          0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_UART1_CCLK_1          0x00000100                                          // PCLK to this peripheral is CCLK
      #define PCLK_UART1_CCLK_2          0x00000200                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_UART1_CCLK_8          0x00000300                                          // PCLK to this peripheral is CCLK/8

      #define PCLK_PWM1_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_PWM1_CCLK_1           0x00001000                                          // PCLK to this peripheral is CCLK
      #define PCLK_PWM1_CCLK_2           0x00002000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_PWM1_CCLK_8           0x00003000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_I2C0_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_I2C0_CCLK_1           0x00004000                                          // PCLK to this peripheral is CCLK
      #define PCLK_I2C0_CCLK_2           0x00008000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_I2C0_CCLK_8           0x0000c000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_SPI_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_SPI_CCLK_1            0x00010000                                          // PCLK to this peripheral is CCLK
      #define PCLK_SPI_CCLK_2            0x00020000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_SPI_CCLK_8            0x00030000                                          // PCLK to this peripheral is CCLK/8

      #define PCLK_SSP1_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_SSP1_CCLK_1           0x00100000                                          // PCLK to this peripheral is CCLK
      #define PCLK_SSP1_CCLK_2           0x00200000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_SSP1_CCLK_8           0x00300000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_DAC_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_DAC_CCLK_1            0x00400000                                          // PCLK to this peripheral is CCLK
      #define PCLK_DAC_CCLK_2            0x00800000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_DAC_CCLK_8            0x00c00000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_ADC_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_ADC_CCLK_1            0x01000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_ADC_CCLK_2            0x02000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_ADC_CCLK_8            0x03000000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_CAN1_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_CAN1_CCLK_1           0x04000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_CAN1_CCLK_2           0x08000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_CAN1_CCLK_6           0x0c000000                                          // PCLK to this peripheral is CCLK/6
      #define PCLK_CAN2_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_CAN2_CCLK_1           0x10000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_CAN2_CCLK_2           0x20000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_CAN2_CCLK_6           0x30000000                                          // PCLK to this peripheral is CCLK/6
      #define PCLK_ACF_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_ACF_CCLK_1            0x40000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_ACF_CCLK_2            0x80000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_ACF_CCLK_8            0xc0000000                                          // PCLK to this peripheral is CCLK/6
    #define PCLKSEL1                     *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1ac)    // Peripheral Clock Selection Register 1
      #define PCLK_QEI_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_QEI_CCLK_1            0x00000001                                          // PCLK to this peripheral is CCLK
      #define PCLK_QEI_CCLK_2            0x00000002                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_QEI_CCLK_8            0x00000003                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_GPIO_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_GPIO_CCLK_1           0x00000004                                          // PCLK to this peripheral is CCLK
      #define PCLK_GPIO_CCLK_2           0x00000008                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_GPIO_CCLK_8           0x0000000c                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_PCB_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_PCB_CCLK_1            0x00000010                                          // PCLK to this peripheral is CCLK
      #define PCLK_PCB_CCLK_2            0x00000020                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_PCB_CCLK_8            0x00000030                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_I2C1_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_I2C1_CCLK_1           0x00000040                                          // PCLK to this peripheral is CCLK
      #define PCLK_I2C1_CCLK_2           0x00000080                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_I2C1_CCLK_8           0x000000c0                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_SSP0_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_SSP0_CCLK_1           0x00000400                                          // PCLK to this peripheral is CCLK
      #define PCLK_SSP0_CCLK_2           0x00000800                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_SSP0_CCLK_8           0x00000c00                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_TIMER2_CCLK_4         0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_TIMER2_CCLK_1         0x00001000                                          // PCLK to this peripheral is CCLK
      #define PCLK_TIMER2_CCLK_2         0x00002000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_TIMER2_CCLK_8         0x00003000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_TIMER3_CCLK_4         0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_TIMER3_CCLK_1         0x00004000                                          // PCLK to this peripheral is CCLK
      #define PCLK_TIMER3_CCLK_2         0x00008000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_TIMER3_CCLK_8         0x0000c000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_UART2_CCLK_4          0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_UART2_CCLK_1          0x00010000                                          // PCLK to this peripheral is CCLK
      #define PCLK_UART2_CCLK_2          0x00020000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_UART2_CCLK_8          0x00030000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_UART3_CCLK_4          0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_UART3_CCLK_1          0x00040000                                          // PCLK to this peripheral is CCLK
      #define PCLK_UART3_CCLK_2          0x00080000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_UART3_CCLK_8          0x000c0000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_I2C2_CCLK_4           0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_I2C2_CCLK_1           0x00100000                                          // PCLK to this peripheral is CCLK
      #define PCLK_I2C2_CCLK_2           0x00200000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_I2C2_CCLK_8           0x00300000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_I2S_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_I2S_CCLK_1            0x00400000                                          // PCLK to this peripheral is CCLK
      #define PCLK_I2S_CCLK_2            0x00800000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_I2S_CCLK_8            0x00c00000                                          // PCLK to this peripheral is CCLK/8

      #define PCLK_RIT_CCLK_4            0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_RIT_CCLK_1            0x04000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_RIT_CCLK_2            0x08000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_RIT_CCLK_8            0x0c000000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_PCLK_SYSCON_CCLK_4    0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_PCLK_SYSCON_CCLK_1    0x10000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_PCLK_SYSCON_CCLK_2    0x20000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_PCLK_SYSCON_CCLK_8    0x30000000                                          // PCLK to this peripheral is CCLK/8
      #define PCLK_MC_CCLK_4             0x00000000                                          // PCLK to this peripheral is CCLK/4
      #define PCLK_MC_CCLK_1             0x40000000                                          // PCLK to this peripheral is CCLK
      #define PCLK_MC_CCLK_2             0x80000000                                          // PCLK to this peripheral is CCLK/2
      #define PCLK_MC_CCLK_8             0xc0000000                                          // PCLK to this peripheral is CCLK/8
#else
    #define PCLKSEL                      *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1a8)    // Peripheral Clock Selection Register
    #define PBOOST                       *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1b0)    // Power Boost Control Register
      #define BOOST                      0x00000003                                          // power boost enabled (default is on)

    #define LCD_CFG                      *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1b8)    // LCD Configuration and Clocking Control Register
#endif
#define USBIntSt                         *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1c0)    // USB Clock Control
#define CLKOUTCFG                        *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1c8)// Clock Output Configuration Register
  #define CLKOUT_IS_CPU_CLK              0x00000000
  #define CLKOUT_IS_MAIN_OSCILLATOR      0x00000001
  #define CLKOUT_IS_RC_OSCILLATOR        0x00000002
  #define CLKOUT_IS_USB_CLK              0x00000003
  #define CLKOUT_IS_RTC_CLK              0x00000004
  #ifdef LPC1788
    #define CLKOUT_IS_WDT_CLK            0x00000006
  #endif
  #define CLKOUT_DIV_1                   0x00000000
  #define CLKOUT_DIV_2                   0x00000010
  #define CLKOUT_DIV_3                   0x00000020
  #define CLKOUT_DIV_4                   0x00000030
  #define CLKOUT_DIV_5                   0x00000040
  #define CLKOUT_DIV_6                   0x00000050
  #define CLKOUT_DIV_7                   0x00000060
  #define CLKOUT_DIV_8                   0x00000070
  #define CLKOUT_DIV_9                   0x00000080
  #define CLKOUT_DIV_10                  0x00000090
  #define CLKOUT_DIV_11                  0x000000a0
  #define CLKOUT_DIV_12                  0x000000b0
  #define CLKOUT_DIV_13                  0x000000c0
  #define CLKOUT_DIV_14                  0x000000d0
  #define CLKOUT_DIV_15                  0x000000e0
  #define CLKOUT_DIV_16                  0x000000f0
  #define CLKOUT_ENABLE                  0x00000100
  #define CLKOUT_ACT                     0x00000200

#ifdef LPC1788
    #define EMCDLYCTL                    *(unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1dc)    // Delay Control Register
    #define EMCCAL                       *(volatile unsigned long*)(SYSTEM_CONTROL_MODULE + 0x1e0)    // Calibration Register
      #define EMCCAL_CALVALUE_MASK       0x000000ff
      #define EMCCAL_START               0x00004000
      #define EMCCAL_DONE                0x00008000
#endif

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
  #define SIMULATION_RESET               0x00004000
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
#define SUPP                             *(volatile unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x018) // PHY Support Register
  #define SUPP_SPEED                     0x00000100
  #define RESET_RMII                     0x00000800
#define TEST                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x01c)    // Test Register
#define MCFG                             *(unsigned long*)(ETHERNET_CONTROLLER_BLOCK + 0x020)    // MII Management Configuration Register
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


typedef struct stLPC17XX_RX_BD
{
volatile unsigned char   *ptrBlock;
volatile unsigned long    bd_details;
} LPC17XX_RX_BD;

#define RX_LAN_SIZE_MASK                  0x000007ff
#define RX_LAN_INTERRUPT_ON_DONE          0x80000000
#define TX_OVERRIDE                       0x04000000
#define TX_HUGE_FRAME                     0x08000000
#define TX_PAD_FRAME                      0x10000000
#define TX_APPEND_CRC                     0x20000000
#define TX_LAST_FRAG                      0x40000000
#define TX_LAN_INTERRUPT_ON_DONE          0x80000000

typedef struct stLPC17XX_RX_STATUS
{
volatile unsigned long    ulStatusInfo;
volatile unsigned long    ulStatusHASH_CRC;
} LPC17XX_RX_STATUS;

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

typedef struct stLPC17XX_TX_STATUS
{
volatile unsigned long    ulStatusInfo;
} LPC17XX_TX_STATUS;

#define COLLISION_COUNT_SHIFT             21
#define COLLISION_COUNT_MASK              0x01e00000
#define DEFER                             0x02000000
#define EXCESSIVE_DEFER                   0x04000000
#define EXCESSIVE_COLLISION               0x08000000
#define LATE_COLLISION                    0x10000000
#define TX_LAN_OVERRUN                    0x20000000
#define TX_NO_DESCRIPTOR                  0x40000000
#define TX_FRAME_ERROR                    0x80000000


// DP83848 PHYSICAL
#define DP83848_BASIC_CONTROL_REGISTER                 (0x0)
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
#define DP83848_BASIC_STATUS_REGISTER                  (0x1)
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
#define DP83848_PHY_IDENTIFIER_1                       (0x2)
#define DP83848_PHY_IDENTIFIER_2                       (0x3)
#define DP83848_AUTO_NEG_ADVERT_REG                    (0x4)
#define DP83848_AUTO_NEG_LINK_PARTNER_ABIL_REG         (0x5)
#define DP83848_AUTO_NEG_EXPANSION_REG                 (0x6)
#define DP83848_AUTO_NEG_NEXT_PAGE_REG                 (0x7)
#define DP83848_LINK_PARTNER_NEXT_PAGE_ABIL            (0x8)
#define DP83848_RXER_COUNTER_REGISTER                  (0x15)
#define DP83848_INTERRUPT_CONTROL_STATUS_REGISTER      (0x1b)
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
#define LAN8700_INTERRUPT_FLAG                         (0x1d)
#define LAN8700_INTERRUPT_MASK                         (0x1e)
  #define PHY_LAN8700_AUTO_NEG_PAGE_RECEIVED          0x0002
  #define PHY_LAN8700_PARALLEL_DETECTION_FAULT        0x0004
  #define PHY_LAN8700_AUTO_NEG_LP_ACK                 0x0008
  #define PHY_LAN8700_LINK_DOWN                       0x0010
  #define PHY_LAN8700_REMOTE_FAULT_DETECTED           0x0020
  #define PHY_LAN8700_AUTO_NEG_COMPLETE               0x0040
  #define PHY_LAN8700_ENERGYON_GENERATED              0x0080
#define DP83848_100BASETX_PHY_CONTROL_REGISTER         (0x1f)
  #define PHY_DISABLE_SCRAMBLER                       0x0001
  #define PHY_ENABLE_SQE_TEST                         0x0002
  #define PHY_OP_MODE_MASK                            0x001c
  #define PHY_OP_MODE_STILL_AUTONEGOTIATING           0x0000
  #define PHY_OP_MODE_10_BASE_T_HALF_DUPLEX           0x0004
  #define PHY_OP_MODE_100_BASE_T_HALF_DUPLEX          0x0008
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


#if defined LPC1788
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

#define LCD_PAL_ADD                      (unsigned long*)(LCD_CONTROLLER_BLOCK + 0x200)    // 256 x 16-bit Color Palette registers
  #define SIZE_OF_PAL                    0x200

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

// External Memory Controller
//
#define EMCControl                       *(unsigned long*)(EXT_MEMORY_CONTROLLER_BLOCK + 0x000)    // Controls operation of the memory controller
    #define EMC_ENABLE                   0x00000001
    #define EMC_ADDRESS_MIRROR           0x00000002
    #define EMC_LOW_POWER_MODE           0x00000004
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
  #define ADDRESS_MAP_128M_32BIT         0x00004480                      // 128M, 4 banks, row length 12, column length 9
  #define ADDRESS_MAP_256M_32BIT         0x00004680                      // 256M, 4 banks, row length 13, column length 9
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

#endif


#if defined LPC1788                                                      // {8}
    #define USBPortSel                   *(unsigned long*)(USB_CONTROLLER_BLOCK_LPC1788 + 0x000)   // USB port select register - only LPC1788
      #define PORT_SELECT_MAP_TO_1       0x00000000                                                // USB mapped to CONNECT_1, UP_LED1, D+1 and D-1
      #define PORT_SELECT_MAP_TO_2       0x00000003                                                // USB mapped to CONNECT_2, UP_LED2, D+2 and D-2
#endif

// USB device controller
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
  #define PORTSEL_CLK_EN                 0x00000008                                               // device clock enable (2378 only)
  #define AHB_CLK_EN                     0x00000010                                               // device clock enable
#define USBClkSt                         *(unsigned long*)(USB_CONTROLLER_BLOCK + 0xdf8)          // USB Clock Status

#define USB_FIFO_BUFFER_DEPTH 2

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



// Software references used when setting up interrupts
//
#define PORT_INTERRUPT            0
#define TIMER_INTERRUPT           1
#define ADC_INTERRUPT             2                                      // {4}


#define RIT_INTERRUPT             10

#define TIMER_SINGLE_SHOT         0x0000
#define TIMER_PWM_MAT0            0x0001
#define TIMER_PWM_MAT1            0x0002
#define TIMER_PWM_MAT2            0x0004
#define TIMER_PERIODIC            0x0010
#define TIMER_MS_VALUE            0x0000                                 // {5}
#define TIMER_US_VALUE            0x0020
#define TIMER_STOP_PWM_MAT0       0x0040
#define TIMER_STOP_PWM_MAT1       0x0080
#define TIMER_STOP_PWM_MAT2       0x0100
#define TIMER_DONT_DISTURB        0x0200


#define _PWM_PERCENT(percent_pwm, frequency_value)       ((frequency_value * percent_pwm)/100)
#define _PWM_TENTH_PERCENT(percent_pwm, frequency_value) ((frequency_value * percent_pwm)/1000)
#define TIMER_MS_DELAY(msec)           ((TIMER_CLOCK/1000)*msec)
#define TIMER_US_DELAY(usec)           ((TIMER_CLOCK/1000000)*usec)
#define TIMER_FREQUENCY_VALUE(hertz)   (1000000 / hertz)


// Define interrupt setup structures to suit this processor
//
typedef struct stINTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set

  unsigned char    int_port_sense;                                       // PULLUP_ON, PULLDOWN_ON, PULLUP_DOWN_OFF, IRQ_RISING_EDGE etc.
  unsigned char    int_port;                                             // the port. PORT_0 or PORT_2 have interrupt support - EXTINT also allowed
  unsigned long    int_port_bits;                                        // the input bits
} INTERRUPT_SETUP;

typedef struct stRIT_SETUP
{
  void ( *int_handler )( void );                                         // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set
  unsigned short   mode;                                                 // periodic or single shot
  unsigned long    count_delay;                                          // the delay in PCLKs
} RIT_SETUP;

#define RIT_SINGLE_SHOT  0x0001                                          // configure for single interrupt
#define RIT_PERIODIC     0x0002                                          // configure for periodic interrupt
#define RIT_STOP         0x8000                                          // stop operation

#define RIT_US_DELAY(us_delay)  ((us_delay * (RIT_CLOCK/1000))/1000)     // macros for setting RIT interrupt times
#define RIT_MS_DELAY(ms_delay)  (ms_delay * (RIT_CLOCK/1000))
#define RIT_S_DELAY(s_delay)    (s_delay * (RIT_CLOCK))

#define _HW_TIMER_MODE                                                   // {5} for compatibility with LPC2xxx projects

typedef struct stTIMER_INTERRUPT_SETUP
{
  void (*int_handler)( void );                                           // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set

  unsigned long    timer_value;                                          // the ms delay value
  unsigned long    pwm_value;                                            // pwm value
  unsigned short   timer_mode;                                           // timer mode
  unsigned char    timer_reference;                                      // hardware timer to use (0, 1, 2, 3)
} TIMER_INTERRUPT_SETUP;


typedef struct stRTC_SETUP                                               // {1}
{
    void (*int_handler)(void);                                           // interrupt handler to be configured
    unsigned short   usYear;
    unsigned char    ucMonthOfYear;
    unsigned char    ucDayOfMonth;
    unsigned char    ucDayOfWeek;
    unsigned char    ucHours;
    unsigned char    ucMinutes;
    unsigned char    ucSeconds;
    unsigned char    command;                                            // identifier for command to perform
} RTC_SETUP;


#define RTC_TIME_SETTING    0x01
#define RTC_TICK_SEC        0x02
#define RTC_TICK_MIN        0x03
#define RTC_TICK_HOUR       0x04
#define RTC_TICK_DAY        0x05
#define RTC_ALARM_TIME      0x06
#define RTC_STOPWATCH_GO    0x07
#define RTC_GET_TIME        0x08
#define RTC_DISABLE         0x80

#define RTC_INITIALISATION  0x40
#define RTC_32KHZ           0x20


#define PORT_0              0
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

// Define interrupt setup structure to suit this processor               {4}
//
typedef struct stADC_SETUP
{
  void (*int_handler)(void);                                             // interrupt handler to be configured
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



#define TIMERS_AVAILABLE    4
#ifdef LPC1788
    #define CHIP_HAS_UARTS  5
    #define PORTS_AVAILABLE 6
#else
    #define CHIP_HAS_UARTS  4
    #define PORTS_AVAILABLE 5
#endif
#define CHIP_HAS_IIC        3
#define PORT_WIDTH          32



// Cortex M3 private registers
//
// NVIC
//
#define INT_CONT_TYPE               *(const unsigned long*)(CORTEX_M3_BLOCK + 0x04)    // NVIC Interrupt Controller Type Register (read only)

// SYSTICK
//
#define SYSTICK_CSR                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x10) // SYSTICK Control and Status Register
  #define SYSTICK_ENABLE            0x00000001
  #define SYSTICK_TICKINT           0x00000002
  #define SYSTICK_CORE_CLOCK        0x00000004
  #define SYSTICK_COUNTFLAG         0x00010000
#define SYSTICK_RELOAD              *(unsigned long*)(CORTEX_M3_BLOCK + 0x14)          // SYSTICK Reload value
#define SYSTICK_CURRENT             *(unsigned long*)(CORTEX_M3_BLOCK + 0x18)          // SYSTICK Current value
  #define SYSTICK_COUNT_MASK        0x00ffffff                                         // valid count width in registers
#define SYSTICK_CALIB               *(const unsigned long*)(CORTEX_M3_BLOCK + 0x1c)    // SYSTICK Calibration value (not available in Stellaris devices) (read-only)

// NVIC
//
#define IRQ0_31_SER_ADD             ( unsigned long*)(CORTEX_M3_BLOCK + 0x100)
#define IRQ0_31_SER                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x100)// NVIC IRQ0..31    Set Enable Register
#define IRQ32_63_SER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x104)// NVIC IRQ32..64   Set Enable Register
#define IRQ64_95_SER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x108)// NVIC IRQ64..95   Set Enable Register
#define IRQ96_127_SER               *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x10c)// NVIC IRQ96..127  Set Enable Register
#define IRQ128_159_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x110)// NVIC IRQ128..159 Set Enable Register
#define IRQ160_191_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x114)// NVIC IRQ160..191 Set Enable Register
#define IRQ192_223_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x118)// NVIC IRQ192..223 Set Enable Register
#define IRQ224_239_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x11c)// NVIC IRQ224..239 Set Enable Register

#define IRQ0_31_CER                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x180)// NVIC IRQ0..31    Clear Enable Register
#define IRQ32_63_CER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x184)// NVIC IRQ32..64   Clear Enable Register
#define IRQ64_95_CER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x188)// NVIC IRQ64..95   Clear Enable Register
#define IRQ96_127_CER               *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x18c)// NVIC IRQ96..127  Clear Enable Register
#define IRQ128_159_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x190)// NVIC IRQ128..159 Clear Enable Register
#define IRQ160_191_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x194)// NVIC IRQ160..191 Clear Enable Register
#define IRQ192_223_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x198)// NVIC IRQ192..223 Clear Enable Register
#define IRQ224_239_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x19c)// NVIC IRQ224..239 Clear Enable Register

#define IRQ0_31_SPR                 *( unsigned long*)(CORTEX_M3_BLOCK + 0x200)        // NVIC IRQ0..31    Set Pending Register
#define IRQ32_63_SPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x204)        // NVIC IRQ32..64   Set Pending Register
#define IRQ64_95_SPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x208)        // NVIC IRQ64..95   Set Pending Register
#define IRQ96_127_SPR               *( unsigned long*)(CORTEX_M3_BLOCK + 0x20c)        // NVIC IRQ96..127  Set Pending Register
#define IRQ128_159_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x210)        // NVIC IRQ128..159 Set Pending Register
#define IRQ160_191_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x214)        // NVIC IRQ160..191 Set Pending Register
#define IRQ192_223_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x218)        // NVIC IRQ192..223 Set Pending Register
#define IRQ224_239_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x21c)        // NVIC IRQ224..239 Set Pending Register

#define IRQ0_31_CPR                 *( unsigned long*)(CORTEX_M3_BLOCK + 0x280)        // NVIC IRQ0..31    Clear Pending Register
#define IRQ32_63_CPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x284)        // NVIC IRQ32..64   Clear Pending Register
#define IRQ64_95_CPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x288)        // NVIC IRQ64..95   Clear Pending Register
#define IRQ96_127_CPR               *( unsigned long*)(CORTEX_M3_BLOCK + 0x28c)        // NVIC IRQ96..127  Clear Pending Register
#define IRQ128_159_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x290)        // NVIC IRQ128..159 Clear Pending Register
#define IRQ160_191_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x294)        // NVIC IRQ160..191 Clear Pending Register
#define IRQ192_223_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x298)        // NVIC IRQ192..223 Clear Pending Register
#define IRQ224_239_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x29c)        // NVIC IRQ224..239 Clear Pending Register

#define IRQ0_31_ABR                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x300)// NVIC IRQ0..31    Active Bit Register (read only)
#define IRQ32_63_ABR                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x304)// NVIC IRQ32..64   Active Bit Register (read only)
#define IRQ64_95_ABR                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x308)// NVIC IRQ64..95   Active Bit Register (read only)
#define IRQ96_127_ABR               *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x30c)// NVIC IRQ96..127  Active Bit Register (read only)
#define IRQ128_159_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x310)// NVIC IRQ128..159 Active Bit Register (read only)
#define IRQ160_191_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x314)// NVIC IRQ160..191 Active Bit Register (read only)
#define IRQ192_223_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x318)// NVIC IRQ192..223 Active Bit Register (read only)
#define IRQ224_239_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x31c)// NVIC IRQ224..239 Active Bit Register (read only)

#define IRQ0_3_PRIORITY_REGISTER_ADD ( unsigned char*)(CORTEX_M3_BLOCK + 0x400)
#define IRQ0_3_PRIORITY_REGISTER    *( unsigned long*)(CORTEX_M3_BLOCK + 0x400)        // NVIC IRQ0..3     Priority Register
#define IRQ4_7_PRIORITY_REGISTER    *( unsigned long*)(CORTEX_M3_BLOCK + 0x404)        // NVIC IRQ4..7     Priority Register
#define IRQ8_11_PRIORITY_REGISTER   *( unsigned long*)(CORTEX_M3_BLOCK + 0x408)        // NVIC IRQ8..11    Priority Register
#define IRQ12_15_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x40c)        // NVIC IRQ12..15   Priority Register
#define IRQ16_19_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x410)        // NVIC IRQ16..19   Priority Register
#define IRQ20_23_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x414)        // NVIC IRQ20..23   Priority Register
#define IRQ24_27_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x418)        // NVIC IRQ24..27   Priority Register
#define IRQ28_31_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x41c)        // NVIC IRQ28..31   Priority Register
#define IRQ32_35_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x420)        // NVIC IRQ32..35   Priority Register
#define IRQ36_39_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x424)        // NVIC IRQ36..39   Priority Register
#define IRQ40_43_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x428)        // NVIC IRQ40..43   Priority Register
#define IRQ44_47_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x42c)        // NVIC IRQ44..47   Priority Register
#define IRQ48_51_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x430)        // NVIC IRQ48..51   Priority Register
#define IRQ52_55_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x434)        // NVIC IRQ52..55   Priority Register
#define IRQ56_59_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x438)        // NVIC IRQ56..59   Priority Register
#define IRQ60_63_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x43c)        // NVIC IRQ60..63   Priority Register
#define IRQ64_67_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x440)        // NVIC IRQ64..67   Priority Register
#define IRQ68_71_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x444)        // NVIC IRQ68..71   Priority Register
#define IRQ72_75_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x448)        // NVIC IRQ72..75   Priority Register
#define IRQ76_79_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x44c)        // NVIC IRQ76..79   Priority Register
#define IRQ80_83_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x450)        // NVIC IRQ80..83   Priority Register
#define IRQ84_87_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x454)        // NVIC IRQ84..87   Priority Register
#define IRQ88_91_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x458)        // NVIC IRQ88..91   Priority Register
#define IRQ92_95_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x45c)        // NVIC IRQ92..95   Priority Register
#define IRQ96_99_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x460)        // NVIC IRQ96..99   Priority Register
#define IRQ100_103_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x464)      // NVIC IRQ100..103   Priority Register
#define IRQ104_107_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x468)      // NVIC IRQ104..107   Priority Register
#define IRQ108_111_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x46c)      // NVIC IRQ108..111   Priority Register
#define IRQ112_115_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x470)      // NVIC IRQ112..115   Priority Register
#define IRQ116_119_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x474)      // NVIC IRQ116..119   Priority Register
#define IRQ120_123_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x478)      // NVIC IRQ120..123   Priority Register
#define IRQ124_127_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x47c)      // NVIC IRQ124..127   Priority Register
#define IRQ128_131_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x480)      // NVIC IRQ128..131   Priority Register
#define IRQ132_135_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x484)      // NVIC IRQ132..135   Priority Register
#define IRQ136_139_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x488)      // NVIC IRQ136..139   Priority Register
#define IRQ140_143_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x48c)      // NVIC IRQ140..143   Priority Register
#define IRQ144_147_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x490)      // NVIC IRQ144..147   Priority Register
#define IRQ148_151_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x494)      // NVIC IRQ148..151   Priority Register
#define IRQ152_155_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x498)      // NVIC IRQ152..155   Priority Register
#define IRQ156_159_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x49c)      // NVIC IRQ156..159   Priority Register
#define IRQ160_163_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4a0)      // NVIC IRQ160..163   Priority Register
#define IRQ164_167_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4a4)      // NVIC IRQ164..167   Priority Register
#define IRQ168_171_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4a8)      // NVIC IRQ168..171   Priority Register
#define IRQ172_175_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4ac)      // NVIC IRQ172..175   Priority Register
#define IRQ176_179_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4b0)      // NVIC IRQ176..179   Priority Register
#define IRQ180_183_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4b4)      // NVIC IRQ180..183   Priority Register
#define IRQ184_187_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4b8)      // NVIC IRQ184..187   Priority Register
#define IRQ188_191_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4bc)      // NVIC IRQ188..191   Priority Register
#define IRQ192_195_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4c0)      // NVIC IRQ192..195   Priority Register
#define IRQ196_199_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4c4)      // NVIC IRQ196..199   Priority Register
#define IRQ200_203_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4c8)      // NVIC IRQ200..203   Priority Register
#define IRQ204_207_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4cc)      // NVIC IRQ204..207   Priority Register
#define IRQ208_211_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4d0)      // NVIC IRQ208..211   Priority Register
#define IRQ212_215_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4d4)      // NVIC IRQ212..215   Priority Register
#define IRQ216_219_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4d8)      // NVIC IRQ216..219   Priority Register
#define IRQ220_223_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4dc)      // NVIC IRQ220..223   Priority Register
#define IRQ224_227_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4e0)      // NVIC IRQ224..227   Priority Register
#define IRQ228_231_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4e4)      // NVIC IRQ228..231   Priority Register
#define IRQ232_235_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4e8)      // NVIC IRQ232..235   Priority Register
#define IRQ236_239_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4ec)      // NVIC IRQ236..239   Priority Register

#define CPUID_BASE_REGISTER           *(unsigned long*)(CORTEX_M3_BLOCK + 0xd00)       // (read only)
#define INT_CONT_STATE_REG            *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0xd04) // Interrupt Control State Register
  #define NMIPENDSET                  0x80000000                                       // set pending NMI bit
  #define PENDSVSET                   0x10000000                                       // set pending pendSV bit
  #define PENDSVCLR                   0x08000000                                       // clear pending pendSV bit
  #define PENDSTSET                   0x04000000                                       // set pending sysTick bit
  #define PENDSTCLR                   0x02000000                                       // clear pending sysTick bit
  #define ISRPREEMPT                  0x00800000                                       //
  #define ISRPENDING                  0x00400000                                       // Interrupt Pending Flag
  #define VECT_PENDING_MASK           0x003ff000                                       // Pending ISR number field
  #define RETTOBASE                   0x00000800                                       //
  #define VECT_ACTIVE_MASK            0x00000010                                       // Active ISR number field
#define VECTOR_TABLE_OFFSET_REG       *(unsigned long*)(CORTEX_M3_BLOCK + 0xd08)       // Interrupt Control State Register
  #define TBLBASE_IN_RAM              0x20000000                                       // vector table base is in RAM
  #define TBLBASE_IN_CODE             0x00000000
  #define TBLOFF_MASK                 0x1fffff80                                       // table offset from bottom of Code / RAM
#define APPLICATION_INT_RESET_CTR_REG *(unsigned long*)(CORTEX_M3_BLOCK + 0xd0c)       // Application Interrupt and Reset Control Register
  #define VECTKEY                     0x05fa0000
  #define ENDIANESS_BIG               0x00008000
  #define ENDIANESS_LITTLE            0x00000000
  #define PRIGROUP_7_1                0x00000100
  #define PRIGROUP_6_2                0x00000200
  #define PRIGROUP_5_3                0x00000300
  #define PRIGROUP_4_4                0x00000400
  #define PRIGROUP_3_5                0x00000500
  #define PRIGROUP_2_6                0x00000600
  #define PRIGROUP_1_7                0x00000700
  #define PRIGROUP_0_8                0x00000800
  #define SYSRESETREQ                 0x00000004
  #define VECTCLRACTIVE               0x00000002
  #define VECTRESET                   0x00000001
#define SYSTEM_CONTROL_REGISTER       *(unsigned long*)(CORTEX_M3_BLOCK + 0xd10)       // System Control Register
  #define SLEEPONEXIT                 0x00000002
  #define SLEEPDEEP                   0x00000004
  #define SEVONPEND                   0x00000010
#define CONFIGURATION_CONTROL_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd14)      // Configuration Control Register

#define SYSTEM_HANDLER_4_7_PRIORITY_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd18) // System Handler Priority Register 4..7
#define SYSTEM_HANDLER_8_11_PRIORITY_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd1c) // System Handler Priority Register 8..11
#define SYSTEM_HANDLER_12_15_PRIORITY_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd20) // System Handler Priority Register 12..15

// Interrupt sources
//
#define irq_Watchdog_Timer_ID         0
#define irq_Timer0_ID                 1
#define irq_Timer1_ID                 2
#define irq_Timer2_ID                 3
#define irq_Timer3_ID                 4
#define irq_UART0_ID                  5
#define irq_UART1_ID                  6
#define irq_UART2_ID                  7
#define irq_UART3_ID                  8
#define irq_PWM1_ID                   9
#define irq_I2C0_ID                   10
#define irq_I2C1_ID                   11
#define irq_I2C2_ID                   12
#define irq_SPI_ID                    13
#define irq_SSP0_ID                   14
#define irq_SSP1_ID                   15
#define irq_PLL0_ID                   16
#define irq_RTC_ID                    17
#define irq_EINT0_ID                  18
#define irq_EINT1_ID                  19
#define irq_EINT2_ID                  20
#define irq_EINT3_ID                  21
#define irq_ADC_ID                    22
#define irq_BOD_ID                    23
#define irq_USB_ID                    24
#define irq_CAN_ID                    25
#define irq_GPDMA_ID                  26
#define irq_I2S_ID                    27
#define irq_Ethernet_Controller_ID    28
#define irq_RITINT_ID                 29
#define irq_MotorControlPWM_ID        30
#define irq_QuadEncoder_ID            31
#define irq_PLL1_ID                   32
#define irq_USB_Activity_ID           33
#define irq_CAN_Activity_ID           34
#define irq_UART4_ID                  35                                 // LPC1788
#define irq_SSP2_ID                   36                                 // LPC1788
#define irq_LCD_ID                    37                                 // LPC1788
#define irq_GPIO_ID                   38                                 // LPC1788
#define irq_PWM0_ID                   39                                 // LPC1788
#define irq_EEPROM_ID                 40                                 // LPC1788


#define PIN_COUNT_144_PIN             0
#define PIN_COUNT_180_PIN             1
#define PIN_COUNT_208_PIN             2

#define PACKAGE_LQFP                  0
#define PACKAGE_TFBGA                 1

#ifdef LPC1788
    #define UARTS_AVAILABLE           5
#else
    #define UARTS_AVAILABLE           4
#endif

/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:   	    LM3SXXXX.h
   Project: 	Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   *********************************************************************

   16.12.2008 Add GPIO H interrupt for specific processors               {1}
   18.12.2008 PORTS_AVAILABLE and PORT_WIDTH moved here from WinSim.c    {2}
   18.12.2008 Add USB registers                                          {3}
   02.01.2009 Add uDMA registers                                         {4}
   17.01.2009 Add CAN registers                                          {5}
   18.01.2009 Add SSI1 registers                                         {6}
   18.01.2009 Added device specific CHIP_HAS_UARTS                       {7}
   05.02.2009 Added TIMER interrupt support                              {8}
   05.02.2009 Add _CLEAR_INTERRUPT macro                                 {9}
   31.03.2009 Add ADC registers                                          {10}
   03.08.2009 Add LM3S2139 configuration                                 {11}
   12.08.2009 Add port simulation to port macros                         {12}
   29.08.2009 Add timer PWM configuration                                {13}
   03.09.2009 Add alternative DMA channel assignments                    {14}
   08.09.2009 Add internal temperature to ADC channels                   {15}
   05.11.2009 Add LM3S9B90 and extend Tempest class crystals             {16}
   07.11.2009 Adjust PWM to 1/10 % from 1% steps                         {17}
   11.11.2009 Extend PLL support for LM3S9BXX                            {18}
   29.11.2009 User line buffer with LM3S9BXX FLASH to avoid programming long words more than once {19}
   26.02.2010 Add LM3S6911 configuration                                 {20}
   13.05.2010 Add LM3S1958 configuration                                 {21}
   15.05.2010 Add additional port macros _CONFIG_PORT_PULLUP(), _CONFIG_PORT_PULLDOWN(), _UNLOCK_GPIO(), _UNLOCK_PERIPHERAL(), and _LOCK_PORT() {22}
   15.05.2010 Use volatile with port masking addresses                   {23}
   07.07.2010 Correct VECTKEY value                                      {24}
   08.07.2010 Add CGC_SSI1 bit                                           {25}
   11.07.2010 Correct ADC_CHANNEL_15 and add CGC_SARADC1 and LM3XXXXX_ADC{26}
   10.08.2010 Add SDCARD_MALLOC()                                        {27}
   17.08.2010 Add _RESET_PORT_PERIPHERAL() macro                         {28}
   06.10.2010 Add 16 bit timer mode                                      {29}
   27.12.2010 Add PWM_CONFIGURATION                                      {30}
   27.12.2010 Add _LM3S1776                                              {31}
   22.02.2011 Add _LM3S9B96                                              {32}

*/

#if defined _WINDOWS
    #include "SimLM3SXXXX.h"
    #define _EXCEPTION(x)        *(unsigned char *)0 = 0                 // generate exception when simulating
#else
    #define _EXCEPTION(x)
#endif

/* =================================================================== */
/*                           struct packing control                    */
/* =================================================================== */

#if defined _GNU
    #define _PACK      __attribute__((__packed__))
#else
    #define _PACK
#endif
#define __PACK_ON 
#define __PACK_OFF


#define _ALIGN_HEAP_4
#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary

#define SDCARD_MALLOC(x) uMalloc(x)                                      // use standard uMalloc() {27}

#define SAVE_COMPLETE_FLASH                                              // when simulating save complete FLASH content rather than only file system content

#define _LITTLE_ENDIAN                                                   // compile and operate project in LITTLE ENDIAN mode

#define MASTER_CLOCK    PLL_OUTPUT_FREQ                                  // 50 MHz operation max. (80MHz for LM3S9BXX)

#define SLOW_CLOCK      32000                                            // RC oscillator

#define CAST_POINTER_ARITHMETIC unsigned long                            // LM3SXXXX uses 32 bit pointers

#define ARM_MATH_CM3

#if defined _WINDOWS
    extern unsigned char uninitialisedRAM[16];
    #define BOOT_MAIL_BOX           (unsigned short *)&uninitialisedRAM[0]
    #define RANDOM_SEED_LOCATION    (unsigned short *)&uninitialisedRAM[2]
    #define RTC_SECONDS_LOCATION    (unsigned long *)&uninitialisedRAM[4]
    #define RTC_ALARM_LOCATION      (unsigned long *)&uninitialisedRAM[8]
    #define RTC_VALID_LOCATION      (unsigned short *)&uninitialisedRAM[12]
    #define RTC_PRESCALER_LOCATION  (unsigned short *)&uninitialisedRAM[14]
#else
    #define BOOT_MAIL_BOX           (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 2))
    #define RANDOM_SEED_LOCATION    (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 4)) // location of a short word which is never initialised and so has a random power on value
    #define RTC_SECONDS_LOCATION    (unsigned long *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 8))
    #define RTC_ALARM_LOCATION      (unsigned long *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 12))
    #define RTC_VALID_LOCATION      (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 14))
    #define RTC_PRESCALER_LOCATION  (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 16))
#endif

#if !defined PERSISTENT_RAM_SIZE
    #define PERSISTENT_RAM_SIZE          0
#endif
#define NON_INITIALISED_RAM_SIZE         (4 + PERSISTENT_RAM_SIZE)       // reserve a long word at the end of SRAM for use by random number and serial loader mailbox

// The Luminary-Micro devices sorted by class
//
#if defined _LM3S9B90 || defined _LM3S9B92 || defined _LM3S9B95 || defined _LM3S9B96
    #define _TEMPEST_CLASS                                               // 2009
#elif defined _LM3S3748 || defined _LM3S3768 || defined _LM3S1627 || defined _LM3S5732 || defined _LM3S2678 || defined _LM3S5632 || defined _LM3S1776
    #define _DUST_DEVIL_CLASS                                            // 2008
#elif defined _LM3S6965 || defined _LM3S8962 || defined _LM3S1958 || defined _LM3S1968 || defined _LM3S1138 || defined _LM3S6938 || defined _LM3S2965 || defined _LM3S2139 || defined LM3S6911
    #define _FURY_CLASS                                                  // 2007
#else                                                                    // all others
    #define _SANDSTORM_CLASS                                             // 2006
#endif
                             
#if (CRYSTAL_FREQ == 10000000) && defined _TEMPEST_CLASS             // {16}
    #define CRYSTAL_SPEED_VALUE     XTAL_10_0000
#elif (CRYSTAL_FREQ == 12000000) && defined _TEMPEST_CLASS 
    #define CRYSTAL_SPEED_VALUE     XTAL_12_0000
#elif (CRYSTAL_FREQ == 12288000) && defined _TEMPEST_CLASS 
    #define CRYSTAL_SPEED_VALUE     XTAL_12_2880
#elif (CRYSTAL_FREQ == 13560000) && defined _TEMPEST_CLASS 
    #define CRYSTAL_SPEED_VALUE     XTAL_13_5600
#elif (CRYSTAL_FREQ == 14318180) && defined _TEMPEST_CLASS 
    #define CRYSTAL_SPEED_VALUE     XTAL_14_318180
#elif (CRYSTAL_FREQ == 16000000) && defined _TEMPEST_CLASS 
    #define CRYSTAL_SPEED_VALUE     XTAL_16_0000
#elif (CRYSTAL_FREQ == 16384000) && defined _TEMPEST_CLASS 
    #define CRYSTAL_SPEED_VALUE     XTAL_16_3840
#elif CRYSTAL_FREQ   == 8192000                                          // VCO output is always 400MHz
    #define CRYSTAL_SPEED_VALUE     XTAL_8_1920
#elif CRYSTAL_FREQ == 8000000
    #define CRYSTAL_SPEED_VALUE     XTAL_8_0000
#elif CRYSTAL_FREQ == 7372800
    #define CRYSTAL_SPEED_VALUE     XTAL_7_3728
#elif CRYSTAL_FREQ == 6144000
    #define CRYSTAL_SPEED_VALUE     XTAL_6_1440
#elif CRYSTAL_FREQ == 6000000
    #define CRYSTAL_SPEED_VALUE     XTAL_6_0000
#elif CRYSTAL_FREQ == 5120000
    #define CRYSTAL_SPEED_VALUE     XTAL_5_1200
#elif CRYSTAL_FREQ == 5000000
    #define CRYSTAL_SPEED_VALUE     XTAL_5_0000
#elif CRYSTAL_FREQ == 4915200
    #define CRYSTAL_SPEED_VALUE     XTAL_4_9152
#elif CRYSTAL_FREQ == 4096000
    #define CRYSTAL_SPEED_VALUE     XTAL_4_096
#elif CRYSTAL_FREQ == 4000000
    #define CRYSTAL_SPEED_VALUE     XTAL_4_0000
#elif CRYSTAL_FREQ == 3686400
    #define CRYSTAL_SPEED_VALUE     XTAL_3_6864
#elif CRYSTAL_FREQ == 3579545
    #define CRYSTAL_SPEED_VALUE     XTAL_3_579545
#else
    #error Invalid crystal frequency: a crystal frequency from the allowed list must be used!!
#endif

#if (PLL_OUTPUT_FREQ   == 100000000) && defined _LM3S9B95                // {16}
    #define PLL_OUTPUT_VALUE        SYSDIV_4
#elif (PLL_OUTPUT_FREQ == 80000000) && defined _TEMPEST_CLASS
    #define PLL_OUTPUT_VALUE        SYSDIV_6
    #define EXTENDED_PLL_SETTING                                         // {18}
#elif (PLL_OUTPUT_FREQ == 66666666) && defined _TEMPEST_CLASS
    #define PLL_OUTPUT_VALUE        SYSDIV_6
#elif PLL_OUTPUT_FREQ   == 50000000                                      // the possible PLL output frequencies
    #define PLL_OUTPUT_VALUE        SYSDIV_8
#elif PLL_OUTPUT_FREQ == 44444444
    #define PLL_OUTPUT_VALUE        SYSDIV_10
    #define EXTENDED_PLL_SETTING                                         // {18}
#elif PLL_OUTPUT_FREQ == 40000000
    #define PLL_OUTPUT_VALUE        SYSDIV_10
#elif PLL_OUTPUT_FREQ == 33333333
    #define PLL_OUTPUT_VALUE        SYSDIV_12
#elif PLL_OUTPUT_FREQ == 28571429
    #define PLL_OUTPUT_VALUE        SYSDIV_14
#elif PLL_OUTPUT_FREQ == 25000000
    #define PLL_OUTPUT_VALUE        SYSDIV_16
#elif PLL_OUTPUT_FREQ == 22222222
    #define PLL_OUTPUT_VALUE        SYSDIV_18
#elif PLL_OUTPUT_FREQ == 20000000
    #define PLL_OUTPUT_VALUE        SYSDIV_20
#elif PLL_OUTPUT_FREQ == 18181818
    #define PLL_OUTPUT_VALUE        SYSDIV_22
#elif PLL_OUTPUT_FREQ == 16666667
    #define PLL_OUTPUT_VALUE        SYSDIV_24
#elif PLL_OUTPUT_FREQ == 15384615
    #define PLL_OUTPUT_VALUE        SYSDIV_26
#elif PLL_OUTPUT_FREQ == 14285714
    #define PLL_OUTPUT_VALUE        SYSDIV_28
#elif PLL_OUTPUT_FREQ == 13333333
    #define PLL_OUTPUT_VALUE        SYSDIV_30
#elif PLL_OUTPUT_FREQ == 12500000
    #define PLL_OUTPUT_VALUE        SYSDIV_32
#else
    #error Invalid PLL output frequency: a frequency from the allowed list must be used!!
#endif

#define FLASH_START_ADDRESS            0x00000000                        // start of internal FLASH (fixed)
#define RAM_START_ADDRESS              0x20000000                        // start of internal RAM (fixed)


/*****************************************************************************************************/
// Device capabilities - DC0
  #define SRAM_SIZE_4K              0x000f0000
  #define SRAM_SIZE_8K              0x001f0000
  #define SRAM_SIZE_16K             0x003f0000
  #define SRAM_SIZE_32K             0x007f0000
  #define SRAM_SIZE_64K             0x00ff0000
  #define SRAM_SIZE_96K             0x017f0000
  #define FLASH_SIZE_16K            0x00000007
  #define FLASH_SIZE_32K            0x0000000f
  #define FLASH_SIZE_64K            0x0000001f
  #define FLASH_SIZE_128K           0x0000003f
  #define FLASH_SIZE_256K           0x0000007f
// Device capabilities - DC1
  #define JTAG_PRESENT1             0x00000001
  #define SWD_PRESENT1              0x00000002
  #define SWO_PRESENT1              0x00000004
  #define WATCHDOG0_PRESENT1        0x00000008
  #define PLL_PRESENT1              0x00000010
  #define TEMP_SENSOR_PRESENT1      0x00000020
  #define HIB_MODULE_PRESENT1       0x00000040
  #define MEMPROT_PRESENT1          0x00000080
  #define MAX_ADC0_1MEG_1           0x00000300
  #define MAX_ADC1_1MEG_1           0x00000c00
  #define MIN_SYS_DIV_400M_5_1      0x00001000
  #define MIN_SYS_DIV_400M_6_1      0x00002000
  #define MIN_SYS_DIV_50M_4_1       0x00003000
  #define MIN_SYS_DIV_25M_8_1       0x00007000
  #define MIN_SYS_DIV_20M_10_1      0x00009000
  #define ADC0_PRESENT1             0x00010000
  #define ADC1_PRESENT1             0x00020000
  #define PWM_PRESENT1              0x00100000
  #define CAN0_PRESENT1             0x01000000
  #define CAN1_PRESENT1             0x02000000
  #define WATCHDOG1_PRESENT1        0x10000000
// Device capabilities - DC2
  #define UART0_PRESENT2            0x00000001
  #define UART1_PRESENT2            0x00000002
  #define UART2_PRESENT2            0x00000004
  #define SSI0_PRESENT2             0x00000010
  #define SSI1_PRESENT2             0x00000020
  #define QEI0_PRESENT2             0x00000100
  #define QEI1_PRESENT2             0x00000200
  #define I2C0_PRESENT2             0x00001000
  #define I2C1_PRESENT2             0x00004000
  #define TIMER0_PRESENT2           0x00010000
  #define TIMER1_PRESENT2           0x00020000
  #define TIMER2_PRESENT2           0x00040000
  #define TIMER3_PRESENT2           0x00080000
  #define COMP0_PRESENT2            0x01000000
  #define COMP1_PRESENT2            0x02000000
  #define COMP2_PRESENT2            0x04000000
  #define I2S0_PRESENT2             0x10000000
  #define EPI0_PRESENT2             0x40000000
// Device capabilities - DC3
  #define PWM0_PRESENT3             0x00000001
  #define PWM1_PRESENT3             0x00000002
  #define PWM2_PRESENT3             0x00000004
  #define PWM3_PRESENT3             0x00000008
  #define PWM4_PRESENT3             0x00000010
  #define PWM5_PRESENT3             0x00000020
  #define C0MINUS_PRESENT3          0x00000040
  #define C0PLUS_PRESENT3           0x00000080
  #define C1MINUS_PRESENT3          0x00000200
  #define C1PLUS_PRESENT3           0x00000400
  #define PWM_FAULT_PRESENT3        0x00008000
  #define ADC0_PRESENT3             0x00010000
  #define ADC1_PRESENT3             0x00020000
  #define ADC2_PRESENT3             0x00040000
  #define ADC3_PRESENT3             0x00080000
  #define ADC4_PRESENT3             0x00100000
  #define ADC5_PRESENT3             0x00200000
  #define ADC6_PRESENT3             0x00400000
  #define ADC7_PRESENT3             0x00800000
  #define CCP0_PRESENT3             0x01000000
  #define CCP1_PRESENT3             0x02000000
  #define CCP2_PRESENT3             0x04000000
  #define CCP3_PRESENT3             0x08000000
  #define CCP4_PRESENT3             0x10000000
  #define CCP5_PRESENT3             0x20000000
  #define INPUT32K_PRESENT3         0x80000000
// Device capabilities - DC4
  #define GPIOA_PRESENT4            0x00000001
  #define GPIOB_PRESENT4            0x00000002
  #define GPIOC_PRESENT4            0x00000004
  #define GPIOD_PRESENT4            0x00000008
  #define GPIOE_PRESENT4            0x00000010
  #define GPIOF_PRESENT4            0x00000020
  #define GPIOG_PRESENT4            0x00000040
  #define GPIOH_PRESENT4            0x00000080
  #define GPIOJ_PRESENT4            0x00000100
  #define CODE_ROM_PRESENT4         0x00001000
  #define UDMA_PRESENT4             0x00002000
  #define CCP6_PRESENT4             0x00004000
  #define CCP7_PRESENT4             0x00008000
  #define E1588_PRESENT4            0x01000000
  #define EPHY0_PRESENT4            0x40000000
  #define EMAC0_PRESENT4            0x10000000
// Device capabilities - DC5
  #define PWM0_PRESENT5             0x00000001
  #define PWM1_PRESENT5             0x00000002
  #define PWM2_PRESENT5             0x00000004
  #define PWM3_PRESENT5             0x00000008
  #define PWM4_PRESENT5             0x00000010
  #define PWM5_PRESENT5             0x00000020
  #define PWM6_PRESENT5             0x00000040
  #define PWM7_PRESENT5             0x00000080
  #define PWM_EXTSYNC_PRESENT5      0x00100000
  #define PWM_EXTFAULT_PRESENT5     0x00200000
  #define PWM_FAULT0_PRESENT5       0x01000000
  #define PWM_FAULT1_PRESENT5       0x02000000
  #define PWM_FAULT2_PRESENT5       0x04000000
  #define PWM_FAULT3_PRESENT5       0x08000000
// Device capabilities - DC6
  #define USB0_DEVICE_HOST_PRESENT6 0x00000002
  #define USB0_OTG_PRESENT6         0x00000003
  #define USB0_PHY_PRESENT6         0x00000010
// Device capabilities - DC7
  #define DMA_0_PRESENT7            0x00000001
  #define DMA_1_PRESENT7            0x00000002
  #define DMA_2_PRESENT7            0x00000004
  #define DMA_3_PRESENT7            0x00000008
  #define DMA_4_PRESENT7            0x00000010
  #define DMA_5_PRESENT7            0x00000020
  #define DMA_6_PRESENT7            0x00000040
  #define DMA_7_PRESENT7            0x00000080
  #define DMA_8_PRESENT7            0x00000100
  #define DMA_9_PRESENT7            0x00000200
  #define DMA_10_PRESENT7           0x00000400
  #define DMA_11_PRESENT7           0x00000800
  #define DMA_12_PRESENT7           0x00001000
  #define DMA_13_PRESENT7           0x00002000
  #define DMA_14_PRESENT7           0x00004000
  #define DMA_15_PRESENT7           0x00008000
  #define DMA_16_PRESENT7           0x00010000
  #define DMA_17_PRESENT7           0x00020000
  #define DMA_18_PRESENT7           0x00040000
  #define DMA_19_PRESENT7           0x00080000
  #define DMA_20_PRESENT7           0x00100000
  #define DMA_21_PRESENT7           0x00200000
  #define DMA_22_PRESENT7           0x00400000
  #define DMA_23_PRESENT7           0x00800000
  #define DMA_24_PRESENT7           0x01000000
  #define DMA_25_PRESENT7           0x02000000
  #define DMA_26_PRESENT7           0x04000000
  #define DMA_27_PRESENT7           0x08000000
  #define DMA_28_PRESENT7           0x10000000
  #define DMA_29_PRESENT7           0x20000000
  #define DMA_30_PRESENT7           0x40000000
  #define DMA_31_PRESENT7           0x80000000
// Device capabilities - DC8
  #define ADC0AIN0_PRESENT8         0x00000001
  #define ADC0AIN1_PRESENT8         0x00000002
  #define ADC0AIN2_PRESENT8         0x00000004
  #define ADC0AIN3_PRESENT8         0x00000008
  #define ADC0AIN4_PRESENT8         0x00000010
  #define ADC0AIN5_PRESENT8         0x00000020
  #define ADC0AIN6_PRESENT8         0x00000040
  #define ADC0AIN7_PRESENT8         0x00000080
  #define ADC0AIN8_PRESENT8         0x00000100
  #define ADC0AIN9_PRESENT8         0x00000200
  #define ADC0AIN10_PRESENT8        0x00000400
  #define ADC0AIN11_PRESENT8        0x00000800
  #define ADC0AIN12_PRESENT8        0x00001000
  #define ADC0AIN13_PRESENT8        0x00002000
  #define ADC0AIN14_PRESENT8        0x00004000
  #define ADC0AIN15_PRESENT8        0x00008000
  #define ADC1AIN0_PRESENT8         0x00010000
  #define ADC1AIN1_PRESENT8         0x00020000
  #define ADC1AIN2_PRESENT8         0x00040000
  #define ADC1AIN3_PRESENT8         0x00080000
  #define ADC1AIN4_PRESENT8         0x00100000
  #define ADC1AIN5_PRESENT8         0x00200000
  #define ADC1AIN6_PRESENT8         0x00400000
  #define ADC1AIN7_PRESENT8         0x00800000
  #define ADC1AIN8_PRESENT8         0x01000000
  #define ADC1AIN9_PRESENT8         0x02000000
  #define ADC1AIN10_PRESENT8        0x04000000
  #define ADC1AIN11_PRESENT8        0x08000000
  #define ADC1AIN12_PRESENT8        0x10000000
  #define ADC1AIN13_PRESENT8        0x20000000
  #define ADC1AIN14_PRESENT8        0x40000000
  #define ADC1AIN15_PRESENT8        0x80000000
/*****************************************************************************************************/
#define CODE_LM3S101            (0x01 << 16)                             // 28 SOIC
#define CODE_LM3S102            (0x02 << 16)                             // 28 SOIC

#define CODE_LM3S300            (0x?? << 16)                             // 48 LQFP
#define CODE_LM3S301            (0x?? << 16)
#define CODE_LM3S308            (0x1a << 16)
#define CODE_LM3S310            (0x?? << 16)
#define CODE_LM3S316            (0x?? << 16)
#define CODE_LM3S317            (0x?? << 16)
#define CODE_LM3S328            (0x?? << 16)
#define CODE_LM3S600            (0x?? << 16)
#define CODE_LM3S601            (0x?? << 16)
#define CODE_LM3S608            (0x2b << 16)
#define CODE_LM3S610            (0x?? << 16)
#define CODE_LM3S611            (0x?? << 16)
#define CODE_LM3S612            (0x?? << 16)
#define CODE_LM3S613            (0x?? << 16)
#define CODE_LM3S615            (0x?? << 16)
#define CODE_LM3S617            (0x?? << 16)
#define CODE_LM3S618            (0x?? << 16)
#define CODE_LM3S628            (0x?? << 16)
#define CODE_LM3S800            (0x?? << 16)
#define CODE_LM3S801            (0x?? << 16)
#define CODE_LM3S811            (0x?? << 16)
#define CODE_LM3S828            (0x?? << 16)
#define CODE_LM3S312            (0x?? << 16)
#define CODE_LM3S315            (0x?? << 16)
#define CODE_LM3S815            (0x?? << 16)
#define CODE_LM3S817            (0x?? << 16)
#define CODE_LM3S818            (0x37 << 16)

#define CODE_LM3S1607           (0x?? << 16)                             // 64 LQFP
#define CODE_LM3S1625           (0x?? << 16)
#define CODE_LM3S1626           (0x?? << 16)
#define CODE_LM3S1627           (0x05 << 16)
#define CODE_LM3S1776           (0x10 << 16)                             // {31}
#define CODE_LM3S2276           (0x?? << 16)
#define CODE_LM3S2616           (0x?? << 16)
#define CODE_LM3S2671           (0x?? << 16)
#define CODE_LM3S2678           (0x50 << 16)
#define CODE_LM3S3651           (0x?? << 16)
#define CODE_LM3S5632           (0x81 << 16)
#define CODE_LM3S5652           (0x?? << 16)
#define CODE_LM3S5662           (0x?? << 16)
#define CODE_LM3S5732           (0x96 << 16)
#define CODE_LM3S5752           (0x?? << 16)
#define CODE_LM3S5762           (0x?? << 16)

#define CODE_LM3S1110           (0x?? << 16)                             // 100 LQFP
#define CODE_LM3S1133           (0x?? << 16)
#define CODE_LM3S1138           (0xc5 << 16)
#define CODE_LM3S1150           (0x?? << 16)
#define CODE_LM3S1162           (0x?? << 16)
#define CODE_LM3S1165           (0x?? << 16)
#define CODE_LM3S1332           (0x?? << 16)
#define CODE_LM3S1435           (0x?? << 16)
#define CODE_LM3S1439           (0x?? << 16)
#define CODE_LM3S1512           (0x?? << 16)
#define CODE_LM3S1538           (0x?? << 16)
#define CODE_LM3S1601           (0x?? << 16)
#define CODE_LM3S1608           (0x?? << 16)
#define CODE_LM3S1620           (0x?? << 16)
#define CODE_LM3S1635           (0x?? << 16)
#define CODE_LM3S1637           (0x?? << 16)
#define CODE_LM3S1751           (0x?? << 16)
#define CODE_LM3S1850           (0x?? << 16)
#define CODE_LM3S1911           (0x?? << 16)
#define CODE_LM3S1918           (0x?? << 16)
#define CODE_LM3S1937           (0x?? << 16)
#define CODE_LM3S1958           (0xbe << 16)
#define CODE_LM3S1960           (0x?? << 16)
#define CODE_LM3S1968           (0xb8 << 16)
#define CODE_LM3S2110           (0x51 << 16)
#define CODE_LM3S2139           (0x84 << 16)
#define CODE_LM3S2410           (0x?? << 16)
#define CODE_LM3S2412           (0x?? << 16)
#define CODE_LM3S2432           (0x?? << 16)
#define CODE_LM3S2533           (0x?? << 16)
#define CODE_LM3S2601           (0x?? << 16)
#define CODE_LM3S2608           (0x?? << 16)
#define CODE_LM3S2620           (0x?? << 16)
#define CODE_LM3S2637           (0x?? << 16)
#define CODE_LM3S2651           (0x?? << 16)
#define CODE_LM3S2730           (0x?? << 16)
#define CODE_LM3S2739           (0x?? << 16)
#define CODE_LM3S2776           (0x?? << 16)
#define CODE_LM3S2793           (0x?? << 16)
#define CODE_LM3S2911           (0x?? << 16)
#define CODE_LM3S2918           (0x?? << 16)
#define CODE_LM3S2939           (0x?? << 16)
#define CODE_LM3S2948           (0x?? << 16)
#define CODE_LM3S2950           (0x?? << 16)
#define CODE_LM3S2965           (0x55 << 16)
#define CODE_LM3S3739           (0x?? << 16)
#define CODE_LM3S3748           (0x49 << 16)
#define CODE_LM3S3768           (0x48 << 16)
#define CODE_LM3S5737           (0x?? << 16)
#define CODE_LM3S5739           (0x?? << 16)
#define CODE_LM3S5747           (0x?? << 16)
#define CODE_LM3S5749           (0x?? << 16)
#define CODE_LM3S5791           (0x?? << 16)
#define CODE_LM3S6100           (0x?? << 16)
#define CODE_LM3S6110           (0x?? << 16)
#define CODE_LM3S6420           (0x?? << 16)
#define CODE_LM3S6432           (0x?? << 16)
#define CODE_LM3S6537           (0x?? << 16)
#define CODE_LM3S6610           (0x?? << 16)
#define CODE_LM3S6611           (0x?? << 16)
#define CODE_LM3S6618           (0x?? << 16)
#define CODE_LM3S6633           (0x?? << 16)
#define CODE_LM3S6637           (0x?? << 16)
#define CODE_LM3S6730           (0x?? << 16)
#define CODE_LM3S6753           (0x?? << 16)
#define CODE_LM3S6911           (0xe9 << 16)                             // {20}
#define CODE_LM3S6918           (0x?? << 16)
#define CODE_LM3S6938           (0x89 << 16)
#define CODE_LM3S6950           (0x?? << 16)
#define CODE_LM3S6952           (0x?? << 16)
#define CODE_LM3S6965           (0x73 << 16)
#define CODE_LM3S8530           (0x?? << 16)
#define CODE_LM3S8538           (0x?? << 16)
#define CODE_LM3S8630           (0x?? << 16)
#define CODE_LM3S8730           (0x?? << 16)
#define CODE_LM3S8733           (0x?? << 16)
#define CODE_LM3S8738           (0x?? << 16)
#define CODE_LM3S8930           (0x?? << 16)
#define CODE_LM3S8933           (0x?? << 16)
#define CODE_LM3S8938           (0x?? << 16)
#define CODE_LM3S8962           (0xa6 << 16)
#define CODE_LM3S8970           (0x?? << 16)
#define CODE_LM3S8971           (0x?? << 16)
#define CODE_LM3S9790           (0x?? << 16)
#define CODE_LM3S9792           (0x?? << 16)

#define CODE_LM3S2B91           (0x?? << 16)
#define CODE_LM3S5B90           (0x?? << 16)
#define CODE_LM3S5B91           (0x?? << 16)

#define CODE_LM3S9B90           (0x66 << 16)
#define CODE_LM3S9B92           (0x6a << 16)
#define CODE_LM3S9B95           (0x6e << 16)
#define CODE_LM3S9B96           (0x6f << 16)

#define CODE_PIN_COUNT_28_PIN   (0x0  << 13)
#define CODE_PIN_COUNT_48_PIN   (0x1  << 13)
#define CODE_PIN_COUNT_64_PIN   (0x3  << 13)
#define CODE_PIN_COUNT_100_PIN  (0x2  << 13)
#define CODE_PIN_COUNT_108_PIN  (0x2  << 13)

#define CODE_PACKAGE_SOIC       (0x0  << 3) 
#define CODE_PACKAGE_LQFP       (0x1  << 3) 
#define CODE_PACKAGE_BGA        (0x2  << 3) 

#if PART_CODE == CODE_LM3S101                                            // this part doesn't have these registers but values are set to configure the project
    #define PART_DC0            (0x00000000)
    #define PART_DC1            (0x00000000)
    #define PART_DC2            (UART0_PRESENT2 | TIMER0_PRESENT2 | TIMER1_PRESENT2 | SSI0_PRESENT2)
    #define PART_DC3            (0x00000000)
    #define PART_DC4            (GPIOA_PRESENT4 | GPIOB_PRESENT4 | GPIOC_PRESENT4)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
    #define SIZE_OF_RAM         (2*1024)
    #define SIZE_OF_FLASH       (8*1024)
#elif PART_CODE == CODE_LM3S102                                          // this part doesn't have these registers but values are set to configure the project
    #define PART_DC0            (0x00000000)
    #define PART_DC1            (0x00000000)
    #define PART_DC2            (UART0_PRESENT2 | TIMER0_PRESENT2 | TIMER1_PRESENT2 | SSI0_PRESENT2 | I2C0_PRESENT2)
    #define PART_DC3            (0x00000000)
    #define PART_DC4            (GPIOA_PRESENT4 | GPIOB_PRESENT4 | GPIOC_PRESENT4)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
    #define SIZE_OF_RAM         (2*1024)
    #define SIZE_OF_FLASH       (8*1024)
#elif (PART_CODE == CODE_LM3S308) || (PART_CODE == CODE_LM3S608)
    #if (PART_CODE == CODE_LM3S608)
      #define PART_DC0          (0x001f000f)                             // 8k SRAM, 32k FLASH
      #define PART_DC1          (0x000132bf)                             // 50MHz
    #else
      #define PART_DC0          (0x000f0007)                             // 4k SRAM, 16k FLASH
      #define PART_DC1          (0x000172bf)                             // 25MHz
    #endif
    #define PART_DC2            (0x01071013)
    #define PART_DC3            (0xbfff00c0)
    #define PART_DC4            (0x0000001f)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S1958                                         // {21}
    #define PART_DC0            (0x00ff007f)                             // 256k FLASH, 64k SRAM
    #define PART_DC1            (0x000133ff)
    #define PART_DC2            (0x000f5037)
    #define PART_DC3            (0xbfff0000)
    #define PART_DC4            (0x0000c0ff)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S1968
    #define PART_DC0            (0x00ff007f)                             // 256k FLASH, 64k SRAM
    #define PART_DC1            (0x001133ff)
    #define PART_DC2            (0x070f5337)
    #define PART_DC3            (0x8fffb7ff)
    #define PART_DC4            (0x000000ff)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S2110
    #define PART_DC0            (0x003f001f)                             // 64k FLASH, 16k SRAM
    #define PART_DC1            (0x0110709f)
    #define PART_DC2            (0x07071011)
    #define PART_DC3            (0x8f00b7c3)
    #define PART_DC4            (0x000000ff)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S2139
    #define PART_DC0            (0x003f001f)                             // 64k FLASH, 16k SRAM
    #define PART_DC1            (0x010171bf)
    #define PART_DC2            (0x07071013)
    #define PART_DC3            (0xbf0f37c0)
    #define PART_DC4            (0x000000ff)
    #define PART_DC5            (0x00000040)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S2678
    #define PART_DC0            (0x007f003f)                             // 128k FLASH, 32k SRAM
    #define PART_DC1            (0x011132bf)
    #define PART_DC2            (0x000f0111)
    #define PART_DC3            (0x83ff800f)
    #define PART_DC4            (0x0000301f)
    #define PART_DC5            (0x0330000f)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000f00)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S2965
    #define PART_DC0            (0x00ff007f)                             // 256k FLASH, 64k SRAM
    #define PART_DC1            (0x031133ff)
    #define PART_DC2            (0x070f5337)
    #define PART_DC3            (0xbf0fb7ff)
    #define PART_DC4            (0x000000ff)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S5632 || PART_CODE == CODE_LM3S5732
    #if PART_CODE == CODE_LM3S5732
      #define PART_DC0          (0x00ff003f)                             // 128k FLASH, 64k SRAM
    #elif PART_CODE == CODE_LM3S5632
      #define PART_DC0          (0x007f003f)                             // 128k FLASH, 32k SRAM
    #endif
    #define PART_DC1            (0x010132ff)
    #define PART_DC2            (0x00075013)
    #define PART_DC3            (0x9f3f0000)
    #define PART_DC4            (0x0000301f)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000002)                             // USB Device/Host
    #define PART_DC7            (0x00c00f3f)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S3748
    #define PART_DC0            (0x00ff003f)                             // 128k FLASH, 64k SRAM
    #define PART_DC1            (0x001133ff)
    #define PART_DC2            (0x030f5133)
    #define PART_DC3            (0xbfff86ff)
    #define PART_DC4            (0x0000f0ff)
    #define PART_DC5            (0x0f3000ff)
    #define PART_DC6            (0x00000002)                             // USB Device/Host
    #define PART_DC7            (0x03c00f3f)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S3768
    #define PART_DC0            (0x00ff003f)                             // 128k FLASH, 64k SRAM
    #define PART_DC1            (0x001133ff)
    #define PART_DC2            (0x030f5133)
    #define PART_DC3            (0xbfff86ff)
    #define PART_DC4            (0x0000f0ff)
    #define PART_DC5            (0x0f3000ff)
    #define PART_DC6            (0x00000003)                             // USB OTG/Device/Host
    #define PART_DC7            (0x03c00f3f)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S6965
    #define PART_DC0            (0x00ff007f)                             // 256k FLASH, 64k SRAM
    #define PART_DC1            (0x001133ff)
    #define PART_DC2            (0x030f5317)
    #define PART_DC3            (0x8f0f87ff)
    #define PART_DC4            (0x5000007f)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S6911                                         // {20}
    #define PART_DC0            (0x00ff007f)                             // 256k FLASH, 64k SRAM
    #define PART_DC1            (0x000030df)
    #define PART_DC2            (0x030f5037)
    #define PART_DC3            (0xbf000fc0)
    #define PART_DC4            (0x5000007f)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S8962
    #define PART_DC0            (0x00ff007f)                             // 256k FLASH, 64k SRAM
    #define PART_DC1            (0x011132ff)
    #define PART_DC2            (0x010f1313)
    #define PART_DC3            (0x830f81ff)
    #define PART_DC4            (0x5100007f)
    #define PART_DC5            (0x00000040)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x00000000)
    #define PART_DC8            (0x00000000)
#elif PART_CODE == CODE_LM3S9B90                                         // {16}
    #define PART_DC0            (0x017f007f)                             // 256k FLASH, 96k SRAM
    #define PART_DC1            (0x13030fff)
    #define PART_DC2            (0x570f5037)
    #define PART_DC3            (0xbfff36c0)
    #define PART_DC4            (0x5004f1ff)
    #define PART_DC5            (0x00000000)
    #define PART_DC6            (0x00000013)
    #define PART_DC7            (0xffffffff)                             // 31 uDMA channels
    #define PART_DC8            (0xffffffff)                             // 16 ADC inputs
#elif PART_CODE == CODE_LM3S9B95
    #define PART_DC0            (0x017f007f)                             // 256k FLASH, 96k SRAM
    #define PART_DC1            (0x13130fbf)
    #define PART_DC2            (0x570f5337)
    #define PART_DC3            (0xbfffb6ff)
    #define PART_DC4            (0x5100f1ff)
    #define PART_DC5            (0x0f3000ff)
    #define PART_DC6            (0x00000013)
    #define PART_DC7            (0xffffffff)                             // 31 uDMA channels
    #define PART_DC8            (0xffffffff)                             // 16 ADC inputs
#elif PART_CODE == CODE_LM3S9B96                                         // {32}
    #define PART_DC0            (0x017f007f)                             // 256k FLASH, 96k SRAM
    #define PART_DC1            (0x13130fbf)
    #define PART_DC2            (0x570f5337)
    #define PART_DC3            (0xbfffffff)
    #define PART_DC4            (0x5100f1ff)
    #define PART_DC5            (0x0f3000ff)
    #define PART_DC6            (0x00000013)
    #define PART_DC7            (0xffffffff)                             // 31 uDMA channels
    #define PART_DC8            (0xffffffff)                             // 16 ADC inputs
#elif PART_CODE == CODE_LM3S1776                                         // {31}
    #define PART_DC0            (0x00ff003f)                             // 128k FLASH, 64k SRAM
    #define PART_DC1            (0x001133ff)
    #define PART_DC2            (0x00071011)
    #define PART_DC3            (0x853f803f)
    #define PART_DC4            (0x0000301f)
    #define PART_DC5            (0x073000ff)
    #define PART_DC6            (0x00000000)
    #define PART_DC7            (0x40000f00)
    #define PART_DC8            (0x00000000)
#endif


// SRAM size
//
#if (PART_DC0 & 0xffff0000)   == SRAM_SIZE_4K
    #define SIZE_OF_RAM         (4*1024)
#elif (PART_DC0 & 0xffff0000) == SRAM_SIZE_8K
    #define SIZE_OF_RAM         (8*1024)
#elif (PART_DC0 & 0xffff0000) == SRAM_SIZE_16K
    #define SIZE_OF_RAM         (16*1024)
#elif (PART_DC0 & 0xffff0000) == SRAM_SIZE_64K
    #define SIZE_OF_RAM         (64*1024)
#elif (PART_DC0 & 0xffff0000) == SRAM_SIZE_96K
    #define SIZE_OF_RAM         (96*1024)
#endif


// FLASH size
//
#if (PART_DC0 & 0x0000ffff)   == FLASH_SIZE_16K
    #define SIZE_OF_FLASH       (16*1024)
#elif (PART_DC0 & 0x0000ffff) == FLASH_SIZE_32K
    #define SIZE_OF_FLASH       (32*1024)
#elif (PART_DC0 & 0x0000ffff) == FLASH_SIZE_64K
    #define SIZE_OF_FLASH       (64*1024)
#elif (PART_DC0 & 0x0000ffff) == FLASH_SIZE_128K
    #define SIZE_OF_FLASH       (128*1024)
#elif (PART_DC0 & 0x0000ffff) == FLASH_SIZE_256K
    #define SIZE_OF_FLASH       (256*1024)
#endif


// CAN
//
#if PART_DC1 & CAN1_PRESENT1
    #define CHIP_HAS_CAN        2
#elif PART_DC1 & CAN0_PRESENT1
    #define CHIP_HAS_CAN        1
#endif

// I2C
//
#if PART_DC2 & I2C1_PRESENT2
    #define CHIP_HAS_I2C        2
#elif PART_DC2 & I2C0_PRESENT2
    #define CHIP_HAS_I2C        1
#endif
#define I2C_AVAILABLE           CHIP_HAS_I2C
#define LPI2C_AVAILABLE         0

// uDMA
//
#if PART_DC4 & UDMA_PRESENT4
    #define CHIP_HAS_uDMA
#endif

// JTAG debug
//
#if defined _LM3S6965 || defined _LM3S8962 || defined _LM3S1958 || defined _LM3S1968 || defined _LM3S10X || defined _LM3S2110 || defined _LM3S2139 // {11}
    #define DEVICE_HAS_JTAG_PB7
#endif

// UARTS
//
#if PART_DC2 & UART2_PRESENT2
    #define CHIP_HAS_UARTS      3
#elif PART_DC2 & UART1_PRESENT2
    #define CHIP_HAS_UARTS      2
#elif PART_DC2 & UART0_PRESENT2
    #define CHIP_HAS_UARTS      1
#endif
#define UARTS_AVAILABLE   CHIP_HAS_UARTS


// SPI (SSI)
//
#if PART_DC2 & SSI1_PRESENT2
    #define CHIP_HAS_SSI        2
#elif PART_DC2 & SSI0_PRESENT2
    #define CHIP_HAS_SSI        1
#endif

// Timers (32 bit)
//
#if PART_DC2 & TIMER3_PRESENT2
    #define CHIP_HAS_GP_TIMER   4
#elif PART_DC2 & TIMER2_PRESENT2
    #define CHIP_HAS_GP_TIMER   3
#elif PART_DC2 & TIMER1_PRESENT2
    #define CHIP_HAS_GP_TIMER   2
#elif PART_DC2 & TIMER0_PRESENT2
    #define CHIP_HAS_GP_TIMER   1
#endif


// Device capabilities - DC3
//
#define PWM0_PRESENT3             0x00000001
#define PWM1_PRESENT3             0x00000002
#define PWM2_PRESENT3             0x00000004
#define PWM3_PRESENT3             0x00000008
#define PWM4_PRESENT3             0x00000010
#define PWM5_PRESENT3             0x00000020

// PWM
//
#if PART_DC3 & PWM5_PRESENT3
    #define PWM_AVAILABLE    6
#elif PART_DC3 & PWM4_PRESENT3
    #define PWM_AVAILABLE    5
#elif PART_DC3 & PWM3_PRESENT3
    #define PWM_AVAILABLE    4
#elif PART_DC3 & PWM2_PRESENT3
    #define PWM_AVAILABLE    3
#elif PART_DC3 & PWM1_PRESENT3
    #define PWM_AVAILABLE    2
#elif PART_DC3 & PWM4_PRESENT3
    #define PWM_AVAILABLE    1
#endif


// Ports (GPIO)
//
#if PART_DC4 & GPIOJ_PRESENT4
    #define __PORTS_AVAILABLE 9
#elif PART_DC4 & GPIOH_PRESENT4
    #define __PORTS_AVAILABLE 8
#elif PART_DC4 & GPIOG_PRESENT4
    #define __PORTS_AVAILABLE 7
#elif PART_DC4 & GPIOF_PRESENT4
    #define __PORTS_AVAILABLE 6
#elif PART_DC4 & GPIOE_PRESENT4
    #define __PORTS_AVAILABLE 5
#elif PART_DC4 & GPIOD_PRESENT4
    #define __PORTS_AVAILABLE 4
#elif PART_DC4 & GPIOC_PRESENT4
    #define __PORTS_AVAILABLE 3
#elif PART_DC4 & GPIOB_PRESENT4
    #define __PORTS_AVAILABLE 2
#elif PART_DC4 & GPIOA_PRESENT4
    #define __PORTS_AVAILABLE 1
#endif

#if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS
    #define PORTS_AVAILABLE      __PORTS_AVAILABLE                       // ADC multiplexed with ports
#else
    #if PART_DC1 & ADC0_PRESENT1
        #define PORTS_AVAILABLE  (__PORTS_AVAILABLE + 1)                 // add ADC
    #else
        #define PORTS_AVAILABLE  __PORTS_AVAILABLE                       // no ADC
    #endif
#endif
/*****************************************************************************************************/

// Peripheral register blocks
#ifdef _WINDOWS
  #define WATCHDOG_BLOCK                ((unsigned char *)(&LM3Sxxxx.WATCHDOG))
  #define GP_TIMER_0_BLOCK              ((unsigned char *)(&LM3Sxxxx.GPT[0])) // {8}
  #define GP_TIMER_1_BLOCK              ((unsigned char *)(&LM3Sxxxx.GPT[1]))
  #define GP_TIMER_2_BLOCK              ((unsigned char *)(&LM3Sxxxx.GPT[2]))
  #define GP_TIMER_3_BLOCK              ((unsigned char *)(&LM3Sxxxx.GPT[3]))
  #define ADC_0_BLOCK                   ((unsigned char *)(&LM3Sxxxx.ADC[0])) // {10}
  #define ADC_1_BLOCK                   ((unsigned char *)(&LM3Sxxxx.ADC[1]))
  #define GPIO_PORT_A_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_A))
  #define GPIO_PORT_B_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_B))
  #define GPIO_PORT_C_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_C))
  #define GPIO_PORT_D_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_D))
  #define SSI0_BLOCK                    ((unsigned char *)(&LM3Sxxxx.SSI[0]))
  #define SSI1_BLOCK                    ((unsigned char *)(&LM3Sxxxx.SSI[1])) // {6}
  #define UART_0_BLOCK                  ((unsigned char *)(&LM3Sxxxx.UART[0]))
  #define UART_1_BLOCK                  ((unsigned char *)(&LM3Sxxxx.UART[1]))
  #define UART_2_BLOCK                  ((unsigned char *)(&LM3Sxxxx.UART[2]))
  #define I2C0_BLOCK                    ((unsigned char *)(&LM3Sxxxx.I2C[0]))
  #define I2C1_BLOCK                    ((unsigned char *)(&LM3Sxxxx.I2C[1]))
  #define GPIO_PORT_E_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_E))
  #define GPIO_PORT_F_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_F))
  #define GPIO_PORT_G_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_G))
  #define GPIO_PORT_H_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_H))
  #define GPIO_PORT_J_BLOCK             ((unsigned char *)(&LM3Sxxxx.GPIO_PORT_J))

  #define PWM_BLOCK                     ((unsigned char *)(&LM3Sxxxx.PWM)) // {30}

  #define CAN_BLOCK                     ((unsigned char *)(&LM3Sxxxx.CAN))
  #define ETHERNET_BLOCK                ((unsigned char *)(&LM3Sxxxx.ETHERNET))

  #define USB_BLOCK                     ((unsigned char *)(&LM3Sxxxx.USB)) // {3}

  #define FLASH_CONTROL_BLOCK           ((unsigned char *)(&LM3Sxxxx.FLASH_CONTROL))
  #define SYSTEM_CONTROL_BLOCK          ((unsigned char *)(&LM3Sxxxx.SYSTEM_CONTROL))
  #define uDMA_BLOCK                    ((unsigned char *)(&LM3Sxxxx.uDMA))// {4}

  #define CORTEX_M3_BLOCK               ((unsigned char *)(&LM3Sxxxx.CORTEX_M3))
#else
  #define WATCHDOG_BLOCK                0x40000000
  #define GP_TIMER_0_BLOCK              0x40030000                       // {8}
  #define GP_TIMER_1_BLOCK              0x40031000
  #define GP_TIMER_2_BLOCK              0x40032000
  #define GP_TIMER_3_BLOCK              0x40033000
  #define ADC_0_BLOCK                   0x40038000                       // {10}
  #define ADC_1_BLOCK                   0x40039000
  #if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
      #define GPIO_PORT_A_BLOCK         0x40058000                       // AHB aperture - faster for devices supporting it
      #define GPIO_PORT_B_BLOCK         0x40059000
      #define GPIO_PORT_C_BLOCK         0x4005a000
      #define GPIO_PORT_D_BLOCK         0x4005b000
  #else
      #define GPIO_PORT_A_BLOCK         0x40004000                       // APB aperture
      #define GPIO_PORT_B_BLOCK         0x40005000
      #define GPIO_PORT_C_BLOCK         0x40006000
      #define GPIO_PORT_D_BLOCK         0x40007000
  #endif
  #define SSI0_BLOCK                    0x40008000
  #define SSI1_BLOCK                    0x40009000                       // {6}
  #define UART_0_BLOCK                  0x4000c000
  #define UART_1_BLOCK                  0x4000d000
  #define UART_2_BLOCK                  0x4000e000
  #define I2C0_BLOCK                    0x40020000
  #define I2C1_BLOCK                    0x40021000
  #if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
      #define GPIO_PORT_E_BLOCK         0x4005c000                       // AHB aperture - faster for devices supporting it
      #define GPIO_PORT_F_BLOCK         0x4005d000
      #define GPIO_PORT_G_BLOCK         0x4005e000
      #define GPIO_PORT_H_BLOCK         0x4005f000
      #define GPIO_PORT_J_BLOCK         0x40060000
  #else
      #define GPIO_PORT_E_BLOCK         0x40024000                       // APB aperture
      #define GPIO_PORT_F_BLOCK         0x40025000
      #define GPIO_PORT_G_BLOCK         0x40026000
      #define GPIO_PORT_H_BLOCK         0x40027000
      #define GPIO_PORT_J_BLOCK         0x4003d000
  #endif

  #define PWM_BLOCK                     0x40028000                       // {30}

  #define CAN_BLOCK                     0x40040000
  #define ETHERNET_BLOCK                0x40048000

  #define USB_BLOCK                     0x40050000                       // {3}

  #define FLASH_CONTROL_BLOCK           0x400fd000
  #define SYSTEM_CONTROL_BLOCK          0x400fe000
  #define uDMA_BLOCK                    0x400ff000                       // {4}

  #define CORTEX_M3_BLOCK               0xe000e000
#endif



typedef struct stRESET_VECTOR
{
    void  *ptrResetSP;                                                   // initial stack pointer
    void  (*ptrResetPC)(void);                                           // initial program counter
} RESET_VECTOR;

// LM3S6965 interrupts
//
typedef struct stPROCESSOR_IRQ
{
    void  (*irq_GPIO_Port_A)(void);                                      // 0
    void  (*irq_GPIO_Port_B)(void);                                      // 1
    void  (*irq_GPIO_Port_C)(void);                                      // 2
    void  (*irq_GPIO_Port_D)(void);                                      // 3
    void  (*irq_GPIO_Port_E)(void);                                      // 4
    void  (*irq_UART0)(void);                                            // 5
    void  (*irq_UART1)(void);                                            // 6
    void  (*irq_SSI0)(void);                                             // 7
    void  (*irq_I2C0)(void);                                             // 8
    void  (*irq_PWM_fault)(void);                                        // 9
    void  (*irq_PWM_Generator_0)(void);                                  // 10
    void  (*irq_PWM_Generator_1)(void);                                  // 11
    void  (*irq_PWM_Generator_2)(void);                                  // 12
    void  (*irq_QEI0)(void);                                             // 13
    void  (*irq_ADC0_Sequence_0)(void);                                  // 14
    void  (*irq_ADC0_Sequence_1)(void);                                  // 15
    void  (*irq_ADC0_Sequence_2)(void);                                  // 16
    void  (*irq_ADC0_Sequence_3)(void);                                  // 17
    void  (*irq_Watchdog_Timer)(void);                                   // 18
    void  (*irq_Timer0_A)(void);                                         // 19
    void  (*irq_Timer0_B)(void);                                         // 20
    void  (*irq_Timer1_A)(void);                                         // 21
    void  (*irq_Timer1_B)(void);                                         // 22
    void  (*irq_Timer2_A)(void);                                         // 23
    void  (*irq_Timer2_B)(void);                                         // 24
    void  (*irq_Analog_Comparator_0)(void);                              // 25
    void  (*irq_Analog_Comparator_1)(void);                              // 26
    void  (*irq_Analog_Comparator_2)(void);                              // 27
    void  (*irq_System_Control)(void);                                   // 28
    void  (*irq_FLASH_Control)(void);                                    // 29
    void  (*irq_GPIO_Port_F)(void);                                      // 30
    void  (*irq_GPIO_Port_G)(void);                                      // 31
    void  (*irq_GPIO_Port_H)(void);                                      // 32 {1}
    void  (*irq_UART2)(void);                                            // 33
    void  (*irq_SSI1)(void);                                             // 34
    void  (*irq_Timer3_A)(void);                                         // 35
    void  (*irq_Timer3_B)(void);                                         // 36
    void  (*irq_I2C1)(void);                                             // 37
    void  (*irq_QEI1)(void);                                             // 38
    void  (*irq_CAN0)(void);                                             // 39
    void  (*irq_CAN1)(void);                                             // 40
    void  *irq_Reserved_1[1];                                            // 41
    void  (*irq_Ethernet_Controller)(void);                              // 42
    void  (*irq_Hibernation_Module)(void);                               // 43
    void  (*irq_USB_Controller)(void);                                   // 44 {1}
    void  (*irq_PWM_Generator_3)(void);                                  // 45
    void  (*irq_uDMA_Software)(void);                                    // 46
    void  (*irq_uDMA_Error)(void);                                       // 47
    void  (*irq_ADC1_Sequence_0)(void);                                  // 48
    void  (*irq_ADC1_Sequence_1)(void);                                  // 49
    void  (*irq_ADC1_Sequence_2)(void);                                  // 50
    void  (*irq_ADC1_Sequence_3)(void);                                  // 51
    void  (*irq_I2S0)(void);                                             // 52
    void  (*irq_EPI)(void);                                              // 53
    void  (*irq_GPIO_Port_J)(void);                                      // 54   
} PROCESSOR_IRQ;

#define LAST_PROCESSOR_IRQ  irq_Hibernation_Module


typedef struct stVECTOR_TABLE
{
    RESET_VECTOR  reset_vect;
    void  (*ptrNMI)(void);
    void  (*ptrHardFault)(void);
    void  (*ptrMemManagement)(void);
    void  (*ptrBusFault)(void);
    void  (*ptrUsageFault)(void);
    void  *ptrReserved1[4];
    void  (*ptrSVCall)(void);
    void  (*ptrDebugMonitor)(void);
    void  *ptrReserved2;
    void  (*ptrPendSV)(void);
    void  (*ptrSysTick)(void);
    PROCESSOR_IRQ processor_interrupts;                                  // length is processor specific
} VECTOR_TABLE;

#define VECTOR_SIZE                      (sizeof(VECTOR_TABLE))
#define CHECK_VECTOR_SIZE                284                             // ((16 + 54 + 1) = 71)*4 - adequate for this processor



// Watchdog
//
#define WDTLOAD                          *(unsigned long*)(WATCHDOG_BLOCK + 0x000) // Watchdog load register
#define WDTVALUE                         *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x004) // Watchdog counter current value
#define WDTCTL                           *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x008) // Watchdog Control Register
  #define WD_RESEN                       0x00000002                                // enable reset on timeout
  #define WD_INTEN                       0x00000001                                // enable interrupt on timeout
#define WDTICR                           *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x00c) // Watchdog Interrupt Clear Register (write only)
#define WDTRIS                           *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x010) // Watchdog Raw Interrupt Status (read only)
  #define WTDRIS                         0x00000001                                // watchdog interrupt
#define WDTMIS                           *(volatile unsigned long*)(WATCHDOG_BLOCK + 0x014) // Watchdog Masked Interrupt Status (read only)
  #define WTDMIS                         0x00000001                                // Watchdog interrupt
#define WDTTEST                          *(unsigned long*)(WATCHDOG_BLOCK + 0x418) // Watchdog Test Register
  #define WD_STALL                       0x00000100                                // Watchdog Stall Enable
#define WDTLOCK                          *(volatile unsigned long*)(WATCHDOG_BLOCK + 0xc00) // Watchdog Lock Register
  #define WD_LOCKED                      0x00000001                                // read only
  #define WD_UNLOCKED                    0x00000000                                // read only
  #define WD_ACCESS                      0x1acce551                                // enable write access
#define WDTPeriphID4                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfd0) // Watchdog Peripheral Identication 4 [7..0]
#define WDTPeriphID5                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfd4) // Watchdog Peripheral Identication 5 [15..8]
#define WDTPeriphID6                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfd8) // Watchdog Peripheral Identication 6 [23..16]
#define WDTPeriphID7                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfdc) // Watchdog Peripheral Identication 7 [31..24]
#define WDTPeriphID0                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfe0) // Watchdog Peripheral Identication 0 [7..0]
#define WDTPeriphID1                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfe4) // Watchdog Peripheral Identication 1 [15..8]
#define WDTPeriphID2                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfe8) // Watchdog Peripheral Identication 2 [23..16]
#define WDTPeriphID3                     *(unsigned long*)(WATCHDOG_BLOCK + 0xfec) // Watchdog Peripheral Identication 3 [31..24]
#define WDTPCellID0                      *(unsigned long*)(WATCHDOG_BLOCK + 0xff0) // Watchdog PrimeCell Identication 0 [7..0]
#define WDTPCellID1                      *(unsigned long*)(WATCHDOG_BLOCK + 0xff4) // Watchdog PrimeCell Identication 1 [15..8]
#define WDTPCellID2                      *(unsigned long*)(WATCHDOG_BLOCK + 0xff8) // Watchdog PrimeCell Identication 2 [23..16]
#define WDTPCellID3                      *(unsigned long*)(WATCHDOG_BLOCK + 0xffc) // Watchdog PrimeCell Identication 3 [31..24]


// General Purpose Timers                                                // {8}
//
#define GPTMCFG_0                        *(unsigned long*)(GP_TIMER_0_BLOCK + 0x000) // GPTM 0 Configuration
  #define GPTMCFG_32_BIT_MODE            0x00000000
  #define GPTMCFG_32_BIT_RTC_MODE        0x00000001
  #define GPTMCFG_16_BIT_MODE            0x00000004
#define GPTMTAMR_0                       *(unsigned long*)(GP_TIMER_0_BLOCK + 0x004) // GPTM 0 TimerA Mode
  #define TMR_ONE_SHOT                   0x00000001
  #define TMR_PERIODIC                   0x00000002
  #define TMR_CAPTURE                    0x00000003
  #define TCMR_EDGE_COUNT                0x00000000
  #define TCMR_EDGE_TIME                 0x00000004
  #define TAMS_CAPTURE_MODE              0x00000000
  #define TAMS_PWM_MODE                  0x00000008
#define GPTMTBMR_0                       *(unsigned long*)(GP_TIMER_0_BLOCK + 0x008) // GPTM 0 TimerB Mode
#define GPTMCTL_0                        *(unsigned long*)(GP_TIMER_0_BLOCK + 0x00c) // GPTM 0 Control
  #define TAEN                           0x00000001                      // Enable TimerA
  #define TASTALL                        0x00000002                      // Enable TimerA Stall
  #define TAEVENT_POS_EDGE               0x00000000                      // Events on positive edges
  #define TAEVENT_NEG_EDGE               0x00000004                      // Events on negative edges
  #define TAEVENT_BOTH_EDGE              0x0000000c                      // Events on both edges
  #define RTC_EV                         0x00000010                      // RTC counting enables
  #define TAOTE                          0x00000020                      // TimerA ADC trigger enabled
  #define TAPWML                         0x00000040                      // PWM Output Level Inverted
  #define TBEN                           0x00000100                      // Enable TimerB
  #define TBSTALL                        0x00000200                      // Enable TimerB Stall
  #define TBEVENT_POS_EDGE               0x00000000                      // Events on positive edges
  #define TBEVENT_NEG_EDGE               0x00000400                      // Events on negative edges
  #define TBEVENT_BOTH_EDGE              0x00000c00                      // Events on both edges
  #define TBOTE                          0x00002000                      // TimerB ADC trigger enabled
  #define TBPWML                         0x00004000                      // PWM Output Level Inverted
#define GPTMIMR_0                        *(unsigned long*)(GP_TIMER_0_BLOCK + 0x018) // GPTM 0 Interrupt Mask
  #define TATOIM                         0x00000001                      // TimerA Timeout Interrupt Mask
  #define CAMIM                          0x00000002                      // CaptureA Match Interrupt Mask
  #define CAEIM                          0x00000004                      // CaptureA Event Interrupt Mask
  #define RTCIM                          0x00000008                      // RTC Interrupt Mask
  #define TBTOIM                         0x00000100                      // TimerB Timeout Interrupt Mask
  #define CBMIM                          0x00000200                      // CaptureB Match Interrupt Mask
  #define CBEIM                          0x00000400                      // CaptureB Event Interrupt Mask
#define GPTMRIS_0                        *(volatile unsigned long*)(GP_TIMER_0_BLOCK + 0x01c) // GPTM 0 Raw Interrupt Status
  #define TATORIS                        0x00000001                      // TimerA Timeout Raw Interrupt
  #define CAMRIS                         0x00000002                      // CaptureA Match Raw Interrupt
  #define CAERIS                         0x00000004                      // CaptureA Event Raw Interrupt
  #define RTCRIS                         0x00000008                      // RTC Raw Interrupt
  #define TBTORIS                        0x00000100                      // TimerB Timeout Raw Interrupt
  #define CBMRIS                         0x00000200                      // CaptureB Match Raw Interrupt
  #define CBERIS                         0x00000400                      // CaptureB Event Raw Interrupt
#define GPTMMIS_0                        *(volatile unsigned long*)(GP_TIMER_0_BLOCK + 0x020) // GPTM 0 Masked Interrupt Status
  #define TATOMIS                        0x00000001                      // TimerA Timeout Masked Interrupt
  #define CAMMIS                         0x00000002                      // CaptureA Match Masked Interrupt
  #define CAEMIS                         0x00000004                      // CaptureA Event Masked Interrupt
  #define RTCMIS                         0x00000008                      // RTC Masked Interrupt
  #define TBTOMIS                        0x00000100                      // TimerB Timeout Masked Interrupt
  #define CBMMIS                         0x00000200                      // CaptureB Match Masked Interrupt
  #define CBEMIS                         0x00000400                      // CaptureB Event Masked Interrupt
#define GPTMICR_0                        *(volatile unsigned long*)(GP_TIMER_0_BLOCK + 0x024) // GPTM 0 Interrupt Clear
  #define TATOCINT                       0x00000001                      // TimerA Timeout Interrupt Clear
  #define CAMCINT                        0x00000002                      // CaptureA Match Interrupt Clear
  #define CAECINT                        0x00000004                      // CaptureA Event Interrupt Clear
  #define RTCCINT                        0x00000008                      // RTC Interrupt Clear
  #define TBTOCINT                       0x00000100                      // TimerB Timeout Interrupt Clear
  #define CBMCINT                        0x00000200                      // CaptureB Match Interrupt Clear
  #define CBECINT                        0x00000400                      // CaptureB Event Interrupt Clear
#define GPTMTAILR_0                      *(unsigned long*)(GP_TIMER_0_BLOCK + 0x028) // GPTM 0 TimerA Interval Load
#define GPTMTBILR_0                      *(unsigned long*)(GP_TIMER_0_BLOCK + 0x02c) // GPTM 0 TimerB Interval Load
#define GPTMTAMATCHR_0                   *(unsigned long*)(GP_TIMER_0_BLOCK + 0x030) // GPTM 0 TimerA Match
#define GPTMTBMATCHR_0                   *(unsigned long*)(GP_TIMER_0_BLOCK + 0x034) // GPTM 0 TimerB Match
#define GPTMTAPR_0                       *(unsigned long*)(GP_TIMER_0_BLOCK + 0x038) // GPTM 0 TimerA Prescale
#define GPTMTBPR_0                       *(unsigned long*)(GP_TIMER_0_BLOCK + 0x03c) // GPTM 0 TimerB Prescale
#define GPTMTAPMR_0                      *(unsigned long*)(GP_TIMER_0_BLOCK + 0x040) // GPTM 0 TimerA Prescale Match
#define GPTMTBPMR_0                      *(unsigned long*)(GP_TIMER_0_BLOCK + 0x044) // GPTM 0 TimerB Prescale Match
#define GPTMTAR_0                        *(volatile unsigned long*)(GP_TIMER_0_BLOCK + 0x048) // GPTM 0 TimerA
#define GPTMTBR_0                        *(volatile unsigned long*)(GP_TIMER_0_BLOCK + 0x04c) // GPTM 0 TimerB

#define GPTMCFG_1                        *(unsigned long*)(GP_TIMER_1_BLOCK + 0x000) // GPTM 1 Configuration
#define GPTMTAMR_1                       *(unsigned long*)(GP_TIMER_1_BLOCK + 0x004) // GPTM 1 TimerA Mode
#define GPTMTBMR_1                       *(unsigned long*)(GP_TIMER_1_BLOCK + 0x008) // GPTM 1 TimerB Mode
#define GPTMCTL_1                        *(unsigned long*)(GP_TIMER_1_BLOCK + 0x00c) // GPTM 1 Control
#define GPTMIMR_1                        *(unsigned long*)(GP_TIMER_1_BLOCK + 0x018) // GPTM 1 Interrupt Mask
#define GPTMRIS_1                        *(volatile unsigned long*)(GP_TIMER_1_BLOCK + 0x01c) // GPTM 1 Raw Interrupt Status
#define GPTMMIS_1                        *(volatile unsigned long*)(GP_TIMER_1_BLOCK + 0x020) // GPTM 1 Masked Interrupt Status
#define GPTMICR_1                        *(volatile unsigned long*)(GP_TIMER_1_BLOCK + 0x024) // GPTM 1 Interrupt Clear
#define GPTMTAILR_1                      *(unsigned long*)(GP_TIMER_1_BLOCK + 0x028) // GPTM 1 TimerA Interval Load
#define GPTMTBILR_1                      *(unsigned long*)(GP_TIMER_1_BLOCK + 0x02c) // GPTM 1 TimerB Interval Load
#define GPTMTAMATCHR_1                   *(unsigned long*)(GP_TIMER_1_BLOCK + 0x030) // GPTM 1 TimerA Match
#define GPTMTBMATCHR_1                   *(unsigned long*)(GP_TIMER_1_BLOCK + 0x034) // GPTM 1 TimerB Match
#define GPTMTAPR_1                       *(unsigned long*)(GP_TIMER_1_BLOCK + 0x038) // GPTM 1 TimerA Prescale
#define GPTMTBPR_1                       *(unsigned long*)(GP_TIMER_1_BLOCK + 0x03c) // GPTM 1 TimerB Prescale
#define GPTMTAPMR_1                      *(unsigned long*)(GP_TIMER_1_BLOCK + 0x040) // GPTM 1 TimerA Prescale Match
#define GPTMTBPMR_1                      *(unsigned long*)(GP_TIMER_1_BLOCK + 0x044) // GPTM 1 TimerB Prescale Match
#define GPTMTAR_1                        *(volatile unsigned long*)(GP_TIMER_1_BLOCK + 0x048) // GPTM 1 TimerA
#define GPTMTBR_1                        *(volatile unsigned long*)(GP_TIMER_1_BLOCK + 0x04c) // GPTM 1 TimerB

#define GPTMCFG_2                        *(unsigned long*)(GP_TIMER_2_BLOCK + 0x000) // GPTM 2 Configuration
#define GPTMTAMR_2                       *(unsigned long*)(GP_TIMER_2_BLOCK + 0x004) // GPTM 2 TimerA Mode
#define GPTMTBMR_2                       *(unsigned long*)(GP_TIMER_2_BLOCK + 0x008) // GPTM 2 TimerB Mode
#define GPTMCTL_2                        *(unsigned long*)(GP_TIMER_2_BLOCK + 0x00c) // GPTM 2 Control
#define GPTMIMR_2                        *(unsigned long*)(GP_TIMER_2_BLOCK + 0x018) // GPTM 2 Interrupt Mask
#define GPTMRIS_2                        *(volatile unsigned long*)(GP_TIMER_2_BLOCK + 0x01c) // GPTM 2 Raw Interrupt Status
#define GPTMMIS_2                        *(volatile unsigned long*)(GP_TIMER_2_BLOCK + 0x020) // GPTM 2 Masked Interrupt Status
#define GPTMICR_2                        *(volatile unsigned long*)(GP_TIMER_2_BLOCK + 0x024) // GPTM 2 Interrupt Clear
#define GPTMTAILR_2                      *(unsigned long*)(GP_TIMER_2_BLOCK + 0x028) // GPTM 2 TimerA Interval Load
#define GPTMTBILR_2                      *(unsigned long*)(GP_TIMER_2_BLOCK + 0x02c) // GPTM 2 TimerB Interval Load
#define GPTMTAMATCHR_2                   *(unsigned long*)(GP_TIMER_2_BLOCK + 0x030) // GPTM 2 TimerA Match
#define GPTMTBMATCHR_2                   *(unsigned long*)(GP_TIMER_2_BLOCK + 0x034) // GPTM 2 TimerB Match
#define GPTMTAPR_2                       *(unsigned long*)(GP_TIMER_2_BLOCK + 0x038) // GPTM 2 TimerA Prescale
#define GPTMTBPR_2                       *(unsigned long*)(GP_TIMER_2_BLOCK + 0x03c) // GPTM 2 TimerB Prescale
#define GPTMTAPMR_2                      *(unsigned long*)(GP_TIMER_2_BLOCK + 0x040) // GPTM 2 TimerA Prescale Match
#define GPTMTBPMR_2                      *(unsigned long*)(GP_TIMER_2_BLOCK + 0x044) // GPTM 2 TimerB Prescale Match
#define GPTMTAR_2                        *(volatile unsigned long*)(GP_TIMER_2_BLOCK + 0x048) // GPTM 2 TimerA
#define GPTMTBR_2                        *(volatile unsigned long*)(GP_TIMER_2_BLOCK + 0x04c) // GPTM 2 TimerB

#define GPTMCFG_3                        *(unsigned long*)(GP_TIMER_3_BLOCK + 0x000) // GPTM 3 Configuration
#define GPTMTAMR_3                       *(unsigned long*)(GP_TIMER_3_BLOCK + 0x004) // GPTM 3 TimerA Mode
#define GPTMTBMR_3                       *(unsigned long*)(GP_TIMER_3_BLOCK + 0x008) // GPTM 3 TimerB Mode
#define GPTMCTL_3                        *(unsigned long*)(GP_TIMER_3_BLOCK + 0x00c) // GPTM 3 Control
#define GPTMIMR_3                        *(unsigned long*)(GP_TIMER_3_BLOCK + 0x018) // GPTM 3 Interrupt Mask
#define GPTMRIS_3                        *(volatile unsigned long*)(GP_TIMER_3_BLOCK + 0x01c) // GPTM 3 Raw Interrupt Status
#define GPTMMIS_3                        *(volatile unsigned long*)(GP_TIMER_3_BLOCK + 0x020) // GPTM 3 Masked Interrupt Status
#define GPTMICR_3                        *(volatile unsigned long*)(GP_TIMER_3_BLOCK + 0x024) // GPTM 3 Interrupt Clear
#define GPTMTAILR_3                      *(unsigned long*)(GP_TIMER_3_BLOCK + 0x028) // GPTM 3 TimerA Interval Load
#define GPTMTBILR_3                      *(unsigned long*)(GP_TIMER_3_BLOCK + 0x02c) // GPTM 3 TimerB Interval Load
#define GPTMTAMATCHR_3                   *(unsigned long*)(GP_TIMER_3_BLOCK + 0x030) // GPTM 3 TimerA Match
#define GPTMTBMATCHR_3                   *(unsigned long*)(GP_TIMER_3_BLOCK + 0x034) // GPTM 3 TimerB Match
#define GPTMTAPR_3                       *(unsigned long*)(GP_TIMER_3_BLOCK + 0x038) // GPTM 3 TimerA Prescale
#define GPTMTBPR_3                       *(unsigned long*)(GP_TIMER_3_BLOCK + 0x03c) // GPTM 3 TimerB Prescale
#define GPTMTAPMR_3                      *(unsigned long*)(GP_TIMER_3_BLOCK + 0x040) // GPTM 3 TimerA Prescale Match
#define GPTMTBPMR_3                      *(unsigned long*)(GP_TIMER_3_BLOCK + 0x044) // GPTM 3 TimerB Prescale Match
#define GPTMTAR_3                        *(volatile unsigned long*)(GP_TIMER_3_BLOCK + 0x048) // GPTM 3 TimerA
#define GPTMTBR_3                        *(volatile unsigned long*)(GP_TIMER_3_BLOCK + 0x04c) // GPTM 3 TimerB

typedef struct stLM3XXXXX_GPT_CONTROL
{
unsigned long GPTMCFG;        
unsigned long GPTMTAMR;       
unsigned long GPTMTBMR;       
unsigned long GPTMCTL;        
unsigned long ulRes1[2];
unsigned long GPTMIMR;        
volatile unsigned long GPTMRIS;        
volatile unsigned long GPTMMIS;        
volatile unsigned long _GPTMICR;        
unsigned long GPTMTAILR;      
unsigned long GPTMTBILR;      
unsigned long GPTMTAMATCHR;   
unsigned long GPTMTBMATCHR;   
unsigned long GPTMTAPR;       
unsigned long GPTMTBPR;       
unsigned long GPTMTAPMR;      
unsigned long GPTMTBPMR;      
volatile unsigned long GPTMTAR;        
volatile unsigned long GPTMTBR;
unsigned long ulRes2[1024 - 20];                                         // pack to 4k boundary
} LM3XXXXX_GPT_CONTROL;

// ADC                                                                   // {10}
//
#if PART_DC1 & ADC0_PRESENT1
#define ADCACTSS_0                       *(unsigned long*)(ADC_0_BLOCK + 0x000) // ADC Active Sample Sequencer
  #define ASEN0                          0x00000001
  #define ASEN1                          0x00000002
  #define ASEN2                          0x00000004
  #define ASEN3                          0x00000008
#define ADCRIS_0                         *(volatile unsigned long*)(ADC_0_BLOCK + 0x004) // ADC Raw Interrupt Status (read-only)
#define ADCIM_0                          *(unsigned long*)(ADC_0_BLOCK + 0x008) // ADC Interrupt Mask
  #define MASK_SS0                       0x00000001
  #define MASK_SS1                       0x00000002
  #define MASK_SS2                       0x00000004
  #define MASK_SS3                       0x00000008
#define ADCISC_0                         *(volatile unsigned long*)(ADC_0_BLOCK + 0x00c) // ADC Interrupt Status and Clear (write 1 to clear)
  #define INT_SS0                        0x00000001
  #define INT_SS1                        0x00000002
  #define INT_SS2                        0x00000004
  #define INT_SS3                        0x00000008
#define ADCOSTAT_0                       *(volatile unsigned long*)(ADC_0_BLOCK + 0x010) // ADC Overflow Status (write 1 to clear)
#define ADCEMUX_0                        *(unsigned long*)(ADC_0_BLOCK + 0x014) // ADC Event Multiplexer Select
  #define SS0_MASK                       0x0000000f
  #define SS1_MASK                       0x000000f0
  #define SS2_MASK                       0x00000f00
  #define SS3_MASK                       0x0000f000
  #define SS0_SHIFT                      0
  #define SS1_SHIFT                      4
  #define SS2_SHIFT                      8
  #define SS3_SHIFT                      12
  #define TRIG_CONTROLLER                0
  #define TRIG_AN_COMP_0                 1
  #define TRIG_AN_COMP_1                 2
  #define TRIG_AN_COMP_2                 3
  #define TRIG_GPIO_PB4                  4
  #define TRIG_TIMER                     5
  #define TRIG_PMW_0                     6
  #define TRIG_PMW_1                     7
  #define TRIG_PMW_2                     8
  #define TRIG_PMW_3                     9
  #define TRIG_FREERUN                   15
#define ADCUSTAT_0                       *(volatile unsigned long*)(ADC_0_BLOCK + 0x018) // ADC Underflow Status (write 1 to clear)
#define ADCSSPRI_0                       *(unsigned long*)(ADC_0_BLOCK + 0x020) // ADC Sample Sequencer Priority
  #define SS0_PRIORITY_MASK              0x00000003
  #define SS1_PRIORITY_MASK              0x00000030
  #define SS2_PRIORITY_MASK              0x00000300
  #define SS3_PRIORITY_MASK              0x00003000
  #define SS0_PRIORITY_0                 0x00000000                      // highest priority (sequencer piorities shoudl always be unique)
  #define SS0_PRIORITY_1                 0x00000001
  #define SS0_PRIORITY_2                 0x00000002
  #define SS0_PRIORITY_3                 0x00000003                      // lower priority
  #define SS1_PRIORITY_0                 0x00000000                      // highest priority (sequencer piorities shoudl always be unique)
  #define SS1_PRIORITY_1                 0x00000010
  #define SS1_PRIORITY_2                 0x00000020
  #define SS1_PRIORITY_3                 0x00000030                      // lower priority
  #define SS2_PRIORITY_0                 0x00000000                      // highest priority (sequencer piorities shoudl always be unique)
  #define SS2_PRIORITY_1                 0x00000100
  #define SS2_PRIORITY_2                 0x00000200
  #define SS2_PRIORITY_3                 0x00000300                      // lower priority
  #define SS3_PRIORITY_0                 0x00000000                      // highest priority (sequencer piorities shoudl always be unique)
  #define SS3_PRIORITY_1                 0x00001000
  #define SS3_PRIORITY_2                 0x00002000
  #define SS3_PRIORITY_3                 0x00003000                      // lower priority
#define ADCPSSI_0                        *(volatile unsigned long*)(ADC_0_BLOCK + 0x028) // ADC Processor Sample Sequence Initiate (write-only - read also for GSYNC and SYNCWAIT on advanced devices)
  #define INITIATE_SS0                   0x00000001
  #define INITIATE_SS1                   0x00000002
  #define INITIATE_SS2                   0x00000004
  #define INITIATE_SS3                   0x00000008
  #if defined _TEMPEST_CLASS
    #define SYNCWAIT                     0x08000000
    #define GSYNC                        0x80000000
  #endif
#define ADCSAC_0                         *(unsigned long*)(ADC_0_BLOCK + 0x030) // ADC Sample Averaging Control
  #define NO_HW_AVERAGING                0x00000000
  #define HW_AVERAGING_2                 0x00000001
  #define HW_AVERAGING_4                 0x00000002
  #define HW_AVERAGING_8                 0x00000003
  #define HW_AVERAGING_16                0x00000004
  #define HW_AVERAGING_32                0x00000005
  #define HW_AVERAGING_64                0x00000006
  #if defined _TEMPEST_CLASS
    #define ADCDCISC_0                   *(volatile unsigned long*)(ADC_0_BLOCK + 0x034) // ADC Digital Comparator Interrupt Status and Clear (write 1 to clear)
    #define ADCCTL_0                     *(unsigned long*)(ADC_0_BLOCK + 0x038) // ADC Control
      #define ADC_EXTERN_VREFA           0x00000001
  #endif
#define ADCSSMUX0_0                      *(unsigned long*)(ADC_0_BLOCK + 0x040) // ADC Sample Input Sequence Multiplexer Select 0
#define ADCSSCTL0_0                      *(unsigned long*)(ADC_0_BLOCK + 0x044) // ADC Sample Sequence Control 0
  #define SINGLE_END_SAMPLE_1            0x00000000
  #define DIF_SAMPLE_1                   0x00000001
  #define END_SAMPLE_1                   0x00000002
  #define INT_SAMPLE_1                   0x00000004
  #define TEMP_SAMPLE_1                  0x00000008
  #define SINGLE_END_SAMPLE_2            0x00000000
  #define DIF_SAMPLE_2                   0x00000010
  #define END_SAMPLE_2                   0x00000020
  #define INT_SAMPLE_2                   0x00000040
  #define TEMP_SAMPLE_2                  0x00000080
  #define SINGLE_END_SAMPLE_3            0x00000000
  #define DIF_SAMPLE_3                   0x00000100
  #define END_SAMPLE_3                   0x00000200
  #define INT_SAMPLE_3                   0x00000400
  #define TEMP_SAMPLE_3                  0x00000800
  #define SINGLE_END_SAMPLE_4            0x00000000
  #define DIF_SAMPLE_4                   0x00001000
  #define END_SAMPLE_4                   0x00002000
  #define INT_SAMPLE_4                   0x00004000
  #define TEMP_SAMPLE_4                  0x00008000
  #define SINGLE_END_SAMPLE_5            0x00000000
  #define DIF_SAMPLE_5                   0x00010000
  #define END_SAMPLE_5                   0x00020000
  #define INT_SAMPLE_5                   0x00040000
  #define TEMP_SAMPLE_5                  0x00080000
  #define SINGLE_END_SAMPLE_6            0x00000000
  #define DIF_SAMPLE_6                   0x00100000
  #define END_SAMPLE_6                   0x00200000
  #define INT_SAMPLE_6                   0x00400000
  #define TEMP_SAMPLE_6                  0x00800000
  #define SINGLE_END_SAMPLE_7            0x00000000
  #define DIF_SAMPLE_7                   0x01000000
  #define END_SAMPLE_7                   0x02000000
  #define INT_SAMPLE_7                   0x04000000
  #define TEMP_SAMPLE_7                  0x08000000
  #define SINGLE_END_SAMPLE_8            0x00000000
  #define DIF_SAMPLE_8                   0x10000000
  #define END_SAMPLE_8                   0x20000000
  #define INT_SAMPLE_8                   0x40000000
  #define TEMP_SAMPLE_8                  0x80000000
  #define SAMPLE_SHIFT_1                 0
  #define SAMPLE_SHIFT_2                 4
  #define SAMPLE_SHIFT_3                 8
  #define SAMPLE_SHIFT_4                 12
  #define SAMPLE_SHIFT_5                 16
  #define SAMPLE_SHIFT_6                 20
  #define SAMPLE_SHIFT_7                 24
  #define SAMPLE_SHIFT_8                 28
#define ADCSSFIFO0_0                     *(volatile unsigned long*)(ADC_0_BLOCK + 0x048) // ADC Sample Sequence Result 0 (read-only)
#define ADCSSFSTAT0_0                    *(volatile unsigned long*)(ADC_0_BLOCK + 0x04c) // ADC Sample Sequence FIFO 0 Status (read-only)
  #define FIFO_FULL                      0x00001000
  #define FIFO_EMPTY                     0x00000100
  #define HEAD_POINTER_MASK              0x000000f0
  #define TAIL_POINTER_MASK              0x0000000f
  #if defined _TEMPEST_CLASS
    #define ADCSSOP0_0                   *(unsigned long*)(ADC_0_BLOCK + 0x050) // ADC Sample Sequence 0 Operation (write 1 to clear)
    #define ADCSSDC0_0                   *(unsigned long*)(ADC_0_BLOCK + 0x054) // ADC Sample Sequence 0 Digital Comparator Select
  #endif
#define ADCSSMUX1_0                      *(unsigned long*)(ADC_0_BLOCK + 0x060) // ADC Sample Input Sequence Multiplexer Select 1
#define ADCSSCTL1_0                      *(unsigned long*)(ADC_0_BLOCK + 0x064) // ADC Sample Sequence Control 1
#define ADCSSFIFO1_0                     *(volatile unsigned long*)(ADC_0_BLOCK + 0x068) // ADC Sample Sequence Result 1 (read-only)
#define ADCSSFSTAT1_0                    *(volatile unsigned long*)(ADC_0_BLOCK + 0x06c) // ADC Sample Sequence FIFO 1 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP1_0                   *(unsigned long*)(ADC_0_BLOCK + 0x070) // ADC Sample Sequence 1 Operation (write 1 to clear)
    #define ADCSSDC1_0                   *(unsigned long*)(ADC_0_BLOCK + 0x074) // ADC Sample Sequence 1 Digital Comparator Select
  #endif
#define ADCSSMUX2_0                      *(unsigned long*)(ADC_0_BLOCK + 0x080) // ADC Sample Input Sequence Multiplexer Select 2
#define ADCSSCTL2_0                      *(unsigned long*)(ADC_0_BLOCK + 0x084) // ADC Sample Sequence Control 2
#define ADCSSFIFO2_0                     *(volatile unsigned long*)(ADC_0_BLOCK + 0x088) // ADC Sample Sequence Result 2 (read-only)
#define ADCSSFSTAT2_0                    *(volatile unsigned long*)(ADC_0_BLOCK + 0x08c) // ADC Sample Sequence FIFO 2 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP2_0                   *(unsigned long*)(ADC_0_BLOCK + 0x090) // ADC Sample Sequence 2 Operation (write 1 to clear)
    #define ADCSSDC2_0                   *(unsigned long*)(ADC_0_BLOCK + 0x094) // ADC Sample Sequence 2 Digital Comparator Select
  #endif
#define ADCSSMUX3_0                      *(unsigned long*)(ADC_0_BLOCK + 0x0a0) // ADC Sample Input Sequence Multiplexer Select 3
#define ADCSSCTL3_0                      *(unsigned long*)(ADC_0_BLOCK + 0x0a4) // ADC Sample Sequence Control 3
#define ADCSSFIFO3_0                     *(volatile unsigned long*)(ADC_0_BLOCK + 0x0a8) // ADC Sample Sequence Result 3 (read-only)
#define ADCSSFSTAT3_0                    *(volatile unsigned long*)(ADC_0_BLOCK + 0x0ac) // ADC Sample Sequence FIFO 3 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP3_0                   *(unsigned long*)(ADC_0_BLOCK + 0x0b0) // ADC Sample Sequence 3 Operation (write 1 to clear)
    #define ADCSSDC3_0                   *(unsigned long*)(ADC_0_BLOCK + 0x0b4) // ADC Sample Sequence 3 Digital Comparator Select

    #define ADCDCRIC_0                   *(unsigned long*)(ADC_0_BLOCK + 0xd00) // ADC Digital Comparator Reset Initial Conditions

    #define ADCDCCTL0_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe00) // ADC Digital Comparator Control 0
    #define ADCDCCTL1_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe04) // ADC Digital Comparator Control 1
    #define ADCDCCTL2_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe08) // ADC Digital Comparator Control 2
    #define ADCDCCTL3_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe0c) // ADC Digital Comparator Control 3
    #define ADCDCCTL4_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe10) // ADC Digital Comparator Control 4
    #define ADCDCCTL5_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe14) // ADC Digital Comparator Control 5
    #define ADCDCCTL6_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe18) // ADC Digital Comparator Control 6
    #define ADCDCCTL7_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe1c) // ADC Digital Comparator Control 7

    #define ADCDCCMP0_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe40) // ADC Digital Comparator Range 0
    #define ADCDCCMP1_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe44) // ADC Digital Comparator Range 1
    #define ADCDCCMP2_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe48) // ADC Digital Comparator Range 2
    #define ADCDCCMP3_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe4c) // ADC Digital Comparator Range 3
    #define ADCDCCMP4_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe50) // ADC Digital Comparator Range 4
    #define ADCDCCMP5_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe54) // ADC Digital Comparator Range 5
    #define ADCDCCMP6_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe58) // ADC Digital Comparator Range 6
    #define ADCDCCMP7_0                  *(unsigned long*)(ADC_0_BLOCK + 0xe5c) // ADC Digital Comparator Range 7
  #endif
#endif
#if PART_DC1 & ADC1_PRESENT1
#define ADCACTSS_1                       *(unsigned long*)(ADC_1_BLOCK + 0x000) // ADC Active Sample Sequencer
#define ADCRIS_1                         *(volatile unsigned long*)(ADC_1_BLOCK + 0x004) // ADC Raw Interrupt Status (read-only)
#define ADCIM_1                          *(unsigned long*)(ADC_1_BLOCK + 0x008) // ADC Interrupt Mask
#define ADCISC_1                         *(volatile unsigned long*)(ADC_1_BLOCK + 0x00c) // ADC Interrupt Status and Clear (write 1 to clear)
#define ADCOSTAT_1                       *(volatile unsigned long*)(ADC_1_BLOCK + 0x010) // ADC Overflow Status (write 1 to clear)
#define ADCEMUX_1                        *(unsigned long*)(ADC_1_BLOCK + 0x014) // ADC Event Multiplexer Select
#define ADCUSTAT_1                       *(volatile unsigned long*)(ADC_1_BLOCK + 0x018) // ADC Underflow Status (write 1 to clear)
#define ADCSSPRI_1                       *(unsigned long*)(ADC_1_BLOCK + 0x020) // ADC Sample Sequencer Priority
#define ADCPSSI_1                        *(volatile unsigned long*)(ADC_1_BLOCK + 0x028) // ADC Processor Sample Sequence Initiate (write-only)
#define ADCSAC_1                         *(unsigned long*)(ADC_1_BLOCK + 0x030) // ADC Sample Averaging Control
  #if defined _TEMPEST_CLASS
    #define ADCDCISC_1                   *(volatile unsigned long*)(ADC_1_BLOCK + 0x034) // ADC Digital Comparator Interrupt Status and Clear (write 1 to clear)
    #define ADCCTL_1                     *(unsigned long*)(ADC_1_BLOCK + 0x038) // ADC Control
  #endif
#define ADCSSMUX0_1                      *(unsigned long*)(ADC_1_BLOCK + 0x040) // ADC Sample Input Sequence Multiplexer Select 0
#define ADCSSCTL0_1                      *(unsigned long*)(ADC_1_BLOCK + 0x044) // ADC Sample Sequence Control 0
#define ADCSSFIFO0_1                     *(volatile unsigned long*)(ADC_1_BLOCK + 0x048) // ADC Sample Sequence Result 0 (read-only)
#define ADCSSFSTAT0_1                    *(volatile unsigned long*)(ADC_1_BLOCK + 0x04c) // ADC Sample Sequence FIFO 0 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP0_1                   *(unsigned long*)(ADC_1_BLOCK + 0x050) // ADC Sample Sequence 0 Operation (write 1 to clear)
    #define ADCSSDC0_1                   *(unsigned long*)(ADC_1_BLOCK + 0x054) // ADC Sample Sequence 0 Digital Comparator Select
  #endif
#define ADCSSMUX1_1                      *(unsigned long*)(ADC_1_BLOCK + 0x060) // ADC Sample Input Sequence Multiplexer Select 1
#define ADCSSCTL1_1                      *(unsigned long*)(ADC_1_BLOCK + 0x064) // ADC Sample Sequence Control 1
#define ADCSSFIFO1_1                     *(volatile unsigned long*)(ADC_1_BLOCK + 0x068) // ADC Sample Sequence Result 1 (read-only)
#define ADCSSFSTAT1_1                    *(volatile unsigned long*)(ADC_1_BLOCK + 0x06c) // ADC Sample Sequence FIFO 1 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP1_1                   *(unsigned long*)(ADC_1_BLOCK + 0x070) // ADC Sample Sequence 1 Operation (write 1 to clear)
    #define ADCSSDC1_1                   *(unsigned long*)(ADC_1_BLOCK + 0x074) // ADC Sample Sequence 1 Digital Comparator Select
  #endif
#define ADCSSMUX2_1                      *(unsigned long*)(ADC_1_BLOCK + 0x080) // ADC Sample Input Sequence Multiplexer Select 2
#define ADCSSCTL2_1                      *(unsigned long*)(ADC_1_BLOCK + 0x084) // ADC Sample Sequence Control 2
#define ADCSSFIFO2_1                     *(volatile unsigned long*)(ADC_1_BLOCK + 0x088) // ADC Sample Sequence Result 2 (read-only)
#define ADCSSFSTAT2_1                    *(volatile unsigned long*)(ADC_1_BLOCK + 0x08c) // ADC Sample Sequence FIFO 2 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP2_1                   *(unsigned long*)(ADC_1_BLOCK + 0x090) // ADC Sample Sequence 2 Operation (write 1 to clear)
    #define ADCSSDC2_1                   *(unsigned long*)(ADC_1_BLOCK + 0x094) // ADC Sample Sequence 2 Digital Comparator Select
  #endif
#define ADCSSMUX3_1                      *(unsigned long*)(ADC_1_BLOCK + 0x0a0) // ADC Sample Input Sequence Multiplexer Select 3
#define ADCSSCTL3_1                      *(unsigned long*)(ADC_1_BLOCK + 0x0a4) // ADC Sample Sequence Control 3
#define ADCSSFIFO3_1                     *(volatile unsigned long*)(ADC_1_BLOCK + 0x0a8) // ADC Sample Sequence Result 3 (read-only)
#define ADCSSFSTAT3_1                    *(volatile unsigned long*)(ADC_1_BLOCK + 0x0ac) // ADC Sample Sequence FIFO 3 Status (read-only)
  #if defined _TEMPEST_CLASS
    #define ADCSSOP3_1                   *(unsigned long*)(ADC_1_BLOCK + 0x0b0) // ADC Sample Sequence 3 Operation (write 1 to clear)
    #define ADCSSDC3_1                   *(unsigned long*)(ADC_1_BLOCK + 0x0b4) // ADC Sample Sequence 3 Digital Comparator Select

    #define ADCDCRIC_1                   *(unsigned long*)(ADC_1_BLOCK + 0xd00) // ADC Digital Comparator Reset Initial Conditions

    #define ADCDCCTL0_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe00) // ADC Digital Comparator Control 0
    #define ADCDCCTL1_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe04) // ADC Digital Comparator Control 1
    #define ADCDCCTL2_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe08) // ADC Digital Comparator Control 2
    #define ADCDCCTL3_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe0c) // ADC Digital Comparator Control 3
    #define ADCDCCTL4_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe10) // ADC Digital Comparator Control 4
    #define ADCDCCTL5_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe14) // ADC Digital Comparator Control 5
    #define ADCDCCTL6_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe18) // ADC Digital Comparator Control 6
    #define ADCDCCTL7_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe1c) // ADC Digital Comparator Control 7

    #define ADCDCCMP0_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe40) // ADC Digital Comparator Range 0
    #define ADCDCCMP1_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe44) // ADC Digital Comparator Range 1
    #define ADCDCCMP2_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe48) // ADC Digital Comparator Range 2
    #define ADCDCCMP3_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe4c) // ADC Digital Comparator Range 3
    #define ADCDCCMP4_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe50) // ADC Digital Comparator Range 4
    #define ADCDCCMP5_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe54) // ADC Digital Comparator Range 5
    #define ADCDCCMP6_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe58) // ADC Digital Comparator Range 6
    #define ADCDCCMP7_1                  *(unsigned long*)(ADC_1_BLOCK + 0xe5c) // ADC Digital Comparator Range 7
  #endif
#endif

typedef struct stLM3XXXXX_ADC                                            // {26} used only by Tempest class code
{
volatile unsigned long ADCACTSS;
volatile unsigned long ADCRIS;
volatile unsigned long ADCIM;
volatile unsigned long ADCISC;
volatile unsigned long ADCOSTAT;
volatile unsigned long ADCEMUX;
volatile unsigned long ADCUSTAT;
volatile unsigned long ulRes1;
volatile unsigned long ADCSSPRI;
volatile unsigned long ulRes2;
volatile unsigned long ADCPSSI;
volatile unsigned long ulRes3;
volatile unsigned long ADCSAC;
volatile unsigned long ADCDCISC;
volatile unsigned long ADCCTL;
volatile unsigned long ulRes4;
volatile unsigned long ADCSSMUX0;
volatile unsigned long ADCSSCTL0;
volatile unsigned long ADCSSFIFO;
volatile unsigned long ADCSSFSTAT0;
volatile unsigned long ADCSSOP;
volatile unsigned long ADCSSDC;
volatile unsigned long ulRes5[2];
volatile unsigned long ADCSSMUX1;
volatile unsigned long ADCSSCTL1;
volatile unsigned long ADCSSFIFO1;
volatile unsigned long ADCSSFSTAT1;
volatile unsigned long ADCSSOP1;
volatile unsigned long ADCSSDC1;
volatile unsigned long ulRes6[2];
volatile unsigned long ADCSSMUX2;
volatile unsigned long ADCSSCTL2;
volatile unsigned long ADCSSFIFO2;
volatile unsigned long ADCSSFSTAT2;
volatile unsigned long ADCSSOP2;
volatile unsigned long ADCSSDC2;
volatile unsigned long ulRes7[2];
volatile unsigned long ADCSSMUX3;
volatile unsigned long ADCSSCTL3;
volatile unsigned long ADCSSFIFO3;
volatile unsigned long ADCSSFSTAT3;
volatile unsigned long ADCSSOP3;
volatile unsigned long ADCSSDC3;
} LM3XXXXX_ADC;

#define ADC_CHANNEL_0                    0x00000001
#define ADC_CHANNEL_1                    0x00000002
#define ADC_CHANNEL_2                    0x00000004
#define ADC_CHANNEL_3                    0x00000008
#define ADC_CHANNEL_4                    0x00000010
#define ADC_CHANNEL_5                    0x00000020
#define ADC_CHANNEL_6                    0x00000040
#define ADC_CHANNEL_7                    0x00000080
#define ADC_CHANNEL_8                    0x00000100
#define ADC_CHANNEL_9                    0x00000200
#define ADC_CHANNEL_10                   0x00000400
#define ADC_CHANNEL_11                   0x00000800
#define ADC_CHANNEL_12                   0x00001000
#define ADC_CHANNEL_13                   0x00002000
#define ADC_CHANNEL_14                   0x00004000
#define ADC_CHANNEL_15                   0x00008000                      // {26}
#define ADC_CHANNEL_0_INTERNAL_TEMP      0x00010001                      // {15}
#define ADC_CHANNEL_1_INTERNAL_TEMP      0x00020002
#define ADC_CHANNEL_2_INTERNAL_TEMP      0x00040004
#define ADC_CHANNEL_3_INTERNAL_TEMP      0x00080008
#define ADC_CHANNEL_4_INTERNAL_TEMP      0x00100010
#define ADC_CHANNEL_5_INTERNAL_TEMP      0x00200020
#define ADC_CHANNEL_6_INTERNAL_TEMP      0x00400040
#define ADC_CHANNEL_7_INTERNAL_TEMP      0x00800080
#define ADC_CHANNEL_8_INTERNAL_TEMP      0x01000100
#define ADC_CHANNEL_9_INTERNAL_TEMP      0x02000200
#define ADC_CHANNEL_10_INTERNAL_TEMP     0x04000400
#define ADC_CHANNEL_11_INTERNAL_TEMP     0x08000800
#define ADC_CHANNEL_12_INTERNAL_TEMP     0x10001000
#define ADC_CHANNEL_13_INTERNAL_TEMP     0x20002000
#define ADC_CHANNEL_14_INTERNAL_TEMP     0x40004000
#define ADC_CHANNEL_15_INTERNAL_TEMP     0x80008000





#ifdef _WINDOWS
    extern unsigned long fnGetADCfifo(int adc, int seq);
    #define GET_ADC_FIFO(adc, seq, y)    fnGetADCfifo(adc, seq)
#else
    #define GET_ADC_FIFO(x, y, z)         z
#endif


// Ports
//
#define GPIODATA_A                        *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x03fc) // GPIO Data Register - PORT A
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_A                         *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0400) // GPIO Direction Register - PORT A
#define GPIOIS_A                          *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT A
#define GPIOIBE_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT A
#define GPIOIEV_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT A
#define GPIOIM_A                          *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT A
#define GPIORIS_A                         *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT A (read only)
#define GPIOMIS_A                         *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT A (read only)
#define GPIOICR_A                         *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT A
#define GPIOAFSEL_A                       *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT A
#define GPIODR2R_A                        *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT A
#define GPIODR4R_A                        *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT A
#define GPIODR8R_A                        *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT A
#define GPIOODR_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT A
#define GPIOPUR_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT A
#define GPIOPDR_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT A
#define GPIOSLR_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT A
#define GPIODEN_A                         *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT A
#define GPIOLOCK_A                        *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0520) // GPIO Lock Register - PORT A
  #define UNLOCK_GPIO_COMMIT_KEY          0x1acce551
#define GPIOCR_A                          *(volatile unsigned long*)(GPIO_PORT_A_BLOCK + 0x0524) // GPIO Commit Register - PORT A
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_A                     *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT A
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_A                      *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x052c) // GPIO Port Control Register - PORT A
#endif
#define GPIOPeriphID4_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT A (read-only)
#define GPIOPeriphID5_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT A (read-only)
#define GPIOPeriphID6_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT A (read-only)
#define GPIOPeriphID7_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT A (read-only)
#define GPIOPeriphID0_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT A (read-only)
#define GPIOPeriphID1_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT A (read-only)
#define GPIOPeriphID2_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT A (read-only)
#define GPIOPeriphID3_A                   *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT A (read-only)
#define GPIOPcellID0_A                    *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT A (read-only)
#define GPIOPcellID1_A                    *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT A (read-only)
#define GPIOPcellID2_A                    *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT A (read-only)
#define GPIOPcellID3_A                    *(unsigned long*)(GPIO_PORT_A_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT A (read-only)


#define GPIODATA_B                        *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x03fc) // GPIO Data Register - PORT B
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_B                         *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0400) // GPIO Direction Register - PORT B
#define GPIOIS_B                          *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT B
#define GPIOIBE_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT B
#define GPIOIEV_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT B
#define GPIOIM_B                          *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT B
#define GPIORIS_B                         *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT B (read only)
#define GPIOMIS_B                         *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT B (read only)
#define GPIOICR_B                         *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT B
#define GPIOAFSEL_B                       *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT B
#define GPIODR2R_B                        *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT B
#define GPIODR4R_B                        *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT B
#define GPIODR8R_B                        *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT B
#define GPIOODR_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT B
#define GPIOPUR_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT B
#define GPIOPDR_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT B
#define GPIOSLR_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT B
#define GPIODEN_B                         *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT B
#define GPIOLOCK_B                        *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0520) // GPIO Lock Register - PORT B
#define GPIOCR_B                          *(volatile unsigned long*)(GPIO_PORT_B_BLOCK + 0x0524) // GPIO Commit Register - PORT B
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_B                     *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT B
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_B                      *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x052c) // GPIO Port Control Register - PORT B
#endif
#define GPIOPeriphID4_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT B (read-only)
#define GPIOPeriphID5_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT B (read-only)
#define GPIOPeriphID6_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT B (read-only)
#define GPIOPeriphID7_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT B (read-only)
#define GPIOPeriphID0_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT B (read-only)
#define GPIOPeriphID1_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT B (read-only)
#define GPIOPeriphID2_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT B (read-only)
#define GPIOPeriphID3_B                   *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT B (read-only)
#define GPIOPcellID0_B                    *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT B (read-only)
#define GPIOPcellID1_B                    *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT B (read-only)
#define GPIOPcellID2_B                    *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT B (read-only)
#define GPIOPcellID3_B                    *(unsigned long*)(GPIO_PORT_B_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT B (read-only)


#define GPIODATA_C                        *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x03fc) // GPIO Data Register - PORT C
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_C                         *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0400) // GPIO Direction Register - PORT C
#define GPIOIS_C                          *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT C
#define GPIOIBE_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT C
#define GPIOIEV_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT C
#define GPIOIM_C                          *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT C
#define GPIORIS_C                         *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT C (read only)
#define GPIOMIS_C                         *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT C (read only)
#define GPIOICR_C                         *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT C
#define GPIOAFSEL_C                       *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT C
#define GPIODR2R_C                        *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT C
#define GPIODR4R_C                        *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT C
#define GPIODR8R_C                        *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT C
#define GPIOODR_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT C
#define GPIOPUR_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT C
#define GPIOPDR_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT C
#define GPIOSLR_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT C
#define GPIODEN_C                         *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT C
#define GPIOLOCK_C                        *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0520) // GPIO Lock Register - PORT C
#define GPIOCR_C                          *(volatile unsigned long*)(GPIO_PORT_C_BLOCK + 0x0524) // GPIO Commit Register - PORT C
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_C                     *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT C
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_C                      *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x052c) // GPIO Port Control Register - PORT C
#endif
#define GPIOPeriphID4_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT C (read-only)
#define GPIOPeriphID5_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT C (read-only)
#define GPIOPeriphID6_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT C (read-only)
#define GPIOPeriphID7_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT C (read-only)
#define GPIOPeriphID0_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT C (read-only)
#define GPIOPeriphID1_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT C (read-only)
#define GPIOPeriphID2_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT C (read-only)
#define GPIOPeriphID3_C                   *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT C (read-only)
#define GPIOPcellID0_C                    *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT C (read-only)
#define GPIOPcellID1_C                    *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT C (read-only)
#define GPIOPcellID2_C                    *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT C (read-only)
#define GPIOPcellID3_C                    *(unsigned long*)(GPIO_PORT_C_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT C (read-only)


#define GPIODATA_D                        *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x03fc) // GPIO Data Register - PORT D
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_D                         *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0400) // GPIO Direction Register - PORT D
#define GPIOIS_D                          *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT D
#define GPIOIBE_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT D
#define GPIOIEV_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT D
#define GPIOIM_D                          *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT D
#define GPIORIS_D                         *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT D (read only)
#define GPIOMIS_D                         *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT D (read only)
#define GPIOICR_D                         *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT D
#define GPIOAFSEL_D                       *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT D
#define GPIODR2R_D                        *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT D
#define GPIODR4R_D                        *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT D
#define GPIODR8R_D                        *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT D
#define GPIOODR_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT D
#define GPIOPUR_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT D
#define GPIOPDR_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT D
#define GPIOSLR_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT D
#define GPIODEN_D                         *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT D
#define GPIOLOCK_D                        *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0520) // GPIO Lock Register - PORT D
#define GPIOCR_D                          *(volatile unsigned long*)(GPIO_PORT_D_BLOCK + 0x0524) // GPIO Commit Register - PORT D
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_D                     *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT D
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_D                      *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x052c) // GPIO Port Control Register - PORT D
#endif
#define GPIOPeriphID4_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT D (read-only)
#define GPIOPeriphID5_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT D (read-only)
#define GPIOPeriphID6_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT D (read-only)
#define GPIOPeriphID7_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT D (read-only)
#define GPIOPeriphID0_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT D (read-only)
#define GPIOPeriphID1_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT D (read-only)
#define GPIOPeriphID2_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT D (read-only)
#define GPIOPeriphID3_D                   *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT D (read-only)
#define GPIOPcellID0_D                    *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT D (read-only)
#define GPIOPcellID1_D                    *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT D (read-only)
#define GPIOPcellID2_D                    *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT D (read-only)
#define GPIOPcellID3_D                    *(unsigned long*)(GPIO_PORT_D_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT D (read-only)


#define GPIODATA_E                        *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x03fc) // GPIO Data Register - PORT E
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_E                         *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0400) // GPIO Direction Register - PORT E
#define GPIOIS_E                          *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT E
#define GPIOIBE_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT E
#define GPIOIEV_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT E
#define GPIOIM_E                          *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT E
#define GPIORIS_E                         *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT E (read only)
#define GPIOMIS_E                         *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT E (read only)
#define GPIOICR_E                         *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT E
#define GPIOAFSEL_E                       *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT E
#define GPIODR2R_E                        *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT E
#define GPIODR4R_E                        *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT E
#define GPIODR8R_E                        *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT E
#define GPIOODR_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT E
#define GPIOPUR_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT E
#define GPIOPDR_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT E
#define GPIOSLR_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT E
#define GPIODEN_E                         *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT E
#define GPIOLOCK_E                        *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0520) // GPIO Lock Register - PORT E
#define GPIOCR_E                          *(volatile unsigned long*)(GPIO_PORT_E_BLOCK + 0x0524) // GPIO Commit Register - PORT E
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_E                     *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT E
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_E                      *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x052c) // GPIO Port Control Register - PORT E
#endif
#define GPIOPeriphID4_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT E (read-only)
#define GPIOPeriphID5_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT E (read-only)
#define GPIOPeriphID6_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT E (read-only)
#define GPIOPeriphID7_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT E (read-only)
#define GPIOPeriphID0_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT E (read-only)
#define GPIOPeriphID1_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT E (read-only)
#define GPIOPeriphID2_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT E (read-only)
#define GPIOPeriphID3_E                   *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT E (read-only)
#define GPIOPcellID0_E                    *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT E (read-only)
#define GPIOPcellID1_E                    *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT E (read-only)
#define GPIOPcellID2_E                    *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT E (read-only)
#define GPIOPcellID3_E                    *(unsigned long*)(GPIO_PORT_E_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT E (read-only)


#define GPIODATA_F                        *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x03fc) // GPIO Data Register - PORT F
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_F                         *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0400) // GPIO Direction Register - PORT F
#define GPIOIS_F                          *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT F
#define GPIOIBE_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT F
#define GPIOIEV_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT F
#define GPIOIM_F                          *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT F
#define GPIORIS_F                         *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT F (read only)
#define GPIOMIS_F                         *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT F (read only)
#define GPIOICR_F                         *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT F
#define GPIOAFSEL_F                       *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT F
#define GPIODR2R_F                        *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT F
#define GPIODR4R_F                        *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT F
#define GPIODR8R_F                        *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT F
#define GPIOODR_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT F
#define GPIOPUR_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT F
#define GPIOPDR_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT F
#define GPIOSLR_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT F
#define GPIODEN_F                         *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT F
#define GPIOLOCK_F                        *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0520) // GPIO Lock Register - PORT F
#define GPIOCR_F                          *(volatile unsigned long*)(GPIO_PORT_F_BLOCK + 0x0524) // GPIO Commit Register - PORT F
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_F                     *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT F
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_F                      *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x052c) // GPIO Port Control Register - PORT F
#endif
#define GPIOPeriphID4_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT F (read-only)
#define GPIOPeriphID5_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT F (read-only)
#define GPIOPeriphID6_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT F (read-only)
#define GPIOPeriphID7_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT F (read-only)
#define GPIOPeriphID0_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT F (read-only)
#define GPIOPeriphID1_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT F (read-only)
#define GPIOPeriphID2_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT F (read-only)
#define GPIOPeriphID3_F                   *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT F (read-only)
#define GPIOPcellID0_F                    *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT F (read-only)
#define GPIOPcellID1_F                    *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT F (read-only)
#define GPIOPcellID2_F                    *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT F (read-only)
#define GPIOPcellID3_F                    *(unsigned long*)(GPIO_PORT_F_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT F (read-only)


#define GPIODATA_G                        *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x03fc) // GPIO Data Register - PORT G
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_G                         *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0400) // GPIO Direction Register - PORT G
#define GPIOIS_G                          *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT G
#define GPIOIBE_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT G
#define GPIOIEV_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT G
#define GPIOIM_G                          *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT G
#define GPIORIS_G                         *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT G (read only)
#define GPIOMIS_G                         *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT G (read only)
#define GPIOICR_G                         *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT G
#define GPIOAFSEL_G                       *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT G
#define GPIODR2R_G                        *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT G
#define GPIODR4R_G                        *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT G
#define GPIODR8R_G                        *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT G
#define GPIOODR_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT G
#define GPIOPUR_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT G
#define GPIOPDR_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT G
#define GPIOSLR_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT G
#define GPIODEN_G                         *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT G
#define GPIOLOCK_G                        *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0520) // GPIO Lock Register - PORT G
#define GPIOCR_G                          *(volatile unsigned long*)(GPIO_PORT_G_BLOCK + 0x0524) // GPIO Commit Register - PORT G
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_G                     *(unsigned long *)(GPIO_PORT_G_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT G
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_G                      *(unsigned long *)(GPIO_PORT_G_BLOCK + 0x052c) // GPIO Port Control Register - PORT G
#endif
#define GPIOPeriphID4_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT G (read-only)
#define GPIOPeriphID5_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT G (read-only)
#define GPIOPeriphID6_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT G (read-only)
#define GPIOPeriphID7_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT G (read-only)
#define GPIOPeriphID0_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT G (read-only)
#define GPIOPeriphID1_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT G (read-only)
#define GPIOPeriphID2_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT G (read-only)
#define GPIOPeriphID3_G                   *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT G (read-only)
#define GPIOPcellID0_G                    *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT G (read-only)
#define GPIOPcellID1_G                    *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT G (read-only)
#define GPIOPcellID2_G                    *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT G (read-only)
#define GPIOPcellID3_G                    *(unsigned long*)(GPIO_PORT_G_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT G (read-only)

#define GPIODATA_H                        *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x03fc) // GPIO Data Register - PORT H
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_H                         *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0400) // GPIO Direction Register - PORT H
#define GPIOIS_H                          *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT H
#define GPIOIBE_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT H
#define GPIOIEV_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT H
#define GPIOIM_H                          *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT H
#define GPIORIS_H                         *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT H (read only)
#define GPIOMIS_H                         *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT H (read only)
#define GPIOICR_H                         *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT H
#define GPIOAFSEL_H                       *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT H
#define GPIODR2R_H                        *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT H
#define GPIODR4R_H                        *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT H
#define GPIODR8R_H                        *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT H
#define GPIOODR_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT H
#define GPIOPUR_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT H
#define GPIOPDR_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT H
#define GPIOSLR_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT H
#define GPIODEN_H                         *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT H
#define GPIOLOCK_H                        *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0520) // GPIO Lock Register - PORT H
#define GPIOCR_H                          *(volatile unsigned long*)(GPIO_PORT_H_BLOCK + 0x0524) // GPIO Commit Register - PORT H
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_H                     *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT H
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_H                      *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x052c) // GPIO Port Control Register - PORT H
#endif
#define GPIOPeriphID4_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT H (read-only)
#define GPIOPeriphID5_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT H (read-only)
#define GPIOPeriphID6_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT H (read-only)
#define GPIOPeriphID7_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT H (read-only)
#define GPIOPeriphID0_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT H (read-only)
#define GPIOPeriphID1_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT H (read-only)
#define GPIOPeriphID2_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT H (read-only)
#define GPIOPeriphID3_H                   *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT H (read-only)
#define GPIOPcellID0_H                    *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT H (read-only)
#define GPIOPcellID1_H                    *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT H (read-only)
#define GPIOPcellID2_H                    *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT H (read-only)
#define GPIOPcellID3_H                    *(unsigned long*)(GPIO_PORT_H_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT H (read-only)

#define GPIODATA_J                        *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x03fc) // GPIO Data Register - PORT J
                                          // This register occupies 0..0x3fc where the changed/read bit is addresses by A9:A2
#define GPIODIR_J                         *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0400) // GPIO Direction Register - PORT J
#define GPIOIS_J                          *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0404) // GPIO Interrupt Sense Register - PORT J
#define GPIOIBE_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0408) // GPIO Interrupt Both Edges Register - PORT J
#define GPIOIEV_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x040c) // GPIO Interrupt Event Register - PORT J
#define GPIOIM_J                          *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0410) // GPIO Interrupt Mask Register - PORT J
#define GPIORIS_J                         *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0414) // GPIO Raw Interrupt Status Register - PORT J (read only)
#define GPIOMIS_J                         *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0418) // GPIO Masked Interrupt Status Register - PORT J (read only)
#define GPIOICR_J                         *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x041c) // GPIO Interrupt Clear Register - PORT J
#define GPIOAFSEL_J                       *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0420) // GPIO Alternate Function Select Register - PORT J
#define GPIODR2R_J                        *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0500) // GPIO 2mA Drive Select Register - PORT J
#define GPIODR4R_J                        *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0504) // GPIO 4mA Drive Select Register - PORT J
#define GPIODR8R_J                        *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0508) // GPIO 8mA Drive Select Register - PORT J
#define GPIOODR_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x050c) // GPIO Open Drain Select Register - PORT J
#define GPIOPUR_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0510) // GPIO Pull-Up Select Register - PORT J
#define GPIOPDR_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0514) // GPIO Pull-Down Select Register - PORT J
#define GPIOSLR_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0518) // GPIO Slew Rate Control Select Register - PORT J
#define GPIODEN_J                         *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x051c) // GPIO Digital Enable Register - PORT J
#define GPIOLOCK_J                        *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0520) // GPIO Lock Register - PORT J
#define GPIOCR_J                          *(volatile unsigned long*)(GPIO_PORT_J_BLOCK + 0x0524) // GPIO Commit Register - PORT J
#if defined _TEMPEST_CLASS || defined _DUST_DEVIL_CLASS
  #define GPIOAMSEL_J                     *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0528) // GPIO Analog Mode Select Register - PORT J
#endif
#if defined _TEMPEST_CLASS
  #define GPIOPCTL_J                      *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x052c) // GPIO Port Control Register - PORT J
#endif
#define GPIOPeriphID4_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fd0) // GPIO Peripheral Identification Register 4 - PORT J (read-only)
#define GPIOPeriphID5_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fd4) // GPIO Peripheral Identification Register 5 - PORT J (read-only)
#define GPIOPeriphID6_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fd8) // GPIO Peripheral Identification Register 6 - PORT J (read-only)
#define GPIOPeriphID7_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fdc) // GPIO Peripheral Identification Register 7 - PORT J (read-only)
#define GPIOPeriphID0_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fe0) // GPIO Peripheral Identification Register 0 - PORT J (read-only)
#define GPIOPeriphID1_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fe4) // GPIO Peripheral Identification Register 1 - PORT J (read-only)
#define GPIOPeriphID2_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fe8) // GPIO Peripheral Identification Register 2 - PORT J (read-only)
#define GPIOPeriphID3_J                   *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0fec) // GPIO Peripheral Identification Register 3 - PORT J (read-only)
#define GPIOPcellID0_J                    *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0ff0) // GPIO Prime Cell Identification Register 0 - PORT J (read-only)
#define GPIOPcellID1_J                    *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0ff4) // GPIO Prime Cell Identification Register 1 - PORT J (read-only)
#define GPIOPcellID2_J                    *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0ff8) // GPIO Prime Cell Identification Register 2 - PORT J (read-only)
#define GPIOPcellID3_J                    *(unsigned long*)(GPIO_PORT_J_BLOCK + 0x0ffc) // GPIO Prime Cell Identification Register 3 - PORT J (read-only)




typedef struct stLM3XXXXX_PORT_CONTROL
{
volatile unsigned long GPIODATA;
volatile unsigned long GPIODIR;
unsigned long GPIOIS; 
unsigned long GPIOIBE;
unsigned long GPIOIEV;
unsigned long GPIOIM;
volatile unsigned long GPIORIS;
volatile unsigned long GPIOMIS;
volatile unsigned long _GPIOICR;
unsigned long GPIOAFSEL;
unsigned char ucRes1[220];
volatile unsigned long GPIODR2R;
volatile unsigned long GPIODR4R;
volatile unsigned long GPIODR8R;
unsigned long GPIOODR;
unsigned long GPIOPUR;
unsigned long GPIOPDR;
unsigned long GPIOSLR;
unsigned long GPIODEN;
volatile unsigned long GPIOLOCK;
volatile unsigned long GPIOCR;
unsigned long GPIOAMSEL;
unsigned long GPIOPCTL;
} LM3XXXXX_PORT_CONTROL;

#define PORT_BIT0                       0x01                             // general bit defines
#define PORT_BIT1                       0x02
#define PORT_BIT2                       0x04
#define PORT_BIT3                       0x08
#define PORT_BIT4                       0x10
#define PORT_BIT5                       0x20
#define PORT_BIT6                       0x40
#define PORT_BIT7                       0x80

#define PORTA_BIT0                      0x01                             // port specific bit defines (for stronger code relationship, but same effect)
#define PORTA_BIT1                      0x02
#define PORTA_BIT2                      0x04
#define PORTA_BIT3                      0x08
#define PORTA_BIT4                      0x10
#define PORTA_BIT5                      0x20
#define PORTA_BIT6                      0x40
#define PORTA_BIT7                      0x80

#define PORTB_BIT0                      0x01
#define PORTB_BIT1                      0x02
#define PORTB_BIT2                      0x04
#define PORTB_BIT3                      0x08
#define PORTB_BIT4                      0x10
#define PORTB_BIT5                      0x20
#define PORTB_BIT6                      0x40
#define PORTB_BIT7                      0x80

#define PORTC_BIT0                      0x01
#define PORTC_BIT1                      0x02
#define PORTC_BIT2                      0x04
#define PORTC_BIT3                      0x08
#define PORTC_BIT4                      0x10
#define PORTC_BIT5                      0x20
#define PORTC_BIT6                      0x40
#define PORTC_BIT7                      0x80

#define PORTD_BIT0                      0x01
#define PORTD_BIT1                      0x02
#define PORTD_BIT2                      0x04
#define PORTD_BIT3                      0x08
#define PORTD_BIT4                      0x10
#define PORTD_BIT5                      0x20
#define PORTD_BIT6                      0x40
#define PORTD_BIT7                      0x80

#define PORTE_BIT0                      0x01
#define PORTE_BIT1                      0x02
#define PORTE_BIT2                      0x04
#define PORTE_BIT3                      0x08
#define PORTE_BIT4                      0x10
#define PORTE_BIT5                      0x20
#define PORTE_BIT6                      0x40
#define PORTE_BIT7                      0x80

#define PORTF_BIT0                      0x01
#define PORTF_BIT1                      0x02
#define PORTF_BIT2                      0x04
#define PORTF_BIT3                      0x08
#define PORTF_BIT4                      0x10
#define PORTF_BIT5                      0x20
#define PORTF_BIT6                      0x40
#define PORTF_BIT7                      0x80

#define PORTG_BIT0                      0x01
#define PORTG_BIT1                      0x02
#define PORTG_BIT2                      0x04
#define PORTG_BIT3                      0x08
#define PORTG_BIT4                      0x10
#define PORTG_BIT5                      0x20
#define PORTG_BIT6                      0x40
#define PORTG_BIT7                      0x80

#define PORTH_BIT0                      0x01
#define PORTH_BIT1                      0x02
#define PORTH_BIT2                      0x04
#define PORTH_BIT3                      0x08
#define PORTH_BIT4                      0x10
#define PORTH_BIT5                      0x20
#define PORTH_BIT6                      0x40
#define PORTH_BIT7                      0x80

#define PORTJ_BIT0                      0x01
#define PORTJ_BIT1                      0x02
#define PORTJ_BIT2                      0x04
#define PORTJ_BIT3                      0x08
#define PORTJ_BIT4                      0x10
#define PORTJ_BIT5                      0x20
#define PORTJ_BIT6                      0x40
#define PORTJ_BIT7                      0x80

// Tempest Class Peripheral Functions
//
#define PA0_U0Rx                     (0x00000001)
#define PA0_I2C1SCL                  (0x00000008)
#define PA0_U1Rx                     (0x00000009)
#define PA1_U0Tx                     (0x00000001 << 4)
#define PA1_I2C1SDA                  (0x00000008 << 4)
#define PA1_U1Tx                     (0x00000009 << 4)
#define PA2_SSI0Clk                  (0x00000001 << 8)
#define PA2_PWM4                     (0x00000004 << 8)
#define PA2_I2S0RXSD                 (0x00000009 << 8)
#define PA3_SSI0Fss                  (0x00000001 << 12)
#define PA3_PWM5                     (0x00000004 << 12)
#define PA3_I2S0RXMCLK               (0x00000009 << 12)
#define PA4_SSI0Rx                   (0x00000001 << 16)
#define PA4_PWM6                     (0x00000004 << 16)
#define PA4_CAN0Rx                   (0x00000005 << 16)
#define PA4_I2S0TXSCK                (0x00000009 << 16)
#define PA5_SSI0Tx                   (0x00000001 << 20)
#define PA5_PWM7                     (0x00000004 << 20)
#define PA5_CAN0Tx                   (0x00000005 << 20)
#define PA5_I2STXWS                  (0x00000009 << 20)
#define PA6_I2C1SCL                  (0x00000001 << 24)
#define PA6_CCP1                     (0x00000002 << 24)
#define PA6_PWM0                     (0x00000004 << 24)
#define PA6_PWM4                     (0x00000004 << 24)
#define PA6_CAN0Rx                   (0x00000006 << 24)
#define PA6_USB0EPEN                 (0x00000008 << 24)
#define PA6_U1CTS                    (0x00000009 << 24)
#define PA7_I2C1SDA                  (0x00000001 << 28)
#define PA7_CCP4                     (0x00000002 << 28)
#define PA7_PWM1                     (0x00000004 << 28)
#define PA7_PWM5                     (0x00000005 << 28)
#define PA7_CAN0Tx                   (0x00000006 << 28)
#define PA7_CCP3                     (0x00000007 << 28)
#define PA7_USB0PFLT                 (0x00000008 << 28)
#define PA7_U1DCD                    (0x00000009 << 28)

#define PB0_CCP0                     (0x00000001)
#define PB0_PWM2                     (0x00000002)
#define PB0_U1Rx                     (0x00000005)
#define PB1_CCP2                     (0x00000001 << 4)
#define PB1_PWM3                     (0x00000002 << 4)
#define PB1_CCP1                     (0x00000004 << 4)
#define PB1_U1Tx                     (0x00000005 << 4)
#define PB2_I2C0SCL                  (0x00000001 << 8)
#define PB2_IDX0                     (0x00000002 << 8)
#define PB2_CCP3                     (0x00000004 << 8)
#define PB2_CCP0                     (0x00000005 << 8)
#define PB2_USB0EPEN                 (0x00000008 << 8)
#define PB3_I2C0SDA                  (0x00000001 << 12)
#define PB3_Fault0                   (0x00000002 << 12)
#define PB3_Fault3                   (0x00000004 << 12)
#define PB3_USB0PFLT                 (0x00000008 << 12)
#define PB4_U2Rx                     (0x00000004 << 16)
#define PB4_CAN0Rx                   (0x00000005 << 16)
#define PB4_IDX0                     (0x00000006 << 16)
#define PB4_U1Rx                     (0x00000007 << 16)
#define PB4_EPI0S23                  (0x00000008 << 16)
#define PB5_C0o                      (0x00000001 << 20)
#define PB5_CCP5                     (0x00000002 << 20)
#define PB5_CCP6                     (0x00000003 << 20)
#define PB5_CCP0                     (0x00000004 << 20)
#define PB5_CAN0Tx                   (0x00000005 << 20)
#define PB5_CCP2                     (0x00000006 << 20)
#define PB5_U1Tx                     (0x00000007 << 20)
#define PB5_EPI0S22                  (0x00000008 << 20)
#define PB6_CCP1                     (0x00000001 << 24)
#define PB6_CCP7                     (0x00000002 << 24)
#define PB6_C0o                      (0x00000003 << 24)
#define PB6_Fault1                   (0x00000004 << 24)
#define PB6_IDX0                     (0x00000005 << 24)
#define PB6_CCP5                     (0x00000006 << 24)
#define PB6_I2S0TXSCK                (0x00000009 << 24)
#define PB7_NMI                      (0x00000004 << 28)

#define PC0_TCK_SWCLK                (0x00000003)
#define PC1_TMS_SWDIO                (0x00000003 << 4)
#define PC2_TDI                      (0x00000003 << 8)
#define PC3_TDO_SWO                  (0x00000003 << 12)
#define PC4_CCP5                     (0x00000001 << 16)
#define PC4_PhA0                     (0x00000002 << 16)
#define PC4_PWM6                     (0x00000004 << 16)
#define PC4_CCP2                     (0x00000005 << 16)
#define PC4_CCP4                     (0x00000006 << 16)
#define PC4_EPI0S2                   (0x00000008 << 16)
#define PC4_CCP1                     (0x00000009 << 16)
#define PC5_CCP1                     (0x00000001 << 20)
#define PC5_C1o                      (0x00000002 << 20)
#define PC5_C0o                      (0x00000003 << 20)
#define PC5_Fault2                   (0x00000004 << 20)
#define PC5_CCP3                     (0x00000005 << 20)
#define PC5_USB0EPEN                 (0x00000006 << 20)
#define PC5_EPI0S3                   (0x00000008 << 20)
#define PC6_CCP3                     (0x00000001 << 24)
#define PC6_PhB0                     (0x00000002 << 24)
#define PC6_C2o                      (0x00000003 << 24)
#define PC6_PWM7                     (0x00000004 << 24)
#define PC6_U1Rx                     (0x00000005 << 24)
#define PC6_CCP0                     (0x00000006 << 24)
#define PC6_USB0PFLT                 (0x00000007 << 24)
#define PC6_EPI0S4                   (0x00000008 << 24)
#define PC7_CCP4                     (0x00000001 << 28)
#define PC7_PhB0                     (0x00000002 << 28)
#define PC7_CCP0                     (0x00000004 << 28)
#define PC7_U1Tx                     (0x00000005 << 28)
#define PC7_USB0PFLT                 (0x00000006 << 28)
#define PC7_C1o                      (0x00000007 << 28)
#define PC7_EPI0S5                   (0x00000008 << 28)

#define PD0_PWM0                     (0x00000001)
#define PD0_CAN0Rx                   (0x00000002)
#define PD0_IDX0                     (0x00000003)
#define PD0_U2Rx                     (0x00000004)
#define PD0_U1Rx                     (0x00000005)
#define PD0_CCP6                     (0x00000006)
#define PD0_I2S0RXSCK                (0x00000008)
#define PD0_U1CTS                    (0x00000009)
#define PD1_PWM1                     (0x00000001 << 4)
#define PD1_CAN0Tx                   (0x00000002 << 4)
#define PD1_PhA0                     (0x00000003 << 4)
#define PD1_U2Tx                     (0x00000004 << 4)
#define PD1_U1Tx                     (0x00000005 << 4)
#define PD1_CCP7                     (0x00000006 << 4)
#define PD1_I2S0RXWS                 (0x00000008 << 4)
#define PD1_U1DCD                    (0x00000009 << 4)
#define PD1_CCP2                     (0x0000000a << 4)
#define PD1_PhB1                     (0x0000000b << 4)
#define PD2_U1Rx                     (0x00000001 << 8)
#define PD2_CCP6                     (0x00000002 << 8)
#define PD2_PWM2                     (0x00000003 << 8)
#define PD2_CCP5                     (0x00000004 << 8)
#define PD2_EPI0S20                  (0x00000008 << 8)
#define PD3_U1Tx                     (0x00000001 << 12)
#define PD3_CCP7                     (0x00000002 << 12)
#define PD3_PWM3                     (0x00000003 << 12)
#define PD3_CCP0                     (0x00000004 << 12)
#define PD3_EPI0S21                  (0x00000008 << 12)
#define PD4_CCP0                     (0x00000001 << 16)
#define PD4_CCP3                     (0x00000002 << 16)
#define PD4_I2S0RXSD                 (0x00000008 << 16)
#define PD4_U1RI                     (0x00000009 << 16)
#define PD4_EPI0S19                  (0x0000000a << 16)
#define PD5_CCP2                     (0x00000001 << 20)
#define PD5_CCP4                     (0x00000002 << 20)
#define PD5_I2S0RXMCLK               (0x00000008 << 20)
#define PD5_U2Rx                     (0x00000009 << 20)
#define PD5_EPI0S28                  (0x0000000a << 20)
#define PD6_Fault0                   (0x00000001 << 24)
#define PD6_I2S0TXSCK                (0x00000002 << 24)
#define PD6_U2Tx                     (0x00000009 << 24)
#define PD6_EPI0S29                  (0x0000000a << 24)
#define PD7_IDX0                     (0x00000001 << 28)
#define PD7_C0o                      (0x00000002 << 28)
#define PD7_CCP1                     (0x00000003 << 28)
#define PD7_I2S0TXWS                 (0x00000008 << 28)
#define PD7_U1DTR                    (0x00000009 << 28)
#define PD7_EPI0S30                  (0x0000000a << 28)

#define PE0_PWM4                     (0x00000001)
#define PE0_SSI1Clk                  (0x00000002)
#define PE0_CCP3                     (0x00000003)
#define PE0_EPI0S8                   (0x00000008)
#define PE0_USB0PFLT                 (0x00000009)
#define PE1_PWM5                     (0x00000001 << 4)
#define PE1_SSI1Fss                  (0x00000002 << 4)
#define PE1_Fault0                   (0x00000003 << 4)
#define PE1_CCP2                     (0x00000004 << 4)
#define PE1_CCP6                     (0x00000005 << 4)
#define PE1_EPI0S9                   (0x00000008 << 4)
#define PE2_CCP4                     (0x00000001 << 8)
#define PE2_SSI1Rx                   (0x00000002 << 8)
#define PE2_PhB1                     (0x00000003 << 8)
#define PE2_PhA0                     (0x00000004 << 8)
#define PE2_CCP2                     (0x00000005 << 8)
#define PE2_EPI0S24                  (0x00000008 << 8)
#define PE3_CCP1                     (0x00000001 << 12)
#define PE3_SSI1Tx                   (0x00000002 << 12)
#define PE3_PhA1                     (0x00000003 << 12)
#define PE3_PhB0                     (0x00000004 << 12)
#define PE3_CCP7                     (0x00000005 << 12)
#define PE3_EPI0S25                  (0x00000008 << 12)
#define PE4_CCP3                     (0x00000001 << 16)
#define PE4_Fault0                   (0x00000004 << 16)
#define PE4_U2Tx                     (0x00000005 << 16)
#define PE4_CCP2                     (0x00000006 << 16)
#define PE4_I2S0TXWS                 (0x00000009 << 16)
#define PE5_CCP5                     (0x00000001 << 20)
#define PE5_I2S0TXSD                 (0x00000009 << 20)
#define PE6_PWM4                     (0x00000001 << 24)
#define PE6_C1o                      (0x00000002 << 24)
#define PE6_U1CTS                    (0x00000009 << 24)
#define PE7_PWM5                     (0x00000001 << 28)
#define PE7_C2o                      (0x00000002 << 28)
#define PE7_U1DCD                    (0x00000009 << 28)

#define PF0_CAN1Rx                   (0x00000001)
#define PF0_PhB0                     (0x00000002)
#define PF0_PWM0                     (0x00000003)
#define PF0_I2S0TXSD                 (0x00000008)
#define PF0_U1DSR                    (0x00000009)
#define PF1_CAN1Tx                   (0x00000001 << 4)
#define PF1_IDX1                     (0x00000002 << 4)
#define PF1_PWM1                     (0x00000003 << 4)
#define PF1_I2S0TXMCLK               (0x00000008 << 4)
#define PF1_U1RTS                    (0x00000009 << 4)
#define PF1_CCP3                     (0x0000000a << 4)
#define PF2_LED1                     (0x00000001 << 8)
#define PF2_PWM4                     (0x00000002 << 8)
#define PF2_PWM2                     (0x00000004 << 8)
#define PF2_SSI1Clk                  (0x00000009 << 8)
#define PF3_LED0                     (0x00000001 << 12)
#define PF3_PWM5                     (0x00000002 << 12)
#define PF3_PWM3                     (0x00000004 << 12)
#define PF3_SSI1Fss                  (0x00000009 << 12)
#define PF4_CCP0                     (0x00000001 << 16)
#define PF4_C0o                      (0x00000002 << 16)
#define PF4_Fault0                   (0x00000004 << 16)
#define PF4_EPI0S12                  (0x00000008 << 16)
#define PF4_SSI1Rx                   (0x00000009 << 16)
#define PF5_CCP2                     (0x00000001 << 20)
#define PF5_C1o                      (0x00000002 << 20)
#define PF5_EPI0S15                  (0x00000008 << 20)
#define PF5_SSI1Tx                   (0x00000009 << 20)

#define PG0_U2Rx                     (0x00000001)
#define PG0_PWM0                     (0x00000002)
#define PG0_I2C1SCL                  (0x00000003)
#define PG0_PWM4                     (0x00000004)
#define PG0_USB0EPEN                 (0x00000007)
#define PG0_EPI0S13                  (0x00000008)
#define PG1_U2Tx                     (0x00000001 << 4)
#define PG1_PWM1                     (0x00000002 << 4)
#define PG1_I2C1SDA                  (0x00000003 << 4)
#define PG1_PWM5                     (0x00000004 << 4)
#define PG1_EPI0S14                  (0x00000008 << 4)
#define PG7_PhB1                     (0x00000001 << 28)
#define PG7_PWM7                     (0x00000004 << 28)
#define PG7_CCP5                     (0x00000008 << 28)
#define PG7_EPI0S31                  (0x00000009 << 28)

#define PH0_CCP6                     (0x00000001)
#define PH0_PWM2                     (0x00000002)
#define PH0_EPI0S6                   (0x00000008)
#define PH0_PWM4                     (0x00000009)
#define PH1_CCP7                     (0x00000001 << 4)
#define PH1_PWM3                     (0x00000002 << 4)
#define PH1_EPI0S7                   (0x00000008 << 4)
#define PH1_PWM5                     (0x00000009 << 4)
#define PH2_IDX1                     (0x00000001 << 8)
#define PH2_C1o                      (0x00000002 << 8)
#define PH2_Fault3                   (0x00000004 << 8)
#define PH2_EPI0S1                   (0x00000008 << 8)
#define PH3_PhB0                     (0x00000001 << 12)
#define PH3_Fault0                   (0x00000002 << 12)
#define PH3_USB0EPEN                 (0x00000004 << 12)
#define PH3_EPI0S0                   (0x00000008 << 12)
#define PH4_USB0PFLT                 (0x00000004 << 16)
#define PH4_EPI0S10                  (0x00000008 << 16)
#define PH4_SSI1Clk                  (0x0000000b << 16)
#define PH5_EPI0S11                  (0x00000008 << 20)
#define PH5_Fault2                   (0x0000000a << 20)
#define PH5_SSI1Fss                  (0x0000000b << 20)
#define PH6_EPI0S26                  (0x00000008 << 24)
#define PH6_PWM4                     (0x0000000a << 24)
#define PH6_SSI1Rx                   (0x0000000b << 24)
#define PH7_EPI0S27                  (0x00000008 << 28)
#define PH7_PWM5                     (0x0000000a << 28)
#define PH7_SSI1Tx                   (0x0000000b << 28)

#define PJ0_EPI0S16                  (0x00000008)
#define PJ0_PWM0                     (0x0000000a)
#define PJ0_I2C1SCL                  (0x0000000b)
#define PJ1_EPI0S17                  (0x00000008 << 4)
#define PJ1_USB0PFLT                 (0x00000009 << 4)
#define PJ1_PWM1                     (0x0000000a << 4)
#define PJ1_I2C1SDA                  (0x0000000b << 4)
#define PJ2_EPI0S18                  (0x00000008 << 8)
#define PJ2_CCP0                     (0x00000009 << 8)
#define PJ2_Fault0                   (0x0000000a << 8)
#define PJ3_EPI0S19                  (0x00000008 << 12)
#define PJ3_U1CTS                    (0x00000009 << 12)
#define PJ3_CCP6                     (0x0000000a << 12)
#define PJ4_EPI0S28                  (0x00000008 << 16)
#define PJ4_U1DCD                    (0x00000009 << 16)
#define PJ4_CCP4                     (0x0000000a << 16)
#define PJ5_EPI0S29                  (0x00000008 << 20)
#define PJ5_U1DSR                    (0x00000009 << 20)
#define PJ5_CCP2                     (0x0000000a << 20)
#define PJ6_EPI0S30                  (0x00000008 << 24)
#define PJ6_U1RTS                    (0x00000009 << 24)
#define PJ6_CCP1                     (0x0000000a << 24)
#define PJ7_U1DTR                    (0x00000009 << 28)
#define PJ7_CCP0                     (0x0000000a << 28)

#ifdef _WINDOWS
    #define  _SIM_PORT() fnPortChanges(1)                                // {12}
#else
    #define  _SIM_PORT()
#endif

// Port macros                                                           // {23} masking addresses made volatile
//
// Configure pins as output, including enabling power and digital use. eg. _CONFIG_PORT_OUTPUT(A, PORTA_BIT0);
//
#define _CONFIG_PORT_OUTPUT(ref, pins)  RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIODEN_##ref |= (pins); GPIODIR_##ref |= (pins); _SIM_PORT()
// Configure pins as input, including enabling power and digital use. eg. _CONFIG_PORT_INPUT(B, PORTB_BIT2);
//
#define _CONFIG_PORT_INPUT(ref, pins)   RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIODEN_##ref |= (pins); GPIODIR_##ref &= ~(pins); _SIM_PORT()

// Configure a peripheral function
//
#ifdef _TEMPEST_CLASS
    #define _CONFIG_PERIPHERAL(ref, periph, peripheral_block, pins, functions) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref;  \
                                       RCGC##peripheral_block |= CGC_##periph;  SRCR##peripheral_block &= ~(CGC_##periph); \
                                       GPIOPCTL_##ref  |= functions;                       \
                                       GPIOAFSEL_##ref |= pins; GPIODEN_##ref |= pins; _SIM_PORT()
#else
    #define _CONFIG_PERIPHERAL(ref, periph, peripheral_block, pins, functions) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref;  \
                                       RCGC##peripheral_block |= CGC_##periph;  SRCR##peripheral_block &= ~(CGC_##periph); \
                                       GPIOAFSEL_##ref |= pins; GPIODEN_##ref |= pins; _SIM_PORT()
#endif

#define _RESET_PORT_PERIPHERAL(ref, pins) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIOAFSEL_##ref &= ~pins; _SIM_PORT() // {28}


// Write full port width
//
#define _WRITE_PORT(ref, value)         GPIODATA_##ref = value; _SIM_PORT()


// Write to a port with a mask
//
#ifdef _WINDOWS
    #define _WRITE_PORT_MASK(ref, value, mask)    GPIODATA_##ref = ((GPIODATA_##ref & ~mask) | (value & mask)); _SIM_PORT()
#else
    #define _WRITE_PORT_MASK(ref, value, mask)    *(volatile unsigned long *)(GPIO_PORT_##ref##_BLOCK + 4*mask) = value
#endif

// Toggle a port with a mask
//
#define _TOGGLE_PORT(ref, mask)        GPIODATA_##ref ^= mask; _SIM_PORT()

// Read full port width
//
#define _READ_PORT(ref)                (GPIODATA_##ref)

// Read from a port with a mask
//
#ifdef _WINDOWS
    #define _READ_PORT_MASK(ref, mask) (GPIODATA_##ref & mask)
#else
    #define _READ_PORT_MASK(ref, mask) *(volatile unsigned long *)(GPIO_PORT_##ref##_BLOCK + 4*mask)
#endif

// Configure outputs and then set a value to them
//
#ifdef _WINDOWS
    #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIODEN_##ref |= (pins); GPIODIR_##ref |= (pins); GPIODATA_##ref = ((GPIODATA_##ref & ~(pins)) | ((value) & (pins))); _SIM_PORT()
#else
    #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIODEN_##ref |= (pins); GPIODIR_##ref |= (pins); *(volatile unsigned long *)(GPIO_PORT_##ref##_BLOCK + 4*(pins)) = (value)
#endif

// Set from outputs to inputs
//
#define _FLOAT_PORT(ref, pins)         GPIODIR_##ref &= ~(pins); _SIM_PORT()

// Set from inputs to outputs 
//
#define _DRIVE_PORT_OUTPUT(ref, pins)  GPIODIR_##ref |= (pins); _SIM_PORT()

// Set from inputs to outputs and set a value to them
//
#ifdef _WINDOWS
    #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) GPIODIR_##ref |= (pins); GPIODATA_##ref = ((GPIODATA_##ref & ~(pins)) | ((value) & (pins))); _SIM_PORT()
#else
    #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) GPIODIR_##ref |= (pins); *(volatile unsigned long *)(GPIO_PORT_##ref##_BLOCK + 4*(pins)) = (value)
#endif

// Set and clear individual bits of a port
//
#ifdef _WINDOWS
    #define _SETBITS(ref, mask)        GPIODATA_##ref |= mask; _SIM_PORT()
#else
    #define _SETBITS(ref, mask)        *(volatile unsigned long *)(GPIO_PORT_##ref##_BLOCK + 4*mask) = 0xff
#endif
#ifdef _WINDOWS
    #define _CLEARBITS(ref, mask)      GPIODATA_##ref &= ~mask; _SIM_PORT()
#else
    #define _CLEARBITS(ref, mask)      *(volatile unsigned long *)(GPIO_PORT_##ref##_BLOCK + 4*mask) = 0x00
#endif

// Configure Pull-ups and Pull downs                                     {22}
//
#define _CONFIG_PORT_PULLUP(ref, pins) GPIOPUR_##ref |= (pins); _SIM_PORT()
#define _CONFIG_PORT_PULLDN(ref, pins) GPIOPDR_##ref |= (pins); _SIM_PORT()


// Port unlocking and locking                                            {22}
//
#define _UNLOCK_GPIO(ref, pins) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIOLOCK_##ref = UNLOCK_GPIO_COMMIT_KEY; GPIOCR_##ref |= pins; GPIOAFSEL_##ref &= ~(pins); _SIM_PORT()
#define _UNLOCK_PERIPHERAL(ref, pins) RCGC2 |= CGC_GPIO##ref; SRCR2 &= ~CGC_GPIO##ref; GPIOLOCK_##ref = UNLOCK_GPIO_COMMIT_KEY; GPIOCR_##ref |= pins; GPIOAFSEL_##ref |= (pins); _SIM_PORT()
#define _LOCK_PORT(ref, pins) GPIOLOCK_##ref = UNLOCK_GPIO_COMMIT_KEY; GPIOCR_##ref &= ~(pins); _SIM_PORT()



// PWM Module                                                            {30}
//
#define PWMCTL                      *(unsigned long*)(PWM_BLOCK + 0x000) // PWM Master Control
#define PWMSYNC                     *(unsigned long*)(PWM_BLOCK + 0x004) // PWM Time base Sync
#define PWMENABLE                   *(unsigned long*)(PWM_BLOCK + 0x008) // PWM Output Enable
  #define PWM0En                    0x00000001
  #define PWM1En                    0x00000002
  #define PWM2En                    0x00000004
  #define PWM3En                    0x00000008
  #define PWM4En                    0x00000010
  #define PWM5En                    0x00000020
  #define PWM6En                    0x00000040
  #define PWM7En                    0x00000080
#define PWMINVERT                   *(unsigned long*)(PWM_BLOCK + 0x00c) // PWM Output Inversion
#define PWMFAULT                    *(unsigned long*)(PWM_BLOCK + 0x010) // PWM Output Fault
#define PWMINTEN                    *(unsigned long*)(PWM_BLOCK + 0x014) // PWM Interrupt Enable
#define PWMRIS                      *(volatile unsigned long*)(PWM_BLOCK + 0x018) // PWM Raw Interrupt Status (read-only)
#define PWMISC                      *(volatile unsigned long*)(PWM_BLOCK + 0x01c) // PWM Interrupt Status and Clear (read-only - write 1 to clear)
#define PWMSTATUS                   *(volatile unsigned long*)(PWM_BLOCK + 0x020) // PWM Status (read-only)
#define PWMFAULTVAL                 *(volatile unsigned long*)(PWM_BLOCK + 0x024) // PWM Fault Condition Value

#define PWM0CTL                     *(unsigned long*)(PWM_BLOCK + 0x040) // PWM0 Control
  #define PWM_BLOCK_ENABLE          0x00000001
#define PWM0INTEN                   *(unsigned long*)(PWM_BLOCK + 0x044) // PWM0 Interrupt and Trigger Enable
#define PWM0RIS                     *(volatile unsigned long*)(PWM_BLOCK + 0x048) // PWM0 Raw Interrupt Status
#define PWM0ISC                     *(volatile unsigned long*)(PWM_BLOCK + 0x04c) // PWM0 Interrupt Status and Clear (read-only - write 1 to clear)
#define PWM0LOAD                    *(unsigned long*)(PWM_BLOCK + 0x050) // PWM0 Load
#define PWM0COUNT                   *(volatile unsigned long*)(PWM_BLOCK + 0x054) // PWM0 Counter
#define PWM0CMPA                    *(unsigned long*)(PWM_BLOCK + 0x058) // PWM0 Compare A
#define PWM0CMPB                    *(unsigned long*)(PWM_BLOCK + 0x05c) // PWM0 Compare B
#define PWM0GENA                    *(unsigned long*)(PWM_BLOCK + 0x060) // PWM0 Generator A Control
  #define ACT_ZERO_NOTHING          0x00000000                           // action when the counter reaches 0
  #define ACT_ZERO_INVERT           0x00000001
  #define ACT_ZERO_CLEAR            0x00000002
  #define ACT_ZERO_SET              0x00000003
  #define ACT_LOAD_NOTHING          0x00000000                           // action when counter matches load value
  #define ACT_LOAD_INVERT           0x00000004
  #define ACT_LOAD_CLEAR            0x00000008
  #define ACT_LOAD_SET              0x0000000c
  #define ACT_CMPAU_NOTHING         0x00000000                           // action when counter matches comparator A value when counting up (only when PWMnCTL register is set to 1)
  #define ACT_CMPAU_INVERT          0x00000010
  #define ACT_CMPAU_CLEAR           0x00000020
  #define ACT_CMPAU_SET             0x00000030
  #define ACT_CMPAD_NOTHING         0x00000000                           // action when counter matches comparator A value when counting down
  #define ACT_CMPAD_INVERT          0x00000040
  #define ACT_CMPAD_CLEAR           0x00000080
  #define ACT_CMPAD_SET             0x000000c0
  #define ACT_CMPBU_NOTHING         0x00000000                           // action when counter matches comparator B value when counting up (only when PWMnCTL register is set to 1)
  #define ACT_CMPBU_INVERT          0x00000100
  #define ACT_CMPBU_CLEAR           0x00000200
  #define ACT_CMPBU_SET             0x00000300
  #define ACT_CMPBD_NOTHING         0x00000000                           // action when counter matches comparator B value when counting up (only when PWMnCTL register is set to 1)
  #define ACT_CMPBD_INVERT          0x00000400
  #define ACT_CMPBD_CLEAR           0x00000800
  #define ACT_CMPBD_SET             0x00000c00
#define PWM0GENB                    *(unsigned long*)(PWM_BLOCK + 0x064) // PWM0 Generator B Control
#define PWM0DBCTL                   *(unsigned long*)(PWM_BLOCK + 0x068) // PWM0 Dead-Band Control
#define PWM0DBRISE                  *(unsigned long*)(PWM_BLOCK + 0x06c) // PWM0 Dead-Band Rising Edge Delay
#define PWM0DBRALL                  *(unsigned long*)(PWM_BLOCK + 0x070) // PWM0 Dead-Band Falling Edge Delay
#define PWM0FLTSRC0                 *(unsigned long*)(PWM_BLOCK + 0x074) // PWM0 Fault Source 0
#define PWM0MINFLTPER               *(unsigned long*)(PWM_BLOCK + 0x07c) // PWM0 Minimum Fault Period

#define PWM1CTL                     *(unsigned long*)(PWM_BLOCK + 0x080) // PWM1 Control
#define PWM1INTEN                   *(unsigned long*)(PWM_BLOCK + 0x084) // PWM1 Interrupt and Trigger Enable
#define PWM1RIS                     *(volatile unsigned long*)(PWM_BLOCK + 0x088) // PWM1 Raw Interrupt Status
#define PWM1ISC                     *(volatile unsigned long*)(PWM_BLOCK + 0x08c) // PWM1 Interrupt Status and Clear (read-only - write 1 to clear)
#define PWM1LOAD                    *(unsigned long*)(PWM_BLOCK + 0x090) // PWM1 Load
#define PWM1COUNT                   *(volatile unsigned long*)(PWM_BLOCK + 0x094) // PWM1 Counter
#define PWM1CMPA                    *(unsigned long*)(PWM_BLOCK + 0x098) // PWM1 Compare A
#define PWM1CMPB                    *(unsigned long*)(PWM_BLOCK + 0x09c) // PWM1 Compare B
#define PWM1GENA                    *(unsigned long*)(PWM_BLOCK + 0x0a0) // PWM1 Generator A Control
#define PWM1GENB                    *(unsigned long*)(PWM_BLOCK + 0x0a4) // PWM1 Generator B Control
#define PWM1DBCTL                   *(unsigned long*)(PWM_BLOCK + 0x0a8) // PWM1 Dead-Band Control
#define PWM1DBRISE                  *(unsigned long*)(PWM_BLOCK + 0x0ac) // PWM1 Dead-Band Rising Edge Delay
#define PWM1DBRALL                  *(unsigned long*)(PWM_BLOCK + 0x0b0) // PWM1 Dead-Band Falling Edge Delay
#define PWM1FLTSRC0                 *(unsigned long*)(PWM_BLOCK + 0x0b4) // PWM1 Fault Source 0
#define PWM1MINFLTPER               *(unsigned long*)(PWM_BLOCK + 0x0bc) // PWM1 Minimum Fault Period

#define PWM2CTL                     *(unsigned long*)(PWM_BLOCK + 0x0c0) // PWM2 Control
#define PWM2INTEN                   *(unsigned long*)(PWM_BLOCK + 0x0c4) // PWM2 Interrupt and Trigger Enable
#define PWM2RIS                     *(volatile unsigned long*)(PWM_BLOCK + 0x0c8) // PWM2 Raw Interrupt Status
#define PWM2ISC                     *(volatile unsigned long*)(PWM_BLOCK + 0x0cc) // PWM2 Interrupt Status and Clear (read-only - write 1 to clear)
#define PWM2LOAD                    *(unsigned long*)(PWM_BLOCK + 0x0d0) // PWM2 Load
#define PWM2COUNT                   *(volatile unsigned long*)(PWM_BLOCK + 0x0d4) // PWM2 Counter
#define PWM2CMPA                    *(unsigned long*)(PWM_BLOCK + 0x0d8) // PWM2 Compare A
#define PWM2CMPB                    *(unsigned long*)(PWM_BLOCK + 0x0dc) // PWM2 Compare B
#define PWM2GENA                    *(unsigned long*)(PWM_BLOCK + 0x0e0) // PWM2 Generator A Control
#define PWM2GENB                    *(unsigned long*)(PWM_BLOCK + 0x0e4) // PWM2 Generator B Control
#define PWM2DBCTL                   *(unsigned long*)(PWM_BLOCK + 0x0e8) // PWM2 Dead-Band Control
#define PWM2DBRISE                  *(unsigned long*)(PWM_BLOCK + 0x0ec) // PWM2 Dead-Band Rising Edge Delay
#define PWM2DBRALL                  *(unsigned long*)(PWM_BLOCK + 0x0f0) // PWM2 Dead-Band Falling Edge Delay
#define PWM2FLTSRC0                 *(unsigned long*)(PWM_BLOCK + 0x0f4) // PWM2 Fault Source 0
#define PWM2MINFLTPER               *(unsigned long*)(PWM_BLOCK + 0x0fc) // PWM2 Minimum Fault Period

#define PWM3CTL                     *(unsigned long*)(PWM_BLOCK + 0x100) // PWM3 Control
#define PWM3INTEN                   *(unsigned long*)(PWM_BLOCK + 0x104) // PWM3 Interrupt and Trigger Enable
#define PWM3RIS                     *(volatile unsigned long*)(PWM_BLOCK + 0x108) // PWM3 Raw Interrupt Status
#define PWM3ISC                     *(volatile unsigned long*)(PWM_BLOCK + 0x10c) // PWM3 Interrupt Status and Clear (read-only - write 1 to clear)
#define PWM3LOAD                    *(unsigned long*)(PWM_BLOCK + 0x110) // PWM3 Load
#define PWM3COUNT                   *(volatile unsigned long*)(PWM_BLOCK + 0x114) // PWM3 Counter
#define PWM3CMPA                    *(unsigned long*)(PWM_BLOCK + 0x118) // PWM3 Compare A
#define PWM3CMPB                    *(unsigned long*)(PWM_BLOCK + 0x11c) // PWM3 Compare B
#define PWM3GENA                    *(unsigned long*)(PWM_BLOCK + 0x120) // PWM3 Generator A Control
#define PWM3GENB                    *(unsigned long*)(PWM_BLOCK + 0x124) // PWM3 Generator B Control
#define PWM3DBCTL                   *(unsigned long*)(PWM_BLOCK + 0x128) // PWM3 Dead-Band Control
#define PWM3DBRISE                  *(unsigned long*)(PWM_BLOCK + 0x12c) // PWM3 Dead-Band Rising Edge Delay
#define PWM3DBRALL                  *(unsigned long*)(PWM_BLOCK + 0x130) // PWM3 Dead-Band Falling Edge Delay
#define PWM3FLTSRC0                 *(unsigned long*)(PWM_BLOCK + 0x134) // PWM3 Fault Source 0
#define PWM3MINFLTPER               *(unsigned long*)(PWM_BLOCK + 0x13c) // PWM3 Minimum Fault Period

#define PWM0FLTSEN                  *(unsigned long*)(PWM_BLOCK + 0x800) // PWM0 Fault Pin Logic Sense
#define PWM0FLTSTAT0                *(volatile unsigned long*)(PWM_BLOCK + 0x804) // PWM0 Fault Status 0
#define PWM1FLTSEN                  *(unsigned long*)(PWM_BLOCK + 0x880) // PWM1 Fault Pin Logic Sense
#define PWM1FLTSTAT0                *(volatile unsigned long*)(PWM_BLOCK + 0x884) // PWM1 Fault Status 0
#define PWM2FLTSEN                  *(unsigned long*)(PWM_BLOCK + 0x900) // PWM2 Fault Pin Logic Sense
#define PWM2FLTSTAT0                *(volatile unsigned long*)(PWM_BLOCK + 0x904) // PWM2 Fault Status 0
#define PWM3FLTSTAT0                *(volatile unsigned long*)(PWM_BLOCK + 0x984) // PWM3 Fault Status 0

// CAN controller                                                        {5}
//
#define CANCTL                      *(unsigned long*)(CAN_BLOCK + 0x000) // CAN Control
#define CANSTS                      *(unsigned long*)(CAN_BLOCK + 0x004) // CAN Status
#define CANERR                      *(volatile unsigned long*)(CAN_BLOCK + 0x008) // CAN Error Counter
#define CANBIT                      *(unsigned long*)(CAN_BLOCK + 0x00c) // CAN Bit Timing
#define CANINT                      *(volatile unsigned long*)(CAN_BLOCK + 0x010) // CAN Interrupt
#define CANTST                      *(unsigned long*)(CAN_BLOCK + 0x014) // CAN Test
#define CANBRPE                     *(unsigned long*)(CAN_BLOCK + 0x018) // CAN Baud Rate Prescaler Extension
#define CANIF1CRQ                   *(unsigned long*)(CAN_BLOCK + 0x020) // CAN IF1 Command Request
#define CANIF1CMSK                  *(unsigned long*)(CAN_BLOCK + 0x024) // CAN IF1 Command Mask
#define CANIF1MSK1                  *(unsigned long*)(CAN_BLOCK + 0x028) // CAN IF1 Mask 1
#define CANIF1MSK2                  *(unsigned long*)(CAN_BLOCK + 0x02c) // CAN IF1 Mask 2
#define CANIF1ARB1                  *(unsigned long*)(CAN_BLOCK + 0x030) // CAN IF1 Arbitration 1
#define CANIF1ARB2                  *(unsigned long*)(CAN_BLOCK + 0x034) // CAN IF1 Arbitration 2
#define CANIF1MCTL                  *(unsigned long*)(CAN_BLOCK + 0x038) // CAN IF1 Message Control
#define CANIF1DA1                   *(unsigned long*)(CAN_BLOCK + 0x03c) // CAN IF1 Data A1
#define CANIF1DA2                   *(unsigned long*)(CAN_BLOCK + 0x040) // CAN IF1 Data A2
#define CANIF1DB1                   *(unsigned long*)(CAN_BLOCK + 0x044) // CAN IF1 Data B1
#define CANIF1DB2                   *(unsigned long*)(CAN_BLOCK + 0x048) // CAN IF1 Data B2
#define CANIF2CRQ                   *(unsigned long*)(CAN_BLOCK + 0x080) // CAN IF2 Command Request
#define CANIF2CMSK                  *(unsigned long*)(CAN_BLOCK + 0x084) // CAN IF2 Command Mask
#define CANIF2MSK1                  *(unsigned long*)(CAN_BLOCK + 0x088) // CAN IF2 Mask 1
#define CANIF2MSK2                  *(unsigned long*)(CAN_BLOCK + 0x08c) // CAN IF2 Mask 2
#define CANIF2ARB1                  *(unsigned long*)(CAN_BLOCK + 0x090) // CAN IF2 Arbitration 1
#define CANIF2ARB2                  *(unsigned long*)(CAN_BLOCK + 0x094) // CAN IF2 Arbitration 2
#define CANIF2MCTL                  *(unsigned long*)(CAN_BLOCK + 0x098) // CAN IF2 Message Control
#define CANIF2DA1                   *(unsigned long*)(CAN_BLOCK + 0x09c) // CAN IF2 Data A1
#define CANIF2DA2                   *(unsigned long*)(CAN_BLOCK + 0x0a0) // CAN IF2 Data A2
#define CANIF2DB1                   *(unsigned long*)(CAN_BLOCK + 0x0a4) // CAN IF2 Data B1
#define CANIF2DB2                   *(unsigned long*)(CAN_BLOCK + 0x0a8) // CAN IF2 Data B2
#define CANTXRQ1                    *(volatile unsigned long*)(CAN_BLOCK + 0x100) // CAN Transmission Request 1
#define CANTXRQ2                    *(volatile unsigned long*)(CAN_BLOCK + 0x104) // CAN Transmission Request 2 
#define CANTXNWDA1                  *(volatile unsigned long*)(CAN_BLOCK + 0x120) // CAN New Data 1
#define CANTXNWDA2                  *(volatile unsigned long*)(CAN_BLOCK + 0x124) // CAN New Data 2
#define CANMSG1INT                  *(volatile unsigned long*)(CAN_BLOCK + 0x140) // CAN Message 1 Interrupt Pending
#define CANMSG2INT                  *(volatile unsigned long*)(CAN_BLOCK + 0x144) // CAN Message 2 Interrupt Pending
#define CANMSG1VAL                  *(volatile unsigned long*)(CAN_BLOCK + 0x160) // CAN Message 1 Valid
#define CANMSG2VAL                  *(volatile unsigned long*)(CAN_BLOCK + 0x164) // CAN Message 2 Valid


// Ethernet Controller
//
#define MACRIS_IACK                 *(volatile unsigned long*)(ETHERNET_BLOCK + 0x00) // Ethernet MAC Raw Interrupt Status / Interrupt Acknowledge
  #define RXINT                     0x00000001
  #define TXER                      0x00000002
  #define TXEMP                     0x00000004
  #define FOV                       0x00000008
  #define RXER                      0x00000010
  #define MDINT                     0x00000020
  #define PHYINT                    0x00000040
#define MACIM                       *(volatile unsigned long*)(ETHERNET_BLOCK + 0x04) // Ethernet MAC Interrupt Mask
#define MACRCTL                     *(unsigned long*)(ETHERNET_BLOCK + 0x08)        // Ethernet MAC Receive Control
  #define MAC_RXEN                  0x00000001
  #define MAC_AMUL                  0x00000002                                      // multicast frames
  #define MAC_PRMS                  0x00000004                                      // promiscuous mode
  #define MAC_BADCRC                0x00000008                                      // reject frames with bad CRC
  #define MAC_RSTFIFO               0x00000010
#define MACTCTL                     *(unsigned long*)(ETHERNET_BLOCK + 0x0c)        // Ethernet MAC Transmit Control
  #define MAC_TXEN                  0x00000001
  #define MAC_PADEN                 0x00000002
  #define MAC_TX_CRC                0x00000004
  #define MAC_DUPLEX                0x00000010
#define MACDATA                     *(volatile unsigned long*)(ETHERNET_BLOCK + 0x10) // Ethernet MAC Data
#define MACIA0                      *(unsigned long*)(ETHERNET_BLOCK + 0x14)        // Ethernet MAC Individual Address 0
#define MACIA1                      *(unsigned long*)(ETHERNET_BLOCK + 0x18)        // Ethernet MAC Individual Address 1
#define MACTHR                      *(unsigned long*)(ETHERNET_BLOCK + 0x1c)        // Ethernet MAC Threshold
#define MACMCTL                     *(unsigned long*)(ETHERNET_BLOCK + 0x20)        // Ethernet MAC Management Control
  #define MII_START                 0x00000001
  #define MII_WRITE                 0x00000002
  #define MII_READ                  0x00000000
  #define READ_FROM_PHY             (MII_READ | MII_START)
  #define WRITE_TO_PHY              (MII_WRITE | MII_START)
#define MACMDV                      *(unsigned long*)(ETHERNET_BLOCK + 0x24)        // Ethernet MAC Management Divider
#define MACMTXD                     *(unsigned long*)(ETHERNET_BLOCK + 0x2c)        // Ethernet MAC Management Transmit Data
#define MACMRXD                     *(volatile unsigned long*)(ETHERNET_BLOCK + 0x30) // Ethernet MAC Management Receive Data
#define MACNP                       *(volatile unsigned long*)(ETHERNET_BLOCK + 0x34) // Ethernet MAC Number of Packets
#define MACTR                       *(volatile unsigned long*)(ETHERNET_BLOCK + 0x38) // Ethernet MAC Transmission Request
  #define NEWTX                     0x00000001
#if defined _LM3S8962
    #define MACTS                   *(unsigned long*)(ETHERNET_BLOCK + 0x3c)        // Ethernet MAC Timer Support
#endif
#if defined _TEMPEST_CLASS
    #define MACLED                  *(unsigned long*)(ETHERNET_BLOCK + 0x40)        // Ethernet MAC LED Encoding
    #define MDIX                    *(unsigned long*)(ETHERNET_BLOCK + 0x44)        // Ethernet PHY MDIX
#endif

// Integrated PHYSICAL interface
//
#define LM3SXXXX_BASIC_CONTROL_REGISTER               (0x0 << 3)
//#define PHY_DISABLE_TRANSMITTER                     0x0001
  #define PHY_ENABLE_COLLISION_TEST                   0x0080
  #define PHY_FULL_DUPLEX_MODE                        0x0100
  #define PHY_RESTART_AUTO_NEGOTIATION                0x0200
  #define PHY_ELECTRICALLY_ISOLATE                    0x0400
  #define PHY_POWER_DOWN_MODE                         0x0800
  #define PHY_ENABLE_AUTO_NEGOTIATION                 0x1000
  #define PHY_SELECT_100M_SPEED                       0x2000
  #define PHY_LOOP_BACK                               0x4000
  #define PHY_SOFTWARE_RESET                          0x8000
#define LM3SXXXX_BASIC_STATUS_REGISTER                (0x1 << 3)
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
//#define PHY_100MEG_T4_CAPABLE                       0x8000
#define LM3SXXXX_PHY_IDENTIFIER_1                     (0x2  << 3)
#define LM3SXXXX_PHY_IDENTIFIER_2                     (0x3  << 3)
#define LM3SXXXX_AUTO_NEG_ADVERT_REG                  (0x4  << 3)
#define LM3SXXXX_AUTO_NEG_LINK_PARTNER_ABIL_REG       (0x5  << 3)
#define LM3SXXXX_AUTO_NEG_EXPANSION_REG               (0x6  << 3)
//#define LM3SXXXX_AUTO_NEG_NEXT_PAGE_REG             (0x7  << 3)
//#define LM3SXXXX_LINK_PARTNER_NEXT_PAGE_ABIL        (0x8  << 3)
#define LM3SXXXX_VENDOR_SPECIFIC                      (0x10 << 3)
//#define LM3SXXXX_RXER_COUNTER_REGISTER              (0x15 << 3)
#if defined _TEMPEST_CLASS
    #define LM3SXXXX_SPECIAL_CONTROL_STATUS           (0x1b << 3)
    #define LM3SXXXX_PHY_INTERRUPT_STATUS             (0x1d << 3)
      #define PHY_ENERGYON_INT                            0x0080
      #define PHY_AUTONEG_COMPLETE_INT                    0x0040
      #define PHY_REMOTE_FAULT_INT                        0x0020
      #define PHY_LINK_DOWN_INT                           0x0010
      #define PHY_LINK_PARTNER_ACK_INT                    0x0008
      #define PHY_PARALLEL_DETECT_FAULT_INT               0x0004
      #define PHY_PAGE_RECEIVED_INT                       0x0002
    #define LM3SXXXX_PHY_INTERRUPT_MASK               (0x1e << 3)
      #define PHY_ENERGYON_INT_ENABLE                     0x0080
      #define PHY_AUTONEG_COMPLETE_INT_ENABLE             0x0040
      #define PHY_REMOTE_FAULT_INT_ENABLE                 0x0020
      #define PHY_LINK_DOWN_INT_ENABLE                    0x0010
      #define PHY_LINK_PARTNER_ACK_INT_ENABLE             0x0008
      #define PHY_PARALLEL_DETECT_FAULT_INT_ENABLE        0x0004
      #define PHY_PAGE_RECEIVED_INT_ENABLE                0x0002
    #define LM3SXXXX_PHY_SPECIAL_CONTROL_STATUS       (0x1f << 3)
#else
    #define LM3SXXXX_INTERRUPT_CONTROL_STATUS_REGISTER    (0x11 << 3)
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
    #define LM3SXXXX_DIAGNOSTICS_REGISTER             (0x12 << 3)
    #define LM3SXXXX_TRANSCEIVER_CONTROL              (0x13 << 3)
    #define LM3SXXXX_LED_CONFIGURATION                (0x17 << 3)
    #define LM3SXXXX_MDI_MDIX_CONTROL                 (0x18 << 3)
#endif

#ifdef _TEMPEST_CLASS
    #define PHY_IDENTIFIER                            0x0161b410
#else
    #define PHY_IDENTIFIER                            0x000e7230
#endif
#define PHY_MASK                                      0xfffffff0


// USB Register Map                                                      // {3}
//
#define USBFADDR                    *(unsigned char*)(USB_BLOCK + 0x000)           // USB Device Functional Address (device)
#define USBPOWER                    *(unsigned char*)(USB_BLOCK + 0x001)           // USB Power
  #define PWRDNPHY                  0x01                                           // set to power down the internal PHY
  #define USB_SUSPEND               0x02                                           // read only for device
  #define USB_RESUME                0x04
  #define USB_RESET                 0x08                                           // read only for device
  #define SOFTCONN                  0x40                                           // enable USB D+/D- lines
  #define ISOUP                     0x80                                           // (only isochronous)
#define USBTXIS                     *(volatile unsigned short*)(USB_BLOCK + 0x002) // USB Transmit Interrupt Status (read only)
  #define EP0_TX_RX_INT             0x0001
  #define EP1_TX_INT                0x0002
  #define EP2_TX_INT                0x0004
  #define EP3_TX_INT                0x0008
#define USBRXIS                     *(volatile unsigned short*)(USB_BLOCK + 0x004) // USB Receive Interrupt Status (read only)
  #define EP1_RX_INT                0x0002
  #define EP2_RX_INT                0x0004
  #define EP3_RX_INT                0x0008
#define USBTXIE                     *(unsigned short*)(USB_BLOCK + 0x006)          // USB Transmit Interrupt Enable
#define USBRXIE                     *(unsigned short*)(USB_BLOCK + 0x008)          // USB Receive Interrupt Enable
#define USBIS                       *(volatile unsigned char*)(USB_BLOCK + 0x00a)  // USB General Interrupt Status (read only)
  #define SUSPEND_INT               0x01
  #define RESUME_INT                0x02
  #define RESET_INT                 0x04
  #define SOF_INT                   0x08
  #define DISCON_INT                0x20
  #define SESREQ_INT                0x40                                           // (only OTG A device)
  #define VBUSERR_INT               0x80                                           // (only OTG A device)
#define USBIE                       *(unsigned char*)(USB_BLOCK + 0x00b)           // USB Interrupt Enable
#define USBFRAME                    *(volatile unsigned short*)(USB_BLOCK + 0x00c) // USB Frame Value (read only)
#define USBEPIDX                    *(unsigned char*)(USB_BLOCK + 0x00e)           // USB Endpoint Index
#define USBTEST                     *(unsigned char*)(USB_BLOCK + 0x00f)           // USB Test Mode
#define USBFIFO0_ADD                (volatile unsigned long*)(USB_BLOCK + 0x020)   // USB FIFO Endpoint 0 address
#define USBFIFO0                    *(volatile unsigned long*)(USB_BLOCK + 0x020)  // USB FIFO Endpoint 0
#define USBFIFO1_ADD                (volatile unsigned long*)(USB_BLOCK + 0x024)   // USB FIFO Endpoint 1 address
#define USBFIFO1                    *(volatile unsigned long*)(USB_BLOCK + 0x024)  // USB FIFO Endpoint 1
#define USBFIFO2_ADD                (volatile unsigned long*)(USB_BLOCK + 0x028)   // USB FIFO Endpoint 2 address
#define USBFIFO2                    *(volatile unsigned long*)(USB_BLOCK + 0x028)  // USB FIFO Endpoint 2
#define USBFIFO3_ADD                (volatile unsigned long*)(USB_BLOCK + 0x02c)   // USB FIFO Endpoint 3 address
#define USBFIFO3                    *(volatile unsigned long*)(USB_BLOCK + 0x02c)  // USB FIFO Endpoint 3
#define USBDEVCTL                   *(unsigned char*)(USB_BLOCK + 0x060)           // USB Device Control
#define USBTXFIFOSZ                 *(unsigned char*)(USB_BLOCK + 0x062)           // USB Transmit Dynamic FIFO sizing
  #define FIFO_SIZE_8               0x00
  #define FIFO_SIZE_16              0x01
  #define FIFO_SIZE_32              0x02
  #define FIFO_SIZE_64              0x03
  #define FIFO_SIZE_128             0x04
  #define FIFO_SIZE_256             0x05
  #define FIFO_SIZE_512             0x06
  #define FIFO_SIZE_1024            0x07
  #define FIFO_SIZE_2048            0x08
  #define FIFO_DOUBLE_BUFFERED      0x10
#define USBRXFIFOSZ                 *(unsigned char*)(USB_BLOCK + 0x063)           // USB Receive Dynamic FIFO sizing
#define USBTXFIFOADD                *(unsigned short*)(USB_BLOCK + 0x064)          // USB Transmit FIFO Start Address
/*
  #define FIFO_OFFSET_0             0x00                                           // documentation error - length is size divided by 8
  #define FIFO_OFFSET_8             0x01
  #define FIFO_OFFSET_16            0x02
  #define FIFO_OFFSET_32            0x03
  #define FIFO_OFFSET_64            0x04
  #define FIFO_OFFSET_128           0x05
  #define FIFO_OFFSET_256           0x06
  #define FIFO_OFFSET_512           0x07
  #define FIFO_OFFSET_1024          0x08
  #define FIFO_OFFSET_2048          0x09 */
#define USBRXFIFOADD                *(unsigned short*)(USB_BLOCK + 0x066)          // USB Receive FIFO Start Address
#define USBCONTIM                   *(unsigned char*)(USB_BLOCK + 0x07a)           // USB Connect Timing
#define USBVPLEN                    *(unsigned char*)(USB_BLOCK + 0x07b)           // USB OTG VBus Pulse Timing (OTG)
#define USBFSEOF                    *(unsigned char*)(USB_BLOCK + 0x07d)           // USB Full-Speed Last Transaction to End of Frame Timing
#define USBLSEOF                    *(unsigned char*)(USB_BLOCK + 0x07e)           // USB Low-Speed Last Transaction to End of Frame Timing
#define USBTXFUNCADDR0              *(unsigned char*)(USB_BLOCK + 0x080)           // USB Transmit Function Address Endpoint 0
#define USBTXHUBADDR0               *(unsigned char*)(USB_BLOCK + 0x082)           // USB Transmit Hub Address Endpoint 0
#define USBTXHUBPORT0               *(unsigned char*)(USB_BLOCK + 0x083)           // USB Transmit Hub Port Endpoint 0
#define USBTXFUNCADDR1              *(unsigned char*)(USB_BLOCK + 0x088)           // USB Transmit Function Address Endpoint 1
#define USBTXHUBADDR1               *(unsigned char*)(USB_BLOCK + 0x08a)           // USB Transmit Hub Address Endpoint 1
#define USBTXHUBPORT1               *(unsigned char*)(USB_BLOCK + 0x08b)           // USB Transmit Hub Port Endpoint 1
#define USBRXFUNCADDR1              *(unsigned char*)(USB_BLOCK + 0x08c)           // USB Receive Function Address Endpoint 1
#define USBRXHUBADDR1               *(unsigned char*)(USB_BLOCK + 0x08e)           // USB Receive Hub Address Endpoint 1
#define USBRXHUBPORT1               *(unsigned char*)(USB_BLOCK + 0x08f)           // USB Receive Hub Port Endpoint 1
#define USBTXFUNCADDR2              *(unsigned char*)(USB_BLOCK + 0x090)           // USB Transmit Function Address Endpoint 2
#define USBTXHUBADDR2               *(unsigned char*)(USB_BLOCK + 0x092)           // USB Transmit Hub Address Endpoint 2
#define USBTXHUBPORT2               *(unsigned char*)(USB_BLOCK + 0x093)           // USB Transmit Hub Port Endpoint 2
#define USBRXFUNCADDR2              *(unsigned char*)(USB_BLOCK + 0x094)           // USB Receive Function Address Endpoint 2
#define USBRXHUBADDR2               *(unsigned char*)(USB_BLOCK + 0x096)           // USB Receive Hub Address Endpoint 2
#define USBRXHUBPORT2               *(unsigned char*)(USB_BLOCK + 0x097)           // USB Receive Hub Port Endpoint 2
#define USBTXFUNCADDR3              *(unsigned char*)(USB_BLOCK + 0x098)           // USB Transmit Function Address Endpoint 3
#define USBTXHUBADDR3               *(unsigned char*)(USB_BLOCK + 0x09a)           // USB Transmit Hub Address Endpoint 3
#define USBTXHUBPORT3               *(unsigned char*)(USB_BLOCK + 0x09b)           // USB Transmit Hub Port Endpoint 3
#define USBRXFUNCADDR3              *(unsigned char*)(USB_BLOCK + 0x09c)           // USB Receive Function Address Endpoint 3
#define USBRXHUBADDR3               *(unsigned char*)(USB_BLOCK + 0x09e)           // USB Receive Hub Address Endpoint 3
#define USBRXHUBPORT3               *(unsigned char*)(USB_BLOCK + 0x09f)           // USB Receive Hub Port Endpoint 3
#define USBTXCTRADD0                (volatile unsigned char*)(USB_BLOCK + 0x102)
#define USBCSRL0                    *(volatile unsigned char*)(USB_BLOCK + 0x102)  // USB Control and Status Endpoint 0 Low (write 1 to clear)
  #define EP0_DEVICE_RXRDY          0x01                                           // receive packet ready
  #define EP0_DEVICE_TXRDY          0x02                                           // transmit packet ready
  #define EP0_DATAEND               0x08
  #define EP0_SETEND                0x10
  #define EP0_STALL                 0x20
  #define EP0_RXRDYC                0x40
  #define EP0_SETENDC               0x80
#define USBCSRH0                    *(volatile unsigned char*)(USB_BLOCK + 0x103)  // USB Control and Status Endpoint 0 High (write 1 to clear)
  #define USB_FLUSH                 0x01
#define USBRXCTRADD0                (unsigned char*)(USB_BLOCK + 0x106)            // fictive address for SW use
#define USBRXCOUNTADD0              (volatile unsigned char*)(USB_BLOCK + 0x108)
#define USBCOUNT0                   *(volatile unsigned char*)(USB_BLOCK + 0x108)  // USB Receive Byte Count Endpoint 0 (read only)
#define USBTYPE0                    *(unsigned char*)(USB_BLOCK + 0x10a)           // USB Receive Type Endpoint 0 
#define USBNAKLMT                   *(unsigned char*)(USB_BLOCK + 0x10b)           // USB NAK Limit
#define USBTXMAXP1ADD               (unsigned short*)(USB_BLOCK + 0x110)
#define USBTXMAXP1                  *(unsigned short*)(USB_BLOCK + 0x110)          // USB Maximum Transmit Data Endpoint 1
#define USBTXCTRADD1                (unsigned char*)(USB_BLOCK + 0x112)
#define USBTXCSRL1                  *(unsigned char*)(USB_BLOCK + 0x112)           // USB Transmit Control and Status Endpoint 1 Low
  #define DEVICE_TXRDY              0x01                                           // transmit packet ready
  #define DEVICE_FIFONE             0x02                                           // FIFO not empty
  #define DEVICE_UNDRN              0x04                                           // underrun - set when IN token received without TXRDY being set
  #define DEVICE_FLUSH_FIFO         0x08
  #define DEVICE_STALL              0x10
  #define DEVICE_STALLED            0x20
  #define DEVICE_CLRDT              0x40
  #define DEVICE_INCOMPLETE_ISO_TX  0x80*/
#define USBTXCSRH1ADD               (unsigned char*)(USB_BLOCK + 0x113)
#define USBTXCSRH1                  *(unsigned char*)(USB_BLOCK + 0x113)           // USB Transmit Control and Status Endpoint 1 High
  #define USB_ISO                   0x40                                           // set for isochronous mode and cleared for bulk and interrupt
  #define USB_TX                    0x20
  #define USB_DMA                   0x10
  #define FDT                       0x08
  #define USB_DMA_REQ_MODE_1        0x04
#define USBRXMAXP1ADD               (unsigned short*)(USB_BLOCK + 0x114)
#define USBRXMAXP1                  *(unsigned short*)(USB_BLOCK + 0x114)          // USB Maximum Receive Data Endpoint 1
#define USBRXCTRADD1                (unsigned char*)(USB_BLOCK + 0x116)
#define USBRXCSRL1                  *(unsigned char*)(USB_BLOCK + 0x116)           // USB Receive Control and Status Endpoint 1 Low
  #define DEVICE_RXRDY              0x01                                           // receive packet ready
#define USBRXCSRH1ADD               (unsigned char*)(USB_BLOCK + 0x117)
#define USBRXCSRH1                  *(unsigned char*)(USB_BLOCK + 0x117)           // USB Receive Control and Status Endpoint 1 High
  #define USB_DMA_RX_EN             0x20
  #define DISNYET_PIDERR            0x10
#define USBRXCOUNTADD1              (volatile unsigned char*)(USB_BLOCK + 0x118)
#define USBRXCOUNT1                 *(volatile unsigned short*)(USB_BLOCK + 0x118) // USB Receive Byte Count Endpoint 1 (read only)
#define USBTXTYPE1                  *(unsigned char*)(USB_BLOCK + 0x11a)           // USB Host Transmit Configure Type Endpoint 1
#define USBTXINTERVAL1              *(unsigned char*)(USB_BLOCK + 0x11b)           // USB Host Transmit Interval Endpoint 1
#define USBRCTYPE1                  *(unsigned char*)(USB_BLOCK + 0x11c)           // USB Host Receive Configure Type Endpoint 1
#define USBRXINTERVAL1              *(unsigned char*)(USB_BLOCK + 0x11d)           // USB Host Receive Polling Interval Endpoint 1
#define USBTXMAXP2ADD               (unsigned short*)(USB_BLOCK + 0x120)
#define USBTXMAXP2                  *(unsigned short*)(USB_BLOCK + 0x120)          // USB Maximum Transmit Data Endpoint 2
#define USBTXCTRADD2                (unsigned char*)(USB_BLOCK + 0x122)
#define USBTXCSRL2                  *(unsigned char*)(USB_BLOCK + 0x122)           // USB Transmit Control and Status Endpoint 2 Low
#define USBTXCSRH2                  *(unsigned char*)(USB_BLOCK + 0x123)           // USB Transmit Control and Status Endpoint 2 High
#define USBRXMAXP2                  *(unsigned short*)(USB_BLOCK + 0x124)          // USB Maximum Receive Data Endpoint 2
#define USBRXCTRADD2                (unsigned char*)(USB_BLOCK + 0x126)
#define USBRXCSRL2                  *(unsigned char*)(USB_BLOCK + 0x126)           // USB Receive Control and Status Endpoint 2 Low
#define USBRXCSRH2                  *(unsigned char*)(USB_BLOCK + 0x127)           // USB Receive Control and Status Endpoint 2 High
#define USBRXCOUNT2                 *(volatile unsigned short*)(USB_BLOCK + 0x128) // USB Receive Byte Count Endpoint 2 (read only)
#define USBTXTYPE2                  *(unsigned char*)(USB_BLOCK + 0x12a)           // USB Host Transmit Configure Type Endpoint 2
#define USBTXINTERVAL2              *(unsigned char*)(USB_BLOCK + 0x12b)           // USB Host Transmit Interval Endpoint 2
#define USBRCTYPE2                  *(unsigned char*)(USB_BLOCK + 0x12c)           // USB Host Receive Configure Type Endpoint 2
#define USBRXINTERVAL2              *(unsigned char*)(USB_BLOCK + 0x12d)           // USB Host Receive Polling Interval Endpoint 2
#define USBTXMAXP3                  *(unsigned short*)(USB_BLOCK + 0x130)          // USB Maximum Transmit Data Endpoint 3
#define USBTXCTRADD3                (unsigned char*)(USB_BLOCK + 0x132)
#define USBTXCSRL3                  *(unsigned char*)(USB_BLOCK + 0x132)           // USB Transmit Control and Status Endpoint 3 Low
#define USBTXCSRH3                  *(unsigned char*)(USB_BLOCK + 0x133)           // USB Transmit Control and Status Endpoint 3 High
#define USBRXMAXP3                  *(unsigned short*)(USB_BLOCK + 0x134)          // USB Maximum Receive Data Endpoint 3
#define USBRXCTRADD3                (unsigned char*)(USB_BLOCK + 0x136)
#define USBRXCSRL3                  *(unsigned char*)(USB_BLOCK + 0x136)           // USB Receive Control and Status Endpoint 3 Low
#define USBRXCSRH3                  *(unsigned char*)(USB_BLOCK + 0x137)           // USB Receive Control and Status Endpoint 3 High
#define USBRXCOUNT3                 *(volatile unsigned short*)(USB_BLOCK + 0x138) // USB Receive Byte Count Endpoint 3 (read only)
#define USBTXTYPE3                  *(unsigned char*)(USB_BLOCK + 0x13a)           // USB Host Transmit Configure Type Endpoint 3
#define USBTXINTERVAL3              *(unsigned char*)(USB_BLOCK + 0x13b)           // USB Host Transmit Interval Endpoint 3
#define USBRCTYPE3                  *(unsigned char*)(USB_BLOCK + 0x13c)           // USB Host Receive Configure Type Endpoint 3
#define USBRXINTERVAL3              *(unsigned char*)(USB_BLOCK + 0x13d)           // USB Host Receive Polling Interval Endpoint 3
#define USBRQPKTCOUNT1              *(unsigned short*)(USB_BLOCK + 0x304)          // USB Request Packet Count in Block Transfer Endpoint 1
#define USBRQPKTCOUNT2              *(unsigned short*)(USB_BLOCK + 0x308)          // USB Request Packet Count in Block Transfer Endpoint 2
#define USBRQPKTCOUNT3              *(unsigned short*)(USB_BLOCK + 0x30c)          // USB Request Packet Count in Block Transfer Endpoint 3
#define USBRXDPKTBUFDIS             *(unsigned short*)(USB_BLOCK + 0x340)          // USB Receive Double Packet Buffer Disable
#define USBTXDPKTBUFDIS             *(unsigned short*)(USB_BLOCK + 0x342)          // USB Transmit Double Packet Buffer Disable
#define USBEPC                      *(unsigned long*)(USB_BLOCK + 0x400)           // USB External Power Control
#define USBEPCRIS                   *(volatile unsigned long*)(USB_BLOCK + 0x404)  // USB External Power Control Raw Interrupt Status (read only)
#define USBEPCIM                    *(unsigned long*)(USB_BLOCK + 0x408)           // USB External Power Control Interrupt Mask
#define USBEPCISC                   *(volatile unsigned long*)(USB_BLOCK + 0x40c)  // USB External Power Control Interrupt Status and Clear
#define USBDRRIS                    *(volatile unsigned long*)(USB_BLOCK + 0x410)  // USB Device Resume Raw Interrupt Status (read only)
#define USBDRIM                     *(unsigned long*)(USB_BLOCK + 0x414)           // USB Device Resume Interrupt Mask
#define USBDRISC                    *(volatile unsigned long*)(USB_BLOCK + 0x418)  // USB Device Resume Interrupt Status and Clear (write one to clear)
#define USBGPCS                     *(unsigned long*)(USB_BLOCK + 0x41c)           // USB General Purpose Control and Status (not OTG)
  #define DEVMOD                    0x00000001                                     // device mode

#ifdef _WINDOWS
    extern void fnGetUSB_FIFO_config(void);
    extern void fnPutUSB_FIFO_config(void);
    extern void fnFIFO_buffer_out(int iEndpoint);
    extern volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned long *ptrAdd, int iDirection);
    extern void fnPutUSB_FIFOLength(int iEndpoint, unsigned short usLength);
    #define CLEAR_RXRDY()      USBCSRL0 = (EP0_RXRDYC /*| EP0_DATAEND*/); USBCOUNT0 = 0;// acknowledge reception of setup on endpoint 0
    #define PARTIAL_CLEAR_RXRDY() USBCSRL0 = EP0_RXRDYC; USBCOUNT0 = 0;// acknowledge reception of setup on endpoint 0
  //#define CLEAR_RXRDY(iEndpoint_ref)  if (iEndpoint_ref == 0) { USBCSRL0 &= ~DEVICE_RXRDY; USBCOUNT0 = 0; } else {*(USBRXCTRADD0 + (iEndpoint_ref * (USBRXCTRADD1 - USBRXCTRADD0))) &= ~DEVICE_RXRDY; *(unsigned short *)(USBRXCOUNTADD0 + iEndpoint_ref * (USBRXCOUNTADD1 - USBRXCOUNTADD0)) = 0;} 
#else
    #define fnGetUSB_FIFOAdd(x,y)       x
  //#define CLEAR_RXRDY(iEndpoint_ref)  if (iEndpoint_ref == 0) { USBCSRL0 = RXRDYC;} else { *(USBRXCTRADD0 + (iEndpoint_ref * (USBRXCTRADD1 - USBRXCTRADD0))) = RXRDYC; } 
    #define CLEAR_RXRDY()      USBCSRL0 = (EP0_RXRDYC/* | EP0_DATAEND*/);           // acknowledge reception of setup on endpoint 0
    #define PARTIAL_CLEAR_RXRDY() USBCSRL0 = EP0_RXRDYC;
#endif
#define CLEAR_SETUP(ptrUSB_HW)                                           // not required

typedef struct stUSB_END_POINT
{
    unsigned short usEndpointSize;                                        // contains size of endpoint plus some control flags
    unsigned char ucEndpointTxFlags;
    unsigned char ucEndpointRxFlags;
} USB_END_POINT;

#define ENDPOINT_DUAL_BANK   0x01


#define ALTERNATE_TX_BUFFER    0                                         // any bit not clashing with the length in the buffer descriptor control word (used as SW flag)
#define RX_DATA_TOGGLE         0                                         // any bit not clashing with DATA_1 in the buffer descriptor control word (used as SW flag)

#define USB_FIFO                                                         // LM3Sxxxx has USB buffers realised as FIFOs
#define FIFO_TO_QUEUE                                                    // activate queue driver support to allow FIFO copy to a circular buffer
#define PROTECT_USB_TX_BUFFER                                            // use additional tx buffer protection to avoid ping-pong buffer overrun. Register status alone is not reliable.
#define USB_FIFO_BUFFER_DEPTH              2
#define NUMBER_OF_USB_ENDPOINTS            4                             // supports 4 endpoints
#define MAX_FIFO_LENGTH                    1024

// This hardware specific structure allows generic USB routines to control specific hardware using macros
//
typedef struct stUSB_HW
{
    unsigned long  ulRxControl;
    volatile unsigned char *ptr_ucUSB_BDControl;                         // pointer to the reception control register
    USB_END_POINT *ptrEndpoint;                                          // pointer to endpoint details
    volatile unsigned char *ptrFIFO;                                     // the rx/tx FIFO 
    unsigned short usLength;                                             // reception length information
    unsigned char  ucUSBAddress;                                         // our USB address
    unsigned char  usEP0State;                                           // state of endpoint 0
} USB_HW;

#define USB_STATE_IDLE       0x00                                        // endpoint 0 is idle and waiting for a SETUP token
#define USB_STATE_RX         0x01                                        // endpoint 0 has received a SETUP token and is now expecting further OUT data
#define USB_STATE_TX         0x02                                        // endpoint 0 is sending multiple packets
#define USB_STATE_LAST_TX    0x04                                        // endpoint 0 is sending a single packet or the last of multiple packtets and expecting the last ack before the status transfer
#define USB_STATE_STATUS     0x08                                        // endpoint 0 is sending a single packet or the last of multiple packets and expecting the status stage reception
#define USB_STATE_STALL      0x10                                        // endpoint 0 has sent a stall and the status handshake is expected
#define USB_STATE_ADDRESSING 0x20                                        // endpoint 0 has just acknowledged with a zero packed and will set the address when this has been acknowledged
#define USB_STATE_OUT        0x40                                        // endpoint 0 has received part of a set up packet and is now expecting further OUT data


#define FNSEND_USB_DATA(buffer_addr, length, Endpoint, hw)        fnSendUSB_FIFO(buffer_addr, length, Endpoint, hw); _SIM_USB(USB_SIM_TX, Endpoint, hw)
#define FNSEND_ZERO_DATA(hw, Endpoint)                            fnSendUSB_FIFO(0, 0, Endpoint, hw);                _SIM_USB(USB_SIM_TX, Endpoint, hw)

//if (iEndpoint_ref == 0) { USBCSRL0 = (/*DATAEND | */DEVICE_TXRDY);} else { *(USBTXCTRADD0 + (iEndpoint_ref * (USBTXCTRADD1 - USBTXCTRADD0))) = (DATAEND | DEVICE_TXRDY); } _SIM_USB(USB_SIM_TX, iEndpoint_ref, ptrUSB_HW)

#define SET_CONTROL_DIRECTION(ptrUSB_HW, ucFirstByte)           //


// SSI Register Map
//
#define SSICR0_0                    *(unsigned long*)(SSI0_BLOCK + 0x000)                 // SSI Control 0
  #define DSS_4_BIT                 0x00000003
  #define DSS_5_BIT                 0x00000004
  #define DSS_6_BIT                 0x00000005
  #define DSS_7_BIT                 0x00000006
  #define DSS_8_BIT                 0x00000007
  #define DSS_9_BIT                 0x00000008
  #define DSS_10_BIT                0x00000009
  #define DSS_11_BIT                0x0000000a
  #define DSS_12_BIT                0x0000000b
  #define DSS_13_BIT                0x0000000c
  #define DSS_14_BIT                0x0000000d
  #define DSS_15_BIT                0x0000000e
  #define DSS_16_BIT                0x0000000f
  #define FRS_FREESCALE_SPI         0x00000000
  #define FRS_TI_SSFF               0x00000010
  #define FRS_MICROWIRE             0x00000020
  #define SSI_SPO                   0x00000040                                            // serial clock polarity
  #define SSI_SPH                   0x00000080                                            // serial clock phase
  #define SSI_SCR_SHIFT                      8
#define SSICR1_0                    *(unsigned long*)(SSI0_BLOCK + 0x004)                 // SSI Control 1
  #define SSI_LBM                   0x00000001                                            // Loop-back mode
  #define SSI_SSE                   0x00000002                                            // SSI Enable
  #define SSI_MS                    0x00000004                                            // Slave mode
  #define SSI_SOD                   0x00000008                                            // Slave mode output disable
#define SSIDR_0                     *(volatile unsigned long*)(SSI0_BLOCK + 0x008)        // SSI Data
#define SSISR_0                     *(volatile unsigned long*)(SSI0_BLOCK + 0x00c)        // SSI Status (Read-only)
  #define SSI_BSY                   0x00000010                                            // SSI Busy bit
  #define SSI_RFF                   0x00000008                                            // SSI Receive FIFO is full
  #define SSI_RNE                   0x00000004                                            // SSI Receive FIFO not empty
  #define SSI_TNF                   0x00000002                                            // SSI Transmit FIFO not full
  #define SSI_TFE                   0x00000001                                            // SSI Transmit FIFO is empty
#define SSICPSR_0                   *(unsigned long*)(SSI0_BLOCK + 0x010)                 // SSI Clock Prescale
#define SSIIM_0                     *(unsigned long*)(SSI0_BLOCK + 0x014)                 // SSI Interrupt Mask
  #define SSI_TXIM                  0x00000008                                            // SSI Transmit FIFO half full, or less interrupt not masked
  #define SSI_RXIM                  0x00000004                                            // SSI Receive FIFO half-full, or more interrupt not masked
  #define SSI_RTIM                  0x00000002                                            // SSI Receive FIFO timeout interrupt not masked
  #define SSI_RORIM                 0x00000001                                            // SSI Receive FIFO over-run interrupt not masked
#define SSIRIS_0                    *(volatile unsigned long*)(SSI0_BLOCK + 0x018)        // SSI RAW Interrupt Status (Read-only)
  #define SSI_TXRIS                 0x00000008                                            // SSI Transmit FIFO half full, or less RAW interrupt status
  #define SSI_RXRIS                 0x00000004                                            // SSI Receive FIFO half-full, or more RAW interrupt status
  #define SSI_RTRIS                 0x00000002                                            // SSI Receive FIFO timeout RAW interrupt status
  #define SSI_RORRIS                0x00000001                                            // SSI Receive FIFO over-run RAW interrupt status
#define SSIMIS_0                    *(volatile unsigned long*)(SSI0_BLOCK + 0x01c)        // SSI Masked Interrupt Status (Read-only)
  #define SSI_TXMIS                 0x00000008                                            // SSI Transmit FIFO half full, or less masked interrupt status
  #define SSI_RXMIS                 0x00000004                                            // SSI Receive FIFO half-full, or more masked interrupt status
  #define SSI_RTMIS                 0x00000002                                            // SSI Receive FIFO timeout masked interrupt status
  #define SSI_RORMIS                0x00000001                                            // SSI Receive FIFO over-run masked interrupt status
#define SSIICR_0                    *(volatile unsigned long*)(SSI0_BLOCK + 0x020)        // SSI Interrupt Clear (write bits)
  #define SSI_RTIC                  0x00000002                                            // SSI Receive FIFO timeout interrupt clear
  #define SSI_RORIC                 0x00000001                                            // SSI Receive FIFO over-run interrupt clear

#define SSIPeriphID4_0              *(unsigned long*)(SSI0_BLOCK + 0xfd0)                 // SSI 0 Peripheral Identification 4 (read-only)
#define SSIPeriphID5_0              *(unsigned long*)(SSI0_BLOCK + 0xfd4)                 // SSI 0 Peripheral Identification 5 (read-only)
#define SSIPeriphID6_0              *(unsigned long*)(SSI0_BLOCK + 0xfd8)                 // SSI 0 Peripheral Identification 6 (read-only)
#define SSIPeriphID7_0              *(unsigned long*)(SSI0_BLOCK + 0xfdc)                 // SSI 0 Peripheral Identification 7 (read-only)
#define SSIPeriphID0_0              *(unsigned long*)(SSI0_BLOCK + 0xfe0)                 // SSI 0 Peripheral Identification 0 (read-only)
#define SSIPeriphID1_0              *(unsigned long*)(SSI0_BLOCK + 0xfe4)                 // SSI 0 Peripheral Identification 1 (read-only)
#define SSIPeriphID2_0              *(unsigned long*)(SSI0_BLOCK + 0xfe8)                 // SSI 0 Peripheral Identification 2 (read-only)
#define SSIPeriphID3_0              *(unsigned long*)(SSI0_BLOCK + 0xfec)                 // SSI 0 Peripheral Identification 3 (read-only)
#define SSIPCellID0_0               *(unsigned long*)(SSI0_BLOCK + 0xff0)                 // SSI 0 Peripheral Identification 0 (read-only)
#define SSIPCellID1_0               *(unsigned long*)(SSI0_BLOCK + 0xff4)                 // SSI 0 Peripheral Identification 1 (read-only)
#define SSIPCellID2_0               *(unsigned long*)(SSI0_BLOCK + 0xff8)                 // SSI 0 Peripheral Identification 2 (read-only)
#define SSIPCellID3_0               *(unsigned long*)(SSI0_BLOCK + 0xffc)                 // SSI 0 Peripheral Identification 3 (read-only)

#define SSICR0_1                    *(unsigned long*)(SSI1_BLOCK + 0x000)                 // {6} SSI 1 Control 0
#define SSICR1_1                    *(unsigned long*)(SSI1_BLOCK + 0x004)                 // SSI 1 Control 1
#define SSIDR_1                     *(volatile unsigned long*)(SSI1_BLOCK + 0x008)        // SSI 1 Data
#define SSISR_1                     *(volatile unsigned long*)(SSI1_BLOCK + 0x00c)        // SSI 1 Status (Read-only)
#define SSICPSR_1                   *(unsigned long*)(SSI1_BLOCK + 0x010)                 // SSI 1 Clock Prescale
#define SSIIM_1                     *(unsigned long*)(SSI1_BLOCK + 0x014)                 // SSI 1 Interrupt Mask
#define SSIRIS_1                    *(volatile unsigned long*)(SSI1_BLOCK + 0x018)        // SSI 1 RAW Interrupt Status (Read-only)
#define SSIMIS_1                    *(volatile unsigned long*)(SSI1_BLOCK + 0x01c)        // SSI 1 Masked Interrupt Status (Read-only)
#define SSIICR_1                    *(volatile unsigned long*)(SSI1_BLOCK + 0x020)        // SSI 1 Interrupt Clear (write bits)
#define SSIPeriphID4_1              *(unsigned long*)(SSI1_BLOCK + 0xfd0)                 // SSI 1 Peripheral Identification 4 (read-only)
#define SSIPeriphID5_1              *(unsigned long*)(SSI1_BLOCK + 0xfd4)                 // SSI 1 Peripheral Identification 5 (read-only)
#define SSIPeriphID6_1              *(unsigned long*)(SSI1_BLOCK + 0xfd8)                 // SSI 1 Peripheral Identification 6 (read-only)
#define SSIPeriphID7_1              *(unsigned long*)(SSI1_BLOCK + 0xfdc)                 // SSI 1 Peripheral Identification 7 (read-only)
#define SSIPeriphID0_1              *(unsigned long*)(SSI1_BLOCK + 0xfe0)                 // SSI 1 Peripheral Identification 0 (read-only)
#define SSIPeriphID1_1              *(unsigned long*)(SSI1_BLOCK + 0xfe4)                 // SSI 1 Peripheral Identification 1 (read-only)
#define SSIPeriphID2_1              *(unsigned long*)(SSI1_BLOCK + 0xfe8)                 // SSI 1 Peripheral Identification 2 (read-only)
#define SSIPeriphID3_1              *(unsigned long*)(SSI1_BLOCK + 0xfec)                 // SSI 1 Peripheral Identification 3 (read-only)
#define SSIPCellID0_1               *(unsigned long*)(SSI1_BLOCK + 0xff0)                 // SSI 1 Peripheral Identification 0 (read-only)
#define SSIPCellID1_1               *(unsigned long*)(SSI1_BLOCK + 0xff4)                 // SSI 1 Peripheral Identification 1 (read-only)
#define SSIPCellID2_1               *(unsigned long*)(SSI1_BLOCK + 0xff8)                 // SSI 1 Peripheral Identification 2 (read-only)
#define SSIPCellID3_1               *(unsigned long*)(SSI1_BLOCK + 0xffc)                 // SSI 1 Peripheral Identification 3 (read-only)


// UART Register Map
//
#define UARTDR_0_ADDR               (volatile unsigned long*)(UART_0_BLOCK + 0x000)
#define UARTDR_0                    *(volatile unsigned long*)(UART_0_BLOCK + 0x000)      // UART 0 Data
  #define UARTDR_FE                 0x00000100                                            // framing error
  #define UARTDR_PE                 0x00000200                                            // parity error
  #define UARTDR_BE                 0x00000400                                            // break error
  #define UARTDR_OE                 0x00000800                                            // overrun error
#define UARTRSR_ECR_0               *(volatile unsigned long*)(UART_0_BLOCK + 0x004)      // UART 0 Receive Status / Error Clear
  #define UART_FE                   0x00000001                                            // framing error
  #define UART_PE                   0x00000002                                            // parity error
  #define UART_BE                   0x00000004                                            // break error
  #define UART_OE                   0x00000008                                            // overrun error
#define UARTFR_0                    *(volatile unsigned long*)(UART_0_BLOCK + 0x018)      // UART 0 Flag (read-only)
  #define UARTBUSY                  0x00000008                                            // UART busy
  #define UARTRXFE                  0x00000010                                            // receive FIFO empty
  #define UARTTXFF                  0x00000020                                            // transmit FIFO full
  #define UARTRXFF                  0x00000040                                            // receive FIFO full
  #define UARTTXFE                  0x00000080                                            // transmit FIFO empty
#define UARTILPR_0                  *(unsigned long*)(UART_0_BLOCK + 0x020)               // UART 0 IrDA Low-Power Register
#define UARTIBRD_0                  *(unsigned long*)(UART_0_BLOCK + 0x024)               // UART 0 Integer Baud-Rate Divisor
#define UARTFBRD_0                  *(unsigned long*)(UART_0_BLOCK + 0x028)               // UART 0 Fractional Baud-Rate Divisor
#define UARTLCRH_0                  *(unsigned long*)(UART_0_BLOCK + 0x02c)               // UART 0 Line Control
  #define BREAK_CONTROL             0x00000001                                            // force break on TXD line
  #define UART_PARITY_DISABLED      0x00000000                                            // disable parity
  #define UART_PARITY_ENABLE        0x00000002                                            // enable parity
  #define UART_PARITY_ODD           0x00000000                                            // odd parity
  #define UART_PARITY_EVEN          0x00000004                                            // set even parity mode
  #define UART_1_STOP_BITS          0x00000000                                            // set 1 stop bits
  #define UART_2_STOP_BITS          0x00000008                                            // set 2 stop bits
  #define UART_ENABLE_FIFOS         0x00000010                                            // enable FIFO operating mode
  #define UART_5_BIT_CHAR           0x00000000                                            // set 5 bit characters
  #define UART_6_BIT_CHAR           0x00000020                                            // set 6 bit characters
  #define UART_7_BIT_CHAR           0x00000040                                            // set 7 bit characters
  #define UART_8_BIT_CHAR           0x00000060                                            // set 8 bit characters
  #define UART_STICK_PARITY         0x00000080                                            // enable stick parity
#define UARTCTL_0                   *(unsigned long*)(UART_0_BLOCK + 0x030)               // UART 0 Control
  #define UARTEN                    0x00000001                                            // enable UART
  #define SIREN                     0x00000002                                            // IrDA SIR block enable
  #define SIRLP                     0x00000004                                            // UART SIR low power mode
  #define UARTLBE                   0x00000080                                            // loop back enable
  #define UARTTXEN                  0x00000100                                            // UART transmit enable
  #define UARTRXEN                  0x00000200                                            // UART receive enable
#define UARTIFLS_0                  *(unsigned long*)(UART_0_BLOCK + 0x034)               // UART 0 Interrupt FIFO Level Select
#define UARTIM_0                    *(unsigned long*)(UART_0_BLOCK + 0x038)               // UART 0 Interrupt Mask
  #define UART_RXIM                 0x00000010                                            // receive interrupt mask
  #define UART_TXIM                 0x00000020                                            // transmit interrupt mask
  #define UART_RTIM                 0x00000040                                            // receiver time-out interrupt mask
  #define UART_FEIM                 0x00000080                                            // framing error interrupt mask
  #define UART_PEIM                 0x00000100                                            // parity error interrupt mask
  #define UART_BEIM                 0x00000200                                            // break error interrupt mask
  #define UART_OEIM                 0x00000400                                            // overrun error interrupt mask
#define UARTRIS_0                   *(volatile unsigned long*)(UART_0_BLOCK + 0x03c)      // UART 0 RAW Interrupt Status (read-only)
#define UARTMIS_0                   *(volatile unsigned long*)(UART_0_BLOCK + 0x040)      // UART 0 Masked Interrupt Status (read-only)
#define UARTICR_0                   *(volatile unsigned long*)(UART_0_BLOCK + 0x044)      // UART 0 Interrupt Clear
#define UARTDMACTL_0                *(unsigned long*)(UART_0_BLOCK + 0x048)               // UART 0 DMA Control
  #define RXDMAE                    0x00000001                                            // receive DMA enable
  #define TXDMAE                    0x00000002                                            // transmit DMA enable
  #define DMAERR                    0x00000004                                            // receive requests automatically disabled when receive errors occur
#define UARTPeriphID4_0             *(unsigned long*)(UART_0_BLOCK + 0xfd0)               // UART 0 Peripheral Identification 4 (read-only)
#define UARTPeriphID5_0             *(unsigned long*)(UART_0_BLOCK + 0xfd4)               // UART 0 Peripheral Identification 5 (read-only)
#define UARTPeriphID6_0             *(unsigned long*)(UART_0_BLOCK + 0xfd8)               // UART 0 Peripheral Identification 6 (read-only)
#define UARTPeriphID7_0             *(unsigned long*)(UART_0_BLOCK + 0xfdc)               // UART 0 Peripheral Identification 7 (read-only)
#define UARTPeriphID0_0             *(unsigned long*)(UART_0_BLOCK + 0xfe0)               // UART 0 Peripheral Identification 0 (read-only)
#define UARTPeriphID1_0             *(unsigned long*)(UART_0_BLOCK + 0xfe4)               // UART 0 Peripheral Identification 1 (read-only)
#define UARTPeriphID2_0             *(unsigned long*)(UART_0_BLOCK + 0xfe8)               // UART 0 Peripheral Identification 2 (read-only)
#define UARTPeriphID3_0             *(unsigned long*)(UART_0_BLOCK + 0xfec)               // UART 0 Peripheral Identification 3 (read-only)
#define UARTPCellID0_0              *(unsigned long*)(UART_0_BLOCK + 0xff0)               // UART 0 Peripheral Identification 0 (read-only)
#define UARTPCellID1_0              *(unsigned long*)(UART_0_BLOCK + 0xff4)               // UART 0 Peripheral Identification 1 (read-only)
#define UARTPCellID2_0              *(unsigned long*)(UART_0_BLOCK + 0xff8)               // UART 0 Peripheral Identification 2 (read-only)
#define UARTPCellID3_0              *(unsigned long*)(UART_0_BLOCK + 0xffc)               // UART 0 Peripheral Identification 3 (read-only)

#define UARTDR_1_ADDR               (volatile unsigned long*)(UART_1_BLOCK + 0x000)
#define UARTDR_1                    *(volatile unsigned long*)(UART_1_BLOCK + 0x000)      // UART 1 Data
#define UARTRSR_ECR_1               *(volatile unsigned long*)(UART_1_BLOCK + 0x004)      // UART 1 Receive Status / Error Clear
#define UARTFR_1                    *(volatile unsigned long*)(UART_1_BLOCK + 0x018)      // UART 1 Flag (read-only)
#define UARTILPR_1                  *(unsigned long*)(UART_1_BLOCK + 0x020)               // UART 1 IrDA Low-Power Register
#define UARTIBRD_1                  *(unsigned long*)(UART_1_BLOCK + 0x024)               // UART 1 Integer Baud-Rate Divisor
#define UARTFBRD_1                  *(unsigned long*)(UART_1_BLOCK + 0x028)               // UART 1 Fractional Baud-Rate Divisor
#define UARTLCRH_1                  *(unsigned long*)(UART_1_BLOCK + 0x02c)               // UART 1 Line Control
#define UARTCTL_1                   *(unsigned long*)(UART_1_BLOCK + 0x030)               // UART 1 Control
#define UARTIFLS_1                  *(unsigned long*)(UART_1_BLOCK + 0x034)               // UART 1 Interrupt FIFO Level Select
#define UARTIM_1                    *(unsigned long*)(UART_1_BLOCK + 0x038)               // UART 1 Interrupt Mask
#define UARTRIS_1                   *(volatile unsigned long*)(UART_1_BLOCK + 0x03c)      // UART 1 RAW Interrupt Status (read-only)
#define UARTMIS_1                   *(volatile unsigned long*)(UART_1_BLOCK + 0x040)      // UART 1 Masked Interrupt Status (read-only)
#define UARTICR_1                   *(volatile unsigned long*)(UART_1_BLOCK + 0x044)      // UART 1 Interrupt Clear
#define UARTDMACTL_1                *(unsigned long*)(UART_1_BLOCK + 0x048)               // UART 1 DMA Control
#define UARTPeriphID4_1             *(unsigned long*)(UART_1_BLOCK + 0xfd0)               // UART 1 Peripheral Identification 4 (read-only)
#define UARTPeriphID5_1             *(unsigned long*)(UART_1_BLOCK + 0xfd4)               // UART 1 Peripheral Identification 5 (read-only)
#define UARTPeriphID6_1             *(unsigned long*)(UART_1_BLOCK + 0xfd8)               // UART 1 Peripheral Identification 6 (read-only)
#define UARTPeriphID7_1             *(unsigned long*)(UART_1_BLOCK + 0xfdc)               // UART 1 Peripheral Identification 7 (read-only)
#define UARTPeriphID0_1             *(unsigned long*)(UART_1_BLOCK + 0xfe0)               // UART 1 Peripheral Identification 0 (read-only)
#define UARTPeriphID1_1             *(unsigned long*)(UART_1_BLOCK + 0xfe4)               // UART 1 Peripheral Identification 1 (read-only)
#define UARTPeriphID2_1             *(unsigned long*)(UART_1_BLOCK + 0xfe8)               // UART 1 Peripheral Identification 2 (read-only)
#define UARTPeriphID3_1             *(unsigned long*)(UART_1_BLOCK + 0xfec)               // UART 1 Peripheral Identification 3 (read-only)
#define UARTPCellID0_1              *(unsigned long*)(UART_1_BLOCK + 0xff0)               // UART 1 Peripheral Identification 0 (read-only)
#define UARTPCellID1_1              *(unsigned long*)(UART_1_BLOCK + 0xff4)               // UART 1 Peripheral Identification 1 (read-only)
#define UARTPCellID2_1              *(unsigned long*)(UART_1_BLOCK + 0xff8)               // UART 1 Peripheral Identification 2 (read-only)
#define UARTPCellID3_1              *(unsigned long*)(UART_1_BLOCK + 0xffc)               // UART 1 Peripheral Identification 3 (read-only)

#define UARTDR_2_ADDR               (volatile unsigned long*)(UART_2_BLOCK + 0x000)
#define UARTDR_2                    *(volatile unsigned long*)(UART_2_BLOCK + 0x000)      // UART 2 Data
#define UARTRSR_ECR_2               *(volatile unsigned long*)(UART_2_BLOCK + 0x004)      // UART 2 Receive Status / Error Clear
#define UARTFR_2                    *(volatile unsigned long*)(UART_2_BLOCK + 0x018)      // UART 2 Flag (read-only)
#define UARTILPR_2                  *(unsigned long*)(UART_2_BLOCK + 0x020)               // UART 2 IrDA Low-Power Register
#define UARTIBRD_2                  *(unsigned long*)(UART_2_BLOCK + 0x024)               // UART 2 Integer Baud-Rate Divisor
#define UARTFBRD_2                  *(unsigned long*)(UART_2_BLOCK + 0x028)               // UART 2 Fractional Baud-Rate Divisor
#define UARTLCRH_2                  *(unsigned long*)(UART_2_BLOCK + 0x02c)               // UART 2 Line Control
#define UARTCTL_2                   *(unsigned long*)(UART_2_BLOCK + 0x030)               // UART 2 Control
#define UARTIFLS_2                  *(unsigned long*)(UART_2_BLOCK + 0x034)               // UART 2 Interrupt FIFO Level Select
#define UARTIM_2                    *(unsigned long*)(UART_2_BLOCK + 0x038)               // UART 2 Interrupt Mask
#define UARTRIS_2                   *(volatile unsigned long*)(UART_2_BLOCK + 0x03c)      // UART 2 RAW Interrupt Status (read-only)
#define UARTMIS_2                   *(volatile unsigned long*)(UART_2_BLOCK + 0x040)      // UART 2 Masked Interrupt Status (read-only)
#define UARTICR_2                   *(volatile unsigned long*)(UART_2_BLOCK + 0x044)      // UART 2 Interrupt Clear
#define UARTDMACTL_2                *(unsigned long*)(UART_0_BLOCK + 0x048)               // UART 2 DMA Control
#define UARTPeriphID4_2             *(unsigned long*)(UART_2_BLOCK + 0xfd0)               // UART 2 Peripheral Identification 4 (read-only)
#define UARTPeriphID5_2             *(unsigned long*)(UART_2_BLOCK + 0xfd4)               // UART 2 Peripheral Identification 5 (read-only)
#define UARTPeriphID6_2             *(unsigned long*)(UART_2_BLOCK + 0xfd8)               // UART 2 Peripheral Identification 6 (read-only)
#define UARTPeriphID7_2             *(unsigned long*)(UART_2_BLOCK + 0xfdc)               // UART 2 Peripheral Identification 7 (read-only)
#define UARTPeriphID0_2             *(unsigned long*)(UART_2_BLOCK + 0xfe0)               // UART 2 Peripheral Identification 0 (read-only)
#define UARTPeriphID1_2             *(unsigned long*)(UART_2_BLOCK + 0xfe4)               // UART 2 Peripheral Identification 1 (read-only)
#define UARTPeriphID2_2             *(unsigned long*)(UART_2_BLOCK + 0xfe8)               // UART 2 Peripheral Identification 2 (read-only)
#define UARTPeriphID3_2             *(unsigned long*)(UART_2_BLOCK + 0xfec)               // UART 2 Peripheral Identification 3 (read-only)
#define UARTPCellID0_2              *(unsigned long*)(UART_2_BLOCK + 0xff0)               // UART 2 Peripheral Identification 0 (read-only)
#define UARTPCellID1_2              *(unsigned long*)(UART_2_BLOCK + 0xff4)               // UART 2 Peripheral Identification 1 (read-only)
#define UARTPCellID2_2              *(unsigned long*)(UART_2_BLOCK + 0xff8)               // UART 2 Peripheral Identification 2 (read-only)
#define UARTPCellID3_2              *(unsigned long*)(UART_2_BLOCK + 0xffc)               // UART 2 Peripheral Identification 3 (read-only)

typedef struct stLM3XXXXX_UART_CONTROL
{
unsigned long UARTDR;       
unsigned long UARTRSR_ECR;  
unsigned long ulRes1[4];
unsigned long UARTFR; 
unsigned long ulRes2;      
unsigned long UARTILPR;     
unsigned long UARTIBRD;     
unsigned long UARTFBRD;     
unsigned long UARTLCRH;     
unsigned long UARTCTL;      
unsigned long UARTIFLS;     
unsigned long UARTIM;       
unsigned long UARTRIS;      
unsigned long UARTMIS;      
unsigned long _UARTICR;      
unsigned long ulRes3[994];
unsigned long UARTPeriphID4;
unsigned long UARTPeriphID5;
unsigned long UARTPeriphID6;
unsigned long UARTPeriphID7;
unsigned long UARTPeriphID0;
unsigned long UARTPeriphID1;
unsigned long UARTPeriphID2;
unsigned long UARTPeriphID3;
unsigned long UARTPCellID0; 
unsigned long UARTPCellID1; 
unsigned long UARTPCellID2; 
unsigned long UARTPCellID3;
} LM3XXXXX_UART_CONTROL;


// I2C Register Map
//
#define I2CMSA_0                    *(volatile unsigned long*)(I2C0_BLOCK + 0x000)        // Master 0 Slave Address
#define I2CMCS_0                    *(volatile unsigned long*)(I2C0_BLOCK + 0x004)        // Master 0 Control/Status
  #define I2C_RUN                   0x00000001                                            // write-only bits
  #define I2C_START                 0x00000002
  #define I2C_STOP                  0x00000004
  #define I2C_ACK                   0x00000008
  #define I2C_BUSY                  0x00000001                                            // read-only bits
  #define I2C_ERROR                 0x00000002
  #define I2C_ADRACK                0x00000004
  #define I2C_DATACK                0x00000008
  #define I2C_ARBLST                0x00000010
  #define I2C_IDLE                  0x00000020
  #define I2C_BUSBSY                0x00000040
#define I2CMDR_0                    *(volatile unsigned long*)(I2C0_BLOCK + 0x008)        // Master 0 Data
#define I2CMTPR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x00c)        // Master 0 Timer Period
#define I2CMIMR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x010)        // Master 0 Interrupt Mask
  #define I2C_INTERRUPT_BIT         0x00000001
#define I2CMRIS_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x014)        // Master 0 Raw Interrupt Status (read only)
#define I2CMMIS_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x018)        // Master 0 Masked Interrupt Status (read only)
#define I2CMICR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x01c)        // Master 0 Interrupt Clear (write only)
#define I2CMCR_0                    *(volatile unsigned long*)(I2C0_BLOCK + 0x020)        // Master 0 Configuration
  #define I2C_LPBK                  0x00000001                                            // Loopback mode
  #define I2C_MFE                   0x00000010                                            // Master Function Enable
  #define I2C_SFE                   0x00000020                                            // Slave Function Enable

#define I2CSOAR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x800)        // Slave 0 Own Address
#define I2CSCSR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x804)        // Slave 0 Control/Status
#define I2CSDR_0                    *(volatile unsigned long*)(I2C0_BLOCK + 0x808)        // Slave 0 Data
#define I2CSIMR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x80c)        // Slave 0 Interrupt Mask
#define I2CSRIS_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x810)        // Slave 0 Raw Interrupt Status (read only)
#define I2CSMIS_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x814)        // Slave 0 Masked Interrupt Status (read only)
#define I2CSICR_0                   *(volatile unsigned long*)(I2C0_BLOCK + 0x818)        // Slave 0 Interrupt Clear (write only)


#define I2CMSA_1                    *(volatile unsigned long*)(I2C1_BLOCK + 0x000)        // Master 1 Slave Address
#define I2CMCS_1                    *(volatile unsigned long*)(I2C1_BLOCK + 0x004)        // Master 1 Control/Status
#define I2CMDR_1                    *(volatile unsigned long*)(I2C1_BLOCK + 0x008)        // Master 1 Data
#define I2CMTPR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x00c)        // Master 1 Timer Period
#define I2CMIMR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x010)        // Master 1 Interrupt Mask
#define I2CMRIS_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x014)        // Master 1 Raw Interrupt Status (read only)
#define I2CMMIS_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x018)        // Master 1 Masked Interrupt Status (read only)
#define I2CMICR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x01c)        // Master 1 Interrupt Clear (write only)
#define I2CMCR_1                    *(volatile unsigned long*)(I2C1_BLOCK + 0x020)        // Master 1 Configuration

#define I2CSOAR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x800)        // Slave 1 Own Address
#define I2CSCSR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x804)        // Slave 1 Control/Status
#define I2CSDR_1                    *(volatile unsigned long*)(I2C1_BLOCK + 0x808)        // Slave 1 Data
#define I2CSIMR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x80c)        // Slave 1 Interrupt Mask
#define I2CSRIS_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x810)        // Slave 1 Raw Interrupt Status (read only)
#define I2CSMIS_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x814)        // Slave 1 Masked Interrupt Status (read only)
#define I2CSICR_1                   *(volatile unsigned long*)(I2C1_BLOCK + 0x818)        // Slave 1 Interrupt Clear (write only)


#define SCL_LP       6
#define SCL_HP       4

typedef struct stLM3XXXXX_I2C_CONTROL
{
    unsigned long I2CMSA;                                                // master control registers
    unsigned long I2CMCS;
    unsigned long I2CMDR;
    unsigned long I2CMTPR;
    unsigned long I2CMIMR;
    unsigned long I2CMRIS;
    unsigned long I2CMMIS;
    unsigned long I2CMICR;
    unsigned long I2CMCR;  
    unsigned long ulRes[503];
    unsigned long I2CSOAR;                                               // slave control registers
    unsigned long I2CSCSR;
    unsigned long I2CSDR;
    unsigned long I2CSIMR;
    unsigned long I2CSRIS;
    unsigned long I2CSMIS;
    unsigned long I2CSICR;
} LM3XXXXX_I2C_CONTROL;


// FLASH Control Register Map
//
#define FMA                         *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x00)         // FLASH Memory Address
#define FMD                         *(volatile unsigned long*)(FLASH_CONTROL_BLOCK + 0x04)// FLASH Memory Data
#define FMC                         *(volatile unsigned long*)(FLASH_CONTROL_BLOCK + 0x08)// FLASH Memory Control
  #define WRKEY                     0xa4420000
  #define FLASH_WRITE               0x00000001
  #define FLASH_ERASE               0x00000002
  #define FLASH_MERASE              0x00000004
  #define FLASH_COMT                0x00000008
#define FCRIS                       *(volatile unsigned long*)(FLASH_CONTROL_BLOCK + 0x0c)// FLASH Controller Raw Interrupt Status (read only)
  #define ARIS                      0x00000001
  #define PRIS                      0x00000002
#define FCIM                        *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x10)         // FLASH Controller Interrupt Mask
  #define AMASK                     0x00000001
  #define PMASK                     0x00000002
#define FCMISC                      *(volatile unsigned long*)(FLASH_CONTROL_BLOCK + 0x14)// FLASH Controller Masked Interrupt Status Clear
  #define AMISC                     0x00000001
  #define PMISC                     0x00000002
#ifdef _TEMPEST_CLASS                                                                     // {16}
    #define FMC2                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x20)         // FLASH Memory Control 2
      #define FLASH_BUFFERED_WRITE  0x00000001
    #define FWBVAL                  *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x30)         // FLASH Write Buffer Valid
    #define FCTL                    *(volatile unsigned long*)(FLASH_CONTROL_BLOCK + 0xf8)// FLASH Control
      #define USDREG                0x00000001
      #define USDACK                0x00000002
    #define FWB0                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x100)        // FLASH Write Buffer 0
    #define FWB1                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x104)        // FLASH Write Buffer 1
    #define FWB2                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x108)        // FLASH Write Buffer 2
    #define FWB3                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x10c)        // FLASH Write Buffer 3
    #define FWB4                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x110)        // FLASH Write Buffer 4
    #define FWB5                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x114)        // FLASH Write Buffer 5
    #define FWB6                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x118)        // FLASH Write Buffer 6
    #define FWB7                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x11c)        // FLASH Write Buffer 7
    #define FWB8                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x120)        // FLASH Write Buffer 8
    #define FWB9                    *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x124)        // FLASH Write Buffer 9
    #define FWB10                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x128)        // FLASH Write Buffer 10
    #define FWB11                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x12c)        // FLASH Write Buffer 11
    #define FWB12                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x130)        // FLASH Write Buffer 12
    #define FWB13                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x134)        // FLASH Write Buffer 13
    #define FWB14                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x138)        // FLASH Write Buffer 14
    #define FWB15                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x13c)        // FLASH Write Buffer 15
    #define FWB16                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x140)        // FLASH Write Buffer 16
    #define FWB17                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x144)        // FLASH Write Buffer 17
    #define FWB18                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x148)        // FLASH Write Buffer 18
    #define FWB19                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x14c)        // FLASH Write Buffer 19
    #define FWB20                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x150)        // FLASH Write Buffer 20
    #define FWB21                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x154)        // FLASH Write Buffer 21
    #define FWB22                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x158)        // FLASH Write Buffer 22
    #define FWB23                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x15c)        // FLASH Write Buffer 23
    #define FWB24                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x160)        // FLASH Write Buffer 24
    #define FWB25                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x164)        // FLASH Write Buffer 25
    #define FWB26                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x168)        // FLASH Write Buffer 26
    #define FWB27                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x16c)        // FLASH Write Buffer 27
    #define FWB28                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x170)        // FLASH Write Buffer 28
    #define FWB29                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x174)        // FLASH Write Buffer 29
    #define FWB30                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x178)        // FLASH Write Buffer 30
    #define FWB31                   *(unsigned long*)(FLASH_CONTROL_BLOCK + 0x17c)        // FLASH Write Buffer 31
#endif

#if defined _TEMPEST_CLASS                                                                // {19}
    #define FLASH_LINE_SIZE         8                                                     // lines made up of 8 bytes to avoid file header problems with the FLASH characteristics
    #define FLASH_ROW_SIZE          4                                                     // FLASH writes are performed on this many bytes at a time (this is not FLASH structure dependent but rather logical when working with the long word write restriction - a buffer of the same size as the FLASH granularity results in fastest operation due to less FLASH writes)
    #define MAX_SECTOR_PARS         ((FLASH_GRANULARITY - (2*FLASH_ROW_SIZE))/FLASH_ROW_SIZE) // the number of user bytes fitting into first parameter block
#else
    #define MAX_SECTOR_PARS         (FLASH_GRANULARITY - 2)                               // the number of user bytes fitting into first parameter block
#endif
#define FLASH_GRANULARITY           (1024)                                                // smallest sector which can be erased independently



// System Control Register Map
//
#define DID0                        *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x000)       // Device Identification 0 (read only)
#define DID1                        *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x004)       // Device Identification 1 (read only)
#define DC0                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x008)       // Device Capabilities 0 (read only)
#define DC1                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x010)       // Device Capabilities 1 (read only)
#define DC2                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x014)       // Device Capabilities 2 (read only)
#define DC3                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x018)       // Device Capabilities 3 (read only)
#define DC4                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x01c)       // Device Capabilities 4 (read only)
#define DC5                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x020)       // Device Capabilities 5 (read only)
#define DC6                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x024)       // Device Capabilities 6 (read only)
#define DC7                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x028)       // Device Capabilities 7 (read only)
#define DC8                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x028)       // Device Capabilities 8 (read only)
#define PBORCTL                     *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x030)       // brown-Out Reset Control
#define LDOPCTL                     *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x034)       // LDO Power Control

#define SRCR0                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x040) // Software Reset Control 0 (set volatile to allow short delays)
#define SRCR1                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x044) // Software Reset Control 1
#define SRCR2                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x048) // Software Reset Control 2

#define RIS                         *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x050) // RAW Interrupt Status (Read only)
  #define BORRIS                    0x00000002                                            // Brown-Out Reset RAW Interrupt Status
  #define PLLLRIS                   0x00000040                                            // PLL locked
  #define USBPLLLRIS                0x00000080                                            // USB PLL locked
#define IMC                         *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x054)       // Interrupt Mask Control
#define MISC                        *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x058) // Masked Interrupt Status and Clear
#define RESC                        *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x05c) // ResetCause
#define RCC                         *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x060) // Run-Mode Clock Configuration
  #define MOSCDIS                   0x00000001                                            // Main oscillator is disabled (default)
  #define IOSCDIS                   0x00000002                                            // Internal oscillator is disabled
  #define OSCSRC_MAIN               0x00000000                                            // Oscillator taken from main oscillator
  #define OSCSRC_INT                0x00000010                                            // Oscillator taken internal
  #define OSCSRC_INT_4              0x00000020                                            // Oscillator taken internal/4 (required when PLL input)
  #define OSCSRC_MASK               0x00000030
  #define XTAL_1_0000               0x00000000                                            // 1MHz oscillator attached
  #define XTAL_1_8432               0x00000040                                            // 1.8432MHz oscillator attached
  #define XTAL_2_0000               0x00000080                                            // 2MHz oscillator attached
  #define XTAL_2_4576               0x000000c0                                            // 2.4576MHz oscillator attached
  #define XTAL_3_579545             0x00000100                                            // 3.579545MHz oscillator attached
  #define XTAL_3_6864               0x00000140                                            // 3.6864MHz oscillator attached
  #define XTAL_4_0000               0x00000180                                            // 4MHz oscillator attached (USB possible)
  #define XTAL_4_096                0x000001c0                                            // 4.096MHz oscillator attached
  #define XTAL_4_9152               0x00000200                                            // 4.9152MHz oscillator attached
  #define XTAL_5_0000               0x00000240                                            // 5MHz oscillator attached (USB possible)
  #define XTAL_5_1200               0x00000280                                            // 5.12MHz oscillator attached
  #define XTAL_6_0000               0x000002c0                                            // 6MHz oscillator attached (reset value) (USB possible)
  #define XTAL_6_1440               0x00000300                                            // 6.144MHz oscillator attached
  #define XTAL_7_3728               0x00000340                                            // 7.3728MHz oscillator attached
  #define XTAL_8_0000               0x00000380                                            // 8MHz oscillator attached (USB possible)
  #define XTAL_8_1920               0x000003c0                                            // 8.129MHz oscillator attached
  #define XTAL_10_0000              0x00000400                                            // 10MHz oscillator attached (USB possible)
  #define XTAL_12_0000              0x00000440                                            // 12MHz oscillator attached (USB possible)
  #define XTAL_12_2880              0x00000480                                            // 12.288MHz oscillator attached
  #define XTAL_13_5600              0x000004c0                                            // 13.56MHz oscillator attached
  #define XTAL_14_31818             0x00000500                                            // 14.31318MHz oscillator attached
  #define XTAL_16_0000              0x00000540                                            // 16MHz oscillator attached (USB possible)
  #define XTAL_16_3840              0x00000580                                            // 16.384MHz oscillator attached

  #define BYPASS                    0x00000800
  #define PWRDN                     0x00002000                                            // PLL Power down
  #define PWMDIV_2                  0x00000000
  #define PWMDIV_4                  0x00020000
  #define PWMDIV_8                  0x00040000
  #define PWMDIV_16                 0x00060000
  #define PWMDIV_32                 0x00080000
  #define PWMDIV_64                 0x000e0000
  #define USEPWMDIV                 0x00100000
  #define USESYSDIV                 0x00400000
  #define SYSDIV_4                  0x00800000                                            // 100MHz operation
  #define SYSDIV_6                  0x01000000                                            // 80MHz operation
  #define SYSDIV_8                  0x01800000                                            // VCO frequency is 400MHz - 50MHz operation
  #define SYSDIV_10                 0x02000000                                            // 40MHz operation
  #define SYSDIV_12                 0x02800000                                            // 33.33MHz operation
  #define SYSDIV_14                 0x03000000                                            // 28.57MHz operation
  #define SYSDIV_16                 0x03800000                                            // 25MHz operation
  #define SYSDIV_18                 0x04000000                                            // 22.22MHz operation
  #define SYSDIV_20                 0x04800000                                            // 20MHz operation
  #define SYSDIV_22                 0x05000000                                            // 18.18MHz operation
  #define SYSDIV_24                 0x05800000                                            // 16.67MHz operation
  #define SYSDIV_26                 0x06000000                                            // 15.38MHz operation
  #define SYSDIV_28                 0x06800000                                            // 14.29MHz operation
  #define SYSDIV_30                 0x07000000                                            // 13.33MHz operation
  #define SYSDIV_32                 0x07800000                                            // 12.5MHz operation (default)
  #define ACG                       0x08000000                                            // Auto Clock Gating
#define PLLCFG                      *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x064) // XTAL to PLL Translation (read only)
#if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS
    #define GPIOHSCTL               *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x06c) // GPIO High-Speed Control
      #define PORTAHS               0x00000001                                            // Port A High-Speed
      #define PORTBHS               0x00000002                                            // Port B High-Speed
      #define PORTCHS               0x00000004                                            // Port C High-Speed
      #define PORTDHS               0x00000008                                            // Port D High-Speed
      #define PORTEHS               0x00000010                                            // Port E High-Speed
      #define PORTFHS               0x00000020                                            // Port F High-Speed
      #define PORTGHS               0x00000040                                            // Port G High-Speed
      #define PORTHHS               0x00000080                                            // Port H High-Speed
      #define PORTJHS               0x00000100                                            // Port J High-Speed
#endif
#define RCC2                        *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x070) // Run Mode Clock Configuration 2
  #define USERCC2                   0x80000000                                               // {18} override RCC register fields with those in this register (LM3S9BXX)
  #define DIV400                    0x40000000                                               // PLL output used directly from 400MHz (LM3S9BXX)
  #define SYSDIV2LSB                0x00400000
  #define USBPWRDWN                 0x00004000                                               // USB power down bit

#if defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS
    #define MOSCCTL                 *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x07c) // Main Oscillator Control
      #define CVAL 0x00000001                                                                // Clock Validation for MOSC
#endif
#ifdef _TEMPEST_CLASS                                                                        // {16}
    #define RMCTL                   *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x0f0) // ROM Control
      #define BOOT_ALIAS            0x00000001                                               // boot address is 0xffffffff and the microcontroller's ROM is switched in
    #define RMVER                   *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x0f4) // ROM Version Register - read-only
#endif
#define RCGC0                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x100) // Run Mode Clock Gating Control Register 0
  #define CGC_WDT                   0x00000008                                               // Watchdog module clocked/enabled
  #define CGC_HIB                   0x00000040                                               // Hibernation module clocked/enabled
  #define CGC_SARADC0               0x00010000                                               // SAR ADC0 module clocked/enabled
  #define CGC_SARADC1               0x00020000                                               // SAR ADC1 module clocked/enabled {26}
  #define CGC_PWM                   0x00100000                                               // PWM module clocked/enabled
#define RCGC1                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x104) // Run Mode Clock Gating Control Register 1
  #define CGC_UART0                 0x00000001                                               // UART0 module clocked/enabled
  #define CGC_UART1                 0x00000002                                               // UART1 module clocked/enabled
  #define CGC_UART2                 0x00000004                                               // UART2 module clocked/enabled
  #define CGC_SSI0                  0x00000010                                               // SSI0 module clocked/enabled
  #define CGC_SSI1                  0x00000020                                               // SSI1 module clocked/enabled {25}
  #define CGC_QEI0                  0x00000100                                               // QEI0 module clocked/enabled
  #define CGC_QEI1                  0x00000200                                               // QEI1 module clocked/enabled
  #define CGC_I2C0                  0x00001000                                               // I2C0 module clocked/enabled
  #define CGC_I2C1                  0x00004000                                               // I2C1 module clocked/enabled
  #define CGC_TIMER0                0x00010000                                               // Timer 0 module clocked/enabled
  #define CGC_TIMER1                0x00020000                                               // Timer 1 module clocked/enabled
  #define CGC_TIMER2                0x00040000                                               // Timer 2 module clocked/enabled
  #define CGC_TIMER3                0x00080000                                               // Timer 3 module clocked/enabled
  #define CGC_COMP0                 0x01000000                                               // Comparator 0 module clocked/enabled
  #define CGC_COMP1                 0x02000000                                               // Comparator 1 module clocked/enabled
#define RCGC2                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x108) // Run Mode Clock Gating Control Register 2
  #define CGC_GPIOA                 0x00000001                                               // GPIOA module clocked/enabled
  #define CGC_GPIOB                 0x00000002                                               // GPIOB module clocked/enabled
  #define CGC_GPIOC                 0x00000004                                               // GPIOC module clocked/enabled
  #define CGC_GPIOD                 0x00000008                                               // GPIOD module clocked/enabled
  #define CGC_GPIOE                 0x00000010                                               // GPIOE module clocked/enabled
  #define CGC_GPIOF                 0x00000020                                               // GPIOF module clocked/enabled
  #define CGC_GPIOG                 0x00000040                                               // GPIOG module clocked/enabled
  #define CGC_GPIOH                 0x00000080                                               // GPIOH module clocked/enabled
  #define CGC_GPIOJ                 0x00000100                                               // GPIOJ module clocked/enabled
  #define CGC_uDMA                  0x00002000                                               // uDMA module clocked/enabled
  #define CGC_USB0                  0x00010000                                               // USB0 module clocked/enabled
  #define CGC_EMAC0                 0x10000000                                               // EMAC0 module clocked/enabled
  #define CGC_EPHY0                 0x40000000                                               // EPHY0 module clocked/enabled
#define SCGC0                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x110) // Sleep Mode Clock Gating Control Register 0
#define SCGC1                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x114) // Sleep Mode Clock Gating Control Register 1
#define SCGC2                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x118) // Sleep Mode Clock Gating Control Register 2

#define DCGC0                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x120) // Deep Sleep Mode Clock Gating Control Register 0
#define DCGC1                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x124) // Deep Sleep Mode Clock Gating Control Register 1
#define DCGC2                       *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x128) // Deep Sleep Mode Clock Gating Control Register 2

#define FMPRE0                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x130)          // FLASH Memory Protection Read Enable 0
#define FMPPE0                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x134)          // FLASH Memory Protection Program Enable 0

#define USECRL                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x140)          // uSEC Reload
#define DSLPCLKCFG                  *(volatile unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x144) // Deep Sleep Clock Configuration Register

#define USER_DBG                    *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x1d0)          // User Debug
#define USER_REG0                   *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x1e0)          // User Register 0
#define USER_REG1                   *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x1e4)          // User Register 1
#ifdef _TEMPEST_CLASS                                                                        // {16}
    #define USER_REG2               *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x1e8)          // User Register 2
    #define USER_REG3               *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x1ec)          // User Register 3
    #define FMPRE0_0                *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x200)          // FLASH Memory Protection Read Enable 0 - (Alias)
#endif
#define FMPRE1                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x204)          // FLASH Memory Protection Read Enable 1
#define FMPRE2                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x208)          // FLASH Memory Protection Read Enable 2
#define FMPRE3                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x20c)          // FLASH Memory Protection Read Enable 3

#ifdef _TEMPEST_CLASS                                                                        // {16}
    #define FMPPE0_0                *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x400)          // FLASH Memory Protection Program Enable 0 - (Alias)
#endif
#define FMPPE1                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x404)          // FLASH Memory Protection Program Enable 1
#define FMPPE2                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x408)          // FLASH Memory Protection Program Enable 2
#define FMPPE3                      *(unsigned long*)(SYSTEM_CONTROL_BLOCK + 0x40c)          // FLASH Memory Protection Program Enable 3


// Register number used to connect power and control reset of each module
//
#define WATCHDOG_POWER              0
#define HIB_POWER                   0
#define SAR_ADC0_POWER              0
#define PWM_POWER                   0

#define UART_POWER                  1
#define SSI_POWER                   1
#define QEI_POWER                   1
#define I2C_POWER                   1
#define TIMER_POWER                 1
#define COMPARATOR_POWER            1

#define POWER_POWER                 2
#define USB_POWER                   2
#define EMAC_POWER                  2
#define EPHY_POWER                  2

// uDMA                                                                  // {4}
//
#define DMASTAT                     *(volatile unsigned long*)(uDMA_BLOCK + 0x000)     // DMA Status (read only)
#define DMACFG                      *(volatile unsigned long*)(uDMA_BLOCK + 0x004)     // DMA Configuration (write only)
  #define uDMA_MASTEN               0x00000001                                         // master enable
#define DMACTLBASE                  *(unsigned long*)(uDMA_BLOCK + 0x008)              // DMA Channel Control Base Pointer
#define DMAALTBASE                  *(volatile unsigned long*)(uDMA_BLOCK + 0x00c)     // DMA Alternative Channel Control Base Pointer (read only)
#define DMAWAITSTAT                 *(volatile unsigned long*)(uDMA_BLOCK + 0x010)     // DMA Channel Wait on Request Status (read only)
#define DMASWREQ                    *(volatile unsigned long*)(uDMA_BLOCK + 0x014)     // DMA Channel Software Request (write only)
#define DMAUSEBURSTSET              *(volatile unsigned long*)(uDMA_BLOCK + 0x018)     // DMA Channel Useburst Set
#define DMAUSEBURSTCLR              *(volatile unsigned long*)(uDMA_BLOCK + 0x01c)     // DMA Channel Useburst Clear (write only)
#define DMAREQMASKSET               *(volatile unsigned long*)(uDMA_BLOCK + 0x020)     // DMA Channel Request Mask Set
#define DMAREQMASKCLR               *(volatile unsigned long*)(uDMA_BLOCK + 0x024)     // DMA Channel Request Mask Clear (write only)
#define DMAENASET                   *(volatile unsigned long*)(uDMA_BLOCK + 0x028)     // DMA Channel Enable Set
#define DMAENACLR                   *(volatile unsigned long*)(uDMA_BLOCK + 0x02c)     // DMA Channel Enable Clear (write only)
#define DMAALTSET                   *(volatile unsigned long*)(uDMA_BLOCK + 0x030)     // DMA Channel Primary Alternate Set
#define DMAALTCLR                   *(volatile unsigned long*)(uDMA_BLOCK + 0x034)     // DMA Channel Primary Alternate Clear (write only)
#define DMAPRIOSET                  *(volatile unsigned long*)(uDMA_BLOCK + 0x038)     // DMA Channel Primary Priority Set
#define DMAPRIOCLR                  *(volatile unsigned long*)(uDMA_BLOCK + 0x03c)     // DMA Channel Primary Priority Clear (write only)
#define DMAERRCLR                   *(volatile unsigned long*)(uDMA_BLOCK + 0x04c)     // DMA Bus Error Clear

#define DMAPeriphID4                *(volatile unsigned long*)(uDMA_BLOCK + 0xfd0)     // DMA Peripheral Identification 4 (read only)
#define DMAPeriphID0                *(volatile unsigned long*)(uDMA_BLOCK + 0xfe0)     // DMA Peripheral Identification 0 (read only)
#define DMAPeriphID1                *(volatile unsigned long*)(uDMA_BLOCK + 0xfe4)     // DMA Peripheral Identification 1 (read only)
#define DMAPeriphID2                *(volatile unsigned long*)(uDMA_BLOCK + 0xfe8)     // DMA Peripheral Identification 2 (read only)
#define DMAPeriphID3                *(volatile unsigned long*)(uDMA_BLOCK + 0xfec)     // DMA Peripheral Identification 3 (read only)
#define DMACellID0                  *(volatile unsigned long*)(uDMA_BLOCK + 0xff0)     // DMA PrimeCell Identification 0 (read only)
#define DMACellID1                  *(volatile unsigned long*)(uDMA_BLOCK + 0xff4)     // DMA PrimeCell Identification 1 (read only)
#define DMACellID2                  *(volatile unsigned long*)(uDMA_BLOCK + 0xff8)     // DMA PrimeCell Identification 2 (read only)
#define DMACellID3                  *(volatile unsigned long*)(uDMA_BLOCK + 0xffc)     // DMA PrimeCell Identification 3 (read only)

#define MAX_TX_DMA                  1024                                 // longest single DMA transfer length

typedef struct stDMA_channel_control_structure
{
  volatile unsigned char *DMASRCENDP;                                    // DMA Channel Source Address End Pointer
  volatile unsigned char *DMADSTENDP;                                    // DMA Channel Destination Address End Pointer
  volatile unsigned long  DMACHCTL;                                      // DMA Channel Control Word
  unsigned long           reserved;                                      // DMA Channel reserved long word (unused)
} DMA_CONTROL_STRUCTURE;

// DMA control word defininitions
//
#define XFERMODE_MASK                0x00000007
#define XFERMODE_STOP                0x00000000
#define XFERMODE_BASIC               0x00000001
#define XFERMODE_AUTO_REQUEST        0x00000002
#define XFERMODE_PING_PONG           0x00000003
#define XFERMODE_MEM_SCATTER_GATHER  0x00000004
#define XFERMODE_ALTM_SCATTER_GATHER 0x00000005
#define XFERMODE_PER_SCATTER_GATHER  0x00000006
#define XFERMODE_ALTP_SCATTER_GATHER 0x00000007
#define XFERMODE_MEM_SCATTER_GATHER  0x00000004
#define NXTUSEBURST                  0x00000008
#define XFERSIZE_MASK                0x00003ff0
#define XFERSIZE_SHIFT               4
#define ARBSIZE_1                    0x00000000
#define ARBSIZE_2                    0x00004000
#define ARBSIZE_4                    0x00008000
#define ARBSIZE_8                    0x0000c000
#define ARBSIZE_16                   0x00010000
#define ARBSIZE_32                   0x00014000
#define ARBSIZE_64                   0x00018000
#define ARBSIZE_128                  0x0001c000
#define ARBSIZE_256                  0x00020000
#define ARBSIZE_512                  0x00024000
#define ARBSIZE_1024                 0x00028000
#define SRCSIZE_8_BIT                0x00000000
#define SRCSIZE_16_BIT               0x01000000
#define SRCSIZE_32_BIT               0x02000000
#define SRCINC_BYTE                  0x00000000
#define SRCINC_HALF_WORD             0x04000000
#define SRCINC_WORD                  0x08000000
#define SRCINC_NO_INC                0x0c000000
#define DSTSIZE_8_BIT                0x00000000
#define DSTSIZE_16_BIT               0x10000000
#define DSTSIZE_32_BIT               0x20000000
#define DSTINC_BYTE                  0x00000000
#define DSTINC_HALF_WORD             0x40000000
#define DSTINC_WORD                  0x80000000
#define DSTINC_NO_INC                0xc0000000

// DMA channel assignments
//
#define DMA_USB_ENDPOINT_1_RECEIVE_CH   0
#define DMA_USB_ENDPOINT_1_RECEIVE_BIT  0x00000001
#define DMA_USB_ENDPOINT_1_TRANSMIT_CH  1
#define DMA_USB_ENDPOINT_1_TRANSMIT_BIT 0x00000002
#define DMA_USB_ENDPOINT_2_RECEIVE_CH   2
#define DMA_USB_ENDPOINT_2_RECEIVE_BIT  0x00000004
#define DMA_USB_ENDPOINT_2_TRANSMIT_CH  3
#define DMA_USB_ENDPOINT_2_TRANSMIT_BIT 0x00000008
#define DMA_USB_ENDPOINT_3_RECEIVE_CH   4
#define DMA_USB_ENDPOINT_3_RECEIVE_BIT  0x00000010
#define DMA_USB_ENDPOINT_3_TRANSMIT_CH  5
#define DMA_USB_ENDPOINT_3_TRANSMIT_BIT 0x00000020
//0x00000040
//0x00000080
#define DMA_UART0_RECEIVE_CH            8
#define DMA_UART0_RECEIVE_BIT           0x00000100
#define DMA_UART0_TRANSMIT_CH           9
#define DMA_UART0_TRANSMIT_BIT          0x00000200
#define DMA_SSI0_RECEIVE_CH             10
#define DMA_SSI0_RECEIVE_BIT            0x00000400
#define DMA_SSI0_TRANSMIT_CH            11
#define DMA_SSI0_TRANSMIT_BIT           0x00000800
//0x00001000
//0x00002000
//0x00004000
//0x00008000
//0x00010000
//0x00020000
//0x00040000
//0x00080000
//0x00100000
//0x00200000
#define DMA_UART1_RECEIVE_CH            22
#define DMA_UART1_RECEIVE_BIT           0x00400000
#define DMA_UART1_TRANSMIT_CH           23
#define DMA_UART1_TRANSMIT_BIT          0x00800000
#define DMA_SSI1_RECEIVE_CH             24
#define DMA_SSI1_RECEIVE_BIT            0x01000000
#define DMA_SSI1_TRANSMIT_CH            25
#define DMA_SSI1_TRANSMIT_BIT           0x02000000
//0x04000000
//0x08000000
//0x10000000
//0x20000000
#define DMA_SOFTWARE_CH                 30
#define DMA_SOFTWARE_BIT                0x40000000
//0x80000000


// Alternate DMA assignment                                              // {14}
#define DMA_ALT_UART2_RECEIVE_CH_1          0
#define DMA_ALT_UART2_RECEIVE_BIT_1         0x00000001
#define DMA_ALT_UART2_TRANSMIT_CH_1         1
#define DMA_ALT_UART2_TRANSMIT_BIT_1        0x00000002
//0x00000004
//0x00000008
//0x00000010
//0x00000020
//0x00000040
//0x00000080
//0x00000100
//0x00000200
#define DMA_ALT_UART1_RECEIVE_CH            10
#define DMA_ALT_UART1_RECEIVE_BIT           0x00000400
#define DMA_ALT_UART1_TRANSMIT_CH           11
#define DMA_ALT_UART1_TRANSMIT_BIT          0x00000800
#define DMA_ALT_UART2_RECEIVE_CH_2          12
#define DMA_ALT_UART2_RECEIVE_BIT_2         0x00001000
#define DMA_ALT_UART2_TRANSMIT_CH_2         13
#define DMA_ALT_UART2_TRANSMIT_BIT_2        0x00002000
//0x00004000
//0x00008000
//0x00010000
//0x00020000
//0x00040000
//0x00080000
//0x00100000
//0x00200000
//0x00400000
//0x01000000
//0x02000000
//0x04000000
//0x08000000
//0x10000000
//0x20000000
//0x40000000
//0x80000000


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

#define IRQ0_31_SPR                 *(unsigned long*)(CORTEX_M3_BLOCK + 0x200)         // NVIC IRQ0..31    Set Pending Register
#define IRQ32_63_SPR                *(unsigned long*)(CORTEX_M3_BLOCK + 0x204)         // NVIC IRQ32..64   Set Pending Register
#define IRQ64_95_SPR                *(unsigned long*)(CORTEX_M3_BLOCK + 0x208)         // NVIC IRQ64..95   Set Pending Register
#define IRQ96_127_SPR               *(unsigned long*)(CORTEX_M3_BLOCK + 0x20c)         // NVIC IRQ96..127  Set Pending Register
#define IRQ128_159_SPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x210)         // NVIC IRQ128..159 Set Pending Register
#define IRQ160_191_SPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x214)         // NVIC IRQ160..191 Set Pending Register
#define IRQ192_223_SPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x218)         // NVIC IRQ192..223 Set Pending Register
#define IRQ224_239_SPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x21c)         // NVIC IRQ224..239 Set Pending Register

#define IRQ0_31_CPR_ADD             (unsigned long*)(CORTEX_M3_BLOCK + 0x280)
#define IRQ0_31_CPR                 *(unsigned long*)(CORTEX_M3_BLOCK + 0x280)         // NVIC IRQ0..31    Clear Pending Register
#define IRQ32_63_CPR                *(unsigned long*)(CORTEX_M3_BLOCK + 0x284)         // NVIC IRQ32..64   Clear Pending Register
#define IRQ64_95_CPR                *(unsigned long*)(CORTEX_M3_BLOCK + 0x288)         // NVIC IRQ64..95   Clear Pending Register
#define IRQ96_127_CPR               *(unsigned long*)(CORTEX_M3_BLOCK + 0x28c)         // NVIC IRQ96..127  Clear Pending Register
#define IRQ128_159_CPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x290)         // NVIC IRQ128..159 Clear Pending Register
#define IRQ160_191_CPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x294)         // NVIC IRQ160..191 Clear Pending Register
#define IRQ192_223_CPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x298)         // NVIC IRQ192..223 Clear Pending Register
#define IRQ224_239_CPR              *(unsigned long*)(CORTEX_M3_BLOCK + 0x29c)         // NVIC IRQ224..239 Clear Pending Register

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
  #define VECTKEY                     0x05fa0000                         // {24}
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
#define irq_GPIO_Port_A_ID            0
#define irq_GPIO_Port_B_ID            1
#define irq_GPIO_Port_C_ID            2
#define irq_GPIO_Port_D_ID            3
#define irq_GPIO_Port_E_ID            4
#define irq_UART0_ID                  5
#define irq_UART1_ID                  6
#define irq_SSI0_ID                   7
#define irq_I2C0_ID                   8
#define irq_PWM_fault_ID              9
#define irq_PWM_Generator_0_ID        10
#define irq_PWM_Generator_1_ID        11
#define irq_PWM_Generator_2_ID        12
#define irq_QEI0_ID                   13
#define irq_ADC_Sequence_0_ID         14
#define irq_ADC_Sequence_1_ID         15
#define irq_ADC_Sequence_2_ID         16
#define irq_ADC_Sequence_3_ID         17
#define irq_Watchdog_Timer_ID         18
#define irq_Timer0_A_ID               19
#define irq_Timer0_B_ID               20
#define irq_Timer1_A_ID               21
#define irq_Timer1_B_ID               22
#define irq_Timer2_A_ID               23
#define irq_Timer2_B_ID               24
#define irq_Analog_Comparator_0_ID    25
#define irq_Analog_Comparator_1_ID    26

#define irq_System_Control_ID         28
#define irq_FLASH_Control_ID          29
#define irq_GPIO_Port_F_ID            30
#define irq_GPIO_Port_G_ID            31
#define irq_GPIO_Port_H_ID            32
#define irq_UART2_ID                  33

#define irq_Timer3_A_ID               35
#define irq_Timer3_B_ID               36
#define irq_I2C1_ID                   37
#define irq_QEI1_ID                   38

#define irq_Ethernet_Controller_ID    42
#define irq_Hibernation_Module_ID     43
#define irq_USB_ID                    44

#define irq_ADC1_Sequence_0_ID        48                                 // {26}
#define irq_ADC1_Sequence_1_ID        49
#define irq_ADC1_Sequence_2_ID        50
#define irq_ADC1_Sequence_3_ID        51

#define irq_GPIO_Port_J_ID            54
/*
#define irq_GPIO_Port_A               0x00000001
#define irq_GPIO_Port_B               0x00000002
#define irq_GPIO_Port_C               0x00000004
#define irq_GPIO_Port_D               0x00000008
#define irq_GPIO_Port_E               0x00000010
#define irq_UART0                     0x00000020
#define irq_UART1                     0x00000040
#define irq_SSI0                      0x00000080
#define irq_I2C0                      0x00000100
#define irq_PWM_fault                 0x00000200
#define irq_PWM_Generator_0           0x00000400
#define irq_PWM_Generator_1           0x00000800
#define irq_PWM_Generator_2           0x00001000
#define irq_QEI0                      0x00002000
#define irq_ADC_Sequence_0            0x00004000
#define irq_ADC_Sequence_1            0x00008000
#define irq_ADC_Sequence_2            0x00010000
#define irq_ADC_Sequence_3            0x00020000
#define irq_Watchdog_Timer            0x00040000
#define irq_Timer0_A                  0x00080000
#define irq_Timer0_B                  0x00100000
#define irq_Timer1_A                  0x00200000
#define irq_Timer1_B                  0x00400000
#define irq_Timer2_A                  0x00800000
#define irq_Timer2_B                  0x01000000
#define irq_Analog_Comparator_0       0x02000000
#define irq_Analog_Comparator_1       0x04000000

#define irq_System_Control            0x10000000
#define irq_FLASH_Control             0x20000000
#define irq_GPIO_Port_F               0x40000000
#define irq_GPIO_Port_G               0x80000000

#define irq_GPIO_Port_H               0x00000001
#define irq_UART2                     0x00000002

#define irq_Timer3_A                  0x00000008
#define irq_Timer3_B                  0x00000010
#define irq_I2C1                      0x00000020
#define irq_QEI1                      0x00000040

#define irq_Ethernet_Controller       0x00000400
#define irq_Hibernation_Module        0x00000800
#define irq_USB_Module                0x00001000

#define irq_GPIO_Port_H               0x00040000
*/

#ifdef _WINDOWS                                                          // {9}
    extern void fnClearInt(void *reg, int iModule, unsigned long bit);
    #define _CLEAR_INTERRUPT(reg, module, bit)            fnClearInt(reg, module, bit)
    #define GPTMICR                   GP_TIMER_0_BLOCK
    #define GPIOICR                   GPIO_PORT_A_BLOCK
    #define UARTICR                   UART_0_BLOCK
    #define A                         0
    #define B                         1
    #define C                         2
    #define D                         3
    #define E                         4
    #define F                         5
    #define G                         6
    #define H                         7
    #define J                         8
#else
    #define _CLEAR_INTERRUPT(reg, module, bit)            reg##_##module = bit
#endif




// Software references used when setting up interrupts
//
#define PORT_INTERRUPT            0
#define TIMER_INTERRUPT           1                                      // {8}
#define ADC_INTERRUPT             2
#define PWM_CONFIGURATION         3                                      // {30}

#define PORTA                     0
#define PORTB                     1
#define PORTC                     2
#define PORTD                     3
#define PORTE                     4
#define PORTF                     5
#define PORTG                     6
#define PORTH                     7
#define PORTJ                     8

#define IRQ_LEVEL_SENSITIVE_HIGH  0
#define IRQ_LEVEL_SENSITIVE_LOW   1
#define IRQ_FALLING_EDGE          2
#define IRQ_RISING_EDGE           3
#define IRQ_BOTH_EDGES            4

#define NO_CHARACTERISTICS        0
#define PULLUP_ON                 1 
#define PULLDOWN_ON               2
#define OPEN_COLLECTOR_ON         3
#define OUTPUT_DRIVE_2MA          4
#define OUTPUT_DRIVE_4MA          5
#define OUTPUT_DRIVE_8MA_NO_SLEW  6
#define OUTPUT_DRIVE_8MA_SLEW_ON  7
#define SET_ADC_TRIGGER           8

#define TIMER_PWM_START_0               0x0001                           // the start bits match with the PWMxEn bits in PWMENABLE
#define TIMER_PWM_START_1               0x0002
#define TIMER_PWM_START_2               0x0004
#define TIMER_PWM_START_3               0x0008
#define TIMER_PWM_START_4               0x0010
#define TIMER_PWM_START_5               0x0020
#define TIMER_PWM_START_6               0x0040
#define TIMER_PWM_START_7               0x0080
#define PWM_DIV_1                       0x0000
#define PWM_DIV_2                       0x0100
#define PWM_DIV_4                       0x0200
#define PWM_DIV_8                       0x0300
#define PWM_DIV_16                      0x0400
#define PWM_DIV_32                      0x0500
#define PWM_DIV_64                      0x0700
#define TIMER_PWM_NO_CONFIG             0x4000
#define TIMER_STOP_PWM                  0x8000
#define TIMER_PWM_STOP_0                (TIMER_PWM_START_0 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_1                (TIMER_PWM_START_1 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_2                (TIMER_PWM_START_2 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_3                (TIMER_PWM_START_3 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_4                (TIMER_PWM_START_4 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_5                (TIMER_PWM_START_5 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_6                (TIMER_PWM_START_6 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_7                (TIMER_PWM_START_7 | TIMER_STOP_PWM)

#define PWM_FREQUENCY_VALUE(frequency_hz, pwm_divide)      (((MASTER_CLOCK/pwm_divide)/frequency_hz) - 1)

// Define interrupt setup structures to suit this processor
//
typedef struct stINTERRUPT_SETUP
{
    void ( *int_handler )( void );                                       // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set
    unsigned char    int_port;                                           // the port to use (PORTA..PORTG)
    unsigned char    int_port_bit;                                       // the bit on the port (eg. PORTA_BIT4)
    unsigned char    int_port_sense;                                     // level sensitive, falling, rising, both
    unsigned char    int_port_characteristic;                            // level sensitive, falling, rising, both
} INTERRUPT_SETUP;


typedef struct stTIMER_INTERRUPT_SETUP                                   // {8}
{
    void ( *int_handler )( void );                                       // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set

    unsigned short   timer_mode;                                         // the mode
    unsigned short   pwm_value;                                          // {13}{17}
    unsigned long    timer_value;                                        // {13} the timer value (depending on mode) - change to long word to support 32 bit mode
    unsigned char    timer_reference;                                    // hardware timer to use
} TIMER_INTERRUPT_SETUP;

#define TIMER_US_VALUE        0x0000
#define TIMER_MS_VALUE        0x0001
#define TIMER_SINGLE_SHOT     0x0000
#define TIMER_PERIODIC        0x0002
#define TIMER_FREQUENCY       0x0004
#define TIMER_TRIGGER_ADC     0x0008
#define TIMER_PWM_A           0x0010                                     // {13}
#define TIMER_PWM_B           0x0020                                     // {13}
#define TIMER_STOP_PWM_A      0x0040                                     // {13}
#define TIMER_STOP_PWM_B      0x0080                                     // {13}
#define TIMER_DONT_DISTURB    0x0100                                     // {13} use only when timer block is already initialised to jump block reset and initialisation
#define TIMER_16BIT_CHANNEL_A 0x0200                                     // {29} use channel A in 16 bit mode
#define TIMER_16BIT_CHANNEL_B 0x0400                                     // {29} use channel B in 16 bit mode


#define _PWM_PERCENT(percent_pwm, frequency_value)       ((frequency_value * percent_pwm)/100)  // {17}
#define _PWM_TENTH_PERCENT(percent_pwm, frequency_value) ((frequency_value * percent_pwm)/1000)

#define TIMER_US_DELAY(usec)                             ((MASTER_CLOCK/1000000)*(usec)) // {13}
#define TIMER_FREQUENCY_VALUE(hertz)                     (1000000 / (hertz))

typedef struct stRTC_SETUP
{
    void(*int_handler)(void);                                           // interrupt handler to be configured
    unsigned short   usYear;
    unsigned char    ucMonthOfYear;
    unsigned char    ucDayOfMonth;
    unsigned char    ucDayOfWeek;
    unsigned char    ucHours;
    unsigned char    ucMinutes;
    unsigned char    ucSeconds;
    unsigned char    command;                                            // identifier for command to perform
    unsigned long    ulLocalUTC;                                         // optional UTC field
} RTC_SETUP;

#if PART_DC8 & ADC0AIN15_PRESENT8
  #define ADC_CHANNELS 16
#elif PART_DC8 & ADC0AIN14_PRESENT8
  #define ADC_CHANNELS 15
#elif PART_DC8 & ADC0AIN13_PRESENT8
  #define ADC_CHANNELS 14
#elif PART_DC8 & ADC0AIN12_PRESENT8
  #define ADC_CHANNELS 13
#elif PART_DC8 & ADC0AIN11_PRESENT8
  #define ADC_CHANNELS 12
#elif PART_DC8 & ADC0AIN10_PRESENT8
  #define ADC_CHANNELS 11
#elif PART_DC8 & ADC0AIN9_PRESENT8
  #define ADC_CHANNELS 10
#elif PART_DC8 & ADC0AIN8_PRESENT8
  #define ADC_CHANNELS 9
#elif PART_DC3 & ADC7_PRESENT3
  #define ADC_CHANNELS 8
#elif PART_DC3 & ADC6_PRESENT3
  #define ADC_CHANNELS 7
#elif PART_DC3 & ADC5_PRESENT3
  #define ADC_CHANNELS 6
#elif PART_DC3 & ADC4_PRESENT3
  #define ADC_CHANNELS 5
#elif PART_DC3 & ADC3_PRESENT3
  #define ADC_CHANNELS 4
#elif PART_DC3 & ADC2_PRESENT3
  #define ADC_CHANNELS 3
#elif PART_DC3 & ADC1_PRESENT3
  #define ADC_CHANNELS 2
#elif PART_DC3 & ADC0_PRESENT3
  #define ADC_CHANNELS 1
#endif


#define ADC_REFERENCE_VOLTAGE           3000
#define ADC_FULL_SCALE                  (0x3ff)
#define ADC_VOLT                        (unsigned short)((ADC_FULL_SCALE * 1000) / ADC_REFERENCE_VOLTAGE)

#define ADC_ENABLE_INTS                 0x00
#define ADC_END_OF_SCAN_INT             0x01
#define ADC_ZERO_CROSSING_INT_POSITIVE  0x02
#define ADC_ZERO_CROSSING_INT_NEGATIVE  0x04
#define ADC_LOW_LIMIT_INT               0x08
#define ADC_SINGLE_SHOT_CROSSING_INT    0x10
#define ADC_HIGH_LIMIT_INT              0x20
#define ADC_SINGLE_SHOT_TRIGGER_INT     0x40
#define ADC_DISABLE_INTS                0x80

#define ADC_SEQUENTIAL_MODE             0x00000000
#define ADC_PARALLEL_MODE               0x00000001
#define ADC_SINGLE_ENDED                0x00000000
#define ADC_DIFFERENTIAL                0x00000002
#define ADC_SINGLE_SHOT_MODE            0x00000000
#define ADC_LOOP_MODE                   0x00000004
#define ADC_TRIGGERED_MODE              0x00000008
#define ADC_START_OPERATION             0x00000010
#define ADC_STOP_OPERATION              0x00000020
#define ADC_SIMULT_MODE                 0x00000040
#define ADC_HIGH_REF_AN2                0x00000080
#define ADC_LOW_REF_AN6                 0x00000100
#define ADC_HIGH_REF_VRH                0x00000000
#define ADC_LOW_REF_VRL                 0x00000000
#define ADC_READ_ONLY                   0x00001000
#define ADC_GET_RESULT                  0x00002000
#define ADC_ALL_RESULTS                 0x00004000
#define ADC_CONFIGURE_ADC               0x00008000
#define ADC_TRIGGER_GPIO_PB4_RISING     0x00010000
#define ADC_TRIGGER_GPIO_PB4_FALLING    0x00020000
#define ADC_CONFIGURE_CHANNEL           0x00040000
#define ADC_DISABLE_CHANNEL             0x00080000
#define ADC_DISABLE_ADC                 0x00100000
#define ADC_SET_CHANNEL_OFFSET          0x00200000 
#define ADC_TRIGGER_PWM_0               0x00800000
#define ADC_TRIGGER_PWM_1               0x04000000
#define ADC_TRIGGER_PWM_2               0x08000000
#define ADC_TRIGGER_PWM_3               0x0c000000
#define ADC_TRIGGER_TIMER               0x20000000
#define ADC_TRIGGER_COMPARATOR_0        0x40000000
#define ADC_TRIGGER_COMPARATOR_1        0x80000000
#define ADC_TRIGGER_COMPARATOR_2        0xc0000000

// Define interrupt setup structure to suit this processor
//
typedef struct stADC_SETUP
{
  void ( *int_handler )( void );                                         // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set
  unsigned long    int_adc_single_ended_inputs;                          // the ADC inputs to be sampled (one bit per channel, up to 8 channels possible)
  unsigned long    int_adc_differential_inputs;                          // the ADC inputs to be sampled (one bit per channel pair, up to 4 channels possible)
  unsigned long    int_adc_mode;                                         // single or multi-shot and trigger type
  unsigned short   int_sequence_count;                                   // the number of sequences to be sampled
  unsigned char    int_adc_averaging;                                    // HW filtering
  unsigned short  *int_adc_result;                                       // location to save samples to
} ADC_SETUP;


#define PORT_WIDTH          8                                            // GPIO ports are 8 bits in width

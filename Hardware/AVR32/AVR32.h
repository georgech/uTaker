/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      AVR32.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    18.11.2009 Add _AT32UC3B support                                     {1}
    24.12.2009 Add SPI pin definitions                                   {2}
    05.06.2010 Add USBB                                                  {3}
    10.08.2010 Add SDCARD_MALLOC()                                       {4}
    18.10.2010 Add port characteristics macros                           {5}
    18.10.2010 Add optional hardware timer mode                          {6}
    08.11.2010 Add SDRAM controller and change EIB to EBI                {7}
    07.06.2011 Add ADC_INTERRUPT                                         {8}
    10.06.2011 Add 9 bit USART mode                                      {9}
    19.06.2011 Add _AT32UC3C                                             {10}
    24.06.2011 USBB operational                                          {11}
    25.06.2011 Add _EXCEPTION()                                          {12}
    10.01.2012 Add additional MACB_NCFGR register bit defines            {13}
    20.06.2012 Correct MACB_USRIO bit defines                            {14}
    03.07.2012 Add SPI0 alternate pins                                   {15}
    19.07.2012 Add RTC                                                   {16}
    06.08.2012 Replaced ODMER by PDER registers for _AT32UC3C            {17}
    10.11.2012 Correct port macros' use of OVR                           {18}
    17.12.2012 Extend UC3C SPI pin defines                               {19}
    17.12.2012 Add additional UC3C watchdog timer flags                  {20}

*/

#if defined _WINDOWS
    #include "SimAVR32.h"
    #define _SIM_CHANGE(x)               fnSimPortsAVR(x)
    #define _EXCEPTION(x)                *(unsigned char *)0 = 0         // generate exception when simulating {12}
#else
    #if defined COMPILE_IAR
        #include <intrinsics.h>
    #endif
    #define _SIM_CHANGE(x)
    #define _EXCEPTION(x)                                                // ignore on target {12}
#endif

#if defined _GNU
    #define _PACK          __attribute__((__packed__))
#else
    #define _PACK               
#endif
#define __PACK_ON
#define __PACK_OFF

#define SAVE_COMPLETE_FLASH                                              // when simulating save complete FLASH content rather than only file system content

#define _ALIGN_HEAP_4
#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary

#define SDCARD_MALLOC(x) uMalloc(x)                                      // use standard uMalloc() {4}

#define _BIG_ENDIAN                                                      // compile and operate project in BIG ENDIAN mode

#define SLOW_CLOCK       115000
#define MASTER_CLOCK     (PLL_OUTPUT_FREQ_INT/CPU_HSB_DIVIDE)
#define PBA_CLOCK        (PLL_OUTPUT_FREQ_INT/PBA_DIVIDE)
#define PBB_CLOCK        (PLL_OUTPUT_FREQ_INT/PBB_DIVIDE)
#define PLL_OUTPUT_FREQ  MASTER_CLOCK 

#define CAST_POINTER_ARITHMETIC unsigned long                            // AVR32 uses 32 bit pointers
 
#define FLASH_START_ADDRESS            0x80000000
#define RAM_START_ADDRESS              0x00000000
#define USER_PAGE_SIZE                 512

#if defined _AT32UC3B                                                    // {1}
    #define MAX_CLOCK_WITHOUT_FLASH_WAITSTATE     30000000
    #define START_HIGH_PLL_OUTPUT_RANGE           170000000
#elif defined _AT32UC3C                                                  // {10}
    #define MIN_CLOCK_WITH_HIGHSPEED_READ_MODE    66000000
    #define MAX_CLOCK_WITHOUT_FLASH_WAITSTATE     33000000
    #define START_HIGH_PLL_OUTPUT_RANGE           170000000
#else
    #define MAX_CLOCK_WITHOUT_FLASH_WAITSTATE     33000000
    #define START_HIGH_PLL_OUTPUT_RANGE           170000000
#endif

#define LONG_UMEMSET                                                     // enable long versions of these functions since they are required by FLASH routines
#define LONG_UMEMCPY

/*****************************************************************************************************/


// Peripheral register blocks
//
#if defined _WINDOWS
  #define USBB_CONFIG_BLOCK              ((unsigned char *)(&ucAVR32.ucUSB))    // USB configuration
  #define HMATRIX_BLOCK                  ((unsigned char *)(&ucAVR32.ucHSBMatrix)) // HMATRIX configuration
  #define FLASHC_BLOCK                   ((unsigned char *)(&ucAVR32.ucFlashc)) // FLASH controller
  #define MACB_BLOCK                     ((unsigned char *)(&ucAVR32.ucMACB))   // MACB configuration
  #define SMC_BLOCK                      ((unsigned char *)(&ucAVR32.ucSMC))    // Static memory controller
  #define SDRAMC_BLOCK                   ((unsigned char *)(&ucAVR32.ucSDRAM))  // SDRAM memory controller
  #define PDCA_BLOCK                     ((unsigned char *)(&ucAVR32.ucPDCA))   // Peripheral DMA interface
  #define INTC_BLOCK                     ((unsigned char *)(&ucAVR32.ucIntc))   // Interrupt controller interface
  #define PM_BLOCK                       ((unsigned char *)(&ucAVR32.ucPower))  // Power manager
  #define SCIF_BLOCK                     ((unsigned char *)(&ucAVR32.ucSCIF))   // System Control interface
  #define RTC_BLOCK                      ((unsigned char *)(&ucAVR32.ucRTC))    // Real Time Clock
  #define WDT_BLOCK                      ((unsigned char *)(&ucAVR32.ucWDT))    // Watchdog timer
  #define EIC_BLOCK                      ((unsigned char *)(&ucAVR32.ucEIC))    // External interrupt controller
  #define GPIO_BLOCK                     ((unsigned char *)(&ucAVR32.ucGPIO))   // General purpose IO controller
  #define USART0_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucSimUart[0])) // Universal synchronouse/asynchronous receiver/transmitter 0
  #define USART1_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucSimUart[1])) // Universal synchronouse/asynchronous receiver/transmitter 1
  #define USART2_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucSimUart[2])) // Universal synchronouse/asynchronous receiver/transmitter 2
  #define USART3_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucSimUart[3])) // Universal synchronouse/asynchronous receiver/transmitter 3
#ifdef _AT32UC3C
  #define USART4_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucSimUart[4])) // Universal synchronouse/asynchronous receiver/transmitter 3
#endif
  #define SPI0_BLOCK                     ((unsigned char *)(&ucAVR32.ucSimSPI[0]))  // Serial peripheral interface 0
  #define SPI1_BLOCK                     ((unsigned char *)(&ucAVR32.ucSimSPI[1]))  // Serial peripheral interface 1
  #define TWI_BLOCK                      ((unsigned char *)(&ucAVR32.ucTWI))    // Two wire interface
  #define PWM_BLOCK                      ((unsigned char *)(&ucAVR32.ucPWM))    // Pluse width modulation controller
  #define SSC_BLOCK                      ((unsigned char *)(&ucAVR32.ucSCC))    // Synchronous serial controller
  #define TIMER0_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucTimer0))
  #define TIMER1_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucTimer1))
  #define TIMER2_PERIPHERAL_BLOCK        ((unsigned char *)(&ucAVR32.ucTimer2))
  #define TIMER_CONTROL_BLOCK            ((unsigned char *)(&ucAVR32.ucTimerCtl))
  #define ADC_BLOCK                      ((unsigned char *)(&ucAVR32.ucADC))    // Analog to digital converter
 
  #define USBB_HSB_0                     (volatile unsigned char *)0xd0000000 // USBB dual-port RAM
  #define USBB_HSB_1                     (volatile unsigned char *)0xd0010000
  #define USBB_HSB_2                     (volatile unsigned char *)0xd0020000
  #define USBB_HSB_3                     (volatile unsigned char *)0xd0030000
  #define USBB_HSB_4                     (volatile unsigned char *)0xd0040000
  #define USBB_HSB_5                     (volatile unsigned char *)0xd0050000
  #define USBB_HSB_6                     (volatile unsigned char *)0xd0060000
#elif defined _AT32UC3C
  #define PDCA_BLOCK                     0xfffd0000                      // Peripheral DMA Controller
  #define MDMA_BLOCK                     0xfffd1000                      // Memory DMA-MDMA
  #define USART1_PERIPHERAL_BLOCK        0xfffd1400                      // Universal synchronouse/asynchronous receiver/transmitter 1
  #define SPI0_BLOCK                     0xfffd1800                      // Serial peripheral interface 0
  #define CANIF_BLOCK                    0xfffd1c00                      // CAN interface
  #define TIMER0_PERIPHERAL_BLOCK        0xfffd2000                      // Timer/counter 0
  #define ADCIFA_BLOCK                   0xfffd2400                      // Analog to digital converter with touch functionality
  #define USART4_PERIPHERAL_BLOCK        0xfffd2800                      // Universal synchronouse/asynchronous receiver/transmitter 4
  #define TWIM2_BLOCK                    0xfffd2c00                      // Two wire interface 2 - master
  #define TWIS2_BLOCK                    0xfffd3000                      // Two wire interface 2 - slave
  #define FLASHC_BLOCK                   0xfffe0000                      // FLASH controller
  #define USBB_CONFIG_BLOCK              0xfffe1000                      // USB(C) configuration
  #define HMATRIX_BLOCK                  0xfffe2000                      // HMATRIX configuration
  #define SAU_BLOCK                      0xfffe2400                      // Secure Access Unit
  #define SMC_BLOCK                      0xfffe2800                      // Static memory controller
  #define SDRAMC_BLOCK                   0xfffe2c00                      // SDRAM memory controller
  #define MACB_BLOCK                     0xfffe3000                      // MACB configuration
  #define INTC_BLOCK                     0xffff0000                      // Interrupt controller interface
  #define PM_BLOCK                       0xffff0400                      // Power manager
  #define SCIF_BLOCK                     0xffff0800                      // System Control interface
  #define AST_BLOCK                      0xffff0c00                      // Asynchronous Timer
  #define WDT_BLOCK                      0xffff1000                      // Watchdog timer
  #define EIC_BLOCK                      0xffff1400                      // External interrupt controller
  #define FREQM_BLOCK                    0xffff1800                      // Frequency Meter
  #define GPIO_BLOCK                     0xffff2000                      // General purpose IO controller
  #define USART0_PERIPHERAL_BLOCK        0xffff2800                      // Universal synchronouse/asynchronous receiver/transmitter 0
  #define USART2_PERIPHERAL_BLOCK        0xffff2c00                      // Universal synchronouse/asynchronous receiver/transmitter 2
  #define USART3_PERIPHERAL_BLOCK        0xffff3000                      // Universal synchronouse/asynchronous receiver/transmitter 3
  #define SPI1_BLOCK                     0xffff3400                      // Serial peripheral interface 1
  #define TWIM0_BLOCK                    0xffff3800                      // Two wire interface 0 - master
  #define TWIM1_BLOCK                    0xffff3c00                      // Two wire interface 1 - master
  #define TWIS0_BLOCK                    0xffff4000                      // Two wire interface 0 - slave
  #define TWIS1_BLOCK                    0xffff4400                      // Two wire interface 1 - slave
  #define IISC_BLOCK                     0xffff4800                      // I2S controller
  #define PWM_BLOCK                      0xffff4c00                      // Pluse width modulation controller
  #define QDEC0_BLOCK                    0xffff5000                      // Quadrature Decoder 0
  #define QDEC1_BLOCK                    0xffff5400                      // Quadrature Decoder 1
  #define TIMER1_PERIPHERAL_BLOCK        0xffff5800                      // Timer/counter 1
  #define PEVC_BLOCK                     0xffff5c00                      // Parallel Event Controller
  #define ACIFA0_BLOCK                   0xffff6000                      // Analogue Comparators Interface 0
  #define ACIFA1_BLOCK                   0xffff6400                      // Analogue Comparators Interface 1
  #define DACIFB0_BLOCK                  0xffff6800                      // DAC interface 0
  #define DACIFB1_BLOCK                  0xffff6c00                      // DAC interface 1
  #define AW_BLOCK                       0xffff7000                      // aWire
#else
  #define USBB_CONFIG_BLOCK              0xfffe0000                      // USB configuration
  #define HMATRIX_BLOCK                  0xfffe1000                      // HMATRIX configuration
  #define FLASHC_BLOCK                   0xfffe1400                      // FLASH controller
  #define MACB_BLOCK                     0xfffe1800                      // MACB configuration
  #define SMC_BLOCK                      0xfffe1c00                      // Static memory controller
  #define SDRAMC_BLOCK                   0xfffe2000                      // SDRAM memory controller
  #define PDCA_BLOCK                     0xffff0000                      // Peripheral DMA interface
  #define INTC_BLOCK                     0xffff0800                      // Interrupt controller interface
  #define PM_BLOCK                       0xffff0c00                      // Power manager
  #define RTC_BLOCK                      0xffff0d00                      // Real Time Clock
  #define WDT_BLOCK                      0xffff0d30                      // Watchdog timer
  #define EIC_BLOCK                      0xffff0d80                      // External interrupt controller
  #define GPIO_BLOCK                     0xffff1000                      // General purpose IO controller
  #define USART0_PERIPHERAL_BLOCK        0xffff1400                      // Universal synchronouse/asynchronous receiver/transmitter 0
  #define USART1_PERIPHERAL_BLOCK        0xffff1800                      // Universal synchronouse/asynchronous receiver/transmitter 1
  #define USART2_PERIPHERAL_BLOCK        0xffff1c00                      // Universal synchronouse/asynchronous receiver/transmitter 2
  #define USART3_PERIPHERAL_BLOCK        0xffff2000                      // Universal synchronouse/asynchronous receiver/transmitter 3
  #define SPI0_BLOCK                     0xffff2400                      // Serial peripheral interface 0
  #define SPI1_BLOCK                     0xffff2800                      // Serial peripheral interface 1
  #define TWI_BLOCK                      0xffff2c00                      // Two wire interface
  #define PWM_BLOCK                      0xffff3000                      // Pluse width modulation controller
  #define SSC_BLOCK                      0xffff3400                      // Synchronous serial controller
  #define TIMER0_PERIPHERAL_BLOCK        0xffff3800                      // Timer/counter
  #define TIMER1_PERIPHERAL_BLOCK        0xffff3840
  #define TIMER2_PERIPHERAL_BLOCK        0xffff3880
  #define TIMER_CONTROL_BLOCK            0xffff38c0
  #define ADC_BLOCK                      0xffff3c00                      // Analog to digital converter

    #ifdef _AT32UC3B
        #define USBB_HSB_0               (volatile unsigned char *)0xd0000000 // USBB dual-port RAM
        #define USBB_HSB_1               (volatile unsigned char *)0xd0010000
        #define USBB_HSB_2               (volatile unsigned char *)0xd0020000
        #define USBB_HSB_3               (volatile unsigned char *)0xd0030000
        #define USBB_HSB_4               (volatile unsigned char *)0xd0040000
        #define USBB_HSB_5               (volatile unsigned char *)0xd0050000
        #define USBB_HSB_6               (volatile unsigned char *)0xd0060000
    #else
        #define USBB_HSB_0               (volatile unsigned char *)0xe0000000
        #define USBB_HSB_1               (volatile unsigned char *)0xe0010000
        #define USBB_HSB_2               (volatile unsigned char *)0xe0020000
        #define USBB_HSB_3               (volatile unsigned char *)0xe0030000
        #define USBB_HSB_4               (volatile unsigned char *)0xe0040000
        #define USBB_HSB_5               (volatile unsigned char *)0xe0050000
        #define USBB_HSB_6               (volatile unsigned char *)0xe0060000
    #endif
#endif


// Peripheral identifiers
//
#define AIC_FIQ                          0x00000001                      // Advanced Interrupt Controller - fast external interrupt
#define SYSIRQ                           0x00000002                      //
#define PIOA                             0x00000004                      // Parallel Input/Output Controller A
#define PIOB                             0x00000008                      // Parallel Input/Output Controller B
#define SPI0                             0x00000010                      // Serial Peripheral Interface 0
#define SPI1                             0x00000020                      // Serial Peripheral Interface 1
#define USART0                           0x00000040                      // USART 0
#define USART1                           0x00000080                      // USART 1
#define USART2                           0x00000040                      // USART 2 - TEMP AVR32
#define USART3                           0x00000080                      // USART 3 - TEMP AVR32
#define SSC                              0x00000100                      // Synchronous Serial Controller
#define TWI                              0x00000200                      // Two Wire Interface (I2C)
#define PWMC                             0x00000400                      // Pulse Width Modulator Controller
#define UDP                              0x00000800                      // USB Device Port
#define TC0                              0x00001000                      // Timer Counter 0
#define TC1                              0x00002000                      // Timer Counter 1
#define TC2                              0x00004000                      // Timer Counter 2
#define CAN_CTL                          0x00008000                      // CAN Controller
#define EMAC                             0x00010000                      // EMAC
#define ADC                              0x00020000                      // Analog to Digital Converter
#define AES128                           0x00040000                      // Advanced Encryption Standard 128-bit
#define TDES                             0x00080000                      // Tripple Data Encryption Standard
                                                                         // 20..29 reserved
#define AIC_IRQ0                         0x40000000                      // Advanced Interrupt Controller - external interrupt 0
#define AIC_IRQ1                         0x80000000                      // Advanced Interrupt Controller - external interrupt 1


#define POWER_UP(bus, module)            PM_##bus##MASK |= module;                        // power up a module (apply clock to it)
#define POWER_DOWN(bus, module)          PM_##bus##MASK &= ~module;                       // power down a module (disable clock to it)

// USBB
//
#define UDCON                            *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x000) // Device General Control Register
  #define UDCON_ADDMASK                  0x0000007f                                       // address mask
  #define UDCON_ADDEN                    0x00000080                                       // address enabled
  #define UDCON_DETACH                   0x00000100                                       // device physically detached
  #define UDCON_LS                       0x00001000                                       // low speed mode
#define UDINT                            *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x004) // Device Global Interrupt Register (read-only)
#define UDINTCLR                         *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x008) // Device Global Interrupt Clear Register (write-only)
#define UDINTSET                         *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x00c) // Device Global Interrupt Set Register (write-only)
#define UDINTE                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x010) // Device Global Interrupt Enable Register (read-only)
#define UDINTECLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x014) // Device Global Interrupt Enable Clear Register (write-only)
#define UDINTESET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x018) // Device Global Interrupt Enable Set Register (write-only)
  #define UDINT_SUSP                     0x00000001                                       // suspend interrupt
  #define UDINT_SOF                      0x00000004                                       // start of frame interrupt
  #define UDINT_EORST                    0x00000008                                       // end of reset interrupt
  #define UDINT_WAKEUP                   0x00000010                                       // wakeup interrupt
  #define UDINT_EORSM                    0x00000020                                       // end of resume interrupt
  #define UDINT_UPRSM                    0x00000040                                       // upstream resume interrupt
  #define UDINT_EP0                      0x00001000                                       // endpoint 0 interrupt
  #define UDINT_EP1                      0x00002000                                       // endpoint 1 interrupt
  #define UDINT_EP2                      0x00004000                                       // endpoint 2 interrupt
  #define UDINT_EP3                      0x00008000                                       // endpoint 3 interrupt
  #define UDINT_EP4                      0x00010000                                       // endpoint 4 interrupt
  #define UDINT_EP5                      0x00020000                                       // endpoint 5 interrupt
  #define UDINT_EP6                      0x00040000                                       // endpoint 6 interrupt
  #define UDINT_DMA1                     0x02000000                                       // DMA 1 interrupt
  #define UDINT_DMA2                     0x04000000                                       // DMA 2 interrupt
  #define UDINT_DMA3                     0x08000000                                       // DMA 3 interrupt
  #define UDINT_DMA4                     0x10000000                                       // DMA 4 interrupt
  #define UDINT_DMA5                     0x20000000                                       // DMA 5 interrupt
  #define UDINT_DMA6                     0x40000000                                       // DMA 6 interrupt
#define UERST                            *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x01c) // Endpoint Enable/Reset Register
  #define EPEN0                          0x00000001                                       // endpoint 0 enable
  #define EPEN1                          0x00000002                                       // endpoint 1 enable
  #define EPEN2                          0x00000004                                       // endpoint 2 enable
  #define EPEN3                          0x00000008                                       // endpoint 3 enable
  #define EPEN4                          0x00000010                                       // endpoint 4 enable
  #define EPEN5                          0x00000020                                       // endpoint 5 enable
  #define EPEN6                          0x00000040                                       // endpoint 6 enable
  #define EPRST0                         0x00000100                                       // endpoint 0 reset
  #define EPRST1                         0x00000200                                       // endpoint 1 reset
  #define EPRST2                         0x00000400                                       // endpoint 2 reset
  #define EPRST3                         0x00000800                                       // endpoint 3 reset
  #define EPRST4                         0x00001000                                       // endpoint 4 reset
  #define EPRST5                         0x00002000                                       // endpoint 5 reset
  #define EPRST6                         0x00004000                                       // endpoint 6 reset
#define UDFNUM                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x020) // Device Frame Number Register (read-only)

#define UECFG0_ADR                       (unsigned long*)(USBB_CONFIG_BLOCK + 0x100)      // Endpoint 0 Configuration Register address
#define UECFG0                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x100)     // Endpoint 0 Configuration Register
  #define UECFG_ALLOC                    0x00000002                                       // endpoint memory allocate
  #define UECFG_EPBK_SINGLE_BANK         0x00000000                                       // endpoint has single bank
  #define UECFG_EPBK_DOUBLE_BANK         0x00000004                                       // endpoint has double bank
  #define UECFG_EPBK_TRIPPLE_BANK        0x00000008                                       // endpoint has tripple bank
  #define UECFG_EPSIZE_8                 0x00000000                                       // size of each bank used by the endpoint - 8 bytes
  #define UECFG_EPSIZE_16                0x00000010                                       // 16 bytes
  #define UECFG_EPSIZE_32                0x00000020                                       // 32 bytes
  #define UECFG_EPSIZE_64                0x00000030                                       // 64 bytes
  #define UECFG_EPSIZE_128               0x00000040                                       // 128 bytes
  #define UECFG_EPSIZE_256               0x00000050                                       // 256 bytes
  #define UECFG_EPSIZE_512               0x00000060                                       // 512 bytes
  #define UECFG_EPSIZE_1024              0x00000070                                       // 1024 bytes
  #define UECFG_EPDIR_OUT                0x00000000                                       // OUT endpoint
  #define UECFG_EPDIR_IN                 0x00000100                                       // IN endpoint
  #define UECFG_AUTOSW                   0x00000200                                       // automatically switch bank when it is ready
  #define UECFG_EPTYPE_CONTROL           0x00000000                                       // control enpoint
  #define UECFG_EPTYPE_ISO               0x00000800                                       // isochronous endpoint
  #define UECFG_EPTYPE_BULK              0x00001000                                       // bulk endpoint
  #define UECFG_EPTYPE_INTERRUPT         0x00001800                                       // interrupt endpoint
  #define UECFG_EPTYPE_MASK              0x00001800
#define UECFG1                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x104)     // Endpoint 1 Configuration Register
#define UECFG2                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x108)     // Endpoint 2 Configuration Register
#define UECFG3                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x10c)     // Endpoint 3 Configuration Register
#define UECFG4                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x110)     // Endpoint 4 Configuration Register
#define UECFG5                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x114)     // Endpoint 5 Configuration Register
#define UECFG6                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x118)     // Endpoint 6 Configuration Register

#define ADD_UESTA0                       (volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x130)
#define UESTA0                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x130) // Endpoint 0 Status Register (read-only)
  #define TXINI                          0x00000001                                       // transmitter empty
  #define RXOUTI                         0x00000002                                       // data received
  #define RXSTPI                         0x00000004                                       // SETUP token received
  #define NAKOUTI                        0x00000008                                       // 
  #define NAKINI                         0x00000010                                       // 
  #define OVERFI                         0x00000020                                       // 
  #define STALLEDI                       0x00000040                                       // 
  #define SHORTPACKET                    0x00000080                                       // 
  #define DTSEQ_0                        0x00000000                                       // 
  #define DTSEQ_1                        0x00000100                                       // 
  #define NBUSYBK_0                      0x00000000                                       // 
  #define NBUSYBK_1                      0x00001000                                       // 
  #define NBUSYBK_2                      0x00002000                                       // 
  #define NBUSYBK_3                      0x00003000                                       // 
  #define CURRBK_0                       0x00000000                                       // 
  #define CURRBK_1                       0x00004000                                       // 
  #define CURRBK_2                       0x00008000                                       // 
  #define RWALL                          0x00010000                                       // Read/Write allowed (not for control endpoint use)
  #define CTRLDIR_OUT                    0x00000000                                       // 
  #define CTRLDIR_IN                     0x00020000                                       // 
  #define CFGOK                          0x00040000                                       //
  #define BYCT_MASK                      0x7ff00000                                       //
  #define BYCT_SHIFT                     20
#define UESTA1                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x134) // Endpoint 1 Status Register (read-only)
#define UESTA2                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x138) // Endpoint 2 Status Register (read-only)
#define UESTA3                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x13c) // Endpoint 3 Status Register (read-only)
#define UESTA4                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x140) // Endpoint 4 Status Register (read-only)
#define UESTA5                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x144) // Endpoint 5 Status Register (read-only)
#define UESTA6                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x148) // Endpoint 6 Status Register (read-only)

#define UESTA0CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x160) // Endpoint 0 Status Clear Register (write-only)
#define UESTA1CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x164) // Endpoint 1 Status Clear Register (write-only)
#define UESTA2CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x168) // Endpoint 2 Status Clear Register (write-only)
#define UESTA3CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x16c) // Endpoint 3 Status Clear Register (write-only)
#define UESTA4CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x170) // Endpoint 4 Status Clear Register (write-only)
#define UESTA5CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x174) // Endpoint 5 Status Clear Register (write-only)
#define UESTA6CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x178) // Endpoint 6 Status Clear Register (write-only)

#define UESTA0SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x190) // Endpoint 0 Status Set Register (write-only)
#define UESTA1SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x194) // Endpoint 1 Status Set Register (write-only)
#define UESTA2SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x198) // Endpoint 2 Status Set Register (write-only)
#define UESTA3SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x19c) // Endpoint 3 Status Set Register (write-only)
#define UESTA4SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1a0) // Endpoint 4 Status Set Register (write-only)
#define UESTA5SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1a4) // Endpoint 5 Status Set Register (write-only)
#define UESTA6SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1a8) // Endpoint 6 Status Set Register (write-only)

#define UECON0                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1c0) // Endpoint 0 Control Register
  #define TXINE                          0x00000001                                       // transmitted IN data interrupt enable
  #define RXOUTE                         0x00000002                                       // received OUT data interrupt enable
  #define RXSTPE                         0x00000004                                       // received SETUP interrupt flag
  #define NAKOUTE                        0x00000008                                       // 
  #define NAKINE                         0x00000010                                       // 
  #define OVERFE                         0x00000020                                       // 
  #define STALLEDE                       0x00000040                                       // 
  #define SHORTPACKETE                   0x00000080                                       // 
  #define NBUSYBKE                       0x00001000                                       // 
  #define KILLBK                         0x00002000                                       // 
  #define FIFOCON                        0x00004000                                       // 
  #define EPDISHDMA                      0x00010000                                       // 
  #define RSTDT                          0x00040000                                       // 
  #define STALLRQ                        0x00080000                                       // 
#define UECON1                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x1c4)     // Endpoint 1 Control Register
#define UECON2                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x1c8)     // Endpoint 2 Control Register
#define UECON3                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x1cc)     // Endpoint 3 Control Register
#define UECON4                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x1d0)     // Endpoint 4 Control Register
#define UECON5                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x1d4)     // Endpoint 5 Control Register
#define UECON6                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x1d8)     // Endpoint 6 Control Register

#define UECON0SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1f0) // Endpoint 0 Control Set Register (write-only)
#define UECON1SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1f4) // Endpoint 1 Control Set Register (write-only)
#define UECON2SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1f8) // Endpoint 2 Control Set Register (write-only)
#define UECON3SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x1fc) // Endpoint 3 Control Set Register (write-only)
#define UECON4SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x200) // Endpoint 4 Control Set Register (write-only)
#define UECON5SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x204) // Endpoint 5 Control Set Register (write-only)
#define UECON6SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x208) // Endpoint 6 Control Set Register (write-only)

#define UECON0CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x220) // Endpoint 0 Control Clear Register (write-only)
#define UECON1CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x224) // Endpoint 1 Control Clear Register (write-only)
#define UECON2CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x228) // Endpoint 2 Control Clear Register (write-only)
#define UECON3CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x22c) // Endpoint 3 Control Clear Register (write-only)
#define UECON4CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x230) // Endpoint 4 Control Clear Register (write-only)
#define UECON5CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x234) // Endpoint 5 Control Clear Register (write-only)
#define UECON6CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x238) // Endpoint 6 Control Clear Register (write-only)

#define UDDMA1_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x310)     // Device DMA Channel 1 Next Descriptor Address Register
#define UDDMA1_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x314)     // Device DMA Channel 1 HSB Address Register
#define UDDMA1_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x318)     // Device DMA Channel 1 Control Register
#define UDDMA1_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x31c)     // Device DMA Channel 1 Status Register
#define UDDMA2_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x320)     // Device DMA Channel 2 Next Descriptor Address Register
#define UDDMA2_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x324)     // Device DMA Channel 2 HSB Address Register
#define UDDMA2_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x328)     // Device DMA Channel 2 Control Register
#define UDDMA2_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x32c)     // Device DMA Channel 2 Status Register
#define UDDMA3_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x330)     // Device DMA Channel 3 Next Descriptor Address Register
#define UDDMA3_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x334)     // Device DMA Channel 3 HSB Address Register
#define UDDMA3_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x338)     // Device DMA Channel 3 Control Register
#define UDDMA3_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x33c)     // Device DMA Channel 3 Status Register
#define UDDMA4_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x340)     // Device DMA Channel 4 Next Descriptor Address Register
#define UDDMA4_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x344)     // Device DMA Channel 4 HSB Address Register
#define UDDMA4_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x348)     // Device DMA Channel 4 Control Register
#define UDDMA4_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x34c)     // Device DMA Channel 4 Status Register
#define UDDMA5_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x350)     // Device DMA Channel 5 Next Descriptor Address Register
#define UDDMA5_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x354)     // Device DMA Channel 5 HSB Address Register
#define UDDMA5_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x358)     // Device DMA Channel 5 Control Register
#define UDDMA5_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x35c)     // Device DMA Channel 5 Status Register
#define UDDMA6_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x360)     // Device DMA Channel 6 Next Descriptor Address Register
#define UDDMA6_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x364)     // Device DMA Channel 6 HSB Address Register
#define UDDMA6_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x368)     // Device DMA Channel 6 Control Register
#define UDDMA6_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x36c)     // Device DMA Channel 6 Status Register

#define UHCON                            *(unsigned long*)(USBB_CONFIG_BLOCK + 0x400)     // Host General Control Register
#define UHINT                            *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x404) // Host Global Interrupt Register (read-only)
#define UHINTCLR                         *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x408) // Host Global Interrupt Clear Register (write-only)
#define UHINTSET                         *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x40c) // Host Global Interrupt Set Register (write-only)
#define UHINTE                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x410) // Host Global Interrupt Enable Register (read-only)
#define UHINTECLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x414) // Host Global Interrupt Enable Clear Register (write-only)
#define UHINTESET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x418) // Host Global Interrupt Enable Set Register (write-only)
#define UPRST                            *(unsigned long*)(USBB_CONFIG_BLOCK + 0x41c)     // Pipe Enable/Reset Register
#define UHFNUM                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x420)     // Host Frame Number Register
#define UHADDR1                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x424)     // Host Address 1 Register
#define UHADDR2                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x428)     // Host Address 2 Register

#define UPCFG0                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x500)     // Pipe 0 Configuration Register
#define UPCFG1                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x504)     // Pipe 1 Configuration Register
#define UPCFG2                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x508)     // Pipe 2 Configuration Register
#define UPCFG3                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x50c)     // Pipe 3 Configuration Register
#define UPCFG4                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x510)     // Pipe 4 Configuration Register
#define UPCFG5                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x514)     // Pipe 5 Configuration Register
#define UPCFG6                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x518)     // Pipe 6 Configuration Register

#define UPSTA0                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x530) // Pipe 0 Status Register (read-only)
  #define RXBYTECNT_SHIFT                20
  #define RXINE                          0x00000001                                       // USB message available in current bank of pipe
  #define TXOUTE                         0x00000002                                       // current OUT bank is free and can be filled
#define UPSTA1                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x534) // Pipe 1 Status Register (read-only)
#define UPSTA2                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x538) // Pipe 2 Status Register (read-only)
#define UPSTA3                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x53c) // Pipe 3 Status Register (read-only)
#define UPSTA4                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x540) // Pipe 4 Status Register (read-only)
#define UPSTA5                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x544) // Pipe 5 Status Register (read-only)
#define UPSTA6                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x548) // Pipe 6 Status Register (read-only)

#define SPSTA0CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x560) // Pipe 0 Status Clear Register (write-only)
#define SPSTA1CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x564) // Pipe 1 Status Clear Register (write-only)
#define SPSTA2CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x568) // Pipe 2 Status Clear Register (write-only)
#define SPSTA3CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x56c) // Pipe 3 Status Clear Register (write-only)
#define SPSTA4CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x570) // Pipe 4 Status Clear Register (write-only)
#define SPSTA5CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x574) // Pipe 5 Status Clear Register (write-only)
#define SPSTA6CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x578) // Pipe 6 Status Clear Register (write-only)

#define SPSTA0SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x590) // Pipe 0 Status Set Register (write-only)
#define SPSTA1SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x594) // Pipe 1 Status Set Register (write-only)
#define SPSTA2SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x598) // Pipe 2 Status Set Register (write-only)
#define SPSTA3SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x59c) // Pipe 3 Status Set Register (write-only)
#define SPSTA4SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5a0) // Pipe 4 Status Set Register (write-only)
#define SPSTA5SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5a4) // Pipe 5 Status Set Register (write-only)
#define SPSTA6SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5a8) // Pipe 6 Status Set Register (write-only)

#define UPCON0                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5c0) // Pipe 0 Control Register (read-only)
#define UPCON1                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5c4) // Pipe 1 Control Register (read-only)
#define UPCON2                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5c8) // Pipe 2 Control Register (read-only)
#define UPCON3                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5cc) // Pipe 3 Control Register (read-only)
#define UPCON4                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5d0) // Pipe 4 Control Register (read-only)
#define UPCON5                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5d4) // Pipe 5 Control Register (read-only)
#define UPCON6                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5d8) // Pipe 6 Control Register (read-only)

#define UPCON0SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5f0) // Pipe 0 Control Set Register (write-only)
#define UPCON1SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5f4) // Pipe 1 Control Set Register (write-only)
#define UPCON2SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5f8) // Pipe 2 Control Set Register (write-only)
#define UPCON3SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x5fc) // Pipe 3 Control Set Register (write-only)
#define UPCON4SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x600) // Pipe 4 Control Set Register (write-only)
#define UPCON5SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x604) // Pipe 5 Control Set Register (write-only)
#define UPCON6SET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x608) // Pipe 6 Control Set Register (write-only)

#define UPCON0CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x620) // Pipe 0 Control Clear Register (write-only)
#define UPCON1CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x624) // Pipe 1 Control Clear Register (write-only)
#define UPCON2CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x628) // Pipe 2 Control Clear Register (write-only)
#define UPCON3CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x62c) // Pipe 3 Control Clear Register (write-only)
#define UPCON4CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x630) // Pipe 4 Control Clear Register (write-only)
#define UPCON5CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x634) // Pipe 5 Control Clear Register (write-only)
#define UPCON6CLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x638) // Pipe 6 Control Clear Register (write-only)

#define UPINRQ0                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x650)     // Pipe 0 IN Request Register
#define UPINRQ1                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x654)     // Pipe 1 IN Request Register
#define UPINRQ2                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x658)     // Pipe 2 IN Request Register
#define UPINRQ3                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x65c)     // Pipe 3 IN Request Register
#define UPINRQ4                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x650)     // Pipe 4 IN Request Register
#define UPINRQ5                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x654)     // Pipe 5 IN Request Register
#define UPINRQ6                          *(unsigned long*)(USBB_CONFIG_BLOCK + 0x658)     // Pipe 6 IN Request Register

#define UPERR0                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x680)     // Pipe 0 Error Register
#define UPERR1                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x684)     // Pipe 1 Error Register
#define UPERR2                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x688)     // Pipe 2 Error Register
#define UPERR3                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x68c)     // Pipe 3 Error Register
#define UPERR4                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x690)     // Pipe 4 Error Register
#define UPERR5                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x694)     // Pipe 5 Error Register
#define UPERR6                           *(unsigned long*)(USBB_CONFIG_BLOCK + 0x698)     // Pipe 6 Error Register

#define UHDMA1_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x710)     // Host DMA Channel 1 Next Descriptor Address Register
#define UHDMA1_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x714)     // Host DMA Channel 1 HSB Address Register
#define UHDMA1_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x718)     // Host DMA Channel 1 Control Register
#define UHDMA1_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x71c)     // Host DMA Channel 1 Status Register
#define UHDMA2_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x720)     // Host DMA Channel 2 Next Descriptor Address Register
#define UHDMA2_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x724)     // Host DMA Channel 2 HSB Address Register
#define UHDMA2_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x728)     // Host DMA Channel 2 Control Register
#define UHDMA2_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x72c)     // Host DMA Channel 2 Status Register
#define UHDMA3_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x730)     // Host DMA Channel 3 Next Descriptor Address Register
#define UHDMA3_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x734)     // Host DMA Channel 3 HSB Address Register
#define UHDMA3_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x738)     // Host DMA Channel 3 Control Register
#define UHDMA3_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x73c)     // Host DMA Channel 3 Status Register
#define UHDMA4_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x740)     // Host DMA Channel 4 Next Descriptor Address Register
#define UHDMA4_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x744)     // Host DMA Channel 4 HSB Address Register
#define UHDMA4_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x748)     // Host DMA Channel 4 Control Register
#define UHDMA4_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x74c)     // Host DMA Channel 4 Status Register
#define UHDMA5_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x750)     // Host DMA Channel 5 Next Descriptor Address Register
#define UHDMA5_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x754)     // Host DMA Channel 5 HSB Address Register
#define UHDMA5_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x758)     // Host DMA Channel 5 Control Register
#define UHDMA5_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x75c)     // Host DMA Channel 5 Status Register
#define UHDMA6_NEXTDESC                  *(unsigned long*)(USBB_CONFIG_BLOCK + 0x760)     // Host DMA Channel 6 Next Descriptor Address Register
#define UHDMA6_ADDR                      *(unsigned long*)(USBB_CONFIG_BLOCK + 0x764)     // Host DMA Channel 6 HSB Address Register
#define UHDMA6_CONTROL                   *(unsigned long*)(USBB_CONFIG_BLOCK + 0x768)     // Host DMA Channel 6 Control Register
#define UHDMA6_STATUS                    *(unsigned long*)(USBB_CONFIG_BLOCK + 0x76c)     // Host DMA Channel 6 Status Register

#define USBCON                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x800) // General Control Register
  #define USBCON_VBUSHWC                 0x00000100                                       // disable hw control over USB_VBOF output pin
  #define USBCON_OTGPADE                 0x00001000                                       // OTG pad enable
  #define USBCON_FRZCLK                  0x00004000                                       // freeze USB clock
  #define USBCON_USBEN                   0x00008000                                       // enable macro operation
  #define USBCON_UIDE                    0x01000000                                       // USB_ID pin enable
  #define USBCON_UIMOD                   0x02000000                                       // USB USB Macro Mode
#define USBSTA                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x804) // Global Status Register (read-only)
#define USBSTACLR                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x808) // Global Status Clear Register (write-only)
#define USBSTASET                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x80c) // Global Status Set Register (write-only)

#define UVERS                            *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x818) // IP Version Register (read-only)
#define UFEATURES                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x81c) // IP Features Register (read-only)
#define UADDRSIZE                        *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x820) // IP FB Address Size Register (read-only)
#define UNAME1                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x824) // IP Name Register 1 (read-only)
#define UNAME2                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x828) // IP Name Register 2 (read-only)
#define USBFSM                           *(volatile unsigned long*)(USBB_CONFIG_BLOCK + 0x82c) // USB Finite State Machine Status Register (read-only)


#ifdef _WINDOWS
    #define CLEAR_STATUS(endpoint, flags)         UESTA##endpoint## &= ~(flags)
    #define CLEAR_EPCONTROL(endpoint, flags)      UECON##endpoint## &= ~(flags)
    #define SET_EPCONTROL(endpoint, flags)        UECON##endpoint## |= (flags)
    #define ENABLE_USB_INT(flags)                 UDINTE |= (flags)
    #define DISABLE_USB_INT(flags)                UDINTE &= ~(flags)
#else
    #define CLEAR_STATUS(endpoint, flags)         UESTA##endpoint##CLR = (flags)
    #define CLEAR_EPCONTROL(endpoint, flags)      UECON##endpoint##CLR = (flags)
    #define SET_EPCONTROL(endpoint, flags)        UECON##endpoint##SET = (flags)
    #define ENABLE_USB_INT(flags)                 UDINTESET = (flags)
    #define DISABLE_USB_INT(flags)                UDINTECLR = (flags)
#endif


#define SIZE_OF_DPRAM                    960                             // 960 bytes of dual-port RAM used by the USB endpoints {11}

/****************************************************************************/

typedef struct stUSB_END_POINT
{
    unsigned short usEndpointSize;                                       // contains size of endpoint plus some control flags
    unsigned char  ucEndpointRxFlags;                                    // rx flags
    unsigned char  ucEndpointTxFlags;                                    // tx flags
    #ifdef _WINDOWS
    unsigned long ulSimFlags;
    #endif
} USB_END_POINT;

#define ENDPOINT_DUAL_BANK 0x01

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
} USB_HW;

#define USB_FIFO_BUFFER_DEPTH 2

#define FNSEND_USB_DATA(buffer_addr, length, Endpoint, hw)      fnSendUSB_FIFO(buffer_addr, length, Endpoint, hw); _SIM_USB(USB_SIM_TX, Endpoint, hw)

// Macros for setting and clearing configuration bits and then waiting until they have been confirmed, due to synchronisation requirement between MCK and UDPCK
//
#define SYNCHRONISE_CONTROL_SET(bits)    while ((*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) != (bits)) { *ptrUSB_HW->ptr_ulUSB_BDControl |= (bits);  }
#define SYNCHRONISE_CONTROL_CLEAR(bits)  while (*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) { *ptrUSB_HW->ptr_ulUSB_BDControl &= ~(bits); }

#define SYNCHRONISE_CONTROL_SET_(bits)   while ((*usb_hardware.ptr_ulUSB_BDControl & (bits)) != (bits)) { *usb_hardware.ptr_ulUSB_BDControl |= (bits); }
#define SYNCHRONISE_CONTROL_CLEAR_(bits) while (*usb_hardware.ptr_ulUSB_BDControl & (bits)) { *usb_hardware.ptr_ulUSB_BDControl &= ~(bits); }

#define VALIDATE_NEW_CONFIGURATION()

// Macro for sending a zero data token
// start the transmission and wait for the flag to be set, when simulating mark that the data buffer is empty
//
#ifdef _WINDOWS
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref) *(ptrUSB_HW->ptr_ulUSB_BDControl) &= ~TXINE; SET_EPCONTROL(0, TXINE); _SIM_USB(USB_SIM_TX, iEndpoint_ref, ptrUSB_HW) // clear the transmitted data IN flag to cause next transmission to take place
#else
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref) *(ptrUSB_HW->ptr_ulUSB_BDControl + (0x30/sizeof(unsigned long))) = TXINE; SET_EPCONTROL(0, TXINE) // clear the transmitted data IN flag to cause next transmission to take place
/*SYNCHRONISE_CONTROL_SET(TXPKTRDY) */
#endif

#define SET_CONTROL_DIRECTION(ptrUSB_HW, ucFirstByte)
/*
 if (ucFirstByte & STANDARD_DEVICE_TO_HOST) {SYNCHRONISE_CONTROL_SET(UDP_DIR);}
 */
#define CLEAR_SETUP(ptrUSB_HW)
/*           SYNCHRONISE_CONTROL_CLEAR(RXSETUP)*/

#ifdef _WINDOWS
    extern volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned char *ptrAdd, int iDirection);
#else
    #define fnGetUSB_FIFOAdd(x, y)         x
#endif

#define NUMBER_OF_USB_ENDPOINTS            7                             // AVR32 supports 7 endpoints
#define NUMBER_OF_BUFFERED_ENDPOINT        4                             // endpoints 0 and 3 are single buffered

//#define USB_FIFO                                                         // AVR32 has USB buffers realised as FIFOs
//#define FIFO_TO_QUEUE                                                    // activate queue driver support to allow FIFO copy to a circular buffer
#define ENDPOINT0_LENGTH                   64                            // endpoint 0 has a single-bank FIFO which can be used for control, bulk or interrupt types
#define ENDPOINT1_LENGTH                   64                            // endpoint 1 has a dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types
#define ENDPOINT2_LENGTH                   64                            // endpoint 2 has a dual-bank 64 byte FIFO wich can be used for bulk, interrupt or isochronous types
#define ENDPOINT3_LENGTH                   64                            // endpoint 3 has a single-bank 64 byte FIFO wich can be used for bulk, interrupt or isochronous types
#define ENDPOINT4_LENGTH                   64                            // endpoint 4 has a dual-bank 64 byte FIFO wich can be used for bulk, interrupt or isochronous types
#define ENDPOINT5_LENGTH                   256                           // endpoint 5 has a dual-bank 256 byte FIFO wich can be used for bulk, interrupt or isochronous types
#define ENDPOINT6_LENGTH                   256                           // endpoint 6 has a dual-bank 256 byte FIFO wich can be used for bulk, interrupt or isochronous types
#define MAX_FIFO_LENGTH                    ENDPOINT6_LENGTH              // longest endpoint FIFO

/****************************************************************************/

// HSB Bus Matrix
//
#define HSB_MCFG0                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x000)// Master Configuration Register 0
  #define INFINITE_LENGTH_BURST          0x00000000                                       // burst coming from this master cannot be broken
  #define SINGLE_ACCESS                  0x00000001
  #define FOUR_BEAT_BURST                0x00000002
  #define EIGHT_BEAT_BURST               0x00000003
  #define SIXTEEN_BEAT_BURST             0x00000004
#define HSB_MCFG1                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x004)// Master Configuration Register 1
#define HSB_MCFG2                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x008)// Master Configuration Register 2
#define HSB_MCFG3                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x00c)// Master Configuration Register 3
#define HSB_MCFG4                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x010)// Master Configuration Register 4
#define HSB_MCFG5                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x014)// Master Configuration Register 5
#define HSB_MCFG6                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x018)// Master Configuration Register 6
#define HSB_MCFG7                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x01c)// Master Configuration Register 7
#define HSB_MCFG8                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x020)// Master Configuration Register 8
#define HSB_MCFG9                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x024)// Master Configuration Register 9
#define HSB_MCFG10                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x028)// Master Configuration Register 10
#define HSB_MCFG11                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x02c)// Master Configuration Register 11
#define HSB_MCFG12                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x030)// Master Configuration Register 12
#define HSB_MCFG13                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x034)// Master Configuration Register 13
#define HSB_MCFG14                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x038)// Master Configuration Register 14
#define HSB_MCFG15                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x03c)// Master Configuration Register 15
  #define HSB_BUS_MASTER_CPU_DATA        0
  #define HSB_BUS_MASTER_CPU_INSTRUCTION 1
  #define HSB_BUS_MASTER_CPU_SAB         2
  #define HSB_BUS_MASTER_PDCA            3
  #define HSB_BUS_MASTER_MACB_DMA        4
  #define HSB_BUS_MASTER_USBB_DMA        5
#define HSB_SCFG0                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x040)// Slave Configuration Register 0
#define HSB_SCFG1                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x044)// Slave Configuration Register 1
#define HSB_SCFG2                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x048)// Slave Configuration Register 2
#define HSB_SCFG3                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x04c)// Slave Configuration Register 3
#define HSB_SCFG4                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x050)// Slave Configuration Register 4
#define HSB_SCFG5                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x054)// Slave Configuration Register 5
#define HSB_SCFG6                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x058)// Slave Configuration Register 6
#define HSB_SCFG7                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x05c)// Slave Configuration Register 7
#define HSB_SCFG8                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x060)// Slave Configuration Register 8
#define HSB_SCFG9                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x064)// Slave Configuration Register 9
#define HSB_SCFG10                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x068)// Slave Configuration Register 10
#define HSB_SCFG11                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x06c)// Slave Configuration Register 11
#define HSB_SCFG12                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x070)// Slave Configuration Register 12
#define HSB_SCFG13                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x074)// Slave Configuration Register 13
#define HSB_SCFG14                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x078)// Slave Configuration Register 14
#define HSB_SCFG15                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x07c)// Slave Configuration Register 15
  #define HSB_BUS_SLAVE_INTERNAL_FLASH   0
  #define HSB_BUS_SLAVE_HSB_PB_BRIDGE_0  1
  #define HSB_BUS_SLAVE_HSB_PB_BRIDGE_1  2
  #define HSB_BUS_SLAVE_INTERNAL_SRAM    3
  #define HSB_BUS_SLAVE_USBB_DPRAM       4
  #define HSB_BUS_SLAVE_EBI              5
#define HSB_PRAS0                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x080)// Priority Register A for Slave 0
#define HSB_PRBS0                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x084)// Priority Register B for Slave 0
#define HSB_PRAS1                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x088)// Priority Register A for Slave 1
#define HSB_PRBS1                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x08c)// Priority Register B for Slave 1
#define HSB_PRAS2                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x090)// Priority Register A for Slave 2
#define HSB_PRBS2                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x094)// Priority Register B for Slave 2
#define HSB_PRAS3                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x098)// Priority Register A for Slave 3
#define HSB_PRBS3                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x09c)// Priority Register B for Slave 3
#define HSB_PRAS4                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0a0)// Priority Register A for Slave 4
#define HSB_PRBS4                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0a4)// Priority Register B for Slave 4
#define HSB_PRAS5                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0a8)// Priority Register A for Slave 5
#define HSB_PRBS5                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0ac)// Priority Register B for Slave 5
#define HSB_PRAS6                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0b0)// Priority Register A for Slave 6
#define HSB_PRBS6                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0b4)// Priority Register B for Slave 6
#define HSB_PRAS7                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0b8)// Priority Register A for Slave 7
#define HSB_PRBS7                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0bc)// Priority Register B for Slave 7
#define HSB_PRAS8                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0c0)// Priority Register A for Slave 8
#define HSB_PRBS8                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0c4)// Priority Register B for Slave 8
#define HSB_PRAS9                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0c8)// Priority Register A for Slave 9
#define HSB_PRBS9                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0cc)// Priority Register B for Slave 9
#define HSB_PRAS10                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0d0)// Priority Register A for Slave 10
#define HSB_PRBS10                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0d4)// Priority Register B for Slave 10
#define HSB_PRAS11                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0d8)// Priority Register A for Slave 11
#define HSB_PRBS11                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0dc)// Priority Register B for Slave 11
#define HSB_PRAS12                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0e0)// Priority Register A for Slave 12
#define HSB_PRBS12                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0e4)// Priority Register B for Slave 12
#define HSB_PRAS13                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0e8)// Priority Register A for Slave 13
#define HSB_PRBS13                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0ec)// Priority Register B for Slave 13
#define HSB_PRAS14                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0f0)// Priority Register A for Slave 14
#define HSB_PRBS14                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0f4)// Priority Register B for Slave 14
#define HSB_PRAS15                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0f8)// Priority Register A for Slave 15
#define HSB_PRBS15                       *(volatile unsigned long*)(HMATRIX_BLOCK + 0x0fc)// Priority Register B for Slave 15
#define HSB_MRCR                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x100)// Master Remap Control Register

#define HSB_SFR0                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x110)// Special Function Register 0
#define HSB_SFR1                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x114)// Special Function Register 1
#define HSB_SFR2                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x118)// Special Function Register 2
#define HSB_SFR3                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x11c)// Special Function Register 3
#define HSB_SFR4                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x120)// Special Function Register 4
#define HSB_SFR5                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x124)// Special Function Register 5
  #define SDRAM_CHIP_SELECT              0x00000002
#define HSB_SFR6                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x128)// Special Function Register 6
#define HSB_SFR7                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x12c)// Special Function Register 7
#define HSB_SFR8                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x130)// Special Function Register 8
#define HSB_SFR9                         *(volatile unsigned long*)(HMATRIX_BLOCK + 0x134)// Special Function Register 9
#define HSB_SFR10                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x138)// Special Function Register 10
#define HSB_SFR11                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x13c)// Special Function Register 11
#define HSB_SFR12                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x140)// Special Function Register 12
#define HSB_SFR13                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x144)// Special Function Register 13
#define HSB_SFR14                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x148)// Special Function Register 14
#define HSB_SFR15                        *(volatile unsigned long*)(HMATRIX_BLOCK + 0x14c)// Special Function Register 15
// The Bus matrix registers need to be read with a dummy load operation after they have been written, which is performed by this macro
//
#define HSB_READBACK(reg, value)         HSB_##reg = value; HSB_##reg


// Flash Controller
//
#define FC_FCR                           *(volatile unsigned long*)(FLASHC_BLOCK + 0x00)  // FLASH Control Register
  #define FRDY                           0x00000001                                       // flash ready generates an interrupt
  #define LOCKE                          0x00000004                                       // lock error interrupt enable
  #define PROGE                          0x00000008                                       // programming error generates an interrupt
  #define FWS_ONE_WAIT_STATE             0x00000040                                       // flash wait state
  #define SASD                           0x00000100                                       // sense amplifier sample disable
#define FC_CMD                           *(volatile unsigned long*)(FLASHC_BLOCK + 0x04)  // FLASH Command Register
  #define FLASH_KEY                      0xa5000000
  #define FCMD_NOP                       0x00000000                                       // FLASH command : no command
  #define FCMD_WP                        0x00000001                                       // FLASH command : write page
  #define FCMD_EP                        0x00000002                                       // FLASH command : erase page
  #define FCMD_CPB                       0x00000003                                       // FLASH command : clear page buffer
  #define FCMD_LP                        0x00000004                                       // FLASH command : lock region containing given page
  #define FCMD_UP                        0x00000005                                       // FLASH command : unlock region containing given page
  #define FCMD_EA                        0x00000006                                       // FLASH command : erase all
  #define FCMD_WGPB                      0x00000007                                       // FLASH command : write general-purpose fuse bit
  #define FCMD_EGPB                      0x00000008                                       // FLASH command : erase general-purpose fuse bit
  #define FCMD_SSB                       0x00000009                                       // FLASH command : set security bit
  #define FCMD_PGPFB                     0x0000000a                                       // FLASH command : program general-purpose fuse byte
  #define FCMD_EAGPF                     0x0000000b                                       // FLASH command : erase all general-purpose fuses
  #define FCMD_QPR                       0x0000000c                                       // FLASH command : quick page read
  #define FCMD_WUP                       0x0000000d                                       // FLASH command : write user page
  #define FCMD_EUP                       0x0000000e                                       // FLASH command : erase user page
  #define FCMD_QPRUP                     0x0000000f                                       // FLASH command : quick page read user page
#ifdef _AT32UC3C 
  #define FCMD_HSEN                      0x00000010                                       // FLASH command : high speed mode enable
  #define FCMD_HSDIS                     0x00000011                                       // FLASH command : high speed mode disable
#endif
#define FC_FSR                           *(volatile unsigned long*)(FLASHC_BLOCK + 0x08)  // FLASH Status Register
  #define FLASH_READY                    0x00000001                                       // FLASH ready for new command
  #define LOCKE                          0x00000004                                       // FLASH Lock error
  #define PROGE                          0x00000008                                       // FLASH Programming error status
  #define FLASH_SECURITY                 0x00000010                                       // FLASH security active
  #define FLASH_SIZE_32K                 0x00000000
  #define FLASH_SIZE_64K                 0x00002000
  #define FLASH_SIZE_128K                0x00004000
  #define FLASH_SIZE_256K                0x00006000
  #define FLASH_SIZE_384K                0x00008000
  #define FLASH_SIZE_512K                0x00009000
  #define FLASH_SIZE_768K                0x0000c000
  #define FLASH_SIZE_1024K               0x0000e000
  #define FLASH_LOCK0                    0x00010000                                       // lock region locks set
  #define FLASH_LOCK1                    0x00020000
  #define FLASH_LOCK2                    0x00040000
  #define FLASH_LOCK3                    0x00080000
  #define FLASH_LOCK4                    0x00100000
  #define FLASH_LOCK5                    0x00200000
  #define FLASH_LOCK6                    0x00400000
  #define FLASH_LOCK7                    0x00800000
  #define FLASH_LOCK8                    0x01000000
  #define FLASH_LOCK9                    0x02000000
  #define FLASH_LOCK10                   0x04000000
  #define FLASH_LOCK11                   0x08000000
  #define FLASH_LOCK12                   0x10000000
  #define FLASH_LOCK13                   0x20000000
  #define FLASH_LOCK14                   0x40000000
  #define FLASH_LOCK15                   0x80000000
#ifdef _AT32UC3C                                                       // {10}
    #define FC_PR                        *(volatile unsigned long*)(FLASHC_BLOCK + 0x0c)  // FLASH parameter register (read-only)
    #define FC_VR                        *(volatile unsigned long*)(FLASHC_BLOCK + 0x10)  // FLASH version register (read-only)
    #define FC_FGPFRHI                   *(volatile unsigned long*)(FLASHC_BLOCK + 0x14)  // FLASH general purpose fuse register high (read-only)
    #define FC_FGPFRLO                   *(volatile unsigned long*)(FLASHC_BLOCK + 0x18)  // FLASH general purpose fuse register low (read-only)
#else
    #define FC_FGPFRHI                   *(volatile unsigned long*)(FLASHC_BLOCK + 0x0c)  // FLASH general purpose fuse register high (read-only)
    #define FC_FGPFRLO                   *(volatile unsigned long*)(FLASHC_BLOCK + 0x10)  // FLASH general purpose fuse register low (read-only)
#endif
#define FLASH_GRANULARITY                512                                              // smallest sector which can be erased independently
#ifndef FLASH_LINE_SIZE
    #define FLASH_LINE_SIZE              16                                               // lines made up of 16 words (theoretically 4 are necessary but it is more practical to have the size larger than a file header)
#endif
#ifndef FLASH_ROW_SIZE
    #define FLASH_ROW_SIZE               FLASH_GRANULARITY                                // FLASH writes are performed on this many bytes at a time (this is not FLASH structure dependent but rather logical when working with the long word write restriction - a buffer of the same size as the FLASH granularity results in fastest operation due to less FLASH writes)
#endif
#define MAX_SECTOR_PARS                  ((FLASH_GRANULARITY - (2*FLASH_LINE_SIZE))/FLASH_LINE_SIZE) // the number of user bytes fitting into first parameter block
#define LOCK_REGION_SIZE                 (16*1024)


// MACB
//
#define MACB_NCR                         *(unsigned long*)(MACB_BLOCK + 0x0)               // Network Control Register (read/write)
  #define MPE                            0x00000010                                        // Enable management port
  #define MACB_TE                        0x00000008                                        // Enable transmitter
  #define MACB_RE                        0x00000004                                        // Enable receiver
  #define LLB                            0x00000002                                        // MII loopback
  #define CLRSTAT                        0x00000020                                        // Reset stats registers
  #define TSTART                         0x00000200                                        // Start transmission registers
#define MACB_NCFGR                       *(unsigned long*)(MACB_BLOCK + 0x4)               // Network Configuration Register (read/write)
  #define SPD                            0x00000001                                        // 100M speed
  #define FD                             0x00000002                                        // full duplex (ignore collision)
  #define JFRAME                         0x00000008                                        // {13} Enable jumbo frames up to 10240 bytes in length
  #define CAF                            0x00000010                                        // Copy all received frames (promiscuous)
  #define NBI                            0x00000020                                        // {13} No broadcast
  #define MTI                            0x00000040                                        // {13} Multicast Hash enable
  #define UNI                            0x00000080                                        // {13} Unicast Hash enable
  #define BIG                            0x00000100                                        // {13} enable big frames to 1536, rather than 1518
  #define MCLK_32                        0x00000800
  #define PAE                            0x00002000                                        // enable pause frames
  #define RLCE                           0x00010000                                        // receive Length Field Checking Enable
  #define DRFCS                          0x00020000                                        // don't copy FCS to receive buffer
  #define EFRHD                          0x00040000                                        // {13} enable frames to be received in half-duple mode while transmitting
  #define IRXFCS                         0x00080000                                        // {13} don't reject frames with FCS/CRC error
#define MACB_NSR                         *(volatile unsigned long*)(MACB_BLOCK + 0x8)      // Network Status Register (read only)
  #define MDIO                           0x00000002
  #define PHY_LOGIC_RUNNING              0x00000004
#define MACB_RES1                        *(unsigned long*)(MACB_BLOCK + 0xc)               // Reserved
#define MACB_RES2                        *(unsigned long*)(MACB_BLOCK + 0x10)              // Reserved
#define MACB_TSR                         *(unsigned long*)(MACB_BLOCK + 0x14)              // Transmit Status Register (read/write)
  #define TSR_UBR                        0x00000001                                        // Used Bit Read
  #define TSR_COL                        0x00000002                                        // Collision
  #define TSR_RLE                        0x00000004                                        // Retry Limit Exceeded
  #define TSR_TGO                        0x00000008                                        // Transmission presently active
  #define TSR_BEX                        0x00000010                                        // Buffers exhausted in mid-frame
  #define TSR_COMP                       0x00000020                                        // Transmission completed
  #define TSR_UND                        0x00000040                                        // Underrun
#define MACB_RBQP                        *(unsigned long*)(MACB_BLOCK + 0x18)              // Receive Buffer Queue Pointer (read/write)
#define MACB_TBQP                        *(unsigned long*)(MACB_BLOCK + 0x1c)              // Transmit Buffer Queue Pointer (read/write)
#define MACB_RSR                         *(unsigned long*)(MACB_BLOCK + 0x20)              // Receive Status Register (read/write)
  #define RSR_BNA                        0x00000001                                        // Receive buffer not available 
  #define RSR_REC                        0x00000002                                        // Frame received 
  #define RSR_OVR                        0x00000004                                        // Receive Overrun 
#define MACB_ISR                         *(volatile unsigned long*)(MACB_BLOCK + 0x24)     // Interrupt Status Register (read/write - various bits cleared on read)
#define MACB_IER                         *(volatile unsigned long*)(MACB_BLOCK + 0x28)     // Interrupt Enable Register (write only)
#define MACB_IDR                         *(volatile unsigned long*)(MACB_BLOCK + 0x2c)     // Interrupt Disable Register (write only)
#define MACB_IMR                         *(volatile unsigned long*)(MACB_BLOCK + 0x30)     // Interrupt Mask Register (read only)
  #define MFD                            0x00000001                                        // Management frame sent
  #define RCOMP                          0x00000002                                        // Receive complete
  #define RXUBR                          0x00000004                                        // Receive used bit read
  #define TXUBR                          0x00000008                                        // Transmit used bit read
  #define TUND                           0x00000010                                        // Transmit Underrun
  #define RLE                            0x00000020                                        // Retry limit exceeded
  #define TXERR                          0x00000040                                        // Transmit buffer exhaused in mid frame
  #define TCOMP                          0x00000080                                        // Transmit complete
  #define ROVR                           0x00000400                                        // Receive overrun
  #define HRESP                          0x00000800                                        // Hresp not OK
  #define PFR                            0x00001000                                        // Pause frame received
  #define PTZ                            0x00002000                                        // Pause time zero
#define MACB_MAN                         *(volatile unsigned long*)(MACB_BLOCK + 0x34)     // PHY Maintenance Register (read/write)
  #define SOF                            0x40000000
  #define PHY_CODE                       0x00020000
  #define PHY_READ                       0x20000000
  #define PHY_WRITE                      0x10000000
  #define READ_FROM_PHY (SOF | PHY_READ  | (PHY_ADDRESS_ << 23) | PHY_CODE)
  #define WRITE_TO_PHY  (SOF | PHY_WRITE | (PHY_ADDRESS_ << 23) | PHY_CODE)
#define MACB_PTR                         *(unsigned long*)(MACB_BLOCK + 0x38)              // Pause Time Register (read/write)
#define MACB_PFR                         *(unsigned long*)(MACB_BLOCK + 0x3c)              // Pause Frames received Register (read/write)
#define MACB_FTO                         *(unsigned long*)(MACB_BLOCK + 0x40)              // Frames Transmitted Ok (read/write)
#define MACB_SCF                         *(unsigned long*)(MACB_BLOCK + 0x44)              // Singe Collision Frames Register (read/write)
#define MACB_MCF                         *(unsigned long*)(MACB_BLOCK + 0x48)              // Multiple Collision Frames Register (read/write)
#define MACB_FRO                         *(unsigned long*)(MACB_BLOCK + 0x4c)              // Frames Received OK Register (read/write)
#define MACB_FCSE                        *(unsigned long*)(MACB_BLOCK + 0x50)              // Frame Check Sequence Errors Register (read/write)
#define MACB_ALE                         *(unsigned long*)(MACB_BLOCK + 0x54)              // Alignment Errors Register (read/write)
#define MACB_DTF                         *(unsigned long*)(MACB_BLOCK + 0x58)              // Deferred Transmission Frames Register (read/write)
#define MACB_LCOL                        *(unsigned long*)(MACB_BLOCK + 0x5c)              // Late Collisions Register (read/write)
#define MACB_EXCOL                       *(unsigned long*)(MACB_BLOCK + 0x60)              // Excessive Collisions Register (read/write)
#define MACB_TUND                        *(unsigned long*)(MACB_BLOCK + 0x64)              // Transmit Underrun Errors Register (read/write)
#define MACB_CSE                         *(unsigned long*)(MACB_BLOCK + 0x68)              // Carrier Sense Errors Register (read/write)
#define MACB_RRE                         *(unsigned long*)(MACB_BLOCK + 0x6c)              // Receive Resource Errors Register (read/write)
#define MACB_ROV                         *(unsigned long*)(MACB_BLOCK + 0x70)              // Receiver Overrun Errors Register (read/write)
#define MACB_RSE                         *(unsigned long*)(MACB_BLOCK + 0x74)              // Receive Symbols Errors Register (read/write)
#define MACB_ELE                         *(unsigned long*)(MACB_BLOCK + 0x78)              // Excessive Length Errors Register (read/write)
#define MACB_RJA                         *(unsigned long*)(MACB_BLOCK + 0x7c)              // Receive Jabbers Register (read/write)
#define MACB_USF                         *(unsigned long*)(MACB_BLOCK + 0x80)              // Undersize Frames Register (read/write)
#define MACB_STE                         *(unsigned long*)(MACB_BLOCK + 0x84)              // SQE Test Errors Register (read/write)
#define MACB_RLE                         *(unsigned long*)(MACB_BLOCK + 0x88)              // Received Length Field Mismatch Register (read/write)
#define MACB_TPF                         *(unsigned long*)(MACB_BLOCK + 0x8c)              // Transmitted Pause Frames Register (read/write)
#define MACB_HRB                         *(unsigned long*)(MACB_BLOCK + 0x90)              // Hash Bottom [31:0] Register (read/write)
#define MACB_HRT                         *(unsigned long*)(MACB_BLOCK + 0x94)              // Hash Top [63:32] Register (read/write)
#define MACB_SA1B                        *(unsigned long*)(MACB_BLOCK + 0x98)              // Specific Address 1 Bottom Register (read/write)
#define MACB_SA1T                        *(unsigned long*)(MACB_BLOCK + 0x9c)              // Specific Address 1 Top Register (read/write)
#define MACB_SA2B                        *(unsigned long*)(MACB_BLOCK + 0xa0)              // Specific Address 2 Bottom Register (read/write)
#define MACB_SA2T                        *(unsigned long*)(MACB_BLOCK + 0xa4)              // Specific Address 2 Top Register (read/write)
#define MACB_SA3B                        *(unsigned long*)(MACB_BLOCK + 0xa8)              // Specific Address 3 Bottom Register (read/write)
#define MACB_SA3T                        *(unsigned long*)(MACB_BLOCK + 0xac)              // Specific Address 3 Top Register (read/write)
#define MACB_SA4B                        *(unsigned long*)(MACB_BLOCK + 0xb0)              // Specific Address 4 Bottom Register (read/write)
#define MACB_SA4T                        *(unsigned long*)(MACB_BLOCK + 0xb4)              // Specific Address 4 Top Register (read/write)
#define MACB_TID                         *(unsigned long*)(MACB_BLOCK + 0xb8)              // Type ID Checking Register (read/write)
#define MACB_TPQ                         *(unsigned long*)(MACB_BLOCK + 0xbc)              // Transmit Pasue Quentum Register (read/write)
#define MACB_USRIO                       *(unsigned long*)(MACB_BLOCK + 0xc0)              // {14} User Input/Output Register (read/write)
  #define MACB_USRIO_MII                 0x00000001                                        // select MII mode rather than RMII mode
  #define MACB_USRIO_EAM                 0x00000002                                        // cause a frame copy to memory (when EAE in NCFGR is set)
  #define MACB_USRIO_TX_PAUSE            0x00000004                                        // toggling this bit causes a pause frame to be transmitted
  #define MACB_USRIO_TX_PAUSE_ZERO       0x00000008                                        // selects zero rather than the transmit quantum register as the transmitted pause frame quantum
#define MACB_WOL                         *(unsigned long*)(MACB_BLOCK + 0xc4)              // Wake on LAN Register (read/write)

// SMC - Static Memory Controller
//
#define SMC_SETUP_0                      *(unsigned long*)(SMC_BLOCK + 0x00)               // SMC Setup Register Channel 0
  #define NWE_SETUP_MASK                 0x0000003f
  #define NWE_SETUP_SHIFT                0
  #define NCS_WR_SETUP_MASK              0x00003f00
  #define NCS_WR_SETUP_SHIFT             8
  #define NRD_SETUP_MASK                 0x003f0000
  #define NRD_SETUP_SHIFT                16
  #define NCS_RD_SETUP_MASK              0x3f000000
  #define NCS_RD_SETUP_SHIFT             24
#define SMC_PULSE_0                      *(unsigned long*)(SMC_BLOCK + 0x04)               // SMC Pulse Register Channel 0
  #define NWE_PULSE_MASK                 0x0000007f
  #define NWE_PULSE_SHIFT                0
  #define NCS_WR_PULSE_MASK              0x00007f00
  #define NCS_WR_PULSE_SHIFT             8
  #define NRD_PULSE_MASK                 0x007f0000
  #define NRD_PULSE_SHIFT                16
  #define NCS_RD_PULSE_MASK              0x7f000000
  #define NCS_RD_PULSE_SHIFT             24
#define SMC_CYCLE_0                      *(unsigned long*)(SMC_BLOCK + 0x08)               // SMC Pulse Cycle Channel 0
  #define NWE_CYCLE_MASK                 0x000001ff
  #define NWE_CYCLE_SHIFT                0
  #define NCS_RD_CYCLE_MASK              0x01ff0000
  #define NCS_RD_CYCLE_SHIFT             16
#define SMC_MODE_0                       *(unsigned long*)(SMC_BLOCK + 0x0c)               // SMC Mode Register Channel 0
  #define READ_MODE_NCS                  0x00000000
  #define READ_MODE_NRD                  0x00000001
  #define WRITE_MODE_NCS                 0x00000000
  #define WRITE_MODE_NWE                 0x00000002
  #define NWAIT_MODE_DISABLED            0x00000000
  #define NWAIT_MODE_FROZEN              0x00000020
  #define NWAIT_MODE_READY               0x00000030
  #define BYTE_WRITE_ACCESS_TYPE         0x00000100
  #define DBW_8_BIT_BUS                  0x00000000
  #define DBW_16_BIT_BUS                 0x00001000
  #define DBW_32_BIT_BUS                 0x00002000
  #define TDF_CYCLES_MASK                0x000f0000
  #define TDF_CYCLES_SHIFT               16
  #define TDF_MODE_OPTIMISATION          0x00100000
  #define PAGE_MODE_ENABLED              0x01000000
  #define PAGE_SIZE_4_BYTES              0x00000000
  #define PAGE_SIZE_8_BYTES              0x10000000
  #define PAGE_SIZE_16_BYTES             0x20000000
  #define PAGE_SIZE_32_BYTES             0x30000000
#define SMC_SETUP_1                      *(unsigned long*)(SMC_BLOCK + 0x10)               // SMC Setup Register Channel 1
#define SMC_PULSE_1                      *(unsigned long*)(SMC_BLOCK + 0x14)               // SMC Pulse Register Channel 1
#define SMC_CYCLE_1                      *(unsigned long*)(SMC_BLOCK + 0x18)               // SMC Pulse Cycle Channel 1
#define SMC_MODE_1                       *(unsigned long*)(SMC_BLOCK + 0x1c)               // SMC Mode Register Channel 1
#define SMC_SETUP_2                      *(unsigned long*)(SMC_BLOCK + 0x20)               // SMC Setup Register Channel 2
#define SMC_PULSE_2                      *(unsigned long*)(SMC_BLOCK + 0x24)               // SMC Pulse Register Channel 2
#define SMC_CYCLE_2                      *(unsigned long*)(SMC_BLOCK + 0x28)               // SMC Pulse Cycle Channel 2
#define SMC_MODE_2                       *(unsigned long*)(SMC_BLOCK + 0x2c)               // SMC Mode Register Channel 2
#define SMC_SETUP_3                      *(unsigned long*)(SMC_BLOCK + 0x30)               // SMC Setup Register Channel 3
#define SMC_PULSE_3                      *(unsigned long*)(SMC_BLOCK + 0x34)               // SMC Pulse Register Channel 3
#define SMC_CYCLE_3                      *(unsigned long*)(SMC_BLOCK + 0x38)               // SMC Pulse Cycle Channel 3
#define SMC_MODE_3                       *(unsigned long*)(SMC_BLOCK + 0x3c)               // SMC Mode Register Channel 3


// Peripheral DMA
//
#define DMA_MAR_0                        *(unsigned long*)(PDCA_BLOCK + 0x000)             // Memory Address Register DMA Channel 0
#define DMA_PSR_0                        *(unsigned long*)(PDCA_BLOCK + 0x004)             // Peripheral Select Register DMA Channel 0
  #define PDC_HANDSHAKE_ADC              0
  #define PDC_HANDSHAKE_SCC_RX           1
  #define PDC_HANDSHAKE_USART0_RX        2
  #define PDC_HANDSHAKE_USART1_RX        3
  #define PDC_HANDSHAKE_USART2_RX        4
  #define PDC_HANDSHAKE_USART3_RX        5
  #define PDC_HANDSHAKE_TWI_RX           6
  #define PDC_HANDSHAKE_SPI0_RX          7
  #define PDC_HANDSHAKE_SPI1_RX          8
  #define PDC_HANDSHAKE_SCC_TX           9
  #define PDC_HANDSHAKE_USART0_TX        10
  #define PDC_HANDSHAKE_USART1_TX        11
  #define PDC_HANDSHAKE_USART2_TX        12
  #define PDC_HANDSHAKE_USART3_TX        13
  #define PDC_HANDSHAKE_TWI_TX           14
  #define PDC_HANDSHAKE_SPI0_TX          15
  #define PDC_HANDSHAKE_SPI1_TX          16
  #define PDC_HANDSHAKE_ABDAC            17
#define DMA_TCR_0                        *(volatile unsigned long*)(PDCA_BLOCK + 0x008)    // Transmit Count Register DMA Channel 0
#define DMA_MARR_0                       *(unsigned long*)(PDCA_BLOCK + 0x00c)             // Memory Address Reload Register DMA Channel 0
#define DMA_TCRR_0                       *(volatile unsigned long*)(PDCA_BLOCK + 0x010)    // Transmit Count Reload Register DMA Channel 0
#define DMA_CR_0                         *(volatile unsigned long*)(PDCA_BLOCK + 0x014)    // Control Register DMA Channel 0 (write-only)
  #define DMA_TEN                        0x00000001                                        // transfer enable
  #define DMA_TDIS                       0x00000002                                        // transfer disable
  #define DMA_ECLR                       0x00000100                                        // clear transfer error flag in status register
#define DMA_MR_0                         *(volatile unsigned long*)(PDCA_BLOCK + 0x018)    // Mode Register DMA Channel 0
  #define DMA_BYTE_TRANSFER              0x00000000
  #define DMA_HALF_WORD_TRANSFER         0x00000001
  #define DMA_WORD_TRANSFER              0x00000002
#define DMA_SR_0                         *(volatile unsigned long*)(PDCA_BLOCK + 0x01c)    // Status Register DMA Channel 0 (read-only)
#define DMA_IER_0                        *(volatile unsigned long*)(PDCA_BLOCK + 0x020)    // Interrupt Enable Register DMA Channel 0 (write-only)
  #define DMA_RCZ                        0x00000001                                        // interrupt on reload counter zero
  #define DMA_TRC                        0x00000002                                        // interrupt on transfer complete
  #define DMA_TERR                       0x00000004                                        // interrupt on transfer error
#define DMA_IDR_0                        *(volatile unsigned long*)(PDCA_BLOCK + 0x024)    // Interrupt Disable Register DMA Channel 0 (write-only)
#define DMA_IMR_0                        *(volatile unsigned long*)(PDCA_BLOCK + 0x028)    // Interrupt Mask Register DMA Channel 0 (read-only)
#define DMA_ISR_0                        *(volatile unsigned long*)(PDCA_BLOCK + 0x02c)    // Interrupt Status Register DMA Channel 0 (read-only)

#define DMA_MAR_1                        *(unsigned long*)(PDCA_BLOCK + 0x040)             // Memory Address Register DMA Channel 1
#define DMA_PSR_1                        *(unsigned long*)(PDCA_BLOCK + 0x044)             // Peripheral Select Register DMA Channel 1
#define DMA_TCR_1                        *(volatile unsigned long*)(PDCA_BLOCK + 0x048)    // Transmit Count Register DMA Channel 1
#define DMA_MARR_1                       *(unsigned long*)(PDCA_BLOCK + 0x04c)             // Memory Address Reload Register DMA Channel 1
#define DMA_TCRR_1                       *(volatile unsigned long*)(PDCA_BLOCK + 0x050)    // Transmit Count Reload Register DMA Channel 1
#define DMA_CR_1                         *(volatile unsigned long*)(PDCA_BLOCK + 0x054)    // Control Register DMA Channel 1 (write-only)
#define DMA_MR_1                         *(volatile unsigned long*)(PDCA_BLOCK + 0x058)    // Mode Register DMA Channel 1
#define DMA_SR_1                         *(volatile unsigned long*)(PDCA_BLOCK + 0x05c)    // Status Register DMA Channel 1 (read-only)
#define DMA_IER_1                        *(volatile unsigned long*)(PDCA_BLOCK + 0x060)    // Interrupt Enable Register DMA Channel 1 (write-only)
#define DMA_IDR_1                        *(volatile unsigned long*)(PDCA_BLOCK + 0x064)    // Interrupt Disable Register DMA Channel 1 (write-only)
#define DMA_IMR_1                        *(volatile unsigned long*)(PDCA_BLOCK + 0x068)    // Interrupt Mask Register DMA Channel 1 (read-only)
#define DMA_ISR_1                        *(volatile unsigned long*)(PDCA_BLOCK + 0x06c)    // Interrupt Status Register DMA Channel 1 (read-only)

#define DMA_MAR_2                        *(unsigned long*)(PDCA_BLOCK + 0x080)             // Memory Address Register DMA Channel 2
#define DMA_PSR_2                        *(unsigned long*)(PDCA_BLOCK + 0x084)             // Peripheral Select Register DMA Channel 2
#define DMA_TCR_2                        *(volatile unsigned long*)(PDCA_BLOCK + 0x088)    // Transmit Count Register DMA Channel 2
#define DMA_MARR_2                       *(unsigned long*)(PDCA_BLOCK + 0x08c)             // Memory Address Reload Register DMA Channel 2
#define DMA_TCRR_2                       *(volatile unsigned long*)(PDCA_BLOCK + 0x090)    // Transmit Count Reload Register DMA Channel 2
#define DMA_CR_2                         *(volatile unsigned long*)(PDCA_BLOCK + 0x094)    // Control Register DMA Channel 2 (write-only)
#define DMA_MR_2                         *(volatile unsigned long*)(PDCA_BLOCK + 0x098)    // Mode Register DMA Channel 2
#define DMA_SR_2                         *(volatile unsigned long*)(PDCA_BLOCK + 0x09c)    // Status Register DMA Channel 2 (read-only)
#define DMA_IER_2                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0a0)    // Interrupt Enable Register DMA Channel 2 (write-only)
#define DMA_IDR_2                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0a4)    // Interrupt Disable Register DMA Channel 2 (write-only)
#define DMA_IMR_2                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0a8)    // Interrupt Mask Register DMA Channel 2 (read-only)
#define DMA_ISR_2                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0ac)    // Interrupt Status Register DMA Channel 2 (read-only)

#define DMA_MAR_3                        *(unsigned long*)(PDCA_BLOCK + 0x0c0)             // Memory Address Register DMA Channel 3
#define DMA_PSR_3                        *(unsigned long*)(PDCA_BLOCK + 0x0c4)             // Peripheral Select Register DMA Channel 3
#define DMA_TCR_3                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0c8)    // Transmit Count Register DMA Channel 3
#define DMA_MARR_3                       *(unsigned long*)(PDCA_BLOCK + 0x0cc)             // Memory Address Reload Register DMA Channel 3
#define DMA_TCRR_3                       *(volatile unsigned long*)(PDCA_BLOCK + 0x0d0)    // Transmit Count Reload Register DMA Channel 3
#define DMA_CR_3                         *(volatile unsigned long*)(PDCA_BLOCK + 0x0d4)    // Control Register DMA Channel 3 (write-only)
#define DMA_MR_3                         *(volatile unsigned long*)(PDCA_BLOCK + 0x0d8)    // Mode Register DMA Channel 3
#define DMA_SR_3                         *(volatile unsigned long*)(PDCA_BLOCK + 0x0dc)    // Status Register DMA Channel 3 (read-only)
#define DMA_IER_3                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0e0)    // Interrupt Enable Register DMA Channel 3 (write-only)
#define DMA_IDR_3                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0e4)    // Interrupt Disable Register DMA Channel 3 (write-only)
#define DMA_IMR_3                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0e8)    // Interrupt Mask Register DMA Channel 3 (read-only)
#define DMA_ISR_3                        *(volatile unsigned long*)(PDCA_BLOCK + 0x0ec)    // Interrupt Status Register DMA Channel 3 (read-only)

#define DMA_MAR_4                        *(unsigned long*)(PDCA_BLOCK + 0x100)             // Memory Address Register DMA Channel 4
#define DMA_PSR_4                        *(unsigned long*)(PDCA_BLOCK + 0x104)             // Peripheral Select Register DMA Channel 4
#define DMA_TCR_4                        *(volatile unsigned long*)(PDCA_BLOCK + 0x108)    // Transmit Count Register DMA Channel 4
#define DMA_MARR_4                       *(unsigned long*)(PDCA_BLOCK + 0x10c)             // Memory Address Reload Register DMA Channel 4
#define DMA_TCRR_4                       *(volatile unsigned long*)(PDCA_BLOCK + 0x110)    // Transmit Count Reload Register DMA Channel 4
#define DMA_CR_4                         *(volatile unsigned long*)(PDCA_BLOCK + 0x114)    // Control Register DMA Channel 4 (write-only)
#define DMA_MR_4                         *(volatile unsigned long*)(PDCA_BLOCK + 0x118)    // Mode Register DMA Channel 4
#define DMA_SR_4                         *(volatile unsigned long*)(PDCA_BLOCK + 0x11c)    // Status Register DMA Channel 4 (read-only)
#define DMA_IER_4                        *(volatile unsigned long*)(PDCA_BLOCK + 0x120)    // Interrupt Enable Register DMA Channel 4 (write-only)
#define DMA_IDR_4                        *(volatile unsigned long*)(PDCA_BLOCK + 0x124)    // Interrupt Disable Register DMA Channel 4 (write-only)
#define DMA_IMR_4                        *(volatile unsigned long*)(PDCA_BLOCK + 0x128)    // Interrupt Mask Register DMA Channel 4 (read-only)
#define DMA_ISR_4                        *(volatile unsigned long*)(PDCA_BLOCK + 0x12c)    // Interrupt Status Register DMA Channel 4 (read-only)

#define DMA_MAR_5                        *(unsigned long*)(PDCA_BLOCK + 0x140)             // Memory Address Register DMA Channel 5
#define DMA_PSR_5                        *(unsigned long*)(PDCA_BLOCK + 0x144)             // Peripheral Select Register DMA Channel 5
#define DMA_TCR_5                        *(volatile unsigned long*)(PDCA_BLOCK + 0x148)    // Transmit Count Register DMA Channel 5
#define DMA_MARR_5                       *(unsigned long*)(PDCA_BLOCK + 0x14c)             // Memory Address Reload Register DMA Channel 5
#define DMA_TCRR_5                       *(volatile unsigned long*)(PDCA_BLOCK + 0x150)    // Transmit Count Reload Register DMA Channel 5
#define DMA_CR_5                         *(volatile unsigned long*)(PDCA_BLOCK + 0x154)    // Control Register DMA Channel 5 (write-only)
#define DMA_MR_5                         *(volatile unsigned long*)(PDCA_BLOCK + 0x158)    // Mode Register DMA Channel 5
#define DMA_SR_5                         *(volatile unsigned long*)(PDCA_BLOCK + 0x15c)    // Status Register DMA Channel 5 (read-only)
#define DMA_IER_5                        *(volatile unsigned long*)(PDCA_BLOCK + 0x160)    // Interrupt Enable Register DMA Channel 5 (write-only)
#define DMA_IDR_5                        *(volatile unsigned long*)(PDCA_BLOCK + 0x164)    // Interrupt Disable Register DMA Channel 5 (write-only)
#define DMA_IMR_5                        *(volatile unsigned long*)(PDCA_BLOCK + 0x168)    // Interrupt Mask Register DMA Channel 5 (read-only)
#define DMA_ISR_5                        *(volatile unsigned long*)(PDCA_BLOCK + 0x16c)    // Interrupt Status Register DMA Channel 5 (read-only)

#define DMA_MAR_6                        *(unsigned long*)(PDCA_BLOCK + 0x180)             // Memory Address Register DMA Channel 6
#define DMA_PSR_6                        *(unsigned long*)(PDCA_BLOCK + 0x184)             // Peripheral Select Register DMA Channel 6
#define DMA_TCR_6                        *(volatile unsigned long*)(PDCA_BLOCK + 0x188)    // Transmit Count Register DMA Channel 6
#define DMA_MARR_6                       *(unsigned long*)(PDCA_BLOCK + 0x18c)             // Memory Address Reload Register DMA Channel 6
#define DMA_TCRR_6                       *(volatile unsigned long*)(PDCA_BLOCK + 0x190)    // Transmit Count Reload Register DMA Channel 6
#define DMA_CR_6                         *(volatile unsigned long*)(PDCA_BLOCK + 0x194)    // Control Register DMA Channel 6 (write-only)
#define DMA_MR_6                         *(volatile unsigned long*)(PDCA_BLOCK + 0x198)    // Mode Register DMA Channel 6
#define DMA_SR_6                         *(volatile unsigned long*)(PDCA_BLOCK + 0x19c)    // Status Register DMA Channel 6 (read-only)
#define DMA_IER_6                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1a0)    // Interrupt Enable Register DMA Channel 6 (write-only)
#define DMA_IDR_6                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1a4)    // Interrupt Disable Register DMA Channel 6 (write-only)
#define DMA_IMR_6                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1a8)    // Interrupt Mask Register DMA Channel 6 (read-only)
#define DMA_ISR_6                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1ac)    // Interrupt Status Register DMA Channel 6 (read-only)

#define DMA_MAR_7                        *(unsigned long*)(PDCA_BLOCK + 0x1c0)             // Memory Address Register DMA Channel 7
#define DMA_PSR_7                        *(unsigned long*)(PDCA_BLOCK + 0x1c4)             // Peripheral Select Register DMA Channel 7
#define DMA_TCR_7                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1c8)    // Transmit Count Register DMA Channel 7
#define DMA_MARR_7                       *(unsigned long*)(PDCA_BLOCK + 0x1cc)             // Memory Address Reload Register DMA Channel 7
#define DMA_TCRR_7                       *(volatile unsigned long*)(PDCA_BLOCK + 0x1d0)    // Transmit Count Reload Register DMA Channel 7
#define DMA_CR_7                         *(volatile unsigned long*)(PDCA_BLOCK + 0x1d4)    // Control Register DMA Channel 7 (write-only)
#define DMA_MR_7                         *(volatile unsigned long*)(PDCA_BLOCK + 0x1d8)    // Mode Register DMA Channel 7
#define DMA_SR_7                         *(volatile unsigned long*)(PDCA_BLOCK + 0x1dc)    // Status Register DMA Channel 7 (read-only)
#define DMA_IER_7                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1e0)    // Interrupt Enable Register DMA Channel 7 (write-only)
#define DMA_IDR_7                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1e4)    // Interrupt Disable Register DMA Channel 7 (write-only)
#define DMA_IMR_7                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1e8)    // Interrupt Mask Register DMA Channel 7 (read-only)
#define DMA_ISR_7                        *(volatile unsigned long*)(PDCA_BLOCK + 0x1ec)    // Interrupt Status Register DMA Channel 7 (read-only)

#define DMA_MAR_8                        *(unsigned long*)(PDCA_BLOCK + 0x200)             // Memory Address Register DMA Channel 8
#define DMA_PSR_8                        *(unsigned long*)(PDCA_BLOCK + 0x204)             // Peripheral Select Register DMA Channel 8
#define DMA_TCR_8                        *(volatile unsigned long*)(PDCA_BLOCK + 0x208)    // Transmit Count Register DMA Channel 8
#define DMA_MARR_8                       *(unsigned long*)(PDCA_BLOCK + 0x20c)             // Memory Address Reload Register DMA Channel 8
#define DMA_TCRR_8                       *(volatile unsigned long*)(PDCA_BLOCK + 0x210)    // Transmit Count Reload Register DMA Channel 8
#define DMA_CR_8                         *(volatile unsigned long*)(PDCA_BLOCK + 0x214)    // Control Register DMA Channel 8 (write-only)
#define DMA_MR_8                         *(volatile unsigned long*)(PDCA_BLOCK + 0x218)    // Mode Register DMA Channel 8
#define DMA_SR_8                         *(volatile unsigned long*)(PDCA_BLOCK + 0x21c)    // Status Register DMA Channel 8 (read-only)
#define DMA_IER_8                        *(volatile unsigned long*)(PDCA_BLOCK + 0x220)    // Interrupt Enable Register DMA Channel 8 (write-only)
#define DMA_IDR_8                        *(volatile unsigned long*)(PDCA_BLOCK + 0x224)    // Interrupt Disable Register DMA Channel 8 (write-only)
#define DMA_IMR_8                        *(volatile unsigned long*)(PDCA_BLOCK + 0x228)    // Interrupt Mask Register DMA Channel 8 (read-only)
#define DMA_ISR_8                        *(volatile unsigned long*)(PDCA_BLOCK + 0x22c)    // Interrupt Status Register DMA Channel 8 (read-only)

#define DMA_MAR_9                        *(unsigned long*)(PDCA_BLOCK + 0x240)             // Memory Address Register DMA Channel 9
#define DMA_PSR_9                        *(unsigned long*)(PDCA_BLOCK + 0x244)             // Peripheral Select Register DMA Channel 9
#define DMA_TCR_9                        *(volatile unsigned long*)(PDCA_BLOCK + 0x248)    // Transmit Count Register DMA Channel 9
#define DMA_MARR_9                       *(unsigned long*)(PDCA_BLOCK + 0x24c)             // Memory Address Reload Register DMA Channel 9
#define DMA_TCRR_9                       *(volatile unsigned long*)(PDCA_BLOCK + 0x250)    // Transmit Count Reload Register DMA Channel 9
#define DMA_CR_9                         *(volatile unsigned long*)(PDCA_BLOCK + 0x254)    // Control Register DMA Channel 9 (write-only)
#define DMA_MR_9                         *(volatile unsigned long*)(PDCA_BLOCK + 0x258)    // Mode Register DMA Channel 9
#define DMA_SR_9                         *(volatile unsigned long*)(PDCA_BLOCK + 0x25c)    // Status Register DMA Channel 9 (read-only)
#define DMA_IER_9                        *(volatile unsigned long*)(PDCA_BLOCK + 0x260)    // Interrupt Enable Register DMA Channel 9 (write-only)
#define DMA_IDR_9                        *(volatile unsigned long*)(PDCA_BLOCK + 0x264)    // Interrupt Disable Register DMA Channel 9 (write-only)
#define DMA_IMR_9                        *(volatile unsigned long*)(PDCA_BLOCK + 0x268)    // Interrupt Mask Register DMA Channel 9 (read-only)
#define DMA_ISR_9                        *(volatile unsigned long*)(PDCA_BLOCK + 0x26c)    // Interrupt Status Register DMA Channel 9 (read-only)

#define DMA_MAR_10                        *(unsigned long*)(PDCA_BLOCK + 0x280)             // Memory Address Register DMA Channel 10
#define DMA_PSR_10                        *(unsigned long*)(PDCA_BLOCK + 0x284)             // Peripheral Select Register DMA Channel 10
#define DMA_TCR_10                        *(volatile unsigned long*)(PDCA_BLOCK + 0x288)    // Transmit Count Register DMA Channel 10
#define DMA_MARR_10                       *(unsigned long*)(PDCA_BLOCK + 0x28c)             // Memory Address Reload Register DMA Channel 10
#define DMA_TCRR_10                       *(volatile unsigned long*)(PDCA_BLOCK + 0x290)    // Transmit Count Reload Register DMA Channel 10
#define DMA_CR_10                         *(volatile unsigned long*)(PDCA_BLOCK + 0x294)    // Control Register DMA Channel 10 (write-only)
#define DMA_MR_10                         *(volatile unsigned long*)(PDCA_BLOCK + 0x298)    // Mode Register DMA Channel 10
#define DMA_SR_10                         *(volatile unsigned long*)(PDCA_BLOCK + 0x29c)    // Status Register DMA Channel 10 (read-only)
#define DMA_IER_10                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2a0)    // Interrupt Enable Register DMA Channel 10 (write-only)
#define DMA_IDR_10                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2a4)    // Interrupt Disable Register DMA Channel 10 (write-only)
#define DMA_IMR_10                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2a8)    // Interrupt Mask Register DMA Channel 10 (read-only)
#define DMA_ISR_10                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2ac)    // Interrupt Status Register DMA Channel 10 (read-only)

#define DMA_MAR_11                        *(unsigned long*)(PDCA_BLOCK + 0x2c0)             // Memory Address Register DMA Channel 11
#define DMA_PSR_11                        *(unsigned long*)(PDCA_BLOCK + 0x2c4)             // Peripheral Select Register DMA Channel 11
#define DMA_TCR_11                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2c8)    // Transmit Count Register DMA Channel 11
#define DMA_MARR_11                       *(unsigned long*)(PDCA_BLOCK + 0x2cc)             // Memory Address Reload Register DMA Channel 11
#define DMA_TCRR_11                       *(volatile unsigned long*)(PDCA_BLOCK + 0x2d0)    // Transmit Count Reload Register DMA Channel 11
#define DMA_CR_11                         *(volatile unsigned long*)(PDCA_BLOCK + 0x2d4)    // Control Register DMA Channel 11 (write-only)
#define DMA_MR_11                         *(volatile unsigned long*)(PDCA_BLOCK + 0x2d8)    // Mode Register DMA Channel 11
#define DMA_SR_11                         *(volatile unsigned long*)(PDCA_BLOCK + 0x2dc)    // Status Register DMA Channel 11 (read-only)
#define DMA_IER_11                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2e0)    // Interrupt Enable Register DMA Channel 11 (write-only)
#define DMA_IDR_11                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2e4)    // Interrupt Disable Register DMA Channel 11 (write-only)
#define DMA_IMR_11                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2e8)    // Interrupt Mask Register DMA Channel 11 (read-only)
#define DMA_ISR_11                        *(volatile unsigned long*)(PDCA_BLOCK + 0x2ec)    // Interrupt Status Register DMA Channel 11 (read-only)

#define DMA_MAR_12                        *(unsigned long*)(PDCA_BLOCK + 0x300)             // Memory Address Register DMA Channel 12
#define DMA_PSR_12                        *(unsigned long*)(PDCA_BLOCK + 0x304)             // Peripheral Select Register DMA Channel 12
#define DMA_TCR_12                        *(volatile unsigned long*)(PDCA_BLOCK + 0x308)    // Transmit Count Register DMA Channel 12
#define DMA_MARR_12                       *(unsigned long*)(PDCA_BLOCK + 0x30c)             // Memory Address Reload Register DMA Channel 12
#define DMA_TCRR_12                       *(volatile unsigned long*)(PDCA_BLOCK + 0x310)    // Transmit Count Reload Register DMA Channel 12
#define DMA_CR_12                         *(volatile unsigned long*)(PDCA_BLOCK + 0x314)    // Control Register DMA Channel 12 (write-only)
#define DMA_MR_12                         *(volatile unsigned long*)(PDCA_BLOCK + 0x318)    // Mode Register DMA Channel 12
#define DMA_SR_12                         *(volatile unsigned long*)(PDCA_BLOCK + 0x31c)    // Status Register DMA Channel 12 (read-only)
#define DMA_IER_12                        *(volatile unsigned long*)(PDCA_BLOCK + 0x320)    // Interrupt Enable Register DMA Channel 12 (write-only)
#define DMA_IDR_12                        *(volatile unsigned long*)(PDCA_BLOCK + 0x324)    // Interrupt Disable Register DMA Channel 12 (write-only)
#define DMA_IMR_12                        *(volatile unsigned long*)(PDCA_BLOCK + 0x328)    // Interrupt Mask Register DMA Channel 12 (read-only)
#define DMA_ISR_12                        *(volatile unsigned long*)(PDCA_BLOCK + 0x32c)    // Interrupt Status Register DMA Channel 12 (read-only)

#define DMA_MAR_13                        *(unsigned long*)(PDCA_BLOCK + 0x340)             // Memory Address Register DMA Channel 13
#define DMA_PSR_13                        *(unsigned long*)(PDCA_BLOCK + 0x344)             // Peripheral Select Register DMA Channel 13
#define DMA_TCR_13                        *(volatile unsigned long*)(PDCA_BLOCK + 0x348)    // Transmit Count Register DMA Channel 13
#define DMA_MARR_13                       *(unsigned long*)(PDCA_BLOCK + 0x34c)             // Memory Address Reload Register DMA Channel 13
#define DMA_TCRR_13                       *(volatile unsigned long*)(PDCA_BLOCK + 0x350)    // Transmit Count Reload Register DMA Channel 13
#define DMA_CR_13                         *(volatile unsigned long*)(PDCA_BLOCK + 0x354)    // Control Register DMA Channel 13 (write-only)
#define DMA_MR_13                         *(volatile unsigned long*)(PDCA_BLOCK + 0x358)    // Mode Register DMA Channel 13
#define DMA_SR_13                         *(volatile unsigned long*)(PDCA_BLOCK + 0x35c)    // Status Register DMA Channel 13 (read-only)
#define DMA_IER_13                        *(volatile unsigned long*)(PDCA_BLOCK + 0x360)    // Interrupt Enable Register DMA Channel 13 (write-only)
#define DMA_IDR_13                        *(volatile unsigned long*)(PDCA_BLOCK + 0x364)    // Interrupt Disable Register DMA Channel 13 (write-only)
#define DMA_IMR_13                        *(volatile unsigned long*)(PDCA_BLOCK + 0x368)    // Interrupt Mask Register DMA Channel 13 (read-only)
#define DMA_ISR_13                        *(volatile unsigned long*)(PDCA_BLOCK + 0x36c)    // Interrupt Status Register DMA Channel 13 (read-only)

#define DMA_MAR_14                        *(unsigned long*)(PDCA_BLOCK + 0x380)             // Memory Address Register DMA Channel 14
#define DMA_PSR_14                        *(unsigned long*)(PDCA_BLOCK + 0x384)             // Peripheral Select Register DMA Channel 14
#define DMA_TCR_14                        *(volatile unsigned long*)(PDCA_BLOCK + 0x388)    // Transmit Count Register DMA Channel 14
#define DMA_MARR_14                       *(unsigned long*)(PDCA_BLOCK + 0x38c)             // Memory Address Reload Register DMA Channel 14
#define DMA_TCRR_14                       *(volatile unsigned long*)(PDCA_BLOCK + 0x390)    // Transmit Count Reload Register DMA Channel 14
#define DMA_CR_14                         *(volatile unsigned long*)(PDCA_BLOCK + 0x394)    // Control Register DMA Channel 14 (write-only)
#define DMA_MR_14                         *(volatile unsigned long*)(PDCA_BLOCK + 0x398)    // Mode Register DMA Channel 14
#define DMA_SR_14                         *(volatile unsigned long*)(PDCA_BLOCK + 0x39c)    // Status Register DMA Channel 14 (read-only)
#define DMA_IER_14                        *(volatile unsigned long*)(PDCA_BLOCK + 0x3a0)    // Interrupt Enable Register DMA Channel 14 (write-only)
#define DMA_IDR_14                        *(volatile unsigned long*)(PDCA_BLOCK + 0x3a4)    // Interrupt Disable Register DMA Channel 14 (write-only)
#define DMA_IMR_14                        *(volatile unsigned long*)(PDCA_BLOCK + 0x3a8)    // Interrupt Mask Register DMA Channel 14 (read-only)
#define DMA_ISR_14                        *(volatile unsigned long*)(PDCA_BLOCK + 0x3ac)    // Interrupt Status Register DMA Channel 14 (read-only)


typedef struct stUART_PDCA
{
unsigned long DMA_MAR;
unsigned long DMA_PSR;
unsigned long DMA_TCR;
unsigned long DMA_MARR;
unsigned long DMA_TCRR;
unsigned long DMA_CR;
unsigned long DMA_MR;
unsigned long DMA_SR;
unsigned long DMA_IER;
unsigned long DMA_IDR;
unsigned long DMA_IMR;
unsigned long DMA_ISR;
unsigned long ulRes[4];
} UART_PDCA;



// SDRAM Controller                                                      {7}
//
#define SDRAMC_MR                        *(volatile unsigned long*)(SDRAMC_BLOCK + 0x00) // SDRAM Mode Register
  #define SDRAM_MODE_NORMAL              0x00000000
  #define SDRAM_MODE_NOP                 0x00000001
  #define SDRAM_MODE_ALL_BANKS_PRECHARGE 0x00000002
  #define SDRAM_MODE_LOAD_REGISTER       0x00000003
  #define SDRAM_MODE_AUTO_REFRESH        0x00000004
  #define SDRAM_MODE_EXTENDED_LOAD       0x00000005
  #define SDRAM_MODE_DEEP_POWER_DOWN     0x00000006
#define SDRAMC_TR                        *(volatile unsigned long*)(SDRAMC_BLOCK + 0x04) // SDRAM Refresh Timer Register
#define SDRAMC_CR                        *(volatile unsigned long*)(SDRAMC_BLOCK + 0x08) // SDRAM Configuration Register
#define SDRAMC_HSR                       *(volatile unsigned long*)(SDRAMC_BLOCK + 0x0c) // SDRAM High Speed Register
#define SDRAMC_LPR                       *(volatile unsigned long*)(SDRAMC_BLOCK + 0x10) // SDRAM Low-Power Register
#define SDRAMC_IER                       *(volatile unsigned long*)(SDRAMC_BLOCK + 0x14) // SDRAM Interrupt Enable Register (write-only)
#define SDRAMC_IDR                       *(volatile unsigned long*)(SDRAMC_BLOCK + 0x18) // SDRAM Interrupt Disable Register (write-only)
#define SDRAMC_IMR                       *(volatile unsigned long*)(SDRAMC_BLOCK + 0x1c) // SDRAM Interrupt Mask Register (read-only)
#define SDRAMC_ISR                       *(volatile unsigned long*)(SDRAMC_BLOCK + 0x20) // SDRAM Interrupt Status Register (read-only)
#define SDRAMC_MDR                       *(unsigned long*)(SDRAMC_BLOCK + 0x24) // SDRAM Memory Device Register
// The SDRAM registers need to be read with a dummy load operation after they have been written, which is performed by this macro
//
#define SDRAM_READBACK(reg, value)       SDRAMC_##reg = value; SDRAMC_##reg

// Interrupt controller
//
#define IPR0                             *(unsigned long*)(INTC_BLOCK + 0x000) // Interrupt Priority Register 0
  #define INT_LEVEL_0                    0x00                                  // interrupt level is shifted 24 bits to the left when set
  #define INT_LEVEL_1                    0x40
  #define INT_LEVEL_2                    0x80
  #define INT_LEVEL_3                    0xc0
#define IPR1                             *(unsigned long*)(INTC_BLOCK + 0x004) // Interrupt Priority Register 1
#define IPR2                             *(unsigned long*)(INTC_BLOCK + 0x008) // Interrupt Priority Register 2
#define IPR3                             *(unsigned long*)(INTC_BLOCK + 0x00c) // Interrupt Priority Register 3
#define IPR4                             *(unsigned long*)(INTC_BLOCK + 0x010) // Interrupt Priority Register 4
#define IPR5                             *(unsigned long*)(INTC_BLOCK + 0x014) // Interrupt Priority Register 5
#define IPR6                             *(unsigned long*)(INTC_BLOCK + 0x018) // Interrupt Priority Register 6
#define IPR7                             *(unsigned long*)(INTC_BLOCK + 0x01c) // Interrupt Priority Register 7
#define IPR8                             *(unsigned long*)(INTC_BLOCK + 0x020) // Interrupt Priority Register 8
#define IPR9                             *(unsigned long*)(INTC_BLOCK + 0x024) // Interrupt Priority Register 9
#define IPR10                            *(unsigned long*)(INTC_BLOCK + 0x028) // Interrupt Priority Register 10
#define IPR11                            *(unsigned long*)(INTC_BLOCK + 0x02c) // Interrupt Priority Register 11
#define IPR12                            *(unsigned long*)(INTC_BLOCK + 0x030) // Interrupt Priority Register 12
#define IPR13                            *(unsigned long*)(INTC_BLOCK + 0x034) // Interrupt Priority Register 13
#define IPR14                            *(unsigned long*)(INTC_BLOCK + 0x038) // Interrupt Priority Register 14
#define IPR15                            *(unsigned long*)(INTC_BLOCK + 0x03c) // Interrupt Priority Register 15
#define IPR16                            *(unsigned long*)(INTC_BLOCK + 0x040) // Interrupt Priority Register 16
#define IPR17                            *(unsigned long*)(INTC_BLOCK + 0x044) // Interrupt Priority Register 17
#define IPR18                            *(unsigned long*)(INTC_BLOCK + 0x048) // Interrupt Priority Register 18
#define IPR19                            *(unsigned long*)(INTC_BLOCK + 0x04c) // Interrupt Priority Register 19
#define IPR20                            *(unsigned long*)(INTC_BLOCK + 0x050) // Interrupt Priority Register 20
#define IPR21                            *(unsigned long*)(INTC_BLOCK + 0x054) // Interrupt Priority Register 21
#define IPR22                            *(unsigned long*)(INTC_BLOCK + 0x058) // Interrupt Priority Register 22
#define IPR23                            *(unsigned long*)(INTC_BLOCK + 0x05c) // Interrupt Priority Register 23
#define IPR24                            *(unsigned long*)(INTC_BLOCK + 0x060) // Interrupt Priority Register 24
#define IPR25                            *(unsigned long*)(INTC_BLOCK + 0x064) // Interrupt Priority Register 25
#define IPR26                            *(unsigned long*)(INTC_BLOCK + 0x068) // Interrupt Priority Register 26
#define IPR27                            *(unsigned long*)(INTC_BLOCK + 0x06c) // Interrupt Priority Register 27
#define IPR28                            *(unsigned long*)(INTC_BLOCK + 0x070) // Interrupt Priority Register 28
#define IPR29                            *(unsigned long*)(INTC_BLOCK + 0x074) // Interrupt Priority Register 29
#define IPR30                            *(unsigned long*)(INTC_BLOCK + 0x078) // Interrupt Priority Register 30
#define IPR31                            *(unsigned long*)(INTC_BLOCK + 0x07c) // Interrupt Priority Register 31
#define IPR32                            *(unsigned long*)(INTC_BLOCK + 0x080) // Interrupt Priority Register 32
#define IPR33                            *(unsigned long*)(INTC_BLOCK + 0x084) // Interrupt Priority Register 33
#define IPR34                            *(unsigned long*)(INTC_BLOCK + 0x088) // Interrupt Priority Register 34
#define IPR35                            *(unsigned long*)(INTC_BLOCK + 0x08c) // Interrupt Priority Register 35
#define IPR36                            *(unsigned long*)(INTC_BLOCK + 0x090) // Interrupt Priority Register 36
#define IPR37                            *(unsigned long*)(INTC_BLOCK + 0x094) // Interrupt Priority Register 37
#define IPR38                            *(unsigned long*)(INTC_BLOCK + 0x098) // Interrupt Priority Register 38
#define IPR39                            *(unsigned long*)(INTC_BLOCK + 0x09c) // Interrupt Priority Register 39
#define IPR40                            *(unsigned long*)(INTC_BLOCK + 0x0a0) // Interrupt Priority Register 40
#define IPR41                            *(unsigned long*)(INTC_BLOCK + 0x0a4) // Interrupt Priority Register 41
#define IPR42                            *(unsigned long*)(INTC_BLOCK + 0x0a8) // Interrupt Priority Register 42
#define IPR43                            *(unsigned long*)(INTC_BLOCK + 0x0ac) // Interrupt Priority Register 43
#define IPR44                            *(unsigned long*)(INTC_BLOCK + 0x0b0) // Interrupt Priority Register 44
#define IPR45                            *(unsigned long*)(INTC_BLOCK + 0x0b4) // Interrupt Priority Register 45
#define IPR46                            *(unsigned long*)(INTC_BLOCK + 0x0b8) // Interrupt Priority Register 46
#define IPR47                            *(unsigned long*)(INTC_BLOCK + 0x0bc) // Interrupt Priority Register 47
#define IPR48                            *(unsigned long*)(INTC_BLOCK + 0x0c0) // Interrupt Priority Register 48
#define IPR49                            *(unsigned long*)(INTC_BLOCK + 0x0c4) // Interrupt Priority Register 49
#define IPR50                            *(unsigned long*)(INTC_BLOCK + 0x0c8) // Interrupt Priority Register 50
#define IPR51                            *(unsigned long*)(INTC_BLOCK + 0x0cc) // Interrupt Priority Register 51
#define IPR52                            *(unsigned long*)(INTC_BLOCK + 0x0d0) // Interrupt Priority Register 52
#define IPR53                            *(unsigned long*)(INTC_BLOCK + 0x0d4) // Interrupt Priority Register 53
#define IPR54                            *(unsigned long*)(INTC_BLOCK + 0x0d8) // Interrupt Priority Register 54
#define IPR55                            *(unsigned long*)(INTC_BLOCK + 0x0dc) // Interrupt Priority Register 55
#define IPR56                            *(unsigned long*)(INTC_BLOCK + 0x0e0) // Interrupt Priority Register 56
#define IPR57                            *(unsigned long*)(INTC_BLOCK + 0x0e4) // Interrupt Priority Register 57
#define IPR58                            *(unsigned long*)(INTC_BLOCK + 0x0e8) // Interrupt Priority Register 58
#define IPR59                            *(unsigned long*)(INTC_BLOCK + 0x0ec) // Interrupt Priority Register 59
#define IPR60                            *(unsigned long*)(INTC_BLOCK + 0x0f0) // Interrupt Priority Register 60
#define IPR61                            *(unsigned long*)(INTC_BLOCK + 0x0f4) // Interrupt Priority Register 61
#define IPR62                            *(unsigned long*)(INTC_BLOCK + 0x0f8) // Interrupt Priority Register 62
#define IPR63                            *(unsigned long*)(INTC_BLOCK + 0x0fc) // Interrupt Priority Register 63
#define IRR0                             *(volatile unsigned long*)(INTC_BLOCK + 0x100) // Interrupt Request Register 0
#define IRR1                             *(volatile unsigned long*)(INTC_BLOCK + 0x104) // Interrupt Request Register 1
#define IRR2                             *(volatile unsigned long*)(INTC_BLOCK + 0x108) // Interrupt Request Register 2
#define IRR3                             *(volatile unsigned long*)(INTC_BLOCK + 0x10c) // Interrupt Request Register 3
#define IRR4                             *(volatile unsigned long*)(INTC_BLOCK + 0x110) // Interrupt Request Register 4
#define IRR5                             *(volatile unsigned long*)(INTC_BLOCK + 0x114) // Interrupt Request Register 5
#define IRR6                             *(volatile unsigned long*)(INTC_BLOCK + 0x118) // Interrupt Request Register 6
#define IRR7                             *(volatile unsigned long*)(INTC_BLOCK + 0x11c) // Interrupt Request Register 7
#define IRR8                             *(volatile unsigned long*)(INTC_BLOCK + 0x120) // Interrupt Request Register 8
#define IRR9                             *(volatile unsigned long*)(INTC_BLOCK + 0x124) // Interrupt Request Register 9
#define IRR10                            *(volatile unsigned long*)(INTC_BLOCK + 0x128) // Interrupt Request Register 10
#define IRR11                            *(volatile unsigned long*)(INTC_BLOCK + 0x12c) // Interrupt Request Register 11
#define IRR12                            *(volatile unsigned long*)(INTC_BLOCK + 0x130) // Interrupt Request Register 12
#define IRR13                            *(volatile unsigned long*)(INTC_BLOCK + 0x134) // Interrupt Request Register 13
#define IRR14                            *(volatile unsigned long*)(INTC_BLOCK + 0x138) // Interrupt Request Register 14
#define IRR15                            *(volatile unsigned long*)(INTC_BLOCK + 0x13c) // Interrupt Request Register 15
#define IRR16                            *(volatile unsigned long*)(INTC_BLOCK + 0x140) // Interrupt Request Register 16
#define IRR17                            *(volatile unsigned long*)(INTC_BLOCK + 0x144) // Interrupt Request Register 17
#define IRR18                            *(volatile unsigned long*)(INTC_BLOCK + 0x148) // Interrupt Request Register 18
#define IRR19                            *(volatile unsigned long*)(INTC_BLOCK + 0x14c) // Interrupt Request Register 19
#define IRR20                            *(volatile unsigned long*)(INTC_BLOCK + 0x150) // Interrupt Request Register 20
#define IRR21                            *(volatile unsigned long*)(INTC_BLOCK + 0x154) // Interrupt Request Register 21
#define IRR22                            *(volatile unsigned long*)(INTC_BLOCK + 0x158) // Interrupt Request Register 22
#define IRR23                            *(volatile unsigned long*)(INTC_BLOCK + 0x15c) // Interrupt Request Register 23
#define IRR24                            *(volatile unsigned long*)(INTC_BLOCK + 0x160) // Interrupt Request Register 24
#define IRR25                            *(volatile unsigned long*)(INTC_BLOCK + 0x164) // Interrupt Request Register 25
#define IRR26                            *(volatile unsigned long*)(INTC_BLOCK + 0x168) // Interrupt Request Register 26
#define IRR27                            *(volatile unsigned long*)(INTC_BLOCK + 0x16c) // Interrupt Request Register 27
#define IRR28                            *(volatile unsigned long*)(INTC_BLOCK + 0x170) // Interrupt Request Register 28
#define IRR29                            *(volatile unsigned long*)(INTC_BLOCK + 0x174) // Interrupt Request Register 29
#define IRR30                            *(volatile unsigned long*)(INTC_BLOCK + 0x178) // Interrupt Request Register 30
#define IRR31                            *(volatile unsigned long*)(INTC_BLOCK + 0x17c) // Interrupt Request Register 31
#define IRR32                            *(volatile unsigned long*)(INTC_BLOCK + 0x180) // Interrupt Request Register 32
#define IRR33                            *(volatile unsigned long*)(INTC_BLOCK + 0x184) // Interrupt Request Register 33
#define IRR34                            *(volatile unsigned long*)(INTC_BLOCK + 0x188) // Interrupt Request Register 34
#define IRR35                            *(volatile unsigned long*)(INTC_BLOCK + 0x18c) // Interrupt Request Register 35
#define IRR36                            *(volatile unsigned long*)(INTC_BLOCK + 0x190) // Interrupt Request Register 36
#define IRR37                            *(volatile unsigned long*)(INTC_BLOCK + 0x194) // Interrupt Request Register 37
#define IRR38                            *(volatile unsigned long*)(INTC_BLOCK + 0x198) // Interrupt Request Register 38
#define IRR39                            *(volatile unsigned long*)(INTC_BLOCK + 0x19c) // Interrupt Request Register 39
#define IRR40                            *(volatile unsigned long*)(INTC_BLOCK + 0x1a0) // Interrupt Request Register 40
#define IRR41                            *(volatile unsigned long*)(INTC_BLOCK + 0x1a4) // Interrupt Request Register 41
#define IRR42                            *(volatile unsigned long*)(INTC_BLOCK + 0x1a8) // Interrupt Request Register 42
#define IRR43                            *(volatile unsigned long*)(INTC_BLOCK + 0x1ac) // Interrupt Request Register 43
#define IRR44                            *(volatile unsigned long*)(INTC_BLOCK + 0x1b0) // Interrupt Request Register 44
#define IRR45                            *(volatile unsigned long*)(INTC_BLOCK + 0x1b4) // Interrupt Request Register 45
#define IRR46                            *(volatile unsigned long*)(INTC_BLOCK + 0x1b8) // Interrupt Request Register 46
#define IRR47                            *(volatile unsigned long*)(INTC_BLOCK + 0x1bc) // Interrupt Request Register 47
#define IRR48                            *(volatile unsigned long*)(INTC_BLOCK + 0x1c0) // Interrupt Request Register 48
#define IRR49                            *(volatile unsigned long*)(INTC_BLOCK + 0x1c4) // Interrupt Request Register 49
#define IRR50                            *(volatile unsigned long*)(INTC_BLOCK + 0x1c8) // Interrupt Request Register 50
#define IRR51                            *(volatile unsigned long*)(INTC_BLOCK + 0x1cc) // Interrupt Request Register 51
#define IRR52                            *(volatile unsigned long*)(INTC_BLOCK + 0x1d0) // Interrupt Request Register 52
#define IRR53                            *(volatile unsigned long*)(INTC_BLOCK + 0x1d4) // Interrupt Request Register 53
#define IRR54                            *(volatile unsigned long*)(INTC_BLOCK + 0x1d8) // Interrupt Request Register 54
#define IRR55                            *(volatile unsigned long*)(INTC_BLOCK + 0x1dc) // Interrupt Request Register 55
#define IRR56                            *(volatile unsigned long*)(INTC_BLOCK + 0x1e0) // Interrupt Request Register 56
#define IRR57                            *(volatile unsigned long*)(INTC_BLOCK + 0x1e4) // Interrupt Request Register 57
#define IRR58                            *(volatile unsigned long*)(INTC_BLOCK + 0x1e8) // Interrupt Request Register 58
#define IRR59                            *(volatile unsigned long*)(INTC_BLOCK + 0x1ec) // Interrupt Request Register 59
#define IRR60                            *(volatile unsigned long*)(INTC_BLOCK + 0x1f0) // Interrupt Request Register 60
#define IRR61                            *(volatile unsigned long*)(INTC_BLOCK + 0x1f4) // Interrupt Request Register 61
#define IRR62                            *(volatile unsigned long*)(INTC_BLOCK + 0x1f8) // Interrupt Request Register 62
#define IRR63                            *(volatile unsigned long*)(INTC_BLOCK + 0x1fc) // Interrupt Request Register 63
#define ICR3                             *(unsigned long*)(INTC_BLOCK + 0x200) // Interrupt Cause Register 3
#define ICR2                             *(unsigned long*)(INTC_BLOCK + 0x204) // Interrupt Cause Register 2
#define ICR1                             *(unsigned long*)(INTC_BLOCK + 0x208) // Interrupt Cause Register 1
#define ICR0                             *(unsigned long*)(INTC_BLOCK + 0x20c) // Interrupt Cause Register 0

// Interrupt requests
//
#ifdef _AT32UC3C                                                         // {10}
    #define IR_GROUP_SYSBLOCK                0
    #define IR_SYSBLOCK_COMPARE              0x00000001                  // group 0

    #define IR_GROUP_UC3C_CPU                1
    #define IR_OCD_DCEMU_DIRTY               0x00000001                  // group 1
    #define IR_OCD_DCEMU_READ                0x00000002                  // group 1

    #define IR_GROUP_SAU                     2
    #define IR_SECURE_ACCESS_UNIT            0x00000001                  // group 2

    #define IR_GROUP_PDMA_0_3                3
    #define IR_PDCA_0                        0x00000001                  // group 3
    #define IR_PDCA_1                        0x00000002                  // group 3
    #define IR_PDCA_2                        0x00000004                  // group 3
    #define IR_PDCA_3                        0x00000008                  // group 3

    #define IR_GROUP_PDMA_4_7                4
    #define IR_PDCA_4                        0x00000001                  // group 4
    #define IR_PDCA_5                        0x00000002                  // group 4
    #define IR_PDCA_6                        0x00000004                  // group 4
    #define IR_PDCA_7                        0x00000008                  // group 4

    #define IR_GROUP_PDMA_8_11               5
    #define IR_PDCA_8                        0x00000001                  // group 5
    #define IR_PDCA_9                        0x00000002                  // group 5
    #define IR_PDCA_10                       0x00000004                  // group 5
    #define IR_PDCA_11                       0x00000008                  // group 5

    #define IR_GROUP_PDMA_12_15              6
    #define IR_PDCA_12                       0x00000001                  // group 6
    #define IR_PDCA_13                       0x00000002                  // group 6
    #define IR_PDCA_14                       0x00000004                  // group 6
    #define IR_PDCA_15                       0x00000008                  // group 6

    #define IR_GROUP_MDMA                    7
    #define IR_MDMA                          0x00000001                  // group 7

    #define IR_GROUP_USBB                    8
    #define IR_USBB                          0x00000001                  // group 8

    #define IR_GROUP_CANIF                   9
    #define IR_CANIF_BOFF_0                  0x00000001                  // group 9
    #define IR_CANIF_ERROR_0                 0x00000002                  // group 9
    #define IR_CANIF_RXOK_0                  0x00000004                  // group 9
    #define IR_CANIF_TXOK_0                  0x00000008                  // group 9
    #define IR_CANIF_WAKEUP_0                0x00000010                  // group 9
    #define IR_CANIF_BOFF_1                  0x00000020                  // group 9
    #define IR_CANIF_ERROR_1                 0x00000040                  // group 9
    #define IR_CANIF_RXOK_1                  0x00000080                  // group 9
    #define IR_CANIF_TXOK_1                  0x00000100                  // group 9
    #define IR_CANIF_WAKEUP_1                0x00000200                  // group 9

    #define IR_GROUP_FLASHC                  10
    #define IR_FLASHC                        0x00000001                  // group 10

    #define IR_GROUP_SDRAM                   11
    #define IR_SDRAM                         0x00000001                  // group 11

    #define IR_GROUP_PM                      12
    #define IR_PM                            0x00000001                  // group 12

    #define IR_GROUP_SCIF                    13
    #define IR_SCIF                          0x00000001                  // group 13

    #define IR_GROUP_ASYNC_TIMER             14
    #define IR_AST_ALARM                     0x00000001                  // group 14
    #define IR_AST_CLKREADY                  0x00000002                  // group 14
    #define IR_AST_OVF                       0x00000004                  // group 14
    #define IR_AST_PER                       0x00000008                  // group 14
    #define IR_AST_READY                     0x00000010                  // group 14

    #define IR_GROUP_INTC_1_4                15
    #define IR_EXTERNAL_INT_CONTROLLER_1     0x00000001                  // group 15
    #define IR_EXTERNAL_INT_CONTROLLER_2     0x00000002                  // group 15
    #define IR_EXTERNAL_INT_CONTROLLER_3     0x00000004                  // group 15
    #define IR_EXTERNAL_INT_CONTROLLER_4     0x00000008                  // group 15

    #define IR_GROUP_INTC_5_8                16
    #define IR_EXTERNAL_INT_CONTROLLER_5     0x00000001                  // group 16
    #define IR_EXTERNAL_INT_CONTROLLER_6     0x00000002                  // group 16
    #define IR_EXTERNAL_INT_CONTROLLER_7     0x00000004                  // group 16
    #define IR_EXTERNAL_INT_CONTROLLER_8     0x00000008                  // group 16

    #define IR_GROUP_FREQUENCY_METER         17
    #define IR_FREQUENCY_METER               0x00000001                  // group 17

    #define IR_GROUP_GPIO                    18
    #define IR_GPIO_0                        0x00000001                  // group 18 (P0-7..0)
    #define IR_GPIO_1                        0x00000002                  // group 18 (P0-15..8)
    #define IR_GPIO_2                        0x00000004                  // group 18 (P0-23..16)
    #define IR_GPIO_3                        0x00000008                  // group 18 (P0-31..17)
    #define IR_GPIO_4                        0x00000010                  // group 18 (P1-7..0)
    #define IR_GPIO_5                        0x00000020                  // group 18 (P1-15..8)
    #define IR_GPIO_6                        0x00000040                  // group 18 (P1-23..16)
    #define IR_GPIO_7                        0x00000080                  // group 18 (P1-31..17)
    #define IR_GPIO_8                        0x00000100                  // group 18 (P2-7..0)
    #define IR_GPIO_9                        0x00000200                  // group 18 (P2-15..8)
    #define IR_GPIO_10                       0x00000400                  // group 18 (P2-23..16)
    #define IR_GPIO_11                       0x00000800                  // group 18 (P2-31..17)
    #define IR_GPIO_12                       0x00001000                  // group 18 (P3-7..0)
    #define IR_GPIO_13                       0x00002000                  // group 18 (P3-15..8)
    #define IR_GPIO_14                       0x00004000                  // group 18 (P3-23..16
    #define IR_GPIO_15                       0x00008000                  // group 18 (P3-31..24)
    #define IRR_GPIO                         IRR18
    #define IPR_GPIO                         IPR18

    #define IR_GROUP_USART0                  19
    #define IR_USART0                        0x00000001                  // group 19
    #define IPR_UART0                        IPR19
    #define IRR_UART0                        IRR19

    #define IR_GROUP_USART1                  20
    #define IR_USART1                        0x00000001                  // group 20
    #define IPR_UART1                        IPR20
    #define IRR_UART1                        IRR20

    #define IR_GROUP_USART2                  21
    #define IR_USART2                        0x00000001                  // group 21
    #define IPR_UART2                        IPR21
    #define IRR_UART2                        IRR21

    #define IR_GROUP_USART3                  22
    #define IR_USART3                        0x00000001                  // group 22
    #define IPR_UART3                        IPR22
    #define IRR_UART3                        IRR22

    #define IR_GROUP_SPI0                    23
    #define IR_SPI0                          0x00000001                  // group 23

    #define IR_GROUP_SPI1                    24
    #define IR_SPI1                          0x00000001                  // group 24

    #define IR_GROUP_TWI_MASTER_0            25
    #define IR_TWIM0                         0x00000001                  // group 25

    #define IR_GROUP_TWI_MASTER_1            26
    #define IR_TWIM1                         0x00000001                  // group 26

    #define IR_GROUP_TWI_SLAVE_0             27
    #define IR_TWIS0                         0x00000001                  // group 27

    #define IR_GROUP_TWI_SLAVE_1             28
    #define IR_TWIS1                         0x00000001                  // group 28

    #define IR_GROUP_I2S                     29
    #define IR_II2S                          0x00000001                  // group 29

    #define IR_GROUP_PWM                     30
    #define IR_PWM                           0x00000001                  // group 30

    #define IR_GROUP_QUAD_DEC_0              31
    #define IR_QDEC0                         0x00000001                  // group 31

    #define IR_GROUP_QUAD_DEC_1              32
    #define IR_QDEC1                         000000001                   // group 32

    #define IR_GROUP_TIMERS_0                33
    #define IR_TC00                          0x00000001                  // group 33
    #define IR_TC01                          0x00000002                  // group 33
    #define IR_TC02                          0x00000004                  // group 33

    #define IR_GROUP_TIMERS_1                34
    #define IR_TC10                          0x00000001                  // group 34
    #define IR_TC11                          0x00000002                  // group 34
    #define IR_TC12                          0x00000004                  // group 34

    #define IR_GROUP_PERIPH_EVENTS           35
    #define IR_PEVC_TR                       0x00000001                  // group 35
    #define IR_PEVC_OV                       0x00000002                  // group 35

    #define IR_GROUP_ADC                     36
    #define IR_ADCIFA_SEQ0                   0x00000001                  // group 36
    #define IR_ADCIFA_SEQ1                   0x00000002                  // group 36
    #define IR_ADCIFA_SUTD                   0x00000004                  // group 36
    #define IR_ADCIFA_WINDOW                 0x00000008                  // group 36
    #define IR_ADCIFA_AWAKEUP                0x00000010                  // group 36
    #define IR_ADCIFA_PENDET                 0x00000020                  // group 36

    #define IR_GROUP_ANALOG_COMP_0           37
    #define IR_ACIFA0                        0x00000001                  // group 37

    #define IR_GROUP_ANALOG_COMP_1           38
    #define IR_ACIFA1                        0x00000001                  // group 38

    #define IR_GROUP_DAC_0                   39
    #define IR_DACIFB0_CHB_UNDERRUN          0x00000001                  // group 39
    #define IR_DACIFB0_CHB_OVERRUN           0x00000002                  // group 39
    #define IR_DACIFB0_CHB_DATA_EMPTY        0x00000004                  // group 39
    #define IR_DACIFB0_CHA_UNDERRUN          0x00000008                  // group 39
    #define IR_DACIFB0_CHA_OVERRUN           0x00000010                  // group 39
    #define IR_DACIFB0_CHA_DATA_EMPTY        0x00000020                  // group 39

    #define IR_GROUP_DAC_1                   40
    #define IR_DACIFB1_CHB_UNDERRUN          0x00000001                  // group 40
    #define IR_DACIFB1_CHB_OVERRUN           0x00000002                  // group 40
    #define IR_DACIFB1_CHB_DATA_EMPTY        0x00000004                  // group 40
    #define IR_DACIFB1_CHA_UNDERRUN          0x00000008                  // group 40
    #define IR_DACIFB1_CHA_OVERRUN           0x00000010                  // group 40
    #define IR_DACIFB1_CHA_DATA_EMPTY        0x00000020                  // group 40

    #define IR_GROUP_aWIRE                   41
    #define IR_AW                            0x00000001                  // group 41

    #define IR_GROUP_MACB                    42
    #define IR_MACB                          0x00000001                  // group 42
    #define IRR_MACB                         IRR42
    #define IPR_MACB                         IPR42

    #define IR_GROUP_USART4                  44
    #define IR_USART4                        0x00000001                  // group 44
    #define IPR_UART4                        IPR44
    #define IRR_UART4                        IRR44

    #define IR_GROUP_TWI_MASTER_2            45
    #define IR_TWIM2                         0x00000001                  // group 45

    #define IR_GROUP_TWI_SLAVE_2             46
    #define IR_TWIS2                         0x00000001                  // group 46
#else
    #define IR_GROUP_SYSBLOCK                0
    #define IR_SYSBLOCK_COMPARE              0x00000001                  // group 0

    #define IR_GROUP_INTC_RTC_POW_FREQ       1
    #define IR_EXTERNAL_INT_CONTROLLER_0     0x00000001                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_1     0x00000002                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_2     0x00000004                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_3     0x00000008                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_4     0x00000010                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_5     0x00000020                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_6     0x00000040                  // group 1
    #define IR_EXTERNAL_INT_CONTROLLER_7     0x00000080                  // group 1
    #define IR_REAL_TIME_COUNTER             0x00000100                  // group 1
    #define IR_POWER_MANAGEMENT              0x00000200                  // group 1
    #define IR_FREQUENCY_METER               0x00000400                  // group 1

    #define IR_GROUP_GPIO                    2
    #define IR_GPIO_0                        0x00000001                  // group 2 (P0-7..0)
    #define IR_GPIO_1                        0x00000002                  // group 2 (P0-15..8)
    #define IR_GPIO_2                        0x00000004                  // group 2 (P0-23..16)
    #define IR_GPIO_3                        0x00000008                  // group 2 (P0-31..17)
    #define IR_GPIO_4                        0x00000010                  // group 2 (P1-7..0)
    #define IR_GPIO_5                        0x00000020                  // group 2 (P1-15..8)
    #define IR_GPIO_6                        0x00000040                  // group 2 (P1-23..16)
    #define IR_GPIO_7                        0x00000080                  // group 2 (P1-31..17)
    #define IR_GPIO_8                        0x00000100                  // group 2 (P2-7..0)
    #define IR_GPIO_9                        0x00000200                  // group 2 (P2-15..8)
    #define IR_GPIO_10                       0x00000400                  // group 2 (P2-23..16)
    #define IR_GPIO_11                       0x00000800                  // group 2 (P2-31..17)
    #define IR_GPIO_12                       0x00001000                  // group 2 (P3-7..0)
    #define IR_GPIO_13                       0x00002000                  // group 2 (P3-15..8)
    #define IRR_GPIO                         IRR2
    #define IPR_GPIO                         IPR2

    #define IR_GROUP_PDCA                    3
    #define IR_PDCA0                         0x00000001                  // group 3
    #define IR_PDCA1                         0x00000002                  // group 3
    #define IR_PDCA2                         0x00000004                  // group 3
    #define IR_PDCA3                         0x00000008                  // group 3
    #define IR_PDCA4                         0x00000010                  // group 3
    #define IR_PDCA5                         0x00000020                  // group 3
    #define IR_PDCA6                         0x00000040                  // group 3
    #define IR_PDCA7                         0x00000080                  // group 3
    #define IR_PDCA8                         0x00000100                  // group 3
    #define IR_PDCA9                         0x00000200                  // group 3
    #define IR_PDCA10                        0x00000400                  // group 3
    #define IR_PDCA11                        0x00000800                  // group 3
    #define IR_PDCA12                        0x00001000                  // group 3
    #define IR_PDCA13                        0x00002000                  // group 3
    #define IR_PDCA14                        0x00004000                  // group 3

    #define IR_GROUP_FLASHC                  4
    #define IR_FLASHC                        0x00000001                  // group 4

    #define IR_GROUP_USART0                  5
    #define IR_USART0                        0x00000001                  // group 5
    #define IPR_UART0                        IPR5
    #define IRR_UART0                        IRR5

    #define IR_GROUP_USART1                  6
    #define IR_USART1                        0x00000001                  // group 6
    #define IPR_UART1                        IPR6
    #define IRR_UART1                        IRR6

    #define IR_GROUP_USART2                  7
    #define IR_USART2                        0x00000001                  // group 7
    #define IPR_UART2                        IPR7
    #define IRR_UART2                        IRR7

    #define IR_GROUP_USART3                  8
    #define IR_USART3                        0x00000001                  // group 8
    #define IPR_UART3                        IPR8
    #define IRR_UART3                        IRR8

    #define IR_GROUP_SPI0                    9
    #define IR_SPI0                          0x00000001                  // group 9

    #define IR_GROUP_SPI1                    10
    #define IR_SPI1                          0x00000001                  // group 10

    #define IR_GROUP_TWI                     11
    #define IR_TWI                           0x00000001                  // group 11

    #define IR_GROUP_PWM                     12
    #define IR_PWM                           0x00000001                  // group 12

    #define IR_GROUP_SCC                     13
    #define IR_SCC                           0x00000001                  // group 13

    #define IR_GROUP_TIMERS                  14
    #define IR_TC0                           0x00000001                  // group 14
    #define IR_TC1                           0x00000002                  // group 14
    #define IR_TC2                           0x00000004                  // group 14

    #define IR_GROUP_ADC                     15
    #define IR_ADC                           0x00000001                  // group 15

    #define IR_GROUP_MACB                    16
    #define IR_MACB                          0x00000001                  // group 16
    #define IRR_MACB                         IRR16
    #define IPR_MACB                         IPR16

    #define IR_GROUP_USBB                    17
    #define IR_USBB                          0x00000001                  // group 17

    #define IR_GROUP_SDRAM                   18
    #define IR_SDRAM                         0x00000001                  // group 18

    #define IR_GROUP_DAC                     19
    #define IR_DAC                           0x00000001                  // group 19
#endif




// Event table which is put to SRAM to handle all exceptions and interrupts
//
typedef struct stEXCEPTION_TABLE
{
    unsigned long evUnrecoverableException;
    unsigned long evTLBmultipleHit;
    unsigned long evBusErrorDataFetch;
    unsigned long evBusErrorInstructionFetch;
    unsigned long evNonMaskableInterrupt;
    unsigned long evMissingAddress;
    unsigned long evITLBProtection;
    unsigned long evBreakPoint;
    unsigned long evIllegalOpcode;
    unsigned long evUnimplementedInstruction;
    unsigned long evPrivilegeViolation;
    unsigned long evFloatingPoint;
    unsigned long evCoprocessorAbsent;
    unsigned long evDataAddressRead;
    unsigned long evDataAddressWrite;
    unsigned long evDTLBProtectionRead;
    unsigned long evDTLBProtectionWrite;
    unsigned long evDTLBModified;
    unsigned long evGroup0;                                              // 0x48
    unsigned long evAdd0;                                                // 0x4c
    unsigned long evITLBMiss;
    unsigned long evGroup1;                                              // 0x54
    unsigned long evAdd1;                                                // 0x58
    unsigned long evRes1;                                                // 0x5c
    unsigned long evITLBMissRead;
    unsigned long evGroup2;                                              // 0x64
    unsigned long evAdd2;                                                // 0x68
    unsigned long evRes2;                                                // 0x6c
    unsigned long evITLBMissWrite;
    unsigned long evGroup3;                                              // 0x74
    unsigned long evAdd3;                                                // 0x78
    unsigned long evGroup4;                                              // 0x7c
    unsigned long evAdd4;                                                // 0x80
    unsigned long evGroup5;                                              // 0x84
    unsigned long evAdd5;                                                // 0x88
    unsigned long evGroup6;                                              // 0x8c
    unsigned long evAdd6;                                                // 0x90
    unsigned long evGroup7;                                              // 0x94
    unsigned long evAdd7;                                                // 0x98
    unsigned long evGroup8;                                              // 0x9c
    unsigned long evAdd8;                                                // 0xa0
    unsigned long evGroup9;                                              // 0xa4
    unsigned long evAdd9;                                                // 0xa8
    unsigned long evGroup10;                                             // 0xac
    unsigned long evAdd10;                                               // 0xb0
    unsigned long evGroup11;                                             // 0xb4
    unsigned long evAdd11;                                               // 0xb8
    unsigned long evGroup12;                                             // 0xbc
    unsigned long evAdd12;                                               // 0xc0
    unsigned long evGroup13;                                             // 0xc4
    unsigned long evAdd13;                                               // 0xc8
    unsigned long evGroup14;                                             // 0xcc
    unsigned long evAdd14;                                               // 0xd0
    unsigned long evGroup15;                                             // 0xd4
    unsigned long evAdd15;                                               // 0xd8
    unsigned long evGroup16;                                             // 0xdc
    unsigned long evAdd16;                                               // 0xe0
    unsigned long evGroup17;                                             // 0xe4
    unsigned long evAdd17;                                               // 0xe8
    unsigned long evGroup18;                                             // 0xec
    unsigned long evAdd18;                                               // 0xf0
    unsigned long evGroup19;                                             // 0xf4
    unsigned long evAdd19;                                               // 0xf8
    unsigned long evRes3;                                                // 0xfc
    unsigned long evSupervisorCall;                                      // 0x100
#ifdef _AT32UC3C
    unsigned long evGroup20;
    unsigned long evAdd20;
    unsigned long evGroup21;
    unsigned long evAdd21;
    unsigned long evGroup22;
    unsigned long evAdd22;
    unsigned long evGroup23;
    unsigned long evAdd23;
    unsigned long evGroup24;
    unsigned long evAdd24;
    unsigned long evGroup25;
    unsigned long evAdd25;
    unsigned long evGroup26;
    unsigned long evAdd26;
    unsigned long evGroup27;
    unsigned long evAdd27;
    unsigned long evGroup28;
    unsigned long evAdd28;
    unsigned long evGroup29;
    unsigned long evAdd29;
    unsigned long evGroup30;
    unsigned long evAdd30;
    unsigned long evGroup31;
    unsigned long evAdd31;
    unsigned long evGroup32;
    unsigned long evAdd32;
    unsigned long evGroup33;
    unsigned long evAdd33;
    unsigned long evGroup34;
    unsigned long evAdd34;
    unsigned long evGroup35;
    unsigned long evAdd35;
    unsigned long evGroup36;
    unsigned long evAdd36;
    unsigned long evGroup37;
    unsigned long evAdd37;
    unsigned long evGroup38;
    unsigned long evAdd38;
    unsigned long evGroup39;
    unsigned long evAdd39;
    unsigned long evGroup40;
    unsigned long evAdd40;
    unsigned long evGroup41;
    unsigned long evAdd41;
    unsigned long evGroup42;
    unsigned long evAdd42;
    unsigned long evGroup43;
    unsigned long evAdd43;
    unsigned long evGroup44;
    unsigned long evAdd44;
    unsigned long evGroup45;
    unsigned long evAdd45;
    unsigned long evGroup46;
    unsigned long evAdd46;
#endif
} EXCEPTION_TABLE;


#define BRANCH_TO_SELF                   0xe08f0000                      // AVR32 machine code to create a forever loop
#define LOAD_PC_WITH_NEXT_VALUE          0x481fd703                      // LDDPC relative plus NOP


// Timers
//
#define TC_CCR_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x0) // Channel Control Register (write only)
  #define TIM_CLKEN                      0x00000001                                       // enable timer
  #define TIM_CLKDIS                     0x00000002                                       // disable timer
  #define TIM_SWTRG                      0x00000004                                       // software trigger (reset and start)
#define TC_CMR_0                         *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x4) // Channel Mode Register (read/write)
  // Waveform mode
  #define TIMER_CLOCK_1                  0x00000000                                       // 32kHz oscillator
  #define TIMER_CLOCK_2                  0x00000001                                       // PBA/2   [note that rev. B (-UES, engineering samples) have /4]
  #define TIMER_CLOCK_3                  0x00000002                                       // PBA/8   [/8]
  #define TIMER_CLOCK_4                  0x00000003                                       // PBA/32  [/16]
  #define TIMER_CLOCK_5                  0x00000004                                       // PBA/128 [/32]
  #define CPCSTOP                        0x00000040                                       // Counter Clock stopped with RC compare
  #define CPCDIS                         0x00000080                                       // Counter Clock disable with RC compare
  #define WAVSEL_UP_TRG_RC               0x00004000                                       // up mode with automatic trigger on RC compare
  #define WAVE                           0x00008000                                       // enable waveform mode
  #define ACPA_RA_SET                    0x00010000
  #define ACPC_RC_CLEAR                  0x00080000
  #define ACPC_RC_TOGGLE                 0x000c0000
#define TC_CV_0                          *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x10) // Counter Value (read only)
#define TC_RA_0                          *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x14) // Register A (read/write)
#define TC_RB_0                          *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x18) // Register B (read/write)
#define TC_RC_0                          *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x1c) // Register C (read/write)
#define TC_SR_0                          *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x20) // Status Register (read only)
  #define TIM_COVSF                      0x00000001                                        // Counter Overflow Status
  #define TIM_LOVRS                      0x00000002                                        // Load Overrun Status
  #define TIM_CPAS                       0x00000004                                        // RA Compare Status
  #define TIM_CPBS                       0x00000008                                        // RB Compare Status
  #define TIM_CPCS                       0x00000010                                        // RC Compare Status
  #define TIM_LDRAS                      0x00000020                                        // RA Loading Status
  #define TIM_LDRBS                      0x00000040                                        // RB Loading Status
  #define TIM_ETRGS                      0x00000080                                        // External Trigger Status
  #define TIM_CLKSTA                     0x00010000                                        // Clock is enabled
  #define TIM_MTIOA                      0x00020000                                        // MTIOA Mirror
  #define TIM_MTIOB                      0x00040000                                        // MTIOB Mirror
#define TC_IER_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x24) // Interrupt Enable Register (write only)
#define TC_IDR_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x28) // Interrupt Disable Register (write only)
#define TC_IMR_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)

#define TC_CCR_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x0) // Channel Control Register (write only)
#define TC_CMR_1                         *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x4) // Channel Mode Register (read/write)
#define TC_CV_1                          *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x10) // Counter Value (read only)
#define TC_RA_1                          *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x14) // Register A (read/write)
#define TC_RB_1                          *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x18) // Register B (read/write)
#define TC_RC_1                          *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x1c) // Register C (read/write)
#define TC_SR_1                          *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x20) // Status Register (read only)
#define TC_IER_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x24) // Interrupt Enable Register (write only)
#define TC_IDR_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x28) // Interrupt Disable Register (write only)
#define TC_IMR_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)

#define TC_CCR_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x0) // Channel Control Register (write only)
#define TC_CMR_2                         *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x4) // Channel Mode Register (read/write)
#define TC_CV_2                          *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x10) // Counter Value (read only)
#define TC_RA_2                          *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x14) // Register A (read/write)
#define TC_RB_2                          *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x18) // Register B (read/write)
#define TC_RC_2                          *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x1c) // Register C (read/write)
  #define LOW_RES_MS                     (((2*MASTER_CLOCK/1024000) + 1)/2)                // slowest clock rate to timer, giving longest period but minimum resolution
  #define MED_RES_MS                     (((2*MASTER_CLOCK/128000) + 1)/2/                 // medium clock rate to timer, giving medium period and medium resolution
  #define HIGH_RES_MS                    (((2*MASTER_CLOCK/32000) + 1)/2)                  // high clock rate to timer, giving minimum period and maximum resolution
#define TC_SR_2                          *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x20) // Status Register (read only)
#define TC_IER_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x24) // Interrupt Enable Register (write only)
#define TC_IDR_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x28) // Interrupt Disable Register (write only)
#define TC_IMR_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)

#define TC_BCR                           *(volatile unsigned long*)(TIMER_CONTROL_BLOCK + 0x0) // TC Block Control Register (write only)
#define TC_BMR                           *(unsigned long*)(TIMER_CONTROL_BLOCK + 0x4)      // TC Block Mode Register (read/write)

#define HW_TIMERS_AVAILABLE   3                                          // AVR32 has 3 x 16 bit timer / counters

typedef struct stTIMER_MODULE
{
unsigned long       TC_CCR;
unsigned long       TC_CMR;
unsigned long       Res_1[2];
const unsigned long TC_CV;
unsigned long       TC_RA;
unsigned long       TC_RB;
unsigned long       TC_RC;
unsigned long       TC_SR;
unsigned long       TC_IER;
unsigned long       TC_IDR;
unsigned long       TC_IMR;
unsigned long       Res_2[4];
} TIMER_MODULE;


// Serial interfaces
//
#define US_CR_0                          *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x0) // Control register (write only)
  #define AVR32_TX_ON                    0x00000040
  #define AVR32_RX_ON                    0x00000010
  #define AVR32_TX_OFF                   0x00000080
  #define AVR32_RX_OFF                   0x00000020
  #define AVR32_START_BRK                0x00000200
  #define AVR32_STOP_BRK                 0x00000400
  #define AVR32_STTTO                    0x00000800                      // start timeout
  #define AVR32_RETTO                    0x00008000                      // retrigger timeout
  #define AVR32_RTSEN                    0x00040000                      // assert RTS line
  #define AVR32_RTSDIS                   0x00080000                      // negate RTS line
#define US_MR_0                          *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x4)  // Mode register (read/write)
  #define US_MR_OFFSET                   0x04
  #define AVR32_NO_PARITY                0x00000800
  #define AVR32_ODD_PARITY               0x00000200
  #define AVR32_EVEN_PARITY              0x00000000
  #define AVR32_2_STOPS                  0x00002000
  #define AVR32_ONE_HALF_STOPS           0x00001000
  #define AVR32_CHAR_7                   0x00000080
  #define AVR32_CHAR_8                   0x000000c0
  #define USART_MODE_MASK                0x0000000f
  #define USART_MODE_NORMAL              0x00000000
  #define USART_MODE_RS485               0x00000001
  #define USART_MODE_HW_HANDSHAKE        0x00000002
  #define USART_MODE_MODEM               0x00000003
  #define USART_MODE_ISO7816_T_0         0x00000004
  #define USART_MODE_ISO7816_T_1         0x00000006
  #define USART_MODE_IrDA                0x00000008
  #define USART_MODE9                    0x00020000                      // {9}
#define US_IER_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x8) // Interrupt enable register (write only)
#define US_IDR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0xc) // Interrupt Disable register (write only)
#define US_IMR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x10)// Interrupt Mask Register (read only)
  #define RXRDY                          0x00000001                      // enable receive character ready interrupt
  #define TXRDY                          0x00000002                      // enable transmitter ready interrupt
  #define ENDRX                          0x00000008                      // enable end of receive transfer interrupt
  #define ENDTX                          0x00000010                      // enable end of transmit transfer interrupt
  #define RX_TIMEOUT                     0x00000100                      // enable idle timeout interrupt
  #define CTSIC                          0x00080000                      // CTS input change interrupt enable
#define US_CSR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
  #define CTS_HIGH                       0x00100000                      // state of CTS input
  #define _RX_ENABLED                    0x80000000                      // this bit is used only by the simulator!
#define US_RHR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x18)// Receiver Holding Register (read only)
  #define US_RHR_9TH_BIT                 0x00000100                      // 9th bit of last character received if RXRDY is set {9}
  #define US_RHR_RXSYNH                  0x00008000                      // last character received is a command {9}
#define US_THR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x1c)// Transmitter Holding Register (write only)
  #define US_THR_9TH_BIT                 0x00000100                      // 9th bit of character to be transmitted if TXRDY is not set {9}
  #define US_THR_TXSYNH                  0x00008000                      // next character set is a command {9}
#define US_BRGR_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x20)// Baud Rate Generator Register (read/write)
  #define US_BRGR_OFFSET                 0x20
#define US_RTOR_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x24)// Receive Timeout Register (read/write)
#define US_TTGR_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x28)// Transmitter Time Guard Register (read/write)
#define US_RES1_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x2c)// Reserved
#define US_RES2_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x30)// Reserved
#define US_RES3_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x34)// Reserved
#define US_RES4_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x38)// Reserved
#define US_RES5_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x3c)// Reserved
#define US_FIDI_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x40)// Transmitter Time Guard Register (read/write)
#define US_NER_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x44)// Number of Errors Register (read only)
#define US_RES6_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x48)// Reserved
#define US_IF_0                          *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
#define US_MAN_0                         *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)
#define US_VERSION_0                     *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0xfc) // Version Register

#define US_CR_1                          *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x0) // Control register (write only)
#define US_MR_1                          *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x4) // Mode register (read/write)
#define US_IER_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x8) // Interrupt enable register (write only)
#define US_IDR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0xc) // Interrupt Disable register (write only)
#define US_IMR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x10)// Interrupt Mask Register (read only)
#define US_CSR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
#define US_RHR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x18)// Receiver Holding Register (read only)
#define US_THR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x1c)// Transmitter Holding Register (write only)
#define US_BRGR_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x20)// Baud Rate Generator Register (read/write)
#define US_RTOR_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x24)// Receive Timeout Register (read/write)
#define US_TTGR_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x28)// Transmitter Time Guard Register (read/write)
#define US_RES1_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x2c)// Reserved
#define US_RES2_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x30)// Reserved
#define US_RES3_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x34)// Reserved
#define US_RES4_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x38)// Reserved
#define US_RES5_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x3c)// Reserved
#define US_FIDI_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x40)// Transmitter Time Guard Register (read/write)
#define US_NER_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x44)// Number of Errors Register (read only)
#define US_RES6_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x48)// Reserved
#define US_IF_1                          *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
#define US_MAN_1                         *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)
#define US_VERSION_1                     *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0xfc) // Version Register

#define US_CR_2                          *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x0) // Control register (write only)
#define US_MR_2                          *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x4) // Mode register (read/write)
#define US_IER_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x8) // Interrupt enable register (write only)
#define US_IDR_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0xc) // Interrupt Disable register (write only)
#define US_IMR_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x10)// Interrupt Mask Register (read only)
#define US_CSR_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
#define US_RHR_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x18)// Receiver Holding Register (read only)
#define US_THR_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x1c)// Transmitter Holding Register (write only)
#define US_BRGR_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x20)// Baud Rate Generator Register (read/write)
#define US_RTOR_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x24)// Receive Timeout Register (read/write)
#define US_TTGR_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x28)// Transmitter Time Guard Register (read/write)
#define US_RES1_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x2c)// Reserved
#define US_RES2_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x30)// Reserved
#define US_RES3_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x34)// Reserved
#define US_RES4_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x38)// Reserved
#define US_RES5_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x3c)// Reserved
#define US_FIDI_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x40)// Transmitter Time Guard Register (read/write)
#define US_NER_2                         *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x44)// Number of Errors Register (read only)
#define US_RES6_2                        *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x48)// Reserved
#define US_IF_2                          *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
#define US_MAN_2                         *(unsigned long*)(USART2_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)
#define US_VERSION_2                     *(volatile unsigned long*)(USART2_PERIPHERAL_BLOCK + 0xfc) // Version Register

#define US_CR_3                          *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x0) // Control register (write only)
#define US_MR_3                          *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x4) // Mode register (read/write)
#define US_IER_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x8) // Interrupt enable register (write only)
#define US_IDR_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0xc) // Interrupt Disable register (write only)
#define US_IMR_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x10)// Interrupt Mask Register (read only)
#define US_CSR_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
#define US_RHR_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x18)// Receiver Holding Register (read only)
#define US_THR_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x1c)// Transmitter Holding Register (write only)
#define US_BRGR_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x20)// Baud Rate Generator Register (read/write)
#define US_RTOR_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x24)// Receive Timeout Register (read/write)
#define US_TTGR_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x28)// Transmitter Time Guard Register (read/write)
#define US_RES1_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x2c)// Reserved
#define US_RES2_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x30)// Reserved
#define US_RES3_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x34)// Reserved
#define US_RES4_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x38)// Reserved
#define US_RES5_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x3c)// Reserved
#define US_FIDI_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x40)// Transmitter Time Guard Register (read/write)
#define US_NER_3                         *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x44)// Number of Errors Register (read only)
#define US_RES6_3                        *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x48)// Reserved
#define US_IF_3                          *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
#define US_MAN_3                         *(unsigned long*)(USART3_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)
#define US_VERSION_3                     *(volatile unsigned long*)(USART3_PERIPHERAL_BLOCK + 0xfc) // Version Register

#ifdef _AT32UC3C
    #define US_CR_4                      *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x0) // Control register (write only)
    #define US_MR_4                      *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x4) // Mode register (read/write)
    #define US_IER_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x8) // Interrupt enable register (write only)
    #define US_IDR_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0xc) // Interrupt Disable register (write only)
    #define US_IMR_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x10)// Interrupt Mask Register (read only)
    #define US_CSR_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
    #define US_RHR_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x18)// Receiver Holding Register (read only)
    #define US_THR_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x1c)// Transmitter Holding Register (write only)
    #define US_BRGR_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x20)// Baud Rate Generator Register (read/write)
    #define US_RTOR_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x24)// Receive Timeout Register (read/write)
    #define US_TTGR_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x28)// Transmitter Time Guard Register (read/write)
    #define US_RES1_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x2c)// Reserved
    #define US_RES2_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x30)// Reserved
    #define US_RES3_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x34)// Reserved
    #define US_RES4_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x38)// Reserved
    #define US_RES5_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x3c)// Reserved
    #define US_FIDI_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x40)// Transmitter Time Guard Register (read/write)
    #define US_NER_4                     *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x44)// Number of Errors Register (read only)
    #define US_RES6_4                    *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x48)// Reserved
    #define US_IF_4                      *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
    #define US_MAN_4                     *(unsigned long*)(USART4_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)
    #define US_VERSION_4                 *(volatile unsigned long*)(USART4_PERIPHERAL_BLOCK + 0xfc) // Version Register
#endif

// I2C Interface (TWI)
//
#define TWI_CR                           *(volatile unsigned long*)(TWI_BLOCK + 0x00)      // Control Register (write only)
  #define TWI_START                      0x01
  #define TWI_STOP                       0x02
  #define TWI_MSEN                       0x04
  #define TWI_MSDIS                      0x08
  #define TWI_SWRST                      0x80
#define TWI_MMR                          *(unsigned long*)(TWI_BLOCK + 0x04)               // Master Mode Register (read/write)
  #define BYTE_COMMAND_PROT              0x00000000
  #define ONE_BYTE_INTERNAL_ADDRESS      0x00000100
  #define TWO_BYTE_INTERNAL_ADDRESS      0x00000200
  #define THREE_BYTE_INTERNAL_ADDRESS    0x00000300
  #define TWI_MREAD                      0x00001000
  #define TWI_ADDRESS_SHIFT              16
#define TWI_SMR                          *(unsigned long*)(TWI_BLOCK + 0x08)               // Slave Mode Register (read/write)
#define TWI_IADR                         *(unsigned long*)(TWI_BLOCK + 0x0c)               // Internal Address Register (read/write)
#define TWI_CWGR                         *(unsigned long*)(TWI_BLOCK + 0x10)               // Clock Waveform Generator Register (read/write)
#define TWI_SR                           *(volatile unsigned long*)(TWI_BLOCK + 0x20)      // Status Register (read only)
  #define TWI_TXCOMP                     0x00000001
  #define TWI_RXRDY                      0x00000002
  #define TWI_TXRDY                      0x00000004
  #define TWI_OVRE                       0x00000040
  #define TWI_UNRE                       0x00000080
  #define TWI_NACK                       0x00000100
#define TWI_IER                          *(volatile unsigned long*)(TWI_BLOCK + 0x24)      // Interrupt Enable Register (write only)
#define TWI_IDR                          *(volatile unsigned long*)(TWI_BLOCK + 0x28)      // Interrupt Disable Register (write only)
#define TWI_IMR                          *(volatile unsigned long*)(TWI_BLOCK + 0x2c)      // Interrupt Mask Register (read only)
#define TWI_RHR                          *(volatile unsigned long*)(TWI_BLOCK + 0x30)      // Receive Holding Register (read only)
#define TWI_THR                          *(unsigned long*)(TWI_BLOCK + 0x34)               // Transmit Holding Register (read/write)

#define TWI_VER                          *(volatile unsigned long*)(TWI_BLOCK + 0xfc)      // Version Register (read only)


// ADC block
//
#define ADC_CR                           *(volatile unsigned long*)(ADC_BLOCK + 0x0)       // ADC Control Register (write only)
  #define START                          0x00000002                                        // start conversion
#define ADC_MR                           *(unsigned long*)(ADC_BLOCK + 0x4)                // ADC Mode Register (read/write)
  #define TRGEN                          0x00000001                                        // hardware trigger enabled
  #define TRGSEL_EXT                     0x0000000c                                        // trigger on ADTRG rising edge
  #define TRGSEL_T0                      0x00000000                                        // TIOA from timer 0 acts as conversion trigger
  #define TRGSEL_T1                      0x00000002                                        // TIOA from timer 1 acts as conversion trigger
  #define TRGSEL_T2                      0x00000004                                        // TIOA from timer 2 acts as conversion trigger
  #define LOWRES                         0x00000010                                        // 8 bit mode rather than 10 bit mode
  #define SLEEP                          0x00000020                                        // SLEEP mode
  #define MEDIUM_ADC_CLOCK               (((MASTER_CLOCK / (2*500000)) - 1)<<8)            // 500kHz clock
  #define MEDIUM_STARTUP_TIME            (5 << 16)                                         // test value
  #define MEDIUM_SAMPLE_AND_HOLD         (5 << 24)                                         // test value
#define ADC_RES1                         *(unsigned long*)(ADC_BLOCK + 0x8)                // ADC reserved 1
#define ADC_RES2                         *(unsigned long*)(ADC_BLOCK + 0xc)                // ADC reserved 2
#define ADC_CHER                         *(volatile unsigned long*)(ADC_BLOCK + 0x10)      // ADC Channel Enable Register (write only)
  #define ADC_CHANNEL_0                  0x01
  #define ADC_CHANNEL_1                  0x02
  #define ADC_CHANNEL_2                  0x04
  #define ADC_CHANNEL_3                  0x08
  #define ADC_CHANNEL_4                  0x10
  #define ADC_CHANNEL_5                  0x20
  #define ADC_CHANNEL_6                  0x40
  #define ADC_CHANNEL_7                  0x80
#define ADC_CHDR                         *(volatile unsigned long*)(ADC_BLOCK + 0x14)      // ADC Channel Disable Register (write only)
#define ADC_CHSR                         *(volatile unsigned long*)(ADC_BLOCK + 0x18)      // ADC Channel Status Register (read only)
#define ADC_SR                           *(volatile unsigned long*)(ADC_BLOCK + 0x1c)      // ADC Status Register (read only)
  #define EOC0                           0x00000001                                        // End Of Conversion - channel 0
  #define EOC1                           0x00000002                                        // End Of Conversion - channel 1
  #define EOC2                           0x00000004                                        // End Of Conversion - channel 2
  #define EOC3                           0x00000008                                        // End Of Conversion - channel 3
  #define EOC4                           0x00000010                                        // End Of Conversion - channel 4
  #define EOC5                           0x00000020                                        // End Of Conversion - channel 5
  #define EOC6                           0x00000040                                        // End Of Conversion - channel 6
  #define EOC7                           0x00000080                                        // End Of Conversion - channel 7
  #define OVRE0                          0x00000100                                        // Overrun Error - channel 0
  #define OVRE1                          0x00000200                                        // Overrun Error - channel 1
  #define OVRE2                          0x00000400                                        // Overrun Error - channel 2
  #define OVRE3                          0x00000800                                        // Overrun Error - channel 3
  #define OVRE4                          0x00001000                                        // Overrun Error - channel 4
  #define OVRE5                          0x00002000                                        // Overrun Error - channel 5
  #define OVRE6                          0x00004000                                        // Overrun Error - channel 6
  #define OVRE7                          0x00008000                                        // Overrun Error - channel 7
  #define DRDY                           0x00010000                                        // Data Ready (since last read of ADC_LCDR)
  #define GOVRE                          0x00020000                                        // General Overrun Error (since last read of ADC_SR)
  #define ADC_ENDRX                      0x00040000                                        // End of Rx Buffer (Receive Counter Register has reached 0 since the last write to  ADC_RCR or ADC_RNCR)
  #define ADC_RXBUFF                     0x00080000                                        // Rx Buffer Full (both ADC_RCR and ADC_RNCR have a value of 0)
#define ADC_LCDR                         *(volatile unsigned long*)(ADC_BLOCK + 0x20)      // ADC Last Converted Data Register (read only)
#define ADC_IER                          *(volatile unsigned long*)(ADC_BLOCK + 0x24)      // ADC Interrupt Enable Register (write only)
#define ADC_IDR                          *(volatile unsigned long*)(ADC_BLOCK + 0x28)      // ADC Interrupt Disable Register (write only)
#define ADC_IMR                          *(volatile unsigned long*)(ADC_BLOCK + 0x2c)      // ADC Interrupt Mask Register (read only)
#define ADC_CDR0_ADD                     (volatile unsigned long*)(ADC_BLOCK + 0x30)
#define ADC_CDR0                         *(volatile unsigned long*)(ADC_BLOCK + 0x30)      // ADC Channel Data Register 0 (read only)
#define ADC_CDR1                         *(volatile unsigned long*)(ADC_BLOCK + 0x34)      // ADC Channel Data Register 1 (read only)
#define ADC_CDR2                         *(volatile unsigned long*)(ADC_BLOCK + 0x38)      // ADC Channel Data Register 2 (read only)
#define ADC_CDR3                         *(volatile unsigned long*)(ADC_BLOCK + 0x3c)      // ADC Channel Data Register 3 (read only)
#define ADC_CDR4                         *(volatile unsigned long*)(ADC_BLOCK + 0x40)      // ADC Channel Data Register 4 (read only)
#define ADC_CDR5                         *(volatile unsigned long*)(ADC_BLOCK + 0x44)      // ADC Channel Data Register 5 (read only)
#define ADC_CDR6                         *(volatile unsigned long*)(ADC_BLOCK + 0x48)      // ADC Channel Data Register 6 (read only)
#define ADC_CDR7                         *(volatile unsigned long*)(ADC_BLOCK + 0x4c)      // ADC Channel Data Register 7 (read only)


// Power Management
//
#ifdef _AT32UC3C                                                         // {10}
    #define PM_MCCTRL_SHORT_ADD          0
    #define PM_MCCTRL                    *(volatile unsigned long*)(PM_BLOCK + 0x0000)     // Main Clock Control (Read/Write)
      #define MCSEL_SLOW_CLOCK           0x00000000                                        // System RC oscillaror (RCSYS)
      #define MCSEL_OSC0                 0x00000001                                        // Oscillator 0 is the source of the main clock
      #define MCSEL_OSC1                 0x00000002                                        // Oscillator 1 is the source of the main clock
      #define MCSEL_PLL0                 0x00000003                                        // PLL0 is the source of the main clock
      #define MCSEL_PLL1                 0x00000004                                        // PLL1 is the source of the main clock
      #define MCSEL_RC8M                 0x00000005                                        // 8MHz RC oscillator
      #define MCSEL_RC120M               0x00000007                                        // 120MHz RC oscillator
    #define PM_CPUSEL_SHORT_ADD          0x04
    #define PM_CPUSEL                    *(unsigned long*)(PM_BLOCK + 0x0004)              // CPU Clock Select (Read/Write)
      #define PM_CLKSEL_CPUDIV           0x00000080
      #define CPU_DIV_2                  0x00000000                                        // CPU clock equals main clock/2
      #define CPU_DIV_4                  0x00000001                                        // CPU clock equals main clock/4
      #define CPU_DIV_8                  0x00000002                                        // CPU clock equals main clock/8
      #define CPU_DIV_16                 0x00000003                                        // CPU clock equals main clock/16
      #define CPU_DIV_32                 0x00000004                                        // CPU clock equals main clock/32
      #define CPU_DIV_64                 0x00000005                                        // CPU clock equals main clock/64
      #define CPU_DIV_128                0x00000006                                        // CPU clock equals main clock/128
      #define CPU_DIV_256                0x00000007                                        // CPU clock equals main clock/256
    #define PM_HSBSEL_SHORT_ADD          0x08
    #define PM_HSBSEL                    *(volatile unsigned long*)(PM_BLOCK + 0x0008)     // HSB Clock Select (Read-only)
      #define HSB_DIV_2                  0x00000000                                        // HSB clock equals main clock/2
      #define HSB_DIV_4                  0x00000001                                        // HSB clock equals main clock/4
      #define HSB_DIV_8                  0x00000002                                        // HSB clock equals main clock/8
      #define HSB_DIV_16                 0x00000003                                        // HSB clock equals main clock/16
      #define HSB_DIV_32                 0x00000004                                        // HSB clock equals main clock/32
      #define HSB_DIV_64                 0x00000005                                        // HSB clock equals main clock/64
      #define HSB_DIV_128                0x00000006                                        // HSB clock equals main clock/128
      #define HSB_DIV_256                0x00000007                                        // HSB clock equals main clock/256
    #define PM_PBASEL_SHORT_ADD          0x0c
    #define PM_PBASEL                    *(unsigned long*)(PM_BLOCK + 0x000c)              // PBA Clock Select (Read/Write)
      #define PBA_DIV_2                  0x00000000                                        // PBA clock equals main clock/2
      #define PBA_DIV_4                  0x00000001                                        // PBA clock equals main clock/4
      #define PBA_DIV_8                  0x00000002                                        // PBA clock equals main clock/8
      #define PBA_DIV_16                 0x00000003                                        // PBA clock equals main clock/16
      #define PBA_DIV_32                 0x00000004                                        // PBA clock equals main clock/32
      #define PBA_DIV_64                 0x00000005                                        // PBA clock equals main clock/64
      #define PBA_DIV_128                0x00000006                                        // PBA clock equals main clock/128
      #define PBA_DIV_256                0x00000007                                        // PBA clock equals main clock/256
    #define PM_PBBSEL_SHORT_ADD          0x10
    #define PM_PBBSEL                    *(unsigned long*)(PM_BLOCK + 0x0010)              // PBB Clock Select (Read/Write)
      #define PBB_DIV_2                  0x00000000                                        // PBB clock equals main clock/2
      #define PBB_DIV_4                  0x00000001                                        // PBB clock equals main clock/4
      #define PBB_DIV_8                  0x00000002                                        // PBB clock equals main clock/8
      #define PBB_DIV_16                 0x00000003                                        // PBB clock equals main clock/16
      #define PBB_DIV_32                 0x00000004                                        // PBB clock equals main clock/32
      #define PBB_DIV_64                 0x00000005                                        // PBB clock equals main clock/64
      #define PBB_DIV_128                0x00000006                                        // PBB clock equals main clock/128
      #define PBB_DIV_256                0x00000007                                        // PBB clock equals main clock/256
    #define PM_PBCSEL_SHORT_ADD          0x14
    #define PM_PBCSEL                    *(unsigned long*)(PM_BLOCK + 0x0014)              // PBC Clock Select (Read/Write)
      #define PBC_DIV_2                  0x00000000                                        // PBC clock equals main clock/2
      #define PBC_DIV_4                  0x00000001                                        // PBC clock equals main clock/4
      #define PBC_DIV_8                  0x00000002                                        // PBC clock equals main clock/8
      #define PBC_DIV_16                 0x00000003                                        // PBC clock equals main clock/16
      #define PBC_DIV_32                 0x00000004                                        // PBC clock equals main clock/32
      #define PBC_DIV_64                 0x00000005                                        // PBC clock equals main clock/64
      #define PBC_DIV_128                0x00000006                                        // PBC clock equals main clock/128
      #define PBC_DIV_256                0x00000007                                        // PBC clock equals main clock/256
    #define PM_CPUMASK                   *(unsigned long*)(PM_BLOCK + 0x0020)              // CPU Mask (Read/Write)
      #define CPUMASK_OCD                0x00000002
    #define PM_HSBMASK                   *(unsigned long*)(PM_BLOCK + 0x0024)              // HSB Mask (Read/Write)
      #define HSBMASK_SAU                0x00000001
      #define HSBMASK_PDCA               0x00000002
      #define HSBMASK_MDMA               0x00000004
      #define HSBMASK_USBC               0x00000008
      #define HSBMASK_CANIF              0x00000010
      #define HSBMASK_HFLASHC            0x00000020
      #define HSBMASK_PBA_BRIDGE         0x00000040
      #define HSBMASK_PBB_BRIDGE         0x00000080
      #define HSBMASK_PBC_BRIDGE         0x00000100
      #define HSBMASK_HSB_RAM            0x00000200
      #define HSBMASK_EBI                0x00000400
      #define HSBMASK_MACB               0x00000800
      #define HSBMASK_PEVC               0x00001000
    #define PM_PBAMASK                   *(unsigned long*)(PM_BLOCK + 0x0028)              // PBA Mask (Read/Write)
      #define PBAMASK_INTC               0x00000001
      #define PBAMASK_PM                 0x00000002
      #define PBAMASK_FCIF               0x00000004
      #define PBAMASK_AST                0x00000008
      #define PBAMASK_WDT                0x00000010
      #define PBAMASK_EIC                0x00000020
      #define PBAMASK_FREQM              0x00000040
      #define PBAMASK_GPIO               0x00000080
      #define PBAMASK_USART0             0x00000100
      #define PBAMASK_USART2             0x00000200
      #define PBAMASK_USART3             0x00000400
      #define PBAMASK_SPI1               0x00000800
      #define PBAMASK_TWIM0              0x00001000
      #define PBAMASK_TWIM1              0x00002000
      #define PBAMASK_TWIS0              0x00004000
      #define PBAMASK_TWIS1              0x00008000
      #define PBAMASK_IISC               0x00010000
      #define PBAMASK_PWM                0x00020000
      #define PBAMASK_QDEC0              0x00040000
      #define PBAMASK_QDEC1              0x00080000
      #define PBAMASK_TC1                0x00100000
      #define PBAMASK_PEVC               0x00200000
      #define PBAMASK_ACIFA0             0x00400000
      #define PBAMASK_ACIFA1             0x00800000
      #define PBAMASK_DACIFB0            0x01000000
      #define PBAMASK_DACIFB1            0x02000000
      #define PBAMASK_AW                 0x04000000
    #define PM_PBBMASK                   *(unsigned long*)(PM_BLOCK + 0x002c)              // PBB Mask (Read/Write)
      #define PBBMASK_FLASHC             0x00000001
      #define PBBMASK_USBC               0x00000002
      #define PBBMASK_HMATRIX            0x00000004
      #define PBBMASK_SAU                0x00000008
      #define PBBMASK_SMC                0x00000010
      #define PBBMASK_SDRAMC             0x00000020
      #define PBBMASK_MACB               0x00000040
    #define PM_PBCMASK                   *(unsigned long*)(PM_BLOCK + 0x0030)              // PBC Mask (Read/Write)
      #define PBCMASK_PDCA               0x00000001
      #define PBCMASK_MDMA               0x00000002
      #define PBCMASK_USART1             0x00000004
      #define PBCMASK_SPI0               0x00000008
      #define PBCMASK_CANIF              0x00000010
      #define PBCMASK_TC0                0x00000020
      #define PBCMASK_ADCIFA             0x00000040
      #define PBCMASK_USART4             0x00000080
      #define PBCMASK_TWIM2              0x00000100
      #define PBCMASK_TWIS2              0x00000200
    #define PM_PBADIVMASK                *(unsigned long*)(PM_BLOCK + 0x0040)              // PBA Mask
    #define PM_PBBDIVMASK                *(unsigned long*)(PM_BLOCK + 0x0044)              // PBB Mask
    #define PM_PBCDIVMASK                *(unsigned long*)(PM_BLOCK + 0x0048)              // PBC Mask
    #define PM_CFDCTRL                   *(unsigned long*)(PM_BLOCK + 0x0054)              // Clock Failure Detector Control (Read/Write)
    #define PM_UNLOCK                    *(volatile unsigned long*)(PM_BLOCK + 0x0058)     // Unlock Register (Write-only)
      #define PM_ADDRESS_MASK            0x0000003f
      #define PM_UNLOCK_KEY              0xaa000000
    #define PM_IER                       *(volatile unsigned long*)(PM_BLOCK + 0x00c0)     // PM Interrupt Enable Register (Write-only
    #define PM_IDR                       *(volatile unsigned long*)(PM_BLOCK + 0x00c4)     // PM Interrupt Disable Register (Write-only
    #define PM_IMR                       *(volatile unsigned long*)(PM_BLOCK + 0x00c8)     // PM Interrupt Mask Register (Read-only)
    #define PM_ISR                       *(volatile unsigned long*)(PM_BLOCK + 0x00cc)     // PM Interrupt Status Register (Read-only)
    #define PM_ICR                       *(volatile unsigned long*)(PM_BLOCK + 0x00d0)     // PM Interrupt Clear Register (Write-only)
    #define PM_SR                        *(volatile unsigned long*)(PM_BLOCK + 0x00d4)     // PM Status Register (Read-only)
      #define PM_SR_CFD                  0x00000001                                        // failure of main clock detected, main clock is now running from RC oscillator
      #define PM_SR_CKRDY                0x00000020                                        // the synchronous clocks have frequencies as indicated in the CKSEL register
      #define PM_SR_AE                   0x80000000                                        // write to lock protected register without unlocking it has occured
    #define PM_RCAUSE                    *(volatile unsigned long*)(PM_BLOCK + 0x0180)     // Reset Cause Register (Read-only)
      #define PM_RCAUSE_POR              0x00000001                                        // power on reset
      #define PM_RCAUSE_BOD              0x00000002                                        // brown-out reset
      #define PM_RCAUSE_EXT              0x00000004                                        // external pin reset
      #define PM_RCAUSE_WDT              0x00000008                                        // watchdog reset
      #define PM_RCAUSE_JTAG             0x00000010                                        // JTAG reset
      #define PM_RCAUSE_CPU_ERR          0x00000080                                        // cpu error reset
      #define PM_RCAUSE_OCD              0x00000100                                        // OCD reset
      #define PM_RCAUSE_JTAG_HARD        0x00000200                                        // JTAG reset or halt instruction reset
    #define PM_WCAUSE                    *(volatile unsigned long*)(PM_BLOCK + 0x0184)     // Wake Cause Register (Read-only)
    #define PM_AWEN                      *(unsigned long*)(PM_BLOCK + 0x0188)              // Asynchronous Wake Enable (Read/Write)
    #define PM_CONFIG                    *(volatile unsigned long*)(PM_BLOCK + 0x03f8)     // Reset Cause Register (Read-only)
    #define PM_VERSION                   *(volatile unsigned long*)(PM_BLOCK + 0x03fc)     // Reset Cause Register (Read-only)

    // System Control Interface
    //
    #define SCIF_IER                     *(volatile unsigned long*)(SCIF_BLOCK + 0x0000)   // SCIF Interrupt Enabled Register (Read/Write)
    #define SCIF_IDR                     *(volatile unsigned long*)(SCIF_BLOCK + 0x0004)   // SCIF Interrupt Disable Register (Read/Write)
    #define SCIF_IMR                     *(volatile unsigned long*)(SCIF_BLOCK + 0x0008)   // SCIF Interrupt Mask Register (Read-only)
    #define SCIF_ISR                     *(volatile unsigned long*)(SCIF_BLOCK + 0x000c)   // SCIF Interrupt Status Register (Read-only)
    #define SCIF_ICR                     *(volatile unsigned long*)(SCIF_BLOCK + 0x0010)   // SCIF Interrupt Clear Register (Write-only)
    #define SCIF_PCLKSR                  *(volatile unsigned long*)(SCIF_BLOCK + 0x0014)   // SCIF Power and Clocks Status Register (Read-only)
      #define SCIF_PCLKSR_OSC0RDY        0x00000001
      #define SCIF_PCLKSR_OSC1RDY        0x00000002
      #define SCIF_PCLKSR_OSC32RDY       0x00000004
      #define SCIF_PCLKSR_RCOSC8MRDY     0x00000008
      #define SCIF_PCLKSR_PLL0_LOCK      0x00000010
      #define SCIF_PCLKSR_PLL1_LOCK      0x00000020
      #define SCIF_PCLKSR_BODDET         0x00000040
      #define SCIF_PCLKSR_BOD33DET       0x00000080
      #define SCIF_PCLKSR_BOD50DET       0x00000100
      #define SCIF_PCLKSR_PLL0_LOCK_LOST 0x00000200
      #define SCIF_PCLKSR_PLL1_LOCK_LOST 0x00000400
    #define SCIF_UNLOCK                  *(volatile unsigned long*)(SCIF_BLOCK + 0x0018)   // SCIF Unlock Register (Write-only)
      #define SCIF_ADDRESS_MASK          0x0000003f
      #define SCIF_UNLOCK_KEY            0xaa000000
    #define SCIF_PLL0_SHORT_ADD          0x1c
    #define SCIF_PLL0                    *(unsigned long*)(SCIF_BLOCK + 0x001c)            // SCIF PLL0 Control Register (Read/Write)
      #define SCIF_PLL_PLLCOUNT_MASK     0x3f000000
      #define SCIF_PLL_PLLMUL_MASK       0x000f0000
      #define SCIF_PLL_PLLDIV_MASK       0x00000f00
      #define SCIF_PLL_PLLOPT_VCO_160_240MHZ              0x00000000
      #define SCIF_PLL_PLLOPT_VCO_80_180MHZ               0x00000008
      #define PLLOPTION_OUTPUT_DIV2      0x00000010
      #define SCIF_PLL_PLLOPT_WIDE_BANDWITH_MODE_ENABLED  0x00000000
      #define SCIF_PLL_PLLOPT_WIDE_BANDWITH_MODE_DISABLED 0x00000020
      #define SCIF_PLL_PLLOSC_0          0x00000000
      #define SCIF_PLL_PLLOSC_1          0x00000002
      #define SCIF_PLL_PLLOSC_RC         0x00000004
      #define SCIF_PLL_PLLEN             0x00000001
    #define SCIF_PLL1                    *(unsigned long*)(SCIF_BLOCK + 0x0020)            // SCIF PLL1 Control Register (Read/Write)
    #define SCIF_OSCCTRL0_SHORT_ADD      0x24
    #define SCIF_OSCCTRL0                *(unsigned long*)(SCIF_BLOCK + 0x0024)            // SCIF Oscillator0 Control Register (Read/Write)
      #define SCIF_OSCCTRL_MODE_EXTERN   0x00000000
      #define SCIF_OSCCTRL_MODE_CRYSTAL  0x00000001
      #define SCIF_OSCCTRL_GAIN_0        0x00000000
      #define SCIF_OSCCTRL_GAIN_1        0x00000002
      #define SCIF_OSCCTRL_GAIN_2        0x00000004
      #define SCIF_OSCCTRL_GAIN_3        0x00000006
      #define SCIF_OSCCTRL_AGC           0x00000008
      #define SCIF_OSCCTRL_STARTUP_0     0x00000000
      #define SCIF_OSCCTRL_STARTUP_35U   0x00000800
      #define SCIF_OSCCTRL_STARTUP_70U   0x00000900
      #define SCIF_OSCCTRL_STARTUP_140U  0x00000a00
      #define SCIF_OSCCTRL_STARTUP_280U  0x00000b00
      #define SCIF_OSCCTRL_STARTUP_560U  0x00000100
      #define SCIF_OSCCTRL_STARTUP_1M1   0x00000200
      #define SCIF_OSCCTRL_STARTUP_2M2   0x00000c00
      #define SCIF_OSCCTRL_STARTUP_4M4   0x00000d00
      #define SCIF_OSCCTRL_STARTUP_8M8   0x00000e00
      #define SCIF_OSCCTRL_STARTUP_18M   0x00000300
      #define SCIF_OSCCTRL_STARTUP_36M   0x00000400
      #define SCIF_OSCCTRL_STARTUP_71M   0x00000500
      #define SCIF_OSCCTRL_STARTUP_142M  0x00000600
      #define SCIF_OSCCTRL_STARTUP_285M  0x00000700
      #define SCIF_OSCCTRL_OSCEN         0x00010000
    #define SCIF_OSCCTRL1                *(unsigned long*)(SCIF_BLOCK + 0x0028)            // SCIF Oscillator1 Control Register (Read/Write)
    #define SCIF_BOD                     *(unsigned long*)(SCIF_BLOCK + 0x002c)            // SCIF 1.8V BOD Control Register (Read/Write)
    #define SCIF_BGCR                    *(unsigned long*)(SCIF_BLOCK + 0x0030)            // SCIF Bandgap Calibration Register (Read/Write)
    #define SCIF_BOD33                   *(unsigned long*)(SCIF_BLOCK + 0x0034)            // SCIF 3.3V BOD Control Register (Read/Write)
    #define SCIF_BOD50                   *(unsigned long*)(SCIF_BLOCK + 0x0038)            // SCIF 5V BOD Control Register (Read/Write)
    #define SCIF_VREGCR                  *(unsigned long*)(SCIF_BLOCK + 0x003c)            // SCIF Voltage Regulation Calibration Register (Read/Write)
    #define SCIF_VREGCTRL                *(unsigned long*)(SCIF_BLOCK + 0x0040)            // SCIF Voltage Regulation Control Register (Read/Write)
    #define SCIF_RCCR                    *(unsigned long*)(SCIF_BLOCK + 0x0044)            // SCIF RXSYSn Calibration Register (Read/Write)
    #define SCIF_RCCR8                   *(unsigned long*)(SCIF_BLOCK + 0x0048)            // SCIF 8MHz/1MHz RC Oscillator Control Register (Read/Write)
    #define SCIF_OSCCTRL32               *(unsigned long*)(SCIF_BLOCK + 0x004c)            // SCIF Oscillator 32 Control Register (Read/Write)
    #define SCIF_RC120MCR                *(unsigned long*)(SCIF_BLOCK + 0x0058)            // SCIF 120MHz RC Oscillator Control Register (Read/Write)
    #define SCIF_GPLP0                   *(unsigned long*)(SCIF_BLOCK + 0x005c)            // SCIF General Purpose Low Power Register 0 (Read/Write)
    #define SCIF_GPLP1                   *(unsigned long*)(SCIF_BLOCK + 0x0060)            // SCIF General Purpose Low Power Register 1 (Read/Write)
    #define SCIF_GCCTRL0                 *(unsigned long*)(SCIF_BLOCK + 0x0064)            // SCIF Generic Clock Control Register 0 (Read/Write)
    #define SCIF_GCCTRL1                 *(unsigned long*)(SCIF_BLOCK + 0x0068)            // SCIF Generic Clock Control Register 1 (Read/Write)
    #define SCIF_GCCTRL2                 *(unsigned long*)(SCIF_BLOCK + 0x006c)            // SCIF Generic Clock Control Register 2 (Read/Write)
    #define SCIF_GCCTRL4                 *(unsigned long*)(SCIF_BLOCK + 0x0074)            // SCIF Generic Clock Control Register 4 (Read/Write)
    #define SCIF_GCCTRL5                 *(unsigned long*)(SCIF_BLOCK + 0x0078)            // SCIF Generic Clock Control Register 5 (Read/Write)
    #define SCIF_GCCTRL6                 *(unsigned long*)(SCIF_BLOCK + 0x007c)            // SCIF Generic Clock Control Register 6 (Read/Write)
    #define SCIF_GCCTRL7                 *(unsigned long*)(SCIF_BLOCK + 0x0080)            // SCIF Generic Clock Control Register 7 (Read/Write)
    #define SCIF_GCCTRL8                 *(unsigned long*)(SCIF_BLOCK + 0x0084)            // SCIF Generic Clock Control Register 8 (Read/Write)
    #define SCIF_GCCTRL9                 *(unsigned long*)(SCIF_BLOCK + 0x0088)            // SCIF Generic Clock Control Register 9 (Read/Write)
    #define SCIF_GCCTRL10                *(unsigned long*)(SCIF_BLOCK + 0x008c)            // SCIF Generic Clock Control Register 10 (Read/Write)
    #define SCIF_GCCTRL11                *(unsigned long*)(SCIF_BLOCK + 0x0090)            // SCIF Generic Clock Control Register 11 (Read/Write)

    #define PM_GEN_USB_CTRL              SCIF_GCCTRL0

    #define SCIF_PLLVERSION              *(volatile unsigned long*)(SCIF_BLOCK + 0x03c8)   // SCIF PLL Interface Version Register (Read-only)
    #define SCIF_OSCVERSION              *(volatile unsigned long*)(SCIF_BLOCK + 0x03cc)   // SCIF Oscillator 0/1 Interface Version Register (Read-only)
    #define SCIF_BODVERSION              *(volatile unsigned long*)(SCIF_BLOCK + 0x03d0)   // SCIF 1.8V BOD Interface Version Register (Read-only)
    #define SCIF_BODBVERSION             *(volatile unsigned long*)(SCIF_BLOCK + 0x03d4)   // SCIF 3.3/5.9V BOD Interface Version Register (Read-only)
    #define SCIF_VREGVERSION             *(volatile unsigned long*)(SCIF_BLOCK + 0x03d8)   // SCIF Voltage Regulator Interface Version Register (Read-only)
    #define SCIF_RCCRVERSION             *(volatile unsigned long*)(SCIF_BLOCK + 0x03dc)   // SCIF RCSYS Interface Version Register (Read-only)
    #define SCIF_RCCR8VERSION            *(volatile unsigned long*)(SCIF_BLOCK + 0x03e0)   // SCIF 8MHz/1MHz RCOSC Interface Version Register (Read-only)
    #define SCIF_OSC32VERSION            *(volatile unsigned long*)(SCIF_BLOCK + 0x03e4)   // SCIF 32kHz Oscillator Interface Version Register (Read-only)
    #define SCIF_RC120MVERSION           *(volatile unsigned long*)(SCIF_BLOCK + 0x03f0)   // SCIF 120MHz RC Oscillator Interface Version Register (Read-only)
    #define SCIF_GPLPVERSION             *(volatile unsigned long*)(SCIF_BLOCK + 0x03f4)   // SCIF GPLP Version Register (Read-only)
    #define SCIF_GCLKVERSION             *(volatile unsigned long*)(SCIF_BLOCK + 0x03f8)   // SCIF Generic Clock Version Register (Read-only)
    #define SCIF_VERSION                 *(volatile unsigned long*)(SCIF_BLOCK + 0x03fc)   // SCIF SCIF Version Register (Read-only)
#else
    // _AT32UC3A and _AT32UC3B
    #define PM_MCCTRL                    *(volatile unsigned long*)(PM_BLOCK + 0x0000)     // Main Clock Control (Read/Write)
      #define MCSEL_SLOW_CLOCK           0x00000000                                        // slow clock is the source of the main clock
      #define MCSEL_OSC0                 0x00000001                                        // Oscillator 0 is the source of the main clock
      #define MCSEL_PLL0                 0x00000002                                        // PLL0 is the source of the main clock
      #define OSC0EN                     0x00000004                                        // oscillator 0 is enabled
      #define OSC1EN                     0x00000008                                        // oscillator 1 is enabled
    #define PM_CKSEL                     *(unsigned long*)(PM_BLOCK + 0x0004)              // Clock Select (Read/Write)
      #define PBB_MAIN_CLOCK             0x00000000                                        // PBB clock equals main clock
      #define PBB_DIV_2                  0x80000000                                        // PBB clock equals main clock/2
      #define PBB_DIV_4                  0x81000000                                        // PBB clock equals main clock/4
      #define PBB_DIV_8                  0x82000000                                        // PBB clock equals main clock/8
      #define PBB_DIV_16                 0x83000000                                        // PBB clock equals main clock/16
      #define PBB_DIV_32                 0x84000000                                        // PBB clock equals main clock/32
      #define PBB_DIV_64                 0x85000000                                        // PBB clock equals main clock/64
      #define PBB_DIV_128                0x86000000                                        // PBB clock equals main clock/128
      #define PBB_DIV_256                0x87000000                                        // PBB clock equals main clock/256
      #define PBA_MAIN_CLOCK             0x00000000                                        // PBA clock equals main clock
      #define PBA_DIV_2                  0x00800000                                        // PBA clock equals main clock/2
      #define PBA_DIV_4                  0x00810000                                        // PBA clock equals main clock/4
      #define PBA_DIV_8                  0x00820000                                        // PBA clock equals main clock/8
      #define PBA_DIV_16                 0x00830000                                        // PBA clock equals main clock/16
      #define PBA_DIV_32                 0x00840000                                        // PBA clock equals main clock/32
      #define PBA_DIV_64                 0x00850000                                        // PBA clock equals main clock/64
      #define PBA_DIV_128                0x00860000                                        // PBA clock equals main clock/128
      #define PBA_DIV_256                0x00870000                                        // PBA clock equals main clock/256
      #define HSB_MAIN_CLOCK             0x00000000                                        // HSB clock equals main clock [HSB clock is equal to CPU clock on AT32UC3A]
      #define HSB_DIV_2                  0x00008000                                        // HSB clock equals main clock/2
      #define HSB_DIV_4                  0x00008100                                        // HSB clock equals main clock/4
      #define HSB_DIV_8                  0x00008200                                        // HSB clock equals main clock/8
      #define HSB_DIV_16                 0x00008300                                        // HSB clock equals main clock/16
      #define HSB_DIV_32                 0x00008400                                        // HSB clock equals main clock/32
      #define HSB_DIV_64                 0x00008500                                        // HSB clock equals main clock/64
      #define HSB_DIV_128                0x00008600                                        // HSB clock equals main clock/128
      #define HSB_DIV_256                0x00008700                                        // HSB clock equals main clock/256
      #define CPU_MAIN_CLOCK             0x00000000                                        // CPU clock equals main clock
      #define CPU_DIV_2                  0x00000080                                        // CPU clock equals main clock/2
      #define CPU_DIV_4                  0x00000081                                        // CPU clock equals main clock/4
      #define CPU_DIV_8                  0x00000082                                        // CPU clock equals main clock/8
      #define CPU_DIV_16                 0x00000083                                        // CPU clock equals main clock/16
      #define CPU_DIV_32                 0x00000084                                        // CPU clock equals main clock/32
      #define CPU_DIV_64                 0x00000085                                        // CPU clock equals main clock/64
      #define CPU_DIV_128                0x00000086                                        // CPU clock equals main clock/128
      #define CPU_DIV_256                0x00000087                                        // CPU clock equals main clock/256
    #define PM_CPUMASK                   *(unsigned long*)(PM_BLOCK + 0x0008)              // CPU Mask (Read/Write)
      #define CPUMASK_OCD                0x00000002
      #define CPUMASK_SYSTIMER           0x00010000
    #define PM_HSBMASK                   *(unsigned long*)(PM_BLOCK + 0x000c)              // HSB Mask (Read/Write)
      #define HSBMASK_FLASHC             0x00000001
      #define HSBMASK_PBA_BRIDGE         0x00000002
      #define HSBMASK_PBB_BRIDGE         0x00000004
      #define HSBMASK_USBB               0x00000008
        #ifdef _AT32UC3B                                                 // {1}
      #define HSBMASK_PDCA               0x00000010
        #else
      #define HSBMASK_MACB               0x00000010
      #define HSBMASK_PDCA               0x00000020
      #define HSBMASK_EBI                0x00000040
        #endif
    #define PM_PBAMASK                   *(unsigned long*)(PM_BLOCK + 0x0010)              // PBA Mask (Read/Write)
      #define PBAMASK_INTC               0x00000001
      #define PBAMASK_GPIO               0x00000002
      #define PBAMASK_PDCA               0x00000004
      #define PBAMASK_RM_RTC_EIC         0x00000008
      #define PBAMASK_ADC                0x00000010
      #define PBAMASK_SPI0               0x00000020
        #ifdef _AT32UC3B                                                 // {1}
      #define PBAMASK_TWI                0x00000040
      #define PBAMASK_USART0             0x00000080
      #define PBAMASK_USART1             0x00000100
      #define PBAMASK_USART2             0x00000200
      #define PBAMASK_PWM                0x00000400
      #define PBAMASK_SSC                0x00000800
      #define PBAMASK_TC                 0x00001000
      #define PBAMASK_SPI1               0                               // some dummy bits for compatibility
      #define PBAMASK_ABDAC              0
      #define HSBMASK_EBI                0
      #define PBAMASK_USART3             0
      #define HSBMASK_MACB               0
        #else
      #define PBAMASK_SPI0               0x00000020
      #define PBAMASK_SPI1               0x00000040
      #define PBAMASK_TWI                0x00000080
      #define PBAMASK_USART0             0x00000100
      #define PBAMASK_USART1             0x00000200
      #define PBAMASK_USART2             0x00000400
      #define PBAMASK_USART3             0x00000800
      #define PBAMASK_PWM                0x00001000
      #define PBAMASK_SSC                0x00002000
      #define PBAMASK_TC                 0x00004000
      #define PBAMASK_ABDAC              0x00008000
        #endif
    #define PM_PBBMASK                   *(unsigned long*)(PM_BLOCK + 0x0014)              // PBB Mask (Read/Write)
      #define PBBMASK_HMATRIX            0x00000001
      #define PBBMASK_USBB               0x00000002
      #define PBBMASK_FLASHC             0x00000004
      #define PBBMASK_MACB               0x00000008
      #define PBBMASK_SMC                0x00000010
      #define PBBMASK_SDRAMC             0x00000020
    #define PM_PLL0                      *(unsigned long*)(PM_BLOCK + 0x0020)              // PLL0 Control (Read/Write)
      #define PLLEN                      0x00000001
      #define PLLOSC_SOURCE_0            0x00000000
      #define PLLOSC_SOURCE_1            0x00000002
      #define PLLOPTION_RANGE_160_240MHZ 0x00000000
      #define PLLOPTION_RANGE_80_180MHZ  0x00000004
      #define PLLOPTION_OUTPUT_DIV2      0x00000008
      #define PLLOPTION_WIDE_BANDWITH_MODE 0x00000010
    #define PM_PLL1                      *(unsigned long*)(PM_BLOCK + 0x0024)              // PLL1 Control (Read/Write)
    #define PM_OSCCTRL0                  *(unsigned long*)(PM_BLOCK + 0x0028)              // Oscillator 0 Control Register (Read/Write)
    #define PM_OSCCTRL1                  *(unsigned long*)(PM_BLOCK + 0x002c)              // Oscillator 1 Control Register (Read/Write)
      #define OSCILLATOR_RANGE_400_900KHZ  0x00000004
      #define OSCILLATOR_RANGE_900_3000KHZ 0x00000005
      #define OSCILLATOR_RANGE_3_8MHZ      0x00000006
      #define OSCILLATOR_RANGE_ABOVE_8MHZ  0x00000007
      #define STARTUP_0                  0x00000000
      #define STARTUP_560US              0x00000100
      #define STARTUP_1_1MS              0x00000200
      #define STARTUP_18MS               0x00000300
      #define STARTUP_36MS               0x00000400
      #define STARTUP_71MS               0x00000500
      #define STARTUP_142MS              0x00000600
    #define PM_OSCCTRL32                 *(unsigned long*)(PM_BLOCK + 0x0030)              // Oscillator 32 Control Register (Read/Write)
    #define PM_IER                       *(volatile unsigned long*)(PM_BLOCK + 0x0040)     // PM Interrupt Enable Register (Write-only)
    #define PM_IDR                       *(volatile unsigned long*)(PM_BLOCK + 0x0044)     // PM Interrupt Disable Register (Write-only)
    #define PM_IMR                       *(volatile unsigned long*)(PM_BLOCK + 0x0048)     // PM Interrupt Mask Register (Read-only)
    #define PM_ISR                       *(volatile unsigned long*)(PM_BLOCK + 0x004c)     // PM Interrupt Status Register (Read-only)
    #define PM_ICR                       *(volatile unsigned long*)(PM_BLOCK + 0x0050)     // PM Interrupt Clear Register (Write-only)
    #define PM_POSCSR                    *(volatile unsigned long*)(PM_BLOCK + 0x0054)     // Power and Oscillators Status Register (Read/Write)
      #define LOCK0                      0x00000001
      #define LOCK1                      0x00000002
      #define CKRDY                      0x00000020
      #define MSKRDY                     0x00000040
      #define OSC0RDY                    0x00000080
      #define OSC1RDY                    0x00000100
      #define OSC32RDY                   0x00000200
      #define BODDET                     0x00010000
    #define PM_GCCTRL0                   *(unsigned long*)(PM_BLOCK + 0x0060)              // Generic Clock Control 0 (Read/Write)
      #define PM_GCCTRL_OSCSEL           0x00000001                                        // select generic clock source 1
      #define PM_GCCTRL_PLLSEL           0x00000002                                        // select PLL rather than oscillator
      #define PM_GCCTRL_CEN              0x00000004                                        // clock running
      #define PM_GCCTRL_DIVEN            0x00000010                                        // divider enabled
      #define PM_GCCTRL_DIVMASK          0x00000f00                                        // mask for divider value
      #define PM_GCCTRL_DIVSHIFT         8                                                 // divider value shift to position
    #define PM_GCCTRL1                   *(unsigned long*)(PM_BLOCK + 0x0064)              // Generic Clock Control 1 (Read/Write)
    #define PM_GCCTRL2                   *(unsigned long*)(PM_BLOCK + 0x0068)              // Generic Clock Control 2 (Read/Write)
    #define PM_GCCTRL3                   *(unsigned long*)(PM_BLOCK + 0x006c)              // Generic Clock Control 3 (Read/Write)
    #if !defined _AT32UC3B
        #define PM_GCCTRL4               *(unsigned long*)(PM_BLOCK + 0x0070)              // Generic Clock Control 4 (Read/Write)
        #define PM_GCCTRL5               *(unsigned long*)(PM_BLOCK + 0x0074)              // Generic Clock Control 5 (Read/Write)
        #define PM_GEN_USB_CTRL          PM_GCCTRL4
    #else
        #define PM_GEN_USB_CTRL          PM_GCCTRL3
    #endif
    #define PM_RCCR                      *(unsigned long*)(PM_BLOCK + 0x00c0)              // RC Oscillator Calibration Register (Read/Write)
    #define PM_BGCR                      *(unsigned long*)(PM_BLOCK + 0x00c4)              // Bandgap Calibration Register (Read/Write)
    #define PM_VREGCR                    *(unsigned long*)(PM_BLOCK + 0x00c8)              // Linear Regulator Calibration Register (Read/Write)
    #define PM_BOD                       *(unsigned long*)(PM_BLOCK + 0x00d0)              // Brown-Out-Detector Level Register (Read/Write)
    #define PM_RCAUSE                    *(volatile unsigned long*)(PM_BLOCK + 0x0140)     // Reset Cause Register (Read-only)
      #define PM_RCAUSE_POR              0x00000001                                        // power on reset
      #define PM_RCAUSE_BOD              0x00000002                                        // brown-out reset
      #define PM_RCAUSE_EXT              0x00000004                                        // external pin reset
      #define PM_RCAUSE_WDT              0x00000008                                        // watchdog reset
      #define PM_RCAUSE_JTAG             0x00000010                                        // JTAG reset
      #define PM_RCAUSE_CPU_ERR          0x00000080                                        // cpu error reset
      #define PM_RCAUSE_OCD              0x00000100                                        // OCD reset
      #define PM_RCAUSE_JTAG_HARD        0x00000200                                        // JTAG reset or halt instruction reset
    #define PM_GPLP0                     *(unsigned long*)(PM_BLOCK + 0x0200)              // General Purpose Low-Power Register 0 (Read/Write)
    #define PM_GPLP1                     *(unsigned long*)(PM_BLOCK + 0x0204)              // General Purpose Low-Power Register 1 (Read/Write)


    // RTC                                                               {16}
    //
    #define RTC_CTRL                     *(volatile unsigned long*)(RTC_BLOCK + 0x00)      // RTC control (Read/Write)
      #define RTC_CTRL_EN                0x00000001
      #define RTC_CTRL_PCLR              0x00000002
      #define RTC_CTRL_WAKE_EN           0x00000004
      #define RTC_CTRL_CLK32             0x00000008
      #define RTC_CTRL_BUSY              0x00000010
      #define RTC_CTRL_PSEL_MASK         0x00000f00
      #define RTC_CTRL_CLKEN             0x00010000
    #define RTC_VAL                      *(volatile unsigned long*)(RTC_BLOCK + 0x04)      // RTC value (Read/Write)
    #define RTC_TOP                      *(unsigned long*)(RTC_BLOCK + 0x08)               // RTC top (Read/Write)
    #define RTC_IER                      *(volatile unsigned long*)(RTC_BLOCK + 0x10)      // RTC interrupt enable (Write-only)
    #define RTC_IDR                      *(volatile unsigned long*)(RTC_BLOCK + 0x14)      // RTC interrupt disable (Write-only)
    #define RTC_IMR                      *(volatile unsigned long*)(RTC_BLOCK + 0x18)      // RTC interrupt mask (Read-only)
    #define RTC_ISR                      *(volatile unsigned long*)(RTC_BLOCK + 0x1c)      // RTC interrupt status (Read-only)
    #define RTC_ICR                      *(volatile unsigned long*)(RTC_BLOCK + 0x20)      // RTC interrupt clear (Write-only)
      #define RTC_TOPI                   0x00000001
#endif


// Watchdog Timer
//
#define WDT_CTRL                         *(volatile unsigned long*)(WDT_BLOCK + 0x0)       // Watchdog Timer Control
  #define WATCHDOG_KEY_1                 0x55000000                                        // Watchdog write key - part 1
  #define WATCHDOG_KEY_2                 0xaa000000                                        // Watchdog write key - part 2
  #define WATCHDOG_PRE_MASK              0x00001f00
  #define WATCHDOG_PRE_SHIFT             8
#if defined _AT32UC3C                                                                      // {20}
  #define WATCHDOG_DAR                   0x00000002                                        // disable after reset
  #define WATCHDOG_WINDOW_MODE           0x00000004                                        // window mode of operation
  #define WATCHDOG_SFV                   0x00000008                                        // store final value (lock further writes)
  #define WATCHDOG_FCD                   0x00000080                                        // flash calibration done
  #define WATCHDOG_CEN                   0x00010000                                        // WDT clock enable
  #define WATCHDOG_CSSEL                 0x00020000                                        // select 32kHz as source
  #define WATCHDOG_TBAN                  0x007c0000                                        // time ban prescale select
  #define WATCHDOG_ENABLE                (0x00000001 | WATCHDOG_CEN | WATCHDOG_DAR)        // Watchdog enable (plus clock enable for compatibility)
#else
  #define WATCHDOG_ENABLE                0x00000001                                        // Watchdog enable
#endif
#define WDT_CLR                          *(volatile unsigned long*)(WDT_BLOCK + 0x4)       // Watchdog Timer Clear (write-only)
#if defined _AT32UC3C                                                                      // {20}
  #define WDT_CLR_WDTCLR                 0x00000001                                        // Watchdog clear
#endif


// External Interrupt Controller
//
#define EIC_IER                          *(volatile unsigned long*)(EIC_BLOCK + 0x00)      // Interrupt Enable Register (write-only)
#define EIC_IDR                          *(volatile unsigned long*)(EIC_BLOCK + 0x04)      // Interrupt Disable Register (write-only)
#define EIC_IMR                          *(volatile unsigned long*)(EIC_BLOCK + 0x08)      // Interrupt Mask Register (read-only)
#define EIC_ISR                          *(volatile unsigned long*)(EIC_BLOCK + 0x0c)      // Interrupt Status Register (read-only)
#define EIC_ICR                          *(volatile unsigned long*)(EIC_BLOCK + 0x10)      // Interrupt Clear Register (write-only)
#define EIC_MODE                         *(unsigned long*)(EIC_BLOCK + 0x14)               // Mode Register
#define EIC_EDGE                         *(unsigned long*)(EIC_BLOCK + 0x18)               // Edge Register
#define EIC_LEVEL                        *(unsigned long*)(EIC_BLOCK + 0x1c)               // Level Register
#define EIC_FILTER                       *(unsigned long*)(EIC_BLOCK + 0x20)               // Filter Register
  #define EIC_INT0                       0x00000001
  #define EIC_INT1                       0x00000002
  #define EIC_INT2                       0x00000004
  #define EIC_INT3                       0x00000008
  #define EIC_INT4                       0x00000010
  #define EIC_INT5                       0x00000020
  #define EIC_INT6                       0x00000040
  #define EIC_INT7                       0x00000080
  #define EIC_NMI                        0x00000100
#define EIC_TEST                         *(unsigned long*)(EIC_BLOCK + 0x24)               // Test Register
  #define EIC_TEST_EN                    0x80000000
#define EIC_ASYNC                         *(unsigned long*)(EIC_BLOCK + 0x28)              // External Interrupt Asynchronous
#define EIC_SCAN                         *(unsigned long*)(EIC_BLOCK + 0x2c)               // External Interrupt Scan
#define EIC_EN                           *(volatile unsigned long*)(EIC_BLOCK + 0x30)      // External Interrupt Enable (write-only)
#define EIC_DIS                          *(volatile unsigned long*)(EIC_BLOCK + 0x34)      // External Interrupt Disable (write-only)
#define EIC_CTRL                         *(unsigned long*)(EIC_BLOCK + 0x38)               // External Interrupt Control

// GPIO
//
#ifdef _AT32UC3C                                                         // {10]
    // Port 0 (A)
    //
    #define GPER_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0000)   // GPIO Enable Register (Read/Write) - Port 0
    #define GPERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0004)   // GPIO Register Set (Write only) - Port 0
    #define GPERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0008)   // GPIO Enable Register Register Clear (Write only) - Port 0
    #define GPERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x000c)   // GPIO Enable Register Toggle (Write only) - Port 0

    #define PMR0_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0010)   // Peripheral Mux Register 0 (Read/Write) - Port 0
    #define PMR0S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0014)   // Peripheral Mux Register 0 Set (Write only) - Port 0
    #define PMR0C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0018)   // Peripheral Mux Register 0 Clear (Write only) - Port 0
    #define PMR0T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x001c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 0

    #define PMR1_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0020)   // Peripheral Mux Register 1 (Read/Write) - Port 0
    #define PMR1S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0024)   // Peripheral Mux Register 1 Set (Write only) - Port 0
    #define PMR1C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0028)   // Peripheral Mux Register 1 Clear (Write only) - Port 0
    #define PMR1T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x002c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 0

    #define PMR2_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0030)   // Peripheral Mux Register 2 (Read/Write) - Port 0
    #define PMR2S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0034)   // Peripheral Mux Register 2 Set (Write only) - Port 0
    #define PMR2C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0038)   // Peripheral Mux Register 2 Clear (Write only) - Port 0
    #define PMR2T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x003c)   // Peripheral Mux Register 2 Toggle (Write only) - Port 0

    #define ODER_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0040)   // Output Driver Enable Register (Read/Write) - Port 0
    #define ODERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0044)   // Output Driver Enable Register Set (Write only) - Port 0
    #define ODERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0048)   // Output Driver Enable Register Clear (Write only) - Port 0
    #define ODERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x004c)   // Output Driver Enable Register Toggle (Write only) - Port 0

    #define OVR_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0050)   // Output Value Register (Read/Write) - Port 0
    #define OVRS_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0054)   // Output Value Register Set (Write only) - Port 0
    #define OVRC_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0058)   // Output Value Register Clear (Write only) - Port 0
    #define OVRT_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x005c)   // Output Value Register Toggle (Write only) - Port 0

    #define PVR_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0060)   // Pin Value Register (Read only) - Port 0

    #define PUER_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0070)   // Pull-up Enable Register (Read/Write) - Port 0
    #define PUERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0074)   // Pull-up Enable Register Set (Write only) - Port 0
    #define PUERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0078)   // Pull-up Enable Register Clear (Write only) - Port 0
    #define PUERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x007c)   // Pull-up Enable Register Toggle (Write only) - Port 0

    #define PDER_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0080)   // {17} Pull-down Enable Register (Read/Write) - Port 0
    #define PDERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0084)   // Pull-down Enable Register Set (Write only) - Port 0
    #define PDERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0088)   // Pull-down Enable Register Clear (Write only) - Port 0
    #define PDERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x008c)   // Pull-down Enable Register Toggle (Write only) - Port 0

    #define IER_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0090)   // Interrupt Enable Register 0 (Read/Write) - Port 0
    #define IERS_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0094)   // Interrupt Enable Register 0 Set (Write only) - Port 0
    #define IERC_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0098)   // Interrupt Enable Register 0 Clear (Write only) - Port 0
    #define IERT_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x009c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 0

    #define IMR0_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x00a0)   // Interrupt Mode Register 0 (Read/Write) - Port 0
    #define IMR0S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00a4)   // Interrupt Mode Register 0 Set (Write only) - Port 0
    #define IMR0C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 0
    #define IMR0T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 0

    #define IMR1_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x00b0)   // Interrupt Mode Register 1 (Read/Write) - Port 0
    #define IMR1S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00b4)   // Interrupt Mode Register 1 Set (Write only) - Port 0
    #define IMR1C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 0
    #define IMR1T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 0

    #define GFER_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x00c0)   // Glitch Filter Enable Register (Read/Write) - Port 0
    #define GFERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00c4)   // Glitch Filter Enable Register Set (Write only) - Port 0
    #define GFERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00c8)   // Glitch Filter Enable Register Clear (Write only) - Port 0
    #define GFERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 0

    #define IFR_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x00d0)   // Interrupt Flag Register (Read only) - Port 0
    #define IFRC_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x00d8)   // Interrupt Flag Register (Write only) - Port 0

    #define ODCR0_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0100)   // Output Driving Capability Register 0 (Read/Write) - Port 0
    #define ODCR0S_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0104)   // Output Driving Capability Register 0 Set (Write only) - Port 0
    #define ODCR0C_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0108)   // Output Driving Capability Register 0 Clear (Write only) - Port 0
    #define ODCR0T_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x010c)   // Output Driving Capability Register 0 Toggle (Write only) - Port 0

    #define ODCR1_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0110)   // Output Driving Capability Register 1 (Read/Write) - Port 0
    #define ODCR1S_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0114)   // Output Driving Capability Register 1 Set (Write only) - Port 0
    #define ODCR1C_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0118)   // Output Driving Capability Register 1 Clear (Write only) - Port 0
    #define ODCR1T_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x011c)   // Output Driving Capability Register 1 Toggle (Write only) - Port 0

    #define LOCK_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x01a0)   // Lock Register (Read/Write) - Port 0
    #define LOCKS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01a4)   // Lock Register Set (Write only) - Port 0
    #define LOCKC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01a8)   // Lock Register Clear (Write only) - Port 0
    #define LOCKT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01ac)   // Lock Register Toggle (Write only) - Port 0

    #define UNLOCKS_0                    *(volatile unsigned long*)(GPIO_BLOCK + 0x01e0)   // Inlock Register (Write only) - Port 0
    #define ASR_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x01e4)   // Access Status Register (Read/Write) - Port 0
    #define PARAMETER_0                  *(volatile unsigned long*)(GPIO_BLOCK + 0x01f8)   // Parameter Register (Read only) - Port 0
    #define VERSION_0                    *(volatile unsigned long*)(GPIO_BLOCK + 0x01fc)   // Version Register (Read only) - Port 0

    // Port 1 (B)
    //
    #define GPER_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0200)   // GPIO Enable Register (Read/Write) - Port 1
    #define GPERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0204)   // GPIO Register Set (Write only) - Port 1
    #define GPERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0208)   // GPIO Enable Register Register Clear (Write only) - Port 1
    #define GPERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x020c)   // GPIO Enable Register Toggle (Write only) - Port 1

    #define PMR0_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0210)   // Peripheral Mux Register 0 (Read/Write) - Port 1
    #define PMR0S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0214)   // Peripheral Mux Register 0 Set (Write only) - Port 1
    #define PMR0C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0218)   // Peripheral Mux Register 0 Clear (Write only) - Port 1
    #define PMR0T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x021c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 1

    #define PMR1_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0220)   // Peripheral Mux Register 1 (Read/Write) - Port 1
    #define PMR1S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0224)   // Peripheral Mux Register 1 Set (Write only) - Port 1
    #define PMR1C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0228)   // Peripheral Mux Register 1 Clear (Write only) - Port 1
    #define PMR1T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x022c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 1

    #define PMR2_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0230)   // Peripheral Mux Register 2 (Read/Write) - Port 1
    #define PMR2S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0234)   // Peripheral Mux Register 2 Set (Write only) - Port 1
    #define PMR2C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0238)   // Peripheral Mux Register 2 Clear (Write only) - Port 1
    #define PMR2T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x023c)   // Peripheral Mux Register 2 Toggle (Write only) - Port 1

    #define ODER_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0240)   // Output Driver Enable Register (Read/Write) - Port 1
    #define ODERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0244)   // Output Driver Enable Register Set (Write only) - Port 1
    #define ODERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0248)   // Output Driver Enable Register Clear (Write only) - Port 1
    #define ODERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x024c)   // Output Driver Enable Register Toggle (Write only) - Port 1

    #define OVR_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0250)   // Output Value Register (Read/Write) - Port 1
    #define OVRS_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0254)   // Output Value Register Set (Write only) - Port 1
    #define OVRC_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0258)   // Output Value Register Clear (Write only) - Port 1
    #define OVRT_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x025c)   // Output Value Register Toggle (Write only) - Port 1

    #define PVR_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0260)   // Pin Value Register (Read only) - Port 1

    #define PUER_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0270)   // Pull-up Enable Register (Read/Write) - Port 1
    #define PUERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0274)   // Pull-up Enable Register Set (Write only) - Port 1
    #define PUERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0278)   // Pull-up Enable Register Clear (Write only) - Port 1
    #define PUERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x027c)   // Pull-up Enable Register Toggle (Write only) - Port 1

    #define PDER_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0280)   // {17} Pull-down Enable Register (Read/Write) - Port 1
    #define PDERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0284)   // Pull-down Enable Register Set (Write only) - Port 1
    #define PDERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0288)   // Pull-down Enable Register Clear (Write only) - Port 1
    #define PDERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x028c)   // Pull-down Enable Register Toggle (Write only) - Port 1

    #define IER_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0290)   // Interrupt Enable Register 0 (Read/Write) - Port 1
    #define IERS_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0294)   // Interrupt Enable Register 0 Set (Write only) - Port 1
    #define IERC_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0298)   // Interrupt Enable Register 0 Clear (Write only) - Port 1
    #define IERT_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x029c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 1

    #define IMR0_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x02a0)   // Interrupt Mode Register 0 (Read/Write) - Port 1
    #define IMR0S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02a4)   // Interrupt Mode Register 0 Set (Write only) - Port 1
    #define IMR0C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 1
    #define IMR0T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 1

    #define IMR1_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x02b0)   // Interrupt Mode Register 1 (Read/Write) - Port 1
    #define IMR1S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02b4)   // Interrupt Mode Register 1 Set (Write only) - Port 1
    #define IMR1C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 1
    #define IMR1T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 1

    #define GFER_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x02c0)   // Glitch Filter Enable Register (Read/Write) - Port 1
    #define GFERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02c4)   // Glitch Filter Enable Register Set (Write only) - Port 1
    #define GFERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02c8)   // Glitch Filter Enable Register Clear (Write only) - Port 1
    #define GFERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 1

    #define IFR_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x02d0)   // Interrupt Flag Register (Read only) - Port 1
    #define IFRC_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x02d8)   // Interrupt Flag Register (Write only) - Port 1

    #define ODCR0_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0300)   // Output Driving Capability Register 0 (Read/Write) - Port 1
    #define ODCR0S_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0304)   // Output Driving Capability Register 0 Set (Write only) - Port 1
    #define ODCR0C_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0308)   // Output Driving Capability Register 0 Clear (Write only) - Port 1
    #define ODCR0T_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x030c)   // Output Driving Capability Register 0 Toggle (Write only) - Port 1

    #define ODCR1_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0310)   // Output Driving Capability Register 1 (Read/Write) - Port 1
    #define ODCR1S_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0314)   // Output Driving Capability Register 1 Set (Write only) - Port 1
    #define ODCR1C_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0318)   // Output Driving Capability Register 1 Clear (Write only) - Port 1
    #define ODCR1T_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x031c)   // Output Driving Capability Register 1 Toggle (Write only) - Port 1

    #define LOCK_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x03a0)   // Lock Register (Read/Write) - Port 1
    #define LOCKS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03a4)   // Lock Register Set (Write only) - Port 1
    #define LOCKC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03a8)   // Lock Register Clear (Write only) - Port 1
    #define LOCKT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03ac)   // Lock Register Toggle (Write only) - Port 1

    #define UNLOCKS_1                    *(volatile unsigned long*)(GPIO_BLOCK + 0x03e0)   // Inlock Register (Write only) - Port 1
    #define ASR_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x03e4)   // Access Status Register (Read/Write) - Port 1
    #define PARAMETER_1                  *(volatile unsigned long*)(GPIO_BLOCK + 0x03f8)   // Parameter Register (Read only) - Port 1
    #define VERSION_1                    *(volatile unsigned long*)(GPIO_BLOCK + 0x03fc)   // Version Register (Read only) - Port 1


    // Port 2 (C)
    //
    #define GPER_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0400)   // GPIO Enable Register (Read/Write) - Port 2
    #define GPERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0404)   // GPIO Register Set (Write only) - Port 2
    #define GPERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0408)   // GPIO Enable Register Register Clear (Write only) - Port 2
    #define GPERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x040c)   // GPIO Enable Register Toggle (Write only) - Port 2

    #define PMR0_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0410)   // Peripheral Mux Register 0 (Read/Write) - Port 2
    #define PMR0S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0414)   // Peripheral Mux Register 0 Set (Write only) - Port 2
    #define PMR0C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0418)   // Peripheral Mux Register 0 Clear (Write only) - Port 2
    #define PMR0T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x041c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 2

    #define PMR1_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0420)   // Peripheral Mux Register 1 (Read/Write) - Port 2
    #define PMR1S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0424)   // Peripheral Mux Register 1 Set (Write only) - Port 2
    #define PMR1C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0428)   // Peripheral Mux Register 1 Clear (Write only) - Port 2
    #define PMR1T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x042c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 2

    #define PMR2_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0430)   // Peripheral Mux Register 2 (Read/Write) - Port 2
    #define PMR2S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0434)   // Peripheral Mux Register 2 Set (Write only) - Port 2
    #define PMR2C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0438)   // Peripheral Mux Register 2 Clear (Write only) - Port 2
    #define PMR2T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x043c)   // Peripheral Mux Register 2 Toggle (Write only) - Port 2

    #define ODER_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0440)   // Output Driver Enable Register (Read/Write) - Port 2
    #define ODERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0444)   // Output Driver Enable Register Set (Write only) - Port 2
    #define ODERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0448)   // Output Driver Enable Register Clear (Write only) - Port 2
    #define ODERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x044c)   // Output Driver Enable Register Toggle (Write only) - Port 2

    #define OVR_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0450)   // Output Value Register (Read/Write) - Port 2
    #define OVRS_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0454)   // Output Value Register Set (Write only) - Port 2
    #define OVRC_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0458)   // Output Value Register Clear (Write only) - Port 2
    #define OVRT_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x045c)   // Output Value Register Toggle (Write only) - Port 2

    #define PVR_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0460)   // Pin Value Register (Read only) - Port 2

    #define PUER_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0470)   // Pull-up Enable Register (Read/Write) - Port 2
    #define PUERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0474)   // Pull-up Enable Register Set (Write only) - Port 2
    #define PUERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0478)   // Pull-up Enable Register Clear (Write only) - Port 2
    #define PUERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x047c)   // Pull-up Enable Register Toggle (Write only) - Port 2

    #define PDER_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0480)   // {17} Pull-down Enable Register (Read/Write) - Port 2
    #define PDERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0484)   // Pull-down Enable Register Set (Write only) - Port 2
    #define PDERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0488)   // Pull-down Enable Register Clear (Write only) - Port 2
    #define PDERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x048c)   // Pull-down Enable Register Toggle (Write only) - Port 2

    #define IER_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0490)   // Interrupt Enable Register 0 (Read/Write) - Port 2
    #define IERS_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0494)   // Interrupt Enable Register 0 Set (Write only) - Port 2
    #define IERC_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0498)   // Interrupt Enable Register 0 Clear (Write only) - Port 2
    #define IERT_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x049c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 2

    #define IMR0_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x04a0)   // Interrupt Mode Register 0 (Read/Write) - Port 2
    #define IMR0S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04a4)   // Interrupt Mode Register 0 Set (Write only) - Port 2
    #define IMR0C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 2
    #define IMR0T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 2

    #define IMR1_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x04b0)   // Interrupt Mode Register 1 (Read/Write) - Port 2
    #define IMR1S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04b4)   // Interrupt Mode Register 1 Set (Write only) - Port 2
    #define IMR1C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 2
    #define IMR1T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 2

    #define GFER_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x04c0)   // Glitch Filter Enable Register (Read/Write) - Port 2
    #define GFERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04c4)   // Glitch Filter Enable Register Set (Write only) - Port 2
    #define GFERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04c8)   // Glitch Filter Enable Register Clear (Write only) - Port 2
    #define GFERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 2

    #define IFR_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x04d0)   // Interrupt Flag Register (Read only) - Port 2
    #define IFRC_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x04d8)   // Interrupt Flag Register (Write only) - Port 2

    #define ODCR0_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0500)   // Output Driving Capability Register 0 (Read/Write) - Port 2
    #define ODCR0S_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0504)   // Output Driving Capability Register 0 Set (Write only) - Port 2
    #define ODCR0C_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0508)   // Output Driving Capability Register 0 Clear (Write only) - Port 2
    #define ODCR0T_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x050c)   // Output Driving Capability Register 0 Toggle (Write only) - Port 2

    #define ODCR1_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0510)   // Output Driving Capability Register 1 (Read/Write) - Port 2
    #define ODCR1S_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0514)   // Output Driving Capability Register 1 Set (Write only) - Port 2
    #define ODCR1C_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0518)   // Output Driving Capability Register 1 Clear (Write only) - Port 2
    #define ODCR1T_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x051c)   // Output Driving Capability Register 1 Toggle (Write only) - Port 2

    #define LOCK_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x05a0)   // Lock Register (Read/Write) - Port 2
    #define LOCKS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x05a4)   // Lock Register Set (Write only) - Port 2
    #define LOCKC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x05a8)   // Lock Register Clear (Write only) - Port 2
    #define LOCKT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x05ac)   // Lock Register Toggle (Write only) - Port 2

    #define UNLOCKS_2                    *(volatile unsigned long*)(GPIO_BLOCK + 0x05e0)   // Inlock Register (Write only) - Port 2
    #define ASR_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x05e4)   // Access Status Register (Read/Write) - Port 2
    #define PARAMETER_2                  *(volatile unsigned long*)(GPIO_BLOCK + 0x05f8)   // Parameter Register (Read only) - Port 2
    #define VERSION_2                    *(volatile unsigned long*)(GPIO_BLOCK + 0x05fc)   // Version Register (Read only) - Port 2

    // Port 3 (D)
    //
    #define GPER_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0600)   // GPIO Enable Register (Read/Write) - Port 3
    #define GPERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0604)   // GPIO Register Set (Write only) - Port 3
    #define GPERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0608)   // GPIO Enable Register Register Clear (Write only) - Port 3
    #define GPERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x060c)   // GPIO Enable Register Toggle (Write only) - Port 3

    #define PMR0_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0610)   // Peripheral Mux Register 0 (Read/Write) - Port 3
    #define PMR0S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0614)   // Peripheral Mux Register 0 Set (Write only) - Port 3
    #define PMR0C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0618)   // Peripheral Mux Register 0 Clear (Write only) - Port 3
    #define PMR0T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x061c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 3

    #define PMR1_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0620)   // Peripheral Mux Register 1 (Read/Write) - Port 3
    #define PMR1S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0624)   // Peripheral Mux Register 1 Set (Write only) - Port 3
    #define PMR1C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0628)   // Peripheral Mux Register 1 Clear (Write only) - Port 3
    #define PMR1T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x062c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 3

    #define PMR2_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0630)   // Peripheral Mux Register 2 (Read/Write) - Port 3
    #define PMR2S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0634)   // Peripheral Mux Register 2 Set (Write only) - Port 3
    #define PMR2C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0638)   // Peripheral Mux Register 2 Clear (Write only) - Port 3
    #define PMR2T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x063c)   // Peripheral Mux Register 2 Toggle (Write only) - Port 3

    #define ODER_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0640)   // Output Driver Enable Register (Read/Write) - Port 3
    #define ODERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0644)   // Output Driver Enable Register Set (Write only) - Port 3
    #define ODERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0648)   // Output Driver Enable Register Clear (Write only) - Port 3
    #define ODERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x064c)   // Output Driver Enable Register Toggle (Write only) - Port 3

    #define OVR_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0650)   // Output Value Register (Read/Write) - Port 3
    #define OVRS_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0654)   // Output Value Register Set (Write only) - Port 3
    #define OVRC_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0658)   // Output Value Register Clear (Write only) - Port 3
    #define OVRT_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x065c)   // Output Value Register Toggle (Write only) - Port 3

    #define PVR_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0660)   // Pin Value Register (Read only) - Port 3

    #define PUER_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0670)   // Pull-up Enable Register (Read/Write) - Port 3
    #define PUERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0674)   // Pull-up Enable Register Set (Write only) - Port 3
    #define PUERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0678)   // Pull-up Enable Register Clear (Write only) - Port 3
    #define PUERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x067c)   // Pull-up Enable Register Toggle (Write only) - Port 3

    #define PDER_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0680)   // {17} Pull-down Enable Register (Read/Write) - Port 3
    #define PDERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0684)   // Pull-down Enable Register Set (Write only) - Port 3
    #define PDERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0688)   // Pull-down Enable Register Clear (Write only) - Port 3
    #define PDERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x068c)   // Pull-down Enable Register Toggle (Write only) - Port 3

    #define IER_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0690)   // Interrupt Enable Register 0 (Read/Write) - Port 3
    #define IERS_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0694)   // Interrupt Enable Register 0 Set (Write only) - Port 3
    #define IERC_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0698)   // Interrupt Enable Register 0 Clear (Write only) - Port 3
    #define IERT_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x069c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 3

    #define IMR0_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x06a0)   // Interrupt Mode Register 0 (Read/Write) - Port 3
    #define IMR0S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06a4)   // Interrupt Mode Register 0 Set (Write only) - Port 3
    #define IMR0C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 3
    #define IMR0T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 3

    #define IMR1_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x06b0)   // Interrupt Mode Register 1 (Read/Write) - Port 3
    #define IMR1S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06b4)   // Interrupt Mode Register 1 Set (Write only) - Port 3
    #define IMR1C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 3
    #define IMR1T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 3

    #define GFER_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x06c0)   // Glitch Filter Enable Register (Read/Write) - Port 3
    #define GFERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06c4)   // Glitch Filter Enable Register Set (Write only) - Port 3
    #define GFERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06c8)   // Glitch Filter Enable Register Clear (Write only) - Port 3
    #define GFERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x06cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 3

    #define IFR_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x06d0)   // Interrupt Flag Register (Read only) - Port 3
    #define IFRC_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x06d8)   // Interrupt Flag Register (Write only) - Port 3

    #define ODCR0_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0700)   // Output Driving Capability Register 0 (Read/Write) - Port 3
    #define ODCR0S_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0704)   // Output Driving Capability Register 0 Set (Write only) - Port 3
    #define ODCR0C_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0708)   // Output Driving Capability Register 0 Clear (Write only) - Port 3
    #define ODCR0T_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x070c)   // Output Driving Capability Register 0 Toggle (Write only) - Port 3

    #define ODCR1_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0710)   // Output Driving Capability Register 1 (Read/Write) - Port 3
    #define ODCR1S_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0714)   // Output Driving Capability Register 1 Set (Write only) - Port 3
    #define ODCR1C_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0718)   // Output Driving Capability Register 1 Clear (Write only) - Port 3
    #define ODCR1T_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x071c)   // Output Driving Capability Register 1 Toggle (Write only) - Port 3

    #define LOCK_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x07a0)   // Lock Register (Read/Write) - Port 3
    #define LOCKS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x07a4)   // Lock Register Set (Write only) - Port 3
    #define LOCKC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x07a8)   // Lock Register Clear (Write only) - Port 3
    #define LOCKT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x07ac)   // Lock Register Toggle (Write only) - Port 3

    #define UNLOCKS_3                    *(volatile unsigned long*)(GPIO_BLOCK + 0x07e0)   // Inlock Register (Write only) - Port 3
    #define ASR_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x07e4)   // Access Status Register (Read/Write) - Port 3
    #define PARAMETER_3                  *(volatile unsigned long*)(GPIO_BLOCK + 0x07f8)   // Parameter Register (Read only) - Port 3
    #define VERSION_3                    *(volatile unsigned long*)(GPIO_BLOCK + 0x07fc)   // Version Register (Read only) - Port 3


    typedef struct stGPIO_REGS
    {
    unsigned long GPER;
    unsigned long GPERS;
    unsigned long GPERC; 
    unsigned long GPERT; 

    unsigned long PMR0;  
    unsigned long PMR0S; 
    unsigned long PMR0C; 
    unsigned long PMR0T; 

    unsigned long PMR1;  
    unsigned long PMR1S; 
    unsigned long PMR1C; 
    unsigned long PMR1T; 

    unsigned long PMR2;  
    unsigned long PMR2S; 
    unsigned long PMR2C; 
    unsigned long PMR2T; 

    unsigned long ODER;   
    unsigned long ODERS;  
    unsigned long ODERC;  
    unsigned long ODERT;  

    unsigned long OVR;    
    unsigned long OVRS;   
    unsigned long OVRC;   
    unsigned long OVRT;   

    unsigned long PVR;    
    unsigned long ulRes1[3];

    unsigned long PUER;   
    unsigned long PUERS;  
    unsigned long PUERC;  
    unsigned long PUERT;  

    unsigned long PDER;                                                  // {17}
    unsigned long PDERS; 
    unsigned long PDERC; 
    unsigned long PDERT;

    unsigned long IER;    
    unsigned long IERS;   
    unsigned long IERC;   
    unsigned long IERT;   

    unsigned long IMR0;   
    unsigned long IMR0S;  
    unsigned long IMR0C;  
    unsigned long IMR0T;  

    unsigned long IMR1;   
    unsigned long IMR1S;  
    unsigned long IMR1C;  
    unsigned long IMR1T;  

    unsigned long GFER;   
    unsigned long GFERS;  
    unsigned long GFERC;  
    unsigned long GFERT;  

    unsigned long IFR;    
    unsigned long ulRes2;
    unsigned long IFRC;   
    unsigned long ulRes3[9];

    unsigned long ODCR0;
    unsigned long ODCR0S;
    unsigned long ODCR0C;
    unsigned long ODCR0T;
    unsigned long ODCR1;
    unsigned long ODCR1S;
    unsigned long ODCR1C;
    unsigned long ODCR1T;
    unsigned long ulRes4[32];
    unsigned long LOCK;
    unsigned long LOCKS;
    unsigned long LOCKC;
    unsigned long LOCKT;
    unsigned long ulRes5[12];
    unsigned long UNLOCKS;
    unsigned long ASR;
    unsigned long ulRes6[4];
    unsigned long PARAMETER;
    unsigned long VERSION;
    } GPIO_REGS;
#else
    // Port 0 (A)
    //
    #define GPER_0                       *(unsigned long*)(GPIO_BLOCK + 0x0000)            // GPIO Enable Register (Read/Write) - Port 0
    #define GPERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0004)   // GPIO Register Set (Write only) - Port 0
    #define GPERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0008)   // GPIO Enable Register Register Clear (Write only) - Port 0
    #define GPERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x000c)   // GPIO Enable Register Toggle (Write only) - Port 0

    #define PMR0_0                       *(unsigned long*)(GPIO_BLOCK + 0x0010)            // Peripheral Mux Register 0 (Read/Write) - Port 0
    #define PMR0S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0014)   // Peripheral Mux Register 0 Set (Write only) - Port 0
    #define PMR0C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0018)   // Peripheral Mux Register 0 Clear (Write only) - Port 0
    #define PMR0T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x001c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 0

    #define PMR1_0                       *(unsigned long*)(GPIO_BLOCK + 0x0020)            // Peripheral Mux Register 1 (Read/Write) - Port 0
    #define PMR1S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0024)   // Peripheral Mux Register 1 Set (Write only) - Port 0
    #define PMR1C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0028)   // Peripheral Mux Register 1 Clear (Write only) - Port 0
    #define PMR1T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x002c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 0

    #define ODER_0                       *(unsigned long*)(GPIO_BLOCK + 0x0040)            // Output Driver Enable Register (Read/Write) - Port 0
    #define ODERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0044)   // Output Driver Enable Register Set (Write only) - Port 0
    #define ODERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0048)   // Output Driver Enable Register Clear (Write only) - Port 0
    #define ODERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x004c)   // Output Driver Enable Register Toggle (Write only) - Port 0

    #define OVR_0                        *(unsigned long*)(GPIO_BLOCK + 0x0050)            // Output Value Register (Read/Write) - Port 0
    #define OVRS_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0054)   // Output Value Register Set (Write only) - Port 0
    #define OVRC_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0058)   // Output Value Register Clear (Write only) - Port 0
    #define OVRT_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x005c)   // Output Value Register Toggle (Write only) - Port 0

    #define PVR_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0060)   // Pin Value Register (Read only) - Port 0

    #define PUER_0                       *(unsigned long*)(GPIO_BLOCK + 0x0070)            // Pull-up Enable Register (Read/Write) - Port 0
    #define PUERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0074)   // Pull-up Enable Register Set (Write only) - Port 0
    #define PUERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0078)   // Pull-up Enable Register Clear (Write only) - Port 0
    #define PUERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x007c)   // Pull-up Enable Register Toggle (Write only) - Port 0

    #define ODMER_0                      *(unsigned long*)(GPIO_BLOCK + 0x0080)            // Open Drain Mode Enable Register (Read/Write) - Port 0
    #define ODMERS_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0084)   // Open Drain Mode Enable Register Set (Write only) - Port 0
    #define ODMERC_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0088)   // Open Drain Mode Enable Register Clear (Write only) - Port 0
    #define ODMERT_0                     *(volatile unsigned long*)(GPIO_BLOCK + 0x008c)   // Open Drain Mode Enable Register Toggle (Write only) - Port 0

    #define IER_0                        *(unsigned long*)(GPIO_BLOCK + 0x0090)            // Interrupt Enable Register 0 (Read/Write) - Port 0
    #define IERS_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0094)   // Interrupt Enable Register 0 Set (Write only) - Port 0
    #define IERC_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0098)   // Interrupt Enable Register 0 Clear (Write only) - Port 0
    #define IERT_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x009c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 0

    #define IMR0_0                       *(unsigned long*)(GPIO_BLOCK + 0x00a0)            // Interrupt Mode Register 0 (Read/Write) - Port 0
    #define IMR0S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00a4)   // Interrupt Mode Register 0 Set (Write only) - Port 0
    #define IMR0C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 0
    #define IMR0T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 0

    #define IMR1_0                       *(unsigned long*)(GPIO_BLOCK + 0x00b0)            // Interrupt Mode Register 1 (Read/Write) - Port 0
    #define IMR1S_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00b4)   // Interrupt Mode Register 1 Set (Write only) - Port 0
    #define IMR1C_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 0
    #define IMR1T_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 0

    #define GFER_0                       *(unsigned long*)(GPIO_BLOCK + 0x00c0)            // Glitch Filter Enable Register (Read/Write) - Port 0
    #define GFERS_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00c4)   // Glitch Filter Enable Register Set (Write only) - Port 0
    #define GFERC_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00c8)   // Glitch Filter Enable Register Clear (Write only) - Port 0
    #define GFERT_0                      *(volatile unsigned long*)(GPIO_BLOCK + 0x00cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 0

    #define IFR_0                        *(volatile unsigned long*)(GPIO_BLOCK + 0x00d0)   // Interrupt Flag Register (Read only) - Port 0
    #define IFRC_0                       *(volatile unsigned long*)(GPIO_BLOCK + 0x00d8)   // Interrupt Flag Register (Write only) - Port 0


    // Port 1 (B)
    //
    #define GPER_1                       *(unsigned long*)(GPIO_BLOCK + 0x0100)            // GPIO Enable Register (Read/Write) - Port 1
    #define GPERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0104)   // GPIO Register Set (Write only) - Port 1
    #define GPERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0108)   // GPIO Enable Register Register Clear (Write only) - Port 1
    #define GPERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x010c)   // GPIO Enable Register Toggle (Write only) - Port 1

    #define PMR0_1                       *(unsigned long*)(GPIO_BLOCK + 0x0110)            // Peripheral Mux Register 0 (Read/Write) - Port 1
    #define PMR0S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0114)   // Peripheral Mux Register 0 Set (Write only) - Port 1
    #define PMR0C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0118)   // Peripheral Mux Register 0 Clear (Write only) - Port 1
    #define PMR0T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x011c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 1

    #define PMR1_1                       *(unsigned long*)(GPIO_BLOCK + 0x0120)            // Peripheral Mux Register 1 (Read/Write) - Port 1
    #define PMR1S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0124)   // Peripheral Mux Register 1 Set (Write only) - Port 1
    #define PMR1C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0128)   // Peripheral Mux Register 1 Clear (Write only) - Port 1
    #define PMR1T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x012c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 1

    #define ODER_1                       *(unsigned long*)(GPIO_BLOCK + 0x0140)            // Output Driver Enable Register (Read/Write) - Port 1
    #define ODERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0144)   // Output Driver Enable Register Set (Write only) - Port 1
    #define ODERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0148)   // Output Driver Enable Register Clear (Write only) - Port 1
    #define ODERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x014c)   // Output Driver Enable Register Toggle (Write only) - Port 1

    #define OVR_1                        *(unsigned long*)(GPIO_BLOCK + 0x0150)            // Output Value Register (Read/Write) - Port 1
    #define OVRS_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0154)   // Output Value Register Set (Write only) - Port 1
    #define OVRC_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0158)   // Output Value Register Clear (Write only) - Port 1
    #define OVRT_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x015c)   // Output Value Register Toggle (Write only) - Port 1

    #define PVR_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0160)   // Pin Value Register (Read only) - Port 1

    #define PUER_1                       *(unsigned long*)(GPIO_BLOCK + 0x0170)            // Pull-up Enable Register (Read/Write) - Port 1
    #define PUERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0174)   // Pull-up Enable Register Set (Write only) - Port 1
    #define PUERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0178)   // Pull-up Enable Register Clear (Write only) - Port 1
    #define PUERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x017c)   // Pull-up Enable Register Toggle (Write only) - Port 1

    #define ODMER_1                      *(unsigned long*)(GPIO_BLOCK + 0x0180)            // Open Drain Mode Enable Register (Read/Write) - Port 1
    #define ODMERS_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0184)   // Open Drain Mode Enable Register Set (Write only) - Port 1
    #define ODMERC_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0188)   // Open Drain Mode Enable Register Clear (Write only) - Port 1
    #define ODMERT_1                     *(volatile unsigned long*)(GPIO_BLOCK + 0x018c)   // Open Drain Mode Enable Register Toggle (Write only) - Port 1

    #define IER_1                        *(unsigned long*)(GPIO_BLOCK + 0x0190)            // Interrupt Enable Register 0 (Read/Write) - Port 1
    #define IERS_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0194)   // Interrupt Enable Register 0 Set (Write only) - Port 1
    #define IERC_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0198)   // Interrupt Enable Register 0 Clear (Write only) - Port 1
    #define IERT_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x019c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 1

    #define IMR0_1                       *(unsigned long*)(GPIO_BLOCK + 0x01a0)            // Interrupt Mode Register 0 (Read/Write) - Port 1
    #define IMR0S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01a4)   // Interrupt Mode Register 0 Set (Write only) - Port 1
    #define IMR0C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 1
    #define IMR0T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 1

    #define IMR1_1                       *(unsigned long*)(GPIO_BLOCK + 0x01b0)            // Interrupt Mode Register 1 (Read/Write) - Port 1
    #define IMR1S_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01b4)   // Interrupt Mode Register 1 Set (Write only) - Port 1
    #define IMR1C_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 1
    #define IMR1T_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 1

    #define GFER_1                       *(unsigned long*)(GPIO_BLOCK + 0x01c0)            // Glitch Filter Enable Register (Read/Write) - Port 1
    #define GFERS_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01c4)   // Glitch Filter Enable Register Set (Write only) - Port 1
    #define GFERC_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01c8)   // Glitch Filter Enable Register Clear (Write only) - Port 1
    #define GFERT_1                      *(volatile unsigned long*)(GPIO_BLOCK + 0x01cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 1

    #define IFR_1                        *(volatile unsigned long*)(GPIO_BLOCK + 0x01d0)   // Interrupt Flag Register (Read only) - Port 1
    #define IFRC_1                       *(volatile unsigned long*)(GPIO_BLOCK + 0x01d8)   // Interrupt Flag Register (Write only) - Port 1


    // Port 2 (C)
    //
    #define GPER_2                       *(unsigned long*)(GPIO_BLOCK + 0x0200)            // GPIO Enable Register (Read/Write) - Port 2
    #define GPERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0204)   // GPIO Register Set (Write only) - Port 2
    #define GPERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0208)   // GPIO Enable Register Register Clear (Write only) - Port 2
    #define GPERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x020c)   // GPIO Enable Register Toggle (Write only) - Port 2

    #define PMR0_2                       *(unsigned long*)(GPIO_BLOCK + 0x0210)            // Peripheral Mux Register 0 (Read/Write) - Port 2
    #define PMR0S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0214)   // Peripheral Mux Register 0 Set (Write only) - Port 2
    #define PMR0C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0218)   // Peripheral Mux Register 0 Clear (Write only) - Port 2
    #define PMR0T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x021c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 2

    #define PMR1_2                       *(unsigned long*)(GPIO_BLOCK + 0x0220)            // Peripheral Mux Register 1 (Read/Write) - Port 2
    #define PMR1S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0224)   // Peripheral Mux Register 1 Set (Write only) - Port 2
    #define PMR1C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0228)   // Peripheral Mux Register 1 Clear (Write only) - Port 2
    #define PMR1T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x022c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 2

    #define ODER_2                       *(unsigned long*)(GPIO_BLOCK + 0x0240)            // Output Driver Enable Register (Read/Write) - Port 2
    #define ODERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0244)   // Output Driver Enable Register Set (Write only) - Port 2
    #define ODERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0248)   // Output Driver Enable Register Clear (Write only) - Port 2
    #define ODERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x024c)   // Output Driver Enable Register Toggle (Write only) - Port 2

    #define OVR_2                        *(unsigned long*)(GPIO_BLOCK + 0x0250)            // Output Value Register (Read/Write) - Port 2
    #define OVRS_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0254)   // Output Value Register Set (Write only) - Port 2
    #define OVRC_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0258)   // Output Value Register Clear (Write only) - Port 2
    #define OVRT_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x025c)   // Output Value Register Toggle (Write only) - Port 2

    #define PVR_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0260)   // Pin Value Register (Read only) - Port 2

    #define PUER_2                       *(unsigned long*)(GPIO_BLOCK + 0x0270)            // Pull-up Enable Register (Read/Write) - Port 2
    #define PUERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0274)   // Pull-up Enable Register Set (Write only) - Port 2
    #define PUERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0278)   // Pull-up Enable Register Clear (Write only) - Port 2
    #define PUERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x027c)   // Pull-up Enable Register Toggle (Write only) - Port 2

    #define ODMER_2                      *(unsigned long*)(GPIO_BLOCK + 0x0280)            // Open Drain Mode Enable Register (Read/Write) - Port 2
    #define ODMERS_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0284)   // Open Drain Mode Enable Register Set (Write only) - Port 2
    #define ODMERC_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0288)   // Open Drain Mode Enable Register Clear (Write only) - Port 2
    #define ODMERT_2                     *(volatile unsigned long*)(GPIO_BLOCK + 0x028c)   // Open Drain Mode Enable Register Toggle (Write only) - Port 2

    #define IER_2                        *(unsigned long*)(GPIO_BLOCK + 0x0290)            // Interrupt Enable Register 0 (Read/Write) - Port 2
    #define IERS_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0294)   // Interrupt Enable Register 0 Set (Write only) - Port 2
    #define IERC_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0298)   // Interrupt Enable Register 0 Clear (Write only) - Port 2
    #define IERT_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x029c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 2

    #define IMR0_2                       *(unsigned long*)(GPIO_BLOCK + 0x02a0)            // Interrupt Mode Register 0 (Read/Write) - Port 2
    #define IMR0S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02a4)   // Interrupt Mode Register 0 Set (Write only) - Port 2
    #define IMR0C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 2
    #define IMR0T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 2

    #define IMR1_2                       *(unsigned long*)(GPIO_BLOCK + 0x02b0)            // Interrupt Mode Register 1 (Read/Write) - Port 2
    #define IMR1S_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02b4)   // Interrupt Mode Register 1 Set (Write only) - Port 2
    #define IMR1C_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 2
    #define IMR1T_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 2

    #define GFER_2                       *(unsigned long*)(GPIO_BLOCK + 0x02c0)            // Glitch Filter Enable Register (Read/Write) - Port 2
    #define GFERS_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02c4)   // Glitch Filter Enable Register Set (Write only) - Port 2
    #define GFERC_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02c8)   // Glitch Filter Enable Register Clear (Write only) - Port 2
    #define GFERT_2                      *(volatile unsigned long*)(GPIO_BLOCK + 0x02cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 2

    #define IFR_2                        *(volatile unsigned long*)(GPIO_BLOCK + 0x02d0)   // Interrupt Flag Register (Read only) - Port 2
    #define IFRC_2                       *(volatile unsigned long*)(GPIO_BLOCK + 0x02d8)   // Interrupt Flag Register (Write only) - Port 2


    // Port 3 (D)
    //
    #define GPER_3                       *(unsigned long*)(GPIO_BLOCK + 0x0300)            // GPIO Enable Register (Read/Write) - Port 3
    #define GPERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0304)   // GPIO Register Set (Write only) - Port 3
    #define GPERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0308)   // GPIO Enable Register Register Clear (Write only) - Port 3
    #define GPERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x030c)   // GPIO Enable Register Toggle (Write only) - Port 3

    #define PMR0_3                       *(unsigned long*)(GPIO_BLOCK + 0x0310)            // Peripheral Mux Register 0 (Read/Write) - Port 3
    #define PMR0S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0314)   // Peripheral Mux Register 0 Set (Write only) - Port 3
    #define PMR0C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0318)   // Peripheral Mux Register 0 Clear (Write only) - Port 3
    #define PMR0T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x031c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 3

    #define PMR1_3                       *(unsigned long*)(GPIO_BLOCK + 0x0320)            // Peripheral Mux Register 1 (Read/Write) - Port 3
    #define PMR1S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0324)   // Peripheral Mux Register 1 Set (Write only) - Port 3
    #define PMR1C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0328)   // Peripheral Mux Register 1 Clear (Write only) - Port 3
    #define PMR1T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x032c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 3

    #define ODER_3                       *(unsigned long*)(GPIO_BLOCK + 0x0340)            // Output Driver Enable Register (Read/Write) - Port 3
    #define ODERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0344)   // Output Driver Enable Register Set (Write only) - Port 3
    #define ODERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0348)   // Output Driver Enable Register Clear (Write only) - Port 3
    #define ODERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x034c)   // Output Driver Enable Register Toggle (Write only) - Port 3

    #define OVR_3                        *(unsigned long*)(GPIO_BLOCK + 0x0350)            // Output Value Register (Read/Write) - Port 3
    #define OVRS_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0354)   // Output Value Register Set (Write only) - Port 3
    #define OVRC_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0358)   // Output Value Register Clear (Write only) - Port 3
    #define OVRT_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x035c)   // Output Value Register Toggle (Write only) - Port 3

    #define PVR_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0360)   // Pin Value Register (Read only) - Port 3

    #define PUER_3                       *(unsigned long*)(GPIO_BLOCK + 0x0370)            // Pull-up Enable Register (Read/Write) - Port 3
    #define PUERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0374)   // Pull-up Enable Register Set (Write only) - Port 3
    #define PUERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0378)   // Pull-up Enable Register Clear (Write only) - Port 3
    #define PUERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x037c)   // Pull-up Enable Register Toggle (Write only) - Port 3

    #define ODMER_3                      *(unsigned long*)(GPIO_BLOCK + 0x0380)            // Open Drain Mode Enable Register (Read/Write) - Port 3
    #define ODMERS_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0384)   // Open Drain Mode Enable Register Set (Write only) - Port 3
    #define ODMERC_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0388)   // Open Drain Mode Enable Register Clear (Write only) - Port 3
    #define ODMERT_3                     *(volatile unsigned long*)(GPIO_BLOCK + 0x038c)   // Open Drain Mode Enable Register Toggle (Write only) - Port 3

    #define IER_3                        *(unsigned long*)(GPIO_BLOCK + 0x0390)            // Interrupt Enable Register 0 (Read/Write) - Port 3
    #define IERS_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0394)   // Interrupt Enable Register 0 Set (Write only) - Port 3
    #define IERC_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0398)   // Interrupt Enable Register 0 Clear (Write only) - Port 3
    #define IERT_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x039c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 3

    #define IMR0_3                       *(unsigned long*)(GPIO_BLOCK + 0x03a0)            // Interrupt Mode Register 0 (Read/Write) - Port 3
    #define IMR0S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03a4)   // Interrupt Mode Register 0 Set (Write only) - Port 3
    #define IMR0C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 3
    #define IMR0T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 3

    #define IMR1_3                       *(unsigned long*)(GPIO_BLOCK + 0x03b0)            // Interrupt Mode Register 1 (Read/Write) - Port 3
    #define IMR1S_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03b4)   // Interrupt Mode Register 1 Set (Write only) - Port 3
    #define IMR1C_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 3
    #define IMR1T_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 3

    #define GFER_3                       *(unsigned long*)(GPIO_BLOCK + 0x03c0)            // Glitch Filter Enable Register (Read/Write) - Port 3
    #define GFERS_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03c4)   // Glitch Filter Enable Register Set (Write only) - Port 3
    #define GFERC_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03c8)   // Glitch Filter Enable Register Clear (Write only) - Port 3
    #define GFERT_3                      *(volatile unsigned long*)(GPIO_BLOCK + 0x03cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 3

    #define IFR_3                        *(volatile unsigned long*)(GPIO_BLOCK + 0x03d0)   // Interrupt Flag Register (Read only) - Port 3
    #define IFRC_3                       *(volatile unsigned long*)(GPIO_BLOCK + 0x03d8)   // Interrupt Flag Register (Write only) - Port 3

    // Port 4 (E)
    //
    #define GPER_4                       *(unsigned long*)(GPIO_BLOCK + 0x0400)            // GPIO Enable Register (Read/Write) - Port 4
    #define GPERS_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0404)   // GPIO Register Set (Write only) - Port 4
    #define GPERC_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0408)   // GPIO Enable Register Register Clear (Write only) - Port 4
    #define GPERT_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x040c)   // GPIO Enable Register Toggle (Write only) - Port 4

    #define PMR0_4                       *(unsigned long*)(GPIO_BLOCK + 0x0410)            // Peripheral Mux Register 0 (Read/Write) - Port 4
    #define PMR0S_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0414)   // Peripheral Mux Register 0 Set (Write only) - Port 4
    #define PMR0C_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0418)   // Peripheral Mux Register 0 Clear (Write only) - Port 4
    #define PMR0T_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x041c)   // Peripheral Mux Register 0 Toggle (Write only) - Port 4

    #define PMR1_4                       *(unsigned long*)(GPIO_BLOCK + 0x0420)            // Peripheral Mux Register 1 (Read/Write) - Port 4
    #define PMR1S_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0424)   // Peripheral Mux Register 1 Set (Write only) - Port 4
    #define PMR1C_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0428)   // Peripheral Mux Register 1 Clear (Write only) - Port 4
    #define PMR1T_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x042c)   // Peripheral Mux Register 1 Toggle (Write only) - Port 4

    #define ODER_4                       *(unsigned long*)(GPIO_BLOCK + 0x0440)            // Output Driver Enable Register (Read/Write) - Port 4
    #define ODERS_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0444)   // Output Driver Enable Register Set (Write only) - Port 4
    #define ODERC_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0448)   // Output Driver Enable Register Clear (Write only) - Port 4
    #define ODERT_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x044c)   // Output Driver Enable Register Toggle (Write only) - Port 4

    #define OVR_4                        *(unsigned long*)(GPIO_BLOCK + 0x0450)            // Output Value Register (Read/Write) - Port 4
    #define OVRS_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0454)   // Output Value Register Set (Write only) - Port 4
    #define OVRC_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0458)   // Output Value Register Clear (Write only) - Port 4
    #define OVRT_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x045c)   // Output Value Register Toggle (Write only) - Port 4

    #define PVR_4                        *(volatile unsigned long*)(GPIO_BLOCK + 0x0460)   // Pin Value Register (Read only) - Port 4

    #define PUER_4                       *(unsigned long*)(GPIO_BLOCK + 0x0470)            // Pull-up Enable Register (Read/Write) - Port 4
    #define PUERS_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0474)   // Pull-up Enable Register Set (Write only) - Port 4
    #define PUERC_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x0478)   // Pull-up Enable Register Clear (Write only) - Port 4
    #define PUERT_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x047c)   // Pull-up Enable Register Toggle (Write only) - Port 4

    #define ODMER_4                      *(unsigned long*)(GPIO_BLOCK + 0x0480)            // Open Drain Mode Enable Register (Read/Write) - Port 4
    #define ODMERS_4                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0484)   // Open Drain Mode Enable Register Set (Write only) - Port 4
    #define ODMERC_4                     *(volatile unsigned long*)(GPIO_BLOCK + 0x0488)   // Open Drain Mode Enable Register Clear (Write only) - Port 4
    #define ODMERT_4                     *(volatile unsigned long*)(GPIO_BLOCK + 0x048c)   // Open Drain Mode Enable Register Toggle (Write only) - Port 4

    #define IER_4                        *(unsigned long*)(GPIO_BLOCK + 0x0490)            // Interrupt Enable Register 0 (Read/Write) - Port 4
    #define IERS_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0494)   // Interrupt Enable Register 0 Set (Write only) - Port 4
    #define IERC_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x0498)   // Interrupt Enable Register 0 Clear (Write only) - Port 4
    #define IERT_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x049c)   // Interrupt Enable Register 0 Toggle (Write only) - Port 4

    #define IMR0_4                       *(unsigned long*)(GPIO_BLOCK + 0x04a0)            // Interrupt Mode Register 0 (Read/Write) - Port 4
    #define IMR0S_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04a4)   // Interrupt Mode Register 0 Set (Write only) - Port 4
    #define IMR0C_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04a8)   // Interrupt Mode Register 0 Clear (Write only) - Port 4
    #define IMR0T_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04ac)   // Interrupt Mode Register 0 Toggle (Write only) - Port 4

    #define IMR1_4                       *(unsigned long*)(GPIO_BLOCK + 0x04b0)            // Interrupt Mode Register 1 (Read/Write) - Port 4
    #define IMR1S_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04b4)   // Interrupt Mode Register 1 Set (Write only) - Port 4
    #define IMR1C_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04b8)   // Interrupt Mode Register 1 Clear (Write only) - Port 4
    #define IMR1T_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04bc)   // Interrupt Mode Register 1 Toggle (Write only) - Port 4

    #define GFER_4                       *(unsigned long*)(GPIO_BLOCK + 0x04c0)            // Glitch Filter Enable Register (Read/Write) - Port 4
    #define GFERS_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04c4)   // Glitch Filter Enable Register Set (Write only) - Port 4
    #define GFERC_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04c8)   // Glitch Filter Enable Register Clear (Write only) - Port 4
    #define GFERT_4                      *(volatile unsigned long*)(GPIO_BLOCK + 0x04cc)   // Glitch Filter Enable Register Toggle (Write only) - Port 4

    #define IFR_4                        *(volatile unsigned long*)(GPIO_BLOCK + 0x04d0)   // Interrupt Flag Register (Read only) - Port 4
    #define IFRC_4                       *(volatile unsigned long*)(GPIO_BLOCK + 0x04d8)   // Interrupt Flag Register (Write only) - Port 4

    typedef struct stGPIO_REGS
    {
    unsigned long GPER;
    unsigned long GPERS;
    unsigned long GPERC; 
    unsigned long GPERT; 

    unsigned long PMR0;  
    unsigned long PMR0S; 
    unsigned long PMR0C; 
    unsigned long PMR0T; 

    unsigned long PMR1;  
    unsigned long PMR1S; 
    unsigned long PMR1C; 
    unsigned long PMR1T; 

    unsigned long ulRes0[4];

    unsigned long ODER;   
    unsigned long ODERS;  
    unsigned long ODERC;  
    unsigned long ODERT;  

    unsigned long OVR;    
    unsigned long OVRS;   
    unsigned long OVRC;   
    unsigned long OVRT;   

    unsigned long PVR;    
    unsigned long ulRes1[3];

    unsigned long PUER;   
    unsigned long PUERS;  
    unsigned long PUERC;  
    unsigned long PUERT;  

    unsigned long ODMER;  
    unsigned long ODMERS; 
    unsigned long ODMERC; 
    unsigned long ODMERT;

    unsigned long IER;    
    unsigned long IERS;   
    unsigned long IERC;   
    unsigned long IERT;   

    unsigned long IMR0;   
    unsigned long IMR0S;  
    unsigned long IMR0C;  
    unsigned long IMR0T;  

    unsigned long IMR1;   
    unsigned long IMR1S;  
    unsigned long IMR1C;  
    unsigned long IMR1T;  

    unsigned long GFER;   
    unsigned long GFERS;  
    unsigned long GFERC;  
    unsigned long GFERT;  

    unsigned long IFR;    
    unsigned long ulRes2;
    unsigned long IFRC;   
    unsigned long ulRes3[9];
    } GPIO_REGS;
#endif

// Port macros
//
// Configure pins as output, including enabling power and digital use. eg. _CONFIG_PORT_OUTPUT(1, PB27);
//
#define _CONFIG_PORT_OUTPUT(port, pins)  GPERS_##port = (pins); _SIM_CHANGE(port); ODERS_##port = (pins); _SIM_CHANGE(port)

// Configure pins as input, including enabling power and digital use. eg. _CONFIG_PORT_INPUT(0, PA06);
//
#define _CONFIG_PORT_INPUT(port, pins)   GPERS_##port = (pins); _SIM_CHANGE(port); ODERC_##port = (pins); _SIM_CHANGE(port)

// Configure a peripheral function - this chip has a configuration macro per peripheral type A..D (A..H for UC3C)
//
#ifdef _AT32UC3C                                                         // {10}
    #define _CONFIG_PERIPHERAL_A(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0C_##port = (pins); PMR1C_##port = (pins); PMR2C_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_B(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0S_##port = (pins); PMR1C_##port = (pins); PMR2C_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_C(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0C_##port = (pins); PMR1S_##port = (pins); PMR2C_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_D(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0S_##port = (pins); PMR1S_##port = (pins); PMR2C_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_E(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0C_##port = (pins); PMR1C_##port = (pins); PMR2S_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_F(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0S_##port = (pins); PMR1C_##port = (pins); PMR2S_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_G(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0C_##port = (pins); PMR1S_##port = (pins); PMR2S_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_H(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0S_##port = (pins); PMR1S_##port = (pins); PMR2S_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
#else
    #define _CONFIG_PERIPHERAL_A(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0C_##port = (pins); PMR1C_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_B(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0S_##port = (pins); PMR1C_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_C(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0C_##port = (pins); PMR1S_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
    #define _CONFIG_PERIPHERAL_D(port, pins) ODERC_##port = (pins); GPERS_##port = (pins); PMR0S_##port = (pins); PMR1S_##port = (pins); _SIM_CHANGE(port); GPERC_##port = pins; _SIM_CHANGE(port)
#endif

// Write full port width
//
#define _WRITE_PORT(port, value)         OVR_##port = value;  _SIM_CHANGE(port)

// Write to a port with a mask                                           // {18}
//
#define _WRITE_PORT_MASK(port, value, mask) OVR_##port = ((OVR_##port & ~(mask)) | ((value) & (mask)));   _SIM_CHANGE(port)

// Toggle a port with a mask
//
#define _TOGGLE_PORT(port, pins)         OVRT_##port = (pins);  _SIM_CHANGE(port)

// Read full port width
//
#define _READ_PORT(port)                 (PVR_##port)

// Read from a port with a mask
//
#define _READ_PORT_MASK(port, mask)      (PVR_##port & (mask))

// Configure outputs and then set a value to them                        // {18}
//
#define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(port, pins, value) GPERS_##port = (pins); _SIM_CHANGE(port); ODERS_##port = (pins); OVR_##port = ((OVR_##port & ~(pins)) | ((value) & (pins)));

// Set from outputs to inputs
//
#define _FLOAT_PORT(port, mask)          ODERC_##port = (mask);  _SIM_CHANGE(port)

// Set from inputs to outputs 
//
#define _DRIVE_PORT_OUTPUT(port, pins) ODERS_##port = (pins);  _SIM_CHANGE(port)

// Set from inputs to outputs and set a value to them                    // {18}
//
#define _DRIVE_PORT_OUTPUT_VALUE(port, pins, value) ODERS_##port = (pins); OVR_##port = ((OVR_##port & ~(pins)) | ((value) & (pins)));  _SIM_CHANGE(port)

// Set and clear individual bits of a port
//
#define _SETBITS(port, pins)             OVRS_##port = (pins);  _SIM_CHANGE(port)
#define _CLEARBITS(port, pins)           OVRC_##port = (pins);  _SIM_CHANGE(port)

// Configuring pin characteristics                                       // {5}
//
#ifdef _WINDOWS
    #define _GLITCH_FILTER_ENABLE(port, pins)   GFER_##port |= (pins)
    #define _GLITCH_FILTER_DISABLE(port, pins)  GFER_##port &= ~(pins)
    #define _PULLUP_ENABLE(port, pins)          PUER_##port |= (pins)
    #define _PULLUP_DISABLE(port, pins)         PUER_##port &= ~(pins)
    #if defined _AT32UC3C                                                // {17}
        #define _PULLDOWN_ENABLE(port, pins)    PDER_##port |= (pins)
        #define _PULLDOWN_DISABLE(port, pins)   PDER_##port &= ~(pins)
    #else
        #define _OPEN_DRAIN_ENABLE(port, pins)  ODMER_##port |= (pins)
        #define _OPEN_DRAIN_DISABLE(port, pins) ODMER_##port &= ~(pins)
    #endif
#else
    #define _GLITCH_FILTER_ENABLE(port, pins)   GFERS_##port = (pins)
    #define _GLITCH_FILTER_DISABLE(port, pins)  GFERC_##port = (pins)
    #define _PULLUP_ENABLE(port, pins)          PUERS_##port = (pins)
    #define _PULLUP_DISABLE(port, pins)         PUERC_##port = (pins)
    #if defined _AT32UC3C                                                // {17}
        #define _PULLDOWN_ENABLE(port, pins)    PDER_##port  = (pins)
        #define _PULLDOWN_DISABLE(port, pins)   PDER_##port  = (pins)
    #else
        #define _OPEN_DRAIN_ENABLE(port, pins)  ODMERS_##port = (pins)
        #define _OPEN_DRAIN_DISABLE(port, pins) ODMERC_##port = (pins)
    #endif
#endif



#define AVR_GPIO(x)                      (1 << (x%32))
#define AVR_PORT(x)                      (x/32)

#if defined _AT32UC3B                                                    // {1}
    #define USART0_TX_0_A                AVR_GPIO(19)
    #define USART0_TX_1_C                AVR_GPIO(43)
    #define USART0_RX_0_A                AVR_GPIO(18)
    #define USART0_RX_1_C                AVR_GPIO(42)
    #define USART1_TX_0_A                AVR_GPIO(23)
    #define USART1_TX_1_C                AVR_GPIO(34)
    #define USART1_RX_0_A                AVR_GPIO(24)
    #define USART1_RX_1_C                AVR_GPIO(35)
    #define USART2_TX_0_C                AVR_GPIO(21)
    #define USART2_TX_0_B                AVR_GPIO(26)
    #define USART2_RX_0_C                AVR_GPIO(20)
    #define USART2_RX_0_B                AVR_GPIO(27)

    #define USART0_RTS_0_A               AVR_GPIO(11)
    #define USART1_RTS_0_B               AVR_GPIO(10)
    #define USART1_RTS_1_A               AVR_GPIO(37)
    #define USART2_RTS_1_C               AVR_GPIO(32)

    #define USART0_CTS_0_A               AVR_GPIO(12)
    #define USART1_CTS_0_C               AVR_GPIO(9)
    #define USART1_CTS_0_A               AVR_GPIO(36)
    #define USART2_CTS_1_C               AVR_GPIO(33)

    #define TWI_SDA_A                    AVR_GPIO(10)
    #define TWI_SCL_A                    AVR_GPIO(9)

    #define SPI0_MISO_A                  AVR_GPIO(25)                    // {2}
    #define SPI0_MOSI_A                  AVR_GPIO(14)
    #define SPI0_SCK_A                   AVR_GPIO(15)
    #define SPI0_CS0_A                   AVR_GPIO(16)
    #define SPI0_CS1_A                   AVR_GPIO(17)
    #define SPI0_CS2_B                   AVR_GPIO(9)
    #define SPI0_CS3_B                   AVR_GPIO(10)

    #define ADC_TRIG_C                   AVR_GPIO(22)

    #define EIC_0_A                      AVR_GPIO(5)
    #define EIC_1_A                      AVR_GPIO(6)
    #define EIC_2_C                      AVR_GPIO(14)
    #define EIC_3_C                      AVR_GPIO(23)
    #define EIC_4_C                      AVR_GPIO(24)
    #define EIC_5_C                      AVR_GPIO(25)
    #define EIC_6_A                      AVR_GPIO(34)
    #define EIC_7_A                      AVR_GPIO(35)

    #define EIC_0_X                      EIC_0_A                         // for simulator and mapping
    #define EIC_1_X                      EIC_1_A
    #define EIC_2_X                      EIC_2_C
    #define EIC_3_X                      EIC_3_C
    #define EIC_4_X                      EIC_4_C
    #define EIC_5_X                      EIC_5_C
    #define EIC_6_X                      EIC_6_A
    #define EIC_7_X                      EIC_7_A
#elif defined _AT32UC3C                                                  // {10}
    #define USART0_TXD_3_A               AVR_GPIO(123)
    #define USART0_TXD_2_D               AVR_GPIO(80)
    #define USART0_TXD_3_E               AVR_GPIO(96)
    #define USART0_RXD_3_A               AVR_GPIO(124)
    #define USART0_RXD_2_D               AVR_GPIO(79)
    #define USART0_RXD_3_E               AVR_GPIO(97)
    #define USART0_RTS_3_A               AVR_GPIO(126)
    #define USART0_RTS_2_D               AVR_GPIO(77)
    #define USART0_CTS_3_A               AVR_GPIO(125)
    #define USART0_CTS_2_D               AVR_GPIO(78)
    #define USART0_CLK_1_A               AVR_GPIO(32)
    #define USART0_CLK_2_B               AVR_GPIO(78)
    #define USART0_CLK_3_C               AVR_GPIO(125)

    #define USART1_TXD_1_A               AVR_GPIO(49)
    #define USART1_TXD_3_A               AVR_GPIO(107)
    #define USART1_RXD_1_A               AVR_GPIO(48)
    #define USART1_RXD_3_A               AVR_GPIO(108)
    #define USART1_RTS_1_A               AVR_GPIO(46)
    #define USART1_RTS_3_A               AVR_GPIO(110)
    #define USART1_CTS_1_A               AVR_GPIO(47)
    #define USART1_CTS_3_A               AVR_GPIO(109)
    #define USART1_DSR_1_A               AVR_GPIO(43)
    #define USART1_DSR_3_A               AVR_GPIO(104)
    #define USART1_DTR_1_A               AVR_GPIO(42)
    #define USART1_DTR_3_A               AVR_GPIO(103)
    #define USART1_DCD_1_A               AVR_GPIO(44)
    #define USART1_DCD_3_A               AVR_GPIO(105)
    #define USART1_RI_1_A                AVR_GPIO(45)
    #define USART1_RI_3_A                AVR_GPIO(106)
    #define USART1_CLK_1_B               AVR_GPIO(47)
    #define USART1_CLK_3_B               AVR_GPIO(109)
    #define USART1_CLK_1_D               AVR_GPIO(38)

    #define USART2_TXD_2_0_C             AVR_GPIO(67)
    #define USART2_TXD_2_1_C             AVR_GPIO(68)
    #define USART2_RXD_2_0_C             AVR_GPIO(66)
    #define USART2_RXD_2_1_C             AVR_GPIO(69)
    #define USART2_RTS_2_0_C             AVR_GPIO(65)
    #define USART2_RTS_2_1_C             AVR_GPIO(71)
    #define USART2_CTS_2_0_C             AVR_GPIO(64)
    #define USART2_CTS_2_1_C             AVR_GPIO(70)
    #define USART2_CLK_2_B               AVR_GPIO(70)
    #define USART2_CLK_2_D               AVR_GPIO(76)

    #define USART3_TXD_2_F               AVR_GPIO(81)
    #define USART3_TXD_3_B               AVR_GPIO(111)
    #define USART3_TXD_3_A               AVR_GPIO(117)
    #define USART3_RXD_2_F               AVR_GPIO(82)
    #define USART3_RXD_3_B               AVR_GPIO(112)
    #define USART3_RXD_3_A               AVR_GPIO(118)
    #define USART3_RTS_3_A               AVR_GPIO(120)
    #define USART3_RTS_3_B               AVR_GPIO(114)
    #define USART3_RTS_2_F               AVR_GPIO(84)
    #define USART3_CTS_3_A               AVR_GPIO(119)
    #define USART3_CTS_3_B               AVR_GPIO(113)
    #define USART3_CTS_2_F               AVR_GPIO(83)
    #define USART3_CLK_3_B               AVR_GPIO(119)
    #define USART3_CLK_1_D               AVR_GPIO(37)
    #define USART3_CLK_3_D               AVR_GPIO(113)
    #define USART3_CLK_2_E               AVR_GPIO(86)

    #define USART4_TXD_2_0_E             AVR_GPIO(72)
    #define USART4_TXD_2_1_E             AVR_GPIO(89)
    #define USART4_TXD_3_0_E             AVR_GPIO(103)
    #define USART4_RXD_2_0_E             AVR_GPIO(73)
    #define USART4_RXD_2_1_E             AVR_GPIO(90)
    #define USART4_RXD_3_2_E             AVR_GPIO(104)
    #define USART4_RTS_2_0_E             AVR_GPIO(92)
    #define USART4_RTS_3_1_E             AVR_GPIO(106)
    #define USART4_CTS_2_0_E             AVR_GPIO(91)
    #define USART4_CTS_3_1_E             AVR_GPIO(105)
    #define USART4_CLK_3_0_E             AVR_GPIO(121)
    #define USART4_CLK_2_1_E             AVR_GPIO(95)

    #define SPI0_MOSI_0_A                AVR_GPIO(96)                    // {19}
    #define SPI0_MOSI_1_A                AVR_GPIO(100)                   // {19}
    #define SPI0_MOSI_B                  AVR_GPIO(42)                    // {19}
    #define SPI0_MISO_B                  AVR_GPIO(43)                    // {19}
    #define SPI0_MISO_0_A                AVR_GPIO(97)                    // {19}
    #define SPI0_MISO_1_A                AVR_GPIO(101)                   // {19}
    #define SPI0_SCK_0_A                 AVR_GPIO(38)                    // {19}
    #define SPI0_SCK_B                   AVR_GPIO(44)                    // {19}
    #define SPI0_SCK_1_A                 AVR_GPIO(98)                    // {19}
    #define SPI0_SCK_2_A                 AVR_GPIO(102)                   // {19}
    #define SPI0_CS0_0_B                 AVR_GPIO(12)                    // {19}
    #define SPI0_CS0_1_B                 AVR_GPIO(45)                    // {19}
    #define SPI0_CS0_2_B                 AVR_GPIO(61)                    // {19}
    #define SPI0_CS0_3_B                 AVR_GPIO(79)                    // {19}
    #define SPI0_CS0_0_A                 AVR_GPIO(99)                    // {19}
    #define SPI0_CS1_0_B                 AVR_GPIO(13)                    // {19}
    #define SPI0_CS1_1_B                 AVR_GPIO(46)                    // {19}
    #define SPI0_CS1_2_B                 AVR_GPIO(64)                    // {19}
    #define SPI0_CS1_3_B                 AVR_GPIO(80)                    // {19}
    #define SPI0_CS1_0_A                 AVR_GPIO(93)                    // {19}
    #define SPI0_CS2_0_B                 AVR_GPIO(48)                    // {19}
    #define SPI0_CS2_1_B                 AVR_GPIO(65)                    // {19}
    #define SPI0_CS2_2_B                 AVR_GPIO(81)                    // {19}
    #define SPI0_CS2_0_A                 AVR_GPIO(94)                    // {19}
    #define SPI0_CS3_0_B                 AVR_GPIO(49)                    // {19}
    #define SPI0_CS3_1_B                 AVR_GPIO(66)                    // {19}
    #define SPI0_CS3_0_A                 AVR_GPIO(95)                    // {19}

    #define SPI1_MISO_0_A                AVR_GPIO(37)                    // {19}
    #define SPI1_MISO_1_B                AVR_GPIO(52)
    #define SPI1_MISO_0_B                AVR_GPIO(59)                    // {19}
    #define SPI1_MOSI_1_B                AVR_GPIO(51)
    #define SPI1_MOSI_2_B                AVR_GPIO(60)                    // {19}
    #define SPI1_MOSI_A                  AVR_GPIO(36)                    // {19}
    #define SPI1_SCK_1_A                 AVR_GPIO(38)                    // {19}
    #define SPI1_SCK_1_B                 AVR_GPIO(53)
    #define SPI1_SCK_2_B                 AVR_GPIO(58)                    // {19}
    #define SPI1_CS0_0_B                 AVR_GPIO(14)                    // {19}
    #define SPI1_CS0_1_B                 AVR_GPIO(57)
    #define SPI1_CS0_0_A                 AVR_GPIO(39)                    // {19}
    #define SPI1_CS1_0_A                 AVR_GPIO(40)                    // {19}
    #define SPI1_CS1_0_B                 AVR_GPIO(15)                    // {19}
    #define SPI1_CS1_1_B                 AVR_GPIO(56)
    #define SPI1_CS1_2_B                 AVR_GPIO(72)                    // {19}
    #define SPI1_CS2_0_B                 AVR_GPIO(24)                    // {19}
    #define SPI1_CS2_0_A                 AVR_GPIO(41)                    // {19}
    #define SPI1_CS2_1_B                 AVR_GPIO(55)
    #define SPI1_CS2_2_B                 AVR_GPIO(73)                    // {19}
    #define SPI1_CS3_0_B                 AVR_GPIO(25)                    // {19}
    #define SPI1_CS3_1_B                 AVR_GPIO(54)
    #define SPI1_CS3_2_B                 AVR_GPIO(74)                    // {19}
    #define SPI1_CS3_0_A                 AVR_GPIO(92)                    // {19}

    #define MACB_SPEED_0_E               AVR_GPIO(22)
    #define MACB_WOL_0_E                 AVR_GPIO(23)
    #define MACB_TX_CLK_3_E              AVR_GPIO(125)
    #define MACB_TX_EN_1_E               AVR_GPIO(58)
    #define MACB_TX_EN_3_E               AVR_GPIO(126)
    #define MACB_TXDATA0_1_E             AVR_GPIO(59)
    #define MACB_TXDATA1_1_E             AVR_GPIO(60)
    #define MACB_TXDATA0_3_E             AVR_GPIO(107)
    #define MACB_TXDATA1_3_E             AVR_GPIO(108)
    #define MACB_TXDATA2_1_E             AVR_GPIO(36)
    #define MACB_TXDATA3_1_E             AVR_GPIO(37)
    #define MACB_TX_ER_1_E               AVR_GPIO(38)
    #define MACB_RX_DV_1_E               AVR_GPIO(39)
    #define MACB_RX_DV_3_E               AVR_GPIO(124)
    #define MACB_RXDATA0_1_E             AVR_GPIO(40)
    #define MACB_RXDATA0_3_E             AVR_GPIO(109)
    #define MACB_RXDATA1_1_E             AVR_GPIO(41)
    #define MACB_RXDATA1_3_E             AVR_GPIO(110)
    #define MACB_RXDATA2_1_E             AVR_GPIO(53)
    #define MACB_RXDATA3_1_E             AVR_GPIO(54)
    #define MACB_RX_ER_1_E               AVR_GPIO(45)
    #define MACB_RX_ER_3_E               AVR_GPIO(123)
    #define MACB_MDC_1_E                 AVR_GPIO(46)
    #define MACB_MDIO_1_E                AVR_GPIO(47)
    #define MACB_MDC_2_E                 AVR_GPIO(66)
    #define MACB_MDIO_2_E                AVR_GPIO(67)
    #define MACB_CRS_1_E                 AVR_GPIO(51)
    #define MACB_COL_1_E                 AVR_GPIO(52)
    #define MACB_RX_CLK_1_E              AVR_GPIO(55)

    #define EBI_DATA_0_2_C               AVR_GPIO(83)
    #define EBI_DATA_1_2_C               AVR_GPIO(84)
    #define EBI_DATA_2_2_C               AVR_GPIO(85)
    #define EBI_DATA_3_2_C               AVR_GPIO(86)
    #define EBI_DATA_4_2_C               AVR_GPIO(87)
    #define EBI_DATA_5_2_C               AVR_GPIO(88)
    #define EBI_DATA_6_2_C               AVR_GPIO(89)
    #define EBI_DATA_7_2_C               AVR_GPIO(90)
    #define EBI_DATA_8_2_C               AVR_GPIO(91)
    #define EBI_DATA_9_2_C               AVR_GPIO(92)
    #define EBI_DATA_10_2_C              AVR_GPIO(93)
    #define EBI_DATA_11_2_C              AVR_GPIO(94)
    #define EBI_DATA_12_2_C              AVR_GPIO(95)
    #define EBI_DATA_13_3_C              AVR_GPIO(96)
    #define EBI_DATA_14_3_C              AVR_GPIO(97)
    #define EBI_DATA_15_3_C              AVR_GPIO(98)

    #define EBI_NWE_0_3_C                AVR_GPIO(121)
    #define EBI_NRD_0_3_C                AVR_GPIO(122)

    #define EBI_NCS_0_2_C                AVR_GPIO(72)
    #define EBI_ADD_22_2_C               AVR_GPIO(74)

    #define EIC_0_0_C                    AVR_GPIO(25)
    #define EIC_0_0_B                    AVR_GPIO(29)
    #define EIC_0_2_D                    AVR_GPIO(91)
    #define EIC_0_3_B                    AVR_GPIO(117)

    #define EIC_1_0A_B                   AVR_GPIO(3)
    #define EIC_1_0B_B                   AVR_GPIO(19)
    #define EIC_1_0C_B                   AVR_GPIO(26)
    #define EIC_1_2_B                    AVR_GPIO(67)

    #define EIC_2_0_C                    AVR_GPIO(8)
    #define EIC_2_0_B                    AVR_GPIO(27)
    #define EIC_2_1_B                    AVR_GPIO(39)

    #define EIC_3_0_B                    AVR_GPIO(28)
    #define EIC_3_2_B                    AVR_GPIO(68)
    #define EIC_3_3_B                    AVR_GPIO(126)

    #define EIC_4_0_B                    AVR_GPIO(10)
    #define EIC_4_1_C                    AVR_GPIO(50)
    #define EIC_4_2_B                    AVR_GPIO(69)

    #define EIC_5_2_B                    AVR_GPIO(82)
    #define EIC_5_3_B                    AVR_GPIO(103)

    #define EIC_6_3A_B                   AVR_GPIO(104)
    #define EIC_6_3B_B                   AVR_GPIO(125)

    #define EIC_7_2_B                    AVR_GPIO(77)
    #define EIC_7_3_B                    AVR_GPIO(110)

    #define EIC_8_1_C                    AVR_GPIO(32)
    #define EIC_8_3_B                    AVR_GPIO(121)
#else
    #define USART0_RX_0_A                AVR_GPIO(0)
    #define USART0_TX_0_A                AVR_GPIO(1)
    #define USART1_RX_0_A                AVR_GPIO(5)
    #define USART1_TX_0_A                AVR_GPIO(6)
    #define USART2_RX_1_A                AVR_GPIO(61)
    #define USART2_TX_1_A                AVR_GPIO(62)
    #define USART3_RX_1_B                AVR_GPIO(42)
    #define USART3_TX_1_B                AVR_GPIO(43)

    #define USART0_TX_3_B                AVR_GPIO(99)
    #define USART0_RX_3_B                AVR_GPIO(100)
    #define USART1_TX_2_B                AVR_GPIO(95)
    #define USART1_RX_3_B                AVR_GPIO(96)
    #define USART2_TX_3_B                AVR_GPIO(109)
    #define USART2_RX_2_B                AVR_GPIO(90)
    #define USART3_TX_2_B                AVR_GPIO(91)
    #define USART3_RX_2_B                AVR_GPIO(92)

    #define USART0_RTS_0_A               AVR_GPIO(3)
    #define USART0_CTS_0_A               AVR_GPIO(4)
    #define USART0_RTS_3_B               AVR_GPIO(97)
    #define USART0_CTS_3_B               AVR_GPIO(98)

    #define USART1_RTS_0_A               AVR_GPIO(8)
    #define USART1_CTS_0_A               AVR_GPIO(9)
    #define USART1_RTS_2_B               AVR_GPIO(93)
    #define USART1_CTS_2_B               AVR_GPIO(94)

    #define USART2_RTS_0_B               AVR_GPIO(29)
    #define USART2_CTS_0_B               AVR_GPIO(30)
    #define USART2_RTS_1_B               AVR_GPIO(32)
    #define USART2_CTS_1_B               AVR_GPIO(33)
    #define USART2_RTS_3_B               AVR_GPIO(107)
    #define USART2_CTS_3_B               AVR_GPIO(108)

    #define USART3_RTS_1_C               AVR_GPIO(32)
    #define USART3_CTS_1_C               AVR_GPIO(33)
    #define USART3_RTS_2_B               AVR_GPIO(89)
    #define USART3_CTS_2_B               AVR_GPIO(88)

    #define TWI_SDA_A                    AVR_GPIO(29)
    #define TWI_SCL_A                    AVR_GPIO(30)

    #define SPI0_MISO_A                  AVR_GPIO(11)                    // {2}
    #define SPI0_MOSI_A                  AVR_GPIO(12)
    #define SPI0_SCK_A                   AVR_GPIO(13)
    #define SPI0_CS0_A                   AVR_GPIO(10)
    #define SPI0_CS1_B                   AVR_GPIO(8)
    #define SPI0_CS2_B                   AVR_GPIO(9)
    #define SPI0_CS3_C                   AVR_GPIO(7)

    #define SPI0_MISO_B                  AVR_GPIO(77)                    // {15}
    #define SPI0_MOSI_B                  AVR_GPIO(76)
    #define SPI0_SCK_B                   AVR_GPIO(75)
    #define SPI0_CS0_B                   AVR_GPIO(74)
    #define SPI0_CS1_1_B                 AVR_GPIO(73)
    #define SPI0_CS2_1_B                 AVR_GPIO(72)
    #define SPI0_CS3_B                   AVR_GPIO(71)

    #define SPI1_MISO_B                  AVR_GPIO(17)                    // {2}
    #define SPI1_MOSI_B                  AVR_GPIO(16)
    #define SPI1_SCK_B                   AVR_GPIO(15)
    #define SPI1_CS0_B                   AVR_GPIO(14)
    #define SPI1_CS1_B                   AVR_GPIO(18)
    #define SPI1_CS2_B                   AVR_GPIO(19)
    #define SPI1_CS3_B                   AVR_GPIO(20)

    #define MACB_WOL_C                   AVR_GPIO(18)
    #define MACB_TX_CLK_A                AVR_GPIO(32)
    #define MACB_TX_EN_A                 AVR_GPIO(33)
    #define MACB_TXDATA0_A               AVR_GPIO(34)
    #define MACB_TXDATA1_A               AVR_GPIO(35)
    #define MACB_TXDATA2_A               AVR_GPIO(42)
    #define MACB_TXDATA3_A               AVR_GPIO(43)

    #define MACB_CRS_A                   AVR_GPIO(36)
    #define MACB_RX_DV_A                 AVR_GPIO(47)

    #define MACB_COL_A                   AVR_GPIO(48)
    #define MACB_SPEED_A                 AVR_GPIO(50)

    #define MACB_RX_CLK_A                AVR_GPIO(49)
    #define MACB_RXDATA0_A               AVR_GPIO(37)
    #define MACB_RXDATA1_A               AVR_GPIO(38)
    #define MACB_RXDATA2_A               AVR_GPIO(45)
    #define MACB_RXDATA3_A               AVR_GPIO(46)

    #define MACB_RX_ER_A                 AVR_GPIO(39)
    #define MACB_TX_ER_A                 AVR_GPIO(44)

    #define MACB_RX_MDC_A                AVR_GPIO(40)
    #define MACB_RX_MDIO_A               AVR_GPIO(41)

    #define ADC_TRIG_B                   AVR_GPIO(50)

    #define EBI_DATA_0_2_A               AVR_GPIO(90)
    #define EBI_DATA_1_2_A               AVR_GPIO(91)
    #define EBI_DATA_2_2_A               AVR_GPIO(92)
    #define EBI_DATA_3_2_A               AVR_GPIO(93)
    #define EBI_DATA_4_2_A               AVR_GPIO(94)
    #define EBI_DATA_5_2_A               AVR_GPIO(95)
    #define EBI_DATA_6_3_A               AVR_GPIO(96)
    #define EBI_DATA_7_3_A               AVR_GPIO(97)
    #define EBI_DATA_8_3_A               AVR_GPIO(98)
    #define EBI_DATA_9_3_A               AVR_GPIO(99)
    #define EBI_DATA_10_3_A              AVR_GPIO(100)
    #define EBI_DATA_11_3_A              AVR_GPIO(101)
    #define EBI_DATA_12_3_A              AVR_GPIO(102)
    #define EBI_DATA_13_3_A              AVR_GPIO(103)
    #define EBI_DATA_14_3_A              AVR_GPIO(104)
    #define EBI_DATA_15_3_A              AVR_GPIO(105)

    #define EBI_ADD_0_A                  AVR_GPIO(70)
    #define EBI_ADD_1_A                  AVR_GPIO(71)
    #define EBI_ADD_2_A                  AVR_GPIO(72)
    #define EBI_ADD_3_A                  AVR_GPIO(73)
    #define EBI_ADD_4_A                  AVR_GPIO(74)
    #define EBI_ADD_5_A                  AVR_GPIO(75)
    #define EBI_ADD_6_A                  AVR_GPIO(76)
    #define EBI_ADD_7_A                  AVR_GPIO(77)
    #define EBI_ADD_8_A                  AVR_GPIO(78)
    #define EBI_ADD_9_A                  AVR_GPIO(79)
    #define EBI_ADD_10_A                 AVR_GPIO(80)
    #define EBI_ADD_11_A                 AVR_GPIO(81)
    #define EBI_ADD_12_A                 AVR_GPIO(82)
    #define EBI_ADD_13_A                 AVR_GPIO(83)
    #define EBI_ADD_14_A                 AVR_GPIO(84)
    #define EBI_ADD_15_A                 AVR_GPIO(85)
    #define EBI_ADD_16_A                 AVR_GPIO(86)
    #define EBI_ADD_17_A                 AVR_GPIO(87)
    #define EBI_ADD_18_A                 AVR_GPIO(88)
    #define EBI_ADD_19_A                 AVR_GPIO(89)
    #define EBI_ADD_20_C                 AVR_GPIO(26)
    #define EBI_ADD_21_0_C               AVR_GPIO(27)

    #define EBI_NCS_0_0_C                AVR_GPIO(25)
    #define EBI_NCS_1_A                  AVR_GPIO(106)
    #define EBI_NRD_0_3_A                AVR_GPIO(107)
    #define EBI_NWE_0_3_A                AVR_GPIO(108)
    #define EBI_NWE_1_A                  AVR_GPIO(109)

    #define EBI_SDCK_C                   AVR_GPIO(42)
    #define EBI_SDCKE_C                  AVR_GPIO(43)
    #define EBI_RAS_C                    AVR_GPIO(44)
    #define EBI_CAS_C                    AVR_GPIO(45)
    #define EBI_SDWE_C                   AVR_GPIO(46)
    #define EBI_SDA_C                    AVR_GPIO(48)

    #define EIC_0_B                      AVR_GPIO(21)
    #define EIC_1_B                      AVR_GPIO(22)
    #define EIC_2_B                      AVR_GPIO(23)
    #define EIC_3_B                      AVR_GPIO(24)
    #define EIC_4_B                      AVR_GPIO(3)
    #define EIC_5_B                      AVR_GPIO(4)
    #define EIC_6_B                      AVR_GPIO(10)
    #define EIC_7_C                      AVR_GPIO(8)
    #define EIC_8_A                      AVR_GPIO(20)

    #define EIC_0_X                      EIC_0_B                         // for simulator and mapping
    #define EIC_1_X                      EIC_1_B
    #define EIC_2_X                      EIC_2_B
    #define EIC_3_X                      EIC_3_B
    #define EIC_4_X                      EIC_4_B
    #define EIC_5_X                      EIC_5_B
    #define EIC_6_X                      EIC_6_B
    #define EIC_7_X                      EIC_7_C
#endif

#define PA00    0x00000001
#define PA01    0x00000002
#define PA02    0x00000004
#define PA03    0x00000008
#define PA04    0x00000010
#define PA05    0x00000020
#define PA06    0x00000040
#define PA07    0x00000080
#define PA08    0x00000100
#define PA09    0x00000200
#define PA10    0x00000400
#define PA11    0x00000800
#define PA12    0x00001000
#define PA13    0x00002000
#define PA14    0x00004000
#define PA15    0x00008000
#define PA16    0x00010000
#ifndef _AT32UC3C                                                        // {10}
    #define PA17    0x00020000
    #define PA18    0x00040000
#endif
#define PA19    0x00080000
#define PA20    0x00100000
#define PA21    0x00200000
#define PA22    0x00400000
#define PA23    0x00800000
#define PA24    0x01000000
#define PA25    0x02000000
#define PA26    0x04000000
#define PA27    0x08000000
#define PA28    0x10000000
#define PA29    0x20000000
#ifndef _AT32UC3C                                                        // {10}
    #define PA30    0x40000000
    #ifdef _AT32UC3B                                                     // {1}
        #define PA31  0x80000000
    #endif
#endif

#define PB00    0x00000001
#define PB01    0x00000002
#define PB02    0x00000004
#define PB03    0x00000008
#define PB04    0x00000010
#define PB05    0x00000020
#define PB06    0x00000040
#define PB07    0x00000080
#define PB08    0x00000100
#define PB09    0x00000200
#define PB10    0x00000400
#define PB11    0x00000800
#define PB12    0x00001000
#define PB13    0x00002000
#define PB14    0x00004000
#define PB15    0x00008000
#define PB16    0x00010000
#define PB17    0x00020000
#define PB18    0x00040000
#define PB19    0x00080000
#define PB20    0x00100000
#define PB21    0x00200000
#define PB22    0x00400000
#define PB23    0x00800000
#define PB24    0x01000000
#define PB25    0x02000000
#define PB26    0x04000000
#define PB27    0x08000000
#define PB28    0x10000000
#define PB29    0x20000000
#define PB30    0x40000000
#define PB31    0x80000000

#define PC00    0x00000001
#define PC01    0x00000002
#define PC02    0x00000004
#define PC03    0x00000008
#define PC04    0x00000010
#define PC05    0x00000020
#ifdef _AT32UC3C                                                         // {10}
    #define PC06    0x00000040
    #define PC07    0x00000080
    #define PC08    0x00000100
    #define PC09    0x00000200
    #define PC10    0x00000400
    #define PC11    0x00000800
    #define PC12    0x00001000
    #define PC13    0x00002000
    #define PC14    0x00004000
    #define PC15    0x00008000
    #define PC16    0x00010000
    #define PC17    0x00020000
    #define PC18    0x00040000
    #define PC19    0x00080000
    #define PC20    0x00100000
    #define PC21    0x00200000
    #define PC22    0x00400000
    #define PC23    0x00800000
    #define PC24    0x01000000
    #define PC25    0x02000000
    #define PC26    0x04000000
    #define PC27    0x08000000
    #define PC28    0x10000000
    #define PC29    0x20000000
    #define PC30    0x40000000
    #define PC31    0x80000000

    #define PD00    0x00000001
    #define PD01    0x00000002
    #define PD02    0x00000004
    #define PD03    0x00000008
    #define PD04    0x00000010
    #define PD05    0x00000020
    #define PD06    0x00000040
    #define PD07    0x00000080
    #define PD08    0x00000100
    #define PD09    0x00000200
    #define PD10    0x00000400
    #define PD11    0x00000800
    #define PD12    0x00001000
    #define PD13    0x00002000
    #define PD14    0x00004000
    #define PD15    0x00008000
    #define PD16    0x00010000
    #define PD17    0x00020000
    #define PD18    0x00040000
    #define PD19    0x00080000
    #define PD20    0x00100000
    #define PD21    0x00200000
    #define PD22    0x00400000
    #define PD23    0x00800000
    #define PD24    0x01000000
    #define PD25    0x02000000
    #define PD26    0x04000000
    #define PD27    0x08000000
    #define PD28    0x10000000
    #define PD29    0x20000000
    #define PD30    0x40000000
    #define PD31    0x80000000
#endif
#ifndef _AT32UC3C                                                        // {10}
    #define PX34    0x00000040
    #define PX33    0x00000080
    #define PX32    0x00000100
    #define PX31    0x00000200
    #define PX30    0x00000400
    #define PX29    0x00000800
    #define PX28    0x00001000
    #define PX27    0x00002000
    #define PX26    0x00004000
    #define PX25    0x00008000
    #define PX24    0x00010000
    #define PX23    0x00020000
    #define PX22    0x00040000
    #define PX21    0x00080000
    #define PX20    0x00100000
    #define PX19    0x00200000
    #define PX18    0x00400000
    #define PX17    0x00800000
    #define PX16    0x01000000
    #define PX15    0x02000000

    #define PX10    0x04000000
    #define PX9     0x08000000
    #define PX8     0x10000000
    #define PX7     0x20000000
    #define PX6     0x40000000
    #define PX5     0x80000000

    #define PX04    0x00000001
    #define PX03    0x00000002
    #define PX02    0x00000004
    #define PX01    0x00000008
    #define PX00    0x00000010

    #define PX39    0x00000020
    #define PX38    0x00000040
    #define PX37    0x00000080
    #define PX36    0x00000100
    #define PX35    0x00000200

    #define PX14    0x00000400
    #define PX13    0x00000800
    #define PX12    0x00001000
    #define PX11    0x00002000
#endif

#if defined _AT32UC3B
    #define PIN_XIN0   PA18
    #define PIN_XOUT0  PA19
    #define PIN_XIN1   PA28
    #define PIN_XOUT1  PA29
    #define PIN_XIN32  PA11
    #define PIN_XOUT32 PA12
#elif defined _AT32UC3C
    #define PIN_XIN0   PB30
    #define PIN_XOUT0  PB31
    #define PIN_XIN1   PB02
    #define PIN_XOUT1  PB03
    #define PIN_XIN32  PB00
    #define PIN_XOUT32 PB01
#else
    #define PIN_XIN0   PC02
    #define PIN_XOUT0  PC03
    #define PIN_XIN1   PC04
    #define PIN_XOUT1  PC05
    #define PIN_XIN32  PC00
    #define PIN_XOUT32 PC01
#endif

#define ACD_TRIGGER_INPUT  PB18
#define TIMER_TIOA0        PB23
#define TIMER_TIOA1        PB25
#define TIMER_TIOA2        PB27




// Software references used when setting up interrupts
//
#define PORT_INTERRUPT            1
#define TIMER_INTERRUPT           2
#define ADC_INTERRUPT             3                                      // {8}

#define IRQ_HIGH_LEVEL_SENSITIVE  0x00
#define IRQ_FALLING_EDGE          0x01
#define IRQ_RISING_EDGE           0x02
#define IRQ_LOW_LEVEL_SENSITIVE   0x04
#define IRQ_BOTH_EDGES            (IRQ_FALLING_EDGE | IRQ_RISING_EDGE)
#define IRQ_ASYNC                 0x04
#define IRQ_ENABLE_GLITCH_FILER   0x80


#define PORT_0                    0
#define PORT_1                    1
#define PORT_2                    2
#define PORT_3                    3
#define PORT_4                    4
#define EXT_INT_CONTROLLER        5
#define EXT_INT_CONTROLLER_ALT_1  (5 | 0x10)
#define EXT_INT_CONTROLLER_ALT_2  (5 | 0x20)
#define EXT_INT_CONTROLLER_ALT_3  (5 | 0x30)

#define EXT_INT_0                 0x001
#define EXT_INT_1                 0x002
#define EXT_INT_2                 0x004
#define EXT_INT_3                 0x008
#define EXT_INT_4                 0x010
#define EXT_INT_5                 0x020
#define EXT_INT_6                 0x040
#define EXT_INT_7                 0x080
#define EXT_INT_8                 0x100


// Define interrupt setup structures to suit this processor
//
typedef struct stINTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set

  unsigned char    int_port;                                             // The port (A..B)
  unsigned char    int_port_sense;                                       // Level sensitive, falling, rising, both
  unsigned long    int_port_bits;                                        // The input pins to be activated (in bit form)
} INTERRUPT_SETUP;

typedef struct stTIMER_INTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set

  unsigned char    timer_reference;                                      // Hardware timer to use
#ifdef _HW_TIMER_MODE
  unsigned char    timer_mode;                                           // {6} timer mode
#endif
  unsigned short   timer_value;                                          // The ms delay value
  unsigned short   timer_us_value;                                       // us value when delay is under 1ms
} TIMER_INTERRUPT_SETUP;


#define TIMER_SINGLE_SHOT               0x00                             // {6}
#define TIMER_PERIODIC                  0x01
#define TIMER_DISABLE                   0x02

typedef struct stADC_INTERRUPT_RESULT
{
    signed short     sADC_value;                                         // present ADC sample value
    unsigned char    ucADC_channel;                                      // ADC channel status
    unsigned char    ucADC_flags;                                        // flags
} ADC_INTERRUPT_RESULT;

#define ADC_RESULT_VALID                0x00
#define ADC_CHANNEL_OVERRUN             0x01                             // an overrun occured on this channel
#define ADC_RESULT_NOT_READY            0x02
#define ADC_INT_ZERO_CROSSING           0x04
#define ADC_INT_HIGH_LEVEL              0x08
#define ADC_INT_LOW_LEVEL               0x10
#define ADC_8BIT_MODE                   0x80

#define ADC_CHANNELS 8

typedef struct stADC_RESULTS
{
    signed short     sADC_value[ADC_CHANNELS];                           // present ADC sample values
    unsigned char    ucADC_status[ADC_CHANNELS];                         // present ADC channel status
} ADC_RESULTS;

// define interrupt setup structure to suit this processor
typedef struct stADC_SETUP
{
    void ( *int_handler )( ADC_INTERRUPT_RESULT* );                      // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set
    unsigned char    int_adc_bits;                                       // the ADC input bits
    unsigned char    int_adc_int_type;                                   // interrupt type (ADC_END_OF_SCAN_INT, ADC_ZERO_CROSSING_INT, ADC_LOW_LIMIT_INT, ADC_HIGH_LIMIT_INT)
    unsigned long    int_samples;                                        // the number of samples to be collected - using DMA
    unsigned long    int_adc_sample_rate;                                // sample rate im Hz
    unsigned long    int_adc_mode;                                       // parallel, sequencial, single-ended, differential, start/stop, etc.
    unsigned long    int_adc_speed;                                      // operating speed value (register value)
    unsigned short   int_zero_crossing_trigger;                          // zero crossing value
    unsigned short   int_high_level_trigger;                             // trigger when higher than this level
    unsigned short   int_low_level_trigger;                              // trigger when lower than this level
    void            *int_adc_result;                                     // results structure to be filled - type variable
} ADC_SETUP;

#define ADC_SAMPLING_SPEED(x) (((unsigned char)((MASTER_CLOCK/(x * 2)) - 1) << 8) | ((unsigned char)((x * 20)/800000) << 16) | ((unsigned char)((x * 6)/10000000) << 16))

#define ADC_FULL_SCALE_10BIT            (0x3ff)
#define ADC_FULL_SCALE_8BIT             (0xff)
#define ADC_VOLT_10BIT                  (unsigned short)((ADC_FULL_SCALE_10BIT * 1000) / ADC_REFERENCE_VOLTAGE)
#define ADC_VOLT_8BIT                   (unsigned char)((ADC_FULL_SCALE_8BIT * 1000) / ADC_REFERENCE_VOLTAGE)

#define ADC_ENABLE_INTS                 0x00
#define ADC_END_OF_SCAN_INT             0x01
#define ADC_ZERO_CROSSING_INT_POSITIVE  0x02
#define ADC_ZERO_CROSSING_INT_NEGATIVE  0x04
#define ADC_LOW_LIMIT_INT               0x08
#define ADC_SINGLE_SHOT_CROSSING_INT    0x10
#define ADC_HIGH_LIMIT_INT              0x20
#define ADC_SINGLE_SHOT_TRIGGER_INT     0x40
#define ADC_TRIGGERS_TERMINATED         0x80
#define ADC_DISABLE_INTS                0x80

#define ADC_SINGLE_SHOT_MODE            0x00000000                       // perform a single conversion on enabled channels
#define ADC_LOOP_MODE                   0x00000001                       // perform multiple conversions - if trigger enabled automatically monitor and report only trigger events
#define ADC_START_OPERATION             0x00000002                       // start software controlled conversion now
#define ADC_STOP_OPERATION              0x00000004                       // disable further conversions
#define ADC_READ_ONLY                   0x00000008                       // read values only
#define ADC_GET_RESULT                  0x00000010                       // get a stored result for the specified channel
#define ADC_ALL_RESULTS                 0x00000020                       // get all stored results for all channels
#define ADC_CONFIGURE_ADC               0x00000040                       // ADC general configuration
#define ADC_AUTO_POWER_DOWN_MODE        0x00000080                       // activate auto sleep mode
#define ADC_CONFIGURE_CHANNEL           0x00000100                       // a channel specific configuration
#define ADC_DISABLE_CHANNEL             0x00000200                       // disable specified channels
#define ADC_SET_CHANNEL_OFFSET          0x00000400                       // set an offset to the channel
#define ADC_TRIGGERED_TIM0              0x00000800                       // conversion triggers from timer 0
#define ADC_TRIGGERED_TIM1              0x00001000                       // conversion triggers from timer 1
#define ADC_TRIGGERED_TIM2              0x00002000                       // conversion triggers from timer 2
#define ADC_TRIGGERED_EXT               0x00004000                       // conversion triggers from external pin - rising edge ADTRG
#define ADC_8_BIT_MODE                  0x00008000                       // 8 bit mode rather than 10 bit mode



typedef struct stRTC_SETUP                                               // {16}
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
#define RTC_32K             0x20





// DP83848 PHYSICAL
//
#define DP83848_BASIC_MODE_CONTROL_REGISTER            (0x0 << 18)
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
#define DP83848_BASIC_MODE_STATUS_REGISTER             (0x1 << 18)
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
#define DP83848_PHY_IDENTIFIER_1                       (0x2 << 18)
#define DP83848_PHY_IDENTIFIER_2                       (0x3 << 18)
#define DP83848_AUTO_NEG_ADVERT_REG                    (0x4 << 18)
#define DP83848_AUTO_NEG_LINK_PARTNER_ABIL_REG         (0x5 << 18)
#define DP83848_AUTO_NEG_EXPANSION_REG                 (0x6 << 18)
#define DP83848_AUTO_NEG_NEXT_PAGE_REG                 (0x7 << 18)
#define DP83848_PHY_STATUS                             (0x10 << 18)
  #define DP83848_LINK_STATUS_UP                      0x0001
  #define DP83848_SPEED_STATUS                        0x0002
  #define DP83848_DUPLEX_STATUS                       0x0004
  #define DP83848_AUTO_NEG_COMPLETE                   0x0010
  #define DP83848_100BASET_DESCRAMBLE_LOCK            0x0200
  #define DP83848_100BASET_SIGNAL_DETECT              0x0400
#define DP83848_MICR                                   (0x11 << 18)
  #define DP83848_INTOE                               0x0001
  #define DP83848_INTEN                               0x0002
#define DP83848_MISR                                   (0x12 << 18)
  #define DP83848_LINK_INT_EN                         0x0020
  #define DP83848_LINK_INT                            0x2000
#define DP83848_FCS_COUNTER_REGISTER                   (0x14 << 18)
#define DP83848_RXER_COUNTER_REGISTER                  (0x15 << 18)
#define DP83848_SUBLAYER_CONFIG_STATUS_REGISTER        (0x16 << 18)
#define DP83848_RMII_AND_BYPASS_REGISTER               (0x17 << 18)
#define DP83848_LED_DIRECT_CONTROL_REGISTER            (0x18 << 18)
#define DP83848_PHY_CONTROL_REGISTER                   (0x19 << 18)
#define DP83848_10BASET_STATUS_CONTROL_REGISTER        (0x1a << 18)
#define DP83848_CD_TEST_CONTROL_REGISTER               (0x1b << 18)
#define DP83848_ENERGY_DETECT_CONTROL_REGISTER         (0x1d << 18)




typedef struct stAVR32_RX_BD
{
    volatile unsigned char   *ptrBlock;
    volatile unsigned long    bd_details;
} AVR32_RX_BD;

#define AVR32_FIXED_BUFFER_SIZE                       128                // rx and tx buffers are made up of several fixed sized buffers


// Receive buffer description
//
#define BD_OWNERSHIP                                  0x00000001
#define BD_RX_WRAP                                    0x00000002

#define BD_INFO_GLOBAL_FRAME                          0x80000000
#define BD_INFO_MULTICAST_HASH_MATCH                  0x40000000
#define BD_INFO_UNICAST_HASH_MATCH                    0x20000000
#define BD_INFO_EXTERNAL_ADDRESS_MATCH                0x10000000

#define BD_INFO_SPECIFIC_ADD_1_MATCH                  0x04000000
#define BD_INFO_SPECIFIC_ADD_2_MATCH                  0x02000000
#define BD_INFO_SPECIFIC_ADD_3_MATCH                  0x01000000
#define BD_INFO_SPECIFIC_ADD_4_MATCH                  0x00800000
#define BD_INFO_TYPE_ID_MATCH                         0x00400000
#define BD_INFO_VLAN_TAG_DETECTED                     0x00200000
#define BD_INFO_PRIORITY_TAG_DETECTED                 0x00100000
#define BD_INFO_VLAN_PRIORITY_FIELD                   0x000e0000
#define BD_INFO_CONCATENATION_FORMAT_INDICATOR        0x00010000
#define BD_INFO_END_OF_FRAME                          0x00008000
#define BD_INFO_START_OF_FRAME                        0x00004000
#define BD_INFO_LENGTH_OF_FRAME_MASK                  0x00000fff
#define BD_INFO_RECEIVE_BUFFER_OFSET_MASK             0x00003000

// Transmit buffer description
//
#define BD_INFO_USED                                  0x80000000
#define BD_TX_WRAP                                    0x40000000
#define BD_RETRY_LIMIT_EXCEEDED                       0x20000000
#define BD_TRANSMIT_UNDERRUN                          0x10000000
#define BD_BUFFER_EXHAUSTED_IN_MID_FRAME              0x08000000
#define BD_DONT_SEND_CRC                              0x00010000
#define BD_LAST_FRAME_BUFFER                          0x00008000
#define BD_TX_FRAME_LENGTH                            0x000007ff

// SPI modules
//
#define SPI_CR_0                         *(volatile unsigned long*)(SPI0_BLOCK + 0x0000) // SPI Control Register - write only
  #define SPIEN                          0x00000001
  #define SPIDIS                         0x00000002
  #define SPI_SWRST                      0x00000080
  #define LASTXFER                       0x01000000
#define SPI_MR_0                         *(unsigned long*)(SPI0_BLOCK + 0x0004)          // SPI Mode Register - R/W
  #define SPI_MSTR                       0x00000001
  #define SPI_PS                         0x00000002
  #define PCSDEC                         0x00000004
  #define SPI_FDIV                       0x00000008
  #define MODFDIS                        0x00000010
  #define SPI_LLB                        0x00000080
  #define SPI_CS_0                       0x000e0000
  #define SPI_CS_1                       0x000d0000
  #define SPI_CS_2                       0x000b0000
  #define SPI_CS_3                       0x00070000

#define SPI_RDR_0                        *(volatile unsigned long*)(SPI0_BLOCK + 0x0008) // SPI Receive Data Register - read only
#define SPI_TDR_0                        *(volatile unsigned long*)(SPI0_BLOCK + 0x000c) // SPI Transmit Data Register - write only
#define SPI_SR_0                         *(volatile unsigned long*)(SPI0_BLOCK + 0x0010) // SPI Status Register - read only
  #define SPI_RDRF                       0x00000001
  #define SPI_TDRE                       0x00000002
  #define SPI_MODF                       0x00000004
  #define SPI_OVRES                      0x00000008
  #define SPI_ENDRX                      0x00000010
  #define SPI_ENDTX                      0x00000020
  #define SPI_RXBUFF                     0x00000040
  #define SPI_TXBUFE                     0x00000080
  #define SPI_NSSR                       0x00000100
  #define SPI_TXEMPTY                    0x00000200
  #define SPI_SPIENS                     0x00010000
#define SPI_IER_0                        *(volatile unsigned long*)(SPI0_BLOCK + 0x0014) // SPI Interrupt Enable Register - write only
#define SPI_IDR_0                        *(volatile unsigned long*)(SPI0_BLOCK + 0x0018) // SPI Interrupt Disable Register - write only
#define SPI_IMR_0                        *(volatile unsigned long*)(SPI0_BLOCK + 0x001c) // SPI Interrupt Mask Register - read only
#define SPI_CSR0_0                       *(unsigned long*)(SPI0_BLOCK + 0x0030)          // SPI Chip Select Register 0 - R/W
#define SPI_CSR1_0                       *(unsigned long*)(SPI0_BLOCK + 0x0034)          // SPI Chip Select Register 1 - R/W
#define SPI_CSR2_0                       *(unsigned long*)(SPI0_BLOCK + 0x0038)          // SPI Chip Select Register 2 - R/W
#define SPI_CSR3_0                       *(unsigned long*)(SPI0_BLOCK + 0x003c)          // SPI Chip Select Register 3 - R/W
  #define SPI_CPOL                       0x00000001
  #define SPI_NCPHA                      0x00000002
  #define SPI_CSAAT                      0x00000008
  #define SPI_8_BITS                     0x00000000
  #define SPI_9_BITS                     0x00000010
  #define SPI_10_BITS                    0x00000020
  #define SPI_11_BITS                    0x00000030
  #define SPI_12_BITS                    0x00000040
  #define SPI_13_BITS                    0x00000050
  #define SPI_14_BITS                    0x00000060
  #define SPI_15_BITS                    0x00000070
  #define SPI_16_BITS                    0x00000080
  #define SPI_MAX_CLOCK                  0x00000100

#define SPI_CR_1                         *(volatile unsigned long*)(SPI1_BLOCK + 0x0000) // SPI Control Register - write only
#define SPI_MR_1                         *(unsigned long*)(SPI1_BLOCK + 0x0004)          // SPI Mode Register - R/W
#define SPI_RDR_1                        *(volatile unsigned long*)(SPI1_BLOCK + 0x0008) // SPI Receive Data Register - read only
#define SPI_TDR_1                        *(volatile unsigned long*)(SPI1_BLOCK + 0x000c) // SPI Transmit Data Register - write only
#define SPI_SR_1                         *(volatile unsigned long*)(SPI1_BLOCK + 0x0010) // SPI Status Register - read only
#define SPI_IER_1                        *(volatile unsigned long*)(SPI1_BLOCK + 0x0014) // SPI Interrupt Enable Register - write only
#define SPI_IDR_1                        *(volatile unsigned long*)(SPI1_BLOCK + 0x0018) // SPI Interrupt Disable Register - write only
#define SPI_IMR_1                        *(volatile unsigned long*)(SPI1_BLOCK + 0x001c) // SPI Interrupt Mask Register - read only
#define SPI_CSR0_1                       *(unsigned long*)(SPI1_BLOCK + 0x0030)          // SPI Chip Select Register 0 - R/W
#define SPI_CSR1_1                       *(unsigned long*)(SPI1_BLOCK + 0x0034)          // SPI Chip Select Register 1 - R/W
#define SPI_CSR2_1                       *(unsigned long*)(SPI1_BLOCK + 0x0038)          // SPI Chip Select Register 2 - R/W
#define SPI_CSR3_1                       *(unsigned long*)(SPI1_BLOCK + 0x003c)          // SPI Chip Select Register 3 - R/W


#if defined _AT32UC3B
    #if defined CHIP_48_PIN
        #define PORTS_AVAILABLE     1
    #else                                                                // 64 pin
        #define PORTS_AVAILABLE     2
    #endif
#else
    #define PORTS_AVAILABLE         4
#endif
#define PORT_WIDTH                  32

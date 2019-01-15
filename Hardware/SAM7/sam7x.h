/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:   	SAM7X.h
    Project: 	Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2015
    *********************************************************************
    22.03.2007 Removed IAR uEnableInterrupt() and uDisableInterrupt() mapping to intrinsics
    28.09.2007 Add INTERRUPT_SETUP struct                                {1}
    04.11.2007 Add #define LITTLE ENDIAN to sam7x.h since it is fixed for the processor
    04.11.2007 Add define for PITS
    23.12.2007 Extend USART with Peripheral DMA Registers                {2}
    02.01.2008 Adjust PIO register defines to be all volatile            {3}
    15.02.2008 AIC_IPR declared volatile
    11.05.2008 Add define LOCK_REGION_SIZE
    14.06.2008 Add USB definitions                                       {4}
    14.06.2008 Add EMAC_RSR bit definitions                              {5}
    02.07.2008 Add PORTA_IRQ0 and PORTA_FIQ port interrupt parameters    {6}
    02.07.2008 Modify port interrupt to control multiple bits            {7}
    05.10.2008 Add UART2 (DBGU) support                                  {8}
    06.10.2008 Add POWER_UP and POWER_DOWN macros                        {9}
    08.10.2008 Remove PLL setup to project file app_hw_sam7x.h           {10}
    21.10.2008 Extend and correct peripheral functions                   {11}
    21.10.2008 Configure SAM7X project to save complete FLASH when simulating {12}
    25.10.2008 SPI1 volatile register adjustments                        {13}
    15.11.2008 Interrupt controller volatile register adjustments        {14}
    22.11.2008 Change all read only and write only register defines to volatile {15}
    01.12.2008 Add IAR5 support                                          {16}
    07.12.2008 Change IRQ characteristics define to allow falling and rising edges to be defined {17}
    18.12.2008 PORTS_AVAILABLE and PORT_WIDTH moved here from WinSim.c   {18}
    22.12.2008 Add ADC interface and ADC DMA support                     {19}
    31.12.2008 Add glitch filter and pull up macros, and correct glitch filter addresses {20}
    14.01.2009 Add PORTA_IRQ1                                            {21}
    04.02.2009 Add us support to timer interrupt                         {22}
    28.02.2009 Add CTS_1 to port configurations                          {23}
    28.02.2009 Add RTSEN and RSTDIS                                      {24}
    01.03.2009 Add USART modes                                           {25}
    03.03.2009 Add USART idle timeout                                    {26}
    17.04.2009 Add USART CTS input change interrupt enable and CTS_HIGH  {27}
    13.06.2009 Add additional port macros for overall project compatibility {28}
    16.08.2009 Add SSC peripheral block                                  {29}
    28.08.2009 Correct CAN mailbox 6 and 7 addresses                     {30}
    07.09.2009 Add optional hardware timer mode                          {31}
    29.09.2009 Correct _CONFIG_DRIVE_PORT_OUTPUT_VALUE() macro           {32}
    14.10.2009 Correct CTS_HIGH                                          {33}
    28.10.2009 Add IRQ_PULLUP_ENABLE and IRQ_GLITCH_ENABLE options for port interrupts {34}
    03.01.2010 Add start_application()                                   {35}
    10.01.2010 Add additional EMAC_NCFGR register bit defines            {36}
    14.01.2010 Add PWM controller and PWM control                        {37}
    03.02.2010 Extend PMC status register content                        {38}
    13.02.2010 Add SAM7Sxxx                                              {39}
    13.02.2010 Add RAM and FLASH sizes                                   {40}
    14.02.2010 Correct port macros using PIO_OWER (use PIO_OWDR to clear mask) {41}
    03.03.2010 Add timer source from external pins                       {42}
    30.03.2010 Add FLASH controller 1 for 512k devices                   {43}
    02.04.2010 Add dummy VALIDATE_NEW_CONFIGURATION()                    {44}
    25.06.2010 Change USB synchronisation macros                         {45}
    12.10.2010 Add SAM7SE                                                {46}
    09.02.2011 Add RTT support                                           {47}
    17.02.2011 Monitor USB suspend when waiting for USB clock domain synchronisation to avoid potential hang {48}
    12.05.2011 Add SAM7SE SDRAM registers                                {49}
    07.06.2011 Add ADC_INTERRUPT                                         {50}
    11.06.2011 Add 9 bit USART mode                                      {51}
    23.10.2011 Add _EXCEPTION(x)                                         {52}
    27.10.2011 Add RTC_SETUP                                             {53}
    27.10.2011 Move the memory location definitions to here from app_hw_sam7x.h {54}
    27.10.2011 Add reset causes                                          {55}
    05.12.2011 Add _UART_INTERSPACE_RX() macro                           {56}
    05.12.2011 Add SAM7S peripheral identifier set                       {57}
    18.01.2013 Add RTC_INITIALISATION for compatibility                  {58}
    21.01.2013 Add _HW_SAM7L support                                     {59}
    23.11.2013 Modify USB synchonised control flag clears to support multiple flags {60}
    16.06.2013 Add UTC seconds value to RTC_SETUP                        {61}

*/


#ifdef _WINDOWS
    #include "SimSAM7X.h"
    #define _SIM_PORTS fnSimPorts()
    #define _EXCEPTION(x)            *(unsigned char *)0 = 0             // generate exception when simulating {52}
#else
    #define _EXCEPTION(x)                                                // ignore on target
    #if defined COMPILE_IAR
        #if defined COMPILE_IAR5                                         // {16}
            #include <intrinsics.h>
        #else
            #include <intrinsic.h>
        #endif
    #endif
    #define _SIM_PORTS
#endif

#define start_application(call_address)  ((void (*)(void))call_address)(); // {35} call specified address

#define SAVE_COMPLETE_FLASH                                              // {12} when simulating save complete FLASH content rather than only file system content

#define _ALIGN_HEAP_4
#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary

#define _LITTLE_ENDIAN                                                   // compile and operate project in LITTLE ENDIAN mode

#define MASTER_CLOCK   (PLL_OUTPUT_FREQ_INT/POST_DIVIDE)                 // {10}

#define PLL_OUTPUT_FREQ  MASTER_CLOCK

#define SLOW_CLOCK      32000                                            // RC oscillator

#define CAST_POINTER_ARITHMETIC unsigned long                            // SAM7X uses 32 bit pointers

#define FLASH_START_ADDRESS        0x100000                              // {54}
#define START_OF_FLASH             FLASH_START_ADDRESS
#define RAM_START_ADDRESS          0x200000

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
    #define RANDOM_SEED_LOCATION    (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 4)) // location of a long word which is never initialised and so has a random power on value
    #define RTC_SECONDS_LOCATION    (unsigned long *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 8))
    #define RTC_ALARM_LOCATION      (unsigned long *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 12))
    #define RTC_VALID_LOCATION      (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 14))
    #define RTC_PRESCALER_LOCATION  (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 16))
#endif

#if defined _HW_SAM7S
    #if defined _SAM7S16
        #define SIZE_OF_FLASH          (16 * 1024)
        #define SIZE_OF_RAM            (4 * 1024)
        #define CHIP_48_PIN
    #elif defined _SAM7S161                                              // 64 pin
        #define SIZE_OF_FLASH          (16 * 1024)
        #define SIZE_OF_RAM            (4 * 1024)
    #elif defined _SAM7S32
        #define SIZE_OF_FLASH          (8 * 1024)
        #define SIZE_OF_RAM            (32 * 1024)
        #define CHIP_48_PIN
    #elif defined _SAM7S321
        #define SIZE_OF_FLASH          (8 * 1024)
        #define SIZE_OF_RAM            (32 * 1024)
    #elif defined _SAM7S64
        #define SIZE_OF_FLASH          (16 * 1024)
        #define SIZE_OF_RAM            (64 * 1024)
    #elif defined _SAM7S128
        #define SIZE_OF_FLASH          (32 * 1024)
        #define SIZE_OF_RAM            (128 * 1024)
    #elif defined _SAM7S256
        #define SIZE_OF_FLASH          (256 * 1024)
        #define SIZE_OF_RAM            (64 * 1024)
    #elif defined _SAM7S512
        #define SIZE_OF_FLASH          (512 * 1024)
        #define SIZE_OF_RAM            (64 * 1024)
    #endif
#elif defined _HW_SAM7SE                                                 // {46} available in LQFP128 or LFBGA144
    #if defined _SAM7SE32
        #define SIZE_OF_FLASH          (32 * 1024)
        #define SIZE_OF_RAM            (8 * 1024)
    #elif defined _SAM7SE256
        #define SIZE_OF_FLASH          (256 * 1024)
        #define SIZE_OF_RAM            (32 * 1024)
    #elif defined _SAM7SE512
        #define SIZE_OF_FLASH          (512 * 1024)
        #define SIZE_OF_RAM            (32 * 1024)
    #endif
#elif defined _HW_SAM7L                                                  // {59}
    #if defined _SAM7L128
        #define SIZE_OF_FLASH          (128 * 1024)
        #define SIZE_OF_RAM            (6 * 1024)
    #else
        #define SIZE_OF_FLASH          (64 * 1024)
        #define SIZE_OF_RAM            (6 * 1024)
    #endif
#else                                                                    // 100pin
    #if defined _SAM7X128
        #define SIZE_OF_FLASH          (128 * 1024)
        #define SIZE_OF_RAM            (32 * 1024)
    #elif defined _SAM7X256
        #define SIZE_OF_FLASH          (256 * 1024)                      // {40}
        #define SIZE_OF_RAM            (64 * 1024)
    #elif defined _SAM7X512
        #define SIZE_OF_FLASH          (512 * 1024)
        #define SIZE_OF_RAM            (128 * 1024)
    #endif
#endif

// Ethernet configuration
//
#if defined _SAM7X128 || defined _SAM7X256 || defined _SAM7X512
    #define ETHERNET_AVAILABLE
#endif

// UART configuration
//
#define UARTS_AVAILABLE   3

/*****************************************************************************************************/


// Peripheral register blocks (each 16k in size)
//
#ifdef _WINDOWS
  #define TIMER0_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucTimer0))
  #define TIMER1_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucTimer1))
  #define TIMER2_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucTimer2))
  #define TIMER_CONTROL_BLOCK           ((unsigned char *)(&ucSAM7x.ucTimerCtl))
  #define USB_PERIPHERAL_BLOCK          ((unsigned char *)(&ucSAM7x.ucUSB))
  #define TWI_PERIPHERAL_BLOCK          ((unsigned char *)(&ucSAM7x.ucSimTWI))
  #define USART0_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimUart0))
  #define USART1_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimUart1))
  #define PWMC_PERIPHERAL_BLOCK         ((unsigned char *)(&ucSAM7x.ucPWMC)) // {37}
  #define CAN_PERIPHERAL_BLOCK          ((unsigned char *)(&ucSAM7x.ucSimCAN))
  #define SSC_PERIPHERAL_BLOCK          ((unsigned char *)(&ucSAM7x.ucSimSSC))
  #define ADC_PERIPHERAL_BLOCK          ((unsigned char *)(&ucSAM7x.ucSimADC))
  #define EMAC_PERIPHERAL_BLOCK         ((unsigned char *)(&ucSAM7x.ucSimMAC))
  #define SPI0_PERIPHERAL_BLOCK         ((unsigned char *)(&ucSAM7x.ucSimSPI0))
  #define SPI1_PERIPHERAL_BLOCK         ((unsigned char *)(&ucSAM7x.ucSimSPI1))
#else
  #define TIMER0_PERIPHERAL_BLOCK        0xFFFA0000
  #define TIMER1_PERIPHERAL_BLOCK        0xFFFA0040
  #define TIMER2_PERIPHERAL_BLOCK        0xFFFA0080
  #define TIMER_CONTROL_BLOCK            0xFFFA00C0

  #define AES128_PERIPHERAL_BLOCK        0xFFFA4000
  #define TDES_PERIPHERAL_BLOCK          0xFFFA8000
  #define USB_PERIPHERAL_BLOCK           0xFFFB0000
  #define TWI_PERIPHERAL_BLOCK           0xFFFB8000
  #define USART0_PERIPHERAL_BLOCK        0xFFFC0000
  #define USART1_PERIPHERAL_BLOCK        0xFFFC4000
  #define PWMC_PERIPHERAL_BLOCK          0xFFFCC000
  #define CAN_PERIPHERAL_BLOCK           0xFFFD0000
  #define SSC_PERIPHERAL_BLOCK           0xFFFD4000
  #define ADC_PERIPHERAL_BLOCK           0xFFFD8000
  #define EMAC_PERIPHERAL_BLOCK          0xFFFDC000
  #define SPI0_PERIPHERAL_BLOCK          0xFFFE0000
  #define SPI1_PERIPHERAL_BLOCK          0xFFFE4000
#endif

// System Controller blocks (variable sizes)
//
#ifdef _WINDOWS
  #define AIC_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimAIC))
  #define DBGU_PERIPHERAL_BLOCK      ((unsigned char *)(&ucSAM7x.ucSimDBGU))
  #define PIOA_PERIPHERAL_BLOCK      ((unsigned char *)(&ucSAM7x.ucSimPIOA))
  #ifndef _HW_SAM7S
    #define PIOB_PERIPHERAL_BLOCK    ((unsigned char *)(&ucSAM7x.ucSimPIOB))
  #endif
  #if defined _HW_SAM7SE || defined _HW_SAM7L                            // {46}{59}
    #define PIOC_PERIPHERAL_BLOCK    ((unsigned char *)(&ucSAM7x.ucSimPIOC))
  #endif
  #define PMC_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimPMC))
  #define RSTC_PERIPHERAL_BLOCK      ((unsigned char *)(&ucSAM7x.ucSimRST))
  #define RTT_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimRTT)) // {47}
  #define PIT_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimPIT))
  #define WDT_PERIPHERAL_BLOCK       ((unsigned char *)(&ucSAM7x.ucSimWDT))
  #define MC_PERIPHERAL_BLOCK        ((unsigned char *)(&ucSAM7x.ucSimMC))
  #ifdef _HW_SAM7SE                                                      // {46}
    #define EBI_PERIPHERAL_BLOCK     ((unsigned char *)(&ucSAM7x.ucSimEBI))
    #define SMC_PERIPHERAL_BLOCK     ((unsigned char *)(&ucSAM7x.ucSimSMC))
    #define ECC_PERIPHERAL_BLOCK     ((unsigned char *)(&ucSAM7x.ucSimECC))
  #endif
#else
  #define AIC_PERIPHERAL_BLOCK           0xFFFFF000
  #define DBGU_PERIPHERAL_BLOCK          0xFFFFF200
  #define PIOA_PERIPHERAL_BLOCK          0xFFFFF400
  #ifndef _HW_SAM7S
    #define PIOB_PERIPHERAL_BLOCK        0xFFFFF600
  #endif
  #ifdef _HW_SAM7SE                                                      // {46}
    #define PIOC_PERIPHERAL_BLOCK        0xFFFFF800
  #endif
  #define PMC_PERIPHERAL_BLOCK           0xFFFFFC00
  #define RSTC_PERIPHERAL_BLOCK          0xFFFFFD00
  #define RTT_PERIPHERAL_BLOCK           0xFFFFFD20
  #define PIT_PERIPHERAL_BLOCK           0xFFFFFD30
  #define WDT_PERIPHERAL_BLOCK           0xFFFFFD40
  #define VREG_PERIPHERAL_BLOCK          0xFFFFFD60
  #define MC_PERIPHERAL_BLOCK            0xFFFFFF00
  #ifdef _HW_SAM7SE                                                      // {46}
    #define EBI_PERIPHERAL_BLOCK         0xFFFFFF80                      // external bus interface
    #define SMC_PERIPHERAL_BLOCK         0xFFFFFF90                      // static memory controller
    #define ECC_PERIPHERAL_BLOCK         0xFFFFFFDC                      // error corrected code controller
    #define SDRAMC_PERIPHERAL_BLOCK      0xFFFFFFB0                      // {49} SDRAM memory controller
    #define SDRAM_BASE_ADDR              0x20000000                      // SDRAM base address
  #endif
#endif


// Peripheral identifiers
//
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {46}{59}
    #define AIC_FIQ                      0x00000001                      // Advanced Interrupt Controller - fast external interrupt
    #define SYSIRQ                       0x00000002                      //
    #define PIOA                         0x00000004                      // Parallel Input/Output Controller A
    #define PIOB                         0x00000008                      // Parallel Input/Output Controller B
    #define PIOC                         0x00000010                      // Parallel Input/Output Controller C
    #define SPI0                         0x00000020                      // Serial Peripheral Interface 0
    #define USART0                       0x00000040                      // USART 0
    #define USART1                       0x00000080                      // USART 1
    #define SSC                          0x00000100                      // Synchronous Serial Controller
    #define TWI                          0x00000200                      // Two Wire Interface (IIC)
    #define PWMC                         0x00000400                      // Pulse Width Modulator Controller
    #define UDP                          0x00000800                      // USB Device Port
    #define TC0                          0x00001000                      // Timer Counter 0
    #define TC1                          0x00002000                      // Timer Counter 1
    #define TC2                          0x00004000                      // Timer Counter 2
    #define ADC                          0x00008000                      // ADC Converter
                                                                         // 20..29 reserved
    #define AIC_IRQ0                     0x40000000                      // Advanced Interrupt Controller - external interrupt 0
    #define AIC_IRQ1                     0x80000000                      // Advanced Interrupt Controller - external interrupt 1
#elif defined _HW_SAM7S                                                  // {57}
    #define AIC_FIQ                      0x00000001                      // Advanced Interrupt Controller - fast external interrupt
    #define SYSIRQ                       0x00000002                      //
    #define PIOA                         0x00000004                      // Parallel Input/Output Controller A
                                                                         // 3 reserved
    #define ADC                          0x00000010                      // ADC Converter
    #define SPI0                         0x00000020                      // Serial Peripheral Interface 0
    #define USART0                       0x00000040                      // USART 0
    #define USART1                       0x00000080                      // USART 1
    #define SSC                          0x00000100                      // Synchronous Serial Controller
    #define TWI                          0x00000200                      // Two Wire Interface (IIC)
    #define PWMC                         0x00000400                      // Pulse Width Modulator Controller
    #define UDP                          0x00000800                      // USB Device Port
    #define TC0                          0x00001000                      // Timer Counter 0
    #define TC1                          0x00002000                      // Timer Counter 1
    #define TC2                          0x00004000                      // Timer Counter 2
                                                                         // 15..29 reserved
    #define AIC_IRQ0                     0x40000000                      // Advanced Interrupt Controller - external interrupt 0
    #define AIC_IRQ1                     0x80000000                      // Advanced Interrupt Controller - external interrupt 1
#else
    #define AIC_FIQ                      0x00000001                      // Advanced Interrupt Controller - fast external interrupt
    #define SYSIRQ                       0x00000002                      //
    #define PIOA                         0x00000004                      // Parallel Input/Output Controller A
    #define PIOB                         0x00000008                      // Parallel Input/Output Controller B
    #define SPI0                         0x00000010                      // Serial Peripheral Interface 0
    #define SPI1                         0x00000020                      // Serial Peripheral Interface 1
    #define USART0                       0x00000040                      // USART 0
    #define USART1                       0x00000080                      // USART 1
    #define SSC                          0x00000100                      // Synchronous Serial Controller
    #define TWI                          0x00000200                      // Two Wire Interface (IIC)
    #define PWMC                         0x00000400                      // Pulse Width Modulator Controller
    #define UDP                          0x00000800                      // USB Device Port
    #define TC0                          0x00001000                      // Timer Counter 0
    #define TC1                          0x00002000                      // Timer Counter 1
    #define TC2                          0x00004000                      // Timer Counter 2
    #define CAN_CTL                      0x00008000                      // CAN Controller
    #define EMAC                         0x00010000                      // EMAC
    #define ADC                          0x00020000                      // Analog to Digital Converter
    #define AES128                       0x00040000                      // Advanced Encryption Standard 128-bit
    #define TDES                         0x00080000                      // Tripple Data Encryption Standard
                                                                         // 20..29 reserved
    #define AIC_IRQ0                     0x40000000                      // Advanced Interrupt Controller - external interrupt 0
    #define AIC_IRQ1                     0x80000000                      // Advanced Interrupt Controller - external interrupt 1
#endif

// Real Time Timer                                                       {47}
//
#define RTT_MR                           *(volatile unsigned long*)(RTT_PERIPHERAL_BLOCK + 0x0) // RTT Mode register (R/W)
  #define RTT_MR_RTPRES_MASK             0x0000ffff
  #define RTT_MR_ALMIEN                  0x00010000                      // alarm interrupt enable
  #define RTT_MR_RTTINCIEN               0x00020000                      // increment interrupt enable
  #define RTT_MR_RTTRST                  0x00040000                      // timer restart
#define RTT_AR                           *(unsigned long*)(RTT_PERIPHERAL_BLOCK + 0x4) // RTT Alarm register (R/W)
#define RTT_VR                           *(volatile unsigned long*)(RTT_PERIPHERAL_BLOCK + 0x8) // RTT Value register (read-only)
#define RTT_SR                           *(volatile unsigned long*)(RTT_PERIPHERAL_BLOCK + 0xc) // RTT Status register (read-only)
  #define RTT_SR_ALMS                    0x00000001                      // alarm occurred since last read of the status register
  #define RTT_SR_RTTINC                  0x00000002                      // RTT has incremented since last read of the status register

#ifdef _WINDOWS
    #define _SET_RTT_DIVIDER(div)        RTT_MR = (div); RTT_VR = 0;
#else
    #define _SET_RTT_DIVIDER(div)        RTT_MR = (RTT_MR_RTTRST | div);
#endif

// Periodic Timer
//
#define PIT_MR                           *(unsigned long*)(PIT_PERIPHERAL_BLOCK + 0x00) // Mode register (R/W)
  #define PITIEN                         0x02000000                      // PIT Interrupt enable
  #define PITEN                          0x01000000                      // PIT enable
  #define PIV_MASK                       0x000FFFFF
#define PIT_SR                           *(volatile unsigned long*)(PIT_PERIPHERAL_BLOCK + 0x04) // {15} Status register (Read only)
  #define PITS                           0x00000001                      // Periodic Interval Timer has reached PIV since last read of PIT_PIVR
#define PIT_PIVR                         *(volatile unsigned long*)(PIT_PERIPHERAL_BLOCK + 0x08) // {15} Periodic Interval Value Register (Read only)
#define PIT_PIIR                         *(volatile unsigned long*)(PIT_PERIPHERAL_BLOCK + 0x0c) // {15} Periodic Interval Image Register (Read only)


// Watchdog Timer
//
#define WDT_CR                           *(volatile unsigned long*)(WDT_PERIPHERAL_BLOCK + 0x00) // {15} Control register (write only)
  #define WATCHDOG_KEY                   0xA5000000                      // Watchdog write key
  #define WDRSTT                         0x00000001                      // Watchdog Restart
#define WDT_MR                           *(unsigned long*)(WDT_PERIPHERAL_BLOCK + 0x04) // Mode register (Read/Write once)
  #define WDFIEN                         0x00001000                      // Fault interrupt enable
  #define WDRSTEN                        0x00002000                      // Watchdog reset enable
  #define WDRPROC                        0x00004000                      // Watchdog reset processor
  #define WDDBGHLT                       0x10000000                      // Watchdog debug halt
  #define WDIDLEHLT                      0x20000000                      // Watchdog IDLE MODE halt
  #define WDDIS                          0x00008000                      // Watchdog disable
#define WDT_SR                           *(volatile unsigned long*)(WDT_PERIPHERAL_BLOCK + 0x08) // {15} Status Register (Read only)
  #define WDERR                          0x00000002                      // Watchdog error
  #define WDUNF                          0x00000001                      // Underflow

// Power Management Controller
//
#define PMC_SCER                         *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x00) // {15} System Clock Enable Register (write only)
#define PMC_SCDR                         *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x04) // {15} System Clock Disable Register (write only)
#define PMC_SCSR                         *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x08) // {15} System Clock Status Register (read only)
  #define PROCESSOR_CLK_ENABLE           0x00000001
  #define PCK0_ENABLE                    0x00000100
  #define PCK1_ENABLE                    0x00000200
  #define PCK2_ENABLE                    0x00000400
  #define PCK3_ENABLE                    0x00000800
  #define UDP_ENABLE                     0x00000080

#define PMC_RES1                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x0c)   // Reserved
#define PMC_PCER                         *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x10) // {15} Peripheral Clock Enable Register (write only)
#define PMC_PCDR                         *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x14) // {15} Peripheral Clock Disable Register (write only)
#define PMC_PCSR                         *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x18) // {15} Peripheral Clock Status Register (read only)
#define PMC_RES2                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x1c)   // Reserved
#define CKGR_MOR                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x20)   // Main Oscillator Register (read/write)
  #define MOSCEN                         0x00000001
#define CKGR_MCFR                        *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x24) // {15} Main Clock Frequency Register (read only)
#define PMC_RES3                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x28)   // Reserved
#define CKGR_PLLR                        *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x2c)   // PLL Register (read/write)
  #define DIVIDE_5                       0x00000005
  #define MUL_26                         0x00190000
  #define UDP_DIV_2                      0x10000000
  #define UDP_DIV_4                      0x20000000
  #define OUT_B                          0x00008000                                       // higher frequency PLL output
#define PMC_MCKR                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x30)   // Master Clock Register (read/write)
  #define SELECT_MASTER_CLK              0x00000001
  #define SELECT_PLL                     0x00000003
  #define CLK_PRES_1                     0x00000000
  #define CLK_PRES_2                     0x00000004
  #define CLK_PRES_4                     0x00000008
  #define CLK_PRES_8                     0x0000000c
  #define CLK_PRES_16                    0x00000010
  #define CLK_PRES_32                    0x00000014
  #define CLK_PRES_64                    0x00000018
#define PMC_ACKR                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x34)   // Application Clock Register (read/write)
#define PMC_RES4                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x38)   // Reserved
#define PMC_RES5                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x3c)   // Reserved
#define PMC_PCK0                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x40)   // Programmable Clock 0 Register (read/write)
#define PMC_PCK1                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x44)   // Programmable Clock 1 Register (read/write)
#define PMC_RESA                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x48)   // Reserved
#define PMC_RESB                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x4c)   // Reserved
#define PMC_RESC                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x50)   // Reserved
#define PMC_RESD                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x54)   // Reserved
#define PMC_RESE                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x58)   // Reserved
#define PMC_RESF                         *(unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x5c)   // Reserved
#define PMC_IER                          *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x60) // {15} Interrupt Enable Register (write only)
#define PMC_IDR                          *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x64) // {15} Interrupt Disable Register (write only)
#define PMC_SR                           *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x68) // Status Register (read only)
  #define MOSCS                          0x00000001
  #define PLL_LOCK                       0x00000004
  #define MCKRDY                         0x00000008                                       // {38}
  #define PCKRDY0                        0x00000100
  #define PCKRDY1                        0x00000200
  #define PCKRDY2                        0x00000400
  #define PCKRDY3                        0x00000800
#define PMC_IMR                          *(volatile unsigned long*)(PMC_PERIPHERAL_BLOCK + 0x6c) // {15} Interrupt Mask Register (read only)

#ifdef _WINDOWS                                                          // {9}
    #define _POWER_UP(module)            PMC_PCSR |= module; PMC_PCER = PMC_PCDR = 0      // synchronise the status register
    #define _POWER_DOWN(module)          PMC_PCSR &= ~module; PMC_PCER = PMC_PCDR = 0
#else
    #define _POWER_UP(module)
    #define _POWER_DOWN(module)
#endif
#define POWER_UP(module)                 PMC_PCER = (module); _POWER_UP(module)   //  power up a module (apply clock to it)
#define POWER_DOWN(module)               PMC_PCDR = (module); _POWER_DOWN(module) // power down a module (disable clock to it)


// Reset Controller
//
#define RSTC_CR                          *(volatile unsigned long*)(RSTC_PERIPHERAL_BLOCK + 0x00) // {15} Clock Register (write only)
  #define PROCRST                        0x00000001                                       // command reset (use key too)
  #define PERRST                         0x00000004                                       // peripheral reset (use key too)
  #define EXTRST                         0x00000008                                       // assert reset line (use key too)

#define RSTC_SR                          *(volatile unsigned long*)(RSTC_PERIPHERAL_BLOCK + 0x04) // {15} Status Register (read only)
  #define RSTC_SR_URSTS                  0x00000001                                       // user reset status {55}
  #define RSTC_SR_BODSTS                 0x00000002                                       // brownout detection status
  #define RSTC_SR_RSTTYP_MASK            0x00000700                                       // reset type mask
  #define RSTC_SR_RSTTYP_POWER_UP_RESET  0x00000000                                       // power-up reset
  #define RSTC_SR_RSTTYP_WATCHDOG_RESET  0x00000200                                       // watchdog reset
  #define RSTC_SR_RSTTYP_SOFTWARE_RESET  0x00000300                                       // software reset
  #define RSTC_SR_RSTTYP_USER_RESET      0x00000400                                       // user reset (NRST)
  #define RSTC_SR_RSTTYP_BROWNOUT_RESET  0x00000500                                       // brownout reset
  #define RSTC_SR_NRSTL                  0x00010000                                       // NRST pin level as detected by MCLK
  #define RSTC_SR_SRCMP                  0x00020000                                       // software reset command in progress
#define RSTC_MR                          *(unsigned long*)(RSTC_PERIPHERAL_BLOCK + 0x08)  // Mode Register (read/write)
  #define RESET_60US                     (1 << 8)
  #define RESET_KEY                      0xa5000000                                       // reset key
  #define URSTEN                         0x00000001                                       // enable reset



// Advanced Interrupt Controller
//
#define ADD_AIC_SMR0                      (unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x00)
#define AIC_SMR0                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x00)   // Source mode Register 0 (read/write)  [FIQ]
#define AIC_SMR1                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x04)   // Source mode Register 1 (read/write)  [SYSIRQ]
#define AIC_SMR2                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x08)   // Source mode Register 2 (read/write)  [PIOA]
#define AIC_SMR3                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x0c)   // Source mode Register 3 (read/write)  [PIOB]
#define AIC_SMR4                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x10)   // Source mode Register 4 (read/write)  [SPI0]
#define AIC_SMR5                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x14)   // Source mode Register 5 (read/write)  [SPI1]
#define AIC_SMR6                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x18)   // Source mode Register 6 (read/write)  [UART0]
#define AIC_SMR7                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x1c)   // Source mode Register 7 (read/write)  [UART1]
#define AIC_SMR8                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x20)   // Source mode Register 8 (read/write)  [SSC]
#define AIC_SMR9                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x24)   // Source mode Register 9 (read/write)  [TWI]
#define AIC_SMR10                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x28)   // Source mode Register 10 (read/write) [PWMC]
#define AIC_SMR11                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x2c)   // Source mode Register 11 (read/write) [UDP]
#define AIC_SMR12                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x30)   // Source mode Register 12 (read/write) [TC0]
#define AIC_SMR13                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x34)   // Source mode Register 13 (read/write) [TC1]
#define AIC_SMR14                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x38)   // Source mode Register 14 (read/write) [TC2]
#define AIC_SMR15                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x3c)   // Source mode Register 15 (read/write) [CAN_CTL]
#define AIC_SMR16                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x40)   // Source mode Register 16 (read/write) [EMAC]
#define AIC_SMR17                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x44)   // Source mode Register 17 (read/write) [ADC]
#define AIC_SMR18                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x48)   // Source mode Register 18 (read/write) [AES128]
#define AIC_SMR19                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x4c)   // Source mode Register 19 (read/write) [TDES]
#define AIC_SMR20                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x50)   // Source mode Register 20 (read/write) [reserved]
#define AIC_SMR21                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x54)   // Source mode Register 21 (read/write) [reserved]
#define AIC_SMR22                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x58)   // Source mode Register 22 (read/write) [reserved]
#define AIC_SMR23                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x5c)   // Source mode Register 23 (read/write) [reserved]
#define AIC_SMR24                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x60)   // Source mode Register 24 (read/write) [reserved]
#define AIC_SMR25                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x64)   // Source mode Register 25 (read/write) [reserved]
#define AIC_SMR26                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x68)   // Source mode Register 26 (read/write) [reserved]
#define AIC_SMR27                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x6c)   // Source mode Register 27 (read/write) [reserved]
#define AIC_SMR28                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x70)   // Source mode Register 28 (read/write) [reserved]
#define AIC_SMR29                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x74)   // Source mode Register 29 (read/write) [reserved]
#define AIC_SMR30                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x78)   // Source mode Register 30 (read/write) [AIC_IRQ0]
#define AIC_SMR31                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x7c)   // Source mode Register 31 (read/write) [AIC_IRQ1]
  #define HIGH_LEVEL_SENSITIVE           0x40
  #define LOW_LEVEL_SENSITIVE            0x00
  #define POSITIVE_EDGE_TRIGGERED        0x60
  #define NEGATIVE_EDGE_TRIGGERED        0x20

#define ADD_AIC_SVR0                      (unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x80)   // Source vector Register 0 (read/write)
#define AIC_SVR0                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x80)   // Source vector Register 0 (read/write)  [FIQ]
#define AIC_SVR1                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x84)   // Source vector Register 1 (read/write)  [SYSIRQ]
#define AIC_SVR2                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x88)   // Source vector Register 2 (read/write)  [PIOA]
#define AIC_SVR3                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x8c)   // Source vector Register 3 (read/write)  [PIOB]
#define AIC_SVR4                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x90)   // Source vector Register 4 (read/write)  [SPI0]
#define AIC_SVR5                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x94)   // Source vector Register 5 (read/write)  [SPI1]
#define AIC_SVR6                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x98)   // Source vector Register 6 (read/write)  [UART0]
#define AIC_SVR7                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x9c)   // Source vector Register 7 (read/write)  [UART1]
#define AIC_SVR8                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xa0)   // Source vector Register 8 (read/write)  [SSC]
#define AIC_SVR9                         *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xa4)   // Source vector Register 9 (read/write)  [TWI]
#define AIC_SVR10                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xa8)   // Source vector Register 10 (read/write) [PWMC]
#define AIC_SVR11                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xac)   // Source vector Register 11 (read/write) [UDP]
#define AIC_SVR12                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xb0)   // Source vector Register 12 (read/write) [TC0]
#define AIC_SVR13                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xb4)   // Source vector Register 13 (read/write) [TC1]
#define AIC_SVR14                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xb8)   // Source vector Register 14 (read/write) [TC2]
#define AIC_SVR15                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xbc)   // Source vector Register 15 (read/write) [CAN_CTL]
#define AIC_SVR16                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xc0)   // Source vector Register 16 (read/write) [EMAC]
#define AIC_SVR17                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xc4)   // Source vector Register 17 (read/write) [ADC]
#define AIC_SVR18                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xc8)   // Source vector Register 18 (read/write) [AES128]
#define AIC_SVR19                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xcc)   // Source vector Register 19 (read/write) [TDES]
#define AIC_SVR20                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xd0)   // Source vector Register 20 (read/write) [reserved]
#define AIC_SVR21                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xd4)   // Source vector Register 21 (read/write) [reserved]
#define AIC_SVR22                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xd8)   // Source vector Register 22 (read/write) [reserved]
#define AIC_SVR23                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xdc)   // Source vector Register 23 (read/write) [reserved]
#define AIC_SVR24                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xe0)   // Source vector Register 24 (read/write) [reserved]
#define AIC_SVR25                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xe4)   // Source vector Register 25 (read/write) [reserved]
#define AIC_SVR26                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xe8)   // Source vector Register 26 (read/write) [reserved]
#define AIC_SVR27                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xec)   // Source vector Register 27 (read/write) [reserved]
#define AIC_SVR28                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xf0)   // Source vector Register 28 (read/write) [reserved]
#define AIC_SVR29                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xf4)   // Source vector Register 29 (read/write) [reserved]
#define AIC_SVR30                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xf8)   // Source vector Register 30 (read/write) [AIC_IRQ0]
#define AIC_SVR31                        *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0xfc)   // Source vector Register 31 (read/write) [AIC_IRQ1]

#define AIC_IVR                          *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x100) // {14} Interrupt Vector Register (read only)
#define AIC_FVR                          *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x104) // {14} Fast Interrupt Vector Register (read only)
#define AIC_ISR                          *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x108) // {14} Interrupt Status Register (read only)
#define AIC_IPR                          *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x10c) // Interrupt Pending Register (read only)
#define AIC_IMR                          *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x110) // {14} Interrupt Mask Register (read only)
#define AIC_CISR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x114) // {14} Core Interrupt Status Register (read only)

#define AIC_IECR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x120) // {14} Interrupt Enable Command Register (write only)
#define AIC_IDCR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x124) // {14} Interrupt Disable Command Register (write only)
#define AIC_ICCR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x128) // {14} Interrupt Clear Command Register (write only)
#define AIC_ISCR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x12c) // {14} Interrupt Set Command Register (write only)
#define AIC_EOICR                        *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x130) // {14} End of Interrupt Command Register (write only)
#define AIC_SPU                          *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x134)  // Spurious interrupt vector Register (read/write)
#define AIC_DCR                          *(unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x138)  // Debug Control Register (read/write)

#define AIC_FFER                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x140)  // {14} Fast Forcing Enable Register (write only)
#define AIC_FFDR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x144)  // {14} Fast Forcing Disable Register (write only)
#define AIC_FFSR                         *(volatile unsigned long*)(AIC_PERIPHERAL_BLOCK + 0x148)  // {14} Fast Forcing Status Register (read only)

// Memory controller
//
#define MC_RCR                           *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x0) // {15} Remap Control Register (write only)
  #define RCB                            0x00000001                                       // Remap command bit
#define MC_ASR                           *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x4) // {15} Abort Status Register (read only)
  #define UNDADD                         0x00000001                                       // Last abort due to undefined address
  #define MISADD                         0x00000002                                       // Last abort due to address misalignment
  #define NEBP                           0x00000080                                       // No Erase Before Programming
  #define ABTSZ_MASK                     0x00000300                                       // Abort size
  #define ABTTYT_MASK                    0x00000c00                                       // Abort type
  #define MST_EMAC                       0x00010000                                       // Abort caused by EMAC access
  #define MST_PDC                        0x00020000                                       // Abort caused by PDC access
  #define MST_ARM                        0x00040000                                       // Abort caused by ARM access
  #define SVMST_EMAC                     0x01000000                                       // Abort by EMAC access since last ASR read
  #define SVMST_PDC                      0x02000000                                       // Abort by PDC access since last ASR read
  #define SVMST_ARM                      0x04000000                                       // Abort by ARM access since last ASR read

#define MC_AASR                          *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x8) // {15} Abort Address Status Register (read only)
#define MC_FMR                           *(unsigned long*)(MC_PERIPHERAL_BLOCK + 0x60)    // FLASH Mode Register (read/write)
  #define ONE_WAIT_STATE                 0x00000100
  #define TWO_WAIT_STATES                0x00000200
  #define THREE_WAIT_STATES              0x00000300
  #define FLASH_TIME_SECURITY            (((unsigned long)(((MASTER_CLOCK + (1000000/2))/1000000) * 1.0))<<16) // clocks in 1,0us needed when writing FLASH security
  #define FLASH_TIME_NORMAL              (((unsigned long)(((MASTER_CLOCK + (1000000/2))/1000000) * 1.5))<<16) // clocks in 1,5us needed when writing FLASH, etc.
  #define NEBP                           0x00000080                                       // No Erase Before Programming
#define MC_FCR                           *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x64) // {15} FLASH Command Register (write only)
  #define FLASH_KEY                      0x5a000000
  #define FCMD_NOP                       0x00000000                                       // FLASH command : no command
  #define FCMD_WP                        0x00000001                                       // FLASH command : write page
  #define FCMD_SLB                       0x00000002                                       // FLASH command : set lock bit
  #define FCMD_WPL                       0x00000003                                       // FLASH command : write page lock
  #define FCMD_CLB                       0x00000004                                       // FLASH command : clear lock bit
  #define FCMD_EA                        0x00000008                                       // FLASH command : erase all
  #define FCMD_SGPB                      0x0000000b                                       // FLASH command : set general purpose NVM
  #define FCMD_CGPB                      0x0000000d                                       // FLASH command : clear general purpose NVM
  #define FCMD_SSB                       0x0000000f                                       // FLASH command : set security bit

  #define NV_BOOT_FROM_FLASH             0x00000200
#define MC_FSR                           *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x68) // FLASH Status Register (read only)
  #define FLASH_READY                    0x00000001                                       // FLASH ready for new command
  #define LOCKE                          0x00000004                                       // FLASH Lock error
  #define PROGE                          0x00000008                                       // FLASH Programming error status
  #define FLASH_SECURITY                 0x00000010                                       // FLASH security active

#define MC_FMR1                           *(unsigned long*)(MC_PERIPHERAL_BLOCK + 0x70)    // {43} FLASH Mode Register (read/write)
#define MC_FCR1                           *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x74) // FLASH Command Register (write only)
#define MC_FSR1                           *(volatile unsigned long*)(MC_PERIPHERAL_BLOCK + 0x78) // FLASH Status Register (read only)



#define FLASH_GRANULARITY                256                                              // smallest sector which can be erased independently
#define MAX_SECTOR_PARS                  (FLASH_GRANULARITY - 2)                          // The number of user bytes fitting into first parameter block
#define LOCK_REGION_SIZE                 (16*1024)

// External Bus Interface
//
#ifdef _HW_SAM7SE                                                        // {46}
    #define EBI_CSA                     *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x00)    // Chip Select Assignment Register
      #define EBI_CSA_CS1A_SDRAM        0x00000002                                        // assign chip select 1 to SDRAM controller rather than static memory controller
      #define EBI_CSA_CS2A_COMP_FLASH   0x00000004                                        // assign chip select 2 to static memory controller and compact flash logic is activated
      #define EBI_CSA_CS3A_NAND         0x00000008                                        // assign chip select 3 to static memory controller and NAND flash logic is activated
      #define EBI_CSA_CS4A_COMP_FLASH   0x00000010                                        // assign chip select 4 to static memory controller and compact flash logic is activated
      #define EBI_CSA_NWPC              0x00010000                                        // NWAIT pin is connected is connected to the External Wait Request input of the static memory controller

    #define SDRAM_BASE                  *(unsigned long*)(SDRAM_BASE_ADDR + 0x00)         // SDRAM base pointer {49}
    #define SDRAM_BASE_0x20             *(unsigned long*)(SDRAM_BASE_ADDR + 0x20)         // SDRAM base pointer + 0x20   
    #define SDRAMC_MR                   *(unsigned long*)(SDRAMC_PERIPHERAL_BLOCK + 0x00) // SDRAMC MR Register
    #define SDRAMC_TR                   *(unsigned long*)(SDRAMC_PERIPHERAL_BLOCK + 0x04) // SDRAMC TR Register
    #define SDRAMC_CR                   *(unsigned long*)(SDRAMC_PERIPHERAL_BLOCK + 0x08) // SDRAMC CR Register

    #define EBI_SMC0                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x10)    // 
    #define EBI_SMC1                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x14)    // 
    #define EBI_SMC2                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x18)    // 
    #define EBI_SMC3                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x1c)    // 
    #define EBI_SMC4                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x20)    // 
    #define EBI_SMC5                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x24)    // 
    #define EBI_SMC6                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x28)    // 
    #define EBI_SMC7                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x2c)    // 
    #define EBI_SDRAM0                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x30)    // 
    #define EBI_SDRAM1                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x34)    // 
    #define EBI_SDRAM2                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x38)    // 
    #define EBI_SDRAM3                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x3c)    // 
    #define EBI_SDRAM4                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x40)    // 
    #define EBI_SDRAM5                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x44)    // 
    #define EBI_SDRAM6                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x48)    // 
    #define EBI_SDRAM7                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x4c)    // 
    #define EBI_SDRAM8                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x50)    // 
    #define EBI_SDRAM9                  *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x54)    // 
    #define EBI_SDRAM10                 *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x58)    // 
    #define EBI_ECC0                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x5c)    // 
    #define EBI_ECC1                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x60)    // 
    #define EBI_ECC2                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x64)    // 
    #define EBI_ECC3                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x68)    // 
    #define EBI_ECC4                    *(unsigned long*)(EBI_PERIPHERAL_BLOCK + 0x6c)    // 
#endif

// Static Memory Controller
//
#ifdef _HW_SAM7SE                                                        // {46}
    #define SMC_CSR0                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x00)   // SMC Chip Select Register 0
      #define SMC_SCR_NWS_MASK           0x0000007f                                       // 0 = 1 wait state - 127 = 128 wait states
      #define SMC_SCR_SWEN               0x00000080                                       // enable wait states
      #define SMC_SCR_NWS_1_WAIT_STATE   0x00000080                                       // 1 wait state enabled
      #define SMC_SCR_NWS_2_WAIT_STATES  0x00000081                                       // 2 wait states enabled
      #define SMC_SCR_NWS_3_WAIT_STATES  0x00000082                                       // 3 wait states enabled
      #define SMC_SCR_NWS_4_WAIT_STATES  0x00000083                                       // 4 wait states enabled
      #define SMC_SCR_NWS_5_WAIT_STATES  0x00000084                                       // 5 wait states enabled
      #define SMC_SCR_NWS_6_WAIT_STATES  0x00000085                                       // 6 wait states enabled
      #define SMC_SCR_NWS_7_WAIT_STATES  0x00000086                                       // 7 wait states enabled
      #define SMC_SCR_NWS_8_WAIT_STATES  0x00000087                                       // 8 wait states enabled
      #define SMC_SCR_TDF_1              0x00000100                                       // 1 data float cycle added
      #define SMC_SCR_TDF_2              0x00000200                                       // 2 data float cycles
      #define SMC_SCR_TDF_3              0x00000300                                       // 3 data float cycles
      #define SMC_SCR_TDF_4              0x00000400                                       // 4 data float cycles
      #define SMC_SCR_TDF_5              0x00000500                                       // 5 data float cycles
      #define SMC_SCR_TDF_6              0x00000600                                       // 6 data float cycles
      #define SMC_SCR_TDF_7              0x00000700                                       // 7 data float cycles
      #define SMC_SCR_TDF_8              0x00000800                                       // 8 data float cycles
      #define SMC_SCR_TDF_9              0x00000900                                       // 9 data float cycles
      #define SMC_SCR_TDF_10             0x00000a00                                       // 10 data float cycles
      #define SMC_SCR_TDF_11             0x00000b00                                       // 11 data float cycles
      #define SMC_SCR_TDF_12             0x00000c00                                       // 12 data float cycles
      #define SMC_SCR_TDF_13             0x00000d00                                       // 13 data float cycles
      #define SMC_SCR_TDF_14             0x00000e00                                       // 14 data float cycles
      #define SMC_SCR_TDF_15             0x00000f00                                       // 15 data float cycles
      #define SMC_SCR_BAT_8              0x00000000                                       // 2 x 8 bit devices (only when SMC_SCR_DBW_16)
      #define SMC_SCR_BAT_16             0x00001000                                       // 1 x 16 bit device (only when SMC_SCR_DBW_16)
      #define SMC_SCR_DBW_16             0x00002000                                       // 16 bit interface
      #define SMC_SCR_DBW_8              0x00004000                                       // 8 bit interface
      #define SMC_SCR_DRP_STANDARD       0x00000000                                       // standard data read protocol
      #define SMC_SCR_DRP_EARLY          0x00008000                                       // early data read protocol
      #define SMC_SCR_ACSS_STANDARD      0x00000000                                       // standard chip select waveform (asserted at the beginning and de-asserted at end)
      #define SMC_SCR_ACSS_1             0x00010000                                       // chip select waveform (asserted one cycle less at the beginning and at end)
      #define SMC_SCR_ACSS_2             0x00020000                                       // chip select waveform (asserted two cycles less at the beginning and at end)
      #define SMC_SCR_ACSS_3             0x00030000                                       // chip select waveform (asserted three cycles less at the beginning and at end)
      #define SMC_SCR_RWSETUP_0          0x00000000                                       // RW setup half-cycle 0
      #define SMC_SCR_RWSETUP_1          0x01000000                                       // RW setup half-cycle plus 1
      #define SMC_SCR_RWSETUP_2          0x02000000                                       // RW setup half-cycle plus 2
      #define SMC_SCR_RWSETUP_3          0x03000000                                       // RW setup half-cycle plus 3
      #define SMC_SCR_RWSETUP_4          0x04000000                                       // RW setup half-cycle plus 4
      #define SMC_SCR_RWSETUP_5          0x05000000                                       // RW setup half-cycle plus 5
      #define SMC_SCR_RWSETUP_6          0x06000000                                       // RW setup half-cycle plus 6
      #define SMC_SCR_RWSETUP_7          0x07000000                                       // RW setup half-cycle plus 7
      #define SMC_SCR_RWHOLD_0           0x00000000                                       // RW hold 0 cycles
      #define SMC_SCR_RWHOLD_1           0x10000000                                       // RW hold 1 cycle
      #define SMC_SCR_RWHOLD_2           0x20000000                                       // RW hold 2 cycles
      #define SMC_SCR_RWHOLD_3           0x30000000                                       // RW hold 3 cycles
      #define SMC_SCR_RWHOLD_4           0x40000000                                       // RW hold 4 cycles
      #define SMC_SCR_RWHOLD_5           0x50000000                                       // RW hold 5 cycles
      #define SMC_SCR_RWHOLD_6           0x60000000                                       // RW hold 6 cycles
      #define SMC_SCR_RWHOLD_7           0x70000000                                       // RW hold 7 cycles
    #define SMC_CSR1                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x04)   // SMC Chip Select Register 1
    #define SMC_CSR2                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x08)   // SMC Chip Select Register 2
    #define SMC_CSR3                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x0c)   // SMC Chip Select Register 3
    #define SMC_CSR4                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x10)   // SMC Chip Select Register 4
    #define SMC_CSR5                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x14)   // SMC Chip Select Register 5
    #define SMC_CSR6                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x18)   // SMC Chip Select Register 6
    #define SMC_CSR7                     *(unsigned long*)(SMC_PERIPHERAL_BLOCK + 0x1c)   // SMC Chip Select Register 7
#endif

// Error Corrected Code Controller
//
#ifdef _HW_SAM7SE                                                        // {46}
    #define ECC_CR                       *(volatile unsigned long*)(ECC_PERIPHERAL_BLOCK + 0x00) // ECC Control Register (write-only)
      #define EEC_CR_RST                 0x00000001
    #define ECC_MR                       *(unsigned long*)(ECC_PERIPHERAL_BLOCK + 0x04)   // ECC Mode Register 
      #define ECC_MR_NAND_PAGE_SIZE_528  0x00000000
      #define ECC_MR_NAND_PAGE_SIZE_1056 0x00000001
      #define ECC_MR_NAND_PAGE_SIZE_2112 0x00000002
      #define ECC_MR_NAND_PAGE_SIZE_4224 0x00000003
    #define ECC_SR                       *(volatile unsigned long*)(ECC_PERIPHERAL_BLOCK + 0x08) // ECC Status Register (read-only)
      #define RECERR                     0x00000001                                       // Error detected - whether recoverable depends on the MULERR bit
      #define ECCERR                     0x00000002                                       // Single bit error in the ECC bytes
      #define MULERR                     0x00000004                                       // Multiple errors dected - not correctable
    #define ECC_PR                       *(volatile unsigned long*)(ECC_PERIPHERAL_BLOCK + 0x0c) // ECC Parity Register (read-only)
      #define ECC_PR_WORDADDR_MASK       0x0000fff0
      #define ECC_PR_WORDADDR_SHIFT      4
      #define ECC_PR_BITADDR_MASK        0x0000000f
    #define ECC_NPR                      *(volatile unsigned long*)(ECC_PERIPHERAL_BLOCK + 0x10) // ECC NParity Register (read-only)
#endif


// Timers
//
#define TC_CCR_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x0) // {15} Channel Control Register (write only)
  #define TIM_CLKEN                      0x00000001                                       // enable timer
  #define TIM_CLKDIS                     0x00000002                                       // disable timer
  #define TIM_SWTRG                      0x00000004                                       // software trigger (reset and start)
#define TC_CMR_0                         *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x4) // Channel Mode Register (read/write)
  // Waveform mode
  #define TIMER_CLOCK_1                  0x00000000                                       // MCK/2
  #define TIMER_CLOCK_2                  0x00000001                                       // MCK/8
  #define TIMER_CLOCK_3                  0x00000002                                       // MCK/32
  #define TIMER_CLOCK_4                  0x00000003                                       // MCK/128
  #define TIMER_CLOCK_5                  0x00000004                                       // MCK/1024
  #define CPCSTOP                        0x00000040                                       // Counter Clock stopped with RC compare
  #define CPCDIS                         0x00000080                                       // Counter Clock disable with RC compare
  #define WAVSEL_UP_TRG_RC               0x00004000                                       // up mode with automatic trigger on RC compare
  #define WAVE                           0x00008000                                       // enable waveform mode
  #define ACPA_RA_SET                    0x00010000
  #define ACPC_RC_CLEAR                  0x00080000
  #define ACPC_RC_TOGGLE                 0x000c0000
  // Capture mode                                                                         {42}
  #define CLOCK_SOURCE_XC0               0x00000005                                       // XC0 clock selection
  #define CLOCK_SOURCE_XC1               0x00000006                                       // XC1 clock selection
  #define CLOCK_SOURCE_XC2               0x00000007                                       // XC2 clock selection
  #define CLKI                           0x00000008                                       // counter incremented on falling clock edge rather than rising edge
  #define BURSTXC0                       0x00000010                                       // XC0 ANDed with selected clock source
  #define BURSTXC1                       0x00000020                                       // XC1 ANDed with selected clock source
  #define BURSTXC2                       0x00000030                                       // XC2 ANDed with selected clock source
  #define LDBSTOP                        0x00000040                                       // counter  clock is stopped when RB loading occurs
  #define LDBDIS                         0x00000080                                       // counter  clock is disabled when RB loading occurs
  #define ETRGEDG_RISING                 0x00000100                                       // external trigger on rising edge
  #define ETRGEDG_FALLING                0x00000200                                       // external trigger on falling edge
  #define ETRGEDG_BOTH_EDGES             0x00000300                                       // external Trigger on both edge
  #define ABETRG                         0x00000400                                       // TIOA is used as trigger input (else TIOB)
  #define CPCTRG                         0x00004000                                       // RC compare trigger enable
  #define LDRA_RISING_TIOA               0x00010000                                       // RA loading on rising edge of TIOA
  #define LDRA_FALLING_TIOA              0x00020000                                       // RA loading on falling edge of TIOA
  #define LDRA_BOTH_EDGES_TIOA           0x00030000                                       // RA loading on both edges of TIOA
  #define LDRB_RISING_TIOA               0x00040000                                       // RB loading on rising edge of TIOA
  #define LDRB_FALLING_TIOA              0x00080000                                       // RB loading on falling edge of TIOA
  #define LDRB_BOTH_EDGES_TIOA           0x000c0000                                       // RB loading on both edges of TIOA
#define TC_CV_0                          *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x10) // Counter Value (read only)
#define TC_RA_0                          *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x14) // Register A (read/write)
#define TC_RB_0                          *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x18) // Register B (read/write)
#define TC_RC_0                          *(unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x1c) // Register C (read/write)
#define TC_SR_0                          *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x20) // Status Register (read only)
  #define TIM_COVSF                      0x00000001                                       // Counter Overflow Status
  #define TIM_LOVRS                      0x00000002                                       // Load Overrun Status
  #define TIM_CPAS                       0x00000004                                       // RA Compare Status
  #define TIM_CPBS                       0x00000008                                       // RB Compare Status
  #define TIM_CPCS                       0x00000010                                       // RC Compare Status
  #define TIM_LDRAS                      0x00000020                                       // RA Loading Status
  #define TIM_LDRBS                      0x00000040                                       // RB Loading Status
  #define TIM_ETRGS                      0x00000080                                       // External Trigger Status
  #define TIM_CLKSTA                     0x00010000                                       // Clock is enabled
  #define TIM_MTIOA                      0x00020000                                       // MTIOA Mirror
  #define TIM_MTIOB                      0x00040000                                       // MTIOB Mirror
#define TC_IER_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x24) // {15} Interrupt Enable Register (write only)
#define TC_IDR_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x28) // {15} Interrupt Disable Register (write only)
#define TC_IMR_0                         *(volatile unsigned long*)(TIMER0_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)

#define TC_CCR_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x0) // {15} Channel Control Register (write only)
#define TC_CMR_1                         *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x4) // Channel Mode Register (read/write)
#define TC_CV_1                          *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x10) // Counter Value (read only)
#define TC_RA_1                          *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x14) // Register A (read/write)
#define TC_RB_1                          *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x18) // Register B (read/write)
#define TC_RC_1                          *(unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x1c) // Register C (read/write)
#define TC_SR_1                          *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x20) // Status Register (read only)
#define TC_IER_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x24) // {15} Interrupt Enable Register (write only)
#define TC_IDR_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x28) // {15} Interrupt Disable Register (write only)
#define TC_IMR_1                         *(volatile unsigned long*)(TIMER1_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)

#define TC_CCR_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x0) // {15} Channel Control Register (write only)
#define TC_CMR_2                         *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x4) // Channel Mode Register (read/write)
#define TC_CV_2                          *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x10) // Counter Value (read only)
#define TC_RA_2                          *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x14) // Register A (read/write)
#define TC_RB_2                          *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x18) // Register B (read/write)
#define TC_RC_2                          *(unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x1c) // Register C (read/write)
  #define LOW_RES_MS                     (((2*MASTER_CLOCK/1024000) + 1)/2)                // slowest clock rate to timer, giving longest period but minimum resolution
  #define MED_RES_MS                     (((2*MASTER_CLOCK/128000) + 1)/2)                 // medium clock rate to timer, giving medium period and medium resolution
  #define HIGH_RES_MS                    (((2*MASTER_CLOCK/32000) + 1)/2)                  // high clock rate to timer, giving minimum period and maximum resolution
#define TC_SR_2                          *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x20) // Status Register (read only)
#define TC_IER_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x24) // {15} Interrupt Enable Register (write only)
#define TC_IDR_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x28) // {15} Interrupt Disable Register (write only)
#define TC_IMR_2                         *(volatile unsigned long*)(TIMER2_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)

#define TC_BCR                           *(volatile unsigned long*)(TIMER_CONTROL_BLOCK + 0x0) // {15} TC Block Control Register (write only)
#define TC_BMR                           *(unsigned long*)(TIMER_CONTROL_BLOCK + 0x4)      // TC Block Mode Register (read/write)
  #define BMR_XC0_MASK                   0x0003                                            // {42}
  #define BMR_XC1_MASK                   0x000c
  #define BMR_XC2_MASK                   0x0030
  #define XC0_TCLK0                      0x0000
  #define XC0_TIOA1                      0x0002
  #define XC0_TIOA2                      0x0003
  #define XC1_TCLK1                      0x0000
  #define XC1_TIOA0                      0x0008
  #define XC1_TIOA2                      0x000c
  #define XC2_TCLK2                      0x0000
  #define XC2_TIOA0                      0x0020
  #define XC2_TIOA1                      0x0030

#define _COUNTER_VALUE(timer)            TC_CV_##timer

// USB Interface                                                         {4}
//
#define UDP_FRM_NUM                      *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x00) // Frame Number Register (read only)
  #define FRM_NUM_MASK                   0x000007ff
  #define FRM_ERR                        0x00010000
  #define FRM_OK                         0x00020000
#define UDP_GBL_STAT                     *(unsigned long*)(USB_PERIPHERAL_BLOCK + 0x04) // Global State Register
  #define FADDEN                         0x00000001                      // Function Address Enabled (write one only)
  #define CONFG                          0x00000001                      // Device configured
#define UDP_FADDR                        *(unsigned long*)(USB_PERIPHERAL_BLOCK + 0x08) // Function Address Register
  #define FADD_MASK                      0x0000007f
  #define FEN                            0x00000100                      // Function Enable bit
#define UDP_IER                          *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x10) // Interrupt Enable Register (write only)
  #define EP0INT                         0x00000001                      // Endpoint 0 interrupt
  #define EP1INT                         0x00000002                      // Endpoint 1 interrupt
  #define EP2INT                         0x00000004                      // Endpoint 2 interrupt
  #define EP3INT                         0x00000008                      // Endpoint 3 interrupt
  #define EP4INT                         0x00000010                      // Endpoint 4 interrupt
  #define EP5INT                         0x00000020                      // Endpoint 5 interrupt
  #define EP6INT                         0x00000040                      // Endpoint 6 interrupt (SAM7SE)
  #define EP7INT                         0x00000080                      // Endpoint 7 interrupt (SAM7SE)
  #define RXSUSP                         0x00000100                      // UDP suspend interrupt
  #define RXRSM                          0x00000200                      // UDP resume interrupt
  #define SOFINT                         0x00000800                      // Start of Frame interrupt
  #define BUS_WAKEUP                     0x00002000                      // UDP Bus Wakeup interrupt
#define UDP_IDR                          *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x14) // Interrupt Disable Register (write only)
#define UDP_IMR                          *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x18) // Interrupt Mask Register (read only)
#define UDP_ISR                          *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x1c) // Interrupt Status Register (read only)
  #define ENDBUSRES                      0x00001000                      // End of bus reset interrupt status
#define UDP_ICR                          *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x20) // Interrupt Clear Register (write only)
  #define RXSUSP_CLR                     0x00000100                      // UDP suspend clear
  #define RXRSM_CLR                      0x00000200                      // UDP resume clear
  #define SOFINT_CLR                     0x00000800                      // Start of Frame clear
  #define ENDBUSRES_CLR                  0x00001000                      // End of bus reset clear
  #define BUS_WAKEUP_CLR                 0x00002000                      // UDP Bus Wakeup clear
#define UDP_RST_EP                       *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x28) // Reset Endpoint Register
  #define EP0RST                         0x00000001                      // Reset Endpoint 0
  #define EP1RST                         0x00000002                      // Reset Endpoint 1
  #define EP2RST                         0x00000004                      // Reset Endpoint 2
  #define EP3RST                         0x00000008                      // Reset Endpoint 3
  #define EP4RST                         0x00000010                      // Reset Endpoint 4
  #define EP5RST                         0x00000020                      // Reset Endpoint 5
  #define EP6RST                         0x00000040                      // Reset Endpoint 6 (SAM7SE)
  #define EP7RST                         0x00000080                      // Reset Endpoint 7 (SAM7SE)
#define UDP_CSR0_ADD                     (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x30)  // Endpoint 0 Control and Status Register Address
#define UDP_CSR0                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x30) // Endpoint 0 Control and Status Register
  #define TXCOMP                         0x00000001                      // IN transfer complete (set by USB, cleared by firmware)
  #define RX_DATA_BK0                    0x00000002                      // Data packet received and stored in bank 0 (set by USB, cleared by firmware)
  #define RXSETUP                        0x00000004                      // SETUP packet received and stored (set by USB, cleared by firmware)
  #define STALLSENT                      0x00000008                      // Host has aknowledged a stall (set by USB, cleared by firmware)
  #define ISOERROR                       STALLSENT                       // Isochronous receive data in input FIFO is corrupt - CRC-error (set by USB, cleared by firmware)
  #define TXPKTRDY                       0x00000010                      // Transmit packet ready to be sent on next IN transfer (set by firmware, cleared by USB)
  #define FORCESTALL                     0x00000020                      // Send stall (set by firmware, cleared by USB)
  #define RX_DATA_BK1                    0x00000040                      // Data packet received and stored in bank 1 (set by USB, cleared by firmware)
  #define UDP_DIR                        0x00000080                      // Data IN transfers enabled in control data stage
  #define EPTYPE_CONTROL                 0x00000000                      // Control endpoint type
  #define EPTYPE_ISO_OUT                 0x00000100                      // Isochronous out endpoint type
  #define EPTYPE_ISO_IN                  0x00000500                      // Isochronous in endpoint type
  #define EPTYPE_BULK_OUT                0x00000200                      // Bulk out endpoint type
  #define EPTYPE_BULK_IN                 0x00000600                      // Bulk in endpoint type
  #define EPTYPE_INTERRUPT_OUT           0x00000300                      // Interrupt out endpoint type
  #define EPTYPE_INTERRUPT_IN            0x00000700                      // Interrupt in endpoint type
  #define EPTYPE_IN                      0x00000400
  #define EPTYPE_MASK                    0x00000700
  #define DTGLE                          0x00000800                      // Data 1 packet identifier (read only bit)
  #define EPEDS                          0x00008000                      // Endpoint Enabled
  #define RXBYTECNT_MASK                 0x07ff0000                      // Number of bytes available in the FIFO (read only)
  #define RXBYTECNT_SHIFT                16
#define UDP_CSR1_ADD                     (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x34)  // Endpoint 1 Control and Status Register Address
#define UDP_CSR1                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x34) // Endpoint 1 Control and Status Register
#define UDP_CSR2_ADD                     (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x38)  // Endpoint 2 Control and Status Register Address
#define UDP_CSR2                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x38) // Endpoint 2 Control and Status Register
#define UDP_CSR3_ADD                     (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x3c)  // Endpoint 3 Control and Status Register Address
#define UDP_CSR3                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x3c) // Endpoint 3 Control and Status Register
#define UDP_CSR4_ADD                     (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x40)  // Endpoint 4 Control and Status Register Address
#define UDP_CSR4                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x40) // Endpoint 4 Control and Status Register
#define UDP_CSR5_ADD                     (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x44)  // Endpoint 5 Control and Status Register Address
#define UDP_CSR5                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x44) // Endpoint 5 Control and Status Register
#if defined _HW_SAM7SE
    #define UDP_CSR6_ADD                 (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x48)  // Endpoint 6 Control and Status Register Address
    #define UDP_CSR6                     *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x48) // Endpoint 6 Control and Status Register
    #define UDP_CSR7_ADD                 (volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x4c)  // Endpoint 7 Control and Status Register Address
    #define UDP_CSR7                     *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x4c) // Endpoint 7 Control and Status Register
#endif
#define UDP_FDR0_ADD                     (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x50)  // Endpoint 0 FIFO Data Register Address
#define UDP_FDR0                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x50) // Endpoint 0 FIFO Data Register
#define UDP_FDR1_ADD                     (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x54)  // Endpoint 1 FIFO Data Register Address
#define UDP_FDR1                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x54) // Endpoint 1 FIFO Data Register
#define UDP_FDR2_ADD                     (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x58)  // Endpoint 2 FIFO Data Register
#define UDP_FDR2                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x58) // Endpoint 2 FIFO Data Register
#define UDP_FDR3_ADD                     (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x5c)  // Endpoint 3 FIFO Data Register
#define UDP_FDR3                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x5c) // Endpoint 3 FIFO Data Register
#define UDP_FDR4_ADD                     (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x60)  // Endpoint 4 FIFO Data Register
#define UDP_FDR4                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x60) // Endpoint 4 FIFO Data Register
#define UDP_FDR5_ADD                     (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x64)  // Endpoint 5 FIFO Data Register
#define UDP_FDR5                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x64) // Endpoint 5 FIFO Data Register
#if defined _HW_SAM7SE
    #define UDP_FDR6_ADD                 (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x68)  // Endpoint 6 FIFO Data Register
    #define UDP_FDR6                     *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x68) // Endpoint 6 FIFO Data Register
    #define UDP_FDR7_ADD                 (volatile unsigned char*)(USB_PERIPHERAL_BLOCK + 0x6c)  // Endpoint 7 FIFO Data Register
    #define UDP_FDR7                     *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x6c) // Endpoint 7 FIFO Data Register
#endif
#define UDP_TXVC                         *(volatile unsigned long*)(USB_PERIPHERAL_BLOCK + 0x74) // Transceiver Control Register
  #define UDP_TXVC_TXVDIS                0x00000100                      // Transceiver disable
  #define UDP_TXVC_UDP_PUON              0x00000200                      // pull-up enable (SAM7SE)

#ifdef _WINDOWS
    extern volatile unsigned char *fnGetUSB_FIFOAdd(volatile unsigned char *ptrAdd, int iDirection);
#else
    #define fnGetUSB_FIFOAdd(x,y)          x
#endif

#define PROTECT_USB_TX_BUFFER                                            // use additional tx buffer protection to avoid ping-pong buffer overrun. Register status alone is not reliable.
//#define _SUPRESS_REPEAT_DATA

#define USB_FIFO_BUFFER_DEPTH              2                             // SAM7X has two rx and tx buffers for some endpoints - test reserve of one
#if defined _HW_SAM7SE
    #define NUMBER_OF_USB_ENDPOINTS        8                             // SAM7SE supports 8 endpoints
    #define NUMBER_OF_BUFFERED_ENDPOINT    6                             // endpoints 0 and 3 are single buffered
#else
    #define NUMBER_OF_USB_ENDPOINTS        6                             // SAM7X supports 6 endpoints
    #define NUMBER_OF_BUFFERED_ENDPOINT    4                             // endpoints 0 and 3 are single buffered
#endif
#define USB_FIFO                                                         // SAM7X has USB buffers realised as FIFOs
#define FIFO_TO_QUEUE                                                    // activate queue driver support to allow FIFO copy to a circular buffer
#define ENDPOINT0_LENGTH                   8                             // endpoint 0 has a fixed single-bank 8 byte FIFO which can be used for control, bulk or interrupt types
#define ENDPOINT1_LENGTH                   64                            // endpoint 1 has a fixed dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types
#define ENDPOINT2_LENGTH                   64                            // endpoint 2 has a fixed dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types
#define ENDPOINT3_LENGTH                   64                            // endpoint 3 has a fixed single-bank 64 byte FIFO which can be used for control, bulk or interrupt types
#if defined _HW_SAM7SE
    #define ENDPOINT4_LENGTH               64                            // endpoint 4 has a fixed dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types
    #define ENDPOINT5_LENGTH               64                            // endpoint 5 has a fixed dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types
    #define ENDPOINT6_LENGTH               64                            // endpoint 6 has a fixed dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types
    #define ENDPOINT7_LENGTH               64                            // endpoint 7 has a fixed dual-bank 64 byte FIFO which can be used for bulk, interrupt or isochronous types

    #define MAX_FIFO_LENGTH                ENDPOINT7_LENGTH              // longest endpoint FIFO
#else
    #define ENDPOINT4_LENGTH               256                           // endpoint 4 has a fixed dual-bank 256 byte FIFO which can be used for bulk, interrupt or isochronous types
    #define ENDPOINT5_LENGTH               256                           // endpoint 5 has a fixed dual-bank 256 byte FIFO which can be used for bulk, interrupt or isochronous types
    #define MAX_FIFO_LENGTH                ENDPOINT5_LENGTH              // longest endpoint FIFO
#endif

typedef struct stUSB_END_POINT
{
    unsigned short usEndpointSize;                                       // contains size of endpoint plus some control flags
    unsigned char  ucEndpointRxFlags;                                    // rx flags
    unsigned char  ucEndpointTxFlags;                                    // tx flags
#ifdef _WINDOWS
    unsigned long ulSimFlags;
#endif
} USB_END_POINT;

#define ENDPOINT_DUAL_BANK                 0x01
#define ENDPOINT_RX_BANK1                  0x02
#define ENDPOINT_TX_BANK1                  0x04
#define ENDPOINT_SECOND_BANK_BUSY          0x08
#define ENDPOINT_RX_DATA1                  0x10

#define PUT_BANK_1                         0x00000001
#define GET_BANK_1                         0x00000002

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

#define FNSEND_USB_DATA(buffer_addr, length, Endpoint, hw)      fnSendUSB_FIFO(buffer_addr, length, Endpoint, hw); _SIM_USB(USB_SIM_TX, Endpoint, hw)

// Macros for setting and clearing configuration bits and then waiting until they have been confirmed, due to synchronisation requirement between MCK and UDPCK
// {45} macros corrected to avoid potential race state that could lead to OUT frame loss ({48} if suspend is detected in the loop quite to avoid potential hang)
//
#define SYNCHRONISE_CONTROL_SET(bits)    *ptrUSB_HW->ptr_ulUSB_BDControl = ((*ptrUSB_HW->ptr_ulUSB_BDControl | (RX_DATA_BK0 | RX_DATA_BK1)) | (bits)); while ((*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) != (bits)) { if (UDP_ISR & RXSUSP) break; }
//#define SYNCHRONISE_CONTROL_CLEAR(bits)  *ptrUSB_HW->ptr_ulUSB_BDControl = ((*ptrUSB_HW->ptr_ulUSB_BDControl | (RX_DATA_BK0 | RX_DATA_BK1)) & ~(bits)); while ((*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) == (bits)) { if (UDP_ISR & RXSUSP) break; }
#define SYNCHRONISE_CONTROL_CLEAR(bits)  *ptrUSB_HW->ptr_ulUSB_BDControl = ((*ptrUSB_HW->ptr_ulUSB_BDControl | (RX_DATA_BK0 | RX_DATA_BK1)) & ~(bits)); while ((*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) != 0) { if (UDP_ISR & RXSUSP) break; } // {60} allow clearing multiple bits and waiting until all are 0

#define SYNCHRONISE_CONTROL_SET_(bits)   *usb_hardware.ptr_ulUSB_BDControl = ((*usb_hardware.ptr_ulUSB_BDControl | (RX_DATA_BK0 | RX_DATA_BK1)) | (bits)); while ((*usb_hardware.ptr_ulUSB_BDControl & (bits)) != (bits)) { if (UDP_ISR & RXSUSP) break; }
//#define SYNCHRONISE_CONTROL_CLEAR_(bits) *usb_hardware.ptr_ulUSB_BDControl = ((*usb_hardware.ptr_ulUSB_BDControl | (RX_DATA_BK0 | RX_DATA_BK1)) & ~(bits)); while ((*usb_hardware.ptr_ulUSB_BDControl & (bits)) == (bits)) { if (UDP_ISR & RXSUSP) break; }
#define SYNCHRONISE_CONTROL_CLEAR_(bits) *usb_hardware.ptr_ulUSB_BDControl = ((*usb_hardware.ptr_ulUSB_BDControl | (RX_DATA_BK0 | RX_DATA_BK1)) & ~(bits)); while ((*usb_hardware.ptr_ulUSB_BDControl & (bits)) != 0) { if (UDP_ISR & RXSUSP) break; } // {60} allow clearing multiple bits and waiting until all are 0

//#define SYNCHRONISE_CONTROL_SET(bits)    while ((*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) != (bits)) { *ptrUSB_HW->ptr_ulUSB_BDControl |= (bits);  }
//#define SYNCHRONISE_CONTROL_CLEAR(bits)  while (*ptrUSB_HW->ptr_ulUSB_BDControl & (bits)) { *ptrUSB_HW->ptr_ulUSB_BDControl &= ~(bits); }

//#define SYNCHRONISE_CONTROL_SET_(bits)   while ((*usb_hardware.ptr_ulUSB_BDControl & (bits)) != (bits)) { *usb_hardware.ptr_ulUSB_BDControl |= (bits); }
//#define SYNCHRONISE_CONTROL_CLEAR_(bits) while (*usb_hardware.ptr_ulUSB_BDControl & (bits)) { *usb_hardware.ptr_ulUSB_BDControl &= ~(bits); }

#define VALIDATE_NEW_CONFIGURATION()                                     // {44}

// Macro for sending a zero data token
// start the transmission and wait for the flag to be set, when simulating mark that the data buffer is empty
//
#ifdef _WINDOWS
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref) \
    { \
        SYNCHRONISE_CONTROL_SET(TXPKTRDY); \
        *ptrUSB_HW->ptrDatBuffer = 0; \
        *(ptrUSB_HW->ptrDatBuffer + 1) = 0; \
        _SIM_USB(USB_SIM_TX, iEndpoint_ref, ptrUSB_HW); \
    }
#else
  //#define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref) while (*ptrUSB_HW->ptr_ulUSB_BDControl & TXPKTRDY) { if (UDP_ISR & RXSUSP) break; } SYNCHRONISE_CONTROL_SET(TXPKTRDY)
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint_ref) SYNCHRONISE_CONTROL_SET(TXPKTRDY)
#endif

#define SET_CONTROL_DIRECTION(ptrUSB_HW, ucFirstByte) if (ucFirstByte & STANDARD_DEVICE_TO_HOST) {SYNCHRONISE_CONTROL_SET(UDP_DIR);}
#define CLEAR_SETUP(ptrUSB_HW)           SYNCHRONISE_CONTROL_CLEAR(RXSETUP)

// I2C Interface
//
#define TWI_CR                           *(volatile unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x00) // Control Register (write only)
  #define TWI_START                      0x01
  #define TWI_STOP                       0x02
  #define TWI_MSEN                       0x04
  #define TWI_MSDIS                      0x08
  #define TWI_SWRST                      0x80
#define TWI_MMR                          *(unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x04)    // Master Mode Register (read/write)
  #define BYTE_COMMAND_PROT              0x00000000
  #define ONE_BYTE_INTERNAL_ADDRESS      0x00000100
  #define TWO_BYTE_INTERNAL_ADDRESS      0x00000200
  #define THREE_BYTE_INTERNAL_ADDRESS    0x00000300
  #define TWI_MREAD                      0x00001000
  #define TWI_ADDRESS_SHIFT              16
#define TWI_IADR                         *(unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x0c)    // Internal Address Register (read/write)
#define TWI_CWGR                         *(unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x10)    // Clock Waveform Generator Register (read/write)
#define TWI_SR                           *(volatile unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x20)// Status Register (read only)
  #define TWI_TXCOMP                     0x00000001
  #define TWI_RXRDY                      0x00000002
  #define TWI_TXRDY                      0x00000004
  #define TWI_OVRE                       0x00000040
  #define TWI_UNRE                       0x00000080
  #define TWI_NACK                       0x00000100
#define TWI_IER                          *(volatile unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x24) // {15} Interrupt Enable Register (write only)
#define TWI_IDR                          *(volatile unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x28) // {15} Interrupt Disable Register (write only)
#define TWI_IMR                          *(volatile unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x2c) // Interrupt Mask Register (read only)
#define TWI_RHR                          *(volatile unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x30) // Receive Holding Register (read only)
#define TWI_THR                          *(unsigned long*)(TWI_PERIPHERAL_BLOCK + 0x34)    // Transmit Holding Register (read/write)

// Serial interfaces
//
#define US_CR_0                          *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x0)  // {15} Control register (write only)
  #define SAM7_TX_ON                     0x00000040
  #define SAM7_RX_ON                     0x00000010
  #define SAM7_TX_OFF                    0x00000080
  #define SAM7_RX_OFF                    0x00000020
  #define SAM7_START_BRK                 0x00000200
  #define SAM7_STOP_BRK                  0x00000400
  #define SAM7X_STTTO                    0x00000800                      // {26} start timeout
  #define SAM7X_RETTO                    0x00008000                      // {26} retrigger timeout
  #define SAM7_RTSEN                     0x00040000                      // {24} assert RTS line
  #define SAM7_RTSDIS                    0x00080000                      // {24} negate RTS line
#define US_MR_0                          *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x4)  // Mode register (read/write)
  #define US_MR_OFFSET                   0x04
  #define SAM7_NO_PARITY                 0x00000800
  #define SAM7_ODD_PARITY                0x00000200
  #define SAM7_EVEN_PARITY               0x00000000
  #define SAM7_2_STOPS                   0x00002000
  #define SAM7_ONE_HALF_STOPS            0x00001000
  #define SAM7_CHAR_7                    0x00000080
  #define SAM7_CHAR_8                    0x000000c0
  #define USART_MODE_MASK                0x0000000f                      // {25}
  #define USART_MODE_NORMAL              0x00000000
  #define USART_MODE_RS485               0x00000001
  #define USART_MODE_HW_HANDSHAKE        0x00000002
  #define USART_MODE_MODEM               0x00000003
  #define USART_MODE_ISO7816_T_0         0x00000004
  #define USART_MODE_ISO7816_T_1         0x00000006
  #define USART_MODE_IrDA                0x00000008
  #define USART_MODE9                    0x00020000                      // {51}
#define US_IER_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x8) // {15} Interrupt enable register (write only)
#define US_IDR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0xc) // {15} Interrupt Disable register (write only)
#define US_IMR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x10)// {15} Interrupt Mask Register (read only)
  #define RXRDY                          0x00000001                      // enable receive character ready interrupt
  #define TXRDY                          0x00000002                      // enable transmitter ready interrupt
  #define ENDRX                          0x00000008                      // enable end of receive transfer interrupt
  #define ENDTX                          0x00000010                      // enable end of transmit transfer interrupt
  #define RX_TIMEOUT                     0x00000100                      // {26} enable idle timeout interrupt
  #define CTSIC                          0x00080000                      // {27} CTS input change interrupt enable
#define US_CSR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
  #define CTS_HIGH                       0x00800000                      // {27}{33} State of CTS input
#define US_RHR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x18)// {15} Receiver Holding Register (read only)
  #define US_RHR_RXCHR                   0x00000100                      // last character received if RXRDY is set {51}
  #define US_RHR_RXSYNH                  0x00008000                      // last character received is a command {51}
#define US_THR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x1c)// {15} Transmitter Holding Register (write only)
  #define US_THR_TXCHR                   0x00000100                      // next character to be transmitted if TXRDY is not set {51}
  #define US_THR_TXSYNH                  0x00008000                      // next character set is a command {51}
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
#define US_NER_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x44)// {15} Number of Errors Register (read only)
#define US_RES6_0                        *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x48)// Reserved
#define US_IF_0                          *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
#define US_MAN_0                         *(unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)

#define PDC_CHANNEL_OFFSET               0x100                                             // Peripheral DMA Controller starts always this offset from the peripheral control block start
#define US_RPR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x100)// {15} PDC Receive Pointer Register {2}
#define US_RCR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x104)// {15} PDC Receive Count Register
#define US_TPR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x108)// {15} PDC Transmit Pointer Register
#define US_TCR_0                         *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x10c)// {15} PDC Transmit Count Register
#define US_RNPR_0                        *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x110)// {15} PDC Receive Next Pointer Register
#define US_RNCR_0                        *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x114)// {15} PDC Receive Next Count Register
#define US_TNPR_0                        *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x118)// {15} PDC Transmit Next Pointer Register
#define US_TNCR_0                        *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x11c)// {15} PDC Transmit Next Count Register
#define US_PTCR_0                        *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x120)// {15} PDC Transfer Control Register (write only)
  #define PDC_RXTEN                      0x00000001
  #define PDC_RXTDIS                     0x00000002
  #define PDC_TXTEN                      0x00000100
  #define PDC_TXTDIS                     0x00000200
#define US_PTSR_0                        *(volatile unsigned long*)(USART0_PERIPHERAL_BLOCK + 0x124)// {15} PDC Transfer Status Register (read only)

#define US_CR_1                          *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x0) // {15} Control register (write only)
#define US_MR_1                          *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x4) // Mode register (read/write)
#define US_IER_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x8) // {15} Interrupt enable register (write only)
#define US_IDR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0xc) // {15} Interrupt Disable register (write only)
#define US_IMR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x10)// {15} Interrupt Mask Register (read only)
#define US_CSR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
#define US_RHR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x18)// {15} Receiver Holding Register (read only)
#define US_THR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x1c)// {15} Transmitter Holding Register (write only)
#define US_BRGR_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x20)// Baud Rate Generator Register (read/write)
#define US_RTOR_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x24)// Receive Timeout Register (read/write)
#define US_TTGR_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x28)// Transmitter Time Guard Register (read/write)
#define US_RES1_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x2c)// Reserved
#define US_RES2_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x30)// Reserved
#define US_RES3_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x34)// Reserved
#define US_RES4_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x38)// Reserved
#define US_RES5_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x3c)// Reserved
#define US_FIDI_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x40)// Transmitter Time Guard Register (read/write)
#define US_NER_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x44)// {15} Number of Errors Register (read only)
#define US_RES6_1                        *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x48)// Reserved
#define US_IF_1                          *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x4c)// IrDA Filter Register (read/write)
#define US_MAN_1                         *(unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x50)// Manchester Encoder Decoder Register (read/write)

#define US_RPR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x100)// {15} PDC Receive Pointer Register {2}
#define US_RCR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x104)// {15} PDC Receive Count Register
#define US_TPR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x108)// {15} PDC Transmit Pointer Register
#define US_TCR_1                         *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x10c)// {15} PDC Transmit Count Register
#define US_RNPR_1                        *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x110)// {15} PDC Receive Next Pointer Register
#define US_RNCR_1                        *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x114)// {15} PDC Receive Next Count Register
#define US_TNPR_1                        *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x118)// {15} PDC Transmit Next Pointer Register
#define US_TNCR_1                        *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x11c)// {15} PDC Transmit Next Count Register
#define US_PTCR_1                        *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x120)// {15} PDC Transfer Control Register (write only)
#define US_PTSR_1                        *(volatile unsigned long*)(USART1_PERIPHERAL_BLOCK + 0x124)// {15} PDC Transfer Status Register (read only)


// Macro to modify the USART receive idle time                           // {56}
// - set new idle time and reset (the timeout doesn't need a following character to trigger it)
#define _UART_INTERSPACE_RX(usart_channel, idle_time_in_baud_periods)  US_RTOR_##usart_channel = idle_time_in_baud_periods; US_CR_##usart_channel = (SAM7X_STTTO | SAM7X_RETTO); _SIM_PORTS

// Debug Unit (UART 2)                                                                     // {8}
//
#define DBGU_CR                          *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x0) // {15} Control register (write only)
#define DBGU_MR                          *(unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x4)    // Mode register (read/write)
#define DBGU_IER                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x8) // {15} Interrupt enable register (write only)
#define DBGU_IDR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0xc) // {15} Interrupt Disable register (write only)
#define DBGU_IMR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x10)// {15} Interrupt Mask Register (read only)
#define DBGU_SR                          *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x14)// Channel Status Register (read only)
#define DBGU_RHR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x18)// {15} Receiver Holding Register (read only)
#define DBGU_THR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x1c)// {15} Transmitter Holding Register (write only)
#define DBGU_BRGR                        *(unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x20)   // Baud Rate Generator Register (read/write)

#define DBGU_CIDR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x40) // {15} Chip ID Register (read only)
  #define NVPTYPE_EMBEDDED_FLASH_MEMORY  0x20000000
  #define ARCH_AT91SAM7X_SERIES          0x07500000
  #define SRAM_SIZ_32K                   0x000a0000
  #define SRAM_SIZ_64K                   0x000b0000
  #define SRAM_SIZ_128K                  0x000c0000
  #define SRAM_SIZ_256K                  0x000d0000
  #define NVPSIZ_64K                     0x00000500
  #define NVPSIZ_128K                    0x00000700
  #define NVPSIZ_256K                    0x00000900
  #define NVPSIZ_512K                    0x00000a00
  #define EPROC_ARM7TDMI                 0x00000040
  #define VERSION_ID                     0x00000000
#define DBGU_EXID                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x44) // {15} Chip ID Extension Register (read only)
#define DBGU_FNR                         *(unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x48)   // Force NTRST (read/write)

#define DBGU_RPR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x100) // {15} PDC Receive Pointer Register {2}
#define DBGU_RCR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x104) // {15} PDC Receive Count Register
#define DBGU_TPR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x108) // {15} PDC Transmit Pointer Register
#define DBGU_TCR                         *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x10c) // {15} PDC Transmit Count Register
#define DBGU_RNPR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x110) // {15} PDC Receive Next Pointer Register
#define DBGU_RNCR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x114) // {15} PDC Receive Next Count Register
#define DBGU_TNPR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x118) // {15} PDC Transmit Next Pointer Register
#define DBGU_TNCR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x11c) // {15} PDC Transmit Next Count Register
#define DBGU_PTCR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x120) // {15} PDC Transfer Control Register (write only)
#define DBGU_PTSR                        *(volatile unsigned long*)(DBGU_PERIPHERAL_BLOCK + 0x124) // {15} PDC Transfer Status Register (read only)

// PWM Controller                                                        // {37}
//
#define PWM_MR                           *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x000) // PWM Mode Register
#define PWM_ENA                          *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x004) // PWM Enable Register (write-only)
#define PWM_DIS                          *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x008) // PWM Disable Register (write-only)
#define PWM_SR                           *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x00c) // PWM Status Register (read-only)
#define PWM_IER                          *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x010) // PWM Interrupt Enable Register (write-only)
#define PWM_IDR                          *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x014) // PWM Interrupt Disable Register (write-only)
#define PWM_IMR                          *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x018) // PWM Interrupt Mask Register (read-only)
#define PWM_ISR                          *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x01c) // PWM Interrupt Status Register (read-only)

#define PWM_CMR_ADD                      (unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x200)
#define PWM_CMR_0                        *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x200) // PWM Channel 0 Mode Register
  #define PWM_CALG                       0x00000100                                       // period center aligned (not left aligned)
  #define PWM_CPOL                       0x00000200                                       // output waveform starts at high level
  #define PWM_CPD                        0x00000400                                       // update period (rather than duty cycle) on next period start
#define PWM_CDTY_0                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x204) // PWM Channel 0 Duty Cycle Register
#define PWM_CPRD_0                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x208) // PWM Channel 0 Period Register
#define PWM_CCNT_0                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x20c) // PWM Channel 0 Counter Register (read-only)
#define PWM_CUPD_0                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x210) // PWM Channel 0 Update Register (write-only)

#define PWM_CMR_1                        *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x220) // PWM Channel 1 Mode Register
#define PWM_CDTY_1                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x224) // PWM Channel 1 Duty Cycle Register
#define PWM_CPRD_1                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x228) // PWM Channel 1 Period Register
#define PWM_CCNT_1                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x22c) // PWM Channel 1 Counter Register (read-only)
#define PWM_CUPD_1                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x230) // PWM Channel 1 Update Register (write-only)

#define PWM_CMR_2                        *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x240) // PWM Channel 2 Mode Register
#define PWM_CDTY_2                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x244) // PWM Channel 2 Duty Cycle Register
#define PWM_CPRD_2                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x248) // PWM Channel 2 Period Register
#define PWM_CCNT_2                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x24c) // PWM Channel 2 Counter Register (read-only)
#define PWM_CUPD_2                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x250) // PWM Channel 2 Update Register (write-only)

#define PWM_CMR_3                        *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x260) // PWM Channel 3 Mode Register
#define PWM_CDTY_3                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x264) // PWM Channel 3 Duty Cycle Register
#define PWM_CPRD_3                       *(unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x268) // PWM Channel 3 Period Register
#define PWM_CCNT_3                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x26c) // PWM Channel 3 Counter Register (read-only)
#define PWM_CUPD_3                       *(volatile unsigned long*)(PWMC_PERIPHERAL_BLOCK + 0x270) // PWM Channel 3 Update Register (write-only)


// SSC block                                                             // {29}
//
#define SSC_CR                           *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x0) // Control Register (write-only)
  #define SSC_RXEN                       0x0001
  #define SSC_RXDIS                      0x0002
  #define SSC_TXEN                       0x0100
  #define SSC_TXDIS                      0x0200
  #define SSC_SWRST                      0x8000
#define SSC_CMR                          *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x4)          // Clock Mode register

#define SSC_RCMR                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x10)         // Receive Clock Mode Register
  #define SSC_RX_CLK_INTERNAL            0x0000
  #define SSC_RX_CLK_TX_CLK              0x0001
  #define SSC_RX_CLK_RX_CLK_PIN          0x0002
  #define SSC_RX_CLK_OUT_NONE            0x0000
  #define SSC_RX_CLK_OUT_CONTINOUS       0x0004
  #define SSC_RX_CLK_OUT_DATA_TRANS      0x0008
  #define SSC_RX_CLK_INVERTED            0x0020
  #define SSC_RX_CLK_NO_GATING           0x0000
  #define SSC_RX_CLK_WITH_RF_LOW         0x0040
  #define SSC_RX_CLK_WITH_RF_HIGH        0x0080
  #define SSC_RX_START_CONTINUOUS        0x0000
  #define SSC_RX_START_TX_START          0x0100
  #define SSC_RX_START_RF_LOW            0x0200
  #define SSC_RX_START_RF_HIGH           0x0300
  #define SSC_RX_START_RF_FALLING        0x0400
  #define SSC_RX_START_RF_RISING         0x0500
  #define SSC_RX_START_RF_ANY_LEVEL      0x0600
  #define SSC_RX_START_RF_ANY_EDGE       0x0700
  #define SSC_RX_START_COMPARE_0         0x0800
  #define SSC_RX_STOP_COMPARE            0x1000
  #define SSC_RX_START_DELAY_SHIFT       16
  #define SSC_RX_PERIOD_SHIFT            24
#define SSC_RFMR                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x14)         // Receive Frame Mode Register
  #define SSC_RX_LOOP_ENABLE             0x0020
  #define SSC_RX_MSB_FIRST               0x0080
  #define SSC_RX_DATA_PER_FRAME_SHIFT    8
  #define SSC_RX_FSLEN_SHIFT             16
  #define SSC_RF_OUT_NEG_PULSE           0x00100000
  #define SSC_RF_OUT_POS_PULSE           0x00200000
  #define SSC_RF_OUT_LOW_ON_TRANSFER     0x00300000
  #define SSC_RF_OUT_HIGH_ON_TRAMSFER    0x00400000
  #define SSC_RF_OUT_TOGGLING            0x00500000
  #define SSC_RF_POS_EDGE_DETECTION      0x00000000
  #define SSC_RF_NEG_EDGE_TEDECTION      0x01000000
#define SSC_TCMR                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x18)         // Transmit Clock Mode register
  #define SSC_TX_CLK_INTERNAL            0x0000
  #define SSC_TX_CLK_RX_CLK              0x0001
  #define SSC_TX_CLK_TX_CLK_PIN          0x0002
  #define SSC_TX_CLK_OUT_NONE            0x0000
  #define SSC_TX_CLK_OUT_CONTINOUS       0x0004
  #define SSC_TX_CLK_OUT_DATA_TRANS      0x0008
  #define SSC_TX_CLK_INVERTED            0x0020
  #define SSC_TX_CLK_NO_GATING           0x0000
  #define SSC_TX_CLK_WITH_TF_LOW         0x0040
  #define SSC_TX_CLK_WITH_TF_HIGH        0x0080
  #define SSC_TX_START_CONTINUOUS        0x0000
  #define SSC_TX_START_RX_START          0x0100
  #define SSC_TX_START_TF_LOW            0x0200
  #define SSC_TX_START_TF_HIGH           0x0300
  #define SSC_TX_START_TF_FALLING        0x0400
  #define SSC_TX_START_TF_RISING         0x0500
  #define SSC_TX_START_TF_ANY_LEVEL      0x0600
  #define SSC_TX_START_TF_ANY_EDGE       0x0700
  #define SSC_TX_START_DELAY_SHIFT       16
  #define SSC_TX_PERIOD_SHIFT            24
#define SSC_TFMR                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x1c)         // Transmit Frame Mode Register
  #define SSC_TX_PIN_DEFAULT_LEVEL_HIGH  0x0020
  #define SSC_TX_PIN_DEFAULT_LEVEL_LOW   0x0000
  #define SSC_TX_MSB_FIRST               0x0080
  #define SSC_TX_LSB_FIRST               0x0000
  #define SSC_TX_DATA_PER_FRAME_SHIFT    8
  #define SSC_TX_FSLEN_SHIFT             16
  #define SSC_TF_OUT_NEG_PULSE           0x00100000
  #define SSC_TF_OUT_POS_PULSE           0x00200000
  #define SSC_TF_OUT_LOW_ON_TRANSFER     0x00300000
  #define SSC_TF_OUT_HIGH_ON_TRANSFER    0x00400000
  #define SSC_TF_OUT_TOGGLING            0x00500000
  #define SSC_TF_BEFORE_DATA             0x00000000
  #define SSC_TF_WITH_DATA               0x00800000
  #define SSC_TF_POS_EDGE_DETECTION      0x00000000
  #define SSC_TF_NEG_EDGE_TEDECTION      0x01000000
#define SSC_RHR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x20) // Receive Holding Register (read-only)
#define SSC_THR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x24) // Transmit Holding Register (write-only)

#define SSC_RSHR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x30) // Receive Sync. Holding Register (read-only)
#define SSC_TSHR                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x34)          // Transmit Sync. Holding Register
#define SSC_RC0R                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x38)          // Receive Compare 0 Register
#define SSC_RC1R                         *(unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x3c)          // Receive Compare 1 Register
#define SSC_SR                           *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x40) // Status Register (read-only)
  #define SSC_TX_READY                   0x00000001
  #define SSC_TX_EMPTY                   0x00000002
  #define SSC_TX_END                     0x00000004
  #define SSC_TX_BUF_EMPTY               0x00000008
  #define SSC_RX_READY                   0x00000010
  #define SSC_RX_OVERRUN                 0x00000020
  #define SSC_RX_END                     0x00000040
  #define SSC_RX_BUFF_FULL               0x00000080
  #define SSC_COMPARE_0                  0x00000100
  #define SSC_COMPARE_1                  0x00000200
  #define SSC_TX_SYNC                    0x00000400
  #define SSC_RX_SYNC                    0x00000800
  #define SSC_TX_ENABLED                 0x00010000
  #define SSC_RX_ENABLED                 0x00020000
#define SSC_IER                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x44) // Interrupt Enable Register (write-only)
#define SSC_IDR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x48) // Interrupt Disable Register (write-only)
#define SSC_IMR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x4c) // Interrupt Mask Register (read-only)

#define SSC_RPR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x100) // PDC Receive Pointer Register
#define SSC_RCR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x104) // PDC Receive Count Register
#define SSC_TPR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x108) // PDC Transmit Pointer Register
#define SSC_TCR                          *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x10c) // PDC Transmit Count Register
#define SSC_RNPR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x110) // PDC Receive Next Pointer Register
#define SSC_RNCR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x114) // PDC Receive Next Count Register
#define SSC_TNPR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x118) // PDC Transmit Next Pointer Register
#define SSC_TNCR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x11c) // PDC Transmit Next Count Register
#define SSC_PTCR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x120) // PDC Transfer Control Register (write only)
#define SSC_PTSR                         *(volatile unsigned long*)(SSC_PERIPHERAL_BLOCK + 0x124) // PDC Transfer Status Register (read only)


// ADC block
//
#define ADC_CR                           *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x0) // {15} ADC Control Register (write only)
  #define START                          0x00000002                                        // start conversion
#define ADC_MR                           *(unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x4)     // ADC Mode Register (read/write)
  #define TRGEN                          0x00000001                                        // hardware trigger enabled
  #define TRGSEL_EXT                     0x0000000c                                        // trigger on ADTRG rising edge
  #define TRGSEL_T0                      0x00000000                                        // TIOA from timer 0 acts as conversion trigger
  #define TRGSEL_T1                      0x00000002                                        // TIOA from timer 1 acts as conversion trigger
  #define TRGSEL_T2                      0x00000004                                        // TIOA from timer 2 acts as conversion trigger
  #define LOWRES                         0x00000010                                        // 8 bit mode rather than 10 bit mode
  #define SLEEP                          0x00000020                                        // SLEEP mode
  #define MEDIUM_ADC_CLOCK               (((MASTER_CLOCK / (2*500000)) - 1)<<8)            // 500kHz clock
  #define MEDIUM_STARTUP_TIME            (5<<16)                                           // test value
  #define MEDIUM_SAMPLE_AND_HOLD         (5<<24)                                           // test value
#define ADC_RES1                         *(unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x8)     // ADC reserved 1
#define ADC_RES2                         *(unsigned long*)(ADC_PERIPHERAL_BLOCK + 0xc)     // ADC reserved 2
#define ADC_CHER                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x10) // {15} ADC Channel Enable Register (write only)
  #define ADC_CHANNEL_0                  0x01
  #define ADC_CHANNEL_1                  0x02
  #define ADC_CHANNEL_2                  0x04
  #define ADC_CHANNEL_3                  0x08
  #define ADC_CHANNEL_4                  0x10
  #define ADC_CHANNEL_5                  0x20
  #define ADC_CHANNEL_6                  0x40
  #define ADC_CHANNEL_7                  0x80
#define ADC_CHDR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x14) // {15} ADC Channel Disable Register (write only)
#define ADC_CHSR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x18) // ADC Channel Status Register (read only)
#define ADC_SR                           *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x1c) // ADC Status Register (read only)
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
#define ADC_LCDR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x20) // {15} ADC Last Converted Data Register (read only)
#define ADC_IER                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x24) // {15} ADC Interrupt Enable Register (write only)
#define ADC_IDR                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x28) // {15} ADC Interrupt Disable Register (write only)
#define ADC_IMR                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x2c) // {15} ADC Interrupt Mask Register (read only)
#define ADC_CDR0_ADD                     (volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x30)
#define ADC_CDR0                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x30) // ADC Channel Data Register 0 (read only)
#define ADC_CDR1                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x34) // ADC Channel Data Register 1 (read only)
#define ADC_CDR2                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x38) // ADC Channel Data Register 2 (read only)
#define ADC_CDR3                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x3c) // ADC Channel Data Register 3 (read only)
#define ADC_CDR4                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x40) // ADC Channel Data Register 4 (read only)
#define ADC_CDR5                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x44) // ADC Channel Data Register 5 (read only)
#define ADC_CDR6                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x48) // ADC Channel Data Register 6 (read only)
#define ADC_CDR7                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x4c) // ADC Channel Data Register 7 (read only)

#define ADC_RPR                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x100)// {19} PDC Receive Pointer Register
#define ADC_RCR                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x104)// PDC Receive Count Register
#define ADC_TPR                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x108)// PDC Transmit Pointer Register
#define ADC_TCR                          *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x10c)// PDC Transmit Count Register
#define ADC_RNPR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x110)// PDC Receive Next Pointer Register
#define ADC_RNCR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x114)// PDC Receive Next Count Register
#define ADC_TNPR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x118)// PDC Transmit Next Pointer Register
#define ADC_TNCR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x11c)// PDC Transmit Next Count Register
#define ADC_PTCR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x120)// PDC Transfer Control Register (write only)
#define ADC_PTSR                         *(volatile unsigned long*)(ADC_PERIPHERAL_BLOCK + 0x124)// PDC Transfer Status Register (read only)

// CAN block
//
#define CAN_MR                           *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x0)     // CAN Controller Mode Register (read/write)
  #define CANEN                          0x00000001
  #define LPM                            0x00000002                                        // Low Power Mode
  #define ABM                            0x00000004                                        // Enable Autobaud
  #define OVL                            0x00000008                                        // Overload frames enabled
  #define TEOF                           0x00000010                                        // Timestamp messages at each end of frame
  #define TTM                            0x00000020                                        // Time Triggered Mode
  #define TIMFRZ                         0x00000040                                        // Enable Timer Freeze
  #define DRPT                           0x00000080                                        // Disable repeat

#define CAN_IER                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x4) // {15} CAN Controller Interrupt Enable Register (write only)
  #define CAN_OVLSY                      0x80000000
  #define CAN_TBSY                       0x40000000
  #define CAN_RBSY                       0x20000000

  #define CAN_BERR                       0x10000000
  #define CAN_FERR                       0x08000000
  #define CAN_AERR                       0x04000000
  #define CAN_SERR                       0x02000000
  #define CAN_CERR                       0x01000000
  #define CAN_TSTP                       0x00800000
  #define CAN_TOVF                       0x00400000
  #define CAN_WAKEUP                     0x00200000
  #define CAN_SLEEP                      0x00100000
  #define CAN_BOFF                       0x00080000
  #define CAN_ERRP                       0x00040000
  #define CAN_WARN                       0x00020000
  #define CAN_ERRA                       0x00010000
  #define CAN_MB7                        0x00000080
  #define CAN_MB6                        0x00000040
  #define CAN_MB5                        0x00000020
  #define CAN_MB4                        0x00000010
  #define CAN_MB3                        0x00000008
  #define CAN_MB2                        0x00000004
  #define CAN_MB1                        0x00000002
  #define CAN_MB0                        0x00000001

#define CAN_IDR                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x8) // {15} CAN Controller Interrupt Disable Register (write only)
#define CAN_IMR                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0xc) // {15} CAN Controller Interrupt Mask Register (read only)
#define CAN_SR                           *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x10)// {15} CAN Controller Status Register (read only)
#define CAN_BR                           *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x14)    // CAN Baud Rate Register (read/write)
  #define SMP                            0x01000000                                        // Sampling mode (three samples)
  #define CAN_SPEED_1MEG                 (((MASTER_CLOCK/1000000 - 1) & 0x7f) << 16)
#define CAN_TIM                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x18) // {15} CAN Timer Register (read only)
#define CAN_TIMESTP                      *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x1c) // {15} CAN Timestamp Register (read only)
#define CAN_ECR                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x20) // {15} CAN Error Counter Register (read only)
#define CAN_TCR                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x24) // {15} CAN Transfer Command Register (write only)
  #define TIMRST                         0x80000000
#define CAN_ACR                          *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x28) // {15} CAN Abort Command Register (write only)

#define CAN_MMR0                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x200)   // CAN Mailbox 0 Mode Register (read/write)
#define CAN_MAM0                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x204)   // CAN Mailbox 0 Acceptance Mask Register (read/write)
#define CAN_MID0                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x208)   // CAN Mailbox 0 ID Register (read/write)
#define CAN_MFID0                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x20c)   // CAN Mailbox 0 Family ID Register (read/write)
#define CAN_MSR0                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x210)   // CAN Mailbox 0 Status Register (read/write)
#define CAN_MDL0                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x214)   // CAN Mailbox 0 Data Low Register (read/write)
#define CAN_MDH0                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x218)   // CAN Mailbox 0 Data High Register (read/write)
#define CAN_MCR0                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x21c) // {15} CAN Mailbox 0 Control Register (write only)

#define CAN_MMR1                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x220)   // CAN Mailbox 1 Mode Register (read/write)
#define CAN_MAM1                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x224)   // CAN Mailbox 1 Acceptance Mask Register (read/write)
#define CAN_MID1                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x228)   // CAN Mailbox 1 ID Register (read/write)
#define CAN_MFID1                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x22c)   // CAN Mailbox 1 Family ID Register (read/write)
#define CAN_MSR1                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x230)   // CAN Mailbox 1 Status Register (read/write)
#define CAN_MDL1                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x234)   // CAN Mailbox 1 Data Low Register (read/write)
#define CAN_MDH1                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x238)   // CAN Mailbox 1 Data High Register (read/write)
#define CAN_MCR1                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x23c) // {15} CAN Mailbox 1 Control Register (write only)

#define CAN_MMR2                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x240)   // CAN Mailbox 2 Mode Register (read/write)
#define CAN_MAM2                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x244)   // CAN Mailbox 2 Acceptance Mask Register (read/write)
#define CAN_MID2                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x248)   // CAN Mailbox 2 ID Register (read/write)
#define CAN_MFID2                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x24c)   // CAN Mailbox 2 Family ID Register (read/write)
#define CAN_MSR2                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x250)   // CAN Mailbox 2 Status Register (read/write)
#define CAN_MDL2                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x254)   // CAN Mailbox 2 Data Low Register (read/write)
#define CAN_MDH2                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x258)   // CAN Mailbox 2 Data High Register (read/write)
#define CAN_MCR2                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x25c) // {15} CAN Mailbox 2 Control Register (write only)

#define CAN_MMR3                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x260)   // CAN Mailbox 3 Mode Register (read/write)
#define CAN_MAM3                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x264)   // CAN Mailbox 3 Acceptance Mask Register (read/write)
#define CAN_MID3                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x268)   // CAN Mailbox 3 ID Register (read/write)
#define CAN_MFID3                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x26c)   // CAN Mailbox 3 Family ID Register (read/write)
#define CAN_MSR3                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x270)   // CAN Mailbox 3 Status Register (read/write)
#define CAN_MDL3                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x274)   // CAN Mailbox 3 Data Low Register (read/write)
#define CAN_MDH3                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x278)   // CAN Mailbox 3 Data High Register (read/write)
#define CAN_MCR3                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x27c) // {15} CAN Mailbox 3 Control Register (write only)

#define CAN_MMR4                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x280)   // CAN Mailbox 4 Mode Register (read/write)
#define CAN_MAM4                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x284)   // CAN Mailbox 4 Acceptance Mask Register (read/write)
#define CAN_MID4                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x288)   // CAN Mailbox 4 ID Register (read/write)
#define CAN_MFID4                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x28c)   // CAN Mailbox 4 Family ID Register (read/write)
#define CAN_MSR4                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x290)   // CAN Mailbox 4 Status Register (read/write)
#define CAN_MDL4                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x294)   // CAN Mailbox 4 Data Low Register (read/write)
#define CAN_MDH4                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x298)   // CAN Mailbox 4 Data High Register (read/write)
#define CAN_MCR4                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x29c) // {15} CAN Mailbox 4 Control Register (write only)

#define CAN_MMR5                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2a0)   // CAN Mailbox 5 Mode Register (read/write)
#define CAN_MAM5                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2a4)   // CAN Mailbox 5 Acceptance Mask Register (read/write)
#define CAN_MID5                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2a8)   // CAN Mailbox 5 ID Register (read/write)
#define CAN_MFID5                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2ac)   // CAN Mailbox 5 Family ID Register (read/write)
#define CAN_MSR5                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2b0)   // CAN Mailbox 5 Status Register (read/write)
#define CAN_MDL5                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2b4)   // CAN Mailbox 5 Data Low Register (read/write)
#define CAN_MDH5                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2b8)   // CAN Mailbox 5 Data High Register (read/write)
#define CAN_MCR5                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2bc) // {15} CAN Mailbox 5 Control Register (write only)

#define CAN_MMR6                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2c0)   // {30} CAN Mailbox 6 Mode Register (read/write)
#define CAN_MAM6                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2c4)   // CAN Mailbox 6 Acceptance Mask Register (read/write)
#define CAN_MID6                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2c8)   // CAN Mailbox 6 ID Register (read/write)
#define CAN_MFID6                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2cc)   // CAN Mailbox 6 Family ID Register (read/write)
#define CAN_MSR6                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2d0)   // CAN Mailbox 6 Status Register (read/write)
#define CAN_MDL6                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2d4)   // CAN Mailbox 6 Data Low Register (read/write)
#define CAN_MDH6                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2d8)   // CAN Mailbox 6 Data High Register (read/write)
#define CAN_MCR6                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2dc) // {15} CAN Mailbox 6 Control Register (write only)

#define CAN_MMR7                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2e0)   // {30} CAN Mailbox 7 Mode Register (read/write)
#define CAN_MAM7                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2e4)   // CAN Mailbox 7 Acceptance Mask Register (read/write)
#define CAN_MID7                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2e8)   // CAN Mailbox 7 ID Register (read/write)
#define CAN_MFID7                        *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2ec)   // CAN Mailbox 7 Family ID Register (read/write)
#define CAN_MSR7                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2f0)   // CAN Mailbox 7 Status Register (read/write)
#define CAN_MDL7                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2f4)   // CAN Mailbox 7 Data Low Register (read/write)
#define CAN_MDH7                         *(unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2f8)   // CAN Mailbox 7 Data High Register (read/write)
#define CAN_MCR7                         *(volatile unsigned long*)(CAN_PERIPHERAL_BLOCK + 0x2fc) // {15} CAN Mailbox 7 Control Register (write only)

  #define MAIL_BOX_DISABLED              0x00000000
  #define RECEPTION_MAIL_BOX             0x01000000
  #define RECEPTION_MAIL_BOX_OV          0x02000000
  #define TRANSMISSION_MAIL_BOX          0x03000000
  #define CONSUMER_MAIL_BOX              0x04000000
  #define PRODUCER_MAIL_BOX              0x05000000

  #define MAILBOX_TX_PRIORITY_0          0x00000000
  #define MAILBOX_TX_PRIORITY_1          0x00010000
  #define MAILBOX_TX_PRIORITY_2          0x00020000
  #define MAILBOX_TX_PRIORITY_3          0x00030000
  #define MAILBOX_TX_PRIORITY_4          0x00040000
  #define MAILBOX_TX_PRIORITY_5          0x00050000
  #define MAILBOX_TX_PRIORITY_6          0x00060000
  #define MAILBOX_TX_PRIORITY_7          0x00070000
  #define MAILBOX_TX_PRIORITY_8          0x00080000
  #define MAILBOX_TX_PRIORITY_9          0x00090000
  #define MAILBOX_TX_PRIORITY_10         0x000a0000
  #define MAILBOX_TX_PRIORITY_11         0x000b0000
  #define MAILBOX_TX_PRIORITY_12         0x000c0000
  #define MAILBOX_TX_PRIORITY_13         0x000d0000
  #define MAILBOX_TX_PRIORITY_14         0x000e0000
  #define MAILBOX_TX_PRIORITY_15         0x000f0000

// EMAC block
//
#define EMAC_NCR                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x0)    // Network Control Register (read/write)
  #define MPE                            0x00000010                                        // Enable management port
  #define EMAC_TE                        0x00000008                                        // Enable transmitter
  #define EMAC_RE                        0x00000004                                        // Enable receiver
  #define LLB                            0x00000002                                        // MII loopback
  #define CLRSTAT                        0x00000020                                        // Reset stats registers
  #define TSTART                         0x00000200                                        // Start transmission registers
#define EMAC_NCFGR                       *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x4)    // Network Configuration Register (read/write)
  #define SPD                            0x00000001                                        // 100M speed
  #define FD                             0x00000002                                        // full duplex (ignore collision)
  #define JFRAME                         0x00000008                                        // {36} Enable jumbo frames up to 10240 bytes in length
  #define CAF                            0x00000010                                        // Copy all received frames (promiscuous)
  #define NBI                            0x00000020                                        // {36} No broadcast
  #define MTI                            0x00000040                                        // {36} Multicast Hash enable
  #define UNI                            0x00000080                                        // {36} Unicast Hash enable
  #define BIG                            0x00000100                                        // {36} enable big frames to 1536, rather than 1518
  #define MCLK_32                        0x00000800
  #define PAE                            0x00002000                                        // enable pause frames
  #define RLCE                           0x00010000                                        // receive Length Field Checking Enable
  #define DRFCS                          0x00020000                                        // don't copy FCS to receive buffer
  #define EFRHD                          0x00040000                                        // {36} enable frames to be received in half-duple mode while transmitting
  #define IRXFCS                         0x00080000                                        // {36} don't reject frames with FCS/CRC error
#define EMAC_NSR                         *(volatile unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x8) // Network Status Register (read only)
  #define MDIO                           0x00000002
  #define PHY_LOGIC_RUNNING              0x00000004
#define EMAC_RES1                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xc)    // Reserved
#define EMAC_RES2                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x10)   // Reserved
#define EMAC_TSR                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x14)   // Transmit Status Register (read/write)
  #define TSR_UBR                        0x00000001                                        // Used Bit Read
  #define TSR_COL                        0x00000002                                        // Collision
  #define TSR_RLE                        0x00000004                                        // Retry Limit Exceeded
  #define TSR_TGO                        0x00000008                                        // Transmission presently active
  #define TSR_BEX                        0x00000010                                        // Buffers exhausted in mid-frame
  #define TSR_COMP                       0x00000020                                        // Transmission completed
  #define TSR_UND                        0x00000040                                        // Underrun
#define EMAC_RBQP                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x18)   // Receive Buffer Queue Pointer (read/write)
#define EMAC_TBQP                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x1c)   // Transmit Buffer Queue Pointer (read/write)
#define EMAC_RSR                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x20)   // Receive Status Register (read/write)
  #define RSR_BNA                        0x00000001                                        // Receive buffer not available {5}
  #define RSR_REC                        0x00000002                                        // Frame received {5}
  #define RSR_OVR                        0x00000004                                        // Receive Overrun {5}
#define EMAC_ISR                         *(volatile unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x24) // {15} Interrupt Status Register (read/write - various bits cleared on read)
#define EMAC_IER                         *(volatile unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x28) // {15} Interrupt Enable Register (write only)
#define EMAC_IDR                         *(volatile unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x2c) // {15} Interrupt Disable Register (write only)
#define EMAC_IMR                         *(volatile unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x30) // {15} Interrupt Mask Register (read only)
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
#define EMAC_MAN                         *(volatile unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x34)   // PHY Maintenance Register (read/write)
  #define SOF                            0x40000000
  #define PHY_CODE                       0x00020000
  #define PHY_READ                       0x20000000
  #define PHY_WRITE                      0x10000000
  #define READ_FROM_PHY (SOF | PHY_READ  | PHY_ADDRESS_ | PHY_CODE)
  #define WRITE_TO_PHY  (SOF | PHY_WRITE | PHY_ADDRESS_ | PHY_CODE)
#define EMAC_PTR                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x38)    // Pause Time Register (read/write)
#define EMAC_PFR                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x3c)    // Pause Frames received Register (read/write)
#define EMAC_FTO                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x40)    // Frames Transmitted Ok (read/write)
#define EMAC_SCF                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x44)    // Singe Collision Frames Register (read/write)
#define EMAC_MCF                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x48)    // Multiple Collision Frames Register (read/write)
#define EMAC_FRO                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x4c)    // Frames Received OK Register (read/write)
#define EMAC_FCSE                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x50)    // Frame Check Sequence Errors Register (read/write)
#define EMAC_ALE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x54)    // Alignment Errors Register (read/write)
#define EMAC_DTF                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x58)    // Deferred Transmission Frames Register (read/write)
#define EMAC_LCOL                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x5c)    // Late Collisions Register (read/write)
#define EMAC_ECOL                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x60)    // Excessive Collisions Register (read/write)
#define EMAC_TUND                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x64)    // Transmit Underrun Errors Register (read/write)
#define EMAC_CSE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x68)    // Carrier Sense Errors Register (read/write)
#define EMAC_RRE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x6c)    // Receive Resource Errors Register (read/write)
#define EMAC_ROV                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x70)    // Receiver Overrun Errors Register (read/write)
#define EMAC_RSE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x74)    // Receive Symbols Errors Register (read/write)
#define EMAC_ELE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x78)    // Excessive Length Errors Register (read/write)
#define EMAC_RJA                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x7c)    // Receive Jabbers Register (read/write)
#define EMAC_USF                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x80)    // Undersize Frames Register (read/write)
#define EMAC_STE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x84)    // SQE Test Errors Register (read/write)
#define EMAC_RLE                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x88)    // Received Length Field Mismatch Register (read/write)
#define EMAC_RES3                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x8c)    // Reserved (???) - not in data sheet!!
#define EMAC_HRB                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x90)    // Hash Bottom [31:0] Register (read/write)
#define EMAC_HRT                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x94)    // Hash Top [63:32] Register (read/write)
#define EMAC_SA1B                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x98)    // Specific Address 1 Bottom Register (read/write)
#define EMAC_SA1T                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0x9c)    // Specific Address 1 Top Register (read/write)
#define EMAC_SA2B                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xa0)    // Specific Address 2 Bottom Register (read/write)
#define EMAC_SA2T                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xa4)    // Specific Address 2 Top Register (read/write)
#define EMAC_SA3B                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xa8)    // Specific Address 3 Bottom Register (read/write)
#define EMAC_SA3T                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xac)    // Specific Address 3 Top Register (read/write)
#define EMAC_SA4B                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xb0)    // Specific Address 4 Bottom Register (read/write)
#define EMAC_SA4T                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xb4)    // Specific Address 4 Top Register (read/write)
#define EMAC_TID                         *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xb8)    // Type ID Checking Register (read/write)
#define EMAC_RES4                        *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xbc)    // Reserved (???) - not in data sheet!!
#define EMAC_USRIO                       *(unsigned long*)(EMAC_PERIPHERAL_BLOCK + 0xc0)    // User Input/Output Register (read/write)
  #define TRANSCEIVER_CLKEN              0x00000002                                         // Enable transceiver clock


// Ports                                                                 {3}
//
#define PIO_PER_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0000) // PIO Enable Register (Write only)
#define PIO_PDR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0004) // PIO Disable Register (Write only)
#define PIO_PSR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0008) // PIO Status Register (Read only)
#define PIO_RES1_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x000c) // PIO reserved
#define PIO_OER_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0010) // PIO Output Enable Register (Write only)
#define PIO_ODR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0014) // PIO Output Disable Register (Write only)
#define PIO_OSR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0018) // PIO Output Status Register (Read only)
#define PIO_RES2_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x001c) // PIO reserved
#define PIO_IFER_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0020) // {20} PIO Glitch Input Filter Enable Register (Write only)
#define PIO_IFDR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0024) // {20} PIO Glitch Input Filter Disable Register (Write only)
#define PIO_IFSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0028) // {20} PIO Glitch Input Filter Status Register (Read only)
#define PIO_RES3_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x002c) // PIO reserved
#define PIO_SODR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0030) // PIO Set Output Data Register (Write only)
#define PIO_CODR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0034) // PIO Clear Output Data Register (Write only)
#define PIO_ODSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0038) // PIO Data Output Status Register (Read only - write also when PIO_OWER mask set)
#define PIO_PDSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x003c) // PIO Pin Data Status Register (Read only)
#define PIO_IER_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0040) // PIO Interrupt Enable Register (Write only)
#define PIO_IDR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0044) // PIO Interrupt Disable Register (Write only)
#define PIO_IMR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0048) // PIO Interrupt Mask Register (Read only)
#define PIO_ISR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x004c) // PIO Interrupt Status Register (Read only)
#define PIO_MDER_A                       *(volatile long*)(PIOA_PERIPHERAL_BLOCK + 0x0050) // PIO Multi-driver Enable Register (Write only)
#define PIO_MDDR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0054) // PIO Multi-driver Disable Register (Write only)
#define PIO_MDSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0058) // PIO Multi-driver Status Register (Read only)
#define PIO_RES4_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x005c) // PIO reserved
#define PIO_PUDR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0060) // PIO Pull-up Disable Register (Write only)
#define PIO_PUER_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0064) // PIO Pull-up Enable Register (Write only)
#define PIO_PUSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0068) // PIO Pull-up Status Register (Read only)
#define PIO_RES5_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x006c) // PIO reserved
#define PIO_ASR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0070) // PIO Peripheral A Select Register (Write only)
#define PIO_BSR_A                        *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0074) // PIO Peripheral B Select Register (Write only)
#define PIO_ABSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0078) // PIO AB Status Register (Read only)
#define PIO_RES6_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x007c) // PIO reserved
#define PIO_RES7_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0080) // PIO reserved
#define PIO_RES8_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0084) // PIO reserved
#define PIO_RES9_A                       *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0088) // PIO reserved
#define PIO_RES10_A                      *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x008c) // PIO reserved
#define PIO_RES11_A                      *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0090) // PIO reserved
#define PIO_RES12_A                      *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0094) // PIO reserved
#define PIO_RES13_A                      *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x0098) // PIO reserved
#define PIO_RES14_A                      *(unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x009c) // PIO reserved
#define PIO_OWER_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x00a0) // PIO Output Write Enable (Write only)
#define PIO_OWDR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x00a4) // PIO Output Write Disable (Write only)
#define PIO_OWSR_A                       *(volatile unsigned long*)(PIOA_PERIPHERAL_BLOCK + 0x00a8) // PIO Output Write Status Register (Read only)

#ifndef _HW_SAM7S                                                        // {39}
    #define PIO_PER_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0000) // PIO Enable Register (Write only)
    #define PIO_PDR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0004) // PIO Disable Register (Write only)
    #define PIO_PSR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0008) // PIO Status Register (Read only)
    #define PIO_RES1_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x000c) // PIO reserved
    #define PIO_OER_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0010) // PIO Output Enable Register (Write only)
    #define PIO_ODR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0014) // PIO Output Disable Register (Write only)
    #define PIO_OSR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0018) // PIO Output Status Register (Read only)
    #define PIO_RES2_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x001c) // PIO reserved
    #define PIO_IFER_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0020) // {20} PIO Glitch Input Filter Enable Register (Write only)
    #define PIO_IFDR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0024) // {20} PIO Glitch Input Filter Disable Register (Write only)
    #define PIO_IFSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0028) // {20} PIO Glitch Input Filter Status Register (Read only)
    #define PIO_RES3_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x02c) // PIO reserved
    #define PIO_SODR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0030) // PIO Set Output Data Register (Write only)
    #define PIO_CODR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0034) // PIO Clear Output Data Register (Write only)
    #define PIO_ODSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0038) // PIO Data Output Status Register (Read only)
    #define PIO_PDSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x003c) // PIO Pin Data Status Register (Read only)
    #define PIO_IER_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0040) // PIO Interrupt Enable Register (Write only)
    #define PIO_IDR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0044) // PIO Interrupt Disable Register (Write only)
    #define PIO_IMR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0048) // PIO Interrupt Mask Register (Read only)
    #define PIO_ISR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x004c) // PIO Interrupt Status Register (Read only)
    #define PIO_MDER_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0050) // PIO Multi-driver Enable Register (Write only)
    #define PIO_MDDR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0054) // PIO Multi-driver Disable Register (Write only)
    #define PIO_MDSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0058) // PIO Multi-driver Status Register (Read only)
    #define PIO_RES4_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x005c) // PIO reserved
    #define PIO_PUDR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0060) // PIO Pull-up Disable Register (Write only)
    #define PIO_PUER_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0064) // PIO Pull-up Enable Register (Write only)
    #define PIO_PUSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0068) // PIO Pull-up Status Register (Read only)
    #define PIO_RES5_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x006c) // PIO reserved
    #define PIO_ASR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0070) // PIO Peripheral A Select Register (Write only)
    #define PIO_BSR_B                    *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0074) // PIO Peripheral B Select Register (Write only)
    #define PIO_ABSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0078) // PIO AB Status Register (Read only)
    #define PIO_RES6_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x007c) // PIO reserved
    #define PIO_RES7_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0080) // PIO reserved
    #define PIO_RES8_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0084) // PIO reserved
    #define PIO_RES9_B                   *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0088) // PIO reserved
    #define PIO_RES10_B                  *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x008c) // PIO reserved
    #define PIO_RES11_B                  *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0090) // PIO reserved
    #define PIO_RES12_B                  *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0094) // PIO reserved
    #define PIO_RES13_B                  *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x0098) // PIO reserved
    #define PIO_RES14_B                  *(unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x009c) // PIO reserved
    #define PIO_OWER_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x00a0) // PIO Output Write Enable (Write only)
    #define PIO_OWDR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x00a4) // PIO Output Write Disable (Write only)
    #define PIO_OWSR_B                   *(volatile unsigned long*)(PIOB_PERIPHERAL_BLOCK + 0x00a8) // PIO Output Write Status Register (Read only)
#endif

#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {46}{59}
    #define PIO_PER_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0000) // PIO Enable Register (Write only)
    #define PIO_PDR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0004) // PIO Disable Register (Write only)
    #define PIO_PSR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0008) // PIO Status Register (Read only)
    #define PIO_RES1_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x000c) // PIO reserved
    #define PIO_OER_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0010) // PIO Output Enable Register (Write only)
    #define PIO_ODR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0014) // PIO Output Disable Register (Write only)
    #define PIO_OSR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0018) // PIO Output Status Register (Read only)
    #define PIO_RES2_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x001c) // PIO reserved
    #define PIO_IFER_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0020) // {20} PIO Glitch Input Filter Enable Register (Write only)
    #define PIO_IFDR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0024) // {20} PIO Glitch Input Filter Disable Register (Write only)
    #define PIO_IFSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0028) // {20} PIO Glitch Input Filter Status Register (Read only)
    #define PIO_RES3_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x02c) // PIO reserved
    #define PIO_SODR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0030) // PIO Set Output Data Register (Write only)
    #define PIO_CODR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0034) // PIO Clear Output Data Register (Write only)
    #define PIO_ODSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0038) // PIO Data Output Status Register (Read only)
    #define PIO_PDSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x003c) // PIO Pin Data Status Register (Read only)
    #define PIO_IER_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0040) // PIO Interrupt Enable Register (Write only)
    #define PIO_IDR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0044) // PIO Interrupt Disable Register (Write only)
    #define PIO_IMR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0048) // PIO Interrupt Mask Register (Read only)
    #define PIO_ISR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x004c) // PIO Interrupt Status Register (Read only)
    #define PIO_MDER_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0050) // PIO Multi-driver Enable Register (Write only)
    #define PIO_MDDR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0054) // PIO Multi-driver Disable Register (Write only)
    #define PIO_MDSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0058) // PIO Multi-driver Status Register (Read only)
    #define PIO_RES4_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x005c) // PIO reserved
    #define PIO_PUDR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0060) // PIO Pull-up Disable Register (Write only)
    #define PIO_PUER_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0064) // PIO Pull-up Enable Register (Write only)
    #define PIO_PUSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0068) // PIO Pull-up Status Register (Read only)
    #define PIO_RES5_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x006c) // PIO reserved
    #define PIO_ASR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0070) // PIO Peripheral A Select Register (Write only)
    #define PIO_BSR_C                    *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0074) // PIO Peripheral B Select Register (Write only)
    #define PIO_ABSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0078) // PIO AB Status Register (Read only)
    #define PIO_RES6_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x007c) // PIO reserved
    #define PIO_RES7_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0080) // PIO reserved
    #define PIO_RES8_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0084) // PIO reserved
    #define PIO_RES9_C                   *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0088) // PIO reserved
    #define PIO_RES10_C                  *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x008c) // PIO reserved
    #define PIO_RES11_C                  *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0090) // PIO reserved
    #define PIO_RES12_C                  *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0094) // PIO reserved
    #define PIO_RES13_C                  *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x0098) // PIO reserved
    #define PIO_RES14_C                  *(unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x009c) // PIO reserved
    #define PIO_OWER_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x00a0) // PIO Output Write Enable (Write only)
    #define PIO_OWDR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x00a4) // PIO Output Write Disable (Write only)
    #define PIO_OWSR_C                   *(volatile unsigned long*)(PIOC_PERIPHERAL_BLOCK + 0x00a8) // PIO Output Write Status Register (Read only)
#endif

// Port macros                                                           // {28}
//
// Configure pins as output, including enabling port clock. eg. _CONFIG_PORT_OUTPUT(B, PB19);
//
#ifdef _WINDOWS
    #define _CONFIG_PORT_OUTPUT(ref, pins)  PMC_PCER = PIO##ref; PMC_PCSR |= PMC_PCER; PMC_PCER = 0; PIO_PER_##ref = (pins); PIO_OER_##ref = (pins); _SIM_PORTS
#else
    #define _CONFIG_PORT_OUTPUT(ref, pins)  PMC_PCER = PIO##ref; PIO_PER_##ref = (pins); PIO_OER_##ref = (pins)
#endif

// Configure pins as input, including enabling port clock. eg. _CONFIG_PORT_INPUT(B, PORTB_BIT2);
//
#ifdef _WINDOWS
    #define _CONFIG_PORT_INPUT(ref, pins)   PMC_PCER = PIO##ref; PMC_PCSR |= PMC_PCER; PMC_PCER = 0; PIO_PER_##ref = (pins); PIO_ODR_##ref = (pins); _SIM_PORTS
#else
    #define _CONFIG_PORT_INPUT(ref, pins)   PMC_PCER = PIO##ref; PIO_PER_##ref = (pins); PIO_ODR_##ref = (pins)
#endif

// Configure a peripheral function
//
#define _CONFIG_PERIPHERAL(ref, periph, peripheral_block, pins, functions) // not supported

// Write full port width
//
#ifdef _WINDOWS
    #define _WRITE_PORT(ref, value) PIO_SODR_##ref = (value); PIO_CODR_##ref = ~(value); _SIM_PORTS
#else                                                                    // {41}
    #define _WRITE_PORT(ref, value) PIO_OWER_##ref = (0xffffffff); PIO_ODSR_##ref = value; PIO_OWDR_##ref = (0xffffffff)
#endif

// Write to a port with a mask
//
#ifdef _WINDOWS
    #define _WRITE_PORT_MASK(ref, value, mask) PIO_SODR_##ref = ((mask) & (value)); PIO_CODR_##ref = ((mask) & ~(value)); _SIM_PORTS
#else                                                                    // {41}
    #define _WRITE_PORT_MASK(ref, value, mask) PIO_OWER_##ref = (mask); PIO_ODSR_##ref = value; PIO_OWDR_##ref = (mask)
#endif

// Toggle a port with a mask
//
#ifdef _WINDOWS
    #define _TOGGLE_PORT(ref, mask)     PIO_CODR_##ref = (PIO_ODSR_##ref & (mask)); PIO_SODR_##ref = (~(PIO_ODSR_##ref) & (mask)); _SIM_PORTS
#else                                                                    // {41}
    #define _TOGGLE_PORT(ref, mask)     PIO_OWER_##ref = (mask); PIO_ODSR_##ref = (PIO_ODSR_##ref ^ (mask)); PIO_OWDR_##ref = (mask)
#endif

// Read full port width
//
#define _READ_PORT(ref)                 (PIO_PDSR_##ref)

// Read from a port with a mask
//
#define _READ_PORT_MASK(ref, mask)      (PIO_PDSR_##ref & (mask))

// Configure outputs and then set a value to them (this processor sets the value before driving the outputs)
//
#ifdef _WINDOWS
    #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) PMC_PCER = PIO##ref; PMC_PCSR |= PMC_PCER; PMC_PCER = 0; PIO_PER_##ref = (pins); PIO_SODR_##ref = ((pins) & (value)); PIO_CODR_##ref = ((pins) & ~(value)); PIO_OER_##ref = (pins); _SIM_PORTS
#else                                                                    // {32}{41} correct macro to write PIO_ODSR_ with value rather than pins 
    #define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) PMC_PCER = PIO##ref; PIO_PER_##ref = pins; PIO_OWER_##ref = (pins); PIO_ODSR_##ref = (value); PIO_OWDR_##ref = (pins); PIO_OER_##ref = (pins)
#endif

// Set from outputs to inputs
//
#define _FLOAT_PORT(ref, pins)          PIO_ODR_##ref = (pins); _SIM_PORTS

// Set from inputs to outputs 
//
#define _DRIVE_PORT_OUTPUT(ref, pins)   PIO_OER_##ref = (pins); _SIM_PORTS

// Set from inputs to outputs and set a value to them (this processor sets the value before driving the outputs - it doesn't need to configure the port in this case)
//
#ifdef _WINDOWS
    #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) PIO_SODR_##ref = ((pins) & (value)); PIO_CODR_##ref = ((pins) & ~(value)); PIO_OER_##ref = (pins); _SIM_PORTS
#else                                                                    // {41}
    #define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value) PIO_OWER_##ref = (pins); PIO_ODSR_##ref = (pins); PIO_OWDR_##ref = (pins); PIO_OER_##ref = (pins)
#endif


// Set and clear individual bits of a port
//
#define _SETBITS(ref, mask)             PIO_SODR_##ref = (mask); _SIM_PORTS
#define _CLEARBITS(ref, mask)           PIO_CODR_##ref = (mask); _SIM_PORTS


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
#define PA17    0x00020000
#define PA18    0x00040000
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
#define PA30    0x40000000
#if defined _HW_SAM7S || defined _HW_SAM7SE                              // {39}
    #define PA31    0x80000000
#endif
#if !defined _HW_SAM7S
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
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {46}{59}
    #define PC00    0x00000001
    #define PC01    0x00000002
    #define PC02    0x00000004
    #define PC03    0x00000008
    #define PC04    0x00000010
    #define PC05    0x00000020
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
    #if defined _HW_SAM7L                                                // {59}
        #define PC24    0x01000000
        #define PC25    0x02000000
        #define PC26    0x04000000
        #define PC27    0x08000000
        #define PC28    0x10000000
        #define PC29    0x20000000
    #endif
#endif

// PORT A Peripheral A/B functions                                       // {11}
//
#if defined _HW_SAM7S                                                    // {39}
    #define PWM0            PA00
        #define TIOA0_B     PA00
    #define PWM1            PA01
        #define TIOB0_B     PA01
    #define PWM2            PA02
        #define SCK_0B      PA02
    #define TWD             PA03
        #define SPI_CS3_0B  PA03
    #define TWCK            PA04
        #define TCLK0_B     PA04
    #define UART_RX_0       PA05
        #define SPI_CS3_0b  PA05
    #define UART_TX_0       PA06
        #define PCK0_B      PA06
    #define RTS_0           PA07
        #define PWM3_b      PA07
    #define CTS_0           PA08
        #define ADTRG_B     PA08
    #define UART_RX_2       PA09
        #define SPI_CS1_0B  PA09
    #define UART_TX_2       PA10
        #define SPI_CS2_0B  PA10
    #define SPI_CS0_0       PA11
        #define PWM0_B      PA11
    #define SPI_MISO_0      PA12
        #define PWM1_B      PA12
    #define SPI_MOSI_0      PA13
        #define PWM2_B      PA13
    #define SPI_CLK_0       PA14
        #define PWM3_B      PA14
    #define TF              PA15
        #define TIOA1_B     PA15
    #define TK              PA16
        #define TIOB1_B     PA16
    #define TD              PA17
        #define PCK1_B      PA17
    #define RD              PA18
        #define PCK2_B      PA18
    #define RK              PA19
        #define FIQ_B       PA19
    #define RF              PA20
        #define IRQ0_B      PA20
    #define UART_RX_1       PA21
        #define PCK1_b      PA21
    #define UART_TX_1       PA22
        #define SPI_CS3_0bb PA22
    #define SCK_1           PA23
        #define PWM0_b      PA23
    #define RTS_1           PA24
        #define PWM1_b      PA24
    #define CTS_1           PA25
        #define PWM2_b      PA25
    #define DCD_1           PA26
        #define TIOA2_B     PA26
    #define DTR_1           PA27
        #define TIOB2_B     PA27
    #define DSR_1           PA28
        #define TCLK1_B     PA28
    #define RI_1            PA29
        #define TCLK2_B     PA29
    #define IRQ1            PA30
        #define SPI_CS2_0b  PA30
    #define SPI_CS1_0       PA31
        #define PCK2_b      PA31
#elif defined _HW_SAM7SE                                                 // {46}
    // PORT A Peripheral A/B functions
    //
    #define PWM0            PA00
        #define A0_NBS0_B   PA00
    #define PWM1            PA01
        #define A1_NBS2_B   PA01
    #define PWM2            PA02
        #define A2_B        PA02
    #define TWD             PA03
        #define A3_B        PA03
    #define TWCK            PA04
        #define A4_B        PA04
    #define UART_RX_0       PA05
        #define A5_B        PA05
    #define UART_TX_0       PA06
        #define A6_B        PA06
    #define RTS_0           PA07
        #define A7_B        PA07
    #define CTS_0           PA08
        #define A8_B        PA08
    #define UART_RX_2       PA09
        #define A9_B        PA09
    #define UART_TX_2       PA10
        #define A10_B       PA10
    #define SPI_CS0_0       PA11
        #define A11_B       PA11
    #define SPI_MISO_0      PA12
        #define A12_B       PA12
    #define SPI_MOSI_0      PA13
        #define A13_B       PA13
    #define SPI_CLK_0       PA14
        #define A14_B       PA14
    #define TF              PA15
        #define A15_B       PA15
    #define TK              PA16
        #define A16_BA0_B   PA16
    #define TD              PA17
        #define A17_BA1_B   PA17
    #define RD              PA18
        #define NBS3_CFIOW_B PA18
    #define RK              PA19
        #define NCS4_CFCS0_B PA19
    #define RF              PA20
        #define NCS2_CFCS1_B PA20
    #define UART_RX_1       PA21
        #define NCS6_CFCE2_B PA21
    #define UART_TX_1       PA22
        #define NCS5_CFCE1_B PA22
    #define SCK_1           PA23
        #define NWR1_NBS1_CFIOR_B PA23
    #define RTS_1           PA24
        #define SDA10_B     PA24
    #define CTS_1           PA25
        #define SDCKE_B     PA25
    #define DCD_1           PA26
        #define NCS1_SDCS_B PA26
    #define DTR_1           PA27
        #define SDWE_B      PA27
    #define DSR_1           PA28
        #define CAS_B       PA28
    #define RI_1            PA29
        #define RAS_B       PA29
    #define IRQ1            PA30
        #define D30_B       PA30
    #define SPI_CS1_0       PA31
        #define D31_B       PA31

    // PORT B Peripheral A/B functions
    //
    #define TIOA0           PB00
        #define A0_NBS0_BB  PB00
    #define TIOB0           PB01
        #define A1_NBS2_BB  PB01
    #define SCK0            PB02
        #define A2_BB       PB02
    #define SPI_CS3_0       PB03
        #define A3_BB       PB03
    #define TCLK0           PB04
        #define A4_BB       PB04
    #define SPI_CS3_0b      PB05
        #define A5_BB       PB05
    #define PCK0            PB06
        #define A6_BB       PB06
    #define PWM3            PB07
        #define A7_BB       PB07
    #define ADTRG           PB08
        #define A8_BB       PB08
    #define SPI_CS1_0b      PB09
        #define A9_BB       PB09
    #define SPI_CS2_0       PB10
        #define A10_BB      PB10
    #define PWM0b           PB11
        #define A11_BB      PB11
    #define PWM1b           PB12
        #define A12_BB      PB12
    #define PWM2b           PB13
        #define A13_BB      PB13
    #define PWM3b           PB14
        #define A14_BB      PB14
    #define TIOA1           PB15
        #define A15_BB      PB15
    #define TIOB1           PB16
        #define A16_BA0_BB  PB16
    #define PCK1            PB17
        #define A17_BA1_BB  PB17
    #define PCK2            PB18
        #define D16_B       PB18
    #define FIQ             PB19
        #define D17_B       PB19
    #define IRQ0            PB20
        #define D18_B       PB20
    #define PCK1b           PB21
        #define D19_B       PB21
    #define SPI_CS3_0bb     PB22
        #define D20_B       PB22
    #define PWM0bb          PB23
        #define D21_B       PB23
    #define PWM1bb          PB24
        #define D22_B       PB24
    #define PWM2bb          PB25
        #define D23_B       PB25
    #define TIOA2           PB26
        #define D24_B       PB26
    #define TIOB2           PB27
        #define D25_B       PB27
    #define TCLK1           PB28
        #define D26_B       PB28
    #define TCLK2           PB29
        #define D27_B       PB29
    #define SPI_CS2_0b      PB30
        #define D28_B       PB30
    #define PCK2b           PB31
        #define D29_B       PB31

    // PORT C Peripheral A/B functions
    //
    #define D0              PC00
    #define D1              PC01
    #define D2              PC02
    #define D3              PC03
    #define D4              PC04
    #define D5              PC05
    #define D6              PC06
    #define D7              PC07
    #define D8              PC08
        #define RTS1_B      PC08
    #define D9              PC09
        #define DTR1_B      PC09
    #define D10             PC10
        #define PCK0_B      PC10
    #define D11             PC11
        #define PCK1_B      PC11
    #define D12             PC12
        #define PCK2_B      PC12
    #define D13             PC13
    #define D14             PC14
        #define SPI_CS1_0B  PC14
    #define D15             PC15
        #define NCS3_NANDCS_B PC15
    #define A18             PC16
        #define NWAIT_B     PC16
    #define A19             PC17
        #define NANDOE_B    PC17
    #define A20             PC18
        #define NANDWE_B    PC18
    #define A21_NANDALE     PC19
    #define A22_REG_NANDCLE PC20
        #define NCS7_B      PC20
        #define NWR0_NWE_CFWE_B PC21
        #define NDR_CFOE_B  PC22
    #define CFRNW           PC23
        #define NCS0_B      PC23
#elif defined _HW_SAM7L                                                  // {59}
    // PORT A Peripheral A/B functions
    //


    // PORT B Peripheral A/B functions
    //
    #define RTS_1a          PB15
    #define RTS_0a          PB16

    // PORT C Peripheral A/B functions
    //
    #define CTS_1           PC00
        #define TIOA2_B     PC01
    #define IRQ1            PC05
        #define TIOA0_B     PC07
    #define UART_RX_0       PC12
    #define UART_TX_0       PC13
    #define RTS_0           PC14
    #define CTS_0           PC15
    #define UART_RX_2       PC16
    #define UART_TX_2       PC17
        #define TIOA1_B     PC22
    #define UART_RX_1       PC24
    #define UART_TX_1       PC25
    #define RTS_1           PC26

        #define IRQ0_B      PC25
#else
    #define UART_RX_0       PA00
    #define UART_TX_0       PA01
    #define SCK_0           PA02
        #define SPI_CS1_1b  PA02
    #define RTS_0           PA03
        #define SPI_CS2_1b  PA03
    #define CTS_0           PA04
        #define SPI_CS3_1b  PA04
    #define UART_RX_1       PA05
    #define UART_TX_1       PA06
        #define SPI_CS1_0B  PA07
    #define SCK_1           PA07
        #define SPI_CS2_0B  PA08
    #define RTS_1           PA08
        #define SPI_CS3_0B  PA09
    #define CTS_1           PA09                                         // {23}
    #define TWD             PA10
    #define TWCK            PA11
    #define SPI_CS0_0       PA12
    #define SPI_CS1_0       PA13
        #define PCK_1B      PA13
    #define SPI_CS2_0       PA14
        #define IRQ1B       PA14
    #define SPI_CS3_0       PA15
        #define TCLK2_B     PA15
    #define SPI_MISO_0      PA16
    #define SPI_MOSI_0      PA17
    #define SPI_CLK_0       PA18
    #define CAN_RX          PA19
    #define CAN_TX          PA20
        #define SPI_CS0_1B  PA21
    #define TF              PA21
        #define SPI_CLK_1B  PA22
    #define TK              PA22
        #define SPI_MOSI_1B PA23
    #define TD              PA23
        #define SPI_MISO_1B PA24
    #define RD              PA24
        #define SPI_CS1_1B  PA25
    #define RK              PA25
        #define SPI_CS2_1B  PA26
    #define RF              PA26
    #define UART_RX_2       PA27                                        // {8}
        #define PCK3_B      PA27
    #define UART_TX_2       PA28
        #define SPI_CS3_1B  PA29
    #define FIQ             PA29
    #define IRQ0            PA30
        #define PCK2_B      PA30

    // PORT B Peripheral A/B functions                                   // {11}
    //
    #define ETXCK_EREFCK    PB00
        #define PCK0_B      PB00
    #define ETXEN           PB01
    #define ETX0            PB02
    #define ETX1            PB03
    #define ECRS            PB04
    #define ERX0            PB05
    #define ERX1            PB06
    #define ERXER           PB07
    #define EMDC            PB08
    #define EMDIO           PB09
    #define ETX2            PB10
        #define SPI_CS1_1BB PB10
    #define ETX3            PB11
        #define SPI_CS2_1BB PB11
    #define ETXER           PB12
        #define TCLK0_B     PB12
    #define ERX2            PB13
        #define SPI_CS1_0BB PB13
    #define ERX3            PB14
        #define SPI_CS2_0BB PB14
    #define ERXDV_ECRSDV    PB15
    #define ECOL            PB16
        #define SPI_CS3_1BB PB16
    #define ERXCK           PB17
        #define SPI_CS3_0BB PB17
    #define EF100           PB18
        #define ADTRG_B     PB18
    #define PWM0            PB19
        #define TCLK1_B     PB19
    #define PWM1            PB20
        #define PCK0_BB     PB20
    #define PWM2            PB21
        #define PCK1_BB     PB21
    #define PWM3            PB22
        #define PCK2_BB     PB22
    #define TIOA0           PB23
        #define DCD1_B      PB23
    #define TIOB0           PB24
        #define DSR1_B      PB24
    #define TIOA1           PB25
        #define DTR1_B      PB25
    #define TIOB1           PB26
        #define RI1_B       PB26
    #define TIOA2           PB27
        #define PWM0_B      PB27
    #define TIOB2           PB28
        #define PWM1_B      PB28
    #define PCK0            PB29
        #define PWM2_B      PB29
    #define PCK1            PB30
        #define PWM3_B      PB30
#endif

#ifdef _WINDOWS                                                          // {20}
	#define _ENABLE_PULLUP(port, x)          PIO_PUER_##port = x; PIO_PUSR_##port |= PIO_PUER_##port; PIO_PUER_##port = 0
	#define _DISABLE_PULLUP(port, x)         PIO_PUDR_##port = x; PIO_PUSR_##port &= ~PIO_PUDR_##port; PIO_PUDR_##port = 0
	#define _ENABLE_GLITCH_FILTER(port, x)   PIO_IFER_##port = x; PIO_IFSR_##port |= PIO_IFER_##port; PIO_IFER_##port = 0
	#define _DISABLE_GLITCH_FILTER(port, x)  PIO_IFDR_##port = x; PIO_IFSR_##port &= ~PIO_IFDR_##port; PIO_IFDR_##port = 0
#else
	#define _ENABLE_PULLUP(port, x)          PIO_PUER_##port = x 
	#define _DISABLE_PULLUP(port, x)         PIO_PUDR_##port = x 
	#define _ENABLE_GLITCH_FILTER(port, x)   PIO_IFER_##port = x 
	#define _DISABLE_GLITCH_FILTER(port, x)  PIO_IFDR_##port = x 
#endif


// Software references used when setting up interrupts
//
#define PORT_INTERRUPT            1
#define TIMER_INTERRUPT           2
#define PWM_CONFIGURATION         3                                      // {37}
#define ADC_INTERRUPT             4                                      // {50}


#define IRQ_HIGH_LEVEL_SENSITIVE  0x00
#define IRQ_FALLING_EDGE          0x01
#define IRQ_RISING_EDGE           0x02
#define IRQ_LOW_LEVEL_SENSITIVE   0x04                                   // {17}
#define IRQ_PULLUP_ENABLE         0x40                                   // {34}
#define IRQ_GLITCH_ENABLE         0x80                                   // {34}

#define PORT_A                    0
#define PORT_B                    1
#define PORTA_IRQ0                2                                      // {6}
#define PORTA_IRQ1                3                                      // {21}
#define PORTA_FIQ                 4


// Define interrupt setup structures to suit this processor              {1}
//
typedef struct stINTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set

  unsigned char    int_port;                                             // the port (A..B)
  unsigned char    int_port_sense;                                       // level sensitive, falling, rising, both
  unsigned long    int_port_bits;                                        // the input pins to be activated (in bit form) {7}
} INTERRUPT_SETUP;

typedef struct stTIMER_INTERRUPT_SETUP
{
    void ( *int_handler )( void );                                       // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set

    unsigned char    timer_reference;                                    // hardware timer to use
#ifdef _HW_TIMER_MODE
    unsigned char    timer_mode;                                         // {31} timer mode
#endif
    unsigned short   timer_value;                                        // the ms delay value
    unsigned short   timer_us_value;                                     // {22} us value when delay is under 1ms
    unsigned short   pwm_value;                                          // {37}
} TIMER_INTERRUPT_SETUP;

#define TIMER_SINGLE_SHOT               0x00                             // {31}
#define TIMER_SOURCE_TCLK0              0x01                             // {42}
#define TIMER_SOURCE_TCLK1              0x02
#define TIMER_SOURCE_TCLK2              0x03
#define TIMER_SOURCE_TIOA0              0x04
#define TIMER_SOURCE_TIOA1              0x05
#define TIMER_SOURCE_TIOA2              0x06
#define EXTERNAL_TIMER_SOURCE           0x07
#define TIMER_DISABLE                   0x7f
#define TIMER_SOURCE_RISING_EDGE        0x00
#define TIMER_SOURCE_FALLING_EDGE       0x80
#define TIMER_PWM_START_0               0x01                             // {37}
#define TIMER_PWM_START_1               0x02
#define TIMER_PWM_START_2               0x04
#define TIMER_PWM_START_3               0x08
#define TIMER_PERIODIC                  0x80
#define TIMER_PWM_NO_CONFIG             0x10
#define TIMER_US_VALUE                  0x20
#define TIMER_PWM                       0x00
#define TIMER_PWM_ALT                   0x40
#define TIMER_STOP_PWM                  0x80
#define TIMER_PWM_STOP_0                (TIMER_PWM_START_0 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_1                (TIMER_PWM_START_1 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_2                (TIMER_PWM_START_2 | TIMER_STOP_PWM)
#define TIMER_PWM_STOP_3                (TIMER_PWM_START_3 | TIMER_STOP_PWM)

#define _PWM_PERCENT(percent_pwm, frequency_value)       ((frequency_value * percent_pwm)/100)  // {17}
#define _PWM_TENTH_PERCENT(percent_pwm, frequency_value) ((frequency_value * percent_pwm)/1000)

#define TIMER_US_DELAY(usec)           ((MASTER_CLOCK/1000000)*usec)     // {13}
#define TIMER_FREQUENCY_VALUE(hertz)   (1000000 / hertz)

typedef struct stADC_INTERRUPT_RESULT                                    // {19}
{
  signed short     sADC_value;                                           // present ADC sample value
  unsigned char    ucADC_channel;                                        // ADC channel status
  unsigned char    ucADC_flags;                                          // flags
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
  signed short     sADC_value[ADC_CHANNELS];                             // present ADC sample values
  unsigned char    ucADC_status[ADC_CHANNELS];                           // present ADC channel status
} ADC_RESULTS;

// Define interrupt setup structure to suit this processor
//
typedef struct stADC_SETUP
{
  void ( *int_handler )( ADC_INTERRUPT_RESULT* );                        // interrupt handler to be configured
  unsigned char    int_type;                                             // identifier for when configuring
  unsigned char    int_priority;                                         // priority the user wants to set
  unsigned char    int_adc_bits;                                         // the ADC input bits
  unsigned char    int_adc_int_type;                                     // interrupt type (ADC_END_OF_SCAN_INT, ADC_ZERO_CROSSING_INT, ADC_LOW_LIMIT_INT, ADC_HIGH_LIMIT_INT)
  unsigned long    int_samples;                                          // the number of samples to be collected - using DMA
  unsigned long    int_adc_sample_rate;                                  // sample rate im Hz
  unsigned long    int_adc_mode;                                         // parallel, sequencial, single-ended, differential, start/stop, etc.
  unsigned long    int_adc_speed;                                        // operating speed value (register value) 
  unsigned short   int_zero_crossing_trigger;                            // zero crossing value
  unsigned short   int_high_level_trigger;                               // trigger when higher than this level
  unsigned short   int_low_level_trigger;                                // trigger when lower than this level
  void            *int_adc_result;                                       // results structure to be filled - type variable
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


typedef struct stRTC_SETUP                                               // {53}
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
    unsigned long    ulLocalUTC;                                         // {61} optional UTC field
} RTC_SETUP;


#define RTC_TIME_SETTING    0x01
#define RTC_TICK_SEC        0x02
#define RTC_TICK_MIN        0x03
#define RTC_TICK_HOUR       0x04
#define RTC_TICK_DAY        0x05
#define RTC_ALARM_TIME      0x06
#define RTC_STOPWATCH_GO    0x07
#define RTC_GET_TIME        0x08
#define RTC_CONVERT_TO_UTC  0x09
#define RTC_GET_ALARM       0x0b
#define RTC_RETURNED_TIME   0x80
#define RTC_DISABLE         0x80
#define RTC_SET_UTC         0x20                                         // use UTC rather than time and date when setting
#define RTC_INITIALISATION  0x40                                         // {58}


#define MII_INTERFACE  (PB00 | PB01 | PB02 | PB03 | PB04 | PB05 | PB06 | PB07 | PB08 | PB09 | PB10 | PB11 | PB12 | PB13 | PB14 | PB15 | PB16 | PB17)


// MICREL PHYSICAL
//
#define KS8721_BASIC_CONTROL_REGISTER                 (0x0 << 18)
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
#define KS8721_BASIC_STATUS_REGISTER                  (0x1 << 18)
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
#define KS8721_PHY_IDENTIFIER_1                       (0x2  << 18)
#define KS8721_PHY_IDENTIFIER_2                       (0x3  << 18)
#define KS8721_AUTO_NEG_ADVERT_REG                    (0x4  << 18)
#define KS8721_AUTO_NEG_LINK_PARTNER_ABIL_REG         (0x5  << 18)
#define KS8721_AUTO_NEG_EXPANSION_REG                 (0x6  << 18)
#define KS8721_AUTO_NEG_NEXT_PAGE_REG                 (0x7  << 18)
#define KS8721_LINK_PARTNER_NEXT_PAGE_ABIL            (0x8  << 18)
#define KS8721_RXER_COUNTER_REGISTER                  (0x15 << 18)
#define KS8721_INTERRUPT_CONTROL_STATUS_REGISTER      (0x1b << 18)
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
#define KS8721_100BASETX_PHY_CONTROL_REGISTER         (0x1f << 18)






typedef struct stSAM7X_RX_BD
{
volatile unsigned char   *ptrBlock;
volatile unsigned long    bd_details;
} SAM7X_RX_BD;

#define SAM7X_FIXED_BUFFER_SIZE                       128                // rx and tx buffers are made up of several fixed sized buffers


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
#define SPI_CR_0                         *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0000) // {15} SPI Control Register - write only
  #define SPIEN                          0x00000001
  #define SPIDIS                         0x00000002
  #define SPI_SWRST                      0x00000080
  #define LASTXFER                       0x01000000
#define SPI_MR_0                         *(unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0004) // SPI Mode Register - R/W
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

#define SPI_RDR_0                        *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0008) // SPI Receive Data Register - read only
#define SPI_TDR_0                        *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x000c) // SPI Transmit Data Register - write only
#define SPI_SR_0                         *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0010) // SPI Status Register - read only
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
#define SPI_IER_0                        *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0014) // {15} SPI Interrupt Enable Register - write only
#define SPI_IDR_0                        *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0018) // {15} SPI Interrupt Disable Register - write only
#define SPI_IMR_0                        *(volatile unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x001c) // {15} SPI Interrupt Mask Register - read only
#define SPI_CSR0_0                       *(unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0030) // SPI Chip Select Register 0 - R/W
#define SPI_CSR1_0                       *(unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0034) // SPI Chip Select Register 1 - R/W
#define SPI_CSR2_0                       *(unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x0038) // SPI Chip Select Register 2 - R/W
#define SPI_CSR3_0                       *(unsigned long*)(SPI0_PERIPHERAL_BLOCK + 0x003c) // SPI Chip Select Register 3 - R/W
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

#define SPI_CR_1                         *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0000) // {15} SPI Control Register - write only
#define SPI_MR_1                         *(unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0004) // SPI Mode Register - R/W
#define SPI_RDR_1                        *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0008) // {13} SPI Receive Data Register - read only
#define SPI_TDR_1                        *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x000c) // {13} SPI Transmit Data Register - write only
#define SPI_SR_1                         *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0010) // {13} SPI Status Register - read only
#define SPI_IER_1                        *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0014) // {15} SPI Interrupt Enable Register - write only
#define SPI_IDR_1                        *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0018) // {15} SPI Interrupt Disable Register - write only
#define SPI_IMR_1                        *(volatile unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x001c) // {15} SPI Interrupt Mask Register - read only
#define SPI_CSR0_1                       *(unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0030) // SPI Chip Select Register 0 - R/W
#define SPI_CSR1_1                       *(unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0034) // SPI Chip Select Register 1 - R/W
#define SPI_CSR2_1                       *(unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x0038) // SPI Chip Select Register 2 - R/W
#define SPI_CSR3_1                       *(unsigned long*)(SPI1_PERIPHERAL_BLOCK + 0x003c) // SPI Chip Select Register 3 - R/W


#if defined _HW_SAM7S                                                    // {39}
    #define PORTS_AVAILABLE     2                                        // 2nd port for ADC pins
    #define PORT_WIDTH          32
#elif defined _HW_SAM7SE || defined _HW_SAM7L                            // {46}{59}
    #define PORTS_AVAILABLE     4                                        // 4th port for ADC pins
    #define PORT_WIDTH          32
#else
    #define PORTS_AVAILABLE     3                                        // {18}{19} - 3rd port for ADC pins
    #define PORT_WIDTH          31
#endif

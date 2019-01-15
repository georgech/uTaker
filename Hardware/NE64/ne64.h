/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Obere Bahnhofstrasse 13, 5507 Mellingen

   www.uTasker.com      Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:           NE64.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   *********************************************************************

   27.05.2007    Add LOW_RES_MS, MED_RES_MS, HIGH_RES_MS hardware resolution control
   04.10.2007    Change port data registers (A,B,E,K) defines to be volatile
   31.12.2007    Add FLASH size                                          {1}
   13.05.2007    Add fnConfigureInterrupt interface for Timer interrupt  {2}
   18.12.2008    PORTS_AVAILABLE and PORT_WIDTH moved here from WinSim.c {3}
   01.05.2009    DDR registers set volatile                              {4}

*/

#ifdef _WINDOWS
  #include "SimNE64.h"
#endif


#define OSCCLK               25000000                                    // 25 MHz oscillator
#define PLL_OUTPUT_FREQ      (2 * OSCCLK)                                // 50 MHz bus clock to allow 100Mb Ethernet
#define BUS_CLOCK            PLL_OUTPUT_FREQ

#define START_OF_RAM (unsigned char *)0x2000

#if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM
    #define CAST_EXTENDED_POINTER_ARITHMETIC unsigned long               // use 32 bit pointer for extended virtual memory
#else
    #define CAST_EXTENDED_POINTER_ARITHMETIC unsigned short              // NE64 uses 16 bit pointers even for extended memory
#endif
    #define CAST_POINTER_ARITHMETIC unsigned short                       // NE64 uses 16 bit pointers

#define SIZE_OF_FLASH        (unsigned long)((unsigned long)64*(unsigned long)1024) // {1}


// Clock and Reset Generator block
//
#ifdef _WINDOWS
  #define MEBI_ADD           ((unsigned char *)(&ucNE64.ucSimMebi))      // Multiplexed external bus interface
  #define CLK_RESET_BASE_ADD ((unsigned char *)(&ucNE64.ucSimClockMod))
  #define TIMERS_BASE_ADD    ((unsigned char *)(&ucNE64.ucSimTimers))
  #define ATD_BASE_ADD       ((unsigned char *)(&ucNE64.ucSimATD))
  #define SCI_BASE_ADD       ((unsigned char *)(&ucNE64.ucSimSCI))
  #define SPI_BASE_ADD       ((unsigned char *)(&ucNE64.ucSimSPI))
  #define IIC_BASE_ADD       ((unsigned char *)(&ucNE64.ucSimIIC))
  #define FLASH_BASE_ADD     ((unsigned char *)(&ucNE64.ucSimFLASH))
  #define EPHY_BASE_ADD      ((unsigned char *)(&ucNE64.ucSimEPHY))
  #define EMAC_BASE_ADD      ((unsigned char *)(&ucNE64.ucSimEMAC))
  #define PIM_BASE_ADD       ((unsigned char *)(&ucNE64.ucSimPIM))
#else
  #define MEBI_ADD            0x00
  #define CLK_RESET_BASE_ADD  0x34
  #define TIMERS_BASE_ADD     0x40
  #define ATD_BASE_ADD        0x80
  #define SCI_BASE_ADD        0xc8
  #define SPI_BASE_ADD        0xd8
  #define IIC_BASE_ADD        0xe0
  #define FLASH_BASE_ADD      0x100
  #define EPHY_BASE_ADD       0x120
  #define EMAC_BASE_ADD       0x140
  #define PIM_BASE_ADD        0x240
#endif


#define PORTA   *(volatile unsigned char*)(MEBI_ADD + 0x0)               // PORT A Data Register
#define PORTB   *(volatile unsigned char*)(MEBI_ADD + 0x1)               // PORT B Data Register
#define DDRA    *(volatile unsigned char*)(MEBI_ADD + 0x2)               // Data Direction register PORT A {4}
#define DDRB    *(volatile unsigned char*)(MEBI_ADD + 0x3)               // Data Direction register PORT B {4}

#define PORTE   *(volatile unsigned char*)(MEBI_ADD + 0x8)               // PORT E Data Register
#define DDRE    *(volatile unsigned char*)(MEBI_ADD + 0x9)               // Data Direction register PORT E {4}
  #define PE4_ECLK  0x10
  #define PE1_IRQ   0x02
  #define PE0_XIRQ  0x01
#define REAR    *(unsigned char*)(MEBI_ADD + 0xa)                        // PORT E assignment register
#define MODE    *(unsigned char*)(MEBI_ADD + 0xb)                        // Mode register
#define PUCR    *(unsigned char*)(MEBI_ADD + 0xc)                        // Pull-up control register
#define RDRIV   *(unsigned char*)(MEBI_ADD + 0xd)                        // reduced drive register
#define EBICTL  *(unsigned char*)(MEBI_ADD + 0xe)                        // External bus interface control register

#define INITRM  *(unsigned char*)(MEBI_ADD + 0x10)                       // Initialisation of Internal RAM Position Register
#define INITRG  *(unsigned char*)(MEBI_ADD + 0x11)                       // Initialisation of Internal Registers Position Register
#define INITEE  *(unsigned char*)(MEBI_ADD + 0x12)                       // Initialisation of Internal EEPROM Position Register
#define MISC    *(unsigned char*)(MEBI_ADD + 0x13)                       // Miscellaneous System Control Register
#define MTST0   *(unsigned char*)(MEBI_ADD + 0x14)                       // Reserved Test Register 0 (Read only)
#define ITCR    *(unsigned char*)(MEBI_ADD + 0x15)                       // Interrupt Test Control Register
#define ITEST   *(unsigned char*)(MEBI_ADD + 0x16)                       // Interrupt Test Register
#define MTST1   *(unsigned char*)(MEBI_ADD + 0x17)                       // Reserved Test Register 1 (Read only)

#define PARTID  *(unsigned short*)(MEBI_ADD + 0x1a)                      // Device ID Register
    #define MASK_SET_0L19S  0x8200
    #define MASK_SET_1L19S  0x8201
#define MEMSIZ0 *(unsigned char*)(MEBI_ADD + 0x1c)                       // Memory Size Register 0 (read only)
#define MEMSIZ1 *(unsigned char*)(MEBI_ADD + 0x1d)                       // Memory Size Register 1 (Read only)
#define IRQCR   *(unsigned char*)(MEBI_ADD + 0x1e)                       // IRQ Control Register
#define HPRIO   *(unsigned char*)(MEBI_ADD + 0x1f)                       // Highest Priority Interrupt (optional)
#define DBGC1   *(unsigned char*)(MEBI_ADD + 0x20)                       // Debug Control Register 1
#define DBGSC   *(unsigned char*)(MEBI_ADD + 0x21)                       // Debug Status and Control Register 
#define DBGTB   *(unsigned short*)(MEBI_ADD + 0x22)                      // Debug Trace Buffer Register 
#define DBGCNT  *(unsigned char*)(MEBI_ADD + 0x24)                       // Debug Count Register 
#define DBGCCX  *(unsigned char*)(MEBI_ADD + 0x25)                       // Debug Comparator C Extended Register 
#define DBGCC   *(unsigned short*)(MEBI_ADD + 0x26)                      // Debug Comparator C Register 
#define DBGC2   *(unsigned char*)(MEBI_ADD + 0x28)                       // Debug Control Register 2
#define DBGC3   *(unsigned char*)(MEBI_ADD + 0x29)                       // Debug Control Register 3
#define DBGCAX  *(unsigned char*)(MEBI_ADD + 0x2a)                       // Debug Comparator A Extended Register 
#define DBGCA   *(unsigned short*)(MEBI_ADD + 0x2b)                      // Debug Comparator A Register 
#define DBGCBX  *(unsigned char*)(MEBI_ADD + 0x2d)                       // Debug Comparator B Extended Register 
#define DBGCB   *(unsigned short*)(MEBI_ADD + 0x2e)                      // Debug Comparator B Register 
#define PPAGE   *(unsigned char*)(MEBI_ADD + 0x30)                       // Program Page Index Register 

#define PORTK   *(volatile  unsigned char*)(MEBI_ADD + 0x32)             // PORT K Data Register
#define DDRK    *(volatile unsigned char*)(MEBI_ADD + 0x33)              // PORT K Data Direction Register {4}

// Clock and Reset Generator
#define SYNR   *(unsigned char*)(CLK_RESET_BASE_ADD + 0)                 // Synthesizer register
#define REFDV  *(unsigned char*)(CLK_RESET_BASE_ADD + 1)                 // Reference divider register
#define CTFLG  *(unsigned char*)(CLK_RESET_BASE_ADD + 2)                 // Test flags register (production testing)
#define CRGFLG *(volatile unsigned char*)(CLK_RESET_BASE_ADD + 3)        // Real Time Clock Request Flag
 #define RTIF 0x80
 #define LOCK 0x08
#define CRGINT *(unsigned char*)(CLK_RESET_BASE_ADD + 4)                 // Real Time Clock Interrupt control
 #define RTIE 0x80
#define CLKSEL *(unsigned char*)(CLK_RESET_BASE_ADD + 5)                 // Clock select register
 #define PLLSEL 0x80
#define PLLCTL *(unsigned char*)(CLK_RESET_BASE_ADD + 6)                 // PLL control register
 #define PLLON 0x40
#define RTICTL *(unsigned char*)(CLK_RESET_BASE_ADD + 7)                 // Real Time Clock Control
#define COPCTL *(unsigned char*)(CLK_RESET_BASE_ADD + 8)                 // COP control register
#define FORBYP *(unsigned char*)(CLK_RESET_BASE_ADD + 9)                 // Force and Bypass test register (production testing)
#define CTCTL  *(unsigned char*)(CLK_RESET_BASE_ADD + 0xa)               // Test control register (production testing)
#define ARMCOP *(volatile unsigned char*)(CLK_RESET_BASE_ADD + 0xb)      // Arm/timer reset 


// Timers
#define TIOS     *(unsigned char*)(TIMERS_BASE_ADD + 0x00)
  #define IOS7 0x80                                                      // Input/Output compare
  #define IOS6 0x40
  #define IOS5 0x20
  #define IOS4 0x10
#define CFORC    *(unsigned char*)(TIMERS_BASE_ADD + 0x01)
  #define FOC7 0x80                                                     // Timer Compare Force Register
  #define FOC6 0x40
  #define FOC5 0x20
  #define FOC4 0x10
#define OC7M     *(unsigned char*)(TIMERS_BASE_ADD + 0x02)
  #define OC7M7 0x80                                                     // Output compare 7 mask register
  #define OC7M6 0x40
  #define OC7M5 0x20
  #define OC7M4 0x10
#define OC7D     *(unsigned char*)(TIMERS_BASE_ADD + 0x03)
  #define OC7D7 0x80                                                     // Output compare 7 data register
  #define OC7D6 0x40
  #define OC7D5 0x20
  #define OC7D4 0x10
#define TCNT_HI  *(unsigned short*)(TIMERS_BASE_ADD + 0x04)
//#define TCNT_HI  *(unsigned char*)(TIMERS_BASE_ADD + 0x04)
//#define TCNT_LO  *(unsigned char*)(TIMERS_BASE_ADD + 0x05)
#define TSCR1    *(unsigned char*)(TIMERS_BASE_ADD + 0x06)
  #define TEN   0x80                                                     // Timer enable 
  #define TSWAI 0x40                                                     // Stop timer module while in Wait
  #define TSFRZ 0x20                                                     // Stop time while in freeze mode
  #define TFFCA 0x10                                                     // Time fast flag clear all
#define TTOV     *(unsigned char*)(TIMERS_BASE_ADD + 0x07)
#define TCTL1    *(unsigned char*)(TIMERS_BASE_ADD + 0x08)
#define RES_1    *(unsigned char*)(TIMERS_BASE_ADD + 0x09)
#define TCTL3    *(unsigned char*)(TIMERS_BASE_ADD + 0x0a)
#define RES_2    *(unsigned char*)(TIMERS_BASE_ADD + 0x0b)
#define TTIE     *(unsigned char*)(TIMERS_BASE_ADD + 0x0c)
  #define C7I 0x80                                                       // timer channel interrupt mask
  #define C6I 0x40
  #define C5I 0x20
  #define C4I 0x10
#define TSCR2    *(unsigned char*)(TIMERS_BASE_ADD + 0x0d)
  #define LOW_RES_MS         (OSCCLK / 128000)                           // slowest clock rate to timer, giving longest period but minimum resolution
  #define MED_RES_MS         (OSCCLK / 32000)                            // medium clock rate to timer, giving medium period and medium resolution
  #define HIGH_RES_MS        (OSCCLK / 4000)                             // high clock rate to timer, giving minimum period and maximum resolution

  #define BUS_CLOCK_128  0x07                                            // timer clocked from bus clock divided by 128
  #define BUS_CLOCK_64   0x06                                            // timer clocked from bus clock divided by 64
  #define BUS_CLOCK_32   0x05                                            // timer clocked from bus clock divided by 32
  #define BUS_CLOCK_16   0x04                                            // timer clocked from bus clock divided by 16
  #define BUS_CLOCK_8    0x03                                            // timer clocked from bus clock divided by 8
  #define BUS_CLOCK_4    0x02                                            // timer clocked from bus clock divided by 4
  #define BUS_CLOCK_2    0x01                                            // timer clocked from bus clock divided by 2
  #define BUS_CLOCK_1    0x00                                            // timer clocked from bus clock divided by 1

  #define TCRE 0x08                                                      // enable conter reset on channel 7 compare
#define TFLG1    *(unsigned char*)(TIMERS_BASE_ADD + 0x0e)
  #define C7F 0x80                                                       // timer channel interrupt mask
  #define C6F 0x40
  #define C5F 0x20
  #define C4F 0x10
#define TFLG2    *(unsigned char*)(TIMERS_BASE_ADD + 0x0f)
#define RES_3    *(unsigned char*)(TIMERS_BASE_ADD + 0x10)
#define RES_4    *(unsigned char*)(TIMERS_BASE_ADD + 0x11)
#define RES_5    *(unsigned char*)(TIMERS_BASE_ADD + 0x12)
#define RES_6    *(unsigned char*)(TIMERS_BASE_ADD + 0x13)
#define RES_7    *(unsigned char*)(TIMERS_BASE_ADD + 0x14)
#define RES_8    *(unsigned char*)(TIMERS_BASE_ADD + 0x15)
#define RES_9    *(unsigned char*)(TIMERS_BASE_ADD + 0x16)
#define RES_10   *(unsigned char*)(TIMERS_BASE_ADD + 0x17)
#define TC4_HI   *(unsigned short*)(TIMERS_BASE_ADD + 0x18)
//#define TC4_LO   *(unsigned char*)(TIMERS_BASE_ADD + 0x19)
#define TC5_HI   *(unsigned short*)(TIMERS_BASE_ADD + 0x1a)
//#define TC5_LO   *(unsigned char*)(TIMERS_BASE_ADD + 0x1b)
#define TC6_HI   *(unsigned short*)(TIMERS_BASE_ADD + 0x1c)
//#define TC6_LO   *(unsigned char*)(TIMERS_BASE_ADD + 0x1d)
#define TC7_HI   *(unsigned short*)(TIMERS_BASE_ADD + 0x1e)
//#define TC7_LO   *(unsigned char*)(TIMERS_BASE_ADD + 0x1f)
#define PACTL    *(unsigned char*)(TIMERS_BASE_ADD + 0x20)
    #define PAEN  0x40
    #define PAMOD 0x20
    #define PEDGE 0x10
    #define PAOVI 0x02
#define PAFLG    *(unsigned char*)(TIMERS_BASE_ADD + 0x21)
    #define PAOVF 0x02
#define PACNT_WORD *(volatile unsigned short*)(TIMERS_BASE_ADD + 0x22)
#define PACNT_HI *(unsigned char*)(TIMERS_BASE_ADD + 0x22)
#define PACNT_LO *(unsigned char*)(TIMERS_BASE_ADD + 0x23)
#define RES_11   *(unsigned char*)(TIMERS_BASE_ADD + 0x24)
#define RES_12   *(unsigned char*)(TIMERS_BASE_ADD + 0x25)
#define RES_13   *(unsigned char*)(TIMERS_BASE_ADD + 0x26)
#define RES_14   *(unsigned char*)(TIMERS_BASE_ADD + 0x27)
#define RES_15   *(unsigned char*)(TIMERS_BASE_ADD + 0x28)
#define RES_16   *(unsigned char*)(TIMERS_BASE_ADD + 0x29)
#define RES_17   *(unsigned char*)(TIMERS_BASE_ADD + 0x2a)
#define RES_18   *(unsigned char*)(TIMERS_BASE_ADD + 0x2b)
#define RES_19   *(unsigned char*)(TIMERS_BASE_ADD + 0x2c)
#define TIMTST   *(unsigned char*)(TIMERS_BASE_ADD + 0x2d)
#define RES_20   *(unsigned char*)(TIMERS_BASE_ADD + 0x2e)
#define RES_21   *(unsigned char*)(TIMERS_BASE_ADD + 0x2f)

// Analog to Digital converter
#define ATDCTL0   *(unsigned char*)(ATD_BASE_ADD + 0)                    // ATD Control 0
#define ATDCTL1   *(unsigned char*)(ATD_BASE_ADD + 1)                    // ATD Control 1
#define ATDCTL2   *(unsigned char*)(ATD_BASE_ADD + 2)                    // ATD Control 2
  #define ADPU  0x80                                                     // ATD Power down
  #define AFFC  0x40                                                     // ATD Fast Flag Clear all
  #define AWAI  0x20                                                     // ATD Power down in Wait mode
  #define ASCIE 0x02                                                     // ATD Sequence complete interrupt enable
  #define ASCIF 0x01                                                     // ATD Sequence complete interrupt flag

#define ATDCTL3   *(unsigned char*)(ATD_BASE_ADD + 3)                    // ATD Control 3
#define ATDCTL4   *(unsigned char*)(ATD_BASE_ADD + 4)                    // ATD Control 4
  #define SREG8 0x80                                                     // ATD Resolution select
#define ATDCTL5   *(unsigned char*)(ATD_BASE_ADD + 5)                    // ATD Control 5
  #define DJM  0x80                                                      // ATD Result register data justification
  #define DSGN 0x40                                                      // ATD Result register signed or unsigned
  #define SCAN 0x20                                                      // ATD Scan mode
  #define MULT 0x10                                                      // ATD Multichannel sample mode
#define ATDSTAT0   *(unsigned char*)(ATD_BASE_ADD + 6)                   // ATD Status Register 0
  #define SCR   0x80                                                     // ATD Sequence complete flag
  #define ETORF 0x20                                                     // ATD External Trigger Overrun Flag
  #define FIFOR 0x10                                                     // ATD FIFO Overrun Flag
#define ATD_UNUSED1   *(unsigned char*)(ATD_BASE_ADD + 7)                // Unused
#define ATDTEST0   *(unsigned char*)(ATD_BASE_ADD + 8)                   // ATD Test 0
#define ATDTEST1   *(unsigned char*)(ATD_BASE_ADD + 9)                   // ATD Test 1
  #define SC 0x01                                                        // ATD Special Channel Convertion Bit
#define ATD_UNUSED_2   *(unsigned char*)(ATD_BASE_ADD + 0x0a)            // Unused
#define ATDSTAT1   *(unsigned char*)(ATD_BASE_ADD + 0x0b)                // ATD Status Register 1
  #define CCF7 0x80                                                      // ATD Channel 7 conversion complete
  #define CCF6 0x40                                                      // ATD Channel 6 conversion complete
  #define CCF5 0x20                                                      // ATD Channel 5 conversion complete
  #define CCF4 0x10                                                      // ATD Channel 4 conversion complete
  #define CCF3 0x08                                                      // ATD Channel 3 conversion complete
  #define CCF2 0x04                                                      // ATD Channel 2 conversion complete
  #define CCF1 0x02                                                      // ATD Channel 1 conversion complete
  #define CCF0 0x01                                                      // ATD Channel 0 conversion complete
#define ATD_UNUSED_3   *(unsigned char*)(ATD_BASE_ADD + 0x0c)            // Unused
#define ATDDIEN   *(unsigned char*)(ATD_BASE_ADD + 0x0d)                 // ATD Input Enable Register
  #define IEN7 0x80                                                      // ATD Digital Input Enable on channel 7
  #define IEN6 0x40                                                      // ATD Digital Input Enable on channel 6
  #define IEN5 0x20                                                      // ATD Digital Input Enable on channel 5
  #define IEN4 0x10                                                      // ATD Digital Input Enable on channel 4
  #define IEN3 0x08                                                      // ATD Digital Input Enable on channel 3
  #define IEN2 0x04                                                      // ATD Digital Input Enable on channel 2
  #define IEN1 0x02                                                      // ATD Digital Input Enable on channel 1
  #define IEN0 0x01                                                      // ATD Digital Input Enable on channel 0
#define ATD_UNUSED_4   *(unsigned char*)(ATD_BASE_ADD + 0x0e)            // Unused
#define PORTAD   *(unsigned char*)(ATD_BASE_ADD + 0x0f)                  // ATD Port Data Register
  #define PTAD7 0x80                                                     // ATD Port Data channel 7
  #define PTAD6 0x40                                                     // ATD Port Data channel 6
  #define PTAD5 0x20                                                     // ATD Port Data channel 5
  #define PTAD4 0x10                                                     // ATD Port Data channel 4
  #define PTAD3 0x80                                                     // ATD Port Data channel 3
  #define PTAD2 0x40                                                     // ATD Port Data channel 2
  #define PTAD1 0x20                                                     // ATD Port Data channel 1
  #define PTAD0 0x10                                                     // ATD Port Data channel 0
#define ATDDR0H   *(unsigned char*)(ATD_BASE_ADD + 0x10)                 // ATD Conversion result registers
#define ATDDR0L   *(unsigned char*)(ATD_BASE_ADD + 0x11)    
#define ATDDR1H   *(unsigned char*)(ATD_BASE_ADD + 0x12) 
#define ATDDR1L   *(unsigned char*)(ATD_BASE_ADD + 0x13) 
#define ATDDR2H   *(unsigned char*)(ATD_BASE_ADD + 0x14) 
#define ATDDR2L   *(unsigned char*)(ATD_BASE_ADD + 0x15) 
#define ATDDR3H   *(unsigned char*)(ATD_BASE_ADD + 0x16) 
#define ATDDR3L   *(unsigned char*)(ATD_BASE_ADD + 0x17) 
#define ATDDR4H   *(unsigned char*)(ATD_BASE_ADD + 0x18) 
#define ATDDR4L   *(unsigned char*)(ATD_BASE_ADD + 0x19) 
#define ATDDR5H   *(unsigned char*)(ATD_BASE_ADD + 0x1a) 
#define ATDDR5L   *(unsigned char*)(ATD_BASE_ADD + 0x1b) 
#define ATDDR6H   *(unsigned char*)(ATD_BASE_ADD + 0x1c) 
#define ATDDR6L   *(unsigned char*)(ATD_BASE_ADD + 0x1d) 
#define ATDDR7H   *(unsigned char*)(ATD_BASE_ADD + 0x1e) 
#define ATDDR7L   *(unsigned char*)(ATD_BASE_ADD + 0x1f) 

// port integration module
#define PTT  *(volatile unsigned char*)(PIM_BASE_ADD + 0)
  #define  PTT0  0x01
  #define  PTT1  0x02
  #define  PTT2  0x04
  #define  PTT3  0x08
  #define  PTT4  0x10
  #define  PTT5  0x20
  #define  PTT6  0x40
  #define  PTT7  0x80
#define PTIT *(unsigned char*)(PIM_BASE_ADD + 1)
#define DDRT *(volatile unsigned char*)(PIM_BASE_ADD + 2)
#define RDRT *(unsigned char*)(PIM_BASE_ADD + 3)
#define PERT *(unsigned char*)(PIM_BASE_ADD + 4)
#define PPST *(unsigned char*)(PIM_BASE_ADD + 5)
#define RES1 *(unsigned char*)(PIM_BASE_ADD + 6)
#define RES2 *(unsigned char*)(PIM_BASE_ADD + 7)
#define PTS  *(volatile unsigned char*)(PIM_BASE_ADD + 8)
#define PTIS *(unsigned char*)(PIM_BASE_ADD + 9)
#define DDRS *(volatile unsigned char*)(PIM_BASE_ADD + 0x0a)
#define RDRS *(unsigned char*)(PIM_BASE_ADD + 0x0b)
#define PERS *(unsigned char*)(PIM_BASE_ADD + 0x0c)
#define PPSS *(unsigned char*)(PIM_BASE_ADD + 0x0d)
#define WOMS *(unsigned char*)(PIM_BASE_ADD + 0x0e)
  #define  PTS0  0x01
  #define  PTS1  0x02
  #define  PTS2  0x04
  #define  PTS3  0x08
  #define  PTS4  0x10
  #define  PTS5  0x20
  #define  PTS6  0x40
  #define  PTS7  0x80
#define RES3 *(unsigned char*)(PIM_BASE_ADD + 0x0f)
#define PTG  *(volatile unsigned char*)(PIM_BASE_ADD + 0x10)
#define PTIG *(volatile unsigned char*)(PIM_BASE_ADD + 0x11)
#define DDRG *(volatile unsigned char*)(PIM_BASE_ADD + 0x12)
#define RDRG *(unsigned char*)(PIM_BASE_ADD + 0x13)
#define PERG *(unsigned char*)(PIM_BASE_ADD + 0x14)
#define PPSG *(unsigned char*)(PIM_BASE_ADD + 0x15)
#define PIEG *(unsigned char*)(PIM_BASE_ADD + 0x16)
  #define  PIEG0  0x01
  #define  PIEG1  0x02
  #define  PIEG2  0x04
  #define  PIEG3  0x08
  #define  PIEG4  0x10
  #define  PIEG5  0x20
  #define  PIEG6  0x40
  #define  PIEG7  0x80
#define PIFG *(unsigned char*)(PIM_BASE_ADD + 0x17)
  #define  PTG0  0x01
  #define  PTG1  0x02
  #define  PTG2  0x04
  #define  PTG3  0x08
  #define  PTG4  0x10
  #define  PTG5  0x20
  #define  PTG6  0x40
  #define  PTG7  0x80
#define PTH  *(volatile unsigned char*)(PIM_BASE_ADD + 0x18)
#define PTIH *(unsigned char*)(PIM_BASE_ADD + 0x19)
#define DDRH *(volatile unsigned char*)(PIM_BASE_ADD + 0x1a)
#define RDRH *(unsigned char*)(PIM_BASE_ADD + 0x1b)
#define PERH *(unsigned char*)(PIM_BASE_ADD + 0x1c)
#define PPSH *(unsigned char*)(PIM_BASE_ADD + 0x1d)
#define PIEH *(unsigned char*)(PIM_BASE_ADD + 0x1e)
#define PIFH *(unsigned char*)(PIM_BASE_ADD + 0x1f)
  #define  PTH0  0x01
  #define  PTH1  0x02
  #define  PTH2  0x04
  #define  PTH3  0x08
  #define  PTH4  0x10
  #define  PTH5  0x20
  #define  PTH6  0x40
  #define  PTH7  0x80
#define PTJ  *(volatile unsigned char*)(PIM_BASE_ADD + 0x20)
#define PTIJ *(unsigned char*)(PIM_BASE_ADD + 0x21)
#define DDRJ *(volatile unsigned char*)(PIM_BASE_ADD + 0x22)
  #define DDRJ0 0x01
  #define DDRJ1 0x02
#define RDRJ *(unsigned char*)(PIM_BASE_ADD + 0x23)
#define PERJ *(unsigned char*)(PIM_BASE_ADD + 0x24)
#define PPSJ *(unsigned char*)(PIM_BASE_ADD + 0x25)
#define PIEJ *(unsigned char*)(PIM_BASE_ADD + 0x26)
#define PIFJ *(unsigned char*)(PIM_BASE_ADD + 0x27)
  #define  PTJ0  0x01
  #define  PTJ1  0x02
  #define  PTJ2  0x04
  #define  PTJ3  0x08
  #define  PTJ4  0x10
  #define  PTJ5  0x20
  #define  PTJ6  0x40
  #define  PTJ7  0x80
#define PTL  *(volatile unsigned char*)(PIM_BASE_ADD + 0x28)             // link LEDs
  #define  PTL0  0x01
  #define  PTL1  0x02
  #define  PTL2  0x04
  #define  PTL3  0x08
  #define  PTL4  0x10
  #define  PTL5  0x20
  #define  PTL6  0x40
  #define  PTL7  0x80
  #define  ACTLED     PTL0                                               // dedicated port use when PHY controls LED outputs
  #define  LNKLED     PTL1
  #define  SPDLED     PTL2
  #define  DUPLED     PTL3
  #define  COLLED     PTL4
#define PTIL *(unsigned char*)(PIM_BASE_ADD + 0x29)
#define DDRL *(volatile unsigned char*)(PIM_BASE_ADD + 0x2a)
#define RDRL *(unsigned char*)(PIM_BASE_ADD + 0x2b)
#define PERL *(unsigned char*)(PIM_BASE_ADD + 0x2c)
#define PPSL *(unsigned char*)(PIM_BASE_ADD + 0x2d)
#define WOML *(unsigned char*)(PIM_BASE_ADD + 0x2e)


// SCI [0 followed by 1]
#define SCIBDH_0 *(unsigned char*)(SCI_BASE_ADD + 0x0)
#define SCIBDL_0 *(unsigned char*)(SCI_BASE_ADD + 0x1)
#define SCICR1_0 *(unsigned char*)(SCI_BASE_ADD + 0x2)
#define SCICR2_0 *(unsigned char*)(SCI_BASE_ADD + 0x3)
#define SCISR1_0 *(volatile unsigned char*)(SCI_BASE_ADD + 0x4)
#define SCISR2_0 *(unsigned char*)(SCI_BASE_ADD + 0x5)
#define SCIDRH_0 *(unsigned char*)(SCI_BASE_ADD + 0x6)
#define SCIDRL_0 *(unsigned char*)(SCI_BASE_ADD + 0x7)
#define SCI_BLOCK_SIZE 8                                                 // add this to address second block
#define SCIBDH_1 *(unsigned char*)(SCI_BASE_ADD + 0x8)
#define SCIBDL_1 *(unsigned char*)(SCI_BASE_ADD + 0x9)
#define SCICR1_1 *(unsigned char*)(SCI_BASE_ADD + 0xa)
#define SCICR2_1 *(unsigned char*)(SCI_BASE_ADD + 0xb)
#define SCISR1_1 *(volatile unsigned char*)(SCI_BASE_ADD + 0xc)
#define SCISR2_1 *(unsigned char*)(SCI_BASE_ADD + 0xd)
#define SCIDRH_1 *(unsigned char*)(SCI_BASE_ADD + 0xe)
#define SCIDRL_1 *(unsigned char*)(SCI_BASE_ADD + 0xf)


#define LOOPS  0x80                                                      // Loop select bit
#define DATA_FORMAT_MODE 0x10                                            // set = 9 bit / clear = 8 bit. !! when parity is enabled must be set otherwise
                                                                         // the 8th bit is set with parity !!!
#define PE     0x02                                                      // enable parity
#define PT     0x01                                                      // parity type - odd

#define TIE    0x80                                                      // Transmitter interrupt enable 
#define RIE    0x20                                                      // Receiver interrupt enable 
#define TE     0x08                                                      // Transmitter enable
#define RE     0x04                                                      // Receiver enable

#define TDRE   0x80
#define TC     0x40
#define RDRF   0x20


#define SPICR1  *(unsigned char*)(SPI_BASE_ADD + 0x0)
    #define SPIE   0x80                                                  // SPI Interrupt Enable Bit
    #define SPE    0x40                                                  // SPI System Enable Bit
    #define SPTIE  0x20                                                  // SPI Transmit Interrupt Enable
    #define MSTR   0x10                                                  // SPI Matser/Slave Mode Select Bit
    #define CPOL   0x08                                                  // SPI Clock Polarity Bit
    #define CPHA   0x04                                                  // SPI Clock Phase Bit
    #define SSOE   0x02                                                  // SPI Slave Select Output Enable
    #define LSBFE  0x01                                                  // LSB-First Enable
#define SPICR2  *(unsigned char*)(SPI_BASE_ADD + 0x1)
    #define MODFEN  0x10                                                 // Mode Fault Enable Bit
    #define BIDIROE 0x08                                                 // Output enable in Bidirectional mode
    #define SPISWAI 0x02                                                 // SPI Stop in Wait Bit
    #define SPC0    0x01                                                 // Serial Control Bit 0
#define SPIBR   *(unsigned char*)(SPI_BASE_ADD + 0x2)
    #define SPPR2 0x40
    #define SPPR1 0x20
    #define SPPR0 0x10
    #define SPRR2 0x04
    #define SPRR1 0x02
    #define SPRR0 0x01
#define SPISR   *(volatile unsigned char*)(SPI_BASE_ADD + 0x3)
    #define SPIF    0x80                                                 // SPIF Interrupt Flag
    #define SPTEF   0x20                                                 // SPI Transmit Empty Interrupt Flag
    #define MODF    0x10                                                 // Mode fault flag
#define SPIres1 *(unsigned char*)(SPI_BASE_ADD + 0x4)
#define SPIDR   *(volatile unsigned char*)(SPI_BASE_ADD + 0x5)
#define SPIres2 *(unsigned char*)(SPI_BASE_ADD + 0x6)
#define SPIres3 *(unsigned char*)(SPI_BASE_ADD + 0x7)

#define IBAD   *(unsigned char*)(IIC_BASE_ADD + 0x00)
#define IBFD   *(unsigned char*)(IIC_BASE_ADD + 0x01)
#define IBCR   *(unsigned char*)(IIC_BASE_ADD + 0x02)
    #define IBEN   0x80
    #define IBIE   0x40
    #define MS_SL  0x20
    #define TX_RX  0x10
    #define TXAK   0x08
    #define RSTA   0x04
    #define IBSWAI 0x01

#define IBSR   *(volatile unsigned char*)(IIC_BASE_ADD + 0x03)
    #define TCF    0x80
    #define IAAS   0x40
    #define IBB    0x20
    #define IBAL   0x10
    #define SRW    0x04
    #define IBIF   0x02                                                  // IIC bus interrupt
    #define RXACK  0x01
#define IBDR   *(volatile unsigned char*)(IIC_BASE_ADD + 0x04)

#define FCLKDIV *(unsigned char*)(FLASH_BASE_ADD + 0)
    #define FDIVLD    0x80                                               // Clock divider loaded
    #define PRDIV8    0x40                                               // prescaler / 8
    #define FDIV_MASK 0x3f                                               // divider mask
#define FSEC    *(unsigned char*)(FLASH_BASE_ADD + 1)
#define F_RES1  *(unsigned char*)(FLASH_BASE_ADD + 2)
#define FCNFG   *(unsigned char*)(FLASH_BASE_ADD + 3)
#define FPROT   *(unsigned char*)(FLASH_BASE_ADD + 4)
#define FSTAT   *(volatile unsigned char*)(FLASH_BASE_ADD + 5)
    #define CBEIF  0x80                                                  // command buffer empty interrupt flag
    #define CCIF   0x40                                                  // command complete interrupt flag
    #define PVIOL  0x20                                                  // protection violation flag
    #define ACCERR 0x10                                                  // access error flag
    #define BLANK  0x04                                                  // Flash block verified as erased
#define FCMD    *(unsigned char*)(FLASH_BASE_ADD + 6)
    #define CMD_PROGRAMM_WORD 0x20
    #define CMD_SECTOR_ERASE  0x40
    #define CMD_ERASE_VERIFY  0x05
    #define CMD_MASS_ERASE    0x41
#define F_RES2  *(unsigned char*)(FLASH_BASE_ADD + 7)
#define FADDRHI *(unsigned char*)(FLASH_BASE_ADD + 8)
#define FADDRLO *(unsigned char*)(FLASH_BASE_ADD + 9)
#define FDATAHI *(unsigned char*)(FLASH_BASE_ADD + 0xa)
#define FDATALO *(unsigned char*)(FLASH_BASE_ADD + 0xb)
#define F_RES3  *(unsigned char*)(FLASH_BASE_ADD + 0xc)
#define F_RES4  *(unsigned char*)(FLASH_BASE_ADD + 0xd)
#define F_RES5  *(unsigned char*)(FLASH_BASE_ADD + 0xe)
#define F_RES6  *(unsigned char*)(FLASH_BASE_ADD + 0xf)

#define FLASH_GRANULARITY                512                             // smallest sector which can be erased independently
#define MAX_SECTOR_PARS      ((FLASH_GRANULARITY - 4)/2)                 // The number of user bytes fitting into first parameter block

#define NO_ACCUMULATIVE_WORDS                                            // The NE64 FLASH must be writted as words. It is not allowed to write accumulative bits in words!!


#define EPHYCTL0 *(unsigned char*)(EPHY_BASE_ADD + 0)
  #define EPHYEN  0x80                                                   // enable EPHY
  #define ANDIS   0x40                                                   // disable autonegotiation
  #define DIS100  0x20                                                   // disable 100Mb clocks 
  #define DIS10   0x10                                                   // disable 10Mb clocks 
  #define LEDEN   0x08                                                   // enable LEDs from PHY module
  #define EPHYWAI 0x04                                                   // allow disable PHY in wait mode
  #define EPHYIEN 0x01                                                   // enable PHY interrupts

#define EPHYCTL1 *(unsigned char*)(EPHY_BASE_ADD + 1)
#define EPHYSR   *(unsigned char*)(EPHY_BASE_ADD + 2)
  #define EPHYIF 0x01                                                    // PHY interrupt occurred
#define EPHYTST  *(unsigned char*)(EPHY_BASE_ADD + 3)                    // factory register

#define NETCTL     *(unsigned char*)(EMAC_BASE_ADD + 0)
  #define FDX    0x02
  #define MLB    0x04
  #define EXTPHY 0x08
  #define EMACE  0x80
#define EMAC_RES_1 *(unsigned char*)(EMAC_BASE_ADD + 1)
#define EMAC_RES_2 *(unsigned char*)(EMAC_BASE_ADD + 2)
#define RXCTS  *(unsigned char*)(EMAC_BASE_ADD + 3)
  #define RFCE  0x10
  #define PROM  0x04
  #define CONMC 0x02
  #define BCREJ 0x01
#define TXCTS  *(volatile unsigned char*)(EMAC_BASE_ADD + 4)
  #define TXACT      0x80
  #define TCMD_START 0x01
  #define TCMD_PAUSE 0x02
  #define TCMD_ABORT 0x03
#define ETCTL  *(unsigned char*)(EMAC_BASE_ADD + 5)
  #define FIEEE 0x01
  #define FIPV4 0x02
  #define FARP  0x04
  #define FIPV6 0x08
  #define FEMW  0x10
  #define FPET  0x80
#define ETYPE  *(unsigned short*)(EMAC_BASE_ADD + 6)
#define PTIME  *(unsigned short*)(EMAC_BASE_ADD + 8)
#define IEVENT *(volatile unsigned short*)(EMAC_BASE_ADD + 0xa)
  #define MMCIF  0x0080                                                  // MII Transfer complete interrupt flag
#define IMASK  *(unsigned short*)(EMAC_BASE_ADD + 0xc)
  #define RFCIE  0x8000                                                  // Receive flow control interrupt enable
  #define BREIE  0x2000                                                  // Babbling Receive interrupt enable
  #define RXEIE  0x1000                                                  // Receive error interrupt enable
  #define RXAOIE 0x0800                                                  // Receive buffer A overrun interrupt enable
  #define RXBOIE 0x0400                                                  // Receive buffer B overrun interrupt enable
  #define RXACIE 0x0200                                                  // Valid frame reception A interrupt enable
  #define RXBCIE 0x0100                                                  // Valid frame reception B interrupt enable
  #define MMCIE  0x0080                                                  // MII Transfer complete interrupt enable
  #define LCIE   0x0020                                                  // Late collision interrupt enable
  #define ECIE   0x0010                                                  // Excessive collision interrupt enable
  #define TXCIE  0x0002                                                  // Frame transmission complete interrupt enable
#define SWRST  *(unsigned char*)(EMAC_BASE_ADD + 0xe)
  #define MACRST 0x80
#define EMAC_RES_3 *(unsigned char*)(EMAC_BASE_ADD + 0xf)
#define MPADR  *(unsigned char*)(EMAC_BASE_ADD + 0x10)
#define MRADR  *(unsigned char*)(EMAC_BASE_ADD + 0x11)
#define MWDATA *(unsigned short*)(EMAC_BASE_ADD + 0x12)
#define MRDATA *(unsigned short*)(EMAC_BASE_ADD + 0x14)
#define MCMST  *(volatile unsigned char*)(EMAC_BASE_ADD + 0x16)
  #define BUSY      0x20
  #define MII_WRITE 0x40
  #define MII_READ  0x80
#define EMAC_RES_4 *(unsigned char*)(EMAC_BASE_ADD + 0x17)
#define BUFCFG *(unsigned short*)(EMAC_BASE_ADD + 0x18)
  #define BUFFER_128 0x0000
  #define BUFFER_256 0x1000
  #define BUFFER_512 0x2000
  #define BUFFER_1K  0x3000
  #define BUFFER_1K5 0x4000
#define RXAEFP *(unsigned short*)(EMAC_BASE_ADD + 0x1a)
#define RXBEFP *(unsigned short*)(EMAC_BASE_ADD + 0x1c)
#define TXEFP  *(unsigned short*)(EMAC_BASE_ADD + 0x1e)
#define MCHASH_7  *(unsigned char*)(EMAC_BASE_ADD + 0x20)
#define MCHASH_6  *(unsigned char*)(EMAC_BASE_ADD + 0x21)
#define MCHASH_5  *(unsigned char*)(EMAC_BASE_ADD + 0x22)
#define MCHASH_4  *(unsigned char*)(EMAC_BASE_ADD + 0x23)
#define MCHASH_3  *(unsigned char*)(EMAC_BASE_ADD + 0x24)
#define MCHASH_2  *(unsigned char*)(EMAC_BASE_ADD + 0x25)
#define MCHASH_1  *(unsigned char*)(EMAC_BASE_ADD + 0x26)
#define MCHASH_0  *(unsigned char*)(EMAC_BASE_ADD + 0x27)

#define MACAD      (unsigned char*)(EMAC_BASE_ADD + 0x28)
#define MACAD_5   *(unsigned char*)(EMAC_BASE_ADD + 0x28)
#define MACAD_4   *(unsigned char*)(EMAC_BASE_ADD + 0x29)
#define MACAD_3   *(unsigned char*)(EMAC_BASE_ADD + 0x2a)
#define MACAD_2   *(unsigned char*)(EMAC_BASE_ADD + 0x2b)
#define MACAD_1   *(unsigned char*)(EMAC_BASE_ADD + 0x2c)
#define MACAD_0   *(unsigned char*)(EMAC_BASE_ADD + 0x2d)
#define EMISC     *(unsigned short*)(EMAC_BASE_ADD + 0x2e)

// PHY registers symbolic names
// (located in MII memory map, accessible through MDIO)
#define PHY_REG_CR      0x00                                             // Control Register 
#define PHY_REG_SR      0x01                                             // Status Register
#define PHY_REG_ID1     0x02                                             // PHY Identification Register 1 
#define PHY_REG_ID2     0x03                                             // PHY Identification Register 2
#define PHY_REG_ANAR    0x04                                             // Auto-Negotiation Advertisement Register 
#define PHY_REG_ANLPAR  0x05                                             // Auto-Negotiation Link Partner Ability Register 
#define PHY_REG_ER      0x06                                             // Auto-Negotiation Expansion Register 
#define PHY_REG_NPTR    0x07                                             // Auto-Negotiation Next Page Transfer Register
#define PHY_REG_ICR     0x10                                             // Interrupt Control Register 
  #define PHY_ACKIE 0x4000                                               // acknowledge bit received interrupt enable
  #define PHY_PRIE  0x2000                                               // Page received interrupt enable
  #define PHY_LCIE  0x1000                                               // Link changed interrupt enable
  #define PHY_ANIE  0x0800                                               // Auto negotiate changed interrupt enable
  #define PHY_PDFIE 0x0400                                               // Parallel detect fault interrupt enable
  #define PHY_RFIE  0x0200                                               // Remote fault interrupt enable
  #define PHY_JABIE 0x0100                                               // Jabber interrupt enable

  #define PHY_ACKR  0x0040                                               // acknowledge bit received
  #define PHY_PGR   0x0020                                               // Page received 
  #define PHY_LKC   0x0010                                               // Link changed
  #define PHY_ANC   0x0008                                               // Auto negotiate changed
  #define PHY_PDF   0x0004                                               // Parallel detect fault 
  #define PHY_RMTF  0x0002                                               // Remote fault 
  #define PHY_JABI  0x0001                                               // Jabber interrupt 

#define PHY_REG_PSR     0x11                                             // Proprietary Status Register
#define PHY_REG_PCR     0x12                                             // Proprietary Control Register 
#define PHY_REG_10BTBC  0x13                                             // 10Base-T Bypass Control Register 
#define PHY_REG_100BXBC 0x14                                             // 100Base-X Bypass Control Register 
#define PHY_REG_ADDR    0x15                                             // Test & Trim Control Register 
#define PHY_REG_DSPRC   0x17                                             // DSP Reset Control 
#define PHY_REG_DSPRR1  0x18                                             // 100Base-X DSP Read Registers 
#define PHY_REG_DSPRR2  0x19
#define PHY_REG_DSPRR3  0x1A
#define PHY_REG_DSPWR1  0x1B                                             // 100Base-X DSP Write Registers 
#define PHY_REG_DSPWR2  0x1C                              
#define PHY_REG_DSPWR3  0x1D


// 0 - Control Register
#define PHY_R0_RESET    0x8000                                           // Reset
#define PHY_R0_LB       0x4000                                           // Loop Back
#define PHY_R0_DR       0x2000                                           // Data Rate (100Mb/s)
#define PHY_R0_ANE      0x1000                                           // Auto-Negotiation Enable
#define PHY_R0_PD       0x0800                                           // Power Down
#define PHY_R0_ISOLATE  0x0400                                           // Isolate (MII is disconnected)
#define PHY_R0_RAN      0x0200                                           // Restart Auto-Negotiation
#define PHY_R0_DPLX     0x0100                                           // Duplex (Full duplex)
#define PHY_R0_CT       0x0080                                           // Collision Test (Enable)

// 4 - Auto Negotiation Advertisement Register
#define PHY_R4_NP       0x8000                                           // Next Page (capable of sending next pages)
#define PHY_R4_RF       0x2000                                           // Remote Fault
#define PHY_R4_FC       0x0400                                           // Flow Control
#define PHY_R4_100F     0x0100                                           // 100Base-TX Full Duplex Capable 
#define PHY_R4_100H     0x0080                                           // 100Base-TX Half Duplex Capable
#define PHY_R4_10F      0x0040                                           // 10Base-T Full Duplex Capable 
#define PHY_R4_10H      0x0020                                           // 10Base-T Half Duplex Capable 

////Proprietary Status Register
#define PHY_R17_LNK   0x4000    
#define PHY_R17_DPM   0x2000                                             //Duplex Mode
#define PHY_R17_SPD   0x1000                                             //Speed
#define PHY_R17_ANNC  0x0400                                             //Auto-Negotiation Complete
#define PHY_R17_PRCVD 0x0200
#define PHY_R17_ANCM  0x0100                                             // Auto-Negotiation (A-N) Common Operating Mode
#define PHY_R17_PLR   0x0020


#define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE 2



#define TIMER_INTERRUPT    1
// define interrupt setup structures to suit this processor              {2}
//
typedef struct stINTERRUPT_SETUP
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    int_priority;                                         // Priority the user wants to set
  unsigned char    int_port_bit;                                         // The IRQ input number (1..15)
  unsigned char    int_port_sense;                                       // Level sensitive, falling, rising, both
} INTERRUPT_SETUP;

typedef struct stTIMER_SETUP                                             
{
  void ( *int_handler )( void );                                         // Interrupt handler to be configured
  unsigned char    int_type;                                             // Identifier for when configuring
  unsigned char    channel;                                              // DMA timer channel (0..3)
  unsigned char    clock_divider;                                        // 0..255 (1..256)
  unsigned long    mode;                                                 // DMA timer operation mode
  unsigned long    count_delay;                                          // count delay until interrupt (clocks or external pulses)
} TIMER_SETUP;

#ifdef _CODE_WARRIOR                                                     // if compiling with Code Warrior
    #include "start12.h"
    extern struct _tagStartup _startupData;     
#endif


#define PORTS_AVAILABLE     11                                           // {3}
#define PORT_WIDTH          8

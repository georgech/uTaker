/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        NE64.c [FREESCALE MC9S12NE64]
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2019
   *********************************************************************

   25.02.2007 IIC control structure extended to array for project compatibility {1}
   11.05.2007 fnEraseFlashSector() : changed FILE_GRANULARITY to FLASH_GRANULARITY in SPI file system (increased flexibility) {2}
   11.05.2007 fnEraseFlashSector() in fnSetParameters() replaced by new fnDeleteParBlock() (increased flexibility) {3}
   27.05.2007 Changed to MILLISEC hardware timer method                  {4}
   22.09.2007 Add SPI FLASH support                                      {5}
   23.09.2007 Adapt fnEraseFlashSector() and its use in fnDeleteParBlock() {6}
   23.09.2007 Quieten GCC warnings with extra parentheses                {7}
   15.12.2007 Inform simulator of IIC speed                              {8}
   30.12.2007 Allow SPI and FLASH files system to work together          {9}
   31.12.2007 Enable accurate distinction between simulated and Window's memory {10}
   31.12.2007 Enable files from internal memory                          {11}
   12.05.2008 Add fnStopHW_Timer()                                       {12}
   13.05.2008 Add fnConfigureInterrupt()                                 {13}
   27.08.2008 Modify fnTxIIC Channel parameter to QUEUE_HANDLE           {14}
   04.03.2009 Change use of random number seed                           {15}

*/

#ifdef _HW_NE64

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
#ifdef _WINDOWS
  #include "config.h"
  #define INITHW  extern
  extern void fnOpenDefaultHostAdapter(void);
#else
  #define OPSYS_CONFIG                                                   // this module owns the operating system configuration
  #define INITHW  static
  #include "config.h"
#endif

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {5}
    static void fnConfigSPIFileSystem(unsigned char ucSpeed);
    #ifdef FLASH_FILE_SYSTEM
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckAT45dbxxx(int iChipSelect);
        #else
        static unsigned char fnCheckAT45dbxxx(void);
        #endif
    #endif
#endif


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#ifdef COMPILE_IAR                                                       // interrupt vector configuration as required by IAR compiler
 #pragma language=extended

 #define         EMAC_EC_VECTOR       0xFFA0                             // 16 0xffa0 EMAC interrupt
 #define         EMAC_LC_VECTOR       0xFFA2                             // 17 0xffa2 interrupt
 #define         EMAC_B_RX_ERR_VECTOR 0xFFA4                             // 18 0xffa4 interrupt
 #define         EMAC_RX_B_BO_VECTOR  0xFFA6                             // 19 0xffa6 interrupt
 #define         EMAC_RX_B_A_O_VECTOR 0xFFA8                             // 20 0xffa8 interrupt
 #define         EMAC_RX_ERR_VECTOR   0xFFAA                             // 21 0xffaa interrupt
 #define         EMAC_MII_MTC_VECTOR  0xFFAC                             // 22 0xffac interrupt
 #define         EMAC_RX_FC_VECTOR    0xFFAE                             // 23 0xffae EMAC interrupt
 #define         EMAC_F_TX_C_VECTOR   0xFFB0                             // 24 0xffb0 EMAC interrupt
 #define         EMAC_B_B_C_VECTOR    0xFFB2                             // 25 0xffb2 EMAC interrupt
 #define         EMAC_B_A_C_VECTOR    0xFFB4                             // 26 0xffb4 EMAC interrupt
 #define         PHY_VECTOR           0xFFB6                             // 27 0xffb6 PHY interrupt

 #define         IIC_VECTOR           0xFFC0                             // 32 0xffc0 IIC interrupt

 #define         PORTH_VECTOR         0xFFCC                             // 38 0xffcc PORT H interrupt

 #define         SCI1_VECTOR          0xFFD4                             // 42 0xffd4 (0xf7d4 with debugger) SCI1
 #define         SCI0_VECTOR          0xFFD6                             // 43 0xffd6 SCI0 (only without debugger)
 #define         SPI_VECTOR           0xFFD8                             // 44 0xffd8 SPI

 #define         ACC_OVERFLOW_VECTOR  0xFFDC                             // 46 Default interrupt 0xffdc PULSE OVERFLOW
 #define         STD_TIMER_OVERFLOW   0xFFDE                             // 48
 #define         STD_TIMER_CHAN_7     0xFFE0                             // 49
 #define         STD_TIMER_CHAN_6     0xFFE2                             // 50
 #define         STD_TIMER_CHAN_5     0xFFE4                             // 51
 #define         STD_TIMER_CHAN_4     0xFFE6                             // 52

 #define         RTI_VECTOR           0xFFF0                             // 56 0xfff0 RTI
 #define         IRQ_VECTOR           0xFFF2                             // 57 Default interrupt 0xfff2 IRQ
 #define         XIRQ_VECTOR          0xFFF4                             // 58 Default interrupt 0xfff4 XIRQ
 #define         SWI_BDM_VECTOR       0xFFF6                             // 59 Default interrupt 0xfff6 SWI or BDM
 #define         TRAP_VECTOR          0xFFF8                             // 60 Default interrupt 0xfff8 TRAP (unimplemented op code)
 #define         COP_RESET_VECTOR     0xFFFA                             // 61 Default interrupt 0xfffa COP reset
 #define         CLOCK_MON_VACTOR     0xFFFC                             // 62 Default interrupt 0xfffc CRYSTAL MONITOR
 #define         RESET_VECTOR         0xFFFE                             // Reset vector 0xfffe (0xf7fe with debugger)
#else
  #ifdef _GNU
    #define __interrupt
  #endif
  #ifdef _WINDOWS
    #define __interrupt
  #endif
#endif

#if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM                 // {5}
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];
    #else
    static int SPI_FLASH_Danger[1] = {0};
    static unsigned char ucSPI_FLASH_Type[1];
    #endif

    #define MANUFACTURER_ID_ATMEL      0x1F                              // D-version manufacturer's ID
    #define DEVICE_ID_1_DATA_FLASH_4M  0x24                              // D-version FLASH size - 4MBit / 0.5MegByte
    #define DEVICE_ID_1_DATA_FLASH_8M  0x25                              // D-version FLASH size - 8MBit / 1MegByte
    #define DEVICE_ID_1_DATA_FLASH_16M 0x26                              // D-version FLASH size - 16MBit / 2MegByte

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        unsigned char ucChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #ifdef CS2_LINE
            ,CS2_LINE
            #ifdef CS3_LINE
            ,CS3_LINE
            #endif
        #endif
        };
        #define EXTENDED_CS , &iChipSelect
        #define _EXTENDED_CS  iChipSelect,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif
#ifndef CS0_LINE                                                         // for compatibility when signals are not defined by user
    #define CS0_LINE    CS_OUTPUT
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#ifdef ETH_INTERFACE
    static UTASK_TASK EthernetWake = 0;                                  // Task to wake on ethernet events

    // We set two pointers to the rx Ethernet buffers. The first is fixed but the second depends on buffer size
    static unsigned char *ptrRxBuf[NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE];
    static unsigned char *ptrTxBuf;                                      // The position of the tx buffer depends on the size of RX buffers
#endif

unsigned short *ptrSeed;


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */
#ifdef ETH_INTERFACE
    extern ETHERNETQue *eth_tx_control;                                  // Ethernet structure pointers for Ethernet Interrupt handling
    extern ETHERNETQue *eth_rx_control;
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

#ifndef _WINDOWS
 #ifndef COMPILE_IAR                                                   // Interrupt vectors as required by the GNU/CodeWarrior compiler
   #ifdef _CODE_WARRIOR
    static void Cpu_Interrupt(void);
    static void emac_ec_isr(void);
    static void emac_lc_isr(void);
    static void emac_b_rx_error_isr(void);
    static void emac_rx_b_b_o_isr(void);
    static void emac_rx_b_a_o_isr(void);
    static void emac_rx_error_isr(void);
    static void emac_mii_mtc_isr(void);
    static void emac_rx_fc_isr(void);
    static void emac_f_tx_c_isr(void);
    static void emac_rx_b_b_c_isr(void);
    static void emac_rx_b_a_c_isr(void);
    static void ephy_isr(void);
    static void SPI_Interrupt(void);
    static void Timer_7_int(void);
    static void RealTimeInterrupt(void);
    static void SCI1_Interrupt(void);
    static void SCI0_Interrupt(void);
    static void IIC_Interrupt(void);
   #else
    static void Cpu_Interrupt(void) __attribute__((interrupt));
    static void emac_ec_isr(void) __attribute__((interrupt));
    static void emac_lc_isr(void) __attribute__((interrupt));
    static void emac_b_rx_error_isr(void) __attribute__((interrupt));
    static void emac_rx_b_b_o_isr(void) __attribute__((interrupt));
    static void emac_rx_b_a_o_isr(void) __attribute__((interrupt));
    static void emac_rx_error_isr(void) __attribute__((interrupt));
    static void emac_mii_mtc_isr(void) __attribute__((interrupt));
    static void emac_rx_fc_isr(void) __attribute__((interrupt));
    static void emac_f_tx_c_isr(void) __attribute__((interrupt));
    static void emac_rx_b_b_c_isr(void) __attribute__((interrupt));
    static void emac_rx_b_a_c_isr(void) __attribute__((interrupt));
    static void ephy_isr(void) __attribute__((interrupt));
    #ifdef SPI_INTERFACE
    static void SPI_Interrupt(void) __attribute__((interrupt));
    #endif
    static void Timer_7_int(void) __attribute__((interrupt));
    static void RealTimeInterrupt(void) __attribute__((interrupt));
    #ifdef SERIAL_INTERFACE
    static void SCI1_Interrupt(void) __attribute__((interrupt));
    static void SCI0_Interrupt(void) __attribute__((interrupt));
    #endif
    #ifdef IIC_INTERFACE
    static void IIC_Interrupt(void) __attribute__((interrupt));
    #endif
   #endif
 #endif
    static void fnInitHW(void);
#endif


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

#ifndef _WINDOWS
  #ifdef _CODE_WARRIOR
    #define _start _Startup
  #endif
    extern __interrupt void _start(void);                                // reset vector location
#endif


/* =================================================================== */
/*                      STARTUP CODE                                   */
/* =================================================================== */


// This is the interrupt vector table for the processor including the reset vector (entry point)
// It is at a fixed address
typedef void (*tIsrFunc)(void);

#ifndef _WINDOWS
 #ifndef COMPILE_IAR                                                     // interrupt table for GNU/Code Warrior compiler use
   #ifdef _CODE_WARRIOR
  const tIsrFunc _vect[] @0xFF80 =
   #else
  const tIsrFunc __attribute__((section(".vectors"))) _vect[] =
   #endif
     {
        Cpu_Interrupt,                                                   // 0 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 1 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 2 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 3 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 4 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 5 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 6 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 7 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 8 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 9 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 10 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 11 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 12 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 13 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 14 Default (unused) interrupt
        Cpu_Interrupt,                                                   // 15 Default (unused) interrupt
 #ifdef ETH_INTERFACE
        emac_ec_isr,                                                     // 16 0xffa0 EMAC interrupt
        emac_lc_isr,                                                     // 17 0xffa2 interrupt
        emac_b_rx_error_isr,                                             // 18 0xffa4 interrupt
        emac_rx_b_b_o_isr,                                               // 19 0xffa6 interrupt
        emac_rx_b_a_o_isr,                                               // 20 0xffa8 interrupt
        emac_rx_error_isr,                                               // 21 0xffaa interrupt
        emac_mii_mtc_isr,                                                // 22 0xffac interrupt
        emac_rx_fc_isr,                                                  // 23 0xffae EMAC interrupt
        emac_f_tx_c_isr,                                                 // 24 0xffb0 EMAC interrupt
        emac_rx_b_b_c_isr,                                               // 25 0xffb2 EMAC interrupt
        emac_rx_b_a_c_isr,                                               // 26 0xffb4 EMAC interrupt
        ephy_isr,                                                        // 27 0xffb6 PHY interrupt
 #else
        Cpu_Interrupt,                                                   // 16 0xffa0 EMAC interrupt
        Cpu_Interrupt,                                                   // 17 0xffa2 interrupt
        Cpu_Interrupt,                                                   // 18 0xffa4 interrupt
        Cpu_Interrupt,                                                   // 19 0xffa6 interrupt
        Cpu_Interrupt,                                                   // 20 0xffa8 interrupt
        Cpu_Interrupt,                                                   // 21 0xffaa interrupt
        Cpu_Interrupt,                                                   // 22 0xffac interrupt
        Cpu_Interrupt,                                                   // 23 0xffae EMAC interrupt
        Cpu_Interrupt,                                                   // 24 0xffb0 EMAC interrupt
        Cpu_Interrupt,                                                   // 25 0xffb2 EMAC interrupt
        Cpu_Interrupt,                                                   // 26 0xffb4 EMAC interrupt
        Cpu_Interrupt,                                                   // 27 0xffb6 PHY interrupt
 #endif
        Cpu_Interrupt,                                                   // 28 0xffb8 FLASH interrupt
        Cpu_Interrupt,                                                   // 29 0xffba (unused) interrupt
        Cpu_Interrupt,                                                   // 30 0xffbc (unused) interrupt
        Cpu_Interrupt,                                                   // 31 0xffbe (unused) interrupt
 #ifdef IIC_INTERFACE
        IIC_Interrupt,                                                   // 32 0xffc0 IIC interrupt
 #else
        Cpu_Interrupt,                                                   // 32 0xffc0 IIC interrupt
 #endif
        Cpu_Interrupt,                                                   // 33 0xffc2 (unused) interrupt
        Cpu_Interrupt,                                                   // 34 0xffc4 CRG interrupt
        Cpu_Interrupt,                                                   // 35 0xffc6 PLL interrupt
        Cpu_Interrupt,                                                   // 36 0xffc8 (unused) interrupt
        Cpu_Interrupt,                                                   // 37 0xffca PORT G interrupt
        Cpu_Interrupt,                                                   // 38 0xffcc PORT H interrupt
        Cpu_Interrupt,                                                   // 39 0xffce PORT J interrupt
        Cpu_Interrupt,                                                   // 40 0xffd0 (unused) interrupt
        Cpu_Interrupt,                                                   // 41 0xffd2 ATD interrupt
 #ifdef SERIAL_INTERFACE
        SCI1_Interrupt,                                                  // 42 0xffd4 (0xf7d4 with debugger) SCI1
        SCI0_Interrupt,                                                  // 43 0xffd6 SCI0 (only without debugger)
 #else
        Cpu_Interrupt,                                                   // 42 0xffd4 (0xf7d4 with debugger) SCI1
        Cpu_Interrupt,                                                   // 43 0xffd6 SCI0 (only without debugger)
 #endif
 #ifdef SPI_INTERFACE
        SPI_Interrupt,                                                   // 44 0xffd8 SPI
 #else
        Cpu_Interrupt,                                                   // 44 0xffd8 SPI
 #endif
        Cpu_Interrupt,                                                   // 45 Default (unused) interrupt 0xffda PULSE ACC INPUT EDGE
        Cpu_Interrupt,                                                   // 46 Default interrupt 0xffdc PULSE OVERFLOW
        Cpu_Interrupt,                                                   // 47 Default (unused) interrupt 0xffde TIM OVERFLOW
 #ifdef GLOBAL_HARDWARE_TIMER
        Timer_7_int,                                                     // 58 0xffe0 TIM 7
 #else
        Cpu_Interrupt,                                                   // 48 Default (unused) interrupt 0xffe0 TIM 7
 #endif
        Cpu_Interrupt,                                                   // 49 Default (unused) interrupt 0xffe2 TIM 6
        Cpu_Interrupt,                                                   // 50 Default (unused) interrupt 0xffe4 TIM 5
        Cpu_Interrupt,                                                   // 51 Default (unused) interrupt 0xffe4 TIM 4
        Cpu_Interrupt,                                                   // 52 Default (unused) interrupt 0xffe8 - Res.
        Cpu_Interrupt,                                                   // 53 Default (unused) interrupt 0xffea - Res.
        Cpu_Interrupt,                                                   // 54 Default (unused) interrupt 0xffec - Res.
        Cpu_Interrupt,                                                   // 55 Default (unused) interrupt 0xffee - Res.
        RealTimeInterrupt,                                               // 56 0xfff0 RTI
        Cpu_Interrupt,                                                   // 57 Default (unused) interrupt 0xfff2 IRQ
        Cpu_Interrupt,                                                   // 58 Default (unused) interrupt 0xfff4 XIRQ
        Cpu_Interrupt,                                                   // 59 Default (unused) interrupt 0xfff6 SWI or BDM
        Cpu_Interrupt,                                                   // 60 Default (unused) interrupt 0xfff8 TRAP (unimplementaed op code)
        Cpu_Interrupt,                                                   // 61 Default (unused) interrupt 0xfffa COP reset
        Cpu_Interrupt,                                                   // 62 Default (unused) interrupt 0xfffc CRYSTAL MONITOR
        _start                                                           // Reset vector 0xfffe (0xf7fe with debugger)
   };
 #endif


    #if defined (COMPILE_IAR)
        #pragma segment="DATA16_Z"
        #define HEAP_START_ADDRESS __segment_end("DATA16_Z")             // IAR compiler - last location of static variables
    #elif defined (_CODE_WARRIOR)
         #include "start12.h"
         #define HEAP_START_ADDRESS (unsigned char *)(_startupData.stackOffset)
    #else                                                                // GNU
         extern unsigned char _end;
         #define HEAP_START_ADDRESS &_end                                // GNU last location of static variables
    #endif



// main entry for the target code
//
int main(void)
{
#ifdef MULTISTART
    MULTISTART_TABLE *prtInfo;
    unsigned char *pucHeapStart = HEAP_START_ADDRESS;
#endif
#ifdef RANDOM_NUMBER_GENERATOR
    unsigned short usRandomSeed;                                         // we put an uninitialised variable on to the stack for use as a random seed
    ptrSeed = &usRandomSeed;
#endif

    fnInitHW();                                                          // perform hardware initialisation (note that we do not have heap yet)
#ifdef MULTISTART
    prtInfo = ptMultiStartTable;                                         // if the user has already set to alternative start configuration
    if (prtInfo == 0) {                                                  // no special start required
        fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                 // initialise heap
        uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system
        while ((prtInfo = (MULTISTART_TABLE*)uTaskerSchedule()) == 0) {} // schedule uTasker
    }
#else
    fnInitialiseHeap(ctOurHeap, HEAP_START_ADDRESS);                     // initialise heap
    uTaskerStart((UTASKTABLEINIT *)ctTaskTable, ctNodes, PHYSICAL_QUEUES); // start the operating system
#endif

#ifdef MULTISTART
    do {
        while ((prtInfo = (MULTISTART_TABLE*)uTaskerSchedule()) == 0) {} // schedule uTasker
        if (prtInfo->new_hw_init) {                                      // info to next task configuration available
            pucHeapStart = prtInfo->new_hw_init(JumpTable);              // get heap details from next configuration
        }

        fnInitialiseHeap(prtInfo->ptHeapNeed, pucHeapStart);             // initialise the new heap
                                                                         // start the operating system with next configuration
        uTaskerStart((UTASKTABLEINIT *)prtInfo->ptTaskTable, prtInfo->ptNodesTable, PHYSICAL_QUEUES);
    } while (1);
#else
    while (1) {
        uTaskerSchedule();                                               // schedule uTasker
    }
#endif
    return 0;
}
#endif

#ifdef RANDOM_NUMBER_GENERATOR
// How the random number seed is set depends on the hardware possibilities available.
//
extern void fnInitialiseRND(unsigned short *usSeedValue)                 // {15}
{
    if ((*usSeedValue = *ptrSeed) == 0) {                                // we hope that the content of the random seed is random after a power up
        *usSeedValue = 0x127b;                                           // we must not set a zero - so we set a fixed value
    }                                                                    // after a reset it should be well random since it will contain the value at the reset time
    *ptrSeed = fnRandom();                                               // ensure that the seed value is modified between resets
}
#endif


// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#ifdef _WINDOWS
    unsigned char ucPortPullups[] = {
        PTA_DEFAULT_INPUT,                                               // set the port states out of reset in the project file app_hw_ne64.h
        PTB_DEFAULT_INPUT,
        PTE_DEFAULT_INPUT,
        PTG_DEFAULT_INPUT,
        PTH_DEFAULT_INPUT,
        PTJ_DEFAULT_INPUT,
        PTK_DEFAULT_INPUT,
        PTL_DEFAULT_INPUT,
        PTS_DEFAULT_INPUT,
        PTT_DEFAULT_INPUT,
        PAD_DEFAULT_INPUT
    };
    static unsigned short usRandomSeed = 0;
    ptrSeed = &usRandomSeed;
    fnInitialiseDevice((void *)ucPortPullups);
#endif
    fnUserHWInit();                                                      // allow user hardware configuration

    if (!(CLKSEL & PLLSEL)) {                                            // we set the PLL if it has not already been set by the debugger
                                                                         // not set so we have not started with debugger support
        PLLCTL |= PLLON;                                                 // turn on PLL

        while (!(CRGFLG & LOCK)) {                                       // wait for lock to occur
#ifdef _WINDOWS
            CRGFLG |= LOCK;                                              // simulate PLL locking
#endif
        }
        CLKSEL |= PLLSEL;                                                // set PLL as system clock
        COPCTL = 0x47;                                                   // set max COP period of 0.6s (only when no debugger) - stop when in BDM
    }

    INIT_WATCHDOG_LED();                                                 // allow user configuration of a blink LED
    INIT_WATCHDOG_DISABLE();
    if (!WATCHDOG_DISABLE()) {
        ARMCOP = 0x55;
        ARMCOP = 0xaa;
    }


#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #ifdef FLASH_FILE_SYSTEM
    fnConfigSPIFileSystem(MAX_SPI);                                      // configure SPI interface for maximum speed {5}

        #ifdef SPI_FLASH_MULTIPLE_CHIPS
    ucSPI_FLASH_Type[0] = fnCheckAT45dbxxx(0);                           // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckAT45dbxxx();                            // flag whether the SPI FLASH device is connected
        #endif
        #if SPI_FLASH_PAGES == 512
    if (ucSPI_FLASH_Type[0] < AT45DB011B)                                // we expect at least this part to be available
        #else
    if (ucSPI_FLASH_Type[0] < AT45DB021B)                                // we expect at least this part to be available
        #endif
    {
        SPICR1 = 0;                                                      // disable SPI if no chip connected
    }
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // check for further devices
    else {
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckAT45dbxxx(i);
        }
    }
        #endif
    #else
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
    #endif
#endif
}

// Support watchdog re-triggering of specific hardware
//
void fnRetriggerWatchdog(void)
{
    ARMCOP = 0x55;
    ARMCOP = 0xaa;

    TOGGLE_WATCHDOG_LED();
}

// Routine to disable interrupts during critical region
//
void uDisable_Interrupt()
{
#ifndef _WINDOWS
    asm("sei");
#endif
}

// Routine to re-enable interrupts on leaving a critical region
//
void uEnable_Interrupt()
{
#ifndef _WINDOWS
    asm("cli");
#endif
}


// Routine to initialise the Real Time Tick interrupt
//
#define REQUIRED_MS ((1000/TICK_RESOLUTION))                             // the TICK frequency we require in kHz
#define TICK_DIVIDE (OSCCLK/REQUIRED_MS)                                 // the divide ration required
#define REGISTER_VALUE (((TICK_DIVIDE + 65536/2)/65536) - 1)             // we assume 2^16 prescaler to give range 2.5...40ms

extern void fnStartTick(void)
{
    RTICTL = (0x70 + REGISTER_VALUE);                                    // set the prescaler to generate the periodic tick
    CRGFLG = RTIF;                                                       // reset interrupt request flag
    CRGINT = RTIE;                                                       // enable interrupt
}



/**************************** Real Time Clock interrupt ******************************************/
#ifdef COMPILE_IAR
 #pragma vector=RTI_VECTOR
#endif
__interrupt void RealTimeInterrupt(void)
{
  CRGFLG = RTIF;                                                         // reset interrupt request flag
  fnRtmkSystemTick();                                                    // operating system tick
}

#ifdef GLOBAL_HARDWARE_TIMER
/**************************** Timer 7 interrupt **********************************************/
#ifdef COMPILE_IAR
 #pragma vector=STD_TIMER_CHAN_7
#endif
__interrupt void Timer_7_int(void)
{
  static const unsigned char hw_timer_int_message[HEADER_LENGTH] = { INTERNAL_ROUTE, INTERNAL_ROUTE, TASK_TIMER, INTERRUPT_EVENT, 0 };

  TFLG1 = C7F;                                                           // reset interrupt request flag
  TSCR1 = 0;                                                             // stop the timer
  fnWrite(INTERNAL_ROUTE, (unsigned char*)hw_timer_int_message, HEADER_LENGTH); // inform the global interrupt task
}
#endif

/**************************** Non-defined interrupt **********************************************/
#ifndef COMPILE_IAR
void Cpu_Interrupt(void)
{                                                                        // not expected and no handling defined
}
#endif



#ifdef ETH_INTERFACE
/* =================================================================== */
/*                          Ethernet Interface                         */
/* =================================================================== */

#define PHY_ADDRESS 0x5

static unsigned char EMAC_int_message[ HEADER_LENGTH ] = { 0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, 0 }; // define standard interrupt event

static const unsigned char EMAC_RXA_int_message[ HEADER_LENGTH ] = { 0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RXA_INTERRUPT };    // define fixed interrupt event
static const unsigned char EMAC_RXB_int_message[ HEADER_LENGTH ] = { 0, 0 , TASK_ETHERNET, INTERRUPT_EVENT, EMAC_RXB_INTERRUPT };    // define fixed interrupt event

#include "../../stack/tcpip.h"
static unsigned short fnMIIread(unsigned char _mpadr, unsigned char _mradr);


// This is called when an Ethernet event has been received. Generally it is due to a reception and returns chanel and message details.
//
extern signed char fnEthernetEvent(unsigned char *ucEvent, ETHERNET_FRAME *rx_frame)
{
#ifdef LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_int_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_RX_INTERRUPT };
#endif
    if (EMAC_RXA_INTERRUPT == *ucEvent) {
      if ((rx_frame->frame_size = fnGetDataBlock(Ethernet_handle, 0, (unsigned char **)&rx_frame->ptEth)) == 0) { // get frame details
          return -1;
      }
      *ucEvent = EMAC_RXB_INTERRUPT;                                     // set to next buffer
#ifdef LAN_REPORT_ACTIVITY
      fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the task of event
#endif
      return 0;                                                          // channel received from
    }
    else if (EMAC_RXB_INTERRUPT == *ucEvent) {
      if ((rx_frame->frame_size = fnGetDataBlock(Ethernet_handle, 1, (unsigned char **)&rx_frame->ptEth)) == 0) { // get frame details
          return -1;
      }
      *ucEvent = EMAC_RXA_INTERRUPT;                                     // set to next buffer
#ifdef LAN_REPORT_ACTIVITY
      fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the task of event
#endif
      return 1;                                                          // channel received from
    }
#ifdef USE_IP_STATS
    else if (ETHERNET_RX_OVERRUN == *ucEvent) {
        fnIncrementEthernetStats(TOTAL_LOST_RX_FRAMES);                  // we lost a frame due to RX overrun so count the event
    }
    else {
        fnIncrementEthernetStats(TOTAL_OTHER_EVENTS);                    // count other unexpected events
    }
#endif
    return -1;                                                           // chanel invalid
}


/**************************** late collision Ethernet interrupt ******************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_LC_VECTOR
#endif
__interrupt void emac_lc_isr(void)
{
  IEVENT = LCIE;                                                         // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_COLLISION;            // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** excessive collisions Ethernet interrupt ************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_EC_VECTOR
#endif
__interrupt void emac_ec_isr(void)
{
  IEVENT = ECIE;                                                         // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_EXESSIVE_COLLISION;   // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** babbling receive Ethernet interrupt ***************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_B_RX_ERR_VECTOR
#endif
__interrupt void emac_b_rx_error_isr(void)
{
  IEVENT = BREIE;                                                        // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = BABBLING_RX;                   // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** rx buffer A overrun Ethernet interrupt ************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_RX_B_A_O_VECTOR
#endif
__interrupt void emac_rx_b_a_o_isr(void)
{
  IEVENT = RXAOIE;                                                       // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_RX_OVERRUN;           // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

 /**************************** rx buffer B overrun Ethernet interrupt ************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_RX_B_BO_VECTOR
#endif
__interrupt static void emac_rx_b_b_o_isr(void)
{
  IEVENT = RXBOIE;                                                       // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_RX_OVERRUN;           // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** receive error Ethernet interrupt *******************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_RX_ERR_VECTOR
#endif
__interrupt void emac_rx_error_isr(void)
{
  IEVENT = RXEIE;                                                        // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_RX_ERROR;             // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** flow control Ethernet interrupt ********************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_RX_FC_VECTOR
#endif
__interrupt void emac_rx_fc_isr(void)
{
  IEVENT = RFCIE;                                                        // Reset interrupt
  EMAC_int_message[MSG_INTERRUPT_EVENT] = ETHERNET_FLOW_CONTROL;         // Mark interrupt cause
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** transmission complete Ethernet interrupt ***********************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_F_TX_C_VECTOR
#endif
__interrupt void emac_f_tx_c_isr(void)
{
#ifdef LAN_REPORT_ACTIVITY
    static const unsigned char EMAC_int_message[ HEADER_LENGTH ] = { 0, 0, INTERRUPT_TASK_LAN_EXCEPTIONS, INTERRUPT_EVENT, EMAC_TX_INTERRUPT };
#endif
    IEVENT = TXCIE;                                                      // Reset interrupt
    eth_tx_control->ETH_queue.chars = 0;                                 // reset the transmission buffer for next use
    eth_tx_control->ETH_queue.put = eth_tx_control->ETH_queue.QUEbuffer;
#ifdef LAN_REPORT_ACTIVITY
    fnWrite(INTERNAL_ROUTE, (unsigned char*)EMAC_int_message, HEADER_LENGTH); // Inform the task of event
#endif
}

/**************************** valid receive buffer A Ethernet interrupt **********************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_B_A_C_VECTOR
#endif
__interrupt void emac_rx_b_a_c_isr(void)
{
  IMASK &= ~RXACIE;                                                      // mask further use until application has read buffer
  eth_rx_control->ETH_queue.chars = RXAEFP;                              // put the length of the received frame in the buffer
  RXAEFP = 0;                                                            // clear reception length
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_RXA_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

/**************************** valid receive buffer B Ethernet interrupt **********************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_B_B_C_VECTOR
#endif
__interrupt void emac_rx_b_b_c_isr(void)
{
  IMASK &= ~RXBCIE;                                                      // mask further use until application has read buffer
  ((ETHERNETQue*)(eth_rx_control->NextTTYbuffer))->ETH_queue.chars = RXBEFP; // put the length of the received frame in the buffer
  RXBEFP = 0;                                                            // clear reception length
  fnWrite( INTERNAL_ROUTE, (unsigned char*)EMAC_RXB_int_message, HEADER_LENGTH); // Inform the Ethernet task
}

 /**************************** PHY Ethernet interrupt ****************************************************/
#ifdef COMPILE_IAR
 #pragma vector=PHY_VECTOR
#endif
__interrupt void ephy_isr(void)
{
#ifdef INTERRUPT_TASK_PHY
  unsigned char int_phy_message[ HEADER_LENGTH ]; // = { 0, 0 , INTERRUPT_TASK_PHY, INTERRUPT_EVENT, UNKNOWN_INTERRUPT }; define standard interrupt event
#endif
  unsigned char int_message[ HEADER_LENGTH ]; // = { 0, 0 , cEthernetWake, INTERRUPT_EVENT, UNKNOWN_INTERRUPT };  define standard interrupt event
  unsigned short usInterrupt = 0;
  unsigned short usStatus = 0;

#ifdef INTERRUPT_TASK_PHY
  int_phy_message[0] = int_phy_message[1] = INTERNAL_ROUTE;
  int_phy_message[2] = INTERRUPT_TASK_PHY;
  int_phy_message[3] = INTERRUPT_EVENT;
  int_phy_message[4] = UNKNOWN_INTERRUPT;
#endif

  int_message[0] = int_message[1] = INTERNAL_ROUTE;
  int_message[2] = EthernetWake;
  int_message[3] = INTERRUPT_EVENT;
  int_message[4] = UNKNOWN_INTERRUPT;

  usInterrupt = fnMIIread(PHY_ADDRESS, PHY_REG_ICR);                     // read interrupt source

  if (usInterrupt & PHY_ANC) {                                           // auto neg. changed
    usStatus = fnMIIread(PHY_ADDRESS, PHY_REG_PSR);                      // get status
    if (PHY_R17_LNK & usStatus) {
#ifdef INTERRUPT_TASK_PHY
        int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
        fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // Inform the corresponding task
#else
        int_message[4] = LAN_LINK_DOWN;
#endif
    }
    else {
        int_message[MSG_INTERRUPT_EVENT] = AUTO_NEG_CHANGED;
        if (usStatus & PHY_R17_ANNC) {
            if (PHY_R17_ANCM & usStatus) {                               // auto. neg complete
              int_message[MSG_INTERRUPT_EVENT] = AUTO_NEG_COMMON_MODE;
            }
            else {
              int_message[MSG_INTERRUPT_EVENT] = AUTO_NEG_COMPLETE;
            }

            if (     & usStatus) {
                int_message[MSG_INTERRUPT_EVENT] |= AUTO_FULL_DUPLEX;
                NETCTL |= FDX;
                usStatus = fnMIIread(PHY_ADDRESS, PHY_REG_ANLPAR);       // check flow control pars
                if (usStatus & PHY_REG_ANLPAR) {
                    int_message[4] |= AUTO_PAUSE_RESOLVED;
                }
            }
        }
    }
  }
  else if (usInterrupt & PHY_ACKR) {
    int_message[MSG_INTERRUPT_EVENT] = ACK_BIT_REC;
  }
  else if (usInterrupt & PHY_PGR) {
    int_message[MSG_INTERRUPT_EVENT] = PAGE_REC;
  }
  else if (usInterrupt & PHY_LKC) {
    usStatus = fnMIIread(PHY_ADDRESS, PHY_REG_PSR);                      // get status
    if (usStatus & PHY_R17_LNK) {
 #ifdef INTERRUPT_TASK_PHY
        int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
 #else
        int_message[MSG_INTERRUPT_EVENT] = LAN_LINK_DOWN;
 #endif
    }
    else {
        if (PHY_R17_SPD & usStatus) {
 #ifdef INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
 #else
            int_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_100;
 #endif
        }
        else {
 #ifdef INTERRUPT_TASK_PHY
            int_phy_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
 #else
            int_message[MSG_INTERRUPT_EVENT] = LAN_LINK_UP_10;
 #endif
        }

    }
 #ifdef INTERRUPT_TASK_PHY
    fnWrite(INTERNAL_ROUTE, (unsigned char*)int_phy_message, HEADER_LENGTH); // Inform the corresponding task
 #endif
  }
  else if (usInterrupt & PHY_PDF) {
    int_message[MSG_INTERRUPT_EVENT] = PAR_DET_FAULT;
  }
  else if (usInterrupt & PHY_RMTF) {
    int_message[MSG_INTERRUPT_EVENT] = REMOTE_FAULT;
  }
  else if (usInterrupt & PHY_JABI) {
    int_message[MSG_INTERRUPT_EVENT] = JABBER_INT;
  }

  EPHYSR = EPHYIF;                                                       // clear interrupt

  if (EthernetWake) {
    if (int_message[MSG_INTERRUPT_EVENT] != UNKNOWN_INTERRUPT) {
        fnWrite( INTERNAL_ROUTE, int_message, HEADER_LENGTH);
    }
  }
}

/**************************** MII Ethernet interrupt *****************************************************/
#ifdef COMPILE_IAR
 #pragma vector=EMAC_MII_MTC_VECTOR
#endif
__interrupt void emac_mii_mtc_isr(void)
{
}                                                                        //  interrupt not used

/**************************** END of Ethernet Interrupts *************************************************/

// Function to read from the MII interface
//
static unsigned short fnMIIread(unsigned char _mpadr, unsigned char _mradr)
{
  while (MCMST & BUSY) {}                                                // MII busy -> we must wait
  IEVENT = MMCIF;                                                        // clear the MMI interrupt flag
  MPADR  = _mpadr;
  MRADR  = _mradr;
  MCMST  = (MII_READ | (BUS_CLOCK/5000000));

  while (!(IEVENT & MMCIF)) {};                                          // MII interrupt flag not yet set -> we must wait

#ifdef _WINDOWS
  if (PHY_REG_ICR == _mradr) {
      MRDATA = PHY_LKC;                                                  // signal link up
  }
  else if (PHY_REG_PSR == _mradr) {
      MRDATA = PHY_R17_SPD;                                              // state link up 100M
  }
#endif

  return MRDATA;                                                         // Return the data read
}

// Function to write to the MII interface
//
static void fnMIIwrite(unsigned char _mpadr, unsigned char _mradr, unsigned short _mwdata)
{
  while (MCMST & BUSY) {}                                                // MII busy -> we must wait

  MPADR  = _mpadr;
  MRADR  = _mradr;
  MWDATA = _mwdata;
  MCMST |= (MII_WRITE | (BUS_CLOCK/5000000));                            // write word

  fnMIIread(PHY_ADDRESS, PHY_REG_ICR);                                   // dummy read due to chip error (Errata MUCts01900)
}


// Ethernet configuration routine
//
extern void fnConfigEthernet(ETHTABLE *pars)
{
  unsigned short usMIIData;
  volatile unsigned short usDelay;

  // configure the MII interface for an external PHY
  if (pars->usMode & EXTERNAL_PHY_USED) {
    #ifdef HUTA_HPLAT_EXT_PHY
      RESET_PORT |= POWER_LINE_RESET;                                    // take power line modem out of reset
    #endif
  }
  else {
      EPHYCTL1 = PHY_ADDRESS;                                            // configure physical layer and its interrupts

      if (pars->usMode & (LAN_10M | LAN_100M)) {
        EPHYCTL0 = ANDIS;                                                // disable autonegotiate
      }
      else {
        EPHYCTL0 = 0;                                                    // enable autonegotiate
      }

      if (pars->usMode & LAN_LEDS) {
        EPHYCTL0 |= LEDEN;                                               // enable LED support in the Hardware
      }

      EPHYSR = EPHYIF;                                                   // clear interrupt
      EPHYCTL0 |= (EPHYEN | EPHYIEN);                                    // enable PHY with interrupt

      for (usDelay=64000; usDelay >0; usDelay--);                        // delay before continuing
  }
                                                                         // EMAC initialisation - note we start in half duplex mode
  SWRST = MACRST;                                                        // EMAC software reset

  for (usDelay=16; usDelay >0; usDelay--);                               // short wait

  MCMST = (BUS_CLOCK/5000000);                                           // set up MDC clock to 2.5MHz
                                                                         // setup FIFO memory configuration
  if (pars->usSizeRx > 128) {                                            // Here we have a choice between 4 possible configurations
      if (pars->usSizeRx > 256) {
          if (pars->usSizeRx > 512) {
              if (pars->usSizeRx > 1024) {
                BUFCFG = BUFFER_1K5;                                     //  reception buffers of 1k5 byte
                pars->usSizeRx = (unsigned short)(1024*1.5);
              }
              else {
                BUFCFG = BUFFER_1K;                                      //  reception buffers of 1k byte
                pars->usSizeRx = (1024);
              }
          }
          else {
            BUFCFG = BUFFER_512;                                         //  reception buffers of 512 byte
            pars->usSizeRx = (512);
          }
      }
      else {
        BUFCFG = BUFFER_256;                                             //  reception buffers of 256 byte
        pars->usSizeRx = (256);
      }
  }
  else {
    BUFCFG = BUFFER_128;                                                 // smallest reception buffers of 128 byte
    pars->usSizeRx = (128);
  }
#ifdef _WINDOWS
  ptrRxBuf[0] = ucSimRxABuf;                                             // Simulated buffers
  ptrRxBuf[1] = ucSimRxBBuf;
  ptrTxBuf =  ucSimTxBuf;
#else
  ptrRxBuf[0] = START_OF_RAM;                                            // At the target the Ethernet buffers are at the biginning of RAM
  ptrRxBuf[1] = ptrRxBuf[0] + pars->usSizeRx;
  ptrTxBuf = ptrRxBuf[1] + pars->usSizeRx;
#endif

  BUFCFG |= (pars->usSizeRx & 0x7ff);                                    // fix the maximum reception length

  uMemcpy(MACAD, pars->ucMAC, MAC_LENGTH);                               // program our MAC address to the device
                                                                         // setup the ethertype control register
  ETCTL = 0;                                                             // accept all frame types
  if (pars->ucEthTypes != ALL_ETYPES) {
    if (pars->ucEthTypes & EMWARE) ETCTL |= FEMW;
    if (pars->ucEthTypes & IPV6)   ETCTL |= FIPV6;
    if (pars->ucEthTypes & ARP)    ETCTL |= FARP;
    if (pars->ucEthTypes & IPV4)   ETCTL |= FIPV4;
    if (pars->ucEthTypes & IEEE)   ETCTL |= FIEEE;

    if (pars->usExtEthype) {
        ETYPE = pars->usExtEthype;
        ETCTL |= FPET;
    }
  }

  RXCTS = 0;                                                             // setup the reception mode
  if (pars->usMode & EXTERNAL_PHY_USED) {
      NETCTL |= EXTPHY;                                                  // enable MII to an external PHY
  }
  else {
      NETCTL = 0;
  }

  if (pars->usMode & (LAN_10M | LAN_100M)) {                             // when not auto-neg
      if ((pars->usMode & FULL_DUPLEX) && (pars->usMode & RX_FLOW_CONTROL)) {
          RXCTS = RFCE;                                                  // uses pause flow control
      }
      if (pars->usMode & FULL_DUPLEX) {                                  // define network control
          NETCTL |= FDX;
      }
  }

  if (pars->usMode & PROMISCUOUS) {
      RXCTS |= PROM;                                                     // no destination address checking
  }
  else {
      if (pars->usMode & NO_BROADCAST) {
          RXCTS |= BCREJ;                                                // reject broadcast frames
      }
  }

  if (pars->usMode & CON_MULTICAST) {
      RXCTS |= CONMC;                                                    // check multicast addresses
  }


  if (pars->usMode & LOOPBACK) {                                         // Optional Loopback for testing
      NETCTL |= MLB;
  }

  NETCTL |= EMACE;                                                       // after that the EMAC can be enabled (do not combine with previous writes since it must be performed alone)

  IMASK = (RXAOIE | RXBOIE | RXEIE | BREIE | RFCIE | TXCIE | RXACIE | RXBCIE | ECIE | LCIE);   //Enable EMAC interrupts:

  if (!(pars->usMode & EXTERNAL_PHY_USED)) {
                                                                         // Enable PHY interrupts in Reg 16 (PHY Interrupt Control Register) and set PHY Interrupt Control Register
      fnMIIwrite(PHY_ADDRESS, PHY_REG_ICR, (PHY_ACKIE | PHY_PRIE | PHY_LCIE | PHY_ANIE | PHY_PDFIE | PHY_RFIE | PHY_JABIE));

      usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_CR);                    // Read PHY Control Register 0

      if (!(pars->usMode & (LAN_10M | LAN_100M))) {                      // if auto-negotiation
        usMIIData |= PHY_R0_RAN;                                         // start autonegotiation
      }
      else {                                                             // manual configuration
        usMIIData = fnMIIread(PHY_ADDRESS, PHY_REG_CR);                  // Read PHY Control Register 0

        if (pars->usMode & FULL_DUPLEX) {
            usMIIData |= PHY_R0_DPLX;                                    // set full duplex
        }
        else {
            usMIIData &= ~PHY_R0_DPLX;                                   // set half duplex
        }

        if (pars->usMode & LAN_100M) {
            usMIIData |= PHY_R0_DR;                                      // set manual 100Mb
        }
        else {
            usMIIData &= ~PHY_R0_DR;                                     // set manual 10Mb
        }
      }
      fnMIIwrite(PHY_ADDRESS, PHY_REG_CR, usMIIData);
  }

  EthernetWake = pars->Task_to_wake;                                     // the driver can wake this task to inform of PHY events
  #ifdef _WINDOWS
  fnOpenDefaultHostAdapter();
  #endif
}

 #ifdef _WINDOWS
   #ifndef USE_IP
   const unsigned char cucBroadcast[MAC_LENGTH] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };  // used also for broadcase IP
   #endif
    // This routine is supplied to aid the simulator. It checks whether Ethereal frames should be accepted based
    // on the programmed MAC address and the Ethernet operating mode
    //
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen)
    {
        // This function checks to see whether we can receive the simulated frame and can block it if wanted
        if (!uMemcmp(&ucData[MAC_LENGTH], MACAD, MAC_LENGTH)) return 0;  // we never accept echos of our own transmissions

        if (RXCTS & PROM) return 1;                                      // in promiscuous mode we accept everything

        if (usLen < (2*MAC_LENGTH + 2)) return 0;

        if (uMemcmp(ucData, MACAD, MAC_LENGTH) && (uMemcmp(ucData, cucBroadcast, MAC_LENGTH))) return 0;

        return 1;                                                        // OK we can accept it
    }
 #endif

// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
extern int fnWaitTxFree(void)
{
    unsigned short usMaxWait = 500;
    while (TXCTS & TXACT) {
      if (!(--usMaxWait)) {
        return 1;                                                        // something wrong - quit
      }
    }                                                                    // wait until the transmitter is free
    return 0;
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
extern int fnGetQuantityRxBuf(void)
{
    return NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE;
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *fnGetRxBufferAdd(int iBufNr)
{
    return (unsigned char*)ptrRxBuf[iBufNr];
}

// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
extern unsigned char *fnGetTxBufferAdd(int iBufNr)
{
    return ptrTxBuf;
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
extern void fnFreeEthernetBuffer(int iBufNr)
{
  // the buffer has been completely used so free it for use by the ethernet controller
    if (iBufNr) {                                                        // buffer 1
      IEVENT = RXBCIE;                                                   // clear interrupt
      IMASK |= RXBCIE;                                                   // enable next interrupt
    }
    else {                                                               // buffer 0
      IEVENT = RXACIE;                                                   // clear interrupt
      IMASK |= RXACIE;                                                   // enable next interrupt
    }
}

// Data has been prepared, now send it over the ETHERNET interface
//
extern QUEUE_TRANSFER fnStartEthTx(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    while (DataLen < 60) {
      *ptr_put++ = 0x00;                                                 // pad with zeros if smaller than 60 [chip must send at least 60]
      DataLen++;
    }
    TXEFP = DataLen - 1;                                                 // offset of last data byte in FIFO
    TXCTS = TCMD_START;                                                  // start transmission

#ifdef _WINDOWS
    fnSimulateEthTx(TXEFP+1);
    emac_f_tx_c_isr();                                                   // simulate end of transmission - tx interrupt
#endif
    return (DataLen);
}
#endif






#ifdef SERIAL_INTERFACE
/* =================================================================== */
/*                       Serial Interface - UART                       */
/* =================================================================== */

// configure the SCI hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    unsigned char *ucReg = (unsigned char*)(SCI_BASE_ADD + (Channel * SCI_BLOCK_SIZE)); // set register pointer to corresponding block

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 300/2)/300);             // set 300
        break;
    case SERIAL_BAUD_600:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 600/2)/600);             // set 600
        break;
    case SERIAL_BAUD_1200:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 1200/2)/1200);           // set 1200
        break;
    case SERIAL_BAUD_2400:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 2400/2)/2400);           // set 2400
        break;
    case SERIAL_BAUD_4800:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 4800/2)/4800);           // set 4800
        break;
    case SERIAL_BAUD_9600:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 9600/2)/9600);           // set 9600
        break;
    case SERIAL_BAUD_14400:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 14400/2)/14400);         // set 14400
        break;
    default:                                                             // if not valid value set this one
    case SERIAL_BAUD_19200:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 19200/2)/19200);         // set 19200
        break;
    case SERIAL_BAUD_38400:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 38400/2)/38400);         // set 38400
        break;
    case SERIAL_BAUD_57600:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 57600/2)/57600);         // set 57600
        break;
    case SERIAL_BAUD_115200:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 115200/2)/115200);       // set 115200
        break;
    case SERIAL_BAUD_230400:
        *(unsigned short*)ucReg = ((OSCCLK/16 + 230400/2)/230400);       // set 230400
        break;
    }
    ucReg += 2;                                                          // set next register
    if (pars->usConfig & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {
        if (pars->usConfig & RS232_ODD_PARITY) {
          *ucReg = (DATA_FORMAT_MODE | PE | PT);                         // 8 bits - odd parity
        }
        else {
          *ucReg = (DATA_FORMAT_MODE | PE);                              // 8 bits - even parity
        }
    }
    else {
      *ucReg = 0;                                                        // 8 bits - no parity
    }
    *(++ucReg) = 0;                                                      // RX/TX initially off
#ifdef _WINDOWS
    ucReg -= 3;                                                          // set back to speed setting
    fnConfigSimSCI(Channel, OSCCLK/(*(unsigned short*)ucReg*16), pars);  // open a serial port on PC if desired
#endif
}

// Enable reception on the defined chanel
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    unsigned char *ucReg = (unsigned char*)(SCI_BASE_ADD + 3 + (Channel * SCI_BLOCK_SIZE)); // set register pointer to corresponding block
    *ucReg |= (RIE | RE);                                                // enable RX interrupt and Rx
}

// Disable reception on the defined chanel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    unsigned char *ucReg = (unsigned char*)(SCI_BASE_ADD + 3 + (Channel * SCI_BLOCK_SIZE)); // set register pointer to corresponding block
    *ucReg &= ~(RIE | RE);                                               // disable RX interrupt and Rx
}

// Enable transmission on the defined chanel
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    unsigned char *ucReg = (unsigned char*)(SCI_BASE_ADD + 3 + (Channel * SCI_BLOCK_SIZE)); // set register pointer to corresponding block
    *ucReg |= (TE);                                                      // enable TX

    // we send a dummy character on initialisation because the first otherwise never seems to come out
    *(ucReg + 4) = 0x55;                                                 // send the dummy byte
    *ucReg |= (TIE);                                                     // enable TX interrupt
}

// Disable transmission on the defined chanel
//
extern void fnTxOff(QUEUE_HANDLE Channel)
{
    unsigned char *ucReg = (unsigned char*)(SCI_BASE_ADD + 3 + (Channel * SCI_BLOCK_SIZE)); // set register pointer to corresponding block
    *ucReg &= ~(TE);                                                     // disable TX
}


/**************************** SCI0 interrupt ******************************************/
#ifdef COMPILE_IAR
 #pragma vector=SCI0_VECTOR
#endif
__interrupt void SCI0_Interrupt(void)
{
    unsigned char ucState;                                               // get cause of interrupt

    while ((ucState = (SCISR1_0 & SCICR2_0 & (TDRE | RDRF))) != 0) {     // while interrupts present {7}
        if (ucState & RDRF) {
            fnSciRxByte(SCIDRL_0, 0);                                    // receive data interrupt - read the byte
#ifdef _WINDOWS
            SCISR1_0 &= ~(RDRF);                                         // simulate reset of interrupt flag
#endif
        }

        if (ucState & TDRE) {
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
        }
    }
}


/**************************** SCI1 interrupt ******************************************/
#ifdef COMPILE_IAR
 #pragma vector=SCI1_VECTOR
#endif
__interrupt void SCI1_Interrupt(void)
{
    unsigned char ucState;                                               // get cause of interrupt

    while ((ucState = (SCISR1_1 & SCICR2_1 & (TDRE | RDRF))) != 0) {     // while interrupts present {7}
        if (ucState & RDRF) {
            fnSciRxByte(SCIDRL_1, 1);                                    // receive data interrupt - read the byte
#ifdef _WINDOWS
            SCISR1_1 &= ~(RDRF);                                         // simulate reset of interrupt flag
#endif
        }

        if (ucState & TDRE) {
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
        }
    }
}


// The TTY driver uses this call to disable the transmit interrupt of the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE channel)
{
    if (channel) {
        SCICR2_1 &= ~(TIE);                                              // disable TX interrupt on channel 1
        return;
    }
    SCICR2_0 &= ~(TIE);                                                  // disable TX interrupt on channel 0
}

// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    volatile unsigned char ucDummy;

    if (channel) {                                                       // channel 1
        if (SCISR1_1 & TDRE) {                                           // tx not busy
          ucDummy = SCISR1_1;                                            // read interrupt flags to ensure empty flag is reset when the character is written
          SCIDRL_1 = ucTxByte;                                           // send
          SCICR2_1 |= (TIE);                                             // enable TX interrupt
  #ifdef _WINDOWS
          SCISR1_1 &= ~(TDRE);
          iInts |= CHANNEL_1_SERIAL_INT;                                 // simulate interrupt
  #endif
          return 0;                                                      // OK, we were not busy
        }
        return 1;                                                        // not sent because we're busy
    }

    // channel 0
    if (SCISR1_0 & TDRE) {                                               // tx not busy
      ucDummy = SCISR1_0;                                                // read interrupt flags to ensure empty flag is reset when the character is written
      SCIDRL_0 = ucTxByte;                                               // send
      SCICR2_0 |= (TIE);                                                 // enable TX interrupt
      return 0;                                                          // OK, we were not busy
    }
    return 1;                                                            // not sent because we're busy
}

#endif



#ifdef FLASH_FILE_SYSTEM
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */

static void fnConfigFlash(void)
{
    FSTAT = (ACCERR | PVIOL);                                            // clear error flags
    if (FCLKDIV & FDIVLD) return;                                        // already configured

    // before erasing or writing FLASH, the clock must be configured correctly
    // we want to be in the 150kHz..200kHz range but as fast as possible

    // This works for oscillator clocks greater than 12.8Mhz
    FCLKDIV = PRDIV8 + ((unsigned char)(OSCCLK/200000/8) + 1);           // warning - If in doubt, best check that it is in range before using - errors can destroy FLASH!!!
#ifdef _WINDOWS
    FCLKDIV |= FDIVLD;                                                   // set the loaded bit in the divide register
    FSTAT |= CBEIF;                                                      // no command queued
#endif
}

#ifdef _WINDOWS
  #define PROG_SIZE  100
#else
  #define PROG_SIZE  40                                                  // optimised from experience
#endif
static void fnFlashRoutine(void)
{
#ifdef _WINDOWS
    FSTAT = (CBEIF | CCIF);
#else
    FSTAT = CBEIF;
#endif
    while (!(FSTAT & CBEIF)) {}                                          // wait until buffer free
    while (!(FSTAT & CCIF)) {}                                           // wait until execution complete
}

 #ifdef _CODE_WARRIOR
  #pragma CODE_SEG FILE_ROUTINE                                          // force CODEWARRIOR compiler to put this routine at the start of code
 #endif
 #ifdef COMPILE_IAR
  #pragma location="FILE_ROUTINE"                                        // force IAR compiler to put this routine at the start of code
 #endif

static int fnFlashNow(unsigned char ucCommand, unsigned short *ptrWord, unsigned char ucPage, unsigned short usWord)
{
    unsigned char ucProgSpace[PROG_SIZE];
    unsigned char *ptrProg = (unsigned char*)fnFlashRoutine;
    void (*fnRAM_code)(void) = (void(*)(void))ucProgSpace;
    int i = 0;
    unsigned char ucOriginalPage = PPAGE;                                // backup present page

    while (i < PROG_SIZE) {                                              // copy program to stack
      ucProgSpace[i++] = *ptrProg++;
    }

    uDisable_Interrupt();                                                // protect this region from interrupts

    PPAGE = ucPage;                                                      // we set the desired page

#ifdef _WINDOWS                                                          // command sequence - set the word to the address (in the sector)
    if (ucCommand != CMD_SECTOR_ERASE) {
      *ptrWord = usWord;                                                 // value not important for deletes
    }
#else
    *ptrWord = usWord;                                                   // value not important for deletes
#endif
    FCMD = ucCommand;                                                    // The command

    fnRAM_code();                                                        // execute from RAM [stack]

    PPAGE = ucOriginalPage;                                              // return original page

    uEnable_Interrupt();                                                 // safe to accept interrupts again

    return (FSTAT & (ACCERR | PVIOL));                                   // if there was an error this will be non-zero
}
#endif                                                                   // #endif FLASH_FILE_SYSTEM

#if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM                 // {5}{9}

/* =================================================================== */
/*                           AT45DB041 driver                          */
/* =================================================================== */

#define READ_STATUS_REGISTER     0xd7
  #define STATUS_READY           0x80
  #define STATUS_MISMATCH        0x40
  #define STATUS_1MEG            0x0c                                    // expected in 1Meg bit device
  #define STATUS_2MEG            0x14                                    // expected in 2Meg bit device
  #define STATUS_4MEG            0x1c                                    // expected in 4Meg bit device
  #define STATUS_8MEG            0x24                                    // expected in 8Meg bit device
  #define STATUS_PROTECTED_SECT  0x02                                    // sector is protected (D-device)
  #define STATUS_PAGE_SIZE       0x01                                    // 256 byte pages size (rather than 264)
#define WRITE_BUFFER_1           0x84
#define WRITE_BUFFER_2           0x87
#define ERASE_PROG_FROM_BUFFER_1 0x83
#define ERASE_PROG_FROM_BUFFER_2 0x86
#define PROG_FROM_BUFFER_1       0x88
#define PROG_FROM_BUFFER_2       0x89
#define PAGE_ERASE               0x81
#define BLOCK_ERASE              0x50
#define PROG_THROUGH_BUFFER_1    0x82                                    // combines WRITE_BUFFER and ERASE_PROG_FROM_BUFFER
#define PROG_THROUGH_BUFFER_2    0x85
#define MAIN_TO_BUFFER_1         0x53                                    // copy page of main memory to buffer
#define MAIN_TO_BUFFER_2         0x55
#define COMPARE_WITH_BUFFER_1    0x60                                    // compare page of main memory with buffer
#define COMPARE_WITH_BUFFER_2    0x61
#define AUTO_PAGE_REWRITE_1      0x58                                    // combines MAIN_TO_BUFFER and ERASE_PROG_FROM_BUFFER and can be used to refresh pages (it is recommeded to refresh each page in a sector at least every 10'000 page erase/program operations of the sector)
#define AUTO_PAGE_REWRITE_2      0x59
#define CONTINUOUS_ARRAY_READ    0xe8


// Command supported only by D-device
#define SECTOR_ERASE             0x7c
#define READ_ARRAY_LOW_FREQ      0x03
#define READ_MANUFACTURER_ID     0x9f
// Chip erase is not spported due to bugs in some chips - sector erase is used instead

// Routine to request local SPI FLASH type
//
extern unsigned char fnSPI_Flash_available(void)
{
    return ucSPI_FLASH_Type[0];
}

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
extern unsigned char fnSPI_FlashExt_available(int iExtension)
{
    if (iExtension > SPI_FLASH_DEVICE_COUNT) {
        return 0;
    }
    return ucSPI_FLASH_Type[iExtension];
}
    #endif

// SPI FLASH hardware interface
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #define CMD_WRITE 0x01
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    unsigned char ucChipSelectLine = ucChipSelect[iChipSelect];
    #define ucChipSelectLineSim 0xff
    #else
    #define ucChipSelectLine    CS0_LINE
    #define ucChipSelectLineSim CS0_LINE
    #define iChipSelect 0
    #endif
    unsigned char ucTxCount = 0;
    volatile unsigned long ulDummy;
    unsigned char ucCommandBuffer[3];

    if ((SPI_FLASH_Danger[iChipSelect] != 0) && (ucCommand != READ_STATUS_REGISTER)) {// check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1);
        } while (!(ucStatus & STATUS_READY));
    }
    SPI_FLASH_Danger[iChipSelect] = 0;                                   // device is presently not busy

    SPI_FLASH_PORT &= ~ucChipSelectLine;                                 // assert SS low before starting

    ulDummy = SPIDR;                                                     // reset receive data flag with dummy read - the rx data is not interesting here
    SPIDR = ucCommand;                                                   // send command

#ifdef _WINDOWS
    fnSimSPI_Flash(AT45DBXXX_WRITE, (unsigned char)SPIDR, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI FLASH device
    SPISR |= (SPTEF | SPIF);                                             // simulate tx and rx interrupt flags being set
#endif
    while (!(SPISR & SPTEF)) {}                                          // wait until tx byte has been sent and rx byte has been completely received
    ulDummy = SPIDR;                                                     // reset receive data flag with dummy read - the rx data is not interesting here

    switch (ucCommand) {
    case MAIN_TO_BUFFER_1:                                               // command chip to copy a page from main memory to the buffer (400us transfer time)
  //case MAIN_TO_BUFFER_2:
    case PROG_FROM_BUFFER_1:                                             // command programming buffer to FLASH (no erase)
  //case PROG_FROM_BUFFER_2:
    case ERASE_PROG_FROM_BUFFER_1:                                       // erase before programming
  //case ERASE_PROG_FROM_BUFFER_2:
    case PAGE_ERASE:
    case BLOCK_ERASE:
#if SPI_FLASH_PAGE_LENGTH >= 1024
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 5);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 3);
#elif SPI_FLASH_PAGE_LENGTH >= 512
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 6);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 2);
#else
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 7);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 1);
#endif
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case WRITE_BUFFER_1:                                                 // write data to the buffer
  //case WRITE_BUFFER_2:
        ucCommandBuffer[0] = 0;
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)ulPageNumberOffset;
        break;


    case CONTINUOUS_ARRAY_READ:                                          // this is a legacy command for compatibility between B and D-devices
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 16);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)(ulPageNumberOffset);
        break;

    case READ_MANUFACTURER_ID:                                           // this only works on D-device
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        while (DataLength--) {
            SPIDR = 0xff;                                                // empty transmission byte
            while (!(SPISR & SPTEF)) {};                                 // wait until dummy tx byte has been sent and rx byte has been completely received
#ifdef _WINDOWS
            SPIDR = fnSimSPI_Flash(AT45DBXXX_READ, 0, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI FLASH device
            SPISR |= (SPTEF | SPIF);                                     // simulate tx and rx interrupt flags being set
#endif
            *ucData++ = (unsigned char)SPIDR;                            // read received byte and clear rx interrupt
        }
        SPI_FLASH_PORT |= ucChipSelectLine;                              // deassert SS always on completion
#ifdef _WINDOWS
        fnSimSPI_Flash(AT45DBXXX_CHECK_SS, 0, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI EEPROM device
#endif
        return;

        // Note - no default here. This must never be called with non-supported command since it will hang.
    }

    while (ucTxCount < sizeof(ucCommandBuffer)) {                        // complete the command sequence
        SPIDR = ucCommandBuffer[ucTxCount++];                            // send data
#ifdef _WINDOWS
        fnSimSPI_Flash(AT45DBXXX_WRITE, (unsigned char)SPIDR, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI FLASH device
        SPISR |= (SPTEF | SPIF);                                         // simulate tx and rx interrupt flags being set
#endif
        while (!(SPISR & SPTEF)) {};                                     // wait until tx byte has been sent and rx byte has been completely received
    }

    if (CONTINUOUS_ARRAY_READ == ucCommand) {
        int iDummyReads = 4;
        while (iDummyReads-- != 0) {                                      // this command requires 4 dummy reads before data is returned
            SPIDR = 0xff;
#ifdef _WINDOWS
            SPISR |= (SPTEF | SPIF);                                     // simulate tx and rx interrupt flags being set
#endif
            while (!(SPISR & SPTEF)) {};                                 // wait until tx byte has been sent and rx byte has been completely received
        }
        while (DataLength-- != 0) {                                      // while data bytes to be read
            SPIDR = 0xff;
#ifdef _WINDOWS
            SPIDR = fnSimSPI_Flash(AT45DBXXX_READ, 0, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI FLASH device
            SPISR |= (SPTEF | SPIF);                                     // simulate tx and rx interrupt flags being set
#endif
            while (!(SPISR & SPTEF)) {};                                 // wait until tx byte has been sent and rx byte has been completely received
            *ucData++ = (unsigned char)SPIDR;
        }
    }
    else {
        while (DataLength-- != 0) {                                      // while data bytes to be written
            SPIDR = *ucData++;                                           // send data
#ifdef _WINDOWS
            fnSimSPI_Flash(AT45DBXXX_WRITE, (unsigned char)SPIDR, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI FLASH device
            SPISR |= (SPTEF | SPIF);                                     // simulate tx and rx interrupt flags being set
#endif
            while (!(SPISR & SPTEF)) {};                                 // wait until tx byte has been sent and rx byte has been completely received
        }
    }

    SPI_FLASH_PORT |= ucChipSelectLine;                                  // deassert SS always on completion
#ifdef _WINDOWS
    fnSimSPI_Flash(AT45DBXXX_CHECK_SS, 0, (SPI_FLASH_PORT & ucChipSelectLineSim)); // simulate the SPI EEPROM device
#endif
}

// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckAT45dbxxx(int iChipSelect)
#else
static unsigned char fnCheckAT45dbxxx(void)
#endif
{
    volatile unsigned char ucID[4];

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ATMEL) {                              // ATMEL D-part recognised
        switch (ucID[1]) {
        case DEVICE_ID_1_DATA_FLASH_4M:
            return AT45DB041D;
        case DEVICE_ID_1_DATA_FLASH_8M:
            return AT45DB081D;
        case DEVICE_ID_1_DATA_FLASH_16M:
            return AT45DB161D;
        default:                                                         // possibly a larger part but we don't accept it
            break;
        }
    }

    fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);        // it is possibly a B-device so check the status register
    switch (ucID[0] & 0x3c) {                                            // check part size field
    case STATUS_1MEG:
        return AT45DB011B;
    case STATUS_2MEG:
        return AT45DB021B;
    case STATUS_4MEG:
        return AT45DB041B;
    case STATUS_8MEG:
        return AT45DB081B;
    default:
        break;
    }

    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}

#ifdef SPI_FLASH_MULTIPLE_CHIPS
// Calculate the device to access from the absolute address. It is assumed that all devices are operational.
//
static int fnGetSPI_Device(MEMORY_RANGE_POINTER *ptrFlash)               // define the device to be accessed
{
    if (*ptrFlash < (MEMORY_RANGE_POINTER)(SPI_DATA_FLASH_0_SIZE)) {
        return 0;
    }
    #ifdef CS1_LINE
    if (*ptrFlash < (MEMORY_RANGE_POINTER)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;
    }
        #ifdef CS2_LINE
    if (*ptrFlash < (MEMORY_RANGE_POINTER)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
        return 2;
    }
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 3;
        #else
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 2;
        #endif
    #else
    *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
    return 1;
    #endif
}
#endif

// Return the page number and optionally the address offset in the page
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
    static unsigned short fnGetSPI_FLASH_location(MEMORY_RANGE_POINTER ptrSector, unsigned short *usPageOffset, int *iChipSelect)
#else
    static unsigned short fnGetSPI_FLASH_location(MEMORY_RANGE_POINTER ptrSector, unsigned short *usPageOffset)
#endif
{
    unsigned short usPageNumber;
    ptrSector -= (SIZE_OF_FLASH);                                        // location relative to the start of the SPI FLASH chip address

    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    #if SPI_FLASH_PAGE_LENGTH != 256                                     // B-device or D-device set to 256 page mode
    usPageNumber = 0;
    while (ptrSector >= (MEMORY_RANGE_POINTER)(SPI_FLASH_PAGE_LENGTH*100)) { // large hops to get close to the page if at high FLASH address
        usPageNumber += 100;
        ptrSector -= (SPI_FLASH_PAGE_LENGTH*100);
    }
    while (ptrSector >= (MEMORY_RANGE_POINTER)SPI_FLASH_PAGE_LENGTH) {   // page sizes to reach actual page
        usPageNumber++;
        ptrSector -= SPI_FLASH_PAGE_LENGTH;
    }
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector);                       // remainder is offset in the page
    }
    #else
    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    #endif
    return usPageNumber;
}
#elif defined SPI_FILE_SYSTEM                                            // uses external file system in M95XXX EEPROM
 #ifdef SPI_INTERFACE
  extern QUEUE_HANDLE SPI_handle;
 #endif
// EEPROM commands
#define M95XXX_WRITE_ENABLE   0x06                                       // send this prior to write or write status register command
#define M95XXX_WRITE_DISABLE  0x04
#define M95XXX_READ_STAT_REG  0x05
#define M95XXX_WRITE_STAT_REG 0x01
#define M95XXX_READ_MEMORY    0x03
#define M95XXX_WRITE_MEMORY   0x02

static unsigned char ucEEPROMDanger = 0;


int fnWriteBytesEEPROM(unsigned char *ucDestination, unsigned char *ucData, unsigned short usLength)
{
    unsigned char usByteWrite[4];                                        // our file sytem is in external EEPROM via SPI bus.
    usByteWrite[0] = M95XXX_WRITE_MEMORY;
    usByteWrite[1] = (unsigned char)((unsigned short)(ucDestination)>> 8);
    usByteWrite[2] = (unsigned char)((unsigned short)ucDestination);
    usByteWrite[3] = PREPARE_PAGE_WRITE;
#ifdef SPI_INTERFACE
    fnWrite(SPI_handle, usByteWrite, sizeof(usByteWrite));               // set the byte write address
    fnWrite(SPI_handle, ucData, usLength);                               // write the data
#else
    fnSendSPIMessage(usByteWrite, sizeof(usByteWrite));                  // send immediately
    fnSendSPIMessage(ucData, usLength);                                  // write the data immediately
#endif
    ucEEPROMDanger = 1;                                                  // we should avoid reading until we know that the write has terminated
    return 0;
}

static void fnWaitWriteComplete(void)
{
    #define WIP 0x01
    static const unsigned char ucRead[] = {M95XXX_READ_STAT_REG, PREPARE_READ}; // read from status register
    unsigned char ucStatus;

    if (!ucEEPROMDanger) return;                                         // no point in checking since we are sure no write is in progress

    do {
 #ifdef SPI_INTERFACE
        fnWrite(SPI_handle, (unsigned char *)ucRead, sizeof(ucRead));
        fnRead(SPI_handle, &ucStatus, 1);                                // read the present status
 #else
        fnSendSPIMessage((unsigned char *)ucRead, sizeof(ucRead));
        fnGetSPI_bytes(&ucStatus, 1);
 #endif
    } while (ucStatus & WIP);                                            // wait until an in progress write is complete
    ucEEPROMDanger = 0;                                                  // mark eeprom not writimg so no danger reading data from it
 #ifdef _WINDOWS
    fnEEPROM_safe();
 #endif
}
#endif                                                                   // #endif SPI_FILE_SYSTEM


#ifdef ACTIVE_FILE_SYSTEM
extern int fnEraseFlashSector(MEMORY_RANGE_POINTER ptrSector, MAX_FILE_LENGTH Length) // {6}
{
    do {
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {9}
        static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
        unsigned short ucDeleted = 0;
        unsigned char *ptrEEPROM = (unsigned char *)ptrSector;
        unsigned short usTemp = (unsigned short)ptrEEPROM;

        usTemp &= ~(EEPROM_PAGE_SIZE-1);                                 // we want to erase a block (program them to 0xff)
        ptrEEPROM = (unsigned char *)usTemp;
        while (ucDeleted < FLASH_GRANULARITY) {                          // {2}
            fnWaitWriteComplete();                                       // wait until free to write
        #ifdef SPI_INTERFACE
            fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrEEPROM, ucDel, EEPROM_PAGE_SIZE);      // delete a page
        #else
            fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrEEPROM, 0, EEPROM_PAGE_SIZE);          // delete a page
        #endif
            ucDeleted += EEPROM_PAGE_SIZE;
            ptrEEPROM += EEPROM_PAGE_SIZE;
        }
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    #else
        #if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM         // {5}{9}
        if (ptrSector >= (MEMORY_RANGE_POINTER)(SIZE_OF_FLASH)) {        // we are working from external SPI FLASH memory
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            unsigned short usPageNumber, usPageOffset, usLength;
            unsigned char ucCommand;
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset EXTENDED_CS);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                usLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                usLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);   // delete appropriate page/block
            if (Length <= usLength) {
                break;                                                   // delete complete
            }
            ptrSector += usLength;
            Length -= usLength;
            continue;
        }
        #endif
        #ifdef _WINDOWS
        (unsigned long)ptrSector &= ~(FILE_GRANULARITY-1);
      //ptrSector = (MEMORY_RANGE_POINTER)(fnGetFlashAdd((unsigned char *)ptrSector));
        #endif

        fnConfigFlash();
        #ifdef _WINDOWS
        FSTAT |= CBEIF;                                                      // no command queued
        #endif
        if (!(FSTAT & CBEIF)) return 1;                                      // the command buffers are full - we can not proceed

        #ifdef _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd((unsigned char*)ptrSector));       // simulate deletion
        #endif
        if (fnFlashNow(CMD_SECTOR_ERASE, (unsigned short*)((unsigned short)ptrSector & ~0x1), 0x3d, (unsigned short)0) != 0) { // ensure even address
            return 1;                                                        // delete error
        }
        ptrSector += FLASH_GRANULARITY;
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    #endif
    } while (Length);
    return 0;
}


#ifdef FLASH_FILE_SYSTEM
// The address should be even or it may fail
//
static int fnWriteWordFlash(unsigned short *ptrAdd, unsigned short usValue)
{
    #ifdef _WINDOWS
    ptrAdd = (unsigned short *)fnGetFlashAdd((unsigned char *)ptrAdd);
    #endif
    fnConfigFlash();
    #ifdef _WINDOWS
    FSTAT |= CBEIF;                                                      // no command queued
    #endif
    if (!(FSTAT & CBEIF)) return 1;                                      // the command buffers are full - we can not program yet

    #ifdef _WINDOWS
    if ((*ptrAdd != 0xffff) && (usValue != *ptrAdd)) {
        ptrAdd = 0;                                                      // this will fail so signal it when debugging
        *ptrAdd = 0;
    }
    #endif

    return (fnFlashNow(CMD_PROGRAMM_WORD, ptrAdd, 0x3d, usValue));
}
#endif



extern int fnWriteBytesFlash(MEMORY_RANGE_POINTER ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // simulate the same using SPI EEPROM {9}
    static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
    unsigned short usPageLength;
    while (Length) {
        usPageLength = ((unsigned short)ucDestination) & (EEPROM_PAGE_SIZE-1); // calculate the page length based on EEPROM page boundaries
        usPageLength = EEPROM_PAGE_SIZE - usPageLength;
        if (Length < usPageLength) {
            usPageLength = Length;
        }
        fnWaitWriteComplete();
  #ifdef SPI_INTERFACE
        fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);           // write a page
  #else
        fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);           // write a page
  #endif
        Length -= usPageLength;
        ucData += usPageLength;
        ucDestination += usPageLength;
    }

    return 0;
#else
    unsigned short usValue = 0;

    #if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM             // {5}{9}
    if (ucDestination >= (MEMORY_RANGE_POINTER)(SIZE_OF_FLASH)) {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        unsigned short usDataLength;
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
            if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) { // are we writing a partial page?
                fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
            }
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        }
        return 0;
    }
    #endif


    // The NE64 writes always in words so we must ensure that the data is correctly aligned and filled (no accumulative programming!!)
    while (Length--) {
  #ifdef _WINDOWS                                                        // Little-endian
      usValue = *ucData++;
      if (Length) {
        Length--;
        usValue |= ((*ucData++) << 8);
      }
      else {
        usValue |= 0xff00;
      }
  #else
      usValue = *ucData++;                                               // Big-endian
      usValue <<= 8;
      if (Length) {
        Length--;
        usValue |= *ucData++;
      }
      else {
        usValue |= 0xff;
      }
  #endif

      if (fnWriteWordFlash((unsigned short *)(CAST_POINTER_ARITHMETIC)ucDestination, usValue)) return 1;

      ucDestination += 2;
    }

    return 0;
#endif
}




#endif




/************************************************************************************************************/

#ifdef SPI_INTERFACE

 #ifdef SPI_FILE_SYSTEM
// SPI hardware configuration for SPI and file system use
//
void fnConfigSPI(SPITABLE *pars)
{
    #define SPI_SSEL PTS7
    switch (pars->ucSpeed) {
    case MAX_SPI:
	    SPIBR = 0;                                                       // set maximum possible SPI speed
	    break;

    case SPI_4MEG:
	    SPIBR = (SPPR1);
	    break;

    case SPI_2MEG:
	    SPIBR = (SPPR2 | SPPR0);
	    break;

    case SPI_1MEG:
	    SPIBR = (SPPR2 | SPPR0 | SPRR0);
	    break;

    case SPI_SLAVE:
	    SPICR1 = (SPIE | SPE);                                           // enable in slave mode - enable interrupt of rx char

        while (!(SPISR & SPTEF)) {}                                      // wait until Tx complete before setting next byte
	    SPIDR = 0x00;                                                    // prime a zero in TX which indicates not yet synchronous
	    return;
    }
    SPICR1 = (MSTR | SPE /*| SSOE*/);                                    // master enabled without auto SSEL support
    SPICR2 = 0; //MODFEN;

    PTS |= SPI_SSEL;                                                     // ensure high
    DDRS |= SPI_SSEL;                                                    // drive SPI SS output high
}
 #else                                                                   // else not SPI_FILE_SYSTEM
// SPI HW configuration for general use
//
void fnConfigSPI(SPITABLE *pars)
{
}
 #endif                                                                  // #endif not SPI_FILE_SYSTEM

extern SPIQue *spi_rx_control;

#ifdef COMPILE_IAR
 #pragma vector=SPI_VECTOR
#endif
 __interrupt void SPI_Interrupt(void)
{
  unsigned short usMsgLen;
  volatile unsigned char dummy;

   usMsgLen = SPISR;                                                     // dummy read of status register on entry - we are here because of Rx interrupt

   dummy = SPIDR;

   fnSPIRxByte(dummy, 0);                                                // receive data interrupt - read the byte

   fnSPITxByte(0);                                                       // prepare next byte for transmission

}

extern int fnTxSPIByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    return 0;
}

#endif

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM)                  // {5}
static void fnConfigSPIFileSystem(unsigned char ucSpeed)
{
    switch (ucSpeed) {
    case MAX_SPI:
	    SPIBR = 0;                                                       // set maximum possible SPI speed
	    break;

    case SPI_4MEG:
	    SPIBR = (SPPR1);
	    break;

    case SPI_2MEG:
	    SPIBR = (SPPR2 | SPPR0);
	    break;

    case SPI_1MEG:
	    SPIBR = (SPPR2 | SPPR0 | SPRR0);
	    break;

    case SPI_SLAVE:
	    SPICR1 = (SPIE | SPE);                                           // enable in slave mode - enable interrupt of rx char

        while (!(SPISR & SPTEF)) {}                                      // wait until Tx complete before setting next byte
	    SPIDR = 0x00;                                                    // prime a zero in TX which indicates not yet synchronous
	    return;
    }
    SPICR1 = (MSTR | SPE /*| SSOE*/);                                    // master enabled without auto SSEL support
    SPICR2 = 0; //MODFEN;

    #if defined (SPI_FILE_SYSTEM) && defined (FLASH_FILE_SYSTEM)         // {5}
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
            #if defined CS3_LINE
    SPI_FLASH_PORT |= (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE);
    SPI_FLASH_PORT_DDR |= (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE);   // drive CS lines high
            #elif defined CS2_LINE
    SPI_FLASH_PORT |= (CS0_LINE | CS1_LINE | CS2_LINE);
    SPI_FLASH_PORT_DDR |= (CS0_LINE | CS1_LINE | CS2_LINE);              // drive CS lines high
            #else
    SPI_FLASH_PORT |= (CS0_LINE | CS1_LINE);
    SPI_FLASH_PORT_DDR |= (CS0_LINE | CS1_LINE);                         // drive CS lines high
            #endif
        #else
    SPI_FLASH_PORT |= CS0_LINE;                                          // ensure high
    SPI_FLASH_PORT_DDR |= CS0_LINE;                                      // drive SPI SS output high
        #endif
    #else
    SPI_FLASH_PORT |= CS0_LINE;                                          // ensure high
    SPI_FLASH_PORT_DDR |= CS0_LINE;                                      // drive SPI SS output high
    #endif
}
#endif

#ifdef IIC_INTERFACE
// configure the IIC hardware
//
void fnConfigIIC(IICTABLE *pars)
{
    if (pars->usSpeed != IIC_SLAVE_SPEED) {
        IBFD = 0x79;                                                     // set IIC bus frequency to 100kHz
        IBCR = (IBEN);                                                   // enable IIC bus (not sure whether it must be performed in 2 steps but to be sure)
        IBCR = (IBEN | MS_SL);                                           // enable IIC bus in master mode
    }
    else {
        IBCR = (IBEN);                                                   // enable IIC bus as slave
    }
#ifdef _WINDOWS
    fnConfigSimIIC(pars->Channel, (pars->usSpeed * 1000));               // {8}
#endif
}


extern IICQue *IIC_rx_control[NUMBER_IIC];                               // {1}
extern IICQue *IIC_tx_control[NUMBER_IIC];

// send a first byte to IIC bus
//
extern void fnTxIIC(IICQue *ptIICQue, QUEUE_HANDLE Channel)              // {14}
{
    unsigned char ucAddress;

    ptIICQue->ucPresentLen = *ptIICQue->IIC_queue.get++;                 // get present length
    if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
        ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;         // handle circular buffer
    }

    if (ptIICQue->ucState & TX_ACTIVE) {                                 // restart since we are hanging a second telegram on to previous one
      IBCR |= RSTA;
    }
    else {
      IBCR |= (MS_SL | TX_RX | IBIE);                                    // generate start condition / enable IIC interrupt
    }
    ucAddress = *ptIICQue->IIC_queue.get++;
    IBDR = ucAddress;                                                    // send the slave address
    if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
        ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;         // handle circular buffer
    }

    if (ucAddress & 0x01) {                                              // reading
        IIC_tx_control[Channel]->ucState |= (RX_ACTIVE | TX_ACTIVE);
        ptIICQue->IIC_queue.chars -= 3;
        IIC_rx_control[Channel]->wake_task = *ptIICQue->IIC_queue.get++; // enter task to be woken when reception has completed
        if (ptIICQue->IIC_queue.get >= ptIICQue->IIC_queue.buffer_end) {
            ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.QUEbuffer;     // handle circular buffer
        }
    }
    else {
        IIC_tx_control[Channel]->ucState |= (TX_ACTIVE);                 // writing
        ptIICQue->IIC_queue.chars -= (ptIICQue->ucPresentLen+1);
    }

 #ifdef _WINDOWS
    IBSR |= IBIF;                                                        // simulate the interrupt directly
    fnSimIIC_devices(IIC_ADDRESS, IBDR);
    iInts |= IIC_INT0;
 #endif
}

#ifdef COMPILE_IAR
 #pragma vector=IIC_VECTOR
 __interrupt void IIC_Interrupt(void)
#else
 void IIC_Interrupt(void)
#endif
{
    IBSR |= IBIF;                                                        // clear the interrupt flag (by writing a '1')

    if (IIC_tx_control[0]->ucState & RX_ACTIVE) {
        if (IIC_tx_control[0]->ucPresentLen == 1) {
            IBCR |= TXAK;                                                // we don't acknowledge last byte
        }
        else if (!IIC_tx_control[0]->ucPresentLen) {                     // we have completed the read
            IBCR &= ~MS_SL;                                              // send end condition
            IBCR &= ~(IBIE);                                             // disable interrupts
            IIC_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            IIC_rx_control[0]->msgs++;                                   // complete message ready
            if ( IIC_rx_control[0]->wake_task ) {                       // wake up the receiver task if desired
               uTaskerStateChange( IIC_rx_control[0]->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        else {
            IBCR &= ~TXAK;                                               // ensure we acknowledge multibyte reads
        }

        if (IBCR & TX_RX) {                                              // have we just sent the slave address?
            unsigned char ucRx;

            IBCR &= ~TX_RX;                                              // go to master receive mode
            ucRx = IBDR;                                                 // dummy read
        }
        else {
            *IIC_rx_control[0]->IIC_queue.put++ = IBDR;                  // read the byte
            IIC_rx_control[0]->IIC_queue.chars++;                        // and put it into the rx buffer
            if (IIC_rx_control[0]->IIC_queue.put >= IIC_rx_control[0]->IIC_queue.buffer_end) {
                IIC_rx_control[0]->IIC_queue.put = IIC_rx_control[0]->IIC_queue.QUEbuffer;
            }
        }

        if (IIC_tx_control[0]->ucPresentLen) {
          IIC_tx_control[0]->ucPresentLen--;
 #ifdef _WINDOWS
          IBDR = fnSimIIC_devices(IIC_RX_DATA, IBDR);                    // simulate the interrupt directly
          IBSR |= IBIF;
          iInts |= IIC_INT0;
          if (!IIC_tx_control[0]->ucPresentLen) {
              IBDR = fnSimIIC_devices(IIC_RX_COMPLETE, 0);               // inform of termination
          }
 #endif
        }
        return;
    }

    if (IIC_tx_control[0]->ucPresentLen--) {                             // send next byte if available
        IBDR = *IIC_tx_control[0]->IIC_queue.get++;
        if (IIC_tx_control[0]->IIC_queue.get >= IIC_tx_control[0]->IIC_queue.buffer_end) {
            IIC_tx_control[0]->IIC_queue.get = IIC_tx_control[0]->IIC_queue.QUEbuffer; // handle the ring buffer
        }
 #ifdef _WINDOWS
        IBSR |= IBIF;                                                    // simulate the interrupt directly
        fnSimIIC_devices(IIC_TX_DATA, IBDR);
        iInts |= IIC_INT0;
        if (!IIC_tx_control[0]->ucPresentLen) {
            fnSimIIC_devices(IIC_TX_COMPLETE, 0);
        }
 #endif
    }
    else {
        if (!(IIC_tx_control[0]->IIC_queue.chars)) {                     // transmission complete
            IBCR &= ~MS_SL;                                              // send end condition
            IBCR &= ~(IBIE);                                             // disable interrupts
            IIC_tx_control[0]->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);

            if ( IIC_tx_control[0]->wake_task ) {
               uTaskerStateChange( IIC_tx_control[0]->wake_task, UTASKER_ACTIVATE);// wake up owner task
            }
        }
        else {
            fnTxIIC(IIC_tx_control[0], 0);                               // we have another message to send so we can send a repeated start condition
        }
    }
}
#endif

#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // SPI reserved exclusively for a EEPROM file system {9}
void fnSendSPIMessage(unsigned char *ptrData, QUEUE_TRANSFER Length)     // master transmission
{
    volatile unsigned char ucDummy;                                      // dummy variable for clearing flags
    int iDelete = (ptrData == 0);
    unsigned char  ucMode = (PTS & SPI_SSEL);                            // check the state of the SSEL line on entry

    if (ucMode) {                                                        // If this is the first entry it has a control byte at the end
        Length--;                                                        // reduce the count so we don't send the control byte
    }

    PTS &= ~SPI_SSEL;                                                    // assert SS low before starting
    if (SPISR & SPIF) {                                                  // Check whether receive interrupt flag is set on entry
        ucDummy = SPIDR;                                                 // ensure start with receive data flag reset
    }

#ifdef _WINDOWS
    SPISR |= SPTEF;
    SPISR &= ~SPIF;
#endif

    while (Length--) {                                                   // for each byte we want to send
        if (iDelete) {
            SPIDR = 0xff;
        }
        else {
            SPIDR = *ptrData++;                                          // start byte transmission
        }
#ifdef _WINDOWS
        fnSimM95xxx(M95XXX_WRITE, SPIDR, (unsigned char)(PTS & SPI_SSEL)); // simulate the SPI EEPROM device
        SPISR |= (SPTEF | SPIF);                                         // simulate tx and rx interrupt flags being set
#endif
        while (!(SPISR & SPIF)) {};                                      // wait until tx byte has been sent and rx byte has been completely received
        ucDummy = SPIDR;                                                 // reset receive data flag with dummy read - the rx data is not interesting  here
    }

    if ((!ucMode) || (PREPARE_READ != *ptrData)) {
        PTS |= SPI_SSEL;                                                 // deassert SS when complete
    }
#ifdef _WINDOWS
    fnSimM95xxx(M95XXX_CHECK_SS, 0, (unsigned char)(PTS & SPI_SSEL));    // simulate the SPI EEPROM device
#endif
}


// We use this in master mode to get the next SPI byte as fast as possible
//
void fnGetSPI_bytes(unsigned char *ptrBuffer, QUEUE_TRANSFER Length)
{   // master reception - we assume SSEL asserted before entry (commanded previously)
    while (!(SPISR & SPTEF)) {};                                         // wait until free - should be very short wait

    while (Length--) {
        SPIDR = 0xff;                                                    // send a dummy byte to cause reception transfer
#ifdef _WINDOWS
        SPIDR = fnSimM95xxx(M95XXX_READ, SPIDR, (unsigned char)(PTS & SPI_SSEL));
        SPISR |= (SPTEF | SPIF);
#endif
        while (!(SPISR & SPIF)) {};                                      // wait for the byte to be transmitted / received (check RX)
        *ptrBuffer++ = SPIDR;
#ifdef _WINDOWS
        SPISR &= ~(SPTEF | SPIF);
#endif
    }
    PTS |= SPI_SSEL;                                                     // deassert SS always on completion
#ifdef _WINDOWS
    fnSimM95xxx(M95XXX_CHECK_SS, 0, (unsigned char)(PTS & SPI_SSEL));
#endif
}
#endif


#ifndef _WINDOWS
    #ifdef _CODE_WARRIOR
        #pragma CODE_SEG FILE_ROUTINE
    #endif
    #ifdef COMPILE_IAR
      #pragma location="FILE_ROUTINE"                                    // force IAR compiler to put this routine at the start of code
    #endif
#endif

#if (defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES) && defined _WINDOWS
    extern int iFetchingInternalMemory = 0;                              // {10}
#endif

extern void fnGetParsFile(MEMORY_RANGE_POINTER ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // {9}
    unsigned char usRead[4];                                             // our file sytem is in external EEPROM via SPI bus.
    usRead[0] = M95XXX_READ_MEMORY;
    usRead[1] = (unsigned char)((unsigned short)(ParLocation)>> 8);
    usRead[2] = (unsigned char)((unsigned short)ParLocation);
    usRead[3] = PREPARE_READ;
    fnWaitWriteComplete();                                               // ensure the chip is ready to be read
    #ifdef SPI_INTERFACE
    fnWrite(SPI_handle, usRead, sizeof(usRead));                         // set the read address
    fnRead(SPI_handle, ptrValue, Size);                                  // read from the device to the return buffer
    #else
    fnSendSPIMessage(usRead, sizeof(usRead));                            // send immediately
    fnGetSPI_bytes(ptrValue, Size);                                      // fetch immediately
    #endif
#else
    unsigned char ucOriginalPage;
    #ifdef _WINDOWS
    if (iFetchingInternalMemory != 0) {                                  // {10}
        uMemcpy(ptrValue, (const void *)ParLocation, Size);                            
        iFetchingInternalMemory = 0;
        return;
    }
    #endif
    #if defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM             // {5}{9}
    if (ParLocation >= (MEMORY_RANGE_POINTER)(SIZE_OF_FLASH)) {          // we are working from external SPI FLASH memory
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
        #if SPI_FLASH_PAGE_LENGTH >= 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #elif SPI_FLASH_PAGE_LENGTH >= 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #endif
        return;
    }
    #endif
    #if defined FILE404_IN_PROG || defined SUPPORT_INTERNAL_HTML_FILES   // {11}
    if ((ParLocation < (MEMORY_RANGE_POINTER)0x8000) || (ParLocation >= (MEMORY_RANGE_POINTER)0xc000)) {
        uMemcpy(ptrValue, (const void *)(unsigned short)ParLocation, Size);
        return;
    }
    #endif

    // Our file sytem is in internal FLASH
    // Warning this routine must not be in the page at 0x8000..0xbfff otherwise it will fail !!

    ucOriginalPage = PPAGE;

    uDisable_Interrupt();

    PPAGE = 0x3d;                                                        // switch to parameter page while protected from interrupts

    while (Size--) {
#ifdef _WINDOWS
          *ptrValue++ = *fnGetFlashAdd((unsigned char *)ParLocation);    // return each byte
#else
          *ptrValue++ = *(unsigned char *)(unsigned short)ParLocation;   // return each byte
#endif
          ParLocation = (MEMORY_RANGE_POINTER)((CAST_EXTENDED_POINTER_ARITHMETIC)ParLocation) + 1;
    }

    PPAGE = ucOriginalPage;                                              // set back original page so that we can run

    uEnable_Interrupt();
#endif
}

#ifdef _CODE_WARRIOR
    #pragma CODE_SEG DEFAULT
#endif

#ifdef USE_PARAMETER_BLOCK

#define PARAMETER_BLOCK_1 (MEMORY_RANGE_POINTER)(uFILE_START)
#define PARAMETER_BLOCK_2 (PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)
// This device can use an internal FLASH parameter block in the 0x3d page is required which is not deleted when loading new software
// The NE64 has 512 byte blocks which can be individually modified so we use one of these, and a backup if desired
unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned short usValidUse[2];
    unsigned short usCompare;

#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM                // {9}
    fnWaitWriteComplete();                                               // if a write is in progress to the first sector we must wait to ensure we read the correct value
#endif

    // The first 2 word location define the validity of the block. 0x5555 0x5555 means it is fully valid. 0xffff 0x5555 means it contains temporary unvalidated data
    fnGetParsFile(PARAMETER_BLOCK_1, (unsigned char*)usValidUse, sizeof(usValidUse));

    if (PRESENT_TEMP_BLOCK != ucValid) {
        usCompare = 0x5555;                                              // we are looking for validated pattern
    }
    else {
        usCompare = 0xffff;                                              // we are looking for temporary pattern
    }

    if ((usValidUse[0] == 0x5555) && (usValidUse[1] == usCompare)) {
        return 1;                                                        // first block matched request
    }
 #ifdef USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile(PARAMETER_BLOCK_2, (unsigned char*)usValidUse, sizeof(usValidUse));
        if ((usValidUse[0] == 0x5555) && (usValidUse[1] == usCompare)) {
            return 2;                                                    // second block matched request
        }
    }
 #endif

    return 0;                                                            // all other cases indicate the searched block doesn't exist
}

// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(MEMORY_RANGE_POINTER ptrAddInPar)            // {6}
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}

/*
// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(MEMORY_RANGE_POINTER ptrAddInPar)            // {3}
{
    int iSectors = (PARAMETER_BLOCK_SIZE/FLASH_GRANULARITY);

    ptrAddInPar = (MEMORY_RANGE_POINTER)((CAST_POINTER_ARITHMETIC)ptrAddInPar & ~(PARAMETER_BLOCK_SIZE-1));

    while (iSectors--) {
        if (fnEraseFlashSector(ptrAddInPar, 0)) return 1;
        ptrAddInPar += FLASH_GRANULARITY;
    }
    return 0;
}
  */


int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    MEMORY_RANGE_POINTER ptrPar = PARAMETER_BLOCK_1;

    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }

    ptrPar += (usParameterReference * 2);
    ptrPar += 4;                                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile(ptrPar, ucValue++, 1);
        ptrPar += 2;
    }
    return 0;
}

extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    MEMORY_RANGE_POINTER ptrPar = PARAMETER_BLOCK_1;
    MEMORY_RANGE_POINTER  ptrStart;
    unsigned short usValid = 0x5555;
    unsigned char  ucCheck;
    int iBlockUse = 0;

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {
        // No valid parameter blocks have been found so we can use the first for saving the data.
        // We delete it to be sure it is fresh
        fnDeleteParBlock((MEMORY_RANGE_POINTER)PARAMETER_BLOCK_1);       // {3}
        fnDeleteParBlock((MEMORY_RANGE_POINTER)PARAMETER_BLOCK_2);
        ucValidBlock = 1;
    }


    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    ptrStart = ptrPar;
    ptrPar += (usParameterReference * 2);
    ptrPar += 4;                                                         // first parameter starts after validation information

    if (!usLength) {
        if (iBlockUse) {
            iBlockUse = 0;                                               // if we have a new set waiting, set it to use state
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock(ptrPar);                                    // delete previous
        }
        else {
            fnDeleteParBlock(ptrStart);                                  // we must delete the block
            if (usParameterReference) return 0;                          // we don't validate it again
        }
    }
    else {
        while (usLength--) {
            fnGetParsFile(ptrPar, &ucCheck, 1);                          // get the present value to see whether it is already set or if it can not be modified
            if (ucCheck != *ucValue) {
                                                                         // the value is different
                if (ucCheck != 0xff) {                                   // we can not change a not blank value so we must swap block
                                                                         // we presently don't do anything in this case
                }
                else {
                    fnWriteBytesFlash(ptrPar, ucValue, 1);               // write each parameter to FLASH
                }
            }
            ucValue++;
            ptrPar += 2;                                                 // we use a word for each parameter
        }
    }
    // Now we must validate the parameter block which we have just written to
    fnWriteBytesFlash(ptrStart, (unsigned char *)&usValid, sizeof(usValid)); // validate data
    if (!iBlockUse) {
        fnWriteBytesFlash(ptrStart+sizeof(usValid), (unsigned char *)&usValid, sizeof(usValid)); // use data
    }
    return 0;
}

#endif


#ifdef GLOBAL_HARDWARE_TIMER
// In order to support a global hardware timer a second timer is required. We use 16 bit timer, set to 1ms rate
//
extern CLOCK_LIMIT fnSetHardwareTimer(CLOCK_LIMIT *hw_delay)
{
    DELAY_LIMIT Adjusted = 0;
    CLOCK_LIMIT NewDelay = *hw_delay;                                    // clock count desired {4}

    uDisable_Interrupt();

    if (TSCR1 & TEN) {                                                   // timer already running
        CLOCK_LIMIT Remaining = (TC7_HI - TCNT_HI);                      // time remaining before next timeout (in clocks)
        if (NewDelay >= Remaining) {
            *hw_delay = (NewDelay - Remaining);                          // leave the present timer running but adjust the next setting
            uEnable_Interrupt();
            return 0;
        }
        TSCR1 = 0;                                                       // freeze the timer
        Adjusted = TCNT_HI;                                              // note the frozen value
        TTIE = 0;                                                        // disable timer interrupt
    }
#if   MILLISEC == LOW_RES_MS                                             // configurable resolution {4}
    TSCR2 = (BUS_CLOCK_128 | TCRE);                                      // Bus clock / 128
#elif MILLISEC == MED_RES_MS
    TSCR2 = (BUS_CLOCK_32 | TCRE);                                       // Bus clock / 32
#else
    TSCR2 = (BUS_CLOCK_8 | TCRE);                                        // Bus clock / 8
#endif
    TIOS = (IOS7);                                                       // enable output compare
    TC7_HI = TCNT_HI;                                                    // cause match to reset counter
#ifdef _WINDOWS
    if (TSCR2 & TCRE) {
        TCNT_HI = 0;
    }
#endif
    TFLG1 = C7F;                                                         // reset interrupt request flag
    TC7_HI = NewDelay;                                                   // set ms unit compare rate (max is 167ms)
    TTIE = C7I;                                                          // enable interrupt
    TSCR1 = TEN;                                                         // enable timer (again)

//  *hw_delay = NewDelay;                                                // this is the value in clock pulses

    uEnable_Interrupt();
    return Adjusted;
}

// Stop the hardware timer
//
extern CLOCK_LIMIT fnStopHW_Timer(void)                                  // {12}
{
    CLOCK_LIMIT count_value;
    TSCR1 = 0;                                                           // freeze the timer
    TTIE = 0;                                                            // disable timer interrupt
    TFLG1 = C7F;                                                         // reset possible interrupt request flag
    count_value = TCNT_HI;                                               // note the frozen value
    TSCR1 = 0;                                                           // stop the timer
    return count_value;                                                  // return the value when stopped
}
#endif

// Configure a specific interrupt, including processor specific settings and enter a handler routine.
// Some specific peripheral control may be performed here.
//
extern void fnConfigureInterrupt(void *ptrSettings)                      // {13}
{
    switch (((INTERRUPT_SETUP *)ptrSettings)->int_type) {
#ifdef SUPPORT_TIMER
    case TIMER_INTERRUPT:
        #define FIRST_TIMER_CHANNEL 4
        #define LAST_TIMER_CHANNEL  7
        #define HW_TIMER_USES_TIMER 7
        {
            TIMER_SETUP *ptr_timer_settings = (TIMER_SETUP *)ptrSettings;
            int iChannel = ptr_timer_settings->channel;
            if ((iChannel < FIRST_TIMER_CHANNEL) || (iChannel > LAST_TIMER_CHANNEL)
    #if defined GLOBAL_TIMER_TASK && defined GLOBAL_HARDWARE_TIMER
                || (iChannel == HW_TIMER_USES_TIMER)
    #endif
                ) {
                return;                                                  // invalid channel or conflict with global hardware timer
            }
            else {                                                       // valid channel
            }
        }
        break;
#endif
    }
}


// This routine switches to low power mode. It is always called with disabled interrupts
//
extern void fnDoLowPower(void)
{
    // switch to low power mode

    // WAI intsruction (warning don't set CWAI in CLKSEL since it will disable PHY clock)

    uEnable_Interrupt();                                                 // allow wakeup on interrupts

    // Wait here until the processor is woken by an interrupt
}

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    ARMCOP = 0x33;                                                       // bad sequence should cause immediate reset
#ifndef _WINDOWS
    while (1) {};                                                        // rely on watchdog to reset us
#endif
}

#endif

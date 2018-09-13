/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5223XSim.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    16.03.2007 Limit tx UART characters to amount possible in a TICK period {1}
    16.03.2007 Added fnGetFlashAdd() for simulation of direct FLASH access {2}
    12.04.2007 Added fnPutFlashAdd() routine                             {3}
    15.05.2007 Add DMA Timer 3 simulation support                        {4}
    23.06.2007 Correct IRQ simulation on user port changes and improve level sensitive simulation - for IRQ1..IRQ7 {5}
    25.06.2007 Correct DD, GP and AN port function simulation            {6}
    11.08.2007 Add RTC support                                           {7}
    24.08.2007 User memset rather than uMemset since uMemset is not yet available when running from SRAM {8}
    14.09.2007 Correct transmission limits on UART 1 and UART 2          {9}
    15.09.2007 Add M5222X support         
    18.09.2007 Add Port DD peripheral                                    {10}
    22.09.2007 Add PNQPAR initialisation                                 {11}
    22.09.2007 Add M5221X support    
    10.10.2007 Allow application calling of interrupt simulation without argument list {12}
    21.10.2007 Add USB interface test support                            {13}
    02.11.2007 Add RTS output monitoring and fnSimulateModemChange()     {14}
    02.11.2007 Removed various interrupt routines in preference to direct __VECTOR_RAM accesses.
               Added some more accurate interrupt enable mask settings before calling interrupts.
               Use ucTxLast rather than UTB_RB_X to avoid data in the buffer getting corrupted by rx simulation {15}
    17.11.2007 Allow SPI and FLASH files system to work together         {16}
    09.12.2007 Correct TD and UC to dual function pins                   {17}
    15.12.2007 Add I2C simulation speed limitation for increased accuracy{18}
    27.12.2007 Correct DMA timer simulation when restart mode not active {19}
    27.12.2007 Extend DMA timers to include DMA timers 0 and 1           {20}
    18.03.2008 Add _SETx() and _CLEARx()                                 {21}
    22.03.2008 Add fnIRQ_int() to ensure IRQ is really enabled before calling handler {22}
    13.04.2008 Initialise ADC high level reference registers             {23}
    19.04.2008 Initialise extra registers                                {24}
    19.04.2008 Add _POWER_UP() and _POWER_DOWN()                         {25}
    20.04.2008 Add PIT0 and PIT1 simulations                             {26}
    22.04.2008 Add event flags to DMA timer simulations                  {27}
    27.04.2008 Adjust MCF_CLOCK_CCHR default for _M5221X                 {28}
    28.04.2008 Add ADC initial values and ADC simulation support         {29}
    23.05.2008 Add Internal RTC initialisation for chips which have this capability {30}
    02.06.2008 Correct IRQ8..IRQ15 mask                                  {31}
    02.06.2008 Add fnUpdateADC()                                         {32}
    02.06.2008 Extend trigger simulation to include second ADC in parallel mode {33}
    07.06.2008 Add second I2C channel                                    {34}
    09.06.2008 Add fnGetFlashSize() and support SAVE_COMPLETE_FLASH configuration {35}
    06.07.2008 Check DMA timer DMA mode and don't call interrupt when set{36}
    19.07.2008 Add fnSimulateBreak()                                     {37}
    19.07.2008 Add full serial reception DMA support                     {38}
    19.07.2008 Add M5225X support                                        {39}
    17.09.2008 Initialise IRQ1..IRQ7 priority levels                     {40}
    19.09.2008 Add USB host support                                      {41}
    29.10.2008 Add General Purpose Timer support                         {42}
    21.01.2009 Add PHY link up simulation for M5225x                     {43}
    27.01.2009 Add NVRAM support                                         {44}
    28.01.2009 Add Backup Watchdog Timer simulation                      {45}
    30.01.2009 Improve reset Controller Simulation                       {46}
    08.02.2009 Add iForce parameter to fnSimulateEthernetIn()            {47}
    15.02.2009 Initialise pointer ptrUSBData to avoid VS message         {48}
    26.02.2009 Extend RTS fnSimulateModemChange() to UARTs 1 and 2       {49}
    03.03.2009 Add iForce parameter to fnPortChanges()                   {50}
    13.03.2009 Add PWM module                                            {51}
    25.03.2009 Add M521XX configuration
    26.03.2009 Add M521X configuration (replacing _HW_M5213)             {52}
    10.04.2009 Limit tx UART characters to amount possible in a TICK period in DMA mode {53}
    17.04.2009 Handle CTS toggles in simulator                           {54}
    12.06.2009 Prime EPPDR0 with initial pin states and ensure that they follow the input when it changes {55}
    12.06.2009 Correct update of ucPerGP                                 {56}
    25.08.2009 Support directly connected keypad (non-matrix)            {57}
    25.08.2009 Queue forced port refreshes                               {58}
    10.04.2010 Add M5214, M5216, M5280, M5281 and M5282                  {59}
    13.05.2010 Add CRC32 to end of Ethernet rx frame                     {60}
    21.05.2010 Add M520X                                                 {61}
    13.09.2010 Only call DMA timer interrupt when their interrupt is unmasked {62}
    25.09.2010 Extend ADC simulation to check active conversion at each tick {63}
    13.03.2011 Correct UB peripheral                                     {64}
    17.06.2011 Add M523X                                                 {65}
    21.08.2011 Change time reference from first 2008 to 1970             {66}
    14.09.2011 Add M523X IRQ                                             {67}
    03.12.2011 Add channel number to fnSimCAN() and fnGetCANOwner() and Komodo simulation interface {68}
    13.02.2012 Improve simulation of PORTIN_SETNQ when pins programmed as IRQ {69}
    28.07.2012 Add handling DMA input capture events                     {70}
    16.10.2012 Flag port refresh on simulated input change               {71}
    16.10.2012 Reflect general purpose time inputs in their data register{72}
    13.03.2013 Add additional set and clear routines for _M5225X         {73}
    20.10.2013 Add GPT input state monitoring                            {74}

*/  


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#if defined _M5223X

#include "config.h"

#if defined CAN_INTERFACE && defined SIM_KOMODO                          // {68}
    #include "..\..\WinSim\Komodo\komodo.h" 
#endif



/**************************************************************************************************/
/********************************* FREESCALE M5223X Coldfire **************************************/
/**************************************************************************************************/

static unsigned char *fnSetDMADestination(unsigned char *ptrDestination, unsigned long ulValue, unsigned long ulCommand);
static void fnModemChange(int iUart, int iState, int iSetPort);

static unsigned char puQS, orig_puQS;
static unsigned char puAN, orig_puAN;
static unsigned char puLD, orig_puLD;
static unsigned char puNQ, orig_puNQ;
static unsigned char puTA, orig_puTA;
static unsigned char puTC, orig_puTC;
static unsigned char puTD, orig_puTD;
static unsigned char puUA, orig_puUA;
static unsigned char puUB, orig_puUB;
static unsigned char puUC, orig_puUC;
static unsigned char puAS, orig_puAS;
static unsigned char puGP, orig_puGP;
static unsigned char puDD, orig_puDD;
#if defined _M5225X                                                      // {39}
static unsigned char puTE, orig_puTE;
static unsigned char puTF, orig_puTF;
static unsigned char puTG, orig_puTG;
static unsigned char puTH, orig_puTH;
static unsigned char puTI, orig_puTI;
static unsigned char puTJ, orig_puTJ;
#endif
#if defined _M52XX_SDRAM                                                 // {59}
static unsigned char puA, orig_puA;
static unsigned char puB, orig_puB;
static unsigned char puC, orig_puC;
static unsigned char puD, orig_puD;
static unsigned char puE, orig_puE;
static unsigned char puF, orig_puF;
static unsigned char puG, orig_puG;
static unsigned char puH, orig_puH;
static unsigned char puJ, orig_puJ;
    #ifndef _M521X_SDRAM
static unsigned char puEH, orig_puEH;
    #endif
static unsigned char puEL, orig_puEL;
static unsigned char puQA, orig_puQA;
static unsigned char puQB, orig_puQB;
static unsigned char puSD, orig_puSD;
static unsigned char puTB, orig_puTB;
#endif

static unsigned char ucPortQS = 0;
static unsigned char ucPerQS = 0;
static unsigned char ucPort_in_QS;
static unsigned char ucPortAN = 0;
static unsigned char ucPerAN = 0;
static unsigned char ucPort_in_AN;
static unsigned char ucPortLD = 0;
static unsigned char ucPerLD = 0;
static unsigned char ucPort_in_LD;
static unsigned char ucPortNQ = 0;
static unsigned char ucPerNQ = 0;
static unsigned char ucPort_in_NQ;
static unsigned char _irqPORTIN_SETNQ;                                   // {69 input according to IRQs}
static unsigned char ucPortTA = 0;
static unsigned char ucPerTA = 0;
extern unsigned char ucPort_in_TA = 0;                                   // {72} extern to allow M5223x.c to use it when simulating
static unsigned char ucPortTC = 0;
static unsigned char ucPerTC = 0;
static unsigned char ucPort_in_TC;
static unsigned char ucPortTD = 0;
static unsigned char ucPerTD = 0;
static unsigned char ucPort_in_TD;
static unsigned char ucPortUA = 0;
static unsigned char ucPerUA = 0;
static unsigned char ucPort_in_UA;
static unsigned char ucPortUB = 0;
static unsigned char ucPerUB = 0;
static unsigned char ucPort_in_UB;
static unsigned char ucPortUC = 0;
static unsigned char ucPerUC = 0;
static unsigned char ucPort_in_UC;
static unsigned char ucPortAS = 0;
static unsigned char ucPerAS = 0;
static unsigned char ucPort_in_AS;
static unsigned char ucPortGP = 0;
static unsigned char ucPerGP = 0;
static unsigned char ucPort_in_GP;
static unsigned char ucPortDD = 0;
static unsigned char ucPerDD = 0;
static unsigned char ucPort_in_DD;
#if defined _M5225X                                                      // {39}
static unsigned char ucPortTE = 0;
static unsigned char ucPerTE = 0;
static unsigned char ucPort_in_TE;
static unsigned char ucPortTF = 0;
static unsigned char ucPerTF = 0;
static unsigned char ucPort_in_TF;
static unsigned char ucPortTG = 0;
static unsigned char ucPerTG = 0;
static unsigned char ucPort_in_TG;
static unsigned char ucPortTH = 0;
static unsigned char ucPerTH = 0;
static unsigned char ucPort_in_TH;
static unsigned char ucPortTI = 0;
static unsigned char ucPerTI = 0;
static unsigned char ucPort_in_TI;
static unsigned char ucPortTJ = 0;
static unsigned char ucPerTJ = 0;
static unsigned char ucPort_in_TJ;
#endif
#if defined _M52XX_SDRAM                                                 // {59}
static unsigned char ucPortA = 0;
static unsigned char ucPerA = 0;
static unsigned char ucPort_in_A;
static unsigned char ucPortB = 0;
static unsigned char ucPerB = 0;
static unsigned char ucPort_in_B;
static unsigned char ucPortC = 0;
static unsigned char ucPerC = 0;
static unsigned char ucPort_in_C;
static unsigned char ucPortD = 0;
static unsigned char ucPerD = 0;
static unsigned char ucPort_in_D;
static unsigned char ucPortE = 0;
static unsigned char ucPerE = 0;
static unsigned char ucPort_in_E;
static unsigned char ucPortF = 0;
static unsigned char ucPerF = 0;
static unsigned char ucPort_in_F;
static unsigned char ucPortG = 0;
static unsigned char ucPerG = 0;
static unsigned char ucPort_in_G;
static unsigned char ucPortH = 0;
static unsigned char ucPerH = 0;
static unsigned char ucPort_in_H;
static unsigned char ucPortJ = 0;
static unsigned char ucPerJ = 0;
static unsigned char ucPort_in_J;
    #ifndef _M521X_SDRAM
static unsigned char ucPortEH = 0;
static unsigned char ucPerEH = 0;
static unsigned char ucPort_in_EH;
    #endif
static unsigned char ucPortEL = 0;
static unsigned char ucPerEL = 0;
static unsigned char ucPort_in_EL;
static unsigned char ucPortQA = 0;
static unsigned char ucPerQA = 0;
static unsigned char ucPort_in_QA;
static unsigned char ucPortQB = 0;
static unsigned char ucPerQB = 0;
static unsigned char ucPort_in_QB;
static unsigned char ucPortSD = 0;
static unsigned char ucPerSD = 0;
static unsigned char ucPort_in_SD;
static unsigned char ucPortTB = 0;
static unsigned char ucPerTB = 0;
static unsigned char ucPort_in_TB;

static unsigned char ucPortDDR_A = 0;
static unsigned char ucPortDDR_B = 0;
static unsigned char ucPortDDR_C = 0;
static unsigned char ucPortDDR_D = 0;
static unsigned char ucPortDDR_E = 0;
static unsigned char ucPortDDR_F = 0;
static unsigned char ucPortDDR_G = 0;
static unsigned char ucPortDDR_H = 0;
static unsigned char ucPortDDR_J = 0;
static unsigned char ucPortDDR_EH = 0;
static unsigned char ucPortDDR_EL = 0;
static unsigned char ucPortDDR_QA = 0;
static unsigned char ucPortDDR_QB = 0;
static unsigned char ucPortDDR_TB = 0;
#endif
#if defined _M523X                                                       // {65}
static unsigned char ucPerADDR = 0;
static unsigned char ucPortADDR = 0;
static unsigned char ucPortDDR_ADDR = 0;
static unsigned char ucPerBS = 0;
static unsigned char ucPortBS = 0;
static unsigned char ucPortDDR_BS = 0;
static unsigned char ucPerBUSCTL = 0;
static unsigned char ucPortBUSCTL = 0;
static unsigned char ucPortDDR_BUSCTL = 0;
static unsigned char ucPerCS = 0;
static unsigned char ucPortCS = 0;
static unsigned char ucPortDDR_CS = 0;
static unsigned char ucPerDATAH = 0;
static unsigned char ucPortDATAH = 0;
static unsigned char ucPortDDR_DATAH = 0;
static unsigned char ucPerDATAL = 0;
static unsigned char ucPortDATAL = 0;
static unsigned char ucPortDDR_DATAL = 0;
static unsigned char ucPerFECI2C = 0;
static unsigned char ucPortFECI2C = 0;
static unsigned char ucPortDDR_FECI2C = 0;
static unsigned char ucPerETPU = 0;
static unsigned char ucPortETPU = 0;
static unsigned char ucPortDDR_ETPU = 0;
static unsigned char ucPerIRQ = 0;
static unsigned char ucPortIRQ = 0;
static unsigned char ucPortDDR_IRQ = 0;
static unsigned char ucPerQSPI = 0;
static unsigned char ucPortQSPI = 0;
static unsigned char ucPortDDR_QSPI = 0;
static unsigned char ucPerSDRAM = 0;
static unsigned char ucPortSDRAM = 0;
static unsigned char ucPortDDR_SDRAM = 0;
static unsigned char ucPerTIMER = 0;
static unsigned char ucPortTIMER = 0;
static unsigned char ucPortDDR_TIMER = 0;
static unsigned char ucPerUARTH = 0;
static unsigned char ucPortUARTH = 0;
static unsigned char ucPortDDR_UARTH = 0;
static unsigned char ucPerUARTL = 0;
static unsigned char ucPortUARTL = 0;
static unsigned char ucPortDDR_UARTL = 0;

static unsigned char ucPort_in_ADDR = 0;
static unsigned char ucPort_in_BS = 0;
static unsigned char ucPort_in_BUSCTL = 0;
static unsigned char ucPort_in_CS = 0;
static unsigned char ucPort_in_DATAH = 0;
static unsigned char ucPort_in_DATAL = 0;
static unsigned char ucPort_in_FECI2C = 0;
static unsigned char ucPort_in_IRQ = 0;
static unsigned char ucPort_in_ETPU = 0;
static unsigned char ucPort_in_QSPI = 0;
static unsigned char ucPort_in_SDRAM = 0;
static unsigned char ucPort_in_TIMER = 0;
static unsigned char ucPort_in_UARTH = 0;
static unsigned char ucPort_in_UARTL = 0;

static unsigned char puADDR, orig_puADDR;
static unsigned char puBS, orig_puBS;
static unsigned char puBUSCTL, orig_puBUSCTL;
static unsigned char puCS, orig_puCS;
static unsigned char puDATAH, orig_puDATAH;
static unsigned char puDATAL, orig_puDATAL;
static unsigned char puETPU, orig_puETPU;
static unsigned char puFECI2C, orig_puFECI2C;
static unsigned char puIRQ,  orig_puIRQ;
static unsigned char puQSPI, orig_puQSPI;
static unsigned char puSDRAM, orig_puSDRAM;
static unsigned char puTIMER, orig_puTIMER;
static unsigned char puUARTH, orig_puUARTH;
static unsigned char puUARTL, orig_puUARTL;
#endif
#if defined _M520X                                                       // {61}
static unsigned char ucPerFECH = 0;
static unsigned char ucPortFECH = 0;
static unsigned char ucPortDDR_FECH = 0;
static unsigned char ucPerFECL = 0;
static unsigned char ucPortFECL = 0;
static unsigned char ucPortDDR_FECL = 0;
static unsigned char ucPerBUSCTL = 0;
static unsigned char ucPortBUSCTL = 0;
static unsigned char ucPortDDR_BUSCTL = 0;
static unsigned char ucPerBE = 0;
static unsigned char ucPortBE = 0;
static unsigned char ucPortDDR_BE = 0;
static unsigned char ucPerCS = 0;
static unsigned char ucPortCS = 0;
static unsigned char ucPortDDR_CS = 0;
static unsigned char ucPerFECI2C = 0;
static unsigned char ucPortFECI2C = 0;
static unsigned char ucPortDDR_FECI2C = 0;
static unsigned char ucPerUART = 0;
static unsigned char ucPortUART = 0;
static unsigned char ucPortDDR_UART = 0;
static unsigned char ucPerQSPI = 0;
static unsigned char ucPortQSPI = 0;
static unsigned char ucPortDDR_QSPI = 0;
static unsigned char ucPerTIMER = 0;
static unsigned char ucPortTIMER = 0;
static unsigned char ucPortDDR_TIMER = 0;
static unsigned char ucPerIRQ = 0;
static unsigned char ucPortIRQ = 0;
static unsigned char ucPortDDR_IRQ = 0;

static unsigned char ucPort_in_FECH = 0;
static unsigned char ucPort_in_FECL = 0;
static unsigned char ucPort_in_BUSCTL = 0;
static unsigned char ucPort_in_BE = 0;
static unsigned char ucPort_in_CS = 0;
static unsigned char ucPort_in_FECI2C = 0;
static unsigned char ucPort_in_UART = 0;
static unsigned char ucPort_in_QSPI = 0;
static unsigned char ucPort_in_TIMER = 0;
static unsigned char ucPort_in_IRQ = 0;

static unsigned char puFECH, orig_puFECH;
static unsigned char puFECL, orig_puFECL;
static unsigned char puBUSCTL, orig_puBUSCTL;
static unsigned char puBE, orig_puBE;
static unsigned char puCS, orig_puCS;
static unsigned char puFECI2C, orig_puFECI2C;
static unsigned char puUART, orig_puUART;
static unsigned char puQSPI, orig_puQSPI;
static unsigned char puTIMER, orig_puTIMER;
static unsigned char puIRQ, orig_puIRQ;
#endif
static unsigned char ucPortDDR_QS = 0;
static unsigned char ucPortDDR_AN = 0;
static unsigned char ucPortDDR_LD = 0;
static unsigned char ucPortDDR_NQ = 0;
static unsigned char ucPortDDR_TA = 0;
static unsigned char ucPortDDR_TC = 0;
static unsigned char ucPortDDR_TD = 0;
static unsigned char ucPortDDR_UA = 0;
static unsigned char ucPortDDR_UB = 0;
static unsigned char ucPortDDR_UC = 0;
static unsigned char ucPortDDR_AS = 0;
static unsigned char ucPortDDR_GP = 0;
static unsigned char ucPortDDR_DD = 0;
#if defined _M5225X                                                      // {39}
static unsigned char ucPortDDR_TE = 0;
static unsigned char ucPortDDR_TF = 0;
static unsigned char ucPortDDR_TG = 0;
static unsigned char ucPortDDR_TH = 0;
static unsigned char ucPortDDR_TI = 0;
static unsigned char ucPortDDR_TJ = 0;
#endif
static unsigned char ucIRQ0 = 0;                                         // {55}
static unsigned char ucIRQ_P0 = 0;
static unsigned char ucIRQ1 = 0;

#if defined SUPPORT_ADC
static unsigned short usADC_values[ADC_CHANNELS];                        // {29}
#endif
static unsigned char ucPortFunctions[PORTS_AVAILABLE][PORT_WIDTH] = {0};
#if defined SUPPORT_ADC
    static void *ptrPorts[2] = {(void *)ucPortFunctions, (void *)usADC_values}; // {29}
#endif
#if defined USB_INTERFACE
    static unsigned long ulEndpointInt = 0;    
#endif
#if defined SERIAL_INTERFACE
    extern unsigned char ucEnabledState[3];                              // back up of the UART interrupt enable state (which is non-readable)
#endif
static int iFlagRefresh = 0;

// Initialisation for all non-zero registers in the device
//
static void fnSetDevice(unsigned short *usInputPorts)                    // {29} short passed to allow ADC voltage to be set
{
    extern void fnEnterHW_table(void *hw_table);

    MCF_SCM_IPSBAR = 0x40000001;                                         // System Control Module
#if !defined _M521X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {59}{61}{65}
    #if defined _M5221X || defined _M521XX || defined _M5225X
    MCF_CLOCK_CCHR = 5;
    MCF_CLOCK_OCLR = (RANGE | LPEN);
        #if defined _M5225X
    MCF_CLOCK_RTCCR = (RTC_REFS | RTC_LPEN);
        #endif
    #else
    MCF_CLOCK_CCHR = 4;
    #endif
#endif
#if defined _M5221X || defined _M5222X || defined _M521XX || defined _M5225X
    MCF_CLOCK_ROCR = 0x029e;                                             // typical relaxation oscillator trim value
#endif
#if defined _M523X                                                       // {65}
    MCF_CLOCK_SYNCR = 0x01100000;
#elif !defined _M520X                                                    // {61}
    MCF_CLOCK_SYNCR = 0x1002;
#endif
    MPARK      = 0x30e10000;
    MPR        = 0x03;
#if !defined _M523X                                                      // {65}
    IPSBMT     = 0x08;
#endif
    CCM_CIR    = COLDFIRE_ID;                                            // {24}
#if !defined _M52XX_SDRAM && !defined _M523X                             // {59}{65}
    CCM_CCR    = (BMT_32K | CCR_SINGLE_CHIP_MODE);                       // {24}
    PPMRL      = 0x00000008;                                             // {24}
    PWMPER0 = PWMPER1 = PWMPER2 = PWMPER3 = PWMPER4 = PWMPER5 = PWMPER6 = PWMPER7 = 0xff; // {51}
    PWMDTY0 = PWMDTY1 = PWMDTY2 = PWMDTY3 = PWMDTY4 = PWMDTY5 = PWMDTY6 = PWMDTY7 = 0xff;
#endif

#if defined _M5221X || defined _M521XX || defined _M5225X                // M5222X / M5221X / M521XX / M5225X have Backup Watchdog Timer Registers
    WCR        = 0x02;
    WMR        = 0xffff;
    WCNTR      = 0xffff;
    MCF_CLOCK_BWCR = 0x02;
#endif
#if !defined _M5221X && !defined _M5222X && !defined _M521XX             // M5222X / M5221X / M521XX have no FEC
    ECR        = 0xf0000000;                                             // FEC
    RCR        = 0x05ee0001;
    PAUR       = 0x00008808;
    MIBC       = 0xc0000000;
    OPD        = 0x00010000;
    FRBR       = 0x00000600;
    FRSR       = 0x00000500;
#endif
    IC_IMRL_0  = 0xffffffff;                                             // Interrupt controller
    IC_IMRH_0  = 0xffffffff;
#if !defined _M5222X && !defined _M5221X && !defined _M521XX             // M5222X / M5221X / M521XX have only one interrupt controller
    IC_IMRL_1  = 0xffffffff;
    IC_IMRH_1  = 0xffffffff;
#endif
    IC_ICR_0_1 = INTERRUPT_LEVEL_1;                                      // {40}
    IC_ICR_0_2 = INTERRUPT_LEVEL_2;
    IC_ICR_0_3 = INTERRUPT_LEVEL_3;
    IC_ICR_0_4 = INTERRUPT_LEVEL_4;
    IC_ICR_0_5 = INTERRUPT_LEVEL_5;
    IC_ICR_0_6 = INTERRUPT_LEVEL_6;
    IC_ICR_0_7 = INTERRUPT_LEVEL_7;

    CFMUSTAT   = 0xc0;                                                   // CFM controller

    EPDR0      = 0xff;                                                   // EPORT

    RESET_RCR  = (LVDIE | LVDE);                                         // {46} Reset controller
    RESET_RSR  = (RESET_POR | RESET_LVD);                                // assume power-on reset
#if !defined _M5222X && !defined _M5221X && !defined _M521XX             // M5222X / M5221X / M521XX have no CAN
    CANMCR = 0xd890000f;                                                 // CAN controller
    RXGMASK = 0x1fffffff;
    RX14MASK = 0x1fffffff;
    RX15MASK = 0x1fffffff;
#endif

    UIPCR_UACR_0 = 0x0f;                                                 // UART
    UIPCR_UACR_1 = 0x0f;
    UIPCR_UACR_2 = 0x0f;
    UIP_0 = 0xff;
    UIP_1 = 0xff;
    UIP_2 = 0xff;

    QMR = 0x0104;                                                        // QSPI
    QDLYR = 0x0404;

    DTRR0 = 0xffffffff;                                                  // DMA timers
    DTRR1 = 0xffffffff;
    DTRR2 = 0xffffffff;
    DTRR3 = 0xffffffff;
#if !defined _M52XX_SDRAM                                                // {59}
    ADC_CTRL1 = (STOP0 | SYNC0 | ADC_TRIG_OPN | ADC_PARALLEL_OPN);       // ADC
    ADC_CTRL2 = 0x0002;
    ADC_ADLST1 = 0x3210;
    ADC_ADLST2 = 0x7654;
    ADC_POWER = (0x00d0 | PD2 | PD1 | PD0);
    ADC_ADHLR0 = 0x7ff8;                                                 // {23}
    ADC_ADHLR1 = 0x7ff8;
    ADC_ADHLR2 = 0x7ff8;
    ADC_ADHLR3 = 0x7ff8;
    ADC_ADHLR4 = 0x7ff8;
    ADC_ADHLR5 = 0x7ff8;
    ADC_ADHLR6 = 0x7ff8;
    ADC_ADHLR7 = 0x7ff8;
#endif

    PIT_PMR_0   = 0xffff;                                                // PITs 
    PIT_PCNTR_0 = 0xffff;
    PIT_PMR_1   = 0xffff;
    PIT_PCNTR_1 = 0xffff;

#if defined _M5222X || defined _M5221X || defined _M5225X                // these devices have USB {39}
    PER_ID = 0x04;
    ID_COMP = 0xfb;
    REV = 0x33;
    ADD_INFO = IEHOST;
    OTG_INT_STAT = MSEC_1;
#endif

    I2SR = 0x81;                                                         // I2C
#if defined _M5221X || defined _M5225X || defined _M521XX                // {39}
    I2SR_1 = 0x81;                                                       // second I2C
#endif
#if !defined _M5222X && !defined _M5221X && !defined _M5225X && !defined _M521XX && !defined _M52XX_SDRAM // {59} M5222X / M5221X / M521XX / M5225X / M528X have no PHY
    EPHYCTL1 = 0x70;                                                     // EPHY
    EPHYSR   = 0x30;
#endif
#if defined RND_HW_SUPPORT
    RNGSR = 0x10000;
#endif
#if !defined _M521XX && !defined _M52XX_SDRAM                            // {59}
    RTCCTL = RTC_ENABLE;                                                 // RTC {7}
    STPWCH = MINUTES_MASK;
#endif
#if defined _M52XX_SDRAM                                                 // {59}
    PORTA = PORTB = PORTC = PORTD = PORTE = PORTF = PORTG = PORTH = PORTEL = 0xff;
    #ifndef _M521X_SDRAM
    PORTEH = 0xff;
    #endif
    PORTQS = 0x7f;
    PORTAS = PORTSD = 0x3f;
    PORTTC = PORTTD = PORTUA = 0x0f;
    PORTIN_SETQS = ucPort_in_QS = puQS = orig_puQS = (unsigned char)*usInputPorts++;
    ucPort_in_NQ = puNQ = orig_puNQ = (unsigned char)*usInputPorts++;
    EPPDR0 = ucPort_in_NQ;
    GPTPORT = ucPort_in_TA = puTA = orig_puTA = (unsigned char)*usInputPorts++;
    PORTIN_SETTC = ucPort_in_TC = puTC = orig_puTC = (unsigned char)*usInputPorts++;
    PORTIN_SETTD = ucPort_in_TD = puTD = orig_puTD = (unsigned char)*usInputPorts++;
    PORTIN_SETUA = ucPort_in_UA = puUA = orig_puUA = (unsigned char)*usInputPorts++;
    PORTIN_SETAS = ucPort_in_AS = puAS = orig_puAS = (unsigned char)*usInputPorts++;
    PORTIN_SETDD = ucPort_in_DD = puDD = orig_puDD = (unsigned char)*usInputPorts++;
    PORTIN_SETA  = ucPort_in_A  = puA  = orig_puA  = (unsigned char)*usInputPorts++;
    PORTIN_SETB  = ucPort_in_B  = puB  = orig_puB  = (unsigned char)*usInputPorts++;
    PORTIN_SETC  = ucPort_in_C  = puC  = orig_puC  = (unsigned char)*usInputPorts++;
    PORTIN_SETD  = ucPort_in_D  = puD  = orig_puD  = (unsigned char)*usInputPorts++;
    PORTIN_SETE  = ucPort_in_E  = puE  = orig_puE  = (unsigned char)*usInputPorts++;
    PORTIN_SETF  = ucPort_in_F  = puF  = orig_puF  = (unsigned char)*usInputPorts++;
    PORTIN_SETG  = ucPort_in_G  = puG  = orig_puG  = (unsigned char)*usInputPorts++;
    PORTIN_SETH  = ucPort_in_H  = puH  = orig_puH  = (unsigned char)*usInputPorts++;
    PORTIN_SETJ  = ucPort_in_J  = puJ  = orig_puJ  = (unsigned char)*usInputPorts++;
    #ifndef _M521X_SDRAM
    PORTIN_SETEH = ucPort_in_EH = puEH = orig_puEH = (unsigned char)*usInputPorts++;
    #endif
    PORTIN_SETEL = ucPort_in_EL = puEL = orig_puEL = (unsigned char)*usInputPorts++;
    PORTQA = ucPort_in_QA = puQA = orig_puQA = (unsigned char)*usInputPorts++;
    PORTQB = ucPort_in_QB = puQB = orig_puQB = (unsigned char)*usInputPorts++;
    PORTIN_SETSD = ucPort_in_SD = puSD = orig_puSD = (unsigned char)*usInputPorts++;
    GPT1PORT = ucPort_in_TB = puTB = orig_puTB = (unsigned char)*usInputPorts++;
    if (RCON_ASSERTED & (unsigned char)*usInputPorts++) {                // causes the states of D26:24, 21, 19:16 to determine the chip mode of operation
        unsigned char ucMode = (orig_puA & PORT_A_BIT2);                 // D26
        ucMode |= (orig_puB & PORT_B_BIT1);                              // D17
        ucMode |= (orig_puB & PORT_B_BIT0);                              // D16
        switch (ucMode) {
        case 0x7:                                                        // master mode
            CCM_RCON = RCON_MODE;
            CCM_CCR  = (CCR_MASTER_MODE | CCR_TSIZ | CCR_BME | CCR_PSTEN);            
            break;
        case 0x6:                                                        // single-chip mode
            CCM_RCON = RCON_SINGLE_CHIP;
            CCM_CCR  = (CCR_SINGLE_CHIP_MODE | CCR_BME | CCR_PSTEN);
            break;
        default:
            _EXCEPTION("Illegal configuration!!");
            break;
        }
        ucMode  = (orig_puB & PORT_B_BIT3);                              // D19
        ucMode |= (orig_puB & PORT_B_BIT2);                              // D18
        CCM_RCON |= (ucMode << 1);                                       // boot device width
        if (orig_puB & PORT_B_BIT5) {                                    // D21
            CCM_RCON |= RCON_RLOAD;                                      // full drive strength
            CCM_CCR |= CCR_LOAD;
        }
        ucMode  = (orig_puA & PORT_A_BIT1);                              // D25
        ucMode |= (orig_puA & PORT_A_BIT0);                              // D24
        CCM_RCON |= (ucMode << 8);                                       // chip select configuration
        if (CCM_RCON & RCON_MODE) {
            switch (CCM_RCON & RCON_BOOT_32BITS) {
            case RCON_BOOT_32BITS:
                PBCDPAR = (PBCDPAR_PBPA | PBCDPAR_PCDPA);
            case RCON_BOOT_16BITS:
                PBCDPAR |= (PBCDPAR_PCDPA);
                break;
            default:
                break;
            }
            PFPAR = 0xe0;
            PEPAR = 0x555f;
            PJPAR = 0xff;
            PSDPAR = PSDPAR_PSDPA;
        }
    }
    else {
        CCM_RCON = (0x00c0 | RCON_RLOAD | RCON_SINGLE_CHIP);             // default is single-chip mode with full drive strength
        CCM_CCR  = (CCR_LOAD | CCR_SINGLE_CHIP_MODE);
    }
    MCF_CLOCK_SYNSR = (PLLREF | PLLSEL | PLLMODE);                       // assumption that the CLKMOD setup choses normal PLL mode with crystal oscillator reference
#elif defined _M520X                                                     // {61}
    PAR_BUSCTL = 0x1f;
    PAR_BE = 0x0f;
    PAR_CS = 0x0f;
    PORTIN_SETFECH = ucPort_in_FECH = puFECH = orig_puFECH = (unsigned char)*usInputPorts++;
    PORTIN_SETFECL = ucPort_in_FECL = puFECL = orig_puFECL = (unsigned char)*usInputPorts++;
    ucPort_in_BUSCTL = puBUSCTL = orig_puBUSCTL = (unsigned char)*usInputPorts++;
    ucPort_in_BE = puBE = orig_puBE = (unsigned char)*usInputPorts++;
    PORTIN_SETCS = ucPort_in_CS = puCS = orig_puCS = (unsigned char)*usInputPorts++;        
    PORTIN_SETFECI2C = ucPort_in_FECI2C = puFECI2C = orig_puFECI2C = (unsigned char)*usInputPorts++;
    PORTIN_SETUART = ucPort_in_UART = puUART = orig_puUART = (unsigned char)*usInputPorts++;
    PORTIN_SETQSPI = ucPort_in_QSPI = puQSPI = orig_puQSPI = (unsigned char)*usInputPorts++;
    PORTIN_SETTIMER = ucPort_in_TIMER = puTIMER = orig_puTIMER = (unsigned char)*usInputPorts++;
    ucPort_in_IRQ = puIRQ = orig_puIRQ = (unsigned char)*usInputPorts++;

    CSAR0 = 0; 
    CSCR0 = (ASET_FOURTH_EDGE | RDAH_4 | WRAH_4 | WAIT_STATE_MASK | FB_AA | BEM);
    CSMR0 = (0x001f0000 | FB_CS_VALID);

    if (RCON_ASSERTED & (unsigned char)*usInputPorts++) {
        unsigned char ucMode = (unsigned char)*usInputPorts++;
        unsigned char ucBootPortSize = ((ucMode >> 2) & 0x03);
        if (ucMode & 0x01) {                                             // D1 determines PLL mode
            PODR = 0x36;                                                 // 166MHz core / 88MHz peripherals
            PFDR = 0x58;
        }
        else {
            PODR = 0x48;                                                 // 88MHz core / 44MHz peripherals
            PFDR = 0x7d;
        }
        if (ucMode & 0x02) {                                             // D2 determines oscillator mode
            // oscillator bypass mode
        }
        else {
            // crystal oscillator mode
        }
        switch (ucBootPortSize) {                                        // D4 and D3 determine the boot port size
        case 0:
        case 3:
            break;                                                       // 32 bit
        case 1:
            CSCR0 |= (PORT_SIZE_16);
            break;                                                       // 16 bit
        case 2:
            CSCR0 |= (PORT_SIZE_8);
            break;                                                       // 8 bit
        }

        if (ucMode & 0x10) {                                             // D5 determines ouput pad drive strength
            // high strength
        }
        else {
            // low strength
        }
        if (ucMode & 0x20) {                                             // D6 determines limp mode
            // limp mode enabled
        }
        else {
            // limp mode disabled
        }
        if (ucMode & 0x40) {                                             // D7 determines clock frequency input
            // 16.667MHz
        }
        else {
            // 16.000MHz
        }
        if (ucMode & 0x80) {                                             // D9 determines chip select configuration
            // A23..A22 are CS4..CS5
        }
        else {
            // A23..A22 are A23..A22
        }
    }
    else {                                                               // default configuration (D1..D8 not checked)
        usInputPorts++;
        // to do - set default modes
    }
#elif defined _M523X                                                     // {65}
    PORTIN_SETADDR = ucPort_in_ADDR = puADDR = orig_puADDR = (unsigned char)*usInputPorts++;
    PORTIN_SETBS = ucPort_in_BS = puBS = orig_puBS = (unsigned char)*usInputPorts++;
    PORTIN_SETBUSCTL = ucPort_in_BUSCTL = puBUSCTL = orig_puBUSCTL = (unsigned char)*usInputPorts++;
    PORTIN_SETCS = ucPort_in_CS = puCS = orig_puCS = (unsigned char)*usInputPorts++;
    PORTIN_SETDATAH = ucPort_in_DATAH = puDATAH = orig_puDATAH = (unsigned char)*usInputPorts++;
    PORTIN_SETDATAL = ucPort_in_DATAL = puDATAL = orig_puDATAL = (unsigned char)*usInputPorts++;
    PORTIN_SETETPU = ucPort_in_ETPU = puETPU = orig_puETPU = (unsigned char)*usInputPorts++;
    PORTIN_SETFECI2C = ucPort_in_FECI2C = puFECI2C = orig_puFECI2C = (unsigned char)*usInputPorts++;
    ucPort_in_IRQ = puIRQ = orig_puIRQ = (unsigned char)*usInputPorts++;
    PORTIN_SETQSPI = ucPort_in_QSPI = puQSPI = orig_puQSPI = (unsigned char)*usInputPorts++;
    PORTIN_SETSDRAM = ucPort_in_SDRAM = puSDRAM = orig_puSDRAM = (unsigned char)*usInputPorts++;
    PORTIN_SETTIMER = ucPort_in_TIMER = puTIMER = orig_puTIMER = (unsigned char)*usInputPorts++;
    PORTIN_SETUARTH = ucPort_in_UARTH = puUARTH = orig_puUARTH = (unsigned char)*usInputPorts++;
    PORTIN_SETUARTL = ucPort_in_UARTL = puUARTL = orig_puUARTL = (unsigned char)*usInputPorts++;

    CSAR0 = 0; 
    CSCR0 = (ASET_FOURTH_EDGE | RDAH_4 | WRAH_4 | WAIT_STATE_MASK | FB_AA | BEM);
    CSMR0 = (0x001f0000 | FB_CS_VALID);


    if (RCON_ASSERTED & (unsigned char)*usInputPorts++) {
        unsigned char ucMode = (unsigned char)*usInputPorts++;
        unsigned char ucBootPortSize = ((ucMode >> 1) & 0x03);           // D20, D19 - RCON[4:3] - boot device
        unsigned char ucClockMode = ((ucMode >> 6) & 0x03);              // CLKMOD1, CLKMOD0
        unsigned char ucChipSelect = ((ucMode >> 4) & 0x03);             // D25, D24 [RCON[9:8]]

        PAR_AD = (PAR_ADDR23 | PAR_ADDR22 | PAR_ADDR21);                 // PADDR default to peripheral functions
        switch (ucBootPortSize) {
        case 1:                                                          // external with 16 bit port
            break;
        case 2:                                                          // external with 8 bit port
            break;
        case 0:                                                          // external with 32 bit port
            PAR_AD = (PAR_ADDR23 | PAR_ADDR22 | PAR_ADDR21 | PAR_DATAL); // set PAR_DATAL to enable Data 15..0
        case 3:
            break;
        }
        CCM_RCON |= (ucBootPortSize << 3);                               // set corresponding bits in RCON
        if (ucMode & 0x08) {                                             // D21
            CCM_RCON |= 20;                                              // full strength drive
        }
        switch (ucChipSelect) {
        case 0:                                                          // PADDR[7:5] = A[23:21]
            break;
        case 1:                                                          // PADDR[7:6] = CS[6:5]; PADDR[5] = A[21]
            break;
        case 2:                                                          // PADDR[7] = CS[6]; PADDR[6:5] = A[22:21]
            break;
        case 3:                                                          // PADDR[7:5] = CS[7:5]
            break;
        }
        CCM_RCON |= ((unsigned short)ucChipSelect << 8);                 // set corresponding bits in RCON
    }
    else {                                                               // default configuration (D1..D8 not checked)
        usInputPorts++;
        // to do - set default modes
    }
#else
    #if defined _M521X                                                   // interrupt inputs
    PNQPAR = 0xfd;
    ucIRQ_P0 = ucIRQ0 = 0xfe;                                            // {55} default IRQs on port NQ
    #elif defined _M5225X                                                // {39}
    PNQPAR = (NQ_IRQ1_FUNCTION | NQ_IRQ3_FUNCTION | NQ_IRQ5_FUNCTION | NQ_IRQ7_FUNCTION);
    ucIRQ_P0 = ucIRQ0 = 0xaa;                                            // {55} default IRQs on port NQ
    #else
    PORTGP = 0xff;
    PORTLD = 0x7f;
    PNQPAR = 0x5554;                                                     // {11}
    ucIRQ_P0 = ucIRQ0 = 0xfe;                                            // {55} default IRQs on port NQ
    #endif

    PORTAN = PORTDD = 0xff;
    PORTAS = PORTTA = PORTTC = PORTUA = PORTUB = PORTUC = 0x0f;
    #if defined _M5225X   
    PORTTE = PORTTF = PORTTG = PORTTH = PORTTI = PORTTJ = 0xff;          // {39}
    #else
    PORTTD = 0x0f;
    #endif
    PORTQS = 0x7f;
    PORTNQ = 0xfe;

    PORTIN_SETQS = ucPort_in_QS = puQS = orig_puQS = (unsigned char)*usInputPorts++;
    PORTIN_SETAN = ucPort_in_AN = puAN = puAN = orig_puAN = (unsigned char)*usInputPorts++;
    #if !defined _M521X && !defined _M5225X                              // {39}
    ucPort_in_LD = puLD = orig_puLD = (unsigned char)*usInputPorts++;
    PORTIN_SETLD = ucPort_in_LD;
    #endif
    _irqPORTIN_SETNQ = ucPort_in_NQ = puNQ = orig_puNQ = (unsigned char)*usInputPorts++;
    PORTIN_SETNQ = 0xff;                                                 // {69} these pins default to IRQ function and read 0xff until programmed as GPIO
    EPPDR0 = (ucPort_in_NQ & ucIRQ0);                                    // {55}
    PORTIN_SETTA = ucPort_in_TA = puTA = orig_puTA = (unsigned char)*usInputPorts++;
    PORTIN_SETTC = ucPort_in_TC = puTC = orig_puTC = (unsigned char)*usInputPorts++;
    #if !defined _M5225X                                                     // {39}
    PORTIN_SETTD = ucPort_in_TD = puTD = orig_puTD = (unsigned char)*usInputPorts++;
    #endif
    PORTIN_SETUA = ucPort_in_UA = puUA = orig_puUA = (unsigned char)*usInputPorts++;
    PORTIN_SETUB = ucPort_in_UB = puUB = orig_puUB = (unsigned char)*usInputPorts++;
    PORTIN_SETUC = ucPort_in_UC = puUC = orig_puUC = (unsigned char)*usInputPorts++;
    PORTIN_SETAS = ucPort_in_AS = puAS = orig_puAS = (unsigned char)*usInputPorts++;
    #if !defined _M521X && !defined _M5225X                                  // {39}
    PORTIN_SETGP = ucPort_in_GP = puGP = orig_puGP = (unsigned char)*usInputPorts++;;
    #endif
    PORTIN_SETDD = ucPort_in_DD = puDD = orig_puDD = (unsigned char)*usInputPorts++;
    #if defined _M5225X                                                      // {39}
    PORTIN_SETTE = ucPort_in_TE = puTE = orig_puTE = (unsigned char)*usInputPorts++;
    PORTIN_SETTF = ucPort_in_TF = puTF = orig_puTF = (unsigned char)*usInputPorts++;
    PORTIN_SETTG = ucPort_in_TG = puTG = orig_puTG = (unsigned char)*usInputPorts++;
    PORTIN_SETTH = ucPort_in_TH = puTH = orig_puTH = (unsigned char)*usInputPorts++;
    PORTIN_SETTI = ucPort_in_TI = puTI = orig_puTI = (unsigned char)*usInputPorts++;
    PORTIN_SETTJ = ucPort_in_TJ = puTJ = orig_puTJ = (unsigned char)*usInputPorts++;
    CSCR0 = (BEM | FB_AA | ASET_FOURTH_EDGE | PORT_SIZE_8 | RDAH_4 | WRAH_4 | (MAX_WAIT_STATES << WAIT_STATE_SHIFT)); // FlexBus
  //CSCR1 = (BEM | FB_AA);
    #endif
#endif
#if defined SUPPORT_ADC
    memcpy(usADC_values, usInputPorts, sizeof(usADC_values));            // {29} prime initial ADC values
    fnEnterHW_table(ptrPorts);                                           
#else
    fnEnterHW_table(ucPortFunctions);
#endif
}

unsigned char ucFLASH[SIZE_OF_FLASH];                                    // simulated internal FLASH memory
#if defined NVRAM && defined _M5225X                                     // {44}
    static unsigned char ucNVRAM[SIZE_OF_NVRAM];                         // simulated NVRAM memory
#endif

extern unsigned long fnGetFlashSize(int iMemory)                         // {35}{44}
{
    switch (iMemory) {
#if defined NVRAM && defined _M5225X                                     // {44}
    case 1:
        return SIZE_OF_NVRAM;
#endif
    default:
        return SIZE_OF_FLASH;
    }
}

extern void fnInitialiseDevice(void *init_ports)
{
    memset(ucFLASH, 0xff, sizeof(ucFLASH));                              // we start with deleted FLASH memory contents {8}
#if defined NVRAM && defined _M5225X                                     // {44}
    memset(ucNVRAM, 0x55, sizeof(ucNVRAM));
#endif
    fnPrimeFileSystem();                                                 // the project can then set parameters or files as required
    fnSetDevice((unsigned short *)init_ports);                           // set device registers to startup condition (if not zerod)
}

extern void fnDeleteFlashSector(unsigned char *ptrSector)
{
	int i = FLASH_GRANULARITY;

	while (i--) {
		*ptrSector++ = 0xff;
	}
}

// Transform physical access address to address in simulated FLASH
//
extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd)                // {2}
{
    #if defined NVRAM && defined _M5225X                                 // {44}
    if (ucAdd >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) {
        unsigned long sim_add = (unsigned long)(ucNVRAM - SIZE_OF_FLASH);
        return (ucAdd + sim_add);                                        // return address in simulated NVRAM
    }
    else {
        unsigned long sim_add = (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
        return (ucAdd + sim_add);                                        // return address in simulated FLASH
    }
    #else
    unsigned char *ucSimulatedFlash = ucAdd + (unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS;
    if (ucSimulatedFlash >= &ucFLASH[SIZE_OF_FLASH]) {                   // check flash bounds
        _EXCEPTION("Attempted access outside of internal Flash bounds!!!");
    }
    return ucSimulatedFlash;                                             // return address in simulated FLASH
    #endif
}

extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd)                //  {3}
{
    #if defined NVRAM && defined _M5225X                                 // {44}
    if ((ucAdd >= ucNVRAM) && (ucAdd < (ucNVRAM + SIZE_OF_NVRAM))) {
        unsigned long sim_add = ((unsigned long)ucAdd - (unsigned long)(FLASH_START_ADDRESS + SIZE_OF_FLASH));
        return (ucAdd - sim_add);
    }
    else {
        unsigned long sim_add = ((unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS);
        return (ucAdd - sim_add);
    }
    #else
    unsigned long sim_add = ((unsigned long)ucFLASH - (unsigned long)FLASH_START_ADDRESS);
    return (ucAdd - sim_add);
    #endif
}


extern unsigned char *fnGetEEPROM(unsigned short usOffset);

extern unsigned char *fnGetFileSystemStart(int iMemory)                  // {44} - parameter added
{
    switch (iMemory) {
#if defined NVRAM && defined _M5225X
    case 1:                                                              // {44} specify get start of NVRAM
        return (ucNVRAM);
#endif

    default:
        #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM        // {16}
        return (fnGetEEPROM(uFILE_START));
        #else
            #if defined SAVE_COMPLETE_FLASH                              // {35}
        return (ucFLASH);
            #else
        return (&ucFLASH[uFILE_START]);
            #endif
        #endif
        break;
    }
}


extern M52235_PERIPH ucM5223X = {0};                                     // the devices peripherals

unsigned char uninitialisedRAM[4];

typedef struct stQSPI_RAM
{
    volatile unsigned short usTransmitRAM[16];
    volatile unsigned short usReceiveRAM[16];
    volatile unsigned char  ucCommandRAM[16];
} QSPI_RAM;


static QSPI_RAM ucQSPI_RAM[80];                                          // Uninitialised QSPI RAM - indirectly accessible




unsigned char *fnGetSimTxBufferAdd(void)
{
#if defined ETH_INTERFACE
    return (fnGetTxBufferAdd(0));
#else
    return 0;
#endif
}

extern void ( *__VECTOR_RAM[256] )( void) = {0};                         // interrupt vector table in RAM


extern void RealTimeInterrupt(void)                                      // {26} now dummy - PIT simulation used instead
{
//    __VECTOR_RAM[PIT0_VECTOR]();
}

#ifndef _M521X
extern void fec_txf_isr(void)
{
    __VECTOR_RAM[FEC_TX_F_VECTOR]();
}
#endif



// Process simulated interrupts
//
extern unsigned long fnSimInts(char *argv[])
{    
#if defined SERIAL_INTERFACE
    extern unsigned char ucTxLast[NUMBER_SERIAL];
#endif
    unsigned long ulNewActions = 0;
    int *ptrCnt;                                                         // {1}

    if ((iInts & CHANNEL_0_SERIAL_INT) && (argv)) {                      // {12}
        ptrCnt = (int *)argv[THROUGHPUT_UART0];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_0_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_0_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx0(ucTxLast[0]);                                   // {15}
                ulNewActions |= SEND_COM_0;
                if (ucEnabledState[0] & UART_TXRDY_MASK) {
                    UIMR_UISR_0 |= (UART_TXRDY_MASK);                    // simulate Tx int 
                    __VECTOR_RAM[UART0_VECTOR]();                        // call the interrupt routine
                }
#endif
            }
        }
	}

	if ((iInts & CHANNEL_1_SERIAL_INT) && (argv)) {                      // {12}
        ptrCnt = (int *)argv[THROUGHPUT_UART1];                          // {9}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_1_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_1_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx1(ucTxLast[1]);                                   // {15}
                ulNewActions |= SEND_COM_1;
                if (ucEnabledState[1] & UART_TXRDY_MASK) {               // check whether the interrupt is enabled
                    UIMR_UISR_1 |= (UART_TXRDY_MASK);                    // simulate Tx int 
                    __VECTOR_RAM[UART1_VECTOR]();
                }
#endif
            }
        }
	}

	if ((iInts & CHANNEL_2_SERIAL_INT) && (argv)) {                      // {12}
        ptrCnt = (int *)argv[THROUGHPUT_UART2];                          // {9}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= CHANNEL_2_SERIAL_INT;                          // enough serial interupts handled in this tick period
            }
            else {
		        iInts &= ~CHANNEL_2_SERIAL_INT;                          // interrupt has been handled
#if defined SERIAL_INTERFACE
	            fnLogTx2(ucTxLast[2]);                                   // {15}
                ulNewActions |= SEND_COM_2;
                if (ucEnabledState[2] & UART_TXRDY_MASK) {               // check whether the interrupt is enabled
                    UIMR_UISR_2 |= (UART_TXRDY_MASK);                    // simulate Tx int 
                    __VECTOR_RAM[UART2_VECTOR]();
                }
#endif
            }
        }
	}

    if (iInts & I2C_INT0) {
        ptrCnt = (int *)argv[THROUGHPUT_I2C0];                           // {18}
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT0;                                      // enough I2C interupts handled in this tick period
            }
            else {
#if defined I2C_INTERFACE
		        iInts &= ~I2C_INT0;
                __VECTOR_RAM[I2C_VECTOR]();                              // simulate I2C int 
#endif
            }
        }
    }
                                                       
    if (iInts & I2C_INT1) {                                              // {4}
        ptrCnt = (int *)argv[THROUGHPUT_I2C1];
        if (*ptrCnt) {
            if (--(*ptrCnt) == 0) {
                iMasks |= I2C_INT1;                                      // enough I2C interupts handled in this tick period
            }
            else {
#if NUMBER_I2C > 1
		        iInts &= ~I2C_INT1;
                __VECTOR_RAM[I2C_1_VECTOR]();                            // simulate I2C int 
#endif
            }
        }
    }
#if defined USB_INTERFACE
    if (iInts & USB_INT) {
        int iEndpoint = 0;
        iInts &= ~USB_INT;
        while ((iEndpoint < NUMBER_OF_USB_ENDPOINTS) && (ulEndpointInt != 0)) {
            if (ulEndpointInt & (1 << iEndpoint)) {
                ulEndpointInt &= ~(1 << iEndpoint);
                fnCheckUSBOut(0, iEndpoint);
            }
            iEndpoint++;
        }
    }
#endif
    return ulNewActions;
}

static unsigned char *fnSetDMADestination(unsigned char *ptrDestination, unsigned long ulValue, unsigned long ulCommand)
{
    switch (ulCommand & DCR_DSIZE_LINE) {
        case DCR_DSIZE_LONG:
            *(unsigned long *)ptrDestination = ulValue;
            if (ulCommand & DCR_DINC) {                      // destination increment active
                ptrDestination += sizeof(unsigned long);
            }
            break;
        case DCR_DSIZE_WORD:
            *(unsigned short *)ptrDestination = (unsigned short)ulValue;
            if (ulCommand & DCR_DINC) {                      // destination increment active
                ptrDestination += sizeof(unsigned short);
            }
            break;
        case DCR_DSIZE_LINE:
        case DCR_DSIZE_BYTE:
            *ptrDestination = (unsigned char)ulValue;
            if (ulCommand & DCR_DINC) {                      // destination increment active
                ptrDestination++;
            }
            break;
    }
    return ptrDestination;
}


// Process simulated DMA
//
extern unsigned long fnSimDMA(char *argv[])
{
    unsigned long ulNewActions = 0;
    int *ptrCnt;                                                         // {53}

    if (argv == 0) {                                                     // memory to memory transfer
        if ((DMA_DCR3 & DCR_START) != 0) {
            unsigned long ulLength = (DMA_SR_BCR3 & DMA_BYTE_COUNT_MASK);
            unsigned char *ptrSource = (unsigned char *)DMA_SAR3;
            unsigned char *ptrDestination = (unsigned char *)DMA_DAR3;
            DMA_DCR3 &= ~DCR_START;

            while (ulLength--) {
                *ptrDestination = *ptrSource;
                if (DMA_DCR3 & DCR_DINC) {
                    ptrDestination++;
                }
                if (DMA_DCR3 & DCR_SINC) {
                    ptrSource++;
                }
            }

            DMA_SR_BCR3 = DSR_DONE;                                      // indicate transfer complete (count has decremented to zero)
        }
        return 0;
    }

    if ((iDMA & DMA_CONTROLLER_0) != 0) {                                // DMA 0 has work to do
        unsigned long *ptrDMA = (unsigned long *)DMA_ADD;
        unsigned char *ptrSource = *(unsigned char **)ptrDMA++;
        unsigned char *ptrDestination = *(unsigned char **)ptrDMA++;
        unsigned long ulLength = (*ptrDMA++ & DMA_BYTE_COUNT_MASK);
        unsigned long ulLengthHandled = 0;
        unsigned long ulCommand = *ptrDMA--;

        if ((*ptrDMA & DMA_ABORTED) != 0) {                              // {53} DMA transfer aborted
            iDMA &= ~DMA_CONTROLLER_0;                                   // no more DMA processing to be performed
        }
        else {
            if ((ptrDestination == (UART_ADD + UTB_RB_0_OFFSET)) && (ulCommand & DCR_EEXT)) { // we support only UART transmissions
                unsigned char *ptrReg = (unsigned char *)(UART_ADD + UCR_0_OFFSET);
                ptrCnt = (int *)argv[THROUGHPUT_UART0];                  // {53}
                if (*ptrCnt != 0) {
                    if ((*ptrReg == UART_TX_ENABLE) || (*ptrReg  == UART_RESET_CMD_PTR)) { // we expect the last command to be an enable command (or reset cmd pointer)
                        while (ulLength-- != 0) {                        // for each DMA transfer
                            switch (ulCommand & DCR_SSIZE_LINE) {
                            case DCR_SSIZE_LONG:
                                ptrDestination = fnSetDMADestination(ptrDestination, *(unsigned long *)ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource += sizeof(unsigned long);
                                }
                                break;

                            case DCR_SSIZE_BYTE:
                                ptrDestination = fnSetDMADestination(ptrDestination, *ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource++;
                                }
                                break;

                            case DCR_SSIZE_WORD:
                                ptrDestination = fnSetDMADestination(ptrDestination, *(unsigned short *)ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource += sizeof(unsigned short);
                                }
                                break;

                            case DCR_SSIZE_LINE:
                                {
                                    int iBurst = 16;                     // 16 byte burst
                                    while (iBurst--) {
                                        ptrDestination = fnSetDMADestination(ptrDestination, *ptrSource, ulCommand);
                                        if (ulCommand & DCR_SINC) {      // source increment active
                                            ptrSource++;
                                        }
                                    }                         
                                    break;
                                }
                            }
	                        fnLogTx0(UTB_RB_0);                          // assuming the transfer has been into the tx buffer, cause the byte to be sent
                            ulLengthHandled++;
                            if (--(*ptrCnt) == 0) {                      // {53}
                                iMasks |= DMA_CONTROLLER_0;              // enough serial DMA transfers handled in this tick period
                                break;
                            }
                        }
                        ulLength = (*ptrDMA & DMA_BYTE_COUNT_MASK);      // original length
                        ulLength -= ulLengthHandled;                     // new length
                        *ptrDMA &= ~DMA_BYTE_COUNT_MASK;
                        *ptrDMA |= ulLength;
                        if (ulLength == 0) {                             // {53}
                            *ptrDMA &= ~DSR_BSY;
                            *ptrDMA |= DSR_DONE;
                        }
                        *(ptrDMA-1) = (unsigned long)ptrDestination;     // {53}
                        *(ptrDMA-2) = (unsigned long)ptrSource;

                        if (*ptrDMA & DSR_DONE) {                        // {53}
                            iDMA &= ~DMA_CONTROLLER_0;                   // no more DMA processing to be performed
                            if (ulCommand & DCR_INT) {                   // if DMA interrup enabled
                                __VECTOR_RAM[DMA0_VECTOR]();             // simulate DMA interrupt once transfer has completed
                            }
                        }
                        ulNewActions |= SEND_COM_0;                      // inform UART simulator that data should be sent
                    }
                }
            }
        }
    }

    if ((iDMA & DMA_CONTROLLER_1) != 0) {                                // DMA 1 has work to do
        unsigned long *ptrDMA = (unsigned long *)(DMA_ADD + DMA_CHAN_1_OFFSET);
        unsigned char *ptrSource = *(unsigned char **)ptrDMA++;
        unsigned char *ptrDestination = *(unsigned char **)ptrDMA++;
        unsigned long ulLength = (*ptrDMA++ & DMA_BYTE_COUNT_MASK);
        unsigned long ulLengthHandled = 0;
        unsigned long ulCommand = *ptrDMA--;

        if (*ptrDMA & DMA_ABORTED) {                                     // {53} DMA transfer aborted
            iDMA &= ~DMA_CONTROLLER_1;                                   // no more DMA processing to be performed
        }
        else {
            if ((ptrDestination == (UART_ADD + SCI_BLOCK_SIZE + UTB_RB_0_OFFSET)) && (ulCommand & DCR_EEXT)) { // we support only UART transmissions
                unsigned char *ptrReg = (unsigned char *)(UART_ADD + SCI_BLOCK_SIZE + UCR_0_OFFSET);
                ptrCnt = (int *)argv[THROUGHPUT_UART1];                  // {53}
                if (*ptrCnt != 0) {
                    if ((*ptrReg == UART_TX_ENABLE) || (*ptrReg == UART_RESET_CMD_PTR)) { // we expect the last command to be an enable command (or reset cmd pointer)
                        while (ulLength-- != 0) {                        // for each DMA transfer
                            switch (ulCommand & DCR_SSIZE_LINE) {
                            case DCR_SSIZE_LONG:
                                ptrDestination = fnSetDMADestination(ptrDestination, *(unsigned long *)ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource += sizeof(unsigned long);
                                }
                                break;

                            case DCR_SSIZE_BYTE:
                                ptrDestination = fnSetDMADestination(ptrDestination, *ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource++;
                                }
                                break;

                            case DCR_SSIZE_WORD:
                                ptrDestination = fnSetDMADestination(ptrDestination, *(unsigned short *)ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource += sizeof(unsigned short);
                                }
                                break;

                            case DCR_SSIZE_LINE:
                                {
                                    int iBurst = 16;                     // 16 byte burst
                                    while (iBurst--) {
                                        ptrDestination = fnSetDMADestination(ptrDestination, *ptrSource, ulCommand);
                                        if (ulCommand & DCR_SINC) {      // source increment active
                                            ptrSource++;
                                        }
                                    }                         
                                    break;
                                }
                            }
	                        fnLogTx1(UTB_RB_1);                          // assuming the transfer has been into the tx buffer, cause the byte to be sent
                            ulLengthHandled++;
                            if (--(*ptrCnt) == 0) {                      // {53}
                                iMasks |= DMA_CONTROLLER_1;              // enough serial DMA transfers handled in this tick period
                                break;
                            }
                        }
                        ulLength = (*ptrDMA & DMA_BYTE_COUNT_MASK);      // original length
                        ulLength -= ulLengthHandled;                     // new length
                        *ptrDMA &= ~DMA_BYTE_COUNT_MASK;
                        *ptrDMA |= ulLength;
                        if (ulLength == 0) {                             // {53}
                            *ptrDMA &= ~DSR_BSY;
                            *ptrDMA |= DSR_DONE;
                        }
                        *(ptrDMA-1) = (unsigned long)ptrDestination;     // {53}
                        *(ptrDMA-2) = (unsigned long)ptrSource;

                        if (*ptrDMA & DSR_DONE) {                        // {53}
                            iDMA &= ~DMA_CONTROLLER_1;                   // no more DMA processing to be performed
                            if (ulCommand & DCR_INT) {                   // if DMA interrup enabled
                                __VECTOR_RAM[DMA1_VECTOR]();             // simulate DMA interrupt once transfer has completed
                            }
                        }
                        ulNewActions |= SEND_COM_1;                      // inform UART simulator that data should be sent
                    }
                }
            }
        }
    }

    if ((iDMA & DMA_CONTROLLER_2) != 0) {                                // DMA 2 has work to do
        unsigned long *ptrDMA = (unsigned long *)(DMA_ADD + 2*DMA_CHAN_1_OFFSET);
        unsigned char *ptrSource = *(unsigned char **)ptrDMA++;
        unsigned char *ptrDestination = *(unsigned char **)ptrDMA++;
        unsigned long ulLength = (*ptrDMA++ & DMA_BYTE_COUNT_MASK);
        unsigned long ulLengthHandled = 0;
        unsigned long ulCommand = *ptrDMA--;

        if (*ptrDMA & DMA_ABORTED) {                                     // {53} DMA transfer aborted
            iDMA &= ~DMA_CONTROLLER_2;                                   // no more DMA processing to be performed
        }
        else {
            if ((ptrDestination == (UART_ADD + 2*SCI_BLOCK_SIZE + UTB_RB_0_OFFSET)) && (ulCommand & DCR_EEXT)) { // we support only UART transmissions
                unsigned char *ptrReg = (unsigned char *)(UART_ADD + 2*SCI_BLOCK_SIZE + UCR_0_OFFSET);
                ptrCnt = (int *)argv[THROUGHPUT_UART2];                  // {53}
                if (*ptrCnt != 0) {
                    if ((*ptrReg == UART_TX_ENABLE) || (*ptrReg == UART_RESET_CMD_PTR)) { // we expect the last command to be an enable command (or reset cmd pointer)
                        while (ulLength-- != 0) {                        // for each DMA transfer
                            switch (ulCommand & DCR_SSIZE_LINE) {
                            case DCR_SSIZE_LONG:
                                ptrDestination = fnSetDMADestination(ptrDestination, *(unsigned long *)ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource += sizeof(unsigned long);
                                }
                                break;

                            case DCR_SSIZE_BYTE:
                                ptrDestination = fnSetDMADestination(ptrDestination, *ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource++;
                                }
                                break;

                            case DCR_SSIZE_WORD:
                                ptrDestination = fnSetDMADestination(ptrDestination, *(unsigned short *)ptrSource, ulCommand);
                                if (ulCommand & DCR_SINC) {              // source increment active
                                    ptrSource += sizeof(unsigned short);
                                }
                                break;

                            case DCR_SSIZE_LINE:
                                {
                                    int iBurst = 16;                     // 16 byte burst
                                    while (iBurst--) {
                                        ptrDestination = fnSetDMADestination(ptrDestination, *ptrSource, ulCommand);
                                        if (ulCommand & DCR_SINC) {      // source increment active
                                            ptrSource++;
                                        }
                                    }                         
                                    break;
                                }
                            }
	                        fnLogTx2(UTB_RB_2);                          // assuming the transfer has been into the tx buffer, cause the byte to be sent
                            ulLengthHandled++;
                            if (--(*ptrCnt) == 0) {                      // {53}
                                iMasks |= DMA_CONTROLLER_2;              // enough serial DMA transfers handled in this tick period
                                break;
                            }
                        }
                        ulLength = (*ptrDMA & DMA_BYTE_COUNT_MASK);      // original length
                        ulLength -= ulLengthHandled;                     // new length
                        *ptrDMA &= ~DMA_BYTE_COUNT_MASK;
                        *ptrDMA |= ulLength;
                        if (ulLength == 0) {                             // {53}
                            *ptrDMA &= ~DSR_BSY;
                            *ptrDMA |= DSR_DONE;
                        }
                        *(ptrDMA-1) = (unsigned long)ptrDestination;     // {53}
                        *(ptrDMA-2) = (unsigned long)ptrSource;

                        if (*ptrDMA & DSR_DONE) {                        // {53}
                            iDMA &= ~DMA_CONTROLLER_2;                   // no more DMA processing to be performed
                            if (ulCommand & DCR_INT) {                   // if DMA interrup enabled
                                __VECTOR_RAM[DMA2_VECTOR]();             // simulate DMA interrupt once transfer has completed
                            }
                        }
                        ulNewActions |= SEND_COM_2;                      // inform UART simulator that data should be sent
                    }
                }
            }
        }
    }
    return ulNewActions;
}

// Process serial reception using DMA transfer                           {38}
//
static void fnRxSerialDMA(int iChannel)
{
    int iDMA_channel;
    unsigned char *ptrReg = 0;
    unsigned char ucData;
    volatile unsigned long *ptrDMA;
    switch (iChannel) {                                                  // the first thing to be checked is whether DMA is configured and DMA controller is configured to work with this UART
    case 0:
        if ((DMAREQC & (DMA_MASK << DMAC_0_SHIFT)) ==  (DMA_UART_0_RX << DMAC_0_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 0 is configured
            iDMA_channel = 0;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_1_SHIFT)) ==  (DMA_UART_0_RX << DMAC_1_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 1 is configured
            ptrReg += DMA_CHAN_1_OFFSET;
            iDMA_channel = 1;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_2_SHIFT)) ==  (DMA_UART_0_RX << DMAC_2_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 2 is configured
            ptrReg += DMA_CHAN_2_OFFSET;
            iDMA_channel = 2;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_3_SHIFT)) ==  (DMA_UART_0_RX << DMAC_3_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 3 is configured
            ptrReg += DMA_CHAN_3_OFFSET;
            iDMA_channel = 3;
        }
        break;
    case 1:
        if ((DMAREQC & (DMA_MASK << DMAC_0_SHIFT)) ==  (DMA_UART_1_RX << DMAC_0_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 0 is configured
            iDMA_channel = 0;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_1_SHIFT)) ==  (DMA_UART_1_RX << DMAC_1_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 1 is configured
            ptrReg += DMA_CHAN_1_OFFSET;
            iDMA_channel = 1;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_2_SHIFT)) ==  (DMA_UART_1_RX << DMAC_2_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 2 is configured
            ptrReg += DMA_CHAN_2_OFFSET;
            iDMA_channel = 2;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_3_SHIFT)) ==  (DMA_UART_1_RX << DMAC_3_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 3 is configured
            ptrReg += DMA_CHAN_3_OFFSET;
            iDMA_channel = 3;
        }
        break;
    case 2:
        if ((DMAREQC & (DMA_MASK << DMAC_0_SHIFT)) ==  (DMA_UART_2_RX << DMAC_0_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 0 is configured
            iDMA_channel = 0;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_1_SHIFT)) ==  (DMA_UART_2_RX << DMAC_1_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 1 is configured
            ptrReg += DMA_CHAN_1_OFFSET;
            iDMA_channel = 1;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_2_SHIFT)) ==  (DMA_UART_2_RX << DMAC_2_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 2 is configured
            ptrReg += DMA_CHAN_2_OFFSET;
            iDMA_channel = 2;
        }
        else if ((DMAREQC & (DMA_MASK << DMAC_3_SHIFT)) ==  (DMA_UART_2_RX << DMAC_3_SHIFT)) {
            ptrReg = (unsigned char *)DMA_ADD;                           // DMA controller 3 is configured
            ptrReg += DMA_CHAN_3_OFFSET;
            iDMA_channel = 3;
        }
        break;
    }
    if (ptrReg == 0) {
        return;                                                          // no DMA controller handling this UART
    }
    ptrDMA = (volatile unsigned long *)ptrReg;
    if (*(ptrDMA + 2) & DSR_DONE) {
        return;
    }
    ucData = *(unsigned char *)*ptrDMA++;                                // read the data from the source
    *(unsigned char *)*ptrDMA = ucData;
    *ptrDMA += 1;                                                        // assume destination increment
    ptrDMA++;
    *ptrDMA -= 1;                                                        // decrement the count
                                                                         // style assumed (DCR_INT | DCR_EEXT | DCR_CS | DCR_DINC | DCR_SSIZE_BYTE | DCR_DSIZE_BYTE)
    if (*ptrDMA++ == 0) {                                                // DMA transfer terminated
        switch (iDMA_channel) {
        case 0:
            if ((*ptrDMA & DCR_INT) && (!(IC_IMRL_0 & DMA0_PIF_INT_L))) {// interrupt on completion
                __VECTOR_RAM[DMA0_VECTOR]();                             // call the interrupt routine
            }
            break;
        case 1:
            if ((*ptrDMA & DCR_INT) && (!(IC_IMRL_0 & DMA1_PIF_INT_L))) {// interrupt on completion
                __VECTOR_RAM[DMA1_VECTOR]();                             // call the interrupt routine
            }
            break;
        case 2:
            if ((*ptrDMA & DCR_INT) && (!(IC_IMRL_0 & DMA2_PIF_INT_L))) {// interrupt on completion
                __VECTOR_RAM[DMA2_VECTOR]();                             // call the interrupt routine
            }
            break;
        case 3:
            if ((*ptrDMA & DCR_INT) && (!(IC_IMRL_0 & DMA3_PIF_INT_L))) {// interrupt on completion
                __VECTOR_RAM[DMA3_VECTOR]();                             // call the interrupt routine
            }
            break;
        }
    }
}


// Simulate the reception of serial data by inserting bytes into the input buffer and calling interrupts
//
extern void fnSimulateSerialIn(int iPort, unsigned char *ptrDebugIn, unsigned short usLen)
{
 #if defined SERIAL_INTERFACE
    if (iPort == 2) {
	    while (usLen--) {
            UIMR_UISR_2 |= (UART_RXRDY_MASK);
		    UTB_RB_2 = *ptrDebugIn++;
            if (ucEnabledState[2] & UART_RXRDY_MASK) {
                __VECTOR_RAM[UART2_VECTOR]();
            }
            else {
                fnRxSerialDMA(2);                                        // no interrupt enabled so check RX DMA operation {38}
            }
	    }
    }
    else if (iPort == 1) {
	    while (usLen--) {
            UIMR_UISR_1 |= (UART_RXRDY_MASK);
		    UTB_RB_1 = *ptrDebugIn++;
            if (ucEnabledState[1] & UART_RXRDY_MASK) {
                __VECTOR_RAM[UART1_VECTOR]();
            }
            else {
                fnRxSerialDMA(1);                                        // no interrupt enabled so check RX DMA operation {38}
            }
	    }
    }
    else if (iPort == 0) {
	    while (usLen--) {
		    UTB_RB_0 = *ptrDebugIn++;
            if (ucEnabledState[0] & UART_RXRDY_MASK) {
                UIMR_UISR_0 |= (UART_RXRDY_MASK);                        // set the interrupt status bit
                __VECTOR_RAM[UART0_VECTOR]();                            // call the interrupt routine
            }
            else {
                fnRxSerialDMA(0);                                        // no interrupt enabled so check RX DMA operation {38}
            }
	    }
    }
 #endif
}

static int fnHandleModemInput(int iPort, unsigned char ucBit, unsigned char ucState, int iAction) // {54}
{
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // temp
    if (iAction) {                                                       // convert port to UART
        switch (iPort) {
        case _PORT_UC:
            iPort = 2;                                                   // UART2 can be on this port
            break;
        case _PORT_UA:
            iPort = 0;                                                   // UART0 can be on this port
            break;
    #if defined UART1_ON_QS
        case _PORT_QS:
            iPort = 1;                                                   // UART1 can be on this port
            break;
    #else
        case _PORT_UB:
            iPort = 1;                                                   // UART1 can be on this port
            break;
    #endif
        default:
            return 0;                                                    // no UART control line on this port
        }
    }
    switch (iPort) {
    case 0:
        if ((ucBit & PORT_UA_BIT3) && ((PUAPAR & (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT))) {
            if (iAction != 0) {
                fnModemChange(0, (ucState == 0), 0);                     // UART0 CTS
            }
            return 1;
        }
        break;
    case 1:
        #if defined UART1_ON_QS
        if ((ucBit & PORT_QS_BIT3) && ((PQSPAR & (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)) == (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT))) {
            if (iAction != 0) {
                fnModemChange(1, (ucState == 0), 0);                     // UART1 CTS
            }
            return 1;
        }
        #else
        if ((ucBit & PORT_UB_BIT3) && ((PUBPAR & (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT))) {
            if (iAction != 0) {
                fnModemChange(1, (ucState == 0), 0);                     // UART1 CTS
            }
            return 1;
        }
        #endif
        break;
    case 2:
#if !defined CHIP_80_PIN && !defined CHIP_64_PIN && !defined CHIP_81_PIN // HW flow control not possible on 80 pin device
    #if defined _M5222X || defined _M5225X                               // {54}
        if ((ucBit & PORT_UC_BIT3) && ((PUCPAR & (ALT_2_FUNCTION_Q << BIT_3_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_3_Q_SHIFT)))
    #else
        if ((ucBit & PORT_UC_BIT3) && (PUCPAR & (PRIMARY_FUNCTION_D << BIT_3_D_SHIFT)))
    #endif
        {                                                                // CTS function on pin
            if (iAction != 0) {
                fnModemChange(2, (ucState == 0), 0);                     // UART2 CTS
            }
            return 1;
        }
#endif
        break;
    }
#endif
    return 0;                                                            // not a modem input
}

static void fnModemChange(int iUart, int iState, int iSetPort)
{
#if defined SERIAL_INTERFACE
    switch (iUart) {
    case 0:
        if (!fnHandleModemInput(0,0xff,0,0)) {
            return;                                                      // ignore non-modem ports
        }
        if (iState != 0) {                                               // CTS has just been activated
            if (iSetPort != 0) {
    #if defined _M520X || defined _M523X                                 // {61}
    #else
                fnSimulateInputChange(_PORT_UA, 0, CLEAR_INPUT);
    #endif
                return;
            }
            UIP_0 &= ~UART_CTS_STATE;
            UIPCR_UACR_0 &= ~(UART_CTS_IN);
            UIPCR_UACR_0 |= (UART_COS);
        }
        else {                                                           // CTS has just been deactivated
            if (iSetPort != 0) {
    #if defined _M520X || defined _M523X                                 // {61}
    #else
                fnSimulateInputChange(_PORT_UA, 0, SET_INPUT);
    #endif
                return;
            }
            UIP_0 |= UART_CTS_STATE;
            UIPCR_UACR_0 |= (UART_COS | UART_CTS_IN);
        }
        UIMR_UISR_0 |= (UART_COS_MASK);
        if (ucEnabledState[0] & UART_COS_MASK) {                         // if interrupt enabled
            __VECTOR_RAM[UART0_VECTOR]();                                // call the interrupt routine
        }
        break;
    case 1:
        if (!fnHandleModemInput(1,0xff,0,0)) {
            return;                                                      // ignore non-modem ports
        }
        if (iState != 0) {                                               // CTS has just been activated
            if (iSetPort != 0) {
                #if defined _M52XX_SDRAM
                #elif defined _M520X || defined _M523X                   // {61}
                #else
                    #if defined UART1_ON_QS
                fnSimulateInputChange(_PORT_QS, 4, CLEAR_INPUT);
                    #else
                fnSimulateInputChange(_PORT_UB, 0, CLEAR_INPUT);
                    #endif
                #endif
                return;
            }
            UIP_1 &= ~UART_CTS_STATE;
            UIPCR_UACR_1 &= ~(UART_CTS_IN);
            UIPCR_UACR_1 |= (UART_COS);
        }
        else {                                                           // CTS has just been deactivated
            if (iSetPort != 0) {
                #if defined _M52XX_SDRAM
                #elif defined _M520X || defined _M523X                   // {61}
                #else
                    #if defined UART1_ON_QS
                fnSimulateInputChange(_PORT_QS, 4, SET_INPUT);
                    #else
                fnSimulateInputChange(_PORT_UB, 0, SET_INPUT);
                    #endif
                #endif
                return;
            }
            UIP_1 |= UART_CTS_STATE;
            UIPCR_UACR_1 |= (UART_COS | UART_CTS_IN);
        }
        UIMR_UISR_1 |= (UART_COS_MASK);
        if (ucEnabledState[1] & UART_COS_MASK) {                         // if interrupt enabled
            __VECTOR_RAM[UART1_VECTOR]();                                // call the interrupt routine
        }
        break;
    case 2:
        if (!fnHandleModemInput(2, 0xff, 0, 0)) {
            return;                                                      // ignore non-modem ports
        }
        if (iState != 0) {                                               // CTS has just been activated
            if (iSetPort != 0) {
                #if defined _M52XX_SDRAM
                #elif defined _M520X || defined _M523X                   // {61}
                #else
                fnSimulateInputChange(_PORT_UC, 0, CLEAR_INPUT);
                #endif
                return;
            }
            UIP_2 &= ~UART_CTS_STATE;
            UIPCR_UACR_2 &= ~(UART_CTS_IN);
            UIPCR_UACR_2 |= (UART_COS);
        }
        else {                                                           // CTS has just been deactivated
            if (iSetPort != 0) {
                #if defined _M52XX_SDRAM
                #elif defined _M520X || defined _M523X                   // {61}
                #else
                fnSimulateInputChange(_PORT_UC, 0, SET_INPUT);
                #endif
                return;
            }
            UIP_2 |= UART_CTS_STATE;
            UIPCR_UACR_2 |= (UART_COS | UART_CTS_IN);
        }
        UIMR_UISR_2 |= (UART_COS_MASK);
        if (ucEnabledState[2] & UART_COS_MASK) {                         // if interrupt enabled
            __VECTOR_RAM[UART2_VECTOR]();                                // call the interrupt routine
        }
        break;
    }
#endif
}

extern void fnSimulateModemChange(int iPort, unsigned long ulNewState, unsigned long ulOldState) // {14}
{
// Note that the modem status bits are according to the MS specifications as passed by GetCommModemStatus().
// To avoid including MS headers, the bits are defined here - it is not expected that they will ever change...
#define MS_CTS_ON  0x0010
#define MS_DSR_ON  0x0020
#define MS_RING_ON 0x0040
#define MS_RLSD_ON 0x0080                                                // carrier detect

#if defined SUPPORT_HW_FLOW
    unsigned long ulChange = (ulNewState ^ ulOldState);

    if (ulChange & MS_CTS_ON) {                                          // CTS change - set the state in the corresponding register and generate interrupt, if enabled
        fnModemChange(iPort, ((ulNewState & MS_CTS_ON) != 0), 1);
    }
#endif
}

// UART Break detection simulation                                       {37}
//
extern void fnSimulateBreak(int iPort)
{
#if defined SERIAL_INTERFACE
    switch (iPort) {
    case 0:
        USR_UCSR_0 |= UART_RB;                                           // set break flags
        UIMR_UISR_0 |= (UART_BD_MASK);
        if (ucEnabledState[0] & UART_BD_MASK) {                          // if break interrupt enabled
            __VECTOR_RAM[UART0_VECTOR]();
        }
        break;
    case 1:
        USR_UCSR_1 |= UART_RB;                                           // set break flags
        UIMR_UISR_1 |= (UART_BD_MASK);
        if (ucEnabledState[1] & UART_BD_MASK) {                          // if break interrupt enabled
            __VECTOR_RAM[UART1_VECTOR]();
        }
        break;
    case 2:
        USR_UCSR_2 |= UART_RB;                                           // set break flags
        UIMR_UISR_2 |= (UART_BD_MASK);
        if (ucEnabledState[2] & UART_BD_MASK) {                          // if break interrupt enabled
            __VECTOR_RAM[UART2_VECTOR]();
        }
        break;
    }
#endif
}


#if defined SUPPORT_KEY_SCAN

#if KEY_COLUMNS == 0                                                     // {57}
    #define _NON_MATRIX
    #undef KEY_COLUMNS
    #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
    #undef KEY_ROWS
    #define KEY_ROWS VIRTUAL_KEY_ROWS
#endif

static unsigned char fnMapPortBit(unsigned char ucRealBit)
{
    unsigned char ucBit = 0x80;
    unsigned char ucRef = 0;

    while (ucBit) {
        if (ucRealBit & ucBit) {
            break;
        }
        ucBit >>= 1;
        ucRef++;
    }
    return ucRef;
}


int iKeyPadInputs[KEY_COLUMNS][KEY_ROWS] = {0};

extern void fnSimulateKeyChange(int *intTable)
{
#if defined _NON_MATRIX                                                  // {57}
    int iCol, iRow;
    int iChange;
    for (iCol = 0; iCol < KEY_COLUMNS; iCol++) {
        for (iRow = 0; iRow < KEY_ROWS; iRow++) {
            iChange = iKeyPadInputs[iCol][iRow];                         // original value
            iKeyPadInputs[iCol][iRow] = *intTable++;                     // new value
            if (iChange != iKeyPadInputs[iCol][iRow]) {
    #if defined KEY_POLARITY_POSITIVE
                if (iChange)
    #else
                if (iChange)                                             // generally a key press is a '0' 
    #endif
                {
                    iChange = SET_INPUT;
                }
                else {
                    iChange = CLEAR_INPUT;
                }
                switch ((iCol * KEY_ROWS) + iRow) {                      // the specific input
    #if defined KEY_1_PORT_REF
                case 0:                                                  // first key
                    fnSimulateInputChange(KEY_1_PORT_REF, fnMapPortBit(KEY_1_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_2_PORT_REF
                case 1:                                                  // second key
                    fnSimulateInputChange(KEY_2_PORT_REF, fnMapPortBit(KEY_2_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_3_PORT_REF
                case 2:                                                  // third key
                    fnSimulateInputChange(KEY_3_PORT_REF, fnMapPortBit(KEY_3_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_4_PORT_REF
                case 3:                                                  // fourth key
                    fnSimulateInputChange(KEY_4_PORT_REF, fnMapPortBit(KEY_4_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_5_PORT_REF
                case 4:                                                  // fifth key
                    fnSimulateInputChange(KEY_5_PORT_REF, fnMapPortBit(KEY_5_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_6_PORT_REF
                case 5:                                                  // sixth key
                    fnSimulateInputChange(KEY_6_PORT_REF, fnMapPortBit(KEY_6_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_7_PORT_REF
                case 6:                                                  // seventh key
                    fnSimulateInputChange(KEY_7_PORT_REF, fnMapPortBit(KEY_7_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_8_PORT_REF
                case 7:                                                  // eighth key
                    fnSimulateInputChange(KEY_8_PORT_REF, fnMapPortBit(KEY_8_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_9_PORT_REF
                case 8:                                                  // ninth key
                    fnSimulateInputChange(KEY_9_PORT_REF, fnMapPortBit(KEY_9_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_10_PORT_REF
                case 9:                                                  // tenth key
                    fnSimulateInputChange(KEY_10_PORT_REF, fnMapPortBit(KEY_10_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_11_PORT_REF
                case 10:                                                 // eleventh key
                    fnSimulateInputChange(KEY_11_PORT_REF, fnMapPortBit(KEY_11_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_12_PORT_REF
                case 11:                                                 // twelf key
                    fnSimulateInputChange(KEY_12_PORT_REF, fnMapPortBit(KEY_12_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_13_PORT_REF
                case 12:                                                 // thirteenth key
                    fnSimulateInputChange(KEY_13_PORT_REF, fnMapPortBit(KEY_13_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_14_PORT_REF
                case 13:                                                 // fourteenth key
                    fnSimulateInputChange(KEY_14_PORT_REF, fnMapPortBit(KEY_14_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_15_PORT_REF
                case 14:                                                 // fifteenth key
                    fnSimulateInputChange(KEY_15_PORT_REF, fnMapPortBit(KEY_15_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_16_PORT_REF
                case 15:                                                 // sixteenth key
                    fnSimulateInputChange(KEY_16_PORT_REF, fnMapPortBit(KEY_16_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_17_PORT_REF
                case 16:                                                 // seventeenth key
                    fnSimulateInputChange(KEY_17_PORT_REF, fnMapPortBit(KEY_17_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_18_PORT_REF
                case 17:                                                 // eighteenth key
                    fnSimulateInputChange(KEY_18_PORT_REF, fnMapPortBit(KEY_18_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_19_PORT_REF
                case 18:                                                 // nineteenth key
                    fnSimulateInputChange(KEY_19_PORT_REF, fnMapPortBit(KEY_19_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_20_PORT_REF
                case 19:                                                 // twentieth key
                    fnSimulateInputChange(KEY_20_PORT_REF, fnMapPortBit(KEY_20_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_21_PORT_REF
                case 20:                                                 // twenty first key
                    fnSimulateInputChange(KEY_21_PORT_REF, fnMapPortBit(KEY_21_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_22_PORT_REF
                case 21:                                                 // twenty second key
                    fnSimulateInputChange(KEY_22_PORT_REF, fnMapPortBit(KEY_22_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_23_PORT_REF
                case 22:                                                 // twenty third key
                    fnSimulateInputChange(KEY_23_PORT_REF, fnMapPortBit(KEY_23_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_24_PORT_REF
                case 23:                                                 // twenty fourth key
                    fnSimulateInputChange(KEY_24_PORT_REF, fnMapPortBit(KEY_24_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_25_PORT_REF
                case 24:                                                 // twenty fifth key
                    fnSimulateInputChange(KEY_25_PORT_REF, fnMapPortBit(KEY_25_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_26_PORT_REF
                case 25:                                                 // twenty sixth key
                    fnSimulateInputChange(KEY_26_PORT_REF, fnMapPortBit(KEY_26_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_27_PORT_REF
                case 26:                                                  // twenty seventh key
                    fnSimulateInputChange(KEY_27_PORT_REF, fnMapPortBit(KEY_27_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_28_PORT_REF
                case 27:                                                  // twenty eighth key
                    fnSimulateInputChange(KEY_28_PORT_REF, fnMapPortBit(KEY_28_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_29_PORT_REF
                case 28:                                                  // twenty ninth key
                    fnSimulateInputChange(KEY_29_PORT_REF, fnMapPortBit(KEY_29_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_30_PORT_REF
                case 29:                                                  // thirtieth key
                    fnSimulateInputChange(KEY_30_PORT_REF, fnMapPortBit(KEY_30_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_31_PORT_REF
                case 30:                                                  // thirty first key
                    fnSimulateInputChange(KEY_31_PORT_REF, fnMapPortBit(KEY_31_PORT_PIN), iChange);
                    break;
    #endif
    #if defined KEY_32_PORT_REF
                case 31:                                                  // thirty second key
                    fnSimulateInputChange(KEY_32_PORT_REF, fnMapPortBit(KEY_32_PORT_PIN), iChange);
                    break;
    #endif
                }
            }
        }
    }
#else
    memcpy(iKeyPadInputs, intTable, sizeof(iKeyPadInputs));              // copy key pad state to local set
#endif
}

static int fnColumnLow(int iColumnOutput)
{
#if defined KEY_COLUMNS && !defined _NON_MATRIX                           // {37}
    switch (iColumnOutput) {
    case 0:
        return (KEY_COL_OUT_1 & ~KEY_COL_OUT_PORT_1 & KEY_COL_OUT_DDR_1);
    #if KEY_COLUMNS > 1
    case 1:
        return (KEY_COL_OUT_2 & ~KEY_COL_OUT_PORT_2 & KEY_COL_OUT_DDR_2);
    #endif
    #if KEY_COLUMNS > 2
    case 2:
        return (KEY_COL_OUT_3 & ~KEY_COL_OUT_PORT_3 & KEY_COL_OUT_DDR_3);
    #endif
    #if KEY_COLUMNS > 3
    case 3:
        return (KEY_COL_OUT_4 & ~KEY_COL_OUT_PORT_4 & KEY_COL_OUT_DDR_4);
    #endif
    #if KEY_COLUMNS > 4
    case 4:
        return (KEY_COL_OUT_5 & ~KEY_COL_OUT_PORT_5 & KEY_COL_OUT_DDR_5);
    #endif
    #if KEY_COLUMNS > 5
    case 5:
        return (KEY_COL_OUT_6 & ~KEY_COL_OUT_PORT_6 & KEY_COL_OUT_DDR_6);
    #endif
    #if KEY_COLUMNS > 6
    case 6:
        return (KEY_COL_OUT_7 & ~KEY_COL_OUT_PORT_7 & KEY_COL_OUT_DDR_7);
    #endif
    #if KEY_COLUMNS > 7
    case 7:
        return (KEY_COL_OUT_8 & ~KEY_COL_OUT_PORT_8 & KEY_COL_OUT_DDR_8);
    #endif
    }
#endif
    return 0;
}

static void fnSetRowInput(int iRowInput, int iState)
{
    int iChange;

    if (iState) {
        iChange = CLEAR_INPUT;
    }
    else {
        iChange = SET_INPUT;
    }
#if !defined _NON_MATRIX                                                 // {57}
    switch (iRowInput) {
    case 0:
    #if defined KEY_ROWS
        fnSimulateInputChange(KEY_ROW_IN_PORT_1_REF, fnMapPortBit(KEY_ROW_IN_1), iChange);
    #endif
        break;
    case 1:
    #if KEY_ROWS > 1
        fnSimulateInputChange(KEY_ROW_IN_PORT_2_REF, fnMapPortBit(KEY_ROW_IN_2), iChange);
    #endif
        break;
    case 2:
    #if KEY_ROWS > 2
        fnSimulateInputChange(KEY_ROW_IN_PORT_3_REF, fnMapPortBit(KEY_ROW_IN_3), iChange);
    #endif
        break;
    case 3:
    #if KEY_ROWS > 3
        fnSimulateInputChange(KEY_ROW_IN_PORT_4_REF, fnMapPortBit(KEY_ROW_IN_4), iChange);
    #endif
        break;
    case 4:
    #if KEY_ROWS > 4
        fnSimulateInputChange(KEY_ROW_IN_PORT_5_REF, fnMapPortBit(KEY_ROW_IN_5), iChange);
    #endif
        break;
    case 5:
    #if KEY_ROWS > 5
        fnSimulateInputChange(KEY_ROW_IN_PORT_6_REF, fnMapPortBit(KEY_ROW_IN_6), iChange);
    #endif
        break;
    case 6:
    #if KEY_ROWS > 6
        fnSimulateInputChange(KEY_ROW_IN_PORT_7_REF, fnMapPortBit(KEY_ROW_IN_7), iChange);
    #endif
        break;
    case 7:
    #if KEY_ROWS > 7
        fnSimulateInputChange(KEY_ROW_IN_PORT_8_REF, fnMapPortBit(KEY_ROW_IN_8), iChange);
    #endif
        break;
    }
#endif
}

// This routine updates the ports to respect the present setting of a connected matrix key pad
//
extern void fnSimMatrixKB(void)
{
    int i, j;

    iFlagRefresh = fnPortChanges(1);                                     // {50} synchronise with present settings
    // Check whether a column control is being driven low. If this is the case, any pressed ones in the column are seen at the row input
    for (i = 0; i < KEY_COLUMNS; i++) {
        if (fnColumnLow(i)) {
            for (j = 0; j < KEY_ROWS; j++) {
                fnSetRowInput(j, iKeyPadInputs[i][j]);
            }
        }
    }
}
#endif

#if defined SUPPORT_ADC && !defined _M52XX_SDRAM && !defined _M520X      // {59}{61}
static void fnSimADC(int iChannel)                                       // {29}
{
    unsigned short usPreviousValue = *(ADC_ADRSLT_ADD + iChannel);       // previous signed value
    unsigned short usValueWithoutOffset = (usADC_values[iChannel] << 3);
    signed short sResult, usPreviousResult;
    usPreviousResult = (signed short)usPreviousValue;                    // signed value

    sResult = (signed short)(usValueWithoutOffset);                      // positive value
    sResult -= (signed short)*(ADC_ADOFS_ADD + iChannel);                // minus offset
    *(ADC_ADRSLT_ADD + iChannel) = (unsigned short)sResult;              // assigned signed value (with offset)
    ADC_ADSTAT |= (0x01 << iChannel);

    if ((usPreviousResult < 0) && (sResult >= 0)) {                      // zero crossing positive
        if (ADC_ADZCC & (ZERO_CROSS_POS << (iChannel * 2))) {            // if positive crossing enabled
            ADC_ADZCSTAT |= (ZCS_CHANNEL0 << iChannel);                  // set flag
            if (ADC_CTRL1 & ZCIE) {                                      // if zero crossing interrupt enabled
                if (!(IC_IMRH_0 & ADCINT_PIF_INT_H)) {
                    __VECTOR_RAM[ADC_ADCINT_VECTOR]();
                }
            }
        }
    }
    if ((usPreviousResult >= 0) && (sResult < 0)) {                      // zero crossing negative
        if (ADC_ADZCC & (ZERO_CROSS_NEG << (iChannel * 2))) {            // if negative crossing enabled
            ADC_ADZCSTAT |= (ZCS_CHANNEL0 << iChannel);                  // set flag
            if (ADC_CTRL1 & ZCIE) {                                      // if zero crossing interrupt enabled
                if (!(IC_IMRH_0 & ADCINT_PIF_INT_H)) {
                    __VECTOR_RAM[ADC_ADCINT_VECTOR]();
                }
            }
        }
    }

    if (usValueWithoutOffset > *(ADC_ADLLR_ADD + ADC_CHANNELS + iChannel)) { // high trigger
        ADC_ADLSTAT |= (HLS_CHANNEL0 << iChannel);
        if (ADC_CTRL1 & HLMTIE) {                                        // high level trigger interrupt enabled
            if (!(IC_IMRH_0 & ADCINT_PIF_INT_H)) {
                __VECTOR_RAM[ADC_ADCINT_VECTOR]();
            }
        }
    }
    else if (usValueWithoutOffset < *(ADC_ADLLR_ADD + iChannel)) { // low trigger
        ADC_ADLSTAT |= (LLS_CHANNEL0 << iChannel);
        if (ADC_CTRL1 & LLMTIE) {                                        // high level trigger interrupt enabled
            if (!(IC_IMRH_0 & ADCINT_PIF_INT_H)) {
                __VECTOR_RAM[ADC_ADCINT_VECTOR]();
            }
        }
    }
}

// A SYNC trigger has occured - scan the ADC channels and generate a complete interrupt if enabled
//
static void fnTriggADC(int iADC_channel)
{
    int iChannel = 0;
    unsigned short usBit = 0x0001;
    while (iChannel < ADC_CHANNELS) {
        if (ADC_ADSDIS & usBit) {                                        // if channel disabled stop
            if (iChannel != 0) {
                if (iADC_channel == 0) {
                    if (ADC_CTRL1 & EOSIE0) {                            // if end of conversion interrupt enabled
                        ADC_ADSTAT |= (EOSI0);                           // set flag
                        if (!(IC_IMRH_0 & ADCA_PIF_INT_H)) {
                            __VECTOR_RAM[ADC_ADCA_VECTOR]();
                        }
                    }
                }
                else {
                    if (ADC_CTRL2 & EOSIE1) {                            // if end of conversion interrupt enabled
                        ADC_ADSTAT |= (EOSIE1);                          // set flag
                        if (!(IC_IMRH_0 & ADCB_PIF_INT_H)) {
                            __VECTOR_RAM[ADC_ADCB_VECTOR]();
                        }
                    }
                }
            }
            if (usBit < 0x0010) {                                        // if scan complete before second ADC {33}
                if (ADC_CTRL1 & ADC_PARALLEL_OPN) {                      // and parallel operation enabled
                    usBit = 0x0010;                                      // continue checking second ADC
                    iChannel = 4;
                    continue;
                }
            }
            break;
        }
        fnSimADC(iChannel);
        iChannel++;
        usBit <<= 1;
    }
}

// Capture the present input, if enabled
//
extern void fnUpdateADC(int iChannel)                                    // {32}
{
    if (iChannel < ADC_CHANNELS) {
        if (!(ADC_ADSDIS & (0x01 << iChannel))) {                        // if channel is not disabled
            fnSimADC(iChannel);                                          // update present sample
        }
    }
    else {
        iChannel = 0;
        while (iChannel < ADC_CHANNELS) {
            if (!(ADC_ADSDIS & (0x01 << iChannel))) {                    // if channel is not disabled
                fnSimADC(iChannel);                                      // update present sample
            }
            iChannel++;
        }
    }
}

#endif

#if !defined _M520X && !defined _M523X                                   // {61}{65}
// Process a GTP capture input event
//
static void fnCaptureGPT(unsigned char ucBit)
{
    switch (ucBit) {
    case 0x01:
        GPTC0 = GPTCNT;                                                  // capture present timer count value
        GPTFLG1 |= 0x01;
        if (GPTIE & ucBit) {                                             // interrupt enabled
            if (!(IC_IMRH_0 & GPTC0F_PIF_INT_H)) {                       // not masked
	            __VECTOR_RAM[GPT_C0F_VECTOR]();                          // call the capture interrupt routine
            }
        }
        break;
    case 0x02:
        GPTC1 = GPTCNT;                                                  // capture present timer count value
        GPTFLG1 |= 0x02;
        if (GPTIE & ucBit) {                                             // interrupt enabled
            if (!(IC_IMRH_0 & GPTC1F_PIF_INT_H)) {                       // not masked
	            __VECTOR_RAM[GPT_C1F_VECTOR]();                          // call the capture interrupt routine
            }
        }
        break;
    case 0x04:
        GPTC2 = GPTCNT;                                                  // capture present timer count value
        GPTFLG1 |= 0x04;
        if (GPTIE & ucBit) {                                             // interrupt enabled
            if (!(IC_IMRH_0 & GPTC2F_PIF_INT_H)) {                       // not masked
	            __VECTOR_RAM[GPT_C2F_VECTOR]();                          // call the capture interrupt routine
            }
        }
        break;
    case 0x08:
        GPTC3 = GPTCNT;                                                  // capture present timer count value
        GPTFLG1 |= 0x08;
        if (GPTIE & ucBit) {                                             // interrupt enabled
            if (!(IC_IMRH_0 & GPTC3F_PIF_INT_H)) {                       // not masked
	            __VECTOR_RAM[GPT_C3F_VECTOR]();                          // call the capture interrupt routine
            }
        }
        break;
    }
}
#endif

extern void fnSimulateInputChange(unsigned char ucPort, unsigned char ucPortBit, int iChange)
{
    unsigned char ucBit = (0x01 << (7 - ucPortBit));

    switch (ucPort) {
#if defined _M52XX_SDRAM                                                // {59}
    case _PORT_A:
        if (!(ucPortDDR_A & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_A ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_A |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_A &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETA = ucPort_in_A;
        }
        break;
    case _PORT_B:
        if (!(ucPortDDR_B & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_B ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_B |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_B &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETB = ucPort_in_B;
        }
        break;
    case _PORT_C:
        if (!(ucPortDDR_C & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_C ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_C |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_C &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETC = ucPort_in_C;
        }
        break;
    case _PORT_D:
        if (!(ucPortDDR_D & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_D ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_D |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_D &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETD = ucPort_in_D;
        }
        break;
    case _PORT_E:
        if (!(ucPortDDR_E & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_E ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_E |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_E &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETE = ucPort_in_E;
        }
        break;
    case _PORT_F:
        if (!(ucPortDDR_F & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_F ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_F |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_F &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETF = ucPort_in_F;
        }
        break;
    case _PORT_G:
        if (!(ucPortDDR_G & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_G ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_G |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_G &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETG = ucPort_in_G;
        }
        break;
    case _PORT_H:
        if (!(ucPortDDR_H & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_H ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_H |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_H &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETH = ucPort_in_H;
        }
        break;
    case _PORT_J:
        if (!(ucPortDDR_J & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_J ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_J |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_J &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETJ = ucPort_in_J;
        }
        break;
    case _PORT_EH:
        if (!(ucPortDDR_EH & ucBit)) {                                  // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_EH ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_EH |= ucBit;                                  // set the input high
            }
            else  {
                ucPort_in_EH &= ~ucBit;                                 // set the input low
            }
            PORTIN_SETEH = ucPort_in_EH;
        }
        break;
    case _PORT_EL:
        if (!(ucPortDDR_EL & ucBit)) {                                  // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_EL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_EL |= ucBit;                                  // set the input high
            }
            else  {
                ucPort_in_EL &= ~ucBit;                                 // set the input low
            }
            PORTIN_SETEL = ucPort_in_EL;
        }
        break;
    case _PORT_QA:
        if (!(ucPortDDR_QA & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_QA ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_QA |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_QA &= ~ucBit;                                  // set the input low
            }
            PORTQA = ucPort_in_QA;
        }
        break;
    case _PORT_QB:
        if (!(ucPortDDR_QB & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_QB ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_QB |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_QB &= ~ucBit;                                  // set the input low
            }
            PORTQA = ucPort_in_QB;
        }
        break;
    case _PORT_TA:
        ucBit >>= 4;
        if (!(ucPortDDR_TA & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TA ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TA |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TA &= ~ucBit;                                  // set the input low
            }
            GPTPORT = ucPort_in_TA;
        }
        break;
    case _PORT_TB:
        ucBit >>= 4;
        if (!(ucPortDDR_TB & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TB ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TB |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TB &= ~ucBit;                                  // set the input low
            }
            GPT1PORT = ucPort_in_TB;
        }
        break;
#elif defined _M520X                                                     // {61}
    case _PORT_FECH:
        if (!(ucPortDDR_FECH & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_FECH ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_FECH |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_FECH &= ~ucBit;                                // set the input low
            }
            PORTFECH = ucPort_in_FECH;
        }
        break;
    case _PORT_FECL:
        if (!(ucPortDDR_FECL & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_FECL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_FECL |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_FECL &= ~ucBit;                                // set the input low
            }
            PORTFECL = ucPort_in_FECL;
        }
        break;
    case _PORT_BUSCTL:
        if (!(ucPortDDR_BUSCTL & ucBit)) {                               // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_BUSCTL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_BUSCTL |= ucBit;                               // set the input high
            }
            else  {
                ucPort_in_BUSCTL &= ~ucBit;                              // set the input low
            }
            PORTBUSCTL = ucPort_in_BUSCTL;
        }
        break;
    case _PORT_BE:
        if (!(ucPortDDR_BE & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_BE ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_BE |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_BE &= ~ucBit;                                  // set the input low
            }
            PORTBE = ucPort_in_BE;
        }
        break;
    case _PORT_CS:
        if (!(ucPortDDR_CS & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_CS ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_CS |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_CS &= ~ucBit;                                  // set the input low
            }
            PORTCS = ucPort_in_CS;
        }
        break;
    case _PORT_FECI2C:
        if (!(ucPortDDR_FECI2C & ucBit)) {                               // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_FECI2C ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_FECI2C |= ucBit;                               // set the input high
            }
            else  {
                ucPort_in_FECI2C &= ~ucBit;                              // set the input low
            }
            PORTFECI2C = ucPort_in_FECI2C;
        }
        break;
    case _PORT_UART:
        if (!(ucPortDDR_UART & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_UART ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_UART |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_UART &= ~ucBit;                                // set the input low
            }
            PORTUART = ucPort_in_UART;
        }
        break;
    case _PORT_QSPI:
        if (!(ucPortDDR_QSPI & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_QSPI ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_QSPI |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_QSPI &= ~ucBit;                                // set the input low
            }
            PORTQSPI = ucPort_in_QSPI;
        }
        break;
    case _PORT_TIMER:
        if (!(ucPortDDR_TIMER & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TIMER ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TIMER |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_TIMER &= ~ucBit;                               // set the input low
            }
            PORTTIMER = ucPort_in_TIMER;
        }
        break;
    case _PORT_IRQ:
        if (!(ucPortDDR_IRQ & ucBit)) {                                  // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_IRQ ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_IRQ |= ucBit;                                  // set the input high
            }
            else  {
                ucPort_in_IRQ &= ~ucBit;                                 // set the input low
            }
          //PORTIRQ = ucPort_in_IRQ;
            fnPortChanges(1);                                            // allow IRQs to be serviced
        }
        break;
#elif defined _M523X                                                     // {65}
    case _PORT_ADDR:
        if (!(ucPortDDR_ADDR & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_ADDR ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_ADDR |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_ADDR &= ~ucBit;                                // set the input low
            }
            PORTADDR = ucPort_in_ADDR;
        }
        break;
    case _PORT_BS:
        if (!(ucPortDDR_BS & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_BS ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_BS |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_BS &= ~ucBit;                                  // set the input low
            }
            PORTBS = ucPort_in_BS;
        }
        break;
    case _PORT_BUSCTL:
        if (!(ucPortDDR_BUSCTL & ucBit)) {                               // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_BUSCTL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_BUSCTL |= ucBit;                              // set the input high
            }
            else  {
                ucPort_in_BUSCTL &= ~ucBit;                              // set the input low
            }
            PORTBUSCTL = ucPort_in_BUSCTL;
        }
        break;
    case _PORT_CS:
        if (!(ucPortDDR_CS & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_CS ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_CS |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_CS &= ~ucBit;                                  // set the input low
            }
            PORTCS = ucPort_in_CS;
        }
        break;
    case _PORT_DATAH:
        if (!(ucPortDDR_DATAH & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_DATAH ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_DATAH |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_DATAH &= ~ucBit;                               // set the input low
            }
            PORTDATAH = ucPort_in_DATAH;
        }
        break;
    case _PORT_DATAL:
        if (!(ucPortDDR_DATAL & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_DATAL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_DATAL |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_DATAL &= ~ucBit;                               // set the input low
            }
            PORTDATAL = ucPort_in_DATAL;
        }
        break;
    case _PORT_FECI2C:
        if (!(ucPortDDR_FECI2C & ucBit)) {                               // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_FECI2C ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_FECI2C |= ucBit;                               // set the input high
            }
            else  {
                ucPort_in_FECI2C &= ~ucBit;                              // set the input low
            }
            PORTFECI2C = ucPort_in_FECI2C;
        }
        break;
    case _PORT_ETPU:
        if (!(ucPortDDR_ETPU & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_ETPU ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_ETPU |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_ETPU &= ~ucBit;                                // set the input low
            }
            PORTETPU = ucPort_in_ETPU;
        }
        break;
    case _PORT_QSPI:
        if (!(ucPortDDR_QSPI & ucBit)) {                                 // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_QSPI ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_QSPI |= ucBit;                                 // set the input high
            }
            else  {
                ucPort_in_QSPI &= ~ucBit;                                // set the input low
            }
            PORTQSPI = ucPort_in_QSPI;
        }
        break;
    case _PORT_SDRAM:
        if (!(ucPortDDR_SDRAM & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_SDRAM ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_SDRAM |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_SDRAM &= ~ucBit;                               // set the input low
            }
            PORTSDRAM = ucPort_in_SDRAM;
        }
        break;
    case _PORT_TIMER:
        if (!(ucPortDDR_TIMER & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TIMER ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TIMER |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_TIMER &= ~ucBit;                               // set the input low
            }
            PORTTIMER = ucPort_in_TIMER;
        }
        break;
    case _PORT_UARTH:
        if (!(ucPortDDR_UARTH & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_UARTH ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_UARTH |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_UARTH &= ~ucBit;                               // set the input low
            }
            PORTUARTH = ucPort_in_UARTH;
        }
        break;
    case _PORT_UARTL:
        if (!(ucPortDDR_UARTL & ucBit)) {                                // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_UARTL ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_UARTL |= ucBit;                                // set the input high
            }
            else  {
                ucPort_in_UARTL &= ~ucBit;                               // set the input low
            }
            PORTUARTL = ucPort_in_UARTL;
        }
        break;
    case _PORT_IRQ:                                                      // only IRQ function
        if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
            ucPort_in_IRQ ^= ucBit;
        }
        else if (iChange == SET_INPUT) {
            ucPort_in_IRQ |= ucBit;                                      // set the input high
        }
        else  {
            ucPort_in_IRQ &= ~ucBit;                                     // set the input low
        }
        fnPortChanges(1);                                                // allow IRQs to be serviced
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_QS:
        if (!(ucPortDDR_QS & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_QS ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_QS |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_QS &= ~ucBit;                                  // set the input low
            }
            fnHandleModemInput(_PORT_QS, ucBit, (unsigned char)(ucPort_in_QS & ucBit), 1);// {54} potential MODEM inputs
            PORTIN_SETQS = ucPort_in_QS;
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // temp
    case _PORT_AN:
        if (!(ucPortDDR_AN & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {           // {29}
    #if defined SUPPORT_ADC                                              // {29}
                if (PANPAR & ucBit) {                                    // input is ADC so change the voltage
                    int iAdc = 0;
                    unsigned short usStepSize = ((ADC_SIM_STEP_SIZE * 0xfff) / ADC_REFERENCE_VOLTAGE);
                    do {
                        iAdc++,
                        ucBit >>= 1;
                    } while (ucBit);
                    if (TOGGLE_INPUT_NEG & iChange) {
                        if (usADC_values[iAdc-1] >= usStepSize) {
                            usADC_values[iAdc-1] -= usStepSize;
                            fnSimADC(iAdc-1);
                        }
                    }
                    else {
                        if ((usADC_values[iAdc-1] + usStepSize) <= 0xfff) {
                            usADC_values[iAdc-1] += usStepSize;
                            fnSimADC(iAdc-1);
                        }
                    }
                    break;
                }
    #endif
                ucPort_in_AN ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_AN |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_AN &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETAN = ucPort_in_AN;
        }
        break;
#endif

#if !defined _M521X && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {39}{59}{61}
    case _PORT_LD:
        if (!(ucPortDDR_LD & ucBit)) {                                   // if input
            ucPort_in_LD ^= ucBit;
            PORTIN_SETLD = ucPort_in_LD;
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_NQ:
    #if defined _M52XX_SDRAM
        if (!(EPDDR0 & (ucBit & 0xfe))) {                                // if not configured as output
            EPPDR0 = (ucPort_in_NQ ^ ucBit);
            fnPortChanges(1);                                            // allow IRQs to be serviced
        }
    #else
        if ((!(ucPortDDR_NQ & ucBit)) || (ucIRQ_P0 & ucBit)) {           // {69} if input or peripheral functionality
            _irqPORTIN_SETNQ = ucPort_in_NQ ^ (ucBit & 0xfe);
            EPPDR0 = ((ucPort_in_NQ ^ (ucBit & 0xfe)) & ucIRQ0);         // {55}
            PORTIN_SETNQ = (_irqPORTIN_SETNQ | ucIRQ_P0);                // {69} synchronise the port input value to the pin input value but ensure that any IRQ functions are kept at '1'
            fnPortChanges(1);                                            // allow IRQs to be serviced {5}{50}
        }
    #endif
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {61}
    case _PORT_TA:
        {
            unsigned char ucMask;
            ucBit >>= 4;
            switch (ucBit) {
            case 0x01:
                ucMask = 0x03;
                break;
            case 0x02:
                ucMask = 0x0c;
                break;
            case 0x04:
                ucMask = 0x30;
                break;
            case 0x08:
                ucMask = 0xc0;
                break;
            }
            if ((GPTSCR1 & GPTEN) && (GPTCTL2 & ucMask)) {               // {42} channel is being used for capture as long as DDR is set as input
                if (!(GPTDDR & ucBit)) {
                    ucPort_in_TA ^= ucBit;
                    PORTIN_SETTA = ucPort_in_TA;
                    if ((ucPort_in_TA & ucBit) && (GPTCTL2 & ucMask & 0x55)) { // rising edge recognised
                        fnCaptureGPT(ucBit);                             // process a capture event
                    }
                    else if ((!(ucPort_in_TA & ucBit)) && (GPTCTL2 & ucMask & 0xaa)) {
                        fnCaptureGPT(ucBit);                             // process a capture event
                    }
                    if (ucPort_in_TA & ucBit) {                          // {72} handle timer's own input data register
                        GPTPORT |= ucBit;          
                    }
                    else {
                        GPTPORT &= ~ucBit;
                    }
                    break;
                }
            }
            if (!(ucPortDDR_TA & ucBit)) {                               // if input
                ucPort_in_TA ^= ucBit;
                PORTIN_SETTA = ucPort_in_TA;
            }
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_TC:
        ucBit >>= 4;
        if (((PTCPAR >> ((3 - ucPortBit) * 2)) & (ALT_2_FUNCTION_Q)) == PRIMARY_FUNCTION_Q) { // {70} if the pin is a DMA timer input function
            DMA_TIMER *dma_timer = (DMA_TIMER *)DMA_TIMER_0_ADD;
            dma_timer += (3 - ucPortBit);
            if (dma_timer->usDTMR & CAPTURE_RISING) {                    // capture on rising edge
                if (!(ucPort_in_TC & ucBit)) {
                    dma_timer->vucDTER |= DMA_TIM_EVENT_CAP;
                }
            }
            if (dma_timer->usDTMR & CAPTURE_FALLING) {                   // capture on falling edge
                if (ucPort_in_TC & ucBit) {
                    dma_timer->vucDTER |= DMA_TIM_EVENT_CAP;
                }
            }
            if (dma_timer->vucDTER & DMA_TIM_EVENT_CAP) {                // capture event occurred
                dma_timer->vulDTCR = dma_timer->vulDTCN;                 // set the count value to the capture register
                if (dma_timer->ucDTXMR & DMA_TIMER_DMAEN) {              // timer programmed to perform DMA rather than interrupt
                }
                else {
#if defined _M520X
                    if ((!((DTIM0_PIF_INT_H << (3 - ucPortBit)) & IC_IMRH_0)) && (!(MASK_ALL_INT & IC_IMRL_0))) 
#else
                    if (!(((DTIM0_PIF_INT_L << (3 - ucPortBit)) | MASK_ALL_INT) & IC_IMRL_0)) 
#endif
                    {
                        __VECTOR_RAM[DTIM0_VECTOR + (3 - ucPortBit)]();  // call interrupt routine
                    }
                }

            }
        }
        if (!(ucPortDDR_TC & ucBit)) {                                   // if input
            ucPort_in_TC ^= ucBit;
            PORTIN_SETTC = ucPort_in_TC;
        }
        break;
#endif
#if !defined _M5225X && !defined _M520X && !defined _M523X               // {39}{61}
    case _PORT_TD:
        ucBit >>= 4;
        if (!(ucPortDDR_TD & ucBit)) {                                   // if input
            ucPort_in_TD ^= ucBit;
            PORTIN_SETTD = ucPort_in_TD;
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_UA:
        ucBit >>= 4;
        if (!(ucPortDDR_UA & ucBit)) {                                   // if input
            ucPort_in_UA ^= ucBit;
            PORTIN_SETUA = ucPort_in_UA;
            fnHandleModemInput(_PORT_UA, ucBit, (unsigned char)(ucPort_in_UA & ucBit), 1);// {54} potential MODEM inputs
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_UB:
        ucBit >>= 4;
        if (!(ucPortDDR_UB & ucBit)) {                                   // if input
            ucPort_in_UB ^= ucBit;
            fnHandleModemInput(_PORT_UB, ucBit, (unsigned char)(ucPort_in_UB & ucBit), 1);// {54} potential MODEM inputs
   #if defined SYNCB_ON_PUB
            if ((ucBit & PORT_UC_BIT2) && (ucPort_in_UB & PORT_UC_BIT2)) { // PUB2 rising edge
                if ((PUBPAR & (ALT_2_FUNCTION_Q << BIT_2_Q_SHIFT)) == (PRIMARY_FUNCTION_Q << BIT_2_Q_SHIFT)) {
                    if (ADC_CTRL2 & SYNC1) {                             // pin is configured for SYNCB function and scan enabled in ADC
                        fnTriggADC(1);
                    }
                }
            }
    #endif
            PORTIN_SETUB = ucPort_in_UB;
        }
        break;

    case _PORT_UC:
        ucBit >>= 4;
        if (!(ucPortDDR_UC & ucBit)) {                                   // if input
            ucPort_in_UC ^= ucBit;
            PORTIN_SETUC = ucPort_in_UC;
            fnHandleModemInput(_PORT_UC, ucBit, (unsigned char)(ucPort_in_UC & ucBit), 1);// {54} potential MODEM inputs
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_AS:
    #ifndef _M52XX_SDRAM                                                 // {59}
        ucBit >>= 4;
    #endif
        if (!(ucPortDDR_AS & ucBit)) {                                   // if input
            ucPort_in_AS ^= ucBit;
            PORTIN_SETAS = ucPort_in_AS;
        }
        break;
#endif
#if !defined _M5221X && !defined _M5222X && !defined _M521XX && !defined _M5225X && !defined _M521X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {39}{59}{61}
    case _PORT_GP:
        if (!(ucPortDDR_GP & ucBit)) {                                   // if input
            PORTIN_SETGP = ucPort_in_GP ^ ucBit;
            EPPDR1 = ((ucPort_in_GP ^ ucBit) & ucIRQ1);                  // {55}
            fnPortChanges(1);                                            // allow IRQs to be serviced {5}{50}
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_DD:
        if (!(ucPortDDR_DD & ucBit)) {                                   // if input
            ucPort_in_DD ^= ucBit;
            PORTIN_SETDD = ucPort_in_DD;
        }
        break;
#endif
#if defined _M5225X                                                      // {39}
    case _PORT_TE:
        if (!(ucPortDDR_TE & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TE ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TE |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TE &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETTE = ucPort_in_TE;
        }
        break;
    case _PORT_TF:
        if (!(ucPortDDR_TF & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TF ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TF |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TF &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETTF = ucPort_in_TF;
        }
        break;
    case _PORT_TG:
        if (!(ucPortDDR_TG & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TG ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TG |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TG &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETTG = ucPort_in_TG;
        }
        break;  
    case _PORT_TH:
        if (!(ucPortDDR_TH & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TH ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TH |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TH &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETTH = ucPort_in_TH;
        }
        break;
    case _PORT_TI:
        if (!(ucPortDDR_TI & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TI ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TI |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TI &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETTI = ucPort_in_TI;
        }
        break;
    case _PORT_TJ:
        if (!(ucPortDDR_TJ & ucBit)) {                                   // if input
            if (iChange & (TOGGLE_INPUT | TOGGLE_INPUT_NEG)) {
                ucPort_in_TJ ^= ucBit;
            }
            else if (iChange == SET_INPUT) {
                ucPort_in_TJ |= ucBit;                                   // set the input high
            }
            else  {
                ucPort_in_TJ &= ~ucBit;                                  // set the input low
            }
            PORTIN_SETTJ = ucPort_in_TJ;
        }
        break; 
#endif
    }
    iFlagRefresh = PORT_CHANGE;                                          // {71}
}

#if defined ETH_INTERFACE
    extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen);
#endif

extern int fnSimulateEthernetIn(unsigned char *ucData, unsigned short usLen, int iForce) // {47}
{
#if defined ETH_INTERFACE                                                // we feed frames in promiscuous mode and filter them according to their details
    static M5223X_FEC_BD *ptrRxBD;
    static int iFirstRec = 0;
	unsigned char *ptrInput;
    unsigned short usCopyLen;
    unsigned short usFullLength;
    #if defined USE_MULTIPLE_BUFFERS
    unsigned short ucMaxRxBufLen = 256;
    #else
    unsigned short ucMaxRxBufLen = 0x600;
    #endif

    if ((iForce == 0) && (fnCheckEthernetMode(ucData, usLen) == 0)) {
        return 0;                                                        // if we are not in promiscuous mode and the MAC address is not defined to be received we ignore the frame
    }

    if (iFirstRec == 0) {
        if ((ECR & ETHER_EN) == 0) {
            return 0;                                                    // ignore if the FEC has not yet been programmed
        }

        iFirstRec = 1;                                                   // we do this only once
        ptrRxBD = (M5223X_FEC_BD *)ERDSR;                                // set to first BD
    }

    usFullLength = usLen;

    while (usLen != 0) {
        ptrInput = ptrRxBD->ptrBD_Data;
        if (usLen > ucMaxRxBufLen) {
            ptrRxBD->usBDLength = ucMaxRxBufLen;
            usCopyLen = ucMaxRxBufLen;
        }
        else {
            usCopyLen = ptrRxBD->usBDLength = usLen;
            ptrRxBD->usBDLength = usFullLength;
            ptrRxBD->usBDControl |= LAST_IN_FRAME;
        }
        usLen -= usCopyLen;
	    while (usCopyLen-- != 0) {
	        *ptrInput++ = *ucData++;                                     // put bytes in input buffer
	    }
        if (usLen == 0) {                                                // {60} last buffer
            ptrRxBD->usBDLength += 4;                                    // add dummy CRC32 - this corrects the receive count and also tests that there is adequate buffer space
            *ptrInput++ = 0x33;
            *ptrInput++ = 0x66;
            *ptrInput++ = 0xaa;
            *ptrInput   = 0x55;
        }
        ptrRxBD->usBDControl &= ~EMPTY_BUFFER;                           // mark that the buffer contrains data
        EIR |= RXF;                                                      // set receive frame interrupt event
        if ((EIMR & RXF) && (ptrRxBD->usBDControl & LAST_IN_FRAME)) {    // if interrupts are enabled, 
	        __VECTOR_RAM[FEC_RX_F_VECTOR]();                             // call the rx interrupt routine
	    }
        if (ptrRxBD->usBDControl & WRAP_BIT) {
            ptrRxBD = (M5223X_FEC_BD *)ERDSR;                            // set to first BD
        }
        else {
            ptrRxBD++;                                                   // set to next RX BD
        }
    }
    return 1;
#else
    return 0;
#endif
}

// We can update port state displays if we want
//
extern void fnSimPorts(void)
{
    static int iRTS[3] = {0};                                            // {14}

    if (QIR & QSPI_SPIFE) {                                              // if the QSPI interrupt has been enabled we assume that an interrupt will be required each time we arrive here
        QIR |= QSPI_SPIF;
        __VECTOR_RAM[QSPI_VECTOR]();
    }
#if !defined _M523X
    if (UOP1_0 & UART_SET_RESET_RTS) {                                   // RTS output asserted {14}
        iRTS[0] = 1;
        UOP1_0 = 0;
        fnConfigSimSCI(0, ASSERT_RTS_COM_0, 0);                          // assert control line
        ucPort_in_UA &= ~PORT_UA_BIT2;
    }
    if (UOP0_0 & UART_SET_RESET_RTS) {                                   // RTS output negated
        iRTS[0] = 0;
        UOP0_0 = 0;
        fnConfigSimSCI(0, NEGATE_RTS_COM_0, 0);                          // negate control line
        ucPort_in_UA |= PORT_UA_BIT2;
    }
    if (UOP1_1 & UART_SET_RESET_RTS) {                                   // RTS output asserted
        iRTS[1] = 1;
        UOP1_1 = 0;
        fnConfigSimSCI(1, ASSERT_RTS_COM_1, 0);                          // assert control line {49}
        #if defined UART1_ON_QS
        ucPort_in_QS &= ~PORT_QS_BIT2;
        #else
        ucPort_in_UB &= ~PORT_UB_BIT2;
        #endif
    }
    if (UOP0_1 & UART_SET_RESET_RTS) {                                   // RTS output negated
        iRTS[1] = 0;
        UOP0_1 = 0;
        fnConfigSimSCI(1, NEGATE_RTS_COM_1, 0);                          // negate control line {49}
        #if defined UART1_ON_QS
        ucPort_in_QS |= PORT_QS_BIT2;
        #else
        ucPort_in_UB |= PORT_UB_BIT2;
        #endif
    }
#endif
#if NUMBER_SERIAL > 2                                                    // only if third UART is available
    #if defined SUPPORT_HW_FLOW && !defined CHIP_80_PIN && !defined CHIP_64_PIN && !defined CHIP_81_PIN  // HW flow control not possible on 80 pin device
    if (UOP1_2 & UART_SET_RESET_RTS) {                                   // RTS output asserted
        iRTS[2] = 1;
        UOP1_2 = 0;
        fnConfigSimSCI(2, ASSERT_RTS_COM_2, 0);                          // assert control lines {49}
        ucPort_in_UC &= ~PORT_UC_BIT2;
    }
    if (UOP0_2 & UART_SET_RESET_RTS) {                                   // RTS output negated
        iRTS[2] = 0;
        UOP0_2 = 0;
        fnConfigSimSCI(2, NEGATE_RTS_COM_2, 0);                          // negate control lines {49}
        ucPort_in_UC |= PORT_UC_BIT2;
    }
    #endif
#endif
}


extern void sw_wdog_timeout(void);

#if !defined _M520X
static void CAN_buf_Interrupt(int iBuffer)
{
    __VECTOR_RAM[CAN0_VECTOR + iBuffer]();
}
#endif

#if defined SUPPORT_RTC                                                  // {30}

#define NTP_TO_1970_TIME 2208988800u
#define LEAP_YEAR(year) ((year%4)==0)                                    // valid from 1970 to 2038
static const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

// Synchronise the internal RTC to the PC time when simulating {66}
//
extern void fnInitInternalRTC(char *argv[])
{
    #if defined _M5221X || defined _M5225X || defined _M521XX            // families with battery backup capability
    unsigned short *ptrShort = (unsigned short *)*argv;
    unsigned short RTC_YEAR, RTC_MONTH, RTC_DOW, RTC_DOM, RTC_HOUR, RTC_MIN, RTC_SEC;
    unsigned long ulColdfireTime;
    unsigned long ulLeapYears = 1970;

    RTC_YEAR = *ptrShort++;
    RTC_MONTH = *ptrShort++;
    RTC_DOW = *ptrShort++;
    RTC_DOM = *ptrShort++;
    RTC_HOUR = *ptrShort++;
    RTC_MIN = *ptrShort++;
    RTC_SEC = *ptrShort++;

    ulColdfireTime = ((RTC_YEAR - 1970) * 365);                          // years since reference time, represented in days without respecting leap years
    while (ulLeapYears < RTC_YEAR) {
        if (LEAP_YEAR(ulLeapYears)) {                                    // count leap years
            if (ulLeapYears == RTC_YEAR) {                               // presently in a leap year
                if ((RTC_MONTH > 2) || (RTC_DOM > 28)) {                 // past February 28 so count extra leap day in this year
                    ulColdfireTime++;
                }
            }
            else {
                ulColdfireTime++;                                        // count extra days in passed leap years
            }
        }
        ulLeapYears++;
    }
    while (--RTC_MONTH != 0) {
        ulColdfireTime += monthDays[RTC_MONTH];                          // add past days of previous months of this year
    }
    ulColdfireTime += (RTC_DOM - 1);                                     // add past number of days in present month
    ulColdfireTime *= 24;                                                // convert days to hours
    ulColdfireTime += RTC_HOUR;                                          // add hours passed in present day
    ulColdfireTime *= 60;                                                // convert hours to minutes
    ulColdfireTime += RTC_MIN;                                           // add minutes in present hour
    ulColdfireTime *= 60;                                                // convert minutes to seconds
    ulColdfireTime += RTC_SEC;                                           // add seconds in present minute

    DAYS    = (unsigned short)(ulColdfireTime / (60 * 60 * 24));         // the number of whole days passed
    ulColdfireTime -= (DAYS * (60 * 60 * 24));
    HOURMIN = (ulColdfireTime / (60 * 60));
    ulColdfireTime -= (HOURMIN * (60 * 60));
    HOURMIN <<= 8;
    HOURMIN |= (ulColdfireTime / 60);
    ulColdfireTime -= ((ulColdfireTime / 60) * 60);
    SECONDS = ulColdfireTime;

    RTCCTL = RTC_ENABLE;                                                 // enable the RTC (this assumes that it was previously enabled before last power down, which is the general case)
    #endif
}
#endif

// We can simulate timers during a tick here if required
//
extern int fnSimTimers(void)
{
    static unsigned long ulWatchdogTimer = 0;
    int iWatchdogFired = 0;

#if defined _WINDOWS && defined CAN_INTERFACE && defined SIM_KOMODO      // {68}
    fnSimCAN(0, 0, CAN_SIM_CHECK_RX);                                    // poll the CAN interface at the tick rate
#endif

#if defined SIMULATE_EXT_RTC_INTERRUPT                                   // can be used for external I2C based RTC with port interrupt
    if (fnCheckRTC()) {
        TOGGLE_RTC_INPUT();
    }
#endif
#if defined SUPPORT_RTC && !defined _M521X && !defined _M520X && !defined _M523X // {7}{61}{65}
    if ((RTCCTL & RTC_ENABLE) != 0) {                                    // if RTC is enabled
        static int iSecs = 0;
        if (++iSecs >= SEC) {                                            // 1 second period elapsed
            iSecs = 0;
            RTCISR |= RTC_SECOND_INT;
            if (++SECONDS >= 60) {
                SECONDS = 0;
                RTCISR |= RTC_MINUTE_INT;
                if (STPWCH != SECONDS_MASK) {                            // check stopwatch
                    if (STPWCH == 0) {
                        STPWCH = SECONDS_MASK;
                    }
                    else {
                        (STPWCH)--;
                    }
                }
                if (STPWCH == SECONDS_MASK) {                            // stopwatch count down interrupt
                    RTCISR |= RTC_STOPWATCH;
                }
                (HOURMIN)++;
                if ((HOURMIN & MINUTES_MASK) >= 60) {
                    unsigned char ucHours = (unsigned char)(HOURMIN >> 8);
                    RTCISR |= RTC_HOUR_INT;
                    HOURMIN &= ~MINUTES_MASK;
                    if (++ucHours >= 24) {
                        ucHours = 0;
                        (DAYS)++;
                        RTCISR |= RTC_DAY_INT;
                    }
                    HOURMIN &= ~HOURS_MASK;
                    HOURMIN |= (ucHours << 8);
                }
            }
            if ((ALRM_SEC == SECONDS) && (ALRM_HM == HOURMIN) && (ALRM_DAY == DAYS)) { // check alarm match
                RTCISR |= RTC_ALARM_INT;
            }
            if (RTCISR & RTCIENR) {                                      // interrupt enabled
                if (((IC_IMRH_0 & RTC_INT_H) == 0) && ((IC_IMRL_0 & MASK_ALL_INT) == 0)) { // interrupt source unmasked
                    __VECTOR_RAM[RTC_VECTOR]();                          // call interrupt handler
                }
            }
        }
    }
#endif
    if ((CWCR & CWE) != 0) {                                             // if watchdog timer active
        if (CWSR == 0xaa) {                                              // assume watchdog timer has been reset since last check
            CWSR = 0x55;                                                 // mark requires resetting again
            ulWatchdogTimer = 0;
        }
        else {
            ulWatchdogTimer += (BUS_CLOCK/1000)*(TICK_RESOLUTION/1000);
            switch (CWCR & 0x38) {
            case (0x38):
                if (ulWatchdogTimer >= (2147483648)) {
                    iWatchdogFired = 1;;
                }
                break;

            case (0x30):
                if (ulWatchdogTimer >= (134217728)) {
                    iWatchdogFired = 1;
                }
                break;

            case (0x28):
                if (ulWatchdogTimer >= (8388608)) {
                    iWatchdogFired = 1;
                }
                break;

            default:                                                     // slower timeouts lead always to watchdog firing when simulating...
                iWatchdogFired = 1;
                break;
            }
            if (iWatchdogFired) {
                CWCR |= CWTIF;
                if ((IC_IMRL_0 & (SW_WDG_PIF_INT_L | MASK_ALL_INT)) == 0) {
                   __VECTOR_RAM[SW_WD_VECTOR]();                         // software watchdog interrupt if enabled
                }
            }
        }
    }
#if defined BACKUP_WATCHDOG_TIMER_AVAILABLE                              // {45}
    if ((WCR & BWT_EN) != 0) {                                           // if backup watchdog timer is enabled
        if (WSR == 0xaaaa) {                                             // assume watchdog timer has been reset since last check
            WSR = 0x3333;                                                // mark requires resetting again
            ulWatchdogTimer = 0;
            WCNTR = WMR;                                                 // set the initial count value again
        }
        else {                                                           // watchdog trigger has not been detected so count down
    #if defined _M52XX_SDRAM || defined _M520X                           // {59}{61}
            unsigned long ulClocks = ((BUS_CLOCK/2/4096/1000)*(TICK_RESOLUTION/1000));
    #else
			unsigned long ulClocks = 0;
        #if defined _M523X                                               // {65}
			ulClocks = ((BUS_CLOCK/2/4096/1000)*TICK_RESOLUTION);
        #else
            if ((MCF_CLOCK_BWCR & BWDSEL_RELAX) != 0) {                  // clock from relaxation oscillator
                if (MCF_CLOCK_OCHR & RELAX_OCOEN) {                      // check that the relaxation oscillator is enabled
                    ulClocks = (RELAXATION_OSCILLATOR_FREQ/4096/1000)*TICK_RESOLUTION;
                }
            }
            else {                                                       // clock from system clock/2
                ulClocks = (((BUS_CLOCK/2/4096/1000)*TICK_RESOLUTION)/1000);
            }
        #endif
    #endif
            if ((unsigned long)WCNTR >= ulClocks) {
                WCNTR -= (unsigned short)ulClocks;                       // count down
            }
            else {
                return RESET_CARD_WATCHDOG;                              // watchdog fires and causes a soft reset
            }
        }
    }
#endif

    // PIT timers
    //
    if ((PIT_PCSR_0 & PIT_EN) != 0) {                                    // {26} if PIT 0 is enabled
        unsigned long ulTickCount = (TICK_RESOLUTION * (BUS_CLOCK/2/1000)); // count with no prescaler
        int iPrescaler = ((PIT_PCSR_0 >> 8) & 0x0f);
        ulTickCount /= (1 << iPrescaler);
        if (ulTickCount < PIT_PCNTR_0) {
            PIT_PCNTR_0 -= (unsigned short)ulTickCount;
        }
        else {                                                           // assumed always in reload mode
            PIT_PCNTR_0 = PIT_PMR_0;                                     // reload the start value
            if (PIT_PCSR_0 & PIT_PIE) {                                  // if interrupt enabled
#if defined _M520X                                                       // {61}
                if (!(IC_IMRL_0 & PIT_0_PIF_INT_L)) {                    // if interrupt not masked
                    __VECTOR_RAM[PIT0_VECTOR]();
                }
#else
                if (!(IC_IMRH_0 & PIT_0_PIF_INT_H)) {                    // if interrupt not masked
                    __VECTOR_RAM[PIT0_VECTOR]();
                }
#endif
            }
        }
    }
    if ((PIT_PCSR_1 & PIT_EN) != 0) {                                    // {26} if PIT 1 is enabled
        unsigned long ulTickCount = (TICK_RESOLUTION * (BUS_CLOCK/2/1000)); // count with no prescaler
        int iPrescaler = ((PIT_PCSR_1 >> 8) & 0x0f);
        ulTickCount /= (1 << iPrescaler);
        if (ulTickCount < PIT_PCNTR_1) {
            PIT_PCNTR_1 -= (unsigned short)ulTickCount;
        }
        else {                                                           // assumed always in reload mode
            PIT_PCNTR_1 = PIT_PMR_1;                                     // reload the start value
            if (PIT_PCSR_1 & PIT_PIE) {                                  // if interrupt enabled
#if defined _M520X                                                       // {61}
                if (!(IC_IMRL_0 & PIT_1_PIF_INT_L)) {                    // if interrupt not masked
                    __VECTOR_RAM[PIT1_VECTOR]();
                }
#else
                if (!(IC_IMRH_0 & PIT_1_PIF_INT_H)) {                    // if interrupt not masked
                    __VECTOR_RAM[PIT1_VECTOR]();
                }
#endif
            }
        }
    }

    // DMA Timer 0                                                       {20}
    //
    if ((DTMR0 & ENABLE_DMA_TIMER) != 0) {                               // if timer 0 is enabled
        unsigned long ulTimerCount = ((TICK_RESOLUTION/1000) * (BUS_CLOCK/1000)); // timer count in a TICK period
        switch (DTMR0 & DMA_TIMER_STOP_COUNT_MASK) {
        case BUS_CLOCK_DIRECT:
            break;
        case BUS_CLOCK_16:
            ulTimerCount /= 16;
            break;
        case CLOCK_DTIN:
        default:                                                         
            ulTimerCount = 0;
            break;
        }
        if (ulTimerCount != 0) {
            ulTimerCount /= (((DTMR0 >> PRESCALER_SHIFT) & 0xff) + 1);   // respect prescaler
            DTCN0 += ulTimerCount;                                       // new count value after TICK period
            if (DTCN0 >= DTRR0) {
                DTER0 |= DMA_TIM_EVENT_REF;                              // {27}
                if (DTMR0 & DMA_TIM_RESTART) {
                    DTCN0 = 0;
                }
                if (DTMR0 & ORRI) {                                      // match with enabled interrupt
                    if (DTXMR0 & DMA_TIMER_DMAEN) {                      // timer programmed to perform DMA rather than interrupt {36}
                    }
                    else {
#if defined _M520X
                        if ((!((DTIM0_PIF_INT_H) & IC_IMRH_0)) && (!(MASK_ALL_INT & IC_IMRL_0))) 
#else
                        if (!((DTIM0_PIF_INT_L | MASK_ALL_INT) & IC_IMRL_0)) 
#endif
                        {                                                // {62}
                            __VECTOR_RAM[DTIM0_VECTOR]();                // call interrupt routine
                        }
                    }
                }
            }
        }
    }

    // DMA Timer 1                                                       {20}
    //
    if ((DTMR1 & ENABLE_DMA_TIMER) != 0) {                               // if timer 1 is enabled
        unsigned long ulTimerCount = ((TICK_RESOLUTION/1000) * (BUS_CLOCK/1000)); // timer count in a TICK period
        switch (DTMR1 & DMA_TIMER_STOP_COUNT_MASK) {
        case BUS_CLOCK_DIRECT:
            break;
        case BUS_CLOCK_16:
            ulTimerCount /= 16;
            break;
        case CLOCK_DTIN:
        default:                                                         
            ulTimerCount = 0;
            break;
        }
        if (ulTimerCount != 0) {
            ulTimerCount /= (((DTMR1 >> PRESCALER_SHIFT) & 0xff) + 1);   // respect prescaler
            DTCN1 += ulTimerCount;                                       // new count value after TICK period
            if (DTCN1 >= DTRR1) {
                DTER1 |= DMA_TIM_EVENT_REF;                              // {27}
                if (DTMR1 & DMA_TIM_RESTART) {
                    DTCN1 = 0;
                }
                if (DTMR1 & ORRI) {                                      // match with enabled interrupt
                    if (DTXMR1 & DMA_TIMER_DMAEN) {                      // timer programmed to perform DMA rather than interrupt {36}
                    }
                    else {
#if defined _M520X
                        if ((!((DTIM1_PIF_INT_H) & IC_IMRH_0)) && (!(MASK_ALL_INT & IC_IMRL_0))) 
#else
                        if (!((DTIM1_PIF_INT_L | MASK_ALL_INT) & IC_IMRL_0)) 
#endif
                        {                                                // {62}
                            __VECTOR_RAM[DTIM1_VECTOR]();                // call interrupt routine
                        }
                    }
                }
            }
        }
    }

    // DMA Timer 2
    //
    if ((DTMR2 & ENABLE_DMA_TIMER) != 0) {                               // if timer 2 is enabled
        unsigned long ulTimerCount = ((TICK_RESOLUTION/1000) * (BUS_CLOCK/1000)); // timer count in a TICK period
        switch (DTMR2 & DMA_TIMER_STOP_COUNT_MASK) {
        case BUS_CLOCK_DIRECT:
            break;
        case BUS_CLOCK_16:
            ulTimerCount /= 16;
            break;
        case CLOCK_DTIN:
        default:                                                         // {19}
            ulTimerCount = 0;
            break;
        }
        if (ulTimerCount != 0) {                                         // {19}
            ulTimerCount /= (((DTMR2 >> PRESCALER_SHIFT) & 0xff) + 1);   // respect prescaler
            DTCN2 += ulTimerCount;                                       // new count value after TICK period
            if (DTCN2 >= DTRR2) {
                DTER2 |= DMA_TIM_EVENT_REF;                              // {27}
                if (DTMR2 & DMA_TIM_RESTART) {
                    DTCN2 = 0;
                }
                if (DTMR2 & ORRI) {                                      // match with enabled interrupt {19}
                    if (DTXMR2 & DMA_TIMER_DMAEN) {                      // timer programmed to perform DMA rather than interrupt {36}
                    }
                    else {
#if defined _M520X
                        if ((!((DTIM2_PIF_INT_H) & IC_IMRH_0)) && (!(MASK_ALL_INT & IC_IMRL_0))) 
#else
                        if (!((DTIM2_PIF_INT_L | MASK_ALL_INT) & IC_IMRL_0)) 
#endif
                        {                                                // {62}
                            __VECTOR_RAM[DTIM2_VECTOR]();                // call interrupt routine
                        }
                    }
                }
            }
        }
    }

    // DMA Timer 3                                                       {4}
    //
    if ((DTMR3 & ENABLE_DMA_TIMER) != 0) {                               // if timer 3 is enabled
        unsigned long ulTimerCount = ((TICK_RESOLUTION/1000) * (BUS_CLOCK/1000)); // timer count in a TICK period
        switch (DTMR3 & DMA_TIMER_STOP_COUNT_MASK) {
        case BUS_CLOCK_DIRECT:
            break;
        case BUS_CLOCK_16:
            ulTimerCount /= 16;
            break;
        case CLOCK_DTIN:
        default:                                                         // {19}
            ulTimerCount = 0;
            break;
        }
        if (ulTimerCount != 0) {                                         // {19}
            ulTimerCount /= (((DTMR3 >> PRESCALER_SHIFT) & 0xff) + 1);   // respect prescaler
            DTCN3 += ulTimerCount;                                       // new count value after TICK period
            if (DTCN3 >= DTRR3) {
                DTER3 |= DMA_TIM_EVENT_REF;                              // {27}
                if (DTMR3 & DMA_TIM_RESTART) {
                    DTCN3 = 0;
                }
                if (DTMR3 & ORRI) {                                      // match with enabled interrupt {19}
                    if (DTXMR3 & DMA_TIMER_DMAEN) {                      // timer programmed to perform DMA rather than interrupt {36}
                    }
                    else {
#if defined _M520X
                        if ((!((DTIM3_PIF_INT_H) & IC_IMRH_0)) && (!(MASK_ALL_INT & IC_IMRL_0))) 
#else
                        if (!((DTIM3_PIF_INT_L | MASK_ALL_INT) & IC_IMRL_0)) 
#endif
                        {                                                // {62}
                            __VECTOR_RAM[DTIM3_VECTOR]();                // call interrupt routine
                        }
                    }
                }
            }
        }
    }

#if !defined _M520X && !defined _M523X                                   // {61}{65}
    // General Purpose Timer                                             // {42}
    //
    if ((GPTSCR1 & GPTEN) != 0) {                                        // if the timer module is operating
        if (GPTPACTL & (PGPT_PACLK_65536 | PGPT_PACLK_256 | PGPT_PACLK)) { // using pulse accumulation mode clock
                                                                         // pulse accululator not presently supported
        }
        else {                                                           // clocked from system clock and through prescaler
            unsigned long ulTimerCount = ((TICK_RESOLUTION * (BUS_CLOCK/2000))/1000); // timer count in a TICK period with no prescaler
            unsigned long ulNewCount = GPTCNT;
            ulTimerCount >>= (GPTSCR2 & GPT_PRESCALE_128);               // prescale
            ulNewCount += ulTimerCount;                                  // new counter value
            if (ulNewCount > 0xffff) {                                   // check for 16 bit overflow
                if (GPTSCR2 & TOI) {                                     // timer overflow interrupt active
                    if (!(IC_IMRH_0 & GPTTOF_PIF_INT_H)) {               // if interrupt is not masked
                        __VECTOR_RAM[GPT_TOF_VECTOR]();                  // call interrupt routine
                    }
                }
                ulNewCount &= 0xffff;
            }
            GPTCNT = (unsigned short)ulNewCount;
        }        
    }
#endif
    if ((RESET_RCR & SOFT_RST) != 0) {                                   // {46} (just check bit)
        if (iWatchdogFired != 0) {
            return RESET_CARD_WATCHDOG;
        }
        return RESET_SIM_CARD;                                           // signal that the board needs to be reset
    }
#if defined SUPPORT_ADC && !defined _M520X                               // {63}
    if (((PPMRH & CDADC) == 0) && ((ADC_CTRL1 & START0) != 0)) {         // ADC converter powered up and running
        fnTriggADC(0);                                                   // update all active channels
        if (ADC_CTRL1 & EOSIE0) {                                        // if end of conversion interrupt enabled
            ADC_ADSTAT |= (EOSI0);                                       // set flag
            if (!(IC_IMRH_0 & ADCA_PIF_INT_H)) {
                __VECTOR_RAM[ADC_ADCA_VECTOR]();
            }
        }
        if (ADC_CTRL2 & EOSIE1) {                                        // module B interrupt
            ADC_ADSTAT |= (EOSI1);                                       // set flag
            if (!(IC_IMRH_0 & ADCB_PIF_INT_H)) {
                __VECTOR_RAM[ADC_ADCB_VECTOR]();
            }
        }
    }
#endif
    return 0;
}

static void fnCheckGPT(unsigned char *ptrMask)                           // {42} if GPT is in operation the timer can override a pin function
{
    if (GPTSCR1 & GPTEN) {                                               // if GPT is in operation the timer can override a pin function
        if (GPTCTL2 & 0x03) {                                            // channel 0 is being used for capture as long as DDR is set as input
            if (!(GPTDDR & 0x01)) {
                *ptrMask &= ~0x01;
            }
        }
        if (GPTCTL2 & 0x0c) {                                            // channel 1 is being used for capture as long as DDR is set as input
            if (!(GPTDDR & 0x02)) {
                *ptrMask &= ~0x02;
            }
        }
        if (GPTCTL2 & 0x30) {                                           // channel 2 is being used for capture as long as DDR is set as input
            if (!(GPTDDR & 0x04)) {
                *ptrMask &= ~0x04;
            }
        }
        if (GPTCTL2 & 0xc0) {                                            // channel 3 is being used for capture as long as DDR is set as input
            if (!(GPTDDR & 0x08)) {
                *ptrMask &= ~0x08;
            }
        }
    }
}

static void fnSetNewPer(int iPort)
{
#if !defined _M520X && !defined _M523X                                   // {61}
    unsigned long ulBits = 0x0003;
    unsigned long ulPer;
    int iShift = 0;
    int iPortBits = 0;
    unsigned char ucMask = 0xff;
#endif

    switch (iPort) {
#if defined _M52XX_SDRAM                                                 // {59}
    case _PORT_A:
    case _PORT_G:
    case _PORT_H:
        if (CCM_RCON & RCON_MODE) {                                      // master mode
            ucPortFunctions[iPort][0] = 1;
            ucPortFunctions[iPort][1] = 1;
            ucPortFunctions[iPort][2] = 1;
            ucPortFunctions[iPort][3] = 1;
            ucPortFunctions[iPort][4] = 1;
            ucPortFunctions[iPort][5] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][7] = 1;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_B:
        if (PBCDPAR & PBCDPAR_PBPA) {
            ucPortFunctions[iPort][0] = 1;
            ucPortFunctions[iPort][1] = 1;
            ucPortFunctions[iPort][2] = 1;
            ucPortFunctions[iPort][3] = 1;
            ucPortFunctions[iPort][4] = 1;
            ucPortFunctions[iPort][5] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][7] = 1;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_C:
    case _PORT_D:
        if (PBCDPAR & PBCDPAR_PCDPA) {
            ucPortFunctions[iPort][0] = 1;
            ucPortFunctions[iPort][1] = 1;
            ucPortFunctions[iPort][2] = 1;
            ucPortFunctions[iPort][3] = 1;
            ucPortFunctions[iPort][4] = 1;
            ucPortFunctions[iPort][5] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][7] = 1;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_E:
        while (iPortBits < 8) {
            ulPer = PEPAR & ulBits;
            ulPer >>= iShift;
            if ((iPortBits == 2) || (iPortBits == 3)) {
                if (!(CCM_CCR & CCR_TSIZ)) {                             // SIZ0 and SIZ1 not selected as peripheral
                    ulPer = 3;
                } 
            }
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;
    case _PORT_F:
        while (iPortBits < 8) {
            if ((iPortBits < 5) && (CCM_RCON & RCON_MODE)) {             // master mode
                ulPer = 0x1f;
            }
            else {
                ulPer = ((PFPAR & ulBits) & 0xe0);
            }
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    case _PORT_J:
        while (iPortBits < 8) {
            ulPer = (PJPAR & ulBits);
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    case _PORT_EL:
        if (PEHLPAR & PEHLPAR_PELPA) {
            ucPortFunctions[iPort][0] = 1;
            ucPortFunctions[iPort][1] = 1;
            ucPortFunctions[iPort][2] = 1;
            ucPortFunctions[iPort][3] = 1;
            ucPortFunctions[iPort][4] = 1;
            ucPortFunctions[iPort][5] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][7] = 1;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_EH:
        if (PEHLPAR & PEHLPAR_PEHPA) {
            ucPortFunctions[iPort][0] = 1;
            ucPortFunctions[iPort][1] = 1;
            ucPortFunctions[iPort][2] = 1;
            ucPortFunctions[iPort][3] = 1;
            ucPortFunctions[iPort][4] = 1;
            ucPortFunctions[iPort][5] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][7] = 1;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_QA:                                                       // QADC not yet supported so only port function
    case _PORT_QB:
    case _PORT_TA:                                                       // GTP not yet supported so only port function
    case _PORT_TB:
        ucPortFunctions[iPort][0] = 0;
        ucPortFunctions[iPort][1] = 0;
        ucPortFunctions[iPort][2] = 0;
        ucPortFunctions[iPort][3] = 0;
        ucPortFunctions[iPort][4] = 0;
        ucPortFunctions[iPort][5] = 0;
        ucPortFunctions[iPort][6] = 0;
        ucPortFunctions[iPort][7] = 0;
        break;
    case _PORT_DD:
        if (CCM_CCR & CCR_PSTEN) {
            ucPortFunctions[iPort][0] = 1;
            ucPortFunctions[iPort][1] = 1;
            ucPortFunctions[iPort][2] = 1;
            ucPortFunctions[iPort][3] = 1;
            ucPortFunctions[iPort][4] = 1;
            ucPortFunctions[iPort][5] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][7] = 1;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_NQ:
        ulBits = 0x01;
        while (iPortBits < 8) {
            ulPer = (~EPDDR0 & ulBits);
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
#elif defined _M520X                                                     // {61}
    case _PORT_FECH:
        if ((PAR_FEC & PAR_FEC_7W) == PAR_U1RTS) {
          //ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 1;                               // U1RTS on FEC_TXEN
            ucPortFunctions[iPort][7] = 0;
        }
        else if ((PAR_FEC & PAR_FEC_7W) == PAR_FEC_7W) {                 // 7 wire pins active
          //ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 3;
            ucPortFunctions[iPort][2] = 3;
            ucPortFunctions[iPort][3] = 3;
            ucPortFunctions[iPort][4] = 3;
            ucPortFunctions[iPort][5] = 3;
            ucPortFunctions[iPort][6] = 3;
            ucPortFunctions[iPort][7] = 3;
        }
        else {
          //ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
        }
        break;
    case _PORT_FECL:
        if ((PAR_FEC & PAR_FEC_MII) == PAR_U0CTS_U1CTS_U0RTS) {
            ucPortFunctions[iPort][0] = 1;                               // U0CTS on FEC_RXER
            ucPortFunctions[iPort][1] = 1;                               // U1CTS on FEC_RXD1
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 1;                               // U0RTS on FEC_TXER
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
            ucPortFunctions[_PORT_FECH][0] = 0;
        }
        else if ((PAR_FEC & PAR_FEC_MII) == PAR_FEC_MII) {               // FEC MII pins
            ucPortFunctions[iPort][0] = 3;
            ucPortFunctions[iPort][1] = 3;
            ucPortFunctions[iPort][2] = 3;
            ucPortFunctions[iPort][3] = 3;
            ucPortFunctions[iPort][4] = 3;
            ucPortFunctions[iPort][5] = 3;
            ucPortFunctions[iPort][6] = 3;
            ucPortFunctions[iPort][7] = 3;
            ucPortFunctions[_PORT_FECH][0] = 3;
        }
        else {
            ucPortFunctions[iPort][0] = 0;
            ucPortFunctions[iPort][1] = 0;
            ucPortFunctions[iPort][2] = 0;
            ucPortFunctions[iPort][3] = 0;
            ucPortFunctions[iPort][4] = 0;
            ucPortFunctions[iPort][5] = 0;
            ucPortFunctions[iPort][6] = 0;
            ucPortFunctions[iPort][7] = 0;
            ucPortFunctions[_PORT_FECH][0] = 0;
        }
        break;
    case _PORT_BUSCTL:
        ucPortFunctions[iPort][0] = (PAR_BUSCTL & PAR_TS_FUNCTION);
        ucPortFunctions[iPort][1] = ((PAR_BUSCTL >> 2) & 0x01);
        ucPortFunctions[iPort][2] = ((PAR_BUSCTL >> 3) & 0x01);
        ucPortFunctions[iPort][3] = ((PAR_BUSCTL >> 4) & 0x01);
        break;
    case _PORT_BE:
        ucPortFunctions[iPort][0] = ((PAR_BE >> 0) & 0x01);
        ucPortFunctions[iPort][1] = ((PAR_BE >> 1) & 0x01);
        ucPortFunctions[iPort][2] = ((PAR_BE >> 2) & 0x01);
        ucPortFunctions[iPort][3] = ((PAR_BE >> 3) & 0x01);
        break;
    case _PORT_CS:
        ucPortFunctions[iPort][0] = (PAR_CS & PAR_FB_CS1);
        ucPortFunctions[iPort][1] = ((PAR_CS >> 2) & 0x01);
        ucPortFunctions[iPort][2] = ((PAR_CS >> 3) & 0x01);
        break;
    case _PORT_FECI2C:
        ucPortFunctions[iPort][0] = ((PAR_FECI2C >> 0) & 0x03);
        ucPortFunctions[iPort][1] = ((PAR_FECI2C >> 2) & 0x03);
        ucPortFunctions[iPort][2] = ((PAR_FECI2C >> 4) & 0x03);
        ucPortFunctions[iPort][3] = ((PAR_FECI2C >> 6) & 0x03);
        break;
    case _PORT_UART:
        ucPortFunctions[iPort][0] = ((PAR_UART >> 0) & 0x01);
        ucPortFunctions[iPort][1] = ((PAR_UART >> 1) & 0x01);
        ucPortFunctions[iPort][2] = ((PAR_UART >> 2) & 0x03);
        ucPortFunctions[iPort][3] = ((PAR_UART >> 4) & 0x03);
        ucPortFunctions[iPort][4] = ((PAR_UART >> 6) & 0x01);
        ucPortFunctions[iPort][5] = ((PAR_UART >> 7) & 0x01);
        ucPortFunctions[iPort][6] = ((PAR_UART >> 9) & 0x03);
        ucPortFunctions[iPort][7] = ((PAR_UART >> 11) & 0x03);
        break;
    case _PORT_QSPI:
        ucPortFunctions[iPort][0] = ((PAR_QSPI >> 0) & 0x03);
        ucPortFunctions[iPort][1] = ((PAR_QSPI >> 2) & 0x03);
        ucPortFunctions[iPort][2] = ((PAR_QSPI >> 4) & 0x03);
        ucPortFunctions[iPort][3] = ((PAR_QSPI >> 6) & 0x03);
        break;
    case _PORT_TIMER:
        ucPortFunctions[iPort][0] = ((PAR_TIMER >> 0) & 0x03);
        ucPortFunctions[iPort][1] = ((PAR_TIMER >> 2) & 0x03);
        ucPortFunctions[iPort][2] = ((PAR_TIMER >> 4) & 0x03);
        ucPortFunctions[iPort][3] = ((PAR_TIMER >> 6) & 0x03);
        break;
    case _PORT_IRQ:
        ucPortFunctions[iPort][0] = 0;
        ucPortFunctions[iPort][1] = 0;
        ucPortFunctions[iPort][2] = 0;
        ucPortFunctions[iPort][3] = 0;
        if (PAR_IRQ & PAR_IRQ4) {
            ucPortFunctions[iPort][4] = 1;
        }
        else {
            ucPortFunctions[iPort][4] = 0;
        }
        ucPortFunctions[iPort][5] = 0;
        ucPortFunctions[iPort][6] = 0;
        ucPortFunctions[iPort][7] = 0;
        break;
#elif defined _M523X                                                     // {65}
    case _PORT_ADDR:
        switch (CCM_RCON >> 8) {
        case 0:                                                          // PADDR[7:5] = A[23:21]
            ucPortFunctions[iPort][7] = 1;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][5] = 1;
            break;
        case 1:                                                          // PADDR[7:6] = CS[6:5]; PADDR[5] = A[21]
            ucPortFunctions[iPort][7] = 2;
            ucPortFunctions[iPort][6] = 2;
            ucPortFunctions[iPort][5] = 1;
            break;
        case 2:                                                          // PADDR[7] = CS[6]; PADDR[6:5] = A[22:21]
            ucPortFunctions[iPort][7] = 2;
            ucPortFunctions[iPort][6] = 1;
            ucPortFunctions[iPort][5] = 1;
            break;
        case 3:                                                          // PADDR[7:5] = CS[7:5]
            ucPortFunctions[iPort][7] = 2;
            ucPortFunctions[iPort][6] = 2;
            ucPortFunctions[iPort][5] = 2;
        }
        if (!(PAR_AD & PAR_ADDR23)) {
            ucPortFunctions[iPort][7] = 0;
        }
        if (!(PAR_AD & PAR_ADDR22)) {
            ucPortFunctions[iPort][6] = 0;
        }
        if (!(PAR_AD & PAR_ADDR21)) {
            ucPortFunctions[iPort][5] = 0;
        }
        break;

    case _PORT_DATAH:
    case _PORT_DATAL:
        if (PAR_AD & PAR_DATAL) {                                        // control the peripheral function of the data ports
            int i = 0;
            while (i < 8) {
                ucPortFunctions[iPort][i] = 1;
                i++;
            }
        }
        else {
            int i = 0;
            while (i < 8) {
                ucPortFunctions[iPort][0] = 0;
                i++;
            }
        }
        break;

    case _PORT_FECI2C:
        ucPortFunctions[iPort][0] = ((PAR_FECI2C >> 0) & 0x03);
        ucPortFunctions[iPort][1] = ((PAR_FECI2C >> 2) & 0x03);
        ucPortFunctions[iPort][2] = ((PAR_FECI2C >> 4) & 0x03);
        ucPortFunctions[iPort][3] = ((PAR_FECI2C >> 6) & 0x03);
        break;

    case _PORT_UARTL:
        ucPortFunctions[iPort][0] = ((PAR_UART >> 3) & 0x01);
        ucPortFunctions[iPort][1] = ((PAR_UART >> 2) & 0x01);
        ucPortFunctions[iPort][2] = ((PAR_UART >> 0) & 0x01);
        ucPortFunctions[iPort][3] = ((PAR_UART >> 1) & 0x01);
        ucPortFunctions[iPort][4] = ((PAR_UART >> 10) & 0x03);
        ucPortFunctions[iPort][5] = ((PAR_UART >> 8) & 0x03);
        ucPortFunctions[iPort][6] = ((PAR_UART >> 4) & 0x03);
        ucPortFunctions[iPort][7] = ((PAR_UART >> 6) & 0x03);
        break;
#else
    case _PORT_AN:
        while (iPortBits < 8) {
            ulPer = PANPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    #if !defined _M5221X && !defined _M5222X && !defined _M521XX && !defined _M521X && !defined _M5225X // {39}
    case _PORT_GP:
        ulBits = 0x01;
        while (iPortBits < 8) {
            ulPer = PGPPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            if ((ulPer & 0x01) != 0) {
                ucIRQ1 |= (unsigned char)ulBits;
                EPPDR1 |= ((unsigned char)ulBits & ucPort_in_GP);        // IRQ so synchronise to input state
            }
            else {
                ucIRQ1 &= (unsigned char)~ulBits;
                EPPDR1 &= (unsigned char)~ulBits;                        // not IRQ so remove state
            }
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    #endif
#endif
#if !defined _M520X && !defined _M523X
    case _PORT_QS:
        while (iPortBits < 7) {
            ulPer = PQSPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_TA:
        {
            unsigned char ucMask = 0x0f;
            fnCheckGPT(&ucMask);                                         // {42} if GPT is in operation the timer can override a pin function
            while (iPortBits < 4) {
                ulPer = PTAPAR & ulBits;
                ulPer >>= iShift;
                ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
                if (!(ucMask & (1 << iPortBits))) {                      // {42} if the GPT function is overriding, set GPT
                    ucPortFunctions[iPort][iPortBits] = 1;               // primary function
                }
                ulBits <<= 2;
                iShift += 2;
                iPortBits++;
            }
        }
        break;

    #if !defined _M521X && !defined _M5225X                              // {39}
    case _PORT_LD:  
        while (iPortBits < 8) {
            ulPer = PLDPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }        
        break;
    #endif
#endif
#if !defined _M520X && !defined _M523X
    case _PORT_TC:
        while (iPortBits < 4) {
            ulPer = PTCPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;
#endif
#if !defined _M5225X && !defined _M520X && !defined _M523X               // {39}{61}
    case _PORT_TD:
        while (iPortBits < 4) {
            ulPer = PTDPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_UA:
        while (iPortBits < 4) {
            ulPer = PUAPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_UB:
        while (iPortBits < 4) {
            ulPer = PUBPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;

    case _PORT_UC:
        while (iPortBits < 4) {
    #if defined _M5222X || defined _M5225X                               // {39} quad-function
            ulPer = PUCPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
    #else                                                                // dual function
            ulPer = PUCPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
    #endif
        }
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_AS: 
    #if defined _M52XX_SDRAM                                             // {59}
        while (iPortBits < 8) 
    #else
        while (iPortBits < 4) 
    #endif
        {
            ulPer = PASPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_NQ:       
    #if defined _M521X
        ulBits <<= 1;
        iShift += 1;
        iPortBits++;
        while (iPortBits < 8) {
            if (iPortBits == 1) {                                        // {52} special case for quad-function pin
                ucPortFunctions[iPort][iPortBits] = (unsigned char)(PNQPAR & 0x03);
                if ((PNQPAR & 0x03) == 0x01) {
                    ucIRQ0 |= 0x02;
                    ucIRQ_P0 |= 0x02;
                    EPPDR0 |= (0x02 & ucPort_in_NQ);                     // {55} IRQ so synchronise to input state
                }
                else {
                    ucIRQ0 &= ~0x02;
                    if (PNQPAR & 0x03) {
                        ucIRQ_P0 |= 0x02;
                    }
                    else {
                        ucIRQ_P0 &= ~0x02;
                    }
                    EPPDR0 &= ~0x02;                                     // {55} not IRQ so remove state
                }
            }
            else {
                ulPer = PNQPAR & ulBits;
                ulPer >>= iShift;
                ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
                if ((ulPer & 0x01) != 0) {
                    ucIRQ0 |= (unsigned char)ulBits;
                    ucIRQ_P0 |= (unsigned char)ulBits;
                    EPPDR0 |= (ulBits & ucPort_in_NQ);                   // {55} IRQ so synchronise to input state
                }
                else {
                    ucIRQ0 &= (unsigned char)~ulBits;
                    ucIRQ_P0 &= ~(unsigned char)ulBits;
                    EPPDR0 &= ~ulBits;                                   // {55} not IRQ so remove state
                }
            }
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
    #else
        ulBits <<= 2;
        iShift += 2;
        iPortBits++;
        while (iPortBits < 8) {
            ulPer = PNQPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            if ((ulPer & 0x03) == 0x01) {
                ucIRQ0 |= (0x01 << (iShift/2));
                ucIRQ_P0 |= (0x01 << (iShift/2));
                EPPDR0 |= ((0x01 << (iShift/2)) & ucPort_in_NQ);         // {55} IRQ so synchronise to input state
            }
            else {
                ucIRQ0 &= ~(0x01 << (iShift/2));
                if (ulPer & 0x03) {
                    ucIRQ_P0 |= (0x01 << (iShift/2));
                }
                else {
                    ucIRQ_P0 &= ~(0x01 << (iShift/2));
                }
                EPPDR0 &= ~(0x01 << (iShift/2));                         // {55} not IRQ so remove state
            }
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
    #endif
        break;

    case _PORT_DD:
        while (iPortBits < 8) {
            ulPer = PDDPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
#endif
#if defined _M5225X
    case _PORT_TE:
        while (iPortBits < 8) {
            ulPer = PTEPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    case _PORT_TF:
        while (iPortBits < 8) {
            ulPer = PTFPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    case _PORT_TG:
        while (iPortBits < 8) {
            ulPer = PTGPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    case _PORT_TH:
        while (iPortBits < 8) {
            ulPer = PTHPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x03);
            ulBits <<= 2;
            iShift += 2;
            iPortBits++;
        }
        break;
    case _PORT_TI:
        while (iPortBits < 8) {
            ulPer = PTIPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
    case _PORT_TJ:
        while (iPortBits < 8) {
            ulPer = PTJPAR & ulBits;
            ulPer >>= iShift;
            ucPortFunctions[iPort][iPortBits] = (unsigned char)(ulPer & 0x01);
            ulBits <<= 1;
            iShift += 1;
            iPortBits++;
        }
        break;
#endif
    }
}

// Return a mask of the bist that are configured as GPIO
//
static unsigned char fnGetPorts(int iPort)
{
    unsigned char ucMask = 0xff;                                         // initially assume all bits are ports
    switch (iPort) {
#if defined _M52XX_SDRAM                                                 // {59}
    case _PORT_A:
    case _PORT_G:
    case _PORT_H:
        if (CCM_RCON & RCON_MODE) {                                      // master mode
            ucMask = 0;                                                  // always peripheral function
        }
        break;
    case _PORT_B:
        if (PBCDPAR & PBCDPAR_PBPA) {
            ucMask = 0;                                                  // peripheral function
        }
        break;
    case _PORT_C:
    case _PORT_D:
        if (PBCDPAR & PBCDPAR_PCDPA) {
            ucMask = 0;                                                  // peripheral function
        }
        break;
    case _PORT_E:
        if (PEPAR & 0x0003) {
            ucMask &= ~0x01;
        }
        if (PEPAR & 0x000c) {
            ucMask &= ~0x02;
        }
        if (PEPAR & 0x0010) {
            ucMask &= ~0x04;
        }
        if (PEPAR & 0x0040) {
            ucMask &= ~0x08;
        }
        if (PEPAR & 0x0100) {
            ucMask &= ~0x10;
        }
        if (PEPAR & 0x0400) {
            ucMask &= ~0x20;
        }
        if (PEPAR & 0x1000) {
            ucMask &= ~0x40;
        }
        if (PEPAR & 0x4000) {
            ucMask &= ~0x80;
        }
        break;

    case _PORT_F:
        if (CCM_RCON & RCON_MODE) {                                      // master mode
            ucMask &= ~0x1f;                                             // always peripheral function
        }
        ucMask &= ~(PFPAR & 0xe0);
        break;
    case _PORT_J:
        ucMask = ~PJPAR;
        break;
    case _PORT_SD:
        if (PSDPAR & PSDPAR_PSDPA) {
            ucMask = 0;
        }
        break;
    case _PORT_EL:
        if (PEHLPAR & PEHLPAR_PELPA) {
            ucMask = 0;
        }
        break;
    case _PORT_EH:
        if (PEHLPAR & PEHLPAR_PEHPA) {
            ucMask = 0;
        }
        break;
    case _PORT_QA:                                                       // QADC not yet supported so only port function
    case _PORT_QB:
    case _PORT_TA:                                                       // GTP not yet supported so only port function
    case _PORT_TB:
        break;
    case _PORT_DD:
        if (CCM_CCR & CCR_PSTEN) {
            ucMask = 0;
        }
        break;
    case _PORT_NQ:                                                       // this port is controlled via the EPORT module - it is considered a peripheral when programmed as input (irrespective of whether interrupt is enabled)
        ucMask &= EPDDR0;                                                // outputs considered ports
        break;
#elif defined _M520X                                                     // {61}
    case _PORT_FECH:
        if ((PAR_FEC & PAR_FEC_7W) == PAR_FEC_7W) {
            ucMask &= ~0xfe;                                             // 7 FEC functions
        }
        else if ((PAR_FEC & PAR_FEC_7W) == PAR_U1RTS) {
            ucMask &= ~0x40;                                             // FEC_TXEN is U1RTS pin
        }
        if ((PAR_FEC & PAR_FEC_MII) == PAR_FEC_MII) {
            ucMask &= ~0x01;                                             // FEC_CSR pin
        }
        break;
    case _PORT_FECL:
        if ((PAR_FEC & PAR_FEC_MII) == PAR_FEC_MII) {
            ucMask = 0;                                                  // all FEC functions
        }
        else if ((PAR_FEC & PAR_FEC_MII) == PAR_U0CTS_U1CTS_U0RTS) {
            ucMask &= ~0x13;                                             // U1CTS, U0CTS and U0RTS functions
        }
        break;
    case _PORT_BUSCTL:
        if (PAR_BUSCTL & PAR_TS_FUNCTION) {
            ucMask &= ~(0x01);
        }
        ucMask &= ~(PAR_BUSCTL >> 1);
        break;
    case _PORT_BE:
        ucMask = ~PAR_BE;
        break;
    case _PORT_CS:
        if (PAR_CS & PAR_FB_CS1) {
            ucMask &= ~(0x01);
        }
        ucMask &= ~(PAR_CS >> 1);
        break;
    case _PORT_FECI2C:
        if (PAR_FECI2C & PAR_SDA_I2C_SDA) {
            ucMask &= ~(0x01);
        }
        if (PAR_FECI2C & PAR_SCL_I2C_SCL) {
            ucMask &= ~(0x02);
        }
        if (PAR_FECI2C & PAR_MDIO_FEC_MDIO) {
            ucMask &= ~(0x04);
        }
        if (PAR_FECI2C & PAR_MDC_FEC_MDC) {
            ucMask &= ~(0x08);
        }
        break;
    case _PORT_UART:
        ucMask &= ~(PAR_UART & (PAR_U0RXD | PAR_U0TXD));
        ucMask &= ~((PAR_UART & (PAR_U1RXD | PAR_U1TXD)) >> 2);
        if (PAR_UART & PAR_U0RTS) {
            ucMask &= ~(0x04);
        }
        if (PAR_UART & PAR_U0CTS) {
            ucMask &= ~(0x08);
        }
        if (PAR_UART & PAR_U1RTS_U1RTS) {
            ucMask &= ~(0x10);
        }
        if (PAR_UART & PAR_U1CTS_U1CTS) {
            ucMask &= ~(0x20);
        }
        break;
    case _PORT_QSPI:
        if (PAR_QSPI & PAR_SCK_QSPI_SCK) {
            ucMask &= ~(0x01);
        }
        if (PAR_QSPI & PAR_DOUT_QSPI_DOUT) {
            ucMask &= ~(0x02);
        }
        if (PAR_QSPI & PAR_DIN_QSPI_DIN) {
            ucMask &= ~(0x04);
        }
        if (PAR_QSPI & PAR_PCS2_QSPI_PCS2) {
            ucMask &= ~(0x08);
        }
        break;
    case _PORT_TIMER:
        if (PAR_TIMER & PAR_T0IN) {
            ucMask &= ~(0x01);
        }
        if (PAR_TIMER & PAR_T1IN) {
            ucMask &= ~(0x02);
        }
        if (PAR_TIMER & PAR_T2IN) {
            ucMask &= ~(0x04);
        }
        if (PAR_TIMER & PAR_T3IN) {
            ucMask &= ~(0x08);
        }
        break;
    case _PORT_IRQ:
        ucMask = ~PAR_IRQ;
        break;
#elif defined _M523X                                                     // {65}
    case _PORT_ADDR:
        ucMask &= ~(PAR_AD & (PAR_ADDR23 | PAR_ADDR22 | PAR_ADDR21));
        break;
    case _PORT_BS:
    case _PORT_BUSCTL:
        break;
    case _PORT_CS:
        break;
    case _PORT_DATAH:
    case _PORT_DATAL:
        if (PAR_AD & PAR_DATAL) {
            ucMask &= ~(0xff);
        }
        break;
    case _PORT_FECI2C:
        if (PAR_FECI2C & (PAR_CAN0TX_F | PAR_I2C_SDA_F0)) {
            ucMask &= ~(0x01);
        }
        if (PAR_FECI2C & (PAR_CAN0RX_F | PAR_I2C_SCL_F0)) {
            ucMask &= ~(0x02);
        }
        if (PAR_FECI2C & (PAR_I2C_SDA_F1 | PAR_FEC_EMDIO)) {
            ucMask &= ~(0x04);
        }
        if (PAR_FECI2C & (PAR_I2C_SCL_F1 | PAR_FEC_EMDC)) {
            ucMask &= ~(0x08);
        }
        break;
    case _PORT_ETPU:
    case _PORT_QSPI:
    case _PORT_SDRAM:
    case _PORT_TIMER:
    case _PORT_UARTH:
        break;
    case _PORT_UARTL:
        if (PAR_UART & PAR_U0TXD) {
            ucMask &= ~(0x02);
        }
        if (PAR_UART & PAR_U0RXD) {
            ucMask &= ~(0x01);
        }
        if (PAR_UART & PAR_U0RTS) {
            ucMask &= ~(0x04);
        }
        if (PAR_UART & PAR_U0CTS) {
            ucMask &= ~(0x08);
        }
        if (PAR_UART & (PAR_U1RTS | PAR_U2RTS)) {
            ucMask &= ~(0x40);
        }
        if (PAR_UART & (PAR_U2CTS | PAR_U1CTS)) {
            ucMask &= ~(0x80);
        }
        if (PAR_UART & (PAR_U1TXD | PAR_CAN0TX)) {
            ucMask &= ~(0x20);
        }
        if (PAR_UART & (PAR_U1RXD | PAR_CAN0RX)) {
            ucMask &= ~(0x10);
        }
        break;
#else
    case _PORT_AN:                                                       // AN dual function port
        ucMask = ~PANPAR;                                                // {6}
        break;

    #if !defined _M521X && !defined _M5225X                              // {39}
    case _PORT_GP:                                                       // GP dual function port
        ucMask = ~PGPPAR;                                                // {6}
        break;
    #endif

    case _PORT_DD:                                                       // DD dual function port
        ucMask = ~PDDPAR;                                                // {6}
        break;
#endif
#if !defined _M520X && !defined _M523X && !defined _M523X                // {61}
    case _PORT_QS:
        if (PQSPAR & 0x03) {                                             // 7 bit port
            ucMask &= ~0x01;
        }
        if (PQSPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PQSPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PQSPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        if (PQSPAR & 0x0300) {
            ucMask &= ~0x10;
        }
        if (PQSPAR & 0x0c00) {
            ucMask &= ~0x20;
        }
        if (PQSPAR & 0x3000) {
            ucMask &= ~0x40;
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_TA:                                                       // TA - 4 bit port
        ucMask = 0x0f;
        if (PTAPAR & 0x03) {                                             // get present port setting
            ucMask &= ~0x01;
        }
        if (PTAPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PTAPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PTAPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        break;

    #if !defined _M521X && !defined _M5225X                              // {39}
    case _PORT_LD:                                             
        ucMask = (~PLDPAR & 0x7f);
        break;
    #endif
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_TC:                                                       // TC - 4 bit port
        ucMask = 0x0f;
        if (PTCPAR & 0x03) {
            ucMask &= ~0x01;
        }
        if (PTCPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PTCPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PTCPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        break;
#endif
#if !defined _M5225X && !defined _M520X && !defined _M523X               // {39}{61}
    case _PORT_TD:                                                       // TD - 4 bit port (dual function)
        ucMask = (~PTDPAR & 0x0f);                                       // {17}
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_UA:                                                       // UA - 4 bit port
        ucMask = 0x0f;
        if (PUAPAR & 0x03) {
            ucMask &= ~0x01;
        }
        if (PUAPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PUAPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PUAPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_UB:                                                       // UB - 4 bit port
        ucMask = 0x0f;
        if (PUBPAR & 0x03) {
            ucMask &= ~0x01;
        }
        if (PUBPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PUBPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PUBPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        break;

    case _PORT_UC:                                                       // UC - 4 bit port
    #if defined _M5222X || defined _M5225X                               // {39} - quad-function
        ucMask = 0x0f;
        if (PUCPAR & 0x03) {
            ucMask &= ~0x01;
        }
        if (PUCPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PUCPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PUCPAR & 0xc0) {
            ucMask &= ~0x08;
        }
    #else                                                                // dual-function
        ucMask = (~PUCPAR & 0x0f);                                       // {17}
    #endif
        break;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_AS:                                                       // AS
    #ifndef _M52XX_SDRAM                                                 // {59}
        ucMask = 0x0f;
    #endif
        if (PASPAR & 0x03) {
            ucMask &= ~0x01;
        }
        if (PASPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PASPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PASPAR & 0xc0) {
            ucMask &= ~0x08;
        }
    #if defined _M52XX_SDRAM                                             // {59}
        if (PASPAR & 0x300) {
            ucMask &= ~0x10;
        }
        if (PASPAR & 0xc00) {
            ucMask &= ~0x20;
        }
    #endif
        break;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_NQ:
    #if defined _M521X
        ucMask = 0xfe;
        if (PNQPAR & 0x03) {
            ucMask &= ~0x02;
        }
        if (PNQPAR & 0x04) {
            ucMask &= ~0x04;
        }
        if (PNQPAR & 0x08) {
            ucMask &= ~0x08;
        }
        if (PNQPAR & 0x10) {
            ucMask &= ~0x10;
        }
        if (PNQPAR & 0x20) {
            ucMask &= ~0x20;
        }
        if (PNQPAR & 0x40) {
            ucMask &= ~0x40;
        }
        if (PNQPAR & 0x80) {
            ucMask &= ~0x80;
        }
    #else
        ucMask = 0xfe;
        if (PNQPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PNQPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PNQPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        if (PNQPAR & 0x300) {
            ucMask &= ~0x10;
        }
        if (PNQPAR & 0xc00) {
            ucMask &= ~0x20;
        }
        if (PNQPAR & 0x3000) {
            ucMask &= ~0x40;
        }
        if (PNQPAR & 0xc000) {
            ucMask &= ~0x80;
        }
    #endif
#endif
        break;

#if defined _M5225X                                                      // {39}
    case _PORT_TE:                                                       // dual functions
        ucMask = ~PTEPAR;
        break;
    case _PORT_TF:
        ucMask = ~PTFPAR;
        break;
    case _PORT_TG:
        ucMask = ~PTGPAR;
        break;
    case _PORT_TI:
        ucMask = ~PTIPAR;
        break;
    case _PORT_TJ:
        ucMask = ~PTJPAR;
        break;

    case _PORT_TH:                                                       // quad function
        ucMask = 0xff;
        if (PTHPAR & 0x03) {
            ucMask &= ~0x01;
        }
        if (PTHPAR & 0x0c) {
            ucMask &= ~0x02;
        }
        if (PTHPAR & 0x30) {
            ucMask &= ~0x04;
        }
        if (PTHPAR & 0xc0) {
            ucMask &= ~0x08;
        }
        if (PTHPAR & 0x300) {
            ucMask &= ~0x10;
        }
        if (PTHPAR & 0xc00) {
            ucMask &= ~0x20;
        }
        if (PTHPAR & 0x3000) {
            ucMask &= ~0x40;
        }
        if (PTHPAR & 0xc000) {
            ucMask &= ~0x80;
        }
        break;
#endif
    default:
        return 0;
    }
    return ucMask;
}

static void fnIRQ_int(int irq_vector)                                    // {22}
{
    if (irq_vector <= IRQ7_VECTOR) {                                     // IRQ 1..7
        irq_vector -= IRQ1_VECTOR;
        if (IC_IMRL_0 & ((IRQ1_PIF_INT_L << irq_vector) | MASK_ALL_INT)) {
            return;                                                      // not unmasked in interrupt controller
        }
        irq_vector += IRQ1_VECTOR;
    }
#if !defined _M5222X && !defined _M521X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {59}{61}{65} these devices have only EPORT0
    else {                                                               // IRQ 8..15
        irq_vector -= IRQ8_VECTOR;
        if ((IC_IMRL_1 & MASK_ALL_INT) || (IC_IMRH_1 & ((IRQ8_PIF_INT_H << irq_vector)))) { // {31}
            return;                                                      // not unmasked in interrupt controller
        }
        irq_vector += IRQ8_VECTOR;
    }
#endif
    __VECTOR_RAM[irq_vector]();                                          // call interrupt handler
}


static int fnUpdatePort(unsigned char *ptr_ucPer, unsigned char *ptr_ucPortValue, unsigned char *ptr_ucPortDDR, unsigned char ucPortValue, unsigned char ucDDR_Value, int iPortRef)
{
    int iRtn = 0;
    unsigned char ucMask = fnGetPorts(iPortRef);                         // get a mask of bits configured as GPIO
    if (*ptr_ucPer != (~ucMask & 0xff)) {                                // check whether peripheral functions have changed
        *ptr_ucPer = (~ucMask & 0xff);
        fnSetNewPer(iPortRef);                                           // update the peripheral function
        iRtn = PORT_CHANGE;                                              // display refresh required
    }
    if ((ucPortValue & ucDDR_Value & ucMask) != *ptr_ucPortValue) {      // check whether port value has changed
        *ptr_ucPortValue = (ucPortValue & ucDDR_Value & ucMask);         // new port value
        *ptr_ucPortDDR = (ucDDR_Value & ucMask);                         // synchronise data direction value
        iRtn = PORT_CHANGE;                                              // display refresh required
    }
    else if (*ptr_ucPortDDR != (ucDDR_Value & ucMask)) {                 // check whether data direction has changed
        *ptr_ucPortDDR = (ucDDR_Value & ucMask);                         // update data direction
        iRtn = PORT_CHANGE;                                              // display refresh required
    }
    return iRtn;
}



extern int fnPortChanges(int iForce)                                     // {50}
{
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {74}
    unsigned char ucGPTmask = 0xff;
#endif
    unsigned char ucMask;
    int iRtn = iFlagRefresh;
    iFlagRefresh = 0;
#if defined _M52XX_SDRAM                                                 // {59}
    ucMask = fnGetPorts(_PORT_A);
    if (ucPerA != (~ucMask & 0xff)) {
        ucPerA = (~ucMask & 0xff);
        fnSetNewPer(_PORT_A);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTA & DDRA & ucMask) != ucPortA) {
        ucPortA = (PORTA & DDRA & ucMask);
        ucPortDDR_A = (DDRA & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_A != (DDRA & ucMask)) {
        ucPortDDR_A = (DDRA & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_B);
    if (ucPerB != (~ucMask & 0xff)) {
        ucPerB = (~ucMask & 0xff);
        fnSetNewPer(_PORT_B);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTB & DDRB & ucMask) != ucPortB) {
        ucPortB = (PORTB & DDRB & ucMask);
        ucPortDDR_B = (DDRB & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_B != (DDRB & ucMask)) {
        ucPortDDR_B = (DDRB & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_C);
    if (ucPerC != (~ucMask & 0xff)) {
        ucPerC = (~ucMask & 0xff);
        fnSetNewPer(_PORT_C);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTC & DDRC & ucMask) != ucPortC) {
        ucPortC = (PORTC & DDRC & ucMask);
        ucPortDDR_C = (DDRC & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_C != (DDRC & ucMask)) {
        ucPortDDR_C = (DDRC & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_D);
    if (ucPerD != (~ucMask & 0xff)) {
        ucPerD = (~ucMask & 0xff);
        fnSetNewPer(_PORT_D);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTD & DDRD & ucMask) != ucPortD) {
        ucPortD = (PORTD & DDRD & ucMask);
        ucPortDDR_D = (DDRD & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_D != (DDRD & ucMask)) {
        ucPortDDR_D = (DDRD & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_E);
    if (ucPerE != (~ucMask & 0xff)) {
        ucPerE = (~ucMask & 0xff);
        fnSetNewPer(_PORT_E);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTE & DDRE & ucMask) != ucPortE) {
        ucPortE = (PORTE & DDRE & ucMask);
        ucPortDDR_E = (DDRE & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_E != (DDRE & ucMask)) {
        ucPortDDR_E = (DDRE & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_F);
    if (ucPerF != (~ucMask & 0xff)) {
        ucPerF = (~ucMask & 0xff);
        fnSetNewPer(_PORT_F);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTF & DDRF & ucMask) != ucPortF) {
        ucPortF = (PORTF & DDRF & ucMask);
        ucPortDDR_F = (DDRF & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_F != (DDRF & ucMask)) {
        ucPortDDR_F = (DDRF & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_G);
    if (ucPerG != (~ucMask & 0xff)) {
        ucPerG = (~ucMask & 0xff);
        fnSetNewPer(_PORT_G);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTG & DDRG & ucMask) != ucPortG) {
        ucPortG = (PORTG & DDRG & ucMask);
        ucPortDDR_G = (DDRG & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_G != (DDRG & ucMask)) {
        ucPortDDR_G = (DDRG & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_H);
    if (ucPerH != (~ucMask & 0xff)) {
        ucPerH = (~ucMask & 0xff);
        fnSetNewPer(_PORT_H);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTH & DDRH & ucMask) != ucPortH) {
        ucPortH = (PORTH & DDRH & ucMask);
        ucPortDDR_H = (DDRH & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_H != (DDRH & ucMask)) {
        ucPortDDR_H = (DDRH & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_J);
    if (ucPerJ != (~ucMask & 0xff)) {
        ucPerJ = (~ucMask & 0xff);
        fnSetNewPer(_PORT_J);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTJ & DDRJ & ucMask) != ucPortJ) {
        ucPortJ = (PORTJ & DDRJ & ucMask);
        ucPortDDR_J = (DDRJ & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_J != (DDRJ & ucMask)) {
        ucPortDDR_J = (DDRJ & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_EH);
    if (ucPerEH != (~ucMask & 0xff)) {
        ucPerEH = (~ucMask & 0xff);
        fnSetNewPer(_PORT_EH);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTEH & DDREH & ucMask) != ucPortEH) {
        ucPortEH = (PORTEH & DDREH & ucMask);
        ucPortDDR_EH = (DDREH & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_EH != (DDREH & ucMask)) {
        ucPortDDR_EH = (DDREH & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_EL);
    if (ucPerEL != (~ucMask & 0xff)) {
        ucPerEL = (~ucMask & 0xff);
        fnSetNewPer(_PORT_EL);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTEL & DDREL & ucMask) != ucPortEL) {
        ucPortEL = (PORTEL & DDREL & ucMask);
        ucPortDDR_EL = (DDREL & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_EL != (DDREL & ucMask)) {
        ucPortDDR_EL = (DDREL & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_QA);
    if (ucPerQA != (~ucMask & 0xff)) {
        ucPerQA = (~ucMask & 0xff);
        fnSetNewPer(_PORT_QA);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTQA & DDRQA & ucMask) != ucPortQA) {
        ucPortQA = (PORTQA & DDRQA & ucMask);
        ucPortDDR_QA = (DDRQA & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_QA != (DDRQA & ucMask)) {
        ucPortDDR_QA = (DDRQA & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_QB);
    if (ucPerQB != (~ucMask & 0xff)) {
        ucPerQB = (~ucMask & 0xff);
        fnSetNewPer(_PORT_QB);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTQB & DDRQB & ucMask) != ucPortQB) {
        ucPortQB = (PORTQB & DDRQB & ucMask);
        ucPortDDR_QB = (DDRQB & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_QB != (DDRQB & ucMask)) {
        ucPortDDR_QB = (DDRQB & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_TA);
    if (ucPerTA != (~ucMask & 0xff)) {
        ucPerTA = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TA);
        iRtn |= PORT_CHANGE;
    }
    if ((GPTPORT & GPTDDR & ucMask) != ucPortTA) {
        ucPortTA = (GPTPORT & GPTDDR & ucMask);
        ucPortDDR_TA = (GPTDDR & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TA != (GPTDDR & ucMask)) {
        ucPortDDR_TA = (GPTDDR & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_TB);
    if (ucPerTB != (~ucMask & 0xff)) {
        ucPerTB = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TB);
        iRtn |= PORT_CHANGE;
    }
    if ((GPT1PORT & GPT1DDR & ucMask) != ucPortTB) {
        ucPortTB = (GPT1PORT & GPT1DDR & ucMask);
        ucPortDDR_TB = (GPT1DDR & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TB != (GPT1DDR & ucMask)) {
        ucPortDDR_TB = (GPT1DDR & ucMask);
        iRtn |= PORT_CHANGE;
    }
#elif defined _M520X                                                     // {61}
    ucMask = fnGetPorts(_PORT_FECH);
    if (ucPerFECH != (~ucMask & 0xff)) {
        ucPerFECH = (~ucMask & 0xff);
        fnSetNewPer(_PORT_FECH);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTFECH & DDRFECH & ucMask) != ucPortFECH) {
        ucPortFECH = (PORTFECH & DDRFECH & ucMask);
        ucPortDDR_FECH = (DDRFECH & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_FECH != (DDRFECH & ucMask)) {
        ucPortDDR_FECH = (DDRFECH & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_FECL);
    if (ucPerFECL != (~ucMask & 0xff)) {
        ucPerFECL = (~ucMask & 0xff);
        fnSetNewPer(_PORT_FECL);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTFECL & DDRFECL & ucMask) != ucPortFECL) {
        ucPortFECL = (PORTFECL & DDRFECL & ucMask);
        ucPortDDR_FECL = (DDRFECL & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_FECL != (DDRFECL & ucMask)) {
        ucPortDDR_FECL = (DDRFECL & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_BUSCTL);
    if (ucPerBUSCTL != (~ucMask & 0xff)) {
        ucPerBUSCTL = (~ucMask & 0xff);
        fnSetNewPer(_PORT_BUSCTL);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTBUSCTL & DDRBUSCTL & ucMask) != ucPortBUSCTL) {
        ucPortBUSCTL = (PORTBUSCTL & DDRBUSCTL & ucMask);
        ucPortDDR_BUSCTL = (DDRBUSCTL & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_BUSCTL != (DDRBUSCTL & ucMask)) {
        ucPortDDR_BUSCTL = (DDRBUSCTL & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_BE);
    if (ucPerBE != (~ucMask & 0xff)) {
        ucPerBE = (~ucMask & 0xff);
        fnSetNewPer(_PORT_BE);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTBE & DDRBE & ucMask) != ucPortBE) {
        ucPortBE = (PORTBE & DDRBE & ucMask);
        ucPortDDR_BE = (DDRBE & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_BE != (DDRBE & ucMask)) {
        ucPortDDR_BE = (DDRBE & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_CS);
    if (ucPerCS != (~ucMask & 0xff)) {
        ucPerCS = (~ucMask & 0xff);
        fnSetNewPer(_PORT_CS);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTCS & DDRCS & ucMask) != ucPortCS) {
        ucPortCS = (PORTCS & DDRCS & ucMask);
        ucPortDDR_CS = (DDRCS & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_CS != (DDRCS & ucMask)) {
        ucPortDDR_CS = (DDRCS & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_FECI2C);
    if (ucPerFECI2C != (~ucMask & 0xff)) {
        ucPerFECI2C = (~ucMask & 0xff);
        fnSetNewPer(_PORT_FECI2C);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTFECI2C & DDRFECI2C & ucMask) != ucPortFECI2C) {
        ucPortFECI2C = (PORTFECI2C & DDRFECI2C & ucMask);
        ucPortDDR_FECI2C = (DDRFECI2C & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_FECI2C != (DDRFECI2C & ucMask)) {
        ucPortDDR_FECI2C = (DDRFECI2C & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_UART);
    if (ucPerUART != (~ucMask & 0xff)) {
        ucPerUART = (~ucMask & 0xff);
        fnSetNewPer(_PORT_UART);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTUART & DDRUART & ucMask) != ucPortUART) {
        ucPortUART = (PORTUART & DDRUART & ucMask);
        ucPortDDR_UART = (DDRUART & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_UART != (DDRUART & ucMask)) {
        ucPortDDR_UART = (DDRUART & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_QSPI);
    if (ucPerQSPI != (~ucMask & 0xff)) {
        ucPerQSPI = (~ucMask & 0xff);
        fnSetNewPer(_PORT_QSPI);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTQSPI & DDRQSPI & ucMask) != ucPortQSPI) {
        ucPortQSPI = (PORTQSPI & DDRQSPI & ucMask);
        ucPortDDR_QSPI = (DDRQSPI & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_QSPI != (DDRQSPI & ucMask)) {
        ucPortDDR_QSPI = (DDRQSPI & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_TIMER);
    if (ucPerTIMER != (~ucMask & 0xff)) {
        ucPerTIMER = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TIMER);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTIMER & DDRTIMER & ucMask) != ucPortTIMER) {
        ucPortTIMER = (PORTTIMER & DDRTIMER & ucMask);
        ucPortDDR_TIMER = (DDRTIMER & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TIMER != (DDRTIMER & ucMask)) {
        ucPortDDR_TIMER = (DDRTIMER & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucMask = fnGetPorts(_PORT_IRQ);
    if (ucPerIRQ != (~ucMask & 0xff)) {
        ucPerIRQ = (~ucMask & 0xff);
        fnSetNewPer(_PORT_IRQ);
        iRtn |= PORT_CHANGE;
    }
#elif defined _M523X                                                     // {65}
    iRtn |= fnUpdatePort(&ucPerADDR,   &ucPortADDR,   &ucPortDDR_ADDR,   PORTADDR,   DDRADDR,   _PORT_ADDR);
    iRtn |= fnUpdatePort(&ucPerBS,     &ucPortBS,     &ucPortDDR_BS,     PORTBS,     DDRBS,     _PORT_BS);
    iRtn |= fnUpdatePort(&ucPerBUSCTL, &ucPortBUSCTL, &ucPortDDR_BUSCTL, PORTBUSCTL, DDRBUSCTL, _PORT_BUSCTL);
    iRtn |= fnUpdatePort(&ucPerCS,     &ucPortCS,     &ucPortDDR_CS,     PORTCS,     DDRCS,     _PORT_CS);
    iRtn |= fnUpdatePort(&ucPerDATAH,  &ucPortDATAH,  &ucPortDDR_DATAH,  PORTDATAH,  DDRDATAH,  _PORT_DATAH);
    iRtn |= fnUpdatePort(&ucPerDATAL,  &ucPortDATAL,  &ucPortDDR_DATAL,  PORTDATAL,  DDRDATAL,  _PORT_DATAL);
    iRtn |= fnUpdatePort(&ucPerFECI2C, &ucPortFECI2C, &ucPortDDR_FECI2C, PORTFECI2C, DDRFECI2C, _PORT_FECI2C);
    iRtn |= fnUpdatePort(&ucPerETPU,   &ucPortETPU,   &ucPortDDR_ETPU,   PORTETPU,   DDRETPU,   _PORT_ETPU);
  //iRtn |= fnUpdatePort(&ucPerIRQ,    &ucPortIRQ,    &ucPortDDR_IRQ,    PORTIRQ,    DDRIRQ,    _PORT_IRQ); IRQ is not a full port
    iRtn |= fnUpdatePort(&ucPerQSPI,   &ucPortQSPI,   &ucPortDDR_QSPI,   PORTQSPI,   DDRQSPI,   _PORT_QSPI);
    iRtn |= fnUpdatePort(&ucPerSDRAM,  &ucPortSDRAM,  &ucPortDDR_SDRAM,  PORTSDRAM,  DDRSDRAM,  _PORT_SDRAM);
    iRtn |= fnUpdatePort(&ucPerTIMER,  &ucPortTIMER,  &ucPortDDR_TIMER,  PORTTIMER,  DDRTIMER,  _PORT_TIMER);
    iRtn |= fnUpdatePort(&ucPerUARTH,  &ucPortUARTH,  &ucPortDDR_UARTH,  PORTUARTH,  DDRUARTH,  _PORT_UARTH);
    iRtn |= fnUpdatePort(&ucPerUARTL,  &ucPortUARTL,  &ucPortDDR_UARTL,  PORTUARTL,  DDRUARTL,  _PORT_UARTL);

    // Handle edge port interrupts                                       {67}
    //
    if (EPPDR0 != ucPort_in_IRQ) {                                       // possible edge sensitive change
        unsigned char ucChange = (EPPDR0 ^ ucPort_in_IRQ);               // changed bits
        EPPDR0 = ucPort_in_IRQ;                                          // new input states
        if ((EPIER0 & 0x80) && (ucChange & 0x80)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x80) && (EPPAR0 & 0x4000)) {            // both or rising edge
                fnIRQ_int(IRQ7_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x80)) && (EPPAR0 & 0x8000)) {    // both or falling edge
                fnIRQ_int(IRQ7_VECTOR);
            }
        }
        if ((EPIER0 & 0x40) && (ucChange & 0x40)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x40) && (EPPAR0 & 0x1000)) {            // both or rising edge
                fnIRQ_int(IRQ6_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x40)) && (EPPAR0 & 0x2000)) {    // both or falling edge
                fnIRQ_int(IRQ6_VECTOR);
            }
        }    
        if ((EPIER0 & 0x20) && (ucChange & 0x20)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x20) && (EPPAR0 & 0x0400)) {            // both or rising edge
                fnIRQ_int(IRQ5_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x20)) && (EPPAR0 & 0x0800)) {    // both or falling edge
                fnIRQ_int(IRQ5_VECTOR);
            }
        }
        if ((EPIER0 & 0x10) && (ucChange & 0x10)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x10) && (EPPAR0 & 0x0100)) {            // both or rising edge
                fnIRQ_int(IRQ4_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x10)) && (EPPAR0 & 0x0200)) {    // both or falling edge
                fnIRQ_int(IRQ4_VECTOR);
            }
        }
        if ((EPIER0 & 0x08) && (ucChange & 0x08)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x08) && (EPPAR0 & 0x0040)) {            // both or rising edge
                fnIRQ_int(IRQ3_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x08)) && (EPPAR0 & 0x0080)) {    // both or falling edge
                fnIRQ_int(IRQ3_VECTOR);
            }
        }
        if ((EPIER0 & 0x04) && (ucChange & 0x04)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x04) && (EPPAR0 & 0x0010)) {            // both or rising edge
                fnIRQ_int(IRQ2_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x04)) && (EPPAR0 & 0x0020)) {    // both or falling edge
                fnIRQ_int(IRQ2_VECTOR);
            }
        }
        if ((EPIER0 & 0x02) && (ucChange & 0x02)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x02) && (EPPAR0 & 0x0004)) {            // both or rising edge
                fnIRQ_int(IRQ1_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x02)) && (EPPAR0 & 0x0008)) {    // both or falling edge
                fnIRQ_int(IRQ1_VECTOR);
            }
        }
        iRtn |= PORT_CHANGE;
    }
#else
    ucMask = fnGetPorts(_PORT_AN);
    if (ucPerAN != (~ucMask & 0xff)) {
        ucPerAN = (~ucMask & 0xff);
        fnSetNewPer(_PORT_AN);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTAN & DDRAN & ucMask) != ucPortAN) {
        ucPortAN = (PORTAN & DDRAN & ucMask);
        ucPortDDR_AN = (DDRAN & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_AN != (DDRAN & ucMask)) {
        ucPortDDR_AN = (DDRAN & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    ucMask = fnGetPorts(_PORT_QS);
    if (ucPerQS != (~ucMask & 0x7f)) {
        ucPerQS = (~ucMask & 0x7f);
        fnSetNewPer(_PORT_QS);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTQS & DDRQS & ucMask) != ucPortQS) {
        ucPortQS = (PORTQS & DDRQS & ucMask);
        ucPortDDR_QS = (DDRQS & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_QS != (DDRQS & ucMask)) {
        ucPortDDR_QS = (DDRQS & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    ucMask = fnGetPorts(_PORT_TA);
    fnCheckGPT(&ucGPTmask);                                              // {42}{74} if GPT is in operation the timer can override a pin function
    ucMask &= ucGPTmask;                                                 // remove any port bits that are configured as GPT function
    if (ucPerTA != (~ucMask & 0x0f)) {                                   // peripheral configuration change
        ucPerTA = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_TA);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTA & DDRTA & ucMask) != ucPortTA) {                         // output drive change
        ucPortTA = (PORTTA & DDRTA & ucMask);
        ucPortDDR_TA = (DDRTA & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TA != (DDRTA & ucMask)) {                         // DDR setting change
        ucPortDDR_TA = (DDRTA & ucMask);
        iRtn |= PORT_CHANGE;
    }
    ucGPTmask = ~ucGPTmask;
    if ((GPTPORT & GPTDDR & ucGPTmask) != ucPortTA) {                    // {74} GPT output state change
        ucPortTA = (GPTPORT & GPTDDR & ucGPTmask);
        ucPortDDR_TA = (GPTDDR & ucGPTmask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TA != (GPTDDR & ucGPTmask)) {                     // GPT DDR setting change
        ucPortDDR_TA = (GPTDDR & ucGPTmask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    ucMask = fnGetPorts(_PORT_TC);
    if (ucPerTC != (~ucMask & 0x0f)) {
        ucPerTC = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_TC);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTC & DDRTC & ucMask) != ucPortTC) {
        ucPortTC = (PORTTC & DDRTC & ucMask);
        ucPortDDR_TC = (DDRTC & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_TC != (DDRTC & ucMask)) {
        ucPortDDR_TC = (DDRTC & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M5225X && !defined _M520X && !defined _M523X               // {39}{61}
    ucMask = fnGetPorts(_PORT_TD);
    if (ucPerTD != (~ucMask & 0x0f)) {
        ucPerTD = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_TD);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTD & DDRTD & ucMask) != ucPortTD) {
        ucPortTD = (PORTTD & DDRTD & ucMask);
        ucPortDDR_TD = (DDRTD & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_TD != (DDRTD & ucMask)) {
        ucPortDDR_TD = (DDRTD & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M521X && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {39}{59}{61}
    ucMask = fnGetPorts(_PORT_LD);
    if (ucPerLD != (~ucMask & 0x7f)) {
        ucPerLD = (~ucMask & 0x7f);
        fnSetNewPer(_PORT_LD);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTLD & DDRLD & ucMask) != ucPortLD) {
        ucPortLD = (PORTLD & DDRLD & ucMask);
        ucPortDDR_LD = (DDRLD & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_LD != (DDRLD & ucMask)) {
        ucPortDDR_LD = (DDRLD & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M521X && !defined _M520X && !defined _M523X                // {61}
    ucMask = fnGetPorts(_PORT_DD);                                       // {10}
    if (ucPerDD != (~ucMask & 0xff)) {
        ucPerDD = (~ucMask & 0xff);
        fnSetNewPer(_PORT_DD);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTDD & DDRDD & ucMask) != ucPortDD) {
        ucPortDD = (PORTDD & DDRDD & ucMask);
        ucPortDDR_DD = (DDRDD & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_DD != (DDRDD & ucMask)) {
        ucPortDDR_DD = (DDRDD & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    ucMask = fnGetPorts(_PORT_UA);
    if (ucPerUA != (~ucMask & 0x0f)) {
        ucPerUA = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_UA);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTUA & DDRUA & ucMask) != ucPortUA) {
        ucPortUA = (PORTUA & DDRUA & ucMask);
        ucPortDDR_UA = (DDRUA & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_UA != (DDRUA & ucMask)) {
        ucPortDDR_UA = (DDRUA & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}{64} [inverse defined _M52XX_SDRAM]
    ucMask = fnGetPorts(_PORT_UB);
    if (ucPerUB != (~ucMask & 0x0f)) {
        ucPerUB = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_UB);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTUB & DDRUB & ucMask) != ucPortUB) {
        ucPortUB = (PORTUB & DDRUB & ucMask);
        ucPortDDR_UB = (DDRUB & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_UB != (DDRUB & ucMask)) {
        ucPortDDR_UB = (DDRUB & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_UC);
    if (ucPerUC != (~ucMask & 0x0f)) {
        ucPerUC = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_UC);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTUC & DDRUC & ucMask) != ucPortUC) {
        ucPortUC = (PORTUC & DDRUC & ucMask);
        ucPortDDR_UC = (DDRUC & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_UC != (DDRUC & ucMask)) {
        ucPortDDR_UC = (DDRUC & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    ucMask = fnGetPorts(_PORT_AS);
    #if defined _M52XX_SDRAM                                             // {59}
    if (ucPerAS != (~ucMask & 0x3f)) {
        ucPerAS = (~ucMask & 0x3f);
        fnSetNewPer(_PORT_AS);
        iRtn |= PORT_CHANGE;
    }
    #else
    if (ucPerAS != (~ucMask & 0x0f)) {
        ucPerAS = (~ucMask & 0x0f);
        fnSetNewPer(_PORT_AS);
        iRtn |= PORT_CHANGE;
    }
    #endif
    if ((PORTAS & DDRAS & ucMask) != ucPortAS) {
        ucPortAS = (PORTAS & DDRAS & ucMask);
        ucPortDDR_AS = (DDRAS & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_AS != (DDRAS & ucMask)) {
        ucPortDDR_AS = (DDRAS & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif
#if defined _M52XX_SDRAM                                                 // {59}
    ucMask = fnGetPorts(_PORT_NQ);
    if (ucPerNQ != (~ucMask & 0xfe)) {
        ucPerNQ = (~ucMask & 0xfe);
        fnSetNewPer(_PORT_NQ);
        iRtn |= PORT_CHANGE;
    }
    if ((EPDR0 & EPDDR0 & ucMask) != ucPortNQ) {                         // change of output detected?
        ucPortNQ = (EPDR0 & EPDDR0 & ucMask);
        ucPortDDR_NQ = (EPDDR0 & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_NQ != (EPDDR0 & ucMask)) {                        // change of DDR detected?
        ucPortDDR_NQ = (EPDDR0 & ucMask);
        iRtn |= PORT_CHANGE;
    }
#elif !defined _M520X && !defined _M523X                                 // {61}
    ucMask = fnGetPorts(_PORT_NQ);
    if (ucPerNQ != (~ucMask & 0xfe)) {
        ucPerNQ = (~ucMask & 0xfe);
        fnSetNewPer(_PORT_NQ);
        PORTIN_SETNQ = (_irqPORTIN_SETNQ | ucIRQ_P0);                    // {69} synchronise the port input value to the pin input value but ensure that any IRQ functions are kept at '1'
        iRtn |= PORT_CHANGE;
    }
    if ((PORTNQ & DDRNQ & ucMask) != ucPortNQ) {                         // change of output detected?
        ucPortNQ = (PORTNQ & DDRNQ & ucMask);
        ucPortDDR_NQ = (DDRNQ & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_NQ != (DDRNQ & ucMask)) {                         // change of DDR detected?
        ucPortDDR_NQ = (DDRNQ & ucMask);
        iRtn |= PORT_CHANGE;
    }

    #if !defined _M5225X && !defined _M521X                              // {39}
    ucMask = fnGetPorts(_PORT_GP);
    if (ucPerGP != (~ucMask & 0xff)) {
        ucPerGP = ~ucMask;                                               // {56}
        fnSetNewPer(_PORT_GP);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTGP & DDRGP & ucMask) != ucPortGP) {                         // change of output detected?
        ucPortGP = (PORTGP & DDRGP & ucMask);
        ucPortDDR_GP = (DDRGP & ucMask);
        iRtn |= PORT_CHANGE;
    }  
    else if (ucPortDDR_GP != (DDRGP & ucMask)) {                         // change of DDR detected?
        ucPortDDR_GP = (DDRGP & ucMask);
        iRtn |= PORT_CHANGE;
    }
    #endif
#endif

#if defined _M5225X                                                     // {39}
    ucMask = fnGetPorts(_PORT_TE);
    if (ucPerTE != (~ucMask & 0xff)) {
        ucPerTE = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TE);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTE & DDRTE & ucMask) != ucPortTE) {
        ucPortTE = (PORTTE & DDRTE & ucMask);
        ucPortDDR_TE = (DDRTE & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TE != (DDRTE & ucMask)) {
        ucPortDDR_TE = (DDRTE & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_TF);
    if (ucPerTF != (~ucMask & 0xff)) {
        ucPerTF = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TF);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTF & DDRTF & ucMask) != ucPortTF) {
        ucPortTF = (PORTTF & DDRTF & ucMask);
        ucPortDDR_TF = (DDRTF & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TF != (DDRTF & ucMask)) {
        ucPortDDR_TF = (DDRTF & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_TG);
    if (ucPerTG != (~ucMask & 0xff)) {
        ucPerTG = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TG);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTG & DDRTG & ucMask) != ucPortTG) {
        ucPortTG = (PORTTG & DDRTG & ucMask);
        ucPortDDR_TG = (DDRTG & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TG != (DDRTG & ucMask)) {
        ucPortDDR_TG = (DDRTG & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_TH);
    if (ucPerTH != (~ucMask & 0xff)) {
        ucPerTH = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TH);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTH & DDRTH & ucMask) != ucPortTH) {
        ucPortTH = (PORTTH & DDRTH & ucMask);
        ucPortDDR_TH = (DDRTH & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TH != (DDRTH & ucMask)) {
        ucPortDDR_TH = (DDRTH & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_TI);
    if (ucPerTI != (~ucMask & 0xff)) {
        ucPerTI = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TI);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTI & DDRTI & ucMask) != ucPortTI) {
        ucPortTI = (PORTTI & DDRTI & ucMask);
        ucPortDDR_TI = (DDRTI & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TI != (DDRTI & ucMask)) {
        ucPortDDR_TI = (DDRTI & ucMask);
        iRtn |= PORT_CHANGE;
    }

    ucMask = fnGetPorts(_PORT_TJ);
    if (ucPerTJ != (~ucMask & 0xff)) {
        ucPerTJ = (~ucMask & 0xff);
        fnSetNewPer(_PORT_TJ);
        iRtn |= PORT_CHANGE;
    }
    if ((PORTTJ & DDRTJ & ucMask) != ucPortTJ) {
        ucPortTJ = (PORTTJ & DDRTJ & ucMask);
        ucPortDDR_TJ = (DDRTJ & ucMask);
        iRtn |= PORT_CHANGE;
    }
    else if (ucPortDDR_TJ != (DDRTJ & ucMask)) {
        ucPortDDR_TJ = (DDRTJ & ucMask);
        iRtn |= PORT_CHANGE;
    }
#endif

    // Interrupt handling on input changes - if interrupt is enabled
    //
#if defined _M52XX_SDRAM
    if ((~ucPortDDR_NQ & EPPDR0) != (~ucPortDDR_NQ & ucPort_in_NQ)) {    // change of input detected?
        unsigned char ucChange = ucPort_in_NQ;
        ucPort_in_NQ = (~ucPortDDR_NQ & EPPDR0);
        ucChange ^= ucPort_in_NQ;                                        // determine changed bit
        if ((EPIER0 & 0x80) && (ucChange & 0x80)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x80) && (EPPAR0 & 0x4000)) {            // both or rising edge
                fnIRQ_int(IRQ7_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x80)) && (EPPAR0 & 0x8000)) {    // both or falling edge
                fnIRQ_int(IRQ7_VECTOR);
            }
        }
        if ((EPIER0 & 0x40) && (ucChange & 0x40)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x40) && (EPPAR0 & 0x1000)) {            // both or rising edge
                fnIRQ_int(IRQ6_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x40)) && (EPPAR0 & 0x2000)) {    // both or falling edge
                fnIRQ_int(IRQ6_VECTOR);
            }
        }    
        if ((EPIER0 & 0x20) && (ucChange & 0x20)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x20) && (EPPAR0 & 0x0400)) {            // both or rising edge
                fnIRQ_int(IRQ5_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x20)) && (EPPAR0 & 0x0800)) {    // both or falling edge
                fnIRQ_int(IRQ5_VECTOR);
            }
        }
        if ((EPIER0 & 0x10) && (ucChange & 0x10)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x10) && (EPPAR0 & 0x0100)) {            // both or rising edge
                fnIRQ_int(IRQ4_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x10)) && (EPPAR0 & 0x0200)) {    // both or falling edge
                fnIRQ_int(IRQ4_VECTOR);
            }
        }
        if ((EPIER0 & 0x08) && (ucChange & 0x08)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x08) && (EPPAR0 & 0x0040)) {            // both or rising edge
                fnIRQ_int(IRQ3_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x08)) && (EPPAR0 & 0x0080)) {    // both or falling edge
                fnIRQ_int(IRQ3_VECTOR);
            }
        }
        if ((EPIER0 & 0x04) && (ucChange & 0x04)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x04) && (EPPAR0 & 0x0010)) {            // both or rising edge
                fnIRQ_int(IRQ2_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x04)) && (EPPAR0 & 0x0020)) {    // both or falling edge
                fnIRQ_int(IRQ2_VECTOR);
            }
        }
        if ((EPIER0 & 0x02) && (ucChange & 0x02)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x02) && (EPPAR0 & 0x0004)) {            // both or rising edge
                fnIRQ_int(IRQ1_VECTOR);
            }
            else if ((!(ucPort_in_NQ & 0x02)) && (EPPAR0 & 0x0008)) {    // both or falling edge
                fnIRQ_int(IRQ1_VECTOR);
            }
        }
        iRtn |= PORT_CHANGE;
    }
#elif defined _M520X                                                     // {61}
    // To do
#elif defined _M523X                                                     // {65}
    // To do
#else
    if ((~ucPortDDR_NQ & _irqPORTIN_SETNQ) != (~ucPortDDR_NQ & ucPort_in_NQ)) { // change of input detected?
        unsigned char ucChange = ucPort_in_NQ;
        ucPort_in_NQ = (~ucPortDDR_NQ & _irqPORTIN_SETNQ);
        ucChange ^= ucPort_in_NQ;                                        // determine changed bit
        if ((EPIER0 & 0x80) && (ucChange & 0x80)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x80) && (EPPAR0 & 0x4000)) {            // both or rising edge
                fnIRQ_int(IRQ7_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x80)) && (EPPAR0 & 0x8000)) {    // both or falling edge
                fnIRQ_int(IRQ7_VECTOR);                                  // {22}
            }
        } 
    #if !defined CHIP_80_PIN && !defined CHIP_64_PIN                     // {52}
        if ((EPIER0 & 0x40) && (ucChange & 0x40)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x40) && (EPPAR0 & 0x1000)) {            // both or rising edge
                fnIRQ_int(IRQ6_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x40)) && (EPPAR0 & 0x2000)) {    // both or falling edge
                fnIRQ_int(IRQ6_VECTOR);                                  // {22}
            }
        }    
        if ((EPIER0 & 0x20) && (ucChange & 0x20)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x20) && (EPPAR0 & 0x0400)) {            // both or rising edge
                fnIRQ_int(IRQ5_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x20)) && (EPPAR0 & 0x0800)) {    // both or falling edge
                fnIRQ_int(IRQ5_VECTOR);                                  // {22}
            }
        }        
    #endif
        if ((EPIER0 & 0x10) && (ucChange & 0x10)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x10) && (EPPAR0 & 0x0100)) {            // both or rising edge
                fnIRQ_int(IRQ4_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x10)) && (EPPAR0 & 0x0200)) {    // both or falling edge
                fnIRQ_int(IRQ4_VECTOR);                                  // {22}
            }
        }
    #ifndef CHIP_80_PIN
        if ((EPIER0 & 0x08) && (ucChange & 0x08)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x08) && (EPPAR0 & 0x0040)) {            // both or rising edge
                fnIRQ_int(IRQ3_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x08)) && (EPPAR0 & 0x0080)) {    // both or falling edge
                fnIRQ_int(IRQ3_VECTOR);                                  // {22}
            }
        }
        if ((EPIER0 & 0x04) && (ucChange & 0x04)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x04) && (EPPAR0 & 0x0010)) {            // both or rising edge
                fnIRQ_int(IRQ2_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x04)) && (EPPAR0 & 0x0020)) {    // both or falling edge
                fnIRQ_int(IRQ2_VECTOR);                                  // {22}
            }
        }
    #endif
    #if defined SUPPORT_ADC                                              // {29}  handle SYNC inputs 
        #if defined SYNCA_ON_PNQ                                         // if ADC channel A set up to trigger on this input
            #if defined _M521X                                           // {52}
        if ((ucChange & 0x02) && (SYNC0 & ADC_CTRL1) && ((PNQPAR & 0x03) == 0x02)) {
            if (ucPort_in_NQ & 0x02) {                                   // positive edge
                fnTriggADC(0);
            }
        }
            #else
        if ((ucChange & 0x02) && (SYNC0 & ADC_CTRL1) && ((PNQPAR & (ALT_2_FUNCTION_Q << BIT_1_Q_SHIFT)) == (ALT_1_FUNCTION_Q << BIT_1_Q_SHIFT))) {
            if (ucPort_in_NQ & 0x02) {                                   // positive edge
                fnTriggADC(0);
            }
        }
            #endif
        #endif
    #endif
        if ((EPIER0 & 0x02) && (ucChange & 0x02)) {                      // interrupt enabled and input change
            if ((ucPort_in_NQ & 0x02) && (EPPAR0 & 0x0004)) {            // both or rising edge
                fnIRQ_int(IRQ1_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_NQ & 0x02)) && (EPPAR0 & 0x0008)) {    // both or falling edge
                fnIRQ_int(IRQ1_VECTOR);                                  // {22}
            }
        }
        iRtn |= PORT_CHANGE;
    } 
    #if !defined _M5225X && !defined _M521X                              // {39}    
    if ((~ucPortDDR_GP & PORTIN_SETGP) != (~ucPortDDR_GP & ucPort_in_GP)) { // change of input detected? {5}
        unsigned char ucChange = ucPort_in_GP;
        ucPort_in_GP = (~ucPortDDR_GP & PORTIN_SETGP);
        ucChange ^= ucPort_in_GP;                                        // determine changed bit
        #if !defined CHIP_80_PIN && !defined _M5222X && !defined _M5221X && !defined _M521XX
        if (EPIER1 & 0x80 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x80) && (EPPAR1 & 0x4000)) {            // both or rising edge
                fnIRQ_int(IRQ15_VECTOR);                                 // {22}
            }
            else if ((!(ucPort_in_GP & 0x80)) && (EPPAR1 & 0x8000)) {    // both or falling edge
                fnIRQ_int(IRQ15_VECTOR);                                 // {22}
            }
        }
        if (EPIER1 & 0x40 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x40) && (EPPAR1 & 0x1000)) {            // both or rising edge
                fnIRQ_int(IRQ14_VECTOR);                                 // {22}
            }
            else if ((!(ucPort_in_GP & 0x40)) && (EPPAR1 & 0x2000)) {    // both or falling edge
                fnIRQ_int(IRQ14_VECTOR);                                 // {22}
            }
        }
        if (EPIER1 & 0x20 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x20) && (EPPAR1 & 0x0400)) {            // both or rising edge
                fnIRQ_int(IRQ13_VECTOR);                                 // {22}
            }
            else if ((!(ucPort_in_GP & 0x20)) && (EPPAR1 & 0x0800)) {    // both or falling edge
                fnIRQ_int(IRQ13_VECTOR);                                 // {22}
            }
        }
        if (EPIER1 & 0x10 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x10) && (EPPAR1 & 0x0100)) {            // both or rising edge
                fnIRQ_int(IRQ12_VECTOR);                                 // {22}
            }
            else if ((!(ucPort_in_GP & 0x10)) && (EPPAR1 & 0x0200)) {    // both or falling edge
                fnIRQ_int(IRQ12_VECTOR);                                 // {22}
            }
        }
        #endif
        #if !defined _M5222X && !defined _M5221X && !defined _M521XX
        if (EPIER1 & 0x08 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x08) && (EPPAR1 & 0x0040)) {            // both or rising edge
                fnIRQ_int(IRQ11_VECTOR);                                 // {22}
            }
            else if ((!(ucPort_in_GP & 0x08)) && (EPPAR1 & 0x0080)) {    // both or falling edge
                fnIRQ_int(IRQ11_VECTOR);                                 // {22}
            }
        }
        #endif
        #if !defined CHIP_80_PIN && !defined _M5222X && !defined _M5221X && !defined _M521XX
        if (EPIER1 & 0x04 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x04) && (EPPAR1 & 0x0010)) {            // both or rising edge
                fnIRQ_int(IRQ10_VECTOR);                                 // {22}
            }
            else if ((!(ucPort_in_GP & 0x04)) && (EPPAR1 & 0x0020)) {    // both or falling edge
                fnIRQ_int(IRQ10_VECTOR);                                 // {22}
            }
        }
        if (EPIER1 & 0x02 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x02) && (EPPAR1 & 0x0004)) {            // both or rising edge
                fnIRQ_int(IRQ9_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_GP & 0x02)) && (EPPAR1 & 0x0008)) {    // both or falling edge
                fnIRQ_int(IRQ9_VECTOR);                                  // {22}
            }
        }
        if (EPIER1 & 0x01 & ucChange) {                                  // interrupt enabled and input change
            if ((ucPort_in_GP & 0x01) && (EPPAR1 & 0x0001)) {            // both or rising edge
                fnIRQ_int(IRQ8_VECTOR);                                  // {22}
            }
            else if ((!(ucPort_in_GP & 0x01)) && (EPPAR1 & 0x0002)) {    // both or falling edge
                fnIRQ_int(IRQ8_VECTOR);                                  // {22}
            }
        }
        #endif
    }
    #endif
    
    ucPort_in_NQ = (~ucPortDDR_NQ & _irqPORTIN_SETNQ);                   // handle level sensitive interrupts {5}
    #if !defined _M5225X && !defined _M521X                              // {39}
    ucPort_in_GP = (~ucPortDDR_GP & PORTIN_SETGP);
    #endif
    #if !defined _M5222X && !defined _M5221X && !defined _M5225X && !defined _M521XX && !defined _M521X // {39}
        #if !defined CHIP_80_PIN
    if (((EPPAR1 & 0xc000) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x80)) {
        fnIRQ_int(IRQ15_VECTOR);                                         // {22}
    }
    if (((EPPAR1 & 0x3000) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x40)) {
        fnIRQ_int(IRQ14_VECTOR);                                         // {22}
    }
    if (((EPPAR1 & 0x0c00) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x20)) {
        fnIRQ_int(IRQ13_VECTOR);                                         // {22}
    }
    if (((EPPAR1 & 0x0300) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x10)) {
        fnIRQ_int(IRQ12_VECTOR);                                         // {22}
    }
    if (((EPPAR1 & 0x00c0) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x08)) {
        fnIRQ_int(IRQ11_VECTOR);                                         // {22}
    }
    if (((EPPAR1 & 0x0030) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x04)) {
        fnIRQ_int(IRQ10_VECTOR);                                         // {22}
    }
    if (((EPPAR1 & 0x000c) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x02)) {
        fnIRQ_int(IRQ9_VECTOR);                                          // {22}
    }
    if (((EPPAR1 & 0x0003) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x01)) {
        fnIRQ_int(IRQ8_VECTOR);                                          // {22}
    }
        #else
    if (((EPPAR1 & 0x00c0) == 0x0000) && (EPIER1 & ~ucPort_in_GP & 0x08)) {
        fnIRQ_int(IRQ11_VECTOR);                                         // {22}
    }
        #endif
    #endif
#endif
#if defined _M523X                                                       // {67}
    #define INTERRUPT_INPUTS ucPort_in_IRQ
#else
    #define INTERRUPT_INPUTS ucPort_in_NQ
#endif
    if (((EPPAR0 & 0xc000) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x80)) {
        fnIRQ_int(IRQ7_VECTOR);                                          // {22}
    }
#if !defined CHIP_80_PIN && !defined _M520X                              // {61}
    if (((EPPAR0 & 0x3000) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x40)) {
        fnIRQ_int(IRQ6_VECTOR);                                          // {22}
    }
    if (((EPPAR0 & 0x0c00) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x20)) {
        fnIRQ_int(IRQ5_VECTOR);                                          // {22}
    }
#endif
    if (((EPPAR0 & 0x0300) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x10)) {
        fnIRQ_int(IRQ4_VECTOR);                                          // {22}
    }
#if !defined CHIP_80_PIN && !defined _M520X                              // {61}
    if (((EPPAR0 & 0x00c0) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x08)) {
        fnIRQ_int(IRQ3_VECTOR);                                          // {22}
    }
    if (((EPPAR0 & 0x0030) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x04)) {
        fnIRQ_int(IRQ2_VECTOR);                                          // {22}
    }
#endif
    if (((EPPAR0 & 0x000c) == 0x0000) && (EPIER0 & ~INTERRUPT_INPUTS & 0x02)) {
        fnIRQ_int(IRQ1_VECTOR);                                          // {22}
    }
    if (iForce != 0) {                                                   // {58} queue refreshes if forced
        iFlagRefresh = iRtn;
    }
    return iRtn;
}


extern unsigned long fnGetPresentPortState(int portNr)
{
    switch (portNr - 1) {
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_QS:
        return (ucPortQS & ucPortDDR_QS) | (~ucPortDDR_QS & ucPort_in_QS);
#endif
#if defined _M52XX_SDRAM                                                 // {59}
    case _PORT_A:
        if (CCM_RCON & RCON_MODE) {                                      // master mode
            return 0xff;
        }
        else {
            return (ucPortA & ucPortDDR_A) | (~ucPortDDR_A & ucPort_in_A);
        }
        break;
    case _PORT_B:
        if (PBCDPAR & PBCDPAR_PBPA) {
            return 0xff;
        }
        else {
            return (ucPortB & ucPortDDR_B) | (~ucPortDDR_B & ucPort_in_B);
        }
        break;
    case _PORT_C:
        if (PBCDPAR & PBCDPAR_PCDPA) {
            return 0xff;
        }
        else {
            return (ucPortC & ucPortDDR_C) | (~ucPortDDR_C & ucPort_in_C);
        }
        break;
    case _PORT_D:
        if (PBCDPAR & PBCDPAR_PCDPA) {
            return 0xff;
        }
        else {
            return (ucPortD & ucPortDDR_D) | (~ucPortDDR_D & ucPort_in_D);
        }
        break;
    case _PORT_E:
        return (ucPortE & ucPortDDR_E) | (~ucPortDDR_E & ucPort_in_E);
    case _PORT_G:
        if (CCM_RCON & RCON_MODE) {                                      // master mode
            return 0xff;
        }
        else {
            return (ucPortG & ucPortDDR_G) | (~ucPortDDR_G & ucPort_in_G);
        }
        break;
    case _PORT_H:
        if (CCM_RCON & RCON_MODE) {                                      // master mode
            return 0xff;
        }
        else {
            return (ucPortH & ucPortDDR_H) | (~ucPortDDR_H & ucPort_in_H);
        }
        break;
    case _PORT_J:
        return (ucPortJ & ucPortDDR_J) | (~ucPortDDR_J & ucPort_in_J);
    case _PORT_EH:
        return (ucPortEH & ucPortDDR_EH) | (~ucPortDDR_EH & ucPort_in_EH);
    case _PORT_EL:
        return (ucPortEL & ucPortDDR_EL) | (~ucPortDDR_EL & ucPort_in_EL);
    case _PORT_QA:
        return (ucPortQA & ucPortDDR_QA) | (~ucPortDDR_QA & ucPort_in_QA);
    case _PORT_QB:
        return (ucPortQB & ucPortDDR_QB) | (~ucPortDDR_QB & ucPort_in_QB);
    case _PORT_TB:
        return (ucPortTB & ucPortDDR_TB) | (~ucPortDDR_TB & ucPort_in_TB);
#elif defined _M520X                                                     // {61}
    case _PORT_FECH:
        return (ucPortFECH & ucPortDDR_FECH) | (~ucPortDDR_FECH & ucPort_in_FECH);
    case _PORT_FECL:
        return (ucPortFECL & ucPortDDR_FECL) | (~ucPortDDR_FECL & ucPort_in_FECL);
    case _PORT_BUSCTL:
        return (ucPortBUSCTL & ucPortDDR_BUSCTL) | (~ucPortDDR_BUSCTL & ucPort_in_BUSCTL);
    case _PORT_BE:
        return (ucPortBE & ucPortDDR_BE) | (~ucPortDDR_BE & ucPort_in_BE);
    case _PORT_CS:
        return (ucPortCS & ucPortDDR_CS) | (~ucPortDDR_CS & ucPort_in_CS);
    case _PORT_FECI2C:
        return (ucPortFECI2C & ucPortDDR_FECI2C) | (~ucPortDDR_FECI2C & ucPort_in_FECI2C);
    case _PORT_UART:
        return (ucPortUART & ucPortDDR_UART) | (~ucPortDDR_UART & ucPort_in_UART);
    case _PORT_QSPI:
        return (ucPortQSPI & ucPortDDR_QSPI) | (~ucPortDDR_QSPI & ucPort_in_QSPI);
    case _PORT_TIMER:
        return (ucPortTIMER & ucPortDDR_TIMER) | (~ucPortDDR_TIMER & ucPort_in_TIMER);
    case _PORT_IRQ:
        return (ucPortIRQ & ucPortDDR_IRQ) | (~ucPortDDR_IRQ & ucPort_in_IRQ);
#elif defined _M523X                                                     // {65}
    case _PORT_ADDR:
        return (ucPortADDR & ucPortDDR_ADDR) | (~ucPortDDR_ADDR & ucPort_in_ADDR);
    case _PORT_BS:
        return (ucPortBS & ucPortDDR_BS) | (~ucPortDDR_BS & ucPort_in_BS);
    case _PORT_BUSCTL:
        return (ucPortBUSCTL & ucPortDDR_BUSCTL) | (~ucPortDDR_BUSCTL & ucPort_in_BUSCTL);
    case _PORT_CS:
        return (ucPortCS & ucPortDDR_CS) | (~ucPortDDR_CS & ucPort_in_CS);
    case _PORT_DATAH:
        return (ucPortDATAH & ucPortDDR_DATAH) | (~ucPortDDR_DATAH & ucPort_in_DATAH);
    case _PORT_DATAL:
        return (ucPortDATAL & ucPortDDR_DATAL) | (~ucPortDDR_DATAL & ucPort_in_DATAL);
    case _PORT_FECI2C:
        return (ucPortFECI2C & ucPortDDR_FECI2C) | (~ucPortDDR_FECI2C & ucPort_in_FECI2C);
    case _PORT_ETPU:
        return (ucPortETPU & ucPortDDR_ETPU) | (~ucPortDDR_ETPU & ucPort_in_ETPU);
    case _PORT_QSPI:
        return (ucPortQSPI & ucPortDDR_QSPI) | (~ucPortDDR_QSPI & ucPort_in_QSPI);
    case _PORT_SDRAM:
        return (ucPortSDRAM & ucPortDDR_SDRAM) | (~ucPortDDR_SDRAM & ucPort_in_SDRAM);
    case _PORT_TIMER:
        return (ucPortTIMER & ucPortDDR_TIMER) | (~ucPortDDR_TIMER & ucPort_in_TIMER);
    case _PORT_UARTH:
        return (ucPortUARTH & ucPortDDR_UARTH) | (~ucPortDDR_UARTH & ucPort_in_UARTH);
    case _PORT_UARTL:
        return (ucPortUARTL & ucPortDDR_UARTL) | (~ucPortDDR_UARTL & ucPort_in_UARTL);
    case _PORT_IRQ:
        return (EPPDR0);
#else
    case _PORT_AN:
        return (ucPortAN & ucPortDDR_AN) | (~ucPortDDR_AN & ucPort_in_AN);
    #if !defined _M5225X && !defined _M521X                              // {39}
    case _PORT_LD:
        return (ucPortLD & ucPortDDR_LD) | (~ucPortDDR_LD & ucPort_in_LD);
    #endif
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_NQ:
        return (ucPortNQ & ucPortDDR_NQ) | (~ucPortDDR_NQ & ucPort_in_NQ);
    case _PORT_TA:
        return (ucPortTA & ucPortDDR_TA) | (~ucPortDDR_TA & ucPort_in_TA);
    case _PORT_TC:
        return (ucPortTC & ucPortDDR_TC) | (~ucPortDDR_TC & ucPort_in_TC);
#endif
#if !defined _M5225X && !defined _M520X && !defined _M523X               // {39}{61}
    case _PORT_TD:
        return (ucPortTD & ucPortDDR_TD) | (~ucPortDDR_TD & ucPort_in_TD);
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_UA:
        return (ucPortUA & ucPortDDR_UA) | (~ucPortDDR_UA & ucPort_in_UA);
    #ifndef _M52XX_SDRAM                                                 // {59}
    case _PORT_UB:
        return (ucPortUB & ucPortDDR_UB) | (~ucPortDDR_UB & ucPort_in_UB);
    case _PORT_UC:
        return (ucPortUC & ucPortDDR_UC) | (~ucPortDDR_UC & ucPort_in_UC);
    #endif
    case _PORT_AS:
        return (ucPortAS & ucPortDDR_AS) | (~ucPortDDR_AS & ucPort_in_AS);
    #if !defined _M5225X && !defined _M521X && !defined _M52XX_SDRAM     // {39}{59}
    case _PORT_GP:
        return (ucPortGP & ucPortDDR_GP) | (~ucPortDDR_GP & ucPort_in_GP);
    #endif
    case _PORT_DD:
        return (ucPortDD & ucPortDDR_DD) | (~ucPortDDR_DD & ucPort_in_DD);
#endif
#if defined _M5225X                                                      // {39}
    case _PORT_TE:
        return (ucPortTE & ucPortDDR_TE) | (~ucPortDDR_TE & ucPort_in_TE);
    case _PORT_TF:
        return (ucPortTF & ucPortDDR_TF) | (~ucPortDDR_TF & ucPort_in_TF);
    case _PORT_TG:
        return (ucPortTG & ucPortDDR_TG) | (~ucPortDDR_TG & ucPort_in_TG);  
    case _PORT_TH:
        return (ucPortTH & ucPortDDR_TH) | (~ucPortDDR_TH & ucPort_in_TH);
    case _PORT_TI:
        return (ucPortTI & ucPortDDR_TI) | (~ucPortDDR_TI & ucPort_in_TI);
    case _PORT_TJ:
        return (ucPortTJ & ucPortDDR_TJ) | (~ucPortDDR_TJ & ucPort_in_TJ);  
#endif
    }
    return 0;
}

extern unsigned long fnGetPresentPortDir(int portNr)
{
    switch (portNr - 1) {
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_QS:
        return ucPortDDR_QS;
#endif
#if defined _M52XX_SDRAM                                                 // {59}
    case _PORT_A:
        return ucPortDDR_A;
    case _PORT_B:
        return ucPortDDR_B;
    case _PORT_C:
        return ucPortDDR_C;
    case _PORT_D:
        return ucPortDDR_D;
    case _PORT_E:
        return ucPortDDR_E;
    case _PORT_F:
        return ucPortDDR_F;
    case _PORT_G:
        return ucPortDDR_G;
    case _PORT_H:
        return ucPortDDR_H;
    case _PORT_J:
        return ucPortDDR_J;
    case _PORT_EH:
        return ucPortDDR_EH;
    case _PORT_EL:
        return ucPortDDR_EL;
#elif defined _M520X                                                     // {61}
    case _PORT_FECH:
        return ucPortDDR_FECH;
    case _PORT_FECL:
        return ucPortDDR_FECL;
    case _PORT_BUSCTL:
        return ucPortDDR_BUSCTL;
    case _PORT_BE:
        return ucPortDDR_BE;
    case _PORT_CS:
        return ucPortDDR_CS;
    case _PORT_FECI2C:
        return ucPortDDR_FECI2C;
    case _PORT_UART:
        return ucPortDDR_UART;
    case _PORT_QSPI:
        return ucPortDDR_QSPI;
    case _PORT_TIMER:
        return ucPortDDR_TIMER;
    case _PORT_IRQ:
        return ucPortDDR_IRQ;
#elif defined _M523X                                                     // {65}
    case _PORT_ADDR:
        return ucPortDDR_ADDR;
    case _PORT_BS:
        return ucPortDDR_BS;
    case _PORT_BUSCTL:
        return ucPortDDR_BUSCTL;
    case _PORT_CS:
        return ucPortDDR_CS;
    case _PORT_DATAH:
        return ucPortDDR_DATAH;
    case _PORT_DATAL:
        return ucPortDDR_DATAL;
    case _PORT_FECI2C:
        return ucPortDDR_FECI2C;
    case _PORT_ETPU:
        return ucPortDDR_ETPU;
    case _PORT_QSPI:
        return ucPortDDR_QSPI;
    case _PORT_SDRAM:
        return ucPortDDR_SDRAM;
    case _PORT_TIMER:
        return ucPortDDR_TIMER;
    case _PORT_UARTH:
        return ucPortDDR_UARTH;
    case _PORT_UARTL:
        return ucPortDDR_UARTL;
        break;
#else
    case _PORT_AN:
        return ucPortDDR_AN;
    #if !defined _M5225X && !defined _M521X                              // {39}
    case _PORT_LD:
        return ucPortDDR_LD;
    #endif
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_NQ:
        return ucPortDDR_NQ;
    case _PORT_TA:
        return ucPortDDR_TA;
    case _PORT_TC:
        return ucPortDDR_TC;
    #if !defined _M5225X                                                 // {39}
    case _PORT_TD:
        return ucPortDDR_TD;
    #endif
    case _PORT_UA:
        return ucPortDDR_UA;
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
    case _PORT_UB:
        return ucPortDDR_UB;
    case _PORT_UC:
        return ucPortDDR_UC;
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
    case _PORT_AS:
        return ucPortDDR_AS;
    #if !defined _M5225X && !defined _M521X && !defined _M52XX_SDRAM     // {39}{59}
    case _PORT_GP:
        return ucPortDDR_GP;
    #endif
    case _PORT_DD:
        return ucPortDDR_DD;
#endif
#if defined _M5225X                                                      // {39}
    case _PORT_TE:
        return ucPortDDR_TE;
    case _PORT_TF:
        return ucPortDDR_TF;
    case _PORT_TG:
        return ucPortDDR_TG;
    case _PORT_TH:
        return ucPortDDR_TH;
    case _PORT_TI:
        return ucPortDDR_TI;
    case _PORT_TJ:
        return ucPortDDR_TJ;
#endif
    }
    return 0;
}

extern unsigned long fnGetPresentPortPeriph(int iPort)
{
    return ~fnGetPorts(iPort - 1);
}
#if !defined _M520X && !defined _M523X                                   // {61}
extern void _SETQS(unsigned char ucValue)                                // {21}
{
    PORTQS |= ucValue;
  //PORTIN_SETQS = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARQS(unsigned char ucValue)                              // {21}
{
    CLEARQS = ucValue;                                                   
    PORTQS &= CLEARQS;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
extern void _SETAN(unsigned char ucValue)                                // {21}
{
    PORTAN |= ucValue;
  //PORTIN_SETAN = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARAN(unsigned char ucValue)                              // {21}
{
    CLEARAN = ucValue;                                                   
    PORTAN &= CLEARAN;                                                   // any zeros are cleared in the output register
}
#endif

#if !defined _M521X && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {39}{59}{61}
extern void _SETLD(unsigned char ucValue)                                // {21}
{
    PORTLD |= ucValue;
  //PORTIN_SETLD = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARLD(unsigned char ucValue)                              // {21}
{
    CLEARLD = ucValue;                                                   
    PORTLD &= CLEARLD;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
extern void _SETNQ(unsigned char ucValue)                                // {21}
{
    PORTNQ |= ucValue;
  //PORTIN_SETNQ = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARNQ(unsigned char ucValue)                              // {21}
{
    CLEARNQ = ucValue;                                                   
    PORTNQ &= CLEARNQ;                                                   // any zeros are cleared in the output register
}

extern void _SETTA(unsigned char ucValue)                                // {21}
{
    PORTTA |= ucValue;
  //PORTIN_SETTA = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARTA(unsigned char ucValue)                              // {21}
{
    CLEARTA = ucValue;                                                   
    PORTTA &= CLEARTA;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
extern void _SETTC(unsigned char ucValue)                                // {21}
{
    PORTTC |= ucValue;
  //PORTIN_SETTC = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARTC(unsigned char ucValue)                              // {21}
{
    CLEARTC = ucValue;                                                   
    PORTTC &= CLEARTC;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M521X && !defined _M5225X && !defined _M520X && !defined _M523X // {39}{61}
extern void _SETTD(unsigned char ucValue)                                // {21}
{
    PORTTD |= ucValue;
  //PORTIN_SETTD = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARTD(unsigned char ucValue)                              // {21}
{
    CLEARTD = ucValue;                                                   
    PORTTD &= CLEARTD;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
extern void _SETUA(unsigned char ucValue)                                // {21}
{
    PORTUA |= ucValue;
  //PORTIN_SETUA = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARUA(unsigned char ucValue)                              // {21}
{
    CLEARUA = ucValue;                                                   
    PORTUA &= CLEARUA;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X          // {59}{61}
extern void _SETUB(unsigned char ucValue)                                // {21}
{
    PORTUB |= ucValue;
  //PORTIN_SETUB = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARUB(unsigned char ucValue)                              // {21}
{
    CLEARUB = ucValue;                                                   
    PORTUB &= CLEARUB;                                                   // any zeros are cleared in the output register
}

extern void _SETUC(unsigned char ucValue)                                // {21}
{
    PORTUC |= ucValue;
  //PORTIN_SETUC = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARUC(unsigned char ucValue)                              // {21}
{
    CLEARUC = ucValue;                                                   
    PORTUC &= CLEARUC;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M520X && !defined _M523X                                   // {61}
extern void _SETAS(unsigned char ucValue)                                // {21}
{
    PORTAS |= ucValue;
  //PORTIN_SETAS = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARAS(unsigned char ucValue)                              // {21}
{
    CLEARAS = ucValue;                                                   
    PORTAS &= CLEARAS;                                                   // any zeros are cleared in the output register
}
#endif
#if !defined _M521X && !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {39}{59}{61}
extern void _SETGP(unsigned char ucValue)                                // {21}
{
    PORTGP |= ucValue;
  //PORTIN_SETGP = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARGP(unsigned char ucValue)                              // {21}
{
    CLEARGP = ucValue;                                                   
    PORTGP &= CLEARGP;                                                   // any zeros are cleared in the output register
}
#endif
#if defined _M520X                                                       // {61}
extern void _SETUART(unsigned char ucValue)
{
    PORTUART |= ucValue;
  //PORTIN_SETDD = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}
extern void _CLEARUART(unsigned char ucValue)
{
    PCLRR_UART = ucValue;                                                   
    PORTUART &= PCLRR_UART;                                              // any zeros are cleared in the output register
}
extern void _SETTIMER(unsigned char ucValue)
{
    PORTTIMER |= ucValue;
  //PORTIN_SETDD = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}
extern void _CLEARTIMER(unsigned char ucValue)
{
    PCLRR_TIMER = ucValue;                                                   
    PORTTIMER &= PCLRR_TIMER;                                            // any zeros are cleared in the output register
}
#elif defined _M523X                                                     // {65}
  // To do
#else
extern void _SETDD(unsigned char ucValue)                                // {21}
{
    PORTDD |= ucValue;
  //PORTIN_SETDD = ucValue;                                              // any set bits are set in the output register - the input register is however not disturbed
}

extern void _CLEARDD(unsigned char ucValue)                              // {21}
{
    CLEARDD = ucValue;                                                   
    PORTDD &= CLEARDD;                                                   // any zeros are cleared in the output register
}

    #if defined _M5225X                                                  // {73}
extern void _SETTE(unsigned char ucValue)
{
    PORTTE |= ucValue;
}

extern void _CLEARTE(unsigned char ucValue)
{
    CLEARTE = ucValue;
    PORTTE &= CLEARTE;                                                   // any zeros are cleared in the output register
}

extern void _SETTF(unsigned char ucValue)
{
    PORTTF |= ucValue;
}

extern void _CLEARTF(unsigned char ucValue)
{
    CLEARTF = ucValue;
    PORTTF &= CLEARTF;                                                   // any zeros are cleared in the output register
}

extern void _SETTG(unsigned char ucValue)
{
    PORTTG |= ucValue;
}

extern void _CLEARTG(unsigned char ucValue)
{
    CLEARTG = ucValue;
    PORTTG &= CLEARTG;                                                   // any zeros are cleared in the output register
}

extern void _SETTH(unsigned char ucValue)
{
    PORTTH |= ucValue;
}

extern void _CLEARTH(unsigned char ucValue)
{
    CLEARTH = ucValue;
    PORTTH &= CLEARTH;                                                   // any zeros are cleared in the output register
}

extern void _SETTI(unsigned char ucValue)
{
    PORTTI |= ucValue;
}

extern void _CLEARTI(unsigned char ucValue)
{
    CLEARTI = ucValue;
    PORTTI &= CLEARTI;                                                   // any zeros are cleared in the output register
}

extern void _SETTJ(unsigned char ucValue)
{
    PORTTJ |= ucValue;
}

extern void _CLEARTJ(unsigned char ucValue)
{
    CLEARTJ = ucValue;
    PORTTJ &= CLEARTJ;                                                   // any zeros are cleared in the output register
}
    #endif

#endif

extern void fnSimulateLinkUp(void)
{
#if defined ETH_INTERFACE
    #if (defined _M5225X || (defined _M52XX_SDRAM && !defined _M521X_SDRAM) || (defined _M520X)) && defined PHY_INTERRUPT // {43}{59}{61}
        #if defined _M520X
    fnSimulateInputChange(_PORT_IRQ, (7 - PHY_INTERRUPT), CLEAR_INPUT);
        #else
    fnSimulateInputChange(_PORT_NQ, (7 - PHY_INTERRUPT), CLEAR_INPUT);
        #endif
    #elif !defined _M5225X && !defined _M52XX_SDRAM && !defined _M520X && !defined _M523X // {65}
    EPHYSR = (EPHYIF | EPHY_10DIS);                                      // link up 100M
    if (EPHYCTL0 & EPHYIEN) {
        __VECTOR_RAM[EPHY_VECTOR]();                                     // simulate link up 
    }
    #endif
#endif
    fnUpdateIPConfig();                                                  // update display in simulator
}

#if defined CAN_INTERFACE

static void CAN_buf_Interrupt(int);

#if !defined UTASKER_SIM && !defined SIM_KOMODO
    #define _LOCAL_SIMULATION
    #define _TX_OK
#endif


#define ES_NO_CONNECTION_SIM       0
#define ES_CONNECTING_CLIENT_SIM   1
#define ES_CONNECTED_SIM           2

// This should be called whenever there is a meaningful change to the CAN registers.
//

static USOCKET SimSocket = -1;

#if defined UTASKER_SIM
    static unsigned char ucSimIP[IPV4_LENGTH] = {SIM_HW_IP_ADD};
    static unsigned short usSimPort = SIM_HW_PORT_NUMBER;
#endif
static unsigned short usPartnerPort;
static unsigned short usCAN_state = ES_NO_CONNECTION_SIM;

static int iLastTxBuffer;


typedef struct stTCP_MESSAGE                                             // definition of a data frame structure
{
  //TCP_HEADER     tTCP_Header;                                          // reserve header space
    unsigned char  ucTCP_Message[50];                                    // space for content
} TCP_MESSAGE;

#define SIM_CAN_TX_OK           0
#define SIM_CAN_TX_REMOTE_OK    1
#define SIM_CAN_TX_FAIL         2
#define SIM_CAN_TX_REMOTE_FAIL  3
#define SIM_CAN_RX_READY        4


static void fnBufferSent(int iBuffer, int iRemote)
{
    COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;
    COLDFIRE_CAN_CONTROL *ptrCAN_control;
    ptrMessageBuffer += iBuffer;

    switch (iRemote) {        
    case SIM_CAN_TX_OK:
        ptrCAN_control = (COLDFIRE_CAN_CONTROL *)CAN_BASE_ADD;
        if (((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ONCE) && (ptrMessageBuffer->ulCode_Len_TimeStamp & RTR)) { // sending remote frame
            ptrMessageBuffer->ulCode_Len_TimeStamp = (((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_RX_EMPTY) | (ptrCAN_control->_CAN_TIMER & 0x0000ffff)); // convert to temporary rx message buffer
        }
        else {
            ptrMessageBuffer->ulCode_Len_TimeStamp = (((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE) | (ptrCAN_control->_CAN_TIMER & 0x0000ffff));
        }
        break;
    case SIM_CAN_TX_FAIL:
        {
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                CAN_ERRSTAT |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (CANCTRL & ERRMSK) {                                  // only generate interrupt when interrupt is enabled
                    __VECTOR_RAM[CANERR_VECTOR]();
                }
            }
        }
        break;
    }
    switch (iBuffer) {                                                   // call transmission complete interrupt
    case 0:
        CAN_IFLAG |= 0x00000001;
        if (CAN_IMASK & 0x00000001) {                                    // check whether the interrupt is enabled
            __VECTOR_RAM[CAN0_VECTOR]();
        }
        break;
    case 1:
        CAN_IFLAG |= 0x00000002;
        if (CAN_IMASK & 0x00000002) {
            __VECTOR_RAM[CAN1_VECTOR]();
        }
        break;
    case 2:
        CAN_IFLAG |= 0x00000004;
        if (CAN_IMASK & 0x00000004) {
            __VECTOR_RAM[CAN2_VECTOR]();
        }
        break;
    case 3:
        CAN_IFLAG |= 0x00000008;
        if (CAN_IMASK & 0x00000008) {
            __VECTOR_RAM[CAN3_VECTOR]();
        }
        break;
    case 4:
        CAN_IFLAG |= 0x00000010;
        if (CAN_IMASK & 0x00000010) {
            __VECTOR_RAM[CAN4_VECTOR]();
        }
        break;
    case 5:
        CAN_IFLAG |= 0x00000020;
        if (CAN_IMASK & 0x00000020) {
            __VECTOR_RAM[CAN5_VECTOR]();
        }
        break;
    case 6:
        CAN_IFLAG |= 0x00000040;
        if (CAN_IMASK & 0x00000040) {
            __VECTOR_RAM[CAN6_VECTOR]();
        }
        break;
    case 7:
        CAN_IFLAG |= 0x00000080;
        if (CAN_IMASK & 0x00000080) {
            __VECTOR_RAM[CAN7_VECTOR]();
        }
        break;
    case 8:
        CAN_IFLAG |= 0x00000100;
        if (CAN_IMASK & 0x00000100) {
            __VECTOR_RAM[CAN8_VECTOR]();
        }
        break;
    case 9:
        CAN_IFLAG |= 0x00000200;
        if (CAN_IMASK & 0x00000200) {
            __VECTOR_RAM[CAN9_VECTOR]();
        }
        break;
    case 10:
        CAN_IFLAG |= 0x00000400;
        if (CAN_IMASK & 0x00000400) {
            __VECTOR_RAM[CAN10_VECTOR]();
        }
        break;
    case 11:
        CAN_IFLAG |= 0x00000800;
        if (CAN_IMASK & 0x00000800) {
            __VECTOR_RAM[CAN11_VECTOR]();
        } 
        break;
    case 12:
        CAN_IFLAG |= 0x00001000;
        if (CAN_IMASK & 0x00001000) {
            __VECTOR_RAM[CAN12_VECTOR]();
        }
        break;
    case 13:
        CAN_IFLAG |= 0x00002000;
        if (CAN_IMASK & 0x00002000) {
            __VECTOR_RAM[CAN13_VECTOR]();
        }
        break;
    case 14:
        CAN_IFLAG |= 0x00004000;
        if (CAN_IMASK & 0x00004000) {
            __VECTOR_RAM[CAN14_VECTOR]();
        }
        break;
    case 15:
        CAN_IFLAG |= 0x00008000;
        if (CAN_IMASK & 0x00008000) {
            __VECTOR_RAM[CAN15_VECTOR]();
        }
        break;

    default:                                                             // this happens when something is received
        break;
    }
}


static unsigned char fnRxBuffer(unsigned char *ucRxData, unsigned char ucLength)
{
    COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;
    unsigned short usTimeStamp;
    unsigned char ucRxType;
    int i = 0;
    unsigned char ucBuffer = *ucRxData++;

    ucLength = *ucRxData++;                                              // total length
    ucRxType = *ucRxData++;                                              // reception type

    if (ucRxType & CAN_REMOTE_MSG_RX) {
        ucLength -= 3;                                                   // remove rx type and time stamp from data length
    }
    else {
        ucLength -= 7;                                                   // remove info bytes to leave the data length
    }

    usTimeStamp = *ucRxData++;
    usTimeStamp <<= 8;
    usTimeStamp |= *ucRxData++;

    if (ucBuffer == 0xff) {                                              // this is a reception for a specific ID so we have to search for a buffer to accept it
        unsigned long ulId;
        int iRxAvailable = 0;       

        ulId = *ucRxData++;
        ulId <<= 8;
        ulId |= *ucRxData++;
        ulId <<= 8;
        ulId |= *ucRxData++;
        ulId <<= 8;
        ulId |= *ucRxData++;

        if (ulId & CAN_EXTENDED_ID) {
            ulId &= ~CAN_EXTENDED_ID;
        }
        else {
            ulId <<= CAN_STANDARD_SHIFT;
            ulId &= CAN_STANDARD_BITMASK;
        }

        while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
            if (ptrMessageBuffer->ulID == ulId) {
                iRxAvailable++;
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_RX_EMPTY) {
                    ucBuffer = i;                                        // we use this buffer for reception since it suits
                    ptrMessageBuffer = MBUFF0_ADD;
                    break;
                }
            }
            ptrMessageBuffer++;
            i++;
        }

        if (iRxAvailable) {
            i = 0;
            ptrMessageBuffer = MBUFF0_ADD;
            while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
                if (ptrMessageBuffer->ulID == ulId) {
                    if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == (MB_RX_FULL)) {
                        ucBuffer = i;                                    // we use this buffer for reception - it will set overrun...
                        ptrMessageBuffer = MBUFF0_ADD;

                        *ucRxData |= CAN_RX_OVERRUN;                     // set overrun flag
                        break;
                    }
                }
                ptrMessageBuffer++;
                i++;
            }
        }
        else {
            return 0xff;                                                 // no reception buffer found so ignore
        }
    }

    ptrMessageBuffer = MBUFF0_ADD + ucBuffer;                            // set the local simulate buffer correspondingly

    if (ucRxType & CAN_RX_OVERRUN) {
        ptrMessageBuffer->ulCode_Len_TimeStamp = ((MB_RX_OVERRUN | MB_RX_FULL) | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME)));
    }
    else {
        ptrMessageBuffer->ulCode_Len_TimeStamp = ((MB_RX_FULL) | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME))); // clear out code, length and time stamp fields
    }

    ptrMessageBuffer->ulCode_Len_TimeStamp |= usTimeStamp;
    ptrMessageBuffer->ulCode_Len_TimeStamp |= (ucLength << 16);

    i = 0;
    while (ucLength--) {
        ptrMessageBuffer->ucData[i++] = *ucRxData++;
    }
    return ucBuffer;
}

#if defined USE_TELNET && defined UTASKER_SIM
// Local listener on CAN port
//
static int fnCANListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    if (Socket != SimSocket) return APP_REJECT;                          // ignore if not our socket

    switch (ucEvent) {
    case TCP_EVENT_ARP_RESOLUTION_FAILED:
        usCAN_state = ES_NO_CONNECTION_SIM;                              // the simulator hardware is not there...
        break;

    case TCP_EVENT_CONNECTED:
        usCAN_state = ES_CONNECTED_SIM;
        break;

    case TCP_EVENT_ACK:
        break;

    case TCP_EVENT_CLOSE:
        break;

    case TCP_EVENT_ABORT:
        // fall through intentional
    case TCP_EVENT_CLOSED:
        break;

    case TCP_EVENT_REGENERATE:                                           // we must repeat
        break;

    case TCP_EVENT_DATA:                                                 // we have new receive data
        switch (*ucIp_Data++) {
        case 'E':
            fnBufferSent(*ucIp_Data, SIM_CAN_TX_FAIL);                   // last transmission was unsuccessful
            break;

        case 'e':
            fnBufferSent(*ucIp_Data, SIM_CAN_TX_REMOTE_FAIL);            // last remote transmission was unsuccessful
            break;

        case 'T':
            fnBufferSent(*ucIp_Data, SIM_CAN_TX_OK);                     // last transmission was successful
            break;

        case 'r':                                                        // remote rx was successful
            {
                unsigned char ucBuffer = fnRxBuffer(ucIp_Data, (unsigned char)usPortLen);
                if (ucBuffer != 0xff) {
                    fnBufferSent(ucBuffer, SIM_CAN_TX_REMOTE_OK);        // first remote transmission was successful
                }
            }
            break;

        case 'R':
            fnBufferSent(*ucIp_Data, SIM_CAN_TX_REMOTE_OK);              // first remote transmission was successful
            break;

        case 'B':                                                        // simulator HW has no free buffer...
            Socket = 0;                                                  // just for break point
            break;
        }
        break;

    case TCP_EVENT_CONREQ:                                               // we do not accept connection requests
        default:
        return APP_REJECT;
    }

    return APP_ACCEPT;
}
#endif

#if !defined _LOCAL_SIMULATION && defined SIM_KOMODO
// Komodo has received something on the CAN bus
//
static void fnCAN_reception(int iChannel, unsigned char ucDataLength, unsigned char *ptrData, unsigned long ulId, int iExtendedAddress, int iRemodeRequest, unsigned short usTimeStamp, Komodo km)
{
    COLDFIRE_CAN_BUF *ptrMessageBuffer;
    COLDFIRE_CAN_CONTROL *ptrCAN_control;
    int i = 0;
    int iRxAvailable = 0;
    int iOverrun = 0;
    unsigned char ucBuffer;
    if ((ucDataLength == 0) && (iRemodeRequest == 0)) {
        return;                                                          // ignore when reception without data
    }
    ptrCAN_control = (COLDFIRE_CAN_CONTROL *)CAN_BASE_ADD;
    ptrMessageBuffer = MBUFF0_ADD;                                       // the first of 16 message buffers in the FlexCan module

    if (iExtendedAddress == 0) {
        ulId <<= CAN_STANDARD_SHIFT;
        ulId &= CAN_STANDARD_BITMASK;
    }

    while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
        if (ptrMessageBuffer->ulID == ulId) {
            iRxAvailable++;
            if (iRemodeRequest != 0) {                                   // remote request being received
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ON_REQ) { // remote message waiting to be sent
                    int iResult;
                    km_can_packet_t pkt;
                    unsigned long arb_count = 0;
                    unsigned char ucTxDataLength;
                    pkt.id = ptrMessageBuffer->ulID;
                    // Send the CAN frame via remote simulator
                    // 
                    pkt.remote_req = 0;          
                    if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                        pkt.id |= CAN_EXTENDED_ID;                       // the address is to be handled as extended
                        pkt.extend_addr = 1;
                    }   
                    else {
                        pkt.id >>= CAN_STANDARD_SHIFT;                   // the address if a standard address
                        pkt.extend_addr = 0;
                    }
                    ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                    pkt.dlc = ucTxDataLength;
                    km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ptrMessageBuffer->ucData); // send
                    iResult = km_can_async_collect(km, 10, &arb_count);  // collect the result of the last transmission
                    switch (iResult) {
                    case KM_OK:
                        fnBufferSent(((iChannel << 24) | i), SIM_CAN_TX_REMOTE_OK);
                        break;
                    default:
                        fnBufferSent(((iChannel << 24) | i), SIM_CAN_TX_FAIL);
                        break;
                    }
                    return;
                }
            }
            else if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_RX_EMPTY) {
                ucBuffer = (unsigned char)i;                             // we use this buffer for reception since it suits
                break;
            }
        }
        ptrMessageBuffer++;
        i++;
    }

    ptrMessageBuffer = MBUFF0_ADD;

    if (iRxAvailable != 0) {
        i = 0;
        while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
            if (ptrMessageBuffer->ulID == ulId) {
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == (MB_RX_FULL)) {
                    ucBuffer = (unsigned char)i;                         // we use this buffer for reception - it will set overrun...
                    iOverrun = 1;
                    break;
                }
            }
            ptrMessageBuffer++;
            i++;
        }
    }
    else {
        return;                                                          // no reception buffer found so ignore
    }

    ptrMessageBuffer = MBUFF0_ADD;
    ptrMessageBuffer += ucBuffer;                                        // set the local simulate buffer correspondingly

    if (iOverrun != 0) {
        ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_OVERRUN | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME)));
    }
    else {
        ptrMessageBuffer->ulCode_Len_TimeStamp = ((MB_RX_FULL) | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~(CAN_CODE_FIELD | CAN_LENGTH_AND_TIME))); // clear out code, length and time stamp fields
    }

    ptrMessageBuffer->ulCode_Len_TimeStamp |= usTimeStamp;
    ptrMessageBuffer->ulCode_Len_TimeStamp |= (ucDataLength << 16);
    ptrMessageBuffer->ulID = ulId;

    i = 0;
    uMemcpy(ptrMessageBuffer->ucData, ptrData, 8);
    fnBufferSent((ucBuffer | (iChannel << 24)), SIM_CAN_RX_READY);       // generate interrupt
}
#endif


extern void fnSimCAN(int iChannel, int iBufferNumber, int iSpecial)      // {68}
{
    COLDFIRE_CAN_BUF *ptrMessageBuffer = MBUFF0_ADD;                     // the first of 16 message buffers in the FlexCan module
    COLDFIRE_CAN_CONTROL *ptrCAN_control = (COLDFIRE_CAN_CONTROL *)CAN_BASE_ADD;
#ifndef _LOCAL_SIMULATION
    #if defined USE_TELNET && defined UTASKER_SIM
    unsigned short usTxDataLength;
    TCP_MESSAGE tcp_message;
    #elif defined SIM_KOMODO                                             // {68}
    unsigned char ucTxDataLength;
    static Komodo km = -1;
    #endif
#endif

#if defined SIM_KOMODO
    if (CAN_SIM_CHECK_RX == iSpecial) {
        u08 data_in[8];
        km_can_packet_t pkt;
        km_can_info_t   info;
        pkt.remote_req  = 0;
        pkt.extend_addr = 0;
        pkt.dlc         = 8;
        pkt.id          = 0xff;                                          // promiscuous

        if (km < 0) {
            return;
        }
        (CAN_TIMER)++;                                                   // normally these only count when CAN controller is enabled and they would count at the CAN bit rate
        while ((km_can_read(km, &info, &pkt, 8, data_in)) >= 0) {        // has something been received?
            fnCAN_reception(info.channel, pkt.dlc, data_in, pkt.id, pkt.extend_addr, pkt.remote_req, (unsigned short)(info.timestamp), km);
        }
        return;
    }
    else if (CAN_SIM_TERMINATE == iSpecial) {
        if (km < 0) {
            return;
        }
        km_disable(km);
        km_close(KOMODO_USB_PORT);
        return;
    }
#endif

    // Configuration changes
    //
    if ((CANMCR & (CAN_FRZ | CAN_HALT)) != 0x00) {
        return;                                                          // off so don't do anything
    }

#if defined USE_TELNET && defined UTASKER_SIM
    if (SimSocket == -1) {
        SimSocket = fnStartTelnet(usSimPort, 0xffff, 0, 0, fnCANListener);
        fnTelnet(SimSocket, TELNET_RAW_MODE);                            // set to RAW operating mode
        fnTCP_Connect(SimSocket, ucSimIP, usSimPort, 0, 0);              // attempt to establish a connection to remote  server
        return;
    }
#elif defined SIM_KOMODO                                                 // {68}
    if (CAN_SIM_INITIALISE == iSpecial) {
        if (km < 0) {
            km = km_open(KOMODO_USB_PORT);                               // open a link to the Komodo via USB (channel 0 uses CAN A, channel 1 uses CAN B)
        }
        if (km >= 0) {
            int iCAN_speed = 0;
            unsigned char ucTimeQuanta = (unsigned char)((ptrCAN_control->CAN_CTRL1 & 0x7) + ((ptrCAN_control->CAN_CTRL1 >> 16) & 0x7) + ((ptrCAN_control->CAN_CTRL1 >> 19) & 0x7) + 4);
            if (!(CLK_SRC_INT_BUS & ptrCAN_control->CAN_CTRL1)) {        // CAN clock derived from external clock/crystal (lowest jitter)
                iCAN_speed = ((CRYSTAL_FREQ/ucTimeQuanta)/((ptrCAN_control->CAN_CTRL1 >> 24) + 1));
            }
            else {                                                       // bus clock / 2
                iCAN_speed = (((BUS_CLOCK/2)/ucTimeQuanta)/((ptrCAN_control->CAN_CTRL1 >> 24) + 1));
            }
            km_disable(km);
            km_acquire(km, (KM_FEATURE_CAN_A_CONFIG | KM_FEATURE_CAN_A_CONTROL | KM_FEATURE_CAN_A_LISTEN)); // acquire features
            km_timeout(km, KM_TIMEOUT_IMMEDIATE);
            km_can_bitrate(km, KM_CAN_CH_A, iCAN_speed);                 // set bitrate
          //km_can_target_power(km, KM_CAN_CH_A, 1);                     // set target power
            km_enable(km);
          //km_can_target_power(km, KM_CAN_CH_A, 0);                     // disable target power
        }
    }

#endif

    ptrMessageBuffer += iBufferNumber;

    // Transmission
    //
    switch (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) {
    case MB_TX_INACTIVE:
        // assume that we were previously active - for example sending remote frames - disable the HW simulator
#if !defined _LOCAL_SIMULATION
    #if defined USE_TELNET && defined UTASKER_SIM
        if (fnGetCANOwner(0, iBufferNumber) != TASK_CAN_SIM) {
            if (iSpecial == CAN_SIM_FREE_BUFFER) {
                tcp_message.ucTCP_Message[0] = 'd';
            }
            else  {
                tcp_message.ucTCP_Message[0] = 'D';
            }
            tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number
            fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, 2, TCP_BUF_SEND); // send data to HW simulator
        }
    #elif defined SIM_KOMODO                                             // {68}
        if (fnGetCANOwner(iChannel, iBufferNumber) != TASK_CAN_SIM) {
            if (iSpecial == CAN_SIM_FREE_BUFFER) {
                // Nothing to do in this case
                //
            }
            else  {
                unsigned char data[3] = {0x01, 0x02, 0x03};
                km_can_packet_t pkt;
                pkt.remote_req   = 0;
                pkt .extend_addr = 1;
                pkt.dlc          = 3;
                pkt.id           = 0x105;
                km_can_async_submit(km, iChannel, 0, &pkt, pkt.dlc, data);
            }
        }
    #endif
#endif
        break;

    case MB_TX_SEND_ON_REQ:                                              // this buffer containes a queued message to be sent on a Remote Frame
#if defined _LOCAL_SIMULATION
        // fall through
#endif
    case MB_TX_SEND_ONCE:                                           
        if (ptrMessageBuffer->ulCode_Len_TimeStamp & RTR) {              // remote frame is to be transmitted
#if defined _LOCAL_SIMULATION
    #if defined _TX_OK
            // The buffer converts automatically to a receive buffer
            //
            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_FULL | RTR | (0x03<<16));
            ptrMessageBuffer->ucData[0] = 0x01;                          // receive some dummy data
            ptrMessageBuffer->ucData[1] = 0x02;
            ptrMessageBuffer->ucData[2] = 0x03;
            goto _rx_int;
     #else
            // The remote transmission failed
            //
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                CAN_ERRSTAT |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (CANCTRL & ERRMSK) {                                  // only generate interrupt when interrupt is enabled
                    CAN_error_Interrupt();                               // simulate a number of errors
                }
            }
     #endif
#else

            if (fnGetCANOwner(0, iBufferNumber) != TASK_CAN_SIM) {       // send a remote frame
    #if defined USE_TELNET && defined UTASKER_SIM
                unsigned long ulID;
                tcp_message.ucTCP_Message[0] = 'r';
                tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number

                ulID = ptrMessageBuffer->ulID;
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    ulID |= CAN_EXTENDED_ID;                             // the address is to be handled as extended
                }   
                else {
                    ulID >>= CAN_STANDARD_SHIFT;                         // the address if a standard address
                }
                tcp_message.ucTCP_Message[2] = (unsigned char)(ulID >> 24);
                tcp_message.ucTCP_Message[3] = (unsigned char)(ulID >> 16);
                tcp_message.ucTCP_Message[4] = (unsigned char)(ulID >> 8);
                tcp_message.ucTCP_Message[5] = (unsigned char)(ulID);

                fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, 6, TCP_BUF_SEND); // send data to HW simulator
    #elif defined SIM_KOMODO
                int iResult;
                km_can_packet_t pkt;
                unsigned long arb_count = 0;
                pkt.id = ptrMessageBuffer->ulID;
                // Send the CAN frame via remote simulator
                // 
                pkt.remote_req = 1;          
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    pkt.id |= CAN_EXTENDED_ID;                           // the address is to be handled as extended
                    pkt.extend_addr = 1;
                }   
                else {
                    pkt.id >>= CAN_STANDARD_SHIFT;                       // the address if a standard address
                    pkt.extend_addr = 0;
                }
                iLastTxBuffer = iBufferNumber;
                iBufferNumber = 0;
                ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                pkt.dlc = ucTxDataLength;
                km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, 0, 0); // send
                iResult = km_can_async_collect(km, 10, &arb_count);      // collect the result of the last transmission
                switch (iResult) {
                case KM_OK:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_OK);
                    break;
                default:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_FAIL);
                    break;
                }
    #endif
            }
            else {
                // The buffer converts automatically to a receive buffer
                //
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_FULL | RTR | (0x03<<16));
                // receive some dummy data
                ptrMessageBuffer->ucData[0] = 0x01;
                ptrMessageBuffer->ucData[1] = 0x02;
                ptrMessageBuffer->ucData[2] = 0x03;

                fnBufferSent(iBufferNumber, SIM_CAN_TX_REMOTE_OK);
            }
#endif
        }
        else {                                                           // this buffer contains a message to be transmitted once
#if defined _LOCAL_SIMULATION                                            // simple simulation for simple testing of driver
    #if defined _TX_OK
            ptrMessageBuffer->ulCode_Len_TimeStamp = ((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE);
_rx_int:
            CAN_IFLAG |= (0x00000001 << iBufferNumber);                  // set interrupt flag
            if (CAN_IMASK & (0x00000001 << iBufferNumber)) {
                CAN_buf_Interrupt(iBufferNumber);                        // call interrupt routine
            }
    #else
            int x;
            for (x = 0; x < MAX_TX_CAN_TRIES; x++) {                     // simulate frame transmission failing
                CAN_ERRSTAT |= (CAN_ACK_ERR | TXWRN | CAN_BUS_IDLE | CAN_ERROR_PASSIVE | ERRINT);
                if (CANCTRL & ERRMSK) {                                  // only generate interrupt when interrupt is enabled
                    CAN_error_Interrupt();                               // simulate a number of errors
                }
            }
    #endif
#else
            if (fnGetCANOwner(0, iBufferNumber) != TASK_CAN_SIM) {
    #if defined USE_TELNET && defined UTASKER_SIM
                unsigned long ulID;
                // Send the CAN frame to remote simulator
                // [['T'] - BUFFER NUMBER - LENGTH - ID - DATA[...]]
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) ==  MB_TX_SEND_ON_REQ) {
                    tcp_message.ucTCP_Message[0] = 'R';                  // set remote transmission
                }
                else {
                    tcp_message.ucTCP_Message[0] = 'T';
                }
                tcp_message.ucTCP_Message[1] = (unsigned char)iBufferNumber; // the buffer number
                tcp_message.ucTCP_Message[2] = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                ulID = ptrMessageBuffer->ulID;
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    ulID |= CAN_EXTENDED_ID;                             // the address is to be handled as extended
                }   
                else {
                    ulID >>= CAN_STANDARD_SHIFT;                         // the address if a standard address
                }
                tcp_message.ucTCP_Message[3] = (unsigned char)(ulID >> 24);
                tcp_message.ucTCP_Message[4] = (unsigned char)(ulID >> 16);
                tcp_message.ucTCP_Message[5] = (unsigned char)(ulID >> 8);
                tcp_message.ucTCP_Message[6] = (unsigned char)(ulID);

                iLastTxBuffer = iBufferNumber;

                iBufferNumber = 0;
                usTxDataLength = tcp_message.ucTCP_Message[2] + 7;
                while (iBufferNumber++ < tcp_message.ucTCP_Message[2]) {
                    tcp_message.ucTCP_Message[iBufferNumber+6] = ptrMessageBuffer->ucData[iBufferNumber-1];
                }
                fnSendBufTCP(SimSocket, (unsigned char *)&tcp_message.ucTCP_Message, usTxDataLength, TCP_BUF_SEND); // send data to HW simulator
    #elif defined SIM_KOMODO
                int iResult;
                km_can_packet_t pkt;
                unsigned long arb_count = 0;
                pkt.id = ptrMessageBuffer->ulID;
                // Send the CAN frame via remote simulator
                // 
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) == MB_TX_SEND_ON_REQ) {
                    return;                                              // the message is not sent in this case but will be sent on any remote frame request receptions
                }
                else {
                    pkt.remote_req = 0;
                }             
                if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                    pkt.id |= CAN_EXTENDED_ID;                           // the address is to be handled as extended
                    pkt.extend_addr = 1;
                }   
                else {
                    pkt.id >>= CAN_STANDARD_SHIFT;                       // the address if a standard address
                    pkt.extend_addr = 0;
                }

                iLastTxBuffer = iBufferNumber;
                iBufferNumber = 0;
                ucTxDataLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                pkt.dlc = ucTxDataLength;
                km_can_async_submit(km, iChannel, KM_CAN_ONE_SHOT, &pkt, ucTxDataLength, (const unsigned char *)ptrMessageBuffer->ucData); // send
                iResult = km_can_async_collect(km, 10, &arb_count);      // collect the result of the last transmission
                switch (iResult) {
                case KM_OK:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_OK);
                    break;
                default:
                    fnBufferSent(((iChannel << 24) | iLastTxBuffer), SIM_CAN_TX_FAIL);
                    break;
                }
    #endif
            }
            else {
                if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_CODE_FIELD) ==  MB_TX_SEND_ON_REQ) {
                    fnBufferSent(iBufferNumber, SIM_CAN_TX_REMOTE_OK);
                }
                else {
                    fnBufferSent(iBufferNumber, SIM_CAN_TX_OK);
                }
                return;
            }
#endif
        }
        break;
    default:
        break;
    }
}
#endif

#if !defined _M52XX_SDRAM && !defined _M523X                             // {59}{65} - these processor types don't have peripheral power control
// Power up the defined module
//
extern void _POWER_UP(unsigned char ucModule)                            // {25}
{
    if (ucModule >= 64) {                                                // remove all clock disables
        PPMRH = 0;
        PPMRL = 0;
    }
    else if (ucModule > 31) {                                            // high register
        PPMRH &= ~(1 << (ucModule - 32));                                // remove clock disable
    }
    else {
        PPMRL &= ~(1 << ucModule);                                       // remove clock disable
    }
}

// Power down the defined module
//
extern void _POWER_DOWN(unsigned char ucModule)                          // {25}
{
    if (ucModule >= 64) {                                                // disable all module clocks
        PPMRH = 0xffffffff;
        PPMRL = 0xffffffff;
    }
    else if (ucModule > 31) {                                            // high register
        PPMRH |= (1 << (ucModule - 32));                                 // set clock disable
    }
    else {
        PPMRL |= (1 << ucModule);                                        // set clock disable
    }
}
#endif

#if defined USB_INTERFACE                                                // {13}      
// The Buffer Descriptor Tables work in Little-endian mode so we have to convert some addresses
//
static void *_fnLE_add(unsigned long long_word)
{
    #if defined _M5225X
    if (CCM_CCE & USB_END) {                                             // as long as the big-endian mode has been set in this device, feed without little-endian conversion
        return (void *)long_word;
    }
    else {
        unsigned long ulLE_long_word;
        ulLE_long_word = ((long_word >> 24) | ((long_word >> 8) & 0x0000ff00) | ((long_word << 8) & 0x00ff0000) | ((long_word << 24) & 0xff000000));
        return (void *)ulLE_long_word;
    }
    #else
    unsigned long ulLE_long_word;
    ulLE_long_word = ((long_word >> 24) | ((long_word >> 8) & 0x0000ff00) | ((long_word << 8) & 0x00ff0000) | ((long_word << 24) & 0xff000000));
    return (void *)ulLE_long_word;
    #endif
}


static unsigned char ucTxBuffer[NUMBER_OF_USB_ENDPOINTS] = {0};          // monitor the controller's transmission buffer use

// Inject USB transactions for test purposes
//
extern int fnSimulateUSB(int iDevice, int iEndPoint, unsigned char ucPID, unsigned char *ptrDebugIn, unsigned short usLenEvent)
{
    static unsigned char ucRxBank[NUMBER_OF_USB_ENDPOINTS];              // monitor the buffer to inject to
    unsigned char *ptrData;
    M5222X_USB_ENDPOINT_BD *ptrBDT = (M5222X_USB_ENDPOINT_BD *)((BDT_PAGE_01 << 8) + (BDT_PAGE_02 << 16) + (BDT_PAGE_03 << 24));

    STAT = ((unsigned char)iEndPoint << END_POINT_SHIFT);                // set the endpoint to the STAT register
    if (!ptrDebugIn) {
        INT_STAT = 0;
        if (usLenEvent & USB_RESET_CMD) {
            INT_STAT |= USB_RST;
            memset(ucRxBank,   0, sizeof(ucRxBank));                     // default is even bank
            memset(ucTxBuffer, 0, sizeof(ucTxBuffer));                   // default is even buffer
        }
        if (usLenEvent & USB_SLEEP_CMD) {
            INT_STAT |= SLEEP;
        }
        if (usLenEvent & USB_RESUME_CMD) {
            INT_STAT |= RESUME;
        }
        if (usLenEvent & USB_IN_SUCCESS) {
            INT_STAT |= (TOK_DNE);                                       // interrupt status
            STAT |= TX_TRANSACTION;
            STAT |= ucTxBuffer[iEndPoint];
        }
    #if defined USB_HOST_SUPPORT                                         // {41}
        if (usLenEvent & USB_FULLSPEED_ATTACH_CMD) {
            INT_STAT |= ATTACH;                                          // attach interrupt
            CTL |= JSTATE;                                               // full speed state
        }
        else if (usLenEvent & USB_LOWSPEED_ATTACH_CMD) {
            INT_STAT |= ATTACH;                                          // attach interrupt
            CTL &= ~JSTATE;                                              // low speed state
        }
    #endif
    }
    else {
        unsigned short usLength;
        if (ADDR != iDevice) {                                           // not our device address so ignore
            if (iDevice != 0xff) {                                       // special broadcast for simulator use so that it doesn't have to know the USB address
                return 1;
            }
        }
        INT_STAT |= TOK_DNE;                                             // interrupt status                                           
        STAT |= ucRxBank[iEndPoint];                                     // the buffer used

        ptrBDT += iEndPoint;
        if (!(ucRxBank[iEndPoint] & ODD_BANK)) {                         // even buffer
            if (!(ptrBDT->usb_bd_rx_even.ulUSB_BDControl & OWN)) {       // check whether it owned by the USB controller
                return 1;                                                // no controller ownership so ignore
            }
            if (!(ptrBDT->usb_bd_rx_even.ulUSB_BDControl & KEEP)) {
                ptrBDT->usb_bd_rx_even.ulUSB_BDControl &= ~OWN;
            }
    #if defined _M5225X                                                  // this device supports big-endian mode
            if (CCM_CCE & USB_END) {
                usLength = (unsigned short)((unsigned char)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> BE_USB_CNT_SHIFT));
            }
            else {
                usLength = (unsigned short)((unsigned char)(ptrBDT->usb_bd_rx_even.ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB) | (ptrBDT->usb_bd_rx_even.ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB));
            }
    #else
            usLength = (unsigned short)((unsigned char)(ptrBDT->usb_bd_rx_even.ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB) | (ptrBDT->usb_bd_rx_even.ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB));
    #endif
            if (usLength < usLenEvent) {                                 // limite the injected length to the endpoint limit
                usLenEvent = usLength;
            }
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl &= ~(RX_PID_MASK | USB_BYTE_CNT_MASK | DATA_1);
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= (ucPID << RX_PID_SHIFT);
            ptrData = ptrBDT->usb_bd_rx_even.ptrUSB_BD_Data;
            ptrData = _fnLE_add((unsigned long)ptrData);
            ptrBDT->usb_bd_rx_even.ulUSB_BDControl |= SET_FRAME_LENGTH(usLenEvent);// add length
        }
        else {
            if (!(ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & OWN)) {
                return 1;                                                // no controller ownership so ignore
            }
            if (!(ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & KEEP)) {
                ptrBDT->usb_bd_rx_odd.ulUSB_BDControl &= ~OWN;
            }
    #if defined _M5225X                                                  // this device supports big-endian mode
            if (CCM_CCE & USB_END) {
                usLength = (unsigned short)((unsigned char)((ptrBDT->usb_bd_rx_odd.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> BE_USB_CNT_SHIFT));
            }
            else {
                usLength = (unsigned short)((unsigned char)(ptrBDT->usb_bd_rx_odd.ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB) | (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB));
            }
    #else
            usLength = (unsigned short)((unsigned char)(ptrBDT->usb_bd_rx_odd.ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB) | (ptrBDT->usb_bd_rx_odd.ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB));
    #endif
            if (usLength < usLenEvent) {                                 // limit the injected length to the endpoint limit
                usLenEvent = usLength;
            }
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl &= ~(RX_PID_MASK | USB_BYTE_CNT_MASK);
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= (ucPID << RX_PID_SHIFT);
            ptrData = ptrBDT->usb_bd_rx_odd.ptrUSB_BD_Data;
            ptrData = _fnLE_add((unsigned long)ptrData);
            ptrBDT->usb_bd_rx_odd.ulUSB_BDControl |= SET_FRAME_LENGTH(usLenEvent); // add length
        }
        memcpy(ptrData, ptrDebugIn, usLenEvent);
        ucRxBank[iEndPoint] ^= ODD_BANK;                                 // the next one to be used - toggle mechanism
    }
    __VECTOR_RAM[USB_OTG_VECTOR]();                                      // call the interrupt handler
    return 0;
}

// Handle data sent by USB
//
extern void fnSimUSB(int iType, int iEndpoint, USB_HW *ptrUSB_HW)
{
    extern void fnChangeUSBState(int iNewState);
    switch (iType) {
    case USB_SIM_RESET:
        {
            int x;
            fnChangeUSBState(0);
            for (x = 0; x < NUMBER_OF_USB_ENDPOINTS; x++) {
                fnLogUSB(x, 0, 0, (unsigned char *)0xffffffff, 0);       // log reset on each endpoint
            }
        }
        break;
    case USB_SIM_TX:                                                     // a frame transmission has just been started
        if (!(OWN & *ptrUSB_HW->ptr_ulUSB_BDControl)) {                  // if the ownership has not been passed to the USB controller ignore it
            return;
        }
        iInts |= USB_INT;                                                // flag that the interrupt should be handled
        ulEndpointInt |= (1 << iEndpoint);                               // flag endpoint
        break;
    case USB_SIM_ENUMERATED:                                             // flag that we have completed emumeration
        fnChangeUSBState(1);
        break;
    case USB_SIM_STALL:
        fnLogUSB(iEndpoint, 0, 1, (unsigned char *)0xffffffff, 0);       // log stall
        break;
    case USB_SIM_SUSPEND:
        fnChangeUSBState(0);
        break;
    }
}

extern void fnCheckUSBOut(int iDevice, int iEndpoint)
{
    M5222X_USB_BD *bufferDescriptor;
    unsigned long ulAddress = ((BDT_PAGE_01 << 8) | (BDT_PAGE_02 << 16) | (BDT_PAGE_03 << 24));
    M5222X_USB_ENDPOINT_BD *bdt = (M5222X_USB_ENDPOINT_BD *)ulAddress;
    bdt += iEndpoint;

    do {
        if (ucTxBuffer[iEndpoint] != 0) {
            bufferDescriptor = &bdt->usb_bd_tx_odd;
        }
        else {
            bufferDescriptor = &bdt->usb_bd_tx_even;
        }
        if (bufferDescriptor->ulUSB_BDControl & OWN) {                   // owned by USB controller so interpret it
            unsigned short usUSBLength;
            unsigned char *ptrUSBData = 0;                               // {48}
    #if defined _M5225X                                                  // this device supports big-endian mode
            if (CCM_CCE & USB_END) {
                usUSBLength  = (unsigned char)((bufferDescriptor->ulUSB_BDControl & USB_BYTE_CNT_MASK) >> BE_USB_CNT_SHIFT); // get the length from the control register
            }
            else {
                usUSBLength  = (unsigned char)(bufferDescriptor->ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB); // get the length from the control register
                usUSBLength |= ((bufferDescriptor->ulUSB_BDControl & 0x3) << USB_BYTE_CNT_SHIFT_MSB);
            }
    #else
            usUSBLength  = (unsigned char)(bufferDescriptor->ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB); // get the length from the control register
            usUSBLength |= ((bufferDescriptor->ulUSB_BDControl & 0x3) << USB_BYTE_CNT_SHIFT_MSB);
    #endif
            if (usUSBLength != 0) {
                ptrUSBData = _fnLE_add((CAST_POINTER_ARITHMETIC)bufferDescriptor->ptrUSB_BD_Data); // the data to be sent
            }
            if (!(bufferDescriptor->ulUSB_BDControl & KEEP)) {           // if the KEEP bit is not set
                bufferDescriptor->ulUSB_BDControl &= ~OWN;               // remove SIE ownership
            }
            fnLogUSB(iEndpoint, 0, usUSBLength, ptrUSBData, ((bufferDescriptor->ulUSB_BDControl & DATA_1) != 0));
            fnSimulateUSB(iDevice, iEndpoint, 0, 0, USB_IN_SUCCESS);     // generate tx interrupt
            ucTxBuffer[iEndpoint] ^= ODD_BANK;                           // toggle buffer
        }
        else {
            break;
        }
    } while (1);
}

// Request an endpoint buffer size
//
extern unsigned short fnGetEndpointInfo(int iEndpoint)
{
    M5222X_USB_ENDPOINT_BD *ptrBDT = (M5222X_USB_ENDPOINT_BD *)((BDT_PAGE_01 << 8) + (BDT_PAGE_02 << 16) + (BDT_PAGE_03 << 24));
    unsigned short usBufferSize;
    ptrBDT += iEndpoint;
    #if defined _M5225X                                                  // this device supports big-endian mode
    if (CCM_CCE & USB_END) {
        usBufferSize =  (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl & USB_BYTE_CNT_MASK) >> BE_USB_CNT_SHIFT); // get the length from the control register
    }
    else {
        usBufferSize =  (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB) & 0x00ff);
        usBufferSize |= (unsigned short)(ptrBDT->usb_bd_rx_even.ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB);
    }
    #else
    usBufferSize =  (unsigned short)((ptrBDT->usb_bd_rx_even.ulUSB_BDControl >> USB_BYTE_CNT_SHIFT_LSB) & 0x00ff);
    usBufferSize |= (unsigned short)(ptrBDT->usb_bd_rx_even.ulUSB_BDControl << USB_BYTE_CNT_SHIFT_MSB);
    #endif
    return usBufferSize;
}
#endif


#if defined SUPPORT_TOUCH_SCREEN
static int iPenLocationX = 0;                                            // last sample ADC input value
static int iPenLocationY = 0;
static int iPenDown = 0;

#define MIN_X_TOUCH          0x0d50                                      // tuned values - for calibration these should be taken from parameters
#define MAX_X_TOUCH          0x7186
#define MIN_Y_TOUCH          0x0f40
#define MAX_Y_TOUCH          0x7100

extern void fnPenPressed(int iX, int iY)
{
    iPenLocationX = (iX - 6);
    iPenLocationY = (iY - 3);
    iPenDown = 1;
}

extern void fnPenMoved(int iX, int iY)
{
    iPenLocationX = (iX - 6);
    iPenLocationY = (iY - 3);
}

extern void fnPenLifted(void)
{
    iPenLocationX = 0;
    iPenLocationY = 0;
    iPenDown = 0;
}

extern void fnGetPenSamples(unsigned short *ptrX, unsigned short *ptrY)
{
    if (iPenLocationX != 0) {
        iPenLocationX = iPenLocationX;
    }
    if (iPenDown == 0) {
        *ptrX = 0;
        *ptrY = 0;
    }
    else {
        *ptrX = (MIN_X_TOUCH + ((iPenLocationX * ((MAX_X_TOUCH - MIN_X_TOUCH)))/GLCD_X));
        *ptrY = (MIN_Y_TOUCH + ((iPenLocationY * ((MAX_Y_TOUCH - MIN_Y_TOUCH)))/GLCD_Y));
    }
}
#endif

#endif
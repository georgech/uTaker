/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_UART.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    03.03.2012 Add K70 UART2 alternative port mapping                    {3}
    05.04.2012 Add UART DMA support for Tx                               {6}
    06.04.2012 Modify control of RTS line when not using automatic RTS control on transmission {7}
    06.04.2012 Add UART DMA support for Rx                               {8}
    08.05.2012 Add UART3 option on port F for K70                        {12}
    24.06.2012 Add SERIAL_SUPPORT_DMA_RX_FREERUN support                 {15}
    24.06.2012 Correct some RTS/CTS pins                                 {16}
    14.09.2012 Add UART alternative port options to K61                  {25}
    23.10.2013 Ensure source address remains stable in free-running UART mode {56}
    06.05.2014 Add KL DMA based UART transmission                        {81}
    05.07.2014 Rework of UART interrupt handler including receiver overrun checking {92}
    19.07.2014 Add UART low power stop mode support                      {96}
    17.11.2014 Add KL03 LPUART support                                   {106}
    04.12.2014 Ensure source and destination addresses remain stable in UART tx DMA mode {107}
    24.03.2015 Peripheral drivers removed to their own include files
    27.08.2015 Add Kinetis KE/KL RTS control via GPIO and option to use the transmit complete interrupt for accurate end of frame call-back (UART_FRAME_END_COMPLETE) {200}
    10.10.2015 Correct KL Baud clock calculation rounding and configure the IRC clock source when selecting it for LPUART use {201}
    17.11.2015 Mask out LPUART clock source when setting IRC48M in case it has already been adjusted by internal ROM boot operation {202}
    01.12.2015 Avoid disturbing DMA when modifying UART configurations on the fly {203}
    06.02.2016 Correct LPUART parity polarity and avoid possible transmit interrupt loss when changing mode on the fly {204}
    20.12.2016 Add break support                                         {205}
    04.01.2017 Don't adjust the RC clock setting when the processor is running from it {206}
    05.01.2017 Add optional midi baud rate                               {207}
    07.01.2017 Add UART_TIMED_TRANSMISSION support for parts with DMA    {208}
    03.05.2017 Add free-running DMA reception mode (SERIAL_SUPPORT_DMA_RX_FREERUN) on LPUARTs and UARTs for KL parts based on modulo rx buffer {209}
    06.08.2017 Use POWER_UP_ATOMIC() instead of POWER_UP() to enable clocks to UART modules (using bit-banding access)
    01.09.2017 Correct clearing LPUART overrun flag                      {210}
    19.10.2017 Use ATOMIC_PERIPHERAL_BIT_REF_SET() and ATOMIC_PERIPHERAL_BIT_REF_CLEAR() to enable/disable DMA_ERQ (interrupt and DMA safe)
    05.02.2018 Add free-running LPUART DM Areception for KL devices with eDMA {211}
    13.03.2018 Add RTS/CTS flow control to devices without integrated modem control {212}
    25.03.2018 Rework of DMA routines to make use of generic DMA subroutines (code saving and better clarity) and allow KL parts to use full or half-buffer DMA interrupts
    05.05.2018 Add UART_HW_TRIGGERED_TX_MODE                             {213}
    01.06.2018 Consolidate interrupt operation into a generic version (saves place and simplifies maintenance of increased feature set)
    01.06.2018 Add option user callback options USER_DEFINED_UART_RX_HANDLER, USER_DEFINED_UART_RX_BREAK_DETECTION, USER_DEFINED_UART_TX_FRAME_COMPLETE
    12.06.2018 Protect bit setting and clearing from other UART interrupts (with higher priority) using PROTECTED_SET_VARIABLE() and PROTECTED_SET_VARIABLE()
    22.07.2018 Added fnPrepareRxDMA_mode() to handle generic configuration of rx DMA mode {214}
    31.07.2018 Corrected shared DMA channel numbering between Rx and Tx  {215}

*/

#if defined SERIAL_INTERFACE && (NUMBER_EXTERNAL_SERIAL > 0)
    #if defined EXT_UART_SC16IS7XX
        #include "spi_sc16IS7xx.h"                                       // include driver
    #endif
#endif

#if !defined UART_PULL_UPS
    #define UART_PULL_UPS 0
#endif

#if defined _WINDOWS
    static void _fnConfigSimSCI(QUEUE_HANDLE Channel, TTYTABLE *pars, unsigned short usDivider, unsigned char ucFraction, unsigned long ulBusClock, unsigned long ulSpecialClock);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0                         // device contains both UART and LPUART
// For details about UART/LPUART numbering conventions see http://www.utasker.com/kinetis/UART_LPUART.html
//
#define UART_TYPE_LPUART 0
#define UART_TYPE_UART   1
static const unsigned char uart_type[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = { // UART type reference of each channel
    #if defined LPUARTS_PARALLEL
    UART_TYPE_UART,                                                      // UART0
        #if UARTS_AVAILABLE > 1
    UART_TYPE_UART,                                                      // UART1
        #endif
        #if UARTS_AVAILABLE > 2
    UART_TYPE_UART,                                                      // UART2
        #endif
        #if UARTS_AVAILABLE > 3
    UART_TYPE_UART,                                                      // UART1
        #endif
        #if UARTS_AVAILABLE > 4
    UART_TYPE_UART,                                                      // UART2
        #endif
    UART_TYPE_LPUART,                                                    // LPUART0 (numbered starting with UARTS_AVAILABLE)
    #else
    UART_TYPE_LPUART,                                                    // LPUART0
        #if LPUARTS_AVAILABLE > 1
    UART_TYPE_LPUART,                                                    // LPUART1
        #endif
        #if LPUARTS_AVAILABLE > 2
    UART_TYPE_LPUART,                                                    // LPUART2
        #endif
        #if LPUARTS_AVAILABLE > 3
    UART_TYPE_LPUART,                                                    // LPUART3
        #endif
    UART_TYPE_UART,                                                      // UART
    #endif
};
#endif

#if defined SERIAL_SUPPORT_DMA                                           // {6} DMA support on transmission
// DMA channel assignments for each UART/LPUART transmitter
//
static const unsigned char UART_DMA_TX_CHANNEL[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
    DMA_UART0_TX_CHANNEL, 
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    DMA_UART1_TX_CHANNEL,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    DMA_UART2_TX_CHANNEL, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    DMA_UART3_TX_CHANNEL,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    DMA_UART4_TX_CHANNEL,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    DMA_UART5_TX_CHANNEL,
    #endif
};

// DMA channel interrupt priority assignments for each UART/LPUART transmitter
//
static const unsigned char UART_DMA_TX_INT_PRIORITY[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
    DMA_UART0_TX_INT_PRIORITY, 
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    DMA_UART1_TX_INT_PRIORITY, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    DMA_UART2_TX_INT_PRIORITY, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    DMA_UART3_TX_INT_PRIORITY,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    DMA_UART4_TX_INT_PRIORITY,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    DMA_UART5_TX_INT_PRIORITY,
    #endif
};
#endif

 #if defined SERIAL_SUPPORT_DMA_RX                                       // DMA support on reception
// DMA channel assignments for each UART/LPUART receiver
//
static const unsigned char UART_DMA_RX_CHANNEL[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
        DMA_UART0_RX_CHANNEL, 
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
        DMA_UART1_RX_CHANNEL, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
        DMA_UART2_RX_CHANNEL, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
        DMA_UART3_RX_CHANNEL,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
        DMA_UART4_RX_CHANNEL,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
        DMA_UART5_RX_CHANNEL
    #endif
};

// DMA channel interrupt priority assignments for each UART/LPUART receiver
//
static const unsigned char UART_DMA_RX_INT_PRIORITY[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
        DMA_UART0_RX_INT_PRIORITY, 
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
        DMA_UART1_RX_INT_PRIORITY, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
        DMA_UART2_RX_INT_PRIORITY, 
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
        DMA_UART3_RX_INT_PRIORITY,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
        DMA_UART4_RX_INT_PRIORITY,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
        DMA_UART5_RX_INT_PRIORITY
    #endif
};
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned char ucUART_mask[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0}; // used to mask 7 bit characters
#if defined TRUE_UART_TX_2_STOPS
    static unsigned char ucStops[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if defined SERIAL_INTERFACE && defined SERIAL_SUPPORT_DMA_RX
    static unsigned long ulSerialDMA_rules[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0}; // {214}
    #if defined SERIAL_SUPPORT_DMA_RX_FREERUN
    static unsigned long ulDMA_progress[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
        #if (defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA // {209}
    static QUEUE_TRANSFER RxModulo[UARTS_AVAILABLE + LPUARTS_AVAILABLE];
        #endif
    #endif
#endif
#if defined SUPPORT_HW_FLOW && (defined KINETIS_KE || defined KINETIS_KL)
    static unsigned char ucRTS_neg[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
    static unsigned char ucReportEndOfFrame[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION        // {208}
    static unsigned long ulInterCharTxDelay[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
    static void fnStopTxTimer(int Channel);
#endif
#if defined UART_HW_TRIGGERED_MODE_SUPPORTED
    static unsigned char ucTriggeredTX_mode[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0}; // {213}
#endif
#if defined SERIAL_SUPPORT_RX_DMA_BREAK || defined UART_BREAK_SUPPORT
    static unsigned char ucBreakSynchronised[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if defined USER_DEFINED_UART_RX_HANDLER
    static int (*fnUserRxIrq[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(unsigned char, QUEUE_LIMIT) = {0};
#endif
#if defined USER_DEFINED_UART_RX_BREAK_DETECTION
    static int (*fnUserRxBreakIrq[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(QUEUE_LIMIT) = {0};
#endif
#if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
    static void (*fnUserTxEndIrq[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(QUEUE_LIMIT) = {0};
#endif


/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */


// Collect a pointer to referenced UART/LPUART control block
//
static void *fnSelectChannel(QUEUE_HANDLE Channel)
{
    static const void *uart_control_block[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
    #if UARTS_AVAILABLE > 0 && (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
                                                              (void *)UART0_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
                                                              (void *)UART1_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)
                                                              (void *)UART2_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 3 && (LPUARTS_AVAILABLE < 4 || defined LPUARTS_PARALLEL)
                                                              (void *)UART3_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 4 && (LPUARTS_AVAILABLE < 5 || defined LPUARTS_PARALLEL)
                                                              (void *)UART4_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 5 && (LPUARTS_AVAILABLE < 6 || defined LPUARTS_PARALLEL)
                                                              (void *)UART5_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 0
                                                              (void *)LPUART0_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 1
                                                              (void *)LPUART1_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE == 2 && UARTS_AVAILABLE == 1        // KL43/KL27
                                                              (void *)UART2_BLOCK,
    #elif LPUARTS_AVAILABLE > 2
                                                              (void *)LPUART2_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 3
                                                              (void *)LPUART3_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 4
                                                              (void *)LPUART4_BLOCK,
    #endif
    };
    return (void *)uart_control_block[Channel];
}


/* =================================================================== */
/*                     LPUART interrupt handlers                       */
/* =================================================================== */

    #if LPUARTS_AVAILABLE > 0
// Generic LPUART interrupt handler
//
static __interrupt void _LPUART_interrupt(KINETIS_LPUART_CONTROL *ptrLPUART, int LPUART_Reference) // generic LPUART interrupt handler
{
    unsigned long ulState = ptrLPUART->LPUART_STAT;                      // status register on entry to the interrupt routine
    if (((ulState & LPUART_STAT_RDRF) & ptrLPUART->LPUART_CTRL) != 0) {  // if reception interrupt flag is set and the reception interrupt is enabled
        unsigned long ulRxData = ptrLPUART->LPUART_DATA;                 // read the received data (which resets the reception interrupt flag)
        #if defined UART_BREAK_SUPPORT
        if (((ptrLPUART->LPUART_BAUD & LPUART_BAUD_LBKDIE) == 0) || (ucBreakSynchronised[LPUART_Reference] != 0)) { // if in break framing mode we ignore reception until a first break has been detected
        #endif
            ulRxData &= ucUART_mask[LPUART_Reference];                   // mask the received data with the character width
        #if defined USER_DEFINED_UART_RX_HANDLER
            if ((fnUserRxIrq[LPUART_Reference] == 0) || (fnUserRxIrq[LPUART_Reference]((unsigned char)ulRxData, (QUEUE_LIMIT)LPUART_Reference) != 0)) { // if a user reception handler is installed it is called - if it doesn't decide to handle the data the standard handler is used
        #endif
                fnSciRxByte((unsigned char)ulRxData, (QUEUE_LIMIT)LPUART_Reference); // receive data interrupt - the reception data (masked with character width)
        #if defined USER_DEFINED_UART_RX_HANDLER
            }
        #endif
        #if defined UART_BREAK_SUPPORT
        }
        #endif
        #if defined _WINDOWS
        ptrLPUART->LPUART_STAT &= ~(LPUART_STAT_RDRF);                   // simulate reset of interrupt flag (reset by read of the data)
        #endif
        ulState = ptrLPUART->LPUART_STAT;                                // update the status register
        if ((ulState & LPUART_STAT_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
            ptrLPUART->LPUART_STAT = ulState;                            // {210} write the OR flag back to clear it and allow further operation
        }
    }
        #if defined UART_EXTENDED_MODE && defined UART_SUPPORT_IDLE_LINE_INTERRUPT
    else if (((ulState & LPUART_STAT_IDLE) & ptrLPUART->LPUART_CTRL) != 0) { // idle line interrupt flagged and enabled
        WRITE_ONE_TO_CLEAR(ptrLPUART->LPUART_STAT, LPUART_STAT_IDLE);    // write the IDLE flag back to clear it
        fnSciRxIdle((QUEUE_LIMIT)LPUART_Reference);                      // call the idle line interrupt handler
    }
        #endif
    if (((ulState & LPUART_STAT_TDRE) & ptrLPUART->LPUART_CTRL) != 0) {  // if transmit buffer is empty and the transmit interrupt is enabled
        fnSciTxByte((QUEUE_LIMIT)LPUART_Reference);                      // transmit data empty interrupt - write next byte, if waiting
    }
        #if defined SUPPORT_LOW_POWER || defined UART_HW_TRIGGERED_MODE_SUPPORTED || ((defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE)
    if (((ulState & LPUART_STAT_TC) & ptrLPUART->LPUART_CTRL) != 0) {    // if transmit complete interrupt after final byte transmission is enabled and flag set
        ptrLPUART->LPUART_CTRL &= ~(LPUART_CTRL_TCIE);                   // disable the interrupt
            #if defined UART_HW_TRIGGERED_MODE_SUPPORTED
        if (ucTriggeredTX_mode[LPUART_Reference] != 0) {                 // {213}
            ptrLPUART->LPUART_CTRL &= ~(LPUART_CTRL_TE);                 // disable the transmitter before preparing next message
            fnSciTxByte(LPUART_Reference);                               // prepare next message if waiting
        }
            #endif
            #if defined SUPPORT_LOW_POWER
        PROTECTED_CLEAR_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << LPUART_Reference)); // confirmation that the final byte has been sent out on the line so the LPUART no longer needs a UART clock (stop mode doesn't needed to be blocked)
            #endif
            #if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
        if (fnUserTxEndIrq[LPUART_Reference] != 0) {                    // if a transmission complete handler is installed it is called
            fnUserTxEndIrq[LPUART_Reference]((QUEUE_LIMIT)LPUART_Reference);
        }
            #elif (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
        if (ucReportEndOfFrame[LPUART_Reference] != 0) {                // if the end of frame call-back is enabled
            fnUARTFrameTermination(LPUART_Reference);
        }
            #endif
    }
        #endif
        #if defined SERIAL_SUPPORT_RX_DMA_BREAK || defined UART_BREAK_SUPPORT
    if ((ulState & LPUART_STAT_LBKDIF) != 0) {                           // if a break has been detected
        WRITE_ONE_TO_CLEAR(LPUART0_STAT, LPUART_STAT_LBKDIF);            // clear the flag
        if ((ptrLPUART->LPUART_BAUD & LPUART_BAUD_LBKDIE) != 0) {        // if the interrupt is to be handled
            #if defined SERIAL_SUPPORT_RX_DMA_BREAK
            fnSciRxByte((unsigned char)(ucBreakSynchronised[LPUART_Reference] == 0), (QUEUE_LIMIT)LPUART_Reference); // handle the reception (it will ignore any data received before the first synchronisation)
            #else
            if (ucBreakSynchronised[LPUART_Reference] != 0) {            // if not the first break after enabling the receiver
                #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
                if ((fnUserRxBreakIrq[LPUART_Reference] == 0) || (fnUserRxBreakIrq[LPUART_Reference]((QUEUE_LIMIT)LPUART_Reference) != 0)) {
                #endif
                    fnSciRxMsg((QUEUE_LIMIT)LPUART_Reference);           // break signals the end of a reception frame
                #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
                }
                #endif
            }
            #endif
            ucBreakSynchronised[LPUART_Reference] = 1;                   // from this point on data will be accepted
        }
    }
        #endif
}

// LPUART 0 interrupt handler
//
static __interrupt void _LPSCI0_Interrupt(void)                          // LPUART 0 interrupt
{
        #if defined LPUARTS_PARALLEL                                     // if both LPUARTs and UARTs are counted from 0 in the device
            #define LPUART0_CH_NUMBER     UARTS_AVAILABLE                // we reference the LPUART as continuation of UARTs
        #else
            #define LPUART0_CH_NUMBER     0                              // referencing matches the LPUART number
        #endif
    _LPUART_interrupt((KINETIS_LPUART_CONTROL *)LPUART0_BLOCK, LPUART0_CH_NUMBER); // call generic LPUART handler
}
    #endif

    #if LPUARTS_AVAILABLE > 1
// LPUART 1 interrupt handler
//
static __interrupt void _LPSCI1_Interrupt(void)                          // LPUART 1 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART1_CH_NUMBER     (UARTS_AVAILABLE + 1)
        #else
            #define LPUART1_CH_NUMBER     1
        #endif
    _LPUART_interrupt((KINETIS_LPUART_CONTROL *)LPUART1_BLOCK, LPUART1_CH_NUMBER); // call generic LPUART handler
}
    #endif

    #if LPUARTS_AVAILABLE > 2
// LPUART 2 interrupt handler
//
static __interrupt void _LPSCI2_Interrupt(void)                          // LPUART 2 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART2_CH_NUMBER     (UARTS_AVAILABLE + 2)
        #else
            #define LPUART2_CH_NUMBER     2
        #endif
    _LPUART_interrupt((KINETIS_LPUART_CONTROL *)LPUART2_BLOCK, LPUART2_CH_NUMBER); // call generic LPUART handler
}
    #endif

    #if LPUARTS_AVAILABLE > 3
// LPUART 3 interrupt handler
//
static __interrupt void _LPSCI3_Interrupt(void)                          // LPUART 3 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART3_CH_NUMBER     (UARTS_AVAILABLE + 3)
        #else
            #define LPUART3_CH_NUMBER     3
        #endif
    _LPUART_interrupt((KINETIS_LPUART_CONTROL *)LPUART3_BLOCK, LPUART3_CH_NUMBER); // call generic LPUART handler
}
    #endif

    #if LPUARTS_AVAILABLE > 4
// LPUART 4 interrupt handler
//
static __interrupt void _LPSCI4_Interrupt(void)                          // LPUART 4 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART4_CH_NUMBER     (UARTS_AVAILABLE + 4)
        #else
            #define LPUART4_CH_NUMBER     4
        #endif
    _LPUART_interrupt((KINETIS_LPUART_CONTROL *)LPUART4_BLOCK, LPUART4_CH_NUMBER); // call generic LPUART handler
}
    #endif


/* =================================================================== */
/*                      UART interrupt handlers                        */
/* =================================================================== */

#if UARTS_AVAILABLE > 0
    #define UART_DMA_RX_MODE   0x1
    #define UART_DMA_TX_MODE   0x2
    #if !defined fnUART0_HANDLER                                         // default reception handler (this can be defined by user code if required)
        #define fnUART0_HANDLER(data, channel) fnSciRxByte(data, channel)
    #endif
    #if !defined fnUART1_HANDLER                                         // default reception handler (this can be defined by user code if required)
        #define fnUART1_HANDLER(data, channel) fnSciRxByte(data, channel)
    #endif
    #if !defined fnUART2_HANDLER                                         // default reception handler (this can be defined by user code if required)
        #define fnUART2_HANDLER(data, channel) fnSciRxByte(data, channel)
    #endif
    #if !defined fnUART3_HANDLER                                         // default reception handler (this can be defined by user code if required)
        #define fnUART3_HANDLER(data, channel) fnSciRxByte(data, channel)
    #endif
    #if !defined fnUART4_HANDLER                                         // default reception handler (this can be defined by user code if required)
        #define fnUART4_HANDLER(data, channel) fnSciRxByte(data, channel)
    #endif
    #if !defined fnUART5_HANDLER                                         // default reception handler (this can be defined by user code if required)
        #define fnUART5_HANDLER(data, channel) fnSciRxByte(data, channel)
    #endif

// Generic UART interrupt handler
//
static __interrupt void _UART_interrupt(KINETIS_UART_CONTROL *ptrUART, int UART_Reference, int iFlags)
{
    unsigned char ucState = ptrUART->UART_S1;                            // status register on entry to the interrupt routine
        #if defined SERIAL_SUPPORT_DMA                                   // {8}
    if ((iFlags & UART_DMA_RX_MODE) == 0) {                              // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if (((ucState & UART_S1_RDRF) & ptrUART->UART_C2) != 0) {        // reception interrupt flag is set and the reception interrupt is enabled
            unsigned char ucRxData = ptrUART->UART_D;                    // read the received data (which resets the reception interrupt flag and framing error)
        #if defined _WINDOWS
            ptrUART->UART_S1 &= ~(UART_S1_FE);
        #endif
        #if defined UART_BREAK_SUPPORT
            if ((ucState & UART_S1_FE) != 0) {                           // framing error signifies that a '0' was received at a stop bits location and it is used to detect a break character
                if ((ptrUART->UART_BDH & UART_BDH_LBKDIE) != 0) {        // if break character operation is enabled (this register flag is used to signal the mode and not enable LIN break operation)
                    if (ucBreakSynchronised[UART_Reference] != 0) {      // if not the first break after enabling the receiver
                        #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
                        if ((fnUserRxBreakIrq[UART_Reference] == 0) || (fnUserRxBreakIrq[UART_Reference]((QUEUE_LIMIT)UART_Reference) != 0)) {
                        #endif
                            fnSciRxMsg((QUEUE_LIMIT)UART_Reference);     // break signals the end of a reception frame
                        #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
                        }
                        #endif
                    }
                    ucBreakSynchronised[UART_Reference] = 1;             // a first break character has been received and from this point on we accept data
                }
            }
            else if (((ptrUART->UART_BDH & UART_BDH_LBKDIE) == 0) || (ucBreakSynchronised[UART_Reference] != 0)) { // if in break framing mode we ignore reception until a first break has been detected
        #endif
                ucRxData &= ucUART_mask[UART_Reference];                 // mask the received data with the character width
        #if defined USER_DEFINED_UART_RX_HANDLER
                if ((fnUserRxIrq[UART_Reference] == 0) || (fnUserRxIrq[UART_Reference](ucRxData, (QUEUE_LIMIT)UART_Reference) != 0)) { // if a user reception handler is installed it is called - if it doesn't decide to handle the data the standard handler is used
        #endif
                    switch (UART_Reference) {                            // for compatibility (to be phased out)
                    case 0:
                        fnUART0_HANDLER(ucRxData, 0);                    // receive data interrupt - read the byte (masked with character width)
                        break;
                    case 1:
                        fnUART1_HANDLER(ucRxData, 1);                    // receive data interrupt - read the byte (masked with character width)
                        break;
                    case 2:
                        fnUART0_HANDLER(ucRxData, 2);                    // receive data interrupt - read the byte (masked with character width)
                        break;
                    case 3:
                        fnUART1_HANDLER(ucRxData, 3);                    // receive data interrupt - read the byte (masked with character width)
                        break;
                    case 4:
                        fnUART0_HANDLER(ucRxData, 4);                    // receive data interrupt - read the byte (masked with character width)
                        break;
                    case 5:
                        fnUART1_HANDLER(ucRxData, 5);                    // receive data interrupt - read the byte (masked with character width)
                        break;
                    }
        #if defined USER_DEFINED_UART_RX_HANDLER
                }
        #endif
        #if defined UART_BREAK_SUPPORT
            }
        #endif
        #if defined _WINDOWS
            ptrUART->UART_S1 &= ~(UART_S1_RDRF);                         // simulate reset of interrupt flag due to data read
        #endif
            ucState = ptrUART->UART_S1;                                  // {92} update the status register
            if ((ucState & UART_S1_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)ptrUART->UART_D;                                   // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
    if ((iFlags & UART_DMA_TX_MODE) == 0) {                              // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if (((ucState & (UART_S1_TDRE | UART_S1_TC)) & ptrUART->UART_C2) != 0) { // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte((QUEUE_LIMIT)UART_Reference);                    // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[UART_Reference] != 0) {                          // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER || defined UART_HW_TRIGGERED_MODE_SUPPORTED || ((defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE) // {96}
    if (((ptrUART->UART_C2 & UART_C2_TCIE) != 0) && ((ptrUART->UART_S1 & UART_S1_TC) != 0)) { // transmit complete interrupt after final byte transmission together with low power operation
        ptrUART->UART_C2 &= ~(UART_C2_TCIE);                             // disable the interrupt
            #if defined SUPPORT_LOW_POWER
        PROTECTED_CLEAR_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << UART_Reference)); // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
            #endif
            #if defined UART_HW_TRIGGERED_MODE_SUPPORTED                 // {213}
        if (ucTriggeredTX_mode[UART_Reference] != 0) {
            ptrUART->UART_C2 &= ~(UART_C2_TE);                           // disable the transmitter before preparing next message
            fnSciTxByte((QUEUE_LIMIT)UART_Reference);                    // prepare next message if waiting
        }
            #endif
            #if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
        if (fnUserTxEndIrq[UART_Reference] != 0) {                       // if a transmission complete handler is installed it is called
            fnUserTxEndIrq[UART_Reference]((QUEUE_LIMIT)UART_Reference);
        }
            #elif (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
        if (ucReportEndOfFrame[UART_Reference] != 0) {                   // if the end of frame call-back is enabled
            fnUARTFrameTermination(UART_Reference);                      // {200}
        }
            #endif
    }
        #endif
        #if defined SERIAL_SUPPORT_RX_DMA_BREAK_ || defined UART_BREAK_SUPPORT_ // a method based on lin break detection is not used (see framing error code above)
    if ((ptrUART->UART_S2 & UART_S2_LBKDIF) != 0) {                      // if a break has been detected
        ptrUART->UART_S2 &= ~(UART_S2_LBKDE);                            // clear the flag (by writing with UART_S2_LBKDIF and disable the break detection because it otherwise blocks reception interrupts)
            #if defined _WINDOWS
        ptrUART->UART_S2 &= ~(UART_S2_LBKDIF);
            #endif
        if ((ptrUART->UART_BDH & UART_BDH_LBKDIE) != 0) {                // if the interrupt is to be handled
            #if defined SERIAL_SUPPORT_RX_DMA_BREAK
            fnSciRxByte((unsigned char)(ucBreakSynchronised[UART_Reference] == 0), (QUEUE_LIMIT)UART_Reference);// handle the reception (it will ignore any data received before the first synchronisation)
            #else
            if (ucBreakSynchronised[UART_Reference] != 0) {              // if not the first break after enabling the receiver
                #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
                if ((fnUserRxBreakIrq[UART_Reference] == 0) || (fnUserRxBreakIrq[UART_Reference]((QUEUE_LIMIT)UART_Reference) != 0)) {
                #endif
                    fnSciRxMsg((QUEUE_LIMIT)UART_Reference);             // break signals the end of a reception frame
                #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
                }
                #endif
            }
            #endif
            ucBreakSynchronised[UART_Reference] = 1;                     // from this point on data will be accepted
        }
    }
        #endif
}
#endif

#if UARTS_AVAILABLE > 0 && (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
// UART 0 interrupt handler
//
static __interrupt void _SCI0_Interrupt(void)                            // UART 0 interrupt
{
    int iFlags = 0;
    #if defined SERIAL_SUPPORT_DMA
    if ((UART0_C5 & UART_C5_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
    }
    if ((UART0_C5 & UART_C5_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
    #endif
    _UART_interrupt((KINETIS_UART_CONTROL *)UART0_BLOCK, 0, iFlags);     // call generic UART handler
}
#endif
#if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
// UART 1 interrupt handler
//
static __interrupt void _SCI1_Interrupt(void)                            // UART 1 interrupt
{
    int iFlags = 0;
    #if defined SERIAL_SUPPORT_DMA
        #if defined KINETIS_KL
    if ((UART1_C4 & UART_C4_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
}
    if ((UART1_C4 & UART_C4_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
        #else
    if ((UART1_C5 & UART_C5_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
    }
    if ((UART1_C5 & UART_C5_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
        #endif
    #endif
    _UART_interrupt((KINETIS_UART_CONTROL *)UART1_BLOCK, 1, iFlags);     // call generic UART handler
}
#endif
#if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
// UART 2 interrupt handler
//
static __interrupt void _SCI2_Interrupt(void)                            // UART 2 interrupt
{
    int iFlags = 0;
    #if defined SERIAL_SUPPORT_DMA
        #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
    if ((UART2_C4 & UART_C4_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
    }
    if ((UART2_C4 & UART_C4_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
        #else
    if ((UART2_C5 & UART_C5_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
    }
    if ((UART2_C5 & UART_C5_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
        #endif
    #endif
    _UART_interrupt((KINETIS_UART_CONTROL *)UART2_BLOCK, 2, iFlags);     // call generic UART handler
}
#endif
#if UARTS_AVAILABLE > 3
// UART 3 interrupt handler
//
static __interrupt void _SCI3_Interrupt(void)                            // UART 3 interrupt
{
    int iFlags = 0;
    #if defined SERIAL_SUPPORT_DMA
    if ((UART3_C5 & UART_C5_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;                                       // receiver is in DMA mode (else interrupt mode)
    }
    if ((UART3_C5 & UART_C5_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;                                      // transmitter is in DMA mode
    }
    #endif
    _UART_interrupt((KINETIS_UART_CONTROL *)UART3_BLOCK, 3, iFlags);     // call generic UART handler
}
#endif
#if UARTS_AVAILABLE > 4
// UART 4 interrupt handler
//
static __interrupt void _SCI4_Interrupt(void)                            // UART 4 interrupt
{
    int iFlags = 0;
    #if defined SERIAL_SUPPORT_DMA
    if ((UART4_C5 & UART_C5_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
    }
    if ((UART4_C5 & UART_C5_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
    #endif
    _UART_interrupt((KINETIS_UART_CONTROL *)UART4_BLOCK, 4, iFlags);     // call generic UART handler
}
#endif
#if UARTS_AVAILABLE > 5
// UART 5 interrupt handler
//
static __interrupt void _SCI5_Interrupt(void)                            // UART 5 interrupt
{
    int iFlags = 0;
    #if defined SERIAL_SUPPORT_DMA
    if ((UART5_C5 & UART_C5_RDMAS) != 0) {
        iFlags = UART_DMA_RX_MODE;
    }
    if ((UART5_C5 & UART_C5_TDMAS) != 0) {
        iFlags |= UART_DMA_TX_MODE;
    }
    #endif
    _UART_interrupt((KINETIS_UART_CONTROL *)UART5_BLOCK, 5, iFlags);     // call generic UART handler
}
#endif

// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE Channel, unsigned char ucTxByte)
{
    KINETIS_UART_CONTROL *uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(Channel);
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCI_send((QUEUE_HANDLE)(Channel - NUMBER_SERIAL), ucTxByte);// pass on to the external interface for transmission
        return 0;
    }
    #endif
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT & LPUART_STAT_TDRE) == 0) {
            return 1;                                                    // LPUART transmitter is presently active
        }
        #if defined SUPPORT_LOW_POWER                                    // {96}
        PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << Channel)); // mark that this UART is in use
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT;               // read the status register to clear the transmit complete flag when the transmit data register is written
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_DATA = ucTxByte;    // send the character
        #if defined _WINDOWS
        ucTxLast[Channel] = (unsigned char)((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_DATA; // back up the data written so that it can't get lost when rx data uses the simulated register
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT &= ~LPUART_STAT_TDRE; // mark transmitter presently not empty
        iInts |= (CHANNEL_0_SERIAL_INT << Channel);                      // flag that an interrupt is expected on this LPUART channel
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= (LPUART_CTRL_TIE); // enable the LPUART transmission interrupt
        #if UARTS_AVAILABLE > 0
    }
    else {                                                               // UART
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
        if ((uart_reg->UART_S1 & UART_S1_TDRE) == 0) {                   // check whether transmit buffer is presently empty
            return 1;                                                    // UART transmitter is presently active
        }
        #if defined SUPPORT_LOW_POWER                                    // {96}
        PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << Channel)); // mark that this UART is in use
        (void)(uart_reg->UART_S1);                                       // read the status register to clear the transmit complete flag when the transmit data register is written
        #endif
        uart_reg->UART_D = ucTxByte;                                     // send the character
        #if defined _WINDOWS
        ucTxLast[Channel] = (unsigned char)uart_reg->UART_D;             // back up the data written so that it can't get lost when rx data uses the simulated register
        uart_reg->UART_S1 &= ~(UART_S1_TDRE | UART_S1_TC);               // mark transmitter presently not empty
        iInts |= (CHANNEL_0_SERIAL_INT << Channel);
        #endif
        #if defined TRUE_UART_TX_2_STOPS
        if (ucStops[Channel] != 0) {
            uart_reg->UART_C2 |= (UART_C2_TCIE);                         // enable UART transmission interrupt (transmission complete)
        }
        else {
            uart_reg->UART_C2 |= (UART_C2_TIE);                          // enable UART transmission interrupt (buffer empty)   
        }
        #else
        uart_reg->UART_C2 |= (UART_C2_TIE);                              // enable UART transmission interrupt (buffer empty)
        #endif
    #endif
    #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0
    }
    #endif
    return 0;                                                            // transmission started
}

// The TTY driver uses this call to reset/disable the transmit interrupt on the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        return;
    }
    #endif
    uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(Channel);
    #if defined SERIAL_SUPPORT_DMA                                       // {6}
        #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
    if (Channel == 0) {
        if (uart_reg->UART_C5 & UART_C5_TDMAS) {                         // if the transmitter is operating in DMA mode
            return;                                                      // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
        }
    }
    else {
        if (uart_reg->UART_MA1_C4 & UART_C4_TDMAS) {                     // if the transmitter is operating in DMA mode
            return;                                                      // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
        }
    }
        #elif LPUARTS_AVAILABLE > 0                                      // LPUART(s) available
            #if UARTS_AVAILABLE > 0                                      // both LPUART and UART available
    if (uart_type[Channel] == UART_TYPE_LPUART) {
        if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_BAUD & LPUART_BAUD_TDMAE) != 0) { // LPUART tx is being used in DMA mode
            return;
        }
    }
    else {
        if ((uart_reg->UART_C5 & UART_C5_TDMAS) != 0) {                  // if the transmitter is operating in DMA mode
            return;                                                      // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
        }
    }
            #else
    if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_BAUD & LPUART_BAUD_TDMAE) != 0) { // LPUART tx is being used in DMA mode
        return;
    }
            #endif
        #else
    if ((uart_reg->UART_C5 & UART_C5_TDMAS) != 0) {                      // if the transmitter is operating in DMA mode
        return;                                                          // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
    }
        #endif
    #endif                                                               // end if defined SERIAL_SUPPORT_DMA

    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // the channel is a LPUART
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} no more transmissions are required
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= (LPUART_CTRL_TCIE); // enable LPUART transmit complete interrupt to signal when the complete last character has been sent
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_TIE); // disable LPUART transmission interrupt
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
        #if defined SUPPORT_LOW_POWER                                    // {96} no more transmissions are required
            #if defined TRUE_UART_TX_2_STOPS
    if (ucStops[Channel] != 0) {                                         // channel operating in true 2 stop bit mode (using the transmit complete interrupt rather than the buffer empty interrupt)
        uart_reg->UART_C2 &= ~(UART_C2_TIE | UART_C2_TCIE);              // disable the transmission interrupts since the buffer has been fully sent
        PROTECTED_CLEAR_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << Channel)); // this transmitter is now idle since its final bit has been sent (stop mode is no longer blocked)
        return;
    }
    else {
        uart_reg->UART_C2 |= (UART_C2_TCIE);                             // enable UART transmit complete interrupt to signal when the complete last character has been sent
    }
            #else
    uart_reg->UART_C2 |= (UART_C2_TCIE);                                 // enable UART transmit complete interrupt to signal when the complete last character has been sent
            #endif
        #elif (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
    if (ucReportEndOfFrame[Channel] != 0) {                              // if an end of frame interrupt is required
        uart_reg->UART_C2 |= (UART_C2_TCIE);                             // {200} enable UART transmit complete interrupt to signal when the complete last character has been sent
    }
        #endif
    uart_reg->UART_C2 &= ~(UART_C2_TIE);                                 // disable UART transmission interrupt
    #endif
}

/* =================================================================== */
/*                LPUART/UART Tx DMA interrupt handlers                */
/* =================================================================== */

#if defined SERIAL_SUPPORT_DMA                                           // {6}
    #if LPUARTS_AVAILABLE > 0
static __interrupt void _lpuart_tx_dma_Interrupt(KINETIS_LPUART_CONTROL *ptrLPUART, int iLPUART_reference)
{
        #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION// {208}
    if (ulInterCharTxDelay[iLPUART_reference] != 0) {
        fnStopTxTimer(iLPUART_reference);                                // stop the periodic timer that controlled byte transmissions
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    PROTECTED_CLEAR_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << iLPUART_reference)); // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte((QUEUE_HANDLE)iLPUART_reference);                        // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & (UART0_TX_CLK_REQUIRED << iLPUART_reference)) == 0) { // if no further transmission was started we need to wait until the final byte has been transferred
        PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << iLPUART_reference)); // block stop mode until the final interrupt arrives
        ptrLPUART->LPUART_CTRL |= LPUART_CTRL_TCIE;                      // enable the LPUART transmit complete flag interrupt to detect transmission completion
    }
        #elif defined KINETIS_KL && defined UART_FRAME_END_COMPLETE
    if (ucReportEndOfFrame[iLPUART_reference] != 0) {                    // if an end of frame interrupt is required
        ptrLPUART->LPUART_CTRL |= LPUART_CTRL_TCIE;                      // enable LPUART transmit complete interrupt to signal when the complete last character has been sent
    }
        #endif
}
    #endif

    #if UARTS_AVAILABLE > 0
static __interrupt void _uart_tx_dma_Interrupt(KINETIS_UART_CONTROL *ptrUART, int iUART_reference)
{
        #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION// {208}
    if (ulInterCharTxDelay[iUART_reference] != 0) {
        fnStopTxTimer(iUART_reference0);                                 // stop the periodic timer that controlled byte transmissions
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    PROTECTED_CLEAR_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << iUART_reference)); // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte((QUEUE_HANDLE)iUART_reference);                          // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & (UART0_TX_CLK_REQUIRED << iUART_reference)) == 0) { // if no further transmission was started we need to wait until the final byte has been transferred
        PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << iUART_reference)); // block stop mode until the final interrupt arrives
        ptrUART->UART_C2 |= UART_C2_TCIE;                                // enable the UART transmit complete flag interrupt to detect transmission completion
    }
        #elif defined KINETIS_KL && defined UART_FRAME_END_COMPLETE
    if (ucReportEndOfFrame[iUART_reference] != 0) {                      // if an end of frame interrupt is required
        ptrUART->UART_C2 |= UART_C2_TCIE;                                // enable UART transmit complete interrupt to signal when the complete last character has been sent
    }
        #endif
}
    #endif

// Transmission UART/LPUART DMA completion interrupt handlers
//
static __interrupt void _uart0_tx_dma_Interrupt(void)
{
    #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART0_BLOCK, 0);// handle LPUART method
    #else
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART0_BLOCK, 0);      // handle UART method
    #endif
}

    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
static __interrupt void _uart1_tx_dma_Interrupt(void)
{
        #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART1_BLOCK, 1);// handle LPUART method
        #elif LPUARTS_AVAILABLE == 1 && !defined LPUARTS_PARALLEL
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART0_BLOCK, 1);      // handle UART method on UART0
        #else
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART1_BLOCK, 1);      // handle UART method
        #endif
}
    #endif

    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
static __interrupt void _uart2_tx_dma_Interrupt(void)
{
        #if LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART2_BLOCK, 2);// handle LPUART method
        #elif LPUARTS_AVAILABLE == 2 && defined LPUARTS_PARALLEL
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART0_BLOCK, 2);      // handle UART method on UART0
        #else
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART2_BLOCK, 2);      // handle UART method
        #endif
}
    #endif

    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
static __interrupt void _uart3_tx_dma_Interrupt(void)
{
        #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART3_BLOCK, 3);// handle LPUART method
        #elif LPUARTS_AVAILABLE == 3 && !defined LPUARTS_PARALLEL
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART0_BLOCK, 3);      // handle UART method on UART0
        #elif UARTS_AVAILABLE == 3
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART0_BLOCK, 3);// handle LPUART method
        #else
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART3_BLOCK, 3);      // handle UART method
        #endif
}
    #endif

    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
static __interrupt void _uart4_tx_dma_Interrupt(void)
{
        #if LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART4_BLOCK, 4);// handle LPUART method
        #else
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART4_BLOCK, 4);      // handle UART method
        #endif
}
    #endif

    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
static __interrupt void _uart5_tx_dma_Interrupt(void)
{
        #if LPUARTS_AVAILABLE == 1
    _lpuart_tx_dma_Interrupt((KINETIS_LPUART_CONTROL *)LPUART0_BLOCK, 5);// handle LPUART method
        #else
    _uart_tx_dma_Interrupt((KINETIS_UART_CONTROL *)UART5_BLOCK, 5);      // handle UART method
        #endif
}
    #endif

static void (*_uart_tx_dma_Interrupts[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(void) = {
    _uart0_tx_dma_Interrupt,
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    _uart1_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    _uart2_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    _uart3_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    _uart4_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    _uart5_tx_dma_Interrupt
    #endif
};

#if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION        // {208}
// Start the periodic timer that will control subsequent byte transmissions
//
static void fnStartTxTimer(int Channel, unsigned long ulDelay)
{
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.int_priority = 0;                                          // not used
    pit_setup.count_delay = ulDelay;                                     // transmission time base
    pit_setup.mode = (PIT_PERIODIC);                                     // periodic DMA trigger
    pit_setup.int_handler = 0;                                           // no interrupt since the PIT will be used for triggering DMA
    pit_setup.ucPIT = (unsigned char)Channel;                            // use PIT equal to the channel number
    fnConfigureInterrupt((void *)&pit_setup);                            // configure and start PIT
    *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT0 + Channel) | DMAMUX_CHCFG_ENBL); // connect PIT to DMA channel
    #if defined _WINDOWS
    if (UART_DMA_TX_CHANNEL[Channel] != pit_setup.ucPIT) {
        _EXCEPTION("PITs can only trigger DMA on their DMA channel number");
    }
    #endif
}

static void fnStopTxTimer(int Channel)
{
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.mode = (PIT_STOP);                                         // stop the timer
    pit_setup.ucPIT = (unsigned char)Channel;                            // the PIT channel equal to the channel number
    fnConfigureInterrupt((void *)&pit_setup);                            // stop PIT
}
#endif

// Start transfer of a block via DMA
//
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE Channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length)
{
        #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION && defined _WINDOWS
    int iNoDMA_int = 0;
        #endif
        #if defined SUPPORT_LOW_POWER || (defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION) || defined _WINDOWS
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
        #endif
        #if (defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA // {81}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += UART_DMA_TX_CHANNEL[Channel];
        #else
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += UART_DMA_TX_CHANNEL[Channel];
        #endif
        #if LPUARTS_AVAILABLE > 0
            #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
            #endif
            #if defined SUPPORT_LOW_POWER
        (void)(((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT);       // read the status register to clear the transmit complete flag when the transmit data register is written
                #if defined SUPPORT_LOW_POWER    
        PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << Channel)); // mark that stop mode should be avoided until the transmit activity has completed
                #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_TCIE); // disable transmit complete interrupt
            #endif
            #if UARTS_AVAILABLE > 0
    }
    else {                                                               // UART
            #endif
        #endif
        #if UARTS_AVAILABLE > 0
            #if defined SUPPORT_LOW_POWER
        (void)(uart_reg->UART_S1);                                       // read the status register to clear the transmit complete flag when the transmit data register is written
        uart_reg->UART_C2 &= ~(UART_C2_TCIE);                            // disable transmit complete interrupt
            #endif
        #endif
        #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0
    }
        #endif    
        #if (defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA
            #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION // {208}
    if ((tx_length > 1) && (ulInterCharTxDelay[Channel] != 0)) {         // if timed transmissions are required on this channel (linear buffer is always assumed and 7/8 bit characters)
       ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                           // clear the DONE flag and clear errors etc. before starting the HW timer to trigger transfers
        fnStartTxTimer(Channel, ulInterCharTxDelay[Channel]);            // start the periodic timer that will control subsequent byte transmissions
                #if defined _WINDOWS
        iNoDMA_int = 1;
                #endif
    }
            #endif
    ptrDMA->DMA_DSR_BCR = (tx_length & DMA_DSR_BCR_BCR_MASK);            // the number of service requests (the number of bytes to be transferred)
    ptrDMA->DMA_SAR = (unsigned long)ptrStart;                           // source is tty output buffer
            #if defined SUPPORT_LOW_POWER
    PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << Channel)); // mark that stop mode should be avoided until the transmit activity has completed
            #endif
    ptrDMA->DMA_DCR |= DMA_DCR_ERQ;                                      // enable request source
        #else
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = tx_length; // the number of service requests (the number of bytes to be transferred)
    ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)ptrStart;                 // source is tty output buffer's present location
            #if defined SUPPORT_LOW_POWER
    PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (UART0_TX_CLK_REQUIRED << Channel)); // mark that stop mode should be avoided until the transmit activity has completed
            #endif
            #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION // {208}
    if ((tx_length > 1) && (ulInterCharTxDelay[Channel] != 0)) {         // if timed transmissions are required on this channel (linear buffer is always assumed and 7/8 bit characters)
        fnStartTxTimer(Channel, ulInterCharTxDelay[Channel]);            // start the periodic timer that will control subsequent byte transmissions
                #if defined _WINDOWS
        iNoDMA_int = 1;
                #endif
    }
            #endif
    ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, UART_DMA_TX_CHANNEL[Channel]);// enable request source in order to start DMA activity
        #endif 
        #if defined _WINDOWS                                             // simulation
            #if LPUARTS_AVAILABLE > 0
                #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // if an LPUART is being used
                #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT &= ~(LPUART_STAT_TDRE | LPUART_STAT_TC); // mark transmitter presently not empty
                #if UARTS_AVAILABLE > 0
    }
    else {
        uart_reg->UART_S1 &= ~(UART_S1_TDRE | UART_S1_TC);               // mark transmitter presently not empty
    }
                #endif
            #elif UARTS_AVAILABLE > 0
    uart_reg->UART_S1 &= ~(UART_S1_TDRE | UART_S1_TC);                   // mark transmitter presently not empty
            #endif
            #if ((!defined KINETIS_KL && !defined KINETIS_KM) || defined DEVICE_WITH_eDMA)
    ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_ACTIVE;                       // trigger activity
            #endif
            #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION
    if (iNoDMA_int != 0) {
        return tx_length;
    }
            #endif
    iDMA |= (DMA_CONTROLLER_0 << UART_DMA_TX_CHANNEL[Channel]);          // activate first DMA request
        #endif
    return tx_length;
}

        #if (defined SERIAL_SUPPORT_XON_XOFF || defined SUPPORT_HW_FLOW)
extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue)
{
    return 0;
}
        #endif
    #endif

/* =================================================================== */
/*                LPUART/UART Rx DMA interrupt handlers                */
/* =================================================================== */

    #if defined SERIAL_SUPPORT_DMA_RX
static __interrupt void _uart0_rx_dma_Interrupt(void)
{
    fnSciRxByte(0, 0);                                                   // tty block ready for read
}

        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
static __interrupt void _uart1_rx_dma_Interrupt(void)
{
    fnSciRxByte(0, 1);                                                   // tty block ready for read
}
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
static __interrupt void _uart2_rx_dma_Interrupt(void)
{
    fnSciRxByte(0, 2);                                                   // tty block ready for read
}
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
static __interrupt void _uart3_rx_dma_Interrupt(void)
{
    fnSciRxByte(0, 3);                                                   // tty block ready for read
}
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
static __interrupt void _uart4_rx_dma_Interrupt(void)
{
    fnSciRxByte(0, 4);                                                   // tty block ready for read
}
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
static __interrupt void _uart5_rx_dma_Interrupt(void)
{
    fnSciRxByte(0, 5);                                                   // tty block ready for read
}
        #endif

static void (*_uart_rx_dma_Interrupt[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(void) = {
    _uart0_rx_dma_Interrupt,
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    _uart1_rx_dma_Interrupt,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    _uart2_rx_dma_Interrupt,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    _uart3_rx_dma_Interrupt,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    _uart4_rx_dma_Interrupt,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    _uart5_rx_dma_Interrupt
        #endif
};

        #if defined SERIAL_SUPPORT_DMA_RX_FREERUN
// Check the progress of the channel's free-running DMA reception and update the TTY character account accordingly
// - retrigger the DMA max. count on each check so that it never terminates
//
static void fnCheckFreerunningDMA_reception(int channel, QUEQUE *tty_queue) // {209}
{
            #if (defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA // {211}
    unsigned long ulDMA_rx;
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += UART_DMA_RX_CHANNEL[channel];                              // set the DMA channel that the UART channel is using
    ulDMA_rx = ptrDMA->DMA_DAR;                                          // snap-shot of DMA reception progress
    if (ulDMA_progress[channel] <= ulDMA_rx) {
        tty_queue->chars += (QUEUE_TRANSFER)(ulDMA_rx - ulDMA_progress[channel]); // add the extra number of characters received by DMA since last check
    }
    else {
        tty_queue->chars += (QUEUE_TRANSFER)(RxModulo[channel] - (ulDMA_progress[channel] - ulDMA_rx)); // add the extra number of characters received by DMA since last check
    }
    ulDMA_progress[channel] = ulDMA_rx;                                  // remember this check state for future comparisons
    ptrDMA->DMA_DSR_BCR |= (0xffff0);                                    // retrigger maximum DMA transfer at each poll
            #else
    unsigned short usDMA_rx;
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += UART_DMA_RX_CHANNEL[channel];
    usDMA_rx = ptrDMA_TCD->DMA_TCD_CITER_ELINK;                          // snap-shot of DMA reception progress
    if (ulDMA_progress[channel] >= usDMA_rx) {
        tty_queue->chars += (QUEUE_TRANSFER)(ulDMA_progress[channel] - usDMA_rx); // the extra number of characters received by DMA since last check
    }
    else {
        tty_queue->chars += (QUEUE_TRANSFER)ulDMA_progress[channel];
        tty_queue->chars += (ptrDMA_TCD->DMA_TCD_BITER_ELINK - usDMA_rx); // the extra number of characters received by DMA since last check
    }
    ulDMA_progress[channel] = usDMA_rx;                                  // remember the check state
            #endif
}
        #endif
// Configure DMA for reception to a free-running modulo buffer and then enable reception (also configuring the RXD input)
//
static void fnEnableRxAndDMA(int channel, unsigned long buffer_length, unsigned long buffer_address, void *uart_data_reg) // {209}
{
        #if defined KINETIS_KM                                           // UART0/1 an UART2/3 share channel assignment, resticted to certain DMAMUX channels
    unsigned short usDMAMUX;
    if (channel < 2) {
        usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART0_RX);                      // valid for UART0 and UART1
    }
    else {
        usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART2_RX);                      // valid for UART2 and UART3
    }
            #if defined _WINDOWS
    switch (channel) {
    case 0:
        if ((UART_DMA_RX_CHANNEL[channel] != 0) && (UART_DMA_RX_CHANNEL[channel] != 3)) {
            _EXCEPTION("UART0 Rx is only possible of DMA channels 0 and 3!");
        }
        break;
    case 1:
        if ((UART_DMA_RX_CHANNEL[channel] != 1) && (UART_DMA_RX_CHANNEL[channel] != 2)) {
            _EXCEPTION("UART1 Rx is only possible of DMA channels 1 and 2!");
        }
        break;
    case 2:
        if ((UART_DMA_RX_CHANNEL[channel] != 2) && (UART_DMA_RX_CHANNEL[channel] != 3)) {
            _EXCEPTION("UART2 Rx is only possible of DMA channels 2 and 3!");
        }
        break;
    case 3:
        if ((UART_DMA_RX_CHANNEL[channel] != 0) && (UART_DMA_RX_CHANNEL[channel] != 1)) {
            _EXCEPTION("UART3 Rx is only possible of DMA channels 0 and 1!");
        }
        break;
    }
            #endif
        #else
            #if defined DMAMUX0_CHCFG_SOURCE_UART0_RX
    unsigned short usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART0_RX + (2 * channel));
            #else
    unsigned short usDMAMUX = (DMAMUX0_CHCFG_SOURCE_LPUART0_RX + (2 * channel));
            #endif
            #if ((defined KINETIS_K21 || defined KINETIS_K22) && (UARTS_AVAILABLE > 4)) || defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66
    if (channel > 3) {                                                   // channels 4 and above each share DMA source for TX and RX
        usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART4_RX + (channel - 4));      // {215}
    }
            #endif
        #endif
    if ((ulSerialDMA_rules[channel] & DMA_HALF_BUFFER_INTERRUPT) != 0) { // for compatibility with the tty driver which is calling (it passes half the buffer length and we need the full buffer length)
        buffer_length *= 2;
    }
        #if defined SERIAL_SUPPORT_DMA_RX_FREERUN
            #if (defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA // {211}
    RxModulo[channel] = (QUEUE_TRANSFER)buffer_length;                   // this must be modulo 2 (16, 32, 64, 128...256k)
    ulDMA_progress[channel] = buffer_address;                            // destination must be modulo aligned
            #else
    ulDMA_progress[channel] = buffer_length;
            #endif
    if (ulSerialDMA_rules[channel] == 0) {                               // {214} if we are in free-running DMA Rx mode
        fnConfigDMA_buffer(UART_DMA_RX_CHANNEL[channel], usDMAMUX, buffer_length, uart_data_reg, (void *)buffer_address, (DMA_DIRECTION_INPUT | DMA_BYTES), 0, 0); // no interrupt handler
    }
    else {
        fnConfigDMA_buffer(UART_DMA_RX_CHANNEL[channel], usDMAMUX, buffer_length/*pars->Rx_tx_sizes.RxQueueSize*/, uart_data_reg, (void *)buffer_address, ulSerialDMA_rules[channel], _uart_rx_dma_Interrupt[channel], UART_DMA_RX_INT_PRIORITY[channel]); // configure DMA and enter interrupt on buffer completion (or half-completion)
    }
        #else
    fnConfigDMA_buffer(UART_DMA_RX_CHANNEL[channel], usDMAMUX, buffer_length, uart_data_reg, (void *)buffer_address, ulSerialDMA_rules[channel], _uart_rx_dma_Interrupt[channel], UART_DMA_RX_INT_PRIORITY[channel]); // configure DMA and enter interrupt on buffer completion (or half-completion)
        #endif
    fnDMA_BufferReset(UART_DMA_RX_CHANNEL[channel], DMA_BUFFER_START);   // enable DMA operation
    fnRxOn(channel);                                                     // configure receiver pin and enable reception and its interrupt/DMA
}

// The Kinetis buffer has been set up to run continuously in circular buffer mode and this routine is used to enable the receiver and DMA on first call and optionally to later poll the DMA transfer state
//
extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
        #if (UARTS_AVAILABLE > 0) || (LPUARTS_AVAILABLE > 0)
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(channel);           // select the UART/LPUART channel register set
        #endif
        #if LPUARTS_AVAILABLE > 0                                        // if the device has LPUART(s)
            #if UARTS_AVAILABLE > 0                                      // if also UARTs
    if (uart_type[channel] == UART_TYPE_LPUART) {                        // LPUART channel
            #endif
        if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL & LPUART_CTRL_RE) == 0) { // if receiver is not yet enabled
            fnEnableRxAndDMA(channel, rx_length, (unsigned long)ptrStart, (void *)&(((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_DATA)); // configure DMA and reception, including configuring the RXD input
        }
            #if defined SERIAL_SUPPORT_DMA_RX_FREERUN
        else if (rx_length == 0) {                                       // call to update DMA progress
            fnCheckFreerunningDMA_reception(channel, (QUEQUE *)ptrStart);
        }
            #endif
        return;
            #if UARTS_AVAILABLE > 0                                      // if also UARTs
    }
            #endif
        #endif                                                           // end LPUART support
        #if (UARTS_AVAILABLE > 0)                                        // if UARTs are available
    // UART channel
    //
    if ((uart_reg->UART_C2 & UART_C2_RE) == 0) {                         // if receiver not yet enabled
        fnEnableRxAndDMA(channel, rx_length, (unsigned long)ptrStart, (void *)&(uart_reg->UART_D)); // configure DMA and reception, including configuring the RXD input
    }
            #if defined SERIAL_SUPPORT_DMA_RX_FREERUN                    // {15}
    else if (rx_length == 0) {                                           // call to update DMA progress
        fnCheckFreerunningDMA_reception(channel, (QUEQUE *)ptrStart);
    }
            #endif
        #endif
}
    #endif                                                               // endif SERIAL_SUPPORT_DMA_RX

    #if defined UART_EXTENDED_MODE
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE Channel)            // dummy
{
    return 0;
}
    #endif

// Rules for numbering LPUARTS/UART channels
//
#if LPUARTS_AVAILABLE > 0
    #if defined LPUARTS_PARALLEL
        #define FIRST_LPUART_CHANNEL     (UARTS_AVAILABLE + 0)
    #else
        #define FIRST_LPUART_CHANNEL     0
    #endif
    #if LPUARTS_AVAILABLE > 1
        #if defined LPUARTS_PARALLEL
            #define SECOND_LPUART_CHANNEL    (UARTS_AVAILABLE + 1)
        #else
            #define SECOND_LPUART_CHANNEL    1
        #endif
    #endif
    #if LPUARTS_AVAILABLE > 2
        #if defined LPUARTS_PARALLEL
            #define THIRD_LPUART_CHANNEL    (UARTS_AVAILABLE + 2)
        #else
            #define THIRD_LPUART_CHANNEL    2
        #endif
    #endif
    #if LPUARTS_AVAILABLE > 3
        #if defined LPUARTS_PARALLEL
            #define FOURTH_LPUART_CHANNEL    (UARTS_AVAILABLE + 3)
        #else
            #define ROURTH_LPUART_CHANNEL    3
        #endif
    #endif
    #if LPUARTS_AVAILABLE > 4
        #if defined LPUARTS_PARALLEL
            #define FIFTH_LPUART_CHANNEL    (UARTS_AVAILABLE + 4)
        #else
            #define FIFTH_LPUART_CHANNEL    4
        #endif
    #endif
#endif
#if (UARTS_AVAILABLE > 1)
    #if (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
        #define FIRST_UART_CHANNEL          0
    #endif
    #if (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
        #define SECOND_UART_CHANNEL         1
    #endif
    #if ((LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2))
        #define THIRD_UART_CHANNEL          2
    #endif
    #if UARTS_AVAILABLE > 3
        #define FOURTH_UART_CHANNEL         3
    #endif
    #if UARTS_AVAILABLE > 4
        #define FIFTH_UART_CHANNEL          4
    #endif
    #if UARTS_AVAILABLE > 5
        #define SIXTH_UART_CHANNEL          5
    #endif
#endif

#define LPUART_TX_PIN            0
#define UART_TX_PIN              0
#define LPUART_RX_PIN            1
#define UART_RX_PIN              1
#define LPUART_RTS_PIN           2
#define UART_RTS_PIN             2

#include "kinetis_uart_pins.h"                                           // include fnConfigureUARTpin() for configuring LPUART/UART pins and entering related interrupt handlers 

    #if defined SUPPORT_HW_FLOW
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
        #if NUMBER_EXTERNAL_SERIAL > 0
    if (channel >= NUMBER_SERIAL) {
        fnSetRTS(channel, 0);                                            // prepare to drive the RTS line in negated state
        return;
    }
        #endif
        #if !defined KINETIS_KE && !defined KINETIS_KL
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(channel);
    unsigned long ulMode;
    if (uart_reg == 0) {
        return;                                                          // invalid channel
    }
            #if defined LPUART_WITH_RTS_CTS
    if (uart_type[channel] == UART_TYPE_LPUART) {                        // if this is a LPUART with RTS/CTS control
        ulMode = (((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_MODIR &= ~(LPUART_MODIR_TXRTSE | LPUART_MODIR_TXRTSPOL)); // read the original modem configuration
    }
    else {
        ulMode = (uart_reg->UART_MODEM & ~(UART_MODEM_TXRTSE | UART_MODEM_TXRTSPOL)); // read the original modem configuration
    }
            #else
    ulMode = (uart_reg->UART_MODEM & ~(UART_MODEM_TXRTSE | UART_MODEM_TXRTSPOL)); // read the original modem configuration
            #endif
        #endif

    if ((usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) != 0) {
        if ((usModifications & CONFIG_RTS_PIN) != 0) {                   // if the caller wants to modify the RTS pin configuration
            #if defined KINETIS_KE15
            fnConfigureUARTpin(channel, LPUART_RTS_PIN);                 // configure the LPUART rx pin and enter its interrupt handler
            #else
            switch (channel) {
            case 0:                                                      // configure the UART0_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // {200} families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_0_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[0] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_0_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[0] = 1;                                    // inverted RTS mode
                }
                #elif defined KINETIS_K02
                    #if defined UART0_A_LOW
                _CONFIG_PERIPHERAL(A, 3, (PA_3_UART0_RTS | UART_PULL_UPS)); // UART0_RX on PA1 (alt. function 2)
                    #elif defined UART0_ON_D
                _CONFIG_PERIPHERAL(D, 4, (PD_4_UART0_RTS | UART_PULL_UPS)); // UART0_RX on PD6 (alt. function 3)
                    #else
                _CONFIG_PERIPHERAL(B, 2, (PB_2_UART0_RTS | UART_PULL_UPS)); // UART0_RX on PB16 (alt. function 3)
                    #endif
                #elif defined UART0_A_LOW
                _CONFIG_PERIPHERAL(A, 3, (PA_3_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on PA3 (alt. function 2)
                #elif defined UART0_ON_B
                _CONFIG_PERIPHERAL(B, 2, (PB_2_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on PB2 (alt. function 3)
                #elif defined UART0_ON_D
                _CONFIG_PERIPHERAL(D, 4, (PD_4_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on PD4 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(A, 17, (PA_17_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on A17 (alt. function 3)
                #endif
                break;

            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
            case 1:                                                      // configure the UART1_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_1_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[1] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_1_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[1] = 1;                                    // inverted RTS mode
                }
                #elif defined UART1_ON_C
                _CONFIG_PERIPHERAL(C, 1, (PC_1_UART1_RTS | UART_PULL_UPS)); // UART1_RTS on PC1 (alt. function 3) {16}
                #else
                _CONFIG_PERIPHERAL(E, 3, (PE_3_UART1_RTS | UART_PULL_UPS)); // UART1_RTS on PE3 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
            case 2:                                                      // configure the UART2_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_2_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[2] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_2_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[2] = 1;                                    // inverted RTS mode
                }
                #elif (defined KINETIS_K61 || defined KINETIS_K70 || defined KINETIS_K21 || defined KINETIS_KW2X) && defined UART2_ON_E // {25}
                _CONFIG_PERIPHERAL(E, 19, (PE_19_UART2_RTS | UART_PULL_UPS)); // UART2_RTS on PE19 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART2_ON_F // {25}
                _CONFIG_PERIPHERAL(F, 11, (PF_11_UART2_RTS | UART_PULL_UPS)); // UART2_RTS on PF11 (alt. function 4)
                #else
                _CONFIG_PERIPHERAL(D, 0, (PD_0_UART2_RTS | UART_PULL_UPS)); // UART2_RTS on PD0 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case 3:                                                      // configure the UART Rx 3 pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_3_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[3] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_3_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[3] = 1;                                    // inverted RTS mode
                }
                #elif defined UART3_ON_B
                _CONFIG_PERIPHERAL(B, 8, (PB_8_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PB8 (alt. function 3)
                #elif defined UART3_ON_C
                _CONFIG_PERIPHERAL(C, 18, (PC_18_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PC18 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART3_ON_F // {25}
               _CONFIG_PERIPHERAL(F, 9, (PF_9_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PF9 (alt. function 4) {12}
                #else
                _CONFIG_PERIPHERAL(E, 7, (PE_7_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PE7 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case 4:                                                      // configure the UART4_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_4_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[4] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_4_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[4] = 1;                                    // inverted RTS mode
                }
                #elif defined UART4_ON_C
                _CONFIG_PERIPHERAL(C, 12, (PC_12_UART4_RTS | UART_PULL_UPS)); // UART4_RTS on PC12 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(E, 27, (PE_27_UART4_RTS | UART_PULL_UPS)); // UART4_RTS on PE27 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case 5:                                                      // configure the UART5_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_5_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[5] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_5_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[5] = 1;                                    // inverted RTS mode
                }
                #elif defined UART5_ON_D
                _CONFIG_PERIPHERAL(D, 10, (PD_10_UART5_RTS | UART_PULL_UPS)); // UART5_RTS on PD10 (alt. function 3)
                #else
                    #if UARTS_AVAILABLE == 5
                        #if defined LPUART0_ON_A
                _CONFIG_PERIPHERAL(A, 3, (PA_3_LPUART0_RTS | UART_PULL_UPS)); // LPUART0_RTS on PA3 (alt. function 5)
                        #elif defined LPUART0_ON_D
                _CONFIG_PERIPHERAL(D, 10, (PD_10_LPUART0_RTS | UART_PULL_UPS)); // LPUART0_RTS on PD10 (alt. function 5)
                        #else
                _CONFIG_PERIPHERAL(E, 11, (PE_11_LPUART0_RTS | UART_PULL_UPS)); // LPUART0_RTS on PE11 (alt. function 5)
                        #endif
                    #else
                _CONFIG_PERIPHERAL(E, 11, (PE_11_UART5_RTS | UART_PULL_UPS)); // UART5_RTS on PE11 (alt. function 3)
                    #endif
                #endif
                break;
            #endif
            }
            #endif
            #if defined KINETIS_KE15
            if ((usModifications & SET_RS485_MODE) != 0) {               // LPUART supports automatic control of the RTS line which is used in RS485 mode
                KINETIS_LPUART_CONTROL *lpuart_reg = fnSelectChannel(channel);
                if ((usModifications & SET_RS485_NEG) == 0) {            // the polarity of the RTS line is inverted
                    lpuart_reg->LPUART_MODIR = (LPUART_MODIR_RXRTSE | LPUART_MODIR_TXRTSPOL);  // positive RTS polarity
                }
                else {
                    lpuart_reg->LPUART_MODIR = (LPUART_MODIR_RXRTSE);  // positive RTS polarity
                }
            }
            #elif !defined KINETIS_KE && !defined KINETIS_KL
            if ((usModifications & SET_RS485_MODE) != 0) {               // kinetis supports automatic control of the RTS line which is used in RS485 mode
                ulMode |= UART_MODEM_TXRTSE;                             // enable automatic RTS control
                if ((usModifications & SET_RS485_NEG) == 0) {            // the polarity of the RTS line is inverted
                    ulMode |= UART_MODEM_TXRTSPOL;                       // positive RTS polarity
                } 
            }
                #if defined LPUART_WITH_RTS_CTS
            if (uart_type[channel] == UART_TYPE_LPUART) {                // if this is a LPUART with RTS/CTS control
                ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_MODIR = ulMode;
            }
            else {
                uart_reg->UART_MODEM = (unsigned char)ulMode;            // set the modem mode
            }
                #else
            uart_reg->UART_MODEM = (unsigned char)ulMode;                // set the modem mode
                #endif
            #endif
        }
            #if !defined KINETIS_KE && !defined KINETIS_KL
        if ((usModifications & CONFIG_CTS_PIN) != 0) {                   // configure CTS for HW flow control
            switch (channel) {
            case 0:                                                      // configure the UART0_CTS pin
            #if defined UART0_A_LOW
                _CONFIG_PERIPHERAL(A, 0, (PA_0_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PA0 (alt. function 2)
            #elif defined UART0_ON_B
                _CONFIG_PERIPHERAL(B, 3, (PB_3_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PB3 (alt. function 3)
            #elif defined UART0_ON_D
                _CONFIG_PERIPHERAL(D, 5, (PD_5_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PD5 (alt. function 3)
            #else
                _CONFIG_PERIPHERAL(A, 16, (PA_16_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PA16 (alt. function 3)
            #endif
                break;

            case 1:                                                      // configure the UART1_CTS pin
            #if defined UART1_ON_C
                _CONFIG_PERIPHERAL(C, 2, (PC_2_UART1_CTS | UART_PULL_UPS)); // UART1_CTS on PC2 (alt. function 3)
            #else
                _CONFIG_PERIPHERAL(E, 2, (PE_2_UART1_CTS | UART_PULL_UPS)); // UART1_CTS on PE2 (alt. function 3)
            #endif
                break;

            case 2:                                                      // configure the UART2_CTS pin
            #if (defined KINETIS_K61 || defined KINETIS_K70 || defined KINETIS_K21 || defined KINETIS_KW2X) && defined UART2_ON_E // {25}
                _CONFIG_PERIPHERAL(E, 18, (PE_18_UART2_CTS | UART_PULL_UPS)); // UART2_CTS on PE18 (alt. function 3)
            #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART2_ON_F // {25}
                _CONFIG_PERIPHERAL(F, 12, (PF_12_UART2_CTS | UART_PULL_UPS)); // UART2_CTS on PF12 (alt. function 4)
            #else
                _CONFIG_PERIPHERAL(D, 1, (PD_1_UART2_CTS | UART_PULL_UPS)); // UART2_CTS on PD1 (alt. function 3)
            #endif
                break;
            #if UARTS_AVAILABLE > 3
            case 3:                                                      // configure the UART3_CTS pin
                #if defined UART3_ON_B
                _CONFIG_PERIPHERAL(B, 9, (PB_9_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PB9 (alt. function 3)
                #elif defined UART3_ON_C
                _CONFIG_PERIPHERAL(C, 19, (PC_19_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PC19 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART3_ON_F // {25}
                _CONFIG_PERIPHERAL(F, 10, (PF_10_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PF10 (alt. function 4) {12}
                #else
                _CONFIG_PERIPHERAL(E, 6, (PE_6_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PE6 (alt. function 3)
                #endif
                break;
            #endif
            #if UARTS_AVAILABLE > 4
            case 4:                                                      // configure the UART4_CTS pin
                #if defined UART4_ON_C
                _CONFIG_PERIPHERAL(C, 13, (PC_13_UART4_CTS | UART_PULL_UPS)); // UART4_CTS on PC13 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(E, 26, (PE_26_UART4_CTS | UART_PULL_UPS)); // UART4_CTS on PE26 (alt. function 3)
                #endif
                break;

            case 5:                                                      // configure the UART5_CTS pin
                #if defined UART5_ON_D
                _CONFIG_PERIPHERAL(D, 11, (PD_11_UART5_CTS | UART_PULL_UPS)); // UART5_CTS on PD11 (alt. function 3)
                #else
                    #if UARTS_AVAILABLE == 5
                        #if defined LPUART0_ON_A
                _CONFIG_PERIPHERAL(A, 0, (PA_0_LPUART0_CTS | UART_PULL_UPS)); // LPUART0_CTS on PA0 (alt. function 5)
                        #elif defined LPUART0_ON_D
                _CONFIG_PERIPHERAL(D, 11, (PD_11_LPUART0_CTS | UART_PULL_UPS)); // LPUART0_CTS on PD11 (alt. function 5)
                        #else
                _CONFIG_PERIPHERAL(E, 10, (PE_10_LPUART0_CTS | UART_PULL_UPS)); // LPUART0_CTS on PE10 (alt. function 5)
                        #endif
                    #else
                _CONFIG_PERIPHERAL(E, 10, (PE_10_UART5_CTS | UART_PULL_UPS)); // UART5_CTS on PE10 (alt. function 3)
                    #endif
                #endif
                break;
            #endif
            }
        }
            #endif
    }
        #if defined KINETIS_KE || defined KINETIS_KL                     // {200}
        if ((((usModifications & (SET_RTS)) != 0) && (ucRTS_neg[0] == 0)) || (((usModifications & (CLEAR_RTS)) != 0) && (ucRTS_neg[0] != 0))) { // assert RTS output signal by setting output to '0' (or inverted negate)
            #if defined KINETIS_KE15
            _EXCEPTION("To do");
            #else
            switch (channel) {
            case 0:
                _SET_RTS_0_LOW();
                break;
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
            case 1:
                _SET_RTS_1_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
            case 2:
                _SET_RTS_2_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case 3:
                _SET_RTS_3_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case 4:
                _SET_RTS_4_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case 5:
                _SET_RTS_5_LOW();
                break;
            #endif
            }
            #endif
        }
        if ((((usModifications & (CLEAR_RTS)) != 0) && (ucRTS_neg[0] == 0)) || (((usModifications & (SET_RTS)) != 0) && (ucRTS_neg[0] != 0))) { // negate RTS output signal by setting output to '0' (or inverted assert)
            #if defined KINETIS_KE15
            _EXCEPTION("To do");
            #else
            switch (channel) {
            case 0:
                _SET_RTS_0_HIGH();
                break;
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
            case 1:
                _SET_RTS_1_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
            case 2:
                _SET_RTS_2_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case 3:
                _SET_RTS_3_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case 4:
                _SET_RTS_4_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case 5:
                _SET_RTS_5_HIGH();
                break;
            #endif
            }
            #endif
        }
        #else
    if ((ulMode & UART_MODEM_TXRTSE) == 0) {                             // control the polarity of the RTS line when not in RS485 mode
        if ((usModifications & (SET_RTS)) != 0) {                        // assert RTS output signal
            #if defined LPUART_WITH_RTS_CTS
            if (uart_type[channel] == UART_TYPE_LPUART) {                // if this is a LPUART with RTS/CTS control
                ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_MODIR |= LPUART_MODIR_RXRTSE; // RTS is asserted as long as the receive FIFO has space to receive
            }
            else {
                //uart_reg->UART_MODEM |= UART_MODEM_TXRTSPOL;
                uart_reg->UART_MODEM |= UART_MODEM_RXRTSE;               // {7} RTS is asserted as long as the receive FIFO has space to receive
            }
            #else
          //uart_reg->UART_MODEM |= UART_MODEM_TXRTSPOL;
            uart_reg->UART_MODEM |= UART_MODEM_RXRTSE;                   // {7} RTS is asserted as long as the receive FIFO has space to receive
            #endif
        }
        if ((usModifications & (CLEAR_RTS)) != 0) {                      // negate RTS output signal
            #if defined LPUART_WITH_RTS_CTS
            if (uart_type[channel] == UART_TYPE_LPUART) {                // if this is a LPUART with RTS/CTS control
                ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_MODIR &= ~LPUART_MODIR_RXRTSE; // disable control of RTS by receiver space which negates the RTS line
            }
            else {
                //uart_reg->UART_MODEM &= ~UART_MODEM_TXRTSPOL;
                uart_reg->UART_MODEM &= ~UART_MODEM_RXRTSE;              // {7} disable control of RTS by receiver space which negates the RTS line
            }
            #else
          //uart_reg->UART_MODEM &= ~UART_MODEM_TXRTSPOL;
            uart_reg->UART_MODEM &= ~UART_MODEM_RXRTSE;                  // {7} disable control of RTS by receiver space which negates the RTS line
            #endif
        }
    }
        #endif
}

#if defined SUPPORT_HW_FLOW && (defined KINETIS_KL && !defined LPUART_WITH_RTS_CTS) // {212}
static const unsigned char uCTS_priority[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {
    CTS_0_INTERRUPT_PRIORITY,
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
    CTS_1_INTERRUPT_PRIORITY,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
    CTS_2_INTERRUPT_PRIORITY,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
    CTS_3_INTERRUPT_PRIORITY,
    #endif
};

static const unsigned char ucCTS_port[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {
    CTS_0_PORT,
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
    CTS_1_PORT,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
    CTS_2_PORT,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
    CTS_3_PORT,
    #endif
};

static const unsigned long ulPin[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {
    CTS_0_PIN,
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
    CTS_1_PIN,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
    CTS_2_PIN,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
    CTS_3_PIN,
    #endif
};

static void _cts_change_0(void)
{
    GPIO_REGS *ptrGPIO = (GPIO_REGS *)GPIOA_ADD;
    ptrGPIO += ucCTS_port[0];
    fnRTS_change(0, ((ptrGPIO->PDIR & ulPin[0]) == 0));                  // synchronise control with buffer control
}
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
static void _cts_change_1(void)
{
    GPIO_REGS *ptrGPIO = (GPIO_REGS *)GPIOA_ADD;
    ptrGPIO += ucCTS_port[1];
    fnRTS_change(1, ((ptrGPIO->PDIR & ulPin[1]) == 0));                  // synchronise control with buffer control
}
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
static void _cts_change_2(void)
{
    GPIO_REGS *ptrGPIO = (GPIO_REGS *)GPIOA_ADD;
    ptrGPIO += ucCTS_port[2];
    fnRTS_change(2, ((ptrGPIO->PDIR & ulPin[2]) == 0));                  // synchronise control with buffer control
}
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
static void _cts_change_3(void)
{
    GPIO_REGS *ptrGPIO = (GPIO_REGS *)GPIOA_ADD;
    ptrGPIO += ucCTS_port[3];
    fnRTS_change(3, (ptrGPIO->PDIR & ulPin[3] == 0));                    // synchronise control with buffer control
}
    #endif

static const void (*cts_handlers[LPUARTS_AVAILABLE + UARTS_AVAILABLE])(void) = {
    _cts_change_0,
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 1
    _cts_change_1,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 2
    _cts_change_2,
    #endif
    #if (LPUARTS_AVAILABLE + UARTS_AVAILABLE) > 3
    _cts_change_3,
    #endif
};
#endif
 
// Modify control line interrupt operation (this is called from entry_tty() with disabled interrupts)
//
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
        #if !defined KINETIS_KE && !defined KINETIS_KL
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(channel);

    if ((usModifications & ENABLE_CTS_CHANGE) != 0) {
            #if defined LPUART_WITH_RTS_CTS
        if (uart_type[channel] == UART_TYPE_LPUART) {                    // if this is a LPUART with RTS/CTS control
            ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_MODIR |= LPUART_MODIR_TXCTSE; // enable CTS line to automatically stop the transmitter when negated (HW flow control)
        }
        else {
            uart_reg->UART_MODEM |= UART_MODEM_TXCTSE;                    // enable CTS line to automatically stop the transmitter when negated (HW flow control)
        }
            #else
        uart_reg->UART_MODEM |= UART_MODEM_TXCTSE;                       // enable CTS line to automatically stop the transmitter when negated (HW flow control)
            #endif
    }
    if ((usModifications & DISABLE_CTS_CHANGE) != 0) {
            #if defined LPUART_WITH_RTS_CTS
        if (uart_type[channel] == UART_TYPE_LPUART) {                    // if this is a LPUART with RTS/CTS control
            ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_MODIR &= ~LPUART_MODIR_TXCTSE; // disable CTS line flow control
        }
        else {
            uart_reg->UART_MODEM &= ~UART_MODEM_TXCTSE;                  // disable CTS line flow control
        }
            #else
        uart_reg->UART_MODEM &= ~UART_MODEM_TXCTSE;                      // disable CTS line flow control
            #endif
    }
    return SET_CTS;                                                      // the state of the CTS line cannot be read but report that it is asserted since flow control is performed by hardware
        #elif defined KINETIS_KE15
    _EXCEPTION("To do");
    return 0;
        #else
    // Devices without CTS input use a port interrupt                    {212}
    //
    GPIO_REGS *ptrGPIO = (GPIO_REGS *)GPIOA_ADD;
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    interrupt_setup.int_type =       PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_handler =    (void(*)(void))cts_handlers[channel]; // handling function
    interrupt_setup.int_priority =   uCTS_priority[channel];             // interrupt priority level
    interrupt_setup.int_port =       ucCTS_port[channel];                // the port that the interrupt input is on
    ptrGPIO += interrupt_setup.int_port;
    interrupt_setup.int_port_bits =  ulPin[channel];                     // the IRQ input connected
    interrupt_setup.int_port_sense = (IRQ_BOTH_EDGES | PULLUP_ON);       // interrupt is to fire on any edge
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
    if ((ptrGPIO->PDIR & interrupt_setup.int_port_bits) == 0) {          // return the present state of the CTS input
        return SET_CTS;
    }
    else {
        return 0;
    }
        #endif
}
    #endif                                                               // end SUPPORT_HW_FLOW




// Enable transmission on the defined channel - including configuring the transmit data output
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        fnConfigureUARTpin(Channel, LPUART_TX_PIN);                      // configure the LPUART tx pin and enter its interrupt handler
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= LPUART_CTRL_TE; // LPUART transmitter is enabled but not the transmission interrupt
        return;
        #if UARTS_AVAILABLE > 0        
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    fnConfigureUARTpin(Channel, UART_TX_PIN);                            // configure the UART tx pin and enter its interrupt handle
    uart_reg->UART_C2 |= (UART_C2_TE);                                   // UART transmitter is enabled but not the transmission interrupt
        #if defined KINETIS_KE
    _SIM_PER_CHANGE;
        #endif
    #endif
}

// Disable transmission on the defined channel
//
extern void fnTxOff(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0                                            // LPUART available in the device
        #if UARTS_AVAILABLE > 0                                          // also UART available
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART type
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_TE | LPUART_CTRL_TIE); // disable LPUART transmitter and transmission interrupt (LPUART)
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    uart_reg->UART_C2 &= ~(UART_C2_TE | UART_C2_TIE | UART_C2_TCIE);     // disable UART transmitter and transmission interrupt
    #endif
}


// Enable reception on the defined channel - including configuring the receive data input
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        fnConfigureUARTpin(Channel, LPUART_RX_PIN);                      // configure the LPUART rx pin and enter its interrupt handler
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= (LPUART_CTRL_RIE | LPUART_CTRL_RE); // enable LPUART receiver and reception interrupt (or DMA)
        #if defined KINETIS_KE
        _SIM_PER_CHANGE;
        #endif
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    fnConfigureUARTpin(Channel, UART_RX_PIN);                            // configure the UART rx pin and enter its interrupt handler
    uart_reg->UART_C2 |= (UART_C2_RE | UART_C2_RIE);                     // enable UART receiver and reception interrupt (or DMA)
        #if defined KINETIS_KE
    _SIM_PER_CHANGE;
        #endif
    #endif
    #if defined SERIAL_SUPPORT_RX_DMA_BREAK || defined UART_BREAK_SUPPORT
    ucBreakSynchronised[Channel] = 0;                                    // each time the receiver is (re)enabled a break synchronisation is expected before data is accepted
    #endif
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_RIE | LPUART_CTRL_RE); // disable LPUART receiver and reception interrupts (LPUART)
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    uart_reg->UART_C2 &= ~(UART_C2_RE | UART_C2_RIE);                    // disable UART receiver and reception interrupts (UART)
    #endif
}

#if defined SERIAL_SUPPORT_DMA_RX                                        // {214}
// Prepare the receive DMA mode that is required
//
static void fnPrepareRxDMA_mode(unsigned char ucDMAConfig, QUEUE_HANDLE Channel)
{
    if ((ucDMAConfig & (UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_FULL_BUFFER)) != 0) { // if operating in half-buffer or full buffer mode
        unsigned long ulDMA_rules = (DMA_BYTES | DMA_DIRECTION_INPUT);
        if ((ucDMAConfig & UART_RX_DMA_HALF_BUFFER) != 0) {
            ulSerialDMA_rules[Channel] = (DMA_BYTES | DMA_DIRECTION_INPUT | DMA_HALF_BUFFER_INTERRUPT); // mark that we are not in free-running DMA Rx mode and require half-buffer and full interrupts
        }
        else {
            ulSerialDMA_rules[Channel] = (DMA_BYTES | DMA_DIRECTION_INPUT); // mark that we are not in free-running DMA Rx mode and require full buffer interrupts
        }
        ulSerialDMA_rules[Channel] = ulDMA_rules;                    
    }
    else {
        ulSerialDMA_rules[Channel] = 0;                              // mark that we are in free-running DMA Rx mode
    }
}
#endif

#if LPUARTS_AVAILABLE > 0
// LPUART configuration
//
static void fnConfigLPUART(QUEUE_HANDLE Channel, TTYTABLE *pars, KINETIS_LPUART_CONTROL *lpuart_reg, unsigned short usDivider)
{
    lpuart_reg->LPUART_BAUD = ((lpuart_reg->LPUART_BAUD & ~LPUART_BAUD_SBR) | (usDivider | LPUART_BAUD_OSR_16)); // set the (new) baud rate
    uDisable_Interrupt();                                                // protect this region in case transmit interrupt is presently enabled
    {                                                                    // {204}
        unsigned long ulCtrlReg = (lpuart_reg->LPUART_CTRL & (LPUART_CTRL_RIE | LPUART_CTRL_TCIE | LPUART_CTRL_TIE | LPUART_CTRL_RE | LPUART_CTRL_TE)); // preserve only the interrupt values and rx/tx enable (in case a byte is presently being transmitted)
        if ((pars->Config & CHAR_7) != 0) {                              // Kinetis supports only one fixed stop bit on its main UART(s)
            ucUART_mask[Channel] = 0x7f;                                 // set 7 bit mask to remove parity bits
        }
        else {
            ucUART_mask[Channel] = 0xff;                                 // full 8 bit mask
        }
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) { // if parity is enabled
            if ((pars->Config & RS232_ODD_PARITY) != 0) {
                ulCtrlReg |= (LPUART_CTRL_PE | LPUART_CTRL_PT_ODD);      // odd parity
            }
            else {                                                       
                ulCtrlReg |= (LPUART_CTRL_PE | LPUART_CTRL_PT_EVEN);     // even parity
            }
        }
        lpuart_reg->LPUART_CTRL = (ulCtrlReg | LPUART_CHARACTERISTICS);  // set the new control register value (this also disables rx and tx, if enabled, which is in conformance with the requirement to only change settings with these turned off)
    }
    uEnable_Interrupt();                                                 // enable interrupt again (transmit interrupt enable has been preserved)
    if ((pars->Config & TWO_STOPS) != 0) {                               // LPUART supports 2 stop bits
        lpuart_reg->LPUART_BAUD |= LPUART_BAUD_SBNS_2;                   // set 2 stop bits
    }
    #if defined SERIAL_SUPPORT_DMA
    if ((pars->ucDMAConfig & UART_TX_DMA) != 0) {                        // if transmission is to be DMA driven
        if ((lpuart_reg->LPUART_BAUD & LPUART_BAUD_TDMAE) == 0) {        // if the DMA has already been configured don't disturb it
        #if defined LPUARTS_PARALLEL                                     // serial interfaces referenced by UARTs first, followed by LPUARTs - see http://www.utasker.com/kinetis/UART_LPUART.html
            fnConfigDMA_buffer(UART_DMA_TX_CHANNEL[Channel], (DMAMUX0_CHCFG_SOURCE_LPUART0_TX + (2 * (Channel - UARTS_AVAILABLE))), 0, 0, (void *)&(lpuart_reg->LPUART_DATA), (DMA_BYTES | DMA_DIRECTION_OUTPUT | DMA_SINGLE_CYCLE), _uart_tx_dma_Interrupts[Channel], UART_DMA_TX_INT_PRIORITY[Channel]);
        #else
            fnConfigDMA_buffer(UART_DMA_TX_CHANNEL[Channel], (DMAMUX0_CHCFG_SOURCE_LPUART0_TX + (2 * Channel)), 0, 0, (void *)&(lpuart_reg->LPUART_DATA), (DMA_BYTES | DMA_DIRECTION_OUTPUT | DMA_SINGLE_CYCLE), _uart_tx_dma_Interrupts[Channel], UART_DMA_TX_INT_PRIORITY[Channel]);
        #endif
            lpuart_reg->LPUART_CTRL &= ~(LPUART_CTRL_TIE | LPUART_CTRL_TCIE); // ensure tx interrupts are not enabled
            lpuart_reg->LPUART_BAUD |= LPUART_BAUD_TDMAE;                // use DMA rather than interrupts for transmission
        }
    }
    else {                                                               // interrupt driven transmitter
        lpuart_reg->LPUART_BAUD &= ~LPUART_BAUD_TDMAE;                   // disable tx DMA so that tx interrupt mode can be used
    }
    #endif
    lpuart_reg->LPUART_BAUD &= ~(LPUART_BAUD_LBKDIE);                    // disable break detection interrupt by default
    #if defined SERIAL_SUPPORT_DMA_RX                                    // {209}
    if ((pars->ucDMAConfig & UART_RX_DMA) != 0) {                        // dma required on receiver
        lpuart_reg->LPUART_CTRL &= ~(LPUART_CTRL_RIE);                   // ensure rx interrupt is not enabled
        lpuart_reg->LPUART_BAUD |= LPUART_BAUD_RDMAE;                    // use DMA rather than interrupts for reception
        #if defined SERIAL_SUPPORT_RX_DMA_BREAK
        if ((pars->ucDMAConfig & UART_RX_DMA_BREAK) != 0) {              // if breaks are to terminate reception
            lpuart_reg->LPUART_BAUD |= LPUART_BAUD_LBKDIE;               // enable break detection interrupt
        }
        #endif
        fnPrepareRxDMA_mode(pars->ucDMAConfig, Channel);                 // prepare the rx DMA mode that is required
    }
    else {
        lpuart_reg->LPUART_BAUD &= ~LPUART_BAUD_RDMAE;                   // disable rx DMA so that rx interrupt mode can be used
    }
    #endif
    #if defined UART_BREAK_SUPPORT
    if ((pars->Config & MSG_BREAK_MODE) != 0) {                          // if reception breaks are to terminate reception
        lpuart_reg->LPUART_BAUD |= LPUART_BAUD_LBKDIE;                   // enable break detection interrupt
    }
    #endif
    #if defined UART_EXTENDED_MODE
    if ((pars->Config & UART_INVERT_TX) != 0) {
        lpuart_reg->LPUART_CTRL |= LPUART_CTRL_TXINV;                    // invert the polarity of the transmit signal
    }
    else {
        lpuart_reg->LPUART_CTRL &= ~LPUART_CTRL_TXINV;                   // ensure no transmit signal inversion
    }
        #if defined SIM_SOPT5_LPUART0ODE
    if ((pars->Config & UART_OPEN_DRAIN_OUTPUT) != 0) {
        SIM_SOPT5 |= (SIM_SOPT5_LPUART0ODE << Channel);                  // enable open drain on the LPUART
    }
    else {
        SIM_SOPT5 &= ~(SIM_SOPT5_LPUART0ODE << Channel);                 // ensure open drain mode is disabled
    }
        #endif
    if ((pars->Config & UART_IDLE_LINE_INTERRUPT) != 0) {                // if the idle line interrupt is to be used
        lpuart_reg->LPUART_CTRL |= (LPUART_CTRL_IDLECFG_4 | LPUART_CTRL_ILIE | LPUART_CTRL_ILT); // idle line starts after the stop bit and 4 idle characters
    }
    else {
        lpuart_reg->LPUART_CTRL &= ~(LPUART_CTRL_IDLECFG_4 | LPUART_CTRL_ILIE | LPUART_CTRL_ILT); // ensure that the idle line interrupt is disabled
    }
    #endif
    #if defined KINETIS_KL && defined UART_FRAME_END_COMPLETE
    if ((pars->Config & INFORM_ON_FRAME_TRANSMISSION) != 0) {
        ucReportEndOfFrame[Channel] = 1;                                 // we want to work with a frame completion interrupt
    }
    #endif
}
#endif

#if UARTS_AVAILABLE > 0
// UART configuration
//
static void fnConfigUART(QUEUE_HANDLE Channel, TTYTABLE *pars, KINETIS_UART_CONTROL *uart_reg, unsigned short usDivider)
{
    uart_reg->UART_BDH = (unsigned char)((usDivider >> 8) & 0x1f);       // program speed to divisor latch registers
    uart_reg->UART_BDL = (unsigned char)usDivider;                       // the complete baud rate generator setting is committed when this byte is written

    if ((pars->Config & CHAR_7) != 0) {                                  // Kinetis supports only one fixed stop bit on its main UART(s)
        ucUART_mask[Channel] = 0x7f;                                     // set 7 bit mask to remove parity bits
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) {
            if ((pars->Config & RS232_ODD_PARITY) != 0) {
                uart_reg->UART_C1 = (UART_C1_PE | UART_C1_PT_ODD);       // program odd parity
            }
            else {                                                       // program even parity
                uart_reg->UART_C1 = (UART_C1_PE | UART_C1_PT_EVEN);
            }
        }
        else {
            uart_reg->UART_C1 = UART_C1_PARITY_DISABLED;                 // no parity - the UART in the Kinetis will use address mark at the 8th bit position in 7 bit character mode without a parity setting
        }
    }
    else {
        ucUART_mask[Channel] = 0xff;                                     // full 8 bit mask
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) {
            if ((pars->Config & RS232_ODD_PARITY) != 0) {
                uart_reg->UART_C1 = (UART_C1_M | UART_C1_PE | UART_C1_PT_ODD); // program odd parity
            }
            else {
                uart_reg->UART_C1 = (UART_C1_M | UART_C1_PE | UART_C1_PT_EVEN); // program even parity
            }
        }
        else {
            uart_reg->UART_C1 = UART_C1_PARITY_DISABLED;                 // no parity - the UART in the Kinetis will use address mark at the 8th bit position in 7 bit character mode without a parity setting
        }
    }
    #if defined UART_EXTENDED_MODE
    if ((pars->Config & UART_INVERT_TX) != 0) {
        uart_reg->UART_C3 = UART_C3_TXINV;                               // invert the polarity of the transmit signal
    }
    else {
        uart_reg->UART_C3 = 0;                                           // ensure no transmit signal inversion
    }
        #if defined SIM_SOPT5_UART2ODE
    if ((pars->Config & UART_OPEN_DRAIN_OUTPUT) != 0) {
        SIM_SOPT5 |= (SIM_SOPT5_UART2ODE);                               // enable open drain on the LPUART
    }
    else {
        SIM_SOPT5 &= ~(SIM_SOPT5_UART2ODE);                              // ensure open drain mode is disabled
    }
        #endif
    if ((pars->Config & UART_IDLE_LINE_INTERRUPT) != 0) {                // if the idle line interrut is to be used
        uart_reg->UART_C1 |= UART_C1_ILT;                                // idle line starts after the stop bit
        uart_reg->UART_C2 |= UART_C2_ILIE;                               // enable idle line interrupt
    }
    else {
        uart_reg->UART_C2 &= ~UART_C2_ILIE;                              // ensure that the idle line interrupt is disabled
    }
    #endif
    #if (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
    if ((pars->Config & INFORM_ON_FRAME_TRANSMISSION) != 0) {            // {200}
        ucReportEndOfFrame[Channel] = 1;                                 // we want to work with a frame completion interrupt
    }
    #endif
    #if defined TRUE_UART_TX_2_STOPS
    ucStops[Channel] = 0;                                                // default is 1 stop bit
    #endif
    #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
    if ((Channel > 0) && ((pars->Config & TWO_STOPS) != 0)) {            // simplified UARTs (above UART0) support 2 stop bits
        uart_reg->UART_BDH |= (UART_BDH_SBNS);                           // set 2 stop bits
        #if defined SERIAL_SUPPORT_DMA
        goto _configDMA;
        #else
        return;
        #endif
    }
    #elif defined UART_BDH_SBNS                                          // supports 2 stop bits
    if ((pars->Config & TWO_STOPS) != 0) {                               // 2 stop bits requested
        uart_reg->UART_BDH |= (UART_BDH_SBNS);                           // set 2 stop bits
        #if defined SERIAL_SUPPORT_DMA
        goto _configDMA;
        #else
        return;
        #endif
    }
    #endif
    #if defined TRUE_UART_TX_2_STOPS                                     // control transmitter 2 stop bits using interrupt as long as not operating in DMA mode
        #if defined SERIAL_SUPPORT_DMA
    if (((pars->ucDMAConfig & UART_TX_DMA) == 0) && ((pars->Config & TWO_STOPS) != 0)) {
        ucStops[Channel] = 1;                                            // mark that the end of transmission interrupt is to be used instead of the transmit empty interrupt - this causes an extra stop bit to be inserted between characters and so 2-stop bits to be achieved
    }
        #else
    if ((pars->Config & TWO_STOPS) != 0) {
        ucStops[Channel] = 1;                                            // mark that the end of transmission interrupt is to be used instead of the transmit empty interrupt - this causes an extra stop bit to be inserted between characters and so 2-stop bits to be achieved
    }
        #endif
    #endif
    #if defined SERIAL_SUPPORT_DMA                                       // {6}
        #if defined UART_BDH_SBNS
    _configDMA:
        #endif
    if ((pars->ucDMAConfig & UART_TX_DMA) != 0) {
        #if defined KINETIS_KL                                           // {81}
        int iConfigured = 0;
            #if UARTS_AVAILABLE > 1
        if (Channel == 0) {
            if ((uart_reg->UART_C5 & UART_C5_TDMAS) != 0) {              // {203} if DMA has already been configured we leave the configuration untouched
                iConfigured = 1;
            }
        }
        else {
            if ((uart_reg->UART_MA1_C4 & UART_C4_TDMAS) != 0) {          // {203} if DMA has already been configured we leave the configuration untouched
                iConfigured = 1;
            }
        }
            #else
        if ((uart_reg->UART_C5 & UART_C5_TDMAS) != 0) {                  // {203} if DMA has already been configured we leave the configuration untouched
            iConfigured = 1;
        }
            #endif
        if (0 == iConfigured) {
            uart_reg->UART_C2 &= ~(UART_C2_TIE | UART_C2_TCIE);          // ensure tx interrupt is not enabled
            #if defined DMAMUX0_CHCFG_SOURCE_UART0_TX
            fnConfigDMA_buffer(UART_DMA_TX_CHANNEL[Channel], (DMAMUX0_CHCFG_SOURCE_UART0_TX + (2 * Channel)), 0, 0, (void *)&(uart_reg->UART_D), (DMA_BYTES | DMA_DIRECTION_OUTPUT | DMA_SINGLE_CYCLE), _uart_tx_dma_Interrupts[Channel], UART_DMA_TX_INT_PRIORITY[Channel]);
            #else
            fnConfigDMA_buffer(UART_DMA_TX_CHANNEL[Channel], (DMAMUX0_CHCFG_SOURCE_LPUART0_TX + (2 * Channel)), 0, 0, (void *)&(uart_reg->UART_D), (DMA_BYTES | DMA_DIRECTION_OUTPUT | DMA_SINGLE_CYCLE), _uart_tx_dma_Interrupts[Channel], UART_DMA_TX_INT_PRIORITY[Channel]);
            #endif
            #if UARTS_AVAILABLE > 1
            if (Channel == 0) {
                uart_reg->UART_C5 |= UART_C5_TDMAS;                      // use DMA rather than interrupts for transmission
            }
            else {
                uart_reg->UART_MA1_C4 |= UART_C4_TDMAS;                  // use DMA rather than interrupts for transmission
                uart_reg->UART_C2 |= (UART_C2_TIE);                      // enable the tx dma request (DMA not yet enabled) rather than interrupt mode
            }
            #else
                #if defined K_STYLE_UART2                                // the UART in some KL parts behave like one in the K devices with respect to needing the interrupt enabled for DMA to be triggered
            uart_reg->UART_C2 |= (UART_C2_TIE);                          // enable the tx dma request (DMA not yet enabled) rather than interrupt mode
                #endif
            uart_reg->UART_C5 |= UART_C5_TDMAS;                          // use DMA rather than interrupts for transmission
            #endif
        }
        #else
        if ((uart_reg->UART_C2 & UART_C2_TIE) == 0) {                    // {203} if DMA has already been configured we leave it untouched
            #if defined KINETIS_KM                                       // UART0/1 an UART2/3 share channel assignment, resticted to certain DMAMUX channels
            unsigned short usDMAMUX;
            if (Channel < 2) {
                usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART0_TX);              // valid for UART0 and UART1
            }
            else {
                usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART2_TX);              // valid for UART2 and UART3
            }
                #if defined _WINDOWS
            switch (Channel) {
            case 0:
                if ((UART_DMA_TX_CHANNEL[Channel] != 0) && (UART_DMA_TX_CHANNEL[Channel] != 3)) {
                    _EXCEPTION("UART0 Tx is only possible of DMA channels 0 and 3!");
                }
                break;
            case 1:
                if ((UART_DMA_TX_CHANNEL[Channel] != 1) && (UART_DMA_TX_CHANNEL[Channel] != 2)) {
                    _EXCEPTION("UART1 Tx is only possible of DMA channels 1 and 2!");
                }
                break;
            case 2:
                if ((UART_DMA_TX_CHANNEL[Channel] != 2) && (UART_DMA_TX_CHANNEL[Channel] != 3)) {
                    _EXCEPTION("UART2 Tx is only possible of DMA channels 2 and 3!");
                }
                break;
            case 3:
                if ((UART_DMA_TX_CHANNEL[Channel] != 0) && (UART_DMA_TX_CHANNEL[Channel] != 1)) {
                    _EXCEPTION("UART3 Tx is only possible of DMA channels 0 and 1!");
                }
                break;
            }
                #endif
            #else
            unsigned short usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART0_TX + (2 * Channel));
            #endif
            #if ((defined KINETIS_K21 || defined KINETIS_K22) && (UARTS_AVAILABLE > 4)) || defined KINETIS_K64 || defined KINETIS_K65 || defined KINETIS_K66
            if (Channel > 3) {                                           // channels 4 and above each share DMA source for TX and RX
                usDMAMUX = (DMAMUX0_CHCFG_SOURCE_UART3_TX + (Channel - 3));
            }
            #endif
            uart_reg->UART_C5 |= UART_C5_TDMAS;                          // use DMA rather than interrupts for transmission
            fnConfigDMA_buffer(UART_DMA_TX_CHANNEL[Channel], usDMAMUX, 0, 0, (void *)&(uart_reg->UART_D), (DMA_BYTES | DMA_DIRECTION_OUTPUT | DMA_SINGLE_CYCLE), _uart_tx_dma_Interrupts[Channel], UART_DMA_TX_INT_PRIORITY[Channel]);
            uart_reg->UART_C2 |= (UART_C2_TIE);                          // enable the tx dma request (DMA not yet enabled) rather than interrupt mode
        }
        #endif
    }
    else {                                                               // interrupt driven transmitter
        #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
        if (Channel == 0) {
            uart_reg->UART_C5 &= ~(UART_C5_TDMAS);                       // disable tx DMA so that tx interrupt mode can be used
        }
        else {
            uart_reg->UART_MA1_C4 &= ~(UART_C4_TDMAS);                   // disable tx DMA so that tx interrupt mode can be used
        }
        #else
        uart_reg->UART_C5 &= ~(UART_C5_TDMAS);                           // disable tx DMA so that tx interrupt mode can be used
        #endif
    }
        #if defined SERIAL_SUPPORT_DMA_RX
    if ((pars->ucDMAConfig & UART_RX_DMA) != 0) {                        // {8} reception dma is required 
            #if defined KINETIS_KL                                       // {209}
                #if (UARTS_AVAILABLE > 1)
        if (Channel == 0) {
            uart_reg->UART_C5 |= UART_C5_RDMAS;                          // use DMA rather than interrupts for reception
        }
        else {
            uart_reg->UART_MA1_C4 |= UART_C4_RDMAS;                      // use DMA rather than interrupts for reception
        }
                #else
        uart_reg->UART_C5 |= UART_C5_RDMAS;                              // use DMA rather than interrupts for reception
                #endif
            #else
        uart_reg->UART_C5 |= UART_C5_RDMAS;                              // use DMA rather than interrupts for reception
            #endif
        #if defined SERIAL_SUPPORT_RX_DMA_BREAK
        if ((pars->ucDMAConfig & UART_RX_DMA_BREAK) != 0) {              // if breaks are to terminate reception
            uart_reg->UART_BDH |= (UART_BDH_LBKDIE);                     // enable break detection interrupt
        }
        #endif
        fnPrepareRxDMA_mode(pars->ucDMAConfig, Channel);                 // prepare the rx DMA mode that is required
    }
    else {                                                               // interrupt driven receiver
            #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
        if (Channel == 0) {
            uart_reg->UART_C5 &= ~(UART_C5_RDMAS);                       // disable rx DMA so that rx interrupt mode can be used
        }
        else {
            uart_reg->UART_MA1_C4 &= ~(UART_C4_RDMAS);                   // disable rx DMA so that rx interrupt mode can be used
        }
            #else
        uart_reg->UART_C5 &= ~(UART_C5_RDMAS);                           // disable rx DMA so that rx interrupt mode can be used
                #if defined UART_BREAK_SUPPORT
        if ((pars->Config & MSG_BREAK_MODE) != 0) {                      // if reception breaks are to terminate reception
          //uart_reg->UART_S2 |= (UART_S2_LBKDE);                        // enable break detection operation
            uart_reg->UART_BDH |= (UART_BDH_LBKDIE);                     // enable break detection interrupt
        }
                #endif
            #endif
    }
        #endif
    #endif
}
#endif

#if defined UART_BREAK_SUPPORT
extern void fnStartBreak(QUEUE_HANDLE channel)                           // {205}
{
    KINETIS_UART_CONTROL *uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(channel);
    #if defined USER_CODE_START_BREAK
    USER_CODE_START_BREAK();
    #endif
    #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE == 0
    ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= LPUART_CTRL_SBK; // start sending break characters
    #elif LPUARTS_AVAILABLE > 0
    if (uart_type[channel] == UART_TYPE_LPUART) {
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= LPUART_CTRL_SBK; // start sending break characters
    }
    else {
        uart_reg->UART_C2 |= UART_C2_SBK;                                // start sending break characters
    }
    #else
    uart_reg->UART_C2 |= UART_C2_SBK;                                    // start sending break characters
    #endif
    #if defined _WINDOWS
    fnControlBreak(channel, 1);
    #endif
}

extern void fnStopBreak(QUEUE_HANDLE channel)                            // {205}
{
    KINETIS_UART_CONTROL *uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(channel);
    #if defined USER_CODE_START_BREAK
    USER_CODE_START_BREAK();
    #endif
    #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE == 0
    ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~LPUART_CTRL_SBK; // stop sending break characters
    #elif LPUARTS_AVAILABLE > 0
    if (uart_type[channel] == UART_TYPE_LPUART) {
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~LPUART_CTRL_SBK; // stop sending break characters
    }
    else {
        uart_reg->UART_C2 &= ~UART_C2_SBK;                               // stop sending break characters
    }
    #else
    uart_reg->UART_C2 &= ~UART_C2_SBK;                                   // stop sending break characters
    #endif
    #if defined _WINDOWS
    fnControlBreak(channel, 0);
    #endif
}

extern QUEUE_TRANSFER fnGetDMACount(QUEUE_HANDLE channel, QUEUE_TRANSFER max_count)
{
    return 0;
}
#endif

#define SUPPORTED_BAUD_RATES  (SERIAL_BAUD_250K)

typedef struct stUART_BAUD_CONFIG
{
    int iBaudRateRef;
    unsigned short usDivider;
#if (!defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_K80) || defined K_STYLE_UART2
    unsigned char ucFraction;
#endif
    int iValid;
} UART_BAUD_CONFIG;

#if (UARTS_AVAILABLE > 2) || (defined KINETIS_KEA8 || defined KINETIS_K02 || defined K_STYLE_UART2) || (defined KINETIS_KV && UARTS_AVAILABLE > 1)
static const unsigned short bus_clock_divider[SUPPORTED_BAUD_RATES + 1] = {
    #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
    (BUS_CLOCK / 16 / 300),                                              // set 300 divide
    (BUS_CLOCK / 16 / 600),                                              // set 600 divide
    (BUS_CLOCK / 16 / 1200),                                             // set 1200 divide
    (BUS_CLOCK / 16 / 2400),                                             // set 2400 divide
    (BUS_CLOCK / 16 / 4800),                                             // set 4800 divide
    (BUS_CLOCK / 16 / 9600),                                             // set 9600 divide
    (BUS_CLOCK / 16 / 14400),                                            // set 14400 divide
    (BUS_CLOCK / 16 / 19200),                                            // set 19200 divide
        #if defined SUPPORT_MIDI_BAUD_RATE
    (BUS_CLOCK / 16 / 31250),                                            // set 31250 divide
        #endif
    (BUS_CLOCK / 16 / 38400),                                            // set 38400 divide
    (BUS_CLOCK / 16 / 57600),                                            // set 57600 divide
    (BUS_CLOCK / 16 / 115200),                                           // set 115200 divide
    (BUS_CLOCK / 16 / 230400),                                           // set 230400 divide
    (BUS_CLOCK / 16 / 250000),                                           // set 250000 divide
    #else
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 300)) + (float)0.5) * (float)2) / 2), // set 300 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 600)) + (float)0.5) * (float)2) / 2), // set 600 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 1200)) + (float)0.5) * (float)2) / 2), // set 1200 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 2400)) + (float)0.5) * (float)2) / 2), // set 2400 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 4800)) + (float)0.5) * (float)2) / 2), // set 4800 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 9600)) + (float)0.5) * (float)2) / 2), // set 9600 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 11400)) + (float)0.5) * (float)2) / 2), // set 14400 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 19200)) + (float)0.5) * (float)2) / 2), // set 19200 divide (rounded)
        #if defined SUPPORT_MIDI_BAUD_RATE
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 31250)) + (float)0.5) * (float)2) / 2), // set 31250 divide (rounded)
        #endif
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 38400)) + (float)0.5) * (float)2) / 2), // set 38400 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 57600)) + (float)0.5) * (float)2) / 2), // set 57600 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 115200)) + (float)0.5) * (float)2) / 2), // set 115200 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 230400)) + (float)0.5) * (float)2) / 2), // set 230400 divide (rounded)
    (unsigned short)((((float)((float)BUS_CLOCK / (float)(16 * 250000)) + (float)0.5) * (float)2) / 2), // set 250000 divide (rounded)
    #endif
};
    #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
static const unsigned char bus_clock_fraction[SUPPORTED_BAUD_RATES + 1] = {
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)300) - (int)(BUS_CLOCK / 16 / 300)) * 32)) + (float)0.5), // set 300 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)600) - (int)(BUS_CLOCK / 16 / 600)) * 32)) + (float)0.5), // set 600 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)1200) - (int)(BUS_CLOCK / 16 / 1200)) * 32)) + (float)0.5), // set 1200 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)2400) - (int)(BUS_CLOCK / 16 / 2400)) * 32)) + (float)0.5), // set 2400 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)4800) - (int)(BUS_CLOCK / 16 / 4800)) * 32)) + (float)0.5), // set 4800 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)9600) - (int)(BUS_CLOCK / 16 / 9600)) * 32)) + (float)0.5), // set 9600 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)14400) - (int)(BUS_CLOCK / 16 / 14400)) * 32)) + (float)0.5), // set 14400 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)19200) - (int)(BUS_CLOCK / 16 / 19200)) * 32)) + (float)0.5), // set 19200 fraction
        #if defined SUPPORT_MIDI_BAUD_RATE
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)31250) - (int)(BUS_CLOCK / 16 / 31250)) * 32)) + (float)0.5), // set 31250 fraction
        #endif
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)38400) - (int)(BUS_CLOCK / 16 / 38400)) * 32)) + (float)0.5), // set 38400 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)57600) - (int)(BUS_CLOCK / 16 / 57600)) * 32)) + (float)0.5), // set 57600 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)115200) - (int)(BUS_CLOCK / 16 / 115200)) * 32)) + (float)0.5), // set 115200 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)230400) - (int)(BUS_CLOCK / 16 / 230400)) * 32)) + (float)0.5), // set 230400 fraction
    (unsigned char)(((float)((((float)BUS_CLOCK / (float)16 / (float)250000) - (int)(BUS_CLOCK / 16 / 250000)) * 32)) + (float)0.5), // set 250000 fraction
};
    #endif

static const unsigned short system_clock_divider[SUPPORTED_BAUD_RATES + 1] = {
    #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
    (SYSTEM_CLOCK / 16 / 300),                                           // set 300 divide
    (SYSTEM_CLOCK / 16 / 600),                                           // set 600 divide
    (SYSTEM_CLOCK / 16 / 1200),                                          // set 1200 divide
    (SYSTEM_CLOCK / 16 / 2400),                                          // set 2400 divide
    (SYSTEM_CLOCK / 16 / 4800),                                          // set 4800 divide
    (SYSTEM_CLOCK / 16 / 9600),                                          // set 9600 divide
    (SYSTEM_CLOCK / 16 / 14400),                                         // set 14400 divide
    (SYSTEM_CLOCK / 16 / 19200),                                         // set 19200 divide
        #if defined SUPPORT_MIDI_BAUD_RATE
    (SYSTEM_CLOCK / 16 / 31250),                                         // set 31250 divide
        #endif
    (SYSTEM_CLOCK / 16 / 38400),                                         // set 38400 divide
    (SYSTEM_CLOCK / 16 / 57600),                                         // set 57600 divide
    (SYSTEM_CLOCK / 16 / 115200),                                        // set 115200 divide
    (SYSTEM_CLOCK / 16 / 230400),                                        // set 230400 divide
    (SYSTEM_CLOCK / 16 / 250000),                                        // set 250000 divide
    #else
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 300)) + (float)0.5) * (float)2) / 2), // set 300 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 600)) + (float)0.5) * (float)2) / 2), // set 600 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 1200)) + (float)0.5) * (float)2) / 2), // set 1200 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 2400)) + (float)0.5) * (float)2) / 2), // set 2400 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 4800)) + (float)0.5) * (float)2) / 2), // set 4800 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 9600)) + (float)0.5) * (float)2) / 2), // set 9600 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 11400)) + (float)0.5) * (float)2) / 2), // set 14400 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 19200)) + (float)0.5) * (float)2) / 2), // set 19200 divide (rounded)
        #if defined SUPPORT_MIDI_BAUD_RATE
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 31250)) + (float)0.5) * (float)2) / 2), // set 31250 divide (rounded)
        #endif
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 38400)) + (float)0.5) * (float)2) / 2), // set 38400 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 57600)) + (float)0.5) * (float)2) / 2), // set 57600 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 115200)) + (float)0.5) * (float)2) / 2), // set 115200 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 230400)) + (float)0.5) * (float)2) / 2), // set 230400 divide (rounded)
    (unsigned short)((((float)((float)SYSTEM_CLOCK / (float)(16 * 250000)) + (float)0.5) * (float)2) / 2), // set 250000 divide (rounded)
    #endif
};
    #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
static const unsigned char system_clock_fraction[SUPPORTED_BAUD_RATES + 1] = {
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)300) - (int)(SYSTEM_CLOCK / 16 / 300)) * 32)) + (float)0.5), // set 300 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)600) - (int)(SYSTEM_CLOCK / 16 / 600)) * 32)) + (float)0.5), // set 600 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)1200) - (int)(SYSTEM_CLOCK / 16 / 1200)) * 32)) + (float)0.5), // set 1200 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)2400) - (int)(SYSTEM_CLOCK / 16 / 2400)) * 32)) + (float)0.5), // set 2400 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)4800) - (int)(SYSTEM_CLOCK / 16 / 4800)) * 32)) + (float)0.5), // set 4800 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)9600) - (int)(SYSTEM_CLOCK / 16 / 9600)) * 32)) + (float)0.5), // set 9600 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)14400) - (int)(SYSTEM_CLOCK / 16 / 14400)) * 32)) + (float)0.5), // set 14400 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)19200) - (int)(SYSTEM_CLOCK / 16 / 19200)) * 32)) + (float)0.5), // set 19200 fraction
        #if defined SUPPORT_MIDI_BAUD_RATE
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)31250) - (int)(SYSTEM_CLOCK / 16 / 31250)) * 32)) + (float)0.5), // set 31250 fraction
        #endif
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)38400) - (int)(SYSTEM_CLOCK / 16 / 38400)) * 32)) + (float)0.5), // set 38400 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)57600) - (int)(SYSTEM_CLOCK / 16 / 57600)) * 32)) + (float)0.5), // set 57600 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)115200) - (int)(SYSTEM_CLOCK / 16 / 115200)) * 32)) + (float)0.5), // set 115200 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)230400) - (int)(SYSTEM_CLOCK / 16 / 230400)) * 32)) + (float)0.5), // set 230400 fraction
    (unsigned char)(((float)((((float)SYSTEM_CLOCK / (float)16 / (float)250000) - (int)(SYSTEM_CLOCK / 16 / 250000)) * 32)) + (float)0.5), // set 250000 fraction
};
    #endif
#endif

#if LPUARTS_AVAILABLE > 0
// Determine the LPUART clock frequency being
//
#if defined KINETIS_WITH_PCC                                             // same LPUART clock source for all used LPUARTs assumed
    #define SPECIAL_LPUART_CLOCK  (FIRC_CLK)                             // fast clock assumed - 48MHz, 52MHz, 56MHz or 60MHz, depending on system clock configuration
#else
    #if defined LPUART_IRC48M                                            // use the IRC48M clock as UART clock
        #if defined KINETIS_K65 || defined KINETIS_K66
            #if defined PERIPHERAL_CLOCK_DIVIDE
                #if PERIPHERAL_CLOCK_DIVIDE_FRACTION == 5
                    #define SPECIAL_LPUART_CLOCK ((2 * 48000000)/((2 * PERIPHERAL_CLOCK_DIVIDE) + 1)))
                #else
                    #define SPECIAL_LPUART_CLOCK (48000000/PERIPHERAL_CLOCK_DIVIDE)
                #endif
            #else
                #define SPECIAL_LPUART_CLOCK  (48000000)
            #endif
        #else
            #define SPECIAL_LPUART_CLOCK  (48000000)
        #endif
    #elif defined LPUART_MCGPLLCLK
        #define SPECIAL_LPUART_CLOCK   (MCGOUTCLK)
    #elif defined LPUART_OSCERCLK                                        // clock the UART from the external clock
        #define SPECIAL_LPUART_CLOCK  (_EXTERNAL_CLOCK)
    #else
        #define SPECIAL_LPUART_CLOCK  (MCGIRCLK)
    #endif
#endif


#if defined LPUART_IRC48M
static const unsigned short IRC48M_clock_divider[SUPPORTED_BAUD_RATES + 1] = {
#if !defined KINETIS_KL && !defined KINETIS_K80 && !defined KINETIS_KE15 && !defined KINETIS_KE18
    (48000000 / 16 / 300),                                               // set 300 divide
    (48000000 / 16 / 600),                                               // set 600 divide
    (48000000 / 16 / 1200),                                              // set 1200 divide
    (48000000 / 16 / 2400),                                              // set 2400 divide
    (48000000 / 16 / 4800),                                              // set 4800 divide
    (48000000 / 16 / 9600),                                              // set 9600 divide
    (48000000 / 16 / 14400),                                             // set 14400 divide
    (48000000 / 16 / 19200),                                             // set 19200 divide
    #if defined SUPPORT_MIDI_BAUD_RATE
    (48000000 / 16 / 31250),                                             // set 31250 divide
    #endif
    (48000000 / 16 / 38400),                                             // set 38400 divide
    (48000000 / 16 / 57600),                                             // set 57600 divide
    (48000000 / 16 / 115200),                                            // set 115200 divide
    (48000000 / 16 / 230400),                                            // set 230400 divide
    (48000000 / 16 / 250000),                                            // set 250000 divide
#else
    (unsigned short)((((float)((float)48000000 / (float)(16 * 300)) + (float)0.5) * (float)2) / 2), // set 300 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 600)) + (float)0.5) * (float)2) / 2), // set 600 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 1200)) + (float)0.5) * (float)2) / 2), // set 1200 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 2400)) + (float)0.5) * (float)2) / 2), // set 2400 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 4800)) + (float)0.5) * (float)2) / 2), // set 4800 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 9600)) + (float)0.5) * (float)2) / 2), // set 9600 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 11400)) + (float)0.5) * (float)2) / 2), // set 14400 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 19200)) + (float)0.5) * (float)2) / 2), // set 19200 divide (rounded)
    #if defined SUPPORT_MIDI_BAUD_RATE
    (unsigned short)((((float)((float)48000000 / (float)(16 * 31250)) + (float)0.5) * (float)2) / 2), // set 31250 divide (rounded)
    #endif
    (unsigned short)((((float)((float)48000000 / (float)(16 * 38400)) + (float)0.5) * (float)2) / 2), // set 38400 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 57600)) + (float)0.5) * (float)2) / 2), // set 57600 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 115200)) + (float)0.5) * (float)2) / 2), // set 115200 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 230400)) + (float)0.5) * (float)2) / 2), // set 230400 divide (rounded)
    (unsigned short)((((float)((float)48000000 / (float)(16 * 250000)) + (float)0.5) * (float)2) / 2), // set 250000 divide (rounded)
#endif
};
#if !defined KINETIS_KL && !defined KINETIS_K80 && !defined KINETIS_KE15 && !defined KINETIS_KE18
static const unsigned char IRC48M_clock_fraction[SUPPORTED_BAUD_RATES + 1] = {
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)300) - (int)(48000000 / 16 / 300)) * 32)) + (float)0.5), // set 300 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)600) - (int)(48000000 / 16 / 600)) * 32)) + (float)0.5), // set 600 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)1200) - (int)(48000000 / 16 / 1200)) * 32)) + (float)0.5), // set 1200 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)2400) - (int)(48000000 / 16 / 2400)) * 32)) + (float)0.5), // set 2400 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)4800) - (int)(48000000 / 16 / 4800)) * 32)) + (float)0.5), // set 4800 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)9600) - (int)(48000000 / 16 / 9600)) * 32)) + (float)0.5), // set 9600 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)14400) - (int)(48000000 / 16 / 14400)) * 32)) + (float)0.5), // set 14400 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)19200) - (int)(48000000 / 16 / 19200)) * 32)) + (float)0.5), // set 19200 fraction
    #if defined SUPPORT_MIDI_BAUD_RATE
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)31250) - (int)(48000000 / 16 / 31250)) * 32)) + (float)0.5), // set 31250 fraction
    #endif
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)38400) - (int)(48000000 / 16 / 38400)) * 32)) + (float)0.5), // set 38400 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)57600) - (int)(48000000 / 16 / 57600)) * 32)) + (float)0.5), // set 57600 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)115200) - (int)(48000000 / 16 / 115200)) * 32)) + (float)0.5), // set 115200 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)230400) - (int)(48000000 / 16 / 230400)) * 32)) + (float)0.5), // set 230400 fraction
    (unsigned char)(((float)((((float)48000000 / (float)16 / (float)250000) - (int)(48000000 / 16 / 250000)) * 32)) + (float)0.5), // set 250000 fraction
};
#endif
#endif


#if !defined KINETIS_WITH_PCC
// Configure the LPUART clock and define the baud rate setting to be used
//
static void fnConfigLPUART_clock(int Channel, UART_BAUD_CONFIG *ptrBaudConfig)
{
    #if defined LPUART_IRC48M                                            // use the IRC48M clock as LPUART clock
        #if defined KINETIS_WITH_MCG_LITE
    MCG_MC |= MCG_MC_HIRCEN;                                             // ensure that the IRC48M is operating, even when the processor is not in HIRC mode
        #endif
        #if defined SIM_SOPT2_LPUART0SRC_MCGIRCLK
    SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUART0SRC_MCGIRCLK << (2 * Channel))) | (SIM_SOPT2_LPUART0SRC_IRC48M << (2 * Channel)) | SIM_SOPT2_PLLFLLSEL_IRC48M); // {3} select the 48MHz IRC48MHz clock as source for the particular LPUART
        #else
    SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUARTSRC_MGCIR)) | (SIM_SOPT2_LPUARTSRC_SEL | SIM_SOPT2_PLLFLLSEL_IRC48M)); // {3} select the 48MHz IRC48MHz clock as source for all LPUARTs
        #endif
    ptrBaudConfig->usDivider = IRC48M_clock_divider[ptrBaudConfig->iBaudRateRef];
        #if !defined KINETIS_KL && !defined KINETIS_K80 && !defined KINETIS_KE15 && !defined KINETIS_KE18
    ptrBaudConfig->ucFraction = IRC48M_clock_fraction[ptrBaudConfig->iBaudRateRef];
        #endif
    ptrBaudConfig->iValid = 1;
    #elif defined LPUART_MCGPLLCLK
    SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUART0SRC_MCGIRCLK << (2 * Channel)) | ((SIM_SOPT2_LPUART0SRC_IRC48M << (2 * Channel)) | SIM_SOPT2_PLLFLLSEL_PLL));
    SIM_CLKDIV3 = SIM_CLKDIV3_PLLFLLDIV_2;                               // fixed clock
    #elif defined LPUART_OSCERCLK                                        // clock the UART from the external clock
    SIM_SOPT2 |= (SIM_SOPT2_LPUART0SRC_OSCERCLK << (2 * Channel));
    #else                                                                // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2)
        #if !defined RUN_FROM_LIRC                                       // {206} if the processor is running from the the internal clock we don't change settings here
    MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);                        // ensure that the internal reference is enabled and runs in stop mode
            #if defined USE_FAST_INTERNAL_CLOCK                          // {201}
    MCG_SC = MCG_SC_FCRDIV_1;                                            // remove fast IRC divider
    MCG_C2 |= MCG_C2_IRCS;                                               // select fast internal reference clock (4MHz [8MHz for devices with MCG Lite]) as MCGIRCLK
            #else
    MCG_C2 &= ~MCG_C2_IRCS;                                              // select slow internal reference clock (32kHz [2MHz for devices with MCG Lite]) as MCGIRCLK
            #endif
        #endif
        #if defined SIM_SOPT2_LPUART0SRC_MCGIRCLK
    SIM_SOPT2 |= (SIM_SOPT2_LPUART0SRC_MCGIRCLK << (2 * Channel));       // select the LPUART clock source to MCGIRCLK (dedicated channel)
        #else
    SIM_SOPT2 |= (SIM_SOPT2_LPUART0SRC_MGC << (2 * Channel));            // select the LPUART clock source to MCGIRCLK (general)
        #endif
    #endif
}
#endif
#endif

#if UARTS_AVAILABLE > 0
// Configure the UART clock and define the baud rate setting to be used
//
static void fnConfigUART_clock(int Channel, UART_BAUD_CONFIG *ptrBaudConfig)
{
    #if defined UART0_ClOCKED_FROM_MCGIRCLK                              // clocked from internal 4MHz RC clock
    SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_MCGIRCLK);                          // enable UART0 clock source from MCGIRCLK
    #elif defined UART0_ClOCKED_FROM_OSCERCLK
    SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_OSCERCLK << (2 * Channel));         // enable UART0 clock source from OSCERCLK
    #elif !defined MCG_WITHOUT_PLL && !defined UART0_ClOCKED_FROM_MCGFFLCLK && defined SIM_SOPT2_UART0SRC_MCG
    SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL | (SIM_SOPT2_UART0SRC_MCG << (2 * Channel))); // enable UART clock source from MCGPLLCLK/2
    #elif defined SIM_SOPT2_UART0SRC_MCG
    SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_MCG << (2 * Channel));              // enable UART clock source from MCGFFLCLK
    #else
        #if defined KINETIS_KM
    if ((Channel & 1) != 0)                                              // UART1 and 3 are clocked from the core/system clock and the other from the bus clock
        #elif defined KINETIS_KV10
    if (Channel < 1)                                                     // UART 0 is clocked from the core/system clock and the others from the bus clock
        #else
    if (Channel < 2)
        #endif
    {
        ptrBaudConfig->usDivider = system_clock_divider[ptrBaudConfig->iBaudRateRef]; // set the optimal divider value for the required baud rate
        #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
        ptrBaudConfig->ucFraction = system_clock_fraction[ptrBaudConfig->iBaudRateRef]; // set the fraction value for the required baud rate
        #endif
        ptrBaudConfig->iValid = 1;
    }
    else {
        ptrBaudConfig->usDivider = bus_clock_divider[ptrBaudConfig->iBaudRateRef]; // set the optimal divider value for the required baud rate
        #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
        ptrBaudConfig->ucFraction = bus_clock_fraction[ptrBaudConfig->iBaudRateRef]; // set the fraction value for the required baud rate
        #endif
        ptrBaudConfig->iValid = 1;
    }
    #endif
}
#endif



// General UART/LPUART configuration
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    KINETIS_UART_CONTROL *uart_reg;
    UART_BAUD_CONFIG baud_config = {0};
    unsigned short usDivider = 0;
    #if (!defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_K80) || defined K_STYLE_UART2
    unsigned char ucFraction = 0;
    #endif    
    if (pars->ucSpeed >= SUPPORTED_BAUD_RATES) {                         // in case of invalid baud rate reference
        baud_config.iBaudRateRef = SERIAL_BAUD_19200;                    // default to 19200 Baud
    }
    else {
        baud_config.iBaudRateRef = pars->ucSpeed;
    }
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= (NUMBER_SERIAL)) {
        fnConfigExtSCI((QUEUE_HANDLE)(Channel - NUMBER_SERIAL), pars);   // pass on to external UART driver
        #if defined SUPPORT_HW_FLOW
        if ((pars->Config & RTS_CTS) != 0) {                             // HW flow control defined so configure RTS/CTS pins
            fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
        }
        #endif
        return;
    }
    #endif
    uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(Channel);         // select the register set for use by this channel
    #if LPUARTS_AVAILABLE > 0                                            // if the device has low power UART(s)
        #if UARTS_AVAILABLE > 0                                          // if LPUART(s) and also UART(s)
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        switch (Channel) {                                               // the UART channel
        #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE):                                          // first LPUART
        #else
        case (0):                                                        // LPUART 0
        #endif
        #if defined KINETIS_WITH_PCC
            SELECT_PCC_PERIPHERAL_SOURCE(LPUART0, LPUART0_PCC_SOURCE);   // select the PCC clock used by LPUART0
            POWER_UP_ATOMIC(5, LPUART0);
        #else
            #if defined KINETIS_KL
            POWER_UP_ATOMIC(5, LPUART0);                                 // power up LPUART 0
            #elif defined KINETIS_K80 || defined KINETIS_K66 || defined KINETIS_K65
            POWER_UP_ATOMIC(2, LPUART0);                                 // power up LPUART 0
            #else
            POWER_UP_ATOMIC(6, LPUART0);                                 // power up LPUART 0
            #endif
            fnConfigLPUART_clock(0, &baud_config);                       // configure the clock source to be used
        #endif
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):                                      // second LPUART
            #else
        case (1):                                                        // LPUART 1
            #endif
            #if defined KINETIS_WITH_PCC
            SELECT_PCC_PERIPHERAL_SOURCE(LPUART1, LPUART1_PCC_SOURCE);   // select the PCC clock used by LPUART1
            POWER_UP_ATOMIC(5, LPUART1);
            #else
                #if defined KINETIS_KL
            POWER_UP_ATOMIC(5, LPUART1);                                 // power up LPUART 1
                #else
            POWER_UP_ATOMIC(2, LPUART1);                                 // power up LPUART 1
                #endif
            fnConfigLPUART_clock(1, &baud_config);                       // configure the clock source to be used
            #endif
            break;
        #endif
        #if LPUARTS_AVAILABLE > 2
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 2):                                      // third LPUART
            #else
        case (2):                                                        // LPUART 2
            #endif
            #if defined KINETIS_WITH_PCC
            SELECT_PCC_PERIPHERAL_SOURCE(LPUART2, LPUART2_PCC_SOURCE);   // select the PCC clock used by LPUART2
            POWER_UP_ATOMIC(5, LPUART2);
            #else
                #if defined KINETIS_KL
            POWER_UP_ATOMIC(5, LPUART2);                                 // power up LPUART 2
                #else
            POWER_UP_ATOMIC(2, LPUART2);                                 // power up LPUART 2
                #endif
            fnConfigLPUART_clock(2, &baud_config);                       // configure the clock source to be used
            #endif
            break;
        #endif
        #if LPUARTS_AVAILABLE > 3
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 3):                                      // forth LPUART
            #else
        case (3):                                                        // LPUART 3
            #endif
            POWER_UP_ATOMIC(2, LPUART3);                                 // power up LPUART 3
            fnConfigLPUART_clock(3, &baud_config);                       // configure the clock source to be used
            break;
        #endif
        #if LPUARTS_AVAILABLE > 4
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 4):                                      // fifth PUART
            #else
        case (4):                                                        // LPUART 4
            #endif
            POWER_UP_ATOMIC(2, LPUART4);                                 // power up LPUART 4
            fnConfigLPUART_clock(4, &baud_config);                       // configure the clock source to be used
            break;
        #endif
        default:
            _EXCEPTION("Trying to configure non-existent LPUART channel!");
            return;
        }
        #if UARTS_AVAILABLE > 0 
    }
        #else
            #define SPECIAL_UART_CLOCK   (SPECIAL_LPUART_CLOCK)
        #endif
        #if UARTS_AVAILABLE > 0
    else {
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
        switch (Channel) {
        #if UARTS_AVAILABLE > 0 && (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
        case 0:
            POWER_UP_ATOMIC(4, UART0);                                   // power up UART 0
            fnConfigUART_clock(0, &baud_config);                         // configure the clock source to be used
            break;
        #endif
        #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
        case 1:
            POWER_UP_ATOMIC(4, UART1);                                   // power up UART 1
            fnConfigUART_clock(1, &baud_config);                         // configure the clock source to be used
            break;
        #endif
        #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2) // KL43/KL27
        case 2:
            POWER_UP_ATOMIC(4, UART2);                                   // power up UART 2
            fnConfigUART_clock(2, &baud_config);                         // configure the clock source to be used
            break;
        #endif
        #if UARTS_AVAILABLE > 3
        case 3:
            POWER_UP_ATOMIC(4, UART3);                                   // power up UART 3
            fnConfigUART_clock(3, &baud_config);                         // configure the clock source to be used
            break;
        #endif
        #if UARTS_AVAILABLE > 4
        case 4:
            POWER_UP_ATOMIC(1, UART4);                                   // power up UART 4
            fnConfigUART_clock(4, &baud_config);                         // configure the clock source to be used
            break;
        #endif
        #if UARTS_AVAILABLE > 5
        case 5:
            POWER_UP_ATOMIC(1, UART5);                                   // power up UART 5
            fnConfigUART_clock(5, &baud_config);                         // configure the clock source to be used
            break;
        #endif
        default:
            _EXCEPTION("Trying to configure non-existent UART channel!");
            return;
        }
        #if LPUARTS_AVAILABLE > 0
    }
        #endif
    #endif

    #if defined SUPPORT_HW_FLOW
    if ((pars->Config & RTS_CTS) != 0) {                                 // HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    if (baud_config.iValid == 0) {                                       // the following is being phased out in preference of setting the baud rate settings from a look up table and fnConfigUART_clock()/fnConfigLPUART_clock() integration

    #if (((SYSTEM_CLOCK != BUS_CLOCK) || defined KINETIS_KL || defined KINETIS_KE15 || defined KINETIS_KE18 || (LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0))) && !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
        #if defined KINETIS_KL || defined KINETIS_K80
            #if LPUARTS_AVAILABLE == 0
                #if defined UART0_ClOCKED_FROM_MCGIRCLK                  // clocked from internal 4MHz RC clock
                    #define SPECIAL_UART_CLOCK    (MCGIRCLK)
                #elif defined UART0_ClOCKED_FROM_OSCERCLK
                    #define SPECIAL_UART_CLOCK    (OSCERCLK)
                #else
                    #if !defined MCG_WITHOUT_PLL && !defined UART0_ClOCKED_FROM_MCGFFLCLK
                        #define SPECIAL_UART_CLOCK    (MCGPLLCLK/2)
                    #else
                        #define SPECIAL_UART_CLOCK    (MCGFLLCLK)
                    #endif
                #endif
    if (Channel == 0)                                                    // UART 0 is clocked from a selectable source
            #else
                #if !defined SPECIAL_UART_CLOCK
                    #define SPECIAL_UART_CLOCK  SPECIAL_LPUART_CLOCK
                #endif
    if (Channel < LPUARTS_AVAILABLE)                                     // LPUART is clocked from a selectable source
            #endif
        #else
            #if (UARTS_AVAILABLE == 0)
                #define SPECIAL_UART_CLOCK    (SPECIAL_LPUART_CLOCK)
            #else
                #define SPECIAL_UART_CLOCK    (SYSTEM_CLOCK)
            #endif
            #if defined KINETIS_KM
    if ((Channel & 1) != 0)                                              // UART1 and 3 are clocked from the core/system clock and the other from the bus clock
            #elif defined KINETIS_KV10
    if (Channel < 1)                                                     // UART 0 is clocked from the core/system clock and the others from the bus clock
            #else
    if (Channel <= 1)                                                    // UARTs 0 and 1 are clocked from the core/system clock and the others from the bus clock
            #endif
        #endif
    {
        switch (pars->ucSpeed) {           
        case SERIAL_BAUD_300:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/300) + 1)/2);            // {201} set 300
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)300) - (int)(SPECIAL_UART_CLOCK/16/300)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/300);                     // set 300
        #endif
            break;
        case SERIAL_BAUD_600:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/600) + 1)/2);            // {201} set 600
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)600) - (int)(SPECIAL_UART_CLOCK/16/600)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/600);                     // set 600
        #endif
            break;
        case SERIAL_BAUD_1200:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/1200) + 1)/2);           // {201} set 1200
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)1200) - (int)(SPECIAL_UART_CLOCK/16/1200)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/1200);                    // set 1200
        #endif
            break;
        case SERIAL_BAUD_2400:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/2400) + 1)/2);           // {201} set 2400
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)2400) - (int)(SPECIAL_UART_CLOCK/16/2400)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/2400);                    // set 2400
        #endif
            break;
        case SERIAL_BAUD_4800:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/4800) + 1)/2);           // {201} set 4800
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)4800) - (int)(SPECIAL_UART_CLOCK/16/4800)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/4800);                    // set 4800
        #endif
            break;
        case SERIAL_BAUD_9600:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/9600) + 1)/2);          // {201} set 9600
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)9600) - (int)(SPECIAL_UART_CLOCK/16/9600)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/9600);                   // set 9600
        #endif
            break;
        case SERIAL_BAUD_14400:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/14400) + 1)/2);          // {201} set 14400
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)14400) - (int)(SPECIAL_UART_CLOCK/16/14400)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/14400);                   // set 14400
        #endif
            break;
        default:                                                         // if not valid value set this speed
        case SERIAL_BAUD_19200:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/19200) + 1)/2);          // {201} set 19200
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)19200) - (int)(SPECIAL_UART_CLOCK/16/19200)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/19200);                   // set 19200
        #endif
            break;
        #if defined SUPPORT_MIDI_BAUD_RATE
        case SERIAL_BAUD_31250:                                          // {207} set 31250
            #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/31250) + 1)/2);
            #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)31250) - (int)(SPECIAL_UART_CLOCK/16/31250)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/31250);                   // set 31250
            #endif
            break;
        #endif
        case SERIAL_BAUD_38400:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/38400) + 1)/2);          // {201} set 38400
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)38400) - (int)(SPECIAL_UART_CLOCK/16/38400)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/38400);                   // set 38400
        #endif
            break;
        case SERIAL_BAUD_57600:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/57600) + 1)/2);          // {201} set 57600
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)57600) - (int)(SPECIAL_UART_CLOCK/16/57600)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/57600);                   // set 57600
        #endif
            break;
        case SERIAL_BAUD_115200:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/115200) + 1)/2);         // {201} set 115200
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)115200) - (int)(SPECIAL_UART_CLOCK/16/115200)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/115200);                  // set 115200
        #endif
            break;
        case SERIAL_BAUD_230400:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/230400) + 1)/2);         // {201} set 230400
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)230400) - (int)(SPECIAL_UART_CLOCK/16/230400)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/230400);                  // set 230400
        #endif
            break;
        case SERIAL_BAUD_250K:
        #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
            usDivider = (((SPECIAL_UART_CLOCK/8/250000) + 1)/2);         // set 250000
        #else
            ucFraction = (unsigned char)(((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)250000) - (int)(SPECIAL_UART_CLOCK/16/250000)) * 32)) + (float)0.5); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/250000);                  // set 250000
        #endif
            break;
        }
    }
    else {
    #elif !defined SPECIAL_UART_CLOCK
        #define SPECIAL_UART_CLOCK    (SYSTEM_CLOCK)
    #endif
    #if (UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE > 0 && defined LPUARTS_PARALLEL) // UARTs above channel 1 use bus clock but LPUARTs use their own special clock
        if (Channel >= UARTS_AVAILABLE) {                                // LPUART using its own special clock
            switch (pars->ucSpeed) {              
            case SERIAL_BAUD_300:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 300)) + (float)0.5) * (float)2)/2); // best divider for 300
        #else
                ucFraction = (unsigned char)((float)(((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)300) - (int)(BUS_CLOCK/16/300)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/300);               // set 300
        #endif
                break;
            case SERIAL_BAUD_600:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 600)) + (float)0.5) * (float)2)/2); // best divider for 600
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)600) - (int)(SPECIAL_LPUART_CLOCK/16/600)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/600);               // set 600
        #endif
                break;
            case SERIAL_BAUD_1200:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 1200)) + (float)0.5) * (float)2)/2); // best divider for 1200
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)1200) - (int)(SPECIAL_LPUART_CLOCK/16/1200)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/1200);              // set 1200
        #endif
                break;
            case SERIAL_BAUD_2400:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 2400)) + (float)0.5) * (float)2)/2); // best divider for 2400
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)2400) - (int)(SPECIAL_LPUART_CLOCK/16/2400)) * 32)) + (float)0.5); // calculate fractio
                usDivider = (SPECIAL_LPUART_CLOCK/16/2400);              // set 2400
        #endif
                break;
            case SERIAL_BAUD_4800:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 4800)) + (float)0.5) * (float)2)/2); // best divider for 4800
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)4800) - (int)(SPECIAL_LPUART_CLOCK/16/4800)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/4800);              // set 4800
        #endif
                break;
            case SERIAL_BAUD_9600:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 9600)) + (float)0.5) * (float)2)/2); // best divider for 9600
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)9600) - (int)(SPECIAL_LPUART_CLOCK/16/9600)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/9600);              // set 9600
        #endif
                break;
            case SERIAL_BAUD_14400:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 14400)) + (float)0.5) * (float)2)/2); // best divider for 14400
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)14400) - (int)(SPECIAL_LPUART_CLOCK/16/14400)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/14400);             // set 14400
        #endif
                break;
            default:                                                     // if not valid value set this speed
            case SERIAL_BAUD_19200:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 19200)) + (float)0.5) * (float)2)/2); // best divider for 19200
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)19200) - (int)(SPECIAL_LPUART_CLOCK/16/19200)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/19200);             // set 19200
        #endif
                break;
        #if defined SUPPORT_MIDI_BAUD_RATE
            case SERIAL_BAUD_31250:                                      // {207} set 31250
            #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 31250)) + (float)0.5) * (float)2)/2); // best divider for 31250
            #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)31250) - (int)(SPECIAL_LPUART_CLOCK/16/ 31250)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/ 31250);            // set 31250
            #endif
                break;
        #endif
            case SERIAL_BAUD_38400:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 38400)) + (float)0.5) * (float)2)/2); // best divider for 38400
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)38400) - (int)(SPECIAL_LPUART_CLOCK/16/38400)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/38400);             // set 38400
        #endif
                break;
            case SERIAL_BAUD_57600:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 57600)) + (float)0.5) * (float)2)/2); // best divider for 57600
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)57600) - (int)(SPECIAL_LPUART_CLOCK/16/57600)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/57600);             // set 57600
        #endif
                break;
            case SERIAL_BAUD_115200:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 115200)) + (float)0.5) * (float)2)/2); // best divider for 115200
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)115200) - (int)(SPECIAL_LPUART_CLOCK/16/115200)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/115200);            // set 115200
        #endif
                break;
            case SERIAL_BAUD_230400:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 230400)) + (float)0.5) * (float)2)/2); // best divider for 230400
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)230400) - (int)(SPECIAL_LPUART_CLOCK/16/230400)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/230400);            // set 230400
        #endif
                break;
            case SERIAL_BAUD_250K:
        #if (defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K65 || defined KINETIS_K66) && !defined K_STYLE_UART2
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 250000)) + (float)0.5) * (float)2)/2); // best divider for 250000
        #else
                ucFraction = (unsigned char)(((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)250000) - (int)(SPECIAL_LPUART_CLOCK/16/ 250000)) * 32)) + (float)0.5); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/250000);            // set 250000
        #endif
                break;
            }
        }
        else {
    #endif
    #if (UARTS_AVAILABLE > 2) || (defined KINETIS_KEA8 || defined KINETIS_K02 || defined K_STYLE_UART2) || (defined KINETIS_KV && UARTS_AVAILABLE > 1)
            // UART uses bus clock
            //
            baud_config.usDivider = bus_clock_divider[baud_config.iBaudRateRef]; // set the optimal divider value for the required baud rate
        #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
            baud_config.ucFraction = bus_clock_fraction[baud_config.iBaudRateRef]; // set the fraction value for the required baud rate
        #endif
            baud_config.iValid = 1;
    #endif
    #if UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE > 0 && defined LPUARTS_PARALLEL
        }
    #endif
    #if (((SYSTEM_CLOCK != BUS_CLOCK) || defined KINETIS_KL || defined KINETIS_KE15 || defined KINETIS_KE18 || (LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0))) && !(defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18)
    }
    #endif

    }

    #if defined UART_EXTENDED_MODE && defined UART_TIMED_TRANSMISSION && defined SERIAL_SUPPORT_DMA // {208}
    if ((pars->Config & UART_TIMED_TRANSMISSION_MODE) != 0) {
        ulInterCharTxDelay[Channel] = PIT_US_DELAY(pars->usMicroDelay);
    }
    else {
        ulInterCharTxDelay[Channel] = 0;
    }
    #endif
    #if defined UART_HW_TRIGGERED_MODE_SUPPORTED
    if ((pars->Config & UART_HW_TRIGGERED_TX_MODE) != 0) {               // {213}
        ucTriggeredTX_mode[Channel] = 1;                                 // mark that we are operating in HW triggered mode
    }
    #endif    
    #if LPUARTS_AVAILABLE > 0                                            // {106}
        #if UARTS_AVAILABLE > 0                                          // device contains both UART and LPUART
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // configure the low power UART
        #endif
        if (baud_config.iValid == 0) {
            baud_config.usDivider = usDivider;
        }
        fnConfigLPUART(Channel, pars, (KINETIS_LPUART_CONTROL *)uart_reg, baud_config.usDivider); // configure the LPUART
        #if UARTS_AVAILABLE > 0
    }
    else {
            #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined K_STYLE_UART2
        if (baud_config.iValid == 0) {
            baud_config.ucFraction = ucFraction;
            baud_config.usDivider = usDivider;
        }
        uart_reg->UART_C4  = baud_config.ucFraction;
            #endif
        fnConfigUART(Channel, pars, uart_reg, baud_config.usDivider);    // configure the UART
    }
        #endif
    #else
        #if !defined KINETIS_KL && !defined KINETIS_KE
    if (baud_config.iValid == 0) {
        baud_config.ucFraction = ucFraction;
    }
    uart_reg->UART_C4 = baud_config.ucFraction;
        #endif
    if (baud_config.iValid == 0) {
        baud_config.usDivider = usDivider;
    }
    fnConfigUART(Channel, pars, uart_reg, baud_config.usDivider);        // configure the UART
    #endif

    #if defined USER_DEFINED_UART_RX_HANDLER
    fnUserRxIrq[Channel] = pars->receptionHandler;
    #endif
    #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
    fnUserRxBreakIrq[Channel] = pars->receiveBreakHandler;
    #endif
    #if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
    fnUserTxEndIrq[Channel] = pars->txFrameCompleteHandler;
    #endif

    #if defined _WINDOWS
    if (baud_config.iValid == 0) {
        #if (!defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_K80) || defined K_STYLE_UART2
        baud_config.ucFraction = ucFraction;
        #endif
        baud_config.usDivider = usDivider;
    }
        #if (!defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_K80) || defined K_STYLE_UART2
    _fnConfigSimSCI(Channel, pars, baud_config.usDivider, baud_config.ucFraction, BUS_CLOCK, SPECIAL_UART_CLOCK); // open a serial port on PC if desired
        #else
    _fnConfigSimSCI(Channel, pars, baud_config.usDivider, 0, BUS_CLOCK, SPECIAL_UART_CLOCK); // open a serial port on PC if desired
        #endif
    #endif
}

#if defined _WINDOWS
static void _fnConfigSimSCI(QUEUE_HANDLE Channel, TTYTABLE *pars, unsigned short usDivider, unsigned char ucFraction, unsigned long ulBusClock, unsigned long ulSpecialClock)
{
    unsigned long ulBaudRate;
    #if defined KINETIS_KL || defined KINETIS_K80 || defined KINETIS_KE15 || defined KINETIS_KE18
        #if LPUARTS_AVAILABLE > 0
            #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {
        ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider)/16);
    }
    else {
        #if defined K_STYLE_UART2
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider + (((float)ucFraction)/32))/16);
        #else
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider)/16);
        #endif
    }
            #else
    ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider)/16);
            #endif
        #else
    if (Channel == 0) {
        ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider)/16);
    }
    else {
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider)/16);
    }
        #endif
    #elif defined KINETIS_KE
    ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider)/16);// theoretical baud rate
    #else
        #if defined KINETIS_KV10
    if (Channel < 1)                                                     // UART 0 is clocked from the core clock
        #else
    if (Channel <= 1)                                                    // UARTs 0 and 1 are clocked from the core or a special clock
        #endif
    {
        ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider + (((float)ucFraction)/32))/16);
    }
    else {                                                               // remaining UARTs are clocked from the bus clock
        #if LPUARTS_AVAILABLE > 0
        if (Channel >= UARTS_AVAILABLE) {
            ulBaudRate = (unsigned long)((float)SPECIAL_LPUART_CLOCK /((float)usDivider)/16);
        }
        else {
        #endif
            ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider + (((float)ucFraction)/32))/16);
        #if LPUARTS_AVAILABLE > 0
            }
        #endif
    }
    #endif
    fnConfigSimSCI(Channel, ulBaudRate, pars);
}
#endif

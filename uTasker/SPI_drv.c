/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      SPI_drv.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#if defined SPI_INTERFACE

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined SPI_DRV_MALLOC
    #define SPI_DRV_MALLOC(x)         uMalloc((MAX_MALLOC)(x))
    #define SPI_DRV_MALLO_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (unsigned short)(y))
#endif

#define NUMBER_SPI    1

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */
static void send_next_byte(QUEUE_HANDLE channel, TTYQUE *ptTTYQue);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


static TTYQUE *tx_control[NUMBER_SPI] = {0};
static TTYQUE *rx_control[NUMBER_SPI] = {0};


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_spi(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    TTYQUE *ptTTYQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_DRIVER:                                                    // request changes and return status
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // rx state as default
        if ((((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) != 0) || (ptTTYQue == 0)) { // set tx state if specified or in case there is no rx control available
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control);// tx state
        }

        if (Counter != 0) {                                              // modify driver state
            if ((Counter & RX_ON) != 0) {
#if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX
                if ((ptTTYQue->ucDMA_mode & UART_RX_DMA) != 0) {
                    QUEUE_TRANSFER transfer_length = ptTTYQue->tty_queue.buf_length;
                    if ((ptTTYQue->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) != 0) {
                        transfer_length /= 2;
                    }
                    if ((ptTTYQue->opn_mode & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
                        transfer_length -= 2;
    #else
                        transfer_length -= 1;
    #endif
                    }
                    ptTTYQue->msgchars = transfer_length;
                    fnPrepareRxDMA(channel, ptTTYQue->tty_queue.put, transfer_length);
                }
                else {
                    fnRxOn(channel);
                }
#else
                fnRxOn(channel);
#endif
            }
            else if ((Counter & RX_OFF) != 0) {
                fnRxOff(channel);
            }

            if ((Counter & TX_ON) != 0) {
                if ((Counter & PAUSE_TX) != 0) { 
                    ptTTYQue->ucState &= ~(TX_WAIT);                     // remove pause
                    if ((ptTTYQue->ucState & (TX_ACTIVE)) == 0) {
                        send_next_byte(channel, ptTTYQue);               // this is not done when the transmitter is already performing a transfer or if suspended
                    }
                }
                else {
                    fnTxOn(channel);
                }
            }
            else if ((Counter & TX_OFF) != 0) {
                if ((Counter & PAUSE_TX) != 0) {
                    ptTTYQue->ucState |= (TX_WAIT);                      // pause transmission so that data can be added to the buffer without yet being released
                }
                else {
                    fnTxOff(channel);
                }
            }
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
            if ((Counter & SET_MSG_CNT_MODE) != 0) {
                ptTTYQue->ucMessageTerminator = (unsigned char)((CAST_POINTER_ARITHMETIC)ptBuffer & 0xff);
                ptTTYQue->opn_mode |= (MSG_MODE | MSG_MODE_RX_CNT);
            }
            else if (Counter & SET_CHAR_MODE) {
                ptTTYQue->opn_mode &= ~(MSG_MODE | MSG_MODE_RX_CNT);
            }
#endif

            if ((Counter & MODIFY_WAKEUP) != 0) {
                ptTTYQue->wake_task = (UTASK_TASK)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
            }
        }
        rtn_val = (QUEUE_TRANSFER)ptTTYQue->opn_mode;
        break;
#if !defined _NO_CHECK_QUEUE_INPUT
    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        if ((ptTTYQue->opn_mode & (MSG_MODE | MSG_BREAK_MODE)) != 0) {
    #if defined (SUPPORT_MSG_CNT)
            if ((ptTTYQue->opn_mode & MSG_MODE_RX_CNT) != 0) {
                rtn_val = (ptTTYQue->msgs + 1)/2;                        // in count mode we count the count and the actual message
            }
            else {
                rtn_val = ptTTYQue->msgs;
            }
    #else
            rtn_val = ptTTYQue->msgs;
    #endif
        }
        else {
    #if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX && defined SPI_SUPPORT_DMA_RX_FREERUN
        if ((ptTTYQue->ucDMA_mode & (UART_RX_DMA | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_BREAK)) == (UART_RX_DMA)) { // if receiver is free-running in DMA mode
            fnPrepareRxDMA(channel, (unsigned char *)&(ptTTYQue->tty_queue), 0); // update the input with present DMA reception information
        }
    #endif
            rtn_val = ptTTYQue->tty_queue.chars;
        }
        break;
#endif
    case CALL_WRITE:                                                     // write data into the queue
                                                                         // copy the data to the output buffer and start transmission if not already done
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        if (ptBuffer == 0) {                                             // the caller wants to see whether the data will fit and not copy data so inform
#if defined SPI_SUPPORT_DMA_
            if ((ptTTYQue->ucDMA_mode & UART_TX_DMA) != 0) {
                QUEUE_TRANSFER reduction = (ptTTYQue->lastDMA_block_length - fnRemainingDMA_tx(channel)); // get the number of characters
                ptTTYQue->tty_queue.chars -= reduction;
                ptTTYQue->lastDMA_block_length -= reduction;
                ptTTYQue->tty_queue.put += reduction;
                if (ptTTYQue->tty_queue.put >= ptTTYQue->tty_queue.buffer_end) {
                    ptTTYQue->tty_queue.put -= ptTTYQue->tty_queue.buf_length;
                }
            }
#endif
            if ((ptTTYQue->tty_queue.buf_length - ptTTYQue->tty_queue.chars) >= Counter) {
                rtn_val = (ptTTYQue->tty_queue.buf_length - ptTTYQue->tty_queue.chars); // the remaining space
            }
        }
        else {
            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
                rtn_val = fnFillBuf(&ptTTYQue->tty_queue, ptBuffer, Counter);
            uDisable_Interrupt();
            if ((ptTTYQue->ucState & (TX_WAIT | TX_ACTIVE)) == 0) {
                send_next_byte(channel, ptTTYQue);                       // this is not done when the transmitter is already performing a transfer or if suspended
            }
            uEnable_Interrupt();
            return (rtn_val);
        }
        break;

    case CALL_READ:                                                      // read data from the queue
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
#if defined (SUPPORT_MSG_MODE) || defined (SUPPORT_MSG_CNT)
        if (ptTTYQue->opn_mode & (MSG_MODE | MSG_MODE_RX_CNT | MSG_BREAK_MODE)) {
    #if defined SUPPORT_MSG_MODE_EXTRACT
            if (Counter == 0) {
                Counter = 1;
            }
            else {
    #endif
                if (ptTTYQue->msgs == 0) {
                    uEnable_Interrupt();                                 // enable interrupts
                    return rtn_val;
                }
                --ptTTYQue->msgs;                                        // delete this message (or its length)
    #if defined SUPPORT_MSG_MODE_EXTRACT
            }
    #endif
        }
#endif
#if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX_FREERUN
        if ((ptTTYQue->ucDMA_mode & (UART_RX_DMA | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_BREAK)) == (UART_RX_DMA)) { // if receiver is free-running in DMA mode
            fnPrepareRxDMA(channel, (unsigned char *)&(ptTTYQue->tty_queue), 0); // update the input with present DMA reception information
    #if defined SUPPORT_FLOW_CONTROL && defined SUPPORT_HW_FLOW          // handle CTS control when the buffer is critical
            if (((ptTTYQue->opn_mode & RTS_CTS) != 0) && ((ptTTYQue->ucState & RX_HIGHWATER) == 0) // RTS/CTS for receiver
        #if defined SUPPORT_FLOW_HIGH_LOW
            && ((ptTTYQue->tty_queue.chars >= ptTTYQue->high_water_level)))
        #else
            && ((ptTTYQue->tty_queue.chars > (ptTTYQue->tty_queue.buf_length - HIGH_WATER_MARK))))
        #endif
            {
                fnControlLine(channel, CLEAR_CTS, ((struct stTTYQue *)(que_ids[DriverID].output_buffer_control))->opn_mode); // remove CTS as quickly as possible
                ptTTYQue->ucState |= RX_HIGHWATER;                       // mark that we have requested that the transmitter stop sending to us
        #if defined SERIAL_STATS
                ptTTYQue->ulSerialCounter[SERIAL_STATS_FLOW]++;          // count the number of times we stall the flow
        #endif
            }
    #endif
        }
#endif
        rtn_val = fnGetBuf(&ptTTYQue->tty_queue, ptBuffer, Counter);     // interrupts are re-enabled as soon as no longer critical
#if defined SPI_SUPPORT_DMA
        if (((ptTTYQue->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) != 0) && ((ptTTYQue->msgs & 0x1) == 0)) { // complete message extracted, set to next half buffer
            if (ptTTYQue->tty_queue.get < ptTTYQue->tty_queue.QUEbuffer + (ptTTYQue->tty_queue.buf_length/2)) {
                ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer + (ptTTYQue->tty_queue.buf_length/2);
            }
            else {
                ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer;
            }
        }
#endif
        return rtn_val;

#if defined SPI_STATS
    case CALL_STATS:                                                     // return a driver statistic value
    {
        unsigned char *ptr;
        int i;
        if (Counter >= SPI_COUNTERS) {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
            Counter -= SERIAL_COUNTERS;
        }
        else {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        }
        if (ptBuffer == 0) {                                             // reset a counter rather than obtain it
            if (Counter >= SERIAL_COUNTERS) {
                for (i = 0; i < SERIAL_COUNTERS; i++) {
                    ptTTYQue->ulSerialCounter[i] = 0;                    // delete all output counters
                }
                ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
                for (i = 0; i < SPI_COUNTERS; i++) {
                    ptTTYQue->ulSerialCounter[i] = 0;                    // delete all input counters
                }
            }
            else {
                ptTTYQue->ulSerialCounter[Counter] = 0;                  // reset specific counter
            }
        }
        else {
            ptr = (unsigned char *)&ptTTYQue->ulSerialCounter[Counter];
            i = sizeof(ptTTYQue->ulSerialCounter[0]);
            while (i-- != 0) {
                *ptBuffer++ = *ptr++;
            }
        }
    }
    break;
#endif

#if defined SUPPORT_FLUSH
    case CALL_FLUSH:                                                     // flush input or output queue completely
        if (Counter != FLUSH_RX) {                                       // tx
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        }
        else {                                                           // rx
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
        }
        ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.put = ptTTYQue->tty_queue.QUEbuffer;
        ptTTYQue->msgs = ptTTYQue->tty_queue.chars = 0;

    #if defined SUPPORT_MSG_CNT
        if ((Counter == FLUSH_RX) && ((ptTTYQue->opn_mode & MSG_MODE_RX_CNT) != 0)) {
          //*(ptTTYQue->tty_queue.put) = 0;                              // reserve space for length in next message
            (ptTTYQue->tty_queue.put)++;
        #if defined MSG_CNT_WORD
            (ptTTYQue->tty_queue.put)++;
        #endif
        #if defined SUPPORT_MSG_MODE
            ptTTYQue->msgchars = 0;
        #endif
            ptTTYQue->tty_queue.chars = 1;
        }
    #endif
        break;
#endif

    default:
       break;
    }
    uEnable_Interrupt();                                                 // enable interrupts
    return (rtn_val);
}

#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
static TTYQUE *fnGetControlBlock(QUEUE_TRANSFER queue_size, int iModulo)
#else
static TTYQUE *fnGetControlBlock(QUEUE_TRANSFER queue_size)
#endif
{
    TTYQUE *ptTTYQue;

    if (NO_MEMORY == (ptTTYQue = (TTYQUE *)SPI_DRV_MALLOC(sizeof(struct stTTYQue)))) {
        return (0);                                                      // failed, no memory
    }
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
    if (iModulo != 0) {
        if (NO_MEMORY == (ptTTYQue->tty_queue.QUEbuffer = (unsigned char *)TTY_DRV_MALLO_ALIGN(queue_size, queue_size))) {
            return (0);                                                  // failed, no memory
        }
    }
    else {
#endif
        if (NO_MEMORY == (ptTTYQue->tty_queue.QUEbuffer = (unsigned char *)SPI_DRV_MALLOC(queue_size))) {
            return (0);                                                  // failed, no memory
        }
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
    }
#endif
    ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.put = ptTTYQue->tty_queue.buffer_end = ptTTYQue->tty_queue.QUEbuffer;
    ptTTYQue->tty_queue.buffer_end += queue_size;
    ptTTYQue->tty_queue.buf_length = queue_size;
    return (ptTTYQue);                                                   // malloc returns zeroed memory so other variables are zero initialised
}


extern QUEUE_HANDLE fnOpenSPI(SPITABLE *pars, unsigned char driver_mode)
{
    QUEUE_HANDLE DriverID;
    IDINFO *ptrQueue;
    QUEUE_TRANSFER (*entry_add)(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID) = entry_spi;

    if (NO_ID_ALLOCATED != (DriverID = fnSearchID (entry_add, pars->Channel))) {
        if ((driver_mode & MODIFY_CONFIG) == 0) {
            return DriverID;                                             // channel already configured
        }
    }
    else if (NO_ID_ALLOCATED == (DriverID = fnSearchID (0, 0))) {        // get next free ID
        return (NO_ID_ALLOCATED);                                        // no free IDs available
    }

    ptrQueue = &que_ids[DriverID - 1];
    ptrQueue->CallAddress = entry_add;

    if ((driver_mode & FOR_WRITE) != 0) {                                // define transmitter
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
        ptrQueue->output_buffer_control = (QUEQUE *)(tx_control[pars->Channel] = fnGetControlBlock(pars->Rx_tx_sizes.TxQueueSize, ((pars->ucDMAConfig & UART_TX_MODULO) != 0)));
#else
        ptrQueue->output_buffer_control = (QUEQUE *)(tx_control[pars->Channel] = fnGetControlBlock(pars->Rx_tx_sizes.TxQueueSize));
#endif
    }

    if ((driver_mode & FOR_READ) != 0) {                                 // define receiver
        TTYQUE *ptTTYQue;
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
        ptTTYQue = fnGetControlBlock(pars->Rx_tx_sizes.RxQueueSize, ((pars->ucDMAConfig & UART_RX_MODULO) != 0));
#else
        ptTTYQue = fnGetControlBlock(pars->Rx_tx_sizes.RxQueueSize);
#endif
        ptrQueue->input_buffer_control = (QUEQUE *)(rx_control[pars->Channel] = ptTTYQue);
#if defined SUPPORT_MSG_CNT
        if ((pars->Config & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
            ptTTYQue->tty_queue.put += 2;
            ptTTYQue->tty_queue.chars = 2;                               // reserve for length of first message
    #else
            ++ptTTYQue->tty_queue.put;
            ptTTYQue->tty_queue.chars = 1;                               // reserve for length of first message
    #endif
        }
#endif
    }

    if (tx_control[pars->Channel] != 0) {                                // if transmission is supported on the channel
        tx_control[pars->Channel]->opn_mode = pars->Config;
#if defined SPI_SUPPORT_DMA
        tx_control[pars->Channel]->ucDMA_mode = pars->ucDMAConfig;       // the DMA configuration for this channel
     #if defined UART_TIMED_TRANSMISSION
        tx_control[pars->Channel]->usMicroDelay = pars->usMicroDelay;    // inter-character transmission timebase (valid when UART_TIMED_TRANSMISSION_MODE is specified)
    #endif
#endif
#if defined WAKE_BLOCKED_TX
    #if defined WAKE_BLOCKED_TX_BUFFER_LEVEL
        tx_control[pars->Channel]->low_water_level  = pars->tx_wake_level; // set specified wake up level
    #elif defined SUPPORT_FLOW_HIGH_LOW
        tx_control[pars->Channel]->low_water_level  = ((pars->ucFlowLowWater * pars->Rx_tx_sizes.TxQueueSize)/100);   // save the number of byte converted from % of buffer size
    #endif
#endif
    }

    if (rx_control[pars->Channel] != 0) {
        rx_control[pars->Channel]->wake_task = pars->Task_to_wake;
#if defined SUPPORT_FLOW_HIGH_LOW
        rx_control[pars->Channel]->high_water_level = ((pars->ucFlowHighWater * pars->Rx_tx_sizes.RxQueueSize)/100);  // save the number of byte converted from % of buffer size
        rx_control[pars->Channel]->low_water_level  = ((pars->ucFlowLowWater * pars->Rx_tx_sizes.RxQueueSize)/100);   // save the number of byte converted from % of buffer size
#endif
        rx_control[pars->Channel]->opn_mode = pars->Config;
#if defined SPI_SUPPORT_DMA
        rx_control[pars->Channel]->ucDMA_mode = pars->ucDMAConfig;
#endif
#if defined SUPPORT_MSG_MODE
        rx_control[pars->Channel]->ucMessageTerminator = pars->ucMessageTerminator;
#endif
#if defined SERIAL_SUPPORT_ESCAPE
        rx_control[pars->Channel]->ucMessageFilter = pars->ucMessageFilter;
#endif
    }

    ptrQueue->qHandle = pars->Channel;
    fnConfigSPI(pars);                                                   // configure hardware for this channel
    return (DriverID);                                                   // return the allocated ID
}

#if defined WAKE_BLOCKED_TX
static void fnWakeBlockedTx(TTYQUE *ptTTYQue, QUEUE_TRANSFER low_water_level)
{
    if ((ptTTYQue->wake_task != 0) && (ptTTYQue->tty_queue.chars <= low_water_level)) { // we have just reduced the buffer content adequately so inform a blocked task that it can continue writing
        unsigned char tx_continue_message[ HEADER_LENGTH ]; // = { INTERNAL_ROUTE, INTERNAL_ROUTE , ptTTYQue->wake_task, INTERRUPT_EVENT, TX_FREE };  // define standard interrupt event
        tx_continue_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
        tx_continue_message[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;
        tx_continue_message[MSG_DESTINATION_TASK] = ptTTYQue->wake_task;
        tx_continue_message[MSG_SOURCE_TASK]      = INTERRUPT_EVENT;
        tx_continue_message[MSG_INTERRUPT_EVENT]  = TX_FREE;

        fnWrite(INTERNAL_ROUTE, (unsigned char*)tx_continue_message, HEADER_LENGTH); // inform the blocked task
        ptTTYQue->wake_task = 0;                                         // remove task since this is only performed once
    }
}
#endif

static void send_next_byte(QUEUE_HANDLE channel, TTYQUE *ptTTYQue)       // interrupts are assumed to be disabled here
{
#if defined SPI_SUPPORT_DMA
    if ((ptTTYQue->ucDMA_mode & UART_TX_DMA) != 0) {                     // DMA mode of operation
        ptTTYQue->tty_queue.chars -= ptTTYQue->lastDMA_block_length;     // the last block has been transmitted so this space is available again for additional characters
        ptTTYQue->lastDMA_block_length = 0;
    }
#endif
#if defined SERIAL_SUPPORT_XON_XOFF                                      // support for XON XOFF at receiver
    if ((ptTTYQue->ucState & (SEND_XON | SEND_XOFF)) != 0) {             // if receiver requires this to be sent by the transmitter
        unsigned char ucBusy;
        if ((ptTTYQue->ucState & SEND_XON) != 0) {
            ucBusy = fnTxByte(channel, XON_CODE);
        }
        else {
            ucBusy = fnTxByte(channel, XOFF_CODE);
        }
        if (ucBusy == 0) {
            ptTTYQue->ucState |= TX_ACTIVE;                              // set active flag during XON/XOFF transmission phase {8}
            ptTTYQue->ucState &= ~(SEND_XON | SEND_XOFF);
        }
        return;
    }
#endif

    if ((ptTTYQue->ucState & TX_WAIT) == 0) {                            // send the next byte if possible - either first char or tx interrupt
#if defined SERIAL_SUPPORT_ESCAPE
        int iBlock = 0;
#endif
        if (ptTTYQue->tty_queue.chars == 0) {                            // are there more to send?
            ptTTYQue->ucState &= ~TX_ACTIVE;                             // transmission of a block has terminated
            fnClearTxInt(channel);                                       // clear interrupt
#if defined (WAKE_BLOCKED_TX) && defined (SPI_SUPPORT_DMA)
            fnWakeBlockedTx(ptTTYQue, 0);
#endif
#if defined UART_BREAK_SUPPORT
            if ((ptTTYQue->opn_mode & BREAK_AFTER_TX) != 0) {
                fnStartBreak(channel);
            }
#endif
#if defined UART_FRAME_COMPLETE
            if ((ptTTYQue->opn_mode & INFORM_ON_FRAME_TRANSMISSION) != 0) {
                fnUARTFrameTermination(channel);                         // this routine is supplied by the user
            }
#endif
        }
        else {
            unsigned char ucNextByte = *(ptTTYQue->tty_queue.get);
#if defined SERIAL_SUPPORT_ESCAPE
            if ((ptTTYQue->opn_mode & TX_ESC_MODE) != 0) {
                if (ptTTYQue->ucState & ESCAPE_SEQUENCE) {
                    ptTTYQue->ucState &= ~ESCAPE_SEQUENCE;
                }
                else if ((ucNextByte == ptTTYQue->ucMessageFilter) || ((ucNextByte == ptTTYQue->ucMessageTerminator) && (ptTTYQue->tty_queue.chars != 1))) {
                    ptTTYQue->ucState |= ESCAPE_SEQUENCE;
                    ucNextByte = ptTTYQue->ucMessageFilter;
                    iBlock = 1;
                }
            }
#endif
#if defined SPI_SUPPORT_DMA
            if ((ptTTYQue->ucDMA_mode & UART_TX_DMA) != 0) {             // DMA mode of operation
                QUEUE_TRANSFER tx_length;
                QUEUE_TRANSFER charactersWaiting = ptTTYQue->tty_queue.chars;
    #if defined UART_HW_TRIGGERED_MODE_SUPPORTED
                if ((ptTTYQue->opn_mode & UART_HW_TRIGGERED_TX_MODE) != 0) {
                    charactersWaiting = *ptTTYQue->tty_queue.get++;      // the first two bytes in the queue are the message length
                    charactersWaiting <<= 8;
                    charactersWaiting |= *ptTTYQue->tty_queue.get++;
                    ptTTYQue->tty_queue.chars -= 2;
        #if defined _WINDOWS
                    if (charactersWaiting > ptTTYQue->tty_queue.chars) {
                        _EXCEPTION("Bad use of UART_HW_TRIGGERED_TX_MODE"); // the first two characters indicate the individual transmit message length
                    }
                    if (ptTTYQue->tty_queue.get >= ptTTYQue->tty_queue.buffer_end) {
                        _EXCEPTION("Bad use of UART_HW_TRIGGERED_TX_MODE"); // a linear buffer is expected
                    }
        #endif
                }
    #endif
                // Calculate whether we can send block in one go or not
                //
                if ((ptTTYQue->tty_queue.get + charactersWaiting) >= ptTTYQue->tty_queue.buffer_end) {
                    tx_length = (QUEUE_TRANSFER)(ptTTYQue->tty_queue.buffer_end - ptTTYQue->tty_queue.get); // single transfer up to the end of the buffer
                }
                else {
                    tx_length = charactersWaiting;                       // single transfer block
                }
                if (tx_length == 0) {
                    return;
                }
    #if defined WAKE_BLOCKED_TX
        #if defined SUPPORT_FLOW_HIGH_LOW
                if ((ptTTYQue->tty_queue.chars > ptTTYQue->tty_queue.buf_length/2) && ((ptTTYQue->tty_queue.chars - tx_length) >= tx_control[channel]->low_water_level)) 
        #else
                if ((ptTTYQue->tty_queue.chars > ptTTYQue->tty_queue.buf_length/2) && ((ptTTYQue->tty_queue.chars - tx_length) >= LOW_WATER_MARK)) 
        #endif
                {
                    if (tx_length > ptTTYQue->tty_queue.chars/2) {
                        tx_length /= 2;                                  // perform the DMA in at least two blocks so that a waiting task
                    }
                }                                                        // has a chance of being woken before the transmit buffer completely empties
    #endif
                ptTTYQue->ucState |= TX_ACTIVE;                          // mark activity
                tx_length = fnTxByteDMA(channel, ptTTYQue->tty_queue.get, tx_length); // initiate buffer transfer using DMA
                if ((ptTTYQue->tty_queue.get += tx_length) >= ptTTYQue->tty_queue.buffer_end) { // assume that transmission will be successful
                    ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer;
                }
    #if defined WAKE_BLOCKED_TX
        #if defined SUPPORT_FLOW_HIGH_LOW
                fnWakeBlockedTx(ptTTYQue, tx_control[channel]->low_water_level);
        #else
                fnWakeBlockedTx(ptTTYQue, LOW_WATER_MARK);                
        #endif
    #endif
        //      ptTTYQue->tty_queue.chars -= tx_length;                  // don't remove length until after complete transmission
                ptTTYQue->lastDMA_block_length = tx_length;              // save the block length for removal after complete transmission
                return;
            }
#endif
            fnTxByte(channel, ucNextByte);
#if defined SERIAL_STATS
            ptTTYQue->ulSerialCounter[SERIAL_STATS_CHARS]++;
#endif
            ptTTYQue->ucState |= TX_ACTIVE;                              // mark activity

#if defined SERIAL_SUPPORT_ESCAPE
            if (iBlock != 0) {
                return;                                                  // we have sent an escape character and so don't delete from input buffer
            }
#endif
            if (++ptTTYQue->tty_queue.get >= ptTTYQue->tty_queue.buffer_end) { // wrap in circular buffer
                ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer; // wrap to beginning
            }
            --ptTTYQue->tty_queue.chars;
#if defined WAKE_BLOCKED_TX
    #if defined SUPPORT_FLOW_HIGH_LOW
            fnWakeBlockedTx(ptTTYQue, tx_control[channel]->low_water_level);
    #else
            fnWakeBlockedTx(ptTTYQue, LOW_WATER_MARK);
    #endif
#endif
        }
    }
    else {
        fnClearTxInt(channel);                                           // clear interrupt since we are not allowed to send at the moment
    }
}

/* =================================================================== */
/*                         interrupt  interface routines               */
/* =================================================================== */

// This is called by the interrupt routine to put one received byte into the input buffer
//
extern void fnSPIRxByte(unsigned char ch, QUEUE_HANDLE Channel)
{
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
    unsigned char *ptrBuffer;
#endif
    int iBlockBuffer = 0;
    TTYQUE *rx_ctl = rx_control[Channel];
#if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX
    if ((rx_ctl->ucDMA_mode & UART_RX_DMA) != 0) {                       // new characters in the buffer - increment message count
        QUEUE_TRANSFER transfer_length = rx_ctl->tty_queue.buf_length;
    #if defined SERIAL_SUPPORT_RX_DMA_BREAK
        if ((UART_RX_DMA_BREAK & rx_ctl->ucDMA_mode) != 0) {             // break detected
            QUEUE_TRANSFER OriginalLength = rx_ctl->tty_queue.chars;
            fnPrepareRxDMA(Channel, (unsigned char *)(&(rx_ctl->tty_queue)), 0); // update the waiting data
            OriginalLength = (rx_ctl->tty_queue.chars - OriginalLength); // the additional characters collected since the previous break
            if (ch != 0) {                                               // first break after enabling the received so we ignore received data
                rx_ctl->tty_queue.get += rx_ctl->tty_queue.chars;
                if (rx_ctl->tty_queue.get >= rx_ctl->tty_queue.buffer_end) {
                    rx_ctl->tty_queue.get -= rx_ctl->tty_queue.buf_length;
                }
                rx_ctl->tty_queue.chars = 0;
            }
            else {
                fnPostFrameEvent(rx_ctl->wake_task, (unsigned char)TTY_BREAK_FRAME_RECEPTION, Channel, OriginalLength);
            }
            return;
        }
    #endif
        if ((rx_ctl->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) != 0) {
            transfer_length /= 2;
        }
        rx_ctl->tty_queue.put += transfer_length;
        rx_ctl->tty_queue.chars += transfer_length;                      // amount of characters that are ready to be read
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
            transfer_length -= 2;
    #else
            transfer_length -= 1;
    #endif
        }
        if (rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) {
            rx_ctl->tty_queue.put -= rx_ctl->tty_queue.buf_length;
        }
        fnPrepareRxDMA(Channel, rx_ctl->tty_queue.put, transfer_length); // enable next reception
        rx_ctl->msgs++;                                                  // we have 1 complete message
        if (rx_ctl->wake_task != 0) {                                    // wake up an input task, if defined
            uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);     // wake up service interface task
        }
        return;
    }
#endif
#if defined SUPPORT_MSG_MODE
    if ((MSG_MODE & rx_ctl->opn_mode) != 0) {
        if (rx_ctl->ucMessageTerminator == ch) {
            iBlockBuffer = 1;
        }
    }
#endif
#if defined SPI_STATS
    rx_ctl->ulSerialCounter[SPI_STATS_CHARS]++;                          // count the number of characters we receive
#endif
    if (iBlockBuffer == 0) {
        if (rx_ctl->tty_queue.chars < rx_ctl->tty_queue.buf_length) {    // never overwrite contents of buffer
            rx_ctl->tty_queue.chars++;                                   // increment the total character count
#if (defined SUPPORT_MSG_CNT && defined SUPPORT_MSG_MODE) || defined UART_BREAK_SUPPORT
            rx_ctl->msgchars++;                                          // update the present message length
#endif
            *(rx_ctl->tty_queue.put) = ch;                               // put received character in to the input buffer
            if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // wrap in circular buffer
                rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer;     // wrap to beginning
            }
        }
        if ((rx_ctl->opn_mode & (MSG_MODE | MSG_BREAK_MODE)) != 0) {
            return;                                                      // we don't wake until a complete message has been received
        }
    }
    else  {                                                              // end of message received
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
            if (rx_ctl->msgchars != 0) {                                 // if we have not actually collected any characters don't save a length
                rx_ctl->msgs += 2;                                       // a new message length and data message is ready
    #if defined MSG_CNT_WORD
                ptrBuffer = rx_ctl->tty_queue.put - rx_ctl->msgchars - 2;// move back to start of this message
    #else
                ptrBuffer = rx_ctl->tty_queue.put - rx_ctl->msgchars - 1;// move back to start of this message
    #endif
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
    #if defined MSG_CNT_WORD
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
    #endif
                if (ptrBuffer < rx_ctl->tty_queue.QUEbuffer) {           // set pointer to reserved message length position
                    ptrBuffer += rx_ctl->tty_queue.buf_length;
                }
    #if defined MSG_CNT_WORD
                *ptrBuffer = (unsigned char)(rx_ctl->msgchars>>8);
                if (++ptrBuffer >= rx_ctl->tty_queue.buffer_end) {       // handle wrap in circular buffer
                    ptrBuffer = rx_ctl->tty_queue.QUEbuffer;             // wrap to beginning
                }
                rx_ctl->tty_queue.chars++;                               // include the length of next message
    #endif
                *ptrBuffer = (unsigned char)rx_ctl->msgchars;            // write the length of the message at first position (doesn't include the terminator in this mode)
                rx_ctl->tty_queue.chars++;                               // include the length of next message
                rx_ctl->msgchars = 0;                                    // reset for next message
            }
        }
        else {
#endif
            rx_ctl->msgs++;                                              // we have 1 complete message
            if (rx_ctl->tty_queue.chars < rx_ctl->tty_queue.buf_length) {// never overwrite contents of buffer
                ++rx_ctl->tty_queue.chars;
                *(rx_ctl->tty_queue.put) = ch;                           // put received character in input buffer
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
            }
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
        }
#endif
    }

    if (rx_ctl->wake_task != 0) {                                        // wake up an input task, if defined
        uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);         // wake up owner task
    }
}


// The tx interrupt routine calls this
//
extern void fnSPITxByte(QUEUE_HANDLE Channel)
{
    send_next_byte(Channel, tx_control[Channel]);
}

#endif

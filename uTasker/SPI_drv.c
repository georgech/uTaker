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

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnSendNextSPI_word(QUEUE_HANDLE channel, SPIQUE *ptSPIQue);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


static SPIQUE *tx_control[NUMBER_SPI] = {0};
static SPIQUE *rx_control[NUMBER_SPI] = {0};


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
    SPIQUE *ptSPIQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_DRIVER:                                                    // request changes and return status
        ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].input_buffer_control); // rx state as default
        if ((((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) != 0) || (ptSPIQue == 0)) { // set tx state if specified or in case there is no rx control available
            ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].output_buffer_control);// tx state
        }

        if (Counter != 0) {                                              // modify driver state
            if ((Counter & RX_ON) != 0) {
#if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX
                if ((ptSPIQue->ucDMA_mode & UART_RX_DMA) != 0) {
                    QUEUE_TRANSFER transfer_length = ptSPIQue->spi_queue.buf_length;
                    if ((ptSPIQue->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) != 0) {
                        transfer_length /= 2;
                    }
                    if ((ptSPIQue->opn_mode & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
                        transfer_length -= 2;
    #else
                        transfer_length -= 1;
    #endif
                    }
                    ptSPIQue->msgchars = transfer_length;
                    fnPrepareRxDMA(channel, ptSPIQue->spi_queue.put, transfer_length);
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
                    ptSPIQue->ucState &= ~(TX_WAIT);                     // remove pause
                    if ((ptSPIQue->ucState & (TX_ACTIVE)) == 0) {
                        fnSendNextSPI_word(channel, ptSPIQue);           // this is not done when the transmitter is already performing a transfer or if suspended
                    }
                }
                else {
                    fnTxOn(channel);
                }
            }
            else if ((Counter & TX_OFF) != 0) {
                if ((Counter & PAUSE_TX) != 0) {
                    ptSPIQue->ucState |= (TX_WAIT);                      // pause transmission so that data can be added to the buffer without yet being released
                }
                else {
                    fnTxOff(channel);
                }
            }
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
            if ((Counter & SET_MSG_CNT_MODE) != 0) {
                ptSPIQue->ucMessageTerminator = (unsigned char)((CAST_POINTER_ARITHMETIC)ptBuffer & 0xff);
                ptSPIQue->opn_mode |= (SPI_TX_MESSAGE_MODE | MSG_MODE_RX_CNT);
            }
            else if (Counter & SET_CHAR_MODE) {
                ptSPIQue->opn_mode &= ~(SPI_TX_MESSAGE_MODE | MSG_MODE_RX_CNT);
            }
#endif

            if ((Counter & MODIFY_WAKEUP) != 0) {
                ptSPIQue->wake_task = (UTASK_TASK)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
            }
        }
        rtn_val = (QUEUE_TRANSFER)ptSPIQue->opn_mode;
        break;
#if !defined _NO_CHECK_QUEUE_INPUT
    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        if ((ptSPIQue->opn_mode & (SPI_TX_MESSAGE_MODE | MSG_BREAK_MODE)) != 0) {
    #if defined (SUPPORT_MSG_CNT)
            if ((ptSPIQue->opn_mode & MSG_MODE_RX_CNT) != 0) {
                rtn_val = (ptSPIQue->msgs + 1)/2;                        // in count mode we count the count and the actual message
            }
            else {
                rtn_val = ptSPIQue->msgs;
            }
    #else
            rtn_val = ptSPIQue->msgs;
    #endif
        }
        else {
    #if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX && defined SPI_SUPPORT_DMA_RX_FREERUN
        if ((ptSPIQue->ucDMA_mode & (UART_RX_DMA | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_BREAK)) == (UART_RX_DMA)) { // if receiver is free-running in DMA mode
            fnPrepareRxDMA(channel, (unsigned char *)&(ptSPIQue->spi_queue), 0); // update the input with present DMA reception information
        }
    #endif
            rtn_val = ptSPIQue->spi_queue.chars;
        }
        break;
#endif
    case CALL_WRITE:                                                     // write data into the queue
                                                                         // copy the data to the output buffer and start transmission if not already done
        ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        if (ptBuffer == 0) {                                             // the caller wants to see whether the data will fit and not copy data so inform
#if defined SPI_SUPPORT_DMA_
            if ((ptSPIQue->ucDMA_mode & UART_TX_DMA) != 0) {
                QUEUE_TRANSFER reduction = (ptSPIQue->lastDMA_block_length - fnRemainingDMA_tx(channel)); // get the number of characters
                ptSPIQue->spi_queue.chars -= reduction;
                ptSPIQue->lastDMA_block_length -= reduction;
                ptSPIQue->spi_queue.put += reduction;
                if (ptSPIQue->spi_queue.put >= ptSPIQue->spi_queue.buffer_end) {
                    ptSPIQue->spi_queue.put -= ptSPIQue->spi_queue.buf_length;
                }
            }
#endif
            if ((ptSPIQue->spi_queue.buf_length - ptSPIQue->spi_queue.chars) >= Counter) {
                rtn_val = (ptSPIQue->spi_queue.buf_length - ptSPIQue->spi_queue.chars); // the remaining space
            }
        }
        else {
            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
                rtn_val = fnFillBuf(&ptSPIQue->spi_queue, ptBuffer, Counter);
            uDisable_Interrupt();
            if ((ptSPIQue->ucState & (TX_WAIT | TX_ACTIVE)) == 0) {
                fnSendNextSPI_word(channel, ptSPIQue);                   // this is not done when the transmitter is already performing a transfer or if suspended
            }
            uEnable_Interrupt();
            return (rtn_val);
        }
        break;

    case CALL_READ:                                                      // read data from the queue
        ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
#if defined (SUPPORT_MSG_MODE) || defined (SUPPORT_MSG_CNT)
        if (ptSPIQue->opn_mode & (SPI_TX_MESSAGE_MODE | MSG_MODE_RX_CNT | MSG_BREAK_MODE)) {
    #if defined SUPPORT_MSG_MODE_EXTRACT
            if (Counter == 0) {
                Counter = 1;
            }
            else {
    #endif
                if (ptSPIQue->msgs == 0) {
                    uEnable_Interrupt();                                 // enable interrupts
                    return rtn_val;
                }
                --ptSPIQue->msgs;                                        // delete this message (or its length)
    #if defined SUPPORT_MSG_MODE_EXTRACT
            }
    #endif
        }
#endif
#if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX_FREERUN
        if ((ptSPIQue->ucDMA_mode & (UART_RX_DMA | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_BREAK)) == (UART_RX_DMA)) { // if receiver is free-running in DMA mode
            fnPrepareRxDMA(channel, (unsigned char *)&(ptSPIQue->spi_queue), 0); // update the input with present DMA reception information
    #if defined SUPPORT_FLOW_CONTROL && defined SUPPORT_HW_FLOW          // handle CTS control when the buffer is critical
            if (((ptSPIQue->opn_mode & RTS_CTS) != 0) && ((ptSPIQue->ucState & RX_HIGHWATER) == 0) // RTS/CTS for receiver
        #if defined SUPPORT_SPI_FLOW_HIGH_LOW
            && ((ptSPIQue->spi_queue.chars >= ptSPIQue->high_water_level)))
        #else
            && ((ptSPIQue->spi_queue.chars > (ptSPIQue->spi_queue.buf_length - HIGH_WATER_MARK))))
        #endif
            {
                fnControlLine(channel, CLEAR_CTS, ((struct stSPIQue *)(que_ids[DriverID].output_buffer_control))->opn_mode); // remove CTS as quickly as possible
                ptSPIQue->ucState |= RX_HIGHWATER;                       // mark that we have requested that the transmitter stop sending to us
            }
    #endif
        }
#endif
        rtn_val = fnGetBuf(&ptSPIQue->spi_queue, ptBuffer, Counter);     // interrupts are re-enabled as soon as no longer critical
#if defined SPI_SUPPORT_DMA
        if (((ptSPIQue->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) != 0) && ((ptSPIQue->msgs & 0x1) == 0)) { // complete message extracted, set to next half buffer
            if (ptSPIQue->spi_queue.get < ptSPIQue->spi_queue.QUEbuffer + (ptSPIQue->spi_queue.buf_length/2)) {
                ptSPIQue->spi_queue.get = ptSPIQue->spi_queue.QUEbuffer + (ptSPIQue->spi_queue.buf_length/2);
            }
            else {
                ptSPIQue->spi_queue.get = ptSPIQue->spi_queue.QUEbuffer;
            }
        }
#endif
        return rtn_val;

#if defined SUPPORT_FLUSH
    case CALL_FLUSH:                                                     // flush input or output queue completely
        if (Counter != FLUSH_RX) {                                       // tx
            ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        }
        else {                                                           // rx
            ptSPIQue = (struct stSPIQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
        }
        ptSPIQue->spi_queue.get = ptSPIQue->spi_queue.put = ptSPIQue->spi_queue.QUEbuffer;
        ptSPIQue->msgs = ptSPIQue->spi_queue.chars = 0;

    #if defined SUPPORT_MSG_CNT
        if ((Counter == FLUSH_RX) && ((ptSPIQue->opn_mode & MSG_MODE_RX_CNT) != 0)) {
          //*(ptSPIQue->spi_queue.put) = 0;                              // reserve space for length in next message
            (ptSPIQue->spi_queue.put)++;
        #if defined MSG_CNT_WORD
            (ptSPIQue->spi_queue.put)++;
        #endif
        #if defined SUPPORT_MSG_MODE
            ptSPIQue->msgchars = 0;
        #endif
            ptSPIQue->spi_queue.chars = 1;
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
static SPIQUE *fnGetControlBlock(QUEUE_TRANSFER queue_size, int iModulo)
#else
static SPIQUE *fnGetControlBlock(QUEUE_TRANSFER queue_size)
#endif
{
    SPIQUE *ptSPIQue;

    if (NO_MEMORY == (ptSPIQue = (SPIQUE *)SPI_DRV_MALLOC(sizeof(struct stSPIQue)))) {
        return (0);                                                      // failed, no memory
    }
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
    if (iModulo != 0) {
        if (NO_MEMORY == (ptSPIQue->spi_queue.QUEbuffer = (unsigned char *)TTY_DRV_MALLO_ALIGN(queue_size, queue_size))) {
            return (0);                                                  // failed, no memory
        }
    }
    else {
#endif
        if (NO_MEMORY == (ptSPIQue->spi_queue.QUEbuffer = (unsigned char *)SPI_DRV_MALLOC(queue_size))) {
            return (0);                                                  // failed, no memory
        }
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
    }
#endif
    ptSPIQue->spi_queue.get = ptSPIQue->spi_queue.put = ptSPIQue->spi_queue.buffer_end = ptSPIQue->spi_queue.QUEbuffer;
    ptSPIQue->spi_queue.buffer_end += queue_size;
    ptSPIQue->spi_queue.buf_length = queue_size;
    return (ptSPIQue);                                                   // malloc returns zeroed memory so other variables are zero initialised
}


extern QUEUE_HANDLE fnOpenSPI(SPITABLE *pars, unsigned char driver_mode)
{
    QUEUE_HANDLE DriverID;
    IDINFO *ptrQueue;
    QUEUE_TRANSFER (*entry_add)(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID) = entry_spi;

    if (NO_ID_ALLOCATED != (DriverID = fnSearchID (entry_add, pars->Channel))) {
        if ((driver_mode & ADD_CONFIG) != 0) {
            tx_control[pars->Channel]->uBusWidth[pars->ucChipSelect] = pars->ucWordWidth;
            fnConfigSPI(pars, 1);                                        // add chip select configurations
        }
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
        SPIQUE *ptSPIQue;
#if defined SPI_SUPPORT_DMA && ((defined KINETIS_KL || defined KINETIS_KM) && !defined DEVICE_WITH_eDMA)
        ptSPIQue = fnGetControlBlock(pars->Rx_tx_sizes.RxQueueSize, ((pars->ucDMAConfig & UART_RX_MODULO) != 0));
#else
        ptSPIQue = fnGetControlBlock(pars->Rx_tx_sizes.RxQueueSize);
#endif
        ptrQueue->input_buffer_control = (QUEQUE *)(rx_control[pars->Channel] = ptSPIQue);
#if defined SUPPORT_SPI_MSG_CNT
        if ((pars->Config & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
            ptSPIQue->spi_queue.put += 2;
            ptSPIQue->spi_queue.chars = 2;                               // reserve for length of first message
    #else
            ++ptSPIQue->spi_queue.put;
            ptSPIQue->spi_queue.chars = 1;                               // reserve for length of first message
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
    #elif defined SUPPORT_SPI_FLOW_HIGH_LOW
        tx_control[pars->Channel]->low_water_level  = ((pars->ucFlowLowWater * pars->Rx_tx_sizes.TxQueueSize)/100);   // save the number of byte converted from % of buffer size
    #endif
#endif
    }

    if (rx_control[pars->Channel] != 0) {
        rx_control[pars->Channel]->wake_task = pars->Task_to_wake;
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
    tx_control[pars->Channel]->uBusWidth[pars->ucChipSelect] = pars->ucWordWidth;
    fnConfigSPI(pars, 0);                                                // configure hardware for this channel
    return (DriverID);                                                   // return the allocated ID
}

#if defined WAKE_BLOCKED_TX
static void fnWakeBlockedTx(SPIQUE *ptSPIQue, QUEUE_TRANSFER low_water_level)
{
    if ((ptSPIQue->wake_task != 0) && (ptSPIQue->spi_queue.chars <= low_water_level)) { // we have just reduced the buffer content adequately so inform a blocked task that it can continue writing
        unsigned char tx_continue_message[HEADER_LENGTH]; // = { INTERNAL_ROUTE, INTERNAL_ROUTE , ptSPIQue->wake_task, INTERRUPT_EVENT, TX_FREE };  // define standard interrupt event
        tx_continue_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
        tx_continue_message[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;
        tx_continue_message[MSG_DESTINATION_TASK] = ptSPIQue->wake_task;
        tx_continue_message[MSG_SOURCE_TASK]      = INTERRUPT_EVENT;
        tx_continue_message[MSG_INTERRUPT_EVENT]  = TX_FREE;

        fnWrite(INTERNAL_ROUTE, (unsigned char*)tx_continue_message, HEADER_LENGTH); // inform the blocked task
        ptSPIQue->wake_task = 0;                                         // remove task since this is only performed once
    }
}
#endif

static unsigned char fnGetFromCircularBuffer(QUEQUE *ptrQUE)
{
    if (ptrQUE->chars > 0) {
        unsigned char ucValue = *(ptrQUE->get);
        if (++ptrQUE->get >= ptrQUE->buffer_end) {                       // check for end of circular buffer
            ptrQUE->get = ptrQUE->QUEbuffer;                             // wrap to beginning
        }
        --ptrQUE->chars;                                                 // one less character
        return ucValue;
    }
    else {
        _EXCEPTION("Extracting non-existet character!");
        return 0;
    }
}


static void fnSendNextSPI_word(QUEUE_HANDLE channel, SPIQUE *ptSPIQue)   // interrupts are assumed to be disabled here
{
    if ((ptSPIQue->ucState & TX_WAIT) == 0) {                            // send the next byte if possible - either first char or tx interrupt
        if (ptSPIQue->spi_queue.chars == 0) {                            // are there more to send?
            ptSPIQue->ucState &= ~TX_ACTIVE;                             // transmission of a block has terminated
            fnClearSPITxInt(channel);                                    // clear interrupt
        }
        else {
            QUEQUE *ptrTxQUE = &(ptSPIQue->spi_queue);
            unsigned char ucChipSelect = ptSPIQue->ucChipSelect;
            unsigned short usNextByte = fnGetFromCircularBuffer(ptrTxQUE);
            if ((ptSPIQue->opn_mode & SPI_TX_MESSAGE_MODE) != 0) {       // if transmission is based on independent message blocks
                if (ptSPIQue->msgchars == 0) {                           // starting a new message
                    ptSPIQue->msgchars = (usNextByte | (fnGetFromCircularBuffer(ptrTxQUE) << 8)); // the number of words in the message to be sent
                    if ((ptSPIQue->opn_mode & SPI_TX_MULTI_MODE) != 0) { // if there are details of the chip select to be used
                        ptSPIQue->ucChipSelect = fnGetFromCircularBuffer(ptrTxQUE);
                        ptSPIQue->ucChipSelect |= (fnGetFromCircularBuffer(ptrTxQUE) << 8);
                        if (ptSPIQue->ucChipSelect >= SPI_CHIP_SELECTS) {
                            _EXCEPTION("Invalid chip select");
                        }
                    }
                    if (ptrTxQUE->chars == 0) {                          // allow preparing transmission details before adding the first message
                        return;                                          // message details ready but not yet active
                    }
                    ucChipSelect = ptSPIQue->ucChipSelect;;
                    ucChipSelect |= FIRST_SPI_MESSAGE_WORD;
                    usNextByte = fnGetFromCircularBuffer(ptrTxQUE);      // first data byte
                }
                else if ((ptSPIQue->ucState & TX_ACTIVE) == 0) {         // message being added after preparing its details
                    ucChipSelect = ptSPIQue->ucChipSelect;;
                    ucChipSelect |= FIRST_SPI_MESSAGE_WORD;
                }
                if (ptSPIQue->uBusWidth[ptSPIQue->ucChipSelect] > 8) {   // if the word is held in two bytes
                    usNextByte |= (fnGetFromCircularBuffer(ptrTxQUE) << 8); // 16 bit word
                }
                if (ptSPIQue->msgchars <= 1) {
                    ucChipSelect |= LAST_SPI_MESSAGE_WORD;
                    ptSPIQue->msgchars = 0;
                }
                else {
                    ptSPIQue->msgchars--;
                }
            }
            else {                                                       // non message oriented transmission
                if ((ptSPIQue->ucState & TX_ACTIVE) == 0) {
                    ucChipSelect |= FIRST_SPI_MESSAGE_WORD;
                }
                if (ptSPIQue->spi_queue.chars <= 1) {
                    ucChipSelect |= LAST_SPI_MESSAGE_WORD;
                }
            }
            fnTxSPIByte(channel, usNextByte, ucChipSelect);
            ptSPIQue->ucState |= TX_ACTIVE;                              // mark activity
#if defined WAKE_BLOCKED_TX
    #if defined SUPPORT_SPI_FLOW_HIGH_LOW
            fnWakeBlockedTx(ptSPIQue, tx_control[channel]->low_water_level);
    #else
            fnWakeBlockedTx(ptSPIQue, LOW_WATER_MARK);
    #endif
#endif
        }
    }
    else {
        fnClearSPITxInt(channel);                                        // clear interrupt since we are not allowed to send at the moment
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
    SPIQUE *rx_ctl = rx_control[Channel];
#if defined SPI_SUPPORT_DMA && defined SPI_SUPPORT_DMA_RX
    if ((rx_ctl->ucDMA_mode & UART_RX_DMA) != 0) {                       // new characters in the buffer - increment message count
        QUEUE_TRANSFER transfer_length = rx_ctl->spi_queue.buf_length;
    #if defined SERIAL_SUPPORT_RX_DMA_BREAK
        if ((UART_RX_DMA_BREAK & rx_ctl->ucDMA_mode) != 0) {             // break detected
            QUEUE_TRANSFER OriginalLength = rx_ctl->spi_queue.chars;
            fnPrepareRxDMA(Channel, (unsigned char *)(&(rx_ctl->spi_queue)), 0); // update the waiting data
            OriginalLength = (rx_ctl->spi_queue.chars - OriginalLength); // the additional characters collected since the previous break
            if (ch != 0) {                                               // first break after enabling the received so we ignore received data
                rx_ctl->spi_queue.get += rx_ctl->spi_queue.chars;
                if (rx_ctl->spi_queue.get >= rx_ctl->spi_queue.buffer_end) {
                    rx_ctl->spi_queue.get -= rx_ctl->spi_queue.buf_length;
                }
                rx_ctl->spi_queue.chars = 0;
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
        rx_ctl->spi_queue.put += transfer_length;
        rx_ctl->spi_queue.chars += transfer_length;                      // amount of characters that are ready to be read
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
            transfer_length -= 2;
    #else
            transfer_length -= 1;
    #endif
        }
        if (rx_ctl->spi_queue.put >= rx_ctl->spi_queue.buffer_end) {
            rx_ctl->spi_queue.put -= rx_ctl->spi_queue.buf_length;
        }
        fnPrepareRxDMA(Channel, rx_ctl->spi_queue.put, transfer_length); // enable next reception
        rx_ctl->msgs++;                                                  // we have 1 complete message
        if (rx_ctl->wake_task != 0) {                                    // wake up an input task, if defined
            uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);     // wake up service interface task
        }
        return;
    }
#endif
#if defined SUPPORT_MSG_MODE
    if ((SPI_TX_MESSAGE_MODE & rx_ctl->opn_mode) != 0) {
        if (rx_ctl->ucMessageTerminator == ch) {
            iBlockBuffer = 1;
        }
    }
#endif
    if (iBlockBuffer == 0) {
        if (rx_ctl->spi_queue.chars < rx_ctl->spi_queue.buf_length) {    // never overwrite contents of buffer
            rx_ctl->spi_queue.chars++;                                   // increment the total character count
#if (defined SUPPORT_MSG_CNT && defined SUPPORT_MSG_MODE) || defined UART_BREAK_SUPPORT
            rx_ctl->msgchars++;                                          // update the present message length
#endif
            *(rx_ctl->spi_queue.put) = ch;                               // put received character in to the input buffer
            if (++rx_ctl->spi_queue.put >= rx_ctl->spi_queue.buffer_end) { // wrap in circular buffer
                rx_ctl->spi_queue.put = rx_ctl->spi_queue.QUEbuffer;     // wrap to beginning
            }
        }
        if ((rx_ctl->opn_mode & (SPI_TX_MESSAGE_MODE | MSG_BREAK_MODE)) != 0) {
            return;                                                      // we don't wake until a complete message has been received
        }
    }
    else  {                                                              // end of message received
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
            if (rx_ctl->msgchars != 0) {                                 // if we have not actually collected any characters don't save a length
                rx_ctl->msgs += 2;                                       // a new message length and data message is ready
    #if defined MSG_CNT_WORD
                ptrBuffer = rx_ctl->spi_queue.put - rx_ctl->msgchars - 2;// move back to start of this message
    #else
                ptrBuffer = rx_ctl->spi_queue.put - rx_ctl->msgchars - 1;// move back to start of this message
    #endif
                if (++rx_ctl->spi_queue.put >= rx_ctl->spi_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                    rx_ctl->spi_queue.put = rx_ctl->spi_queue.QUEbuffer; // wrap to beginning
                }
    #if defined MSG_CNT_WORD
                if (++rx_ctl->spi_queue.put >= rx_ctl->spi_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                    rx_ctl->spi_queue.put = rx_ctl->spi_queue.QUEbuffer; // wrap to beginning
                }
    #endif
                if (ptrBuffer < rx_ctl->spi_queue.QUEbuffer) {           // set pointer to reserved message length position
                    ptrBuffer += rx_ctl->spi_queue.buf_length;
                }
    #if defined MSG_CNT_WORD
                *ptrBuffer = (unsigned char)(rx_ctl->msgchars>>8);
                if (++ptrBuffer >= rx_ctl->spi_queue.buffer_end) {       // handle wrap in circular buffer
                    ptrBuffer = rx_ctl->spi_queue.QUEbuffer;             // wrap to beginning
                }
                rx_ctl->spi_queue.chars++;                               // include the length of next message
    #endif
                *ptrBuffer = (unsigned char)rx_ctl->msgchars;            // write the length of the message at first position (doesn't include the terminator in this mode)
                rx_ctl->spi_queue.chars++;                               // include the length of next message
                rx_ctl->msgchars = 0;                                    // reset for next message
            }
        }
        else {
#endif
            rx_ctl->msgs++;                                              // we have 1 complete message
            if (rx_ctl->spi_queue.chars < rx_ctl->spi_queue.buf_length) {// never overwrite contents of buffer
                ++rx_ctl->spi_queue.chars;
                *(rx_ctl->spi_queue.put) = ch;                           // put received character in input buffer
                if (++rx_ctl->spi_queue.put >= rx_ctl->spi_queue.buffer_end) { // wrap in circular buffer
                    rx_ctl->spi_queue.put = rx_ctl->spi_queue.QUEbuffer; // wrap to beginning
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
extern void fnSPITxByte(QUEUE_HANDLE channel)
{
    fnSendNextSPI_word(channel, tx_control[channel]);
}

#endif

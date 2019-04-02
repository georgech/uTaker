/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_LPI2C.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    20.02.2018 Enable ignoring Nacks to allow compatible operation with other I2C controllers {1}
    24.03.2019 Add DMA mode and I2C_2_BYTE_LENGTH option

*/

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined I2C_DMA_SUPPORT                                              // DMA support on transmission
// DMA channel assignments for each LPI2C transmitter
//
static const unsigned char LPI2C_DMA_TX_CHANNEL[LPI2C_AVAILABLE] = {
    DMA_I2C0_TX_CHANNEL, 
    #if (LPI2C_AVAILABLE) > 1
    DMA_I2C1_TX_CHANNEL,
    #endif
    #if (LPI2C_AVAILABLE) > 2
    DMA_I2C2_TX_CHANNEL, 
    #endif
    #if (LPI2C_AVAILABLE) > 3
    DMA_I2C3_TX_CHANNEL,
    #endif
};

// DMA channel assignments for each LPI2C receiver
//
static const unsigned char LPI2C_DMA_RX_CHANNEL[LPI2C_AVAILABLE] = {
    DMA_I2C0_RX_CHANNEL,
    #if (LPI2C_AVAILABLE) > 1
    DMA_I2C1_RX_CHANNEL,
    #endif
    #if (LPI2C_AVAILABLE) > 2
    DMA_I2C2_RX_CHANNEL,
    #endif
    #if (LPI2C_AVAILABLE) > 3
    DMA_I2C3_RX_CHANNEL,
    #endif
};

// DMA channel interrupt priority assignments for each LPI2C transmitter
//
static const unsigned char LPI2C_DMA_TX_INT_PRIORITY[LPI2C_AVAILABLE] = {
    DMA_I2C0_TX_INT_PRIORITY, 
    #if (LPI2C_AVAILABLE) > 1
    DMA_I2C1_TX_INT_PRIORITY, 
    #endif
    #if (LPI2C_AVAILABLE) > 2
    DMA_I2C2_TX_INT_PRIORITY, 
    #endif
    #if (LPI2C_AVAILABLE) > 3
    DMA_I2C3_TX_INT_PRIORITY,
    #endif
};

// DMA channel interrupt priority assignments for each LPI2C receiver
//
static const unsigned char LPI2C_DMA_RX_INT_PRIORITY[LPI2C_AVAILABLE] = {
    DMA_I2C0_RX_INT_PRIORITY,
    #if (LPI2C_AVAILABLE) > 1
    DMA_I2C1_RX_INT_PRIORITY,
    #endif
    #if (LPI2C_AVAILABLE) > 2
    DMA_I2C2_RX_INT_PRIORITY,
    #endif
    #if (LPI2C_AVAILABLE) > 3
    DMA_I2C3_RX_INT_PRIORITY,
    #endif
};
#endif
/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnConfigLPI2C_pins(QUEUE_HANDLE Channel,  int iMaster);

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned char ucCheckTxI2C = 0;                                   // {2}
#if defined I2C_SLAVE_MODE
    static unsigned char *ptrMessage[LPI2C_AVAILABLE] = {0};
    static unsigned char ucMessageLength[LPI2C_AVAILABLE] = {0};
    static int (*fnI2C_SlaveCallback[LPI2C_AVAILABLE])(int iChannel, unsigned char *ptrDataByte, int iType) = {0};
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined I2C_INTERFACE
    extern I2CQue *I2C_rx_control[LPI2C_AVAILABLE];
    extern I2CQue *I2C_tx_control[LPI2C_AVAILABLE];
#endif

/* =================================================================== */
/*                        I2C Interrupt Handlers                       */
/* =================================================================== */


//#define MAX_EVENTS 200                                                 // comment this in to enable event logging
#if defined MAX_EVENTS
unsigned long ulTemp[MAX_EVENTS] = {0};
int iEventCounter = 0;

static void fnLogEvent(unsigned char ucLocation, unsigned long ulValue)
{
    if (iEventCounter < (MAX_EVENTS - 3)) {
        ulTemp[iEventCounter++] = ucLocation;
        ulTemp[iEventCounter++] = ulValue;
        ulTemp[iEventCounter++] = '-';
    }
}
#else
    #define fnLogEvent(x, y)                                             // dummy when logging is disabled
#endif

#if defined I2C_SLAVE_MODE
    #if defined I2C_SLAVE_RX_BUFFER
static int fnSaveRx(I2CQue *ptrRxControl, unsigned char ucRxByte)
{
    fnLogEvent('r', ucRxByte);
    if (ptrRxControl->I2C_queue.chars < ptrRxControl->I2C_queue.buf_length) { // ensure there is space in the input buffer
        *ptrRxControl->I2C_queue.put = ucRxByte;                         // save the received byte
        ptrRxControl->I2C_queue.chars++;                                 // increment the character count
        if (++(ptrRxControl->I2C_queue.put) >= ptrRxControl->I2C_queue.buffer_end) { // handle circular input buffer
            ptrRxControl->I2C_queue.put = ptrRxControl->I2C_queue.QUEbuffer;
        }
        return 0;                                                        // received byte was saved
    }
    else {
        return -1;                                                       // the received byte wasn't saved
    }
}
    #endif

static unsigned char fnGetTxByte(int iChannel, I2CQue *ptrTxControl, int iType)
{
    #if defined I2C_SLAVE_TX_BUFFER
    int iResult = I2C_SLAVE_BUFFER;
    #endif
    unsigned char ucByte = 0xff;                                         // default in case there is no defined data to return
    if (fnI2C_SlaveCallback[iChannel] != 0) {                            // if there is a callback
        #if defined I2C_SLAVE_TX_BUFFER
        iResult = fnI2C_SlaveCallback[iChannel](iChannel, &ucByte, iType);
        #else
        fnI2C_SlaveCallback[iChannel](iChannel, &ucByte, iType);
        #endif
    }
    #if defined I2C_SLAVE_TX_BUFFER
    if (I2C_SLAVE_BUFFER == iResult) {                                   // extract the data from the buffer
        if (ptrTxControl != 0) {
            if (ptrTxControl->I2C_queue.chars != 0) {                    // if there is data in the queue
                ucByte = *(ptrTxControl->I2C_queue.get);                 // the next byte to send is taken from the queue
                if (++(ptrTxControl->I2C_queue.get) >= ptrTxControl->I2C_queue.buffer_end) { // handle the circular buffer
                    ptrTxControl->I2C_queue.get = ptrTxControl->I2C_queue.QUEbuffer;
                }
                ptrTxControl->I2C_queue.chars--;
            }
        }
    }
    #endif
    return ucByte;                                                       // prepare the byte to be returned
}
#endif

#if defined TEMP_LPI2C_TEST
    unsigned long ulRxLPI2Cpause = 0;
    unsigned long ulTxLPI2Cpause = 0;
    unsigned long ulChange = 0;
    static unsigned long ulChars = LPI2C_CHARACTERISTICS;
    #define LPI2C_CHARACTERISTICS_ ulChars
#else
    #define LPI2C_CHARACTERISTICS_ LPI2C_CHARACTERISTICS
#endif

static void ReceptionComplete(I2CQue *ptrTxControl, KINETIS_LPI2C_CONTROL *ptrLPI2C, int iChannel)
{
    I2CQue *ptrRxControl = I2C_rx_control[iChannel];
    ptrRxControl->msgs++;                                    // reception message available
    if (ptrRxControl->wake_task != 0) {                      // wake up the receiver task if desired
        uTaskerStateChange(ptrRxControl->wake_task, UTASKER_ACTIVATE); // wake up owner task
    }
    if (ptrTxControl->I2C_queue.chars != 0) {                // reception has completed but we have further queued data
        fnLogEvent('P', 0);
        fnTxI2C(ptrTxControl, iChannel);                     // we have another message to send so we can send a repeated start condition
    }
    else {
        ptrLPI2C->LPI2C_MIER = LPI2C_MIER_SDIE;              // disable reception interrupt and enable stop detect interrupt
        ptrLPI2C->LPI2C_MTDR = LPI2C_MTDR_CMD_STOP;          // send stop condition
#if defined _WINDOWS
        ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_SDF;                // simulate the interrupt directly
        iInts |= (I2C_INT0 << iChannel);                     // signal that an interrupt is to be generated
#endif
    }
}

static void fnLPI2C_Handler(KINETIS_LPI2C_CONTROL *ptrLPI2C, int iChannel) // general LPI2C interrupt handler
{
    I2CQue *ptrTxControl = I2C_tx_control[iChannel];
#if defined _WINDOWS
    int irq_id = (irq_LPI2C0_ID + iChannel);
    #if LPI2C_AVAILABLE > 2
    if (iChannel == 2) {
        irq_id = (irq_INTMUX0_0_ID + INPUT_TO_INTMUX0_CHANNEL_2);
    }
    #endif
#endif

    if (((ptrLPI2C->LPI2C_MIER & ptrLPI2C->LPI2C_MSR) & LPI2C_MSR_SDF) != 0) { // stop condition has completed
        fnLogEvent('S', ptrLPI2C->LPI2C_MSR);
        ptrLPI2C->LPI2C_MIER = 0;                                        // disable all interrupt sources
        WRITE_ONE_TO_CLEAR_INTERRUPT(ptrLPI2C->LPI2C_MSR, LPI2C_MSR_SDF, irq_id); // clear interrupt flag
        if ((ptrTxControl->ucState & RX_ACTIVE) != 0) {
          //I2CQue *ptrRxControl = I2C_rx_control[iChannel];
          //ptrRxControl->msgs++;
          //if (ptrRxControl->wake_task != 0) {                          // wake up the receiver task if desired
          //    uTaskerStateChange(ptrRxControl->wake_task, UTASKER_ACTIVATE); // wake up owner task
          //}
        }
        else {
            if (ptrTxControl->wake_task != 0) {
                uTaskerStateChange(ptrTxControl->wake_task, UTASKER_ACTIVATE); // wake up owner task since the transmission has terminated
            }
        }
        ptrTxControl->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);     // interface is idle
    #if defined TEMP_LPI2C_TEST
        if (ulChange != 0) {
            ptrLPI2C->LPI2C_MCR &= ~(LPI2C_MCR_MEN);
            if (ulChange & 0x01) {
                ptrLPI2C->LPI2C_MCFGR1 |= (LPI2C_MCFGR1_AUTOSTOP);
            }
            else if (ulChange & 0x02) {
                ptrLPI2C->LPI2C_MCFGR1 &= ~(LPI2C_MCFGR1_AUTOSTOP);
            }
            else if (ulChange & 0x04) {
                ulChars |= LPI2C_MCR_DOZEN;
                ptrLPI2C->LPI2C_MCR |= (LPI2C_MCR_DOZEN);
            }
            else if (ulChange & 0x08) {
                ulChars &= ~LPI2C_MCR_DOZEN;
                ptrLPI2C->LPI2C_MCR &= ~(LPI2C_MCR_DOZEN);
            }
            else if (ulChange & 0x10) {
                ulChars |= LPI2C_MCR_DBGEN;
                ptrLPI2C->LPI2C_MCR |= (LPI2C_MCR_DBGEN);
            }
            else if (ulChange & 0x20) {
                ulChars &= ~LPI2C_MCR_DBGEN;
                ptrLPI2C->LPI2C_MCR &= ~(LPI2C_MCR_DBGEN);
            }
            ulChange = 0;
            ptrLPI2C->LPI2C_MCR |= (LPI2C_MCR_MEN);
        }
    #endif
        if (ptrTxControl->I2C_queue.chars != 0) {                        // if there is further data waiting we can send it now
            fnLogEvent('n', ptrTxControl->I2C_queue.chars);
            fnTxI2C(ptrTxControl, iChannel);                             // we have another message to send so we can send a repeated start condition
        }
        return;
    }

    if ((ptrTxControl->ucState & RX_ACTIVE) != 0) {                      // if the master is reading from a slave
        I2CQue *ptrRxControl = I2C_rx_control[iChannel];
        fnLogEvent('R', ptrLPI2C->LPI2C_MSR);
        if ((ptrTxControl->ucState & TX_ACTIVE) != 0) {                  // the address transmission has completed
            fnLogEvent('A', ptrLPI2C->LPI2C_MRDR);
            (void)(ptrLPI2C->LPI2C_MRDR);                                // dummy read to clear address
            ptrLPI2C->LPI2C_MCR = (LPI2C_MCR_RTF | LPI2C_MCR_MEN | LPI2C_CHARACTERISTICS_); // ensure receive FIFO is reset
#if defined I2C_2_BYTE_LENGTH
            ptrLPI2C->LPI2C_MTDR = (LPI2C_MTDR_CMD_RX_DATA | (unsigned char)(ptrTxControl->usPresentLen - 1)); // command the read sequence
#else
            ptrLPI2C->LPI2C_MTDR = (LPI2C_MTDR_CMD_RX_DATA | (ptrTxControl->ucPresentLen - 1)); // command the read sequence
#endif
            ptrLPI2C->LPI2C_MIER = LPI2C_MIER_RDIE;                      // enable interrupt on reception available and disable transmission interrupt
            ptrTxControl->ucState = (RX_ACTIVE);                         // signal that the address has been sent and we are now receiving
#if defined _WINDOWS
            ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_RDF;
            iInts |= (I2C_INT0 << iChannel);
#endif
        }
        else {                                                           // reception character available
#if defined _WINDOWS
            if ((LPI2C2_MDER & LPI2C_MDER_RDDE) == 0) {                  // if not receiving with DMA
                if ((ptrLPI2C->LPI2C_MTDR & LPI2C_MTDR_DATA_MASK) != 0) {// this is a write-only register but the simulator uses it to count down the number of reception bytes that was set
                    ptrLPI2C->LPI2C_MTDR--;
                    ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_RDF;
                    iInts |= (I2C_INT0 << iChannel);
                }
                else {
                    ptrLPI2C->LPI2C_MSR &= ~LPI2C_MSR_RDF;               // final reception was received so remove the reception ready flag
                }
                ptrLPI2C->LPI2C_MRDR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)(ptrLPI2C->LPI2C_MRDR));
            }
#endif
            *ptrRxControl->I2C_queue.put = (unsigned char)(ptrLPI2C->LPI2C_MRDR); // read the byte
            fnLogEvent('r', *ptrRxControl->I2C_queue.put);
            ptrRxControl->I2C_queue.chars++;                             // and put it into the rx buffer
            if (++(ptrRxControl->I2C_queue.put) >= ptrRxControl->I2C_queue.buffer_end) { // handle circular input buffer
                ptrRxControl->I2C_queue.put = ptrRxControl->I2C_queue.QUEbuffer;
            }
#if defined I2C_2_BYTE_LENGTH
            if (--(ptrTxControl->usPresentLen) == 0)
#else
            if (--(ptrTxControl->ucPresentLen) == 0)
#endif
            {
                ReceptionComplete(ptrTxControl, ptrLPI2C, iChannel);
            }
        }
        return;
    }
    else
#if defined I2C_2_BYTE_LENGTH
    if (ptrTxControl->usPresentLen-- != 0)                               // TX_ACTIVE - send next byte if available
#else
    if (ptrTxControl->ucPresentLen-- != 0)                               // TX_ACTIVE - send next byte if available
#endif
    {
        fnLogEvent('U', ptrLPI2C->LPI2C_MSR);
        fnLogEvent('X', *ptrTxControl->I2C_queue.get);
        ptrLPI2C->LPI2C_MTDR = *ptrTxControl->I2C_queue.get++;           // send the next byte (note that the command byte is 0, meaning just send byte)
        if (ptrTxControl->I2C_queue.get >= ptrTxControl->I2C_queue.buffer_end) { // handle the circular transmit buffer
            ptrTxControl->I2C_queue.get = ptrTxControl->I2C_queue.QUEbuffer;
        }
        #if defined _WINDOWS
        ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_TDF;                            // simulate the interrupt directly
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)(ptrLPI2C->LPI2C_MTDR));
        iInts |= (I2C_INT0 << iChannel);                                 // signal that an interrupt is to be generated
        #endif
    }
    else {                                                               // last byte in TX_ACTIVE
        if (ptrTxControl->I2C_queue.chars == 0) {                        // transmission complete
            ptrLPI2C->LPI2C_MIER = LPI2C_MIER_SDIE;                      // disable transmit interrupt and enable stop detect interrupt
            ptrLPI2C->LPI2C_MTDR = LPI2C_MTDR_CMD_STOP;                  // send stop condition
            fnLogEvent('E', ptrLPI2C->LPI2C_MSR);
        #if defined _WINDOWS
            ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_SDF;                        // simulate the interrupt directly
            iInts |= (I2C_INT0 << iChannel);                             // signal that an interrupt is to be generated
        #endif
        }
        else {
            fnLogEvent('p', 0);
            fnTxI2C(ptrTxControl, iChannel);                             // we have another message to send so we can send a repeated start condition
        }
    }
}

static __interrupt void _I2C_Interrupt_0(void)                           // LPI2C0 interrupt
{
    fnLPI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK, 0);
}

    #if LPI2C_AVAILABLE > 1
static __interrupt void _I2C_Interrupt_1(void)                           // LPI2C1 interrupt
{
    fnLPI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK, 1);
}
    #endif

    #if LPI2C_AVAILABLE > 2
static __interrupt void _I2C_Interrupt_2(void)                           // LPI2C2 interrupt
{
    fnLPI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK, 2);
}
    #endif

    #if LPI2C_AVAILABLE > 3
static __interrupt void _I2C_Interrupt_3(void)                           // LPI2C3 interrupt
{
    fnLPI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C3_BLOCK, 3);
}
    #endif


#if defined I2C_DMA_SUPPORT                                              // DMA support
static void fnStartLPI2C_TxDMA(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    QUEUE_TRANSFER max_linear_data = (ptI2CQue->I2C_queue.buffer_end - ptI2CQue->I2C_queue.get);
    #if defined I2C_2_BYTE_LENGTH
    ptI2CQue->dma_length = ptI2CQue->usPresentLen;
    #else
    ptI2CQue->dma_length = ptI2CQue->ucPresentLen;
    #endif
    if (ptI2CQue->dma_length > max_linear_data) {
    #if defined I2C_2_BYTE_LENGTH
        ptI2CQue->dma_length = (unsigned short)max_linear_data;
    #else
        ptI2CQue->dma_length = (unsigned char)max_linear_data;
    #endif
    }
    ptrDMA_TCD += LPI2C_DMA_TX_CHANNEL[Channel];
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = ptI2CQue->dma_length; // the number of service requests (the number of bytes to be transferred)
    ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)ptI2CQue->I2C_queue.get;  // source is I2C output buffer's present location
    #if defined SUPPORT_LOW_POWER
    PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (I2C0_CLK_REQUIRED << Channel)); // mark that stop mode should be avoided until the I2C activity has completed
    #endif
    ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, LPI2C_DMA_TX_CHANNEL[Channel]); // enable request source in order to start DMA activity
    #if defined _WINDOWS                                                 // simulation
    iDMA |= (DMA_CONTROLLER_0 << LPI2C_DMA_TX_CHANNEL[Channel]);         // activate first DMA request
    #endif
}

static void fnStartLPI2C_RxDMA(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    QUEQUE *ptrRxQueue = &I2C_rx_control[Channel]->I2C_queue;
    QUEUE_TRANSFER max_linear_data = (ptrRxQueue->buffer_end - ptrRxQueue->put);
    #if defined I2C_2_BYTE_LENGTH
    ptI2CQue->dma_length = ptI2CQue->usPresentLen;
    #else
    ptI2CQue->dma_length = ptI2CQue->ucPresentLen;
    #endif
    if (ptI2CQue->dma_length > max_linear_data) {
    #if defined I2C_2_BYTE_LENGTH
        ptI2CQue->dma_length = (unsigned short)max_linear_data;
    #else
        ptI2CQue->dma_length = (unsigned char)max_linear_data;
    #endif
    }
    ptrDMA_TCD += LPI2C_DMA_RX_CHANNEL[Channel];
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = ptI2CQue->dma_length; // the number of service requests (the number of bytes to be transferred)
    ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptrRxQueue->put;          // destination is I2C input buffer's present location
    #if defined SUPPORT_LOW_POWER
    PROTECTED_SET_VARIABLE(ulPeripheralNeedsClock, (I2C0_CLK_REQUIRED << Channel)); // mark that stop mode should be avoided until the I2C activity has completed
    #endif
    ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, LPI2C_DMA_RX_CHANNEL[Channel]); // enable request source in order to start DMA activity
    #if defined _WINDOWS                                                 // simulation
    iDMA |= (DMA_CONTROLLER_0 << LPI2C_DMA_RX_CHANNEL[Channel]);         // activate first DMA request
    #endif
}

// Common DMA tx interrupt handler
//
static void _lpi2c_tx_dma_Interrupt(KINETIS_LPI2C_CONTROL *ptrLPI2C, int iChannel)
{
    I2CQue *ptrTxControl = I2C_tx_control[iChannel];
        #if defined _WINDOWS
    fnSimI2C_devices(I2C_TX_DATA, (unsigned char)(ptrLPI2C->LPI2C_MTDR));
        #endif
    #if defined I2C_2_BYTE_LENGTH
    ptrTxControl->usPresentLen -= ptrTxControl->dma_length;
    #else
    ptrTxControl->ucPresentLen -= ptrTxControl->dma_length;
    #endif
    ptrTxControl->I2C_queue.get += ptrTxControl->dma_length;
    if (ptrTxControl->I2C_queue.get >= ptrTxControl->I2C_queue.buffer_end) {
        ptrTxControl->I2C_queue.get -= ptrTxControl->I2C_queue.buf_length;
    }
    #if defined I2C_2_BYTE_LENGTH
    if (ptrTxControl->usPresentLen == 0)
    #else
    if (ptrTxControl->ucPresentLen == 0)
    #endif
    {                                                                    // if all data transmitted
        fnLPI2C_Handler(ptrLPI2C, iChannel);
    }
    else {                                                               // transmit remaining (after circular buffer wrap-around)
        fnStartLPI2C_TxDMA(ptrTxControl, iChannel);
        #if defined _WINDOWS
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)(ptrLPI2C->LPI2C_MTDR));
        #endif
    }    
}
static void _lpi2c0_tx_dma_Interrupt(void)
{
    _lpi2c_tx_dma_Interrupt((KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK, 0);
}

    #if LPI2C_AVAILABLE > 1
static void _lpi2c1_tx_dma_Interrupt(void)
{
    _lpi2c_tx_dma_Interrupt((KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK, 1);
}
    #endif
    #if LPI2C_AVAILABLE > 2
static void _lpi2c2_tx_dma_Interrupt(void)
{
    _lpi2c_tx_dma_Interrupt((KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK, 2);
}
    #endif

// Common DMA tx interrupt handler
//
static void _lpi2c_rx_dma_Interrupt(KINETIS_LPI2C_CONTROL *ptrLPI2C, int iChannel)
{
    I2CQue *ptrTxControl = I2C_tx_control[iChannel];
    I2CQue *ptrRxControl = I2C_rx_control[iChannel];
    #if defined I2C_2_BYTE_LENGTH
    ptrTxControl->usPresentLen -= ptrTxControl->dma_length;
    #else
    ptrTxControl->ucPresentLen -= ptrTxControl->dma_length;
    #endif
    ptrRxControl->I2C_queue.put += ptrRxControl->dma_length;
    if (ptrRxControl->I2C_queue.put >= ptrRxControl->I2C_queue.buffer_end) {
        ptrRxControl->I2C_queue.put -= ptrRxControl->I2C_queue.buf_length;
    }
    ptrRxControl->I2C_queue.chars += ptrTxControl->dma_length;
    #if defined I2C_2_BYTE_LENGTH
    if (ptrTxControl->usPresentLen == 0)
    #else
    if (ptrTxControl->ucPresentLen == 0)
    #endif
    {                                                                    // if all data received
        ReceptionComplete(ptrTxControl, ptrLPI2C, iChannel);
    }
    else {                                                               // receive remaining (after circular buffer wrap-around)
        fnStartLPI2C_RxDMA(ptrTxControl, iChannel);
    }
}


static void _lpi2c0_rx_dma_Interrupt(void)
{
    _lpi2c_rx_dma_Interrupt((KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK, 0);
}

    #if LPI2C_AVAILABLE > 1
static void _lpi2c1_rx_dma_Interrupt(void)
{
    _lpi2c_rx_dma_Interrupt((KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK, 1);
}
    #endif
    #if LPI2C_AVAILABLE > 2
static void _lpi2c2_rx_dma_Interrupt(void)
{
    _lpi2c_rx_dma_Interrupt((KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK, 2);
}
    #endif

static void (*_lpi2c_tx_dma_Interrupts[LPI2C_AVAILABLE])(void) = {
    _lpi2c0_tx_dma_Interrupt,
    #if LPI2C_AVAILABLE > 1
    _lpi2c1_tx_dma_Interrupt,
    #endif
    #if LPI2C_AVAILABLE > 2
    _lpi2c2_tx_dma_Interrupt,
    #endif
};

static void(*_lpi2c_rx_dma_Interrupts[LPI2C_AVAILABLE])(void) = {
    _lpi2c0_rx_dma_Interrupt,
    #if LPI2C_AVAILABLE > 1
    _lpi2c1_rx_dma_Interrupt,
    #endif
    #if LPI2C_AVAILABLE > 2
    _lpi2c2_rx_dma_Interrupt,
    #endif
};
#endif

/* =================================================================== */
/*                   I2C Transmission Initiation                       */
/* =================================================================== */


// Send a first byte to I2C bus
//
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    KINETIS_LPI2C_CONTROL *ptrLPI2C;
    unsigned char ucAddress;
    #if LPI2C_AVAILABLE > 1
    if (Channel == 1) {
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK;
    }
        #if LPI2C_AVAILABLE > 2
    else if (Channel == 2) {
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK;
    }
        #endif
        #if LPI2C_AVAILABLE > 3
    else if (Channel == 3) {
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C3_BLOCK;
    }
        #endif
    else {
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK;
    }
    #else
    ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK;
    #endif
    #if defined I2C_2_BYTE_LENGTH
    ptI2CQue->usPresentLen = *ptI2CQue->I2C_queue.get++;                 // get the length of this transaction (MSB)
    ptI2CQue->usPresentLen <<= 8;
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {     // handle circular buffer
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;
    }
    ptI2CQue->usPresentLen |= *ptI2CQue->I2C_queue.get++;                // length LSB
    #else
    ptI2CQue->ucPresentLen = *ptI2CQue->I2C_queue.get++;                 // get the length of this transaction
    #endif
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {     // handle circular buffer
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;
    }

    if ((ptI2CQue->ucState & (TX_ACTIVE | RX_ACTIVE)) != 0) {            // restart since we are hanging a second telegram on to previous one
        fnLogEvent('*', ptrLPI2C->LPI2C_MSR);
    }
    else {                                                               // address to be sent on an idle bus
        if ((ucCheckTxI2C & (1 << Channel)) == 0) {                      // on first use we check that the bus is not held in a busy state (can happen when a reset took place during an acknowledge period and the slave is holding the bus)
            ucCheckTxI2C |= (1 << Channel);                              // checked only once
            uEnable_Interrupt();
                fnConfigLPI2C_pins(Channel, 1);                          // check and configure pins for LPI2C use
            uDisable_Interrupt();
        }
        fnLogEvent('B', ptrLPI2C->LPI2C_MSR);
        while ((ptrLPI2C->LPI2C_MSR & LPI2C_MSR_BBF) != 0) {             // wait until the stop has actually been sent to avoid a further transmission being started in the mean time
        }
        fnLogEvent('b', ptrLPI2C->LPI2C_MSR);
    }
    // Generate a start condition/repeated start and send the address
    //
    ucAddress = *ptI2CQue->I2C_queue.get++;                              // get the slave address
    ptrLPI2C->LPI2C_MTDR = (LPI2C_MTDR_CMD_START_DATA | ucAddress);      // generate a start or repeated start and send the slave address (this includes the rd/wr bit)
    #if defined TEMP_LPI2C_TEST
    if ((ulRxLPI2Cpause != 0) && ((ucAddress & 0x01) != 0)) {
        fnDelayLoop(ulRxLPI2Cpause);
    }
    else if ((ulTxLPI2Cpause != 0) && ((ucAddress & 0x01) == 0)) {
        fnDelayLoop(ulTxLPI2Cpause);
    }
    #endif
    fnLogEvent('?', ucAddress);
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {     // handle circular buffer
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;
    }
    if ((ucAddress & 0x01) != 0) {                                       // reading from the slave
        I2C_tx_control[Channel]->ucState |= (RX_ACTIVE | TX_ACTIVE);
    #if defined I2C_2_BYTE_LENGTH
        ptI2CQue->I2C_queue.chars -= 4;
    #else
        ptI2CQue->I2C_queue.chars -= 3;
    #endif
        fnLogEvent('g', (unsigned char)(ptI2CQue->I2C_queue.chars));
        I2C_rx_control[Channel]->wake_task = *ptI2CQue->I2C_queue.get++; // enter task to be woken when reception has completed
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
    #if defined I2C_DMA_SUPPORT
        if ((ptrLPI2C->LPI2C_MDER & LPI2C_MDER_RDDE) != 0) {             // if using DMA for reception
            ptrLPI2C->LPI2C_MIER = 0;                                    // disable interrupts
            fnStartLPI2C_RxDMA(ptI2CQue, Channel);
        #if defined _WINDOWS
            fnSimI2C_devices(I2C_ADDRESS, (unsigned char)(ptrLPI2C->LPI2C_MTDR));
        #endif
            ptrLPI2C->LPI2C_MTDR = (LPI2C_MTDR_CMD_RX_DATA | (ptI2CQue->dma_length - 1)); // command the read sequence
            return;
        }
    #endif
    }
    else {
        I2C_tx_control[Channel]->ucState &= ~(RX_ACTIVE);
        I2C_tx_control[Channel]->ucState |= (TX_ACTIVE);                 // writing to the slave
    #if defined I2C_2_BYTE_LENGTH
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->usPresentLen + 1);       // the remaining queue content after this transaction completes
    #else
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->ucPresentLen + 1);       // the remaining queue content after this transaction completes
    #endif
        fnLogEvent('h', (unsigned char)(ptI2CQue->I2C_queue.chars));
    #if defined I2C_DMA_SUPPORT
        if ((ptrLPI2C->LPI2C_MDER & LPI2C_MDER_TDDE) != 0) {             // if using DMA for transmission
            fnStartLPI2C_TxDMA(ptI2CQue, Channel);
        #if defined _WINDOWS
            fnSimI2C_devices(I2C_ADDRESS, (unsigned char)(ptrLPI2C->LPI2C_MTDR));
        #endif
            return;
        }
    #endif
    }
    ptrLPI2C->LPI2C_MIER = (LPI2C_MIER_TDIE);                            // enable interrupt on transmission completion
    fnLogEvent('T', ptrLPI2C->LPI2C_MSR);
    #if defined _WINDOWS
    ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_TDF;                                // simulate the interrupt directly
    fnSimI2C_devices(I2C_ADDRESS, (unsigned char)(ptrLPI2C->LPI2C_MTDR));
    iInts |= (I2C_INT0 << Channel);                                      // signal that an interrupt is to be generated
    #endif
}



/* =================================================================== */
/*                         I2C Configuration                           */
/* =================================================================== */

// Initially the I2C pins were configured as inputs to allow an I2C bus lockup state to be detected
// - this routine checks for this state and generates clocks if needed to recover from it before setting I2C pin functions
//
static void fnConfigLPI2C_pins(QUEUE_HANDLE Channel, int iMaster)        // {2}
{
    if (Channel == 0)  {                                                 // LPI2C0
    #if defined KINETIS_KE15 && defined I2C0_ON_B
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT6) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT7, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT7, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 6, (PB_6_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PB6 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 7, (PB_7_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PB7 (alt. function 2)
    #elif defined KINETIS_KE15
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT2) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT3, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT3, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 2, (PA_2_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PA2 (alt. function 3)
        _CONFIG_PERIPHERAL(A, 3, (PA_3_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PA3 (alt. function 3)
    #elif defined I2C0_E_LOW
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT18) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, PORTE_BIT19, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_HIGH(E, PORTE_BIT19, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 18, (PE_18_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PE18 (alt. function 4)
        _CONFIG_PERIPHERAL(E, 19, (PE_19_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PE19 (alt. function 4)
    #elif defined I2C0_B_LOW
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT1) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT0, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT__FAST_LOW(B, PORTB_BIT0, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 1, (PB_1_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PB1 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 0, (PB_0_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PB0 (alt. function 2)
    #elif defined I2C0_B_HIGH
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT3) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT2, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT2, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 3, (PB_3_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PB3 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 2, (PB_2_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PB2 (alt. function 2)
    #elif defined I2C0_ON_C
        if (iMaster != 0) {
            while (_READ_PORT_MASK(C, PORTC_BIT9) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, PORTC_BIT8, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(C, PORTC_BIT8, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(C, 9, (PC_9_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PC9 (alt. function 2)
        _CONFIG_PERIPHERAL(C, 8, (PC_8_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PC8 (alt. function 2)
    #elif defined I2C0_D_LOW
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT9) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE__FAST_LOW(D, PORTD_BIT8, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT__FAST_LOW(D, PORTD_BIT8, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 9, (PD_9_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PD9 (alt. function 2)
        _CONFIG_PERIPHERAL(D, 8, (PD_8_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PD8 (alt. function 2)
    #elif defined I2C0_D_HIGH
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT11) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE__FAST_LOW(D, PORTD_BIT10, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT__FAST_LOW(D, PORTD_BIT10, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 11, (PD_11_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PD11 (alt. function 3)
        _CONFIG_PERIPHERAL(D, 10, (PD_10_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PD10 (alt. function 3)
    #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT25) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, PORTE_BIT24, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_HIGH(E, PORTE_BIT24, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 25, (PE_25_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SDA on PE25 (alt. function 5)
        _CONFIG_PERIPHERAL(E, 24, (PE_24_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C0_SCL on PE24 (alt. function 5)
    #endif
    }
    #if LPI2C_AVAILABLE > 1
    else if (Channel == 1)  {                                            // LPI2C1
        #if defined KINETIS_KE15 && defined I2C1_ON_D                    // initially configure as inputs with pull-up
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT8) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE__FAST_LOW(D, PORTD_BIT9, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT__FAST_LOW(D, PORTD_BIT9, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 8, (PD_8_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SDA on PD8 (alt. function 2)
        _CONFIG_PERIPHERAL(D, 9, (PD_9_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SCL on PD9 (alt. function 2)
        #elif defined KINETIS_KE15
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT0) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT1, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(E, PORTE_BIT1, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 0, (PE_0_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SDA on PE0 (alt. function 6)
        _CONFIG_PERIPHERAL(E, 1, (PE_1_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SCL on PE1 (alt. function 6)
        #elif defined I2C1_ON_A
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT4) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT3, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT3, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 4, (PA_4_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SDA on PA4 (alt. function 3)
        _CONFIG_PERIPHERAL(A, 3, (PA_3_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SCL on PA3 (alt. function 3)
        #elif defined I2C1_C_LOW
        if (iMaster != 0) {
            while (_READ_PORT_MASK(C, PORTC_BIT2) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, PORTC_BIT1, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(C, PORTC_BIT1, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(C, 2, (PC_2_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SDA on PC2 (alt. function 2)
        _CONFIG_PERIPHERAL(C, 1, (PC_1_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SCL on PC1 (alt. function 2)
        #elif defined I2C1_C_HIGH
        if (iMaster != 0) {
            while (_READ_PORT_MASK(C, PORTC_BIT11) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, PORTC_BIT10, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(C, PORTC_BIT10, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(C, 11, (PC_11_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SDA on PC9 (alt. function 2)
        _CONFIG_PERIPHERAL(C, 10, (PC_10_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SCL on PC8 (alt. function 2)
        #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT0) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT1, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(E, PORTE_BIT1, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 0, (PE_0_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SDA on PE0 (alt. function 6)
        _CONFIG_PERIPHERAL(E, 1, (PE_1_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C1_SCL on PE1 (alt. function 6)
        #endif
    }
    #endif
    #if LPI2C_AVAILABLE > 2
    else if (Channel == 2) {                                             // LPI2C2
        #if defined I2C2_A_HIGH
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT13) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT14, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT14, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 13, (PA_13_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C2_SDA on PA13 (alt. function 5)
        _CONFIG_PERIPHERAL(A, 14, (PA_14_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C2_SCL on PA14 (alt. function 5)
        #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT11) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT12, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT12, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 11, (PA_11_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C2_SDA on PA11 (alt. function 5)
        _CONFIG_PERIPHERAL(A, 12, (PA_12_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // LPI2C2_SCL on PA12 (alt. function 5)
        #endif
    }
    #endif
}

// Configure the LPI2C hardware
//
extern void fnConfigI2C(I2CTABLE *pars)
{
    KINETIS_LPI2C_CONTROL *ptrLPI2C;
    #if defined I2C_DMA_SUPPORT
    unsigned short usTriggerSource = 0;
    #endif
    if (pars->Channel == 0) {                                            // LPI2C channel 0
    #if defined KINETIS_WITH_PCC
        SELECT_PCC_PERIPHERAL_SOURCE(LPI2C0, LPI2C0_PCC_SOURCE);         // select the PCC clock used by LPI2C0
    #endif
        POWER_UP_ATOMIC(4, I2C0);                                        // enable clock to module
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK;
        LPI2C0_MCR = (LPI2C_MCR_RST | LPI2C_MCR_RTF | LPI2C_MCR_RRF);    // reset LPI2C controller and ensure FIFOs are reset
    #if defined irq_LPI2C0_ID
        fnEnterInterrupt(irq_LPI2C0_ID, PRIORITY_I2C0, _I2C_Interrupt_0); // enter I2C0 interrupt handler
    #else
        fnEnterInterrupt(irq_I2C0_ID, PRIORITY_I2C0, _I2C_Interrupt_0);  // enter I2C0 interrupt handler
    #endif
    #if defined KINETIS_KE15 && defined I2C0_ON_B                        // initially configure as inputs with pull-up
        _CONFIG_PORT_INPUT(B, (PORTB_BIT6 | PORTB_BIT7), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined KINETIS_KE15
        _CONFIG_PORT_INPUT(A, (PORTA_BIT2 | PORTA_BIT3), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_E_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(E, (PORTE_BIT18 | PORTE_BIT19), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_B_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT1 | PORTB_BIT0), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_B_HIGH
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT3 | PORTB_BIT2), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_ON_C
        _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT9 | PORTC_BIT8), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_D_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(D, (PORTD_BIT9 | PORTD_BIT8), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_D_HIGH
        _CONFIG_PORT_INPUT_FAST_LOW(D, (PORTD_BIT11 | PORTD_BIT10), (PORT_ODE | PORT_PS_UP_ENABLE));
    #else
        _CONFIG_PORT_INPUT_FAST_HIGH(E, (PORTE_BIT25 | PORTE_BIT24), (PORT_ODE | PORT_PS_UP_ENABLE));
    #endif
    #if defined I2C_DMA_SUPPORT
        usTriggerSource = DMAMUX0_CHCFG_SOURCE_LPI2C0_RX;
    #endif
    }
    #if LPI2C_AVAILABLE > 1
    else if (pars->Channel == 1) {                                       // LPI2C channel 1
        #if defined KINETIS_WITH_PCC
        SELECT_PCC_PERIPHERAL_SOURCE(LPI2C1, LPI2C1_PCC_SOURCE);         // select the PCC clock used by LPI2C1
        #endif
        POWER_UP_ATOMIC(4, I2C1);                                        // enable clock to module
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK;
        LPI2C1_MCR = (LPI2C_MCR_RST | LPI2C_MCR_RTF | LPI2C_MCR_RRF);    // reset LPI2C controller and ensure FIFOs are reset
        #if defined irq_LPI2C1_ID
        fnEnterInterrupt(irq_LPI2C1_ID, PRIORITY_I2C1, _I2C_Interrupt_1);// enter LPI2C1 interrupt handler
        #elif !defined irq_I2C1_ID && defined INTMUX0_AVAILABLE
        fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_I2C1), INTMUX0_PERIPHERAL_I2C1, _I2C_Interrupt_1); // enter I2C1 interrupt handler based on INTMUX
        #else
        fnEnterInterrupt(irq_I2C1_ID, PRIORITY_I2C1, _I2C_Interrupt_1);  // enter I2C1 interrupt handler
        #endif
        #if defined KINETIS_KE15 && defined I2C1_ON_D                    // initially configure as inputs with pull-up
        _CONFIG_PORT_INPUT(D, (PORTD_BIT8 | PORTD_BIT9), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined KINETIS_KE15
        _CONFIG_PORT_INPUT(E, (PORTE_BIT0 | PORTE_BIT1), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined I2C1_ON_A
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT4 | PORTA_BIT3), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined I2C1_C_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT2 | PORTC_BIT1), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined I2C1_C_HIGH
        _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT11 | PORTC_BIT10), (PORT_ODE | PORT_PS_UP_ENABLE));
        #else
        _CONFIG_PORT_INPUT_FAST_HIGH(E, (PORTE_BIT0 | PORTE_BIT1), (PORT_ODE | PORT_PS_UP_ENABLE));
        #endif
        #if defined I2C_DMA_SUPPORT
        usTriggerSource = DMAMUX0_CHCFG_SOURCE_LPI2C1_RX;
        #endif
    }
    #endif
    #if LPI2C_AVAILABLE > 2
    else if (pars->Channel == 2) {                                       // LPI2C channel 2
        #if defined KINETIS_WITH_PCC
        SELECT_PCC_PERIPHERAL_SOURCE(LPI2C2, LPI2C2_PCC_SOURCE);         // select the PCC clock used by LPI2C1
        #endif
        POWER_UP_ATOMIC(1, I2C2);                                        // enable clock to module
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK;
        LPI2C2_MCR = (LPI2C_MCR_RST | LPI2C_MCR_RTF | LPI2C_MCR_RRF);    // reset LPI2C controller and ensure FIFOs are reset
        #if !defined irq_I2C2_ID && defined INTMUX0_AVAILABLE
        fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_LPI2C2), INTMUX0_PERIPHERAL_I2C2, _I2C_Interrupt_2); // enter I2C2 interrupt handler
        #else
        fnEnterInterrupt(irq_I2C2_ID, PRIORITY_I2C2, _I2C_Interrupt_2);  // enter I2C2 interrupt handler
        #endif
        #if defined I2C2_A_HIGH
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT13 | PORTA_BIT14), (PORT_ODE | PORT_PS_UP_ENABLE));
        #else
        _CONFIG_PORT_INPUT_FAST_HIGH(A, (PORTA_BIT11 | PORTA_BIT12), (PORT_ODE | PORT_PS_UP_ENABLE));
        #endif
        #if defined I2C_DMA_SUPPORT
        usTriggerSource = DMAMUX0_CHCFG_SOURCE_LPI2C2_RX;
        #endif
    }
    #endif
    else {
        return;
    }
    ptrLPI2C->LPI2C_MDER = 0;                                            // disable DMA operation
    ptrLPI2C->LPI2C_MCR = (LPI2C_CHARACTERISTICS_);                      // take the LPI2C controller out of reset
    #if defined I2C_DMA_SUPPORT
    if ((pars->ucDMAConfig & I2C_TX_DMA) != 0) {
        ptrLPI2C->LPI2C_MDER = LPI2C_MDER_TDDE;                          // enable DMA on transmission
        fnConfigDMA_buffer(LPI2C_DMA_TX_CHANNEL[pars->Channel], (usTriggerSource + 1), 0, 0, (void *)&(ptrLPI2C->LPI2C_MTDR), (DMA_BYTES | DMA_DIRECTION_OUTPUT | DMA_SINGLE_CYCLE), _lpi2c_tx_dma_Interrupts[pars->Channel], LPI2C_DMA_TX_INT_PRIORITY[pars->Channel]);
    }
    if ((pars->ucDMAConfig & I2C_RX_DMA) != 0) {
        ptrLPI2C->LPI2C_MDER |= LPI2C_MDER_RDDE;                         // enable DMA on reception
        fnConfigDMA_buffer(LPI2C_DMA_RX_CHANNEL[pars->Channel], (usTriggerSource), 0, (void *)&(ptrLPI2C->LPI2C_MRDR), 0, (DMA_BYTES | DMA_DIRECTION_INPUT | DMA_SINGLE_CYCLE), _lpi2c_rx_dma_Interrupts[pars->Channel], LPI2C_DMA_RX_INT_PRIORITY[pars->Channel]);
    }
    #endif
    if (pars->usSpeed != 0) {
        int iPrecaler;
        int iClkHiCycle;
        unsigned char ucPrescalerSetting = 0;
        unsigned long ulPrescaledClock = MCGOUTCLK;                      // assumed running from HIRC
        unsigned long ulDesiredSpeed = (pars->usSpeed * 1000);           // desired I2C speed in Hz
        unsigned char ucClkHi = 0;
        unsigned long ulAbsoluteDeviation;
        unsigned long ulBestDeviation = 0xffffffff;
        unsigned long ulI2C_speed;
        // Baud rate = (sourceClock_Hz/2^iPrecaler)/(CLKLO+1+CLKHI+1 + ROUNDDOWN((2+FILTSCL)/2^iPrecaler)
        // Assume CLKLO = 2*CLKHI, SETHOLD = CLKHI, DATAVD = CLKHI/2
        // - this calculation is based on the NXP reference
        //
        for (iPrecaler = 1; (iPrecaler <= 128); iPrecaler *= 2) {        // attempt with all possible prescaler values
            for (iClkHiCycle = 1; iClkHiCycle < 32; iClkHiCycle++) {     // attempt with all possible high cycle values
                if (iClkHiCycle == 1) {
                    ulI2C_speed = (ulPrescaledClock / (1 + 3 + 2 + (2/iPrecaler))); // the I2C frequency that this combination gives
                }
                else {
                    ulI2C_speed = (ulPrescaledClock / ((3 * iClkHiCycle) + 2 + (2/iPrecaler))); // the I2C frequency that this combination gives
                }

                if (ulDesiredSpeed >= ulI2C_speed) {                     // absolute deviation from desired frequency
                    ulAbsoluteDeviation = (ulDesiredSpeed - ulI2C_speed);
                }
                else {
                    ulAbsoluteDeviation = (ulI2C_speed - ulDesiredSpeed);
                }

                if (ulAbsoluteDeviation < ulBestDeviation) {             // if this setting is the best that has been found yet
                    ptrLPI2C->LPI2C_MCFGR1 = (ucPrescalerSetting | LPI2C_MCFGR1_IGNACK | LPI2C_MCFGR1_PINCFG_2_OPEN); // {1} set the clock prescaler and normal I2C mode (2-line with open drain) - ignore nacks
                    ucClkHi = (unsigned char)iClkHiCycle;
                    if (ulAbsoluteDeviation == 0) {
                        break;                                           // in case a pefect combination has been found we can quit searching
                    }
                    ulBestDeviation = ulAbsoluteDeviation;               // new closest settings
                }
            }
            ulPrescaledClock /= 2;
            ucPrescalerSetting++;
        }

        if (ucClkHi < 2) {
            ptrLPI2C->LPI2C_MCCR0 = (((ucClkHi << LPI2C_MCCR_CLKHI_SHIFT) & LPI2C_MCCR0_CLKHI_MASK) | (((3 << LPI2C_MCCR_CLKLO_SHIFT) & LPI2C_MCCR0_CLKLO_MASK) | ((2 << LPI2C_MCCR_SETHOLD_SHIFT) & LPI2C_MCCR0_SETHOLD_MASK) | ((1 << LPI2C_MCCR_DATAVD_SHIFT) & LPI2C_MCCR0_DATAVD_MASK)));
        }
        else {
            ptrLPI2C->LPI2C_MCCR0 = (((ucClkHi << LPI2C_MCCR_CLKHI_SHIFT) & LPI2C_MCCR0_CLKHI_MASK) | ((((2 * ucClkHi) << LPI2C_MCCR_CLKLO_SHIFT) & LPI2C_MCCR0_CLKLO_MASK) | ((ucClkHi << LPI2C_MCCR_SETHOLD_SHIFT) & LPI2C_MCCR0_SETHOLD_MASK) | (((ucClkHi/2) << LPI2C_MCCR_DATAVD_SHIFT) & LPI2C_MCCR0_DATAVD_MASK)));
        }
      //ptrLPI2C->LPI2C_MCCR0 = 0x09131326;                              // reference for 100kHz from 48MHz clock (prescale value of 3)
        ptrLPI2C->LPI2C_MCR = (LPI2C_MCR_MEN | LPI2C_CHARACTERISTICS_);  // enable master mode of operation
    }
    else {
    #if defined I2C_SLAVE_MODE
        ucSpeed = 0;                                                     // speed is determined by master
        ptrLPI2C->I2C_A1 = pars->ucSlaveAddress;                         // program the slave's address
        fnConfigLPI2C_pins(pars->Channel, 0);                            // configure pins for LPI2C use
        I2C_rx_control[pars->Channel]->ucState = I2C_SLAVE_RX_MESSAGE_MODE; // the slave receiver operates in message mode
        I2C_tx_control[pars->Channel]->ucState = I2C_SLAVE_TX_BUFFER_MODE; // the slave transmitter operates in buffer mode
        fnI2C_SlaveCallback[pars->Channel] = pars->fnI2C_SlaveCallback;  // optional callback to use on slave reception

        ptrLPI2C->I2C_C1 |= I2C_IIEN;                                    // immediately enable interrupts if operating as a slave
    #if defined DOUBLE_BUFFERED_I2C || defined I2C_START_CONDITION_INTERRUPT
        ptrLPI2C->I2C_FLT = I2C_FLT_FLT_SSIE;                            // enable start/stop condition interrupt
    #else
        ptrLPI2C->I2C_FLT = I2C_FLT_FLT_STOPIE;                          // enable stop condition interrupt
    #endif
    #else
        _EXCEPTION("I2C slave mode not enabled!");
    #endif
    }
    #if defined _WINDOWS
    fnConfigSimI2C(pars->Channel, (pars->usSpeed * 1000));               // inform the simulator of the I2C speed that has been set
    #endif
}


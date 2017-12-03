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
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnConfigI2C_pins(QUEUE_HANDLE Channel,  int iMaster);
static void fnSendSlaveAddress(I2CQue *ptI2CQue, QUEUE_HANDLE Channel, KINETIS_LPI2C_CONTROL *ptrLPI2C);

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned char ucCheckTxI2C = 0;                                   // {2}
#if defined I2C_SLAVE_MODE
    static unsigned char *ptrMessage[NUMBER_I2C] = {0};
    static unsigned char ucMessageLength[NUMBER_I2C] = {0};
    static int (*fnI2C_SlaveCallback[NUMBER_I2C])(int iChannel, unsigned char *ptrDataByte, int iType) = {0};
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined I2C_INTERFACE
    extern I2CQue *I2C_rx_control[NUMBER_I2C];
    extern I2CQue *I2C_tx_control[NUMBER_I2C];
#endif

/* =================================================================== */
/*                        I2C Interrupt Handlers                       */
/* =================================================================== */


//#define MAX_EVENTS 200                                                 // comment this in to enable event logging (mainly used to determine behaviour in double-buffered mode in order to find workarounds for silicon issues)
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

static void fnI2C_Handler(KINETIS_LPI2C_CONTROL *ptrLPI2C, int iChannel) // general LPI2C interrupt handler
{
    I2CQue *ptrTxControl = I2C_tx_control[iChannel];

    if ((ptrLPI2C->LPI2C_MIER & ptrLPI2C->LPI2C_MSR) & LPI2C_MSR_SDF) {  // stop condition has completed
        fnLogEvent('S', ptrLPI2C->LPI2C_MSR);
        ptrLPI2C->LPI2C_MIER = 0;                                        // disable all interrupt sources
        WRITE_ONE_TO_CLEAR(ptrLPI2C->LPI2C_MSR, LPI2C_MSR_SDF);          // clear interrupt flag
        if ((ptrTxControl->ucState & RX_ACTIVE) != 0) {
            I2CQue *ptrRxControl = I2C_rx_control[iChannel];
            ptrRxControl->msgs++;
            if (ptrRxControl->wake_task != 0) {                          // wake up the receiver task if desired
                uTaskerStateChange(ptrRxControl->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        else {
            if (ptrTxControl->wake_task != 0) {
                uTaskerStateChange(ptrTxControl->wake_task, UTASKER_ACTIVATE); // wake up owner task since the transmission has terminated
            }
        }
        ptrTxControl->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);     // interface is idle
        return;
    }

    if ((ptrTxControl->ucState & RX_ACTIVE) != 0) {                      // if the master is reading from a slave
        I2CQue *ptrRxControl = I2C_rx_control[iChannel];
        fnLogEvent('R', ptrLPI2C->LPI2C_MSR);
        if ((ptrTxControl->ucState & TX_ACTIVE) != 0) {                  // the address transmission has completed
            fnLogEvent('A', ptrLPI2C->LPI2C_MRDR);
            (void)(ptrLPI2C->LPI2C_MRDR);                                // dummy read to clear address
            ptrLPI2C->LPI2C_MCR = (LPI2C_MCR_RTF | LPI2C_MCR_MEN | LPI2C_CHARACTERISTICS); // ensure receive FIFO is reset
            ptrLPI2C->LPI2C_MTDR = (LPI2C_MTDR_CMD_RX_DATA | (ptrTxControl->ucPresentLen - 1)); // command the read sequence
            ptrLPI2C->LPI2C_MIER = LPI2C_MIER_RDIE;                      // enable interrupt on reception available and disable transmission interrupt
            ptrTxControl->ucState = (RX_ACTIVE);                         // signal that the address has been sent and we are now receiving
#if defined _WINDOWS
            ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_RDF;
            iInts |= (I2C_INT0 << iChannel);
#endif
        }
        else {                                                           // reception character available
#if defined _WINDOWS
            if ((ptrLPI2C->LPI2C_MTDR & LPI2C_MTDR_DATA_MASK) != 0) {    // this is a write-only register but the simulator uses it to count down the number or reception bytes that was set
                ptrLPI2C->LPI2C_MTDR--;
                ptrLPI2C->LPI2C_MSR |= LPI2C_MSR_RDF;
                iInts |= (I2C_INT0 << iChannel);
            }
            else {
                ptrLPI2C->LPI2C_MSR &= ~LPI2C_MSR_RDF;                   // final reception was received so remove the reception ready flag
            }
            ptrLPI2C->LPI2C_MRDR = fnSimI2C_devices(I2C_RX_DATA, (unsigned char)(ptrLPI2C->LPI2C_MRDR));
#endif
            *ptrRxControl->I2C_queue.put = (unsigned char)(ptrLPI2C->LPI2C_MRDR); // read the byte
            fnLogEvent('r', *ptrRxControl->I2C_queue.put);
            ptrRxControl->I2C_queue.chars++;                             // and put it into the rx buffer
            if (++(ptrRxControl->I2C_queue.put) >= ptrRxControl->I2C_queue.buffer_end) { // handle circular input buffer
                ptrRxControl->I2C_queue.put = ptrRxControl->I2C_queue.QUEbuffer;
            }
            if (--(ptrTxControl->ucPresentLen) == 0) {
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
        }
        return;
    }
    else if (ptrTxControl->ucPresentLen-- != 0) {                        // TX_ACTIVE - send next byte if available
        fnLogEvent('X', *ptrTxControl->I2C_queue.get);
        ptrLPI2C->LPI2C_MTDR = *ptrTxControl->I2C_queue.get++;;          // send the next byte (note that the command byte is 0, meaning just send byte)
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

static __interrupt void _I2C_Interrupt_0(void)                           // I2C0 interrupt
{
    fnI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C0_BLOCK, 0);
}

    #if NUMBER_I2C > 1
static __interrupt void _I2C_Interrupt_1(void)                           // I2C1 interrupt
{
    fnI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK, 1);
}
    #endif

    #if NUMBER_I2C > 2
static __interrupt void _I2C_Interrupt_2(void)                           // I2C2 interrupt
{
    fnI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK, 2);
}
    #endif

    #if NUMBER_I2C > 3
static __interrupt void _I2C_Interrupt_3(void)                           // I2C3 interrupt
{
    fnI2C_Handler((KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK, 3);
}
    #endif

/* =================================================================== */
/*                   I2C Transmission Initiation                       */
/* =================================================================== */

// Send a first byte to I2C bus
//
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    KINETIS_LPI2C_CONTROL *ptrLPI2C;
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

    ptI2CQue->ucPresentLen = *ptI2CQue->I2C_queue.get++;                 // get the length of this transaction
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {     // handle circular buffer
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;
    }

    if ((ptI2CQue->ucState & (TX_ACTIVE | RX_ACTIVE)) != 0) {            // restart since we are hanging a second telegram on to previous one
        fnLogEvent('*', ptrLPI2C->LPI2C_MSR);
#if 0
        ptrLPI2C->I2C_C1 = (I2C_IEN | I2C_IIEN | I2C_MSTA | I2C_MTX | I2C_RSTA); // repeated start
#endif
    #if defined DOUBLE_BUFFERED_I2C
        ptrLPI2C->I2C_FLT = (I2C_FLT_FLT_STARTF | I2C_FLT_FLT_STOPF);      // clear flags
        ptrLPI2C->I2C_FLT = I2C_FLT_FLT_SSIE;                              // enable start/stop condition interrupt
        #if defined _WINDOWS
        ptrLPI2C->I2C_FLT |= I2C_FLT_FLT_STARTF;
        ptrLPI2C->I2C_S |= I2C_IIF;                                        // simulate the interrupt directly
        iInts |= (I2C_INT0 << Channel);                                  // signal that an interrupt is to be generated
        #endif
        return;
    #endif
    }
    else {                                                               // address to be sent on an idle bus
        if ((ucCheckTxI2C & (1 << Channel)) == 0) {                      // on first use we check that the bus is not held in a busy state (can happen when a reset took place during an acknowledge period and the slave is holding the bus)
            ucCheckTxI2C |= (1 << Channel);                              // checked only once
            uEnable_Interrupt();
                fnConfigI2C_pins(Channel, 1);                            // check and configure pins for I2C use
            uDisable_Interrupt();
        }
        fnLogEvent('B', ptrLPI2C->LPI2C_MSR);
        while ((ptrLPI2C->LPI2C_MSR & LPI2C_MSR_BBF) != 0) {             // wait until the stop has actually been sent to avoid a further transmission being started in the mean time
    #if 0
            if ((ptrLPI2C->I2C_S & I2C_IAL) != 0) {                        // {3} arbitration lost flag set
                ptrLPI2C->I2C_S = I2C_IAL;                                 // clear arbitration lost flag
                ptrLPI2C->I2C_C1 &= ~(I2C_IEN);                            // disable I2S to clear busy
                fnDelayLoop(100);                                        // short delay before re-enabling the I2C controller (without delay it doesn't generate any further interrupts)
                ptrLPI2C->I2C_C1 |= (I2C_IEN);                             // re-enable
            }
    #endif
        }
        fnLogEvent('b', ptrLPI2C->LPI2C_MSR);
    }
    fnSendSlaveAddress(ptI2CQue, Channel, ptrLPI2C); // send a start condition or a repeated start
}

static void fnSendSlaveAddress(I2CQue *ptI2CQue, QUEUE_HANDLE Channel, KINETIS_LPI2C_CONTROL *ptrLPI2C)
{
    register unsigned char ucAddress = *ptI2CQue->I2C_queue.get++;       // the slave address
    ptrLPI2C->LPI2C_MTDR = (LPI2C_MTDR_CMD_START_DATA | ucAddress);      // generate a start or repeated start and send the slave address (this includes the rd/wr bit)
    fnLogEvent('?', ucAddress);
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {     // handle circular buffer
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;
    }
    if ((ucAddress & 0x01) != 0) {                                       // reading from the slave
        I2C_tx_control[Channel]->ucState |= (RX_ACTIVE | TX_ACTIVE);
        ptI2CQue->I2C_queue.chars -= 3;
        fnLogEvent('g', (unsigned char)(ptI2CQue->I2C_queue.chars));
        I2C_rx_control[Channel]->wake_task = *ptI2CQue->I2C_queue.get++; // enter task to be woken when reception has completed
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
    }
    else {
        I2C_tx_control[Channel]->ucState &= ~(RX_ACTIVE);
        I2C_tx_control[Channel]->ucState |= (TX_ACTIVE);                 // writing to the slave
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->ucPresentLen + 1);       // the remaining queue content
        fnLogEvent('h', (unsigned char)(ptI2CQue->I2C_queue.chars));
    }
    ptrLPI2C->LPI2C_MIER = LPI2C_MIER_TDIE;                              // enable interrupt on transmission completion
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
static void fnConfigI2C_pins(QUEUE_HANDLE Channel, int iMaster)          // {2}
{
    if (Channel == 0)  {
    #if defined KINETIS_KE && defined I2C0_B
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, KE_PORTA_BIT2) == 0) {             // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, KE_PORTB_BIT6, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT(A, KE_PORTB_BIT6, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        SIM_PINSEL0 |= SIM_PINSEL_I2C0PS;                                // remap the I2C pins
        _CONFIG_PERIPHERAL(B, 6,  (PB_6_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PB6 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 7,  (PB_7_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PB7 (alt. function 2)
    #elif defined KINETIS_KE
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT2) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, KE_PORTA_BIT3, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT(A, KE_PORTA_BIT3, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 2,  (PA_2_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PA2 (alt. function 3)
        _CONFIG_PERIPHERAL(A, 3,  (PA_3_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PA3 (alt. function 3)
    #elif defined KINETIS_KV10
        if (iMaster != 0) {
            while (_READ_PORT_MASK(C, PORTC_BIT7) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, PORTC_BIT6, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(C, PORTC_BIT6, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(C, 7,  (PC_7_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PC7 (alt. function 7)
        _CONFIG_PERIPHERAL(C, 6,  (PC_6_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PC6 (alt. function 7)
    #elif defined KINETIS_KV30
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT3) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, PORTD_BIT2, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(D, PORTD_BIT2, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 3,  (PD_3_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PD3 (alt. function 7)
        _CONFIG_PERIPHERAL(D, 2,  (PD_2_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PCD2 (alt. function 7)
    #elif ((defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05) && defined I2C0_A_0)
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT4) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT3, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT3, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 4,  (PA_4_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PA4 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 3,  (PA_3_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PA3 (alt. function 2)
    #elif ((defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05) && defined I2C0_A_1)
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT3) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT4, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT4, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 3,  (PA_3_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PA3 (alt. function 3)
        _CONFIG_PERIPHERAL(A, 4,  (PA_4_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PA4 (alt. function 3)
    #elif (defined KINETIS_KL03 && defined I2C0_B_0)
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT1) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT0, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT0, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 1,  (PB_1_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PB1 (alt. function 4)
        _CONFIG_PERIPHERAL(B, 0,  (PB_0_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PB0 (alt. function 4)
    #elif (defined KINETIS_KL03 && defined I2C0_A_2)
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT9) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT8, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT8, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 9,  (PA_9_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PA9 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 8,  (PA_8_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PA8 (alt. function 2)
    #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT4) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT3, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT3, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 4,  (PB_4_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PB4 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 3,  (PB_3_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PB3 (alt. function 2)
    #elif defined I2C0_B_LOW
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT1) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT0, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT0, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 1,  (PB_1_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PB1 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 0,  (PB_0_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PB0 (alt. function 2)
    #elif defined KINETIS_KL82 && defined I2C0_ON_D_LOW
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT3) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, PORTD_BIT2, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(D, PORTD_BIT2, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 3,  (PD_3_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PD3 (alt. function 7)
        _CONFIG_PERIPHERAL(D, 2,  (PD_2_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PD2 (alt. function 7)
    #elif defined I2C0_ON_D
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT9) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, PORTD_BIT8, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(D, PORTD_BIT8, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 9,  (PD_9_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PD9 (alt. function 2)
        _CONFIG_PERIPHERAL(D, 8,  (PD_8_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PD8 (alt. function 2)
    #elif (defined KINETIS_K64 || defined KINETIS_KL17 || defined KINETIS_K24 || defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43 || defined KINETIS_KL46) && defined I2C0_ON_E
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT25) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, PORTE_BIT24, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_HIGH(E, PORTE_BIT24, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 25, (PE_25_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PE25 (alt. function 5)
        _CONFIG_PERIPHERAL(E, 24, (PE_24_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PE24 (alt. function 5)
    #elif (defined KINETIS_K26 || defined KINETIS_K65 || defined KINETIS_K66 ||  defined KINETIS_K70) && defined I2C0_ON_E
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT18) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, PORTE_BIT19, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_HIGH(E, PORTE_BIT19, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 18, (PE_18_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PE18 (alt. function 4)
        _CONFIG_PERIPHERAL(E, 19, (PE_19_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PE19 (alt. function 4)
    #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT3) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT2, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT2, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 3,  (PB_3_I2C0_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SDA on PB3 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 2,  (PB_2_I2C0_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C0_SCL on PB2 (alt. function 2)
    #endif
    #if defined KINETIS_KE
        I2C0_C1 = (I2C_IEN);                                      _SIM_PER_CHANGE; // enable I2C controller after checking bus so that the peripheral pin function becomes valie
    #endif
    }
    #if LPI2C_AVAILABLE > 1
    else if (Channel == 1)  {
        #if defined KINETIS_KE
            #if defined I2C1_ON_H
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, KE_PORTH_BIT3) == 0) {             // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, KE_PORTH_BIT4, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT(B, KE_PORTH_BIT4, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        SIM_PINSEL1 |= SIM_PINSEL1_I2C1PS;                               // remap port
        _CONFIG_PERIPHERAL(B, 3,  (PH_3_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PH3 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 4,  (PH_4_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PH4 (alt. function 2)
            #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, KE_PORTE_BIT0) == 0) {             // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, KE_PORTE_BIT1, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT(B, KE_PORTE_BIT1, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        SIM_PINSEL1 &= ~SIM_PINSEL1_I2C1PS;
        _CONFIG_PERIPHERAL(B, 0,  (PE_0_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PE0 (alt. function 4)
        _CONFIG_PERIPHERAL(B, 1,  (PE_1_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PE1 (alt. function 4)
            #endif
        #elif (defined KINETIS_KL17 || defined KINETIS_KL27) && defined I2C1_ON_D
        if (iMaster != 0) {
            while (_READ_PORT_MASK(D, PORTD_BIT6) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, PORTD_BIT7, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(D, PORTD_BIT7, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(D, 6, (PD_6_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PD6 (alt. function 4)
        _CONFIG_PERIPHERAL(D, 7, (PD_7_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PD7 (alt. function 4)
        #elif defined KINETIS_KL02 && defined I2C1_A_1
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT3) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT4, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT4, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 3,  (PA_3_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PA3 (alt. function 3)
        _CONFIG_PERIPHERAL(A, 4,  (PA_4_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PA4 (alt. function 3)
        #elif defined KINETIS_KL02
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT9) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT8, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT8, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 9,  (PA_9_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PA9 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 8,  (PA_8_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PA8 (alt. function 2)
        #elif defined I2C1_ON_E
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT1) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT0, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(E, PORTE_BIT0, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 0,  (PE_0_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PE0 (alt. function 6)
        _CONFIG_PERIPHERAL(E, 1,  (PE_1_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PE1 (alt. function 6)
        #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(C, PORTC_BIT11) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, PORTC_BIT10, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(C, PORTC_BIT10, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
#if 0
        _CONFIG_PERIPHERAL(C, 11, (PC_11_I2C1_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SDA on PC11 (alt. function 2)
        _CONFIG_PERIPHERAL(C, 10, (PC_10_I2C1_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C1_SCL on PC10 (alt. function 2)
#endif
        #endif
    }
    #endif
    #if LPI2C_AVAILABLE > 2
    else if (Channel == 2) {
        #if defined KINETIS_K80 && defined I2C2_ON_B
        if (iMaster != 0) {
            while (_READ_PORT_MASK(B, PORTB_BIT10) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT11, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(B, PORTB_BIT11, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(B, 10, (PB_10_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SDA on PB10 (alt. function 4)
        _CONFIG_PERIPHERAL(B, 11, (PB_11_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SCL on PB11 (alt. function 4)
        #elif defined KINETIS_K80 && defined I2C2_A_LOW                    // initially configure as input with pull-up
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT7) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT6, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT6, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 7, (PA_7_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SDA on PA7 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 6, (PA_6_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SCL on PA6 (alt. function 2)
        #elif defined I2C2_A_HIGH                                          // initially configure as input with pull-up
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT13) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT14, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT14, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 13, (PA_13_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SDA on PA13 (alt. function 5)
        _CONFIG_PERIPHERAL(A, 14, (PA_14_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SCL on PA14 (alt. function 5)
        #else
            #if defined KINETIS_K80
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT10) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT11, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT11, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 10, (PA_10_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SDA on PA10 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 11, (PA_11_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SCL on PA11 (alt. function 2)
            #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT11) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT12, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT12, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
#if 0
        _CONFIG_PERIPHERAL(A, 11, (PA_11_I2C2_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SDA on PA11 (alt. function 5)
        _CONFIG_PERIPHERAL(A, 12, (PA_12_I2C2_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C2_SCL on PA12 (alt. function 5)
#endif
            #endif
        #endif
    }
    #endif
    #if LPI2C_AVAILABLE > 3
    else {
        #if defined I2C3_ON_E
        if (iMaster != 0) {
            while (_READ_PORT_MASK(E, PORTE_BIT10) == 0) {               // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT11, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(E, PORTE_BIT11, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(E, 10, (PE_10_I2C3_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C3_SDA on PE10 (alt. function 4)
        _CONFIG_PERIPHERAL(E, 11, (PE_11_I2C3_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C3_SCL on PE11 (alt. function 4)
        #else
        if (iMaster != 0) {
            while (_READ_PORT_MASK(A, PORTA_BIT1) == 0) {                // if the SDA line is low we clock the SCL line to free it
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT2, 0, (PORT_ODE | PORT_PS_UP_ENABLE)); // set output '0'
                fnDelayLoop(10);
                _CONFIG_PORT_INPUT_FAST_LOW(A, PORTA_BIT2, (PORT_ODE | PORT_PS_UP_ENABLE));
                fnDelayLoop(10);
            }
        }
        _CONFIG_PERIPHERAL(A, 1, (PA_1_I2C3_SDA | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C3_SDA on PA1 (alt. function 4)
        _CONFIG_PERIPHERAL(A, 2, (PA_2_I2C3_SCL | PORT_ODE | PORT_PS_UP_ENABLE)); // I2C3_SCL on PA2 (alt. function 4)
        #endif
    }
    #endif
}

// Configure the I2C hardware
//
extern void fnConfigI2C(I2CTABLE *pars)
{
    KINETIS_LPI2C_CONTROL *ptrLPI2C;
    unsigned char ucSpeed;
    if (pars->Channel == 0) {                                            // I2C channel 0
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
    #if defined KINETIS_KE && defined I2C0_B                             // initially configure as inputs with pull-up
        _CONFIG_PORT_INPUT(A, (KE_PORTB_BIT6 | KE_PORTB_BIT7), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined KINETIS_KE
        _CONFIG_PORT_INPUT(A, (KE_PORTA_BIT2 | KE_PORTA_BIT3), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined KINETIS_KV10
        _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT6 | PORTC_BIT7), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined KINETIS_KV30
        _CONFIG_PORT_INPUT_FAST_LOW(D, (PORTD_BIT2 | PORTD_BIT3), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif ((defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05) && (defined I2C0_A_0 || defined I2C0_A_1))
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT3 | PORTA_BIT4), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif (defined KINETIS_KL02 && defined I2C0_A_0)
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT3 | PORTA_BIT4), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif (defined KINETIS_KL03 && defined I2C0_B_0)
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT1 | PORTB_BIT0), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif (defined KINETIS_KL03 && defined I2C0_A_2)
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT8 | PORTA_BIT9), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif (defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05)
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT3 | PORTB_BIT4), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_B_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT1 | PORTB_BIT0), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined KINETIS_KL82 && defined I2C0_ON_D_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(D, (PORTD_BIT3 | PORTD_BIT2), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif defined I2C0_ON_D
        _CONFIG_PORT_INPUT_FAST_LOW(D, (PORTD_BIT9 | PORTD_BIT8), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif (defined KINETIS_K64 || defined KINETIS_KL17 || defined KINETIS_K24 || defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL28 || defined KINETIS_KL43 || defined KINETIS_KL46) && defined I2C0_ON_E
        _CONFIG_PORT_INPUT_FAST_HIGH(E, (PORTE_BIT25 | PORTE_BIT24), (PORT_ODE | PORT_PS_UP_ENABLE));
    #elif (defined KINETIS_K26 || defined KINETIS_K65 || defined KINETIS_K66 || defined KINETIS_K70) && defined I2C0_ON_E
        _CONFIG_PORT_INPUT_FAST_HIGH(E, (PORTE_BIT19 | PORTE_BIT18), (PORT_ODE | PORT_PS_UP_ENABLE));
    #else
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT3 | PORTB_BIT2), (PORT_ODE | PORT_PS_UP_ENABLE));
    #endif
    }
    #if LPI2C_AVAILABLE > 1
    else if (pars->Channel == 1) {                                       // I2C channel 1
    #if defined KINETIS_WITH_PCC
        SELECT_PCC_PERIPHERAL_SOURCE(LPI2C1, LPI2C1_PCC_SOURCE);          // select the PCC clock used by LPI2C1
    #endif
        POWER_UP_ATOMIC(4, I2C1);                                        // enable clock to module
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C1_BLOCK;
        LPI2C1_MCR = (LPI2C_MCR_RST | LPI2C_MCR_RTF | LPI2C_MCR_RRF);    // reset LPI2C controller and ensure FIFOs are reset
        #if defined irq_LPI2C1_ID
        fnEnterInterrupt(irq_LPI2C1_ID, PRIORITY_I2C1, _I2C_Interrupt_1);  // enter LPI2C1 interrupt handler
        #elif !defined irq_I2C1_ID && defined INTMUX0_AVAILABLE
        fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_I2C1), INTMUX0_PERIPHERAL_I2C1, _I2C_Interrupt_1); // enter I2C1 interrupt handler based on INTMUX
        #else
        fnEnterInterrupt(irq_I2C1_ID, PRIORITY_I2C1, _I2C_Interrupt_1);  // enter I2C1 interrupt handler
        #endif
        #if defined KINETIS_KE                                           // initially configure as inputs with pull-up
            #if defined I2C1_ON_H
        _CONFIG_PORT_INPUT(B, (KE_PORTH_BIT3 | KE_PORTH_BIT4), (PORT_ODE | PORT_PS_UP_ENABLE));
            #else
        _CONFIG_PORT_INPUT(B, (KE_PORTE_BIT0 | KE_PORTE_BIT1), (PORT_ODE | PORT_PS_UP_ENABLE));
            #endif
        #elif (defined KINETIS_KL17 || defined KINETIS_KL27) && defined I2C1_ON_D
        _CONFIG_PORT_INPUT_FAST_LOW(D, (PORTD_BIT7 | PORTD_BIT6), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined KINETIS_KL02 && defined I2C1_A_1
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT3 | PORTA_BIT4), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined KINETIS_KL02
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT8 | PORTA_BIT9), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined I2C1_ON_E
        _CONFIG_PORT_INPUT_FAST_LOW(E, (PORTE_BIT1 | PORTE_BIT0), (PORT_ODE | PORT_PS_UP_ENABLE));
        #else
        _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT11 | PORTC_BIT10), (PORT_ODE | PORT_PS_UP_ENABLE));
        #endif
    }
    #endif
    #if LPI2C_AVAILABLE > 2
    else if (pars->Channel == 2) {                                       // I2C channel 2
        #if defined KINETIS_WITH_PCC
        SELECT_PCC_PERIPHERAL_SOURCE(LPI2C2, LPI2C2_PCC_SOURCE);         // select the PCC clock used by LPI2C1
        #endif
        POWER_UP_ATOMIC(1, I2C2);                                        // enable clock to module
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C2_BLOCK;
        LPI2C2_MCR = (LPI2C_MCR_RST | LPI2C_MCR_RTF | LPI2C_MCR_RRF);    // reset LPI2C controller and ensure FIFOs are reset
        #if !defined irq_I2C2_ID && defined INTMUX0_AVAILABLE
        fnEnterInterrupt((irq_INTMUX0_0_ID + INTMUX_I2C2), INTMUX0_PERIPHERAL_I2C2, _I2C_Interrupt_2); // enter I2C2 interrupt handler
        #else
        fnEnterInterrupt(irq_I2C2_ID, PRIORITY_I2C2, _I2C_Interrupt_2);  // enter I2C2 interrupt handler
        #endif
        #if defined KINETIS_K80 && defined I2C2_ON_B                     // initially configure as inputs with pull-up
        _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT10 | PORTB_BIT11), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined KINETIS_K80 && defined I2C2_A_LOW
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT6 | PORTA_BIT7), (PORT_ODE | PORT_PS_UP_ENABLE));
        #elif defined I2C2_A_HIGH 
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT13 | PORTA_BIT14), (PORT_ODE | PORT_PS_UP_ENABLE));
        #else
            #if defined KINETIS_K80
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT10 | PORTA_BIT11), (PORT_ODE | PORT_PS_UP_ENABLE));
            #else
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT11 | PORTA_BIT12), (PORT_ODE | PORT_PS_UP_ENABLE));
            #endif
        #endif
    }
    #endif
    #if LPI2C_AVAILABLE > 3
    else if (pars->Channel == 3) {                                       // I2C channel 3
        POWER_UP_ATOMIC(1, I2C3);                                        // enable clock to module
        ptrLPI2C = (KINETIS_LPI2C_CONTROL *)LPI2C3_BLOCK;
        LPI2C3_MCR = (LPI2C_MCR_RST | LPI2C_MCR_RTF | LPI2C_MCR_RRF);    // reset LPI2C controller and ensure FIFOs are reset
        fnEnterInterrupt(irq_I2C3_ID, PRIORITY_I2C3, _I2C_Interrupt_3);  // enter I2C3 interrupt handler
        #if defined I2C3_ON_E                                            // initially configure as inputs with pull-up
        _CONFIG_PORT_INPUT_FAST_LOW(E, (PORTE_BIT10 | PORTE_BIT11), (PORT_ODE | PORT_PS_UP_ENABLE));
        #else
        _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT1 | PORTA_BIT2), (PORT_ODE | PORT_PS_UP_ENABLE));
        #endif
    }
    #endif
    else {
        return;
    }
    ptrLPI2C->LPI2C_MCR = (LPI2C_CHARACTERISTICS);                       // take the LPI2C controller out of reset
    if (pars->usSpeed != 0) {
       // ptrLPI2C->LPI2C_MCFGR1 = (LPI2C_MCFG1_PRESCALE_128 | LPI2C_MCFG1_PINCFG_2_OPEN); // set the clock prescaler and normal I2C mode (2-line with open drain)
       // ptrLPI2C->LPI2C_MCCR0 = (LPI2C_MCCR0_CLKLO_MASK | LPI2C_MCCR0_CLKHI_MASK | LPI2C_MCCR0_SETHOLD_MASK | LPI2C_MCCR0_DATAVD_MASK); // try max timing

        ptrLPI2C->LPI2C_MCFGR1 = (3 | LPI2C_MCFG1_PINCFG_2_OPEN); // set the clock prescaler and normal I2C mode (2-line with open drain)
        ptrLPI2C->LPI2C_MCCR0 = 0x09131326; // try max timing - 100kHz from 48MHz clock (calculation needs to be added!!!)


        ptrLPI2C->LPI2C_MCR = (LPI2C_MCR_MEN | LPI2C_CHARACTERISTICS);   // enable master mode of operation
    }
    else {
    #if defined I2C_SLAVE_MODE
        ucSpeed = 0;                                                     // speed is determined by master
        ptrLPI2C->I2C_A1 = pars->ucSlaveAddress;                         // program the slave's address
        fnConfigI2C_pins(pars->Channel, 0);                              // configure pins for I2C use
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


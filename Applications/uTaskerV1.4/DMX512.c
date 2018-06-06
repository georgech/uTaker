/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET
    
    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland
    
    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      DMX512.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined USE_DMX512_MASTER || defined USE_DMX512_SLAVE

#define OWN_TASK                  TASK_DMX512

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if defined FRDM_K66F
    #define DMX512_MASTER_UART      5                                    // K66 LPUART 0
    #define DMX512_RDM_TE_MASTER    PORTB_BIT6                           // master's drive output
    #define DMX512_RDM_TE_SLAVE     PORTB_BIT7                           // slave's drive output
    #define DMX512_SLAVE_UART       3                                    // K66 UART 1
#elif defined FRDM_K82F
    #define DMX512_MASTER_UART      3                                    // K82 LPUART 3
    #define DMX512_RDM_TE_MASTER    PORTB_BIT6                           // master's drive output
    #define DMX512_RDM_TE_SLAVE     PORTB_BIT7                           // slave's drive output
    #define DMX512_SLAVE_UART       1                                    // K82 LPUART 1
#elif defined FRDM_KL27Z
    #define DMX512_MASTER_UART      1
    #define DMX512_SLAVE_UART       2
#endif
#if defined _WINDOWS
    #define DMX512_PERIOD          300000                                // 300m period
#else
    #define DMX512_PERIOD          30000                                 // 30.000ms
#endif
#define DMX512_BREAK               100                                   // 100us break time
#define DMX512_MAB                 16                                    // 16us MAB time
#define DMX_SLOT_COUNT             512                                   // maximum DMX512 data length
#define DMX512_TX_BUFFER_COUNT     2                                     // 2 buffers that can be prepared in advance

#define DMX512_MASTER_INTERFACE_COUNT 1
#define DMX512_SLAVE_INTERFACE_COUNT  1


#define DMX_RX_MAX_SLOT_COUNT         512
#if defined USE_DMX_RDM_SLAVE
    #define DMX512_RX_BUFFER_COUNT    1
#else
    #define DMX512_RX_BUFFER_COUNT    2
#endif

#if defined USE_DMX_RDM_MASTER
    #define DMX512_TX_BUFFER_SIZE    (DMX_SLOT_COUNT + 1)
#else
    #define DMX512_TX_BUFFER_SIZE    (DMX_SLOT_COUNT + 1 + 2)
#endif

#if defined FRDM_K66F || defined FRDM_K82F
    #if defined FRDM_K82F
        #define START_DMX512_BREAK() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, PORTC_BIT17, 0, (PORT_SRE_FAST | PORT_DSE_HIGH)) // start break generation by configuring pin as GPIO, drivng 0
        #define END_DMX512_BREAK()   _CONFIG_PERIPHERAL(C, 17, (PC_17_LPUART3_TX | UART_PULL_UPS)) // LPUART3_TX on PTC17 (alt. function 3)
    #else
        #define START_DMX512_BREAK() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT8, 0, (PORT_SRE_FAST | PORT_DSE_HIGH)) // start break generation by configuring pin as GPIO, drivng 0
        #define END_DMX512_BREAK()   _CONFIG_PERIPHERAL(E, 8, (PE_8_LPUART0_TX | UART_PULL_UPS)) // LPUART0_TX on PTE8 (alt. function 5)
    #endif
    #if defined USE_DMX_RDM_MASTER
        #define START_DMX512_TX()    fnDriver(DMX512_Master_PortID, (TX_ON | PAUSE_TX), MODIFY_TX)  // release paused output buffer
    #else
        #define START_DMX512_TX()    LPUART0_CTRL |= (LPUART_CTRL_TE)    // enable the transmitter so that waiting frame starts
    #endif
    #define DMX512_CONFIGURE_MASTER_DRIVE_OUTPUT()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, PORTB_BIT6, PORTB_BIT6, (PORT_SRE_SLOW | PORT_DSE_LOW))
    #define DMX512_MASTER_DRIVE_OUTPUT()             _SETBITS(B, PORTB_BIT6)
    #define DMX512_SET_MASTER_INPUT()                _CLEARBITS(B, PORTB_BIT6)

    #define DMX512_CONFIGURE_SLAVE_DRIVE_OUTPUT()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, PORTB_BIT5, 0, (PORT_SRE_SLOW | PORT_DSE_LOW))
    #define DMX512_SLAVE_DRIVE_OUTPUT()              _SETBITS(B, PORTB_BIT5)
    #define DMX512_SET_SLAVE_INPUT()                 _CLEARBITS(B, PORTB_BIT5)
#elif defined FRDM_KL27Z
    #define START_DMX512_BREAK()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT0, 0, (PORT_SRE_FAST | PORT_DSE_HIGH)) // start break generation by configuring pin as GPIO, drivng 0
    #define END_DMX512_BREAK()     _CONFIG_PERIPHERAL(E, 0, (PE_0_LPUART1_TX | UART_PULL_UPS)) // LPUART1_TX on PE0 (alt. function 3)
    #define START_DMX512_TX()      LPUART1_CTRL |= LPUART_CTRL_TE;       // enable the transmitter so that waiting DMA starts
#endif
//#define START_DMX512_TX() fnDriver(DMX512_Master_PortID, (TX_ON), MODIFY_TX); // note that this doesn't work since it configures the Tx again

#define START_CODE_DMX512               0x00
#define START_CODE_RDM                  0xcc

#define DISC_UNIQUE_BRANCH_PREAMBLE     0xfe
#define DISC_UNIQUE_PREAMBLE_SEPARATOR  0xaa

#define SUB_START_CODE_MESSAGE          0x01

// Interrupt events
//
#define DMX512_TX_NEXT_TRANSMISSION          1
#define DMX512_RDM_RECEPTION_READY           2
#define DMX512_RDM_BROADCAST_RECEPTION_READY 3
#define DMX512_DMX_RECEPTION_READY           4

// Hardware timer interrupt events
//
#define DMX512_START_MAB                1
#define DMX512_START_TX                 2


typedef struct stDMX512_RDM_PACKET
{
    unsigned char ucStartCode;
    unsigned char ucSubStartCode;
    unsigned char ucMessageLength;                                       // length includes the start code but excludes the checksum field (24..255)
    unsigned char ucDestinationUID[6];
    unsigned char ucSourceUID[6];
    unsigned char ucTransactionNumber;
    unsigned char ucPortID_ResponseType;
    unsigned char ucMessageCount;
    unsigned char ucSubDevice[2];
    unsigned char ucMessageDataBlock[1];                                 // variable size
  //unsigned char ucChecksum[2];                                         // closes the complete packet
} DMX512_RDM_PACKET;

typedef struct stDMX512_MESSAGE_DATA_BLOCK
{
    unsigned char ucCommandClass;
    unsigned char ucParameterID[2];
    unsigned char ucParameterDataLength;
    unsigned char ucParameterData[1];                                    // variable size
} DMX512_MESSAGE_DATA_BLOCK;

typedef struct stDMX512_RDM_DISC_UNIQUE_BRANCH_RESPONSE_PACKET
{
    unsigned char ucPreamble[7];
    unsigned char ucPreambleSeparator;
    unsigned char ucManufacturerID[4];
    unsigned char ucUID[8];
    unsigned char ucChecksum[4];
} DMX512_RDM_DISC_UNIQUE_BRANCH_RESPONSE_PACKET;


#define DMX512_RDM_COMMAND_CLASS_GET_COMMAND                 0x10
#define DMX512_RDM_COMMAND_CLASS_GET_COMMAND_RESPONSE        0x11
#define DMX512_RDM_COMMAND_CLASS_SET_COMMAND                 0x20
#define DMX512_RDM_COMMAND_CLASS_SET_COMMAND_RESPONSE        0x21
#define DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND           0x30
#define DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND_RESPONSE  0x31

#define DMX512_RDM_PARAMETER_ID_DISC_UNIQUE_BRANCH           0x0001      // required
#define DMX512_RDM_PARAMETER_ID_DISC_MUTE                    0x0002      // required
#define DMX512_RDM_PARAMETER_ID_DISC_UNMUTE                  0x0003      // required
#define DMX512_RDM_PARAMETER_ID_SUPPORTED_PARAMETERS         0x0050      // required if supporting parameters beyond the minimum required set
#define DMX512_RDM_PARAMETER_ID_PARAMETER_DESCRIPTION        0x0051      // required for manufacturer specific PIDs
#define DMX512_RDM_PARAMETER_ID_DEVICE_INFO                  0x0060      // required
#define DMX512_RDM_PARAMETER_ID_SOFTWARE_VERSION_LABEL       0x00c0      // required
#define DMX512_RDM_PARAMETER_ID_DMX_START_ADDRESS            0x00f0      // required if device uses a DMX512 slot
#define DMX512_RDM_PARAMETER_ID_DEVICE_IDENTIFY              0x1000      // required

#define MUTE_MESSAGE_CONTROL_FIELD_MANAGED_PROXY_FLAG        0x0001
#define MUTE_MESSAGE_CONTROL_FIELD_SUB_DEVICE_FLAG           0x0002
#define MUTE_MESSAGE_CONTROL_FIELD_BOOT_LOADER_FLAG          0x0004
#define MUTE_MESSAGE_CONTROL_FIELD_PROXIED_DEVICE_FLAG       0x0008

#define DMX512_RDM_RESPONSE_TYPE_DISCOVERY_COMMAND_RESPONSE
#define DMX512_RDM_RESPONSE_TYPE_GET_COMMAND_RESPONSE
#define DMX512_RDM_RESPONSE_TYPE_SET_COMMAND_RESPONSE

#define DMX512_RDM_RESPONSE_TYPE_ACK                         0x00
#define DMX512_RDM_RESPONSE_TYPE_ACK_TIMER                   0x01
#define DMX512_RDM_RESPONSE_TYPE_NACK_REASON                 0x02
#define DMX512_RDM_RESPONSE_TYPE_ACK_OVERFLOW                0x03

#define DMX512_RDM_DISCOVERY     0x01
#define DMX512_RDM_GLOBAL_UNMUTE 0x02
#define DMX512_RDM_DISCOVERING   0x03
#define DMX512_RDM_UNICAST_MUTE  0x04
#define DMX512_RDM_STATE_MASK    0x0f
#define DMX512_INITIALISED       0x10
#define DMX512_RDM_RESTART       0x40
#define DMX512_RDM_RECEPTION     0x80


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined USE_DMX512_SLAVE || (defined USE_DMX512_MASTER && defined USE_DMX_RDM_MASTER)
    static int fnHandleDMX512_frame(QUEUE_HANDLE uart_handle, unsigned short usFrameLength, int iBroadcast);
    static void fnHandleDMX(QUEUE_HANDLE uart_handle, DMX512_RDM_PACKET *ptrPacket, unsigned short usPacketLength);
#endif
#if defined USE_DMX_RDM_MASTER || defined USE_DMX_RDM_SLAVE
    static unsigned short fnDMX512_RDM_checksum(DMX512_RDM_PACKET *ptrPacket, unsigned short usPacketLength);
    static int fnSend_DMX512_RDM(QUEUE_HANDLE uart_handle, unsigned char ucCommandClass, unsigned short usParameterID, const unsigned char ucDestinationUID[6], DMX512_RDM_PACKET *ptrPacket);
#endif
#if defined USE_DMX512_MASTER
    static unsigned short fnConstructDMX512(unsigned char *ptrFrameBuffer);
    static void fnConfigureDMX512_framing(void);
#endif
#if defined USE_DMX_RDM_MASTER
    static int fnHandleRDM_MasterRx(QUEUE_HANDLE uart_handle, unsigned char ucUID[6], unsigned short usExpected);
        #define RDM_MASTER_NO_RECEPTION       0
        #define RDM_MASTER_RECEPTION_DETECTED 1
        #define RDM_MASTER_VALID_RECEPTION    2
    static void fnDMX512_Master_TxFrameTermination(QUEUE_HANDLE channel);
#endif
#if defined USE_DMX_RDM_SLAVE
    static int fnDMX512_slave_rx(unsigned char data, QUEUE_HANDLE channel);
    static int fnDMX512_break_rx(QUEUE_HANDLE channel);
    static void fnDMX512_Slave_TxFrameTermination(QUEUE_HANDLE channel);
    static void fnHandleRDM_SlaveRx(QUEUE_HANDLE uart_handle, DMX512_RDM_PACKET *ptrPacket, unsigned short usPacketLength, int iBroadcast);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned char ucDMX512_state = 0;
static unsigned char ucDMX512_HW_event = 0;
#if defined USE_DMX512_MASTER
    static QUEUE_HANDLE  DMX512_Master_PortID = NO_ID_ALLOCATED;
#endif
#if defined USE_DMX512_SLAVE
    static QUEUE_HANDLE  DMX512_Slave_PortID = NO_ID_ALLOCATED;
#endif
#if defined USE_DMX_RDM_SLAVE
    static unsigned char ucMuteFlag[256/8] = {{0}};                      // mute flags (reset by default and after a DISC_UN_MUTE message) for each port ID
    static int iRxCount = 0;
    static int iTurnAroundDelaySlave = 0;
    static unsigned char ucType = 0;
    static unsigned short usThisLength = 0;
    static unsigned char ucRxBuffer[DMX_RX_MAX_SLOT_COUNT + 1] = {0};
    static unsigned char ucSlaveSourceUID[6] = {0xcb, 0xa9, 0x87, 0x65, 0x43, 0x22}; // slave's UID
#endif
#if defined USE_DMX_RDM_MASTER
    static unsigned char ucTransactionNumber = 0;                        // message transaction number
    static unsigned char ucPortID = 0x01;
    static unsigned char ucLowerBoundUID[6] = {0};
    static unsigned char ucUpperBoundUID[6] = {0};
    static unsigned char ucMasterSourceUID[6] = {0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21}; // master's UID
#endif


// DMX512 task
//
extern void fnDMX512(TTASKTABLE *ptrTaskTable)
{
    #if defined USE_DMX512_MASTER
        #if defined USE_DMX_RDM_MASTER
    int iResult;
    unsigned char ucDMX512_tx_buffer[DMX512_TX_BUFFER_SIZE];
    unsigned char ucUID[6];
        #else
    unsigned char ucDMX512_tx_buffer[DMX512_TX_BUFFER_SIZE];
        #endif
    unsigned short usTxLength;
    #endif
    QUEUE_HANDLE  PortIDInternal = ptrTaskTable->TaskID;                 // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (ucDMX512_state == 0) {                                           // on initialisation
        unsigned char ucMode;
        TTYTABLE tInterfaceParameters;                                   // table for passing information to driver
        tInterfaceParameters.Channel = DMX512_MASTER_UART;
        tInterfaceParameters.ucSpeed = SERIAL_BAUD_250K;                 // fixed DMX512 baud rate
        tInterfaceParameters.Config = (CHAR_8 | NO_PARITY | TWO_STOPS | CHAR_MODE); // fixed DMX512 settings
        tInterfaceParameters.Task_to_wake = OWN_TASK;                    // wake self when messages have been received
    #if defined SERIAL_SUPPORT_DMA
        tInterfaceParameters.ucDMAConfig = (UART_TX_DMA);                // activate DMA on transmission
    #endif
    #if defined USE_DMX512_MASTER
        #if defined USE_DMX_RDM_MASTER
        DMX512_CONFIGURE_MASTER_DRIVE_OUTPUT();
        DMX512_MASTER_DRIVE_OUTPUT();                                    // drive the transmitter line
            #if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
        tInterfaceParameters.txFrameCompleteHandler = fnDMX512_Master_TxFrameTermination; // we require interrupt call back on the end of transmission (after second stop bit has completed)
            #endif
            #if defined FRDM_KL27Z && defined SERIAL_SUPPORT_RX_DMA_BREAK && defined SERIAL_SUPPORT_DMA_RX && defined SERIAL_SUPPORT_DMA_RX_FREERUN
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 1024;             // input buffer size must be modulo size for free-running DMA use
            #else
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = (DMX_RX_MAX_SLOT_COUNT + 1); // input buffer size
            #endif
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = (sizeof(ucDMX512_tx_buffer)); // output buffer size for a single frame
        ucMode = FOR_I_O;                                                // UART will be opened for read and write
        #else
        tInterfaceParameters.Config |= (UART_HW_TRIGGERED_TX_MODE);      // set hardware triggered mode when only fixed length DMX512 is transmitted
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 0;
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = (sizeof(ucDMX512_tx_buffer) * DMX512_TX_BUFFER_COUNT); // output buffer size - for multiple frames
        ucMode = FOR_WRITE;                                              // UART will be opened for write only
            #if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
        tInterfaceParameters.txFrameCompleteHandler = 0;                 // no handler
            #endif
        #endif
        if (NO_ID_ALLOCATED == (DMX512_Master_PortID = fnSetNewSerialMode(&tInterfaceParameters, ucMode))) { // open serial port for I/O
            return;                                                      // if the serial port could not be opened we quit
        }
        #if defined USE_DMX_RDM_MASTER
        fnDriver(DMX512_Master_PortID, (TX_OFF | PAUSE_TX), MODIFY_TX);  // set the transmitter to paused mode so that we can queue a first message
        fnDriver(DMX512_Master_PortID, (RX_OFF), MODIFY_RX);             // disable the receiver
        #else
        fnDriver(DMX512_Master_PortID, (TX_OFF), MODIFY_TX);             // disable the transmitter since we will be preparing output and then starting by enabling the transmitter
        #endif
        usTxLength = fnConstructDMX512(ucDMX512_tx_buffer);
        fnWrite(DMX512_Master_PortID, ucDMX512_tx_buffer, usTxLength);   // prepare first DMX512 frame (queued)
        fnConfigureDMX512_framing();                                     // configure and start the DMX512 frame time base
    #endif
    #if defined USE_DMX512_SLAVE
        tInterfaceParameters.Channel = DMX512_SLAVE_UART;
        #if defined FRDM_KL27Z && defined SERIAL_SUPPORT_RX_DMA_BREAK && defined SERIAL_SUPPORT_DMA_RX && defined SERIAL_SUPPORT_DMA_RX_FREERUN
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 1024;             // input buffer size must be modulo size for free-running DMA use
        #else
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = ((DMX_RX_MAX_SLOT_COUNT + 1) * DMX512_RX_BUFFER_COUNT); // input buffer size
        #endif
        #if defined USE_DMX_RDM_SLAVE
        DMX512_CONFIGURE_SLAVE_DRIVE_OUTPUT();                           // configure the transmitter driver as inactive
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = (sizeof(ucDMX512_tx_buffer)); // output buffer size for a single frame
        tInterfaceParameters.Config |= (MSG_BREAK_MODE);                 // use interrupt driven message mode reception with break detection
            #if defined USER_DEFINED_UART_RX_HANDLER
        tInterfaceParameters.receptionHandler = fnDMX512_slave_rx;       // handle each reception character in interrupt
            #endif
            #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
        tInterfaceParameters.receiveBreakHandler = fnDMX512_break_rx;    // break detection handler
            #endif
            #if defined USER_DEFINED_UART_TX_FRAME_COMPLETE
        tInterfaceParameters.txFrameCompleteHandler = fnDMX512_Slave_TxFrameTermination; // we require interrupt call back on the end of transmission (after second stop bit has completed)
            #endif
        ucMode = FOR_I_O;                                                // UART will be opened for read and write
        #else
            #if defined SERIAL_SUPPORT_RX_DMA_BREAK && defined SERIAL_SUPPORT_DMA_RX && defined SERIAL_SUPPORT_DMA_RX_FREERUN
        tInterfaceParameters.ucDMAConfig |= (UART_RX_DMA | UART_RX_DMA_BREAK);
                #if defined FRDM_KL27Z
        tInterfaceParameters.ucDMAConfig |= UART_RX_MODULO;              // modulo reception buffer demanded
                #endif
            #elif defined UART_BREAK_SUPPORT
        tInterfaceParameters.Config |= (MSG_BREAK_MODE);                 // use interrupt driven message mode reception with break detection
            #endif
            #if defined USER_DEFINED_UART_RX_HANDLER
        tInterfaceParameters.receptionHandler = ?;                       // no handler
            #endif
            #if defined USER_DEFINED_UART_RX_BREAK_DETECTION
        tInterfaceParameters.receiveBreakHandler = 0;                    // no handler
            #endif
        ucMode = FOR_READ;                                               // UART will be opened for read only
        #endif
        if (NO_ID_ALLOCATED == (DMX512_Slave_PortID = fnSetNewSerialMode(&tInterfaceParameters, ucMode))) { // open serial port for I/O
            return;                                                      // if the serial port could not be opened we quit
        }
        fnDriver(DMX512_Slave_PortID, (TX_OFF | PAUSE_TX), MODIFY_TX);   // set the transmitter to paused mode so that we can queue a message
    #endif
        ucDMX512_state = DMX512_INITIALISED;
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on message source
        case TIMER_EVENT:
            switch (ucInputMessage[MSG_TIMER_EVENT]) {
            case 0:
                break;
            default:
                break;
            }
            break;

        case INTERRUPT_EVENT:
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
    #if defined USE_DMX512_MASTER
            case DMX512_TX_NEXT_TRANSMISSION:                            // DMX512 frame is starting so we should prepare the following frame's data content
        #if defined USE_DMX_RDM_MASTER
                switch (ucDMX512_state & DMX512_RDM_STATE_MASK) {
                case 0:                                                  // DMX512 mode
                    usTxLength = fnConstructDMX512(ucDMX512_tx_buffer);  // construct next DMX512 frame
                    fnWrite(DMX512_Master_PortID, ucDMX512_tx_buffer, usTxLength); // prepare next DMX512 frame in the output buffer (it will be released at the end of the MAB period)
                    break;
                case DMX512_RDM_DISCOVERY:                               // discovery is to start
                    fnSend_DMX512_RDM(DMX512_Master_PortID, DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND, DMX512_RDM_PARAMETER_ID_DISC_UNMUTE, cucBroadcast, 0); // prepare a broadcast unmute so that all slaves will later respond
                    uDisable_Interrupt();
                        ucDMX512_state &= ~(DMX512_RDM_STATE_MASK);
                        ucDMX512_state |= (DMX512_RDM_GLOBAL_UNMUTE);    // mark that we are sending a global unmute
                    uEnable_Interrupt();
                    uMemset(ucLowerBoundUID, 0x00, 6);                   // initial lower bound
                    uMemset(ucUpperBoundUID, 0xff, 5);                   // initial upper bound
                    ucUpperBoundUID[5] = 0xfe;
                    break;
                case DMX512_RDM_DISCOVERING:
                    // Check whether slaves have been detected and choose the next bounds
                    //
                    iResult = fnHandleRDM_MasterRx(DMX512_Master_PortID, ucUID, 0);
                    switch (iResult) {
                    case RDM_MASTER_NO_RECEPTION:                        // no slave activity detected
                        break;
                    case RDM_MASTER_RECEPTION_DETECTED:                  // invalid data or collision detected
                        break;
                    case RDM_MASTER_VALID_RECEPTION:                     // a slave has been identified
                        fnSend_DMX512_RDM(DMX512_Master_PortID, DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND, DMX512_RDM_PARAMETER_ID_DISC_MUTE, ucUID, 0); // prepare a uncast mute to the slave that we have discovered
                        uDisable_Interrupt();
                            ucDMX512_state &= ~(DMX512_RDM_STATE_MASK);
                            ucDMX512_state |= (DMX512_RDM_UNICAST_MUTE); // mark that we are sending a unicast mute
                        uEnable_Interrupt();
                        break;
                    }
                    // Fall through intentionally
                    //
                case DMX512_RDM_GLOBAL_UNMUTE:
                    fnSend_DMX512_RDM(DMX512_Master_PortID, DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND, DMX512_RDM_PARAMETER_ID_DISC_UNIQUE_BRANCH, cucBroadcast, 0); // prepare a discovery transmission
                    uDisable_Interrupt();
                        ucDMX512_state &= ~(DMX512_RDM_STATE_MASK);
                        ucDMX512_state |= (DMX512_RDM_DISCOVERING | DMX512_RDM_RESTART); // mark that we are sending a discovery and allow a following message to start
                    uEnable_Interrupt();
                    break;
                case DMX512_RDM_UNICAST_MUTE:
                    if (fnHandleRDM_MasterRx(DMX512_Master_PortID, 0, DMX512_RDM_PARAMETER_ID_DISC_MUTE) == 0) {
                        // Slave was successfully muted
                        //
                    }
                    break;
                }
        #else
                if (fnWrite(DMX512_Master_PortID, 0, DMX512_TX_BUFFER_SIZE) > 0) { // if there is free buffer space
                    usTxLength = fnConstructDMX512(ucDMX512_tx_buffer);
                    fnWrite(DMX512_Master_PortID, ucDMX512_tx_buffer, usTxLength); // prepare next DMX512 frame
                }
        #endif
                break;
    #endif
    #if defined USE_DMX_RDM_SLAVE
            case DMX512_DMX_RECEPTION_READY:                             // DMX frame received
            case DMX512_RDM_BROADCAST_RECEPTION_READY:
            case DMX512_RDM_RECEPTION_READY:                             // an RDM message has been received which must be responed to
                fnHandleDMX512_frame(DMX512_Slave_PortID, usThisLength, (DMX512_RDM_BROADCAST_RECEPTION_READY == ucInputMessage[MSG_INTERRUPT_EVENT])); // handle the received frame
                break;
    #endif
            default:
                break;
            }
            break;

    #if defined USE_DMX512_SLAVE && !defined USE_DMX_RDM_SLAVE
        case TASK_TTY:                                                   // message from the UART (pseudo) task
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]);
            switch (ucInputMessage[0]) {
            case TTY_BREAK_FRAME_RECEPTION:
                {
                    QUEUE_HANDLE Channel;
                    unsigned short usFrameLength;
                    Channel = ucInputMessage[1];                         // uart channel that the frame has been received on
                    usFrameLength = ucInputMessage[2];
                    usFrameLength <<= 8;
                    usFrameLength |= ucInputMessage[3];                  // the length of the frame waiting in the UART buffer
                    fnHandleDMX512_frame(DMX512_Slave_PortID, usFrameLength, 0); // handle the received frame
                    break;
                }
            }
            break;
    #endif
        default:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // flush any unexpected messages (assuming they arrived from another task)
            break;
        }
    }
}



#if defined USE_DMX512_MASTER
// DMX512 transmit frame period interrupt
//
static void _frame_interrupt(void)
{
    #if defined USE_DMX_RDM_MASTER
    if ((ucDMX512_state & DMX512_RDM_RECEPTION) != 0) {
        if ((DMX512_RDM_RESTART & ucDMX512_state) != 0) {                // if a restart is queued
            ucDMX512_state &= ~(DMX512_RDM_RESTART | DMX512_RDM_RECEPTION); // reset the flags and allow the break to be sent
        }
        else {
            return;
        }
    }
    #endif
    START_DMX512_BREAK();                                                // initiate the break
    #if !defined USE_DMX_RDM_MASTER
    fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_NEXT_TRANSMISSION)); // inform the application that a DMX512 frame is just starting and it should prepare the following frame content
    #endif
}

// Interrupt at the end of the break period
//
static void _mab_start_interrupt(void)
{
    #if defined USE_DMX_RDM_MASTER
    if ((ucDMX512_state & DMX512_RDM_RECEPTION) != 0) {
        return;
    }
    #endif
    END_DMX512_BREAK();                                                  // remove the break condition (this is the start of the MAB period)
}

// Interrupt at the end of the MAB period
//
static void _mab_stop_interrupt(void)
{
    #if defined USE_DMX_RDM_MASTER
    if ((ucDMX512_state & DMX512_RDM_RECEPTION) != 0) {
        return;
    }
    #endif
    START_DMX512_TX();                                                   // initiate frame transmission (this is the end of the MAB period)
}

static void fnConfigureDMX512_framing(void)
{
    // Set up a periodic timer to control the frame rate
    // - this method is suitable for kintis parts using a PWM channel to control the break and period, plus a second channel to define the MAB
    //

    // TPM1 setup
    //
    PWM_INTERRUPT_SETUP pwm_setup;
    pwm_setup.int_type = PWM_INTERRUPT;
    pwm_setup.pwm_mode = (PWM_SYS_CLK | PWM_PRESCALER_16 | PWM_EDGE_ALIGNED | PWM_POLARITY/* | PWM_NO_OUTPUT*/); // clock PWM timer from the system clock with /16 pre-scaler
    pwm_setup.int_handler = 0;
    pwm_setup.pwm_reference = (_TPM_TIMER_1 | 0);                        // timer module 1, channel 0
    #if defined _WINDOWS
    pwm_setup.pwm_mode |= PWM_PRESCALER_128;
    pwm_setup.pwm_frequency = (unsigned short)PWM_TPM_CLOCK_US_DELAY(DMX512_PERIOD, 128); // generate frame rate frequency on PWM output
    #else
    pwm_setup.pwm_frequency = (unsigned short)PWM_TPM_CLOCK_US_DELAY(DMX512_PERIOD, 16); // generate frame rate frequency on PWM output
    #endif
    pwm_setup.pwm_value = ((pwm_setup.pwm_frequency * DMX512_BREAK) / DMX512_PERIOD); // output starts low (inverted polarity) and goes high after the break time
    pwm_setup.int_priority = PRIORITY_HW_TIMER;                          // interrupt priority of cycle interrupt
    pwm_setup.pwm_mode |= PWM_CHANNEL_INTERRUPT;                         // use channel interrupt to stop the break
    pwm_setup.channel_int_handler = _mab_start_interrupt;
    START_DMX512_BREAK();                                                // generate the first break immediately
    fnConfigureInterrupt((void *)&pwm_setup);                            // configure and start
    pwm_setup.pwm_reference = (_TPM_TIMER_1 | 1);                        // timer module 1, channel 1
    pwm_setup.pwm_value = ((pwm_setup.pwm_frequency * (DMX512_BREAK + DMX512_MAB - 44))/DMX512_PERIOD); // second channel goes high after the MAB period to control the timing for the start of the frame transmission
    pwm_setup.int_handler = _frame_interrupt;                            // interrupt call-back on PWM cycle
    pwm_setup.channel_int_handler = _mab_stop_interrupt;
    fnConfigureInterrupt((void *)&pwm_setup);                            // configure and start
}

static unsigned short fnConstructDMX512(unsigned char *ptrFrameBuffer)
{
    unsigned char ucData = 0;
    int i = 0;
    #if !defined USE_DMX_RDM_MASTER
    ptrFrameBuffer[i++] = (unsigned char)((DMX_SLOT_COUNT + 1) >> 8);    // individual message content length
    ptrFrameBuffer[i++] = (unsigned char)(DMX_SLOT_COUNT + 1);
    #endif
    ptrFrameBuffer[i++] = START_CODE_DMX512;                             // DMX512 frame
    while (i < DMX512_TX_BUFFER_SIZE) {
        ptrFrameBuffer[i] = ++ucData;
        i++;
    }
    return (unsigned short)i;
}

    #if (defined USE_DMX_RDM_MASTER && defined UART_FRAME_COMPLETE) || defined USE_DMX_RDM_SLAVE
static void fnStartDelay(unsigned long ulDelay_us, void(*int_handler)(void))
{
    PIT_SETUP pit_setup;                                                 // PIT interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.int_priority = PIT0_INTERRUPT_PRIORITY;
    pit_setup.count_delay = ulDelay_us;
    pit_setup.mode = (PIT_SINGLE_SHOT);                                  // single shot timer mode
    pit_setup.int_handler = int_handler;
    pit_setup.ucPIT = 0;
    fnConfigureInterrupt((void *)&pit_setup);
}
    #endif

    #if defined USE_DMX_RDM_MASTER
// Timeout after sending an RDM frame
//
static void fnRDM_timeout(void)
{
    fnDriver(DMX512_Master_PortID, (RX_OFF), MODIFY_RX);                 // disable reception
    DMX512_MASTER_DRIVE_OUTPUT();                                        // drive the output again
    fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_NEXT_TRANSMISSION));
}

// The last stop bit of the master UART transmission of a frame has been sent
//
static void fnDMX512_Master_TxFrameTermination(QUEUE_HANDLE channel)
{
    if ((ucDMX512_state & DMX512_RDM_STATE_MASK) > DMX512_RDM_GLOBAL_UNMUTE) { // if a RDM transmission was sent that expects a response
        DMX512_SET_MASTER_INPUT();                                       // stop driving the line so that slaves can respond
        fnDriver(DMX512_Master_PortID, (RX_ON), MODIFY_RX);              // enable the receiver
        ucDMX512_state |= DMX512_RDM_RECEPTION;                          // mark that we are receiving
        // A slave is expected to respond after a minimum delay of 176us and maximum of 2.8ms
        // discovery responses are data without a break but all other responses have a break before the data
        // the master must not send a further RDM discovery message withing 5.8ms, or a normal message within 176us
        //
        fnStartDelay(PIT_US_DELAY(6000), fnRDM_timeout);                 // start timeout
        return;
    }
    else {
        fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_NEXT_TRANSMISSION)); // inform the application that a DMX512 frame is just starting and it should prepare the following frame content
    }
    fnDriver(DMX512_Master_PortID, (TX_OFF | PAUSE_TX), MODIFY_TX);      // set the transmitter to paused mode so that we can queue a subsequent message
}
    #endif
#endif


#if defined USE_DMX512_SLAVE
static int fnHandleDMX512_frame(QUEUE_HANDLE uart_handle, unsigned short usFrameLength, int iBroadcast)
{
    unsigned char *ptrRxFrame;
    #if defined USE_DMX_RDM_SLAVE
    ptrRxFrame = ucRxBuffer;
    #else
    unsigned char ucRxFrame[DMX_RX_MAX_SLOT_COUNT + 1];
    if (usFrameLength > sizeof(ucRxFrame)) {                             // flush and ignore oversize reception frames
        while (usFrameLength >= sizeof(ucRxFrame)) {
            fnRead(uart_handle, ucRxFrame, sizeof(ucRxFrame));           // flush the data
            usFrameLength -= sizeof(ucRxFrame);
        }
        fnRead(uart_handle, ucRxFrame, usFrameLength);                   // flush the data
        return -1;
    }
    fnRead(uart_handle, ucRxFrame, usFrameLength);                       // extract the complete DMX512 reception frame
    ptrRxFrame = ucRxFrame;
    #endif
    switch (*ptrRxFrame) {                                               // decide on what to do with it based on slot 0 value
    case START_CODE_DMX512:                                              // (0x00) DMX512 content
        fnHandleDMX(uart_handle, (DMX512_RDM_PACKET *)ptrRxFrame, usFrameLength);
        break;
    #define USE_DMX_RDM_SLAVE
    case START_CODE_RDM:                                                 // (0xcc) RDM content
        fnHandleRDM_SlaveRx(uart_handle, (DMX512_RDM_PACKET *)ptrRxFrame, usFrameLength, iBroadcast);
        break;
    #endif
    default:
        break;
    }
    return 0;
}


static void fnHandleDMX(QUEUE_HANDLE uart_handle, DMX512_RDM_PACKET *ptrPacket, unsigned short usPacketLength)
{
    // To do
    //
}

    #if defined USE_DMX_RDM_SLAVE
static void fnDMX512_discoveryResponse(QUEUE_HANDLE uart_handle, unsigned char ucLowerBound[6], unsigned char ucUpperBound[6])
{
    DMX512_RDM_DISC_UNIQUE_BRANCH_RESPONSE_PACKET disc_unique_branch_response;
    unsigned char *ptrContent;
    int i = 0;
    unsigned short usCheckSum;
    while (i < 6) {
        if (ucSlaveSourceUID[i] < ucLowerBound[i]) {
            return;
        }
        if (ucSlaveSourceUID[i] > ucUpperBound[i]) {
            return;
        }
        i++;
    }
    // The slave's UID is within the lower/upper bound region and so it returns a response
    //
    ptrContent = disc_unique_branch_response.ucManufacturerID;
    uMemset(disc_unique_branch_response.ucPreamble, DISC_UNIQUE_BRANCH_PREAMBLE, sizeof(disc_unique_branch_response.ucPreamble)); // insert 7 x 0xfe response preamble
    disc_unique_branch_response.ucPreambleSeparator = DISC_UNIQUE_PREAMBLE_SEPARATOR; // 0xaa
    i = 0;
    while (i < 6) {                                                      // encode the UID
        *ptrContent++ = (unsigned char)(ucSlaveSourceUID[i] | 0xaa);
        *ptrContent++ = (unsigned char)(ucSlaveSourceUID[i] | 0x55);
        i++;
    }
    usCheckSum = fnDMX512_RDM_checksum((DMX512_RDM_PACKET *)disc_unique_branch_response.ucManufacturerID, 12);                                        
    *ptrContent++ = (unsigned char)((usCheckSum >> 8) | 0xaa);           // encode the checksum
    *ptrContent++ = (unsigned char)((usCheckSum >> 8) | 0x55);
    *ptrContent++ = (unsigned char)(usCheckSum | 0xaa);
    *ptrContent = (unsigned char)(usCheckSum | 0x55);
    fnWrite(uart_handle, (unsigned char *)&disc_unique_branch_response, sizeof(disc_unique_branch_response)); // queue the response
    uDisable_Interrupt();                                                // protect against the turnaround timer interrupt
        if (iTurnAroundDelaySlave != 0) {                                // if the turnaround timer hasn't yet fired
            iTurnAroundDelaySlave = 2;                                   // allow the turnaround timer to start the transmission as soon as it fires
        }
        else {
            i = 0;                                                       // the turn around delay has already expired we we will release the message now
        }
    uEnable_Interrupt();
    if (i == 0) {                                                        // if the turnaroudn time has already expired
        DMX512_SLAVE_DRIVE_OUTPUT();                                     // switch to transmission mode
        fnDriver(uart_handle, (TX_ON | PAUSE_TX), MODIFY_TX);            // release paused output buffer
    }
}

// This function handles a valid RDM packet which has been addressed to us, either as unicast or as broadcast
//
static void fnHandleRDM_SlaveRx(QUEUE_HANDLE uart_handle, DMX512_RDM_PACKET *ptrPacket, unsigned short usPacketLength, int iBroadcast)
{
    DMX512_MESSAGE_DATA_BLOCK *ptrDataBlock = (DMX512_MESSAGE_DATA_BLOCK *)ptrPacket->ucMessageDataBlock;
    unsigned short usPID = ((ptrDataBlock->ucParameterID[0] << 8) | (ptrDataBlock->ucParameterID[1]));
    switch (ptrDataBlock->ucCommandClass) {
    case DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND:
        switch (usPID) {
        case DMX512_RDM_PARAMETER_ID_DISC_UNIQUE_BRANCH:
            if (ptrDataBlock->ucParameterDataLength == 0x0c) {           // check the fixed length of the bounds is correct
                if ((ucMuteFlag[ptrPacket->ucPortID_ResponseType/8] & (1 << (ptrPacket->ucPortID_ResponseType%8))) == 0) { // ignore if muted
                    fnDMX512_discoveryResponse(uart_handle, ptrDataBlock->ucParameterData, (ptrDataBlock->ucParameterData + 6)); // respond if the discovery boundary matches our UID
                }
            }
            break;
        case DMX512_RDM_PARAMETER_ID_DISC_MUTE:                          // mute on this port
            ucMuteFlag[ptrPacket->ucPortID_ResponseType/8] |= (1 << (ptrPacket->ucPortID_ResponseType%8));
            break;
        case DMX512_RDM_PARAMETER_ID_DISC_UNMUTE:                        // unmute on this port 
            ucMuteFlag[ptrPacket->ucPortID_ResponseType/8] &= ~(1 << (ptrPacket->ucPortID_ResponseType%8));
            break;
        default:
            return;
        }
        break;
    default:
        return;
    }
    if (iBroadcast == 0) {                                               // when addressed as unicast there is a response returned
        fnSend_DMX512_RDM(uart_handle, (ptrDataBlock->ucCommandClass + 1), usPID, (const unsigned char *)ptrPacket->ucDestinationUID, ptrPacket); // return response
    }
}

// The slave's turnaround timer interrupt
//
static void fnRDM_turnaround(void)
{
    if (iTurnAroundDelaySlave > 1) {                                     // if the slave has prepared data in the meantime
        DMX512_SLAVE_DRIVE_OUTPUT();                                     // switch to transmission mode
        fnDriver(DMX512_Slave_PortID, (TX_ON | PAUSE_TX), MODIFY_TX);    // release paused output buffer
    }
    iTurnAroundDelaySlave = 0;                                           // slave may transmit now if it wasn't ready
}

// Interrupt callback on termination of slave transmit frame
//
static void fnDMX512_Slave_TxFrameTermination(QUEUE_HANDLE channel)
{
    DMX512_SET_SLAVE_INPUT();                                            // stop driving the bus
}

// Due to strict timing requirements the receiver handles each individual reception interrupt directly
//
static int fnDMX512_slave_rx(unsigned char data, QUEUE_HANDLE channel)
{
    if (iRxCount < sizeof(ucRxBuffer)) {                                 // as long as we have space in the reception buffer
        ucRxBuffer[iRxCount] = data;                                     // store the data byte into the input buffer
        if (iRxCount == 0) {                                             // if this is the first byte in a frame
            ucType = data;                                               // the first byte indicates the frame type
            if (ucType == DISC_UNIQUE_BRANCH_PREAMBLE) {                 // ignore pre-ambles
                return 0;
            }
        }
        else if (iRxCount == 2) {                                        // the message length
            if (ucType == START_CODE_RDM) {
                usThisLength = (data + 1);                               // expected length, including CRC
            }
        }
        else {
            switch (ucType) {                                            // the frame type being received
            case START_CODE_DMX512:                                      // receiving DMX512 data
                if (iRxCount == sizeof(ucRxBuffer)) {                    // expect fixed full-size frames
                    usThisLength = sizeof(ucRxBuffer);
                    fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_DMX_RECEPTION_READY)); // inform the task that it should process the content
                }
                break;
            case DISC_UNIQUE_PREAMBLE_SEPARATOR:
                if (17 == (unsigned short)iRxCount) {                    // if a completedisc unique branch packet has been received
                }
                break;
            case START_CODE_RDM:
                if (usThisLength == (unsigned short)iRxCount) {          // if a complete RDM packet has been received
                    int iUnicast = (uMemcmp(ucSlaveSourceUID, &ucRxBuffer[3], 6) == 0); // compare the destination address with our unicast address
                    if ((iUnicast != 0) || (uMemcmp(cucBroadcast, &ucRxBuffer[3], 6) == 0)) { // if the slave is being addressed or a broadcast
                        unsigned short usCheckSum = (ucRxBuffer[usThisLength - 1] << 8);
                        usCheckSum |= ucRxBuffer[usThisLength];
                        if (usCheckSum == fnDMX512_RDM_checksum((DMX512_RDM_PACKET *)ucRxBuffer, (usThisLength - 1))) { // check that the checksum is correct
                            usThisLength++;
                            iTurnAroundDelaySlave = 1;                   // no transmission is allowed until the minimum turnaround time has elapsed
                            fnStartDelay(PIT_US_DELAY(200), fnRDM_turnaround); // 176us delay must be respected until the slave start sending either a packet or the break before it
                            if (iUnicast != 0) {                         // handle unicast
                                fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_RDM_RECEPTION_READY)); // inform the task that it should process the content and send a reply
                            }
                            else {
                                fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_RDM_BROADCAST_RECEPTION_READY)); // inform the task that it should process the broadcast content and not send a reply
                            }
                        }
                    }
                    iRxCount = 0;
                    return 0;
                }
                break;
            default:
                break;
            }
        }
    }
    iRxCount++;
    return 0;                                                            // reception handled and no standard handling required
}

static int fnDMX512_break_rx(QUEUE_HANDLE channel)
{
    usThisLength = (unsigned short)iRxCount;                             // the data bytes received in previous frame
    iRxCount = 0;                                                        // reset for next frame
    return 0;                                                            // break handled and no standard handling required
}
    #endif

    #if defined USE_DMX_RDM_MASTER || defined USE_DMX_RDM_SLAVE
static unsigned short fnDMX512_RDM_checksum(DMX512_RDM_PACKET *ptrPacket, unsigned short usPacketLength)
{
    unsigned char *ptr_ucContent = (unsigned char *)ptrPacket;
    unsigned short usCheckSum = 0;
    while (usPacketLength-- >= 2) {
        usCheckSum += *ptr_ucContent++;
    }
    return usCheckSum;
}

static int fnSend_DMX512_RDM(QUEUE_HANDLE uart_handle, unsigned char ucCommandClass, unsigned short usParameterID, const unsigned char ucDestinationUID[6], DMX512_RDM_PACKET *ptrPacket)
{
    if (fnWrite(uart_handle, 0, (DMX_SLOT_COUNT + 1)) > 0) {             // if there is free buffer space
        int iLength;
        unsigned short usCheckSum;
        unsigned char ucDMX512_RDM_buffer[DMX_SLOT_COUNT + 1];           // temporary transmit buffer for constructing the command
        DMX512_RDM_PACKET *ptrRDMpacket = (DMX512_RDM_PACKET *)&ucDMX512_RDM_buffer[0];
        DMX512_MESSAGE_DATA_BLOCK *ptrDataBlock = (DMX512_MESSAGE_DATA_BLOCK *)&ptrRDMpacket->ucMessageDataBlock[0];
        unsigned char *ptr_ucData = ptrDataBlock->ucParameterData;
        unsigned char *ptrCheckSum = ptr_ucData;
        int iSlave = ((ucCommandClass & 0x01) != 0);
        ptrRDMpacket->ucStartCode = START_CODE_RDM;
        ptrRDMpacket->ucSubStartCode = SUB_START_CODE_MESSAGE;
        uMemcpy(ptrRDMpacket->ucDestinationUID, ucDestinationUID, 6);
        if (iSlave != 0) {
    #if defined USE_DMX_RDM_SLAVE
            uMemcpy(ptrRDMpacket->ucSourceUID, ucSlaveSourceUID, 6);     // add out source UID
            ptrRDMpacket->ucTransactionNumber = ptrPacket->ucTransactionNumber; // respond with the same transaction number that the master sent
            ptrRDMpacket->ucPortID_ResponseType = ptrPacket->ucPortID_ResponseType; // respond from the same port ID that the matser sent
    #endif
        }
        else {
    #if defined USE_DMX_RDM_MASTER
            uMemcpy(ptrRDMpacket->ucSourceUID, ucMasterSourceUID, 6);
            ptrRDMpacket->ucTransactionNumber = ++ucTransactionNumber;   // increment the transaction number at each transmission
            ptrRDMpacket->ucPortID_ResponseType = ucPortID;
    #endif
        }
        ptrRDMpacket->ucMessageCount = 0x00;
        ptrDataBlock->ucParameterDataLength = 0x00;
        switch (ucCommandClass) {
        case DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND:
        case DMX512_RDM_COMMAND_CLASS_DISCOVERY_COMMAND_RESPONSE:
            ptrRDMpacket->ucSubDevice[0] = ptrRDMpacket->ucSubDevice[1] = 0; // discovery messages shall always be addressed to root devices (0)
            break;
        }
        ptrDataBlock->ucCommandClass = ucCommandClass;
        ptrDataBlock->ucParameterID[0] = (unsigned char)(usParameterID >> 8);
        ptrDataBlock->ucParameterID[1] = (unsigned char)(usParameterID);
        switch (usParameterID) {
        case DMX512_RDM_PARAMETER_ID_DISC_MUTE:                          // parameter IDs with no parameter data (master)
        case DMX512_RDM_PARAMETER_ID_DISC_UNMUTE:
    #if defined USE_DMX_RDM_SLAVE
            if (iSlave != 0) {
                unsigned short usControlField = 0;                       // control field value
                ptrDataBlock->ucParameterDataLength = 0x02;              // optional binding UID not used
                ptrRDMpacket->ucMessageCount = 1;
                *ptr_ucData = (unsigned char)(usControlField >> 8);
                *(ptr_ucData + 1) = (unsigned char)(usControlField);
                ptrCheckSum = (ptr_ucData + 2);
            }
    #endif
            break;
        case DMX512_RDM_PARAMETER_ID_DISC_UNIQUE_BRANCH:
    #if defined USE_DMX_RDM_MASTER
            if (iSlave == 0) {
                ptrDataBlock->ucParameterDataLength = 0x0c;
                ptrRDMpacket->ucMessageCount = 1;
                uMemcpy(ptr_ucData, ucLowerBoundUID, 6);
                uMemcpy((ptr_ucData + 6), ucUpperBoundUID, 6);
                ptrCheckSum = (ptr_ucData + 12);
            }
    #endif
            break;
        }
        iLength = (ptrCheckSum - (unsigned char *)ptrRDMpacket);
        ptrRDMpacket->ucMessageLength = (unsigned char)iLength;
        usCheckSum = fnDMX512_RDM_checksum(ptrRDMpacket, (unsigned short)iLength);
        *ptrCheckSum++ = (unsigned char)(usCheckSum >> 8);
        *ptrCheckSum = (unsigned char)usCheckSum;
        iLength += 2;
        return (fnWrite(uart_handle, ucDMX512_RDM_buffer, (QUEUE_TRANSFER)iLength)); // prepare next DMX512 RDM frame
    }
    else {
        return -1;                                                       // this must never happen in order for the smaster tate machine to operate correctly
    }
}
    #endif

    #if defined USE_DMX_RDM_MASTER
// Function that causes the master to start DMX512 RDM discovery
//
extern void fnDMX512_discover(void)
{
    ucDMX512_state = DMX512_RDM_DISCOVERY;                               // start discovery
}

// It is possible that slaves responded so we extract reception and handle its content here
//
static int fnHandleRDM_MasterRx(QUEUE_HANDLE uart_handle, unsigned char ucUID[6], unsigned short usExpected)
{
    unsigned char ucRDM_response[257];                                   // maximum possible message size
    int iResult = RDM_MASTER_NO_RECEPTION;                               // assume no reception
    int iLength = 0;
    while (fnRead(uart_handle, &ucRDM_response[iLength], 1) != 0) {      // extract the DMX512 reception frame one byte at a time
        iResult = RDM_MASTER_RECEPTION_DETECTED;                         // reception indicates that there was somethng received
        if (iLength == 0) {
            if (ucUID != 0) {                                            // response to a discovery
                if (ucRDM_response[0] == DISC_UNIQUE_PREAMBLE_SEPARATOR) { // pre-amble separator [0xaa] (this must be detected for the frame to be decoded)
                    unsigned short usCheckSum;
                    iLength = 1;                                         // synchronised
                    if (fnRead(uart_handle, &ucRDM_response[0], 16) != 16) { // read out the fixed length discovery response
                        break;                                           // invalid size received
                    }
                    // Decode the data
                    //
                    ucUID[0] = (ucRDM_response[0] & ucRDM_response[1]);
                    ucUID[1] = (ucRDM_response[2] & ucRDM_response[3]);
                    ucUID[2] = (ucRDM_response[4] & ucRDM_response[5]);
                    ucUID[3] = (ucRDM_response[6] & ucRDM_response[7]);
                    ucUID[4] = (ucRDM_response[8] & ucRDM_response[9]);
                    ucUID[5] = (ucRDM_response[10] & ucRDM_response[11]);
                    usCheckSum = ((ucRDM_response[12] & ucRDM_response[13]) << 8);
                    usCheckSum |= (ucRDM_response[14] & ucRDM_response[15]);
                    if (usCheckSum == fnDMX512_RDM_checksum((DMX512_RDM_PACKET *)ucUID, 6)) {
                        // Valid data found
                        //
                        iResult = RDM_MASTER_VALID_RECEPTION;            // success
                        break;
                    }
                    break;
                }
                else if (ucRDM_response[0] != DISC_UNIQUE_BRANCH_PREAMBLE) { // pre-amble [0xfe] (there can be one or more pre-amble bytes)
                    break;
                }
            }
            else {
                if (ucRDM_response[0] != START_CODE_RDM) {               // if there is no valid RDM start code as first byte
                    break;
                }
                else {
                    iLength = 1;                                         // synchronised
                }
            }
        }
        else {
            if (iLength++ == 2) {                                        // if the message length is ready
                unsigned short usChecksum;
                unsigned char usReceivedChecksum;
                if (fnRead(uart_handle, &ucRDM_response[3], ucRDM_response[2]) != ucRDM_response[2]) { // read out the advertised length
                    break;                                               // invalid size received
                }
                // Check the check sum of the received message
                //
                usReceivedChecksum = (ucRDM_response[ucRDM_response[2]] << 8);
                usReceivedChecksum |= (ucRDM_response[ucRDM_response[2] + 1]);
                usChecksum = fnDMX512_RDM_checksum((DMX512_RDM_PACKET *)ucRDM_response, ucRDM_response[2]);
                if (usReceivedChecksum == usChecksum) {
                    unsigned short usPID;
                    DMX512_RDM_PACKET *ptrRDM_packet = (DMX512_RDM_PACKET *)ucRDM_response;
                    DMX512_MESSAGE_DATA_BLOCK *ptrDataBlock = (DMX512_MESSAGE_DATA_BLOCK *)ptrRDM_packet->ucMessageDataBlock;
                    usPID = (ptrDataBlock->ucParameterID[0] << 8);
                    usPID |= (ptrDataBlock->ucParameterID[1]);
                    if (usExpected == usPID) {
                        switch (usPID) {
                        case DMX512_RDM_PARAMETER_ID_DISC_MUTE:
                            if (ptrDataBlock->ucParameterDataLength == 2) { // control field in the response

                            }
                            else if (ptrDataBlock->ucParameterDataLength == 8) { // control filed plus optional UI binding

                            }
                            else {                                       // invalid length
                                break;
                            }
                            break;
                        }
                        iResult = RDM_MASTER_VALID_RECEPTION;            // valid message content
                    }
                }
                break;
            }
        }
    }
    fnFlush(SerialPortID, FLUSH_RX);                                     // ensure receiver is empty for next use
    return iResult;
}
    #endif
#endif

/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uCANopenApp.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined CAN_INTERFACE && defined SUPPORT_CANopen

#include "CO_OD.c"                                                       // include the CANopen object directory used by this application

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                            TASK_CANOPEN

#define CANOPEN_RX_NODE_ID                  0x7a                         // we receive this node-ID
#define CANOPEN_TX_NODE_ID                  0x37                         // this is the node-ID hat we use when transmitting

extern void fnDisplayCANopen(unsigned long ulID, unsigned char *ptrData, unsigned char ucLength);

extern void tmrTask_thread(int iInstance);
extern void fnCANopenRx(int iInstance, unsigned long ulID, unsigned char *ucInputMessage, int iLength, unsigned short usTimeStamp);
extern void fnCANopenTxOK(int iInstance);
extern int uCANopenInit(int iInstance, QUEUE_HANDLE CAN_interface_ID, unsigned char ucNodeID);
extern void uCANopenPoll(int iInstance, unsigned long ulCAN_ms);
#define E_TIMER_CAN_MS                      1

#define OPENCAN_DATA_TYPE_SDO               1
#define OPENCAN_DATA_TYPE_NMT_CONTROL       2
#define OPENCAN_DATA_TYPE_NMT_MONITOR       3

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

static QUEUE_HANDLE fnInitCANopenInterface(void);
static unsigned long ulCANopen_ms = 0;


/* =================================================================== */
/*                                task                                 */
/* =================================================================== */

QUEUE_HANDLE CANopen_interface_ID0 = NO_ID_ALLOCATED;

// uCANopen task
//
extern void fnTaskCANopen(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_MESSAGE];                         // reserve space for receiving messages
    QUEUE_TRANSFER Length;

    if (NO_ID_ALLOCATED == CANopen_interface_ID0) {                      // first call
        fnDebugMsg("CANopen ");
        CANopen_interface_ID0 = fnInitCANopenInterface();
        if (uCANopenInit(0, CANopen_interface_ID0, CANOPEN_TX_NODE_ID) != 0) { // initialise the CANopen stack
            fnDebugMsg("failed\r\n");
        }
        else {
            fnDebugMsg("initialised\r\n");
            uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.001 * SEC), E_TIMER_CAN_MS); // start a 1ms timer
            uTaskerStateChange(OWN_TASK, UTASKER_POLLING);               // set the task to polling mode
        }
        return;                                                          // initialisation complete (we return when polling, for CAN receptions and at 1ms timer eent)
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on source
        case TIMER_EVENT:
            uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.001 * SEC), E_TIMER_CAN_MS); // start next 1ms timer
            ulCANopen_ms++;
            tmrTask_thread(0);                                           // 1ms rate
            break;
        case INTERRUPT_EVENT:                                            // interrupt event without data
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
            case CAN_TX_REMOTE_ERROR:
                fnDebugMsg("CAN TX REMOTE ERROR: ");
                Length = fnRead(CANopen_interface_ID0, ucInputMessage, GET_CAN_TX_REMOTE_ERROR); // read error
                if (Length != 0) {
                    int i = 0;
                    while (Length--) {                                   // display received message
                        fnDebugHex(ucInputMessage[i++], (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
                    }
                }
                fnDebugMsg("\r\n");
                break;

            case CAN_TX_ERROR:                                           // no response to a message we sent
                fnDebugMsg("CAN TX ERROR: ");
                Length = fnRead(CANopen_interface_ID0, ucInputMessage, GET_CAN_TX_ERROR); // read error
                if (Length != 0) {
                    int i = 0;
                    while (Length--) {                                   // display received message
                        fnDebugHex(ucInputMessage[i++], (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
                    }
                }
                fnDebugMsg("\r\n");
                break;

            case CAN_OTHER_ERROR:                                        // other non-categorised error
                fnDebugMsg("CAN 2\r\n");
                break;

            case CAN_TX_ACK:                                             // a CAN message was successfully sent
                fnCANopenTxOK(0);
                fnDebugMsg("CAN TX OK\r\n");
                break;

            case CAN_TX_REMOTE_ACK:                                      // a remote CAN message was successfully sent
                fnDebugMsg("CAN remote TX OK\r\n");
                break;

            case CAN_RX_REMOTE_MSG:
                Length = fnRead(CANopen_interface_ID0, ucInputMessage, (GET_CAN_RX_REMOTE | GET_CAN_RX_TIME_STAMP | GET_CAN_RX_ID)); // read received CAN message
                // Fall through intentional
                //
            case CAN_RX_MSG:                                             // a CAN message is waiting
                if (ucInputMessage[MSG_INTERRUPT_EVENT] == CAN_RX_MSG) {
                    Length = fnRead(CANopen_interface_ID0, ucInputMessage, (GET_CAN_RX_TIME_STAMP | GET_CAN_RX_ID)); // read received CAN message
                }
                if (Length != 0) {
                    int i = 0;
                    unsigned long ulID;
                    unsigned short usTimeStamp;

                    if ((ucInputMessage[i] & CAN_MSG_RX) != 0) {
                        fnDebugMsg("CAN RX");
                    }
                    else {
                        fnDebugMsg("CAN REMOTE RX");
                    }
                    if ((ucInputMessage[i++] & CAN_RX_OVERRUN) != 0) {
                        fnDebugMsg(" [OVERRUN!!]");
                    }
                    fnDebugMsg(": TimeStamp = ");
                    usTimeStamp = ucInputMessage[i++];
                    usTimeStamp <<= 8;
                    usTimeStamp |= ucInputMessage[i++];
                    fnDebugHex(usTimeStamp, (WITH_LEADIN | WITH_TERMINATOR | sizeof(usTimeStamp)));
                    fnDebugMsg(" ID = ");
                    ulID = ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    if (Length > 7) {
                        Length -= 7;                                     // remove info to leave data length
                    }
                    else {
                        Length = 0;
                    }
                    fnDisplayCANopen(ulID, &ucInputMessage[i], (unsigned char)Length);
                    fnCANopenRx(0, ulID, &ucInputMessage[i], Length, usTimeStamp);  // handle reception at CANopen layer
                }
                fnDebugMsg("\r\n");
                break;
            }
            break;
        default:
            break;
        }
    }
    uCANopenPoll(0, ulCANopen_ms);                                       // CANopen polling
}

static void fnDisplayData(unsigned char *ptrData, QUEUE_TRANSFER Length, int iDataType)
{
    switch (iDataType) {
    case OPENCAN_DATA_TYPE_NMT_MONITOR:
        fnDebugMsg(" \x22");
        switch (*ptrData) {
        case 0x00:
            fnDebugMsg("Boot up");
            break;
        case 0x04:
            fnDebugMsg("Stopped");
            break;
        case 0x7f:
            fnDebugMsg("Pre-");
            // Fall through intentionally
            //
        case 0x05:
            fnDebugMsg("Operational");
            break;
        default:
            fnDebugMsg("?");
            break;
        }
        fnDebugMsg("\x22");
        break;
    case OPENCAN_DATA_TYPE_NMT_CONTROL:
        fnDebugMsg(" Node");
        fnDebugHex(*(ptrData + 1), (WITH_SPACE | WITH_LEADIN | sizeof(unsigned char)));
        fnDebugMsg(" \x22Go to '");
        switch (*ptrData) {
        case 0x02:
            fnDebugMsg("stopped");
            break;
        case 0x80:
            fnDebugMsg("pre-");
            // Fall through intentionally
            //
        case 0x01:
            fnDebugMsg("operational");
            break;
        case 0x81:
            fnDebugMsg("reset node");
            break;
        case 0x82:
            fnDebugMsg("reset communication");
            break;
        default:
            fnDebugMsg("?");
            break;
        }
        fnDebugMsg("'\x22");
        break;
    case OPENCAN_DATA_TYPE_SDO:
        {
            unsigned char ucSubIndex;
            unsigned char ucCommand = *ptrData++;
            unsigned short usOD = *ptrData++;
            usOD |= (*ptrData++ << 8);
            ucSubIndex = *ptrData++;
            fnDebugMsg(" ");
            switch ((ucCommand >> 5)) {                                  // CCS - client command specifier
            case 0:
                fnDebugMsg("segment download");
                break;
            case 1:
                fnDebugMsg("initiating download");
                break;
            case 2:
                fnDebugMsg("initiating upload");
                break;
            case 3:
                fnDebugMsg("segment upload");
                break;
            case 4:
                fnDebugMsg("aborting");
                break;
            case 5:
                fnDebugMsg("block upload");
                break;
            case 6:
                fnDebugMsg("block download");
                break;
            default:
                fnDebugMsg("?");
                break;
            }
            if ((ucCommand & 0x02) != 0) {                               // all data in single frame
                fnDebugMsg(" expedited");
                if ((ucCommand & 0x01) != 0) {                           // number of bytes [in data 4..7] that do not contain data    
                    fnDebugDec(((ucCommand >> 2) & 0x3), WITH_SPACE);
                }
            }
            fnDebugMsg(" OD =");
            fnDebugHex(usOD, (WITH_LEADIN | WITH_SPACE | sizeof(usOD)));
            fnDebugMsg(":");
            fnDebugHex(ucSubIndex, (sizeof(ucSubIndex)));
            Length -= 4;
        }
        break;
    default:
        break;
    }
    fnDebugMsg(" Data =");
    while (Length-- != 0) {                                              // display received message
        fnDebugHex(*ptrData++, (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
    }
}

extern void fnDisplayCANopen(unsigned long ulID, unsigned char *ptrData, unsigned char ucLength)
{
    int iDataType = 0;
    fnDebugMsg("0x");
    if ((ulID & CAN_EXTENDED_ID) != 0) {
        fnDebugHex((ulID & ~CAN_EXTENDED_ID), (WITH_TERMINATOR | sizeof(ulID)));
    }
    else {
        CHAR cBuf[5];
        fnBufferHex(ulID, (WITH_TERMINATOR | sizeof(unsigned short)), cBuf);
        fnDebugMsg(&cBuf[1]);
    }
    fnDebugMsg(" [Node-ID = ");
    fnDebugHex((ulID & 0x7f), (WITH_LEADIN | WITH_TERMINATOR | sizeof(unsigned char)));
    fnDebugMsg(" COM-Object: ");
    switch ((ulID >> 7) & 0x0f) {
    case 0x0:                                            // receive only at slave
        fnDebugMsg("NMT control");
        iDataType = OPENCAN_DATA_TYPE_NMT_CONTROL;
        break;
    case 0x1:
        if ((ulID & 0x7f) == 0) {
            fnDebugMsg("SYNC");                          // receive only at slave
        }
        else {
            fnDebugMsg("EMCY");                          // slave transmits
        }
        break;
    case 0x2:
        fnDebugMsg("TIME");                              // receive only at slave
        break;
    case 0x3:
        fnDebugMsg("Tx PDO 1");                          // slave transmission
        break;
    case 0x4:
        fnDebugMsg("Rx PDO 1");                          // slave reception
        break;
    case 0x5:
        fnDebugMsg("Tx PDO 2");                          // slave transmission
        break;
    case 0x6:
        fnDebugMsg("Rx PDO 2");                          // slave reception
        break;
    case 0x7:
        fnDebugMsg("Tx PDO 3");                          // slave transmission
        break;
    case 0x8:
        fnDebugMsg("Rx PDO 3");                          // slave reception
        break;
    case 0x9:
        fnDebugMsg("Tx PDO 4");                          // slave transmission
        break;
    case 0xa:
        fnDebugMsg("Rx PDO 4");                          // slave reception
        break;
    case 0xb:
        fnDebugMsg("Tx SDO");                            // slave transmission
        iDataType = OPENCAN_DATA_TYPE_SDO;
        break;
    case 0xc:
        fnDebugMsg("Rx SDO");                            // slave reception
        iDataType = OPENCAN_DATA_TYPE_SDO;
        break;
    case 0xe:
        if ((ulID & 0x7f) == 0xe4) {
            fnDebugMsg("LSS Tx");                        // slave transmission
        }
        else if ((ulID & 0x7f) == 0xe5) {
            fnDebugMsg("LSS Rx");                        // slave reception
        }
        else {
            fnDebugMsg("NMT monitor");                   // slave transmission
            iDataType = OPENCAN_DATA_TYPE_NMT_MONITOR;
        }
        break;
    default:
        fnDebugMsg("?");                                 // unknown function code
        break;
    }
    fnDebugMsg("]");
    if (ucLength > 0) {
        fnDisplayData(ptrData, ucLength, iDataType);
    }
    else {
        fnDebugMsg(" No Data");
    }
}

// Configure the CAN interface
//
static QUEUE_HANDLE fnInitCANopenInterface(void)
{
    CANTABLE tCANParameters;                                             // table for passing information to driver
    QUEUE_HANDLE CANopenHandle;

    tCANParameters.Task_to_wake = OWN_TASK;                              // wake us on buffer events
    tCANParameters.Channel = 0;                                          // CAN0 interface
    tCANParameters.ulSpeed = 250000;                                     // 250k speed
    tCANParameters.ulTxID = (121);                                       // default ID of destination (not used by CANopen)
    tCANParameters.ulRxID = (CAN_EXTENDED_ID | 0x00080000 | CANOPEN_RX_NODE_ID); // extended node ID that we receive
    tCANParameters.ulRxIDMask = (CAN_EXTENDED_MASK | 0x00080000 | CANOPEN_RX_NODE_ID); // receive extended address with 0x80000 set and exactly matching the node ID
    tCANParameters.usMode = 0;                                           // use normal mode
    tCANParameters.ucTxBuffers = 2;                                      // assign two tx buffers for use
    tCANParameters.ucRxBuffers = 1;                                      // assign one rx buffers for extended ID use
    CANopenHandle = fnOpen(TYPE_CAN, FOR_I_O, &tCANParameters);          // open CAN interface
    tCANParameters.ulRxID = 0;                                           // broadcast address
    tCANParameters.ulRxIDMask = CAN_STANDARD_MASK;                       // accept ID 0
    tCANParameters.ucTxBuffers = 0;
    tCANParameters.ucRxBuffers = 1;
    fnConfigCAN(CANopenHandle, &tCANParameters);                         // configure 1 buffer for this logical channel
    tCANParameters.ulRxID = CANOPEN_RX_NODE_ID;                          // node ID that we receive
    tCANParameters.ulRxIDMask = 0x7f;                                    // accept only exact node ID
    tCANParameters.ucRxBuffers = 1;
    fnConfigCAN(CANopenHandle, &tCANParameters);                         // configure 1 buffer for this logical channel
    return CANopenHandle;                                                // open CAN interface
}
#endif


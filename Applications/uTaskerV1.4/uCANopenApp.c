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

#define CANOPEN_NODE_ID                     10

extern void tmrTask_thread(void);
extern void fnCANopenRx(int iCanRef, unsigned long ulID, unsigned char *ucInputMessage, int iLength, unsigned short usTimeStamp);
extern void fnCANopenTxOK(int iCanRef);
extern int uCANopenInit(QUEUE_HANDLE CAN_interface_ID, unsigned char ucNodeID);
extern int uCANopenPoll(QUEUE_HANDLE CAN_interface_ID);
#define E_TIMER_CAN_MS                      1

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

/* =================================================================== */
/*                                task                                 */
/* =================================================================== */

QUEUE_HANDLE CANopen_interface_ID0 = NO_ID_ALLOCATED;

// uCANopen task
//
extern void fnTaskCANopen(TTASKTABLE *ptrTaskTable)
{
  //static QUEUE_HANDLE CANopen_interface_ID0 = NO_ID_ALLOCATED;
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_MESSAGE];                         // reserve space for receiving messages
    QUEUE_TRANSFER Length;

    if (NO_ID_ALLOCATED == CANopen_interface_ID0) {                      // first call
        CANopen_interface_ID0 = fnInitCANopenInterface();
        if (uCANopenInit(CANopen_interface_ID0, CANOPEN_NODE_ID) != 0) { // initialise the CANopen stack
            fnDebugMsg("CANopen failed\r\n");
        }
        else {
            uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.001 * SEC), E_TIMER_CAN_MS); // start a 1ms timer
            uTaskerStateChange(OWN_TASK, UTASKER_POLLING);               // set the task to polling mode
        }
        return;
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on source
        case TIMER_EVENT:
            uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.001 * SEC), E_TIMER_CAN_MS); // start next 1ms timer
            tmrTask_thread();                                            // 1ms rate
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
                    int iLength;
                    unsigned long ulID;
                    unsigned short usTimeStamp;
                    unsigned char *ptrData;

                    if (ucInputMessage[i] & CAN_MSG_RX) {
                        fnDebugMsg("CAN RX");
                    }
                    else {
                        fnDebugMsg("CAN REMOTE RX");
                    }

                    if (ucInputMessage[i++] & CAN_RX_OVERRUN) {
                        fnDebugMsg(" [OVERRUN!!]");
                    }

                    fnDebugMsg(": TimeStamp = ");
                    usTimeStamp = ucInputMessage[i++];
                    usTimeStamp <<= 8;
                    usTimeStamp |= ucInputMessage[i++];
                    fnDebugHex(usTimeStamp, (WITH_LEADIN | WITH_TERMINATOR | 2));

                    fnDebugMsg(" ID = ");

                    ulID = ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];
                    ulID <<= 8;
                    ulID |= ucInputMessage[i++];

                    if ((ulID & CAN_EXTENDED_ID) != 0) {
                        fnDebugHex((ulID & ~CAN_EXTENDED_ID), (WITH_LEADIN | WITH_TERMINATOR | 4));
                    }
                    else {
                        fnDebugHex(ulID, (WITH_LEADIN | WITH_TERMINATOR | 2));
                    }
                    ptrData = &ucInputMessage[i];

                    if (Length > 7) {
                        Length -= 7;                                     // remove info to leave data length
                        iLength = Length;
                        fnDebugMsg(" Data =");

                        while (Length-- != 0) {                          // display received message
                            fnDebugHex(ucInputMessage[i++], (WITH_LEADIN | WITH_TERMINATOR | WITH_SPACE | 1));
                        }
                    }
                    else {
                        iLength = 0;
                        fnDebugMsg(" No Data");
                    }
                    fnCANopenRx(0, ulID, ptrData, iLength, usTimeStamp);
                }
                fnDebugMsg("\r\n");
                break;
            }
            break;
        default:
            break;
        }
    }
    uCANopenPoll(CANopen_interface_ID0);                                 // polling
}

static QUEUE_HANDLE fnInitCANopenInterface(void)
{
    CANTABLE tCANParameters;                                             // table for passing information to driver

    tCANParameters.Task_to_wake = OWN_TASK;                              // wake us on buffer events
    tCANParameters.Channel = 0;                                          // CAN0 interface
    tCANParameters.ulSpeed = 250000;                                     // 250k speed
    tCANParameters.ulTxID = (CAN_EXTENDED_ID | 121);                     // default ID of destination (not extended)
    tCANParameters.ulRxID = (CAN_EXTENDED_ID | 0x00080000 | 122);        // our ID (extended)
    tCANParameters.ulRxIDMask = 0x00080000;
    tCANParameters.usMode = 0;                                           // use normal mode
    tCANParameters.ucTxBuffers = 2;                                      // assign two tx buffers for use
    tCANParameters.ucRxBuffers = 3;                                      // assign three rx buffers for use
    return (fnOpen(TYPE_CAN, FOR_I_O, &tCANParameters));                 // open CAN interface
}
#endif


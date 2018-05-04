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

/*****************************************************************************/
// The project includes a variety of quick tests which can be activated here
/*****************************************************************************/


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
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



// DMX512 task
//
extern void fnDMX512(TTASKTABLE *ptrTaskTable)
{
#define DMX512_UART 1
    static int iDMXState = 0;
    static QUEUE_HANDLE DMX512_PortID = NO_ID_ALLOCATED;
    QUEUE_HANDLE        PortIDInternal = ptrTaskTable->TaskID;           // queue ID for task input
    unsigned char       ucInputMessage[SMALL_QUEUE];                     // reserve space for receiving messages

    if (0 == iDMXState) {
        TTYTABLE tInterfaceParameters;                                   // table for passing information to driver
        tInterfaceParameters.Channel = DMX512_UART;
        tInterfaceParameters.ucSpeed = SERIAL_BAUD_250K;                 // fixed DMX512 baud rate
        tInterfaceParameters.Config = (CHAR_8 + NO_PARITY + TWO_STOPS + CHAR_MODE); // fixed DMX512 settings
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 1024;             // input buffer size
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = 1024;             // output buffer size
        tInterfaceParameters.Task_to_wake = OWN_TASK;                    // wake self when messages have been received
        tInterfaceParameters.ucDMAConfig = UART_TX_DMA;                  // activate DMA on transmission
        if (NO_ID_ALLOCATED == (DMX512_PortID = fnSetNewSerialMode(&tInterfaceParameters, FOR_I_O))) { // open serial port for I/O
            return;                                                      // if the serial port could not be opened we quit
        }
        else {
            iDMXState = 1;                                               // initialised
        }
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
            case 0:
                break;
            default:
                break;
            }
            break;

        default:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // flush any unexpected messages (assuming they arrived from another task)
            break;
        }
    }
}

#endif
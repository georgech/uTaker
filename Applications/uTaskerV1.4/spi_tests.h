/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_tests.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    The file is otherwise not specifically linked in to the project since it
    is included by application.c when needed.

*/

#if defined SPI_INTERFACE && !defined _SPI_CONFIG
    #define _SPI_CONFIG

    #define TEST_SPI                                                     // test SPI operation
        #define TEST_SPI_MASTER_MODE
        #define TEST_MESSAGE_TX_MODE
        #define TEST_SPI_SLAVE_MODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

    #if defined SPI_INTERFACE && defined TEST_SPI
        static void fnInitSPIInterface(void);
    #endif


/* =================================================================== */
/*                     local variable definitions                      */
/* =================================================================== */

    #if defined SPI_INTERFACE && defined TEST_SPI
        static QUEUE_HANDLE SPI_master_ID = NO_ID_ALLOCATED;
        #if defined TEST_SPI_SLAVE_MODE
        static QUEUE_HANDLE SPI_slave_ID = NO_ID_ALLOCATED;
        #endif
    #endif

#endif


#if defined _SPI_READ_CODE && defined SPI_INTERFACE && defined TEST_SPI && defined TEST_SPI_SLAVE_MODE
    if ((Length = fnMsgs(SPI_slave_ID)) != 0) {                          // if SPI reception available
        fnDebugMsg("SPI Rx:");
        fnDebugDec(Length, 0);
        while (fnRead(SPI_slave_ID, ucInputMessage, 1) != 0) {           // while reception data available
            fnDebugHex(ucInputMessage[0], (sizeof(ucInputMessage[0]) | WITH_SPACE | WITH_LEADIN));
        }
        fnDebugMsg("\r\n");
    }
#endif


#if defined _SPI_INIT_CODE && defined SPI_INTERFACE && defined TEST_SPI
static void fnInitSPIInterface(void)
{
    SPITABLE tSPIParameters;                                             // table for passing information to driver
    #if defined TEST_SPI_MASTER_MODE
        #if defined TEST_MESSAGE_TX_MODE
    static const unsigned short usTestTx1[] = { 8,   1,    1, 2, 4, 8, 16, 32, 64, 128 }; // 8 words on chip select 0 (> 8 bit words)
    static const unsigned char  ucTestTx2[] = { LITTLE_SHORT_WORD_BYTES(8), LITTLE_SHORT_WORD_BYTES(0),  1, 2, 4, 8, 16, 32, 64, 128 }; // 8 bytes on chip select 1 (<= 8 bit words)
        #else
    static const unsigned char ucTestTx[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
        #endif
    tSPIParameters.Channel = 1;                                          // SPI 1
    tSPIParameters.ucSpeed = SPI_100K;                                   // master mode at 100kb/s
        #if defined TEST_MESSAGE_TX_MODE
    tSPIParameters.Config = (SPI_PHASE | SPI_POL | SPI_TX_MULTI_MODE | SPI_TX_MESSAGE_MODE);   // transmissions are message oriented with controllable characteristics
    tSPIParameters.ucChipSelect = 0;
    tSPIParameters.ucWordWidth = 16;                                     // 16 bit words
        #else
    tSPIParameters.ucWordWidth = 8;
    tSPIParameters.Config = (SPI_PHASE | SPI_POL);
        #endif
    tSPIParameters.Rx_tx_sizes.TxQueueSize = 128;
    tSPIParameters.Rx_tx_sizes.RxQueueSize = 128;
    tSPIParameters.Task_to_wake = OWN_TASK;                              // wake us on events
    SPI_master_ID = fnOpen(TYPE_SPI, FOR_I_O, &tSPIParameters);          // open interface
        #if defined TEST_MESSAGE_TX_MODE
    tSPIParameters.ucChipSelect = 1;
    tSPIParameters.ucWordWidth = 8;
    tSPIParameters.ucSpeed = SPI_1MEG;                                   // master mode at 1Mb/s
    SPI_master_ID = fnOpen(TYPE_SPI, ADD_CONFIG, &tSPIParameters);       // add chip select 1
        #endif
    #endif
    #if defined TEST_SPI_SLAVE_MODE
    tSPIParameters.Channel = 0;                                          // SPI 0
    tSPIParameters.ucSpeed = 0;                                          // slave mode
    tSPIParameters.ucWordWidth = 8;                                      // 8 bit words
    tSPIParameters.Config = (SPI_PHASE | SPI_POL);
    tSPIParameters.Rx_tx_sizes.TxQueueSize = 128;
    tSPIParameters.Rx_tx_sizes.RxQueueSize = 128;
    tSPIParameters.Task_to_wake = OWN_TASK;                              // wake us on events
    SPI_slave_ID = fnOpen(TYPE_SPI, FOR_I_O, &tSPIParameters);           // open interface
    #endif
    #if defined TEST_SPI_MASTER_MODE
        #if defined TEST_MESSAGE_TX_MODE
    fnWrite(SPI_master_ID, (unsigned char *)usTestTx1, sizeof(usTestTx1)); // send a test transmission
    fnWrite(SPI_master_ID, (unsigned char *)ucTestTx2, sizeof(ucTestTx2)); // send a test transmission
        #else
    fnWrite(SPI_master_ID, (unsigned char *)ucTestTx, sizeof(ucTestTx)); // send a test transmission
        #endif
    #endif
}
#endif


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

  //#define TEST_SPI                                                     // test SPI operation
        #define TEST_SPI_MASTER_MODE
        #define TEST_SPI_SLAVE_MODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

    #if defined SPI_INTERFACE && defined TEST_SPI
        static void fnInitSPIInterface(void);
        #if defined TEST_SPI_MASTER_MODE
            static void fnSendSPI(int key);
        #endif
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
        fnDebugDec(Length, WITH_CR_LF);
        while (fnRead(SPI_slave_ID, ucInputMessage, 1) != 0) {           // while reception data available
        }
    }
#endif


#if defined _SPI_INIT_CODE && defined SPI_INTERFACE && defined TEST_SPI
static void fnInitSPIInterface(void)
{
    SPITABLE tSPIParameters;                                             // table for passing information to driver
    #if defined TEST_SPI_MASTER_MODE
    static const unsigned char ucTestTx[4] = { 1, 2, 3, 4 };
    tSPIParameters.Channel = 1;                                          // SPI 1
    tSPIParameters.ucSpeed = SPI_100K;                                   // master mode at 100kb/s
    tSPIParameters.Config = 0;
    tSPIParameters.Rx_tx_sizes.TxQueueSize = 128;
    tSPIParameters.Rx_tx_sizes.RxQueueSize = 128;
    tSPIParameters.Task_to_wake = OWN_TASK;                              // wake us on events
    SPI_master_ID = fnOpen(TYPE_SPI, FOR_I_O, &tSPIParameters);          // open interface
    #endif
    #if defined TEST_SPI_SLAVE_MODE
    tSPIParameters.Channel = 0;                                          // SPI 0
    tSPIParameters.ucSpeed = 0;                                          // slave mode
    tSPIParameters.Config = 0;
    tSPIParameters.Rx_tx_sizes.TxQueueSize = 128;
    tSPIParameters.Rx_tx_sizes.RxQueueSize = 128;
    tSPIParameters.Task_to_wake = OWN_TASK;                              // wake us on events
    SPI_slave_ID = fnOpen(TYPE_SPI, FOR_I_O, &tSPIParameters);           // open interface
    #endif
    #if defined TEST_SPI_MASTER_MODE
    fnWrite(SPI_master_ID, (unsigned char *)ucTestTx, sizeof(ucTestTx)); // send a test transmission
    #endif
}
#endif


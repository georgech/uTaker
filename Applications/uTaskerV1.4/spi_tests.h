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
            #define CHIP_SELECT_DEVICE_0      (1)
            #define CHIP_SELECT_DEVICE_1      (0)
            #define CHIP_SELECT_SLAVE_0       (0)
    static const unsigned short usTestTx1a[] = { 8,   CHIP_SELECT_DEVICE_0 }; // 8 words on chip select 0 (> 8 bit words)
    static const unsigned short usTestTx1b[] = { 0x0001, 0x0004, 0x0010, 0x0040, 0x0100, 0x0400, 0x1000, 0x4000 }; // 8 words on chip select 0 (> 8 bit words)
    static const unsigned char  ucTestTx2[] = { LITTLE_SHORT_WORD_BYTES(8), LITTLE_SHORT_WORD_BYTES(CHIP_SELECT_DEVICE_1),  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 }; // 8 bytes on chip select 1 (<= 8 bit words)
        #else
    static const unsigned char ucTestTx[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
        #define CHIP_SELECT_DEVICE_0      (1)
        #define CHIP_SELECT_DEVICE_1      (0)
        #define CHIP_SELECT_SLAVE_0       (0)
        #endif
    tSPIParameters.Channel = 1;                                          // SPI 1
    tSPIParameters.ucSpeed = SPI_100K;                                   // master mode at 100kb/s
        #if defined TEST_MESSAGE_TX_MODE
    tSPIParameters.Config = (SPI_TX_MULTI_MODE | SPI_TX_MESSAGE_MODE | SPI_LSB); // transmissions are message oriented with controllable characteristics
    tSPIParameters.ucWordWidth = 16;                                     // 16 bit words
        #else
    tSPIParameters.ucWordWidth = 8;
    tSPIParameters.Config = (SPI_PHASE | SPI_POL);
        #endif
    tSPIParameters.ucChipSelect = CHIP_SELECT_DEVICE_0;
    tSPIParameters.Rx_tx_sizes.TxQueueSize = 128;
    tSPIParameters.Rx_tx_sizes.RxQueueSize = 128;
    tSPIParameters.Task_to_wake = OWN_TASK;                              // wake us on events
    SPI_master_ID = fnOpen(TYPE_SPI, FOR_I_O, &tSPIParameters);          // open interface
        #if defined TEST_MESSAGE_TX_MODE
    tSPIParameters.ucChipSelect = CHIP_SELECT_DEVICE_1;
    tSPIParameters.ucWordWidth = 8;
    tSPIParameters.ucSpeed = SPI_1MEG;                                   // master mode at 1Mb/s
    tSPIParameters.Config = (SPI_PHASE | SPI_POL | SPI_TX_MULTI_MODE | SPI_TX_MESSAGE_MODE); // transmissions are message oriented with controllable characteristics
    SPI_master_ID = fnOpen(TYPE_SPI, ADD_CONFIG, &tSPIParameters);       // add chip select 1
        #endif
    #endif
    #if defined TEST_SPI_SLAVE_MODE
    tSPIParameters.Channel = 0;                                          // SPI 0
    tSPIParameters.ucSpeed = 0;                                          // slave mode
    tSPIParameters.ucWordWidth = 8;                                      // 8 bit words
    tSPIParameters.ucChipSelect = CHIP_SELECT_SLAVE_0;
    tSPIParameters.Config = (SPI_PHASE | SPI_POL);
    tSPIParameters.Rx_tx_sizes.TxQueueSize = 128;
    tSPIParameters.Rx_tx_sizes.RxQueueSize = 128;
    tSPIParameters.Task_to_wake = OWN_TASK;                              // wake us on events
    SPI_slave_ID = fnOpen(TYPE_SPI, FOR_I_O, &tSPIParameters);           // open interface
    #endif
    #if defined TEST_SPI_MASTER_MODE
        #if defined TEST_MESSAGE_TX_MODE
    fnWrite(SPI_master_ID, (unsigned char *)usTestTx1a, sizeof(usTestTx1a)); // prepare a test transmission
    fnWrite(SPI_master_ID, (unsigned char *)usTestTx1b, sizeof(usTestTx1b)); // send a test transmission

    fnWrite(SPI_master_ID, (unsigned char *)ucTestTx2, sizeof(ucTestTx2)); // send a test transmission

    fnWrite(SPI_master_ID, (unsigned char *)usTestTx1a, sizeof(usTestTx1a)); // prepare a test transmission
    fnWrite(SPI_master_ID, (unsigned char *)usTestTx1b, sizeof(usTestTx1b)); // send a test transmission
        #else
    fnWrite(SPI_master_ID, (unsigned char *)ucTestTx, sizeof(ucTestTx)); // send a test transmission
        #endif
    #endif
}
#endif


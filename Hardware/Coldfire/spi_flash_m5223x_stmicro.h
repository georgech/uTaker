/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_m5223x_stmicro.h [FREESCALE Coldfire V2 MCU]
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

    28.06.2008 Remove configuration of QSPI commands from driver interface (these are initialised once on start up) {1}
    29.06.2008 Modify _CLEARBITS macro to use positive logic             {2}
    27.10.2008 Remove unnecessary reset of danger flag                   {3}
    15.10.2009 Add extra SPI FLASH sizes and recognise any SPI FLASH size{4}
    02.10.2011 Add STORAGE_REV_1 support                                 {5}
    20.01.2013 Disable Flash protection by default                       {6}
    20.01.2013 Add CHECK_SPI_FLASH_BUSY and managed mode support         {7}

*/ 

#ifdef SPI_FLASH_ST

#ifdef _SPI_DEFINES
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckSTM25Pxxx(int iChipSelect);
        #ifdef STORAGE_REV_1                                             // {5}
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            SPI_FLASH_DEVICE_COUNT                                       // multiple devices
        };
        #endif
    #else
        static unsigned char fnCheckSTM25Pxxx(void);
        #ifdef STORAGE_REV_1                                             // {5}
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            0                                                            // not multiple devices
        };
        #endif
    #endif
#endif


    // This code is inserted to detect the presence of the SPI FLASH device(s). If the first device is not detected the SPI interface is disabled.
    // If there are multiple devices, each will be recorded
    //
#ifdef _CHECK_SPI_CHIPS
        #ifdef SPI_FLASH_MULTIPLE_CHIPS 
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx(0);                           // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx();                            // flag whether the SPI FLASH device is connected
        #endif
    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {                 // {4} we expect SPI FLASH to be detected
        QMR = 0;                                                         // disable SPI if no chip connected
    }
    else {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckSTM25Pxxx(i);
        }
        #endif
        #ifdef STORAGE_REV_1                                             // {5}
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
        #endif
    }
#endif



#ifdef _SPI_FLASH_INTERFACE
// This is the main interface code to the STMicroelectronics SPI FLASH device
//
/* =================================================================== */
/*                           STM25PXX driver                           */
/* =================================================================== */

#define CHECK_SPI_FLASH_BUSY     0x00                                    // {7} pseudo request to see whether device is ready
#define READ_STATUS_REGISTER     0x05
  #define STATUS_BUSY            0x01
  #define STATUS_WR_ENABLED      0x02
  #define STATUS_BL_PROTECT1     0x04
  #define STATUS_BL_PROTECT2     0x08
  #define STATUS_BL_PROTECT3     0x10
  #define STATUS_WRITE_PROTECT   0x80
#define WRITE_ENABLE             0x06
#define WRITE_DISABLE            0x04
#define WRITE_STATUS_REGISTER    0x01
#define READ_DATA_BYTES          0x03
#define FAST_READ                0x0B
#define PAGE_PROG                0x02
#define SECTOR_ERASE             0xd8
#define BULK_ERASE               0xc7
#define SUB_SECTOR_ERASE         0x20                                    // only available on the M25PExxx devices
#define READ_MANUFACTURER_ID     0x9f

#define MANUFACTURER_ID_ST       0x20                                    // ST manufacturer's ID

#define DEVICE_ID_1_DATA_ST_FLASH_512K 0x10                              // 512kBit / 64kByte
#define DEVICE_ID_1_DATA_ST_FLASH_1M   0x11                              // 1MBit / 128kByte
#define DEVICE_ID_1_DATA_ST_FLASH_2M   0x12                              // 2MBit / 256kByte
#define DEVICE_ID_1_DATA_ST_FLASH_4M   0x13                              // 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_8M   0x14                              // 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_16M  0x15                              // 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_32M  0x16                              // FLASH size - 32MBit / 4MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_64M  0x17                              // FLASH size - 64MBit / 8 MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_128M 0x18                              // FLASH size - 128MBit / 16 MegByte

// QSPI FLASH hardware interface
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
#else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
#endif
{
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    unsigned char ucChipSelectLine = ucChipSelect[iChipSelect];
    #define ucChipSelectLineSim 0xff
    #else
    #define ucChipSelectLine    CS0_LINE
    #define ucChipSelectLineSim CS0_LINE
    #define iChipSelect 0
    #endif
    unsigned char ucTxCount = 3;

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register 
    #ifdef MANAGED_FILES                                                 // {7}
            if (ucCommand == CHECK_SPI_FLASH_BUSY) {                     // pseudo request to see whether device is ready
                if (!(ucStatus & STATUS_BUSY)) {
                    return;                                              // the device is no longer busy
                }
                else if (--(*ucData) == 0) {
                    SPI_FLASH_Danger[iChipSelect] = 1;                   // put the busy bit back
                    return;                                              // the maximum number of status requests has expired so quit
                }
            }
    #endif
        } while ((ucStatus & STATUS_BUSY) != 0);                         // until no longer busy
    }
    #ifdef MANAGED_FILES                                                 // {7}
    else if (ucCommand == CHECK_SPI_FLASH_BUSY) {                        // pseudo command used to check device's status
        return;                                                          // the device is known to be not busy so return immediately
    }
    #endif

//  SPI_FLASH_Danger[iChipSelect] = 0;                                   // {3} (unnecessary since always zero here) device is presently not busy

    _CLEARBITS(QS, ucChipSelectLine);                                    // assert SS low before starting {2}
//  QAR = QSPI_COMMAND_RAM_0;                                            // set address to first control location {1}
//  QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0);  // no chip select since we control it via port - 8 bit access {1}
    QAR = QSPI_TRANSMIT_RAM_0;                                           // set address to first transmit location
    QDR = ucCommand;                                                     // prepare command
	#ifdef _WINDOWS
    fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);                 // simulate the SPI FLASH device
    QIR |= QSPI_SPIF;
	#endif

    switch (ucCommand) {
    case WRITE_ENABLE:                                                   // enable subsequent writes
        ucTxCount = 0;
        DataLength = 0;
        break;
    case PAGE_PROG:                                                      // program data to a page (programming started when CS line negated)
        QDR = (unsigned char)(ulPageNumberOffset >> 16);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
		#endif
        QDR = (unsigned char)(ulPageNumberOffset >> 8);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
		#endif
        QDR = (unsigned char)ulPageNumberOffset;
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
		#endif
		SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the chip will be busy for some time
        break;

    case SUB_SECTOR_ERASE:                                               // erase one sub-sector
    case SECTOR_ERASE:                                                   // erase one sector
        QDR = (unsigned char)(ulPageNumberOffset >> 16);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
		#endif
        QDR = (unsigned char)(ulPageNumberOffset >> 8);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
		#endif
        QDR = 0;                                                         // page offset is don't care
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
		#endif
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case READ_DATA_BYTES:                                                // read data from the chip
        QDR = (unsigned char)(ulPageNumberOffset >> 16);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
		#endif
        QDR = (unsigned char)(ulPageNumberOffset >> 8);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
		#endif
        QDR = (unsigned char)(ulPageNumberOffset);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
		#endif
        ucTxCount = 3;
        SPI_FLASH_Danger[iChipSelect] = 1;                               // in this case this marks that no 0xff have been set to tx when reading (it will be cleared before exiting)
        break;

    case READ_MANUFACTURER_ID:                                           // get then manufacturer and chip size information
        ucTxCount = 3;
                                                                         // fall through intentionally
    case READ_STATUS_REGISTER:                                           // read single byte from status register
		if (ucCommand == READ_STATUS_REGISTER) {
		    ucTxCount = 1;
		}
        QWR = ((0<<QSPI_START_SHIFT) | (ucTxCount<<QSPI_END_SHIFT));     // starting at queue 0 send/receive the required amount of entries
        QDLYR = QSPI_SPE;                                                // start transfer - bytes will be sent and read in
        while (!(QIR & QSPI_SPIF)) {};                                   // wait for transfer to complete
        QIR = QSPI_SPIF;                                                 // clear interrupt flag
        QAR = QSPI_RECEIVE_RAM_0;                                        // set address to first receive location
		*ucData = (unsigned char)QDR;									 // dummy read
        while (DataLength--) {
		#ifdef _WINDOWS
            QDR = fnSimSPI_Flash(STM25PXXX_READ, (unsigned char)QAR);    // simulate the SPI FLASH device
		#endif
            *ucData++ = (unsigned char)QDR;
        }
        _SETBITS(QS, ucChipSelectLine);                                  // deassert SS when complete
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
		#endif
        return;

    case WRITE_STATUS_REGISTER:                                          // {6}
        QDR = *ucData;                                                   // add single status byte
        ucTxCount = 1;
        DataLength = 0;
        SPI_FLASH_Danger[iChipSelect] = 1;                               // writes to the status register are writes to non-volatile memory and the chip will be busy for a short time
        break;

    case BULK_ERASE:
        ucTxCount = 0;
        DataLength = 0;
        break;
        // Note - no default here. This must never be called with non-supported command since it will hang.
    }

    QWR = ((0 << QSPI_START_SHIFT) | (ucTxCount << QSPI_END_SHIFT));     // starting at queue 0 send/receive entries
    QDLYR = QSPI_SPE;                                                    // start transfer - bytes will be sent and read in

    while (DataLength != 0) {                                            // while data bytes to be read or written
        int iDataCnt = 0;                                                // QSPI block counter

        if (ucTxCount != 0) {
            while (!(QIR & QSPI_SPIF)) {};                               // wait for transfer to complete
            QIR = QSPI_SPIF;                                             // clear interrupt flag
        }

      //QAR = QSPI_COMMAND_RAM_0;                                        // set address to first control location {1}
      //QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); // no chip select since we control it via port - 8 bit access {1}
        QAR = QSPI_TRANSMIT_RAM_0;                                       // set address to first transmit location
        if (ucCommand != READ_DATA_BYTES) {                              // write type
            QDR = *ucData++;                                             // prepare data
		#ifdef _WINDOWS
            fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);         // simulate the SPI FLASH device
            QIR |= QSPI_SPIF;                                            // set flag for simulator
		#endif
            while (--DataLength != 0) {                                  // for each byte in the QSPI transfer block
                QDR = *ucData++;                                         // prepare data
		#ifdef _WINDOWS
                fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)QDR);     // simulate the SPI FLASH device
		#endif
                if (++iDataCnt >= 15) {                                  // maximum QSPI transfer length reached
                    DataLength--;
                    break;
                }
            }
            QWR = ((0<<QSPI_START_SHIFT) | (iDataCnt<<QSPI_END_SHIFT));  // starting at queue 0 send/receive programmed entries
            QDLYR = QSPI_SPE;                                            // start transfer - programmed number of bytes will be sent and read in
        }
        else {                                                           // read rather than write
            ucTxCount = 0;                                               // stop polling of status at begin of loop
            if (DataLength > 15) {                                       // maximum transfer length possible
                iDataCnt = 15;
                DataLength -= 16;                                        // complete block
            }
            else {                                                       // last transfer
                iDataCnt = (DataLength - 1);                             // last transfer block
                DataLength = 0;
            }
            if (SPI_FLASH_Danger[iChipSelect] != 0) {                    // this is performed only once in a read sequence (previously always true and so not optimal)
                int iSetIdle = iDataCnt;
                while (iSetIdle-- >= 0) {
                    QDR = 0xff;                                          // fill output buffer with 0xff so that output remains '1'
                }
                SPI_FLASH_Danger[iChipSelect] = 0;                       // stop more 0xff being written
            }
            QWR = ((0<<QSPI_START_SHIFT) | (iDataCnt<<QSPI_END_SHIFT));  // starting at queue 0 send/receive programmed entries
            QDLYR = QSPI_SPE;                                            // start transfer - programmed number of bytes will be sent and read in
            while (!(QIR & QSPI_SPIF)) {};                               // wait for transfer to complete
            QIR = QSPI_SPIF;                                             // clear interrupt flag

            QAR = QSPI_RECEIVE_RAM_0;                                    // set address to first receive location
            do {                                                         // collect each byte
		#ifdef _WINDOWS
                QDR = fnSimSPI_Flash(STM25PXXX_READ, (unsigned char)QAR);// simulate the SPI FLASH device
		#endif
                *ucData++ = (unsigned char)QDR;                          // read bytes to the return buffer
            } while (iDataCnt--);
        }
    }
    if (ucCommand != READ_DATA_BYTES) {                                  // commands that have only written wait here for transmission to complete
        while (!(QIR & QSPI_SPIF)) {};                                   // wait for transfer to complete
        QIR = QSPI_SPIF;                                                 // clear interrupt flag
    }
    _SETBITS(QS, ucChipSelectLine);                                      // deassert SS when complete
		#ifdef _WINDOWS
    fnSimSPI_Flash(STM25PXXX_CHECK_SS, 0);                               // simulate the SPI FLASH device
		#endif
}



// Check whether a known SPI FLASH device can be detected - called only once on start up
//
		#ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckSTM25Pxxx(int iChipSelect)
		#else
static unsigned char fnCheckSTM25Pxxx(void)
		#endif
{
    volatile unsigned char ucID[3];
    volatile unsigned long ulDelay = ((BUS_CLOCK/12000) * 15);           // 15ms start up delay
    int iReturn = NO_SPI_FLASH_AVAILABLE;

		#ifdef SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        while (ulDelay--) {};                                            // start up delay to ensure SPI FLASH ready
    }
		#else
    while (ulDelay--) {};                                                // start up delay to ensure SPI FLASH ready
		#endif

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ST) {                                 // {4}
        switch (ucID[2]) {
        case (DEVICE_ID_1_DATA_ST_FLASH_512K):                           // 512kBit / 64kByte
            iReturn = STM25P05;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_1M):                             // 1MBit / 128kByte
            iReturn = STM25P10;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_2M):                             // 2MBit / 256kByte
            iReturn = STM25P20;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_4M):                             // 4MBit / 0.5MegByte
            iReturn = STM25P40;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_8M):                             // 8MBit / 1MegByte
            iReturn = STM25P80;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_16M):                            // 16MBit / 2MegByte
            iReturn = STM25P16;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_32M):                            // 32MBit / 4MegByte
            iReturn = STM25P32;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_64M):                            // 64MBit / 8MegByte
            iReturn = STM25P64;
            break;
        case (DEVICE_ID_1_DATA_ST_FLASH_128M):                           // 128MBit / 16MegByte
            iReturn = STM25P128;
            break;
        }
    }
    #if defined REMOVE_SPI_FLASH_PROTECTION
    if (iReturn != NO_SPI_FLASH_AVAILABLE ) {                            // if device has been recognised
        unsigned char ucStatus;
        fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read status register 
        if (ucStatus & (STATUS_BL_PROTECT1 | STATUS_BL_PROTECT2 | STATUS_BL_PROTECT3)) { // if protection set
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // {6} sequence to write status register to remove protection - first enable status register write
            fnSPI_command(WRITE_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1); // now write to status register
        }
    }
    #endif
    return iReturn;
}
#endif
#endif

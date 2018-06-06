/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_m5223x_atmel.h [FREESCALE Coldfire V2 MCU]
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

    23.05.2008 Add automatic power of 2 mode setting if the page size is defined for this {1}
    28.06.2008 Remove configuration of QSPI commands from driver interface (these are initialised once on start up) {2}
    29.06.2008 Modify _CLEARBITS macro to use positive logic             {3}
    15.10.2009 Add 1MBit and 2MBit parts                                 {4}
    02.10.2011 Add STORAGE_REV_1 support                                 {5}

*/ 

#ifdef SPI_FLASH_ATMEL

#ifdef _SPI_DEFINES
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckAT45dbxxx(int iChipSelect);
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
        static unsigned char fnCheckAT45dbxxx(void);
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
    ucSPI_FLASH_Type[0] = fnCheckAT45dbxxx(0);                           // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckAT45dbxxx();                            // flag whether the SPI FLASH device is connected
        #endif
        #if SPI_FLASH_PAGES == 512
    if (ucSPI_FLASH_Type[0] < AT45DB011B)                                // we expect at least this part to be available
        #else
    if (ucSPI_FLASH_Type[0] < AT45DB021B)                                // we expect at least this part to be available
        #endif
    {                                                                    
        QMR = 0;                                                         // disable SPI if no chip connected
    }
    else {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckAT45dbxxx(i);
        }
        #endif
        #ifdef STORAGE_REV_1                                             // {5}
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
        #endif
    }
#endif




#ifdef _SPI_FLASH_INTERFACE
    // This is the main interface code to the ATMEL SPI FLASH device

/* =================================================================== */
/*                           AT45DB041 driver                          */
/* =================================================================== */

#define MANUFACTURER_ID_ATMEL      0x1F                                  // D-version manufacturer's ID
#define DEVICE_ID_1_DATA_FLASH_1M  0x22                                  // D-version FLASH size - 1MBit / 128kByte {4}
#define DEVICE_ID_1_DATA_FLASH_2M  0x23                                  // D-version FLASH size - 2MBit / 256kByte {4}
#define DEVICE_ID_1_DATA_FLASH_4M  0x24                                  // D-version FLASH size - 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_FLASH_8M  0x25                                  // D-version FLASH size - 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_FLASH_16M 0x26                                  // D-version FLASH size - 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_FLASH_32M 0x27                                  // C/D-version FLASH size - 32MBit / 4MegByte
#define DEVICE_ID_1_DATA_FLASH_64M 0x28                                  // D-version FLASH size - 64MBit / 8MegByte


#define READ_STATUS_REGISTER     0xd7
  #define STATUS_READY           0x80
  #define STATUS_MISMATCH        0x40
  #define STATUS_1MEG            0x0c                                    // expected in 1Meg bit device
  #define STATUS_2MEG            0x14                                    // expected in 2Meg bit device
  #define STATUS_4MEG            0x1c                                    // expected in 4Meg bit device
  #define STATUS_8MEG            0x24                                    // expected in 8Meg bit device
  #define STATUS_16MEG           0x2c                                    // expected in 16Meg bit device
  #define STATUS_32MEG           0x34                                    // expected in 32Meg bit device
  #define STATUS_64MEG           0x3c                                    // expected in 64Meg bit device
  #define STATUS_PROTECTED_SECT  0x02                                    // sector is protected (D-device)
  #define STATUS_PAGE_SIZE       0x01                                    // 256 byte pages size (rather than 264)
#define WRITE_BUFFER_1           0x84
#define WRITE_BUFFER_2           0x87
#define ERASE_PROG_FROM_BUFFER_1 0x83
#define ERASE_PROG_FROM_BUFFER_2 0x86
#define PROG_FROM_BUFFER_1       0x88
#define PROG_FROM_BUFFER_2       0x89
#define PAGE_ERASE               0x81
#define BLOCK_ERASE              0x50
#define PROG_THROUGH_BUFFER_1    0x82                                    // combines WRITE_BUFFER and ERASE_PROG_FROM_BUFFER
#define PROG_THROUGH_BUFFER_2    0x85
#define MAIN_TO_BUFFER_1         0x53                                    // copy page of main memory to buffer
#define MAIN_TO_BUFFER_2         0x55
#define COMPARE_WITH_BUFFER_1    0x60                                    // compare page of main memory with buffer
#define COMPARE_WITH_BUFFER_2    0x61
#define AUTO_PAGE_REWRITE_1      0x58                                    // combines MAIN_TO_BUFFER and ERASE_PROG_FROM_BUFFER and can be used to refresh pages (it is recommeded to refresh each page in a sector at least every 10'000 page erase/program operations of the sector)
#define AUTO_PAGE_REWRITE_2      0x59
#define CONTINUOUS_ARRAY_READ    0xe8


// Command supported only by D-device
#define SECTOR_ERASE             0x7c
#define READ_ARRAY_LOW_FREQ      0x03
#define READ_MANUFACTURER_ID     0x9f
#define POWER_OF_TWOS            0x3d
// Chip erase is not spported due to bugs in some chips - sector erase is used instead



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
        #define iChipSelect         0
    #endif
    unsigned char ucTxCount = 3;

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register 
        } while (!(ucStatus & STATUS_READY));                            // until no longer busy
    }

    _CLEARBITS(QS, ucChipSelectLine);                                    // assert SS low before starting {3}
  //QAR = QSPI_COMMAND_RAM_0;                                            // set address to first control location {2}
  //QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0);  // no chip select since we control it via port - 8 bit access {2}
    QAR = QSPI_TRANSMIT_RAM_0;                                           // set address to first transmit location
    QDR = ucCommand;                                                     // prepare command
#ifdef _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);                 // simulate the SPI FLASH device
    QIR |= QSPI_SPIF;
#endif

    switch (ucCommand) {
#if SPI_FLASH_PAGE_LENGTH == 256 || SPI_FLASH_PAGE_LENGTH == 512 || SPI_FLASH_PAGE_LENGTH == 1024
    case POWER_OF_TWOS:
        QDR = 0x2a;                                                      // prepare unlock sequence
        QDR = 0x80;
        QDR = 0xa6;
        break;
#endif
    case WRITE_BUFFER_1:                                                 // write data to the buffer
  //case WRITE_BUFFER_2:
        QDR = 0;
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
#endif
        QDR = (unsigned char)(ulPageNumberOffset >> 8);
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
#endif
        QDR = (unsigned char)ulPageNumberOffset;
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
#endif
        break;

#ifdef TEST_SPI_REFRESH
    case AUTO_PAGE_REWRITE_1: 
#endif
    case MAIN_TO_BUFFER_1:                                               // command chip to copy a page from main memory to the buffer (400us transfer time)
  //case MAIN_TO_BUFFER_2:
    case PROG_FROM_BUFFER_1:                                             // command programming buffer to FLASH (no erase)
  //case PROG_FROM_BUFFER_2:
    case ERASE_PROG_FROM_BUFFER_1:                                       // erase before programming
  //case ERASE_PROG_FROM_BUFFER_2:
    case PAGE_ERASE:
    case BLOCK_ERASE:
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024                                    // power of 2s mode
        QDR = (unsigned char)(ulPageNumberOffset >> 6);
    #else
        QDR = (unsigned char)(ulPageNumberOffset >> 5);
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512                                    // power of 2s mode
        QDR = (unsigned char)(ulPageNumberOffset >> 7);
    #else
        QDR = (unsigned char)(ulPageNumberOffset >> 6);
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256                                    // power of 2s mode
        QDR = (unsigned char)(ulPageNumberOffset >> 8);
    #else
        QDR = (unsigned char)(ulPageNumberOffset >> 7);
    #endif
#endif
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
#endif
#if SPI_FLASH_PAGE_LENGTH >= 1024
    #if SPI_FLASH_PAGE_LENGTH == 1024                                    // power of 2s mode
        QDR = (unsigned char)(ulPageNumberOffset << 2);
    #else
        QDR = (unsigned char)(ulPageNumberOffset << 3);
    #endif
#elif SPI_FLASH_PAGE_LENGTH >= 512
    #if SPI_FLASH_PAGE_LENGTH == 512                                    // power of 2s mode
        QDR = (unsigned char)(ulPageNumberOffset << 1);
    #else
        QDR = (unsigned char)(ulPageNumberOffset << 2);
    #endif
#else
    #if SPI_FLASH_PAGE_LENGTH == 256                                    // power of 2s mode
        QDR = (unsigned char)(ulPageNumberOffset);
    #else
        QDR = (unsigned char)(ulPageNumberOffset << 1);
    #endif
#endif
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
#endif
        QDR = 0;                                                         // dummy byte
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
#endif
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case CONTINUOUS_ARRAY_READ:                                          // this is a legacy command for compatibility between B and D-devices
        QDR = (unsigned char)(ulPageNumberOffset >> 16);
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
#endif
        QDR = (unsigned char)(ulPageNumberOffset >> 8);
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
#endif
        QDR = (unsigned char)(ulPageNumberOffset);
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);             // simulate the SPI FLASH device
        QIR |= QSPI_SPIF;                                                // set flag for simulator
#endif
        QDR = 0xff;                                                      // 4 dummy bytes needed before the device returns data
        QDR = 0xff;
        QDR = 0xff;
        QDR = 0xff;
        ucTxCount = 7;
        SPI_FLASH_Danger[iChipSelect] = 1;                               // in this case this marks that no 0xff have been set to tx when reading (it will be cleared before exiting)
        break;

    case READ_MANUFACTURER_ID:                                           // this only works on D-device
        QDR = 0xff;                                                      // dummy writes
        QDR = 0xff;
        QDR = 0xff;
        ucTxCount = 4;                                                   // fall through intentionally
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        QDR = 0xff;
        if (READ_STATUS_REGISTER == ucCommand) {
            ucTxCount = 1;
        }
        QWR = ((0 << QSPI_START_SHIFT) | (ucTxCount << QSPI_END_SHIFT)); // starting at queue 0 send/receive the required amount of entries
        QDLYR = QSPI_SPE;                                                // start transfer - bytes will be sent and read in
        while (!(QIR & QSPI_SPIF)) {};                                   // wait for transfer to complete
        QIR = QSPI_SPIF;                                                 // clear interrupt flag
        QAR = QSPI_RECEIVE_RAM_0;                                        // set address to first receive location
        ucTxCount = (unsigned char)QDR;                                  // clear command rx byte
        while (DataLength--) {
#ifdef _WINDOWS
            QDR = fnSimAT45DBXXX(AT45DBXXX_READ, (unsigned char)QAR);    // simulate the SPI FLASH device
#endif
            *ucData++ = (unsigned char)QDR;
        }
        _SETBITS(QS, ucChipSelectLine);                                  // deassert SS when complete
#ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
#endif
        return;

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
      //QAR = QSPI_COMMAND_RAM_0;                                        // set address to first control location {2}
      //QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); // no chip select since we control it via port - 8 bit access {2}
        QAR = QSPI_TRANSMIT_RAM_0;                                       // set address to first transmit location
        if (ucCommand != CONTINUOUS_ARRAY_READ) {                        // write type
            QDR = *ucData++;                                             // prepare data
#ifdef _WINDOWS
            fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);         // simulate the SPI FLASH device
            QIR |= QSPI_SPIF;                                            // set flag for simulator
#endif
            while (--DataLength != 0) {                                  // for each byte in the QSPI transfer block
                QDR = *ucData++;                                         // prepare data
#ifdef _WINDOWS
                fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)QDR);     // simulate the SPI FLASH device
#endif
                if (++iDataCnt >= 15) {                                  // maximum QSPI transfer length reached
                    DataLength--;
                    break;
                }
            }
            QWR = ((0 << QSPI_START_SHIFT) | (iDataCnt << QSPI_END_SHIFT)); // starting at queue 0 send/receive programmed entries
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
                SPI_FLASH_Danger[iChipSelect] = 0;                       // set value needed in case of CONTINUOUS_ARRAY_READ and stop more 0xff being written
            }
            QWR = ((0 << QSPI_START_SHIFT) | (iDataCnt << QSPI_END_SHIFT)); // starting at queue 0 send/receive programmed entries
            QDLYR = QSPI_SPE;                                            // start transfer - programmed number of bytes will be sent and read in
            while (!(QIR & QSPI_SPIF)) {};                               // wait for transfer to complete
            QIR = QSPI_SPIF;                                             // clear interrupt flag

            QAR = QSPI_RECEIVE_RAM_0;                                    // set address to first receive location
            do {                                                         // collect each byte
#ifdef _WINDOWS
                QDR = fnSimAT45DBXXX(AT45DBXXX_READ, (unsigned char)QAR);// simulate the SPI FLASH device
#endif
                *ucData++ = (unsigned char)QDR;                          // read bytes to the return buffer
            } while (iDataCnt--);
        }
    }
    if (ucCommand != CONTINUOUS_ARRAY_READ) {                            // commands that have only written wait here for transmission to complete
        while (!(QIR & QSPI_SPIF)) {};                                   // wait for transfer to complete
        QIR = QSPI_SPIF;                                                 // clear interrupt flag
    }

    _SETBITS(QS, ucChipSelectLine);                                      // deassert SS when complete
#ifdef _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_CHECK_SS, 0);                               // simulate the SPI FLASH device
#endif
}

/*
static void ReadPage(unsigned long ulPageNumberOffset, volatile unsigned char *ucData,unsigned long Length)
{    
    fnSPI_command(CONTINUOUS_ARRAY_READ, (ulPageNumberOffset << 9), ucData, Length);
}*/

#ifdef TEST_SPI_REFRESH
// Refresh all pages in a particular sector
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
    extern void fnAutoRefreshSector(unsigned char ptrSector, int iChipSelect)
#else
    extern void fnAutoRefreshSector(unsigned char ptrSector)
#endif
{
    unsigned short i;
    unsigned short usPageNumber;
    
    usPageNumber = (ptrSector * (SPI_FLASH_SECTOR_LENGTH / SPI_FLASH_PAGE_LENGTH)); 
    
    for(i = 0; i < (SPI_FLASH_SECTOR_LENGTH / SPI_FLASH_PAGE_LENGTH); i++, usPageNumber++) {            
        fnSPI_command(AUTO_PAGE_REWRITE_1, (unsigned long)(usPageNumber), _EXTENDED_CS 0, 0);
    }
}
#endif

// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
    static unsigned char fnCheckAT45dbxxx(int iChipSelect)
#else
    static unsigned char fnCheckAT45dbxxx(void)
#endif
{
    volatile unsigned char ucID[4];
    volatile unsigned long ulDelay = ((BUS_CLOCK/12000) * 25);           // 25ms start up delay

#ifdef SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        while (ulDelay--) {};                                            // start up delay to ensure SPI FLASH ready
    }
#else
    while (ulDelay--) {};                                                // start up delay to ensure SPI FLASH ready
#endif

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ATMEL) {                              // ATMEL D-part recognised
        int iRtn;
        switch (ucID[1]) {
        case DEVICE_ID_1_DATA_FLASH_1M:                                  // {4}
            iRtn = AT45DB011D;
            break;
        case DEVICE_ID_1_DATA_FLASH_2M:                                  // {4}
            iRtn = AT45DB021D;
            break;
        case DEVICE_ID_1_DATA_FLASH_4M:
            iRtn = AT45DB041D;
            break;
        case DEVICE_ID_1_DATA_FLASH_8M:
            iRtn = AT45DB081D;
            break;
        case DEVICE_ID_1_DATA_FLASH_16M:
            iRtn = AT45DB161D;
            break;
        case DEVICE_ID_1_DATA_FLASH_32M:
            iRtn = AT45DB321D;
            break;
        case DEVICE_ID_1_DATA_FLASH_64M:
            iRtn = AT45DB641D;
            break;
        default:                                                         // possibly a larger part but we don't accept it
            iRtn = NO_SPI_FLASH_AVAILABLE;
            break;
        }
        if (iRtn != NO_SPI_FLASH_AVAILABLE) {
#if SPI_FLASH_PAGE_LENGTH == 256 || SPI_FLASH_PAGE_LENGTH == 512 || SPI_FLASH_PAGE_LENGTH == 1024 // {1} the user wants to operate the device in power of 2 mode so check this and set if necessary
            volatile unsigned char ucCheck;
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);// read the status register
            ucCheck = (ucID[0] & 0x01);                                  // this transfer to a second register is to workaround a CW compiler error which otherwise checks the wrong location in the ucID array
            if (ucCheck == 0) {                                          // not configured for power of 2 page size
                fnSPI_command(POWER_OF_TWOS, 0, _EXTENDED_CS 0, 0);      // set the mode - this requires a power cycle too before use!!
            }
#endif
            return iRtn;
        }
    }

    fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);        // it is possibly a B-device so check the status register
    switch (ucID[0] & 0x3c) {                                            // check part size field
    case STATUS_1MEG:
        return AT45DB011B; 
    case STATUS_2MEG:
        return AT45DB021B; 
    case STATUS_4MEG:
        return AT45DB041B; 
    case STATUS_8MEG:
        return AT45DB081B; 
    default:
        break;
    }

    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}
#endif

#endif

/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        spi_flash_sam7x_atmel.h [Atmel AT91SAM7X128 / 256 / 512]
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

    01.03.2008 Add macros to allow changing SPI mode when entering and leaving SPI FLASH command routine {1} (originally {26})
    18.03.2008 Remove chip select parameter from fnSimAT45DBXXX() - moved to spi_flash_sam7x_atmel.h 
    25.03.2008 Optimise use of SPI_FLASH_Danger[iChipSelect] - moved to spi_flash_sam7x_atmel.h {2} (originally {28})
    25.10.2008 wait until last byte has been completely received before negating the CS line {3}
    15.10.2009 Add 1MBit and 2MBit parts                                 {4}
    01.12.2009 Add managed memory support                                {5}
    28.10.2011 Add STORAGE_REV_1 support                                 {6}

*/ 

#ifdef SPI_FLASH_ATMEL

#ifdef _SPI_DEFINES
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckAT45dbxxx(int iChipSelect);
        #ifdef STORAGE_REV_1                                             // {6}
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
        #ifdef STORAGE_REV_1                                             // {6}
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
    // If there are multiple devices, each will be recorded.
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
        REMOVE_POWER_SPI();                                              // disable SPI if no chip connected
    }
    else {
    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckAT45dbxxx(i);
        }
    #endif
    #ifdef STORAGE_REV_1                                                 // {6}
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

#define CHECK_SPI_FLASH_BUSY     0x00                                    // pseudo request to see whether device is ready
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
// Chip erase is not spported due to bugs in some chips - sector erase is used instead



// SPI FLASH hardware interface
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #ifdef COUNT_SPI_TX
    int iSPI_transmissions = (DataLength + 4);                           // counter used only in special cases to record the number of bytes sent using the command
    #endif
    #define CMD_WRITE 0x01
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine = ulChipSelect[iChipSelect];
    #define ulChipSelectLineSim 0xffffffff
    #else
    #define ulChipSelectLine    CS0_LINE
    #define ulChipSelectLineSim CS0_LINE
    #define iChipSelect 0
    #endif
    unsigned char ucTxCount = 0;
    unsigned char ucCommandBuffer[3];
                                                                         // {2}
    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register
    #ifdef MANAGED_FILES                                                 // {5}
            if (ucCommand == CHECK_SPI_FLASH_BUSY) {                     // pseudo request to see whether device is ready
                if (ucStatus & STATUS_READY) {
                    return;                                              // the device is no longer busy
                }
                else if (--(*ucData) == 0) {
                    SPI_FLASH_Danger[iChipSelect] = 1;                   // put the busy bit back
                    return;                                              // the maximum number of status requests has expired so quit
                }
            }
    #endif
        } while (!(ucStatus & STATUS_READY));                            // until no longer busy
    }
    #ifdef MANAGED_FILES                                                 // {5}
    else if (ucCommand == CHECK_SPI_FLASH_BUSY) {                        // pseudo command used to check device's status
        return;                                                          // the device is not busy so return immediately
    }
    #endif

    SET_SPI_FLASH_MODE();                                                // {1}

    SPI_FLASH_PORT_CLR = ulChipSelectLine;             _SIM_PORT_CHANGE  // assert SS low before starting

    (void)SPI_RDR;                                                       // reset receive data flag with dummy read - the rx data is not interesting here
    SPI_TDR = ucCommand;                                                 // send command

    #ifdef _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TDR);             // simulate the SPI FLASH device
    SPI_SR |= SPI_RDRF;
    #endif
    while (!(SPI_SR & SPI_RDRF)) {};                                     // wait until tx byte has been sent and rx byte has been completely received
    (void)SPI_RDR;                                                       // reset receive data flag with dummy read - the rx data is not interesting here

    switch (ucCommand) {
    case WRITE_BUFFER_1:                                                 // write data to the buffer
  //case WRITE_BUFFER_2:
        ucCommandBuffer[0] = 0;
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)ulPageNumberOffset;
        break;

    case MAIN_TO_BUFFER_1:                                               // command chip to copy a page from main memory to the buffer (400us transfer time)
  //case MAIN_TO_BUFFER_2:
    case PROG_FROM_BUFFER_1:                                             // command programming buffer to FLASH (no erase)
  //case PROG_FROM_BUFFER_2:
    case ERASE_PROG_FROM_BUFFER_1:                                       // erase before programming
  //case ERASE_PROG_FROM_BUFFER_2:
    case PAGE_ERASE:
    case BLOCK_ERASE:
    #if SPI_FLASH_PAGE_LENGTH >= 1024
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 5);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 3);
    #elif SPI_FLASH_PAGE_LENGTH >= 512
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 6);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 2);
    #else
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 7);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 1);
    #endif
        ucCommandBuffer[2] = 0;
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case CONTINUOUS_ARRAY_READ:                                          // this is a legacy command for compatibility between B,C and D-devices
    #ifdef COUNT_SPI_TX
        iSPI_transmissions += 4;
    #endif
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 16);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)(ulPageNumberOffset);
        break;

    case READ_MANUFACTURER_ID:                                           // this only works on D-device
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        while (DataLength--) {
            SPI_TDR = 0xff;                                              // empty transmission byte
            while (!(SPI_SR & SPI_RDRF)) {};                             // wait until dummy tx byte has been sent and rx byte has been completely received
    #ifdef _WINDOWS
            SPI_RDR = fnSimAT45DBXXX(AT45DBXXX_READ, 0);                 // simulate the SPI FLASH device
            SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                          // simulate tx and rx interrupt flags being set
    #endif
            *ucData++ = (unsigned char)SPI_RDR;                          // read received byte and clear rx interrupt
        }
        SPI_FLASH_PORT_SET = ulChipSelectLine;          _SIM_PORT_CHANGE // deassert SS when complete
    #ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
    #endif
    #ifdef COUNT_SPI_TX
        iSPI_transmissions = 5;                                          // counter used only in special cases to record the number of bytes sent using the command
    #endif
        REMOVE_SPI_FLASH_MODE();                                         // {1}
        return;

        // Note - no default here. This must never be called with non-supported command since it will otherwise hang.
    }

    while (ucTxCount < sizeof(ucCommandBuffer)) {                        // complete the command sequence
        SPI_TDR = ucCommandBuffer[ucTxCount++];                          // send data
    #ifdef _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TDR);         // simulate the SPI FLASH device
        SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                              // simulate tx and rx interrupt flags being set
    #endif
        while (!(SPI_SR & SPI_TXEMPTY)) {};                              // wait until tx byte has been sent
    }

    if (CONTINUOUS_ARRAY_READ == ucCommand) {
        ucTxCount = 4;
        while (ucTxCount--) {                                            // 4 dummy bytes required before device returns data
            SPI_TDR = 0xff;                                              // send data
    #ifdef _WINDOWS
            SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                          // simulate tx and rx interrupt flags being set
    #endif
            while (!(SPI_SR & SPI_TXEMPTY)) {};                          // wait until tx byte has been sent
        }
        (void)SPI_RDR;                                                   // reset receive data flag with dummy read - the rx data is not interesting here
        while (DataLength-- != 0) {                                      // while data bytes to be read
            SPI_TDR = 0xff;
    #ifdef _WINDOWS
            SPI_RDR = fnSimAT45DBXXX(AT45DBXXX_READ, 0);                 // simulate the SPI FLASH device
            SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                          // simulate tx and rx interrupt flags being set
    #endif
            while (!(SPI_SR & SPI_RDRF)) {};                             // wait until tx byte has been sent and rx byte has been completely received
            *ucData++ = (unsigned char)SPI_RDR;
        }
    }
    else {
        while (DataLength-- != 0) {                                      // while data bytes to be written
            SPI_TDR = *ucData++;                                         // send data
    #ifdef _WINDOWS
            fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TDR);     // simulate the SPI FLASH device
            SPI_SR |= (SPI_RDRF | SPI_TXEMPTY);                          // simulate tx and rx interrupt flags being set
    #endif
            while (!(SPI_SR & SPI_TXEMPTY)) {};                          // wait until tx byte has been sent
        }
        while (!(SPI_SR & SPI_RDRF)) {};                                 // {3} wait until last byte has been completely received before negating the CS line
    }

    SPI_FLASH_PORT_SET = ulChipSelectLine;              _SIM_PORT_CHANGE // deassert SS when complete
    #ifdef _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_CHECK_SS, 0);                               // simulate the SPI FLASH device
    #endif
    REMOVE_SPI_FLASH_MODE();                                             // {1}
}



// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckAT45dbxxx(int iChipSelect)
#else
static unsigned char fnCheckAT45dbxxx(void)
#endif
{
    volatile unsigned char ucID[4];
    volatile unsigned long ulDelay = ((PLL_OUTPUT_FREQ/18000) * 25);     // 25ms start up delay

#ifdef SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        while (ulDelay--) {};                                            // start up delay to ensure SPI FLASH ready {17}
    }
#else
    while (ulDelay--) {};                                                // start up delay to ensure SPI FLASH ready {17}
#endif

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ATMEL) {                              // ATMEL D-part recognised
        switch (ucID[1]) {
        case DEVICE_ID_1_DATA_FLASH_1M:                                  // {4}
            return AT45DB011D;
        case DEVICE_ID_1_DATA_FLASH_2M:                                  // {4}
            return AT45DB021D;
        case DEVICE_ID_1_DATA_FLASH_4M:
            return AT45DB041D;
        case DEVICE_ID_1_DATA_FLASH_8M:
            return AT45DB081D;
        case DEVICE_ID_1_DATA_FLASH_16M:
            return AT45DB161D;
        case DEVICE_ID_1_DATA_FLASH_32M:
            return AT45DB321D;
        case DEVICE_ID_1_DATA_FLASH_64M:
            return AT45DB641D;
        default:                                                         // possibly a larger part but we don't accept it
            break;
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
    case STATUS_16MEG:
        return AT45DB321C;
    default:
        break;
    }

    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}
#endif

#endif

/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_eeprom_kinetis_25AA160.h (Microchip)
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/

#if defined SPI_FLASH_MX25L

#if defined _SPI_EEPROM_DEFINES
    #if !defined SPI_FLASH_FIFO_DEPTH
        #define SPI_FLASH_FIFO_DEPTH     1                               // if no fifo depth is specified we assume that it is 1
    #endif
    #if defined SPI_EEPROM_MULTIPLE_CHIPS
        #define __EXTENDED_CS     iChipSelect,
        static const STORAGE_AREA_ENTRY spi_eeprom_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_EEPROM_SIZE - 1)),
            _STORAGE_SPI_EEPROM,                                         // type
            SPI_EEPROM_DEVICE_COUNT                                      // multiple devices
        };
    #else
        #define __EXTENDED_CS
        #if defined SPI_FILE_SYSTEM                                      // spi flash as well as spi eeprom
            static const STORAGE_AREA_ENTRY spi_eeprom_storage = {
            (void *)&spi_flash_storage,                                  // link to SPI flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + SPI_DATA_FLASH_SIZE), // spi eeprom area starts after spi flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + SPI_DATA_FLASH_SIZE + (SPI_DATA_EEPROM_SIZE - 1)),
            _STORAGE_SPI_EEPROM,                                         // type
            0                                                            // not multiple devices
        };
        #else                                                            // just spi eeprom
        static const STORAGE_AREA_ENTRY spi_eeprom_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi eeprom area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_EEPROM,                                         // type
            0                                                            // not multiple devices
        };
        #endif
    #endif
#endif


    // Since the EEPROM doesn't allow its type to be read we assume that it is present and insert its storage characteristics into the storage list
    //
#if defined _CHECK_SPI_CHIPS
    UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_eeprom_storage;  // insert spi eeprom as storage medium
#endif




#if defined _SPI_EEPROM_INTERFACE
// This is the main interface code to the 25AA160 SPI EEPROM device

/* =================================================================== */
/*                           25AA160 driver                            */
/* =================================================================== */

#define EEPROM_WRITE_STATUS_REGISTER    0x01
#define EEPROM_PAGE_PROG                0x02
#define EEPROM_READ_DATA_BYTES          0x03
#define EEPROM_WRITE_DISABLE            0x04
#define EEPROM_READ_STATUS_REGISTER     0x05
  #define STATUS_WIP             0x01                                    // WIP (write in progress)
  #define STATUS_WEL             0x02                                    // WEL (write enable latch)
  #define STATUS_BP0             0x04                                    // block protection 0
  #define STATUS_BP1             0x08                                    // block protection 1
  #define STATUS_BP2             0x10                                    // block protection 2
  #define STATUS_BP3             0x20                                    // block protection 3
  #define STATUS_QE              0x40                                    // quad enable
  #define STATUS_WPEN            0x80                                    // status register write protect
#define EEPROM_WRITE_ENABLE             0x06

#define DANGER_EEPROM_PROGRAMMING   0x80000000



// SPI FLASH hardware interface
//
    #if defined SPI_EEPROM_MULTIPLE_CHIPS
static void fnSPI_EEPROM_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_EEPROM_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #if defined SPI_EEPROM_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine_eeprom = ulChipSelect[iChipSelect];
    #define ulChipSelectLineSim_eeprom 0xffffffff
    #else
    #define ulChipSelectLine_eeprom    CS0_LINE_EEPROM
    #define ulChipSelectLineSim_eeprom CS0_LINE_EEPROM
    #define iChipSelect 0
    #endif
    MAX_FILE_LENGTH dummyWrites  = 0;
    MAX_FILE_LENGTH dataWrites   = 0;
    MAX_FILE_LENGTH discardCount = 0;

    FLUSH_SPI_FIFO_AND_FLAGS();                                          // ensure that the FIFOs are empty and the status flags are reset before starting

    if ((SPI_EEPROM_Danger[iChipSelect] & DANGER_EEPROM_PROGRAMMING) != 0) { // check whether the chip is ready to work - if not, wait until the present internal operation has completed or suspend it
        volatile unsigned char ucStatus;
    #if defined SUPPORT_ERASE_SUSPEND
        unsigned long ulInitialState = SPI_EEPROM_Danger[iChipSelect];   // backup the original state
    #endif
        SPI_EEPROM_Danger[iChipSelect] = 0;                              // device will no longer be busy after continuing
        do {
            fnSPI_EEPROM_command(READ_STATUS_REGISTER, 0, __EXTENDED_CS &ucStatus, 1); // read busy status register
        } while ((ucStatus & STATUS_WIP) != 0);                          // until no longer busy
    }

    #if defined SET_SPI_FLASH_MODE
    SET_SPI_FLASH_MODE();
    #endif

    #if !defined DSPI_SPI || defined MANUAL_FLASH_CS_CONTROL
    ASSERT_CS_LINE_EEPROM(ulChipSelectLine);                             // assert the chip select line
    #endif

    switch (ucCommand) {
    case EEPROM_PAGE_PROG:                                               // 1 to 16 bytes to be programmed to a page
        SPI_EEPROM_Danger[iChipSelect] = DANGER_EEPROM_PROGRAMMING;      // mark that there is an operation in progress that will require time to complete
        dataWrites = discardCount = DataLength;
    case EEPROM_READ_DATA_BYTES:                                         // read - first setting the address and then reading the defined amount of data bytes
        WRITE_SPI_CMD0(ucCommand);                                       // write command byte
    #if defined _WINDOWS
        fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);       // simulate the SPI FLASH device
    #endif
    #if (SPI_FLASH_FIFO_DEPTH < 2)
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
    #endif
        if (DataLength != 0) {
            WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 8));    // write address (msb)
    #if defined _WINDOWS
            fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);   // simulate the SPI FLASH device
    #endif
    #if (SPI_FLASH_FIFO_DEPTH < 2)
            WAIT_SPI_RECEPTION_END();                                    // wait until the command has been sent
            (void)READ_SPI_FLASH_DATA();                                 // discard the received byte
            CLEAR_RECEPTION_FLAG();                                      // clear the receive flag
            discardCount += 1;
    #else
            discardCount += 4;
    #endif
            if (DataLength == 0) {
                WRITE_SPI_CMD0_LAST((unsigned char)(ulPageNumberOffset));// write address (lsb)
            }
            else {
                WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset));     // write address (lsb)
            }
    #if defined _WINDOWS
            fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);   // simulate the SPI FLASH device
    #endif
            if (dataWrites == 0) {
                dummyWrites = DataLength;                                // since we are reading we perform only dummy writes
            }
        }
        else {                                                           // no address is required in this case since we are in an AAI sequence
            WRITE_SPI_CMD0(*ucData++);                                   // always a write pair performed
    #if defined _WINDOWS
            fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);   // simulate the SPI FLASH device
    #endif
    #if (SPI_FLASH_FIFO_DEPTH < 2)
            WAIT_SPI_RECEPTION_END();                                    // wait until the command has been sent
            (void)READ_SPI_FLASH_DATA();                                 // discard the received byte
            CLEAR_RECEPTION_FLAG();                                      // clear the receive flag
    #endif
            WRITE_SPI_CMD0_LAST(*ucData);
            discardCount = 3;
    #if defined _WINDOWS
            fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);   // simulate the SPI FLASH device
    #endif
        }
        break;

    case EEPROM_READ_STATUS_REGISTER:                                    // read single byte from status register (0x05)
        WRITE_SPI_CMD0(ucCommand);                                       // write command byte
    #if defined _WINDOWS
        fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);       // simulate the SPI FLASH device
    #endif
    #if (SPI_FLASH_FIFO_DEPTH < 2)
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
    #else 
        discardCount = 1;
    #endif
        WRITE_SPI_CMD0_LAST(0xff);                                       // ensure transmit FIFO has more than one byte in it
        break;

    case EEPROM_WRITE_DISABLE:
    case EEPROM_WRITE_ENABLE:
        WRITE_SPI_CMD0_LAST(ucCommand);                                  // write single command byte
    #if defined _WINDOWS
        fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);       // simulate the SPI FLASH device
    #endif
        discardCount = 1;
        break;

    case EEPROM_WRITE_STATUS_REGISTER:
        WRITE_SPI_CMD0(ucCommand);                                       // write command byte
    #if defined _WINDOWS
        fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);       // simulate the SPI FLASH device
    #endif
    #if (SPI_FLASH_FIFO_DEPTH < 2)
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
        discardCount = 1;
    #else
        discardCount = 2;
    #endif
        WRITE_SPI_CMD0_LAST(ucCommand);                                  // write new value
    #if defined _WINDOWS
        fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);       // simulate the SPI FLASH device
    #endif
        DataLength = 0;
        break;
    }

    while (DataLength != 0) {
        WAIT_SPI_RECEPTION_END();                                        // wait until at least one byte is in the receive FIFO
        if (discardCount != 0) {
            (void)READ_SPI_FLASH_DATA();                                 // discard
            discardCount--;
        }
        else {
    #if defined _WINDOWS
            SPI_RX_BYTE = fnSimS25FL1_K(S25FL1_K_READ, (unsigned char)SPI_TX_BYTE); // simulate the SPI FLASH device
    #endif
            *ucData++ = READ_SPI_FLASH_DATA();                           // read the byte from the receive FIFO and save to the application buffer
            DataLength--;
        }
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
        if (dummyWrites != 0) {                                          // dummy writes
            if (dummyWrites > 1) {
                WRITE_SPI_CMD0(0xff);                                    // write dummy
            }
            else {
                WRITE_SPI_CMD0_LAST(0xff);                               // write final dummy
            }
            dummyWrites--;
        }
        if (dataWrites != 0) {                                           // data writes
            if (dataWrites > 1) {
                WRITE_SPI_CMD0(*ucData++);                               // write data byte
            }
            else {
                WRITE_SPI_CMD0_LAST(*ucData++);                          // write final data byte
            }
    #if defined _WINDOWS
            fnSimS25FL1_K(S25FL1_K_WRITE, (unsigned char)SPI_TX_BYTE);   // simulate the SPI FLASH device
    #endif
            dataWrites--;
            DataLength--;
        }
    }
    while (discardCount != 0) {
        WAIT_SPI_RECEPTION_END();                                        // wait until at least one byte is in the receive FIFO
        (void)READ_SPI_FLASH_DATA();                                     // discard
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
        discardCount--;
    }
    #if (!defined DSPI_SPI) || defined MANUAL_FLASH_CS_CONTROL
    NEGATE_CS_LINE_EEPROM(ulChipSelectLine);                             // negate the chip select line
    #endif
    #if defined _WINDOWS
        #if defined DSPI_SPI && !defined MANUAL_FLASH_CS_CONTROL
    if ((SPI_TX_BYTE & SPI_PUSHR_EOQ) != 0) {                            // check that the CS has been negated
        SPI_TX_BYTE &= ~(ulChipSelectLine);
    }
        #endif
    fnSimS25FL1_K(S25FL1_K_CHECK_SS, 0);                                 // simulate the SPI FLASH device
    #endif
    #if defined REMOVE_SPI_FLASH_MODE
    REMOVE_SPI_FLASH_MODE();
    #endif
}
#endif
#endif

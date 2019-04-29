/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        spi_flash_lpc_sst25.h
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

    18.09.2012 Add STORAGE_REV_1 support                                 {1}

*/

#ifdef SPI_FLASH_SST25

#ifdef _SPI_DEFINES
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckSST25xxx(int iChipSelect);
        #ifdef STORAGE_REV_1                                             // {1}
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            SPI_FLASH_DEVICE_COUNT                                       // multiple devices
        };
        #endif
    #else
        static unsigned char fnCheckSST25xxx(void);
        #ifdef STORAGE_REV_1                                             // {1}
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
    ucSPI_FLASH_Type[0] = fnCheckSST25xxx(0);                            // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckSST25xxx();                             // flag whether the SPI FLASH device is connected
        #endif
    if (ucSPI_FLASH_Type[0] < SST25VF010A) {                             // we expect at least this part to be available
        SSPCR0_X = 0;                                                    // disable SPI if no chip connected
        POWER_DOWN(PCSSP_X);                                             // power down SSI
    }
    else {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckSST25xxx(i);
        }
        #endif
        #ifdef STORAGE_REV_1                                             // {1}
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
        #endif
    }
#endif




#ifdef _SPI_FLASH_INTERFACE
// This is the main interface code to the SST SPI FLASH device

/* =================================================================== */
/*                             SST25 driver                            */
/* =================================================================== */

#define READ_STATUS_REGISTER     0x05
  #define STATUS_BUSY            0x01
  #define STATUS_WEL             0x02
  #define STATUS_BP0             0x04
  #define STATUS_BP1             0x08
  #define STATUS_BP2             0x10
  #define STATUS_BP3             0x20
  #define STATUS_AAI             0x40                                    // Auto Address Increment Programming Status
  #define STATUS_BPL             0x80
#define READ_DATA_BYTES          0x03
#define FAST_READ                0x0B
#define SUB_SECTOR_ERASE         0x20
#define HALF_SECTOR_ERASE        0x52
#define SECTOR_ERASE             0xd8
#define BULK_ERASE               0xc7
#define BYTE_PROG                0x02
#ifdef SST25_A_VERSION
    #define AAI_WORD_PROG        0xaf
#else
    #define AAI_WORD_PROG        0xad
#endif
#define ENABLE_WRITE_STATUS_REG  0x50
#define WRITE_STATUS_REGISTER    0x01
#define WRITE_ENABLE             0x06
#define WRITE_DISABLE            0x04
#define READ_ID                  0x90
#define READ_JEDEC_ID            0x9f
#define READ_MANUFACTURER_ID     READ_JEDEC_ID                           // for compatibility
#define ENABLE_SO_TO_RY          0x70
#define DISABLE_SO_TO_RY         0x80



#define MANUFACTURER_ID_SST      0xbf                                    // SST manufacturer's ID
#define DEVICE_TYPE              0x25

#define DEVICE_ID_DATA_SST_FLASH_A_1M  0x49                              // 1MBit / 128MegByte - A type
#define DEVICE_ID_DATA_SST_FLASH_A_2M  0x43                              // 2MBit / 256MegByte - A type
#define DEVICE_ID_DATA_SST_FLASH_A_4M  0x44                              // 4MBit / 0.5MegByte - A type
#define DEVICE_ID_1_DATA_SST_FLASH_4M  0x8d                              // 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_SST_FLASH_8M  0x8e                              // 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_SST_FLASH_16M 0x41                              // 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_SST_FLASH_32M 0x4a                              // FLASH size - 32MBit / 4MegByte



// SPI FLASH hardware interface
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
extern void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #define CMD_WRITE 0x01
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine = ulChipSelect[iChipSelect];
    #define ulChipSelectLineSim 0xffffffff
    #else
    #define ulChipSelectLine    CS0_LINE
    #define ulChipSelectLineSim CS0_LINE
    #define iChipSelect 0
    #endif
    int iEraseCommand = 0;
    int iDiscard = 1;
    int iFifoDepth = 1;
    volatile unsigned long ulDummy;

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register
        } while (ucStatus & STATUS_BUSY);                                // until no longer busy
    }

    SET_SPI_FLASH_MODE();

    CS_CLR_PORT = ulChipSelectLine;                    _SIM_PORT_CHANGE  // assert SS low before starting

    SSPDR_X = ucCommand;                                                 // send command

    #ifdef _WINDOWS
    fnSimSPI_Flash(SST25_WRITE, (unsigned char)SSPDR_X);                 // simulate the SPI FLASH device
    SSPSR_X &= ~SSP_BSY;                                                 // clear flag for simulator
    #endif

    switch (ucCommand) {
    case SUB_SECTOR_ERASE:
    case HALF_SECTOR_ERASE:
    case SECTOR_ERASE:
        iEraseCommand = 1;
    case BYTE_PROG:
    case AAI_WORD_PROG:
        SPI_FLASH_Danger[iChipSelect] = 1;                               // a write/erase will be started so we need to poll the status before next command
    case READ_ID:
    case READ_DATA_BYTES:                                                // 25MHz read - first setting the address and then reading the defined amount of data bytes
        if ((DataLength != 0) || (iEraseCommand != 0)) {
            SSPDR_X = (unsigned char)(ulPageNumberOffset >> 16);         // send page number offset
    #ifdef _WINDOWS
            fnSimSPI_Flash(SST25_WRITE, (unsigned char)SSPDR_X);         // simulate the SPI FLASH device
    #endif
            SSPDR_X = (unsigned char)(ulPageNumberOffset >> 8);          // send page number offset
    #ifdef _WINDOWS
            fnSimSPI_Flash(SST25_WRITE, (unsigned char)SSPDR_X);         // simulate the SPI FLASH device
    #endif
            SSPDR_X = (unsigned char)(ulPageNumberOffset);               // send page number offset
    #ifdef _WINDOWS
            fnSimSPI_Flash(SST25_WRITE, (unsigned char)SSPDR_X);         // simulate the SPI FLASH device
            SSPSR_X &= ~SSP_BSY;                                         // clear flag for simulator
    #endif
            iDiscard = 4;
            iFifoDepth = 4;
        }
        else {                                                           // no address is required in this case since we are in an AAI sequence
    #ifdef SST25_A_VERSION
            DataLength = 1;                                              // always a single byte for A types
    #else
            DataLength = 2;                                              // always a write pair performed
    #endif
        }                                                                // fall through
        if (SPI_FLASH_Danger[iChipSelect] != 0) {                        // erase or write
            while (DataLength--) {                                       // maximum write size of 2 bytes so no special fifo treatment
                SSPDR_X = *ucData++;                                     // send data
                iDiscard++;
    #ifdef _WINDOWS
                fnSimSPI_Flash(SST25_WRITE, (unsigned char)SSPDR_X);     // simulate the SPI FLASH device
                SSPSR_X &= ~SSP_BSY;                                     // clear flag for simulator
    #endif
            }
            break;
        }                                                                // fall through in read case
    case READ_MANUFACTURER_ID:
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        while (DataLength--) {
            SSPDR_X = 0xff;                                              // empty transmission byte
            if (++iFifoDepth >= 8) {
                while (SSPSR_X & SSP_BSY) {};                            // wait for transfer to complete
                while (iFifoDepth--) {
    #ifdef _WINDOWS
                    if (iDiscard == 0) {
                        SSPDR_X = fnSimSPI_Flash(SST25_READ, 0);         // simulate the SPI FLASH device
                    }
                    else {
                        SSPDR_X = 0xff;
                    }
                    SSPSR_X &= ~(SSP_RNE | SSP_BSY);                     // simulate tx and rx interrupt flags being set
    #endif
                    *ucData = (unsigned char)SSPDR_X;                    // read received bytes
                    if (iDiscard == 0) {                                 // ensure that unwanted read data is discarded
                        ucData++;
                    }
                    else {
                        iDiscard--;
                    }
                }
                iFifoDepth = 0;
            }
        }
        while (SSPSR_X & SSP_BSY) {};                                    // wait for transfer to complete
        while (iFifoDepth--) {
    #ifdef _WINDOWS
            if (iDiscard == 0) {
                SSPDR_X = fnSimSPI_Flash(SST25_READ, 0);                 // simulate the SPI FLASH device
            }
            else {
                SSPDR_X = 0xff;
            }
            SSPSR_X &= ~(SSP_RNE | SSP_BSY);                             // simulate tx and rx interrupt flags being set
    #endif
            *ucData = (unsigned char)SSPDR_X;                            // read received bytes
            if (iDiscard == 0) {                                         // ensure that unwanted read data is discarded
                ucData++;
            }
            else {
                iDiscard--;
            }
        }
        break;

    case BULK_ERASE:                                                     // command without further data
    case WRITE_DISABLE:
    case WRITE_ENABLE:
    case ENABLE_WRITE_STATUS_REG:
        break;

    case WRITE_STATUS_REGISTER:
    default:
        while (DataLength--) {            
            SSPDR_X = (unsigned char)*ucData++;                          // send data
            iDiscard++;
    #ifdef _WINDOWS
            fnSimSPI_Flash(SST25_WRITE, (unsigned char)SSPDR_X);         // simulate the SPI FLASH device
            SSPSR_X &= ~SSP_BSY;                                         // clear flag for simulator
    #endif
            if (++iFifoDepth >= 8) {
                while (SSPSR_X & SSP_BSY) {};                            // wait for transfer to complete
                while (iFifoDepth--) {
                    ulDummy = (unsigned char)SSPDR_X;                    // discard FIFO content
                }
                iDiscard = 0;
            }
        }
        break;
    }

    while (SSPSR_X & SSP_BSY) {};                                        // wait for transfer to complete
    while (iDiscard--) {
        ulDummy = (unsigned char)SSPDR_X;                                // discard rx FIFO content
    }
    CS_SET_PORT = ulChipSelectLine;                     _SIM_PORT_CHANGE // deassert SS when complete
    #ifdef _WINDOWS
    fnSimSPI_Flash(SST25_CHECK_SS, 0);                                   // simulate the SPI FLASH device
    #endif
    REMOVE_SPI_FLASH_MODE();
}



// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckSST25xxx(int iChipSelect)
#else
static unsigned char fnCheckSST25xxx(void)
#endif
{
    volatile unsigned char ucID[3];
    unsigned char ucReturnType = NO_SPI_FLASH_AVAILABLE;
    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if ((ucID[0] == MANUFACTURER_ID_SST) && (ucID[1] == DEVICE_TYPE)) {  // SST memory part recognised
        switch (ucID[2]) {
        case DEVICE_ID_1_DATA_SST_FLASH_4M:
            ucReturnType = SST25VF040B;
            break;
        case DEVICE_ID_1_DATA_SST_FLASH_8M:
            ucReturnType = SST25VF080B;
            break;
        case DEVICE_ID_1_DATA_SST_FLASH_16M:
            ucReturnType = SST25VF016B;
            break;
        case DEVICE_ID_1_DATA_SST_FLASH_32M:
            ucReturnType = SST25VF032B;
            break;
        default:                                                         // possibly a larger part but we don't accept it
            return NO_SPI_FLASH_AVAILABLE;
        }
    }
    else {                                                               // try requesting the ID since it may be an A-version
        fnSPI_command(READ_ID, 0, _EXTENDED_CS ucID, 2);
        if (ucID[0] == MANUFACTURER_ID_SST) {
            switch (ucID[1]) {
            case DEVICE_ID_DATA_SST_FLASH_A_1M:
                ucReturnType = SST25VF010A;
                break;
            case DEVICE_ID_DATA_SST_FLASH_A_2M:
                ucReturnType = SST25LF020A;
                break;
            case DEVICE_ID_DATA_SST_FLASH_A_4M:
                ucReturnType = SST25LF040A;
                break;
            default:                                                     // possibly a larger part but we don't accept it
                return NO_SPI_FLASH_AVAILABLE;
            }
        }
    }
    ucID[0] = 0;
    fnSPI_command(ENABLE_WRITE_STATUS_REG, 0, _EXTENDED_CS 0, 0);        // sequence to write status register to remove protection - first enable status register write
    fnSPI_command(WRITE_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);       // now write to status register
    return ucReturnType;
}
#endif

#endif

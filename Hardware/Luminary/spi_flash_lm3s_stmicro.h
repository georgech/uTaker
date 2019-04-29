/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher 

   ---------------------------------------------------------------------
   File:        spi_flash_lm3s_stmicro.h [Luminary Micro]
   Project:     Single Chip Embedded Internet 
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2019
   *********************************************************************
   This file contains SPI FLASH specific code for all chips that are supported.
   It is declared as a header so that projects do not need to specify that it is not to be compiled.
   Its goal is to improve overall readability of the hardware interface.

   31.02.2009 Remove unnecessary flag                                    {1}
   12.03.2009 Add user definable SET_SPI_FLASH_MODE() and REMOVE_SPI_FLASH_MODE() {2}
   15.10.2009 Add extra SPI FLASH sizes and recognise any SPI FLASH size {3}

*/ 

#ifdef SPI_FLASH_ST

#ifdef _SPI_DEFINES
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckSTM25Pxxx(int iChipSelect);
    #else
        static unsigned char fnCheckSTM25Pxxx(void);
    #endif
#endif


    // This code is inserted to detect the presence of the SPI FLASH device(s). If the first device is not detected the SPI interface is disabled.
    // If there are multiple devices, each will be recorded.
#ifdef _CHECK_SPI_CHIPS
        #ifdef SPI_FLASH_MULTIPLE_CHIPS 
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx(0);                           // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx();                            // flag whether the SPI FLASH device is connected
        #endif
    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {                 // {3} we expect SPI FLASH to be detected
        SSICR1_0 = 0;                                                    // disable SPI if no chip connected
        RCGC1 &= ~CGC_SSI0;                                              // power down SSI
    }
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // check for further devices
    else {
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckSTM25Pxxx(i);
        }
    }
        #endif
#endif



#ifdef _SPI_FLASH_INTERFACE
// This is the main interface code to the STMicroelectronics SPI FLASH device

/* =================================================================== */
/*                           STM25PXX driver                           */
/* =================================================================== */

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
#define SUB_SECTOR_ERASE         0x20                                    // only available of the M25PExxx devices
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

// SPI FLASH hardware interface
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
        } while ((ucStatus & STATUS_BUSY) != 0);                         // until no longer busy
    }

    SET_SPI_FLASH_MODE();                                                // {2}
    GPIODATA_A &= ~ucChipSelectLine;                                     // assert SS low before starting
    SSIDR_0 = ucCommand;                                                 // send command
#ifdef _WINDOWS
    fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);             // simulate the SPI FLASH device
    SSISR_0 &= ~SSI_BSY;                                                 // clear flag for simulator
#endif

    switch (ucCommand) {
    case WRITE_ENABLE:                                                   // enable subsequent writes
        ucTxCount = 0;
        DataLength = 0;
        break;
    case PAGE_PROG:                                                      // program data to a page (programming started when CS line negated)
        SSIDR_0 = (unsigned char)(ulPageNumberOffset >> 16);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
		#endif
        SSIDR_0 = (unsigned char)(ulPageNumberOffset >> 8);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
		#endif
        SSIDR_0 = (unsigned char)ulPageNumberOffset;
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
        SSISR_0 &= ~SSI_BSY;                                             // clear flag for simulator
		#endif
		SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the chip will be busy for some time
        break;

    case SUB_SECTOR_ERASE:                                               // erase one sub-sector
    case SECTOR_ERASE:                                                   // erase one sector
        SSIDR_0 = (unsigned char)(ulPageNumberOffset >> 16);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
		#endif
        SSIDR_0 = (unsigned char)(ulPageNumberOffset >> 8);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
		#endif
        SSIDR_0 = 0;                                                     // page offset is don't care
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
        SSISR_0 &= ~SSI_BSY;                                             // clear flag for simulator
		#endif
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case READ_DATA_BYTES:                                                // read data from the chip
        SSIDR_0 = (unsigned char)(ulPageNumberOffset >> 16);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
		#endif
        SSIDR_0 = (unsigned char)(ulPageNumberOffset >> 8);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
		#endif
        SSIDR_0 = (unsigned char)(ulPageNumberOffset);
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0);         // simulate the SPI FLASH device
        SSISR_0 &= ~SSI_BSY;                                             // clear flag for simulator
		#endif
        ucTxCount = 3;
      //SPI_FLASH_Danger[iChipSelect] = 1;                               // {1} in this case this marks that no 0xff have been set to tx when reading (it will be cleared before exiting)
        break;

    case READ_MANUFACTURER_ID:                                           // get then manufacturer and chip size information
        SSIDR_0 = 0xff;                                                  // dummy writes
        SSIDR_0 = 0xff;
        ucTxCount = 3;
                                                                         // fall through intentionally
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        SSIDR_0 = 0xff;
		if (ucCommand == READ_STATUS_REGISTER) {
		    ucTxCount = 1;
		}
        while (SSISR_0 & SSI_BSY) {};                                    // wait for transfer to complete
		*ucData = (unsigned char)SSIDR_0;                                // dummy read
        while (DataLength--) {
		#ifdef _WINDOWS
            SSIDR_0 = fnSimSPI_Flash(STM25PXXX_READ, (unsigned char)SSIDR_0); // simulate the SPI FLASH device
		#endif
            *ucData++ = (unsigned char)SSIDR_0;
        }
        GPIODATA_A |= ucChipSelectLine;                                  // deassert SS when complete
        REMOVE_SPI_FLASH_MODE();                                         // {2}
		#ifdef _WINDOWS
        fnSimSPI_Flash(STM25PXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
		#endif
        return;

    case BULK_ERASE:
        ucTxCount = 0;
        DataLength = 0;
        break;
        // Note - no default here. This must never be called with non-supported command since it will hang.
    }

    while (DataLength != 0) {                                            // while data bytes to be read or written
        int iDataCnt = 0;                                                // FIFO block counter

        if (ucTxCount != 0) {
            while (SSISR_0 & (SSI_BSY | SSI_RNE)) {                      // wait for transfer to complete and clear rx buffer
                volatile unsigned long ulDummy = SSIDR_0;
#ifdef _WINDOWS
                SSISR_0 &= ~(SSI_RNE | SSI_BSY);
#endif
            }        
        }

        if (ucCommand != READ_DATA_BYTES) {                              // write type
            while (DataLength != 0) {                                    // for each byte in the FIFO transfer block
                SSIDR_0 = *ucData++;                                     // send data
                DataLength--;
#ifdef _WINDOWS
                fnSimSPI_Flash(STM25PXXX_WRITE, (unsigned char)SSIDR_0); // simulate the SPI FLASH device
#endif
                if (++iDataCnt >= 8) {                                   // maximum FIFO depth
                    break;
                }
            }
        }
        else {                                                           // read rather than write
            ucTxCount = 0;                                               // stop polling of status at begin of loop
            if (DataLength > 8) {                                        // maximum FIFO depth
                iDataCnt = 7;
                DataLength -= 8;                                         // complete block
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
                SSIDR_0 = 0xff;
            }
            else {                                                       // last transfer
                int iSetIdle;
                iSetIdle = iDataCnt = (DataLength - 1);                  // last transfer block
                DataLength = 0;                
                while (iSetIdle-- >= 0) {
                    SSIDR_0 = 0xff;                                      // fill output FIFO with 0xff so that output remains '1'
                }
            }

            do {                                                         // collect each byte
#ifdef _WINDOWS
                SSISR_0 |= SSI_RNE;
#endif
                while (!(SSISR_0 & SSI_RNE)) {}                          // wait only if no reception ready
#ifdef _WINDOWS
                SSIDR_0 = fnSimSPI_Flash(STM25PXXX_READ, (unsigned char)0);// simulate the SPI FLASH device
#endif
                *ucData++ = (unsigned char)SSIDR_0;                      // read bytes to the return buffer
            } while (iDataCnt--);
        }
    }
    if (ucCommand != READ_DATA_BYTES) {                                  // commands that have only written wait here for transmission to complete
    }
    GPIODATA_A |= ucChipSelectLine;                                      // deassert SS when complete
    REMOVE_SPI_FLASH_MODE();                                             // {2}
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
    volatile unsigned long ulDelay = ((MASTER_CLOCK/12000) * 15);        // 15ms start up delay

		#ifdef SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        while (ulDelay--) {};                                            // start up delay to ensure SPI FLASH ready
    }
		#else
    while (ulDelay--) {};                                                // start up delay to ensure SPI FLASH ready
		#endif


    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ST) {                                 // {3}
        switch (ucID[2]) {
        case (DEVICE_ID_1_DATA_ST_FLASH_512K):                           // 512kBit / 64kByte
            return STM25P05;
        case (DEVICE_ID_1_DATA_ST_FLASH_1M):                             // 1MBit / 128kByte
            return STM25P10;
        case (DEVICE_ID_1_DATA_ST_FLASH_2M):                             // 2MBit / 256kByte
            return STM25P20;
        case (DEVICE_ID_1_DATA_ST_FLASH_4M):                             // 4MBit / 0.5MegByte
            return STM25P40;
        case (DEVICE_ID_1_DATA_ST_FLASH_8M):                             // 8MBit / 1MegByte
            return STM25P80;
        case (DEVICE_ID_1_DATA_ST_FLASH_16M):                            // 16MBit / 2MegByte
            return STM25P16;
        case (DEVICE_ID_1_DATA_ST_FLASH_32M):                            // 32MBit / 4MegByte
            return STM25P32;
        case (DEVICE_ID_1_DATA_ST_FLASH_64M):                            // 64MBit / 8MegByte
            return STM25P64;
        case (DEVICE_ID_1_DATA_ST_FLASH_128M):                           // 128MBit / 16MegByte
            return STM25P128;
        }
    }

    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}

#endif

#endif

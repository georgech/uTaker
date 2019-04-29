/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      spi_flash_interface.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if defined SPI_FLASH_SECOND_SOURCE_MODE
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    static void(*fnSPI_command)(unsigned char, unsigned long, int iChipSelect, volatile unsigned char *, MAX_FILE_LENGTH) = 0;
    #else
    static void (*fnSPI_command)(unsigned char, unsigned long, volatile unsigned char *, MAX_FILE_LENGTH) = 0;
    #endif
#else
    #if defined SPI_FLASH_MX25L
        #define fnSPI_command    fnSPI_command_MX25L
    #elif defined SPI_FLASH_W25Q
        #define fnSPI_command    fnSPI_command_w25q
    #elif defined SPI_FLASH_IS25
        #define fnSPI_command    fnSPI_command_is25
    #endif
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    #if !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K && !defined SPI_FLASH_MX25L && !defined SPI_FLASH_IS25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(ptrAccessDetails->ulOffset - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif

    #if defined SPI_FLASH_ST
    fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_IS25
    fnSPI_command(READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #else                                                                // ATMEL
        #if SPI_FLASH_PAGE_LENGTH >= 1024
            #if SPI_FLASH_PAGE_LENGTH == 1024                            // {108}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #elif SPI_FLASH_PAGE_LENGTH >= 512
            #if SPI_FLASH_PAGE_LENGTH == 512                            // {108}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #else
            #if SPI_FLASH_PAGE_LENGTH == 256                            // {108}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #endif
    #endif
}

// This routine writes data from a buffer to an area in SPI Flash (the caller has already defined the data to a particular area and device)
//
static void fnWriteSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned long Destination = ptrAccessDetails->ulOffset;
    unsigned short usDataLength;
    #if defined SPI_FLASH_SST25
    int iMultipleWrites = 0;
    if (Length == 0) {
        return;                                                          // ignore if length is zero
    }
    if ((Destination & 0x1) != 0) {                                      // start at odd SPI address, requires an initial byte write
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // command write enable to allow byte programming
        fnSPI_command(BYTE_PROG, Destination++, _EXTENDED_CS ptrBuffer++, 1);// program last byte 
        if (--Length == 0) {                                             // single byte write so complete
            return ;
        }
    }
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow programming
    #else
    unsigned long ulPageNumber = (unsigned long)(Destination/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(Destination - (ulPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif
    while (Length != 0) {
        usDataLength = (unsigned short)Length;
    #if defined SPI_FLASH_ST || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_IS25
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // write enable
        #if defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_IS25
        fnSPI_command(PAGE_PROG, ((ulPageNumber * SPI_FLASH_PAGE_LENGTH) | usPageOffset), _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        #else
        fnSPI_command(PAGE_PROG, ((ulPageNumber << 8) | usPageOffset), _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        #endif
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        ulPageNumber++;
        usPageOffset = 0;
    #elif defined SPI_FLASH_SST25
        #if defined SST25_A_VERSION
            #define AAI_LENGTH 1
        #else
            #define AAI_LENGTH 2
        #endif
        if (usDataLength > 1) {
            if (iMultipleWrites == 0) {
                fnSPI_command(AAI_WORD_PROG, Destination, _EXTENDED_CS ptrBuffer, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                iMultipleWrites = 1;                                     // mark that we are in a AAI sequence
            }
            else {
                fnSPI_command(AAI_WORD_PROG, Destination, _EXTENDED_CS ptrBuffer, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
            }
            Destination += AAI_LENGTH;
            Length -= AAI_LENGTH;
            ptrBuffer += AAI_LENGTH;
        }
        else {
            if (iMultipleWrites != 0) {
                fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);      // first close AAI sequence
                fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);       // command write enable to allow byte programming
            }
            fnSPI_command(BYTE_PROG, Destination, _EXTENDED_CS ptrBuffer, 1); // program last byte 
            break;               
        }
    #else                                                                // ATMEL
        if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) {   // are we writing a partial page?
            fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
        }
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        usPageNumber++;
        usPageOffset = 0;
    #endif
    }
    #if defined SPI_FLASH_ST || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_IS25
    fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);                  // disable writes on exit
    #endif
}

// The routine is used to delete an area in SPI Flash, whereby the caller has set the address to the start of a page and limited the erase to a single storage area and device
//
static MAX_FILE_LENGTH fnDeleteSPI(ACCESS_DETAILS *ptrAccessDetails)
{
    MAX_FILE_LENGTH BlockLength = SPI_FLASH_PAGE_LENGTH;
    #if !defined SPI_FLASH_ST
    unsigned char  ucCommand;
    #endif
    #if !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K && !defined SPI_FLASH_MX25L && !defined SPI_FLASH_IS25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    #endif
    #if defined SPI_FLASH_ST
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // enable the write/erase
        #if defined SPI_DATA_FLASH
    fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
    BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
        #else
    fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
    BlockLength = SPI_FLASH_SECTOR_LENGTH;
        #endif
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_IS25
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow erasing
        #if !defined SST25_A_VERSION
    if ((ptrAccessDetails->BlockLength >= (64 * 1024)) && ((ptrAccessDetails->ulOffset & ((64 * 1024) - 1)) == 0)) { // if a complete 64k block can be deleted
            #if defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_W25Q || defined SPI_FLASH_IS25
        ucCommand = BLOCK_ERASE;                                         // delete block of 64k
            #else
        ucCommand = SECTOR_ERASE;                                        // delete block of 64k
            #endif
        BlockLength = (64 * 1024);
    }
            #if defined SPI_FLASH_HALF_SECTOR_LENGTH
    else if ((ptrAccessDetails->BlockLength >= (SPI_FLASH_HALF_SECTOR_LENGTH)) && ((ptrAccessDetails->ulOffset & ((SPI_FLASH_HALF_SECTOR_LENGTH) - 1)) == 0)) { // if a complete half block can be deleted
        ucCommand = HALF_BLOCK_ERASE;                                    // delete half block
        BlockLength = (SPI_FLASH_HALF_SECTOR_LENGTH);
    }
            #endif
    else 
        #endif
        #if defined SPI_FLASH_S25FL1_K || defined SPI_FLASH_MX25L || defined SPI_FLASH_W25Q || defined SPI_FLASH_IS25
    {
        ucCommand = SECTOR_ERASE;                                        // delete smallest sector of 4k
            #if defined SPI_FLASH_SUB_SECTOR_LENGTH
        BlockLength = (SPI_FLASH_SUB_SECTOR_LENGTH);
            #else
        BlockLength = (SPI_FLASH_SECTOR_LENGTH);
            #endif
    }
        #else
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
        ucCommand = HALF_SECTOR_ERASE;                                   // delete block of 32k
        BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
    }
    else {
        ucCommand = SUB_SECTOR_ERASE;                                    // delete smallest sector of 4k
        BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
    }
        #endif
    fnSPI_command(ucCommand, ptrAccessDetails->ulOffset, _EXTENDED_CS 0, 0); // command the erase
    #else                                                                // ATMEL
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber % 8 == 0)) { // if delete range corresponds to a block, use faster block delete
        BlockLength = SPI_FLASH_BLOCK_LENGTH;
        ucCommand = BLOCK_ERASE;
    }
    else {
        BlockLength = SPI_FLASH_PAGE_LENGTH;
        ucCommand = PAGE_ERASE;
    }
    fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);           // delete appropriate page/block
    #endif
    return (BlockLength);                                                // the block size that was erased
}

#if defined SPI_EEPROM_ENABLED                                           // {202}
// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI_EEPROM(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    fnSPI_EEPROM_command(EEPROM_READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
}

// This routine writes data from a buffer to an area in SPI Flash (the caller has already defined the data to a particular area and device)
//
static void fnWriteSPI_EEPROM(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned long Destination = ptrAccessDetails->ulOffset;
    unsigned short usDataLength;
    unsigned short usPageNumber = (unsigned short)(Destination/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(Destination - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    while (Length != 0) {
        usDataLength = (unsigned short)Length;
        if (usDataLength > (SPI_EEPROM_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_EEPROM_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_EEPROM_command(EEPROM_WRITE_ENABLE, 0, _EXTENDED_CS 0, 0); // write enable
        fnSPI_EEPROM_command(PAGE_PROG, ((usPageNumber * SPI_EEPROM_PAGE_LENGTH)) | usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        usPageNumber++;
        usPageOffset = 0;
    }
    fnSPI_EEPROM_command(EEPROM_WRITE_DISABLE, 0, _EXTENDED_CS 0, 0); // write disable (this will generally not be necessary since it is automatically set after each successful write operation=
}
#endif

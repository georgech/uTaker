/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        STM32_boot.c
    Project:     Single Chip Embedded Internet - boot loader hardware support
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    06.03.2012 Add start_application()                                   {1}
    12.12.2012 Correct flash erase routine for F1 parts                  {2}
    02.10.2013 Correct STM32F1xx flash control                           {3}
    20.01.2017 Add 2MByte Flash support                                  {4}
    18.06.2018 Change uMemset() to match memset() parameters             {5}


*/

#if defined _STM32

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#if defined _WINDOWS
    #include "config.h"
    #define INITHW  extern
    #define START_CODE 0
    unsigned long ulFlashLockState = FLASH_CR_LOCK;

    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
#else
    #define STM32_LowLevelInit main
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #if defined _COMPILE_KEIL
        extern void __main(void);                                        // Keil library initialisation routine
        #define START_CODE __main
    #elif defined COMPILE_IAR
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE __iar_program_start
      //#define _main main
    #elif defined ROWLEY && !defined ROWLEY_2                            // if project uses Rowley < V1.7 build 17
        #define START_CODE _main2
    #else                                                                // general GCC
        #define START_CODE main
    #endif
    #define SIM_DMA(x)
#endif


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);
}

#ifndef _COMPILE_KEIL                                                    // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)           // {1}
{
    #ifndef _WINDOWS
    asm(" ldr sp, [r0,#0]");
    asm(" ldr pc, [r0,#4]");
    #endif
}
#endif


// CRC-16 routine
//
extern unsigned short fnCRC16(unsigned short usCRC, unsigned char *ptrInput, unsigned short usBlockSize)
{
    while (usBlockSize--) {
        usCRC = (unsigned char)(usCRC >> 8) | (usCRC << 8);
        usCRC ^= *ptrInput++;
        usCRC ^= (unsigned char)(usCRC & 0xff) >> 4;
        usCRC ^= (usCRC << 8) << 4;
        usCRC ^= ((usCRC & 0xff) << 4) << 1;
    }
    return usCRC;
}



#if defined SPI_SW_UPLOAD
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_STM32_atmel.h"
            #include "spi_flash_STM32_stmicro.h"
            #include "spi_flash_STM32_sst25.h"
        #undef _SPI_DEFINES
    #endif

    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #ifdef CS2_LINE
            ,CS2_LINE
            #ifdef CS3_LINE
            ,CS3_LINE
            #endif
        #endif
        };
        #define EXTENDED_CS , &iChipSelect
        #define _EXTENDED_CS  ptrAccessDetails->ucDeviceNumber,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent SPI Flash driver code
        #include "spi_flash_STM32_atmel.h"
        #include "spi_flash_STM32_stmicro.h"
        #include "spi_flash_STM32_sst25.h"
    #undef _SPI_FLASH_INTERFACE
#endif






// memcpy implementation
//
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr1 = (unsigned char *)ptrTo;
    unsigned char *ptr2 = (unsigned char *)ptrFrom;

    while (Size--) {
        *ptr1++ = *ptr2++;
    }

    return buffer;
}


#if defined _STM32F2XX || defined _STM32F4XX
// The STM32F2xx and STM32F4xx have variable flash granularity - this routine determines the size of the flash sector that the access is in as well as the sector's number
//
static unsigned long fnGetFlashSectorSize(unsigned char *ptrSector, unsigned long *ulSectorNumber)
{
    #if SIZE_OF_FLASH >= (2 * 1024 * 1024)                               // {4}
    if (ptrSector < (unsigned char *)(FLASH_START_ADDRESS + (1 * 1024 * 1024))) {
        *ulSectorNumber = 0;                                             // access in first bank
    }
    else {
        ptrSector -= (1 * 1024 * 1024);
        *ulSectorNumber = 16;                                            // access in second bank (note that the first sector in teh secdn bank is named as sector 1 but it has to be addressed as sector 16!)
    }
    #else
    *ulSectorNumber = 0;
    #endif
    if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT) + (NUMBER_OF_PARAMETER_SECTORS * FLASH_GRANULARITY_PARAMETER))) { // {22}
        ptrSector -= (FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT) + (NUMBER_OF_PARAMETER_SECTORS * FLASH_GRANULARITY_PARAMETER));
        *ulSectorNumber += ((NUMBER_OF_BOOT_SECTORS + NUMBER_OF_PARAMETER_SECTORS) + (((CAST_POINTER_ARITHMETIC)ptrSector)/FLASH_GRANULARITY));
        return FLASH_GRANULARITY;                                        // access in code area
    }
    else if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT))) {
        #if NUMBER_OF_PARAMETER_SECTORS > 1
        ptrSector -= (FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT));
        *ulSectorNumber += ((NUMBER_OF_BOOT_SECTORS) + (((CAST_POINTER_ARITHMETIC)ptrSector)/FLASH_GRANULARITY_PARAMETER));
        #else
        *ulSectorNumber += (NUMBER_OF_BOOT_SECTORS);
        #endif
        return FLASH_GRANULARITY_PARAMETER;                              // access in parameter area
    }
    else {
        ptrSector -= (FLASH_START_ADDRESS);
        *ulSectorNumber += (((CAST_POINTER_ARITHMETIC)ptrSector)/FLASH_GRANULARITY_BOOT);
        return FLASH_GRANULARITY_BOOT;                                   // access in boot area
    }
}
#endif

#if defined SPI_SW_UPLOAD
// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    #if !defined SPI_FLASH_SST25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(ptrAccessDetails->ulOffset - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif

    #if defined SPI_FLASH_ST
    fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #elif defined SPI_FLASH_SST25
    fnSPI_command(READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #else                                                                // ATMEL
        #if SPI_FLASH_PAGE_LENGTH >= 1024
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #elif SPI_FLASH_PAGE_LENGTH >= 512
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #endif
    #endif
}

// The routine is used to delete an area in SPI Flash, whereby the caller has set the address to the start of a page and limited the erase to a single storage area and device
//
static MAX_FILE_LENGTH fnDeleteSPI(ACCESS_DETAILS *ptrAccessDetails)
{
    MAX_FILE_LENGTH BlockLength = SPI_FLASH_PAGE_LENGTH;
    #if !defined SPI_FLASH_ST
    unsigned char   ucCommand;
    #endif
    #if !defined SPI_FLASH_SST25
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    #endif
    #if defined SPI_FLASH_ST
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // enable the write
        #ifdef SPI_DATA_FLASH
    fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
    BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
        #else
    fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
    BlockLength = SPI_FLASH_SECTOR_LENGTH;
        #endif
    #elif defined SPI_FLASH_SST25
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow byte programming
        #ifndef SST25_A_VERSION
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_SECTOR_LENGTH - 1)) == 0)) { // if a complete 64k sector can be deleted
        ucCommand = SECTOR_ERASE;                                        // delete block of 64k
        BlockLength = SPI_FLASH_SECTOR_LENGTH;
    }
    else 
        #endif
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
        ucCommand = HALF_SECTOR_ERASE;                                   // delete block of 32k
        BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
    }
    else {
        ucCommand = SUB_SECTOR_ERASE;                                    // delete smallest sector of 4k
        BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
    }
    fnSPI_command(ucCommand, ptrAccessDetails->ulOffset, _EXTENDED_CS 0, 0);    
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
    return (BlockLength);
}

#endif

extern int uFileErase(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
#if defined _STM32F2XX || defined _STM32F4XX
    unsigned long _ulSectorSize;
    unsigned long ulSectorNumber;
#else
    #define _ulSectorSize  FLASH_GRANULARITY                             // {2}
#endif
#if defined SPI_SW_UPLOAD
    if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // if in SPI flash
        ACCESS_DETAILS AccessDetails;
        Length += (((CAST_POINTER_ARITHMETIC)(ptrSector - (FLASH_START_ADDRESS + SIZE_OF_FLASH))) - ((CAST_POINTER_ARITHMETIC)(ptrSector - (FLASH_START_ADDRESS + SIZE_OF_FLASH)) & ~(SPI_FLASH_PAGE_LENGTH - 1)));
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(SPI_FLASH_PAGE_LENGTH - 1)); // set to sector boundary
        AccessDetails.ulOffset = (unsigned long)(ptrSector - (FLASH_START_ADDRESS + SIZE_OF_FLASH)); // offset in spi flash
        while (Length != 0) {
            AccessDetails.BlockLength = Length;
            AccessDetails.BlockLength = fnDeleteSPI(&AccessDetails);     // delete page/block in SPI flash
            if (Length <= AccessDetails.BlockLength) {
                break;
            }
            Length -= AccessDetails.BlockLength;                         // length reduced by the last block deletion length
            AccessDetails.ulOffset += AccessDetails.BlockLength;         // offset increased by the last block deletion length
        }
        return 0;
    }
#endif
    do {
#if defined _STM32F2XX || defined _STM32F4XX
        _ulSectorSize = fnGetFlashSectorSize(ptrSector, &ulSectorNumber);
#endif
        Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)));
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)); // set to sector boundary
        if (FLASH_CR & FLASH_CR_LOCK) {                                  // if the flash has not been unlocked, unlock it before erasing
            FLASH_KEYR = FLASH_KEYR_KEY1;
            FLASH_KEYR = FLASH_KEYR_KEY2;
#ifdef _WINDOWS
            FLASH_CR &= ~FLASH_CR_LOCK;
            ulFlashLockState = 0;
#endif
        }
#if defined _STM32F2XX || defined _STM32F4XX
        FLASH_CR = (FLASH_CR_SER | MAXIMUM_PARALLELISM | (ulSectorNumber << FLASH_CR_SNB_SHIFT)); // prepare the section to be deleted
        FLASH_CR |= (FLASH_CR_STRT);                                     // start the erase operation
#else
        FLASH_CR = FLASH_CR_PER;                                         // {3} select page erase
        FLASH_SR = (FLASH_SR_PGERR | FLASH_SR_WRPRTERR | FLASH_SR_EOP);  // {3} reset status flags
    #if defined _WINDOWS                                                 // {3}
        FLASH_SR = 0;                                                    // reset self-clearing status bits
    #endif
        FLASH_AR = (CAST_POINTER_ARITHMETIC)ptrSector;                   // set pointer to first location in the page to be erased
        FLASH_CR = (FLASH_CR_PER | FLASH_CR_STRT);                       // {3} start page erase operation
      //FLASH_CR |= FLASH_CR_STRT;                                       // start page erase operation
#endif
#ifdef _WINDOWS
        FLASH_CR |= ulFlashLockState;
        if (FLASH_CR & FLASH_CR_LOCK) {                                  // if lock bit set don't erase
            FLASH_SR |= FLASH_ERROR_FLAGS;
        }
        else {
            uMemset(fnGetFlashAdd(ptrSector), 0xff, _ulSectorSize);      // delete the page content
        }
#endif
        while (FLASH_SR & FLASH_SR_BSY) {}                               // wait until delete operation completes
        if (FLASH_SR & FLASH_ERROR_FLAGS) {
            return -1;                                                   // erase error
        }
        if (Length <= _ulSectorSize) {
            FLASH_CR = FLASH_CR_LOCK;                                    // lock flash when complete
#ifdef _WINDOWS
            ulFlashLockState = FLASH_CR_LOCK;
#endif
#ifdef MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size
	            return (int)_ulSectorSize;
	        }
#endif
            break;
        }
        ptrSector += _ulSectorSize;                                      // advance sector point to next internal flash sector
        Length -= _ulSectorSize;
    } while (1);
    return 0;
}

#if defined _STM32F2XX || defined _STM32F4XX
static int fnSingleByteFlashWrite(unsigned char *ucDestination, unsigned char ucData)
{
    if (*(unsigned char *)fnGetFlashAdd((unsigned char *)ucDestination) == ucData) {
        return 0;                                                        // if the value is already programmed in flash there is no need to write
    }
    FLASH_CR = FLASH_CR_PG;                                              // select byte programming
    #ifdef _WINDOWS
    FLASH_CR |= ulFlashLockState;
    if (FLASH_CR & FLASH_CR_LOCK) {                                      // if lock bit set don't program
        FLASH_SR |= FLASH_ERROR_FLAGS;
    }
    else {
    #endif
        *((unsigned char *)fnGetFlashAdd((unsigned char *)ucDestination)) = ucData; // program the byte
    #ifdef _WINDOWS
    }
    #endif
    while (FLASH_SR & FLASH_SR_BSY) {}                                   // wait until write operation completes
    if (FLASH_SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)) { // check for errors
        return -1;                                                       // write error
    }
    return 0;
}

    #if SUPPLY_VOLTAGE > SUPPLY_1_8__2_1                                 // short word writes are only possible when the supply voltage is greater than 2.1V
static int fnSingleWordFlashWrite(unsigned short *usDestination, unsigned short usData)
{
    if (*(unsigned short *)fnGetFlashAdd((unsigned char *)usDestination) == usData) {
        return 0;                                                        // if the value is already programmed in flash there is no need to write
    }
    FLASH_CR = (FLASH_CR_PG | FLASH_CR_PSIZE_16);                        // select short word programming
    #ifdef _WINDOWS
    FLASH_CR |= ulFlashLockState;
    if (FLASH_CR & FLASH_CR_LOCK) {                                      // if lock bit set don't program
        FLASH_SR |= FLASH_ERROR_FLAGS;
    }
    else {
    #endif
        *((unsigned short *)fnGetFlashAdd((unsigned char *)usDestination)) = usData; // program the byte
    #ifdef _WINDOWS
    }
    #endif
    while (FLASH_SR & FLASH_SR_BSY) {}                                   // wait until write operation completes
    if (FLASH_SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)) { // check for errors
        return -1;                                                       // write error
    }
    return 0;
}
    #endif

    #if SUPPLY_VOLTAGE >= SUPPLY_2_7__3_6                                 // long word writes are only possible when the supply voltage is greater than 2.7V
static int fnSingleLongWordFlashWrite(unsigned long *ulDestination, unsigned long ulData)
{
    if (*(unsigned long *)fnGetFlashAdd((unsigned char *)ulDestination) == ulData) {
        return 0;                                                        // if the value is already programmed in flash there is no need to write
    }
    FLASH_CR = (FLASH_CR_PG | FLASH_CR_PSIZE_32);                        // select long short word programming
    #ifdef _WINDOWS
    FLASH_CR |= ulFlashLockState;
    if (FLASH_CR & FLASH_CR_LOCK) {                                      // if lock bit set don't program
        FLASH_SR |= FLASH_ERROR_FLAGS;
    }
    else {
    #endif
        *((unsigned long *)fnGetFlashAdd((unsigned char *)ulDestination)) = ulData; // program the byte
    #ifdef _WINDOWS
    }
    #endif
    while (FLASH_SR & FLASH_SR_BSY) {}                                   // wait until write operation completes
    if (FLASH_SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)) { // check for errors
        return -1;                                                       // write error
    }
    return 0;
}
    #endif
#endif

static int fnWriteInternalFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ucData)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned char *ucDestination = (unsigned char *)ptrAccessDetails->ulOffset;

    if (FLASH_CR & FLASH_CR_LOCK) {                                      // if the flash has not been unlocked, unlock it before programming
        FLASH_KEYR = FLASH_KEYR_KEY1;
        FLASH_KEYR = FLASH_KEYR_KEY2;
    #ifdef _WINDOWS
        FLASH_CR &= ~FLASH_CR_LOCK;
        ulFlashLockState = 0;
    #endif
    }

    #if defined _STM32F2XX || defined _STM32F4XX                         // depending on the power supply range it is possible to write bytes, short- and long words (with external Vpp 64 bits but this is not supported in this implementation)
    FLASH_SR = (FLASH_STATUS_FLAGS);                                     // reset status flags
        #ifdef _WINDOWS
    FLASH_SR = 0;
        #endif
        #if SUPPLY_VOLTAGE == SUPPLY_1_8__2_1                            // only byte wise programming is possible
    while (Length-- != 0) {        
        if (fnSingleByteFlashWrite(ucDestination, *ucData) != 0) {
            return 1;                                                    // write error
        }
        ucDestination++;
        ucData++;
    }
        #elif SUPPLY_VOLTAGE < SUPPLY_2_7__3_6                           // byte or half-word programming possible
    {
        unsigned short usWord;
        int iCollected = 0;
        while (Length != 0) {
            if (iCollected == 0) {
                if ((Length == 1) || (((CAST_POINTER_ARITHMETIC)ucDestination) & 0x1)) { // single byte write (or final byte write) or misaligned half-word
                    if (fnSingleByteFlashWrite(ucDestination, *ucData) != 0) { // perform single byte programming
                        return 1;                                        // write error
                    }
                    ucDestination++;
                }
                else {
                    usWord = *ucData;                                    // collect in little endian format
                    iCollected++;
                }
            }
            else {
                usWord |= (*ucData << 8);
                if (fnSingleWordFlashWrite((unsigned short *)ucDestination, usWord) != 0) { // perform a short word write
                    return 1;
                }
                ucDestination += 2;
                iCollected = 0;
            }
            ucData++;
            Length--;
        }
    }
        #else                                                            // byte, half-word and word programming possible when aligned
    {
        unsigned long  ulLongWord = 0;
        unsigned short usWord = 0;
        int iCollected = 0;
        while (Length != 0) {
            if (iCollected == 0) {
                if ((Length == 1) || (((CAST_POINTER_ARITHMETIC)ucDestination) & 0x1)) { // single byte write (or final byte write) or misaligned half-word
                    if (fnSingleByteFlashWrite(ucDestination, *ucData) != 0) { // perform single byte programming
                        return 1;                                        // write error
                    }
                    ucDestination++;
                }
                else {
                    usWord = *ucData;                                    // collect in little endian format
                    iCollected++;
                }
            }
            else if (iCollected == 1) {
                usWord |= (*ucData << 8);
                iCollected++;
                if ((Length <= 2) || ((((CAST_POINTER_ARITHMETIC)(ucDestination + 2)) & 0x3) == 0)) { // last short word or ending on a 128 bit line boundary
                    if (fnSingleWordFlashWrite((unsigned short *)ucDestination, usWord) != 0) { // perform a short word write
                        return 1;
                    }
                    ucDestination += 2;
                    iCollected = 0;
                }
            }
            else if (iCollected == 2) {
                ulLongWord = (usWord | (*ucData << 16));
                iCollected++;
            }
            else {
                ulLongWord |= (*ucData << 24);
                if (fnSingleLongWordFlashWrite((unsigned long *)ucDestination, ulLongWord) != 0) { // perform a long word write
                    return 1;
                }
                ucDestination += 4;
                iCollected = 0;
            }
            ucData++;
            Length--;
        }
    }
        #endif
    #else
    {
        unsigned short usValue;
        // The STM32F1xx writes always in short words so the start is expected to be aligned and the length to be a multiple of half words
        //
        while (Length != 0) {
            usValue = *ucData++;                                         // little endian format
            usValue |= (*ucData++ << 8);
            if (*(unsigned short *)fnGetFlashAdd((unsigned char *)ucDestination) != usValue) { // if the value is already programmed skip the write
                FLASH_CR = FLASH_CR_PG;                                  // select half-word programming
                FLASH_SR = (FLASH_STATUS_FLAGS);                         // reset status flags
                #ifdef _WINDOWS
                FLASH_SR = 0;
                #endif
                *(volatile unsigned short *)fnGetFlashAdd((unsigned char *)ucDestination) = usValue; // write the value to the flash location
                while (FLASH_SR & FLASH_SR_BSY) {}                       // wait until write operation completes
                if (FLASH_SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR)) {   // check for errors
                    return 1;                                            // write error
                }
            }
            if (Length <= 2) {
                break;
            }
            ucDestination += 2;
            Length -= 2;
        }
    }
    #endif
    FLASH_CR = FLASH_CR_LOCK;                                            // lock flash when complete
    #ifdef _WINDOWS
    ulFlashLockState = FLASH_CR_LOCK;
    #endif
    return 0;
}

// RAW write of data to non-volatile memory
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    ACCESS_DETAILS AccessDetails;
    AccessDetails.ulOffset = (unsigned long)ucDestination;
    AccessDetails.BlockLength = Length;
    return (fnWriteInternalFlash(&AccessDetails, ucData));
}


extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#if defined SPI_SW_UPLOAD
    if (ParLocation >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // if in SPI flash
        ACCESS_DETAILS AccessDetails;
        AccessDetails.ulOffset = (unsigned long)(ParLocation - (FLASH_START_ADDRESS + SIZE_OF_FLASH)); // offset in SPI flash
        AccessDetails.BlockLength = Size;
        fnReadSPI(&AccessDetails, ptrValue);                             // read from the SPI device
        return;
    }
#endif
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // directly copy memory since this must be a pointer to code (embedded file)
}


#ifdef SPI_SW_UPLOAD
extern int fnConfigSPIFileSystem(void)
{
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code to detect the SPI Flash devices
        #include "spi_flash_STM32_atmel.h"
        #include "spi_flash_STM32_stmicro.h"
        #include "spi_flash_STM32_sst25.h"
    #undef _CHECK_SPI_CHIPS
    return (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE);
}
#endif












#if !defined _MINIMUM_IRQ_INITIALISATION
// Serious error interrupts
//
static void irq_hard_fault(void)
{
}

static void irq_memory_man(void)
{
}

static void irq_bus_fault(void)
{
}

static void irq_usage_fault(void)
{
}

static void irq_debug_monitor(void)
{
}

static void irq_NMI(void)
{
}

static void irq_pend_sv(void)
{
}

static void irq_SVCall(void)
{
}
#endif

// Default handler to indicate processor input from unconfigured source
//
static void irq_default(void)
{
}

// Delay loop for simple but accurate short delays (eg. for stabilisation delays)
//
extern void fnDelayLoop(unsigned long ulDelay_us)
{
    #define LOOP_FACTOR  14000000                                        // tuned
    volatile unsigned long ulDelay = ((SYSCLK/LOOP_FACTOR) * ulDelay_us);
    while (ulDelay--) {}                                                 // simple loop tuned to perform us timing
}


#if defined _GNU
    extern const RESET_VECTOR reset_vect;                                // force GCC to link the reset table
    volatile void *ptrTest1;
#endif

#if defined (_GNU)
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

extern void __init_gnu_data(void)
{
    unsigned char *ptrData = &__data_start__;
    const unsigned char *ptrFlash = &__data_load_start__;
    unsigned long ulInitDataLength = (&__data_end__ - &__data_start__);
    while (ulInitDataLength--) {                                         // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrData != ptrFlash) {
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength--) {                                     // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }

    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }

    ptrTest1 = (void *)&reset_vect;                                      // force GCC to link the reset table
}
#endif


// Perform very low level initialisation - called by the start up code
//
extern int STM32_LowLevelInit(void)
{
#if !defined _MINIMUM_IRQ_INITIALISATION
    void ( **processor_ints )( void );
#endif
    VECTOR_TABLE *ptrVect;

    RCC_CR = (0x00000080 | RCC_CR_HSIRDY | RCC_CR_HSION);                // set reset state - default is high-speed internal clock
    RCC_CFGR = 0;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    RCC_PLLCFGR = RCC_PLLCFGR_RESET_VALUE;                               // set the PLL configuration register to default
#endif
#if !defined USE_HSI_CLOCK
    RCC_CR = (0x00000080 | RCC_CR_HSIRDY | RCC_CR_HSION | RCC_CR_HSEON); // enable the high-speed external clock
#endif
#if defined _STM32F2XX || defined _STM32F4XX
    FLASH_ACR = (FLASH_ACR_ICRST | FLASH_ACR_DCRST);                     // flush data and instruction cache
    FLASH_ACR = (/*FLASH_ACR_PRFTEN | */FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_WAIT_STATES); // set flash wait states appropriately and enable pre-fetch buffer anyd cache
    RCC_CFGR = (_RCC_CFGR_HPRE_SYSCLK | _RCC_CFGR_PPRE1_HCLK | _RCC_CFGR_PPRE2_HCLK); // set HCLK (AHB), PCLK1 and PCLK2 speeds
#elif defined _CONNECTIVITY_LINE
    FLASH_ACR = (FLASH_ACR_PRFTBE | FLASH_WAIT_STATES);                  // set flash wait states appropriately and enable pre-fetch buffer
    RCC_CFGR = (RCC_CFGR_HPRE_SYSCLK | RCC_CFGR_PPRE1_HCLK_DIV2 | RCC_CFGR_PPRE2_HCLK); // set HCLK to SYSCLK, PCLK2 to HCLK and PCLK1 to HCLK/2 - PCLK1 must not be greater than SYSCLK/2
#else
    FLASH_ACR = 0; //FLASH_ACR_HLFCYA;                                   // enable half-cycle access - to do???
    RCC_CFGR = (RCC_CFGR_HPRE_SYSCLK | RCC_CFGR_PPRE1_HCLK_DIV2 | RCC_CFGR_PPRE2_HCLK); // set HCLK to SYSCLK, PCLK2 to HCLK and PCLK1 to HCLK/2 - PCLK1 must not be greater than SYSCLK/2
#endif
#if !defined USE_HSI_CLOCK
    while (!(RCC_CR & RCC_CR_HSERDY)) {                                  // wait until the oscillator is ready
    #ifdef _WINDOWS
        RCC_CR |= RCC_CR_HSERDY;
    #endif
    }
#endif
#if defined DISABLE_PLL
    #if !defined USE_HSI_CLOCK
    RCC_CFGR |= RCC_CFGR_HSE_SELECT;                                     // set oscillator as direct source
    #endif
#else
    #if defined _STM32F2XX || defined _STM32F4XX
        #if SYSCLK > 144000000
    POWER_UP(APB1, (RCC_APB1ENR_PWREN));
    PWR_CR = PWR_CR_VOS;                                                 // enable high performance mode when the speed is greater than 144MHz
        #endif
        #if defined USE_HSI_CLOCK    
    RCC_PLLCFGR = ((PLL_Q_VALUE << 24) | (PLL_P_VALUE << 16) | PLL_INPUT_DIV | (_PLL_VCO_MUL << 6) | RCC_PLLCFGR_PLLSRC_HSI);
        #else
    RCC_PLLCFGR = ((PLL_Q_VALUE << 24) | (PLL_P_VALUE << 16) | PLL_INPUT_DIV | (_PLL_VCO_MUL << 6) | RCC_PLLCFGR_PLLSRC_HSE);
        #endif
    #else
        #ifdef USE_HSI_CLOCK    
    RCC_CFGR |= (((_PLL_VCO_MUL - 2) << 18));                            // set PLL multiplication factor from HSI input (divided by 2)
        #else
            #if defined _CONNECTIVITY_LINE && defined USE_PLL2_CLOCK
    // Generate an intermediate frequency on PLL2 and then use this as input to the main PLL
    //
    RCC_CFGR2 = (((PLL2_INPUT_DIV - 1) << 4) | ((_PLL2_VCO_MUL - 2) << 8) | RCC_CFGR2_PREDIV1SRC | (PLL_INPUT_DIV - 1));
    RCC_CR |= RCC_CR_PLL2ON;                                             // enable PLL2 and wait for it to become ready
    while (!(RCC_CR & RCC_CR_PLL2RDY)) {
                #ifdef _WINDOWS
        RCC_CR |= RCC_CR_PLL2RDY;
                #endif
    }
            #else
    RCC_CFGR2 = (PLL_INPUT_DIV - 1);                                     // set PLL input pre-divide
            #endif
    RCC_CFGR |= (((_PLL_VCO_MUL - 2) << 18) | RCC_CFGR_PLL_SRC);         // set PLL multiplication factor and select the pre-divide clock source
        #endif
    #endif
    RCC_CR |= RCC_CR_PLLON;                                              // enable PLL and wait for it to become ready
    while (!(RCC_CR & RCC_CR_PLLRDY)) {
    #ifdef _WINDOWS
        RCC_CR |= RCC_CR_PLLRDY;
    #endif
    }   
    RCC_CFGR |= RCC_CFGR_PLL_SELECT;                                     // select PLL as system clock source
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_PLL_USED) {        // wait until PLL used as system clock
    #ifdef _WINDOWS
        RCC_CFGR &= ~RCC_CFGR_SWS_MASK; RCC_CFGR |= RCC_CFGR_PLL_USED;
    #endif
    }
#endif
#ifdef _WINDOWS
    ptrVect = (VECTOR_TABLE *)((unsigned char *)((unsigned char *)&vector_ram));
#else
    ptrVect = (VECTOR_TABLE *)(RAM_START_ADDRESS);
#endif
    VECTOR_TABLE_OFFSET_REG   = (TBLBASE_IN_RAM);                        // position the vector table at the bottom of RAM
#if !defined _MINIMUM_IRQ_INITIALISATION
    ptrVect->ptrHardFault     = irq_hard_fault;
    ptrVect->ptrMemManagement = irq_memory_man;
    ptrVect->ptrBusFault      = irq_bus_fault;
    ptrVect->ptrUsageFault    = irq_usage_fault;
    ptrVect->ptrDebugMonitor  = irq_debug_monitor;
    ptrVect->ptrNMI           = irq_NMI;
    ptrVect->ptrPendSV        = irq_pend_sv;
    ptrVect->ptrSVCall        = irq_SVCall;
    processor_ints = (void (**)(void))&ptrVect->processor_interrupts;    // fill all processor specific interrupts with a default handler
    do {
        *processor_ints = irq_default;
        if (processor_ints == (void (**)(void))&ptrVect->processor_interrupts.LAST_PROCESSOR_IRQ) {
            break;
        }
        processor_ints++;
    } while (1);
#else
    ptrVect->ptrHardFault     = irq_default;
    ptrVect->ptrMemManagement = irq_default;
    ptrVect->ptrBusFault      = irq_default;
    ptrVect->ptrUsageFault    = irq_default;
    ptrVect->ptrDebugMonitor  = irq_default;
    ptrVect->ptrNMI           = irq_default;
    ptrVect->ptrPendSV        = irq_default;
    ptrVect->ptrSVCall        = irq_default;
#endif
#if defined DMA_MEMCPY_SET                                               // if uMemcpy()/uMemset() is to use DMA enble the DMA controller
    #if MEMCPY_CHANNEL > 7
    POWER_UP(AHB1, RCC_AHB1ENR_DMA2EN);
    #else
    POWER_UP(AHB1, RCC_AHB1ENR_DMA1EN);
    #endif
#endif
#if defined (_GNU)
    __init_gnu_data();
#endif
#ifdef _WINDOWS                                                          // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the size fo interrupt vectors required by the processor!!");
    }
#endif
#ifndef _WINDOWS
    uTaskerBoot();
#endif
    return 0;
}


#if defined ROWLEY && !defined ROWLEY_2                                  // Rowley project requires extra initialisation for debugger to work correctly before V1.7 build 17
static void _main2(void)
{
    asm("mov lr, #0");
    asm("b _main");
}
#endif



// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR
__root const RESET_VECTOR __vector_table @ ".intvec"                     // __root forces the function to be linked in IAR project
#elif defined _GNU
const RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
__attribute__((section("RESET"))) const RESET_VECTOR reset_vect
#else
const RESET_VECTOR reset_vect
#endif
= {
    (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - 4),                       // stack pointer to top of RAM (reserving one long word for random number)
    (void (*)(void))START_CODE
};





















#if defined _WINDOWS
// The following routines are only for simulator compatibility

extern void *fnGetHeapStart(void) { return 0; }

// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)               // {5}
{
    unsigned char ucValue = (unsigned char)iValue;
    void *buffer = ptrTo;
    unsigned char *ptr = (unsigned char *)ptrTo;

    while (Size-- != 0) {
        *ptr++ = ucValue;
    }

    return buffer;
}

// Convert a MAC address to a string
//
// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
    signed char cDummyMac[] = "--:--:--:--:--:--";
    int i = 0;
    while (cDummyMac[i] != 0) {
        *cStr++ = cDummyMac[i++];
    }
    *cStr = 0;
    return (cStr);
}

// Convert an IP address to a string
//
signed char *fnIPStr(unsigned char *ptrIP, signed char *cStr)
{
    signed char cDummyIP[] = "---.---.---.---";
    int i = 0;
    while (cDummyIP[i] != 0) {
        *cStr++ = cDummyIP[i++];
    }
    *cStr = 0;
    return (cStr);
}

extern unsigned char *fnGetTxBufferAdd(int iBufNr) { return 0;}

extern void DMA0_handler(void) {}
extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen) {return 0;}
TASK_LIMIT uTaskerStart(const UTASKTABLEINIT *ptATaskTable, const signed char *a_node_descriptions, const PHYSICAL_Q_LIMIT nr_physicalQueues) {return 0;}
void fnInitialiseHeap(const HEAP_NEEDS *ctOurHeap, void *start_heap ){}


// Hardware specific port reset states - if the hardware has pullups set '1' for each bit, else '0'
// (assumed all inputs have pullups)
#define QS_DEFAULT_INPUT           0xff
#define AN_DEFAULT_INPUT           0xff
#define LD_DEFAULT_INPUT           0xff
#define NQ_DEFAULT_INPUT           0xff
#define TA_DEFAULT_INPUT           0xff
#define TC_DEFAULT_INPUT           0xff
#define TD_DEFAULT_INPUT           0xff
#define UA_DEFAULT_INPUT           0xff
#define UB_DEFAULT_INPUT           0xff
#define UC_DEFAULT_INPUT           0xff
#define AS_DEFAULT_INPUT           0xff
#define GP_DEFAULT_INPUT           0xff
#define DD_DEFAULT_INPUT           0xff

INITHW void fnInitHW(void)
{
	extern unsigned long __VECTOR_RAM[];
    unsigned char ucPortPullups[] = {
        QS_DEFAULT_INPUT,                                                // set the port states out of reset in the project file app_hw_str91xf.h
        AN_DEFAULT_INPUT,
        LD_DEFAULT_INPUT,
        NQ_DEFAULT_INPUT,
        TA_DEFAULT_INPUT,
        TC_DEFAULT_INPUT,
        TD_DEFAULT_INPUT,
        UA_DEFAULT_INPUT,
        UB_DEFAULT_INPUT,
        UC_DEFAULT_INPUT,
        AS_DEFAULT_INPUT,
        GP_DEFAULT_INPUT,
        DD_DEFAULT_INPUT
    };

//  __VECTOR_RAM[PIT0_VECTOR] = (unsigned long)fnDummyTick;

    fnInitialiseDevice((void *)ucPortPullups);
    STM32_LowLevelInit();
}


extern void uTaskerBoot(void);
extern void uTaskerSchedule( void )
{
    static int iDone = 0;

    if (!iDone) {
        iDone = 1;
        uTaskerBoot();
    }
}
#endif
#endif

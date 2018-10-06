/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        AVR32_boot.c
    Project:     Single Chip Embedded Internet - boot loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2012
    *********************************************************************
    03.01.2010 Quieten VS warnings by removing strcpy()                  {1}
    15.06.2011 Add SPI Flash support                                     {2}
    08.01.2012 SPI Flash page calculation adapted for all power-of-2 devices {3}
    19.06.2011 Add _AT32UC3C support                                     {4}

*/

#ifdef _HW_AVR32

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#ifdef _WINDOWS
    #include "config.h"
    #include "../../WinSim/WinSim.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    extern void AVR32_LowLevelInit(void);
    #define _SIM_PORT_CHANGE   fnSimPorts();                             // make sure simulator knows of change
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define _SIM_PORT_CHANGE
    extern void  __set_evba(unsigned long ulEVBA);
    extern void  __set_count(unsigned long ulCountValue);
    extern void  __set_compare(unsigned long ulCompareValue);
    #include "config.h"

    #if defined (_GNU)
        extern void main(void);
    #endif
#endif

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_avr32_atmel.h"
            #include "spi_flash_avr32_stmicro.h"
            #include "spi_flash_avr32_sst25.h"
        #undef _SPI_DEFINES
    #endif
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)]; // FLASH row backup buffer (on word boundary)

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];
    #else
    static int SPI_FLASH_Danger[1] = {0};
    static unsigned char ucSPI_FLASH_Type[1];
    #endif

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
        #define _EXTENDED_CS  iChipSelect,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif




// This routine is used to command FLASH - we put it in SRAM
//
static void fnFlashFlash(unsigned long ulCommand)
{
    FC_CMD = ulCommand;                                                  // execute command
    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH ready again
    #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
    #endif
    }
    #ifdef FLASH_ERRATA_WORKAROUND                                       // it is expected that the two zeroed FLASH pages exist in high and low regions of FLASH
    ulCommand &= 0x000000ff;                                             // mask the command
    if ((ulCommand == FCMD_EP) || (ulCommand == FCMD_WP)) {              // perform workaround in these cases
        unsigned long *ptrFlash = (unsigned long *)FLASH_LOW_BLOCK_ZERO;
        volatile unsigned long ulDummy;
        do {
            ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from lower half of FLASH (page is zeroed)
        } while (ptrFlash < (unsigned long *)(FLASH_LOW_BLOCK_ZERO + FLASH_GRANULARITY));
        ptrFlash = (unsigned long *)FLASH_LOW_BLOCK_ZERO;                // repeat
        do {
            ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from lower half of FLASH (page is zeroed)
        } while (ptrFlash < (unsigned long *)(FLASH_LOW_BLOCK_ZERO + FLASH_GRANULARITY));
        ptrFlash = (unsigned long *)FLASH_HIGH_BLOCK_ZERO;               // same for high block
        do {
            ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from higher half of FLASH (page is zeroed)
        } while (ptrFlash < (unsigned long *)(FLASH_HIGH_BLOCK_ZERO + FLASH_GRANULARITY));
        ptrFlash = (unsigned long *)FLASH_HIGH_BLOCK_ZERO;               // repeat
        do {
            ulDummy = *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrFlash++); // dummy read from higher half of FLASH (page is zeroed)
        } while (ptrFlash < (unsigned long *)(FLASH_HIGH_BLOCK_ZERO + FLASH_GRANULARITY));            
    }
    #endif
}


// Cover routine for FLASH command execution
//
static void fnCommandFlash(unsigned long ulCommand)
{
#ifdef FLASH_ERRATA_WORKAROUND
    #define PROG_WORD_SIZE 150                                           // adequate space for the small program
#else
    #define PROG_WORD_SIZE 40                                            // adequate space for the small program
#endif

    static unsigned long ulProgSpace[PROG_WORD_SIZE];                    // make space for the routine (this will have a word boundary)
    static void (*fnRAM_code)(unsigned long) = 0;

    if (!fnRAM_code) {                                                   // real address of code
        uMemcpy(ulProgSpace, (void const *)fnFlashFlash, sizeof(ulProgSpace));
        fnRAM_code = (void(*)(unsigned long))(ulProgSpace);
    }

    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH is ready for programming
    #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
    #endif
    }

  //uDisable_Interrupt();                                                // protect this region from interrupts
    #ifdef _WINDOWS
    fnFlashFlash(ulCommand);
    #else
    fnRAM_code(ulCommand);                                               // execute code from SRAM
    #endif

  //uEnable_Interrupt();                                                 // safe to accept interrupts again
}


#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // SPI FLASH for SW upload

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_avr32_atmel.h"                               // check the chip(s) availablility
        #include "spi_flash_avr32_stmicro.h"
        #include "spi_flash_avr32_sst25.h"
    #undef _SPI_FLASH_INTERFACE

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
extern unsigned char fnSPI_FlashExt_available(int iExtension)
{
    if (iExtension > SPI_FLASH_DEVICE_COUNT) {
        return 0;
    }
    return ucSPI_FLASH_Type[iExtension];
}
    #endif

    #if defined SPI_FLASH_SST25
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination, int *iChipSelect)
        #else
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
        #endif
{
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    ucDestination -= (SPI_FLASH_START);                                  // location relative to the start of the SPI FLASH chip address
        #else
    ucDestination -= (uFILE_START + FILE_SYSTEM_SIZE);                   // location relative to the start of the SPI FLASH chip address
        #endif
        #ifdef SPI_FLASH_MULTIPLE_CHIPS                                  // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
        #endif
    return ucDestination;
}
    #endif                                                               // end SST25


    #if defined SPI_FLASH_SST25 && defined SPI_FLASH_MULTIPLE_CHIPS
// Calculate the device to access from the absolute address. It is assumed that all devices are operational.
//
static int fnGetSPI_Device(unsigned char **ptrFlash)                     // define the device to be accessed
{
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE)) {
        return 0;
    }
    #ifdef QSPI_CS2_LINE
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;
    }
        #ifdef QSPI_CS3_LINE
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
        return 2;
    }
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 3;
        #else
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 2;
        #endif
    #else
    *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
    return 1;
    #endif

}
    #endif

    #ifndef SPI_FLASH_SST25
// Return the page number and optionally the address offset in the page
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset, int *iChipSelect)
    #else
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
    #endif
{
    unsigned short usPageNumber;
    #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
    #else
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
    #endif
    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    #if (SPI_FLASH_PAGE_LENGTH != 256) && (SPI_FLASH_PAGE_LENGTH != 512) && (SPI_FLASH_PAGE_LENGTH != 1024) // {3} B-device or D-device set to 264 page mode
    usPageNumber = 0;
    while (ptrSector >= (unsigned char *)(SPI_FLASH_PAGE_LENGTH*100)) {  // large hops to get close to the page if at high FLASH address
        usPageNumber += 100;
        ptrSector -= (SPI_FLASH_PAGE_LENGTH*100);
    }
    while (ptrSector >= (unsigned char *)SPI_FLASH_PAGE_LENGTH) {        // page sizes to reach actual page
        usPageNumber++;
        ptrSector -= SPI_FLASH_PAGE_LENGTH;
    }
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector); // remainder is offset in the page
    }
    #else
    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    #endif
    return usPageNumber;
}
    #endif

    #endif


// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
    #ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
    #endif
    do {
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
        static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
        unsigned short ucDeleted = 0;
        unsigned short usTemp = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector);

        usTemp &= ~(EEPROM_PAGE_SIZE-1);                                 // we want to erase a block (program them to 0xff)
        Length += ((unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector) - usTemp);
        ptrSector = (unsigned char *)usTemp;
        while (ucDeleted < FLASH_GRANULARITY) {
            fnWaitWriteComplete();                                       // wait until free to write
        #ifdef SPI_INTERFACE
            fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrSector, ucDel, EEPROM_PAGE_SIZE);      // delete a page
        #else
            fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrSector, 0, EEPROM_PAGE_SIZE);          // delete a page
        #endif
            ucDeleted += EEPROM_PAGE_SIZE;
            ptrSector += EEPROM_PAGE_SIZE;
        }
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    #else                                                                // end SPI_FILE_SYSTEM
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)))
            #else
        if ((ptrSector >= (unsigned char *)SPI_FLASH_START) && (ptrSector < ((unsigned char *)(SPI_FLASH_START + SPI_DATA_FLASH_SIZE))))
            #endif
        {                                                                // we are working from external SPI FLASH memory
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            #if !defined SPI_FLASH_ST
            MAX_FILE_LENGTH BlockLength;
            unsigned char  ucCommand;
            #endif
            #ifdef SPI_FLASH_SST25
            unsigned long ulSectorAlign;
            unsigned long ulLocation = (CAST_POINTER_ARITHMETIC)fnGetSPI_FLASH_address(ptrSector EXTENDED_CS);   // convert to virtual SPI Flash memory location
            ulSectorAlign = (ulLocation & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1));
            Length += (ulLocation - ulSectorAlign);
            ptrSector -= (ulLocation - ulSectorAlign);
            #else
            unsigned short usPageNumber, usPageOffset;
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset EXTENDED_CS);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page
            #endif
            #if defined SPI_FLASH_ST
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // enable the write
                #ifdef SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #elif defined SPI_FLASH_SST25
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
                #ifndef SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && (ulSectorAlign == (ulLocation & (SPI_FLASH_SECTOR_LENGTH - 1)))) { // if a complete 64k sector can be deleted
                ucCommand = SECTOR_ERASE;                                // delete block of 64k
                BlockLength = SPI_FLASH_SECTOR_LENGTH;
            }
            else
                #endif
            if ((Length >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
                ucCommand = HALF_SECTOR_ERASE;                           // delete block of 32k
                BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
            }
            else {
                ucCommand = SUB_SECTOR_ERASE;                            // delete smallest sector of 4k
                BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
            }
            fnSPI_command(ucCommand, ulLocation, _EXTENDED_CS 0, 0);
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #else
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                BlockLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                BlockLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);   // delete appropriate page/block
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #endif
            continue;
        }
        #endif
        {                                                                // delete in internal FLASH
            unsigned long ulCommand = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // set to sector boundary
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)));
            ulCommand -= FLASH_START_ADDRESS;
            ulCommand /= FLASH_GRANULARITY;
            ulCommand <<= 8;                                             // page to be deleted
            ulCommand |= (FLASH_KEY | FCMD_EP);

            fnCommandFlash(ulCommand); // command individual page erase

            #ifdef _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion
            #endif

            if (FC_FSR & (LOCKE | PROGE)) {
                return 1;                                                // error
            }

            ptrSector += FLASH_GRANULARITY;
            if (Length <= FLASH_GRANULARITY) {
                break;
            }
            Length -= FLASH_GRANULARITY;
        }
    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);

    #ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
    #endif
    return 0;
}

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
extern int fnConfigSPIFileSystem(void)
{
    CONFIG_SPI_PORTS();
    POWER_SPI();                                                         // enable clocks to SPI in PMC

    SPI_MR = (SPI_MSTR | MODFDIS | SPI_CS_0);                            // master mode with no clock divider and control defined by CS0 configuration

    SPI_CSR0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 16000000/2)/16000000)<<8)); // maximum possible SPI speed 16MHz

    SPI_CR = SPIEN;                                                      // enable SPI

    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
    #include "spi_flash_avr32_atmel.h"                                   // check the chip availablility
    #include "spi_flash_avr32_stmicro.h"
    #include "spi_flash_avr32_sst25.h"
    #undef _CHECK_SPI_CHIPS

    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {
        return 1;
    }
    return 0;
}
#endif

// Retrieve file contents from FLASH
//
extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef SPI_SW_UPLOAD
    if (ParLocation >= ((unsigned char *)(SPI_FLASH_START))) {           // we are working from external SPI FLASH memory
        #ifdef SPI_FLASH_SST25
        ParLocation = fnGetSPI_FLASH_address(ParLocation EXTENDED_CS);
        #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);
        #endif
            #if defined SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #elif defined SPI_FLASH_SST25
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, _EXTENDED_CS ptrValue, Size);
            #else
                #if SPI_FLASH_PAGE_LENGTH >= 1024                        // {7}
                    #if SPI_FLASH_PAGE_LENGTH == 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #elif SPI_FLASH_PAGE_LENGTH >= 512
                    #if SPI_FLASH_PAGE_LENGTH == 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9)  | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #else
                    #if SPI_FLASH_PAGE_LENGTH == 256
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
                    #endif
                #endif
            #endif
        return;
    }
#endif
#ifdef _WINDOWS
    if (ParLocation < uFILE_SYSTEM_END) {
        ParLocation += (unsigned long)ucFLASH;
        ParLocation -= FLASH_START_ADDRESS;
    }
#endif
    uMemcpy(ptrValue, ParLocation, Size);                                // the AVR32 uses a file system in FLASH with no access restrictions so we can simply copy the data
}

// Commit the present write buffer to FLASH
//
static int fnCommitFLASH(unsigned long ulPage)
{
    fnCommandFlash(FLASH_KEY | FCMD_WP | (ulPage << 8));
    return (FC_FSR & (LOCKE | PROGE));                                   // zero when no error
}


#ifdef _WINDOWS
extern void uFlashMemcpy_long(unsigned long *ptrTo, const unsigned long *ptrFrom, size_t Size)
{
    Size /= sizeof(unsigned long);
    while (Size-- != 0) {
        *ptrTo &= *ptrFrom++;                                            // and the result with present FLASH content
        ptrTo++;
    }
}
#else
    #define uFlashMemcpy_long  uMemcpy_long
#endif


// Write a buffer to FLASH.
// The AVR32 does not support multiple writes to a single long word, so we collect data to be saved to a block of FLASH_ROW_SIZE bytes
// until a virtual block boundary is crossed or else a closing command is received. The larger the size of block (up to the flash granularity size)
// the less FLASH writes are performed but the larger the block buffer needs to be
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    #ifdef FLASH_FILE_SYSTEM
    static unsigned char *ptrOpenBuffer = 0;
    unsigned long ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1));
    unsigned char *ptrFlashBuffer;
    MAX_FILE_LENGTH BufferCopyLength;
    int iRtn = 0;
    #endif

    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ucDestination >= ((unsigned char *)(SPI_FLASH_START)))
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))
        #endif
    {                                                                    // we are working from external SPI FLASH memory
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        #if defined SPI_FLASH_SST25
        int iMultipleWrites = 0;
        #endif
        unsigned short usDataLength;
        #if defined SPI_FLASH_SST25
        if (Length == 0) {
            return 0;                                                    // ignore if length is zero
        }
        ucDestination = fnGetSPI_FLASH_address(ucDestination EXTENDED_CS); // convert to virtual SPI Flash memory location
        if ((CAST_POINTER_ARITHMETIC)ucDestination & 0x1) {              // start at odd SPI address, requires an initial byte write
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
            fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, 1); // program last byte
            if (--Length == 0) {                                         // single byte write so complete
                return 0;
            }
            ucDestination++;
            ucData++;
        }
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // command write enable to allow programming
        #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
        #endif
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
        #if defined SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #elif defined SPI_FLASH_SST25
            #ifdef SST25_A_VERSION
            #define AAI_LENGTH 1
            #else
            #define AAI_LENGTH 2
            #endif
            if (usDataLength > 1) {
                if (iMultipleWrites == 0) {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                    iMultipleWrites = 1;                                 // mark that we are in a AAI sequence
                }
                else {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
                }
                ucDestination += AAI_LENGTH;
                Length -= AAI_LENGTH;
                ucData += AAI_LENGTH;
            }
            else {
                if (iMultipleWrites != 0) {
                    fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);  // first close AAI sequence
                    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);   // command write enable to allow byte programming
                }
                fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, _EXTENDED_CS ucData, 1); // program last byte
                break;
            }
        #else
            if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) { // are we writing a partial page?
                fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
            }
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #endif
        }
        #if defined SPI_FLASH_SST25
        fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);              // disable writes
        #endif
        return 0;
    }
    #endif
    #ifdef FLASH_FILE_SYSTEM
    if (ucData == 0) {                                                   // close an open buffer
        ucDestination = ptrOpenBuffer;
        ulBufferOffset = FLASH_ROW_SIZE;
    }
    else {
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ucDestination & ~(FLASH_ROW_SIZE-1));
    }
    do {
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {
            BufferCopyLength = Length;
        }
        ptrFlashBuffer = (unsigned char *)ulFlashRow + ulBufferOffset;   // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= ((unsigned char *)ulFlashRow + FLASH_ROW_SIZE)) { // a complete backup buffer is ready to be copied to FLASH
            fnCommandFlash(FLASH_KEY | FCMD_CPB);                        // clear the Flash page buffer
            uFlashMemcpy_long((unsigned long *)fnGetFlashAdd(ptrOpenBuffer), ulFlashRow, FLASH_ROW_SIZE); // copy the data to the Flash page using long word accesses
            ptrFlashBuffer = (unsigned char *)ulFlashRow;                // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            iRtn |= fnCommitFLASH(((CAST_POINTER_ARITHMETIC)ptrOpenBuffer - FLASH_START_ADDRESS)/FLASH_GRANULARITY); // commit the page buffer to Flash
        #ifdef CHECK_FLASH_WRITE                                         // generally used only during debugging to verify that the written FLASH content equals the expected contents in FLASH
            if (uMemcmp(ulFlashRow, fnGetFlashAdd(ptrOpenBuffer), FLASH_ROW_SIZE) != 0) {
                iRtn = -1;                                               // mark that an error was identified
            }
        #endif
            ptrOpenBuffer += FLASH_ROW_SIZE;
            uMemset_long(ulFlashRow, 0xffffffff, FLASH_ROW_SIZE);        // flush the intermediate buffer
        }
    } while (Length != 0);
    #endif
    return iRtn;
}


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    WDT_CTRL = (WATCHDOG_KEY_1 | WATCHDOG_ENABLE | (12 << WATCHDOG_PRE_SHIFT));  // enable watchdog with two writes using the key sequence
    WDT_CTRL = (WATCHDOG_KEY_2 | WATCHDOG_ENABLE | (12 << WATCHDOG_PRE_SHIFT));  // watchdog will fire after about 70ms (this also gives the startup code enough time to disable teh watchdog after the reset, if needed)
#ifdef _WINDOWS
    WDT_CTRL = (WATCHDOG_KEY_2 | WATCHDOG_ENABLE);                       // ensure simulator recognises reset
#else
    while (1) {}                                                         // wait until watchdog fires
#endif
}

#if defined (_GNU)                                                       // GCC initialisation of variables
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
}
#endif

// Perform very low level AVR32 initialisation - called by the start up code
//
extern void AVR32_LowLevelInit(void)
{
    WDT_CTRL = (WATCHDOG_KEY_1);                                         // enable watchdog with two writes using the key sequence
    WDT_CTRL = (WATCHDOG_KEY_2);                                         // disable watchdog so that it can't disturb when loading new code
#if !defined RUN_FROM_RC_OSCILLATOR                                      // configure the oscillator
    #if defined _AT32UC3C                                                // {4}
    SCIF_UNLOCK = (SCIF_UNLOCK_KEY | SCIF_OSCCTRL0_SHORT_ADD);           // unlock the SCIF_OSCCTRL0 for following access
        #if CRYSTAL_FREQ < 900000
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_142M | SCIF_OSCCTRL_GAIN_0 | SCIF_OSCCTRL_OSCEN);
        #elif CRYSTAL_FREQ < 3000000
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_71M | SCIF_OSCCTRL_GAIN_1 | SCIF_OSCCTRL_OSCEN);
        #elif CRYSTAL_FREQ < 8000000
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_36M | SCIF_OSCCTRL_GAIN_2 | SCIF_OSCCTRL_OSCEN);
        #else
    SCIF_OSCCTRL0 = (SCIF_OSCCTRL_MODE_CRYSTAL | SCIF_OSCCTRL_STARTUP_18M | SCIF_OSCCTRL_GAIN_3 | SCIF_OSCCTRL_OSCEN);
        #endif
    while (!(SCIF_PCLKSR & SCIF_PCLKSR_OSC0RDY)) {                       // wait for oscillator to become ready
        #ifdef _WINDOWS
        SCIF_PCLKSR |= SCIF_PCLKSR_OSC0RDY;
        #endif
    }
        #ifdef _WINDOWS
    fnSimPortsAVR(PORTS_AVAILABLE);
        #endif
        #ifdef DISABLE_PLL
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_MCCTRL_SHORT_ADD);                   // unlock the PM_MCCTRL for following access
    PM_MCCTRL = MCSEL_OSC0;                                              // select oscillator 0 as main source
        #endif
    #else
        #if CRYSTAL_FREQ < 900000
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_400_900KHZ | OSC_STARTUP_TIME);
        #else
            #if CRYSTAL_FREQ < 3000000
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_900_3000KHZ | OSC_STARTUP_TIME);
            #else
                #if CRYSTAL_FREQ < 8000000
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_3_8MHZ | OSC_STARTUP_TIME);
                #else
    PM_OSCCTRL0 = (OSCILLATOR_RANGE_ABOVE_8MHZ | OSC_STARTUP_TIME);
                #endif
            #endif
        #endif
    PM_MCCTRL = OSC0EN;                                                  // enable the oscillator
    while (!(PM_POSCSR & OSC0RDY)) {                                     // wait until the oscillator has stabilised
        #ifdef _WINDOWS
        fnSimPortsAVR(-1);
        PM_POSCSR = OSC0RDY;
        #endif
    }
    #if defined DISABLE_PLL
    PM_MCCTRL = (OSC0EN | MCSEL_OSC0);                                   // switch to oscillator clock
        #endif
    #endif
#endif
#if !defined DISABLE_PLL && !defined RUN_FROM_RC_OSCILLATOR
    #if defined _AT32UC3C                                                // {4}
    SCIF_UNLOCK = (SCIF_UNLOCK_KEY | SCIF_PLL0_SHORT_ADD);               // unlock the SCIF_PLL0 for following access
    SCIF_PLL0 = (SCIF_PLL_PLLOSC_0 | SCIF_PLL_PLLEN | PLL_SETTING);      // configure PLL0    
    while (!(SCIF_PCLKSR & SCIF_PCLKSR_PLL0_LOCK)) {                     // wait for PLL to lock
        #ifdef _WINDOWS
        SCIF_PCLKSR |= SCIF_PCLKSR_PLL0_LOCK;
        #endif
    }
        #if MASTER_CLOCK > MAX_CLOCK_WITHOUT_FLASH_WAITSTATE
    FC_FCR = FWS_ONE_WAIT_STATE;                                         // set 1 wait state for FLASH accesses
        #endif
    // Configure the various bus clocks
    //
        #if CPU_HSB_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_CPUSEL_SHORT_ADD);                   // unlock the PM_CPUSEL for following access
    PM_CPUSEL = (PM_CLKSEL_CPUDIV | CPU_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if HSB_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_HSBSEL_SHORT_ADD);                   // unlock the PM_HSBSEL for following access
    PM_HSBSEL = (PM_CLKSEL_CPUDIV | HSB_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if PBB_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_PBBSEL_SHORT_ADD);                   // unlock the PM_PBBSEL for following access
    PM_PBBSEL = (PM_CLKSEL_CPUDIV | PBB_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if PBA_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_PBASEL_SHORT_ADD);                   // unlock the PM_PBASEL for following access
    PM_PBASEL = (PM_CLKSEL_CPUDIV | PBA_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
        #if PBC_DIVIDE > 1
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_PBCSEL_SHORT_ADD);                   // unlock the PM_PBCSEL for following access
    PM_PBCSEL = (PM_CLKSEL_CPUDIV | PBC_DIV);
    while (!(PM_SR & PM_SR_CKRDY)) {
            #ifdef _WINDOWS
        PM_SR |= PM_SR_CKRDY;
            #endif
    }
        #endif
    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH controller is ready to receive the following command
        #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
        #endif
    }
        #if MASTER_CLOCK > MIN_CLOCK_WITH_HIGHSPEED_READ_MODE
    FC_CMD = FCMD_HSEN;                                                  // execute high speed enable command
        #else
    FC_CMD = FCMD_HSDIS;                                                 // execute high speed disable command    
        #endif
    while (!(FC_FSR & FLASH_READY)) {                                    // wait until FLASH ready again
        #ifdef _WINDOWS
        FC_FSR |= FLASH_READY;
        #endif
    }
    PM_UNLOCK = (PM_UNLOCK_KEY | PM_MCCTRL_SHORT_ADD);                   // unlock the PM_MCCTRL for following access
    PM_MCCTRL = MCSEL_PLL0;                                              // select PLL 0 as main source
    #else
    PM_PLL0 = (PLLEN | PLLOSC_SOURCE_0 | PLL_SETTING);                   // configure and enable PLL 0
    while (!(PM_POSCSR & LOCK0)) {                                       // wait for PLL to lock
        #ifdef _WINDOWS
        PM_POSCSR |= LOCK0;
        #endif
    }
        #if MASTER_CLOCK > 33000000                                      // add one wait state to FLASH accesses when the CPU is operating from 33MHz
    FC_FCR = FWS_ONE_WAIT_STATE;
        #endif
    PM_CKSEL = (BUS_DIVIDES);                                            // set CPU and the internal bus speeds (this write clears the CKRDY flag)
    while (!(PM_POSCSR & CKRDY)) {
        #ifdef _WINDOWS
        PM_POSCSR |= CKRDY;
        #endif
    }
    PM_MCCTRL |= MCSEL_PLL0;                                             // switch to PLL output for CPU clock
    #endif
#else
    #if BUS_DIVIDES != 0
    PM_CKSEL = (BUS_DIVIDES);                                            // set CPU and the internal bus speeds (this write clears the CKRDY flag)
    while (!(PM_POSCSR & CKRDY)) {
        #ifdef _WINDOWS
        PM_POSCSR |= CKRDY;
        #endif
    }
    #endif
#endif
#if defined _AT32UC3C                                                    // {4}
    POWER_DOWN(PBA, (PBAMASK_FCIF | PBAMASK_AST | PBAMASK_WDT | PBAMASK_EIC | PBAMASK_FREQM  | PBAMASK_USART0 | PBAMASK_USART2 | PBAMASK_USART3 | PBAMASK_SPI1 | PBAMASK_TWIM0 | PBAMASK_TWIM1 | PBAMASK_TWIS0 | PBAMASK_TWIS1 | PBAMASK_IISC | PBAMASK_PWM | PBAMASK_QDEC0 | PBAMASK_QDEC1 | PBAMASK_TC1 | PBAMASK_PEVC | PBAMASK_ACIFA0 | PBAMASK_ACIFA1 | PBAMASK_DACIFB0 | PBAMASK_DACIFB1 | PBAMASK_AW)); // disable all peripheral A clocks by default, except from the interrupt controller, GPIO and power management
    POWER_DOWN(PBB, (PBBMASK_USBC | PBBMASK_SAU | PBBMASK_SMC | PBBMASK_SDRAMC | PBBMASK_MACB));// disable all peripheral B clocks by default, apart from the FLASH controller and HSB matrix
    POWER_DOWN(PBC, (PBCMASK_PDCA | PBCMASK_MDMA | PBCMASK_USART1 | PBCMASK_SPI0 | PBCMASK_CANIF | PBCMASK_TC0 | PBCMASK_ADCIFA | PBCMASK_USART4 | PBCMASK_TWIM2 | PBCMASK_TWIS2));// disable all peripheral C clocks by default
    POWER_DOWN(HSB, (HSBMASK_SAU | HSBMASK_PDCA | HSBMASK_MDMA | HSBMASK_USBC | HSBMASK_CANIF | HSBMASK_EBI | HSBMASK_MACB | HSBMASK_PEVC)); // disable all HSB clocks apart from to the FLASH controller, RAM and bridges
#else
    POWER_DOWN(PBA, (PBAMASK_PDCA | PBAMASK_ADC  | PBAMASK_SPI0 | PBAMASK_SPI1 | PBAMASK_TWI | PBAMASK_USART0 | PBAMASK_USART1 | PBAMASK_USART2 | PBAMASK_USART3 | PBAMASK_PWM | PBAMASK_SSC | PBAMASK_TC | PBAMASK_ABDAC)); // disable all peripheral A clocks by default, except from the interrupt controller, GPIO and power management
    POWER_DOWN(PBB, (PBBMASK_USBB | PBBMASK_MACB | PBBMASK_SMC  | PBBMASK_SDRAMC));// disable all peripheral B clocks by default, apart from the FLASH controller and HSB matrix
    POWER_DOWN(HSB, (HSBMASK_USBB | HSBMASK_MACB | HSBMASK_PDCA | HSBMASK_EBI)); // disable all HSB clocks apart from to the FLASH controller and bridges
#endif
#if defined (_GNU)
    __init_gnu_data();                                                   // initialise variables in SRAM
    main();                                                              // main and never return
#endif
}


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


extern void uMemset_long(unsigned long *ptrTo, unsigned long ulValue, size_t Size)
{
    Size /= sizeof(unsigned long);
    while (Size-- != 0) {
        *ptrTo++ = ulValue;
    }
}

extern void uMemcpy_long(unsigned long *ptrTo, const unsigned long *ptrFrom, size_t Size)
{
    Size /= sizeof(unsigned long);
    while (Size-- != 0) {
        *ptrTo++ = *ptrFrom++;
    }
}


// memset() implementation
//
extern void *uMemset(void *ptrTo, unsigned char ucValue, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr = (unsigned char *)ptrTo;

    while (Size--) {
        *ptr++ = ucValue;
    }

    return buffer;
}

// memcpy() implementation
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


#ifdef _WINDOWS
// The following routines are only for simulator compatibility

extern void *fnGetHeapStart(void) { return 0; }


// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
    signed char cDummyMac[] = "--:--:--:--:--:--";
    int i = 0;
    while (cDummyMac[i] != 0) {                                          // {1}
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
    while (cDummyIP[i] != 0) {                                           // {1}
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

static void fnDummyTick(void)
{
}


// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               //perform hardware initialisation
{
#ifdef _WINDOWS
    #define PORT0_DEFAULT_INPUT        0xffffffff
    #define PORT1_DEFAULT_INPUT        0xffffffff
    #define PORT2_DEFAULT_INPUT        0xffffffff
    #define PORT3_DEFAULT_INPUT        0xffffffff

    unsigned long ulPortPullups[] = {
        PORT0_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_avr32.h
        PORT1_DEFAULT_INPUT,
        PORT2_DEFAULT_INPUT,
        PORT3_DEFAULT_INPUT,
    };

    fnInitialiseDevice((void *)ulPortPullups);
    AVR32_LowLevelInit();
#endif
#ifdef _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif
#ifndef SPI_INTERFACE
    #ifdef SPI_FILE_SYSTEM
    fnConfigSPIFileSystem();                                             // configure SPI interface for SPI file system and 2M speed
    #endif
#endif
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

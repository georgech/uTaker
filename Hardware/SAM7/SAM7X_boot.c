/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        SAM7X_boot.c
    Project:     Single Chip Embedded Internet - boot loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************

    16.12.2007 Add SPI SW upload support                                 {1}
    18.03.2008 Remove chip select parameter from fnSimAT45DBXXX()
    10.08.2009 Quieten VS warnings by removing strcpy()                  {2}
    12.12.2009 Add variable initialisation for GNU                       {3}
    30.03.2010 Support FLASH controller 1 in devices with more that 256k FLASH {4}
    27.06.2010 Correct PLL configuration to respect new known restrictions {5}
    21.07.2013 Integrate multiple SPI drivers                            {6}

*/

#ifdef _HW_SAM7X

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#ifdef _WINDOWS
  #include "config.h"

  #include "../../WinSim\WinSim.h"
  #define INITHW  extern
  extern void fnOpenDefaultHostAdapter(void);
  #define __interrupt__
  #define SIM_LOCK MCF_CLOCK_SYNSR |= PLLOCKED;
  #define _LITTLE_ENDIAN
  #define SIM_DMA_3 fnSimDMA(0);
  #define BACKDOOR_FLASH 0

  #define _SIM_PORT_CHANGE   fnSimPorts();                               // make sure simulator knows of change

  NETWORK_PARAMETERS network[1];
#else
  #define OPSYS_CONFIG                                                   // this module owns the operating system configuration
  #define INITHW  static
  #define SIM_LOCK
  #define SIM_DMA_3
  #define _SIM_PORT_CHANGE
  #define BACKDOOR_FLASH (CFM_FLASH)
  #ifdef _GNU
    #define __interrupt__
  #endif
  #include "config.h"
#endif


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    RSTC_CR = (PROCRST | PERRST | RESET_KEY);                            // command reset to core and peripherals
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

#ifdef SPI_SW_UPLOAD
    #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25                // {6}
        #define SPI_FLASH_ATMEL                                          // default if not otherwise defined
    #endif
    #define _EXTENDED_CS  
    static unsigned char ucSPI_FLASH_Type[1];
    static int SPI_FLASH_Danger[1] = {0};
    #define _SPI_DEFINES
        #include "spi_flash_sam7x_atmel.h"
        #include "spi_flash_sam7x_stmicro.h"
        #include "spi_flash_sam7x_sst25.h"
    #undef _SPI_DEFINES

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code {6}
        #include "spi_flash_sam7x_atmel.h"
        #include "spi_flash_sam7x_stmicro.h"
        #include "spi_flash_sam7x_sst25.h"
    #undef _SPI_FLASH_INTERFACE


extern int fnConfigSPIFileSystem(void)                                   // {1}
{
    CONFIG_SPI_PORTS();                                                  // {6}
    POWER_SPI();                                                         // enable clocks to SPI in PMC
    SPI_MR = (SPI_MSTR | MODFDIS | SPI_CS_0);                            // master mode with no clock divider and control defined by CS0 configuration
	SPI_CSR0 = (SPI_CPOL | SPI_8_BITS | (((MASTER_CLOCK + 16000000/2)/16000000) << 8)); // 16MHz
    SPI_CR = SPIEN;                                                      // enable SPI

    #define _CHECK_SPI_CHIPS                                             // {6} insert manufacturer dependent code
        #include "spi_flash_sam7x_atmel.h"
        #include "spi_flash_sam7x_stmicro.h"
        #include "spi_flash_sam7x_sst25.h"
    #undef _CHECK_SPI_CHIPS
    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {
        return 1;                                                        // no SPI flash detected
    }
    return 0;                                                            // SPI flash detected
}

    #if !defined SPI_FLASH_SST25
// Return the page number and optionally the address offset in the page
//
static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
{
    unsigned short usPageNumber;
    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address

        #if SPI_FLASH_PAGE_LENGTH != 256                                     // B-device or D-device set to 264 page mode
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

#ifdef _WINDOWS
  #define PROG_SIZE  100
#else
  #define PROG_SIZE  80                                                  // optimised from experience
#endif

#if defined SPI_FLASH_SST25                                              // {6}
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
{
    ucDestination -= (uFILE_START + FILE_SYSTEM_SIZE);                   // location relative to the start of the SPI FLASH chip address
    return ucDestination;
}
#endif                                                                   // end SST25
// Retrieve file contents from FLASH
//
extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef SPI_SW_UPLOAD                                                     // {1}
    if (ParLocation >= uFILE_SYSTEM_END) {                               // get from SPI FLASH
    #ifdef SPI_FLASH_SST25                                               // {6}
        ParLocation = fnGetSPI_FLASH_address(ParLocation);
    #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset);
    #endif
    #if defined SPI_FLASH_ST                                             // {6}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
    #elif defined SPI_FLASH_SST25                                        // {6}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, _EXTENDED_CS ptrValue, Size);
    #else
        #if SPI_FLASH_PAGE_LENGTH >= 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), ptrValue, Size);
        #elif SPI_FLASH_PAGE_LENGTH >= 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), ptrValue, Size);
        #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), ptrValue, Size);
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
    uMemcpy(ptrValue, ParLocation, Size);                                // the SAM7x uses a file system in FLASH with no access restrictions so we can simply copy the data
}


// This routine to command FLASH - we put it in SRAM
//
static void fnFlashFlash(unsigned long ulCommand)
{
    MC_FCR = ulCommand;                                                  // execute command
    while (!(MC_FSR & FLASH_READY)) {}                                   // wait until FLASH ready again
}

// Warning: this is designed for little endian Thumb mode ONLY
// Cover routine for FLASH command execution
//
static void fnCommandFlash(unsigned long ulCommand)
{
    #define PROG_WORD_SIZE 100                                           // adequate space for the small program

    static unsigned short usProgSpace[PROG_WORD_SIZE];                   // make space for the routine on stack (this will have a word boundary)
    static void (*fnRAM_code)(unsigned long) = 0;

    if (!fnRAM_code) {                                                   // real address of code in thumb mode
        unsigned char *ptrThumb = (unsigned char *)fnFlashFlash;
        int i = 0;

        ptrThumb--;                                                      // real address of thumb routine

        while (i < PROG_WORD_SIZE) {                                     // copy program to SRAM
            usProgSpace[i++] = *(unsigned short*)ptrThumb;
            ptrThumb += 2;
        }
        ptrThumb = (unsigned char *)usProgSpace;
        ptrThumb++;                                                      // create a thumb call
        fnRAM_code = (void(*)(unsigned long))(ptrThumb);
    }

    while (!(MC_FSR & FLASH_READY)) {}                                   // wait until FLASH ready for programming

    uDisable_Interrupt();                                                // protect this region from interrupts

    fnRAM_code(ulCommand);                                               // execute code from SRAM

    uEnable_Interrupt();                                                 // safe to accept interrupts again
}

#if SIZE_OF_FLASH > (256 * 1024)                                         // {4}
// This routine is used to command FLASH - we put it in SRAM
//
static void fnFlashFlash1(unsigned long ulCommand)
{
    MC_FCR1 = ulCommand;                                                 // execute command
    while (!(MC_FSR1 & FLASH_READY)) {}                                  // wait until FLASH ready again
}

// Warning: this is designed for little endian Thumb mode ONLY
// Cover routine for FLASH command execution
//
static void fnCommandFlash1(unsigned long ulCommand)
{
    #define PROG_WORD_SIZE 100                                           // adequate space for the small program

    static unsigned short usProgSpace1[PROG_WORD_SIZE];                  // make space for the routine on stack (this will have a word boundary)
    static void (*fnRAM_code1)(unsigned long) = 0;

    if (!fnRAM_code1) {                                                  // real address of code in thumb mode
        unsigned char *ptrThumb1 = (unsigned char *)fnFlashFlash1;
        int i = 0;

        ptrThumb1--;                                                     // real address of thumb routine

        while (i < PROG_WORD_SIZE) {                                     // copy program to SRAM
            usProgSpace1[i++] = *(unsigned short*)ptrThumb1;
            ptrThumb1 += sizeof(unsigned short);
        }
        ptrThumb1 = (unsigned char *)usProgSpace1;
        ptrThumb1++;                                                     // create a thumb call
        fnRAM_code1 = (void(*)(unsigned long))(ptrThumb1);
    }

    while (!(MC_FSR1 & FLASH_READY)) {}                                  // wait until FLASH ready for programming

    uDisable_Interrupt();                                                // protect this region from interrupts

    fnRAM_code1(ulCommand);                                              // execute code from SRAM

    uEnable_Interrupt();                                                 // safe to accept interrupts again
}
#endif


// Commit the present write buffer to FLASH
//
static int fnCommitFLASH(unsigned long ulPage)
{
#if SIZE_OF_FLASH > (256 * 1024)                                         // {4}
    if (ulPage >= ((256 * 1024)/FLASH_GRANULARITY)) {                    // check whether in the second FLASH block
    #ifdef _WINDOWS
        MC_FSR1 |= FLASH_READY;
    #endif
        ulPage -= ((256 * 1024)/FLASH_GRANULARITY);                      // page reference for the second FLASH controller
        fnCommandFlash1(FLASH_KEY | FCMD_WP | (ulPage << 8));
        if (MC_FSR1 & (LOCKE | PROGE)) {
            return 1;                                                    // FLASH error - quit
        }
        return 0;
    }
#endif
#ifdef _WINDOWS
    MC_FSR |= FLASH_READY;
#endif
    fnCommandFlash(FLASH_KEY | FCMD_WP | (ulPage++<<8));

    if (MC_FSR & (LOCKE | PROGE)) {
        return 1;                                                        // FLASH error - quit
    }
    return 0;
}

// Write a buffer to FLASH
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
#ifdef SPI_FILE_SYSTEM                                                   // simulate the same using SPI EEPROM
    static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
    unsigned short usPageLength;
    while (Length) {
        usPageLength = ((unsigned short)ucDestination) & (EEPROM_PAGE_SIZE-1); // calculate the page length based on EEPROM page boundaries
        usPageLength = EEPROM_PAGE_SIZE - usPageLength;
        if (Length < usPageLength) {
            usPageLength = (unsigned short)Length;
        }
        fnWaitWriteComplete();
  #ifdef SPI_INTERFACE
        fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);           // write a page
  #else
        fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);           // write a page
  #endif
        Length -= usPageLength;
        ucData += usPageLength;
        ucDestination += usPageLength;
    }
    return 0;
#else
    // We have already check that it is possible to modify the byte requested but we have to be sure that we only write 32 bits to long word addresses
    //
    #ifdef _WINDOWS
    unsigned long *ptrLong = (unsigned long*)((unsigned long)((unsigned long)ucDestination + (unsigned long)ucFLASH - FLASH_START_ADDRESS) & ~0x3);  // long address starting on
    #else
    unsigned long *ptrLong = (unsigned long*)((unsigned long)ucDestination & ~0x3);  // long address starting on
    #endif
    unsigned long ulValue = *ptrLong;                                    // present value at location
    #ifdef _WINDOWS
    int iByteInLong = (ucDestination - FLASH_START_ADDRESS + (unsigned long)ucFLASH) - (unsigned char *)ptrLong;
    #else
    int iByteInLong = ucDestination - (unsigned char *)ptrLong;
    #endif
    unsigned long ulPage = (unsigned long)(ucDestination - FLASH_START_ADDRESS)/FLASH_GRANULARITY;
    unsigned long *ulPageEnd = (unsigned long *)((unsigned long)ucDestination & ~(FLASH_GRANULARITY-1)) + (FLASH_GRANULARITY/ sizeof(unsigned long));

    #ifdef _WINDOWS
    ulPageEnd += ((unsigned long)ucFLASH - FLASH_START_ADDRESS)/sizeof(unsigned long);
    #endif

    while (Length--) {
    #if defined (_LITTLE_ENDIAN) || defined (_WINDOWS)
        ulValue &= ~(0xff << (iByteInLong*8));                           // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong*8));                       // insert new byte
    #else
        ulValue &= ~(0xff << ((3 - iByteInLong)*8));                     // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong)*8));                 // insert new byte
    #endif
        if (iByteInLong == 3) {                                          // can we write a long?
            *ptrLong++ = ulValue;
            if (ptrLong >= ulPageEnd) {
                if (fnCommitFLASH(ulPage++) != 0) {                      // we must commit the write buffer to flash
                    return 1;
                }

                if (!Length) {
                    return 0;                                            // completed
                }

                ulPageEnd += FLASH_GRANULARITY/sizeof(unsigned long);    // set next sector for comparison
            }
            ulValue = *ptrLong;
            iByteInLong = 0;
        }
        else {
            iByteInLong++;
        }
    }
    *ptrLong++ = ulValue;
    return (fnCommitFLASH(ulPage));                                      // commit to FLASH after copying to write buffer
#endif
}


// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length) // {1}
{
    do {
#if defined SPI_SW_UPLOAD
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) { // we are working from external SPI FLASH memory
    #ifdef SPI_FLASH_SST25                                               // {6}
            MAX_FILE_LENGTH BlockLength;
            unsigned long ulSectorAlign;
            unsigned long ulLocation = (CAST_POINTER_ARITHMETIC)fnGetSPI_FLASH_address(ptrSector); // convert to virtual SPI Flash memory location
            unsigned char ucCommand;
            ulSectorAlign = (ulLocation & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1));
            Length += (ulLocation - ulSectorAlign);
            ptrSector -= (ulLocation - ulSectorAlign);
    #else
        #if !defined SPI_FLASH_ST                                        // {6}
            unsigned short usLength;
            unsigned char ucCommand;
        #endif
            unsigned short usPageNumber, usPageOffset;
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page
    #endif
    #if defined SPI_FLASH_ST                                             // {6}
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // enable the write
        #ifdef SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
        #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
        #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
    #elif defined SPI_FLASH_SST25                                        // {6}
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // command write enable to allow byte programming
        #ifndef SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && (ulSectorAlign == (ulLocation & (SPI_FLASH_SECTOR_LENGTH - 1)))) { // {53} if a complete 64k sector can be deleted
                ucCommand = SECTOR_ERASE;                                // delete block of 64k
                BlockLength = SPI_FLASH_SECTOR_LENGTH;
            }
            else
        #endif
            if ((Length >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) { // {53}
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
    #else                                                                // AT45DBxxx
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                usLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                usLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, 0, 0);                // delete appropriate page/block
            if (Length <= usLength) {
                break;                                                   // delete complete
            }
            ptrSector += usLength;
            Length -= usLength;
    #endif
            continue;
        }
#endif
                                                                         // delete in internal FLASH
        Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)));
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // set to sector boundary

        #ifndef _WINDOWS
        uMemset(ptrSector, 0xff, FLASH_GRANULARITY);                     // ensure page buffer is empty before erasing
        #endif

        #ifdef _WINDOWS
        MC_FSR |= FLASH_READY;
            #if SIZE_OF_FLASH > (256 * 1024)                             // {4}
        MC_FSR1 |= FLASH_READY;
            #endif
        #endif

        #if SIZE_OF_FLASH > (256 * 1024)                                 // {4}
        if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (256 * 1024))) {
            MC_FMR1 &= ~NEBP;                                            // enable delete before write
            fnCommandFlash1(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - (FLASH_START_ADDRESS + (256 * 1024))));
            MC_FMR1 |= NEBP;                                             // generally we want to be in this mode so that we can modify the contents of a page

            #ifdef _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion
            #endif

            if (MC_FSR1 & (LOCKE | PROGE)) {
                return 1;                                                // error
            }
        }
        else {
            MC_FMR &= ~NEBP;                                             // enable delete before write
            fnCommandFlash(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS));
            MC_FMR |= NEBP;                                              // generally we want to be in this mode so that we can modify the contents of a page

            #ifdef _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd(ptrSector));               // simulate deletion
            #endif

            if (MC_FSR & (LOCKE | PROGE)) {
                return 1;                                                // error
            }
        }
        #else
        MC_FMR &= ~NEBP;                                                 // enable delete before write
        fnCommandFlash(FLASH_KEY | FCMD_WP | (unsigned long)((CAST_POINTER_ARITHMETIC)ptrSector - FLASH_START_ADDRESS));
        MC_FMR |= NEBP;                                                  // generally we want to be in this mode so that we can modify the contents of a page

            #ifdef _WINDOWS
        fnDeleteFlashSector(fnGetFlashAdd(ptrSector));                   // simulate deletion
            #endif

        if (MC_FSR & (LOCKE | PROGE)) {
            return 1;                                                    // error
        }
        #endif

        ptrSector += FLASH_GRANULARITY;
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    } while (Length);

#ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
#endif
    return 0;
}



// The following functions must be writen in ARM mode since they called directly
// by exception vector
static void AT91F_Spurious_handler(void)
{
}

static void AT91F_Default_IRQ_handler(void)
{
}

static void AT91F_Default_FIQ_handler(void)
{
}

#ifndef COMPILE_IAR
// Routine to disable interrupts during critical region (IAR uses intrinsic function)
//
int iDisables = 0;

void uDisable_Interrupt()
{
    iDisables++;
//  asm("sei"); Example
}

// Routine to re-enable interrupts on leaving a critical region (IAR uses intrinsic function)
//
void uEnable_Interrupt()
{
    iDisables--;
//  asm("cli"); Example
}
#endif

#if defined (_GNU)                                                       // {3}
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
    ulInitDataLength = (&__text_end__ - &__text_start__);
    while (ulInitDataLength--) {                                         // initialise text
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif


// Perform very low level SAM7X initialisation - called by the start up code
//
#ifdef _WINDOWS
 void AT91F_LowLevelInit( void)
#else
    #if defined _GNU || defined _COMPILE_KEIL
  void AT91F_LowLevelInit( void)
    #else
  void AT91F_LowLevelInit( void) @ "ICODE"
    #endif
#endif
{
    extern void __segment_init(void);
    unsigned long *ptrInt;
    int i;
#if MASTER_CLOCK > 30000000
    MC_FMR = (ONE_WAIT_STATE | FLASH_TIME_NORMAL | NEBP);                // set one wait in FLASH since we will be working faster than 30MHz
    #if SIZE_OF_FLASH > (256 * 1024)                                     // {4}
    MC_FMR1 = (ONE_WAIT_STATE | FLASH_TIME_NORMAL | NEBP);               // configure in second FLASH controller in larger parts
    #endif
#else
    MC_FMR = (FLASH_TIME_NORMAL | NEBP);                                 // no wait states in FLASH when not working higher than 30MHz
    #if SIZE_OF_FLASH > (256 * 1024)                                     // {4}
    MC_FMR1 = (FLASH_TIME_NORMAL | NEBP);                                // configure in second FLASH controller in larger parts
    #endif
#endif
    RSTC_MR = (RESET_KEY | URSTEN);                                      // reset input causes a user reset

    if (!(PMC_SR & PLL_LOCK)) {                                          // it may be that the debugger has programmed the PLL so we don't change it
                                                                         // enable the main oscillator in the power management controller
      //CKGR_MOR = ((6<<8) | MOSCEN);                                    // 6 x 8 slow clocks
        CKGR_MOR = ((64 << 8) | MOSCEN); /* test increasing from 6 to 64 for some 512k parts */

#ifdef _WINDOWS
        PMC_SR |= (MOSCS | PLL_LOCK);
#endif

        while (!(PMC_SR & MOSCS)) {}                                     // wait for start up time to pass before continuing
                                                                         // set the PLL to generate required PLL frequency
#ifdef USE_PLL_B
        CKGR_PLLR = (OUT_B | OSC_DIVIDE | ((PLL_MUL - 1) << 16) | (63 << 8)); // allow 63 slow clocks to clock
#else
        CKGR_PLLR = (OSC_DIVIDE | ((PLL_MUL - 1) << 16) | (63 << 8));    // allow 63 slow clocks to clock
#endif
        while (!(PMC_SR & PLL_LOCK)) {}                                  // {5} wait for PLL lock before continuing
        PMC_MCKR = CLK_PRES;                                             // first program the prescale and wait for ready
        while (!(PMC_SR & MCKRDY)) {}                                    // wait for the master clock status to indicate ready
        PMC_MCKR = (SELECT_PLL | CLK_PRES);                              // finally set the master clock to half the PLL value
        while (!(PMC_SR & MCKRDY)) {}                                    // wait for the master clock status to indicate ready




/*

        while (!(PMC_SR & MOSCS)) {}                                     // wait for start up time to pass before continuing
                                                                         // set the PLL to generate 95MHz clock (18 / 5 x 26
        CKGR_PLLR = (DIVIDE_5 | MUL_26 | (28 << 8));                     // allow 28 slow clocks to clock

        while (!(PMC_SR & PLL_LOCK)) {}                                  // wait for PLL lock before continuing

        PMC_MCKR = (SELECT_PLL | CLK_PRES_2);                            // finally set the master clock to half the PLL value*/
    }

    ptrInt = ADD_AIC_SVR0;                                               // set up the default interrupts handler vectors
    *ptrInt++ = (unsigned long)AT91F_Default_FIQ_handler;                // enter fast interrupt
    for (i = 0; i < 31; i++) {
        *ptrInt++ = (unsigned long)AT91F_Default_IRQ_handler;            // set default handler to alll locations
    }
    AIC_SPU = (unsigned long)AT91F_Spurious_handler;                     // enter spurious interrupt hanlder
#ifndef _BOOT_PROGRAM
    #ifndef _WINDOWS
        #if defined COMPILE_IAR && !defined COMPILE_IAR5
        __segment_init();                                                // initialise variables
        #elif defined (_GNU)                                             // {3}
        __init_gnu_data();
        #endif
    #endif
#endif
}




// memset implementation
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


#ifdef _WINDOWS
// The following routines are only for simulator compatibility

extern void *fnGetHeapStart(void) { return 0; }


// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
    signed char cDummyMac[] = "--:--:--:--:--:--";
    int i = 0;
    while (cDummyMac[i] != 0) {                                          // {2}
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
    while (cDummyIP[i] != 0) {                                           // {2}
        *cStr++ = cDummyIP[i++];
    }
    *cStr = 0;
    return (cStr);
}

extern unsigned char *fnGetTxBufferAdd(int iBufNr) { return 0;}
__interrupt__ void sw_wdog_timeout(void){}
__interrupt__ void ephy_isr(void){}
__interrupt__ void fec_rx_frame_isr(void){}
__interrupt__ void QSPI_Interrupt(void){}
__interrupt__ void irq1_handler(void){}
__interrupt__ void irq2_handler(void){}
__interrupt__ void irq3_handler(void){}
__interrupt__ void irq4_handler(void){}
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
    #define PORTA_DEFAULT_INPUT        0xffffffff
    #define PORTB_DEFAULT_INPUT        0xffffffff

    unsigned long ulPortPullups[] = {
        PORTA_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_sam7x.h
        PORTB_DEFAULT_INPUT
    };

    fnInitialiseDevice((void *)ulPortPullups);
    AT91F_LowLevelInit();
#endif

#ifdef _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif

#ifndef SPI_INTERFACE
    #ifdef SPI_FILE_SYSTEM
    fnConfigSPIFileSystem(SPI_2MEG);                                     // configure SPI interface for SPI file system and 2M speed
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

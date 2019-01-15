/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC17XX_boot.c
    Project:     Single Chip Embedded Internet - boot loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2011
    *********************************************************************

    16.11.2011 Add start_application()                                   {1}
    19.11.2011 Force GCC to link the reset table                         {2}
    19.11.2011 Add asm define for crossworks 2                           {3}

*/

#ifdef _LPC17XX

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#ifdef _WINDOWS
    #include "config.h"

    #include "..\..\WinSim\WinSim.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    #define __interrupt__
    #define SIM_LOCK MCF_CLOCK_SYNSR |= PLLOCKED;
    #define _LITTLE_ENDIAN
    #define SIM_DMA_3 fnSimDMA(0);
    #define START_CODE 0

    NETWORK_PARAMETERS network;

    extern unsigned char vector_ram[sizeof(VECTOR_TABLE)];               // vector table in simulated RAM (long word aligned)
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define SIM_LOCK
    #define SIM_DMA_3
    #define _SIM_PORT_CHANGE
    #ifdef _GNU
        #define __interrupt__
    #endif
    #include "config.h"
    #if defined _COMPILE_KEIL
	    extern void _main(void);
        extern void __main(void);                                        // Keil library initialisation routine
        #define START_CODE __main
    #elif defined COMPILE_IAR5
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE  _LowLevelInit
    #else
        extern int _main(void);
        #define START_CODE  _LowLevelInit
    #endif
    #ifdef ROWLEY_2                                                      // {3}
        #define asm(x) __asm__(x)
    #endif
#endif

#if defined SPI_SW_UPLOAD
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
        #define _EXTENDED_CS  iChipSelect,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST && !defined SPI_FLASH_SST25
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_lpc_atmel.h"
            #include "spi_flash_lpc_stmicro.h"
            #include "spi_flash_lpc_sst25.h"
        #undef _SPI_DEFINES
    #endif

// Configure the SPI interface for file system use - SSP used
//
extern int fnConfigSPIFileSystem(void)
{
    POWER_UP(PCSSP_X);                                                   // power up the SSP used
    CONFIGURE_SPI_PINS();
    CONFIGURE_CS_LINES();

    SSPCR1_X = 0;                                                        // ensure disabled (master mode)
    SSPCPSR_X = 2;                                                       // set clock prescaler (even prescale 2..254)

    SSPCR0_X = (FRS_FREESCALE_SPI | DSS_8_BIT | ((((SSP_CLOCK/2 + 2000000/2)/2000000) - 1) << SSP_SCR_SHIFT)); // set 2MHz frequency and mode
    SSPCR1_X = SSP_SSE;                                                  // enable
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
    #include "spi_flash_lpc_atmel.h"                                     // check the chip availablility
    #include "spi_flash_lpc_stmicro.h"
    #include "spi_flash_lpc_sst25.h"
    #undef _CHECK_SPI_CHIPS

    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {
        return 1;
    }
    return 0;

}
#endif


// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    WDMOD  = (WDEN | WDRESET);                                           // ensure watchdog is enabled
    WDFEED = 0xaa;                                                       // command reset to core and peripherals
    WDFEED = 0x55;
    WDFEED = 0xaa;
    WDFEED = 0;                                                          // bad feed sequence causes immediate reset
#ifdef _WINDOWS
    WDTV   = 0;
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
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_lpc_atmel.h"
        #include "spi_flash_lpc_stmicro.h"
        #include "spi_flash_lpc_sst25.h"
    #undef _SPI_FLASH_INTERFACE

    #ifdef SPI_FLASH_SST25
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
    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif
    return ucDestination;
}
#else
// Return the page number and optionally the address offset in the page
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset, int *iChipSelect)
    #else
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
    #endif
{
    unsigned short usPageNumber;
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address

    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
#endif
#endif


// Retrieve file contents from FLASH
//
extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
    #if defined SPI_SW_UPLOAD
    if (ParLocation >= ((unsigned char *)(SPI_FLASH_START))) {            // we are working from external SPI FLASH memory
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);

        #ifdef SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #else
            #if SPI_FLASH_PAGE_LENGTH >= 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #elif SPI_FLASH_PAGE_LENGTH >= 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #endif
        #endif
        return;
    }
    #endif

    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the LPC17XX uses a file system in FLASH with no access restrictions so we can simply copy the data
}


static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)];  // FLASH row backup buffer (on word boundary)
static unsigned char *ucFlashRow = (unsigned char *)ulFlashRow;         // pointer to array of bytes

static unsigned long fnGetFlashSector(unsigned long FlashAddress)
{
    FlashAddress -= FLASH_START_ADDRESS;
    if (FlashAddress < FLASH_SECTOR_16_OFFSET) {                         // one of the first small sectors
        return (FlashAddress/FLASH_GRANULARITY_SMALL);
    }
    else {                                                               // one of the large sectors
        FlashAddress -= FLASH_SECTOR_16_OFFSET;
        return ((FlashAddress/FLASH_GRANULARITY_LARGE) + 16);
    }
}

static int fnFlashCommand(int iCommand, unsigned char *ptrFlashAddress, MAX_FILE_LENGTH Length)
{
#define WRITE_FLASH_BUFFER 0
#define DELETE_FLASH       1
    IAP iap_entry = IAP_LOCATION;
    unsigned long command[5];                                            // command buffer to be passed to IAP routine
    unsigned long result[3];                                             // result buffer for use by the IAP routine
    switch (iCommand) {
    case WRITE_FLASH_BUFFER:                                             // write a present FLASH row backup buffer to the FLASH memory
        {
            unsigned long ulSector = fnGetFlashSector((unsigned long)ptrFlashAddress);
            command[0] = FLASH_PREPARE_SECTOR_TO_WRITE;                  // prepare the sector for the command
            command[1] = ulSector;
            command[2] = ulSector;
          //uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
          //uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // sector invalid or busy
            }
            command[0] = FLASH_COPY_RAM_TO_FLASH;
            command[1] = (unsigned long)ptrFlashAddress;                 // flash destination start address (256 byte boundard)
            command[2] = (unsigned long)ucFlashRow;                      // source ram address is always the ROW backup buffer (word aligned)
            command[3] = FLASH_ROW_SIZE;                                 // allowed: 256 | 512 | 1024 | 4096 | 8192 - we use always the row size
            command[4] = (MASTER_CLOCK/1000);                            // system clock frequency (CCLK) in kHz
          //uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
          //uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // write error
            }
        }
        break;

    case DELETE_FLASH:                                                   // delete a sector or several sectors in FLASH
        {
            unsigned long ulSectorStart = fnGetFlashSector((unsigned long)ptrFlashAddress);           // get the first sector to be deleted
            unsigned long ulSectorEnd   = fnGetFlashSector((unsigned long)(ptrFlashAddress + (Length - 1)));// get the last sector to be deleted
            if (Length == 0) {                                           // don't delete if length 0 is given
                break;
            }
            command[0] = FLASH_PREPARE_SECTOR_TO_WRITE;                  // prepare the sector for the command
            command[1] = ulSectorStart;
            command[2] = ulSectorEnd;
          //uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
          //uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // sector invalid or busy
            }
            command[0] = FLASH_ERASE_SECTORS;
            command[3] = (MASTER_CLOCK/1000);                            // system clock frequency (CCLK) in kHz
          //uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
          //uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // erase error
            }
        }
        break;
    }
    return 0;
}

// The LPC17xx writes blocks of 256 (FLASH_ROW_SIZE) bytes therefore these are collected here until a block boundary is crossed or else a closing command is received
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    static unsigned char *ptrOpenBuffer = 0;
    unsigned long ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1));
    unsigned char *ptrFlashBuffer;
    MAX_FILE_LENGTH BufferCopyLength;

    #if defined SPI_SW_UPLOAD
    if (ucDestination >= ((unsigned char *)(SPI_FLASH_START))) {         // we are working from external SPI FLASH memory
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        unsigned short usDataLength;
        #ifdef FLASH_LINE_SIZE
        if (ucData == 0) {                                               // ignore command to close an open line buffer (compatibility with internal FLASH)
            return 0;
        }
        #endif
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
        #ifdef SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);        // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
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
        return 0;
    }
    #endif

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
        ptrFlashBuffer = ucFlashRow + ulBufferOffset;                    // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= (ucFlashRow + FLASH_ROW_SIZE)) {           // a complete backup buffer is ready to be copied to FLASH
            fnFlashCommand(WRITE_FLASH_BUFFER, ptrOpenBuffer, 0);        // perform the write command
            uMemset(ucFlashRow, 0xff, FLASH_ROW_SIZE);                   // flush ready for next buffer use
            ptrFlashBuffer = ucFlashRow;                                 // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            ptrOpenBuffer += FLASH_ROW_SIZE;
        }
    } while (Length != 0);
    return 0;
}


// Erase FLASH sectors. The pointer can be anywhere in the first sector to be erased
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
#if defined FLASH_FILE_SYSTEM
    unsigned long ulPage = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // ensure page boundary
    ulPage -= FLASH_START_ADDRESS;
#endif
#ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
#endif
    do {
        #if defined SPI_SW_UPLOAD
        if (ptrSector >= ((unsigned char *)(SPI_FLASH_START))) {         // we are working from external SPI FLASH memory
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            unsigned short usPageNumber, usPageOffset;
            #ifndef SPI_FLASH_ST
            unsigned short usLength;
            unsigned char  ucCommand;
            #endif
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset EXTENDED_CS);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page

            #ifdef SPI_FLASH_ST
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
            #else
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                usLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                usLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);   // delete appropriate page/block
            if (Length <= usLength) {
                break;                                                   // delete complete
            }
            ptrSector += usLength;
            Length -= usLength;
            #endif
            continue;
        }
        #endif
        #ifdef FLASH_FILE_SYSTEM
        fnFlashCommand(DELETE_FLASH, ptrSector, Length);
        break;
        #endif
    } while (Length);

#ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
#endif
    return 0;
}


#if defined (_GNU)
extern const VECTOR_TABLE_MINIMUM reset_vect;                            // {2}
volatile void *ptrTest;

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
    if (ptrFlash != ptrData) {                                           // if a move is required
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


// Perform very low level initialisation - called by the start up code
//
#if defined COMPILE_IAR
static void _LowLevelInit(void) @ "ICODE"
#else
static void _LowLevelInit(void)
#endif
{
#if defined _GNU
    ptrTest = (volatile void *)&reset_vect;                              // {2} force GCC to link the reset table
#endif 
    // Configure the PLL for the desired operating frequency - note the exact sequence is very important
    //
    if (PLL0STAT & PLLCONNECTED) {                                       // if PLL main is already connected (can be connected by ISP)
        PLL0CON  = PLLE;                                                 // disconnect PLL, but leave enabled
        PLL0FEED = 0xaa;PLL0FEED = 0x55;                                 // perform feed sequence so that register changes are accepted
    }
    PLL0CON  = 0;                                                        // disable PLL and disconnect
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted

    #if CRYSTAL_FREQ > 15000000
    SCS |= (OSCRANGE | OSCEN);                                           // enable the oscillator
    #else
    SCS |= (OSCEN);                                                      // enable the oscillator
    #endif
    #ifndef _WINDOWS
    while (!(SCS & OSCSTAT)) {}                                          // wait for it to stabilise
    #endif

    #if defined USE_RC_OSCILLATOR
    CLKSRCSEL = PLL_SOURCE_4M_RC;                                        // ensure RC source
    #elif defined USE_32K_OSCILLATOR
    CLKSRCSEL = PLL_SOURCE_RTC;                                          // RTC crystal as sourcey
    #else
    CLKSRCSEL = PLL_SOURCE_OSCILLATOR;                                   // oscillator as PLL clock source
    #endif
#ifndef DISABLE_PLL
    PLL0CFG = PLL_FREQUENCY_CONFIGURATION;                               // PLL setting to generate required frequency
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted
    PLL0CON = PLLE;                                                      // enable the PLL to search for lock
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted
    USBCLKCFG = USB_FREQUENCY_CONFIGURATION;                             // set 48MHz USB clock if needed
    CCLKCFG = CPU_CLOCK_CONFIGURATION;                                   // set CPU clock frequency from PLL output
    while (!(PLL0STAT & PLLLOCKED)) {                                    // wait for lock to be achieved
    #ifdef _WINDOWS
        PLL0STAT = (PLLLOCKED | PLL_FREQUENCY_CONFIGURATION);
    #endif
    }
    PLL0CON = (PLLE | PLLC);                                             // connect PLL to clock CPU
    PLL0FEED = 0xaa;PLL0FEED = 0x55;                                     // perform feed sequence so that register changes are accepted
#endif

    FLASHCFG = (FLASHCFG_FIXED | FLASH_CPU_CLOCKS);                      // set optimum FLASH wait states

    PCLKSEL0 = 0;                                                        // peripheral clocks set to default 1/4 speed
    PCLKSEL1 = 0;
    PINSEL0  = 0x00000000;                                               // set all pin states and ports to default
    PINSEL1  = 0x00000000;
    PINSEL2  = 0x00000000;
    PINSEL3  = 0x00000000;
    PINSEL4  = 0x00000000;
    PINSEL5  = 0x00000000;
    PINSEL6  = 0x00000000;
    PINSEL7  = 0x00000000;
    PINSEL8  = 0x00000000;
    PINSEL9  = 0x00000000;
    PINSEL10 = 0x00000000;
    FIO0DIR  = 0x00000000;
    FIO1DIR  = 0x00000000;
    FIO2DIR  = 0x00000000;
    FIO3DIR  = 0x00000000;
    FIO4DIR  = 0x00000000;
    FIO0MASK = 0x00000000;
    FIO1MASK = 0x00000000;
    FIO2MASK = 0x00000000;
    FIO3MASK = 0x00000000;
    FIO4MASK = 0x00000000;
                                                  // initialise variables
#if defined (_GNU)
    __init_gnu_data();
#endif
#ifndef _WINDOWS
    #ifdef COMPILE_IAR
    __iar_program_start();
    #else
    _main();                                                             // call boot code
    #endif
#endif
}

#if defined _COMPILE_KEIL
// Keil demands the use of a main function with int return value
//
extern int main(void)
{
    _LowLevelInit();
    return 0;
}
#else
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

// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR5
__root const VECTOR_TABLE_MINIMUM __vector_table @ ".intvec"               // __root forces the function to be linked in IAR project
#elif defined COMPILE_IAR
__root const VECTOR_TABLE_MINIMUM reset_vect @ "RESETVECT"                 // __root forces the function to be linked in IAR project
#elif defined _GNU
const VECTOR_TABLE_MINIMUM __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
__attribute__((section("RESET"))) const VECTOR_TABLE_MINIMUM reset_vect
#else
const VECTOR_TABLE_MINIMUM reset_vect
#endif
= {
    {
        (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - IAP_STACK_SIZE - 4),  // stack pointer to top of RAM - leaving space for IAP and random number
        (void (*)(void))START_CODE,                                      // start address
    },
    0x53415475,                                                          // dummy vector values (for recognition)
    0x2052454b,
    0x3143504c,
    0x20585858,
    0x2e522e43,
#ifndef COMPILE_IAR5
    0xffffffff,                                                          // blank for check sum
#endif
};

    #if defined COMPILE_IAR5
    __root const unsigned long __vector_table_0x1c @ ".intvec" = 0xffffffff;
    #endif


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
    _LowLevelInit();
#endif

#ifdef _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
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

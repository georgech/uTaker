/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3SXXXX_boot.c
   Project:     Single Chip Embedded Internet - boot loader
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   *********************************************************************
   17.01.2009 Integrate SPI upload support                               {1}
   07.10.2009 Add Rowley debugger workaround                             {2}
   04.01.2010 Quieten VS warnings by removing strcpy()                   {3}
   30.07.2010 Correct fnConfigSPIFileSystem() power configuration names for Tempest Class compatibility {4}
   21.09.2010 Correct SPI FLASH page addressing when ATMEL FLASH is in power of twos mode {5}

*/

#if defined _LM3SXXXX

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#if defined _WINDOWS
    #include "config.h"
    #include "../../WinSim\WinSim.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    #define _SIM_PORT_CHANGE   fnSimPorts();                             // make sure simulator knows of change
    #define START_CODE 0
    #define _fnRAM_code fnCommandFlash
#else
    #ifdef _GNU
        #define __root
    #endif
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define _SIM_PORT_CHANGE
    #include "config.h"
    #ifdef _COMPILE_KEIL
        extern void __main(void);                                        // Keil library initialisation routine
        #define START_CODE __main
    #elif defined COMPILE_IAR5
        extern void __iar_program_start(void);                           // IAR library initialisation routine
        #define START_CODE __iar_program_start
        #define _main main
    #elif defined ROWLEY && !defined ROWLEY_1_7_17                       // {2} if project uses Rowley < V1.7 build 17
        #define START_CODE   _debugger_jump
    #else
        #define START_CODE   LM3SXXXX_LowLevelInit
    #endif
    #define _fnRAM_code fnRAM_code
#endif

#define _EXTENDED_CS

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
  //HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ; other method?
    RCGC0 |= CGC_WDT;                                                    // enable clocks to the watchdog module, if not presently active
    WDTLOCK = WD_ACCESS;                                                 // ensure we can access the watchdog registers
    WDTLOAD = 0;                                                         // immediate timeout
    WDTCTL = WD_RESEN;                                                   // enable reset on second timeout
    WDTCTL = (WD_RESEN | WD_INTEN);                                      // enable watchdog and interrupt
#if !defined _WINDOWS
    while (1) {}                                                         // wait for watchdog to fire
#endif
}

#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)           // {1}
{
    #if !defined _WINDOWS
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


// memcpy implementation
//
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr1 = (unsigned char *)ptrTo;
    unsigned char *ptr2 = (unsigned char *)ptrFrom;

    while (Size-- != 0) {
        *ptr1++ = *ptr2++;
    }

    return buffer;
}


// This routine to command FLASH - we put it in SRAM and run it with disabled interrupts
//
static void fnCommandFlash(unsigned long ulCommand, unsigned long ptrAdd, unsigned long ulValue)
{
    FMD = ulValue;                                                       // data value
    FMA = ptrAdd;                                                        // address in flash
    FMC = (WRKEY | ulCommand);                                           // command
        #if defined _WINDOWS
    switch (FMC & 0x0000000f) {
    case FLASH_WRITE:
        if ((~(*(unsigned long *)FMA)) & ulValue) {
            *(unsigned char *)0 = 0;                                     // signal attempt to program bits from 0 to 1
        }
        *(unsigned long *)FMA = FMD;
        break;

    case FLASH_ERASE:
        fnDeleteFlashSector(fnGetFlashAdd((unsigned char *)ptrAdd));      // simulate deletion
        break;
    }
    FMC &= ~0x0000000f;
        #endif
    while (FMC & ulCommand) {}                                           // wait until the command has been executed (about 50us)
}

#if defined _RUN_FLASH_FROM_RAM
// Initialise flash and routines in RAM ready for later use
//
static void fnInitFlash(void)
{
    #if defined _RUN_FLASH_FROM_RAM
    #define PROG_WORD_SIZE 100                                           // adequate space for the small program
    static unsigned short usProgSpace[PROG_WORD_SIZE];                   // make space for the routine on stack (this will have an even boundary)
    unsigned char *ptrThumb2 = (unsigned char *)fnCommandFlash;
    int i = 0;

    ptrThumb2--;                                                         // thumb 2 address
    while (i < PROG_WORD_SIZE) {                                         // copy program to SRAM
        usProgSpace[i++] = *(unsigned short *)ptrThumb2;
        ptrThumb2 += sizeof (unsigned short);
    }
    ptrThumb2 = (unsigned char *)usProgSpace;
    ptrThumb2++;                                                         // create a thumb 2 call
    fnRAM_code = (void(*)(unsigned long, unsigned long, unsigned long))(ptrThumb2);
    #endif

    USECRL = ((MASTER_CLOCK/1000000)-1);                                 // set FLASH programming usec value to suit system clock rate
}
#endif

#if defined SPI_SW_UPLOAD                                                // {1}
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_lm3s_atmel.h"
        #include "spi_flash_lm3s_stmicro.h"
        #include "spi_flash_lm3s_sst25.h"
    #undef _SPI_FLASH_INTERFACE

    #if !defined SPI_FLASH_SST25
// Return the page number and optionally the address offset in the page
//
static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
{
    unsigned short usPageNumber;
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
        #else
    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address
        #endif

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
    #endif                                                               // end not SST25

#if defined SPI_FLASH_SST25                                              // {1}
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
{
    ucDestination -= (uFILE_START + FILE_SYSTEM_SIZE);                   // location relative to the start of the SPI FLASH chip address
    return ucDestination;
}
#endif

// Configure the SPI interface for file system use
//
extern int fnConfigSPIFileSystem(void)
{
    RCGC2 |= CGC_GPIOA;                                                  // port A must be enabled to use SSI on it {4}
    SRCR2 &= ~CGC_GPIOA;
    RCGC1 |= CGC_SSI0;                                                   // enable SSI
    SRCR1 &= ~(CGC_SSI0);                                                // ensure not held in reset
    GPIOAFSEL_A |= (PORTA_BIT2 | PORTA_BIT4 | PORTA_BIT5);               // connect SSI pins
    GPIODEN_A |= (PORTA_BIT2 | PORTA_BIT4 | PORTA_BIT5);                 // use SSIRX,SSITX,SSICLK

    CONFIGURE_CS_LINES();                                                // configure chip select outputs

    SSICR1_0 = 0;                                                        // ensure disabled (master mode)
    SSICPSR_0 = 2;                                                       // set clock prescaler (even prescale 2..254)

    SSICR0_0 = (FRS_FREESCALE_SPI | DSS_8_BIT | (1 << SSI_SCR_SHIFT));   // set maximum possible SPI speed and mode
    SSICR1_0 = SSI_SSE;                                                  // enable
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
    #include "spi_flash_lm3s_atmel.h"                                    // check the chip availablility
    #include "spi_flash_lm3s_stmicro.h"
    #include "spi_flash_lm3s_sst25.h"
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
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
    unsigned char usRead[4];                                             // our file sytem is in external EEPROM via SPI bus.
    usRead[0] = M95XXX_READ_MEMORY;
    usRead[1] = (unsigned char)((unsigned short)(ParLocation)>> 8);
    usRead[2] = (unsigned char)((unsigned short)ParLocation);
    usRead[3] = PREPARE_READ;
    fnWaitWriteComplete();                                               // ensure the chip is ready to be read
        #ifdef SPI_INTERFACE
    fnWrite(SPI_handle, usRead, sizeof(usRead));                         // set the read address
    fnRead(SPI_handle, ptrValue, usSize);                                // read from the device to the return buffer
        #else
    fnSendSPIMessage(usRead, sizeof(usRead));                            // send immediately
    fnGetSPI_bytes(ptrValue, Size);                                      // fetch immediately
        #endif
    #else
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ParLocation >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
        #else
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory
        #endif
    {
        #ifdef SPI_FLASH_SST25
        ParLocation = fnGetSPI_FLASH_address(ParLocation);
        #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset);
        #endif
        #ifdef SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), ptrValue, Size);
        #elif defined SPI_FLASH_SST25
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, ptrValue, Size);
        #else
            #if SPI_FLASH_PAGE_LENGTH >= 1024                            // {5}
                #if SPI_FLASH_PAGE_LENGTH = 1024
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
    #if defined _WINDOWS
	ParLocation += (unsigned long)ucFLASH;
    #endif
    uMemcpy(ptrValue, ParLocation, Size);                                // the LM3S uses a file system in FLASH with no access restrictions so we can simply copy the data
    #endif
}



// Write a buffer to FLASH
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
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
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);         // write a page
        #else
        fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
        fnWriteBytesEEPROM(ucDestination, ucData, usPageLength);         // write a page
        #endif
        Length -= usPageLength;
        ucData += usPageLength;
        ucDestination += usPageLength;
    }
    return 0;
    #else
    // we have already checked that it is possible to modify the byte requested but we have to be sure that we only write 32 bits to long word addresses
    unsigned long *ptrLong;
    unsigned long ulValue;
    int iByteInLong;

        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))  // we are working from external SPI FLASH memory
        #endif
    {
        #if defined SPI_FLASH_SST25
        int iMultipleWrites = 0;
        #endif
        unsigned short usDataLength;
        #if defined SPI_FLASH_SST25
        if (Length == 0) {
            return 0;                                                    // ignore if length is zero
        }
        ucDestination = fnGetSPI_FLASH_address(ucDestination);           // convert to virtual SPI Flash memory location
        if ((CAST_POINTER_ARITHMETIC)ucDestination & 0x1) {              // start at odd SPI address, requires an initial byte write
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // command write enable to allow byte programming
            fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, ucData, 1); // program last byte
            if (--Length == 0) {                                         // single byte write so complete
                return 0;
            }
            ucDestination++;
            ucData++;
        }
        fnSPI_command(WRITE_ENABLE, 0, 0, 0);                            // command write enable to allow programming
        #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset);
        #endif
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
    #ifdef SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, ucData, usDataLength);// copy new content
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
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, ucData, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                    iMultipleWrites = 1;                                 // mark that we are in a AAI sequence
                }
                else {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, ucData, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
                }
                ucDestination += AAI_LENGTH;
                Length -= AAI_LENGTH;
                ucData += AAI_LENGTH;
            }
            else {
                if (iMultipleWrites != 0) {
                    fnSPI_command(WRITE_DISABLE, 0, 0, 0);               // first close AAI sequence
                    fnSPI_command(WRITE_ENABLE, 0, 0, 0);                // command write enable to allow byte programming
                }
                fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, ucData, 1); // program last byte
                break;
            }
    #else
            if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) { // are we writing a partial page?
                fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, 0, 0);     // copy main memory to buffer
            }
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_BUFFER_1, usPageOffset, ucData, usDataLength);// copy new content
            fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, 0, 0);       // program to main memory
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
    #endif
        }
    #if defined SPI_FLASH_SST25
        fnSPI_command(WRITE_DISABLE, 0, 0, 0);                           // disable writes
    #endif
        return 0;
    }
        #endif

        #if defined _WINDOWS
    ptrLong = (unsigned long *)((unsigned long)((unsigned long)ucDestination + (unsigned long)ucFLASH - FLASH_START_ADDRESS) & ~0x3);  // long address starting on
    iByteInLong = (ucDestination - FLASH_START_ADDRESS + (unsigned long)ucFLASH) - (unsigned char *)ptrLong;
        #else
    ptrLong = (unsigned long *)((unsigned long)ucDestination & ~0x3);    // long address starting on
    iByteInLong = ucDestination - (unsigned char *)ptrLong;              // long word align
        #endif
    ulValue = *ptrLong;                                                  // present value at location

    while (Length-- != 0) {
    #if defined (_LITTLE_ENDIAN) || defined (_WINDOWS)
        ulValue &= ~(0xff << (iByteInLong*8));                           // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong*8));                       // insert new byte
    #else
        ulValue &= ~(0xff << ((3 - iByteInLong)*8));                     // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong)*8));                 // insert new byte
    #endif
        if (iByteInLong == 3) {                                          // can we write a long?
    #if defined _RUN_FLASH_FROM_RAM
            uDisable_Interrupt();                                        // protect call from interrupts
                _fnRAM_code(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong++, ulValue); // program the long word
            uEnable_Interrupt();                                         // re-enable interrupts
    #else
            fnCommandFlash(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong++, ulValue); // program the long word
    #endif
            if (Length == 0) {
                return 0;                                                // completed
            }
            ulValue = *ptrLong;
            iByteInLong = 0;
        }
        else {
            iByteInLong++;
        }
    }
    #if defined _RUN_FLASH_FROM_RAM
    uDisable_Interrupt();                                                // protect call from interrupts
        _fnRAM_code(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong, ulValue); // program the long word
    uEnable_Interrupt();                                                 // re-enable interrupts
    #else
    fnCommandFlash(FLASH_WRITE, (CAST_POINTER_ARITHMETIC)ptrLong, ulValue); // program the long word
    #endif
    return 0;
#endif
}


// Erase FLASH sectors. The pointer can be anywhere in the first sector to be erased
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
#if defined FLASH_FILE_SYSTEM
    unsigned long ulPage = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // ensure page boundary
    ulPage -= FLASH_START_ADDRESS;
#endif
#if defined PROTECTED_FILE
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
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
            #else
        if ((ptrSector >= (uFILE_SYSTEM_END)) && (ptrSector < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory
            #endif
        {
            #ifndef SPI_FLASH_ST
            MAX_FILE_LENGTH BlockLength;
            unsigned char  ucCommand;
            #endif
            #ifdef SPI_FLASH_SST25
            unsigned long ulSectorAlign;
            unsigned long ulLocation = (CAST_POINTER_ARITHMETIC)fnGetSPI_FLASH_address(ptrSector); // convert to virtual SPI Flash memory location
            ulSectorAlign = (ulLocation & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1));
            Length += (ulLocation - ulSectorAlign);
            ptrSector -= (ulLocation - ulSectorAlign);
            #else
            unsigned short usPageNumber, usPageOffset;
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page
            #endif
            #ifdef SPI_FLASH_ST
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // enable the write
                #ifdef SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #elif defined SPI_FLASH_SST25
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // command write enable to allow byte programming
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
            fnSPI_command(ucCommand, ulLocation, 0, 0);
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
            fnSPI_command(ucCommand, usPageNumber, 0, 0);                // delete appropriate page/block
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #endif
            continue;
        }
        #endif

        #if defined _RUN_FLASH_FROM_RAM
        uDisable_Interrupt();                                             // protect the call from interrupts
            _fnRAM_code(FLASH_ERASE, ulPage, 0);                          // command page erase
        uEnable_Interrupt();                                              // enable interrupts again
        #else
        fnCommandFlash(FLASH_ERASE, ulPage, 0);                           // command page erase
        #endif
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        ulPage += FLASH_GRANULARITY;
        Length -= FLASH_GRANULARITY;

    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);

#if defined PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
#endif
    return 0;
}

#if defined (COMPILE_IAR)

#pragma segment="DATA_ID"
#pragma segment="DATA_I"
#pragma segment="DATA_Z"

static void __segment_init(void)
{
    unsigned long *pulSrc, *pulDest, *pulEnd;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = __segment_begin("DATA_ID");
    pulDest = __segment_begin("DATA_I");
    pulEnd = __segment_end("DATA_I");
    while(pulDest < pulEnd) {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    pulDest = __segment_begin("DATA_Z");
    pulEnd = __segment_end("DATA_Z");
    while(pulDest < pulEnd) {
        *pulDest++ = 0;
    }
}
#elif defined (_GNU)
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


// Perform very low level initialisation - called by the start up code
//
extern void LM3SXXXX_LowLevelInit(void)
{
    RCC &= ~USESYSDIV;
    RCC |= BYPASS;                                                       // ensure clocking is from raw clock source
    RCC &= ~(XTAL_8_1920 | OSCSRC_MASK | SYSDIV_32);                     // mask out XTAL frequency and divider fields and set main oscillator source
    RCC |= (CRYSTAL_SPEED_VALUE | PLL_OUTPUT_VALUE);                     // select cyrstal and required PLL output frequency
    RCC &= ~(PWRDN);                                                     // clear PLL power down bit
    RCC |= USESYSDIV;                                                    // enable system divide

    while ((RIS & PLLLRIS) == 0) {                                       // wait for PLL to lock
#if defined _WINDOWS
        RIS |= PLLLRIS;
#endif
    }
    RCC &= ~BYPASS;                                                      // switch to PLL source

#if defined (COMPILE_IAR)
    __segment_init();                                                    // initialise variables
#elif defined (_GNU)
    __init_gnu_data();
#endif
#if !defined _WINDOWS
    #if defined _RUN_FLASH_FROM_RAM
    fnInitFlash();
    #endif
    uTaskerBoot();
#endif
}

#if defined ROWLEY && !defined ROWLEY_1_7_17                             // {2} Rowley project requires extra initialisation for debugger to work correctly before V1.7 build 17
static void _debugger_jump(void)
{
    asm("mov lr, #0");
    asm("b LM3SXXXX_LowLevelInit");
}
#endif

#if defined _COMPILE_KEIL
// Keil demands the use of a main function with int return value
//
extern int main(void)
{
    LM3SXXXX_LowLevelInit();
    return 0;
}
#endif

// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined COMPILE_IAR5
    __root const RESET_VECTOR __vector_table @ ".intvec"                 // __root forces the function to be linked in IAR project
#elif defined COMPILE_IAR
    __root const RESET_VECTOR reset_vect @ "RESETVECT"                   // __root forces the function to be linked in IAR project
#elif defined _GNU
    const RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _COMPILE_KEIL
    __attribute__((section("RESET"))) const RESET_VECTOR reset_vect
#else
    const RESET_VECTOR reset_vect
#endif
= {
    (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - 8),                       // stack pointer to top of RAM
    START_CODE
};









#if defined _WINDOWS
// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)
{
    unsigned char ucValue = (unsigned char)iValue;
    void *buffer = ptrTo;
    unsigned char *ptr = (unsigned char *)ptrTo;

    while (Size-- != 0) {
        *ptr++ = ucValue;
    }

    return buffer;
}

// The following routines are only for simulator compatibility

extern void *fnGetHeapStart(void) { return 0; }


// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
    signed char cDummyMac[] = "--:--:--:--:--:--";
    int i = 0;
    while (cDummyMac[i] != 0) {                                          // {3}
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
    while (cDummyIP[i] != 0) {                                           // {3}
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
INITHW void fnInitHW(void)                                               // perform hardware initialisation
{
#if defined GLOBAL_HARDWARE_TIMER
  static __interrupt void hwtimer_interrupt(void);
  volatile unsigned int dummy;
#endif
#if defined _WINDOWS
    unsigned long ulPortPullups[] = {
        0xffffffff,                                                      // set the port states out of reset in the project file app_hw_lm3sxxxx.h
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0xffffffff
    };
    fnInitialiseDevice((void *)ulPortPullups);
#endif
    LM3SXXXX_LowLevelInit();
}


extern void uTaskerBoot(void);
extern void uTaskerSchedule( void )
{
    static int iDone = 0;

    if (iDone == 0) {
        iDone = 1;
        uTaskerBoot();
    }
}
 #endif
#endif

/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        M5223X_boot.c [FREESCALE Coldfire]
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************

    11.08.2007 SPI FLASH support added                                   {1}
    12.09.2007 Added start up delay to ensure SPI FLASH is ready         {2}
    16.12.2007 Extend SPI support to include all page sizes              {3}
    26.01.2008 Correct Flash state-machine frequency and improve accuracy{4}
    18.03.2008 Remove chip select parameter from fnSimAT45DBXXX()         
    26.06.2008 Use SPI FLASH includes from main project, adding ST FLASH support {5}    
    28.06.2008 Initialise QSPI control registers                         {6}
    19.01.2009 Add SST SPI FLASH                                         {7}
    19.01.2009 Add M5225X support                                        {8}
    15.02.2009 Remove strcpy() use (only relevant for simulator)         {9}
    12.02.2012 Add Coldfire IAR support                                  {10}
	26.02.2015 Add power of 2s ATMEL flash support                       {11}

*/

#ifdef _M5223X

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

 
#ifdef _WINDOWS
    #include "config.h"

    #include "../../WinSim/WinSim.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    #define __interrupt__ 
    #define SIM_LOCK MCF_CLOCK_SYNSR |= PLLOCKED;
    #define _LITTLE_ENDIAN
    #define SIM_DMA_3 fnSimDMA(0);
    #define BACKDOOR_FLASH 0

    NETWORK_PARAMETERS network[1];
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #define SIM_LOCK
    #define SIM_DMA_3
    #define BACKDOOR_FLASH (CFM_FLASH)
    #ifdef _CODE_WARRIOR_10
        #define __interrupt__   __declspec(interrupt)  
    #elif defined _GNU
        #define __interrupt__
    #elif defined _COMPILE_IAR                                           // {10}
        #include <intrinsics.h>
        #define __interrupt__  __interrupt
        #define asm_int_off()  __disable_interrupts()                    // IAR intrinsic
        #define asm_int_on()   __enable_interrupts()                     // IAR intrinsic
        __no_init unsigned long __VECTOR_RAM[256] @ "VECTOR_RAM";        // vector table reserved at start of SRAM
        #define mcf52235_boot_init mcf52235_init
    #endif
    #include "config.h"
#endif  

#define _EXTENDED_CS                                                     // {3}
#undef  BUS_CLOCK
#define BUS_CLOCK  OSCCLK                                                // {4} no PLL set up so bus speed is the same as the oscillator speed
static int fnWriteLongWordFlash(unsigned long *ptrAdd, unsigned long ulValue);

// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    RESET_RCR = SOFT_RST;                                                // command a soft reset of the board
}

#if !defined _WINDOWS && !defined _GNU
// default interrupt handler - an undefined interrupts will arrive here
//
static __interrupt__ void undef_int(void)
{
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

// Boot initialisation doesn't program the PLL. It copies the data required by the boot program and fills the exception vector with default interrupt hanlder
//
extern void mcf52235_boot_init(void)
{
#if !defined _GNU && !defined _COMPILE_IAR
	extern char __DATA_ROM[];
	extern char __DATA_RAM[];
	extern char __DATA_END[];
	extern char __BSS_START[];
	extern char __BSS_END[];
	extern unsigned long VECTOR_TABLE[];
	extern unsigned long __VECTOR_RAM[];
	extern unsigned long FLASH_CONFIG[];	
#endif

#if !defined _WINDOWS && !defined _GNU
    register unsigned long n;
    register unsigned char *dp, *sp;
 #endif

#if defined _M5225X                                                      // {8} enable crystal and switch off relaxation oscillator
    MCF_CLOCK_OCLR = (OSCEN | REFS);                                     // enable external crystal oscillator
    MCF_CLOCK_CCLR = 0x00;                                               // switch to crystal source
    MCF_CLOCK_OCHR = 0x00;                                               // disable releaxation oscillator
#endif

    MCF_SCM_RAMBAR = ((RAM_START_ADDRESS & RAMBAR_ADD_MASK) | BACK_DOOR_ACCESS); // enable SRAM access by peripherals

#if !defined _WINDOWS && !defined _GNU && !defined _COMPILE_IAR
    __VECTOR_RAM[0] = FLASH_CONFIG[0];                                   // force flash config table to be linked

    if (__VECTOR_RAM != VECTOR_TABLE) {                                  // fill the vector table in RAM with a handler for undefined interrupts
        for (n = 0; n < 256; n++)
	__VECTOR_RAM[n] = (unsigned long)undef_int;
    }

    mcf5xxx_wr_vbr((unsigned long)__VECTOR_RAM);                         // move vector to RAM position
    if (__DATA_ROM != __DATA_RAM) {                                      // move initialized data from ROM to RAM.
        dp = (unsigned char *)__DATA_RAM;
        sp = (unsigned char *)__DATA_ROM;
        n = (unsigned long)(__DATA_END - __DATA_RAM);
        while (n--) {
            *dp++ = *sp++;
        }
    }

    if (__BSS_START != __BSS_END) {                                      // zero uninitialised data
        sp = (unsigned char *)__BSS_START;
        n = (unsigned long)(__BSS_END - __BSS_START);
        while (n--) {
            *sp++ = 0;
        }
    }
#endif
}

#ifdef _BOOT_USES_INTERRUPTS
static int iInterruptLevel = 0;

// Routine to disable interrupts during critical regions
//
void uDisable_Interrupt()
{
#ifndef _WINDOWS
    asm_int_off();
#endif
    iInterruptLevel++;                                                   // monitor the level of interrupt nesting
}

// Routine to re-enable interrupts on leaving a critical region
//
void __enable_interrupt()                                                // interrupts are ALWAYS off when entering
{
#if defined _WINDOWS
    if (!iInterruptLevel) {
        *(int *)0 = 0;                                                   // basic error - casue exception
    }
#endif
    if (!(--iInterruptLevel)) {                                          // only when no more interrupt nesting,
#if !defined _WINDOWS
        asm_int_on();
#endif
    }
}
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

#ifdef _BOOT_USES_INTERRUPTS
#ifdef _WINDOWS
  #define PROG_SIZE  100
#else
  #define PROG_SIZE  80                                                  // optimised from experience
#endif

static unsigned char ucProgSpace[PROG_SIZE];
static void (*fnRAM_code)(void) = (void(*)(void))ucProgSpace;
#endif

static void fnFlashRoutine(void)
{
#ifdef _WINDOWS
    CFMUSTAT = (CBEIF | CCIF);
#else
    CFMUSTAT = CBEIF;                                                    // launch command
#endif
    while (!(CFMUSTAT & CBEIF)) {}                                       // wait until buffer free
    while (!(CFMUSTAT & CCIF)) {}                                        // wait until execution complete
}

static void fnConfigFlash(void)
{
    CFMUSTAT = (ACCERR | PVIOL);                                         // clear error flags

    if (CFMCLKD & DIVLD) return;                                         // already configured
    else {
#ifdef _BOOT_USES_INTERRUPTS
        unsigned char *ptrProg = (unsigned char*)fnFlashRoutine;
        int i = 0;

        while (i < PROG_SIZE) {                                          // copy program to stack
          ucProgSpace[i++] = *ptrProg++;
        }
#endif

#if BUS_CLOCK > (12800000)                                               // {4} divider required above this frequency (required clock is between 150kHz and 200kHz - incorrect value > 200kHz could reduce programming reliability. incorrect value < 150kHz could result in FLASH damage)
    #define FLASH_DIV   ((BUS_CLOCK/200000/8/2) - 1) 
    #define FLASH_ROUND (((BUS_CLOCK + ((200000*8*2) - 1))/200000/8/2) - 1)
    #if FLASH_ROUND > FLASH_DIV
        #undef FLASH_DIV  
        #define FLASH_DIV FLASH_ROUND
    #endif
    CFMCLKD = (PRDIV8 | FLASH_DIV);                                      // set the FLASH state machine frequency as close to, but not higher than 200kHz
#else
    #define FLASH_DIV   ((BUS_CLOCK/200000/2) - 1) 
    #define FLASH_ROUND (((BUS_CLOCK + ((200000*2) - 1))/200000/2) - 1)
    #if FLASH_ROUND > FLASH_DIV
        #undef FLASH_DIV  
        #define FLASH_DIV FLASH_ROUND
    #endif
    CFMCLKD = FLASH_DIV;                                                 // set the FLASH state machine frequency as close to, but not higher than 200kHz
#endif
#ifdef _WINDOWS
        CFMCLKD |= DIVLD;                                                // set the loaded bit in the divide register
        CFMUSTAT |= CBEIF;                                               // no command queued
#endif
    }
}

static int fnFlashNow(unsigned char ucCommand, unsigned long *ptrWord, unsigned long ulWord)
{
#ifdef _BOOT_USES_INTERRUPTS
    uDisable_Interrupt();                                                // protect this region from interrupts
#endif

#ifdef _WINDOWS                                                          // command sequence - set the word to the address (in the sector)
    if (ucCommand != CMD_SECTOR_ERASE) {
      *ptrWord = ulWord;                                                 // value not important for deletes
    }
#else
    ptrWord += (CFM_FLASH/sizeof(unsigned long));                        // position the write into CFM FLASH (a copy of real FLASH for commands)
    *ptrWord = ulWord;                                                   // value not important for deletes
#endif
    CFMCMD = ucCommand;                                                  // the command

#ifdef _BOOT_USES_INTERRUPTS
    fnRAM_code();                                                        // execute from RAM [stack]
#else
    fnFlashRoutine();                                                    // run routine from FLASH
#endif

#ifdef _BOOT_USES_INTERRUPTS
    __enable_interrupt();                                                // safe to accept interrupts again
#endif

    return (CFMUSTAT & (ACCERR | PVIOL));                                // if there was an error this will be non-zero
}

#ifdef SPI_SW_UPLOAD                                                     // if SPI FLASH is holding the new code

/* =================================================================== */
/*                      SPI FLASH driver interface                     */
/* =================================================================== */

static int SPI_FLASH_Danger[1] = {0};                                    // signal that the FLASH status should be checked before using since there is a danger that it is still busy {5} - array for compatibility


#ifdef SPI_FLASH_SST25                                                  // {7}
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
{
    ucDestination -= (uFILE_START + FILE_SYSTEM_SIZE);                   // location relative to the start of the SPI FLASH chip address
    return ucDestination;
}
#else
// Return the page number and optionally the address offset in the page
//
static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
{
    unsigned short usPageNumber;

    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address
    #if SPI_FLASH_PAGE_LENGTH != 256                                     // B-device or D-device set to 256 page mode
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
        *usPageOffset = (unsigned short)ptrSector;                       // remainder is offset in the page
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

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
    #include "spi_flash_m5223x_atmel.h"                                  // {5}
    #include "spi_flash_m5223x_stmicro.h"
    #include "spi_flash_m5223x_sst25.h"                                  // {7}
    #undef _SPI_FLASH_INTERFACE

extern int fnConfigSPIFileSystem(void)
{
    int iFifoDepth = 16;
    unsigned char ucSPI_FLASH_Type[1];                                   // {5}
    PORTQS |= QSPI_CS0_LINE;                                          
    DDRQS |= QSPI_CS0_LINE;                                              // drive CS line high

    PQSPAR |= 0x0015;                                                    // set QSPI_CLK, QSPI_DIN and QSPI_DOUT on QS port

    QMR = ((QSPI_MSTR | QSPI_8BITS) | 2);                                // set mode and fastest speed
    QAR = QSPI_COMMAND_RAM_0;                                            // set address to first control location {60}
    while (iFifoDepth--) {                                               // initialise all control registers
        QDR = (QSPI_BITSE | QSPI_CS_3 | QSPI_CS_2 | QSPI_CS_1 | QSPI_CS_0); // no chip selects since we control via port - 8 bit access
    }
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
    #include "spi_flash_m5223x_atmel.h"                                  // {5} - check the chip availablility
    #include "spi_flash_m5223x_stmicro.h"
    #include "spi_flash_m5223x_sst25.h"                                  // {7}
    #undef _CHECK_SPI_CHIPS

    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {                 // {5}
        return 1;
    }
    return 0;
}
#endif


// Erase a FLASH sector. The pointer can be anywhere in the sector to be erased
//
extern unsigned long fnEraseFlash(unsigned char *ptrSector) 
{
#ifdef SPI_SW_UPLOAD 
    if (ptrSector >= uFILE_SYSTEM_END) { 
    #ifdef SPI_FLASH_SST25                                               // {7}
        unsigned long ulLocation = (CAST_POINTER_ARITHMETIC)fnGetSPI_FLASH_address(ptrSector);  // convert to virtual SPI Flash memory location
    #else
        unsigned short usPageNumber = fnGetSPI_FLASH_location(ptrSector, 0);
    #endif
    #ifdef SPI_FLASH_ST                                                  // {5}
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // enable the write/delete
        #ifdef SPI_DATA_FLASH
        fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
        #else
        fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sector
        #endif
    #elif defined SPI_FLASH_SST25                                        // {7}                                
        fnSPI_command(SUB_SECTOR_ERASE, ulLocation, _EXTENDED_CS 0, 0);  // delete smallest sector of 4k
    #else
        fnSPI_command(BLOCK_ERASE, usPageNumber, 0, 0);                  // delete appropriate block
    #endif
        return SPI_FLASH_BLOCK_LENGTH;
    }
    else {
#endif

#ifdef _WINDOWS
        (unsigned long)ptrSector &= ~(FLASH_GRANULARITY-1);              // set to sector boundary
        ptrSector = (unsigned char *)((unsigned long)ucFLASH + (unsigned long)ptrSector);
#endif

        fnConfigFlash();
#ifdef _WINDOWS
        CFMUSTAT |= CBEIF;                                               // no command queued
#endif
        if (!(CFMUSTAT & CBEIF)) {
            return 0;                                                    // the command buffers are full - we can not proceed
        }

#ifdef _WINDOWS
        fnDeleteFlashSector((unsigned char *)ptrSector);                 // simulate deletion
#endif

        if ((fnFlashNow(CMD_SECTOR_ERASE, (unsigned long *)((unsigned long)ptrSector & ~0x3), (unsigned long)0)) != 0) {
            return 0;
        }
        return FILE_GRANULARITY;
#ifdef SPI_SW_UPLOAD
    }
#endif
}


extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    // we have already checked that it is possible to modify the byte requested but we have to be sure that we only write 32 bits to long word addresses
#ifdef _WINDOWS
    unsigned long *ptrLong = (unsigned long *)((unsigned long)((unsigned long)ucDestination + (unsigned long)ucFLASH - FLASH_START_ADDRESS) & ~0x3);  // long address starting on
    unsigned char *ptr;
#else
    unsigned long *ptrLong = (unsigned long *)((unsigned long)ucDestination & ~0x3);  // long address starting on
#endif
    unsigned long ulValue = *ptrLong;                                    // present value at location
#ifdef _WINDOWS
    int iByteInLong = (ucDestination - FLASH_START_ADDRESS + (unsigned long)ucFLASH) - (unsigned char *)ptrLong;
#else
    int iByteInLong = ucDestination - (unsigned char *)ptrLong;
#endif

    while (Length--) {
#ifdef _LITTLE_ENDIAN
        ulValue &= ~(0xff << (iByteInLong*8));                           // clear space for new byte
        ulValue |= (*ucData++ << (iByteInLong*8));                       // insert new byte
#else
        ulValue &= ~(0xff << ((3 - iByteInLong)*8));                     // clear space for new byte
        ulValue |= (*ucData++ << ((3 - iByteInLong)*8));                 // insert new byte
#endif
        if (iByteInLong == 3) {                                          // can we write a long ?
#ifdef _WINDOWS
            ptr = (unsigned char *)ptrLong;
            ptr -= (unsigned long)ucFLASH;
            ptr += FLASH_START_ADDRESS;
            if (fnWriteLongWordFlash((unsigned long *)ptr, ulValue) != 0) return 1;
#else
            if (fnWriteLongWordFlash(ptrLong, ulValue) != 0) return 1;
#endif

            if (!Length) return 0;                                       // completed

            ulValue = *++ptrLong;
            iByteInLong = 0;
        }
        else {
          iByteInLong++;
        }
    }
#ifdef _WINDOWS
    ptr = (unsigned char *)ptrLong;
    ptr -= (unsigned long)ucFLASH;
    ptr += FLASH_START_ADDRESS;
    return (fnWriteLongWordFlash((unsigned long *)ptr, ulValue));
#else
    return (fnWriteLongWordFlash(ptrLong, ulValue));
#endif
}

// The address must be on a word boundary or it will fail
//
static int fnWriteLongWordFlash(unsigned long *ptrAdd, unsigned long ulValue)
{
 #ifdef _WINDOWS
    ptrAdd = (unsigned long *)((unsigned long)ucFLASH + (unsigned long)ptrAdd);
 #endif
    fnConfigFlash();

 #ifdef _WINDOWS
    CFMUSTAT |= CBEIF;                                                   // no command queued
 #endif
    if (!(CFMUSTAT & CBEIF)) return 1;                                   // the command buffers are full - we can not program yet

 #ifdef _WINDOWS
    if (ulValue & ~(*(unsigned long*)ptrAdd)) {                          // we check that we are not trying to program a bit from '0' to '1'
        *(unsigned char*)0 = 0;;                                         // this will fail so signal it when debugging
    }
 #endif
    return (fnFlashNow(CMD_PROGRAMM_WORD, ptrAdd, ulValue));
}



extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef SPI_SW_UPLOAD                                                     // {1}
    if (ParLocation >= uFILE_SYSTEM_END) {                               // get from SPI FLASH
    #ifdef SPI_FLASH_SST25                                               // {7}
        ParLocation = fnGetSPI_FLASH_address(ParLocation);
    #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset);
    #endif

    #ifdef SPI_FLASH_ST                                                  // {5}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
    #elif defined SPI_FLASH_SST25                                        // {7}
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, _EXTENDED_CS ptrValue, Size);
    #else                                                                // Atmel
        #if SPI_FLASH_PAGE_LENGTH >= 1024
            #if SPI_FLASH_PAGE_LENGTH == 1024                            // {11}
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #endif
        #elif SPI_FLASH_PAGE_LENGTH >= 512
            #if SPI_FLASH_PAGE_LENGTH == 512                             // {11}
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #endif
        #else
            #if SPI_FLASH_PAGE_LENGTH == 256                             // {11}
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
    uMemcpy(ptrValue, ParLocation, Size);                                // the Coldfire uses a file system in FLASH with no access restrictions so we can simply copy the data
}

extern int uFileErase(unsigned char *ptrFile, MAX_FILE_LENGTH FileLength)
{
    unsigned long ulFileGranilarity;

    while (FileLength) {
        if ((ulFileGranilarity = fnEraseFlash(ptrFile)) == 0) {          // erase a file block
            return 1;                                                    // return error if one found
        }
        ptrFile += ulFileGranilarity;
        if (FileLength <= ulFileGranilarity) {
            return 0;
        }
        else {
            FileLength -= ulFileGranilarity;
        }
    }
    return 0;
}

extern MAX_FILE_LENGTH uGetFileLength(unsigned char *ptrfile)
{
    MAX_FILE_LENGTH FileLength;

    fnGetPars(ptrfile, (unsigned char *)&FileLength, sizeof(MAX_FILE_LENGTH));

    if (FileLength == 0) {                                               // protect against corrupted file so that it can be deleted
        return FILE_GRANULARITY;                                         // the size of a flash block
    }

    if (FileLength == (MAX_FILE_LENGTH)(-1)) {                           // check for blank FLASH memory
        return 0;                                                        // no length so empty
    }

    // if a (corrupted) length should be too large, we limit it to end of the file system
    if (((CAST_POINTER_ARITHMETIC)((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER)) - (CAST_POINTER_ARITHMETIC)ptrfile) < FileLength) {
        FileLength = (MAX_FILE_LENGTH)((CAST_POINTER_ARITHMETIC)(unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER) - (CAST_POINTER_ARITHMETIC)ptrfile); // the space remaining
    }

    return FileLength;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    #ifdef _WINDOWS
// The following routines are only for simulator compatibility
//
extern void *fnGetHeapStart(void) { return 0; }

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

// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
  //signed char cDummyMac[] = "--:--:--:--:--:--";
  //strcpy(cStr, cDummyMac);                                             {9}
  //return (cStr + strlen(cDummyMac));
	return cStr;
}

// Convert an IP address to a string
//
signed char *fnIPStr(unsigned char *ptrIP, signed char *cStr)
{
  //signed char cDummyIP[] = "---.---.---.---";
  //strcpy(cStr, cDummyIP);                                              // {9}
  //return (cStr + strlen(cDummyIP));
	return cStr;
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
        QS_DEFAULT_INPUT,                                                // set the port states out of reset in the project file app_hw_m5223x.h
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

    __VECTOR_RAM[PIT0_VECTOR] = (unsigned long)fnDummyTick;

    fnInitialiseDevice((void *)ucPortPullups);                                                    
    mcf52235_boot_init();                                                // call initialisation routine for code checking only
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

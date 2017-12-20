/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_boot.c
    Project:   Single Chip Embedded Internet - boot loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    17.04.2012 Add start_application() and flash support for KINETIS_K_FPU types {1}
    25.08.2013 Allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined {2}
    18.06.2014 Implement fnFlashRoutine() as assemble code to avoid possibility of compilers in-lining it {3}
    27.01.2015 Add Green Hills project support (_COMPILE_GHS)            {4}
    18.04.2016 Add clock configuration options RUN_FROM_HIRC, RUN_FROM_HIRC_FLL and RUN_FROM_HIRC_PLL {5}
    13.07.2017 Add w25q (winbond) and s25fl1-k (spansion) SPI Flash support {6}
    13.07.2017 Use standard SPI Flash interface                          {7}
    13.07.2017 Use standard clock configuration                          {8}
    17.07.2017 Use standard port configuration                           {9}
    26.07.2017 Add Cortex-m0+ assembler code                             {10}
    20.12.2017 Change uMemset() to match memset() parameters             {11}

    */

#if defined _KINETIS

#if defined _WINDOWS
    #include "config.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    extern void fec_txf_isr(void);
    extern void fnSimulateDMA(int channel);
    #define START_CODE 0
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #define  fnSimulateDMA(x)
    #if defined ROWLEY || defined _KDS
        #define asm(x) __asm__(x)
    #endif
    #if defined _COMPILE_KEIL
        #define START_CODE main
    #elif defined _COMPILE_IAR
        #define START_CODE disable_watchdog
    #elif defined _COMPILE_GHS                                           // {4}
        extern void _start_T(void);                                      // GHS library initialisation routine
        #define START_CODE disable_watchdog
    #else
        #define START_CODE main
    #endif
#endif

#if !defined ONLY_INTERNAL_FLASH_STORAGE
    static const STORAGE_AREA_ENTRY default_flash = {                    // {7}
        0,                                                               // end of list
        (unsigned char *)(FLASH_START_ADDRESS),                          // start address of internal flash
        (unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH - 1)),    // end of internal flash
        _STORAGE_INTERNAL_FLASH,                                         // type
        0                                                                // not multiple devices
    };

    STORAGE_AREA_ENTRY *UserStorageListPtr = (STORAGE_AREA_ENTRY *)&default_flash; // default entry
#endif

#if defined SPI_SW_UPLOAD
    #define SPI_FLASH_DEVICE_COUNT 1
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];
    #define _EXTENDED_CS
#endif
static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)];   // FLASH row backup buffer (on word boundary)


#if defined _COMPILE_IAR || defined _COMPILE_GHS                         // {4}
extern void __iar_program_start(void);                                   // IAR library initialisation routine
// This is the first function called so that it can immediately disable the watchdog so that it doesn't fire during variable initialisation
//
static void disable_watchdog(void)
{
    CONFIGURE_WATCHDOG();                                                // allow user configuration of internal watch dog timer
    #if defined USER_STARTUP_CODE                                        // {2} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
    #endif
    #if defined _COMPILE_GHS                                             // {4}
    _start_T();                                                          // now call the GHS initialisation code which initialises variables and then calls main() 
    #else
    __iar_program_start();                                               // now call the IAR initialisation code which initialises variables and then calls main() 
    #endif
}
#endif

// The boot loader doesn't use interrupts so these routines are dummy
//
extern void uDisable_Interrupt(void)
{
}

extern void uEnable_Interrupt(void)
{
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

extern void fnDelayLoop(unsigned long ulDelay_us)
{
    volatile int i_us;
    while (ulDelay_us--) {                                               // for each us required
        i_us = (CORE_CLOCK/8300000);
        while (i_us-- != 0) {}                                           // simple loop tuned to perform us timing
    }
}

// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)               // {11}
{
    unsigned char ucValue = (unsigned char)iValue;
    void *buffer = ptrTo;
    unsigned char *ptr = (unsigned char *)ptrTo;

    while (Size-- != 0) {
        *ptr++ = ucValue;
    }

    return buffer;
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


/* =================================================================== */
/*                              GPIO                                   */
/* =================================================================== */
#define _PORT_MUX_CODE
    #include "kinetis_PORTS.h"                                           // {9}
#undef _PORT_MUX_CODE



#if defined SPI_SW_UPLOAD
/* =================================================================== */
/*                        SPI FLASH driver                             */
/* =================================================================== */
    #define _SPI_DEFINES
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
        #include "spi_flash_w25q.h"                                      // {6}
        #include "spi_flash_kinetis_s25fl1-k.h"
    #undef _SPI_DEFINES
    #undef _SPI_FLASH_INTERFACE

// Power up the SPI interface, configure the pins used and select the mode and speed
//
extern int fnConfigSPIFileSystem(void)
{
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
        #include "spi_flash_w25q.h"                                      // {6}
        #include "spi_flash_kinetis_s25fl1-k.h"
    #undef _CHECK_SPI_CHIPS
    if (NO_SPI_FLASH_AVAILABLE == ucSPI_FLASH_Type[0]) {                 // if no SPI Flash detected
        return 1;
    }
    return 0;
}
#endif

#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */
    #include "kinetis_FLASH.h"                                           // include FLASH driver code
#endif

extern int uFileErase(unsigned char *ptrFile, MAX_FILE_LENGTH FileLength)
{
    return fnEraseFlashSector(ptrFile, FileLength);
}

extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
    fnGetParsFile(ParLocation, ptrValue, Size);                          // {7}
}



// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);             // request Cortex core reset, which will cause the software reset bit to be set in the mode controller for recognistion after restart
#if !defined _WINDOWS
    while ((int)1 != (int)0) {}
#endif
}

#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #if defined ARM_MATH_CM0PLUS                                         // {10} cortex-M0+ assembler code
        #if !defined _WINDOWS
    asm(" ldr r1, [r0,#0]");                                             // get the stack pointer value from the program's reset vector
    asm(" mov sp, r1");                                                  // copy the value to the stack pointer
    asm(" ldr r0, [r0,#4]");                                             // get the program counter value from the program's reset vector
    asm(" blx r0");                                                      // jump to the start address
        #endif
    #else                                                                // cortex-M3/M4 assembler code
        #if !defined _WINDOWS
    asm(" ldr sp, [r0,#0]");                                             // load the stack pointer value from the program's reset vector
    asm(" ldr pc, [r0,#4]");                                             // load the program counter value from the program's reset vector to cause operation to continue from there
        #endif
    #endif
}
#endif


#if defined _COMPILE_KEIL
typedef struct stREGION_TABLE
{
    unsigned char *ptrConst;                                             // const variables belonging to block
    unsigned char *ptrDestination;                                       // destination in RAM
    unsigned long  ulLength;                                             // length of the block in SRAM
    unsigned long  ulUnknown;                                            // entry unknown
} REGION_TABLE;

// Calculate the end of used SRAM from the Keil linker information and optionally initialise variables
//
static unsigned char *_keil_ram_size(int iInit)
{
    extern REGION_TABLE Region$$Table$$Base;                            // table location supplied by linker
    extern REGION_TABLE Region$$Table$$Limit;                           // end of table list
    REGION_TABLE *ptrEntries = &Region$$Table$$Base;                    // first block
    unsigned char *ptrRam = ptrEntries->ptrDestination;                 // RAM address
    do {
        if (iInit != 0) {
            if (ptrEntries->ulUnknown == 0x60) {
                uMemset(ptrRam, 0, ptrEntries->ulLength);               // zero data
            }
            else {
                uMemcpy(ptrRam, ptrEntries->ptrConst, ptrEntries->ulLength); // intialise data
            }
        }
        ptrRam += ptrEntries->ulLength;                                 // add length
        ptrEntries++;                                                   // move to next block
    } while (ptrEntries != &Region$$Table$$Limit);
    return ptrRam;
}
#endif

#if defined (_GNU) || defined _CODE_WARRIOR
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

extern void __init_gnu_data(void)
{
    unsigned char *ptrData;
    unsigned long ulInitDataLength;
    #if !defined _RAM_DEBUG
    const unsigned char *ptrFlash = &__data_load_start__;
    ulInitDataLength = (&__data_end__ - &__data_start__);
    ptrData = &__data_start__;
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
    #endif
    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif

// Perform very low level initialisation - called by the start up code
//
#if defined _COMPILE_KEIL || defined _GNU || defined _CODE_WARRIOR
extern int
#else
extern void
#endif
#if defined _WINDOWS
            _LowLevelInit(void)
#else
            main(void)
#endif
{
#if (defined KINETIS_K64 || (defined KINETIS_K24 && (SIZE_OF_FLASH == (1024 * 1024)))) && (defined RUN_FROM_HIRC_PLL || defined RUN_FROM_HIRC_FLL || defined RUN_FROM_HIRC) // {5} older K64 devices require the IRC48M to be switched on by the USB module
    #define IRC48M_TIMEOUT 1000
    int iIRC48M_USB_control = 0;
#endif
#if !defined _COMPILE_IAR
    CONFIGURE_WATCHDOG();                                                // allow user configuration of internal watch dog timer
    #if defined USER_STARTUP_CODE                                        // {2} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
    #endif
#endif
    // Configure clock generator                                         {8}
    //
#if defined KINETIS_KE
    #include "kinetis_KE_CLOCK.h"                                        // KE and KEA clock configuration
#elif defined RUN_FROM_HIRC || defined RUN_FROM_HIRC_FLL || defined RUN_FROM_HIRC_PLL // 48MHz
    #include "kinetis_HIRC.h"                                            // high speed internal clock
#elif defined KINETIS_WITH_MCG_LITE
    #include "kinetis_MCG_LITE.h"                                        // MCG LITE clock configuration
#elif defined KINETIS_KL
    #include "kinetis_KL_CLOCK.h"                                        // KL clock configuration
#elif defined KINETIS_KV
    #include "kinetis_KV_CLOCK.h"                                        // KV clock configuration
#elif defined KINETIS_KW
    #include "kinetis_KW_CLOCK.h"                                        // KW clock configuration
#elif defined KINETIS_KM
    #include "kinetis_KM_CLOCK.h"                                        // KM clock configuration
#else
    #include "kinetis_K_CLOCK.h"                                         // K clock configuration
#endif
#if defined (_GNU) || defined _CODE_WARRIOR
    __init_gnu_data();                                                   // initialise variables
#elif defined _COMPILE_KEIL
    _keil_ram_size(1);                                                   // initialise variables
#endif
#if !defined _WINDOWS
    uTaskerBoot();                                                       // call the boot loader
#endif
#if defined _COMPILE_KEIL || defined _GNU || defined _CODE_WARRIOR
    return 0;
#endif
}


// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined _COMPILE_IAR
__root const RESET_VECTOR __vector_table @ ".intvec"                     // __root forces the function to be linked in IAR project
#elif defined _GNU
const RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _CODE_WARRIOR
#pragma define_section vectortable ".RESET" ".RESET" ".RESET" far_abs R
static __declspec(vectortable) RESET_VECTOR __vect_table
#elif defined _COMPILE_KEIL
__attribute__((section("VECT"))) const RESET_VECTOR reset_vect
#else
    #if defined _COMPILE_GHS                                             // {4}
        #pragma ghs section rodata=".vectors"
    #endif
const RESET_VECTOR __vector_table
#endif
= {
    (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - 4),                       // stack pointer to top of RAM
    (void (*)(void))START_CODE,                                          // start address
};


// Flash configuration - this is linked at address 0x00000400
//
#if defined _COMPILE_IAR
__root const KINETIS_FLASH_CONFIGURATION __flash_config @ ".f_config"    // __root forces the function to be linked in IAR project
#elif defined _GNU
const KINETIS_FLASH_CONFIGURATION __attribute__((section(".f_config"))) __flash_config
#elif defined _CODE_WARRIOR
#pragma define_section flash_cfg ".FCONFIG" ".FCONFIG" ".FCONFIG" far_abs R
static __declspec(flash_cfg) KINETIS_FLASH_CONFIGURATION __flash_config
#elif defined _COMPILE_KEIL
__attribute__((section("F_INIT"))) const KINETIS_FLASH_CONFIGURATION __flash_config
#else
    #if defined _COMPILE_GHS                                             // {4}
        #pragma ghs section rodata=".f_config"
    #endif
const KINETIS_FLASH_CONFIGURATION __flash_config
#endif
= {
    KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY,
    KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION,
    KINETIS_FLASH_CONFIGURATION_SECURITY,
    KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION,
    KINETIS_FLASH_CONFIGURATION_EEPROM_PROT,
    KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT
};













#if defined _WINDOWS
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

extern CHAR *fnBufferDec(signed long slNumberToConvert, unsigned char ucStyle, CHAR *ptrBuf)
{
    return ptrBuf;
}

extern CHAR *uStrcpy(CHAR *ptrTo, const CHAR *ptrFrom)
{
    return ptrTo;
}


extern unsigned char *fnGetTxBufferAdd(int iBufNr) { return 0;}

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
#if defined _WINDOWS
    #define PORTA_DEFAULT_INPUT        0xffffffff
    #define PORTB_DEFAULT_INPUT        0xffffffff

    unsigned long ulPortPullups[] = {
        PORTA_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_sam7x.h
        PORTB_DEFAULT_INPUT
    };

    fnInitialiseDevice((void *)ulPortPullups);
    _LowLevelInit();
#endif
#if defined _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif
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

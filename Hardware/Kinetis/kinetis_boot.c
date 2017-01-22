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
    UNLOCK_WDOG();                                                       // enable watchdog modification
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
        while (i_us--) {}                                                // simple loop tuned to perform us timing
    }
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


#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE
/* =================================================================== */
/*                           FLASH driver                              */
/* =================================================================== */
    #include "kinetis_FLASH.h"                                           // include FLASH driver code
#endif

#if defined SPI_SW_UPLOAD
    #if defined SPI_FLASH_SST25
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
{
    ucDestination -= (SPI_FLASH_START);                                  // location relative to the start of the SPI FLASH chip address
    return ucDestination;
}
    #else
// Return the page number and optionally the address offset in the page
//
static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
{
    unsigned short usPageNumber;
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
    #endif

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
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
    #undef _CHECK_SPI_CHIPS
    if (NO_SPI_FLASH_AVAILABLE == ucSPI_FLASH_Type[0]) {                 // if no SPI Flash detected
        return 1;
    }
    return 0;
}
#endif

extern int uFileErase(unsigned char *ptrFile, MAX_FILE_LENGTH FileLength)
{
    return fnEraseFlashSector(ptrFile, FileLength);
}

extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#if defined SPI_SW_UPLOAD
    if (ParLocation >= uFILE_SYSTEM_END) {                               // get from SPI FLASH
    #if defined SPI_FLASH_SST25
        ParLocation = fnGetSPI_FLASH_address(ParLocation);
    #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset);
    #endif

    #if defined SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), ptrValue, Size);
    #elif defined SPI_FLASH_SST25
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, ptrValue, Size);
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
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the processor uses a file system in FLASH with no access restrictions so we can simply copy the data
}



// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);             // request Cortex core reset, which will cause the software reset bit to be set in the mode controller for recognistion after restart
#if !defined _WINDOWS
    while (1) {}
#endif
}

#if !defined _COMPILE_KEIL                                               // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #if !defined _WINDOWS
    asm(" ldr sp, [r0,#0]");
    asm(" ldr pc, [r0,#4]");
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
    UNLOCK_WDOG();                                                       // enable watchdog modification
    CONFIGURE_WATCHDOG();                                                // allow user configuration of internal watch dog timer
    #if defined USER_STARTUP_CODE                                        // {2} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
    #endif
#endif
#if defined RUN_FROM_HIRC || defined RUN_FROM_HIRC_FLL || defined RUN_FROM_HIRC_PLL // 48MHz {5}
    #if !defined KINETIS_K64 && defined SUPPORT_RTC && !defined RTC_USES_RTC_CLKIN && !defined RTC_USES_LPO_1kHz
    MCG_C2 = MCG_C2_EREFS;                                               // request oscillator
    OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);                       // enable the external reference clock and keep it enabled in stop mode
    #endif
  //MCG_MC = MCG_MC_HIRCEN;                                              // this is optional and would allow the HIRC to run even when the processor is not working in HIRC mode
    #if defined MCG_C1_CLKS_HIRC
    MCG_C1 = MCG_C1_CLKS_HIRC;                                           // select HIRC clock source
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_HICR) {             // wait until the source is selected
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_HICR;
        #endif
    }
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    #else
    MCG_C7 = MCG_C7_OSCSEL_IRC48MCLK;                                    // route the IRC48M clock to the external reference clock input (this enables IRC48M)
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C1 = (MCG_C1_IREFS | MCG_C1_CLKS_EXTERN_CLK);                    // switch IRC48M reference to MCGOUTCLK
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // wait until the new source is valid (move to FBI using IRC48M external source is complete)
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
        #endif
        #if (defined KINETIS_K64 || (defined KINETIS_K24 && (SIZE_OF_FLASH == (1024 * 1024)))) // older K64 devices require the IRC48M to be switched on by the USB module
        if (++iIRC48M_USB_control >= IRC48M_TIMEOUT) {                   // if the switch-over is taking too long it means that the clock needs to be enabled in the USB controller
            POWER_UP(4, SIM_SCGC4_USBOTG);                               // power up the USB controller module
            USB_CLK_RECOVER_IRC_EN = (USB_CLK_RECOVER_IRC_EN_REG_EN | USB_CLK_RECOVER_IRC_EN_IRC_EN); // the IRC48M is only usable when enabled via the USB module
        }
        #endif
    }
        #if defined RUN_FROM_HIRC_FLL
    MCG_C2 = (MCG_C2_IRCS | MCG_C2_RANGE_8M_32M);                        // select a high frquency range values so that the FLL input divide range is increased
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1280);                  // switch FLL input to the external clock source with correct divide value, and select the FLL output for MCGOUTCLK
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_FLL) {              // wait for the output to be set
        #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_FLL;
        #endif
    }
    MCG_C4 = ((MCG_C4 & ~(MCG_C4_DMX32 | MCG_C4_HIGH_RANGE)) | (_FLL_VALUE)); // adjust FLL factor to obtain the required operating frequency
        #elif defined RUN_FROM_HIRC_PLL                                  // we are presently running directly from the IRC48MCLK and have also determined whether a K64 is an older or newer device (with IRC48M independent from the USB module)
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1280);               // switch the external clock source also to the FLL to satisfy the PBE state requirement
    MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                        // PLL remains enabled in normal stop modes
    MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);          // complete PLL configuration and move to PBE
    while ((MCG_S & MCG_S_PLLST) == 0) {                                 // loop until the PLLS clock source becomes valid
            #if defined _WINDOWS
        MCG_S |= MCG_S_PLLST;
            #endif
    }
    while ((MCG_S & MCG_S_LOCK) == 0) {                                  // loop until PLL locks
            #if defined _WINDOWS
        MCG_S |= MCG_S_LOCK;
            #endif
    }
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
            #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
            #endif
    }
        #else
    MCG_C2 |= MCG_C2_LP;                                                 // set bypass to disable FLL and complete move to BLPE (in which PLL is also always disabled)
        #endif
    #endif
#else
    #if defined EXTERNAL_CLOCK                                           // first move from state FEI to state FBE (presently running from about 25MHz internal clock)
    MCG_C2 = (MCG_C2_RANGE_8M_32M);                                      // don't use oscillator
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1024);               // switch to external input clock (the FLL input clock is set to as close to its input range as possible, although this is not necessary since the FLL will not be used)
    #else
        #if CRYSTAL_FREQUENCY > 8000000
    MCG_C2 = (MCG_C2_RANGE_8M_32M | MCG_C2_HGO | MCG_C2_EREFS);          // select crystal oscillator
        #elif CRYSTAL_FREQUENCY >= 1000000
    MCG_C2 = (MCG_C2_RANGE_1M_8M | MCG_C2_HGO | MCG_C2_EREFS);           // select crystal oscillator
        #else                                                                // assumed to be 32kHz crystal
    MCG_C2 = (MCG_C2_RANGE_32K_40K | MCG_C2_HGO | MCG_C2_EREFS);         // select crystal oscillator
        #endif
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_256);                // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary since the FLL will not be used) this is accurate for 8MHz clock but hasn't been tested for other values
    while (!(MCG_S & MCG_S_OSCINIT)) {                                   // loop until the crystal source has been selected
        #if defined _WINDOWS
        MCG_S |= MCG_S_OSCINIT;
        #endif
    }
    #endif
    while (MCG_S & MCG_S_IREFST) {                                       // loop until the FLL source is no longer the internal reference clock
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_IREFST;
    #endif
    }
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // loop until the external reference clock source is valid
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
    #endif
    }    
    MCG_C5 = (CLOCK_DIV - 1);                                            // now move from state FBE to state PBE
    MCG_C6 = ((CLOCK_MUL - 24) | MCG_C6_PLLS);
    while ((MCG_S & MCG_S_PLLST) == 0) {                                 // loop until the PLLS clock source becomes valid
    #if defined _WINDOWS
        MCG_S |= MCG_S_PLLST;
    #endif
    }
    while ((MCG_S & MCG_S_LOCK) == 0) {                                  // loop until PLL locks
    #if defined _WINDOWS
        MCG_S |= MCG_S_LOCK;
    #endif
    }
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
    #if defined _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
    #endif
    }   
#endif
#if defined (_GNU) || defined _CODE_WARRIOR
    __init_gnu_data();                                                   // initialise variables
#elif defined _COMPILE_KEIL
    _keil_ram_size(1);                                                   // initialise variables
#endif
#if defined _WINDOWS                                                     // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the vector table size setting!!");
    }
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

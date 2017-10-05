/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      modbus_app.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    Modbus slave loader configuration

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#if defined USE_MODBUS
/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define TEST_QUEUE                                                     // activate to test master queues
//#define TEST_DELAYED_RESPONSE                                          // activate to test delayed responses

#define MODBUS_PAR_BLOCK_VERSION   0

#define ADDRESS_FIRMWARE_UPLOAD    0                                     // holding register address 0



/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int           fnMODBUSPostFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function);


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


static const MODBUS_PARS cMODBUS_default = {
    MODBUS_PAR_BLOCK_VERSION,
#if defined MODBUS_SERIAL_INTERFACES && defined SERIAL_INTERFACE
    {
        (1),                                                             // slave address
    },
    {
        (CHAR_8 + NO_PARITY + ONE_STOP + CHAR_MODE),                     // serial interface settings
    },
    {
        SERIAL_BAUD_115200,                                              // baud rate of serial interface
    },
    {
        (MODBUS_MODE_RTU | MODBUS_SERIAL_SLAVE | MODBUS_RS485_POSITIVE), // RTU mode as slave
    },
    #if defined MODBUS_ASCII
    {
        (DELAY_LIMIT)(2 * SEC),                                          // inter-character delays greater than 2s are considered errors in ASCII mode
    },
    {
        0x0a,                                                            // ASCII mode line feed character
    },
    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && !defined NO_SLAVE_MODBUS_REPORT_SLAVE_ID
    { 'u', 'T', 'a', 's', 'k', 'e', 'r', '-', 'M', 'O', 'D', 'B', 'U', 'S', '-', 's', 'l','a','v','e' },
    #endif
#endif
};


#define INPUT_REGS_START      0                                          // start address
#if SIZE_OF_FLASH > (128 * 1024)
    #define INPUT_REGS_END        (64 * 1024)                            // end address
#else
    #define INPUT_REGS_END        (SIZE_OF_FLASH/2)                      // end address
#endif
#define INPUT_REGS_QUANTITY   (INPUT_REGS_END - INPUT_REGS_START)       // quantity
static MODBUS_REGISTERS test_input_regs = { FLASH_START_ADDRESS, {INPUT_REGS_START, INPUT_REGS_END}};

#define HOLDING_REGS_START    0                                          // start address
#define HOLDING_REGS_END      65                                         // end address
#define HOLDING_REGS_QUANTITY ((HOLDING_REGS_END - HOLDING_REGS_START) + 1) // quantity
static unsigned short firmware_load[65 + 1];
static MODBUS_REGISTERS test_holding_regs = { firmware_load, {HOLDING_REGS_START, HOLDING_REGS_END}};

static const MODBUS_CONFIG modbus_configuration = {
    0,                                                                   // read-only discrete input configuration
    0,                                                                   // read/write coil configuration
    &test_input_regs,                                                    // read-only input registers
    &test_holding_regs,                                                  // read/write input registers
};


static const MODBUS_CALLBACKS modbus_slave_callbacks = {
    0,
    fnMODBUSPostFunction,
    0,
};



/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

MODBUS_PARS *ptrMODBUS_pars = 0;

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    static unsigned char ucCodeStart[FLASH_ROW_SIZE];
#else
    static unsigned char ucCodeStart[4] = {0xff, 0xff, 0xff, 0xff};
#endif
#if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
    static FILE_OBJECT_INFO fileObjInfo = {0};
#endif


// This routine is called by the MODBUS interface after a write to the MODBUS table
// It can be used to respond to table changes
//
static int fnMODBUSPostFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function)
{
    static MEMORY_RANGE_POINTER ptrFirmware = 0;
    unsigned short usAddress = modbus_rx_function->usElementAddress;
    unsigned short usLength = modbus_rx_function->usElementLength;
    int iCodeStart = 0;
    switch (iType) {
    case UPDATE__HOLDING_REGISTERS:                                      // holding registers have been altered - react to changes if necessary
        if ((usAddress == ADDRESS_FIRMWARE_UPLOAD) && (usLength == 65)) {// a block of data has been received for new firmware (must always be a write starting at this register and 65 registers in length)
            static unsigned short usContentCounter = 0xffff;
            unsigned char ucContentLength = (unsigned char)(firmware_load[0]);
            unsigned char ucThisContentCounter = (unsigned char)(firmware_load[0] >> 8);
            if (usContentCounter != (unsigned short)ucThisContentCounter) { // ignore if the counter were to repeat
                int iSwap;
                unsigned short usSwapped;
                usContentCounter = ucThisContentCounter;                 // synchronise to new content counter value
                if (ptrFirmware == 0) {                                  // the first write
                    fnEraseFlashSector((unsigned char *)UTASKER_APP_START, (UTASKER_APP_END - (unsigned char *)UTASKER_APP_START)); // erase the application
                    ptrFirmware = (unsigned char *)_UTASKER_APP_START_;
                    iCodeStart = 1;
                }
                if (ucContentLength > (64 * sizeof(unsigned short))) {
                    ucContentLength = (64 * sizeof(unsigned short));
                }
                for (iSwap = 1; iSwap < 65; iSwap++) {
                    usSwapped = firmware_load[iSwap];
                    usSwapped = ((usSwapped >> 8) | (usSwapped << 8));
                    firmware_load[iSwap] = usSwapped;
                }
                if (iCodeStart != 0) {                                   // first block of code to be programmed
                    uMemcpy(ucCodeStart, firmware_load, sizeof(ucCodeStart)); // save the start to an intermediate buffer (this will be programmed at end)
                    fnWriteBytesFlash((ptrFirmware + sizeof(ucCodeStart)), (unsigned char *)&firmware_load[1 + sizeof(ucCodeStart)], (ucContentLength - sizeof(ucCodeStart))); // program to Flash
                }
                else {
                    fnWriteBytesFlash(ptrFirmware, (unsigned char *)&firmware_load[1], ucContentLength); // program to Flash
                }
                ptrFirmware += ucContentLength;
                firmware_load[65] = usContentCounter;                    // count valid frames
                if (ucContentLength < (64 * sizeof(unsigned short))) {   // last block has less valid content
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
                    fnWriteBytesFlash(ptrFirmware, 0, 0);                // close any outstanding FLASH buffer from end of the file
    #endif
    #if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
                    fileObjInfo.ptrLastAddress = ptrFirmware;            // save information about the length of data written
    #endif
                    ptrFirmware = (unsigned char *)_UTASKER_APP_START_;
                    fnWriteBytesFlash(ptrFirmware, ucCodeStart, sizeof(ucCodeStart)); // program start of code to Flash
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
                    fnWriteBytesFlash(ptrFirmware, 0, 0);                // close any outstanding FLASH buffer from write to start of code
    #endif
                    firmware_load[65] = 0x5555;                          // mark that the upload has completed and a reset will be performed
                    uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(1 * SEC), T_RESET); // reset after a short delay to acknowledge the final write
                }
            }
        }
        else {
            firmware_load[65] = 0xaa01;                                  // mark that the write was invalid as upload write
        }
    }
    return 0;
}


extern void fnInitModbus(void)
{
    ptrMODBUS_pars = (MODBUS_PARS *)&cMODBUS_default;
#if defined _WINDOWS
    test_input_regs.ptrRegisters = (unsigned short *)fnGetFlashAdd((unsigned char *)FLASH_START_ADDRESS);
#endif
    fnInitialiseMODBUS_port(0, &modbus_configuration, &modbus_slave_callbacks, 0); // port 0
}
#endif

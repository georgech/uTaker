/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      LCD.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    09.10.2007 Add configurable delay to ensure two back to back writes are not too fast in 4 bit mode {1}
    10.10.2007 Correct nibble adjustment in 4-bit read                   {2}
    12.10.2007 T_INIT_WAIT modified to 5ms rather than 1 tick Ticks of greater will default to 1 tick) {3}
    14.04.2007 Add optional command before E_LCD_READ_RAM                {4}
    27.02.2009 Adapt for Luminary-Micro (DDR must be changed before setting driving value) {5}
    02.08.2010 Add LCD contrast and backlight configurations             {6}
    03.08.2011 Add simulated LCD read support                            {7}
    15.10.2011 Decide on port access style based on processor type       {8}
    25.11.2017 Add PWM interface option                                  {9}
    25.11.2017 Modify fnSetContrast() to fnSetLCDContrast() and make external {10}
    11.03.2019 Add AVAGO_HCMS_CHAR_LCD mode                              {11}

*/        

#include "config.h"



#if defined SUPPORT_LCD                                                  // activate this define to support LCD


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */


#define OWN_TASK                 TASK_LCD


#define T_INIT_WAIT               (DELAY_LIMIT)(0.005*SEC)               // > 4,1ms (smallest value will be 1 TICK)

#define E_INIT_CONTINUE           1

#define STATE_INIT                0
#define STATE_INITIALISING        1
#define WRITE_FUNCTION_SET        2
#define STATE_INIT_2              3
#define STATE_INIT_3              4
#define STATE_INIT_4              5

#define STATE_DISPLAY_INITIALISED 6
#define STATE_LCD_SENDING         7
#define STATE_LCD_READY           8


#define DL_BIT 0x10                                                      // data length 8 bits
#define N_BIT  0x8                                                       // 2 line mode 
#define F_BIT  0                                                         // 5 x 7 dot font
#define I_BIT  0x2                                                       // curser increment 
#define S_BIT  0                                                         // display doesn't shift

#define LCD_BUSY 0x80


#if defined _HW_SAM7X || defined _HW_AVR32 || defined _LPC23XX || defined _LPC17XX || defined _STM32 || defined _KINETIS
    #define _ACCESSTYPE1                                                 // define access type best suited to these processor types
#endif
#if defined _HW_SAM7X || defined _HW_AVR32 || defined _LPC23XX || defined _LPC17XX || defined _KINETIS
    #define _ACCESSTYPE2                                                 // define access type best suited to these processor types
#endif
#if defined _LM3SXXXX || defined _STM32
    #define _ACCESSTYPE3                                                 // define access type best suited to these processor types
#endif

#if defined AVAGO_HCMS_CHAR_LCD                                          // {11}
    #define AVAGO_HCMS_CONTROL_WORD_0_SLEEP_MODE              0x00
    #define AVAGO_HCMS_CONTROL_WORD_0_NORMAL_OPERATION        0x40
    #define AVAGO_HCMS_CONTROL_WORD_0_PEAK_CURRENT_4mA        0x20
    #define AVAGO_HCMS_CONTROL_WORD_0_PEAK_CURRENT_6_4mA      0x10
    #define AVAGO_HCMS_CONTROL_WORD_0_PEAK_CURRENT_9_3mA      0x00
    #define AVAGO_HCMS_CONTROL_WORD_0_PEAK_CURRENT_12_8mA     0x30
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_0_PERCENT    0x00
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_1_7_PERCENT  0x01
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_3_3_PERCENT  0x02
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_5_PERCENT    0x03
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_6_7_PERCENT  0x04
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_8_3_PERCENT  0x05
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_11_7_PERCENT 0x06
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_15_PERCENT   0x07
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_18_PERCENT   0x08
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_23_PERCENT   0x09
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_30_PERCENT   0x0a
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_37_PERCENT   0x0b
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_47_PERCENT   0x0c
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_60_PERCENT   0x0d
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_80_PERCENT   0x0e
    #define AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_100_PERCENT  0x0f

    #define AVAGO_HCMS_CONTROL_WORD_1_SERIAL_MODE             0x00
    #define AVAGO_HCMS_CONTROL_WORD_1_SIMULTANEOUS_MODE       0x01
    #define AVAGO_HCMS_CONTROL_WORD_1_OSCILLATOR_FREQ_1       0x00
    #define AVAGO_HCMS_CONTROL_WORD_1_OSCILLATOR_FREQ_16      0x02

    #define BITS_PER_HCMS_CHARACTER          (5 * 8)
    #define BYTES_PER_HCMS_CHARACTER         (BITS_PER_HCMS_CHARACTER / 8)

    #define HCMS_CLEAR_DISPLAY  0
    #define HCMS_SET_HOME       1
    #define HCMS_SHIFT_RIGHT    2
    #define HCMS_SHIFT_LEFT     3
#endif


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int fnInitDisplay(int iState);
static void fnWriteLine(unsigned char *pMsg, unsigned char ucMsgLen, LCD_CONTROL_PORT_SIZE Mode);
static unsigned char fnReadDisplay(LCD_CONTROL_PORT_SIZE Rs);
static void fnSendAppRead(unsigned char ucData);
static void fnSecureCommands(unsigned char *ucPtr, unsigned char ucLen);
static int fnSendDisplay(void);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined AVAGO_HCMS_CHAR_LCD                                          // {11} 5 x 7 fonts for alphanumeric display
static const unsigned char ucFont[256][5] = {
    #if defined LCD_CYRILLIC_FONT
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x4f, 0x00, 0x00 },
    { 0x00, 0x07, 0x00, 0x07, 0x00 },
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },
    { 0x23, 0x13, 0x08, 0x64, 0x62 },
    { 0x36, 0x49, 0x55, 0x22, 0x50 },
    { 0x00, 0x05, 0x03, 0x00, 0x00 },
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },
    { 0x14, 0x08, 0x3e, 0x08, 0x14 },
    { 0x08, 0x08, 0x3e, 0x08, 0x08 },
    { 0x00, 0x50, 0x30, 0x00, 0x00 },
    { 0x08, 0x08, 0x08, 0x08, 0x08 },
    { 0x00, 0x60, 0x60, 0x00, 0x00 },
    { 0x20, 0x10, 0x08, 0x04, 0x02 },
    { 0x3e, 0x51, 0x49, 0x45, 0x3e },
    { 0x00, 0x42, 0x7f, 0x40, 0x00 },
    { 0x42, 0x61, 0x51, 0x49, 0x46 },
    { 0x21, 0x41, 0x45, 0x4b, 0x31 },
    { 0x18, 0x14, 0x12, 0x7f, 0x10 },
    { 0x27, 0x45, 0x45, 0x45, 0x39 },
    { 0x3c, 0x4a, 0x49, 0x49, 0x30 },
    { 0x01, 0x71, 0x09, 0x05, 0x03 },
    { 0x36, 0x49, 0x49, 0x49, 0x36 },
    { 0x06, 0x49, 0x49, 0x29, 0x1e },
    { 0x00, 0x36, 0x36, 0x00, 0x00 },
    { 0x00, 0x56, 0x36, 0x00, 0x00 },
    { 0x08, 0x14, 0x22, 0x41, 0x00 },
    { 0x14, 0x14, 0x14, 0x14, 0x14 },
    { 0x00, 0x41, 0x22, 0x14, 0x08 },
    { 0x02, 0x01, 0x51, 0x09, 0x06 },
    { 0x32, 0x49, 0x79, 0x41, 0x3e },
    { 0x7e, 0x09, 0x09, 0x09, 0x7e },
    { 0x7f, 0x49, 0x49, 0x49, 0x36 },
    { 0x3e, 0x41, 0x41, 0x41, 0x22 },
    { 0x7f, 0x41, 0x41, 0x22, 0x1c },
    { 0x7f, 0x49, 0x49, 0x49, 0x41 },
    { 0x7f, 0x09, 0x09, 0x09, 0x01 },
    { 0x3e, 0x41, 0x49, 0x49, 0x7a },
    { 0x7f, 0x08, 0x08, 0x08, 0x7f },
    { 0x00, 0x41, 0x7f, 0x41, 0x00 },
    { 0x20, 0x40, 0x41, 0x3f, 0x01 },
    { 0x7f, 0x08, 0x14, 0x22, 0x41 },
    { 0x7f, 0x40, 0x40, 0x40, 0x40 },
    { 0x7f, 0x02, 0x0c, 0x02, 0x7f },
    { 0x7f, 0x04, 0x08, 0x10, 0x7f },
    { 0x3e, 0x41, 0x41, 0x41, 0x3e },
    { 0x7f, 0x09, 0x09, 0x09, 0x06 },
    { 0x3e, 0x41, 0x51, 0x21, 0x5e },
    { 0x7f, 0x09, 0x19, 0x29, 0x46 },
    { 0x46, 0x49, 0x49, 0x49, 0x31 },
    { 0x01, 0x01, 0x7f, 0x01, 0x01 },
    { 0x3f, 0x40, 0x40, 0x40, 0x3f },
    { 0x1f, 0x20, 0x40, 0x20, 0x1f },
    { 0x3f, 0x40, 0x38, 0x40, 0x3f },
    { 0x63, 0x14, 0x08, 0x14, 0x63 },
    { 0x07, 0x08, 0x70, 0x08, 0x07 },
    { 0x61, 0x51, 0x49, 0x45, 0x43 },
    { 0x00, 0x7f, 0x41, 0x41, 0x00 },
    { 0x15, 0x16, 0x7c, 0x16, 0x15 },
    { 0x00, 0x41, 0x41, 0x7f, 0x00 },
    { 0x04, 0x02, 0x01, 0x02, 0x04 },
    { 0x40, 0x40, 0x40, 0x40, 0x40 },
    { 0x00, 0x01, 0x02, 0x04, 0x00 },
    { 0x20, 0x54, 0x54, 0x54, 0x78 },
    { 0x7f, 0x48, 0x44, 0x44, 0x38 },
    { 0x38, 0x44, 0x44, 0x44, 0x00 },
    { 0x38, 0x44, 0x44, 0x48, 0x7f },
    { 0x38, 0x54, 0x54, 0x54, 0x18 },
    { 0x08, 0x7e, 0x09, 0x01, 0x02 },
    { 0x0c, 0x52, 0x52, 0x52, 0x3e },
    { 0x7f, 0x08, 0x04, 0x04, 0x78 },
    { 0x00, 0x44, 0x7d, 0x40, 0x00 },
    { 0x20, 0x40, 0x44, 0x3d, 0x00 },
    { 0x7f, 0x10, 0x28, 0x44, 0x00 },
    { 0x00, 0x41, 0x7f, 0x40, 0x00 },
    { 0x7c, 0x04, 0x18, 0x04, 0x78 },
    { 0x7c, 0x08, 0x04, 0x04, 0x78 },
    { 0x38, 0x44, 0x44, 0x44, 0x38 },
    { 0x7c, 0x14, 0x14, 0x14, 0x08 },
    { 0x08, 0x14, 0x14, 0x18, 0x7c },
    { 0x7c, 0x08, 0x04, 0x04, 0x08 },
    { 0x48, 0x54, 0x54, 0x54, 0x20 },
    { 0x04, 0x3f, 0x44, 0x40, 0x20 },
    { 0x3c, 0x40, 0x40, 0x20, 0x7c },
    { 0x1c, 0x20, 0x40, 0x20, 0x1c },
    { 0x3c, 0x40, 0x30, 0x40, 0x3c },
    { 0x44, 0x28, 0x10, 0x28, 0x44 },
    { 0x0c, 0x50, 0x50, 0x50, 0x3c },
    { 0x44, 0x64, 0x54, 0x4c, 0x44 },
    { 0x00, 0x08, 0x36, 0x41, 0x00 },
    { 0x00, 0x00, 0x7f, 0x00, 0x00 },
    { 0x00, 0x41, 0x36, 0x08, 0x00 },
    { 0x08, 0x08, 0x2a, 0x1c, 0x08 },
    { 0x08, 0x1c, 0x2a, 0x08, 0x08 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x7f, 0x49, 0x49, 0x49, 0x33 },
    { 0x7f, 0x01, 0x01, 0x01, 0x03 },
    { 0x7c, 0x55, 0x54, 0x45, 0x00 },
    { 0x77, 0x08, 0x7f, 0x08, 0x77 },
    { 0x41, 0x49, 0x49, 0x49, 0x36 },
    { 0x7f, 0x10, 0x08, 0x04, 0x7f },
    { 0x7c, 0x21, 0x12, 0x09, 0x7c },
    { 0x20, 0x41, 0x3f, 0x01, 0x7f },
    { 0x7f, 0x01, 0x01, 0x01, 0x7f },
    { 0x47, 0x28, 0x10, 0x08, 0x07 },
    { 0x1c, 0x22, 0x7f, 0x22, 0x1c },
    { 0x07, 0x08, 0x08, 0x08, 0x7f },
    { 0x7f, 0x40, 0x7f, 0x40, 0x7f },
    { 0x01, 0x7f, 0x48, 0x48, 0x30 },
    { 0x7f, 0x48, 0x30, 0x00, 0x7f },
    { 0x22, 0x41, 0x49, 0x49, 0x3e },
    { 0x7f, 0x08, 0x3e, 0x41, 0x3e },
    { 0x46, 0x29, 0x19, 0x09, 0x7f },
    { 0x3c, 0x4a, 0x4a, 0x49, 0x31 },
    { 0x7c, 0x54, 0x54, 0x28, 0x00 },
    { 0x7c, 0x04, 0x04, 0x04, 0x0c },
    { 0x38, 0x55, 0x54, 0x55, 0x18 },
    { 0x6c, 0x10, 0x7c, 0x10, 0x6c },
    { 0x44, 0x54, 0x54, 0x54, 0x28 },
    { 0x7c, 0x20, 0x10, 0x08, 0x7c },
    { 0x78, 0x42, 0x24, 0x12, 0x78 },
    { 0x7c, 0x10, 0x28, 0x44, 0x00 },
    { 0x20, 0x44, 0x3c, 0x04, 0x7c },
    { 0x7c, 0x08, 0x10, 0x08, 0x7c },
    { 0x7c, 0x10, 0x10, 0x10, 0x7c },
    { 0x7c, 0x04, 0x04, 0x04, 0x7c },
    { 0x04, 0x04, 0x7c, 0x04, 0x04 },
    { 0x0c, 0x10, 0x10, 0x10, 0x7c },
    { 0x7c, 0x40, 0x7c, 0x40, 0x7c },
    { 0x04, 0x7c, 0x50, 0x50, 0x20 },
    { 0x7c, 0x50, 0x20, 0x00, 0x7c },
    { 0x7c, 0x50, 0x50, 0x20, 0x00 },
    { 0x28, 0x44, 0x54, 0x54, 0x38 },
    { 0x7c, 0x10, 0x38, 0x44, 0x38 },
    { 0x08, 0x54, 0x34, 0x14, 0x7c },
    { 0x10, 0x28, 0x44, 0x10, 0x28 },
    { 0x28, 0x10, 0x44, 0x28, 0x10 },
    { 0x60, 0x50, 0x00, 0x60, 0x50 },
    { 0x05, 0x03, 0x00, 0x05, 0x03 },
    { 0x0f, 0x02, 0x74, 0x5f, 0x70 },
    { 0x30, 0x48, 0x45, 0x40, 0x20 },
    { 0x40, 0x48, 0x3e, 0x09, 0x01 },
    { 0x08, 0x7e, 0x49, 0x4a, 0x48 },
    { 0x00, 0x00, 0x60, 0x00, 0x00 },
    { 0x00, 0x00, 0x70, 0x00, 0x00 },
    { 0x00, 0x5c, 0x40, 0x5c, 0x00 },
    { 0x00, 0x5c, 0x00, 0x5c, 0x00 },
    { 0x45, 0x24, 0x15, 0x08, 0x04 },
    { 0x22, 0x36, 0x1c, 0x36, 0x22 },
    { 0x45, 0x22, 0x15, 0x08, 0x04 },
    { 0x00, 0x21, 0x3f, 0x21, 0x00 },
    { 0x21, 0x3f, 0x21, 0x3f, 0x21 },
    { 0x04, 0x02, 0x7f, 0x02, 0x04 },
    { 0x10, 0x20, 0x7f, 0x20, 0x10 },
    { 0x7f, 0x08, 0x1c, 0x3e, 0x08 },
    { 0x08, 0x3e, 0x1c, 0x08, 0x7f },
    { 0x7f, 0x08, 0x1c, 0x08, 0x3e },
    { 0x47, 0x22, 0x12, 0x08, 0x04 },
    { 0x00, 0x00, 0x00, 0x0c, 0x0c },
    { 0x60, 0x51, 0x4f, 0x41, 0x7e },
    { 0x7f, 0x40, 0x40, 0x40, 0x7f },
    { 0x7f, 0x40, 0x7f, 0x40, 0x7f },
    { 0x60, 0x54, 0x4c, 0x44, 0x7c },
    { 0x30, 0x48, 0x7e, 0x48, 0x30 },
    { 0x7c, 0x40, 0x40, 0x40, 0x7c },
    { 0x7c, 0x40, 0x7c, 0x40, 0x7c },
    { 0x00, 0x00, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x00, 0x01, 0x00 },
    { 0x02, 0x01, 0x02, 0x02, 0x01 },
    { 0x38, 0x54, 0x56, 0x55, 0x18 },
    { 0x18, 0x24, 0x64, 0x24, 0x10 },
    { 0x00, 0x5d, 0x40, 0x3d, 0x00 },
    { 0x20, 0x3c, 0x7e, 0x3c, 0x20 },
    { 0x08, 0x22, 0x00, 0x22, 0x08 },
    { 0x1c, 0x22, 0x22, 0x22, 0x1c },
    { 0x17, 0x08, 0x14, 0x1a, 0x7d },
    { 0x17, 0x08, 0x44, 0x56, 0x7d },
    { 0x17, 0x08, 0x64, 0x56, 0x5d },
    { 0x15, 0x1f, 0x20, 0x30, 0x78 },
    { 0x7f, 0x2a, 0x2a, 0x2a, 0x7f },
    { 0x01, 0x11, 0x10, 0x04, 0x04 },
    { 0x4e, 0x45, 0x6e, 0x40, 0x78 },
    { 0x00, 0x7f, 0x2a, 0x2a, 0x08 },
    { 0x08, 0x2a, 0x7f, 0x2a, 0x08 },
    { 0x08, 0x2a, 0x2a, 0x40, 0x2a },
    { 0x08, 0x2a, 0x2a, 0x7f, 0x00 },
    { 0x08, 0x2a, 0x7f, 0x2a, 0x08 },
    { 0x22, 0x14, 0x08, 0x7f, 0x00 },
    { 0x40, 0x1a, 0x25, 0x59, 0x02 },
    { 0x06, 0x09, 0x7f, 0x09, 0x7f },
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f }
    #else
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x7f, 0x2b, 0x55, 0x2b, 0x7f },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x4f, 0x00, 0x00 },
    { 0x00, 0x07, 0x00, 0x07, 0x00 },
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },
    { 0x23, 0x13, 0x08, 0x64, 0x62 },
    { 0x36, 0x49, 0x55, 0x22, 0x50 },
    { 0x00, 0x05, 0x03, 0x00, 0x00 },
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },
    { 0x14, 0x08, 0x3e, 0x08, 0x14 },
    { 0x08, 0x08, 0x3e, 0x08, 0x08 },
    { 0x00, 0x50, 0x30, 0x00, 0x00 },
    { 0x08, 0x08, 0x08, 0x08, 0x08 },
    { 0x00, 0x60, 0x60, 0x00, 0x00 },
    { 0x20, 0x10, 0x08, 0x04, 0x02 },
    { 0x3e, 0x51, 0x49, 0x45, 0x3e },
    { 0x00, 0x42, 0x7f, 0x40, 0x00 },
    { 0x42, 0x61, 0x51, 0x49, 0x46 },
    { 0x21, 0x41, 0x45, 0x4b, 0x31 },
    { 0x18, 0x14, 0x12, 0x7f, 0x10 },
    { 0x27, 0x45, 0x45, 0x45, 0x39 },
    { 0x3c, 0x4a, 0x49, 0x49, 0x30 },
    { 0x01, 0x71, 0x09, 0x05, 0x03 },
    { 0x36, 0x49, 0x49, 0x49, 0x36 },
    { 0x06, 0x49, 0x49, 0x29, 0x1e },
    { 0x00, 0x36, 0x36, 0x00, 0x00 },
    { 0x00, 0x56, 0x36, 0x00, 0x00 },
    { 0x08, 0x14, 0x22, 0x41, 0x00 },
    { 0x14, 0x14, 0x14, 0x14, 0x14 },
    { 0x00, 0x41, 0x22, 0x14, 0x08 },
    { 0x02, 0x01, 0x51, 0x09, 0x06 },
    { 0x32, 0x49, 0x79, 0x41, 0x3e },
    { 0x7e, 0x09, 0x09, 0x09, 0x7e },
    { 0x7f, 0x49, 0x49, 0x49, 0x36 },
    { 0x3e, 0x41, 0x41, 0x41, 0x22 },
    { 0x7f, 0x41, 0x41, 0x22, 0x1c },
    { 0x7f, 0x49, 0x49, 0x49, 0x41 },
    { 0x7f, 0x09, 0x09, 0x09, 0x01 },
    { 0x3e, 0x41, 0x49, 0x49, 0x7a },
    { 0x7f, 0x08, 0x08, 0x08, 0x7f },
    { 0x00, 0x41, 0x7f, 0x41, 0x00 },
    { 0x20, 0x40, 0x41, 0x3f, 0x01 },
    { 0x7f, 0x08, 0x14, 0x22, 0x41 },
    { 0x7f, 0x40, 0x40, 0x40, 0x40 },
    { 0x7f, 0x02, 0x0c, 0x02, 0x7f },
    { 0x7f, 0x04, 0x08, 0x10, 0x7f },
    { 0x3e, 0x41, 0x41, 0x41, 0x3e },
    { 0x7f, 0x09, 0x09, 0x09, 0x06 },
    { 0x3e, 0x41, 0x51, 0x21, 0x5e },
    { 0x7f, 0x09, 0x19, 0x29, 0x46 },
    { 0x46, 0x49, 0x49, 0x49, 0x31 },
    { 0x01, 0x01, 0x7f, 0x01, 0x01 },
    { 0x3f, 0x40, 0x40, 0x40, 0x3f },
    { 0x1f, 0x20, 0x40, 0x20, 0x1f },
    { 0x3f, 0x40, 0x38, 0x40, 0x3f },
    { 0x63, 0x14, 0x08, 0x14, 0x63 },
    { 0x07, 0x08, 0x70, 0x08, 0x07 },
    { 0x61, 0x51, 0x49, 0x45, 0x43 },
    { 0x00, 0x7f, 0x41, 0x41, 0x00 },
    { 0x15, 0x16, 0x7c, 0x16, 0x15 },
    { 0x00, 0x41, 0x41, 0x7f, 0x00 },
    { 0x04, 0x02, 0x01, 0x02, 0x04 },
    { 0x40, 0x40, 0x40, 0x40, 0x40 },
    { 0x00, 0x01, 0x02, 0x04, 0x00 },
    { 0x20, 0x54, 0x54, 0x54, 0x78 },
    { 0x7f, 0x48, 0x44, 0x44, 0x38 },
    { 0x38, 0x44, 0x44, 0x44, 0x00 },
    { 0x38, 0x44, 0x44, 0x48, 0x7f },
    { 0x38, 0x54, 0x54, 0x54, 0x18 },
    { 0x08, 0x7e, 0x09, 0x01, 0x02 },
    { 0x0c, 0x52, 0x52, 0x52, 0x3e },
    { 0x7f, 0x08, 0x04, 0x04, 0x78 },
    { 0x00, 0x44, 0x7d, 0x40, 0x00 },
    { 0x20, 0x40, 0x44, 0x3d, 0x00 },
    { 0x7f, 0x10, 0x28, 0x44, 0x00 },
    { 0x00, 0x41, 0x7f, 0x40, 0x00 },
    { 0x7c, 0x04, 0x18, 0x04, 0x78 },
    { 0x7c, 0x08, 0x04, 0x04, 0x78 },
    { 0x38, 0x44, 0x44, 0x44, 0x38 },
    { 0x7c, 0x14, 0x14, 0x14, 0x08 },
    { 0x08, 0x14, 0x14, 0x18, 0x7c },
    { 0x7c, 0x08, 0x04, 0x04, 0x08 },
    { 0x48, 0x54, 0x54, 0x54, 0x20 },
    { 0x04, 0x3f, 0x44, 0x40, 0x20 },
    { 0x3c, 0x40, 0x40, 0x20, 0x7c },
    { 0x1c, 0x20, 0x40, 0x20, 0x1c },
    { 0x3c, 0x40, 0x30, 0x40, 0x3c },
    { 0x44, 0x28, 0x10, 0x28, 0x44 },
    { 0x0c, 0x50, 0x50, 0x50, 0x3c },
    { 0x44, 0x64, 0x54, 0x4c, 0x44 },
    { 0x00, 0x08, 0x36, 0x41, 0x00 },
    { 0x00, 0x00, 0x7f, 0x00, 0x00 },
    { 0x00, 0x41, 0x36, 0x08, 0x00 },
    { 0x08, 0x08, 0x2a, 0x1c, 0x08 },
    { 0x08, 0x1c, 0x2a, 0x08, 0x08 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x70, 0x50, 0x70, 0x00, 0x00 },
    { 0x00, 0x00, 0x0f, 0x01, 0x01 },
    { 0x40, 0x40, 0x78, 0x00, 0x00 },
    { 0x10, 0x20, 0x40, 0x00, 0x00 },
    { 0x00, 0x18, 0x18, 0x00, 0x00 },
    { 0x0a, 0x0a, 0x4a, 0x2a, 0x1e },
    { 0x04, 0x44, 0x34, 0x14, 0x0c },
    { 0x20, 0x10, 0x78, 0x04, 0x00 },
    { 0x10, 0x08, 0x4c, 0x48, 0x38 },
    { 0x48, 0x48, 0x78, 0x48, 0x48 },
    { 0x48, 0x28, 0x18, 0x7c, 0x08 },
    { 0x08, 0x7c, 0x08, 0x28, 0x18 },
    { 0x40, 0x48, 0x48, 0x78, 0x40 },
    { 0x54, 0x54, 0x54, 0x7c, 0x00 },
    { 0x18, 0x00, 0x58, 0x40, 0x38 },
    { 0x08, 0x08, 0x08, 0x08, 0x08 },
    { 0x01, 0x41, 0x3d, 0x09, 0x07 },
    { 0x10, 0x08, 0x7c, 0x02, 0x01 },
    { 0x0e, 0x02, 0x43, 0x22, 0x1e },
    { 0x42, 0x42, 0x7e, 0x42, 0x42 },
    { 0x22, 0x12, 0x0a, 0x7f, 0x02 },
    { 0x42, 0x3f, 0x02, 0x42, 0x3e },
    { 0x0a, 0x0a, 0x7f, 0x0a, 0x0a },
    { 0x08, 0x46, 0x42, 0x22, 0x1e },
    { 0x04, 0x03, 0x42, 0x3e, 0x02 },
    { 0x42, 0x42, 0x42, 0x42, 0x7e },
    { 0x02, 0x4f, 0x22, 0x1f, 0x02 },
    { 0x4a, 0x4a, 0x40, 0x20, 0x1c },
    { 0x42, 0x22, 0x12, 0x2a, 0x46 },
    { 0x02, 0x3f, 0x42, 0x4a, 0x46 },
    { 0x06, 0x48, 0x40, 0x20, 0x1e },
    { 0x08, 0x46, 0x4a, 0x32, 0x1e },
    { 0x0a, 0x4a, 0x3e, 0x09, 0x08 },
    { 0x0e, 0x00, 0x4e, 0x20, 0x1e },
    { 0x04, 0x45, 0x3d, 0x05, 0x04 },
    { 0x00, 0x7f, 0x08, 0x10, 0x00 },
    { 0x44, 0x24, 0x1f, 0x04, 0x04 },
    { 0x40, 0x42, 0x42, 0x42, 0x40 },
    { 0x42, 0x2a, 0x12, 0x2a, 0x06 },
    { 0x22, 0x12, 0x7b, 0x16, 0x22 },
    { 0x00, 0x40, 0x20, 0x1f, 0x00 },
    { 0x78, 0x00, 0x02, 0x04, 0x78 },
    { 0x3f, 0x44, 0x44, 0x44, 0x44 },
    { 0x02, 0x42, 0x42, 0x22, 0x1e },
    { 0x04, 0x02, 0x04, 0x08, 0x30 },
    { 0x32, 0x02, 0x7f, 0x02, 0x32 },
    { 0x02, 0x12, 0x22, 0x52, 0x0e },
    { 0x00, 0x2a, 0x2a, 0x2a, 0x40 },
    { 0x3c, 0x22, 0x21, 0x20, 0x70 },
    { 0x40, 0x28, 0x10, 0x28, 0x06 },
    { 0x0a, 0x3e, 0x4a, 0x4a, 0x4a },
    { 0x04, 0x7f, 0x04, 0x14, 0x0c },
    { 0x40, 0x42, 0x42, 0x7e, 0x40 },
    { 0x4a, 0x4a, 0x4a, 0x4a, 0x7e },
    { 0x04, 0x05, 0x45, 0x25, 0x1c },
    { 0x0f, 0x40, 0x20, 0x1f, 0x00 },
    { 0x7c, 0x00, 0x7e, 0x40, 0x30 },
    { 0x7e, 0x40, 0x20, 0x10, 0x08 },
    { 0x7e, 0x42, 0x42, 0x42, 0x7e },
    { 0x0e, 0x02, 0x42, 0x22, 0x1e },
    { 0x42, 0x42, 0x40, 0x20, 0x18 },
    { 0x02, 0x04, 0x01, 0x02, 0x00 },
    { 0x07, 0x05, 0x07, 0x00, 0x00 },
    { 0x38, 0x44, 0x48, 0x30, 0x4c },
    { 0x20, 0x55, 0x54, 0x55, 0x78 },
    { 0x78, 0x54, 0x54, 0x54, 0x28 },
    { 0x28, 0x54, 0x54, 0x44, 0x20 },
    { 0x7c, 0x40, 0x40, 0x20, 0x7c },
    { 0x38, 0x44, 0x4c, 0x54, 0x24 },
    { 0x70, 0x48, 0x44, 0x44, 0x38 },
    { 0x38, 0x44, 0x44, 0x44, 0x7c },
    { 0x20, 0x40, 0x3c, 0x04, 0x04 },
    { 0x02, 0x02, 0x00, 0x07, 0x00 },
    { 0x00, 0x00, 0x04, 0x7d, 0x00 },
    { 0x0a, 0x04, 0x0a, 0x00, 0x00 },
    { 0x18, 0x24, 0x7e, 0x24, 0x10 },
    { 0x14, 0x7f, 0x54, 0x40, 0x40 },
    { 0x7c, 0x09, 0x05, 0x05, 0x78 },
    { 0x38, 0x45, 0x44, 0x45, 0x38 },
    { 0x7c, 0x48, 0x44, 0x44, 0x38 },
    { 0x38, 0x44, 0x44, 0x48, 0x7c },
    { 0x3c, 0x4a, 0x4a, 0x4a, 0x3c },
    { 0x30, 0x28, 0x10, 0x28, 0x18 },
    { 0x58, 0x64, 0x04, 0x64, 0x58 },
    { 0x3c, 0x41, 0x40, 0x21, 0x7c },
    { 0x63, 0x55, 0x49, 0x41, 0x41 },
    { 0x44, 0x3c, 0x04, 0x7c, 0x44 },
    { 0x45, 0x29, 0x11, 0x29, 0x45 },
    { 0x3c, 0x40, 0x40, 0x40, 0x7c },
    { 0x14, 0x14, 0x7c, 0x14, 0x12 },
    { 0x44, 0x3c, 0x14, 0x14, 0x74 },
    { 0x7c, 0x14, 0x1c, 0x14, 0x7c },
    { 0x10, 0x10, 0x54, 0x10, 0x10 },
    { 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f }
    #endif
};
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined _WINDOWS
    static int iCompleted = 1;
#endif

#if defined AVAGO_HCMS_CHAR_LCD                                          // {11}
    static unsigned char ucPixels[(2 * LCD_CHARACTERS) * BYTES_PER_HCMS_CHARACTER] = {0}; // pixel buffer twice the width of the HW so that the content can be shifted
    static int iPixelLocation = 0;
#endif



extern void fnLCD(TTASKTABLE *ptrTaskTable)                              // LCD task
{
    static int iState = STATE_INIT;
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[MEDIUM_MESSAGE];                        // reserve space for receiving messages

    if (iState != STATE_LCD_READY) {
        iState = fnInitDisplay(iState);
#if defined _WINDOWS
        while (0 == iCompleted) {
            iState = fnInitDisplay(iState);
        }
#endif
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
            break;

        case INTERRUPT_EVENT:
            break;

        case TASK_APPLICATION:
            {
                unsigned char ucLen = ucInputMessage[MSG_CONTENT_LENGTH];
                fnRead(PortIDInternal, ucInputMessage, ucLen);           // read the complete message
                ucLen--;
                if (E_LCD_COMMAND == ucInputMessage[0]) {
                    fnSecureCommands(&ucInputMessage[1], ucLen);         // we do a special check to ensure that the user doesn't change our interface mode of operation
                    fnWriteLine(&ucInputMessage[1], ucLen, 0);
                }
                else if (E_LCD_TEXT == ucInputMessage[0]) {
                    fnWriteLine(&ucInputMessage[1], ucLen, O_CONTROL_RS);     
                }
                else if (E_LCD_COMMAND_TEXT == ucInputMessage[0]) {      // first do command and then write text
                    fnSecureCommands(&ucInputMessage[1], 1);             // ensure command is not dangerous
                    fnWriteLine(&ucInputMessage[1], 1, 0);               // do one byte command
                    while (fnSendDisplay() == 0) {};                     // this should never return blocked but we test anyway
                    fnWriteLine(&ucInputMessage[2], (unsigned char)(ucLen - 1), O_CONTROL_RS); // write text
                }
                else if (E_LCD_PATTERN == ucInputMessage[0]) {
                    fnWriteLine(&ucInputMessage[2], 1, O_CONTROL_RS);    // we write the pattern to all locations in presently connected LCD
                    uTaskerStateChange(OWN_TASK, UTASKER_GO);            // switch to polling mode of operation
                    iState = fnInitDisplay(iState = STATE_LCD_READY + ucInputMessage[1] - 1); // try to start transmission 
                    break;
                }
                else if (E_LCD_READ_ADDRESS == ucInputMessage[0]) {
                    fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, fnReadDisplay(0)); // we read the value in the address register and return the result
                    fnSendAppRead(fnReadDisplay(0));
                    break;
                }
                else if (E_LCD_READ_RAM == ucInputMessage[0]) {
                    if (ucLen == 1) {                                    // {4}
                        fnSecureCommands(&ucInputMessage[1], 1);         // ensure command is not dangerous
                        fnWriteLine(&ucInputMessage[1], 1, 0);           // do one byte command
                        while (fnSendDisplay() == 0) {};                 // this should never return blocked but we test anyway
                    }
                    fnSendAppRead(fnReadDisplay(O_CONTROL_RS));          // we read the value in the internal RAM and return the result
                    break;
                }
                                                                         // assume only this type is received
                uTaskerStateChange(OWN_TASK, UTASKER_GO);                // switch to polling mode of operation
                iState = fnInitDisplay(iState = STATE_LCD_SENDING);      // try to start transmission 
            }
            break;
        }
    }
}


/**************************************************************************************************************/
#define MAX_TX_DBUF  40
static unsigned char ucTXDisp[MAX_TX_DBUF];                              // display output buffer
static unsigned char ucDispCount = 0;
static unsigned char ucDispSend = 0;                                     // the number of bytes already sent to display
static LCD_CONTROL_PORT_SIZE DisplayTextMode = 0;
#if defined _WINDOWS 
    static unsigned char ucLastRead = 0xff;                              // {7}
#endif

#define INIT_FUNCTION_SET          0x30
#if defined LCD_BUS_4BIT
    #define INIT_FUNCTION_SET_MODE 0x20
#else
    #define _fnWriteDisplay        fnWriteDisplay
    #define INIT_FUNCTION_SET_MODE 0x30
#endif
#define DISPLAY_OFF_NO_CURSOR      0x08
#define CLEAR_DISPLAY              0x01
#define DISPLAY_ON_NO_CURSOR       0x0c


static unsigned char fnReadDisplay(LCD_CONTROL_PORT_SIZE rs)
{
#if defined AVAGO_HCMS_CHAR_LCD && !defined _WINDOWS                     // {11}
    return 0;                                                            // we don't support reading from the AVAGO HCMS display so just return 0
#else
    #if defined LCD_BUS_4BIT
    LCD_BUS_PORT_SIZE RdData_lsb;
    #endif
    LCD_BUS_PORT_SIZE RdData_msb;
#if defined _ACCESSTYPE1                                                 // {8}
    SET_CONTROL_LINES(rs | O_WRITE_READ);                                // set required rs level and command read
    SET_DATA_LINES_INPUT();                                              // ensure the data lines are inputs
#else
    LCD_CONTROL_PORT_SIZE Control = (LCD_CONTROL_PORT_SIZE)(O_CONTROL_PORT_DAT & ~(O_CONTROL_LINES)); // backup other outputs of control port

    O_CONTROL_PORT_DAT = (Control | rs | O_WRITE_READ);                  // set required rs level and command read
    IO_BUS_PORT_DDR &= ~LCD_BUS_MASK;                                    // ensure the data lines are inputs
#endif

    CLOCK_EN_HIGH();                                                     // then clock EN to high state - tAS >= 40ns
                                                                         // date will be ready within about 100ns
    RdData_msb = (LCD_CONTROL_PORT_SIZE)(IO_BUS_PORT_DAT_IN & LCD_BUS_MASK); // read data in
#if DATA_SHIFT_LEFT > 0
    RdData_msb >>= DATA_SHIFT_LEFT;                                      // shift into position {2}
#elif DATA_SHIFT_RIGHT > 0
    RdData_msb <<= DATA_SHIFT_RIGHT;                                     // shift into position {2}
#endif

#if defined _ACCESSTYPE2                                                 // {8}
    O_SET_CONTROL_LOW(O_CONTROL_EN);                                     // then set clock EN low PWEH >= 220ns
#else
    O_CONTROL_PORT_DAT &= ~(O_CONTROL_EN);                               // then set clock EN low PWEH >= 220ns
#endif

#if defined LCD_BUS_4BIT
    // Since we are in 4 bit mode we must repeat clocking to ensure read is completed
    //
    DELAY_ENABLE_CLOCK_HIGH();                                           // ensure the second read is not too fast when in 4 bit mode {1}
    CLOCK_EN_HIGH();

    RdData_lsb = (LCD_BUS_PORT_SIZE)(IO_BUS_PORT_DAT_IN & LCD_BUS_MASK); // read data in
    #if DATA_SHIFT_LEFT > 0
    RdData_lsb >>= DATA_SHIFT_LEFT;                                      // shift into position {2}
    #elif DATA_SHIFT_RIGHT > 0
    RdData_lsb <<= DATA_SHIFT_RIGHT;                                     // shift into position {2}
    #endif

    #if defined _ACCESSTYPE2                                             // {8}
    O_SET_CONTROL_LOW(O_CONTROL_EN);
    #else
    O_CONTROL_PORT_DAT &= ~(O_CONTROL_EN);
    #endif
#endif

   // RS and R/W can be set back if required 10ns after EN falling edge
   // Data will remain stable at least 20ns
   // EN could then be reactivated with a periodicity of 500ns
    //
#if defined _ACCESSTYPE1                                                 // {8}
    SET_DATA_LINES_OUTPUT();                                             // ensure data bus outputs (between cycles)
#else
    IO_BUS_PORT_DDR |= LCD_BUS_MASK;                                     // ensure data bus outputs (between cycles)
#endif
#if defined _WINDOWS
    if (rs == 0) {
        RdData_msb &= ~LCD_BUSY;                                         // avoid blocking when simulating
    }
    #if defined LCD_BUS_4BIT                                             // {7}
    RdData_msb = (ucLastRead & 0xf0);
    RdData_lsb = (ucLastRead << 4);
    #else
    RdData_msb = ucLastRead;                                             // return the previously read value from last write command
    #endif
    ucLastRead = 0;                                                      // single-shot due to the way that the LCD simulatro works
#endif
#if defined LCD_BUS_4BIT
   return ((unsigned char)(RdData_msb | (RdData_lsb >> 4)));             // return the data byte read
#else
   return ((unsigned char)RdData_msb);
#endif
#endif
}

#if defined AVAGO_HCMS_CHAR_LCD                                          // {11}
static void fnPreparePixels(unsigned char ucValue)
{
    int iChar = BYTES_PER_HCMS_CHARACTER;
    unsigned char *ptrFontPixel = (unsigned char *)ucFont[ucValue];
    if (iPixelLocation > ((LCD_CHARACTERS - 1) * BYTES_PER_HCMS_CHARACTER)) { // don't alow writing beyond end of display (no wrap around)
        return;
    }
    while (iChar-- != 0) {
        ucPixels[iPixelLocation++] = *ptrFontPixel++;                    // fill the charcater in the pixel buffer with the font pixels
    }
}

static void fnHCMS_Byte(unsigned char ucByte)
{
    int iDataBits = 8;
    while (iDataBits-- != 0) {
        O_SET_CONTROL_LOW(HCMS_DISPLAY_CLOCK);                           // clock low
        if ((ucByte & 0x80) != 0) {                                      // send MSB first (prepare the data bit)
            O_SET_CONTROL_HIGH(HCMS_DISPLAY_DATA_TO_DISPLAY);            // data is '1'
        }
        else {
            O_SET_CONTROL_LOW(HCMS_DISPLAY_DATA_TO_DISPLAY);             // data is '0'
        }
        O_SET_CONTROL_HIGH(HCMS_DISPLAY_CLOCK);                          // clock high to latch in teh data
        ucByte <<= 1;
    }
}

// Chip enable is assumed to be high on entry - clock and RS assumed to be low on entry
//
static void fnHCMS_Data(unsigned char *ptr_ucPixels)
{
    int iDataBytes = (LCD_CHARACTERS * BYTES_PER_HCMS_CHARACTER);
    O_SET_CONTROL_HIGH(HCMS_DISPLAY_CLOCK);                              // clock high
    O_SET_CONTROL_LOW(HCMS_DISPLAY_CHIP_ENABLE);                         // latch pixel mode
    while (iDataBytes-- != 0) {
    #if defined _WINDOWS 
        CollectCommand(1, *ptr_ucPixels);
    #endif
        fnHCMS_Byte(*ptr_ucPixels++);                                    // update pixel content
    }
    O_SET_CONTROL_HIGH(HCMS_DISPLAY_CHIP_ENABLE);
    O_SET_CONTROL_LOW(HCMS_DISPLAY_CLOCK);                               // clock low to latch new data in
}

// Chip enable is assumed to be high on entry - clock assumed to be low on entry
//
static void fnHCMS_Control(unsigned char ucControlWord)
{
    O_SET_CONTROL_HIGH(HCMS_DISPLAY_CLOCK);                              // clock high
    O_SET_CONTROL_HIGH(HCMS_DISPLAY_RS);                                 // set RS high
    O_SET_CONTROL_LOW(HCMS_DISPLAY_CHIP_ENABLE);                         // latch control mode
    fnHCMS_Byte(ucControlWord);
    #if LCD_CHARACTERS > 4
    fnHCMS_Byte(ucControlWord);                                          // clock command also to second chip
    #endif
    #if defined _WINDOWS 
    CollectCommand(0, ucControlWord);
    #endif
    O_SET_CONTROL_HIGH(HCMS_DISPLAY_CHIP_ENABLE);
    O_SET_CONTROL_LOW(HCMS_DISPLAY_CLOCK);                               // clock low to latch new data in
    O_SET_CONTROL_LOW(HCMS_DISPLAY_RS);                                  // set RS low
}

static void fnHCMS_command(int iCommand)
{
    unsigned char ucBackup[BYTES_PER_HCMS_CHARACTER];
    switch (iCommand) {
    case HCMS_SET_HOME:
        iPixelLocation = 0;
        break;
    case HCMS_CLEAR_DISPLAY:
        uMemset(ucPixels, 0, sizeof(ucPixels));
        fnHCMS_Data(ucPixels);                                           // clear pixel shift register by writing to the display
        break;
    case HCMS_SHIFT_RIGHT:
        uMemcpy(ucBackup, &ucPixels[sizeof(ucPixels) - BYTES_PER_HCMS_CHARACTER], BYTES_PER_HCMS_CHARACTER);
        uReverseMemcpy(&ucPixels[BYTES_PER_HCMS_CHARACTER], ucPixels, (sizeof(ucPixels) - BYTES_PER_HCMS_CHARACTER)); // if this is know knon, enable UREVERSEMEMCPY in config.h
        uMemcpy(ucPixels, ucBackup, BYTES_PER_HCMS_CHARACTER);
        fnHCMS_Data(ucPixels);
        break;
    case HCMS_SHIFT_LEFT:
        uMemcpy(ucBackup, ucPixels, BYTES_PER_HCMS_CHARACTER);
        uMemcpy(ucPixels, &ucPixels[BYTES_PER_HCMS_CHARACTER], (sizeof(ucPixels) - BYTES_PER_HCMS_CHARACTER));
        uMemcpy(&ucPixels[sizeof(ucPixels) - BYTES_PER_HCMS_CHARACTER], ucBackup, BYTES_PER_HCMS_CHARACTER);
        fnHCMS_Data(ucPixels);
        break;
    }
}
#endif

// Write a byte or nibble to the display
//
static void _fnWriteDisplay(LCD_CONTROL_PORT_SIZE rs, unsigned char ucData)
{
#if defined AVAGO_HCMS_CHAR_LCD                                          // {11}
    if (rs == 0) {                                                       // control
        switch (ucData) {                                                // we partly behave as a standard charcater LCD when commands are received
        case (INIT_FUNCTION_SET_MODE | N_BIT | F_BIT):                   // when we recognise this initialisation we take the display out of reset and configure its with blank display
            O_SET_CONTROL_HIGH(HCMS_DISPLAY_RESET);
            fnHCMS_command(HCMS_SET_HOME);
            fnHCMS_command(HCMS_CLEAR_DISPLAY);
            fnHCMS_Control(AVAGO_HCMS_CONTROL_WORD_0_BRIGHTNESS_80_PERCENT | AVAGO_HCMS_CONTROL_WORD_0_NORMAL_OPERATION); // set brightness and enable display (we use exclusively serial mode)
            break;
        case 0x01:                                                       // clear LCD
            fnHCMS_command(HCMS_CLEAR_DISPLAY);
            break;
        case 0x02:                                                       // set home
            fnHCMS_command(HCMS_SET_HOME);
            break;
        case 0x03:                                                       // set home and clear LCD
            fnHCMS_command(HCMS_SET_HOME);
            fnHCMS_command(HCMS_CLEAR_DISPLAY);
            break;
        case 0x1c:                                                       // shift to the right
            fnHCMS_command(HCMS_SHIFT_RIGHT);
            break;
        case 0x18:                                                       // shift to the left
            fnHCMS_command(HCMS_SHIFT_LEFT);
            break;
        default:
            if ((ucData & 0x80) != 0) {                                  // set address
                iPixelLocation = ((ucData & 0x7f) * BITS_PER_HCMS_CHARACTER);
            }
            break;
        }
    }
    else {                                                               // data
        fnPreparePixels(ucData);
    }
#else
#if defined _ACCESSTYPE2                                                 // {8}
    LCD_BUS_PORT_SIZE DataBus = 0;
#else
    LCD_BUS_PORT_SIZE DataBus = (LCD_BUS_PORT_SIZE)IO_BUS_PORT_DAT;      // backup the present data lines (in case of supplimentary bits)
    LCD_CONTROL_PORT_SIZE Control = (LCD_BUS_PORT_SIZE)O_CONTROL_PORT_DAT; // backup present control line state
    DataBus &= ~LCD_BUS_MASK;                                            // mask out data in byte
#endif

#if DATA_SHIFT_LEFT > 0
    DataBus |= (((LCD_BUS_PORT_SIZE)ucData << DATA_SHIFT_LEFT) & LCD_BUS_MASK); // insert the data to be written
#else 
    DataBus |= (((LCD_BUS_PORT_SIZE)ucData >> DATA_SHIFT_RIGHT) & LCD_BUS_MASK);// insert the data to be written
#endif
#if defined _ACCESSTYPE2                                                 // {8}
    SET_BUS_DATA(DataBus);                                               // prepare data (it will be driven on to the bus later)
    SET_CONTROL_LINES(rs);                                               // set the RS bit accordingly, WR at zero, EN zero
    O_SET_CONTROL_HIGH(O_CONTROL_EN);                                    // then clock EN to high state - tAS >= 40ns 
                                                                         // date must have been set >= 60ns before EN falling edge 
#else
    #if defined _ACCESSTYPE3                                             // {5}{8}
    LCD_DRIVE_DATA();                                                    // drive the data bus (it initially drives the present state) - keep it stable for long enough before clocking
    #endif
    IO_BUS_PORT_DAT = DataBus;                                           // prepare data (it will be driven on to the bus later)
    Control &= ~O_CONTROL_LINES;                                         // mask out control bits
    Control |= rs;                                                       // set the RS bit accordingly
                                                                         // first set RS and R/W to write state 
    O_CONTROL_PORT_DAT = Control;                                        // RS as defined, WR at zero, EN zero
    O_CONTROL_PORT_DAT |= O_CONTROL_EN;                                  // then clock EN to high state - tAS >= 40ns 
                                                                         // date must have been set >= 60ns before EN falling edge 
#endif
#if !defined _ACCESSTYPE3                                                // {5}{8}
    LCD_DRIVE_DATA();                                                    // drive the data onto the bus - keep it stable for long enough before clocking
#endif
 
#if defined _WINDOWS 
    #if DATA_SHIFT_LEFT > 0   
        ucLastRead = CollectCommand((rs != 0), (unsigned char)((DataBus & LCD_BUS_MASK) >> DATA_SHIFT_LEFT)); // {7}
    #else
        ucLastRead = CollectCommand((rs != 0), (unsigned char)((DataBus & LCD_BUS_MASK) << DATA_SHIFT_RIGHT)); // {7}
    #endif
#endif

#if defined _ACCESSTYPE2                                                 // {8}
    O_SET_CONTROL_LOW(O_CONTROL_EN);                                     // then set clock EN low PWEH >= 220ns 
#else
    O_CONTROL_PORT_DAT &= ~O_CONTROL_EN;                                 // then set clock EN low PWEH >= 220ns 
#endif
    // RS and R/W can be set back if required 10ns after EN falling edge
    // Data must remain stable >= 10ns
    // EN could then be reactivated with a periodicity of 500ns
    //
#endif
}

#if defined LCD_BUS_4BIT
// This is only used in 4 bit mode to feed the byte to be written with two cycles
//
static void fnWriteDisplay(LCD_CONTROL_PORT_SIZE rs, unsigned char ucData)
{
    _fnWriteDisplay(rs, ucData);                                         // write high nibble
    _fnWriteDisplay(rs, (unsigned char)(ucData << 4));                   // write low nibble
}
#endif


// This routine copies the display message to the display buffer, ready for transmission
// It is assumed that the message will be completely sent before this routine is called again !!
//
static void fnWriteLine(unsigned char *pMsg, unsigned char ucMsgLen, LCD_CONTROL_PORT_SIZE Mode)
{
    if (ucDispCount != 0) {
        return;                                                          // the display is busy - must never occur !!
    }

    // The unterminated string or bytes are written to the display as fast as possible from the buffer
    //
    while (ucMsgLen-- != 0) {
        ucTXDisp[ucDispCount++] = *pMsg++;
        if (ucDispCount >= MAX_TX_DBUF) {
            break;
        }
    }
  
    DisplayTextMode = Mode;

    // The bytes will be sent to the display in the main loop one at a time
    //
}

static void fnSecureCommands(unsigned char *ucPtr, unsigned char ucLen)
{
    while (ucLen-- != 0) {
        if ((*ucPtr & 0xe0) == 0x20) {                                   // check for function set command
            *ucPtr &= ~DL_BIT;                                           // ensure we stay in 4 bit mode of operation by clearing the DL bit
        }
        ucPtr++;
    }
}


// If there is data waiting to be sent to the display and the display is not busy, send the next data using 2 writes in 4 bit mode
//
static int fnSendDisplay(void)
{
    if (ucDispCount != 0) {                                              // there is date to be sent to the display
        if ((fnReadDisplay(0) & LCD_BUSY) == 0) {                        // display is ready to accept data
            fnWriteDisplay(DisplayTextMode, (ucTXDisp[ucDispSend++]));   // write byte
            if (--ucDispCount == 0) {                                    // complete message has been sent
                ucDispSend = 0;
    #if defined _WINDOWS
                iCompleted = 1;
    #endif
                return (1);                                              // all has been set to display
            }
        }
    }
    #if defined _WINDOWS
    iCompleted = 0;
    #endif
    return (0);                                                          // either nothing done or not completed
}

#if defined LCD_CONTRAST_CONTROL                                         // {6}
extern void fnSetLCDContrast(unsigned char ucContrast)                   // {10}
{
    #if defined SUPPORT_PWM_MODULE && (defined _KINETIS || defined _M5223X) // {9}
    PWM_INTERRUPT_SETUP pwm_setup;
    pwm_setup.int_type = PWM_INTERRUPT;
    pwm_setup.pwm_mode = _LCD_CONTRAST_TIMER_MODE_OF_OPERATION;          // PWM timer configuration
    pwm_setup.int_handler = 0;                                           // no user interrupt call-back on PWM cycle
    pwm_setup.pwm_reference = _LCD_CONTRAST_TIMER;                       // timer module 0, channel 3
    pwm_setup.pwm_frequency = _LCD_CONTRAST_PWM_FREQUENCY;               // contrast control frequency
    if (ucContrast > 100) {
        ucContrast = 100;
    }
    pwm_setup.pwm_value = (ucContrast, pwm_setup.pwm_frequency);         // contrast as PWM value
    fnConfigureInterrupt((void *)&pwm_setup);                            // configure and start the PWM output
    #else
    TIMER_INTERRUPT_SETUP timer_setup = {0};                             // PWM Timer Init Struct
    timer_setup.int_type = TIMER_INTERRUPT;                              // timer setup type
    timer_setup.int_priority = 0;
    timer_setup.int_handler = 0;                                         // no interrupts used
    timer_setup.timer_reference = _LCD_CONTRAST_TIMER;                   // the timer used
    timer_setup.timer_mode  = _LCD_CONTRAST_TIMER_MODE_OF_OPERATION;     // the mode of operation
    timer_setup.timer_value = _LCD_CONTRAST_PWM_FREQUENCY;               // contrast control frequency
    if (ucContrast > 100) {
        ucContrast = 100;
    }
    timer_setup.pwm_value   = _PWM_PERCENT(ucContrast, timer_setup.timer_value); // contrast as PWM value
    fnConfigureInterrupt((void *)&timer_setup);                          // configure PWM output for contrast 
    #endif
}
#endif

#if defined LCD_BACKLIGHT_CONTROL
extern void fnSetBacklight(void)
{
    TIMER_INTERRUPT_SETUP timer_setup = {0};                             // PWM Timer Init Struct
    timer_setup.int_type = TIMER_INTERRUPT;                              // timer setup type
    timer_setup.int_priority = 0;
    timer_setup.int_handler = 0;                                         // no interrupts used
    timer_setup.timer_reference = _LCD_BACKLIGHT_TIMER;                  // use PWM module
    timer_setup.timer_mode  = _LCD_BACKLIGHT_TIMER_MODE_OF_OPERATION;    // use PWM channel 2 on port 1 output location
    timer_setup.timer_value = _LCD_BACKLIGHT_PWM_FREQUENCY;              // 150 Hz backlight frequency
    timer_setup.pwm_value   = _PWM_PERCENT(temp_pars->temp_parameters.ucGLCDBacklightPWM, timer_setup.timer_value); // contrast as PWM value
    fnConfigureInterrupt((void *)&timer_setup);                          // configure PWM output for contrast control
}        
#endif


// General control procedure for display - including initialisation
//
static int fnInitDisplay(int iState)
{
    // After power up the display needs to be idle for >= 30ms. This is ensured by a start up delay of the task
    //
    if ((ucDispCount != 0) && (fnSendDisplay() == 0)) {
        return iState;                                                   // return when display transmitter busy, sending next byte on the way
    }

    if ((iState > WRITE_FUNCTION_SET) && ((fnReadDisplay(0) & LCD_BUSY) != 0)) {
        return iState;                                                   // LCD is busy
    }

    switch (iState) {
    case STATE_INIT:                                                     // initialise the LCD hardware once on startup
        INITIALISE_LCD_CONTROL_LINES();
#if defined LCD_CONTRAST_CONTROL                                         // {6}
        fnSetLCDContrast(temp_pars->temp_parameters.ucGLCDContrastPWM);  // {10} set default contrast
#endif
        // Fall through intentionally
        //
    case STATE_INITIALISING:
        _fnWriteDisplay(0, INIT_FUNCTION_SET);                           // write function set
        uTaskerMonoTimer(OWN_TASK, T_INIT_WAIT, E_INIT_CONTINUE);        // wait at least 4,1ms the first time and 100us the second time - we do longer waits than necessary
        break;

    case WRITE_FUNCTION_SET:
        _fnWriteDisplay(0, INIT_FUNCTION_SET);                           // repeat after at least 4,1ms
        _fnWriteDisplay(0, INIT_FUNCTION_SET_MODE);                      // set final value after another 100us (after this the LCD is in the required 4 or 8 bit mode)
        uTaskerStateChange(OWN_TASK, UTASKER_GO);                        // switch to polling mode of operation since BF (Busy Flag) is not valid
        break;

    case STATE_INIT_2:
        fnWriteDisplay(0, (INIT_FUNCTION_SET_MODE | N_BIT | F_BIT));     // 2 line mode, display on
        break;

    case STATE_INIT_3:
        fnWriteDisplay(0, DISPLAY_OFF_NO_CURSOR);
        break;

    case STATE_INIT_4:
        fnWriteDisplay(0, CLEAR_DISPLAY);
        break;

    case STATE_DISPLAY_INITIALISED:
        fnWriteDisplay(0, DISPLAY_ON_NO_CURSOR);

        // At this point the display is clear and the cursor is at the home position.
        // We inform the application that the initialisation has terminated, so that it can start using it
        //
        fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_INITIALISED);
#if defined LCD_BACKLIGHT_CONTROL
        fnSetBacklight();                                                // {6}
#endif
        return STATE_LCD_READY;                                          // we are ready to work....

    case STATE_LCD_SENDING:
#if defined AVAGO_HCMS_CHAR_LCD                                          // {11}
        fnHCMS_Data(ucPixels);                                           // commit the new pixel buffer to the display
#endif
        uTaskerStateChange(OWN_TASK, UTASKER_STOP);                      // switch to event mode of operation
        fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_READY);         // we have completed an operation for the application
        break;

    default:                                                             
        if (iState > STATE_LCD_READY) {                                  // writing a pattern
            if (--iState <= STATE_LCD_READY) {                           // last character to be written
                iState = STATE_LCD_SENDING;
            }
            fnWriteLine(ucTXDisp, 1, O_CONTROL_RS);                      // repeat the test pattern 
        }
        return iState;
    }
    return (++iState);
}

// Send data read from the LCD to the application 
//
static void fnSendAppRead(unsigned char ucData)
{
    unsigned char ucMessage[HEADER_LENGTH + 2];

    ucMessage[MSG_DESTINATION_NODE]   = INTERNAL_ROUTE;                  // destination node 
    ucMessage[MSG_SOURCE_NODE]        = INTERNAL_ROUTE;                  // own node 
    ucMessage[MSG_DESTINATION_TASK]   = LCD_PARTNER_TASK;                // destination task
    ucMessage[MSG_SOURCE_TASK]        = OWN_TASK;                        // own task
    ucMessage[MSG_CONTENT_LENGTH]     = 2;
    ucMessage[MSG_CONTENT_COMMAND]    = E_LCD_READ;
    ucMessage[MSG_CONTENT_DATA_START] = ucData;

    fnWrite(0, ucMessage, (HEADER_LENGTH + 2));                          // send message to defined task
}
#endif

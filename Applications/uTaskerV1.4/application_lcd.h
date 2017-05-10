/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      application_lcd.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    06.08.2009 Add OLED graphical demo                                    {1}
    11.08.2009 Add line and rectangle drawing tests                       {2}
    26.09.2009 Move GLCD message routines to GLCD.c in uGLCDLIB           {3}
    27.09.2009 Add scrolling test to the demo                             {4}
    16.01.2010 Remove OLED_X/Y and use only GLCD_X/Y
    21.05.2011 Add picture frame demo to TWR_K60N512                      {5}
    18.11.2013 Change a string initialisation to avoid a problem with newer Codewarrior versions {6}
    01.03.2017 Add FT800 mode                                             {7}

The file is otherwise not specifically linked in to the project since it
is included by application.c when needed.

*/        

//#define TEST_DISPLAY_TTY                                               // test using LCD as simple debug output

#if defined SUPPORT_LCD && !defined _SUPPORT_LCD
  //#define LCD_READ_TEST                                                // test read of character LCD content

    #define LCD_TEST_MAX_SHIFT   5
    #define LCD_TEST_STARTING    (LCD_TEST_MAX_SHIFT + 1)

    #define _SUPPORT_LCD                                                 // include just once
    #if defined TEST_DISPLAY_TTY
    #define MAX_MESSAGES 10
    static CHAR message_queue[MAX_MESSAGES][LCD_CHARACTERS + 1];
    static int iQueueIn = 0;
    static int iQueueOut = 0;
    static CHAR cLine = '\x01';
    static int iLCD_state = 0;

    static void fnCheckLCD(void)
    {
        if (iLCD_state == 0) {
            return;
        }
        if (iQueueIn == iQueueOut) {                                     // nothing waiting
            return;
        }
        fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)message_queue[iQueueOut++], (LCD_CHARACTERS + 1));
        if (iQueueOut >= MAX_MESSAGES) {
            iQueueOut = 0;
        }
        iLCD_state = 0;                                                  // LCD busy
    }

    extern void fnDebugLcd(CHAR *message, QUEUE_TRANSFER nr_of_bytes)    // put messages into queue
    {
        message_queue[iQueueIn][0] = cLine;
        if (nr_of_bytes > LCD_CHARACTERS) {
            nr_of_bytes = LCD_CHARACTERS;
        }
        uMemcpy(&message_queue[iQueueIn][1], message, nr_of_bytes);
        uMemset(&message_queue[iQueueIn][nr_of_bytes + 1], ' ', (LCD_CHARACTERS - nr_of_bytes));
        iQueueIn++;
        if (iQueueIn >= MAX_MESSAGES) {
            iQueueIn = 0;
        }
        if (cLine == '\x01') {                                           // if first line, set second line, and reverse
            cLine = '\xc0';
        }
        else {
            cLine = '\x01';
        }
        fnCheckLCD();                                                    // send if not busy
    }
    #endif
#elif (defined SUPPORT_GLCD || defined SUPPORT_OLED) && (!defined GLCD_COLOR && !defined SUPPORT_TFT && !defined _SUPPORT_GLCD)
    #include "widgets.h"                                                 // widgets and images used by the project
    #define _SUPPORT_GLCD                                                // include just once
#endif


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//                 Character LCD (see below for GLCD)                  //
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


// Application task message reception from the LCD task - character LCD
//
#if defined SUPPORT_LCD && defined HANDLE_LCD_MESSAGES
        case TASK_LCD:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the complete message
            if (E_LCD_INITIALISED == ucInputMessage[0]) {
    #if defined USE_TIME_SERVER
                static const CHAR cWelcomeLCD[] = " Fetching Time";      // display a welcome message from present cursor position
    #else
                static const CHAR cWelcomeLCD[] = "Hi \xe4Tasker!";      // display a welcome message from present cursor position
    #endif
    #if defined TEST_DISPLAY_TTY
                fnDebugLcd("Test Line 1", 11);
                fnDebugLcd("Test Line 2", 11);
                iLCD_state = 1;
                fnCheckLCD();                                            // send if anything is waiting
    #else
                fnDoLCD_com_text(E_LCD_TEXT, (unsigned char *)cWelcomeLCD, (sizeof(cWelcomeLCD) - 1));
    #endif
            }
            else if (E_LCD_READY == ucInputMessage[0]) {
    #if defined TEST_DISPLAY_TTY
                iLCD_state = 1;
                fnCheckLCD();                                            // send if anything is waiting
    #else
                static const CHAR cSecondLineLCD[] = "\xc0****************"; // set second line
        #if defined USE_TIME_SERVER
              /*if (ucTimeServerTry != 0) {
                    break;
                }*/
                fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cSecondLineLCD, (sizeof(cSecondLineLCD) - 1));
        #else
                static const CHAR cFillSecondLineLCD[] = "\xe3*****";    // hidden before second line
                if (cShiftTest == LCD_TEST_STARTING) {
                    fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cSecondLineLCD, (sizeof(cSecondLineLCD) - 1));
                    uTaskerMonoTimer( OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), E_SHIFT_DISPLAY );
                    cShiftTest++;
                }
                else if (cShiftTest == (LCD_TEST_STARTING + 1)) {
                    cShiftTest = 0;
                    fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cFillSecondLineLCD, (sizeof(cFillSecondLineLCD) - 1));
                    uTaskerMonoTimer( OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), E_SHIFT_DISPLAY );
                }
            #if defined LCD_READ_TEST
                else {                                                   // ack after successfully writing new text
                    static unsigned char ucAddress = 0x80;
                    fnDoLCD_com_text(E_LCD_READ_RAM, (unsigned char *)&ucAddress, sizeof(ucAddress)); // request the content of the first LCD location
                    if (++ucAddress > 0x90) {
                        ucAddress = 0x80;
                    }
                }
            #endif
        #endif
    #endif
            }
    #if defined LCD_READ_TEST
            else if (E_LCD_READ == ucInputMessage[0]) {                  // answer from read of LCD display content
                fnDebugMsg("LCD = ");
                fnDebugHex(ucInputMessage[1], (WITH_LEADIN | 1));
                fnDebugMsg(" = ");
                fnWrite(DebugHandle, &ucInputMessage[1], 1);             // display the read value
                fnDebugMsg("\r\n");
            }
    #endif
            break;
#endif


// Application timer events for the character LCD
//
#if defined SUPPORT_LCD && defined HANDLE_TIMERS && !defined USE_TIME_SERVER
        case E_SHIFT_DISPLAY:                                            // timer event to command content movement
            { 
                static const unsigned char ucShiftRightCommand = 0x1c;   // command to shift content one position to right
                static const unsigned char ucShiftLeftCommand  = 0x18;   // command to shift content one position to left

                if (++cShiftTest > LCD_TEST_MAX_SHIFT) {                 // if maximum shift in one direction has been reached, change the direction
                    cShiftTest = -(LCD_TEST_MAX_SHIFT - 1);              // move to shift left
                }
                if (cShiftTest > 0) {
                    fnDoLCD_com_text(E_LCD_COMMAND, (unsigned char *)&ucShiftRightCommand, sizeof(ucShiftRightCommand));
                }
                else {
                    fnDoLCD_com_text(E_LCD_COMMAND, (unsigned char *)&ucShiftLeftCommand, sizeof(ucShiftLeftCommand));
                }
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), E_SHIFT_DISPLAY); // start a timer for the next movement
            }
            break;
#endif


// Message transmission routine used to send text and commands to the LCD task - character LCD
//
#if defined SUPPORT_LCD && defined LCD_MESSAGE_ROUTINES
    #if defined SUPPORT_DOUBLE_QUEUE_WRITES
// Demonstration of improved efficiency using double write support
//
extern void fnDoLCD_com_text(unsigned char ucType, unsigned char *ptrInput, unsigned char ucLength)
{
    unsigned char ucMessage[HEADER_LENGTH + 1];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = ucLength+1;                        // message length
    ucMessage[MSG_CONTENT_COMMAND]  = ucType;                            // command(s) or text for LCD

    if (fnWrite(INTERNAL_ROUTE, ucMessage, 0) != 0) {                    // send message header
        fnWrite(INTERNAL_ROUTE, ptrInput, ucLength);                     // add data
    }
}
    #else
extern void fnDoLCD_com_text(unsigned char ucType, unsigned char *ptrInput, unsigned char ucLength)
{
    #define MAX_LCD_MSG 50
    unsigned char ucMessage[HEADER_LENGTH + 1 + MAX_LCD_MSG];

    uMemcpy(&ucMessage[HEADER_LENGTH + 1], ptrInput, ucLength);

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = ++ucLength;                        // message length
    ucMessage[MSG_CONTENT_COMMAND]  = ucType;                            // command(s) or text for LCD

    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(ucLength + HEADER_LENGTH)); // send message to defined task
}
    #endif
#endif








/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//                       Graphical mono-GLCD                           //
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////





// Application task message reception from the LCD task - character mono-GLCD
//
#if defined HANDLE_LCD_MESSAGES && (defined SUPPORT_GLCD || defined SUPPORT_OLED) && !defined GLCD_COLOR // {1}
        case TASK_LCD:                                                   // messages received from the LCD task
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the complete message
            if (E_LCD_INITIALISED == ucInputMessage[0]) {                // the LCD initialisation has completed and use may start
    #if defined SUPPORT_GLCD
        #if !(defined USE_USB_AUDIO && defined AUDIO_FFT && defined BLAZE_K22) && !defined BLAZE_DIGITAL_WATCH && !defined FT800_GLCD_MODE
                GLCD_TEXT_POSITION text_pos;
        #endif
        #if defined SUPPORT_TOUCH_SCREEN && defined USE_PARAMETER_BLOCK && !defined TOUCH_FT6206 // only calibrate when it is possible to save parameters
                if ((temp_pars->temp_parameters.sTouchXminCal == 0) && (temp_pars->temp_parameters.sTouchXmaxCal == 0) && (temp_pars->temp_parameters.sTouchYminCal == 0) && (temp_pars->temp_parameters.sTouchYmaxCal == 0)) { // all zeros is recognised as not caibrated
                    extern void fnStartTouchCalibration(void);
#if 0
                    GLCD_LINE line;
#endif
                    GLCD_RECT box;
                    fnStartTouchCalibration();
                    text_pos.ucMode = (PAINT_LIGHT);
                    text_pos.usX = 10;
                    text_pos.usY = 15;
                    text_pos.ucFont = FONT_NINE_DOT;
                    fnDoLCD_text(&text_pos, "TOUCH SCREEN");
                    text_pos.ucMode = (PAINT_LIGHT);
                    text_pos.usY += 12;
                    fnDoLCD_text(&text_pos, "CALIBRATION REQUIRED");
                    box.ucMode = (PAINT_LIGHT | REDRAW);
                    box.rect_corners.usX_start = (GLCD_X/CGLCD_PIXEL_SIZE - 2);
                    box.rect_corners.usX_end = (GLCD_X/CGLCD_PIXEL_SIZE - 1);
                    box.rect_corners.usY_start = (GLCD_Y/CGLCD_PIXEL_SIZE - 2);
                    box.rect_corners.usY_end = (GLCD_Y/CGLCD_PIXEL_SIZE - 1);
#if 0
                    box.rect_corners.usX_start = 0;
                    box.rect_corners.usX_end = 1;
                    box.rect_corners.usY_start = 0;
                    box.rect_corners.usY_end = 1;
#endif
                    fnDoLCD_rect(&box);
#if 0
                    box.ucMode = PAINT_LIGHT;
                    box.rect_corners.usX_start = (GLCD_X/CGLCD_PIXEL_SIZE/2 - 5);
                    box.rect_corners.usX_end = (GLCD_X/CGLCD_PIXEL_SIZE/2 + 5);
                    box.rect_corners.usY_start = (GLCD_Y/CGLCD_PIXEL_SIZE/2 - 5);
                    box.rect_corners.usY_end = (GLCD_Y/CGLCD_PIXEL_SIZE/2 + 5);
                    fnDoLCD_rect(&box);
                    box.ucMode = PAINT_DARK;
                    line.line_start_end.usX_start = box.rect_corners.usX_start;
                    line.line_start_end.usX_end = box.rect_corners.usX_end;
                    line.line_start_end.usY_start = box.rect_corners.usY_start;
                    line.line_start_end.usY_end = box.rect_corners.usY_end;
                    box.rect_corners.usX_start += 1;
                    box.rect_corners.usX_end -= 1;
                    box.rect_corners.usY_start += 1;
                    box.rect_corners.usY_end -= 1;
                    fnDoLCD_rect(&box);
                    line.ucMode = PAINT_LIGHT;
                    fnDoLCD_line(&line);
                    line.ucMode = (PAINT_LIGHT | REDRAW);
                    line.line_start_end.usY_start += 10;
                    line.line_start_end.usY_end -= 10;
                    fnDoLCD_line(&line);
#endif
                    break;
                }
        #endif
        #if defined TEMP_HUM_TEST
                GLCD_LINE line1;
                GLCD_STYLE graphic_style;
                graphic_style.ucMode = STYLE_PIXEL_COLOR;
                graphic_style.color = (COLORREF)RGB(192,192,192);
                fnDoLCD_style(&graphic_style);
                line1.ucMode = PAINT_LIGHT;
                line1.line_start_end.usX_start = 10;
                line1.line_start_end.usY_start = 48;
                line1.line_start_end.usX_end = 150;
                line1.line_start_end.usY_end = 48;
                fnDoLCD_line(&line1);
                line1.line_start_end.usY_start += 3;
                line1.line_start_end.usY_end += 3;

                fnDoLCD_line(&line1);
                text_pos.ucMode = (PAINT_LIGHT);
                text_pos.usX = 32;
                text_pos.usY = 5;
                text_pos.ucFont = FONT_TEN_DOT;
                fnDoLCD_text(&text_pos, "Temperature");
                text_pos.usY = 17;
                text_pos.usX = 68;
                fnDoLCD_text(&text_pos, "and");
                text_pos.usY = 30;
                text_pos.usX = 25;
                text_pos.ucMode = (PAINT_LIGHT | REDRAW);
                fnDoLCD_text(&text_pos, "Humidity Test");
                graphic_style.ucMode = STYLE_PIXEL_COLOR;
                graphic_style.color = (COLORREF)RGB(255,255,0);
                fnDoLCD_style(&graphic_style);
        #else
            #if (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC1766_STK || defined OLIMEX_LPC2478_STK || defined K70F150M_12M) && defined SDCARD_SUPPORT // {5}
                ptr_utDirectory = utAllocateDirectory(DISK_D, 0);        // allocate a directory for use by this module associated with D: - no path string
                if (utOpenDirectory("pics", ptr_utDirectory) == UTFAT_SUCCESS)  { // open the root directory of disk D
			        ptr_utDirectory->usDirectoryFlags |= UTDIR_REFERENCED; // all accesses are referenced to the root directory
                    if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) { // if the directory exists
                        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.05 * SEC), E_NEXT_PHOTO); // start a timer which is used to display various images   
                        break;
                    }                                                    // else fall through to standard demo
                }
                SET_SPI_SD_INTERFACE_FULL_SPEED();                       // assume that the SD card is now valid so ensure that high speed is set for the LCD interface
            #endif 
            #if defined GLCD_MENU_TEST
                fnDrawMenu(&Menu);
            #elif defined USE_USB_AUDIO && defined AUDIO_FFT && defined BLAZE_K22
                #if defined VARIABLE_PIXEL_COLOUR
                GLCD_STYLE graphic_style;
                #endif
                static const GLCD_RECT rect1 = {(PAINT_LIGHT),{0, 0, (GLCD_X - 1), ((GLCD_Y/2) - 1)}}; // draw a rect
                static const GLCD_RECT rect2 = {(PAINT_INVERTED | REDRAW),{ 1, 1, (GLCD_X - 2), ((GLCD_Y / 2) - 2) } }; // draw a rect
                static const GLCD_RECT rect3 = {(PAINT_LIGHT), { 0, ((GLCD_Y / 2) + 1), (GLCD_X - 1), ((GLCD_Y) - 1) } }; // draw a rect
                static const GLCD_RECT rect4 = {(PAINT_INVERTED | REDRAW /*| GIVE_ACK*/),{ 1, ((GLCD_Y / 2) + 2), (GLCD_X - 2), ((GLCD_Y) - 2) } }; // draw a rect
                fnDoLCD_rect((GLCD_RECT *)&rect1);
                #if defined VARIABLE_PIXEL_COLOUR
                graphic_style.ucMode = STYLE_BACKGROUND_COLOR;
                graphic_style.color = (COLORREF)RGB(0, 0, 0);            // black background
                fnDoLCD_style(&graphic_style);
                #endif
                fnDoLCD_rect((GLCD_RECT *)&rect2);                       // draw black background
                #if defined VARIABLE_PIXEL_COLOUR
                graphic_style.color = LCD_ON_COLOUR;                     // set original background again
                fnDoLCD_style(&graphic_style);
                #endif
                fnDoLCD_rect((GLCD_RECT *)&rect3);
                fnDoLCD_rect((GLCD_RECT *)&rect4);                       // draw oscilloscope and spectrum analyser frames
                iNotBlocked = 1;
            #elif defined BLAZE_DIGITAL_WATCH
                iNotBlocked = 1;                                         // allow LCD update
            #elif defined FT800_GLCD_MODE                                // {7}
                {
                    Ft_Gpu_Hal_Context_t *phost = 0;                     // dummy for compatibility
                    Ft_Gpu_CoCmd_Dlstart(phost);
                    Ft_App_WrCoCmd_Buffer(phost,CLEAR_COLOR_RGB(255,255,255));
                    Ft_App_WrCoCmd_Buffer(phost,CLEAR(1,1,1));
                    Ft_App_WrCoCmd_Buffer(phost,COLOR_RGB(0x80,0x80,0x00));
                    Ft_Gpu_CoCmd_Text(phost,(GLCD_X/2), GLCD_Y/2, 31, OPT_CENTERX, "Set0   START");
                    Ft_App_WrCoCmd_Buffer(phost,DISPLAY());
                    Ft_Gpu_CoCmd_Swap(phost);
                    if (Ft_App_Flush_Co_Buffer(phost) != 0) {            // download the commands into fifo 
                  //Ft_Gpu_Hal_WaitCmdfifo_empty(phost);                 // wait till coprocessor completes the operation
                        return;                                          // wait until the co-processor has completed
                    }
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1.0 * SEC), E_NEXT_PIC);
                }
            #else
                text_pos.ucMode = PAINT_LIGHT;
                text_pos.usX = 2;
                text_pos.usY = 0;
                text_pos.ucFont = FONT_NINE_DOT;
                fnDoLCD_text(&text_pos, "Welcome to the");
                text_pos.usY = 15;
                fnDoLCD_text(&text_pos, "uTasker GLCD demo!");
                text_pos.usY = 40;
                text_pos.ucFont = (FONT_FIVE_DOT);
                text_pos.ucMode = (REDRAW | GIVE_ACK);                   // an ack is requested when this has been completely displayed
                fnDoLCD_text(&text_pos, "Starting...");
            #endif
        #endif
    #endif
            }
            else if (E_LCD_READY == ucInputMessage[0]) {                 // an acknowledgment from the display task (last job has been completed)
    #if defined USE_USB_AUDIO && defined AUDIO_FFT && defined BLAZE_K22
                iNotBlocked = 1;                                         // no longer block operation
    #else
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1.0 * SEC), E_NEXT_PIC); // start a timer which is used to display various images
    #endif
            }
    #if defined SUPPORT_TOUCH_SCREEN && defined TOUCH_MOUSE_TASK
            else if (E_TOUCH_MOUSE_EVENT == ucInputMessage[0]) {
                TOUCH_MOUSE_EVENT_MESSAGE tmEvent;
                uMemcpy(&tmEvent, &ucInputMessage[1], sizeof(TOUCH_MOUSE_EVENT_MESSAGE));
                switch (tmEvent.ucEvent) {
                case TOUCH_MOUSE_RELEASE:
                    fnDebugMsg("Release detect: ");
                    fnDebugDec(tmEvent.usX, 0);
                    fnDebugMsg(", ");
                    fnDebugDec(tmEvent.usY, WITH_CR_LF);
                    break;
        #if defined SUPPORT_TOUCH_SCREEN && defined USE_PARAMETER_BLOCK
                case TOUCH_MOUSE_CALIBRATION:
                    fnDebugMsg("Touch Calibration: ");
                    fnDebugDec(tmEvent.usX, 0);
                    fnDebugMsg(", ");
                    fnDebugDec(tmEvent.usY, WITH_CR_LF);
                    break;
    #endif
                case TOUCH_MOUSE_PRESS:
                    fnDebugMsg("Touch detect: ");
                    fnDebugDec(tmEvent.usX, 0);
                    fnDebugMsg(", ");
                    fnDebugDec(tmEvent.usY, WITH_CR_LF);
                    break;
                case TOUCH_MOUSE_MOVE:
                    fnDebugMsg("M");
                    break;
                }
            }
    #endif
            break;
#endif


// Application timer events for the LCD
//
#if defined HANDLE_TIMERS && defined SUPPORT_GLCD && !defined GLCD_COLOR && !defined SUPPORT_TFT && !(defined USE_USB_AUDIO && defined AUDIO_FFT && defined BLAZE_K22) // {1}
        case E_NEXT_PIC:                                                 // timer event
    #if defined FT800_GLCD_MODE                                          // {7} the following code was taken from the FT800 reference from FTDI and modified slightly to be used in a non-blocking manner
            {
                static int iTestNumber = 0;
                DELAY_LIMIT next_delay = (DELAY_LIMIT)(1.0 * SEC);
                Ft_Gpu_Hal_Context_t *phost = 0;                         // dummy for compatibility
                switch (iTestNumber++) {
                case 0:
                    {
	                    /* Construct DL of points */
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 0 , CLEAR_COLOR_RGB(128,128,128));
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 4 , CLEAR(1,1,1));
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 8 , COLOR_RGB(128, 0, 0) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 12, POINT_SIZE(5 * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 16, BEGIN(FTPOINTS) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 20, VERTEX2F((GLCD_X/5) * 16, (GLCD_Y/2) * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 24, COLOR_RGB(0, 128, 0) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 28, POINT_SIZE(15 * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 32, VERTEX2F((GLCD_X*2/5) * 16, (GLCD_Y/2) * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 36, COLOR_RGB(0, 0, 128) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 40, POINT_SIZE(25 * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 44, VERTEX2F((GLCD_X*3/5) * 16, (GLCD_Y/2) * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 48, COLOR_RGB(128, 128, 0) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 52, POINT_SIZE(35 * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 56, VERTEX2F((GLCD_X*4/5) * 16, (GLCD_Y/2) * 16) );
	                    Ft_Gpu_Hal_Wr32(phost, RAM_DL + 60, DISPLAY()); // display the image
	                    //delayms(100);

	                    /* Do a swap */
                        if (SAMAPP_GPU_DLSwap(DLSWAP_FRAME) != 0) {
                            next_delay = 0;
                        }
                    }
                    break;
                case 1:
                    {
	                    ft_int16_t LineHeight = 25;

	                    Ft_App_WrDlCmd_Buffer(phost, CLEAR(1, 1, 1)); // clear screen
	                    Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(128, 0, 0) );
	                    Ft_App_WrDlCmd_Buffer(phost, LINE_WIDTH(5 * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, BEGIN(LINES) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F((GLCD_X/4) * 16,((GLCD_Y - LineHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F((GLCD_X/4) * 16,((GLCD_Y + LineHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(0, 128, 0) );
	                    Ft_App_WrDlCmd_Buffer(phost, LINE_WIDTH(10 * 16) );
	                    LineHeight = 40;
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F((GLCD_X*2/4) * 16,((GLCD_Y - LineHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F((GLCD_X*2/4) * 16,((GLCD_Y + LineHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(128, 128, 0) );
	                    Ft_App_WrDlCmd_Buffer(phost, LINE_WIDTH(20 * 16) );
	                    LineHeight = 55;
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F((GLCD_X*3/4) * 16, ((GLCD_Y - LineHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F((GLCD_X*3/4) * 16, ((GLCD_Y + LineHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, DISPLAY() );

	                    /* Download the DL into DL RAM */
	                    Ft_App_Flush_DL_Buffer(phost);

	                    /* Do a swap */
                        if (SAMAPP_GPU_DLSwap(DLSWAP_FRAME) != 0) {
                            next_delay = 0;
                        }
                    }
                    break;
                case 2:
                    {
	                    ft_int16_t RectWidth,RectHeight;

	                    Ft_App_WrDlCmd_Buffer(phost, CLEAR(1, 1, 1)); // clear screen
	                    Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(0, 0, 128) );
	                    Ft_App_WrDlCmd_Buffer(phost, LINE_WIDTH(1 * 16) );//LINE_WIDTH is used for corner curvature
	                    Ft_App_WrDlCmd_Buffer(phost, BEGIN(RECTS) );
	                    RectWidth = 5;RectHeight = 25;
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F( ((GLCD_X/4) - (RectWidth/2)) * 16,((GLCD_Y - RectHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F( ((GLCD_X/4) + (RectWidth/2)) * 16,((GLCD_Y + RectHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(0, 128, 0) );
	                    Ft_App_WrDlCmd_Buffer(phost, LINE_WIDTH(5 * 16) );
	                    RectWidth = 10;RectHeight = 40;
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F( ((GLCD_X*2/4) - (RectWidth/2)) * 16,((GLCD_Y - RectHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F( ((GLCD_X*2/4) + (RectWidth/2)) * 16,((GLCD_Y + RectHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(128, 128, 0) );
	                    Ft_App_WrDlCmd_Buffer(phost, LINE_WIDTH(10 * 16) );
	                    RectWidth = 20;RectHeight = 55;
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F( ((GLCD_X*3/4) - (RectWidth/2)) * 16,((GLCD_Y - RectHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, VERTEX2F( ((GLCD_X*3/4) + (RectWidth/2)) * 16,((GLCD_Y + RectHeight)/2) * 16) );
	                    Ft_App_WrDlCmd_Buffer(phost, DISPLAY() );

	                    /* Download the DL into DL RAM */
	                    Ft_App_Flush_DL_Buffer(phost);

	                    /* Do a swap */
                        if (SAMAPP_GPU_DLSwap(DLSWAP_FRAME) != 0) {
                            next_delay = 0;
                        }
                    }
                    break;
                case 3:
                    {
                        /* sample app structure definitions */
                        typedef struct SAMAPP_Bitmap_header
                        {
	                        ft_uint8_t Format;
	                        ft_int16_t Width;
	                        ft_int16_t Height;
	                        ft_int16_t Stride;
	                        ft_int32_t Arrayoffset;
                        }SAMAPP_Bitmap_header_t;

                        /* Header of raw data containing properties of bitmap */
                        SAMAPP_Bitmap_header_t SAMAPP_Bitmap_RawData_Header[] = 
                        {
	                        /* format,width,height,stride,arrayoffset */
	                        {RGB565,	40,	40,	40*2,	0	},
	                        {PALETTED,	40,	40,	40,	0	},
	                        {PALETTED,	480,	272,	480,	0	},
                        };

                        /* Raw data array for Lena Face*/
                        FT_PROGMEM  ft_prog_uchar8_t SAMAPP_Bitmap_RawData[] = 
                        {
                        72,57,105,57,171,65,236,73,237,65,236,65,72,41,35,8,70,49,44,139,80,188,243,204,148,213,182,164,120,173,24,198,55,206,214,197,149,197,149,197,83,205,18,213,50,221,214,229,55,230,121,238,217,230,218,230,250,238,27,239,60,247,59,247,
                        87,238,237,203,198,121,164,48,198,40,43,90,147,172,110,139,40,49,72,49,139,57,238,73,237,65,236,65,104,49,99,16,232,81,141,155,145,188,84,205,149,205,214,172,120,173,248,197,88,206,214,197,117,197,84,197,19,205,18,213,83,221,245,229,
                        88,238,153,238,185,238,218,238,219,238,27,239,60,247,59,247,152,238,46,212,231,129,196,64,165,40,202,81,83,164,143,139,231,48,8,49,74,57,46,82,237,73,12,66,235,57,70,57,108,147,112,196,84,205,22,214,246,205,22,173,118,181,56,206,
                        120,206,181,197,83,197,242,204,209,220,18,221,116,229,246,229,88,230,153,238,185,238,250,238,250,238,28,239,92,247,60,247,218,246,209,228,104,146,4,73,133,24,71,57,242,147,208,147,73,57,40,57,8,49,172,65,40,41,106,49,75,82,11,139,
                        15,180,209,204,147,213,22,214,88,206,87,173,151,181,247,197,214,205,19,205,209,204,210,212,18,221,115,229,148,229,245,229,87,238,152,238,185,238,217,238,250,238,27,247,60,247,92,247,59,247,147,229,11,171,70,89,132,32,230,40,144,139,207,147,
                        40,49,73,57,105,57,106,49,231,32,104,57,172,106,207,171,209,196,50,205,115,213,246,205,121,206,248,189,150,181,84,189,243,196,210,212,19,229,83,229,116,229,148,229,180,229,213,229,54,230,120,230,185,230,185,238,217,238,250,238,27,247,93,255,
                        92,247,22,238,140,187,134,105,132,40,165,32,13,115,208,155,9,49,74,57,172,65,40,41,39,41,41,90,141,147,209,196,147,213,83,213,84,205,246,205,186,214,23,198,210,180,238,171,173,179,207,187,48,196,209,204,83,221,181,237,180,229,180,237,
                        246,229,87,238,120,238,152,238,184,238,217,246,27,247,59,247,250,246,147,229,11,163,69,89,131,32,165,40,140,98,207,147,138,57,138,57,139,65,231,32,233,81,76,139,144,188,179,205,21,214,180,205,181,205,24,206,88,206,50,197,205,179,75,179,
                        12,179,11,163,234,162,109,171,80,196,51,221,116,229,147,237,180,237,54,238,120,238,119,238,119,230,184,238,151,238,54,230,115,213,46,188,105,146,37,89,132,40,164,32,10,82,239,147,13,74,171,57,8,41,38,41,234,130,80,180,115,205,53,222,
                        87,214,213,205,215,205,246,205,208,188,140,187,141,195,205,195,173,187,11,171,231,129,198,121,105,146,173,187,18,221,51,229,147,237,245,237,87,230,87,238,119,238,245,237,17,205,237,179,10,147,169,154,72,146,134,105,196,56,133,32,169,65,208,147,
                        38,41,6,41,197,24,41,90,14,164,50,205,21,222,120,214,153,214,87,214,82,189,204,163,39,146,39,138,231,121,7,122,231,113,230,121,231,121,137,146,170,170,11,187,239,211,209,228,51,229,213,237,120,238,152,246,87,238,209,204,43,163,198,113,
                        69,89,166,105,198,121,134,105,4,65,132,32,136,65,176,147,99,24,196,32,135,57,141,155,241,188,212,213,86,222,217,214,185,214,17,181,10,139,134,97,228,80,5,81,195,64,70,81,232,89,232,89,5,73,232,121,202,170,76,195,173,211,47,212,
                        241,228,213,237,185,246,152,230,144,180,138,122,167,89,135,89,135,81,5,73,36,73,5,73,196,56,164,32,71,65,143,147,132,24,5,41,106,106,112,180,114,205,22,214,152,222,251,222,213,205,140,155,166,105,163,64,4,73,5,65,164,48,201,73,
                        112,123,83,148,10,98,232,121,136,170,43,203,108,203,238,219,241,220,54,238,186,246,212,213,170,122,70,65,71,65,76,98,13,131,70,73,195,56,195,48,196,48,164,40,38,65,143,139,39,49,201,73,110,147,242,196,181,205,119,214,186,214,87,206,
                        79,188,202,146,134,105,37,89,134,89,232,97,135,81,238,114,88,181,220,222,240,171,137,146,72,162,233,186,43,203,206,211,242,228,119,238,184,238,177,180,8,98,103,65,43,90,112,139,114,180,135,97,196,64,131,40,196,48,229,40,38,57,143,139,
                        10,82,204,114,81,172,115,197,55,206,185,214,87,206,144,180,43,179,76,187,235,162,105,138,8,130,204,138,14,131,115,156,57,206,219,238,19,221,108,195,201,186,234,186,43,203,141,211,51,229,185,246,152,222,206,179,236,138,206,122,145,131,83,164,
                        49,188,166,105,195,64,131,40,197,40,38,49,70,57,143,139,108,155,238,163,19,197,22,214,217,214,22,206,79,180,10,179,108,195,206,203,238,203,141,187,12,171,235,170,78,179,16,188,211,196,244,204,20,205,48,204,141,195,11,195,44,195,140,203,
                        19,229,153,246,218,230,211,196,240,187,143,171,241,171,143,163,105,138,101,105,5,81,228,64,229,48,39,57,38,57,111,139,237,171,111,188,213,213,153,214,245,197,46,180,234,170,43,195,206,211,48,212,80,212,80,212,239,211,173,203,174,195,239,195,
                        80,196,115,204,244,212,80,212,174,195,76,195,76,203,109,211,211,220,121,238,28,239,182,213,113,204,239,195,174,187,44,163,72,138,7,130,133,113,227,72,196,48,39,57,6,49,79,131,177,196,82,197,119,222,55,198,46,164,201,162,137,170,77,203,
                        239,211,81,220,146,220,178,228,177,220,145,220,113,212,145,212,211,212,211,212,243,212,113,212,239,203,141,203,108,203,44,203,145,220,89,238,61,239,88,230,243,212,80,204,174,203,76,187,202,170,201,162,230,129,4,81,196,48,38,57,5,49,46,123,
                        180,213,245,213,151,214,144,156,136,138,71,162,234,186,141,211,239,219,112,220,210,228,18,229,83,221,51,221,51,229,84,229,84,229,117,221,20,221,113,212,239,203,173,203,108,211,76,211,81,220,56,238,93,239,154,222,84,221,145,212,14,212,141,203,
                        44,195,11,187,39,138,36,89,229,56,37,65,229,48,13,123,184,222,119,214,116,173,231,89,230,129,136,170,43,195,108,203,238,211,80,220,209,228,19,229,116,229,148,229,148,229,149,229,117,229,148,221,18,221,112,220,47,220,238,203,108,211,44,203,
                        16,212,247,229,61,239,154,230,84,221,209,212,15,212,174,211,76,195,75,195,71,146,69,81,197,56,70,73,196,48,204,114,215,222,148,181,170,90,195,64,230,137,233,186,42,203,107,203,205,211,47,220,145,228,242,228,116,237,181,237,181,229,148,229,
                        148,229,82,229,242,228,111,212,47,220,238,211,140,211,75,203,15,212,215,229,61,239,219,230,83,213,209,212,47,212,174,211,108,211,75,195,39,138,37,73,196,56,38,65,196,48,139,106,19,157,239,123,229,40,5,73,7,138,201,178,42,203,107,211,
                        173,211,14,220,112,220,208,228,18,229,148,229,148,229,147,229,115,229,50,221,208,220,79,220,13,212,173,211,107,203,75,203,206,211,150,229,28,239,251,230,83,213,177,212,48,212,206,211,108,203,42,195,231,121,229,72,196,40,38,73,197,56,74,106,
                        170,82,232,65,164,40,37,81,230,137,169,178,43,203,108,211,141,211,238,219,80,220,145,228,242,228,82,237,84,229,115,229,82,229,17,229,175,220,13,212,173,211,140,203,75,203,10,203,174,211,85,221,219,238,60,239,83,213,177,212,79,212,207,211,
                        140,203,234,178,167,105,229,56,229,48,102,73,196,56,9,90,163,24,164,32,228,48,101,89,230,137,168,178,42,203,107,203,140,211,237,219,80,220,112,220,209,228,241,228,50,229,50,229,50,229,241,228,143,220,237,211,139,203,107,203,75,203,9,203,
                        140,203,243,220,154,230,61,239,84,221,145,212,47,212,238,211,108,203,169,170,101,89,229,48,38,57,134,81,196,56,200,81,34,16,132,24,69,65,133,105,230,137,135,170,10,195,75,211,108,211,205,219,14,220,79,220,111,220,144,220,209,228,241,220,
                        241,228,176,228,47,220,171,203,75,203,75,203,75,203,11,195,140,211,178,220,89,238,125,247,116,221,145,220,47,212,206,211,108,195,72,138,6,65,197,40,37,65,134,89,195,48,103,73,66,24,164,40,102,81,167,113,230,137,135,170,10,195,75,203,
                        108,211,173,211,238,219,47,220,79,228,111,220,144,220,177,220,209,220,144,220,14,212,75,195,43,203,108,211,172,211,107,211,107,211,145,220,56,238,92,239,84,221,112,212,14,212,173,203,43,179,168,97,197,40,197,32,37,57,133,81,195,56,38,65,
                        99,32,196,40,135,73,231,121,6,146,135,178,9,195,42,203,75,211,173,219,206,219,47,220,79,220,79,220,144,228,177,220,208,220,176,220,14,212,108,195,42,195,42,187,202,170,201,186,201,194,204,211,83,229,120,230,242,212,79,212,238,211,140,195,
                        170,154,37,65,197,32,229,48,37,65,69,81,228,64,228,56,99,32,196,48,199,81,39,130,39,154,136,186,9,203,42,203,107,211,140,211,238,219,15,220,46,220,47,220,112,220,144,220,176,220,176,228,79,220,204,211,75,203,234,194,169,170,200,186,
                        201,186,238,203,51,221,214,229,209,212,48,204,239,203,76,179,40,114,197,32,198,32,6,49,69,73,133,81,4,65,196,40,99,32,228,48,232,81,105,130,103,154,168,186,9,203,42,203,74,211,140,219,205,219,238,219,14,212,47,220,111,220,144,220,
                        176,220,177,228,111,220,14,212,172,211,75,203,75,203,140,203,238,195,211,212,149,221,148,221,177,212,16,212,205,195,235,154,102,81,165,32,230,32,38,57,134,81,166,89,37,73,164,48,131,40,229,48,200,81,137,130,104,154,200,178,41,203,42,203,
                        75,203,140,211,205,219,237,219,238,219,15,220,80,228,112,228,144,220,145,220,144,220,111,220,47,220,14,220,112,220,83,229,181,221,88,230,120,222,115,221,112,204,15,204,109,179,106,122,230,40,198,24,6,41,103,57,166,81,166,89,69,73,196,56,
                        164,40,5,49,200,81,170,130,136,154,200,186,42,203,74,203,74,211,140,211,173,211,205,219,205,219,238,211,46,220,79,220,80,220,111,220,112,220,79,212,79,212,144,220,82,221,121,230,121,230,185,238,152,230,83,221,48,204,15,204,12,163,168,81,
                        165,32,197,32,6,49,103,65,134,89,101,89,69,81,196,56,164,48,229,56,102,73,106,122,104,146,168,178,9,203,74,211,107,203,140,219,172,211,204,219,205,219,204,211,205,219,238,219,14,220,79,220,79,220,79,212,112,220,176,220,50,221,87,230,
                        120,238,88,230,247,229,242,212,15,204,174,195,139,130,38,49,165,24,198,32,38,49,102,65,166,89,133,89,69,73,196,48,228,56,5,57,69,73,73,114,39,138,103,170,233,202,74,211,74,211,107,211,140,219,173,219,205,219,205,219,140,211,107,195,
                        75,195,172,203,204,211,173,211,204,203,204,211,237,211,177,228,209,220,209,212,112,204,239,203,239,203,76,171,233,97,197,32,198,32,230,40,38,49,102,73,166,89,166,89,101,81,228,56,37,65,5,65,37,65,40,106,230,121,70,154,232,194,74,203,
                        42,203,106,211,139,211,172,219,237,219,12,220,106,203,232,186,167,178,233,194,10,195,42,203,9,195,234,194,11,195,141,211,173,211,108,195,11,187,76,187,206,195,170,122,102,65,197,24,197,40,229,48,37,57,134,81,167,97,166,89,102,81,229,56,
                        37,73,37,65,37,57,232,97,198,105,38,138,168,186,9,195,10,203,42,203,74,211,172,211,237,211,45,220,172,211,42,195,168,194,104,186,136,194,201,202,12,203,77,203,143,203,17,220,18,212,176,211,110,195,206,203,108,171,168,81,229,40,197,32,
                        197,32,229,40,37,65,134,81,198,97,133,89,134,81,37,65,69,73,69,73,37,65,166,89,165,97,230,121,136,162,233,186,233,194,9,195,75,203,108,203,204,211,237,211,14,220,204,219,75,211,9,211,233,210,43,227,142,227,50,228,180,236,245,236,
                        147,220,82,212,16,204,14,196,105,114,5,49,197,32,229,40,196,40,228,48,5,57,101,81,230,97,166,97,102,81,70,65,70,65,70,65,70,65,166,81,133,81,198,105,38,146,167,178,200,178,233,194,42,195,75,203,172,211,206,211,238,219,204,219,
                        140,211,74,203,42,211,10,211,43,211,109,211,174,219,143,211,240,219,48,220,15,204,75,163,135,73,197,40,197,32,229,48,228,48,229,48,5,57,134,81,231,97,199,97,102,81,69,73,69,65,37,65,38,65,102,73,102,73,134,97,197,121,70,154,
                        136,170,201,186,9,195,75,195,108,211,173,211,205,211,206,211,173,211,140,211,108,211,140,203,140,211,141,211,141,211,173,211,240,211,47,204,141,179,7,106,5,49,197,32,229,32,5,49,5,49,229,56,4,57,102,81,7,106,230,97,100,89,101,81,
                        37,65,37,65,5,57,37,65,69,73,69,73,133,97,197,129,38,154,135,170,233,186,42,195,107,203,173,203,173,211,205,203,238,211,238,211,14,212,47,212,111,220,144,220,112,220,144,220,144,220,14,196,202,146,37,65,164,32,197,24,5,41,38,57,
                        37,57,4,49,5,57,134,89,7,106,199,105,133,89,134,81,36,65,36,65,5,65,4,57,37,65,36,65,36,81,36,89,165,113,38,154,135,170,10,195,107,195,140,203,174,211,238,211,15,212,80,220,177,212,210,220,19,221,84,229,51,229,242,220,
                        177,220,173,179,41,114,196,40,196,32,229,32,38,41,70,57,69,57,228,48,4,65,199,89,72,114,231,97,198,97,134,89,228,48,4,57,4,65,5,57,37,65,37,65,69,73,36,65,195,72,68,97,229,145,168,170,43,195,108,203,173,211,14,212,
                        47,212,112,220,211,220,19,229,51,221,51,229,52,229,243,220,177,212,76,155,167,89,228,40,6,33,39,33,70,41,38,57,5,57,196,48,37,65,231,97,73,114,231,105,166,97,198,97,229,40,228,48,5,57,37,57,38,65,38,65,70,73,101,81,
                        228,64,4,73,101,97,197,129,71,146,169,162,43,179,141,195,205,203,47,212,113,212,210,220,19,221,18,221,209,220,144,212,47,204,202,146,102,73,197,32,39,33,39,41,71,49,70,57,4,49,228,48,70,65,8,98,104,114,7,98,231,105,230,97,
                        };
                        SAMAPP_Bitmap_header_t *p_bmhdr;
	                    ft_int16_t BMoffsetx,BMoffsety;

	                    p_bmhdr = (SAMAPP_Bitmap_header_t *)&SAMAPP_Bitmap_RawData_Header[0];
	                    /* Copy raw data into address 0 followed by generation of bitmap */
	                    Ft_Gpu_Hal_WrMemFromFlash(phost, RAM_G,&SAMAPP_Bitmap_RawData[p_bmhdr->Arrayoffset], p_bmhdr->Stride*p_bmhdr->Height);


	                    Ft_App_WrDlCmd_Buffer(phost, CLEAR(1, 1, 1)); // clear screen
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_RGB(255,255,255));
	                    Ft_App_WrDlCmd_Buffer(phost,BITMAP_SOURCE(RAM_G));
	                    Ft_App_WrDlCmd_Buffer(phost,BITMAP_LAYOUT(p_bmhdr->Format, p_bmhdr->Stride, p_bmhdr->Height));
	                    Ft_App_WrDlCmd_Buffer(phost,BITMAP_SIZE(NEAREST, BORDER, BORDER, p_bmhdr->Width, p_bmhdr->Height));
	                    Ft_App_WrDlCmd_Buffer(phost,BEGIN(BITMAPS)); // start drawing bitmaps
	                    BMoffsetx = ((GLCD_X/4) - (p_bmhdr->Width/2));
	                    BMoffsety = ((GLCD_Y/2) - (p_bmhdr->Height/2));
	                    Ft_App_WrDlCmd_Buffer(phost,VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_RGB(255, 64, 64)); // red at (200, 120)
	                    BMoffsetx = ((GLCD_X*2/4) - (p_bmhdr->Width/2));
	                    BMoffsety = ((GLCD_Y/2) - (p_bmhdr->Height/2));
	                    Ft_App_WrDlCmd_Buffer(phost,VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_RGB(64, 180, 64)); // green at (216, 136)
	                    BMoffsetx += (p_bmhdr->Width/2);
	                    BMoffsety += (p_bmhdr->Height/2);
	                    Ft_App_WrDlCmd_Buffer(phost,VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_RGB(255, 255, 64)); // transparent yellow at (232, 152)
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_A(150));
	                    BMoffsetx += (p_bmhdr->Width/2);
	                    BMoffsety += (p_bmhdr->Height/2);
	                    Ft_App_WrDlCmd_Buffer(phost,VERTEX2II(BMoffsetx, BMoffsety, 0, 0));
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_A(255));
	                    Ft_App_WrDlCmd_Buffer(phost,COLOR_RGB(255,255,255));
	                    Ft_App_WrDlCmd_Buffer(phost,VERTEX2F(-10*16, -10*16));//for -ve coordinates use vertex2f instruction
	                    Ft_App_WrDlCmd_Buffer(phost,END());
	                    Ft_App_WrDlCmd_Buffer(phost, DISPLAY() );

	                    /* Download the DL into DL RAM */
	                    Ft_App_Flush_DL_Buffer(phost);

	                    /* Do a swap */
                        if (SAMAPP_GPU_DLSwap(DLSWAP_FRAME) != 0) {
                            next_delay = 0;
                        }
                    }
                    break;
                case 4:
                case 5:
                case 6:
                case 7:
                    {
	                    ft_int32_t j,hoffset,voffset,stringlen;
	                    ft_uint32_t FontTableAddress;
	                    const ft_uchar8_t Display_string[] = "Hello World";
	                    FT_Gpu_Fonts_t Display_fontstruct;
                        int iFontNumber = (iTestNumber - 5);

	                    hoffset = ((GLCD_X - 100) / 2);
	                    voffset = GLCD_Y / 2;

	                    /* Read the font address from 0xFFFFC location */
	                    FontTableAddress = Ft_Gpu_Hal_Rd32(phost, ROMFONT_TABLEADDRESS);
	                    stringlen = sizeof(Display_string) - 1;

		                /* Read the font table from hardware */
		                Ft_Gpu_Hal_RdMem(phost,(FontTableAddress + iFontNumber*FT_GPU_FONT_TABLE_SIZE),(ft_uint8_t *)&Display_fontstruct,FT_GPU_FONT_TABLE_SIZE);

                #if defined(FT_81X_ENABLE)
		                /* Display hello world by offsetting wrt char size */
		                if ((iFontNumber + 16) > 31) {
			                Ft_Gpu_CoCmd_Dlstart(phost);

			                //this is a co-processor command and it needs to get pushed to the display list first.
			                Ft_Gpu_CoCmd_RomFont(phost, ((iFontNumber + 16) % 32), (iFontNumber + 16));

			                /* Download the commands into FIFIO */
			                Ft_App_Flush_Co_Buffer(phost);
			                /* Wait till coprocessor completes the operation */
			                Ft_Gpu_Hal_WaitCmdfifo_empty(phost);

			                //update the display list pointer for display list commands
			                Ft_DlBuffer_Index = Ft_Gpu_Hal_Rd16(phost, REG_CMD_DL);
		                }
                #endif

		                Ft_App_WrDlCmd_Buffer(phost, CLEAR(1, 1, 1)); // clear screen
		                Ft_App_WrDlCmd_Buffer(phost, COLOR_RGB(255, 255, 255)); // clear screen

		                /* Display string at the center of display */
		                Ft_App_WrDlCmd_Buffer(phost, BEGIN(BITMAPS));
		                hoffset = ((GLCD_X - 160) / 2);
		                voffset = ((GLCD_Y - Display_fontstruct.FontHeightInPixels) / 2);

		                //FT81X devices support larger rom fonts fonts in font handle 32, 33, 34
                #if defined(FT_81X_ENABLE)
		                /* Display hello world by offsetting wrt char size */
		                {
			                Ft_App_WrDlCmd_Buffer(phost, BITMAP_HANDLE((iFontNumber + 16) % 32));
		                }
                #else
		                /* Display hello world by offsetting wrt char size */
		                Ft_App_WrDlCmd_Buffer(phost, BITMAP_HANDLE((iFontNumber+16)));
                #endif
		                for (j = 0; j < stringlen; j++)
		                {
			                Ft_App_WrDlCmd_Buffer(phost, CELL(Display_string[j]));
			                Ft_App_WrDlCmd_Buffer(phost, VERTEX2F(hoffset*16,voffset*16));
			                hoffset += Display_fontstruct.FontWidth[Display_string[j]];
		                }
		                Ft_App_WrDlCmd_Buffer(phost, END());
		                Ft_App_WrDlCmd_Buffer(phost, DISPLAY() );

		                /* Download the DL into DL RAM */
		                Ft_App_Flush_DL_Buffer(phost);

		                /* Do a swap */
                        if (SAMAPP_GPU_DLSwap(DLSWAP_FRAME) != 0) {
                            next_delay = 0;
                        }
                    }
                    break;
                case 8:
                {
                  //ft_uint32_t NumBytesGen = 0;
                  //ft_uint16_t CurrWriteOffset = 0;

                    /*************************************************************************/
                    /* Below code demonstrates the usage of calibrate function. Calibrate    */
                    /* function will wait untill user presses all the three dots. Only way to*/
                    /* come out of this api is to reset the coprocessor bit.                 */
                    /*************************************************************************/
                    {

                        Ft_Gpu_CoCmd_Dlstart(phost);

                        Ft_App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(64, 64, 64));
                        Ft_App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
                        Ft_App_WrCoCmd_Buffer(phost, COLOR_RGB(0xff, 0xff, 0xff));
                        /* Draw number at 0,0 location */
                        //Ft_App_WrCoCmd_Buffer(phost,COLOR_A(30));
                        Ft_Gpu_CoCmd_Text(phost, (GLCD_X / 2), (GLCD_Y / 2), 27, OPT_CENTER, "Please Tap on the dot");
                        Ft_Gpu_CoCmd_Calibrate(phost, 0);

                        /* Download the commands into FIFIO */
                        Ft_App_Flush_Co_Buffer(phost); // this will start the calibration procedure so we wait for the completion event before continuing
                        next_delay = 0;
                        break;
                        /* Wait till coprocessor completes the operation */
                      //Ft_Gpu_Hal_WaitCmdfifo_empty(phost); - this is not necessary
                        /* Print the configured values */
                      //Ft_Gpu_Hal_RdMem(phost, REG_TOUCH_TRANSFORM_A, (ft_uint8_t *)TransMatrix, 4 * 6);//read all the 6 coefficients
                    }
                }
                    break;
                case 9:
                {
                    ft_uint32_t TransMatrix[6];
                    Ft_Gpu_Hal_RdMem(phost, REG_TOUCH_TRANSFORM_A, (ft_uint8_t *)TransMatrix, 4 * 6);//read all the 6 coefficients
                }
                    break;
                default:
                {
                    ft_int32_t wbutton, hbutton, tagval, tagoption;
                    ft_char8_t StringArray[100];
                    ft_uint32_t ReadWord;
                    ft_int16_t xvalue, yvalue, pendown;

                    /*************************************************************************/
                    /* Below code demonstrates the usage of touch function. Display info     */
                    /* touch raw, touch screen, touch tag, raw adc and resistance values     */
                    /*************************************************************************/
                    wbutton = GLCD_X / 8;
                    hbutton = GLCD_Y / 8;
                    {
                        CHAR *ptrString = StringArray;
                        Ft_Gpu_CoCmd_Dlstart(phost);
                        Ft_App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(64, 64, 64));
                        Ft_App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
                        Ft_App_WrCoCmd_Buffer(phost, COLOR_RGB(0xff, 0xff, 0xff));
                        Ft_App_WrCoCmd_Buffer(phost, TAG_MASK(0));
                        /* Draw informative text at width/2,20 location */
                        ptrString = uStrcpy(ptrString, "Touch Raw XY (");
                        ReadWord = Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_RAW_XY);
                        yvalue = (ft_int16_t)(ReadWord & 0xffff);
                        xvalue = (ft_int16_t)((ReadWord >> 16) & 0xffff);
                        ptrString = fnBufferDec(xvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ',';
                        ptrString = fnBufferDec(yvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ')';
                        *ptrString++ = '\0';
                        Ft_Gpu_CoCmd_Text(phost, GLCD_X / 2, 10, 26, OPT_CENTER, StringArray);

                        ptrString = StringArray;
                        ptrString = uStrcpy(ptrString, "Touch RZ (");
                        ReadWord = Ft_Gpu_Hal_Rd16(phost, REG_TOUCH_RZ);
                        ptrString = fnBufferDec(ReadWord, 0, ptrString);
                        *ptrString++ = ')';
                        *ptrString++ = '\0';
                        Ft_Gpu_CoCmd_Text(phost, GLCD_X / 2, 25, 26, OPT_CENTER, StringArray);

                        ptrString = StringArray;                        
                        ptrString = uStrcpy(ptrString, "Touch Screen XY (");
                        ReadWord = Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_SCREEN_XY);
                        yvalue = (ft_int16_t)(ReadWord & 0xffff);
                        xvalue = (ft_int16_t)((ReadWord >> 16) & 0xffff);
                        ptrString = fnBufferDec(xvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ',';
                        ptrString = fnBufferDec(yvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ')';
                        *ptrString++ = '\0';
                        Ft_Gpu_CoCmd_Text(phost, GLCD_X / 2, 40, 26, OPT_CENTER, StringArray);

                        ptrString = StringArray;
                        ptrString = uStrcpy(ptrString, "Touch TAG (");
                        ReadWord = Ft_Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
                        ptrString = fnBufferDec(ReadWord, 0, ptrString);
                        *ptrString++ = ')';
                        *ptrString++ = '\0';
                        Ft_Gpu_CoCmd_Text(phost, GLCD_X / 2, 55, 26, OPT_CENTER, StringArray);
                        tagval = ReadWord;

                        ptrString = StringArray;
                        ptrString = uStrcpy(ptrString, "Touch Direct XY (");
                        ReadWord = Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_DIRECT_XY);
                        yvalue = (ft_int16_t)(ReadWord & 0x03ff);
                        xvalue = (ft_int16_t)((ReadWord >> 16) & 0x03ff);
                        ptrString = fnBufferDec(xvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ',';
                        ptrString = fnBufferDec(yvalue, DISPLAY_NEGATIVE, ptrString);
                        pendown = (ft_int16_t)((ReadWord >> 31) & 0x01);
                        *ptrString++ = ',';
                        ptrString = fnBufferDec((ft_int32_t)pendown, 0, ptrString);
                        *ptrString++ = ')';
                        *ptrString++ = '\0';
                        Ft_Gpu_CoCmd_Text(phost, GLCD_X / 2, 70, 26, OPT_CENTER, StringArray);

                        ptrString = StringArray;
                        ptrString = uStrcpy(ptrString, "Touch Direct Z1Z2 (");
                        ReadWord = Ft_Gpu_Hal_Rd32(phost, REG_TOUCH_DIRECT_Z1Z2);
                        yvalue = (ft_int16_t)(ReadWord & 0x03ff);
                        xvalue = (ft_int16_t)((ReadWord >> 16) & 0x03ff);
                        ptrString = fnBufferDec(xvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ',';
                        ptrString = fnBufferDec(yvalue, DISPLAY_NEGATIVE, ptrString);
                        *ptrString++ = ')';
                        *ptrString++ = '\0';
                        Ft_Gpu_CoCmd_Text(phost, GLCD_X / 2, 85, 26, OPT_CENTER, StringArray);

                        Ft_Gpu_CoCmd_FgColor(phost, 0x008000);
                        Ft_App_WrCoCmd_Buffer(phost, TAG_MASK(1));
                        tagoption = 0;
                        if (12 == tagval)
                        {
                            tagoption = OPT_FLAT;
                        }

                        Ft_App_WrCoCmd_Buffer(phost, TAG(12));
                        Ft_Gpu_CoCmd_Button(phost, (GLCD_X / 4) - (wbutton / 2), (GLCD_Y * 2 / 4) - (hbutton / 2), wbutton, hbutton, 26, tagoption, "Tag12");
                        Ft_App_WrCoCmd_Buffer(phost, TAG(13));
                        tagoption = 0;
                        if (13 == tagval)
                        {
                            tagoption = OPT_FLAT;
                        }
                        Ft_Gpu_CoCmd_Button(phost, (GLCD_X * 3 / 4) - (wbutton / 2), (GLCD_Y * 3 / 4) - (hbutton / 2), wbutton, hbutton, 26, tagoption, "Tag13");

                        Ft_App_WrCoCmd_Buffer(phost, DISPLAY());
                        Ft_Gpu_CoCmd_Swap(phost);

                        /* Download the commands into fifo */
                        if (Ft_App_Flush_Co_Buffer(phost) != 0) {
                            next_delay = 0;
                            break;
                        }

                        /* Wait till coprocessor completes the operation */
                      //Ft_Gpu_Hal_WaitCmdfifo_empty(phost);
                        next_delay = (DELAY_LIMIT)(0.05 * SEC);
                    }
                }
                break;
                }
                if (next_delay != 0) {
                    uTaskerMonoTimer(OWN_TASK, next_delay, E_NEXT_PIC);
                }
            }
    #else
            {
                static int iCnt = 0;
                GLCD_PIC test_pic;
                test_pic.usX = 0;
                test_pic.usY = 0;
                test_pic.ucMode = (REDRAW | GIVE_ACK);
                switch (iCnt++) {                                        // cycle through some demonstration bitmap images
                case 0:                    
                    test_pic.ptrPic = (GBITMAP *)uTaskerPic1;
                    fnDoLCD_pic(&test_pic);                              // send the command to the GLCD task to draw the image
                    break;
                case 1:
                    test_pic.ptrPic = (GBITMAP *)uTaskerPic3;
                    fnDoLCD_pic(&test_pic);                              // send the command to the GLCD task to draw the image
                    break;
                case 2:
                    test_pic.ptrPic = (GBITMAP *)uTaskerPic2;
                    fnDoLCD_pic(&test_pic);                              // send the command to the GLCD task to draw the image
                    break;
                case 3:                                                  // {2}
                    {
                        #define ABOVE_LEFT_X   0
                        #define ABOVE_LEFT_Y   0
                        #define BOTTOM_RIGHT_X ((GLCD_X/CGLCD_PIXEL_SIZE) - 1)
                        #define BOTTOM_RIGHT_Y ((GLCD_Y/CGLCD_PIXEL_SIZE) - 1)
                        GLCD_LINE line1;// = {PAINT_LIGHT, {ABOVE_LEFT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, ABOVE_LEFT_Y}}; // first line from top left to top right
                        GLCD_LINE line2;// = {PAINT_LIGHT, {BOTTOM_RIGHT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y}}; // second line from top right to bottom right
                        GLCD_LINE line3;// = {PAINT_LIGHT, {BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y, ABOVE_LEFT_X, BOTTOM_RIGHT_Y}}; // third line from bottom right to bottom left
                        GLCD_LINE line4;// = {PAINT_LIGHT, {ABOVE_LEFT_X, BOTTOM_RIGHT_Y, ABOVE_LEFT_X, ABOVE_LEFT_Y}}; // fourth line from bottom left to top left
                        GLCD_LINE diagonal1;// = {PAINT_INVERTED, {ABOVE_LEFT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y}}; // draw a cross in the middle
                        GLCD_LINE diagonal2;// = {(PAINT_INVERTED), {BOTTOM_RIGHT_X, ABOVE_LEFT_Y, ABOVE_LEFT_X, BOTTOM_RIGHT_Y}};
                        GLCD_RECT_BLINK rect1;// = {(BLINKING_OBJECT | PAINT_INVERTED), {42, 54, 103, 68}, (DELAY_LIMIT)(0.10 * SEC)}; // draw a blinking rectangle
                        GLCD_RECT_BLINK rect2;// = {(BLINKING_OBJECT | PAINT_INVERTED), {82, 9, 100, 42},  (DELAY_LIMIT)(0.20 * SEC)}; // draw a blinking rectangle
                        GLCD_LINE_BLINK line5;//   = {(BLINKING_OBJECT | PAINT_INVERTED | REDRAW | GIVE_ACK), {8, 54, 41, 68},   (DELAY_LIMIT)(0.35 * SEC)}; // draw a blinking line
                        rect1.ucMode = (BLINKING_OBJECT | PAINT_INVERTED);
                        rect1.rect_corners.usX_start = 42;
                        rect1.rect_corners.usY_start = 54;
                        rect1.rect_corners.usX_end = 103;
                        rect1.rect_corners.usY_end = 68;
                        rect1.blink_half_period = (DELAY_LIMIT)(0.10 * SEC);
                        fnDoLCD_rect(&rect1);
                        rect2.ucMode = (BLINKING_OBJECT | PAINT_INVERTED);
                        rect2.rect_corners.usX_start = 82;
                        rect2.rect_corners.usY_start = 9;
                        rect2.rect_corners.usX_end = 100;
                        rect2.rect_corners.usY_end = 42;
                        rect2.blink_half_period = (DELAY_LIMIT)(0.20 * SEC);
                        fnDoLCD_rect(&rect2);
                        line1.ucMode = PAINT_LIGHT;
                        line1.line_start_end.usX_start = ABOVE_LEFT_X;
                        line1.line_start_end.usY_start = ABOVE_LEFT_Y;
                        line1.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        line1.line_start_end.usY_end = ABOVE_LEFT_Y;
                        fnDoLCD_line(&line1);
                        line2.ucMode = PAINT_LIGHT;
                        line2.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        line2.line_start_end.usY_start = ABOVE_LEFT_Y;
                        line2.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        line2.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&line2);
                        line3.ucMode = PAINT_LIGHT;
                        line3.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        line3.line_start_end.usY_start = BOTTOM_RIGHT_Y;
                        line3.line_start_end.usX_end = ABOVE_LEFT_X;
                        line3.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&line3);
                        line4.ucMode = PAINT_LIGHT;
                        line4.line_start_end.usX_start = ABOVE_LEFT_X;
                        line4.line_start_end.usY_start = BOTTOM_RIGHT_Y;
                        line4.line_start_end.usX_end = ABOVE_LEFT_X;
                        line4.line_start_end.usY_end = ABOVE_LEFT_Y;
                        fnDoLCD_line(&line4);
                        diagonal1.ucMode = PAINT_INVERTED;
                        diagonal1.line_start_end.usX_start = ABOVE_LEFT_X;
                        diagonal1.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal1.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        diagonal1.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal1);
                        diagonal2.ucMode = PAINT_INVERTED;
                        diagonal2.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        diagonal2.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal2.line_start_end.usX_end = ABOVE_LEFT_X;
                        diagonal2.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal2);
                        line5.ucMode = (BLINKING_OBJECT | PAINT_INVERTED | REDRAW | GIVE_ACK);
                        line5.line_start_end.usX_start = 8;
                        line5.line_start_end.usY_start = 54;
                        line5.line_start_end.usX_end = 41;
                        line5.line_start_end.usY_end = 68;
                        line5.blink_half_period = (DELAY_LIMIT)(0.35 * SEC);
                        fnDoLCD_line(&line5);
                    }
                    break;
                case 4:                                                  // {2}
                    {
                        GLCD_LINE diagonal1;// = {PAINT_INVERTED, {ABOVE_LEFT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y}}; // draw a cross in the middle
                        GLCD_LINE diagonal2;// = {(PAINT_INVERTED | REDRAW | GIVE_ACK), {BOTTOM_RIGHT_X, ABOVE_LEFT_Y, ABOVE_LEFT_X, BOTTOM_RIGHT_Y}};
                        GLCD_RECT_BLINK rect1;// = {(BLINKING_OBJECT | PAINT_LIGHT | REDRAW), {42, 54, 103, 68}, 0}; // cancel a blinking rectangle and paint it dark
                        GLCD_RECT_BLINK rect2;// = {(BLINKING_OBJECT | PAINT_DARK | REDRAW), {44, 56, 101, 66}, (DELAY_LIMIT)(1 * SEC)}; // restart a blinking rect and paint it dark
                        diagonal1.ucMode = PAINT_INVERTED;
                        diagonal1.line_start_end.usX_start = ABOVE_LEFT_X;
                        diagonal1.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal1.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        diagonal1.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal1);
                        diagonal2.ucMode = (PAINT_INVERTED | REDRAW | GIVE_ACK);
                        diagonal2.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        diagonal2.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal2.line_start_end.usX_end = ABOVE_LEFT_X;
                        diagonal2.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal2);
                        rect1.ucMode = (BLINKING_OBJECT | PAINT_LIGHT | REDRAW); // ensure object painted light
                        rect1.rect_corners.usX_start = 42;
                        rect1.rect_corners.usY_start = 54;
                        rect1.rect_corners.usX_end = 103;
                        rect1.rect_corners.usY_end = 68;
                        rect1.blink_half_period = 0;                     // stop this object blinking
                        fnDoLCD_rect(&rect1);
                        rect2.ucMode = (BLINKING_OBJECT | PAINT_DARK | REDRAW);
                        rect2.rect_corners.usX_start = 44;
                        rect2.rect_corners.usY_start = 56;
                        rect2.rect_corners.usX_end = 101;
                        rect2.rect_corners.usY_end = 66;
                        rect2.blink_half_period = (DELAY_LIMIT)(1 * SEC);
                        fnDoLCD_rect(&rect2);
                    }
                    break;
                case 5:                                                  // {2}
                    {
                        static GLCD_RECT rect = {(PAINT_INVERTED | REDRAW | GIVE_ACK), {12, 8, 12, 8}}; // draw a rect
                        fnDoLCD_rect(&rect);
                        if (++rect.rect_corners.usX_end < 60) {
                            rect.rect_corners.usY_end++;
                            iCnt = 5;
                        }
                        else {
                            rect.rect_corners.usX_end = 12;
                            rect.rect_corners.usY_end = 8;
                        }
                    }
                    break;
                case 6:                                                  // {4}
                case 7:
                    {
                        GLCD_SCROLL scroll;// = {(REDRAW | GIVE_ACK | KILL_BLINK_OBJECTS), 0, 9};
                        GLCD_TEXT_POSITION text_pos;// = {PAINT_LIGHT, X, Y, FONT_SIX_DOT};
                        scroll.ucMode = (REDRAW | GIVE_ACK | KILL_BLINK_OBJECTS);
                        scroll.sX_scroll = 0;
    #if defined BLAZE_K22
                        scroll.sY_scroll = 14;
    #else
                        scroll.sY_scroll = 9;
    #endif
                        fnDoLCD_scroll(&scroll);                        
                        text_pos.ucMode = (PAINT_LIGHT | REDRAW | GIVE_ACK);
                        text_pos.usX = 0;
    #if defined BLAZE_K22
                        text_pos.usY = ((GLCD_Y / CGLCD_PIXEL_SIZE) - 13);  // position on very bottom of display
                        text_pos.ucFont = FONT_NINE_DOT;
    #else
                        text_pos.usY = ((GLCD_Y / CGLCD_PIXEL_SIZE) - 8);  // position on very bottom of display
                        text_pos.ucFont = FONT_SIX_DOT;
    #endif
                        if (iCnt == 6) {
                            fnDoLCD_text(&text_pos, "Test of scrolling up");
                        }
                        else {
                          //static CHAR cTest[] = "abcdefghijklmnop"; declare as array to avoid newer versions of CodeWarrior from putting the string in flash
                            static CHAR cTest[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 0}; // {6}

                            int i = 0;
                            fnDoLCD_text(&text_pos, cTest);
                            while (i < 16) {
                                if (cTest[i] == 'z') {
                                    cTest[i] = 'a';
                                }
                                else {
                                    cTest[i]++;
                                }
                                i++;
                            }
                        }
                        iCnt = 6;
                    }
                    break;
                }
            }
    #endif
            break;
    #if (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC1766_STK || defined OLIMEX_LPC2478_STK || defined K70F150M_12M) && defined SDCARD_SUPPORT // {5}
        case E_NEXT_PHOTO:
            {
                static UTLISTDIRECTORY utListDirectory;                  // list directory object for a single user               
                if (utListDirectory.ptr_utDirObject == 0) {
                    utListDirectory.ptr_utDirObject = ptr_utDirectory;
                    utLocateDirectory(0, &utListDirectory);              // open a list referenced to the main directory
                }
                if (utReadDirectory(&utListDirectory, &utFileInfo) == UTFAT_END_OF_DIRECTORY) {
                    utListDirectory.ptr_utDirObject = 0;                 // end of the directory content
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0), E_NEXT_PHOTO);
                    break;
                }
                if (uMatchFileExtension(&utFileInfo, "BMP") == 0) {      // if BMP assume content is correct and display the new image
                    fnDisplayPhoto(1);
                }
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(5.0 * SEC), E_NEXT_PHOTO); // start a timer which is used to display various images   
            }
            break;
    #endif
#endif


// Application interrupt events for the LCD
//
#if defined HANDLE_LCD_INTERRUPT_EVENTS && defined SUPPORT_RTC && defined BLAZE_DIGITAL_WATCH
        case E_NEXT_PIC:                                                 // interrupt event requesting display of next time
            {
                GLCD_TEXT_POSITION text_pos;
                CHAR cTimeDateBuf[20];
                fnSetShowTime(DISPLAY_RTC_TIME, cTimeDateBuf);
                text_pos.ucMode = PAINT_LIGHT;
                text_pos.usX = 60;
                text_pos.usY = 90;
                text_pos.ucFont = FONT_EIGHTEEN_DOT;
                text_pos.ucMode = (PAINT_LIGHT);
                fnDoLCD_text(&text_pos, cTimeDateBuf);
                text_pos.usX = 75;
                text_pos.usY = 130;
                fnSetShowTime(DISPLAY_RTC_DATE, cTimeDateBuf);
                text_pos.ucFont = FONT_TEN_DOT;
                text_pos.ucMode = (PAINT_LIGHT | REDRAW);
                fnDoLCD_text(&text_pos, cTimeDateBuf);
            }
            break;
#endif

#if defined GLCD_MENU_TEST

static void fnDrawMenu(MENU_POSITION *ptrMenuPosition)
{
    #define START_MENU_X       20
    #define START_MENU_Y       2
    #define MENU_ENTRY_WIDTH   100
    #define MENU_ENTRY_HEIGHT  18
    #define MENUE_ENTRY_BORDER 3
    #define MENU_VERTICAL_GAP  1
    #define TEXT_LEFT_MARGIN   12
    #define TEXT_TOP_MARGIN    1
    #define MAX_ENTRIES        3

    const MENU_LOCATION *ptrPresentLocation = ptrMenuLocation;
    GLCD_TEXT_POSITION text_pos;
    GLCD_RECT box;
    const CHAR *ptrMenuText;
    unsigned char ucItemNumber = 0;

    box.rect_corners.usX_start = START_MENU_X;
    box.rect_corners.usX_end = (START_MENU_X + MENU_ENTRY_WIDTH);
    box.rect_corners.usY_start = START_MENU_Y;
    box.rect_corners.usY_end = (START_MENU_Y + MENU_ENTRY_HEIGHT);

    while (ucItemNumber < MAX_ENTRIES) {
        ptrMenuText = ptrPresentLocation->menuName;
        // Draw a box for the menu text
        //
        if (ptrMenuText == 0) {
            box.ucMode = PAINT_DARK;
            if (++ucItemNumber >= MAX_ENTRIES) {                         // if end of menu items at this level
                box.ucMode |= (REDRAW);                                  // the final box will cause a redraw
            }
            fnDoLCD_rect(&box);
        }
        else {
            box.ucMode = PAINT_LIGHT;
            fnDoLCD_rect(&box);
            if (ptrMenuPosition->ucMenuItem != ucItemNumber) {           // presently selected item is highlighted
                box.ucMode = PAINT_DARK;
                text_pos.ucMode = PAINT_LIGHT;
            }
            else {
                text_pos.ucMode = PAINT_DARK;
            }
            box.rect_corners.usX_start += MENUE_ENTRY_BORDER;
            box.rect_corners.usX_end -= MENUE_ENTRY_BORDER;
            box.rect_corners.usY_start += MENUE_ENTRY_BORDER;
            box.rect_corners.usY_end -= MENUE_ENTRY_BORDER;
            fnDoLCD_rect(&box);
            // Add the menu text
            //
            text_pos.ucFont = FONT_NINE_DOT;
            text_pos.usX = (box.rect_corners.usX_start + TEXT_LEFT_MARGIN);
            text_pos.usY = (box.rect_corners.usY_start + TEXT_TOP_MARGIN);

            if (ptrPresentLocation->ucOptions & OPTION_1) {
                GLCD_RECT Option;
                Option.ucMode = PAINT_LIGHT;
                Option.rect_corners.usX_start = (box.rect_corners.usX_end - 6);
                Option.rect_corners.usX_end = (box.rect_corners.usX_end - 1);
                Option.rect_corners.usY_start = (box.rect_corners.usY_start + 1);
                Option.rect_corners.usY_end = (box.rect_corners.usY_end - 1);
                fnDoLCD_rect(&Option);
            }
            if (ptrPresentLocation->ucOptions & OPTION_2) {
                GLCD_LINE Option;
                Option.ucMode = PAINT_LIGHT;
                Option.line_start_end.usX_start = (box.rect_corners.usX_end - 6);
                Option.line_start_end.usX_end = (box.rect_corners.usX_end - 1);
                Option.line_start_end.usY_start = (box.rect_corners.usY_start + 1);
                Option.line_start_end.usY_end = (box.rect_corners.usY_end - 1);
                fnDoLCD_line(&Option);
            }

            ptrPresentLocation++;                                        // move to next menu item

            if (++ucItemNumber >= MAX_ENTRIES) {                         // if end of menu items at this level
                text_pos.ucMode |= (REDRAW);                             // the final text will cause a redraw
            }
            fnDoLCD_text(&text_pos, ptrMenuText);
        }
        // Prepare for next menu entry
        //
        box.rect_corners.usX_start -= MENUE_ENTRY_BORDER;
        box.rect_corners.usX_end += MENUE_ENTRY_BORDER;
        box.rect_corners.usY_start += (1 + MENU_ENTRY_HEIGHT - MENUE_ENTRY_BORDER + MENU_VERTICAL_GAP);
        box.rect_corners.usY_end += (1 + MENU_ENTRY_HEIGHT + MENUE_ENTRY_BORDER + MENU_VERTICAL_GAP);
    }
}

static void fnDrawParameter(MENU_POSITION *ptrMenuPosition)
{
    PARAMETER *ptrPar = (PARAMETER *)((ptrMenuLocation + ptrMenuPosition->ucMenuItem)->ptrNextLevel);
    GLCD_RECT box;
    GLCD_TEXT_POSITION text_pos;
    CHAR cParValueBuffer[8];
    box.ucMode = PAINT_DARK;
    text_pos.ucMode = PAINT_LIGHT;
    box.rect_corners.usX_start = START_MENU_X;
    box.rect_corners.usX_end = (START_MENU_X + MENU_ENTRY_WIDTH);
    box.rect_corners.usY_start = START_MENU_Y;
    box.rect_corners.usY_end = (START_MENU_Y + ((MENU_ENTRY_HEIGHT + MENU_VERTICAL_GAP + 1) * 3));
    fnDoLCD_rect(&box);

    text_pos.ucFont = FONT_NINE_DOT;
    text_pos.usX = (START_MENU_X + 4);
    text_pos.usY = (START_MENU_Y + 20);
    fnDoLCD_text(&text_pos, ptrPar->title);

    text_pos.usX = (START_MENU_X + 55);
    fnBufferDec(*ptrPar->ptrVariable , 0, cParValueBuffer);              // convert the parameter value to a string
    fnDoLCD_text(&text_pos, cParValueBuffer);

    text_pos.ucFont = FONT_SEVEN_DOT;
    text_pos.ucMode = (PAINT_LIGHT | REDRAW);
    text_pos.usY += 3;
    text_pos.usX = (START_MENU_X + 80);
    fnDoLCD_text(&text_pos, ptrPar->unit);
}


static void fnHandleKey(MENU_POSITION *ptrMenuPosition, unsigned char ucKeyAction)
{
    switch (ucKeyAction) {
    case 14:                                                             // down key pressed
        switch (ptrMenuPosition->ucPresentFunction) {
        case PRESENT_FUNCTION_MENU:
            if ((ptrMenuLocation + ptrMenuPosition->ucMenuItem + 1)->menuName != 0) { // if not already at bottom
                ptrMenuPosition->ucMenuItem++;                           // move to next menu item
                fnDrawMenu(ptrMenuPosition);                             // redraw menu
            }
            break;
        case PRESENT_FUNCTION_PARAMETER:
            // Decrement present variable
            //
            {
                PARAMETER *ptrPar = (PARAMETER *)((ptrMenuLocation + ptrMenuPosition->ucMenuItem)->ptrNextLevel);
                if (*ptrPar->ptrVariable > ptrPar->ucValueMin) {
                    (*ptrPar->ptrVariable)--;
                    fnDrawParameter(ptrMenuPosition);
                }
            }
            break;
        }
        break;
    case 8:                                                              // up key pressed
        switch (ptrMenuPosition->ucPresentFunction) {
        case PRESENT_FUNCTION_MENU:
            if (ptrMenuPosition->ucMenuItem != 0) {                      // if not already at top
                ptrMenuPosition->ucMenuItem--;                           // move to previous menu item
                fnDrawMenu(ptrMenuPosition);                             // redraw menu
            }
            break;
        case PRESENT_FUNCTION_PARAMETER:
            // Increment present variable
            //
            {
                PARAMETER *ptrPar = (PARAMETER *)((ptrMenuLocation + ptrMenuPosition->ucMenuItem)->ptrNextLevel);
                if (*ptrPar->ptrVariable < ptrPar->ucValueMax) {
                    (*ptrPar->ptrVariable)++;
                    fnDrawParameter(ptrMenuPosition);
                }
            }
            break;
        }
        break;
    case 12:                                                             // enter key pressed
        switch (ptrMenuPosition->ucPresentFunction) {
        case PRESENT_FUNCTION_MENU:
            if ((ptrMenuLocation + ptrMenuPosition->ucMenuItem)->ptrNextLevel != 0) { // if there is a lower level (or parameter)
                if ((ptrMenuLocation + ptrMenuPosition->ucMenuItem)->ucOptions & DO_PARAMETER) { // move to a parameter setting rather than a sub-menu
                    ptrMenuPosition->ucPresentFunction = PRESENT_FUNCTION_PARAMETER;
                    fnDrawParameter(ptrMenuPosition);
                }
                else {
                    ptrParentMenu[ptrMenuPosition->ucMenuDepth] = ptrMenuLocation; // remember the higher level menu
                    ptrMenuLocation = (ptrMenuLocation + ptrMenuPosition->ucMenuItem)->ptrNextLevel; // move to next menu list
                    ptrMenuPosition->ucMenuDepth++;                      // mark the new level
                    ptrMenuPosition->ucMenuItem = 0;                     // start at the top of the new list
                    fnDrawMenu(ptrMenuPosition);                         // redraw menu
                }
            }
            break;
        case PRESENT_FUNCTION_PARAMETER:
            // Save present variable value
            //
            break;
        }
        break;
    case 10:                                                             // back up key pressed
        switch (ptrMenuPosition->ucPresentFunction) {
        case PRESENT_FUNCTION_MENU:
            if (ptrMenuPosition->ucMenuDepth != 0) {                     // if there is a higher level
                ptrMenuPosition->ucMenuDepth--;                          // mark the new level
                ptrMenuLocation = ptrParentMenu[ptrMenuPosition->ucMenuDepth]; // move up
                fnDrawMenu(ptrMenuPosition);                             // redraw menu
            }
            break;
        case PRESENT_FUNCTION_PARAMETER:
            ptrMenuPosition->ucPresentFunction = PRESENT_FUNCTION_MENU;  // move back to menu mode
            fnDrawMenu(ptrMenuPosition);                                 // redraw menu
            break;
        }
        break;
    }
}
#endif

// Message transmission routine used to send text and images to the LCD task - mono-GLCD
//
#if defined LCD_MESSAGE_ROUTINES && defined SUPPORT_OLED  && !defined GLCD_COLOR && !defined SUPPORT_TFT // {1}{3}
// Text message causing the GLCD to display the text at the defined position, using the defined font
//
extern void fnDoLCD_text(GLCD_TEXT_POSITION *text_pos, const CHAR *cText)
{
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_TEXT_MESSAGE)];
    unsigned char ucLength;

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_TEXT;                        // message type
    uMemcpy(&ucMessage[ MSG_CONTENT_COMMAND + 1], text_pos, sizeof(GLCD_TEXT_POSITION));
    ucLength = (unsigned char)((unsigned char *)uStrcpy((CHAR *)&ucMessage[MSG_CONTENT_COMMAND + 1 + sizeof(GLCD_TEXT_POSITION)], cText) - &ucMessage[ MSG_CONTENT_COMMAND + sizeof(GLCD_TEXT_POSITION)]);
    ucLength += sizeof(GLCD_TEXT_POSITION);
    ucMessage[MSG_CONTENT_LENGTH]   = ucLength;                          // message content length
    fnWrite( INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(ucLength + HEADER_LENGTH));// send message to defined task
}

// Image message causing the GLCD to display the image (pointer to image used) at the defined position
//
extern void fnDoLCD_pic(GLCD_PIC *pic)
{
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_PIC)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = (1 + sizeof(GLCD_PIC));            // message content length
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_PIC;                         // message type
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], pic, sizeof(GLCD_PIC));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + 1 + sizeof(GLCD_PIC))); // send message to defined task
}

// Message causing the GLCD to draw a line                               {2}
//
extern void fnDoLCD_line(void *line)
{
    int iLength = (sizeof(GLCD_LINE) + 1);
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_LINE_BLINK)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_LINE;                        // message type
    if (((GLCD_LINE *)line)->ucMode & (BLINKING_OBJECT)) {
        iLength += (sizeof(GLCD_LINE_BLINK) - sizeof(GLCD_LINE));        // extended command
    }
    ucMessage[MSG_CONTENT_LENGTH]   = iLength;                           // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], line, (iLength - 1));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + iLength)); // send message to defined task
}

// Message causing the GLCD to draw a filled rect                      {2}
//
extern void fnDoLCD_rect(void *rect)
{
    int iLength = (sizeof(GLCD_RECT) + 1);
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_RECT_BLINK)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_rect;                        // message type
    if (((GLCD_RECT *)rect)->ucMode & (BLINKING_OBJECT)) {               // extended command
        iLength += (sizeof(GLCD_RECT_BLINK) - sizeof(GLCD_RECT));
    }
    ucMessage[MSG_CONTENT_LENGTH]   = (unsigned char)iLength;            // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], rect, (iLength - 1));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + iLength)); // send message to defined task
}

// Message causing the display to scroll
//
extern void fnDoLCD_scroll(GLCD_SCROLL *scroll)
{
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_SCROLL)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_SCROLL;                      // message type
    ucMessage[MSG_CONTENT_LENGTH]   = (sizeof(GLCD_SCROLL) + 1);         // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], scroll, sizeof(GLCD_SCROLL));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + (sizeof(GLCD_SCROLL) + 1))); // send message to defined task
}
#endif

#if defined LCD_MESSAGE_ROUTINES && defined SUPPORT_RTC && defined BLAZE_DIGITAL_WATCH
static void _rtc_second_interrupt(void)                                  // seconds interrupt call-back handler
{
    if (iNotBlocked != 0) {                                              // do not request update when the LCD hasn't yet been initialsied
        fnInterruptMessage(OWN_TASK, E_NEXT_PIC);                        // schedule the task to display the time
    }
}
#endif

#if defined LCD_MESSAGE_ROUTINES && defined USE_USB_AUDIO && defined AUDIO_FFT && defined BLAZE_K22
// Display a scope and spectrum analyser
//
typedef struct _PACK stSCOPE_SCREEN
{
    unsigned char ucType;
    unsigned char ucWidth[2];
    unsigned char ucHeight[2];
    unsigned char content[0x74][232/8];
} SCOPE_SCREEN;

typedef struct _PACK stFFT_SCREEN
{
    unsigned char ucType;
    unsigned char ucWidth[2];
    unsigned char ucHeight[2];
    unsigned char content[0x74][232/8];
} FFT_SCREEN;

static FFT_SCREEN scope = {
    0x00,
    {0,232}, {0x00,0x74},
    {{0x00}}
};

static SCOPE_SCREEN real_scope = {
    0x00,
    {0,232}, {0x00,0x74},
    {{0x00}}
};

extern void fnDisplayFFT(float *fft_magnitude_buffer, signed short *sRawInput)
{
  //#define HALF_SCREEN                                                  // display only half of the spectrum
    static int iScope = 0;
    unsigned char ucTemp;
    int iSpike;
    int bin;
    unsigned char ucBit;
    GLCD_PIC test_pic;

    if (iNotBlocked == 0) {                                              // ignore in case the display is still busy
        iNotBlocked++;
        return;
    }
    iNotBlocked = 0;                                                     // block further until draw has been confirmed
    test_pic.usX = 1;
    test_pic.usY = 124;
    test_pic.ucMode = (PAINT_LIGHT | REDRAW | GIVE_ACK);
    test_pic.ptrPic = (GBITMAP *)&scope;
    uMemset(scope.content, 0, sizeof(scope.content));
    #if defined _WINDOWS
    if (++iScope >= 1)
    #else
    if (++iScope >= 20)                                                  // redraw the oscilloscope signal periodically
    #endif
    {
        static int line = 0;
        GLCD_PIC scope_pic;
    #if defined VARIABLE_PIXEL_COLOUR
        GLCD_STYLE graphic_style;
    #endif
        int iTime;
        signed short temp1;
        unsigned short temp2;
        scope_pic.usX = 1;
        scope_pic.usY = 1;
    #if defined VARIABLE_PIXEL_COLOUR
        scope_pic.ucMode = (PAINT_LIGHT | REDRAW);
    #else
        scope_pic.ucMode = (PAINT_LIGHT);
    #endif
        scope_pic.ptrPic = (GBITMAP *)&real_scope;
        uMemset(real_scope.content, 0, sizeof(real_scope.content));
        iScope = 0;
        ucBit = 0x80;
        for (iTime = 0; iTime < 232; iTime++) {
            temp1 = (*sRawInput / (0xffff / 0x74));                      // scale
            temp2 = temp1 + (0x74 / 2);                                  // shift to center
            if (temp2 > (unsigned short)0x73) {
                temp2 = (unsigned short)0x73;
            }
            sRawInput++;
            real_scope.content[temp2][(iTime / 8)] |= ucBit;
            if (++line >= 0x74) {
                line = 0;
            }
            ucBit >>= 1;
            if (ucBit == 0) {
                ucBit = 0x80;
            }
        }
        graphic_style.ucMode = STYLE_BACKGROUND_COLOR;
        graphic_style.color = (COLORREF)RGB(0, 0, 0);                    // black background
        fnDoLCD_style(&graphic_style);
        graphic_style.ucMode = STYLE_PIXEL_COLOR;
        graphic_style.color = (COLORREF)RGB(255, 255, 255);              // white pixels
        fnDoLCD_style(&graphic_style);
        fnDoLCD_pic(&scope_pic);                                         // send the command to the GLCD task to draw the image
        graphic_style.color = LCD_PIXEL_COLOUR;                          // set original pixels again
        fnDoLCD_style(&graphic_style);
        graphic_style.ucMode = STYLE_BACKGROUND_COLOR;
        graphic_style.color = LCD_ON_COLOUR;                             // set original background again
        fnDoLCD_style(&graphic_style);
    }
    #if defined HALF_SCREEN
    ucBit = 0xc0;
    for (bin = 0; bin < 232; bin += 2)
    #else
    ucBit = 0x80;
    for (bin = 0; bin < 240; bin += 1)
    #endif
    {
        ucTemp = (unsigned char)(*fft_magnitude_buffer++ / (200000 / 40));
        if (ucTemp > 0x73) {
            ucTemp = 0x73;
        }
        for (iSpike = 0x73; iSpike >= 0x73 - ucTemp; iSpike--) {
            scope.content[iSpike][(bin / 8)] |= ucBit;
        }
    #if defined HALF_SCREEN
        ucBit >>= 2;
        if (ucBit == 0) {
            ucBit = 0xc0;
        }
    #else
        ucBit >>= 1;
        if (ucBit == 0) {
            ucBit = 0x80;
        }
    #endif
    }
    fnDoLCD_pic(&test_pic);                                              // send the command to the GLCD task to draw the image
}
#endif


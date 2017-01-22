/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      KeyPadSim.cpp
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    02.11.2009 Remove pressed key filtering and add display of keypad LEDs {1}
    01.05.2010 Add local define _EXCLUDE_WINDOWS_
    17.03.2012 Add optional KEYPAD_KEY_DEFINITIONS support               {2}
    26.04.2012 Add optional BUTTON_KEY_DEFINITIONS support               {3}
    05.08.2012 Allow user multiple user keys to be pressed with a single click (useful for joystick type switches) {4}
    08.10.2012 Remove requirement for KEYPAD_LEDS to be defined          {5}
    09.10.2012 Handle simultaneous release of all buttons                {6}
    09.10.2012 Correct toggling of held buttons with right mouse click   {7}
    22.01.2013 Increase windows by 50px to ensure front panels have adequate room {8}
    09.04.2014 Display port details when hovering over board inputs      {9}

*/

#include <windows.h>
#define _EXCLUDE_WINDOWS_
#include "config.h"

#if defined SUPPORT_KEY_SCAN || defined KEYPAD || defined BUTTON_KEY_DEFINITIONS

extern void fnDisplayKeypadLEDs(HDC hdc);
extern void fnSetLastPort(int iInputLastPort, int iInputPortBit);        // {9}

#if !defined KEYPAD && defined SUPPORT_KEY_SCAN
    #define KEYPAD "keypad.bmp"
    #define DEFAULT_KEYPAD
#endif

extern HWND ghWnd;

static int cxDib, cyDib;
static RECT kb_rect;
static BITMAPFILEHEADER *pbmfh = 0;
#if defined LCD_ON_KEYPAD
    static BITMAPFILEHEADER *pbmfh_backup = 0;
#endif
static BITMAPINFO *pbmi = 0;
static BYTE *pBits;
#if defined SUPPORT_KEY_SCAN
    #if KEY_COLUMNS == 0
        #undef KEY_COLUMNS
        #define KEY_COLUMNS  VIRTUAL_KEY_COLUMNS
        #undef KEY_ROWS
        #define KEY_ROWS VIRTUAL_KEY_ROWS
    #endif
    static int iKeyStates[KEY_COLUMNS][KEY_ROWS] = {0};
#endif

#if defined KEYPAD_KEY_DEFINITIONS                                       // {2}
typedef struct stUSER_KEY
{
    int        iColumn;
    int        iRow;
    RECT       key_area;
} USER_KEY;


USER_KEY user_keys[] = {
    KEYPAD_KEY_DEFINITIONS
};
#endif

#if defined BUTTON_KEY_DEFINITIONS                                       // {3}
    typedef struct stUSER_BUTTON
    {
        unsigned long  Port_Ref;
        unsigned long  Port_Bit;
        RECT       button_area;
    } USER_BUTTON;


    USER_BUTTON user_buttons[] = {
        BUTTON_KEY_DEFINITIONS
    };

    static int iUserButtonStates[(sizeof(user_buttons)/sizeof(USER_BUTTON))] = {0};
#endif


extern void fnInitKeyPad(RECT &rt, int iOrigWidth, int iMaxTop)
{
#if defined KEYPAD
    HANDLE hFile;
	DWORD dwFileSize, dwHighSize, dwBytesRead;	                         // load a picture of the keypad
    hFile = CreateFile(KEYPAD, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        dwFileSize = GetFileSize(hFile, &dwHighSize);
	    if (pbmfh = (BITMAPFILEHEADER *)malloc(dwFileSize)) {
		    ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
	    }
	    CloseHandle (hFile);
	    pbmi = (BITMAPINFO *)(pbmfh + 1);
	    pBits = (BYTE *)pbmfh + pbmfh->bfOffBits;
	    if (pbmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER)) {
		    cxDib = ((BITMAPCOREHEADER *)pbmi)->bcWidth;
		    cyDib = ((BITMAPCOREHEADER *)pbmi)->bcHeight;
	    }
	    else {
		    cxDib = pbmi->bmiHeader.biWidth;
		    cyDib = abs(pbmi->bmiHeader.biHeight);
	    }
    }

    if ((iOrigWidth + cxDib + 20) >  rt.right) {                         // adjust main windows size to suit
        rt.right = (iOrigWidth + cxDib + 20);
    }
    if ((iMaxTop + cyDib + 125) > rt.bottom) {                           // {8}
        rt.bottom = (iMaxTop + cyDib + 125);
    }
#endif
}


extern void DisplayKeyPad(HWND hwnd, RECT rt, RECT refresh_rect)
{
#if defined KEYPAD_KEY_DEFINITIONS
    int i, j;
#elif defined SUPPORT_KEY_SCAN
    int x, y, i, j, iKeyWidth, iKeyHeight, iKeySizeX, iKeySizeY;
#elif defined BUTTON_KEY_DEFINITIONS
    int i;
#endif
    if (NULL == hwnd) {
        return;
    }

    HDC hdc = GetDC(hwnd);                                               // get the devie context
    
    if (pbmfh != 0) {                                                    // if there is a bit map for a front panel/keypad
#if defined KEYPAD_LED_DEFINITIONS                                       // {5}
        extern void fnConfigureKeypad_leds(RECT kb_rect);
#endif
		SetDIBitsToDevice(hdc, rt.left, rt.top, cxDib, cyDib, 0,0,0, cyDib, pBits, pbmi, DIB_RGB_COLORS); // draw the bitmap
#if defined LCD_ON_KEYPAD
        pbmfh_backup = pbmfh;
        pbmfh = 0;
#endif
        kb_rect.left = rt.left;                                          // update the keypad coordinates
        kb_rect.top  = rt.top;
        kb_rect.right = kb_rect.left + cxDib;
        kb_rect.bottom = rt.top + cyDib;
#if defined KEYPAD_LED_DEFINITIONS                                       // {1}{5}
        fnConfigureKeypad_leds(kb_rect);
#endif
	}

  //if ((refresh_rect.right < rt.left) || (refresh_rect.bottom < rt.top)) return; // {1}
#if defined SUPPORT_KEY_SCAN
    #if defined KEYPAD_KEY_DEFINITIONS                                   // {2}
    for (i = 0; i < KEY_COLUMNS; i++) {                                  // for each column
        for (j = 0; j < KEY_ROWS; j++) {                                 // for each row    
            if (iKeyStates[i][j]) {                                      // if key is presently pressed
                int iKey = 0;
                while (iKey < (sizeof(user_keys)/sizeof(USER_KEY))) {    // for each possible key
                    if ((user_keys[iKey].iColumn == i) && (user_keys[iKey].iRow == j)) {
                        MoveToEx(hdc, (user_keys[iKey].key_area.left + kb_rect.left), (user_keys[iKey].key_area.top + kb_rect.top), NULL);
                        LineTo(hdc, (user_keys[iKey].key_area.right + kb_rect.left), (user_keys[iKey].key_area.top + kb_rect.top));
                        LineTo(hdc, (user_keys[iKey].key_area.right + kb_rect.left), (user_keys[iKey].key_area.bottom + kb_rect.top));
                        LineTo(hdc, (user_keys[iKey].key_area.left + kb_rect.left), (user_keys[iKey].key_area.bottom + kb_rect.top));
                        LineTo(hdc, (user_keys[iKey].key_area.left + kb_rect.left), (user_keys[iKey].key_area.top + kb_rect.top));
                        break;
                    }
                    iKey++;
                }
            }
        }
    }
    #else                                                                // the front panel is automatically divided into columns and rows
    iKeyWidth = cxDib / KEY_COLUMNS;                                     // display pressed keys
    iKeyHeight = cyDib / KEY_ROWS;
    iKeySizeX = (iKeyWidth/4);
    iKeySizeY = (iKeyHeight/ 4);
    for (i = 0; i < KEY_COLUMNS; i++) {
        x = kb_rect.left + iKeyWidth/2 + i*iKeyWidth;                    // mid point of each key
        y = kb_rect.top + iKeyHeight/2;
        for (j = 0; j < KEY_ROWS; j++) {
            if (iKeyStates[i][j]) {                                      // if key presently pressed
                MoveToEx (hdc, x - iKeySizeX, y - iKeySizeY, NULL);
                LineTo(hdc, x + iKeySizeX, y - iKeySizeY);
                LineTo(hdc, x + iKeySizeX, y + iKeySizeY);
                LineTo(hdc, x - iKeySizeX, y + iKeySizeY);
                LineTo(hdc, x - iKeySizeX, y - iKeySizeY);
            }
            y += iKeyHeight;
        }
    }
    #endif
#endif
#if defined BUTTON_KEY_DEFINITIONS                                       // {3}
    for (i = 0; i < (sizeof(user_buttons)/sizeof(USER_BUTTON)); i++) {
        if (iUserButtonStates[i] != 0) {                                 // if presently pressed
            MoveToEx(hdc, (user_buttons[i].button_area.left + kb_rect.left), (user_buttons[i].button_area.top + kb_rect.top), NULL);
            LineTo(hdc, (user_buttons[i].button_area.right + kb_rect.left), (user_buttons[i].button_area.top + kb_rect.top));
            LineTo(hdc, (user_buttons[i].button_area.right + kb_rect.left), (user_buttons[i].button_area.bottom + kb_rect.top));
            LineTo(hdc, (user_buttons[i].button_area.left + kb_rect.left), (user_buttons[i].button_area.bottom + kb_rect.top));
            LineTo(hdc, (user_buttons[i].button_area.left + kb_rect.left), (user_buttons[i].button_area.top + kb_rect.top));
        }
    }
#endif
#if defined KEYPAD_LED_DEFINITIONS                                       // {1}{5}
    fnDisplayKeypadLEDs(hdc);
#endif
    ReleaseDC(hwnd, hdc);                                                // release the device context
}


#if defined SUPPORT_KEY_SCAN
static void fnKeyDetected(int iColumn, int iRow,int iPressRelease)
{
    if (iPressRelease == 3) {                                            // ignore hover
        return;
    }
    // The new state is entered into the keypad structure
    //
    if (iPressRelease == 2) {
        iKeyStates[iColumn][iRow] ^= 1;                                  // toggle present state
    }
    else {
        iKeyStates[iColumn][iRow] = iPressRelease;                       // set on or off
    }
}

extern void fnsetKeypadState(char **ptr)
{
    *ptr++ = (char *)&iKeyStates;
}
#endif

extern int fnCheckKeypad(int x, int y, int iPressRelease)
{
#if defined KEYPAD_KEY_DEFINITIONS
    int iKey = 0;
#elif defined SUPPORT_KEY_SCAN
    int iRow, iColumn = 0;
    int iSizeX, iSizeY;
#endif
#if defined BUTTON_KEY_DEFINITIONS
    int i;
#endif

    if (x < 0) {
#if defined SUPPORT_KEY_SCAN
        memset(iKeyStates, 0, sizeof(iKeyStates));
#endif
#if defined BUTTON_KEY_DEFINITIONS                                       // {6}
    for (i = 0; i < (sizeof(user_buttons)/sizeof(USER_BUTTON)); i++) {
        if (iUserButtonStates[i] != 0) {                                 // originally pressed
            extern void fnInjectInputChange(unsigned long ulPortRef, unsigned long ulPortBit, int iAction);
            iUserButtonStates[i] = 0;
            fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE); // cause the input to toggle
        }
    }
#endif
		InvalidateRect(ghWnd, &kb_rect, FALSE);
        return KEY_CHANGED;
    }

    // Check whether the mouse was on a key (first it has to be within the keypad/front panel area)
    //
    if (x < kb_rect.left) {
        return 0;
    }
    if (y < kb_rect.top) {
        return 0;
    }
    if (x > kb_rect.right) {
        return 0;
    }
    if (y > kb_rect.bottom) {
        return 0;
    }
//  if (iPressRelease > 2) {                                             // {9} removed to allow hover location to be detected
//      return 1;                                                        // check on whether in keypad area
//  }

    x -= kb_rect.left;
    y -= kb_rect.top;

#if defined BUTTON_KEY_DEFINITIONS                                       // {3}
    for (i = 0; i < (sizeof(user_buttons)/sizeof(USER_BUTTON)); i++) {
        if (user_buttons[i].button_area.left <= x) {
            if (user_buttons[i].button_area.right >= x) {
                if (user_buttons[i].button_area.top <= y) {
                    if (user_buttons[i].button_area.bottom >= y) {
                        extern void fnInjectInputChange(unsigned long ulPortRef, unsigned long ulPortBit, int iAction);
                        if (iPressRelease == 3) {                        // hovering over an input
                            fnSetLastPort((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit); // {9} cause connected port details to be displayed
                            return 1;
                        }
                        if (iPressRelease != 0) {
                            if (iPressRelease == 2) {                    // {7} toggle
                                if (iUserButtonStates[i] != 0) {         // already held
                                    iUserButtonStates[i] = 0;
                                    fnInjectInputChange((user_buttons[i].Port_Ref & 0x3fffffff), user_buttons[i].Port_Bit, INPUT_TOGGLE);
                                    continue;
                                }
                            }
                            iUserButtonStates[i] = 1;
                            switch (user_buttons[i].Port_Ref & (ANALOGUE_SWITCH_INPUT | POSITIVE_SWITCH_INPUT)) {
                            case 0:
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_NEG); // force button to low state
                                break;
                            case ANALOGUE_SWITCH_INPUT:
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_NEG_ANALOG); // force button to low state (large analog swing)
                                break;
                            case POSITIVE_SWITCH_INPUT:
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_POS); // force button to high state
                                break;
                            case (ANALOGUE_SWITCH_INPUT | POSITIVE_SWITCH_INPUT):
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_POS_ANALOG); // force button to high state (large analog swing)
                                break;
                            }                            
                        }
                        else {
                            iUserButtonStates[i] = 0;
                            switch (user_buttons[i].Port_Ref & (ANALOGUE_SWITCH_INPUT | POSITIVE_SWITCH_INPUT)) {
                            case 0:
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE); // toggle button state
                                break;
                            case POSITIVE_SWITCH_INPUT:
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_NEG); // toggle button state
                                break;
                            case ANALOGUE_SWITCH_INPUT:
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_ANALOG); // toggle button state (large analog swing)
                                break;
                            case (ANALOGUE_SWITCH_INPUT | POSITIVE_SWITCH_INPUT):
                                fnInjectInputChange((user_buttons[i].Port_Ref & SWITCH_PORT_REF_MASK), user_buttons[i].Port_Bit, INPUT_TOGGLE_NEG_ANALOG); // toggle button state (large analog swing)
                                break;
                            }  
                        }
		                InvalidateRect(ghWnd, &kb_rect, FALSE);
                      //break;                                           // {4} don't break when first area is detected so that multiple keys could be actuated
                    }
                }
            }
        }
    }
#endif
#if defined SUPPORT_KEY_SCAN
    #if defined KEYPAD_KEY_DEFINITIONS                                   // {2}
    while (iKey < (sizeof(user_keys)/sizeof(USER_KEY))) {                // for each possible key
        if (user_keys[iKey].key_area.left <= x) {
            if (user_keys[iKey].key_area.right >= x) {
                if (user_keys[iKey].key_area.top <= y) {
                    if (user_keys[iKey].key_area.bottom >= y) {
                        fnKeyDetected(user_keys[iKey].iColumn, user_keys[iKey].iRow, iPressRelease);
		                InvalidateRect(ghWnd, &kb_rect, FALSE);
                        return KEY_CHANGED;
                    }
                }
            }
        }
        iKey++;
    }
    #else
    iSizeX = cxDib/KEY_COLUMNS;
    while (iColumn < KEY_COLUMNS) {                                      // which key is it ?
        if (x < iSizeX) {                                                // row found
            iRow = 0;
            iSizeY = cyDib/KEY_ROWS;
            while (iRow < KEY_ROWS) {
                if (y < iSizeY) {                                        // column found too
                    fnKeyDetected(iColumn, iRow, iPressRelease);
		            InvalidateRect(ghWnd, &kb_rect, FALSE);
                    return KEY_CHANGED;
                }
                iRow++;
                iSizeY += cyDib/KEY_ROWS;
            }
        }
        iColumn++;
        iSizeX += cxDib/KEY_COLUMNS;
    }
    #endif
#endif
    if (iPressRelease == 3) {                                            // {9} no button is matched so remove port details
        fnSetLastPort(-1, 0);                                            // no longer on an input
        return 1;
    }
    return 0;
}

#endif

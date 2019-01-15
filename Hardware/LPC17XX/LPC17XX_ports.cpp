/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC17XX_ports.cpp
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    14.11.2011 Add LPC1788 support

*/

#include "config.h"

#ifdef _LPC17XX

#define BUF1SIZE 200

#if _VC80_UPGRADE>=0x0600
  #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
  #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
  #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
  #define STRCPY strcpy
  #define SPRINTF sprintf
  #define STRCAT strcat
#endif


#ifdef LPC1788
    #include "LPC1788_port.h"
#else
    #include "LPC17XX_port.h"
#endif



static unsigned char *_ptrPerFunctions;

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
#ifdef LPC1788
    int i;
#endif
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE+1];

    switch (iPort) {
    case _PORT0:
        STRCPY(cPortDetails, "Port 0");
        break;
    case _PORT1:
        STRCPY(cPortDetails, "Port 1");
        break;
    case _PORT2:
        STRCPY(cPortDetails, "Port 2");
        break;
    case _PORT3:
        STRCPY(cPortDetails, "Port 3");
        break;
    case _PORT4:
        STRCPY(cPortDetails, "Port 4");
        break;
#ifdef LPC1788
    case _PORT5:
        STRCPY(cPortDetails, "Port 5");
        break;
#endif
    default:
        STRCPY(cPortDetails, "?????");
        return;
    }

    iBit = iPortLength - iBit - 1;                                       // bit position
    if (iBit < 0) {
        return;
    }
    SPRINTF(cBuf, " Bit %i Pin: ", iBit);
    STRCAT(cPortDetails, cBuf);
#ifdef LPC1788
    if (*cPinNumber[iPort][iBit][_PIN_COUNT] == '-') {
        return;
    }
    STRCPY(cBuf, cPinNumber[iPort][iBit][_PIN_COUNT]);
    STRCAT(cPortDetails, cBuf);
    STRCAT(cPortDetails, " {");
    STRCAT(cPortDetails, cPer[iPort][iBit][0]);

    for (i = 1; i <= ALTERNATIVE_FUNCTIONS; i++) {
        if (*cPer[iPort][iBit][i] != '-') {
            STRCAT(cPortDetails, "/");
            STRCAT(cPortDetails, cPer[iPort][iBit][i]);
        }
    }
    STRCAT(cPortDetails, "} ");

    if (ulPortPeripheral[iPort] & (0x01 << iBit)) {
        unsigned char *ptrList = _ptrPerFunctions;
        int _iPort = iPort;
        int _iBit = iBit;
        while (_iPort--) {
            ptrList += (PORT_WIDTH);
        }
        while (_iBit--) {
            ptrList++;
        }
        if (*ptrList > ALTERNATIVE_FUNCTIONS) {
            STRCPY(&cBuf[0], "??");
        }
        else {
            if (*cPer[iPort][iBit][*ptrList] == '-') {
                STRCPY(&cBuf[0], "INVALID");
            }
            else {
                STRCPY(&cBuf[0], cPer[iPort][iBit][*ptrList]);
            }
        }
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
    }
#else
    STRCPY(cBuf, cPinNumber[iPort][iBit]);
    STRCAT(cPortDetails, cBuf);
    if (!strcmp(cBuf, "NA")) {
        return;
    }

    if (ulPortPeripheral[iPort] & (0x01 << iBit)) {
        unsigned char *ptrList = _ptrPerFunctions;
        int _iPort = iPort;
        int _iBit = iBit;
        while (_iPort--) {
            ptrList += (PORT_WIDTH);
        }
        while (_iBit--) {
            ptrList++;
        }
        cBuf[0] = ' ';
        if (*ptrList > 4) {
            STRCPY(&cBuf[1], "??");
        }
        else {
            STRCPY(&cBuf[1], cPer[iPort][iBit][*ptrList]);
        }
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
    }
#endif
}

#ifdef LPC1788
extern unsigned long fnGetPortMask(int iPortNumber)
{
    unsigned long ulPortMask = 0x00000000;
    unsigned long ulMaskBit = 0x00000001;
    int i = 0;
    for (i = 0; i < PORT_WIDTH; i++) {
        if (*cPinNumber[iPortNumber][i][_PIN_COUNT] == '-') {
            ulPortMask |= ulMaskBit;
        }
        ulMaskBit <<= 1;
    }
    return ulPortMask;
}
#endif


// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
    _ptrPerFunctions = (unsigned char *)hw_table; 
}
#endif

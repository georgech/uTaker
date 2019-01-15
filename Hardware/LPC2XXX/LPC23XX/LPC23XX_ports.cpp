/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC23XX_ports.c
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    08.10.2009 Add note that Port 0-31 is output only on LPC2101..03     {1} [removed with 2 since tests show it can operate as input]
    07.11.2009 Add note that Ports 0-2 and 0-3 are open collector when LPC2101..03 / 27..28 for others {2}
    30.11.2009 Add note that Ports 31 is recommended for output only, with pull-up LPC2101..03 {3}
    23.02.2010 Add LPC214x                                               {4}
    30.01.2011 Add LPC213x                                               {5}

*/

#include "config.h"

#ifdef _LPC23XX

#define BUF1SIZE 200

#if _VC80_UPGRADE>=0x0600
    #define STRCPY(a, b)     strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b)     strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define STRCAT strcat
#endif

#if defined _LPC24XX
    #include "LPC24XX_port.h"
#elif defined _LPC21XX                                                   // LPC21XX
    #if defined _LPC214X || defined _LPC213X                             // {4}{5} LPC214x/LPC213x
        #include "LPC214X_port.h"
    #else
        #include "LPC21XX_port.h"
    #endif
#else                                                                    // LPC23XX
    #include "LPC23XX_port.h"
#endif



static unsigned char *_ptrPerFunctions;

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE + 1];

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
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        if ((iBit == 2) || (iBit == 3)) {                                // {2}
            STRCAT(cPortDetails, " [WARNING - Open Collector]");
        }
    #endif
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
            if ((iBit == 2) || (iBit == 3)) {                            // {2}
                STRCAT(cPortDetails, " Output [WARNING - Open Collector]");
                return;
            }
            if (iBit == 31) {                                            // {3}
                STRCAT(cPortDetails, " Output [Recomm. output only, with pull-up]");
                return;
            }
    #elif defined _LPC214X                                               // {4}
            if ((iPort == 0) && (iBit == 31)) {
                STRCAT(cPortDetails, " Output [Recomm. output only]");
                return;
            }
    #else
            if ((iBit == 27) || (iBit == 28)) {                          // {2}
                STRCAT(cPortDetails, " Output [WARNING - Open Collector]");
                return;
            }
    #endif
            STRCAT(cPortDetails, " Output");
        }
        else {
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
            if (iBit == 31) {                                            // {3}
                STRCAT(cPortDetails, " Input [Recomm. output only, with pull-up!!]");
            }
    #elif defined _LPC214X                                               // {4}
            if ((iPort == 0) && (iBit == 31)) {
                STRCAT(cPortDetails, " Input [Recomm. output only]");
                return;
            }
    #endif
            STRCAT(cPortDetails, " Input");
        }
    }
}

// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
    _ptrPerFunctions = (unsigned char *)hw_table; 
}
#endif

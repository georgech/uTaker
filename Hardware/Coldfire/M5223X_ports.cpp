/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5223X_ports.c (supports also M5221X/M5222X)
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    09.09.2007 Add M5222X support (_M5222X)
    22.09.2007 Add M5221X support (_M5221X)
    28.04.2008 Add ADC simulation support                                {1}
    23.05.2008 Correct Port UC for chips with less than 100 pins         {2}
    06.06.2008 Use SPRINTF() instead of sprintf()                        {3}
    07.06.2008 Correct some M5221X details                               {4}
    07.06.2008 Correct AS port length for M5222X and M5221X              {5}
    17.06.2008 Remove TA port for 64 pin M5222X and M5221X               {6}
    13.08.2008 Remove details of ports to external header and add M5225X {7}
    27.09.2009 Correct Port DD on 80pin devices                          {8}
    25.03.2009 Add M521XX configuration
    26.03.2009 Add M521X configuration                                   {9}
    24.09.2009 Correct TA on 64 pin Add M521XX                           {10}
    10.04.2010 Add M5214, M5216, M5280, M5281 and M5282                  {11}
    21.04.2010 Add M520X                                                 {12}
    11.06.2011 Correct TA on M521x                                       {13}
    17.06.2011 Add M523X                                                 {14}

*/

#include "config.h"

#ifdef _M5223X

#define BUF1SIZE 100

#if _VC80_UPGRADE>=0x0600
    #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define STRCAT strcat
#endif

#if defined _M5222X || defined _M5221X || defined _M521XX
    #include "M5222X_M5221X_port.h"
#elif defined _M520X                                                     // {12}
    #include "M520X.h"
#elif defined _M52XX_SDRAM                                               // {11}
    #include "M52XX_SDRAM.h"
#elif defined _M521X                                                     // {9}
    #include "M521X_port.h"
#elif defined _M5225X
    #include "M5225X_port.h"                                             // {7}
#elif defined _M523X
    #include "M523X_port.h"                                              // {14}
#else
    #include "M5223X_port.h"
#endif                                                                   // end M5223X

static unsigned char *_ptrPerFunctions;
#ifdef SUPPORT_ADC                                                       // {1}
    typedef struct stPorts
    {
        unsigned char *ports;
        unsigned short * adcs;
    } PORTS;

    static unsigned short *_ptrADC;
#endif

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE+1];
#ifdef SUPPORT_ADC
    int iAdc = -1;                                                       // {1}
#endif
    switch (iPort) {
#ifdef _M52XX_SDRAM                                                      // {5}
    case _PORT_A:
        STRCPY(cPortDetails, "Port A");
        break;
    case _PORT_B:
        STRCPY(cPortDetails, "Port B");
        break;
    case _PORT_C:
        STRCPY(cPortDetails, "Port C");
        break;
    case _PORT_D:
        STRCPY(cPortDetails, "Port D");
        break;
    case _PORT_E:
        STRCPY(cPortDetails, "Port E");
        break;
    case _PORT_F:
        STRCPY(cPortDetails, "Port F");
        break;
    case _PORT_G:
        STRCPY(cPortDetails, "Port G");
        break;
    case _PORT_H:
        STRCPY(cPortDetails, "Port H");
        break;
    case _PORT_J:
        STRCPY(cPortDetails, "Port J");
        break;
    case _PORT_EH:
        STRCPY(cPortDetails, "Port EH");
        break;
    case _PORT_EL:
        STRCPY(cPortDetails, "Port EL");
        break;
    case _PORT_QA:
        STRCPY(cPortDetails, "Port QA");
        break;
    case _PORT_QB:
        STRCPY(cPortDetails, "Port QB");
        break;
    case _PORT_SD:
        STRCPY(cPortDetails, "Port SD");
        break;
    case _PORT_TB:
        STRCPY(cPortDetails, "Port TB");
        iPortLength = 4;
        break;
#elif defined _M520X                                                     // {12}
    case _PORT_FECH:
        STRCPY(cPortDetails, "Port FECH");
        break;
    case _PORT_FECL:
        STRCPY(cPortDetails, "Port FECL");
        break;
    case _PORT_BUSCTL:
        STRCPY(cPortDetails, "Port BUSCTL");
        break;
    case _PORT_BE:
        STRCPY(cPortDetails, "Port BE");
        break;
    case _PORT_CS:
        STRCPY(cPortDetails, "Port CS");
        break;
    case _PORT_FECI2C:
        STRCPY(cPortDetails, "Port FECI2C");
        break;
    case _PORT_UART:
        STRCPY(cPortDetails, "Port UART");
        break;
    case _PORT_QSPI:
        STRCPY(cPortDetails, "Port QSPI");
        break;
    case _PORT_TIMER:
        STRCPY(cPortDetails, "Port TIMER");
        break;
    case _PORT_IRQ:
        STRCPY(cPortDetails, "Port IRQ");
        break;
#elif defined _M523X                                                     // {14}
    case _PORT_ADDR:
        STRCPY(cPortDetails, "Port ADDR");
        break;
    case _PORT_BS:
        STRCPY(cPortDetails, "Port BS");
        break;
    case _PORT_BUSCTL:
        STRCPY(cPortDetails, "Port BUSCTL");
        break;
    case _PORT_CS:
        STRCPY(cPortDetails, "Port CS");
        break;
    case _PORT_DATAH:
        STRCPY(cPortDetails, "Port DATAH");
        break;
    case _PORT_DATAL:
        STRCPY(cPortDetails, "Port DATAL");
        break;
    case _PORT_FECI2C:
        STRCPY(cPortDetails, "Port FECI2C");
        break;
    case _PORT_ETPU:
        STRCPY(cPortDetails, "Port ETPU");
        break;
    case _PORT_IRQ:
        STRCPY(cPortDetails, "Port IRQ");
        break;
    case _PORT_QSPI:
        STRCPY(cPortDetails, "Port QSPI");
        break;
    case _PORT_SDRAM:
        STRCPY(cPortDetails, "Port SDRAM");
        break;
    case _PORT_TIMER:
        STRCPY(cPortDetails, "Port TIMER");
        break;
    case _PORT_UARTH:
        STRCPY(cPortDetails, "Port UARTH");
        break;
    case _PORT_UARTL:
        STRCPY(cPortDetails, "Port UARTL");
        break;
#else
    case _PORT_AN:
        STRCPY(cPortDetails, "Port AN");
        break;
    #if !defined _M5225X && !defined _M521X                              // {2}{9}
    case _PORT_LD:
        STRCPY(cPortDetails, "Port LD");
        break;
    case _PORT_GP:
        STRCPY(cPortDetails, "Port GP");
        break;
    #endif
    case _PORT_UB:
        STRCPY(cPortDetails, "Port UB");
        iPortLength = 4;
        break;
    case _PORT_UC:
    #if defined CHIP_80_PIN
        STRCPY(cPortDetails, "Port UC - not available on 80 pin device");
        return; 
    #elif defined CHIP_81_PIN
        STRCPY(cPortDetails, "Port UC - not available on 81 MAPBGA pin device");
        return;
    #elif defined CHIP_64_PIN
        STRCPY(cPortDetails, "Port UC - not available on 64 pin device");
        return;
    #else
        STRCPY(cPortDetails, "Port UC");
        iPortLength = 4;
        break;
    #endif
#endif
#if !defined _M520X && !defined _M523X                                   // {12}{14}
    case _PORT_QS:
        STRCPY(cPortDetails, "Port QS");
        break;
    case _PORT_NQ:
        STRCPY(cPortDetails, "Port NQ");
        break;
    case _PORT_TA:
#if defined CHIP_64_PIN && !defined _M521XX && !defined _M521X           // {6}{10}{13}
        STRCPY(cPortDetails, "Port TA - not available on 64 pin device");
        return;
#else
        STRCPY(cPortDetails, "Port TA");
        iPortLength = 4;
#endif
        break;
    case _PORT_TC:
        STRCPY(cPortDetails, "Port TC");
        iPortLength = 4;
        break;
#if !defined _M5225X                                                     // {2}
    case _PORT_TD:
    #if defined CHIP_80_PIN
        STRCPY(cPortDetails, "Port TD - not available on 80 pin device");
        return;
    #elif defined CHIP_64_PIN
        STRCPY(cPortDetails, "Port TD - not available on 64 pin device");
        return;
    #else
        STRCPY(cPortDetails, "Port TD");
        iPortLength = 4;
        break;
    #endif
#endif
    case _PORT_UA:
        STRCPY(cPortDetails, "Port UA");
        iPortLength = 4;
        break;

    case _PORT_AS:
        STRCPY(cPortDetails, "Port AS");
#if defined _M5221X || defined _M5222X || defined _M521XX                // {5}
        if (iBit < 2) {
            STRCPY(cPortDetails, "Port AS - this pin is not available on this device");
            return;
        }
#endif
#if !defined _M52XX_SDRAM                                                // {5}
        iPortLength = 4;
#endif
        break;
    case _PORT_DD:
#if defined _M5225X                                                      // {7}
    #if defined CHIP_144_PIN
        STRCPY(cPortDetails, "Port DD");
    #else
        STRCPY(cPortDetails, "Port DD - not available on 100 pin device");
        return;
    #endif
#else
    #if defined CHIP_81_PIN
        STRCPY(cPortDetails, "Port DD - not available on 81 MAPBGA pin device");
        return;
    #elif defined CHIP_64_PIN
        STRCPY(cPortDetails, "Port DD - not available on 64 pin device");
        return;
    #elif defined CHIP_80_PIN                                            // {8}
        STRCPY(cPortDetails, "Port DD - not available on 80 pin device");
        return;
    #else
        STRCPY(cPortDetails, "Port DD");
    #endif
#endif
        break;
#if defined _M5225X                                                      // {7}
    case _PORT_TE:
    #if defined CHIP_144_PIN
        STRCPY(cPortDetails, "Port TE");
    #else
        STRCPY(cPortDetails, "Port TE - not available on 100 pin device");
        return;
    #endif
        break;

    case _PORT_TF:
    #if defined CHIP_144_PIN
        STRCPY(cPortDetails, "Port TF");
    #else
        STRCPY(cPortDetails, "Port TF - not available on 100 pin device");
        return;
    #endif
        break;

    case _PORT_TG:
    #if defined CHIP_144_PIN
        STRCPY(cPortDetails, "Port TG");
    #else
        STRCPY(cPortDetails, "Port TG - not available on 100 pin device");
        return;
    #endif
        break;

    case _PORT_TH:
    #if defined CHIP_144_PIN
        STRCPY(cPortDetails, "Port TH");
    #else
        STRCPY(cPortDetails, "Port TH - not available on 100 pin device");
        return;
    #endif
        break;

    case _PORT_TI:
        STRCPY(cPortDetails, "Port TI");
        break;

    case _PORT_TJ:
        STRCPY(cPortDetails, "Port TJ");
        break;
#endif
#endif
    default:
      //STRCPY(cPortDetails, "?????");
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
        if (*ptrList > 3) {
            STRCPY(&cBuf[1], "??");
        }
        else {
            STRCPY(&cBuf[1], cPer[iPort][iBit][*ptrList]);
#ifdef SUPPORT_ADC
            if ((cBuf[1] == 'A') && (cBuf[2] == 'N')) {                  // ADC function {1}
                iAdc = cBuf[3] - '0';
                if ((iAdc < 0) || (iAdc > (ADC_CHANNELS-1))) {
                    iAdc = -1;                                           // invalid so ignore
                }
            }
#endif
        }
        STRCAT(cPortDetails, cBuf);
#ifdef SUPPORT_ADC
        if (iAdc >= 0) {                                                 // {1}
            SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iAdc]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)0xfff)); // {3}
            STRCAT(cPortDetails, cBuf);
        }
#endif
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
    }
}


// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
#ifdef SUPPORT_ADC                                                       // {1}
    PORTS *_ports = (PORTS *)hw_table;
    _ptrPerFunctions = _ports->ports;
    _ptrADC          = _ports->adcs;
#else
    _ptrPerFunctions = (unsigned char *)hw_table;
#endif
}

#endif

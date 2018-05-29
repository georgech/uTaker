/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3SXXXX_ports.c
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   **********************************************************************
   16.12.2008 Remove details of ports to external headers                {1}
   01.04.2009 Add ADC support                                            {2}
   03.08.2009 Add LM3S2139                                               {3}
   05.11.2009 Add _LM3S9B90                                              {4}
   13.05.2010 Add _LM3S1958                                              {5}
   16.07.2010 Adapt ADC to work with additional ADC inputs               {6}

*/

#include "config.h"

#ifdef _LM3SXXXX

#define BUF1SIZE 100

#if _VC80_UPGRADE>=0x0600
  #define STRCPY(a, b)     strcpy_s (a, BUF1SIZE, b)
  #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
  #define STRCAT(a, b)     strcat_s(a, BUF1SIZE, b)
#else
  #define STRCPY           strcpy
  #define SPRINTF          sprintf
  #define STRCAT           strcat
#endif

#if defined _LM3S9B95 || defined _LM3S9B90 || defined _LM3S9B96          // {4}
    #include "LM3S9B95_port.h"
#elif defined   _LM3S1968 || defined _LM3S1958                           // {5}
    #include "LM3S1968_port.h"                                           // {1}
#elif defined _LM3S8962
    #include "LM3S8962_port.h"
#elif defined _LM3S6965
    #include "LM3S6965_port.h"
#elif defined _LM3S10X
    #include "LM3S10X_port.h"
#elif defined _LM3S5732
    #include "LM3S5732_port.h"
#elif defined _LM3S3748
    #include "LM3S3748_port.h"
#elif defined _LM3S2110
    #include "LM3S2110_port.h"
#elif defined _LM3S2139                                                  // {3}
    #include "LM3S2139_port.h"
#elif defined _LM3S3768
    #include "LM3S3768_port.h"
#endif


static unsigned char *_ptrPerFunctions;


#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)                    // {2}
    typedef struct stPorts
    {
        unsigned char *ports;
        unsigned long *adcs;
    } PORTS;

    static unsigned long *_ptrADC;
#endif

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
    int iADC = -1;
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE+1];
    #if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) && (defined _FURY_CLASS || defined _SANDSTORM_CLASS) // {6}
    if (iPort >= __PORTS_AVAILABLE) {                                    // dedicated ADC port
        iPort = _GPIO_ADC;
    }
    #endif
    iBit = (iPortLength - iBit - 1);                                     // bit position
    switch (iPort) {
    case _GPIO_A:
        STRCPY(cPortDetails, "Port A");
        break;
    case _GPIO_B:
        STRCPY(cPortDetails, "Port B");
    #if defined SUPPORT_ADC && (PART_DC1 & ADC1_PRESENT1)                // {6}
        if (ulPortPeripheral[iPort] & (0x01 << iBit)) {                  // analogue pin isolation set
            switch (iBit) {
            case 4:
                iADC = 10;
                break;
            case 5:
                iADC = 11;
                break;
            }
        }
    #endif
        break;
    case _GPIO_C:
        STRCPY(cPortDetails, "Port C");
        break;
    case _GPIO_D:
        STRCPY(cPortDetails, "Port D");
    #if (defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS) && defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) // {2}
        if (ulPortPeripheral[iPort] & (0x01 << iBit)) {                              // analogue pin isolation set
            switch (iBit) {
            case 4:
                iADC = 7;
                break;
            case 5:
                iADC = 6;
                break;
            case 6:
                iADC = 5;
                break;
            case 7:
                iADC = 4;
                break;
            }
        }
    #endif
    #if defined SUPPORT_ADC && (PART_DC1 & ADC1_PRESENT1)                            // {6}
        if (ulPortPeripheral[iPort] & (0x01 << iBit)) {                              // analogue pin isolation set
            switch (iBit) {
            case 0:
                iADC = 15;
                break;
            case 1:
                iADC = 14;
                break;
            case 2:
                iADC = 13;
                break;
            case 3:
                iADC = 12;
                break;
            }
        }
    #endif
        break;
    case _GPIO_E:
        STRCPY(cPortDetails, "Port E");
    #if (defined _DUST_DEVIL_CLASS || defined _TEMPEST_CLASS) && defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1) // {2}
        if (ulPortPeripheral[iPort] & (0x01 << iBit)) {                              // analogue pin isolation set
            switch (iBit) {
            case 4:
                iADC = 3;
                break;
            case 5:
                iADC = 2;
                break;
            case 6:
                iADC = 1;
                break;
            case 7:
                iADC = 0;
                break;
            }
        }
    #endif
    #if defined SUPPORT_ADC && (PART_DC1 & ADC1_PRESENT1)                            // {6}
        if (ulPortPeripheral[iPort] & (0x01 << iBit)) {                              // analogue pin isolation set
            switch (iBit) {
            case 2:
                iADC = 9;
                break;
            case 3:
                iADC = 8;
                break;
            }
        }
    #endif
        break;
    case _GPIO_F:
        STRCPY(cPortDetails, "Port F");
        break;
    case _GPIO_G:
        STRCPY(cPortDetails, "Port G");
        break;
    case _GPIO_H:
        STRCPY(cPortDetails, "Port H");
        break;
    case _GPIO_J:
        STRCPY(cPortDetails, "Port J");
        break;
    case _GPIO_ADC:                                                      // {2}
        STRCPY(cPortDetails, "ADC inputs");
        iADC = iBit;
        iPort = (PORTS_AVAILABLE - 1);
        break;
    default:
        STRCPY(cPortDetails, "?????");
        return;
    }
    if (iBit < 0) {
        return;
    }
#if defined _FURY_CLASS || defined _SANDSTORM_CLASS                      // {6}
    if (iADC < 0) {
        SPRINTF(cBuf, " Bit %i Pin: ", iBit);
        STRCAT(cPortDetails, cBuf);
    }
    else {
        STRCAT(cPortDetails, " Pin: ");
    }
#else
    SPRINTF(cBuf, " Bit %i Pin: ", iBit);
    STRCAT(cPortDetails, cBuf);
#endif
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
#if defined _TEMPEST_CLASS
        if (*ptrList > 11) {
            STRCPY(&cBuf[1], "??");
        }
#else
        if (*ptrList > 1) {
            STRCPY(&cBuf[1], "??");
        }
#endif
        else {
            STRCPY(&cBuf[1], cPer[iPort][iBit][*ptrList]);
        }
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if (iADC >= 0) {                                                 // {2}
            STRCAT(cPortDetails, " Inactive");
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
#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)                    // {2}
    if (iADC >= 0) {
        SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iADC]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)ADC_FULL_SCALE));
        STRCAT(cPortDetails, cBuf);
    }
#endif
}

// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
#if defined SUPPORT_ADC && (PART_DC1 & ADC0_PRESENT1)                    // {1}
    PORTS *_ports = (PORTS *)hw_table;
    _ptrPerFunctions = _ports->ports;
    _ptrADC          = _ports->adcs;
#else
    _ptrPerFunctions = (unsigned char *)hw_table; 
#endif
}

#endif

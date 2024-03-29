/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, R�tihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      Watchdog.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    
*/        

#include "config.h"

#define OWN_TASK    TASK_WATCHDOG


extern void fnTaskWatchdog(TTASKTABLE *ptrTaskTable)                     // watchdog called regularly
{
    fnRetriggerWatchdog();                                               // hardware dependent
} 

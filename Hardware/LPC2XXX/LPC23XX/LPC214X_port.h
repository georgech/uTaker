/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC214X_port.h
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    30.01.2011 Add LPC213x

*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT 0 (0..31)
#if defined _LPC213X
    #if defined LPC2131
    { "19 {P0.0/TXD0/PWM1}", "21 {P0.1/RXD0/PWM3/EINT0}", "22 {P0.2/SCL0/CAP0.0}", "26 {P0.3/SDA0/MAT0.0/EINT1}", "27 {P0.4/SCK0/CAP0.1/AD0.6}", "29 {P0.5/MISO0/MAT0.1/AD0.7}", "30 {P0.6/MOSI0/CAP0.2}", "31 {P0.7/SSEL0/PWM2/EINT2}", "33 {P0.8/TXD1/PWM4}", "34 {P0.9/RXD1/PWM6/EINT3}", "35 {P0.10/CAP1.0}", "37 {P0.11/CAP1.1/SCL1}", "38 {P0.12/MAT1.0}", "39 {P0.13/MAT1.1}", "41 {P0.14/EINT1/SDA1((ISP))}", "45 {P0.15/EINT2}", "46 {P0.16/EINT0/MAT0.2/CAP0.2}", "47 {P0.17/CAP1.2/SCK1/MAT1.2}", "53 {P0.18/CAP1.3/MISO1/MAT1.3}", "54 {P0.19/MAT1.2/MOSI1/CAP1.2}", "55 {P0.20/MAT1.3/SSEL1/EINT3}", "1 {P0.21/PWM5/CAP1.3}", "2 {P0.22/CAP0.0/MAT0.0}", "58 {P0.23}", "NA", "9 {P0.25/AD0.4}", "10 {P0.26/AD0.5}", "11 {P0.27/AD0.0/CAP0.1/MAT0.1}", "13 {P0.28/AD0.1/CAP0.2/MAT0.2}", "14 {P0.29/AD0.2/CAP0.3/MAT0.3}", "15 {P0.30/AD0.3/EINT3/CAP0.0}", "17 {P0.31}"},
    #elif defined LPC2132
    { "19 {P0.0/TXD0/PWM1}", "21 {P0.1/RXD0/PWM3/EINT0}", "22 {P0.2/SCL0/CAP0.0}", "26 {P0.3/SDA0/MAT0.0/EINT1}", "27 {P0.4/SCK0/CAP0.1/AD0.6}", "29 {P0.5/MISO0/MAT0.1/AD0.7}", "30 {P0.6/MOSI0/CAP0.2}", "31 {P0.7/SSEL0/PWM2/EINT2}", "33 {P0.8/TXD1/PWM4}", "34 {P0.9/RXD1/PWM6/EINT3}", "35 {P0.10/CAP1.0}", "37 {P0.11/CAP1.1/SCL1}", "38 {P0.12/MAT1.0}", "39 {P0.13/MAT1.1}", "41 {P0.14/EINT1/SDA1((ISP))}", "45 {P0.15/EINT2}", "46 {P0.16/EINT0/MAT0.2/CAP0.2}", "47 {P0.17/CAP1.2/SCK1/MAT1.2}", "53 {P0.18/CAP1.3/MISO1/MAT1.3}", "54 {P0.19/MAT1.2/MOSI1/CAP1.2}", "55 {P0.20/MAT1.3/SSEL1/EINT3}", "1 {P0.21/PWM5/CAP1.3}", "2 {P0.22/CAP0.0/MAT0.0}", "58 {P0.23}", "NA", "9 {P0.25/AD0.4/AOUT}", "10 {P0.26/AD0.5}", "11 {P0.27/AD0.0/CAP0.1/MAT0.1}", "13 {P0.28/AD0.1/CAP0.2/MAT0.2}", "14 {P0.29/AD0.2/CAP0.3/MAT0.3}", "15 {P0.30/AD0.3/EINT3/CAP0.0}", "17 {P0.31}"},
    #else                                                                // LPC2134/36/38
    { "19 {P0.0/TXD0/PWM1}", "21 {P0.1/RXD0/PWM3/EINT0}", "22 {P0.2/SCL0/CAP0.0}", "26 {P0.3/SDA0/MAT0.0/EINT1}", "27 {P0.4/SCK0/CAP0.1/AD0.6}", "29 {P0.5/MISO0/MAT0.1/AD0.7}", "30 {P0.6/MOSI0/CAP0.2/AD1.0}", "31 {P0.7/SSEL0/PWM2/EINT2}", "33 {P0.8/TXD1/PWM4/AD1.1}", "34 {P0.9/RXD1/PWM6/EINT3}", "35 {P0.10/RTS1/CAP1.0/AD1.2}", "37 {P0.11/CTS1/CAP1.1/SCL1}", "38 {P0.12/DSR1/MAT1.0/AD1.3}", "39 {P0.13/DTR1/MAT1.1/AD1.4}", "41 {P0.14/DCD1/EINT1/SDA1((ISP))}", "45 {P0.15/RI1/EINT2/AD1.5}", "46 {P0.16/EINT0/MAT0.2/CAP0.2}", "47 {P0.17/CAP1.2/SCK1/MAT1.2}", "53 {P0.18/CAP1.3/MISO1/MAT1.3}", "54 {P0.19/MAT1.2/MOSI1/CAP1.2}", "55 {P0.20/MAT1.3/SSEL1/EINT3}", "1 {P0.21/PWM5/AD1.6/CAP1.3}", "2 {P0.22/AD1.7/CAP0.0/MAT0.0}", "58 {P0.23}", "NA", "9 {P0.25/AD0.4/AOUT}", "10 {P0.26/AD0.5}", "11 {P0.27/AD0.0/CAP0.1/MAT0.1}", "13 {P0.28/AD0.1/CAP0.2/MAT0.2}", "14 {P0.29/AD0.2/CAP0.3/MAT0.3}", "15 {P0.30/AD0.3/EINT3/CAP0.0}", "17 {P0.31}"},
    #endif
#else
    { "19 {P0.0/TXD0/PWM1}", "21 {P0.1/RXD0/PWM3/EINT0}", "22 {P0.2/SCL0/CAP0.0}", "26 {P0.3/SDA0/MAT0.0/EINT1}", "27 {P0.4/SCK0/CAP0.1/AD0.6}", "29 {P0.5/MISO0/MAT0.1/AD0.7}", "30 {P0.6/MOSI0/CAP0.2/AD1.0}", "31 {P0.7/SSEL0/PWM2/EINT2}", "33 {P0.8/TXD1/PWM4/AD1.1}", "34 {P0.9/RXD1/PWM6/EINT3}", "35 {P0.10/RTS1/CAP1.0/AD1.2}", "37 {P0.11/CTS1/CAP1.1/SCL1}", "38 {P0.12/DSR1/MAT1.0/AD1.3}", "39 {P0.13/DTR1/MAT1.1/AD1.4}", "41 {P0.14/DCD1/EINT1/SDA1((ISP))}", "45 {P0.15/RI1/EINT2/AD1.5}", "46 {P0.16/EINT0/MAT0.2/CAP0.2}", "47 {P0.17/CAP1.2/SCK1/MAT1.2}", "53 {P0.18/CAP1.3/MISO1/MAT1.3}", "54 {P0.19/MAT1.2/MOSI1/CAP1.2}", "55 {P0.20/MAT1.3/SSEL1/EINT3}", "1 {P0.21/PWM5/AD1.6/CAP1.3}", "2 {P0.22/AD1.7/CAP0.0/MAT0.0}", "58 {P0.23/VBUS}", "NA", "9 {P0.25/AD0.4/AOUT}", "NA", "NA", "13 {P0.28/AD0.1/CAP0.2/MAT0.2}", "14 {P0.29/AD0.2/CAP0.3/MAT0.3}", "15 {P0.30/EINT3/CAP0.0}", "17 {P0.31/UP_LED/CONNECT}"},
#endif
    // PORT 1 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "16 {P1.16/TRACEPKT0}", "12 {P1.17/TRACEPKT1}", "8 {P1.18/TRACEPKT2}", "4 {P1.19/TRACEPKT3}", "48 {P1.20/TRACESYNC}", "44 {P1.21/PIPESTAT0}", "40 {P1.22/PIPESTAT1}", "36 {P1.23/PIPESTAT2}", "32 {P1.24/TRACECLK}", "28 {P1.25/EXTIN0}", "24 {P1.26/RTCK}", "64 {P1.27/TDO}", "60 {P1.28/TDI}", "56 {P1.29/TCK}", "52 {P1.30/TMS}", "20 {P1.31/TRST}"},

};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
#if defined _LPC213X
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "TXD0",      "PWM1",  "INVALID"  },                      // PORT 0
        {  "Q", "RXD0",      "PWM3",  "EINT0"    },
        {  "Q", "SCL0",      "CAP0.0","INVALID"  },
        {  "Q", "SDA0",      "MAT0.0","EINT1"    },
        {  "Q", "SCK0",      "CAP0.1","AD0.6"    },
        {  "Q", "MISO0",     "MAT0.1","AD0.7"    },
    #if defined LPC2131 || defined LPC2132
        {  "Q", "MOSI0",     "CAP0.2","INVALID"  },
    #else
        {  "Q", "MOSI0",     "CAP0.2","AD1.0"    },
    #endif
        {  "Q", "SSEL0",     "PWM2",  "EINT2"    },
    #if defined LPC2131 || defined LPC2132
        {  "Q", "TXD1",      "PWM4",  "INVALID"  },  
    #else
        {  "Q", "TXD1",      "PWM4",  "AD1.1"    },                 
    #endif
        {  "Q", "RXD1",      "PWM6",  "EINT3"    },
    #if defined LPC2131 || defined LPC2132
        {  "Q", "INVALID",   "CAP1.0","INVALID"  },
        {  "Q", "INVALID",   "CAP1.1","SCL1"     },
        {  "Q", "INVALID",   "MAT1.0","INVALID"  },
        {  "Q", "INVALID",   "MAT1.1","INVALID"  },
        {  "Q", "INVALID",   "EINT1", "SDA1"     },
        {  "Q", "INVALID",   "EINT2", "INVALID"  },
    #else
        {  "Q", "RTS1",      "CAP1.0","AD1.2"    },
        {  "Q", "CTS1",      "CAP1.1","SCL1"     },
        {  "Q", "DSR1",      "MAT1.0","AD1.3"    },
        {  "Q", "DTR1",      "MAT1.1","AD1.4"    },
        {  "Q", "DCD1",      "EINT1", "SDA1"     },
        {  "Q", "RI1",       "EINT2", "AD1.5"    },
    #endif
        {  "Q", "EINT0",     "MAT0.2","CAP0.2"   },
        {  "Q", "CAP1.2",    "SCK1",  "MAT1.2"   },                 
        {  "Q", "CAP1.3",    "MISO1", "MAT1.3"   },
        {  "Q", "MAT1.2",    "MOSI1", "CAP1.2"   },
        {  "Q", "MAT1.3",    "SSEL1", "EINT3"    },
    #if defined LPC2131 || defined LPC2132
        {  "Q", "PWM5",      "INVALID","CAP1.3"  },
        {  "Q", "INVALID",   "CAP0.0","MAT0.0"   },
    #else
        {  "Q", "PWM5",      "AD1.6", "CAP1.3"   },
        {  "Q", "AD1.7",     "CAP0.0","MAT0.0"   },
    #endif
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
    #if defined LPC2131
        {  "Q", "AD0.4",     "INVALID","INVALID" },
    #else
        {  "Q", "AD0.4",     "AOUT",   "INVALID" },
    #endif
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "AD0.1",     "CAP0.2", "MAT0.2"  },
        {  "Q", "AD0.2",     "CAP0.3", "MAT0.3"  },
        {  "Q", "EINT3",     "CAP0.0","INVALID"  },
        {  "Q", "INVALID",   "INVALID","INVALID" }                       // port is output only
#else
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "TXD0",      "PWM1",  "INVALID"  },                      // PORT 0
        {  "Q", "RXD0",      "PWM3",  "EINT0"    },
        {  "Q", "SCL0",      "CAP0.0","INVALID"  },
        {  "Q", "SDA0",      "MAT0.0","EINT1"    },
        {  "Q", "SCK0",      "CAP0.1","AD0.6"    },
        {  "Q", "MISO0",     "MAT0.1","AD0.7"    },
        {  "Q", "MOSI0",     "CAP0.2","AD1.0"    },
        {  "Q", "SSEL0",     "PWM2",  "EINT2"    },
        {  "Q", "TXD1",      "PWM4",  "AD1.1"    },                 
        {  "Q", "RXD1",      "PWM6",  "EINT3"    },
        {  "Q", "RTS1",      "CAP1.0","AD1.2"    },
        {  "Q", "CTS1",      "CAP1.1","SCL1"     },
        {  "Q", "DSR1",      "MAT1.0","AD1.3"    },
        {  "Q", "DTR1",      "MAT1.1","AD1.4"    },
        {  "Q", "DCD1",      "EINT1", "SDA1"     },
        {  "Q", "RI1",       "EINT2", "AD1.5"    },
        {  "Q", "EINT0",     "MAT0.2","CAP0.2"   },
        {  "Q", "CAP1.2",    "SCK1",  "MAT1.2"   },                 
        {  "Q", "CAP1.3",    "MISO1", "MAT1.3"   },
        {  "Q", "MAT1.2",    "MOSI1", "CAP1.2"   },
        {  "Q", "MAT1.3",    "SSEL1", "EINT3"    },
        {  "Q", "PWM5",      "AD1.6", "CAP1.3"   },
        {  "Q", "AD1.7",     "CAP0.0","MAT0.0"   },
        {  "Q", "VBUS",      "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "AD0.4",     "AOUT",   "INVALID" },                 
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "AD0.1",     "CAP0.2", "MAT0.2"  },
        {  "Q", "AD0.2",     "CAP0.3", "MAT0.3"  },
        {  "Q", "EINT3",     "CAP0.0","INVALID"  },
        {  "Q", "UP_LED",    "CONNECT","INVALID" }                       // port is output only
#endif
    },
    {
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "INVALID",   "INVALID","INVALID" },                      // PORT 1
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "INVALID",   "INVALID","INVALID" },
        {  "Q", "TRACEPKT0", "INVALID","INVALID" },
        {  "Q", "TRACEPKT1", "INVALID","INVALID" },
        {  "Q", "TRACEPKT2", "INVALID","INVALID" },                 
        {  "Q", "TRACEPKT3", "INVALID","INVALID" },
        {  "Q", "TRACESYNC", "INVALID","INVALID" },
        {  "Q", "PIPESTAT0", "INVALID","INVALID" },
        {  "Q", "PIPESTAT1", "INVALID","INVALID" },
        {  "Q", "PIPESTAT2", "INVALID","INVALID" },
        {  "Q", "TRACECLK",  "INVALID","INVALID" },
        {  "Q", "EXTIN0",    "INVALID","INVALID" },
        {  "Q", "RTCK",      "INVALID","INVALID" },
        {  "Q", "TDO",       "INVALID","INVALID" },
        {  "Q", "TDI",       "INVALID","INVALID" },
        {  "Q", "TCK",       "INVALID","INVALID" },
        {  "Q", "TMS",       "INVALID","INVALID" },
        {  "Q", "TRST",      "INVALID","INVALID" }
    }
};

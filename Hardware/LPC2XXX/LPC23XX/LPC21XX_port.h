/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC21XX_port.h
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************

    06.03.2010 Correct I2C1/CAP1 configuration                           {1}
*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT 0 (0..31)
#if defined LPC2101 || defined LPC2102 || defined LPC2103
    #ifdef PLCC44
    { "18 {P0.0/TXD0/MAT3.1}", "19 {P0.1/RXD0/MAT3.2}", "22 {P0.2/SCL0/CAP0.0}", "25 {P0.3/SDA0/MAT0.0}", "26 {P0.4/SCK0/CAP0.1}", "27 {P0.5/MISO0/MAT0.1}", "28 {P0.6/MOSI0/CAP0.2}", "31 {P0.7/SSEL0/MAT2.0}", "32 {P0.8/TXD1/MAT2.1}", "33 {P0.9/RXD1/MAT2.2}", "38 {P0.10/RTS1/CAP1.0/AD0.3}", "39 {P0.11/CTS1/CAP1.1/AD0.4}", "40 {P0.12/DSR1/MAT1.0/AD0.5}", "43 {P0.13/DTR1/MAT1.1}", "2 {P0.14/DCD1/SCK1/EINT1(ISP)}", "3 {P0.15/RI1/EINT2}", "4 {P0.16/EINT0/MAT0.2}", "5 {P0.17/CAP1.2/SCL1}", "6 {P0.18/CAP1.3/SDA1}", "7 {P0.19/MAT1.2/MISO1}", "8 {P0.20/MAT1.3/MOSI1}", "9 {P0.21/SSEL1/MAT3.0}", "35 {P0.22/AD0.0}", "36 {P0.23/AD0.1}", "37 {P0.24/AD0.2}", "41 {P0.25/AD0.6}", "NA", "13 {P0.27/TRST/CAP2.0}", "14 {P0.28/TMS/CAP2.1}", "15 {P0.29/TCK/CAP2.2}", "20 {P0.30/TDI/MAT3.3}", "21 {P0.31/TDO}"},
    #else
    { "13 {P0.0/TXD0/MAT3.1}", "14 {P0.1/RXD0/MAT3.2}", "18 {P0.2/SCL0/CAP0.0}", "21 {P0.3/SDA0/MAT0.0}", "22 {P0.4/SCK0/CAP0.1}", "23 {P0.5/MISO0/MAT0.1}", "24 {P0.6/MOSI0/CAP0.2}", "28 {P0.7/SSEL0/MAT2.0}", "29 {P0.8/TXD1/MAT2.1}", "30 {P0.9/RXD1/MAT2.2}", "35 {P0.10/RTS1/CAP1.0/AD0.3}", "36 {P0.11/CTS1/CAP1.1/AD0.4}", "37 {P0.12/DSR1/MAT1.0/AD0.5}", "41 {P0.13/DTR1/MAT1.1}", "44 {P0.14/DCD1/SCK1/EINT1(ISP)}", "45 {P0.15/RI1/EINT2}", "46 {P0.16/EINT0/MAT0.2}", "47 {P0.17/CAP1.2/SCL1}", "48 {P0.18/CAP1.3/SDA1}", "1 {P0.19/MAT1.2/MISO1}", "2 {P0.20/MAT1.3/MOSI1}", "3 {P0.21/SSEL1/MAT3.0}", "32 {P0.22/AD0.0}", "33 {P0.23/AD0.1}", "34 {P0.24/AD0.2}", "38 {P0.25/AD0.6}", "39 {P0.26/AD0.7}", "8 {P0.27/TRST/CAP2.0}", "9 {P0.28/TMS/CAP2.1}", "10 {P0.29/TCK/CAP2.2}", "15 {P0.30/TDI/MAT3.3}", "16 {P0.31/TDO}"},
    #endif
#else                                                                    // LPC2104/5/6
    { "13 {P0.0/TXD0/PWM1}", "14 {P0.1/RXD0/PWM3}", "18 {P0.2/SCL/CAP0.0}", "21 {P0.3/SDA/MAT0.0}", "22 {P0.4/SCK/CAP0.1}", "23 {P0.5/MISO/MAT0.1}", "24 {P0.6/MOSI/CAP0.2}", "28 {P0.7/SSEL/PWM2}", "29 {P0.8/TXD1/PWM4}", "30 {P0.9/RXD1/PWM6}", "35 {P0.10/RTS1/CAP1.0}", "36 {P0.11/CTS1/CAP1.1}", "37 {P0.12/DSR1/MAT1.0}", "41 {P0.13/DTR1/MAT1.1}", "44 {P0.14/DCD1/EINT1(ISP)}", "45 {P0.15/RI1/EINT2}", "46 {P0.16/EINT0/MAT0.2}", "47 {P0.17/CAP1.2/TRST}", "48 {P0.18/CAP1.3/TMS}", "1 {P0.19/MAT1.2/TCK}", "2 {P0.20/MAT1.3/TDI}", "3 {P0.21/PWM5/TDO}", "32 {P0.22/TRACECLK}", "33 {P0.23/PIPESTAT0}", "34 {P0.24/PIPESTAT1}", "38 {P0.25/PIPESTAT2}", "39 {P0.26/TRACESYNC}", "8 {P0.27/TRACEPKT0/TRST}", "9 {P0.28/TRACEPKT1/TMS}", "10 {P0.29/TRACEPKT2/TCK}", "15 {P0.30/TRACEPKT3/TDI}", "15 {P0.31/EXTIN0/TDO}"},
#endif
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
#if defined LPC2101 || defined LPC2102 || defined LPC2103
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "TXD0",      "MAT3.1","INVALID"  },                      // PORT 0
        {  "Q", "RXD0",      "MAT3.2","INVALID"  },
        {  "Q", "SCL0",      "CAP0.0","INVALID"  },                      // GPIO open collector
        {  "Q", "SDA0",      "MAT0.0","INVALID"  },                      // GPIO open collector
        {  "Q", "SCK0",      "CAP0.1","INVALID"  },
        {  "Q", "MISO0",     "MAT0.1","INVALID"  },
        {  "Q", "MOSI0",     "CAP0.2","INVALID"  },
        {  "Q", "SSEL0",     "MAT2.0","INVALID"  },
        {  "Q", "TXD1",      "MAT2.1","INVALID"  },                 
        {  "Q", "RXD1",      "MAT2.2","INVALID"  },
        {  "Q", "RTS1",      "CAP1.0","AD0.3"    },
        {  "Q", "CTS1",      "CAP1.1","AD0.4"    },
        {  "Q", "DSR1",      "MAT1.0","AD0.5"    },
        {  "Q", "DTR1",      "MAT1.1","INVALID"  },
        {  "Q", "DCD1",      "SCK1",  "EINT1"    },
        {  "Q", "RI1",       "EINT2", "INVALID"  },
        {  "Q", "EINT0",     "MAT0.2","INVALID"  },
        {  "Q", "SCL1",      "CAP1.2","INVALID"  },                      // {1}
        {  "Q", "SDA1",      "CAP1.3","INVALID"  },
        {  "Q", "MAT1.2",    "MISO1", "INVALID"  },
        {  "Q", "MAT1.3",    "MOSI1", "INVALID"  },
        {  "Q", "SSEL1",     "MAT3.0","INVALID"  },
        {  "Q", "AD0.0",     "INVALID","INVALID" },
        {  "Q", "AD0.1",     "INVALID","INVALID" },
        {  "Q", "AD0.2",     "INVALID","INVALID" },
        {  "Q", "AD0.6",     "INVALID","INVALID" }, 
    #ifdef PLCC44
        {  "Q", "INVALID",   "INVALID","INVALID" },
    #else
        {  "Q", "AD0.7",     "INVALID","INVALID" },
    #endif
        {  "Q", "TRST",      "CAP2.0","INVALID"  },
        {  "Q", "TMS",       "CAP2.1","INVALID"  },
        {  "Q", "TCK",       "CAP2.2","INVALID"  },
        {  "Q", "TDI",       "MAT3.3","INVALID"  },
        {  "Q", "TDO",       "INVALID","INVALID" }
#else
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "TXD0",      "PWM1",  "INVALID"  },                      // PORT 0
        {  "Q", "RXD0",      "PWM3",  "INVALID"  },
        {  "Q", "SCL",       "CAP0.0","INVALID"  },
        {  "Q", "SDA",       "MAT0.0","INVALID"  },
        {  "Q", "SCK",       "CAP0.1","INVALID"  },
        {  "Q", "MISO",      "MAT0.1","INVALID"  },
        {  "Q", "MOSI",      "CAP0.2","INVALID"  },
        {  "Q", "SSEL",      "PWM2",  "INVALID"  },
        {  "Q", "TXD1",      "PWM4",  "INVALID"  },                 
        {  "Q", "RXD1",      "PWM6",  "INVALID"  },
        {  "Q", "RTS1",      "CAP1.0","INVALID"  },
        {  "Q", "CTS1",      "CAP1.1","INVALID"  },
        {  "Q", "DSR1",      "MAT1.0","INVALID"  },
        {  "Q", "DTR1",      "MAT1.1","INVALID"  },
        {  "Q", "DCD1",      "EINT1", "INVALID"  },
        {  "Q", "RI1",       "EINT2", "INVALID"  },
        {  "Q", "EINT0",     "MAT0.2","INVALID"  },
        {  "Q", "CAP1.2",    "TRST",  "INVALID"  },                 
        {  "Q", "CAP1.3",    "TMS",   "INVALID"  },
        {  "Q", "MAT1.2",    "TCK",   "INVALID"  },
        {  "Q", "MAT1.3",    "TDI",   "INVALID"  },
        {  "Q", "PWM5",      "TDO",   "INVALID"  },
        {  "Q", "TRACECLK",  "INVALID","INVALID" },
        {  "Q", "PIPESTAT0", "INVALID","INVALID" },
        {  "Q", "PIPESTAT1", "INVALID","INVALID" },
        {  "Q", "PIPESTAT2", "INVALID","INVALID" },                 
        {  "Q", "TRACESYNC", "INVALID","INVALID" },
        {  "Q", "TRACEPKT0", "TRST",  "INVALID"  },
        {  "Q", "TRACEPKT1", "TMS",   "INVALID"  },
        {  "Q", "TRACEPKT2", "TCK",   "INVALID"  },
        {  "Q", "TRACEPKT3", "TDI",   "INVALID"  },
        {  "Q", "EXTIN0",    "TDO",   "INVALID"  }
#endif
    }
};

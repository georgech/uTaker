/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S10X_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   **********************************************************************

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..5)
    { "11 {PA0/U0Rx}", "12 {PA1/U0Tx}", "13 {PA2/SSIClk}", "14 {PA3/SSIFss}", "15 {PA4/SSIRx}", "16 {PA5/SSITx}", "NA", "NA"},
    // PORT B (0..7)
#if PART_CODE == CODE_LM3S102
    { "19 {PB0/CCP0}", "20 {PB1/32kHz}", "23 {PB2/I2CSCL}", "24 {PB3/I2CSDA}", "4 {PB4/C0-}", "3 {PB5/C1-/C0o}", "2 {PB6/C0+}", "1 {PB7/TRST}"},
#else
    { "19 {PB0/CCP0}", "20 {PB1/32kHz}", "23 {PB2}", "24 {PB3}", "4 {PB4/C0-}", "3 {PB5/C1-/C0o}", "2 {PB6/C0+}", "1 {PB7/TRST}"},
#endif
    // PORT C (0..3)
    { "28 {PC0/TCK/SWCLK}", "27 {PC1/TMS/SWDIO}", "26 {PC2/TDI}", "25 {PC3/TDO/SWO}", "NA", "NA", "NA", "NA"},
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][2] = {
    {
        //  Q    Periph.
        {  "Q", "U0Rx"     },                                            // PORT A
        {  "Q", "U0Tx"     },
        {  "Q", "SSI0Clk"  },
        {  "Q", "SSI0Fss"  },
        {  "Q", "SSI0Rx"   },
        {  "Q", "SSI0Tx"   },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "CCP0"     },                                            // PORT B
        {  "Q", "32kHz"    },
#if PART_CODE == CODE_LM3S102
        {  "Q", "I2CSCL"   },
        {  "Q", "I2CSDA"   },
#else
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
#endif
        {  "Q", "C0-"      },
        {  "Q", "C1-/C0o"  },
        {  "Q", "C0+"      },
        {  "Q", "TRST"     }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK/SWCLK"},                                            // PORT C
        {  "Q", "TMS/SWDIO"},
        {  "Q", "TDI"      },
        {  "Q", "TDO/SWO"  },
        {  "Q", "-"        },
        {  "Q", "-"        },
        {  "Q", "-"        },
        {  "Q", "-"        },
    },
};




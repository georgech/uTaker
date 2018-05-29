/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S2110_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   **********************************************************************

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx}", "27 {PA1/U0Tx}", "28 {PA2/SSI0Clk}", "29 {PA3/SSI0Fss}", "30 {PA4/SSI0Rx}", "31 {PA5/SSI0Tx}", "34 {PA6/CCP1}", "NA"},
    // PORT B (0..7)
    { "66 {PB0/CCP0}", "67 {PB1/CCP2}", "70 {PB2/I2C0SCL}", "71 {PB3/I2C0SDA}", "92 {PB4/C0-}", "91 {PB5/C1-}", "90 {PB6/C0+}", "89 {PB7/TRST}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}",  "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4}", "24 {PC5/C1+}", "23 {PC6/C2+}", "22 {PC7/C2-}"},
    // PORT D (0..7)
    { "10 {PD0/CAN0Rx}", "11 {PD1/CAN0Tx}", "12 {PD2}", "13 {PD3}", "95 {PD4/CCP3}", "96 {PD5}", "99 {PD6/Fault}", "100 {PD7/C0o}"},
    // PORT E (0..7)
    { "72 {PE0}", "73 {PE1}", "NA", "96 {NA}", "NA", "NA", "NA", "NA"},
    // PORT F (0..7)
    { "47 {PF0/PWM0}", "61 {PF1/PWM1}", "60 {PF2}", "NA", "NA", "NA", "NA", "NA"},
    // PORT G (0..7)
    { "19 {PG0}", "18 {PG1}", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT H (0..7)
    { "86 {PH0}", "85 {PH1}", "NA", "NA", "NA", "NA", "NA", "NA"},
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
        {  "Q", "CCP1"     },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "CCP0"     },                                            // PORT B
        {  "Q", "CCP2"     },
        {  "Q", "I2C0SCL"  },
        {  "Q", "I2C0SDA"  },
        {  "Q", "C0-"      },
        {  "Q", "C1-"      },
        {  "Q", "C0+"      },
        {  "Q", "TRST"     }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK/SWCLK"},                                            // PORT C
        {  "Q", "TMS/SWDIO"},
        {  "Q", "TDI"      },
        {  "Q", "TDO/SWO"  },
        {  "Q", "invalid"  },
        {  "Q", "C1+"      },
        {  "Q", "C2+"      },
        {  "Q", "C2-"      }
    },
    {
        //  Q    Periph.
        {  "Q", "CAN0Rx"   },                                            // PORT D
        {  "Q", "CAN0Tx"   },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "CCP3"     },
        {  "Q", "invalid"  },
        {  "Q", "Fault"    },
        {  "Q", "C0o"      }
    },
    {
        //  Q    Periph.
        {  "Q", "invalid"  },                                            // PORT E
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM0"     },                                            // PORT F
        {  "Q", "PWM1"     },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "invalid"  },                                            // PORT G
        {  "Q", "invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "invalid"  },                                             // PORT H
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
};




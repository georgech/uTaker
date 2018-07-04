/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S2139_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   **********************************************************************

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx}", "27 {PA1/U0Tx}", "28 {PA2/SSI0Clk}", "29 {PA3/SSI0Fss}", "30 {PA4/SSI0Rx}", "31 {PA5/SSI0Tx}", "34 {PA6/CCP1}", "35 {PA7/CCP4}"},
    // PORT B (0..7)
    { "66 {PB0/CCP0}", "67 {PB1/CCP2}", "70 {PB2/I2C0SCL}", "71 {PB3/I2C0SDA}", "92 {PB4/C0-}", "91 {PB5/C1-}", "90 {PB6/C0+}", "89 {PB7/TRST}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}",  "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4/CCP5}", "24 {PC5/C1+}", "23 {PC6/C2+}", "22 {PC7/C2-}"},
    // PORT D (0..7)
    { "10 {PD0/CAN0Rx}", "11 {PD1/CAN0Tx}", "12 {PD2/U1Rx}", "13 {PD3/U1Tx}", "95 {PD4/CCP3}", "96 {PD5}", "99 {PD6}", "100 {PD7/C0o}"},
    // PORT E (0..7)
    { "72 {PE0}", "73 {PE1}", "74 {PE2}", "75 {PE3}", "NA", "NA", "NA", "NA"},
    // PORT F (0..7)
    { "47 {PF0}", "61 {PF1}", "60 {PF2}", "59 {PF3}", "58 {PF4}", "46 {PF5}", "43 {PF6}", "42 {PF7}"},
    // PORT G (0..7)
    { "19 {PG0}", "18 {PG1}", "17 {PG2}", "16 {PG3}", "41 {PG4}", "40 {PG5}", "37 {PG6}", "36 {PG7}"},
    // PORT H (0..7)
    { "86 {PH0}", "85 {PH1}", "84 {PH2}", "83 {PH3}", "NA", "NA", "NA", "NA"},
    // ADC (0..3)
    { "1 {AIN0}", "2 {AIN1}", "5 {AIN2}", "6 {AIN3}", "NA", "NA", "NA", "NA"},
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
        {  "Q", "CCP4"     }
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
        {  "Q", "CCP5"     },
        {  "Q", "C1+"      },
        {  "Q", "C2+"      },
        {  "Q", "C2-"      }
    },
    {
        //  Q    Periph.
        {  "Q", "CAN0Rx"   },                                            // PORT D
        {  "Q", "CAN0Tx"   },
        {  "Q", "U1Rx"     },
        {  "Q", "U1Tx"     },
        {  "Q", "CCP3"     },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
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
        {  "Q", "invalid"  },                                            // PORT F
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
        {  "Q", "invalid"  },                                            // PORT H
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "AIN0",    "AIN0"     },                                      // ADC
        {  "AIN1",    "AIN1"     },
        {  "AIN2",    "AIN2"     },
        {  "AIN3",    "AIN3"     },
        {  "-",       "-"        },
        {  "-",       "-"        },
        {  "-",       "-"        },
        {  "-",       "-"        },
    },
};




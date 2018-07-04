/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S3748_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   **********************************************************************

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx}", "27 {PA1/U0Tx}", "28 {PA2/SSI0Clk}", "29 {PA3/SSI0Fss}", "30 {PA4/SSI0Rx}", "31 {PA5/SSI0Tx}", "34 {PA6/I2C1SCL}", "35 {PA7/I2C1SDA}"},
    // PORT B (0..7)
    { "66 {PB0}", "67 {PB1}", "72 {PB2/I2C0SCL}", "65 {PB3/I2C0SDA}", "92 {PB4/C0-}", "91 {PB5/C1-}", "90 {PB6/C0+}", "89 {PB7/NMI}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}",  "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4/CCP4}", "24 {PC5/C1+}", "23 {PC6/U1Rx}", "22 {PC7/U1Tx}"},
    // PORT D (0..7)
    { "10 {PD0/IDX0}", "11 {PD1/PhA0}", "12 {PD2/CCP5}", "13 {PD3/CCP0}", "97 {PD4/ACD7}", "98 {PD5/ACD6}", "99 {PD6/ACD5}", "100 {PD7/ACD4}"},
    // PORT E (0..7)
    { "74 {PE0/SSI1Clk}", "75 {PE1/SSI1Fss}", "95 {PE2/SSI1Rx}", "96 {PE3/SSI1Tx}", "6 {PE4/ADC3}", "5 {PE5/ADC2}", "2 {PE6/ADC1}", "1 {PE7/ADC0}"},
    // PORT F (0..7)
    { "47 {PF0/PWM0}", "61 {PF1/PWM1}", "60 {PF2/PMW2}", "59 {PF3/PWM3}", "58 {PF4/Fault0}", "46 {PF5/CCP2}", "43 {PF6/CCP1}", "42 {PF7/PhB0}"},
    // PORT G (0..7)
    { "19 {PG0/PWM4}", "18 {PG1/PWM5}", "17 {PG2}", "16 {PG3/Fault2}", "41 {PG4/CCP3}", "40 {PG5/Fault1}", "37 {PG6/PWM6}", "36 {PG7/PWM7}"},
    // PORT H (0..7)
    { "86 {PH0/CCP6}", "85 {PH1/CCP7}", "84{PH2/Fault3}", "83 {PH3/USB0EPEN}", "76 {PH4/USB0PFLT}", "NA", "NA", "NA"},
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
        {  "Q", "I2C1SCL"  },
        {  "Q", "I2C1SDA"  }
    },
    {
        //  Q    Periph.
        {  "Q", "Invalid"  },                                            // PORT B
        {  "Q", "Invalid"  },
        {  "Q", "I2C0SCL"  },
        {  "Q", "I2C0SDA"  },
        {  "Q", "C0-"      },
        {  "Q", "C1-"      },
        {  "Q", "C0+"      },
        {  "Q", "NMI"      }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK/SWCLK"},                                            // PORT C
        {  "Q", "TMS/SWDIO"},
        {  "Q", "TDI"      },
        {  "Q", "TDO/SWO"  },
        {  "Q", "CCP4"     },
        {  "Q", "C1+"      },
        {  "Q", "U1Rx"     },
        {  "Q", "U1Tx"     }
    },
    {
        //  Q    Periph.
        {  "Q", "IDX0"     },                                            // PORT D
        {  "Q", "PhA0"     },
        {  "Q", "CCP5"     },
        {  "Q", "CCP0"     },
        {  "Q", "ACD7"     },
        {  "Q", "ACD6"     },
        {  "Q", "ACD5"     },
        {  "Q", "ACD4"     }
    },
    {
        //  Q    Periph.
        {  "Q", "SSI1Clk"  },                                            // PORT E
        {  "Q", "SSI1Fss"  },
        {  "Q", "SSI1Rx"   },
        {  "Q", "SSI1Tx"   },
        {  "Q", "ADC3"     },
        {  "Q", "ADC2"     },
        {  "Q", "ADC1"     },
        {  "Q", "ADC0"     }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM0"     },                                            // PORT F
        {  "Q", "PWM1"     },
        {  "Q", "PMW2"     },
        {  "Q", "PWM3"     },
        {  "Q", "Fault0"   },
        {  "Q", "CCP2"     },
        {  "Q", "CCP1"     },
        {  "Q", "PhB0"     }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM4"     },                                            // PORT G
        {  "Q", "PWM5"     },
        {  "Q", "Invalid"  },
        {  "Q", "Fault2"   },
        {  "Q", "CCP3"     },
        {  "Q", "Fault1"   },
        {  "Q", "PWM6"     },
        {  "Q", "PWM7"     }
    },
    {
        //  Q    Periph.
        {  "Q", "CCP6"  },                                               // PORT H
        {  "Q", "CCP7"  },
        {  "Q", "Fault3"  },
        {  "Q", "USB0EPEN"  },
        {  "Q", "USB0PFLT"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
};




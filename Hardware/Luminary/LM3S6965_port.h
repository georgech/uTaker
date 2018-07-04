/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S6965_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   **********************************************************************
   01.04.2009 Add ADC support                                            {1}

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx}", "27 {PA1/U0Tx}", "28 {PA2/SSI0Clk}", "29 {PA3/SSI0Fss}", "30 {PA4/SSI0Rx}", "31 {PA5/SSI0Tx}", "34 {PA6/I2C1SCL}", "35 {PA7/I2C1SDA}"},
    // PORT B (0..7)
    { "66 {PB0/PWM2}", "67 {PB1/PWM3}", "70 {PB2/I2C0SCL}", "71 {PB3/I2C0SDA}", "92 {PB4/C0-}", "91 {PB5/C1-}", "90 {PB6/C0+}", "89 {PB7/TRST}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}", "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4/PhA0}", "24 {PC5/C1+}", "23 {PC6/CCP3}", "22 {PC7/PhB0}"},
    // PORT D (0..7)
    { "10 {PD0/IDX0}", "11 {PD1/PWM1}", "12 {PD2/U1Rx}", "13 {PD3/U1Tx}", "95 {PD4/CCP0}", "96 {PD5/CCP2}", "99 {PD6/Fault}", "100 {PD7/CCP1}"},
    // PORT E (0..7)
    { "72 {PE0/PWM4}", "73 {PE1/PWM5}", "74 {PE2/PhB1}", "75 {PE3/PhA1}", "NA", "NA", "NA", "NA"},
    // PORT F (0..7)
    { "47 {PF0/PWM0}", "61 {PF1/IDX1}", "60 {PF2/LED1}", "59 {PF3/LED0}", "NA", "NA", "NA", "NA"},
    // PORT G (0..7)
    { "19 {PG0/U2Rx}", "18 {PG1/U2Tx}", "NA", "NA", "NA", "NA", "NA", "NA"},
    // ADC (0..3)
    { "1 {AIN0}", "2 {AIN1}", "5 {AIN2}", "6 {AIN3}", "NA", "NA", "NA", "NA"}, // {1}
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
        {  "Q", "PWM2"  },                                               // PORT B
        {  "Q", "PWM3"  },
        {  "Q", "I2C0SCL"  },
        {  "Q", "I2C0SDA"  },
        {  "Q", "C0-"  },
        {  "Q", "C1-"  },
        {  "Q", "C0+"  },
        {  "Q", "TRST"  }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK"    },                                              // PORT C
        {  "Q", "TMS/SWDIO"},
        {  "Q", "TDI/SWO"},
        {  "Q", "TDO"    },
        {  "Q", "PhA0"   },
        {  "Q", "C1+"   },
        {  "Q", "CCP3"  },
        {  "Q", "PhB0"  }
    },
    {
        //  Q    Periph.
        {  "Q", "IDX0"  },                                               // PORT D
        {  "Q", "PWM1"  },
        {  "Q", "U1Rx"  },
        {  "Q", "U1Tx"  },
        {  "Q", "CCP0"  },
        {  "Q", "CCP2"  },
        {  "Q", "Fault" },
        {  "Q", "CCP1"  }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM4"  },                                               // PORT E
        {  "Q", "PWM5"  },
        {  "Q", "PhB1"  },
        {  "Q", "PhA1"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM0"  },                                               // PORT F
        {  "Q", "IDX1"  },
        {  "Q", "LED1"  },
        {  "Q", "LED0"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "U2Rx"  },                                               // PORT G
        {  "Q", "U2Tx"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "AIN0",    "AIN0"     },                                      // {1} ADC
        {  "AIN1",    "AIN1"     },
        {  "AIN2",    "AIN2"     },
        {  "AIN3",    "AIN3"     },
        {  "Invalid", "Invalid"  },
        {  "Invalid", "Invalid"  },
        {  "Invalid", "Invalid"  },
        {  "Invalid", "Invalid"  }
    }
};




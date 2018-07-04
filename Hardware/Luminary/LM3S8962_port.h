/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S8962_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   **********************************************************************
   09.05.2009 Add ADC support and 108 pin BGA                            {1}

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
#if PACKAGE_TYPE == PACKAGE_BGA                                          // {1}
    // PORT A (0..7)
    { "L3 {PA0/U0Rx}", "M3 {PA1/U0Tx}", "M4 {PA2/SSI0Clk}", "L4 {PA3/SSI0Fss}", "L5 {PA4/SSI0Rx}", "M5 {PA5/SSI0Tx}", "L6 {PA6/CCP1}", "M6 {PA7}"},
    // PORT B (0..7)
    { "E12 {PB0/PWM2}", "D12 {PB1/PWM3}", "C11 {PB2/I2C0SCL}", "C12 {PB3/I2C0SDA}", "A6 {PB4/C0-}", "B7 {PB5/C0o}", "A7 {PB6/C0+}", "A8 {PB7/TRST}"},
    // PORT C (0..7)
    { "A9 {PC0/TCK/SWCLK}", "B9 {PC1/TMS/SWDIO}", "B8 {PC2/TDI}", "A10 {PC3/TDO/SWO}", "L1 {PC4/PhA0}", "M1 {PC5}", "M2 {PC6/PhB0}", "L2 {PC7}"},
    // PORT D (0..7)
    { "G1 {PD0/CAN0RX}", "G2 {PD1/CAN0TX}", "H2 {PD2/U1Rx}", "H1 {PD3/U1Tx}", "E1 {PD4/CCP0}", "E2 {PD5}", "F2 {PD6/Fault}", "F1 {PD7/IDX0}"},
    // PORT E (0..7)
    { "A11 {PE0/PWM4}", "B12 {PE1/PWM5}", "B11 {PE2/PhB1}", "A12 {PE3/PhA1}", "NA", "NA", "NA", "NA"},
    // PORT F (0..7)
    { "M9 {PF0/PWM0}", "H12 {PF1/IDX1}", "J1 {PF2/LED1}", "J12 {PF3/LED0}", "NA", "NA", "NA", "NA"},
    // PORT G (0..7)
    { "K1 {PG0}", "K2 {PG1/PWM1}", "NA", "NA", "NA", "NA", "NA", "NA"},
    // ADC (0..3)
    { "B1 {AIN0}", "A1 {AIN1}", "B3 {AIN2}", "B2 {AIN3}", "NA", "NA", "NA", "NA"},
#else
    // PORT A (0..7)
    { "26 {PA0/U0Rx}", "27 {PA1/U0Tx}", "28 {PA2/SSI0Clk}", "29 {PA3/SSI0Fss}", "30 {PA4/SSI0Rx}", "31 {PA5/SSI0Tx}", "34 {PA6/CCP1}", "35 {PA7}"},
    // PORT B (0..7)
    { "66 {PB0/PWM2}", "67 {PB1/PWM3}", "70 {PB2/I2C0SCL}", "71 {PB3/I2C0SDA}", "92 {PB4/C0-}", "91 {PB5/C0o}", "90 {PB6/C0+}", "89 {PB7/TRST}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}", "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4/PhA0}", "24 {PC5}", "23 {PC6/PhB0}", "22 {PC7}"},
    // PORT D (0..7)
    { "10 {PD0/CAN0RX}", "11 {PD1/CAN0TX}", "12 {PD2/U1Rx}", "13 {PD3/U1Tx}", "95 {PD4/CCP0}", "96 {PD5}", "99 {PD6/Fault}", "100 {PD7/IDX0}"},
    // PORT E (0..7)
    { "72 {PE0/PWM4}", "73 {PE1/PWM5}", "74 {PE2/PhB1}", "75 {PE3/PhA1}", "NA", "NA", "NA", "NA"},
    // PORT F (0..7)
    { "47 {PF0/PWM0}", "61 {PF1/IDX1}", "60 {PF2/LED1}", "59 {PF3/LED0}", "NA", "NA", "NA", "NA"},
    // PORT G (0..7)
    { "19 {PG0}", "18 {PG1/PWM1}", "NA", "NA", "NA", "NA", "NA", "NA"},
    // ADC (0..3)
    { "1 {AIN0}", "2 {AIN1}", "5 {AIN2}", "6 {AIN3}", "NA", "NA", "NA", "NA"}, // {1}
#endif
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
        {  "Q", "PWM2"     },                                            // PORT B
        {  "Q", "PWM3"     },
        {  "Q", "I2C0SCL"  },
        {  "Q", "I2C0SDA"  },
        {  "Q", "C0-"      },
        {  "Q", "C0o"      },
        {  "Q", "C0+"      },
        {  "Q", "TRST"     }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK_SWCLK"},                                            // PORT C
        {  "Q", "TMS_SWDIO"},
        {  "Q", "TDI"      },
        {  "Q", "TDO_SWO"  },
        {  "Q", "PhA0"     },
        {  "Q", "Invalid"  },
        {  "Q", "PhB0"     },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "CAN0RX"   },                                            // PORT D
        {  "Q", "CAN0TX"   },
        {  "Q", "U1Rx"     },
        {  "Q", "U1Tx"     },
        {  "Q", "CCP0"     },
        {  "Q", "Invalid"  },
        {  "Q", "Fault"    },
        {  "Q", "IDX0"     }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM4"     },                                            // PORT E
        {  "Q", "PWM5"     },
        {  "Q", "PhB1"     },
        {  "Q", "PhA1"     },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "PWM0"     },                                            // PORT F
        {  "Q", "IDX1"     },
        {  "Q", "LED1"     },
        {  "Q", "LED0"     },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "Invalid"  },                                            // PORT G
        {  "Q", "PWM1"     },
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
    },
};




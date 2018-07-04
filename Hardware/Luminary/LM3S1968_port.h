/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S1968_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   **********************************************************************
   22.04.2009 Add ADC support                                            {1}
   15.05.2010 Add LM3S1965                                               {2}

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx}", "27 {PA1/U0Tx}", "28 {PA2/SSI0Clk}", "29 {PA3/SSI0Fss}", "30 {PA4/SSI0Rx}", "31 {PA5/SSI0Tx}", "34 {PA6/I2C1SCL}", "35 {PA7/I2C1SDA}"},
#if defined _LM3S1958                                                    // {2}
    // PORT B (0..7)
    { "66 {PB0/CCP0}", "67 {PB1/CCP2}", "70 {PB2/I2C0SCL}", "71 {PB3/I2C0SDA}", "92 {PB4}", "91 {PB5/CCP5}", "90 {PB6/CCP1}", "89 {PB7/TRST}"},
    // PORT C (0..7)
    { "80 {PC0/TCK}",  "79 {PC1/TMS}", "78 {PC2/TDI}", "77 {PC3/TDO}", "25 {PC4}", "24 {PC5}", "23 {PC6/CCP5}", "22 {PC7/CCP1}"},
    // PORT D (0..7)
    { "10 {PD0}", "11 {PD1}", "12 {PD2/U1Rx}", "13 {PD3/U1Tx}", "NA", "NA", "NA", "NA"},
#else
    // PORT B (0..7)
    { "66 {PB0/CCP0}", "67 {PB1/CCP2}", "70 {PB2/I2C0SCL}", "71 {PB3/I2C0SDA}", "92 {PB4/C0-}", "91 {PB5/C1-}", "90 {PB6/C0+}", "89 {PB7/TRST}"},
    // PORT C (0..7)
    { "80 {PC0/TCK}",  "79 {PC1/TMS}", "78 {PC2/TDI}", "77 {PC3/TDO}", "25 {PC4/PhA0}", "24 {PC5/C1+}", "23 {PC6/C2+}", "22 {PC7/C2-}"},
    // PORT D (0..7)
    { "10 {PD0/IDX0}", "11 {PD1/PWM1}", "12 {PD2/U1Rx}", "13 {PD3/U1Tx}", "NA", "NA", "NA", "NA"},
#endif
    // PORT E (0..7)
    { "72 {PE0/SSI1Clk}", "73 {PE1/SSI1Fss}", "74 {PE2/SSI1Rx}", "75 {PE3/SSI1Tx}", "NA", "NA", "NA", "NA"},
#if defined _LM3S1958                                                    // {2}
    // PORT F (0..7)
    { "47 {PF0}", "61 {PF1}", "60 {PF2}", "59 {PF3}", "58 {PF4}", "46 {PF5}", "43 {PF6}", "42 {PF7}"},
    // PORT G (0..7)
    { "19 {PG0/U2Rx}", "18 {PG1/U2Tx}", "17 {PG2}", "18 {PG3}", "41 {PG4}", "40 {PG5}", "37 {PG6}", "36 {PG7}"},
    // PORT H (0..7)
    { "86 {PH0/CCP6}", "85 {PH1/CCP7}", "84{PH2}", "83 {PH3}", "NA", "NA", "NA", "NA"},
#else
    // PORT F (0..7)
    { "47 {PF0/PWM0}", "61 {PF1/IDX1}", "60 {PF2/LED1}", "59 {PF3/LED0}", "58 {PF4/CO0}", "46 {PF5}", "43 {PF6/CCP1}", "42 {PF7}"},
    // PORT G (0..7)
    { "19 {PG0/U2Rx}", "18 {PG1/U2Tx}", "17 {PG2/PWM0}", "18 {PG3}", "41 {PG4/CCP3}", "40 {PG5}", "37 {PG6/PhA1}", "36 {PG7/PhB1}"},
    // PORT H (0..7)
    { "86 {PH0/PWM2}", "85 {PH1/PWM3}", "84{PH2}", "83 {PH3/Fault}", "NA", "NA", "NA", "NA"},
#endif
    // ADC (0..3)
    { "1 {AIN0}", "2 {AIN1}", "5 {AIN2}", "6 {AIN3}", "100 {AIN4}", "99 {AIN5}", "96 {AIN6}", "65{AIN7}"}, // {1}
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
        {  "Q", "CCP0"  },                                               // PORT B
        {  "Q", "CCP2"  },
        {  "Q", "I2C0SCL"  },
        {  "Q", "I2C0SDA"  },
#if defined _LM3S1958                                                    // {2}
        {  "Q", "Invalid"  },
        {  "Q", "CCP5"  },
        {  "Q", "CCP1"  },
#else
        {  "Q", "C0-"  },
        {  "Q", "C1-"  },
        {  "Q", "C0+"  },
#endif
        {  "Q", "TRST" }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK"  },                                                // PORT C
        {  "Q", "TMS"  },
        {  "Q", "TDI"  },
        {  "Q", "TDO"  },
#if defined _LM3S1958                                                    // {2}
        {  "Q", "Invalid" },
        {  "Q", "Invalid" },
        {  "Q", "CCP3"  },
        {  "Q", "CCP4"  }
#else
        {  "Q", "PhA0"  },
        {  "Q", "C1+"  },
        {  "Q", "C2+"  },
        {  "Q", "C2-"  }
#endif
    },
    {
        //  Q    Periph.
#if defined _LM3S1958                                                    // {2}
        {  "Q", "Invalid" },
        {  "Q", "Invalid" },
#else
        {  "Q", "IDX0"  },                                               // PORT D
        {  "Q", "PWM1"  },
#endif
        {  "Q", "U1Rx"  },
        {  "Q", "U1Tx"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid" },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "SSI1Clk"  },                                            // PORT E
        {  "Q", "SSI1Fss"  },
        {  "Q", "SSI1Rx"  },
        {  "Q", "SSI1Tx"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
    },
    {
        //  Q    Periph.
#if defined _LM3S1958                                                    // {2}
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
#else
        {  "Q", "PWM0"  },                                               // PORT F
        {  "Q", "IDX1"  },
        {  "Q", "LED1"  },
        {  "Q", "LED0"  },
        {  "Q", "CO0"  },
        {  "Q", "Invalid"  },
        {  "Q", "CCP1"  },
        {  "Q", "Invalid"  }
#endif
    },
    {
        //  Q    Periph.
        {  "Q", "U2Rx"  },                                               // PORT G
        {  "Q", "U2Tx"  },
#if defined _LM3S1958                                                    // {2}
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  },
        {  "Q", "Invalid"  }
#else
        {  "Q", "PWM0"  },
        {  "Q", "Invalid"  },
        {  "Q", "CCP3"  },
        {  "Q", "Invalid"  },
        {  "Q", "PhA1"  },
        {  "Q", "PhB1"  }
#endif
    },
    {
        //  Q    Periph.
#if defined _LM3S1958                                                    // {2}
        {  "Q", "CCP6"  },                                               // PORT H
        {  "Q", "CCP7"  },
        {  "Q", "Invalid" },
        {  "Q", "Invalid" },
#else
        {  "Q", "PWM2"  },                                               // PORT H
        {  "Q", "PWM3"  },
        {  "Q", "Invalid"  },
        {  "Q", "Fault"  },
#endif
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
        {  "AIN4",    "AIN4"     },
        {  "AIN5",    "AIN5"     },
        {  "AIN6",    "AIN6"     },
        {  "AIN7",    "AIN7"     },
    },
};




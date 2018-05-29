/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S5732_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   **********************************************************************

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "17 {PA0/U0Rx}", "18 {PA1/U0Tx}", "19 {PA2/SSI0Clk}", "20 {PA3/SSI0Fss}", "21 {PA4/SSI0Rx}", "22 {PA5/SSI0Tx}", "25 {PA6/I2C1SCL}", "26 {PA7/I2C1SDA}"},
    // PORT B (0..7)
    { "41 {PB0/U1Rx}", "42 {PB1/U1Tx}", "47 {PB2/I2C0SCL}", "27 {PB3/I2C0SDA}", "58 {PB4}", "57 {PB5/CCP0}", "56 {PB6/CCP1}", "55 {PB7/NMI}"},
    // PORT C (0..7)
    { "52 {PC0/TCK/SWCLK}", "51 {PC1/TMS/SWDIO}", "50 {PC2/TDI}", "49 {PC3/TDO/SWO}", "11 {PC4/CCP2}", "14 {PC5/USB0EPEN}", "15 {PC6/USB0PFLT}", "16 {PC7/CCP4}"},
    // PORT D (0..7)
    { "61 {PD0/CAN0Rx}", "62 {PD1/CAN0Tx}", "63 {PD2/ADC5}", "64 {PD3/ADC4}", "NA", "NA", "NA", "NA"},
    // PORT E (0..7)
    { "6 {PE0/ADC3}", "5 {PE1/ADC2}", "2 {PE2/ADC1}", "1 {PE3/ADC0}", "8 {PE4/CCP3}", "NA", "NA", "NA"},
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
        {  "Q", "U1Rx"     },                                            // PORT B
        {  "Q", "U1Tx"     },
        {  "Q", "I2C0SCL"  },
        {  "Q", "I2C0SDA"  },
        {  "Q", "invalid"  },
        {  "Q", "CCP0"     },
        {  "Q", "CCP1"     },
        {  "Q", "NMI"      }
    },
    {
        //  Q    Periph.
        {  "Q", "TCK/SWCLK"},                                            // PORT C
        {  "Q", "TMS/SWDIO"},
        {  "Q", "TDI"      },
        {  "Q", "TDO/SWO"  },
        {  "Q", "CCP2"     },
        {  "Q", "USB0EPEN" },
        {  "Q", "USB0PFLT" },
        {  "Q", "CCP4"     }
    },
    {
        //  Q    Periph.
        {  "Q", "CAN0Rx"   },                                            // PORT D
        {  "Q", "CAN0Tx"   },
        {  "Q", "ADC5"     },
        {  "Q", "ADC4"     },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  }
    },
    {
        //  Q    Periph.
        {  "Q", "ADC3"     },                                            // PORT E
        {  "Q", "ADC2"     },
        {  "Q", "ADC1"     },
        {  "Q", "ADC0"     },
        {  "Q", "CCP3"     },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  },
        {  "Q", "invalid"  }
    }
};




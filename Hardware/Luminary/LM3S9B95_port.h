/************************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LM3S9B95_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2018
   **********************************************************************
   05.11.2009 Add _LM3S9B90
   13.07.2010 Add ADC inputs

*/

#ifdef _LM3S9B90
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx/I2C1SCL/U1Rx}", "27 {PA1/U0Tx/I2C1SDA/U1Tx}", "28 {PA2/SSI0Clk/I2S0RXSD}", "29 {PA3/SSI0Fss/I2S0RXMCLK}", "30 {PA4/SSI0Rx/CAN0Rx/I2S0TXSCK}", "31 {PA5/SSI0Tx/CAN0Tx/I2STXWS}", "34 {PA6/I2C1SCL/CCP1/CAN0Rx/USB0EPEN/U1CTS}", "35 {PA7/I2C1SDA/CCP4/CAN0Tx/CCP3/USB0PFLT/U1DCD}"},
    // PORT B (0..7)
    { "66 {PB0/CCP0/U1Rx}", "67 {PB1/CCP2/CCP1/U1Tx}", "72 {PB2/I2C0SCL/CCP3/CCP0/USB0EPEN}", "65 {PB3/I2C0SDA/USB0PFLT}", "92 {PB4/U2Rx/CAN0Rx/U1Rx/EPI0S23/AIN10}", "91 {PB5/C0o/CCP5/CCP6/CCP0/CAN0Tx/CCP2/U1Tx/EPI0S22/AIN11}", "90 {PB6/CCP1/CCP7/C0o/CCP5/I2S0TXSCK}", "89 {PB7/NMI}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}", "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4/CCP5/CCP2/CCP4/EPI0S2/CCP1}", "24 {PC5/CCP1/C1o/C0o/CCP3/USB0EPEN, EPI0S3}", "23 {PC6/CCP3/C2o/U1Rx/CCP0/USB0PFLT/EPI0S4}", "22 {PC7/CCP4/CCP0/U1Tx/USB0PFLT/C1o/EPI0S5}"},
    // PORT D (0..7)
    { "10 {PD0/CAN0Rx/U2Rx/U1Rx/CCP6/I2S0RXSCK/U1CTS/AIN15}", "11 {PD1/CAN0Tx/U2Tx/U1Tx/CCP7/I2S0RXWS/U1DCD/CCP2/AIN14}", "12 {PD2/U1Rx/CCP6/CCP5/EPI0S20/AIN13}", "13 {PD3/U1Tx/CCP7/CCP0/EPI0S21/AIN12}", "97 {PD4/CCP0/CCP3/I2S0RXSD/U1RI/EPI0S19/AIN7}", "98 {PD5/CCP2/CCP4/I2S0RXMCLK/U2Rx/EPI0S28/AIN6}", "99 {PD6/I2S0TXSCK/U2Tx/EPI0S29/AIN5}", "100 {PD7/C0o/CCP1/I2S0TXWS/U1DTR/EPI0S30/AIN4}"},
    // PORT E (0..7)
    { "74 {PE0/SSI1Clk/CCP3/EPI0S8/USB0PFLT}", "75 {PE1/SSI1Fss/CCP2/CCP6/EPI0S9}", "95 {PE2/CCP4/SSI1Rx/CCP2/EPI0S24/AIN9}", "96 {PE3/CCP1/SSI1Tx/CCP7/EPI0S25/AIN8}", "6 {PE4/CCP3/U2Tx/CCP2/I2S0TXWS/AIN3}", "5 {PE5/CCP5/I2S0TXSD/AIN2}", "2 {PE6/C1o/U1CTS/AIN1}", "1 {PE7/C2o/U1DCD/AIN0}"},
    // PORT F (0..7)
    { "47 {PF0/CAN1Rx/I2S0TXSD/U1DSR}", "61 {PF1/CAN1Tx/I2S0TXMCLK/U1RTS/CCP3}", "60 {PF2/LED1/SSI1Clk}", "59 {PF3/LED0/SSI1Fss}", "42 {PF4/CCP0/C0o/EPI0S12/SSI1Rx}", "41 {PF5/CCP2/C1o/EPI0S15/SSI1Tx}", "NA", "NA"},
    // PORT G (0..7)
    { "19 {PG0/U2Rx/I2C1SCL/USB0EPEN/EPI0S13}", "18 {PG1/U2Tx/I2C1SDA/EPI0S14}", "NA", "NA", "NA", "NA", "NA", "36 {PG7/CCP5/EPI0S31}"},
    // PORT H (0..7)
    { "86 {PH0/CCP6/EPI0S6}", "85 {PH1/CCP7/EPI0S7}", "84 {PH2/C1o/EPI0S1}", "83 {PH3/USB0EPEN/EPI0S0}", "76 {PH4/USB0PFLT/EPI0S10/SSI1Clk}", "63 {PH5/EPI0S11/SSI1Fss}", "62 {PH6/EPI0S26/SSI1Rx}", "15 {PH7/EPI0S27/SSI1Tx}"},
    // PORT J (0..7)
    { "14 {PJ0/EPI0S16/I2C1SCL}", "87 {PJ1/EPI0S17/USB0PFLT/I2C1SDA}", "39 {PJ2/EPI0S18/CCP0}", "NA", "NA", "NA", "NA", "NA"},
};
#else
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..7)
    { "26 {PA0/U0Rx/I2C1SCL/U1Rx}", "27 {PA1/U0Tx/I2C1SDA/U1Tx}", "28 {PA2/SSI0Clk/PWM4/I2S0RXSD}", "29 {PA3/SSI0Fss/PWM5/I2S0RXMCLK}", "30 {PA4/SSI0Rx/PWM6/CAN0Rx/I2S0TXSCK}", "31 {PA5/SSI0Tx/PWM7/CAN0Tx/I2STXWS}", "34 {PA6/I2C1SCL/CCP1/PWM0/PWM4/CAN0Rx/USB0EPEN/U1CTS}", "35 {PA7/I2C1SDA/CCP4/PWM1/PWM5/CAN0Tx/CCP3/USB0PFLT/U1DCD}"},
    // PORT B (0..7)
    { "66 {PB0/CCP0/PWM2/U1Rx}", "67 {PB1/CCP2/PWM3/CCP1/U1Tx}", "72 {PB2/I2C0SCL/IDX0/CCP3/CCP0/USB0EPEN}", "65 {PB3/I2C0SDA/Fault0/Fault3/USB0PFLT}", "92 {PB4/U2Rx/CAN0Rx/IDX0/U1Rx/EPI0S23/AIN10}", "91 {PB5/C0o/CCP5/CCP6/CCP0/CAN0Tx/CCP2/U1Tx/EPI0S22/AIN11}", "90 {PB6/CCP1/CCP7/C0o/Fault1/IDX0/CCP5/I2S0TXSCK/VREFA}", "89 {PB7/NMI}"},
    // PORT C (0..7)
    { "80 {PC0/TCK/SWCLK}", "79 {PC1/TMS/SWDIO}", "78 {PC2/TDI}", "77 {PC3/TDO/SWO}", "25 {PC4/CCP5/PhA0/PWM6/CCP2/CCP4/EPI0S2/CCP1}", "24 {PC5/CCP1/C1o/C0o/Fault2/CCP3/USB0EPEN, EPI0S3}", "23 {PC6/CCP3/PhB0/C2o/PWM7/U1Rx/CCP0/USB0PFLT/EPI0S4}", "22 {PC7/CCP4/PhB0/CCP0/U1Tx/USB0PFLT/C1o/EPI0S5}"},
    // PORT D (0..7)
    { "10 {PD0/PWM0/CAN0Rx/IDX0/U2Rx/U1Rx/CCP6/I2S0RXSCK/U1CTS/AIN15}", "11 {PD1/PWM1/CAN0Tx/PhA0/U2Tx/U1Tx/CCP7/I2S0RXWS/U1DCD/CCP2/PhB1/AIN14}", "12 {PD2/U1Rx/CCP6/PWM2/CCP5/EPI0S20/AIN13}", "13 {PD3/U1Tx/CCP7/PWM3/CCP0/EPI0S21/AIN12}", "97 {PD4/CCP0/CCP3/I2S0RXSD/U1RI/EPI0S19/AIN7}", "98 {PD5/CCP2/CCP4/I2S0RXMCLK/U2Rx/EPI0S28/AIN6}", "99 {PD6/Fault0/I2S0TXSCK/U2Tx/EPI0S29/AIN5}", "100 {PD7/IDX0/C0o/CCP1/I2S0TXWS/U1DTR/EPI0S30/AIN4}"},
    // PORT E (0..7)
    { "74 {PE0/PWM4/SSI1Clk/CCP3/EPI0S8/USB0PFLT}", "75 {PE1/PWM5/SSI1Fss/Fault0/CCP2/CCP6/EPI0S9}", "95 {PE2/CCP4/SSI1Rx/PhB1/PhA0/CCP2/EPI0S24/AIN9}", "96 {PE3/CCP1/SSI1Tx/PhA1/PhB0/CCP7/EPI0S25/AIN8}", "6 {PE4/CCP3/Fault0/U2Tx/CCP2/I2S0TXWS/AIN3}", "5 {PE5/CCP5/I2S0TXSD/AIN2}", "2 {PE6/PWM4/C1o/U1CTS/AIN1}", "1 {PE7/PWM5/C2o/U1DCD/AIN0}"},
    // PORT F (0..7)
    { "47 {PF0/CAN1Rx/PhB0/PWM0/I2S0TXSD/U1DSR}", "61 {PF1/CAN1Tx/IDX1/PWM1/I2S0TXMCLK/U1RTS/CCP3}", "60 {PF2/LED1/PWM4/PWM2/SSI1Clk}", "59 {PF3/LED0/PWM5/PWM3/SSI1Fss}", "42 {PF4/CCP0/C0o/Fault0/EPI0S12/SSI1Rx}", "41 {PF5/CCP2/C1o/EPI0S15/SSI1Tx}", "NA", "NA"},
    // PORT G (0..7)
    { "19 {PG0/U2Rx/PWM0/I2C1SCL/PWM4/USB0EPEN/EPI0S13}", "18 {PG1/U2Tx/PWM1/I2C1SDA/PWM5/EPI0S14}", "NA", "NA", "NA", "NA", "NA", "36 {PG7/PhB1/PWM7/CCP5/EPI0S31}"},
    // PORT H (0..7)
    { "86 {PH0/CCP6/PWM2/EPI0S6/PWM4}", "85 {PH1/CCP7/PWM3/EPI0S7/PWM5}", "84 {PH2/IDX1/C1o/Fault3/EPI0S1}", "83 {PH3/PhB0/Fault0/USB0EPEN/EPI0S0}", "76 {PH4/USB0PFLT/EPI0S10/SSI1Clk}", "63 {PH5/EPI0S11/Fault2/SSI1Fss}", "62 {PH6/EPI0S26/PWM4/SSI1Rx}", "15 {PH7/EPI0S27/PWM5/SSI1Tx}"},
    // PORT J (0..7)
    { "14 {PJ0/EPI0S16/PWM0/I2C1SCL}", "87 {PJ1/EPI0S17/USB0PFLT/PWM1/I2C1SDA}", "39 {PJ2/EPI0S18/CCP0/Fault0}", "50 {PJ3/EPI0S19/U1CTS/CCP6}", "52 {PJ4/EPI0S28/U1DCD/CCP4}", "53 {PJ5/EPI0S29/U1DSR/CCP2}", "54 {PJ6/EPI0S30/U1RTS/CCP1}", "55 {PJ7/U1DTR/CCP0}"},
};
#endif

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][12] = {
    {
        //  Q    Periph.1, 11
        {  "Q", "U0Rx",   "-",    "-", "-",    "-",      "-",      "-",    "I2C1SCL", "U1Rx",      "-", "-" }, // PORT A
        {  "Q", "U0Tx",   "-",    "-", "-",    "-",      "-",      "-",    "I2C1SDA", "U1Tx",      "-", "-" },
#ifdef _LM3S9B90                                                         // B90 doesn't have PWM
        {  "Q", "SSI0Clk","-",    "-", "-",    "-",      "-",      "-",    "-",       "I2S0RXSD",  "-", "-" },
        {  "Q", "SSI0Fss","-",    "-", "-",    "-",      "-",      "-",    "-",       "I2S0RXMCLK","-", "-" },
        {  "Q", "SSI0Rx", "-",    "-", "-",    "CAN0Rx", "-",      "-",    "-",       "I2S0TXSCK", "-", "-" },
        {  "Q", "SSI0Tx", "-",    "-", "-",    "CAN0Tx", "-",      "-",    "-",       "I2STXWS",   "-", "-" },
        {  "Q", "I2C1SCL","CCP1", "-", "-",    "-",      "CAN0Rx", "-",    "USB0EPEN","U1CTS",     "-", "-" },
        {  "Q", "I2C1SDA","CCP4", "-", "-",    "-",      "CAN0Tx", "CCP3", "USB0PFLT","U1DCD",     "-", "-" }
#else
        {  "Q", "SSI0Clk","-",    "-", "PWM4", "-",      "-",      "-",    "-",       "I2S0RXSD",  "-", "-" },
        {  "Q", "SSI0Fss","-",    "-", "PWM5", "-",      "-",      "-",    "-",       "I2S0RXMCLK","-", "-" },
        {  "Q", "SSI0Rx", "-",    "-", "PWM6", "CAN0Rx", "-",      "-",    "-",       "I2S0TXSCK", "-", "-" },
        {  "Q", "SSI0Tx", "-",    "-", "PWM7", "CAN0Tx", "-",      "-",    "-",       "I2STXWS",   "-", "-" },
        {  "Q", "I2C1SCL","CCP1", "-", "PWM0", "PWM4",   "CAN0Rx", "-",    "USB0EPEN","U1CTS",     "-", "-" },
        {  "Q", "I2C1SDA","CCP4", "-", "PWM1", "PWM5",   "CAN0Tx", "CCP3", "USB0PFLT","U1DCD",     "-", "-" }
#endif
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "Q", "CCP0",   "-",     "-",    "-",     "U1Rx",   "-",    "-",    "-",        "-",         "-", "-"    }, // PORT B
        {  "Q", "CCP2",   "-",     "-",    "CCP1",  "U1Tx",   "-",    "-",    "-",        "-",         "-", "-"    },
        {  "Q", "I2C0SCL","-",     "-",    "CCP3",  "CCP0",   "-",    "-",    "USB0EPEN", "-",         "-", "-"    },
        {  "Q", "I2C0SDA","-",     "-",    "-",     "-",      "-",    "-",    "USB0PFLT", "-",         "-", "-"    },
        {  "AIN10", "-",      "-",     "-",    "U2Rx",  "CAN0Rx", "-",    "U1Rx", "EPI0S23",  "-",         "-", "-"    },
        {  "AIN11", "C0o",    "CCP5",  "CCP6", "CCP0",  "CAN0Tx", "CCP2", "U1Tx", "EPI0S22",  "-",         "-", "-"    },
        {  "Q", "CCP1",   "CCP7",  "C0o",  "-",     "-",      "CCP5", "-",    "-",        "I2S0TXSCK", "-", "-"    },
#else
        {  "Q", "CCP0",   "PWM2",  "-",    "-",     "U1Rx",   "-",    "-",    "-",        "-",         "-", "-"    }, // PORT B
        {  "Q", "CCP2",   "PWM3",  "-",    "CCP1",  "U1Tx",   "-",    "-",    "-",        "-",         "-", "-"    },
        {  "Q", "I2C0SCL","IDX0",  "-",    "CCP3",  "CCP0",   "-",    "-",    "USB0EPEN", "-",         "-", "-"    },
        {  "Q", "I2C0SDA","Fault0","-",    "Fault3","-",      "-",    "-",    "USB0PFLT", "-",         "-", "-"    },
        {  "AIN10", "-",      "-",     "-",    "U2Rx",  "CAN0Rx", "IDX0", "U1Rx", "EPI0S23",  "-",         "-", "-"    },
        {  "AIN11", "C0o",    "CCP5",  "CCP6", "CCP0",  "CAN0Tx", "CCP2", "U1Tx", "EPI0S22",  "-",         "-", "-"    },
        {  "Q", "CCP1",   "CCP7",  "C0o",  "Fault1","IDX0",   "CCP5", "-",    "-",        "I2S0TXSCK", "-", "-"    },
#endif
        {  "Q", "-",      "-",     "-",    "NMI",   "-",      "-",    "-",    "-",        "-",         "-", "-"    }
    },
    {
        //  Q    Periph.1, 11
        {  "Q", "-",    "-",    "TCK/SWCLK", "-",      "-",    "-",        "-",        "-",      "-",    "-", "-"    }, // PORT C
        {  "Q", "-",    "-",    "TMS/SWDIO", "-",      "-",    "-",        "-",        "-",      "-",    "-", "-"    },
        {  "Q", "-",    "-",    "TDI",       "-",      "-",    "-",        "-",        "-",      "-",    "-", "-"    },
        {  "Q", "-",    "-",    "TDO/SWO",   "-",      "-",    "-",        "-",        "-",      "-",    "-", "-"    },
#ifdef _LM3S9B90
        {  "Q", "CCP5", "PhA0", "-",         "PWM6",   "CCP2", "CCP4",     "-",        "EPI0S2", "CCP1", "-", "-"    },
        {  "Q", "CCP1", "C1o",  "C0o",       "Fault2", "CCP3", "USB0EPEN", "-",        "EPI0S3", "-",    "-", "-"    },
        {  "Q", "CCP3", "PhB0", "C2o",       "PWM7",   "U1Rx", "CCP0",     "USB0PFLT", "EPI0S4", "-",    "-", "-"    },
        {  "Q", "CCP4", "PhB0", "-",         "CCP0",   "U1Tx", "USB0PFLT", "C1o",      "EPI0S5", "-",    "-", "-"    }
#else
        {  "Q", "CCP5", "-",    "-",         "-",      "CCP2", "CCP4",     "-",        "EPI0S2", "CCP1", "-", "-"    },
        {  "Q", "CCP1", "C1o",  "C0o",       "-",      "CCP3", "USB0EPEN", "-",        "EPI0S3", "-",    "-", "-"    },
        {  "Q", "CCP3", "-",    "C2o",       "-",      "U1Rx", "CCP0",     "USB0PFLT", "EPI0S4", "-",    "-", "-"    },
        {  "Q", "CCP4", "-",    "-",         "CCP0",   "U1Tx", "USB0PFLT", "C1o",      "EPI0S5", "-",    "-", "-"    }
#endif
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "AIN15", "-",      "CAN0Rx", "-",    "U2Rx", "U1Rx", "CCP6", "-", "I2S0RXSCK", "U1CTS", "-",      "-"    }, // PORT D
        {  "AIN14", "-",      "CAN0Tx", "-",    "U2Tx", "U1Tx", "CCP7", "-", "I2S0RXWS",  "U1DCD", "CCP2",   "-" },
        {  "AIN13", "U1Rx",   "CCP6",   "-",    "CCP5", "-",    "-",    "-", "EPI0S20",   "-",     "-",      "-"    },
        {  "AIN12", "U1Tx",   "CCP7",   "-",    "CCP0", "-",    "-",    "-", "EPI0S21",   "-",     "-",      "-"    },
        {  "AIN7", "CCP0",   "CCP3",   "-",    "-",    "-",    "-",    "-", "I2S0RXSD",  "U1RI",  "EPI0S19", "-"   },
        {  "AIN6", "CCP2",   "CCP4",   "-",    "-",    "-",    "-",    "-", "I2S0RXMCLK","U2Rx",  "EPI0S28", "-"   },
        {  "AIN5", "-",      "-",      "-",    "-",    "-",    "-",    "-", "I2S0TXSCK", "U2Tx",  "EPI0S29", "-"   },
        {  "AIN4", "-",      "C0o",    "CCP1", "-",    "-",    "-",    "-", "I2S0TXWS",  "U1DTR", "EPI0S30", "-"   }
#else
        {  "AIN15", "PWM0",   "CAN0Rx", "IDX0", "U2Rx", "U1Rx", "CCP6", "-", "I2S0RXSCK", "U1CTS", "-",      "-"    }, // PORT D
        {  "AIN14", "PWM1",   "CAN0Tx", "PhA0", "U2Tx", "U1Tx", "CCP7", "-", "I2S0RXWS",  "U1DCD", "CCP2",   "PhB1" },
        {  "AIN13", "U1Rx",   "CCP6",   "PWM2", "CCP5", "-",    "-",    "-", "EPI0S20",   "-",     "-",      "-"    },
        {  "AIN12", "U1Tx",   "CCP7",   "PWM3", "CCP0", "-",    "-",    "-", "EPI0S21",   "-",     "-",      "-"    },
        {  "AIN7", "CCP0",   "CCP3",   "-",    "-",    "-",    "-",    "-", "I2S0RXSD",  "U1RI",  "EPI0S19", "-"   },
        {  "AIN6", "CCP2",   "CCP4",   "-",    "-",    "-",    "-",    "-", "I2S0RXMCLK","U2Rx",  "EPI0S28", "-"   },
        {  "AIN5", "Fault0", "-",      "-",    "-",    "-",    "-",    "-", "I2S0TXSCK", "U2Tx",  "EPI0S29", "-"   },
        {  "AIN4", "IDX0",   "C0o",    "CCP1", "-",    "-",    "-",    "-", "I2S0TXWS",  "U1DTR", "EPI0S30", "-"   }
#endif
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "Q", "-",      "SSI1Clk", "CCP3",   "-",      "-",    "-",    "-", "EPI0S8",  "USB0PFLT", "-", "-"    }, // PORT E
        {  "Q", "-",      "SSI1Fss", "-",      "CCP2",   "CCP6", "-",    "-", "EPI0S9",  "-",        "-", "-"    },
        {  "AIN9", "CCP4",   "SSI1Rx",  "-",      "-",      "CCP2", "-",    "-", "EPI0S24", "-",        "-", "-"    },
        {  "AIN8", "CCP1",   "SSI1Tx",  "-",      "-",      "CCP7", "-",    "-", "EPI0S25", "-",        "-", "-"    },
        {  "AIN3", "CCP3",   "-",       "-",      "-",      "U2Tx", "CCP2", "-", "-",       "I2S0TXWS", "-", "-"    },
        {  "AIN2", "CCP5",   "-",       "-",      "-",      "-",    "-",    "-", "-",       "I2S0TXSD", "-", "-"    },
        {  "AIN1", "-",      "C1o",     "-",      "-",      "-",    "-",    "-", "-",       "U1CTS",    "-", "-"    },
        {  "AIN0", "-",      "C2o",     "-",      "-",      "-",    "-",    "-", "-",       "U1DCD",    "-", "-"    }

#else
        {  "Q", "PWM4",   "SSI1Clk", "CCP3",   "-",      "-",    "-",    "-", "EPI0S8",  "USB0PFLT", "-", "-"    }, // PORT E
        {  "Q", "PWM5",   "SSI1Fss", "Fault0", "CCP2",   "CCP6", "-",    "-", "EPI0S9",  "-",        "-", "-"    },
        {  "AIN9", "CCP4",   "SSI1Rx",  "PhB1",   "PhA0",   "CCP2", "-",    "-", "EPI0S24", "-",        "-", "-"    },
        {  "AIN8", "CCP1",   "SSI1Tx",  "PhA1",   "PhB0",   "CCP7", "-",    "-", "EPI0S25", "-",        "-", "-"    },
        {  "AIN3", "CCP3",   "-",       "-",      "Fault0", "U2Tx", "CCP2", "-", "-",       "I2S0TXWS", "-", "-"    },
        {  "AIN2", "CCP5",   "-",       "-",      "-",      "-",    "-",    "-", "-",       "I2S0TXSD", "-", "-"    },
        {  "AIN1", "PWM4",   "C1o",     "-",      "-",      "-",    "-",    "-", "-",       "U1CTS",    "-", "-"    },
        {  "AIN0", "PWM5",   "C2o",     "-",      "-",      "-",    "-",    "-", "-",       "U1DCD",    "-", "-"    }
#endif
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "Q", "CAN1Rx", "-",    "-",    "-",      "-", "-", "-", "I2S0TXSD",   "U1DSR",   "-",    "-"    }, // PORT F
        {  "Q", "CAN1Tx", "-",    "-",    "-",      "-", "-", "-", "I2S0TXMCLK", "U1RTS",   "CCP3", "-"    },
        {  "Q", "LED1",   "-",    "-",    "-",      "-", "-", "-", "-",          "SSI1Clk", "-",    "-"    },
        {  "Q", "LED0",   "-",    "-",    "-",      "-", "-", "-", "-",          "SSI1Fss", "-",    "-"    },
        {  "Q", "CCP0",   "C0o",  "-",    "-",      "-", "-", "-", "EPI0S12",    "SSI1Rx",  "-",    "-"    },
#else
        {  "Q", "CAN1Rx", "PhB0", "PWM0", "-",      "-", "-", "-", "I2S0TXSD",   "U1DSR",   "-",    "-"    }, // PORT F
        {  "Q", "CAN1Tx", "IDX1", "PWM1", "-",      "-", "-", "-", "I2S0TXMCLK", "U1RTS",   "CCP3", "-"    },
        {  "Q", "LED1",   "PWM4", "-",    "PWM2",   "-", "-", "-", "-",          "SSI1Clk", "-",    "-"    },
        {  "Q", "LED0",   "PWM5", "-",    "PWM3",   "-", "-", "-", "-",          "SSI1Fss", "-",    "-"    },
        {  "Q", "CCP0",   "C0o",  "-",    "Fault0", "-", "-", "-", "EPI0S12",    "SSI1Rx",  "-",    "-"    },
#endif
        {  "Q", "CCP2",   "C1o",  "-",    "-",      "-", "-", "-", "EPI0S15",    "SSI1Tx",  "-",    "-"    },
        {  "Q", "-",      "-",    "-",    "-",      "-", "-", "-", "-",          "-",       "-",    "-"    },
        {  "Q", "-",      "-",    "-",    "-",      "-", "-", "-", "-",          "-",       "-",    "-"    }
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "Q", "U2Rx",   "-",    "I2C1SCL", "-",    "-", "-", "USB0EPEN", "EPI0S13", "-",       "-", "-"    }, // PORT G
        {  "Q", "U2Tx",   "-",    "I2C1SDA", "-",    "-", "-", "-",        "EPI0S14", "-",       "-", "-"    },
#else
        {  "Q", "U2Rx",   "PWM0", "I2C1SCL", "PWM4", "-", "-", "USB0EPEN", "EPI0S13", "-",       "-", "-"    }, // PORT G
        {  "Q", "U2Tx",   "PWM1", "I2C1SDA", "PWM5", "-", "-", "-",        "EPI0S14", "-",       "-", "-"    },
#endif
        {  "Q", "-",      "-",    "-",       "-",    "-", "-", "-",        "-",       "-",       "-", "-"    },
        {  "Q", "-",      "-",    "-",       "-",    "-", "-", "-",        "-",       "-",       "-", "-"    },
        {  "Q", "-",      "-",    "-",       "-",    "-", "-", "-",        "-",       "-",       "-", "-"    },
        {  "Q", "-",      "-",    "-",       "-",    "-", "-", "-",        "-",       "-",       "-", "-"    },
        {  "Q", "-",      "-",    "-",       "-",    "-", "-", "-",        "-",       "-",       "-", "-"    },
#ifdef _LM3S9B90
        {  "Q", "-",      "-",    "-",       "-",    "-", "-", "-",        "CCP5",    "EPI0S31", "-", "-"    }
#else
        {  "Q", "PhB1",   "-",    "-",       "PWM7", "-", "-", "-",        "CCP5",    "EPI0S31", "-", "-"    }
#endif
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "Q", "CCP6", "-",      "-", "-",        "-", "-", "-", "EPI0S6",  "-",    "-",      "-"       }, // PORT H
        {  "Q", "CCP7", "-",      "-", "-",        "-", "-", "-", "EPI0S7",  "-",    "-",      "-"       },
        {  "Q", "-",    "C1o",    "-", "-",        "-", "-", "-", "EPI0S1",  "-",    "-",      "-"       },
        {  "Q", "-",    "-",      "-", "USB0EPEN", "-", "-", "-", "EPI0S0",  "-",    "-",      "-"       },
        {  "Q", "-",    "-",      "-", "USB0PFLT", "-", "-", "-", "EPI0S10", "-",    "-",      "SSI1Clk" },
        {  "Q", "-",    "-",      "-", "-",        "-", "-", "-", "EPI0S11", "-",    "-",      "SSI1Fss" },
        {  "Q", "-",    "-",      "-", "-",        "-", "-", "-", "EPI0S26", "-",    "-",      "SSI1Rx"  },
        {  "Q", "-",    "-",      "-", "-",        "-", "-", "-", "EPI0S27", "-",    "-",      "SSI1Tx"  }
#else
        {  "Q", "CCP6", "PWM2",   "-", "-",        "-", "-", "-", "EPI0S6",  "PWM4", "-",      "-"       }, // PORT H
        {  "Q", "CCP7", "PWM3",   "-", "-",        "-", "-", "-", "EPI0S7",  "PWM5", "-",      "-"       },
        {  "Q", "IDX1", "C1o",    "-", "Fault3",   "-", "-", "-", "EPI0S1",  "-",    "-",      "-"       },
        {  "Q", "PhB0", "Fault0", "-", "USB0EPEN", "-", "-", "-", "EPI0S0",  "-",    "-",      "-"       },
        {  "Q", "-",    "-",      "-", "USB0PFLT", "-", "-", "-", "EPI0S10", "-",    "-",      "SSI1Clk" },
        {  "Q", "-",    "-",      "-", "-",        "-", "-", "-", "EPI0S11", "-",    "Fault2", "SSI1Fss" },
        {  "Q", "-",    "-",      "-", "-",        "-", "-", "-", "EPI0S26", "-",    "PWM4",   "SSI1Rx"  },
        {  "Q", "-",    "-",      "-", "-",        "-", "-", "-", "EPI0S27", "-",    "PWM5",   "SSI1Tx"  }
#endif
    },
    {
        //  Q    Periph.1, 11
#ifdef _LM3S9B90
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S16", "-",        "-",      "I2C1SCL"    }, // PORT J
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S17", "USB0PFLT", "-",      "I2C1SDA"    },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S18", "CCP0",     "-",      "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "-",       "-",        "-",      "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "-",       "-",        "-",      "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "-",       "-",        "-",      "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "-",       "-",        "-",      "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "-",       "-",        "CC-P0",  "-"          }
#else
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S16", "-",        "PWM0",   "I2C1SCL"    }, // PORT J
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S17", "USB0PFLT", "PWM1",   "I2C1SDA"    },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S18", "CCP0",     "Fault0", "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S19", "U1CTS",    "CCP6",   "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S28", "U1DCD",    "CCP4",   "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S29", "U1DSR",    "CCP2",   "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "EPI0S30", "U1RTS",    "CCP1",   "-"          },
        {  "Q", "-",   "-", "-", "-", "-", "-", "-", "-",       "U1DTR",    "CCP0",   "-"          }
#endif
    }
};


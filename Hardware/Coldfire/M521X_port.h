/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M521X_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

#ifdef CHIP_64_PIN                                                       // M521X 64 pin package
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
    { "13 {QSPI_DOUT/CANTX/TXD1/PQS[0]}", "12 {QSPI_DIN/CANRX/RXD1/PQS[1]}", "14 {QSPI_CLK/SCL/RTS1/PQS[2]}", "15 {QSPI_CS0/SDA/CTS1/PQS[3]}", "NA", "NA", "NA", "NA"},
    // AN (0..7)
    { "25 {AN0/PAN[0]}", "26 {AN1/PAN[1]}", "27 {AN2/PAN[2]}", "28 {AN3/PAN[3]}", "36 {AN4/PAN[4]}", "35 {AN5/PAN[5]}", "34 {AN6/PAN[6]}", "33 {AN7/PAN[7]}"},                   
    // LD (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},
    // NQ (0..7)    
    { "NA", "56 {IRQ1/SYNCA/PWM1/PNQ[1]}", "NA", "NA", "57 {IRQ4/PNQ[4]", "NA", "NA", "58 {IRQ7/PNQ[7]"},                  
    // TA (0..3)
    { "40 {GPT0/PWM1/PTA[0]}", "41 {GPT1/PWM3/PTA[1]}", "42 {GPT2/PWM5/PTA[2]}", "43 {GPT3/PWM7/PTA[3]}", "", "", "", ""},                           
    // TC (0..3)
    { "22 {DTIN0/DTOUT0/PWM0/PTC[0]}",  "23 {DTIN1/DTOUT1/PWM2/PTC[1]}",  "18 {DTIN2/DTOUT2/PWM4/PTC[2]}", "19 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},   
    // TD (0..3)    
    { "NA", "NA", "NA", "NA", "", "", "", ""},                          
    // UA (0..3)
    { "6 {TXD0/PUA[0]}", "5 {RXD0/PUA[1]}", "7 {RTS0/CANTX/PUA[2]}", "4 {CTS0/CANRX/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "62 {TXD1/PUB[0]}", "63 {RXD1/PUB[1]}", "2 {RTS1/SYNCB/TXD2/PUB[2]}", "61 {CTS1/SYNCA/RXD2/PUB[3]}", "", "", "", ""},
    // UC (0..3)
    { "NA", "NA", "NA", "NA", "", "", "", ""},                           
    // AS (0..2)
    { "8 {SCL/CANTX/TXD2/PAS[0]}", "9 {SDA/CANRX/RXD2/PAS[1]}", "NA", "NA", "", "", "", ""},
    // GP (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},
    // DD (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},     
};
#else                                                                    // M521X 100 LQFP or 81 MAPBGA
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
    { "17-G1 {QSPI_DOUT/CANTX/TXD1/PQS[0]}", "16-F3 {QSPI_DIN/CANRX/RXD1/PQS[1]}", "18-G2 {QSPI_CLK/SCL/RTS1/PQS[2]}", "20-H1 {QSPI_CS0/SDA/CTS1/PQS[3]}", "19-H2 {QSPI_CS1/PQS[4]}", "13-F2 {QSPI_CS2/PQS[5]}", "12-F1 {QSPI_CS3/SYNCA/SYNCB/PQS[6]}", "NA"},
    // AN (0..7)
    { "43-J6 {AN0/PAN[0]}", "44-H6 {AN1/PAN[1]}", "45-G6 {AN2/PAN[2]}", "46-G7 {AN3/PAN[3]}", "54-F9 {AN4/PAN[4]}", "53-G8 {AN5/PAN[5]}", "52-G9 {AN6/PAN[6]}", "51-H9 {AN7/PAN[7]}"},                   
    // LD (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},
    // NQ (0..7)    
    { "NA", "87-C6 {IRQ1/SYNCA/PWM1/PNQ[1]}", "88-B5 {IRQ2/PNQ[2]}", "89-A5 {IRQ3/PNQ[3]}", "90-C5 {IRQ4/PNQ[4]", "91-A4 {IRQ5/PNQ[5]}", "94-B4 {IRQ6/PNQ[6]}", "95-C4 {IRQ7/PNQ[7]"},                  
    // TA (0..3)
    { "58-F7 {GPT0/PWM1/PTA[0]}", "59-E9 {GPT1/PWM3/PTA[1]}", "61-D9 {GPT2/PWM5/PTA[2]}", "62-D8 {GPT3/PWM7/PTA[3]}", "", "", "", ""},                           
    // TC (0..3)
    { "36-H4 {DTIN0/DTOUT0/PWM0/PTC[0]}",  "37-G4 {DTIN1/DTOUT1/PWM2/PTC[1]}",  "31-J3 {DTIN2/DTOUT2/PWM4/PTC[2]}", "32-H3 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},   
    // TD (0..3)    
    { "38-J5 {PWM1/PTD[0]}", "33-J4 {PWM3/PTD[1]}", "60-E8 {PWM5/PTD[2]}", "63-D7 {PWM7/PTD[3]}", "", "", "", ""},                          
    // UA (0..3)
    { "8-D2 {TXD0/PUA[0]}", "7-D1 {RXD0/PUA[1]}", "9-D3 {RTS0/CANTX/PUA[2]}", "6-C1 {CTS0/CANRX/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "99-A2 {TXD1/PUB[0]}", "100-2 {RXD1/PUB[1]}", "4-B1 {RTS1/SYNCB/TXD2/PUB[2]}", "98-C3 {CTS1/SYNCA/RXD2/PUB[3]}", "", "", "", ""},
    // UC (0..3)
    #ifdef CHIP_100_PIN
    { "29 {TXD2/PUC[0]}", "28 {RXD2/PUC[1]}", "30 {RTS2/PUC[2]}", "27 {CTS2/PUC[3]}", "", "", "", ""},
    #else
    { "NA", "NA", "NA", "NA", "", "", "", ""}, 
    #endif
    // AS (0..2)
    { "10-E1 {SCL/CANTX/TXD2/PAS[0]}", "11-E2 {SDA/CANRX/RXD2/PAS[1]}", "NA", "NA", "", "", "", ""}, 
    // GP (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""}, 
    // DD (0..7)
    #ifdef CHIP_100_PIN    
    { "65 {PST[0]/PDD[0]}", "66 {PST[1]/PDD[1]}", "69 {PST[2]/PDD[2]}", "70 {PST[3]/PDD[3]}", "77 {DDATA[0]/PDD[4]}", "78 {DDATA[1]/PDD[5]}", "83 {DDATA[2]/PDD[6]}", "84 {DDATA[3]/PDD[7]}"},  
    #endif
};
#endif

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "QSPI_DOUT", "CANTX",    "TXD1"       },                 // QS
        {  "Q", "QSPI_DIN",  "CANRX",    "RXD1"       },
        {  "Q", "QSPI_SCK",  "SCL",      "RTS1"       },
        {  "Q", "QSPI_CS0",  "SDA",      "CTS1"       },
#ifdef CHIP_64_PIN
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "NA",        "NA",       "NA"         }
#else
        {  "Q", "QSPI_CS1",  "INVALID",  "INVALID"    },
        {  "Q", "QSPI_CS2",  "INVALID",  "INVALID"    },
        {  "Q", "QSPI_CS3",  "SYNCA",    "SYNCB"      },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    }
#endif
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "AN0",       "INVALID",  "INVALID"    },                 // AN
        {  "Q", "AN1",       "INVALID",  "INVALID"    },
        {  "Q", "AN2",       "INVALID",  "INVALID"    },
        {  "Q", "AN3",       "INVALID",  "INVALID"    },
        {  "Q", "AN4",       "INVALID",  "INVALID"    },
        {  "Q", "AN5",       "INVALID",  "INVALID"    },
        {  "Q", "AN6",       "INVALID",  "INVALID"    },
        {  "Q", "AN7",       "INVALID",  "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },                 // Dummy LD
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "NA","NA",        "NA",       "NA"         },                 // NQ
        {  "Q", "IRQ1",      "SYNCA",    "PWM1"       },
        {  "Q", "IRQ2",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ3",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ4",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ5",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ6",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ7",      "INVALID",  "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "GPT0",     "INVALID",  "PWM1"        },                 // TA
        {  "Q", "GPT1",     "INVALID",   "PWM3"       },
        {  "Q", "GPT2",     "INVALID",   "PWM5"       },
        {  "Q", "GPT3",     "INVALID",   "PWM7"       },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "DTIN0",     "DTOUT0",     "PWM0"     },                 // TC
        {  "Q", "DTIN1",     "DTOUT1",     "PWM2"     },
        {  "Q", "DTIN2",     "DTOUT2",     "PWM4"     },
        {  "Q", "DTIN3",     "DTOUT3",     "PWM6"     },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "PWM1",      "INVALID",  "INVALID"    },                 // TD
        {  "Q", "PWM3",      "INVALID",  "INVALID"    },
        {  "Q", "PWM5",      "INVALID",  "INVALID"    },
        {  "Q", "PWM7",      "INVALID",  "INVALID"    },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "TXD0",      "INVALID",  "INVALID"    },                 // UA
        {  "Q", "RXD0",      "INVALID",  "INVALID"    },
        {  "Q", "RTS0",      "CANTX",    "INVALID"    },
        {  "Q", "CTS0",      "CANRX",    "INVALID"    },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "TXD1",      "INVALID",  "INVALID"    },                 // UB
        {  "Q", "RXD1",      "INVALID",  "INVALID" },
        {  "Q", "RTS1",      "SYNCB",    "TXD2"       },
        {  "Q", "CTS1",      "SYNCA",    "RXD2"       },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "TXD2",      "INVALID",  "INVALID"    },                 // UC
        {  "Q", "RXD2",      "INVALID",  "INVALID"    },
        {  "Q", "RTS2",      "INVALID",  "INVALID"    },
        {  "Q", "CTS2",      "INVALID",  "INVALID"    },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "SCL",       "CANTX",    "TXD2"       },                 // AS
        {  "Q", "SDA",       "CANRX",    "RXD2"       },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },                 // Dummy GP
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "PST[0]",  "INVALID",  "INVALID"    },                   // DD
        {  "Q", "PST[1]",  "INVALID",  "INVALID"    },
        {  "Q", "PST[2]",  "INVALID",  "INVALID"    },
        {  "Q", "PST[3]",  "INVALID",  "INVALID"    },
        {  "Q", "DDATA[0]","INVALID",  "INVALID"    },
        {  "Q", "DDATA[1]","INVALID",  "INVALID"    },
        {  "Q", "DDATA[2]","INVALID",  "INVALID"    },
        {  "Q", "DDATA[3]","INVALID",  "INVALID"    }
    },
};

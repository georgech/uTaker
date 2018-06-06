/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5223X_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    03.03.2009 Change TINx/TOUTx to DTINx/DTOUTx

*/

#ifdef CHIP_80_PIN                                                       // M5223X 80 pin package
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
    { "26 {QSPI_DOUT/CANTX/TXD1/PQS[0]}", "25 {QSPI_DIN/CANRX/RXD1/PQS[1]}", "27 {QSPI_SCK/SCL/RTS1/PQS[2]}", "28 {QSPI_CS0/SDA/CTS1/PQS[3]}", "NA", "NA", "NA", "NA"},
    // AN (0..7)
    { "68 {AN0/PAN[0]}", "67 {AN1/PAN[1]}", "66 {AN2/PAN[2]}", "65 {AN3/PAN[3]}", "61 {AN4/PAN[4]}", "62 {AN5/PAN[5]}", "63 {AN6/PAN[6]}", "64 {AN7/PAN[7]}"},                   
    // LD (0..7)
    { "60 {ACTLED/PLD[0]}", "59 {LNKLED/PLD[1]}", "57 {SPDLED/PLD[2]}", "43 {DUPLED/PLD[3]}", "42 {COLLED/PLD[4]}", "NA", "NA", "NA"},                  
    // NQ (0..7)    
    { "NA", "39 {IRQ1/SYNCA/PWM1/PNQ[1]}", "NA", "NA", "29 {IRQ4/PNQ[4]", "NA", "NA", "40 {IRQ7/PNQ[7]"},                  
    // TA (0..3)
    { "78 {GPT0/FEC_TXER/PWM1/PTA[0]}", "77 {GPT1/FEC_TXD[1]/PWM3/PTA[1]}", "76 {GPT2/FEC_TXD[2]/PWM5/PTA[2]}", "75 {GPT3/FEC_TXD[2]/PWM7/PTA[3]}", "", "", "", ""},                           
    // TC (0..3)
    { "8 {DTIN0/DTOUT0/PWM0/PTC[0]}",  "9 {DTIN1/DTOUT1/PWM2/PTC[1]}",  "13 {DTIN2/DTOUT2/PWM4/PTC[2]}", "14 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},   
    // TD (0..3)    
    { "NA", "NA", "NA", "NA", "", "", "", ""},                          
    // UA (0..3)
    { "22 {TXD0/FEC_CRS/PUA[0]}", "21 {RXD0/FEC_RXD/PUA[1]}", "17 {RTS0/CANTX/FEC_RXDV/PUA[2]}", "18 {CTS0/CANRX/FEC_RXCLK/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "24 {TXD1/FEC_COL/PUB[0]}", "23 {RXD1/FEC_TXD[0]/PUB[1]}", "15 {RTS1/SYNCB/TXD2/PUB[2]}", "16 {CTS1/SYNCA/RXD2/PUB[3]}", "", "", "", ""},
    // UC (0..3)
    { "NA", "NA", "NA", "NA", "", "", "", ""},                           
    // AS (0..3)
    { "79 {SCL/CANTX/TXD2/PAS[0]}", "80 {SDA/CANRX/RXD2/PAS[1]}", "19 {SYNCB/CANRX/FEC_MDC/PAS[2]}", "20 {SYNCA/CANTX/FEC_MDIO/PAS[3]", "", "", "", ""},                          
    // GP (0..7)    
    { "NA", "NA", "NA", "41 {IRQ11/PGP[3]}", "NA", "NA", "NA", "NA"},                  
};
#else                                                                    // M5223X 112 LQFP or 121 MAPBGA
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
    { "35-J4 {QSPI_DOUT/CANTX/TXD1/PQS[0]}", "34-H4 {QSPI_DIN/CANRX/RXD1/PQS[1]}", "36-K4 {QSPI_SCK/SCL/RTS1/PQS[2]}", "37-L4 {QSPI_CS0/SDA/CTS1/PQS[3]}", "38-H5 {QSPI_CS1/FEC_TXEN/PQS[4]}", "39-J5 {QSPI_CS2/FEC_TXCLK/PQS[5]}", "40-K5 {QSPI_CS3/SYNCA/SYNCB/PQS[6]}", "NA"},
    // AN (0..7)
    { "92-C8 {AN0/PAN[0]}", "91-A9 {AN1/PAN[1]}", "90-B9 {AN2/PAN[2]}", "89-C9 {AN3/PAN[3]}", "85-B11 {AN4/PAN[4]}", "86-A11 {AN5/PAN[5]}", "87-B10 {AN6/PAN[6]}", "88-A10 {AN7/PAN[7]}"},                   
    // LD (0..7)
    { "84-C11 {ACTLED/PLD[0]}", "83-C10 {LNKLED/PLD[1]}", "81-D11 {SPDLED/PLD[2]}", "59-J10 {DUPLED/PLD[3]}", "58-J9 {COLLED/PLD[4]}", "52-H9 {RXLED/PLD[5]}", "51-H8 {TXLED/PLD[6]}", "NA"},                  
    // NQ (0..7)    
    { "NA", "55-J8 {IRQ1/SYNCA/PWM1/PNQ[1]}", "54-K8 {IRQ2/FEC_RXD[3]/PNQ[2]}", "53-L8 {IRQ3/FEC_RXD[2]/PNQ[3]}", "41-L5 {IRQ4/PNQ[4]", "20-G2 {IRQ5/FEC_RXD[1]/PNQ[5]}", "19-G3 {IRQ6/FEC_ERR/PNQ[6]}", "56-L9 {IRQ7/PNQ[7]"},                  
    // TA (0..3)
    { "110-B3 {GPT0/FEC_TXER/PWM1/PTA[0]}", "109-D4 {GPT1/FEC_TXD[1]/PWM3/PTA[1]}", "108-C4 {GPT2/FEC_TXD[2]/PWM5/PTA[2]}", "107-B4 {GPT3/FEC_TXD[2]/PWM7/PTA[3]}", "", "", "", ""},                           
    // TC (0..3)
    { "8-D2 {DTIN0/DTOUT0/PWM0/PTC[0]}",  "9-D1 {DTIN1/DTOUT1/PWM2/PTC[1]}",  "21-G1 {DTIN2/DTOUT2/PWM4/PTC[2]}", "22-H1 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},   
    // TD (0..3)    
    { "99-B6 {PWM1/PTD[0]}", "100-C6 {PWM3/PTD[1]}", "103-B5 {PWM5/PTD[2]}", "104-C5 {PWM7/PTD[3]}", "", "", "", ""},                          
    // UA (0..3)
    { "31-L2 {TXD0/FEC_CRS/PUA[0]}", "30-K2 {RXD0/FEC_RXD/PUA[1]}", "25-H2 {RTS0/CANTX/FEC_RXDV/PUA[2]}", "26-J2 {CTS0/CANRX/FEC_RXCLK/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "33-L3 {TXD1/FEC_COL/PUB[0]}", "32-K3 {RXD1/FEC_TXD[0]/PUB[1]}", "23-H3 {RTS1/SYNCB/TXD2/PUB[2]}", "24-J3 {CTS1/SYNCA/RXD2/PUB[3]}", "", "", "", ""},
    // UC (0..3)
    { "63-L11 {TXD2/PUC[0]}", "62-K11 {RXD2/PUC[1]}", "60-K10 {RTS2/PUC[2]}", "61-L10 {CTS2/PUC[3]}", "", "", "", ""},
    // AS (0..3)
    { "111-A3 {SCL/CANTX/TXD2/PAS[0]}", "112-A2 {SDA/CANRX/RXD2/PAS[1]}", "27-NA {SYNCB/CANRX/FEC_MDC/PAS[2]}", "28-NA {SYNCA/CANTX/FEC_MDIO/PAS[3]", "", "", "", ""},                          
    // GP (0..7)    
    { "10-E3 {IRQ8/PGP[0]}", "11-E2 {IRQ9/PGP[1]}", "29-L1 {IRQ10/PGP[2]}", "57-K9 {IRQ11/PGP[3]}", "97-C7 {IRQ12/PGP[4]}", "98-A6 {IRQ13/PGP[5]}", "105-A5 {IRQ14/PGP[6]}", "106-A6 {IRQ15/PGP[7]}"},                  
    // DD (0..7)    
    { "77-E9 {PST[0]/PDD[0]}", "78-E10 {PST[1]/PDD[1]}", "79-D9 {PST[2]/PDD[2]}", "80-D10 {PST[3]/PDD[3]}", "17-F1 {DDATA[0]/PDD[4]}", "16-F2 {DDATA[1]/PDD[5]}", "13-F3 {DDATA[2]/PDD[6]}", "12-E1 {DDATA[3]/PDD[7]}"},  
};
#endif

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "QSPI_DOUT", "CANTX",    "TXD1"       },                 // QS
        {  "Q", "QSPI_DIN",  "CANRX",    "RXD1"       },
        {  "Q", "QSPI_SCK",  "SCL",      "RTS1"       },
        {  "Q", "QSPI_CS0",  "SDA",      "CTS1"       },
        {  "Q", "QSPI_CS1",  "INVALID",  "FEC_TXEN"   },
        {  "Q", "QSPI_CS2",  "INVALID",  "FEC_TXCLK"  },
        {  "Q", "QSPI_CS3",  "SYNCA",    "SYNCB"      },
        {  "Q", "NA",        "NA",       "NA"         }
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
        {  "Q", "ACTLED",    "INVALID",  "INVALID"    },                 // LD
        {  "Q", "LNKLED",    "INVALID",  "INVALID"    },
        {  "Q", "SPDLED",    "INVALID",  "INVALID"    },
        {  "Q", "DUPLED",    "INVALID",  "INVALID"    },
        {  "Q", "COLLED",    "INVALID",  "INVALID"    },
        {  "Q", "RXLED",     "INVALID",  "INVALID"    },
        {  "Q", "TXLED",     "INVALID",  "INVALID"    },
        {  "Q", "NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "NA","NA",        "NA",       "NA"         },                 // NQ
        {  "Q", "IRQ1",      "SYNCA",    "PWM1"       },
        {  "Q", "IRQ2",      "INVALID",  "FEC_RXD[3]" },
        {  "Q", "IRQ3",      "INVALID",  "FEC_RXD[2]" },
        {  "Q", "IRQ4",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ5",      "INVALID",  "FEC_RXD[1]" },
        {  "Q", "IRQ6",      "INVALID",  "FEC_RXER"   },
        {  "Q", "IRQ7",      "INVALID",  "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "GPT0",     "FEC_TXER",  "PWM1"       },                 // TA
        {  "Q", "GPT1",     "FEC_TXD[1]","PWM3"       },
        {  "Q", "GPT2",     "FEC_TXD[2]","PWM5"       },
        {  "Q", "GPT3",     "FEC_TXD[3]","PWM7"       },
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
        {  "Q", "TXD0",      "INVALID",  "FEC_CRS"    },                 // UA
        {  "Q", "RXD0",      "INVALID",  "FEC_RXD[0]" },
        {  "Q", "RTS0",      "CANTX",    "FEC_RXDV"   },
        {  "Q", "CTS0",      "CANRX",    "FEC_RXCLK"  },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "TXD1",      "INVALID",  "FEC_COL"    },                 // UB
        {  "Q", "RXD1",      "INVALID",  "FEC_TXD[0]" },
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
        {  "Q", "SYNCB",     "CANTX",    "FEC_MDC"    },
        {  "Q", "SYNCA",     "CANRX",    "FEC_MDIO"   },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "IRQ8",    "INVALID",  "INVALID"    },                   // GP
        {  "Q", "IRQ9",    "INVALID",  "INVALID"    },
        {  "Q", "IRQ10",   "INVALID",  "INVALID"    },
        {  "Q", "IRQ11",   "INVALID",  "INVALID"    },
        {  "Q", "IRQ12",   "INVALID",  "INVALID"    },
        {  "Q", "IRQ13",   "INVALID",  "INVALID"    },
        {  "Q", "IRQ14",   "INVALID",  "INVALID"    },
        {  "Q", "IRQ15",   "INVALID",  "INVALID"    }
    },
     
#ifndef CHIP_80_PIN        
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
#endif
};

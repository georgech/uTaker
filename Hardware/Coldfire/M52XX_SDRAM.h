/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M52XX_SDRAM.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // A (0..7)
    { "H3 {D24/PA[0]}", "H2 {D25/PA[1]}", "H1 {D26/PA[2]}", "G4 {D27/PA[3]}", "G3 {D28/PA[4]}", "G2 {D29/PA[5]}", "G1 {D30/PA[6]}", "F3 {D31/PA[7]}"},
    // B (0..7)
    { "K3 {D16/PB[0]}", "K2 {D17/PB[1]}", "K1 {D18/PB[2]}", "J4 {D19/PB[3]}", "J3 {D20/PB[4]}", "J2 {D21/PB[5]}", "J1 {D22/PB[6]}", "H4 {D23/PB[7]}"},
    // C (0..7)
    { "M4 {D8/PC[0]}", "M3 {D9/PC[1]}", "M2 {D10/PC[2]}", "M1 {D11/PC[3]}", "L4 {D12/PC[4]}", "L3 {D13/PC[5]}", "L2 {D14/PC[6]}", "L1 {D15/PC[7]}"},
    // D (0..7)
    { "P6 {D0/PD[0]}", "R6 {D1/PD[1]}", "T6 {D2/PD[2]}", "N5 {D3/PD[3]}", "P1 {D4/PD[4]}", "N3 {D5/PD[5]}", "N2 {D6/PD[6]}", "N1 {D7/PD[7]}"},
    // E (0..7)
    { "M14 {TIP/PE[0]/SYNCB}", "M15 {TS/PE[1]/SYNCA}", "M16 {SIZ0/PE[2]/SYNCB}", "N14 {SIZ1/PE[3]/SYNCA}", "N15 {R/W/PE[4]}", "P15 {TEA/PE[5]}", "P16 {TA/PE[6]}", "N16 {OE/PE[7]}"},
    // F (0..7)
    { "A3 {A16/PF[0]}", "B3 {A17/PF[1]}", "A4 {A18/PF[2]}", "B4 {A19/PF[3]}", "C4 {A20/PF[4]}", "A5 {A21/PF[5]}", "B6 {A22/PF[6]}", "C6 {A23/PF[7]/CS6}"},
    // G (0..7)
    { "D2 {A8/PG[0]}", "D2 {A9/PG[1]}", "C3 {A10/PG[2]}", "C2 {A11/PG[3]}", "C1 {A12/PG[4]}", "B2 {A13/PG[5]}", "B1 {A14/PG[6]}", "A2 {A15/PG[7]}"},
    // H (0..7)
    { "F2 {A0/PH[0]}", "F1 {A1/PH[1]}", "E4 {A2/PH[2]}", "E3 {A3/PH[3]}", "E2 {A4/PH[4]}", "E1 {A5/PH[5]}", "D4 {A6/PH[6]}", "D3 {A7/PH[7]}"},
    // J (0..7)
    { "L13 {CS0/PJ[0]}", "L14 {CS1/PJ[1]}", "L15 {CS2/PJ[2]}", "L16 {CS3/PJ[3]}", "R16 {BS0/PJ[4]}", "R15 {BS1/PJ[5]}", "T15 {BS2/PJ[6]}", "P14 {BS3/PJ[7]}"},
    // DD (0..7)
    { "A15 {PST0/PDD[0]}", "B14 {PST1/PDD[1]}", "A14 {PST2/PDD[2]}", "C13 {PST3/PDD[3]}", "B13 {DDATA0/PDD[4]}", "A13 {DDATA1/PDD[5]}", "D12 {DDATA2/PDD[6]}", "C12 {DDATA3/PDD[7]}"},
#ifdef _M521X_SDRAM
    // EH (0..7)
    { "A11 {NC}", "D9 {NC}", "C8 {NC}", "A10 {PAS4/UTXD2}", "B11 {NC}", "D7 {NC}", "D6 {NC}", "NA"},
    // EL (0..7)
    { "A8 {PEL[0]}", "C9 {PEL[1]}", "B9 {PEL[2]}", "A9 {PEL[3]}", "D10 {PEL[4]}", "C7 {PEL[5]}", "B7 {PEL[6]}", "A7 {PEL[7]}"},
    // AS (0..5)
    { "E15 {SCL/PAS[0]/UTXD2}", "E14 {SDA/PAS[1]/URXD2}", "E13 {CANTX/PAS[2]/UTXD2}", "D16 {CANRX/PAS[3]/URXD2}", "B10 {NA}", "C10 {PAS[5]/URXD2}", "NA", "NA"},
#else
    { "A11 {ECRS/PEH[0]}", "D9 {ERXD0/PEH[1]}", "C8 {ERXDV/PEH[2]}", "A10 {ERXCLK/PEH[3]/UTXD2}", "B11 {ECOL/PEH[4]}", "D7 {ETXD0/PEH[5]}", "D6 {ETXEN/PEH[6]}", "A8 {ETXCLK/PEH[7]}"},
    // EL (0..7)
    { "B8 {ERXER/PEL[0]}", "C9 {ERXD1/PEL[1]}", "B9 {ERXD2/PEL[2]}", "A9 {ERXD3/PEL[3]}", "D10 {ETXER/PEL[4]}", "C7 {ETXD1/PEL[5]}", "B7 {ETXD2/PEL[6]}", "A7 {ETXCLK/PEL[7]}"},
    // AS (0..5)
    { "E15 {SCL/PAS[0]/UTXD2}", "E14 {SDA/PAS[1]/URXD2}", "E13 {CANTX/PAS[2]/UTXD2}", "D16 {CANRX/PAS[3]/URXD2}", "B10 {EMDC/PAS[4]/UTXD2}", "C10 {EMDIO/PAS[5]/URXD2}", "NA", "NA"},
#endif
    // NQ (1..7)
    { "NA", "D15 {IRQ1/PNQ[1]}", "D14 {IRQ2/PNQ[2]}", "C16 {IRQ3/PNQ[3]}", "C15 {IRQ4/PNQ[4]}", "C14 {IRQ5/PNQ[5]}", "B16 {IRQ6/PNQ[6]}", "B15 {IRQ7/PNQ[7]}"},
    // QA (0,1,3,4)
    { "R4 {AN52/PQA[0]/MA0}", "T4 {AN53/PQA[1]/MA1}", "NA", "P3 {AN55/PQA[3]/ETRIG1}", "R3 {AN56/PQA[4]/ETRIG2}", "NA", "NA", "NA"},
    // QB (0..3)
    { "T3 {AN0/PQB[0]/ANW}", "R2 {AN1/PQB[1]/ANX}", "T2 {AN2/PQB[2]/ANY}", "R1 {AN3/PQB[3]/ANZ}", "NA", "NA", "NA", "NA"},
    // QS (0..6)
    { "F13 {QSPI_DOUT/PQS[0]}", "E16 {QSPI_DIN/PQS[1]}", "F14 {QSPI_CLK/PQS[2]}", "F15 {QSPI_CS0/PQS[3]}", "F16 {QSPI_CS1/PQS[4]}", "G13 {QSPI_CS2/PQS[5]}", "G14 {QSPI_CS3/PQS[6]}", "NA"},
    // SD (0..5)
    { "H14 {SCKE/PSD[0]}", "G16 {SDRAM_CS0/PSD[1]}", "H13 {SDRAM_CS1/PSD[2]}", "G15 {DRAMW/PSD[3]}", "H16 {SCAS/PSD[4]}", "H15 {SRAS/PSD[5]}", "NA", "NA"},
    // TA (0..3)
    { "N13 {GPTA0/PTA[0]}", "P13 {GPTA1/PTA[1]}", "R13 {GPTA2/PTA[2]}", "T13 {GPTA3/PTA[3]}", "NA", "NA", "NA", "NA"},
    // TB (0..3)
    { "N12 {GPTB0/PTB[0]}", "P12 {GPTB1/PTB[1]}", "R12 {GPTB2/PTB[2]}", "T12 {GPTB3/PTB[3]}", "NA", "NA", "NA", "NA"},
    // TC (0..3)
    { "K13 {DTOUT2/PTC[0]/UCTS1/UCTS0}", "K14 {DTIN2/PTC[1]/UCTS1/UCTS0}", "K15 {DTOUT3/PTC[2]/URTS1/URTS0}", "K16 {DTIN3/PTC[3]/URTS1/URTS0}", "NA", "NA", "NA", "NA"},
    // TD (0..3)
    { "J13 {DTOUT0/PTD[0]/UCTS1/UCTS0}", "J14 {DTIN0/PTD[1]/UCTS1/UCTS0}", "J15 {DTOUT1/PTD[2]/URTS1/URTS0}", "J16 {DTIN1/PTD[3]/URTS1/URTS0}", "NA", "NA", "NA", "NA"},
    // UA (0..3)
    { "T7 {UTXD0/PUA[0]}", "N6 {URXD0/PUA[1]}", "P7 {UTXD1/PUA[2]}", "R7 {URXD1/PUA[3]}", "NA", "NA", "NA", "NA"},
};


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q   Prim.   Sec.     Tertiary functions
        {  "Q", "D24",  "PA[0]", "INVALID"      },                       // A
        {  "Q", "D25",  "PA[1]", "INVALID"      },
        {  "Q", "D26",  "PA[2]", "INVALID"      },
        {  "Q", "D27",  "PA[3]", "INVALID"      },
        {  "Q", "D28",  "PA[4]", "INVALID"      },
        {  "Q", "D29",  "PA[5]", "INVALID"      },
        {  "Q", "D30",  "PA[6]", "INVALID"      },
        {  "Q", "D31",  "PA[7]", "INVALID"      }
    },
    {
        //  Q   Prim.   Sec.     Tertiary functions
        {  "Q", "D16",  "PB[0]", "INVALID"      },                       // B
        {  "Q", "D17",  "PB[1]", "INVALID"      },
        {  "Q", "D18",  "PB[2]", "INVALID"      },
        {  "Q", "D19",  "PB[3]", "INVALID"      },
        {  "Q", "D20",  "PB[4]", "INVALID"      },
        {  "Q", "D21",  "PB[5]", "INVALID"      },
        {  "Q", "D22",  "PB[6]", "INVALID"      },
        {  "Q", "D23",  "PB[7]", "INVALID"      }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "D8",  "PC[0]", "INVALID"      },                        // C
        {  "Q", "D9",  "PC[1]", "INVALID"      },
        {  "Q", "D10", "PC[2]", "INVALID"      },
        {  "Q", "D11", "PC[3]", "INVALID"      },
        {  "Q", "D12", "PC[4]", "INVALID"      },
        {  "Q", "D13", "PC[5]", "INVALID"      },
        {  "Q", "D14", "PC[6]", "INVALID"      },
        {  "Q", "D15", "PC[7]", "INVALID"      }
    },        
    {
        //  Q   Prim. Sec.     Tertiary functions
        {  "Q", "D0", "PD[0]", "INVALID"      },                         // D
        {  "Q", "D1", "PD[1]", "INVALID"      },
        {  "Q", "D2", "PD[2]", "INVALID"      },
        {  "Q", "D3", "PD[3]", "INVALID"      },
        {  "Q", "D4", "PD[4]", "INVALID"      },
        {  "Q", "D5", "PD[5]", "INVALID"      },
        {  "Q", "D6", "PD[6]", "INVALID"      },
        {  "Q", "D7", "PD[7]", "INVALID"      }
    }, 
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "SYNCB","PE[0]","TIP"         },                         // E
        {  "Q", "SYNCA","PE[1]","TS"          },
        {  "Q", "SIZ0","PE[2]", "SYNCB"       },
        {  "Q", "SIZ1","PE[3]", "SYNCA"       },
        {  "Q", "R/W", "PE[4]", "INVALID"     },
        {  "Q", "TEA", "PE[5]", "INVALID"     },
        {  "Q", "TA",  "PE[6]", "INVALID"     },
        {  "Q", "OE",  "PE[7]", "INVALID"     }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "A16", "PF[0]", "INVALID"      },                        // F
        {  "Q", "A17", "PF[1]", "INVALID"      },
        {  "Q", "A18", "PF[2]", "INVALID"      },
        {  "Q", "A19", "PF[3]", "INVALID"      },
        {  "Q", "A20", "PF[4]", "INVALID"      },
        {  "Q", "A21", "PF[5]", "INVALID"      },
        {  "Q", "A22", "PF[6]", "INVALID"      },
        {  "Q", "A23", "PF[7]", "INVALID"      }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "A8",  "PG[0]", "INVALID"      },                        // G
        {  "Q", "A9",  "PG[1]", "INVALID"      },
        {  "Q", "A10", "PG[2]", "INVALID"      },
        {  "Q", "A11", "PG[3]", "INVALID"      },
        {  "Q", "A12", "PG[4]", "INVALID"      },
        {  "Q", "A13", "PG[5]", "INVALID"      },
        {  "Q", "A14", "PG[6]", "INVALID"      },
        {  "Q", "A15", "PG[7]", "INVALID"      }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "A0",  "PH[0]", "INVALID"      },                        // H
        {  "Q", "A1",  "PH[1]", "INVALID"      },
        {  "Q", "A2",  "PH[2]", "INVALID"      },
        {  "Q", "A3",  "PH[3]", "INVALID"      },
        {  "Q", "A4",  "PH[4]", "INVALID"      },
        {  "Q", "A5",  "PH[5]", "INVALID"      },
        {  "Q", "A6",  "PH[6]", "INVALID"      },
        {  "Q", "A7",  "PH[7]", "INVALID"      }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "CS0", "PJ[0]", "INVALID"      },                        // J
        {  "Q", "CS1", "PJ[1]", "INVALID"      },
        {  "Q", "CS2", "PJ[2]", "INVALID"      },
        {  "Q", "CS3", "PJ[3]", "INVALID"      },
        {  "Q", "BS0", "PJ[4]", "INVALID"      },
        {  "Q", "BS1", "PJ[5]", "INVALID"      },
        {  "Q", "BS2", "PJ[6]", "INVALID"      },
        {  "Q", "BS3", "PJ[7]", "INVALID"      }
    }, 
    {
        //  Q   Prim.    Sec.      Tertiary functions
        {  "Q", "PST0",  "PDD[0]", "INVALID"      },                     // DD
        {  "Q", "PST1",  "PDD[1]", "INVALID"      },
        {  "Q", "PST2",  "PDD[2]", "INVALID"      },
        {  "Q", "PST3",  "PDD[3]", "INVALID"      },
        {  "Q", "DDATA0","PDD[4]", "INVALID"      },
        {  "Q", "DDATA1","PDD[5]", "INVALID"      },
        {  "Q", "DDATA2","PDD[6]", "INVALID"      },
        {  "Q", "DDATA3","PDD[7]", "INVALID"      }
    }, 
#ifdef _M521X_SDRAM
    {
        //  Q   Prim.      Sec.       Tertiary functions
        {  "Q", "INVALID", "INVALID", "INVALID"   },                     // EH
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "PAS[4]",  "UTXD2",   "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
    }, 
    {
        //  Q   Prim.      Sec.      Tertiary functions
        {  "Q", "INVALID", "PEL[0]", "INVALID"   },                     // EL
        {  "Q", "INVALID", "PEL[1]", "INVALID"   },
        {  "Q", "INVALID", "PEL[2]", "INVALID"   },
        {  "Q", "INVALID", "PEL[3]", "INVALID"   },
        {  "Q", "INVALID", "PEL[4]", "INVALID"   },
        {  "Q", "INVALID", "PEL[5]", "INVALID"   },
        {  "Q", "INVALID", "PEL[6]", "INVALID"   },
        {  "Q", "INVALID", "PEL[7]", "INVALID"   },
    },
    {
        //  Q   Prim.     Sec.      Tertiary functions
        {  "Q", "PAS[0]", "UTXD2",  "SCL"       },                       // AS
        {  "Q", "PAS[1]", "URXD2",  "SDA"       },
        {  "Q", "PAS[2]", "UTXD2",  "CANTX"     },
        {  "Q", "PAS[3]", "URXD2",  "CANRX"     },
        {  "Q", "PAS[4]", "UTXD2",  "INVALID"   },
        {  "Q", "PAS[5]", "URXD2",  "INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
    },
#else
    {
        //  Q   Prim.    Sec.      Tertiary functions
        {  "Q", "ECRS",  "PEH[0]", "INVALID"   },                        // EH
        {  "Q", "ERXD0", "PEH[1]", "INVALID"   },
        {  "Q", "ERXDV", "PEH[2]", "INVALID"   },
        {  "Q", "ERXCLK","PEH[3]", "INVALID"   },
        {  "Q", "ECOL",  "PEH[4]", "INVALID"   },
        {  "Q", "ETXD0", "PEH[5]", "INVALID"   },
        {  "Q", "ETXEN", "PEH[6]", "INVALID"   },
        {  "Q", "ETXCLK","PEH[7]", "INVALID"   },
    }, 
    {
        //  Q   Prim.    Sec.      Tertiary functions
        {  "Q", "ERXER", "PEL[0]", "INVALID"   },                        // EL
        {  "Q", "ERXD1", "PEL[1]", "INVALID"   },
        {  "Q", "ERXD2", "PEL[2]", "INVALID"   },
        {  "Q", "ERXD3", "PEL[3]", "INVALID"   },
        {  "Q", "ETXER", "PEL[4]", "INVALID"   },
        {  "Q", "ETXD1", "PEL[5]", "INVALID"   },
        {  "Q", "ETXD2", "PEL[6]", "INVALID"   },
        {  "Q", "ETXCLK","PEL[7]", "INVALID"   },
    },
    {
        //  Q   Prim.     Sec.      Tertiary functions
        {  "Q", "PAS[0]", "UTXD2",  "SCL"       },                       // AS
        {  "Q", "PAS[1]", "URXD2",  "SDA"       },
        {  "Q", "PAS[2]", "UTXD2",  "CANTX"     },
        {  "Q", "PAS[3]", "URXD2",  "CANRX"     },
        {  "Q", "PAS[4]", "UTXD2",  "EMDC"      },
        {  "Q", "PAS[5]", "URXD2",  "EMDIO"     },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
    },
#endif
    {
        //  Q   Prim.     Sec.      Tertiary functions
        {  "Q", "INVALID","INVALID","INVALID"   },                       // NQ
        {  "Q", "IRQ1",   "PNQ[1]", "INVALID"   },
        {  "Q", "IRQ2",   "PNQ[2]", "INVALID"   },
        {  "Q", "IRQ3",   "PNQ[3]", "INVALID"   },
        {  "Q", "IRQ4",   "PNQ[4]", "INVALID"   },
        {  "Q", "IRQ5",   "PNQ[5]", "INVALID"   },
        {  "Q", "IRQ6",   "PNQ[6]", "INVALID"   },
        {  "Q", "IRQ7",   "PNQ[7]", "INVALID"   }
    }, 
    {
        //  Q   Prim.     Sec.      Tertiary functions
        {  "Q", "AN52",   "PQA[0]", "MA0"       },                       // QA
        {  "Q", "AN53",   "PQA[1]", "MA1"       },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "AN55",   "PQA[3]", "ETRIG1"    },
        {  "Q", "AN56",   "PQA[4]", "ETRIG2"    },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   }
    },
    {
        //  Q   Prim.     Sec.      Tertiary functions
        {  "Q", "AN0",    "PQB[0]", "ANW"       },                       // QB
        {  "Q", "AN1",    "PQB[1]", "ANX"       },
        {  "Q", "AN2",    "PQB[2]", "ANY"       },
        {  "Q", "AN3",    "PQB[3]", "ANZ"       },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   }
    },
    {
        //  Q   Prim.       Sec.      Tertiary functions
        {  "Q", "QSPI_DOUT","PQS[0]", "INVALID" },                       // QS
        {  "Q", "QSPI_DIN", "PQS[1]", "INVALID" },
        {  "Q", "QSPI_CLK", "PQS[2]", "INVALID" },
        {  "Q", "QSPI_CS0", "PQS[3]", "INVALID" },
        {  "Q", "QSPI_CS1", "PQS[4]", "INVALID" },
        {  "Q", "QSPI_CS2", "PQS[5]", "INVALID" },
        {  "Q", "QSPI_CS3", "PQS[6]", "INVALID" },
        {  "Q", "INVALID",  "INVALID","INVALID" }
    },
    {
        //  Q   Prim.       Sec.      Tertiary functions
        {  "Q", "SCKE",     "PSD[0]", "INVALID" },                       // SD
        {  "Q", "SDRAM_CS0","PSD[1]", "INVALID" },
        {  "Q", "SDRAM_CS1","PSD[2]", "INVALID" },
        {  "Q", "DRAMW",    "PSD[3]", "INVALID" },
        {  "Q", "SCAS",     "PSD[4]", "INVALID" },
        {  "Q", "SRAS",     "PSD[5]", "INVALID" },
        {  "Q", "INVALID",  "INVALID","INVALID" },
        {  "Q", "INVALID",  "INVALID","INVALID" }
    },
    {
        //  Q   Prim.      Sec.       Tertiary functions
        {  "Q", "GPTA0",   "PTA[0]",  "INVALID" },                       // TA
        {  "Q", "GPTA1",   "PTA[1]",  "INVALID" },
        {  "Q", "GPTA2",   "PTA[2]",  "INVALID" },
        {  "Q", "GPTA3",   "PTA[3]",  "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" }
    },
    {
        //  Q   Prim.      Sec.       Tertiary functions
        {  "Q", "GPTB0",   "PTB[0]",  "INVALID" },                       // TB
        {  "Q", "GPTB1",   "PTB[1]",  "INVALID" },
        {  "Q", "GPTB2",   "PTB[2]",  "INVALID" },
        {  "Q", "GPTB3",   "PTB[3]",  "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" }
    },
    {
        //  Q   Prim.      Sec.       Tertiary functions
        {  "Q", "DTOUT2",  "PTC[0]",  "UCTS1/UCTS0" },                   // TC
        {  "Q", "DTIN2",   "PTC[1]",  "UCTS1/UCTS0" },
        {  "Q", "DTOUT3",  "PTC[2]",  "URTS1/URTS0" },
        {  "Q", "DTIN3",   "PTC[3]",  "URTS1/URTS0" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" }
    },
    {
        //  Q   Prim.      Sec.       Tertiary functions
        {  "Q", "DTOUT0",  "PTD[0]",  "UCTS1/UCTS0" },                   // TD
        {  "Q", "DTIN0",   "PTD[1]",  "UCTS1/UCTS0" },
        {  "Q", "DTOUT1",  "PTD[2]",  "URTS1/URTS0" },
        {  "Q", "DTIN1",   "PTD[3]",  "URTS1/URTS0" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" }
    },
    {
        //  Q   Prim.      Sec.       Tertiary functions
        {  "Q", "UTXD0",   "PUA[0]",  "INVALID" },                       // UA
        {  "Q", "URXD0",   "PUA[1]",  "INVALID" },
        {  "Q", "UTXD1",   "PUA[2]",  "INVALID" },
        {  "Q", "URXD1",   "PUA[3]",  "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" },
        {  "Q", "INVALID", "INVALID", "INVALID" }
    }
};

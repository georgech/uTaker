/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5225X_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    16.10.2009 Correct TG6 and TG7, TH6 and TH7
    14.04.2010 Reverse previous correction
    14.02.2012 Correct pin numbering of AS2 on M5225x 100 pin package    {1}
    25.02.2012 Correct pin numbering of PTF[4], PTF[6] and PTF[7] on M5225x BGA package {2}
    25.02.2012 Correct pin numbering of PUA[3] on M5225x 144 pin package {3}
    25.02.2012 Correct pin numbering of PTE[0..7] on M5225x BGA package  {4}
    25.02.2012 Correct pin numbering of PTG[0..3] on M5225x BGA package  {5}
    09.09.2013 Correct AN0..AN7 pin numbers for 100 pin package          {6}

*/
#if !defined CHIP_144_PIN                                                // M5225X 100 pin package
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
    { "19 {QSPI_DOUT/I2C_SCL1/UTXD1/PQS[0]}", "18 {QSPI_DIN/I2C_SDA1/URXD1/PQS[1]}", "20 {QSPI_SCK/I2C_SCL0/URTS1/PQS[2]}", "21 {QSPI_CS0/I2C_SDA0/UCTS1/PQS[3]}", "NA", "17 {QSPI_CS2/SYNCB/USB_DM_PDOWN/PQS[5]}", "16 {QSPI_CS3/SYNCA/USB_PD_PDOWN/PQS[6]}", "NA"},
    // AN (0..7)
    { "43 {AN0/PAN[0]}", "44 {AN1/PAN[1]}", "45 {AN2/PAN[2]}", "46 {AN3/PAN[3]}", "54 {AN4/PAN[4]}", "53 {AN5/PAN[5]}", "52 {AN6/PAN[6]}", "51 {AN7/PAN[7]}"}, // {6}
    // NQ (0..7)
    { "NA", "70 {IRQ1/USB_ALTCLK/PNQ[1]}", "NA", "96 {IRQ3/FEC_MDIO/PNQ[3]}", "NA", "95 {IRQ5/FEC_MDC/PNQ[5]}", "NA", "63 {IRQ7/PNQ[7]}"}, 
    // TA (0..3)    
    { "60 {GPT0/PWM1/PTA[0]}", "61 {GPT1/PWM3/PTA[1]}", "62 {GPT2/PWM5/PTA[2]}", "35 {GPT3/PWM7/PTA[3]}", "", "", "", ""}, 
    // TC (0..3)
    { "5 {DTIN0/DTOUT0/PWM0/PTC[0]}", "6 {DTIN1/DTOUT1/PWM2/PTC[1]}", "42 {DTIN2/DTOUT2/PWM4/PTC[2]}", "13 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},  
    // UA (0..3)
    { "10 {UTXD0/PUA[0]}",  "11 {URXD0/PUA[1]}",  "12 {URTS0/USB_VBUSD/PUA[2]}", "9 {UCTS0/USB_VBUSE/PUA[3]}", "", "", "", ""},   
    // UB (0..3)    
    { "39 {UTXD1/I2C_SCL1/PUB[0]}", "40 {URXD1/I2C_SDA1/PUB[1]}", "41 {URTS1/SYNCB/UTXD2/PUB[2]}", "38 {UCTS1/SYNCA/PUB[3]}", "", "", "", ""},
    // UC (0..3)
    { "68 {UTXD2/CANTX/PUC[0]}", "69 {URXD2/CANRX/PUC[1]}", "65 {URTS2/I2C_SDA1/USB_VBUSDIS/PUC[2]}", "64 {UCTS2/I2C_SCL1/USB_VBUSCHG/PUC[3]}", "", "", "", ""}, 
    // AS (0..2)
    { "22 {I2C_SCL0/UTXD2/PAS[0]}", "23 {I2C_SDA0/URXD2/PAS[1]}", "NA", "NA", "", "", "", ""}, // {1}
    // DD (0..7)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"}, 
    // TE (0..7)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},                          
    // TF (0..7)    
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"}, 
    // TG (0..7)    
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"}, 
    // TH (0..7)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"}, 
    // TI (0..7)    
    { "76 {FEC_COL/PTI[0]}", "75 {FEC_CRS/PTI[1]}", "87 {FEC_RXCLK/PTI[2]}", "89 {FEC_RXD[0]/PTI[3]}", "90 {FEC_RXD[1]/PTI[4]}", "93 {FEC_RXD[2]/PTI[5]}", "94 {FEC_RXD[3]/PTI[6]}", "88 {FEC_RXDV/PTI[7]}"},                     
    // TJ (0..7)    
    { "86 {FEC_RXER/PTJ[0]}", "84 {FEC_TXCLK/PTJ[1]}", "80 {FEC_TXD[0]/PTJ[2]}", "79 {FEC_TXD[1]/PTJ[3]}", "78 {FEC_TXD[2]/PTJ[4]}", "77 {FEC_TXD[3]/PTJ[5]}", "83 {FEC_TXEN/PTJ[6]}", "85 {FEC_TXER/PTJ[7]}"},  
};
#else                                                                    // 144 pin package
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
    { "25-G1 {QSPI_DOUT/I2C_SCL1/UTXD1/PQS[0]}", "24-G2 {QSPI_DIN/I2C_SDA1/URXD1/PQS[1]}", "26-H3 {QSPI_SCK/I2C_SCL0/URTS1/PQS[2]}", "27-H4 {QSPI_CS0/I2C_SDA0/UCTS1/PQS[3]}", "NA", "23-G3 {QSPI_CS2/SYNCB/USB_DM_PDOWN/PQS[5]}", "22-G4 {QSPI_CS3/SYNCA/USB_PD_PDOWN/PQS[6]}", "NA"},
    // AN (0..7)
    { "66-L12 {AN0/PAN[0]}", "67-M9 {AN1/PAN[1]}", "68-M10 {AN2/PAN[2]}", "69-L9 {AN3/PAN[3]}", "77-K12 {AN4/PAN[4]}", "76-K11 {AN5/PAN[5]}", "75-K10 {AN6/PAN[6]}", "74-K9 {AN7/PAN[7]}"},                   
    // NQ (0..7)
    { "NA", "103-E9 {IRQ1/USB_ALTCLK/PNQ[1]}", "NA", "129-A6 {IRQ3/FEC_MDIO/PNQ[3]}", "NA", "128-A7 {IRQ5/FEC_MDC/PNQ[5]}", "NA", "96-E12 {IRQ7/PNQ[7]}"},                  
    // TA (0..3)    
    { "93-F12 {GPT0/PWM1/PTA[0]}", "94-J11 {GPT1/PWM3/PTA[1]}", "95-J10 {GPT2/PWM5/PTA[2]}", "58-M7 {GPT3/PWM7/PTA[3]}", "", "", "", ""}, 
    // TC (0..3)
    { "11-D2 {DTIN0/DTOUT0/PWM0/PTC[0]}", "12-C1 {DTIN1/DTOUT1/PWM2/PTC[1]}", "65-J8 {DTIN2/DTOUT2/PWM4/PTC[2]}", "19-F4 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},                           
    // UA (0..3)
    { "16-F1 {UTXD0/PUA[0]}",  "17-F2 {URXD0/PUA[1]}",  "18-F3 {URTS0/USB_VBUSD/PUA[2]}", "15-E2 {UCTS0/USB_VBUSE/PUA[3]}", "", "", "", ""}, // {3}
    // UB (0..3)    
    { "62-K8 {UTXD1/I2C_SCL1/PUB[0]}", "63-L8 {URXD1/I2C_SDA1/PUB[1]}", "64-M8 {URTS1/SYNCB/UTXD2/PUB[2]}", "61-K7 {UCTS1/SYNCA/PUB[3]}", "", "", "", ""},                          
    // UC (0..3)
    { "101-D10 {UTXD2/CANTX/PUC[0]}", "102-C10 {URXD2/CANRX/PUC[1]}", "98-E10 {URTS2/I2C_SDA1/USB_VBUSDIS/PUC[2]}", "97-E11 {UCTS2/I2C_SCL1/USB_VBUSCHG/PUC[3]}", "", "", "", ""}, 
    // AS (0..2)
    { "28-H1 {I2C_SCL0/UTXD2/PAS[0]}", "29-H2 {I2C_SDA0/URXD2/PAS[1]}", "37-K3 {FB_ALE/FB_CS1/PAS[2]}", "NA", "", "", "", ""},
    // DD (0..7)
    { "90-G12 {PST[0]/PDD[0]}", "89-G11 {PST[1]/PDD[1]}", "88-G10 {PST[2]/PDD[2]}", "87-F9 {PST[3]/PDD[3]}", "83-F10 {DDATA[0]/PDD[4]}", "84-F11 {DDATA[1]/PDD[5]}", "85-G9 {DDATA[2]/PDD[6]}", "86-H10 {DDATA[3]/PDD[7]}"},
    // TE (0..7)
    { "57-L7 {FB_AD[0]/PTE[0]}", "56-L6 {FB_AD[1]/PTE[1]}", "55-M6 {FB_AD[2]/PTE[2]}", "54-J6 {FB_AD[3]/PTE[3]", "53-K6 {FB_AD[4]/PTE[4]}", "36-K4 {FB_AD[5]/PTE[5]}", "35-J1 {FB_AD[6]/PTE[6]}", "34-J2 {FB_AD[7]/PTE[7]"}, // {4}
    // TF (0..7)    
    { "33-J4 {FB_AD[8]/PTF[0]}", "32-J3 {FB_AD[9]/PTF[1]}", "6-E3 {FB_AD[10]/PTF[2]}", "5-D3 {FB_AD[11]/PTF[3]", "4-C2 {FB_AD[12]/PTF[4]}", "3-C3 {FB_AD[13]/PTF[5]}", "2-B2 {FB_AD[14]/PTF[6]}", "136-C4 {FB_AD[15]/PTF[7]"}, // {2}
    // TG (0..7)    
    { "133-C5 {FB_AD[16]/PTG[0]}", "132-D6 {FB_AD[17]/PTG[1]}", "131-C6 {FB_AD[18]/PTG[2]}", "130-B6 {FB_AD[19]/PTG[3]}", "NA", "52-M5 {FB_CS0/PTG[5]}", "137-B5 {FB_OE/PTG[6]}", "45-M4 {FB_RW/PTG[7]}"}, // {5}
    // TH (0..7)
    { "47-K5 {FB_D2/USB_VBUSE/PTH[0]}", "46-L4 {FB_D3/USB_VBUSD/PTH[1]}", "1-B3 {FB_D4/I2C_SDA1/PTH[2]}", "140-B4 {FB_D5/I2C_SCL1/PTH[3]}", "139-A4 {FB_D6/CANTX/PTH[4]}", "138-A5 {FB_D7/CANRX/PTH[5]}", "51-J5 {FB_D0/SYNCB/PTH[6]}", "50-L5 {FB_D1/SYNCA/PTH[7]}"}, // {}
    // TI (0..7)    
    { "109-B11 {FEC_COL/PTI[0]}", "108-B12 {FEC_CRS/PTI[1]}", "120-B8 {FEC_RXCLK/PTI[2]}", "122-A8 {FEC_RXD[0]/PTI[3]}", "123-B7 {FEC_RXD[1]/PTI[4]}", "126-C7 {FEC_RXD[2]/PTI[5]}", "127-D7 {FEC_RXD[3]/PTI[6]}", "121-C8 {FEC_RXDV/PTI[7]}"},                     
    // TJ (0..7)    
    { "119-A9 {FEC_RXER/PTJ[0]}", "117-B9 {FEC_TXCLK/PTJ[1]}", "113-D9 {FEC_TXD[0]/PTJ[2]}", "112-C9 {FEC_TXD[1]/PTJ[3]}", "111-B10 {FEC_TXD[2]/PTJ[4]}", "110-A11 {FEC_TXD[3]/PTJ[5]}", "116-A10 {FEC_TXEN/PTJ[6]}", "118-D8 {FEC_TXER/PTJ[7]}"},  
};
#endif


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "QSPI_DOUT", "I2C_SCL1", "UTXD1"      },                 // QS
        {  "Q", "QSPI_DIN",  "I2C_SDA1", "URXD1"      },
        {  "Q", "QSPI_SCK",  "I2C_SCL0", "URTS1"      },
        {  "Q", "QSPI_CS0",  "I2C_SDA0", "UCTS1"      },
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "QSPI_CS2",  "SYNCB", "USB_DM_PDOWN"  },
        {  "Q", "QSPI_CS3",  "SYNCA", "USB_PD_PDOWN"  },
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
        {  "NA","NA",        "NA",       "NA"         },                 // NQ
        {  "Q", "IRQ1",      "INVALID",  "USB_ALTCLK" },
        {  "NA","NA",        "NA",       "NA"         },
        {  "Q", "IRQ3",      "FEC_MDIO", "INVALID"    },
        {  "NA","NA",        "NA",       "NA"         },
        {  "Q", "IRQ5",      "FEC_MDC",  "INVALID" },
        {  "NA","NA",        "NA",       "NA"         },
        {  "Q", "IRQ7",      "INVALID",  "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "GPT0",     "INVALID",  "PWM1"       },                 // TA
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
        {  "Q", "TIN0",     "TOUT0",     "PWM0"       },                 // TC
        {  "Q", "TIN1",     "TOUT1",     "PWM2"       },
        {  "Q", "TIN2",     "TOUT2",     "PWM4"       },
        {  "Q", "TIN3",     "TOUT3",     "PWM6"       },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         }
    }, 

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "UTXD0",     "INVALID",  "INVALID"    },                 // UA
        {  "Q", "URXD0",     "INVALID",  "INVALID"    },
        {  "Q", "URTS0",     "INVALID",  "USB_VBUSD"  },
        {  "Q", "UCTS0",     "INVALID",  "USB_VBUSE"  },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "UTXD1",     "I2C_SCL1", "INVALID"    },                 // UB
        {  "Q", "URXD1",     "I2C_SDA1", "INVALID"    },
        {  "Q", "URTS1",     "SYNCB",    "UTXD2"      },
        {  "Q", "UCTS1",     "SYNCA",    "URXD2"      },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "UTXD2",     "CANTX",  "INVALID"      },                 // UC
        {  "Q", "URXD2",     "CANRX",  "INVALID"      },
        {  "Q", "URTS2",   "I2C_SDA1", "USB_VBUSDIS"  },
        {  "Q", "UCTS2",   "I2C_SCL1", "USB_VBUSCHG"  },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "I2C_SCL0",  "INVALID",  "UTXD2"      },                 // AS
        {  "Q", "I2C_SDA0",  "INVALID",  "URXD2"      },
#if defined CHIP_144_PIN
        {  "Q", "FB_ALE",    "FB_CS1",   "INVALID"    },
#else
        {  "NA","NA",        "NA",       "NA"         },
#endif
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
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

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "FB_AD[0]","INVALID",  "INVALID"    },                   // TE
        {  "Q", "FB_AD[1]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[2]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[3]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[4]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[5]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[6]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[7]","INVALID",  "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "FB_AD[8]","INVALID",  "INVALID"    },                   // TF
        {  "Q", "FB_AD[9]","INVALID",  "INVALID"    },
        {  "Q", "FB_AD[10]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[11]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[12]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[13]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[14]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[15]","INVALID", "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "FB_AD[16]","INVALID", "INVALID"    },                   // TG
        {  "Q", "FB_AD[17]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[18]","INVALID", "INVALID"    },
        {  "Q", "FB_AD[19]","INVALID", "INVALID"    },
        {  "NA","NA",       "NA",      "NA"         },
        {  "Q", "FB_CS0",   "INVALID", "INVALID"    },
        {  "Q", "FB_OE",    "INVALID", "INVALID"    },
        {  "Q", "FB_RW",    "INVALID", "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "FB_D2",  "USB_VBUSE", "INVALID"    },                   // TH
        {  "Q", "FB_D3",  "USB_VBUSD", "INVALID"    },
        {  "Q", "FB_D4",  "I2C_SDA1",  "INVALID"    },
        {  "Q", "FB_D5",  "I2C_SCL1",  "INVALID"    },
        {  "Q", "FB_D6",  "CANTX",     "INVALID"    },
        {  "Q", "FB_D7",  "CANRX",     "INVALID"    },
        {  "Q", "FB_D0",  "SYNCB",     "INVALID"    },
        {  "Q", "FB_D1",  "SYNCA",     "INVALID"    }
    },                     

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "FEC_COL",   "INVALID",  "INVALID"  },                   // TI
        {  "Q", "FEC_CRS",   "INVALID",  "INVALID"  },
        {  "Q", "FEC_RXCLK", "INVALID",  "INVALID"  },
        {  "Q", "FEC_RXD[0]","INVALID",  "INVALID"  },
        {  "Q", "FEC_RXD[1]","INVALID",  "INVALID"  },
        {  "Q", "FEC_RXD[2]","INVALID",  "INVALID"  },
        {  "Q", "FEC_RXD[3]","INVALID",  "INVALID"  },
        {  "Q", "FEC_RXDV",  "INVALID",  "INVALID"  }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "FEC_RXER",  "INVALID",  "INVALID"  },                   // TJ
        {  "Q", "FEC_TXCLK", "INVALID",  "INVALID"  },
        {  "Q", "FEC_TXD[0", "INVALID",  "INVALID"  },
        {  "Q", "FEC_TXD[1]","INVALID",  "INVALID"  },
        {  "Q", "FEC_TXD[2]","INVALID",  "INVALID"  },
        {  "Q", "FEC_TXD[3]","INVALID",  "INVALID"  },
        {  "Q", "FEC_TXEN",  "INVALID",  "INVALID"  },
        {  "Q", "FEC_TXER",  "INVALID",  "INVALID"  }
    },
};

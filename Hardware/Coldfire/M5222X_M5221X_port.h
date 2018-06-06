/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M5222X_M5221X_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    25.03.2009 Add M521XX configuration

*/


    #if defined CHIP_64_PIN                                              // 64 pin LQFP/QFN
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // QS (0..7)
        #if defined _M5221X || defined _M521XX
    { "13 {QSPI_DOUT/UTXD1/PQS[0]}", "12 {QSPI_DIN/URXD1/PQS[1]}", "14 {QSPI_CLK/SCL1/URTS1/PQS[2]}", "15 {QSPI_CS0/SDA1/UCTS1/PQS[3]}", "NA", "NA", "NA", "NA"},
        #else
    { "13 {QSPI_DOUT/UTXD1/PQS[0]}", "12 {QSPI_DIN/URXD1/PQS[1]}", "14 {QSPI_CLK/SCL0/URTS1/PQS[2]}", "15 {QSPI_CS0/SDA0/UCTS1/PQS[3]}", "NA", "NA", "NA", "NA"},
        #endif
    // AN (0..7)
    { "25 {AN0/PAN[0]}", "26 {AN1/PAN[1]}", "27 {AN2/PAN[2]}", "28 {AN3/PAN[3]}", "36 {AN4/PAN[4]}", "35 {AN5/PAN[5]}", "34 {AN6/PAN[6]}", "33 {AN7/PAN[7]}"},
    // LD (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},
        #if defined _M5221X || defined _M521XX
    // NQ (0..7)    
    { "NA", "56 {IRQ1/SYNCA/USB_ALT_CLK/PNQ[1]}", "NA", "NA", "57 {IRQ4/PNQ[4]", "NA", "NA", "58 {IRQ7/PNQ[7]"},                  
        #else                                                            // M5222X
    // NQ (0..7)    
    { "NA", "56 {IRQ1/SYNCA/USB_ALT_CLK/PNQ[1]}", "NA", "NA", "57 {IRQ4/USB_PULLUP/PNQ[4]", "NA", "NA", "58 {IRQ7/PNQ[7]"},                  
        #endif
    // TA (0..3)
    { "40 {GPT0/PWM1/PTA[0]}", "41 {GPT1/PWM3/PTA[1]}", "42 {GPT2/PWM5/PTA[2]}", "43 {GPT3/PWM7/PTA[3]}", "", "", "", ""},                           
    // TC (0..3)
    { "22 {DTIN0/DTOUT0/PWM0/PTC[0]}",  "23 {DTIN1/DTOUT1/PWM2/PTC[1]}",  "18 {DTIN2/DTOUT2/PWM4/PTC[2]}", "19 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},   
    // TD (0..3) [dummy - not available]
    { "", "", "", "", "", "", "", ""}, 
        #if defined _M5221X || defined _M521XX
    // UA (0..3)
    { "6 {UTXD0/RTC_XTAL/PUA[0]}", "5 {URXD0/RTC_EXTAL/PUA[1]}", "7 {URTS0/PUA[2]}", "4 {UCTS0/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "62 {UTXD1/SCL1/PUB[0]}", "63 {URXD1/SDA1/PUB[1]}", "2 {URTS1/SYNCB/UTXD2/PUB[2]}", "61 {UCTS1/SYNCA/URXD2/PUB[3]}", "", "", "", ""},
        #else                                                            // M5222X    
    // UA (0..3)
    { "6 {UTXD0/USB_SUSPEND/PUA[0]}", "5 {URXD0/USB_RCV/PUA[1]}", "7 {URTS0/USB_VBUSD/PUA[2]}", "4 {UCTS0/USB_VBUSE/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "62 {UTXD1/USB_SPEED/PUB[0]}", "63 {URXD1/USB_OE/PUB[1]}", "2 {URTS1/SYNCB/UTXD2/PUB[2]}", "61 {UCTS1/SYNCA/URXD2/PUB[3]}", "", "", "", ""},
        #endif
    // UC (0..3)
    { "NA", "NA", "NA", "NA", "", "", "", ""},                           
    // AS (0..1)
    { "8 {SCL0/USB_DMI/UTXD2/PAS[0]}", "9 {SDA0/USB_DPI/URXD2/PAS[1]}", "", "", "", "", "", ""},
    // GP (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},    
    // DD (0..7)    
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"}, 
};
    #else                                                                // 81 pin MAPBGA/100 pin LQFP
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
        #if defined _M5221X || defined _M521XX
    // QS (0..7)
    { "16-F3 {QSPI_DOUT/SCL1/UTXD1/PQS[0]}", "17-G1 {QSPI_DIN/SDA1/URXD1/PQS[1]}", "18-G2 {QSPI_CLK/SCL0/URTS1/PQS[2]}", "20-H1 {QSPI_CS0/SDA0/UCTS1/PQS[3]}", "13-F2 {QSPI_CS1/PQS[4]}", "19-H2 {QSPI_CS2/SYNCB/PQS[5]}", "20-H1 {QSPI_CS0/SYNCA/PQS[6]}", "NA"},
        #else                                                            // M5222X
    // QS (0..7)
    { "17-G1 {QSPI_DOUT/UTXD1/PQS[0]}", "16-F2 {QSPI_DIN/URXD1/PQS[1]}", "18-G2 {QSPI_CLK/SCL/URTS1/PQS[2]}", "20-H1 {QSPI_CS0/SDA/UCTS1/PQS[3]}", "19-H2 {QSPI_CS1/USB_PULLUP/PQS[4]}", "13-F2 {QSPI_CS2/USB_DM_PDOWN/PQS[5]}", "12-F1 {QSPI_CS3/SYNCA/USB_DP_PDOWN/PQS[6]}", "NA"},
        #endif
    // AN (0..7)
    { "43-J6 {AN0/PAN[0]}", "44-H6 {AN1/PAN[1]}", "45-G6 {AN2/PAN[2]}", "46-G7 {AN3/PAN[3]}", "54-F9 {AN4/PAN[4]}", "53-G8 {AN5/PAN[5]}", "52-G9 {AN6/PAN[6]}", "51-H9 {AN7/PAN[7]}"},                   
    // LD (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},
        #if defined _M5221X || defined _M521XX
    // NQ (0..7)    
    { "NA", "87-C6 {IRQ1/SYNCA/USB_ALT_CLK/PNQ[1]}", "88-B5 {IRQ2/PNQ[2]}", "89-A5 {IRQ3/PNQ[3]}", "90-C5 {IRQ4/PNQ[4]", "91-A4 {IRQ5/PNQ[5]}", "94-B4 {IRQ6/PNQ[6]}", "95-C4 {IRQ7/PNQ[7]"},                  
        #else                                                            // M5222X
    // NQ (0..7)    
    { "NA", "87-C6 {IRQ1/SYNCA/USB_ALT_CLK/PNQ[1]}", "88-B5 {IRQ2/USB_SESSVLD/PNQ[2]}", "89-A5 {IRQ3/USB_SESSEND/PNQ[3]}", "90-C5 {IRQ4/USB_PULLUP/PNQ[4]", "91-A4 {IRQ5/USB_VBUSVLD/PNQ[5]}", "94-B4 {IRQ6/USB_VBUSVLD/PNQ[6]}", "95-C4 {IRQ7/USB_ID/PNQ[7]"},                  
        #endif
    // TA (0..3)
    { "38-J5 {GPT0/PWM1/PTA[0]}", "33-J4 {GPT1/PWM3/PTA[1]}", "58-E8 {GPT2/PWM5/PTA[2]}", "63-D7 {GPT3/PWM7/PTA[3]}", "", "", "", ""},                           
    // TC (0..3)
    { "36-H4 {DTIN0/DTOUT0/PWM0/PTC[0]}",  "37-G4 {DTIN1/DTOUT1/PWM2/PTC[1]}",  "31-J3 {DTIN2/DTOUT2/PWM4/PTC[2]}", "32-H3 {DTIN3/DTOUT3/PWM6/PTC[3]}", "", "", "", ""},   
    // TD (0..3) [dummy - not available]
    { "", "", "", "", "", "", "", ""},  
        #if defined _M5221X || defined _M521XX
    // UA (0..3)
    { "8-D2 {UTXD0/RTC_XTAL/PUA[0]}", "7-D1 {URXD0/RTC_EXTAL/PUA[1]}", "9-D3 {URTS0/PUA[2]}", "6-C1 {UCTS0/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "99-A2 {UTXD1/PUB[0]}", "100-B2 {URXD1/PUB[1]}", "4-B1 {URTS1/SYNCB/UTXD2/PUB[2]}", "98-C3 {UCTS1/SYNCA/URXD2/PUB[3]}", "", "", "", ""},
        #else                                                            // M5222X
    // UA (0..3)
    { "8-D2 {UTXD0/USB_SUSPEND/PUA[0]}", "7-D1 {URXD0/USB_RCV/PUA[1]}", "9-D3 {URTS0/USB_VBUSD/PUA[2]}", "6-C1 {UCTS0/USB_VBUSE/PUA[3]}", "", "", "", ""},
    // UB (0..3)
    { "99-A2 {UTXD1/USB_SPEED/PUB[0]}", "100-B2 {URXD1/USB_OE/PUB[1]}", "4-B1 {URTS1/SYNCB/UTXD2/PUB[2]}", "98-C3 {UCTS1/SYNCA/URXD2/PUB[3]}", "", "", "", ""},
        #endif
        #ifdef CHIP_100_PIN
            #if defined _M5221X || defined _M521XX
    // UC (0..3)
    { "29 {UTXD2/PUC[0]}", "28 {URXD2/PUC[1]}", "30 {URTS2/SDA1/PUC[2]}", "27 {UCTS2/SCL1/PUC[3]}", "", "", "", ""},
            #else                                                        // M5222X
    // UC (0..3)
    { "29 {UTXD2/USB_SE0/PUC[0]}", "28 {URXD2/USB_DATA/PUC[1]}", "30 {URTS2/USB_VBUSDIS/PUC[2]}", "27 {UCTS2/USB_VBUSCHG/PUC[3]}", "", "", "", ""},
            #endif
        #else
    // UC (0..3)
    { "NA", "NA", "NA", "NA", "", "", "", ""},
        #endif
    // AS (0..1)
    { "10-E1 {SCL0/USB_DMI/UTXD2/PAS[0]}", "11-E2 {SDA0/USB_DPI/URXD2/PAS[1]}", "", "", "", "", "", ""}, 
    // GP (0..7) [dummy - not available]
    { "", "", "", "", "", "", "", ""},
        #ifdef CHIP_100_PIN
    // DD (0..7)    
    { "65 {PST[0]/PDD[0]}", "66 {PST[1]/PDD[1]}", "69 {PST[2]/PDD[2]}", "70 {PST[3]/PDD[3]}", "77 {DDATA[0]/PDD[4]}", "78 {DDATA[1]/PDD[5]}", "83 {DDATA[2]/PDD[6]}", "84 {DDATA[3]/PDD[7]}"},  
        #else
    // DD (0..7)    
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},  
        #endif
};
    #endif 

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q  Prim. Sec. Tertiary functions
    #if defined _M5221X || defined _M521XX
        {  "Q", "QSPI_DOUT", "SCL1",     "UTXD1"      },                 // QS
        {  "Q", "QSPI_DIN",  "SDA1",     "URXD1"      },
        {  "Q", "QSPI_SCK",  "SCL1",     "URTS1"      },
        {  "Q", "QSPI_CS0",  "SDA1",     "UCTS1"      },
        {  "Q", "QSPI_CS1",  "INVALID",  "INVALID"    },
        {  "Q", "QSPI_CS2",  "SYNCB",    "INVALID"    },
        {  "Q", "QSPI_CS3",  "SYNCA",    "INVALID"    },
    #else                                                                // M5222X
        {  "Q", "QSPI_DOUT", "INVALID",  "UTXD1"      },                 // QS
        {  "Q", "QSPI_DIN",  "INVALID",  "URXD1"      },
        {  "Q", "QSPI_SCK",  "SCL",      "URTS1"      },
        {  "Q", "QSPI_CS0",  "SDA",      "UCTS1"      },
        {  "Q", "QSPI_CS1",  "INVALID",  "USB_PULLUP" },
        {  "Q", "QSPI_CS2",  "INVALID",  "USB_DM_PDOWN"},
        {  "Q", "QSPI_CS3",  "SYNCA",    "USB_DP_PDOWN"},
    #endif
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
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },                 // Dummy LD
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    }
    },

    #if defined _M5221X || defined _M521XX
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "NA","NA",        "NA",       "NA"         },                 // NQ
        {  "Q", "IRQ1",      "SYNCA",    "INVALID"    },
        {  "Q", "IRQ2",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ3",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ4",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ5",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ6",      "INVALID",  "INVALID"    },
        {  "Q", "IRQ7",      "INVALID",  "INVALID"    }
    },
    #else                                                                // M5222X
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "NA","NA",        "NA",       "NA"         },                 // NQ
        {  "Q", "IRQ1",      "SYNCA",    "USB_ALT_CLK"},
        {  "Q", "IRQ2",      "INVALID",  "USB_SESSVLD"},
        {  "Q", "IRQ3",      "INVALID",  "USB_SESSEND"},
        {  "Q", "IRQ4",      "INVALID",  "USB_PULLUP" },
        {  "Q", "IRQ5",      "INVALID",  "USB_VBUSVLD"},
        {  "Q", "IRQ6",      "INVALID",  "USB_ID"     },
        {  "Q", "IRQ7",      "INVALID",  "INVALID"    }
    },
    #endif
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "GPT0",     "INVALID",   "PWM1"       },                 // TA
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
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },                 // Dummy TD
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
    #if defined _M5221X || defined _M521XX
        {  "Q", "UTXD0",     "RTC_XTAL", "INVALID"    },                 // UA
        {  "Q", "URXD0",     "RTC_EXTAL","INVALID"    },
        {  "Q", "URTS0",     "INVALID",  "INVALID"    },
        {  "Q", "UCTS0",     "INVALID",  "INVALID"    },
    #else                                                                // M5222X
        {  "Q", "UTXD0",     "INVALID",  "USB_SUSPEND"},                 // UA
        {  "Q", "URXD0",     "INVALID",  "USB_RCV"    },
        {  "Q", "URTS0",     "INVALID",  "USB_VBUSD"  },
        {  "Q", "UCTS0",     "INVALID",  "USB_VBUSE"  },
    #endif
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
    #if defined _M5221X || defined _M521XX
        {  "Q", "UTXD1",     "SCL1",     "INVALID"    },                 // UB
        {  "Q", "URXD1",     "SDA1",     "INVALID"    },
    #else                                                                // M5222X
        {  "Q", "UTXD1",     "INVALID",  "USB_SPEED"  },                 // UB
        {  "Q", "URXD1",     "INVALID",  "USB_OE"     },
    #endif
        {  "Q", "URTS1",     "SYNCB",    "UTXD2"      },
        {  "Q", "UCTS1",     "SYNCA",    "URXD2"      },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },
    #ifdef CHIP_100_PIN
    {
        //  Q  Prim. Sec. Tertiary functions
        #if defined _M5221X || defined _M521XX
        {  "Q", "UTXD2",     "INVALID",  "INVALID"    },                 // UC
        {  "Q", "URXD2",     "INVALID",  "INVALID"    },
        {  "Q", "URTS2",     "SDA1",     "INVALID"    },
        {  "Q", "UCTS2",     "SCL1",     "INVALID"    },
        #else                                                            // M5222X
        {  "Q", "UTXD2",     "INVALID",  "USB_SE0"    },                 // UC
        {  "Q", "URXD2",     "INVALID",  "USB_DATA"   },
        {  "Q", "URTS2",     "INVALID",  "USB_VBUSDIS"},
        {  "Q", "UCTS2",     "INVALID",  "USB_VBUSCHG"},
        #endif
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         },
        {  "NA","NA",        "NA",       "NA"         }
    },
    #else
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },                 // Dummy UC on chips without {2}
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    },
        {  "Q", "INVALID",   "INVALID",  "INVALID"    }
    },
    #endif
    {
        //  Q  Prim. Sec. Tertiary functions
    #if defined _M5221X || defined _M521XX
        {  "Q", "SCL0",      "USB_DMI",  "UTXD2"      },                 // AS
        {  "Q", "SDA0",      "USB_DPI",  "URXD2"      },
    #else
        {  "Q", "SCL",       "USB_DMI",  "UTXD2"      },                 // AS
        {  "Q", "SDA",       "USB_DPI",  "URXD2"      },
    #endif
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

/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M520X.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // FECH (0..7)
    { "D2 {FECH[0]/FEC_CRS}", "D5 {FECH[1]/FEC_RXD0}", "A4 {FECH[2]/FEC_RXDV}", "B4 {FECH[3]/FEC_RXCLK}", "D3 {FECH[4]/FEC_COL}", "B1 {FECH[5]/FEC_TXD0}", "A2 {FECH[6]/U1RTS/FEC_TXEN}", "B3 {FECH[7]/FEC_TXCLK}"},
    // FECL (0..7)
    { "C4 {FECL[0]/U0CTS/FEC_RXER}", "C5 {FECL[1]/U1CTS/FEC_RXD1}", "B5 {FECL[2]/FEC_RXD2}", "A5 {FECL[3]/FEC_RXD3}", "A3 {FECL[4]/U0RTS/FEC_TXER}", "B2 {FECL[5]/FEC_TXD1}", "C2 {FECL[6]/FEC_TXD2}", "C1 {FECL[7]/FEC_TXD3}"},
    // BUSCTL (0..3)
    { "E3 {BUSCTL[0]/DACK0/TS}", "L8 {BUSCTL[1]/RW}", "H14 {BUSCTL[2]/TA}", "M8 {BUSCTL[3]/OE}", "NA", "NA", "NA", "NA"}, 
    // BE (0..3)
    { "M6 {BE[0]/SD_DQM0}", "H4 {BE[1]/SD_DQM1}", "P5 {BE[2]/SD_DQM2}", "H2 {BE[3]/SD_DQM3}", "NA", "NA", "NA", "NA"},
    // CS (1..3)
    { "NA", "B10 {CS[1]/SD_CS1/FB_CS1}", "A10 {CS[2]/FB_CS2}", "C11 {CS[3]/FB_CS3}", "NA", "NA", "NA", "NA"},
    // FECI2C (0..3)
    { "D1 {U2RXD/FECI2C[0]/I2C_SDA}", "E4 {U2TXD/FECI2C[1]/I2C_SCL}", "C6 {U2RXD/I2C_SDA/FECI2C[2]/FEC_MDIO}", "D6 {U2TXD/I2C_SCL/FECI2C[3]/FEC_MDC}", "NA", "NA", "NA", "NA"},
    // UART (0..7)
    { "N13 {UART[0]/U0RXD}", "P13 {UART[1]/U0TXD}", "P12 {UART[2]/QSPI_SC0/DT0OUT/U0RTS}", "N12 {UART[3]/QSPI_CS0/DT0IN/U0CTS}", "D8 {UART[4]/U1RXD}", "A8 {UART[5]/U1TXD}", "C8 {UART[6]/QSPI_CS1/DT1OUT/U1RTS}", "D9 {UART[7]/QSPI_CS1/DT1IN/U1CTS}"},
    // QSPI (0..3)
    { "A9 {QSPI[0]/I2C_SCL/QSPI_CLK}", "B9 {QSPI[1]/I2C_SDA/QSPI_DOUT}", "C9 {QSPI[2]/U2CTS/DREQ0/QSPI_DIN}", "D10 {QSPI[3]/U2RTS/DACK0/QSPI_CS2}", "NA", "NA", "NA", "NA"},
    // TIMER (0..3)
    { "B6 {TIMER[0]/U2TXD/DT0OUT/DT0IN}", "A6 {TIMER[1]/U2RXD/DT1OUT/DT1IN}", "A7 {TIMER[2]/U2RTS/DT2OUT/DT2IN}", "B7 {TIMER[3]/U2CTS/DT3OUT/DT3IN}", "NA", "NA", "NA", "NA"},
    // IRQ (1, 4, 7)
    { "NA", "D8 {IRQ[1]}", "NA", "NA", "D7 {IRQ[4]/DREQ0}", "NA", "NA", "C7 {IRQ[7]}"},
};


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q   Prim.   Sec.     Tertiary functions
        {  "Q", "FECH[0]","FECH[0]", "FEC_CRS"      },                   // FECH
        {  "Q", "FECH[1]","FECH[1]", "FEC_RXD0"     },
        {  "Q", "FECH[2]","FECH[2]", "FEC_RXDV"     },
        {  "Q", "FECH[3]","FECH[3]", "FEC_RXCLK"    },
        {  "Q", "FECH[4]","FECH[4]", "FEC_COL"      },
        {  "Q", "FECH[5]","FECH[5]", "FEC_TXD0"     },
        {  "Q", "U1RTS",  "FECH[6]", "FEC_TXEN"     },
        {  "Q", "FECH[7]","FECH[7]", "FEC_TXCLK"    }
    },
    {
        //  Q   Prim.   Sec.     Tertiary functions
        {  "Q", "U0CTS",  "FECL[0]", "FEC_RXER"     },                   // FECL
        {  "Q", "U1CTS",  "FECL[1]", "FEC_RXD1"     },
        {  "Q", "FECL[2]","FECL[2]", "FEC_RXD2"     },
        {  "Q", "FECL[3]","FECL[3]", "FEC_RXD3"     },
        {  "Q", "U0RTS",  "FECL[4]", "FEC_TXER"     },
        {  "Q", "FECL[5]","FECL[5]", "FEC_TXD1"     },
        {  "Q", "FECL[6]","FECL[6]", "FEC_TXD2"     },
        {  "Q", "FECL[7]","FECL[7]", "FEC_TXD3"     }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "BUSCTL[0]", "DACK0", "TS"       },                      // BUSCTL
        {  "Q", "RW",  "INVALID", "INVALID"      },
        {  "Q", "TA",  "INVALID", "INVALID"      },
        {  "Q", "OE",  "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"  },
        {  "Q", "INVALID", "INVALID", "INVALID"  },
        {  "Q", "INVALID", "INVALID", "INVALID"  },
        {  "Q", "INVALID", "INVALID", "INVALID"  }
    },       
    {
        //  Q   Prim. Sec.     Tertiary functions
        {  "Q", "SD_DQM0", "INVALID", "INVALID"      },                  // BE
        {  "Q", "SD_DQM1", "INVALID", "INVALID"      },
        {  "Q", "SD_DQM2", "INVALID", "INVALID"      },
        {  "Q", "SD_DQM3", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      }
    }, 
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "INVALID","INVALID","INVALID"      },                    // CS
        {  "Q", "CS[1]",  "SD_CS1",  "FB_CS1"      },
        {  "Q", "FB_CS2", "INVALID", "SINVALIDYNCB"},
        {  "Q", "FB_CS3", "INVALID", "INVALID"     },
        {  "Q", "INVALID","INVALID", "INVALID"     },
        {  "Q", "INVALID","INVALID", "INVALID"     },
        {  "Q", "INVALID","INVALID", "INVALID"     },
        {  "Q", "INVALID","INVALID", "INVALID"     }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "U2RXD", "FECI2C[0]", "I2C_SDA"      },                  // FECI2C
        {  "Q", "U2TXD", "FECI2C[1]", "I2C_SCL"      },
        {  "Q", "U2RXD", "I2C_SDA",   "FEC_MDIO"     },
        {  "Q", "U2TXD", "I2C_SCL",   "FEC_MDC"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      },
        {  "Q", "INVALID", "INVALID", "INVALID"      }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "U0RXD","INVALID","INVALID"    },                        // UART
        {  "Q", "U0TXD","INVALID","INVALID"    },
        {  "Q", "QSPI_CS0","DT0OUT", "U0RTS"   },
        {  "Q", "QSPI_CS0","DT0IN",  "U0CTS"   },
        {  "Q", "U1RXD","INVALID","INVALID"    },
        {  "Q", "U1TXD","INVALID","INVALID"    },
        {  "Q", "QSPI_CS1","DT1OUT","U1CTS"    },
        {  "Q", "QSPI_CS1","DT1IN","U1CTS"     }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "QSPI[0]", "I2C_SCL", "QSPI_CLK"  },                     // QSPI
        {  "Q", "QSPI[1]", "I2C_SDA", "QSPI_DOUT" },
        {  "Q", "U2CTS",   "DREQ0",   "QSPI_DIN"  },
        {  "Q", "U2RTS",   "DACK0",   "QSPI_CS2"  },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   },
        {  "Q", "INVALID", "INVALID", "INVALID"   }
    },
    {
        //  Q   Prim.  Sec.     Tertiary functions
        {  "Q", "U2TXD",  "DT0OUT", "DT0IN"     },                       // TIMER
        {  "Q", "U2RXD",  "DT1OUT", "DT1IN"     },
        {  "Q", "U2RTS",  "DT2OUT", "DT2IN"     },
        {  "Q", "U2CTS",  "DT3OUT", "DT3IN"     },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   },
        {  "Q", "INVALID","INVALID","INVALID"   }
    }, 
    {
        //  Q   Prim.    Sec.      Tertiary functions
        {  "Q", "INVALID",  "INVALID", "INVALID"      },                 // IRQ
        {  "Q", "INVALID",  "INVALID", "INVALID"      },
        {  "Q", "INVALID",  "INVALID", "INVALID"      },
        {  "Q", "INVALID",  "INVALID", "INVALID"      },
        {  "Q", "DREQ0",    "INVALID", "INVALID"      },
        {  "Q", "INVALID",  "INVALID", "INVALID"      },
        {  "Q", "INVALID",  "INVALID", "INVALID"      },
        {  "Q", "INVALID",  "INVALID", "INVALID"      }
    }
};

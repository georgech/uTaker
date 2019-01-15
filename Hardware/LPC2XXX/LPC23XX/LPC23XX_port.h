/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        LPC23XX_port.h
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    02.08.2010 Corrected PWM1.1 output                                   {1}

*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
#ifdef DEVICE_144_PIN
    // PORT 0 (0..31)
    { "66 {P0.0/RD1/TXD3/SDA1}", "67 {P0.1/TD1/RXD3/SCL1}", "141 {P0.2/TXD0}", "142 {P0.3/RXD0}", "116 {P0.4/I2SRX_CLK/RD2/CAP2.0}", "115 {P0.5/I2SRX_WS/TD2/CAP2.1}", "113 {P0.6/I2SRX_SDA/SSEL1/MAT2.0}", "112 {P0.7/I2STX_CLK/SCK1/MAT2.1}", "111 {P0.8/I2STX_WS/MISO1/MAT2.2}", "109 {P0.9/I2STX_SDA/MOSI1/MAT2.3}", "69 {P0.10/TXD2/SDA2/MAT3.0}", "70 {P0.11/RXD2/SCL2/MAT3.1}", "29 {P0.12/MISO1/AD0.6}", "32 {P0.13/U2UP_LED/MOSI1/AD0.7}", "48 {P0.14/U2CONNECT/SSEL1}", "89 {P0.15/TXD1/SCK0/SCK}", "90 {P0.16/RXD1/SSEL0/SSEL}", "87 {P0.17/CTS1/MISO0/MISO}", "86 {P0.18/DCD1/MOSI0/MOSI}", "85 {P0.19/DSR1/MCICLK/SDA1}", "83 {P0.20/DTR1/MCICMD/SCL1}", "82 {P0.21/RI1/MCIPWR/RD1}", "80 {P0.22/RTS1/MCIDAT0/TD1}", "13 {P0.23/AD0.0/I2SRX_CLK/CAP3.0}", "11 {P0.24/AD0.1/I2SRX_WS/CAP3.1}", "10 {P0.25/AD0.2/I2SRX_SDA/TXD3}", "8 {P0.26/AD0.3/AOUT/RXD3}", "35 {P0.27/SDA0}", "34 {P0.28/SCL0}", "42 {P0.29/U1D+}", "43 {P0.30/U1D-}", "36 {P0.31/U2D+}"},
    // PORT 1 (0..31)
    { "136 {P1.0/ENET_TXD0}", "135 {P1.1/ENET_TXD1}", "NA", "NA", "133 {P1.4/ENET_TX_EN}", "NA", "NA", "NA", "132 {P1.8/ENET_CRS}", "131 {P1.9/ENET_RXD0}", "129 {P1.10/ENET_RXD1}", "NA", "NA", "NA", "128 {P1.14/ENET_RX_ER}", "126 {P1.15/ENET_REF_CLK}", "125 {P1.16/ENET_MDC}", "123 {P1.17/ENET_MDIO}", "46 {P1.18/U1UP_LED/PWM1.1/CAP1.0}", "47 {P1.19/CAP1.1}", "49 {P1.20/PWM1.2/SCK0}", "50 {P1.21/PWM1.3/SSEL0}", "51 {P1.22/MAT1.0}", "53 {P1.23/PWM1.4/MISO0}", "54 {P1.24/PWM1.5/MOSI0}", "56 {P1.25/MAT1.1}", "57 {P1.26/PWM1.6/CAP0.0}", "61 {P1.27/CAP0.1}", "63 {P1.28/PCAP1.0/MAT0.0}", "64 {P1.29/PCAP1.1/MAT0.1}", "30 {P1.30/VBUS/AD0.4}", "28 {P1.31/SCK1/AD0.5}"},
    // PORT 2 (0..31)
    { "107 {P2.0/PWM1.1/TXD1/TRACECLK}", "106 {P2.1/PWM1.2/RXD1/PIPESTAT0}", "105 {P2.2/PWM1.3/CTS1/PIPESTAT1}", "100 {P2.3/PWM1.4/DCD1/PIPESTAT2}", "99 {P2.4/PWM1.5/DSR1/TRACESYNC}", "97 {P2.5/PWM1.6/DTR1/TRACEPKT0}", "96 {P2.6/PCAP1.0/RI1/TRACEPKT1}", "95 {P2.7/RD2/RTS1/TRACEPKT2}", "93 {P2.8/TD2/TXD2/TRACEPKT3}", "92 {P2.9/U1CONNECT/RXD2/EXTIN0}", "76 {P2.10/EINT0(ISP)}", "75 {P2.11/EINT1/MCIDAT1/I2STX_CLK}", "73 {P2.12/EINT2/MCIDAT2/I2STX_WS}", "71 {P2.13/EINT3/MCIDAT3/I2STX_SDA}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT 3 (0..31)
    { "137 {P3.0/D0}", "140 {P3.1/D1}", "144 {P3.2/D2}", "2 {P3.3/D3}", "9 {P3.4/D4}", "12 {P3.5/D5}", "16 {P3.6/D6}", "19 {P3.7/D7}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "45 {P3.23/CAP0.0/PCAP1.0}", "40 {P3.24/CAP0.1/PWM1.1}", "39 {P3.25/MAT0.0/PWM1.2}", "38 {P3.26/MAT0.1/PWM1.3}", "NA", "NA", "NA", "NA", "NA"},
    // PORT 4 (0..31)
    { "52 {P4.0/A0}", "55 {P4.1/A1}", "58 {P4.2/A2}", "68 {P4.3/A3}", "72 {P4.4/A4}", "74 {P4.5/A5}", "78 {P4.6/A6}", "84 {P4.7/A7}", "88 {P4.8/A8}", "91 {P4.9/A9}", "94 {P4.10/A10}", "101 {P4.11/A11}", "104 {P4.12/A12}", "108 {P4.13/A13}", "110 {P4.14/A14}", "120 {P4.15/A15}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "127 {P4.24/OE}", "124 {P4.25/WE}", "NA", "NA", "118 {P4.28/MAT2.0/TXD3}", "122 {P4.29/MAT2.1/RXD3}", "130 {P4.30/CS0}", "130 {P4.31/CS1}"}
#else                                                                    // 100 pin device
    // PORT 0 (0..31)
    { "46 {P0.0/RD1/TXD3/SDA1}", "47 {P0.1/TD1/RXD3/SCL1}", "98 {P0.2/TXD0}", "99 {P0.3/RXD0}", "81 {P0.4/I2SRX_CLK/RD2/CAP2.0}", "80 {P0.5/I2SRX_WS/TD2/CAP2.1}", "79 {P0.6/I2SRX_SDA/SSEL1/MAT2.0}", "78 {P0.7/I2STX_CLK/SCK1/MAT2.1}", "77 {P0.8/I2STX_WS/MISO1/MAT2.2}", "76 {P0.9/TXD2/SDA2/MAT3.0}", "48 {P0.10/TXD2/SDA2/MAT3.0}", "49 {P0.11/RXD2/SCL2/MAT3.1}", "NA", "NA", "NA", "62 {P0.15/TXD1/SCK0/SCK}", "63 {P0.16/RXD1/SSEL0/SSEL}", "61 {P0.17/CTS1/MISO0/MISO}", "60 {P0.18/DCD1/MOSI0/MOSI}", "59 {P0.19/DSR1/MCICLK/SDA1}", "58 {P0.20/DTR1/MCICMD/SCL1}", "57 {P0.21/RI1/MCIPWR/RD1}", "56 {P0.22/RTS1/MCIDAT0/TD1}", "9 {P0.23/AD0.0/I2SRX_CLK/CAP3.0}", "8 {P0.24/AD0.1/I2SRX_WS/CAP3.1}", "7 {P0.25/AD0.2/I2SRX_SDA/TXD3}", "6 {P0.26/AD0.3/AOUT/RXD3}", "25 {P0.27/SDA0}", "24 {P0.28/SCL0}", "29 {P0.29/U1D+}", "30 {P0.30/U1D-}", "NA"},
    // PORT 1 (0..31)
    { "95 {P1.0/ENET_TXD0}", "94 {P1.1/ENET_TXD1}", "NA", "NA", "93 {P1.4/ENET_TX_EN}", "NA", "NA", "NA", "92 {P1.8/ENET_CRS}", "91 {P1.9/ENET_RXD0}", "90 {P1.10/ENET_RXD1}", "NA", "NA", "NA", "89 {P1.14/ENET_RX_ER}", "88 {P1.15/ENET_REF_CLK}", "87 {P1.16/ENET_MDC}", "86 {P1.17/ENET_MDIO}", "32 {P1.18/U1UP_LED/PWM1.1/CAP1.0}", "33 {P1.19/CAP1.1}", "34 {P1.20/PWM1.2/SCK0}", "35 {P1.21/PWM1.3/SSEL0}", "36 {P1.22/MAT1.0}", "37 {P1.23/PWM1.4/MISO0}", "38 {P1.24/PWM1.5/MOSI0}", "39 {P1.25/MAT1.1}", "40 {P1.26/PWM1.6/CAP0.0}", "43 {P1.27/CAP0.1}", "44 {P1.28/PCAP1.0/MAT0.0}", "45 {P1.29/PCAP1.1/MAT0.1}", "21 {P1.30/VBUS/AD0.4}", "20 {P1.31/SCK1/AD0.5}"},
    // PORT 2 (0..31)
    { "75 {P2.0/PWM1.1/TXD1/TRACECLK}", "74 {P2.1/PWM1.2/RXD1/PIPESTAT0}", "73 {P2.2/PWM1.3/CTS1/PIPESTAT1}", "70 {P2.3/PWM1.4/DCD1/PIPESTAT2}", "69 {P2.4/PWM1.5/DSR1/TRACESYNC}", "68 {P2.5/PWM1.6/DTR1/TRACEPKT0}", "67 {P2.6/PCAP1.0/RI1/TRACEPKT1}", "66 {P2.7/RD2/RTS1/TRACEPKT2}", "65 {P2.8/TD2/TXD2/TRACEPKT3}", "64 {P2.9/U1CONNECT/RXD2/EXTIN0}", "53 {P2.10/EINT0/(ISP)}", "52 {P2.11/EINT1/MCIDAT1/I2STX_CLK}", "51 {P2.12/EINT2/MCIDAT2/I2STX_WS}", "52 {P2.13/EINT3/MCIDAT3/I2STX_SDA}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT 3 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "27 {P3.25/MAT0.0/PWM1.2}", "26 {P3.26/MAT0.1/PWM1.3}", "NA", "NA", "NA", "NA", "NA"},
    // PORT 4 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "82 {P4.28/MAT2.0/TXD3}", "85 {P4.29/MAT2.1/RXD3}", "NA", "NA"}
#endif
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "RD1",      "TXD3",     "SDA1"    },                     // PORT 0
        {  "Q", "TD1",      "RXD3",     "SCL1"    },
        {  "Q", "TXD0",     "INVALID",  "INVALID" },
        {  "Q", "RXD0",     "INVALID",  "INVALID" },
        {  "Q", "I2SRX_CLK","RD2",      "CAP2.0"  },
        {  "Q", "I2SRX_WS", "TD2",      "CAP2.1"  },
        {  "Q", "I2SRX_SDA","SSEL1",    "MAT2.0"  },
        {  "Q", "I2STX_CLK","SCK1",     "MAT2.1"  },
        {  "Q", "I2STX_WS", "MISO1",    "MAT2.2"  },                 
        {  "Q", "I2STX_SDA","MOSI1",    "MAT2.3"  },
        {  "Q", "TXD2",     "SDA2",     "MAT3.0"  },
        {  "Q", "RXD2",     "SCL2",     "MAT3.1"  },
        {  "Q", "INVALID",  "MISO1",    "AD0.6"   },
        {  "Q", "USB_UP_LED2","MOSI1",  "AD0.7"   },
        {  "Q", "USB_HSTEN2","USB_CONNECT2","SSEL1"},
        {  "Q", "TXD1",     "SCK0",     "SCK"     },
        {  "Q", "RXD1",     "SSEL0",    "SSEL"    },
        {  "Q", "CTS1",     "MISO0",    "MISO"    },                 
        {  "Q", "DCD1",     "MOSI0",    "MOSI"    },
        {  "Q", "DSR1",     "MCICLK",   "SDA1"    },
        {  "Q", "DTR1",     "MCICMD",   "SCL1"    },
        {  "Q", "RI1",      "MCIPWR",   "RD1"     },
        {  "Q", "RTS1",     "MCIDAT0",  "TD1"     },
        {  "Q", "AD0.0",    "I2SRX_CLK","CAP3.0"  },
        {  "Q", "AD0.1",    "I2SRX_WS", "CAP3.1"  },
        {  "Q", "AD0.2",    "I2SRX_SDA","TXD3"    },                 
        {  "Q", "AD0.3",    "AOUT",     "RXD3"    },
        {  "Q", "SDA0",     "INVALID",  "INVALID" },
        {  "Q", "SCL0",     "INVALID",  "INVALID" },
        {  "Q", "U1D+",     "INVALID",  "INVALID" },
        {  "Q", "U1D-",     "PCK2",     "INVALID" },
        {  "Q", "U2D+",     "INVALID",  "INVALID" }
    },
    {
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "ENET_TXD0", "INVALID",   "INVALID"  },                  // PORT 1
        {  "Q", "ENET_TXD1", "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "ENET_TX_EN","INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "ENET_CRS",  "INVALID",   "INVALID"  },                 
        {  "Q", "ENET_RXD0", "INVALID",   "INVALID"  },
        {  "Q", "ENET_RXD1", "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "ENET_RX_ER","INVALID",   "INVALID"  },
        {  "Q", "ENET_REF_CLK","INVALID", "INVALID"  },
        {  "Q", "ENET_MDC",  "INVALID",   "INVALID"  },
        {  "Q", "ENET_MDIO", "INVALID",   "INVALID"  },                 
        {  "Q", "U1UP_LED",  "PWM1.1",    "CAP1.0"   },
        {  "Q", "INVALID",   "INVALID",   "CAP1.1"   },
        {  "Q", "INVALID",   "PWM1.2",    "SCK0"     },
        {  "Q", "INVALID",   "PWM1.3",    "SSEL0"    },
        {  "Q", "INVALID",   "INVALID",   "MAT1.0"   },
        {  "Q", "INVALID",   "PWM1.4",    "MISO0"    },
        {  "Q", "INVALID",   "PWM1.5",    "MOSI0"    },
        {  "Q", "INVALID",   "INVALID",   "MAT1.0"   },
        {  "Q", "INVALID",   "PWM1.6",    "CAP0.0"   },                 
        {  "Q", "INVALID",   "INVALID",   "CAP0.1"   },
        {  "Q", "INVALID",   "PCAP1.0",   "MAT0.0"   },
        {  "Q", "INVALID",   "PCAP1.1",   "MAT0.1"   },
        {  "Q", "INVALID",   "VBUS",      "AD0.4"    },
        {  "Q", "INVALID",   "SCK1",      "AD0.5"    }
    },
    {
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "PWM1.1",    "TXD1",      "TRACECLK"  },                 // PORT 2
        {  "Q", "PWM1.2",    "RXD1",      "PIPESTAT0" },
        {  "Q", "PWM1.3",    "CTS1",      "PIPESTAT1" },
        {  "Q", "PWM1.4",    "DCD1",      "PIPESTAT2" },
        {  "Q", "PWM1.5",    "DSR1",      "TRACESYN"  },
        {  "Q", "PWM1.6",    "DTR1",      "TRACEPKT0" },
        {  "Q", "PCAP1.0",   "RI1",       "TRACEPKT1" },
        {  "Q", "RD2",       "RTS1",      "TRACEPKT2" },
        {  "Q", "TD2",       "TXD2",      "TRACEPKT3" },                 
        {  "Q", "U1CONNECT", "RXD2",      "EXTIN0"    },
        {  "Q", "EINT0",     "INVALID",   "INVALID"   },
        {  "Q", "EINT1",     "MCIDAT1",   "I2STX_CLK" },
        {  "Q", "EINT2",     "MCIDAT2",   "I2STX_WS"  },
        {  "Q", "EINT3",     "MCIDAT3",   "I2STX_SDA" },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"   }
    },
    {
        // GPIO Function 1,  Function 2,  Function3                      // PORT 3
        {  "Q", "D0",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D1",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D2",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D3",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D4",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D5",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D6",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "D7",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "CAP0.0",    "PCAP1.0"  },                  // only LPC2378
        {  "Q", "INVALID",   "CAP0.1",    "PWM1.1"   },                  // only LPC2378 {1}
        {  "Q", "INVALID",   "MAT0.0",    "PWM1.2"   },
        {  "Q", "INVALID",   "MAT0.1",    "PWM1.3"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  }
    },
    {
        // GPIO Function 1,  Function 2,  Function3                      // PORT 4
        {  "Q", "A0",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A1",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A2",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A3",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A4",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A5",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A6",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A7",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A8",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A9",        "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A10",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A11",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A12",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A13",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A14",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "A15",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "A24",       "OE",        "INVALID"  },                  // only LPC2378
        {  "Q", "A25",       "WE",        "INVALID"  },                  // only LPC2378
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "MAT2.0",    "TXD3"     },
        {  "Q", "INVALID",   "MAT2.1",    "RXD3"     },
        {  "Q", "CS0",       "INVALID",   "INVALID"  },                  // only LPC2378
        {  "Q", "CS1",       "INVALID",   "INVALID"  }                   // only LPC2378
    }
};

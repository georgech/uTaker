/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        LPC17XX_port.h
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2013
   *********************************************************************/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
#ifdef DEVICE_80_PIN
    // PORT 0 (0..31)
    { "46 {P0.0/RD1/TXD3/SDA1}", "47 {P0.1/TD1/RXD3/SCL1}", "98 {P0.2/TXD0/AD0[7]}", "99 {P0.3/RXD0/ADC0[6]}", "NA", "NA", "79 {P0.6/I2SRX_SDA/SSEL1/MAT2.0}", "78 {P0.7/I2STX_CLK/SCK1/MAT2.1}", "77 {P0.8/I2STX_WS/MISO1/MAT2.2}", "76 {P0.9/TXD2/SDA2/MAT3.0}", "48 {P0.10/TXD2/SDA2/MAT3.0}", "49 {P0.11/RXD2/SCL2/MAT3.1}", "NA", "NA", "NA", "62 {P0.15/TXD1/SCK0/SCK}", "63 {P0.16/RXD1/SSEL0/SSEL}", "61 {P0.17/CTS1/MISO0/MISO}", "60 {P0.18/DCD1/MOSI0/MOSI}", "NA", "NA", "NA", "56 {P0.22/RTS1/TD1}", "NA", "NA", "7 {P0.25/AD0.2/I2SRX_SDA/TXD3}", "6 {P0.26/AD0.3/AOUT/RXD3}", "NA", "NA", "29 {P0.29/U1D+}", "30 {P0.30/U1D-}", "NA"},
    // PORT 1 (0..31)
    { "95 {P1.0/ENET_TXD0}", "94 {P1.1/ENET_TXD1}", "NA", "NA", "93 {P1.4/ENET_TX_EN}", "NA", "NA", "NA", "92 {P1.8/ENET_CRS}", "91 {P1.9/ENET_RXD0}", "90 {P1.10/ENET_RXD1}", "NA", "NA", "NA", "89 {P1.14/ENET_RX_ER}", "88 {P1.15/ENET_REF_CLK}", "NA", "NA", "32 {P1.18/U1UP_LED/PWM1.1/CAP1.0}", "33 {P1.19/MC0A/USB_PPWR/CAP1.1}", "34 {P1.20/MCFB0/PWM1.2/SCK0}", "NA", "36 {P1.22/MC0B/USB_PWRD/MAT1.0}", "37 {P1.23/MCFB1/PWM1.4/MISO0}", "38 {P1.24/MCFB2/PWM1.5/MOSI0}", "39 {P1.25/MC1A/CLKOUT/MAT1.1}", "40 {P1.26/MC1B/PWM1.6/CAP0.0}", "NA", "44 {P1.28/MC2A/PCAP1.0/MAT0.0}", "45 {P1.29/MC2A/PCAP1.1/MAT0.1}", "21 {P1.30/VBUS/AD0.4}", "20 {P1.31/SCK1/AD0.5}"},
    // PORT 2 (0..31)
    { "75 {P2.0/PWM1.1/TXD1}", "74 {P2.1/PWM1.2}", "73 {P2.2/PWM1.3/CTS1}", "70 {P2.3/PWM1.4/DCD1}", "69 {P2.4/PWM1.5/DSR1}", "68 {P2.5/PWM1.6/DTR1}", "67 {P2.6/PCAP1.0/RI1}", "66 {P2.7/RD2/RTS1}", "65 {P2.8/TD2/TXD2/ENET_MDC}", "64 {P2.9/U1CONNECT/RXD2/ENET_MDIO}", "53 {P2.10/EINT0/NMI}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT 3 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT 4 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "82 {P4.28/RX_MCLK/MAT2.0/TXD3}", "85 {P4.29/TX_MCLK/MAT2.1/RXD3}", "NA", "NA"}
#else                                                                    // 100 pin
    // PORT 0 (0..31)
    { "46 {P0.0/RD1/TXD3/SDA1}", "47 {P0.1/TD1/RXD3/SCL1}", "98 {P0.2/TXD0/AD0[7]}", "99 {P0.3/RXD0/ADC0[6]}", "81 {P0.4/I2SRX_CLK/RD2/CAP2.0}", "80 {P0.5/I2SRX_WS/TD2/CAP2.1}", "79 {P0.6/I2SRX_SDA/SSEL1/MAT2.0}", "78 {P0.7/I2STX_CLK/SCK1/MAT2.1}", "77 {P0.8/I2STX_WS/MISO1/MAT2.2}", "76 {P0.9/TXD2/SDA2/MAT3.0}", "48 {P0.10/TXD2/SDA2/MAT3.0}", "49 {P0.11/RXD2/SCL2/MAT3.1}", "NA", "NA", "NA", "62 {P0.15/TXD1/SCK0/SCK}", "63 {P0.16/RXD1/SSEL0/SSEL}", "61 {P0.17/CTS1/MISO0/MISO}", "60 {P0.18/DCD1/MOSI0/MOSI}", "59 {P0.19/DSR1/SDA1}", "58 {P0.20/DTR1/SCL1}", "57 {P0.21/RI1/RD1}", "56 {P0.22/RTS1/TD1}", "9 {P0.23/AD0.0/I2SRX_CLK/CAP3.0}", "8 {P0.24/AD0.1/I2SRX_WS/CAP3.1}", "7 {P0.25/AD0.2/I2SRX_SDA/TXD3}", "6 {P0.26/AD0.3/AOUT/RXD3}", "25 {P0.27/SDA0/USB_SDA}", "24 {P0.28/SCL0/USB_SCL}", "29 {P0.29/U1D+}", "30 {P0.30/U1D-}", "NA"},
    // PORT 1 (0..31)
    { "95 {P1.0/ENET_TXD0}", "94 {P1.1/ENET_TXD1}", "NA", "NA", "93 {P1.4/ENET_TX_EN}", "NA", "NA", "NA", "92 {P1.8/ENET_CRS}", "91 {P1.9/ENET_RXD0}", "90 {P1.10/ENET_RXD1}", "NA", "NA", "NA", "89 {P1.14/ENET_RX_ER}", "88 {P1.15/ENET_REF_CLK}", "87 {P1.16/ENET_MDC}", "86 {P1.17/ENET_MDIO}", "32 {P1.18/U1UP_LED/PWM1.1/CAP1.0}", "33 {P1.19/MC0A/USB_PPWR/CAP1.1}", "34 {P1.20/MCFB0/PWM1.2/SCK0}", "35 {P1.21/MCABORT/PWM1.3/SSEL0}", "36 {P1.22/MC0B/USB_PWRD/MAT1.0}", "37 {P1.23/MCFB1/PWM1.4/MISO0}", "38 {P1.24/MCFB2/PWM1.5/MOSI0}", "39 {P1.25/MC1A/CLKOUT/MAT1.1}", "40 {P1.26/MC1B/PWM1.6/CAP0.0}", "43 {P1.27/CLKOUT/USB_OVRCR/CAP0.1}", "44 {P1.28/MC2A/PCAP1.0/MAT0.0}", "45 {P1.29/MC2A/PCAP1.1/MAT0.1}", "21 {P1.30/VBUS/AD0.4}", "20 {P1.31/SCK1/AD0.5}"},
    // PORT 2 (0..31)
    { "75 {P2.0/PWM1.1/TXD1}", "74 {P2.1/PWM1.2}", "73 {P2.2/PWM1.3/CTS1}", "70 {P2.3/PWM1.4/DCD1}", "69 {P2.4/PWM1.5/DSR1}", "68 {P2.5/PWM1.6/DTR1}", "67 {P2.6/PCAP1.0/RI1}", "66 {P2.7/RD2/RTS1}", "65 {P2.8/TD2/TXD2/ENET_MDC}", "64 {P2.9/U1CONNECT/RXD2/ENET_MDIO}", "53 {P2.10/EINT0/NMI}", "52 {P2.11/EINT1/I2STX_CLK}", "51 {P2.12/EINT2/I2STX_WS}", "52 {P2.13/EINT3/I2STX_SDA}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT 3 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "27 {P3.25/MAT0.0/PWM1.2}", "26 {P3.26/STCLK/MAT0.1/PWM1.3}", "NA", "NA", "NA", "NA", "NA"},
    // PORT 4 (0..31)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "82 {P4.28/RX_MCLK/MAT2.0/TXD3}", "85 {P4.29/TX_MCLK/MAT2.1/RXD3}", "NA", "NA"}
#endif
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "RD1",      "TXD3",     "SDA1"    },                     // PORT 0
        {  "Q", "TD1",      "RXD3",     "SCL1"    },
        {  "Q", "TXD0",     "ADC0[7]",  "INVALID" },
        {  "Q", "RXD0",     "ADC0[6]",  "INVALID" },
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
        {  "Q", "DSR1",     "INVALID",  "SDA1"    },
        {  "Q", "DTR1",     "INVALID",  "SCL1"    },
        {  "Q", "RI1",      "INVALID",  "RD1"     },
        {  "Q", "RTS1",     "INVALID",  "TD1"     },
        {  "Q", "AD0.0",    "I2SRX_CLK","CAP3.0"  },
        {  "Q", "AD0.1",    "I2SRX_WS", "CAP3.1"  },
        {  "Q", "AD0.2",    "I2SRX_SDA","TXD3"    },                 
        {  "Q", "AD0.3",    "AOUT",     "RXD3"    },
        {  "Q", "SDA0",     "USB_SDA",  "INVALID" },
        {  "Q", "SCL0",     "USB_SCL",  "INVALID" },
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
        {  "Q", "MC0A",      "USB_PPWR",  "CAP1.1"   },
        {  "Q", "MCFB0",     "PWM1.2",    "SCK0"     },
        {  "Q", "MCABORT",   "PWM1.3",    "SSEL0"    },
        {  "Q", "MC0B",      "USB_PWRD",  "MAT1.0"   },
        {  "Q", "MCFB1",     "PWM1.4",    "MISO0"    },
        {  "Q", "MCFB2",     "PWM1.5",    "MOSI0"    },
        {  "Q", "MC1A",      "CLKOUT",    "MAT1.0"   },
        {  "Q", "MC1B",      "PWM1.6",    "CAP0.0"   },                 
        {  "Q", "CLKOUT",    "USB_OVRCR", "CAP0.1"   },
        {  "Q", "MC2A",      "PCAP1.0",   "MAT0.0"   },
        {  "Q", "MC2B",      "PCAP1.1",   "MAT0.1"   },
        {  "Q", "INVALID",   "VBUS",      "AD0.4"    },
        {  "Q", "INVALID",   "SCK1",      "AD0.5"    }
    },
    {
        // GPIO Function 1,  Function 2,  Function3
        {  "Q", "PWM1.1",    "TXD1",      "INVALID"   },                 // PORT 2
        {  "Q", "PWM1.2",    "RXD1",      "INVALID"   },
        {  "Q", "PWM1.3",    "CTS1",      "INVALID"   },
        {  "Q", "PWM1.4",    "DCD1",      "INVALID"   },
        {  "Q", "PWM1.5",    "DSR1",      "INVALID"   },
        {  "Q", "PWM1.6",    "DTR1",      "INVALID"   },
        {  "Q", "PCAP1.0",   "RI1",       "INVALID"   },
        {  "Q", "RD2",       "RTS1",      "INVALID"   },
        {  "Q", "TD2",       "TXD2",      "ENET_MDC"  },                 
        {  "Q", "U1CONNECT", "RXD2",      "ENET_MDIO" },
        {  "Q", "EINT0",     "NMI",       "INVALID"   },
        {  "Q", "EINT1",     "INVALID",   "I2STX_CLK" },
        {  "Q", "EINT2",     "INVALID",   "I2STX_WS"  },
        {  "Q", "EINT3",     "INVALID",   "I2STX_SDA" },
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
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "CAP0.0",    "PCAP1.0"  },                  // only LPC2378
        {  "Q", "INVALID",   "CAP0.1",    "CAP1.1"   },                  // only LPC2378
        {  "Q", "STCLK",     "MAT0.0",    "PWM1.2"   },
        {  "Q", "INVALID",   "MAT0.1",    "PWM1.3"   },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  }
    },
    {
        // GPIO Function 1,  Function 2,  Function3                      // PORT 4
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
        {  "Q", "RX_MCLK",   "MAT2.0",    "TXD3"     },
        {  "Q", "TX_MCLK",   "MAT2.1",    "RXD3"     },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  }
    }
};

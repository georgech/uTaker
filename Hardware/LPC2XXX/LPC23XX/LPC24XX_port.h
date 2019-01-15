/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    --------------------------------------------------------------------
    File:        LPC24XX_port.h
    Project:     Single Chip Embedded Internet
    --------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    ********************************************************************/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
#ifdef DEVICE_180_PIN
    // PORT 0 (0..31)
    { "M10 {P0.0/RD1/TXD3/SDA1}", "N11 {P0.1/TD1/RXD3/SCL1}", "D5 {P0.2/TXD0}", "A3 {P0.3/RXD0}", "A11 {P0.4/I2SRX_CLK/RD2/CAP2.0}", "B11 {P0.5/I2SRX_WS/TD2/CAP2.1}", "D11 {P0.6/I2SRX_SDA/SSEL1/MAT2.0}", "B12 {P0.7/I2STX_CLK/SCK1/MAT2.1}", "C12 {P0.8/I2STX_WS/MISO1/MAT2.2}", "A13 {P0.9/I2STX_SDA/MOSI1/MAT2.3}", "L10 {P0.10/TXD2/SDA2/MAT3.0}", "P12 {P0.11/RXD2/SCL2/MAT3.1}", "J4 {P0.12/USB_PPWR2/MISO1/AD0.6}", "J5 {P0.13/USB_UP_LED2/MOSI1/AD0.7}", "M5 {P0.14/USB_HSTEN2/USB_CONNECT2/SSEL1}", "H13 {P0.15/TXD1/SCK0/SCK}", "H14 {P0.16/RXD1/SSEL0/SSEL}", "J12 {P0.17/CTS1/MISO0/MISO}", "J13 {P0.18/DCD1/MOSI0/MOSI}", "J10 {P0.19/DSR1/MCICLK/SDA1}", "K14 {P0.20/DTR1/MCICMD/SCL1}", "K11 {P0.21/RI1/MCIPWR/RD1}", "L14 {P0.22/RTS1/MCIDAT0/TD1}", "F5 {P0.23/AD0.0/I2SRX_CLK/CAP3.0}", "E1 {P0.24/AD0.1/I2SRX_WS/CAP3.1}", "E4 {P0.25/AD0.2/I2SRX_SDA/TXD3}", "D1 {P0.26/AD0.3/AOUT/RXD3}", "L3 {P0.27/SDA0}", "M1 {P0.28/SCL0}", "K5 {P0.29/USB_D+1}", "N4 {P0.30/USB_D-1}", "N1 {P0.31/USB_D+2}"},
    // PORT 1 (0..31)
    { "B5 {P1.0/ENET_TXD0}", "A5 {P1.1/ENET_TXD1}", "B7 {P1.2/ENET_TXD2/MCICLK/PWM0.1}", "A9 {P1.3/ENET_TXD3/MCICMD/PWM0.2}", "C6 {P1.4/ENET_TX_EN}", "B13 {P1.5/ENET_TX_ER/MCIPWR/PWM0.3}", "B10 {P1.6/ENET_TX_CLK/MCIDAT0/PWM0.4}", "C13 {P1.7/ENET_COL/MCIDAT1/PWM0.5}", "B6 {P1.8/ENET_CRS_DV/ENET_CRS}", "D7 {P1.9/ENET_RXD0}", "A7 {P1.10/ENET_RXD1}", "A12 {P1.11/ENET_RXD2/MCIDAT2/PWM0.6}", "A14 {P1.12/ENET_RXD3/MCIDAT3/PCAP0.0}", "D14 {P1.13/ENET_RX_DV}", "D8 {P1.14/ENET_RX_ER}", "A8 {P1.15/ENET_REF_CLK/ENET_RX_CLK}", "B8 {P1.16/ENET_MDC}", "C9 {P1.17/ENET_MDIO}", "L5 {P1.18/USB_UP_LED1/PWM1.1/CAP1.0}", "P5 {P1.19/USB_TX_E1/USB_PPWR1/CAP1.1}", "K6 {P1.20/USB_TX_DP1/PWM1.2/SCK0}", "N6 {P1.21/USB_TX_DM1/PWM1.3/SSEL0}", "M6 {P1.22/USB_RCV1/USB_PWRD1/MAT1.0}", "N7 {P1.23/USB_RX_DP1/PWM1.4/MISO0}", "P7 {P1.24/USB_RX_DM1/PWM1.5/MOSI0}", "L7 {P1.25/USB_LS1/USB_HSTEN1/MAT1.1}", "P8 {P1.26/USB_SSPND1/PWM1.6/CAP0.0}", "M9 {P1.27/USB_INT1/USB_OVRCR1/CAP0.1}", "P10 {P1.28/USB_SCL1/PCAP1.0/MAT0.0}", "N10 {P1.29/USB_SDA1/PCAP1.1/MAT0.1}", "K3 {P1.30/USB_PWRD2/VBUS/AD0.4}", "K2 {P1.31/USB_OVRCR2/SCK1/AD0.5}"},
    // PORT 2 (0..31)
    { "D12 {P2.0/PWM1.1/TXD1/TRACECLK}", "C14 {P2.1/PWM1.2/RXD1/PIPESTAT0}", "E11 {P2.2/PWM1.3/CTS1/PIPESTAT1}", "E13 {P2.3/PWM1.4/DCD1/PIPESTAT2}", "E14 {P2.4/PWM1.5/DSR1/TRACESYNC}", "F12 {P2.5/PWM1.6/DTR1/TRACEPKT0}", "F13 {P2.6/PCAP1.0/RI1/TRACEPKT1}", "G11 {P2.7/RD2/RTS1/TRACEPKT2}", "G14 {P2.8/TD2/TXD2/TRACEPKT3}", "H11 {P2.9/USB_CONNECT1/RXD2/EXTIN0}", "M13 {P2.10/EINT0}", "M12 {P2.11/EINT1/MCIDAT1/I2STX_CLK}", "N14 {P2.12/EINT2/MCIDAT2/I2STX_WS}", "M11 {P2.13/EINT3/MCIDAT3/I2STX_SDA}", "NA", "NA", "P9 {P2.16/CAS}", "P11 {P2.17/RAS}", "P3 {P2.18/CLKOUT0}", "N5 {P2.19/CLKOUT1}", "P6 {P2.20/DYCS0}", "N9 {P2.21/DYCS1}", "NA", "NA", "P1 {P2.24/CKEOUT0}", "P2 {P2.25/CKEOUT1}", "NA", "NA", "M2 {P2.28/DQMOUT0}", "L1 {P2.29/DQMOUT1}", "NA", "NA"},
    // PORT 3 (0..31)
    { "D6 {P3.0/D0}", "E6 {P3.1/D1}", "A2 {P3.2/D2}", "G5 {P3.3/D3}", "D3 {P3.4/D4}", "E3 {P3.5/D5}", "F4 {P3.6/D6}", "G3 {P3.7/D7}", "A6 {P3.8/D8}", "A4 {P3.9/D9}", "B3 {P3.10/D10}", "B2 {P3.11/D11}", "A1 {P3.12/D12}", "C1 {P3.13/D13}", "F1 {P3.14/D14}", "G4 {P3.15/D15}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "M4 {P3.23/CAP0.0/PCAP1.0}", "N3 {P3.24/CAP0.1/PWM1.1}", "M3 {P3.25/MAT0.0/PWM1.2}", "K7 {P3.26/MAT0.1/PWM1.3}", "NA", "NA", "NA", "NA", "NA"},
    // PORT 4 (0..31)
    { "L6 {P4.0/A0}", "M7 {P4.1/A1}", "M8 {P4.2/A2}", "K9 {P4.3/A3}", "P13 {P4.4/A4}", "H10 {P4.5/A5}", "K10 {P4.6/A6}", "K12 {P4.7/A7}", "J11 {P4.8/A8}", "H12 {P4.9/A9}", "G12 {P4.10/A10}", "F11 {P4.11/A11}", "F10 {P4.12/A12}", "B14 {P4.13/A13}", "E8 {P4.14/A14}", "C10 {P4.15/A15}", "N12 {P4.16/A16}", "N13 {P4.17/A17}", "P14 {P4.18/A18}", "M14 {P4.19/A19}", "NA", "NA", "NA", "NA", "C8 {P4.24/OE}", "D9 {P4.25/WE}", "K13 {P4.26/BLS0}", "F14 {P4.27/BLS1}", "D10 {P4.28/BLS2/MAT2.0/TXD3}", "B9 {P4.29/BLS3/MAT2.1/RXD3}", "C7 {P4.30/CS0}", "E7 {P4.31/CS1}"}
#else
    // PORT 0 (0..31)
    { "94-U5 {P0.0/RD1/TXD3/SDA1}", "96-T14 {P0.1/TD1/RXD3/SCL1}", "202-C4 {P0.2/TXD0}", "204-D6 {P0.3/RXD0}", "168-B12 {P0.4/I2SRX_CLK/LCDVD[0]/RD2/CAP2.0}", "166-C12 {P0.5/I2SRX_WS/LCDVD[1]/TD2/CAP2.1}", "164-D13 {P0.6/I2SRX_SDA/LCDVD[8]/SSEL1/MAT2.0}", "162-C13 {P0.7/I2STX_CLK/LCDVD[9]/SCK1/MAT2.1}", "160-A15 {P0.8/I2STX_WS/LCDVD[16]/MISO1/MAT2.2}", "158-C14 {P0.9/I2STX_SDA/LCDVD[17]/MOSI1/MAT2.3}", "98-T15 {P0.10/TXD2/SDA2/MAT3.0}", "100-R14 {P0.11/RXD2/SCL2/MAT3.1}", "41-R1 {P0.12/USB_PPWR2/MISO1/AD0.6}", "45-R2 {P0.13/USB_UP_LED2/MOSI1/AD0.7}", "69-T7 {P0.14/USB_HSTEN2/USB_CONNECT2/SSEL1}", "128-J16 {P0.15/TXD1/SCK0/SCK}", "130-J14 {P0.16/RXD1/SSEL0/SSEL}", "126-K17 {P0.17/CTS1/MISO0/MISO}", "124-K15 {P0.18/DCD1/MOSI0/MOSI}", "122-L17 {P0.19/DSR1/MCICLK/SDA1}", "120-M17 {P0.20/DTR1/MCICMD/SCL1}", "118-M16 {P0.21/RI1/MCIPWR/RD1}", "116-N17 {P0.22/RTS1/MCIDAT0/TD1}", "18-H1 {P0.23/AD0.0/I2SRX_CLK/CAP3.0}", "16-G2 {P0.24/AD0.1/I2SRX_WS/CAP3.1}", "14-F1 {P0.25/AD0.2/I2SRX_SDA/TXD3}", "12-E1 {P0.26/AD0.3/AOUT/RXD3}", "50-T1 {P0.27/SDA0}", "48-R3 {P0.28/SCL0}", "61-U4 {P0.29/USB_D+1}", "62-R6 {P0.30/USB_D-1}", "51-T2 {P0.31/USB_D+2}"},
    // PORT 1 (0..31)
    { "196-A3 {P1.0/ENET_TXD0}", "194-B5 {P1.1/ENET_TXD1}", "185-D9 {P1.2/ENET_TXD2/MCICLK/PWM0.1}", "177-A10 {P1.3/ENET_TXD3/MCICMD/PWM0.2}", "192-A5 {P1.4/ENET_TX_EN}", "156-A17 {P1.5/ENET_TX_ER/MCIPWR/PWM0.3}", "171-B11 {P1.6/ENET_TX_CLK/MCIDAT0/PWM0.4}", "153-D14 {P1.7/ENET_COL/MCIDAT1/PWM0.5}", "190-C7 {P1.8/ENET_CRS_DV/ENET_CRS}", "188-A6 {P1.9/ENET_RXD0}", "186-C8 {P1.10/ENET_RXD1}", "163-A14 {P1.11/ENET_RXD2/MCIDAT2/PWM0.6}", "157-A16 {P1.12/ENET_RXD3/MCIDAT3/PCAP0.0}", "147-D16 {P1.13/ENET_RX_DV}", "184-A7 {P1.14/ENET_RX_ER}", "182-A8 {P1.15/ENET_REF_CLK/ENET_RX_CLK}", "180-D10 {P1.16/ENET_MDC}", "178-A9 {P1.17/ENET_MDIO}", "66-P7 {P1.18/USB_UP_LED1/PWM1.1/CAP1.0}", "68-U6 {P1.19/USB_TX_E1/USB_PPWR1/CAP1.1}", "70-U7 {P1.20/USB_TX_DP1/LCDVD[6]/LCDVD[10]/PWM1.2/SCK0}", "72-R8 {P1.21/USB_TX_DM1/LCDVD[7]/LCDVD[11]/PWM1.3/SSEL0}", "74-U8 {P1.22/USB_RCV1/LCDVD[8]/LCDVD[12]/USB_PWRD1/MAT1.0}", "76-P9 {P1.23/USB_RX_DP1/LCDVD[9]/LCDVD[13]/PWM1.4/MISO0}", "78-T9 {P1.24/USB_RX_DM1/LCDVD[10]/LCDVD[14]/PWM1.5/MOSI0}", "80-T10 {P1.25/USB_LS1/LCDVD[11]/LCDVD[15]/USB_HSTEN1/MAT1.1}", "82-R10 {P1.26/USB_SSPND1/LCDVD[12]/LCDVD[20]/PWM1.6/CAP0.0}", "88-T12 {P1.27/USB_INT1/LCDVD[13]/LCDVD[21]/USB_OVRCR1/CAP0.1}", "90-T13 {P1.28/USB_SCL1/LCDVD[14]/LCDVD[22]/PCAP1.0/MAT0.0}", "92-U14 {P1.29/USB_SDA1/LCDVD[15]/LCDVD[23]/PCAP1.1/MAT0.1}", "42-P2 {P1.30/USB_PWRD2/VBUS/AD0.4}", "40-P1 {P1.31/USB_OVRCR2/SCK1/AD0.5}"},
    // PORT 2 (0..31)
    { "154-B17 {P2.0/PWM1.1/TXD1/TRACECLK/LDCPWR}", "152-E14 {P2.1/PWM1.2/RXD1/PIPESTAT0/LCDLE}", "150-D15 {P2.2/PWM1.3/CTS1/PIPESTAT1/LCDDCLK}", "144-E16 {P2.3/PWM1.4/DCD1/PIPESTAT2/LCDFP}", "142-D17 {P2.4/PWM1.5/DSR1/TRACESYNC/LCDENAB/LCDM}", "140-F16 {P2.5/PWM1.6/DTR1/TRACEPKT0/LCDLP}", "138/E17 {P2.6/PCAP1.0/RI1/TRACEPKT1/LCDVD[0]/LCDVD[4]}", "136-G16 {P2.7/RD2/RTS1/TRACEPKT2/LCDVD[1]/LCDVD[5]}", "134-H15 {P2.8/TD2/TXD2/TRACEPKT3/LCDVD[2]/LCDVD[6]}", "132-H16 {P2.9/USB_CONNECT1/RXD2/EXTIN0/LCDVD[3]/LCDVD[7]}", "110-N15 {P2.10/EINT0}", "108-T17 {P2.11/EINT1/LCDCLKIN/MCIDAT1/I2STX_CLK}", "106-N14 {P2.12/EINT2/LCDVD[4]/LCDVD[3]/LCDVD[8]/LCDVD[18]/MCIDAT2/I2STX_WS}", "102-T16 {P2.13/EINT3/LCDVD[5]/LCDVD[9]/LCDVD[19]/MCIDAT3/I2STX_SDA}", "91-R12 {P2.14/CS2/CAP2.0/SDA1}", "99-P13 {P2.15/CS3/CAP2.1/SCL1}", "87-R11 {P2.16/CAS}", "95-R13 {P2.17/RAS}", "59-U3 {P2.18/CLKOUT0}", "67-R7 {P2.19/CLKOUT1}", "73-T8 {P2.20/DYCS0}", "81-U11 {P2.21/DYCS1}", "85-U12 {P2.22/DYCS2/CAP3.0/SCK0}", "64-U5 {P2.23/DYCS3/CAP3.1/SSEL0}", "53-P5 {P2.24/CKEOUT0}", "54-R4 {P2.25/CKEOUT1}", "57-T4 {P2.26/CKEOUT2/MAT3.0/MISO0}", "47-P3 {P2.27/CKEOUT3/MAT3.1/MOSI0}", "49-P4 {P2.28/DQMOUT0}", "43-N3 {P2.29/DQMOUT1}", "31-L4 {P2.30/DQMOUT2/MAT3.2/SDA2}", "39-N2 {P2.31/DQMOUT3/MAT3.3/SCL2}"},
    // PORT 3 (0..31)
    { "197-B4 {P3.0/D0}", "201-B3 {P3.1/D1}", "207-B1 {P3.2/D2}", "3-E4 {P3.3/D3}", "13-F2 {P3.4/D4}", "17-G1 {P3.5/D5}", "23-J1 {P3.6/D6}", "27-L1 {P3.7/D7}", "191-D8 {P3.8/D8}", "199-C5 {P3.9/D9}", "205-B2 {P3.10/D10}", "208-D5 {P3.11/D11}", "1-D4 {P3.12/D12}", "7-C1 {P3.13/D13}", "21-H2 {P3.14/D14}", "28-M1 {P3.15/D15}", "137-F17 {P3.16/D16/PWM0.1/TXD1}", "143-F15 {P3.17/D17/PWM0.2/RXD1}", "151-C15 {P3.18/D18/PWM0.3/CTS1}", "161-B14 {P3.19/D19/PWM0.4/DCD1}", "167-A13 {P3.20/D20/PWM0.5/DSR1}", "175-C10 {P3.21/D21/PWM0.6/DTR1}", "195-C6 {P3.22/D22/PCAP0.0/RI1}", "65-T6 {P3.23/D23/CAP0.0/PCAP1.0}", "58-R5 {P3.24/D24/CAP0.1/PWM1.1}", "56-U2 {P3.25/D25/MAT0.0/PWM1.2}", "55-T3 {P3.26/D26/MAT0.1/PWM1.3}", "203-A1 {P3.27/D27/CAP1.0/PWM1.4}", "5-D2 {P3.28/D28/CAP1.1/PWM1.5}", "11-F3 {P3.29/D29/MAT1.0/PWM1.6}", "19-H3 {P3.30/D30/MAT1.1/RTS1}", "25-J3 {P3.31/D31/MAT1.2}"},
    // PORT 4 (0..31)
    { "75-U9 {P4.0/A0}", "79-U10 {P4.1/A1}", "83-T11 {P4.2/A2}", "97-U16 {P4.3/A3}", "103-R15 {P4.4/A4}", "107-R16 {P4.5/A5}", "113-M14 {P4.6/A6}", "121-L16 {P4.7/A7}", "127-J17 {P4.8/A8}", "131-H17 {P4.9/A9}", "135-G17 {P4.10/A10}", "145-F14 {P4.11/A11}", "149-C16 {P4.12/A12}", "155-B16 {P4.13/A13}", "159-B15 {P4.14/A14}", "173-A11 {P4.15/A15}", "101-U17 {P4.16/A16}", "104-P14 {P4.17/A17}", "105-P15 {P4.18/A18}", "111-P16 {P4.19/A19}", "109-R17 {P4.20/A20/SDA2/SCK1}", "115-M15 {P4.21/A21/SCL2/SSEL1}", "123-K14 {P4.22/A22/TXD2/MISO1}", "129-J15 {P4.23/A23/RXD2/MOSI1}", "183-B8 {P4.24/OE}", "179-B9 {P4.25/WE}", "119-L15 {P4.26/BLS0}", "139-G15 {P4.27/BLS1}", "170-C11 {P4.28/BLS2/MAT2.0/LCDVD[6]/LCDVD[10]/LCDVD[2]/TXD3}", "176-B10 {P4.29/BLS3/MAT2.1/LCDVD[7]/LCDVD[11]/LCDVD[3]/RXD3}", "187-B7 {P4.30/CS0}", "193-A4 {P4.31/CS1}"}
#endif
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][5] = {
    {
        // GPIO Function 1,     Function 2,    Function3                 // PORT 0
        {  "Q", "RD1",         "TXD3",         "SDA1",    "-" },
        {  "Q", "TD1",         "RXD3",         "SCL1",    "-" },
        {  "Q", "TXD0",        "INVALID",      "INVALID", "-" },
        {  "Q", "RXD0",        "INVALID",      "INVALID", "-" },
        {  "Q", "I2SRX_CLK",   "RD2",          "CAP2.0",  "LCDVD[0]"  },
        {  "Q", "I2SRX_WS",    "TD2",          "CAP2.1",  "LCDVD[1]"  },
        {  "Q", "I2SRX_SDA",   "SSEL1",        "MAT2.0",  "LCDVD[8]"  },
        {  "Q", "I2STX_CLK",   "SCK1",         "MAT2.1",  "LCDVD[9]"  },
        {  "Q", "I2STX_WS",    "MISO1",        "MAT2.2",  "LCDVD[16]" },        
        {  "Q", "I2STX_SDA",   "MOSI1",        "MAT2.3",  "LCDVD[17]" },
        {  "Q", "TXD2",        "SDA2",         "MAT3.0",  "-" },
        {  "Q", "RXD2",        "SCL2",         "MAT3.1",  "-" },
        {  "Q", "USB_PPWR2",   "MISO1",        "AD0.6",   "-" },
        {  "Q", "USB_UP_LED2", "MOSI1",        "AD0.7",   "-" },
        {  "Q", "USB_HSTEN2",  "USB_CONNECT2", "SSEL1",   "-" },
        {  "Q", "TXD1",        "SCK0",         "SCK",     "-" },
        {  "Q", "RXD1",        "SSEL0",        "SSEL",    "-" },
        {  "Q", "CTS1",        "MISO0",        "MISO",    "-" },                 
        {  "Q", "DCD1",        "MOSI0",        "MOSI",    "-" },
        {  "Q", "DSR1",        "MCICLK",       "SDA1",    "-" },
        {  "Q", "DTR1",        "MCICMD",       "SCL1",    "-" },
        {  "Q", "RI1",         "MCIPWR",       "RD1",     "-" },
        {  "Q", "RTS1",        "MCIDAT0",      "TD1",     "-" },
        {  "Q", "AD0.0",       "I2SRX_CLK",    "CAP3.0",  "-" },
        {  "Q", "AD0.1",       "I2SRX_WS",     "CAP3.1",  "-" },
        {  "Q", "AD0.2",       "I2SRX_SDA",    "TXD3",    "-" },                 
        {  "Q", "AD0.3",       "AOUT",         "RXD3",    "-" },
        {  "Q", "SDA0",        "INVALID",      "INVALID", "-" },
        {  "Q", "SCL0",        "INVALID",      "INVALID", "-" },
        {  "Q", "USB_D+1",     "INVALID",      "INVALID", "-" },
        {  "Q", "USB_D-1",     "INVALID",      "INVALID", "-" },
        {  "Q", "USB_D+2",     "INVALID",      "INVALID", "-" }
    },
    {
        // GPIO Function 1,    Function 2,  Function3                    // PORT 1
        {  "Q", "ENET_TXD0",   "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_TXD1",   "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_TXD2",   "MCICLK",      "PWM0.1",  "-" },
        {  "Q", "ENET_TXD3",   "MCICMD",      "PWM0.2",  "-" },
        {  "Q", "ENET_TX_EN",  "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_TX_ER",  "MCIPWR",      "PWM0.3",  "-" },
        {  "Q", "ENET_TX_CLK", "MCIDAT0",     "PWM0.4",  "-" },
        {  "Q", "ENET_COL",    "MCIDAT1",     "PWM0.5",  "-" },
        {  "Q", "ENET_CRS_DV", "INVALID",     "INVALID", "ENET_CRS" },                 
        {  "Q", "ENET_RXD0",   "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_RXD1",   "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_RXD2",   "MCIDAT2",     "PWM0.6",  "-" },
        {  "Q", "ENET_RXD3",   "MCIDAT3",     "PCAP0.0", "-" },
        {  "Q", "ENET_RX_DV",  "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_RX_ER",  "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_REF_CLK","ENET_RX_CLK", "INVALID", "-" },
        {  "Q", "ENET_MDC",    "INVALID",     "INVALID", "-" },
        {  "Q", "ENET_MDIO",   "INVALID",     "INVALID", "-" },                 
        {  "Q", "USB_UP_LED1", "PWM1.1",      "CAP1.0",  "-" },
        {  "Q", "USB_TX_E1",   "USB_PPWR1",   "CAP1.1",  "-" },
        {  "Q", "USB_TX_DP1",  "PWM1.2",      "SCK0",    "LCDVD[6]/LCDVD[10]"  },
        {  "Q", "USB_TX_DM1",  "PWM1.3",      "SSEL0",   "LCDVD[7]/LCDVD[11]"  },
        {  "Q", "USB_RCV1",    "USB_PWRD1",   "MAT1.0",  "LCDVD[8]/LCDVD[12]"  },
        {  "Q", "USB_RX_DP1",  "PWM1.4",      "MISO0",   "LCDVD[9]/LCDVD[13]"  },
        {  "Q", "USB_RX_DM1",  "PWM1.5",      "MOSI0",   "LCDVD[10]/LCDVD[14]" },
        {  "Q", "USB_LS1",     "USB_HSTEN1",  "MAT1.0",  "LCDVD[11]/LCDVD[15]" },
        {  "Q", "USB_SSPND1",  "PWM1.6",      "CAP0.0",  "LCDVD[12]/LCDVD[20]" },          
        {  "Q", "USB_INT1",    "USB_OVRCR1",  "CAP0.1",  "LCDVD[13]/LCDVD[21]" },
        {  "Q", "USB_SCL1",    "PCAP1.0",     "MAT0.0",  "LCDVD[14]/LCDVD[22]" },
        {  "Q", "USB_SDA1",    "PCAP1.1",     "MAT0.1",  "LCDVD[15]/LCDVD[23]" },
        {  "Q", "USB_PWRD2",   "VBUS",        "AD0.4",   "-" },
        {  "Q", "USB_OVRCR2",  "SCK1",        "AD0.5",   "-" }
    },
    {
        // GPIO Function 1,     Function 2,  Function3    special        // PORT 2
        {  "Q", "PWM1.1",       "TXD1",      "TRACECLK",  "LDCPWR" },
        {  "Q", "PWM1.2",       "RXD1",      "PIPESTAT0", "LCDLE" },
        {  "Q", "PWM1.3",       "CTS1",      "PIPESTAT1", "LCDDCLK" },
        {  "Q", "PWM1.4",       "DCD1",      "PIPESTAT2", "LCDFP" },
        {  "Q", "PWM1.5",       "DSR1",      "TRACESYN",  "LCDENAB/LCDM" },
        {  "Q", "PWM1.6",       "DTR1",      "TRACEPKT0", "LCDLP" },
        {  "Q", "PCAP1.0",      "RI1",       "TRACEPKT1", "LCDVD[0]/LCDVD[4]" },
        {  "Q", "RD2",          "RTS1",      "TRACEPKT2", "LCDVD[1]/LCDVD[5]" },
        {  "Q", "TD2",          "TXD2",      "TRACEPKT3", "LCDVD[2]/LCDVD[6]" },                 
        {  "Q", "USB_CONNECT1", "RXD2",      "EXTIN0",    "LCDVD[3]/LCDVD[7]" },
        {  "Q", "EINT0",        "INVALID",   "INVALID",   "-" },
        {  "Q", "EINT1",        "MCIDAT1",   "I2STX_CLK", "LCDCLKIN" },
        {  "Q", "EINT2",        "MCIDAT2",   "I2STX_WS",  "LCDVD[4]/LCDVD[3]/LCDVD[8]/LCDVD[18]" },
        {  "Q", "EINT3",        "MCIDAT3",   "I2STX_SDA", "LCDVD[5]/LCDVD[9]/LCDVD[19]" },
        {  "Q", "CS2",          "CAP2.0",    "SDA1",      "-" },
        {  "Q", "CS3",          "CAP2.1",    "SCL1",      "-" },
        {  "Q", "CAS",          "INVALID",   "INVALID",   "-" },
        {  "Q", "RAS",          "INVALID",   "INVALID",   "-" },            
        {  "Q", "CLKOUT0",      "INVALID",   "INVALID",   "-" },
        {  "Q", "CLKOUT1",      "INVALID",   "INVALID",   "-" },
        {  "Q", "DYCS0",        "INVALID",   "INVALID",   "-" },
        {  "Q", "DYCS1",        "INVALID",   "INVALID",   "-" },
        {  "Q", "DYCS2",        "CAP3.0",    "SCK0",      "-" },
        {  "Q", "DYCS3",        "CAP3.1",    "SSEL0",     "-" },
        {  "Q", "CKEOUT0",      "INVALID",   "INVALID",   "-" },
        {  "Q", "CKEOUT1",      "INVALID",   "INVALID",   "-" },
        {  "Q", "CKEOUT2",      "MAT3.0",    "MISO0",     "-" },
        {  "Q", "CKEOUT3",      "MAT3.1",    "MOSI0",     "-" },                
        {  "Q", "DQMOUT0",      "INVALID",   "INVALID",   "-" },
        {  "Q", "DQMOUT1",      "INVALID",   "INVALID",   "-" },
        {  "Q", "DQMOUT2",      "MAT3.2",    "SDA2",      "-" },
        {  "Q", "DQMOUT3",      "MAT3.3",    "SCL2",      "-" },
    },
    {
        // GPIO Function 1,  Function 2,  Function3  special             // PORT 3
        {  "Q", "D0",        "INVALID",   "INVALID", "-" },
        {  "Q", "D1",        "INVALID",   "INVALID", "-" },
        {  "Q", "D2",        "INVALID",   "INVALID", "-" },
        {  "Q", "D3",        "INVALID",   "INVALID", "-" },
        {  "Q", "D4",        "INVALID",   "INVALID", "-" },
        {  "Q", "D5",        "INVALID",   "INVALID", "-" },
        {  "Q", "D6",        "INVALID",   "INVALID", "-" },
        {  "Q", "D7",        "INVALID",   "INVALID", "-" },
        {  "Q", "D8",        "INVALID",   "INVALID", "-" },
        {  "Q", "D9",        "INVALID",   "INVALID", "-" },
        {  "Q", "D10",       "INVALID",   "INVALID", "-" },
        {  "Q", "D11",       "INVALID",   "INVALID", "-" },
        {  "Q", "D12",       "INVALID",   "INVALID", "-" },
        {  "Q", "D13",       "INVALID",   "INVALID", "-" },
        {  "Q", "D14",       "INVALID",   "INVALID", "-" },
        {  "Q", "D15",       "INVALID",   "INVALID", "-" },
        {  "Q", "D16",       "PWM0.1",    "TXD1",    "-" },
        {  "Q", "D17",       "PWM0.2",    "RXD1",    "-" },
        {  "Q", "D18",       "PWM0.3",    "CTS1",    "-" },
        {  "Q", "D19",       "PWM0.4",    "DCD1",    "-" },
        {  "Q", "D20",       "PWM0.5",    "DSR1",    "-" },
        {  "Q", "D21",       "PWM0.6",    "DTR1",    "-" },
        {  "Q", "D22",       "PCAP0.0",   "RI1",     "-" },
        {  "Q", "D23",       "CAP0.0",    "PCAP1.0", "-" },
        {  "Q", "D24",       "CAP0.1",    "PWM1.1",  "-" },    
        {  "Q", "D25",       "MAT0.0",    "PWM1.2",  "-" },
        {  "Q", "D26",       "MAT0.1",    "PWM1.3",  "-" },
        {  "Q", "D27",       "CAP1.0",    "PWM1.4",  "-" },                 
        {  "Q", "D28",       "CAP1.1",    "PWM1.5",  "-" },
        {  "Q", "D29",       "MAT1.0",    "PWM1.6",  "-" },
        {  "Q", "D30",       "MAT1.1",    "RTS1",    "-" },
        {  "Q", "D31",       "MAT1.2",    "INVALID", "-" }
    },
    {
        // GPIO Function 1,  Function 2,  Function3  special             // PORT 4
        {  "Q", "A0",        "INVALID",   "INVALID", "-" },
        {  "Q", "A1",        "INVALID",   "INVALID", "-" },
        {  "Q", "A2",        "INVALID",   "INVALID", "-" },
        {  "Q", "A3",        "INVALID",   "INVALID", "-" },
        {  "Q", "A4",        "INVALID",   "INVALID", "-" },
        {  "Q", "A5",        "INVALID",   "INVALID", "-" },
        {  "Q", "A6",        "INVALID",   "INVALID", "-" },
        {  "Q", "A7",        "INVALID",   "INVALID", "-" },
        {  "Q", "A8",        "INVALID",   "INVALID", "-" },
        {  "Q", "A9",        "INVALID",   "INVALID", "-" },
        {  "Q", "A10",       "INVALID",   "INVALID", "-" },
        {  "Q", "A11",       "INVALID",   "INVALID", "-" },
        {  "Q", "A12",       "INVALID",   "INVALID", "-" },
        {  "Q", "A13",       "INVALID",   "INVALID", "-" },
        {  "Q", "A14",       "INVALID",   "INVALID", "-" },
        {  "Q", "A15",       "INVALID",   "INVALID", "-" },
        {  "Q", "A16",       "INVALID",   "INVALID", "-" },
        {  "Q", "A17",       "INVALID",   "INVALID", "-" },
        {  "Q", "A18",       "INVALID",   "INVALID", "-" },
        {  "Q", "A19",       "INVALID",   "INVALID", "-" },
        {  "Q", "A20",       "SDA2",      "SCK1",    "-" },
        {  "Q", "A21",       "SCL2",      "SSEL1",   "-" },
        {  "Q", "A22",       "TXD2",      "MISO1",   "-" },
        {  "Q", "A23",       "RXD2",      "MOSI1",   "-" },
        {  "Q", "OE",        "INVALID",   "INVALID", "-" },
        {  "Q", "WE",        "INVALID",   "INVALID", "-" },
        {  "Q", "BLS0",      "INVALID",   "INVALID", "-" },
        {  "Q", "BLS1",      "INVALID",   "INVALID", "-" },
        {  "Q", "BLS2",      "MAT2.0",    "TXD3",    "LCDVD[6]/LCDVD[10]/LCDVD[2]" },
        {  "Q", "BLS3",      "MAT2.1",    "RXD3",    "LCDVD[7]/LCDVD[11]/LCDVD[3]" },
        {  "Q", "CS0",       "INVALID",   "INVALID", "-" },
        {  "Q", "CS1",       "INVALID",   "INVALID", "-" }     
    }
};

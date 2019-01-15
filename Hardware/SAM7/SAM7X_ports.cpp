/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        SAM7X_ports.c
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2013
    *********************************************************************
    24.12.2008 Add ADC support                                           {1}
    17.08.2009 Corrected PA26/TF to PA26/RF
    13.02.2010 Add SAM7Sxxx                                              {2}
    12.10.2010 Add SAM7SExxx                                             {3}
    25.01.2012 Add SAM7L                                                 {4}

*/

#include "config.h"

#ifdef _HW_SAM7X

#define BUF1SIZE 100

#if _VC80_UPGRADE>=0x0600
    #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define STRCAT strcat
#endif

#if defined _HW_SAM7S                                                    // {2}
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..31)
    #ifdef CHIP_48_PIN
    { "36 {PA0/PWM0/TIOA0}", "35 {PA1/PWM1/TIOB0}", "32 {PA2/PWM2/SCK0}", "31 {PA3/TWD/NPCS3}", "28 {PA4/TWCK/TCLK0}", "27 {PA5/RXD0/NPCS3}", "26 {PA6/TXD0/PCK0}", "24 {PA7/RTS0/PWM3}", "23 {PA8/CTS0/ADTRG}", "22 {PA9/DRXD/NPCS1}", "21 {PA10/DTXD/NPCS2}", "20 {PA11/NPCS0/PWM0}", "19 {PA12/MISO/PWM1}", "17 {PA13/MOSI/PWM2}", "16 {PA14/SPCK/PMW3}", "15 {PA15/TF/TIOA1}", "14 {PA16/TK/TIOB1}", "9 {PA17/TD/PCK1/AD0}", "10 {PA18/RD/PCK2/AD1}", "11 {PA19/RK/FIQ/AD2}", "12 {PA20/RF/IRQ0/AD3}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "3 {AD4}", "4 {AD5}", "5 {AD6}", "6 {AD7}"},
    #else
    { "48 {PA0/PWM0/TIOA0}", "47 {PA1/PWM1/TIOB0}", "44 {PA2/PWM2/SCK0}", "43 {PA3/TWD/NPCS3}", "36 {PA4/TWCK/TCLK0}", "35 {PA5/RXD0/NPCS3}", "34 {PA6/TXD0/PCK0}", "32 {PA7/RTS0/PWM3}", "31 {PA8/CTS0/ADTRG}", "30 {PA9/DRXD/NPCS1}", "29 {PA10/DTXD/NPCS2}", "28 {PA11/NPCS0/PWM0}", "27 {PA12/MISO/PWM1}", "22 {PA13/MOSI/PWM2}", "21 {PA14/SPCK/PMW3}", "20 {PA15/TF/TIOA1}", "19 {PA16/TK/TIOB1}", "9 {PA17/TD/PCK1/AD0}", "10 {PA18/RD/PCK2/AD1}", "13 {PA19/RK/FIQ/AD2}", "16 {PA20/RF/IRQ0/AD3}", "11 {PA21/RXD1/PCK1}", "14 {PA22/TXD1/NPCS3}", "15 {PA23/SCK1/PWM0}", "23 {PA24/RTS1/PWM1}", "25 {PA25/CTS1/PWM2}", "26 {PA26/DCD1/TIOA2}", "37 {PA27/DTR1/TIOB2}", "38 {PA28/DSR1/TCLK1}", "41 {PA29/RI1/TCLK2}", "42 {PA30/IRQ1/NPCS2}", "52 {PA31/NPCS1/PCK2}"},
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "3 {AD4}", "4 {AD5}", "5 {AD6}", "6 {AD7}"},
    #endif
};
#elif defined _HW_SAM7SE                                                 // {3}
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..31)
    { "32/B11 {PA0/PWM0/A0/NBS0}", "31/B12 {PA1/PWM1/A1/NBS2}", "30/C11 {PA2/PWM2/A2}", "29/C12 {PA3/TWD/A3}", "28/B10 {PA4/TWCK/A4}", "27/C10 {PA5/RXD0/A5}", "26/C9 {PA6/TXD0/A6}", "25/D12 {PA7/RTS0/A7}", "24/D11 {PA8/CTS0/A8}", "20/E10 {PA9/DRXD/A9}", "19/E11 {PA10/DTXD/A10}", "18/D10 {PA11/NPCS0/A11}", "17/D9 {PA12/MISO/A12}", "16/E12 {PA13/MOSI/A13}", "15/D8 {PA14/SPCK/A14}", "14/F11 {PA15/TF/A15}", "13/F12 {PA16/TK/A16/BA0}", "12/L11 {PA17/TD/A17/BA1/AD0}", "11/K12 {PA18/RD/NBCS3/CFIOW/AD1}", "10/H10 {PA19/RK/NCS4/CFCS0/AD2}", "9/H11 {PA20/RF/NCS2/CFCS1/AD3}", "117/L7 {PA21/RXD1/NCS6/CFCE2}", "116/J6 {PA22/TXD1/NCS5/CFCE1}", "115/J5 {PA23/SCK1/NWR1/NBS1/CFIOR}", "114/L6 {PA24/RTS1/SDA10}", "113/M5 {PA25/CTS1/SDCKE}", "112/K6 {PA26/DCD1/NCS1/SDCS}", "111/K5 {PA27/DTR1/SDWE}", "110/J4 {PA28/DSR1/CAS}", "109/L5 {PA29/RI1/RAS}", "108/J3 {PA30/IRQ1/D30}", "107/M4 {PA31/NPCS1/D31}"},
    // PORT B (0..31)
    { "69/C1 {PB0/TIOA0/A0/NBS0}", "68/C2 {PB1/TIOB0/A1/NBS2}", "67/D3 {PB2/SCK0/A2}", "64/B2 {PB3/NPCS3/A3}", "63/B3 {PB4/TCLK0/A4}", "62/C3 {PB5/NPCS3/A5}", "61/B1 {PB6/PCK0/A6}", "60/A1 {PB7/PWM3/A7}", "59/A2 {PB8/ADTRG/A8}", "58/A3 {PB9/NPCS1/A9}", "57/B4 {PB10/NPCS2/A10}", "56/C4 {PB11/PWM0/A11}", "55/A4 {PB12/PWM1/A12}", "54/A5 {PB13/PWM2/A13}", "53/B5 {PB14/PWM3/A14}", "52/C5 {PB15/TIOA1/A15}", "51/A6 {PB16/TIOB1/A16/BA0}", "50/D6 {PB17/PCK1/A17/BA1}", "49/B6 {PB18/PCK2/D16}", "48/C6 {PB19/FIQ/D17}", "44/B7 {PB20/IRQ0/D18}", "43/C7 {PB21/PCK1/D19}", "42/A7 {PB22/NPCS3/D20}", "41/A8 {PB23/PWM0/D21}", "40/B8 {PB24/PWM1/D22}", "39/A9 {PB25/PWM2/D23}", "38/D7 {PB26/TIOA2/D24}", "37/C8 {PB27/TIOB2/D25}", "36/B9 {PB28/TCLK1/D26}", "35/A10 {PB29/TCLK2/D27}", "34/A11 {PB30/NPCS2/D28}", "33/A12 {PB31/PCK2/D29}"},
    // PORT C (0..23)
    { "106/K4 {PC0/D0}", "105/L4 {PC1/D1}", "104/K3 {PC2/D2}", "103/M3 {PC3/D3}", "102/L3 {PC4/D4}", "101/M2 {PC5/D5}", "100/K2 {PC6/D6}", "99/L2 {PC7/D7}", "98/M1 {PC8/D8/RTS1}", "93/G4 {PC9/D9/DTR1}", "92/J2 {PC10/D10/PCK0}", "91/K1 {PC11/D11/PCK1}", "90/J1 {PC12/D12/PCK2}", "89/H3 {PC13/D13}", "88/H2 {PC14/D14/NPCS1}", "87/H1 {PC15/D15/NCS3/NANDCS}", "86/G2 {PC16/A18/NWAIT}", "85/G3 {PC17/A19/NANDOE}", "84/G1 {PC18/A20/NANDWE}", "83/F3 {PC19/A21/NANDALE}", "82/F2 {PC20/A22/REG/NANDCLE/NCS7}", "81/F1 {PC21/NWR0/NWE/CFWE}", "80/E1 {PC22/NRD/CFOE}", "79/E2 {PC23/CFRNW/NCS0}", "NA","NA","NA","NA","NA","NA","NA","NA",},
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "6/G10 {AD4}", "5/F10 {AD5}", "4/J7 {AD6}", "3/J8 {AD7}"}, // {1}
};
#elif defined _HW_SAM7L                                                  // {4}
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..31)
    { "3/B2 {PA0/COM0}", "4/B1 {PA1/COM1}", "5/C3 {PA2/COM2}", "6/C1 {PA3/COM3}", "7/C2 {PA4/COM4}", "8/D2 {PA5/COM5}", "9/D1 {PA6/SEG0}", "10/D3 {PA7/SEG1}", "11/E5 {PA8/SEG2}", "12/E3 {PA9/SEG3}", "13/E2 {PA10/SEG4}", "16/E4 {PA11/SEG5}", "17/F4 {PA12/SEG6}", "18/F2 {PA13/SEG7}", "19/F3 {PA14/SEG8}", "20/G4 {PA15/SEG9}", "21/G3 {PA16/SEG10}", "22/G2 {PA17/SEG11}", "23/A4 {PA18/SEG12}", "24/H3 {PA19/SEG13}", "25/J5 {PA20/SEG14}", "26/J4 {PA21/SEG15}", "27/H2 {PA22/SEG16}", "29/J2 {PA23/SEG17}", "30/J3 {PA24/SEG18}", "31/K4 {PA25/SEG19}", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT B (0..31)
    { "41/M2 {PB0/SEG20}", "42/M3 {PB1/SEG21}", "43/M4 {PB2/SEG22}", "44/K5 {PB3/SEG23}", "45/L4 {PB4/SEG24}", "46/L5 {PB5/SEG25}", "47/M5 {PB6/SEG26}", "48/M6 {PB7/SEG27}", "49/J6 {PB8/SEG28}", "50/J7 {PB9/SEG29}", "51/K6 {PB10/SEG30}", "52/L6 {PB11/SEG31}", "53/L7 {PB12/NPCS3/SEG32}", "54/K7 {PB13/NPCS2/SEG33}", "57/J8 {PB14/NPCS1/SEG34}", "58/K8 {PB15/RTS1/SEG35}", "59/M8 {PB16/RTS0/SEG36}", "60/L8 {PB17/DTR1/SEG37}", "61/M9 {PB18/PWM0/SEG38}", "63/L9 {PB19/PWM1/SEG39}", "64/K9 {PB20/PWM2/COM6}", "65/M10 {PB21/PWM3/COM7}", "66/L10 {PB22/NPCS1/PCK1/COM8}", "67/L11 {PB23/PCK0/NPCS3/COM9}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..23)
    { "87/E10 {PC0/CTS1/PWM2/PGMEN0/WKUP0}", "88/F11 {PC1/DCD1/TIOA2/PGMEN1/WKUP1}", "89/E11 {PC2/DTR1/TIOB2/PGMEN2/WKUP2}", "90/D12 {PC3/DSR1/TCLK1/PGMNCMD/WKUP3}", "91/D11 {PC4/RI1/TCLK2/PGMRDY/WKUP4}", "92/E9 {PC5/IRQ1/NPCS2/PGMNOE/WKUP5}", "93/D9 {PC6/NPCS1/PCK2/PGMNVALID/WKUP6}", "94/D10 {PC7/PWM0/TIOA0/PGMMO/High drive}", "95/C12 {PC8/PWM1/TIOB0/PGMM1/High drive}", "96/C10 {PC9/PWM2/SCK0/PGMM2/High drive}", "97/C11 {PC10/TWD/NPCS3/PGMM3/High drive}", "98/B12 {PC11/TWCK/TCLK0/PGMD0/WKUP7}", "99/A12 {PC12/RXD0/NPCS3/PGMD1/WKUP8}", "101/B11 {PC13/TXD0/PCK0/PGMD2/WKUP9}", "102/A11 {PC14/RTS0/ADTRG/PGMD3/WKUP10}", "103/B10 {PC15/CTS0/PWM3/PGMD4/WKUP11}", "104/C9 {PC16/DRXD/NPCS1/PGMD5}", "105/A9 {PC17/DTXD/NPCS2/PGMD6}", "106/D8 {PC18/NPCS0/PWM0/PGMD7}", "107/C8 {PC19/MISO/PWM1/PGMD8}", "108/B9 {PC20/MOSI/PWM2/PGMD9}", "109/D7 {PC21/SPCK/PWM3/PGMD10}", "110/C7 {PC22/NPCS3/TIOA1/PGMD11}", "111/A8 {PC23/PCK0/TIOB1/PGMD12}", "112/A7 {PC24/RXD1/PCK1/PGMD13}","113/D6 {PC25/TXD1/PCK2/PGMD14}","114/B5 {PC26/RTS1/FIQ/PGMD15/WKUP12}","115/B6 {PC27/NPCS2/IRQ0/WKUP13}","77/H11 {PC28/SCK1/PWM0/WKUP14}","78/H9 {PC29/RTS1/PWM1/WKUP15}","NA","NA",},
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "6/G10 {AD4}", "5/F10 {AD5}", "4/J7 {AD6}", "3/J8 {AD7}"}, // {1}
};
#else                                                                    // SAM7X
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // PORT A (0..30)
    { "81 {PA0/RXD0}", "82 {PA1/TXD0}", "86 {PA2/SCK0/SPI1_NPCS1}", "85 {PA3/RTS0/SPI1_NPCS2}", "88 {PA4/CTS0/SPI1_NPCS3}", "89 {PA5/RXD1}", "90 {PA6/TXD1}", "91 {PA7/SCK1/SPI0_NPCS1}", "13 {PA8/RTS1/SPI0_NPCS2}", "14 {PA9/CTS1/SPI0_NPCS3}", "18 {PA10/TWD}", "19 {PA11/TWCK}", "20 {PA12/SPI0_NPCS0}", "21 {PA13/SPI0_NPC1/PCK1}", "22 {PA14/SPI0_NPC2/IRQ1}", "23 {PA15/SPI0_NPC3/TCLK2}", "24 {PA16/SPI0_MISO}", "25 {PA17/SPI0_MOSI}", "26 {PA18/SPI0_SPCK}", "46 {PA19/CANRX}", "47 {PA20/CANTX}", "49 {PA21/TF/SPI1_NPCS0}", "50 {PA22/TK/SPI1_SPCK}", "55 {PA23/TD/SPI1_MOSI}", "56 {PA24/RD/SPI1_MISO}", "59 {PA25/RK/SPI1_NPCS1}", "60 {PA26/RF/SPI1_NPCS2}", "73 {PA27/DRXD/PCK3}", "74 {PA28/DTXD}", "75 {PA29/FIQ/SPI1_NPCS3}", "80 {PA30/IRQ0/PCK2}"},
    // PORT B (0..30)
    { "40 {PB0/ETXCK-EREFCK/PCK0}", "41 {PB1/ETXEN}", "42 {PB2/ETX0}", "43 {PB3/ETX1}", "54 {PB4/ECRX}", "34 {PB5/ERX0}", "31 {PB6/ERX1}", "38 {PB7/ERXER}", "28 {PB8/EMDC}", "27 {PB9/EMDIO}", "44 {PB10/ETX2/SPI1_NPCS1}", "45 {PB11/ETX3/SPI1_NPCS2}", "39 {PB12/ETXER/TCLK0}", "30 {PB13/ERX2/SPI0_NPCS1}", "29 {PB14/ERX3/SPI0_NPCS2}", "35 {PB15/ERXDV-ECRSDV}", "53 {PB16/ECOL/SPI1_NPCS3}", "36 {PB17/ERXCK/SPI0_NPCS3}", "63 {PB18/EF100/ADTRG}", "64 {PB19/PWM0/TCLK1}", "65 {PB20/PWM1/PCK0}", "66 {PB21/PWM2/PCK1}", "67 {PB22/PWM3/PCK2}", "69 {PB23/TIOA0/DCD1}", "70 {PB24/TIOB0/DSR1}", "71 {PB25/TIOA1/DTR1}", "72 {PB26/TIOB1/RI1}", "9 {PB27/TIOA2/PWM0/AD0}", "10 {PB28/TIOB2/PWM1/AD1}", "11 {PB29/PCK1/PWM2/AD2}", "12 {PB30/PCK2/PWM3/AD3}"},
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "3 {AD4}", "4 {AD5}", "5 {AD6}", "6 {AD7}"}, // {1}
};
#endif

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
#if defined _HW_SAM7S                                                    // {2}
        //  Q    Periph. A   Periph. B    special
        {  "Q", "PWM0",      "TIOA0",   "INVALID"  },                    // PORT A
        {  "Q", "PWM1",      "TIOB0",   "INVALID"  },
        {  "Q", "PWM2",      "SCK0",    "INVALID"  },
        {  "Q", "TWD",       "NPCS3",   "INVALID"  },
        {  "Q", "TWCK",      "TCLK0",   "INVALID"  },
        {  "Q", "RXD0",      "NPCS3",   "INVALID"  },
        {  "Q", "TXD0",      "PCK0",    "INVALID"  },
        {  "Q", "RTS0",      "PWM3",    "INVALID"  },
        {  "Q", "CTS0",      "ADTRG",   "INVALID"  },                 
        {  "Q", "DRXD",      "NPCS1",   "INVALID"  },
        {  "Q", "DTXD",      "NPCS2",   "INVALID"  },
        {  "Q", "NPCS0",     "PWM0",    "INVALID"  },
        {  "Q", "MISO",      "PWM1",    "INVALID"  },
        {  "Q", "MOSI",      "PWM2",    "INVALID"  },
        {  "Q", "SPCK",      "PMW3",    "INVALID"  },
        {  "Q", "TF",        "TIOA1",   "INVALID"  },
        {  "Q", "TK",        "TIOB1",   "INVALID"  },
        {  "Q", "TD",        "PCK1",    "AD0"      },                 
        {  "Q", "RD",        "PCK2",    "AD1"      },
        {  "Q", "RK",        "FIQ",     "AD2"      },
        {  "Q", "RF",        "IRQ0",    "AD3"      },
        {  "Q", "RXD1",      "PCK1",    "INVALID"  },
        {  "Q", "TXD1",      "NPCS3",   "INVALID"  },
        {  "Q", "SCK1",      "PWM0",    "INVALID"  },
        {  "Q", "RTS1",      "PWM1",    "INVALID"  },
        {  "Q", "CTS1",      "PWM2",    "INVALID"  },
        {  "Q", "DCD1",      "TIOA2",   "INVALID"  },                 
        {  "Q", "DTR1",      "TIOB2",   "INVALID"  },
        {  "Q", "DSR1",      "TCLK1",   "INVALID"  },
        {  "Q", "RI1",       "TCLK2",   "INVALID"  },
        {  "Q", "IRQ1",      "NPCS2",   "INVALID"  },
        {  "Q", "NPCS1",     "PCK2",    "INVALID"  },
#elif defined _HW_SAM7SE                                                 // {3} - to modify
        //  Q    Periph. A   Periph. B    special
        {  "Q", "PWM0",      "A0/NBS0", "INVALID"  },                    // PORT A
        {  "Q", "PWM1",      "A1/NBS2", "INVALID"  },
        {  "Q", "PWM2",      "A2",      "INVALID"  },
        {  "Q", "TWD",       "A3",      "INVALID"  },
        {  "Q", "TWCK",      "A4",      "INVALID"  },
        {  "Q", "RXD0",      "A5",      "INVALID"  },
        {  "Q", "TXD0",      "A6",      "INVALID"  },
        {  "Q", "RTS0",      "A7",      "INVALID"  },
        {  "Q", "CTS0",      "A8",      "INVALID"  },                 
        {  "Q", "DRXD",      "A9",      "INVALID"  },
        {  "Q", "DTXD",      "A10",     "INVALID"  },
        {  "Q", "NPCS0",     "A11",     "INVALID"  },
        {  "Q", "MISO",      "A12",     "INVALID"  },
        {  "Q", "MOSI",      "A13",     "INVALID"  },
        {  "Q", "SPCK",      "A14",     "INVALID"  },
        {  "Q", "TF",        "A15",     "INVALID"  },
        {  "Q", "TK",        "A16/BA0", "INVALID"  },
        {  "Q", "TD",        "A17/BA1", "AD0"      },                 
        {  "Q", "RD",        "NBCS3/CFIOW","AD1"   },
        {  "Q", "RK",        "NCS4/CFCS0","AD2"    },
        {  "Q", "RF",        "NCS2/CFCS1","AD3"    },
        {  "Q", "RXD1",      "NCS6/CFCE2","INVALID"  },
        {  "Q", "TXD1",      "NCS5/CFCE1","INVALID"  },
        {  "Q", "SCK1",      "NWR1/NBS1/CFIOR","INVALID"  },
        {  "Q", "RTS1",      "SDA10",   "INVALID"  },
        {  "Q", "CTS1",      "SDCKE",   "INVALID"  },
        {  "Q", "DCD1",      "NCS1/SDCS","INVALID"  },                 
        {  "Q", "DTR1",      "SDWE",    "INVALID"  },
        {  "Q", "DSR1",      "CAS",     "INVALID"  },
        {  "Q", "RI1",       "RAS",     "INVALID"  },
        {  "Q", "IRQ1",      "D30",     "INVALID"  },
        {  "Q", "NPCS1",     "D31",     "INVALID"  },
#elif defined _HW_SAM7L                                                  // {4}
        //  Q    Periph. A   Periph. B    special
        {  "Q", "INVALID",   "INVALID",   "COM0"     },                  // PORT A
        {  "Q", "INVALID",   "INVALID",   "COM1"     },
        {  "Q", "INVALID",   "INVALID",   "COM2"     },
        {  "Q", "INVALID",   "INVALID",   "COM3"     },
        {  "Q", "INVALID",   "INVALID",   "COM4"     },
        {  "Q", "INVALID",   "INVALID",   "COM5"     },
        {  "Q", "INVALID",   "INVALID",   "SEG0"     },
        {  "Q", "INVALID",   "INVALID",   "SEG1"     },
        {  "Q", "INVALID",   "INVALID",   "SEG2"     },                 
        {  "Q", "INVALID",   "INVALID",   "SEG3"     },
        {  "Q", "INVALID",   "INVALID",   "SEG4"     },
        {  "Q", "INVALID",   "INVALID",   "SEG5"     },
        {  "Q", "INVALID",   "INVALID",   "SEG6"     },
        {  "Q", "INVALID",   "INVALID",   "SEG7"     },
        {  "Q", "INVALID",   "INVALID",   "SEG8"     },
        {  "Q", "INVALID",   "INVALID",   "SEG9"     },
        {  "Q", "INVALID",   "INVALID",   "SEG10"    },
        {  "Q", "INVALID",   "INVALID",   "SEG11"    },                 
        {  "Q", "INVALID",   "INVALID",   "SEG12"    },
        {  "Q", "INVALID",   "INVALID",   "SEG13"    },
        {  "Q", "INVALID",   "INVALID",   "SEG14"    },
        {  "Q", "INVALID",   "INVALID",   "SEG15"    },
        {  "Q", "INVALID",   "INVALID",   "SEG16"    },
        {  "Q", "INVALID",   "INVALID",   "SEG17"    },
        {  "Q", "INVALID",   "INVALID",   "SEG18"    },
        {  "Q", "INVALID",   "INVALID",   "SEG19"    },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  }
#else                                                                    // SAM7X
        //  Q    Periph. A   Periph. B    special
        {  "Q", "RXD0",      "INVALID",   "INVALID"  },                  // PORT A
        {  "Q", "TXD0",      "INVALID",   "INVALID"  },
        {  "Q", "SCK0",      "SPI1_NPCS1","INVALID"  },
        {  "Q", "RTS0",      "SPI1_NPCS2","INVALID"  },
        {  "Q", "CTS0",      "SPI1_NPCS3","INVALID"  },
        {  "Q", "RXD1",      "INVALID",   "INVALID"  },
        {  "Q", "TXD1",      "INVALID",   "INVALID"  },
        {  "Q", "SCK1",      "SPI0_NPCS1","INVALID"  },
        {  "Q", "RTS1",      "SPI0_NPCS2","INVALID"  },                 
        {  "Q", "CTS1",      "SPI0_NPCS3","INVALID"  },
        {  "Q", "TWD",       "INVALID",   "INVALID"  },
        {  "Q", "TWCK",      "INVALID",   "INVALID"  },
        {  "Q", "SPI0_NPCS0","INVALID",   "INVALID"  },
        {  "Q", "QSPI_CS1",  "PCK1",      "INVALID"  },
        {  "Q", "QSPI_CS2",  "IRQ1",      "INVALID"  },
        {  "Q", "QSPI_CS3",  "TCLK2",     "INVALID"  },
        {  "Q", "SPI0_MISO", "INVALID",   "INVALID"  },
        {  "Q", "SPI0_MOSI", "INVALID",   "INVALID"  },                 
        {  "Q", "SPI0_SPCK", "INVALID",   "INVALID"  },
        {  "Q", "CANRX",     "INVALID",   "INVALID"  },
        {  "Q", "CANTX",     "INVALID",   "INVALID"  },
        {  "Q", "TF",        "SPI1_NPCS0","INVALID"  },
        {  "Q", "TK",        "SPI1_SPCK", "INVALID"  },
        {  "Q", "TD",        "SPI1_MOSI", "INVALID"  },
        {  "Q", "RD",        "SPI1_MISO", "INVALID"  },
        {  "Q", "RK",        "SPI1_NPCS1","INVALID"  },
        {  "Q", "RF",        "SPI1_NPCS2","INVALID"  },                 
        {  "Q", "DRXD",      "PCK3",      "INVALID"  },
        {  "Q", "DTXD",      "INVALID",   "INVALID"  },
        {  "Q", "FIQ",       "SPI1_NPCS3","INVALID"  },
        {  "Q", "IRQ0",      "PCK2",      "INVALID"  }
#endif
    },
#ifndef _HW_SAM7S                                                        // {2}
    #if defined _HW_SAM7SE                                               // {3} - to modify
    {
        //  Q    Periph. A   Periph. B    special
        {  "Q", "TIOA0",     "A0/NBS0",   "INVALID"  },                  // PORT B
        {  "Q", "TIOB0",     "A1/NBS2",   "INVALID"  },
        {  "Q", "SCK0",      "A2",        "INVALID"  },
        {  "Q", "NPCS3",     "A3",        "INVALID"  },
        {  "Q", "TCLK0",     "A4",        "INVALID"  },
        {  "Q", "NPCS3",     "A5",        "INVALID"  },
        {  "Q", "PCK0",      "A6",        "INVALID"  },
        {  "Q", "PWM3",      "A7",        "INVALID"  },
        {  "Q", "ADTRG",     "A8",        "INVALID"  },                 
        {  "Q", "NPCS1",     "A9",        "INVALID"  },
        {  "Q", "NPCS2",     "A10",       "INVALID"  },
        {  "Q", "PWM0",      "A11",       "INVALID"  },
        {  "Q", "PWM1",      "A12",       "INVALID"  },
        {  "Q", "PWM2",      "A13",       "INVALID"  },
        {  "Q", "PWM3",      "A14",       "INVALID"  },
        {  "Q", "TIOA1",     "A15",       "INVALID"  },
        {  "Q", "TIOB1",     "A16/BA0",   "INVALID"  },
        {  "Q", "PCK1",      "A17/BA1",   "INVALID"  },                 
        {  "Q", "PCK2",      "D16",       "INVALID"  },
        {  "Q", "FIQ",       "D17",       "INVALID"  },
        {  "Q", "IRQ0",      "D18",       "INVALID"  },
        {  "Q", "PCK1",      "D19",       "INVALID"  },
        {  "Q", "NPCS3",     "D20",       "INVALID"  },
        {  "Q", "PWM0",      "D21",       "INVALID"  },
        {  "Q", "PWM1",      "D22",       "INVALID"  },
        {  "Q", "PWM2",      "D23",       "INVALID"  },
        {  "Q", "TIOA2",     "D24",       "INVALID"  },                 
        {  "Q", "TIOB2",     "D25",       "INVALID"  },
        {  "Q", "TCLK1",     "D26",       "INVALID"  },
        {  "Q", "TCLK2",     "D27",       "INVALID"  },
        {  "Q", "NPCS2",     "D28",       "INVALID"  },
        {  "Q", "PCK2",      "D29",       "INVALID"  },
    },
    #elif defined _HW_SAM7L                                              // {4}
    {
        //  Q    Periph. A   Periph. B    special
        {  "Q", "INVALID",   "INVALID",   "SEG20"    },                  // PORT B
        {  "Q", "INVALID",   "INVALID",   "SEG21"    },
        {  "Q", "INVALID",   "INVALID",   "SEG22"    },
        {  "Q", "INVALID",   "INVALID",   "SEG23"    },
        {  "Q", "INVALID",   "INVALID",   "SEG24"    },
        {  "Q", "INVALID",   "INVALID",   "SEG25"    },
        {  "Q", "INVALID",   "INVALID",   "SEG26"    },
        {  "Q", "INVALID",   "INVALID",   "SEG27"    },
        {  "Q", "INVALID",   "INVALID",   "SEG28"    },                 
        {  "Q", "INVALID",   "INVALID",   "SEG29"    },
        {  "Q", "INVALID",   "INVALID",   "SEG30"    },
        {  "Q", "INVALID",   "INVALID",   "SEG31"    },
        {  "Q", "NPCS3",     "INVALID",   "SEG32"    },
        {  "Q", "NPCS2",     "INVALID",   "SEG33"    },
        {  "Q", "NPCS1",     "INVALID",   "SEG34"    },
        {  "Q", "RTS1",      "INVALID",   "SEG35"    },
        {  "Q", "RTS0",      "INVALID",   "SEG36"    },
        {  "Q", "DTR1",      "INVALID",   "SEG37"    },                 
        {  "Q", "PWM0",      "INVALID",   "SEG38"    },
        {  "Q", "PWM1",      "INVALID",   "SEG39"    },
        {  "Q", "PWM2",      "INVALID",   "COM6"     },
        {  "Q", "PWM3",      "INVALID",   "COM7"     },
        {  "Q", "NPCS1",     "PCK1",      "COM8"     },
        {  "Q", "PCK0",      "NPCS3",     "COM9"     },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  }
    },
    #else                                                                // SAM7X
    {
        //  Q    Periph. A   Periph. B    special
        {  "Q", "ETXCK/EREFCK", "PCK0",   "INVALID"  },                  // PORT B
        {  "Q", "ETXEN",     "INVALID",   "INVALID"  },
        {  "Q", "ETX0",      "INVALID",   "INVALID"  },
        {  "Q", "ETX1",      "INVALID",   "INVALID"  },
        {  "Q", "ECRS",      "INVALID",   "INVALID"  },
        {  "Q", "ERX0",      "INVALID",   "INVALID"  },
        {  "Q", "ERX1",      "INVALID",   "INVALID"  },
        {  "Q", "ERXER",     "INVALID",   "INVALID"  },
        {  "Q", "EMDC",      "INVALID",   "INVALID"  },                 
        {  "Q", "EMDIO",     "INVALID",   "INVALID"  },
        {  "Q", "ETX2",      "SPI1_NCS1", "INVALID"  },
        {  "Q", "ETX3",      "SPI1_NCS2", "INVALID"  },
        {  "Q", "ETXER",     "TCLK0",     "INVALID"  },
        {  "Q", "ERX2",      "SPI0_NCS1", "INVALID"  },
        {  "Q", "ERX3",      "SPI0_NCS2", "INVALID"  },
        {  "Q", "ERXDV/ECRSDV","INVALID", "INVALID"  },
        {  "Q", "ECOL",      "SPI1_NCS3", "INVALID"  },
        {  "Q", "ERXCK",     "SPI0_NCS3", "INVALID"  },                 
        {  "Q", "EF100",     "ADTRG",     "INVALID"  },
        {  "Q", "PWM0",      "TCLK1",     "INVALID"  },
        {  "Q", "PWM1",      "PCK0",      "INVALID"  },
        {  "Q", "PWM2",      "PCK1",      "INVALID"  },
        {  "Q", "PWM3",      "PCK2",      "INVALID"  },
        {  "Q", "TIOA0",     "DCD1",      "INVALID"  },
        {  "Q", "TIOB0",     "DSR1",      "INVALID"  },
        {  "Q", "TIOA1",     "DTR1",      "INVALID"  },
        {  "Q", "TIOB1",     "RI1",       "INVALID"  },                 
        {  "Q", "TIOA2",     "PWM0",      "AD0"      },
        {  "Q", "TIOB2",     "PWM1",      "AD1"      },
        {  "Q", "PCK1",      "PWM2",      "AD2"      },
        {  "Q", "PCK2",      "PWM3",      "AD3"      }
    },
    #endif
#endif
#if defined _HW_SAM7SE                                                   // {3} - to modify
    {
        //  Q    Periph. A   Periph. B    special
        {  "Q", "D0",        "INVALID",   "INVALID"  },                  // PORT C
        {  "Q", "D1",        "INVALID",   "INVALID"  },
        {  "Q", "D2",        "INVALID",   "INVALID"  },
        {  "Q", "D3",        "INVALID",   "INVALID"  },
        {  "Q", "D4",        "INVALID",   "INVALID"  },
        {  "Q", "D5",        "INVALID",   "INVALID"  },
        {  "Q", "D6",        "INVALID",   "INVALID"  },
        {  "Q", "D7",        "INVALID",   "INVALID"  },
        {  "Q", "D8",        "RTS1",      "INVALID"  },                 
        {  "Q", "D9",        "DTR1",      "INVALID"  },
        {  "Q", "D10",       "PCK0",      "INVALID"  },
        {  "Q", "D11",       "PCK1",      "INVALID"  },
        {  "Q", "D12",       "PCK2",      "INVALID"  },
        {  "Q", "D13",       "INVALID",   "INVALID"  },
        {  "Q", "D14",       "NPCS1",     "INVALID"  },
        {  "Q", "D15"        "NCS3/NANDCS","INVALID" },
        {  "Q", "A18",       "NWAIT",     "INVALID"  },
        {  "Q", "A19",       "NANDOE",    "INVALID"  },                 
        {  "Q", "A20",       "NANDWE",    "INVALID"  },
        {  "Q", "A21/NANDALE","INVALID",  "INVALID"  },
        {  "Q", "A22/REG/NANDCLE","NCS7", "INVALID"  },
        {  "Q", "INVALID",   "NWR0/NWE/CFWE","INVALID"},
        {  "Q", "INVALID",   "NRD/CFOE",  "INVALID"  },
        {  "Q", "CFRNW",     "NCS0",      "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                 
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
        {  "Q", "INVALID",   "INVALID",   "INVALID"  }
    },
#elif defined _HW_SAM7L                                                  // {4}
    {
        //  Q    Periph. A   Periph. B    special
        {  "Q", "CTS1",      "PWM2",      "PGMEN0/WKUP0"  },             // PORT C
        {  "Q", "DCD1",      "TIOA2",     "PGMEN1/WKUP1"  },
        {  "Q", "DTR1",      "TIOB2",     "PGMEN2/WKUP2"  },
        {  "Q", "DSR1",      "TCLK1",     "PGMNCMD/WKUP3" },
        {  "Q", "RI1",       "TCLK2",     "PGMRDY/WKUP4"  },
        {  "Q", "IRQ1",      "NPCS2",     "PGMNOE/WKUP5"  },
        {  "Q", "NPCS1",     "PCK2",      "PGMNVALID/WKUP6" },
        {  "Q", "PWM0",      "TIOA0",     "PGMMO/High drive"},
        {  "Q", "PWM1",      "TIOB0",     "PGMM1/High drive"},
        {  "Q", "PWM2",      "SCK0",      "PGMM2/High drive"},
        {  "Q", "TWD",       "NPCS3",     "PGMM3/High drive"},
        {  "Q", "TWCK",      "TCLK0",     "PGMD0/WKUP7"   },
        {  "Q", "RXD0",      "NPCS3",     "PGMD1/WKUP8"   },
        {  "Q", "TXD0",      "PCK0",      "PGMD2/WKUP9"   },
        {  "Q", "RTS0",      "ADTRG",     "PGMD3/WKUP10"  },
        {  "Q", "CTS0"       "PWM3",      "PGMD4/WKUP11"  },
        {  "Q", "DRXD",      "NPCS1",     "PGMD5"         },
        {  "Q", "DTXD",      "NPCS2",     "PGMD6"         },
        {  "Q", "NPCS0",     "PWM0",      "PGMD7"         },
        {  "Q", "MISO",      "PWM1",      "PGMD8"         },
        {  "Q", "MOSI",      "PWM2",      "PGMD9"         },
        {  "Q", "SPCK",      "PWM3",      "PGMD10"        },
        {  "Q", "NPCS3",     "TIOA1",     "PGMD11"        },
        {  "Q", "PCK0",      "TIOB1",     "PGMD12"        },
        {  "Q", "RXD1",      "PCK1",      "PGMD13"        },
        {  "Q", "TXD1",      "PCK2",      "PGMD14"        },
        {  "Q", "RTS0",      "FIQ",       "PGMD15/WKUP12" },
        {  "Q", "NPCS2",     "IRQ0",      "WKUP13"        },
        {  "Q", "SCK1",      "PWM0",      "WKUP14"        },
        {  "Q", "RTS1",      "PWM1",      "WKUP15"        },
        {  "Q", "INVALID",   "INVALID",   "INVALID"       }
    },
#endif
    {
        //  Q    Periph. A   Periph. B    special
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },                  // {1} PORT ADC
#if defined _HW_SAM7S || defined _HW_SAM7SE                              // {2}{3}
        {  "Q", "INVALID",   "INVALID",   "INVALID"  },
#endif
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
        {  "AD4", "AD4",     "AD4",       "AD4"      },
        {  "AD5", "AD5",     "AD5",       "AD5"      },
        {  "AD6", "AD6",     "AD6",       "AD6"      },
        {  "AD7", "AD7",     "AD7",       "AD7"      },
    }
};

static unsigned char *_ptrPerFunctions;

#ifdef SUPPORT_ADC                                                       // {1}
    typedef struct stPorts
    {
        unsigned char *ports;
        unsigned long *adcs;
        unsigned long *ulADCMode;
    } PORTS;

    static unsigned long *_ptrADC;
    static unsigned long *_ptrADC_mode;
#endif

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
#ifdef _HW_SAM7S                                                         // {2}
    #define ADC_START 17
#else
    #define ADC_START 27
#endif
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iADC = -1;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE + 1];

    switch (iPort) {
    case _GPIO_A:
        STRCPY(cPortDetails, "Port A");
        break;
#ifndef _HW_SAM7S                                                        // {2}
    case _GPIO_B:
        STRCPY(cPortDetails, "Port B");
        break;
#endif
#if defined _HW_SAM7SE || defined _HW_SAM7L                              // {3}{4}
    case _GPIO_C:
        STRCPY(cPortDetails, "Port C");
        break;
#endif
    case _GPIO_ADC:                                                      // {1}
        STRCPY(cPortDetails, "ADC inputs");
        iADC = 4;
        break;
    default:
        STRCPY(cPortDetails, "?????");
        return;
    }

    iBit = iPortLength - iBit - 1;                                       // bit position
    if (iBit < 0) {
        return;
    }
    if (iADC < 0) {                                                     // {1}
        SPRINTF(cBuf, " Bit %i Pin: ", iBit);
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if (iBit < ADC_START) {
            return;
        }
        STRCAT(cPortDetails, " Pin: ");
    }
    STRCPY(cBuf, cPinNumber[iPort][iBit]);
    STRCAT(cPortDetails, cBuf);
    if (!strcmp(cBuf, "NA")) {
        return;
    }

    if (ulPortPeripheral[iPort] & (0x01 << iBit)) {
        unsigned char *ptrList = _ptrPerFunctions;
        int _iPort = iPort;
        int _iBit = iBit;
        while (_iPort--) {
            ptrList += (PORT_WIDTH);
        }
        while (_iBit--) {
            ptrList++;
        }
        cBuf[0] = ' ';
        if (*ptrList > 3) {
            STRCPY(&cBuf[1], "??");
        }
        else {
            STRCPY(&cBuf[1], cPer[iPort][iBit][*ptrList]);
        }
        STRCAT(cPortDetails, cBuf);
#ifdef SUPPORT_ADC                                                       // {1}
        if (*ptrList == 3) {
            iADC = (0 + (iBit - ADC_START));
            if (iPort == _GPIO_ADC) {
                iADC += 4;
            }
        }
        else if (iADC >= 0) {
            iADC = (iBit - ADC_START);
        }
        if (iADC >= 0) {
            if (*_ptrADC_mode & LOWRES) {
                SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iADC]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)ADC_FULL_SCALE_8BIT)); // {3}
            }
            else {
                SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iADC]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)ADC_FULL_SCALE_10BIT)); // {3}
            }
            STRCAT(cPortDetails, cBuf);
        }
#endif
    }
    else {
        if (iPort == _GPIO_ADC) {                                        // {1}
            STRCAT(cPortDetails, " Inactive");
            return;
        }
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
    }
}

// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
    #ifdef SUPPORT_ADC                                                   // {1}
    PORTS *_ports = (PORTS *)hw_table;
    _ptrPerFunctions = _ports->ports;
    _ptrADC          = _ports->adcs;
    _ptrADC_mode     = _ports->ulADCMode;
    #else
    _ptrPerFunctions = (unsigned char *)hw_table; 
    #endif
}
#endif

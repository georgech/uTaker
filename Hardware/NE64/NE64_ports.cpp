/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   ---------------------------------------------------------------------
   File:        NE64_ports.c
   Project:     Single Chip Embedded Internet
   ---------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2011
   *********************************************************************/

#include "config.h"

#ifdef _HW_NE64

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

#ifdef CHIP_80_PIN
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // Port A (0..7)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // Port B (0..7)
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // Port E (0..7)
    { "40 {PE0/XIRQ}", "39 {PE1/IRQ}", "NA", "NA", "29 {PE4/ECLK}", "NA", "NA", "NA"},                  
    // Port G (0..7)    
    { "14 {PG0/KWG0/MII_RXD0}", "15 {PG1/KWG1/MII_RXD1}", "16 {PG2/KWG2/MII_RXD2}", "17 {PG3/KWG3/MII_RXD3}", "18 {PG4/KWG4/MII_RXCLK}", "19 {PG5/KWG5/MII_RXDV}", "20 {PG6/KWG6/MII_RXER}", "NA"}, 
    // Port H (0..6)
    { "7 {PH0/KWH0/MII_TXD0}", "6 {PH1/KWH1/MII_TXD1}}", "5 {PH2/KWH2/MII_TXD2}", "4 {PH3/KWH3/MII_TXD3}", "3 {PH4/KWH4/MII_TXCLK}", "2 {PH5/KWH5/MII_TXEN}", "1 {PH6/KWH6/MII_TXER}", "NA"}, 
    // PORT J (0..7)
    { "8 {PJ0/KWJ0/MII_MDC}",  "9 {PJ1/KWJ1/MII_MDIO}",  "12 {PJ2/KWJ2/MII_CRS}", "13 {PJ3/KWJ3/MII_COL}", "NA", "NA", "80 {PJ6/KWJ6/IIC_SDA}", "79 {PJ7/KWJ7/IIC_SSL}"}, 
    // PORT K (0..7)    
    { "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},                          
    // PORT L (0..7)
    { "60 {PL0/ACTLED}", "59 {PL1/LNKLED}", "57 {PL2/SPDLED}", "43 {PL3/DUPLED}", "44 {PL4/COLLED}", "NA", "NA", "NA"},
    // PORT S (0..7)
    { "21 {PS0/SCI0_RXD}", "22 {PS1/SCI0_TXD}", "23 {PS2/SCI1_RXD}", "24 {PS3/SCI1_TXD}", "25 {PS4/SPI_MISO}", "26 {PS5/SPI_MOSI}", "27 {PS6/SPI_SCK}", "28 {PS7/SPI_SS}"}, 
    // PORT T (0..7)
    { "NA", "NA", "NA", "NA", "78 {PT[4]/TIM_IOC[4]}", "77 {PT[5]/TIM_IOC[5]}", "76 {PT[6]/TIM_IOC[6]}", "75 {PT[7]/TIM_IOC[7]}"}, 
    // PORT AD (0..7)
    { "61 {PAD[0]/AN[0]}", "62 {PAD[1]/AN[1]}", "63 {PAD[2]/AN[2]}", "64 {PAD[3]/AN[3]}", "65 {PAD[4]/AN[4]}", "66 {PAD[5]/AN[5]}", "67 {PAD[6]/AN[6]}", "68 {PAD[7]/AN[7]}"},
};
#else
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    // Port A (0..7)
    { "60 {PA[0]/ADDR-DATA[8]}", "61 {PA[1]/ADDR-DATA[9]}", "62 {PA[2]/ADDR-DATA[10]}", "63 {PA[3]/ADDR-DATA[11]}", "77 {PA[4]/ADDR-DATA[12]}", "78 {PA[5]/ADDR-DATA[13]}", "79 {PA[6]/ADDR-DATA[14]}", "80 {PA[7]/ADDR-DATA[15]}"},
    // Port B (0..7)
    { "10 {PB[0]/ADDR-DATA[8]}", "11 {PB[1]/ADDR-DATA[9]}", "12 {PB[2]/ADDR-DATA[10]}", "13 {PB[3]/ADDR-DATA[11]}", "16 {PB[4]/ADDR-DATA[12]}", "17 {PB[5]/ADDR-DATA[13]}", "18 {PB[6]/ADDR-DATA[14]}", "19 {PB[7]/ADDR-DATA[15]}"},
    // Port E (0..7)
    { "56 {PE0/XIRQ}", "55 {PE1/IRQ}", "54 {PE2/RW}", "53 {PE3/TAGLO/LSTRB}", "41 {PE4/ECLK}", "40 {PE5/PIPE0/MODA}", "39 {PE6/PIPE1/MODB}", "38 {PE7/NOACC}"},
    // Port G (0..7)    
    { "22 {PG0/KWG0/MII_RXD0}", "23 {PG1/KWG1/MII_RXD1}", "24 {PG2/KWG2/MII_RXD2}", "25 {PG3/KWG3/MII_RXD3}", "26 {PG4/KWG4/MII_RXCLK}", "27 {PG5/KWG5/MII_RXDV}", "28 {PG6/KWG6/MII_RXER}", "29 {PG7/KWG7}"}, 
    // Port H (0..6)
    { "7 {PH0/KWH0/MII_TXD0}", "6 {PH1/KWH1/MII_TXD1}}", "5 {PH2/KWH2/MII_TXD2}", "4 {PH3/KWH3/MII_TXD3}", "3 {PH4/KWH4/MII_TXCLK}", "2 {PH5/KWH5/MII_TXEN}", "1 {PH6/KWH6/MII_TXER}", "NA"}, 
    // Port J (0..7)
    { "8 {PJ0/KWJ0/MII_MDC}",  "9 {PJ1/KWJ1/MII_MDIO}",  "20 {PJ2/KWJ2/MII_CRS}", "21 {PJ3/KWJ3/MII_COL}", "NA", "NA", "112 {PJ6/KWJ6/IIC_SDA}", "111 {PJ7/KWJ7/IIC_SSL}"}, 
    // PORT K (0..7)    
    { "97 {PK0/XADDR14}", "98 {PK1/XADDR15}", "99 {PK2/XADDR16}", "100 {PK3/XADDR17}", "103 {PK4/XADDR18}", "104 {PK5/XADDR19}", "105 {PK6/XCS}", "106 {PK7/ECS/ROMCTL}"},  
    // PORT L (0..7)
    { "84 {PL0/ACTLED}", "83 {PL1/LNKLED}", "81 {PL2/SPDLED}", "59 {PL3/DUPLED}", "58 {PL4/COLLED}", "53 {PL5}", "51  {PL6}", "NA"},
    // PORT S (0..7)
    { "30 {PS0/SCI0_RXD}", "31 {PS1/SCI0_TXD}", "32 {PS2/SCI1_RXD}", "33 {PS3/SCI1_TXD}", "34 {PS4/SPI_MISO}", "35 {PS5/SPI_MOSI}", "36 {PS6/SPI_SCK}", "37 {PS7/SPI_SS}"}, 
    // PORT T (0..7)
    { "NA", "NA", "NA", "NA", "110 {PT[4]/TIM_IOC[4]}", "109 {PT[5]/TIM_IOC[5]}", "108 {PT[6]/TIM_IOC[6]}", "107 {PT[7]/TIM_IOC[7]}"}, 
    // PORT AD (0..7)
    { "85 {PAD[0]/AN[0]}", "86 {PAD[1]/AN[1]}", "87 {PAD[2]/AN[2]}", "88 {PAD[3]/AN[3]}", "89 {PAD[4]/AN[4]}", "90 {PAD[5]/AN[5]}", "91 {PAD[6]/AN[6]}", "92 {PAD[7]/AN[7]}"},
};
#endif

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "AD[8]",    "INVALID"    },                 // PORT A
        {  "Q", "NA",        "AD[9]",    "INVALID"    },
        {  "Q", "NA",        "AD[10]",   "INVALID"    },
        {  "Q", "NA",        "AD[11]",   "INVALID"    },
        {  "Q", "NA",        "AD[12]",   "INVALID"    },
        {  "Q", "NA",        "AD[13]",   "INVALID"    },
        {  "Q", "NA",        "AD[14]",   "INVALID"    },
        {  "Q", "NA",        "AD[15]",   "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "AD[0]",    "INVALID"    },                 // PORT B
        {  "Q", "NA",        "AD[1]",    "INVALID"    },
        {  "Q", "NA",        "AD[2]",    "INVALID"    },
        {  "Q", "NA",        "AD[3]",    "INVALID"    },
        {  "Q", "NA",        "AD[4]",    "INVALID"    },
        {  "Q", "NA",        "AD[5]",    "INVALID"    },
        {  "Q", "NA",        "AD[6]",    "INVALID"    },
        {  "Q", "NA",        "AD[7]",    "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "XIRQ",     "INVALID"    },                 // PORT E
        {  "Q", "NA",        "IRQ",      "INVALID"    },
        {  "Q", "NA",        "RW",       "INVALID"    },
        {  "Q", "NA",        "TAGLO",    "LSTRB"      },
        {  "Q", "NA",        "ECK",      "INVALID"    },
        {  "Q", "NA",        "PIPE0",    "MODA"       },
        {  "Q", "NA",        "PIPE1",    "MODB"       },
        {  "Q", "NA",        "NOACC",    "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "KWG0",     "MII_RXD0"   },                 // PORT G
        {  "Q", "NA",        "KWG1",     "MII_RXD1"   },
        {  "Q", "NA",        "KWG2",     "MII_RXD2"   },
        {  "Q", "NA",        "KWG3",     "MII_RXD3"   },
        {  "Q", "NA",        "KWG4",     "MII_RXCLK"  },
        {  "Q", "NA",        "KWG5",     "MII_RXDV"   },
        {  "Q", "NA",        "KWG6",     "MII_RXER"   },
        {  "Q", "NA",        "KWG7",     "INVALID"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",       "KWH0",      "MII_TXD0"   },                 // PORT H
        {  "Q", "NA",       "KWH1",      "MII_TXD1"   },
        {  "Q", "NA",       "KWH2",      "MII_TXD2"   },
        {  "Q", "NA",       "KWH3",      "MII_TXD3"   },
        {  "Q", "NA",       "KWH4",      "MII_TXCLK"  },
        {  "Q", "NA",       "KWH5",      "MII_EN"     },
        {  "Q", "NA",       "KWH6",      "MII_TXER"   },
        {  "NA","NA",       "NA",        "NA"         }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",       "KWJ0",      "MII_MDIO"   },                 // PORT J
        {  "Q", "NA",       "KWJ1",      "MII_MDC"    },
        {  "Q", "NA",       "KWJ2",      "MII_CRS"    },
        {  "Q", "NA",       "KWJ3",      "MIICOL"     },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "NA",        "NA"         },
        {  "Q", "NA",       "KWJ6",      "IIC_SDA"    },
        {  "Q", "NA",       "KWJ7",      "IIC_SCL"    }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "XADDR[14]",  "INVALID"  },                 // PORT K
        {  "Q", "NA",        "XADDR[15]",  "INVALID"  },
        {  "Q", "NA",        "XADDR[16]",  "INVALID"  },
        {  "Q", "NA",        "XADDR[17]",  "INVALID"  },
        {  "Q", "NA",        "XADDR[18]",  "INVALID"  },
        {  "Q", "NA",        "XADDR[19]",  "INVALID"  },
        {  "Q", "NA",        "XCS",        "INVALID"  },
        {  "Q", "NA",        "ECS",        "INVALID"  }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",      "ACTLED",  "INVALID"       },                 // PORT L
        {  "Q", "NA",      "LNKLED",   "INVALID"      },
        {  "Q", "NA",      "SPDLED",   "INVALID"      },
        {  "Q", "NA",      "DUPLED",   "INVALID"      },
        {  "Q", "NA",      "COLLED",   "INVALID"      },
        {  "Q", "NA",      "INVALID",  "INVALID"      },
        {  "Q", "NA",      "INVALID",  "INVALID"      },
        {  "Q", "NA",      "NA",       "NA"           }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",      "SCI0_RXD",  "NA"          },                 // PORT S
        {  "Q", "NA",      "SCI0_TXD",  "NA"          },
        {  "Q", "NA",      "SCI1_RXD",  "NA"          },
        {  "Q", "NA",      "SCI1_TXD",  "NA"          },
        {  "Q", "NA",      "SPI_MISO",  "NA"          },
        {  "Q", "NA",      "SPI_MOSI",  "NA"          },
        {  "Q", "NA",      "SPI_SCK",   "NA"          },
        {  "Q", "NA",      "SPI_SS",    "NA"          }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "NA",       "NA"         },                 // PORT T
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "NA",        "NA",       "NA"         },
        {  "Q", "NA",        "TIM_IOC[4]","INVALID"   },
        {  "Q", "NA",        "TIM_IOC[5]","INVALID"   },
        {  "Q", "NA",        "TIM_IOC[6]","INVALID"   },
        {  "Q", "NA",        "TIM_IOC[7]","INVALID"   }
    },

    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "NA",        "AN[0]",    "INVALID"    },                 // PORT AD
        {  "Q", "NA",        "AN[1]",    "INVALID"    },
        {  "Q", "NA",        "AN[2]",    "INVALID"    },
        {  "Q", "NA",        "AN[3]",    "INVALID"    },
        {  "Q", "NA",        "AN[4]",    "INVALID"    },
        {  "Q", "NA",        "AN[5]",    "INVALID"    },
        {  "Q", "NA",        "AN[6]",    "INVALID"    },
        {  "Q", "NA",        "AN[7]",    "INVALID"    }
    },
};


static unsigned char *_ptrPerFunctions;



extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE+1];

    switch (iPort) {
    case _PORT_A:
#ifdef CHIP_80_PIN
        STRCPY(cPortDetails, "Port A - not available on 80 pin device");
        return;
#else
        STRCPY(cPortDetails, "Port A");
        break;
#endif
    case _PORT_B:
#ifdef CHIP_80_PIN
        STRCPY(cPortDetails, "Port B - not available on 80 pin device");
        return;
#else
        STRCPY(cPortDetails, "Port B");
        break;
#endif
    case _PORT_E:
        STRCPY(cPortDetails, "Port E");
        break;
    case _PORT_G:
        STRCPY(cPortDetails, "Port G");
        break;
    case _PORT_H:
        STRCPY(cPortDetails, "Port H");
        break;
    case _PORT_J:
        STRCPY(cPortDetails, "Port J");
        break;
    case _PORT_K:
#ifdef CHIP_80_PIN
        STRCPY(cPortDetails, "Port K - not available on 80 pin device");
        return;
#else
        STRCPY(cPortDetails, "Port K");
        break;
#endif
    case _PORT_L:
        STRCPY(cPortDetails, "Port L");
        break;
    case _PORT_S:
        STRCPY(cPortDetails, "Port S");
        break;
    case _PORT_T:
        STRCPY(cPortDetails, "Port T");
        break;
    case _PORT_AD:
        STRCPY(cPortDetails, "Port AD");
        break;
    default:
        STRCPY(cPortDetails, "?????");
        return;
    }

    // Bit position
    iBit = iPortLength - iBit - 1;
    if (iBit < 0) return;
    SPRINTF(cBuf, " Bit %i Pin: ", iBit);
    STRCAT(cPortDetails, cBuf);
    STRCPY(cBuf, cPinNumber[iPort][iBit]);
    STRCAT(cPortDetails, cBuf);
    if (!strcmp(cBuf, "NA")) return;



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
    }
    else {
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
    _ptrPerFunctions = (unsigned char *)hw_table; 
}

#endif

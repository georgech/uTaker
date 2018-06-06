/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      M523X_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
     // ADDR (4..7)
    { "NA", "NA", "NA", "NA", "NA", "A15 {A[21]/CS[4]/PADD[5]}", "C14 {A[22]/CS[5]/PADD[6]}", "B14 {A[23]/CS[6]/PADD[7]}"},
    // BS (4..7)
    { "NA", "NA", "NA", "NA", "A10 {BS[0]/CAS[0]/PBS[4]}", "A9 {BS[1]/CAS[1]/PBS[5]}", "B9 {BS[2]/CAS[2]/PBS[6]}", "C9 {BS[3]/CAS[3]/PBS[7]}"},                   
    // BUSCTL (0..7)    
    { "K15 {TIP/DREQ0/PBUSCTL[0]}", "K16 {TS/DACK2/PBUSCTL[1]}", "P8 {TSIZ0/DACK0/PBUSCTL[2]}", "N8 {TSIZ1/DACK1/PBUSCTL[3]}", "L16 {R/W/PBUSCTL[4]", "K13 {TEA/DREQ1/PBUSCTL[5]}", "K14 {TA/PBUSCTL[6]}", "T7 {OE/PBUSCTL[7]"},
    // CS (1..7)
    { "NA", "B13 {CS1/PCS[1]}", "D12 {CS2/SD_CS[0]/PCS[2]}", "B12 {CS3/SD_CS[1]/PCS[3]}", "A14 {CS4/PCS[4]}", "C13 {CS5/PCS[5]}", "A13 {CS6/PCS[6]}", "C12 {CS7/PCS[7]}"},                           
    // DATAH (0..7)
    { "P4 {D[8]/PDATAL[8]}",  "N4 {D[9]/PDATAL[9]}",  "T3 {D[10]/PDATAL[10]}", "R3 {D[11]/PDATAL[11]}", "P3 {D[12]/PDATAL[12]}", "N3 {D[13]/PDATAL[13]}", "T2 {D[14]/PDATAL[14]}", "R2 {D[15]/PDATAL[15]}"},   
    // DATAL (0..7)
    { "N7 {D[0]/PDATAL[0]}",  "R6 {D[1]/PDATAL[1]}",  "P6 {D[2]/PDATAL[2]}", "N6 {D[3]/PDATAL[3]}", "R5 {D[4]/PDATAL[4]}", "P5 {D[5]/PDATAL[5]}", "T4 {D[6]/PDATAL[6]}", "R4 {D[7]/PDATAL[7]}"},   
    // FECI2C (0..3)
    { "L14 {I2C_SCL/CAN0TX/PFECI2C[0]}", "L15 {I2C_SDA/CAN0RX/PFECI2C[1]}", "C7 {EMDIO/I2C_SDA/U2RXD/PFECI2C[2]}", "D7 {EMDC/I2C_SCL/U2TXD/PFECI2C[3]}", "NA", "NA", "NA", "NA"},
    // ETPU (0..2)
    { "J14 {LTPUODIS/PETPU[0]}", "J13 {UTPUODIS/PETPU[1]}", "F1 {TCRCLK/PETPU[2]}", "NA", "NA", "NA", "NA", "NA"},
    // IRQ (1..7)
    { "NA", "N10 {IRQ1/PIRQ[1]}", "T9 {IRQ2/DREQ2/PIRQ[2]}", "R9 {IRQ3/PIRQ[3]}", "P9 {IRQ4/PIRQ[4]}", "N9 {IRQ5/PIRQ[5]}", "T8 {IRQ6/PIRQ[6]}", "R8 {IRQ7/PIRQ[7]}"},
    // QSPI (0..4)
    { "D8 {QSPI_DOUT/PQSPI[0]}", "C8 {QSPI_DIN/I2C_SDA/PQSPI[1]}", "B8 {QSPI_CLK/I2C_SCL/PQSPI[2]}", "D9 {QSPI_CS0/PQSPI[3]}", "B10 {QSPI_CS1/SD_CKE/PQSPI[4]}", "NA", "NA", "NA"}, 
    // SDRAM (0..5) 
    { "M13 {SD_CS[0]/PSDRAM[0]}", "N15 {SD_CS[1]/PSDRAM[1]}", "C10 {SD_CKE/PSDRAM[2]}", "M14 {SD_SRAS/PSDRAM[3]}", "M15 {SD_SCAS/PSDRAM[4]}", "L13 {SD_WE/PSDRAM[5]}", "NA", "NA"},  
    // TIMER (0..7) 
    { "G3 {DT0OUT/DACK0/PTIMER[0]}", "G4 {DT0IN/DREQ0/PTIMER[1]}", "R7 {DT1OUT/DACK1/PTIMER[2]}", "P7 {DT1IN/DREQ1/PTIMER[3]}", "R10 {DT2OUT/DACK2/PTIMER[4]}", "P10 {DT2IN/DREQ2/DT2OUT/PTIMER[5]}", "J16 {DT3OUT/U2RTS/QSPI_CS3/PTIMER[6]}", "J15 {DT3IN/U2CTS/QSPI_CS2/PTIMER[7]}"},  
    // UARTH (0..1) 
    { "D10 {U2RXD/CAN1RX/PUARTH[0]}", "D11 {U2TXD/CAN1TX/PUARTH[1]}", "NA", "NA", "NA", "NA", "NA", "NA"},  
    // UARTL (0..7) 
    { "G2 {U0RXD/PUARTL[0]}", "H2 {U0TXD/PUARTL[1]}", "H3 {U0RTS/PUARTL[2]}", "G1 {U0CTS/PUARTL[3]}", "A11 {U1RXD/CAN0RX/PUARTL[4]}", "A12 {U1TXD/CAN0TX/PUARTL[5]}", "B11 {U1RTS/U2RTS/PUARTL[6]}", "C11 {U1CTS/U2CTS/PUARTL[7]}"},  

};


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][4] = {
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID", "INVALID",    "INVALID"    },                 // ADDR
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "A[21]",    "CS[4]",     "INVALID"    },
        {  "Q", "A[22]",    "CS[5]",     "INVALID"    },
        {  "Q", "A[23]",    "CS[6]",     "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID", "INVALID",    "INVALID"    },                 // BS
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "BS[0]",    "CAS[0]",    "INVALID"    },
        {  "Q", "BS[1]",    "CAS[1]",    "INVALID"    },
        {  "Q", "BS[2]",    "CAS[2]",    "INVALID"    },
        {  "Q", "BS[3]",    "CAS[3]",    "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "TIP",      "DREQ0",     "INVALID"    },                 // BUSCTL
        {  "Q", "TS",       "DACK2",     "INVALID"    },
        {  "Q", "TSIZ0",    "TSIZ0",     "INVALID"    },
        {  "Q", "TSIZ1",    "DACK1",     "INVALID"    },
        {  "Q", "R/W",      "INVALID",   "INVALID"    },
        {  "Q", "TEA",      "DREQ1",     "INVALID"    },
        {  "Q", "TA",       "INVALID",   "INVALID"    },
        {  "Q", "OE",       "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q","INVALID",   "INVALID",   "INVALID"    },                 // CS
        {  "Q", "CS1",      "INVALID",   "INVALID"    },
        {  "Q", "CS2",      "SD_CS[0]",  "INVALID"    },
        {  "Q", "CS3",      "SD_CS[1]",  "INVALID"    },
        {  "Q", "CS4",      "INVALID",   "INVALID"    },
        {  "Q", "CS5",      "INVALID",   "INVALID"    },
        {  "Q", "CS6",      "INVALID",   "INVALID"    },
        {  "Q", "CS7",      "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "D[8]",     "INVALID",   "INVALID"    },                 // DATAH
        {  "Q", "D[9]",     "INVALID",   "INVALID"    },
        {  "Q", "D[10]",    "INVALID",   "INVALID"    },
        {  "Q", "D[11]",    "INVALID",   "INVALID"    },
        {  "Q", "D[12]",    "INVALID",   "INVALID"    },
        {  "Q", "D[13]",    "INVALID",   "INVALID"    },
        {  "Q", "D[14]",    "INVALID",   "INVALID"    },
        {  "Q", "D[15]",    "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "D[0]",     "INVALID",   "INVALID"    },                 // DATAL
        {  "Q", "D[1]",     "INVALID",   "INVALID"    },
        {  "Q", "D[2]",     "INVALID",   "INVALID"    },
        {  "Q", "D[3]",     "INVALID",   "INVALID"    },
        {  "Q", "D[4]",     "INVALID",   "INVALID"    },
        {  "Q", "D[5]",     "INVALID",   "INVALID"    },
        {  "Q", "D[6]",     "INVALID",   "INVALID"    },
        {  "Q", "D[7]",     "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID",  "CAN0TX",    "I2C_SCL"    },                 // FECI2C
        {  "Q", "INVALID",  "CAN0RX",    "I2C_SDA"    },
        {  "Q", "U2RXD",    "I2C_SDA",   "EMDIO"      },
        {  "Q", "U2TXD",    "I2C_SCL",   "EMDC"       },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "LTPUODIS", "INVALID",   "INVALID"    },                 // ETPU
        {  "Q", "UTPUODIS", "INVALID",   "INVALID"    },
        {  "Q", "TCRCLK",   "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },                 // IRQ - fixed peripheral functions
        {  "IRQ1", "IRQ1",  "INVALID",   "INVALID"    },
        {  "IRQ2", "IRQ2",  "INVALID",   "INVALID"    },
        {  "IRQ3", "IRQ3",  "INVALID",   "INVALID"    },
        {  "IRQ4", "IRQ4",  "INVALID",   "INVALID"    },
        {  "IRQ5", "IRQ5",  "INVALID",   "INVALID"    },
        {  "IRQ6", "IRQ6",  "INVALID",   "INVALID"    },
        {  "IRQ7", "IRQ7",  "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "QSPI_DOUT","INVALID",  "INVALID"    },                 // QSPI
        {  "Q", "QSPI_DIN", "I2C_SDA",   "INVALID"    },
        {  "Q", "QSPI_CLK", "I2C_SCL",   "INVALID"    },
        {  "Q", "QSPI_CS0", "INVALID",   "INVALID"    },
        {  "Q", "QSPI_CS1", "SD_CKE",    "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "SD_CS[0]", "INVALID",   "INVALID"    },                 // SDRAM
        {  "Q", "SD_CS[1]", "INVALID",   "INVALID"    },
        {  "Q", "SD_CKE",   "INVALID",   "INVALID"    },
        {  "Q", "SD_SRAS",  "INVALID",   "INVALID"    },
        {  "Q", "SD_SCAS",  "INVALID",   "INVALID"    },
        {  "Q", "SD_WE",    "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "DT0OUT",   "DACK0",     "INVALID"    },                 // TIMER
        {  "Q", "DT0IN",    "DREQ0",     "INVALID"    },
        {  "Q", "DT1OUT",   "DACK1",     "INVALID"    },
        {  "Q", "DT1IN",    "DREQ1",     "INVALID"    },
        {  "Q", "DT2OUT",   "DACK2",     "INVALID"    },
        {  "Q", "DT2IN",    "DREQ2",     "DT2OUT"     },
        {  "Q", "DT3OUT",   "U2RTS",     "QSPI_CS3"   },
        {  "Q", "DT3IN",    "U2CTS",     "QSPI_CS2"   }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "U2RXD",    "CAN1RX",    "INVALID"    },                 // UARTH
        {  "Q", "U2TXD",    "CAN1TX",    "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "INVALID",   "INVALID"    }
    },
    {
        //  Q  Prim. Sec. Tertiary functions
        {  "Q", "U0RXD",    "INVALID",   "INVALID"    },                 // UARTL
        {  "Q", "U0TXD",    "INVALID",   "INVALID"    },
        {  "Q", "U0RTS",    "INVALID",   "INVALID"    },
        {  "Q", "U0CTS",    "INVALID",   "INVALID"    },
        {  "Q", "INVALID",  "CAN0RX",    "U1RXD"      },
        {  "Q", "INVALID",  "CAN0TX",    "U1TXD"      },
        {  "Q", "INVALID",  "U2RTS",     "U1RTS"      },
        {  "Q", "INVALID",  "U2CTS",     "U1CTS"      }
    }
};

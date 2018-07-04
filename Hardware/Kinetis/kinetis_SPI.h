/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_SPI.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */



/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */



/* =================================================================== */
/*                          SPI Interface                              */
/* =================================================================== */


/* =================================================================== */
/*                       SPI interrupt handlers                        */
/* =================================================================== */


/* =================================================================== */
/*                      SPI DMA interrupt handlers                     */
/* =================================================================== */



// General SPI configuration
//
extern void fnConfigSPI(SPITABLE *pars)
{
    switch (pars->Channel) {
    case 0:
        POWER_UP_ATOMIC(6, SPI0);
        _CONFIG_PERIPHERAL(C, 0, (PC_0_SPI0_PCS4));
        _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT));
        _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN));
        SPI0_MCR = SPI_MCR_HALT;
        SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL);
        SPI0_RSER = (SPI_SRER_RFDF_RE);
        switch (pars->ucSpeed) {
        case 0:                                                          // slave mode of operation
            SPI0_MCR = (SPI_MCR_SLAVE | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF);
            break;
        case MAX_SPI:
        case SPI_8MEG:
        case SPI_4MEG:
        case SPI_2MEG:
        case SPI_1MEG:
        case SPI_100K:
        default:
            SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);
            SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
            break;
        }
        CLEAR_RECEPTION_FLAG();
      //fnEnterInterrupt(irq_SPI0_ID, PRIORITY_TIMERS, fnHandleSPI);
        break;
    default:
        _EXCEPTION("Illegal SPI channel!");
        break;
    }
}


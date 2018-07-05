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

static unsigned long ulTxChipSelectLine[] = {0};

/* =================================================================== */
/*                          SPI Interface                              */
/* =================================================================== */


/* =================================================================== */
/*                       SPI interrupt handlers                        */
/* =================================================================== */

// Interrupt when the output fifo is not full
//
static __interrupt void int_spi_0_master(void)
{
    fnSPITxByte(0);                                                      // put next byte into the output fifo or else disable further interrupts
}

#if SPI_AVAILABLE > 1
static __interrupt void int_spi_1_master(void)
{
    fnSPITxByte(1);                                                      // put next byte into the output fifo or else disable further interrupts
}
#endif


static __interrupt void int_spi_0_slave(void)
{
    while ((SPI0_SR & SPI_SR_RFDF) != 0) {
        WRITE_ONE_TO_CLEAR(SPI0_SR, SPI_SR_RFDF);
        fnSPIRxByte((unsigned char)SPI0_POPR, 0);
    }
}

#if SPI_AVAILABLE > 1
static __interrupt void int_spi_1_slave(void)
{
    while ((SPI1_SR & SPI_SR_RFDF) != 0) {
        WRITE_ONE_TO_CLEAR(SPI1_SR, SPI_SR_RFDF);
        fnSPIRxByte((unsigned char)SPI1_POPR, 1);
    }
}
#endif

/* =================================================================== */
/*                      SPI DMA interrupt handlers                     */
/* =================================================================== */



extern void fnClearSPITxInt(QUEUE_HANDLE channel)
{
    switch (channel) {
    case 0:
        SPI0_RSER &= ~(SPI_SRER_TFFF_RE);
        break;
    #if SPI_AVAILABLE > 1
    case 1:
        SPI1_RSER &= ~(SPI_SRER_TFFF_RE);
        break;
    #endif
    }
}


// Send a byte to the SPI
//
extern int fnTxSPIByte(QUEUE_HANDLE channel, unsigned short usTxByte, int iLast)
{
    switch (channel) {
    case 0:
        if (iLast != 0) {
            SPI0_PUSHR = (usTxByte | SPI_PUSHR_EOQ | ulTxChipSelectLine[channel] | SPI_PUSHR_CTAS_CTAR0); // write a single byte/half-word to the output FIFO - negate CS line after transmission
        }
        else {
            SPI0_PUSHR = (usTxByte | SPI_PUSHR_CONT | ulTxChipSelectLine[channel] | SPI_PUSHR_CTAS_CTAR0); // write a single byte/half-word to the output FIFO - assert CS line
            SPI0_RSER |= SPI_SRER_TFFF_RE;                               // interrupt as long as the transmit fifo is not full
        #if defined _WINDOWS
            iInts |= (CHANNEL_0_SPI_INT << channel);
        #endif
        }
        break;
    #if SPI_AVAILABLE > 1
    case 1:
        if (iLast != 0) {
            SPI1_PUSHR = (usTxByte | SPI_PUSHR_EOQ | ulTxChipSelectLine[channel] | SPI_PUSHR_CTAS_CTAR0); // write a single byte/half-word to the output FIFO - negate CS line after transmission
        }
        else {
            SPI1_PUSHR = (usTxByte | SPI_PUSHR_CONT | ulTxChipSelectLine[channel] | SPI_PUSHR_CTAS_CTAR0); // write a single byte/half-word to the output FIFO - assert CS line
            SPI1_RSER |= SPI_SRER_TFFF_RE;                               // interrupt as long as the transmit fifo is not full
        #if defined _WINDOWS
            iInts |= (CHANNEL_0_SPI_INT << channel);
        #endif
        }
        break;
    #endif
    }
    return 0;
}

// General SPI configuration
//
extern void fnConfigSPI(SPITABLE *pars)
{
    switch (pars->Channel) {
    case 0:
        POWER_UP_ATOMIC(6, SPI0);
        ulTxChipSelectLine[pars->Channel] = SPI_PUSHR_PCS4;              // the chip select line used as output
        _CONFIG_PERIPHERAL(C, 0, (PC_0_SPI0_PCS4));
        _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT));
        _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN));
        SPI0_MCR = SPI_MCR_HALT;
        SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL);
        switch (pars->ucSpeed) {
        case 0:
            SPI0_MCR = (SPI_MCR_SLAVE | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF); // slave mode of operation
            SPI0_RSER |= SPI_SRER_RFDF_RE;
            fnEnterInterrupt(irq_SPI0_ID, PRIORITY_SPI0, int_spi_0_slave);
            break;
        case MAX_SPI:
        case SPI_8MEG:
        case SPI_4MEG:
        case SPI_2MEG:
        case SPI_1MEG:
        case SPI_100K:
        default:
            SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);
            SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_128 | SPI_CTAR_CPHA | SPI_CTAR_CPOL);
            fnEnterInterrupt(irq_SPI0_ID, PRIORITY_SPI0, int_spi_0_master);
            break;
        }
        break;
    #if SPI_AVAILABLE > 1
    case 1:
        POWER_UP_ATOMIC(6, SPI1);
        ulTxChipSelectLine[pars->Channel] = SPI_PUSHR_PCS0;              // the chip select line used as output
        _CONFIG_PERIPHERAL(B, 10, (PB_10_SPI1_PCS0));
        _CONFIG_PERIPHERAL(B, 11, (PB_11_SPI1_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(B, 16, (PB_16_SPI1_SOUT));
        _CONFIG_PERIPHERAL(B, 17, (PB_17_SPI1_SIN));
        SPI1_MCR = SPI_MCR_HALT;
        SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL);
        switch (pars->ucSpeed) {
        case 0:                                                          // slave mode of operation
            SPI1_MCR = (SPI_MCR_SLAVE | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF);
            SPI0_RSER |= SPI_SRER_RFDF_RE;
            fnEnterInterrupt(irq_SPI1_ID, PRIORITY_SPI1, int_spi_1_slave);
            break;
        case MAX_SPI:
        case SPI_8MEG:
        case SPI_4MEG:
        case SPI_2MEG:
        case SPI_1MEG:
        case SPI_100K:
        default:
            SPI1_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);
            SPI1_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_128 | SPI_CTAR_CPHA | SPI_CTAR_CPOL);
            fnEnterInterrupt(irq_SPI1_ID, PRIORITY_SPI1, int_spi_1_master);
            break;
        }
        break;
    #endif
    #if SPI_AVAILABLE > 2
    #endif
    default:
        _EXCEPTION("Illegal SPI channel!");
        break;
    }
}


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

static unsigned long ulTxChipSelectLine[SPI_AVAILABLE][6] = {{0}};
static unsigned long ulCharacteristics[SPI_AVAILABLE][6] = {{0}};
static unsigned long ulTransmissionSet[SPI_AVAILABLE] = {0};

/* =================================================================== */
/*                          SPI Interface                              */
/* =================================================================== */


/* =================================================================== */
/*                       SPI interrupt handlers                        */
/* =================================================================== */

// Interrupt when the output fifo is not full (has space to accept further data)
//
static __interrupt void int_spi_0_master(void)
{
    while (((SPI0_RSER & SPI_SR_TFFF) & SPI0_SR) != 0) {                 // while there is space in the buffer and the interrupt is enabled
        fnSPITxByte(0);                                                  // put next byte into the output fifo or else disable further interrupts
        WRITE_ONE_TO_CLEAR(SPI0_SR, SPI_SR_TFFF);                        // reset the interrupt flag (after writing the next byte to be transmitted)
    }
}

#if SPI_AVAILABLE > 1
static __interrupt void int_spi_1_master(void)
{
    while (((SPI1_RSER & SPI_SR_TFFF) & SPI1_SR) != 0) {                 // while there is space in the buffer and the interrupt is enabled
        fnSPITxByte(1);                                                  // put next byte into the output fifo or else disable further interrupts
        WRITE_ONE_TO_CLEAR(SPI1_SR, SPI_SR_TFFF);                        // reset the interrupt flag (after writing the next byte to be transmitted)
    }
}
#endif

#if SPI_AVAILABLE > 2
static __interrupt void int_spi_2_master(void)
{
    while (((SPI2_RSER & SPI_SR_TFFF) & SPI2_SR) != 0) {                 // while there is space in the buffer and the interrupt is enabled
        fnSPITxByte(2);                                                  // put next byte into the output fifo or else disable further interrupts
        WRITE_ONE_TO_CLEAR(SPI2_SR, SPI_SR_TFFF);                        // reset the interrupt flag (after writing the next byte to be transmitted)
    }
}
#endif


static __interrupt void int_spi_0_slave(void)
{
    while ((SPI0_SR & SPI_SR_RFDF) != 0) {                               // while there is reception data waiting in the input fifo
        fnSPIRxByte((unsigned char)SPI0_POPR, 0);                        // read the data and put it input the input buffer
        WRITE_ONE_TO_CLEAR(SPI0_SR, SPI_SR_RFDF);                        // clear the reception interrupt flag (after reading the data)
    }
}

#if SPI_AVAILABLE > 1
static __interrupt void int_spi_1_slave(void)
{
    while ((SPI1_SR & SPI_SR_RFDF) != 0) {                               // while there is reception data waiting in the input fifo
        fnSPIRxByte((unsigned char)SPI1_POPR, 1);                        // read the data and put it input the input buffer
        WRITE_ONE_TO_CLEAR(SPI1_SR, SPI_SR_RFDF);                        // clear the reception interrupt flag (after reading the data)
    }
}
#endif

#if SPI_AVAILABLE > 2
static __interrupt void int_spi_2_slave(void)
{
    while ((SPI2_SR & SPI_SR_RFDF) != 0) {                               // while there is reception data waiting in the input fifo
        fnSPIRxByte((unsigned char)SPI2_POPR, 1);                        // read the data and put it input the input buffer
        WRITE_ONE_TO_CLEAR(SPI2_SR, SPI_SR_RFDF);                        // clear the reception interrupt flag (after reading the data)
    }
}
#endif

/* =================================================================== */
/*                      SPI DMA interrupt handlers                     */
/* =================================================================== */


// This routine is used to block further transmit interrupts
//
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
#if SPI_AVAILABLE > 2
    case 2:
        SPI2_RSER &= ~(SPI_SRER_TFFF_RE);
        break;
#endif
    }
}


// Send a byte to the SPI
//
extern int fnTxSPIByte(QUEUE_HANDLE channel, unsigned short usTxByte, unsigned char ucChipSelect)
{
    _KINETIS_DSPI *ptrDSPI;
    switch (channel) {
    case 0:
        ptrDSPI = (_KINETIS_DSPI *)DSPI0_BLOCK;
        break;
    #if SPI_AVAILABLE > 1
    case 1:
        ptrDSPI = (_KINETIS_DSPI *)DSPI1_BLOCK;
        break;
    #endif
    #if SPI_AVAILABLE > 2
    case 2:
        ptrDSPI = (_KINETIS_DSPI *)DSPI2_BLOCK;
        break;
    #endif
    }
    if ((ucChipSelect & FIRST_SPI_MESSAGE_WORD) != 0) {
        ulTransmissionSet[channel] ^= SPI_PUSHR_CTAS_CTAR1;              // each new message switches to the opposite transmission control set so that transmissions in progress can continue uninterrupted
        if ((ulTransmissionSet[channel] & SPI_PUSHR_CTAS_CTAR1) != 0) {
            ptrDSPI->SPI_CTAR1 = ulCharacteristics[channel][ucChipSelect & SPI_CHIP_SELECT_MASK];
        }
        else {
            ptrDSPI->SPI_CTAR0 = ulCharacteristics[channel][ucChipSelect & SPI_CHIP_SELECT_MASK];
        }
    }
    if ((ucChipSelect & LAST_SPI_MESSAGE_WORD) != 0) {
        ptrDSPI->SPI_PUSHR = (usTxByte | SPI_PUSHR_EOQ | ulTxChipSelectLine[channel][ucChipSelect & SPI_CHIP_SELECT_MASK] | ulTransmissionSet[channel]); // write a single byte/half-word to the output FIFO - negate CS line after transmission
    }
    else {
        ptrDSPI->SPI_PUSHR = (usTxByte | SPI_PUSHR_CONT | ulTxChipSelectLine[channel][ucChipSelect & SPI_CHIP_SELECT_MASK] | ulTransmissionSet[channel]); // write a single byte/half-word to the output FIFO - assert CS line
        ptrDSPI->SPI_RSER |= SPI_SRER_TFFF_RE;                           // interrupt as long as the transmit fifo is not full
    }
    #if defined _WINDOWS
    iInts |= (CHANNEL_0_SPI_INT << channel);
    #endif
    return 0;
}

// General SPI configuration
//
extern void fnConfigSPI(SPITABLE *pars, int iAddChipSelect)
{
    _KINETIS_DSPI *ptrDSPI;
    int iIRQ_ID;
    unsigned char ucIRQ_priority;
    void(*InterruptFunc)(void);
    void(*InterruptFuncMaster)(void);
    void(*InterruptFuncSlave)(void);
    unsigned long ulWordWidth = ((pars->ucWordWidth - 1) << 24);         // define the word with used by this channel
    if ((pars->ucWordWidth < 4) || (pars->ucWordWidth > 16)) {
        _EXCEPTION("Invalid SPI word width");
    }
    if ((pars->Config & SPI_PHASE) != 0) {
        ulWordWidth |= (SPI_CTAR_CPHA);
    }
    if ((pars->Config & SPI_POL) != 0) {
        ulWordWidth |= (SPI_CTAR_CPOL);
    }
    ulTxChipSelectLine[pars->Channel][pars->ucChipSelect] = (SPI_PUSHR_PCS0 << pars->ucChipSelect); // the chip select line control
    ulCharacteristics[pars->Channel][pars->ucChipSelect] = (SPI_CTAR_DBR | ulWordWidth | SPI_CTAR_PDT_7 | SPI_CTAR_BR_128);
    if (iAddChipSelect != 0) {
        return;
    }
    switch (pars->Channel) {
    case 0:
        POWER_UP_ATOMIC(6, SPI0);
        ptrDSPI = (_KINETIS_DSPI *)DSPI0_BLOCK;
        _CONFIG_PERIPHERAL(C, 4, (PC_4_SPI0_PCS0 | PORT_PS_UP_ENABLE));
        _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT));
        _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN | PORT_PS_UP_ENABLE));
        iIRQ_ID = irq_SPI0_ID;
        ucIRQ_priority = PRIORITY_SPI0;
        InterruptFuncMaster = int_spi_0_master;
        InterruptFuncSlave = int_spi_0_slave;
        break;
    #if SPI_AVAILABLE > 1
    case 1:
        POWER_UP_ATOMIC(6, SPI1);
        ptrDSPI = (_KINETIS_DSPI *)DSPI1_BLOCK;
        switch (pars->ucChipSelect) {
        case 0:
            _CONFIG_PERIPHERAL(B, 10, (PB_10_SPI1_PCS0 | PORT_PS_UP_ENABLE));
            break;
        case 1:
            _CONFIG_PERIPHERAL(B, 9, (PB_9_SPI1_PCS1 | PORT_PS_UP_ENABLE));
            break;
        case 2:
            _CONFIG_PERIPHERAL(E, 5, (PE_5_SPI1_PCS2 | PORT_PS_UP_ENABLE));
            break;
        case 3:
            _CONFIG_PERIPHERAL(E, 6, (PE_6_SPI1_PCS3 | PORT_PS_UP_ENABLE));
            break;
        default:
            _EXCEPTION("Chip select line not available");
            break;
        }
        _CONFIG_PERIPHERAL(B, 11, (PB_11_SPI1_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_SOUT));
        _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_SIN | PORT_PS_UP_ENABLE));
        iIRQ_ID = irq_SPI1_ID;
        ucIRQ_priority = PRIORITY_SPI1;
        InterruptFuncMaster = int_spi_1_master;
        InterruptFuncSlave = int_spi_1_slave;
        break;
    #endif
    #if SPI_AVAILABLE > 2
    case 2:
        POWER_UP_ATOMIC(3, SPI2);
        ptrDSPI = (_KINETIS_DSPI *)DSPI2_BLOCK;
        _CONFIG_PERIPHERAL(B, 20, (PB_20_SPI2_PCS0 | PORT_PS_UP_ENABLE));
        _CONFIG_PERIPHERAL(B, 21, (PB_21_SPI2_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(B, 22, (PB_22_SPI2_SOUT));
        _CONFIG_PERIPHERAL(B, 23, (PB_23_SPI2_SIN | PORT_PS_UP_ENABLE));
        iIRQ_ID = irq_SPI2_ID;
        ucIRQ_priority = PRIORITY_SPI2;
        InterruptFuncMaster = int_spi_2_master;
        InterruptFuncSlave = int_spi_2_slave;
        break;
    #endif
    default:
        _EXCEPTION("Illegal SPI channel!");
        return;
    }
    ptrDSPI->SPI_MCR = SPI_MCR_HALT;
    switch (pars->ucSpeed) {
    case 0:
        ptrDSPI->SPI_MCR = (SPI_MCR_SLAVE | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF); // slave mode of operation
        ptrDSPI->SPI_CTAR0 = ulWordWidth;
        ptrDSPI->SPI_RSER |= SPI_SRER_RFDF_RE;
        InterruptFunc = InterruptFuncSlave;
        break;
    case MAX_SPI:
    case SPI_8MEG:
    case SPI_4MEG:
    case SPI_2MEG:
    case SPI_1MEG:
    case SPI_100K:
    default:
        if (pars->ucChipSelect > 5) {
            _EXCEPTION("Invalid chip select line");
            return;
        }
        ptrDSPI->SPI_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);
        ptrDSPI->SPI_CTAR1 = ptrDSPI->SPI_CTAR0 = ulCharacteristics[pars->Channel][pars->ucChipSelect];
        InterruptFunc = InterruptFuncMaster;
        break;
    }
    fnEnterInterrupt(iIRQ_ID, ucIRQ_priority, InterruptFunc);
}


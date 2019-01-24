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
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined DSPI_SPI
static const _KINETIS_DSPI *DSPI_Base_Address[SPI_AVAILABLE] = {
    (_KINETIS_DSPI *)DSPI0_BLOCK,
    #if SPI_AVAILABLE > 1
    (_KINETIS_DSPI *)DSPI1_BLOCK,
    #endif
    #if SPI_AVAILABLE > 2
    (_KINETIS_DSPI *)DSPI2_BLOCK,
    #endif
};
#elif defined LPSPI_SPI
static const _KINETIS_LPSPI *LPSPI_Base_Address[SPI_AVAILABLE] = {
    (_KINETIS_LPSPI *)LPSPI0_BLOCK,
    #if SPI_AVAILABLE > 1
    (_KINETIS_LPSPI *)LPSPI1_BLOCK,
    #endif
    #if SPI_AVAILABLE > 2
    (_KINETIS_LPSPI *)LPSPI2_BLOCK,
    #endif
};
#else
static const _KINETIS_SPI *SPI_Base_Address[SPI_AVAILABLE] = {
    (_KINETIS_SPI *)SPI0_BLOCK,
    #if SPI_AVAILABLE > 1
    (_KINETIS_SPI *)SPI1_BLOCK,
    #endif
    #if SPI_AVAILABLE > 2
    (_KINETIS_SPI *)SPI2_BLOCK,
    #endif
};
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned long ulTxChipSelectLine[SPI_AVAILABLE][SPI_CHIP_SELECTS] = {{0}};
static unsigned long ulCharacteristics[SPI_AVAILABLE][SPI_CHIP_SELECTS] = {{0}};

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
    #if defined DSPI_SPI
    WRITE_ONE_TO_CLEAR(SPI0_SR, (SPI_SR_EOQF));                          // ensure that the end of queue flag is cleared
    while (((SPI0_RSER & SPI_SR_TFFF) & SPI0_SR) != 0) {                 // while there is space in the buffer and the interrupt is enabled
        fnSPITxByte(0);                                                  // put next byte into the output fifo or else disable further interrupts
        WRITE_ONE_TO_CLEAR(SPI0_SR, SPI_SR_TFFF);                        // reset the interrupt flag (after writing the next byte to be transmitted)
    }
    #elif defined LPSPI_SPI

    #else
    if (((SPI0_C1 & SPI_C1_SPTIE) != 0) && ((SPI0_S & SPI_S_SPTEF) != 0)) { // if the transmit buffer has space and the transmit interrupt is enabled
        fnSPITxByte(0);                                                  // put next byte into the output fifo or else disable further interrupts
    }
    if (((SPI0_C1 & SPI_C1_SPIE) != 0) && ((SPI0_S & SPI_S_SPRF) != 0)) { // if the receive buffer is full and the receive interrupt is enabled
        fnSPITxByte(0);                                                  // this is used to negate the chip select
    }
    #endif
}

#if SPI_AVAILABLE > 1
static __interrupt void int_spi_1_master(void)
{
    #if defined DSPI_SPI
    WRITE_ONE_TO_CLEAR(SPI1_SR, (SPI_SR_EOQF));                          // ensure that the end of queue flag is cleared
    while (((SPI1_RSER & SPI_SR_TFFF) & SPI1_SR) != 0) {                 // while there is space in the buffer and the interrupt is enabled
        fnSPITxByte(1);                                                  // put next byte into the output fifo or else disable further interrupts
        WRITE_ONE_TO_CLEAR(SPI1_SR, SPI_SR_TFFF);                        // reset the interrupt flag (after writing the next byte to be transmitted)
    }
    #elif defined LPSPI_SPI

    #else
    if (((SPI1_C1 & SPI_C1_SPTIE) != 0) && ((SPI1_S & SPI_S_SPTEF) != 0)) { // if the transmit buffer has space and the transmit interrupt is enabled
        fnSPITxByte(1);                                                  // put next byte into the output fifo or else disable further interrupts
    }
    if (((SPI1_C1 & SPI_C1_SPIE) != 0) && ((SPI1_S & SPI_S_SPRF) != 0)) { // if the receive buffer is full and the receive interrupt is enabled
        fnSPITxByte(1);                                                  // this is used to negate the chip select
    }
    #endif
}
#endif

#if SPI_AVAILABLE > 2
static __interrupt void int_spi_2_master(void)
{
    #if defined DSPI_SPI
    WRITE_ONE_TO_CLEAR(SPI2_SR, (SPI_SR_EOQF));                          // ensure that the end of queue flag is cleared
    while (((SPI2_RSER & SPI_SR_TFFF) & SPI2_SR) != 0) {                 // while there is space in the buffer and the interrupt is enabled
        fnSPITxByte(2);                                                  // put next byte into the output fifo or else disable further interrupts
        WRITE_ONE_TO_CLEAR(SPI2_SR, SPI_SR_TFFF);                        // reset the interrupt flag (after writing the next byte to be transmitted)
    }
    #elif defined LPSPI_SPI

    #else
    if (((SPI2_C1 & SPI_C1_SPTIE) != 0) && ((SPI2_S & SPI_S_SPTEF) != 0)) { // if the transmit buffer has space and the transmit interrupt is enabled
        fnSPITxByte(2);                                                  // put next byte into the output fifo or else disable further interrupts
    }
    if (((SPI2_C1 & SPI_C1_SPIE) != 0) && ((SPI2_S & SPI_S_SPRF) != 0)) { // if the receive buffer is full and the receive interrupt is enabled
        fnSPITxByte(2);                                                  // this is used to negate the chip select
    }
    #endif
}
#endif


static __interrupt void int_spi_0_slave(void)
{
    #if defined DSPI_SPI
    while ((SPI0_SR & SPI_SR_RFDF) != 0) {                               // while there is reception data waiting in the input fifo
        fnSPIRxByte((unsigned char)SPI0_POPR, 0);                        // read the data and put it input the input buffer
        WRITE_ONE_TO_CLEAR(SPI0_SR, SPI_SR_RFDF);                        // clear the reception interrupt flag (after reading the data)
    }
    #else
    _EXCEPTION("To do");
    #endif
}

#if SPI_AVAILABLE > 1
static __interrupt void int_spi_1_slave(void)
{
    #if defined DSPI_SPI
    while ((SPI1_SR & SPI_SR_RFDF) != 0) {                               // while there is reception data waiting in the input fifo
        fnSPIRxByte((unsigned char)SPI1_POPR, 1);                        // read the data and put it input the input buffer
        WRITE_ONE_TO_CLEAR(SPI1_SR, SPI_SR_RFDF);                        // clear the reception interrupt flag (after reading the data)
    }
    #else
    _EXCEPTION("To do");
    #endif
}
#endif

#if SPI_AVAILABLE > 2
static __interrupt void int_spi_2_slave(void)
{
    #if defined DSPI_SPI
    while ((SPI2_SR & SPI_SR_RFDF) != 0) {                               // while there is reception data waiting in the input fifo
        fnSPIRxByte((unsigned char)SPI2_POPR, 1);                        // read the data and put it input the input buffer
        WRITE_ONE_TO_CLEAR(SPI2_SR, SPI_SR_RFDF);                        // clear the reception interrupt flag (after reading the data)
    }
    #else
    _EXCEPTION("To do");
    #endif
}
#endif

/* =================================================================== */
/*                      SPI DMA interrupt handlers                     */
/* =================================================================== */

/* =================================================================== */
/*                           SPI driver code                           */
/* =================================================================== */

static void fnAssertChipSelect(QUEUE_HANDLE channel, unsigned char ucChipSelect)
{
    int i;
    int iAssert = 0;
    for (i = 0; i < SPI_CHIP_SELECTS; i++) {
        if (ulTxChipSelectLine[channel][i] != 0) {
            if (ulTxChipSelectLine[channel][i] != ucChipSelect) {        // negate all other chip select lines
                _SETBITS(B, (1 << (ulTxChipSelectLine[channel][i] - 1)));// negate the CS line [note that the port reference is not solved yet]
            }
            else {
                iAssert = i;
            }
        }
    }
    // Assert the chip select line that is to be used
    //
    _CLEARBITS(B, (1 << (ulTxChipSelectLine[channel][iAssert] - 1))); // assert the CS line [note that the port reference is not solved yet]
}


// This routine is used to block further transmit interrupts
//
extern void fnClearSPITxInt(QUEUE_HANDLE channel)
{
    #if defined DSPI_SPI
    _KINETIS_DSPI *ptrDSPI = (_KINETIS_DSPI *)DSPI_Base_Address[channel];
    ptrDSPI->SPI_RSER &= ~(SPI_SRER_TFFF_RE);                            // disable interrupts
    #elif defined LPSPI_SPI
    _KINETIS_LPSPI *ptrLPSPI = (_KINETIS_LPSPI *)LPSPI_Base_Address[channel];
    #else
    _KINETIS_SPI *ptrSPI = (_KINETIS_SPI *)SPI_Base_Address[channel];
    ptrSPI->SPI_C1 &= ~(SPI_C1_SPIE | SPI_C1_SPTIE);                     // disable interrupts
    fnAssertChipSelect(channel, 0);                                      // deassert all chip select lines
    #endif
}


// Send a byte to the SPI
//
extern int fnTxSPIByte(QUEUE_HANDLE channel, unsigned short usTxByte, unsigned char ucChipSelect)
{
    #if defined DSPI_SPI
    static unsigned long ulTransmissionSet[SPI_AVAILABLE] = { 0 };
    _KINETIS_DSPI *ptrDSPI = (_KINETIS_DSPI *)DSPI_Base_Address[channel];
    if ((ucChipSelect & FIRST_SPI_MESSAGE_WORD) != 0) {
        ulTransmissionSet[channel] ^= SPI_PUSHR_CTAS_CTAR1;              // each new message switches to the opposite transmission control set so that transmissions in progress can continue uninterrupted
        ptrDSPI->SPI_MCR |= (SPI_MCR_HALT);                              // temporarily halt the operation so that the CTAR register can be written to
        if ((ulTransmissionSet[channel] & SPI_PUSHR_CTAS_CTAR1) != 0) {
            ptrDSPI->SPI_CTAR1 = ulCharacteristics[channel][ucChipSelect & SPI_CHIP_SELECT_MASK];
        }
        else {
            ptrDSPI->SPI_CTAR0 = ulCharacteristics[channel][ucChipSelect & SPI_CHIP_SELECT_MASK];
        }
        ptrDSPI->SPI_MCR &= ~(SPI_MCR_HALT);
    }
    if ((ucChipSelect & LAST_SPI_MESSAGE_WORD) != 0) {
        ptrDSPI->SPI_PUSHR = (usTxByte | SPI_PUSHR_EOQ | ulTxChipSelectLine[channel][ucChipSelect & SPI_CHIP_SELECT_MASK] | ulTransmissionSet[channel]); // write a single byte/half-word to the output FIFO - negate CS line after transmission
    }
    else {
        ptrDSPI->SPI_PUSHR = (usTxByte | SPI_PUSHR_CONT | ulTxChipSelectLine[channel][ucChipSelect & SPI_CHIP_SELECT_MASK] | ulTransmissionSet[channel]); // write a single byte/half-word to the output FIFO - assert CS line
    }
    ptrDSPI->SPI_RSER = (SPI_SRER_TFFF_RE | SPI_SRER_EOQF_RE);           // interrupt as long as the transmit fifo is not full or when final transmission terminates
    #elif defined LPSPI_SPI
    _KINETIS_LPSPI *ptrLPSPI = (_KINETIS_LPSPI *)LPSPI_Base_Address[channel];
    #else
    _KINETIS_SPI *ptrSPI = (_KINETIS_SPI *)SPI_Base_Address[channel];
    if ((ucChipSelect & FIRST_SPI_MESSAGE_WORD) != 0) {
        fnAssertChipSelect(channel, (ucChipSelect & SPI_CHIP_SELECT_MASK));
    }
    ptrSPI->SPI_D = (unsigned char)(usTxByte);
    if ((ucChipSelect & LAST_SPI_MESSAGE_WORD) != 0) {
        ptrSPI->SPI_C1 &= ~(SPI_C1_SPTIE);                               // interrupt when the transmit buffer is empty
        ptrSPI->SPI_C1 |= (SPI_C1_SPIE);                                 // interrupt when the receive buffer is full (in order to negate the chip select)
    }
    else {
        ptrSPI->SPI_C1 &= ~(SPI_C1_SPIE);
        ptrSPI->SPI_C1 |= (SPI_C1_SPTIE);                                // interrupt when the transmit buffer is empty
    }
    #endif
    #if defined _WINDOWS
    iInts |= (CHANNEL_0_SPI_INT << channel);
    #endif
    return 0;
}

// General SPI configuration
//
extern void fnConfigSPI(SPITABLE *pars, int iAddChipSelect)
{
    #if defined DSPI_SPI
    _KINETIS_DSPI *ptrDSPI;
    #elif defined LPSPI_SPI
    _KINETIS_LPSPI *ptrLPSPI;
    #else
    _KINETIS_SPI *ptrSPI;
    #endif
    int iIRQ_ID;
    unsigned char ucIRQ_priority;
    int iChipSelect = pars->ucChipSelect;
    void(*InterruptFunc)(void);
    void(*InterruptFuncMaster)(void);
    void(*InterruptFuncSlave)(void);
    unsigned long ulWordWidth = 0;
    #if defined DSPI_SPI
    ulWordWidth = ((pars->ucWordWidth - 1) << 27);         // define the word with used by this channel
    if ((pars->ucWordWidth < 4) || (pars->ucWordWidth > 16)) {
        _EXCEPTION("Invalid SPI word width");
    }
    #elif defined LPSPI_SPI
    #else
    if ((pars->ucWordWidth != 8) && (pars->ucWordWidth != 16)) {
        _EXCEPTION("Invalid SPI word width");
    }
    if (pars->ucWordWidth == 16) {
        ulWordWidth |= (SPI_C2_SPIMODE_16 << 8);
    }
    #endif
    if ((pars->Config & SPI_PHASE) != 0) {
    #if defined DSPI_SPI
        ulWordWidth |= (SPI_CTAR_CPHA);
    #else
        ulWordWidth |= SPI_C1_CPHA;
    #endif
    }
    if ((pars->Config & SPI_POL) != 0) {
    #if defined DSPI_SPI
        ulWordWidth |= (SPI_CTAR_CPOL);
    #else
        ulWordWidth |= SPI_C1_CPOL;
    #endif
    }
    if ((pars->Config & SPI_LSB) != 0) {                                 // least significant bit transmitted first
    #if defined DSPI_SPI
        ulWordWidth |= SPI_CTAR_LSBFE;
    #elif defined LPSPI_SPI
        ulWordWidth |= LPSPI_TCR_LSBF;
    #else
        ulWordWidth |= SPI_C1_LSBFE;
    #endif
    }
    if ((pars->Config & SPI_TX_MULTI_MODE) == 0) {
        iChipSelect = 0;                                                 // when multiple devices are not supported the first chip select entry is used
    }
    #if defined DSPI_SPI
    ulTxChipSelectLine[pars->Channel][iChipSelect] = (SPI_PUSHR_PCS0 << pars->ucChipSelect); // the chip select line control
    ulCharacteristics[pars->Channel][iChipSelect] = (SPI_CTAR_DBR | ulWordWidth | SPI_CTAR_PDT_7 | SPI_CTAR_BR_128);
    #else
    ulTxChipSelectLine[pars->Channel][iChipSelect] = (pars->ucChipSelect + 1); // the chip select line control
    ulCharacteristics[pars->Channel][iChipSelect] = (ulWordWidth);
    #endif
    switch (pars->Channel) {
    case 0:
        switch (pars->ucChipSelect) {
        case 0:
    #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(C, 4, (PC_4_SPI0_PCS0 | PORT_PS_UP_ENABLE));
    #else
    #endif
            break;
        case 1:
    #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(C, 3, (PC_3_SPI0_PCS1 | PORT_PS_UP_ENABLE));
    #else
    #endif
            break;
        case 2:
    #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(C, 2, (PC_2_SPI0_PCS2 | PORT_PS_UP_ENABLE));
    #else
    #endif
            break;
        case 3:
    #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(C, 1, (PC_1_SPI0_PCS3 | PORT_PS_UP_ENABLE));
    #else
    #endif
            break;
        case 4:
    #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(C, 0, (PC_0_SPI0_PCS4 | PORT_PS_UP_ENABLE));
    #else
    #endif
            break;
        case 5:
    #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(B, 23, (PB_23_SPI0_PCS5 | PORT_PS_UP_ENABLE));
    #else
    #endif
            break;
        default:
            _EXCEPTION("Chip select line not available");
            break;
        }
        if (iAddChipSelect != 0) {
            return;
        }
    #if defined DSPI_SPI
        POWER_UP_ATOMIC(6, SPI0);
        ptrDSPI = (_KINETIS_DSPI *)DSPI0_BLOCK;
    #elif defined LPSPI_SPI
        POWER_UP_ATOMIC(0, LPSPI0);
        ptrLPSPI = (_KINETIS_LPSPI *)LPSPI0_BLOCK;
    #else
        POWER_UP_ATOMIC(4, SPI0);
        ptrSPI = (_KINETIS_SPI *)SPI0_BLOCK;
    #endif
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
        switch (pars->ucChipSelect) {
        case 0:
        #if defined DSPI_SPI
            #if defined SPI1_CS0_ON_E
            _CONFIG_PERIPHERAL(E, 5, (PE_5_SPI1_PCS0 | PORT_PS_UP_ENABLE));
            #else
            _CONFIG_PERIPHERAL(B, 10, (PB_10_SPI1_PCS0 | PORT_PS_UP_ENABLE));
            #endif
        #else
        #endif
            break;
        case 1:
        #if defined DSPI_SPI
            #if defined SPI1_CS1_ON_E
            _CONFIG_PERIPHERAL(E, 0, (PE_0_SPI1_PCS1 | PORT_PS_UP_ENABLE));
            #else
            _CONFIG_PERIPHERAL(B, 9, (PB_9_SPI1_PCS1 | PORT_PS_UP_ENABLE));
            #endif
        #else
        #endif
            break;
        case 2:
        #if defined DSPI_SPI
            #if defined PE_3_SPI1_PCS2
            _CONFIG_PERIPHERAL(E, 2, (PE_3_SPI1_PCS2 | PORT_PS_UP_ENABLE)); 
            #else
            _CONFIG_PERIPHERAL(E, 5, (PE_5_SPI1_PCS2 | PORT_PS_UP_ENABLE));
            #endif
        #else
        #endif
            break;
        case 3:
        #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(E, 6, (PE_6_SPI1_PCS3 | PORT_PS_UP_ENABLE));
        #else
        #endif
            break;
        default:
            _EXCEPTION("Chip select line not available");
            break;
        }
        if (iAddChipSelect != 0) {
            return;
        }
    #if defined DSPI_SPI
        POWER_UP_ATOMIC(6, SPI1);
        ptrDSPI = (_KINETIS_DSPI *)DSPI1_BLOCK;
    #elif defined LPSPI_SPI
        POWER_UP_ATOMIC(0, LPSPI1);
        ptrLPSPI = (_KINETIS_LPSPI *)LPSPI1_BLOCK;
    #else
        POWER_UP_ATOMIC(4, SPI1);
        ptrSPI = (_KINETIS_SPI *)SPI1_BLOCK;
    #endif
        _CONFIG_PERIPHERAL(B, 11, (PB_11_SPI1_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_SOUT));
        _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_SIN | PORT_PS_UP_ENABLE));
        #if !defined irq_LPUART2_ID && defined INTMUX0_AVAILABLE
        iIRQ_ID = (irq_INTMUX0_0_ID + INTMUX_SPI1);
        ucIRQ_priority = INTMUX0_PERIPHERAL_SPI1;
        #else
        iIRQ_ID = irq_SPI1_ID;
        ucIRQ_priority = PRIORITY_SPI1;
        #endif
        InterruptFuncMaster = int_spi_1_master;
        InterruptFuncSlave = int_spi_1_slave;
        break;
    #endif
    #if SPI_AVAILABLE > 2
    case 2:
        switch (pars->ucChipSelect) {
        case 0:
        #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(B, 20, (PB_20_SPI2_PCS0 | PORT_PS_UP_ENABLE));
        #else
        #endif
            break;
        case 1:
        #if defined DSPI_SPI
            _CONFIG_PERIPHERAL(D, 15, (PD_15_SPI2_PCS1 | PORT_PS_UP_ENABLE));
        #else
        #endif
            break;
        default:
            _EXCEPTION("Chip select line not available");
            break;
        }
        if (iAddChipSelect != 0) {
            return;
        }
    #if defined DSPI_SPI
        POWER_UP_ATOMIC(3, SPI2);
        ptrDSPI = (_KINETIS_DSPI *)DSPI2_BLOCK;
    #elif defined LPSPI_SPI
        POWER_UP_ATOMIC(0, LPSPI2);
        ptrLPSPI = (_KINETIS_LPSPI *)LPSPI2_BLOCK;
    #else
        POWER_UP_ATOMIC(4, SPI2);
        ptrSPI = (_KINETIS_SPI *)SPI2_BLOCK;
    #endif
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
    #if defined DSPI_SPI
    ptrDSPI->SPI_MCR = SPI_MCR_HALT;
    #endif
    switch (pars->ucSpeed) {
    case 0:
    #if defined DSPI_SPI
        ptrDSPI->LPSPI_CR = LPSPI_CR_MEN;
        ptrDSPI->LPSPI_CFGR1 = 0;                                        // slave mode of operation
        ptrDSPI->LPSPI_TCR = ulWordWidth;
        ptrDSPI->SPI_RSER |= SPI_SRER_RFDF_RE;
    #elif defined LPSPI_SPI
        ptrLPSPI->SPI_MCR = (SPI_MCR_SLAVE | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF); // slave mode of operation
        ptrLPSPI->SPI_CTAR0 = ulWordWidth;
        ptrLPSPI->LPSPI_IER |= LPSPI_IER_TDIE;
    #else
        ptrSPI->SPI_C1 = (unsigned char)(ulWordWidth | SPI_C1_SPE | SPI_C1_SPIE); // slave mode of operation with interrupt enabled
        ptrSPI->SPI_C2 = (unsigned char)(ulWordWidth >> 8);
    #endif
        InterruptFunc = InterruptFuncSlave;
        break;
    case MAX_SPI:
    case SPI_8MEG:
    case SPI_4MEG:
    case SPI_2MEG:
    case SPI_1MEG:
    case SPI_100K:
    default:
        if (pars->ucChipSelect >= SPI_CHIP_SELECTS) {
            _EXCEPTION("Invalid chip select line");
            return;
        }
    #if defined DSPI_SPI
        ptrDSPI->SPI_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);
        ptrDSPI->SPI_CTAR1 = ptrDSPI->SPI_CTAR0 = ulCharacteristics[pars->Channel][pars->ucChipSelect];
    #elif defined LPSPI_SPI

    #else
        ptrSPI->SPI_C1 = (unsigned char)(ulWordWidth | SPI_C1_MSTR | SPI_C1_SPE | SPI_C1_SPIE); // master mode of operation with interrupt enabled
        ptrSPI->SPI_C2 = (unsigned char)(ulWordWidth >> 8);
        ptrSPI->SPI_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2);
    #endif
        InterruptFunc = InterruptFuncMaster;
        break;
    }
    #if !defined DSPI_SPI && !defined LPSPI_SPI
    (void)ptrSPI->SPI_S;
    (void)ptrSPI->SPI_D;
    #endif
    fnEnterInterrupt(iIRQ_ID, ucIRQ_priority, InterruptFunc);
}


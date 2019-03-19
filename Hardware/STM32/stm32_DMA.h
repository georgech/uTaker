/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_DMA.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/

#if defined _DMA_SHARED_CODE
// Buffer source to fixed destination address or fixed source address to buffer (beware that only DMA controller 2 can perform memory to memory transfers)
//
extern int fnConfigDMA_buffer(unsigned long ulDmaTriggerSource, unsigned long ulBufLength, void *ptrBufSource, void *ptrBufDest, unsigned long ulRules, void (*int_handler)(void), int int_priority)
{
    unsigned char ucSize = (unsigned char)(ulRules & 0x07);              // transfer size 1, 2 or 4 bytes
    unsigned long ulTransferType;
    STM32_DMA *ptrDMA_controller;
    STM32_DMA_STREAM *ptrDMAstream;
    int iStream = (ulDmaTriggerSource & 0x7);
    int iInterruptID;
    if ((ulDmaTriggerSource & DMA_CONTROLLER_REF_2) != 0) {              // set a pointer to the DMA controller to be used
        ptrDMA_controller = (STM32_DMA *)DMA2_BLOCK;
        if (iStream >= 5) {
            iInterruptID = (irq_DMA2_Stream5_ID + (iStream - 5));
        }
        else {
            iInterruptID = (irq_DMA2_Stream0_ID + iStream);
        }
    }
    else {
        ptrDMA_controller = (STM32_DMA *)DMA1_BLOCK;
        if (iStream == 7) {
            iInterruptID = irq_DMA1_Stream7_ID;
        }
        else {
            iInterruptID = (irq_DMA1_Stream0_ID + iStream);
        }
    }
    ptrDMAstream = &ptrDMA_controller->DMA_stream[iStream];              // select the stream registers to be used
    ptrDMAstream->DMA_SxCR = 0;                                          // disable stream
    
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    ulTransferType = (DMA_SxCR_CHSEL_7 & ulDmaTriggerSource);
    if ((ulRules & DMA_DIRECTION_OUTPUT) != 0) {                         // peripheral destination
        ptrDMAstream->DMA_SxM0AR = (unsigned long)ptrBufSource;          // address of memory source
        ptrDMAstream->DMA_SxPAR = (unsigned long)ptrBufDest;             // address of peripheral destination
        ulTransferType |= (DMA_SxCR_MINC | DMA_SxCR_DIR_M2P);
    }
    else {
        ptrDMAstream->DMA_SxPAR = (unsigned long)ptrBufSource;           // address of peripheral source
        ptrDMAstream->DMA_SxM0AR = (unsigned long)ptrBufDest;            // address of memory destination
        ulTransferType |= (DMA_SxCR_MINC | DMA_SxCR_DIR_P2M);
    }
    ptrDMAstream->DMA_SxNDTR = (unsigned short)(ulBufLength/ucSize);     // the number of transfers to be performed
    switch (ucSize) {
    case 1:
        ulTransferType |= (ulTransferType | DMA_SxCR_PSIZE_8 | DMA_SxCR_MSIZE_8 | DMA_SxCR_PL_HIGH); // set up DMA operation for byte transfer
        break;
    case 2:
        ulTransferType |= (DMA_SxCR_PSIZE_16 | DMA_SxCR_MSIZE_16 | DMA_SxCR_PL_HIGH); // set up DMA operation for short word transfer
        break;
    case 4:
        ulTransferType |= (DMA_SxCR_PSIZE_32 | DMA_SxCR_MSIZE_32 | DMA_SxCR_PL_HIGH); // set up DMA operation for long word transfer
        break;
    }
    #else
    DMA_CNDTR_MEMCPY = ((unsigned long)ulBufLength);                     // the number of byte transfers to be made (max 0xffff)
    DMA_CMAR_MEMCPY  = (unsigned long)ptrBufSource;                      // address of first byte to be transferred
    DMA_CPAR_MEMCPY  = (unsigned long)ptrBufDest;                        // address of first destination byte
    DMA_CCR_MEMCPY   = (DMA1_CCR1_EN | DMA1_CCR1_PINC | DMA1_CCR1_MINC | DMA1_CCR1_PSIZE_8 | DMA1_CCR1_MSIZE_8 | DMA1_CCR1_PL_MEDIUM | DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR); // set up DMA operation and start DMA transfer
    #endif
    if (int_handler != 0) {
        fnEnterInterrupt(iInterruptID, int_priority, int_handler);       // enter interrupt handler for the DMA controller stream
        if ((ulRules & DMA_HALF_BUFFER_INTERRUPT) != 0) {
            ulTransferType |= DMA_SxCR_HTIE;                             // enable interrupt on half-transfer completion
        }
        else {
            ulTransferType |= DMA_SxCR_TCIE;                             // enable interrupt on completion
        }
    }
    ptrDMAstream->DMA_SxCR = ulTransferType;

    // Note that the DMA channel has not been activated yet - to do this fnDMA_BufferReset(channel_number, DMA_BUFFER_START); is performed
    //
    return 0;
}
#endif

/* =================================================================== */
/*            DMA based memcpy(), memset() and reverse memcpy()        */
/* =================================================================== */

#if defined _DMA_MEM_TO_MEM
#if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA
#define SMALLEST_DMA_COPY 20                                             // smaller copies have no DMA advantage
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr1 = (unsigned char *)ptrTo;
    unsigned char *ptr2 = (unsigned char *)ptrFrom;
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= 0xffff) && (DMA2_S0CR == 0)) { // {27} if large enough to be worth while and if not already in use
        unsigned short usTransferSize;
        while (((unsigned long)ptr1) & 0x3) {                            // move to a long word boundary (the source is not guaranteed to be on a boundary, which can make the lomng word copy less efficient)
            *ptr1++ = *ptr2++;
            Size--;
        }
        //  DMA2 is used since DMA1 cannot perform memory to memory transfers
        //
        DMA2_S1PAR = (unsigned long)ptr2;                                // address of source
        DMA2_S1M0AR = (unsigned long)ptr1;                               // address of destination
        if (((unsigned long)ptr2 & 0x3) == 0) {                          // if both source and destination are long word aligned
            usTransferSize = ((unsigned short)(Size/sizeof(unsigned long))); // the number of long words to transfer by DMA
            DMA2_S1NDTR = usTransferSize;                                // the number of byte transfers to be made (max. 0xffff)
            DMA2_S1CR = (DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE_32 | DMA_SxCR_MSIZE_32 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
            usTransferSize *= sizeof(unsigned long);                     // the number of bytes being transferred by the DMA process
        }
        else if (((unsigned long)ptr2 & 0x1) == 0) {                     // if both source and destination are short word aligned
            usTransferSize = ((unsigned short)(Size/sizeof(unsigned short))); // the number of short words to transfer by DMA
            DMA2_S1NDTR = usTransferSize;                                // the number of byte transfers to be made (max. 0xffff)
            DMA2_S1CR = (DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE_16 | DMA_SxCR_MSIZE_16 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
            usTransferSize *= sizeof(unsigned short);                    // the number of bytes being transferred by the DMA process
        }
        else {
        #if defined _WINDOWS
            if (Size > 0xffff) {
                _EXCEPTION("DMA transfer doesn't support more than 64k!!");
            }
        #endif
            usTransferSize = (unsigned short)Size;
            DMA2_S1NDTR = usTransferSize;                                // the number of byte transfers to be made (max. 0xffff)
            DMA2_S1CR = (DMA_SxCR_PINC | DMA_SxCR_MINC | DMA_SxCR_PSIZE_8 | DMA_SxCR_MSIZE_8 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
        }
        DMA2_S1CR |= DMA_SxCR_EN;                                        // start operation
        ptr1 += usTransferSize;                                          // move the destination pointer to beyond the transfer
        ptr2 += usTransferSize;                                          // move the source pointer to beyond the transfer
        Size -= usTransferSize;                                          // bytes remaining
        while ((DMA2_LISR & DMA_LISR_TCIF1) == 0) { SIM_DMA(0) };        // wait until the transfer has terminated
        DMA2_LIFCR = (DMA_LISR_TCIF1 | DMA_LISR_HTIF1 | DMA_LISR_DMEIF1 | DMA_LISR_FEIFO1 | DMA_LISR_DMEIF1); // clear flags
        while (Size-- != 0) {                                            // {29}
            *ptr1++ = *ptr2++;
        }
        #if defined _WINDOWS
        DMA2_LISR = 0;
        #endif
        DMA2_S0CR = 0;                                                   // mark that the DMA stream is free for use again
    }
    #else
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= 0xffff) && (DMA_CNDTR_MEMCPY == 0)) { // if large enough to be worthwhile and if not already in use
        DMA_CNDTR_MEMCPY = ((unsigned long)(Size));                      // the number of byte transfers to be made (max 0xffff)
        DMA_CMAR_MEMCPY  = (unsigned long)ptrFrom;                       // address of first byte to be transferred
        DMA_CPAR_MEMCPY  = (unsigned long)ptrTo;                         // address of first destination byte
        DMA_CCR_MEMCPY   = (DMA1_CCR1_EN | DMA1_CCR1_PINC | DMA1_CCR1_MINC | DMA1_CCR1_PSIZE_8 | DMA1_CCR1_MSIZE_8 | DMA1_CCR1_PL_MEDIUM | DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR); // set up DMA operation and start DMA transfer
        while (DMA_CNDTR_MEMCPY != 0) { SIM_DMA(0) };                    // wait until the transfer has terminated
        DMA_CCR_MEMCPY = 0;
    }
    #endif
    else {                                                               // normal memcpy method
        while (Size--) {
            *ptr1++ = *ptr2++;
        }
  }
    return buffer; 
}

// memset implementation
//
extern void *uMemset(void *ptrTo, int iValue, size_t Size)               // {37}
{
    void *buffer = ptrTo;
    unsigned char ucValue = (unsigned char)iValue;                       // {37}
    unsigned char *ptr = (unsigned char *)ptrTo;
    #if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= (0xffff * sizeof(unsigned long))) && (DMA2_S0CR == 0)) { // {27} if large enough to be worth while and if not already in use
        volatile unsigned long ulToCopy = (ucValue | (ucValue << 8) | (ucValue << 16) | (ucValue << 24));
        unsigned short usTransferSize;
        while (((unsigned long)ptr) & 0x3) {                             // move to a long word bounday
            *ptr++ = ucValue;
            Size--;
        }
        usTransferSize = ((unsigned short)(Size/sizeof(unsigned long))); // the number of long words to transfer by DMA
        DMA2_S1NDTR = usTransferSize;                                    // the number of long word transfers to be made (max. 0xffff)
        DMA2_S1PAR = (unsigned long)&ulToCopy;                           // address of long word to be transfered
        DMA2_S1M0AR = (unsigned long)ptr;                                // address of first destination long word
        DMA2_S1CR = (DMA_SxCR_MINC | DMA_SxCR_PSIZE_32 | DMA_SxCR_MSIZE_32 | DMA_SxCR_PL_MEDIUM | DMA_SxCR_DIR_M2M); // set up DMA operation
        DMA2_S1CR |= DMA_SxCR_EN;                                        // start operation
        usTransferSize *= sizeof(unsigned long);                         // the number of bytes being transferred by the DMA process
        ptr += usTransferSize;                                           // move the destination pointer to beyond the transfer
        Size -= usTransferSize;                                          // bytes remaining
        while (Size--) {
            *ptr++ = ucValue;
        }
        while ((DMA2_LISR & DMA_LISR_TCIF1) == 0) { SIM_DMA(0) };        // wait until the DMA transfer has terminated
        DMA2_LIFCR = (DMA_LISR_TCIF1 | DMA_LISR_HTIF1 | DMA_LISR_DMEIF1 | DMA_LISR_FEIFO1 | DMA_LISR_DMEIF1); // clear flags
        #if defined _WINDOWS
        DMA2_LISR = 0;
        #endif
        DMA2_S0CR = 0;                                                   // mark that the DMA stream is free for use again
    }
    #else
    if ((Size >= SMALLEST_DMA_COPY) && (Size <= 0xffff) && (DMA_CNDTR_MEMCPY == 0)) { // if large enought to be worth while and if not already in use
        volatile unsigned char ucToCopy = ucValue;
        DMA_CNDTR_MEMCPY = ((unsigned long)(Size));                      // the number of byte transfers to be made (max. 0xffff)
        DMA_CMAR_MEMCPY  = (unsigned long)&ucToCopy;                     // address of byte to be transfered
        DMA_CPAR_MEMCPY  = (unsigned long)ptr;                           // address of first destination byte
        DMA_CCR_MEMCPY   = (DMA1_CCR1_EN | DMA1_CCR1_PINC | DMA1_CCR1_PSIZE_8 | DMA1_CCR1_MSIZE_8 | DMA1_CCR1_PL_MEDIUM | DMA1_CCR1_MEM2MEM | DMA1_CCR1_DIR); // set up DMA operation and start DMA transfer       
        while (DMA_CNDTR_MEMCPY != 0) { SIM_DMA(0) };                    // wait until the transfer has terminated
        DMA_CCR_MEMCPY = 0;
    }
    #endif
    else {                                                               // normal memset method
        while (Size--) {
            *ptr++ = ucValue;
        }
    }
    return buffer;
}
#endif
#endif
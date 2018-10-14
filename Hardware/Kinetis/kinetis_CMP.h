/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_CMP.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/
#if defined KINETIS_COMPARATOR
#if defined _CMP_INTERRUPT_CODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _CMP_Interrupt_0(void);
    #if NUMBER_OF_COMPARATORS > 1
static __interrupt void _CMP_Interrupt_1(void);
    #endif
    #if NUMBER_OF_COMPARATORS > 2
static __interrupt void _CMP_Interrupt_2(void);
    #endif
    #if NUMBER_OF_COMPARATORS > 3
static __interrupt void _CMP_Interrupt_3(void);
    #endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char CMP_IQR_ID[NUMBER_OF_COMPARATORS] = {
    (unsigned char)irq_CMP0_ID,
#if NUMBER_OF_COMPARATORS > 1
    (unsigned char)irq_CMP1_ID,
#endif
#if NUMBER_OF_COMPARATORS > 2
    (unsigned char)irq_CMP2_ID,
#endif
#if NUMBER_OF_COMPARATORS > 3
    (unsigned char)irq_CMP3_ID,
#endif
};

static const unsigned char *_CMP_Interrupt[NUMBER_OF_COMPARATORS] = {
    (unsigned char *)_CMP_Interrupt_0,
    #if NUMBER_OF_COMPARATORS > 1
    (unsigned char *)_CMP_Interrupt_1,
    #endif
    #if NUMBER_OF_COMPARATORS > 2
    (unsigned char *)_CMP_Interrupt_2,
    #endif
    #if NUMBER_OF_COMPARATORS > 3
    (unsigned char *)_CMP_Interrupt_3
    #endif
};


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_CMP_Interrupt_handler[NUMBER_OF_COMPARATORS])(int iCMP_reference, int iEvent) = {0}; // user comparator interrupt handlers

/* =================================================================== */
/*                 Comparator Interrupt Handlers                       */
/* =================================================================== */

static void fnHandleCMP(int iCMP, KINETIS_COMPARATOR *ptrCMP)
{
    unsigned char ucInterrupt = ptrCMP->CMP_SCR;                         // read the present status
    ptrCMP->CMP_SCR = ucInterrupt;                                       // reset the rising/falling flag(s) that will be handled
    if (_CMP_Interrupt_handler[iCMP] != 0) {                             // if there is a callback handler
        uDisable_Interrupt();
            _CMP_Interrupt_handler[iCMP](iCMP, (((ucInterrupt >> 1) - 1) & 0x03)); // call handling function
        uEnable_Interrupt();
    }
}


// CMP 0 interrupt
//
static __interrupt void _CMP_Interrupt_0(void)
{
    fnHandleCMP(0, (KINETIS_COMPARATOR *)CMP0_BLOCK);
}

#if NUMBER_OF_COMPARATORS > 1
// CMP 1 interrupt
//
static __interrupt void _CMP_Interrupt_1(void)
{
    fnHandleCMP(1, (KINETIS_COMPARATOR *)CMP1_BLOCK);
}
#endif

#if NUMBER_OF_COMPARATORS > 2
// CMP 2 interrupt
//
static __interrupt void _CMP_Interrupt_2(void)
{
    fnHandleCMP(2, (KINETIS_COMPARATOR *)CMP2_BLOCK);
}
#endif

#if NUMBER_OF_COMPARATORS > 3
// CMP 3 interrupt
//
static __interrupt void _CMP_Interrupt_3(void)
{
    fnHandleCMP(3, (KINETIS_COMPARATOR *)CMP3_BLOCK);
}
#endif
#endif

/* =================================================================== */
/*                  Comparator Configuration                           */
/* =================================================================== */

#if defined _CMP_CONFIG_CODE
        {
            COMPARATOR_SETUP *ptrCMP_settings = (COMPARATOR_SETUP *)ptrSettings;
            KINETIS_COMPARATOR *ptrCMP = (KINETIS_COMPARATOR *)CMP0_BLOCK;
            unsigned char ucControl0 = (unsigned char)(ptrCMP_settings->comparator_mode);
            unsigned char ucControl1 = (unsigned char)(ptrCMP_settings->comparator_mode >> 8);
            unsigned char ucControlIRQ = (unsigned char)(ptrCMP_settings->comparator_mode >> 16);
            unsigned char ucControlDAC = (unsigned char)(ptrCMP_settings->comparator_mode >> 24);
            unsigned char ucPin = ptrCMP_settings->negative_input;
            int i;
            if (ptrCMP_settings->comparator >= NUMBER_OF_COMPARATORS) {
                _EXCEPTION("Invalid CMP referenced!");
                return;
            }
            ptrCMP += ptrCMP_settings->comparator;
            POWER_UP_ATOMIC(4, CMP);                                     // enable clocks to all comparator instances

            for (i = 0; i < 2; i++) {
                switch (ptrCMP_settings->comparator) {
                case 0:
                    break;
    #if NUMBER_OF_COMPARATORS > 1
                case 1:
                    switch (ucPin) {
                    case 0:
                    case 1:
                        #define PC_3_CMP1_IN1 PORT_MUX_ALT0
                        _CONFIG_PERIPHERAL(C, 3, (PC_3_CMP1_IN1));       // ensure the pin function
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        break;
                    default:
                        break;
                    }
                    break;
    #endif
                }
                ucPin = ptrCMP_settings->positive_input;
            }

            if ((COMPARATOR_OUTPUT_ENABLE & ptrCMP_settings->comparator_mode) != 0) {
                // Enable output pin
                //
                #define PB_21_CMP1_OUT PORT_MUX_ALT6
                _CONFIG_PERIPHERAL(B, 21, (PB_21_CMP1_OUT));       // ensure the pin function
            }

            if (ptrCMP_settings->int_handler != 0) {                     // if interrupt handler defined
                _CMP_Interrupt_handler[ptrCMP_settings->comparator] = ptrCMP_settings->int_handler; // enter the interrupt handler function
                fnEnterInterrupt(CMP_IQR_ID[ptrCMP_settings->comparator], ptrCMP_settings->int_priority, (void(*)(void))_CMP_Interrupt[ptrCMP_settings->comparator]);
            }
            else {
                ucControlIRQ = 0;
            }
            ucControlIRQ |= (CMP_SCR_CFF | CMP_SCR_CRF);                 // clear pending interrupt flags
            ptrCMP->CMP_CR0 = ucControl0;                                // set filter samples and hysterisis
            ptrCMP->CMP_FPR = ptrCMP_settings->filter;
            ptrCMP->CMP_SCR = ucControlIRQ;                              // set interrupt/DMA mode
            ptrCMP->CMP_MUXCR = (ptrCMP_settings->negative_input | (ptrCMP_settings->positive_input << 3)); // connect the minus and plus inputs
            ptrCMP->CMP_DACCR = (ucControlDAC | (ptrCMP_settings->DAC_level & 0x3f)); // set DAC mode
            ptrCMP->CMP_CR1 = ucControl1;                                // set mode and enable
        }
#endif
#endif
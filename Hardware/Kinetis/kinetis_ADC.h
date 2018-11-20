/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_ADC.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    03.06.2013 Add ADC result to interrupt call-back                     {41}
    30.09.2013 Add ADC A/B input multiplexer control                     {54}
    27.10.2013 Add ADC DMA configuration                                 {57}
    13.05.2015 Add low/high threshold single shot interrupt mode         {1}
    04.12.2015 Add Kinetis KE ADC mode                                   {2}
    23.12.2015 Add automatic ADC DMA buffer repetition                   {3}
    04.01.2016 Allow free-run ADC with DMA                               {4}
    06.03.2017 Allow alternative DMA trigger sources                     {5}
    01.10.2018 Allow ADC polling (with optional wait/busy behavior)      {6}

*/

#if defined _ADC_INTERRUPT_CODE

#if defined KINETIS_KE && !defined KINETIS_KE15 && !defined KINETIS_KE18
    #define KINETIS_KE_ADC                                               // ADC used by most KE/KEA parts)
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _ADC_Interrupt_0(void);
    #if ADC_CONTROLLERS > 1
static __interrupt void _ADC_Interrupt_1(void);
    #endif
    #if ADC_CONTROLLERS > 2
static __interrupt void _ADC_Interrupt_2(void);
    #endif
    #if ADC_CONTROLLERS > 3
static __interrupt void _ADC_Interrupt_3(void);
    #endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char *_ADC_Interrupt[ADC_CONTROLLERS] = {
    (unsigned char *)_ADC_Interrupt_0,
    #if ADC_CONTROLLERS > 1
    (unsigned char *)_ADC_Interrupt_1,
    #endif
    #if ADC_CONTROLLERS > 2
    (unsigned char *)_ADC_Interrupt_2,
    #endif
    #if ADC_CONTROLLERS > 3
    (unsigned char *)_ADC_Interrupt_3
    #endif
};


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_ADC_Interrupt_handler[ADC_CONTROLLERS])(ADC_INTERRUPT_RESULT *) = {0}; // user ADC interrupt handlers


/* =================================================================== */
/*                        ADC Interrupt Handlers                       */
/* =================================================================== */

// ADC 0 interrupt
//
static __interrupt void _ADC_Interrupt_0(void)
{
    ADC0_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[0] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC0_RA;
        adc_result.ucADC_channel = 0;                                    // channel is not added
        adc_result.ucADC_flags = ADC_RESULT_VALID;                       // assumed that the result is valid
        uDisable_Interrupt();
            _ADC_Interrupt_handler[0](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}

    #if ADC_CONTROLLERS > 1
// ADC 1 interrupt
//
static __interrupt void _ADC_Interrupt_1(void)
{
    ADC1_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[1] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC1_RA;
        adc_result.ucADC_channel = 0;
        adc_result.ucADC_flags = ADC_RESULT_VALID;
        uDisable_Interrupt();
            _ADC_Interrupt_handler[1](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}
    #endif
    #if ADC_CONTROLLERS > 2
// ADC 2 interrupt
//
static __interrupt void _ADC_Interrupt_2(void)
{
    ADC2_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[2] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC2_RA;
        adc_result.ucADC_channel = 0;
        adc_result.ucADC_flags = ADC_RESULT_VALID;
        uDisable_Interrupt();
            _ADC_Interrupt_handler[2](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}
    #endif
    #if ADC_CONTROLLERS > 3
// ADC 3 interrupt
//
static __interrupt void _ADC_Interrupt_3(void)
{
    ADC3_SC1A = ADC_SC1A_ADCH_OFF;                                       // disable further operation and interrupt
    if (_ADC_Interrupt_handler[3] != 0) {
        ADC_INTERRUPT_RESULT adc_result;                                 // {41}
        adc_result.sADC_value = (signed short)ADC3_RA;
        adc_result.ucADC_channel = 0;
        adc_result.ucADC_flags = ADC_RESULT_VALID;
        uDisable_Interrupt();
            _ADC_Interrupt_handler[3](&adc_result);                      // call handling function to inform that the result is ready
        uEnable_Interrupt();
    }
}
    #endif

    #if !defined KINETIS_KE_ADC
static unsigned char fnSetADC_channel(unsigned char ucADC_channel, int iDiffMode)
{
        #if defined _WINDOWS
    if ((ucADC_channel == 0x1c) || (ucADC_channel > ADC_SC1A_ADCH_OFF) || ((ucADC_channel > ADC_SC1A_ADCH_23) && (ucADC_channel < ADC_SC1A_ADCH_TEMP_SENS))) {
        _EXCEPTION("Selecting invalid ADC channel!");                    // invalid channels                                     
    }
        #endif
    if ((iDiffMode != 0) && ((ucADC_channel <= ADC_SC1A_ADCH_D3) || ((ucADC_channel >= ADC_SC1A_ADCH_TEMP_SENS) && (ucADC_channel <= ADC_SC1A_ADCH_VREFSH)))) { // set up channel in differential mode (channels 0..3, temperature, band-gap and VREFSH)
        return (ucADC_channel | ADC_SC1A_DIFF);
    }
    else {                                                               // single ended (channels 0..23, temperature, band-gap and VREFSH and VREFSL)
        switch (ucADC_channel) {
        case ADC_SE23_SINGLE:
        #if defined KINETIS_KL43
            _CONFIG_PERIPHERAL(E, 30, PE_30_ADC0_SE23);                  // ensure that the ADC pin is configured
        #endif
            break;
        case ADC_SE4_SINGLE:
        #if defined KINETIS_KL43
            _CONFIG_PERIPHERAL(E, 21, PE_21_ADC0_SE4);                   // ensure that the ADC pin is configured
        #endif
            break;
        #if defined KINETIS_KL17 || defined KINETIS_KL27
        case ADC_SE6_SINGLE:
            _CONFIG_PERIPHERAL(D, 5, PD_5_ADC0_SE6b);                     // ensure that the ADC pin is configured
            break;
        #endif
        #if defined KINETIS_KL17 || defined KINETIS_KL27
        case ADC_SE8_SINGLE:
            _CONFIG_PERIPHERAL(B, 0, PB_0_ADC0_SE8);                     // ensure that the ADC pin is configured
            break;
        #endif
        #if defined KINETIS_KL03
        case ADC_SE15_SINGLE:
            _CONFIG_PERIPHERAL(A, 0, PA_0_ADC0_SE15);                    // ensure that the ADC pin is configured (warning: this changes the default SWD_CLK configuration, losing any debug support!)
            break;
        #endif
        }
        return (ucADC_channel);
    }
}
    #endif

// Convert a standard 16 bit value to the present ADC mode format
//
static unsigned short fnConvertADCvalue(KINETIS_ADC_REGS *ptrADC, unsigned short usStandardValue, int iPlusOne) // {1}
{
    #if defined KINETIS_KE_ADC                                           // {2}
    switch (ptrADC->ADC_SC3 & (ADC_CFG1_MODE_12 | ADC_CFG1_MODE_10)) {
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x0fff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x03ff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x00ff) {
                usStandardValue++;
            }
        }
        break;
    }
    #else
    switch (ptrADC->ADC_CFG1 & ADC_CFG1_MODE_MASK) {
        #if !defined KINETIS_KE15
    case ADC_CFG1_MODE_16:                                               // conversion mode - single-ended 16 bit or differential 16 bit
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0xffff) {
                usStandardValue++;
            }
        }
        break;
        #endif
    case ADC_CFG1_MODE_12:                                               // conversion mode - single-ended 12 bit or differential 13 bit
        usStandardValue >>= 4;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0800) {
                usStandardValue |= 0xf000;                               // sign extend
            }
        }
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x0fff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_10:                                               // conversion mode - single-ended 10 bit or differential 11 bit
        usStandardValue >>= 6;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0200) {
                usStandardValue |= 0xfc00;                               // sign extend
            }
        }
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x03ff) {
                usStandardValue++;
            }
        }
        break;
    case ADC_CFG1_MODE_8:                                                // conversion mode - single-ended 8 bit or differential 9 bit
        usStandardValue >>= 8;
        if (ptrADC->ADC_SC1A & ADC_SC1A_DIFF) {                          // differential mode
            if (usStandardValue & 0x0080) {
                usStandardValue |= 0xff00;                               // sign extend
            }
        }
        if (iPlusOne != 0) {                                             // increase by 1 so that a value is one LSB above the match threshold
            if (usStandardValue != 0x00ff) {
                usStandardValue++;
            }
        }
        break;
    }
    #endif
    return usStandardValue;
}
#endif

/* =================================================================== */
/*                         ADC Configuration                           */
/* =================================================================== */

#if defined _ADC_CONFIG_CODE
        {
            ADC_SETUP *ptrADC_settings = (ADC_SETUP *)ptrSettings; 
            KINETIS_ADC_REGS *ptrADC;
            int irq_ADC_ID;
    #if !defined KINETIS_KE_ADC
            unsigned char ucADC_channel = ptrADC_settings->int_adc_bit;  // the channel to be configured
    #endif
            if ((ADC_DISABLE_ADC & ptrADC_settings->int_adc_mode) != 0) {
                if (ptrADC_settings->int_adc_controller == 0) {
    #if defined KINETIS_WITH_PCC
                    PCC_ADC0 &= ~(PCC_CGC | PCC_PCS_MASK);               // disable clocks to module
    #else
                    POWER_DOWN_ATOMIC(6, ADC0);                          // disable clocks to module
    #endif
                }
    #if ADC_CONTROLLERS > 1
                else if (ptrADC_settings->int_adc_controller == 1) {
        #if defined KINETIS_WITH_PCC
                    PCC_ADC1 = 0;                                        // disable clocks to module
        #elif defined KINETIS_K22_SF7
                    POWER_DOWN_ATOMIC(6, ADC1);                          // disable clocks to module
        #else
                    POWER_DOWN_ATOMIC(3, ADC1);                          // disable clocks to module
        #endif
                }
        #if ADC_CONTROLLERS > 2
                else if (ptrADC_settings->int_adc_controller == 2) {
                    POWER_DOWN_ATOMIC(6, ADC2);                          // disable clocks to module
                }
        #endif
        #if ADC_CONTROLLERS > 3
                else if (ptrADC_settings->int_adc_controller == 3) {
                    POWER_DOWN_ATOMIC(3, ADC3);                          // disable clocks to module
                }
        #endif
    #endif
    #if defined _WINDOWS
                else {
                    _EXCEPTION("ADC controller doesn't exist!!");
                }
    #endif
                return;
            }
            if (ptrADC_settings->int_adc_controller == 0) {
    #if defined irq_ADCA_ID
                irq_ADC_ID = irq_ADCA_ID;
    #else
                irq_ADC_ID = irq_ADC0_ID;
    #endif
                ptrADC = (KINETIS_ADC_REGS *)ADC0_BLOCK;
            }
    #if ADC_CONTROLLERS > 1
            else if (ptrADC_settings->int_adc_controller == 1) {
    #if defined irq_ADCB_ID
                irq_ADC_ID = irq_ADCB_ID;
    #else
                irq_ADC_ID = irq_ADC1_ID;
    #endif
                ptrADC = (KINETIS_ADC_REGS *)ADC1_BLOCK;
            }
    #endif
    #if ADC_CONTROLLERS > 2 && !defined KINETIS_KE18
            else if (ptrADC_settings->int_adc_controller == 2) {
                irq_ADC_ID = irq_ADC2_ID;
                ptrADC = (KINETIS_ADC_REGS *)ADC2_BLOCK;
            }
    #endif
    #if ADC_CONTROLLERS > 3
            else if (ptrADC_settings->int_adc_controller == 3) {
                irq_ADC_ID = irq_ADC3_ID;
                ptrADC = (KINETIS_ADC_REGS *)ADC3_BLOCK;
            }
    #endif
            else {
    #if defined _WINDOWS
                _EXCEPTION("ADC controller doesn't exist!!");
    #endif
                return;
            }
            if ((ptrADC_settings->int_adc_mode & ADC_READ_ONLY) == 0) {  // if not being called only for read
                unsigned char ucChannelConfig = 0;
                if ((ptrADC_settings->int_adc_mode & ADC_CONFIGURE_ADC) != 0) { // main configuration is to be performed
                    if (ptrADC_settings->int_adc_controller == 0) {      // ADC0
    #if defined KINETIS_WITH_PCC
        #if defined KINETIS_KE15
                        SELECT_PCC_PERIPHERAL_SOURCE(ADC0, ADC0_PCC_SOURCE); // select the PCC clock used by ADC0, which must be supplied by the PCC
        #else
                        if ((ptrADC_settings->int_adc_mode & ADC_CFG1_ADICLK_ASY) == ADC_CFG1_ADICLK_ALT) { // if the alternative clock source is selected
                            SELECT_PCC_PERIPHERAL_SOURCE(ADC0, ADC0_PCC_SOURCE); // select the PCC clock used by ADC0
                        }
        #endif
    #endif
                        POWER_UP_ATOMIC(6, ADC0);                        // enable clocks to module
                    }
    #if ADC_CONTROLLERS > 1
                    else if (ptrADC_settings->int_adc_controller == 1) {
        #if defined KINETIS_WITH_PCC
            #if defined KINETIS_KE15
                        SELECT_PCC_PERIPHERAL_SOURCE(ADC1, ADC1_PCC_SOURCE); // select the PCC clock used by ADC1, which must be supplied by the PCC
            #else
                        if ((ptrADC_settings->int_adc_mode & ADC_CFG1_ADICLK_ASY) == ADC_CFG1_ADICLK_ALT) { // if the alternative clock source is selected
                            SELECT_PCC_PERIPHERAL_SOURCE(ADC1, ADC1_PCC_SOURCE); // select the PCC clock used by ADC1
                        }
            #endif
        #endif
        #if defined KINETIS_K22_SF7
                        POWER_UP_ATOMIC(6, ADC1);                        // enable clocks to module
        #else
                        POWER_UP_ATOMIC(3, ADC1);                        // enable clocks to module
        #endif
                    }
        #if ADC_CONTROLLERS > 2
                    else if (ptrADC_settings->int_adc_controller == 2) {
            #if defined KINETIS_WITH_PCC
                        if ((ptrADC_settings->int_adc_mode & ADC_CFG1_ADICLK_ASY) == ADC_CFG1_ADICLK_ALT) { // if the alternative clock source is selected
                            SELECT_PCC_PERIPHERAL_SOURCE(ADC2, ADC2_PCC_SOURCE); // select the PCC clock used by ADC1
                        }
            #endif
                        POWER_UP_ATOMIC(6, ADC2);                        // enable clocks to module
                    }
            #if ADC_CONTROLLERS > 3
                    else if (ptrADC_settings->int_adc_controller == 3) {
                        POWER_UP_ATOMIC(3, ADC3);                        // enable clocks to module
                    }
            #endif
        #endif
    #endif
                    ptrADC->ADC_CFG1 = (0xff & ptrADC_settings->int_adc_mode);
    #if defined ADC_CFG1_ADICLK_ALT2_
                    if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADICLK_ASY) == ADC_CFG1_ADICLK_ALT2) { // if the IRC48MCLK is being chosen as clock source we ensure that it is enabled
                        MCG_C7 = MCG_C7_OSCSEL_IRC48MCLK;                // enable clock even if not used by other sources
                    }
    #endif
    #if defined _WINDOWS                                                 // simuation check of ADC clock speed range
                    // Check that the ADC frequency is set to a valid range
                    //
                    {
                        unsigned long ulADC_clockMax = 0;
                        unsigned long ulADC_clockMin = 0;
                        switch (ptrADC->ADC_CFG1 & ADC_CFG1_ADICLK_ASY) {
        #if defined KINETIS_KE15
                        case ADC_CFG1_ADICLK_BUS:                        // equivalent to ALTCLK1 in KE15
        #else
                        case ADC_CFG1_ADICLK_ALT:
        #endif
        #if defined KINETIS_WITH_PCC
                            if (ptrADC_settings->int_adc_controller == 0) {
                                ulADC_clockMax = ulADC_clockMin = fnGetPCC_clock(KINETIS_PERIPHERAL_ADC0); // get the clock speed that is configured for ADC0 usage
                            }
            #if ADC_CONTROLLERS > 1
                            else if (ptrADC_settings->int_adc_controller == 1) {
                                ulADC_clockMax = ulADC_clockMin = fnGetPCC_clock(KINETIS_PERIPHERAL_ADC1); // get the clock speed that is configured for ADC1 usage
                            }
            #endif
            #if ADC_CONTROLLERS > 2
                            else if (ptrADC_settings->int_adc_controller == 2) {
                                ulADC_clockMax = ulADC_clockMin = fnGetPCC_clock(KINETIS_PERIPHERAL_ADC2); // get the clock speed that is configured for ADC2 usage
                            }
            #endif
            #if ADC_CONTROLLERS > 3
                            else if (ptrADC_settings->int_adc_controller == 3) {
                                ulADC_clockMax = ulADC_clockMin = fnGetPCC_clock(KINETIS_PERIPHERAL_ADC3); // get the clock speed that is configured for ADC3 usage
                            }
            #endif
        #else
                            _EXCEPTION("Unsupported!");
        #endif
                            break;
        #if defined KINETIS_KE15
                        default:
                            _EXCEPTION("Only ALTCLK1 can be used by the KE15's ADC (use ADC_CLOCK_BUS or leave blank for compatibilty)!");
                            break;
        #else
                        case ADC_CFG1_ADICLK_BUS:
                            ulADC_clockMax = ulADC_clockMin = BUS_CLOCK;
                            break;
            #if defined ADC_CFG1_ADICLK_ALT2
                        case ADC_CFG1_ADICLK_ALT2:
                            ulADC_clockMax = ulADC_clockMin = 48000000;
                            break;
            #else
                        case ADC_CFG1_ADICLK_BUS2:
                            ulADC_clockMax = ulADC_clockMin = (BUS_CLOCK/2);
                            break;
            #endif
                        case ADC_CFG1_ADICLK_ASY:
                            if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADLPC) != 0) { // low power configuration is active (reduced power at the expense of the clock speed)
                                if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADLPC) != 0) { // high speed configuration
                                    ulADC_clockMax = 6100000;            // 4.0MHz typical ADC clock speed (range 2.4MHz..6.1MHz)
                                    ulADC_clockMin = 2400000;
                                }
                                else {                                   // normal conversion sequence
                                    ulADC_clockMax = 3900000;            // 2.4MHz typical ADC clock speed (range 1.2MHz..3.9MHz)
                                    ulADC_clockMin = 1200000;
                                }
                            }
                            else {                                       // low power configuration is not active
                                if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADLPC) != 0) { // high speed configuration
                                    ulADC_clockMax = 9500000;            // 6.2MHz typical ADC clock speed (range 4.4MHz..9.5MHz)
                                    ulADC_clockMin = 4400000;
                                }
                                else {                                   // normal conversion sequence
                                    ulADC_clockMax = 7300000;            // 5.2MHz typical ADC clock speed (range 3.0MHz..7.3MHz)
                                    ulADC_clockMin = 3000000;
                                }
                            }
                            break;
        #endif
                        }
                        ulADC_clockMax >>= ((ADC_CFG1_ADIV_8 & ptrADC->ADC_CFG1) >> 5);
                        ulADC_clockMin >>= ((ADC_CFG1_ADIV_8 & ptrADC->ADC_CFG1) >> 5);
                        switch (ptrADC->ADC_CFG1 & ADC_CFG1_MODE_MASK) {
                        case ADC_CFG1_MODE_8:
                        case ADC_CFG1_MODE_10:
                        case ADC_CFG1_MODE_12:
        #if defined KINETIS_KE_ADC
                            if ((ptrADC->ADC_CFG1 & ADC_CFG1_ADLPC) != 0) { // low power mode used when high sampling speeds are not required
                                if ((ulADC_clockMin < 400000) || (ulADC_clockMax > 4000000)) { // check valid ADC clock rate in low power mode
                                    _EXCEPTION("ADC clock rate outside valid range 400kHz..4MHz");
                                }
                            }
                            else {
                                if ((ulADC_clockMin < 400000) || (ulADC_clockMax > 8000000)) { // check valid ADC clock rate in high speed mode
                                    _EXCEPTION("ADC clock rate outside valid range 400kHz..8MHz");
                                }
                            }
        #else
                            if ((ulADC_clockMin < 1000000) || (ulADC_clockMax > 18000000)) { // check valid ADC clock rate
                                _EXCEPTION("ADC clock rate outside valid range 1MHz..18MHz for modes <= 13 bits");
                            }
        #endif
                            break;
        #if defined KINETIS_KE_ADC || defined KINETIS_KE15
                        default:
                            _EXCEPTION("Invalid ADC resolution!!");
                            break;
        #else
                        case ADC_CFG1_MODE_16:
                            if ((ulADC_clockMin < 2000000) || (ulADC_clockMax > 12000000)) { // check valid ADC clock rate
                                _EXCEPTION("ADC clock rate outside valid range 2MHz..12MHz for 16 bit mode");
                            }
                            else if ((ulADC_clockMax > 10000000) && ((ptrADC_settings->int_adc_sample & ADC_SAMPLE_HIGH_SPEED_CONFIG) == 0) || ((ptrADC_settings->int_adc_mode & ADC_LOW_POWER_CONFIG) != 0)) {
                                _EXCEPTION("Advise adding ADC_SAMPLE_HIGH_SPEED_CONFIG in int_adc_sample and/or removing ADC_LOW_POWER_CONFIG from int_adc_mode since ADC clock is high");
                            }
                            break;
        #endif
                        }
                    }
    #endif
    #if !defined KINETIS_KE_ADC
        #if defined KINETIS_KE15 || defined KINETIS_KE18
                    ptrADC->ADC_CFG2 = (ptrADC_settings->int_adc_sample - 1);
            #if defined _WINDOWS
                    if ((ptrADC->ADC_CFG2 == 0) || (ptrADC->ADC_CFG2 > 255)) {
                        _EXCEPTION("2 to 256 clocks per sample allowed!");
                    }
            #endif
        #else
                    if ((ptrADC_settings->int_adc_mode & ADC_SELECT_INPUTS_B) != 0) { // {54}
                        ptrADC->ADC_CFG2 = (ADC_CFG2_MUXSEL_B | (ptrADC_settings->int_adc_sample & 0x7)); // select mux B inputs
                    }
                    else {
                        ptrADC->ADC_CFG2 = (ADC_CFG2_MUXSEL_A | (ptrADC_settings->int_adc_sample & 0x7)); // select mux A inputs
                    }
        #endif
                    ptrADC->ADC_SC2 = ((ptrADC_settings->int_adc_mode >> 8) & (ADC_SC2_REFSEL_ALT)); // configure the reference voltage used
                    if ((ADC_CALIBRATE & ptrADC_settings->int_adc_mode) != 0) { // calibration which should be performed once after a reset to achieve optimal accuracy
        #if defined KINETIS_KE15
                        ptrADC->ADC_CLPS = 0;
                        ptrADC->ADC_CLP3 = 0;
                        ptrADC->ADC_CLP2 = 0;
                        ptrADC->ADC_CLP1 = 0;
                        ptrADC->ADC_CLP0 = 0;
                        ptrADC->ADC_CLPX = 0;
                        ptrADC->ADC_CLP9 = 0;
        #endif
                        ptrADC->ADC_SC3 = (ADC_SC3_AVGS_32 | ADC_SC3_AVGE | ADC_SC3_CAL); // continuous conversion mode with hardware averaging during calibration
                        while ((ptrADC->ADC_SC1A & ADC_SC1A_COCO) == 0) {// wait for calibration to complete
        #if defined _WINDOWS
                            ptrADC->ADC_SC1A |= ADC_SC1A_COCO;
        #endif
                        }                                                // failure flag not checked since this should never fail
        #if !defined KINETIS_KE15
                        // Perform plus side calibration
                        //
                        ptrADC->ADC_PG = (((ptrADC->ADC_CLP0 + ptrADC->ADC_CLP1 + ptrADC->ADC_CLP2 + ptrADC->ADC_CLP3 + ptrADC->ADC_CLP4 + ptrADC->ADC_CLPS)/2) | 0x8000);

                        // Perform minus side calibration
                        //
                        ptrADC->ADC_MG = (((ptrADC->ADC_CLM0 + ptrADC->ADC_CLM1 + ptrADC->ADC_CLM2 + ptrADC->ADC_CLM3 + ptrADC->ADC_CLM4 + ptrADC->ADC_CLMS)/2) | 0x8000);
        #endif

                        ptrADC->ADC_SC3 &= ~(ADC_SC3_CAL);               // remove calibration enable when calibration has completed
                        ptrADC->ADC_SC1A = (ptrADC->ADC_SC1A & ~(ADC_SC1A_COCO)); // write to SC1A clears the conversion complete flag from calibration
                    }
    #endif
                    ptrADC->ADC_SC2 = ((ptrADC_settings->int_adc_mode >> 8) & (ADC_SC2_REFSEL_ALT | ADC_SC2_ADTRG_HW)); // configure the reference voltage to be used and the trigger mode (hardware or software)
    #if !defined KINETIS_KE_ADC
                    ptrADC->ADC_SC3 = ((ptrADC_settings->int_adc_sample >> 4) & 0x07); // configure hardware averaging
    #endif
                    if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) != 0) { // if continuous conversion is required
    #if defined KINETIS_KE_ADC
                        ucChannelConfig = ADC_SC1A_ADCO;                 // enable continuous conversion
    #else
                        ptrADC->ADC_SC3 |= ADC_SC3_ADCO;                 // enable continuous conversion
    #endif
                    }
    #if !defined KINETIS_KE_ADC
                    else {
                        ptrADC->ADC_SC3 &= ~(ADC_SC3_ADCO);              // disable continuous conversion
                    }
    #endif
                }
                if ((ptrADC_settings->int_adc_mode & ADC_CONFIGURE_CHANNEL) != 0) { // if a channel is to be configured
    #if !defined KINETIS_KE_ADC
                    ucChannelConfig = fnSetADC_channel(ucADC_channel, ((ptrADC_settings->int_adc_mode & ADC_DIFFERENTIAL_INPUT) != 0)); // check that the ADC channel is valid and prepare configuration value
                    if ((ptrADC_settings->int_adc_mode & ADC_HW_TRIGGERED) != 0) { // channel B is only valid in hardware triggered mode
                        ptrADC->ADC_SC1B = fnSetADC_channel(ptrADC_settings->int_adc_bit_b, (ptrADC_settings->int_adc_mode & ADC_DIFFERENTIAL_B));
                    }
    #endif
                  //if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) == 0) { // single shot mode {4} removed to allow free-run ADC with DMA
    #if !defined DEVICE_WITHOUT_DMA
                        if ((ptrADC_settings->int_adc_mode & (ADC_FULL_BUFFER_DMA | ADC_HALF_BUFFER_DMA)) != 0) { // {57} if DMA operation is being specified
                            unsigned long *ptrADC_result = (unsigned long *)((unsigned long)ptrADC + 0x010); // ADC channel as result register
                            unsigned long ulDMA_rules = (DMA_DIRECTION_INPUT | DMA_HALF_WORDS);
                            unsigned short usTriggerSource = ptrADC_settings->usDmaTriggerSource; // {5}
                            ptrADC->ADC_SC2 |= ADC_SC2_DMAEN;            // enable DMA trigger on ADC conversion end
                            if ((ptrADC_settings->int_adc_mode & ADC_FULL_BUFFER_DMA_AUTO_REPEAT) != 0) {
                                ulDMA_rules |= DMA_AUTOREPEAT;
                            }
                            if ((ptrADC_settings->int_adc_mode & ADC_HALF_BUFFER_DMA) != 0) {
                                ulDMA_rules |= DMA_HALF_BUFFER_INTERRUPT;
                            }
                            if (usTriggerSource == 0) {                  // {5} if the default is defined
                                usTriggerSource = (DMAMUX0_CHCFG_SOURCE_ADC0 + ptrADC_settings->int_adc_controller);
                            }
                            fnConfigDMA_buffer(ptrADC_settings->ucDmaChannel, usTriggerSource, ptrADC_settings->ulADC_buffer_length, ptrADC_result, ptrADC_settings->ptrADC_Buffer, ulDMA_rules, ptrADC_settings->dma_int_handler, ptrADC_settings->dma_int_priority); // source is the ADC result register and destination is the ADC buffer
                            fnDMA_BufferReset(ptrADC_settings->ucDmaChannel, DMA_BUFFER_START);
                        }
                        else if ((ptrADC_settings->int_adc_mode & ADC_LOOP_MODE) == 0) { // single shot mode {4}
    #endif
                            if (ptrADC_settings->int_handler != 0) {     // if single-shot conversion handler entered
                                _ADC_Interrupt_handler[ptrADC_settings->int_adc_controller] = ptrADC_settings->int_handler; // enter the interrupt handler function
                                fnEnterInterrupt(irq_ADC_ID, ptrADC_settings->int_priority, (void (*)(void))_ADC_Interrupt[ptrADC_settings->int_adc_controller]);
                                ucChannelConfig |= ADC_SC1A_AIEN;        // enable interrupt on end of conversion
                                if ((ptrADC_settings->int_adc_int_type & (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) != 0) { // {1} if a level is defined
    #if defined KINETIS_KE_ADC
                                    ucChannelConfig = ADC_SC1A_ADCO;     // enable continuous conversion
    #else
                                    ptrADC->ADC_SC3 |= ADC_SC3_ADCO;     // enable continuous conversion
    #endif
                                    switch (ptrADC_settings->int_adc_int_type & (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT)) {
                                    case ADC_LOW_LIMIT_INT:
                                        ptrADC->ADC_CV1 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_low_level_trigger, 0); // set low level threshold according to mode
                                        ptrADC->ADC_SC2 &= ~(ADC_SC2_ACFGT);
                                        ptrADC->ADC_SC2 |= (ADC_SC2_ACFE); // enable compare function for lower than threshold
                                        break;
                                    case ADC_HIGH_LIMIT_INT:
                                        ptrADC->ADC_CV1 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_high_level_trigger, 1); // set high level threshold according to mode
                                        ptrADC->ADC_SC2 |= (ADC_SC2_ACFGT | ADC_SC2_ACFE); // enable compare function for greater or equal to threshold
                                        break;
    #if !defined KINETIS_KE_ADC
                                    case (ADC_LOW_LIMIT_INT | ADC_HIGH_LIMIT_INT): // if the low value is less that the high value it results in a trigger when the value is higher or lower than the thresholds - if the low value is higher than th high value it rsults in a trigger if the value is between the thresholds
                                        ptrADC->ADC_CV1 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_low_level_trigger, 0); // set low range threshold
                                        ptrADC->ADC_CV2 = fnConvertADCvalue(ptrADC, ptrADC_settings->int_high_level_trigger, 1); // set high range threshold
                                        ptrADC->ADC_SC2 &= ~ADC_SC2_ACFGT;
                                        ptrADC->ADC_SC2 |= (ADC_SC2_ACFE | ADC_SC2_ACREN); // enable compare function on range
                                        break;
    #endif
                                    }
                                }
                            }
    #if !defined DEVICE_WITHOUT_DMA
                        }
    #endif
                  //}
    #if defined KINETIS_KE_ADC
                    ucChannelConfig |= (unsigned char)(ptrADC_settings->int_adc_bit & ADC_SC1A_ADCH_OFF); // the channel being configured
                    if (ptrADC_settings->int_adc_bit < ADC_SC1A_ADCH_VSS) { // if configuring a channel that uses a pin
                        ptrADC->ADC_APCTL1 |= (ADC_APCTL1_AD0 << ptrADC_settings->int_adc_bit); // enable the pin's ADC function
                    }
    #else
                    ptrADC->ADC_SC1A = ucChannelConfig;                  // start conversion if software mode or enable triggered conversion
    #endif
                }
    #if defined KINETIS_KE_ADC                                           // {2}
                else {
                    ucChannelConfig |= (ptrADC->ADC_SC1A & ~(ADC_SC1A_ADCO | ADC_SC1A_ADCO));
                }
                ptrADC->ADC_SC1A = ucChannelConfig;                      // start conversion if software mode or enable triggered conversion
    #endif
            }
            if (((ptrADC_settings->int_adc_mode & (ADC_GET_RESULT | ADC_CHECK_CONVERSION)) != 0) && (ptrADC_settings->int_adc_result != 0)) { // if there is a result structure
    #if defined _WINDOWS
                switch (ptrADC_settings->int_adc_controller) {
                case 0:
                    if (IS_POWERED_UP(6, ADC0) == 0) {
                        _EXCEPTION("Trying to read from ADC0 that is not powered up!");
                    }
                    break;
        #if ADC_CONTROLLERS > 1
                case 1:
            #if defined KINETIS_K22_SF7
                    if (IS_POWERED_UP(6, ADC1) == 0)
            #else
                    if (IS_POWERED_UP(3, ADC1) == 0)
            #endif
                    {
                        _EXCEPTION("Trying to read from ADC1 that is not powered up!");
                    }
                    break;
        #endif
        #if ADC_CONTROLLERS > 2
                case 2:
                    if (IS_POWERED_UP(6, ADC2) == 0) {
                        _EXCEPTION("Trying to read from ADC2 that is not powered up!");
                    }
                    break;
        #endif
        #if ADC_CONTROLLERS > 3
                case 3:
                    if (IS_POWERED_UP(3, ADC3) == 0) {
                        _EXCEPTION("Trying to read from ADC3 that is not powered up!");
                    }
                    break;
        #endif
                }
    #endif
                if (((ptrADC_settings->int_adc_mode & ADC_CHECK_CONVERSION) != 0) || ((ptrADC->ADC_SC1A & ADC_SC1A_AIEN) == 0)) { // {6} check the conversion state or a read interrupt, or read without interrupt operation
                    while ((ptrADC->ADC_SC1A & ADC_SC1A_COCO) == 0) {    // wait for conversion to complete
    #if defined _WINDOWS
                        if ((ptrADC->ADC_SC1A & ADC_SC1A_ADCH_OFF) != ADC_SC1A_ADCH_OFF) { // if the converter hasn't been disabled
                            ptrADC->ADC_SC1A |= ADC_SC1A_COCO;           // set conversion complete flag
                        }
    #endif
                        if ((ptrADC_settings->int_adc_mode & ADC_CHECK_CONVERSION) != 0) { // if we are not to wait
                            ptrADC_settings->int_adc_result->ucADC_status[0] = ADC_RESULT_NOT_READY; // result is presently not ready
                            return;
                        }
                        if ((ptrADC->ADC_SC1A & ADC_SC1A_ADCH_OFF) == ADC_SC1A_ADCH_OFF) { // {6} if the converter sub-system has been disabled we return the read value without waiting for conversion completion (this happens after single-shot interrupts disable the converter but the last result is in the result register)
                            break;
                        }
                    }
                }
                ptrADC_settings->int_adc_result->ucADC_status[0] = ADC_RESULT_VALID; // mark that the result is valid
                ptrADC_settings->int_adc_result->sADC_value[0] = (signed short)ptrADC->ADC_RA; // return the read value
            }
        }
#endif

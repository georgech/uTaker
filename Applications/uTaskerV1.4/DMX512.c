/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET
    
    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland
    
    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      DMX512.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined USE_DMX512_MASTER || defined USE_DMX512_SLAVE

#define OWN_TASK                  TASK_DMX512


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define DMX512_UART            5                                         // K66 LPUART0
#define DMX512_PERIOD          30000                                     // 30.000ms
#define DMX512_BREAK           100                                       // 100us break time
#define DMX512_MAB             16                                        // 16us MAB time
#define DMX_SLOT_COUNT         512                                       // maximum DMX512 data length
#define DMX512_TX_BUFFER_COUNT 2                                         // 2 buffers that can be prepared in advance

//#define TEST_SECOND_CHANNEL                                            // This doesn't work completely due to problem with UART1/FTM2-CH0 modulation error
#define DMX512_UART_1          1

#define _DMX512_BREAK          PIT_US_DELAY(DMX512_BREAK)
#define _DMX512_MAB            PIT_US_DELAY(DMX512_MAB)



// Interrupt events
//
#define DMX512_TX_FRAME_COMPLETED       1
#define DMX512_TX_NEXT_TRANSMISSION_0   2
#define DMX512_TX_NEXT_TRANSMISSION_1   3

// Hardware timer interrupt events
//
#define DMX512_START_MAB                1
#define DMX512_START_TX                 2


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnStartDelay(unsigned long ulDelay, unsigned char ucEvent);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */



/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned char ucDMX512_HW_event = 0;

// DMX512 transmit frame period interrupt
//
static void _frame_interrupt_0(void)
{
#if defined _WINDOWS
    if (ucDMX512_HW_event != 0) {                                        // ignore when frame interrupt arrives during actiity (simulator only)
        return;
    }
#endif
    fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_NEXT_TRANSMISSION_0));

  //fnStartBreak(DMX512_UART);                                           // send a break
  //fnStopBreak(DMX512_UART);                                            // queue a single break character (it is possible that 2 are sent [?] - depending on UART timing)
  //if ((SIM_SOPT5 & 0x00010000) == 0) {
  //    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT8, 0, (PORT_SRE_FAST | PORT_DSE_HIGH));
  //}
  //fnStartDelay(_DMX512_BREAK, DMX512_START_MAB);
  //fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_FRAME_COMPLETED));
}

#if defined TEST_SECOND_CHANNEL
// DMX512 transmit frame period interrupt
//
static void _frame_interrupt_1(void)
{
    fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_NEXT_TRANSMISSION_1));
}
#endif

// DMX512 hardware timer interrupt
//
static void _int_DMX512_HW_timer(void)
{
    switch (ucDMX512_HW_event) {
    case DMX512_START_MAB:
      //fnStopBreak(DMX512_UART);                                        // stop the break
        _CONFIG_PERIPHERAL(E, 8, (PE_8_LPUART0_TX | UART_PULL_UPS));     // LPUART0_TX on PE8 (alt. function 5)
        fnStartDelay(_DMX512_MAB, DMX512_START_TX);
        break;
    case DMX512_START_TX:
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT8, 0, (PORT_SRE_FAST | PORT_DSE_HIGH));
        _CONFIG_PERIPHERAL(E, 8, (PE_8_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PE8 (alt. function 5)
        ucDMX512_HW_event = 0;
        fnInterruptMessage(OWN_TASK, (unsigned char)(DMX512_TX_NEXT_TRANSMISSION_0));
        break;
    }
}

static void fnStartDelay(unsigned long ulDelay, unsigned char ucEvent)
{
    PIT_SETUP pit_setup;                                                 // PIT interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.mode = PIT_SINGLE_SHOT;
    pit_setup.ucPIT = 0;
    pit_setup.int_priority = PIT0_INTERRUPT_PRIORITY;                    // not used
    pit_setup.count_delay = ulDelay;
    pit_setup.int_handler = _int_DMX512_HW_timer;
    ucDMX512_HW_event = ucEvent;
    fnConfigureInterrupt((void *)&pit_setup);                            // configure PIT
}

// DMX512 task
//
extern void fnDMX512(TTASKTABLE *ptrTaskTable)
{
    static QUEUE_HANDLE    DMX512_PortID[2] = {NO_ID_ALLOCATED};
    static unsigned char   ucDMX512_tx_buffer[DMX_SLOT_COUNT + 1 + 2];
    QUEUE_HANDLE           PortIDInternal = ptrTaskTable->TaskID;       // queue ID for task input
    unsigned char          ucInputMessage[SMALL_QUEUE];                 // reserve space for receiving messages

    if (DMX512_PortID[0] == NO_ID_ALLOCATED) {
        unsigned char ucMode;
        TTYTABLE tInterfaceParameters;                                   // table for passing information to driver
        tInterfaceParameters.Channel = DMX512_UART;
        tInterfaceParameters.ucSpeed = SERIAL_BAUD_250K;                 // fixed DMX512 baud rate
        tInterfaceParameters.Config = (CHAR_8 | NO_PARITY | TWO_STOPS | CHAR_MODE | UART_HW_TRIGGERED_TX_MODE); // fixed DMX512 settings
    #if defined USE_DMX512_SLAVE
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 1024;             // input buffer size
        ucMode = FOR_READ;
    #else
        tInterfaceParameters.Rx_tx_sizes.RxQueueSize = 0;
    #endif
    #if defined USE_DMX512_MASTER
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = (sizeof(ucDMX512_tx_buffer) * DMX512_TX_BUFFER_COUNT); // output buffer size
        ucMode = FOR_WRITE;
    #else
        tInterfaceParameters.Rx_tx_sizes.TxQueueSize = 0;
    #endif
        tInterfaceParameters.Task_to_wake = OWN_TASK;                    // wake self when messages have been received
        tInterfaceParameters.ucDMAConfig = (UART_TX_DMA | UART_RX_DMA | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_BREAK);                  // activate DMA on transmission
        if (NO_ID_ALLOCATED == (DMX512_PortID[0] = fnSetNewSerialMode(&tInterfaceParameters, ucMode))) { // open serial port for I/O
            return;                                                      // if the serial port could not be opened we quit
        }
        else {
    #if defined USE_DMX512_MASTER
            // Set up a periodic timer to control the frame rate
            //
            // This method is suitable for K66, using a PWM channel to control the break and period, plus a second channel to define the MAB
            //
            static unsigned long enable_lpuart_tx = 0;
        #if defined TEST_SECOND_CHANNEL
            static unsigned char enable_uart_tx = 0;
        #endif
            int i;
            PWM_INTERRUPT_SETUP pwm_setup;
            fnDriver(DMX512_PortID[0], (TX_OFF), MODIFY_TX);             // disable the transmitter since we will be preparing output and then starting by DMA enable
            ucDMX512_tx_buffer[0] = (unsigned char)((DMX_SLOT_COUNT + 1) >> 8); // individual message content length
            ucDMX512_tx_buffer[1] = (unsigned char)(DMX_SLOT_COUNT + 1);
            ucDMX512_tx_buffer[2] = 0;
            i = 3;
            while (i < sizeof(ucDMX512_tx_buffer)) {
                ucDMX512_tx_buffer[i] = (unsigned char)(i - 3);
                i++;
            }
            fnWrite(DMX512_PortID[0], ucDMX512_tx_buffer, (DMX_SLOT_COUNT + 1 + 2)); // prime first DMX512 frame
            ucDMX512_tx_buffer[2] = 1;
            fnWrite(DMX512_PortID[0], ucDMX512_tx_buffer, (DMX_SLOT_COUNT + 1 + 2)); // prime second DMX512 frame
        #if DMX512_UART == 5 && (defined KINETIS_65 || defined KINETIS_K66)
            SIM_SOPT5 |= SIM_SOPT5_LPUART0TXSRC_TPM1_0;                  // use modulation method for break
        #endif

        #if defined TEST_SECOND_CHANNEL
            tInterfaceParameters.Channel = DMX512_UART_1;
            DMX512_PortID[1] = fnSetNewSerialMode(&tInterfaceParameters, FOR_I_O);
            fnDriver(DMX512_PortID[1], (TX_OFF), MODIFY_TX);             // disable the transmitter since we will be preparing output and then starting by DMA enable
            ucDMX512_tx_buffer[2] = 2;
            fnWrite(DMX512_PortID[1], ucDMX512_tx_buffer, (DMX_SLOT_COUNT + 1 + 2)); // prime first DMX512 frame
            ucDMX512_tx_buffer[2] = 3;
            fnWrite(DMX512_PortID[1], ucDMX512_tx_buffer, (DMX_SLOT_COUNT + 1 + 2)); // prime second DMX512 frame
            #if DMX512_UART_1 == 1 && (defined KINETIS_K65 || defined KINETIS_K66)
            SIM_SOPT5 |= SIM_SOPT5_UART1TXSRC_FTM2_0;                    // use modulation method for break
            #endif
        #endif

            // TPM1 setup
            //
            pwm_setup.int_type = PWM_INTERRUPT;
            pwm_setup.pwm_mode = (PWM_SYS_CLK | PWM_PRESCALER_16 | PWM_EDGE_ALIGNED | PWM_POLARITY/* | PWM_NO_OUTPUT*/); // clock PWM timer from the system clock with /16 pre-scaler
            pwm_setup.int_handler = 0;
            pwm_setup.pwm_reference = (_TPM_TIMER_1 | 0);                // timer module 1, channel 0
            pwm_setup.pwm_frequency = (unsigned short)PWM_TPM_CLOCK_US_DELAY(DMX512_PERIOD, 16); // generate frame rate frequency on PWM output
            pwm_setup.pwm_value = ((pwm_setup.pwm_frequency * DMX512_BREAK)/DMX512_PERIOD); // output starts low (inverted polarity) and goes high after the break time
            fnConfigureInterrupt((void *)&pwm_setup);                    // configure and start
            pwm_setup.pwm_reference = (_TPM_TIMER_1 | 1);                // timer module , channel 1
            pwm_setup.pwm_value = ((pwm_setup.pwm_frequency * (DMX512_BREAK + DMX512_MAB - 44))/DMX512_PERIOD); // second channel goes high after the MAB period to control the timing for the start of the frame transmission
            pwm_setup.int_handler = _frame_interrupt_0;                  // interrupt call-back on PWM cycle
            pwm_setup.int_priority = PRIORITY_HW_TIMER;                  // interrupt priority of cycle interrupt
            pwm_setup.pwm_mode |= (PWM_DMA_CHANNEL_ENABLE | PWM_FULL_BUFFER_DMA | PWM_FULL_BUFFER_DMA_AUTO_REPEAT | PWM_DMA_SPECIFY_DESTINATION | PWM_DMA_SPECIFY_LONG_WORD); // we use this to trigger DMA
            pwm_setup.dma_int_handler = 0;
            pwm_setup.ucDmaChannel = 1;
            pwm_setup.usDmaTriggerSource = DMAMUX0_CHCFG_SOURCE_TPM1_C1; // trigger on own match
            pwm_setup.ulPWM_buffer_length = sizeof(enable_lpuart_tx);
            pwm_setup.ptrPWM_Buffer = &enable_lpuart_tx;
            enable_lpuart_tx = LPUART0_CTRL;                             // get the idle value in the LPUART's control register
            enable_lpuart_tx |= LPUART_CTRL_TE;                          // construct the value to enable the transmission
            pwm_setup.ptrRegister = LPUART0_CTRL_ADD;                    // copy to the LPUART0 control register
            fnConfigureInterrupt((void *)&pwm_setup);                    // configure and start

        #if defined TEST_SECOND_CHANNEL
            // FTM1 setup uses core clock for both FTM and UART1
            //
            pwm_setup.pwm_mode = (PWM_SYS_CLK | PWM_PRESCALER_128 | PWM_EDGE_ALIGNED | PWM_POLARITY/* | PWM_NO_OUTPUT*/); // clock PWM timer from the system clock with /128 pre-scaler
            pwm_setup.int_handler = 0;
            pwm_setup.pwm_reference = (_FTM_TIMER_2 | 0);                // timer module 2, channel 0
            pwm_setup.pwm_frequency = (unsigned short)PWM_TIMER_US_DELAY(DMX512_PERIOD, 128); // generate frame rate frequency on PWM output
            pwm_setup.pwm_value = ((pwm_setup.pwm_frequency * DMX512_BREAK)/DMX512_PERIOD); // output starts low (inverted polarity) and goes high after the break time
            fnConfigureInterrupt((void *)&pwm_setup);                    // configure and start
            pwm_setup.pwm_reference = (_FTM_TIMER_2 | 1);                // timer module 2, channel 1
            pwm_setup.pwm_value = ((pwm_setup.pwm_frequency * (DMX512_BREAK + DMX512_MAB - 48)) / DMX512_PERIOD); // second channel goes high after the MAB period to control the timing for the start of the frame transmission
            pwm_setup.int_handler = _frame_interrupt_1;                  // interrupt call-back on PWM cycle
            pwm_setup.int_priority = PRIORITY_HW_TIMER;                  // interrupt priority of cycle interrupt
            pwm_setup.pwm_mode |= (PWM_DMA_CHANNEL_ENABLE | PWM_FULL_BUFFER_DMA | PWM_FULL_BUFFER_DMA_AUTO_REPEAT | PWM_DMA_SPECIFY_DESTINATION | PWM_DMA_SPECIFY_BYTE); // we use this to trigger DMA
            pwm_setup.dma_int_handler = 0;
            pwm_setup.ucDmaChannel = 0;
            pwm_setup.usDmaTriggerSource = DMAMUX0_CHCFG_SOURCE_FTM2_C1; // trigger on own match
            pwm_setup.ulPWM_buffer_length = sizeof(enable_uart_tx);
            pwm_setup.ptrPWM_Buffer = &enable_uart_tx;
            enable_uart_tx = UART1_C2;                                   // get the idle value in the LPUART's control register
            enable_uart_tx |= (UART_C2_TE | UART_C2_TIE);                // construct the value to enable the transmission with interrupt enabled (needed for DMA)
            pwm_setup.ptrRegister = UART1_C2_ADD;                        // copy to the UART0 control register
            fnConfigureInterrupt((void *)&pwm_setup);                    // configure and start
        #endif
    #endif
        }
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on message source
        case TIMER_EVENT:
            switch (ucInputMessage[MSG_TIMER_EVENT]) {
            case 0:
                break;
            default:
                break;
            }
            break;

        case INTERRUPT_EVENT:
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
    #if defined USE_DMX512_MASTER
            case DMX512_TX_FRAME_COMPLETED:
                fnStartBreak(DMX512_UART);                               // send a break
                fnStopBreak(DMX512_UART);                                // queue a single break character (it is possible that 2 are sent - depending on UART timing)
              //_CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT8, 0, (PORT_SRE_FAST | PORT_DSE_HIGH));
                fnStartDelay(_DMX512_BREAK, DMX512_START_MAB);
                break;
            case DMX512_TX_NEXT_TRANSMISSION_0:
            case DMX512_TX_NEXT_TRANSMISSION_1:
                if (fnWrite(DMX512_PortID[ucInputMessage[MSG_INTERRUPT_EVENT] - DMX512_TX_NEXT_TRANSMISSION_0], 0, (DMX_SLOT_COUNT + 1 + 2)) > 0) { // if there is a free buffer space
                    ucDMX512_tx_buffer[2]++;
                    fnWrite(DMX512_PortID[ucInputMessage[MSG_INTERRUPT_EVENT] - DMX512_TX_NEXT_TRANSMISSION_0], ucDMX512_tx_buffer, (DMX_SLOT_COUNT + 1 + 2)); // prepare next DMX512 frame
                }// prepare next DMX512 frame
              //if (iDMXState == 1) {
              //    iDMXState = 2;
              //}
              //else {
                  //_CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, PORTE_BIT8, 0, (PORT_SRE_FAST | PORT_DSE_HIGH));
                  //_CONFIG_PERIPHERAL(E, 8, (PE_8_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PE8 (alt. function 5)
              //    ucDMX512_tx_buffer[2]++;
              //    fnWrite(DMX512_PortID, ucDMX512_tx_buffer, (DMX_SLOT_COUNT + 1 + 2)); // prepare next DMX512 frame
              //}
                break;
    #endif
            default:
                break;
            }
            break;

        default:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // flush any unexpected messages (assuming they arrived from another task)
            break;
        }
    }
}

#endif
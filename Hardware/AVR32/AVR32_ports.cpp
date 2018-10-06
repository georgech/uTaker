/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        AVR32ports.c
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    18.11.2009 Added BGA pin numbers and added _AT32UC3B
    09.11.2010 Correct PX04/EBI-DATA[10] to PX04/EBI-DATA[6]
    19.06.2011 Add _AT32UC3C
    30.11.2012 Add _AT32UC3C 64 pinout

 */

#include "config.h"

#if defined _HW_AVR32

#define BUF1SIZE 200

#if _VC80_UPGRADE>=0x0600
    #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define STRCAT strcat
#endif

#if defined _AT32UC3B
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    #ifdef CHIP_48_PIN                                                   // 48 pin TQFP/QFN (28 GPIO)
    // PORT 0 (0..31)
    { "3 {PA00/TDI}", "4 {PA01/TDO}", "5 {PA02/TMS}", "7 {PA03/ADC-AD[0]/PM-GCLK[0]/USBB-USB_ID}", 
      "8 {PA04/ADC-AD[1]/PM-GCLK[1]/USBB-USB_VBOF}", "9 {PA05/EIC-EXTINT[0]/ADC-AD[2]/USART1-DCD}", "10 {PA06/EIC-EXTINT[1]/ADC-AD[3]/USART1-DSR}", "11 {PA07/PWM-PWM[0]/ADC-AD[4]/USART1-DTR}",
      "12 {PA08/PWM-PWM[1]/ADC-AD[5]/USART1-RI}", "20 {PA09/TWI-SCL/SPI-NPCS[2]/USART1-CTS}", "21 {PA10/TWI-SDA/SPI-NPCS[3]/USART1-RTS}", "22 {PA11/USART0-RTS/TC-A2/PWM-PWM[0]/(xin32)}",
      "23 {PA12/USART0-CTS/TC-B2/PWM-PWM[1]/(xout32)}", "25 {PA13/NMI/PWM-PWM[2]/USART0-CLK}", "26 {PA14/SPI-MOSI/PWM-PWM[3]/EIC-EXTINT[2]}", "27 {PA15/SPI-SCK/PWM-PWM[4]}",
      "28 {PA16/SPI-NPCS[0]/TC-CLK1}", "29 {PA17/SPI-NPCS[1]/TC-CLK2/SPI-SCK}", "30 {PA18/USART0-RXD/PWM-PWM[5]/SPI-MISO/(xin0)}", "31 {PA19/USART0-TXD/PWM-PWM[6]/SPI-MOSI/(xout0)}",
      "32 {PA20/USART1-CLK/TC-CLK0}", "33 {PA21/PWM-PWM[2]/TC-A1}", "34 {PA22/PWM-PWM[6]/TC-B1/ADC-TRIGGER}", "35 {PA23/USART1-TXD/SPI-NPCS[1]/EIC-EXTINT[3]}",
      "43 {PA24/USART1-RXD/SPI-NPCS[0]/EIC-EXTINT[4]}", "44 {PA25/SPI-MISO/PWM-PWM[3]/EIC-EXTINT[5]}", "45 {PA26/USBB-USB_ID/TC-A0}", "46 {PA27/USBB-USB_VBOF/TC-B0}",
      "NA", "NA", "NA", "NA"},

    #else                                                                // 64 pin TQFP/QFN (44 GPIO)
    // PORT 0 (0..31)
    { "3 {PA00/TDI}", "4 {PA01/TDO}", "5 {PA02/TMS}", "9 {PA03/ADC-AD[0]/PM-GCLK[0]/USBB-USB_ID}", 
      "10 {PA04/ADC-AD[1]/PM-GCLK[1]/USBB-USB_VBOF}", "11 {PA05/EIC-EXTINT[0]/ADC-AD[2]/USART1-DCD}", "12 {PA06/EIC-EXTINT[1]/ADC-AD[3]/USART1-DSR}", "13 {PA07/PWM-PWM[0]/ADC-AD[4]/USART1-DTR}",
      "14 {PA08/PWM-PWM[1]/ADC-AD[5]/USART1-RI}", "28 {PA09/TWI-SCL/SPI-NPCS[2]/USART1-CTS}", "29 {PA10/TWI-SDA/SPI-NPCS[3]/USART1-RTS}", "30 {PA11/USART0-RTS/TC-A2/PWM-PWM[0]/(xin32)}",
      "31 {PA12/USART0-CTS/TC-B2/PWM-PWM[1]/(xout32)}", "33 {PA13/NMI/PWM-PWM[2]/USART0-CLK}", "34 {PA14/SPI-MOSI/PWM-PWM[3]/EIC-EXTINT[2]}", "35 {PA15/SPI-SCK/PWM-PWM[4]/USART2-CLK}",
      "36 {PA16/SPI-NPCS[0]/TC-CLK1}", "37 {PA17/SPI-NPCS[1]/TC-CLK2/SPI-SCK}", "39 {PA18/USART0-RXD/PWM-PWM[5]/SPI-MISO/(xin0)}", "40 {PA19/USART0-TXD/PWM-PWM[6]/SPI-MOSI/(xout0)}",
      "44 {PA20/USART1-CLK/TC-CLK0/USART2-RXD}", "45 {PA21/PWM-PWM[2]/TC-A1/USART2-TXD}", "46 {PA22/PWM-PWM[6]/TC-B1/ADC-TRIGGER}", "47 {PA23/USART1-TXD/SPI-NPCS[1]/EIC-EXTINT[3]}",
      "59 {PA24/USART1-RXD/SPI-NPCS[0]/EIC-EXTINT[4]}", "60 {PA25/SPI-MISO/PWM-PWM[3]/EIC-EXTINT[5]}", "61 {PA26/USBB-USB_ID/USART2-TXD/TC-A0}", "62 {PA27/USBB-USB_VBOF/USART2-RXD/TC-B0}",
      "41 {PA28/USART0-CLK/PWM-PWM[4]/SPI-MISO/(xin1)}", "42 {PA29/TC-CLK0/TC-CLK1/SPI-MOSI/(xout1)}", "15 {PA30/ADC-AD[6]/EIC-SCAN[0]/PM-GCLK[2]}", "16 {PA31/ADC-AD[7]/EIC-SCAN[1]}"},

    // PORT 1 (32..63)
    { "6 {PB00/TC-A0/EIC-SCAN[2]/USART2-CTS}", "7 {PB01/TC-B0/EIC-SCAN[3]/USART2-RTS}", "24 {PB02/EIC-EXTINT[6]/TC-A1/USART1-TXD}", "25 {PB03/EIC-EXTINT[7]/TC-B1/USART1-RXD}",
      "26 {PB04/USART1-CTS/SPI-NPCS[3]/TC-CLK2}", "27 {PB05/USART1-RTS/SPI-NPCS[2]/PWM-PWM[5]}", "38 {PB06/SSC-RX_CLOCK/USART1-DCD/EIC-SCAN[4]}", "43 {PB07/SSC-RX_DATA/USART1-DSR/EIC-SCAN[5]}", "54 {PB08/SSC-RX_FRAME_SYNC/USART1-DTR/EIC-SCAN[6]}",
      "55 {PB09/SSC-TX_CLOCK/USART1-RI/EIC-SCAN[7]}", "57 {PB10/SSC-TX_DATA/TC-A2/USART0-RXD}", "58 {PB11/SSC-TX_FRAME_SYNC/TC-B2/USART0-TXD}",
      "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    #endif
};
#elif defined _AT32UC3C
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    #if defined CHIP_64_PIN                                              // 64 pin TQFP/QFN (44 GPIO)
    // PORT 0 (0..31)
    { "1 {PA00/CANIF-TXLINE[1]}", "2 {PA01/CANIF-RXLINE[1]/PEVC_PAD_EVT[0]}", "3 {PA02/SCIF-GCLK[0]/PEVC-PAD_EVT[1]}", "4 {PA03/SCIF-GCLK[1]/EIC-EXTINT[1]}", 
      "7 {PA04/ADCIN0/USBC-ID/ACIFA0-ACAOUT}", "8 {PA05/ADCIN1/USBC-VBOF/ACIFA0-ACBOUT}", "9 {PA06/ADCIN2/AC1AP1/PEVC-PAD_EVT[2]}", "10 {PA07/ADCIN3/AC1AN1/PEVC-PAD_EVT[3]}",
      "11 {PA08/ADCIN4/AC1BP1/EIC-EXTINT[2]}", "12 {PA09/ADCIN5/AC1BN1}", "NA", "NA",
      "NA", "NA", "NA", "NA",
      "13 {PA16/ADCREF0/DACREF}", "NA", "NA", "16 {PA19/ADCIN8/EIC-EXTINT[1]}",
      "19 {PA20/ADCIN9/AC0AP0/DAC0A}", "20 {PA21/ADCIN10/AC0BN0/DAC0B}", "21 {PA22/ADCIN11/AC0AN0/PEVC-PAD_EVT[4]/MACB-SPEED}", "22 {PA23/ADCIN12/AC0BP0/PEVC-PAD_EVT[5]/MACB-WOL}",
      "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA"},

    // PORT 1 (32..63)
    { "62 {PB00/USART0-CLK/CANIF-RXLINE[1]/EIC-EXTINT[8]/PEVC-PAD_EVT[10]/(xin32)}", "63 {PB01/CANIF-TXLINE[1]/PEVC-PAD_EVT[11]/(xout32)}", "NA", "NA",
      "NA", "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA", "31 {PB30/(xin0)}", "32 {PB31/(xout0)}"},

    // PORT 2 (64..95)
    { "NA", "NA", "33 {PC02/TWIMS0-TWD/SPI0-NPCS[3]/USART2-RXD/TC1-CLK1/MACB-MDC", "34 {PC03/TWIMS0-TWCK/EIC-EXTINT[1]/USART2-TXD/TC1-B1/MACB-MDIO}", "37 {PC04/TWIMS1-TWD/EIC-EXTINT[3]/USART2-TXD/TC0-B1}", "38 {PC05/TWIMS1-TWCK/EIC-EXTINT[4]/USART2-RXD/TC0-A2}",
      "NA", "NA", "NA", "NA",
      "NA", "NA", "NA", "NA",
      "NA", "39 {PC15/PWM-PWMH[1]/SPI0-NPCS[0]/USART0-RXD/CANIF-RXLINE[1]}", "40 {PC16/PWM-PWML[1]/SPI0-NPCS[1]/USART0-TXD/CANIF-TXLINE[1]", "41 {PC17/PWM-PWMH[0]/SPI0-NPCS[2]/IISC-ISDO/USART3-TXD}",
      "42 {PC18/PWM-PWML[0]/EIC-EXTINT[5]/IISC-ISDI/USART3-RXD}", "42 {PC19/PWM-PWML[2]/SCIF-GCLK[0]/IISC-IMCK/USART3-CTS}", "44 {PC20/PWM-PWMH[2]/SCIF-GCLK[1]/IISC-ISCK/USART3-RTS}",
      "45 {PC21/PWM-EXT_FAULTS[0]/CANIF-RXLINE[0]/IISC-IWS}", "46 {PC22/PWM-EXT_FAULTS[1]/CANIF-TXLINE[0]/USART3-CLK}", "NA", "NA",
      "NA", "NA", "NA", "NA", "NA",
      "NA", "NA"},

    // PORT 3 (96..105)
    { "47 {PD00/SPI0-MOSI/TC1-CLK0/QDEC0-QEPI/USART0-TXD}", "48 {PD01/SPI0-MISO/TC1-A0/TC0-CLK1/USART0-RXD}", "49 {PD02/SPI0-CLK/TC0-CLK2/QDEC0-QEPA}", "50 {PD03/SPI0-NPCS[0]/TC0-B2/QDEC0-QEPB}", "NA",
      "NA", "NA", "NA", "NA", "NA",
      "NA", "53 {PD11/USART1-TXD/USBC-ID/PEVC-PAD_EVT[6]/MACB-TXD[0]}", "54 {PD12/USART1-RXD/USBC-VBOF/PEVC-PAD_EVT[7]/MACB-TXD[1]}", "55 {PD13/USART1-CTS/USART1-CLK/PEVC-PAD_EVT[8]/MACB-RXD[0]}", "56 {PD14/USART1-RTS/EIC-EXTINT[7]/PEVC-PAD_EVT[9]/MACB-RXD[1]}",
      "NA", "NA", "NA", "NA", "NA",
      "NA", "57 {PD21/USART3-TXD/EIC-EXTINT[0]/QDEC1-QEPI}", "NA", "NA", "NA",
      "NA", "NA", "58 {PD27/USART0-TXD/CANIF-RXLINE[0]/TC0-A0/MACB-RX_ER}", "59 {PD28/USART0-RXD/CANIF-TXLINE[0]/TC0-B0/MACB-RX_DV}", "60 {PD29/USART0-CTS/EIC-EXTINT[6]/USART0-CLK/TC0-CLK0/MACB-TX_CLK}",
      "61 {PD30/USART0-RTS/EIC-EXTINT[3]/TC0-A1/MACB-TX_EN}", "NA"
    },

    #elif defined CHIP_100_PIN                                           // 100 pin TQFP (69 GPIO)
    // TO DO
    #else                                                                // 144 pin LQFP or 144 pin BGA (109 GPIO)
    // PORT 0 (0..31)
    { "1 {PA00/CANIF-TXLINE[1]}", "2 {PA01/CANIF-RXLINE[1]/PEVC_PAD_EVT[0]}", "3 {PA02/SCIF-GCLK[0]/PEVC-PAD_EVT[1]}", "4 {PA03/SCIF-GCLK[1]/EIC-EXTINT[1]}", 
      "21 {PA04/ADCIN0/USBC-ID/ACIFA0-ACAOUT}", "22 {PA05/ADCIN1/USBC-VBOF/ACIFA0-ACBOUT}", "23 {PA06/ADCIN2/AC1AP1/PEVC-PAD_EVT[2]}", "24 {PA07/ADCIN3/AC1AN1/PEVC-PAD_EVT[3]}",
      "25 {PA08/ADCIN4/AC1BP1/EIC-EXTINT[2]}", "26 {PA09/ADCIN5/AC1BN1}", "27 {PA10/ADCIN6/EIC-EXTINT[4]/PEVC-PAD_EVT[13]}", "28 {PA11/ADCIN7/ADCREF1/PEVC-PAD_EVT[14]}",
      "29 {PA12/AC1AP0/SPI0-NPCS[0]/DAC1A}", "30 {PA13/AC1AN0/SPI0-NPCS[1]/ADCIN15}", "31 {PA14/AC1BP0/SPI1-NPCS[0]}", "31 {PA15/AC1BN0/SPI1-NPCS[1]/DAC1B}",
      "33 {PA16/ADCREF0/DACREF}", "NA", "NA", "36 {PA19/ADCIN8/EIC-EXTINT[1]}",
      "39 {PA20/ADCIN9/AC0AP0/DAC0A}", "40 {PA21/ADCIN10/AC0BN0/DAC0B}", "41 {PA22/ADCIN11/AC0AN0/PEVC-PAD_EVT[4]/MACB-SPEED}", "42 {PA23/ADCIN12/AC0BP0/PEVC-PAD_EVT[5]/MACB-WOL}",
      "43 {PA24/ADCIN13/SPI1-NPCS[2]}", "44 {PA25/ADCIN14/SPI1-NPCS[3]/EIC-EXTINT[0]}", "45 {PA26/AC0AP1/EIC-EXTINT[1]}", "46 {PA27/AC0AN1/EIC-EXTINT[2]}",
      "47 {PA28/AC0BP1/EIC-EXTINT[3]}", "48 {PA29/AC0BN1/EIC-EXTINT[0]}", "NA", "NA"},

    // PORT 1 (32..63)
    { "140 {PB00/USART0-CLK/CANIF-RXLINE[1]/EIC-EXTINT[8]/PEVC-PAD_EVT[10]/(xin32)}", "141 {PB01/CANIF-TXLINE[1]/PEVC-PAD_EVT[11]/(xout32)}", "143 {PB02/USBC-ID/PEVC-PAD_EVT[6]/TC1-A1/(xin1)}", "144 {PB03/USBC-VBOF/PEVC-PAD_EVT[7]/(xout1)}",
      "7 {PB04/SPI1-MOSI/CANIF-RXLINE[0]/QDEC1-QEPI/MACB-TXD[2]}", "8 {PB05/SPI1-MISO/CANIF-TXLINE[0]/PEVC-PAD_EVT[12]/USART3_CLK/MACB-TXD[3]}", "9 {PB06/SPI1-SCK/QDEC1-QEPA/USART1-CLK/MACB-TX_ER}", "10 {PB07/SPI1-NPCS[0]/EIC-EXTINT[2]/QDEC1-QEPB/MACB-RX_DV}", "11 {PB08/SPI1-NPCS[1]/PEVC-PAD_EVT[1]/PWM-PWML[0]/MACB-RXD[0]}",
      "12 {PB09/SPI1-NPCS[2]/PWM-PWMH[0]/MACB-RXD[1]}", "13 {PB10/USART1-DTR/SPI0-MOSI/PWM-PWML[1]}", "14 {PB11/USART1-DSR/SPI0-MISO/PWM-PWMH[1]}", "15 {PB12/USART1-DCD/SPI0-SCK/PWM-PWML[2]}",
      "16 {PB13/USART1-RI/SPI0-NPCS[0]/PWM-PWMH[2]/MACB-RX_ER}", "17 {PB14/USART1-RTS/SPI0-NPCS[1]/PWM-PWML[3]/MACB-MDC}", "18 {PB15/USART1-CTS/USART1-CLK/PWM-PWMH[3]/MACB-MDIO}", "19 {PB16/USART1-RXD/SPI0-NPCS[2]/PWM-EXT_FAULTS[0]/CANIF-RXLINE[0]}",
      "20 {PB17/USART1-TXD/SPI0-NPCS[3]/PWM-EXT_FAULTS[1]/CANIF-TXLINE[0]}", "57 {PB18/TC0-CLK2/EIC-EXTINT[4]}", "58 {PB19/TC0-A0/SPI1-MOSI/IISC-ISD0/MACB-CRS}", "59 {PB20/TC0-B0/SPI1-MISO/IISC-ISDI/ACIFA1-ACAOUT/MACB-COL}",
      "60 {PB21/TC0-CLK1/SPI1-SCK/IISC-IMCK/ACIFA1-ACBOUT/MACB-RXD[2]}", "61 {PB22/TC0-A1/SPI1-NPCS[3]/IISC-ISCK/SCIF-GCLK[0]/MACB-RXD[3]}", "62 {PB23/TC0-B1/SPI1-NPCS[2]/IISC-IWS/SCIF-GCLK[1]/MACB-RX_CLK}", "63 {PB24/TC0-CLK0/SPI1-NPCS[1]}", "64 {PB25/TC0-A2/SPI1-NPCS[0]/PEVC-PAD_EVT[8]}",
      "65 {PB26/TC0-B2/SPI1-SCK/PEVC-PAD_EVT[9]/MACB-TX_EN}", "66 {PB27/QDEC0-QEPA/SPI1-MISO/PEVC-PAD_EVT[10]/TC1-CLK0/MACB-TXD[0]}", "67 {PB28/QDEC0-QEPB/SPI1-MOSI/PEVC-PAD_EVT[11]/TC1-B0/MACB-TXD[1]}", "68 {PB29/QDEC0-QEPI/SPI0-NPCS[0]/PEVC-PAD_EVT[12]/TC1-A0}", "69 {PB30/(xin0)}", "70 {PB31/(xout0)}"},

    // PORT 2 (64..95)
    { "71 {PC00/USBC-ID/SPI0-NPCS[1]/USART2-CTS/TC1-B2/CANIF-TXLINE[1]}", "72 {PC01/USBC-VBOF/SPI0-NPCS[2]/USART2-RTS/TC1-A2/CANIF-RXLINE[1]}", "73 {PC02/TWIMS0-TWD/SPI0-NPCS[3]/USART2-RXD/TC1-CLK1/MACB-MDC", "74 {PC03/TWIMS0-TWCK/EIC-EXTINT[1]/USART2-TXD/TC1-B1/MACB-MDIO}", "77 {PC04/TWIMS1-TWD/EIC-EXTINT[3]/USART2-TXD/TC0-B1}", "78 {PC05/TWIMS1-TWCK/EIC-EXTINT[4]/USART2-RXD/TC0-A2}",
      "79 {PC06/PEVC-PAD_EVT[15]/USART2-CLK/USART2-CTS//TC0-CLK2/TWIMS2-TWD/TWIMS0-TWALM}", "80 {PC07/PEVC-PAD_EVT[2]/EBI-NCS[3]/USART2-RTS/TC0-B2/TWIMS2-TWCL/TWIMS1-TWALM}", "81 {PC08/PEVC-PAD_EVT[13]/SPI1-NPCS[1]/EBI-NCS[0]/USART4-TXD}", "82 {PC09/PEVC-PAD_EVT[14]/SPI1-NPCS[2]/EBI-ADDR[23]/USART4-RXD}",
      "83 {PC10/PEVC-PAD_EVT[15]/SPI1-NPCS[3]/EBI-ADDR[22]}", "84 {PC11/PWM-PWMH[3]/CANIF-RXLINE[1]/EBI-ADDR[21]/TC0-CLK0}", "85 {PC12/PWM-PWML[3]/CANIF-TXLINE[1]/EBI-ADDR[20]/USART2-CLK}", "86 {PC13/PWM-PWMH[2]/EIC-EXTINT[7]/USART0-RTS}",
      "87 {PC14/PWM-PWML[2]/USART0-CLK/EBI-SDCKE/USART0-CTS}", "88 {PC15/PWM-PWMH[1]/SPI0-NPCS[0]/EBI-SDWE/USART0-RXD/CANIF-RXLINE[1]}", "89 {PC16/PWM-PWML[1]/SPI0-NPCS[1]/EBI-CAS/USART0-TXD/CANIF-TXLINE[1]", "90 {PC17/PWM-PWMH[0]/SPI0-NPCS[2]/EBI-RAS/IISC-ISDO/USART3-TXD}",
      "91 {PC18/PWM-PWML[0]/EIC-EXTINT[5]/EBI-SDA10/IISC-ISDI/USART3-RXD}", "92 {PC19/PWM-PWML[2]/SCIF-GCLK[0]/EBI-DATA[0]/IISC-IMCK/USART3-CTS}", "93 {PC20/PWM-PWMH[2]/SCIF-GCLK[1]/EBI-DATA[1]/IISC-ISCK/USART3-RTS}",
      "94 {PC21/PWM-EXT_FAULTS[0]/CANIF-RXLINE[0]/EBI-DATA[2]/IISC-IWS}", "95 {PC22/PWM-EXT_FAULTS[1]/CANIF-TXLINE[0]/EBI-DATA[3]/USART3-CLK}", "96 {PC23/QDEC1-QEPB/CANIF-RXLINE[1]/EBI-DATA[4]/PEVC-PAD_EVT[3]}", "97 {PC24/QDEC1-QEPA/CANIF-TXLINE[1]/EBI-DATA[5]/PEVC-PAD_EVT[4]}",
      "98 {PC25/TC1-CLK2/EBI-DATA[6]/SCIF-GCLK[0]/USART4-TXD}", "99 {PC26/QDEC1-QEPI/TC1-B2/EBI-DATA[7]/SCIF-GCLK[1]/USART4-RXD}", "100 {PC27/TC1-A2/EBI-DATA[8]/ECI-EXTINT[0]/USART4-CTS}", "101 {PC28/SPI1-NPCS[3]/TC1-CLK1/EBI-DATA[9]/USART4-RTS}", "102 {PC29/SPI0-NPCS[1]/TC1-B1/EBI-DATA[10]}",
      "105 {PC30/SPI0-NPCS[2]/TC1-A1/EBI-DATA[11]}", "106 {PC31/SPI0-NPCS[3]/TC1-B0/EBI-DATA[12]/PEVC-PAD_EVT[5]/USART4-CLK}"},

    // PORT 3 (96..105)
    { "107 {PD00/SPI0-MOSI/TC1-CLK0/EBI-DATA[13]/QDEC0-QEPI/USART0-TXD}", "108 {PD01/SPI0-MISO/TC1-A0/EBI-DATA[14]/TC0-CLK1/USART0-RXD}", "109 {PD02/SPI0-CLK/TC0-CLK2/EBI-DATA[15]/QDEC0-QEPA}", "110 {PD03/SPI0-NPCS[0]/TC0-B2/EBI-ADDR[0]/QDEC0-QEPB}", "111 {PD04/SPI0-MOSI/EBI-ADDR[1]}",
      "112 {PD05/SPI0-MISO/EBI-ADDR[2]}", "113 {PD06/SPI0-SCK/EBI-ADDR[3]}", "114 {PD07/USART1-DTR/EIC-EXTINT[5]/EBI-ADDR[4]/QDEC0-QEPI/USART4-TXD}", "115 {PD08/USART1-DSR/EIC-EXTINT[6]/EBI-ADDR[5]/TC1-CLK2/USART4-RXD}", "116 {PD09/USART1-DCD/CANIF-RXLINE[0]/EBI-ADDR[6]/QDEC0-QEPA/USART4-CTS}",
      "117 {PD10/USART1-RI/CANIF-TXLINE[0]/EBI-ADDR[7]/QDEC0-QEPB/USART4-RTS}", "120 {PD11/USART1-TXD/USBC-ID/EBI-ADDR[8]/PEVC-PAD_EVT[6]/MACB-TXD[0]}", "121 {PD12/USART1-RXD/USBC-VBOF/EBI-ADDR[9]/PEVC-PAD_EVT[7]/MACB-TXD[1]}", "122 {PD13/USART1-CTS/USART1-CLK/EBI-SDCK/PEVC-PAD_EVT[8]/MACB-RXD[0]}", "123 {PD14/USART1-RTS/EIC-EXTINT[7]/EBI-ADDR[10]/PEVC-PAD_EVT[9]/MACB-RXD[1]}",
      "124 {PD15/TC0-A0/USART3-TXD/EBI-ADDR[11]}", "125 {PD16/TC0-B0/USART3-RXD/EBI-ADDR[12]}", "126 {PD17/TC0-A1/USART3-CTS/EBI-ADDR[12]/USART3-CLK}", "127 {PD18/TC0-B1/USART3-RTS/EBI-ADDR[14]}", "128 {PD19/TC0-A2/EBI-ADDR[15]}",
      "129 {PD20/TC0-B2/EBI-ADDR[16]}", "130 {PD21/USART3-TXD/EIC-EXTINT[0]/EBI-ADDR[17]/QDEC1-QEPI}", "131 {PD22/USART3-RXD/TC0-A2/EBI-ADDR[18]/SCIF-GCLK[0]}", "132 {PD23/USART3-CTS/USART3-CLK/EBI-ADDR[19]/QDEC1-QEPA}", "133 {PD24/USART3-RTS/EIC-EXTINT[8]/EBI-NWE1/QDEC1-QEPB}",
      "134 {PD25/TC0-CLK0/USBC-ID/EBI-NWE0/USART4-CLK}", "135 {PD26/TC0-CLK1/USBC-VBOF/EBI-NRD}", "136 {PD27/USART0-TXD/CANIF-RXLINE[0]/EBI-NCS[1]/TC0-A0/MACB-RX_ER}", "137 {PD28/USART0-RXD/CANIF-TXLINE[0]/EBI-NCS[2]/TC0-B0/MACB-RX_DV}", "138 {PD29/USART0-CTS/EIC-EXTINT[6]/USART0-CLK/TC0-CLK0/MACB-TX_CLK}",
      "139 {PD30/USART0-RTS/EIC-EXTINT[3]/EBI-NWAIT/TC0-A1/MACB-TX_EN}", "NA"
    },
    #endif
};
#else                                                                    // AT32UC3A
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {
    #ifdef CHIP_100_PIN                                                  // 100 pin TQFP (69 GPIO)
    // PORT 0 (0..31)
    { "19 {PA00/RXD0/TC-CLK0}", "20 {PA01/TXD0/TC-CLK1}", "23 {PA02/SCK0/TC-CLK2}", "24 {PA03/RTS0/EIM-EXTINT[4]/DAC-DATA[0]}", 
      "25 {PA04/CTS0/EIM-EXTINT[5]/DAC-DATAN[0]}", "26 {PA05/RXD1/PWM[4]}", "27 {PA06/TXD1/PWM[5]}", "28 {PA07/SCK1/GCLK[0]/SPI0-NPSC3}",
      "29 {PA08/RTS1/SPI0_NPCS[1]/EIM-EXTINT[7]}", "30 {PA09/CTS1/SPI0-NPCS[2]/MACB-WOL}", "31 {PA10/SPI0-NPCS[0]/EIM-EXTINT[6]}", "33 {PA11/SPI0-MISO/USB_ID}",
      "36 {PA12/SPI0-MOSI/USB_VBOF}", "37 {PA13/SPI0-SCK}", "39 {PA14/TX_FRAME_SYNC/SPI1-NPC[0]/EBI-NCS[0]}", "40 {PA15/TX_CLOCK/SPI1-SCK/EBI-ADDR[20]}",
      "41 {PA16/TX_DATA/SPI1_MOSI/EBI-ADDR[21]}", "42 {PA17/RX_DATA/SPI1-MISO/EBI-ADDR[22]}", "43 {PA18/RX_CLOCK/SPI1-NPCS[1]/MACB-WOL}", "44 {PA19/RX_FRAME_SYNC/SPI1-NPCS[2]}",
      "45 {PA20/EIM-EXTINT[8]/SPI1-NPCS[3]}", "51 {PA21/AD[0]/EIM-EXTINT[0]/USB_ID}", "52 {PA22/AD[1]/EIM-EXTINT[1]/USB_VBOF}", "53 {PA23/AD[2]/EIM-EXTINT[2]/DAC-DATA[1]}",
      "54 {PA24/AD[3]/EIM-EXTINT[3]/DAC-DATAN[1]}", "55 {PA25/AD[4]/EIM-SCAN[0]/EBI-NCS[0]}", "56 {PA26/AD[5]/EIM-SCAN[1]/EBI-ADDR[20]}", "57 {PA27/AD[6]/EIM-SCAN[2]/EBI-ADDR[21]}",
      "58 {PA28/AD[7]/EIM-SCAN[3]/EBI-ADDR[22]}", "83 {PA29/TWI-SDA/RTS2}", "84 {PA30/TWI-SCL/CTS2}", "NA"},

    // PORT 1 (32..63)
    { "65 {PB00/MACB-TX_CLK/RTS2/RTS3}", "66 {PB01/MACB-TX_EN/CTS2/CTS3}", "70 {PB02/MACB-TXD[0]/DAC-DATA[0]}", "71 {PB03/MACB-TXD[1]/DAC-DATAN[0]}",
      "72 {PB04/MACB-CRS/CLK3/EBI-NCS[3]}", "73 {PB05/MACB-RXD[0]/DAC-DATA[1]}", "74 {PB06/MACB-RXD[1]/DAC-DATAN[1]}", "75 {PB07/MACB-RX_ER}", "76 {PB08/MACB-MDC}",
      "77 {PB09/MACB-MDIO}", "78 {PB10/MACB-TXD[2]/RXD3/EBI-SDCK}", "81 {PB11/MACB-TXD[3]/TXD3/EBI-SDCKE}", "82 {PB12/MACB-TX_ER/TC-CLK0/EBI-RAS}",
      "87 {PB13/MACB-RXD[2]/TC-CLK1/EBI-CAS}", "88 {PB14/MACB-RXD[3]/TC-CLK2/EBI-SDWE}", "95 {PB15/MACB-RX_DV}", "96 {PB16/MACB-COL/USB_ID/EBI-SDA10}",
      "98 {PB17/MACB-RX_CLK/USB_VBOF/EBI-ADDR[23]}", "99 {PB18/MACB-SPEED/ADC-TRIGGER/PWM[6]}", "100 {PB19/PWM[0]/GCLK[0]/EIM-SCAN[4]}", "1 {PB20/PWM[1]/GCLK[1]/EIM-SCAN[5]}",
      "2 {PB21/PWM[2]/GCLK[2]/EIM-SCAN[6]}", "3 {PB22/PWM[3]/GCLK[3]/EIM-SCAN[7]}", "6 {PB23/TC-A0/DCD1}", "7 {PB24/TC-B0/DSR1}", "8 {PB25/TC-A1/DTR1}",
      "9 {PB26/TC-B1/RI1}", "10 {PB27/TC-A2/PWM[4]}", "14 {PB28/TC-B2/PWM[5]}", "15 {PB29/RXD2/GCLK[1]/EBI-NCS[2]}", "16 {PB30/TXD2/GCLK[2]/EBI-SDCS}", "17 {PB31/CLK2/GCLK[3]/NWAIT}"},

    // PORT 2 (64..95)
    { "63 {PC00/(xin32)}", "64 {PC01/(xout32)}", "85 {PC02/(xin0)}", "86 {PC03/(xout0)}", "93 {PC04/(xin1)}", "94 {PC05/(xout1)}",
      "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},

    #else                                                                // 144 pin LQFP or 144 pin BGA (109 GPIO)
    // PORT 0 (0..31)
    { "25-J6 {PA00/RXD0/TC-CLK0}", "27-H6 {PA01/TXD0/TC-CLK1}", "30-K8 {PA02/SCK0/TC-CLK2}", "32-J7 {PA03/RTS0/EIM-EXTINT[4]/DAC-DATA[0]}", 
      "34-J8 {PA04/CTS0/EIM-EXTINT[5]/DAC-DATAN[0]}", "39-K11 {PA05/RXD1/PWM[4]}", "41-L12 {PA06/TXD1/PWM[5]}", "43-J10 {PA07/SCK1/GCLK[0]/SPI0-NPSC3}",
      "45-H10 {PA08/RTS1/SPI0_NPCS[1]/EIM-EXTINT[7]}", "47-J12 {PA09/CTS1/SPI0-NPCS[2]/MACB-WOL}", "48-H7 {PA10/SPI0-NPCS[0]/EIM-EXTINT[6]}", "50-G11 {PA11/SPI0-MISO/USB_ID}",
      "53-G10 {PA12/SPI0-MOSI/USB_VBOF}", "54-G9 {PA13/SPI0-SCK}", "56-F12 {PA14/TX_FRAME_SYNC/SPI1-NPC[0]/EBI-NCS[0]}", "57-F11 {PA15/TX_CLOCK/SPI1-SCK/EBI-ADDR[20]}",
      "58-G8 {PA16/TX_DATA/SPI1_MOSI/EBI-ADDR[21]}", "60-F9 {PA17/RX_DATA/SPI1-MISO/EBI-ADDR[22]}", "62-D9 {PA18/RX_CLOCK/SPI1-NPCS[1]/MACB-WOL}", "64-C12 {PA19/RX_FRAME_SYNC/SPI1-NPCS[2]}",
      "66-D10 {PA20/EIM-EXTINT[8]/SPI1-NPCS[3]}", "73-B10 {PA21/AD[0]/EIM-EXTINT[0]/USB_ID}", "74-B12 {PA22/AD[1]/EIM-EXTINT[1]/USB_VBOF}", "75-A12 {PA23/AD[2]/EIM-EXTINT[2]/DAC-DATA[1]}",
      "76-A11 {PA24/AD[3]/EIM-EXTINT[3]/DAC-DATAN[1]}", "77-A10 {PA25/AD[4]/EIM-SCAN[0]/EBI-NCS[0]}", "78-A9 {PA26/AD[5]/EIM-SCAN[1]/EBI-ADDR[20]}", "79-B9 {PA27/AD[6]/EIM-SCAN[2]/EBI-ADDR[21]}",
      "80-A8 {PA28/AD[7]/EIM-SCAN[3]/EBI-ADDR[22]}", "122-C3 {PA29/TWI-SDA/RTS2}", "123-F1 {PA30/TWI-SCL/CTS2}", "NA"},

    // PORT 1 (32..63)
    { "88-B6 {PB00/MACB-TX_CLK/RTS2/RTS3}", "90-A6 {PB01/MACB-TX_EN/CTS2/CTS3}", "96-A4 {PB02/MACB-TXD[0]/DAC-DATA[0]}", "98-A5 {PB03/MACB-TXD[1]/DAC-DATAN[0]}",
      "100-B4 {PB04/MACB-CRS/CLK3/EBI-NCS[3]}", "102-A3 {PB05/MACB-RXD[0]/DAC-DATA[1]}", "104-B3 {PB06/MACB-RXD[1]/DAC-DATAN[1]}", "106-A2 {PB07/MACB-RX_ER}", "111-B1 {PB08/MACB-MDC}",
      "113-C1 {PB09/MACB-MDIO}", "115-E1 {PB10/MACB-TXD[2]/RXD3/EBI-SDCK}", "119-D1 {PB11/MACB-TXD[3]/TXD3/EBI-SDCKE}", "121-D3 {PB12/MACB-TX_ER/TC-CLK0/EBI-RAS}",
      "126-D2 {PB13/MACB-RXD[2]/TC-CLK1/EBI-CAS}", "127-F2 {PB14/MACB-RXD[3]/TC-CLK2/EBI-SDWE}", "134-J3 {PB15/MACB-RX_DV}", "136-F4 {PB16/MACB-COL/USB_ID/EBI-SDA10}",
      "139-J2 {PB17/MACB-RX_CLK/USB_VBOF/EBI-ADDR[23]}", "141-L3 {PB18/MACB-SPEED/ADC-TRIGGER/PWM[6]}", "143-K3 {PB19/PWM[0]/GCLK[0]/EIM-SCAN[4]}", "3-K4 {PB20/PWM[1]/GCLK[1]/EIM-SCAN[5]}",
      "5-L1 {PB21/PWM[2]/GCLK[2]/EIM-SCAN[6]}", "6-M1 {PB22/PWM[3]/GCLK[3]/EIM-SCAN[7]}", "9-M2 {PB23/TC-A0/DCD1}", "11-L4 {PB24/TC-B0/DSR1}", "13-M3 {PB25/TC-A1/DTR1}",
      "14-M4 {PB26/TC-B1/RI1}", "15-M6 {PB27/TC-A2/PWM[4]}", "19-M7 {PB28/TC-B2/PWM[5]}", "20-K6 {PB29/RXD2/GCLK[1]/EBI-NCS[2]}", "21-K7 {PB30/TXD2/GCLK[2]/EBI-SDCS}", "22-L7 {PB31/CLK2/GCLK[3]/NWAIT}"},

    // PORT 2 (64..95)
    { "85-A7 {PC00/(xin32)}", "86-B7 {PC01/(xout32)}", "124-C4 {PC02/(xin0)}", "125-G2 {PC03/(xout0)}", "132-K2 {PC04/(xin1)}", "133-K1 {PC05/(xout1)}",
      "120-F3 {PX34/EBI-ADDR[0]/SPI1-MISO}", "118-C2 {PX33/EBI-ADDR[1]/SPI0-NPCS[3]}", "114-E3 {PX32/EBI-ADDR[2]/SPI0-NPCS[2]}", "112-E4 {PX31/EBI-ADDR[3]/SPI0-NPCS[1]}",
      "110-D4 {PX30/EBI-ADDR[4]/SPI0-NPCS[0]}", "107-D5 {PX29/EBI-ADDR[5]/SPI0-SCK}", "105-C5 {PX28/EBI-ADDR[6]/SPI0-MOSI}", "103-E6 {PX27/EBI-ADDR[7]/SPI0-MISO}",
      "101-C6 {PX26/EBI-ADDR[8]/EIM-SCAN[7]}", "99-D6 {PX25/EBI-ADDR[9]/EIM-SCAN[6]}", "97-D7 {PX24/EBI-ADDR[10]/EIM-SCAN[5]}", "95-E7 {PX23/EBI-ADDR[11]/EIM-SCAN[4]}",
      "91-C7 {PX22/EBI-ADDR[12]/EIM-SCAN[3]/TC-CLK2}", "89-C8 {PX21/EBI-ADDR[13]/EIM-SCAN[2]/TC-CLK1}", "87-D8 {PX20/EBI-ADDR[14]/EIM-SCAN[1]/TC-CLK0}",
      "67-C11 {PX19/EBI-ADDR[15]/EIM-SCAN[0]/TC-B2}", "65-E9 {PX18/EBI-ADDR[16]/TC-A2}", "63-E10 {PX17/EBI-ADDR[17]/TC-B2}", "61-F8 {PX16/EBI-ADDR[18]/CTS3/TC-A1}",
      "59-F10 {PX15/EBI-ADDR[19]/RTS3/TC-B0}", "38-L10 {PX10/EBI-DATA[0]/RXD2}", "35-M11 {PX09/EBI-DATA[1]/TXD3}", "33-K9 {PX08/EBI-DATA[2]/RXD3}", "31-M10 {PX07/EBI-DATA[3]/RTS1}",
      "26-L9 {PX06/EBI-DATA[4]/CTS1}", "24-M9 {PX05/EBI-DATA[5]/TXD1}"},

    // PORT 3 (96..105)
    { "12-L6 {PX04/EBI-DATA[6]/RXD0}", "10-M5 {PX03/EBI-DATA[7]/RTS0}", "4-K5 {PX02/EBI-DATA[8]/CTS0}", "2-J5 {PX01/EBI-DATA[9]/TXD0}", "1-J4 {PX00/EBI-DATA[10]/RXD0}",
      "144-H4 {PX39/EBI-DATA[11]/SPI1-NPCS[2]}", "142-H3 {PX38/EBI-DATA[12]/SPI1-NPCS[1]}", "140-G5 {PX37/EBI-DATA[13]/SPI1-NPCS[0]}", "137-G3 {PX36/EBI-DATA[14]/SPI1-SCK}",
      "135-G4 {PX35/EBI-DATA[15]/SPI1-MOSI}", "46-J9 {PX14/EBI-NCS[1]/TC-A0}", "44-J11 {PX13/EBI-NRD/RTS2}", "42-K12 {PX12/EBI-NWE0/CTS2}", "40-H9 {PX11/EBI-NWE1/TXD2}",
      "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    #endif
};
#endif

#if defined _AT32UC3B
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][5] = {
    {
        //  Q    Periph. A        Periph. B       Periph. C        (fictional Periph. D clock function)
        {  "Q", "INVALID",        "INVALID",      "INVALID",       "NA"  },// PORT 0
        {  "Q", "INVALID",        "INVALID",      "INVALID",       "NA"  },
        {  "Q", "INVALID",        "INVALID",      "INVALID",       "NA"  },
        {  "Q", "ADC-AD[0]",      "PM-GCLK[0]",   "USBB-USB_ID",   "NA"  },
        {  "Q", "ADC-AD[1]",      "PM-GCLK[1]",   "USBB-USB_VBOF", "NA"  },
        {  "Q", "EIC-EXTINT[0]",  "ADC-AD[2]",    "USART1-DCD",    "NA"  },
        {  "Q", "EIC-EXTINT[1]",  "ADC-AD[3]",    "USART1-DSR",    "NA"  },
        {  "Q", "PWM-PWM[0]",     "ADC-AD[4]",    "USART1-DTR",    "NA"  },
        {  "Q", "PWM-PWM[1]",     "ADC-AD[5]",    "USART1-RI",     "NA"  },
        {  "Q", "TWI-SCL",        "SPI-NPCS[2]",  "USART1-CTS",    "NA"  },
        {  "Q", "TWI-SDA",        "SPI-NPCS[3]",  "USART1-RTS",    "NA"  },
        {  "Q", "USART0-RTS",     "TC-A2",        "PWM-PWM[0]",    "xin32"},
        {  "Q", "USART0-CTS",     "TC-B2",        "PWM-PWM[1]",    "xout32"},
        {  "Q", "NMI",            "PWM-PWM[2]",   "USART0-CLK",    "NA"  },
        {  "Q", "SPI-MOSI",       "PWM-PWM[3]",   "EIC-EXTINT[2]", "NA"  },
    #if defined CHIP_48_PIN
        {  "Q", "SPI-SCK",        "PWM-PWM[4]",   "INVALID",       "NA"  },
    #else
        {  "Q", "SPI-SCK",        "PWM-PWM[4]",   "USART2-CLK",    "NA"  },
    #endif
        {  "Q", "SPI-NPCS[0]",    "TC-CLK1",      "INVALID",       "NA"  },
        {  "Q", "SPI-NPCS[1]",    "TC-CLK2",      "SPI-SCK",       "NA"  },
        {  "Q", "USART0-RXD",     "PWM-PWM[5]",   "SPI-MISO",      "xin0"},
        {  "Q", "USART0-TXD",     "PWM-PWM[6]",   "SPI-MOSI",      "xout0"},
    #if defined CHIP_48_PIN
        {  "Q", "USART1-CLK",     "TC-CLK0",      "INVALID",       "NA"  },
        {  "Q", "PWM-PWM[2]",     "TC-A1",        "INVALID",       "NA"  },
    #else
        {  "Q", "USART1-CLK",     "TC-CLK0",      "USART2-RXD",    "NA"  },
        {  "Q", "PWM-PWM[2]",     "TC-A1",        "USART2-TXD",    "NA"  },
    #endif
        {  "Q", "PWM-PWM[6]",     "TC-B1",        "ADC-TRIGGER",   "NA"  },
        {  "Q", "USART1-TXD",     "SPI-NPCS[1]",  "EIC-EXTINT[3]", "NA"  },
        {  "Q", "USART1-RXD",     "SPI-NPCS[0]",  "EIC-EXTINT[4]", "NA"  },
        {  "Q", "SPI-MISO",       "PWM-PWM[3]",   "EIC-EXTINT[5]", "NA"  },
    #if defined CHIP_48_PIN
        {  "Q", "USBB-USB_ID",    "INVALID",      "TC-A0",         "NA"  },
        {  "Q", "USBB-USB_VBOF",  "INVALID",      "TC-B0",         "NA"  },
    #else
        {  "Q", "USBB-USB_ID",    "USART2-TXD",   "TC-A0",         "NA"  },
        {  "Q", "USBB-USB_VBOF",  "USART2-RXD",   "TC-B0",         "NA"  },
    #endif
        {  "Q", "USART0-CLK",     "PWM-PWM[4]",   "SPI-MISO",      "xin1" },
        {  "Q", "TC-CLK0",        "TC-CLK1",      "SPI-MOSI",      "xout1"},
        {  "Q", "ADC-AD[6]",      "EIC-SCAN[0]",  "PM-GCLK[2]",    "NA"  },
        {  "Q", "ADC-AD[7]",      "EIC-SCAN[1]",  "INVALID",       "NA"  }
    },
    #ifndef CHIP_48_PIN
    {
        //  Q    Periph. A           Periph. B        Periph. C
        {  "Q", "TC-A0",             "EIC-SCAN[2]",   "USART2-CTS",  "NA"  }, // PORT 1
        {  "Q", "TC-B0",             "EIC-SCAN[3]",   "USART2-RTS",  "NA"  },
        {  "Q", "EIC-EXTINT[6]",     "TC-A1",         "USART1-TXD",  "NA"  },
        {  "Q", "EIC-EXTINT[7]",     "TC-B1",         "USART1-RXD",  "NA"  },
        {  "Q", "USART1-CTS",        "SPI-NPCS[3]",   "TC-CLK2",     "NA"  },
        {  "Q", "USART1-RTS",        "SPI-NPCS[2]",   "PWM-PWM[5]",  "NA"  },
        {  "Q", "SSC-RX_CLOCK",      "USART1-DCD",    "EIC-SCAN[4]", "NA"  },
        {  "Q", "SSC-RX_DATA",       "USART1-DSR",    "EIC-SCAN[5]", "NA"  },
        {  "Q", "SSC-RX_FRAME_SYNC", "USART1-DTR",    "EIC-SCAN[6]", "NA"  },
        {  "Q", "SSC-TX_CLOCK",      "USART1-RI",     "EIC-SCAN[7]", "NA"  },
        {  "Q", "SSC-TX_DATA",       "TC-A2",         "USART0-RXD",  "NA"  },
        {  "Q", "SSC-TX_FRAME_SYNC", "TC-B2",         "USART0-TXD",  "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
        {  "Q", "NA",                "NA",            "NA",          "NA"  },
    }
    #endif
};
#elif defined _AT32UC3C
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][10] = {
    #if defined CHIP_64_PIN                                              // 64 pin TQFP/QFN (44 GPIO)
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)
        {  "Q", "INVALID",      "CANIF-TXLINE[1]","INVALID",     "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  }, // PORT 0
        {  "Q", "INVALID",      "CANIF-RXLINE[1]","PEVC_PAD_EVT[0]","INVALID", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SCIF-GCLK[0]", "INVALID",       "PEVC-PAD_EVT[1]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SCIF-GCLK[1]", "EIC-EXTINT[1]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN0",       "USBC-ID",       "ACIFA0-ACAOUT","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN1",       "USBC-VBOF",     "ACIFA0-ACBOUT","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN2",       "AC1AP1",        "PEVC-PAD_EVT[2]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN3",       "AC1AN1",        "PEVC-PAD_EVT[3]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN4",       "AC1BP1",        "EIC-EXTINT[2]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN5",       "AC1BN1",        "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN6",       "EIC-EXTINT[4]", "PEVC-PAD_EVT[13]","INVALID", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN7",       "ADCREF1ADCREF1","PEVC-PAD_EVT[14]","INVALID", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1AP0",       "SPI0-NPCS[0]",  "DAC1A",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1AN0",       "SPI0-NPCS[1]",  "ADCIN15",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1BP0",       "SPI1-NPCS[0]",  "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1BN0",       "SPI1-NPCS[1]",  "DAC1B",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCREF0",      "INVALID",       "DACREF",       "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN8",       "EIC-EXTINT[1]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN9",       "AC0AP0",        "DAC0A",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN10",      "AC0BN0",        "DAC0B",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN11",      "AC0AN0",        "PEVC-PAD_EVT[4]","INVALID",  "MACB-SPEED", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN12",      "AC0BP0",        "PEVC-PAD_EVT[5]","INVALID",  "MACB-WOL",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN13",      "SPI1-NPCS[2]",  "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN14",      "SPI1-NPCS[3]",  "EIC-EXTINT[0]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0AP1",       "EIC-EXTINT[1]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0AN1",       "EIC-EXTINT[2]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0BP1",       "EIC-EXTINT[3]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0BN1",       "EIC-EXTINT[0]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)
        {  "Q", "USART0-CLK",   "CANIF-RXLINE[1]","EIC-EXTINT[8]","PEVC-PAD_EVT[10]","INVALID","INVALID","INVALID","INVALID", "xin32"}, // PORT 1
        {  "Q", "INVALID",      "CANIF-TXLINE[1]","INVALID",     "PEVC-PAD_EVT[11]","INVALID","INVALID","INVALID", "INVALID", "xout32"},
        {  "Q", "INVALID",      "USBC-ID",       "PEVC-PAD_EVT[6]","TC1-A1",   "INVALID",    "INVALID", "INVALID", "INVALID", "xin1"},
        {  "Q", "INVALID",      "USBC-VBOF",     "PEVC-PAD_EVT[7]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "xout1"},
        {  "Q", "SPI1-MOSI",    "CANIF-RXLINE[0]","QDEC1-QEPI",  "INVALID",    "MACB-TXD[2]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-MISO",    "CANIF-TXLINE[0]","PEVC-PAD_EVT[12]","USART3_CLK","MACB-TXD[3]","INVALID","INVALID","INVALID","NA"  },
        {  "Q", "SPI1-SCK",     "INVALID",       "QDEC1-QEPA",   "USART1-CLK", "MACB-TX_ER", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[0]", "EIC-EXTINT[2]", "QDEC1-QEPB",   "INVALID",    "MACB-RX_DV", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[1]", "PEVC-PAD_EVT[1]","PWM-PWML[0]", "INVALID",    "MACB-RXD[0]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[2]", "INVALID",       "PWM-PWMH[0]",  "INVALID",    "MACB-RXD[1]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DTR",   "SPI0-MOSI",     "PWM-PWML[1]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DSR",   "SPI0-MISO",     "PWM-PWMH[1]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DCD",   "SPI0-SCK",      "PWM-PWML[2]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RI",    "SPI0-NPCS[0]",  "PWM-PWMH[2]",  "INVALID",    "MACB-RX_ER", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RTS",   "SPI0-NPCS[1]",  "PWM-PWML[3]",  "INVALID",    "MACB-MDC",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-CTS",   "USART1-CLK",    "PWM-PWMH[3]",  "INVALID",    "MACB-MDIO",  "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RXD",   "SPI0-NPCS[2]",  "PWM-EXT_FAULTS[0]","INVALID","CANIF-RXLINE[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-TXD",   "SPI0-NPCS[3]",  "PWM-EXT_FAULTS[1]","INVALID","CANIF-TXLINE[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "TC0-CLK2",     "INVALID",       "EIC-EXTINT[4]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A0",       "SPI1-MOSI",     "IISC-ISD0",    "INVALID",    "MACB-CRS",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B0",       "SPI1-MISO",     "IISC-ISDI",    "ACIFA1-ACAOUT","MACB-COL", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK1",     "SPI1-SCK",      "IISC-IMCK",    "ACIFA1-ACBOUT","MACB-RXD[2]","INVALID","INVALID","INVALID", "NA"  },
        {  "Q", "TC0-A1",       "SPI1-NPCS[3]",  "IISC-ISCK",    "SCIF-GCLK[0]","MACB-RXD[3]","INVALID","INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B1",       "SPI1-NPCS[2]",  "IISC-IWS",     "SCIF-GCLK[1]","MACB-RX_CLK","INVALID","INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK0",     "SPI1-NPCS[1]",  "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A2",       "SPI1-NPCS[0]",  "PEVC-PAD_EVT[8]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B2",       "SPI1-SCK",      "PEVC-PAD_EVT[9]","INVALID",  "MACB-TX_EN", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC0-QEPA",   "SPI1-MISO",     "PEVC-PAD_EVT[10]","TC1-CLK0","MACB-TXD[0]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC0-QEPB",   "SPI1-MOSI",     "PEVC-PAD_EVT[11]","TC1-B0",  "MACB-TXD[1]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC0-QEPI",   "SPI0-NPCS[0]",  "PEVC-PAD_EVT[12]","TC1-A0",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "xin0"},
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "xout0"}
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)      
        {  "Q", "USBC-ID",      "SPI0-NPCS[1]",  "USART2-CTS",   "TC1-B2",     "CANIF-TXLINE[1]","INVALID","INVALID","INVALID","NA" }, // PORT 2
        {  "Q", "USBC-VBOF",    "SPI0-NPCS[2]",  "USART2-RTS",   "TC1-A2",     "CANIF-RXLINE[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "TWIMS0-TWD",   "SPI0-NPCS[3]",  "USART2-RXD",   "TC1-CLK1",   "MACB-MDC",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TWIMS0-TWCK",  "EIC-EXTINT[1]", "USART2-TXD",   "TC1-B1",     "MACB-MDIO",  "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TWIMS1-TWD",   "EIC-EXTINT[3]", "USART2-TXD",   "TC0-B1",     "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TWIMS1-TWCK",  "EIC-EXTINT[4]", "USART2-RXD",   "TC0-A2",     "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PEVC-PAD_EVT[15]","USART2-CLK", "USART2-CTS",   "TC0-CLK2",   "TWIMS2-TWD", "TWIMS0-TWALM","INVALID","INVALID","NA"},
        {  "Q", "PEVC-PAD_EVT[2]","INVALID",  "USART2-RTS",   "TC0-B2",     "TWIMS2-TWCL","TWIMS1-TWALM","INVALID","INVALID","NA"},
        {  "Q", "PEVC-PAD_EVT[13]","SPI1-NPCS[1]","INVALID",  "INVALID",    "USART4-TXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PEVC-PAD_EVT[14]","SPI1-NPCS[2]","INVALID","INVALID",    "USART4-RXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PEVC-PAD_EVT[15]","SPI1-NPCS[3]","INVALID","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWMH[3]",  "CANIF-RXLINE[1]","INVALID","TC0-CLK0",   "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWML[3]",  "CANIF-TXLINE[1]","INVALID","USART2-CLK", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWMH[2]",  "EIC-EXTINT[7]", "INVALID",      "USART0-RTS", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWML[2]",  "USART0-CLK",    "INVALID",    "USART0-CTS", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWMH[1]",  "SPI0-NPCS[0]",  "INVALID",     "USART0-RXD", "CANIF-RXLINE[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "PWM-PWML[1]",  "SPI0-NPCS[1]",  "INVALID",      "USART0-TXD", "CANIF-TXLINE[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "PWM-PWMH[0]",  "SPI0-NPCS[2]",  "INVALID",      "IISC-ISDO",  "INVALID",    "USART3-TXD","INVALID","INVALID","NA"  },
        {  "Q", "PWM-PWML[0]",  "EIC-EXTINT[5]", "INVALID",    "IISC-ISDI",  "INVALID",    "USART3-RXD","INVALID","INVALID","NA"  },
        {  "Q", "PWM-PWML[2]",  "SCIF-GCLK[0]",  "INVALID",  "IISC-IMCK",  "INVALID",    "USART3-CTS","INVALID","INVALID","NA"  },
        {  "Q", "PWM-PWMH[2]",  "SCIF-GCLK[1]",  "INVALID",  "IISC-ISCK",  "INVALID",    "USART3-RTS","INVALID","INVALID","NA"  },
        {  "Q", "PWM-EXT_FAULTS[0]","CANIF-RXLINE[0]","INVALID","IISC-IWS","INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-EXT_FAULTS[1]","CANIF-TXLINE[0]","INVALID","INVALID", "USART3-CLK", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC1-QEPB",   "CANIF-RXLINE[1]","INVALID", "PEVC-PAD_EVT[3]","INVALID","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC1-QEPA",   "CANIF-TXLINE[1]","INVALID", "PEVC-PAD_EVT[4]","INVALID","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "TC1-CLK2",      "INVALID",  "SCIF-GCLK[0]","USART4-TXD","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC1-QEPI",   "TC1-B2",        "INVALID",  "SCIF-GCLK[1]","USART4-RXD","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "TC1-A2",        "INVALID",  "ECI-EXTINT[0]","USART4-CTS","INVALID","INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[3]", "TC1-CLK1",      "INVALID",  "INVALID",    "USART4-RTS", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[1]", "TC1-B1",        "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[2]", "TC1-A1",        "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[3]", "TC1-B0",        "INVALID", "PEVC-PAD_EVT[5]","USART4-CLK","INVALID","INVALID","INVALID","NA"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)
        {  "Q", "SPI0-MOSI",    "TC1-CLK0",      "INVALID", "QDEC0-QEPI", "USART0-TXD", "INVALID", "INVALID", "INVALID", "NA"  }, // PORT 3
        {  "Q", "SPI0-MISO",    "TC1-A0",        "INVALID", "TC0-CLK1",   "USART0-RXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-CLK",     "TC0-CLK2",      "INVALID", "QDEC0-QEPA", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[0]", "TC0-B2",        "INVALID",  "QDEC0-QEPB", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-MOSI",    "INVALID",       "INVALID",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-MISO",    "INVALID",       "INVALID",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-SCK",     "INVALID",       "INVALID",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DTR",   "EIC-EXTINT[5]", "INVALID",  "QDEC0-QEPI", "USART4-TXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DSR",   "EIC-EXTINT[6]", "INVALID",  "TC1-CLK2",   "USART4-RXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DCD",   "CANIF-RXLINE[0]","INVALID", "QDEC0-QEPA", "USART4-CTS", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RI",    "CANIF-TXLINE[0]","INVALID", "QDEC0-QEPB", "USART4-RTS", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-TXD",   "USBC-ID",       "INVALID",  "PEVC-PAD_EVT[6]","MACB-TXD[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-RXD",   "USBC-VBOF",     "INVALID",  "PEVC-PAD_EVT[7]","MACB-TXD[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-CTS",   "USART1-CLK",    "INVALID",     "PEVC-PAD_EVT[8]","MACB-RXD[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-RTS",   "EIC-EXTINT[7]", "INVALID", "PEVC-PAD_EVT[9]","MACB-RXD[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "TC0-A0",       "USART3-TXD",    "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B0",       "USART3-RXD",    "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A1",       "USART3-CTS",    "INVALID", "USART3-CLK", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B1",       "USART3-RTS",    "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A2",       "INVALID",       "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B2",       "INVALID",       "INVALID", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-TXD",   "EIC-EXTINT[0]", "INVALID", "QDEC1-QEPI", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-RXD",   "TC0-A2",        "INVALID", "SCIF-GCLK[0]","INVALID",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-CTS",   "USART3-CLK",    "INVALID", "QDEC1-QEPA", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-RTS",   "EIC-EXTINT[8]", "INVALID",     "QDEC1-QEPB", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK0",     "USBC-ID",       "INVALID",     "INVALID",    "USART4-CLK", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK1",     "USBC-VBOF",     "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-TXD",   "CANIF-RXLINE[0]","INVALID",  "TC0-A0",     "MACB-RX_ER", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-RXD",   "CANIF-TXLINE[0]","INVALID",  "TC0-B0",     "MACB-RX_DV", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-CTS",   "EIC-EXTINT[6]", "USART0-CLK",   "TC0-CLK0",   "MACB-TX_CLK","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-RTS",   "EIC-EXTINT[3]", "INVALID",    "TC0-A1",     "MACB-TX_EN", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  }
    },
    #else
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)
        {  "Q", "INVALID",      "CANIF-TXLINE[1]","INVALID",     "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  }, // PORT 0
        {  "Q", "INVALID",      "CANIF-RXLINE[1]","PEVC_PAD_EVT[0]","INVALID", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SCIF-GCLK[0]", "INVALID",       "PEVC-PAD_EVT[1]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SCIF-GCLK[1]", "EIC-EXTINT[1]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN0",       "USBC-ID",       "ACIFA0-ACAOUT","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN1",       "USBC-VBOF",     "ACIFA0-ACBOUT","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN2",       "AC1AP1",        "PEVC-PAD_EVT[2]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN3",       "AC1AN1",        "PEVC-PAD_EVT[3]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN4",       "AC1BP1",        "EIC-EXTINT[2]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN5",       "AC1BN1",        "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN6",       "EIC-EXTINT[4]", "PEVC-PAD_EVT[13]","INVALID", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN7",       "ADCREF1ADCREF1","PEVC-PAD_EVT[14]","INVALID", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1AP0",       "SPI0-NPCS[0]",  "DAC1A",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1AN0",       "SPI0-NPCS[1]",  "ADCIN15",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1BP0",       "SPI1-NPCS[0]",  "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC1BN0",       "SPI1-NPCS[1]",  "DAC1B",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCREF0",      "INVALID",       "DACREF",       "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN8",       "EIC-EXTINT[1]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN9",       "AC0AP0",        "DAC0A",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN10",      "AC0BN0",        "DAC0B",        "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN11",      "AC0AN0",        "PEVC-PAD_EVT[4]","INVALID",  "MACB-SPEED", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN12",      "AC0BP0",        "PEVC-PAD_EVT[5]","INVALID",  "MACB-WOL",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN13",      "SPI1-NPCS[2]",  "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "ADCIN14",      "SPI1-NPCS[3]",  "EIC-EXTINT[0]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0AP1",       "EIC-EXTINT[1]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0AN1",       "EIC-EXTINT[2]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0BP1",       "EIC-EXTINT[3]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "AC0BN1",       "EIC-EXTINT[0]", "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)
        {  "Q", "USART0-CLK",   "CANIF-RXLINE[1]","EIC-EXTINT[8]","PEVC-PAD_EVT[10]","INVALID","INVALID","INVALID","INVALID", "xin32"}, // PORT 1
        {  "Q", "INVALID",      "CANIF-TXLINE[1]","INVALID",     "PEVC-PAD_EVT[11]","INVALID","INVALID","INVALID", "INVALID", "xout32"},
        {  "Q", "INVALID",      "USBC-ID",       "PEVC-PAD_EVT[6]","TC1-A1",   "INVALID",    "INVALID", "INVALID", "INVALID", "xin1"},
        {  "Q", "INVALID",      "USBC-VBOF",     "PEVC-PAD_EVT[7]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "xout1"},
        {  "Q", "SPI1-MOSI",    "CANIF-RXLINE[0]","QDEC1-QEPI",  "INVALID",    "MACB-TXD[2]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-MISO",    "CANIF-TXLINE[0]","PEVC-PAD_EVT[12]","USART3_CLK","MACB-TXD[3]","INVALID","INVALID","INVALID","NA"  },
        {  "Q", "SPI1-SCK",     "INVALID",       "QDEC1-QEPA",   "USART1-CLK", "MACB-TX_ER", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[0]", "EIC-EXTINT[2]", "QDEC1-QEPB",   "INVALID",    "MACB-RX_DV", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[1]", "PEVC-PAD_EVT[1]","PWM-PWML[0]", "INVALID",    "MACB-RXD[0]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[2]", "INVALID",       "PWM-PWMH[0]",  "INVALID",    "MACB-RXD[1]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DTR",   "SPI0-MOSI",     "PWM-PWML[1]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DSR",   "SPI0-MISO",     "PWM-PWMH[1]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DCD",   "SPI0-SCK",      "PWM-PWML[2]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RI",    "SPI0-NPCS[0]",  "PWM-PWMH[2]",  "INVALID",    "MACB-RX_ER", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RTS",   "SPI0-NPCS[1]",  "PWM-PWML[3]",  "INVALID",    "MACB-MDC",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-CTS",   "USART1-CLK",    "PWM-PWMH[3]",  "INVALID",    "MACB-MDIO",  "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RXD",   "SPI0-NPCS[2]",  "PWM-EXT_FAULTS[0]","INVALID","CANIF-RXLINE[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-TXD",   "SPI0-NPCS[3]",  "PWM-EXT_FAULTS[1]","INVALID","CANIF-TXLINE[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "TC0-CLK2",     "INVALID",       "EIC-EXTINT[4]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A0",       "SPI1-MOSI",     "IISC-ISD0",    "INVALID",    "MACB-CRS",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B0",       "SPI1-MISO",     "IISC-ISDI",    "ACIFA1-ACAOUT","MACB-COL", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK1",     "SPI1-SCK",      "IISC-IMCK",    "ACIFA1-ACBOUT","MACB-RXD[2]","INVALID","INVALID","INVALID", "NA"  },
        {  "Q", "TC0-A1",       "SPI1-NPCS[3]",  "IISC-ISCK",    "SCIF-GCLK[0]","MACB-RXD[3]","INVALID","INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B1",       "SPI1-NPCS[2]",  "IISC-IWS",     "SCIF-GCLK[1]","MACB-RX_CLK","INVALID","INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK0",     "SPI1-NPCS[1]",  "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A2",       "SPI1-NPCS[0]",  "PEVC-PAD_EVT[8]","INVALID",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B2",       "SPI1-SCK",      "PEVC-PAD_EVT[9]","INVALID",  "MACB-TX_EN", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC0-QEPA",   "SPI1-MISO",     "PEVC-PAD_EVT[10]","TC1-CLK0","MACB-TXD[0]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC0-QEPB",   "SPI1-MOSI",     "PEVC-PAD_EVT[11]","TC1-B0",  "MACB-TXD[1]","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC0-QEPI",   "SPI0-NPCS[0]",  "PEVC-PAD_EVT[12]","TC1-A0",  "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "xin0"},
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "xout0"}
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)      
        {  "Q", "USBC-ID",      "SPI0-NPCS[1]",  "USART2-CTS",   "TC1-B2",     "CANIF-TXLINE[1]","INVALID","INVALID","INVALID","NA" }, // PORT 2
        {  "Q", "USBC-VBOF",    "SPI0-NPCS[2]",  "USART2-RTS",   "TC1-A2",     "CANIF-RXLINE[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "TWIMS0-TWD",   "SPI0-NPCS[3]",  "USART2-RXD",   "TC1-CLK1",   "MACB-MDC",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TWIMS0-TWCK",  "EIC-EXTINT[1]", "USART2-TXD",   "TC1-B1",     "MACB-MDIO",  "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TWIMS1-TWD",   "EIC-EXTINT[3]", "USART2-TXD",   "TC0-B1",     "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TWIMS1-TWCK",  "EIC-EXTINT[4]", "USART2-RXD",   "TC0-A2",     "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PEVC-PAD_EVT[15]","USART2-CLK", "USART2-CTS",   "TC0-CLK2",   "TWIMS2-TWD", "TWIMS0-TWALM","INVALID","INVALID","NA"},
        {  "Q", "PEVC-PAD_EVT[2]","EBI-NCS[3]",  "USART2-RTS",   "TC0-B2",     "TWIMS2-TWCL","TWIMS1-TWALM","INVALID","INVALID","NA"},
        {  "Q", "PEVC-PAD_EVT[13]","SPI1-NPCS[1]","EBI-NCS[0]",  "INVALID",    "USART4-TXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PEVC-PAD_EVT[14]","SPI1-NPCS[2]","EBI-ADDR[23]","INVALID",    "USART4-RXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PEVC-PAD_EVT[15]","SPI1-NPCS[3]","EBI-ADDR[22]","INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWMH[3]",  "CANIF-RXLINE[1]","EBI-ADDR[21]","TC0-CLK0",   "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWML[3]",  "CANIF-TXLINE[1]","EBI-ADDR[20]","USART2-CLK", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWMH[2]",  "EIC-EXTINT[7]", "INVALID",      "USART0-RTS", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWML[2]",  "USART0-CLK",    "EBI-SDCKE",    "USART0-CTS", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-PWMH[1]",  "SPI0-NPCS[0]",  "EBI-SDWE",     "USART0-RXD", "CANIF-RXLINE[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "PWM-PWML[1]",  "SPI0-NPCS[1]",  "EBI-CAS",      "USART0-TXD", "CANIF-TXLINE[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "PWM-PWMH[0]",  "SPI0-NPCS[2]",  "EBI-RAS",      "IISC-ISDO",  "INVALID",    "USART3-TXD","INVALID","INVALID","NA"  },
        {  "Q", "PWM-PWML[0]",  "EIC-EXTINT[5]", "EBI-SDA10",    "IISC-ISDI",  "INVALID",    "USART3-RXD","INVALID","INVALID","NA"  },
        {  "Q", "PWM-PWML[2]",  "SCIF-GCLK[0]",  "EBI-DATA[0]",  "IISC-IMCK",  "INVALID",    "USART3-CTS","INVALID","INVALID","NA"  },
        {  "Q", "PWM-PWMH[2]",  "SCIF-GCLK[1]",  "EBI-DATA[1]",  "IISC-ISCK",  "INVALID",    "USART3-RTS","INVALID","INVALID","NA"  },
        {  "Q", "PWM-EXT_FAULTS[0]","CANIF-RXLINE[0]","EBI-DATA[2]","IISC-IWS","INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "PWM-EXT_FAULTS[1]","CANIF-TXLINE[0]","EBI-DATA[3]","INVALID", "USART3-CLK", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC1-QEPB",   "CANIF-RXLINE[1]","EBI-DATA[4]", "PEVC-PAD_EVT[3]","INVALID","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC1-QEPA",   "CANIF-TXLINE[1]","EBI-DATA[5]", "PEVC-PAD_EVT[4]","INVALID","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "TC1-CLK2",      "EBI-DATA[6]",  "SCIF-GCLK[0]","USART4-TXD","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "QDEC1-QEPI",   "TC1-B2",        "EBI-DATA[7]",  "SCIF-GCLK[1]","USART4-RXD","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "TC1-A2",        "EBI-DATA[8]",  "ECI-EXTINT[0]","USART4-CTS","INVALID","INVALID", "INVALID", "NA"  },
        {  "Q", "SPI1-NPCS[3]", "TC1-CLK1",      "EBI-DATA[9]",  "INVALID",    "USART4-RTS", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[1]", "TC1-B1",        "EBI-DATA[10]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[2]", "TC1-A1",        "EBI-DATA[11]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[3]", "TC1-B0",        "EBI-DATA[12]", "PEVC-PAD_EVT[5]","USART4-CLK","INVALID","INVALID","INVALID","NA"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C    Periph. D    Periph. E    Periph. F    Periph. G    Periph. H (fictional Periph. clock function)
        {  "Q", "SPI0-MOSI",    "TC1-CLK0",      "EBI-DATA[13]", "QDEC0-QEPI", "USART0-TXD", "INVALID", "INVALID", "INVALID", "NA"  }, // PORT 3
        {  "Q", "SPI0-MISO",    "TC1-A0",        "EBI-DATA[14]", "TC0-CLK1",   "USART0-RXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-CLK",     "TC0-CLK2",      "EBI-DATA[15]", "QDEC0-QEPA", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-NPCS[0]", "TC0-B2",        "EBI-ADDR[0]",  "QDEC0-QEPB", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-MOSI",    "INVALID",       "EBI-ADDR[1]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-MISO",    "INVALID",       "EBI-ADDR[2]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "SPI0-SCK",     "INVALID",       "EBI-ADDR[3]",  "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DTR",   "EIC-EXTINT[5]", "EBI-ADDR[4]",  "QDEC0-QEPI", "USART4-TXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DSR",   "EIC-EXTINT[6]", "EBI-ADDR[5]",  "TC1-CLK2",   "USART4-RXD", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-DCD",   "CANIF-RXLINE[0]","EBI-ADDR[6]", "QDEC0-QEPA", "USART4-CTS", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-RI",    "CANIF-TXLINE[0]","EBI-ADDR[7]", "QDEC0-QEPB", "USART4-RTS", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART1-TXD",   "USBC-ID",       "EBI-ADDR[8]",  "PEVC-PAD_EVT[6]","MACB-TXD[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-RXD",   "USBC-VBOF",     "EBI-ADDR[9]",  "PEVC-PAD_EVT[7]","MACB-TXD[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-CTS",   "USART1-CLK",    "EBI-SDCK",     "PEVC-PAD_EVT[8]","MACB-RXD[0]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "USART1-RTS",   "EIC-EXTINT[7]", "EBI-ADDR[10]", "PEVC-PAD_EVT[9]","MACB-RXD[1]","INVALID","INVALID","INVALID","NA" },
        {  "Q", "TC0-A0",       "USART3-TXD",    "EBI-ADDR[11]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B0",       "USART3-RXD",    "EBI-ADDR[12]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A1",       "USART3-CTS",    "EBI-ADDR[12]", "USART3-CLK", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B1",       "USART3-RTS",    "EBI-ADDR[14]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-A2",       "INVALID",       "EBI-ADDR[15]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-B2",       "INVALID",       "EBI-ADDR[16]", "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-TXD",   "EIC-EXTINT[0]", "EBI-ADDR[17]", "QDEC1-QEPI", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-RXD",   "TC0-A2",        "EBI-ADDR[18]", "SCIF-GCLK[0]","INVALID",   "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-CTS",   "USART3-CLK",    "EBI-ADDR[19]", "QDEC1-QEPA", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART3-RTS",   "EIC-EXTINT[8]", "EBI-NWE1",     "QDEC1-QEPB", "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK0",     "USBC-ID",       "EBI-NWE0",     "INVALID",    "USART4-CLK", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "TC0-CLK1",     "USBC-VBOF",     "EBI-NRD",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-TXD",   "CANIF-RXLINE[0]","EBI-NCS[1]",  "TC0-A0",     "MACB-RX_ER", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-RXD",   "CANIF-TXLINE[0]","EBI-NCS[2]",  "TC0-B0",     "MACB-RX_DV", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-CTS",   "EIC-EXTINT[6]", "USART0-CLK",   "TC0-CLK0",   "MACB-TX_CLK","INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "USART0-RTS",   "EIC-EXTINT[3]", "EBI-NWAIT",    "TC0-A1",     "MACB-TX_EN", "INVALID", "INVALID", "INVALID", "NA"  },
        {  "Q", "INVALID",      "INVALID",       "INVALID",      "INVALID",    "INVALID",    "INVALID", "INVALID", "INVALID", "NA"  }
    },
    #endif
};
#else
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][5] = {
    {
        //  Q    Periph. A   Periph. B    Periph. C
        {  "Q", "RXD0",      "TC-CLK0",   "INVALID", "NA"  },            // PORT 0
        {  "Q", "TXD0",      "TC-CLK1",   "INVALID", "NA"  },
        {  "Q", "SCK0",      "TC-CLK2",   "INVALID", "NA"  },
        {  "Q", "RTS0",      "EIM-EXTINT[4]",   "DAC-DATA[0]", "NA"  },
        {  "Q", "CTS0",      "EIM-EXTINT[5]",   "DAC-DATAN[0]", "NA"  },
        {  "Q", "RXD1",      "PWM[4]",   "INVALID", "NA"  },
        {  "Q", "TXD1",      "PWM[5]",   "INVALID", "NA"  },
        {  "Q", "SCK1",      "GCLK[0]",   "SPI0-NPSC3", "NA"  },
        {  "Q", "RTS1",      "SPI0_NPCS[1]",   "EIM-EXTINT[7]", "NA"  },
        {  "Q", "CTS1",      "SPI0-NPCS[2]",   "MACB-WOL", "NA"  },
        {  "Q", "SPI0-NPCS[0]", "EIM-EXTINT[6]",   "INVALID", "NA"  },
        {  "Q", "SPI0-MISO",    "USB_ID",   "INVALID", "NA"  },
        {  "Q", "SPI0-MOSI",    "USB_VBOF",   "INVALID", "NA"  },
        {  "Q", "SPI0-SCK",     "INVALID",   "INVALID", "NA"  },
        {  "Q", "TX_FRAME_SYNC","SPI1-NPC[0]",   "EBI-NCS[0]", "NA"  },
        {  "Q", "TX_CLOCK",     "SPI1-SCK",   "EBI-ADDR[20]", "NA"  },
        {  "Q", "TX_DATA",      "SPI1_MOSI",   "EBI-ADDR[21]", "NA"  },
        {  "Q", "RX_DATA",      "SPI1-MISO",   "EBI-ADDR[22]", "NA"  },
        {  "Q", "RX_CLOCK",     "SPI1-NPCS[1]",   "MACB-WOL", "NA"  },
        {  "Q", "RX_FRAME_SYNC","SPI1-NPCS[2]",   "INVALID", "NA"  },
        {  "Q", "EIM-EXTINT[8]","SPI1-NPCS[3]",   "INVALID", "NA"  },
        {  "Q", "AD[0]",      "EIM-EXTINT[0]",   "USB_ID", "NA"  },
        {  "Q", "AD[1]",      "EIM-EXTINT[1]",   "USB_VBOF", "NA"  },
        {  "Q", "AD[2]",      "EIM-EXTINT[2]",   "DAC-DATA[1]", "NA"  },
        {  "Q", "AD[3]",      "EIM-EXTINT[3]",   "DAC-DATAN[1]", "NA"  },
        {  "Q", "AD[4]",      "EIM-SCAN[0]",   "EBI-NCS[0]", "NA"  },
        {  "Q", "AD[5]",      "EIM-SCAN[1]",   "EBI-ADDR[20]", "NA"  },
        {  "Q", "AD[6]",      "EIM-SCAN[2]",   "EBI-ADDR[21]", "NA"  },
        {  "Q", "AD[7]",      "EIM-SCAN[3]",   "EBI-ADDR[22]", "NA"  },
        {  "Q", "TWI-SDA",    "RTS2",   "INVALID", "NA"  },
        {  "Q", "TWI-SCL",    "CTS2",   "INVALID", "NA"  },
        {  "Q", "NA",         "NA",   "NA", "NA"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C
        {  "Q", "MACB-TX_CLK", "RTS2",  "RTS3", "INVALID"  },            // PORT 1
        {  "Q", "MACB-TX_EN", "CTS2",  "CTS3", "INVALID"  },
        {  "Q", "MACB-TXD[0]", "DAC-DATA[0]",  "INVALID", "INVALID"  },
        {  "Q", "MACB-TXD[1]", "DAC-DATAN[0]",  "INVALID", "INVALID"  },
        {  "Q", "MACB-CRS", "CLK3",  "EBI-NCS[3]", "INVALID"  },
        {  "Q", "MACB-RXD[0]", "DAC-DATA[1]",  "INVALID", "INVALID"  },
        {  "Q", "MACB-RXD[1]", "DAC-DATAN[1]",  "INVALID", "INVALID"  },
        {  "Q", "MACB-RX_ER", "INVALID",  "INVALID", "INVALID"  },
        {  "Q", "MACB-MDC", "INVALID",  "INVALID", "INVALID"  },
        {  "Q", "MACB-MDIO", "INVALID",  "INVALID", "INVALID"  },
        {  "Q", "MACB-TXD[2]", "RXD3",  "EBI-SDCK", "INVALID"  },
        {  "Q", "MACB-TXD[3]", "TXD3",  "EBI-SDCKE", "INVALID"  },
        {  "Q", "MACB-TX_ER", "TC-CLK0",  "EBI-RAS", "INVALID"  },
        {  "Q", "MACB-RXD[2]", "TC-CLK1",  "EBI-CAS", "INVALID"  },
        {  "Q", "MACB-RXD[3]", "TC-CLK2",  "EBI-SDWE", "INVALID"  },
        {  "Q", "MACB-RX_DV", "INVALID",  "INVALID", "INVALID"  },
        {  "Q", "MACB-COL", "USB_ID",  "EBI-SDA10", "INVALID"  },
        {  "Q", "MACB-RX_CLK", "USB_VBOF",  "EBI-ADDR[23]", "INVALID"  },
        {  "Q", "MACB-SPEED", "ADC-TRIGGER",  "PWM[6]", "INVALID"  },
        {  "Q", "PWM[0]", "GCLK[0]",  "EIM-SCAN[4]", "INVALID"  },
        {  "Q", "PWM[1]", "GCLK[1]",  "EIM-SCAN[5]", "INVALID"  },
        {  "Q", "PWM[2]", "GCLK[2]",  "EIM-SCAN[6]", "INVALID"  },
        {  "Q", "PWM[3]", "GCLK[3]",  "EIM-SCAN[7]", "INVALID"  },
        {  "Q", "TC-A0", "DCD1",  "INVALID", "INVALID"  },
        {  "Q", "TC-B0", "DSR1",  "INVALID", "INVALID"  },
        {  "Q", "TC-A1", "DTR1",  "INVALID", "INVALID"  },
        {  "Q", "TC-B1", "RI1",  "INVALID", "INVALID"  },
        {  "Q", "TC-A2", "PWM[4]",  "INVALID", "INVALID"  },
        {  "Q", "TC-B2", "PWM[5]",  "INVALID", "INVALID"  },
        {  "Q", "RXD2", "GCLK[1]",  "EBI-NCS[2]", "INVALID"  },
        {  "Q", "TXD2", "GCLK[2]",  "EBI-SDCS", "INVALID"  },
        {  "Q", "CLK2", "GCLK[3]",  "NWAIT", "INVALID"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C   (fictional Periph. D clock function)
        {  "Q", "INVALID",     "INVALID",  "INVALID", "xin32"  },        // PORT 2
        {  "Q", "INVALID",     "INVALID",  "INVALID", "xout32"  },
        {  "Q", "INVALID",     "INVALID",  "INVALID", "xin0"  },
        {  "Q", "INVALID",     "INVALID",  "INVALID", "xout0"  },
        {  "Q", "INVALID",     "INVALID",  "INVALID", "xin1"  },
        {  "Q", "INVALID",     "INVALID",  "INVALID", "xout1"  },
        {  "Q", "EBI-ADDR[0]", "SPI1-MISO",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[1]", "SPI0-NPCS[3]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[2]", "SPI0-NPCS[2]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[3]", "SPI0-NPCS[1]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[4]", "SPI0-NPCS[0]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[5]", "SPI0-SCK",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[6]", "SPI0-MOSI",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[7]", "SPI0-MISO",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[8]", "EIM-SCAN[7]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[9]", "EIM-SCAN[6]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[10]", "EIM-SCAN[5]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[11]", "EIM-SCAN[4]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-ADDR[12]", "EIM-SCAN[3]",  "TC-CLK2", "INVALID"  },
        {  "Q", "EBI-ADDR[13]", "EIM-SCAN[2]",  "TC-CLK1", "INVALID"  },
        {  "Q", "EBI-ADDR[14]", "EIM-SCAN[1]",  "TC-CLK0", "INVALID"  },
        {  "Q", "EBI-ADDR[15]", "EIM-SCAN[0]",  "TC-B2", "INVALID"  },
        {  "Q", "EBI-ADDR[16]", "INVALID",  "TC-A2", "INVALID"  },
        {  "Q", "EBI-ADDR[17]", "INVALID",  "TC-B2", "INVALID"  },
        {  "Q", "EBI-ADDR[18]", "CTS3",  "TC-A1", "INVALID"  },
        {  "Q", "EBI-ADDR[19]", "RTS3",  "TC-B0", "INVALID"  },
        {  "Q", "EBI-DATA[0]", "RXD2",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[1]", "TXD3",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[2]", "RXD3",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[3]", "RTS1",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[4]", "CTS1",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[5]", "TXD1",  "INVALID", "INVALID"  }
    },
    {
        //  Q    Periph. A   Periph. B    Periph. C
        {  "Q", "EBI-DATA[6]",  "RXD0",          "INVALID", "INVALID"  },// PORT 3
        {  "Q", "EBI-DATA[7]",  "RTS0",          "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[8]",  "CTS0",          "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[9]",  "TXD0",          "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[10]", "RXD0",          "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[11]", "SPI1-NPCS[2]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[12]", "SPI1-NPCS[1]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[13]", "SPI1-NPCS[0]",  "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[14]", "SPI1-SCK",      "INVALID", "INVALID"  },
        {  "Q", "EBI-DATA[15]", "SPI1-MOSI",     "INVALID", "INVALID"  },
        {  "Q", "EBI-NCS[1]",   "INVALID",       "TC-A0",   "INVALID"  },
        {  "Q", "EBI-NRD",      "RTS2",          "INVALID", "INVALID"  },
        {  "Q", "EBI-NWE0",     "CTS2",          "INVALID", "INVALID"  },
        {  "Q", "EBI-NWE1",     "TXD2",          "INVALID", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  },
        {  "Q", "NA", "NA",  "NA", "INVALID"  }
    },
};
#endif

static unsigned char *_ptrPerFunctions;

#ifdef SUPPORT_ADC
    typedef struct stPorts
    {
        unsigned char *ports;
        unsigned long *adcs;
        unsigned long *ulADCMode;
    } PORTS;

    static unsigned long *_ptrADC;
    static unsigned long *_ptrADC_mode;
#endif

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
    char *ptrBuf = cPortDetails;
    int iLength = 0;
    int iADC = -1;
    int iPortLength = PORT_WIDTH;
    int iOffset = 0;
    char cBuf[BUF1SIZE + 1];

    switch (iPort) {
    case _GPIO_0:
        STRCPY(cPortDetails, "Port 0");
        break;
    case _GPIO_1:
        STRCPY(cPortDetails, "Port 1");
        iOffset = 32;
        break;
    case _GPIO_2:
        STRCPY(cPortDetails, "Port 2");
        iOffset = 64;
        break;
    case _GPIO_3:
#ifdef CHIP_100_PIN
        STRCPY(cPortDetails, "Port 3 - not available on 100 pin device");
        return;
#else
        STRCPY(cPortDetails, "Port 3");
        iOffset = 96;
#endif
        break;
    default:
        STRCPY(cPortDetails, "?????");
        return;
    }

    iBit = iPortLength - iBit - 1;                                       // bit position
    if (iBit < 0) {
        return;
    }
    SPRINTF(cBuf, " GPIO %i Pin: ", (iBit + iOffset));
    STRCAT(cPortDetails, cBuf);
    STRCPY(cBuf, cPinNumber[iPort][iBit]);
    STRCAT(cPortDetails, cBuf);
    if (!strcmp(cBuf, "NA")) {
        return;
    }

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
        STRCPY(&cBuf[1], cPer[iPort][iBit][*ptrList]);
        STRCAT(cPortDetails, cBuf);
#ifdef SUPPORT_ADC
        if ((*ptrList == 1) && (_GPIO_0 == iPort) && ((iBit >= 21) && (iBit <= 28))) { // check for ADC configuration
            iADC = (iBit - 21);
        }
        if (iADC >= 0) {
            if (*_ptrADC_mode & LOWRES) {
                SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iADC]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)ADC_FULL_SCALE_8BIT)); // {3}
            }
            else {
                SPRINTF(cBuf, " [%fV]", (((float)_ptrADC[iADC]*((float)ADC_REFERENCE_VOLTAGE/(float)1000))/(float)ADC_FULL_SCALE_10BIT)); // {3}
            }
            STRCAT(cPortDetails, cBuf);
        }
#endif
    }
    else {
        if (ulPortFunction[iPort] & (0x01 << iBit)) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
#ifdef _AT32UC3B
        if ((iPort == 0) && (iBit <= 2)) {
            STRCAT(cPortDetails, " (Warning - shared with JTAG!)");
        }
#endif
    }
}

// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
    #ifdef SUPPORT_ADC                                                   // {1}
    PORTS *_ports = (PORTS *)hw_table;
    _ptrPerFunctions = _ports->ports;
    _ptrADC          = _ports->adcs;
    _ptrADC_mode     = _ports->ulADCMode;
    #else
    _ptrPerFunctions = (unsigned char *)hw_table; 
    #endif
}

#endif

/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_K27_K28.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

*/


#if PIN_COUNT == PIN_COUNT_169_PIN
    #define _PIN_COUNT          0                                        // 169 MAPBGA
#elif PIN_COUNT == PIN_COUNT_210_PIN
    #define _PIN_COUNT          1                                        // 210 WLCSP
#else
    #error "Invalid package"
#endif

#define ALTERNATIVE_FUNCTIONS   7                                        // GPIO plus 6 possible peripheral/extra/system functions




static const char *cPinNumber[PORTS_AVAILABLE + 1][PORT_WIDTH][4] = {
    {
        // 169BGA    210WLCSP                       GPIO A
        {  "N8",     "M8",       },                 // PA0
        {  "N9",     "N8",       },                 // PA1
        {  "M9",     "P8",       },                 // PA2
        {  "M8",     "M7",       },                 // PA3
        {  "L9",     "N7",       },                 // PA4
        {  "N10",    "P7",       },                 // PA5
        {  "M10",    "P6",       },                 // PA6
        {  "L10",    "N6",       },                 // PA7
        {  "K9",     "M6",       },                 // PA8
        {  "K10",    "L6",       },                 // PA9
        {  "N11",    "P5",       },                 // PA10
        {  "M11",    "N5",       },                 // PA11
        {  "L12",    "M5",       },                 // PA12
        {  "L11",    "L5",       },                 // PA13
        {  "K13",    "P4",       },                 // PA14
        {  "K12",    "N4",       },                 // PA15
        {  "J13",    "P3",       },                 // PA16
        {  "J12",    "N3",       },                 // PA17
        {  "N13",    "P1",       },                 // PA18
        {  "M13",    "N1",       },                 // PA19
        {  "L7",     "P9",       },                 // PA20
        {  "K7",     "N9",       },                 // PA21
        {  "K8",     "M9",       },                 // PA22
        {  "L8",     "L9",       },                 // PA23
        {  "K11",    "M3",       },                 // PA24
        {  "J11",    "M4",       },                 // PA25
        {  "J10",    "L4",       },                 // PA26
        {  "H13",    "L3",       },                 // PA27
        {  "H12",    "L2",       },                 // PA28
        {  "H11",    "L1",       },                 // PA29
        {  "H10",    "K4",       },                 // PA30
        {  "J9",     "K3",       },                 // PA31
    },
    {
        // 169BGA    210WLCSP                       GPIO B
        {  "G13",      "K2",     },                 // PB0
        {  "G12",      "K1",     },                 // PB1
        {  "G11",      "J4",     },                 // PB2
        {  "G10",      "J3",     },                 // PB3
        {  "H9",       "J2",     },                 // PB4
        {  "F13",      "J1",     },                 // PB5
        {  "F12",      "H3",     },                 // PB6
        {  "F11",      "H2",     },                 // PB7
        {  "F10",      "H1",     },                 // PB8
        {  "F9",       "G3",     },                 // PB9
        {  "G9",       "G2",     },                 // PB10
        {  "E13",      "G1",     },                 // PB11
        {  "E12",      "F4",     },                 // PB12
        {  "E11",      "F3",     },                 // PB13
        {  "E10",      "F2",     },                 // PB14
        {  "E9",       "F1",     },                 // PB15
        {  "F8",       "E1",     },                 // PB16
        {  "D13",      "E2",     },                 // PB17
        {  "D12",      "E3",     },                 // PB18
        {  "D11",      "E4",     },                 // PB19
        {  "D10",      "D1",     },                 // PB20
        {  "D9",       "D2",     },                 // PB21
        {  "C13",      "D3",     },                 // PB22
        {  "C12",      "D4",     },                 // PB23
        {  "-",        "-",      },                 // PB24
        {  "-",        "-",      },                 // PB25
        {  "-",        "-",      },                 // PB26
        {  "-",        "-",      },                 // PB27
        {  "-",        "-",      },                 // PB28
        {  "-",        "-",      },                 // PB29
        {  "-",        "-",      },                 // PB30
        {  "-",        "-",      },                 // PB31
    },
    {
        // 169BGA    210WLCSP                       GPIO C
        {  "B13",      "C1",     },                 // PC0
        {  "B12",      "C2",     },                 // PC1
        {  "A13",      "B1",     },                 // PC2
        {  "A12",      "B2",     },                 // PC3
        {  "B11",      "A2",     },                 // PC4
        {  "A11",      "C3",     },                 // PC5
        {  "A10",      "B3",     },                 // PC6
        {  "B10",      "A3",     },                 // PC7
        {  "C10",      "C4",     },                 // PC8
        {  "C9",       "B4",     },                 // PC9
        {  "A8",       "A4",     },                 // PC10
        {  "A9",       "D5",     },                 // PC11
        {  "B9",       "C5",     },                 // PC12
        {  "B8",       "B5",     },                 // PC13
        {  "C8",       "A5",     },                 // PC14
        {  "D8",       "C6",     },                 // PC15
        {  "E8",       "A6",     },                 // PC16
        {  "E7",       "B6",     },                 // PC17
        {  "D7",       "C7",     },                 // PC18
        {  "C7",       "B7",     },                 // PC19
        {  "-",        "-",      },                 // PC20
        {  "-",        "-",      },                 // PC21
        {  "-",        "-",      },                 // PC22
        {  "-",        "-",      },                 // PC23
        {  "B7",       "A7",     },                 // PC24
        {  "A7",       "A8",     },                 // PC25
        {  "E6",       "B8",     },                 // PC26
        {  "D6",       "A9",     },                 // PC27
        {  "C6",       "B9",     },                 // PC28
        {  "B6",       "A10",    },                 // PC29
        {  "-",        "-",      },                 // PC30
        {  "-",        "-",      },                 // PC31
    },
    {
        // 169BGA    210WLCSP                       GPIO D
        {  "A6",      "B10",     },                 // PD0
        {  "A5",      "C10",     },                 // PD1
        {  "A4",      "A11",     },                 // PD2
        {  "B4",      "B11",     },                 // PD3
        {  "B5",      "C11",     },                 // PD4
        {  "C4",      "D11",     },                 // PD5
        {  "C5",      "A12",     },                 // PD6
        {  "E5",      "B12",     },                 // PD7
        {  "D5",      "C12",     },                 // PD8
        {  "D4",      "D12",     },                 // PD9
        {  "D3",      "A13",     },                 // PD10
        {  "C2",      "B13",     },                 // PD11
        {  "B2",      "C13",     },                 // PD12
        {  "B3",      "A14",     },                 // PD13
        {  "A2",      "B14",     },                 // PD14
        {  "A3",      "A15",     },                 // PD15
        {  "-",       "-",       },                 // PD16
        {  "-",       "-",       },                 // PD17
        {  "-",       "-",       },                 // PD18
        {  "-",       "-",       },                 // PD19
        {  "-",       "-",       },                 // PD20
        {  "-",       "-",       },                 // PD21
        {  "-",       "-",       },                 // PD22
        {  "-",       "-",       },                 // PD23
        {  "-",       "-",       },                 // PD24
        {  "-",       "-",       },                 // PD25
        {  "-",       "-",       },                 // PD26
        {  "-",       "-",       },                 // PD27
        {  "-",       "-",       },                 // PD28
        {  "-",       "-",       },                 // PD29
        {  "-",       "-",       },                 // PD30
        {  "-",       "-",       },                 // PD31
    },
    {
        // 169BGA    210WLCSP                       GPIO E
        {  "A1",      "B15",     },                 // PE0
        {  "B1",      "C14",     },                 // PE1
        {  "C1",      "C15",     },                 // PE2
        {  "D1",      "D13",     },                 // PE3
        {  "E1",      "D14",     },                 // PE4
        {  "D2",      "D15",     },                 // PE5
        {  "E2",      "E12",     },                 // PE6
        {  "E3",      "E13",     },                 // PE7
        {  "E4",      "E14",     },                 // PE8
        {  "F3",      "E15",     },                 // PE9
        {  "F4",      "F12",     },                 // PE10
        {  "G4",      "F13",     },                 // PE11
        {  "H4",      "F14",     },                 // PE12
        {  "J6",      "F15",     },                 // PE13
        {  "-",       "-",       },                 // PE14
        {  "-",       "-",       },                 // PE15
        {  "H3",      "G12",     },                 // PE16
        {  "F5",      "G13",     },                 // PE17
        {  "F6",      "G14",     },                 // PE18
        {  "F7",      "G15",     },                 // PE19
        {  "J7",      "H12",     },                 // PE20
        {  "K6",      "H13",     },                 // PE21
        {  "L6",      "H14",     },                 // PE22
        {  "M7",      "J13",     },                 // PE23
        {  "-",       "-",       },                 // PE24
        {  "-",       "-",       },                 // PE25
        {  "-",       "-",       },                 // PE26
        {  "-",       "-",       },                 // PE27
        {  "-",       "-",       },                 // PE28
        {  "-",       "-",       },                 // PE29
        {  "-",       "-",       },                 // PE30
        {  "-",       "-",       },                 // PE31
    },
    {               
        // 169BGA    210WLCSP                       dedicated ADC pins
        {  "G1",      "J15",     },                 // USB0_DP
        {  "F1",      "K15",     },                 // USB0_DM
        {  "G2",      "J14",     },                 // VREG_OUT
        {  "F2",      "K14",     },                 // VREG_IN0
        {  "H2",      "L14",     },                 // VREG_IN1
        {  "K1",      "L15",     },                 // USB1_VSS
        {  "J1",      "M15",     },                 // USB1_DP
        {  "H1",      "N15",     },                 // USB1_DM
        {  "J2",      "M14",     },                 // USB1_VBUS
        {  "M2",      "N14",     },                 // ADC0_DP0
        {  "L2",      "N13",     },                 // ADC0_DM0
        {  "N1",      "P14",     },                 // ADC0_DP3
        {  "N2",      "P13",     },                 // ADC0_DM3
        {  "J3",      "M13",     },                 // VDDA
        {  "K3",      "L13",     },                 // VREFH
        {  "K4",      "L12",     },                 // VREFL
        {  "J4",      "M12",     },                 // VSSA
        {  "L3",      "K12",     },                 // VSSA
        {  "M1",      "-",       },                 // ADC0_DP1
        {  "L1",      "-",       },                 // ADC0_DM1
        {  "N3",      "N12",     },                 // VREF_OUT/CMP1_IN5/CMP0_IN5/ADC0_SE22
        {  "M4",      "P12",     },                 // DAC0_OUT/CMP1_IN3/ADC0_SE23
        {  "N4",      "M11",     },                 // ADC0_SE16
        {  "M5",      "N11",     },                 // RTC_WAKEUP_B
        {  "N5",      "P11",     },                 // XTAL32
        {  "N6",      "P10",     },                 // EXTAL32
        {  "M6",      "N10",     },                 // VBAT
        {  "-",       "-",       },                 // 
        {  "-",       "-",       },                 // 
        {  "-",       "-",       },                 // 
        {  "-",       "-",       },                 // 
        {  "-",       "-",       },                 // 
    },
};

#define _DEDICATED_PINS
static CHAR *cDedicated[PORT_WIDTH] = {                                  // dedicated pin functions
    "USB0_DP",
    "USB0_DM",
    "VREG_OUT",
    "VREG_IN0",
    "VREG_IN1",
    "USB1_VSS",
    "USB1_DP",
    "USB1_DM",
    "USB1_VBUS",
    "ADC0_DP0",
    "ADC0_DM0",
    "ADC0_DP3",
    "ADC0_DM3",
    "VDDA",
    "VREFH",
    "VREFL",
    "VSSA",
    "VSSA",
    "ADC0_DP1",
    "ADC0_DM1",
    "VREF_OUT / CMP1_IN5 / CMP0_IN5 / ADC0_SE22",
    "DAC0_OUT / CMP1_IN3 / ADC0_SE23",
    "ADC0_SE16",
    "RTC_WAKEUP_B",
    "XTAL32",
    "EXTAL32",
    "VBAT",
};

// Table not yet complete
//
static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port B
#if PORTS_AVAILABLE > 2
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port C
#endif
#if PORTS_AVAILABLE > 3
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port D
#endif
#if PORTS_AVAILABLE > 4
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port E
#endif
#if PORTS_AVAILABLE > 5
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port F
#endif
};




static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7               PORT A
        { "-",            "PTA0",     "LPUART0_CTS_b",         "FTM0_CH5",          "-",           "FXIO0_D10",         "-",        "JTAG_TCLK/SWD_CLK" },
        { "-",            "PTA1",     "LPUART0_RX",         "FTM0_CH6",          "I2C3_SDA",           "FXIO0_D11",         "-",        "JTAG_TDI" },
        { "-",            "PTA2",     "LPUART0_TX",         "FTM0_CH7",          "I2C3_SCL",           "FXIO0_D12",         "-",        "JTAG_TDO/TRACE_SWO" },
        { "-",            "PTA3",     "LPUART0_RTS_b",         "FTM0_CH0",          "-",           "FXIO0_D13",         "-",        "JTAG_TMS/SWD_DIO" },
        { "-",            "PTA4/LLWU_P3",     "-",         "FTM0_CH1",          "-",           "FXIO0_D14",         "-",        "NMI_b" },
        { "-",            "PTA5",     "USB0_CLKIN",         "FTM0_CH2",          "-",           "FXIO0_D15",         "I2S0_TX_BCLK",        "JTAG_TRST_b" },
        { "-",            "PTA6",     "I2C2_SCL",         "FTM0_CH3",          "-",           "CLKOUT",         "-",        "TRACE_CLKOUT" },
        { "ADC0_SE10",    "PTA7",     "I2C2_SDA",         "FTM0_CH4",          "-",           "-",         "-",        "TRACE_D3" },
        { "ADC0_SE11",    "PTA8",     "I2C1_SCL",         "FTM1_CH0",          "-",           "-",         "FTM1_QD_PHA/TPM1_CH0",        "TRACE_D2" },
        { "-",            "PTA9",     "I2C1_SDA",         "FTM1_CH1",          "-",           "-",         "FTM1_QD_PHB/TPM1_CH1",        "TRACE_D1" },
        { "-",            "PTA10/LLWU_P22",     "I2C2_SDA",         "FTM2_CH0",          "-",           "FXIO0_D16",         "FTM2_QD_PHA/TPM2_CH0",        "TRACE_D0" },
        { "-",            "PTA11/LLWU_P23",     "I2C2_SCL",         "FTM2_CH1",          "-",           "FXIO0_D17",         "FTM2_QD_PHB/TPM2_CH1",        "USB1_ID" },
        { "-",            "PTA12",     "-",         "FTM1_CH0",          "TRACE_CLKOUT",           "FXIO0_D18",         "I2S0_TXD0",        "FTM1_QD_PHA/TPM1_CH0" },
        { "-",            "PTA13/LLWU_P4",     "-",         "FTM1_CH1",          "TRACE_D3",           "FXIO0_D19",         "I2S0_TX_FS",        "FTM1_QD_PHB/TPM1_CH1" },
        { "-",            "PTA14",     "SPI0_PCS0",         "LPUART0_TX",          "TRACE_D2",           "FXIO0_D20",         "I2S0_RX_BCLK",        "I2S0_TXD1" },
        { "-",            "PTA15",     "SPI0_SCK",         "LPUART0_RX",          "TRACE_D1",           "FXIO0_D21",         "I2S0_RXD0",        "-" },
        { "-",            "PTA16",     "SPI0_SOUT",         "LPUART0_CTS_b",          "TRACE_D0",           "FXIO0_D22",         "I2S0_RX_FS",        "I2S0_RXD1" },
        { "-",            "PTA17",     "SPI0_SIN",         "LPUART0_RTS_b",          "-",           "FXIO0_D23",         "I2S0_MCLK",        "I2S1_MCLK" },
        { "EXTAL0",       "PTA18",     "-",         "FTM0_FLT2",          "FTM_CLKIN0",           "-",         "-",        "TPM_CLKIN0" },
        { "XTAL0",        "PTA19",     "-",         "FTM1_FLT0",          "FTM_CLKIN1",           "-",         "LPTMR0_ALT1/LPTMR1_ALT1",        "TPM_CLKIN1" },
        { "-",            "PTA20",     "I2C0_SCL",         "LPUART4_TX",          "FTM_CLKIN1",           "FXIO0_D8",         "EWM_OUT_b",        "TPM_CLKIN1" },
        { "-",            "PTA21/LLWU_P21",     "I2C0_SDA",         "LPUART4_RX",          "-",           "FXIO0_D9",         "EWM_IN",        "-" },
        { "-",            "PTA22",     "-",         "LPUART4_CTS_b",          "-",           "FXIO0_D6",         "RTC_CLKOUT",        "USB0_CLKIN" },
        { "-",            "PTA23",     "-",         "LPUART4_RTS_b",          "-",           "FXIO0_D7",         "-",        "-" },
        { "-",            "PTA24",     "-",         "LPUART2_TX",          "SDHC0_D1",           "FB_A15/SDRAM_D15",         "FB_A29",        "I2S1_TX_BCLK" },
        { "-",            "PTA25",     "-",         "LPUART2_RX",          "SDHC0_D0",           "FB_A14/SDRAM_D14",         "FB_A28",        "I2S1_TX_FS" },
        { "-",            "PTA26",     "-",         "LPUART2_CTS_b",          "SDHC0_DCLK",           "FB_A13/SDRAM_D13",         "FB_A27",        "I2S1_TXD0" },
        { "-",            "PTA27",     "-",         "LPUART2_RTS_b",          "SDHC0_CMD",           "FB_A12/SDRAM_D12",         "FB_A26",        "I2S1_TXD1" },
        { "-",            "PTA28",     "-",         "LPUART3_TX",          "SDHC0_D3",           "-",         "FB_A25",        "I2S1_RXD1" },
        { "-",            "PTA29",     "-",         "LPUART3_RX",          "SDHC0_D2",           "-",         "FB_A24",        "I2S1_RXD0" },
        { "-",            "PTA30",     "I2C3_SDA",         "LPUART3_CTS_b",          "-",           "FB_A11/SDRAM_D11",         "-",        "I2S1_RX_FS" },
        { "-",            "PTA31",     "I2C3_SCL",         "LPUART3_RTS_b",          "-",           "FB_A10/SDRAM_D10",         "-",        "I2S1_RX_BCLK"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7               PORT B
        {  "ADC0_SE8",     "PTB0/LLWU_P5",     "I2C0_SCL",         "FTM1_CH0",          "-",           "SDRAM_CAS_b",         "FTM1_QD_PHA/TPM1_CH0",        "FXIO0_D0"                 },
        {  "ADC0_SE9",     "PTB1",     "I2C0_SDA",         "FTM1_CH1",          "-",           "SDRAM_RAS_b",         "FTM1_QD_PHB/TPM1_CH1",        "FXIO0_D1"                 },
        {  "ADC0_SE12",    "PTB2",     "I2C0_SCL",         "LPUART0_RTS_b",          "-",           "SDRAM_WE_b",         "FTM0_FLT3",        "FXIO0_D2"                 },
        {  "ADC0_SE13",    "PTB3",     "I2C0_SDA",         "LPUART0_CTS_b",          "-",           "SDRAM_CS0_b",         "FTM0_FLT0",        "FXIO0_D3"                 },
        {  "-",            "PTB4",     "-",         "-",          "-",           "SDRAM_CS1_b",         "FTM1_FLT0",        "-"                 },
        {  "-",            "PTB5",     "-",         "-",          "-",           "-",         "FTM2_FLT0",        "-"                 },
        {  "-",            "PTB6",     "-",         "-",          "-",           "FB_AD23/SDRAM_D23",         "-",        "-"                 },
        {  "-",            "PTB7",     "-",         "-",          "-",           "FB_AD22/SDRAM_D22",         "-",        "-"                 },
        {  "-",            "PTB8",     "-",         "LPUART3_RTS_b",          "-",           "FB_AD21/SDRAM_D21",         "-",        "-"                 },
        {  "-",            "PTB9",     "SPI1_PCS1",         "LPUART3_CTS_b",          "-",           "FB_AD20/SDRAM_D20",         "-",        "-"                 },
        {  "-",            "PTB10",     "SPI1_PCS0",         "LPUART3_RX",          "I2C2_SCL",           "FB_AD19/SDRAM_D19",         "FTM0_FLT1",        "FXIO0_D4"                 },
        {  "-",            "PTB11",     "SPI1_SCK",         "LPUART3_TX",          "I2C2_SDA",           "FB_AD18/SDRAM_D18",         "FTM0_FLT2",        "FXIO0_D5"                 },
        {  "-",            "PTB12",     "LPUART0_RTS_b",         "FTM1_CH0",          "FTM0_CH4",           "FB_A9/SDRAM_D9",         "FTM1_QD_PHA/TPM1_CH0",        "-"                 },
        {  "-",            "PTB13",     "LPUART0_CTS_b",         "FTM1_CH1",          "FTM0_CH5",           "FB_A8/SDRAM_D8",         "FTM1_QD_PHB/TPM1_CH1",        "-"                 },
        {  "-",            "PTB14",     "LPUART0_RX",         "-",          "-",           "FB_A7/SDRAM_D7",         "-",        "-"                 },
        {  "-",            "PTB15",     "LPUART0_TX",         "-",          "-",           "FB_A6/SDRAM_D6",         "-",        "-"                 },
        {  "-",            "PTB16",     "SPI1_SOUT",         "LPUART0_RX",          "FTM_CLKIN0",           "FB_AD17/SDRAM_D17",         "EWM_IN",        "TPM_CLKIN0"                 },
        {  "-",            "PTB17",     "SPI1_SIN",         "LPUART0_TX",          "FTM_CLKIN1",           "FB_AD16/SDRAM_D16",         "EWM_OUT_b",        "TPM_CLKIN1"                 },
        {  "-",            "PTB18",     "-",         "FTM2_CH0",          "I2S0_TX_BCLK",           "FB_AD15/SDRAM_A23",         "FTM2_QD_PHA/TPM2_CH0",        "FXIO0_D6"                 },
        {  "-",            "PTB19",     "SDRAM_CKE",         "FTM2_CH1",          "I2S0_TX_FS",           "FB_OE_b",         "FTM2_QD_PHB/TPM2_CH1",        "FXIO0_D7"                 },
        {  "-",            "PTB20",     "SPI2_PCS0",         "-",          "-",           "FB_AD31/SDRAM_D31",         "CMP0_OUT",        "FXIO0_D8"                 },
        {  "-",            "PTB21",     "SPI2_SCK",         "-",          "-",           "FB_AD30/SDRAM_D30",         "CMP1_OUT",        "FXIO0_D9"                 },
        {  "-",            "PTB22",     "SPI2_SOUT",         "-",          "-",           "FB_AD29/SDRAM_D29",         "-",        "FXIO0_D10"                 },
        {  "-",            "PTB23",     "SPI2_SIN",         "SPI0_PCS5",          "-",           "FB_AD28/SDRAM_D28",         "-",        "FXIO0_D11"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7               PORT C
        {  "ADC0_SE14",    "PTC0",     "SPI0_PCS4",         "PDB0_EXTRG",          "USB0_SOF_OUT",           "FB_AD14/SDRAM_A22",         "I2S0_TXD1",        "FXIO0_D12"                 },
        {  "ADC0_SE15",    "PTC1/LLWU_P6",     "SPI0_PCS3",         "LPUART1_RTS_b",          "FTM0_CH0",           "FB_AD13/SDRAM_A21",         "I2S0_TXD0",        "FXIO0_D13"                 },
        {  "ADC0_SE4b/CMP1_IN0",       "PTC2",     "SPI0_PCS2",         "LPUART1_CTS_b",          "FTM0_CH1",           "FB_AD12/SDRAM_A20",         "I2S0_TX_FS",        "-"                 },
        {  "CMP1_IN1",     "PTC3/LLWU_P7",     "SPI0_PCS1",         "LPUART1_RX",          "FTM0_CH2",           "CLKOUT",         "I2S0_TX_BCLK",        "-"                 },
        {  "-",            "PTC4/LLWU_P8",     "SPI0_PCS0",         "LPUART1_TX",          "FTM0_CH3",           "FB_AD11/SDRAM_A19",         "CMP1_OUT",        "-"                 },
        {  "-",            "PTC5/LLWU_P9",     "SPI0_SCK",         "LPTMR0_ALT2/LPTMR1_ALT2",          "I2S0_RXD0",           "FB_AD10/SDRAM_A18",         "CMP0_OUT",        "FTM0_CH2"                 },
        {  "CMP0_IN0",     "PTC6/LLWU_P10",     "SPI0_SOUT",         "PDB0_EXTRG",          "I2S0_RX_BCLK",           "FB_AD9/SDRAM_A17",         "I2S0_MCLK",        "FXIO0_D14"                 },
        {  "CMP0_IN1",     "PTC7",     "SPI0_SIN",         "USB0_SOF_OUT",          "I2S0_RX_FS",           "FB_AD8/SDRAM_A16",         "-",        "FXIO0_D15"                 },
        {  "CMP0_IN2",     "PTC8",     "-",         "FTM3_CH4",          "I2S0_MCLK",           "FB_AD7/SDRAM_A15",         "-",        "FXIO0_D16"                 },
        {  "CMP0_IN3",     "PTC9",     "-",         "FTM3_CH5",          "I2S0_RX_BCLK",           "FB_AD6/SDRAM_A14",         "FTM2_FLT0",        "FXIO0_D17"                 },
        {  "-",            "PTC10",     "I2C1_SCL",         "FTM3_CH6",          "I2S0_RX_FS",           "FB_AD5/SDRAM_A13",         "-",        "FXIO0_D18"                 },
        {  "-",            "PTC11/LLWU_P11",     "I2C1_SDA",         "FTM3_CH7",          "I2S0_RXD1",           "FB_RW_b",         "-",        "FXIO0_D19"                 },
        {  "-",            "PTC12",     "-",         "LPUART4_RTS_b",          "FTM_CLKIN0",           "FB_AD27/SDRAM_D27",         "FTM3_FLT0",        "TPM_CLKIN0"                 },
        {  "-",            "PTC13",     "-",         "LPUART4_CTS_b",          "FTM_CLKIN1",           "FB_AD26/SDRAM_D26",         "-",        "TPM_CLKIN1"                 },
        {  "-",            "PTC14",     "-",         "LPUART4_RX",          "-",           "FB_AD25/SDRAM_D25",         "-",        "FXIO0_D20"                 },
        {  "-",            "PTC15",     "-",         "LPUART4_TX",          "-",           "FB_AD24/SDRAM_D24",         "-",        "FXIO0_D21"                 },
        {  "-",            "PTC16",     "-",         "LPUART3_RX",          "-",           "FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b/SDRAM_DQM2",         "-",        "-"                 },
        {  "-",            "PTC17",     "-",         "LPUART3_TX",          "-",           "FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b/SDRAM_DQM3",         "-",        "-"                 },
        {  "-",            "PTC18",     "-",         "LPUART3_RTS_b",          "-",           "FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b/SDRAM_DQM1",         "-",        "-"                 },
        {  "-",            "PTC19",     "-",         "LPUART3_CTS_b",          "-",           "FB_CS3_b/FB_BE7_0_BLS31_24_b/SDRAM_DQM0",         "FB_TA_b",        "QSPI0A_SS1_B"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "PTC24",     "-",         "LPUART0_TX",          "-",           "FB_A5/SDRAM_D5",         "-",        "QSPI0A_DATA3"                 },
        {  "-",            "PTC25",     "-",         "LPUART0_RX",          "-",           "FB_A4/SDRAM_D4",         "-",        "QSPI0A_SCLK"                 },
        {  "-",            "PTC26",     "-",         "LPUART0_CTS_b",          "-",           "FB_A3/SDRAM_D3",         "-",        "QSPI0A_DATA0"                 },
        {  "-",            "PTC27",     "-",         "LPUART0_RTS_b",          "-",           "FB_A2/SDRAM_D2",         "-",        "QSPI0A_DATA2"                 },
        {  "-",            "PTC28",     "I2C3_SDA",         "-",          "-",           "FB_A1/SDRAM_D1",         "-",        "QSPI0A_DATA1"                 },
        {  "-",            "PTC29",     "I2C3_SCL",         "-",          "-",           "FB_A0/SDRAM_D0",         "-",        "QSPI0A_SS0_B"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },




    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7               PORT D
        {  "-",            "PTD0/LLWU_P12",     "SPI0_PCS0",         "LPUART2_RTS_b",          "FTM3_CH0",           "FB_ALE/FB_CS1_b/FB_TS_b",         "-",        "FXIO0_D22"                 },
        {  "ADC0_SE5b",    "PTD1",     "SPI0_SCK",         "LPUART2_CTS_b",          "FTM3_CH1",           "FB_CS0_b",         "-",        "FXIO0_D23"                 },
        {  "-",            "PTD2/LLWU_P13",     "SPI0_SOUT",         "LPUART2_RX",          "FTM3_CH2",           "FB_AD4/SDRAM_A12",         "-",        "I2C0_SCL"                 },
        {  "-",            "PTD3",     "SPI0_SIN",         "LPUART2_TX",          "FTM3_CH3",           "FB_AD3/SDRAM_A11",         "-",        "I2C0_SDA"                 },
        {  "-",            "PTD4/LLWU_P14",     "SPI0_PCS1",         "LPUART0_RTS_b",          "FTM0_CH4",           "FB_AD2/SDRAM_A10",         "EWM_IN",        "SPI1_PCS0"                 },
        {  "ADC0_SE6b",    "PTD5",     "SPI0_PCS2",         "LPUART0_CTS_b",          "FTM0_CH5",           "FB_AD1/SDRAM_A9",         "EWM_OUT_b",        "SPI1_SCK"                 },
        {  "ADC0_SE7b",    "PTD6/LLWU_P15",     "SPI0_PCS3",         "LPUART0_RX",          "FTM0_CH6",           "FB_AD0",         "FTM0_FLT0",        "SPI1_SOUT"                 },
        {  "-",            "PTD7",     "CMT_IRO",         "LPUART0_TX",          "FTM0_CH7",           "SDRAM_CKE",         "FTM0_FLT1",        "SPI1_SIN"                 },
        {  "-",            "PTD8/LLWU_P24",     "I2C0_SCL",         "LPUART1_RX",          "-",           "-",         "FB_A16",        "FXIO0_D24"                 },
        {  "-",            "PTD9",     "I2C0_SDA",         "LPUART1_TX",          "-",           "-",         "FB_A17",        "FXIO0_D25"                 },
        {  "-",            "PTD10",     "-",         "LPUART1_RTS_b",          "-",           "-",         "FB_A18",        "FXIO0_D26"                 },
        {  "-",            "PTD11/LLWU_P25",     "SPI2_PCS0",         "LPUART1_CTS_b",          "-",           "-",         "FB_A19",        "FXIO0_D27"                 },
        {  "-",            "PTD12",     "SPI2_SCK",         "FTM3_FLT0",          "-",           "-",         "FB_A20",        "FXIO0_D28"                 },
        {  "-",            "PTD13",     "SPI2_SOUT",         "",          "-",           "-",         "FB_A21",        "FXIO0_D29"                 },
        {  "-",            "PTD14",     "SPI2_SIN",         "",          "-",           "-",         "FB_A22",        "FXIO0_D30"                 },
        {  "-",            "PTD15",     "SPI2_PCS1",         "",          "-",           "-",         "FB_A23",        "FXIO0_D31"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7               PORT E
        {  "-",            "PTE0",     "SPI1_PCS1",         "LPUART1_TX",          "SDHC0_D1",           "QSPI0A_DATA3",         "I2C1_SDA",        "RTC_CLKOUT"                 },
        {  "-",            "PTE1/LLWU_P0",     "SPI1_SCK",         "LPUART1_RX",          "SDHC0_D0",           "QSPI0A_SCLK",         "I2C1_SCL",        "SPI1_SIN"                 },
        {  "-",            "PTE2/LLWU_P1",     "SPI1_SOUT",         "LPUART1_CTS_b",          "SDHC0_DCLK",           "QSPI0A_DATA0",         "FXIO0_D0",        "SPI1_SCK"                 },
        {  "-",            "PTE3",     "SPI1_PCS2",         "LPUART1_RTS_b",          "SDHC0_CMD",           "QSPI0A_DATA2",         "FXIO0_D1",        "SPI1_SOUT"                 },
        {  "-",            "PTE4/LLWU_P2",     "SPI1_SIN",         "LPUART3_TX",          "SDHC0_D3",           "QSPI0A_DATA1",         "-",        "-"                 },
        {  "-",            "PTE5",     "SPI1_PCS0",         "LPUART3_RX",          "SDHC0_D2",           "QSPI0A_SS0_B",         "FTM3_CH0",        "USB0_SOF_OUT"                 },
        {  "-",            "PTE6/LLWU_P16",     "FXIO0_D12",         "LPUART3_CTS_b",          "I2S0_MCLK",           "QSPI0B_DATA3",         "FTM3_CH1",        "SDHC0_D4"                 },
        {  "-",            "PTE7",     "FXIO0_D13",         "LPUART3_RTS_b",          "I2S0_RXD0",           "QSPI0B_SCLK",         "FTM3_CH2",        "QSPI0A_SS1_B"                 },
        {  "-",            "PTE8",     "I2S0_RXD1",         "FXIO0_D14",          "I2S0_RX_FS",           "QSPI0B_DATA0",         "FTM3_CH3",        "SDHC0_D5"                 },
        {  "-",            "PTE9/LLWU_P17",     "I2S0_TXD1",         "FXIO0_D15",          "I2S0_RX_BCLK",           "QSPI0B_DATA2",         "FTM3_CH4",        "SDHC0_D6"                 },
        {  "-",            "PTE10/LLWU_P18",     "I2C3_SDA",         "FXIO0_D16",          "I2S0_TXD0",           "QSPI0B_DATA1",         "FTM3_CH5",        "SDHC0_D7"                 },
        {  "-",            "PTE11",     "I2C3_SCL",         "FXIO0_D17",          "I2S0_TX_FS",           "QSPI0B_SS0_B",         "FTM3_CH6",        "QSPI0A_DQS"                 },
        {  "-",            "PTE12",     "-",         "LPUART2_TX",          "I2S0_TX_BCLK",           "QSPI0B_DQS",         "FTM3_CH7",        "FXIO0_D2"                 },
        {  "-",            "PTE13",     "-",         "LPUART2_RX",          "I2S1_MCLK",           "QSPI0B_SS1_B",         "SDHC0_CLKIN",        "FXIO0_D3"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "ADC0_SE4a",    "PTE16",     "SPI0_PCS0",         "LPUART2_TX",          "FTM_CLKIN0",           "I2S1_TX_BCLK",         "FTM0_FLT3",        "FXIO0_D4"                 },
        {  "ADC0_SE5a",    "PTE17/LLWU_P19",     "SPI0_SCK",         "LPUART2_RX",          "FTM_CLKIN1",           "I2S1_TX_FS",         "LPTMR0_ALT3",        "FXIO0_D5"                 },
        {  "ADC0_SE6a",    "PTE18/LLWU_P20",     "SPI0_SOUT",         "LPUART2_CTS_b",          "I2C0_SDA",           "I2S1_TXD0",         "-",        "FXIO0_D6"                 },
        {  "ADC0_SE7a",    "PTE19",     "SPI0_SIN",         "LPUART2_RTS_b",          "I2C0_SCL",           "I2S1_TXD1",         "-",        "FXIO0_D7"                 },
        {  "-",            "PTE20",     "-",         "LPUART4_TX",          "SPI3_PCS0",           "I2S1_RXD1",         "-",        "FXIO0_D8"                 },
        {  "-",            "PTE21",     "-",         "LPUART4_RX",          "SPI3_SCK",           "I2S1_RXD0",         "-",        "FXIO0_D9"                 },
        {  "-",            "PTE22",     "I2C2_SDA",         "LPUART4_CTS_b",          "SPI3_SOUT",           "I2S1_RX_FS",         "-",        "FXIO0_D10"                 },
        {  "-",            "PTE23",     "I2C2_SCL",         "LPUART4_RTS_b",          "SPI3_SIN",           "I2S1_RX_BCLK",         "-",        "FXIO0_D11"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    }
};

















































































































































